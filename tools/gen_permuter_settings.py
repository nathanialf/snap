#!/usr/bin/env python3
"""Generate `permuter_settings.toml` at the project root.

decomp-permuter (vendored at `tools/decomp-permuter/`) reads
`permuter_settings.toml` from the repo root or `tools/`. The settings
file controls:

  - `compiler_type`        — codegen backend the permuter targets.
  - `[preserve_macros]`    — regex → C-type-string. Macros whose name
                              matches a regex are left un-expanded by
                              the permuter's `cpp` pass and treated as
                              opaque expressions of the given type. This
                              keeps `gSPEndDisplayList(...)` etc. intact
                              instead of exploding into raw `gDma1p`
                              byte goo that pycparser can't randomise
                              cleanly.
  - `[decompme.compilers]` — map our local IDO 7.1 binary path to the
                              decomp.me compiler ID `ido7.1`, so
                              `import.py --decompme` can post scratches
                              for this project.

Why a generator instead of a hand-written file:

  - The IDO path is environment-dependent (`IDO_DIR` env, default
    `~/.local/share/ido-recomp/build/7.1/out/cc`). We resolve it the
    same way `tools/quick_diff.sh` and `tools/permute_run.sh` do.
  - The preserve-macros list should track project headers. Re-running
    this script after adding a new family of macros (e.g. project-side
    `ARRAY_COUNT`, `SQ`, etc. once we land them in `include/`) keeps
    the list honest.

Schema reference: `tools/decomp-permuter/permuter_settings_example.toml`
and `tools/decomp-permuter/import.py::build_preserve_macros`. Keys in
`[preserve_macros]` are anchored regex patterns; values are the C type
the permuter should pretend the macro expands to.

Approach for `[preserve_macros]`:

  We deliberately list *patterns*, not every individual macro. The
  permuter compiles them into one big `^(?:p1)|(?:p2)|...$` regex, so
  small wildcarded patterns are both cheaper and easier to maintain
  than 800+ explicit entries. The list below is curated from a scan of
  `include/*.h` and `lib/ultralib/include/{,PR/,PRinternal/}*.h`,
  filtered to families that:

    (a) actually appear (or are likely to appear) in matchable C, and
    (b) carry meaningful semantics that the permuter shouldn't explode
        — gfx microcode macros, hardware-register I/O, virtual-address
        conversions, libultra time helpers, and the project's small set
        of helper macros.

  Adding a new family: drop a pattern in `_PRESERVE_PATTERNS` below,
  re-run this script, commit. No need to hand-edit the toml.
"""

from __future__ import annotations

import os
import sys
from pathlib import Path
from typing import List, Tuple

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_IDO_VERSION = "7.1"
HEADER_COMMENT = (
    "# permuter_settings.toml — managed by tools/gen_permuter_settings.py.\n"
    "# Re-run that script to regenerate (e.g. after `IDO_DIR=` changes or new\n"
    "# project macros land in `include/`). Do not hand-edit; edits are lost.\n"
)


# (regex_pattern, expansion_type)
#
# Patterns are matched anchored (the permuter wraps them in `^(?:...)$`),
# so `g[sS]?[DSM].*` only matches macros that *start* with that prefix.
# Keep the list curated — every entry is something we expect to see in
# matchable Snap C or canonical libultra wrappers.
_PRESERVE_PATTERNS: List[Tuple[str, str]] = [
    # --- Project / libultra helper macros (ultratypes / common) -----------
    # NULL is a constant pointer; the permuter treats it as `void *`-ish.
    ("NULL", "void *"),
    # Project-side helper macros that may appear once we land them.
    # Defined as patterns even if not yet present in `include/` — harmless
    # if they don't match any source macro yet.
    ("ARRAY_COUNT", "u32"),
    ("ARRAY_COUNTU", "u32"),
    ("ABS", "s32"),
    ("CLAMP", "s32"),
    ("CLAMP_MAX", "s32"),
    ("CLAMP_MIN", "s32"),
    ("SQ", "s32"),
    ("MIN", "s32"),
    ("MAX", "s32"),

    # --- libultra: virtual-address conversions ---------------------------
    # All of K0_TO_K1, K1_TO_K0, K0_TO_PHYS, K1_TO_PHYS, PHYS_TO_K0,
    # PHYS_TO_K1 — same shape, take an address-ish int, return u32.
    ("K[01]_TO_(?:K[01]|PHYS)", "u32"),
    ("PHYS_TO_K[01]", "u32"),
    ("OS_K0_TO_PHYSICAL", "u32"),
    ("OS_PHYSICAL_TO_K[01]", "u32"),

    # --- libultra: memory-mapped I/O -------------------------------------
    # IO_READ returns a u32; IO_WRITE is a statement-expression but typing
    # it as `void` keeps the permuter from trying to feed it through an
    # rvalue context. The two `IO_*` macros are the standard pair.
    ("IO_READ", "u32"),
    ("IO_WRITE", "void"),

    # --- libultra: cycles/time conversions -------------------------------
    # OS_CYCLES_TO_NSEC, OS_CYCLES_TO_USEC, OS_USEC_TO_CYCLES, etc.
    ("OS_CYCLES_TO_[A-Z]+", "u64"),
    ("OS_[NU]?SEC_TO_CYCLES", "u64"),

    # --- libultra: gbi.h gfx microcode macros ----------------------------
    # gSP* / gsSP* / gDP* / gsDP* / gDma* / gsDma* / gMove* / gsMove* /
    # gImm* / gsImm*. The `gs` prefix is the static-init form and shares
    # the void shape (it's an aggregate initialiser, but `void` works at
    # the cpp level since the permuter doesn't see the macro body).
    ("g[sS]?[A-Z][A-Za-z0-9_]*", "void"),
    # Microcode field/format constants are integer-valued.
    ("G_IM_(?:FMT|SIZ)_[A-Za-z0-9_]+", "u32"),
    ("G_TX_[A-Z0-9_]+", "u32"),
    ("G_ML_[A-Z0-9_]+", "u32"),
    ("G_MTX_[A-Z0-9_]+", "u32"),
    ("G_RM_[A-Z0-9_]+", "u32"),
    ("G_CC_[A-Z0-9_]+", "u32"),
    ("G_AC_[A-Z0-9_]+", "u32"),
    ("G_ZS_[A-Z0-9_]+", "u32"),
    ("G_TC_[A-Z0-9_]+", "u32"),
    ("G_TF_[A-Z0-9_]+", "u32"),
    ("G_TT_[A-Z0-9_]+", "u32"),
    ("G_TL_[A-Z0-9_]+", "u32"),
    ("G_TD_[A-Z0-9_]+", "u32"),
    ("G_TP_[A-Z0-9_]+", "u32"),
    ("G_PM_[A-Z0-9_]+", "u32"),
    ("G_CD_[A-Z0-9_]+", "u32"),
    ("G_CK_[A-Z0-9_]+", "u32"),
    ("G_LOD_[A-Z0-9_]+", "u32"),
    ("G_BL_[A-Z0-9_]+", "u32"),
    ("G_SC_[A-Z0-9_]+", "u32"),

    # --- libultra: bit-pack helpers (gbi.h) ------------------------------
    # _SHIFTL, _SHIFTR — the canonical bit-pack helpers from gbi.h.
    ("_SHIFTL", "unsigned int"),
    ("_SHIFTR", "unsigned int"),
]


def resolve_ido_cc() -> str:
    """Mirror the IDO resolution from `tools/quick_diff.sh` and
    `tools/permute_run.sh`: env override `IDO_DIR=`, else
    `~/.local/share/ido-recomp/build/<IDO_VERSION>/out/`."""
    ido_dir = os.environ.get("IDO_DIR")
    if ido_dir:
        return str(Path(ido_dir) / "cc")
    ido_version = os.environ.get("IDO_VERSION", DEFAULT_IDO_VERSION)
    home = Path(os.path.expanduser("~"))
    return str(home / ".local" / "share" / "ido-recomp" / "build" / ido_version / "out" / "cc")


def _toml_quote(s: str) -> str:
    # Basic TOML basic-string quoting — enough for our paths/types which
    # never contain quotes, backslashes, or control chars. Bail loudly
    # if that assumption breaks so we don't silently emit invalid TOML.
    if any(c in s for c in ('"', "\\", "\n", "\t")):
        raise ValueError(f"value needs richer TOML escaping: {s!r}")
    return f'"{s}"'


def render_toml(ido_cc: str) -> str:
    out = [HEADER_COMMENT]
    out.append("\n")
    out.append('compiler_type = "ido"\n')
    out.append("\n")

    out.append("[preserve_macros]\n")
    out.append("# Regex pattern -> C type the permuter pretends the macro returns.\n")
    out.append("# Patterns are anchored by the permuter (`^(?:...)$`).\n")
    for pattern, type_str in _PRESERVE_PATTERNS:
        out.append(f"{_toml_quote(pattern)} = {_toml_quote(type_str)}\n")
    out.append("\n")

    out.append("[decompme.compilers]\n")
    out.append("# Local IDO binary path -> decomp.me compiler ID. Wildcards allowed;\n")
    out.append("# the permuter uses fnmatch on the full compiler[0] path.\n")
    out.append(f"{_toml_quote(ido_cc)} = \"ido7.1\"\n")
    # Also accept any IDO 7.1 install layout under ido-recomp, in case
    # someone runs with a different `IDO_DIR` than the one resolved at
    # generation time. fnmatch globs on the literal path string.
    out.append('"*/ido-recomp/build/7.1/out/cc" = "ido7.1"\n')

    return "".join(out)


def main() -> int:
    ido_cc = resolve_ido_cc()
    body = render_toml(ido_cc)

    out_path = PROJECT_ROOT / "permuter_settings.toml"
    # Idempotent write: only touch the file if contents actually changed.
    # Lets `make`-style consumers see a stable mtime when nothing moved.
    if out_path.exists():
        old = out_path.read_text(encoding="utf-8")
        if old == body:
            print(f"permuter_settings.toml unchanged ({out_path})")
            return 0

    out_path.write_text(body, encoding="utf-8")
    print(f"wrote {out_path}")
    print(f"  compiler_type = ido")
    print(f"  IDO cc        = {ido_cc}")
    print(f"  preserve_macros patterns: {len(_PRESERVE_PATTERNS)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
