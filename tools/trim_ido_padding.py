#!/usr/bin/env python3
"""Resize the .text section of an IDO-emitted ELF to its splat subsegment size.

IDO 7.1 pads `.text` to a 16-byte multiple. The original ROM doesn't always end
a function on a 16-byte boundary, so blindly accepting IDO's padding shifts
neighbouring .o files in the linker output. This tool:

  1. Parses `config/snap.us.yaml` to find the C subsegment that this `.o` is
     emitted from (matched by basename), and the next subsegment's start
     address — the difference is the byte-exact size the linker should see.
  2. Resizes the ELF's `.text` section to that size, dropping or zero-padding
     trailing bytes accordingly.
  3. Rewrites file offsets of any sections that came after `.text` so the
     resulting ELF is well-formed and `mips-linux-gnu-ld` reads it without
     complaining.

Usage: trim_ido_padding.py <obj>
"""
from __future__ import annotations

import re
import struct
import sys
from pathlib import Path


SUBSEG_RE = re.compile(r"-\s*\[\s*(0x[0-9A-Fa-f]+)\s*,\s*([a-zA-Z]+)(?:\s*,\s*([\w]+))?\s*\]")
ROOT = Path(__file__).resolve().parent.parent


def read_u32(b: bytes, off: int) -> int:
    return struct.unpack(">I", b[off:off+4])[0]


def write_u32(buf: bytearray, off: int, v: int) -> None:
    buf[off:off+4] = struct.pack(">I", v)


def find_subseg_size(c_basename: str) -> int | None:
    """Return next_addr - this_addr for the c subsegment named c_basename, or None."""
    yaml_path = ROOT / "config" / "snap.us.yaml"
    if not yaml_path.is_file():
        return None
    text = yaml_path.read_text()
    entries = SUBSEG_RE.findall(text)
    for i, (addr, kind, name) in enumerate(entries):
        if kind == "c" and name == c_basename:
            this_addr = int(addr, 16)
            if i + 1 >= len(entries):
                return None
            next_addr = int(entries[i+1][0], 16)
            return next_addr - this_addr
    return None


def resize_text(in_data: bytes, target_size: int) -> bytes:
    data = bytearray(in_data)
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 2:
        return bytes(data)

    e_shoff = read_u32(data, 0x20)
    e_shentsize = struct.unpack(">H", data[0x2E:0x30])[0]
    e_shnum = struct.unpack(">H", data[0x30:0x32])[0]
    e_shstrndx = struct.unpack(">H", data[0x32:0x34])[0]

    shstr_off = read_u32(data, e_shoff + e_shstrndx*e_shentsize + 0x10)

    text_idx = None
    for i in range(e_shnum):
        sh = e_shoff + i*e_shentsize
        sh_name = read_u32(data, sh)
        end = data.index(b"\x00", shstr_off + sh_name)
        name = data[shstr_off + sh_name:end].decode()
        if name == ".text":
            text_idx = i
            break
    if text_idx is None:
        return bytes(data)

    sh = e_shoff + text_idx*e_shentsize
    sh_offset = read_u32(data, sh + 0x10)
    sh_size = read_u32(data, sh + 0x14)

    if target_size == sh_size:
        return bytes(data)

    delta = target_size - sh_size  # +ve = pad, -ve = trim

    if delta > 0:
        # Insert zero bytes at end of .text
        new_buf = bytearray(
            bytes(data[:sh_offset + sh_size])
            + b"\x00" * delta
            + bytes(data[sh_offset + sh_size:])
        )
    else:
        # Drop bytes from end of .text
        drop = -delta
        end_text = sh_offset + sh_size
        new_buf = bytearray(
            bytes(data[:sh_offset + target_size])
            + bytes(data[end_text:])
        )

    # New ELF positions: any section/header table that lived after .text moves
    # by +delta. Compute the new e_shoff first so we know where to write
    # section headers in `new_buf`.
    new_shoff = e_shoff + delta if e_shoff > sh_offset else e_shoff
    # Write new e_shoff
    write_u32(new_buf, 0x20, new_shoff)
    # Update .text size at its (possibly shifted) section header
    new_sh = new_shoff + text_idx*e_shentsize
    write_u32(new_buf, new_sh + 0x14, target_size)
    # Walk every section header and update offsets that came after .text
    for i in range(e_shnum):
        if i == text_idx:
            continue
        cur_sh = new_shoff + i*e_shentsize
        off_i = read_u32(new_buf, cur_sh + 0x10)
        if off_i > sh_offset:
            write_u32(new_buf, cur_sh + 0x10, off_i + delta)

    return bytes(new_buf)


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__, file=sys.stderr)
        return 1
    in_path = Path(sys.argv[1])
    expected_size = find_subseg_size(in_path.stem)
    if expected_size is None:
        return 0
    in_data = in_path.read_bytes()
    out_data = resize_text(in_data, expected_size)
    if out_data != in_data:
        in_path.write_bytes(out_data)
    return 0


if __name__ == "__main__":
    sys.exit(main())
