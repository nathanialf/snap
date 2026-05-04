#!/usr/bin/env bash
# Install the IP-safety scan as a git pre-commit hook (opt-in).
# Re-run any time tools/check_no_rom.sh or tools/check_yaml_match.sh changes.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
HOOK="$ROOT/.git/hooks/pre-commit"

cat > "$HOOK" <<'EOF'
#!/usr/bin/env bash
# Auto-installed by tools/install_hooks.sh — runs:
#   1. tools/check_no_rom.sh     (IP-safety scan on staged files)
#   2. tools/check_yaml_match.sh (false-match audit: src/.c not linked)
# Bypass with --no-verify only after convincing yourself a hit is a real
# false positive.
set -e
ROOT="$(git rev-parse --show-toplevel)"
"$ROOT/tools/check_no_rom.sh"
# Only run yaml-match check if the linker script exists (fresh clones may not).
[ -f "$ROOT/config/pokemonsnap.us.ld" ] && "$ROOT/tools/check_yaml_match.sh"
EOF
chmod +x "$HOOK"
echo "Installed pre-commit hook at $HOOK"
