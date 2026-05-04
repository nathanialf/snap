#!/usr/bin/env bash
# =============================================================================
# check_yaml_match.sh
#
# Catches the "false-match" workflow trap: a src/func_<addr>.c file whose
# yaml subsegment was never flipped from `asm` to `c`. In that state, the
# asm-derived .o still provides the bytes at link time, so SHA-1 trivially
# matches the baserom regardless of whether the .c compiles to anything
# sensible. The user thinks the function is matched; it isn't.
#
# This script greps the linker script (config/pokemonsnap.us.ld) for each
# src/func_*.c filename. Anything missing means the corresponding yaml
# subsegment is still asm — fix the yaml or remove the .c.
#
# Run as part of `make` post-link, or manually before committing.
# =============================================================================
set -euo pipefail

LD_SCRIPT="${LD_SCRIPT:-config/pokemonsnap.us.ld}"
SRC_DIR="${SRC_DIR:-src}"

if [[ ! -f "$LD_SCRIPT" ]]; then
    echo "check_yaml_match: $LD_SCRIPT not found (run 'make split' first)" >&2
    exit 1
fi

bad=0
for c in "$SRC_DIR"/*.c; do
    [[ ! -f "$c" ]] && continue
    fn="$(basename "$c" .c)"
    # Only check files named like func_<addr>; skip ultra_*, boot_*, etc.
    [[ ! "$fn" =~ ^func_[0-9A-Fa-f]{8}$ ]] && continue
    if ! grep -q "/${fn}\.o" "$LD_SCRIPT"; then
        echo "check_yaml_match: $c not linked (yaml subsegment likely still 'asm')" >&2
        bad=1
    fi
done

if (( bad )); then
    echo "" >&2
    echo "Some src/func_<addr>.c files are not referenced by the linker script." >&2
    echo "Either flip their yaml subsegment from 'asm' to 'c, <funcname>' and" >&2
    echo "rerun 'make split', or delete the unused .c file. False matches" >&2
    echo "(SHA-1 trivially passes via asm path) are how broken commits land." >&2
    exit 1
fi

count=$(ls "$SRC_DIR"/func_*.c 2>/dev/null | wc -l)
echo "check_yaml_match: OK ($count func_*.c files all linked)"
