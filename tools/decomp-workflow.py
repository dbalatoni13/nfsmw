#!/usr/bin/env python3

"""
Wrapper for common decomp workflows.

This script keeps the existing tools as the source of truth and orchestrates the
most common agent flows:

  python tools/decomp-workflow.py health
  python tools/decomp-workflow.py health --smoke-build-unit main/Speed/Indep/SourceLists/zCamera
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll
  python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zCamera -f UpdateAll --no-source
  python tools/decomp-workflow.py unit -u main/Speed/Indep/SourceLists/zCamera
"""

import argparse
import os
import subprocess
import sys
from typing import List, Optional, Sequence, Tuple


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
TOOLS_DIR = os.path.join(ROOT_DIR, "tools")

BUILD_NINJA = os.path.join(ROOT_DIR, "build.ninja")
OBJDIFF_JSON = os.path.join(ROOT_DIR, "objdiff.json")
PS2_TYPES = os.path.join(ROOT_DIR, "symbols", "PS2", "PS2_types.nothpp")

DEFAULT_SMOKE_UNIT = "main/Speed/Indep/SourceLists/zCamera"

SHARED_ASSET_REQUIREMENTS = [
    ("NFSMWRELEASE.ELF", "GameCube ELF"),
    ("NFS.ELF", "PS2 ELF"),
    ("NFS.MAP", "PS2 MAP"),
    (os.path.join("build", "tools"), "downloaded tooling"),
    (os.path.join("orig", "GOWE69", "NFSMWRELEASE.ELF"), "GameCube original ELF"),
    (os.path.join("orig", "SLES-53558-A124", "NFS.ELF"), "PS2 original ELF"),
    (os.path.join("symbols", "Dwarf"), "DWARF dump"),
    (os.path.join("symbols", "mw_dwarfdump.nothpp"), "combined dwarf dump"),
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


def ensure_exists(path: str, hint: str) -> None:
    if not os.path.exists(path):
        raise WorkflowError(f"Missing {path}\nHint: {hint}")


def ensure_decomp_prereqs() -> None:
    ensure_exists(
        BUILD_NINJA,
        "Run: python configure.py",
    )
    ensure_exists(
        OBJDIFF_JSON,
        "Run: python configure.py",
    )


def build_temp_obj(unit_name: str) -> str:
    result = run_capture(python_tool("build-unit.py", "-u", unit_name))
    lines = [line.strip() for line in result.stdout.splitlines() if line.strip()]
    if not lines:
        raise WorkflowError(
            "build-unit.py succeeded but did not print an output path to stdout"
        )
    actual = lines[-1]
    if not os.path.exists(actual):
        raise WorkflowError(f"build-unit.py reported a missing output path: {actual}")
    return actual


def maybe_remove(path: Optional[str]) -> None:
    if not path:
        return
    try:
        if os.path.exists(path):
            os.remove(path)
    except OSError as e:
        print(f"Warning: failed to remove temp object {path}: {e}", file=sys.stderr)


def describe_path(path: str) -> str:
    if os.path.islink(path):
        return "shared-symlink"
    return "present"


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

    try:
        run_capture(python_tool("lookup.py", "--file", PS2_TYPES, "struct", "Camera"))
        report(True, "ps2-lookup", "Camera found in PS2 dump")
    except WorkflowError as e:
        report(False, "ps2-lookup", str(e))

    if args.smoke_build_unit:
        print_section("Build Smoke Test")
        temp_obj = None
        try:
            temp_obj = build_temp_obj(args.smoke_build_unit)
            report(True, "build-unit", temp_obj)
        except WorkflowError as e:
            report(False, "build-unit", str(e))
        finally:
            maybe_remove(temp_obj)

    if failures:
        raise WorkflowError(f"Health check failed with {failures} issue(s)")


def resolve_base_obj(
    unit_name: str, base_obj: Optional[str], no_build: bool, keep_temp: bool
) -> Tuple[Optional[str], bool]:
    if base_obj:
        return os.path.abspath(base_obj), False
    if no_build:
        return None, False
    temp_obj = build_temp_obj(unit_name)
    print(f"Using temp object: {temp_obj}", flush=True)
    return temp_obj, not keep_temp


def command_function(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    temp_obj = None
    cleanup = False
    try:
        temp_obj, cleanup = resolve_base_obj(
            args.unit, args.base_obj, args.no_build, args.keep_temp_obj
        )
        print_section(f"Function Workflow: {args.function}")
        cmd = python_tool("decomp-context.py", "-u", args.unit, "-f", args.function)
        if args.no_source:
            cmd.append("--no-source")
        if args.no_ghidra:
            cmd.append("--no-ghidra")
        if temp_obj:
            cmd.extend(["--base-obj", temp_obj])
        run_stream(cmd)
    finally:
        if cleanup:
            maybe_remove(temp_obj)


def command_unit(args: argparse.Namespace) -> None:
    ensure_decomp_prereqs()
    temp_obj = None
    cleanup = False
    try:
        temp_obj, cleanup = resolve_base_obj(
            args.unit, args.base_obj, args.no_build, args.keep_temp_obj
        )

        print_section(f"Unit Status: {args.unit}")
        run_stream(python_tool("decomp-status.py", "--unit", args.unit))

        common_args: List[str] = ["-u", args.unit, "-t", "function"]
        if temp_obj:
            common_args.extend(["--base-obj", temp_obj])

        print_section("Missing Functions")
        run_stream(python_tool("decomp-diff.py", *common_args, "-s", "missing"))

        print_section("Nonmatching Functions")
        run_stream(python_tool("decomp-diff.py", *common_args, "-s", "nonmatching"))
    finally:
        if cleanup:
            maybe_remove(temp_obj)


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
        "--smoke-build-unit",
        metavar="UNIT",
        nargs="?",
        const=DEFAULT_SMOKE_UNIT,
        help=(
            "Also run build-unit.py as a smoke test. If UNIT is omitted, uses "
            f"{DEFAULT_SMOKE_UNIT}"
        ),
    )
    health.set_defaults(func=command_health)

    function = subparsers.add_parser(
        "function",
        help="Build a temp object if needed and run decomp-context.py for one function",
    )
    function.add_argument("-u", "--unit", required=True, help="Translation unit name")
    function.add_argument("-f", "--function", required=True, help="Function name to inspect")
    function.add_argument(
        "--base-obj",
        help="Use an explicit object file instead of building a temp object",
    )
    function.add_argument(
        "--no-build",
        action="store_true",
        help="Do not build a temp object when --base-obj is not provided",
    )
    function.add_argument(
        "--keep-temp-obj",
        action="store_true",
        help="Keep the auto-built temp object instead of deleting it afterwards",
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
    function.set_defaults(func=command_function)

    unit = subparsers.add_parser(
        "unit",
        help="Show a compact unit workflow summary using decomp-status.py and decomp-diff.py",
    )
    unit.add_argument("-u", "--unit", required=True, help="Translation unit name")
    unit.add_argument(
        "--base-obj",
        help="Use an explicit object file instead of building a temp object",
    )
    unit.add_argument(
        "--no-build",
        action="store_true",
        help="Do not build a temp object when --base-obj is not provided",
    )
    unit.add_argument(
        "--keep-temp-obj",
        action="store_true",
        help="Keep the auto-built temp object instead of deleting it afterwards",
    )
    unit.set_defaults(func=command_unit)

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
