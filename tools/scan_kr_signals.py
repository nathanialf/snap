#!/usr/bin/env python3
"""Scan unmatched functions for K&R-style declaration signals.

The recurring matching headache in this repo: IDO 7.1 K&R-style param
declarations spill incoming arg registers to the *caller's* arg-spill
area (positive sp offsets above the new frame), and those spills are
never reloaded. Every K&R variant has the same backbone — dead spill
of $a0..$a3 to sp+N..sp+N+0xC where N >= frame_size — but the C
recipe that reproduces it differs by sub-shape:

    byte-narrow      char  arg + ((unsigned char *)&arg)[3]   -> lbu+sb
    halfword-narrow  u16   arg                                 -> lhu/lh from sp+offset+2
    struct-by-value  Vec2i v / vec3 etc                        -> dead spill + halfword stores to globals
    f32-promotion    K&R'd f32 wrongly promoted to double      -> ldc1 + cvt.s.d (use ANSI instead)
    live-spill       K&R spill+reload across calls (no $s regs)-> spill, jal, reload from same slot
    plain-dead-spill K&R int with no further narrowing         -> just the spill

This tool walks every still-unmatched function, looks for the spill
backbone, classifies the sub-variant from the surrounding insns, and
prints a histogram. With --list <variant> it dumps the function names
in that bucket so you can pick the next target.

The point of the histogram is *prioritisation*: when six functions
are blocked on the same K&R variant, cracking that variant once
unlocks all six. The variant-counts tell you which IDO quirk is
worth investigating next.

Read-only — only consults asm/ + src/ + yaml. Safe to run during
matching work.

Usage:
    tools/scan_kr_signals.py                       # histogram
    tools/scan_kr_signals.py --list byte-narrow    # show fns in bucket
    tools/scan_kr_signals.py --list all            # show all unmatched K&R candidates
    tools/scan_kr_signals.py --show func_NAME      # explain one function's signals
"""
from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_DIR = ROOT / "asm"
SRC_DIR = ROOT / "src"

ARG_REGS = ("$a0", "$a1", "$a2", "$a3")

FN_RE = re.compile(
    r"^glabel (func_[0-9A-Fa-f]+)\n(.*?)^endlabel \1",
    re.DOTALL | re.MULTILINE,
)

INSN_RE = re.compile(
    r"^\s*/\*\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+\*/\s+(\S+)(?:\s+(.*?))?$"
)

ADDIU_SP_RE = re.compile(r"\$sp\s*,\s*\$sp\s*,\s*(-?0x[0-9A-Fa-f]+|-?\d+)")
SW_SP_RE = re.compile(r"(\$a[0-3])\s*,\s*(0x[0-9A-Fa-f]+|\d+)\(\$sp\)")
LOAD_SP_RE = re.compile(r"(\$\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\(\$sp\)")
ANDI_BYTE_RE = re.compile(r"(\$a[0-3])\s*,\s*\1\s*,\s*0x([0-9A-Fa-f]+)")
SH_GLOBAL_RE = re.compile(r"\$a[0-3]\s*,\s*-?0x[0-9A-Fa-f]+\(\$at\)")


def matched_funcs() -> set[str]:
    out: set[str] = set()
    if not SRC_DIR.is_dir():
        return out
    decl_re = re.compile(r"\b(func_[0-9A-Fa-f]{8})\s*\(")
    for p in SRC_DIR.rglob("*.c"):
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        for m in decl_re.finditer(text):
            out.add(m.group(1))
    return out


def parse_signed_imm(s: str) -> int:
    s = s.strip()
    if s.startswith("-"):
        return -int(s[1:], 0)
    return int(s, 0)


def function_blocks() -> dict[str, list[tuple[str, str, str]]]:
    """Yield (name, [(mnemonic, operands, raw_line), ...])."""
    fns: dict[str, list[tuple[str, str, str]]] = {}
    if not ASM_DIR.is_dir():
        return fns
    for p in sorted(ASM_DIR.rglob("*.s")):
        if "nonmatchings" in p.parts:
            continue
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        for m in FN_RE.finditer(text):
            name = m.group(1)
            body = m.group(2)
            insns: list[tuple[str, str, str]] = []
            for line in body.splitlines():
                im = INSN_RE.match(line)
                if im:
                    mnemonic = im.group(2)
                    operands = (im.group(3) or "").strip()
                    insns.append((mnemonic, operands, line.strip()))
            if insns:
                fns[name] = insns
    return fns


def find_frame_size(insns: list[tuple[str, str, str]]) -> int | None:
    """Locate the prologue addiu $sp,$sp,-N. Return N (positive bytes)."""
    for mnemonic, operands, _ in insns[:6]:
        if mnemonic != "addiu":
            continue
        m = ADDIU_SP_RE.search(operands)
        if not m:
            continue
        n = parse_signed_imm(m.group(1))
        if n < 0:
            return -n
    return None


def collect_arg_spills(insns: list[tuple[str, str, str]],
                       frame_size: int) -> list[tuple[str, int, int]]:
    """Return [(reg, offset, insn_idx)] for every arg-reg spill to the
    caller's spill area (offset >= frame_size).

    The caller's argument-spill area lives at the *original* sp+0..sp+0xC,
    which after the prologue's `addiu $sp,$sp,-N` is at the new sp+N..N+0xC.
    Any `sw $a?, K($sp)` with K >= N is therefore writing to the caller's
    spill area, not the callee's local frame.
    """
    spills: list[tuple[str, int, int]] = []
    for idx, (mnemonic, operands, _) in enumerate(insns):
        if mnemonic == "sw":
            m = SW_SP_RE.search(operands)
            if not m:
                continue
            reg = m.group(1)
            off = parse_signed_imm(m.group(2))
            if off >= frame_size:
                spills.append((reg, off, idx))
    return spills


def collect_sp_loads(insns: list[tuple[str, str, str]]) -> set[int]:
    load_mnemonics = {"lw", "lh", "lhu", "lb", "lbu", "ld", "lwc1", "ldc1"}
    loads: set[int] = set()
    for _, (mnemonic, operands, _) in enumerate(insns):
        if mnemonic in load_mnemonics:
            m = LOAD_SP_RE.search(operands)
            if m:
                loads.add(parse_signed_imm(m.group(2)))
    return loads


def detect_dead_spills(insns: list[tuple[str, str, str]],
                       frame_size: int) -> list[tuple[str, int, int]]:
    spills = collect_arg_spills(insns, frame_size)
    loads_at = collect_sp_loads(insns)
    return [s for s in spills if s[1] not in loads_at]


def detect_live_spills(insns: list[tuple[str, str, str]],
                       frame_size: int) -> list[tuple[str, int, int]]:
    """Spills to caller's spill area that ARE reloaded later (K&R-style
    canonical-home-slot reload across a sub-call)."""
    spills = collect_arg_spills(insns, frame_size)
    loads_at = collect_sp_loads(insns)
    return [s for s in spills if s[1] in loads_at]


def has_callee_saved_spill(insns: list[tuple[str, str, str]]) -> bool:
    """True if the prologue saves any of $s0..$s7 to the stack — the sign
    that ANSI codegen reserved a callee-saved register for cross-call
    preservation. K&R prefers the canonical home slot instead."""
    for mnemonic, operands, _ in insns[:24]:
        if mnemonic != "sw":
            continue
        m = re.search(r"\$s[0-7]\s*,", operands)
        if m and "($sp)" in operands:
            return True
    return False


def detect_byte_narrow(insns: list[tuple[str, str, str]],
                       spilled_regs: set[str]) -> list[str]:
    """Return spilled regs that are masked with `andi $aX, $aX, 0xFF`."""
    found = []
    for mnemonic, operands, _ in insns:
        if mnemonic != "andi":
            continue
        m = ANDI_BYTE_RE.search(operands)
        if not m:
            continue
        reg = m.group(1)
        mask = int(m.group(2), 16)
        if mask == 0xFF and reg in spilled_regs:
            found.append(reg)
    return found


def detect_halfword_narrow(insns: list[tuple[str, str, str]],
                           spilled_regs: set[str]) -> list[str]:
    """andi $aX, $aX, 0xFFFF — halfword unsigned narrow."""
    found = []
    for mnemonic, operands, _ in insns:
        if mnemonic != "andi":
            continue
        m = ANDI_BYTE_RE.search(operands)
        if not m:
            continue
        reg = m.group(1)
        mask = int(m.group(2), 16)
        if mask == 0xFFFF and reg in spilled_regs:
            found.append(reg)
    return found


def detect_struct_by_value(insns: list[tuple[str, str, str]],
                           dead_spills: list[tuple[str, int, int]]) -> bool:
    """Two or more consecutive arg slots dead-spilled at sp+0..sp+0xC,
    plus halfword stores from arg regs to globals."""
    low_offsets = sorted({off for _, off, _ in dead_spills if off < 0x10})
    if len(low_offsets) < 2:
        return False
    has_consecutive = any(b - a == 4 for a, b in zip(low_offsets, low_offsets[1:]))
    if not has_consecutive:
        return False
    for mnemonic, operands, _ in insns:
        if mnemonic in ("sh", "sw") and SH_GLOBAL_RE.search(operands):
            return True
    return False


def detect_f32_promotion(insns: list[tuple[str, str, str]],
                         frame_size: int) -> bool:
    """ldc1 from sp+K with K >= frame_size, plus cvt.s.d — the K&R-promotes-
    f32-to-double anti-pattern. If present, the function should use an ANSI
    prototype, not K&R."""
    has_ldc1_caller = False
    has_cvt_sd = False
    for mnemonic, operands, _ in insns:
        if mnemonic == "ldc1":
            m = re.search(r"\$f\d+\s*,\s*(0x[0-9A-Fa-f]+|\d+)\(\$sp\)", operands)
            if m:
                off = parse_signed_imm(m.group(1))
                if off >= frame_size:
                    has_ldc1_caller = True
        if mnemonic == "cvt.s.d":
            has_cvt_sd = True
    return has_ldc1_caller and has_cvt_sd


def classify(name: str,
             insns: list[tuple[str, str, str]]) -> dict | None:
    frame_size = find_frame_size(insns)
    if frame_size is None or frame_size == 0:
        return None
    dead = detect_dead_spills(insns, frame_size)
    live = detect_live_spills(insns, frame_size)
    if not dead and not live:
        return None

    spilled_regs = {reg for reg, _, _ in dead + live}

    variants: list[str] = []
    if dead and detect_f32_promotion(insns, frame_size):
        variants.append("f32-promotion")
    bn = detect_byte_narrow(insns, spilled_regs)
    if bn:
        variants.append("byte-narrow")
    hn = detect_halfword_narrow(insns, spilled_regs)
    if hn:
        variants.append("halfword-narrow")
    if dead and detect_struct_by_value(insns, dead):
        variants.append("struct-by-value")
    if live and not has_callee_saved_spill(insns):
        variants.append("live-spill")
    if not variants:
        if dead:
            variants.append("plain-dead-spill")
        else:
            return None  # live-spill with $s regs — likely just normal ANSI

    return {
        "name": name,
        "frame_size": frame_size,
        "dead_spills": dead,
        "live_spills": live,
        "variants": variants,
        "byte_regs": bn,
        "halfword_regs": hn,
    }


def cmd_histogram(args: argparse.Namespace) -> int:
    matched = matched_funcs()
    fns = function_blocks()
    buckets: dict[str, list[str]] = defaultdict(list)
    total_seen = 0
    for name, insns in fns.items():
        if name in matched:
            continue
        c = classify(name, insns)
        if c is None:
            continue
        total_seen += 1
        for v in c["variants"]:
            buckets[v].append(name)

    print(f"# {total_seen} unmatched functions show K&R signals "
          f"(dead arg-spill to caller spill area)")
    print(f"# (a function may appear in multiple variants if it combines them)")
    print()
    print(f"  {'variant':<20} {'count':>6}   {'unblocked-by':<60}")
    print(f"  {'-'*20} {'-'*6}   {'-'*60}")
    order = ["byte-narrow", "halfword-narrow", "struct-by-value",
             "f32-promotion", "live-spill", "plain-dead-spill"]
    hint = {
        "byte-narrow": "K&R char arg + ((u8 *)&arg)[3]",
        "halfword-narrow": "K&R u16/s16 arg (no extra cast needed)",
        "struct-by-value": "Vec2i/vec3 by-value arg",
        "f32-promotion": "switch K&R -> ANSI for f32 args",
        "live-spill": "K&R spill+reload across calls (no $sN reserved)",
        "plain-dead-spill": "K&R decl with no further narrowing",
    }
    for v in order:
        names = buckets.get(v, [])
        if not names:
            continue
        print(f"  {v:<20} {len(names):>6}   {hint[v]}")
    extra = [k for k in buckets if k not in order]
    for v in sorted(extra):
        names = buckets[v]
        print(f"  {v:<20} {len(names):>6}")
    print()
    print(f"# tip: tools/scan_kr_signals.py --list <variant> to see fn names")
    return 0


def cmd_list(args: argparse.Namespace) -> int:
    matched = matched_funcs()
    fns = function_blocks()
    bucket = args.list
    rows = []
    for name, insns in fns.items():
        if name in matched:
            continue
        c = classify(name, insns)
        if c is None:
            continue
        if bucket != "all" and bucket not in c["variants"]:
            continue
        rows.append(c)
    rows.sort(key=lambda c: c["name"])
    if not rows:
        print(f"# no unmatched functions in bucket '{bucket}'")
        return 0
    print(f"# {len(rows)} unmatched function(s) in bucket '{bucket}'")
    for c in rows:
        all_spills = c["dead_spills"] + c["live_spills"]
        spills = ", ".join(f"{r}@sp+{off:#x}" for r, off, _ in all_spills)
        variants = ",".join(c["variants"])
        print(f"  {c['name']}  frame={c['frame_size']:#x}  "
              f"variants=[{variants}]  spills=[{spills}]")
    return 0


def cmd_show(args: argparse.Namespace) -> int:
    target = args.show
    if not target.startswith("func_"):
        target = "func_" + target.lstrip("0x").upper()
    fns = function_blocks()
    if target not in fns:
        print(f"# {target} not found in asm/", file=sys.stderr)
        return 1
    insns = fns[target]
    matched = matched_funcs()
    is_matched = target in matched
    frame = find_frame_size(insns)
    print(f"# {target}  matched={is_matched}  frame={frame}")
    if frame is None or frame == 0:
        print("# no frame -- skipping K&R analysis")
        return 0
    dead = detect_dead_spills(insns, frame)
    live = detect_live_spills(insns, frame)
    if not dead and not live:
        print("# no spills to caller spill area -- not a K&R candidate")
        return 0
    print()
    if dead:
        print(f"# dead spills (sp >= frame_size {frame:#x}, never reloaded):")
        for reg, off, idx in dead:
            print(f"  insn[{idx:3d}]  sw  {reg}, {off:#x}($sp)")
    if live:
        print(f"# live spills (sp >= frame_size {frame:#x}, reloaded later):")
        for reg, off, idx in live:
            print(f"  insn[{idx:3d}]  sw  {reg}, {off:#x}($sp)")
        if has_callee_saved_spill(insns):
            print(f"#   (a $sN reg is also saved -- live spill probably "
                  f"unrelated to K&R, just normal arg-preserve)")
    c = classify(target, insns)
    if c is None:
        return 0
    print()
    print(f"# variants detected: {', '.join(c['variants'])}")
    if c["byte_regs"]:
        print(f"#   byte-narrow regs: {', '.join(c['byte_regs'])}")
        print(f"#   recipe: declare arg as `int` (or `char`), use "
              f"`((unsigned char *)&arg)[3]` where it's narrowed")
    if c["halfword_regs"]:
        print(f"#   halfword-narrow regs: {', '.join(c['halfword_regs'])}")
        print(f"#   recipe: declare arg as `u16`/`s16` in K&R block")
    if "struct-by-value" in c["variants"]:
        print(f"#   recipe: declare arg as a small struct passed by value "
              f"(e.g. Vec2i, vec3)")
    if "f32-promotion" in c["variants"]:
        print(f"#   *** ldc1+cvt.s.d present -- DO NOT use K&R for f32 args, "
              f"use ANSI prototype instead")
    return 0


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("--list", metavar="VARIANT",
                   help="list unmatched fns in a bucket: byte-narrow, "
                        "halfword-narrow, struct-by-value, f32-promotion, "
                        "plain-dead-spill, or 'all'")
    p.add_argument("--show", metavar="FUNC",
                   help="explain one function's K&R signals")
    args = p.parse_args()

    if args.show:
        return cmd_show(args)
    if args.list:
        return cmd_list(args)
    return cmd_histogram(args)


if __name__ == "__main__":
    sys.exit(main())
