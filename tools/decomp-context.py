#!/usr/bin/env python3

"""
Function context gatherer for decomp work.

Collects some things an agent needs to work on matching a function: objdiff status/diff and Ghidra decompile.
Source code and dwarf info should be queried from the lookup script instead.

Usage:
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --no-ghidra
  python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f __9CAnimBank --no-source
  python tools/decomp-context.py --ghidra-check
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from typing import Any, Dict, List, Optional, Tuple

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))

OBJDIFF_CLI = os.path.join(root_dir, "build", "tools", "objdiff-cli")
OBJDIFF_JSON = os.path.join(root_dir, "objdiff.json")
DTK = os.path.join(root_dir, "build", "tools", "dtk")
GC_SYMBOLS_FILE = os.path.join(root_dir, "config", "GOWE69", "symbols.txt")
PS2_SYMBOLS_FILE = os.path.join(root_dir, "config", "SLES-53558-A124", "symbols.txt")
GC_GHIDRA_PROGRAM = "NFSMWRELEASE.ELF"
PS2_GHIDRA_PROGRAM = "NFS.ELF"

# Number of lines of context to show before/after the matched function in source
SOURCE_CONTEXT_LINES = 5


def load_project_config() -> Dict[str, Any]:
    """Load objdiff.json."""
    with open(OBJDIFF_JSON) as f:
        return json.load(f)


def find_unit(config: Dict[str, Any], unit_name: str) -> Optional[Dict[str, Any]]:
    """Find a unit in the project config."""
    for unit in config.get("units", []):
        if unit["name"] == unit_name:
            return unit
    return None


def run_objdiff(unit_name: str) -> Optional[Dict[str, Any]]:
    """Run objdiff-cli diff and return parsed JSON."""
    result = subprocess.run(
        [OBJDIFF_CLI, "diff", "-u", unit_name, "-o", "-", "--format", "json"],
        capture_output=True,
        cwd=root_dir,
    )
    if result.returncode != 0:
        return None
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return None


def find_symbol_in_diff(
    diff_data: Dict[str, Any], func_name: str
) -> Tuple[Optional[Dict], Optional[Dict]]:
    """Find a symbol by name in the diff data. Returns (left_sym, right_sym).

    Uses target_symbol as a direct index into the other side's symbol array.
    """
    left_syms = diff_data.get("left", {}).get("symbols", [])
    right_syms = diff_data.get("right", {}).get("symbols", [])

    pattern = func_name.lower()

    for sym in left_syms:
        name = sym.get("demangled_name", sym.get("name", ""))
        mangled = sym.get("name", "")
        if pattern in name.lower() or pattern in mangled.lower():
            right_sym = None
            ts = sym.get("target_symbol")
            if ts is not None and ts < len(right_syms):
                right_sym = right_syms[ts]
            return sym, right_sym

    for sym in right_syms:
        name = sym.get("demangled_name", sym.get("name", ""))
        mangled = sym.get("name", "")
        if pattern in name.lower() or pattern in mangled.lower():
            left_sym = None
            ts = sym.get("target_symbol")
            if ts is not None and ts < len(left_syms):
                left_sym = left_syms[ts]
            return left_sym, sym

    return None, None


def lookup_symbol_address(file: str, mangled_name: str) -> Optional[str]:
    """Look up a symbol's address from symbols.txt."""
    if not os.path.exists(file):
        return None
    pattern = re.compile(
        r"^" + re.escape(mangled_name) + r"\s*=\s*(?:\.(\w+):)?0x([0-9A-Fa-f]+)"
    )
    with open(file) as f:
        for line in f:
            m = pattern.match(line.strip())
            if m:
                return m.group(2)
    return None


def search_symbols_file(file: str, name: str) -> List[Tuple[str, str, str]]:
    """Search symbols.txt for entries matching a name. Returns [(mangled, section, address)]."""
    if not os.path.exists(file):
        return []
    results = []
    pattern = name.lower()
    with open(file) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            if pattern in line.lower():
                m = re.match(r"^(\S+)\s*=\s*(?:\.(\w+):)?0x([0-9A-Fa-f]+)", line)
                if m:
                    results.append((m.group(1), m.group(2), m.group(3)))
    return results


def ghidra_decompile(address: str, program: str) -> Tuple[Optional[str], Optional[str]]:
    """Decompile a function at the given address using Ghidra CLI.

    Returns (code, error_message). On success error_message is None.
    On failure code is None and error_message describes the problem.
    """
    ghidra_cmd = shutil.which("ghidra-cli") or shutil.which("ghidra")
    if ghidra_cmd is None:
        return None, "ghidra-cli / ghidra not found in PATH"

    try:
        result = subprocess.run(
            [
                ghidra_cmd,
                "decompile",
                "--program",
                program,
                f"0x{address}",
                "--json",
            ],
            capture_output=True,
            timeout=30,
        )
        if result.returncode != 0:
            stderr = result.stderr.decode(errors="replace").strip()
            msg = f"ghidra exited with code {result.returncode}"
            if stderr:
                msg += f"\n  stderr: {stderr}"
            return None, msg
        data = json.loads(result.stdout)
        if data and isinstance(data, list) and len(data) > 0:
            return data[0].get("code", ""), None
        return None, "ghidra returned empty result"
    except subprocess.TimeoutExpired:
        return None, "ghidra timed out after 30s"
    except FileNotFoundError:
        return None, f"ghidra binary not executable: {ghidra_cmd}"
    except json.JSONDecodeError as e:
        return None, f"ghidra returned invalid JSON: {e}"


def check_ghidra() -> None:
    """Verify Ghidra CLI is reachable and the required programs are loaded."""
    ghidra_cmd = shutil.which("ghidra-cli") or shutil.which("ghidra")
    if ghidra_cmd is None:
        print("FAIL  ghidra-cli / ghidra not found in PATH")
        print("      Install ghidra-cli and ensure it is on your PATH.")
        sys.exit(1)
    print(f"OK    ghidra binary: {ghidra_cmd}")

    # Try a minimal command that lists available programs
    try:
        result = subprocess.run(
            [ghidra_cmd, "list", "programs"],
            capture_output=True,
            timeout=15,
        )
        output = result.stdout.decode(errors="replace")
        stderr = result.stderr.decode(errors="replace").strip()

        for prog in [GC_GHIDRA_PROGRAM, PS2_GHIDRA_PROGRAM]:
            if prog in output:
                print(f"OK    program found: {prog}")
            else:
                print(f"WARN  program not found in listing: {prog}")
                print(f"      Run: ghidra set-default project NeedForSpeed")

        if result.returncode != 0 and stderr:
            print(f"WARN  ghidra list programs exited {result.returncode}: {stderr}")
    except subprocess.TimeoutExpired:
        print("WARN  ghidra list programs timed out — Ghidra may be slow to start")
    except Exception as e:
        print(f"WARN  could not verify programs: {e}")

    # Quick decompile smoke test — use a known small GC function address if symbols exist
    if os.path.exists(GC_SYMBOLS_FILE):
        with open(GC_SYMBOLS_FILE) as f:
            for line in f:
                m = re.match(r"^\S+\s*=\s*(?:\.text:)?0x([0-9A-Fa-f]+)", line.strip())
                if m:
                    test_addr = m.group(1)
                    break
            else:
                test_addr = None
        if test_addr:
            code, err = ghidra_decompile(test_addr, GC_GHIDRA_PROGRAM)
            if code is not None:
                print(f"OK    decompile smoke test passed (0x{test_addr})")
            else:
                print(f"FAIL  decompile smoke test failed for 0x{test_addr}: {err}")
    else:
        print(f"SKIP  smoke test — symbols file not found: {GC_SYMBOLS_FILE}")


def extract_source_for_function(
    source_path: str, right_sym: Optional[Dict[str, Any]]
) -> Optional[str]:
    """Extract the source lines relevant to the function from its source file.

    Uses the line numbers embedded in the right (decomp) symbol's instructions
    to determine the source region. Falls back to full file if line numbers are
    unavailable or the file cannot be read.

    Returns the extracted source text, or None if the file doesn't exist.
    """
    full_path = os.path.join(root_dir, source_path)
    if not os.path.exists(full_path):
        return None

    with open(full_path) as f:
        all_lines = f.readlines()

    # Collect all source line numbers referenced by the decomp symbol's instructions
    line_numbers: List[int] = []
    if right_sym:
        for inst_entry in right_sym.get("instructions", []):
            ln = inst_entry.get("instruction", {}).get("line_number")
            if ln is not None:
                line_numbers.append(int(ln))

    if not line_numbers:
        # No line info available — return full source
        return "".join(all_lines)

    first_line = min(line_numbers)
    last_line = max(line_numbers)

    # Expand to capture the enclosing function: walk back to find the function
    # signature (a line that looks like a function definition, not just a brace).
    # Heuristic: walk backward from first_line to find an opening that precedes
    # the function body, giving context.
    start = max(1, first_line - SOURCE_CONTEXT_LINES)
    end = min(len(all_lines), last_line + SOURCE_CONTEXT_LINES)

    # 1-indexed lines -> 0-indexed slice
    excerpt = all_lines[start - 1 : end]
    header = f"// Lines {start}–{end} of {source_path}\n"
    return header + "".join(excerpt)


def strip_ansi(text: str) -> str:
    """Remove ANSI escape codes from text."""
    return re.sub(r"\x1b\[[0-9;]*m", "", text)


def tu_name_from_unit(unit: Dict[str, Any]) -> str:
    """Derive the TU .o name from a unit's metadata."""
    source = unit.get("metadata", {}).get("source_path", "")
    if source:
        # src/MetroidPrime/CEntity.cpp -> CEntity.o
        base = os.path.splitext(os.path.basename(source))[0]
        return f"{base}.o"
    # Fallback: derive from unit name
    name = unit["name"]
    base = name.rsplit("/", 1)[-1]
    return f"{base}.o"


def print_section(title: str, content: str) -> None:
    """Print a labeled section."""
    print(f"\n{'=' * 60}")
    print(f"  {title}")
    print(f"{'=' * 60}")
    print(content)


def print_ghidra_decompilation(
    version_name: str,
    symbols_file: str,
    program: str,
    function: str,
    mangled_function: str,
):
    # Try to find address from symbols.txt
    addr = lookup_symbol_address(symbols_file, mangled_function)

    if not addr:
        # Try searching symbols.txt
        matches = search_symbols_file(symbols_file, function)
        if matches:
            # Pick the first function match
            addr = matches[0][2]
            mangled_function = matches[0][0]

    if addr:
        code, err = ghidra_decompile(addr, program)
        if code is not None:
            print_section(f"{version_name}: Ghidra Decompile (0x{addr})", code)
        else:
            print(f"\nGhidra decompile failed for {version_name} 0x{addr}: {err}")
    else:
        print(
            f"\nCould not find address for {mangled_function} in {version_name} symbols.txt"
        )


def main():
    parser = argparse.ArgumentParser(
        description="Gather context for decomp function matching"
    )
    parser.add_argument(
        "-u", "--unit", help="Unit name (e.g. main/MetroidPrime/CEntity)"
    )
    parser.add_argument("-f", "--function", help="Function name to look up")
    parser.add_argument(
        "--no-source", action="store_true", help="Skip source file output"
    )
    parser.add_argument(
        "--no-ghidra", action="store_true", help="Skip Ghidra decompile"
    )
    parser.add_argument(
        "--ghidra-check",
        action="store_true",
        help="Verify Ghidra CLI is reachable and programs are loaded, then exit",
    )
    args = parser.parse_args()

    if args.ghidra_check:
        check_ghidra()
        return

    if not args.unit or not args.function:
        parser.error("-u/--unit and -f/--function are required (or use --ghidra-check)")

    config = load_project_config()
    unit = find_unit(config, args.unit)
    if not unit:
        print(f"Unit not found: {args.unit}", file=sys.stderr)
        sys.exit(1)

    meta = unit.get("metadata", {})
    source_path = meta.get("source_path", "")

    # === objdiff Status (run first so we have line numbers for source scoping) ===
    diff_data = run_objdiff(args.unit)
    left_sym = right_sym = None

    if diff_data:
        left_sym, right_sym = find_symbol_in_diff(diff_data, args.function)

    # === Source File (scoped to function if line info available) ===
    if not args.no_source and source_path:
        excerpt = extract_source_for_function(source_path, right_sym)
        if excerpt is not None:
            label = "Source"
            if right_sym and right_sym.get("instructions"):
                # Check if we actually got line info
                has_lines = any(
                    inst.get("instruction", {}).get("line_number") is not None
                    for inst in right_sym.get("instructions", [])
                )
                if has_lines:
                    label = "Source (function excerpt)"
                else:
                    label = f"Source (full file — no line info): {source_path}"
            print_section(label, excerpt)
        else:
            print(
                f"\nSource file not found: {os.path.join(root_dir, source_path)}",
                file=sys.stderr,
            )

    # === objdiff Status ===
    if diff_data:
        if left_sym or right_sym:
            sym = left_sym if left_sym is not None else right_sym
            assert sym is not None
            name = sym.get("demangled_name", sym.get("name", "?"))
            mangled = sym.get("name", "?")
            mp = sym.get("match_percent")
            size = int(sym.get("size", "0"))

            status_lines = []
            status_lines.append(f"Function: {name}")
            status_lines.append(f"Mangled:  {mangled}")
            status_lines.append(f"Size:     {size} bytes")
            if mp is not None:
                status_lines.append(f"Match:    {mp:.1f}%")
            else:
                status_lines.append("Match:    N/A (not in both sides)")

            # Quick diff summary
            if left_sym and right_sym:
                left_insts = left_sym.get("instructions", [])
                right_insts = right_sym.get("instructions", [])
                n_mismatch = sum(
                    1 for inst in left_insts if inst.get("diff_kind", "") != ""
                )
                status_lines.append(
                    f"Instructions: {max(len(left_insts), len(right_insts))} "
                    f"({n_mismatch} mismatched)"
                )

            print_section("objdiff Status", "\n".join(status_lines))

            # Run decomp-diff.py for the actual diff if not 100%
            if mp is not None and mp < 100.0:
                result = subprocess.run(
                    [
                        sys.executable,
                        os.path.join(script_dir, "decomp-diff.py"),
                        "-u",
                        args.unit,
                        "-d",
                        args.function,
                    ],
                    capture_output=True,
                    cwd=root_dir,
                )
                if result.returncode == 0:
                    print_section("Instruction Diff", result.stdout.decode())
        else:
            print(
                f"\nFunction '{args.function}' not found in objdiff data for {args.unit}"
            )
            # List available symbols
            left_syms = diff_data.get("left", {}).get("symbols", [])
            right_syms = diff_data.get("right", {}).get("symbols", [])
            print("Available functions:")
            seen = set()
            for sym in left_syms + right_syms:
                if "instructions" in sym:
                    name = sym.get("demangled_name", sym.get("name", "?"))
                    if name not in seen:
                        seen.add(name)
                        print(f"  {name}")
    else:
        print(f"\nFailed to run objdiff for {args.unit}", file=sys.stderr)

    # === Ghidra Decompile ===
    if not args.no_ghidra and (left_sym or right_sym):
        sym = left_sym if left_sym is not None else right_sym
        assert sym is not None
        mangled = sym.get("name", "")

        print_ghidra_decompilation(
            "GOWE69", GC_SYMBOLS_FILE, GC_GHIDRA_PROGRAM, args.function, mangled
        )
        print_ghidra_decompilation(
            "SLES-53558-A124",
            PS2_SYMBOLS_FILE,
            PS2_GHIDRA_PROGRAM,
            args.function,
            mangled,
        )


if __name__ == "__main__":
    main()
