#!/usr/bin/env bash
# make_expected.sh — snapshot the current build/ tree into expected/build/.
#
# After a green `make` (SHA-1 matches), run this to take a baseline snapshot.
# asm-differ then compares your in-progress build/<obj> against the saved
# expected/build/<obj>, which lets `diff.py -mo func_NAME` work without
# round-tripping the whole ROM build.
#
# Re-snapshot whenever a function NEAR the one you're matching gets matched
# itself (so the neighbouring objects in expected/ are up to date).

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "${PROJECT_ROOT}"

if [ ! -d build ]; then
    echo "ERROR: build/ does not exist. Run 'make' first." >&2
    exit 1
fi

if [ ! -f build/pokemonsnap.us.z64 ]; then
    echo "WARNING: build/pokemonsnap.us.z64 missing — snapshotting anyway." >&2
fi

# Verify the build is matching before snapshotting. If sha1sum is absent or
# fails, warn — but still let the user snapshot (they may want a baseline of
# a partially-matched state).
if [ -f baserom.us.z64 ] && [ -f build/pokemonsnap.us.z64 ]; then
    base="$(sha1sum baserom.us.z64 | awk '{print $1}')"
    built="$(sha1sum build/pokemonsnap.us.z64 | awk '{print $1}')"
    if [ "${base}" = "${built}" ]; then
        echo "OK: build matches baserom (SHA-1 ${built})"
    else
        echo "WARNING: build does NOT match baserom — snapshotting anyway." >&2
        echo "         baserom: ${base}"
        echo "         built:   ${built}"
    fi
fi

mkdir -p expected
rm -rf expected/build
cp -r build expected/build
echo "Snapshotted build/ → expected/build/ ($(du -sh expected/build | awk '{print $1}'))"
