#!/usr/bin/env python3
"""Classify near-match diffs against the baserom into known buckets.

For each instruction word that differs between baserom and the rebuilt
ROM, decode both via rabbitizer and bucket the diff so you can reach for
the right matching trick instead of staring at hex:

    regalloc      — same opcode + immediates, different register fields
    frame-size    — addiu $sp,$sp,N differs in N
    fp-commute    — mul.s/add.s/sub.s/div.s with fs/ft swapped
    fpu-pipeline  — one side has a `nop`, the other has an FPU op
    immediate     — same opcode + regs, different imm (typical %hi/%lo
                    retargeting or struct-field offset miscount)
    opcode        — different opcode entirely (the deepest hole)

Usage:

    tools/diagnose_match.py func_80111090
    tools/diagnose_match.py 0x4680:0xC4
    tools/diagnose_match.py func_80111090 --base baserom.us.z64 \\
                                          --built build/pokemonsnap.us.z64

Read-only — only consults baserom + built ROM + yaml. Does not invoke
the build, so run `make` first if you want the latest built ROM.
"""
from __future__ import annotations

import argparse
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
YAML = ROOT / "config" / "snap.us.yaml"

SUBSEG_RE = re.compile(
    r"^\s*-\s*\[\s*0x([0-9A-Fa-f]+)\s*,\s*([a-z]+)(?:\s*,\s*([A-Za-z0-9_]+))?"
)

FP_BINOPS = {"mul.s", "add.s", "sub.s", "div.s",
             "mul.d", "add.d", "sub.d", "div.d"}


def venv_rabbitizer():
    try:
        import rabbitizer  # type: ignore
        return rabbitizer
    except ImportError:
        sys.stderr.write(
            "rabbitizer not installed. Run from the project venv:\n"
            "  source .venv/bin/activate && tools/diagnose_match.py ...\n"
        )
        sys.exit(2)


def lookup_func(name: str) -> tuple[int, int]:
    """Return (rom_offset, size_bytes) for a yaml subsegment named NAME."""
    subsegs = []
    for line in YAML.read_text().splitlines():
        m = SUBSEG_RE.match(line)
        if not m:
            continue
        subsegs.append((int(m.group(1), 16), m.group(2), m.group(3)))
    for i, (addr, _kind, sname) in enumerate(subsegs):
        if sname == name:
            if i + 1 >= len(subsegs):
                raise SystemExit(f"{name} is the last subsegment; no size.")
            return addr, subsegs[i + 1][0] - addr
    raise SystemExit(f"{name} not found as a subsegment in {YAML}")


def parse_range(spec: str) -> tuple[int, int]:
    if spec.startswith("func_"):
        return lookup_func(spec)
    if ":" not in spec:
        raise SystemExit("range must be ADDR:SIZE or func_NAME")
    a, s = spec.split(":", 1)
    return int(a, 0), int(s, 0)


def opcode_of(rabbitizer, w: int) -> str:
    return rabbitizer.Instruction(w).getOpcodeName()


def gpr_fields(rabbitizer, w: int) -> tuple:
    """Tuple of register-field values present on the insn (best-effort)."""
    i = rabbitizer.Instruction(w)
    out = []
    for attr in ("rs", "rt", "rd", "fs", "ft", "fd", "sa"):
        try:
            v = getattr(i, attr)
        except RuntimeError:
            continue
        out.append((attr, str(v)))
    return tuple(out)


def imm_of(rabbitizer, w: int):
    try:
        return rabbitizer.Instruction(w).getProcessedImmediate()
    except Exception:
        return None


def is_addiu_sp(rabbitizer, w: int) -> bool:
    i = rabbitizer.Instruction(w)
    if i.getOpcodeName() != "addiu":
        return False
    try:
        return "sp" in str(i.rs) and "sp" in str(i.rt)
    except RuntimeError:
        return False


def disasm(rabbitizer, w: int) -> str:
    if w == 0:
        return "nop"
    return rabbitizer.Instruction(w).disassemble()


def classify(rabbitizer, base_w: int, built_w: int) -> tuple[str, str]:
    """Classify a single (base, built) word pair. Returns (bucket, detail)."""
    if base_w == built_w:
        return ("equal", "")

    # NOP-vs-not is the loudest single signal: usually FPU pipeline padding.
    if base_w == 0 or built_w == 0:
        live = built_w if base_w == 0 else base_w
        side = "built" if base_w == 0 else "base"
        op = opcode_of(rabbitizer, live)
        if op.endswith(".s") or op.endswith(".d") or op in (
            "mtc1", "mfc1", "lwc1", "swc1", "ldc1", "sdc1", "cvt", "trunc",
        ) or op.startswith("cvt.") or op.startswith("trunc."):
            return ("fpu-pipeline", f"{side} has nop where other has {op}")
        return ("nop-shift", f"{side} has nop, other has {op}")

    base_op = opcode_of(rabbitizer, base_w)
    built_op = opcode_of(rabbitizer, built_w)
    if base_op != built_op:
        return ("opcode", f"{base_op} -> {built_op}")

    base_imm = imm_of(rabbitizer, base_w)
    built_imm = imm_of(rabbitizer, built_w)
    base_regs = gpr_fields(rabbitizer, base_w)
    built_regs = gpr_fields(rabbitizer, built_w)

    # Frame-size: addiu sp, sp, K
    if is_addiu_sp(rabbitizer, base_w) and is_addiu_sp(rabbitizer, built_w):
        return (
            "frame-size",
            f"{base_imm} -> {built_imm} (delta {built_imm - base_imm:+d})",
        )

    # FP binop with operands swapped
    if base_op in FP_BINOPS and built_op == base_op:
        rd_b = dict(base_regs)
        rd_u = dict(built_regs)
        if (rd_b.get("fd") == rd_u.get("fd")
                and rd_b.get("fs") == rd_u.get("ft")
                and rd_b.get("ft") == rd_u.get("fs")
                and rd_b.get("fs") != rd_b.get("ft")):
            return (
                "fp-commute",
                f"{base_op}: fs/ft swapped "
                f"({rd_b.get('fs')},{rd_b.get('ft')}) <-> "
                f"({rd_u.get('fs')},{rd_u.get('ft')})",
            )

    if base_imm == built_imm and base_regs != built_regs:
        diffs = []
        rd_b = dict(base_regs)
        rd_u = dict(built_regs)
        for k in rd_b:
            if rd_b[k] != rd_u.get(k):
                diffs.append(f"{k}:{rd_b[k]}->{rd_u[k]}")
        return ("regalloc", f"{base_op} {' '.join(diffs)}")

    if base_regs == built_regs and base_imm != built_imm:
        delta = (built_imm - base_imm) if (base_imm is not None
                                           and built_imm is not None) else None
        msg = f"{base_op} imm {base_imm} -> {built_imm}"
        if delta is not None:
            msg += f" (delta {delta:+d} = {delta:#x})"
        return ("immediate", msg)

    # Mixed regs+imm change — usually a different lui/addiu target entirely.
    return ("other", f"{base_op} regs+imm both differ")


def detect_swaps(diffs: list[dict]) -> None:
    """Annotate adjacent (i, i+1) diffs that are pure swaps as delay-slot."""
    by_idx = {d["idx"]: d for d in diffs}
    for d in diffs:
        i = d["idx"]
        n = by_idx.get(i + 1)
        if not n:
            continue
        if d["base_w"] == n["built_w"] and d["built_w"] == n["base_w"]:
            d["bucket"] = "delay-slot-swap"
            d["detail"] = "this insn and next insn are swapped"
            n["bucket"] = "delay-slot-swap"
            n["detail"] = "swap pair (previous insn)"


def read_words(path: Path, off: int, size: int) -> list[int]:
    raw = path.read_bytes()[off:off + size]
    if len(raw) < size:
        raise SystemExit(f"{path} is too short for {off:#x}+{size:#x}")
    return list(struct.unpack(f">{size // 4}I", raw))


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("target", help="func_NAME or ROMOFFSET:SIZE")
    p.add_argument("--base", default="baserom.us.z64")
    p.add_argument("--built", default="build/pokemonsnap.us.z64")
    p.add_argument("--vram", type=int, default=0x80100400 - 0x1000,
                   help="ROM->VRAM offset (default: snap main segment)")
    p.add_argument("--all", action="store_true",
                   help="show every word (matching ones too)")
    args = p.parse_args()

    rabbitizer = venv_rabbitizer()
    rom_off, size = parse_range(args.target)
    if size <= 0 or size % 4:
        raise SystemExit(f"bad size {size:#x} (must be a positive multiple of 4)")

    base_path = Path(args.base)
    built_path = Path(args.built)
    if not base_path.exists():
        raise SystemExit(f"baserom missing: {base_path}")
    if not built_path.exists():
        raise SystemExit(f"built rom missing: {built_path} — run `make` first")

    base_words = read_words(base_path, rom_off, size)
    built_words = read_words(built_path, rom_off, size)

    label = args.target if args.target.startswith("func_") else \
        f"{rom_off:#x}+{size:#x}"
    print(f"== {label}  rom {rom_off:#x}  size {size:#x}  "
          f"({size // 4} insns)")

    diffs = []
    for i, (bw, uw) in enumerate(zip(base_words, built_words)):
        bucket, detail = classify(rabbitizer, bw, uw)
        if bucket == "equal" and not args.all:
            continue
        diffs.append({
            "idx": i,
            "rom": rom_off + i * 4,
            "vram": rom_off + i * 4 + args.vram,
            "base_w": bw,
            "built_w": uw,
            "bucket": bucket,
            "detail": detail,
        })

    # Multi-insn pattern: pure adjacent swaps.
    detect_swaps(diffs)

    diff_count = sum(1 for d in diffs if d["bucket"] != "equal")
    if diff_count == 0:
        print("== MATCH (0 differing words)")
        return 0
    print(f"== {diff_count} differing word(s)\n")

    for d in diffs:
        if d["bucket"] == "equal":
            continue
        print(f"  +{(d['idx'] * 4):#06x}  rom {d['rom']:#08x}  "
              f"vram {d['vram']:#010x}")
        print(f"    base  {d['base_w']:08X}  {disasm(rabbitizer, d['base_w'])}")
        print(f"    built {d['built_w']:08X}  {disasm(rabbitizer, d['built_w'])}")
        print(f"    bucket: {d['bucket']}")
        if d["detail"]:
            print(f"    detail: {d['detail']}")
        print()

    # Bucket totals — gives a one-glance "what's the dominant problem".
    totals: dict[str, int] = {}
    for d in diffs:
        if d["bucket"] == "equal":
            continue
        totals[d["bucket"]] = totals.get(d["bucket"], 0) + 1
    print("== bucket totals:")
    for k, v in sorted(totals.items(), key=lambda kv: -kv[1]):
        print(f"     {k:<18} {v}")
    return 0 if diff_count == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
