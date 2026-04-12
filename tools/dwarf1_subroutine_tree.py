#!/usr/bin/env python3

"""
Inspect raw MWCC DWARF 1.1 subroutine trees directly from .debug.

This is useful when `dtk dwarf dump` fails to emit a top-level function block,
but the raw DIE tree is still present in the rebuilt object.

Examples:
  python tools/dwarf1_subroutine_tree.py -u main/Speed/Indep/SourceLists/zAttribSys -f 'Attrib::Class::RemoveCollection(Attrib::Collection *)'
  python tools/dwarf1_subroutine_tree.py build/GOWE69/src/Speed/Indep/SourceLists/zAttribSys.o --tag 0x2A2C8
  python tools/dwarf1_subroutine_tree.py -u main/Speed/Indep/SourceLists/zAttribSys -f 'Attrib::Database::RemoveClass(Attrib::Class const *)' --json
  python tools/dwarf1_subroutine_tree.py -u main/Speed/Indep/SourceLists/zAttribSys -f 'Attrib::Class::RemoveCollection(Attrib::Collection *)' --compare-original
  python tools/dwarf1_subroutine_tree.py -u main/Speed/Indep/SourceLists/zAttribSys -f 'Attrib::Class::RemoveCollection(Attrib::Collection *)' --show-non-subroutine

The default tree intentionally compares by owner + bare name, which is great for fast
owner-drift checks but can hide overload-only mismatches. When an exact-text candidate
adds an overloaded inline helper or otherwise reuses the same base name with different
parameters/locals, use `--show-non-subroutine` (and `--show-mangled` when available) to
inspect the rebuilt formal-parameter/local rows directly before trusting a
`--compare-original` pass.
"""

from __future__ import annotations

import argparse
import difflib
import json
import re
from dataclasses import dataclass
from enum import IntEnum
from io import BytesIO
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

from _common import find_objdiff_unit, load_objdiff_config, make_abs
from lookup import split_functions


class TreeError(RuntimeError):
    pass


class Endian:
    def __init__(self, little: bool):
        self.little = little

    def u16(self, data: bytes) -> int:
        return int.from_bytes(data, "little" if self.little else "big")

    def u32(self, data: bytes) -> int:
        return int.from_bytes(data, "little" if self.little else "big")

    def u64(self, data: bytes) -> int:
        return int.from_bytes(data, "little" if self.little else "big")


class TagKind(IntEnum):
    PADDING = 0x0000
    FORMAL_PARAMETER = 0x0005
    GLOBAL_SUBROUTINE = 0x0006
    LABEL = 0x000A
    LEXICAL_BLOCK = 0x000B
    LOCAL_VARIABLE = 0x000C
    COMPILE_UNIT = 0x0011
    SUBROUTINE = 0x0014
    INLINED_SUBROUTINE = 0x001D


class AttributeKind(IntEnum):
    SIBLING = 0x0010 | 0x2
    LOCATION = 0x0020 | 0x3
    NAME = 0x0030 | 0x8
    FUND_TYPE = 0x0050 | 0x5
    MOD_FUND_TYPE = 0x0060 | 0x3
    USER_DEF_TYPE = 0x0070 | 0x2
    MOD_UD_TYPE = 0x0080 | 0x3
    LOW_PC = 0x0110 | 0x1
    HIGH_PC = 0x0120 | 0x1
    MEMBER = 0x0140 | 0x2
    MW_MANGLED = 0x2000 | 0x8
    SPECIFICATION = 0x02B0 | 0x2


class FormKind(IntEnum):
    ADDR = 0x1
    REF = 0x2
    BLOCK2 = 0x3
    BLOCK4 = 0x4
    DATA2 = 0x5
    DATA4 = 0x6
    DATA8 = 0x7
    STRING = 0x8


FORM_MASK = 0xF


@dataclass
class Attribute:
    kind: int
    value: object


@dataclass
class Tag:
    key: int
    kind: int
    attributes: List[Attribute]


def unit_output_path(unit_name: str) -> str:
    config = load_objdiff_config()
    unit = find_objdiff_unit(config, unit_name)
    if unit is None:
        raise TreeError(f"Unit not found in objdiff.json: {unit_name}")

    target = unit.get("base_path") or unit.get("target_path")
    if not target:
        raise TreeError(f"Unit has no build target in objdiff.json: {unit_name}")
    return make_abs(str(target)) or str(target)


def normalize_query(name: str) -> str:
    bare = name.strip()
    paren = bare.find("(")
    if paren != -1:
        bare = bare[:paren]
    return bare.strip()


def candidate_names(name: str) -> List[str]:
    bare = normalize_query(name)
    if not bare:
        return []
    parts = bare.split("::")
    out: List[str] = []
    for index in range(len(parts)):
        candidate = "::".join(parts[index:]).strip()
        if candidate and candidate not in out:
            out.append(candidate)
    return out


def read_string(f: BytesIO) -> str:
    data = bytearray()
    while True:
        b = f.read(1)
        if not b or b == b"\0":
            break
        data.extend(b)
    return data.decode("latin1")


def apply_debug_relocations(elf: ELFFile, section_name: str = ".debug") -> bytes:
    debug_section = elf.get_section_by_name(section_name)
    if debug_section is None:
        raise TreeError(f"Object is missing {section_name}")
    data = bytearray(debug_section.data())

    symtabs = [
        section
        for section in elf.iter_sections()
        if section.header["sh_type"] in ("SHT_SYMTAB", "SHT_DYNSYM")
    ]

    def get_symbol(index: int):
        for symtab in symtabs:
            if index < symtab.num_symbols():
                return symtab.get_symbol(index)
        return None

    for section in elf.iter_sections():
        if not isinstance(section, RelocationSection):
            continue
        target = elf.get_section(section["sh_info"])
        if target.name != section_name:
            continue

        for reloc in section.iter_relocations():
            reloc_type = reloc.entry["r_info_type"]
            if reloc_type == 0:
                continue
            if reloc_type != 1:
                raise TreeError(
                    f"Unhandled {section_name} relocation type {reloc_type} in {section.name}"
                )
            symbol = get_symbol(reloc.entry["r_info_sym"])
            addend = reloc.entry.get("r_addend", 0)
            value = (symbol["st_value"] if symbol is not None else 0) + addend
            offset = reloc["r_offset"]
            data[offset : offset + 4] = int(value).to_bytes(4, "big")

    return bytes(data)


def read_attribute(f: BytesIO, endian: Endian) -> Attribute:
    kind = endian.u16(f.read(2))
    form = FormKind(kind & FORM_MASK)
    if form in (FormKind.ADDR, FormKind.REF, FormKind.DATA4):
        value = endian.u32(f.read(4))
    elif form == FormKind.DATA2:
        value = endian.u16(f.read(2))
    elif form == FormKind.DATA8:
        value = endian.u64(f.read(8))
    elif form == FormKind.BLOCK2:
        size = endian.u16(f.read(2))
        value = f.read(size)
    elif form == FormKind.BLOCK4:
        size = endian.u32(f.read(4))
        value = f.read(size)
    elif form == FormKind.STRING:
        value = read_string(f)
    else:
        raise TreeError(f"Unhandled attribute form {form}")
    return Attribute(kind=kind, value=value)


def read_tags(data: bytes, endian: Endian) -> Dict[int, Tag]:
    f = BytesIO(data)
    tags: Dict[int, Tag] = {}
    while f.tell() < len(data):
        position = f.tell()
        size = endian.u32(f.read(4))
        if size < 8:
            if size > 4:
                f.seek(size - 4, 1)
            tags[position] = Tag(key=position, kind=TagKind.PADDING, attributes=[])
            continue

        kind = endian.u16(f.read(2))
        end = position + size
        attributes: List[Attribute] = []
        while f.tell() < end:
            attributes.append(read_attribute(f, endian))
        tags[position] = Tag(key=position, kind=kind, attributes=attributes)
    return tags


def attribute_value(tag: Tag, kind: AttributeKind) -> Optional[object]:
    for attr in tag.attributes:
        if attr.kind == kind:
            return attr.value
    return None


def tag_name(kind: int) -> str:
    try:
        return TagKind(kind).name
    except ValueError:
        return f"0x{kind:X}"


def sorted_keys(tags: Dict[int, Tag]) -> List[int]:
    return sorted(tags.keys())


def next_sibling_key(tags: Dict[int, Tag], keys: List[int], key: int) -> Optional[int]:
    sibling = attribute_value(tags[key], AttributeKind.SIBLING)
    if isinstance(sibling, int):
        return sibling
    index = keys.index(key)
    if index + 1 < len(keys):
        return keys[index + 1]
    return None


def child_keys(tags: Dict[int, Tag], keys: List[int], key: int) -> List[int]:
    sibling = next_sibling_key(tags, keys, key)
    index = keys.index(key)
    current = keys[index + 1] if index + 1 < len(keys) else None
    out: List[int] = []
    while current is not None and current != sibling:
        if tags[current].kind != TagKind.PADDING:
            out.append(current)
        current = next_sibling_key(tags, keys, current)
    return out


def member_owner_name(tags: Dict[int, Tag], tag: Tag) -> Optional[str]:
    member_ref = attribute_value(tag, AttributeKind.MEMBER)
    if not isinstance(member_ref, int):
        return None
    owner_tag = tags.get(member_ref)
    if owner_tag is None:
        return None
    owner_name = attribute_value(owner_tag, AttributeKind.NAME)
    return owner_name if isinstance(owner_name, str) else None


def resolved_name(tags: Dict[int, Tag], tag: Tag) -> Optional[str]:
    name = attribute_value(tag, AttributeKind.NAME)
    if isinstance(name, str):
        return name
    spec_ref = attribute_value(tag, AttributeKind.SPECIFICATION)
    if isinstance(spec_ref, int) and spec_ref in tags:
        spec_name = attribute_value(tags[spec_ref], AttributeKind.NAME)
        if isinstance(spec_name, str):
            return spec_name
    return None


def resolved_mangled_name(tags: Dict[int, Tag], tag: Tag) -> Optional[str]:
    mangled = attribute_value(tag, AttributeKind.MW_MANGLED)
    if isinstance(mangled, str):
        return mangled
    spec_ref = attribute_value(tag, AttributeKind.SPECIFICATION)
    if isinstance(spec_ref, int) and spec_ref in tags:
        spec_mangled = attribute_value(tags[spec_ref], AttributeKind.MW_MANGLED)
        if isinstance(spec_mangled, str):
            return spec_mangled
    return None


def resolved_owner_name(tags: Dict[int, Tag], tag: Tag) -> Optional[str]:
    owner = member_owner_name(tags, tag)
    if owner:
        return owner
    spec_ref = attribute_value(tag, AttributeKind.SPECIFICATION)
    if isinstance(spec_ref, int) and spec_ref in tags:
        return member_owner_name(tags, tags[spec_ref])
    return None


def qualified_name(tags: Dict[int, Tag], tag: Tag) -> str:
    name = resolved_name(tags, tag) or "<anonymous>"
    owner = resolved_owner_name(tags, tag)
    return f"{owner}::{name}" if owner else name


def tag_range(tag: Tag) -> tuple[Optional[int], Optional[int]]:
    low = attribute_value(tag, AttributeKind.LOW_PC)
    high = attribute_value(tag, AttributeKind.HIGH_PC)
    return (
        low if isinstance(low, int) else None,
        high if isinstance(high, int) else None,
    )


def top_level_subroutines(tags: Dict[int, Tag]) -> List[Tag]:
    return [
        tag
        for tag in tags.values()
        if tag.kind in (TagKind.GLOBAL_SUBROUTINE, TagKind.SUBROUTINE)
        and isinstance(attribute_value(tag, AttributeKind.LOW_PC), int)
        and isinstance(attribute_value(tag, AttributeKind.HIGH_PC), int)
    ]


def choose_subroutine(tags: Dict[int, Tag], query: Optional[str], tag_offset: Optional[int]) -> Tag:
    if tag_offset is not None:
        tag = tags.get(tag_offset)
        if tag is None:
            raise TreeError(f"No tag at offset 0x{tag_offset:X}")
        if tag.kind not in (TagKind.GLOBAL_SUBROUTINE, TagKind.SUBROUTINE):
            raise TreeError(
                f"Tag 0x{tag_offset:X} is {tag_name(tag.kind)}, not a top-level subroutine"
            )
        return tag

    if not query:
        raise TreeError("Either --function or --tag is required")

    matches: List[Tag] = []
    exact: List[Tag] = []
    for tag in top_level_subroutines(tags):
        name = qualified_name(tags, tag)
        if query in name:
            exact.append(tag)
        if any(candidate in name for candidate in candidate_names(query)):
            matches.append(tag)

    selected = exact or matches
    if not selected:
        raise TreeError(f"Top-level raw subroutine '{query}' not found")
    if len(selected) > 1:
        names = "\n".join(f"  - {qualified_name(tags, tag)}" for tag in selected[:12])
        raise TreeError(
            f"Raw subroutine query '{query}' matched multiple functions.\n{names}"
        )
    return selected[0]


def block_label(tags: Dict[int, Tag], tag: Tag) -> str:
    if tag.kind == TagKind.LEXICAL_BLOCK:
        return "<block>"
    return qualified_name(tags, tag)


def render_tree(
    tags: Dict[int, Tag],
    tag: Tag,
    *,
    max_depth: Optional[int] = None,
    include_non_subroutine: bool = False,
) -> List[Dict[str, Any]]:
    keys = sorted_keys(tags)

    def walk(current: Tag, depth: int, out: List[Dict[str, Any]]) -> None:
        if max_depth is not None and depth > max_depth:
            return
        low, high = tag_range(current)
        out.append(
            {
                "tag_offset": current.key,
                "tag_kind": tag_name(current.kind),
                "depth": depth,
                "label": block_label(tags, current),
                "owner": resolved_owner_name(tags, current),
                "name": resolved_name(tags, current),
                "mangled": resolved_mangled_name(tags, current),
                "low_pc": low,
                "high_pc": high,
                "specification": attribute_value(current, AttributeKind.SPECIFICATION),
            }
        )

        for child_key in child_keys(tags, keys, current.key):
            child = tags[child_key]
            if child.kind in (TagKind.INLINED_SUBROUTINE, TagKind.LEXICAL_BLOCK):
                walk(child, depth + 1, out)
            elif include_non_subroutine and child.kind in (
                TagKind.FORMAL_PARAMETER,
                TagKind.LOCAL_VARIABLE,
                TagKind.LABEL,
            ):
                low_pc, high_pc = tag_range(child)
                out.append(
                    {
                        "tag_offset": child.key,
                        "tag_kind": tag_name(child.kind),
                        "depth": depth + 1,
                        "label": block_label(tags, child),
                        "owner": resolved_owner_name(tags, child),
                        "name": resolved_name(tags, child),
                        "mangled": resolved_mangled_name(tags, child),
                        "low_pc": low_pc,
                        "high_pc": high_pc,
                        "specification": attribute_value(child, AttributeKind.SPECIFICATION),
                    }
                )

    rows: List[Dict[str, Any]] = []
    walk(tag, 0, rows)
    return rows


def format_range(low_pc: Optional[int], high_pc: Optional[int]) -> str:
    if low_pc is None or high_pc is None:
        return "-------- --------"
    return f"{low_pc:04X}-{high_pc:04X}"


def print_tree(rows: Iterable[Dict[str, Any]], *, show_mangled: bool = False) -> None:
    for row in rows:
        indent = "  " * int(row["depth"])
        spec = row["specification"]
        spec_suffix = f" [spec=0x{spec:X}]" if isinstance(spec, int) else ""
        mangled = row.get("mangled")
        mangled_suffix = (
            f" [mangled={mangled}]"
            if show_mangled and isinstance(mangled, str) and mangled
            else ""
        )
        print(
            f"{indent}{format_range(row['low_pc'], row['high_pc'])} "
            f"{row['label']}{spec_suffix}{mangled_suffix}"
        )


def find_original_function_block(query: str) -> tuple[str, str, str, str]:
    dwarf_path = Path(make_abs("symbols/Dwarf/functions.nothpp") or "symbols/Dwarf/functions.nothpp")
    try:
        text = dwarf_path.read_text(encoding="utf-8", errors="replace")
    except OSError as exc:
        raise TreeError(f"Failed to read original DWARF dump: {dwarf_path}") from exc

    matches: List[tuple[str, str, str, str]] = []
    exact: List[tuple[str, str, str, str]] = []
    for func in split_functions(text):
        sig_line = func[2]
        if query in sig_line:
            exact.append(func)
        if any(candidate in sig_line for candidate in candidate_names(query)):
            matches.append(func)

    selected = exact or matches
    if not selected:
        raise TreeError(f"Original DWARF function '{query}' not found in {dwarf_path}")
    if len(selected) > 1:
        preview = "\n".join(f"  - {func[2]}" for func in selected[:12])
        raise TreeError(
            f"Original DWARF query '{query}' matched multiple functions.\n{preview}"
        )
    return selected[0]


def normalize_original_label(sig_line: str) -> str:
    line = sig_line.strip()
    if not line:
        return "<block>"

    if line.endswith("{}"):
        line = line[:-2].rstrip()
    if line.endswith("{"):
        line = line[:-1].rstrip()
    if line.endswith(" const"):
        line = line[:-6].rstrip()

    while line.startswith("inline "):
        line = line[len("inline ") :].lstrip()
    while line.startswith("static "):
        line = line[len("static ") :].lstrip()
    while line.startswith("inline "):
        line = line[len("inline ") :].lstrip()

    paren = line.find("(")
    if paren == -1:
        return "<block>"
    prefix = line[:paren].rstrip()
    if not prefix:
        return "<block>"

    depth = 0
    for index in range(len(prefix) - 1, -1, -1):
        char = prefix[index]
        if char == ">":
            depth += 1
        elif char == "<":
            depth = max(0, depth - 1)
        elif depth == 0 and char.isspace():
            candidate = prefix[index + 1 :].strip()
            return candidate or "<block>"
    return prefix


def split_top_level_commas(text: str) -> List[str]:
    parts: List[str] = []
    depth_angle = 0
    depth_paren = 0
    start = 0
    for index, char in enumerate(text):
        if char == "<":
            depth_angle += 1
        elif char == ">":
            depth_angle = max(0, depth_angle - 1)
        elif char == "(":
            depth_paren += 1
        elif char == ")":
            depth_paren = max(0, depth_paren - 1)
        elif char == "," and depth_angle == 0 and depth_paren == 0:
            parts.append(text[start:index].strip())
            start = index + 1
    tail = text[start:].strip()
    if tail:
        parts.append(tail)
    return parts


def extract_decl_name(line: str) -> Optional[str]:
    candidate = re.sub(r"/\*.*?\*/", "", line).split("//", 1)[0].strip()
    if not candidate or candidate.startswith("/*"):
        return None
    if candidate.endswith("{}"):
        return None
    if candidate.endswith("{"):
        candidate = candidate[:-1].rstrip()
    if "=" in candidate:
        candidate = candidate.split("=", 1)[0].rstrip()
    if candidate.endswith(";"):
        candidate = candidate[:-1].rstrip()
    match = re.search(r"([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?$", candidate)
    if not match:
        return None
    name = match.group(1)
    if name in {"const", "volatile", "unsigned", "signed", "struct", "class"}:
        return None
    return name


def original_param_names(sig_line: str) -> List[str]:
    line = re.sub(r"/\*.*?\*/", "", sig_line).strip()
    if line.endswith("{}"):
        line = line[:-2].rstrip()
    if line.endswith("{"):
        line = line[:-1].rstrip()

    is_static = False
    changed = True
    while changed:
        changed = False
        if line.startswith("static "):
            line = line[len("static ") :].lstrip()
            is_static = True
            changed = True
        if line.startswith("inline "):
            line = line[len("inline ") :].lstrip()
            changed = True

    paren = line.find("(")
    close = line.rfind(")")
    if paren == -1 or close == -1 or close < paren:
        return []

    names: List[str] = []
    if not is_static and "::" in line[:paren]:
        names.append("this")

    params_text = line[paren + 1 : close].strip()
    if not params_text or params_text == "void":
        return names

    for part in split_top_level_commas(params_text):
        name = extract_decl_name(part)
        if name:
            names.append(name)
    return names


def original_rows_from_block(
    block: tuple[str, str, str, str],
    function_label: str,
    *,
    max_depth: Optional[int] = None,
    include_non_subroutine: bool = False,
) -> List[Dict[str, Any]]:
    rows: List[Dict[str, Any]] = [
        {
            "tag_kind": "ORIGINAL_FUNCTION",
            "depth": 0,
            "label": function_label,
            "low_pc": int(block[0], 16),
            "high_pc": int(block[1], 16),
        }
    ]
    if include_non_subroutine:
        for name in original_param_names(block[2]):
            rows.append(
                {
                    "tag_kind": "ORIGINAL_PARAM",
                    "depth": 1,
                    "label": name,
                    "low_pc": int(block[0], 16),
                    "high_pc": int(block[1], 16),
                }
            )

    lines = block[3].splitlines()
    range_re = re.compile(
        r"^(\s*)// Range:\s+0x([0-9A-Fa-f]+)\s*->\s*0x([0-9A-Fa-f]+)\s*$"
    )
    for index, line in enumerate(lines):
        if include_non_subroutine and line.strip() == "// Local variables":
            depth = len(line) - len(line.lstrip(" "))
            local_depth = depth // 4
            lookahead = index + 1
            while lookahead < len(lines):
                candidate = lines[lookahead].strip()
                if not candidate or candidate.startswith("// Range:"):
                    break
                if candidate.startswith("//"):
                    lookahead += 1
                    continue
                name = extract_decl_name(candidate)
                if name:
                    rows.append(
                        {
                            "tag_kind": "ORIGINAL_LOCAL",
                            "depth": local_depth,
                            "label": name,
                            "low_pc": 0,
                            "high_pc": 0,
                        }
                    )
                lookahead += 1

        match = range_re.match(line)
        if not match:
            continue

        sig_line = ""
        lookahead = index + 1
        while lookahead < len(lines):
            candidate = lines[lookahead].strip()
            if candidate:
                if candidate.startswith("//"):
                    lookahead += 1
                    continue
                sig_line = candidate
                break
            lookahead += 1

        depth = len(match.group(1)) // 4
        if depth == 0:
            continue
        if max_depth is not None and depth > max_depth:
            continue
        rows.append(
            {
                "tag_kind": "ORIGINAL_RANGE",
                "depth": depth,
                "label": normalize_original_label(sig_line),
                "low_pc": int(match.group(2), 16) & 0xFFFF,
                "high_pc": int(match.group(3), 16) & 0xFFFF,
            }
        )
        if include_non_subroutine:
            for name in original_param_names(sig_line):
                rows.append(
                    {
                        "tag_kind": "ORIGINAL_PARAM",
                        "depth": depth + 1,
                        "label": name,
                        "low_pc": int(match.group(2), 16) & 0xFFFF,
                        "high_pc": int(match.group(3), 16) & 0xFFFF,
                    }
                )
            if normalize_original_label(sig_line) == "<block>":
                name = extract_decl_name(sig_line)
                if name:
                    rows.append(
                        {
                            "tag_kind": "ORIGINAL_LOCAL",
                            "depth": depth + 1,
                            "label": name,
                            "low_pc": int(match.group(2), 16) & 0xFFFF,
                            "high_pc": int(match.group(3), 16) & 0xFFFF,
                        }
                    )

    return rows


def compare_rows(
    original_rows: List[Dict[str, Any]], rebuilt_rows: List[Dict[str, Any]]
) -> Dict[str, Any]:
    original_keys = [(row["depth"], row["label"]) for row in original_rows]
    rebuilt_keys = [(row["depth"], row["label"]) for row in rebuilt_rows]

    mismatches: List[Dict[str, Any]] = []
    matcher = difflib.SequenceMatcher(a=original_keys, b=rebuilt_keys, autojunk=False)
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            continue
        mismatches.append(
            {
                "tag": tag,
                "original_range": [i1, i2],
                "rebuilt_range": [j1, j2],
                "original": [
                    {
                        "depth": row["depth"],
                        "label": row["label"],
                        "low_pc": row["low_pc"],
                        "high_pc": row["high_pc"],
                    }
                    for row in original_rows[i1:i2]
                ],
                "rebuilt": [
                    {
                        "depth": row["depth"],
                        "label": row["label"],
                        "low_pc": row["low_pc"],
                        "high_pc": row["high_pc"],
                    }
                    for row in rebuilt_rows[j1:j2]
                ],
            }
        )

    return {
        "original_count": len(original_rows),
        "rebuilt_count": len(rebuilt_rows),
        "mismatch_count": len(mismatches),
        "mismatches": mismatches,
    }


def print_comparison(compare: Dict[str, Any]) -> None:
    print()
    print("Original-vs-rebuilt range-tree comparison:")
    print(
        f"  original rows: {compare['original_count']}, rebuilt rows: {compare['rebuilt_count']}, "
        f"mismatches: {compare['mismatch_count']}"
    )
    if not compare["mismatches"]:
        if compare["original_count"] == compare["rebuilt_count"]:
            print("  range trees match by depth/label")
        else:
            print("  shared prefix matches by depth/label, but row counts differ")
        return

    for mismatch in compare["mismatches"][:12]:
        print(
            f"  {mismatch['tag']} "
            f"original[{mismatch['original_range'][0]}:{mismatch['original_range'][1]}] "
            f"rebuilt[{mismatch['rebuilt_range'][0]}:{mismatch['rebuilt_range'][1]}]"
        )
        for row in mismatch["original"][:4]:
            print(f"      - original {row['depth']} {row['label']}")
        if len(mismatch["original"]) > 4:
            print(f"      - ... {len(mismatch['original']) - 4} more original rows")
        for row in mismatch["rebuilt"][:4]:
            print(f"      + rebuilt  {row['depth']} {row['label']}")
        if len(mismatch["rebuilt"]) > 4:
            print(f"      + ... {len(mismatch['rebuilt']) - 4} more rebuilt rows")
    if compare["mismatch_count"] > 12:
        print(f"  ... {compare['mismatch_count'] - 12} more mismatches")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Print a raw MWCC DWARF 1.1 inline-subroutine tree directly from "
            "an object file's .debug section."
        )
    )
    parser.add_argument(
        "object",
        nargs="?",
        help="Path to the object file to inspect. Optional when -u/--unit is used.",
    )
    parser.add_argument("-u", "--unit", help="Objdiff unit name to inspect")
    parser.add_argument("-f", "--function", help="Top-level function name to inspect")
    parser.add_argument(
        "--tag",
        help="Inspect a specific raw tag offset (hex like 0x2A2C8 or decimal)",
    )
    parser.add_argument(
        "--max-depth",
        type=int,
        default=None,
        help="Limit the printed tree depth",
    )
    parser.add_argument(
        "--show-non-subroutine",
        action="store_true",
        help="Also include parameters, locals, and labels in the tree output",
    )
    parser.add_argument(
        "--show-mangled",
        action="store_true",
        help="Also print MWCC mangled names when present",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Emit JSON instead of a text tree",
    )
    parser.add_argument(
        "--compare-original",
        action="store_true",
        help=(
            "Also compare the rebuilt raw tree against the original symbols/Dwarf/functions.nothpp "
            "range tree for the same function"
        ),
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.object:
        obj_path = Path(args.object)
    elif args.unit:
        obj_path = Path(unit_output_path(args.unit))
    else:
        raise TreeError("Either an object path or --unit is required")

    if not obj_path.is_file():
        raise TreeError(f"Object file not found: {obj_path}")

    tag_offset = None
    if args.tag:
        tag_offset = int(args.tag, 0)

    with obj_path.open("rb") as f:
        elf = ELFFile(f)
        data = apply_debug_relocations(elf)
        tags = read_tags(data, Endian(elf.little_endian))

    tag = choose_subroutine(tags, args.function, tag_offset)
    rows = render_tree(
        tags,
        tag,
        max_depth=args.max_depth,
        include_non_subroutine=args.show_non_subroutine,
    )
    compare: Optional[Dict[str, Any]] = None
    if args.compare_original:
        if not args.function:
            raise TreeError("--compare-original requires --function")
        original_block = find_original_function_block(args.function)
        original_rows = original_rows_from_block(
            original_block,
            qualified_name(tags, tag),
            max_depth=args.max_depth,
            include_non_subroutine=args.show_non_subroutine,
        )
        compare = compare_rows(original_rows, rows)

    if args.json:
        payload = {
            "object": str(obj_path),
            "selected_tag": tag.key,
            "selected_name": qualified_name(tags, tag),
            "rows": rows,
        }
        if compare is not None:
            payload["compare_original"] = compare
        print(json.dumps(payload, indent=2))
    else:
        print(f"Object: {obj_path}")
        print(f"Tag: 0x{tag.key:X}")
        print(f"Function: {qualified_name(tags, tag)}")
        if args.show_mangled:
            mangled = resolved_mangled_name(tags, tag)
            if mangled:
                print(f"Mangled: {mangled}")
        low_pc, high_pc = tag_range(tag)
        print(f"Range: {format_range(low_pc, high_pc)}")
        print()
        print_tree(rows, show_mangled=args.show_mangled)
        if compare is not None:
            print_comparison(compare)


if __name__ == "__main__":
    try:
        main()
    except TreeError as exc:
        raise SystemExit(str(exc))
