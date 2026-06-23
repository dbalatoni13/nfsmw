#!/usr/bin/env python3
"""
Merge type metadata from related game debug dumps into the GC globals dump.

The GC DWARF1 dump is kept as the canonical declaration source. PS2 STABS and
Carbon DWARF are used as metadata sources for visibility, typedef spelling,
declaration locations, and declaration order.
"""

from __future__ import annotations

import argparse
import bisect
import csv
import os
import re
import sys
from dataclasses import dataclass, field
from typing import Dict, Iterable, List, Optional, Sequence, Set, Tuple


ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEFAULT_GC = os.path.join(ROOT_DIR, "symbols", "Dwarf", "globals.nothpp")
DEFAULT_GC_FUNCTIONS = os.path.join(ROOT_DIR, "symbols", "Dwarf", "functions.nothpp")
DEFAULT_PS2 = os.path.join(ROOT_DIR, "symbols", "PS2", "PS2_types.nothpp")
DEFAULT_PS2_FUNCTIONS = os.path.join(ROOT_DIR, "symbols", "PS2", "PS2_functions.nothpp")
DEFAULT_CARBON = os.path.join(ROOT_DIR, "symbols", "CarbonDwarf", "globals.nothpp")
DEFAULT_CARBON_FUNCTIONS = os.path.join(
    ROOT_DIR, "symbols", "CarbonDwarf", "functions.nothpp"
)
DEFAULT_PROSTREET_CLASSES = os.path.join(ROOT_DIR, "symbols", "prostreet_classes.csv")
DEFAULT_OUTPUT = os.path.join(ROOT_DIR, "symbols", "Dwarf", "dwarf.unified.nothpp")
DEFAULT_UNPAIRED = os.path.join(
    ROOT_DIR, "symbols", "Dwarf", "globals.unified.unpaired.nothpp"
)
ORDER_SCALE = 1000


TOTAL_SIZE_RE = re.compile(r"^// total size:\s*(0x[0-9A-Fa-f]+)")
CARBON_DECL_RE = re.compile(r"Decl:\s*(.*):(\d+)")
GC_OPEN_RE = re.compile(
    r"^(?P<kind>struct|class)\s+(?P<name>[^{:]+(?:[:][^{]+)?)(?P<bases>\s*:[^{]+)?\s*\{"
)
PS2_OPEN_RE = re.compile(
    r"^(?P<kind>struct|class)\s+(?P<name>.+?)(?P<bases>\s*:[^{]+)?\s*\{\s*//\s*(?P<size>0x[0-9A-Fa-f]+)"
)
PS2_TYPEDEF_OPEN_RE = re.compile(
    r"^typedef\s+(?P<kind>struct|class)\s*(?P<anon_name>.*?)\s*\{\s*//\s*(?P<size>0x[0-9A-Fa-f]+)"
)
ENUM_OPEN_RE = re.compile(r"^enum\s+(?:class\s+)?[A-Za-z_]\w*(?:\s*:\s*[\w:]+)?\s*\{")
TYPEDEF_CLOSE_RE = re.compile(r"^\}\s*(?P<name>.+?)\s*;")
OFFSET_RE = re.compile(r"offset\s+(0x[0-9A-Fa-f]+)")
PS2_OFFSET_RE = re.compile(r"/\*\s*(0x[0-9A-Fa-f]+)\s*\*/\s*(.*)")
SIZE_RE = re.compile(r"size(?:\s*:|, size)\s*(0x[0-9A-Fa-f]+)")
FUNC_NAME_TOKEN_RE = re.compile(r"(operator\s*[^\s(]+|~?[A-Za-z_]\w*)\s*\(")
IDENT_RE = re.compile(r"[A-Za-z_]\w*")
COMMENT_SECTION_RE = re.compile(
    r"^// (Functions|Members|Static members|Static Members|Inlines|Inner declarations)$"
)
ATTACHED_COMMENT_RE = re.compile(r"^// (Decl|Overrides):")
RAW_TYPE_OPEN_RE = re.compile(
    r"^(?P<kind>struct|class)\s+(?P<head>.+?)(?P<brace>\s*\{.*)$"
)
TYPE_WORDS = {
    "bool",
    "char",
    "const",
    "double",
    "enum",
    "float",
    "int",
    "long",
    "short",
    "signed",
    "struct",
    "unsigned",
    "void",
    "volatile",
}


@dataclass
class DeclItem:
    kind: str
    lines: List[str]
    name: str = ""
    key: str = ""
    match_key: str = ""
    offset: Optional[int] = None
    visibility: Optional[str] = None
    decl_file: Optional[str] = None
    decl_line: Optional[int] = None
    original_index: int = 0
    section: str = ""
    from_typedef_spelling: bool = False
    from_gc_virtual_source: bool = False
    nested_type: Optional["TypeDecl"] = None

    def text(self) -> str:
        return "\n".join(self.lines)


@dataclass
class TypeDecl:
    source: str
    kind: str
    name: str
    size: Optional[int]
    bases: str
    header_comments: List[str]
    items: List[DeclItem]
    original_index: int
    typedef_name: Optional[str] = None
    decl_file: Optional[str] = None
    decl_line: Optional[int] = None
    paired_ps2: Optional["TypeDecl"] = None
    paired_carbon: Optional["TypeDecl"] = None
    pair_notes: List[str] = field(default_factory=list)

    @property
    def simple_name(self) -> str:
        return simple_name(self.name)

    @property
    def output_name(self) -> str:
        return self.typedef_name or self.name


@dataclass
class TopDecl:
    kind: str
    name: str
    lines: List[str]
    original_index: int
    decl_file: Optional[str] = None
    decl_line: Optional[int] = None
    order_file: Optional[str] = None
    order_line: Optional[int] = None
    match_key: str = ""
    paired_carbon: Optional["TopDecl"] = None


@dataclass
class ParseResult:
    types: List[TypeDecl]
    others: List[str]
    top_decls: List[TopDecl] = field(default_factory=list)


@dataclass
class ProStreetClassIndex:
    exact_names: Set[str]
    simple_names: Dict[str, Set[str]]


Ps2FunctionIndex = Dict[str, Dict[str, DeclItem]]


def read_text(path: str) -> str:
    with open(path, "r", encoding="utf-8", errors="ignore") as fh:
        return fh.read()


def write_text(path: str, text: str) -> None:
    parent = os.path.dirname(path)
    if parent:
        os.makedirs(parent, exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)


def parse_int(value: Optional[str]) -> Optional[int]:
    if not value:
        return None
    return int(value, 16)


def simple_name(name: str) -> str:
    name = name.strip()
    if name.startswith("struct "):
        name = name[7:].strip()
    if name.startswith("class "):
        name = name[6:].strip()
    if "::" in name:
        return name.split("::")[-1].strip()
    return name


def normalize_type_name(name: str) -> str:
    name = re.sub(r"\b(struct|class|union|enum)\s+", "", name)
    name = re.sub(r"\s+", "", name)
    return name.strip()


def load_prostreet_class_index(path: str) -> ProStreetClassIndex:
    exact_names: Set[str] = set()
    simple_names: Dict[str, Set[str]] = {}
    if not path or not os.path.exists(path):
        return ProStreetClassIndex(exact_names, simple_names)

    with open(path, "r", encoding="utf-8", errors="ignore", newline="") as fh:
        for row in csv.DictReader(fh):
            if row.get("kind", "").strip() != "class":
                continue
            qualname = normalize_type_name(
                (row.get("qualname") or row.get("name") or "").strip()
            )
            if not qualname:
                continue
            exact_names.add(qualname)
            simple = normalize_type_name((row.get("name") or simple_name(qualname)).strip())
            if not simple:
                simple = simple_name(qualname)
            simple_names.setdefault(simple, set()).add(qualname)
    return ProStreetClassIndex(exact_names, simple_names)


def prostreet_kind_for_name(
    kind: str, name: str, class_index: ProStreetClassIndex
) -> str:
    if kind == "class":
        return kind
    name = normalize_type_name(name)
    if name in class_index.exact_names:
        return "class"
    if "::" not in name and len(class_index.simple_names.get(name, set())) == 1:
        return "class"
    return kind


def prostreet_type_kind(typ: TypeDecl, class_index: ProStreetClassIndex) -> str:
    return prostreet_kind_for_name(typ.kind, typ.name, class_index)


def split_name_bases(name: str, bases: str = "") -> Tuple[str, str]:
    combined = (name + (" " + bases if bases else "")).strip()
    match = re.search(r"\s+:\s+(?:/\*|public|private|protected)", combined)
    if not match:
        return combined, ""
    return combined[: match.start()].strip(), combined[match.start() :].strip()


def strip_decl_path(path: str) -> str:
    path = path.replace("\\", "/")
    lower = path.lower()
    marker = "leadplat/speed/"
    index = lower.find(marker)
    if index != -1:
        return path[index + len("leadplat/") :]
    return path


def extract_decl_location(line: str) -> Tuple[Optional[str], Optional[int]]:
    match = CARBON_DECL_RE.search(line)
    if not match:
        return None, None
    return strip_decl_path(match.group(1).strip()), int(match.group(2))


def count_braces(line: str) -> int:
    return line.count("{") - line.count("}")


def collect_brace_block(lines: Sequence[str], start: int) -> Tuple[List[str], int]:
    block = [lines[start]]
    depth = count_braces(lines[start])
    index = start + 1
    while index < len(lines) and depth > 0:
        block.append(lines[index])
        depth += count_braces(lines[index])
        index += 1
    return block, index


def skip_range_block(lines: Sequence[str], start: int) -> int:
    index = start + 1
    while index < len(lines):
        stripped = lines[index].strip()
        if stripped.startswith("// Range:") or is_gc_type_start(lines, index):
            return index
        index += 1
    return index


def gc_open_index(lines: Sequence[str], index: int) -> Optional[int]:
    if index >= len(lines) or TOTAL_SIZE_RE.match(lines[index]) is None:
        return None
    scan = index + 1
    while scan < len(lines) and lines[scan].strip().startswith("// Decl:"):
        scan += 1
    if scan < len(lines) and GC_OPEN_RE.match(lines[scan].strip()) is not None:
        return scan
    return None


def is_gc_type_start(lines: Sequence[str], index: int) -> bool:
    return gc_open_index(lines, index) is not None


def is_ps2_type_start(line: str) -> bool:
    stripped = line.strip()
    return PS2_OPEN_RE.match(stripped) is not None or PS2_TYPEDEF_OPEN_RE.match(stripped) is not None


def find_type_end(lines: Sequence[str], start: int, mode: str) -> int:
    if mode == "gc":
        open_index = gc_open_index(lines, start)
        if open_index is None:
            return start + 1
    else:
        open_index = start
    depth = 0
    index = open_index
    while index < len(lines):
        depth += count_braces(lines[index])
        if depth == 0 and lines[index].strip().endswith(";"):
            return index + 1
        index += 1
    return len(lines)


def normalize_decl_code(code: str) -> str:
    code = re.sub(r"\s+", " ", code.strip())
    code = code.replace(" *", " * ")
    code = code.replace("&", " & ")
    code = re.sub(r"\s+", " ", code)
    code = code.replace("* ", "* ")
    code = code.replace("& ", "& ")
    return code.strip()


def split_comment(line: str) -> Tuple[str, str]:
    index = line.find("//")
    if index == -1:
        return line.rstrip(), ""
    return line[:index].rstrip(), line[index:].strip()


def code_part(line: str) -> str:
    code = split_comment(line)[0]
    code = re.sub(r"/\*.*?\*/", "", code)
    return code.strip()


def code_endswith_semicolon(line: str) -> bool:
    return code_part(line).endswith(";")


def has_member_offset(line: str, source: str) -> bool:
    stripped = line.strip()
    if source == "ps2":
        return PS2_OFFSET_RE.search(stripped) is not None
    _code, comment = split_comment(stripped)
    return OFFSET_RE.search(comment) is not None


def item_name_from_member_code(code: str) -> str:
    code = code.strip().rstrip(";")
    code = code_without_initializer(code)
    code = re.sub(r"\[[^\]]*\]", "", code)
    match = re.search(r"\(\s*\*+\s*([A-Za-z_]\w*)\s*\)", code)
    if match:
        return match.group(1)
    match = re.search(r"([A-Za-z_]\w*)\s*$", code)
    if match:
        return match.group(1)
    return ""


def member_type_from_code(code: str, name: str) -> str:
    code = code.strip().rstrip(";")
    if not name:
        return code
    index = code.rfind(name)
    if index == -1:
        return code
    return code[:index].strip()


def initializer_index(code: str) -> int:
    depth = 0
    for index, char in enumerate(code):
        if char in "(<[":
            depth += 1
        elif char in ")>]" and depth > 0:
            depth -= 1
        elif char == "=" and depth == 0:
            return index
    return -1


def code_without_initializer(code: str) -> str:
    code = code.strip().rstrip(";").strip()
    index = initializer_index(code)
    if index == -1:
        return code
    return code[:index].strip()


def initializer_from_code(code: str) -> str:
    code = code.strip().rstrip(";").strip()
    index = initializer_index(code)
    if index == -1:
        return ""
    return code[index:].strip()


def top_variable_type_from_code(code: str, name: str) -> str:
    code = code_without_initializer(code)
    if not name:
        return code
    match = re.search(
        rf"(?:\b[A-Za-z_]\w*::)*{re.escape(name)}(?:\s*(?:\[[^\]]*\])*)\s*$",
        code,
    )
    if not match:
        return member_type_from_code(code, name)
    return code[: match.start()].strip()


def top_variable_decl_name_from_code(code: str, name: str) -> str:
    code = code_without_initializer(code)
    if not name:
        return ""
    match = re.search(
        rf"((?:\b[A-Za-z_]\w*::)*{re.escape(name)})(?:\s*(?:\[[^\]]*\])*)\s*$",
        code,
    )
    if not match:
        return name
    return match.group(1)


def split_params(params: str) -> List[str]:
    if not params.strip() or params.strip() == "void":
        return []
    result: List[str] = []
    current: List[str] = []
    depth = 0
    for char in params:
        if char in "(<[":
            depth += 1
        elif char in ")>]" and depth > 0:
            depth -= 1
        if char == "," and depth == 0:
            result.append("".join(current).strip())
            current = []
        else:
            current.append(char)
    if current:
        result.append("".join(current).strip())
    return result


def split_function_signature(code: str) -> Optional[Tuple[str, str, str]]:
    open_index = code.find("(")
    close_index = code.rfind(")")
    if open_index == -1 or close_index == -1 or close_index < open_index:
        return None
    return (
        code[:open_index].rstrip(),
        code[open_index + 1 : close_index].strip(),
        code[close_index + 1 :].strip(),
    )


def function_return_prefix(before_params: str, func_name: str) -> Optional[str]:
    index = before_params.rfind(func_name)
    if index == -1:
        return None
    return before_params[:index].rstrip()


def strip_function_keywords(prefix: str) -> str:
    words = prefix.strip()
    for keyword in ("inline ", "static ", "virtual "):
        words = words.replace(keyword, "")
    return words.strip()


def is_special_member_name(func_name: str, owner: str) -> bool:
    owner_name = simple_name(owner)
    member_name = simple_name(func_name)
    return member_name == owner_name or member_name == "~" + owner_name


def param_name(param: str) -> str:
    name = item_name_from_member_code(param + ";")
    if name in TYPE_WORDS:
        return ""
    return name


def param_type(param: str) -> str:
    stripped = param.strip()
    name = param_name(param)
    if not name:
        return stripped
    match = re.match(
        rf"^(?P<prefix>.+?\(\s*\*+\s*){re.escape(name)}(?P<suffix>\s*\)\s*\(.*\))$",
        stripped,
    )
    if match:
        return f"{match.group('prefix')}{match.group('suffix')}"
    if " " not in stripped and "\t" not in stripped:
        return stripped
    if stripped.endswith("*") or stripped.endswith("&"):
        return stripped
    type_part = member_type_from_code(param, name).strip()
    if not type_part or type_part in ("struct", "class", "enum"):
        return stripped
    return type_part


def format_type_name(type_name: str) -> str:
    type_name = re.sub(r"\s+", " ", type_name.strip())
    type_name = re.sub(r"\s+\*", " *", type_name)
    type_name = re.sub(r"\s+&", " &", type_name)
    return type_name


def format_param_decl(type_name: str, name: str) -> str:
    type_name = format_type_name(type_name)
    if not name:
        return type_name
    match = re.match(r"^(?P<prefix>.+?\(\s*\*+)(?P<suffix>\s*\)\s*\(.*\))$", type_name)
    if match:
        return f"{match.group('prefix')}{name}{match.group('suffix').lstrip()}"
    return f"{type_name} {name}"


def normalize_type_without_const(type_name: str) -> str:
    type_name = re.sub(r"\bconst\b", "", type_name)
    return normalize_type_name(type_name)


def unqualified_type_name(type_name: str) -> str:
    type_name = normalize_type_without_const(type_name)
    return re.sub(r"\b[A-Za-z_]\w*::", "", type_name)


def type_uses_namespace(type_name: str) -> bool:
    return "::" in normalize_type_name(type_name)


def has_pointee_const(type_name: str) -> bool:
    type_name = format_type_name(type_name)
    return (
        type_name.startswith("const ")
        or re.search(r"\bconst\s*\*", type_name) is not None
    )


def merge_carbon_param_type(base_type: str, carbon_type: str) -> str:
    if (
        normalize_type_without_const(base_type)
        == normalize_type_without_const(carbon_type)
    ):
        if has_pointee_const(carbon_type):
            return format_type_name(carbon_type)
        if has_pointee_const(base_type):
            return format_type_name(base_type)
    return format_type_name(carbon_type)


def merge_ps2_param_type(base_type: str, ps2_type: str) -> str:
    if not ps2_type:
        return format_type_name(base_type)
    if (
        has_pointee_const(base_type)
        and not has_pointee_const(ps2_type)
        and normalize_type_without_const(base_type)
        == normalize_type_without_const(ps2_type)
    ):
        return format_type_name(base_type)
    return format_type_name(ps2_type)


def merge_ps2_member_type(base_type: str, ps2_type: str) -> str:
    if (
        type_uses_namespace(base_type)
        and not type_uses_namespace(ps2_type)
        and unqualified_type_name(base_type) == unqualified_type_name(ps2_type)
    ):
        return format_type_name(base_type)
    return format_type_name(ps2_type or base_type)


def merge_carbon_member_type(base_type: str, carbon_type: str) -> str:
    if (
        has_pointee_const(carbon_type)
        and not has_pointee_const(base_type)
        and normalize_type_without_const(base_type)
        == normalize_type_without_const(carbon_type)
    ):
        return format_type_name(carbon_type)
    return format_type_name(base_type)


def strip_inline_keyword(code: str) -> str:
    return re.sub(r"(^|\s)inline\s+", r"\1", code).strip()


def is_owner_this_param(param: str, owner: str) -> bool:
    owner_name = simple_name(owner)
    if not owner_name:
        return False
    compact = re.sub(r"\s+", " ", param.strip())
    return "*" in compact and re.search(rf"\b{re.escape(owner_name)}\b", compact) is not None


def carbon_function_line(item: DeclItem) -> Optional[str]:
    for line in reversed(item.lines):
        stripped = line.strip()
        if stripped and not stripped.startswith("//"):
            return stripped
    return None


def carbon_function_types(carbon_item: DeclItem, owner: str) -> Optional[Tuple[str, List[str]]]:
    line = carbon_function_line(carbon_item)
    if line is None:
        return None
    signature = split_function_signature(code_part(line))
    if signature is None:
        return None
    before_params, params, _suffix = signature
    func_name = carbon_item.name
    return_prefix = function_return_prefix(before_params, func_name)
    if return_prefix is None:
        return None
    return_type = strip_function_keywords(return_prefix)
    carbon_params = function_effective_params(split_params(params), owner)
    carbon_param_types = [format_type_name(param_type(param)) for param in carbon_params]
    return format_type_name(return_type), carbon_param_types


def apply_carbon_function_spelling(
    base_item: DeclItem, carbon_item: DeclItem, owner: str
) -> DeclItem:
    if not base_item.lines:
        return base_item
    base_line = base_item.lines[-1]
    signature = split_function_signature(code_part(base_line))
    carbon_types = carbon_function_types(carbon_item, owner)
    if signature is None or carbon_types is None:
        return base_item

    before_params, params, suffix = signature
    func_name = base_item.name
    return_prefix = function_return_prefix(before_params, func_name)
    if return_prefix is None:
        return base_item

    carbon_return, carbon_param_types = carbon_types
    base_params = split_params(params)
    new_params = list(base_params)
    if len(base_params) == len(carbon_param_types) and all(carbon_param_types):
        new_params = []
        for base_param, new_type in zip(base_params, carbon_param_types):
            name = param_name(base_param)
            new_type = merge_carbon_param_type(param_type(base_param), new_type)
            new_params.append(format_param_decl(new_type, name))

    new_before = before_params
    if not is_special_member_name(func_name, owner) and carbon_return:
        leading = return_prefix[: len(return_prefix) - len(return_prefix.lstrip())]
        keywords = []
        for keyword in ("inline", "static", "virtual"):
            if re.search(rf"\b{keyword}\b", return_prefix):
                keywords.append(keyword)
        keyword_prefix = (leading + " ".join(keywords) + " ") if keywords else leading
        new_before = f"{keyword_prefix}{carbon_return} {func_name}".strip()

    new_code = f"{new_before}({', '.join(new_params)})"
    if suffix:
        new_code += " " + suffix
    _old_code, comment = split_comment(base_line)
    if comment:
        new_code += " " + comment

    copied = DeclItem(**{**base_item.__dict__})
    copied.lines = base_item.lines[:-1] + [new_code]
    return copied


def function_types(item: DeclItem, owner: str) -> Optional[Tuple[str, List[str]]]:
    line = carbon_function_line(item)
    if line is None:
        return None
    signature = split_function_signature(code_part(line))
    if signature is None:
        return None
    before_params, params, _suffix = signature
    return_prefix = function_return_prefix(before_params, item.name)
    if return_prefix is None:
        return None
    return_prefix = re.sub(r"\S+::\s*$", "", return_prefix).strip()
    return_type = strip_function_keywords(return_prefix)
    effective_params = function_effective_params(split_params(params), owner)
    param_types = [format_type_name(param_type(param)) for param in effective_params]
    return format_type_name(return_type), param_types


def apply_ps2_function_spelling(
    base_item: DeclItem, ps2_item: DeclItem, owner: str
) -> DeclItem:
    if not base_item.lines:
        return base_item
    base_line = base_item.lines[-1]
    signature = split_function_signature(code_part(base_line))
    ps2_types = function_types(ps2_item, owner)
    if signature is None or ps2_types is None:
        return base_item

    before_params, params, suffix = signature
    func_name = base_item.name
    return_prefix = function_return_prefix(before_params, func_name)
    if return_prefix is None:
        return base_item

    ps2_return, ps2_param_types = ps2_types
    base_params = split_params(params)
    if len(base_params) != len(ps2_param_types) or not all(ps2_param_types):
        return base_item

    new_params = []
    for base_param, new_type in zip(base_params, ps2_param_types):
        name = param_name(base_param)
        new_type = merge_ps2_param_type(param_type(base_param), new_type)
        new_params.append(format_param_decl(new_type, name))

    new_before = before_params
    if not is_special_member_name(func_name, owner) and ps2_return:
        leading = return_prefix[: len(return_prefix) - len(return_prefix.lstrip())]
        keywords = []
        for keyword in ("inline", "static", "virtual"):
            if re.search(rf"\b{keyword}\b", return_prefix):
                keywords.append(keyword)
        keyword_prefix = (leading + " ".join(keywords) + " ") if keywords else leading
        new_before = f"{keyword_prefix}{format_type_name(ps2_return)} {func_name}".strip()

    new_code = f"{new_before}({', '.join(new_params)})"
    if suffix:
        new_code += " " + suffix
    _old_code, comment = split_comment(base_line)
    if comment:
        new_code += " " + comment

    copied = DeclItem(**{**base_item.__dict__})
    copied.lines = base_item.lines[:-1] + [new_code]
    return copied


def apply_function_return_spelling(
    base_item: DeclItem, return_type: str, owner: str
) -> DeclItem:
    if not base_item.lines or not return_type:
        return base_item
    base_line = base_item.lines[-1]
    raw_code, comment = split_comment(base_line)
    signature = split_function_signature(raw_code.strip())
    if signature is None:
        return base_item

    before_params, params, suffix = signature
    func_name = base_item.name
    return_prefix = function_return_prefix(before_params, func_name)
    if return_prefix is None:
        return base_item

    if is_special_member_name(func_name, owner):
        return base_item

    leading = return_prefix[: len(return_prefix) - len(return_prefix.lstrip())]
    keywords = []
    for keyword in ("inline", "static", "virtual"):
        if re.search(rf"\b{keyword}\b", return_prefix):
            keywords.append(keyword)
    keyword_prefix = (leading + " ".join(keywords) + " ") if keywords else leading
    new_before = f"{keyword_prefix}{format_type_name(return_type)} {func_name}".strip()
    new_code = f"{new_before}({params})"
    if suffix:
        new_code += " " + suffix
    if comment:
        new_code += " " + comment

    copied = DeclItem(**{**base_item.__dict__})
    copied.lines = base_item.lines[:-1] + [new_code]
    return copied


def enum_name_from_line(line: str) -> str:
    match = re.match(r"^enum\s+(?:class\s+)?([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)", line.strip())
    if match:
        return match.group(1)
    return ""


def top_var_name_from_line(line: str) -> str:
    code = code_part(line)
    if code.endswith(";"):
        code = code[:-1].strip()
    return item_name_from_member_code(code)


def extract_top_var_decl(line: str, index: int) -> TopDecl:
    stripped = line.strip()
    name = top_var_name_from_line(stripped)
    decl_file, decl_line = extract_decl_location(stripped)
    return TopDecl(
        kind="variable",
        name=name,
        lines=[stripped],
        original_index=index,
        decl_file=decl_file,
        decl_line=decl_line,
        match_key=top_variable_decl_name_from_code(code_part(stripped), name),
    )


def top_function_signature_line(lines: Sequence[str]) -> str:
    for line in lines:
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        return stripped
    return ""


def top_function_signature_index(lines: Sequence[str]) -> Optional[int]:
    for index, line in enumerate(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        return index
    return None


def top_function_name_from_code(code: str) -> str:
    signature = split_function_signature(code_part(code.strip()))
    if signature is None:
        return function_name_from_code(code)
    before_params, _params, _suffix = signature
    before_params = strip_function_keywords(before_params)
    if "operator" in before_params:
        return before_params[before_params.rfind("operator") :].strip()
    token = before_params.split()[-1] if before_params.split() else before_params
    return token.strip()


def top_function_match_key_from_code(code: str) -> str:
    signature = split_function_signature(code_part(code.strip()))
    if signature is None:
        return top_function_name_from_code(code)
    before_params, params, suffix = signature
    name = top_function_name_from_code(code)
    split = [re.sub(r"/\*.*?\*/", "", param).strip() for param in split_params(params)]
    param_types = [
        canonical_function_param_type(param_type(param))
        for param in split
    ]
    const_suffix = " const" if re.search(r"\bconst\b", suffix) else ""
    return f"{name}({','.join(param_types)}){const_suffix}"


def extract_top_function_decl(lines: List[str], index: int) -> TopDecl:
    decl_file = None
    decl_line = None
    for line in lines:
        decl_file, decl_line = extract_decl_location(line)
        if decl_file is not None:
            break
    signature = top_function_signature_line(lines)
    return TopDecl(
        kind="function",
        name=top_function_name_from_code(signature),
        lines=[line.rstrip() for line in lines],
        original_index=index,
        decl_file=decl_file,
        decl_line=decl_line,
        match_key=top_function_match_key_from_code(signature),
    )


TYPE_KEY_ALIASES = {
    "CarLoaderHandle": "int",
    "int8": "char",
    "uint8": "unsignedchar",
    "int16": "short",
    "uint16": "unsignedshort",
    "int32": "int",
    "uint32": "unsignedint",
    "int64": "longlong",
    "int64_t": "longlong",
    "uint64": "unsignedlonglong",
    "uint64_t": "unsignedlonglong",
}


def function_name_from_code(code: str, owner: str = "") -> str:
    code = code.strip().rstrip(";")
    code = code.replace("inline ", "")
    code = code.replace("virtual ", "")
    code = re.sub(r"\s+const$", " const", code)
    before_paren = code.split("(", 1)[0].strip()
    if not before_paren:
        return code
    if "operator" in before_paren:
        op_index = before_paren.rfind("operator")
        return before_paren[op_index:].strip()
    token = before_paren.split()[-1]
    if "::" in token:
        token = token.split("::")[-1]
    owner_simple = simple_name(owner)
    if token == owner_simple or token == "~" + owner_simple:
        return token
    return token


def canonical_function_param_type(type_name: str) -> str:
    type_name = normalize_type_name(type_name)
    return TYPE_KEY_ALIASES.get(type_name, type_name)


def function_effective_params(params: List[str], owner: str) -> List[str]:
    for index, param in enumerate(params):
        if is_owner_this_param(param, owner):
            return params[index + 1 :]
    return params


def function_match_key(
    code: str, owner: str = "", drop_owner_this: bool = False
) -> str:
    name = function_name_from_code(code, owner)
    signature = split_function_signature(code_part(code.strip().rstrip(";")))
    if signature is None:
        return name
    _before_params, params, suffix = signature
    split = split_params(params)
    if drop_owner_this:
        split = function_effective_params(split, owner)
    param_types = [
        canonical_function_param_type(param_type(param))
        for param in split
    ]
    const_suffix = " const" if re.search(r"\bconst\b", suffix) else ""
    return f"{name}({','.join(param_types)}){const_suffix}"


def is_ps2_destructor_signature(code: str, owner: str) -> bool:
    signature = split_function_signature(code.strip().rstrip(";"))
    if signature is None:
        return False
    before_params, params, _suffix = signature
    owner_simple = simple_name(owner)
    if before_params.strip().split()[-1] != owner_simple:
        return False
    split = split_params(params)
    if len(split) != 3:
        return False
    first = re.sub(r"\s+", " ", split[0].strip())
    return (
        re.fullmatch(rf"(?:struct\s+)?{re.escape(owner_simple)}\s*\*", first) is not None
        and split[1].strip() == "int"
        and split[2].strip() == "void"
    )


def extract_member_item(line: str, source: str, visibility: Optional[str], index: int) -> DeclItem:
    stripped = line.strip()
    decl_file = None
    decl_line = None
    if source == "ps2":
        offset_match = PS2_OFFSET_RE.search(stripped)
        offset = parse_int(offset_match.group(1)) if offset_match else None
        code = offset_match.group(2).strip() if offset_match else stripped
        name = item_name_from_member_code(code)
    else:
        code, comment = split_comment(stripped)
        offset_match = OFFSET_RE.search(comment)
        offset = parse_int(offset_match.group(1)) if offset_match else None
        decl_file, decl_line = extract_decl_location(stripped)
        name = item_name_from_member_code(code)
    return DeclItem(
        kind="member",
        lines=[stripped],
        name=name,
        key="member:" + name if name else "member:" + stripped,
        offset=offset,
        visibility=visibility,
        decl_file=decl_file,
        decl_line=decl_line,
        original_index=index,
    )


def extract_static_item(line: str, source: str, visibility: Optional[str], index: int) -> DeclItem:
    stripped = line.strip()
    code, comment = split_comment(stripped)
    decl_file, decl_line = extract_decl_location(stripped)
    name = item_name_from_member_code(code.replace("static ", "", 1))
    return DeclItem(
        kind="static",
        lines=[stripped],
        name=name,
        key="static:" + name if name else "static:" + stripped,
        visibility=visibility,
        decl_file=decl_file,
        decl_line=decl_line,
        original_index=index,
    )


def extract_function_item(
    pending_comments: List[str],
    line: str,
    source: str,
    visibility: Optional[str],
    index: int,
    owner: str,
) -> DeclItem:
    stripped = line.strip()
    decl_file = None
    decl_line = None
    for comment in pending_comments:
        decl_file, decl_line = extract_decl_location(comment)
        if decl_file is not None:
            break
    if decl_file is None:
        decl_file, decl_line = extract_decl_location(stripped)
    name = function_name_from_code(stripped, owner)
    match_key = "function:" + function_match_key(
        stripped, owner, drop_owner_this=source == "carbon"
    )
    if source == "ps2" and is_ps2_destructor_signature(stripped, owner):
        name = "~" + simple_name(owner)
        match_key = f"function:{name}()"
    key = "function:" + name
    return DeclItem(
        kind="function",
        lines=pending_comments + [stripped],
        name=key.split(":", 1)[1],
        key=key,
        match_key=match_key,
        visibility=visibility,
        decl_file=decl_file,
        decl_line=decl_line,
        original_index=index,
    )


def make_raw_item(
    lines: List[str],
    kind: str,
    visibility: Optional[str],
    index: int,
) -> DeclItem:
    decl_file = None
    decl_line = None
    for line in lines:
        decl_file, decl_line = extract_decl_location(line)
        if decl_file is not None:
            break
    key = kind + ":" + normalize_type_name(" ".join(line.strip() for line in lines[:2]))
    return DeclItem(
        kind=kind,
        lines=[line.strip() for line in lines if line.strip()],
        key=key,
        visibility=visibility,
        decl_file=decl_file,
        decl_line=decl_line,
        original_index=index,
    )


def prepend_pending_comments(item: DeclItem, pending_comments: List[str]) -> DeclItem:
    if pending_comments:
        item.lines = list(pending_comments) + item.lines
    return item


def parse_type_body(
    source: str,
    name: str,
    lines: Sequence[str],
    start: int,
    end: int,
) -> Tuple[List[DeclItem], Optional[str], Optional[int]]:
    if source in ("gc", "carbon"):
        open_index = gc_open_index(lines, start)
        if open_index is None:
            open_index = start + 1
        body_start = open_index + 1
        close_index = end - 1
        header_scan = lines[start : open_index + 1]
    else:
        body_start = start + 1
        close_index = end - 1
        header_scan = lines[start : start + 1]

    struct_file = None
    struct_line = None
    for line in header_scan:
        struct_file, struct_line = extract_decl_location(line)
        if struct_file is not None:
            break

    items: List[DeclItem] = []
    pending_comments: List[str] = []
    visibility: Optional[str] = None
    section = ""
    item_index = 0
    index = body_start
    while index < close_index:
        line = lines[index]
        stripped = line.strip()
        if not stripped:
            index += 1
            continue
        if COMMENT_SECTION_RE.match(stripped):
            section = COMMENT_SECTION_RE.match(stripped).group(1).lower()
            pending_comments = []
            index += 1
            continue
        if stripped in ("public:", "private:", "protected:"):
            visibility = stripped[:-1]
            pending_comments = []
            index += 1
            continue
        if ATTACHED_COMMENT_RE.match(stripped):
            pending_comments.append(stripped)
            index += 1
            continue
        if stripped.startswith("//"):
            pending_comments = []
            index += 1
            continue
        if is_gc_type_start(lines, index):
            block_end = find_type_end(lines, index, "gc")
            item = prepend_pending_comments(
                make_raw_item(list(lines[index:block_end]), "inner", visibility, item_index),
                pending_comments,
            )
            item.section = section
            items.append(item)
            item_index += 1
            pending_comments = []
            index = block_end
            continue
        if is_ps2_type_start(stripped):
            block_end = find_type_end(lines, index, "ps2")
            item = prepend_pending_comments(
                make_raw_item(list(lines[index:block_end]), "inner", visibility, item_index),
                pending_comments,
            )
            item.section = section
            items.append(item)
            item_index += 1
            pending_comments = []
            index = block_end
            continue
        if "{" in stripped and not stripped.endswith("{}"):
            block, next_index = collect_brace_block(lines, index)
            item = prepend_pending_comments(
                make_raw_item(block, "raw", visibility, item_index),
                pending_comments,
            )
            item.section = section
            items.append(item)
            item_index += 1
            pending_comments = []
            index = next_index
            continue
        if (
            "(" in stripped
            and not has_member_offset(stripped, source)
            and (code_endswith_semicolon(stripped) or stripped.endswith("{}"))
        ):
            item = extract_function_item(
                pending_comments, stripped, source, visibility, item_index, name
            )
            item.section = section
            items.append(item)
            item_index += 1
            pending_comments = []
            index += 1
            continue
        if stripped.startswith("static ") and code_endswith_semicolon(stripped):
            item = prepend_pending_comments(
                extract_static_item(stripped, source, visibility, item_index),
                pending_comments,
            )
            item.section = section
            items.append(item)
            item_index += 1
            pending_comments = []
            index += 1
            continue
        if code_endswith_semicolon(stripped):
            item = prepend_pending_comments(
                extract_member_item(stripped, source, visibility, item_index),
                pending_comments,
            )
            item.section = section
            items.append(item)
            item_index += 1
            pending_comments = []
            index += 1
            continue
        item = prepend_pending_comments(
            make_raw_item([stripped], "raw", visibility, item_index),
            pending_comments,
        )
        item.section = section
        items.append(item)
        item_index += 1
        pending_comments = []
        index += 1
    return items, struct_file, struct_line


def parse_gc_like(text: str, source: str) -> ParseResult:
    lines = text.splitlines()
    types: List[TypeDecl] = []
    others: List[str] = []
    top_decls: List[TopDecl] = []
    index = 0
    type_index = 0
    top_index = 0
    pending_decl: Optional[str] = None
    while index < len(lines):
        stripped = lines[index].strip()
        if stripped.startswith("// Decl:"):
            pending_decl = lines[index].rstrip()
            index += 1
            continue
        if is_gc_type_start(lines, index):
            pending_decl = None
            end = find_type_end(lines, index, "gc")
            size_match = TOTAL_SIZE_RE.match(lines[index])
            open_index = gc_open_index(lines, index)
            open_match = GC_OPEN_RE.match(lines[open_index].strip()) if open_index is not None else None
            assert size_match and open_match
            name, bases = split_name_bases(
                open_match.group("name").strip(), (open_match.group("bases") or "").strip()
            )
            items, decl_file, decl_line = parse_type_body(source, name, lines, index, end)
            types.append(
                TypeDecl(
                    source=source,
                    kind=open_match.group("kind"),
                    name=name,
                    size=parse_int(size_match.group(1)),
                    bases=bases,
                    header_comments=[lines[index].strip()],
                    items=items,
                    original_index=type_index,
                    decl_file=decl_file,
                    decl_line=decl_line,
                )
            )
            type_index += 1
            index = end
            continue
        if TOTAL_SIZE_RE.match(stripped):
            pending_decl = None
            index = find_type_end(lines, index, "gc")
            continue
        if stripped.startswith("// Range:"):
            pending_decl = None
            index = skip_range_block(lines, index)
            continue
        if ENUM_OPEN_RE.match(stripped):
            block, index = collect_brace_block(lines, index)
            if pending_decl is not None:
                block = [pending_decl] + block
            decl_file = None
            decl_line = None
            for line in block:
                decl_file, decl_line = extract_decl_location(line)
                if decl_file is not None:
                    break
            top_decls.append(
                TopDecl(
                    kind="enum",
                    name=enum_name_from_line(stripped),
                    lines=[line.rstrip() for line in block],
                    original_index=top_index,
                    decl_file=decl_file,
                    decl_line=decl_line,
                )
            )
            top_index += 1
            pending_decl = None
            continue
        if "{" in stripped:
            pending_decl = None
            _, index = collect_brace_block(lines, index)
            continue
        if stripped and code_part(stripped).endswith(";"):
            top_decl = extract_top_var_decl(lines[index], top_index)
            if top_decl.name:
                top_decls.append(top_decl)
                top_index += 1
            else:
                others.append(lines[index].rstrip())
            pending_decl = None
        elif stripped:
            pending_decl = None
        index += 1
    return ParseResult(types, others, top_decls)


def parse_functions(text: str) -> List[TopDecl]:
    lines = text.splitlines()
    functions: List[TopDecl] = []
    index = 0
    function_index = 0
    while index < len(lines):
        stripped = lines[index].strip()
        if not stripped.startswith("// Range:"):
            index += 1
            continue

        signature_index = index + 1
        while signature_index < len(lines):
            signature = lines[signature_index].strip()
            if signature and not signature.startswith("//"):
                break
            signature_index += 1

        if signature_index >= len(lines) or "{" not in lines[signature_index]:
            index = skip_range_block(lines, index)
            continue

        block, next_index = collect_brace_block(lines, signature_index)
        full_block = lines[index:signature_index] + block
        functions.append(extract_top_function_decl(full_block, function_index))
        function_index += 1
        index = next_index
    return functions


def virtual_declaration_from_gc_item(
    source_item: DeclItem,
    owner: str,
    visibility: Optional[str],
    original_index: int,
) -> Optional[DeclItem]:
    if not source_item.lines:
        return None
    code = code_part(source_item.lines[-1]).strip()
    if code.endswith("{}"):
        code = code[:-2].rstrip()
    elif code.endswith(";"):
        code = code[:-1].rstrip()
    signature = split_function_signature(code)
    if signature is None:
        return None

    before_params, params, suffix = signature
    before_params = re.sub(r"\b(inline|virtual)\s+", "", before_params).strip()
    suffix = re.sub(r"\b(override|final)\b", "", suffix)
    suffix = re.sub(r"\s+", " ", suffix).strip()
    declaration = f"{before_params}({params})"
    if suffix:
        declaration += " " + suffix
    declaration = "virtual " + declaration + ";"

    item = extract_function_item(
        [], declaration, "gc", visibility, original_index, owner
    )
    item.from_gc_virtual_source = True
    return item


def comparable_function_shape(
    item: DeclItem, owner: str
) -> Optional[Tuple[str, Tuple[str, ...], bool]]:
    types = function_types(item, owner)
    if types is None:
        return None
    return_type, param_types = types

    def comparable_type(type_name: str) -> str:
        type_name = normalize_type_name(type_name)
        return re.sub(r"\b[A-Za-z_]\w*::", "", type_name)

    return (
        comparable_type(return_type),
        tuple(comparable_type(type_name) for type_name in param_types),
        item.match_key.endswith(" const"),
    )


def add_missing_virtual_function_declarations(
    types: Sequence[TypeDecl],
) -> int:
    candidates_by_name: Dict[str, Dict[str, Tuple[TypeDecl, DeclItem]]] = {}
    for source_type in types:
        for item in source_type.items:
            if item.kind != "function" or not item.lines:
                continue
            line = item.lines[-1]
            if "override" not in line and "virtual " not in line:
                continue
            candidates_by_name.setdefault(item.name, {}).setdefault(
                item_match_key(item), (source_type, item)
            )

    added = 0
    for typ in types:
        if typ.paired_ps2 is None:
            continue
        existing_keys = {
            item_match_key(item) for item in typ.items if item.kind == "function"
        }
        next_index = max((item.original_index for item in typ.items), default=-1) + 1
        existing_names = {
            item.name for item in typ.items if item.kind == "function"
        }
        virtuals_by_name: Dict[str, List[DeclItem]] = {}
        for ps2_item in typ.paired_ps2.items:
            if (
                ps2_item.kind != "function"
                or not any("virtual " in line for line in ps2_item.lines)
            ):
                continue
            virtuals_by_name.setdefault(ps2_item.name, []).append(ps2_item)

        for name, ps2_items in virtuals_by_name.items():
            if name in existing_names:
                continue
            candidates = candidates_by_name.get(name, {})
            selected = list(candidates.values())
            if len(selected) > 1 and typ.paired_carbon is not None:
                carbon_shapes = {
                    shape
                    for carbon_item in typ.paired_carbon.items
                    if carbon_item.kind == "function" and carbon_item.name == name
                    for shape in [
                        comparable_function_shape(carbon_item, typ.paired_carbon.name)
                    ]
                    if shape is not None
                }
                matching_carbon = [
                    (source_type, source_item)
                    for source_type, source_item in selected
                    if comparable_function_shape(source_item, source_type.name)
                    in carbon_shapes
                ]
                if not matching_carbon:
                    carbon_type_shapes = {shape[:2] for shape in carbon_shapes}
                    for source_type, source_item in selected:
                        source_shape = comparable_function_shape(
                            source_item, source_type.name
                        )
                        if (
                            source_shape is not None
                            and source_shape[:2] in carbon_type_shapes
                        ):
                            matching_carbon.append((source_type, source_item))
                if matching_carbon:
                    selected = matching_carbon
            if len(selected) > 1:
                ps2_types = function_types(ps2_items[0], typ.paired_ps2.name)
                ps2_return = (
                    normalize_type_name(ps2_types[0]) if ps2_types is not None else ""
                )
                matching_return = []
                for source_type, source_item in selected:
                    source_types = function_types(source_item, source_type.name)
                    source_return = (
                        normalize_type_name(source_types[0])
                        if source_types is not None
                        else ""
                    )
                    if source_return == ps2_return:
                        matching_return.append((source_type, source_item))
                if matching_return:
                    selected = matching_return
            if len(selected) != len(ps2_items):
                continue
            for ps2_item, (_source_type, source_item) in zip(ps2_items, selected):
                item = virtual_declaration_from_gc_item(
                    source_item,
                    typ.name,
                    ps2_item.visibility,
                    next_index,
                )
                if item is None or item_match_key(item) in existing_keys:
                    continue
                typ.items.append(item)
                existing_keys.add(item_match_key(item))
                next_index += 1
                added += 1
    return added


def parse_ps2(text: str) -> ParseResult:
    lines = text.splitlines()
    types: List[TypeDecl] = []
    others: List[str] = []
    index = 0
    type_index = 0
    while index < len(lines):
        stripped = lines[index].strip()
        open_match = PS2_OPEN_RE.match(stripped)
        typedef_match = PS2_TYPEDEF_OPEN_RE.match(stripped)
        if open_match or typedef_match:
            end = find_type_end(lines, index, "ps2")
            close = lines[end - 1].strip() if end > index else ""
            typedef_name = None
            if typedef_match:
                close_match = TYPEDEF_CLOSE_RE.match(close)
                if not close_match:
                    others.append(lines[index].rstrip())
                    index += 1
                    continue
                name = close_match.group("name").strip()
                kind = typedef_match.group("kind")
                size = parse_int(typedef_match.group("size"))
                bases = ""
                typedef_name = name
            else:
                assert open_match
                name, bases = split_name_bases(
                    open_match.group("name").strip(), (open_match.group("bases") or "").strip()
                )
                kind = open_match.group("kind")
                size = parse_int(open_match.group("size"))
            items, decl_file, decl_line = parse_type_body("ps2", name, lines, index, end)
            for item in items:
                item.from_typedef_spelling = typedef_match is not None
            types.append(
                TypeDecl(
                    source="ps2",
                    kind=kind,
                    name=name,
                    size=size,
                    bases=bases,
                    header_comments=[],
                    items=items,
                    original_index=type_index,
                    typedef_name=typedef_name,
                    decl_file=decl_file,
                    decl_line=decl_line,
                )
            )
            type_index += 1
            index = end
            continue
        if stripped:
            others.append(lines[index].rstrip())
        index += 1
    return ParseResult(types, others)


def ps2_function_owner(code: str) -> Optional[str]:
    signature = split_function_signature(code)
    if signature is None:
        return None
    before_params, _params, _suffix = signature
    before_params = before_params.strip()
    if "::" not in before_params:
        return None
    qualifier_index = before_params.rfind("::")
    owner_prefix = before_params[:qualifier_index].strip()
    if not owner_prefix:
        return None
    owner = owner_prefix.split()[-1]
    return owner.strip() or None


def parse_ps2_functions(text: str) -> Ps2FunctionIndex:
    lines = text.splitlines()
    result: Ps2FunctionIndex = {}
    index = 0
    item_index = 0
    while index < len(lines):
        stripped = lines[index].strip()
        code = code_part(stripped)
        owner = ps2_function_owner(code)
        if owner is not None and "{" in code:
            item = extract_function_item([], code, "ps2", None, item_index, owner)
            item.match_key = "function:" + function_match_key(
                code, owner, drop_owner_this=True
            )
            result.setdefault(simple_name(owner), {})
            prefer_item_for_key(result[simple_name(owner)], item_match_key(item), item)
            prefer_item_for_key(result[simple_name(owner)], item.key, item)
            item_index += 1

            depth = count_braces(code)
            index += 1
            while index < len(lines) and depth > 0:
                depth += count_braces(code_part(lines[index]))
                index += 1
            continue
        index += 1
    return result


def index_by_simple_name(types: Iterable[TypeDecl]) -> Dict[str, List[TypeDecl]]:
    result: Dict[str, List[TypeDecl]] = {}
    for typ in types:
        result.setdefault(typ.simple_name, []).append(typ)
    return result


def item_keys(typ: TypeDecl, prefix: Optional[str] = None) -> set[str]:
    keys = set()
    for item in typ.items:
        if prefix is None or item.key.startswith(prefix):
            keys.add(item.key)
    return keys


def score_pair(base: TypeDecl, candidate: TypeDecl) -> int:
    score = 0
    if normalize_type_name(base.name) == normalize_type_name(candidate.name):
        score += 50
    if base.simple_name == candidate.simple_name:
        score += 25
    if base.size is not None and candidate.size is not None:
        score += 15 if base.size == candidate.size else -20
    base_members = item_keys(base, "member:")
    cand_members = item_keys(candidate, "member:")
    score += 4 * len(base_members & cand_members)
    base_static = item_keys(base, "static:")
    cand_static = item_keys(candidate, "static:")
    score += 3 * len(base_static & cand_static)
    base_funcs = item_keys(base, "function:")
    cand_funcs = item_keys(candidate, "function:")
    score += 2 * len(base_funcs & cand_funcs)
    if base_members and cand_members and not (base_members & cand_members):
        score -= 10
    return score


def choose_pair(base: TypeDecl, candidates: Sequence[TypeDecl]) -> Optional[TypeDecl]:
    if not candidates:
        return None
    ranked = sorted(((score_pair(base, cand), cand) for cand in candidates), key=lambda pair: pair[0], reverse=True)
    if not ranked or ranked[0][0] < 25:
        return None
    if len(ranked) > 1 and ranked[0][0] == ranked[1][0]:
        exact = [
            cand
            for score, cand in ranked
            if score == ranked[0][0]
            and normalize_type_name(base.name) == normalize_type_name(cand.name)
        ]
        if len(exact) == 1:
            return exact[0]
        return None
    return ranked[0][1]


def pair_types(
    base_types: List[TypeDecl],
    ps2_types: List[TypeDecl],
    carbon_types: List[TypeDecl],
) -> None:
    ps2_index = index_by_simple_name(ps2_types)
    carbon_index = index_by_simple_name(carbon_types)
    for base in base_types:
        base.paired_ps2 = choose_pair(base, ps2_index.get(base.simple_name, []))
        base.paired_carbon = choose_pair(base, carbon_index.get(base.simple_name, []))
        if base.paired_ps2 is None:
            base.pair_notes.append("no confident PS2 pair")
        if base.paired_carbon is None:
            base.pair_notes.append("no confident Carbon pair")


LocatedDeclIndex = Dict[Tuple[str, str], Tuple[List[int], List[TopDecl]]]


def parse_nested_type_item(item: DeclItem, source: str) -> Optional[TypeDecl]:
    if item.kind not in ("inner", "raw"):
        return None
    parsed = parse_gc_like("\n".join(item.lines), source)
    if len(parsed.types) == 1:
        return parsed.types[0]
    if not item.lines:
        return None
    open_index = 0
    while open_index < len(item.lines) and item.lines[open_index].strip().startswith("//"):
        open_index += 1
    if open_index >= len(item.lines):
        return None
    open_match = GC_OPEN_RE.match(item.lines[open_index].strip())
    if open_match is None:
        return None
    name, bases = split_name_bases(
        open_match.group("name").strip(), (open_match.group("bases") or "").strip()
    )
    temp_lines = ["// total size: 0x0"] + item.lines
    items, decl_file, decl_line = parse_type_body(source, name, temp_lines, 0, len(temp_lines))
    return TypeDecl(
        source=source,
        kind=open_match.group("kind"),
        name=name,
        size=None,
        bases=bases,
        header_comments=[],
        items=items,
        original_index=item.original_index,
        decl_file=decl_file,
        decl_line=decl_line,
    )


def nested_types_from_items(items: Sequence[DeclItem], source: str) -> List[TypeDecl]:
    result: List[TypeDecl] = []
    for item in items:
        nested = parse_nested_type_item(item, source)
        if nested is not None:
            result.append(nested)
    return result


def pair_nested_type_item(
    item: DeclItem,
    owner: TypeDecl,
    ps2_index: Dict[str, List[TypeDecl]],
) -> DeclItem:
    nested = parse_nested_type_item(item, owner.source)
    if nested is None:
        return item

    copied = DeclItem(**{**item.__dict__})
    nested_candidates: List[TypeDecl] = []
    if owner.paired_ps2 is not None:
        nested_candidates.extend(nested_types_from_items(owner.paired_ps2.items, "ps2"))
    nested_candidates.extend(ps2_index.get(nested.simple_name, []))
    nested.paired_ps2 = choose_pair(nested, nested_candidates)

    carbon_candidates: List[TypeDecl] = []
    if owner.paired_carbon is not None:
        carbon_candidates.extend(
            nested_types_from_items(owner.paired_carbon.items, "carbon")
        )
    nested.paired_carbon = choose_pair(nested, carbon_candidates)
    copied.nested_type = nested
    return copied


def decl_file_key(path: Optional[str]) -> str:
    if path is None:
        return ""
    return path.replace("\\", "/").lower()


def same_source_file(left: str, right: str) -> bool:
    return decl_file_key(left) == decl_file_key(right)


def build_located_decl_index(
    located_carbon_by_name: Dict[Tuple[str, str], List[TopDecl]]
) -> Tuple[LocatedDeclIndex, LocatedDeclIndex]:
    by_name: LocatedDeclIndex = {}
    by_name_file: LocatedDeclIndex = {}
    for key, decls in located_carbon_by_name.items():
        ordered = sorted(decls, key=lambda decl: decl.original_index)
        by_name[key] = ([decl.original_index for decl in ordered], ordered)
        by_file: Dict[str, List[TopDecl]] = {}
        for decl in ordered:
            by_file.setdefault(decl_file_key(decl.decl_file), []).append(decl)
        for file_key, file_decls in by_file.items():
            by_name_file[(key[0], key[1] + "\0" + file_key)] = (
                [decl.original_index for decl in file_decls],
                file_decls,
            )
    return by_name, by_name_file


def nearest_directional_decl(
    index: Tuple[List[int], List[TopDecl]],
    original_index: int,
    direction: int,
) -> Optional[TopDecl]:
    indexes, decls = index
    position = bisect.bisect_left(indexes, original_index)
    if direction < 0:
        if position == 0:
            return None
        return decls[position - 1]
    if position >= len(decls):
        return None
    if indexes[position] == original_index:
        position += 1
    if position >= len(decls):
        return None
    return decls[position]


def score_neighbor_decl(candidate: TopDecl, neighbor: TopDecl) -> int:
    score = 1000
    if (
        candidate.decl_file is not None
        and neighbor.decl_file is not None
        and same_source_file(candidate.decl_file, neighbor.decl_file)
    ):
        score += 5000
    score += max(0, 1000 - abs(candidate.original_index - neighbor.original_index))
    return score


def choose_neighbor_matched_top_decl(
    base_decls: List[TopDecl],
    base_index: int,
    candidates: List[TopDecl],
    located_index_by_name: LocatedDeclIndex,
    located_index_by_name_file: LocatedDeclIndex,
    window: int = 5,
) -> Optional[TopDecl]:
    if not candidates:
        return None
    if len(candidates) == 1:
        return candidates[0]

    scores: Dict[int, int] = {id(candidate): 0 for candidate in candidates}
    for offset in range(1, window + 1):
        for direction in (-1, 1):
            neighbor_index = base_index + (direction * offset)
            if neighbor_index < 0 or neighbor_index >= len(base_decls):
                continue
            neighbor = base_decls[neighbor_index]
            neighbor_key = top_decl_pair_key(neighbor)
            neighbor_index_data = located_index_by_name.get(neighbor_key)
            if neighbor_index_data is None:
                continue

            weight = window + 1 - offset
            for candidate in candidates:
                best_score = 0
                same_file_kind, same_file_name = top_decl_pair_key(neighbor)
                same_file_key = (
                    same_file_kind,
                    same_file_name + "\0" + decl_file_key(candidate.decl_file),
                )
                same_file_index_data = located_index_by_name_file.get(same_file_key)
                if same_file_index_data is not None:
                    nearest_same_file = nearest_directional_decl(
                        same_file_index_data, candidate.original_index, direction
                    )
                    if nearest_same_file is not None:
                        best_score = max(
                            best_score, score_neighbor_decl(candidate, nearest_same_file)
                        )
                nearest = nearest_directional_decl(
                    neighbor_index_data, candidate.original_index, direction
                )
                if nearest is not None:
                    best_score = max(best_score, score_neighbor_decl(candidate, nearest))
                scores[id(candidate)] += best_score * weight

    ranked = sorted(candidates, key=lambda candidate: scores[id(candidate)], reverse=True)
    if not ranked or scores[id(ranked[0])] == 0:
        return None
    if len(ranked) > 1 and scores[id(ranked[0])] == scores[id(ranked[1])]:
        return None
    return ranked[0]


def top_decl_has_initializer(decl: TopDecl) -> bool:
    code = top_variable_code(decl)
    return code is not None and initializer_from_code(code) != ""


def choose_initialized_top_decl(
    preferred: Optional[TopDecl], candidates: Sequence[TopDecl]
) -> Optional[TopDecl]:
    if preferred is not None and top_decl_has_initializer(preferred):
        return preferred
    initialized = [candidate for candidate in candidates if top_decl_has_initializer(candidate)]
    if len(initialized) == 1:
        return initialized[0]
    if preferred is not None:
        return preferred
    return None


def top_decl_pair_key(decl: TopDecl) -> Tuple[str, str]:
    if decl.kind == "variable" and decl.match_key:
        return decl.kind, decl.match_key
    return decl.kind, decl.name


def top_function_owner(decl: TopDecl) -> Optional[str]:
    if decl.kind != "function" or "::" not in decl.name:
        return None
    return decl.name.rsplit("::", 1)[0]


def top_function_item(decl: TopDecl, source: str, owner: str) -> Optional[DeclItem]:
    signature_index = top_function_signature_index(decl.lines)
    if signature_index is None:
        return None
    signature = decl.lines[signature_index].strip()
    return DeclItem(
        kind="function",
        lines=[signature],
        name=top_function_name_from_code(signature),
        key="function:" + function_name_from_code(signature, owner),
        match_key="function:" + function_match_key(
            signature, owner, drop_owner_this=source == "carbon"
        ),
        original_index=decl.original_index,
    )


def render_top_function_decl(
    decl: TopDecl, ps2_function_index: Optional[Ps2FunctionIndex] = None
) -> str:
    lines = list(decl.lines)
    signature_index = top_function_signature_index(lines)
    owner = top_function_owner(decl)
    if signature_index is not None and owner is not None:
        item = top_function_item(decl, "gc", owner)
        if item is not None:
            ps2_items = (ps2_function_index or {}).get(simple_name(owner), {})
            ps2_item = ps2_items.get(item.match_key)
            if ps2_item is None:
                ps2_item = ps2_items.get(item.key)
            if ps2_item is not None:
                ps2_types = function_types(ps2_item, owner)
                if ps2_types is not None:
                    item = apply_function_return_spelling(item, ps2_types[0], owner)

            if decl.paired_carbon is not None:
                carbon_item = top_function_item(decl.paired_carbon, "carbon", owner)
                if carbon_item is not None:
                    carbon_types = carbon_function_types(carbon_item, owner)
                    if carbon_types is not None:
                        item = apply_function_return_spelling(
                            item, carbon_types[0], owner
                        )

            lines[signature_index] = item.lines[-1]

    rendered: List[str] = []
    if decl.decl_file is not None and decl.decl_line is not None:
        rendered.append(f"// Decl: {decl.decl_file}:{decl.decl_line}")
    rendered.extend(lines)
    return "\n".join(rendered)


def pair_top_decls(base_decls: List[TopDecl], carbon_decls: List[TopDecl]) -> None:
    carbon_by_name: Dict[Tuple[str, str], List[TopDecl]] = {}
    located_carbon_by_name: Dict[Tuple[str, str], List[TopDecl]] = {}
    for decl in carbon_decls:
        key = top_decl_pair_key(decl)
        carbon_by_name.setdefault(key, []).append(decl)
        if decl.decl_file is not None:
            located_carbon_by_name.setdefault(key, []).append(decl)
    located_index_by_name, located_index_by_name_file = build_located_decl_index(
        located_carbon_by_name
    )
    for index, decl in enumerate(base_decls):
        key = top_decl_pair_key(decl)
        all_candidates = carbon_by_name.get(key, [])
        located_candidates = located_carbon_by_name.get(key, [])
        chosen_location = choose_neighbor_matched_top_decl(
            base_decls,
            index,
            located_candidates or all_candidates,
            located_index_by_name,
            located_index_by_name_file,
        )
        chosen = choose_initialized_top_decl(chosen_location, all_candidates)
        if chosen is None:
            continue
        decl.paired_carbon = chosen
        location_source = chosen_location or chosen
        if location_source.decl_file is not None:
            decl.decl_file = location_source.decl_file
            decl.decl_line = location_source.decl_line


def pair_top_functions(base_functions: List[TopDecl], carbon_functions: List[TopDecl]) -> None:
    carbon_by_key: Dict[str, List[TopDecl]] = {}
    carbon_by_name: Dict[str, List[TopDecl]] = {}
    for decl in carbon_functions:
        if decl.match_key:
            carbon_by_key.setdefault(decl.match_key, []).append(decl)
        carbon_by_name.setdefault(decl.name, []).append(decl)

    previous_file = None
    previous_order_line = None
    for decl in base_functions:
        candidates = carbon_by_key.get(decl.match_key, []) if decl.match_key else []
        if len(candidates) != 1:
            name_candidates = carbon_by_name.get(decl.name, [])
            located_name_candidates = [
                candidate for candidate in name_candidates if candidate.decl_file is not None
            ]
            if len(located_name_candidates) == 1:
                candidates = located_name_candidates
            else:
                candidates = []

        if len(candidates) == 1:
            decl.paired_carbon = candidates[0]
            if candidates[0].decl_file is not None:
                decl.decl_file = candidates[0].decl_file
                decl.decl_line = candidates[0].decl_line

    previous_file: Optional[str] = None
    previous_order_line: Optional[int] = None
    index = 0
    while index < len(base_functions):
        decl = base_functions[index]
        if decl.decl_file is not None and decl.decl_line is not None:
            order_line = decl.decl_line * ORDER_SCALE
            if (
                previous_file is not None
                and same_source_file(decl.decl_file, previous_file)
                and previous_order_line is not None
            ):
                order_line = max(order_line, previous_order_line + 1)
            decl.order_file = decl.decl_file
            decl.order_line = order_line
            previous_file = decl.order_file
            previous_order_line = decl.order_line
            index += 1
            continue

        run_start = index
        while (
            index < len(base_functions)
            and (
                base_functions[index].decl_file is None
                or base_functions[index].decl_line is None
            )
        ):
            index += 1
        run = base_functions[run_start:index]
        if previous_file is not None and previous_order_line is not None:
            for offset, unlocated_decl in enumerate(run, 1):
                unlocated_decl.order_file = previous_file
                unlocated_decl.order_line = previous_order_line + offset
            previous_order_line = run[-1].order_line
        elif index < len(base_functions):
            next_decl = base_functions[index]
            if next_decl.decl_file is not None and next_decl.decl_line is not None:
                first_order_line = max(0, next_decl.decl_line * ORDER_SCALE - len(run))
                for offset, unlocated_decl in enumerate(run):
                    unlocated_decl.order_file = next_decl.decl_file
                    unlocated_decl.order_line = first_order_line + offset

def item_match_key(item: DeclItem) -> str:
    return item.match_key or item.key


def ps2_function_has_omitted_params(item: DeclItem) -> bool:
    if item.kind != "function" or not item.name:
        return False
    return item.match_key == f"function:{item.name}()"


def is_carbon_owner_header_destructor(item: DeclItem, typ: TypeDecl) -> bool:
    owner_name = simple_name(typ.name)
    return (
        typ.source == "carbon"
        and item.kind == "function"
        and item.name == "~" + owner_name
        and item.match_key == f"function:~{owner_name}(int)"
        and item.decl_file is not None
    )


def prefer_item_for_key(
    result: Dict[str, DeclItem], key: str, item: DeclItem
) -> None:
    current = result.get(key)
    if current is None:
        result[key] = item
    elif (
        current.decl_line is None
        and item.decl_line is not None
    ):
        result[key] = item


def items_by_key(typ: Optional[TypeDecl], owner_file: Optional[str] = None) -> Dict[str, DeclItem]:
    if typ is None:
        return {}
    result: Dict[str, DeclItem] = {}
    ps2_omitted_param_functions: Dict[str, Optional[DeclItem]] = {}
    unique_function_names: Dict[str, Optional[DeclItem]] = {}
    for item in typ.items:
        if (
            owner_file is not None
            and item.kind == "function"
            and item.decl_file is not None
            and not same_source_file(item.decl_file, owner_file)
        ):
            continue
        key = item_match_key(item)
        prefer_item_for_key(result, key, item)
        if is_carbon_owner_header_destructor(item, typ):
            prefer_item_for_key(result, f"function:{item.name}()", item)
        if typ.source == "ps2" and ps2_function_has_omitted_params(item):
            current_fallback = ps2_omitted_param_functions.get(item.key)
            if current_fallback is None and item.key not in ps2_omitted_param_functions:
                ps2_omitted_param_functions[item.key] = item
            else:
                ps2_omitted_param_functions[item.key] = None
        if item.kind == "function" and not (
            typ.source == "carbon"
            and owner_file is not None
            and item.decl_file is None
        ):
            current_name = unique_function_names.get(item.key)
            if current_name is None and item.key not in unique_function_names:
                unique_function_names[item.key] = item
            else:
                unique_function_names[item.key] = None
    for key, item in ps2_omitted_param_functions.items():
        if item is not None and key not in result:
            result[key] = item
    if typ.source == "carbon":
        for key, item in unique_function_names.items():
            if item is not None and key not in result:
                result[key] = item
    return result


def member_code(item: DeclItem) -> Optional[str]:
    if not item.lines:
        return None
    stripped = item.lines[-1].strip()
    offset_match = PS2_OFFSET_RE.search(stripped)
    code = offset_match.group(2).strip() if offset_match else code_part(stripped)
    if not code.endswith(";"):
        return None
    return code.rstrip(";").strip()


def ps2_member_code(item: DeclItem) -> Optional[str]:
    return member_code(item)


def array_suffix_from_code(code: str) -> str:
    code = code_without_initializer(code)
    match = re.search(r"((?:\[[^\]]+\])+)\s*;?\s*$", code)
    if match:
        return match.group(1)
    return ""


def apply_ps2_member_spelling(
    base_item: DeclItem, ps2_item: DeclItem, carbon_item: Optional[DeclItem] = None
) -> DeclItem:
    ps2_code = ps2_member_code(ps2_item)
    if not ps2_code or not base_item.name:
        return base_item
    base_line = base_item.lines[-1]
    base_code, base_comment = split_comment(base_line)
    if re.search(r"\(\s*\*+", base_code) or re.search(r"\(\s*\*+", ps2_code):
        return base_item
    array_suffix = array_suffix_from_code(base_code)
    base_type = member_type_from_code(base_code, base_item.name)
    ps2_type = member_type_from_code(ps2_code, ps2_item.name)
    if not ps2_type:
        return base_item
    ps2_type = merge_ps2_member_type(base_type, ps2_type)
    if carbon_item is not None:
        carbon_code = member_code(carbon_item)
        if carbon_code and not re.search(r"\(\s*\*+", carbon_code):
            carbon_type = member_type_from_code(carbon_code, carbon_item.name)
            if carbon_type:
                if (
                    normalize_type_without_const(base_type)
                    == normalize_type_without_const(carbon_type)
                    and normalize_type_without_const(ps2_type)
                    != normalize_type_without_const(carbon_type)
                ):
                    ps2_type = format_type_name(carbon_type)
                elif array_suffix:
                    ps2_type = format_type_name(carbon_type)
                else:
                    ps2_type = merge_carbon_member_type(ps2_type, carbon_type)
    new_code = f"{ps2_type} {base_item.name}{array_suffix};"
    new_code = re.sub(r"\s+\*", " *", new_code)
    new_code = re.sub(r"\s+&", " &", new_code)
    if base_comment:
        new_code += " " + base_comment
    copied = DeclItem(**{**base_item.__dict__})
    copied.lines = base_item.lines[:-1] + [new_code]
    copied.from_typedef_spelling = True
    return copied


def merged_item(
    base_item: DeclItem,
    ps2_items: Dict[str, DeclItem],
    ps2_function_items: Dict[str, DeclItem],
    carbon_items: Dict[str, DeclItem],
    owner: str,
) -> DeclItem:
    item = DeclItem(**{**base_item.__dict__})
    item.lines = list(base_item.lines)
    lookup_key = item_match_key(item)
    ps2_item = ps2_items.get(lookup_key)
    if ps2_item is None and item.kind == "function":
        ps2_item = ps2_items.get(item.key)
    ps2_function_item = None
    if item.kind == "function":
        ps2_function_item = ps2_function_items.get(lookup_key)
        if ps2_function_item is None:
            ps2_function_item = ps2_function_items.get(item.key)
    carbon_item = carbon_items.get(lookup_key)
    if carbon_item is None and item.kind == "function":
        carbon_item = carbon_items.get(item.key)
    if ps2_item is not None and item.kind == "member":
        item = apply_ps2_member_spelling(item, ps2_item, carbon_item)
    if (
        not item.from_gc_virtual_source
        and carbon_item is None
        and ps2_function_item is not None
        and item.kind == "function"
        and ps2_item is not None
        and ps2_function_has_omitted_params(ps2_item)
    ):
        item = apply_ps2_function_spelling(item, ps2_function_item, owner)
    if (
        not item.from_gc_virtual_source
        and carbon_item is not None
        and item.kind == "function"
    ):
        item = apply_carbon_function_spelling(item, carbon_item, owner)
    if carbon_item is not None and carbon_item.visibility is not None:
        item.visibility = carbon_item.visibility
    elif (
        item.visibility is None
        and ps2_item is not None
        and ps2_item.visibility in {"private", "protected"}
    ):
        item.visibility = ps2_item.visibility
    if carbon_item is not None:
        if carbon_item.decl_file is not None:
            item.decl_file = carbon_item.decl_file
        if carbon_item.decl_line is not None:
            item.decl_line = carbon_item.decl_line
    return item


def apply_ps2_omitted_param_overload_visibility(
    items: List[DeclItem], ps2_typ: Optional[TypeDecl]
) -> None:
    if ps2_typ is None:
        return
    ps2_by_name: Dict[str, List[DeclItem]] = {}
    for item in ps2_typ.items:
        if ps2_function_has_omitted_params(item):
            ps2_by_name.setdefault(item.name, []).append(item)
    for name, ps2_items in ps2_by_name.items():
        if len(ps2_items) < 2:
            continue
        candidates = [
            item
            for item in items
            if item.kind == "function" and item.name == name
        ]
        if len(candidates) != len(ps2_items):
            continue
        candidates = sorted(
            candidates,
            key=lambda item: (
                item.decl_line if item.decl_line is not None else 0x7FFFFFFF,
                item.original_index,
            ),
        )
        ps2_items = sorted(ps2_items, key=lambda item: item.original_index)
        for item, ps2_item in zip(candidates, ps2_items):
            if item.visibility is None and ps2_item.visibility is not None:
                item.visibility = ps2_item.visibility


def dedupe_inline_functions(items: List[DeclItem]) -> List[DeclItem]:
    result: List[DeclItem] = []
    seen = set()
    for item in items:
        is_inline = item.kind == "function" and any(
            line.strip().startswith("inline ") for line in item.lines
        )
        dedupe_key = item.key
        if is_inline and item.lines:
            dedupe_key = re.sub(r"\s+", " ", code_part(item.lines[-1])).strip()
        if is_inline and dedupe_key in seen:
            continue
        if is_inline:
            seen.add(dedupe_key)
        result.append(item)
    return result


def source_order_maps(
    typ: Optional[TypeDecl], owner_file: Optional[str] = None
) -> Dict[str, int]:
    if typ is None:
        return {}
    result: Dict[str, int] = {}
    unique_function_names: Dict[str, Optional[DeclItem]] = {}
    for item in typ.items:
        if (
            owner_file is not None
            and item.kind == "function"
            and (
                item.decl_file is None
                or not same_source_file(item.decl_file, owner_file)
            )
        ):
            continue
        result.setdefault(item_match_key(item), item.original_index)
        if is_carbon_owner_header_destructor(item, typ):
            result.setdefault(f"function:{item.name}()", item.original_index)
        if item.kind == "function" and (
            typ.source == "carbon"
            or (typ.source == "ps2" and ps2_function_has_omitted_params(item))
        ):
            current = unique_function_names.get(item.key)
            if current is None and item.key not in unique_function_names:
                unique_function_names[item.key] = item
            else:
                unique_function_names[item.key] = None
    for key, item in unique_function_names.items():
        if item is not None and key not in result:
            result[key] = item.original_index
    return result


def source_order_value(order_map: Dict[str, int], item: DeclItem) -> Optional[int]:
    value = order_map.get(item_match_key(item))
    if value is not None:
        return value
    if item.kind == "function":
        return order_map.get(item.key)
    return None


def source_order_contains(order_map: Dict[str, int], item: DeclItem) -> bool:
    return source_order_value(order_map, item) is not None


def visibility_section_map(
    items: Sequence[DeclItem], explicit_only: bool = False
) -> Dict[str, int]:
    result: Dict[str, int] = {}
    current_visibility: Optional[str] = None
    section = -1
    for item in items:
        if item.visibility is not None and item.visibility != current_visibility:
            current_visibility = item.visibility
            section += 1
        if explicit_only and item.visibility is None:
            continue
        if section < 0:
            section = 0
        result.setdefault(item_match_key(item), section)
    return result


def sorted_items(
    base: TypeDecl, items: List[DeclItem], owner_file: Optional[str]
) -> List[DeclItem]:
    carbon_order = source_order_maps(base.paired_carbon, owner_file)
    ps2_order = source_order_maps(base.paired_ps2)
    carbon_sections = (
        visibility_section_map(base.paired_carbon.items, explicit_only=True)
        if base.paired_carbon is not None
        else {}
    )
    fallback_sections = visibility_section_map(items)
    prefer_ps2_order = any(
        item.kind == "function" and item.from_gc_virtual_source
        for item in items
    ) or any(
        item.kind == "function"
        and source_order_contains(ps2_order, item)
        and not source_order_contains(carbon_order, item)
        for item in items
    )
    function_order: Dict[str, int] = {}
    order_scale = 10000
    unordered_function_order = 0x7FFFFFFF
    if prefer_ps2_order:
        anchors = sorted(
            (carbon_order[key], ps2_order[key])
            for key in ps2_order
            if key in carbon_order
        )
        anchor_carbon = [pair[0] for pair in anchors]
        for item in items:
            if item.kind != "function":
                continue
            order_key = item_match_key(item)
            ps2_value = source_order_value(ps2_order, item)
            carbon_value = source_order_value(carbon_order, item)
            if ps2_value is not None:
                function_order[order_key] = ps2_value * order_scale
                continue
            if carbon_value is not None and anchors:
                carbon_index = carbon_value
                pos = bisect.bisect_right(anchor_carbon, carbon_index)
                if pos > 0 and pos < len(anchors):
                    prev_carbon, prev_ps2 = anchors[pos - 1]
                    next_carbon, next_ps2 = anchors[pos]
                    order = prev_ps2 * order_scale + (carbon_index - prev_carbon)
                    if order >= next_ps2 * order_scale:
                        order = next_ps2 * order_scale - 1
                    function_order[order_key] = order
                    continue
                if pos > 0:
                    prev_carbon, prev_ps2 = anchors[pos - 1]
                    function_order[order_key] = (
                        prev_ps2 * order_scale + (carbon_index - prev_carbon + 1)
                    )
                    continue
                next_carbon, next_ps2 = anchors[0]
                function_order[order_key] = (
                    next_ps2 * order_scale - (next_carbon - carbon_index + 1)
                )
                continue
            function_order[order_key] = unordered_function_order

    known_function_orders: List[int] = []
    for item in items:
        if item.kind != "function":
            continue
        if item.decl_line is not None:
            known_function_orders.append(item.decl_line * order_scale)
            continue
        if prefer_ps2_order:
            order = function_order.get(item_match_key(item))
            if order is not None and order != unordered_function_order:
                known_function_orders.append(order)
                continue
        carbon_value = source_order_value(carbon_order, item)
        if carbon_value is not None:
            known_function_orders.append(carbon_value * order_scale)
            continue
        ps2_value = source_order_value(ps2_order, item)
        if ps2_value is not None:
            known_function_orders.append(ps2_value * order_scale)
    ordered_function_tail = (
        max(known_function_orders) + order_scale
        if known_function_orders
        else unordered_function_order
    )
    if prefer_ps2_order:
        for key, order in list(function_order.items()):
            if order == unordered_function_order:
                function_order[key] = ordered_function_tail
    unordered_function_order = ordered_function_tail

    member_line_order: Dict[str, int] = {}
    for section in sorted(
        {visibility_section_map(items).get(item_match_key(item), 0) for item in items}
    ):
        members = sorted(
            [
                item
                for item in items
                if item.kind == "member"
                and item.offset is not None
                and fallback_sections.get(item.key, 0) == section
            ],
            key=lambda item: (item.offset, item.original_index),
        )
        known = [
            (index, item.decl_line)
            for index, item in enumerate(members)
            if item.decl_line is not None
        ]
        for index, item in enumerate(members):
            if item.decl_line is not None:
                member_line_order[item_match_key(item)] = item.decl_line * order_scale
                continue
            prev_known = None
            next_known = None
            for known_index, known_line in known:
                if known_index < index:
                    prev_known = (known_index, known_line)
                elif known_index > index:
                    next_known = (known_index, known_line)
                    break
            if prev_known is not None:
                member_line_order[item_match_key(item)] = (
                    prev_known[1] * order_scale + (index - prev_known[0])
                )
            elif next_known is not None:
                member_line_order[item_match_key(item)] = (
                    next_known[1] * order_scale - (next_known[0] - index)
                )
            elif source_order_value(carbon_order, item) is not None:
                member_line_order[item_match_key(item)] = (
                    source_order_value(carbon_order, item) * order_scale
                )
            elif source_order_value(ps2_order, item) is not None:
                member_line_order[item_match_key(item)] = (
                    source_order_value(ps2_order, item) * order_scale
                )
            else:
                member_line_order[item_match_key(item)] = item.original_index * order_scale

    def section_index(item: DeclItem) -> int:
        order_key = item_match_key(item)
        if order_key in carbon_sections:
            return carbon_sections[order_key]
        return fallback_sections.get(order_key, 0)

    member_block_order: Dict[int, int] = {}
    for item in items:
        if item.kind != "member" or item.offset is None:
            continue
        section = section_index(item)
        line_order = member_line_order.get(
            item_match_key(item), item.original_index * order_scale
        )
        current = member_block_order.get(section)
        if current is None or line_order < current:
            member_block_order[section] = line_order

    def key(item: DeclItem) -> Tuple[int, int, int, int, int]:
        kind_rank = {"inner": 0, "member": 1, "static": 1, "function": 2}.get(
            item.kind, 3
        )
        if item.kind == "function" and prefer_ps2_order:
            order = function_order.get(item_match_key(item))
            if order is not None:
                return (order, section_index(item), kind_rank, 0, item.original_index)
        if item.kind == "function":
            if item.decl_line is not None:
                return (
                    item.decl_line * order_scale,
                    section_index(item),
                    kind_rank,
                    0,
                    item.original_index,
                )
            carbon_value = source_order_value(carbon_order, item)
            if carbon_value is not None:
                return (
                    carbon_value * order_scale,
                    section_index(item),
                    kind_rank,
                    0,
                    item.original_index,
                )
            ps2_value = source_order_value(ps2_order, item)
            if ps2_value is not None:
                return (
                    ps2_value * order_scale,
                    section_index(item),
                    kind_rank,
                    0,
                    item.original_index,
                )
            return (
                unordered_function_order,
                section_index(item),
                kind_rank,
                0,
                item.original_index,
            )
        if item.kind == "member" and item.offset is not None:
            section = section_index(item)
            return (
                member_block_order.get(section, item.original_index * order_scale),
                section,
                kind_rank,
                item.offset,
                member_line_order.get(item_match_key(item), item.original_index * order_scale),
            )
        if item.decl_line is not None:
            return (
                item.decl_line * order_scale,
                section_index(item),
                kind_rank,
                0,
                item.original_index,
            )
        carbon_value = source_order_value(carbon_order, item)
        if carbon_value is not None:
            return (
                carbon_value * order_scale,
                section_index(item),
                kind_rank,
                0,
                item.original_index,
            )
        ps2_value = source_order_value(ps2_order, item)
        if ps2_value is not None:
            return (
                ps2_value * order_scale,
                section_index(item),
                kind_rank,
                0,
                item.original_index,
            )
        return (
            item.original_index * order_scale,
            section_index(item),
            kind_rank,
            0,
            item.original_index,
        )

    return sorted(items, key=key)


def converted_type_opening(
    stripped: str, class_index: ProStreetClassIndex
) -> Tuple[str, bool]:
    match = RAW_TYPE_OPEN_RE.match(stripped)
    if match is None:
        return stripped, False
    head = match.group("head").strip()
    if "(" in head:
        return stripped, False
    name, bases = split_name_bases(head)
    kind = prostreet_kind_for_name(match.group("kind"), name, class_index)
    if kind == match.group("kind"):
        return stripped, False
    return f"{kind} {name}{bases}{match.group('brace')}", True


def render_type_opening_kind(
    stripped: str, class_index: ProStreetClassIndex
) -> str:
    opening, _converted = converted_type_opening(stripped, class_index)
    return opening


def render_decl_line(
    item: DeclItem,
    line: str,
    class_index: ProStreetClassIndex,
    indent_level: int = 1,
) -> str:
    stripped = line.strip()
    if not stripped:
        return ""
    if COMMENT_SECTION_RE.match(stripped):
        return ""
    if stripped.startswith("// Decl:"):
        return ""
    if stripped.startswith("//"):
        return ("    " * indent_level) + stripped
    stripped = strip_inline_keyword(stripped)
    stripped = render_type_opening_kind(stripped, class_index)
    indent = "    " * indent_level
    if item.decl_file is not None and item.decl_line is not None:
        if "//" in stripped:
            code, comment = split_comment(stripped)
            comment = re.sub(r",?\s*Decl:\s*.*:\d+", "", comment).strip()
            if comment and comment != "//":
                return f"{indent}{code} {comment}, Decl: {item.decl_file}:{item.decl_line}"
            return f"{indent}{code} // Decl: {item.decl_file}:{item.decl_line}"
        return f"{indent}{stripped} // Decl: {item.decl_file}:{item.decl_line}"
    return indent + stripped


def render_item_lines(item: DeclItem, class_index: ProStreetClassIndex) -> List[str]:
    if item.nested_type is not None:
        return [
            ("    " + line) if line else ""
            for line in render_type(item.nested_type, class_index).splitlines()
        ]

    if item.kind not in ("inner", "raw") or len(item.lines) <= 1:
        rendered = []
        for raw_line in item.lines:
            line = render_decl_line(item, raw_line, class_index)
            if line:
                rendered.append(line)
        return rendered

    rendered: List[str] = []
    depth = 0
    for raw_line in item.lines:
        stripped = raw_line.strip()
        if stripped.startswith("}"):
            depth = max(depth - 1, 0)
        converted = False
        if item.kind in ("inner", "raw") and "{" in stripped:
            _opening, converted = converted_type_opening(stripped, class_index)
        line = render_decl_line(item, raw_line, class_index, 1 + depth)
        if line:
            rendered.append(line)
            if converted:
                rendered.append(("    " * (2 + depth)) + "public:")
        code = code_part(stripped)
        opens = code.count("{")
        closes = code.count("}")
        if stripped.startswith("}"):
            closes = max(closes - 1, 0)
        depth = max(depth + opens - closes, 0)
    return rendered


def render_type(
    typ: TypeDecl,
    class_index: ProStreetClassIndex,
    ps2_function_index: Optional[Ps2FunctionIndex] = None,
    ps2_type_index: Optional[Dict[str, List[TypeDecl]]] = None,
) -> str:
    owner_file = typ.decl_file
    if owner_file is None and typ.paired_carbon is not None:
        owner_file = typ.paired_carbon.decl_file
    ps2_type_index = ps2_type_index or {}
    items = [
        pair_nested_type_item(
            merged_item(
                item,
                items_by_key(typ.paired_ps2),
                (ps2_function_index or {}).get(typ.simple_name, {}),
                items_by_key(typ.paired_carbon, owner_file),
                typ.name,
            ),
            typ,
            ps2_type_index,
        )
        for item in typ.items
    ]
    apply_ps2_omitted_param_overload_visibility(items, typ.paired_ps2)
    items = dedupe_inline_functions(items)
    items = sorted_items(typ, items, owner_file)
    lines: List[str] = []
    if typ.size is not None:
        lines.append(f"// total size: 0x{typ.size:X}")
    if typ.decl_file is not None and typ.decl_line is not None:
        lines.append(f"// Decl: {typ.decl_file}:{typ.decl_line}")
    elif typ.paired_carbon and typ.paired_carbon.decl_file and typ.paired_carbon.decl_line:
        lines.append(
            f"// Decl: {typ.paired_carbon.decl_file}:{typ.paired_carbon.decl_line}"
        )
    bases = (" " + typ.bases) if typ.bases else ""
    kind = prostreet_type_kind(typ, class_index)
    lines.append(f"{kind} {typ.name}{bases} {{")
    current_visibility: Optional[str] = None
    previous_kind: Optional[str] = None
    for item in items:
        item_visibility = item.visibility
        if kind == "class" and typ.kind == "struct" and item_visibility is None:
            item_visibility = "public"
        if item_visibility != current_visibility and item_visibility is not None:
            if lines[-1] != f"{kind} {typ.name}{bases} {{" and lines[-1] != "":
                lines.append("")
            lines.append(f"{item_visibility}:")
            current_visibility = item_visibility
            previous_kind = None
        elif previous_kind is not None and (
            item.kind != previous_kind or item.kind == "function"
        ):
            if lines[-1] != "":
                lines.append("")
        lines.extend(render_item_lines(item, class_index))
        previous_kind = item.kind
    if lines and lines[-1] == "":
        lines.pop()
    lines.append("};")
    return "\n".join(lines)


def type_file_and_line(typ: TypeDecl) -> Tuple[str, int]:
    candidates: List[Tuple[str, int]] = []
    if typ.paired_carbon and typ.paired_carbon.decl_file and typ.paired_carbon.decl_line:
        candidates.append((typ.paired_carbon.decl_file, typ.paired_carbon.decl_line))
    if typ.decl_file and typ.decl_line:
        candidates.append((typ.decl_file, typ.decl_line))
    for item in typ.items:
        if item.decl_file and item.decl_line:
            candidates.append((item.decl_file, item.decl_line))
    if candidates:
        return min(candidates, key=lambda pair: pair[1])
    return "<unlocated>", typ.original_index


def top_decl_file_and_line(decl: TopDecl) -> Tuple[str, int]:
    if decl.order_file is not None and decl.order_line is not None:
        return decl.order_file, decl.order_line
    if decl.decl_file is not None:
        if decl.decl_line is not None:
            return decl.decl_file, decl.decl_line * ORDER_SCALE
        return decl.decl_file, decl.original_index * ORDER_SCALE
    return "<top-level declarations>", decl.original_index * ORDER_SCALE


def top_variable_code(decl: TopDecl) -> Optional[str]:
    if decl.kind != "variable" or not decl.lines:
        return None
    code = code_part(decl.lines[-1])
    if not code.endswith(";"):
        return None
    return code.rstrip(";").strip()


def merge_top_variable_code(base: TopDecl, carbon: Optional[TopDecl]) -> str:
    base_code = top_variable_code(base)
    if base_code is None or carbon is None:
        return code_part(base.lines[-1])

    carbon_code = top_variable_code(carbon)
    if carbon_code is None:
        return code_part(base.lines[-1])

    base_type = top_variable_type_from_code(base_code, base.name)
    carbon_type = top_variable_type_from_code(carbon_code, carbon.name)
    if not base_type or not carbon_type:
        return code_part(base.lines[-1])

    type_prefix = ""
    if re.search(r"\bstatic\b", base_type) and not re.search(r"\bstatic\b", carbon_type):
        type_prefix = "static "
    merged_type = format_type_name(type_prefix + carbon_type)
    array_suffix = array_suffix_from_code(base_code)
    initializer = initializer_from_code(carbon_code) or initializer_from_code(base_code)
    if initializer:
        initializer = " " + initializer
    decl_name = top_variable_decl_name_from_code(base_code, base.name) or base.name
    return f"{merged_type} {decl_name}{array_suffix}{initializer};"


def render_top_decl(
    decl: TopDecl, ps2_function_index: Optional[Ps2FunctionIndex] = None
) -> str:
    if decl.kind == "variable":
        carbon = decl.paired_carbon
        line = merge_top_variable_code(decl, carbon)
        _code, comment = split_comment(decl.lines[-1])
        comment = re.sub(r",?\s*Decl:\s*.*:\d+", "", comment).strip()
        if decl.decl_file is not None and decl.decl_line is not None:
            if comment and comment != "//":
                return f"{line} {comment}, Decl: {decl.decl_file}:{decl.decl_line}"
            return f"{line} // Decl: {decl.decl_file}:{decl.decl_line}"
        if comment:
            return f"{line} {comment}"
        return line

    return render_top_function_decl(decl, ps2_function_index)


def type_sort_rank(typ: TypeDecl) -> int:
    if typ.typedef_name is not None:
        return 1
    if typ.paired_ps2 is not None and typ.paired_ps2.typedef_name is not None:
        return 1
    return 2


def render_output(
    base_result: ParseResult,
    class_index: ProStreetClassIndex,
    ps2_function_index: Optional[Ps2FunctionIndex] = None,
    ps2_type_index: Optional[Dict[str, List[TypeDecl]]] = None,
) -> str:
    grouped: Dict[str, Tuple[str, List[Tuple[int, int, object]]]] = {}

    def add_grouped(file_name: str, item: Tuple[int, int, object]) -> None:
        key = decl_file_key(file_name)
        if key not in grouped:
            grouped[key] = (file_name, [])
        grouped[key][1].append(item)

    for typ in base_result.types:
        file_name, line = type_file_and_line(typ)
        add_grouped(file_name, (line * ORDER_SCALE, type_sort_rank(typ), typ))
    for decl in base_result.top_decls:
        file_name, line = top_decl_file_and_line(decl)
        add_grouped(file_name, (line, 0, decl))

    output: List[str] = []
    if base_result.others:
        output.append("// File: <top-level declarations>")
        output.extend(base_result.others)
        output.append("")
    for file_key in sorted(grouped):
        file_name, file_items = grouped[file_key]
        output.append(f"// File: {file_name}")
        for _, _, item in sorted(
            file_items,
            key=lambda pair: (pair[0], pair[1], pair[2].original_index),
        ):
            if isinstance(item, TypeDecl):
                output.append(
                    render_type(item, class_index, ps2_function_index, ps2_type_index)
                )
            else:
                output.append(render_top_decl(item, ps2_function_index))
            output.append("")
    return "\n".join(output).rstrip() + "\n"


def render_unpaired(
    base_types: List[TypeDecl],
    ps2_types: List[TypeDecl],
    carbon_types: List[TypeDecl],
) -> str:
    used_ps2 = {id(typ.paired_ps2) for typ in base_types if typ.paired_ps2 is not None}
    used_carbon = {
        id(typ.paired_carbon) for typ in base_types if typ.paired_carbon is not None
    }
    lines: List[str] = []
    lines.append("// Base structs without a confident pair")
    for typ in base_types:
        if typ.pair_notes:
            lines.append(f"{typ.name} // {', '.join(typ.pair_notes)}")
    lines.append("")
    lines.append("// PS2 structs not used as a pair")
    for typ in ps2_types:
        if id(typ) not in used_ps2:
            size = f" 0x{typ.size:X}" if typ.size is not None else ""
            lines.append(f"{typ.name}{size}")
    lines.append("")
    lines.append("// Carbon structs not used as a pair")
    for typ in carbon_types:
        if id(typ) not in used_carbon:
            loc = ""
            if typ.decl_file and typ.decl_line:
                loc = f" // {typ.decl_file}:{typ.decl_line}"
            size = f" 0x{typ.size:X}" if typ.size is not None else ""
            lines.append(f"{typ.name}{size}{loc}")
    return "\n".join(lines).rstrip() + "\n"


def parse_args(argv: Optional[Sequence[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Unify GC globals.nothpp with PS2 and Carbon debug metadata."
    )
    parser.add_argument("--gc", default=DEFAULT_GC, help="Base GC globals.nothpp")
    parser.add_argument(
        "--gc-functions",
        default=DEFAULT_GC_FUNCTIONS,
        help="Base GC functions.nothpp",
    )
    parser.add_argument("--ps2", default=DEFAULT_PS2, help="PS2 STABS type dump")
    parser.add_argument(
        "--ps2-functions",
        default=DEFAULT_PS2_FUNCTIONS,
        help="PS2 STABS function dump used as a parameter-spelling fallback",
    )
    parser.add_argument("--carbon", default=DEFAULT_CARBON, help="Carbon globals.nothpp")
    parser.add_argument(
        "--carbon-functions",
        default=DEFAULT_CARBON_FUNCTIONS,
        help="Carbon functions.nothpp used for function declaration locations",
    )
    parser.add_argument(
        "--prostreet-classes",
        default=DEFAULT_PROSTREET_CLASSES,
        help="ProStreet class-kind CSV used to upgrade unambiguous structs to classes",
    )
    parser.add_argument(
        "-o",
        "--output",
        default=DEFAULT_OUTPUT,
        help="Unified output file",
    )
    parser.add_argument(
        "--unpaired-output",
        default=DEFAULT_UNPAIRED,
        help="Report file for declarations that could not be paired",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Parse and pair inputs, but do not write output files",
    )
    return parser.parse_args(argv)


def main(argv: Optional[Sequence[str]] = None) -> int:
    args = parse_args(argv)
    gc_result = parse_gc_like(read_text(args.gc), "gc")
    gc_functions = parse_functions(read_text(args.gc_functions))
    ps2_result = parse_ps2(read_text(args.ps2))
    ps2_function_index = parse_ps2_functions(read_text(args.ps2_functions))
    carbon_result = parse_gc_like(read_text(args.carbon), "carbon")
    carbon_functions = parse_functions(read_text(args.carbon_functions))
    prostreet_classes = load_prostreet_class_index(args.prostreet_classes)
    pair_types(gc_result.types, ps2_result.types, carbon_result.types)
    added_virtual_functions = add_missing_virtual_function_declarations(gc_result.types)
    pair_top_decls(gc_result.top_decls, carbon_result.top_decls)
    pair_top_functions(gc_functions, carbon_functions)
    top_decl_count = len(gc_result.top_decls)
    for index, decl in enumerate(gc_functions):
        decl.original_index = top_decl_count + index
    gc_result.top_decls.extend(gc_functions)

    paired_ps2 = sum(1 for typ in gc_result.types if typ.paired_ps2 is not None)
    paired_carbon = sum(1 for typ in gc_result.types if typ.paired_carbon is not None)
    paired_carbon_top_decls = sum(
        1 for decl in gc_result.top_decls[:top_decl_count] if decl.paired_carbon is not None
    )
    paired_carbon_functions = sum(
        1 for decl in gc_functions if decl.paired_carbon is not None
    )
    print(f"GC structs: {len(gc_result.types)}")
    print(f"GC top-level declarations: {len(gc_result.top_decls)}")
    print(f"GC top-level functions: {len(gc_functions)}")
    print(f"PS2 structs: {len(ps2_result.types)}")
    print(f"Carbon structs: {len(carbon_result.types)}")
    print(f"Carbon top-level declarations: {len(carbon_result.top_decls)}")
    print(f"Carbon top-level functions: {len(carbon_functions)}")
    print(f"Paired with PS2: {paired_ps2}")
    print(f"Paired with Carbon: {paired_carbon}")
    print(f"Top-level declarations paired with Carbon: {paired_carbon_top_decls}")
    print(f"Top-level functions paired with Carbon: {paired_carbon_functions}")
    print(f"Virtual function declarations added to GC structs: {added_virtual_functions}")
    print(f"PS2 function owners: {len(ps2_function_index)}")
    print(f"ProStreet class names: {len(prostreet_classes.exact_names)}")

    if not args.dry_run:
        write_text(
            args.output,
            render_output(
                gc_result,
                prostreet_classes,
                ps2_function_index,
                index_by_simple_name(ps2_result.types),
            ),
        )
        write_text(
            args.unpaired_output,
            render_unpaired(gc_result.types, ps2_result.types, carbon_result.types),
        )
        print(f"Wrote {args.output}")
        print(f"Wrote {args.unpaired_output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
