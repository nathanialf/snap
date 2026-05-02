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

# Object lists are derived at parse time from whatever splat has emitted.
# 'make setup' must have run first; otherwise these are empty and link fails
# with a clear "no input files" error.
ASM_SRCS      := $(wildcard $(ASM_DIR)/*.s)
ASM_OBJS      := $(patsubst $(ASM_DIR)/%.s,$(BUILD_DIR)/asm/%.o,$(ASM_SRCS))
BIN_SRCS      := $(wildcard $(ASSET_DIR)/*.bin)
BIN_OBJS      := $(patsubst $(ASSET_DIR)/%.bin,$(BUILD_DIR)/assets/%.o,$(BIN_SRCS))
ALL_OBJS      := $(ASM_OBJS) $(BIN_OBJS)

ASFLAGS       := -EB -march=vr4300 -mabi=32 -G 0 -no-pad-sections -Iinclude
LDSCRIPT      := config/pokemonsnap.us.ld
AUTO_FUNCS    := config/undefined_funcs_auto.us.txt
AUTO_SYMS     := config/undefined_syms_auto.us.txt
EXTRA_SYMS    := config/undefined_funcs_extra.us.txt
SYMBOL_ADDRS  := config/symbol_addrs.us.txt
LDFLAGS       := -EB -T $(LDSCRIPT) \
                 -T $(AUTO_FUNCS) -T $(AUTO_SYMS) -T $(EXTRA_SYMS) \
                 --no-check-sections \
                 -Map $(BUILD_DIR)/pokemonsnap.us.map

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
		$(VENV_BIN)/python tools/postprocess_asm.py; \
		$(VENV_BIN)/python tools/gen_symbols_from_asm.py; \
	fi
	@mkdir -p $(ASM_DIR) $(ASSET_DIR) $(BUILD_DIR)

# ---- Build -------------------------------------------------------------------
#
# Round-trip pipeline:
#
#   asm/<addr>.s ──as──▶ build/asm/<addr>.o     ─┐
#                                                 ├──ld──▶ build/.elf ──objcopy──▶ build/.z64
#   assets/<n>.bin ──ld -r -b binary──▶ build/assets/<n>.o ─┘                          │
#                                                                                       ▼
#                                                                          sha1 vs sha1sums.txt
#
# When all input is asm + raw bin (i.e. no C is decomped yet), the output ROM
# must be byte-identical to baserom.us.z64. That is the gate every PR has to
# preserve: any change that breaks the SHA-1 either (a) introduced bytes that
# differ from the original or (b) shifted layout.

$(BUILD_DIR) $(BUILD_DIR)/asm $(BUILD_DIR)/assets:
	@mkdir -p $@

$(BUILD_DIR)/asm/%.o: $(ASM_DIR)/%.s | $(BUILD_DIR)/asm
	@$(MIPS_PREFIX)as $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/assets/%.o: $(ASSET_DIR)/%.bin | $(BUILD_DIR)/assets
	@$(MIPS_PREFIX)ld -r -b binary -o $@ $<

$(BUILD_DIR)/pokemonsnap.us.elf: $(ALL_OBJS) $(LDSCRIPT) | $(BUILD_DIR)
	@if [ -z "$(strip $(ALL_OBJS))" ]; then \
		echo "ERROR: no object files. Run 'make setup' first." >&2; exit 1; \
	fi
	@echo "  LD   $@"
	@$(MIPS_PREFIX)ld $(LDFLAGS) -o $@

$(TARGET_ROM): $(BUILD_DIR)/pokemonsnap.us.elf
	@echo "  GEN  $@"
	@$(MIPS_PREFIX)objcopy -O binary $< $@
	@expected=$$(awk '$$1 ~ /^[0-9a-f]{40}$$/ && $$2 == "baserom.us.z64" {print $$1; exit}' $(SHA1SUMS)); \
	 actual=$$(sha1sum $@ | awk '{print $$1}'); \
	 if [ "$$expected" = "$$actual" ]; then \
		echo "OK: $@ matches expected SHA-1 ($$expected)"; \
	 else \
		echo "MISMATCH:" >&2; \
		echo "  expected: $$expected" >&2; \
		echo "  got:      $$actual"  >&2; \
		exit 1; \
	 fi

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
