"""
Credits: KooShnoo

objdiff allows one to map symbols from the target binary to the source binary.
this script will take those symbols you have mapped, and write them to `symbols.txt`.

use case: say you have changed the signature/name of a function.
now, your source file says `foo_Ful`, but symbols.txt says `bar__Fl`.
objdiff allows you to map the `foo_Ful` in the target object to `bar__Fl` in the source object.
this script allows you to replace `foo_Ful` with `bar__Fl` in `symbols.txt`.
"""

import json
import sys


def apply(game_version: str):
    SYMBOLS_TXT_PATH = f"./config/{game_version}/symbols.txt"

    with open("./objdiff.json") as f:
        objdiff_config = json.load(f)

    units: list = objdiff_config["units"]

    symbol_mappings = {}
    for unit in units:
        symbol_mapping = unit.get("symbol_mappings")
        if symbol_mapping is None:
            continue
        symbol_mappings.update(symbol_mapping)
        del unit["symbol_mappings"]

    with open(SYMBOLS_TXT_PATH) as f:
        symbols = f.readlines()

    for i, line in enumerate(symbols):
        tokens = line.split()
        old_symbol = tokens[0]

        new_symbol = symbol_mappings.get(old_symbol)

        if new_symbol is None or "gprlr" in new_symbol:
            continue

        tokens[0] = new_symbol
        symbols[i] = " ".join(tokens) + "\n"

    with open(SYMBOLS_TXT_PATH, "w") as f:
        f.writelines(symbols)

    with open("./objdiff.json", "w") as f:
        json.dump(objdiff_config, f)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Expected usage: {0} <game version>".format(sys.argv[0]))
        sys.exit(1)

    apply(sys.argv[1])
