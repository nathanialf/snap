# Contributing

Thanks for your interest in helping decompile *Pokémon Snap*. The project's
guiding principle is **slow, steady, matching progress** — every PR should
either:

- convert one or more functions from asm to byte-for-byte matching C, or
- improve tooling, build hygiene, or documentation, or
- make the build reproducible on a new platform.

## Hard rules

1. **Never commit ROM-derived data.** No `*.z64`, `*.n64`, `*.v64`, `*.bin`
   that contains ROM bytes, no extracted audio/textures/models, no symbol
   tables that were lifted whole from the original binary or from leaked
   materials. The `.gitignore` and `tools/check_no_rom.sh` are guardrails,
   not a substitute for thinking.
2. **Don't use leaked source code or leaked debug builds as a reference.**
   Match by reading your own legally-obtained ROM only. If you've ever read
   leaked material related to this game, please disclose that to the
   maintainers before contributing matching C — we may ask you to focus on
   tooling instead.
3. **No AI-generated decompilation that ingested copyrighted source.** Tools
   that disassemble or pseudo-decompile your own ROM are fine; tools trained
   on leaked source are not.
4. **Be byte-exact.** A "decompiled" function that doesn't produce identical
   bytes when assembled is a non-matching draft — file it under
   `asm/nonmatchings/` (which IS tracked) with notes, but do not call it
   matched.

## Picking a task

1. Run `make setup` so `asm/nonmatchings/` is populated for your ROM.
2. Browse `docs/PROGRESS.md` for the current per-overlay breakdown, or run
   `python3 tools/progress.py --list-unmatched` for a function-level list.
3. Smaller leaf functions are easier first targets than large state machines.
4. Open a draft PR early — even a single matching function is welcome.

## Workflow

```sh
# create a branch
git checkout -b feat/<area>-<short-name>

# work in src/, iterate
make                    # full build
make <object>.ok        # verify a single object matches

# before pushing
./tools/check_no_rom.sh # IP-safety scan of staged files
python3 tools/progress.py --update-readme   # if you matched anything
```

## Coding style

Style choices that don't change codegen are flexible; choices that DO change
codegen are dictated by what makes the function match. When in doubt:

- 4-space indent, no tabs.
- K&R braces.
- `snake_case` for local symbols we name ourselves; preserve any externally-
  documented public symbol names (e.g. libultra entry points) as-is.
- Headers in `include/`, one struct/system per header where reasonable.
- Don't add speculative abstractions. The point is to match the original
  binary, not to refactor it.

## Reviewing

Reviewers check for:
- matching SHA-1 of the produced ROM (CI also enforces this when a baserom
  is available in the runner — local PR runs may skip the link step),
- no committed ROM-derived data,
- no leaked-source contamination signals,
- progress.py output lines up with what the PR claims to have matched.
