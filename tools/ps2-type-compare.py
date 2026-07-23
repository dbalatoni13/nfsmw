#!/usr/bin/env python3

"""Compare PS2 struct/class definitions from a rebuilt object against stdump ground truth."""

import argparse
import difflib
import json
import os
import re
import subprocess
import sys
from typing import Any, Dict, Iterable, List, Sequence, Tuple

from _common import ROOT_DIR, ToolError, find_objdiff_unit, load_objdiff_config, make_abs
from compare_common import build_diff_lines, count_lines_for_opcodes


DEFAULT_TYPES_PATHS = (
    os.path.join(ROOT_DIR, "symbols", "PS2_types.nothpp"),
    os.path.join(ROOT_DIR, "symbols", "PS2", "PS2_types.nothpp"),
)
TYPE_START_RE = re.compile(
    r"^(?:typedef\s+)?(?:struct|class)\s*(.*?)\s*\{\s*//\s*0x[0-9A-Fa-f]+\s*$"
)
TYPE_END_RE = re.compile(r"^}\s*([^;]+)?;")
TypeBlock = Tuple[str, str]


class Ps2TypeCompareError(RuntimeError):
    pass


def default_types_path() -> str:
    for path in DEFAULT_TYPES_PATHS:
        if os.path.exists(path):
            return path
    return DEFAULT_TYPES_PATHS[0]


def rebuilt_object_for_unit(unit_name: str) -> str:
    unit = find_objdiff_unit(load_objdiff_config(), unit_name)
    if unit is None:
        raise Ps2TypeCompareError(f"Unit not found in objdiff.json: {unit_name}")
    if unit.get("scratch", {}).get("platform") != "ps2":
        raise Ps2TypeCompareError(f"Unit is not configured for PS2: {unit_name}")
    output = unit.get("base_path") or unit.get("target_path")
    if not output:
        raise Ps2TypeCompareError(f"Unit has no rebuilt object path: {unit_name}")
    resolved = make_abs(str(output)) or str(output)
    if not os.path.exists(resolved):
        raise Ps2TypeCompareError(f"Missing rebuilt object for {unit_name}: {resolved}")
    return resolved


def run_stdump(stdump_path: str, object_path: str) -> str:
    if not os.path.isfile(stdump_path):
        raise Ps2TypeCompareError(f"stdump executable not found: {stdump_path}")
    result = subprocess.run(
        [stdump_path, "types", object_path],
        cwd=ROOT_DIR,
        text=True,
        encoding="utf-8",
        errors="replace",
        capture_output=True,
    )
    if result.returncode != 0:
        details = result.stderr.strip() or result.stdout.strip()
        raise Ps2TypeCompareError(
            f"stdump failed with exit code {result.returncode}: {details}"
        )
    return result.stdout


def clean_opening_type_name(name: str) -> str:
    name = name.strip()
    if " : " in name:
        name = name.split(" : ", 1)[0].strip()
    return name


def split_type_blocks(text: str) -> List[TypeBlock]:
    results: List[TypeBlock] = []
    current: List[str] = []
    opening_name = ""
    depth = 0
    for line in text.splitlines(keepends=True):
        if not current:
            match = TYPE_START_RE.match(line.rstrip("\r\n"))
            if match is None:
                continue
            opening_name = clean_opening_type_name(match.group(1))
            current = [line]
            depth = line.count("{") - line.count("}")
            continue

        current.append(line)
        depth += line.count("{") - line.count("}")
        if depth != 0:
            continue
        end_match = TYPE_END_RE.match(line.strip())
        closing_name = end_match.group(1).strip() if end_match and end_match.group(1) else ""
        name = opening_name or closing_name
        if name:
            results.append((name, "".join(current)))
        current = []
        opening_name = ""
    return results


def index_type_blocks(blocks: Iterable[TypeBlock]) -> Dict[str, List[str]]:
    index: Dict[str, List[str]] = {}
    for name, block in blocks:
        index.setdefault(name, []).append(block)
        if "::" in name:
            index.setdefault(name.split("::")[-1], []).append(block)
    return index


def normalize_type_line(line: str) -> str:
    return line.rstrip().replace("\t", "    ")


def normalize_type_block(block: str) -> List[str]:
    return [normalize_type_line(line) for line in block.splitlines()]


def choose_ground_truth_variant(variants: Sequence[str], rebuilt: str) -> str:
    rebuilt_lines = normalize_type_block(rebuilt)
    return max(
        variants,
        key=lambda block: difflib.SequenceMatcher(
            a=normalize_type_block(block), b=rebuilt_lines, autojunk=False
        ).ratio(),
    )


def build_type_report(type_name: str, original: str, rebuilt: str) -> Dict[str, Any]:
    original_lines = normalize_type_block(original)
    rebuilt_lines = normalize_type_block(rebuilt)
    matcher = difflib.SequenceMatcher(a=original_lines, b=rebuilt_lines, autojunk=False)
    opcodes = matcher.get_opcodes()
    counts = count_lines_for_opcodes(opcodes)
    total_lines = max(len(original_lines), len(rebuilt_lines), 1)
    return {
        "type": type_name,
        "match_percent": 100.0 * counts["matching_lines"] / total_lines,
        "matching_lines": counts["matching_lines"],
        "total_lines": total_lines,
        "original_line_count": len(original_lines),
        "rebuilt_line_count": len(rebuilt_lines),
        "original_only_lines": counts["original_only_lines"],
        "rebuilt_only_lines": counts["rebuilt_only_lines"],
        "changed_groups": counts["changed_groups"],
        "normalized_exact_match": original_lines == rebuilt_lines,
        "diff_lines": build_diff_lines(
            original_lines, rebuilt_lines, type_name, context=3, collapse=True
        ),
    }


def compare_types(
    queries: Sequence[str], original_text: str, rebuilt_text: str
) -> List[Dict[str, Any]]:
    original_index = index_type_blocks(split_type_blocks(original_text))
    rebuilt_index = index_type_blocks(split_type_blocks(rebuilt_text))
    reports: List[Dict[str, Any]] = []
    for query in queries:
        lookup_name = query.removeprefix("struct ").removeprefix("class ").strip()
        fallback_name = lookup_name.split("::")[-1]
        original_variants = original_index.get(lookup_name, []) or original_index.get(
            fallback_name, []
        )
        rebuilt_variants = rebuilt_index.get(lookup_name, []) or rebuilt_index.get(
            fallback_name, []
        )
        if not original_variants:
            reports.append({"type": query, "error": "type not found in PS2 ground truth"})
            continue
        if not rebuilt_variants:
            reports.append({"type": query, "error": "type not found in rebuilt object"})
            continue
        best_report: Dict[str, Any] | None = None
        for rebuilt in rebuilt_variants:
            original = choose_ground_truth_variant(original_variants, rebuilt)
            report = build_type_report(query, original, rebuilt)
            if best_report is None or report["match_percent"] > best_report["match_percent"]:
                best_report = report
        reports.append(best_report or {"type": query, "error": "no comparable type variant"})
    return reports


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-u", "--unit", required=True, help="Objdiff translation unit")
    parser.add_argument("--stdump", required=True, help="Path to ccc stdump.exe")
    types = parser.add_mutually_exclusive_group(required=True)
    types.add_argument("-t", "--type", help="Struct/class name to compare")
    types.add_argument("--types-file", help="JSON array of struct/class names")
    parser.add_argument("--json", action="store_true", help="Emit JSON")
    parser.add_argument("--object", help="Override the rebuilt object path")
    parser.add_argument("--ground-truth", default=default_types_path())
    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    if args.types_file and not args.json:
        parser.error("--types-file requires --json")
    if args.types_file:
        try:
            with open(args.types_file, encoding="utf-8") as f:
                queries = json.load(f)
        except (OSError, json.JSONDecodeError) as e:
            parser.error(f"failed to read --types-file: {e}")
        if not isinstance(queries, list) or not all(
            isinstance(query, str) and query for query in queries
        ):
            parser.error("--types-file must contain a JSON array of non-empty strings")
    else:
        queries = [args.type]

    try:
        object_path = os.path.abspath(args.object) if args.object else rebuilt_object_for_unit(args.unit)
        with open(args.ground_truth, encoding="utf-8", errors="ignore") as f:
            original_text = f.read()
        rebuilt_text = run_stdump(os.path.abspath(args.stdump), object_path)
        reports = compare_types(queries, original_text, rebuilt_text)
    except (OSError, Ps2TypeCompareError, ToolError) as e:
        print(e, file=sys.stderr)
        sys.exit(1)

    if args.json:
        print(json.dumps(reports if args.types_file else reports[0], indent=2))
        return
    report = reports[0]
    if report.get("error"):
        print(f"{report['type']}: {report['error']}", file=sys.stderr)
        sys.exit(1)
    print(
        f"PS2 type match: {report['type']}: {report['match_percent']:.1f}% "
        f"({report['matching_lines']}/{report['total_lines']} lines)"
    )
    for line in report["diff_lines"]:
        print(line)


if __name__ == "__main__":
    main()
