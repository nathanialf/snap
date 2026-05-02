# Legal notes

This is a community decompilation effort. It is not affiliated with, endorsed
by, or sponsored by Nintendo Co., Ltd., The Pokémon Company, Creatures Inc.,
GAME FREAK Inc., or any of their affiliates. *Pokémon* and *Pokémon Snap*
are trademarks of their respective owners.

## What is in this repository

- Original C source code we have written from scratch as part of the
  decompilation effort.
- Build scripts and tooling we have written.
- Documentation we have written.
- Configuration files (linker scripts, splat YAML, symbol *names* we
  invented) that describe how a user-supplied ROM is split and rebuilt.

## What is NOT in this repository, and must never be added

- The Pokémon Snap ROM, in whole or in any portion, in any encoding.
- Audio, image, texture, model, text, or other assets extracted from the
  ROM.
- Symbol tables, debug strings, or other identifiers lifted directly from
  the original binary or from leaked development materials.
- Any code believed to have been derived from leaked source code.

The `.gitignore` and `tools/check_no_rom.sh` script make it harder to commit
such material accidentally, but they are not a substitute for contributor
diligence. If you suspect a file in this repo violates these rules, open an
issue tagged `legal` immediately — maintainers will treat it as urgent.

## What you need to do to build

You must legally own a copy of *Pokémon Snap* for the Nintendo 64 and supply
the ROM file yourself. The build verifies a SHA-1 hash against your provided
file before proceeding; if you do not provide a valid file, the build will
refuse to run. The supported revisions and their SHA-1 fingerprints are
listed in `docs/BUILDING.md`. ROM cryptographic hashes are not themselves
copyrighted — they are fingerprints used to confirm an exact match — but the
ROM file itself remains the property of its rightsholders.

This project does not provide instructions on how to obtain a ROM file. The
only legitimate source is a personal dump of a cartridge you own, performed
in compliance with the law of your jurisdiction.

## Why decompilation projects can exist

Decompilation projects of this kind have an established history (e.g. Super
Mario 64, Ocarina of Time, Majora's Mask, Paper Mario, Pokémon Red/Blue/
Crystal/Ruby/Emerald, and many more). They produce *original source code* as
a clean reverse-engineering exercise, distribute *no copyrighted assets*,
and require the user to bring their own ROM. We follow the same model and
the same care.

If you are a rightsholder representative and have a concern, please open a
GitHub issue; the maintainers will respond promptly.
