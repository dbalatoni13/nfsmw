#!/usr/bin/env python3
"""
message_class_generator.py

Generate Message class header files from an messages.yml file (new format).

Usage:
    python message_class_generator.py <mssages.yml> <output_dir>
"""

import os
import sys
import yaml

# Simple type → include heuristics
INCLUDE_HERMES = "Speed/Indep/Src/Misc/Hermes.h"
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
    return "_" + base


def base_name_from_field(field_name: str) -> str:
    if not field_name:
        return "ERROR"
    if len(field_name) >= 2 and field_name[0] == "f":
        return field_name[1:]
    else:
        return field_name


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


def detect_includes(fields):
    includes = {INCLUDE_HERMES}
    includes |= detect_includes_from_fields(fields)
    return sorted(includes)


def generate_header_for_message(message: dict) -> str:
    name = message["Name"]

    has_fields = bool(message.get("HasData"))
    fields = message.get("Data") or []

    includes = detect_includes(fields)

    guard = f"GENERATED_MESSAGES_{name.upper()}_H"
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

    out.append(f"// total size: {hex(message['Size'])}")
    out.append(f"class {name} : public Hermes::Message {{")
    out.append("  public:")

    # Static functions
    out.append(f"    static std::size_t _GetSize() {{")
    out.append(f"        return sizeof({name});")
    out.append(f"    }}")
    out.append("")

    out.append(f"    static UCrc32 _GetKind() {{")
    out.append(f'        static UCrc32 k("{name}");')
    out.append("")
    out.append(f"        return k;")
    out.append(f"    }}")
    out.append("")

    # Constructor parameters
    ctor_params = []
    initializer_list = []

    for f in fields:
        param_name = param_name_from_field(f["Name"])
        ctor_params.append(f"{safe_type(f['TypeName'])} {param_name}")
        initializer_list.append(f"{f['Name']}({param_name})")

    param_list = ", ".join(ctor_params)
    initializer_list = ", ".join(initializer_list)
    out.append(
        f"    {name}({param_list}) : Hermes::Message(_GetKind(), _GetSize(), 0), {initializer_list} {{}}"
        if param_list
        else f"    {name}() : Hermes::Message(_GetKind(), _GetSize(), 0) {{}}"
    )
    out.append("")

    out.append(f"    ~{name}() {{}}")
    out.append("")

    if fields:
        for f in fields:
            f_type = safe_type(f["TypeName"])
            base_name = base_name_from_field(f["Name"])
            # Getter
            out.append(f"    {f_type} Get{base_name}() const {{")
            out.append(f"       return {f['Name']};")
            out.append(f"    }}")
            out.append("")

            # Setter
            param_name = param_name_from_field(f["Name"])
            out.append(f"    void Set{base_name}({f_type} {param_name}) {{")
            out.append(f"       {f['Name']} = {param_name};")
            out.append(f"    }}")
            out.append("")

    if fields:
        out.append("  private:")

    # Private Data fields appended at the end
    if fields:
        for f in fields:
            offset = f["Offset"]
            out.append(
                f"    {safe_type(f['TypeName'])} {f['Name']}; // offset {hex(offset + 8)}, size {hex(f['Size'])}"
            )

    out.append("};")
    out.append("")

    out.append("#endif")
    out.append("")

    return "\n".join(out)


def process_file(filename: str, outdirectory: str):
    os.makedirs(outdirectory, exist_ok=True)

    with open(filename, "rb") as f:
        data = yaml.safe_load(f)

    for message in data.get("Messages", []):
        header = generate_header_for_message(message)
        out_path = os.path.join(outdirectory, f"{message['Name']}.hpp")
        with open(out_path, "w", newline="\n") as f:
            f.write(header)
        print("Wrote", out_path)


def main():
    if len(sys.argv) < 3:
        print("Usage: {} <messages.yml> <output_dir>".format(sys.argv[0]))
        sys.exit(1)

    process_file(sys.argv[1], sys.argv[2])


if __name__ == "__main__":
    main()
