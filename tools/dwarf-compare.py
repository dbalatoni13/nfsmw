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
import difflib
import json
import os
import re
import sys
from typing import Any, Dict, Iterable, List, Optional, Sequence, Tuple

from _common import ROOT_DIR, ToolError, find_objdiff_unit, load_objdiff_config, make_abs
from lookup import (
    _candidate_func_names,
    _normalise_func_name,
    _sig_contains_name,
    read_text,
    split_functions,
)


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
TOOLS_DIR = os.path.join(ROOT_DIR, "tools")
GC_DWARF = os.path.join(ROOT_DIR, "symbols", "Dwarf")
DTK = os.path.join(ROOT_DIR, "build", "tools", "dtk")
HEX_RE = re.compile(r"0x[0-9A-Fa-f]+")


class DwarfCompareError(RuntimeError):
    pass


FunctionBlock = Tuple[str, str, str, str]


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
        raise DwarfCompareError(f"Unit has no build target in objdiff.json: {unit_name}")
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

    tool_output = "\n".join(
        part.strip() for part in [result.stdout, result.stderr] if part.strip()
    )
    if "ERROR " in tool_output or tool_output.startswith("ERROR"):
        maybe_remove(output_path)
        raise DwarfCompareError(
            f"dtk reported an error while dumping DWARF:\n{tool_output}"
        )

    if not os.path.exists(output_path):
        raise DwarfCompareError("dtk dwarf dump succeeded but did not write an output file")

    return output_path


def load_function_blocks(path: str, folder_mode: bool) -> List[FunctionBlock]:
    if folder_mode:
        text = read_text(os.path.join(path, "functions.nothpp"))
    else:
        text = read_text(path)
    return split_functions(text)


def find_function_blocks(funcs: Iterable[FunctionBlock], query: str) -> List[FunctionBlock]:
    candidates = _candidate_func_names(query)
    matches: List[FunctionBlock] = []
    exact_substring_matches: List[FunctionBlock] = []

    for func in funcs:
        sig_line = func[2]
        if query in sig_line:
            exact_substring_matches.append(func)
        if any(_sig_contains_name(sig_line, candidate) for candidate in candidates):
            matches.append(func)

    if exact_substring_matches:
        return exact_substring_matches
    return matches


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


def count_lines_for_opcodes(opcodes: Sequence[Tuple[str, int, int, int, int]]) -> Dict[str, int]:
    matching = 0
    original_only = 0
    rebuilt_only = 0
    changed_groups = 0
    for tag, i1, i2, j1, j2 in opcodes:
        if tag == "equal":
            matching += i2 - i1
            continue
        changed_groups += 1
        if tag in ("replace", "delete"):
            original_only += i2 - i1
        if tag in ("replace", "insert"):
            rebuilt_only += j2 - j1
    return {
        "matching_lines": matching,
        "original_only_lines": original_only,
        "rebuilt_only_lines": rebuilt_only,
        "changed_groups": changed_groups,
    }


def build_diff_lines(
    original_lines: Sequence[str],
    rebuilt_lines: Sequence[str],
    function_name: str,
    context: int,
    collapse: bool,
) -> List[str]:
    if list(original_lines) == list(rebuilt_lines):
        return []

    rendered: List[str] = [
        f"--- original:{function_name}",
        f"+++ rebuilt:{function_name}",
    ]

    matcher = difflib.SequenceMatcher(a=original_lines, b=rebuilt_lines)
    for group in matcher.get_grouped_opcodes(context if collapse else max(len(original_lines), len(rebuilt_lines))):
        first = group[0]
        last = group[-1]
        a_start = first[1] + 1
        a_len = last[2] - first[1]
        b_start = first[3] + 1
        b_len = last[4] - first[3]
        rendered.append(f"@@ -{a_start},{a_len} +{b_start},{b_len} @@")

        for tag, i1, i2, j1, j2 in group:
            if tag == "equal":
                for idx in range(i1, i2):
                    rendered.append(f"  L{idx + 1:04d} {original_lines[idx]}")
                continue

            if tag in ("replace", "delete"):
                for idx in range(i1, i2):
                    rendered.append(f"- L{idx + 1:04d} {original_lines[idx]}")
            if tag in ("replace", "insert"):
                for idx in range(j1, j2):
                    rendered.append(f"+ L{idx + 1:04d} {rebuilt_lines[idx]}")

    return rendered


def build_report(
    unit_name: str,
    function_name: str,
    original_block: FunctionBlock,
    rebuilt_block: FunctionBlock,
    collapse: bool,
    context: int,
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
    signature_match = normalize_line(original_block[2]) == normalize_line(rebuilt_block[2])
    raw_exact_match = original_raw == rebuilt_raw
    normalized_exact_match = original_lines == rebuilt_lines

    diff_lines = build_diff_lines(
        original_lines,
        rebuilt_lines,
        function_name,
        context=context,
        collapse=collapse,
    )
    mismatch_summaries: List[str] = []
    for tag, i1, i2, j1, j2 in opcodes:
        if tag == "equal":
            continue
        original_span = (
            f"L{i1 + 1:04d}" if i2 - i1 <= 1 else f"L{i1 + 1:04d}-L{i2:04d}"
        ) if tag in ("replace", "delete") else "-"
        rebuilt_span = (
            f"L{j1 + 1:04d}" if j2 - j1 <= 1 else f"L{j1 + 1:04d}-L{j2:04d}"
        ) if tag in ("replace", "insert") else "-"

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
        mismatch_summaries.append(
            f"- {original_span} -> {rebuilt_span}: {detail}"
        )

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
        print(f"Raw textual exact match: {'yes' if report['raw_exact_match'] else 'no'}")
    print(
        "Address-only range differences are normalized out so the percentage tracks "
        "structural/function-body DWARF changes."
    )
    if not report["signature_match"]:
        print()
        print("Original signature:")
        print(f"  {report['original_signature']}")
        print("Rebuilt signature:")
        print(f"  {report['rebuilt_signature']}")


def print_diff(report: Dict[str, Any]) -> None:
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
    parser.add_argument("-f", "--function", required=True, help="Function name to compare")
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

    rebuilt_dwarf_path: Optional[str] = None
    cleanup_rebuilt_dwarf = False
    try:
        if args.rebuilt_dwarf_file:
            rebuilt_dwarf_path = os.path.abspath(args.rebuilt_dwarf_file)
        else:
            obj_path = get_unit_build_output(args.unit)
            if not os.path.exists(obj_path):
                raise DwarfCompareError(
                    f"Missing built object for {args.unit}: {obj_path}\n"
                    f"Hint: run `python tools/decomp-workflow.py build -u {args.unit}` "
                    "or use the wrapper `python tools/decomp-workflow.py dwarf ...`."
                )
            rebuilt_dwarf_path = dtk_dwarf_dump(obj_path)
            cleanup_rebuilt_dwarf = True

        original_funcs = load_function_blocks(GC_DWARF, folder_mode=True)
        rebuilt_funcs = load_function_blocks(rebuilt_dwarf_path, folder_mode=False)

        original_block = choose_function_block(original_funcs, args.function, "original DWARF")
        rebuilt_block = choose_function_block(rebuilt_funcs, args.function, "rebuilt DWARF")

        report = build_report(
            args.unit,
            args.function,
            original_block,
            rebuilt_block,
            collapse=not args.no_collapse,
            context=args.context,
        )

        if args.json:
            print(json.dumps(report, indent=2))
            if args.require_exact and not report["normalized_exact_match"]:
                sys.exit(1)
            return

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


if __name__ == "__main__":
    main()
