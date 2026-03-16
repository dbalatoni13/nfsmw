#!/usr/bin/env python3
"""
Batch-run DWARF and objdiff percentages across all functions in a TU.

This optimized version avoids per-function subprocess overhead:
- objdiff JSON is loaded once for the whole unit
- rebuilt DWARF is dumped once via dtk
- original/rebuilt function blocks are parsed once, then reused in-process

Usage (compatible with old form):
  python3 tools/dwarf_batch.py <unit> [out_csv]

Optional flags:
  --unit/-u <unit>
  --output/-o <out_csv>
  --limit <N>           (for quick smoke tests)
  --sort name|objdiff   (default: objdiff order)
"""

import argparse
import csv
import difflib
import importlib.util
import os
import subprocess
import sys
import tempfile
from typing import Any, Dict, List, Sequence, Tuple

from _common import (
    ROOT_DIR,
    build_objdiff_symbol_rows,
    find_objdiff_unit,
    load_objdiff_config,
    make_abs,
    run_objdiff_json,
)


OBJDIFF_CLI = os.path.join(ROOT_DIR, "build", "tools", "objdiff-cli")
DTK = os.path.join(ROOT_DIR, "build", "tools", "dtk")
GC_DWARF_DIR = os.path.join(ROOT_DIR, "symbols", "Dwarf")


def load_dwarf_compare_module():
    path = os.path.join(ROOT_DIR, "tools", "dwarf-compare.py")
    spec = importlib.util.spec_from_file_location("dwarf_compare_mod", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Failed to load module spec: {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def get_unit_build_output(unit_name: str) -> str:
    config = load_objdiff_config()
    unit = find_objdiff_unit(config, unit_name)
    if unit is None:
        raise RuntimeError(f"Unit not found in objdiff.json: {unit_name}")
    target = unit.get("base_path") or unit.get("target_path")
    if not target:
        raise RuntimeError(f"Unit has no build target in objdiff.json: {unit_name}")
    return make_abs(str(target)) or str(target)


def dtk_dwarf_dump_once(obj_path: str) -> str:
    fd, output_path = tempfile.mkstemp(prefix="nfsmw_batch_dwarf_", suffix=".nothpp")
    os.close(fd)
    os.remove(output_path)

    result = subprocess.run(
        [DTK, "dwarf", "dump", obj_path, "-o", output_path],
        cwd=ROOT_DIR,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        raise RuntimeError(
            "dtk dwarf dump failed\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}"
        )
    return output_path


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


def dwarf_match_percent(dwarf_mod: Any, original_block: Any, rebuilt_block: Any) -> float:
    original_lines = dwarf_mod.normalize_block(original_block[3])
    rebuilt_lines = dwarf_mod.normalize_block(rebuilt_block[3])
    matcher = difflib.SequenceMatcher(a=original_lines, b=rebuilt_lines)
    counts = count_lines_for_opcodes(matcher.get_opcodes())
    total_lines = max(len(original_lines), len(rebuilt_lines), 1)
    return 100.0 * counts["matching_lines"] / total_lines


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Batch DWARF+objdiff percent CSV for one TU")
    parser.add_argument("unit_pos", nargs="?", help="Translation unit name")
    parser.add_argument("output_pos", nargs="?", help="Output CSV path")
    parser.add_argument("-u", "--unit", help="Translation unit name")
    parser.add_argument("-o", "--output", help="Output CSV path")
    parser.add_argument("--limit", type=int, default=None, help="Process first N functions only")
    parser.add_argument(
        "--sort",
        choices=["objdiff", "name"],
        default="objdiff",
        help="Row order in CSV (default: objdiff)",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    unit = args.unit or args.unit_pos or "main/Speed/Indep/SourceLists/zEAXSound"
    outcsv = args.output or args.output_pos or f"build/{unit.replace('/', '_')}_function_stats.csv"

    os.makedirs(os.path.dirname(outcsv) or ".", exist_ok=True)

    data = run_objdiff_json(OBJDIFF_CLI, unit, root_dir=ROOT_DIR)
    rows = [r for r in build_objdiff_symbol_rows(data) if r.get("type") == "function"]
    if args.sort == "name":
        rows.sort(key=lambda r: r.get("name", "").lower())
    if args.limit is not None:
        rows = rows[: args.limit]
    if not rows:
        print(f"No functions found for unit {unit}", file=sys.stderr)
        sys.exit(1)

    obj_path = get_unit_build_output(unit)
    if not os.path.exists(obj_path):
        print(
            f"Missing built object for {unit}: {obj_path}\n"
            f"Hint: python3 tools/decomp-workflow.py build -u {unit}",
            file=sys.stderr,
        )
        sys.exit(1)

    dwarf_mod = load_dwarf_compare_module()
    rebuilt_dwarf = dtk_dwarf_dump_once(obj_path)
    try:
        original_funcs = dwarf_mod.load_function_blocks(GC_DWARF_DIR, folder_mode=True)
        rebuilt_funcs = dwarf_mod.load_function_blocks(
            rebuilt_dwarf,
            folder_mode=False,
            apply_split_fixups_in_ram=True,
        )

        with open(outcsv, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["function", "objdiff_percent", "dwarf_percent"])
            f.flush()
            os.fsync(f.fileno())

            total = len(rows)
            for i, row in enumerate(rows, 1):
                func_name = row.get("name") or row.get("symbol_name") or ""
                objdiff_mp = row.get("match_percent")
                objdiff_str = f"{float(objdiff_mp):.1f}" if objdiff_mp is not None else ""

                dwarf_str = ""
                try:
                    original_block = dwarf_mod.choose_function_block(
                        original_funcs, func_name, "original DWARF"
                    )
                    rebuilt_block = dwarf_mod.choose_function_block(
                        rebuilt_funcs, func_name, "rebuilt DWARF"
                    )
                    dwarf_pct = dwarf_match_percent(dwarf_mod, original_block, rebuilt_block)
                    dwarf_str = f"{dwarf_pct:.1f}"
                except Exception:
                    dwarf_str = ""

                writer.writerow([func_name, objdiff_str, dwarf_str])
                # Stream progress to disk so long runs are observable.
                f.flush()
                os.fsync(f.fileno())
                print(f"[{i}/{total}] {func_name}", file=sys.stderr)
                sys.stderr.flush()
    finally:
        if os.path.exists(rebuilt_dwarf):
            os.remove(rebuilt_dwarf)

    print(outcsv)


if __name__ == "__main__":
    main()
