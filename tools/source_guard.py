#!/usr/bin/env python3

import argparse
import re
import sys
from pathlib import Path

RAW_POINTER_REGEX = re.compile(r"cast<[^>]*\*[^>]*>\(.*\) \+ 0x")
LBL_REGEX = re.compile(r"lbl_")
ASM_REGEX = re.compile(r"\basm\b")
INCLUDE_REGEX = re.compile(r'^\s*#include\s+"([^"]+)"')

RAW_POINTER_ERROR = (
    "Access of struct fields through raw pointer math is not allowed. Don't try to rewrite it in a "
    "hacky way just to get it to compile. You must use proper field access or virtual function calls"
)
LBL_ERROR = "lbl_ is not allowed, use the constant directly"
ASM_ERROR = "asm is prohibited in most files, this is a multiplatform game."

ASM_JUMBOS = {"zSpeech", "zEAXSound", "zEAXSound2"}
SOURCE_EXTS = {".c", ".cc", ".cpp", ".cxx"}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Guard source files included by a jumbo source list."
    )
    parser.add_argument("source_list", help="Path to jumbo source-list file")
    parser.add_argument("stamp", help="Output stamp path")
    return parser.parse_args()


def resolve_include_path(source_list: Path, include_path: str) -> Path:
    repo_root = Path(__file__).resolve().parent.parent
    candidates = [
        repo_root / include_path,
        repo_root / "src" / include_path,
        source_list.parent / include_path,
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    return candidates[1]


def list_included_sources(source_list: Path) -> list[Path]:
    included_sources: list[Path] = []
    for line in source_list.read_text(encoding="utf-8").splitlines():
        match = INCLUDE_REGEX.match(line)
        if not match:
            continue
        include_path = resolve_include_path(source_list, match.group(1))
        if include_path.suffix.lower() in SOURCE_EXTS:
            included_sources.append(include_path)
    return included_sources


def check_file(
    source_file: Path,
    check_asm: bool,
) -> list[tuple[int, str]]:
    violations: list[tuple[int, str]] = []
    text = source_file.read_text(encoding="utf-8")
    for line_number, line in enumerate(text.splitlines(), start=1):
        if RAW_POINTER_REGEX.search(line):
            violations.append((line_number, RAW_POINTER_ERROR))
        if LBL_REGEX.search(line):
            violations.append((line_number, LBL_ERROR))
        if check_asm and ASM_REGEX.search(line):
            violations.append((line_number, ASM_ERROR))
    return violations


def main() -> int:
    args = parse_args()
    source_list = Path(args.source_list)
    stamp = Path(args.stamp)
    check_asm = source_list.stem in ASM_JUMBOS

    if not source_list.is_file():
        print(f"Missing source list: {source_list}")
        return 1

    failures = 0
    for source_file in list_included_sources(source_list):
        if not source_file.is_file():
            print(f"Missing included source file: {source_file}")
            failures += 1
            continue

        for line_number, message in check_file(source_file, check_asm):
            print(f"{source_file}:{line_number}: {message}")
            failures += 1

    if failures > 0:
        return 1

    stamp.parent.mkdir(parents=True, exist_ok=True)
    stamp.write_text("ok\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
