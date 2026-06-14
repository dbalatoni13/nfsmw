#!/usr/bin/env python3
"""
Compare recovered source declarations against unified DWARF declarations.
Checks type/member spelling, top-level declaration file placement, and order.

Examples:
    python tools/dwarf-audit.py src/Speed/Indep/Src/World/RaceParameters.hpp
    python tools/dwarf-audit.py src/Speed/Indep/Src/World --recursive
    python tools/dwarf-audit.py src --dwarf symbols/Dwarf/dwarf.unified.nothpp
"""

import argparse
import os
import re
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Sequence, Set, Tuple

import lookup


SOURCE_EXTENSIONS = (".h", ".hpp", ".hh", ".c", ".cc", ".cpp", ".cxx")
IGNORED_NAMESPACES = [
    "Attrib::Gen",
    "UMath",
    "RenderConn",
    "Attrib",
    "UTL",
]
IGNORED_TYPES = [
    "intptr_t",
    "uintptr_t",
    "eView *",
]

RE_DECL = re.compile(r"Decl:\s*([^:,][^:]*):(\d+)")
RE_TYPE_START = re.compile(r"^\s*(struct|class)\s+([A-Za-z_]\w*)\b[^;{]*\{")
RE_ENUM_START = re.compile(r"^\s*enum\s+(?:class\s+)?([A-Za-z_]\w*)\b[^;{]*\{")
RE_MEMBER_NAME = re.compile(r"(.+?)\b([A-Za-z_]\w*)\s*(?:\[[^\]]+\])?\s*$")
RE_FUNCTION_NAME = re.compile(r"(.+?)\b([~A-Za-z_]\w*(?:::[~A-Za-z_]\w*)*)\s*\((.*)\)\s*(?:const)?\s*(?:override)?\s*$")
RE_UNIFIED_FILE = re.compile(r"^// File:\s*(.+)$")
RE_BLOCK_COMMENT = re.compile(r"/\*.*?\*/")
RE_LINE_COMMENT = re.compile(r"\s*//.*$")


@dataclass
class DeclLocation:
    path: str
    line: int


@dataclass
class DwarfType:
    kind: str
    name: str
    block: str
    decl: Optional[DeclLocation]
    members: Dict[str, str]
    functions: Dict[str, str]
    function_params: Dict[str, List[str]]


@dataclass
class SourceType:
    kind: str
    name: str
    block: str
    line: int
    members: Dict[str, Tuple[str, int]]
    functions: Dict[str, Tuple[str, int]]
    function_params: Dict[str, Tuple[List[str], int]]


@dataclass
class TopLevelDecl:
    kind: str
    name: str
    key: str
    path: str
    line: int
    order: int


@dataclass
class DwarfIndex:
    types: Dict[Tuple[str, str], List[DwarfType]]
    top_by_file: Dict[str, List[TopLevelDecl]]
    top_by_key: Dict[str, List[TopLevelDecl]]


@dataclass
class Issue:
    path: str
    line: int
    message: str


def clean_code(line: str) -> str:
    if "/*" in line:
        line = RE_BLOCK_COMMENT.sub("", line)
    comment_index = line.find("//")
    if comment_index != -1:
        line = line[:comment_index]
    return line.strip()


def normalize_spaces(text: str) -> str:
    text = re.sub(r"\b(struct|class|enum)\s+", "", text)
    text = re.sub(r"\b(inline|virtual|static|explicit)\s+", "", text)
    text = re.sub(r"\s*([*&])\s*", r" \1 ", text)
    text = re.sub(r"\s+", " ", text)
    return text.strip()


def normalize_template_spaces(type_name: str) -> str:
    result: List[str] = []
    depth = 0
    for char in type_name:
        if char == "<":
            while result and result[-1].isspace():
                result.pop()
            result.append(char)
            depth += 1
            continue
        if char == ">" and depth:
            while result and result[-1].isspace():
                result.pop()
            result.append(char)
            depth -= 1
            continue
        if depth and char.isspace():
            continue
        result.append(char)
    return "".join(result)


def parse_ignored_namespaces() -> List[str]:
    return [namespace.strip().rstrip(":") for namespace in IGNORED_NAMESPACES if namespace.strip()]


def normalize_ignored_namespaces(type_name: str, ignored_namespaces: Sequence[str]) -> str:
    result = type_name
    for namespace in ignored_namespaces:
        result = re.sub(
            r"(?<![A-Za-z_0-9:])%s::(?=[A-Za-z_]\w*\b)" % re.escape(namespace),
            "",
            result,
        )
    return result


def has_namespace_qualifier(type_name: str) -> bool:
    return re.search(r"\b[A-Za-z_]\w*::[A-Za-z_]\w*\b", type_name) is not None


def normalize_missing_namespace_qualifiers(type_name: str) -> str:
    return re.sub(r"(?<![A-Za-z_0-9:])(?:[A-Za-z_]\w*::)+(?=[A-Za-z_]\w*\b)", "", type_name)


def normalize_cv_qualifiers(type_name: str) -> str:
    type_name = re.sub(r"\b(const|volatile)\b", "", type_name)
    type_name = re.sub(r"\s+", " ", type_name)
    return type_name.strip()


def normalize_ignored_type(type_name: str) -> str:
    return normalize_cv_qualifiers(normalize_template_spaces(type_name))


def is_ignored_type(type_name: str) -> bool:
    return normalize_ignored_type(type_name) in IGNORED_TYPES


def types_match(
    left: str,
    right: str,
    ignored_namespaces: Sequence[str],
    ignore_missing_namespaces: bool,
) -> bool:
    if left == right:
        return True
    normalized_left = normalize_ignored_namespaces(left, ignored_namespaces)
    normalized_right = normalize_ignored_namespaces(right, ignored_namespaces)
    if normalized_left == normalized_right:
        return True
    template_normalized_left = normalize_template_spaces(normalized_left)
    template_normalized_right = normalize_template_spaces(normalized_right)
    if template_normalized_left == template_normalized_right:
        return True
    left_uses_namespace = has_namespace_qualifier(template_normalized_left)
    right_uses_namespace = has_namespace_qualifier(template_normalized_right)
    if ignore_missing_namespaces and left_uses_namespace != right_uses_namespace:
        namespace_normalized_left = normalize_missing_namespace_qualifiers(template_normalized_left)
        namespace_normalized_right = normalize_missing_namespace_qualifiers(template_normalized_right)
        if namespace_normalized_left == namespace_normalized_right:
            return True
    if is_ignored_type(template_normalized_left) or is_ignored_type(template_normalized_right):
        return True
    if template_normalized_left == "short" and template_normalized_right == "short int":
        return True
    if template_normalized_left == "unsigned short" and template_normalized_right == "short unsigned int":
        return True
    if normalized_left != left or normalized_right != right:
        return normalize_cv_qualifiers(template_normalized_left) == normalize_cv_qualifiers(
            template_normalized_right
        )
    return False


def normalize_path(path: str) -> str:
    path = path.replace("\\", "/")
    path = re.sub(r"/+", "/", path)
    parts: List[str] = []
    for part in path.split("/"):
        if not part or part == ".":
            continue
        if part == "..":
            if parts:
                parts.pop()
            continue
        parts.append(part)
    normalized = "/".join(parts).lower()
    if normalized.startswith("src/"):
        normalized = normalized[4:]
    return normalized


def source_decl_path(path: str, root: str) -> str:
    abs_path = os.path.abspath(path)
    try:
        rel = os.path.relpath(abs_path, root)
    except ValueError:
        rel = path
    return normalize_path(rel)


def parse_decl(text: str) -> Optional[DeclLocation]:
    match = RE_DECL.search(text)
    if not match:
        return None
    return DeclLocation(normalize_path(match.group(1)), int(match.group(2)))


def type_decl_line(block: str) -> str:
    for line in block.splitlines():
        code = clean_code(line)
        if code.startswith(("struct ", "class ", "enum ")):
            return code
    return ""


def normalize_function_signature_code(signature: str) -> str:
    code = clean_code(signature)
    code = re.sub(r"\s*\{\s*\}\s*$", "", code)
    code = re.sub(r"\s*\{\s*$", "", code)
    code = re.sub(r"\)\s*:\s*.*$", ")", code)
    close_index = code.rfind(")")
    if close_index != -1:
        suffix = code[close_index + 1 :].strip()
        if suffix.startswith(":"):
            code = code[: close_index + 1]
    return code.rstrip(";").strip()


def signature_key(signature: str) -> Optional[str]:
    code = normalize_function_signature_code(signature)
    match = RE_FUNCTION_NAME.match(code)
    if not match:
        return None
    name = match.group(2).split("::")[-1]
    params = normalize_template_spaces(normalize_spaces(match.group(3)))
    suffix = " const" if re.search(r"\)\s*const\b", code) else ""
    return "%s(%s)%s" % (name, params, suffix)


def split_params(params: str) -> List[str]:
    params = params.strip()
    if not params or params == "void":
        return []
    result: List[str] = []
    current: List[str] = []
    depth = 0
    for char in params:
        if char == "<":
            depth += 1
        elif char == ">" and depth:
            depth -= 1
        if char == "," and depth == 0:
            result.append("".join(current).strip())
            current = []
        else:
            current.append(char)
    if current:
        result.append("".join(current).strip())
    return result


def strip_param_name(param: str) -> str:
    param = re.sub(r"\s*=.*$", "", param).strip()
    param = re.sub(r"\[[^\]]*\]\s*$", "", param).strip()
    normalized = normalize_spaces(param)
    if normalized.endswith(("*", "&")):
        return normalized
    match = re.match(r"(.+?)\s+([A-Za-z_]\w*)$", normalized)
    if not match:
        return normalized
    typ = match.group(1).strip()
    name = match.group(2)
    if typ.endswith("::") or name in ("const", "volatile"):
        return normalized
    return typ


def function_arity_key(signature: str) -> Optional[Tuple[str, List[str]]]:
    code = normalize_function_signature_code(signature)
    match = RE_FUNCTION_NAME.match(code)
    if not match:
        return None
    name = match.group(2).split("::")[-1]
    param_types = [strip_param_name(param) for param in split_params(match.group(3))]
    suffix = " const" if re.search(r"\)\s*const\b", code) else ""
    return "%s/%d%s" % (name, len(param_types), suffix), param_types


def parse_field_decl(line: str) -> Optional[Tuple[str, str]]:
    code = clean_code(line)
    if not code.endswith(";"):
        return None
    code = code[:-1].strip()
    if not code or code in ("public:", "protected:", "private:"):
        return None
    if "{" in code or "}" in code:
        return None
    if code.startswith(("typedef ", "static ", "virtual ", "friend ", "enum ", "struct ", "class ")):
        return None
    if code.startswith(("return ", "delete ", "new ")):
        return None
    if "(" in code or ")" in code or "=" in code:
        return None
    match = RE_MEMBER_NAME.match(code)
    if not match:
        return None
    return match.group(2), normalize_spaces(match.group(1))


def parse_function_decl(line: str) -> Optional[Tuple[str, str]]:
    code = clean_code(line)
    if "(" not in code or ")" not in code:
        return None
    if code.startswith(("typedef ", "friend ")):
        return None
    key = signature_key(code)
    if not key:
        return None
    before_name = RE_FUNCTION_NAME.match(code.rstrip(";"))
    if not before_name:
        return None
    return key, normalize_spaces(before_name.group(1))


def top_level_name_from_signature(signature: str) -> str:
    code = normalize_function_signature_code(signature)
    match = RE_FUNCTION_NAME.match(code)
    if not match:
        return code
    return match.group(2)


def top_level_function_key(signature: str) -> Optional[str]:
    arity_key = function_arity_key(signature)
    if not arity_key:
        return None
    local_key, param_types = arity_key
    name = top_level_name_from_signature(signature)
    suffix = " const" if local_key.endswith(" const") else ""
    if "::" not in name:
        return "function:%s(%s)%s" % (name, ",".join(param_types), suffix)
    return "function:%s(%s)%s" % (name, ",".join(param_types), suffix)


def top_level_member_function_name(type_name: str, signature: str) -> Optional[str]:
    code = normalize_function_signature_code(signature)
    match = RE_FUNCTION_NAME.match(code)
    if not match:
        return None
    name = match.group(2).split("::")[-1]
    return "%s::%s" % (type_name, name)


def top_level_member_function_key(type_name: str, signature: str) -> Optional[str]:
    arity_key = function_arity_key(signature)
    if not arity_key:
        return None
    local_key, param_types = arity_key
    name = top_level_member_function_name(type_name, signature)
    if name is None:
        return None
    suffix = " const" if local_key.endswith(" const") else ""
    return "function:%s(%s)%s" % (name, ",".join(param_types), suffix)


def top_level_variable_key(line: str) -> Optional[Tuple[str, str]]:
    code = clean_code(line)
    if not code.endswith(";"):
        return None
    code = code[:-1].strip()
    if code.startswith(("typedef ", "friend ", "enum ", "struct ", "class ", "using ")):
        return None
    code = re.sub(r"\s*=.*$", "", code).strip()
    if code.endswith(")"):
        depth = 0
        initializer_start = -1
        for index in range(len(code) - 1, -1, -1):
            char = code[index]
            if char == ")":
                depth += 1
            elif char == "(":
                depth -= 1
                if depth == 0:
                    initializer_start = index
                    break
        if initializer_start != -1:
            before_initializer = code[:initializer_start].rstrip()
            if re.match(r".+\b[A-Za-z_]\w*$", before_initializer):
                code = before_initializer
    if not code or "{" in code or "}" in code or "(" in code or ")" in code:
        return None
    code = re.sub(r"\[[^\]]*\]\s*$", "", code).strip()
    match = RE_MEMBER_NAME.match(code)
    if not match:
        return None
    name = match.group(2)
    return "variable:" + name, name


def constructed_variable_key(line: str) -> Optional[Tuple[str, str]]:
    code = clean_code(line)
    if not code.endswith(";") or "=" in code or not code.endswith(");"):
        return None
    close_index = len(code) - 2
    depth = 0
    open_index = -1
    for index in range(close_index, -1, -1):
        char = code[index]
        if char == ")":
            depth += 1
        elif char == "(":
            depth -= 1
            if depth == 0:
                open_index = index
                break
    if open_index == -1:
        return None
    args = code[open_index + 1 : close_index].strip()
    if not args or not re.search(r"['\"]|\b\d|[.+\-*/&|]", args):
        return None
    return top_level_variable_key(line)


def top_level_typedef_key(lines: Sequence[str]) -> Optional[Tuple[str, str]]:
    code_lines = [clean_code(line) for line in lines if clean_code(line)]
    if not code_lines:
        return None
    code = " ".join(code_lines)
    if not code.startswith("typedef ") or not code.endswith(";"):
        return None

    match = re.search(r"\(\s*\*?\s*([A-Za-z_]\w*)\s*\)\s*\(", code)
    if match:
        name = match.group(1)
        return "typedef:" + name, name

    close_match = re.search(r"\}\s*([A-Za-z_]\w*)\s*;", code)
    if close_match:
        name = close_match.group(1)
        return "typedef:" + name, name

    stripped = re.sub(r"\[[^\]]*\]\s*$", "", code[:-1].strip())
    if "(" in stripped:
        before_params = stripped.split("(", 1)[0].strip()
        name = before_params.split()[-1] if before_params.split() else ""
    else:
        name = stripped.split()[-1] if stripped.split() else ""
    if not name or name in ("struct", "class", "enum"):
        return None
    return "typedef:" + name, name


def typedef_embedded_type_key(lines: Sequence[str]) -> Optional[Tuple[str, str, str]]:
    code_lines = [clean_code(line) for line in lines if clean_code(line)]
    if not code_lines:
        return None
    code = " ".join(code_lines)
    match = re.match(r"typedef\s+(struct|class|enum)\s+([A-Za-z_]\w*)\b.*\{", code)
    if not match:
        return None
    kind = match.group(1)
    name = match.group(2)
    return kind, name, top_level_type_key(kind, name)


def top_level_type_key(kind: str, name: str) -> str:
    if kind in ("struct", "class"):
        return "type:" + name
    return kind + ":" + name


def parse_function_params(line: str) -> Optional[Tuple[str, List[str]]]:
    code = clean_code(line)
    if "(" not in code or ")" not in code:
        return None
    if code.startswith(("typedef ", "friend ")):
        return None
    return function_arity_key(code)


def parse_members(block: str) -> Dict[str, str]:
    result: Dict[str, str] = {}
    for line in block.splitlines():
        parsed = parse_field_decl(line)
        if parsed:
            name, typ = parsed
            result.setdefault(name, typ)
    return result


def parse_functions(block: str) -> Dict[str, str]:
    result: Dict[str, str] = {}
    for line in block.splitlines():
        parsed = parse_function_decl(line)
        if parsed:
            name, return_type = parsed
            result.setdefault(name, return_type)
    return result


def parse_function_params_map(block: str) -> Dict[str, List[str]]:
    result: Dict[str, List[str]] = {}
    for line in block.splitlines():
        parsed = parse_function_params(line)
        if parsed:
            name, params = parsed
            result.setdefault(name, params)
    return result


def iter_type_body_lines(block: str) -> Iterable[Tuple[int, str]]:
    depth = 0
    for index, line in enumerate(block.splitlines()):
        if depth == 1:
            yield index, line
        depth += line.count("{") - line.count("}")


def line_number_for_offset(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def brace_delta(line: str) -> int:
    return line.count("{") - line.count("}")


def collect_brace_block(lines: Sequence[str], start: int) -> Tuple[List[str], int]:
    block = [lines[start]]
    depth = brace_delta(lines[start])
    index = start + 1
    while index < len(lines) and depth > 0:
        block.append(lines[index])
        depth += brace_delta(lines[index])
        index += 1
    return block, index


def first_code_line(lines: Sequence[str]) -> str:
    for line in lines:
        code = clean_code(line)
        if code:
            return code
    return ""


def find_function_body_brace(lines: Sequence[str], start: int, end: int) -> Optional[int]:
    index = start
    while index < end:
        code = clean_code(lines[index])
        if not code:
            index += 1
            continue
        if "{" in code:
            return index
        if ";" in code:
            return None
        index += 1
    return None


def unified_decl_line(lines: Sequence[str]) -> int:
    for line in lines:
        decl = parse_decl(line)
        if decl:
            return decl.line
    return 0


def leading_metadata_start(lines: Sequence[str], index: int) -> int:
    while index > 0 and lines[index - 1].strip().startswith("//"):
        index -= 1
    return index


def add_top_decl(
    entries: List[TopLevelDecl],
    kind: str,
    name: str,
    key: str,
    path: str,
    line: int,
) -> None:
    entries.append(TopLevelDecl(kind, name, key, path, line, len(entries)))


def parse_unified_file_entries(path: str, section_lines: Sequence[str]) -> List[TopLevelDecl]:
    entries: List[TopLevelDecl] = []
    index = 0
    while index < len(section_lines):
        line = section_lines[index]
        stripped = line.strip()
        code = clean_code(line)
        type_match = RE_TYPE_START.match(line)
        enum_match = RE_ENUM_START.match(line)

        if code.startswith("typedef "):
            metadata_start = leading_metadata_start(section_lines, index)
            if "{" in code and not code.endswith(";"):
                block, index = collect_brace_block(section_lines, index)
            else:
                block = [line]
                index += 1
            parsed_typedef = top_level_typedef_key(block)
            if parsed_typedef:
                decl_line = unified_decl_line(
                    list(section_lines[metadata_start : index - len(block)]) + block
                )
                key, name = parsed_typedef
                if decl_line:
                    add_top_decl(entries, "typedef", name, key, path, decl_line)
            continue

        if type_match or enum_match:
            metadata_start = leading_metadata_start(section_lines, index)
            block, index = collect_brace_block(section_lines, index)
            if type_match:
                kind = type_match.group(1)
                name = type_match.group(2)
            else:
                kind = "enum"
                name = enum_match.group(1)
            decl_line = unified_decl_line(
                list(section_lines[metadata_start : index - len(block)]) + block
            )
            if decl_line:
                add_top_decl(
                    entries,
                    kind,
                    name,
                    top_level_type_key(kind, name),
                    path,
                    decl_line,
                )
            continue

        if stripped.startswith("// Range:"):
            metadata_start = leading_metadata_start(section_lines, index)
            signature_index = index + 1
            while signature_index < len(section_lines):
                signature = clean_code(section_lines[signature_index])
                if signature:
                    break
                signature_index += 1
            if signature_index < len(section_lines):
                block, next_index = collect_brace_block(section_lines, signature_index)
                full_block = list(section_lines[metadata_start:signature_index]) + block
                signature = first_code_line(block)
                key = top_level_function_key(signature)
                decl_line = unified_decl_line(full_block)
                if key and decl_line:
                    add_top_decl(
                        entries,
                        "function",
                        top_level_name_from_signature(signature),
                        key,
                        path,
                        decl_line,
                    )
                index = next_index
                continue

        parsed_var = top_level_variable_key(line)
        if parsed_var:
            metadata_start = leading_metadata_start(section_lines, index)
            key, name = parsed_var
            decl_line = unified_decl_line(list(section_lines[metadata_start:index]) + [line])
            if decl_line:
                add_top_decl(entries, "variable", name, key, path, decl_line)
            index += 1
            continue

        if code and "{" in code:
            _block, index = collect_brace_block(section_lines, index)
            continue
        index += 1
    return entries


def load_dwarf_top_level_decls(path: str) -> Tuple[Dict[str, List[TopLevelDecl]], Dict[str, List[TopLevelDecl]]]:
    lines = lookup.read_text(path).splitlines()
    by_file: Dict[str, List[TopLevelDecl]] = {}
    by_key: Dict[str, List[TopLevelDecl]] = {}
    current_file: Optional[str] = None
    current_lines: List[str] = []

    def flush() -> None:
        if current_file is None:
            return
        normalized_file = normalize_path(current_file)
        entries = parse_unified_file_entries(normalized_file, current_lines)
        by_file[normalized_file] = entries
        for entry in entries:
            by_key.setdefault(entry.key, []).append(entry)

    for line in lines:
        match = RE_UNIFIED_FILE.match(line)
        if match:
            flush()
            current_file = match.group(1).strip()
            current_lines = []
            continue
        if current_file is not None:
            current_lines.append(line)
    flush()
    return by_file, by_key


def parse_source_top_level_decls(text: str, path: str) -> List[TopLevelDecl]:
    lines = text.splitlines()
    entries: List[TopLevelDecl] = []

    def add_inline_member_function_decls(
        block: Sequence[str],
        type_name: str,
        type_start_line: int,
        path: str,
    ) -> None:
        block_text = "\n".join(block)
        for local_index, member_line in iter_type_body_lines(block_text):
            code = clean_code(member_line)
            if "(" not in code or ")" not in code or "{" not in code:
                continue
            if code.startswith(("typedef ", "friend ")):
                continue
            key = top_level_member_function_key(type_name, code)
            name = top_level_member_function_name(type_name, code)
            if key and name:
                add_top_decl(
                    entries,
                    "function",
                    name,
                    key,
                    path,
                    type_start_line + local_index,
                )

    def parse_range(start: int, end: int, base_line: int) -> None:
        index = start
        while index < end:
            line = lines[index]
            stripped = line.strip()
            code = clean_code(line)
            if not code:
                index += 1
                continue

            if re.match(r"^\s*namespace(?:\s+[A-Za-z_]\w*)?\s*\{", line):
                block, next_index = collect_brace_block(lines, index)
                if len(block) > 2:
                    parse_range(index + 1, next_index - 1, base_line)
                index = next_index
                continue

            type_match = RE_TYPE_START.match(line)
            enum_match = RE_ENUM_START.match(line)
            if code.startswith("typedef "):
                if "{" in code and not code.endswith(";"):
                    block, next_index = collect_brace_block(lines, index)
                else:
                    block = [line]
                    next_index = index + 1
                embedded_type = typedef_embedded_type_key(block)
                if embedded_type:
                    kind, name, key = embedded_type
                    add_top_decl(
                        entries,
                        kind,
                        name,
                        key,
                        path,
                        base_line + index,
                    )
                parsed_typedef = top_level_typedef_key(block)
                if parsed_typedef:
                    key, name = parsed_typedef
                    add_top_decl(
                        entries,
                        "typedef",
                        name,
                        key,
                        path,
                        base_line + index,
                    )
                index = next_index
                continue
            if type_match or enum_match:
                _block, next_index = collect_brace_block(lines, index)
                if type_match:
                    kind = type_match.group(1)
                    name = type_match.group(2)
                else:
                    kind = "enum"
                    name = enum_match.group(1)
                add_top_decl(
                    entries,
                    kind,
                    name,
                    top_level_type_key(kind, name),
                    path,
                    base_line + index,
                )
                if type_match:
                    add_inline_member_function_decls(
                        _block,
                        name,
                        base_line + index,
                        path,
                    )
                index = next_index
                continue

            constructed_var = constructed_variable_key(line)
            if constructed_var:
                key, name = constructed_var
                add_top_decl(entries, "variable", name, key, path, base_line + index)
                index += 1
                continue

            if "(" in code and not code.startswith(("typedef ", "friend ")):
                signature_lines = [line]
                signature_end = index
                while (
                    signature_end + 1 < end
                    and ")" not in " ".join(clean_code(item) for item in signature_lines)
                ):
                    signature_end += 1
                    signature_lines.append(lines[signature_end])

                signature_text = " ".join(clean_code(item) for item in signature_lines)
                key = top_level_function_key(signature_text)
                if key:
                    brace_index = find_function_body_brace(lines, index, end)
                    if brace_index is not None:
                        add_top_decl(
                            entries,
                            "function",
                            top_level_name_from_signature(signature_text),
                            key,
                            path,
                            base_line + index,
                        )
                        _block, index = collect_brace_block(lines, brace_index)
                    else:
                        index = signature_end + 1
                    continue

            if not code.endswith(";") and "=" in code:
                statement_lines = [line]
                statement_end = index
                while statement_end + 1 < end and ";" not in clean_code(lines[statement_end]):
                    statement_end += 1
                    statement_lines.append(lines[statement_end])
                statement_text = " ".join(clean_code(item) for item in statement_lines)
                parsed_var = top_level_variable_key(statement_text)
                if parsed_var:
                    key, name = parsed_var
                    add_top_decl(entries, "variable", name, key, path, base_line + index)
                    index = statement_end + 1
                    continue

            parsed_var = top_level_variable_key(line)
            if parsed_var:
                key, name = parsed_var
                add_top_decl(entries, "variable", name, key, path, base_line + index)
                index += 1
                continue

            if "{" in code:
                _block, index = collect_brace_block(lines, index)
                continue
            index += 1

    parse_range(0, len(lines), 1)
    return entries


def split_source_types(text: str) -> List[SourceType]:
    result: List[SourceType] = []
    lines = text.splitlines(True)
    offset = 0
    index = 0
    while index < len(lines):
        line = lines[index]
        enum_match = RE_ENUM_START.match(line)
        type_match = RE_TYPE_START.match(line)
        match = type_match or enum_match
        if not match:
            offset += len(line)
            index += 1
            continue

        kind = "enum" if enum_match else match.group(1)
        name = match.group(1) if enum_match else match.group(2)
        start_offset = offset
        current = [line]
        depth = line.count("{") - line.count("}")
        offset += len(line)
        index += 1
        while index < len(lines) and depth > 0:
            line = lines[index]
            current.append(line)
            depth += line.count("{") - line.count("}")
            offset += len(line)
            index += 1
        block = "".join(current)
        members: Dict[str, Tuple[str, int]] = {}
        functions: Dict[str, Tuple[str, int]] = {}
        function_params: Dict[str, Tuple[List[str], int]] = {}
        block_line = line_number_for_offset(text, start_offset)
        for local_index, member_line in iter_type_body_lines(block):
            parsed_member = parse_field_decl(member_line)
            if parsed_member:
                member_name, member_type = parsed_member
                members.setdefault(member_name, (member_type, block_line + local_index))
            parsed_func = parse_function_decl(member_line)
            if parsed_func:
                func_name, return_type = parsed_func
                functions.setdefault(func_name, (return_type, block_line + local_index))
            parsed_params = parse_function_params(member_line)
            if parsed_params:
                func_name, params = parsed_params
                function_params.setdefault(func_name, (params, block_line + local_index))
        result.append(SourceType(kind, name, block, block_line, members, functions, function_params))
    return result


def load_dwarf_types(path: str) -> Dict[Tuple[str, str], List[DwarfType]]:
    text = lookup.read_text(path)
    by_key: Dict[Tuple[str, str], List[DwarfType]] = {}
    for name, block in lookup.split_structs(text):
        decl = parse_decl(block)
        line = type_decl_line(block)
        kind = "class" if line.startswith("class ") else "struct"
        item = DwarfType(
            kind,
            name,
            block,
            decl,
            parse_members(block),
            parse_functions(block),
            parse_function_params_map(block),
        )
        by_key.setdefault((kind, name), []).append(item)
        if kind in ("struct", "class"):
            by_key.setdefault(("type", name), []).append(item)
    for name, block in lookup.split_enums(text):
        simple_name = name.split("::")[-1]
        item = DwarfType("enum", simple_name, block, parse_decl(block), {}, {}, {})
        by_key.setdefault(("enum", simple_name), []).append(item)
    return by_key


def load_dwarf_index(path: str) -> DwarfIndex:
    types = load_dwarf_types(path)
    top_by_file, top_by_key = load_dwarf_top_level_decls(path)
    return DwarfIndex(types, top_by_file, top_by_key)


def candidate_dwarf_types(
    dwarf_types: Dict[Tuple[str, str], List[DwarfType]], source_type: SourceType
) -> List[DwarfType]:
    if source_type.kind in ("struct", "class"):
        return dwarf_types.get((source_type.kind, source_type.name), []) or dwarf_types.get(
            ("type", source_type.name), []
        )
    return dwarf_types.get((source_type.kind, source_type.name), [])


def choose_dwarf_type(candidates: List[DwarfType], current_path: str) -> Optional[DwarfType]:
    if not candidates:
        return None
    for candidate in candidates:
        if candidate.decl and candidate.decl.path == current_path:
            return candidate
    return candidates[0]


def unique_expected_path(entries: Sequence[TopLevelDecl]) -> Optional[str]:
    paths = {
        entry.path
        for entry in entries
        if not (entry.path.startswith("<") and entry.path.endswith(">"))
    }
    if len(paths) == 1:
        return next(iter(paths))
    return None


def expected_entries_by_key(entries: Sequence[TopLevelDecl]) -> Dict[str, List[TopLevelDecl]]:
    result: Dict[str, List[TopLevelDecl]] = {}
    for entry in entries:
        result.setdefault(entry.key, []).append(entry)
    return result


def choose_expected_entry(
    entries_by_key: Dict[str, List[TopLevelDecl]],
    used_counts: Dict[str, int],
    source_entry: TopLevelDecl,
) -> Optional[TopLevelDecl]:
    candidates = entries_by_key.get(source_entry.key)
    if not candidates:
        return None
    used = used_counts.get(source_entry.key, 0)
    used_counts[source_entry.key] = used + 1
    if used < len(candidates):
        return candidates[used]
    return candidates[-1]


def source_entry_counts(source_entries: Sequence[TopLevelDecl]) -> Dict[str, int]:
    counts: Dict[str, int] = {}
    for entry in source_entries:
        counts[entry.key] = counts.get(entry.key, 0) + 1
    return counts


def top_level_loose_key(entry: TopLevelDecl) -> str:
    if entry.kind != "function":
        return entry.key
    match = re.match(r"^function:.*\((.*)\)( const)?$", entry.key)
    if not match:
        return entry.key
    return "function:%s/%d%s" % (
        entry.name,
        len(split_params(match.group(1))),
        match.group(2) or "",
    )


def source_entry_loose_counts(source_entries: Sequence[TopLevelDecl]) -> Dict[str, int]:
    counts: Dict[str, int] = {}
    for entry in source_entries:
        key = top_level_loose_key(entry)
        counts[key] = counts.get(key, 0) + 1
    return counts


def report_missing_top_level_decls(
    path: str,
    source_counts: Dict[str, int],
    source_loose_counts: Dict[str, int],
    expected_entries: Sequence[TopLevelDecl],
) -> List[Issue]:
    issues: List[Issue] = []
    seen_expected: Dict[str, int] = {}
    seen_loose_expected: Dict[str, int] = {}
    for expected in expected_entries:
        seen = seen_expected.get(expected.key, 0) + 1
        seen_expected[expected.key] = seen
        if seen <= source_counts.get(expected.key, 0):
            continue
        loose_key = top_level_loose_key(expected)
        seen_loose = seen_loose_expected.get(loose_key, 0) + 1
        seen_loose_expected[loose_key] = seen_loose
        if expected.kind == "function" and seen_loose <= source_loose_counts.get(loose_key, 0):
            continue
        issues.append(
            Issue(
                path,
                expected.line if expected.line > 0 else 1,
                "missing %s %s expected by unified DWARF"
                % (expected.kind, expected.name),
            )
        )
    return issues


def audit_top_level_order(
    path: str,
    current_path: str,
    source_entries: Sequence[TopLevelDecl],
    dwarf_index: DwarfIndex,
) -> List[Issue]:
    issues: List[Issue] = []
    expected_entries = dwarf_index.top_by_file.get(current_path, [])
    expected_by_key = expected_entries_by_key(expected_entries)
    source_counts = source_entry_counts(source_entries)
    source_loose_counts = source_entry_loose_counts(source_entries)
    used_counts: Dict[str, int] = {}
    ordered_matches: List[Tuple[TopLevelDecl, TopLevelDecl]] = []
    reported_wrong_file: Set[str] = set()

    issues.extend(
        report_missing_top_level_decls(
            path, source_counts, source_loose_counts, expected_entries
        )
    )

    for source_entry in source_entries:
        expected_entry = choose_expected_entry(expected_by_key, used_counts, source_entry)
        if expected_entry is not None:
            ordered_matches.append((source_entry, expected_entry))
            continue

        global_candidates = dwarf_index.top_by_key.get(source_entry.key, [])
        expected_path = unique_expected_path(global_candidates)
        if expected_path is None or expected_path == current_path:
            continue
        report_key = "%s:%s" % (source_entry.key, expected_path)
        if report_key in reported_wrong_file:
            continue
        reported_wrong_file.add(report_key)
        issues.append(
            Issue(
                path,
                source_entry.line,
                "%s %s is declared in %s, but unified DWARF places it in %s"
                % (source_entry.kind, source_entry.name, current_path, expected_path),
            )
        )

    previous_source: Optional[TopLevelDecl] = None
    previous_expected: Optional[TopLevelDecl] = None
    for source_entry, expected_entry in ordered_matches:
        if previous_expected is not None and expected_entry.order < previous_expected.order:
            assert previous_source is not None
            issues.append(
                Issue(
                    path,
                    source_entry.line,
                    "%s %s is ordered before %s %s in source, but unified DWARF expects the opposite"
                    % (
                        previous_source.kind,
                        previous_source.name,
                        source_entry.kind,
                        source_entry.name,
                    ),
                )
            )
        previous_source = source_entry
        previous_expected = expected_entry
    return issues


def audit_source_file(
    path: str,
    root: str,
    dwarf_index: DwarfIndex,
    ignored_namespaces: Sequence[str],
    ignore_missing_namespaces: bool,
) -> List[Issue]:
    with open(path, "r", encoding="utf-8", errors="ignore") as handle:
        text = handle.read()
    current_path = source_decl_path(path, root)
    issues: List[Issue] = []
    issues.extend(
        audit_top_level_order(
            path,
            current_path,
            parse_source_top_level_decls(text, current_path),
            dwarf_index,
        )
    )
    for source_type in split_source_types(text):
        candidates = candidate_dwarf_types(dwarf_index.types, source_type)
        if not candidates:
            continue
        declared_candidates = [item for item in candidates if item.decl]
        if declared_candidates and not any(item.decl and item.decl.path == current_path for item in candidates):
            expected = declared_candidates[0].decl
            assert expected is not None
            issues.append(
                Issue(
                    path,
                    source_type.line,
                    "%s %s is declared in %s, but DWARF Decl points to %s:%d"
                    % (source_type.kind, source_type.name, current_path, expected.path, expected.line),
                )
            )

        dwarf_type = choose_dwarf_type(candidates, current_path)
        if not dwarf_type:
            continue
        for member_name, (source_member_type, line) in source_type.members.items():
            dwarf_member_type = dwarf_type.members.get(member_name)
            if dwarf_member_type and not types_match(
                source_member_type, dwarf_member_type, ignored_namespaces, ignore_missing_namespaces
            ):
                issues.append(
                    Issue(
                        path,
                        line,
                        "%s::%s has type '%s', but DWARF uses '%s'"
                        % (source_type.name, member_name, source_member_type, dwarf_member_type),
                    )
                )
        for func_name, (source_return_type, line) in source_type.functions.items():
            dwarf_return_type = dwarf_type.functions.get(func_name)
            if dwarf_return_type and not types_match(
                source_return_type, dwarf_return_type, ignored_namespaces, ignore_missing_namespaces
            ):
                issues.append(
                    Issue(
                        path,
                        line,
                        "%s::%s has return type '%s', but DWARF uses '%s'"
                        % (source_type.name, func_name, source_return_type, dwarf_return_type),
                    )
                )
        for func_name, (source_params, line) in source_type.function_params.items():
            dwarf_params = dwarf_type.function_params.get(func_name)
            if not dwarf_params:
                continue
            for param_index, (source_param, dwarf_param) in enumerate(
                zip(source_params, dwarf_params), 1
            ):
                if not types_match(
                    source_param,
                    dwarf_param,
                    ignored_namespaces,
                    ignore_missing_namespaces,
                ):
                    issues.append(
                        Issue(
                            path,
                            line,
                            "%s::%s parameter %d has type '%s', but DWARF uses '%s'"
                            % (source_type.name, func_name, param_index, source_param, dwarf_param),
                        )
                    )
    return issues


def iter_source_files(path: str, recursive: bool) -> Iterable[str]:
    if os.path.isfile(path):
        if path.lower().endswith(SOURCE_EXTENSIONS):
            yield path
        return
    if not os.path.isdir(path):
        return
    if recursive:
        for root, dirs, files in os.walk(path):
            dirs[:] = [d for d in dirs if d not in (".git", "build", "__pycache__")]
            for filename in files:
                full_path = os.path.join(root, filename)
                if full_path.lower().endswith(SOURCE_EXTENSIONS):
                    yield full_path
    else:
        for filename in os.listdir(path):
            full_path = os.path.join(path, filename)
            if os.path.isfile(full_path) and full_path.lower().endswith(SOURCE_EXTENSIONS):
                yield full_path


def build_parser() -> argparse.ArgumentParser:
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    parser = argparse.ArgumentParser(
        description=(
            "Audit source declarations against DWARF Decl, type spelling metadata, "
            "and top-level declaration order."
        )
    )
    parser.add_argument("path", help="Source file or directory to audit")
    parser.add_argument(
        "-r",
        "--recursive",
        action="store_true",
        help="Recursively audit a directory",
    )
    parser.add_argument(
        "--dwarf",
        default=os.path.join(root, "symbols", "Dwarf", "dwarf.unified.nothpp"),
        help="Unified DWARF file to compare against",
    )
    parser.add_argument(
        "--root",
        default=root,
        help="Repository root used for source path normalization",
    )
    parser.add_argument(
        "--strict-namespaces",
        action="store_true",
        help="Report type mismatches caused only by one side having namespace qualifiers",
    )
    return parser


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    root = os.path.abspath(args.root)
    ignored_namespaces = parse_ignored_namespaces()
    ignore_missing_namespaces = not args.strict_namespaces
    dwarf_index = load_dwarf_index(args.dwarf)
    paths = list(iter_source_files(args.path, args.recursive))
    if not paths:
        print("No source files found.", file=sys.stderr)
        return 1

    issues: List[Issue] = []
    for path in paths:
        issues.extend(
            audit_source_file(
                path,
                root,
                dwarf_index,
                ignored_namespaces,
                ignore_missing_namespaces,
            )
        )

    for issue in issues:
        print("%s:%d: %s" % (issue.path, issue.line, issue.message))
    if issues:
        print("%d issue(s) found." % len(issues))
        return 1
    print("No DWARF declaration issues found.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
