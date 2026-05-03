#!/usr/bin/env python3
"""Cross-reference D_8XXXXXXX globals across matched src/ and unmatched asm/.

Lists every D_-style global symbol found in the project and, for each one:

    matched_c   — number of matched .c files in src/ that name it
    asm_files   — number of unmatched .s files in asm/ that name it
    matched_n   — total occurrences in src/
    asm_n       — total occurrences in asm/

This is the "subsystem detector": globals touched by many functions tend
to be subsystem state (camera struct, current Pokemon, audio bank
pointer). Globals named in both matched and unmatched code are bridges —
naming the global lets you propose a semantic file name for the asm
holding its other consumers.

    tools/global_xref.py                    # default: by total refs desc
    tools/global_xref.py --by bridge        # both matched + unmatched
    tools/global_xref.py --by asm-only      # globals only in asm so far
    tools/global_xref.py --addr 0x80048    # filter by address prefix
    tools/global_xref.py --show D_80048734  # list every reference site

Read-only — never touches yaml, src/, or build/.
"""
from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC_DIR = ROOT / "src"
ASM_DIR = ROOT / "asm"

# D_ symbols are emitted by spimdisasm with addresses in the 0x80000000+ range.
# Match exactly 8 hex digits to avoid trailing-identifier overruns.
SYM_RE = re.compile(r"\bD_[0-9A-Fa-f]{8}\b")


def scan(path: Path) -> dict[str, int]:
    out: dict[str, int] = defaultdict(int)
    try:
        text = path.read_text(errors="ignore")
    except OSError:
        return out
    for m in SYM_RE.finditer(text):
        # Normalize hex case so D_80048ABC and D_80048abc collapse.
        out[m.group(0).upper()] += 1
    return out


def collect():
    """Return (per_sym, examples).

    per_sym[name] = {
        'matched_files': set[Path], 'matched_n': int,
        'asm_files': set[Path],     'asm_n': int,
    }
    examples[name] = list[(side, path, lineno, line_text)] (capped)
    """
    per_sym: dict[str, dict] = defaultdict(
        lambda: {"matched_files": set(), "matched_n": 0,
                 "asm_files": set(), "asm_n": 0}
    )

    for p in sorted(SRC_DIR.rglob("*.c")) if SRC_DIR.is_dir() else []:
        counts = scan(p)
        for name, n in counts.items():
            entry = per_sym[name]
            entry["matched_files"].add(p)
            entry["matched_n"] += n

    if ASM_DIR.is_dir():
        for p in sorted(ASM_DIR.rglob("*.s")):
            counts = scan(p)
            for name, n in counts.items():
                entry = per_sym[name]
                entry["asm_files"].add(p)
                entry["asm_n"] += n

    return per_sym


def normalize(s: str) -> str:
    return s.upper()


def cmd_show(args, per_sym):
    target = normalize(args.show)
    entry = per_sym.get(target)
    if not entry:
        print(f"# {target}: no references", file=sys.stderr)
        return 1
    print(f"# {target}")
    print(f"#   matched src files: {len(entry['matched_files'])}, "
          f"{entry['matched_n']} refs")
    print(f"#   asm files:         {len(entry['asm_files'])}, "
          f"{entry['asm_n']} refs")

    pat = re.compile(re.escape(target), re.IGNORECASE)
    for side, files in (("src", entry["matched_files"]),
                        ("asm", entry["asm_files"])):
        for f in sorted(files):
            try:
                text = f.read_text(errors="ignore")
            except OSError:
                continue
            for ln, line in enumerate(text.splitlines(), 1):
                if pat.search(line):
                    rel = f.relative_to(ROOT)
                    print(f"  {side}  {rel}:{ln}  {line.strip()[:100]}")
    return 0


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("--by", choices=("total", "matched", "asm", "bridge",
                                    "asm-only", "matched-only", "addr"),
                   default="total",
                   help="sort/filter mode")
    p.add_argument("--limit", type=int, default=40,
                   help="show top N (0 = all)")
    p.add_argument("--addr", default="",
                   help="prefix filter (e.g. 0x80048 or 80048)")
    p.add_argument("--show", metavar="NAME",
                   help="list every reference site for NAME")
    args = p.parse_args()

    per_sym = collect()
    if args.show:
        return cmd_show(args, per_sym)

    rows = []
    addr_prefix = args.addr.lstrip("0x").upper()
    for name, e in per_sym.items():
        if addr_prefix and not name[2:].startswith(addr_prefix):
            continue
        m_files = len(e["matched_files"])
        a_files = len(e["asm_files"])
        if args.by == "bridge" and not (m_files and a_files):
            continue
        if args.by == "asm-only" and m_files:
            continue
        if args.by == "matched-only" and a_files:
            continue
        rows.append((name, m_files, e["matched_n"], a_files, e["asm_n"]))

    if args.by == "matched":
        rows.sort(key=lambda r: (-r[2], -r[1], r[0]))
    elif args.by == "asm":
        rows.sort(key=lambda r: (-r[4], -r[3], r[0]))
    elif args.by == "bridge":
        rows.sort(key=lambda r: (-(r[1] + r[3]), -(r[2] + r[4]), r[0]))
    elif args.by == "addr":
        rows.sort(key=lambda r: r[0])
    else:  # total / asm-only / matched-only fall through to total ordering
        rows.sort(key=lambda r: (-(r[2] + r[4]), -(r[1] + r[3]), r[0]))

    print(f"# {len(rows)} D_-globals "
          + (f"matching prefix {args.addr}" if addr_prefix else "")
          + (f", filter={args.by}" if args.by in
             {"bridge", "asm-only", "matched-only"} else ""))
    print(f"# {'symbol':<14} {'src_f':>6} {'src_n':>6} "
          f"{'asm_f':>6} {'asm_n':>6}")
    limit = args.limit or len(rows)
    for name, m_files, m_n, a_files, a_n in rows[:limit]:
        print(f"  {name:<14} {m_files:>6} {m_n:>6} {a_files:>6} {a_n:>6}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
