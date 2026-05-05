#!/usr/bin/env python3
"""Flatten a C source file into a single self-contained "context" blob.

The downstream consumers are mips_to_c (which wants a single-file C
context to resolve struct layouts and typedefs) and decomp.me (whose
"context" pane wants the same shape). Both choke on stock-gcc predefined
macros (`__GNUC__`, `__SIZEOF_LONG__`, etc.) — those leak target-host
details and pollute the namespace mips_to_c reasons over.

Algorithm: two cpp passes.

  1. Empty-input pass with `-dM` dumps just the freebie macros gcc
     defines on every translation unit. We capture that set as "stock
     macros to strip".
  2. Real pass on the input expands every `#include` inline and emits
     normal flattened C plus the same stock macros at the top.

We then walk pass-2 line-by-line and drop any `#define` line that also
appeared in pass 1, leaving project content only.

Invocation:

    tools/m2ctx.py src/func_80100928.c            # writes ./ctx.c
    tools/m2ctx.py src/func_80100928.c -o /tmp/x  # custom output path

This is a tooling helper — it never reads or writes ROM bytes, never
touches the build, and is safe to run on any `.c` file in the tree.
"""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path

# Project root = parent of tools/. Resolved off __file__ so the script
# works whatever cwd it's launched from (Make, IDE, plain shell).
ROOT = Path(__file__).resolve().parent.parent
MAKEFILE = ROOT / "Makefile"


def parse_makefile_includes() -> tuple[list[str], list[str]]:
    """Pull -I and project-relevant -D flags out of the Makefile.

    We scan CFLAGS_IDO and CFLAGS_ULTRA (plus their *_OS / *_LL friends)
    for -I directives and -D macro definitions. Optimizer/ABI flags
    (-O*, -mips*, -G, -woff, -signed, -Xfullwarn, etc.) are skipped —
    they're meaningful to IDO but irrelevant to the host gcc preprocessor.

    Returns (include_flags, define_flags). Both are lists of literal
    argv tokens ready to splice into a gcc command line.
    """

    text = MAKEFILE.read_text()
    # Tokenize aggressively: any whitespace, including the line
    # continuations that `make` joins itself. We just want the raw
    # set of `-I path` and `-Dfoo` tokens, regardless of which CFLAGS
    # variable they sit under.
    tokens = re.findall(r"-[ID][^\s\\]*", text)

    includes: list[str] = []
    defines: list[str] = []
    seen: set[str] = set()
    for tok in tokens:
        if tok in seen:
            continue
        seen.add(tok)
        if tok.startswith("-I"):
            path = tok[2:]
            # Resolve relative paths against project root so the tool
            # is cwd-agnostic.
            full = (ROOT / path).resolve() if not Path(path).is_absolute() else Path(path)
            includes.append(f"-I{full}")
        elif tok.startswith("-D"):
            defines.append(tok)

    # Always assert M2CTX so headers can opt-in to context-only shims
    # if they ever need to (e.g. forward-declare opaque types). Matches
    # the convention used by oot/mm/sm64 m2ctx tooling.
    if "-DM2CTX" not in defines:
        defines.append("-DM2CTX")

    return includes, defines


def run_cpp(extra_args: list[str], input_path: str) -> str:
    """Invoke host gcc as a preprocessor and return stdout as text.

    `-x c` forces C parsing even when the input is `/dev/null` (which
    gcc would otherwise refuse to preprocess because of the unknown
    extension). `-ffreestanding` keeps gcc from auto-injecting hosted
    headers we don't care about.
    """

    cmd = [
        "gcc",
        "-E",
        "-P",
        "-x", "c",
        "-ffreestanding",
        "-nostdinc",
        *extra_args,
        input_path,
    ]
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        sys.stderr.write(f"gcc -E failed (exit {proc.returncode}):\n")
        sys.stderr.write(proc.stderr)
        sys.exit(proc.returncode)
    return proc.stdout


# A `#define` line gcc emits looks like:
#     #define NAME body...
#     #define NAME(args) body...
# We only need the NAME to dedupe — body comparison is irrelevant
# because pass-1 and pass-2 see the same gcc, so identical names mean
# identical bodies.
DEFINE_NAME = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)")


def stock_macro_names(dump: str) -> set[str]:
    """Extract macro names from a `-dM` dump."""

    names: set[str] = set()
    for line in dump.splitlines():
        m = DEFINE_NAME.match(line)
        if m:
            names.add(m.group(1))
    return names


def strip_stock_defines(text: str, stock: set[str]) -> str:
    """Drop `#define NAME ...` lines whose NAME is in `stock`.

    Pass-2 output prepends all stock macros at the top of the file
    when `-dM` is also supplied, but we don't ask for `-dM` on pass 2 —
    so most of the stock noise never appears. The few that do leak
    through (mostly from `#pragma` echo or future cpp behavior) are
    cleaned up here for safety.
    """

    out: list[str] = []
    for line in text.splitlines(keepends=True):
        m = DEFINE_NAME.match(line)
        if m and m.group(1) in stock:
            continue
        out.append(line)
    return "".join(out)


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Flatten a C source file into a single mips_to_c / decomp.me context blob.",
    )
    parser.add_argument("input", help="C source file to flatten (path under src/, lib/, etc.)")
    parser.add_argument(
        "-o", "--output",
        default=str(ROOT / "ctx.c"),
        help="Output path for the flattened context (default: ctx.c at project root).",
    )
    args = parser.parse_args(argv)

    if not shutil.which("gcc"):
        sys.stderr.write("error: host gcc not found on PATH; install build-essential or equivalent.\n")
        return 1

    in_path = Path(args.input).resolve()
    if not in_path.is_file():
        sys.stderr.write(f"error: input file not found: {in_path}\n")
        return 1

    includes, defines = parse_makefile_includes()
    cpp_args = [*includes, *defines]

    # Pass 1: stock-macro snapshot. Empty translation unit, just the
    # freebies gcc adds. We supply the project's -I/-D flags too so any
    # macro that would *override* a stock one (e.g. a project header
    # redefining a builtin) doesn't leak through; identical name in
    # both passes still gets stripped.
    stock_dump = run_cpp(["-dM", *cpp_args], "/dev/null")
    stock = stock_macro_names(stock_dump)

    # Pass 2: real expansion. No -dM here — we want flattened C, not a
    # macro dump.
    expanded = run_cpp(cpp_args, str(in_path))

    cleaned = strip_stock_defines(expanded, stock)

    out_path = Path(args.output).resolve()
    out_path.write_text(cleaned)

    line_count = cleaned.count("\n")
    byte_count = len(cleaned.encode("utf-8"))
    sys.stderr.write(
        f"m2ctx: wrote {out_path} ({byte_count} bytes, {line_count} lines; "
        f"stripped {len(stock)} stock macros)\n"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
