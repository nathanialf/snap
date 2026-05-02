# src/

Decompiled C source code. Tracked.

Every file here must:

- be original C written for this project (or in libultra-shaped layouts
  where libultra symbols are public),
- compile to bytes that match the corresponding region of the original
  ROM,
- contain no code copied from leaked source or other unauthorized
  reference material.

When a function does not yet match, leave it under `asm/nonmatchings/`
and link the asm via the appropriate include — do not commit
near-matching C and call it matched.
