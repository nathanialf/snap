#!/usr/bin/env bash
# quick_diff.sh — fast inner-loop diff for matching iteration.
#
# Usage: tools/quick_diff.sh <func_name>
#
# Locates the C source for <func_name> (src/<name>.c, falling back to
# tough_nuts/<name>/<name>.c), compiles it with IDO 7.1 using the same
# CFLAGS the Makefile would, disassembles the resulting .o with
# mips-linux-gnu-objdump, and side-by-side diffs it against the
# corresponding glabel block in asm/<rom>.s.
#
# This skips `make split` and `make` — no yaml flip, no full-ROM SHA-1
# check. Runs in ~1 second; full make-split + link runs ~30-60s on
# this machine. Read-only w.r.t. the repo (writes only to a tmpdir).
#
# Exit status: 0 if the function matches byte-for-byte, 1 otherwise.

set -u

PROG="$(basename "$0")"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

die() { echo "$PROG: error: $*" >&2; exit 2; }

[ $# -eq 1 ] || die "usage: $PROG <func_name>"
FN="$1"

case "$FN" in
    func_*) ;;
    *) die "function name must start with 'func_': $FN" ;;
esac

# ---- 1. Locate C source ----------------------------------------------------

if   [ -f "$ROOT/src/$FN.c" ];                   then SRC="$ROOT/src/$FN.c"
elif [ -f "$ROOT/tough_nuts/$FN/$FN.c" ];        then SRC="$ROOT/tough_nuts/$FN/$FN.c"
else die "no C source found at src/$FN.c or tough_nuts/$FN/$FN.c"
fi

# ---- 2. Locate the asm file containing this function ----------------------

ASM_HIT="$(grep -l -E "^nonmatching $FN, 0x[0-9A-Fa-f]+" "$ROOT"/asm/*.s 2>/dev/null | head -1)"
[ -n "$ASM_HIT" ] || die "no 'nonmatching $FN, 0xSIZE' line found in asm/*.s"

# ---- 3. Pick CFLAGS based on the Makefile's per-file rules ----------------

IDO_VERSION="${IDO_VERSION:-7.1}"
IDO_DIR="${IDO_DIR:-$HOME/.local/share/ido-recomp/build/$IDO_VERSION/out}"
IDO_CC="$IDO_DIR/cc"
[ -x "$IDO_CC" ] || die "IDO compiler not found at $IDO_CC (set IDO_DIR=)"

ULTRA_INC="-I$ROOT/lib/ultralib/include -I$ROOT/lib/ultralib/include/PR -I$ROOT/lib/ultralib/include/PRinternal"
COMMON="-c -G 0 -non_shared -Xfullwarn -fullwarn -O2 -mips2 -woff 649,838,712,807 -signed -nostdinc -I$ROOT/include"

src_basename="$(basename "$SRC")"
case "$src_basename" in
    ultra_libc_ll*.c)
        CFLAGS="-c -G 0 -non_shared -Xfullwarn -Xcpluscomm -fullwarn -O1 -mips3 -32 -woff 649,838,712,807,516 -signed -nostdinc -I$ROOT/include $ULTRA_INC" ;;
    ultra_os_*.c|ultra_log_*.c)
        CFLAGS="-c -G 0 -non_shared -Xfullwarn -Xcpluscomm -fullwarn -O1 -mips2 -woff 649,838,712,807,516 -signed -nostdinc -I$ROOT/include $ULTRA_INC" ;;
    ultra_*.c)
        CFLAGS="-c -G 0 -non_shared -Xfullwarn -Xcpluscomm -fullwarn -O2 -mips2 -woff 649,838,712,807,516 -signed -nostdinc -I$ROOT/include $ULTRA_INC" ;;
    *)
        CFLAGS="$COMMON" ;;
esac

# ---- 4. Compile to .o ------------------------------------------------------

TMPDIR="$(mktemp -d -t quick_diff.XXXXXX)"
trap 'rm -rf "$TMPDIR"' EXIT
OBJ="$TMPDIR/$FN.o"

if ! "$IDO_CC" $CFLAGS -o "$OBJ" "$SRC" 2>"$TMPDIR/cc.err"; then
    cat "$TMPDIR/cc.err" >&2
    die "compile failed"
fi
# Print warnings (but don't fail) so the user sees them.
[ -s "$TMPDIR/cc.err" ] && cat "$TMPDIR/cc.err" >&2

# ---- 5. Build the "candidate" listing from the .o -------------------------
#
# Format every line as: OFFSET: ENCODING MNEMONIC
#   0000: 27bdffe8 addiu sp,sp,-24
# Strip relocation lines (R_MIPS_*) — they describe link-time fixups
# the .s file doesn't carry as separate lines. Strip section headers,
# blank lines, the "<func>:" label.

CAND="$TMPDIR/candidate.txt"
RELOCS="$TMPDIR/relocs.txt"   # offset(hex,4) -> reloc-kind (HI16/LO16/26)
mips-linux-gnu-objdump -d -r "$OBJ" \
  | awk -v relocs="$RELOCS" '
      /^Disassembly/ || /file format/ || /^$/ { next }
      /^[0-9a-f]+ <.*>:$/ { next }
      # Reloc lines look like: "\t\t\t10: R_MIPS_HI16\tD_8004873C"
      /R_MIPS_/ {
          line = $0
          sub(/^[[:space:]]+/, "", line)
          n = index(line, ":")
          if (n == 0) next
          off = substr(line, 1, n-1)
          while (length(off) < 4) off = "0" off
          rest = substr(line, n+2)
          sub(/^[[:space:]]+/, "", rest)
          # rest = "R_MIPS_HI16\tD_..."
          if (match(rest, /R_MIPS_[A-Z0-9_]+/) > 0) {
              kind = substr(rest, RSTART, RLENGTH)
              print off "\t" kind > relocs
          }
          next
      }
      /^[[:space:]]/ {
          line = $0
          sub(/^[[:space:]]+/, "", line)
          n = index(line, ":")
          if (n == 0) next
          off = substr(line, 1, n-1)
          rest = substr(line, n+2)
          sub(/^[[:space:]]+/, "", rest)
          enc = rest
          sub(/[[:space:]].*$/, "", enc)
          mnem = rest
          sub(/^[^[:space:]]+[[:space:]]+/, "", mnem)
          gsub(/[[:space:]]+/, " ", mnem)
          while (length(off) < 4) off = "0" off
          printf "%s: %s %s\n", off, enc, mnem
      }
  ' > "$CAND" || die "objdump pipeline failed"
touch "$RELOCS"

[ -s "$CAND" ] || die "no instructions extracted from $OBJ"

# ---- 6. Build the target listing from asm/*.s ------------------------------
#
# asm lines look like:
#   /* 1AF60 8011A360 27BDFFE8 */  addiu       $sp, $sp, -0x18
# We want OFFSET (relative to first insn): ENCODING (lowercased) MNEMONIC.

TARGET="$TMPDIR/target.txt"
# Lines look like:
#   /* 1AF60 8011A360 27BDFFE8 */  addiu       $sp, $sp, -0x18
# or label-only lines:
#     .L8011A3B8:
# We extract only insn lines (the /* ... */ ones), and number them
# sequentially: idx*4 -> offset.
awk -v fn="$FN" '
    BEGIN { in_block = 0; idx = 0 }
    $0 == "glabel " fn { in_block = 1; next }
    in_block && $0 == "endlabel " fn { in_block = 0; exit }
    in_block {
        # Strip leading whitespace.
        line = $0
        sub(/^[[:space:]]+/, "", line)
        # Must start with /*
        if (substr(line, 1, 2) != "/*") next
        # Find the closing */
        end = index(line, "*/")
        if (end == 0) next
        comment = substr(line, 3, end - 3)
        rest = substr(line, end + 2)
        sub(/^[[:space:]]+/, "", rest)
        sub(/[[:space:]]+$/, "", rest)
        # Comment is "ROMOFF VRAM ENCODING".
        # Split by whitespace.
        n = split(comment, parts, /[[:space:]]+/)
        # parts may have a leading empty due to leading space.
        # Reassemble into pure tokens.
        tokc = 0
        for (i = 1; i <= n; i++) if (parts[i] != "") { tokc++; tok[tokc] = parts[i] }
        if (tokc < 3) next
        enc = tok[3]
        # rest is "mnem<spaces>operands"
        gsub(/[[:space:]]+/, " ", rest)
        sp = index(rest, " ")
        if (sp == 0) { mnem = rest; ops = "" }
        else         { mnem = substr(rest, 1, sp-1); ops = substr(rest, sp+1) }
        gsub(/, /, ",", ops)
        gsub(/\$/, "", ops)
        # offset = idx * 4, formatted as 4 hex digits
        # Build hex without printf %x (mawk supports %x but be safe)
        v = idx * 4
        hex = ""
        if (v == 0) hex = "0"
        else {
            tmp = v
            while (tmp > 0) {
                d = tmp % 16
                tmp = int(tmp / 16)
                if (d < 10) hex = d hex
                else hex = substr("abcdef", d - 9, 1) hex
            }
        }
        while (length(hex) < 4) hex = "0" hex
        printf "%s: %s %s%s%s\n", hex, tolower(enc), mnem, (ops == "" ? "" : " "), ops
        idx++
    }
' "$ASM_HIT" > "$TARGET"

[ -s "$TARGET" ] || die "no instructions extracted from $ASM_HIT for $FN"

# ---- 7. Diff. The encoding column is what really matters for matching;
#         mnemonics/operands may format slightly differently between
#         spimdisasm and objdump, but if the 4 bytes match the function
#         matches.
#
# Compare the encoding-only projection for the pass/fail verdict, but
# show the full side-by-side for the human.

CAND_ENC="$TMPDIR/cand_enc.txt"
TGT_ENC="$TMPDIR/tgt_enc.txt"
# Mask the immediate field of any instruction that carries a relocation
# (HI16, LO16, or R_MIPS_26 jump-target). Without linking, the candidate's
# imm field is zero; the target asm's imm field has the linker-resolved
# value. Mask both sides identically so we compare the *opcode + register
# fields*, which is what matching actually cares about.
mask_enc() {
    # $1 = listing file (lines: "OFFS: ENC mnem...")
    # $2 = relocs file (lines: "OFFS\tKIND")
    awk -v relocs="$2" '
        BEGIN {
            while ((getline line < relocs) > 0) {
                split(line, a, "\t")
                kinds[a[1]] = a[2]
            }
        }
        {
            off = $1; sub(/:$/, "", off)
            enc = $2
            kind = kinds[off]
            if (kind == "R_MIPS_HI16" || kind == "R_MIPS_LO16") {
                # zero the low 16 bits — first 4 hex digits stay, last 4 -> "0000"
                enc = substr(enc, 1, 4) "0000"
            } else if (kind == "R_MIPS_26") {
                # opcode (top 6 bits = 2 hex digits + top 2 bits of next nibble);
                # bottom 26 bits = 6+ hex digits we have to zero. Simplest: keep
                # the high nibble, zero the rest. The opcode is in the top 6 bits;
                # for jal/j the top byte is 0x0c/0x08, so masking to "0c000000" /
                # "08000000" is safe (top 6 bits live entirely in the first 2 hex
                # digits given jal=0x0c, j=0x08, both have low 2 bits clear).
                enc = substr(enc, 1, 2) "000000"
            }
            print enc
        }
    ' "$1"
}
mask_enc "$CAND"   "$RELOCS" > "$CAND_ENC"
mask_enc "$TARGET" "$RELOCS" > "$TGT_ENC"

NCAND=$(wc -l < "$CAND_ENC")
NTGT=$(wc -l < "$TGT_ENC")

echo "== quick_diff: $FN"
echo "==   src:   $SRC"
echo "==   asm:   $ASM_HIT"
echo "==   insns: candidate=$NCAND  target=$NTGT"
echo

# Width that fits both sides without wrapping in an 80-col-ish terminal.
WIDTH="${QUICK_DIFF_WIDTH:-160}"

if cmp -s "$CAND_ENC" "$TGT_ENC"; then
    echo "== MATCH (encoding bytes identical, $NCAND insns)"
    echo
    # Still show the side-by-side so the user can sanity-check.
    diff -y -W "$WIDTH" "$TARGET" "$CAND" || true
    exit 0
fi

echo "== DIFF (encoding bytes differ)"
echo "==   left = target asm   right = candidate (compiled .c)"
echo
diff -y -W "$WIDTH" "$TARGET" "$CAND" || true

# Compact bucket: count differing-encoding rows.
NDIFF=$(diff "$TGT_ENC" "$CAND_ENC" | grep -cE '^[<>]') || true
echo
echo "== $NDIFF differing line(s) on encoding column"
exit 1
