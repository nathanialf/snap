"""asm-differ configuration for Pokémon Snap.

Workflow:
    1. After a green `make`, snapshot the build into expected/:
           tools/make_expected.sh
    2. Edit src/<func>.c (or tough_nuts/<func>/<func>.c).
    3. Run the watch-and-diff loop:
           .venv/bin/python lib/ultralib/tools/asm_differ/diff.py -mo -w func_NAME

The `-o` flag diffs .o files (preserves symbol names). `-m` invokes make
on the .o automatically. `-w` watches for source-file changes and
re-runs the diff on save — paired with editing the C source it makes
match iteration interactive.
"""


def apply(config, args):
    config["baseimg"] = "baserom.us.z64"
    config["myimg"] = "build/pokemonsnap.us.z64"
    config["mapfile"] = "build/pokemonsnap.us.map"
    config["expected_dir"] = "expected/build/"
    config["source_directories"] = ["src", "include", "tough_nuts"]
    config["arch"] = "mips"
    config["map_format"] = "gnu"
    config["makeflags"] = []
