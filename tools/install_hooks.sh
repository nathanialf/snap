#!/usr/bin/env bash
# Install the IP-safety scan as a git pre-commit hook (opt-in).
# Re-run any time tools/check_no_rom.sh is updated.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
HOOK="$ROOT/.git/hooks/pre-commit"

cat > "$HOOK" <<'EOF'
#!/usr/bin/env bash
# Auto-installed by tools/install_hooks.sh — runs the IP-safety scan on
# staged files before each commit. Bypass with --no-verify only after
# convincing yourself a hit is a real false positive.
exec "$(git rev-parse --show-toplevel)/tools/check_no_rom.sh"
EOF
chmod +x "$HOOK"
echo "Installed pre-commit hook at $HOOK"
