#!/usr/bin/env python3
"""
event_yaml_generator.py

Generate the events.yml (new format) from dtk dwarf dump that contains nested structs.

Usage:
    python event_yaml_generator.py <input_dwarf_dump.txt> <output.yml>
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
re_event = re.compile(r"struct\s+(\w+)\s*:\s*public\s+Event")
re_total_size = re.compile(r"total size:\s*0x([0-9A-Fa-f]+)")
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


def parse_struct(lines):
    result = {
        "Name": None,
        "ID": 0,
        "Size": 0,
        "HasStaticData": False,
        "HasExtraData": False,
        "StaticDataSize": 0,
        "StaticData": [],
        "ExtraData": [],
    }

    # Identify event name + total instance size
    for line in lines:
        m = re_event.search(line)
        if m:
            result["Name"] = m.group(1)

        m = re_total_size.search(line)
        if m and result["Size"] == 0:
            result["Size"] = int(m.group(1), 16)
            break

    inside_static = False
    static_struct_declared = False

    static_fields = []
    instance_fields = []

    for line in lines:
        # Start of StaticData struct
        if "struct StaticData" in line and "public StaticData" in line:
            inside_static = True
            static_struct_declared = True
            continue

        # End of StaticData struct
        if inside_static and line.strip().startswith("};"):
            inside_static = False

        # StaticData total size
        if inside_static:
            m = re_total_size.search(line)
            if m:
                result["StaticDataSize"] = int(m.group(1), 16)
                continue

        # Field
        m = re_field.search(line)
        if m:
            raw_type = m.group(1)
            field_type = apply_fixups(raw_type)
            field_name = m.group(2)
            # -4 to not store it in depending on the parent StaticData
            offset = int(m.group(3), 16) - 4
            size = int(m.group(4), 16)

            entry = {
                "Name": field_name,
                "TypeName": field_type,
                "Offset": offset,
                "Size": size,
                "MaxCount": 1,
            }

            if inside_static:
                static_fields.append(entry)
            else:
                instance_fields.append(entry)

    # Assign StaticData
    if static_struct_declared:  # track if StaticData struct was found
        result["HasStaticData"] = True
        result["StaticData"] = static_fields  # may be empty

    # ExtraData = instance fields not in StaticData
    static_names = {f["Name"] for f in static_fields}
    extra = [f for f in instance_fields if f["Name"] not in static_names]

    if extra:
        result["HasExtraData"] = True
        result["ExtraData"] = extra

    return result


def main():
    if len(sys.argv) < 3:
        print(
            "Usage: python event_yaml_generator.py <input_dwarf_dump.txt> <output.yml>"
        )
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    if not os.path.exists(input_file):
        print(f"Error: input file '{input_file}' does not exist.")
        sys.exit(1)

    text = open(input_file, "r", encoding="utf-8", errors="ignore").read()
    blocks = text.split("\n\n")

    events = []
    seen_names = set()

    for b in blocks:
        lines = [l for l in b.split("\n") if l.strip()]
        if not lines:
            continue
        if "public Event" not in b:
            continue

        parsed = parse_struct(lines)
        if parsed["Name"] and parsed["Name"] not in seen_names:
            events.append(parsed)
            seen_names.add(parsed["Name"])

    with open(output_file, "w") as f:
        yaml.dump({"Events": events}, f, sort_keys=False)

    print(f"Wrote {len(events)} events to {output_file}")


if __name__ == "__main__":
    main()
