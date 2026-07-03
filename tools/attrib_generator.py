# -------------------------------------------------------------------------------
# Generate AttribSys class header files from info.yml and a txt file that contains names in a <hex key> - <string> format
# -------------------------------------------------------------------------------
import sys
import os
import yaml

FILE_PROLOGUE = """
#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

"""


def sort_by_offset(field):
    return field["Offset"]


type_replacement = {
    "Attrib::RefSpec": "RefSpec",
    "Attrib::Types::Vector2": "UMath::Vector2",
    "Attrib::Types::Vector3": "UMath::Vector3",
    "Attrib::Types::Vector4": "UMath::Vector4",
    "Attrib::Types::Matrix": "UMath::Matrix4",
}


def get_layout_struct_field(field):
    out = ""
    field_name = field["Name"]
    type_name_raw = field["TypeName"]
    count = field["MaxCount"]
    offset = field["Offset"]
    # TODO override because of the different alignment on GC
    size = field["Size"]
    is_array = "Array" in field["Flags"]
    alignment = field["Alignment"]

    if is_array:
        out += f"Private _Array_{field_name};"
        out += f" // offset {hex(offset)}, size {hex(8)}\n"

    type_name = type_replacement.get(type_name_raw, type_name_raw)
    
    real_offset = offset
    if is_array:
        # 8 is the size of "Private"
        real_offset += 8
        size *= count

        start_of_array = real_offset
        should_pad = (start_of_array % alignment) != 0
        if should_pad:
            aligned = (start_of_array + 15) & ~15
            pad_amount = aligned - 8
            real_offset += pad_amount
            out += f"char _Pad_{field_name}[{pad_amount}]; // offset {hex(start_of_array)}, size {hex(pad_amount)}\n"
    out += f"{type_name} {field_name}{'' if count == 1 else f'[{count}]'};"
    out += f" // offset {hex(real_offset)}, size {hex(size)}\n"

    return out


def get_layout_getters(field, key):
    out = ""
    field_name = field["Name"]
    raw_type_name = field["TypeName"]
    type_name = type_replacement.get(raw_type_name, raw_type_name)
    is_layout = "InLayout" in field["Flags"]
    is_array = "Array" in field["Flags"]

    out += f"""bool {field_name}(TAttrib<{type_name}> &result) const {{
    ATTRIB_CODEGEN_GETATTRIB({type_name}, {key});
}}
"""

    # Checked get
    if is_layout:
        if is_array:
            out += f"""bool {field_name}({type_name} &result, unsigned int index) const {{
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED({field_name}, result, index);
    }}
"""
        else:
            out += f"""bool {field_name}({type_name} &result) const {{
    ATTRIB_CODEGEN_CHECKEDGETLAYOUT({field_name}, result);
}}
"""
    else:
        if is_array:
            out += f"""bool {field_name}({type_name} &result, unsigned int index) const {{
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED({type_name}, {key}, result, index);
}}
"""
        else:
            out += f"""bool {field_name}({type_name} &result) const {{
    ATTRIB_CODEGEN_CHECKEDGETVALUE({type_name}, {key}, result);
}}
"""
    # Normal get
    if is_layout:
        if is_array:
            out += f"""const {type_name} &{field_name}(unsigned int index) const {{
    ATTRIB_CODEGEN_GETLAYOUTINDEXED({type_name}, {field_name}, index);
}}
"""
        else:
            out += f"""const {type_name} &{field_name}() const {{
    ATTRIB_CODEGEN_GETLAYOUT({field_name});
}}
"""
    else:
        out += f"""const {type_name} &{field_name}({"unsigned int index" if is_array else ""}) const {{
    {f"ATTRIB_CODEGEN_GETVALUEINDEXED({type_name}, {key}, index)" if is_array else f"ATTRIB_CODEGEN_GETVALUE({type_name}, {key})"};
}}      
"""

    # Num
    if is_array:
        out += f"""unsigned int Num_{field_name}() const {{
    {f"ATTRIB_CODEGEN_GETLAYOUTLENGTH({field_name})" if is_layout else f"ATTRIB_CODEGEN_GETLENGTH({key})"};
}}
"""

    # Setters
    if is_array:
        # TODO
        out += f"""bool SET_{field_name}(const {type_name} &input, unsigned int index) {{
    {f"ATTRIB_CODEGEN_SETLAYOUTINDEXED({field_name}, input, index)" if is_layout else f"ATTRIB_CODEGEN_SETVALUEINDEXED({type_name}, {key}, input, index)"};
}}
"""
    else:
        pass
        out += f"""bool SET_{field_name}(const {type_name} &input) {{
    {f"ATTRIB_CODEGEN_SETLAYOUT({field_name}, input)" if is_layout else f"ATTRIB_CODEGEN_SETVALUE({type_name}, {key}, input)"};
}}
"""

    return out


def process_file(filename, strings_file, outdirectory):
    print("Processing file:", filename)
    strToKey = {}
    with open(strings_file, "r") as f:
        for line in f.readlines():
            split = line.strip().split(" - ")
            strToKey[split[1]] = split[0]

    with open(filename, "rb") as f:
        data = yaml.safe_load(f)
        for clazz in data["Classes"]:
            name: str = clazz["Name"]
            has_layout_struct = clazz["LayoutSize"] > 0
            constructor_body = (
                "SetDefaultLayout(sizeof(_LayoutStruct));" if has_layout_struct else ""
            )
            out = f"#ifndef _attrib_gen_{name}_h\n#define _attrib_gen_{name}_h\n\n"
            out += FILE_PROLOGUE

            out += f"struct {name} : Instance {{\n"
            if has_layout_struct:
                out += "struct _LayoutStruct {\n"

            fields = sorted(clazz["Fields"], key=sort_by_offset)

            for field in fields:
                if "InLayout" not in field["Flags"]:
                    continue

                out += get_layout_struct_field(field)

            if has_layout_struct:
                out += "};\n\n"

            sorted_fields = sorted(fields, key=lambda item: item["Name"])

            for field in sorted_fields:
                field_name = field["Name"]
                raw_type_name = field["TypeName"]
                type_name = type_replacement.get(raw_type_name, raw_type_name)
                out += f"typedef {type_name} TypeOf_{field_name};\n"

            out += "\n"
            out += f"""static Key ClassKey();
USE_ATTRIB_ALLOC("{name}");
{name}(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {{
    {constructor_body}
}}
{name}(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {{
    {constructor_body}
}}
{name}(const Instance &src) : Instance(src) {{
    {constructor_body}
}}
{name}(const {name} &src) : Instance(src) {{
    {constructor_body}
}}
{name}(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {{
    {constructor_body}
}}
~{name}() {{}}
Instance &GetBase() {{
    return *this;
}}
const Instance &GetBase() const {{
    return *this;
}}
Key GetClass() {{
    return {strToKey[name]};
}}
void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {{
    ModifyInternal({strToKey[name]}, dynamicCollectionKey, spaceForAdditionalAttributes);
}}
Key GenerateUniqueKey(const char *name, bool registerName) const {{
    return GenerateUniqueKey(name, registerName);
}}
void Change(const Collection *c) {{
    Instance::Change(c);
}}
void Change(const RefSpec &refspec) {{
    Instance::Change(refspec);
}}
void Change(Key collectionkey) {{
    Change(FindCollection(ClassKey(), collectionkey));
}}
void ChangeWithDefault(const RefSpec &refspec) {{
    Instance::ChangeWithDefault(refspec);
}}
void ChangeWithDefault(Key collectionkey) {{
    Change(FindCollectionWithDefault(ClassKey(), collectionkey));
}}
const {name} &operator=(const {name} &rhs) {{
    operator=(rhs.GetBase());
    return *this;
}}
const {name} &operator=(const Instance &rhs) {{
    Instance::operator=(rhs);
    return *this;
}}
"""
            for field in sorted_fields:
                try:
                    key = strToKey[field["Name"]]
                    out += get_layout_getters(field, key)
                except KeyError:
                    print(
                        f"Couldn't find key for field {field['Name']} in class {name}"
                    )

            out += f"""private:
unsigned int GetLayoutSize() {{
    return {"sizeof(_LayoutStruct)" if has_layout_struct else "0"};
}}
{name} &ConvertFromInstance(Instance &src) {{}}
const {name} &ConvertFromInstance(const Instance &src) {{}}"""

            out += "};\n"
            out += "}; // namespace Gen\n\n"
            out += "namespace ClassName {\n\n"
            out += f"static const Key {name} = {strToKey[name]};\n\n"
            out += "}; // namespace ClassName\n\n"

            out += "namespace Hash {\n"
            out += f"namespace {name} {{\n\n"

            for field in sorted_fields:
                try:
                    field_name = field["Name"]
                    key = strToKey[field["Name"]]
                    out += f"static const Key {field_name} = {key};\n"
                except KeyError:
                    print(
                        f"Couldn't find key for field {field['Name']} in class {name}"
                    )

            out += f"""\n}}; // namespace {name}
}}; // namespace Hash

inline Key Gen::{name}::ClassKey() {{
    return ClassName::{name};
}}

}}; // namespace Attrib

#endif
"""
            os.makedirs(outdirectory, exist_ok=True)
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
