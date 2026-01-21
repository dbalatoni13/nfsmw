#!/usr/bin/env python3
"""
message_yaml_generator.py

Generate the messages.yml (new format) from dtk dwarf dump.

Usage:
    python message_yaml_generator.py <input_dwarf_dump.txt> <output.yml>
"""

import re
import yaml
import sys
import os

# -----------------------------
#   OPTIONAL TYPE FIXUP TABLE
# -----------------------------
type_fixups = {
    "HSIMABLE__ *": "HSIMABLE",
    "HMODEL__ *": "HMODEL",
    "Vector2": "UMath::Vector2",
    "Vector3": "UMath::Vector3",
    "Vector4": "UMath::Vector4",
    "Info": "COLLISION_INFO",
}

# Regex patterns
re_message = re.compile(r"struct\s+(\w+)\s*:\s*public\s+Message")
re_total_size = re.compile(r"^// total size:\s*0x([0-9A-Fa-f]+)")
re_field = re.compile(
    r"^\s*(?:struct\s+|enum\s+)?([\w:<>_]+(?:\s+[\w:<>_]+)*\s*\*?)\s+(\w+);\s*//\s*offset\s*0x([0-9A-Fa-f]+),\s*size\s*0x([0-9A-Fa-f]+)"
)


def apply_fixups(type_name: str) -> str:
    t = type_name.strip()
    if t.startswith("enum "):
        t = t[len("enum ") :].strip()
    if t.startswith("struct "):
        t = t[len("struct ") :].strip()
    return type_fixups.get(t, t)


import re


def extract_struct_blocks(lines: list[str]) -> list[str]:
    blocks = []
    current = []
    in_block = False
    brace_depth = 0

    for line in lines:
        if not in_block:
            if re_total_size.match(line):
                # Start a new block at the size comment
                current = [line]
                in_block = True
                brace_depth = 0
            continue

        current.append(line)
        brace_depth += line.count("{") - line.count("}")

        # End condition: struct closed
        if brace_depth == 0 and line.rstrip().endswith("};"):
            blocks.append("".join(current))
            in_block = False

    return blocks


def parse_struct(lines):
    result = {
        "Name": None,
        "Size": 0,
        "HasData": False,
        "Data": [],
    }

    # Identify message name + total instance size
    for line in lines:
        m = re_total_size.search(line)
        if m and result["Size"] == 0:
            result["Size"] = int(m.group(1), 16)

        m = re_message.search(line)
        if m:
            name = m.group(1)
            result["Name"] = name
            break

    instance_fields = []

    for line in lines:
        # Field
        m = re_field.search(line)
        if m:
            raw_type = m.group(1)
            field_type = apply_fixups(raw_type)
            field_name = m.group(2)
            offset = int(m.group(3), 16)
            size = int(m.group(4), 16)

            entry = {
                "Name": field_name,
                "TypeName": field_type,
                "Offset": offset,
                "Size": size,
                "MaxCount": 1,
            }

            instance_fields.append(entry)

    if instance_fields:
        result["HasData"] = True
        result["Data"] = instance_fields

    return result


def main():
    if len(sys.argv) < 3:
        print(
            "Usage: python message_yaml_generator.py <input_dwarf_dump.txt> <output.yml>"
        )
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    if not os.path.exists(input_file):
        print(f"Error: input file '{input_file}' does not exist.")
        sys.exit(1)

    text = open(input_file, "r", encoding="utf-8", errors="ignore").readlines()
    structs = extract_struct_blocks(text)

    messages = []
    seen_names = set()

    for struct in structs:
        lines = [l for l in struct.split("\n") if l.strip()]
        if not lines:
            continue
        if "public Message" not in struct:
            continue
        parsed = parse_struct(lines)
        if (
            parsed["Name"].startswith("M")
            and parsed["Name"]
            and parsed["Name"] not in seen_names
        ):
            messages.append(parsed)
            seen_names.add(parsed["Name"])

    with open(output_file, "w") as f:
        yaml.dump({"Messages": messages}, f, sort_keys=False)

    print(f"Wrote {len(messages)} messages to {output_file}")


if __name__ == "__main__":
    main()
