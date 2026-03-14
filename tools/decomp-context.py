#!/usr/bin/env python3

"""
Function context gatherer for decomp work.

Collects the common function context an agent needs for matching work:
source excerpt, objdiff status/diff, GC DWARF lookup, and Ghidra decompile.

Usage:
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --no-ghidra
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --ghidra-version gc
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --lookup-mode signature
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --no-lookup
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --no-source
  python tools/decomp-context.py --ghidra-check
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from typing import Any, Dict, List, Optional, Tuple
from _common import (
    RELOC_DIFF_CHOICES,
    ROOT_DIR,
    ToolError,
    build_objdiff_symbol_rows,
    fail,
    load_objdiff_config,
    run_objdiff_json,
)

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = ROOT_DIR

OBJDIFF_CLI = os.path.join(root_dir, "build", "tools", "objdiff-cli")
DTK = os.path.join(root_dir, "build", "tools", "dtk")
GC_SYMBOLS_FILE = os.path.join(root_dir, "config", "GOWE69", "symbols.txt")
PS2_SYMBOLS_FILE = os.path.join(root_dir, "config", "SLES-53558-A124", "symbols.txt")
GC_DWARF_PATH = os.path.join(root_dir, "symbols", "Dwarf")
DEBUG_LINES_FILE = os.path.join(root_dir, "symbols", "debug_lines.txt")
GC_GHIDRA_PROGRAM = "NFSMWRELEASE.ELF"
PS2_GHIDRA_PROGRAM = "NFS.ELF"

# Number of lines of context to show before/after the matched function in source
SOURCE_CONTEXT_LINES = 5
RELATED_SOURCE_LIMIT = 8
BRIEF_RELATED_SOURCE_LIMIT = 3
BRIEF_SUGGESTED_COMMAND_LIMIT = 2
LOW_UNMATCHED_HINT_THRESHOLD = 192
HIGH_MATCH_HINT_THRESHOLD = 85.0
LARGER_TARGET_RATIO = 3
LARGER_TARGET_MIN_BYTES = 192


def load_project_config() -> Dict[str, Any]:
    """Load objdiff.json."""
    return load_objdiff_config()


def find_unit(config: Dict[str, Any], unit_name: str) -> Optional[Dict[str, Any]]:
    """Find a unit in the project config."""
    for unit in config.get("units", []):
        if unit["name"] == unit_name:
            return unit
    return None


def run_objdiff(
    unit_name: str,
    base_obj: Optional[str] = None,
    reloc_diffs: str = "none",
) -> Optional[Dict[str, Any]]:
    return run_objdiff_json(
        OBJDIFF_CLI,
        unit_name,
        base_obj=base_obj,
        reloc_diffs=reloc_diffs,
        root_dir=root_dir,
    )


def find_symbol_in_diff(
    diff_data: Dict[str, Any], func_name: str
) -> Tuple[Optional[Dict], Optional[Dict]]:
    """Find a symbol by name in the diff data. Returns (left_sym, right_sym).

    Uses target_symbol as a direct index into the other side's symbol array.
    """
    left_syms = diff_data.get("left", {}).get("symbols", [])
    right_syms = diff_data.get("right", {}).get("symbols", [])

    pattern = func_name.lower()

    for sym in left_syms:
        name = sym.get("demangled_name", sym.get("name", ""))
        mangled = sym.get("name", "")
        if pattern in name.lower() or pattern in mangled.lower():
            right_sym = None
            ts = sym.get("target_symbol")
            if ts is not None and ts < len(right_syms):
                right_sym = right_syms[ts]
            return sym, right_sym

    for sym in right_syms:
        name = sym.get("demangled_name", sym.get("name", ""))
        mangled = sym.get("name", "")
        if pattern in name.lower() or pattern in mangled.lower():
            left_sym = None
            ts = sym.get("target_symbol")
            if ts is not None and ts < len(left_syms):
                left_sym = left_syms[ts]
            return left_sym, sym

    return None, None


def describe_symbol_presence(
    left_sym: Optional[Dict[str, Any]], right_sym: Optional[Dict[str, Any]]
) -> str:
    if left_sym is not None and right_sym is None:
        return "missing in decomp output"
    if left_sym is None and right_sym is not None:
        return "extra in decomp output"
    return "present on both sides"


def lookup_symbol_address(file: str, mangled_name: str) -> Optional[str]:
    """Look up a symbol's address from symbols.txt."""
    if not os.path.exists(file):
        return None
    pattern = re.compile(
        r"^" + re.escape(mangled_name) + r"\s*=\s*(?:\.(\w+):)?0x([0-9A-Fa-f]+)"
    )
    with open(file) as f:
        for line in f:
            m = pattern.match(line.strip())
            if m:
                return m.group(2)
    return None


def format_hex_address(address: str) -> str:
    return address if address.lower().startswith("0x") else f"0x{address}"


def lookup_function_dwarf(query: str) -> Tuple[Optional[str], Optional[str]]:
    """Query the split GC DWARF dump for one function."""
    if not os.path.exists(GC_DWARF_PATH):
        return None, f"DWARF dump not found: {GC_DWARF_PATH}"

    cmd = [
        sys.executable,
        os.path.join(script_dir, "lookup.py"),
        GC_DWARF_PATH,
        "function",
        query,
    ]
    result = subprocess.run(cmd, capture_output=True, cwd=root_dir, text=True)
    if result.returncode == 0:
        return result.stdout.strip(), None

    detail = result.stderr.strip() or result.stdout.strip() or "lookup failed"
    return None, detail


def compact_dwarf_function_text(text: str) -> str:
    lines = [line.rstrip() for line in text.splitlines()]
    kept: List[str] = []
    signature = None

    for line in lines:
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith("//"):
            if stripped.startswith("// Range:") or stripped.startswith("// this:"):
                kept.append(line)
            continue
        signature = line
        break

    if signature is not None:
        kept.append(signature)

    return "\n".join(kept) if kept else text


def search_symbols_file(file: str, name: str) -> List[Tuple[str, str, str]]:
    """Search symbols.txt for entries matching a name. Returns [(mangled, section, address)]."""
    if not os.path.exists(file):
        return []
    results = []
    pattern = name.lower()
    with open(file) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            if pattern in line.lower():
                m = re.match(r"^(\S+)\s*=\s*(?:\.(\w+):)?0x([0-9A-Fa-f]+)", line)
                if m:
                    results.append((m.group(1), m.group(2), m.group(3)))
    return results


def ghidra_decompile(address: str, program: str) -> Tuple[Optional[str], Optional[str]]:
    """Decompile a function at the given address using Ghidra CLI.

    Returns (code, error_message). On success error_message is None.
    On failure code is None and error_message describes the problem.
    """
    ghidra_cmd = shutil.which("ghidra-cli") or shutil.which("ghidra")
    if ghidra_cmd is None:
        return None, "ghidra-cli / ghidra not found in PATH"

    try:
        result = subprocess.run(
            [
                ghidra_cmd,
                "decompile",
                "--program",
                program,
                f"0x{address}",
                "--json",
            ],
            capture_output=True,
            timeout=30,
        )
        if result.returncode != 0:
            stderr = result.stderr.decode(errors="replace").strip()
            msg = f"ghidra exited with code {result.returncode}"
            if stderr:
                msg += f"\n  stderr: {stderr}"
            return None, msg
        data = json.loads(result.stdout)
        if data and isinstance(data, list) and len(data) > 0:
            return data[0].get("code", ""), None
        return None, "ghidra returned empty result"
    except subprocess.TimeoutExpired:
        return None, "ghidra timed out after 30s"
    except FileNotFoundError:
        return None, f"ghidra binary not executable: {ghidra_cmd}"
    except json.JSONDecodeError as e:
        return None, f"ghidra returned invalid JSON: {e}"


def check_ghidra() -> None:
    """Verify Ghidra CLI is reachable and the required programs are loaded."""
    ghidra_cmd = shutil.which("ghidra-cli") or shutil.which("ghidra")
    if ghidra_cmd is None:
        print("FAIL  ghidra-cli / ghidra not found in PATH")
        print("      Install ghidra-cli and ensure it is on your PATH.")
        sys.exit(1)
    print(f"OK    ghidra binary: {ghidra_cmd}")

    # Try a minimal command that lists available programs
    try:
        result = subprocess.run(
            [ghidra_cmd, "program", "list"],
            capture_output=True,
            timeout=15,
        )
        output = result.stdout.decode(errors="replace")
        stderr = result.stderr.decode(errors="replace").strip()

        for prog in [GC_GHIDRA_PROGRAM, PS2_GHIDRA_PROGRAM]:
            if prog in output:
                print(f"OK    program found: {prog}")
            else:
                print(f"WARN  program not found in listing: {prog}")
                print(f"      Run: ghidra set-default project NeedForSpeed")

        if result.returncode != 0 and stderr:
            print(f"WARN  ghidra program list exited {result.returncode}: {stderr}")
    except subprocess.TimeoutExpired:
        print("WARN  ghidra program list timed out — Ghidra may be slow to start")
    except Exception as e:
        print(f"WARN  could not verify programs: {e}")

    # Quick decompile smoke test — use a known small GC function address if symbols exist
    if os.path.exists(GC_SYMBOLS_FILE):
        with open(GC_SYMBOLS_FILE) as f:
            for line in f:
                m = re.match(r"^\S+\s*=\s*(?:\.text:)?0x([0-9A-Fa-f]+)", line.strip())
                if m:
                    test_addr = m.group(1)
                    break
            else:
                test_addr = None
        if test_addr:
            code, err = ghidra_decompile(test_addr, GC_GHIDRA_PROGRAM)
            if code is not None:
                print(f"OK    decompile smoke test passed (0x{test_addr})")
            else:
                print(f"FAIL  decompile smoke test failed for 0x{test_addr}: {err}")
    else:
        print(f"SKIP  smoke test — symbols file not found: {GC_SYMBOLS_FILE}")


def extract_source_for_function(
    source_path: str, right_sym: Optional[Dict[str, Any]]
) -> Optional[str]:
    """Extract the source lines relevant to the function from its source file.

    Uses the line numbers embedded in the right (decomp) symbol's instructions
    to determine the source region. Falls back to full file if line numbers are
    unavailable or the file cannot be read.

    Returns the extracted source text, or None if the file doesn't exist.
    """
    full_path = os.path.join(root_dir, source_path)
    if not os.path.exists(full_path):
        return None

    with open(full_path) as f:
        all_lines = f.readlines()

    if not all_lines:
        return ""

    # Collect all source line numbers referenced by the decomp symbol's instructions
    line_numbers: List[int] = []
    if right_sym:
        for inst_entry in right_sym.get("instructions", []):
            ln = inst_entry.get("instruction", {}).get("line_number")
            if ln is not None:
                line_numbers.append(int(ln))

    if not line_numbers:
        # No line info available — return full source
        return "".join(all_lines)

    first_line = min(line_numbers)
    last_line = max(line_numbers)

    # Expand to capture the enclosing function: walk back to find the function
    # signature (a line that looks like a function definition, not just a brace).
    # Heuristic: walk backward from first_line to find an opening that precedes
    # the function body, giving context.
    start = max(1, first_line - SOURCE_CONTEXT_LINES)
    end = min(len(all_lines), last_line + SOURCE_CONTEXT_LINES)
    if start > end:
        return ""

    # 1-indexed lines -> 0-indexed slice
    excerpt = all_lines[start - 1 : end]
    if not excerpt:
        return ""
    header = f"// Lines {start}–{end} of {source_path}\n"
    return header + "".join(excerpt)


def source_excerpt_is_useful(source_path: str, excerpt: str) -> bool:
    lines = [line.strip() for line in excerpt.splitlines()]
    content_lines = [
        line
        for line in lines
        if line and not line.startswith("// Lines ")
    ]
    if not content_lines:
        return False

    include_like = sum(
        1
        for line in content_lines
        if line.startswith("#include")
        or line.startswith("#pragma")
        or line.startswith("#if")
        or line.startswith("#endif")
        or line.startswith("#define")
    )

    source_list_path = source_path.replace("\\", "/")
    if "SourceLists/" in source_list_path:
        if include_like == len(content_lines):
            return False
        if include_like >= max(2, len(content_lines) - 1):
            return False

    useful_tokens = ("{", "}", "if ", "for ", "while ", "::", "return ", "=")
    if include_like == len(content_lines) and not any(
        token in excerpt for token in useful_tokens
    ):
        return False

    return True


def extract_source_around_line(
    source_path: str, line_number: int, context_lines: int = SOURCE_CONTEXT_LINES
) -> Optional[str]:
    full_path = os.path.join(root_dir, source_path)
    if not os.path.exists(full_path):
        return None

    with open(full_path) as f:
        all_lines = f.readlines()

    if not all_lines:
        return ""

    target_line = min(max(1, line_number), len(all_lines))
    start = max(1, target_line - context_lines)
    end = min(len(all_lines), target_line + context_lines)
    excerpt = all_lines[start - 1 : end]
    header = (
        f"// Lines {start}–{end} of {source_path} "
        f"(GC debug-line fallback, target line {line_number})\n"
    )
    return header + "".join(excerpt)


def definition_name_variants(function_name: str) -> Tuple[List[str], Optional[str]]:
    qualified = function_name.split("(", 1)[0].strip()
    scoped_parts = [part.strip() for part in qualified.split("::") if part.strip()]

    qualified_variants: List[str] = []
    if qualified:
        qualified_variants.append(qualified)
    if len(scoped_parts) >= 2:
        tail_qualified = "::".join(scoped_parts[-2:])
        if tail_qualified not in qualified_variants:
            qualified_variants.append(tail_qualified)

    bare_name = scoped_parts[-1] if scoped_parts else (qualified or None)
    return qualified_variants, bare_name


def line_has_nearby_open_brace(lines: List[str], line_number: int) -> bool:
    start = max(1, line_number)
    end = min(len(lines), line_number + 2)
    for idx in range(start, end + 1):
        if "{" in lines[idx - 1]:
            return True
    return False


def find_function_definition_line(
    lines: List[str], function_name: str, target_line: Optional[int] = None
) -> Optional[int]:
    qualified_variants, bare_name = definition_name_variants(function_name)
    candidates: List[Tuple[int, int]] = []

    for index, raw_line in enumerate(lines, start=1):
        line = raw_line.strip()
        if not line:
            continue

        score = 0
        for i, qualified in enumerate(qualified_variants):
            if re.search(rf"\b{re.escape(qualified)}\s*\(", line):
                score = max(score, 260 - (i * 40))

        if bare_name and re.search(rf"\b{re.escape(bare_name)}\s*\(", line):
            score = max(score, 120)

        if score == 0:
            continue

        if line.endswith(";"):
            score -= 120
        if "::" in line:
            score += 20
        if line_has_nearby_open_brace(lines, index):
            score += 35

        if target_line is not None:
            score -= min(abs(index - target_line), 120)

        candidates.append((score, index))

    if not candidates:
        return None

    if target_line is None:
        candidates.sort(key=lambda item: (-item[0], item[1]))
    else:
        candidates.sort(key=lambda item: (-item[0], abs(item[1] - target_line), item[1]))
    return candidates[0][1]


def extract_source_for_definition(
    source_path: str, definition_line: int, target_line: Optional[int] = None
) -> Optional[str]:
    full_path = os.path.join(root_dir, source_path)
    if not os.path.exists(full_path):
        return None

    with open(full_path) as f:
        all_lines = f.readlines()

    if not all_lines:
        return ""

    start = definition_line
    while start > 1:
        previous = all_lines[start - 2].strip()
        if not previous or previous.endswith(";") or previous == "}":
            break
        if previous.startswith("//"):
            break
        start -= 1
        if definition_line - start >= 3:
            break

    brace_depth = 0
    saw_open_brace = False
    end: Optional[int] = None
    for index in range(start, len(all_lines) + 1):
        line = all_lines[index - 1]
        if "{" in line:
            saw_open_brace = True
        if saw_open_brace:
            brace_depth += line.count("{")
            brace_depth -= line.count("}")
            if brace_depth <= 0:
                end = index
                break

    if end is None:
        return extract_source_around_line(
            source_path,
            target_line if target_line is not None else definition_line,
        )

    excerpt = all_lines[start - 1 : end]
    detail = "GC debug-line fallback, matched definition"
    if target_line is not None:
        detail += f", target line {target_line}"
    header = f"// Lines {start}–{end} of {source_path} ({detail})\n"
    return header + "".join(excerpt)


def extract_source_from_debug_hint(
    source_path: str, function_name: str, target_line: int
) -> Optional[str]:
    full_path = os.path.join(root_dir, source_path)
    if not os.path.exists(full_path):
        return None

    with open(full_path) as f:
        all_lines = f.readlines()

    if not all_lines:
        return ""

    definition_line = find_function_definition_line(
        all_lines, function_name, target_line=target_line
    )
    if definition_line is not None:
        return extract_source_for_definition(
            source_path, definition_line, target_line=target_line
        )

    return extract_source_around_line(source_path, target_line)


def resolve_repo_path_from_debug_path(debug_path: str) -> Optional[str]:
    normalized = debug_path.replace("\\", "/").strip()
    lowered = normalized.lower()

    suffixes: List[str] = []
    if "/speed/indep/" in lowered:
        suffixes.append(lowered.split("/speed/indep/", 1)[1].lstrip("/"))
    if "/src/" in lowered:
        suffixes.append("src/" + lowered.split("/src/", 1)[1].lstrip("/"))
    basename = os.path.basename(lowered)
    if basename:
        suffixes.append(basename)

    matches: List[str] = []
    source_root = os.path.join(root_dir, "src")
    for dirpath, _, filenames in os.walk(source_root):
        for filename in filenames:
            rel_path = os.path.relpath(os.path.join(dirpath, filename), root_dir)
            rel_lower = rel_path.replace("\\", "/").lower()
            if any(rel_lower.endswith(suffix) for suffix in suffixes):
                matches.append(rel_path)

    if not matches:
        return None
    matches.sort(key=lambda path: (len(path), path))
    return matches[0]


def lookup_debug_line_source(address: str) -> Tuple[Optional[Dict[str, Any]], Optional[str]]:
    if not os.path.exists(DEBUG_LINES_FILE):
        return None, f"debug lines file not found: {DEBUG_LINES_FILE}"

    try:
        target = int(address, 16)
    except ValueError:
        return None, f"invalid address for debug line lookup: {address}"

    pattern = re.compile(r"^\s*0x([0-9A-Fa-f]+):\s*(.+?)\s+\(line\s+(\d+)\)\s*$")
    best: Optional[Dict[str, Any]] = None

    with open(DEBUG_LINES_FILE) as f:
        for raw_line in f:
            line = raw_line.rstrip("\n")
            match = pattern.match(line)
            if match is None:
                continue

            entry_addr = int(match.group(1), 16)
            diff = abs(entry_addr - target)
            candidate = {
                "address": f"0x{entry_addr:08X}",
                "debug_path": match.group(2),
                "line_number": int(match.group(3)),
                "exact": diff == 0,
                "diff": diff,
            }
            if best is None or diff < int(best["diff"]):
                best = candidate
                if diff == 0:
                    break

    if best is None:
        return None, f"no entries found in {DEBUG_LINES_FILE}"

    best["repo_path"] = resolve_repo_path_from_debug_path(str(best["debug_path"]))
    return best, None


def split_function_search_terms(function_name: str) -> Tuple[List[str], List[str]]:
    qualified = function_name.split("(", 1)[0].strip()
    scoped_parts = [part.strip() for part in qualified.split("::") if part.strip()]

    phrases: List[str] = []
    tokens: List[str] = []

    if qualified:
        phrases.append(qualified)

    for part in scoped_parts:
        if part and part not in tokens:
            tokens.append(part)

    bare = re.sub(r"[^A-Za-z0-9_:]", " ", function_name)
    for token in bare.replace("::", " ").split():
        if len(token) >= 3 and token not in tokens:
            tokens.append(token)

    return phrases, tokens


def find_related_source_files(
    function_name: str, exclude_path: Optional[str] = None
) -> List[Tuple[int, str, List[str]]]:
    source_root = os.path.join(root_dir, "src")
    if not os.path.isdir(source_root):
        return []

    phrases, tokens = split_function_search_terms(function_name)
    if not phrases and not tokens:
        return []

    class_token = tokens[0] if tokens else None
    method_token = tokens[-1] if tokens else None

    candidates: List[Tuple[int, str, List[str]]] = []
    for dirpath, _, filenames in os.walk(source_root):
        for filename in filenames:
            if not filename.endswith((".cpp", ".hpp", ".h")):
                continue

            abs_path = os.path.join(dirpath, filename)
            rel_path = os.path.relpath(abs_path, root_dir)
            if exclude_path and rel_path == exclude_path:
                continue
            path_lower = rel_path.lower()
            score = 0
            reasons: List[str] = []
            is_cpp = rel_path.endswith(".cpp")
            is_header = rel_path.endswith((".hpp", ".h"))

            try:
                file_size = os.path.getsize(abs_path)
            except OSError:
                continue

            if is_cpp:
                score += 35
                reasons.append("implementation file")
            elif is_header:
                score += 5

            if file_size == 0:
                score -= 140
                reasons.append("empty file")

            if "/generated/" in path_lower:
                score -= 40

            if class_token and class_token.lower() in path_lower:
                score += 90
                reasons.append(f"path mentions {class_token}")
            if method_token and method_token.lower() in path_lower and method_token != class_token:
                score += 30
                reasons.append(f"path mentions {method_token}")

            basename = os.path.splitext(os.path.basename(rel_path))[0]
            if class_token and basename.lower() == class_token.lower():
                score += 90 if is_cpp else 45
                reasons.append(f"basename matches {class_token}")

            try:
                with open(abs_path, errors="ignore") as f:
                    text = f.read()
            except OSError:
                continue

            if class_token and method_token:
                qualified = f"{class_token}::{method_token}"
                if qualified in text:
                    score += 320
                    reasons.append(f"contains {qualified}")

            for phrase in phrases:
                if phrase and phrase in text:
                    score += 220
                    reasons.append(f"contains {phrase}")
                    break

            if class_token:
                if re.search(rf"\b(class|struct)\s+{re.escape(class_token)}\b", text):
                    score += 140
                    reasons.append(f"declares {class_token}")
                if f"{class_token}::" in text:
                    score += 80
                    reasons.append(f"mentions {class_token}::")

            if method_token and re.search(rf"\b{re.escape(method_token)}\s*\(", text):
                score += 35
                reasons.append(f"mentions {method_token}(")

            if score > 0:
                deduped_reasons = []
                for reason in reasons:
                    if reason not in deduped_reasons:
                        deduped_reasons.append(reason)
                candidates.append((score, rel_path, deduped_reasons[:3]))

    candidates.sort(key=lambda item: (-item[0], item[1]))
    return candidates[:RELATED_SOURCE_LIMIT]


def format_related_source_files(
    function_name: str,
    source_path: Optional[str],
    gc_addr: Optional[str] = None,
    debug_hint: Optional[Dict[str, Any]] = None,
    debug_err: Optional[str] = None,
    brief: bool = False,
) -> str:
    lines = []
    if source_path:
        full_source_path = os.path.join(root_dir, source_path)
        if not os.path.exists(full_source_path):
            lines.append(f"Primary metadata source is missing: {source_path}")
        elif os.path.getsize(full_source_path) == 0:
            lines.append(f"Primary metadata source is empty: {source_path}")
        else:
            lines.append(f"Primary metadata source was not useful: {source_path}")
        lines.append("")

    if debug_hint is None and debug_err is None and gc_addr:
        debug_hint, debug_err = lookup_debug_line_source(gc_addr)

    if gc_addr:
        if debug_hint is not None:
            display_path = str(debug_hint.get("repo_path") or debug_hint.get("debug_path"))
            suffix = ""
            repo_path = debug_hint.get("repo_path")
            if repo_path:
                full_repo_path = os.path.join(root_dir, str(repo_path))
                if os.path.exists(full_repo_path) and os.path.getsize(full_repo_path) == 0:
                    suffix = ", file is currently empty in repo"
            if debug_hint.get("exact"):
                relation = f"exact address match for {format_hex_address(gc_addr)}"
            else:
                relation = (
                    f"closest debug line to {format_hex_address(gc_addr)} "
                    f"(off by {debug_hint['diff']} / 0x{int(debug_hint['diff']):X})"
                )
            lines.append("GC debug line mapping suggests:")
            lines.append(
                f"- {display_path}:{debug_hint['line_number']} ({relation}{suffix})"
            )
            lines.append("")
        elif debug_err:
            lines.append(f"GC debug line mapping unavailable: {debug_err}")
            lines.append("")

    hints = find_related_source_files(function_name, exclude_path=source_path)
    if not hints:
        lines.append(f"No related source files found for {function_name}.")
        return "\n".join(lines)

    lines.append(f"Likely related files for {function_name}:")
    shown_hints = hints[:BRIEF_RELATED_SOURCE_LIMIT] if brief else hints
    for score, rel_path, reasons in shown_hints:
        lines.append(f"- {rel_path} ({', '.join(reasons)})")
    if brief and len(hints) > len(shown_hints):
        omitted = len(hints) - len(shown_hints)
        lines.append(
            f"- ... {omitted} more omitted in brief mode (rerun without --brief)"
        )
    return "\n".join(lines)


def strip_ansi(text: str) -> str:
    """Remove ANSI escape codes from text."""
    return re.sub(r"\x1b\[[0-9;]*m", "", text)


def tu_name_from_unit(unit: Dict[str, Any]) -> str:
    """Derive the TU .o name from a unit's metadata."""
    source = unit.get("metadata", {}).get("source_path", "")
    if source:
        # src/MetroidPrime/CEntity.cpp -> CEntity.o
        base = os.path.splitext(os.path.basename(source))[0]
        return f"{base}.o"
    # Fallback: derive from unit name
    name = unit["name"]
    base = name.rsplit("/", 1)[-1]
    return f"{base}.o"


def print_section(title: str, content: str) -> None:
    """Print a labeled section."""
    print(f"\n{'=' * 60}")
    print(f"  {title}")
    print(f"{'=' * 60}")
    print(content)


def print_ghidra_decompilation(
    version_name: str,
    symbols_file: str,
    program: str,
    function: str,
    mangled_function: str,
):
    # Try to find address from symbols.txt
    addr = lookup_symbol_address(symbols_file, mangled_function)

    if not addr:
        # Try searching symbols.txt
        matches = search_symbols_file(symbols_file, function)
        if matches:
            # Pick the first function match
            addr = matches[0][2]
            mangled_function = matches[0][0]

    if addr:
        code, err = ghidra_decompile(addr, program)
        if code is not None:
            print_section(f"{version_name}: Ghidra Decompile (0x{addr})", code)
        else:
            print(f"\nGhidra decompile failed for {version_name} 0x{addr}: {err}")
    else:
        print(
            f"\nCould not find address for {mangled_function} in {version_name} symbols.txt"
        )


def print_gc_dwarf_lookup(
    function: str, mangled_function: str, lookup_mode: str = "full"
) -> None:
    addr = lookup_symbol_address(GC_SYMBOLS_FILE, mangled_function)

    lookup_queries = []
    if addr:
        lookup_queries.append(f"0x{addr}")
    lookup_queries.append(function)

    seen_queries = set()
    for query in lookup_queries:
        if query in seen_queries:
            continue
        seen_queries.add(query)
        dwarf_text, err = lookup_function_dwarf(query)
        if dwarf_text is not None:
            title = "GOWE69: DWARF Function"
            if query.startswith("0x"):
                title += f" ({query})"
            if lookup_mode == "signature":
                title += " (signature)"
                dwarf_text = compact_dwarf_function_text(dwarf_text)
            print_section(title, dwarf_text)
            return

    if addr:
        print(
            f"\nDWARF lookup failed for {function} / 0x{addr}: {err}",
            file=sys.stderr,
        )
    else:
        print(f"\nDWARF lookup failed for {function}: {err}", file=sys.stderr)


def format_suggested_commands(
    unit_name: str,
    function_query: str,
    symbol_name: str,
    left_sym: Optional[Dict[str, Any]],
    right_sym: Optional[Dict[str, Any]],
    brief: bool = False,
) -> str:
    commands = []

    if left_sym is not None and right_sym is None:
        commands.extend(
            [
                f"python tools/decomp-workflow.py function -u {unit_name} -f '{function_query}' --lookup-mode full",
                f"python tools/decomp-workflow.py function -u {unit_name} -f '{function_query}' --ghidra-version gc",
                f"python tools/decomp-workflow.py unit -u {unit_name} --search '{function_query}' --limit 20",
            ]
        )
    elif left_sym is not None and right_sym is not None:
        commands.extend(
            [
                f"python tools/decomp-workflow.py diff -u {unit_name} -d '{function_query}' -C 8",
                f"python tools/decomp-workflow.py function -u {unit_name} -f '{function_query}' --lookup-mode full",
                f"python tools/decomp-workflow.py function -u {unit_name} -f '{function_query}' --ghidra-version gc",
            ]
        )
    elif left_sym is None and right_sym is not None:
        commands.extend(
            [
                f"python tools/decomp-workflow.py function -u {unit_name} -f '{function_query}' --lookup-mode full",
                f"python tools/find-symbol.py '{symbol_name}'",
                f"python tools/decomp-workflow.py unit -u {unit_name} --search '{function_query}' --limit 20",
            ]
        )
    else:
        commands.extend(
            [
                f"python tools/decomp-workflow.py unit -u {unit_name} --search '{function_query}' --limit 20",
                f"python tools/decomp-workflow.py diff -u {unit_name} --search '{function_query}' --limit 20",
            ]
        )

    shown_commands = commands[:BRIEF_SUGGESTED_COMMAND_LIMIT] if brief else commands
    lines = [f"- {command}" for command in shown_commands]
    if brief and len(commands) > len(shown_commands):
        lines.append(
            f"- ... {len(commands) - len(shown_commands)} more omitted in brief mode"
        )
    return "\n".join(lines)


def unit_progress_category(unit: Dict[str, Any]) -> Optional[str]:
    categories = unit.get("metadata", {}).get("progress_categories", [])
    if not categories:
        return None
    if len(categories) > 1:
        return str(categories[1])
    return str(categories[0])


def format_priority_guidance(
    unit_name: str,
    unit: Dict[str, Any],
    diff_data: Optional[Dict[str, Any]],
    current_symbol_name: Optional[str],
    brief: bool = False,
) -> Optional[str]:
    if diff_data is None or current_symbol_name is None:
        return None

    function_rows = [
        row
        for row in build_objdiff_symbol_rows(diff_data)
        if row["side"] == "left"
        and row["type"] == "function"
        and row["status"] in ("missing", "nonmatching")
        and row["unmatched_bytes_est"] > 0
    ]
    if not function_rows:
        return None

    function_rows.sort(
        key=lambda row: (-row["unmatched_bytes_est"], -row["size"], row["name"].lower())
    )

    current_row = None
    for row in function_rows:
        if row["symbol_name"] == current_symbol_name:
            current_row = row
            break
    if current_row is None:
        return None

    current_unmatched = int(current_row["unmatched_bytes_est"])
    current_match = current_row.get("match_percent")
    if (
        current_unmatched > LOW_UNMATCHED_HINT_THRESHOLD
        and (current_match is None or float(current_match) < HIGH_MATCH_HINT_THRESHOLD)
    ):
        return None

    unit_top = function_rows[0]
    larger_unit_target = None
    for row in function_rows:
        if row["symbol_name"] == current_symbol_name:
            continue
        if (
            int(row["unmatched_bytes_est"]) >= LARGER_TARGET_MIN_BYTES
            and int(row["unmatched_bytes_est"]) >= current_unmatched * LARGER_TARGET_RATIO
        ):
            larger_unit_target = row
            break

    lines: List[str] = []
    if current_match is not None and float(current_match) >= HIGH_MATCH_HINT_THRESHOLD:
        lines.append(
            f"- Current function is already in cleanup/polish territory "
            f"(~{current_unmatched}B remaining, {float(current_match):.1f}% matched)."
        )
    else:
        lines.append(
            f"- Current function is already low-byte cleanup territory (~{current_unmatched}B remaining)."
        )

    if larger_unit_target is not None:
        larger_match = larger_unit_target.get("match_percent")
        larger_match_detail = ""
        if larger_match is not None:
            larger_match_detail = f", {float(larger_match):.1f}% matched"
        lines.append(
            f"- This unit still has a much larger target: "
            f"{larger_unit_target['name']} "
            f"(~{larger_unit_target['unmatched_bytes_est']}B remaining{larger_match_detail})."
        )
        lines.append(
            f"- Try: python tools/decomp-workflow.py function -u {unit_name} "
            f"-f '{larger_unit_target['name']}'"
        )
    else:
        lines.append(
            f"- This unit's largest remaining function is only ~{unit_top['unmatched_bytes_est']}B "
            f"({unit_top['name']})."
        )
        category = unit_progress_category(unit)
        next_cmd = "python tools/decomp-workflow.py next --strategy balanced --limit 10"
        if category:
            next_cmd = (
                "python tools/decomp-workflow.py next "
                f"--category {category} --strategy balanced --limit 10"
            )
        lines.append(f"- For larger gains elsewhere, rerun: {next_cmd}")

    if brief:
        if larger_unit_target is not None:
            return "\n".join([lines[0], lines[2]])
        return "\n".join([lines[0], lines[2]])
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Gather context for decomp function matching"
    )
    parser.add_argument(
        "-u", "--unit", help="Unit name (e.g. main/MetroidPrime/CEntity)"
    )
    parser.add_argument("-f", "--function", help="Function name to look up")
    parser.add_argument(
        "--no-source", action="store_true", help="Skip source file output"
    )
    parser.add_argument(
        "--no-ghidra", action="store_true", help="Skip Ghidra decompile"
    )
    parser.add_argument(
        "--ghidra-version",
        choices=["both", "gc", "ps2"],
        default="both",
        help="Choose which Ghidra decompile(s) to show (default: both)",
    )
    parser.add_argument(
        "--no-lookup", action="store_true", help="Skip GC DWARF function lookup"
    )
    parser.add_argument(
        "--lookup-mode",
        choices=["full", "signature"],
        default="full",
        help="Choose how much GC DWARF function detail to show (default: full)",
    )
    parser.add_argument(
        "--brief",
        action="store_true",
        help="Trim helper sections like related-source hints and suggested commands",
    )
    parser.add_argument(
        "--ghidra-check",
        action="store_true",
        help="Verify Ghidra CLI is reachable and programs are loaded, then exit",
    )
    parser.add_argument(
        "--base-obj",
        metavar="PATH",
        help=(
            "Use this .o file as the decomp base instead of the one from objdiff.json."
        ),
    )
    parser.add_argument(
        "--reloc-diffs",
        choices=RELOC_DIFF_CHOICES,
        default="none",
        help=(
            "Control relocation-only mismatches in objdiff "
            "(default: none; use all to surface relocation diffs)"
        ),
    )
    args = parser.parse_args()

    if args.ghidra_check:
        check_ghidra()
        return

    if not args.unit or not args.function:
        parser.error("-u/--unit and -f/--function are required (or use --ghidra-check)")

    config = load_project_config()
    unit = find_unit(config, args.unit)
    if not unit:
        print(f"Unit not found: {args.unit}", file=sys.stderr)
        sys.exit(1)

    meta = unit.get("metadata", {})
    source_path = meta.get("source_path", "")

    # === objdiff Status (run first so we have line numbers for source scoping) ===
    diff_data = run_objdiff(
        args.unit, base_obj=args.base_obj, reloc_diffs=args.reloc_diffs
    )
    left_sym = right_sym = None

    if diff_data:
        left_sym, right_sym = find_symbol_in_diff(diff_data, args.function)

    context_name = args.function
    gc_addr = None
    if left_sym or right_sym:
        sym = left_sym if left_sym is not None else right_sym
        assert sym is not None
        context_name = sym.get("demangled_name", sym.get("name", args.function))
        gc_addr = lookup_symbol_address(GC_SYMBOLS_FILE, sym.get("name", ""))
    debug_hint = None
    debug_hint_err = None
    if gc_addr:
        debug_hint, debug_hint_err = lookup_debug_line_source(gc_addr)

    # === Source File (scoped to function if line info available) ===
    source_was_useful = False
    if not args.no_source:
        if source_path:
            excerpt = extract_source_for_function(source_path, right_sym)
            if (
                excerpt is not None
                and excerpt.strip()
                and source_excerpt_is_useful(source_path, excerpt)
            ):
                label = "Source"
                if right_sym and right_sym.get("instructions"):
                    # Check if we actually got line info
                    has_lines = any(
                        inst.get("instruction", {}).get("line_number") is not None
                        for inst in right_sym.get("instructions", [])
                    )
                    if has_lines:
                        label = "Source (function excerpt)"
                    else:
                        label = f"Source (full file — no line info): {source_path}"
                print_section(label, excerpt)
                source_was_useful = True

        if (
            not source_was_useful
            and debug_hint is not None
            and debug_hint.get("repo_path") is not None
        ):
            fallback_source = str(debug_hint["repo_path"])
            fallback_line = int(debug_hint.get("line_number", 0))
            fallback_excerpt = extract_source_from_debug_hint(
                fallback_source,
                context_name,
                fallback_line,
            )
            if fallback_excerpt is not None and fallback_excerpt.strip():
                print_section("Source (GC debug-line fallback)", fallback_excerpt)
                source_was_useful = True

        if not source_was_useful:
            print_section(
                "Related Source Files",
                format_related_source_files(
                    context_name,
                    source_path,
                    gc_addr=gc_addr,
                    debug_hint=debug_hint,
                    debug_err=debug_hint_err,
                    brief=args.brief,
                ),
            )

    # === objdiff Status ===
    if diff_data:
        if left_sym or right_sym:
            sym = left_sym if left_sym is not None else right_sym
            assert sym is not None
            name = sym.get("demangled_name", sym.get("name", "?"))
            mangled = sym.get("name", "?")
            mp = sym.get("match_percent")
            size = int(sym.get("size", "0"))

            status_lines = []
            status_lines.append(f"Function: {name}")
            status_lines.append(f"Mangled:  {mangled}")
            status_lines.append(f"Size:     {size} bytes")
            status_lines.append(
                f"Status:   {describe_symbol_presence(left_sym, right_sym)}"
            )
            if left_sym and right_sym and mp is not None:
                status_lines.append(f"Match:    {mp:.1f}%")

            # Quick diff summary
            if left_sym and right_sym:
                left_insts = left_sym.get("instructions", [])
                right_insts = right_sym.get("instructions", [])
                n_mismatch = sum(
                    1 for inst in left_insts if inst.get("diff_kind", "") != ""
                )
                status_lines.append(
                    f"Instructions: {max(len(left_insts), len(right_insts))} "
                    f"({n_mismatch} mismatched)"
                )

            print_section("objdiff Status", "\n".join(status_lines))

            print_section(
                "Suggested Next Commands",
                format_suggested_commands(
                    args.unit, args.function, name, left_sym, right_sym, brief=args.brief
                ),
            )

            priority_guidance = format_priority_guidance(
                args.unit,
                unit,
                diff_data,
                mangled,
                brief=args.brief,
            )
            if priority_guidance:
                print_section("Higher-impact targets right now", priority_guidance)

            if not source_was_useful and args.no_source:
                print_section(
                    "Related Source Files",
                    format_related_source_files(
                        name,
                        source_path,
                        gc_addr=gc_addr,
                        debug_hint=debug_hint,
                        debug_err=debug_hint_err,
                        brief=args.brief,
                    ),
                )

            # Run decomp-diff.py for the actual diff unless the function is already fully matched.
            show_diff = not (left_sym and right_sym and mp is not None and mp >= 100.0)
            if show_diff:
                diff_cmd = [
                    sys.executable,
                    os.path.join(script_dir, "decomp-diff.py"),
                    "-u",
                    args.unit,
                    "-d",
                    args.function,
                    "--reloc-diffs",
                    args.reloc_diffs,
                ]
                if args.base_obj:
                    diff_cmd += ["--base-obj", args.base_obj]
                result = subprocess.run(
                    diff_cmd,
                    capture_output=True,
                    cwd=root_dir,
                )
                if result.returncode == 0:
                    print_section("Instruction Diff", result.stdout.decode())
        else:
            print(
                f"\nFunction '{args.function}' not found in objdiff data for {args.unit}"
            )
            # List available symbols
            left_syms = diff_data.get("left", {}).get("symbols", [])
            right_syms = diff_data.get("right", {}).get("symbols", [])
            print("Available functions:")
            seen = set()
            for sym in left_syms + right_syms:
                if "instructions" in sym:
                    name = sym.get("demangled_name", sym.get("name", "?"))
                    if name not in seen:
                        seen.add(name)
                        print(f"  {name}")
    else:
        print(f"\nFailed to run objdiff for {args.unit}", file=sys.stderr)

    # === Ghidra Decompile ===
    if not args.no_lookup and (left_sym or right_sym):
        sym = left_sym if left_sym is not None else right_sym
        assert sym is not None
        name = sym.get("demangled_name", args.function)
        mangled = sym.get("name", "")

        print_gc_dwarf_lookup(name, mangled, lookup_mode=args.lookup_mode)

    if not args.no_ghidra and (left_sym or right_sym):
        sym = left_sym if left_sym is not None else right_sym
        assert sym is not None
        mangled = sym.get("name", "")

        if args.ghidra_version in ("both", "gc"):
            print_ghidra_decompilation(
                "GOWE69", GC_SYMBOLS_FILE, GC_GHIDRA_PROGRAM, args.function, mangled
            )
        if args.ghidra_version in ("both", "ps2"):
            print_ghidra_decompilation(
                "SLES-53558-A124",
                PS2_SYMBOLS_FILE,
                PS2_GHIDRA_PROGRAM,
                args.function,
                mangled,
            )


if __name__ == "__main__":
    try:
        main()
    except ToolError as e:
        fail(str(e))
