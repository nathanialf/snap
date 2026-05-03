#!/usr/bin/env python3
"""Diff two function objects and bucket the differences by root cause.

Two distinct uses, one tool:

  baserom-vs-built mode:  diff_objs.py func_NAME
      Compare baserom vs the rebuilt ROM for one function — same as
      `diagnose_match.py` but with a tighter prologue-only summary
      and a single-line "root cause" verdict so you don't have to
      stare at hex.

  twin-function mode:     diff_objs.py func_A func_B
      Compare two functions' baserom bytes against each other —
      useful when find_siblings.py says "these have the same shape"
      and you want a targeted view of *just* the diffs to decide if
      it's worth porting the C body across.

Diff buckets (richer than diagnose_match.py — focused on prologue):

    frame-size        addiu $sp,$sp,N differs in N
    sN-spill          sw $sX,K($sp) appears in one side, not the other
                      (the matched side is reserving a callee-saved reg
                      for cross-call preservation; the other isn't)
    dead-spill        sw $aX,K($sp) with K >= frame_size, never reloaded
                      — appears on one side only (K&R declarations)
    regalloc          same opcode + immediates, different register fields
    fp-commute        FP binop with fs/ft swapped
    scheduler-pick    adjacent insns swapped (delay-slot / FPU pipeline)
    immediate         same opcode + regs, different imm
    opcode            different opcode
    nop-shift         one side has nop, the other a real insn

The verdict at the bottom maps the dominant bucket(s) to a knob:
    sN-spill / dead-spill present  -> "try K&R declarations"
    regalloc only                  -> "register-pressure — try
                                        rearranging local var lifetimes"
    scheduler-pick only            -> "scheduler — try a different
                                        statement order"
    frame-size + sN-spill          -> "callee-save cost different — see
                                        which side has more $s spills"

Read-only — only consults baserom + built ROM + yaml. Works
without `make` for twin-function mode (no built ROM needed).
"""
from __future__ import annotations

import argparse
import re
import struct
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
YAML = ROOT / "config" / "snap.us.yaml"

SUBSEG_RE = re.compile(
    r"^\s*-\s*\[\s*0x([0-9A-Fa-f]+)\s*,\s*([a-z]+)(?:\s*,\s*([A-Za-z0-9_]+))?"
)

FP_BINOPS = {"mul.s", "add.s", "sub.s", "div.s",
             "mul.d", "add.d", "sub.d", "div.d"}

ARG_REGS = {"a0", "a1", "a2", "a3"}
S_REGS = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"}


def venv_rabbitizer():
    try:
        import rabbitizer  # type: ignore
        return rabbitizer
    except ImportError:
        sys.stderr.write(
            "rabbitizer not installed. Run from the project venv:\n"
            "  source .venv/bin/activate && tools/diff_objs.py ...\n"
        )
        sys.exit(2)


def parse_subsegs() -> list[tuple[int, str, str | None]]:
    subsegs = []
    for line in YAML.read_text().splitlines():
        m = SUBSEG_RE.match(line)
        if m:
            subsegs.append((int(m.group(1), 16), m.group(2), m.group(3)))
    return subsegs


def lookup_func(name: str) -> tuple[int, int]:
    subsegs = parse_subsegs()
    for i, (addr, _kind, sname) in enumerate(subsegs):
        if sname == name:
            if i + 1 >= len(subsegs):
                raise SystemExit(f"{name} is the last subsegment; no size.")
            return addr, subsegs[i + 1][0] - addr
    # Fallback: scan asm/*.s for a glabel <name> ... endlabel <name> block
    # and derive (rom_offset, size) from the first/last instruction comment.
    asm_dir = ROOT / "asm"
    fn_re = re.compile(
        rf"^glabel {re.escape(name)}\n(.*?)^endlabel {re.escape(name)}",
        re.DOTALL | re.MULTILINE,
    )
    insn_re = re.compile(
        r"^\s*/\*\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+\*/"
    )
    for p in sorted(asm_dir.rglob("*.s")):
        if "nonmatchings" in p.parts:
            continue
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        m = fn_re.search(text)
        if not m:
            continue
        body = m.group(1)
        offsets: list[int] = []
        for line in body.splitlines():
            im = insn_re.match(line)
            if im:
                offsets.append(int(im.group(1), 16))
        if offsets:
            return offsets[0], (offsets[-1] + 4) - offsets[0]
    raise SystemExit(
        f"{name} not found as a yaml subsegment or in asm/*.s"
    )


def parse_target(spec: str) -> tuple[int, int]:
    if spec.startswith("func_"):
        return lookup_func(spec)
    if ":" not in spec:
        raise SystemExit("range must be ADDR:SIZE or func_NAME")
    a, s = spec.split(":", 1)
    return int(a, 0), int(s, 0)


def read_words(path: Path, off: int, size: int) -> list[int]:
    raw = path.read_bytes()[off:off + size]
    if len(raw) < size:
        raise SystemExit(f"{path} too short for {off:#x}+{size:#x}")
    return list(struct.unpack(f">{size // 4}I", raw))


def opcode_of(rab, w: int) -> str:
    if w == 0:
        return "nop"
    return rab.Instruction(w).getOpcodeName()


def reg_fields(rab, w: int) -> dict[str, str]:
    out = {}
    if w == 0:
        return out
    i = rab.Instruction(w)
    for attr in ("rs", "rt", "rd", "fs", "ft", "fd", "sa"):
        try:
            out[attr] = str(getattr(i, attr))
        except RuntimeError:
            pass
    return out


def imm_of(rab, w: int):
    if w == 0:
        return None
    try:
        return rab.Instruction(w).getProcessedImmediate()
    except Exception:
        return None


def disasm(rab, w: int) -> str:
    if w == 0:
        return "nop"
    return rab.Instruction(w).disassemble()


def is_addiu_sp(rab, w: int) -> bool:
    if w == 0:
        return False
    i = rab.Instruction(w)
    if i.getOpcodeName() != "addiu":
        return False
    try:
        return "sp" in str(i.rs) and "sp" in str(i.rt)
    except RuntimeError:
        return False


def is_sw_to_sp(rab, w: int) -> tuple[str, int] | None:
    """Return (src_reg_short, sp_offset) for `sw $X, K($sp)`, else None."""
    if w == 0:
        return None
    i = rab.Instruction(w)
    if i.getOpcodeName() != "sw":
        return None
    try:
        if "sp" not in str(i.rs):
            return None
        src = str(i.rt).lstrip("$")
        off = i.getProcessedImmediate()
        return src, off
    except (RuntimeError, Exception):
        return None


def find_frame_size(rab, words: list[int]) -> int | None:
    for w in words[:6]:
        if is_addiu_sp(rab, w):
            imm = imm_of(rab, w)
            if imm is not None and imm < 0:
                return -imm
    return None


def collect_loads_at_sp(rab, words: list[int]) -> set[int]:
    """SP-relative loads."""
    loads: set[int] = set()
    load_ops = {"lw", "lh", "lhu", "lb", "lbu", "ld", "lwc1", "ldc1"}
    for w in words:
        if w == 0:
            continue
        i = rab.Instruction(w)
        if i.getOpcodeName() not in load_ops:
            continue
        try:
            if "sp" not in str(i.rs):
                continue
            loads.add(i.getProcessedImmediate())
        except (RuntimeError, Exception):
            continue
    return loads


def classify_word_pair(rab, a_w: int, b_w: int) -> tuple[str, str]:
    if a_w == b_w:
        return ("equal", "")

    if a_w == 0 or b_w == 0:
        live = b_w if a_w == 0 else a_w
        side = "B" if a_w == 0 else "A"
        op = opcode_of(rab, live)
        return ("nop-shift", f"{side} has {op} where other has nop")

    a_op = opcode_of(rab, a_w)
    b_op = opcode_of(rab, b_w)
    if a_op != b_op:
        return ("opcode", f"{a_op} -> {b_op}")

    if a_op in ("jal", "j"):
        a_target = (a_w & 0x03FFFFFF) << 2
        b_target = (b_w & 0x03FFFFFF) << 2
        return ("callee-swap",
                f"{a_op} target {a_target:#08x} -> {b_target:#08x}")

    a_imm = imm_of(rab, a_w)
    b_imm = imm_of(rab, b_w)
    a_regs = reg_fields(rab, a_w)
    b_regs = reg_fields(rab, b_w)

    if is_addiu_sp(rab, a_w) and is_addiu_sp(rab, b_w):
        return ("frame-size",
                f"{a_imm} -> {b_imm} (delta {b_imm - a_imm:+d})")

    if a_op in FP_BINOPS and b_op == a_op:
        if (a_regs.get("fd") == b_regs.get("fd")
                and a_regs.get("fs") == b_regs.get("ft")
                and a_regs.get("ft") == b_regs.get("fs")
                and a_regs.get("fs") != a_regs.get("ft")):
            return ("fp-commute",
                    f"fs/ft swapped: ({a_regs['fs']},{a_regs['ft']}) <-> "
                    f"({b_regs['fs']},{b_regs['ft']})")

    if a_imm == b_imm and a_regs != b_regs:
        diffs = []
        for k in a_regs:
            if a_regs[k] != b_regs.get(k):
                diffs.append(f"{k}:{a_regs[k]}->{b_regs[k]}")
        return ("regalloc", f"{a_op} {' '.join(diffs)}")

    if a_regs == b_regs and a_imm != b_imm:
        delta = (b_imm - a_imm) if (a_imm is not None
                                    and b_imm is not None) else None
        msg = f"{a_op} imm {a_imm} -> {b_imm}"
        if delta is not None:
            msg += f" (delta {delta:+d})"
        return ("immediate", msg)

    return ("other", f"{a_op} regs+imm both differ")


def detect_swap_and_spill_buckets(rab, a_words: list[int],
                                  b_words: list[int],
                                  diffs: list[dict]) -> None:
    """Promote certain diffs to richer buckets (sN-spill, dead-spill,
    scheduler-pick) using cross-insn context."""
    by_idx = {d["idx"]: d for d in diffs}
    for d in diffs:
        i = d["idx"]
        n = by_idx.get(i + 1)
        if not n:
            continue
        if d["a_w"] == n["b_w"] and d["b_w"] == n["a_w"]:
            d["bucket"] = "scheduler-pick"
            d["detail"] = "this insn and next are swapped"
            n["bucket"] = "scheduler-pick"
            n["detail"] = "swap pair (previous insn)"

    a_frame = find_frame_size(rab, a_words) or 0
    b_frame = find_frame_size(rab, b_words) or 0
    a_loads = collect_loads_at_sp(rab, a_words)
    b_loads = collect_loads_at_sp(rab, b_words)

    for d in diffs:
        if d["bucket"] not in ("nop-shift", "opcode", "other"):
            continue
        a_sw = is_sw_to_sp(rab, d["a_w"])
        b_sw = is_sw_to_sp(rab, d["b_w"])
        live_sw = a_sw or b_sw
        if not live_sw:
            continue
        # Only one side spills here.
        spilling_side = "A" if a_sw and not b_sw else (
            "B" if b_sw and not a_sw else None)
        if spilling_side is None:
            continue
        src, off = live_sw
        if src in S_REGS:
            d["bucket"] = "sN-spill"
            d["detail"] = (f"side {spilling_side} reserves $s{src[1]} "
                           f"(callee-saved) — other side spills the same "
                           f"value to canonical home slot or never spills")
        elif src in ARG_REGS:
            frame = a_frame if spilling_side == "A" else b_frame
            loads_here = a_loads if spilling_side == "A" else b_loads
            if frame and off >= frame and off not in loads_here:
                d["bucket"] = "dead-spill"
                d["detail"] = (f"side {spilling_side} dead-spills ${src} "
                               f"to caller spill area sp+{off:#x} (K&R "
                               f"signature signal)")


def render_diffs(rab, diffs: list[dict], rom_off_a: int,
                 rom_off_b: int | None, label_a: str, label_b: str) -> None:
    for d in diffs:
        if d["bucket"] == "equal":
            continue
        a_addr = rom_off_a + d["idx"] * 4
        b_addr = (rom_off_b or rom_off_a) + d["idx"] * 4
        print(f"  +{(d['idx'] * 4):#06x}")
        print(f"    {label_a:<5} {a_addr:#08x}  {d['a_w']:08X}  "
              f"{disasm(rab, d['a_w'])}")
        print(f"    {label_b:<5} {b_addr:#08x}  {d['b_w']:08X}  "
              f"{disasm(rab, d['b_w'])}")
        print(f"    bucket: {d['bucket']}")
        if d["detail"]:
            print(f"    detail: {d['detail']}")
        print()


def verdict(buckets: Counter) -> str:
    nontrivial = {k: v for k, v in buckets.items() if k != "equal"}
    if not nontrivial:
        return "MATCH (no differences)"

    has_kr = any(b in nontrivial for b in ("sN-spill", "dead-spill"))
    only_regalloc = set(nontrivial) <= {"regalloc"}
    only_sched = set(nontrivial) <= {"scheduler-pick"}
    only_immediate = set(nontrivial) <= {"immediate"}
    only_frame = set(nontrivial) <= {"frame-size"}

    only_callee = set(nontrivial) <= {"callee-swap"}

    if has_kr:
        return ("K&R-style declaration suspected — try K&R-style "
                "param decls (and check tools/scan_kr_signals.py "
                "--show)")
    if only_callee:
        return ("callee-swap only — these are siblings; port the "
                "donor's C body via tools/find_matched_neighbors.py")
    if only_regalloc:
        return ("regalloc only — try rearranging local var lifetimes "
                "(extra temp, or splitting an expression)")
    if only_sched:
        return ("scheduler differences only — try moving an "
                "independent statement to break the dependency chain")
    if only_immediate:
        return ("immediate-only differences — likely a wrong constant, "
                "wrong struct offset, or wrong global; check the "
                "delta(s) above")
    if only_frame:
        return ("frame-size only — local var sizes/types differ; "
                "look for missing temp or wrong-sized struct")
    if "fp-commute" in nontrivial:
        return ("FP commutativity — try reordering the multiply/add "
                "operands in the C source")
    return "mixed — see bucket totals below"


def cmd_two(args, rab) -> int:
    addr_a, size_a = parse_target(args.target)
    addr_b, size_b = parse_target(args.second)
    if size_a != size_b:
        sys.stderr.write(
            f"warn: sizes differ ({size_a:#x} vs {size_b:#x}); "
            f"truncating to min\n"
        )
    n = min(size_a, size_b)
    if n <= 0 or n % 4:
        raise SystemExit(f"bad size {n:#x}")

    base_path = Path(args.base)
    if not base_path.exists():
        raise SystemExit(f"baserom missing: {base_path}")

    a_words = read_words(base_path, addr_a, n)
    b_words = read_words(base_path, addr_b, n)

    label_a = args.target if args.target.startswith("func_") else \
        f"{addr_a:#x}+{size_a:#x}"
    label_b = args.second if args.second.startswith("func_") else \
        f"{addr_b:#x}+{size_b:#x}"
    print(f"== twin-function diff:  A={label_a}   B={label_b}")
    print(f"== {n // 4} insns each, comparing baserom-A vs baserom-B")

    diffs = []
    for i, (aw, bw) in enumerate(zip(a_words, b_words)):
        bucket, detail = classify_word_pair(rab, aw, bw)
        if bucket == "equal":
            continue
        diffs.append({
            "idx": i, "a_w": aw, "b_w": bw,
            "bucket": bucket, "detail": detail,
        })

    detect_swap_and_spill_buckets(rab, a_words, b_words, diffs)

    print(f"== {len(diffs)} differing word(s)\n")
    render_diffs(rab, diffs, addr_a, addr_b, "A", "B")

    totals = Counter(d["bucket"] for d in diffs)
    print("== bucket totals:")
    for k, v in sorted(totals.items(), key=lambda kv: -kv[1]):
        print(f"     {k:<18} {v}")
    print()
    print(f"== verdict: {verdict(totals)}")
    return 0 if not diffs else 1


def cmd_one(args, rab) -> int:
    addr, size = parse_target(args.target)
    if size <= 0 or size % 4:
        raise SystemExit(f"bad size {size:#x}")

    base_path = Path(args.base)
    built_path = Path(args.built)
    if not base_path.exists():
        raise SystemExit(f"baserom missing: {base_path}")
    if not built_path.exists():
        raise SystemExit(
            f"built rom missing: {built_path} -- run `make` first or pass "
            f"two function names for twin-function mode"
        )

    a_words = read_words(base_path, addr, size)
    b_words = read_words(built_path, addr, size)

    label = args.target if args.target.startswith("func_") else \
        f"{addr:#x}+{size:#x}"
    print(f"== baserom-vs-built:  {label}")
    print(f"== {size // 4} insns")

    diffs = []
    for i, (aw, bw) in enumerate(zip(a_words, b_words)):
        bucket, detail = classify_word_pair(rab, aw, bw)
        if bucket == "equal":
            continue
        diffs.append({
            "idx": i, "a_w": aw, "b_w": bw,
            "bucket": bucket, "detail": detail,
        })

    detect_swap_and_spill_buckets(rab, a_words, b_words, diffs)

    if not diffs:
        print("== MATCH (0 differing words)")
        return 0
    print(f"== {len(diffs)} differing word(s)\n")
    render_diffs(rab, diffs, addr, addr, "base", "built")

    totals = Counter(d["bucket"] for d in diffs)
    print("== bucket totals:")
    for k, v in sorted(totals.items(), key=lambda kv: -kv[1]):
        print(f"     {k:<18} {v}")
    print()
    print(f"== verdict: {verdict(totals)}")
    return 1


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("target", help="func_NAME or ROMOFFSET:SIZE")
    p.add_argument("second", nargs="?",
                   help="if present, second func_NAME / ROMOFFSET:SIZE — "
                        "switches to twin-function mode (baserom A vs "
                        "baserom B); if absent, runs baserom-vs-built")
    p.add_argument("--base", default="baserom.us.z64")
    p.add_argument("--built", default="build/pokemonsnap.us.z64")
    args = p.parse_args()

    rab = venv_rabbitizer()
    if args.second:
        return cmd_two(args, rab)
    return cmd_one(args, rab)


if __name__ == "__main__":
    sys.exit(main())
