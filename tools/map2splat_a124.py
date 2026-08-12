#!/usr/bin/env python3
"""Add the map-backed A124 library splits and symbols to the Splat config."""

from __future__ import annotations

import argparse
import re
from collections import defaultdict
from pathlib import Path


MAP_SECTIONS = {
    ".text",
    ".vutext",
    ".noover",
    ".rodata",
    ".gcc_except_table",
    ".over",
    ".data",
    ".lit4",
    ".sdata",
    ".sbss",
    ".bss",
}
CODE_SECTIONS = {".text", ".vutext", ".noover", ".over"}
DATA_SECTIONS = MAP_SECTIONS - CODE_SECTIONS
MAP_TO_CONFIG_OFFSET = 0xFF000

OBJECT_ROW = re.compile(
    r"^\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+\S+\s+(.*)$"
)
SYMBOL_LINE = re.compile(r"^(.+?) = (0x[0-9A-Fa-f]+);(?: // (.*))?$")


class MapObject:
    def __init__(self, section: str, address: int, size: int, raw_path: str):
        self.section = section
        self.address = address
        self.size = size
        self.raw_path = raw_path
        self.path = ""


class MapSymbol:
    def __init__(
        self,
        section: str,
        address: int,
        size: int,
        name: str,
        source: str,
    ):
        self.section = section
        self.address = address
        self.size = size
        self.name = name
        self.source = source


def is_hex(value: str) -> bool:
    return bool(re.fullmatch(r"[0-9A-Fa-f]+", value))


def is_object_path(value: str) -> bool:
    lower = value.lower()
    return "\\" in value and (
        ".a(" in lower or lower.endswith(".o") or ".obj" in lower
    )


def member_name(raw_path: str) -> str:
    value = raw_path
    if ".a(" in value.lower():
        value = value[value.find("(") + 1 : value.rfind(")")]
    value = value.rsplit("\\", 1)[-1].rsplit("/", 1)[-1]
    if value.lower().endswith(".obj"):
        value = value[:-4]
    elif value.lower().endswith(".o"):
        value = value[:-2]
    match = re.match(r"^(.*\.(?:c|cc|cpp|cxx|s|asm))(?:\..*)?$", value, re.IGNORECASE)
    if match:
        value = match.group(1)
    elif "." not in value:
        suffix = ".s" if value.lower() in {"crt0", "superbenderasm", "bmiscasm", "vu0cap", "vu1cap"} else ".c"
        value += suffix
    return value


def canonical_source_list_path(raw_path: str, config_paths: dict[str, str]) -> str:
    name = raw_path.rsplit("\\", 1)[-1]
    name = name[:-4] if name.lower().endswith(".obj") else name
    return config_paths.get(
        name.lower(), f"Speed/Indep/SourceLists/{name}"
    )


def load_existing_source_paths(root: Path) -> list[str]:
    paths = []
    src_root = root / "src"
    if src_root.exists():
        for path in src_root.rglob("*"):
            if path.is_file():
                paths.append(path.relative_to(src_root).as_posix())

    split_path = root / "config/GOWE69/splits.txt"
    if split_path.exists():
        for line in split_path.read_text().splitlines():
            if line.endswith(":") and not line.startswith(("\t", " ")):
                paths.append(line[:-1])
    return sorted(set(paths))


def source_path_for(raw_path: str, existing_paths: list[str]) -> str:
    lower = raw_path.lower().replace("\\", "/")
    member = member_name(raw_path)
    member_lower = member.lower()
    library = ""
    if "speed/indep/libs/" in lower:
        library = lower.split("speed/indep/libs/", 1)[1].split("/", 1)[0]

    candidates = [path for path in existing_paths if path.lower().endswith("/" + member_lower)]
    library_prefixes = {
        "snd": ("speed/indep/libs/snd/9/",),
        "realcore": ("speed/indep/libs/realcore/6.24.00/",),
        "path": ("speed/indep/libs/path/5.01.04/",),
        "csis": ("speed/indep/libs/csis/",),
        "spch": ("speed/indep/libs/spch/",),
        "eathread": ("packages/eathread/1.1.0/",),
        "realmemcard": ("packages/realmemcard/3.04.00-layer2/",),
        "vp6": ("packages/vp6/1.0.3/",),
        "rcmp": ("egami/rcmp/",),
        "juice": ("speed/indep/libs/juice/1.5.2a/",),
        "realgraph": ("speed/indep/libs/realgraph/6.09.01/",),
        "dirtysock": ("speed/indep/libs/dirtysock/",),
    }
    if library in library_prefixes:
        candidates = [
            path
            for path in candidates
            if path.lower().startswith(library_prefixes[library])
        ]
    if "/ps2-" in lower or "/ps2/" in lower:
        candidates = [path for path in candidates if "/gc/" not in path.lower()]
    if "speed/psx2" in lower:
        candidates = [
            path
            for path in candidates
            if "/gc/" not in path.lower() and path.lower().startswith("speed/psx2/")
        ]
    if "libs/realmemcard/3.04.00-layer2/" in lower:
        candidates = [path for path in candidates if "3.04.00-layer2" in path]
    if "libs/vp6/1.0.3/" in lower:
        candidates = [path for path in candidates if "vp6/1.0.3" in path]
    if candidates:
        candidates.sort(key=lambda path: ("/gc/" in path.lower(), len(path)))
        return candidates[0]

    if library == "realmemcard":
        versionless = [
            path
            for path in existing_paths
            if path.lower().startswith("packages/realmemcard/")
            and path.lower().endswith("/" + member_lower)
            and "/gc/" not in path.lower()
        ]
        if versionless:
            return versionless[0].replace("3.04.01-layer2", "3.04.00-layer2")
    if library == "vp6":
        versionless = [
            path
            for path in existing_paths
            if path.lower().startswith("packages/vp6/")
            and path.lower().endswith("/" + member_lower)
            and "/gc/" not in path.lower()
        ]
        if versionless:
            return versionless[0].replace("1.0.6", "1.0.3")

    if "speed/indep/libs/" in lower:
        after = lower.split("speed/indep/libs/", 1)[1]
        library, _, remainder = after.partition("/")
        if library == "snd":
            root = "Speed/Indep/Libs/snd/9/source/library/cmn"
            if "/mix/" in remainder:
                root = "Speed/Indep/Libs/snd/9/source/library/mix"
            elif "/coda/" in remainder:
                root = "Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn"
            return f"{root}/{member}"
        if library == "realcore":
            if "realinput" in remainder:
                subdir = "input/ps2"
            elif "realsystem" in remainder:
                subdir = "system/ps2"
            elif "realdebug" in remainder:
                subdir = "debug/ps2"
            elif "realstd" in remainder:
                subdir = "std/cmn"
            else:
                subdir = "file/ps2" if "realfile" in remainder else "cmn"
            return f"Speed/Indep/Libs/realcore/6.24.00/source/{subdir}/{member}"
        if library == "path":
            return f"Speed/Indep/Libs/path/5.01.04/source/cmn/{member}"
        if library == "csis":
            return f"Speed/Indep/Libs/csis/dev/source/library/cmn/{member}"
        if library == "spch":
            return f"Speed/Indep/Libs/spch/dev/source/library/cmn/{member}"
        if library == "eathread":
            return f"Packages/eathread/1.1.0/source/{member}"
        if library == "realmemcard":
            return f"Packages/realmemcard/3.04.00-layer2/source/lib/ps2/{member}"
        if library == "vp6":
            subdir = "decode/cmn"
            if "systemdependant" in member.lower() or member.lower() == "scratchpad.c":
                subdir = "decode/ps2"
            return f"Packages/vp6/1.0.3/source/{subdir}/{member}"
        if library == "rcmp":
            subdir = "decoder/ps2" if "ps2" in member.lower() else "decoder/cmn"
            if member.lower() in {"avplayer.cpp", "avsubtitle.cpp", "audioplayer.cpp"}:
                subdir = "av/cmn"
            return f"egami/rcmp/dev/source/{subdir}/{member}"
        if library == "juice":
            return f"Speed/Indep/Libs/juice/1.5.2a/source/ps2/{member}"
        if library == "realgraph":
            return f"Speed/Indep/Libs/realgraph/6.09.01/source/ps2/{member}"
        if library == "dirtysock":
            directory = remainder.split("/lib", 1)[0].rstrip("/")
            return f"Speed/Indep/Libs/dirtysock/source/{directory}/{member}"
        return f"Speed/Indep/Libs/{library}/source/{member}"

    if "speed/psx2/obj/" in lower:
        suffix = ".s" if member.lower() in {"crt0", "superbenderasm", "bmiscasm", "vu0cap", "vu1cap"} else ".c"
        if "." not in member:
            member += suffix
        return f"Speed/PSX2/Runtime/{member}"
    if "speed/psx2/" in lower:
        after = lower.split("speed/psx2/", 1)[1]
        directory = after.split("/lib", 1)[0].rstrip("/")
        return f"Speed/PSX2/{directory}/{member}"

    return f"Speed/PSX2/Runtime/{member}"


def parse_map(map_path: Path, source_list_paths: dict[str, str], existing_paths: list[str]):
    objects: list[MapObject] = []
    symbols: list[MapSymbol] = []
    current_section = ""
    current_file = ""

    for line in map_path.read_text(errors="replace").splitlines():
        fields = line.split()
        if len(fields) >= 4 and is_hex(fields[0]) and is_hex(fields[1]):
            if fields[3].startswith("."):
                current_section = fields[3]

        match = OBJECT_ROW.match(line)
        if not match:
            continue
        address = int(match.group(1), 16)
        size = int(match.group(2), 16)
        rest = match.group(3).strip()

        if is_object_path(rest):
            current_file = rest
            if current_section not in MAP_SECTIONS or size == 0:
                continue
            obj = MapObject(current_section, address, size, rest)
            if "sourcelists" in rest.lower():
                obj.path = canonical_source_list_path(rest, source_list_paths)
            else:
                obj.path = source_path_for(rest, existing_paths)
            objects.append(obj)
            continue

        if (
            current_section not in MAP_SECTIONS
            or not current_file
            or size == 0
            or rest.startswith(("<", "."))
            or is_object_path(rest)
        ):
            continue
        if "sourcelists" in current_file.lower():
            source = canonical_source_list_path(current_file, source_list_paths)
        else:
            source = source_path_for(current_file, existing_paths)
        symbols.append(MapSymbol(current_section, address, size, rest, source))

    return objects, symbols


def parse_existing_symbols(path: Path):
    pairs = set()
    names = set()
    for line in path.read_text().splitlines():
        match = SYMBOL_LINE.match(line)
        if match:
            name = match.group(1)
            address = int(match.group(2), 16)
            pairs.add((name, address))
            names.add(name)
    return pairs, names


def format_symbol(symbol: MapSymbol, allow_duplicated: bool) -> str:
    if symbol.section in CODE_SECTIONS:
        attributes = ["type:func"]
        if len(symbol.name) > 250:
            attributes.append(f"filename:func_{symbol.address:x}")
        if allow_duplicated:
            attributes.append("allow_duplicated:true")
        return f"{symbol.name} = {hex(symbol.address)}; // {' '.join(attributes)}"
    attributes = [f"size:{hex(symbol.size)}"]
    if allow_duplicated:
        attributes.append("allow_duplicated:true")
    return f"{symbol.name} = {hex(symbol.address)}; // {' '.join(attributes)}"


def symbol_blocks(symbols: list[MapSymbol], existing_pairs: set[tuple[str, int]]) -> str:
    grouped: dict[str, list[MapSymbol]] = defaultdict(list)
    order: list[str] = []
    seen = set(existing_pairs)
    for symbol in symbols:
        key = (symbol.name, symbol.address)
        if key in seen:
            continue
        seen.add(key)
        if symbol.source not in grouped:
            order.append(symbol.source)
        grouped[symbol.source].append(symbol)

    name_counts = defaultdict(int)
    address_counts = defaultdict(int)
    for source in order:
        for symbol in grouped[source]:
            name_counts[symbol.name] += 1
            address_counts[symbol.address] += 1

    chunks = []
    for source in order:
        entries = grouped[source]
        if not entries:
            continue
        chunks.extend(
            [
                "///////////////////////////////////////////////////////////////////",
                f"// {source}",
                "///////////////////////////////////////////////////////////////////",
                *[
                    format_symbol(
                        symbol,
                        name_counts[symbol.name] > 1 or address_counts[symbol.address] > 1,
                    )
                    for symbol in entries
                ],
            ]
        )
    return "\n".join(chunks) + ("\n" if chunks else "")


def config_lines(objects: list[MapObject], sections: set[str]) -> list[str]:
    result = []
    seen = set()
    for obj in objects:
        if obj.section not in sections or obj.raw_path.lower().find("sourcelists") >= 0:
            continue
        key = (obj.section, obj.address, obj.path)
        if key in seen:
            continue
        seen.add(key)
        section_type = "asmtu" if obj.section in CODE_SECTIONS else obj.section
        start = obj.address - MAP_TO_CONFIG_OFFSET
        result.append(f"    - [{hex(start)}, {section_type}, {obj.path}]")
    return result


def insert_before(lines: list[str], marker: str, additions: list[str]) -> None:
    if not additions:
        return
    index = next(i for i, line in enumerate(lines) if marker in line)
    lines[index:index] = additions


def insert_after(lines: list[str], marker: str, additions: list[str]) -> None:
    if not additions:
        return
    index = next(i for i, line in enumerate(lines) if marker in line)
    lines[index + 1 : index + 1] = additions


def update_config(config_path: Path, objects: list[MapObject]) -> None:
    lines = config_path.read_text().splitlines()
    lines = [
        line
        for line in lines
        if not (
            "idk" in line.lower()
            or (
                "0x4397b0" in line.lower()
                and "zOnline.cpp" in line
            )
            or (
                "0x4d7d50" in line.lower()
                and "zFeOverlay.cpp" in line
            )
            or (
                re.match(r"\s+- \[0x[0-9A-Fa-f]+,", line)
                and "SourceLists/" not in line
                and ", rest]" not in line
            )
        )
    ]
    lines = [
        "    # Map-backed library and runtime text splits"
        if line.strip() == "# TODO functions"
        else line
        for line in lines
    ]
    existing_entries = set()
    for line in lines:
        match = re.match(r"\s*- \[(0x[0-9A-Fa-f]+), [^,]+, (.+)\]$", line)
        if match:
            existing_entries.add((int(match.group(1), 16), match.group(2)))
    text_lines = config_lines(objects, {".text"})
    vutext_lines = config_lines(objects, {".vutext"})
    noover_lines = [
        f"    - [{hex(obj.address - MAP_TO_CONFIG_OFFSET)}, asmtu, {obj.path}]"
        for obj in objects
        if obj.section == ".noover" and "sourcelists" in obj.raw_path.lower()
    ]
    over_source_lines = []
    for obj in objects:
        if obj.section != ".over" or "sourcelists" not in obj.raw_path.lower():
            continue
        start = obj.address - MAP_TO_CONFIG_OFFSET
        if (start, obj.path) in existing_entries:
            continue
        over_source_lines.append(f"    - [{hex(start)}, asmtu, {obj.path}]")
    rodata_lines = config_lines(objects, {".rodata"})
    gcc_lines = config_lines(objects, {".gcc_except_table"})
    over_lines = config_lines(objects, {".over"})
    data_lines = config_lines(objects, {".data"})
    small_lines = config_lines(objects, {".lit4", ".sdata", ".sbss"})
    bss_lines = config_lines(objects, {".bss"})
    bss_objects = [obj for obj in objects if obj.section == ".bss" and "sourcelists" not in obj.raw_path.lower()]

    # The old placeholder begins at the first map-backed runtime object.
    insert_before(lines, "    # Map-backed library and runtime text splits", text_lines)
    insert_before(lines, "    - [0x439800, .rodata", vutext_lines + noover_lines)
    insert_before(lines, "    - [0x497580, asmtu, Speed/Indep/SourceLists/zOnline.cpp]", rodata_lines + gcc_lines)
    insert_before(
        lines,
        "    - [0x4b50c0, asmtu, Speed/Indep/SourceLists/zFeOverlay.cpp]",
        [line for line in over_source_lines if "0x4b09b0" in line],
    )
    insert_before(
        lines,
        "    - [0x4daf00, .data, Speed/Indep/SourceLists/zAI.cpp]",
        over_lines + [line for line in over_source_lines if "0x4d7d50" in line],
    )
    insert_after(
        lines,
        "    - [0x54bb68, .data, Speed/Indep/SourceLists/zFeOverlay.cpp]",
        data_lines + small_lines,
    )
    if bss_objects:
        last_bss = max(bss_objects, key=lambda obj: obj.address + obj.size)
        rest_start = last_bss.address + last_bss.size - MAP_TO_CONFIG_OFFSET
        lines = [
            f"    - [{hex(rest_start)}, .bss, rest]" if ", rest]" in line else line
            for line in lines
        ]
    insert_before(lines, ", .bss, rest]", bss_lines)
    config_path.write_text("\n".join(lines) + "\n")


def update_symbols(symbols_path: Path, symbols: list[MapSymbol]) -> None:
    current = symbols_path.read_text()
    current_lines = current.splitlines()
    generated_start = next(
        (
            i
            for i, line in enumerate(current_lines)
            if line.startswith(("// Speed/", "// Packages/", "// egami/", "// LibSN/"))
        ),
        len(current_lines),
    )
    if generated_start and current_lines[generated_start - 1] == "///////////////////////////////////////////////////////////////////":
        generated_start -= 1
    current = "\n".join(current_lines[:generated_start])
    if current and not current.endswith("\n"):
        current += "\n"
    symbols_path.write_text(current)
    existing_pairs, _ = parse_existing_symbols(symbols_path)
    block = symbol_blocks(symbols, existing_pairs)
    if not block:
        return
    symbols_path.write_text(current + block)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()

    root = args.root
    config_path = root / "config/SLES-53558-A124/config.yml"
    symbols_path = root / "config/SLES-53558-A124/symbols.txt"
    map_path = root / "orig/SLES-53558-A124/NFS.MAP"

    source_list_paths = {}
    for line in config_path.read_text().splitlines():
        match = re.search(r"Speed/Indep/SourceLists/([^]]+)", line)
        if match:
            path = match.group(0)
            source_list_paths[Path(path).name.lower()] = path

    existing_paths = load_existing_source_paths(root)
    objects, symbols = parse_map(map_path, source_list_paths, existing_paths)
    non_source_objects = [obj for obj in objects if "sourcelists" not in obj.raw_path.lower()]
    existing_pairs, _ = parse_existing_symbols(symbols_path)
    missing_symbols = [
        symbol for symbol in symbols if (symbol.name, symbol.address) not in existing_pairs
    ]

    print(f"map objects: {len(objects)} ({len(non_source_objects)} non-source)")
    print(f"missing symbols: {len(missing_symbols)}")
    for section in [".text", ".vutext", ".noover", ".rodata", ".gcc_except_table", ".over", ".data", ".lit4", ".sdata", ".sbss", ".bss"]:
        count = sum(obj.section == section and "sourcelists" not in obj.raw_path.lower() for obj in non_source_objects)
        if count:
            print(f"  {section}: {count} objects")

    if args.write:
        update_config(config_path, objects)
        update_symbols(symbols_path, symbols)


if __name__ == "__main__":
    main()
