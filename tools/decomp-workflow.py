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
import re
import os
import subprocess
import sys
import tempfile
from typing import List, Optional, Sequence
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
GC_SYMBOLS = os.path.join(ROOT_DIR, "config", "GOWE69", "symbols.txt")
PS2_SYMBOLS = os.path.join(ROOT_DIR, "config", "SLES-53558-A124", "symbols.txt")
GC_DWARF = os.path.join(ROOT_DIR, "symbols", "Dwarf")
DEBUG_LINES = os.path.join(ROOT_DIR, "symbols", "debug_lines.txt")

DEFAULT_SMOKE_UNIT = "main/Speed/Indep/SourceLists/zCamera"
DEBUG_SYMBOL_PROBE_MANGLED = "UpdateAll__6Cameraf"
DEBUG_SYMBOL_PROBE_DEMANGLED = "Camera::UpdateAll(float)"
DEBUG_SYMBOL_PROBE_GC_ADDR = "0x80065A84"

SHARED_ASSET_REQUIREMENTS = [
    ("NFSMWRELEASE.ELF", "GameCube ELF"),
    ("NFS.ELF", "PS2 ELF"),
    ("NFS.MAP", "PS2 MAP"),
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


def build_shared_unit(unit_name: str) -> str:
    ensure_decomp_prereqs()
    target = get_unit_build_target(unit_name)
    run_stream(["ninja", target])
    return get_unit_build_output(unit_name)


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


def command_function(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    print_section(f"Function Workflow: {args.function}")
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
    run_stream(python_tool("decomp-status.py", "--unit", args.unit))

    common_args: List[str] = ["-u", args.unit, "-t", "function"]
    if args.search:
        common_args.extend(["--search", args.search])
    if args.limit is not None:
        common_args.extend(["--limit", str(args.limit)])

    print_section("Missing Functions")
    run_stream(python_tool("decomp-diff.py", *common_args, "-s", "missing"))

    print_section("Nonmatching Functions")
    run_stream(python_tool("decomp-diff.py", *common_args, "-s", "nonmatching"))


def command_build(args: argparse.Namespace) -> None:
    print(build_shared_unit(args.unit), flush=True)


def command_diff(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    title = f"Diff Workflow: {args.unit}"
    if args.diff:
        title += f" / {args.diff}"
    print_section(title)

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
