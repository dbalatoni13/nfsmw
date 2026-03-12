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
  python tools/decomp-workflow.py unit -u main/Speed/Indep/SourceLists/zCamera
"""

import argparse
import json
import re
import os
import shlex
import subprocess
import sys
import tempfile
from typing import Any, Dict, List, Optional, Sequence
from _common import (
    BUILD_NINJA,
    OBJDIFF_JSON,
    ROOT_DIR,
    ToolError,
    ensure_exists,
    find_objdiff_unit,
    load_objdiff_config,
    make_abs,
)


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
TOOLS_DIR = os.path.join(ROOT_DIR, "tools")
PS2_TYPES = os.path.join(ROOT_DIR, "symbols", "PS2", "PS2_types.nothpp")
DTK = os.path.join(ROOT_DIR, "build", "tools", "dtk")
OBJDIFF_CLI = os.path.join(ROOT_DIR, "build", "tools", "objdiff-cli")
GC_SYMBOLS = os.path.join(ROOT_DIR, "config", "GOWE69", "symbols.txt")
PS2_SYMBOLS = os.path.join(ROOT_DIR, "config", "SLES-53558-A124", "symbols.txt")
GC_DWARF = os.path.join(ROOT_DIR, "symbols", "Dwarf")
DEBUG_LINES = os.path.join(ROOT_DIR, "symbols", "debug_lines.txt")

DEFAULT_SMOKE_UNIT = "main/Speed/Indep/SourceLists/zCamera"
DEBUG_SYMBOL_PROBE_MANGLED = "UpdateAll__6Cameraf"
DEBUG_SYMBOL_PROBE_DEMANGLED = "Camera::UpdateAll(float)"
DEBUG_SYMBOL_PROBE_GC_ADDR = "0x80065A84"
LOW_MATCH_PRIORITY_THRESHOLD = 60.0
VERY_LOW_MATCH_PRIORITY_THRESHOLD = 40.0
HIGH_MATCH_CLEANUP_THRESHOLD = 85.0
VERY_HIGH_MATCH_CLEANUP_THRESHOLD = 95.0

SHARED_ASSET_REQUIREMENTS = [
    (os.path.join("build", "tools"), "downloaded tooling"),
    (os.path.join("orig", "GOWE69", "NFSMWRELEASE.ELF"), "GameCube original ELF"),
    (os.path.join("orig", "SLES-53558-A124", "NFS.ELF"), "PS2 original ELF"),
    (os.path.join("symbols", "Dwarf"), "DWARF dump"),
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

    print_section("Worktree Health")
    print(f"Root: {ROOT_DIR}")

    def report(ok: bool, label: str, detail: str) -> None:
        nonlocal failures
        status = "OK  " if ok else "FAIL"
        print(f"{status} {label}: {detail}", flush=True)
        if not ok:
            failures += 1

    report(
        os.path.exists(BUILD_NINJA),
        "build.ninja",
        BUILD_NINJA if os.path.exists(BUILD_NINJA) else "missing (run: python configure.py)",
    )
    report(
        os.path.exists(OBJDIFF_JSON),
        "objdiff.json",
        OBJDIFF_JSON if os.path.exists(OBJDIFF_JSON) else "missing (run: python configure.py)",
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
        run_capture(python_tool("decomp-context.py", "--ghidra-check"))
        report(True, "ghidra", "GC + PS2 programs available")
    except WorkflowError as e:
        report(False, "ghidra", str(e))

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

    if args.smoke_build:
        print_section("Build Smoke Test")
        try:
            output_path = build_shared_unit(args.smoke_build)
            report(True, "build", output_path)
        except WorkflowError as e:
            report(False, "build", str(e))

    if args.smoke_dtk:
        print_section("DTK Smoke Test")
        dump_path = None
        try:
            obj_path = build_shared_unit(args.smoke_dtk)
            dump_path = dtk_dwarf_dump(obj_path)
            report(True, "dtk", dump_path)
        except WorkflowError as e:
            report(False, "dtk", str(e))
        finally:
            maybe_remove(dump_path)

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
    run_stream(cmd)


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


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Wrapper for common decomp workflows built on top of the existing project tools."
        )
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    health = subparsers.add_parser(
        "health",
        help="Check whether the current worktree is ready for GC and PS2 decomp work",
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
    function.add_argument("-f", "--function", required=True, help="Function name to inspect")
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
    diff.set_defaults(func=command_diff)

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
