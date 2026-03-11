#!/usr/bin/env python3

"""
Compile a single translation unit to a temporary (or specified) object file.

Uses `ninja -t compdb` to extract the exact compile command for the unit, then
redirects the output to a private path so parallel agents never overwrite each
other's work.

Usage:
  # Auto-generate a temp path (printed to stdout):
  python tools/build-unit.py -u main/Speed/Indep/SourceLists/zAnim

  # Compile to an explicit path:
  python tools/build-unit.py -u main/Speed/Indep/SourceLists/zAnim -o /tmp/my.o

The path of the compiled .o is always printed to stdout on success so it can be
captured with command substitution:

  TEMPOBJ=$(python tools/build-unit.py -u main/Speed/Indep/SourceLists/zAnim)
  python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -d MyFunc --base-obj "$TEMPOBJ"
  dtk dwarf dump "$TEMPOBJ" -o /tmp/my_dwarf.nothpp
"""

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from typing import Any, Dict, List, Optional, Tuple, Union

from _common import BUILD_NINJA, OBJDIFF_JSON, ROOT_DIR, ToolError, fail, load_objdiff_config

root_dir = ROOT_DIR
COMPILE_COMMANDS = os.path.join(ROOT_DIR, "compile_commands.json")

Command = Union[str, List[str]]


def load_objdiff() -> Dict[str, Any]:
    return load_objdiff_config()


def find_unit_source(config: Dict[str, Any], unit_name: str) -> Optional[str]:
    """Return the source_path for a unit from objdiff.json, or None."""
    for unit in config.get("units", []):
        if unit["name"] == unit_name:
            src = unit.get("metadata", {}).get("source_path")
            return str(src) if src else None
    return None


def find_unit_target(config: Dict[str, Any], unit_name: str) -> Optional[str]:
    """Return the build target path for a unit from objdiff.json, or None."""
    for unit in config.get("units", []):
        if unit["name"] == unit_name:
            target = unit.get("base_path") or unit.get("target_path")
            return str(target) if target else None
    return None


def get_compdb() -> Optional[List[Dict[str, Any]]]:
    """Load compile_commands.json, falling back to `ninja -t compdb` if needed."""
    if os.path.exists(COMPILE_COMMANDS):
        try:
            with open(COMPILE_COMMANDS) as f:
                return json.load(f)
        except json.JSONDecodeError as e:
            print(f"Failed to parse compile_commands.json: {e}", file=sys.stderr)

    result = subprocess.run(
        ["ninja", "-t", "compdb"],
        capture_output=True,
        cwd=root_dir,
    )
    if result.returncode != 0:
        print(
            f"ninja -t compdb failed:\n{result.stderr.decode(errors='replace')}",
            file=sys.stderr,
        )
        return None
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError as e:
        print(f"Failed to parse ninja compdb output: {e}", file=sys.stderr)
        return None


def get_build_command(target_path: str) -> Optional[str]:
    """Return the final ninja command used to build target_path."""
    result = subprocess.run(
        ["ninja", "-t", "commands", target_path],
        capture_output=True,
        cwd=root_dir,
    )
    if result.returncode != 0:
        print(
            f"ninja -t commands failed:\n{result.stderr.decode(errors='replace')}",
            file=sys.stderr,
        )
        return None

    commands = [line.strip() for line in result.stdout.decode().splitlines() if line.strip()]
    return commands[-1] if commands else None


def find_entry(
    compdb: List[Dict[str, Any]], source_path: str
) -> Optional[Dict[str, Any]]:
    """Find the compdb entry whose 'file' matches source_path.

    Prefers entries whose output is a .o file (actual compiler invocations)
    over auxiliary entries (e.g. hash generation).
    """
    abs_source = os.path.normcase(os.path.abspath(os.path.join(root_dir, source_path)))
    candidates = []
    for entry in compdb:
        file_val = entry.get("file", "")
        if not os.path.isabs(file_val):
            entry_dir = entry.get("directory", root_dir)
            file_val = os.path.abspath(os.path.join(entry_dir, file_val))
        if os.path.normcase(file_val) == abs_source:
            candidates.append(entry)
    for entry in candidates:
        out = entry.get("output", "")
        if out.endswith(".o") or out.endswith(".obj"):
            return entry
    return candidates[0] if candidates else None


def get_command(entry: Dict[str, Any]) -> Command:
    command = entry.get("command")
    if isinstance(command, str):
        return command

    arguments = entry.get("arguments")
    if isinstance(arguments, list):
        return arguments[:]

    print(
        "Compilation entry is missing both 'command' and 'arguments'",
        file=sys.stderr,
    )
    sys.exit(1)


def strip_transform_dep(command: Command) -> Command:
    """Remove the `&& python transform_dep.py ...` step from a compile command.

    The dependency file transformation is only needed for incremental ninja
    builds; it is safe to skip for one-off temp compilations.
    """
    if isinstance(command, list):
        return command
    return re.sub(
        r"\s*&&\s*\S*python3?\S*\s+\S*transform_dep\.py\s+\S+\s+\S+",
        "",
        command,
    )


def find_output_argument(command: Command) -> Optional[Tuple[int, str]]:
    if isinstance(command, list):
        for i in range(len(command) - 1):
            if command[i] == "-o":
                return i + 1, command[i + 1]
        return None

    m = re.search(r"(?<!\S)-o\s+(\S+)", command)
    if not m:
        return None
    return m.start(1), m.group(1)


def redirect_output(command: Command, source_path: str, new_output: str) -> Command:
    """Replace the compiler output path in command with new_output.

    Handles two styles:
      - Direct file output  (-o path/to/file.o):   ngccc/ProDG, MSVC, EE-GCC
      - Directory output   (-o path/to/dir):       mwcc (MWCC outputs to a dir)
    """
    output_arg = find_output_argument(command)
    if output_arg is None:
        print("Could not find -o argument in compile command", file=sys.stderr)
        sys.exit(1)

    index, o_arg = output_arg

    if o_arg.endswith(".o") or o_arg.endswith(".obj"):
        replacement = new_output
    else:
        replacement = os.path.dirname(new_output)

    if isinstance(command, list):
        new_command = command[:]
        new_command[index] = replacement
        return new_command

    return command[:index] + replacement + command[index + len(o_arg) :]


def actual_output_path(command: Command, source_path: str, new_output: str) -> str:
    """Return the path where the compiled .o actually lands.

    For direct-file compilers this is new_output.  For directory-output
    compilers it is <new_output_dir>/<source_stem>.o.
    """
    output_arg = find_output_argument(command)
    if output_arg is None:
        return new_output
    _, o_arg = output_arg
    if o_arg.endswith(".o") or o_arg.endswith(".obj"):
        return new_output
    stem = os.path.splitext(os.path.basename(source_path))[0]
    return os.path.join(os.path.dirname(new_output), stem + ".o")


def compile_unit(unit_name: str, output_path: str) -> str:
    """Compile unit to output_path and return the actual .o path."""
    config = load_objdiff()
    source_path = find_unit_source(config, unit_name)
    target_path = find_unit_target(config, unit_name)
    if not source_path:
        fail(
            f"No source_path found for unit '{unit_name}' in objdiff.json.\n"
            "The unit may not have a source file yet (missing implementation)."
        )
    if not target_path:
        fail(f"No target_path found for unit '{unit_name}' in objdiff.json.")
    if not os.path.exists(BUILD_NINJA):
        fail(f"Missing {BUILD_NINJA}\nHint: Run: python configure.py")

    command = get_build_command(target_path)
    if command is None:
        fail(
            f"No build command found for target '{target_path}'.\n"
            "Make sure the unit exists and `python configure.py` has been run."
        )

    # 1. Strip the dependency-file transform step — not needed for temp builds.
    command = strip_transform_dep(command)

    # 2. Determine the actual output path before modifying the command.
    actual = actual_output_path(command, source_path, output_path)

    # 3. Ensure the output directory exists.
    out_dir = os.path.dirname(actual)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    # 4. Redirect the compiler output.
    command = redirect_output(command, source_path, output_path)

    # 5. Run the compile.
    result = subprocess.run(command, shell=isinstance(command, str), cwd=root_dir)
    if result.returncode != 0:
        fail(f"Compilation failed (exit code {result.returncode})")

    return actual


def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Compile a translation unit to a temporary or specified .o file. "
            "Safe to run in parallel — each call produces an independent output."
        )
    )
    parser.add_argument(
        "-u",
        "--unit",
        required=True,
        help="Unit name (e.g. main/Speed/Indep/SourceLists/zAnim)",
    )
    parser.add_argument(
        "-o",
        "--output",
        help="Explicit output .o path (default: auto-generated temp file)",
    )
    args = parser.parse_args()

    if args.output:
        output_path = os.path.abspath(args.output)
    else:
        unit_stem = os.path.basename(args.unit)
        fd, output_path = tempfile.mkstemp(
            prefix=f"nfsmw_{unit_stem}_",
            suffix=".o",
        )
        os.close(fd)

    try:
        actual = compile_unit(args.unit, output_path)
    except ToolError as e:
        fail(str(e))
    print(actual)


if __name__ == "__main__":
    main()
