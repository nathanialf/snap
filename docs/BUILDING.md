# Building

This page walks through producing a byte-for-byte matching `pokemonsnap.us.z64`
from your own legally-obtained ROM and the source in this repository.

## 1. Prerequisites

Linux is the primary supported host. macOS and Windows-via-WSL2 are expected
to work but are validated less often.

| Tool                | Why                              | Install hint              |
|---------------------|----------------------------------|---------------------------|
| Python 3.10+        | splat, progress, IP-safety check | system package manager    |
| GNU Make            | top-level orchestration          | system package manager    |
| MIPS GCC toolchain  | compile and link the ROM         | see `tools/setup.sh`      |
| `binutils` for MIPS | objcopy, ld for MIPS             | shipped with the toolchain |
| `git`               | submodules: splat, etc.          | system package manager    |

After cloning, run:

```sh
./tools/setup.sh
```

This script installs Python dependencies into a local `.venv`, fetches the
splat / asm-differ / etc. submodules, and (optionally) downloads a known-good
MIPS toolchain into `tools/toolchain/`.

## 2. Provide your ROM

Copy your own ROM to the project root and rename it according to the revision
you have:

| Revision   | File name             |
|------------|-----------------------|
| US 1.0     | `baserom.us.z64`      |
| Europe 1.0 | `baserom.eu.z64`      |
| Japan 1.0  | `baserom.jp.z64`      |

Currently only **US 1.0** is targeted by the build. Other revisions may be
added later — see `config/`.

The file must be in big-endian `.z64` format. If your dump is `.n64` (little-
endian) or `.v64` (byte-swapped), convert it first; many emulators and
`tools/n64_to_z64.py` (a thin wrapper around standard byte-reordering) can do
this.

### SHA-1 verification

The first thing the build does is hash your ROM and compare against the
expected fingerprint for that revision. The expected hashes are listed in
`config/sha1sums.txt`. If your hash does not match, **do not** edit the
sha1sums file — the build is telling you the ROM is not the supported
revision, and the rest of the project (offsets, splits, expected output)
will not apply.

If a `sha1sums.txt` entry is empty, the project does not yet target that
revision. PRs adding support are welcome.

## 3. Configure

```sh
make setup
```

This runs splat against your ROM and `config/snap.us.yaml`, populating:

- `asm/` — split assembly listings (gitignored)
- `assets/` — extracted binary blobs (gitignored)
- `build/` — empty initially

Anything under `asm/` and `assets/` is regenerated from your ROM and is not
part of the repository. **Do not commit it.**

## 4. Build

```sh
make           # full build
make -jN       # parallel
make clean     # wipe build/
```

A successful build produces `build/pokemonsnap.us.z64`. The Makefile then
re-checks its SHA-1 against the expected fingerprint and prints `OK` on
match. Until the project reaches 100%, the matching check applies only to
the regions of the ROM that have been decompiled — early on, the build
will produce a ROM but it will not yet match the original.

## 5. Common problems

- **`make setup` fails with "ROM hash mismatch"** — your ROM file is the
  wrong revision, the wrong region, or has been modified (some dumping
  tools insert a header). Re-dump from your own cartridge.
- **Toolchain not found** — `./tools/setup.sh` did not finish, or your
  `PATH` does not include `tools/toolchain/bin`. Re-run setup, or pass
  `MIPS_PREFIX=/path/to/your/mips-elf-` to `make`.
- **Build output is non-matching even though you didn't change anything** —
  please open an issue; this usually means a host-toolchain mismatch.

## 6. Running the result

This project does not ship an emulator. Once you have a matching
`build/pokemonsnap.us.z64`, you can run it the same way you ran your input
ROM. The output ROM will be identical to the input on a matched build, so
there is no functional reason to "play" it from the build directory; it
exists to prove the decompilation matches.
