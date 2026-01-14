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
config.compilers_tag = "20251015"

if version_num in [0]:
    config.platform = Platform.GC_WII
    config.dtk_tag = "v1.6.2"
    config.binutils_tag = "2.42-1"
elif version_num in [1]:
    config.platform = Platform.X360
    config.dtk_tag = "v0.1.1"
    config.binutils_tag = "2.42-1"
elif version_num in [2]:
    config.platform = Platform.PS2
    config.binutils_tag = "2.45"

config.objdiff_tag = "v3.3.0"
config.sjiswrap_tag = "v1.2.0"
config.wibo_tag = "1.0.0"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"

if config.platform == Platform.GC_WII:
    config.asflags = [
        "-mgekko",
        "--strip-local-absolute",
        "-I include",
        f"-I build/{config.version}/include",
        f"--defsym BUILD_VERSION={version_num}",
    ]

    ldscript_path = Path("config") / config.version / "ldscript.ld"
    config.ldflags = ["-T", str(ldscript_path)]

    # Optional numeric ID for decomp.me preset
    # Can be overridden in libraries or objects
    config.scratch_preset_id = 176
elif config.platform == Platform.X360:
    config.ldflags = [
        "/NODEFAULTLIB",
        "/MACHINE:PPCBE",
        "/XEX:NO",
        f"/PDB:./build/{config.version}/default.pdb",
        f"/DEBUG",
        # f"/LTCG",
        f"/VERBOSE",
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

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
if config.platform == Platform.GC_WII:
    config.linker_version = "ProDG/3.9.3"

    cflags_base = [
        "-O1",
        "-gdwarf",
        # "-Wa,-L",
        # "-Wall",
        # "-Wreturn-type", # enable at some point
        "-Wno-ctor-dtor-privacy",  # because of AttribSys for example
        "-Woverloaded-virtual",
        "-I src/Speed/Indep/Libs/Support/stlgc",
        "-I src/Speed/GameCube/Libs/stl/STLport-4.5/stlport",
        "-I src/Speed/GameCube/bWare/GameCube/bWare/GameCube/SN/include",
        "-I include",
        "-I src/Speed/GameCube/bWare/GameCube/dolphinsdk/include",
        "-I ./",
        "-I src",
        "-DEA_PLATFORM_GAMECUBE",
        "-DEA_REGION_AMERICA",
        "-DGEKKO",
        "-D_USE_MATH_DEFINES",
        f"-I build/{config.version}/include",
        f"-DBUILD_VERSION={version_num}",
        f"-DVERSION_{config.version}",
    ]

    # Debug flags
    if args.debug:
        cflags_base.append("-DDEBUG=1")
    else:
        cflags_base.append("-DNDEBUG=1")

    cflags_game = [
        *cflags_base,
        "-mps-nodf",
        # "-mfast-cast",
        "-G0",
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
        "-DLUA_NUMBER=float",
    ]

    config.extra_clang_flags = [
        "-std=gnu++98",
        "-DSN_TARGET_NGC",
        "-D__SN__",
        "-DCLANGD_DAMNIT",  # used in cases where intellisense breaks
    ]
elif config.platform == Platform.X360:
    config.linker_version = "X360/14.00.2110"

    cflags_base = [
        "/nologo",
        "/c",  # compile without linking
        "/wd4996",  # get rid of string deprecation warnings for now
        "/wd4355",  # gets rid of the warning 'this' used in base member initializer
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
        "/I src",
        "/DEA_PLATFORM_XENON",
        "/D_USE_MATH_DEFINES",
        f"/I build/{config.version}/include",
        f"/DBUILD_VERSION={version_num}",
        f"/DVERSION_{config.version}",
    ]

    cflags_game = [
        *cflags_base,
        "/DLUA_NUMBER=float",
        "/DMILESTONE_OPT",
    ]

    config.extra_clang_flags = [
        "-std=c++03",
        "-D_WIN32",
        "-D_WCHAR_T_DEFINED",
        "-fms-extensions",
    ]
elif config.platform == Platform.PS2:
    config.linker_version = "PS2/ee-gcc2.9-991111"

    cflags_base = [
        "-O2",
        "-g2",
        # "-Wall",
        "-I src/Speed/Indep/Libs/Support/stlps2",
        "-I src/Speed/PSX2/Libs/stl/gpp",
        "-I src/Speed/PSX2/bWare/src/ee/include",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/ee/include",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111/include",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/ee",
        "-I src/Speed/PSX2/bWare/src/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111",
        "-I src",
        "-DEA_PLATFORM_PLAYSTATION2",
        "-DEA_BUILD_A124",
        "-D_NOTHREADS",  # TODO is this necessary?
        f"-I build/{config.version}/include",
        f"-DBUILD_VERSION={version_num}",
        # f"-DVERSION_{config.version}", # TODO it's broken because of the dash?
    ]

    # Debug flags
    # TODO
    # if args.debug:
    # cflags_base.append("-DDEBUG=1")
    # else:
    #     cflags_base.append("-DNDEBUG=1")

    cflags_game = [
        *cflags_base,
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
        "-DMILESTONE_OPT",
    ]

    config.extra_clang_flags = [
        "-std=gnu++98",
    ]

cflags_cmn = [
    *cflags_game,
    #    "-x c++"
]

# Metrowerks library flags
cflags_runtime = [*cflags_base]

cflags_odemuexi = [*cflags_base]

cflags_amcstub = [*cflags_base]

cflags_libc = [*cflags_base]


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "toolchain_version": "ProDG/3.9.3",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


Matching = True  # Object matches and should be linked
NonMatching = False  # Object does not match and should not be linked
Equivalent = (
    config.non_matching
)  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


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
            Object(NonMatching, "Speed/Indep/SourceLists/zAnim.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zAttribSys.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zBWare.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zCamera.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zComms.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zDebug.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zDynamics.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEagl4Anim.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEAXSound.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEAXSound2.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zEcstasy.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zFe.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zFe2.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zFEng.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zFoundation.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zGameModes.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zGameplay.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zLua.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMain.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMisc.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMiscSmall.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zMission.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zPhysics.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zPhysicsBehaviors.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zPlatform.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zRender.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zSim.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zSpeech.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zTrack.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zWorld.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zWorld2.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zOnline.cpp"),
            Object(NonMatching, "Speed/Indep/SourceLists/zFeOverlay.cpp"),
        ],
    },
    {
        "lib": "rcmp",
        "toolchain_version": config.linker_version,
        "cflags": cflags_game,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmpbase.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmp2real.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/rcmp_mad_codec.cpp"),
            Object(
                NonMatching,
                "egami/rcmp/dev/source/decoder/cmn/rcmp_mad_codec_chunk_types.cpp",
            ),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/maddec.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/maddeca.cpp"),
            Object(NonMatching, "egami/rcmp/dev/source/decoder/cmn/madidct.cpp"),
        ],
    },
    {
        "lib": "snd",
        "toolchain_version": config.linker_version,
        "cflags": cflags_cmn,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(NonMatching, "snd/source/library/cmn/saems.c"),
            Object(NonMatching, "snd/source/library/cmn/saemsamb.c"),
            Object(NonMatching, "snd/source/library/cmn/saemsstr.c"),
            Object(NonMatching, "snd/source/library/cmn/saemstimupdt.c"),
            Object(NonMatching, "snd/source/library/cmn/salloc.c"),
            Object(NonMatching, "snd/source/library/cmn/sbadd.c"),
            Object(NonMatching, "snd/source/library/cmn/sbpatinf.c"),
            Object(NonMatching, "snd/source/library/cmn/sgetpvol.c"),
            Object(NonMatching, "snd/source/library/cmn/ssballoc.c"),
            Object(NonMatching, "snd/source/library/cmn/ssbhdrcpy.c"),
            Object(NonMatching, "snd/source/library/cmn/ssbhdrsze.c"),
            Object(NonMatching, "snd/source/library/cmn/ssbplay.c"),
            Object(NonMatching, "snd/source/library/cmn/ssbremove.c"),
            Object(NonMatching, "snd/source/library/cmn/ssbvalid.c"),
            Object(NonMatching, "snd/source/library/gc/sscalcfx.c"),
            Object(NonMatching, "snd/source/library/gc/ssdfx.c"),
            Object(NonMatching, "snd/source/library/gc/sdspmix.c"),
            Object(NonMatching, "snd/source/library/cmn/seffect.c"),
            Object(NonMatching, "snd/source/library/cmn/sevent.cpp"),
            Object(NonMatching, "snd/source/library/cmn/sfxlevel.c"),
            Object(NonMatching, "snd/source/library/mix/sfxrevc.c"),
            Object(NonMatching, "snd/source/library/cmn/sgetdata.c"),
            Object(NonMatching, "snd/source/library/cmn/sinitdts.c"),
            Object(NonMatching, "snd/source/library/cmn/smemcpy.c"),
            Object(NonMatching, "snd/source/library/cmn/smemman.c"),
            Object(NonMatching, "snd/source/library/mix/smixer.c"),
            Object(NonMatching, "snd/source/library/gc/snddrv.c"),
            Object(NonMatching, "snd/source/library/cmn/spatkey.c"),
            Object(NonMatching, "snd/source/library/cmn/spitch.c"),
            Object(NonMatching, "snd/source/library/cmn/spktplay.c"),
            Object(NonMatching, "snd/source/library/cmn/splysdef.c"),
            Object(NonMatching, "snd/source/library/cmn/spoutlat.c"),
            Object(NonMatching, "snd/source/library/cmn/srandom.c"),
            Object(NonMatching, "snd/source/library/cmn/srender.c"),
            Object(NonMatching, "snd/source/library/cmn/sresopat.c"),
            Object(NonMatching, "snd/source/library/cmn/sserver.c"),
            Object(NonMatching, "snd/source/library/cmn/ssine.c"),
            Object(NonMatching, "snd/source/library/cmn/sst.c"),
            Object(NonMatching, "snd/source/library/cmn/sst3dpos.c"),
            Object(NonMatching, "snd/source/library/cmn/sstcrtap.c"),
            Object(NonMatching, "snd/source/library/cmn/sstfxlev.c"),
            Object(NonMatching, "snd/source/library/cmn/sstgetrp.c"),
            Object(NonMatching, "snd/source/library/cmn/sstgetpv.c"),
            Object(NonMatching, "snd/source/library/cmn/ssthighp.c"),
            Object(NonMatching, "snd/source/library/cmn/ssthold.c"),
            Object(NonMatching, "snd/source/library/cmn/sstlowp.c"),
            Object(NonMatching, "snd/source/library/cmn/sstop.c"),
            Object(NonMatching, "snd/source/library/cmn/sstovrhd.c"),
            Object(NonMatching, "snd/source/library/cmn/sstpmult.c"),
            Object(NonMatching, "snd/source/library/cmn/sstqreqi.c"),
            Object(NonMatching, "snd/source/library/cmn/sstrmdry.cpp"),
            Object(NonMatching, "snd/source/library/cmn/sstrstat.c"),
            Object(NonMatching, "snd/source/library/cmn/sstsetgl.c"),
            Object(NonMatching, "snd/source/library/cmn/sststat.c"),
            Object(NonMatching, "snd/source/library/cmn/ssttmul.c"),
            Object(NonMatching, "snd/source/library/cmn/sstvol.c"),
            Object(NonMatching, "snd/source/library/cmn/ssys.cpp"),
            Object(NonMatching, "snd/source/library/cmn/ssysinit.c"),
            Object(NonMatching, "snd/source/library/cmn/ssysserv.c"),
            Object(NonMatching, "snd/source/library/cmn/ssysveccsismutex.cpp"),
            Object(NonMatching, "snd/source/library/cmn/stagpat.c"),
            Object(NonMatching, "snd/source/library/cmn/stimemul.c"),
            Object(NonMatching, "snd/source/library/cmn/stimerem.c"),
            Object(NonMatching, "snd/source/library/cmn/stpparse.c"),
            Object(NonMatching, "snd/source/library/mix/stretch.c"),
            Object(NonMatching, "snd/source/library/cmn/svecreal.cpp"),
            Object(NonMatching, "snd/source/library/cmn/svol.c"),
            Object(NonMatching, "snd/source/library/mix/sx87d16.c"),
            Object(NonMatching, "snd/source/library/extern/coda/cmn/coda.cpp"),
            Object(NonMatching, "snd/source/library/cmn/s3dlow.c"),
            Object(NonMatching, "snd/source/library/gc/saramman.c"),
            Object(NonMatching, "snd/source/library/cmn/satospkr.c"),
            Object(NonMatching, "snd/source/library/cmn/sclcptch.c"),
            Object(NonMatching, "snd/source/library/cmn/sctlfilt.c"),
            Object(NonMatching, "snd/source/library/cmn/sctrldry.cpp"),
            Object(NonMatching, "snd/source/library/cmn/sdownmix.cpp"),
            Object(NonMatching, "snd/source/library/mix/sfamplf.c"),
            Object(NonMatching, "snd/source/library/mix/sfbpffir8.c"),
            Object(NonMatching, "snd/source/library/mix/sfecho.c"),
            Object(NonMatching, "snd/source/library/mix/sfft24.c"),
            Object(NonMatching, "snd/source/library/mix/sfhpffir8.c"),
            Object(NonMatching, "snd/source/library/mix/sfilter.c"),
            Object(NonMatching, "snd/source/library/mix/sfir.c"),
            Object(NonMatching, "snd/source/library/mix/sfir8.c"),
            Object(NonMatching, "snd/source/library/mix/sflpf.c"),
            Object(NonMatching, "snd/source/library/mix/sflpffir8.c"),
            Object(NonMatching, "snd/source/library/mix/sfmixer.c"),
            Object(NonMatching, "snd/source/library/mix/sfreson.c"),
            Object(NonMatching, "snd/source/library/mix/sfsplit.c"),
            Object(NonMatching, "snd/source/library/mix/sfsrc.c"),
            Object(NonMatching, "snd/source/library/cmn/sgettag.c"),
            Object(NonMatching, "snd/source/library/mix/shipass.c"),
            Object(NonMatching, "snd/source/library/mix/sinit16.c"),
            Object(NonMatching, "snd/source/library/mix/sinitut.c"),
            Object(NonMatching, "snd/source/library/mix/sinitxa.c"),
            Object(NonMatching, "snd/source/library/cmn/slib.c"),
            Object(NonMatching, "snd/source/library/cmn/slinklst.c"),
            Object(NonMatching, "snd/source/library/mix/slinkmix.c"),
            Object(NonMatching, "snd/source/library/cmn/slowpass.c"),
            Object(NonMatching, "snd/source/library/cmn/smemhigh.cpp"),
            Object(NonMatching, "snd/source/library/mix/smixc.c"),
            Object(NonMatching, "snd/source/library/mix/smixfram.c"),
            Object(NonMatching, "snd/source/library/mix/smixhip.c"),
            Object(NonMatching, "snd/source/library/mix/smixlowp.c"),
            Object(NonMatching, "snd/source/library/mix/smixptch.c"),
            Object(NonMatching, "snd/source/library/mix/smixtmul.c"),
            Object(NonMatching, "snd/source/library/cmn/SNDI_cos.c"),
            Object(NonMatching, "snd/source/library/cmn/SNDI_mult16.c"),
            Object(NonMatching, "snd/source/library/cmn/SNDI_root1x.c"),
            Object(NonMatching, "snd/source/library/cmn/SNDI_sin.c"),
            Object(NonMatching, "snd/source/library/cmn/sover.c"),
            Object(NonMatching, "snd/source/library/cmn/spantoaz.c"),
            Object(NonMatching, "snd/source/library/cmn/spat2hdr.c"),
            Object(NonMatching, "snd/source/library/cmn/spktctoh.c"),
            Object(NonMatching, "snd/source/library/cmn/srrange.c"),
            Object(NonMatching, "snd/source/library/cmn/sstopall.c"),
            Object(NonMatching, "snd/source/library/cmn/ssysreal.c"),
            Object(NonMatching, "snd/source/library/mix/supf.c"),
            Object(NonMatching, "snd/source/library/mix/suplf.c"),
            Object(NonMatching, "snd/source/library/mix/supmutf.c"),
            Object(NonMatching, "snd/source/library/mix/supmutlf.c"),
            Object(NonMatching, "snd/source/library/mix/supmutpf.c"),
            Object(NonMatching, "snd/source/library/mix/suppf.c"),
            Object(NonMatching, "snd/source/library/mix/supxaf.cpp"),
            Object(NonMatching, "snd/source/library/mix/supxalf.cpp"),
            Object(NonMatching, "snd/source/library/mix/supxapf.cpp"),
            Object(NonMatching, "snd/source/library/extern/coda/cmn/eaxadecf.cpp"),
            Object(NonMatching, "snd/source/library/extern/coda/cmn/mtdecf.cpp"),
            Object(NonMatching, "snd/source/library/cmn/sattrdef.c"),
            Object(NonMatching, "snd/source/library/cmn/sexithndl.c"),
            Object(NonMatching, "snd/source/library/mix/sfrsf.c"),
            Object(NonMatching, "snd/source/library/cmn/sgparse.cpp"),
            Object(NonMatching, "snd/source/library/cmn/SNDI_findprime.c"),
            Object(NonMatching, "snd/source/library/mix/scrsfl.c"),
        ],
    },
    {
        "lib": "realcore",
        "toolchain_version": config.linker_version,
        "cflags": cflags_game,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(NonMatching, "realcore/source/input/cmn/device.cpp"),
            Object(NonMatching, "realcore/source/file/cmn/filesys.cpp"),
            Object(NonMatching, "realcore/source/file/cmn/filesysopts.cpp"),
            Object(NonMatching, "realcore/source/file/cmn/syncfile.cpp"),
            Object(NonMatching, "realcore/source/file/cmn/hlafile.cpp"),
            Object(NonMatching, "realcore/source/file/cmn/hlsfile.cpp"),
            Object(NonMatching, "realcore/source/system/cmn/timer.cpp"),
            Object(NonMatching, "realcore/source/system/cmn/systask.cpp"),
            Object(NonMatching, "realcore/source/system/gc/threads.cpp"),
            Object(NonMatching, "realcore/source/system/gc/signals.cpp"),
            Object(NonMatching, "realcore/source/system/debug/cmn/printstr.cpp"),
            Object(NonMatching, "realcore/source/system/debug/cmn/abortmsg.cpp"),
            Object(NonMatching, "realcore/source/std/cmn/memclear.cpp"),
            Object(NonMatching, "realcore/source/std/cmn/exit.cpp"),
            Object(NonMatching, "realcore/source/system/gc/timerthread.cpp"),
            Object(NonMatching, "realcore/source/system/gc/memfill.cpp"),
        ],
    },
    {
        "lib": "realmemcard",
        "toolchain_version": config.linker_version,
        "cflags": cflags_game,
        "host": False,
        "progress_category": "libs",  # str | List[str]
        "objects": [
            Object(NonMatching, "realmemcard/source/lib/gc/gc_interface.cpp"),
            Object(NonMatching, "realmemcard/source/lib/gc/public.cpp"),
            Object(NonMatching, "realmemcard/source/lib/gc/trctasks.cpp"),
            Object(NonMatching, "realmemcard/source/lib/gc/tasks.cpp"),
        ],
    },
]

if config.platform == Platform.GC_WII:
    config.libs.extend(
        [
            {
                "lib": "libsn",
                "toolchain_version": config.linker_version,
                "cflags": cflags_runtime,
                "host": False,
                "progress_category": "libs",  # str | List[str]
                "objects": [
                    Object(NonMatching, "LibSN/crt0.s"),
                    Object(NonMatching, "LibSN/cvtll.c"),
                    Object(NonMatching, "LibSN/debug.c"),
                    Object(NonMatching, "LibSN/dummy.c"),
                    Object(NonMatching, "LibSN/fileserver.c"),
                    Object(NonMatching, "LibSN/FSasync.c"),
                    Object(NonMatching, "LibSN/inituser.c"),
                    Object(NonMatching, "LibSN/ppcdown.c"),
                    Object(NonMatching, "LibSN/prof.c"),
                    Object(NonMatching, "LibSN/proview.c"),
                    Object(NonMatching, "LibSN/sndvd.c"),
                    Object(NonMatching, "LibSN/tealeaf.c"),
                    Object(NonMatching, "LibSN/tors.c"),
                ],
            },
            {
                "lib": "misc",
                "toolchain_version": config.linker_version,
                "cflags": cflags_game,
                "host": False,
                "progress_category": "libs",  # str | List[str]
                "objects": [
                    Object(NonMatching, "crt2D1.tmp"),
                    Object(NonMatching, "ppc2D2.tmp"),
                    Object(NonMatching, "fil2D3.tmp"),
                    Object(NonMatching, "pro2D4.tmp"),
                    Object(NonMatching, "inituser.c"),
                    Object(NonMatching, "pro2D9.tmp"),
                    Object(NonMatching, "tea2Da.tmp"),
                    Object(NonMatching, "FSasync.c"),
                    Object(NonMatching, "sndvd.c"),
                    Object(NonMatching, "atexit.c"),
                    Object(NonMatching, "qsort.c"),
                    Object(NonMatching, "sn_malloc.c"),
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
                    Object(NonMatching, "base.cpp"),
                    Object(NonMatching, "model.cpp"),
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
                    Object(NonMatching, "fontchar.cpp"),
                    Object(NonMatching, "fontcreate.cpp"),
                    Object(NonMatching, "fontdraw.cpp"),
                    Object(NonMatching, "fontdrawf.cpp"),
                    Object(NonMatching, "fontkern.cpp"),
                    Object(NonMatching, "fontnull.cpp"),
                    Object(NonMatching, "loccore.cpp"),
                    Object(NonMatching, "ustrcpy.cpp"),
                    Object(NonMatching, "creates.cpp"),
                    Object(NonMatching, "shptype.cpp"),
                    Object(NonMatching, "swizzlesize.cpp"),
                    Object(NonMatching, "cluttype.cpp"),
                    Object(NonMatching, "csis/dev/source/library/cmn/csis.cpp"),
                    Object(NonMatching, "dummy.c"),
                    Object(NonMatching, "prodg_fixes.cpp"),
                    Object(NonMatching, "codec/cmn/refsize.cpp"),
                ],
            },
            DolphinLib(
                "ar",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ar/ar.c",
                    ),
                ],
            ),
            DolphinLib(
                "arq",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/arq/arq.c",
                    ),
                ],
            ),
            DolphinLib(
                "ax",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXAlloc.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXAux.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXCL.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AX.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXOut.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXSPB.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXVPB.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ax/AXProf.c",
                    ),
                ],
            ),
            DolphinLib(
                "card",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDBios.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDBlock.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDDir.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDCheck.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDMount.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDFormat.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDCreate.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDRead.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDWrite.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDDelete.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDStat.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDUnlock.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/card/CARDRdwr.c",
                    ),
                ],
            ),
            DolphinLib(
                "dsp",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dsp/dsp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dsp/dsp_task.c",
                    ),
                ],
            ),
            DolphinLib(
                "os",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSFatal.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OS.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSAlarm.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSAlloc.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSArena.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSAudioSystem.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSCache.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSContext.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSError.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSFont.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSInterrupt.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSLink.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSMemory.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSReset.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSResetSW.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSRtc.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSSync.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSThread.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSTime.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/OSReboot.c",
                    ),
                    Object(
                        Matching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/os/__ppc_eabi_init.cpp",
                    ),
                ],
            ),
            DolphinLib(
                "db",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/db/db.c",
                    ),
                ],
            ),
            DolphinLib(
                "mtx",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/mtx.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/mtx44.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/mtx/vec.c",
                    ),
                ],
            ),
            DolphinLib(
                "dvd",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdfs.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvd.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdqueue.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvderror.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdFatal.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/fstload.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/dvd/dvdlow.c",
                    ),
                ],
            ),
            DolphinLib(
                "vi",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/vi/vi.c",
                    ),
                ],
            ),
            DolphinLib(
                "pad",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/pad/Padclamp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/pad/Pad.c",
                    ),
                ],
            ),
            DolphinLib(
                "ai",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ai/ai.c",
                    ),
                ],
            ),
            DolphinLib(
                "gx",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXInit.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXFrameBuf.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXAttr.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXFifo.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXMisc.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXDisplayList.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXLight.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXTexture.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXBump.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXTev.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXPixel.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXTransform.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/gx/GXPerf.c",
                    ),
                ],
            ),
            DolphinLib(
                "exi",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/exi/EXIBios.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/exi/EXIUart.c",
                    ),
                ],
            ),
            DolphinLib(
                "si",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SIBios.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISamplingRate.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISteering.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISteeringXfer.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/si/SISteeringAuto.c",
                    ),
                ],
            ),
            DolphinLib(
                "ip",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPSocket.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPPPPoE.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPPap.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPChap.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPLcp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IP.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPIcmp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPRoute.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPFrag.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPUdp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPEther.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IFFifo.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPTcpTimeWait.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPTcp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPTcpOutput.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPTcpTimer.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPTcpUser.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPDns.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPDhcp.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPZero.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPPPP.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/ip/IPArp.c",
                    ),
                ],
            ),
            DolphinLib(
                "net",
                [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/net/eth.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/net/ethsec.c",
                    ),
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/dolphinsdk/src/net/md5.c",
                    ),
                ],
            ),
            {
                "lib": "OdemuExi2",
                "toolchain_version": config.linker_version,
                "cflags": cflags_odemuexi,
                "progress_category": "sdk",  # str | List[str]
                "objects": [
                    Object(
                        NonMatching,
                        "Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c",
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
                    Object(NonMatching, "libc/vprintf.c"),
                    Object(NonMatching, "libc/vsprintf.c"),
                    Object(NonMatching, "libc/memcpy.c"),
                    Object(NonMatching, "libc/memmove.c"),
                    Object(NonMatching, "libc/memset.c"),
                    Object(NonMatching, "libc/strcasecmp.c"),
                    Object(NonMatching, "libc/strchr.c"),
                    Object(NonMatching, "libc/strcmp.c"),
                    Object(NonMatching, "libc/strcpy.c"),
                    Object(NonMatching, "libc/strlen.c"),
                    Object(NonMatching, "libc/strncmp.c"),
                    Object(NonMatching, "libc/strncpy.c"),
                    Object(NonMatching, "libc/vfprintf.c"),
                    Object(NonMatching, "libc/impure.c"),
                    Object(NonMatching, "libc/vfprintf_1.c"),
                    Object(NonMatching, "libc/locale.c"),
                    Object(NonMatching, "libc/_tolower.c"),
                    Object(NonMatching, "libc/math_support.c"),
                    Object(NonMatching, "libc/mbtowc_r.c"),
                    Object(NonMatching, "libc/libgcc2.c"),
                    Object(NonMatching, "libc/memchr.c"),
                    Object(NonMatching, "libc/libgcc2_1.c"),
                    Object(NonMatching, "libc/libgcc2_2.c"),
                    Object(NonMatching, "libc/libgcc2_3.c"),
                    Object(NonMatching, "libc/libgcc2_5.c"),
                    Object(NonMatching, "libc/libgcc2_6.c"),
                    Object(NonMatching, "libc/libgcc2_7.c"),
                    Object(NonMatching, "libc/libgcc2_8.c"),
                    Object(NonMatching, "libc/libgcc2_9.c"),
                    Object(NonMatching, "libc/libgcc2_10.c"),
                    Object(NonMatching, "libc/e_pow.c"),
                    Object(NonMatching, "libc/e_sqrt.c"),
                    Object(NonMatching, "libc/s_fabs.c"),
                    Object(NonMatching, "libc/ef_pow.c"),
                    Object(NonMatching, "libc/ef_sqrt.c"),
                    Object(NonMatching, "libc/sf_cos.c"),
                    Object(NonMatching, "libc/sf_fabs.c"),
                    Object(NonMatching, "libc/sf_sin.c"),
                    Object(NonMatching, "libc/s_scalbn.c"),
                    Object(NonMatching, "libc/sf_scalbn.c"),
                    Object(NonMatching, "libc/kf_cos.c"),
                    Object(NonMatching, "libc/kf_sin.c"),
                    Object(NonMatching, "libc/ef_rem_pio2.c"),
                    Object(NonMatching, "libc/s_copysign.c"),
                    Object(NonMatching, "libc/sf_copysign.c"),
                    Object(NonMatching, "libc/kf_rem_pio2.c"),
                    Object(NonMatching, "libc/sf_floor.c"),
                    Object(NonMatching, "libc/e_acos.c"),
                    Object(NonMatching, "libc/e_atan2.c"),
                    Object(NonMatching, "libc/s_atan.c"),
                    Object(NonMatching, "libc/ef_atan2.c"),
                    Object(NonMatching, "libc/vfscanf.c"),
                    Object(NonMatching, "libc/strtod2.c"),
                    Object(NonMatching, "sn_buf.cpp"),
                    Object(NonMatching, "libc/tolower.c"),
                    Object(NonMatching, "libc/puts.c"),
                    Object(NonMatching, "libc/atoi.c"),
                    Object(NonMatching, "libc/bsearch.c"),
                    Object(NonMatching, "libc/memcmp.c"),
                    Object(NonMatching, "libc/strstr.c"),
                    Object(NonMatching, "libc/strtol.c"),
                    Object(NonMatching, "libc/fvwrite.c"),
                    Object(NonMatching, "libc/wsetup.c"),
                    Object(NonMatching, "libc/fflush.c"),
                    Object(NonMatching, "libc/fopen.c"),
                    Object(NonMatching, "libc/fwalk.c"),
                    Object(NonMatching, "libc/makebuf.c"),
                    Object(NonMatching, "libc/stdio.c"),
                    Object(NonMatching, "libc/closer.c"),
                    Object(NonMatching, "libc/fstatr.c"),
                    Object(NonMatching, "libc/lseekr.c"),
                    Object(NonMatching, "libc/readr.c"),
                    Object(NonMatching, "libc/writer.c"),
                    Object(NonMatching, "libc/s_sin.c"),
                    Object(NonMatching, "libc/sf_atan.c"),
                    Object(NonMatching, "libc/sf_tan.c"),
                    Object(NonMatching, "libc/k_cos.c"),
                    Object(NonMatching, "libc/k_sin.c"),
                    Object(NonMatching, "libc/k_tan.c"),
                    Object(NonMatching, "libc/e_rem_pio2.c"),
                    Object(NonMatching, "libc/kf_tan.c"),
                    Object(NonMatching, "libc/k_rem_pio2.c"),
                    Object(NonMatching, "libc/s_floor.c"),
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
                Object(Matching, "Speed/Xenon/link_fix.cpp"),
            ],
        }
    )

# Custom build step for hashing
config.custom_build_rules = [
    {
        "name": "hashgen",
        "command": f"$python tools/hasher.py $in $out",
        "description": "HASH $out",
    }
]

# Compile steps to automatically generate the headers containing hashes (e.g. BINHASH)
precompile_steps = []

sourcelist_files: list[Path] = [
    Path("src") / object.name for object in config.libs[0]["objects"]
]

for src_path in sourcelist_files:
    gen_header = Path(
        str(src_path).replace("SourceLists", "Src/Generated/Hashes/")
    ).with_suffix(".h")
    precompile_steps.append(
        {
            "rule": "hashgen",
            "inputs": str(src_path),
            "outputs": str(gen_header),
        }
    )

config.custom_build_steps = {"pre-compile": precompile_steps}


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

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
