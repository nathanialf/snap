#!/usr/bin/env python3
"""Sort splat subsegment lines so they are strictly ascending by address.

When you insert `[0xADDR, c, name]` lines into `config/snap.us.yaml` while
matching, it's easy to drop them in the wrong place and trip splat's
"segments out of order" warning. This tool finds any contiguous block of
`      - [0xADDR, ...]` lines whose addresses are not strictly ascending
and (with `--fix`) rewrites the block in sorted order.

Comments, blank lines, and surrounding YAML are preserved — only the
subsegment lines themselves move. Lines outside subsegment blocks (the
top-level `- [0x1000000]` end-of-ROM marker, segment headers, etc.) are
never touched.

    tools/check_yaml_order.py            # report only (exit 1 if dirty)
    tools/check_yaml_order.py --fix      # rewrite in place

Read-only by default. With --fix, only edits config/snap.us.yaml — never
touches src/, asm/, or build/.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_YAML = ROOT / "config" / "snap.us.yaml"

# Subsegment lines have indent of exactly 6 spaces and start with `- [0x`.
# The top-level end marker `  - [0x1000000]` has 2-space indent, so it
# won't match — that's intentional.
SUBSEG_RE = re.compile(r"^      - \[\s*0x([0-9A-Fa-f]+)")


def find_blocks(lines: list[str]) -> list[tuple[int, int]]:
    """Return [(start, end_exclusive), ...] for runs of subsegment lines."""
    blocks: list[tuple[int, int]] = []
    i = 0
    n = len(lines)
    while i < n:
        if SUBSEG_RE.match(lines[i]):
            j = i + 1
            while j < n and SUBSEG_RE.match(lines[j]):
                j += 1
            blocks.append((i, j))
            i = j
        else:
            i += 1
    return blocks


def block_addrs(lines: list[str], start: int, end: int) -> list[int]:
    return [int(SUBSEG_RE.match(lines[k]).group(1), 16)
            for k in range(start, end)]


def diff_blocks(lines: list[str]) -> list[dict]:
    """Return a record per out-of-order block."""
    out = []
    for s, e in find_blocks(lines):
        addrs = block_addrs(lines, s, e)
        if addrs == sorted(addrs):
            continue
        # Index of first descent
        bad = next(i for i in range(1, len(addrs)) if addrs[i] <= addrs[i - 1])
        out.append({
            "start": s,
            "end": e,
            "first_bad_line": s + bad + 1,  # 1-based
            "first_bad_addr": addrs[bad],
            "prev_addr": addrs[bad - 1],
            "size": len(addrs),
        })
    return out


def sort_block(lines: list[str], start: int, end: int) -> None:
    block = lines[start:end]
    block.sort(key=lambda ln: int(SUBSEG_RE.match(ln).group(1), 16))
    # Detect duplicate addresses in the block — likely a real mistake.
    addrs = [int(SUBSEG_RE.match(ln).group(1), 16) for ln in block]
    dupes = sorted({a for a in addrs if addrs.count(a) > 1})
    if dupes:
        sys.stderr.write("warning: duplicate subsegment addresses in block: "
                         + ", ".join(f"{a:#x}" for a in dupes) + "\n")
    lines[start:end] = block


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("yaml", nargs="?", default=str(DEFAULT_YAML),
                   help=f"yaml file (default: {DEFAULT_YAML.relative_to(ROOT)})")
    p.add_argument("--fix", action="store_true",
                   help="rewrite the yaml file with sorted blocks")
    p.add_argument("--quiet", action="store_true",
                   help="suppress the per-block listing")
    args = p.parse_args()

    path = Path(args.yaml)
    text = path.read_text()
    # Keep trailing newline behavior identical to the input.
    has_trailing_nl = text.endswith("\n")
    lines = text.splitlines()

    def display_path(p: Path) -> str:
        try:
            return str(p.relative_to(ROOT))
        except ValueError:
            return str(p)

    bad = diff_blocks(lines)
    if not bad:
        if not args.quiet:
            print(f"# {display_path(path)}: all subsegment blocks are sorted")
        return 0

    if not args.quiet:
        print(f"# {len(bad)} out-of-order block(s) in {display_path(path)}:")
        for b in bad:
            print(f"  block lines {b['start'] + 1}..{b['end']} "
                  f"({b['size']} entries); "
                  f"first descent at line {b['first_bad_line']}: "
                  f"{b['first_bad_addr']:#x} <= prev {b['prev_addr']:#x}")

    if not args.fix:
        return 1

    for b in bad:
        sort_block(lines, b["start"], b["end"])

    new_text = "\n".join(lines) + ("\n" if has_trailing_nl else "")
    path.write_text(new_text)
    print(f"# rewrote {path}: sorted {len(bad)} block(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
