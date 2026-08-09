#!/usr/bin/env python3
"""
event_yaml_generator.py

Generate the events.yml (new format) from MW's dtk dwarf dumps that contains nested structs. IDs are pulled from Carbon.

Usage:
    python event_yaml_generator.py <input_mw_dwarf_dump.txt> <input_carbon_dwarf_dump.txt> <output.yml>
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
re_total_size = re.compile(r"^\s*//\s*total size:\s*0x([0-9A-Fa-f]+)")
re_field = re.compile(
    r"^\s*(?:struct\s+|enum\s+)?([\w:<>_]+(?:\s+[\w:<>_]+)*\s*\*?)\s+(\w+);\s*//\s*offset\s*0x([0-9A-Fa-f]+),\s*size\s*0x([0-9A-Fa-f]+)"
)
re_event_id = re.compile(
    r"const uint32_t kEventID = (\d+); // size.*, Decl: .*[eE]vents/(.*)\.h:"
)


def apply_fixups(type_name: str) -> str:
    t = type_name.strip()
    if t.startswith("enum "):
        t = t[len("enum ") :].strip()
    if t.startswith("struct "):
        t = t[len("struct ") :].strip()
    return type_fixups.get(t, t)


def parse_event_ids(lines):
    result = {}

    for match in re.finditer(re_event_id, lines):
        id = int(match.group(1))
        class_name = match.group(2)

        result[class_name] = id

    return result

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
        "ID": 0,
        "Size": 0,
        "HasStaticData": False,
        "HasExtraData": False,
        "StaticDataSize": 0,
        "StaticData": [],
        "ExtraData": [],
    }

    offset = 0

    # Identify event name + total instance size
    for line in lines:
        offset += 1
        m = re_total_size.search(line)
        if m and result["Size"] == 0:
            result["Size"] = int(m.group(1), 16)

        m = re_event.search(line)
        if m:
            result["Name"] = m.group(1)
            break

    inside_static = False
    static_struct_declared = False

    static_fields = []
    instance_fields = []

    for line in lines[offset:]:
        # Start of StaticData struct
        m = re_total_size.search(line)
        if m:
            inside_static = True
            size = int(m.group(1), 16)
            static_struct_declared = True
            result["StaticDataSize"] = size
            continue

        # End of StaticData struct
        if inside_static and line.strip().startswith("};"):
            inside_static = False

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
    if len(sys.argv) < 4:
        print(
            "Usage: python event_yaml_generator.py <input_dwarf_dump.txt> <input_carbon_dwarf_dump.txt> <output.yml>"
        )
        sys.exit(1)

    mw_input_file = sys.argv[1]
    carbon_input_file = sys.argv[2]
    output_file = sys.argv[3]

    if not os.path.exists(mw_input_file):
        print(f"Error: input file '{mw_input_file}' does not exist.")
        sys.exit(1)

    if not os.path.exists(carbon_input_file):
        print(f"Error: input file '{carbon_input_file}' does not exist.")
        sys.exit(1)

    carbon_text = open(carbon_input_file, "r", encoding="utf-8", errors="ignore").read()
    event_to_id_map = parse_event_ids(carbon_text)
    print(carbon_text[3])

    mw_text = open(mw_input_file, "r", encoding="utf-8", errors="ignore").readlines()
    structs = extract_struct_blocks(mw_text)

    events = []
    seen_names = set()

    for b in structs:
        lines = [l for l in b.split("\n") if l.strip()]
        if not lines:
            continue
        if "public Event" not in b:
            continue

        parsed = parse_struct(lines)
        if parsed["Name"] and parsed["Name"] not in seen_names:
            if parsed["Name"] in event_to_id_map:
                parsed["ID"] = event_to_id_map[parsed["Name"]]
            events.append(parsed)
            seen_names.add(parsed["Name"])

    with open(output_file, "w") as f:
        yaml.dump({"Events": events}, f, sort_keys=False)

    print(f"Wrote {len(events)} events to {output_file}")


if __name__ == "__main__":
    main()
