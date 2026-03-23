#!/usr/bin/env python3
"""
Decomp-aware code style helper.

Examples:
  python tools/code_style.py audit --base origin/main
  python tools/code_style.py classify src/Speed/Indep/Src/Frontend/FEManager.cpp
  python tools/code_style.py format --check src/Speed/Indep/Src/Frontend/FEManager.cpp
"""

import argparse
import os
import platform
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Sequence, Set

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))
src_dir = os.path.join(root_dir, "src")
ps2_types_path = os.path.join(root_dir, "symbols", "PS2", "PS2_types.nothpp")

CPP_EXTS = {".c", ".cc", ".cpp", ".h", ".hh", ".hpp"}
HEADER_EXTS = {".h", ".hh", ".hpp"}

# Small focused C/C++ subset for targeted probes. The format command itself
# now covers all eligible changed C/C++ files by default; this bucket remains
# useful when a caller explicitly wants a narrower Frontend/FEng-only pass.
SAFE_CPP_PREFIXES = (
    "src/Speed/Indep/Src/Frontend/",
    "src/Speed/Indep/Src/FEng/",
)
DOC_PREFIXES = (
    ".github/skills/",
    "docs/",
)
TOOL_PREFIXES = (
    "tools/",
)
JUMBO_PREFIX = "src/Speed/Indep/SourceLists/"
MATCH_SENSITIVE_PREFIXES = (
    "src/Speed/Indep/Libs/Support/Utility/",
    "src/Speed/Indep/bWare/Inc/",
    "src/Speed/Indep/Src/",
)
ROOT_FILES = {
    "AGENTS.md",
    "README.md",
}
CATEGORIES = (
    "docs",
    "jumbo-source-list",
    "match-sensitive-cpp",
    "match-sensitive-other",
    "other",
    "safe-cpp",
    "safe-other",
    "tooling",
)


@dataclass
class Finding:
    path: str
    line: int
    severity: str
    message: str


DECL_PATTERN = re.compile(
    r"^\s*(struct|class)\s+([A-Za-z_][A-Za-z0-9_]*)\b(?:\s*[:;{]|$)"
)
TYPE_BODY_START_PATTERN = re.compile(r"^\s*(struct|class)\s+([A-Za-z_][A-Za-z0-9_]*)\b.*\{")
FORWARD_DECL_PATTERN = re.compile(r"^\s*(struct|class)\s+([A-Za-z_][A-Za-z0-9_]*)\s*;\s*$")
VISIBILITY_PATTERN = re.compile(r"^\s*(public|private|protected)\s*:", re.MULTILINE)
ACCESS_SPECIFIER_PATTERN = re.compile(r"^\s*(public|private|protected)\s*:\s*$")
CAST_SPACING_PATTERN = re.compile(
    r"\b(?:static_cast|reinterpret_cast|const_cast|dynamic_cast)\s*<\s+"
    r"|"
    r"\b(?:static_cast|reinterpret_cast|const_cast|dynamic_cast)\s*<[^>\n]*\s+>"
)
USING_NAMESPACE_PATTERN = re.compile(r"^\s*using\s+namespace\b")
NULL_PATTERN = re.compile(r"\bNULL\b")
BARE_PRESENCE_IF_PATTERN = re.compile(r"^\s*#if\s+([A-Za-z_][A-Za-z0-9_]*)\s*$")
HEADER_GUARD_IFNDEF_PATTERN = re.compile(r"^\s*#ifndef\s+[A-Za-z0-9_]+\s*$", re.MULTILINE)
HEADER_GUARD_DEFINE_PATTERN = re.compile(r"^\s*#define\s+[A-Za-z0-9_]+\s*$", re.MULTILINE)
EA_PRAGMA_BLOCK_PATTERN = re.compile(
    r"^\s*#ifdef\s+EA_PRAGMA_ONCE_SUPPORTED\s*$"
    r".*?^\s*#pragma\s+once\s*$"
    r".*?^\s*#endif\s*$",
    re.MULTILINE | re.DOTALL,
)
EA_PRAGMA_IFDEF_PATTERN = re.compile(
    r"^\s*#ifdef\s+EA_PRAGMA_ONCE_SUPPORTED\s*$", re.MULTILINE
)
RECOVERED_LAYOUT_COMMENT_PATTERN = re.compile(
    r"//\s*offset 0x[0-9A-Fa-f]+,\s*size 0x[0-9A-Fa-f]+"
)
RECOVERED_NARROW_UNSIGNED_PATTERN = re.compile(r"\bunsigned\s+(char|short)\b")
BARE_RECOVERY_MARKER_PATTERN = re.compile(
    r"//\s*(TODO|UNSOLVED|STRIPPED)\b(?:\s*[.:,-]*)?\s*$"
)
SUSPICIOUS_MEMBER_PATTERN = re.compile(
    r"^(?:"
    r"_?pad(?:ding)?[0-9A-Fa-f_]*"
    r"|pad(?:byte|char)"
    r"|unk(?:nown)?[0-9A-Fa-f_]*"
    r"|unk_[A-Za-z0-9_]+"
    r"|field_[0-9A-Fa-f]+"
    r")$"
)

_source_decl_cache: Optional[Dict[str, List[tuple]]] = None
_ps2_kind_cache: Dict[str, Optional[str]] = {}


def run_git(args: Sequence[str]) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=root_dir,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "git command failed")
    return result.stdout


def relpath(path: str) -> str:
    abs_path = path if os.path.isabs(path) else os.path.join(root_dir, path)
    return os.path.relpath(abs_path, root_dir).replace("\\", "/")


def path_category(path: str) -> str:
    path = relpath(path)
    ext = os.path.splitext(path)[1]

    if path in ROOT_FILES:
        return "docs"
    if any(path.startswith(prefix) for prefix in DOC_PREFIXES):
        return "docs"
    if any(path.startswith(prefix) for prefix in TOOL_PREFIXES):
        return "tooling"
    if path.startswith(JUMBO_PREFIX):
        return "jumbo-source-list"
    if any(path.startswith(prefix) for prefix in SAFE_CPP_PREFIXES):
        return "safe-cpp" if ext in CPP_EXTS else "safe-other"
    if any(path.startswith(prefix) for prefix in MATCH_SENSITIVE_PREFIXES):
        return "match-sensitive-cpp" if ext in CPP_EXTS else "match-sensitive-other"
    return "other"


def file_candidates_from_base(base: str, include_worktree: bool) -> List[str]:
    files: Set[str] = set()
    for line in run_git(["diff", "--name-only", f"{base}...HEAD"]).splitlines():
        if line.strip():
            files.add(line.strip())
    if include_worktree:
        for line in run_git(["diff", "--name-only"]).splitlines():
            if line.strip():
                files.add(line.strip())
    return sorted(files)


def collect_touched_lines_from_diff(diff_text: str) -> Dict[str, Set[int]]:
    touched: Dict[str, Set[int]] = {}
    current_path: Optional[str] = None

    for line in diff_text.splitlines():
        if line.startswith("+++ b/"):
            current_path = line[6:]
            touched.setdefault(current_path, set())
            continue

        if not line.startswith("@@") or current_path is None:
            continue

        match = re.search(r"\+(\d+)(?:,(\d+))?", line)
        if match is None:
            continue

        start = int(match.group(1))
        count = int(match.group(2) or "1")
        if count == 0:
            continue

        for line_no in range(start, start + count):
            touched.setdefault(current_path, set()).add(line_no)

    return touched


def touched_lines_from_base(base: str, include_worktree: bool) -> Dict[str, Set[int]]:
    touched = collect_touched_lines_from_diff(
        run_git(["diff", "--unified=0", f"{base}...HEAD"])
    )
    if include_worktree:
        worktree_touched = collect_touched_lines_from_diff(
            run_git(["diff", "--unified=0"])
        )
        for path, lines in worktree_touched.items():
            touched.setdefault(path, set()).update(lines)
    return touched


def read_text(path: str) -> str:
    with open(
        os.path.join(root_dir, relpath(path)),
        encoding="utf-8",
        errors="ignore",
    ) as f:
        return f.read()


def source_declaration_index() -> Dict[str, List[tuple]]:
    global _source_decl_cache
    if _source_decl_cache is not None:
        return _source_decl_cache

    index: Dict[str, List[tuple]] = {}
    for dirpath, _dirs, files in os.walk(src_dir):
        for fname in files:
            if os.path.splitext(fname)[1] not in CPP_EXTS:
                continue
            fpath = os.path.join(dirpath, fname)
            rel = os.path.relpath(fpath, root_dir).replace("\\", "/")
            try:
                with open(fpath, encoding="utf-8", errors="ignore") as f:
                    for lineno, line in enumerate(f, 1):
                        match = DECL_PATTERN.match(line)
                        if match is None:
                            continue
                        kind = match.group(1)
                        name = match.group(2)
                        index.setdefault(name, []).append((kind, rel, lineno))
            except OSError:
                continue

    _source_decl_cache = index
    return index


def expected_kind_from_source(name: str, current_path: str, current_line: int) -> Optional[str]:
    candidates = source_declaration_index().get(name, [])
    filtered = []
    for kind, rel, lineno in candidates:
        if rel == current_path and lineno == current_line:
            continue
        if os.path.splitext(rel)[1] not in {".h", ".hh", ".hpp"}:
            continue
        filtered.append(kind)
    unique = sorted(set(filtered))
    if len(unique) == 1:
        return unique[0]
    return None


def header_declaration_paths(name: str, current_path: str, current_line: int) -> List[str]:
    candidates = source_declaration_index().get(name, [])
    headers = set()
    for _kind, rel, lineno in candidates:
        if rel == current_path and lineno == current_line:
            continue
        if os.path.splitext(rel)[1] not in {".h", ".hh", ".hpp"}:
            continue
        headers.add(rel)
    return sorted(headers)


def expected_kind_from_ps2(name: str) -> Optional[str]:
    cached = _ps2_kind_cache.get(name)
    if cached is not None or name in _ps2_kind_cache:
        return cached

    if not os.path.isfile(ps2_types_path):
        _ps2_kind_cache[name] = None
        return None

    result = subprocess.run(
        ["python", "tools/lookup.py", "--file", ps2_types_path, "struct", name],
        cwd=root_dir,
        capture_output=True,
        text=True,
    )
    output = (result.stdout or result.stderr).strip()
    if result.returncode != 0 or not output.startswith("struct "):
        _ps2_kind_cache[name] = None
        return None

    if VISIBILITY_PATTERN.search(output):
        _ps2_kind_cache[name] = "class"
    else:
        _ps2_kind_cache[name] = "struct"
    return _ps2_kind_cache[name]


def audit_type_kind_declarations(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings: List[Finding] = []
    for idx, line in enumerate(text.splitlines(), 1):
        if touched_lines is not None and idx not in touched_lines:
            continue
        match = DECL_PATTERN.match(line)
        if match is None:
            continue

        actual_kind = match.group(1)
        name = match.group(2)
        expected_kind = expected_kind_from_source(name, path, idx)
        reason = "repo declaration"
        if expected_kind is None:
            expected_kind = expected_kind_from_ps2(name)
            reason = "PS2 visibility rule"
        if expected_kind is None or expected_kind == actual_kind:
            continue

        findings.append(
            Finding(
                path,
                idx,
                "WARN",
                f"`{actual_kind} {name}` disagrees with known type kind; use `{expected_kind} {name}` ({reason})",
            )
        )
    return findings


def extract_member_name(line: str) -> Optional[str]:
    code = line.split("//", 1)[0].strip()
    if not code or code.startswith("#") or code.endswith(":"):
        return None
    if "(" in code or ")" in code:
        return None
    if any(code.startswith(prefix) for prefix in ("typedef ", "using ", "enum ", "union ", "class ", "struct ", "friend ")):
        return None

    code = code.rstrip(";").strip()
    if "," in code:
        return None
    if "=" in code:
        code = code.split("=", 1)[0].rstrip()

    match = re.search(
        r"([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]+\])?\s*(?::\s*\d+)?\s*$",
        code,
    )
    if match is None:
        return None
    return match.group(1)


def audit_placeholder_members(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings: List[Finding] = []
    current_type: Optional[str] = None
    pending_type: Optional[str] = None
    brace_depth = 0

    for idx, line in enumerate(text.splitlines(), 1):
        stripped = line.strip()

        if current_type is None:
            start_match = TYPE_BODY_START_PATTERN.match(line)
            if start_match is not None:
                current_type = start_match.group(2)
                brace_depth = line.count("{") - line.count("}")
                if brace_depth <= 0:
                    current_type = None
                    brace_depth = 0
                continue

            decl_match = DECL_PATTERN.match(line)
            if decl_match is not None and "{" not in line and not stripped.endswith(";"):
                pending_type = decl_match.group(2)

            if pending_type is not None and "{" in line:
                current_type = pending_type
                pending_type = None
                brace_depth = line.count("{") - line.count("}")
                if brace_depth <= 0:
                    current_type = None
                    brace_depth = 0
                continue

            if stripped.endswith(";"):
                pending_type = None
            continue

        if touched_lines is None or idx in touched_lines:
            if not ACCESS_SPECIFIER_PATTERN.match(line):
                member_name = extract_member_name(line)
                if member_name is not None and SUSPICIOUS_MEMBER_PATTERN.match(member_name):
                    findings.append(
                        Finding(
                            path,
                            idx,
                            "WARN",
                            f"`{current_type}` member `{member_name}` looks like placeholder padding/unknown naming; verify the real member from Dwarf/PS2 instead of inventing pads",
                        )
                    )

        brace_depth += line.count("{") - line.count("}")
        if brace_depth <= 0:
            current_type = None
            brace_depth = 0

    return findings


def audit_forward_declarations(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings: List[Finding] = []
    for idx, line in enumerate(text.splitlines(), 1):
        if touched_lines is not None and idx not in touched_lines:
            continue
        match = FORWARD_DECL_PATTERN.match(line)
        if match is None:
            continue

        name = match.group(2)
        headers = header_declaration_paths(name, path, idx)
        if not headers:
            continue

        sample = ", ".join(headers[:2])
        if len(headers) > 2:
            sample += ", ..."
        findings.append(
            Finding(
                path,
                idx,
                "WARN",
                f"`{name}` is forward-declared even though repo headers exist; include {sample} instead of redeclaring",
            )
        )
    return findings


def audit_style_guide_rules(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings: List[Finding] = []
    ext = os.path.splitext(path)[1]

    for idx, line in enumerate(text.splitlines(), 1):
        if touched_lines is not None and idx not in touched_lines:
            continue
        stripped = line.strip()
        bare_recovery_marker_match = BARE_RECOVERY_MARKER_PATTERN.search(line)
        if bare_recovery_marker_match is not None:
            findings.append(
                Finding(
                    path,
                    idx,
                    "INFO",
                    f"`// {bare_recovery_marker_match.group(1)}` has no context; add a short reason or remove the stale recovery marker",
                )
            )
        if stripped.startswith("//"):
            continue

        if CAST_SPACING_PATTERN.search(line):
            findings.append(
                Finding(
                    path,
                    idx,
                    "WARN",
                    "C++ cast uses spaces inside `<...>`; prefer `static_cast<Type *>(expr)` style",
                )
            )
        if USING_NAMESPACE_PATTERN.search(line):
            findings.append(
                Finding(
                    path,
                    idx,
                    "WARN",
                    "`using namespace` is not allowed here; keep names fully qualified",
                )
            )
        if NULL_PATTERN.search(line):
            findings.append(
                Finding(
                    path,
                    idx,
                    "WARN",
                    "use `nullptr` instead of `NULL`",
                )
            )
        bare_presence_if_match = BARE_PRESENCE_IF_PATTERN.match(line)
        if bare_presence_if_match is not None:
            findings.append(
                Finding(
                    path,
                    idx,
                    "WARN",
                    f"bare `#if {bare_presence_if_match.group(1)}` looks like a presence check; prefer `#ifdef {bare_presence_if_match.group(1)}` unless a numeric test is intentional",
                )
            )
        narrow_type_match = RECOVERED_NARROW_UNSIGNED_PATTERN.search(line)
        if (
            narrow_type_match is not None
            and RECOVERED_LAYOUT_COMMENT_PATTERN.search(line) is not None
        ):
            preferred = "uint8" if narrow_type_match.group(1) == "char" else "uint16"
            findings.append(
                Finding(
                    path,
                    idx,
                    "INFO",
                    f"recovered layout member uses `{narrow_type_match.group(0)}`; prefer explicit-width `{preferred}` when the field width is known",
                )
            )

    if ext in HEADER_EXTS:
        should_check_guard = touched_lines is None or any(
            line_no <= 12 for line_no in touched_lines
        )
        if should_check_guard:
            has_ifndef = HEADER_GUARD_IFNDEF_PATTERN.search(text) is not None
            has_define = HEADER_GUARD_DEFINE_PATTERN.search(text) is not None
            has_pragma_block = EA_PRAGMA_BLOCK_PATTERN.search(text) is not None
            if not (has_ifndef and has_define and has_pragma_block):
                findings.append(
                    Finding(
                        path,
                        1,
                        "WARN",
                        "header guard should use `#ifndef` / `#define` plus the `EA_PRAGMA_ONCE_SUPPORTED` `#pragma once` block",
                    )
                )
            pragma_ifdef_match = EA_PRAGMA_IFDEF_PATTERN.search(text)
            if pragma_ifdef_match is not None:
                pragma_ifdef_line = text[: pragma_ifdef_match.start()].count("\n") + 1
                for idx, line in enumerate(text.splitlines(), 1):
                    if line.strip().startswith("#include ") and idx < pragma_ifdef_line:
                        findings.append(
                            Finding(
                                path,
                                idx,
                                "WARN",
                                "header include appears before the `EA_PRAGMA_ONCE_SUPPORTED` block; keep the guard / pragma block ahead of includes",
                            )
                        )
                        break

    return findings


def audit_source_list(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings: List[Finding] = []
    lines = text.splitlines()
    seen_include = False
    prev_include_line = -1

    for idx, line in enumerate(lines, 1):
        stripped = line.strip()
        if not seen_include:
            if stripped.startswith("#include "):
                seen_include = True
                prev_include_line = idx
            continue

        if stripped.startswith("#include "):
            if idx == prev_include_line + 1 and (
                touched_lines is None
                or idx in touched_lines
                or prev_include_line in touched_lines
            ):
                findings.append(
                    Finding(
                        path,
                        idx,
                        "WARN",
                        "consecutive jumbo includes without a separating blank line",
                    )
                )
            prev_include_line = idx
            continue

        if stripped == "":
            continue

        if touched_lines is None or idx in touched_lines:
            findings.append(
                Finding(
                    path,
                    idx,
                    "INFO",
                    "top-level declaration/code in SourceLists file; keep only if placement is intentional",
                )
            )
        break

    return findings


def audit_safe_cpp(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings = audit_type_kind_declarations(path, text, touched_lines)
    findings.extend(audit_forward_declarations(path, text, touched_lines))
    findings.extend(audit_placeholder_members(path, text, touched_lines))
    findings.extend(audit_style_guide_rules(path, text, touched_lines))
    pointer_pattern = re.compile(
        r"\b[A-Za-z_][A-Za-z0-9_:<>]*\*\s*[A-Za-z_][A-Za-z0-9_]*"
    )

    for idx, line in enumerate(text.splitlines(), 1):
        stripped = line.strip()
        if stripped.startswith("//") or stripped.startswith("#"):
            continue
        if touched_lines is not None and idx not in touched_lines:
            continue
        if pointer_pattern.search(line):
            findings.append(
                Finding(
                    path,
                    idx,
                    "INFO",
                    "pointer declaration/prototype uses `Type* name`; repo style prefers `Type *name`",
                )
            )
    return findings


def audit_match_sensitive_cpp(
    path: str, text: str, touched_lines: Optional[Set[int]]
) -> List[Finding]:
    findings = audit_type_kind_declarations(path, text, touched_lines)
    findings.extend(audit_forward_declarations(path, text, touched_lines))
    findings.extend(audit_placeholder_members(path, text, touched_lines))
    findings.extend(audit_style_guide_rules(path, text, touched_lines))
    nullptr_pattern = re.compile(r"\bif\s*\([^)]*(?:==|!=)\s*nullptr")

    for idx, line in enumerate(text.splitlines(), 1):
        if touched_lines is not None and idx not in touched_lines:
            continue
        if nullptr_pattern.search(line):
            findings.append(
                Finding(
                    path,
                    idx,
                    "INFO",
                    "pointer-null comparison is a candidate for `if (ptr)` cleanup; in match-sensitive code, prefer a mechanical full-TU pass and then rebuild/status-check that unit",
                )
            )
    return findings


def audit_path(path: str, touched_lines: Optional[Set[int]]) -> List[Finding]:
    path = relpath(path)
    abs_path = os.path.join(root_dir, path)
    if not os.path.isfile(abs_path):
        return []

    category = path_category(path)
    text = read_text(path)

    if category == "jumbo-source-list":
        return audit_source_list(path, text, touched_lines)
    if category == "safe-cpp":
        return audit_safe_cpp(path, text, touched_lines)
    if category == "match-sensitive-cpp":
        return audit_match_sensitive_cpp(path, text, touched_lines)
    return []


def gather_paths(args: argparse.Namespace) -> List[str]:
    if args.paths:
        return [relpath(path) for path in args.paths]
    return file_candidates_from_base(args.base, include_worktree=not args.no_worktree)


def filter_paths_by_category(
    paths: Iterable[str], categories: Optional[Sequence[str]]
) -> List[str]:
    if not categories:
        return list(paths)
    allowed = set(categories)
    return [path for path in paths if path_category(path) in allowed]


def format_line_list(lines: Sequence[int], sample_limit: int) -> str:
    sample = list(lines[:sample_limit])
    rendered = ", ".join(str(line) for line in sample)
    if len(lines) > sample_limit:
        rendered += ", ..."
    return rendered


def strip_whitespace(text: str) -> str:
    return re.sub(r"\s+", "", text)


def include_lines(text: str) -> List[str]:
    return [line.strip() for line in text.splitlines() if line.strip().startswith("#include ")]


def has_initializer_guard_comments(text: str) -> bool:
    guard_pattern = re.compile(r"^\s*(?::|,)\s+.*//\s*$", re.MULTILINE)
    return guard_pattern.search(text) is not None


def format_change_summary(before: str, after: str) -> str:
    reasons: List[str] = []
    if strip_whitespace(before) == strip_whitespace(after):
        reasons.append("whitespace-only")
    else:
        reasons.append("non-whitespace token/order changes")

    before_includes = include_lines(before)
    after_includes = include_lines(after)
    if before_includes and before_includes != after_includes and sorted(before_includes) == sorted(after_includes):
        reasons.append("reorders includes")

    if has_initializer_guard_comments(before):
        reasons.append("initializer-list guard comments present")

    return ", ".join(reasons)


def command_classify(args: argparse.Namespace) -> int:
    for path in filter_paths_by_category(gather_paths(args), args.category):
        print(f"{path_category(path):<22} {path}")
    return 0


def command_audit(args: argparse.Namespace) -> int:
    paths = filter_paths_by_category(gather_paths(args), args.category)
    if not paths:
        print("No files selected.")
        return 0

    touched_lines = None if args.paths else touched_lines_from_base(
        args.base, include_worktree=not args.no_worktree
    )

    by_category = {}
    findings: List[Finding] = []
    for path in paths:
        by_category.setdefault(path_category(path), []).append(path)
        findings.extend(
            audit_path(path, None if touched_lines is None else touched_lines.get(path))
        )

    print("File categories:")
    for category in sorted(by_category):
        print(f"  {category}: {len(by_category[category])}")
    print()

    safe_cpp_candidates = [
        path
        for path in paths
        if path_category(path) == "safe-cpp" and os.path.splitext(path)[1] in CPP_EXTS
    ]
    if safe_cpp_candidates:
        print("Focused safe-cpp subset:")
        for path in safe_cpp_candidates:
            print(f"  {path}")
        print()

    if not findings:
        print("No style findings.")
        return 0

    print("Findings:")
    findings = sorted(findings, key=lambda item: (item.path, item.line, item.message))
    if args.ungrouped:
        shown = findings[: args.max_findings]
        for finding in shown:
            print(
                f"  {finding.severity:<4} {finding.path}:{finding.line}: {finding.message}"
            )
        if len(findings) > len(shown):
            print()
            print(f"  ... {len(findings) - len(shown)} more finding(s) omitted")
        return 0

    grouped: Dict[tuple, List[int]] = {}
    for finding in findings:
        grouped.setdefault(
            (finding.severity, finding.path, finding.message), []
        ).append(finding.line)

    grouped_items = sorted(grouped.items(), key=lambda item: (item[0][1], item[1][0], item[0][2]))
    shown = grouped_items[: args.max_findings]
    for (severity, path, message), lines in shown:
        print(
            f"  {severity:<4} {path}: {message} ({len(lines)} occurrence(s); lines {format_line_list(lines, args.sample_lines)})"
        )
    if len(grouped_items) > len(shown):
        print()
        print(f"  ... {len(grouped_items) - len(shown)} more grouped finding(s) omitted")
    return 0


def find_clang_format() -> str:
    env_override = os.environ.get("CLANG_FORMAT")
    if env_override:
        if os.path.isfile(env_override) and os.access(env_override, os.X_OK):
            return env_override
        resolved = shutil.which(env_override)
        if resolved is not None:
            return resolved
        raise RuntimeError(
            f"CLANG_FORMAT is set to '{env_override}', but that executable was not found."
        )

    candidates = (
        "clang-format",
        "clang-format-19",
        "clang-format-18",
        "clang-format-17",
        "clang-format-16",
        "clang-format-15",
        "clang-format-14",
    )
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved is not None:
            return resolved

    system = platform.system()
    if system == "Darwin":
        install_hint = "Install it with `brew install clang-format`."
    elif system == "Linux":
        install_hint = "Install it with your package manager, for example `sudo apt install clang-format`."
    elif system == "Windows":
        install_hint = "Install LLVM/clang-format, for example with `winget install LLVM.LLVM`."
    else:
        install_hint = "Install clang-format and ensure it is available on PATH."

    raise RuntimeError(
        "clang-format not found. "
        + install_hint
        + " You can also point the helper at a specific binary with the CLANG_FORMAT environment variable."
    )


def format_paths(paths: Iterable[str], include_match_sensitive: bool) -> List[str]:
    del include_match_sensitive

    return [
        relpath(path)
        for path in paths
        if os.path.splitext(path)[1] in CPP_EXTS
    ]


def command_format(args: argparse.Namespace) -> int:
    selected = format_paths(
        filter_paths_by_category(gather_paths(args), args.category),
        args.include_match_sensitive,
    )
    if not selected:
        print("No format-eligible files selected.")
        return 0

    clang_format = find_clang_format()
    changed: List[str] = []
    changed_summaries: Dict[str, str] = {}
    for path in selected:
        abs_path = os.path.join(root_dir, path)
        with open(abs_path, encoding="utf-8", errors="ignore") as f:
            before = f.read()

        if args.check:
            result = subprocess.run(
                [clang_format, "--style=file", abs_path],
                capture_output=True,
                text=True,
            )
            if result.returncode != 0:
                print(result.stderr.strip(), file=sys.stderr)
                return result.returncode
            if result.stdout != before:
                changed.append(path)
                changed_summaries[path] = format_change_summary(before, result.stdout)
        else:
            result = subprocess.run([clang_format, "-i", "--style=file", abs_path])
            if result.returncode != 0:
                return result.returncode
            changed.append(path)

    if args.check:
        if changed:
            print("Would reformat:")
            for path in changed:
                print(f"  {path} [{changed_summaries[path]}]")
            return 1
        print("All selected files already match clang-format output.")
        return 0

    print("Formatted files:")
    for path in changed:
        print(f"  {path}")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Decomp-aware code style helper")
    subparsers = parser.add_subparsers(dest="command", required=True)

    shared = argparse.ArgumentParser(add_help=False)
    shared.add_argument(
        "paths",
        nargs="*",
        help="Files to inspect. If omitted, use changed files against --base.",
    )
    shared.add_argument(
        "--base",
        default="origin/main",
        help="Base ref used when paths are omitted (default: origin/main)",
    )
    shared.add_argument(
        "--no-worktree",
        action="store_true",
        help="Ignore uncommitted worktree changes when collecting default files",
    )

    classify = subparsers.add_parser(
        "classify",
        parents=[shared],
        help="Classify files by style-risk bucket",
    )
    classify.add_argument(
        "--category",
        action="append",
        choices=CATEGORIES,
        help="Restrict output to one or more categories",
    )
    classify.set_defaults(func=command_classify)

    audit = subparsers.add_parser(
        "audit",
        parents=[shared],
        help="Audit files for repo-specific style issues",
    )
    audit.add_argument(
        "--category",
        action="append",
        choices=CATEGORIES,
        help="Restrict the audit to one or more categories",
    )
    audit.add_argument(
        "--max-findings",
        type=int,
        default=60,
        help="Maximum number of findings or grouped findings to print (default: 60)",
    )
    audit.add_argument(
        "--sample-lines",
        type=int,
        default=5,
        help="Maximum line samples to print per grouped finding (default: 5)",
    )
    audit.add_argument(
        "--ungrouped",
        action="store_true",
        help="Print individual findings instead of grouped summaries",
    )
    audit.set_defaults(func=command_audit)

    fmt = subparsers.add_parser(
        "format",
        parents=[shared],
        help="Run clang-format on changed C/C++ files by default",
    )
    fmt.add_argument(
        "--category",
        action="append",
        choices=CATEGORIES,
        help="Restrict the format pass to one or more categories",
    )
    fmt.add_argument(
        "--check",
        action="store_true",
        help="Report files that would change instead of formatting them",
    )
    fmt.add_argument(
        "--include-match-sensitive",
        action="store_true",
        help="Deprecated no-op kept for compatibility; eligible match-sensitive C/C++ files are already included by default.",
    )
    fmt.add_argument(
        "--include-initializer-guards",
        action="store_true",
        help="Deprecated no-op kept for compatibility; files with initializer-list guard comments are formatted by default.",
    )
    fmt.set_defaults(func=command_format)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    try:
        return args.func(args)
    except RuntimeError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
