#!/usr/bin/env python3
"""Diff a built ROM against the baserom at one or more function offsets.

Intended for tight matching loops: after `make`, run

    tools/match_check.py 0x3DCB0:0xC 0x391B0:0xC ...

to see per-function MATCH/DIFF with side-by-side hex.
"""
from __future__ import annotations

import argparse
import pathlib
import sys


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--base", default="baserom.us.z64")
    p.add_argument("--built", default="build/pokemonsnap.us.z64")
    p.add_argument(
        "-v",
        "--full",
        action="store_true",
        help="Print every word, not just differing ones.",
    )
    p.add_argument(
        "ranges",
        nargs="+",
        help="addr:size pairs, e.g. 0x391B0:0xC. Size may be omitted (defaults"
             " to 0x20).",
    )
    args = p.parse_args()

    base = pathlib.Path(args.base).read_bytes()
    built = pathlib.Path(args.built).read_bytes()

    any_diff = False
    for spec in args.ranges:
        if ":" in spec:
            addr_s, size_s = spec.split(":", 1)
            size = int(size_s, 0)
        else:
            addr_s, size = spec, 0x20
        addr = int(addr_s, 0)
        a = base[addr : addr + size]
        b = built[addr : addr + size]
        if a == b:
            print(f"  MATCH  {addr:#08x} +{size:#x}")
            continue
        any_diff = True
        print(f"  DIFF   {addr:#08x} +{size:#x}")
        # Word-by-word
        total_words = 0
        diff_words = 0
        for i in range(0, size, 4):
            wa = a[i : i + 4]
            wb = b[i : i + 4]
            differs = wa != wb
            total_words += 1
            if differs:
                diff_words += 1
            if args.full or differs:
                mark = "  " if not differs else "<>"
                print(
                    f"    {addr + i:08x}  base {wa.hex():<10}  "
                    f"built {wb.hex():<10} {mark}"
                )
        if not args.full:
            print(f"    {diff_words}/{total_words} differ")
    return 1 if any_diff else 0


if __name__ == "__main__":
    sys.exit(main())
