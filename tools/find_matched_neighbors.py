#!/usr/bin/env python3
"""Port a matched function's C body onto a sibling unmatched function.

Sibling clusters (see `tools/find_siblings.py --templates`) are groups
of functions with identical mnemonic sequences. The matched member is
the donor; the unmatched members differ only in *which* callees they
jump to and *which* globals/immediates they touch. The C body is the
same shape, just retargeted.

Doing the retarget by hand across 2-3 files in sequence is tedious and
error-prone. This tool walks the donor's asm and the target's asm in
parallel, builds a symbol substitution table by aligning operand
positions, applies the substitutions to the donor's C source, and
emits the candidate C body. The substitution table is printed first so
you can sanity-check it before trusting the output.

Substitutions handled:
    jal func_DONOR -> jal func_TARGET             (callee swap)
    %hi/%lo D_DONOR -> %hi/%lo D_TARGET           (global swap)
    immediate constants that differ are reported  (struct offsets etc)

The tool refuses to swap if the mnemonic sequences don't match — it
expects the two functions to be confirmed siblings already (run
find_siblings.py first).

Read-only by default. With --write FILE it writes the candidate to a
file you specify; it never silently overwrites src/.

Usage:
    tools/find_matched_neighbors.py DONOR TARGET
    tools/find_matched_neighbors.py DONOR TARGET --write src/func_TARGET.c
    tools/find_matched_neighbors.py DONOR TARGET --table-only
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_DIR = ROOT / "asm"
SRC_DIR = ROOT / "src"

FN_RE = re.compile(
    r"^glabel (func_[0-9A-Fa-f]+)\n(.*?)^endlabel \1",
    re.DOTALL | re.MULTILINE,
)
INSN_RE = re.compile(
    r"^\s*/\*\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+\*/\s+(\S+)(?:\s+(.*?))?$"
)
CALLEE_RE = re.compile(r"\b(func_[0-9A-Fa-f]{8})\b")
GLOBAL_RE = re.compile(r"\b(D_[0-9A-Fa-f]{8})\b")
HI_LO_RE = re.compile(r"%(?:hi|lo)\(([A-Za-z_][\w]*)\)")
IMM_HEX_RE = re.compile(r"0x[0-9A-Fa-f]+")
IMM_DEC_RE = re.compile(r"(?<![\w.])(-?\d+)(?![\w.])")


def find_function(name: str) -> tuple[str, list[tuple[str, str]]] | None:
    """Return (raw_body, [(mnemonic, operands), ...]) or None if not found."""
    if not ASM_DIR.is_dir():
        return None
    for p in sorted(ASM_DIR.rglob("*.s")):
        if "nonmatchings" in p.parts:
            continue
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        for m in FN_RE.finditer(text):
            if m.group(1) != name:
                continue
            body = m.group(2)
            insns = []
            for line in body.splitlines():
                im = INSN_RE.match(line)
                if im:
                    insns.append((im.group(2), (im.group(3) or "").strip()))
            if insns:
                return body, insns
    return None


def find_donor_source(name: str) -> Path | None:
    """Locate the .c file that defines a matched donor function."""
    decl_re = re.compile(rf"\b{re.escape(name)}\s*\(")
    if not SRC_DIR.is_dir():
        return None
    for p in SRC_DIR.rglob("*.c"):
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        if decl_re.search(text):
            return p
    return None


def operand_tokens(op: str) -> list[tuple[str, str]]:
    """Tokenise an operand string into (kind, value) pairs in order.

    Kinds: 'callee', 'global', 'hex', 'dec'.

    Hex/dec immediates are picked up so we can report differences even
    though we can't always swap them automatically (an immediate could
    be a struct offset, a literal, or a frame size).
    """
    tokens: list[tuple[str, str]] = []
    seen_spans: list[tuple[int, int]] = []

    def overlaps(span: tuple[int, int]) -> bool:
        for s, e in seen_spans:
            if not (span[1] <= s or span[0] >= e):
                return True
        return False

    for m in CALLEE_RE.finditer(op):
        tokens.append(("callee", m.group(1)))
        seen_spans.append(m.span(1))
    for m in GLOBAL_RE.finditer(op):
        if not overlaps(m.span(1)):
            tokens.append(("global", m.group(1)))
            seen_spans.append(m.span(1))
    for m in HI_LO_RE.finditer(op):
        ident = m.group(1)
        if ident.startswith("D_") or ident.startswith("func_"):
            continue
        if not overlaps(m.span(1)):
            tokens.append(("global", ident))
            seen_spans.append(m.span(1))
    for m in IMM_HEX_RE.finditer(op):
        if not overlaps(m.span()):
            tokens.append(("hex", m.group()))
    for m in IMM_DEC_RE.finditer(op):
        if not overlaps(m.span()):
            tokens.append(("dec", m.group()))
    return tokens


def derive_substitutions(
    donor_insns: list[tuple[str, str]],
    target_insns: list[tuple[str, str]],
) -> tuple[dict[str, str], dict[str, str], list[tuple[int, str, str]]]:
    """Walk both asm in parallel; return (callee_map, global_map, imm_diffs).

    Refuses to align if mnemonic sequences differ.
    """
    if len(donor_insns) != len(target_insns):
        raise SystemExit(
            f"shape mismatch: donor has {len(donor_insns)} insns, "
            f"target has {len(target_insns)}"
        )
    for i, (d, t) in enumerate(zip(donor_insns, target_insns)):
        if d[0] != t[0]:
            raise SystemExit(
                f"shape mismatch at insn {i}: donor='{d[0]}' target='{t[0]}'"
            )

    callee_map: dict[str, str] = {}
    global_map: dict[str, str] = {}
    imm_diffs: list[tuple[int, str, str]] = []

    for i, ((dm, dop), (_tm, top)) in enumerate(zip(donor_insns, target_insns)):
        d_toks = operand_tokens(dop)
        t_toks = operand_tokens(top)
        if len(d_toks) != len(t_toks):
            continue  # punt — operand layout diverged
        for (dk, dv), (tk, tv) in zip(d_toks, t_toks):
            if dk != tk:
                continue
            if dk == "callee":
                _record_swap(callee_map, dv, tv, "callee", i)
            elif dk == "global":
                _record_swap(global_map, dv, tv, "global", i)
            elif dk in ("hex", "dec") and dv != tv:
                imm_diffs.append((i, f"{dm} {dop}", f"{dm} {top}"))
    return callee_map, global_map, imm_diffs


def _record_swap(table: dict[str, str], old: str, new: str,
                 kind: str, insn_idx: int) -> None:
    if old == new:
        return
    if old in table and table[old] != new:
        sys.stderr.write(
            f"warn: insn {insn_idx}: {kind} {old} -> conflicting target "
            f"({table[old]} already, now {new})\n"
        )
        return
    table[old] = new


def apply_subs(text: str, callee_map: dict[str, str],
               global_map: dict[str, str]) -> str:
    """Substitute callee/global names. Only replaces whole-token
    occurrences so we don't mangle prefixes."""
    def sub_callee(m: re.Match) -> str:
        return callee_map.get(m.group(1), m.group(1))

    def sub_global(m: re.Match) -> str:
        return global_map.get(m.group(1), m.group(1))

    text = CALLEE_RE.sub(sub_callee, text)
    text = GLOBAL_RE.sub(sub_global, text)
    return text


def rename_definition(src: str, donor: str, target: str) -> str:
    """Rename donor's defining function to target. Conservative: only
    swaps the literal name; signature, body, etc. are left to apply_subs."""
    return re.sub(rf"\b{re.escape(donor)}\b", target, src)


def extract_donor_block(text: str, donor: str) -> str:
    """Pull a single donor function (with surrounding directives) out of a
    .c file that may contain several siblings.

    Returns: file's `#include`/`typedef`/`extern` preamble + the brace-
    balanced body of the donor's definition. Other function definitions
    in the file are dropped. We over-include typedefs/externs since
    pruning them precisely would need a full parser; unused decls are
    harmless in the candidate."""
    lines = text.splitlines(keepends=True)

    fn_def_re = re.compile(
        r"^\s*[\w\s\*]+\b(func_[0-9A-Fa-f]{8})\s*\("
    )

    fn_starts: list[tuple[int, str]] = []
    for i, line in enumerate(lines):
        m = fn_def_re.match(line)
        if not m:
            continue
        if line.lstrip().startswith(("extern ", "static extern ")):
            continue
        # Definition heuristic: '{' appears within the next few lines AND
        # there's no terminating ';' before that '{'. Declarations end in ';'.
        looked = "".join(lines[i:i + 6])
        brace = looked.find("{")
        semi = looked.find(";")
        if brace == -1:
            continue
        if semi != -1 and semi < brace:
            continue
        fn_starts.append((i, m.group(1)))

    if not fn_starts:
        return text

    donor_start = next((i for i, n in fn_starts if n == donor), None)
    if donor_start is None:
        return text

    other_starts = [i for i, n in fn_starts if n != donor]

    cuts: list[tuple[int, int]] = []
    for j in other_starts:
        end = _function_end(lines, j)
        cuts.append((j, end))
    cuts.sort()

    donor_end = _function_end(lines, donor_start)

    keep_mask = [True] * len(lines)
    for s, e in cuts:
        for k in range(s, e + 1):
            keep_mask[k] = False
    for k in range(donor_start, donor_end + 1):
        keep_mask[k] = True

    return "".join(line for line, keep in zip(lines, keep_mask) if keep)


def dedup_extern_lines(text: str) -> str:
    """Drop duplicate `extern …;` lines that result from substitution
    collapsing two distinct declarations onto the same name."""
    seen: set[str] = set()
    out: list[str] = []
    for line in text.splitlines(keepends=True):
        stripped = line.strip()
        if stripped.startswith("extern ") and stripped.endswith(";"):
            if stripped in seen:
                continue
            seen.add(stripped)
        out.append(line)
    return "".join(out)


def _function_end(lines: list[str], start: int) -> int:
    depth = 0
    seen_open = False
    for i in range(start, len(lines)):
        for ch in lines[i]:
            if ch == "{":
                depth += 1
                seen_open = True
            elif ch == "}":
                depth -= 1
                if seen_open and depth == 0:
                    return i
    return len(lines) - 1


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("donor", help="matched function name (e.g. func_801082E4)")
    p.add_argument("target", help="unmatched function name to port to")
    p.add_argument("--write", metavar="FILE",
                   help="write the candidate C source to FILE "
                        "(refuses to overwrite an existing file)")
    p.add_argument("--table-only", action="store_true",
                   help="just print the substitution table; skip the body")
    p.add_argument("--force", action="store_true",
                   help="allow --write to overwrite an existing file")
    args = p.parse_args()

    donor_data = find_function(args.donor)
    target_data = find_function(args.target)
    if donor_data is None:
        raise SystemExit(f"{args.donor} not found in asm/")
    if target_data is None:
        raise SystemExit(f"{args.target} not found in asm/")

    donor_src_path = find_donor_source(args.donor)
    if donor_src_path is None:
        raise SystemExit(f"no src/*.c defines {args.donor} -- not a matched donor?")

    callee_map, global_map, imm_diffs = derive_substitutions(
        donor_data[1], target_data[1]
    )

    print(f"# donor:  {args.donor}  ({donor_src_path.relative_to(ROOT)})")
    print(f"# target: {args.target}")
    print(f"# {len(donor_data[1])} insns aligned")
    print()
    print("# callee swaps:")
    if callee_map:
        for k, v in sorted(callee_map.items()):
            print(f"  {k}  ->  {v}")
    else:
        print("  (none)")
    print()
    print("# global swaps:")
    if global_map:
        for k, v in sorted(global_map.items()):
            print(f"  {k}  ->  {v}")
    else:
        print("  (none)")
    if imm_diffs:
        print()
        print("# immediate diffs (NOT auto-swapped — review manually):")
        for idx, dline, tline in imm_diffs[:30]:
            print(f"  insn[{idx:3d}]  donor:  {dline}")
            print(f"             target: {tline}")
        if len(imm_diffs) > 30:
            print(f"  ... and {len(imm_diffs) - 30} more")

    if args.table_only:
        return 0

    donor_text = donor_src_path.read_text()
    donor_text = extract_donor_block(donor_text, args.donor)
    candidate = apply_subs(donor_text, callee_map, global_map)
    candidate = rename_definition(candidate, args.donor, args.target)
    candidate = dedup_extern_lines(candidate)

    print()
    print("# === candidate C source ===")
    print(candidate.rstrip())

    if args.write:
        out = Path(args.write)
        if out.exists() and not args.force:
            sys.stderr.write(
                f"refusing to overwrite {out} (use --force)\n"
            )
            return 2
        out.write_text(candidate)
        print(f"\n# wrote {out}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
