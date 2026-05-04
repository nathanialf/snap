# Progress

This file is **regenerated** by `tools/progress.py` after a clean build. Do
not hand-edit the tables; edit narrative sections (top of file, "How
progress is measured") only.

## How progress is measured

We track three numbers per ROM revision:

- **Decompiled / matched code** — bytes of executable text that come from
  C in `src/` whose compiled output matches the original byte-for-byte.
- **Asm split** — bytes of executable text that splat has extracted into
  `asm/` (gitignored, regenerated). This is a function of the splat config,
  not of contributor work, but rises as we cover more overlays in
  `config/snap.us.yaml`.
- **Assets extracted** — bytes of non-code data (textures, audio, level
  data) that splat is able to round-trip cleanly. Assets do not need to be
  "decompiled" in the C sense, but they do need to be placed in the build
  such that the rebuilt ROM matches.

Numerator and denominator both come from the user's ROM at build time;
nothing here embeds ROM bytes.

## Current status (US 1.0)

| Metric              | Bytes |    % |
|---------------------|------:|-----:|
| Decompiled (matched)| 19292 | 0.11 |
| Asm split           | 16986461 | 99.89 |
| Assets extracted    |  4032 | --   |

_No baseline run has produced numbers yet — this table updates after the
first successful `make setup` + `python3 tools/progress.py --update`._

## Per-overlay breakdown (US 1.0)

_(Populated by `tools/progress.py` once `config/snap.us.yaml` lists overlays.)_

## How to update this file

```sh
make setup            # ensures asm/ and build/ are current
make                  # builds whatever is currently decompiled
python3 tools/progress.py --update  # rewrites tables in this file and README
```

`--update` is also wired into a CI job that runs nightly against a
maintainer-provided baserom, so PRs do not need to update progress numbers
themselves.
