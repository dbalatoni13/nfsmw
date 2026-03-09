#!/usr/bin/env python3
"""
split_dwarf_info.py

Extract structs, enums, functions, globals, and typedefs from a DTK dwarf dump.

Output files written to <output_folder>/:
  globals.nothpp   — structs, enums, typedefs, and global declarations
  functions.nothpp — function bodies

Structs/enums with the same name are only deduplicated when they also have
identical non-static member variables (same offset/size annotations).
Structs that share a name but differ in members are all kept.

Usage:
    python split_dwarf_info.py <input_dwarf_dump.txt> <output_folder> [--no-stripped]

Options:
    --no-stripped   Exclude functions where either address is 0xFFFFFFFF (stripped).
"""

import re
import sys
import os
import argparse

# ---------------------------------------------------------------------------
# Regex patterns
# ---------------------------------------------------------------------------

re_total_size = re.compile(r"^// total size:\s*0x[0-9A-Fa-f]+")
re_struct_name = re.compile(r"\bstruct\s+(\w+)")
# Must start at column 0 — indented enums are nested inside structs
re_enum_start = re.compile(r"^enum\s+(?:class\s+)?(\w+)\s*(?::\s*[\w:]+\s*)?\{")
re_enum_name = re.compile(r"^enum\s+(?:class\s+)?(\w+)")
re_func_range = re.compile(r"^// Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)")
# Matches the offset/size annotation on member variable lines
re_member = re.compile(r"offset\s+0x[0-9A-Fa-f]+,\s*size\s+0x[0-9A-Fa-f]+")
re_typedef = re.compile(r"^typedef\s+\S")
# Any line that looks like a declaration: one or more type tokens followed by a name and semicolon.
# Excludes typedefs (handled separately) and lines that are clearly not declarations.
re_global_decl = re.compile(r"^(?:[\w\s:<>*&]+?)\b\w+\s*;")

STRIPPED_ADDR = "0XFFFFFFFF"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _get_name(block: str, pattern: re.Pattern) -> str | None:
    m = pattern.search(block)
    return m.group(1) if m else None


def _member_fingerprint(block: str) -> frozenset[str]:
    """
    Return a frozenset of every 'offset 0xNN, size 0xNN' annotation in the
    block. Identifies non-static members regardless of formatting differences.
    """
    return frozenset(re_member.findall(block))


def deduplicate_blocks(blocks: list[str], name_re: re.Pattern) -> list[str]:
    """
    Deduplicate struct/enum blocks.

    Two blocks are considered true duplicates only when they share both the
    same name AND the same member fingerprint (identical offset/size sets).
    Among true duplicates the longest block wins.

    Blocks with the same name but different members are all preserved.
    """
    # key: (name, fingerprint) -> (insertion_index, longest_block)
    seen: dict[tuple[str, frozenset], tuple[int, str]] = {}
    order: list[tuple[str, frozenset]] = []

    for block in blocks:
        name = _get_name(block, name_re) or block[:40]
        fp = _member_fingerprint(block)
        key = (name, fp)

        if key not in seen:
            seen[key] = (len(order), block)
            order.append(key)
        else:
            if len(block) > len(seen[key][1]):
                seen[key] = (seen[key][0], block)

    return [seen[k][1] for k in order]


# ---------------------------------------------------------------------------
# Struct extraction  (preceded by '// total size:' comment)
# ---------------------------------------------------------------------------


def extract_struct_blocks(lines: list[str]) -> list[str]:
    blocks = []
    current: list[str] = []
    in_block = False
    brace_depth = 0

    for line in lines:
        if not in_block:
            if re_total_size.match(line):
                current = [line]
                in_block = True
                brace_depth = 0
            continue
        current.append(line)
        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and line.rstrip().endswith("};"):
            blocks.append("".join(current))
            in_block = False

    return blocks


# ---------------------------------------------------------------------------
# Enum extraction  (bare 'enum Name {' at column 0 only)
# ---------------------------------------------------------------------------


def extract_enum_blocks(lines: list[str]) -> list[str]:
    blocks = []
    current: list[str] = []
    in_block = False
    brace_depth = 0

    for line in lines:
        stripped = line.strip()
        if not in_block:
            # Match on raw line so indented (nested) enums are skipped
            if re_enum_start.match(line):
                current = [line]
                in_block = True
                brace_depth = line.count("{") - line.count("}")
            continue
        current.append(line)
        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and stripped.endswith("};"):
            blocks.append("".join(current))
            in_block = False

    return blocks


# ---------------------------------------------------------------------------
# Function extraction
# ---------------------------------------------------------------------------


def extract_function_blocks(lines: list[str], exclude_stripped: bool) -> list[str]:
    blocks = []
    current: list[str] = []
    in_block = False
    brace_depth = 0
    skip = False

    for line in lines:
        if not in_block:
            m = re_func_range.match(line)
            if m:
                start, end = m.group(1).upper(), m.group(2).upper()
                skip = exclude_stripped and (
                    start == STRIPPED_ADDR or end == STRIPPED_ADDR
                )
                current = [line]
                in_block = True
                brace_depth = 0
            continue
        current.append(line)
        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and "}" in line:
            if not skip:
                blocks.append("".join(current))
            in_block = False

    return blocks


# ---------------------------------------------------------------------------
# Global / typedef extraction  (strict single-line only)
# ---------------------------------------------------------------------------


def extract_globals_and_typedefs(lines: list[str]) -> list[str]:
    results: list[str] = []
    in_skip_block = False
    brace_depth = 0

    for line in lines:
        stripped = line.strip()

        if not in_skip_block:
            if re_total_size.match(line) or re_enum_start.match(line):
                in_skip_block = True
                brace_depth = line.count("{") - line.count("}")
                continue
            if re_func_range.match(line):
                in_skip_block = True
                brace_depth = 0
                continue

        if in_skip_block:
            brace_depth += line.count("{") - line.count("}")
            if brace_depth == 0 and "}" in line:
                in_skip_block = False
            continue

        if not stripped or stripped.startswith("//"):
            continue
        # Skip indented lines — globals are always at column 0
        if line[0].isspace():
            continue
        # Strip trailing inline comment before checking
        code = re.sub(r"\s*//.*$", "", stripped)
        if not code.endswith(";") or "{" in code:
            continue

        if re_typedef.match(code) or re_global_decl.match(code):
            results.append(line.rstrip())

    return results


# ---------------------------------------------------------------------------
# Writer
# ---------------------------------------------------------------------------


def write_file(path: str, blocks: list[str], sep: str, label: str) -> None:
    with open(path, "w", encoding="utf-8") as f:
        f.write(sep.join(b.rstrip() for b in blocks))
        if blocks:
            f.write("\n")
    print(f"  {label}: {len(blocks)} entries -> {path}", file=sys.stderr)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Extract symbols from a DTK dwarf dump into .nothpp files."
    )
    parser.add_argument("input_file", help="Input dwarf dump text file")
    parser.add_argument("output_folder", help="Folder to write .nothpp files into")
    parser.add_argument(
        "--no-stripped",
        action="store_true",
        help="Exclude functions with a start or end address of 0xFFFFFFFF",
    )
    args = parser.parse_args()

    if not os.path.exists(args.input_file):
        print(f"Error: input file '{args.input_file}' does not exist.")
        sys.exit(1)

    os.makedirs(args.output_folder, exist_ok=True)

    with open(args.input_file, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()

    print("Extracting...", file=sys.stderr)

    all_brace = extract_struct_blocks(lines)
    structs = deduplicate_blocks(
        [b for b in all_brace if re_struct_name.search(b)], re_struct_name
    )
    enums = deduplicate_blocks(extract_enum_blocks(lines), re_enum_name)
    funcs = extract_function_blocks(lines, exclude_stripped=args.no_stripped)
    line_decls = extract_globals_and_typedefs(lines)

    write_file(
        os.path.join(args.output_folder, "globals.nothpp"),
        structs + enums,
        "\n\n",
        "structs+enums",
    )
    if line_decls:
        with open(
            os.path.join(args.output_folder, "globals.nothpp"), "a", encoding="utf-8"
        ) as f:
            f.write("\n\n")
            f.write("\n".join(line_decls))
            f.write("\n")
        print(
            f"  globals+typedefs: {len(line_decls)} entries appended", file=sys.stderr
        )

    write_file(
        os.path.join(args.output_folder, "functions.nothpp"),
        funcs,
        "\n\n",
        "functions",
    )

    print("Done.", file=sys.stderr)


if __name__ == "__main__":
    main()
