#!/usr/bin/env python3
"""Find sibling functions: groups whose mnemonic sequences are identical.

A "sibling cluster" is a set of functions that share an exact instruction
opcode sequence (ignoring registers, immediates, and label offsets). Two
functions in the same cluster typically differ only in which globals
they touch or which constants they use — meaning if you've matched one,
the others almost always come for free with the same C template.

The tool's most valuable mode is --templates: it shows clusters that
contain at least one matched and one unmatched function, ranked so the
biggest payoff is first. Each entry tells you "function X is matched;
write copies of that C for these N other functions, swapping globals."

Read-only — never touches yaml, src/, or build/. Safe to run during
matching work.

Usage:
    tools/find_siblings.py                    # all clusters of size >= 2
    tools/find_siblings.py --templates        # ready-to-port templates
    tools/find_siblings.py --unmatched        # crack-one-unlock-N families
    tools/find_siblings.py --near             # 1-2 edits from a matched fn
    tools/find_siblings.py --near --distance 3
    tools/find_siblings.py --show func_X      # inspect cluster of X
    tools/find_siblings.py --templates -k 20  # cap rows
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

# A function block: glabel <name>\n ... endlabel <name>
FN_RE = re.compile(
    r"^glabel (func_[0-9A-Fa-f]+)\n(.*?)^endlabel \1",
    re.DOTALL | re.MULTILINE,
)

# A real instruction line: /* ADDR HEX HEX */ MNEMONIC ...
INSN_RE = re.compile(
    r"^\s*/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+\*/\s+(\S+)"
)


def matched_funcs() -> set[str]:
    """Return {func_NAME} for every function defined in src/*.c."""
    matched: set[str] = set()
    if not SRC_DIR.is_dir():
        return matched
    decl_re = re.compile(r"\b(func_[0-9A-Fa-f]{8})\s*\(")
    for p in SRC_DIR.rglob("*.c"):
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        # Only count names that appear as a definition (preceded by a type
        # token at start of line). Cheap heuristic: any occurrence of
        # "func_NNNNNNNN(" preceded by something that ends with a non-paren
        # token. To stay safe, just collect all references and intersect
        # with what we see in asm.
        for m in decl_re.finditer(text):
            matched.add(m.group(1))
    return matched


def fingerprint(body: str) -> tuple[str, ...]:
    """Mnemonic-only fingerprint, ignoring registers and immediates."""
    out = []
    for line in body.splitlines():
        m = INSN_RE.match(line)
        if m:
            out.append(m.group(1))
    return tuple(out)


def is_handwritten(body: str) -> bool:
    return "handwritten instruction" in body


def collect() -> dict[tuple[str, ...], list[str]]:
    """Return {fingerprint: [func_name, ...]}, deduped by name."""
    seen: dict[str, tuple[str, ...]] = {}
    if not ASM_DIR.is_dir():
        return {}
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
            if is_handwritten(body):
                continue
            fp = fingerprint(body)
            if not fp:
                continue
            if all(op == "nop" for op in fp):
                continue
            seen[name] = fp  # last-wins; same name in two .s should be identical
    clusters: dict[tuple[str, ...], list[str]] = defaultdict(list)
    for name, fp in seen.items():
        clusters[fp].append(name)
    for names in clusters.values():
        names.sort()
    return clusters


def cluster_summary(fp: tuple[str, ...]) -> str:
    n = len(fp)
    head = ",".join(fp[:8])
    tail = "..." if n > 8 else ""
    return f"[{n} insns] {head}{tail}"


def cmd_all(args: argparse.Namespace) -> int:
    clusters = collect()
    matched = matched_funcs()

    rows = []
    for fp, names in clusters.items():
        if len(names) < args.min_size:
            continue
        if len(fp) < args.min_insns:
            continue
        n_matched = sum(1 for n in names if n in matched)
        rows.append((len(names), n_matched, fp, names))

    rows.sort(key=lambda r: (-r[0], -r[1], r[3][0]))

    limit = args.k or len(rows)
    print(f"# {len(rows)} clusters of size >= {args.min_size} "
          f"(showing top {limit})")
    for size, n_matched, fp, names in rows[:limit]:
        print()
        print(f"=== size={size} matched={n_matched}/{size}  "
              f"{cluster_summary(fp)}")
        for name in names:
            mark = "+" if name in matched else " "
            print(f"  {mark} {name}")
    return 0


def cmd_unmatched(args: argparse.Namespace) -> int:
    """Print fully-unmatched clusters — 'crack one, unlock N' families."""
    clusters = collect()
    matched = matched_funcs()

    rows = []
    for fp, names in clusters.items():
        if len(names) < args.min_size:
            continue
        if len(fp) < args.min_insns:
            continue
        if any(n in matched for n in names):
            continue
        rows.append((len(names), len(fp), fp, names))

    rows.sort(key=lambda r: (-r[0], -r[1]))

    limit = args.k or len(rows)
    if not rows:
        print("# no fully-unmatched clusters found")
        return 0

    print(f"# {len(rows)} fully-unmatched sibling clusters "
          f"(showing top {limit}); each match unlocks the rest")
    for size, n_insns, fp, names in rows[:limit]:
        print()
        print(f"=== payoff={size}x  {n_insns}-insn shape")
        print(f"    members: {', '.join(names)}")
        print(f"    shape: {cluster_summary(fp)}")
    return 0


def cmd_templates(args: argparse.Namespace) -> int:
    """Print clusters with mixed matched/unmatched — copy-paste targets."""
    clusters = collect()
    matched = matched_funcs()

    rows = []
    for fp, names in clusters.items():
        if len(fp) < args.min_insns:
            continue
        n_matched = sum(1 for n in names if n in matched)
        n_unmatched = len(names) - n_matched
        if n_matched == 0 or n_unmatched == 0:
            continue
        # Score: more unmatched siblings = bigger payoff; ties broken by
        # matched count (higher = more confidence the template is solid).
        rows.append((n_unmatched, n_matched, len(fp), fp, names))

    rows.sort(key=lambda r: (-r[0], -r[1], -r[2]))

    limit = args.k or len(rows)
    if not rows:
        print("# no mixed clusters found")
        return 0

    print(f"# {len(rows)} sibling templates (matched fn + unmatched fns "
          f"with same shape) — showing top {limit}")
    for n_unmatched, n_matched, n_insns, fp, names in rows[:limit]:
        matched_names = [n for n in names if n in matched]
        unmatched_names = [n for n in names if n not in matched]
        print()
        print(f"=== payoff={n_unmatched} unmatched, template from "
              f"{n_matched} matched, {n_insns}-insn shape")
        print(f"    template:  {', '.join(matched_names)}")
        print(f"    apply to:  {', '.join(unmatched_names)}")
        print(f"    shape: {cluster_summary(fp)}")
    return 0


def edit_distance(a: tuple[str, ...], b: tuple[str, ...], cap: int) -> int:
    """Levenshtein on mnemonic sequences, returning early if >cap."""
    if abs(len(a) - len(b)) > cap:
        return cap + 1
    n, m = len(a), len(b)
    if n == 0:
        return m
    if m == 0:
        return n
    prev = list(range(m + 1))
    for i in range(1, n + 1):
        cur = [i] + [0] * m
        row_min = cur[0]
        for j in range(1, m + 1):
            ins = cur[j - 1] + 1
            dele = prev[j] + 1
            sub = prev[j - 1] + (0 if a[i - 1] == b[j - 1] else 1)
            cur[j] = min(ins, dele, sub)
            row_min = min(row_min, cur[j])
        if row_min > cap:
            return cap + 1
        prev = cur
    return prev[m]


def cmd_near(args: argparse.Namespace) -> int:
    """Find unmatched fns within edit distance D of any matched fn's shape."""
    clusters = collect()
    matched = matched_funcs()

    matched_fps: dict[tuple[str, ...], list[str]] = defaultdict(list)
    unmatched_fps: dict[tuple[str, ...], list[str]] = defaultdict(list)
    for fp, names in clusters.items():
        if len(fp) < args.min_insns:
            continue
        for n in names:
            if n in matched:
                matched_fps[fp].append(n)
            else:
                unmatched_fps[fp].append(n)

    cap = args.distance
    rows = []
    for u_fp, u_names in unmatched_fps.items():
        if u_fp in matched_fps:
            continue  # exact match — handled by --templates
        best = (cap + 1, None, None)
        for m_fp, m_names in matched_fps.items():
            d = edit_distance(u_fp, m_fp, cap)
            if d <= cap and d < best[0]:
                best = (d, m_fp, m_names)
        if best[1] is not None:
            rows.append((best[0], len(u_names), len(u_fp), u_fp, u_names,
                         best[1], best[2]))

    rows.sort(key=lambda r: (r[0], -r[1], -r[2]))

    limit = args.k or len(rows)
    if not rows:
        print(f"# no unmatched clusters within edit distance {cap}")
        return 0

    print(f"# {len(rows)} unmatched clusters within {cap} edits of a "
          f"matched fn (showing top {limit})")
    for d, size, n_insns, u_fp, u_names, m_fp, m_names in rows[:limit]:
        print()
        print(f"=== edits={d} payoff={size}x  {n_insns}-insn unmatched")
        print(f"    template:  {', '.join(m_names[:3])}"
              f"{' ...' if len(m_names) > 3 else ''}")
        print(f"    apply to:  {', '.join(u_names)}")
        print(f"    template shape:  {cluster_summary(m_fp)}")
        print(f"    unmatched shape: {cluster_summary(u_fp)}")
    return 0


def cmd_show(args: argparse.Namespace) -> int:
    target = args.show
    if not target.startswith("func_"):
        target = "func_" + target.lstrip("0x").upper()
    clusters = collect()
    matched = matched_funcs()
    for fp, names in clusters.items():
        if target in names:
            print(f"# cluster of {len(names)} containing {target}")
            print(f"# shape: {cluster_summary(fp)}")
            for name in names:
                mark = "+" if name in matched else " "
                print(f"  {mark} {name}")
            print()
            print("# full mnemonic sequence:")
            for i, op in enumerate(fp):
                print(f"  {i:3d}  {op}")
            return 0
    print(f"# {target} not found in any cluster of size >= 2", file=sys.stderr)
    return 1


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("--templates", action="store_true",
                   help="only show clusters with both matched and unmatched "
                        "members (copy-paste-template targets)")
    p.add_argument("--unmatched", action="store_true",
                   help="only show fully-unmatched clusters "
                        "(crack-one-unlock-N families)")
    p.add_argument("--near", action="store_true",
                   help="show unmatched clusters within --distance edits "
                        "of a matched template")
    p.add_argument("--distance", type=int, default=2,
                   help="max edit distance for --near (default 2)")
    p.add_argument("--show", metavar="FUNC",
                   help="show the cluster containing FUNC and its full "
                        "mnemonic sequence")
    p.add_argument("--min-size", type=int, default=2,
                   help="ignore clusters smaller than N (default 2)")
    p.add_argument("--min-insns", type=int, default=4,
                   help="ignore functions with fewer than N instructions "
                        "(default 4 — filters out trivial getters that "
                        "cluster too widely)")
    p.add_argument("-k", type=int, default=0,
                   help="show only the top K clusters (0 = all)")
    args = p.parse_args()

    if args.show:
        return cmd_show(args)
    if args.templates:
        return cmd_templates(args)
    if args.unmatched:
        return cmd_unmatched(args)
    if args.near:
        return cmd_near(args)
    return cmd_all(args)


if __name__ == "__main__":
    sys.exit(main())
