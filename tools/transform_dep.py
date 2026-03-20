#!/usr/bin/env python3

###
# Transforms .d files, converting Windows paths to Unix paths.
# Allows usage of the mwcc -MMD flag on platforms other than Windows.
#
# Usage:
#   python3 tools/transform_dep.py build/src/file.d build/src/file.d
#
# If changes are made, please submit a PR to
# https://github.com/encounter/dtk-template
###

import argparse
import os
import re
from platform import uname

wineprefix = os.path.join(os.environ["HOME"], ".wine")
if "WINEPREFIX" in os.environ:
    wineprefix = os.environ["WINEPREFIX"]
winedevices = os.path.join(wineprefix, "dosdevices")


def in_wsl() -> bool:
    return "microsoft-standard" in uname().release


def normalize_path(path: str, build_root: str) -> str:
    path = path.replace("\\", "/")
    if re.match(r"^[A-Za-z]:", path):
        # lowercase drive letter for Windows-style absolute paths
        path = path[0].lower() + path[1:]
        if path[0] == "z":
            # shortcut for z:
            path = path[2:]
        elif in_wsl():
            path = path[0:1] + path[2:]
            path = os.path.join("/mnt", path)
        else:
            # use $WINEPREFIX/dosdevices to resolve path
            path = os.path.realpath(os.path.join(winedevices, path))
    elif os.path.isabs(path):
        path = os.path.realpath(path)
    else:
        # ProDG and ee-gcc often emit repo-relative includes like src/Foo.h.
        # Keep them anchored to the current build root instead of incorrectly
        # treating them as Wine drive roots.
        path = os.path.realpath(os.path.join(build_root, path))
    return path


def format_depfile(target: str, deps: list[str]) -> str:
    if not deps:
        return f"{target}:\n"

    out_text = f"{target}: {deps[0]}"
    if len(deps) == 1:
        return out_text + "\n"

    out_text += " \\\n"
    for idx, dep in enumerate(deps[1:], 1):
        suffix = " \\\n" if idx != len(deps) - 1 else "\n"
        out_text += f"\t{dep}{suffix}"
    return out_text


def import_d_file(in_file: str, expected_target: str | None = None) -> str:
    build_root = os.getcwd()
    first_target = None
    first_deps: list[str] = []
    current_target = None

    if not os.path.exists(in_file):
        return ""

    with open(in_file) as file:
        for raw_line in file:
            line = raw_line.rstrip("\n")
            if not line:
                continue

            if not raw_line[0].isspace():
                target, _, dep_text = line.partition(":")
                if not _:
                    continue

                current_target = target.strip()
                if first_target is None:
                    first_target = current_target
                    if dep_text.strip():
                        for dep in dep_text.replace("\\", " ").split():
                            first_deps.append(normalize_path(dep, build_root))
                continue

            if current_target != first_target:
                continue

            dep_text = line.strip()
            if dep_text.endswith(" \\"):
                dep_text = dep_text[:-2]
            if not dep_text:
                continue
            first_deps.append(normalize_path(dep_text, build_root))

    if first_target is None:
        raise ValueError(f"No dependency target found in {in_file}")

    target = normalize_path(expected_target, build_root) if expected_target else normalize_path(first_target, build_root)
    return format_depfile(target, first_deps)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="""Transform a .d file from Wine paths to normal paths"""
    )
    parser.add_argument(
        "d_file",
        help="""Dependency file in""",
    )
    parser.add_argument(
        "d_file_out",
        help="""Dependency file out""",
    )
    parser.add_argument(
        "target",
        nargs="?",
        default=None,
        help="""Expected target path for the primary output""",
    )
    args = parser.parse_args()

    output = import_d_file(args.d_file, args.target)

    if not output:
        return

    with open(args.d_file_out, "w", encoding="UTF-8") as f:
        f.write(output)


if __name__ == "__main__":
    main()
