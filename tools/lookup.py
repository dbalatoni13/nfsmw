#!/usr/bin/env python3
"""
lookup.py

Query structs, enums, functions, globals, or typedefs from a folder of .nothpp
files, or from a single combined file.

Structs: all variants with the given name are printed (separated by a blank line).
Enums:   supports 'EnumName' for top-level enums, or 'StructName::EnumName' for
         enums nested inside a struct.

Folder mode (default):
    python tools/lookup.py <folder> struct   <StructName>
    python tools/lookup.py <folder> enum     <EnumName>
    python tools/lookup.py <folder> enum     <StructName::EnumName>
    python tools/lookup.py <folder> function <FunctionAddress>
    python tools/lookup.py <folder> function "<DemangledFunctionName>"
    python tools/lookup.py <folder> global   <GlobalName>
    python tools/lookup.py <folder> typedef  <TypedefName>

Single-file mode (--file):
    python tools/lookup.py --file <path> struct   <StructName>
    python tools/lookup.py --file <path> function <FunctionAddress>
    ...  (all kinds searched in the one file)
"""

import re
import sys
import os
import argparse

# ---------------------------------------------------------------------------
# Regex patterns
# ---------------------------------------------------------------------------

re_total_size = re.compile(r"^// total size:\s*0x[0-9A-Fa-f]+")
# PS2 format: struct Name ... { // 0x28
re_struct_inline = re.compile(r"^struct\s+\w+.*\{\s*//\s*0x[0-9A-Fa-f]")
re_struct_name = re.compile(r"\bstruct\s+(\w+)")
re_enum_start = re.compile(r"^enum\s+(?:class\s+)?(\w+)\s*(?::\s*[\w:]+\s*)?\{")
re_enum_name = re.compile(r"^enum\s+(?:class\s+)?(\w+)")
re_func_range = re.compile(r"^// Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)")
re_typedef_name = re.compile(r"^typedef\s+.+?(\w+)\s*;")

# Global name: last \w+ token before the optional trailing ';' or '// comment'.
# Handles all of:
#   int asd;
#   struct CAnimCandidateData * TheAnimCandidateData;
#   extern int foo;
#   const struct UCrc32 CAR; // size: 0x4
# We extract the last word before the semicolon (skipping pointer stars etc.)
re_global_name = re.compile(r"^(?:[\w\s:<>*&]+?)\b(\w+)\s*;")

# ---------------------------------------------------------------------------
# Struct splitter
# ---------------------------------------------------------------------------


def split_structs(text: str) -> list[tuple[str, str]]:
    """Returns [(name, block), ...] for every struct block.

    Supports two formats:
      - PC/GC:  preceded by a '// total size: 0xNN' comment on the previous line
      - PS2:    struct Name ... { // 0xNN  (size inline on the opening line)
    """
    results = []
    lines = text.splitlines(keepends=True)
    current: list[str] = []
    in_block = False
    brace_depth = 0
    pending: list[str] = (
        []
    )  # holds the '// total size:' header line until struct line arrives

    for line in lines:
        if not in_block:
            if re_total_size.match(line):
                # PC/GC: save header, wait for the struct line with '{'
                pending = [line]
                continue
            if re_struct_inline.match(line):
                # PS2: opening brace already on this line
                current = [line]
                in_block = True
                brace_depth = line.count("{") - line.count("}")
                pending = []
                continue
            if pending:
                # Next line after total_size header — should be the struct line
                pending.append(line)
                if "{" in line:
                    current = pending
                    in_block = True
                    brace_depth = sum(l.count("{") - l.count("}") for l in current)
                    pending = []
                else:
                    # Not a struct line, discard pending
                    pending = []
            continue

        current.append(line)
        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and line.rstrip().endswith("};"):
            block = "".join(current)
            m = re_struct_name.search(block)
            if m:
                results.append((m.group(1), block))
            in_block = False

    return results


# ---------------------------------------------------------------------------
# Enum splitter — top-level and nested (StructName::EnumName)
# ---------------------------------------------------------------------------


def _extract_nested_enums(struct_name: str, block: str) -> list[tuple[str, str]]:
    """Find all indented enum blocks inside a struct block."""
    results = []
    lines = block.splitlines(keepends=True)
    current: list[str] = []
    in_enum = False
    brace_depth = 0

    for line in lines:
        stripped = line.strip()
        if not in_enum:
            # Indented: line is not equal to its stripped form
            if line != stripped and re_enum_start.match(stripped):
                current = [line]
                in_enum = True
                brace_depth = line.count("{") - line.count("}")
            continue
        current.append(line)
        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and stripped.endswith("};"):
            enum_block = "".join(current)
            m = re_enum_name.match(enum_block.strip())
            if m:
                results.append((f"{struct_name}::{m.group(1)}", enum_block))
            in_enum = False

    return results


def split_enums(text: str) -> list[tuple[str, str]]:
    """
    Returns [(name, block), ...] for top-level enums and
    [(StructName::EnumName, block), ...] for nested enums.
    """
    results: list[tuple[str, str]] = []
    lines = text.splitlines(keepends=True)
    current: list[str] = []
    in_block = False
    brace_depth = 0

    # top-level enums (col-0 only)
    for line in lines:
        stripped = line.strip()
        if not in_block:
            if re_enum_start.match(line):
                current = [line]
                in_block = True
                brace_depth = line.count("{") - line.count("}")
            continue
        current.append(line)
        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and stripped.endswith("};"):
            block = "".join(current)
            m = re_enum_name.match(block.strip())
            if m:
                results.append((m.group(1), block))
            in_block = False

    # nested enums
    for struct_name, struct_block in split_structs(text):
        results.extend(_extract_nested_enums(struct_name, struct_block))

    return results


# ---------------------------------------------------------------------------
# Function splitter
# ---------------------------------------------------------------------------


def split_functions(text: str) -> list[tuple[str, str, str, str]]:
    """Returns [(start_addr, end_addr, signature_line, block), ...].

    Only top-level (non-indented) // Range: headers are treated as function
    boundaries, so inlined functions nested inside a block are ignored.
    """
    results = []
    lines = text.splitlines(keepends=True)
    current: list[str] = []
    in_block = False
    brace_depth = 0
    start_addr = end_addr = ""
    sig_line = ""

    for line in lines:
        if not in_block:
            # Only match // Range: at column 0 (not indented)
            m = re_func_range.match(line)
            if m:
                start_addr, end_addr = m.group(1).upper(), m.group(2).upper()
                current = [line]
                in_block = True
                brace_depth = 0
                sig_line = ""
            continue

        current.append(line)
        stripped = line.strip()

        # Capture the first non-comment, non-blank line after the Range header
        # as the signature line (before any opening brace content).
        if not sig_line and stripped and not stripped.startswith("//"):
            # Take everything up to (but not including) the opening brace
            sig_line = stripped.split("{")[0].strip()

        brace_depth += line.count("{") - line.count("}")
        if brace_depth == 0 and "}" in line:
            results.append((start_addr, end_addr, sig_line, "".join(current)))
            in_block = False

    return results


# ---------------------------------------------------------------------------
# Function name normalisation & matching
# ---------------------------------------------------------------------------


def _normalise_func_name(name: str) -> str:
    """
    Strip a query like  EPerfectLaunch::~EPerfectLaunch(void)
    down to the bare qualified name without parameter list:
        EPerfectLaunch::~EPerfectLaunch
    This lets us match against signatures that may have different param lists
    or return-type prefixes in the DWARF output.
    """
    # Drop everything from the first '(' onwards
    paren = name.find("(")
    if paren != -1:
        name = name[:paren]
    return name.strip()


def _candidate_func_names(name: str) -> list[str]:
    """
    Generate progressively shorter qualified-name suffixes.

    Example:
      Attrib::Class::RemoveCollection -> [
          'Attrib::Class::RemoveCollection',
          'Class::RemoveCollection',
          'RemoveCollection',
      ]

    This helps match DWARF signatures that omit leading namespaces.
    """
    bare = _normalise_func_name(name)
    if not bare:
        return []

    parts = bare.split("::")
    candidates: list[str] = []
    for index in range(len(parts)):
        candidate = "::".join(parts[index:]).strip()
        if candidate and candidate not in candidates:
            candidates.append(candidate)
    return candidates


def _sig_contains_name(sig_line: str, bare_name: str) -> bool:
    """
    Return True if *bare_name* (e.g. 'EPerfectLaunch::~EPerfectLaunch') appears
    in *sig_line* as a whole token (not a substring of a longer identifier).

    sig_line examples (everything before the first '{'):
        EPerfectLaunch::~EPerfectLaunch()
        void EPerfectLaunch::~EPerfectLaunch()
        struct Foo * (*EPerfectLaunch::~EPerfectLaunch)(int)
    We want to match the qualified name regardless of leading return-type tokens.

    Strategy: search for bare_name in sig_line, then verify the characters
    immediately surrounding the match are not word/colon/tilde characters
    (so we don't match a longer name).
    """
    idx = sig_line.find(bare_name)
    if idx == -1:
        return False
    # Check character before match (must not be a word char, ':', or '~')
    if idx > 0 and re.match(r"[\w:~]", sig_line[idx - 1]):
        return False
    # Check character after match (must not be a word char, ':', or '~')
    # This prevents 'Foo::~Bar' from matching 'Foo::~BarExtra'
    end = idx + len(bare_name)
    if end < len(sig_line) and re.match(r"[\w:~]", sig_line[end]):
        return False
    return True


def find_function_by_address(
    funcs: list[tuple[str, str, str, str]], query: str
) -> str | None:
    q = query.upper()
    for start, end, sig, block in funcs:
        if start == q:
            return block
    return None


def find_functions_by_name(
    funcs: list[tuple[str, str, str, str]], query: str
) -> list[str]:
    """Return all function blocks whose signature matches *query* (ignoring params)."""
    candidates = _candidate_func_names(query)
    return [
        block
        for start, end, sig, block in funcs
        if any(_sig_contains_name(sig, candidate) for candidate in candidates)
    ]


# ---------------------------------------------------------------------------
# Single-line declaration splitter (globals / typedefs)
# Skips content inside any brace block (structs, enums, functions).
# ---------------------------------------------------------------------------


def split_line_decls(text: str, name_re: re.Pattern) -> list[tuple[str, str]]:
    """Returns [(name, line), ...], skipping lines inside any brace block."""
    results = []
    lines = text.splitlines(keepends=True)
    in_skip = False
    brace_depth = 0

    for line in lines:
        stripped = line.strip()

        if not in_skip:
            if re_total_size.match(line) or re_enum_start.match(line):
                in_skip = True
                brace_depth = line.count("{") - line.count("}")
                continue
            if re_struct_inline.match(line):
                in_skip = True
                brace_depth = line.count("{") - line.count("}")
                continue
            if re_func_range.match(line):
                in_skip = True
                brace_depth = 0
                continue

        if in_skip:
            brace_depth += line.count("{") - line.count("}")
            if brace_depth == 0 and "}" in line:
                in_skip = False
            continue

        if not stripped or stripped.startswith("//"):
            continue
        # Strip trailing inline comment before checking for semicolon
        code = re.sub(r"\s*//.*$", "", stripped)
        # Must end with ';' and contain no opening brace
        if not code.endswith(";") or "{" in code:
            continue
        # Skip typedef lines when looking for globals (they have their own splitter)
        if name_re is re_global_name and code.startswith("typedef"):
            continue

        m = name_re.match(code)
        if m:
            results.append((m.group(1), line.rstrip()))

    return results


# ---------------------------------------------------------------------------
# Lookup helpers
# ---------------------------------------------------------------------------


def find_all_by_name(items: list[tuple[str, str]], name: str) -> list[str]:
    return [block for item_name, block in items if item_name == name]


def _looks_like_address(query: str) -> bool:
    return re.match(r"^0x[0-9A-Fa-f]+$", query) is not None


# ---------------------------------------------------------------------------
# File resolution
# ---------------------------------------------------------------------------

KIND_TO_FILE = {
    "struct": "globals.nothpp",
    "enum": "globals.nothpp",
    "global": "globals.nothpp",
    "typedef": "globals.nothpp",
    "function": "functions.nothpp",
}

VALID_KINDS = list(KIND_TO_FILE.keys())


def read_text(path: str) -> str:
    if not os.path.exists(path):
        print(f"Error: '{path}' does not exist.")
        sys.exit(1)
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        return f.read()


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main():
    parser = argparse.ArgumentParser(
        description="Look up symbols from .nothpp files.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        "--file",
        metavar="PATH",
        help="Query a single combined file instead of a folder.",
    )
    parser.add_argument(
        "source", nargs="?", default=None, help="Folder path (omit when using --file)"
    )
    parser.add_argument("kind", help=f"Symbol kind: {', '.join(VALID_KINDS)}")
    parser.add_argument("query", help="Name or address to look up")
    args = parser.parse_args()

    kind = args.kind.lower()
    query = args.query

    if kind not in VALID_KINDS:
        print(f"Error: unknown kind '{kind}'. Choose from: {', '.join(VALID_KINDS)}")
        sys.exit(1)

    # Resolve the file to read
    if args.file:
        text = read_text(args.file)
    else:
        if not args.source:
            print("Error: a folder path is required when --file is not used.")
            sys.exit(1)
        folder = args.source
        if not os.path.isdir(folder):
            print(
                f"Error: '{folder}' is not a directory. Use --file for a single file."
            )
            sys.exit(1)
        text = read_text(os.path.join(folder, KIND_TO_FILE[kind]))

    # Dispatch
    if kind == "struct":
        matches = find_all_by_name(split_structs(text), query)
        if not matches:
            print(f"Error: struct '{query}' not found.")
            sys.exit(1)
        print("\n\n".join(m.rstrip() for m in matches))

    elif kind == "enum":
        matches = find_all_by_name(split_enums(text), query)
        if not matches:
            print(f"Error: enum '{query}' not found.")
            sys.exit(1)
        print("\n\n".join(m.rstrip() for m in matches))

    elif kind == "function":
        funcs = split_functions(text)
        if _looks_like_address(query):
            result = find_function_by_address(funcs, query)
            if result is None:
                print(f"Error: function '{query}' not found.")
                sys.exit(1)
            print(result.rstrip())
        else:
            matches = find_functions_by_name(funcs, query)
            if not matches:
                print(f"Error: function '{query}' not found.")
                sys.exit(1)
            print("\n\n".join(m.rstrip() for m in matches))

    elif kind == "typedef":
        matches = find_all_by_name(split_line_decls(text, re_typedef_name), query)
        if not matches:
            print(f"Error: typedef '{query}' not found.")
            sys.exit(1)
        print(matches[0].rstrip())

    elif kind == "global":
        matches = find_all_by_name(split_line_decls(text, re_global_name), query)
        if not matches:
            print(f"Error: global '{query}' not found.")
            sys.exit(1)
        print(matches[0].rstrip())


if __name__ == "__main__":
    main()
