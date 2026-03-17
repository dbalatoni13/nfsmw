#!/usr/bin/env python3
"""
find-symbol.py

Search src/ for existing definitions of a class, struct, enum, global variable,
or typedef. Use this before declaring any new symbol to avoid redeclaring something
that already exists in the project.

This is the CLI alternative to clangd workspace/symbol search.

Usage:
  python tools/find-symbol.py AITarget
  python tools/find-symbol.py CEntity --type class
  python tools/find-symbol.py EState --type enum
  python tools/find-symbol.py TheAnimCandidateData --type global
  python tools/find-symbol.py HHANDLER --type typedef
  python tools/find-symbol.py --all AIGoal       # show all match types
"""

import argparse
import os
import re
import sys
from typing import List, Optional, Tuple

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))
SRC_DIR = os.path.join(root_dir, "src")

# Extensions to search
SOURCE_EXTS = {".h", ".hpp", ".cpp", ".cc", ".c", ".hh"}

# -------------------------------------------------------------------
# Pattern builders
# -------------------------------------------------------------------


def _word(name: str) -> str:
    """Return a regex that matches `name` as a whole word."""
    return r"(?<![A-Za-z0-9_])" + re.escape(name) + r"(?![A-Za-z0-9_])"


PATTERNS = {
    "struct": re.compile(
        r"^\s*(?:template\s*<[^>]*>\s*)?struct\s+({name})(?:\s|:|\{{|;)"
    ),
    "class": re.compile(
        r"^\s*(?:template\s*<[^>]*>\s*)?class\s+({name})(?:\s|:|\{{|;)"
    ),
    "enum": re.compile(r"^\s*enum\s+(?:class\s+)?({name})(?:\s|:|\{{|;)"),
    "typedef": re.compile(r"\btypedef\b.*\b({name})\s*;"),
    "global": re.compile(
        r"^(?:extern\s+)?(?:const\s+)?[\w:<>*&\s]+?\b({name})\s*(?:;|=|\[)"
    ),
}


def make_pattern(kind: str, name: str) -> re.Pattern:
    return re.compile(PATTERNS[kind].pattern.replace("{name}", re.escape(name)))


# -------------------------------------------------------------------
# Search
# -------------------------------------------------------------------

Match = Tuple[str, int, str, str]  # (filepath, lineno, kind, line_text)


def search_file(filepath: str, name: str, kinds: List[str]) -> List[Match]:
    results: List[Match] = []
    compiled = {k: make_pattern(k, name) for k in kinds}
    try:
        with open(filepath, encoding="utf-8", errors="ignore") as f:
            for lineno, line in enumerate(f, 1):
                for kind, pat in compiled.items():
                    if pat.search(line):
                        results.append((filepath, lineno, kind, line.rstrip()))
                        break  # only report the first kind match per line
    except OSError:
        pass
    return results


def search_tree(name: str, kinds: List[str]) -> List[Match]:
    results: List[Match] = []
    for dirpath, _dirs, files in os.walk(SRC_DIR):
        for fname in files:
            if os.path.splitext(fname)[1] in SOURCE_EXTS:
                fpath = os.path.join(dirpath, fname)
                results.extend(search_file(fpath, name, kinds))
    return results


# -------------------------------------------------------------------
# Main
# -------------------------------------------------------------------

ALL_KINDS = ["struct", "class", "enum", "typedef", "global"]


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Search src/ for existing symbol definitions",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("name", help="Symbol name to search for")
    parser.add_argument(
        "--type",
        dest="kind",
        choices=ALL_KINDS,
        help="Restrict search to a specific symbol kind",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Search for all symbol kinds (default when --type is omitted)",
    )
    args = parser.parse_args()

    kinds = [args.kind] if args.kind else ALL_KINDS

    if not os.path.isdir(SRC_DIR):
        print(f"Error: src/ directory not found at {SRC_DIR}", file=sys.stderr)
        sys.exit(1)

    matches = search_tree(args.name, kinds)

    if not matches:
        print(f"Not found: '{args.name}' in {SRC_DIR}")
        print("Safe to declare — symbol does not exist in the project.")
        sys.exit(0)

    # Group by file for readable output
    by_file: dict = {}
    for filepath, lineno, kind, text in matches:
        rel = os.path.relpath(filepath, root_dir)
        by_file.setdefault(rel, []).append((lineno, kind, text))

    print(f"Found '{args.name}' in {len(by_file)} file(s):\n")
    for rel_path, hits in sorted(by_file.items()):
        print(f"  {rel_path}")
        for lineno, kind, text in hits:
            print(f"    {lineno:>5}: [{kind}] {text.strip()}")
        print()

    print("If it's a header file, include it instead of redeclaring. If it's a .cpp file, you need to move it to the correct header.")
    sys.exit(0)


if __name__ == "__main__":
    main()
