"""
Credits: KooShnoo

Applies a csv file which was created in Ghidra's function list onto symbols.txt
"""

import json
import csv
import sys


def apply(game_version: str):
    SYMBOLS_TXT_PATH = f"./config/{game_version}/symbols.txt"

    with open("./symbols/xenon_symbols.csv") as f:
        ghidra_symbols = list(csv.DictReader(f))

    ghidra_address_to_name = {}
    for symbol in ghidra_symbols:
        ghidra_address_to_name["0x" + symbol["Location"]] = symbol["Name"]

    with open(SYMBOLS_TXT_PATH) as f:
        symbols = f.readlines()

    for i, line in enumerate(symbols):
        tokens = line.split()
        address = tokens[2].split(":")[1].removesuffix(";")
        new_symbol = ghidra_address_to_name.get(address)

        if new_symbol is None:
            continue

        tokens[0] = new_symbol
        symbols[i] = " ".join(tokens) + "\n"

    with open(SYMBOLS_TXT_PATH, "w") as f:
        f.writelines(symbols)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Expected usage: {0} <game version>".format(sys.argv[0]))
        sys.exit(1)

    apply(sys.argv[1])
