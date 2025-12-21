# -------------------------------------------------------------------------------
# Permute ProDG compiler flags to figure out the correct ones for loops
# -------------------------------------------------------------------------------
import sys
import os
import itertools
import subprocess
from elftools.elf.elffile import ELFFile

cflags_base = [
    "-O2",
    "-G0",
    "-I" "src/Speed/Indep/Libs/Support/stlgc",
    "-I" "src/Speed/GameCube/Libs/stl/STLport-4.5/stlport",
    "-I" "src/Speed/GameCube/bWare/GameCube/bWare/GameCube/SN/include",
    "-I" "include",
    "-I" "src/Speed/GameCube/bWare/GameCube/dolphinsdk/include",
    "-I" "./",
    "-I" "src",
    "-DEA_PLATFORM_GAMECUBE",
    "-DGEKKO",
    "-D_USE_MATH_DEFINES",
    # "-mps-nodf",
    # "-ffast-math",
    # "-fno-strength-reduce",
    # "-fforce-addr",
]

cflags_possible = [
    "-mfast-cast",
    "-mpowerpc-gpopt",
    "-fforce-addr",
    "-fident",
    "-fleading-underscore",
    "-finstrument-functions",
    "-fdump-unnumbered",
    "-fprefix-function-name",
    "-fstrict-aliasing",
    "-fargument-noalias-global",
    "-fargument-noalias",
    "-fargument-alias",
    "-fstack-check",
    "-fpack-struct",
    "-foptimize-register-move",
    "-fregmove",
    "-fgnu-linker",
    "-fverbose-asm",
    "-fdata-sections",
    "-ffunction-sections",
    "-finhibit-size-directive",
    "-fcommon",
    "-fbranch-probabilities",
    "-ftest-coverage",
    "-fprofile-arcs",
    "-fasynchronous-exceptions",
    "-fsjlj-exceptions",
    "-fnew-exceptions",
    "-fexceptions",
    "-fpic",
    "-fbranch-count-reg",
    "-fdefault-single-float",
    "-fsched-spec-load-dangerous",
    "-fsched-spec-load",
    "-fsched-spec",
    "-fsched-interblock",
    "-fschedule-insns2",
    "-fschedule-insns",
    "-fpretend-float",
    "-frerun-loop-opt",
    "-frerun-cse-after-loop",
    "-fgcse",
    "-freg-struct-return",
    "-fpcc-struct-return",
    "-fcaller-saves",
    "-fshared-data",
    "-fsyntax-only",
    "-fkeep-static-consts",
    "-finline",
    "-fkeep-inline-functions",
    "-finline-functions",
    "-ffunction-cse",
    "-fforce-mem",
    "-fpeephole",
    "-fwritable-strings",
    "-freduce-all-givs",
    "-fmove-all-movables",
    "-funroll-all-loops",
    "-funroll-loops",
    "-fstrength-reduce",
    "-fcompiler-id",
    "-fthread-jumps",
    "-fexpensive-optimizations",
    "-fcse-skip-blocks",
    "-fcse-follow-jumps",
    "-fomit-frame-pointer",
    "-fdefer-pop",
    "-fvolatile-static",
    "-fvolatile-global",
    "-fvolatile",
    "-ffloat-store",
    "-p",
    "-fallow-single-precision",
    "-funsigned-bitfields",
    "-fsigned-char",
    "-funsigned-char",
    "-traditional",
    "-fhosted",
    "-ffreestanding",
    "-fcond-mismatch",
    "-fshort-double",
    "-fshort-enums",
    "-fshort-wchar",
    "-fopt-dwarf",
    "-fstatic-dtor",
    "-fforce-link-once",
    "-fbyte-bool",
    "-fno-access-control",
    "-falt-external-templates",
    "-fcheck-new",
    "-fconserve-space",
    "-fno-conserve-space",
    "-fno-const-strings",
    "-fno-default-inline",
    "-fno-rtti",
    "-fno-for-scope",
    "-fguiding-decls",
    "-fno-guiding-decls",
    "-fno-gnu-keywords",
    "-fhandle-signatures",
    "-fhuge-objects",
    "-fno-implement-inlines",
    "-fno-implicit-templates",
    "-fno-implicit-inline-templates",
    "-finit-priority",
    "-flabels-ok",
    "-fnew-abi",
    "-foperator-names",
    "-fno-optional-diags",
    "-frepo",
    "-fsquangle",
    "-fstats",
    "-fno-strict-prototype",
    "-fvtable-gc",
]

cflags_without_no = [
    "-fallow-single-precision",
    "-fno-access-control",
    "-fno-conserve-space",
    "-fno-const-strings",
    "-fno-default-inline",
    "-fno-rtti",
    "-fno-for-scope",
    "-fno-guiding-decls",
    "-fno-gnu-keywords",
    "-fno-implement-inlines",
    "-fno-implicit-templates",
    "-fno-implicit-inline-templates",
    "-fno-optional-diags",
    "-fno-strict-prototype",
]

cflags_no = [
    "-fno-" + flag.partition("-f")[2]
    for flag in cflags_possible
    if flag.startswith("-f") and flag not in cflags_without_no
]

cflags_possible.extend(cflags_no)


def main(fn_name, input_cpp, output_o, expected_o):
    with open(expected_o, "rb") as expected_f:
        expected_elf = ELFFile(expected_f)
        expected_text = expected_elf.get_section_by_name(".text")
        expected_symtab = expected_elf.get_section_by_name(".symtab")

        expected_func = next(
            (sym for sym in expected_symtab.iter_symbols() if sym.name == fn_name),
            None,
        )

        if not expected_func:
            print(f"Function {fn_name} not found in target")
            return

        expected_bytes = expected_text.data()[
            expected_func["st_value"] : expected_func["st_value"]
            + expected_func["st_size"]
        ]

        # TODO change
        for n in range(3, 4):
            for combo in itertools.combinations(cflags_possible, n):
                combo = list(combo)
                command = [".\\build\\compilers\\ProDG\\3.9.3\\ngccc.exe"]
                command.extend(cflags_base)
                command.extend(combo)
                command.extend(["-o", output_o, input_cpp])
                subprocess.run(command)
                try:
                    with open(output_o, "rb") as src_f:
                        src_elf = ELFFile(src_f)
                        src_text = src_elf.get_section_by_name(".text")
                        src_symtab = src_elf.get_section_by_name(".symtab")

                        src_func = next(
                            (
                                sym
                                for sym in src_symtab.iter_symbols()
                                if sym.name == fn_name
                            ),
                            None,
                        )

                        if not src_func or not src_text:
                            print(f"Function {fn_name} not found in source")
                            continue

                        src_bytes = src_text.data()[
                            src_func["st_value"] : src_func["st_value"]
                            + src_func["st_size"]
                        ]

                        if src_bytes == expected_bytes:
                            print(f"Solution found: {combo}")
                            return
                except FileNotFoundError:
                    pass

        print("No solution found")


if __name__ == "__main__":
    if len(sys.argv) < 5:
        print(
            "Expected usage: {0} <fn_name> <cpp input file> <generated .o file> <expected .s file>".format(
                sys.argv[0]
            )
        )
        sys.exit(1)
    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
