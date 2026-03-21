#!/usr/bin/env python3

"""
Wrapper for common decomp workflows.

This script keeps the existing tools as the source of truth and orchestrates the
most common agent flows:

  python tools/decomp-workflow.py health
  python tools/decomp-workflow.py health --smoke-build main/Speed/Indep/SourceLists/zCamera
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll --brief
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll --ghidra-version gc
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll --lookup-mode full
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll --no-lookup
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll --no-source
  python tools/decomp-workflow.py diff -u main/Speed/Indep/SourceLists/zCamera -d UpdateAll --reloc-diffs all
  python tools/decomp-workflow.py dwarf -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll
  python tools/decomp-workflow.py dwarf-scan -u main/Speed/Indep/SourceLists/zCamera
  python tools/decomp-workflow.py dwarf-scan -u main/Speed/Indep/SourceLists/zCamera --objdiff-status match
  python tools/decomp-workflow.py dwarf -u main/Speed/Indep/SourceLists/zAttribSys -f 'Attrib::Class::RemoveCollection(Attrib::Collection *)' --full-diff
  python tools/decomp-workflow.py verify -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll
  python tools/decomp-workflow.py unit -u main/Speed/Indep/SourceLists/zCamera
"""

import argparse
import difflib
import json
import re
import os
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
from typing import Any, Dict, List, Optional, Sequence, Tuple
from _common import (
    BUILD_NINJA,
    OBJDIFF_JSON,
    RELOC_DIFF_CHOICES,
    ROOT_DIR,
    ToolError,
    build_objdiff_symbol_rows,
    ensure_exists,
    find_objdiff_unit,
    load_objdiff_config,
    make_abs,
    run_objdiff_json,
)
from lookup import _candidate_func_names, _sig_contains_name, read_text, split_functions
from split_dwarf_info import apply_umath_fixups


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
TOOLS_DIR = os.path.join(ROOT_DIR, "tools")
PS2_TYPES = os.path.join(ROOT_DIR, "symbols", "PS2", "PS2_types.nothpp")
DTK = os.path.join(ROOT_DIR, "build", "tools", "dtk")
OBJDIFF_CLI = os.path.join(ROOT_DIR, "build", "tools", "objdiff-cli")
GC_SYMBOLS = os.path.join(ROOT_DIR, "config", "GOWE69", "symbols.txt")
PS2_SYMBOLS = os.path.join(ROOT_DIR, "config", "SLES-53558-A124", "symbols.txt")
GC_DWARF = os.path.join(ROOT_DIR, "symbols", "Dwarf")
DEBUG_LINES = os.path.join(ROOT_DIR, "symbols", "debug_lines.txt")
X360_COMPILER_DIR = os.path.join(ROOT_DIR, "build", "compilers", "X360", "14.00.2110")
PS2_COMPILER_DIR = os.path.join(ROOT_DIR, "build", "compilers", "PS2", "ee-gcc2.9-991111")
MIPS_BINUTILS_DIR = os.path.join(ROOT_DIR, "build", "mips_binutils")

DEFAULT_SMOKE_UNIT = "main/Speed/Indep/SourceLists/zCamera"
DEBUG_SYMBOL_PROBE_MANGLED = "UpdateAll__6Cameraf"
DEBUG_SYMBOL_PROBE_DEMANGLED = "Camera::UpdateAll(float)"
DEBUG_SYMBOL_PROBE_GC_ADDR = "0x80065A84"
REBUILT_DEBUG_LINE_RE = re.compile(r"^\s*([0-9A-Fa-f]+)\s*:")
DWARF_HEX_RE = re.compile(r"0x[0-9A-Fa-f]+")
LOW_MATCH_PRIORITY_THRESHOLD = 60.0
VERY_LOW_MATCH_PRIORITY_THRESHOLD = 40.0
HIGH_MATCH_CLEANUP_THRESHOLD = 85.0
VERY_HIGH_MATCH_CLEANUP_THRESHOLD = 95.0
FunctionBlock = Tuple[str, str, str, str]

SHARED_ASSET_REQUIREMENTS = [
    (os.path.join("build", "tools"), "downloaded tooling"),
    (os.path.join("orig", "GOWE69", "NFSMWRELEASE.ELF"), "GameCube original ELF"),
    (
        os.path.join("orig", "EUROPEGERMILESTONE", "NfsMWEuropeGerMilestone.xex"),
        "Xbox original XEX",
    ),
    (os.path.join("orig", "SLES-53558-A124", "NFS.ELF"), "PS2 original ELF"),
    (os.path.join("symbols", "Dwarf"), "DWARF dump"),
]

PLATFORM_BUILD_REQUIREMENTS = [
    (
        "x360-compiler",
        X360_COMPILER_DIR,
        "missing (seed build/compilers in this worktree for Xbox builds)",
    ),
    (
        "ps2-compiler",
        PS2_COMPILER_DIR,
        "missing (seed build/compilers in this worktree for PS2 builds)",
    ),
    (
        "ps2-binutils",
        MIPS_BINUTILS_DIR,
        "missing (seed build/mips_binutils in this worktree for PS2 builds)",
    ),
]


class WorkflowError(RuntimeError):
    pass


def tool_path(name: str) -> str:
    return os.path.join(TOOLS_DIR, name)


def python_tool(name: str, *args: str) -> List[str]:
    return [sys.executable, tool_path(name), *args]


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


def run_capture(cmd: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        cmd,
        cwd=ROOT_DIR,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        raise WorkflowError(
            format_failure(cmd, result.returncode, result.stdout, result.stderr)
        )
    return result


def run_stream(cmd: Sequence[str]) -> None:
    sys.stdout.flush()
    sys.stderr.flush()
    result = subprocess.run(cmd, cwd=ROOT_DIR, text=True)
    if result.returncode != 0:
        raise WorkflowError(format_failure(cmd, result.returncode))


def ensure_decomp_prereqs() -> None:
    try:
        ensure_exists(BUILD_NINJA, "Run: python configure.py")
        ensure_exists(OBJDIFF_JSON, "Run: python configure.py")
    except ToolError as e:
        raise WorkflowError(str(e))


def get_unit_build_target(unit_name: str) -> str:
    config = load_objdiff_config()
    unit = find_objdiff_unit(config, unit_name)
    if unit is None:
        raise WorkflowError(f"Unit not found in objdiff.json: {unit_name}")

    target = unit.get("base_path") or unit.get("target_path")
    if not target:
        raise WorkflowError(f"Unit has no build target in objdiff.json: {unit_name}")
    return str(target)


def get_unit_build_output(unit_name: str) -> str:
    target = get_unit_build_target(unit_name)
    return make_abs(target) or target


def build_shared_unit(unit_name: str, quiet: bool = False) -> str:
    ensure_decomp_prereqs()
    target = get_unit_build_target(unit_name)
    cmd = ["ninja", target]
    if quiet:
        result = subprocess.run(
            cmd,
            cwd=ROOT_DIR,
            text=True,
            capture_output=True,
        )
        if result.returncode != 0:
            raise WorkflowError(
                format_failure(cmd, result.returncode, result.stdout, result.stderr)
            )
    else:
        run_stream(cmd)
    return get_unit_build_output(unit_name)


def ensure_shared_unit_output(unit_name: str) -> str:
    output_path = get_unit_build_output(unit_name)
    if os.path.exists(output_path):
        return output_path

    print(f"Shared build missing for {unit_name}; rebuilding...", flush=True)
    try:
        output_path = build_shared_unit(unit_name, quiet=True)
    except WorkflowError as e:
        raise WorkflowError(
            f"Auto-build failed while preparing shared output for {unit_name}\n{e}"
        )
    print(f"Shared build ready: {output_path}", flush=True)
    return output_path


def maybe_remove(path: Optional[str]) -> None:
    if not path:
        return
    try:
        if os.path.exists(path):
            os.remove(path)
    except OSError as e:
        print(f"Warning: failed to remove temporary file {path}: {e}", file=sys.stderr)


def dtk_dwarf_dump(obj_path: str) -> str:
    fd, output_path = tempfile.mkstemp(prefix="nfsmw_dtk_", suffix=".nothpp")
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
        raise WorkflowError(
            format_failure([DTK, "dwarf", "dump", obj_path, "-o", output_path], result.returncode, result.stdout, result.stderr)
        )

    tool_output = "\n".join(
        part.strip() for part in [result.stdout, result.stderr] if part.strip()
    )
    if "ERROR " in tool_output or tool_output.startswith("ERROR"):
        maybe_remove(output_path)
        raise WorkflowError(f"dtk reported an error while dumping DWARF:\n{tool_output}")

    if not os.path.exists(output_path):
        raise WorkflowError("dtk dwarf dump succeeded but did not write an output file")

    return output_path


def describe_path(path: str) -> str:
    if os.path.islink(path):
        return "shared-symlink"
    return "present"


def fuzzy_match(pattern: str, name: str) -> bool:
    return pattern.lower() in name.lower()


def find_objdiff_rows_for_function(
    unit_name: str, function_name: str, reloc_diffs: str = "none"
) -> List[Dict[str, Any]]:
    data = run_objdiff_json(
        OBJDIFF_CLI,
        unit_name,
        reloc_diffs=reloc_diffs,
        root_dir=ROOT_DIR,
    )
    rows = [
        row
        for row in build_objdiff_symbol_rows(data)
        if row["type"] == "function"
    ]

    exact_matches = [
        row
        for row in rows
        if function_name in row["name"] or function_name in row["symbol_name"]
    ]
    if exact_matches:
        return exact_matches

    return [
        row
        for row in rows
        if fuzzy_match(function_name, row["name"])
        or fuzzy_match(function_name, row["symbol_name"])
    ]


def choose_objdiff_row(unit_name: str, function_name: str, reloc_diffs: str = "none") -> Dict[str, Any]:
    matches = find_objdiff_rows_for_function(unit_name, function_name, reloc_diffs=reloc_diffs)
    if not matches:
        raise WorkflowError(
            f"objdiff: function '{function_name}' not found in {unit_name}.\n"
            "Hint: run `python tools/decomp-workflow.py unit -u "
            f"{unit_name} --search {shlex.quote(function_name)}` to inspect nearby symbols."
        )

    if len(matches) > 1:
        preview = "\n".join(f"  - {row['name']}" for row in matches[:8])
        extra = ""
        if len(matches) > 8:
            extra = f"\n  ... {len(matches) - 8} more"
        raise WorkflowError(
            f"objdiff: function query '{function_name}' matched multiple symbols in {unit_name}.\n"
            f"Use a more specific function name.\n{preview}{extra}"
        )
    return matches[0]


def resolve_exact_function_name(
    unit_name: str, function_name: str, reloc_diffs: str = "none"
) -> str:
    return str(
        choose_objdiff_row(unit_name, function_name, reloc_diffs=reloc_diffs)["name"]
    )


def load_dwarf_report(
    unit_name: str,
    function_name: str,
    rebuilt_dwarf_file: Optional[str] = None,
) -> Dict[str, Any]:
    cmd: List[str] = python_tool("dwarf-compare.py", "-u", unit_name, "-f", function_name, "--json")
    if rebuilt_dwarf_file:
        cmd.extend(["--rebuilt-dwarf-file", rebuilt_dwarf_file])
    result = run_capture(cmd)
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError as e:
        raise WorkflowError(f"dwarf-compare.py returned invalid JSON: {e}")


def load_dwarf_blocks(
    path: str, folder_mode: bool, apply_split_fixups_in_ram: bool = False
) -> List[FunctionBlock]:
    if folder_mode:
        text = read_text(os.path.join(path, "functions.nothpp"))
    else:
        text = read_text(path)
    if apply_split_fixups_in_ram:
        text = apply_umath_fixups(text)
    return split_functions(text)


def find_dwarf_function_blocks(
    funcs: Sequence[FunctionBlock], query: str
) -> List[FunctionBlock]:
    candidates = _candidate_func_names(query)
    exact_matches: List[FunctionBlock] = []
    fuzzy_matches: List[FunctionBlock] = []

    for func in funcs:
        sig_line = func[2]
        if sig_line == query:
            exact_matches.append(func)
        elif any(_sig_contains_name(sig_line, candidate) for candidate in candidates):
            fuzzy_matches.append(func)

    if exact_matches:
        return exact_matches
    return fuzzy_matches


def choose_dwarf_function_block(
    funcs: Sequence[FunctionBlock], query: str, label: str
) -> FunctionBlock:
    matches = find_dwarf_function_blocks(funcs, query)
    if not matches:
        raise WorkflowError(f"{label}: function '{query}' not found.")
    if len(matches) > 1:
        preview = "\n".join(f"  - {match[2]}" for match in matches[:8])
        extra = ""
        if len(matches) > 8:
            extra = f"\n  ... {len(matches) - 8} more"
        raise WorkflowError(
            f"{label}: function query '{query}' matched multiple DWARF blocks.\n"
            f"Use a more specific function name.\n{preview}{extra}"
        )
    return matches[0]


def normalize_dwarf_line(line: str) -> str:
    stripped = line.rstrip("\n").rstrip()
    if stripped.startswith("// Range:"):
        return "// Range: <range>"
    return DWARF_HEX_RE.sub("0xADDR", stripped)


def normalize_dwarf_block(block: str) -> List[str]:
    return [normalize_dwarf_line(line) for line in block.splitlines()]


def count_dwarf_opcodes(
    opcodes: Sequence[Tuple[str, int, int, int, int]]
) -> Dict[str, int]:
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


def build_dwarf_scan_row(
    row: Dict[str, Any],
    original_funcs: Sequence[FunctionBlock],
    rebuilt_funcs: Sequence[FunctionBlock],
) -> Dict[str, Any]:
    function_name = str(row["name"])
    result: Dict[str, Any] = {
        "function": function_name,
        "symbol_name": row["symbol_name"],
        "objdiff_status": row["status"],
        "objdiff_match_percent": row["match_percent"],
        "unmatched_bytes_est": row["unmatched_bytes_est"],
        "size": row["size"],
    }

    try:
        original_block = choose_dwarf_function_block(
            original_funcs, function_name, "original DWARF"
        )
        rebuilt_block = choose_dwarf_function_block(
            rebuilt_funcs, function_name, "rebuilt DWARF"
        )
        original_lines = normalize_dwarf_block(original_block[3])
        rebuilt_lines = normalize_dwarf_block(rebuilt_block[3])
        matcher = difflib.SequenceMatcher(a=original_lines, b=rebuilt_lines)
        counts = count_dwarf_opcodes(matcher.get_opcodes())
        total_lines = max(len(original_lines), len(rebuilt_lines), 1)
        result.update(
            {
                "dwarf_status": "exact"
                if original_lines == rebuilt_lines
                else "mismatch",
                "dwarf_match_percent": 100.0 * counts["matching_lines"] / total_lines,
                "changed_groups": counts["changed_groups"],
                "matching_lines": counts["matching_lines"],
                "total_lines": total_lines,
                "original_line_count": len(original_lines),
                "rebuilt_line_count": len(rebuilt_lines),
                "signature_match": normalize_dwarf_line(original_block[2])
                == normalize_dwarf_line(rebuilt_block[2]),
            }
        )
    except WorkflowError as e:
        result.update(
            {
                "dwarf_status": "error",
                "dwarf_match_percent": None,
                "changed_groups": None,
                "matching_lines": None,
                "total_lines": None,
                "original_line_count": None,
                "rebuilt_line_count": None,
                "signature_match": None,
                "error": str(e),
            }
        )
    return result


def filter_dwarf_scan_rows(
    rows: Sequence[Dict[str, Any]], dwarf_status: str
) -> List[Dict[str, Any]]:
    if dwarf_status == "all":
        return list(rows)
    if dwarf_status == "problem":
        return [row for row in rows if row["dwarf_status"] in ("mismatch", "error")]
    return [row for row in rows if row["dwarf_status"] == dwarf_status]


def filter_dwarf_signature_rows(
    rows: Sequence[Dict[str, Any]], signature_status: str
) -> List[Dict[str, Any]]:
    if signature_status == "all":
        return list(rows)
    want_match = signature_status == "match"
    return [
        row
        for row in rows
        if row.get("signature_match") is not None
        and bool(row["signature_match"]) == want_match
    ]


def sort_dwarf_scan_rows(rows: List[Dict[str, Any]]) -> None:
    status_rank = {"error": 0, "mismatch": 1, "exact": 2}
    rows.sort(
        key=lambda row: (
            status_rank.get(str(row["dwarf_status"]), 3),
            row["dwarf_match_percent"]
            if row["dwarf_match_percent"] is not None
            else -1.0,
            0
            if row.get("signature_match") is True
            else 1
            if row.get("signature_match") is False
            else 2,
            -(row["changed_groups"] or 0),
            -(row["unmatched_bytes_est"] or 0),
            row["objdiff_match_percent"]
            if row["objdiff_match_percent"] is not None
            else -1.0,
            row["function"].lower(),
        )
    )


def command_dwarf_scan(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    if not args.json:
        print_section(f"DWARF Scan: {args.unit}")
    ensure_shared_unit_output(args.unit)

    rebuilt_dwarf_path = (
        os.path.abspath(args.rebuilt_dwarf_file) if args.rebuilt_dwarf_file else None
    )
    cleanup_rebuilt_dwarf = False
    try:
        if not rebuilt_dwarf_path:
            rebuilt_dwarf_path = dtk_dwarf_dump(get_unit_build_output(args.unit))
            cleanup_rebuilt_dwarf = True

        data = run_objdiff_json(
            OBJDIFF_CLI,
            args.unit,
            reloc_diffs=args.reloc_diffs,
            root_dir=ROOT_DIR,
        )
        rows = [
            row
            for row in build_objdiff_symbol_rows(data)
            if row["type"] == "function" and row["side"] == "left"
        ]
        if args.objdiff_status != "all":
            rows = [row for row in rows if row["status"] == args.objdiff_status]
        if args.search:
            rows = [
                row
                for row in rows
                if fuzzy_match(args.search, row["name"])
                or fuzzy_match(args.search, row["symbol_name"])
            ]
        if not rows:
            raise WorkflowError("No functions match the given filters.")

        original_funcs = load_dwarf_blocks(GC_DWARF, folder_mode=True)
        rebuilt_funcs = load_dwarf_blocks(
            rebuilt_dwarf_path, folder_mode=False, apply_split_fixups_in_ram=True
        )
        scan_rows = [
            build_dwarf_scan_row(row, original_funcs, rebuilt_funcs) for row in rows
        ]

        summary = {
            "scanned_functions": len(scan_rows),
            "exact_functions": sum(
                1 for row in scan_rows if row["dwarf_status"] == "exact"
            ),
            "mismatch_functions": sum(
                1 for row in scan_rows if row["dwarf_status"] == "mismatch"
            ),
            "error_functions": sum(
                1 for row in scan_rows if row["dwarf_status"] == "error"
            ),
            "byte_matched_dwarf_problems": sum(
                1
                for row in scan_rows
                if row["objdiff_status"] == "match"
                and row["dwarf_status"] in ("mismatch", "error")
            ),
            "signature_mismatch_functions": sum(
                1 for row in scan_rows if row.get("signature_match") is False
            ),
        }

        filtered_rows = filter_dwarf_scan_rows(scan_rows, args.dwarf_status)
        filtered_rows = filter_dwarf_signature_rows(
            filtered_rows, args.signature_status
        )
        sort_dwarf_scan_rows(filtered_rows)
        if args.limit is not None:
            filtered_rows = filtered_rows[: args.limit]

        if args.json:
            print(
                json.dumps(
                    {
                        "unit": args.unit,
                        "summary": summary,
                        "rows": filtered_rows,
                    },
                    indent=2,
                )
            )
            return

        print(
            f"Scanned {summary['scanned_functions']} function(s): "
            f"{summary['exact_functions']} exact, "
            f"{summary['mismatch_functions']} mismatched, "
            f"{summary['error_functions']} errors."
        )
        print(
            "Byte-matched but DWARF-problem functions: "
            f"{summary['byte_matched_dwarf_problems']}"
        )
        print(
            "Signature-mismatch functions: "
            f"{summary['signature_mismatch_functions']}"
        )

        if not filtered_rows:
            print("No functions match the given filters.")
            return

        print()
        print(
            f"{'DSTAT':<8} {'DWARF':>7} {'SIG':>3} {'CHG':>4} {'OBJ':>7} {'OSTAT':<10} {'UNM':>6} FUNCTION"
        )
        print("-" * 120)
        for row in filtered_rows:
            dwarf_percent = (
                f"{row['dwarf_match_percent']:.1f}%"
                if row["dwarf_match_percent"] is not None
                else "ERR"
            )
            objdiff_percent = (
                f"{row['objdiff_match_percent']:.1f}%"
                if row["objdiff_match_percent"] is not None
                else "-"
            )
            changed_groups = (
                str(row["changed_groups"]) if row["changed_groups"] is not None else "-"
            )
            signature_state = (
                "yes"
                if row.get("signature_match") is True
                else "no"
                if row.get("signature_match") is False
                else "-"
            )
            print(
                f"{row['dwarf_status']:<8} {dwarf_percent:>7} {signature_state:>3} {changed_groups:>4} "
                f"{objdiff_percent:>7} {row['objdiff_status']:<10} "
                f"{row['unmatched_bytes_est']:>5}B {row['function']}"
            )
            if args.show_errors and row.get("error"):
                first_line = str(row["error"]).splitlines()[0]
                print(f"  error: {first_line}")

        print()
        print(
            "Tip: focus matched-byte functions first with "
            "`python tools/decomp-workflow.py dwarf-scan "
            f"-u {shlex.quote(args.unit)} --objdiff-status match`"
        )
        if summary["signature_mismatch_functions"]:
            print(
                "Tip: add `--signature-status match` to focus body/local DWARF mismatches "
                "instead of signature-only trouble."
            )
    finally:
        if cleanup_rebuilt_dwarf:
            maybe_remove(rebuilt_dwarf_path)


def lookup_symbol_address(symbols_file: str, mangled_name: str) -> Optional[str]:
    if not os.path.exists(symbols_file):
        return None

    pattern = re.compile(
        r"^" + re.escape(mangled_name) + r"\s*=\s*(?:\.(\w+):)?0x([0-9A-Fa-f]+)"
    )
    with open(symbols_file) as f:
        for line in f:
            match = pattern.match(line.strip())
            if match:
                return "0x" + match.group(2)
    return None


def command_health(args: argparse.Namespace) -> None:
    failures = 0
    timings: List[Tuple[str, float]] = []
    build_cache: Dict[str, str] = {}

    smoke_build_unit = args.smoke_build or args.full
    smoke_dtk_unit = args.smoke_dtk or args.full

    print_section("Worktree Health")
    print(f"Root: {ROOT_DIR}")

    def report(ok: bool, label: str, detail: str) -> None:
        nonlocal failures
        status = "OK  " if ok else "FAIL"
        print(f"{status} {label}: {detail}", flush=True)
        if not ok:
            failures += 1

    def timed(label: str, func):
        start = time.monotonic()
        try:
            return func()
        finally:
            timings.append((label, time.monotonic() - start))

    def build_shared_unit_cached(unit: str) -> str:
        if unit in build_cache:
            return build_cache[unit]
        output_path = timed(f"build {unit}", lambda: build_shared_unit(unit))
        build_cache[unit] = output_path
        return output_path

    report(
        os.path.exists(BUILD_NINJA),
        "build.ninja",
        BUILD_NINJA
        if os.path.exists(BUILD_NINJA)
        else "missing (run: python tools/share_worktree_assets.py bootstrap)",
    )
    report(
        os.path.exists(OBJDIFF_JSON),
        "objdiff.json",
        OBJDIFF_JSON
        if os.path.exists(OBJDIFF_JSON)
        else "missing (run: python tools/share_worktree_assets.py bootstrap)",
    )

    print_section("Shared Assets")
    for rel_path, label in SHARED_ASSET_REQUIREMENTS:
        abs_path = os.path.join(ROOT_DIR, rel_path)
        report(
            os.path.exists(abs_path),
            label,
            describe_path(abs_path) if os.path.exists(abs_path) else f"missing ({rel_path})",
        )

    print_section("Tool Checks")
    report(
        os.path.exists(OBJDIFF_CLI),
        "objdiff-cli",
        OBJDIFF_CLI if os.path.exists(OBJDIFF_CLI) else "missing (seed build/tools in this worktree)",
    )
    report(
        os.path.exists(DTK),
        "dtk",
        DTK if os.path.exists(DTK) else "missing (seed build/tools in this worktree)",
    )
    try:
        timed("ghidra-check", lambda: run_capture(python_tool("decomp-context.py", "--ghidra-check")))
        report(True, "ghidra", "GC + PS2 programs available")
    except WorkflowError as e:
        report(False, "ghidra", str(e))

    print_section("Platform Build Inputs")
    for label, abs_path, missing_detail in PLATFORM_BUILD_REQUIREMENTS:
        report(
            os.path.exists(abs_path),
            label,
            describe_path(abs_path) if os.path.exists(abs_path) else missing_detail,
        )

    print_section("Debug Symbol Checks")
    try:
        gc_addr = lookup_symbol_address(GC_SYMBOLS, DEBUG_SYMBOL_PROBE_MANGLED)
        report(
            gc_addr == DEBUG_SYMBOL_PROBE_GC_ADDR,
            "gc-symbols",
            gc_addr or f"missing ({DEBUG_SYMBOL_PROBE_MANGLED})",
        )
    except Exception as e:
        report(False, "gc-symbols", str(e))

    try:
        ps2_addr = lookup_symbol_address(PS2_SYMBOLS, DEBUG_SYMBOL_PROBE_MANGLED)
        report(
            ps2_addr is not None,
            "ps2-symbols",
            ps2_addr or f"missing ({DEBUG_SYMBOL_PROBE_MANGLED})",
        )
    except Exception as e:
        report(False, "ps2-symbols", str(e))

    try:
        run_capture(
            python_tool("lookup.py", GC_DWARF, "function", DEBUG_SYMBOL_PROBE_DEMANGLED)
        )
        report(True, "gc-dwarf", f"{DEBUG_SYMBOL_PROBE_DEMANGLED} found")
    except WorkflowError as e:
        report(False, "gc-dwarf", str(e))

    try:
        run_capture(python_tool("lookup.py", "--file", PS2_TYPES, "struct", "Camera"))
        report(True, "ps2-types", "Camera found in PS2 dump")
    except WorkflowError as e:
        report(False, "ps2-types", str(e))

    try:
        result = run_capture(
            python_tool("line_lookup.py", DEBUG_LINES, DEBUG_SYMBOL_PROBE_GC_ADDR)
        )
        ok = "Exact match found" in result.stdout and "Camera.cpp" in result.stdout
        detail = "Camera.cpp exact match" if ok else "unexpected line lookup output"
        report(ok, "debug-lines", detail)
    except WorkflowError as e:
        report(False, "debug-lines", str(e))

    if smoke_build_unit:
        print_section("Build Smoke Test")
        try:
            output_path = build_shared_unit_cached(smoke_build_unit)
            report(True, "build", output_path)
        except WorkflowError as e:
            detail = str(e)
            if "objdiff.json" in detail or "build.ninja" in detail:
                detail += "\nHint: Run: python tools/share_worktree_assets.py bootstrap"
            report(False, "build", detail)

    if smoke_dtk_unit:
        print_section("DTK Smoke Test")
        dump_path = None
        debug_lines_dir = None
        try:
            obj_path = build_shared_unit_cached(smoke_dtk_unit)
            dump_path = timed(f"dtk dump {smoke_dtk_unit}", lambda: dtk_dwarf_dump(obj_path))
            report(True, "dtk", dump_path)
        except WorkflowError as e:
            report(False, "dtk", str(e))
        else:
            try:
                debug_lines_dir = tempfile.mkdtemp(prefix="nfsmw_health_debug_lines_")
                timed(
                    f"debug-line export {smoke_dtk_unit}",
                    lambda: run_capture(
                        python_tool("dwarf1_gcc_line_info.py", obj_path, debug_lines_dir)
                    ),
                )
                rebuilt_debug_lines = os.path.join(debug_lines_dir, "debug_lines.txt")
                if not os.path.exists(rebuilt_debug_lines):
                    raise WorkflowError(
                        "rebuilt debug-line export did not produce debug_lines.txt"
                    )
                first_address = None
                with open(rebuilt_debug_lines) as f:
                    for raw_line in f:
                        match = REBUILT_DEBUG_LINE_RE.match(raw_line)
                        if match is not None:
                            first_address = match.group(1)
                            break
                if first_address is None:
                    raise WorkflowError(
                        "rebuilt debug-line export produced no address entries"
                    )
                result = timed(
                    f"rebuilt line lookup {smoke_dtk_unit}",
                    lambda: run_capture(
                        python_tool("line_lookup.py", rebuilt_debug_lines, first_address)
                    ),
                )
                ok = "Exact match found" in result.stdout
                detail = (
                    f"rebuilt line export ok ({first_address})"
                    if ok
                    else "rebuilt line lookup output did not contain an exact match"
                )
                report(ok, "rebuilt-debug-lines", detail)
            except WorkflowError as e:
                report(False, "rebuilt-debug-lines", str(e))
        finally:
            maybe_remove(dump_path)
            if debug_lines_dir is not None:
                shutil.rmtree(debug_lines_dir, ignore_errors=True)

    if args.timings and timings:
        print_section("Timings")
        for label, elapsed in timings:
            print(f"{elapsed:7.2f}s  {label}")

    if failures:
        raise WorkflowError(f"Health check failed with {failures} issue(s)")


def build_next_candidates(
    status_data: Dict[str, Any], strategy: str
) -> List[Dict[str, Any]]:
    candidates: List[Dict[str, Any]] = []

    for category, entries in status_data.items():
        for entry in entries:
            unit_name = entry.get("name", "")
            display_unit = unit_name.replace("main/", "")
            has_source = bool(entry.get("has_source"))

            for func in entry.get("top_unmatched_functions", []):
                function_name = func.get("name", "?")
                unmatched = int(func.get("unmatched_bytes_est", 0))
                match_percent = func.get("match_percent")
                status = func.get("status", "?")
                size = int(func.get("size", 0))
                is_static_init = function_name.startswith(
                    "__static_initialization_and_destruction_0"
                )
                is_initializer = "InitializeTables" in function_name or is_static_init
                reason = "largest remaining byte win"
                score = float(unmatched)

                if strategy == "balanced":
                    if status == "missing":
                        score *= 1.15
                        reason = "whole implementation still missing; high remaining gain"
                    elif status == "nonmatching":
                        score *= 1.05
                        reason = "large remaining win"

                    if match_percent is not None:
                        if match_percent >= VERY_HIGH_MATCH_CLEANUP_THRESHOLD:
                            score *= 0.2
                            reason = (
                                "near-finished cleanup deprioritized in favor of larger remaining gains"
                            )
                        elif match_percent >= HIGH_MATCH_CLEANUP_THRESHOLD:
                            score *= 0.45
                            reason = (
                                "high-match cleanup deprioritized in favor of larger remaining gains"
                            )
                        elif match_percent <= VERY_LOW_MATCH_PRIORITY_THRESHOLD:
                            score *= 1.25
                            reason = "low match % leaves a large amount of work and upside"
                        elif match_percent <= LOW_MATCH_PRIORITY_THRESHOLD:
                            score *= 1.1
                            reason = "plenty of unmatched work remains here"

                    if has_source:
                        score *= 1.08
                        if "source available" not in reason and "deprioritized" not in reason:
                            reason += " with source available"
                    if is_initializer:
                        score *= 0.3
                        reason = (
                            "large remaining win, but likely lower-priority init/setup work"
                        )
                elif strategy == "quick-wins":
                    score = min(float(unmatched), 1024.0)
                    if status == "missing":
                        score *= 1.05
                        reason = "whole implementation missing; early progress should come quickly"
                    elif status == "nonmatching":
                        score *= 1.1
                        reason = "partial implementation exists, but this is still early-progress work"

                    if match_percent is None:
                        score *= 1.35
                        reason = "0% function; early implementation progress is usually fastest"
                    elif match_percent <= VERY_LOW_MATCH_PRIORITY_THRESHOLD:
                        score *= 1.35
                        reason = "very low match % leaves fast early-progress gains"
                    elif match_percent <= LOW_MATCH_PRIORITY_THRESHOLD:
                        score *= 1.2
                        reason = "low match % usually moves faster than cleanup"
                    elif match_percent >= VERY_HIGH_MATCH_CLEANUP_THRESHOLD:
                        score *= 0.12
                        reason = "near-finished cleanup is slower than fresh early-progress work"
                    elif match_percent >= HIGH_MATCH_CLEANUP_THRESHOLD:
                        score *= 0.35
                        reason = "high-match cleanup deprioritized; quicker gains exist earlier"
                    elif match_percent >= 70.0:
                        score *= 0.75
                        reason = "mid/high-match work is less likely to be a quick win"
                    if has_source:
                        score *= 1.05
                        if "source" not in reason:
                            reason += " with source available"
                    if is_initializer:
                        score *= 0.1
                        reason = (
                            "deprioritized init/setup work; likely not the fastest useful win"
                        )

                candidates.append(
                    {
                        "category": category,
                        "unit": unit_name,
                        "display_unit": display_unit,
                        "function": function_name,
                        "status": status,
                        "size": size,
                        "match_percent": match_percent,
                        "unmatched_bytes_est": unmatched,
                        "score": score,
                        "reason": reason,
                    }
                )

    candidates.sort(
        key=lambda c: (
            -c["score"],
            c["match_percent"] if c["match_percent"] is not None else -1.0,
            -c["unmatched_bytes_est"],
            -c["size"],
            c["function"].lower(),
        )
    )
    return candidates


def command_function(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    print_section(f"Function Workflow: {args.function}")
    ensure_shared_unit_output(args.unit)
    resolved_function_name = resolve_exact_function_name(
        args.unit, args.function, reloc_diffs=args.reloc_diffs
    )
    cmd = python_tool("decomp-context.py", "-u", args.unit, "-f", args.function)
    if args.no_source:
        cmd.append("--no-source")
    if args.no_lookup:
        cmd.append("--no-lookup")
    else:
        cmd.extend(["--lookup-mode", args.lookup_mode])
    if args.no_ghidra:
        cmd.append("--no-ghidra")
    else:
        cmd.extend(["--ghidra-version", args.ghidra_version])
    if args.brief:
        cmd.append("--brief")
    if args.reloc_diffs != "none":
        cmd.extend(["--reloc-diffs", args.reloc_diffs])
    run_stream(cmd)
    print(flush=True)
    print(
        "Required completion check: python tools/decomp-workflow.py verify "
        f"-u {shlex.quote(args.unit)} -f {shlex.quote(resolved_function_name)}",
        flush=True,
    )
    if resolved_function_name != args.function:
        print(
            f"(Resolved exact function name for DWARF-safe follow-up: {resolved_function_name})",
            flush=True,
        )


def command_unit(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    print_section(f"Unit Status: {args.unit}")
    ensure_shared_unit_output(args.unit)
    top_unmatched_limit = args.limit if args.limit is not None else 5
    run_stream(
        python_tool(
            "decomp-status.py",
            "--unit",
            args.unit,
            "--top-unmatched",
            str(top_unmatched_limit),
        )
    )

    common_args: List[str] = ["-u", args.unit, "-t", "function"]
    if args.reloc_diffs != "none":
        common_args.extend(["--reloc-diffs", args.reloc_diffs])
    if args.search:
        common_args.extend(["--search", args.search])
    if args.limit is not None:
        common_args.extend(["--limit", str(args.limit)])
    common_args.extend(["--sort", "unmatched"])

    print_section("Missing Functions")
    run_stream(python_tool("decomp-diff.py", *common_args, "-s", "missing"))

    print_section("Nonmatching Functions")
    run_stream(python_tool("decomp-diff.py", *common_args, "-s", "nonmatching"))


def command_next(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    if args.unit:
        ensure_shared_unit_output(args.unit)

    cmd = python_tool("decomp-status.py", "--json")
    if args.category:
        cmd.extend(["--category", args.category])
    if args.unit:
        cmd.extend(["--unit", args.unit])

    result = run_capture(cmd)
    status_data = json.loads(result.stdout)
    candidates = build_next_candidates(status_data, args.strategy)
    if args.limit is not None:
        candidates = candidates[: args.limit]

    if not candidates:
        if args.unit:
            for entries in status_data.values():
                for entry in entries:
                    if entry.get("name") != args.unit:
                        continue
                    status = entry.get("status")
                    if status == "error":
                        raise WorkflowError(
                            f"Unable to rank {args.unit}: {entry.get('error_message', 'objdiff failed')}"
                        )
                    if status == "complete":
                        raise WorkflowError(f"{args.unit} is already complete.")
                    if status == "no_source":
                        raise WorkflowError(
                            f"{args.unit} has no decomp source configured in objdiff.json."
                        )
                    if status == "no_target":
                        raise WorkflowError(
                            f"{args.unit} has no target object configured in objdiff.json."
                        )
        raise WorkflowError("No unmatched function candidates found for the given filters.")

    if args.command_only:
        for candidate in candidates:
            print(
                "python tools/decomp-workflow.py function "
                f"-u {shlex.quote(candidate['unit'])} "
                f"-f {shlex.quote(candidate['function'])}"
            )
        return

    print_section("Next Targets")
    print(
        f"{'UNMATCH':>8}  {'MATCH':>7}  {'SIZE':>6}  {'UNIT':<34} {'FUNCTION'}"
    )
    print("-" * 120)
    for candidate in candidates:
        match_str = (
            f"{candidate['match_percent']:.1f}%"
            if candidate["match_percent"] is not None
            else "-"
        )
        print(
            f"{candidate['unmatched_bytes_est']:>7}B  {match_str:>7}  {candidate['size']:>5}B  "
            f"{candidate['display_unit']:<34} {candidate['function']}"
        )
        print(f"  why: {candidate['reason']}")
        print(
            "  next: python tools/decomp-workflow.py function "
            f"-u {shlex.quote(candidate['unit'])} "
            f"-f {shlex.quote(candidate['function'])}"
        )


def command_build(args: argparse.Namespace) -> None:
    print(build_shared_unit(args.unit), flush=True)


def command_diff(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    title = f"Diff Workflow: {args.unit}"
    if args.diff:
        title += f" / {args.diff}"
    print_section(title)
    ensure_shared_unit_output(args.unit)

    cmd: List[str] = python_tool("decomp-diff.py", "-u", args.unit)
    if args.reloc_diffs != "none":
        cmd.extend(["--reloc-diffs", args.reloc_diffs])
    if args.diff:
        cmd.extend(["-d", args.diff])
    if args.type:
        cmd.extend(["-t", args.type])
    if args.status:
        cmd.extend(["-s", args.status])
    if args.section:
        cmd.extend(["--section", args.section])
    if args.search:
        cmd.extend(["--search", args.search])
    if args.limit is not None:
        cmd.extend(["--limit", str(args.limit)])
    if args.context is not None:
        cmd.extend(["-C", str(args.context)])
    if args.range:
        cmd.extend(["--range", args.range])
    if args.no_collapse:
        cmd.append("--no-collapse")
    run_stream(cmd)


def command_dwarf(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    print_section(f"DWARF Workflow: {args.unit} / {args.function}")
    if not args.rebuilt_dwarf_file:
        ensure_shared_unit_output(args.unit)
    resolved_function_name = resolve_exact_function_name(args.unit, args.function)

    cmd: List[str] = python_tool(
        "dwarf-compare.py", "-u", args.unit, "-f", resolved_function_name
    )
    if args.summary:
        cmd.append("--summary")
    if args.json:
        cmd.append("--json")
    if args.context is not None:
        cmd.extend(["-C", str(args.context)])
    if args.no_collapse:
        cmd.append("--no-collapse")
    if args.require_exact:
        cmd.append("--require-exact")
    if args.rebuilt_dwarf_file:
        cmd.extend(["--rebuilt-dwarf-file", args.rebuilt_dwarf_file])
    run_stream(cmd)


def command_verify(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    print_section(f"Verify Workflow: {args.unit} / {args.function}")
    ensure_shared_unit_output(args.unit)

    objdiff_row = choose_objdiff_row(args.unit, args.function, reloc_diffs=args.reloc_diffs)
    resolved_function_name = str(objdiff_row["name"])
    dwarf_load_error: Optional[str] = None
    dwarf_report: Optional[Dict[str, Any]] = None
    try:
        dwarf_report = load_dwarf_report(
            args.unit,
            resolved_function_name,
            rebuilt_dwarf_file=args.rebuilt_dwarf_file,
        )
    except WorkflowError as e:
        dwarf_load_error = str(e)

    objdiff_exact = (
        objdiff_row["status"] == "match"
        and objdiff_row["match_percent"] is not None
        and float(objdiff_row["match_percent"]) >= 100.0
    )
    dwarf_exact = bool(dwarf_report["normalized_exact_match"]) if dwarf_report else False
    overall_ok = objdiff_exact and dwarf_exact

    objdiff_percent = (
        f"{float(objdiff_row['match_percent']):.1f}%"
        if objdiff_row["match_percent"] is not None
        else "-"
    )
    dwarf_percent = (
        f"{float(dwarf_report['match_percent']):.1f}%" if dwarf_report else "-"
    )

    print(
        f"objdiff: {'PASS' if objdiff_exact else 'FAIL'} | "
        f"{objdiff_percent} | status={objdiff_row['status']} | "
        f"unmatched~{objdiff_row['unmatched_bytes_est']}B"
    )
    if dwarf_report:
        print(
            f"DWARF:  {'PASS' if dwarf_exact else 'FAIL'} | "
            f"{dwarf_percent} | normalized exact={'yes' if dwarf_exact else 'no'} | "
            f"change groups={dwarf_report['changed_groups']}"
        )
    else:
        print("DWARF:  FAIL | unable to compare rebuilt vs original DWARF", flush=True)
    if resolved_function_name != args.function:
        print(f"Resolved DWARF symbol: {resolved_function_name}")
    print(f"Overall: {'PASS' if overall_ok else 'FAIL'}")
    print("Done means both objdiff and normalized DWARF are exact for the function.")

    if overall_ok:
        return

    if dwarf_load_error:
        print(flush=True)
        print("DWARF compare could not complete:", flush=True)
        print(dwarf_load_error, flush=True)
        if (
            objdiff_row["status"] == "missing"
            and "rebuilt DWARF: function" in dwarf_load_error
            and "not found" in dwarf_load_error
        ):
            print(
                "Hint: the rebuilt object does not contain this function yet. "
                "Implement the function or fix its ownership/signature first, then rerun verify.",
                flush=True,
            )

    print(flush=True)
    print("Follow-up commands:", flush=True)
    print(
        f"  python tools/decomp-workflow.py diff -u {shlex.quote(args.unit)} "
        f"-d {shlex.quote(resolved_function_name)}",
        flush=True,
    )
    print(
        f"  python tools/decomp-workflow.py dwarf -u {shlex.quote(args.unit)} "
        f"-f {shlex.quote(resolved_function_name)}",
        flush=True,
    )
    raise WorkflowError(
        "Verification failed: the function is not complete until both objdiff and DWARF match."
    )


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Wrapper for common decomp workflows built on top of the existing project tools."
        )
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    health = subparsers.add_parser(
        "health",
        help="Check whether the current worktree is ready for GC, Xbox, and PS2 work",
    )
    health.add_argument(
        "--full",
        metavar="UNIT",
        nargs="?",
        const=DEFAULT_SMOKE_UNIT,
        help=(
            "Run the full smoke path for one unit: shared build, dtk dump, rebuilt "
            f"debug-line export, and rebuilt line lookup. If UNIT is omitted, uses {DEFAULT_SMOKE_UNIT}"
        ),
    )
    health.add_argument(
        "--smoke-build",
        metavar="UNIT",
        nargs="?",
        const=DEFAULT_SMOKE_UNIT,
        help=(
            "Also build the unit's shared output as a smoke test. If UNIT is omitted, uses "
            f"{DEFAULT_SMOKE_UNIT}"
        ),
    )
    health.add_argument(
        "--timings",
        action="store_true",
        help="Show wall-clock timings for the heavier health-check steps",
    )
    health.add_argument(
        "--smoke-dtk",
        metavar="UNIT",
        nargs="?",
        const=DEFAULT_SMOKE_UNIT,
        help=(
            "Also run a dtk dwarf dump smoke test. If UNIT is omitted, uses "
            f"{DEFAULT_SMOKE_UNIT}"
        ),
    )
    health.set_defaults(func=command_health)

    function = subparsers.add_parser(
        "function",
        help="Run decomp-context.py for one function",
    )
    function.add_argument("-u", "--unit", required=True, help="Translation unit name")
    function.add_argument(
        "-f",
        "--function",
        required=True,
        help="Function name to inspect (full name or a unique substring)",
    )
    function.add_argument(
        "--no-source",
        action="store_true",
        help="Pass through to decomp-context.py",
    )
    function.add_argument(
        "--no-ghidra",
        action="store_true",
        help="Pass through to decomp-context.py",
    )
    function.add_argument(
        "--ghidra-version",
        choices=["both", "gc", "ps2"],
        default="both",
        help="Pass through to decomp-context.py (default: both)",
    )
    function.add_argument(
        "--no-lookup",
        action="store_true",
        help="Pass through to decomp-context.py",
    )
    function.add_argument(
        "--lookup-mode",
        choices=["signature", "full"],
        default="signature",
        help="Pass through to decomp-context.py (default: signature)",
    )
    function.add_argument(
        "--brief",
        action="store_true",
        help="Trim helper sections like related-source hints and suggested commands",
    )
    function.add_argument(
        "--reloc-diffs",
        choices=RELOC_DIFF_CHOICES,
        default="none",
        help="Pass through objdiff relocation diff mode to decomp-context.py",
    )
    function.set_defaults(func=command_function)

    unit = subparsers.add_parser(
        "unit",
        help="Show a compact unit workflow summary using decomp-status.py and decomp-diff.py",
    )
    unit.add_argument("-u", "--unit", required=True, help="Translation unit name")
    unit.add_argument("--search", help="Fuzzy search on demangled symbol name")
    unit.add_argument(
        "--limit",
        type=int,
        help="Limit each symbol list to the first N matching rows",
    )
    unit.add_argument(
        "--reloc-diffs",
        choices=RELOC_DIFF_CHOICES,
        default="none",
        help="Pass through objdiff relocation diff mode to decomp-diff.py",
    )
    unit.set_defaults(func=command_unit)

    next_cmd = subparsers.add_parser(
        "next",
        help="Recommend the highest-impact next functions to work on",
    )
    next_cmd.add_argument("--category", help="Filter by progress category")
    next_cmd.add_argument("--unit", help="Restrict recommendations to one unit")
    next_cmd.add_argument(
        "--limit",
        type=int,
        default=10,
        help="Limit the number of suggested targets (default: 10)",
    )
    next_cmd.add_argument(
        "--strategy",
        choices=["impact", "balanced", "quick-wins"],
        default="balanced",
        help=(
            "Ranking strategy for recommendations (default: balanced; quick-wins favors "
            "low-match functions where early progress is fastest)"
        ),
    )
    next_cmd.add_argument(
        "--command-only",
        action="store_true",
        help="Print only follow-up commands, one per line",
    )
    next_cmd.set_defaults(func=command_next)

    build = subparsers.add_parser(
        "build",
        help="Build a unit's shared output with its configured ninja target",
    )
    build.add_argument("-u", "--unit", required=True, help="Translation unit name")
    build.set_defaults(func=command_build)

    diff = subparsers.add_parser(
        "diff",
        help="Run decomp-diff.py",
    )
    diff.add_argument("-u", "--unit", required=True, help="Translation unit name")
    diff.add_argument(
        "-d",
        "--diff",
        metavar="SYMBOL",
        help="Show diff for a specific symbol instead of overview mode",
    )
    diff.add_argument("-t", "--type", help="Filter by type: function, object")
    diff.add_argument(
        "-s",
        "--status",
        help="Filter by status: missing, matching, nonmatching, extra",
    )
    diff.add_argument("--section", help="Filter by section name")
    diff.add_argument("--search", help="Fuzzy search on demangled symbol name")
    diff.add_argument(
        "--limit",
        type=int,
        help="Limit overview output to the first N matching rows",
    )
    diff.add_argument(
        "-C",
        "--context",
        type=int,
        default=3,
        help="Context lines around mismatches (default: 3)",
    )
    diff.add_argument("--range", help="Instruction offset range (hex, e.g. 100-200)")
    diff.add_argument(
        "--no-collapse",
        action="store_true",
        help="Don't collapse matching instruction runs",
    )
    diff.add_argument(
        "--reloc-diffs",
        choices=RELOC_DIFF_CHOICES,
        default="none",
        help="Pass through objdiff relocation diff mode to decomp-diff.py",
    )
    diff.set_defaults(func=command_diff)

    dwarf = subparsers.add_parser(
        "dwarf",
        help="Compare original vs rebuilt DWARF for one function",
    )
    dwarf.add_argument("-u", "--unit", required=True, help="Translation unit name")
    dwarf.add_argument(
        "-f",
        "--function",
        required=True,
        help="Function name to compare (full name or a unique substring)",
    )
    dwarf.add_argument(
        "--summary",
        action="store_true",
        help="Print only the DWARF summary without the diff view",
    )
    dwarf.add_argument(
        "--json",
        action="store_true",
        help="Print the DWARF comparison report as JSON",
    )
    dwarf.add_argument(
        "-C",
        "--context",
        type=int,
        default=3,
        help="Context lines around collapsed matching DWARF runs (default: 3)",
    )
    dwarf.add_argument(
        "--no-collapse",
        "--full-diff",
        dest="no_collapse",
        action="store_true",
        help="Show the whole normalized DWARF block with diff markers instead of collapsing matching runs",
    )
    dwarf.add_argument(
        "--rebuilt-dwarf-file",
        help="Use an existing rebuilt DWARF dump instead of dumping the unit object",
    )
    dwarf.add_argument(
        "--require-exact",
        action="store_true",
        help="Exit non-zero unless the normalized DWARF block matches exactly",
    )
    dwarf.set_defaults(func=command_dwarf)

    dwarf_scan = subparsers.add_parser(
        "dwarf-scan",
        help="Scan one translation unit and rank per-function DWARF problem areas",
    )
    dwarf_scan.add_argument("-u", "--unit", required=True, help="Translation unit name")
    dwarf_scan.add_argument(
        "--search",
        help="Only include functions whose name or symbol contains this text",
    )
    dwarf_scan.add_argument(
        "--objdiff-status",
        choices=["all", "match", "nonmatching", "missing"],
        default="all",
        help="Filter functions by objdiff status before scanning (default: all)",
    )
    dwarf_scan.add_argument(
        "--dwarf-status",
        choices=["all", "problem", "exact", "mismatch", "error"],
        default="problem",
        help="Filter scan results by DWARF outcome after scanning (default: problem)",
    )
    dwarf_scan.add_argument(
        "--signature-status",
        choices=["all", "match", "mismatch"],
        default="all",
        help="Filter scan results by whether the DWARF signature already matches (default: all)",
    )
    dwarf_scan.add_argument(
        "--limit",
        type=int,
        default=20,
        help="Maximum rows to print after sorting (default: 20)",
    )
    dwarf_scan.add_argument(
        "--json",
        action="store_true",
        help="Print the scan summary and rows as JSON",
    )
    dwarf_scan.add_argument(
        "--show-errors",
        action="store_true",
        help="Print one-line error details under rows that could not be compared",
    )
    dwarf_scan.add_argument(
        "--reloc-diffs",
        choices=RELOC_DIFF_CHOICES,
        default="none",
        help="Pass through objdiff relocation diff mode when loading unit symbols",
    )
    dwarf_scan.add_argument(
        "--rebuilt-dwarf-file",
        help="Use an existing rebuilt DWARF dump instead of dumping the unit object",
    )
    dwarf_scan.set_defaults(func=command_dwarf_scan)

    verify = subparsers.add_parser(
        "verify",
        help="Fail unless one function matches in both objdiff and DWARF",
    )
    verify.add_argument("-u", "--unit", required=True, help="Translation unit name")
    verify.add_argument(
        "-f",
        "--function",
        required=True,
        help="Function name to verify (full name or a unique substring)",
    )
    verify.add_argument(
        "--reloc-diffs",
        choices=RELOC_DIFF_CHOICES,
        default="none",
        help="Pass through objdiff relocation diff mode when checking instruction match",
    )
    verify.add_argument(
        "--rebuilt-dwarf-file",
        help="Use an existing rebuilt DWARF dump instead of dumping the unit object",
    )
    verify.set_defaults(func=command_verify)

    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()

    try:
        args.func(args)
    except WorkflowError as e:
        print(e, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
