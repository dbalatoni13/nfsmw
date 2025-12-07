#!/usr/bin/env python3
"""
event_class_generator.py

Generate Event class header files from an events.yml file (new format).

Usage:
    python event_class_generator.py <events.yml> <output_dir>
"""

import os
import sys
import yaml

# Simple type → include heuristics
INCLUDE_EVENT = "Speed/Indep/Src/Main/Event.h"
INCLUDE_ISIMABLE = "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
INCLUDE_PHY = "Speed/Indep/Src/Physics/PhysicsTypes.h"
INCLUDE_SIM_COLLISION = "Speed/Indep/Src/Sim/Collision.h"
INCLUDE_UTYPES = "Speed/Indep/Libs/Support/Utility/UTypes.h"
INCLUDE_UCRC = "Speed/Indep/Libs/Support/Utility/UCrc.h"

TYPE_REPLACEMENTS = {
    # add custom mappings if needed
}


def safe_type(type_name: str) -> str:
    """Clean up TypeName (remove 'enum ' / 'struct ' prefixes, trim)."""
    if not isinstance(type_name, str):
        return str(type_name)
    t = type_name.strip()
    if t.startswith("enum "):
        t = t[len("enum ") :].strip()
    if t.startswith("struct "):
        t = t[len("struct ") :].strip()
    return TYPE_REPLACEMENTS.get(t, t)


def param_name_from_field(field_name: str) -> str:
    """
    Convert a field name into constructor parameter name.
    Rule: fSomething → pSomething, otherwise p<name>.
    """
    if not field_name:
        return "p"
    if len(field_name) >= 2 and field_name[0] == "f":
        base = field_name[1:]
    else:
        base = field_name
    return "p" + base


def detect_includes_from_fields(fields):
    includes = set()
    for f in fields or []:
        tn = (f.get("TypeName") or "").lower()
        if "hsimable" in tn:
            includes.add(INCLUDE_ISIMABLE)
        if any(k in tn for k in ("physics", "gear", "shift", "enum")):
            includes.add(INCLUDE_PHY)
        if "collision_info" in tn:
            includes.add(INCLUDE_SIM_COLLISION)
        if "umath::" in tn:
            includes.add(INCLUDE_UTYPES)
        if "ucrc32" in tn:
            includes.add(INCLUDE_UCRC)

    return includes


def detect_includes(static_fields, extra_fields):
    includes = {INCLUDE_EVENT}
    includes |= detect_includes_from_fields(static_fields)
    includes |= detect_includes_from_fields(extra_fields)
    return sorted(includes)


def generate_header_for_event(event: dict) -> str:
    name = event["Name"]

    has_static = bool(event.get("HasStaticData"))
    has_extra = bool(event.get("HasExtraData"))

    static_fields = event.get("StaticData") or []
    extra_fields = event.get("ExtraData") or []

    includes = detect_includes(static_fields, extra_fields)

    guard = f"EVENTS_{name.upper()}_H"
    out = []

    # Guard + pragma
    out.append(f"#ifndef {guard}")
    out.append(f"#define {guard}")
    out.append("")
    out.append("#ifdef EA_PRAGMA_ONCE_SUPPORTED")
    out.append("#pragma once")
    out.append("#endif")
    out.append("")

    for inc in includes:
        out.append(f'#include "{inc}"')
    out.append("")

    out.append(f"// total size: {hex(event['Size'])}")
    out.append(f"class {name} : public Event {{")
    out.append("  public:")

    # StaticData struct
    if has_static:
        out.append(f"    // total size: {hex(event['StaticDataSize'])}")
        out.append("    struct StaticData : public Event::StaticData {")
        for f in static_fields:
            offset = f["Offset"]
            tname = safe_type(f["TypeName"])
            out.append(
                f"        {tname} {f['Name']}; // offset {hex(offset + 4)}, size {hex(f['Size'])}"
            )
        out.append("    };")
        out.append("")

    # EventID comment
    if "ID" in event:
        out.append(f"    // enum {{ kEventID = {event['ID']} }};")
        out.append("")

    # Constructor parameters for BOTH static + extra fields
    ctor_params = []

    for f in static_fields:
        ctor_params.append(
            f"{safe_type(f['TypeName'])} {param_name_from_field(f['Name'])}"
        )

    for f in extra_fields:
        ctor_params.append(
            f"{safe_type(f['TypeName'])} {param_name_from_field(f['Name'])}"
        )

    param_list = ", ".join(ctor_params)
    out.append(f"    {name}({param_list});" if param_list else f"    {name}();")
    out.append("")

    out.append(f"    override virtual ~{name}();")
    out.append("")
    out.append("    override virtual const char *GetEventName() {")
    out.append(f'        return "{name}";')
    out.append("    }")

    if static_fields or extra_fields:
        out.append("")
        out.append("  private:")

    # Private StaticData fields
    for f in static_fields:
        offset = f["Offset"]
        out.append(
            f"    {safe_type(f['TypeName'])} {f['Name']}; // offset {hex(offset + 8)}, size {hex(f['Size'])}"
        )

    # Private ExtraData fields appended at the end
    if extra_fields:
        if static_fields:
            out.append("")
        for f in extra_fields:
            offset = f["Offset"]
            out.append(
                f"    {safe_type(f['TypeName'])} {f['Name']}; // offset {hex(offset + 8)}, size {hex(f['Size'])}"
            )

    out.append("};")
    out.append("")

    # Callback only if static data exists
    if has_static:
        out.append(f"void {name}_MakeEvent_Callback(const void *staticData);")
        out.append("")

    out.append("#endif")
    out.append("")

    return "\n".join(out)


def process_file(filename: str, outdirectory: str):
    os.makedirs(outdirectory, exist_ok=True)

    with open(filename, "rb") as f:
        data = yaml.safe_load(f)

    for event in data.get("Events", []):
        header = generate_header_for_event(event)
        out_path = os.path.join(outdirectory, f"{event['Name']}.hpp")
        with open(out_path, "w", newline="\n") as f:
            f.write(header)
        print("Wrote", out_path)


def main():
    if len(sys.argv) < 3:
        print("Usage: {} <events.yml> <output_dir>".format(sys.argv[0]))
        sys.exit(1)

    process_file(sys.argv[1], sys.argv[2])


if __name__ == "__main__":
    main()
