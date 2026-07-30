#!/usr/bin/env python3
"""Generate a target-weighted decomp.dev report for normalized GC DWARF."""

import argparse
import importlib.util
import json
import os
import sys
from typing import Any, Dict, List


ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
TOOLS = os.path.join(ROOT, "tools")
sys.path.insert(0, TOOLS)


def load_compare_module():
    path = os.path.join(TOOLS, "dwarf-compare.py")
    spec = importlib.util.spec_from_file_location("dwarf_compare", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Unable to load {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def measures(
    total_lines: int,
    fuzzy_matching_lines: int,
    perfectly_matching_lines: int,
    total_functions: int,
    perfectly_matching_functions: int,
) -> Dict[str, Any]:
    fuzzy_percent = 100.0 * fuzzy_matching_lines / max(total_lines, 1)
    matched_percent = 100.0 * perfectly_matching_lines / max(total_lines, 1)
    function_percent = (
        100.0 * perfectly_matching_functions / max(total_functions, 1)
    )
    return {
        "fuzzy_match_percent": fuzzy_percent,
        "total_code": str(total_lines),
        "matched_code": str(perfectly_matching_lines),
        "matched_code_percent": matched_percent,
        "total_functions": total_functions,
        "matched_functions": perfectly_matching_functions,
        "matched_functions_percent": function_percent,
        "total_units": 1,
    }


def aggregate_measures(units: List[Dict[str, Any]]) -> Dict[str, Any]:
    total_lines = sum(int(unit["measures"]["total_code"]) for unit in units)
    fuzzy_matching_lines = sum(
        round(
            int(unit["measures"]["total_code"])
            * unit["measures"]["fuzzy_match_percent"]
            / 100.0
        )
        for unit in units
    )
    perfectly_matching_lines = sum(
        int(unit["measures"]["matched_code"]) for unit in units
    )
    total_functions = sum(unit["measures"]["total_functions"] for unit in units)
    perfectly_matching_functions = sum(
        unit["measures"]["matched_functions"] for unit in units
    )
    result = measures(
        total_lines,
        fuzzy_matching_lines,
        perfectly_matching_lines,
        total_functions,
        perfectly_matching_functions,
    )
    result["total_units"] = len(units)
    return result


def compare_unit(
    dc,
    unit: Dict[str, Any],
    target_unit: Dict[str, Any],
    original_by_start,
) -> Dict[str, Any]:
    rebuilt_funcs = []
    base_path = unit.get("base_path")
    if base_path and os.path.isfile(dc.make_abs(base_path)):
        try:
            rebuilt_path = dc.dtk_dwarf_dump(dc.make_abs(base_path))
            try:
                rebuilt_funcs = dc.load_function_blocks(
                    rebuilt_path, folder_mode=False, apply_split_fixups_in_ram=True
                )
            finally:
                dc.maybe_remove(rebuilt_path)
        except dc.DwarfCompareError as error:
            print(
                f"No rebuilt DWARF for {unit['name']}: {error}", file=sys.stderr
            )
    rebuilt_index = dc.build_function_block_index(rebuilt_funcs)

    functions: List[Dict[str, Any]] = []
    total_lines = fuzzy_matching_lines = perfectly_matching_lines = 0
    perfectly_matching_functions = 0
    seen_addresses = set()
    for target_function in target_unit["functions"]:
        original_address = int(
            target_function.get("metadata", {}).get("virtual_address", -1)
        )
        if original_address in seen_addresses:
            continue
        original = original_by_start.get(original_address)
        if original is None:
            continue
        seen_addresses.add(original_address)

        target_count = max(len(dc.normalize_block(original[3])), 1)
        query = dc.function_query_from_signature(original[2])
        display_name = target_function.get("metadata", {}).get(
            "demangled_name", target_function.get("name", query or original[2])
        )
        function_match = 0
        if query is not None:
            try:
                rebuilt = dc.choose_function_block(
                    dc.indexed_function_blocks(rebuilt_funcs, rebuilt_index, query),
                    query,
                    "rebuilt DWARF",
                )
                report = dc.build_report(
                    unit["name"], query, original, rebuilt, True, 0, None, False
                )
                function_match = min(report["matching_lines"], target_count)
            except dc.DwarfCompareError:
                pass

        total_lines += target_count
        fuzzy_matching_lines += function_match
        if function_match == target_count:
            perfectly_matching_lines += target_count
            perfectly_matching_functions += 1
        functions.append(
            {
                "name": display_name,
                "size": str(target_count),
                "fuzzy_match_percent": 100.0 * function_match / target_count,
            }
        )

    # Use objdiff's report metadata rather than copying its configuration
    # metadata. The latter contains internal fields such as reverse_fn_order
    # that are not part of the generated report.
    metadata = target_unit.get("metadata", {}).copy()
    metadata.pop("complete", None)
    return {
        "name": unit["name"],
        "measures": measures(
            total_lines,
            fuzzy_matching_lines,
            perfectly_matching_lines,
            len(functions),
            perfectly_matching_functions,
        ),
        "functions": functions,
        "metadata": metadata,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default="objdiff.json")
    parser.add_argument("--target-report", default="build/GOWE69/report.json")
    parser.add_argument("--output", default="build/GC_Dwarf/report.json")
    parser.add_argument(
        "--unit", action="append", help="Only compare this unit (repeatable)"
    )
    args = parser.parse_args()

    dc = load_compare_module()
    with open(os.path.join(ROOT, args.config), encoding="utf-8") as f:
        config = json.load(f)

    original_funcs = dc.load_function_blocks(dc.GC_DWARF, folder_mode=True)
    original_by_start = {
        int(block[0], 16): block
        for block in original_funcs
        if int(block[0], 16) != 0xFFFFFFFF
    }
    with open(os.path.join(ROOT, args.target_report), encoding="utf-8") as f:
        target_report = json.load(f)
    target_units = {unit["name"]: unit for unit in target_report.get("units", [])}
    units = []
    for unit in config.get("units", []):
        if args.unit and unit.get("name") not in args.unit:
            continue
        target_unit = target_units.get(unit["name"])
        if target_unit is None or not target_unit.get("functions"):
            continue
        print(f"Comparing DWARF: {unit['name']}", file=sys.stderr)
        try:
            report_unit = compare_unit(
                dc,
                unit,
                target_unit,
                original_by_start,
            )
        except dc.DwarfCompareError as error:
            print(f"Skipping {unit['name']}: {error}", file=sys.stderr)
            continue
        if report_unit["functions"]:
            units.append(report_unit)

    if not units:
        raise RuntimeError("No built translation units with comparable DWARF were found")
    categories = []
    for target_category in target_report.get("categories", []):
        category_id = target_category["id"]
        category_units = [
            unit
            for unit in units
            if category_id
            in unit.get("metadata", {}).get("progress_categories", [])
        ]
        categories.append(
            {
                "id": category_id,
                "name": target_category["name"],
                "measures": aggregate_measures(category_units),
            }
        )
    report = {
        "measures": aggregate_measures(units),
        "units": units,
        "version": target_report.get("version", 2),
        "categories": categories,
    }

    output = os.path.join(ROOT, args.output)
    os.makedirs(os.path.dirname(output), exist_ok=True)
    with open(output, "w", encoding="utf-8") as f:
        json.dump(report, f, separators=(",", ":"))
        f.write("\n")
    print(
        f"Wrote {output} "
        f"({len(units)} units, {report['measures']['total_functions']} functions)"
    )


if __name__ == "__main__":
    main()
