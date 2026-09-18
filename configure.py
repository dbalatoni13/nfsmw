#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    Platform,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GOWE69",  # 0
    "EUROPEGERMILESTONE",  # 1
    "SLES-53558-A124",  # 2
    "SLUS-21351",  # 3
    "SPEED_EXE_1_3",  # 4
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--delink",
    metavar="BINARY | DIR",
    type=Path,
    help="path to delink binary (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.delink_path = args.delink
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.compilers_tag = "20260903"

if version_num in [0]:
    config.platform = Platform.GC_WII
    config.dtk_tag = "v1.8.32"
    config.binutils_tag = "2.42-1"
elif version_num in [1]:
    config.platform = Platform.X360
    config.dtk_tag = "v0.1.2"
    config.binutils_tag = "2.42-1"
elif version_num in [2, 3]:
    config.platform = Platform.PS2
    config.binutils_tag = "2.45"
elif version_num in [4]:
    config.platform = Platform.WIN32
    config.delink_tag = "v0.16.1"

config.objdiff_tag = "v3.8.0"  # r79: el nuestro es mas nuevo que el de upstream
config.sjiswrap_tag = "v1.2.2"

# sjiswrap segfault
if config.platform == Platform.GC_WII:
    config.wibo_tag = "1.1.0"
else:
    config.wibo_tag = "1.2.0"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"

compilers_path = (
    Path(config.compilers_path) if config.compilers_path else Path("build/compilers")
)

if config.platform == Platform.GC_WII:
    config.asflags = [
        "-mgekko",
        "--strip-local-absolute",
        "-I include",
        f"-I build/{config.version}/include",
        f"--defsym BUILD_VERSION={version_num}",
    ]

    ldscript_path = Path("config") / config.version / "ldscript.ld"
    keep_list_path = Path("config") / config.version / "keep.lst"
    config.ldflags = [
        "-strip-unused-data",
        # "-report-unused",
        "-keep",
        str(keep_list_path),
        "-T",
        str(ldscript_path),
    ]

    # Optional numeric ID for decomp.me preset
    # Can be overridden in libraries or objects
    config.scratch_preset_id = 176
elif config.platform == Platform.X360:
    config.ldflags = [
        "/NODEFAULTLIB",
        "/MACHINE:PPCBE",
        "/XEX:NO",
        f"/PDB:./build/{config.version}/{config.version}.pdb",
        f"/DEBUG",
        f"/LTCG",
    ]
elif config.platform == Platform.PS2:
    config.asflags = [
        "-no-pad-sections",
        "-EL",
        "-march=5900",
        "-mabi=eabi",
        "-I include",
    ]
    ldscript_path = Path("build") / config.version / "ldscript.ld"
    config.ldflags = [
        "-EL",
        "-T",
        str(ldscript_path),
    ]  # TODO what about undefined_syms_auto.txt?
elif config.platform == Platform.WIN32:
    config.ldflags = [
        "/NODEFAULTLIB",
        f"/PDB:./build/{config.version}/{config.version}.pdb",
        f"/DEBUG",
    ]

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
if config.platform == Platform.GC_WII:
    config.linker_version = "ProDG/3.9.3"

    dolphinsdk_root = "src/Speed/GameCube/bWare/GameCube/dolphinsdk"

    cflags_base_mwcc = [
        "-nodefaults",
        "-proc gekko",
        "-align powerpc",
        "-enum int",
        "-fp hardware",
        "-Cpp_exceptions off",
        # "-W all",
        "-O4,p",
        "-inline auto",
        '-pragma "cats off"',
        '-pragma "warn_notinlined off"',
        "-maxerrors 1",
        "-nosyspath",
        "-RTTI off",
        "-fp_contract off",
        "-str reuse",
        # "-i include",
        # f"-i build/{config.version}/include",
        "-multibyte",
        "-D__GEKKO__",
        f"-DVERSION={version_num}",
    ]

    # TODO move some of these to the game flags
    cflags_base_prodg = [
        "-O1",
        "-gdwarf+",
        # "-Wa,-L",
        # "-Wall",
        # "-Wreturn-type", # enable at some point
        "-Wno-ctor-dtor-privacy",  # because of AttribSys for example
        "-Woverloaded-virtual",
        "-Wno-multichar",
        "-I src/Speed/Indep/Libs/Support/stlgc",
        "-I src/Speed/GameCube/Libs/stl/STLport-4.5/stlport",
        "-I src/Speed/GameCube/bWare/GameCube/SN/include",
        "-I src/Packages/eathread/1.1.0/include",
        f"-I {dolphinsdk_root}/include",
        "-I src/Packages",
        "-I src/Speed/GameCube/bWare/GameCube/SN",
        # "-I ./",
        "-I src",
        "-DEA_PLATFORM_GAMECUBE",
        "-DEA_REGION_AMERICA",
        "-DGEKKO",
        "-D_USE_MATH_DEFINES",
        f"-I build/{config.version}/include",
        f"-DBUILD_VERSION={version_num}",
        f"-DVERSION_{config.version}",
    ]

    # PS2 path/spch: mismos includes que GC (rutas independientes de plataforma)
    cflags_path = [
        *cflags_base_prodg,
        "-I src/Packages/eathread/1.1.0/include",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/path/5.01.04/include",
    ]

    # PS2 spch: los .c se compilan como C++ (mismo caso que snd en GC)
    cflags_spch = [
        *cflags_base_prodg,
        "-G0",
        "-x c++",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/spch/dev/include",
    ]


    config.context_defines = [
        "EA_PLATFORM_GAMECUBE",
        "EA_REGION_AMERICA",
        "GEKKO",
        "_USE_MATH_DEFINES",
        "__SN__",
        "SN_TARGET_NGC",
    ]

    if config.non_matching:
        cflags_base_prodg.append("-DNON_MATCHING")

    # Debug flags
    if args.debug:
        cflags_base_prodg.append("-DDEBUG=1")
    else:
        cflags_base_prodg.append("-DNDEBUG=1")

    cflags_game = [
        *cflags_base_prodg,
        "-mps-nodf",
        # "-mfast-cast",
        "-G0",
        # "-mstrict-align",
        # "-mno-bit-align",
        "-fno-static-dtors",
        "-ffast-math",
        # "-fno-strength-reduce",
        "-fforce-addr",
        "-fcse-follow-jumps",
        "-fcse-skip-blocks",
        "-fforce-mem",
        "-fgcse",
        "-frerun-cse-after-loop",
        "-fschedule-insns",
        "-fschedule-insns2",
        "-fexpensive-optimizations",
        "-frerun-loop-opt",
        "-fmove-all-movables",
        # "-fno-defer-pop",
        # "-fno-function-cse",
        # "-fpeephole",
        # "-fregmove",
        # "-fno-thread-jumps",
        # "-freduce-all-givs",
        # # "-fcaller-saves",
        # # "-ffloat-store",
        # # "-funroll-all-loops",
        # "-fno-sched-spec",
        # "-fno-keep-static-consts",
        # "-fno-common",
        # "-fno-argument-alias",
        # "-fno-ident",
        "-DLUA_NUMBER=float",
        "-DDISABLE_RAIN",
        "-DDEFAULT_ALLOCATOR=0",
        # path.h: PATH_vectortoreal6 y PATH_setallocator solo existen con USEREALCORE.
        "-DUSEREALCORE",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Packages/eathread/1.1.0/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/spch/dev/include",
        "-I src/Speed/Indep/Libs/path/5.01.04/include",
        "-I src/Speed/Indep/Libs/realcore/6.24.00/include/common",
        "-I src/Speed/Indep/Libs/endian/0.5.2/include",
    ]

    cflags_snd = [
        *cflags_base_prodg,
        "-G0",
        "-O2",
        "-fno-strength-reduce",
        "-fno-strict-aliasing",
        "-ffast-math",
        "-mps-float",
        "-x c++",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/snd/9/include",
    ]

    # EA audio middleware: path (music pathing) and spch (speech) libraries.
    # Identified via DWARF1 CU map from NFSMWRELEASE.ELF (C:/packages/path,
    # D:/perforce/audio/libs/SPCH); built with the SN toolchain like snd.
    cflags_path = [
        *cflags_base_prodg,
        "-G8",
        "-O2",
        "-fno-strict-aliasing",
        "-ffast-math",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/path/5.01.04/include",
    ]

    # NOTE: despite the .c extension, these files were compiled as C++ by the
    # original build (all symbols in the base objects use GCC2 C++ mangling,
    # e.g. SPCH_GetExtVecs__Fv), so force -x c++ like the snd lib.
    cflags_spch = [
        *cflags_base_prodg,
        "-G0",
        "-O2",
        "-fno-strength-reduce",
        "-fno-strict-aliasing",
        "-ffast-math",
        "-x c++",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/spch/dev/include",
    ]

    config.extra_clang_flags = [
        "-std=gnu++11",
        "-DSN_TARGET_NGC",
        "-D__SN__",
        "-D_STLP_VENDOR_EXCEPT_STD=std",
        "-DCLANGD_DAMNIT",  # used in cases where intellisense breaks
        "-D__HONOR_STD",
        "-Wno-writable-strings",
    ]

    cflags_dolphin = [
        *cflags_base_mwcc,
        f"-i {dolphinsdk_root}/include",
        f"-i {dolphinsdk_root}/include/libc",
        "-i include",
        f"-i build/{config.version}/include",
        "-char unsigned",
        "-warn pragmas",
        "-requireprotos",
        "-DSDK_REVISION=2",
        f"-ir {dolphinsdk_root}/src",
    ]

    # Tres unidades de LibSN NO son SN ProDG: son Metrowerks CodeWarrior.
    # Prueba: no llevan el simbolo `gcc2_compiled.` (presente en 100 de 105
    # unidades GCC del juego y en 0 de 95 Metrowerks), vm.s llama a __div2i y
    # _savegpr_23/24 (runtime MW; GCC emitiria __divsi3), y sus estaticos de
    # funcion van manglados `nombre$NNN` en vez de `nombre.NNN`.
    # El prologo (`stwu` antes del `mflr`) las situa en la familia 1.3-2.7.
    cflags_libsn_mw = [
        *[f for f in cflags_dolphin if f != "-inline auto"],
        "-inline off",
        "-lang=c",
    ]

    # steering.c (driver Logitech) NO comparte optimizacion con vm/vmbase:
    # medido, con -O4,s y -fp_contract on da 3.232 B y con -O4,p 1.264 B; al reves,
    # aplicar -O4,s a vm lo baja de 1.164 a 304 B y a vmbase de 1.028 a 408 B.
    # Se lee del asm sin compilar: con ,p MWCC desenrolla x8 los bucles de relleno
    # y convierte /360 en un mulhw de numero magico; con ,s emite bucle contado y
    # divw real. Y los fmadds del objetivo son -fp_contract on.
    cflags_libsn_mw_steering = [
        ("-O4,s" if f == "-O4,p" else f)
        for f in cflags_libsn_mw
    ]
    cflags_libsn_mw_steering = [
        ("-fp_contract on" if f == "-fp_contract off" else f)
        for f in cflags_libsn_mw_steering
    ] + ["-use_lmw_stmw on"]
    # Metrowerks library flags
    cflags_runtime = [*cflags_base_mwcc]

    cflags_odemuexi = [*cflags_base_mwcc]

    cflags_amcstub = [*cflags_base_mwcc]

    # Helper function for Dolphin libraries
    def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
        return {
            "lib": lib_name,
            "toolchain_version": "GC/1.2.5n",
            "cflags": cflags_dolphin,
            "progress_category": "sdk",
            "objects": objects,
        }

elif config.platform == Platform.X360:
    config.linker_version = "X360/14.00.2110"

    cflags_base_prodg = [
        "/nologo",
        "/c",  # compile without linking
        "/wd4996",  # get rid of string deprecation warnings for now
        "/wd4355",  # gets rid of the warning 'this' used in base member initializer
        "/wd4716",
        # "/GL",  # enable LTCG
        # "/GR",  # RTTI
        "/Og",
        # "/Os", # no
        "/Ob2",
        "/Oi",
        # "/Oy",  # maybe
        # "/Ox",  # maybe
        # "/Ou",  # enable prescheduling
        "/Oz",  # enable inline asm scheduling
        "/GF",  # Eliminate Duplicate Strings
        # "/Gy",  # maybe?
        # "/Z7",  # /Zi enables debug info (pdb), /Zd for line numbers only (pdb), /Z7 generates debug info per obj file
        "/EHsc",  # enable exception handling (and extern C notthrow?)
        "/I src/Packages/xenonsdk/2.0.2135.2/installed/include/xbox",
        "/I src/Packages",
        "/I src",
        "/DEA_PLATFORM_XENON",
        "/D_USE_MATH_DEFINES",
        f"/I build/{config.version}/include",
        f"/DBUILD_VERSION={version_num}",
        f"/DVERSION_{config.version}",
    ]

    config.context_defines = [
        "EA_PLATFORM_XENON",
        "EA_REGION_GERMANY",
        "_USE_MATH_DEFINES",
        "_WIN32",
    ]

    cflags_game = [
        *cflags_base_prodg,
        "/DLUA_NUMBER=float",
        "/DMILESTONE_BUILD",
        "/DDEFAULT_ALLOCATOR=0",
        # path.h: PATH_vectortoreal6 y PATH_setallocator solo existen con USEREALCORE.
        "/DUSEREALCORE",
        "/I src/Speed/Indep/Libs/allocator/1.5.0",
        "/I src/Speed/Indep/Libs/csis/dev/include",
        "/I src/Packages/eathread/1.1.0/include",
        "/I src/Speed/Indep/Libs/snd/9/include",
        "/I src/Speed/Indep/Libs/spch/dev/include",
        "/I src/Speed/Indep/Libs/path/5.01.04/include",
        "/I src/Speed/Indep/Libs/realcore/6.24.00/include/common",
        "/I src/Speed/Indep/Libs/endian/0.5.2/include",
    ]

    cflags_snd = [
        *cflags_game,
        "/I src/Speed/Indep/Libs/snd/9/include",
    ]

    # path y spch son bibliotecas nuestras (upstream no las lista): sin estos
    # cflags, `configure.py -v EUROPEGERMILESTONE` moria con NameError, igual
    # que le pasaba al bloque WIN32.
    cflags_path = [
        *cflags_game,
        "/I src/Speed/Indep/Libs/allocator/1.5.0",
        "/I src/Speed/Indep/Libs/csis/dev/include",
        "/I src/Speed/Indep/Libs/snd/9/include",
        "/I src/Speed/Indep/Libs/path/5.01.04/include",
    ]

    cflags_spch = [
        *cflags_game,
        "/I src/Speed/Indep/Libs/allocator/1.5.0",
        "/I src/Speed/Indep/Libs/csis/dev/include",
        "/I src/Speed/Indep/Libs/snd/9/include",
        "/I src/Speed/Indep/Libs/spch/dev/include",
    ]

    config.extra_clang_flags = [
        "-std=c++03",
        "-D_WIN32",
        "-D_WCHAR_T_DEFINED",
        "-fms-extensions",
    ]
elif config.platform == Platform.PS2:
    config.linker_version = "PS2/ee-gcc2.9-991111"

    cflags_base_prodg = [
        "-O2",
        "-g2",
        "-Wa,-L",  # Keep compiler-generated $LC* local object symbols
        # "-Wall",
        "-Wno-ctor-dtor-privacy",  # because of AttribSys for example
        "-I src/Speed/Indep/Libs/Support/stlps2",
        "-I src/Speed/PSX2/Libs/stl/gpp",
        "-I src/Speed/PSX2/bWare/src/ee/include",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/ee/include",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111/include",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/ee",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111",
        "-I src/Packages",
        "-I src",
        "-DEA_PLATFORM_PLAYSTATION2",  # TODO rename to PS2
        # EA_BUILD_A124 y MILESTONE_BUILD son del PROTOTIPO alpha 124 (SLES-53558-A124,
        # version_num 2). SLUS-21351 es la version DE VENTA y no es ninguna de las dos:
        # su ELF no tiene `bMalloc__FiPCcii` -- el asignador de cuatro argumentos, con
        # fichero y linea, que solo existe en builds milestone -- y el de SLES si.
        *(["-DEA_BUILD_A124"] if version_num == 2 else []),
        "-D_NOTHREADS",  # TODO is this necessary?
        # r77 (PS2): `_STL` es el espacio de nombres propio con el que esta
        # configurado el STLport de GameCube, y el codigo compartido escribe
        # `_STL::sort(...)` en 49 sitios. En PS2 la STL es la de SGI y va en el
        # espacio global: COMPROBADO contra el binario, el ELF de PS2 no tiene ni
        # una ocurrencia de `_STL`. Definiendolo vacio, `_STL::sort` queda en
        # `::sort`, que es lo que la STL de PS2 ofrece. Cero cambios en fuente y
        # cero efecto en GameCube, que no pasa por este bloque.
        "-D_STL=",
        f"-I build/{config.version}/include",
        f"-DBUILD_VERSION={version_num}",
        # f"-DVERSION_{config.version}", # TODO it's broken because of the dash?
    ]

    # PS2 path/spch: mismos includes que GC (rutas independientes de plataforma)
    # path (SN 2.95.3-136) lleva -G0, -ffast-math y -fno-exceptions/-fno-rtti como
    # el resto del middleware. Medido una a una: -G0 +10.460 B, -ffast-math +788 B
    # y sin excepciones +612 B en SLES (+8.860, +788 y +612 en SLUS).
    cflags_path = [
        *cflags_base_prodg,
        "-G0",
        "-ffast-math",
        "-fno-exceptions",
        "-fno-rtti",
        "-I src/Packages/eathread/1.1.0/include",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/path/5.01.04/include",
    ]

    # PS2 spch: los .c se compilan como C++ (mismo caso que snd en GC)
    cflags_spch = [
        *cflags_base_prodg,
        "-G0",
        "-x c++",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/spch/dev/include",
    ]

    config.context_defines = [
        "EA_PLATFORM_PLAYSTATION2",
        "EA_REGION_AMERICA",
        *(["EA_BUILD_A124"] if version_num == 2 else []),
        "_NOTHREADS",
    ]

    # Debug flags
    # TODO
    # if args.debug:
    # cflags_base.append("-DDEBUG=1")
    # else:
    #     cflags_base.append("-DNDEBUG=1")

    cflags_game = [
        *cflags_base_prodg,
        "-G0",
        "-ffast-math",
        "-fno-exceptions",
        "-fno-rtti",
        # "-funaligned-pointers",
        # "-funaligned-struct-hack",
        # "-fsched-interblock",
        # "-fsched-spec",
        # "-fsched-spec-load-dangerous",
        # "-fedge-sm",
        # "-fedge-lm",
        # "-fedge-lcm",
        # "-fforce-addr",
        # "-fcse-follow-jumps",
        # "-fcse-skip-blocks",
        # "-fforce-mem",
        # "-fgcse",
        # "-fstrength-reduce",
        # "-frerun-cse-after-loop",
        # "-fschedule-insns",
        # "-fschedule-insns2",
        # "-fexpensive-optimizations",
        # "-frerun-loop-opt",
        # "-fmove-all-movables",
        # "-fregmove",
        # "-fcaller-saves",
        "-DLUA_NUMBER=float",
        # Solo el prototipo A124: ver la nota de EA_BUILD_A124 mas arriba.
        *(["-DMILESTONE_BUILD"] if version_num == 2 else []),
        "-DDEFAULT_ALLOCATOR=0",
        # path.h: PATH_vectortoreal6 y PATH_setallocator solo existen con USEREALCORE.
        "-DUSEREALCORE",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Packages/eathread/1.1.0/include",
        "-I src/Speed/Indep/Libs/snd/9/include",
        "-I src/Speed/Indep/Libs/spch/dev/include",
        "-I src/Speed/Indep/Libs/path/5.01.04/include",
        "-I src/Speed/Indep/Libs/realcore/6.24.00/include/common",
        "-I src/Speed/Indep/Libs/endian/0.5.2/include",
    ]

    cflags_snd = [
        *cflags_game,
        "-x c++",
        "-I src/Speed/Indep/Libs/csis/dev/include",
        "-I src/Speed/Indep/Libs/allocator/1.5.0",
        "-I src/Speed/Indep/Libs/snd/9/include",
    ]

    config.extra_clang_flags = [
        "-std=gnu++98",
        "-DCLANGD_DAMNIT",
        "-D__HONOR_STD",
        "-D__STL_MEMBER_TEMPLATE_KEYWORD",
        "-U_MIPS_SIM",
        "-U __mips",
        "-D__mips=3",
        "-D__mips_eabi",
        "-DR5900",
        "-D_R5900",
        "-D__mips_single_float",
        "-D__builtin_next_arg(x)=((void *)0)",
        "-D__builtin_args_info(x)=1",
        "-msoft-float",
    ]
elif config.platform == Platform.WIN32:
    config.linker_version = "Win32/7.1"

    cflags_base_prodg = [
        "/nologo",
        "/c",  # compile without linking
        "/wd4996",  # get rid of string deprecation warnings for now
        "/wd4355",  # gets rid of the warning 'this' used in base member initializer
        "/wd4716",  # functions not decompiled yet have empty bodies, as on Xbox 360
        # "/Og",
        # "/Os",
        # "/Ob2",
        # "/Oi",
        # "/Oy",  # maybe
        "/Ox",
        # "/Ou",  # enable prescheduling
        # "/Oz",  # enable inline asm scheduling
        # "/GF",  # Eliminate Duplicate Strings
        # "/Gy",  # maybe?
        "/Z7",  # /Zi enables debug info (pdb), /Zd for line numbers only (pdb), /Z7 generates debug info per obj file
        "/EHsc",  # enable exception handling (and extern C notthrow?)
        f"/I {compilers_path / config.linker_version / 'Include'}",
        "/I src/Packages",
        "/I src",
        "/DEA_PLATFORM_WIN32",
        "/D_USE_MATH_DEFINES",
        f"/I build/{config.version}/include",
        f"/DBUILD_VERSION={version_num}",
        f"/DVERSION_{config.version}",
    ]

    config.context_defines = [
        "EA_PLATFORM_WIN32",
        "EA_REGION_AMERICA",
        "_USE_MATH_DEFINES",
        "_WIN32",
    ]

    cflags_game = [
        *cflags_base_prodg,
        "/DLUA_NUMBER=float",
        "/DDEFAULT_ALLOCATOR=0",
        # path.h: PATH_vectortoreal6 y PATH_setallocator solo existen con USEREALCORE.
        "/DUSEREALCORE",
        "/I src/Speed/Indep/Libs/allocator/1.5.0",
        "/I src/Speed/Indep/Libs/csis/dev/include",
        "/I src/Packages/eathread/1.1.0/include",
        "/I src/Speed/Indep/Libs/snd/9/include",
        "/I src/Speed/Indep/Libs/spch/dev/include",
        "/I src/Speed/Indep/Libs/path/5.01.04/include",
        "/I src/Speed/Indep/Libs/realcore/6.24.00/include/common",
        "/I src/Speed/Indep/Libs/endian/0.5.2/include",
    ]

    cflags_snd = [
        *cflags_game,
        "/I src/Speed/Indep/Libs/snd/9/include",
    ]

    # r79: las bibliotecas path y spch son nuestras; upstream no las tiene en su
    # lista, asi que su bloque WIN32 no definia estos cflags y
    # `configure.py -v SPEED_EXE_1_3` moria con NameError.
    cflags_path = [
        *cflags_game,
        "/I src/Speed/Indep/Libs/allocator/1.5.0",
        "/I src/Speed/Indep/Libs/csis/dev/include",
        "/I src/Speed/Indep/Libs/snd/9/include",
        "/I src/Speed/Indep/Libs/path/5.01.04/include",
    ]

    cflags_spch = [
        *cflags_game,
        "/I src/Speed/Indep/Libs/allocator/1.5.0",
        "/I src/Speed/Indep/Libs/csis/dev/include",
        "/I src/Speed/Indep/Libs/snd/9/include",
        "/I src/Speed/Indep/Libs/spch/dev/include",
    ]

    config.extra_clang_flags = [
        "-std=c++98",
        "-D_WIN32",
        "-D_WCHAR_T_DEFINED",
        "-fms-extensions",
    ]

cflags_libc = [*cflags_base_prodg, "-O2"]


def solo_gc(*flags):
    """Opciones de ProDG que solo existen en GameCube (EE-GCC y MSVC las rechazan)."""
    return list(flags) if config.platform == Platform.GC_WII else []


def solo_ps2(*flags):
    """Opciones que solo se pasan en PS2."""
    return list(flags) if config.platform == Platform.PS2 else []


# PS2: el middleware NO se compilo con el ee-gcc del juego. El .mdebug del NFS.ELF
# de SLES-53558-A124 guarda el directorio de compilacion de cada fichero: el juego
# sale de mw\Speed (2.9-ee-991111b, el de config.linker_version) y las bibliotecas
# de EA de sus paquetes PlayStation2SN (anotados abajo). Firma: guardan s0-s7/fp
# con `sq` y ra con `sd` (1.863 de 1.864; el otro es decodexac, ensamblador a
# mano). De los cinco ee-gcc de SN que tenemos solo el 2.95.3 SN BUILD 1.36 guarda
# asi: 2.95.2-273a/274 y 2.95.3-107/114 guardan tambien ra con `sq`. Medido con
# los flags de antes en las 189 unidades de biblioteca de SLES que compilan con
# todos: 991111 19.676 B, 273a/274/107/114 24.984 B y 136 68.700 B (en las 119 de
# SLUS: 41.512, 44.808 y 75.364 B). Con el 136 y los flags de abajo, SLES pasa de
# 23,31 % a 24,72 % y SLUS de 25,53 % a 26,71 %, sin tocar una SourceList.
# OJO: en los 2.95.3 de SN `long long` es de 128 bits (16 B) y `long`, de 64. El
# original usa `long` para 64 bits (FILESYS_readlarge__FiUlPvUliT2): donde nuestra
# fuente pone `long long`, el compilador aborta con "unsupported wide integer
# operation" salvo con -frelax-128 (opcion de SN, abajo por fichero), y lo que
# devuelve o guarda esos 64 bits no puede casar hasta que la fuente use `long`.
PS2_TOOLCHAIN = {
    "rcmp": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 2.8.1-8
    "eathread": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 2.8.1-8 (aun sin unidades en PS2)
    "realcore": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 2.8.1-9
    "spch": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 2.8.1-9 (con spchcsis.cpp)
    "vp6": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 2.8.1-9 (PS2 es 1.0.3: sin unidades)
    "snd": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 3.0.2-1
    "realmemcard": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 3.0.2-1 (PS2 es 3.04.00: sin unidades)
    "path": "PS2/ee-gcc2.95.3-136",  # PlayStation2SN 3.0.2-4
}


def toolchain_de(lib):
    """toolchain_version de una biblioteca: en PS2, el ee-gcc de SN de su paquete."""
    if config.platform == Platform.PS2:
        return PS2_TOOLCHAIN.get(lib, config.linker_version)
    return config.linker_version


# vp6: sin -fmove-all-movables ni -fforce-addr, y con -mps-float en vez de
# -mps-nodf (ver la nota del bloque de la biblioteca).
cflags_vp6 = [
    f
    for f in cflags_game
    if f not in ("-G0", "-fmove-all-movables", "-fforce-addr", "-mps-nodf")
] + ["-O2", *solo_gc("-mps-float")]
# eathread va a -O2 como el resto de bibliotecas de EA: el static-init de
# eathread_thread (32 AtomicInt vacios) sale como li/mtctr/bdnz solo a -O2.
cflags_eathread = [*[f for f in cflags_game if f != "-O1"], "-O2"]

Matching = True  # Object matches and should be linked
NonMatching = False  # Object does not match and should not be linked
Equivalent = (
    config.non_matching
)  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


def RenameSectionsFor(versions: tuple[str], renames: tuple[tuple[str]]):
    return renames if config.version in versions else ()


# snd enlaza en dos bloques ordenados cada uno por ruta: el grande (sexithndl.c ..
# supxapf.cpp) y otro de siete unidades, de sgparse.cpp a SNDI_findprime.c, que es
# otro archivo de biblioteca con sus propias opciones. SNDI_root1x.c solo sale con
# las locales del DWARF (p1..p7) sin -ffast-math, que asocia las multiplicaciones;
# las otras seis dan el mismo objeto con y sin la opcion (medido).
cflags_snd_nofastmath = [f for f in cflags_snd if f != "-ffast-math"]

if config.platform != Platform.PS2:
    config.warn_missing_config = True

config.warn_missing_source = False
config.libs = [
    {
        "lib": "Game",
        "toolchain_version": config.linker_version,
        "cflags": cflags_game,
        "host": False,
        "progress_category": "game",  # str | List[str]
        "objects": [
            Object(NonMatching, "Speed/Indep/SourceLists/zAI.cpp"),
            # r76b: CWorldAnimEntity::Init 97,98 % (912/912 B) sin la barrera.
            Object(NonMatching, "Speed/Indep/SourceLists/zAnim.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zAttribSys.cpp"),
            # r76b: bList::Sort 94,54 % (172/172 B). 13 formas medidas; el .lreg da
            # los numeros de allocno_compare y ninguna forma de fuente los cambia.
            Object(NonMatching, "Speed/Indep/SourceLists/zBWare.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zCamera.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zComms.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zDebug.cpp"),
            # r76b: Geometry::SphereVsBox 98,49 % (1896/1896 B).
            Object(NonMatching, "Speed/Indep/SourceLists/zDynamics.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEagl4Anim.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEAXSound.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEAXSound2.cpp"),
            Object(
                NonMatching,
                "Speed/Indep/SourceLists/zEcstasy.cpp",
                # El original usa ps_mul/ps_madds0/psq_l/psq_st explicitos en
                # epCalculateLocalDirectionalPOS16 y en media eDataRender::Render:
                # sin -mps-float el mode(PS) es ilegal y esas ~8 kB no se pueden
                # escribir. Medido: no mueve ni un byte de lo que ya casa.
                cflags=cflags_game,
                extra_cflags=solo_gc("-mps-float"),
            ),
            Object(NonMatching, "Speed/Indep/SourceLists/zFe.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zFe2.cpp"),
            # r76b: ProcessPadsForPackage 99,91 % (3080/3080 B) y Update 98,25 %
            # (764/776). La barrera escondia una SENTENCIA que faltaba: con
            # `Pressed = Held = Released = 0;` detras de Mask/JoyMask, como dice el
            # mapa de lineas, se pasa de 33 filas a 14 sin un solo asm.
            Object(NonMatching, "Speed/Indep/SourceLists/zFEng.cpp"),
            Object(
                NonMatching,
                "Speed/Indep/SourceLists/zFoundation.cpp",
                # Unity TU original con -fforce-addr; los VU0_* requieren -mps-float
                # (paired singles via inline asm) y -fvtable-thunks=1.
                cflags=cflags_game,
                extra_cflags=solo_gc("-mps-float", "-fvtable-thunks=1"),
            ),
            Object(NonMatching, "Speed/Indep/SourceLists/zGameModes.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zGameplay.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zLua.cpp"),
            # r76b: ConvertWheelRotation 98,84 % (172/172) y PollDevice 99,93 %
            # (1348/1348). Todas las secciones ALLOC y la tabla de simbolos siguen
            # identicas -- solo cambian 10 instrucciones, todas de numero de
            # registro -- pero matched_code es todo-o-nada. DOS de los tres andamios
            # sostenian un 100 % FALSO: metian notas de linea que el original no
            # tiene (cruce .line contra debug_lines.txt).
            Object(NonMatching, "Speed/Indep/SourceLists/zMain.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMisc.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMiscSmall.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMission.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zPhysics.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zPhysicsBehaviors.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zPlatform.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zRender.cpp"),
            # r76b: QuickGame::CreateCars 99,75 % (1912/1908). Lo que falta es UNA
            # instruccion `mr r3,r11`, ranura de sched1 en expand_call.
            Object(NonMatching, "Speed/Indep/SourceLists/zSim.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zSpeech.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zTrack.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zWorld.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zWorld2.cpp"),
            # Overlay units: the original build linked these two objects
            # through *zOnline.o(.over .rodata) / *zFEOverlay.o(.over .rodata),
            # so their code section is .over and not .text (see
            # config/GOWE69/ldscript.ld and the .over ranges in splits.txt).
            Object(
                NonMatching,
                "Speed/Indep/SourceLists/zOnline.cpp",
                section_rename=".text=.over",
            ),
            # r76b: CustomizeParts::Setup 99,84 % (1708/1708) y
            # CustomizeMain::NotificationMessage 96,61 % (808/808). El pin de
            # vinyl_group_number era HONESTO (la local existe y vive en r21), pero
            # no compila fuera de GameCube. Y el 100 % de NotificationMessage era
            # falso en mapa de lineas: 78 notas contra las 72 del original, y las
            # seis de mas justo donde estaban los andamios; sin ellos, 74.
            Object(
                NonMatching,
                "Speed/Indep/SourceLists/zFeOverlay.cpp",
                section_rename=".text=.over",
                # r48: emitiamos 96 simbolos de mas (7.184 B) que el original no
                # tiene, y el 90 % no los referencia nadie. La bandera deja de
                # emitir las instanciaciones IMPLICITAS de plantilla, que es
                # exactamente lo que son. Medido: .over 148.408 -> 142.528, cero
                # funciones distintas y cero simbolos perdidos.
                extra_cflags=["-fno-implicit-templates"],
            ),
        ],
    },
    {
        "lib": "rcmp",
        "toolchain_version": toolchain_de("rcmp"),
        "cflags": [
            # Medido: esta biblioteca NO va con los flags del juego. -O2 (el bucle
            # de NEW_tBigYUVSwizzlerTexture solo sale con mtctr/bdnz a -O2),
            # -mps-float en vez de -mps-nodf (con nodf los guardados FPR salen
            # psq_st en vez de stfd), -fno-force-addr (idctcompute pasa de 63% a
            # 100%: el original rematerializa lis/addi en cada llamada en vez de
            # izar la direccion a un salvado) y SIN -fno-static-dtors (sin eso no
            # se emiten los _GLOBAL_.D. ni la rama destructora).
            # Y SIN -G0: rcmp_vp6_codec, maddec y maddeca direccionan sus
            # globales pequenos por @sda21 en el original (30+ accesos), el
            # delator de la seccion 1 del playbook. Medido: quitarlo vale
            # +188 B y una funcion (el ctor de VP6_CODEC_INTERNAL) y no
            # regresa ninguna de las 13 unidades de la biblioteca.
            # Y SIN -fmove-all-movables, igual que vp6: iza fuera del bucle el
            # `li 0` condicional de AV_PLAYER::GetRCMPChunk a un salvado, que el
            # original deja dentro. Medido: +540 B y una funcion, cero regresion
            # en las 13 unidades y los dos objetos Matching siguen byte a byte.
            *[f for f in cflags_game
              if f not in ("-O1", "-mps-nodf", "-fforce-addr", "-fno-static-dtors", "-G0",
                           "-fmove-all-movables")],
            "-O2",
            *solo_gc("-mps-float"),
            "-fno-force-addr",
            "-fno-implement-inlines",
            # PS2 (SN 2.95.3-136) SI lleva -G0: el original no direcciona nada por
            # $gp. Medido en SLES: +156 B (audioplayer); en SLUS, +156 B.
            *solo_ps2("-G0"),
        ],
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmpbase.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmp2real.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmp_mad_codec.cpp"),
            Object(
                Matching,
                "egami/rcmp/dev/source/decoder/cmn/rcmp_mad_codec_chunk_types.cpp",
            ),
            Object(
                Matching,
                "egami/rcmp/dev/source/decoder/cmn/rcmp_vp6_codec_chunk_types.cpp",
            ),
            Object(Matching, "egami/rcmp/dev/source/decoder/cmn/maddec.cpp"),
            Object(Matching, "egami/rcmp/dev/source/decoder/cmn/maddeca.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/madidct.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmp_vp6_codec.cpp"),
            # r76: barrera fuera. GetFirstFrame 98,78 % (656/656 B).
            # av/ sin -ffast-math: el original no reasocia las multiplicaciones por
            # constante (AV_PLAYER::IsTimeForDecode multiplica por 0.001f y luego por
            # el frame rate; con -ffast-math GCC lo reordena). decoder/ si lo lleva:
            # MAD_CODEC_INTERNAL::DecodeChunk baja sin el.
            # PS2: -frelax-128 porque AV_MS_TIMER calcula en `long long` (128 bits en
            # el ee-gcc de SN; ver PS2_TOOLCHAIN). Sin ella no compila; con ella,
            # 56 -> 716 B y 2 -> 10 funciones.
            Object(
                NonMatching,
                "egami/rcmp/dev/source/av/cmn/avplayer.cpp",
                extra_cflags=["-fno-fast-math", *solo_ps2("-frelax-128")],
            ),
            Object(Matching, "egami/rcmp/dev/source/av/cmn/avsubtitle.cpp", extra_cflags=["-fno-fast-math"]),
            Object(NonMatching, "egami/rcmp/dev/source/av/cmn/audioplayer.cpp", extra_cflags=["-fno-fast-math"]),
            Object(NonMatching, "egami/rcmp/dev/source/vd/gc/bigyuvswizzler.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/vd/gc/bigswizzler.cpp"),
        ],
    },
    {
        "lib": "vp6",
        "toolchain_version": toolchain_de("vp6"),
        # vp6 no se compilo con -fmove-all-movables ni con -fforce-addr: quitarlos
        # lleva vputil.c y pb_globals.c a mas match y no regresa ninguna unidad.
        # Y va con -mps-float, no con -mps-nodf, igual que rcmp: con nodf los
        # guardados FPR salen psq_st en vez de stfd y el marco encoge 0x10.
        # Ninguna de las 25 unidades regresa; postproc pasa de 62,91% a 85,31%
        # (gaussian 112 B y PlaneAddNoise_C 620 B, imposibles con nodf).
        # criticalpath usa psq_l/psq_st con registros de cuantificacion, que el
        # propio compilador rechaza "without -mps-float enabled".
        "cflags": cflags_vp6,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(NonMatching, "Packages/vp6/1.0.6/source/decode/cmn/allocator.cpp"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/duck_mem.cpp"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/boolhuff.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/clamp.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/TokenEntropy.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/gc/DSystemDependant.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/gc/doptsystemdependant.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/gc/uoptsystemdependant.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/DeInterlace.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/decodemv.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/recon.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/pb_globals.c", extra_cflags=["-fno-common"]),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/reconstruct.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/loopfilter.c", extra_cflags=["-fno-common"]),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/vputil.c", extra_cflags=["-fno-common"]),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/postproc.c", extra_cflags=["-fno-common"]),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/scale.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/FrameIni.c"),
            Object(NonMatching, "Packages/vp6/1.0.6/source/decode/cmn/vfwpbdll_if.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/DFrameR.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/Huffman.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/borders.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/decodemode.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/decodembs.c"),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/deblock.c", extra_cflags=["-fno-common"]),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/dering.c", extra_cflags=["-fno-common"]),
            Object(Matching, "Packages/vp6/1.0.6/source/decode/cmn/idctpart.c"),
            Object(
                Matching,
                "Packages/vp6/1.0.6/source/decode/cmn/quantize.c",
                cflags=[*cflags_vp6, "-fno-common"],
            ),
            Object(NonMatching, "Packages/vp6/1.0.6/source/decode/cmn/simpledeblocker.c"),
            # r76: despineada en la r75 (9 pines). VP6_DecodeBlock y
            # VP6_PredictFilteredBlock ya no reproducen el objeto (163 B de diferencia
            # en .text, mismo tamano); el enlace usa el extraido.
            Object(NonMatching, "Packages/vp6/1.0.6/source/decode/gc/criticalpath.c"),
        ],
    },
    {
        "lib": "snd",
        "toolchain_version": toolchain_de("snd"),
        "cflags": cflags_snd,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/saems.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/saemsamb.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/saemsmbf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/saemsmbm.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/saemstimupdt.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/salloc.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sattrdef.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbadd.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sballoc.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbasync.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbasyncm.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbhdrcpy.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbhdrsze.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbplay.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbremove.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sbvalid.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/scheckpo.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sclnt100.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sctrldry.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sdata.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sfxlevel.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/slowpass.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/smemcpy.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/smemdis.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/smemlmt.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/smemlu.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/smemman.c"),
            # NOTE: upstream lo marca Matching, pero compila a 0x25c frente a los
            # 0x210 del original ("kinda match") y desplaza todo el .text
            # posterior, rompiendo el sha1 del DOL. NonMatching hasta que case.
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sndfxbus.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spatkey.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spitch.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sgetpvol.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/gc/sscalcfx.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/gc/ssdfx.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/gc/sdspmix.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/seffect.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/sevent.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfxrevc.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sgetdata.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/sinitdts.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixer.c"),
            # r76: despineada. SNDDRV_audiocallback 99,56 % (400/400 B): el DWARF no
            # tiene `mixSource` y r4 es el parametro `context`. Tampoco tiene bloques
            # lexicos, asi que se fueron tambien las llaves que daban ambito al pin.
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/gc/snddrv.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spktplay.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spktctoh.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/splysdef.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spoutlat.c"),
            # r76: barrera fuera. iSNDrandom 98,33 % (260/264 B).
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/srandom.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sreallocbuf.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/srender.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sresopat.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sserver.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssine.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sst.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sst3dpos.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstautov.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstcrtap.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstfxlev.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstgetrp.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstgetpv.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssthighp.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssthold.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstlowp.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstop.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstovrhd.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstpmult.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstqmem.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstqreqi.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstrmdry.cpp"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstrstat.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstsetgl.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sststat.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssttmul.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstvol.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssys.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssysinit.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssysserv.c"),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/cmn/ssysveccsismutex.cpp",
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/stagpat.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/stimemul.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/stimerem.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/stpparse.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/stretch.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/svecreal.cpp"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/svol.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sx87d16.c"),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/coda.cpp",
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/s3dlow.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/gc/saramman.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/satospkr.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sclcptch.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sctlfilt.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sdownmix.cpp"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfamplf.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfbpffir8.c"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfecho.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfft24.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfhpffir8.c"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfilter.c"),
            # r76: los DOS asm fuera. calcFIRCoeffs 89,25 % (940/936 B). Hay formas
            # sin asm mejores ya medidas (95-97 %) pero exigen subir la escritura
            # `halfLen = 4`, y eso puede cambiar que coeficiente se resta: pendiente
            # de comprobar la equivalencia antes de adoptarlas.
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/mix/sfir.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfir8.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sflpf.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sflpffir8.c"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfmixer.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfreson.c"),
            # r76: despineada. SFILTER_splitter 99,53 % (256/256 B): el DWARF no tiene
            # `copySource` ni `copySize`; faltan las 3 cargas de argumento, que rs6000
            # emite r3->r4->r5 y el objetivo deja r3 para el final.
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/mix/sfsplit.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfsrc.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sgettag.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/mix/shipass.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sinit16.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sinitut.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sinitxa.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/slib.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/slinklst.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/slinkmix.c"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/smemhigh.cpp"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixc.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixfram.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixhip.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixlowp.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixptch.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixtmul.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_cos.c"),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_mult16.c",
                cflags=cflags_snd_nofastmath,
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_root1x.c",
                cflags=cflags_snd_nofastmath,
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_sin.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sover.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spantoaz.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/spat2hdr.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/srrange.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/sstopall.c"),
            Object(NonMatching, "Speed/Indep/Libs/snd/9/source/library/cmn/ssysreal.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/suplf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supmutf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supmutlf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supmutpf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/suppf.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supxaf.cpp"),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supxalf.cpp"
            ),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/mix/supxapf.cpp"
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf.cpp",
                cflags=cflags_snd_nofastmath,
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mtdecf.cpp",
                cflags=cflags_snd_nofastmath,
            ),
            Object(
                Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sexithndl.c"
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/sfrsf.c"),
            Object(
                Matching,
                "Speed/Indep/Libs/snd/9/source/library/cmn/sgparse.cpp",
                cflags=cflags_snd_nofastmath,
            ),
            Object(
                MatchingFor("GOWE69"),
                "Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_findprime.c",
                cflags=cflags_snd_nofastmath,
            ),
            Object(
                MatchingFor("GOWE69"),
                "Speed/Indep/Libs/snd/9/source/library/cmn/sbpatinf.c",
            ),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/scrsfl.c", cflags=cflags_snd_nofastmath),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixfilt.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/mix/smixvec.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sformat.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/soutputmap.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/sprofvoc.c"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/cmn/author.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mpeghufftables.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mpegl3base.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/setmemcpy.cpp"),
            # coda music codec extras (identified from DWARF1 CU map):
            # PPCMtdec (0x80311BE4) and lbmpeg.cpp _GLOBAL_ ctor (0x80366298)
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mtdecppc.cpp"),
            Object(Matching, "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/lbmpeg.cpp"),
        ],
    },
    {
        # EA path music library (C:/packages/path/5.01.04) - .text 0x80374530..0x8037D268
        "lib": "path",
        "toolchain_version": toolchain_de("path"),
        # -fno-implement-inlines: barrido unidad a unidad (r30 §2.1);
        # condicion de la promocion de pathreal (532 -> 496 = objetivo).
        "cflags": [*cflags_path, "-fno-implement-inlines"],
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(
                Matching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathcontrol.cpp"
            ),
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathevent.cpp"
            ),
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathinit.cpp"
            ),
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal.cpp"),
            # r79: sin el primer de "%s  %s" (literal muerto de origen desconocido).
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal6.cpp"),
            Object(Matching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathserv.cpp"),
            # r76: barrera fuera. CreateStreamTrack 99,85 % (404/404 B).
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp"),
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp"
            ),
            # r79: sin el primer de "%s  %s" (literal muerto de origen desconocido).
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathvol.cpp"),
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathaction.cpp"
            ),
            Object(NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp"),
            Object(
                Matching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathdebug.cpp"
            ),
            # r76: despineada en la r75 (8 pines). PATHI_calcwaitbeat ya no reproduce
            # el objeto (77 B de diferencia en .text, mismo tamano).
            Object(
                NonMatching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp"
            ),
            Object(Matching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathrand.cpp"),
        ],
    },
    {
        # EA speech library (D:/perforce/audio/libs/SPCH) - .text 0x8037D26C..0x80381654
        "lib": "spch",
        "toolchain_version": toolchain_de("spch"),
        "cflags": cflags_spch,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank.c"),
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchdata.c"),
            Object(NonMatching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchevnt.c"),
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchinit.c"),
            Object(NonMatching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c"),  # r76: despineada (iSPCH_ChooseSamples 412->408 B)
            # r76: barrera fuera. iSPCH_EACrandom 98,33 % (260/264 B).
            Object(NonMatching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchrand.c"),
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchrslv.c"),
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchrule.c"),
            # r76: las dos barreras fuera. iSPCH_GetSampleSizeData 87,65 % (136/136 B).
            Object(NonMatching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c"),
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/sptick.c"),
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/sputil.c"),
            Object(
                Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchcsis.cpp"
            ),
        ],
    },
    {
        "lib": "realcore",
        "toolchain_version": toolchain_de("realcore"),
        # NOTE: sin -fno-static-dtors: las TU realcore con objetos globales
        # polimorficos (filesys, dvd_device, hd_device) SI emiten _GLOBAL_.D.
        # PS2 (SN 2.95.3-136) va con -G0 y no -G8: el original no direcciona nada
        # por $gp. Medido: +900 B en SLES y +180 B en SLUS.
        "cflags": [f for f in cflags_game if f not in ("-G0", "-fforce-addr", "-fno-static-dtors")]
        + [
            "-G0" if config.platform == Platform.PS2 else "-G8",
            "-fstrength-reduce",
            "-fno-implement-inlines",
        ],
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/device.cpp",
            ),
            # PS2: -frelax-128 porque FILEOPERATION guarda sus 64 bits en `long long`
            # (128 bits en el ee-gcc de SN; el original usa `long`, ver
            # PS2_TOOLCHAIN). Sin ella no compila; con ella, 184 -> 2.448 B y 14 -> 21
            # funciones en SLES. Seis accesores de `value` (Complete, SizeOperation::
            # Exec) dejan de casar: con el 991111 casaban porque alli mide 8 B.
            Object(
                NonMatching,  # r76: despineada (AddToQueue 252->248 B)
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp",
                extra_cflags=solo_ps2("-frelax-128"),
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesysopts.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/syncfile.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/hlafile.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/hlsfile.cpp",
            ),
            # r76: degradada para que su objeto EXTRAIDO exporte `gCurRead`, que el
            # snddrv extraido referencia 10 veces. En el original ese dato NO era
            # static (un static de otra unidad no se puede referenciar desde C); en
            # symbols.txt pasa a scope:global. Nuestro fuente se queda como estaba:
            # quitarle el `static` sube la alineacion de SECCION de .bss de 8 a 32 y
            # corre 16 B el .bss del DOL (medido con shift.py).
            Object(
                NonMatching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device.cpp",
            ),
            # r79: sin el relleno de .bss con __attribute__((section)).
            Object(
                NonMatching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/gc/hd_device.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/event.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/effect.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/itimer.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/memvectors.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/eventqueue.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/effectimp.cpp",
            ),
            Object(NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interfaceimp.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_device.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_effect.cpp",
            ),
            Object(NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_interface.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_pad.cpp",
            ),
            Object(
                NonMatching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/bigfile.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys_c.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys_cc.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/std/cmn/locale.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/file/gc/device.cpp",
            ),
            Object(NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/systemvars.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/timer.cpp",
            ),
            Object(NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/systask.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/system/gc/threads.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/system/gc/signals.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/debug/cmn/printstr.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/debug/cmn/abortmsg.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/std/cmn/memclear.cpp",
            ),
            Object(
                NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/std/cmn/exit.cpp"
            ),
            Object(NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/system/gc/timerthread.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/std/gc/memfill.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/std/gc/memcopy.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/mutex.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/exit.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/system/gc/initvblt.cpp",
            ),
            Object(NonMatching, "Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/debug/cmn/printvstr.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/debug/cmn/debugger.cpp",
            ),
            Object(
                Matching,
                "Speed/Indep/Libs/realcore/6.24.00/source/debug/gc/printdrv.cpp",
            ),
        ],
    },
    {
        "lib": "realmemcard",
        "toolchain_version": toolchain_de("realmemcard"),
        "cflags": [
            # Esta biblioteca NO lleva -G0 (sus estaticos viven en .sdata y se
            # leen con @sda21) ni -fforce-addr (accede a los globales sin
            # materializar la direccion en un registro salvado).
            # Y TAMPOCO -fno-static-dtors: sin el, el static-init de gc_interface
            # emite la rama destructora y pasa de 84,29% a 86,67% CON LA COLA BYTE
            # A BYTE. Medido en las 15 unidades de la biblioteca: cero regresiones.
            *[f for f in cflags_game
              if f not in ("-G0", "-fforce-addr", "-fno-static-dtors")],
            # Y se compilo con wchar_t de 16 bits y sin signo: sus simbolos
            # manglan `Uw` (unsigned wchar_t), no `w`.
            "-fshort-wchar",
            # Lleva reduccion de fuerza: sus bucles sobre arrays avanzan
            # punteros en vez de indexar con lwzx/stwx.
            "-fstrength-reduce",
            "-I src/Packages/realmemcard/3.04.01-layer2/include/common",
            "-fno-implement-inlines",
        ],
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_interface.cpp",
            ),
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_interface_impl.cpp",
            ),
            Object(NonMatching, "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_utilities.cpp",
            ),
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/interfaceimp.cpp",
            ),
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_blockcalculator.cpp",
            ),
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_memcard_taskmanager.cpp",
            ),
            Object(NonMatching, "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_memvectors.cpp",
            ),
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_taskmanager.cpp",
            ),
            # r76: despineada y sin un solo asm. OpenFile 99,48 % (692/692 B) y
            # ReadFile 99,09 % (464/468 B). El parametro ya se llamaba `nBytesRead` en
            # la cabecera y en el DWARF: aqui se habia renombrado y copiado con pin.
            Object(
                NonMatching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp",
            ),
            Object(NonMatching, "Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp",
            ),
            Object(NonMatching, "Packages/realmemcard/3.04.01-layer2/source/lib/gc/public.cpp",
            ),
            Object(
                Matching,
                "Packages/realmemcard/3.04.01-layer2/source/lib/gc/trctasks.cpp",
            ),
            Object(NonMatching, "Packages/realmemcard/3.04.01-layer2/source/lib/gc/tasks.cpp",
            ),
            Object(Matching, "Packages/realmemcard/3.04.01-layer2/source/lib/cmn/locale.cpp"),
            Object(Matching, "Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_memcard_interface_impl.cpp"),
        ],
    },
]

if config.platform == Platform.GC_WII:
    config.libs.extend(
        [
            {
                "lib": "libsn",
                "toolchain_version": config.linker_version,
                # SN library code is gcc2-compiled (SN ProDG toolchain),
                # NOT Metrowerks: ngccc rejects the mwcc-style flags.
                # MEDIDO: comparte casi todo con cflags_libc pero SI lleva -G0.
                # sndvd emite lis @ha/@l donde sin -G0 sacamos @sda21: vale
                # +348 B y 3 funciones. Tiene que ser una lista PROPIA: meter
                # -G0 en cflags_libc cuesta -13.032 B y 33 funciones en libc
                # (sf_atan -528, sf_fmod -456, s_floor -348, fopen -324...).
                "cflags": [*cflags_libc, "-G0"],
                "host": False,
                "progress_category": "libs",  # str | List[str]
                "objects": [
                    Object(MatchingFor("GOWE69"), "LibSN/crt0.s"),
                    Object(NonMatching, "LibSN/cvtll.c"),
                    Object(NonMatching, "LibSN/debug.c"),
                    Object(NonMatching, "LibSN/dummy.c"),
                    Object(NonMatching, "LibSN/fileserver.c"),
                    Object(NonMatching, "LibSN/FSasync.c"),
                    Object(NonMatching, "LibSN/inituser.c"),
                    Object(NonMatching, "LibSN/ppcdown.c"),
                    Object(NonMatching, "LibSN/prof.c"),
                    Object(NonMatching, "LibSN/proview.c"),
                    # DSIHandler 97,07 % (620/628 B). Asm original que se queda: el
                    # vector DSIExcHandler y los mtspr del DABR (SPR de supervisor).
                    Object(NonMatching, "LibSN/sndvd.c"),
                    Object(NonMatching, "LibSN/tealeaf.c"),
                    Object(NonMatching, "LibSN/tors.c"),
                    # SN SDK blocks identified from symbols (no DWARF CUs):
                    # MetroTRK proper 0x8030EA7C-0x80311BE4 (licensed SN code),
                    # Logitech steering driver 0x8031F5D8-0x80321810,
                    # SN virtual memory 0x8039B258-0x8039BD38 + VMBASE 0x803A3398-0x803A4004
                    # MetroTRK nunca fue C: va como .s, igual que pro2D4.s.
                    Object(Matching, "LibSN/metrotrk.c", source="LibSN/metrotrk.s"),
                    Object(Matching, "LibSN/syscalls.c"),
                ],
            },
            {
                "lib": "libsn_mw",
                "toolchain_version": "GC/2.7",
                "cflags": cflags_libsn_mw,
                "host": False,
                "progress_category": "libs",
                "objects": [
                    Object(Matching, "LibSN/vm.c"),
                    Object(Matching, "LibSN/VMPageReplacement.c"),
                    Object(Matching, "LibSN/VMMapping.c"),
                    # __VMBASESetupExceptionHandlers: 99,16 % en C (mwcc lleva el
                    # temporal de la instruccion por r0); el 100 % era asm.
                    Object(NonMatching, "LibSN/vmbase.c"),
                ],
            },
            {
                "lib": "libsn_mw_steering",
                "toolchain_version": "GC/2.7",
                "cflags": cflags_libsn_mw_steering,
                "host": False,
                "progress_category": "libs",
                "objects": [
                    Object(NonMatching, "LibSN/steering.c"),
                ],
            },
            {
                # RealShape y RealFont (librealshapez.a / librealfontz.a). MEDIDO: con
                # los cflags de `misc` estas siete unidades dan 83,2%; con estos tres
                # cambios dan 4.644/4.644 B y 34 de 34 funciones, 100% las siete.
                #   -G0 SOBRA: MemObject::sAllocator y RealFontOld::gFontDriver se leen
                #     con @sda21(r0) en UNA instruccion; con -G0 salen lis+lwz @l.
                #     Cuesta fontcreate ENTERA (136 B) y 284 B de base.
                #   -fforce-addr SOBRA: el objetivo emite `lbzx r3,r9,r3` (base primero)
                #     y con el flag sale `lbzx r3,r3,r9`. Cuesta GetDepth y Shape::Create.
                #   -fstrength-reduce FALTA: findkern16 avanza dos punteros (addi 4)
                #     donde sin el sintetizamos slwi+lhzx. Cuesta 64 B.
                # NO se puede arreglar `misc` en bloque: de sus 272 objetos solo 8 tienen
                # fuente, y csis.cpp CAE de 81,58% a 48,71% con estos flags (-1.884 B).
                # Los hermanos sin fuente todavia (shptype, swizzlesize, fontdraw,
                # fontdrawf, fontnull) van aqui cuando se escriban.
                "lib": "realgraph",
                "toolchain_version": config.linker_version,
                "cflags": [
                    *[f for f in cflags_game if f not in ("-G0", "-fforce-addr")],
                    "-fstrength-reduce",
                ],
                "host": False,
                "progress_category": "libs",
                "objects": [
                    Object(Matching, "allocator.cpp"),
                    Object(Matching, "shpdestroy.cpp"),
                    Object(NonMatching, "model.cpp"),
                    Object(Matching, "fontchar.cpp"),
                    Object(Matching, "fontcreate.cpp"),
                    Object(Matching, "fontkern.cpp"),
                    Object(NonMatching, "creates.cpp"),
                    Object(NonMatching, "cluttype.cpp"),
                ],
            },
            {
                "lib": "misc",
                "toolchain_version": config.linker_version,
                "cflags": cflags_game,
                "host": False,
                "progress_category": "libs",  # str | List[str]
                "objects": [
                    Object(Matching, "crt2D1.tmp", source="LibSN/crt2D1.c", cflags=[*cflags_libc, "-G0"]),
                    # r76: barrera fuera. __cvt_fp2unsigned 94,44 % (68/72 B).
                    Object(NonMatching, "ppc2D2.tmp", source="LibSN/ppc2D2.c", cflags=[*cflags_libc, "-G0"]),
                    # r39: __va_arg is MW, unlike the preceding GCC bridges.
                    # Separate objects reproduce the original DOL; see r39-runtime.md.
                    Object(
                        Matching,
                        "Runtime.PPCEABI.H/__va_arg.c",
                        toolchain_version="GC/1.2.5n",
                        cflags=[*cflags_runtime, "-char unsigned", "-lang=c"],
                    ),
                    Object(NonMatching, "fil2D3.tmp"),
                    Object(Matching, "pro2D4.tmp", source="LibSN/pro2D4.s"),
                    Object(NonMatching, "inituser.c"),
                    Object(NonMatching, "pro2D9.tmp"),
                    Object(NonMatching, "tea2Da.tmp"),
                    # 2104/2104 B en C, sin asm. Sigue NonMatching: sus datos viven en
                    # los comodines auto_06_804394D0_data y auto_07_804B4DE8_bss.
                    Object(NonMatching, "FSasync.c", source="LibSN/FSasync.c", cflags=[*cflags_libc, "-G0"]),
                    Object(NonMatching, "sndvd.c"),
                    # This newlib unit uses the libc flags, not the generic
                    # game fallback (-G0/-fforce-addr changes its SDA loads).
                    Object(Matching, "atexit.c", cflags=cflags_libc),
                    Object(Matching, "qsort.c"),
                    Object(Matching, "sn_malloc.c"),
                    Object(NonMatching, "allsrc.c"),
                    Object(NonMatching, "audioplayer.cpp"),
                    Object(NonMatching, "device_cmn.cpp"),
                    Object(NonMatching, "rendercontext.cpp"),
                    Object(NonMatching, "rendercontext_cmn.cpp"),
                    Object(NonMatching, "state.cpp"),
                    Object(NonMatching, "tar.cpp"),
                    Object(NonMatching, "tevstage.cpp"),
                    Object(NonMatching, "texturerc_cmn.cpp"),
                    Object(NonMatching, "viewport.cpp"),
                    Object(NonMatching, "viewport_cmn.cpp"),
                    Object(NonMatching, "profiler_cmn.cpp"),
                    Object(NonMatching, "singledraw.cpp"),
                    Object(NonMatching, "rendermethod.cpp"),
                    Object(NonMatching, "texturerc.cpp"),
                    Object(NonMatching, "pcode.cpp"),
                    Object(NonMatching, "locatbig.cpp"),
                    Object(NonMatching, "filedev.cpp"),
                    Object(NonMatching, "fixdmult.cpp"),
                    Object(NonMatching, "gc_driver.cpp"),
                    Object(NonMatching, "memvectors.cpp"),
                    Object(NonMatching, "interfaceimp.cpp"),
                    Object(NonMatching, "lib/cmn/locale.cpp"),
                    Object(NonMatching, "inittmr.cpp"),
                    Object(NonMatching, "initvblt.cpp"),
                    Object(NonMatching, "mutex2.cpp"),
                    Object(NonMatching, "printn3.cpp"),
                    Object(NonMatching, "memcopy.cpp"),
                    Object(NonMatching, "fontdraw.cpp"),
                    Object(NonMatching, "fontdrawf.cpp"),
                    Object(NonMatching, "fontnull.cpp"),
                    Object(NonMatching, "loccore.cpp"),
                    Object(NonMatching, "ustrcpy.cpp"),
                    Object(NonMatching, "shptype.cpp"),
                    Object(NonMatching, "swizzlesize.cpp"),
                    # r76: CERO asm en el fichero. Subscribe 98,96 % (392/392 B) y
                    # Unsubscribe 99,02 % (232/232 B) con la inline real
                    # gSystems.Push/Remove, que ademas retira dos escrituras por
                    # desplazamiento; Init 99,91 % (264/264 B) al quitar el asm que
                    # emitia un `stw`. Alloc y AllocFast siguen al 100 %.
                    Object(
                        NonMatching,
                        "Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp",
                        cflags=[*cflags_game, "-fno-implement-inlines"],
                    ),
                    Object(NonMatching, "dummy.c"),
                    Object(NonMatching, "prodg_fixes.cpp"),
                    Object(NonMatching, "codec/cmn/refsize.cpp"),
                ],
            },
            DolphinLib(
                "base",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/base/PPCArch.c",
                    ),
                ],
            ),
            DolphinLib(
                "ar",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ar/ar.c",
                    ),
                ],
            ),
            DolphinLib(
                "arq",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ar/arq.c",
                    ),
                ],
            ),
            DolphinLib(
                "ax",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXAlloc.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXAux.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXCL.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AX.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXOut.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXSPB.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXVPB.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXComp.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/DSPCode.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXProf.c",
                    ),
                ],
            ),
            DolphinLib(
                "card",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDBios.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDBlock.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDDir.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDCheck.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDOpen.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDMount.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDNet.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDFormat.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDCreate.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDRead.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDWrite.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDDelete.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDStat.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDStatEx.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDUnlock.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDRdwr.c",
                    ),
                ],
            ),
            DolphinLib(
                "dsp",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dsp/dsp.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dsp/dsp_debug.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dsp/dsp_task.c",
                    ),
                ],
            ),
            DolphinLib(
                "os",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OS.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSAlarm.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSAlloc.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSArena.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSAudioSystem.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSCache.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSContext.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSError.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSExec.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSFont.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSInterrupt.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSLink.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSMemory.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSMessage.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSMutex.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSReset.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSResetSW.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSRtc.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSSemaphore.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSSync.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSThread.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSTime.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSUtf.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSReboot.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/__ppc_eabi_init.c",
                    ),
                ],
            ),
            DolphinLib(
                "db",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/db/db.c",
                    ),
                ],
            ),
            DolphinLib(
                "mtx",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/mtx.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/mtxvec.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/mtx44.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/vec.c",
                    ),
                ],
            ),
            DolphinLib(
                "dvd",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdfs.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvd.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdqueue.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvderror.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdidutils.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdFatal.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/fstload.c",
                        extra_cflags=["-char signed"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdlow.c",
                        extra_cflags=["-char signed"],
                    ),
                ],
            ),
            DolphinLib(
                "vi",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/vi/vi.c",
                    ),
                ],
            ),
            DolphinLib(
                "demo",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/demo/DEMOPad.c",
                    ),
                ],
            ),
            DolphinLib(
                "pad",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/pad/Padclamp.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/pad/Pad.c",
                    ),
                ],
            ),
            DolphinLib(
                "ai",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ai/ai.c",
                    ),
                ],
            ),
            DolphinLib(
                "gx",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXInit.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXFrameBuf.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXAttr.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXFifo.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXMisc.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXGeometry.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXDisplayList.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXLight.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXTexture.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXBump.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXTev.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXPixel.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXTransform.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXPerf.c",
                    ),
                ],
            ),
            DolphinLib(
                "exi",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/exi/EXIBios.c",
                        extra_cflags=["-O4,p", "-schedule off"],
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/exi/EXIUart.c",
                    ),
                ],
            ),
            DolphinLib(
                "si",
                [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SIBios.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISamplingRate.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISteering.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISteeringXfer.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISteeringAuto.c",
                    ),
                ],
            ),
            {
                "lib": "OdemuExi2",
                # MEDIDO: es Metrowerks de la familia GC/1.0-1.2.5n, no ProDG.
                # El discriminador: MWCC 1.0-1.2.5n emite `addi rD,rS,0` como
                # copia de registro y 1.3-2.7 emiten `mr`; DebuggerDriver.s
                # tiene 34 `addi rX,rY,0x0` y EXIBios.s (SDK 1.2.5n) 94.
                # Con config.linker_version (ProDG) el ngccc rechaza los flags
                # de MWCC con "unknown command line option '-nodefaults'".
                "toolchain_version": "GC/1.2.5n",
                "cflags": cflags_dolphin,
                "progress_category": "sdk",  # str | List[str]
                "objects": [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c",
                    ),
                ],
            },
            {
                # The original ELF identifies two more TUs, built with 1.2.5.
                "lib": "OdemuExi2Amc",
                "toolchain_version": "GC/1.2.5",
                "cflags": cflags_dolphin,
                "progress_category": "sdk",
                "objects": [
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/OdemuExi2/src/AmcExi2Comm.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/OdemuExi2/src/AmcExi.c",
                    ),
                ],
            },
            {
                "lib": "amcstubs",
                "mw_version": config.linker_version,
                "cflags": cflags_amcstub,
                "host": False,
                "progress_category": "sdk",  # str | List[str]
                "objects": [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/amcstubs/src/amcExi2/AmcExi.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/amcExi2/AmcExi2Comm.c",
                    ),
                ],
            },
            {
                "lib": "libc",
                "toolchain_version": config.linker_version,
                "cflags": cflags_libc,
                "progress_category": "libc",  # str | List[str]
                "objects": [
                    Object(Matching, "libc/vprintf.c"),
                    Object(Matching, "libc/vsprintf.c"),
                    Object(Matching, "libc/memcpy.c"),
                    Object(Matching, "libc/memmove.c"),
                    Object(Matching, "libc/memset.c"),
                    Object(Matching, "libc/strcasecmp.c"),
                    Object(Matching, "libc/strchr.c"),
                    Object(Matching, "libc/strcmp.c"),
                    Object(Matching, "libc/strcpy.c"),
                    Object(Matching, "libc/strlen.c"),
                    Object(Matching, "libc/strncmp.c"),
                    Object(Matching, "libc/strncpy.c"),
                    # r76: las dos compilaciones de vfprintf.c se despinaron en la r75
                    # (18 pines). _vfprintf_r/_vfiprintf_r/add_separators ya no
                    # reproducen el objeto (mismo tamano, distinto horario).
                    Object(NonMatching, "libc/vfprintf.c"),
                    Object(Matching, "libc/ctype_.c"),
                    Object(Matching, "libc/impure.c"),
                    Object(NonMatching, "libc/vfprintf_1.c"),
                    Object(Matching, "libc/isdigit.c"),
                    Object(Matching, "libc/isspace.c"),
                    Object(Matching, "libc/locale.c"),
                    Object(Matching, "libc/_tolower.c"),
                    Object(Matching, "libc/math_support.c"),
                    Object(Matching, "libc/mbtowc_r.c"),
                    Object(Matching, "libc/libgcc2.c"),
                    Object(Matching, "libc/memchr.c"),
                    Object(Matching, "libc/libgcc2_1.c"),
                    Object(Matching, "libc/libgcc2_2.c"),
                    Object(Matching, "libc/libgcc2_3.c"),
                    Object(Matching, "libc/libgcc2_4.c"),
                    Object(Matching, "libc/libgcc2_5.c"),
                    Object(Matching, "libc/libgcc2_6.c"),
                    Object(Matching, "libc/libgcc2_7.c"),
                    Object(Matching, "libc/libgcc2_8.c"),
                    Object(Matching, "libc/libgcc2_9.c"),
                    Object(Matching, "libc/libgcc2_10.c", extra_cflags=["-G0"]),
                    # MEDIDO r19: -msafe-sda manda el POOL DE CONSTANTES del
                    # compilador a .sdata con @sda21 (UNA instruccion), que es
                    # donde lo tiene el original; sin el, el sesgo
                    # 0x4330000080000000 de cada conversion int->double cae en
                    # .rodata y cuesta un `lis @ha` de mas por conversion.
                    # -mstrict-align da BLKmode a los arrays de 8 B y con ello
                    # el orden `lfsx rD, base, indice` del objetivo.
                    # libm es el newlib de SN tal cual (static const, literales):
                    # -msafe-sda pone su pool donde el original, -fno-builtin
                    # deja fabs/fabsf como llamada (newlib se compila asi) y
                    # -G1024 da @sda21 a las tablas de 16..792 B que el original
                    # lee por @sda21 (two_over_pi de ef_rem_pio2 mide 792 B).
                    Object(Matching, "libc/e_pow.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-G1024", "-fno-builtin"]),
                    Object(Matching, "libc/e_sqrt.c"),
                    Object(Matching, "libc/s_fabs.c"),
                    Object(Matching, "libc/ef_pow.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-fno-builtin"]),
                    Object(Matching, "libc/ef_sqrt.c"),
                    # MEDIDO: -fno-expensive-optimizations lleva estas tres de
                    # ~93% a 100% (58 -> 55 instrucciones, 35 -> 32). Va POR
                    # OBJETO a proposito: el mismo flag tira 14 unidades de esta
                    # misma biblioteca de 100% a 0% (e_acos, e_sqrt, s_floor,
                    # math_support, memcmp, libgcc2_3/6/8/9...).
                    Object(
                        Matching,
                        "libc/sf_cos.c",
                        extra_cflags=["-fno-expensive-optimizations", "-msafe-sda"],
                    ),
                    Object(Matching, "libc/sf_fabs.c"),
                    Object(
                        Matching,
                        "libc/sf_sin.c",
                        extra_cflags=["-fno-expensive-optimizations", "-msafe-sda"],
                    ),
                    Object(Matching, "libc/s_scalbn.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/sf_scalbn.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/kf_cos.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/kf_sin.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/ef_rem_pio2.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-G1024", "-fno-builtin"]),
                    Object(Matching, "libc/s_copysign.c"),
                    Object(Matching, "libc/sf_copysign.c"),
                    Object(Matching, "libc/kf_rem_pio2.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-G1024"]),
                    Object(Matching, "libc/sf_floor.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/e_acos.c", extra_cflags=["-msafe-sda"]),
                    Object(NonMatching, "libc/e_atan2.c"),
                    Object(NonMatching, "libc/s_atan.c"),
                    Object(Matching, "libc/ef_atan2.c", extra_cflags=["-msafe-sda", "-fno-builtin"]),
                    Object(NonMatching, "libc/vfscanf.c"),
                    Object(Matching, "libc/strtod2.c"),
                    Object(Matching, "libc/sn_buf.cpp"),
                    Object(Matching, "libc/tolower.c"),
                    Object(NonMatching, "libc/puts.c"),
                    Object(NonMatching, "libc/atoi.c"),
                    Object(Matching, "libc/bsearch.c"),
                    Object(Matching, "libc/memcmp.c"),
                    Object(Matching, "libc/strstr.c"),
                    Object(NonMatching, "libc/strtol.c"),
                    Object(NonMatching, "libc/fvwrite.c"),
                    Object(Matching, "libc/wsetup.c"),
                    Object(Matching, "libc/fflush.c"),
                    Object(Matching, "libc/fopen.c"),
                    Object(Matching, "libc/fwalk.c"),
                    Object(Matching, "libc/makebuf.c"),
                    Object(Matching, "libc/stdio.c"),
                    Object(Matching, "libc/closer.c"),
                    Object(Matching, "libc/fstatr.c"),
                    Object(Matching, "libc/lseekr.c"),
                    Object(Matching, "libc/readr.c"),
                    Object(Matching, "libc/writer.c"),
                    Object(Matching, "libc/s_sin.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/sf_atan.c", extra_cflags=["-msafe-sda", "-G1024", "-fno-builtin"]),
                    Object(
                        Matching,
                        "libc/sf_tan.c",
                        extra_cflags=["-fno-expensive-optimizations", "-msafe-sda"],
                    ),
                    Object(Matching, "libc/k_cos.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/k_sin.c", extra_cflags=["-msafe-sda"]),
                    Object(NonMatching, "libc/k_tan.c"),
                    Object(Matching, "libc/e_rem_pio2.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-G1024", "-fno-builtin"]),
                    # -msafe-sda solo (el pool a .sdata): cierra al 100%
                    Object(Matching, "libc/kf_tan.c", extra_cflags=["-msafe-sda", "-G1024", "-fno-builtin"]),
                    Object(Matching, "libc/k_rem_pio2.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-G1024"]),
                    Object(Matching, "libc/s_floor.c", extra_cflags=["-msafe-sda"]),
                    # SN libc TUs identified from the gcc2/newlib block
                    # 0x80312C10-0x8031F4C0 and 0x8039BD38-0x8039D230
                    Object(Matching, "libc/fclose.c"),
                    Object(Matching, "libc/fprintf.c"),
                    Object(Matching, "libc/fseek.c"),
                    Object(Matching, "libc/printf.c"),
                    Object(Matching, "libc/sprintf.c"),
                    Object(Matching, "libc/strcat.c"),
                    Object(Matching, "libc/strcoll.c"),
                    Object(Matching, "libc/strcspn.c"),
                    Object(Matching, "libc/strncasecmp.c"),
                    Object(Matching, "libc/strncat.c"),
                    # newlib la escribe en ensamblador (libc/machine/powerpc/setjmp.S).
                    Object(Matching, "libc/setjmp.c", source="libc/setjmp.s"),
                    Object(Matching, "libc/itoa.c"),
                    Object(Matching, "libc/rand.c"),
                    Object(Matching, "libc/openr.c"),
                    Object(Matching, "libc/flags.c"),
                    Object(Matching, "libc/sf_asin.c", extra_cflags=["-msafe-sda", "-fno-builtin"]),
                    Object(Matching, "libc/sf_cosh.c", extra_cflags=["-msafe-sda", "-fno-builtin"]),
                    Object(Matching, "libc/sf_exp.c", extra_cflags=["-msafe-sda", "-mstrict-align"]),
                    Object(Matching, "libc/sf_fmod.c"),
                    # -msafe-sda solo (el pool a .sdata): cierra al 100%
                    Object(Matching, "libc/sf_log.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/sf_log10.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/sf_sinh.c", extra_cflags=["-msafe-sda", "-fno-builtin"]),
                    Object(Matching, "libc/sf_ceil.c", extra_cflags=["-msafe-sda"]),
                    Object(Matching, "libc/sf_isnan.c"),
                    Object(Matching, "libc/sf_tanh.c", extra_cflags=["-msafe-sda", "-fno-builtin"]),
                    Object(Matching, "libc/sf_expm1.c", extra_cflags=["-msafe-sda", "-mstrict-align"]),
                    Object(Matching, "libc/strrchr.c"),
                    Object(Matching, "libc/wcscat.c"),
                    Object(Matching, "libc/wcscpy.c"),
                    Object(Matching, "libc/wcslen.c"),
                    Object(Matching, "libc/e_exp.c", extra_cflags=["-msafe-sda", "-mstrict-align", "-G1024"]),
                ],
            },
            {
                "lib": "eathread",
                "toolchain_version": toolchain_de("eathread"),
                "cflags": cflags_eathread,
                "progress_category": "libs",
                "objects": [
                    Object(
                        Matching,
                        "Packages/eathread/1.1.0/source/eathread_semaphore.cpp",
                    ),
                    Object(
                        Matching,
                        "Packages/eathread/1.1.0/source/eathread_thread.cpp",
                    ),
                ],
            },
        ]
    )

if config.platform == Platform.X360:
    config.libs.append(
        {
            "lib": "Game",
            "toolchain_version": config.linker_version,
            "cflags": cflags_game,
            "host": False,
            "progress_category": "sdk",  # str | List[str]
            "objects": [
                Object(NonMatching, "Speed/Xenon/link_fix.cpp"),
            ],
        }
    )

# # Custom build step for hashing
# config.custom_build_rules = [
#     {
#         "name": "hashgen",
#         "command": f"$python tools/hasher.py $in $out",
#         "description": "HASH $out",
#     }
# ]

# # Compile steps to automatically generate the headers containing hashes (e.g. BINHASH)
# precompile_steps = []

# sourcelist_files: list[Path] = [
#     Path("src") / object.name for object in config.libs[0]["objects"]
# ]

# for src_path in sourcelist_files:
#     gen_header = Path(
#         str(src_path).replace("SourceLists", "Src/Generated/Hashes/")
#     ).with_suffix(".h")
#     precompile_steps.append(
#         {
#             "rule": "hashgen",
#             "inputs": str(src_path),
#             "outputs": str(gen_header),
#         }
#     )

# config.custom_build_steps = {"pre-compile": precompile_steps}


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
    ProgressCategory("libc", "STD Code"),
    ProgressCategory("libs", "Library Code"),
]
config.progress_each_module = args.verbose

# objdiff empareja simbolos por NOMBRE, y el splitter desambigua los simbolos
# locales homonimos anadiendoles la direccion (`_803906C0`). El nuestro sale con
# el nombre pelado que emite GCC, asi que sin este mapeo objdiff no los empareja
# y la funcion se lee al 0% aunque el codigo case. `tools/project.py` conserva
# `symbol_mappings` de un objdiff.json existente, pero objdiff.json esta en
# .gitignore: hay que reponerlo en cada `configure`.
OBJDIFF_SYMBOL_MAPPINGS: Dict[str, Dict[str, str]] = {
    "main/Packages/eathread/1.1.0/source/eathread_thread": {
        "_GLOBAL_.I.AllocateThreadDynamicData__Q22EA6Threadv":
            "GlobalThreadDynamicDataInit",
    },
    "main/Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface": {
        "__static_initialization_and_destruction_0_803906C0": "__static_initialization_and_destruction_0",
    },
    # r36e: los bytes de este comparador son IDENTICOS; lo unico que cambia es
    # el sufijo del contador de clase local, que sale de un contador de
    # declaraciones de la unidad y no coincide entre los dos objetos. Medido:
    # 139.776 -> 139.792 B y 766 -> 767 funciones. OJO, el numero CADUCA en
    # cuanto cambie cualquier declaracion anterior de zGameplay, igual que los
    # alias que resuelve scripts/mangfix.py. YA CADUCO UNA VEZ: la r36f cambio
    # `PackedDecimal.h`, que zGameplay incluye, y el contador paso de .25193 a
    # .25190. Revision 2026-09-14: ahora .25167; mismos 16 B y cero relocs.
    # Si el informe da zGameplay al 99,988% con `fncmp` a cero, revisar esto.
    "main/Speed/Indep/SourceLists/zGameplay": {
        "Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.35326":
            "Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.25167",
    },
}

# r46: `asd2` ya no existe. Era el trozo que el troceador cortaba del final de
# gc_interface --- los dos `_GLOBAL_.I./.D.` y sus entradas de .ctors/.dtors ---
# y su rango esta fundido en gc_interface.cpp en splits.txt. Nuestros simbolos
# se llaman ya igual que los del objetivo, asi que no hacen falta mapeos: el
# unico que sigue haciendo falta es el del static-init con sufijo, que esta
# arriba en OBJDIFF_SYMBOL_MAPPINGS.
OBJDIFF_SHARED_SOURCES: Dict[str, str] = {}
OBJDIFF_SHARED_SOURCE_MAPPINGS: Dict[str, Dict[str, str]] = {}


def apply_objdiff_symbol_mappings() -> None:
    path = Path("objdiff.json")
    if not path.is_file():
        return
    with open(path, "r", encoding="utf-8") as r:
        data = json.load(r)
    changed = False
    units_by_name = {unit["name"]: unit for unit in data.get("units", [])}
    for unit in data.get("units", []):
        name = unit.get("name")
        extra = dict(OBJDIFF_SYMBOL_MAPPINGS.get(name, {}))
        donor_name = OBJDIFF_SHARED_SOURCES.get(name) if config.version == "GOWE69" else None
        if donor_name:
            donor = units_by_name.get(donor_name, {})
            base_path = donor.get("base_path")
            source_path = donor.get("metadata", {}).get("source_path")
            if not base_path or not source_path:
                raise RuntimeError(f"Missing objdiff shared source: {donor_name}")
            if unit.get("base_path") != base_path:
                unit["base_path"] = base_path
                changed = True
            metadata = unit.setdefault("metadata", {})
            if metadata.get("source_path") != source_path:
                metadata["source_path"] = source_path
                changed = True
            extra.update(OBJDIFF_SHARED_SOURCE_MAPPINGS[name])
        if not extra:
            continue
        mappings = unit.get("symbol_mappings") or {}
        if any(mappings.get(k) != v for k, v in extra.items()):
            mappings.update(extra)
            unit["symbol_mappings"] = mappings
            changed = True
    if changed:
        with open(path, "w", encoding="utf-8") as w:
            json.dump(data, w, indent=2)


if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
    apply_objdiff_symbol_mappings()
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)

# fake commit
