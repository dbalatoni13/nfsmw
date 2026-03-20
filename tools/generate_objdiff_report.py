#!/usr/bin/env python3

import argparse
import json
from pathlib import Path
import shlex
import subprocess
import sys


def build_unavailable_report(missing_targets):
    return {
        "version": "1.0",
        "status": "unavailable",
        "reason": "Original reference objects are not available for objdiff report generation.",
        "measures": {
            "fuzzy_match_percent": 0.0,
            "total_code": "0",
            "matched_code": "0",
            "matched_code_percent": 0.0,
            "total_data": "0",
            "matched_data": "0",
            "matched_data_percent": 0.0,
            "total_functions": 0,
            "matched_functions": 0,
            "matched_functions_percent": 0.0,
            "total_units": 0,
        },
        "categories": [],
        "units": [],
        "metadata": {
            "missing_target_count": len(missing_targets),
            "missing_target_examples": missing_targets[:10],
        },
    }


def main():
    parser = argparse.ArgumentParser(
        description="Generate an objdiff report or a marked fallback when targets are unavailable."
    )
    parser.add_argument("--objdiff", required=True, help="Path to objdiff-cli.")
    parser.add_argument(
        "--objdiff-json",
        required=True,
        help="Path to objdiff.json for the current configured version.",
    )
    parser.add_argument("--out", required=True, help="Output report.json path.")
    parser.add_argument(
        "--report-args",
        default="",
        help="Extra arguments forwarded to `objdiff-cli report generate`.",
    )
    parser.add_argument(
        "--allow-missing-targets",
        action="store_true",
        help="Write an unavailable fallback report instead of failing when target objects are missing.",
    )
    args = parser.parse_args()

    objdiff_json = Path(args.objdiff_json)
    out_path = Path(args.out)

    with open(objdiff_json, "r", encoding="utf-8") as f:
        objdiff_config = json.load(f)

    missing_targets = []
    for unit in objdiff_config.get("units", []):
        target_path = unit.get("target_path")
        if target_path and not Path(target_path).is_file():
            missing_targets.append(target_path)

    if missing_targets:
        if not args.allow_missing_targets:
            print(
                "Missing target objects required for objdiff report generation:",
                file=sys.stderr,
            )
            for target_path in missing_targets[:10]:
                print(f"  {target_path}", file=sys.stderr)
            if len(missing_targets) > 10:
                print(
                    f"  ... and {len(missing_targets) - 10} more",
                    file=sys.stderr,
                )
            sys.exit(1)

        out_path.parent.mkdir(parents=True, exist_ok=True)
        with open(out_path, "w", encoding="utf-8") as f:
            json.dump(build_unavailable_report(missing_targets), f, indent=2)
            f.write("\n")

        print(
            f"Wrote fallback report to {out_path} because {len(missing_targets)} original target objects are unavailable.",
            file=sys.stderr,
        )
        return

    command = [
        args.objdiff,
        "report",
        "generate",
        *shlex.split(args.report_args),
        "-o",
        args.out,
    ]
    result = subprocess.run(command)
    sys.exit(result.returncode)


if __name__ == "__main__":
    main()
