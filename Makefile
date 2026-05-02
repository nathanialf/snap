# =============================================================================
# pokemonsnap — top-level Makefile
#
# This Makefile orchestrates a build that takes a USER-SUPPLIED ROM as input,
# verifies its SHA-1, splits it once, and rebuilds an identical image. No
# ROM data is stored in this repository; everything under build/, asm/, and
# assets/ is generated from your local baserom.
# =============================================================================

# ---- Configuration -----------------------------------------------------------

VERSION       ?= us
BASEROM       := baserom.$(VERSION).z64
TARGET_ROM    := build/pokemonsnap.$(VERSION).z64
SPLAT_YAML    := config/snap.$(VERSION).yaml
SHA1SUMS      := config/sha1sums.txt

PYTHON        ?= python3
VENV          := .venv
VENV_BIN      := $(VENV)/bin

MIPS_PREFIX   ?= mips-linux-gnu-
CC            := $(MIPS_PREFIX)gcc
LD            := $(MIPS_PREFIX)ld
OBJCOPY       := $(MIPS_PREFIX)objcopy

BUILD_DIR     := build
ASM_DIR       := asm
ASSET_DIR     := assets

# ---- Phony targets -----------------------------------------------------------

.PHONY: all setup verify split clean distclean check-no-rom progress help

all: $(TARGET_ROM)

help:
	@echo "Targets:"
	@echo "  setup         install python deps, fetch tools, split ROM"
	@echo "  verify        SHA-1 check user-supplied $(BASEROM)"
	@echo "  split         run splat (requires verified baserom)"
	@echo "  all           full build (default)"
	@echo "  progress      regenerate progress tables"
	@echo "  check-no-rom  scan staged files for ROM-derived data"
	@echo "  clean         remove build outputs"
	@echo "  distclean     also remove split asm/, extracted assets/, .venv/"
	@echo ""
	@echo "Variables:"
	@echo "  VERSION=$(VERSION)         (us | eu | jp)"
	@echo "  MIPS_PREFIX=$(MIPS_PREFIX)"
	@echo "  PYTHON=$(PYTHON)"

# ---- Setup -------------------------------------------------------------------

$(VENV)/.installed: tools/requirements.txt
	$(PYTHON) -m venv $(VENV)
	$(VENV_BIN)/pip install --upgrade pip
	$(VENV_BIN)/pip install -r tools/requirements.txt
	@touch $@

setup: $(VENV)/.installed verify split
	@echo "Setup complete. Run 'make' to build."

# ---- ROM verification --------------------------------------------------------
#
# Refuses to proceed if the user's baserom is missing or has an unexpected
# SHA-1. The expected hashes live in config/sha1sums.txt and are version-
# controlled so we can detect tampering. Hashes are fingerprints, not
# copyrighted material.

verify: $(BASEROM) $(SHA1SUMS) | $(VENV)/.installed
	@$(VENV_BIN)/python tools/verify_rom.py \
		--rom $(BASEROM) \
		--version $(VERSION) \
		--sha1sums $(SHA1SUMS)

$(BASEROM):
	@echo "ERROR: $(BASEROM) is missing." >&2
	@echo "       Place your own legally-obtained ROM at this path." >&2
	@echo "       See docs/BUILDING.md and docs/LEGAL.md." >&2
	@exit 1

# ---- Split -------------------------------------------------------------------

split: verify | $(VENV)/.installed
	@if [ ! -f $(SPLAT_YAML) ] || ! grep -q '^segments:' $(SPLAT_YAML) 2>/dev/null; then \
		echo "WARN: $(SPLAT_YAML) is a stub — no segments defined yet." >&2; \
		echo "      The split step is a no-op until config is filled in." >&2; \
	else \
		$(VENV_BIN)/splat split $(SPLAT_YAML); \
	fi
	@mkdir -p $(ASM_DIR) $(ASSET_DIR) $(BUILD_DIR)

# ---- Build (placeholder until src/ has matched code) -------------------------

$(TARGET_ROM): split
	@mkdir -p $(BUILD_DIR)
	@echo "NOTE: no decompiled code is currently linked." >&2
	@echo "      Once src/ contains matched objects, this rule will compile" >&2
	@echo "      and link them into $(TARGET_ROM). For now it produces" >&2
	@echo "      a stub so 'make' succeeds and CI can run." >&2
	@: > $(TARGET_ROM).stub

# ---- Progress / safety -------------------------------------------------------

progress: | $(VENV)/.installed
	@$(VENV_BIN)/python tools/progress.py --update

check-no-rom:
	@./tools/check_no_rom.sh

# ---- Cleanup -----------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf $(ASM_DIR)/* $(ASSET_DIR)/* $(VENV)
	@# Preserve hand-tracked asm/nonmatchings stubs and READMEs
	@git checkout -- $(ASM_DIR) $(ASSET_DIR) 2>/dev/null || true
