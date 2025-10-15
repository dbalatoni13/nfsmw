# Context generator borrowed from https://github.com/Prakxo/ac-decomp/blob/master/tools/decompctx.py

# This script makes leaves most of the heavy lifting to pcpp which does preprocessing and expansion of files:
# https://github.com/ned14/pcpp
# To use it make sure you run 'pip install -r requirements.txt' or 'pip install pcpp'
import os
import re
import argparse

from io import StringIO
from pcpp import CmdPreprocessor
from pcpp.evaluator import Value
from contextlib import redirect_stdout
from typing import Optional

# Note: requires being in the same directory as defines_common.py
from defines_common import cflags_includes, VERSIONS

# region Regex Patterns
at_address_pattern = re.compile(
    r"(?:.*?)(?:[a-zA-Z_$][\w$]*\s*\*?\s[a-zA-Z_$][\w$]*)\s*((?:AT_ADDRESS|:)(?:\s*\(?\s*)(0x[0-9a-fA-F]+|[a-zA-Z_$][\w$]*)\)?);"
)
attribute_pattern = re.compile(r"(__attribute__)")
binary_literal_pattern = re.compile(r"\b(0b[01]+)\b")
# endregion

# region Defaults
default_arguments: list[str] = [
    # Strip out left-over whitespace
    "--compress",
    # Put a newline before each line directive
    # "--line-directive",
    # "\n#line",
]

default_defines: dict[str, str] = {
    "__PPCBROADWAY__": "1",
    "__PPCGECKO__": "1",
    "HX_WII": "1",
    "__GNUC__": "1",
    "SN_TARGET_NGC": "1",
    "__SN__": "1",
    "__IEEE_BIG_ENDIAN": "1",
    "__SIZE_TYPE__": "long unsigned int",
    "__PTRDIFF_TYPE__": "long int",
    "__cplusplus": "199804L",
    "__GNUG__": "1",
}

prodg_options: dict[str, bool] = {
    "little_endian": False,
    "wchar_type": True,
    "exceptions": False,
    "longlong": True,
}

passthrough_defines: list[str] = [
    # C/C++-dependent
    "__STDC__",
    "__STDC_VERSION__",
    # Game version defines
    *[f"VERSION_{version}" for version in VERSIONS],
    # Debug defines
    "NDEBUG",
    "MILO_DEBUG",
    # __option
    "__option",
    *prodg_options.keys(),
    # __declspec
    "__declspec",
    "section",
    "dllexport",
    "dllimport",
    "noreturn",
    "weak",
    # __attribute__
    "__attribute__",
    "aligned",
    "packed",
    "unused",
    "weak",
    "never_inline",
    "format",
    "constructor",
    "destructor",
    # STLport
    # Namespaces are excluded when __cplusplus is undefined, but because we
    # pass it through, pcpp never executes the define for _STLP_HAS_NO_NAMESPACES
    # My declares:
    "_USE_MATH_DEFINES",
    "GEKKO",
    "TARGET_GC",
]

src_dir = "src"
include_dir = "include"
cwd_dir = os.getcwd()
script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))
# Bring in include dirs from configure.py so we don't have to duplicate them here
default_include_directories: list[str] = [
    flag.strip("-i").lstrip() for flag in cflags_includes
]

default_output_filename = "ctx.c"
# endregion


# region ContextArguments
class ContextArguments:
    def __init__(self):
        self.preprocessor_arguments: list[str] = ["pcpp"]
        self.output_path: str = ""
        self.deps_path: Optional[str] = None

        self.strip_attributes: bool = False
        self.strip_at_address: bool = False
        self.convert_binary_literals: bool = False
        self.eval_mwcc_options: bool = False

        # Write initial parser
        parser = argparse.ArgumentParser(
            prog="Decomp Context Generator",
            description="Wrapper around pcpp which generates a context file usable with decomp.me, m2c, or Ghidra",
            add_help=False,
        )
        parser.add_argument(
            "c_file", nargs="?", help="File from which to create context"
        )
        parser.add_argument("-h", "-help", "--help", dest="help", action="store_true")
        parser.add_argument(
            "-D",
            dest="defines",
            metavar="macro[=val]",
            nargs=1,
            action="append",
            help="Predefine a macro [with the given value]",
        )
        parser.add_argument(
            "--strip-attributes",
            dest="strip_attributes",
            action="store_true",
            help="Strip __attribute__(()) directives",
        )
        parser.add_argument(
            "--strip-at-address",
            dest="strip_at_address",
            action="store_true",
            help="Strip AT_ADDRESS or `: 0x12345678` directives",
        )
        parser.add_argument(
            "--convert-binary-literals",
            dest="convert_binary_literals",
            action="store_true",
            help="Convert binary literals (0bxxxx) to decimal",
        )
        parser.add_argument(
            "--eval-code-macros",
            dest="eval_code_macros",
            action="store_true",
            help="Evaluate macros outside of preprocessor directives, and strip their definitions",
        )
        parser.add_argument(
            "--eval-mwcc-options",
            dest="eval_mwcc_options",
            action="store_true",
            help="Evaluate __option() macros, such as __option(longlong) or __option(wchar_type)",
        )
        parser.add_argument(
            "-d",
            dest="deps_path",
            action="store",
            help="Path to output list of included files to",
        )

        # For the output path, we either want to be explicit or relative, but not both
        output_target_group = parser.add_mutually_exclusive_group()
        output_target_group.add_argument(
            "-o",
            dest="output_path",
            action="store",
            help="Explicit path to output the context file to",
        )
        output_target_group.add_argument(
            "-r",
            "--relative",
            dest="relative",
            action="store_true",
            help="Generate context relative to the source file",
        )

        # When targeting a specific platform we want to only do one thing or another
        platform_target_group = parser.add_mutually_exclusive_group()
        platform_target_group.add_argument(
            "--m2c",
            dest="m2c",
            action="store_true",
            help="Generates an m2c-friendly file",
        )
        platform_target_group.add_argument(
            "--ghidra",
            dest="ghidra",
            action="store_true",
            help="Generates a Ghidra-friendly file",
        )

        # Parse the arguments
        parsed_args = parser.parse_known_args()
        known_args = parsed_args[0]

        # Check if we need to do further conversions after the file is preprocessed
        self.strip_at_address = (
            known_args.strip_at_address or known_args.ghidra or known_args.m2c
        )
        self.strip_attributes = (
            known_args.strip_attributes or known_args.ghidra or known_args.m2c
        )
        self.convert_binary_literals = (
            known_args.convert_binary_literals or known_args.ghidra
        )
        self.eval_macros = (
            known_args.eval_code_macros or known_args.m2c or known_args.ghidra
        )
        self.eval_mwcc_options = (
            known_args.eval_mwcc_options or known_args.ghidra or known_args.m2c
        )

        if known_args.help or not known_args.c_file:
            # Since this script acts as a wrapper for the main pcpp script
            # we want to manually display the help and pass it through to the
            # pcpp preprocessor to show its full list of arguments
            parser.print_help()
            self.preprocessor_arguments.append("--help")
            CmdPreprocessor(self.preprocessor_arguments)
            exit(0)

        # Determine output path
        if known_args.output_path:
            self.output_path = known_args.output_path
        elif known_args.relative:
            self.output_path = f"{known_args.c_file}.ctx"
        else:
            self.output_path = os.path.join(os.getcwd(), default_output_filename)

        self.deps_path = known_args.deps_path

        # Append in the default include directories
        include_directories: list[str] = []
        include_directories.extend(default_include_directories)

        for include_directory in include_directories:
            self.preprocessor_arguments.extend(("-I", include_directory))

        # Check if we have any passed in defines
        include_defines = []
        known_defines: list[str] = []
        if known_args.defines:
            argument_defines = [x[0] for x in known_args.defines]
            for define in argument_defines:
                include_defines.append(define)
                known_defines.append(define.split("=")[0])

        # Add in the default defines unless explicitly passed in as arguments
        for default_define, default_define_value in default_defines.items():
            if default_define in known_defines:
                continue
            define_str: str = default_define + "=" + default_define_value
            include_defines.append(define_str)

        # Add the defines to the arguments
        for define in include_defines:
            self.preprocessor_arguments.extend(("-D", define))

        # Preserve macros in code if desired
        if not self.eval_macros:
            self.preprocessor_arguments.append("--passthru-defines")

        # Add other default arguments
        self.preprocessor_arguments.extend(default_arguments)

        # Add unknown arguments and pass them to pcpp
        pass_through_args = parsed_args[1]
        self.preprocessor_arguments.extend(pass_through_args)

        # Add the file we want to read
        self.preprocessor_arguments.append(known_args.c_file)


# endregion


# region ContextPreprocessor
class ContextPreprocessor(CmdPreprocessor):
    def __init__(self, args: ContextArguments):
        self.context_args = args
        self.in_directive = False
        self.include_deps: list[str] = []
        super(ContextPreprocessor, self).__init__(args.preprocessor_arguments)

    def on_include_not_found(
        self, is_malformed, is_system_include, curdir, includepath
    ):
        # Fixup for files that use <> for relative includes,
        # since pcpp doesn't seem to handle those
        if not is_malformed and os.path.exists(os.path.join(curdir, includepath)):
            # Need to return the directory to search in, not the path to the file,
            # otherwise it gets stuck in an infinite loop
            return curdir

        return super(ContextPreprocessor, self).on_include_not_found(
            is_malformed, is_system_include, curdir, includepath
        )

    def on_unknown_macro_in_expr(self, ident):  # type: ignore
        if self.context_args.eval_mwcc_options and ident in prodg_options:
            return 1 if prodg_options[ident] else 0

        if ident in passthrough_defines:
            return None
        return super(ContextPreprocessor, self).on_unknown_macro_in_expr(ident)

    def on_unknown_macro_in_defined_expr(self, tok):
        if tok.value in passthrough_defines:
            return None
        return super(ContextPreprocessor, self).on_unknown_macro_in_defined_expr(tok)

    def on_unknown_macro_function_in_expr(self, ident):  # type: ignore
        def mwcc_option(tokens):
            assert isinstance(tokens, Value), "Unrecognized token type"
            if tokens.exception is not None:
                return None
            return tokens.value()

        def warn_if_arg_expanded(tokens):
            assert isinstance(tokens, Value), "Unrecognized token type"
            if tokens.exception is None and tokens.value() == 0:
                self.on_error(
                    self.source,
                    -1,
                    f"Unhandled argument to {ident} built-in macro (real line number below)",
                )

            # This return value causes an assert, which will be caught and
            # results in a log with the correct line number for the above error
            return None

        if self.context_args.eval_mwcc_options and ident == "__option":
            return mwcc_option
        elif ident in passthrough_defines:
            return warn_if_arg_expanded
        return super(ContextPreprocessor, self).on_unknown_macro_function_in_expr(ident)

    def expand_macros(self, tokens, expanding_from=[]):
        # Don't expand outside of directives
        if not self.context_args.eval_macros and not self.in_directive:
            return tokens

        # Expand first before exiting the directive, since this is called recursively
        ret = super(ContextPreprocessor, self).expand_macros(tokens, expanding_from)
        self.in_directive = False
        return ret

    def evalexpr(self, tokens):
        # Inside an #if or #elif directive
        self.in_directive = True
        return super(ContextPreprocessor, self).evalexpr(tokens)

    def include(self, tokens, original_line):
        # Inside an #include directive
        self.in_directive = True
        return super(ContextPreprocessor, self).include(tokens, original_line)

    def on_file_open(self, is_system_include, includepath):
        # Open before adding, as it may not be a valid path and will raise an error
        ret = super(ContextPreprocessor, self).on_file_open(
            is_system_include, includepath
        )
        # Only successfully opened files will reach this point and be added
        self.include_deps.append(includepath)
        return ret


# endregion


# region Attribute Stripping
def strip_attributes(text_to_strip: str) -> str:
    if not text_to_strip:
        return text_to_strip

    attribute_matches = reversed(list(re.finditer(attribute_pattern, text_to_strip)))
    for attribute_match in attribute_matches:
        # Find the end index of the second double paranthesis
        paren_count = 0

        match_span = attribute_match.span(0)
        end_index = match_span[1]
        attribute_opened = False
        while end_index < len(text_to_strip):
            if text_to_strip[end_index] == "(":
                paren_count += 1

                if paren_count == 2:
                    attribute_opened = True

            if text_to_strip[end_index] == ")":
                paren_count -= 1

            if attribute_opened and paren_count == 0:
                end_index += 1
                break

            end_index += 1

        # Create the substring
        start_index = match_span[0]
        prefix = text_to_strip[0:start_index]
        postfix = text_to_strip[end_index : len(text_to_strip)]
        text_to_strip = prefix + postfix

    return text_to_strip


# endregion


# region At Address Stripping
def strip_at_address(text_to_strip: str) -> str:
    if not text_to_strip:
        return text_to_strip

    at_address_matches = reversed(list(re.finditer(at_address_pattern, text_to_strip)))
    for attribute_match in at_address_matches:
        # Create the substring
        match_span = attribute_match.span(1)
        start_index = match_span[0]
        end_index = match_span[1]
        prefix = text_to_strip[0:start_index]
        postfix = text_to_strip[end_index : len(text_to_strip)]
        text_to_strip = prefix + postfix

    return text_to_strip


# endregion


# region Binary Literal Conversion
def strip_binary_literals(text_to_strip: str) -> str:
    if not text_to_strip:
        return text_to_strip

    binary_literal_matches = reversed(
        list(re.finditer(binary_literal_pattern, text_to_strip))
    )
    for binary_literal_match in binary_literal_matches:
        # Create the substring
        match_span = binary_literal_match.span(1)
        start_index = match_span[0]
        end_index = match_span[1]

        # Convert from binary literal format to regular int
        binary_converted = int(text_to_strip[start_index:end_index], 2)

        prefix = text_to_strip[0:start_index]
        postfix = text_to_strip[end_index : len(text_to_strip)]
        text_to_strip = prefix + str(binary_converted) + postfix

    return text_to_strip


# endregion


# region Main
def main():
    args = ContextArguments()

    # Create the temp string writer to pass to the preprocessor since we still want to modify
    # the contents for project-specific conditions
    with StringIO() as file_string_writer:
        with redirect_stdout(file_string_writer):
            # Parse the target file:
            processor = ContextPreprocessor(args)

        # Check if empty
        string_writer_position = file_string_writer.tell()
        if string_writer_position == 0:
            return

        with open(args.output_path, "w", encoding="utf-8", newline="\n") as f:
            # Do we need to sanitize this further?
            if (
                not args.strip_attributes
                and not args.strip_at_address
                and not args.convert_binary_literals
            ):
                f.write(file_string_writer.getvalue())
                return

            # Sanitize line-by line for easier parsing
            file_string_writer.seek(0)
            while True:
                line_to_write = file_string_writer.readline()
                if not line_to_write:
                    break

                if args.strip_attributes:
                    line_to_write = strip_attributes(line_to_write)

                if args.strip_at_address:
                    line_to_write = strip_at_address(line_to_write)

                if args.convert_binary_literals:
                    line_to_write = strip_binary_literals(line_to_write)

                f.writelines(line_to_write)

    def sanitize_path(path: str) -> str:
        return path.replace("\\", "/").replace(" ", "\\ ")

    san_root_dir = sanitize_path(root_dir)
    if not san_root_dir.endswith("/"):
        san_root_dir = san_root_dir + "/"

    def make_relative(path: str) -> str:
        path = sanitize_path(path)
        return path.replace(san_root_dir, "")

    if args.deps_path:
        with open(os.path.join(root_dir, args.deps_path), "w", encoding="utf-8") as f:
            f.write(make_relative(args.output_path) + ":")
            for dep in processor.include_deps:
                f.write(f" \\\n\t{make_relative(dep)}")


# endregion

if __name__ == "__main__":
    main()
