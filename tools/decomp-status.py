#!/usr/bin/env python3

"""
Project-wide decomp status overview.

Reads objdiff.json and runs objdiff-cli on non-complete units to gather
per-unit and per-category matching statistics.

Usage:
  python tools/decomp-status.py
  python tools/decomp-status.py --category game
  python tools/decomp-status.py --unit main/Speed/Indep/SourceLists/zAnim
  python tools/decomp-status.py --json
"""

import argparse
import json
import os
import sys
from typing import Any, Dict, List, Optional, Tuple
from _common import (
    ROOT_DIR,
    ToolError,
    build_objdiff_symbol_rows,
    fail,
    load_objdiff_config,
    run_objdiff_json,
)

root_dir = ROOT_DIR

OBJDIFF_CLI = os.path.join(root_dir, "build", "tools", "objdiff-cli")


def load_project_config() -> Dict[str, Any]:
    """Load objdiff.json project configuration."""
    return load_objdiff_config()


def run_objdiff(unit_name: str) -> Tuple[Optional[Dict[str, Any]], Optional[str]]:
    """Run objdiff-cli diff for a unit and return parsed JSON."""
    try:
        return run_objdiff_json(OBJDIFF_CLI, unit_name, root_dir=root_dir), None
    except ToolError as e:
        return None, str(e)


def analyze_unit(diff_data: Dict[str, Any]) -> Dict[str, Any]:
    """Analyze a unit's diff data and return summary statistics."""
    left = diff_data.get("left", {})
    left_sections = left.get("sections", [])
    symbol_rows = build_objdiff_symbol_rows(diff_data)
    function_rows = [r for r in symbol_rows if r["type"] == "function" and r["side"] == "left"]
    unmatched_function_rows = [
        r
        for r in function_rows
        if r["status"] in ("missing", "nonmatching") and r["unmatched_bytes_est"] > 0
    ]
    unmatched_function_rows.sort(
        key=lambda r: (-r["unmatched_bytes_est"], -r["size"], r["name"].lower())
    )

    # Section-level stats
    section_stats = {}
    for sec in left_sections:
        kind = sec.get("kind", "")
        if kind not in ("SECTION_CODE", "SECTION_DATA", "SECTION_BSS"):
            continue
        name = sec["name"]
        mp = sec.get("match_percent")
        size = int(sec.get("size", "0"))
        section_stats[name] = {
            "match_percent": mp,
            "size": size,
            "kind": kind,
        }

    # Function-level stats (code symbols only)
    total_funcs = 0
    matching_funcs = 0
    total_code_size = 0
    matching_code_size = 0
    remaining_code_size_est = 0

    for row in function_rows:
        size = row["size"]
        total_funcs += 1
        total_code_size += size
        mp = row["match_percent"]
        if mp is not None and mp >= 100.0:
            matching_funcs += 1
            matching_code_size += size
        remaining_code_size_est += row["unmatched_bytes_est"]

    text_section = section_stats.get(".text", {})
    text_match = text_section.get("match_percent")
    text_size = text_section.get("size", 0)

    return {
        "total_functions": total_funcs,
        "matching_functions": matching_funcs,
        "total_code_size": total_code_size,
        "matching_code_size": matching_code_size,
        "remaining_code_size_est": remaining_code_size_est,
        "text_match_percent": text_match,
        "text_size": text_size,
        "sections": section_stats,
        "top_unmatched_functions": [
            {
                "name": row["name"],
                "status": row["status"],
                "size": row["size"],
                "match_percent": row["match_percent"],
                "unmatched_bytes_est": row["unmatched_bytes_est"],
            }
            for row in unmatched_function_rows[:10]
        ],
    }


def main():
    parser = argparse.ArgumentParser(description="Project-wide decomp status overview")
    parser.add_argument("--unit", help="Show detailed status for a specific unit")
    parser.add_argument(
        "--category",
        help="Filter by progress category (game, core, sdk, third_party)",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        dest="json_output",
        help="Output as JSON",
    )
    parser.add_argument(
        "--top-unmatched",
        type=int,
        metavar="N",
        help="Show the top N unmatched functions by estimated unmatched bytes",
    )
    args = parser.parse_args()

    config = load_project_config()
    units = config.get("units", [])

    # Group units by category
    categorized = {}
    for unit in units:
        meta = unit.get("metadata", {})
        categories = meta.get("progress_categories", ["uncategorized"])
        # Use the second category if available (first is usually 'dol')
        cat = (
            categories[1]
            if len(categories) > 1
            else categories[0] if categories else "uncategorized"
        )

        if args.category and cat != args.category:
            continue

        if args.unit and unit["name"] != args.unit:
            continue

        categorized.setdefault(cat, []).append(unit)

    if not categorized:
        fail("No units match the given filters.")

    # Process each unit
    results = {}
    for cat, cat_units in sorted(categorized.items()):
        results[cat] = []
        for unit in cat_units:
            name = unit["name"]
            meta = unit.get("metadata", {})
            complete = meta.get("complete", False)
            has_target = bool(unit.get("target_path"))
            has_base = bool(unit.get("base_path"))
            source_path = meta.get("source_path", "")

            entry = {
                "name": name,
                "complete": complete,
                "has_source": bool(source_path),
                "source_path": source_path,
            }

            if complete:
                entry["status"] = "complete"
                entry["text_match"] = 100.0
            elif has_target and has_base:
                diff_data, error_message = run_objdiff(name)
                if diff_data:
                    stats = analyze_unit(diff_data)
                    entry.update(stats)
                    entry["status"] = "incomplete"
                else:
                    entry["status"] = "error"
                    entry["error_message"] = error_message
            elif has_target and not has_base:
                entry["status"] = "no_source"
            else:
                entry["status"] = "no_target"

            results[cat].append(entry)

    if args.json_output:
        print(json.dumps(results, indent=2))
        return

    # Pretty print
    grand_total_funcs = 0
    grand_matching_funcs = 0
    grand_total_size = 0
    grand_matching_size = 0
    grand_remaining_size_est = 0
    cat_summaries = {}
    top_unmatched_candidates = []

    for cat, entries in sorted(results.items()):
        print(f"\n=== {cat} ===")

        cat_funcs = 0
        cat_matching = 0
        cat_size = 0
        cat_matching_size = 0

        for entry in entries:
            name = entry["name"]
            # Strip common prefix for display
            display_name = name.replace("main/", "")

            status = entry.get("status", "?")
            if status == "complete":
                # Don't print individual complete units unless --unit specified
                if not args.unit:
                    continue
                print(f"  {display_name:<50s} complete")
            elif status == "incomplete":
                tf = entry.get("total_functions", 0)
                mf = entry.get("matching_functions", 0)
                tm = entry.get("text_match_percent")
                tm_str = f"{tm:.1f}%" if tm is not None else "?"
                remain = entry.get("remaining_code_size_est", 0)
                print(
                    f"  {display_name:<50s} .text {tm_str:>6s}  ~{remain:>6}B rem  ({mf}/{tf} functions)"
                )
                cat_funcs += tf
                cat_matching += mf
                cat_size += entry.get("total_code_size", 0)
                cat_matching_size += entry.get("matching_code_size", 0)
                for candidate in entry.get("top_unmatched_functions", []):
                    top_unmatched_candidates.append(
                        {
                            "unit": name,
                            "display_unit": display_name,
                            "name": candidate["name"],
                            "status": candidate["status"],
                            "size": candidate["size"],
                            "match_percent": candidate["match_percent"],
                            "unmatched_bytes_est": candidate["unmatched_bytes_est"],
                        }
                    )
            elif status == "no_source":
                if args.unit:
                    print(f"  {display_name:<50s} no source file")
            elif status == "error":
                print(f"  {display_name:<50s} error running objdiff")
                if args.unit:
                    error_message = entry.get("error_message")
                    if error_message:
                        for line in error_message.splitlines():
                            print(f"    {line}")

        # Add complete units to totals
        complete_count = sum(1 for e in entries if e.get("status") == "complete")
        if not args.unit and complete_count > 0:
            print(f"  ... {complete_count} complete units")

        # Category summary
        cat_summaries[cat] = {
            "functions": cat_funcs,
            "matching": cat_matching,
            "complete_units": complete_count,
            "total_units": len(entries),
            "remaining_code_size_est": sum(
                e.get("remaining_code_size_est", 0)
                for e in entries
                if e.get("status") == "incomplete"
            ),
        }
        grand_total_funcs += cat_funcs
        grand_matching_funcs += cat_matching
        grand_total_size += cat_size
        grand_matching_size += cat_matching_size
        grand_remaining_size_est += cat_summaries[cat]["remaining_code_size_est"]

    if not args.unit:
        print(f"\n=== Summary ===")
        for cat, s in sorted(cat_summaries.items()):
            total = s["functions"]
            matching = s["matching"]
            complete = s["complete_units"]
            total_units = s["total_units"]
            pct = f"{matching/total*100:.1f}%" if total > 0 else "N/A"
            print(
                f"  {cat:<15s} {pct:>6s} ({matching}/{total} functions)  "
                f"[{complete}/{total_units} units complete, ~{s['remaining_code_size_est']}B rem]"
            )
        if grand_total_funcs > 0:
            grand_pct = grand_matching_funcs / grand_total_funcs * 100
            print(
                f"\n  Total: {grand_pct:.1f}% ({grand_matching_funcs}/{grand_total_funcs} functions, ~{grand_remaining_size_est}B rem)"
            )

    if args.top_unmatched:
        top_unmatched_candidates.sort(
            key=lambda r: (-r["unmatched_bytes_est"], -r["size"], r["name"].lower())
        )
        if args.top_unmatched > 0:
            top_unmatched_candidates = top_unmatched_candidates[: args.top_unmatched]

        print("\n=== Top Unmatched Functions ===")
        if not top_unmatched_candidates:
            print("No unmatched functions found for the given filters.")
        else:
            print(
                f"{'UNMATCH':>8}  {'MATCH':>7}  {'SIZE':>6}  {'UNIT':<34} NAME"
            )
            print("-" * 110)
            for candidate in top_unmatched_candidates:
                match_str = (
                    f"{candidate['match_percent']:.1f}%"
                    if candidate["match_percent"] is not None
                    else "-"
                )
                print(
                    f"{candidate['unmatched_bytes_est']:>7}B  {match_str:>7}  "
                    f"{candidate['size']:>5}B  {candidate['display_unit']:<34} {candidate['name']}"
                )


if __name__ == "__main__":
    main()
