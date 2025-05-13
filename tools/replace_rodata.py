import sys
import os
import regex as re
from defines import DATADIR, WIPE_TYPE, WIPE_MODE, MOTION_ATTR, ATTR, SPRITE_ATTR


def rc(s: str) -> str:
    return s.replace(",", "")


def join(*args) -> str:
    return " ".join(args)


# def replace_rodata(file_content: str, name_line: str, value_line: str):
#     variable_name = name_line.strip().removeprefix(".endobj ").split(",")[0]
#     type, value = value_line.strip().split(" ")[:2]

#     if type not in (".float", ".double"):
#         return file_content
#     if type == ".float":
#         value = value + "f" if "." in value else value + ".0f"
#     else:
#         value = value if "." in value else value + ".0"

#     return re.sub(
#         r"\b%s\b" % variable_name, value, file_content
#     )  # replace only whole words


def replace_double_var_loops(code: str) -> str:
    """
    Where only var2 is bound checked
    """
    loop_pattern = re.compile(
        # r"([0-9a-z_]+) = ([0-9a-z&\._]+);\n\s*([0-9a-z_]+) = ([0-9a-z&\._]+);\n\s*loop_([0-9]+):\n\s*if \(\3 < (\(?.*?\)?)\) {([\s\S]*?)\n\s*(\3.*?);\n\s*(\1.*?);\n\s*goto loop_\5;\n\s+}" # works
        r"([0-9a-z_]+) = (.+?);\n\s*?([0-9a-z_]+) = (.+?);\n\s*?loop_([0-9]+):\n\s*?if \(\3 (?:<|>) (\(?.*?\)?)\) {((?:(?!goto loop_\3)[\s\S])*?)\n\s*?(\3.*?);\n\s*?(\1.*?);\n\s*?goto loop_\5;\n\s+?}"
    )

    def replace(match: re.Match[str]):
        loop_var1 = match.group(1)
        loop_var_val1 = match.group(2)
        loop_var2 = match.group(3)
        loop_var_val2 = match.group(4)
        loop_limit = match.group(6)
        loop_body = match.group(7).strip()
        loop_var2_change = match.group(8)
        loop_var1_change = match.group(9)

        # Recursively replace loops inside the loop body
        loop_body = replace_single_var_loops(loop_body)
        # TODO: loop_var2 < loop limit or loop_var1 < loop_limit?
        for_loop = f"for ({loop_var1} = {loop_var_val1}, {loop_var2} = {loop_var_val2}; {loop_var2} < {loop_limit}; {loop_var2_change}, {loop_var1_change}) {{\n    {loop_body}\n}}"

        return for_loop

    while re.search(loop_pattern, code):
        code = re.sub(loop_pattern, replace, code)

    return code


def replace_single_var_loops(code: str) -> str:
    loop_pattern = re.compile(
        #  r"([0-9a-z_]+) = ([0-9a-z&\._]+);\n\s*loop_([0-9]+):\n\s*if \(\1 < (\(?.*?\)?)\) {([\s\S]*?)\n\s*(\1.*?);\n\s*goto loop_\3;\n\s+}" # works
        # negative lookahead is required because loops might have the same name and if it doesn't match the closest, it might look further
        r"([0-9a-z_]+) = (.+?);\n\s*?loop_([0-9]+):\n\s*?if \(\1 (<|>|<=|=>) (\(?.*?\)?)\) \{((?:(?!goto loop_\3)[\s\S])*?)\n\s*?(\1.*?);\n\s*?goto loop_\3;\n\s+?\}"
    )

    def replace(match: re.Match[str]):
        loop_var = match.group(1)
        # print(f"Loop var: {loop_var}")
        loop_var_val = match.group(2)
        # print(f"Loop var val: {loop_var_val}")
        compare_operator = match.group(4)
        loop_limit = match.group(5)
        # print(f"Loop limit: {loop_limit}")
        loop_body = match.group(6).strip()
        # print(f"Loop body: {loop_body}")
        loop_var_change = match.group(7)

        # Recursively replace loops inside the loop body
        loop_body = replace_single_var_loops(loop_body)

        for_loop = f"for ({loop_var} = {loop_var_val}; {loop_var} {compare_operator} {loop_limit}; {loop_var_change}) {{\n    {loop_body}\n}}"
        return for_loop

    while re.search(loop_pattern, code):
        code = re.sub(loop_pattern, replace, code)

    return code


def parse_number(num: str) -> int:
    if num.startswith("0x"):
        return int(num, 16)
    else:
        return int(num)


def replace_make_num_values(code: str) -> str:
    num_pattern = re.compile(r"0x([0-9a-fA-F]{2})00([0-9a-fA-F]{2})")

    def replace(match: re.Match[str]):
        folder = parse_number("0x" + match.group(1))
        file = match.group(2)
        return f"DATA_MAKE_NUM(DATADIR_{DATADIR[folder]}, {file})"

    while re.search(num_pattern, code):
        try:
            code = re.sub(num_pattern, replace, code)
        except IndexError:
            pass

    return code


def replace_attr_values(code: str) -> str:
    hu3d_pattern = re.compile(
        r"(Hu3DMotionShiftSet|Hu3DModelAttrSet|Hu3DModelAttrReset)\((.*),\s?((?:0x)?[0-9a-fA-F]+)\);"
    )

    def replace_hu3d(match: re.Match[str]):
        function_name = match.group(1)
        rest = match.group(2)
        value = parse_number(match.group(3))

        array = MOTION_ATTR if value & 0x40000000 else ATTR
        if value == 0:
            res = [array[0]]
        else:
            res = [array[i] for i in range(1, len(array)) if value & (1 << (i - 1))]
        return f"{function_name}({rest}, {' | '.join(res)});"

    while re.search(hu3d_pattern, code):
        code = re.sub(hu3d_pattern, replace_hu3d, code)

    # sprite
    sprite_pattern = re.compile(
        r"(HuSprAttrSet|HuSprAttrReset|espAttrSet|espAttrReset)\((.*),\s?((?:0x)?[0-9a-fA-F]+)\);"
    )

    def replace_sprite(match: re.Match[str]):
        function_name = match.group(1)
        rest = match.group(2)
        value = parse_number(match.group(3))

        if value == -1:
            res = [SPRITE_ATTR[0]]
        else:
            res = []
            for i in range(1, len(SPRITE_ATTR)):
                if value & (1 << (i - 1)):
                    res.append(SPRITE_ATTR[i])
        return f"{function_name}({rest}, {' | '.join(res)});"

    while re.search(sprite_pattern, code):
        code = re.sub(sprite_pattern, replace_sprite, code)

    return code


def replace_wipe_values(code: str) -> str:
    wipe_pattern = re.compile(
        r"WipeCreate\((-?(?:0x)?[0-9a-fA-F]+),\s*(-?(?:0x)?[0-9a-fA-F]+),(.*)\);"
    )

    def replace(match: re.Match[str]):
        mode = parse_number(match.group(1))
        type = parse_number(match.group(2))
        duration = match.group(3)

        return f"WipeCreate({WIPE_MODE[mode - 1]}, {WIPE_TYPE[type + 1]},{duration});"

    while re.search(wipe_pattern, code):
        code = re.sub(wipe_pattern, replace, code)

    return code


def interpret_file(asm_file: str, c_file: str) -> None:
    asm_file = os.path.normpath(asm_file)
    c_file = os.path.normpath(c_file)

    f = open(asm_file)
    g = open(c_file, "r+")
    c_file_content = g.read()
    g.seek(0)

    # line = ""
    # for line in f:
    #     if line.startswith(".obj lbl_1_rodata_"):
    #         value_line = f.readline()

    #         # we only replace single values
    #         variable_name_line = line = f.readline()
    #         if variable_name_line.startswith(".endobj lbl_1_rodata_"):
    #             c_file_content = replace_rodata(
    #                 c_file_content, variable_name_line, value_line
    #             )

    c_file_content = (
        c_file_content.replace("f32", "float")
        .replace("f64", "double")
        .replace("Point3d", "Vec")
        .replace("sp08", "sp8")
        .replace("sp09", "sp9")
        .replace("sp0A", "spA")
        .replace("sp0B", "spB")
        .replace("sp0C", "spC")
        .replace("sp0D", "spD")
        .replace("sp0E", "spE")
        .replace("sp0F", "spF")
        .replace("/* irregular */", "")
        .replace("/* inferred */", "")
        .replace("/* fallthrough */", "")
        .replace("(bitwise float)", "")
        .replace("(bitwise s32)", "")
        .replace("(bitwise s64)", "")
        .replace("(bitwise double)", "")
        .replace("(bitwise double *)", "")
    )
    c_file_content = re.sub(
        r"(sin|cos)\(\(3\.141592653589793 \* (.*)\) / 180\.0\)",
        r"\1d(\2)",
        c_file_content,
    )
    c_file_content = re.sub(
        r"180\.0 \* \(atan2\((.*), (.*)\) / 3\.141592653589793\)",
        r"atan2d(\1,\2)",
        c_file_content,
    )
    c_file_content = re.sub(
        r"Hu3DModelCreate\(HuDataSelHeapReadNum\((.*), 0x10000000, HEAP_DATA\)\)",
        r"Hu3DModelCreateFile(\1)",
        c_file_content,
    )
    c_file_content = re.sub(
        r"HuSprAnimRead\(HuDataSelHeapReadNum\((.*), 0x10000000, HEAP_DATA\)\)",
        r"HuSprAnimReadFile(\1)",
        c_file_content,
    )
    c_file_content = re.sub(
        r"Hu3DJointMotion\((.*), HuDataSelHeapReadNum\((.*), 0x10000000, HEAP_DATA\)\)",
        r"Hu3DJointMotionFile(\1, \2)",
        c_file_content,
    )
    c_file_content = re.sub(
        r"HuMemDirectMallocNum\(HEAP_SYSTEM, (.*), 0x10000000\);",
        r"HuMemDirectMallocNum(HEAP_SYSTEM, \1, MEMORY_DEFAULT_NUM);",
        c_file_content,
    )
    c_file_content = re.sub(
        r"if \(_CheckFlag\(0x1000C\) == 0\) {\n\s*\(GWPlayer \+ \(([0-9a-zA-Z_]+) \* 0x30\)\)->unk_28 = ([0-9a-z_]+);\n\s*}",
        r"GWPlayerCoinWinSet(\1, \2);",
        c_file_content,
    )
    c_file_content = re.sub(
        r"struct ([_0-9a-zA-Z]+) {([\s\S]*?\n)};",
        r"typedef struct \1 { \2 } \1;",
        c_file_content,
    )
    c_file_content = re.sub(
        r"float \(\*([_0-9a-zA-Z]+)\)\[4\]", r"Mtx \1", c_file_content
    )
    c_file_content = re.sub(r"\(&(sp[_0-9a-zA-Z]+)\[\d\]\)", r"\1", c_file_content)
    c_file_content = re.sub(r"&(sp[_0-9a-zA-Z]+)\[\d\]", r"\1", c_file_content)
    c_file_content = re.sub(r"\(&(sp[_0-9a-zA-Z]+)\)", r"\1", c_file_content)
    # cast thing
    c_file_content = re.sub(
        r"\s*var_f([0-9]{1,2}) = var_f\1;$", "", c_file_content, flags=re.MULTILINE
    )
    c_file_content = re.sub(
        r"\*([_0-9a-zA-Z]+)->model", r"\1->model[0]", c_file_content
    )
    c_file_content = re.sub(
        r"\*([_0-9a-zA-Z]+)->motion", r"\1->motion[0]", c_file_content
    )

    c_file_content = re.sub(r"^static ", "", c_file_content, flags=re.MULTILINE)
    c_file_content = re.sub(r"\/\* switch \d+(; ?irregular)? \*\/", "", c_file_content)

    c_file_content = replace_attr_values(c_file_content)
    c_file_content = replace_wipe_values(c_file_content)
    # c_file_content = replace_make_num_values(c_file_content) # picks up too much
    c_file_content = replace_double_var_loops(c_file_content)
    c_file_content = replace_single_var_loops(c_file_content)
    # c_file_content = re.sub(
    #     r"([0-9a-z_]+) = ([0-9a-z_]+);\n\s*loop_([0-9]+):\n\s*if \(\1 < ([0-9a-z_]+)\) {(.*)\n\s*(\1.*);\n\s*goto loop_\3;\n\s+}",
    #     r"for (\1 = \2; \1 < \4; \6) {\5\n}",
    #     c_file_content,
    #     flags = re.S
    # )

    # c_file_content = re.sub(
    #     r"\*\(&([0-9a-z_]+[0-9A-F]+) \+ \((.*) \* \d+\)\)", r"\1[\2]", c_file_content
    # )

    # c_file_content = re.sub(r"([\w\d]+.*) ([\+\-])= 1", r"\1\2\2", c_file_content)
    c_file_content = re.sub(
        r"([\w\d]+.*) ([\+\-])= 1([;,)])", r"\1\2\2\3", c_file_content
    )
    c_file_content = re.sub(r"([\w\d]+.*) = \1 ([\+\-]) 1;", r"\1\2\2;", c_file_content)

    g.write(c_file_content)
    g.truncate()

    g.close()
    f.close()


try:
    interpret_file(sys.argv[1], sys.argv[2])
except Exception:
    if len(sys.argv) < 3:
        print("replace_rodata.py")
        print("Usage: python3 extract_struct.py [asm file] [c file]")
        print("Accepts all files that use the generated asm from ninja")
    else:
        raise Exception
