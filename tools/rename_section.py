#!/usr/bin/env python3

###
# Renames ELF sections of a relocatable object in place.
#
# Some translation units were compiled into a non-default output section by the
# original build (the GameCube overlay, `.over`, is the case in this project:
# see the `*zFEOverlay.o(.over .rodata)` line in config/GOWE69/ldscript.ld).
# dtk therefore extracts the target object with its code in `.over`, while our
# compiler emits plain `.text`.  objdiff pairs symbols per section, so without
# this step every function of such a unit is invisible to the progress report.
#
# The rename is a pure byte patch of `.shstrtab`: only names of the same length
# are accepted, so no offset, size or index in the object moves.  The relocation
# section that belongs to a renamed section (`.rela<name>` / `.rel<name>`) is
# renamed along with it.
#
# Usage:
#   python3 tools/rename_section.py <object.o> <old>=<new> [<old>=<new> ...]
#   python3 tools/rename_section.py <object.o> .text=.over -o <output.o>
###

import argparse
import struct
import sys
from pathlib import Path
from typing import Dict, List, Tuple


def parse_renames(specs: List[str]) -> List[Tuple[str, str]]:
    renames: List[Tuple[str, str]] = []
    for spec in specs:
        # Allow a single argument to carry several comma separated pairs.
        for part in spec.split(","):
            part = part.strip()
            if not part:
                continue
            if "=" not in part:
                sys.exit(f"Invalid section rename {part!r}, expected <old>=<new>")
            old, new = part.split("=", 1)
            if len(old) != len(new):
                sys.exit(
                    f"Section names must have the same length: {old!r} vs {new!r}"
                )
            renames.append((old, new))
    return renames


def rename_sections(data: bytearray, renames: List[Tuple[str, str]]) -> int:
    if data[:4] != b"\x7fELF":
        sys.exit("Not an ELF file")
    if data[4] != 1:
        sys.exit("Only 32-bit ELF objects are supported")
    endian = ">" if data[5] == 2 else "<"

    (shoff,) = struct.unpack_from(endian + "I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from(endian + "3H", data, 0x2E)
    if shoff == 0 or shnum == 0:
        sys.exit("Object has no section headers")
    (shstrtab_off,) = struct.unpack_from(
        endian + "I", data, shoff + shstrndx * shentsize + 16
    )

    # A renamed section drags its relocation section along.
    targets: Dict[str, str] = {}
    for old, new in renames:
        targets[old] = new
        targets[".rela" + old] = ".rela" + new
        targets[".rel" + old] = ".rel" + new

    hits = 0
    for i in range(shnum):
        (name_off,) = struct.unpack_from(endian + "I", data, shoff + i * shentsize)
        start = shstrtab_off + name_off
        end = data.index(b"\0", start)
        name = data[start:end].decode("ascii", "replace")
        new_name = targets.get(name)
        if new_name is None:
            continue
        # Same length by construction, so this cannot disturb any other name
        # that shares this suffix in the string table.
        data[start : start + len(new_name)] = new_name.encode("ascii")
        hits += 1
    return hits


def main() -> None:
    parser = argparse.ArgumentParser(description="Rename ELF sections in place.")
    parser.add_argument("object", type=Path, help="relocatable object to patch")
    parser.add_argument(
        "renames", nargs="+", metavar="OLD=NEW", help="section rename, e.g. .text=.over"
    )
    parser.add_argument(
        "-o", "--output", type=Path, help="write here instead of patching in place"
    )
    parser.add_argument(
        "-q", "--quiet", action="store_true", help="do not print what was renamed"
    )
    args = parser.parse_args()

    renames = parse_renames(args.renames)
    data = bytearray(args.object.read_bytes())
    hits = rename_sections(data, renames)
    (args.output or args.object).write_bytes(data)
    if not args.quiet:
        pairs = ", ".join(f"{old} -> {new}" for old, new in renames)
        print(f"{args.object}: renamed {hits} section(s) ({pairs})")


if __name__ == "__main__":
    main()
