#!/usr/bin/env python3

"""
Compare the original DWARF for one function against the rebuilt DWARF from a unit object.

Examples:
  python tools/dwarf-compare.py -u main/Speed/Indep/SourceLists/zCamera -f "Camera::UpdateAll(float)"
  python tools/dwarf-compare.py -u main/Speed/Indep/SourceLists/zAI -f "AIPursuit::AIPursuit(Sim::Param)" --summary
  python tools/dwarf-compare.py -u main/Speed/Indep/SourceLists/zAttribSys -f "Attrib::Class::RemoveCollection(Attrib::Collection *)" --full-diff
  python tools/dwarf-compare.py -u main/Speed/Indep/SourceLists/zCamera -f "Camera::UpdateAll(float)" --require-exact
"""

import argparse
import contextlib
import difflib
import hashlib
import io
import json
import os
import re
import shutil
import sqlite3
import sys
import tempfile
from typing import Any, Dict, Iterable, List, Optional, Sequence, Set, Tuple

from _common import (
    ROOT_DIR,
    ToolError,
    find_objdiff_unit,
    load_objdiff_config,
    make_abs,
)
from compare_common import build_diff_lines, count_lines_for_opcodes
from dwarf1_gcc_line_info import process_file as export_debug_lines
from lookup import (
    _candidate_func_names,
    _normalise_func_name,
    _sig_contains_name,
    read_text,
    split_functions,
)
from split_dwarf_info import apply_umath_fixups

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
TOOLS_DIR = os.path.join(ROOT_DIR, "tools")
GC_DWARF = os.path.join(ROOT_DIR, "symbols", "Dwarf")
DTK = os.path.join(ROOT_DIR, "build", "tools", "dtk.exe" if os.name == "nt" else "dtk")
HEX_RE = re.compile(r"(?<!\+)0x[0-9A-Fa-f]+")
RANGE_RE = re.compile(r"^(\s*)// Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)")
DEBUG_LINE_RE = re.compile(
    r"^\s*(?:0x)?([0-9A-Fa-f]+):\s*(.+?)\s+\(line\s+(\d+)(?:,\s+column\s+(\d+))?\)\s*$"
)
SIGNATURE_FUNCTION_RE = re.compile(
    r"(?<![\w:])((?:[~A-Za-z_]\w*::)*(?:"
    r"operator\s+(?:new|delete)(?:\[\])?"
    r"|operator\s*(?:\(\)|\[\]|[-+*/%<>=!&|^~,]+)"
    r"|[~A-Za-z_]\w*"
    r"))\s*\("
)
PARAMETER_NAME_RE = re.compile(
    r"(?:(?<=\s)|(?<=[*&]))([A-Za-z_]\w*)\s*(?=(?:\[[^\]]*\])?\s*$)"
)
TYPE_NAME_TOKENS = {
    "bool", "char", "double", "float", "int", "long", "short", "signed",
    "unsigned", "void", "wchar_t", "char8_t", "char16_t", "char32_t",
}
SIGNATURE_REGISTER_RE = re.compile(r"\s*/\*\s*[rf]\d+(?:\s*\+\s*0x[0-9A-Fa-f]+)?\s*\*/")
TYPE_QUALIFIER_RE = re.compile(r"(?:\b[~A-Za-z_]\w*::)+")
SIM_HANDLE_RE = re.compile(r"\b(H(?:[A-Z0-9_]*[A-Z0-9])?)\b")


class DwarfCompareError(RuntimeError):
    pass


FunctionBlock = Tuple[str, str, str, str]
FUNCTION_BLOCK_CACHE_VERSION = 1


def tool_path(name: str) -> str:
    return os.path.join(TOOLS_DIR, name)


def print_section(title: str) -> None:
    print(flush=True)
    print("=" * 60, flush=True)
    print(f"  {title}", flush=True)
    print("=" * 60, flush=True)


def format_failure(
    cmd: Sequence[str], returncode: int, stdout: str = "", stderr: str = ""
) -> str:
    message = [f"Command failed (exit {returncode}): {' '.join(cmd)}"]
    stdout = stdout.strip()
    stderr = stderr.strip()
    if stdout:
        message.append(f"stdout:\n{stdout}")
    if stderr:
        message.append(f"stderr:\n{stderr}")
    return "\n".join(message)


def maybe_remove(path: Optional[str]) -> None:
    if not path:
        return
    try:
        if os.path.exists(path):
            os.remove(path)
    except OSError as e:
        print(f"Warning: failed to remove temporary file {path}: {e}", file=sys.stderr)


def get_unit_build_output(unit_name: str) -> str:
    config = load_objdiff_config()
    unit = find_objdiff_unit(config, unit_name)
    if unit is None:
        raise DwarfCompareError(f"Unit not found in objdiff.json: {unit_name}")

    target = unit.get("base_path") or unit.get("target_path")
    if not target:
        raise DwarfCompareError(
            f"Unit has no build target in objdiff.json: {unit_name}"
        )
    return make_abs(str(target)) or str(target)


def dtk_dwarf_dump(obj_path: str) -> str:
    import tempfile
    import subprocess

    fd, output_path = tempfile.mkstemp(prefix="nfsmw_dwarf_compare_", suffix=".nothpp")
    os.close(fd)
    maybe_remove(output_path)

    result = subprocess.run(
        [DTK, "dwarf", "dump", obj_path, "-o", output_path],
        cwd=ROOT_DIR,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        maybe_remove(output_path)
        raise DwarfCompareError(
            format_failure(
                [DTK, "dwarf", "dump", obj_path, "-o", output_path],
                result.returncode,
                result.stdout,
                result.stderr,
            )
        )

    # tool_output = "\n".join(
    #     part.strip() for part in [result.stdout, result.stderr] if part.strip()
    # )
    # if "ERROR " in tool_output or tool_output.startswith("ERROR"):
    #     maybe_remove(output_path)
    #     raise DwarfCompareError(
    #         f"dtk reported an error while dumping DWARF:\n{tool_output}"
    #     )

    if not os.path.exists(output_path):
        raise DwarfCompareError(
            "dtk dwarf dump succeeded but did not write an output file"
        )

    return output_path


def function_blocks_cache_db_path() -> str:
    return os.path.join(debug_lines_cache_dir(), "dwarf_compare.sqlite3")


def function_blocks_cache_key(path: str, apply_split_fixups_in_ram: bool) -> str:
    hasher = hashlib.sha1()
    hasher.update(f"v{FUNCTION_BLOCK_CACHE_VERSION}:".encode("ascii"))
    hasher.update(b"fixups:" if apply_split_fixups_in_ram else b"plain:")
    with open(path, "rb") as f:
        while chunk := f.read(1024 * 1024):
            hasher.update(chunk)
    return hasher.hexdigest()


def ensure_function_blocks_cache_db(conn: sqlite3.Connection) -> None:
    conn.execute("""
        CREATE TABLE IF NOT EXISTS function_blocks (
            cache_key TEXT NOT NULL,
            ordinal INTEGER NOT NULL,
            start_addr TEXT NOT NULL,
            end_addr TEXT NOT NULL,
            signature_line TEXT NOT NULL,
            block TEXT NOT NULL,
            PRIMARY KEY (cache_key, ordinal)
        )
        """)
    conn.execute("""
        CREATE TABLE IF NOT EXISTS function_block_cache_entries (
            cache_key TEXT PRIMARY KEY,
            source_path TEXT NOT NULL,
            block_count INTEGER NOT NULL
        )
        """)


def load_function_blocks(
    path: str, folder_mode: bool, apply_split_fixups_in_ram: bool = False
) -> List[FunctionBlock]:
    source_path = os.path.join(path, "functions.nothpp") if folder_mode else path
    cache_key = function_blocks_cache_key(source_path, apply_split_fixups_in_ram)
    conn = sqlite3.connect(function_blocks_cache_db_path())
    try:
        ensure_function_blocks_cache_db(conn)
        entry = conn.execute(
            "SELECT block_count FROM function_block_cache_entries WHERE cache_key = ?",
            (cache_key,),
        ).fetchone()
        if entry is not None:
            cached = conn.execute(
                """
                SELECT start_addr, end_addr, signature_line, block
                FROM function_blocks
                WHERE cache_key = ?
                ORDER BY ordinal
                """,
                (cache_key,),
            ).fetchall()
            if len(cached) == entry[0]:
                return cached

        text = read_text(source_path)
        if apply_split_fixups_in_ram:
            # Keep fixups in-memory only (do not rewrite dump files on disk).
            text = apply_umath_fixups(text)
        funcs = split_functions(text)

        # Publish the completion marker last so an interrupted write is never a hit.
        conn.execute("DELETE FROM function_blocks WHERE cache_key = ?", (cache_key,))
        conn.executemany(
            """
            INSERT INTO function_blocks(
                cache_key, ordinal, start_addr, end_addr, signature_line, block
            ) VALUES (?, ?, ?, ?, ?, ?)
            """,
            (
                (cache_key, ordinal, start, end, signature, block)
                for ordinal, (start, end, signature, block) in enumerate(funcs)
            ),
        )
        conn.execute(
            """
            INSERT OR REPLACE INTO function_block_cache_entries(
                cache_key, source_path, block_count
            ) VALUES (?, ?, ?)
            """,
            (cache_key, os.path.abspath(source_path), len(funcs)),
        )
        conn.commit()
        return funcs
    finally:
        conn.close()


def find_function_blocks(
    funcs: Iterable[FunctionBlock], query: str
) -> List[FunctionBlock]:
    function_name = signature_function_name(query) or query
    candidates = _candidate_func_names(function_name)
    matches: List[FunctionBlock] = []
    exact_substring_matches: List[FunctionBlock] = []
    normalized_query = normalize_signature_registers(query)

    for func in funcs:
        sig_line = func[2]
        if normalized_query in normalize_signature_registers(sig_line):
            exact_substring_matches.append(func)
        if any(_sig_contains_name(sig_line, candidate) for candidate in candidates):
            matches.append(func)

    if exact_substring_matches:
        return exact_substring_matches
    return matches


def normalize_signature_registers(signature: str) -> str:
    """Remove DWARF parameter-location annotations from a signature."""
    return SIGNATURE_REGISTER_RE.sub("", signature)


def signature_function_name(signature: str) -> Optional[str]:
    """Extract the qualified function name, excluding its return type."""
    matches = list(SIGNATURE_FUNCTION_RE.finditer(signature))
    return matches[-1].group(1) if matches else None


def build_function_block_index(
    funcs: Iterable[FunctionBlock],
) -> Dict[str, List[FunctionBlock]]:
    index: Dict[str, List[FunctionBlock]] = {}
    for func in funcs:
        matches = list(SIGNATURE_FUNCTION_RE.finditer(func[2]))
        if not matches:
            continue
        function_name = matches[-1].group(1)
        for candidate in _candidate_func_names(function_name):
            index.setdefault(candidate, []).append(func)
    return index


def function_query_from_signature(signature: str) -> Optional[str]:
    """Return the name-and-parameters portion of a DWARF function signature."""
    matches = list(SIGNATURE_FUNCTION_RE.finditer(signature))
    if not matches:
        return None
    match = matches[-1]
    open_at = signature.find("(", match.start(1) + len(match.group(1)))
    if open_at == -1:
        return None
    depth = 0
    for index in range(open_at, len(signature)):
        char = signature[index]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return signature[match.start(1) : index + 1]
    return None


def indexed_function_blocks(
    funcs: List[FunctionBlock],
    index: Dict[str, List[FunctionBlock]],
    query: str,
) -> List[FunctionBlock]:
    bare_name = signature_function_name(query) or query
    indexed: List[FunctionBlock] = []
    seen: Set[Tuple[str, str, str]] = set()
    for candidate in _candidate_func_names(bare_name):
        for func in index.get(candidate, []):
            identity = (func[0], func[1], func[2])
            if identity not in seen:
                seen.add(identity)
                indexed.append(func)
    # Complex operator/template spellings that the lightweight index cannot
    # parse retain the original full-scan behavior.
    return indexed or funcs


def signature_parameters(signature: str, function_name: str) -> List[str]:
    """Extract comma-separated parameters while preserving nested template types."""
    name_at = signature.find(function_name)
    if name_at == -1:
        return []
    open_at = signature.find("(", name_at + len(function_name))
    if open_at == -1:
        return []
    depth = 0
    current: List[str] = []
    parameters: List[str] = []
    for char in signature[open_at + 1 :]:
        if char in "(<[":
            depth += 1
        elif char in ")>]":
            if char == ")" and depth == 0:
                if current or parameters:
                    parameters.append("".join(current))
                break
            depth -= 1
        if char == "," and depth == 0:
            parameters.append("".join(current))
            current = []
        else:
            current.append(char)
    return parameters


def compact_parameter(parameter: str, strip_name: bool = False) -> str:
    had_dwarf_comment = "/*" in parameter
    parameter = re.sub(r"/\*.*?\*/", "", parameter)
    name_matches = list(PARAMETER_NAME_RE.finditer(parameter))
    if (strip_name or had_dwarf_comment) and name_matches and name_matches[-1].group(1) not in TYPE_NAME_TOKENS:
        name = name_matches[-1]
        parameter = parameter[: name.start(1)] + parameter[name.end(1) :]
    has_const = re.search(r"\bconst\b", parameter) is not None
    parameter = re.sub(r"\bconst\b", "", parameter)
    parameter = re.sub(r"\b(?:struct|class|enum)\s+", "", parameter)
    parameter = TYPE_QUALIFIER_RE.sub("", parameter)
    parameter = SIM_HANDLE_RE.sub(r"\1__ *", parameter)
    compact = re.sub(r"\s+", "", parameter.strip())
    return ("const" if has_const else "") + compact


def parameter_type_variants(parameter: str) -> Set[str]:
    variants = {parameter}
    const_prefix = "const" if parameter.startswith("const") else ""
    value = parameter[len(const_prefix):]
    families = (
        (("int8_t", "int8", "i8"), ("signedchar",)),
        (("uint8_t", "uint8", "u8"), ("unsignedchar",)),
        (("int16_t", "int16", "i16"), ("short",)),
        (("uint16_t", "uint16", "u16"), ("unsignedshort", "shortunsignedint")),
        (("int32_t", "int32", "i32"), ("int", "long")),
        (("uint32_t", "uint32", "size_t", "u32"), ("unsignedint", "unsignedlong")),
    )
    for aliases, concrete_types in families:
        alias = next((candidate for candidate in aliases if value.startswith(candidate)), None)
        if alias is None:
            continue
        suffix = value[len(alias):]
        variants.update(const_prefix + concrete + suffix for concrete in concrete_types)
    if value in {"va_list", "__builtin_va_list"}:
        variants.add(const_prefix + "__va_list_tag*")
    return variants


def signature_types_match(query: str, signature: str) -> bool:
    parameter_at = query.rfind("(")
    if parameter_at == -1:
        return True
    function_name = signature_function_name(query)
    if function_name is None:
        function_name = query[:parameter_at].strip()
    if not _sig_contains_name(signature, function_name):
        return False
    query_parameters = signature_parameters(query, function_name)
    dwarf_parameters = signature_parameters(signature, function_name)
    if len(query_parameters) != len(dwarf_parameters):
        return False
    for expected, actual in zip(query_parameters, dwarf_parameters):
        expected_compact = compact_parameter(expected)
        actual_compact = compact_parameter(actual, strip_name=True)
        if expected_compact == "void" and not actual_compact:
            continue
        # DWARF parameters append the source variable name to the type.
        expected_variants = parameter_type_variants(expected_compact)
        actual_variants = parameter_type_variants(actual_compact)
        if not any(
            actual.startswith(expected)
            for expected in expected_variants
            for actual in actual_variants
        ):
            return False
    return True


def last_name_token(query: str) -> str:
    bare = _normalise_func_name(query)
    if "::" in bare:
        return bare.split("::")[-1]
    return bare


def find_similar_signatures(
    funcs: Sequence[FunctionBlock], query: str, limit: int = 8
) -> List[str]:
    token = last_name_token(query)
    token_matches: List[str] = []
    seen = set()

    for _, _, sig_line, _ in funcs:
        if token and token in sig_line and sig_line not in seen:
            token_matches.append(sig_line)
            seen.add(sig_line)
            if len(token_matches) >= limit:
                return token_matches

    choices = [sig_line for _, _, sig_line, _ in funcs if sig_line]
    for sig_line in difflib.get_close_matches(query, choices, n=limit, cutoff=0.35):
        if sig_line not in seen:
            token_matches.append(sig_line)
            seen.add(sig_line)
            if len(token_matches) >= limit:
                break
    return token_matches


def choose_function_block(
    funcs: List[FunctionBlock], query: str, label: str
) -> FunctionBlock:
    matches = find_function_blocks(funcs, query)
    function_name = signature_function_name(query)
    if function_name and "::" in function_name:
        qualified_matches = [
            match
            for match in matches
            if signature_function_name(match[2]) == function_name
        ]
        if qualified_matches:
            matches = qualified_matches
    if matches and "(" in query:
        typed_matches = [
            match for match in matches if signature_types_match(query, match[2])
        ]
        # DWARF often expands a source typedef (for example i32) to its base
        # type, and some toolchains disagree on equivalent 32-bit spellings
        # such as int versus long. Parameter text is only necessary when the
        # qualified function name is overloaded; a unique name match is
        # already unambiguous.
        if typed_matches or len(matches) > 1:
            matches = typed_matches
    if not matches:
        if not funcs:
            raise DwarfCompareError(
                f"{label}: function '{query}' not found.\n"
                "The scanned DWARF source contains no top-level function blocks."
            )

        similar = find_similar_signatures(funcs, query)
        details = [
            f"{label}: function '{query}' not found.",
            f"Scanned {len(funcs)} top-level function block(s).",
        ]
        if similar:
            details.append("Closest signatures:")
            details.extend(f"  - {sig}" for sig in similar)
        raise DwarfCompareError("\n".join(details))
    if len(matches) > 1:
        signatures = "\n".join(f"  - {match[2]}" for match in matches[:8])
        extra = ""
        if len(matches) > 8:
            extra = f"\n  ... {len(matches) - 8} more"
        raise DwarfCompareError(
            f"{label}: function query '{query}' matched multiple DWARF blocks.\n"
            f"Use a more specific function name.\n{signatures}{extra}"
        )
    return matches[0]


def normalize_line(line: str) -> str:
    stripped = line.rstrip("\n").rstrip()
    if stripped.startswith("// Range:"):
        return "// Range: <range>"
    return HEX_RE.sub("0xADDR", stripped)


def normalize_block(block: str) -> List[str]:
    return [normalize_line(line) for line in block.splitlines()]


def canonical_debug_path(debug_path: str) -> str:
    normalized = debug_path.replace("\\", "/").strip()
    lowered = normalized.lower().replace("\\", "/")
    if "/src/" in lowered:
        src_index = lowered.index("/src/")
        suffix = normalized[src_index + len("/src/") :].lstrip("/")
        return os.path.normpath("src/" + suffix).replace("\\", "/")
    if "/speed/indep/" in lowered:
        indep_index = lowered.index("/speed/indep/")
        suffix = normalized[indep_index + len("/speed/indep/") :].lstrip("/")
        return os.path.normpath("src/Speed/Indep/" + suffix.lstrip("/")).replace(
            "\\", "/"
        )
    return os.path.normpath(normalized).replace("\\", "/")


def normalize_source_location(path: str, line_number: int) -> str:
    normalized = os.path.normpath(path.replace("\\", "/")).replace("\\", "/").lower()
    return f"{normalized}:{line_number}"


def debug_lines_cache_dir() -> str:
    cache_dir = os.path.join(tempfile.gettempdir(), "nfsmw_dwarf_compare")
    os.makedirs(cache_dir, exist_ok=True)
    return cache_dir


def debug_lines_cache_db_path(path: str) -> str:
    hasher = hashlib.sha1()
    with open(path, "rb") as f:
        while True:
            chunk = f.read(1024 * 1024)
            if not chunk:
                break
            hasher.update(chunk)
    digest = hasher.hexdigest()
    return os.path.join(debug_lines_cache_dir(), f"debug_lines_{digest}.sqlite3")


def ensure_debug_lines_cache_db(path: str) -> str:
    db_path = debug_lines_cache_db_path(path)
    conn = sqlite3.connect(db_path)
    try:
        conn.execute("PRAGMA journal_mode=OFF")
        conn.execute("PRAGMA synchronous=OFF")
        conn.execute("""
            CREATE TABLE IF NOT EXISTS entries (
                ordinal INTEGER PRIMARY KEY,
                address INTEGER NOT NULL,
                display_path TEXT NOT NULL,
                line_number INTEGER NOT NULL,
                normalized_file TEXT NOT NULL,
                normalized TEXT NOT NULL
            )
            """)
        conn.execute(
            "CREATE INDEX IF NOT EXISTS idx_entries_address ON entries(address)"
        )
        conn.execute("""
            CREATE TABLE IF NOT EXISTS metadata (
                key TEXT PRIMARY KEY,
                value TEXT NOT NULL
            )
            """)
        ready = conn.execute(
            "SELECT value FROM metadata WHERE key = 'ready'"
        ).fetchone()
        if ready is not None and ready[0] == "1":
            return db_path

        conn.execute("DELETE FROM entries")
        conn.execute("DELETE FROM metadata")

        rows: List[Tuple[int, int, str, int, str, str]] = []
        ordinal = 0
        with open(path) as f:
            for raw_line in f:
                line = raw_line.rstrip("\n")
                match = DEBUG_LINE_RE.match(line)
                if match is None:
                    continue
                ordinal += 1
                address = int(match.group(1), 16)
                debug_path = match.group(2)
                line_number = int(match.group(3))
                display_path = canonical_debug_path(debug_path)
                normalized_file = (
                    os.path.normpath(display_path.replace("\\", "/"))
                    .replace("\\", "/")
                    .lower()
                )
                rows.append(
                    (
                        ordinal,
                        address,
                        display_path,
                        line_number,
                        normalized_file,
                        normalize_source_location(display_path, line_number),
                    )
                )

        conn.executemany(
            """
            INSERT INTO entries(
                ordinal, address, display_path, line_number, normalized_file, normalized
            ) VALUES (?, ?, ?, ?, ?, ?)
            """,
            rows,
        )
        conn.execute(
            "INSERT INTO metadata(key, value) VALUES('source_path', ?)",
            (os.path.abspath(path),),
        )
        conn.execute("INSERT INTO metadata(key, value) VALUES('ready', '1')")
        conn.commit()
    finally:
        conn.close()
    return db_path


def load_debug_line_entries(
    path: str, addresses: Set[int]
) -> Dict[int, List[Dict[str, Any]]]:
    entries: Dict[int, List[Dict[str, Any]]] = {}
    if not addresses:
        return entries

    db_path = ensure_debug_lines_cache_db(path)
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    try:
        sorted_addresses = sorted(addresses)
        chunk_size = 900
        for start in range(0, len(sorted_addresses), chunk_size):
            chunk = sorted_addresses[start : start + chunk_size]
            placeholders = ",".join("?" for _ in chunk)
            query = (
                "SELECT address, display_path, line_number, normalized_file, normalized "
                f"FROM entries WHERE address IN ({placeholders}) ORDER BY address, ordinal"
            )
            for row in conn.execute(query, chunk):
                address = int(row["address"])
                entries.setdefault(address, []).append(
                    {
                        "address": address,
                        "display_path": row["display_path"],
                        "line_number": int(row["line_number"]),
                        "normalized_file": row["normalized_file"],
                        "normalized": row["normalized"],
                    }
                )
    finally:
        conn.close()
    return entries


def dedupe_source_locations(locations: Sequence[Dict[str, Any]]) -> List[str]:
    deduped: List[str] = []
    seen = set()
    for entry in locations:
        rendered = f"{entry['display_path']}:{entry['line_number']}"
        if rendered in seen:
            continue
        seen.add(rendered)
        deduped.append(rendered)
    return deduped


def dedupe_source_files(locations: Sequence[Dict[str, Any]]) -> List[str]:
    deduped: List[str] = []
    seen = set()
    for entry in locations:
        normalized_file = entry["normalized_file"]
        if normalized_file in seen:
            continue
        seen.add(normalized_file)
        deduped.append(entry["display_path"])
    return deduped


def render_list(items: Sequence[str], limit: int = 6) -> str:
    if not items:
        return "<none>"
    if len(items) <= limit:
        return ", ".join(items)
    hidden = len(items) - limit
    return f"{', '.join(items[:limit])}, ... (+{hidden} more)"


def build_debug_lines_file_for_object(obj_path: str) -> str:
    temp_dir = tempfile.mkdtemp(prefix="nfsmw_debug_lines_")
    with contextlib.redirect_stdout(io.StringIO()):
        export_debug_lines(obj_path, temp_dir)
    debug_lines_path = os.path.join(temp_dir, "debug_lines.txt")
    if not os.path.exists(debug_lines_path):
        raise DwarfCompareError("failed to export rebuilt debug lines")
    return debug_lines_path


def collect_range_entries(block: str) -> List[Dict[str, Any]]:
    lines = block.splitlines()
    entries: List[Dict[str, Any]] = []
    for idx, line in enumerate(lines):
        match = RANGE_RE.match(line)
        if match is None:
            continue
        signature = ""
        for follow in lines[idx + 1 :]:
            stripped = follow.strip()
            if not stripped or stripped.startswith("//"):
                continue
            signature = stripped.split("{")[0].strip()
            break
        entries.append(
            {
                "line_index": idx + 1,
                "indent": len(match.group(1)) // 4,
                "start_address": int(match.group(2), 16),
                "end_address": int(match.group(3), 16),
                "signature": signature,
            }
        )
    return entries


def normalized_signature_key(signature: str) -> str:
    signature = signature.strip()
    if not signature:
        return "<anonymous>"
    return normalize_line(signature)


def align_range_entries(
    original_entries: Sequence[Dict[str, Any]],
    rebuilt_entries: Sequence[Dict[str, Any]],
) -> List[Tuple[Optional[Dict[str, Any]], Optional[Dict[str, Any]]]]:
    original_keys = [
        f"{entry['indent']}|{normalized_signature_key(entry['signature'])}"
        for entry in original_entries
    ]
    rebuilt_keys = [
        f"{entry['indent']}|{normalized_signature_key(entry['signature'])}"
        for entry in rebuilt_entries
    ]
    matcher = difflib.SequenceMatcher(a=original_keys, b=rebuilt_keys)
    aligned: List[Tuple[Optional[Dict[str, Any]], Optional[Dict[str, Any]]]] = []
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            for orig, reb in zip(original_entries[i1:i2], rebuilt_entries[j1:j2]):
                aligned.append((orig, reb))
        elif tag == "replace":
            max_len = max(i2 - i1, j2 - j1)
            for offset in range(max_len):
                orig = original_entries[i1 + offset] if i1 + offset < i2 else None
                reb = rebuilt_entries[j1 + offset] if j1 + offset < j2 else None
                aligned.append((orig, reb))
        elif tag == "delete":
            for orig in original_entries[i1:i2]:
                aligned.append((orig, None))
        elif tag == "insert":
            for reb in rebuilt_entries[j1:j2]:
                aligned.append((None, reb))
    return aligned


def build_range_source_summary(
    original_block: FunctionBlock,
    rebuilt_block: FunctionBlock,
    rebuilt_debug_lines_path: Optional[str],
) -> Dict[str, Any]:
    if not rebuilt_debug_lines_path or not os.path.exists(rebuilt_debug_lines_path):
        return {"available": False}
    original_debug_lines_path = os.path.join(ROOT_DIR, "symbols", "debug_lines.txt")
    if not os.path.exists(original_debug_lines_path):
        return {"available": False}

    original_entries = collect_range_entries(original_block[3])
    rebuilt_entries = collect_range_entries(rebuilt_block[3])
    aligned_entries = align_range_entries(original_entries, rebuilt_entries)
    original_addresses = {int(entry["start_address"]) for entry in original_entries}
    rebuilt_addresses = {int(entry["start_address"]) for entry in rebuilt_entries}
    original_debug_lines = load_debug_line_entries(
        original_debug_lines_path, original_addresses
    )
    rebuilt_debug_lines = load_debug_line_entries(
        rebuilt_debug_lines_path, rebuilt_addresses
    )

    rows: List[Dict[str, Any]] = []
    file_match_count = 0
    exact_match_count = 0
    for original_entry, rebuilt_entry in aligned_entries:
        original_items = (
            original_debug_lines.get(int(original_entry["start_address"]), [])
            if original_entry is not None
            else []
        )
        rebuilt_items = (
            rebuilt_debug_lines.get(int(rebuilt_entry["start_address"]), [])
            if rebuilt_entry is not None
            else []
        )
        # original_locations = (
        #     dedupe_source_locations(original_items)
        #     if original_entry is not None
        #     else []
        # )
        # rebuilt_locations = (
        #     dedupe_source_locations(rebuilt_items) if rebuilt_entry is not None else []
        # )
        original_files_display = (
            dedupe_source_files(original_items) if original_entry is not None else []
        )
        rebuilt_files_display = (
            dedupe_source_files(rebuilt_items) if rebuilt_entry is not None else []
        )
        original_norm = (
            {item["normalized"] for item in original_items}
            if original_entry is not None
            else set()
        )
        original_files = (
            {item["normalized_file"] for item in original_items}
            if original_entry is not None
            else set()
        )
        rebuilt_norm = (
            {item["normalized"] for item in rebuilt_items}
            if rebuilt_entry is not None
            else set()
        )
        rebuilt_files = (
            {item["normalized_file"] for item in rebuilt_items}
            if rebuilt_entry is not None
            else set()
        )
        common_files = [
            path
            for path in original_files_display
            if os.path.normpath(path.replace("\\", "/")).replace("\\", "/").lower()
            in rebuilt_files
        ]
        original_only_files = [
            path
            for path in original_files_display
            if os.path.normpath(path.replace("\\", "/")).replace("\\", "/").lower()
            not in rebuilt_files
        ]
        rebuilt_only_files = [
            path
            for path in rebuilt_files_display
            if os.path.normpath(path.replace("\\", "/")).replace("\\", "/").lower()
            not in original_files
        ]
        if (
            original_entry is not None
            and rebuilt_entry is not None
            and original_files == rebuilt_files
        ):
            file_status = "match"
            file_match_count += 1
        else:
            file_status = "mismatch"
        if (
            original_entry is not None
            and rebuilt_entry is not None
            and original_norm == rebuilt_norm
        ):
            exact_status = "match"
            exact_match_count += 1
        else:
            exact_status = "mismatch"
        rows.append(
            {
                "file_status": file_status,
                "exact_status": exact_status,
                "indent": (original_entry or rebuilt_entry or {}).get("indent", 0),
                "line_number": (original_entry or rebuilt_entry or {}).get(
                    "line_index"
                ),
                "signature": (original_entry or rebuilt_entry or {}).get(
                    "signature", "<missing>"
                ),
                # "original_locations": original_locations,
                # "rebuilt_locations": rebuilt_locations,
                "common_files": common_files,
                "original_only_files": original_only_files,
                "rebuilt_only_files": rebuilt_only_files,
            }
        )
    return {
        "available": True,
        "rows": rows,
        "total": len(rows),
        "file_matches": file_match_count,
        "exact_matches": exact_match_count,
    }


def build_report(
    unit_name: str,
    function_name: str,
    original_block: FunctionBlock,
    rebuilt_block: FunctionBlock,
    collapse: bool,
    context: int,
    rebuilt_debug_lines_path: Optional[str],
    include_diff: bool = True,
) -> Dict[str, Any]:
    original_raw = original_block[3].splitlines()
    rebuilt_raw = rebuilt_block[3].splitlines()
    original_lines = normalize_block(original_block[3])
    rebuilt_lines = normalize_block(rebuilt_block[3])

    matcher = difflib.SequenceMatcher(a=original_lines, b=rebuilt_lines)
    opcodes = matcher.get_opcodes()
    counts = count_lines_for_opcodes(opcodes)
    total_lines = max(len(original_lines), len(rebuilt_lines), 1)
    match_percent = 100.0 * counts["matching_lines"] / total_lines
    signature_match = normalize_line(original_block[2]) == normalize_line(
        rebuilt_block[2]
    )
    raw_exact_match = original_raw == rebuilt_raw
    normalized_exact_match = original_lines == rebuilt_lines

    diff_lines = (
        build_diff_lines(
            original_lines,
            rebuilt_lines,
            function_name,
            context=context,
            collapse=collapse,
        )
        if include_diff
        else []
    )
    mismatch_summaries: List[str] = []
    for tag, i1, i2, j1, j2 in opcodes if include_diff else []:
        if tag == "equal":
            continue
        original_span = (
            (f"L{i1 + 1:04d}" if i2 - i1 <= 1 else f"L{i1 + 1:04d}-L{i2:04d}")
            if tag in ("replace", "delete")
            else "-"
        )
        rebuilt_span = (
            (f"L{j1 + 1:04d}" if j2 - j1 <= 1 else f"L{j1 + 1:04d}-L{j2:04d}")
            if tag in ("replace", "insert")
            else "-"
        )

        if tag == "replace" and i2 - i1 == 1 and j2 - j1 == 1:
            detail = f"{original_lines[i1]} -> {rebuilt_lines[j1]}"
        elif tag == "delete":
            detail = f"removed {i2 - i1} original line(s)"
        elif tag == "insert":
            detail = f"added {j2 - j1} rebuilt line(s)"
        else:
            detail = (
                f"replaced {i2 - i1} original line(s) with "
                f"{j2 - j1} rebuilt line(s)"
            )
        mismatch_summaries.append(f"- {original_span} -> {rebuilt_span}: {detail}")

    # range_sources = build_range_source_summary(
    #     original_block,
    #     rebuilt_block,
    #     rebuilt_debug_lines_path=rebuilt_debug_lines_path,
    # )

    return {
        "unit": unit_name,
        "function": function_name,
        "match_percent": match_percent,
        "matching_lines": counts["matching_lines"],
        "total_lines": total_lines,
        "original_line_count": len(original_lines),
        "rebuilt_line_count": len(rebuilt_lines),
        "original_only_lines": counts["original_only_lines"],
        "rebuilt_only_lines": counts["rebuilt_only_lines"],
        "changed_groups": counts["changed_groups"],
        "signature_match": signature_match,
        "normalized_exact_match": normalized_exact_match,
        "raw_exact_match": raw_exact_match,
        "original_signature": original_block[2],
        "rebuilt_signature": rebuilt_block[2],
        "original_range": [original_block[0], original_block[1]],
        "rebuilt_range": [rebuilt_block[0], rebuilt_block[1]],
        "mismatch_summaries": mismatch_summaries,
        # "range_sources": range_sources,
        "diff_lines": diff_lines,
    }


def print_summary(report: Dict[str, Any]) -> None:
    print_section(f"DWARF Match: {report['function']}")
    print(f"Unit: {report['unit']}")
    print(
        f"Normalized DWARF match: {report['match_percent']:.1f}% "
        f"({report['matching_lines']}/{report['total_lines']} lines)"
    )
    print(
        f"Signature: {'match' if report['signature_match'] else 'mismatch'} | "
        f"Change groups: {report['changed_groups']} | "
        f"Original-only lines: {report['original_only_lines']} | "
        f"Rebuilt-only lines: {report['rebuilt_only_lines']}"
    )
    print(
        f"Normalized exact match: {'yes' if report['normalized_exact_match'] else 'no'}"
    )
    if report["normalized_exact_match"] and not report["raw_exact_match"]:
        print("Raw textual exact match: no (only raw addresses/ranges differ)")
    else:
        print(
            f"Raw textual exact match: {'yes' if report['raw_exact_match'] else 'no'}"
        )
    print(
        "Address-only range differences are normalized out so the percentage tracks "
        "structural/function-body DWARF changes."
    )
    # if report["range_sources"].get("available"):
    #     line_drifts = (
    #         report["range_sources"]["file_matches"]
    #         - report["range_sources"]["exact_matches"]
    #     )
    #     print(
    #         f"Range source ownership: files agree {report['range_sources']['file_matches']}/"
    #         f"{report['range_sources']['total']}"
    #         f" | file mismatches {report['range_sources']['total'] - report['range_sources']['file_matches']}/"
    #         f"{report['range_sources']['total']}"
    #         f" | line drifts {line_drifts}/{report['range_sources']['total']}"
    #     )
    # else:
    #     print("Range source ownership: unavailable (missing debug-line export data)")
    if not report["signature_match"]:
        print()
        print("Original signature:")
        print(f"  {report['original_signature']}")
        print("Rebuilt signature:")
        print(f"  {report['rebuilt_signature']}")


def print_diff(report: Dict[str, Any]) -> None:
    # if not report["range_sources"].get("available"):
    #     file_mismatches = []
    #     line_drifts = []
    # elif report["range_sources"]["rows"]:
    #     file_mismatches = [
    #         row
    #         for row in report["range_sources"]["rows"]
    #         if row["file_status"] != "match"
    #     ]
    #     line_drifts = [
    #         row
    #         for row in report["range_sources"]["rows"]
    #         if row["file_status"] == "match" and row["exact_status"] != "match"
    #     ]
    # else:
    file_mismatches = []
    line_drifts = []

    if file_mismatches:
        print_section("Range Source Ownership")
        for row in file_mismatches:
            location = f"L{row['line_number']:04d}" if row["line_number"] else "L????"
            print(f"- {location} {row['signature']}")
            if row["common_files"]:
                print(f"  common files:        {render_list(row['common_files'])}")
            if row["original_only_files"]:
                print(
                    f"  original-only files: {render_list(row['original_only_files'])}"
                )
            if row["rebuilt_only_files"]:
                print(
                    f"  rebuilt-only files:  {render_list(row['rebuilt_only_files'])}"
                )
            # print(f"  original lines:      {render_list(row['original_locations'])}")
            # print(f"  rebuilt lines:       {render_list(row['rebuilt_locations'])}")
        if line_drifts:
            print()
            print(
                f"Additionally, {len(line_drifts)}/{report['range_sources']['total']} ranges keep "
                "the same source files but drift on line numbers."
            )
    elif line_drifts:
        print_section("Range Source Ownership")
        print(
            "All range starts resolve to the same source files; line numbers drift "
            f"for {len(line_drifts)}/{report['range_sources']['total']} ranges."
        )
    if report["mismatch_summaries"]:
        print_section("Mismatch Summary")
        for line in report["mismatch_summaries"]:
            print(line)
    print_section("DWARF Diff")
    if not report["diff_lines"]:
        print("No DWARF differences after normalization.")
        return
    for line in report["diff_lines"]:
        print(line)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Compare original and rebuilt DWARF for one function and show a "
            "normalized line-match report plus a diff-like view."
        )
    )
    parser.add_argument("-u", "--unit", required=True, help="Translation unit name")
    functions = parser.add_mutually_exclusive_group(required=True)
    functions.add_argument("-f", "--function", help="Function name to compare")
    functions.add_argument(
        "--functions-file",
        help="Read a JSON array of function names and emit a JSON array of reports",
    )
    parser.add_argument(
        "--summary",
        action="store_true",
        help="Print only the summary header without the diff view",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Print the report as JSON",
    )
    parser.add_argument(
        "-C",
        "--context",
        type=int,
        default=3,
        help="Context lines to keep around collapsed matching runs (default: 3)",
    )
    parser.add_argument(
        "--no-collapse",
        "--full-diff",
        dest="no_collapse",
        action="store_true",
        help="Show the whole normalized DWARF block with diff markers instead of collapsing matching runs",
    )
    parser.add_argument(
        "--rebuilt-dwarf-file",
        help="Use an existing rebuilt DWARF dump instead of dumping the unit object",
    )
    parser.add_argument(
        "--require-exact",
        action="store_true",
        help="Exit non-zero unless the normalized DWARF block matches exactly",
    )
    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    if args.functions_file and not args.json:
        parser.error("--functions-file requires --json")

    if args.functions_file:
        try:
            with open(args.functions_file, encoding="utf-8") as f:
                function_queries = json.load(f)
        except (OSError, json.JSONDecodeError) as e:
            parser.error(f"failed to read --functions-file: {e}")
        if not isinstance(function_queries, list) or not all(
            isinstance(query, str) and query for query in function_queries
        ):
            parser.error(
                "--functions-file must contain a JSON array of non-empty strings"
            )
    else:
        function_queries = [args.function]

    rebuilt_dwarf_path: Optional[str] = None
    rebuilt_debug_lines_path: Optional[str] = None
    cleanup_rebuilt_dwarf = False
    cleanup_rebuilt_debug_lines_dir = False
    try:
        obj_path = get_unit_build_output(args.unit)
        if args.rebuilt_dwarf_file:
            rebuilt_dwarf_path = os.path.abspath(args.rebuilt_dwarf_file)
        else:
            if not os.path.exists(obj_path):
                raise DwarfCompareError(
                    f"Missing built object for {args.unit}: {obj_path}\n"
                    f"Hint: run `python tools/decomp-workflow.py build -u {args.unit}` "
                    "or use the wrapper `python tools/decomp-workflow.py dwarf ...`."
                )
            rebuilt_dwarf_path = dtk_dwarf_dump(obj_path)
            cleanup_rebuilt_dwarf = True

        if os.path.exists(obj_path):
            try:
                rebuilt_debug_lines_path = build_debug_lines_file_for_object(obj_path)
                cleanup_rebuilt_debug_lines_dir = True
            except Exception:
                rebuilt_debug_lines_path = None

        original_funcs = load_function_blocks(GC_DWARF, folder_mode=True)
        rebuilt_funcs = load_function_blocks(
            rebuilt_dwarf_path,
            folder_mode=False,
            apply_split_fixups_in_ram=True,
        )
        original_func_index = build_function_block_index(original_funcs)
        rebuilt_func_index = build_function_block_index(rebuilt_funcs)

        reports: List[Dict[str, Any]] = []
        for function_query in function_queries:
            try:
                original_block = choose_function_block(
                    indexed_function_blocks(
                        original_funcs, original_func_index, function_query
                    ),
                    function_query,
                    "original DWARF",
                )
                rebuilt_block = choose_function_block(
                    indexed_function_blocks(
                        rebuilt_funcs, rebuilt_func_index, function_query
                    ),
                    function_query,
                    "rebuilt DWARF",
                )
                reports.append(
                    build_report(
                        args.unit,
                        function_query,
                        original_block,
                        rebuilt_block,
                        collapse=not args.no_collapse,
                        context=args.context,
                        rebuilt_debug_lines_path=rebuilt_debug_lines_path,
                    )
                )
            except DwarfCompareError as e:
                if not args.functions_file:
                    raise
                reports.append({"function": function_query, "error": str(e)})

        if args.json:
            output: Any = reports if args.functions_file else reports[0]
            print(json.dumps(output, indent=2))
            if args.require_exact and any(
                not report.get("normalized_exact_match", False) for report in reports
            ):
                sys.exit(1)
            return

        report = reports[0]

        print_summary(report)
        if not args.summary:
            print_diff(report)
        if args.require_exact and not report["normalized_exact_match"]:
            sys.exit(1)

    except (DwarfCompareError, ToolError) as e:
        print(e, file=sys.stderr)
        sys.exit(1)
    finally:
        if cleanup_rebuilt_dwarf:
            maybe_remove(rebuilt_dwarf_path)
        if cleanup_rebuilt_debug_lines_dir and rebuilt_debug_lines_path:
            shutil.rmtree(os.path.dirname(rebuilt_debug_lines_path), ignore_errors=True)


if __name__ == "__main__":
    main()
