#!/usr/bin/env python3
"""Verify the SHA-1 of a user-supplied ROM against the project's expected list.

This script never reads, copies, or stores ROM bytes outside of streaming them
through hashlib. It is purely a fingerprint check; it tells you whether the
file you have on disk is the revision the project knows how to decompile.

Usage:
    verify_rom.py --rom baserom.us.z64 --version us --sha1sums config/sha1sums.txt

Exit codes:
    0  match
    1  hash mismatch (wrong revision, wrong region, modified, or corrupted)
    2  setup problem (missing file, no sha1 listed for this version, etc.)
"""
from __future__ import annotations

import argparse
import hashlib
import sys
from pathlib import Path


def parse_sha1sums(path: Path) -> dict[str, str]:
    """Read 'config/sha1sums.txt' as { version: sha1 }. Lines are
    'sha1<space>baserom.<version>.z64' or '# comment' or blank. A blank
    sha1 (commented out or absent) means the version is unsupported.
    """
    mapping: dict[str, str] = {}
    if not path.is_file():
        return mapping
    for raw in path.read_text().splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split()
        if len(parts) != 2:
            continue
        sha1, name = parts
        if not name.startswith("baserom.") or not name.endswith(".z64"):
            continue
        version = name[len("baserom."): -len(".z64")]
        mapping[version] = sha1.lower()
    return mapping


def sha1_of(path: Path) -> str:
    h = hashlib.sha1()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--rom", required=True, type=Path)
    ap.add_argument("--version", required=True)
    ap.add_argument("--sha1sums", required=True, type=Path)
    args = ap.parse_args()

    if not args.rom.is_file():
        print(f"ERROR: ROM not found: {args.rom}", file=sys.stderr)
        print("       Provide your own legally-obtained ROM at this path.",
              file=sys.stderr)
        return 2

    expected = parse_sha1sums(args.sha1sums)
    if args.version not in expected:
        print(f"ERROR: no expected SHA-1 listed for version '{args.version}' "
              f"in {args.sha1sums}.", file=sys.stderr)
        print("       This version is not yet supported by the project.",
              file=sys.stderr)
        return 2

    actual = sha1_of(args.rom)
    if actual != expected[args.version]:
        print(f"ERROR: SHA-1 mismatch for {args.rom}", file=sys.stderr)
        print(f"  expected: {expected[args.version]}", file=sys.stderr)
        print(f"  got:      {actual}", file=sys.stderr)
        print("       Your ROM is not the supported revision. Do NOT edit",
              file=sys.stderr)
        print("       sha1sums.txt; re-dump from your own cartridge.",
              file=sys.stderr)
        return 1

    print(f"OK: {args.rom} matches expected SHA-1 for version '{args.version}'.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
