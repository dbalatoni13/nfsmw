#!/usr/bin/env python3
"""
Look up string or raw bytes in an ELF by virtual address.

Examples:
  python tools/elf_lookup.py 0x803E58F4
  python tools/elf_lookup.py 803E58F4 --mode bytes --length 32
  python tools/elf_lookup.py 0x002F1234 --game ps2
  python tools/elf_lookup.py 0x803E58F4 --elf orig/GOWE69/NFSMWRELEASE.ELF
"""

import argparse
import os
import string
import sys
from typing import Any, Dict, Optional

from elftools.elf.elffile import ELFFile

from _common import ROOT_DIR, ToolError


DEFAULT_ELF_BY_GAME = {
    "gc": os.path.join(ROOT_DIR, "orig", "GOWE69", "NFSMWRELEASE.ELF"),
    "ps2": os.path.join(ROOT_DIR, "orig", "SLES-53558-A124", "NFS.ELF"),
}


def parse_address(raw: str) -> int:
    try:
        return int(raw, 16)
    except ValueError as exc:
        raise ToolError(f"invalid hex address: {raw}") from exc


def choose_elf_path(args: argparse.Namespace) -> str:
    if args.elf:
        path = args.elf
        if not os.path.isabs(path):
            path = os.path.join(ROOT_DIR, path)
        return os.path.abspath(path)
    return DEFAULT_ELF_BY_GAME[args.game]


def find_section_for_address(elffile: ELFFile, address: int) -> Optional[Dict[str, Any]]:
    for section in elffile.iter_sections():
        header = section.header
        start = int(header["sh_addr"])
        size = int(header["sh_size"])
        if size <= 0:
            continue
        end = start + size
        if start <= address < end:
            return {
                "name": section.name,
                "address": start,
                "size": size,
                "offset": int(header["sh_offset"]),
                "data": section.data(),
            }
    return None


def read_c_string(data: bytes, start_offset: int, max_bytes: int) -> bytes:
    blob = data[start_offset : start_offset + max_bytes]
    terminator = blob.find(b"\x00")
    if terminator != -1:
        blob = blob[:terminator]
    return blob


def printable_ratio(blob: bytes) -> float:
    if not blob:
        return 1.0
    printable = set(string.printable.encode("ascii"))
    hits = sum(1 for byte in blob if byte in printable and byte not in b"\x0b\x0c")
    return hits / len(blob)


def decode_display_string(blob: bytes) -> str:
    try:
        return blob.decode("utf-8")
    except UnicodeDecodeError:
        return blob.decode("latin-1", errors="replace")


def format_hex_bytes(blob: bytes, width: int = 16) -> str:
    lines = []
    for offset in range(0, len(blob), width):
        chunk = blob[offset : offset + width]
        hex_part = " ".join(f"{byte:02X}" for byte in chunk)
        ascii_part = "".join(chr(byte) if 32 <= byte <= 126 else "." for byte in chunk)
        lines.append(f"  +0x{offset:04X}: {hex_part:<{width * 3}} {ascii_part}")
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Look up a string or raw bytes in an ELF by virtual address."
    )
    parser.add_argument("address", help="Virtual address, with or without 0x prefix")
    parser.add_argument(
        "--game",
        choices=sorted(DEFAULT_ELF_BY_GAME),
        default="gc",
        help="Shortcut for selecting the default GameCube or PS2 ELF (default: gc)",
    )
    parser.add_argument(
        "--elf",
        help="Explicit ELF path. Overrides --game.",
    )
    parser.add_argument(
        "--mode",
        choices=["string", "bytes"],
        default="string",
        help="Read a C string or raw bytes from the address (default: string)",
    )
    parser.add_argument(
        "--length",
        type=int,
        default=64,
        help="Maximum bytes to read (default: 64)",
    )
    args = parser.parse_args()

    address = parse_address(args.address)
    elf_path = choose_elf_path(args)
    if not os.path.exists(elf_path):
        raise ToolError(f"ELF not found: {elf_path}")

    with open(elf_path, "rb") as f:
        elffile = ELFFile(f)
        section = find_section_for_address(elffile, address)

    if section is None:
        raise ToolError(
            f"address 0x{address:08X} is not inside any ELF section in {elf_path}"
        )

    section_offset = address - int(section["address"])
    file_offset = int(section["offset"]) + section_offset
    section_data = section["data"]

    print(f"ELF: {os.path.relpath(elf_path, ROOT_DIR)}")
    print(f"Address: 0x{address:08X}")
    print(
        f"Section: {section['name']} +0x{section_offset:X} "
        f"(section VA 0x{int(section['address']):08X}, file offset 0x{file_offset:X})"
    )

    if args.mode == "string":
        blob = read_c_string(section_data, section_offset, args.length)
        display = decode_display_string(blob)
        kind = "string" if printable_ratio(blob) >= 0.85 else "non-printable bytes"
        print(f"Kind: {kind}")
        print(f"Length: {len(blob)} byte(s)")
        print(f"Value: {display!r}")
        if blob:
            print("Hex:")
            print(format_hex_bytes(blob))
        return

    blob = section_data[section_offset : section_offset + args.length]
    print(f"Length: {len(blob)} byte(s)")
    print("Hex:")
    print(format_hex_bytes(blob))


if __name__ == "__main__":
    try:
        main()
    except ToolError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(1)
