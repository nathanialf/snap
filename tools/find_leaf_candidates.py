#!/usr/bin/env python3
"""Rank asm/*.s subsegments by ease-of-matching.

Excludes: jal-callers (not pure leaves), handwritten functions, data-only
ranges (.word .word .word ...), and obvious nop-padding regions. Within
the leaf set, ranks by instruction count ascending — short, register-
clean leaves first.
"""
from __future__ import annotations

import argparse
import pathlib
import re
import sys


GLABEL = re.compile(r"^glabel\s+(\S+)", re.MULTILINE)
INSN = re.compile(
    r"^\s*/\*\s*[0-9A-F]+\s+[0-9A-F]+\s+[0-9A-F]+\s*\*/\s*(\S+)",
    re.MULTILINE,
)


def classify(path: pathlib.Path) -> dict | None:
    text = path.read_text()
    if "Handwritten function" in text:
        return None
    if ".word" in text and "glabel" not in text:
        # pure data, no func glabel
        return None
    if "jal" in text:
        return None  # not a leaf

    # Find first glabel
    m = GLABEL.search(text)
    if not m:
        return None
    func_name = m.group(1)

    # Count real instructions (lines with /* ADDR HEX HEX */ <op>)
    insns = []
    for line in text.splitlines():
        mm = INSN.match(line)
        if mm:
            insns.append(mm.group(1))

    if not insns:
        return None

    # Skip pure-nop padding regions
    if all(op == "nop" for op in insns):
        return None

    # Skip func bodies that are 100% nops up to jr (e.g. trampolines we can't
    # easily express as C)
    nontrivial = [op for op in insns if op != "nop"]
    if not nontrivial:
        return None

    has_branch = any(op in {"beq", "bne", "beqz", "bnez", "blez", "bgez",
                            "bltz", "bgtz", "b", "j"} for op in insns)
    has_globals = "%hi(" in text
    has_float = any(op.endswith(".s") or op.endswith(".d") or
                    op.startswith("mtc1") or op.startswith("mfc1") or
                    op.startswith("lwc1") or op.startswith("swc1")
                    for op in insns)

    return {
        "path": str(path),
        "func": func_name,
        "addr": path.stem,
        "insn_count": len(insns),
        "has_branch": has_branch,
        "has_globals": has_globals,
        "has_float": has_float,
    }


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--limit", type=int, default=50)
    p.add_argument("--max-insns", type=int, default=12,
                   help="cap instruction count for 'easy' tier")
    p.add_argument("--no-branch", action="store_true",
                   help="exclude functions with branches (cleanest leaves)")
    p.add_argument("--asm-dir", default="asm")
    args = p.parse_args()

    candidates = []
    for sf in sorted(pathlib.Path(args.asm_dir).glob("*.s")):
        if sf.name in {"header.s"}:
            continue
        info = classify(sf)
        if info is None:
            continue
        if info["insn_count"] > args.max_insns:
            continue
        if args.no_branch and info["has_branch"]:
            continue
        candidates.append(info)

    candidates.sort(key=lambda c: (c["insn_count"], c["has_branch"], c["addr"]))

    print(f"# {len(candidates)} candidates (insns<={args.max_insns}"
          + (", no branches" if args.no_branch else "") + ")")
    print(f"# {'addr':<8} {'func':<22} {'n':>3} br fp gl")
    for c in candidates[: args.limit]:
        flags = (
            ("Y" if c["has_branch"] else "."),
            ("Y" if c["has_float"] else "."),
            ("Y" if c["has_globals"] else "."),
        )
        print(f"  {c['addr']:<8} {c['func']:<22} {c['insn_count']:>3}  "
              + " ".join(flags))
    return 0


if __name__ == "__main__":
    sys.exit(main())
