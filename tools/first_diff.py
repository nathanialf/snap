#!/usr/bin/env python3
"""
first_diff.py — symbolic ROM-mismatch diagnosis for the Pokémon Snap decomp.

When `make` reports "MISMATCH: expected SHA-1 X got Y", the rebuilt ROM
diverged somewhere from the baserom. This tool walks both ROMs four bytes
at a time, locates each differing word in the linker mapfile, and prints
the containing function, the byte offset within that function, and a
side-by-side disassembly of the differing instructions.

Output format (one line per differing instruction):

    <FUNC>+<OFFSET>:  base= <DISASM>     built= <DISASM>

For jal / j instructions the absolute target VRAM is resolved through the
mapfile so the disassembly shows the destination function's actual name,
which is the usual signal you want when chasing a single-instruction diff.

Defaults:
    --base   baserom.us.z64
    --built  build/pokemonsnap.us.z64
    --map    build/pokemonsnap.us.map

Tooling-only — does not read or modify any tracked C / asm / yaml.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

try:
    import rabbitizer
    from mapfile_parser import mapfile
except ImportError as exc:  # pragma: no cover
    sys.exit(
        f"first_diff.py: missing dependency ({exc.name}). "
        "Run `.venv/bin/pip install -r tools/requirements.txt`."
    )


REPO_ROOT = Path(__file__).resolve().parent.parent

DEFAULT_BASE = REPO_ROOT / "baserom.us.z64"
DEFAULT_BUILT = REPO_ROOT / "build" / "pokemonsnap.us.z64"
DEFAULT_MAP = REPO_ROOT / "build" / "pokemonsnap.us.map"


def _word_be(buf: bytes) -> int:
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]


def _disasm_with_symbols(
    word_bytes: bytes,
    vram: int,
    map_file: mapfile.MapFile,
) -> str:
    """Disassemble a 4-byte instruction, swapping jal/j targets for symbol names."""
    instr = rabbitizer.Instruction(_word_be(word_bytes), vram)
    if instr.isJumpWithAddress():
        target = instr.getInstrIndexAsVram()
        found = map_file.findSymbolByVramOrVrom(target)
        if found is not None:
            return instr.disassemble(found.symbol.name)
    return instr.disassemble()


def _resolve_function(
    map_file: mapfile.MapFile,
    rom_offset: int,
) -> tuple[str, int, int] | None:
    """Return (function_name, byte_offset_in_function, vram_of_instruction)."""
    found, _possible_files = map_file.findSymbolByVrom(rom_offset)
    if found is None:
        return None
    sym = found.symbol
    # `offset` in FoundSymbolInfo is the byte offset from sym.vrom to rom_offset.
    instr_vram = sym.vram + found.offset
    return sym.name, found.offset, instr_vram


def first_diff(
    base_path: Path,
    built_path: Path,
    map_path: Path,
    diff_count: int,
) -> int:
    for label, p in (("baserom", base_path), ("built", built_path), ("map", map_path)):
        if not p.exists():
            print(f"first_diff: {label} not found at {p}", file=sys.stderr)
            return 2

    base = base_path.read_bytes()
    built = built_path.read_bytes()

    if base == built:
        print("first_diff: ROMs are identical.")
        return 0

    if len(base) != len(built):
        print(
            f"first_diff: size mismatch — base=0x{len(base):X} built=0x{len(built):X}; "
            "comparing common prefix.",
            file=sys.stderr,
        )

    map_file = mapfile.MapFile.newFromMapFile(map_path)

    end = min(len(base), len(built))
    # Skip the 0x40-byte ROM header — splat/linker do not produce instructions there
    # and the mapfile has no symbols for that range, so reporting diffs is noise.
    start = 0x40
    found_any = 0
    for off in range(start, end - (end % 4), 4):
        if base[off : off + 4] == built[off : off + 4]:
            continue
        resolved = _resolve_function(map_file, off)
        if resolved is None:
            print(f"0x{off:08X} (no symbol):  base={base[off:off+4].hex()}  built={built[off:off+4].hex()}")
        else:
            func, in_func_off, vram = resolved
            base_disasm = _disasm_with_symbols(base[off : off + 4], vram, map_file)
            built_disasm = _disasm_with_symbols(built[off : off + 4], vram, map_file)
            print(
                f"{func}+0x{in_func_off:X}:  base= {base_disasm}     built= {built_disasm}"
            )
        found_any += 1
        if found_any >= diff_count:
            break

    if found_any == 0:
        print("first_diff: ROMs differ but no 4-byte word diffs in scanned range.")
        return 1
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Locate the first instruction-level ROM divergences "
        "between the baserom and the rebuilt ROM, with mapfile-resolved "
        "function names and jal targets.",
    )
    parser.add_argument(
        "-n",
        type=int,
        default=5,
        metavar="N",
        help="print up to N differing instructions (default: 5)",
    )
    parser.add_argument(
        "--base",
        type=Path,
        default=DEFAULT_BASE,
        help=f"path to baserom (default: {DEFAULT_BASE.relative_to(REPO_ROOT)})",
    )
    parser.add_argument(
        "--built",
        type=Path,
        default=DEFAULT_BUILT,
        help=f"path to rebuilt ROM (default: {DEFAULT_BUILT.relative_to(REPO_ROOT)})",
    )
    parser.add_argument(
        "--map",
        type=Path,
        default=DEFAULT_MAP,
        dest="map_path",
        help=f"path to linker mapfile (default: {DEFAULT_MAP.relative_to(REPO_ROOT)})",
    )
    args = parser.parse_args(argv)
    return first_diff(args.base, args.built, args.map_path, args.n)


if __name__ == "__main__":
    sys.exit(main())
