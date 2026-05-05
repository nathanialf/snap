# asm/

This directory is **populated at build time** by splat from your local
ROM. Its contents (other than this README, `.gitkeep`, and the
`nonmatchings/` subtree) are gitignored and **must never be committed**.

If you see committed `.s` files at the top of this tree, treat that as
a bug — open an issue tagged `legal` so they can be removed.

## What lives here after `make split`

- `asm/<segment>/...s` — split assembly listings, one file per source-style
  unit, generated from your ROM. Regenerated on every `make split`.
- `asm/data/<segment>/...s` — split data sections.

## What is committed

- `asm/nonmatchings/` — hand-written non-matching drafts. These are stubs
  produced by contributors while working toward a matching C version, and
  contain only assembly that the contributor wrote (typically refactored
  forms of what splat emitted). They are tracked so progress is shareable.
