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
from typing import Any, Dict, List, Optional

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))
OBJDIFF_JSON = os.path.join(root_dir, "objdiff.json")
BUILD_NINJA = os.path.join(root_dir, "build.ninja")


def load_objdiff() -> Dict[str, Any]:
    with open(OBJDIFF_JSON) as f:
        return json.load(f)


def find_unit_source(config: Dict[str, Any], unit_name: str) -> Optional[str]:
    """Return the source_path for a unit from objdiff.json, or None."""
    for unit in config.get("units", []):
        if unit["name"] == unit_name:
            src = unit.get("metadata", {}).get("source_path")
            return str(src) if src else None
    return None


def get_compdb() -> Optional[List[Dict[str, Any]]]:
    """Run `ninja -t compdb` and return the parsed compilation database."""
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


def find_entry(
    compdb: List[Dict[str, Any]], source_path: str
) -> Optional[Dict[str, Any]]:
    """Find the compdb entry whose 'file' matches source_path.

    When multiple entries exist for the same source file (e.g. hasher,
    prodg compile, decompctx), prefer the one whose output ends with
    '.o' or '.obj' — that is the actual compiler invocation.
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
    if not candidates:
        return None
    # Prefer the entry that produces an object file
    for entry in candidates:
        out = entry.get("output", "")
        if out.endswith(".o") or out.endswith(".obj"):
            return entry
    return candidates[0]


def strip_transform_dep(command: str) -> str:
    """Remove the `&& python transform_dep.py ...` step from a compile command.

    The dependency file transformation is only needed for incremental ninja
    builds; it is safe to skip for one-off temp compilations.
    """
    return re.sub(
        r"\s*&&\s*\S*python3?\S*\s+\S*transform_dep\.py\s+\S+\s+\S+",
        "",
        command,
    )


def redirect_output(command: str, source_path: str, new_output: str) -> str:
    """Replace the compiler output path in command with new_output.

    Handles two styles:
      - Direct file output  (-o path/to/file.o):   ngccc/ProDG, MSVC, EE-GCC
      - Directory output   (-o path/to/dir):       mwcc (MWCC outputs to a dir)
    """
    m = re.search(r"(?<!\S)-o\s+(\S+)", command)
    if not m:
        print("Could not find -o argument in compile command", file=sys.stderr)
        sys.exit(1)

    o_arg = m.group(1)

    if o_arg.endswith(".o") or o_arg.endswith(".obj"):
        # Direct-file compilers (ngccc, ee-gcc, MSVC): simply replace the path.
        return command[: m.start(1)] + new_output + command[m.end(1) :]
    else:
        # Directory-output compilers (mwcc): replace the basedir.
        # The compiler will create <basedir>/<source_stem>.o automatically.
        new_basedir = os.path.dirname(new_output)
        return command[: m.start(1)] + new_basedir + command[m.end(1) :]


def actual_output_path(command: str, source_path: str, new_output: str) -> str:
    """Return the path where the compiled .o actually lands.

    For direct-file compilers this is new_output.  For directory-output
    compilers it is <new_output_dir>/<source_stem>.o.
    """
    m = re.search(r"(?<!\S)-o\s+(\S+)", command)
    if not m:
        return new_output
    o_arg = m.group(1)
    if o_arg.endswith(".o") or o_arg.endswith(".obj"):
        return new_output
    # mwcc directory case
    stem = os.path.splitext(os.path.basename(source_path))[0]
    return os.path.join(os.path.dirname(new_output), stem + ".o")


def compile_unit(unit_name: str, output_path: str) -> str:
    """Compile unit to output_path and return the actual .o path."""
    if not os.path.exists(OBJDIFF_JSON):
        print(
            "objdiff.json not found. Run:  python configure.py && ninja all_source",
            file=sys.stderr,
        )
        sys.exit(1)

    config = load_objdiff()
    source_path = find_unit_source(config, unit_name)
    if not source_path:
        print(
            f"No source_path found for unit '{unit_name}' in objdiff.json.\n"
            "The unit may not have a source file yet (missing implementation).",
            file=sys.stderr,
        )
        sys.exit(1)

    if not os.path.exists(BUILD_NINJA):
        print(
            "build.ninja not found. Run:  python configure.py && ninja all_source",
            file=sys.stderr,
        )
        sys.exit(1)

    compdb = get_compdb()
    if compdb is None:
        sys.exit(1)

    entry = find_entry(compdb, source_path)
    if entry is None:
        print(
            f"No compilation entry found for '{source_path}'.\n"
            "Make sure the source file exists and `ninja all_source` has been run.",
            file=sys.stderr,
        )
        sys.exit(1)

    command = entry["command"]

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
    result = subprocess.run(command, shell=True, cwd=root_dir)
    if result.returncode != 0:
        print(
            f"Compilation failed (exit code {result.returncode})", file=sys.stderr
        )
        sys.exit(1)

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

    actual = compile_unit(args.unit, output_path)
    print(actual)


if __name__ == "__main__":
    main()
