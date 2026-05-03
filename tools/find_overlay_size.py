#!/usr/bin/env python3
"""Rank `[0xADDR, asm]` ranges in snap.us.yaml by size and function count.

Each `[0xADDR, asm]` line is a contiguous block of unmatched code. The
block's byte size is `next_subseg.addr - this.addr`. The function count
comes from `glabel` lines in `asm/<ADDR>.s` (the .s file splat emits for
this subsegment).

Use this to pick which asm wall to crack open next:

    tools/find_overlay_size.py                    # default: top 30 by bytes
    tools/find_overlay_size.py --by funcs         # rank by function count
    tools/find_overlay_size.py --by density       # avg bytes/func ascending
    tools/find_overlay_size.py --limit 0          # show all
    tools/find_overlay_size.py --max-funcs 1      # only single-func ranges
                                                  # (the cheapest entry points)

Read-only — never touches yaml, src/, or build/.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
YAML = ROOT / "config" / "snap.us.yaml"
ASM_DIR = ROOT / "asm"

SUBSEG_RE = re.compile(
    r"^\s*-\s*\[\s*0x([0-9A-Fa-f]+)\s*,\s*([a-z]+)(?:\s*,\s*([A-Za-z0-9_]+))?"
)
GLABEL_RE = re.compile(r"^glabel\s+(func_[0-9A-Fa-f]+)", re.MULTILINE)


def parse_subsegments() -> list[tuple[int, str, str | None]]:
    """Return [(addr, kind, name|None), ...] in file order."""
    out = []
    for line in YAML.read_text().splitlines():
        m = SUBSEG_RE.match(line)
        if not m:
            continue
        # Skip the bare end marker `- [0x1000000]` at segment-list indent: it
        # has no second field. The regex requires `, kind`, so it won't match.
        addr = int(m.group(1), 16)
        kind = m.group(2)
        name = m.group(3)
        out.append((addr, kind, name))
    return out


def count_functions(asm_path: Path) -> int:
    if not asm_path.is_file():
        return 0
    return len(GLABEL_RE.findall(asm_path.read_text(errors="ignore")))


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("--by", choices=("bytes", "funcs", "density", "addr"),
                   default="bytes",
                   help="sort key: bytes desc (default), funcs desc, density "
                        "asc (small fns first), or addr asc")
    p.add_argument("--limit", type=int, default=30,
                   help="top N rows (0 = all)")
    p.add_argument("--max-funcs", type=int, default=0,
                   help="filter to ranges with at most N functions "
                        "(0 = no limit)")
    p.add_argument("--min-bytes", type=int, default=0,
                   help="filter to ranges of at least N bytes")
    args = p.parse_args()

    segs = parse_subsegments()
    if not segs:
        print("no subsegments parsed — is config/snap.us.yaml present?",
              file=sys.stderr)
        return 1

    rows = []
    total_asm = 0
    for i, (addr, kind, _name) in enumerate(segs):
        if kind != "asm":
            continue
        if i + 1 >= len(segs):
            # Last subsegment with no successor — can't compute size.
            continue
        next_addr = segs[i + 1][0]
        size = next_addr - addr
        if size <= 0:
            continue
        # splat names the .s file by the subsegment's ROM offset, hex upper.
        asm_path = ASM_DIR / f"{addr:X}.s"
        nfn = count_functions(asm_path)
        rows.append({
            "addr": addr,
            "size": size,
            "funcs": nfn,
            "density": (size / nfn) if nfn else size,
            "asm_path": asm_path,
        })
        total_asm += size

    if args.max_funcs:
        rows = [r for r in rows if r["funcs"] <= args.max_funcs]
    if args.min_bytes:
        rows = [r for r in rows if r["size"] >= args.min_bytes]

    if args.by == "bytes":
        rows.sort(key=lambda r: (-r["size"], r["addr"]))
    elif args.by == "funcs":
        rows.sort(key=lambda r: (-r["funcs"], -r["size"], r["addr"]))
    elif args.by == "density":
        # bytes/func ascending: cheap fns first
        rows.sort(key=lambda r: (r["density"], r["addr"]))
    else:  # addr
        rows.sort(key=lambda r: r["addr"])

    limit = args.limit or len(rows)
    print(f"# {len(rows)} `asm` ranges, {total_asm:,} bytes total "
          f"(~{total_asm / 1024:.1f} KiB)")
    print(f"# sorted by {args.by}; showing top {limit}")
    print(f"# {'addr':<10} {'size':>8} {'%':>5} {'funcs':>5} {'b/fn':>6}  asm")
    for r in rows[:limit]:
        share = (100.0 * r["size"] / total_asm) if total_asm else 0
        bpf = (r["size"] / r["funcs"]) if r["funcs"] else 0
        rel = r["asm_path"].relative_to(ROOT) if r["asm_path"].exists() \
            else f"(missing) {r['asm_path'].name}"
        print(f"  0x{r['addr']:<8X} {r['size']:>8} {share:>4.1f}% "
              f"{r['funcs']:>5} {bpf:>6.0f}  {rel}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
