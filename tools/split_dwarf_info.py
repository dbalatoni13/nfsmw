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
# UMath namespace fixups
# ---------------------------------------------------------------------------

# Struct/type names to qualify with UMath::
UMATH_STRUCTS = [
    "Vector3",
    "Vector4",
    "Matrix4",
]

# Full function signatures to find and replace with UMath:: qualifier.
# Each entry is (find, replace) using exact signature prefixes from the header.
UMATH_FUNCTIONS = [
    (
        "inline struct UMath::Vector3 &Vector4To3(",
        "inline struct UMath::Vector3 &UMath::Vector4To3(",
    ),
    (
        "inline const struct UMath::Vector3 &Vector4To3(",
        "inline const struct UMath::Vector3 &UMath::Vector4To3(",
    ),
    (
        "inline struct UMath::Vector2 Vector2Make(",
        "inline struct UMath::Vector2 UMath::Vector2Make(",
    ),
    (
        "inline struct UMath::Vector3 Vector3Make(",
        "inline struct UMath::Vector3 UMath::Vector3Make(",
    ),
    (
        "inline struct UMath::Vector4 Vector4Make(",
        "inline struct UMath::Vector4 UMath::Vector4Make(",
    ),
    ("inline float Sina(", "inline float UMath::Sina("),
    ("inline float Cosa(", "inline float UMath::Cosa("),
    ("inline float Sinr(", "inline float UMath::Sinr("),
    ("inline float Cosr(", "inline float UMath::Cosr("),
    ("void BuildRotate(", "void UMath::BuildRotate("),
    ("float Ceil(", "float UMath::Ceil("),
    ("inline float Distance(", "inline float UMath::Distance("),
    ("inline float Distancexz(", "inline float UMath::Distancexz("),
    ("inline float DistanceSquare(", "inline float UMath::DistanceSquare("),
    ("inline float DistanceSquarexz(", "inline float UMath::DistanceSquarexz("),
    ("inline void Clear(", "inline void UMath::Clear("),
    ("inline void Copy(", "inline void UMath::Copy("),
    ("inline void Set(", "inline void UMath::Set("),
    ("inline void Transpose(", "inline void UMath::Transpose("),
    ("void Transpose(", "void UMath::Transpose("),
    (
        "inline const struct UMath::Vector3 &ExtractAxis(",
        "inline const struct UMath::Vector3 &UMath::ExtractAxis(",
    ),
    ("inline void ExtractXAxis(", "inline void UMath::ExtractXAxis("),
    ("inline void ExtractYAxis(", "inline void UMath::ExtractYAxis("),
    ("inline void ExtractZAxis(", "inline void UMath::ExtractZAxis("),
    ("inline void RotateTranslate(", "inline void UMath::RotateTranslate("),
    ("inline void Init(", "inline void UMath::Init("),
    ("inline void Mult(", "inline void UMath::Mult("),
    ("void Mult(", "void UMath::Mult("),
    ("inline void Unit(", "inline void UMath::Unit("),
    ("inline void Unitxyz(", "inline void UMath::Unitxyz("),
    ("inline void MultYRot(", "inline void UMath::MultYRot("),
    ("inline void QuaternionToMatrix4(", "inline void UMath::QuaternionToMatrix4("),
    ("void QuaternionToMatrix4(", "void UMath::QuaternionToMatrix4("),
    ("inline void Add(", "inline void UMath::Add("),
    ("inline void Scale(", "inline void UMath::Scale("),
    ("inline void ScaleAdd(", "inline void UMath::ScaleAdd("),
    ("inline void ScaleAddxyz(", "inline void UMath::ScaleAddxyz("),
    ("inline void AddScale(", "inline void UMath::AddScale("),
    ("inline void Sub(", "inline void UMath::Sub("),
    ("inline void Subxyz(", "inline void UMath::Subxyz("),
    ("inline void SetYRot(", "inline void UMath::SetYRot("),
    ("inline void Rotate(", "inline void UMath::Rotate("),
    ("void Rotate(", "void UMath::Rotate("),
    ("inline void RotateInXZ(", "inline void UMath::RotateInXZ("),
    ("inline float Dot(", "inline float UMath::Dot("),
    ("inline void Dot(", "inline void UMath::Dot("),
    ("inline float Dotxyz(", "inline float UMath::Dotxyz("),
    ("inline void Cross(", "inline void UMath::Cross("),
    ("inline void UnitCross(", "inline void UMath::UnitCross("),
    ("void UnitCross(", "void UMath::UnitCross("),
    ("inline float Normalize(", "inline float UMath::Normalize("),
    ("inline void Direction(", "inline void UMath::Direction("),
    ("inline float Lengthxz(", "inline float UMath::Lengthxz("),
    ("inline float Lengthxyz(", "inline float UMath::Lengthxyz("),
    ("inline float LengthSquare(", "inline float UMath::LengthSquare("),
    ("inline float Atan2d(", "inline float UMath::Atan2d("),
    ("inline float Atan2a(", "inline float UMath::Atan2a("),
    ("inline float Atan2r(", "inline float UMath::Atan2r("),
    ("inline float Sqrt(", "inline float UMath::Sqrt("),
    ("inline float Length(", "inline float UMath::Length("),
    ("inline void Matrix4ToQuaternion(", "inline void UMath::Matrix4ToQuaternion("),
    ("inline int Clamp(", "inline int UMath::Clamp("),
    ("inline float Clamp(", "inline float UMath::Clamp("),
    ("inline float Abs(", "inline float UMath::Abs("),
    ("inline float Pow(", "inline float UMath::Pow("),
    ("inline float Ramp(", "inline float UMath::Ramp("),
    ("inline float Lerp(", "inline float UMath::Lerp("),
    ("inline void Negate(", "inline void UMath::Negate("),
    ("inline float Min(", "inline float UMath::Min("),
    ("inline float Max(", "inline float UMath::Max("),
    ("inline unsigned int Min(", "inline unsigned int UMath::Min("),
    ("inline unsigned int Max(", "inline unsigned int UMath::Max("),
    ("inline float Limit(", "inline float UMath::Limit("),
]


def apply_umath_fixups(text: str) -> str:
    """
    Apply UMath:: namespace qualifications to struct names and function
    definitions that belong to the UMath namespace.
    """
    # Qualify struct names: 'struct Foo' -> 'struct UMath::Foo'
    # Only when not already qualified.
    for name in UMATH_STRUCTS:
        text = re.sub(
            rf"\bstruct\s+(?!UMath::){re.escape(name)}\b",
            f"struct UMath::{name}",
            text,
        )

    # Qualify function definitions using exact full signature prefixes.
    for find, replace in UMATH_FUNCTIONS:
        text = text.replace(find, replace)

    return text


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

    # Apply UMath:: namespace fixups to all collected output
    structs = [apply_umath_fixups(b) for b in structs]
    funcs = [apply_umath_fixups(b) for b in funcs]
    line_decls = [apply_umath_fixups(s) for s in line_decls]

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
