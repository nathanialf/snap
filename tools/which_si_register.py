#!/usr/bin/env python3
"""Find C-source idioms that reproduce $s0/$s1 usage in a target function.

The puzzle: an unmatched function holds a value in a callee-saved
register ($s0..$s7) across one or more sub-calls. The C idiom that
makes IDO 7.1 reach for an $s reg can be subtle:

    arg cached across calls            -- s0 = arg pre-call
    global ptr cached across calls     -- s0 = D_GLOBAL (load lifted)
    return value cached across calls   -- s0 = func_X(); ...; func_Y(s0)
    pointer through chain of derefs    -- s0 = arg->next; do { ... } while(s0)
    loop induction variable            -- for(s0=0; s0<N; s0++) ...

This tool profiles the target's $s usage (how many calls span each
$s reg's lifetime, where the value comes from) and lists matched
functions *near* the target that show the same shape, so you can
read their C source and copy the idiom.

"Near" defaults to functions in the same .c file that the target
will eventually live in (same yaml subsegment) plus matched siblings
in the cluster. Pass --window N to widen the VRAM search radius.

Read-only — only consults asm/ + src/ + yaml.

Usage:
    tools/which_si_register.py func_NAME
    tools/which_si_register.py func_NAME --window 0x4000
    tools/which_si_register.py func_NAME --show          # also dump C bodies
"""
from __future__ import annotations

import argparse
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_DIR = ROOT / "asm"
SRC_DIR = ROOT / "src"

FN_RE = re.compile(
    r"^glabel (func_[0-9A-Fa-f]+)\n(.*?)^endlabel \1",
    re.DOTALL | re.MULTILINE,
)
INSN_RE = re.compile(
    r"^\s*/\*\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+\*/\s+(\S+)(?:\s+(.*?))?$"
)
S_REG_RE = re.compile(r"\$(s[0-7])\b")
SP_LOAD_RE = re.compile(r"(\$\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\(\$sp\)")
GLOBAL_OP_RE = re.compile(r"%(?:hi|lo)\([A-Za-z_][\w]*\)")


def matched_funcs() -> dict[str, Path]:
    """Map func_NAME -> defining .c path."""
    out: dict[str, Path] = {}
    if not SRC_DIR.is_dir():
        return out
    decl_re = re.compile(r"^\s*[\w\s\*]+\b(func_[0-9A-Fa-f]{8})\s*\(")
    for p in SRC_DIR.rglob("*.c"):
        try:
            text = p.read_text(errors="ignore")
        except OSError:
            continue
        for line in text.splitlines():
            m = decl_re.match(line)
            if m:
                out[m.group(1)] = p
    return out


def function_blocks() -> dict[str, dict]:
    """name -> {'vram': int, 'rom': int, 'insns': [(mnemonic, operands)]}."""
    fns: dict[str, dict] = {}
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
            insns: list[tuple[str, str]] = []
            rom = vram = None
            for line in body.splitlines():
                im = INSN_RE.match(line)
                if im:
                    if rom is None:
                        rom = int(im.group(1), 16)
                        vram = int(im.group(2), 16)
                    insns.append((im.group(3), (im.group(4) or "").strip()))
            if insns:
                fns[name] = {"vram": vram, "rom": rom, "insns": insns}
    return fns


def vram_from_name(name: str) -> int | None:
    """Pull the VRAM hex out of func_8010ABCD."""
    try:
        return int(name.removeprefix("func_"), 16)
    except ValueError:
        return None


def s_reg_uses(insns: list[tuple[str, str]]) -> dict[str, list[int]]:
    """Map 's0'..'s7' -> [insn_idx, ...] where it appears."""
    uses: dict[str, list[int]] = {}
    for i, (_, operands) in enumerate(insns):
        for m in S_REG_RE.finditer(operands):
            uses.setdefault(m.group(1), []).append(i)
    return uses


def jal_indices(insns: list[tuple[str, str]]) -> list[int]:
    return [i for i, (m, _) in enumerate(insns) if m in ("jal", "jalr")]


def lifetime_jals(uses: list[int], jals: list[int]) -> int:
    """Number of jals between first and last use of an $s reg."""
    if not uses:
        return 0
    lo, hi = uses[0], uses[-1]
    return sum(1 for j in jals if lo < j < hi)


STORE_OPS = {"sw", "sh", "sb", "sd", "swc1", "sdc1", "swl", "swr"}


def origin_of(insns: list[tuple[str, str]], reg: str) -> str:
    """Heuristic: where does the first *definition* of $reg come from?
    Returns one of: 'from-arg', 'from-global', 'from-stack', 'from-call',
    'computed', or 'unknown'.

    Skips stores (sw/sh/sb…) — they have the $sN as a source operand,
    not a destination, so a function's prologue spill of $s0 doesn't
    count as 'defining' $s0."""
    target = f"${reg}"
    for i, (mnemonic, operands) in enumerate(insns):
        if mnemonic in STORE_OPS:
            continue
        toks = operands.split(",", 1)
        if not toks:
            continue
        first = toks[0].strip()
        if first != target:
            continue
        if mnemonic == "or" and "$zero" in operands:
            rhs = operands.split(",")[1].strip() if "," in operands else ""
            if rhs.startswith("$a"):
                return "from-arg"
            if rhs == "$v0":
                return "from-call"
            return "computed"
        if mnemonic in ("addiu", "addu", "add"):
            if "$a0" in operands or "$a1" in operands or \
               "$a2" in operands or "$a3" in operands:
                return "from-arg"
            if GLOBAL_OP_RE.search(operands):
                return "from-global"
            return "computed"
        if mnemonic in ("lw", "lh", "lhu", "lb", "lbu", "lwc1", "ldc1"):
            if "($sp)" in operands:
                return "from-stack"
            if GLOBAL_OP_RE.search(operands):
                return "from-global"
            if any(r in operands for r in ("$a0", "$a1", "$a2", "$a3")):
                return "from-arg"
            return "from-deref"
        if mnemonic == "lui" and GLOBAL_OP_RE.search(operands):
            return "from-global"
        return "unknown"
    return "unknown"


def profile(name: str, insns: list[tuple[str, str]]) -> dict:
    uses = s_reg_uses(insns)
    jals = jal_indices(insns)
    rows = []
    for reg, idxs in sorted(uses.items()):
        rows.append({
            "reg": reg,
            "uses": len(idxs),
            "spans_calls": lifetime_jals(idxs, jals),
            "origin": origin_of(insns, reg),
        })
    return {
        "name": name,
        "n_jals": len(jals),
        "rows": rows,
        "profile_key": tuple(
            (r["reg"], r["origin"], r["spans_calls"]) for r in rows
        ),
    }


def short_profile_str(prof: dict) -> str:
    if not prof["rows"]:
        return "no $s regs"
    return "  ".join(
        f"${r['reg']}={r['origin']}({r['spans_calls']}j)"
        for r in prof["rows"]
    )


def cmd_main(args: argparse.Namespace) -> int:
    fns = function_blocks()
    matched = matched_funcs()
    target_name = args.target
    if not target_name.startswith("func_"):
        target_name = "func_" + target_name.lstrip("0x").upper()

    # The target may be matched (asm not in tree) or unmatched.
    # If it's unmatched we can read its asm directly; if matched, we
    # can still read it via splat's nonmatchings path or from the
    # original asm/*.s before it was moved (best-effort).
    target_data = fns.get(target_name)
    if target_data is None:
        # Try nonmatchings
        nonmatch_path = ASM_DIR / "nonmatchings" / target_name
        if nonmatch_path.is_dir():
            for p in nonmatch_path.rglob("*.s"):
                try:
                    text = p.read_text(errors="ignore")
                except OSError:
                    continue
                m = FN_RE.search(text)
                if m and m.group(1) == target_name:
                    insns = []
                    for line in m.group(2).splitlines():
                        im = INSN_RE.match(line)
                        if im:
                            insns.append((im.group(3), (im.group(4) or "").strip()))
                    target_data = {
                        "vram": vram_from_name(target_name),
                        "rom": None,
                        "insns": insns,
                    }
                    break
        if target_data is None:
            raise SystemExit(f"{target_name} not found in asm/")

    target_prof = profile(target_name, target_data["insns"])

    vram = target_data["vram"] or vram_from_name(target_name) or 0
    print(f"# {target_name}  vram={vram:#x}  "
          f"matched={target_name in matched}")
    print(f"# $s usage: {short_profile_str(target_prof)}")
    print(f"# {target_prof['n_jals']} sub-calls in body")
    if not target_prof["rows"]:
        print("# (target uses no $s registers — nothing to look up)")
        return 0

    target_vram = target_data["vram"] or vram_from_name(target_name) or 0
    window = args.window
    print(f"# searching matched fns within ±{window:#x} VRAM")
    print()

    near: list[tuple[int, str, Path, dict]] = []
    for name, src_path in matched.items():
        v = vram_from_name(name)
        if v is None:
            continue
        d = abs(v - target_vram)
        if d > window:
            continue
        try:
            text = src_path.read_text()
        except OSError:
            continue
        feats = scan_c_source_features(text, name)
        near.append((d, name, src_path, feats))

    near.sort(key=lambda r: r[0])

    if not near:
        print("# no matched functions found within window")
        print("# tip: widen with --window 0x10000")
        return 0

    # Score by feature overlap with target's $s profile (heuristic):
    target_origin_counts = Counter(r["origin"] for r in target_prof["rows"])
    spans = max(r["spans_calls"] for r in target_prof["rows"])

    rows = []
    for dist, name, src_path, feats in near:
        score = score_features(feats, target_origin_counts, spans)
        rows.append((-score, dist, name, src_path, feats))
    rows.sort()

    print(f"# {len(rows)} matched fn(s) in window; ranked by C-source "
          f"feature overlap with target's $s profile")
    print()
    print(f"  {'Δvram':>8}  {'function':<18}  features")
    print(f"  {'-'*8}  {'-'*18}  {'-'*60}")
    for _, dist, name, _, feats in rows[:args.k]:
        feat_str = ", ".join(f"{k}={v}" for k, v in feats.items() if v) or "-"
        print(f"  {dist:#08x}  {name:<18}  {feat_str}")

    if args.show:
        print()
        print(f"# === C sources of top {min(args.show, len(rows))} ===")
        seen_paths: set[Path] = set()
        for _, _, name, src_path, _ in rows[:args.show]:
            if src_path in seen_paths:
                continue
            seen_paths.add(src_path)
            print()
            print(f"# --- {src_path.relative_to(ROOT)}  (closest match: {name}) ---")
            print(src_path.read_text().rstrip())
    else:
        print()
        print(f"# tip: tools/which_si_register.py {target_name} --show 3 "
              f"to also dump the top 3 matched fns' C sources")
    return 0


def scan_c_source_features(text: str, name: str) -> dict[str, int]:
    """Count rough C-source signals that hint at $s-register idioms."""
    fn_re = re.compile(
        rf"^\s*[\w\s\*]+\b{re.escape(name)}\s*\(.*?\)\s*\{{",
        re.MULTILINE | re.DOTALL,
    )
    m = fn_re.search(text)
    if m:
        body = _balanced_block(text, m.end() - 1)
    else:
        body = text
    return {
        "for-loop": len(re.findall(r"\bfor\s*\(", body)),
        "while-loop": len(re.findall(r"\bwhile\s*\(", body)),
        "do-while": len(re.findall(r"\bdo\s*\{", body)),
        "deref-chain": len(re.findall(r"->\w+\s*->", body)),
        "func-call": len(re.findall(r"\bfunc_[0-9A-Fa-f]{8}\s*\(", body)),
        "global-ref": len(re.findall(r"\bD_[0-9A-Fa-f]{8}\b", body)),
        "ptr-cache": len(re.findall(r"=\s*&?\s*\w+(?:->\w+)+\s*;", body)),
    }


def _balanced_block(text: str, start: int) -> str:
    depth = 0
    out = []
    for i in range(start, len(text)):
        ch = text[i]
        out.append(ch)
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                break
    return "".join(out)


def score_features(feats: dict[str, int],
                   target_origins: Counter,
                   target_spans: int) -> float:
    score = 0.0
    if target_origins.get("from-arg", 0) > 0:
        score += min(feats.get("func-call", 0), 3)
    if target_origins.get("from-global", 0) > 0:
        score += min(feats.get("global-ref", 0), 3)
    if target_origins.get("from-call", 0) > 0:
        score += 0.5 * min(feats.get("func-call", 0), 4)
    if target_origins.get("from-deref", 0) > 0:
        score += 2 * min(feats.get("deref-chain", 0), 2)
    if target_spans >= 2:
        score += min(feats.get("for-loop", 0)
                     + feats.get("while-loop", 0)
                     + feats.get("do-while", 0), 3)
    return score


def main() -> int:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("target", help="function name (matched or unmatched)")
    p.add_argument("--window", type=lambda s: int(s, 0), default=0x4000,
                   help="VRAM search radius (default 0x4000)")
    p.add_argument("--show", type=int, default=0, metavar="N",
                   help="dump the C sources of the top N matched fns")
    p.add_argument("-k", type=int, default=30,
                   help="cap the ranked listing (default 30)")
    args = p.parse_args()
    return cmd_main(args)


if __name__ == "__main__":
    sys.exit(main())
