#!/usr/bin/env bash
# =============================================================================
# tools/setup.sh
#
# One-shot host setup. Idempotent — running it twice is fine.
#
#   1. Create a Python venv at .venv and install tools/requirements.txt.
#   2. Initialize / update git submodules (splat, asm-differ, etc.).
#   3. Optionally fetch a known-good MIPS toolchain into tools/toolchain/.
#
# This script downloads no ROM data, no assets, and no Pokémon-Snap-specific
# files. The toolchain is a generic MIPS GCC build used by many N64 decomp
# projects; it is not project-specific.
# =============================================================================
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

PYTHON="${PYTHON:-python3}"
VENV="${VENV:-.venv}"

# --- 1. Python venv ----------------------------------------------------------

if [[ ! -d "$VENV" ]]; then
    echo "==> creating venv at $VENV"
    "$PYTHON" -m venv "$VENV"
fi
# shellcheck disable=SC1091
source "$VENV/bin/activate"
python -m pip install --upgrade pip >/dev/null
python -m pip install -r tools/requirements.txt

# --- 2. Submodules -----------------------------------------------------------

if [[ -d .git ]] && [[ -f .gitmodules ]]; then
    echo "==> updating submodules"
    git submodule update --init --recursive
else
    echo "==> no submodules declared yet (skipping)"
fi

# --- 3. Toolchain (optional) -------------------------------------------------

# Skip with SKIP_TOOLCHAIN=1; useful in CI where the toolchain is preinstalled.
if [[ "${SKIP_TOOLCHAIN:-0}" == "1" ]]; then
    echo "==> SKIP_TOOLCHAIN=1; not fetching MIPS GCC"
elif command -v mips-linux-gnu-gcc >/dev/null 2>&1; then
    echo "==> system mips-linux-gnu-gcc detected; using it"
elif [[ -x tools/toolchain/bin/mips-linux-gnu-gcc ]]; then
    echo "==> tools/toolchain/ already populated"
else
    cat <<'EOF' >&2

==> No MIPS GCC found.

This project does not bundle a toolchain. On Debian/Ubuntu you can install
one with:

    sudo apt-get install gcc-mips-linux-gnu binutils-mips-linux-gnu

On macOS (Homebrew):

    brew install mips-linux-gnu-binutils mips-linux-gnu-gcc      # if available
    # or use a containerized toolchain via 'docker run'.

If you prefer a project-local install, create tools/toolchain/ and place the
prefix-named binaries (mips-linux-gnu-gcc, -ld, -objcopy, ...) in
tools/toolchain/bin/. The Makefile honors MIPS_PREFIX=tools/toolchain/bin/mips-linux-gnu-.

EOF
fi

echo
echo "Setup complete. Next steps:"
echo "  1. Place your own legally-obtained ROM at baserom.us.z64"
echo "  2. Run 'make setup' to verify and split"
echo "  3. Run 'make' to build"
