# Game versions
DEFAULT_VERSION = 1
VERSIONS = [
    "GOWE69",  # 0
]

# Include paths
cflags_includes = [
    # C/C++ stdlib
    "-i src/Speed/Indep/Libs/Support/stlgc",
    "-i src/Speed/GameCube/Libs/stl/STLport-4.5/stlport",
    "-i src/Speed/GameCube/bWare/GameCube/bWare/GameCube/SN/include",
    "-i include",
    "-i src/Speed/GameCube/bWare/GameCube/dolphinsdk/include",
    "-i ./",
    "-i src",
]
