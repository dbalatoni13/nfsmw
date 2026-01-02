#!/usr/bin/env python3

###
# Generates the necessary hashes used in the code.
#
# Usage:
#   python3 tools/hasher.py src/Speed/Indep/SourceLists/z<>.cpp src/Speed/Indep/Src/Generated/Hashes/z<>.h
#
###

import argparse
from ctypes import c_uint32
from pathlib import Path
import re

HEADER_PREFIX = """#ifndef HEADER_NAME_H
#define HEADER_NAME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

"""

include_pattern = re.compile(r'\s*#\s*include "([^"]+)"')
hash_pattern = re.compile(r"BINHASH\(([^)]+)\)")


def bHash(string: str):
    if not string:
        return 0

    result = c_uint32(0xFFFFFFFF)

    for c in string:
        result = c_uint32(result.value * 33 + ord(c))

    return result.value


def generate_header(in_file: Path) -> str:
    out_text = ""

    header_name = (
        "GENERATED_BINHASH_"
        + in_file.name.split(".")[0].replace("/", "_").replace(".", "_").upper()
    )

    out_text = HEADER_PREFIX.replace("HEADER_NAME", header_name)

    strings: list[str] = []

    sourcelist_content = in_file.read_text(encoding="UTF-8")
    for file_match in include_pattern.finditer(sourcelist_content):
        source_file_path = Path("src") / file_match.group(1)
        source_file_content = source_file_path.read_text(encoding="UTF-8")
        for hash_match in hash_pattern.finditer(source_file_content):
            # spaces are encoded as $$ because macro names can't contain spaces
            strings.append(hash_match.group(1))

    for string in strings:
        normalized = string.replace("$$", " ")
        out_text += f"#define BINHASH_{string} ({hex(bHash(normalized))})\n"

    out_text += "\n"
    out_text += "#endif"

    return out_text


def main() -> None:
    parser = argparse.ArgumentParser(
        description="""Generates the necessary hashes used in the code"""
    )
    parser.add_argument(
        "sourcelist_file",
        help="""Source list file in""",
    )
    parser.add_argument(
        "header_out",
        help="""Header file out""",
    )
    args = parser.parse_args()

    output = generate_header(Path(args.sourcelist_file))

    with open(args.header_out, "w", encoding="UTF-8") as f:
        f.write(output)


if __name__ == "__main__":
    main()
