# -------------------------------------------------------------------------------
# Generate AttribSys class header files from info.yml and a txt file that contains names in a <hex hash> - <string> format
# -------------------------------------------------------------------------------
import sys
import os
import yaml

FILE_PROLOGUE = """
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
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
    # TODO override because of the different alignment on GC
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


def get_layout_getter(field):
    out = ""
    field_name = field["Name"]
    type_name = field["TypeName"]
    type = type_replacement.get(type_name, type_name)

    if "Array" in field["Flags"]:
        out += f"""const {type} &{field_name}(unsigned int index) const {{
            const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(GetLayoutPointer());
            if (index < lp->_Array_{field_name}.GetLength()) {{
            return lp->{field_name}[index];
        }} else {{
            return *reinterpret_cast<const {type} *>(DefaultDataArea(sizeof({type})));
        }}
        }}

        unsigned int Num_{field_name}() const {{
            return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->_Array_{field_name}.GetLength();
        }}
        
        """
    else:
        if type_name == "EA::Reflection::Text":
            out += f"""const char*{field_name}() const {{
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->{field_name};
}}

"""
        else:
            out += f"""const {type} &{field_name}() const {{
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->{field_name};
}}

"""

    return out


def get_non_layout_getter(field, hash):
    out = ""
    field_name = field["Name"]
    type_name = field["TypeName"]
    type = type_replacement.get(type_name, type_name)

    out += f"""const {type} &{field_name}(unsigned int index) const {{
        const {type} *resultptr = reinterpret_cast<const {type} *>(GetAttributePointer({hash}, index));
        if (!resultptr) {{
            resultptr = reinterpret_cast<const {type} *>(DefaultDataArea(sizeof({type})));
        }}
        return *resultptr;
    }}
        
"""

    if "Array" in field["Flags"]:
        out += f"""unsigned int Num_{field_name}() const {{
            return Get({hash}).GetLength();
        }}

"""

    return out


def process_file(filename, strings_file, outdirectory):
    print("Processing file:", filename)
    strToHash = {}
    with open(strings_file, "r") as f:
        for line in f.readlines():
            split = line.strip().split(" - ")
            strToHash[split[1]] = split[0]

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

{name}(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {{
    SetDefaultLayout(sizeof(_LayoutStruct));
}}

{name}(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {{
    SetDefaultLayout(sizeof(_LayoutStruct));
}}

{name}(const {name} &src) : Instance(src) {{
    SetDefaultLayout(sizeof(_LayoutStruct));
}}

~{name}() {{}}

void Change(const Collection *c) {{
    Instance::Change(c);
}}

void Change(Key collectionkey) {{
    Change(FindCollection(ClassKey(), collectionkey));
}}

static Key ClassKey() {{
    return {strToHash[name]};
}}

"""

            for field in fields:
                if "InLayout" in field["Flags"]:
                    out += get_layout_getter(field)
                else:
                    try:
                        hash = strToHash[field["Name"]]
                        out += get_non_layout_getter(field, hash)
                    except KeyError:
                        print(
                            f"Couldn't find hash for field {field['Name']} in class {name}"
                        )

            out += "};\n"
            out += FILE_EPILOGUE

            with open(os.path.join(outdirectory, f"{name}.h"), "w") as outfile:
                outfile.writelines(out)


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print(
            "Expected usage: {0} <vault yml file> <vault string file> <output directory>".format(
                sys.argv[0]
            )
        )
        sys.exit(1)
    process_file(sys.argv[1], sys.argv[2], sys.argv[3])
