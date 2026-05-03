#!/usr/bin/env python3
"""Dump implied struct layout from a function's arg-pointer accesses.

For a function still living in asm/, follow the register provenance from
$a0..$a3 through `or/addu rd, rs, $zero` copies, `addiu rd, rs, K` offset
adjustments, and `sw/lw $sp,K` spill-and-reload pairs. Then list every
memory access whose base register traces back to an argN, grouping by
argN and offset:

    arg0: 5 access(es)
      +0x00   4   r     lw            (likely u32 / pointer)
      +0x10   4   w     sw
      +0x14   4   r     lwc1          (f32)
      +0x18   1   r     lbu
      stride hint: pointer advanced by 0x20 across the function

The output is a starter skeleton. It catches direct field reads/writes
(struct->field = X), float members (lwc1/swc1), byte/halfword fields,
and the ptr-advance idiom (sizeof-the-element). It does NOT chase
through loaded pointers (so arg0->next->field shows up as one access at
+0x10, not as a separate field on the chased struct).

Usage:

    tools/extract_struct.py func_80103A80
    tools/extract_struct.py func_80103A80 --asm asm/4680.s   # skip search
    tools/extract_struct.py func_80103A80 --raw              # show every
                                                              # untraced
                                                              # access too

Read-only — only consults asm/. Safe to run during matching work.
"""
from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_DIR = ROOT / "asm"

INSN_RE = re.compile(
    r"^\s*/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+\*/"
    r"\s+(\S+)\s*(.*?)\s*$"
)
GLABEL_RE = re.compile(r"^glabel\s+(\S+)\s*$")
ENDLABEL_RE = re.compile(r"^endlabel\s+(\S+)\s*$")

# Memory-op tables: mnemonic -> (width_bytes, kind, dir)
LOADS = {
    "lb":  (1, "i", "r"),  "lbu": (1, "u", "r"),
    "lh":  (2, "i", "r"),  "lhu": (2, "u", "r"),
    "lw":  (4, "i", "r"),  "lwu": (4, "u", "r"),
    "ld":  (8, "i", "r"),
    "lwc1": (4, "f", "r"), "ldc1": (8, "f", "r"),
    "lwl": (4, "i", "r"),  "lwr": (4, "i", "r"),
}
STORES = {
    "sb": (1, "i", "w"), "sh": (2, "i", "w"),
    "sw": (4, "i", "w"), "sd": (8, "i", "w"),
    "swc1": (4, "f", "w"), "sdc1": (8, "f", "w"),
    "swl": (4, "i", "w"), "swr": (4, "i", "w"),
}
MEM_OPS = {**LOADS, **STORES}

# Operand of mem-op: `$rX, OFFSET($base)`. Offset can be negative or hex.
MEMOP_OPERAND_RE = re.compile(
    r"^\$(\w+)\s*,\s*(-?(?:0x)?[0-9A-Fa-f]+)\s*\(\$(\w+)\)\s*$"
)
# `addiu rd, rs, K`
ADDIU_RE = re.compile(
    r"^\$(\w+)\s*,\s*\$(\w+)\s*,\s*(-?(?:0x)?[0-9A-Fa-f]+)\s*$"
)
# `or rd, rs, rt` and `addu rd, rs, rt`
THREE_REG_RE = re.compile(r"^\$(\w+)\s*,\s*\$(\w+)\s*,\s*\$(\w+)\s*$")
# `move rd, rs`
TWO_REG_RE = re.compile(r"^\$(\w+)\s*,\s*\$(\w+)\s*$")


def parse_int(s: str) -> int:
    s = s.strip()
    if s.startswith(("-0x", "0x", "+0x")):
        return int(s, 16)
    if s.startswith(("-", "+")) and s[1:].isdigit():
        return int(s, 10)
    if s.isdigit():
        return int(s, 10)
    return int(s, 16)


def find_func(name: str) -> tuple[Path, list[str]] | None:
    """Return (asm_path, body_lines) for the first asm file containing NAME."""
    if not ASM_DIR.is_dir():
        return None
    for p in sorted(ASM_DIR.rglob("*.s")):
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        if f"glabel {name}\n" not in text:
            continue
        return p, slice_body(text, name)
    return None


def slice_body(text: str, name: str) -> list[str]:
    lines = text.splitlines()
    start = None
    for i, ln in enumerate(lines):
        m = GLABEL_RE.match(ln)
        if m and m.group(1) == name:
            start = i + 1
            break
    if start is None:
        return []
    body = []
    for ln in lines[start:]:
        if ENDLABEL_RE.match(ln):
            break
        gm = GLABEL_RE.match(ln)
        if gm and gm.group(1) != name:
            break  # next function (some .s files lack endlabel)
        body.append(ln)
    return body


def analyze(body: list[str]):
    """Return (accesses, stride_hints, untraced, insn_count).

    accesses: list of dicts {arg, off, w, kind, dir, mnemonic, line_no}
    stride_hints: list of dicts {arg, delta, line_no}
    untraced: list of dicts {mnemonic, base, off, line_no} — mem ops whose
              base reg had no known provenance (likely a global or chased ptr)
    """
    # provenance: reg -> (arg, off) where the register equals (argptr + off)
    prov: dict[str, tuple[int, int]] = {
        "a0": (0, 0), "a1": (1, 0), "a2": (2, 0), "a3": (3, 0)
    }
    # sp-spill table: stack offset -> (arg, off) the slot currently holds.
    spills: dict[int, tuple[int, int]] = {}

    accesses = []
    stride_hints = []
    untraced = []
    insn_count = 0

    def kill(reg: str):
        prov.pop(reg, None)

    for line_no, raw in enumerate(body, 1):
        m = INSN_RE.match(raw)
        if not m:
            continue
        op = m.group(1)
        ops = m.group(2)
        insn_count += 1

        # --- Memory accesses ---
        if op in MEM_OPS:
            mm = MEMOP_OPERAND_RE.match(ops)
            if not mm:
                continue
            rt, off_s, base = mm.group(1), mm.group(2), mm.group(3)
            try:
                off = parse_int(off_s)
            except ValueError:
                continue
            width, kind, direction = MEM_OPS[op]

            # SP-spill bookkeeping: writes-to-sp from $aN remember the slot.
            if base == "sp" and op == "sw" and rt in ("a0", "a1", "a2", "a3"):
                argn = int(rt[1])
                spills[off] = (argn, 0)
                # The store itself isn't a struct-field access; skip recording.
                continue
            # SP-spill load: pulls argN provenance into rt
            if base == "sp" and op == "lw" and off in spills:
                prov[rt] = spills[off]
                continue

            base_prov = prov.get(base)
            if base_prov is None:
                untraced.append({
                    "mnemonic": op, "base": base, "off": off,
                    "line_no": line_no,
                })
                # Loads still write rt — kill its provenance unless we can
                # carry it (we don't chase loaded pointers in v1).
                if op in LOADS:
                    kill(rt)
                continue

            arg, base_off = base_prov
            accesses.append({
                "arg": arg, "off": base_off + off,
                "w": width, "kind": kind, "dir": direction,
                "mnemonic": op, "line_no": line_no,
            })
            if op in LOADS:
                kill(rt)
            continue

        # --- Provenance-propagating instructions ---
        if op in ("or", "addu"):
            tm = THREE_REG_RE.match(ops)
            if not tm:
                continue
            rd, rs, rt = tm.group(1), tm.group(2), tm.group(3)
            # Treat `or/addu rd, rs, $zero` (or with $zero,rs) as a copy.
            if rt == "zero" and rs in prov:
                prov[rd] = prov[rs]
            elif rs == "zero" and rt in prov:
                prov[rd] = prov[rt]
            else:
                kill(rd)
            continue

        if op == "move":
            tm = TWO_REG_RE.match(ops)
            if not tm:
                kill_target = TWO_REG_RE.match(ops)
                continue
            rd, rs = tm.group(1), tm.group(2)
            if rs in prov:
                prov[rd] = prov[rs]
            else:
                kill(rd)
            continue

        if op == "addiu":
            am = ADDIU_RE.match(ops)
            if not am:
                continue
            rd, rs, k_s = am.group(1), am.group(2), am.group(3)
            try:
                k = parse_int(k_s)
            except ValueError:
                kill(rd)
                continue
            base_prov = prov.get(rs)
            if base_prov is None:
                kill(rd)
                continue
            arg, off = base_prov
            new_off = off + k
            if rd == rs:
                # In-place advance: the same arg pointer now points elsewhere.
                # Record stride hint, then update provenance.
                stride_hints.append(
                    {"arg": arg, "delta": k, "line_no": line_no}
                )
                prov[rd] = (arg, new_off)
            else:
                prov[rd] = (arg, new_off)
            continue

        # Anything else with a destination GPR clobbers it. Conservatively
        # drop provenance for any register named on the LHS.
        # (We can't reliably parse every instruction's destination here, so
        # only kill known categories that obviously write rt/rd.)
        if op in ("lui",):
            tm = re.match(r"^\$(\w+)\s*,", ops)
            if tm:
                kill(tm.group(1))
        elif op in ("andi", "ori", "xori", "sll", "srl", "sra", "slti", "sltiu",
                    "and", "or", "xor", "nor", "sub", "subu", "slt", "sltu",
                    "mflo", "mfhi", "mfc1"):
            tm = re.match(r"^\$(\w+)\s*,", ops)
            if tm:
                kill(tm.group(1))
        # branches, jumps, jr, jal, fp ops, etc. — no GPR provenance impact
        # we care about. (jal does clobber $ra; we never tracked it.)

    return accesses, stride_hints, untraced, insn_count


def fmt_kind(kind: str, width: int) -> str:
    if kind == "f":
        return "f32" if width == 4 else "f64"
    if kind == "u":
        return {1: "u8", 2: "u16", 4: "u32", 8: "u64"}[width]
    if width == 1:
        return "s8/u8"
    if width == 2:
        return "s16/u16"
    if width == 4:
        return "s32/u32 (or ptr)"
    if width == 8:
        return "u64"
    return f"{width}b"


def report(name: str, asm_path: Path, body: list[str], args) -> int:
    accesses, strides, untraced, insn_count = analyze(body)
    rel = asm_path.relative_to(ROOT) if asm_path.is_absolute() else asm_path
    print(f"== {name}  ({rel})")
    print(f"== {insn_count} insns, "
          f"{len(accesses)} traced struct-field access(es), "
          f"{len(untraced)} untraced mem op(s)")
    print()

    grouped: dict[int, list[dict]] = defaultdict(list)
    for a in accesses:
        grouped[a["arg"]].append(a)

    any_arg = False
    for argn in range(4):
        items = grouped.get(argn, [])
        s_items = [s for s in strides if s["arg"] == argn]
        if not items and not s_items:
            continue
        any_arg = True
        print(f"arg{argn} ($a{argn}):  {len(items)} access(es)")
        # Group by offset, merging widths/dirs.
        by_off: dict[int, dict] = {}
        for it in items:
            slot = by_off.setdefault(it["off"], {
                "off": it["off"], "widths": set(), "kinds": set(),
                "dirs": set(), "ops": set(),
            })
            slot["widths"].add(it["w"])
            slot["kinds"].add(it["kind"])
            slot["dirs"].add(it["dir"])
            slot["ops"].add(it["mnemonic"])
        for off in sorted(by_off):
            slot = by_off[off]
            w = max(slot["widths"])  # widest access wins for skeleton
            kind = ("f" if "f" in slot["kinds"] else
                    ("u" if "u" in slot["kinds"] else "i"))
            d = "".join(sorted(slot["dirs"])) or "?"
            print(f"  +{off:#06x}  {w}  {d:<3}  "
                  f"{','.join(sorted(slot['ops'])):<14}  "
                  f"// {fmt_kind(kind, w)}")
        if s_items:
            for s in s_items:
                print(f"  stride hint: pointer advanced by "
                      f"{s['delta']:+#x} (line {s['line_no']})")
        print()

    if not any_arg:
        print("(no arg-pointer accesses traced — function may not take "
              "struct pointer args, or args were spilled and reloaded "
              "through patterns this tool does not recognize)\n")

    if args.raw and untraced:
        print("== untraced mem ops (unknown base provenance):")
        for u in untraced:
            print(f"  line {u['line_no']:>4}: {u['mnemonic']:<5} "
                  f"+{u['off']:#06x}(${u['base']})")
    elif untraced:
        print(f"# {len(untraced)} untraced mem op(s) skipped "
              f"(--raw to list)")

    return 0


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("name", help="function name, e.g. func_80103A80")
    p.add_argument("--asm", help="explicit asm file path (skips search)")
    p.add_argument("--raw", action="store_true",
                   help="list untraced mem ops too")
    args = p.parse_args()

    if args.asm:
        path = Path(args.asm)
        body = slice_body(path.read_text(errors="ignore"), args.name)
        if not body:
            print(f"{args.name} not found in {path}", file=sys.stderr)
            return 1
        return report(args.name, path, body, args)

    found = find_func(args.name)
    if found is None:
        print(f"{args.name} not found in any asm/ file", file=sys.stderr)
        return 1
    path, body = found
    return report(args.name, path, body, args)


if __name__ == "__main__":
    sys.exit(main())
