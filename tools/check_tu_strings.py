#!/usr/bin/env python3
"""
Compare string-like text relocations for one translation unit.

This is meant to answer "which string references in this TU differ from the
original object?" quickly, without manually diffing objdump output.

Examples:
  python tools/check_tu_strings.py -u main/Speed/Indep/SourceLists/zTrack
  python tools/check_tu_strings.py -u main/Speed/Indep/SourceLists/zTrack --all
  python tools/check_tu_strings.py -u main/Speed/Indep/SourceLists/zTrack --min-length 2
"""

from __future__ import annotations

import argparse
from collections import defaultdict
import os
import string
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Sequence, Tuple

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

from _common import ToolError, find_objdiff_unit, load_objdiff_config, make_abs


STRING_SECTIONS = {".rodata", ".sdata", ".sdata2", ".data"}
PRINTABLE_BYTES = set(string.printable.encode("ascii")) - set(b"\x0b\x0c")


@dataclass(frozen=True)
class StringRef:
    reloc_section: str
    reloc_offset: int
    symbol_name: str
    target_section: str
    target_offset: int
    value: str


@dataclass(frozen=True)
class RefGroup:
    reloc_section: str
    offsets: Tuple[int, ...]
    symbol_name: str
    target_section: str
    target_offset: int
    value: str


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare string-like relocations for a translation unit."
    )
    parser.add_argument(
        "-u",
        "--unit",
        required=True,
        help="Unit name from objdiff.json (for example main/Speed/Indep/SourceLists/zTrack)",
    )
    parser.add_argument(
        "--original",
        help="Override original/reference object path (defaults to the unit's obj/ object)",
    )
    parser.add_argument(
        "--rebuilt",
        help="Override rebuilt/decomp object path (defaults to the unit's src/ object)",
    )
    parser.add_argument(
        "--min-length",
        type=int,
        default=3,
        help="Minimum decoded string length to consider (default: 3)",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Show all detected string refs, not just mismatches",
    )
    parser.add_argument(
        "--search",
        help="Only show string values containing this substring",
    )
    return parser.parse_args()


def decode_c_string(data: bytes, start: int, limit: int = 256) -> Optional[str]:
    if start < 0 or start >= len(data):
        return None
    end = data.find(b"\x00", start, min(len(data), start + limit))
    if end == -1:
        return None
    blob = data[start:end]
    if not blob:
        return ""
    if not all(byte in PRINTABLE_BYTES for byte in blob):
        return None
    try:
        return blob.decode("utf-8")
    except UnicodeDecodeError:
        return blob.decode("latin-1", errors="replace")


def resolve_unit_paths(unit_name: str, original_override: Optional[str], rebuilt_override: Optional[str]) -> Tuple[str, str]:
    if original_override and rebuilt_override:
        return os.path.abspath(original_override), os.path.abspath(rebuilt_override)

    config = load_objdiff_config()
    unit = find_objdiff_unit(config, unit_name)
    if unit is None:
        raise ToolError(f"Unit not found in objdiff.json: {unit_name}")

    base_path = make_abs(unit.get("base_path"))
    target_path = make_abs(unit.get("target_path"))
    if not base_path or not target_path:
        raise ToolError(f"Unit is missing base/target object paths: {unit_name}")

    # In this repository, objdiff stores the rebuilt object in base_path and the
    # original reference object in target_path. Keep a small heuristic fallback
    # in case another config flips them.
    if "/obj/" in base_path and "/src/" in target_path:
        original_path = base_path
        rebuilt_path = target_path
    else:
        original_path = target_path
        rebuilt_path = base_path

    if original_override:
        original_path = os.path.abspath(original_override)
    if rebuilt_override:
        rebuilt_path = os.path.abspath(rebuilt_override)

    return original_path, rebuilt_path


def resolve_ref(
    elf: ELFFile,
    reloc_section: RelocationSection,
    reloc,
    min_length: int,
) -> Optional[StringRef]:
    symtab = elf.get_section(reloc_section["sh_link"])
    symbol = symtab.get_symbol(reloc["r_info_sym"])
    symbol_name = symbol.name or "<section>"
    target_section = None
    target_offset = None
    addend = int(reloc.entry.get("r_addend", 0))
    shndx = symbol["st_shndx"]

    if symbol["st_info"]["type"] == "STT_SECTION" and isinstance(shndx, int):
        target_section = elf.get_section(shndx)
        target_offset = addend
    elif isinstance(shndx, int) and 0 <= shndx < elf.num_sections():
        target_section = elf.get_section(shndx)
        target_offset = int(symbol["st_value"]) + addend

    if target_section is None or target_section.name not in STRING_SECTIONS:
        return None

    value = decode_c_string(target_section.data(), int(target_offset))
    if value is None or len(value) < min_length:
        return None

    return StringRef(
        reloc_section=reloc_section.name,
        reloc_offset=int(reloc["r_offset"]),
        symbol_name=symbol_name,
        target_section=target_section.name,
        target_offset=int(target_offset),
        value=value,
    )


def load_string_refs(path: str, min_length: int) -> Dict[int, StringRef]:
    refs: Dict[int, StringRef] = {}
    with open(path, "rb") as f:
        elf = ELFFile(f)
        for section in elf.iter_sections():
            if not isinstance(section, RelocationSection):
                continue
            if not section.name.startswith(".rela.text") and not section.name.startswith(".rel.text"):
                continue
            for reloc in section.iter_relocations():
                ref = resolve_ref(elf, section, reloc, min_length)
                if ref is not None:
                    refs[ref.reloc_offset] = ref
    return refs


def group_refs(refs: Iterable[StringRef]) -> List[RefGroup]:
    ordered = sorted(refs, key=lambda ref: (ref.reloc_section, ref.reloc_offset))
    groups: List[RefGroup] = []
    current: List[StringRef] = []
    for ref in ordered:
        if not current:
            current = [ref]
            continue
        prev = current[-1]
        same_target = (
            ref.reloc_section == prev.reloc_section
            and ref.target_section == prev.target_section
            and ref.target_offset == prev.target_offset
            and ref.value == prev.value
            and ref.symbol_name == prev.symbol_name
        )
        # PPC string address materialization usually lands as a hi/lo pair 4 bytes apart.
        if same_target and ref.reloc_offset - prev.reloc_offset <= 4:
            current.append(ref)
            continue
        groups.append(
            RefGroup(
                reloc_section=current[0].reloc_section,
                offsets=tuple(item.reloc_offset for item in current),
                symbol_name=current[0].symbol_name,
                target_section=current[0].target_section,
                target_offset=current[0].target_offset,
                value=current[0].value,
            )
        )
        current = [ref]

    if current:
        groups.append(
            RefGroup(
                reloc_section=current[0].reloc_section,
                offsets=tuple(item.reloc_offset for item in current),
                symbol_name=current[0].symbol_name,
                target_section=current[0].target_section,
                target_offset=current[0].target_offset,
                value=current[0].value,
            )
        )
    return groups


def format_offsets(offsets: Sequence[int]) -> str:
    if not offsets:
        return "<none>"
    if len(offsets) == 1:
        return f"0x{offsets[0]:X}"
    return ", ".join(f"0x{offset:X}" for offset in offsets)


def bucket_groups(groups: Iterable[RefGroup]) -> Dict[str, List[RefGroup]]:
    buckets: Dict[str, List[RefGroup]] = defaultdict(list)
    for group in groups:
        buckets[group.value].append(group)
    return buckets


def print_group_list(label: str, groups: Sequence[RefGroup]) -> None:
    if not groups:
        print(f"  {label}: <missing>")
        return
    print(f"  {label}: {len(groups)} callsite(s)")
    for group in groups:
        print(
            f"    - [{group.reloc_section} @ {format_offsets(group.offsets)} -> "
            f"{group.target_section}+0x{group.target_offset:X}]"
        )


def main() -> None:
    args = parse_args()
    original_path, rebuilt_path = resolve_unit_paths(args.unit, args.original, args.rebuilt)

    if not os.path.exists(original_path):
        raise ToolError(f"Original object not found: {original_path}")
    if not os.path.exists(rebuilt_path):
        raise ToolError(f"Rebuilt object not found: {rebuilt_path}")

    original_refs = load_string_refs(original_path, args.min_length)
    rebuilt_refs = load_string_refs(rebuilt_path, args.min_length)
    original_groups = group_refs(original_refs.values())
    rebuilt_groups = group_refs(rebuilt_refs.values())
    original_buckets = bucket_groups(original_groups)
    rebuilt_buckets = bucket_groups(rebuilt_groups)
    values = sorted(set(original_buckets) | set(rebuilt_buckets))

    print(f"Unit: {args.unit}")
    print(f"Original: {os.path.relpath(original_path)}")
    print(f"Rebuilt:  {os.path.relpath(rebuilt_path)}")
    print(f"String filter: printable C strings with length >= {args.min_length}")
    print()

    if not values:
        print("No string-like .text relocations found.")
        return

    shown = 0
    for value in values:
        if args.search and args.search not in value:
            continue
        original_list = original_buckets.get(value, [])
        rebuilt_list = rebuilt_buckets.get(value, [])
        if not args.all:
            if len(original_list) == len(rebuilt_list):
                continue
        shown += 1
        print(f"String {shown}: {value!r}")
        print_group_list("original", original_list)
        print_group_list("rebuilt ", rebuilt_list)
        print()

    if shown == 0:
        print("No mismatching string refs found.")


if __name__ == "__main__":
    try:
        main()
    except ToolError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(1)
