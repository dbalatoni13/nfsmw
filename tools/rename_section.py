#!/usr/bin/env python3
"""Rename ELF section names by editing the section-string table in-place.

Usage:
  python tools/rename_section.py <elf_file> <old_name> <new_name>
  python tools/rename_section.py <input_elf> <output_elf> <old1> <new1> [<old2> <new2> ...]

Section names must stay the same length because this is a byte-for-byte string-table
replacement used for objdiff-only post-processing.
"""
import struct
import sys

def rename_sections(input_path, output_path, renames):
    for old_name, new_name in renames:
        assert len(old_name) == len(new_name), (
            f"Names must be same length: {old_name!r} vs {new_name!r}"
        )

    with open(input_path, "rb") as f:
        data = bytearray(f.read())
    assert data[:4] == b"\x7fELF", "Not an ELF file"
    endian = ">" if data[5] == 2 else "<"
    e_shoff = struct.unpack_from(endian + "I", data, 32)[0]
    e_shstrndx = struct.unpack_from(endian + "H", data, 50)[0]
    e_shentsize = struct.unpack_from(endian + "H", data, 46)[0]
    strtab_off = e_shoff + e_shstrndx * e_shentsize
    strtab_sh_offset = struct.unpack_from(endian + "I", data, strtab_off + 16)[0]
    strtab_sh_size = struct.unpack_from(endian + "I", data, strtab_off + 20)[0]
    strtab = data[strtab_sh_offset : strtab_sh_offset + strtab_sh_size]

    for old_name, new_name in renames:
        needle = old_name.encode() + b"\x00"
        pos = strtab.find(needle)
        if pos < 0:
            continue
        abs_pos = strtab_sh_offset + pos
        data[abs_pos : abs_pos + len(old_name)] = new_name.encode()

    with open(output_path, "wb") as f:
        f.write(data)

def main(argv):
    if len(argv) == 4:
        rename_sections(argv[1], argv[1], [(argv[2], argv[3])])
        return

    if len(argv) >= 5 and (len(argv) - 3) % 2 == 0:
        renames = list(zip(argv[3::2], argv[4::2]))
        rename_sections(argv[1], argv[2], renames)
        return

    raise SystemExit(
        "Usage:\n"
        "  python tools/rename_section.py <elf_file> <old_name> <new_name>\n"
        "  python tools/rename_section.py <input_elf> <output_elf> "
        "<old1> <new1> [<old2> <new2> ...]"
    )

if __name__ == "__main__":
    main(sys.argv)
