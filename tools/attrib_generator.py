# -------------------------------------------------------------------------------
# Generate AttribSys class header files from info.yml
# -------------------------------------------------------------------------------
import sys
import os
import yaml

FILE_PROLOGUE = """
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

"""

FILE_EPILOGUE = """
} // namespace Gen
} // namespace Attrib

#endif
"""


def sort_by_offset(field):
    return field["Offset"]


type_replacement = {
    "Attrib::RefSpec": "RefSpec",
    "Attrib::Types::Vector2": "UMath::Vector2",
    "Attrib::Types::Vector3": "UMath::Vector3",
    "Attrib::Types::Vector4": "UMath::Vector4",
    "EA::Reflection::Float": "float",
    "EA::Reflection::UInt32": "unsigned int",
    "EA::Reflection::Int32": "int",
    "EA::Reflection::UInt16": "unsigned short",
    "EA::Reflection::Int16": "short",
    "EA::Reflection::UInt8": "unsigned char",
    "EA::Reflection::Int8": "char",
    "EA::Reflection::Bool": "bool",
}


def get_layout_struct_field(field):
    out = ""
    field_name = field["Name"]
    type_name = field["TypeName"]
    count = field["MaxCount"]
    offset = field["Offset"]
    size = field["Size"]
    is_array = False

    if "Array" in field["Flags"]:
        is_array = True
        out += f"Private _Array_{field_name};"
        out += f" // offset {hex(offset)}, size {hex(8)}\n"

    if type_name == "EA::Reflection::Text":
        out += f"char {field_name}[{size}];"
    else:
        type = type_replacement.get(type_name, type_name)
        out += f"{type} {field_name}{'' if count == 1 else f'[{count}]'};"

    if is_array:
        out += f" // offset {hex(offset + 8)}, size {hex(size * count)}\n"
    else:
        out += f" // offset {hex(offset)}, size {hex(size)}\n"

    return out


def get_getter(field):
    out = ""
    field_name = field["Name"]
    type_name = field["TypeName"]

    if "Array" in field["Flags"]:
        pass  # TODO
    else:
        if type_name == "EA::Reflection::Text":
            out += f"""const char*{field_name}() const {{
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->{field_name};
}}

"""
        else:
            type = type_replacement.get(type_name, type_name)
            out += f"""{type} &{field_name}() const {{
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->{field_name};
}}

"""

    return out


def process_file(filename, outdirectory):
    print("Processing file:", filename)
    with open(filename, "rb") as f:
        data = yaml.safe_load(f)
        for clazz in data["Classes"]:
            name: str = clazz["Name"]
            out = f"#ifndef ATTRIBSYS_CLASSES_{name.upper()}_H\n#define ATTRIBSYS_CLASSES_{name.upper()}_H\n\n"
            out += FILE_PROLOGUE

            out += f"struct {name} : Instance {{\n"
            out += "struct _LayoutStruct {\n"

            fields = sorted(clazz["Fields"], key=sort_by_offset)

            for field in fields:
                if "InLayout" not in field["Flags"]:
                    continue

                out += get_layout_struct_field(field)

            out += "};\n\n"

            out += f"""void operator delete(void *ptr, size_t bytes) {{
    Attrib::Free(ptr, bytes, "{name}");
}}

{name}(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {{
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}}

~{name}() {{}}

"""

            for field in fields:
                if "InLayout" not in field["Flags"]:
                    continue

                out += get_getter(field)

            out += "};\n"
            out += FILE_EPILOGUE

            with open(os.path.join(outdirectory, f"{name}.h"), "w") as outfile:
                outfile.writelines(out)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Expected usage: {0} <input file> <output directory>".format(sys.argv[0]))
        sys.exit(1)
    process_file(sys.argv[1], sys.argv[2])
