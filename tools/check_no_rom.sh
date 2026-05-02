#!/usr/bin/env bash
# =============================================================================
# check_no_rom.sh
#
# IP-safety scan. Refuses to let ROM data, extracted assets, or other
# IP-tainted files slip into a commit. Run before pushing; CI runs it too.
#
# Heuristics (intentionally conservative — false positives are fine, false
# negatives are not):
#
#   1. File extensions associated with ROM dumps and extracted assets.
#   2. Any tracked file > 256 KiB that is not in an allowlisted directory.
#   3. Any file whose first bytes look like an N64 ROM header (.z64/.n64/.v64
#      magic) regardless of extension.
#   4. Any file whose path matches our gitignore patterns but is somehow
#      tracked anyway.
#
# Exit non-zero on any hit.
# =============================================================================
set -euo pipefail

# Files to inspect: staged ones if running pre-commit, otherwise everything
# tracked by git. Override with FILES env var for ad-hoc scans.
if [[ -n "${FILES:-}" ]]; then
    mapfile -t files < <(printf "%s\n" "$FILES" | tr ' ' '\n')
elif git diff --cached --name-only --quiet 2>/dev/null; then
    mapfile -t files < <(git ls-files)
else
    mapfile -t files < <(git diff --cached --name-only --diff-filter=ACMR)
fi

bad=0
note() { echo "check_no_rom: $*" >&2; bad=1; }

# --- 1. extension blocklist ---
ext_block_re='\.(z64|n64|v64|rom|aifc|aiff|wav|mid|seq|ctl|tbl|bnk|rgba16|rgba32|ia8|ia16|ci4|ci8|i4|i8)$'
for f in "${files[@]}"; do
    [[ -z "$f" ]] && continue
    if [[ "$f" =~ $ext_block_re ]]; then
        note "blocked extension: $f"
    fi
done

# --- 2. size cap outside allowlisted dirs ---
# 256 KiB ceiling on tracked files; raise per-directory if a legitimate
# need arises (e.g. a vendored toolchain blob — but prefer a submodule).
allow_large_re='^(tools/toolchain/|\.git/)'
for f in "${files[@]}"; do
    [[ -z "$f" ]] && continue
    [[ ! -f "$f" ]] && continue
    if [[ "$f" =~ $allow_large_re ]]; then continue; fi
    size=$(wc -c < "$f")
    if (( size > 262144 )); then
        note "tracked file >256KiB (suspicious): $f ($size bytes)"
    fi
done

# --- 3. N64 ROM header sniff ---
# .z64 (big-endian):    80 37 12 40
# .n64 (little-endian): 40 12 37 80
# .v64 (byte-swapped):  37 80 40 12
for f in "${files[@]}"; do
    [[ -z "$f" ]] && continue
    [[ ! -f "$f" ]] && continue
    head4=$(head -c 4 "$f" 2>/dev/null | od -An -tx1 -N4 | tr -d ' \n' || true)
    case "$head4" in
        80371240|40123780|37804012)
            note "file starts with N64 ROM magic: $f"
            ;;
    esac
done

# --- 4. gitignore-tracked anomalies ---
# If a file is matched by .gitignore but is tracked anyway, surface it.
while IFS= read -r f; do
    [[ -z "$f" ]] && continue
    if git check-ignore -q "$f" 2>/dev/null; then
        note "tracked file matches .gitignore: $f"
    fi
done < <(printf "%s\n" "${files[@]}")

if (( bad )); then
    echo "" >&2
    echo "IP-safety scan FAILED. See messages above." >&2
    echo "If a hit is a false positive, document why in the PR and consider" >&2
    echo "tightening allowlists in tools/check_no_rom.sh rather than removing" >&2
    echo "the check." >&2
    exit 1
fi

echo "check_no_rom: OK (${#files[@]} files scanned)"
