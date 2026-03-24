#!/usr/bin/env python3
"""
Address lookup tool: given a hex address, finds the closest entry in a map file
and returns 50 lines before and 50 lines after it.

Usage:
    python tools/line_lookup.py <mapfile> <address>

Example:
    python tools/line_lookup.py debug_lines.txt 0x801AE820
    python tools/line_lookup.py debug_lines.txt 801AE820
"""

import sys
import re


def parse_map_file(filepath):
    """Parse the map file into a list of (address, line_text) tuples, preserving order."""
    entries = []
    with open(filepath, "r") as f:
        for line in f:
            line = line.rstrip("\n")
            match = re.match(r"^\s*((?:0x)?[0-9A-Fa-f]+)\s*:", line)
            if match:
                addr = int(match.group(1), 16)
                entries.append((addr, line))
    return entries


def find_closest_index(entries, target_addr):
    """Find the index of the entry with the address closest to target_addr."""
    best_idx = 0
    best_diff = abs(entries[0][0] - target_addr)

    for i, (addr, _) in enumerate(entries):
        diff = abs(addr - target_addr)
        if diff < best_diff:
            best_diff = diff
            best_idx = i

    return best_idx, best_diff


def main():
    if len(sys.argv) != 3:
        print("Usage: python line_lookup.py <mapfile> <address>")
        print("Example: python line_lookup.py debug_lines.txt 0x801AE820")
        sys.exit(1)

    mapfile = sys.argv[1]
    raw_addr = sys.argv[2]

    # Accept addresses with or without 0x prefix
    try:
        target_addr = int(raw_addr, 16)
    except ValueError:
        print(f"Error: '{raw_addr}' is not a valid hex address.")
        sys.exit(1)

    try:
        entries = parse_map_file(mapfile)
    except FileNotFoundError:
        print(f"Error: File '{mapfile}' not found.")
        sys.exit(1)

    if not entries:
        print("Error: No valid address entries found in the file.")
        sys.exit(1)

    idx, diff = find_closest_index(entries, target_addr)
    matched_addr, matched_line = entries[idx]

    # Report whether it was exact or approximate
    if diff == 0:
        print(f"Exact match found: 0x{matched_addr:08X}")
    else:
        print(
            f"No exact match for 0x{target_addr:08X}. "
            f"Closest address: 0x{matched_addr:08X} (off by {diff} / 0x{diff:X})"
        )

    print(f"Match at file line index {idx}: {matched_line}")
    print("-" * 80)

    # Grab 50 lines before and 50 after (clamped to file bounds)
    start = max(0, idx - 50)
    end = min(len(entries), idx + 51)  # +51 so the match itself is included

    print(f"Showing entries [{start}:{end}] (total {end - start} lines):\n")
    for i in range(start, end):
        marker = " >>> " if i == idx else "     "
        print(f"{marker}{entries[i][1]}")


if __name__ == "__main__":
    main()
