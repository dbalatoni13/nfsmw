#!/usr/bin/env python3
"""Rename .text section to a custom name in an ELF .o file.
Usage: python tools/rename_section.py <elf_file> <old_name> <new_name>

Both names must be the same length (byte-level string table replacement).
"""
import struct
import sys

def rename_section(path, old_name, new_name):
    assert len(old_name) == len(new_name), f"Names must be same length: {old_name!r} vs {new_name!r}"
    with open(path, 'rb') as f:
        data = bytearray(f.read())
    assert data[:4] == b'\x7fELF', "Not an ELF file"
    endian = '>' if data[5] == 2 else '<'
    e_shoff = struct.unpack_from(endian + 'I', data, 32)[0]
    e_shstrndx = struct.unpack_from(endian + 'H', data, 50)[0]
    e_shentsize = struct.unpack_from(endian + 'H', data, 46)[0]
    strtab_off = e_shoff + e_shstrndx * e_shentsize
    strtab_sh_offset = struct.unpack_from(endian + 'I', data, strtab_off + 16)[0]
    strtab_sh_size = struct.unpack_from(endian + 'I', data, strtab_off + 20)[0]
    strtab = data[strtab_sh_offset:strtab_sh_offset + strtab_sh_size]
    needle = old_name.encode() + b'\x00'
    pos = strtab.find(needle)
    if pos < 0:
        return  # section not present, nothing to do
    abs_pos = strtab_sh_offset + pos
    data[abs_pos:abs_pos + len(old_name)] = new_name.encode()
    with open(path, 'wb') as f:
        f.write(data)

if __name__ == '__main__':
    rename_section(sys.argv[1], sys.argv[2], sys.argv[3])
