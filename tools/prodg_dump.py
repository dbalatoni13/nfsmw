#!/usr/bin/env python3

"""
Generate and compare ProDG compiler-state dumps for one translation unit.

Examples:
  python tools/prodg_dump.py command -u main/Speed/Indep/SourceLists/zAttribSys
  python tools/prodg_dump.py dump -u main/Speed/Indep/SourceLists/zAttribSys -o /tmp/zattrib_base
  python tools/prodg_dump.py extract /tmp/zattrib_base --stage lreg \
      -f 'VecHashMap<unsigned int,Attrib::Class,Attrib::Class::TablePolicy,false,16>::UpdateSearchLength'
  python tools/prodg_dump.py diff /tmp/zattrib_base /tmp/zattrib_trial \
      -f 'VecHashMap<unsigned int,Attrib::Class,Attrib::Class::TablePolicy,false,16>::UpdateSearchLength'
"""

import argparse
import difflib
import os
import re
import shlex
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Sequence

from _common import (
    ROOT_DIR,
    ToolError,
    ensure_project_prereqs,
    find_objdiff_unit,
    format_subprocess_error,
    load_objdiff_config,
    make_abs,
)


FUNCTION_HEADER_RE = re.compile(r"^;; Function (.+)$")
REGISTER_PREF_RE = re.compile(r"^Register (\d+) used .*; pref (.+)$")
REGISTER_ASSIGN_RE = re.compile(r"^;; Register (\d+) in ([^.]+)\.$")
USER_PSEUDO_RE = re.compile(
    r"\(reg/(?P<flags>[a-z/]+):(?P<mode>[A-Za-z0-9_]+) (?P<num>\d+)(?: r(?P<hard>\d+))?\)"
)
HARD_REG_RE = re.compile(
    r"\(reg(?:/[a-z/]+)?:(?P<mode>[A-Za-z0-9_]+) (?P<num>\d+) r(?P<hard>\d+)\)"
)
PLAIN_REG_RE = re.compile(r"\(reg:(?P<mode>[A-Za-z0-9_]+) (?P<num>\d+)\)")
FRAME_SLOT_PLUS_RE = re.compile(
    r"mem/f:[A-Za-z0-9_]+ \(\s*plus:[A-Za-z0-9_]+ \(\s*reg:SI (?P<base>\d+)\)\s*"
    r"\(\s*const_int (?P<offset>-?\d+) \[[^]]+\]\)\s*\) 0\)",
    re.MULTILINE,
)
FRAME_SLOT_DIRECT_RE = re.compile(r"mem/f:[A-Za-z0-9_]+ \(\s*reg:SI (?P<base>\d+)\) 0\)")
ASM_WEAK_RE = re.compile(r"^\s*\.weak\s+(\S+)$")
ASM_TYPE_RE = re.compile(r"^\s*\.type\s+(\S+),@function$")
ASM_LABEL_RE = re.compile(r"^(\S+):$")
ASM_SIZE_RE = re.compile(r"^\s*\.size\s+(\S+),")
DEFAULT_STAGES = ("rtl", "greg", "lreg")


class DumpToolError(RuntimeError):
    pass


@dataclass
class UnitCompileInfo:
    unit_name: str
    source_path: Path
    target_path: Path
    compile_shell: str
    compile_argv: List[str]
    ngccc_index: int


@dataclass
class FunctionBlock:
    header: str
    start_line: int
    lines: List[str]


@dataclass
class SummaryStats:
    count: int
    first_line: int
    last_line: int


@dataclass
class StageSummary:
    pseudos: Dict[tuple[int, str, str], SummaryStats]
    hard_regs: Dict[int, SummaryStats]
    frame_slots: Dict[tuple[int, int], SummaryStats]
    compares: Dict[tuple[str, str, str], SummaryStats]


def print_section(title: str) -> None:
    print(flush=True)
    print("=" * 60, flush=True)
    print(f"  {title}", flush=True)
    print("=" * 60, flush=True)


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    sys.exit(1)


def run_capture(cmd: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(cmd, cwd=ROOT_DIR, text=True, capture_output=True)
    if result.returncode != 0:
        raise DumpToolError(
            format_subprocess_error(cmd, result.returncode, result.stdout, result.stderr)
        )
    return result


def run_stream(cmd: Sequence[str]) -> None:
    result = subprocess.run(cmd, cwd=ROOT_DIR, text=True)
    if result.returncode != 0:
        raise DumpToolError(format_subprocess_error(cmd, result.returncode))


def get_unit_paths(unit_name: str) -> tuple[Path, Path]:
    ensure_project_prereqs(require_build_ninja=True)
    config = load_objdiff_config()
    unit = find_objdiff_unit(config, unit_name)
    if unit is None:
        raise DumpToolError(f"Unit not found in objdiff.json: {unit_name}")

    metadata = unit.get("metadata") or {}
    source_path = make_abs(metadata.get("source_path"))
    if source_path is None:
        raise DumpToolError(f"Unit has no source_path metadata: {unit_name}")

    target = unit.get("base_path") or unit.get("target_path")
    target_path = make_abs(target)
    if target_path is None:
        raise DumpToolError(f"Unit has no build target in objdiff.json: {unit_name}")

    return Path(source_path), Path(target_path)


def rel_for_ninja(path: Path) -> str:
    try:
        return path.relative_to(ROOT_DIR).as_posix()
    except ValueError:
        return str(path)


def split_shell_pipeline(shell_line: str) -> str:
    if "&&" in shell_line:
        return shell_line.split("&&", 1)[0].strip()
    return shell_line.strip()


def resolve_compile_info(unit_name: str) -> UnitCompileInfo:
    source_path, target_path = get_unit_paths(unit_name)
    target_rel = rel_for_ninja(target_path)
    source_rel = rel_for_ninja(source_path)

    result = run_capture(["ninja", "-t", "commands", target_rel])
    compile_shell = ""
    for raw_line in result.stdout.splitlines():
        line = raw_line.strip()
        if "ngccc.exe" not in line:
            continue
        if source_rel in line or target_rel in line:
            compile_shell = split_shell_pipeline(line)
            break

    if not compile_shell:
        raise DumpToolError(
            "Failed to locate the ProDG compiler command for "
            f"{unit_name} via `ninja -t commands {target_rel}`"
        )

    compile_argv = shlex.split(compile_shell)
    for index, arg in enumerate(compile_argv):
        if arg.endswith("ngccc.exe"):
            return UnitCompileInfo(
                unit_name=unit_name,
                source_path=source_path,
                target_path=target_path,
                compile_shell=compile_shell,
                compile_argv=compile_argv,
                ngccc_index=index,
            )

    raise DumpToolError(f"Compiler line does not contain ngccc.exe:\n{compile_shell}")


def derived_base_name(info: UnitCompileInfo) -> str:
    return info.source_path.stem


def derive_preprocess_command(info: UnitCompileInfo, ii_path: Path) -> List[str]:
    compile_argv = list(info.compile_argv)
    compiler_args = compile_argv[info.ngccc_index + 1 :]
    filtered: List[str] = []

    i = 0
    while i < len(compiler_args):
        arg = compiler_args[i]
        if arg in ("-MMD", "-MD", "-c"):
            i += 1
            continue
        if arg == "-o":
            i += 2
            continue
        filtered.append(arg)
        i += 1

    filtered.extend(["-E", "-o", str(ii_path)])
    return [*compile_argv[: info.ngccc_index + 1], *filtered]


def derive_cc1plus_command(
    info: UnitCompileInfo, ii_path: Path, dumpbase: Path, asm_path: Path
) -> List[str]:
    compile_argv = list(info.compile_argv)
    prefix = compile_argv[: info.ngccc_index]
    ngccc_path = Path(compile_argv[info.ngccc_index])
    cc1plus_path = ngccc_path.with_name("cc1plus.exe")
    if not cc1plus_path.exists():
        raise DumpToolError(f"Missing cc1plus.exe next to ngccc.exe: {cc1plus_path}")

    return [
        *prefix,
        str(cc1plus_path),
        str(ii_path),
        "-da",
        "-dumpbase",
        str(dumpbase),
        "-o",
        str(asm_path),
    ]


def ensure_output_dir(path: Path, force: bool) -> None:
    if path.exists() and not path.is_dir():
        raise DumpToolError(f"Output path is not a directory: {path}")
    if path.exists() and any(path.iterdir()) and not force:
        raise DumpToolError(
            f"Output directory is not empty: {path}\n"
            "Use --force to reuse it."
        )
    path.mkdir(parents=True, exist_ok=True)


def parse_stages(value: str) -> List[str]:
    stages = [stage.strip() for stage in value.split(",") if stage.strip()]
    if not stages:
        raise DumpToolError("Stage list must not be empty")
    for stage in stages:
        if not re.fullmatch(r"[A-Za-z0-9_+-]+", stage):
            raise DumpToolError(f"Invalid stage name: {stage}")
    return stages


def load_function_blocks(path: Path) -> List[FunctionBlock]:
    if path.suffix == ".s":
        return load_assembly_function_blocks(path)

    lines = path.read_text(errors="replace").splitlines()
    blocks: List[FunctionBlock] = []
    current_header: Optional[str] = None
    current_start = 0
    current_lines: List[str] = []

    for index, line in enumerate(lines, start=1):
        match = FUNCTION_HEADER_RE.match(line)
        if match:
            if current_header is not None:
                blocks.append(
                    FunctionBlock(
                        header=current_header,
                        start_line=current_start,
                        lines=current_lines,
                    )
                )
            current_header = match.group(1)
            current_start = index
            current_lines = [line]
            continue

        if current_header is not None:
            current_lines.append(line)

    if current_header is not None:
        blocks.append(
            FunctionBlock(header=current_header, start_line=current_start, lines=current_lines)
        )

    return blocks


def load_assembly_function_blocks(path: Path) -> List[FunctionBlock]:
    lines = path.read_text(errors="replace").splitlines()
    starts: Dict[str, int] = {}
    labels: Dict[str, int] = {}
    blocks: List[FunctionBlock] = []

    for index, line in enumerate(lines, start=1):
        weak_match = ASM_WEAK_RE.match(line)
        if weak_match:
            symbol = weak_match.group(1)
            starts[symbol] = min(starts.get(symbol, index), index)
            continue

        type_match = ASM_TYPE_RE.match(line)
        if type_match:
            symbol = type_match.group(1)
            starts[symbol] = min(starts.get(symbol, index), index)
            continue

        label_match = ASM_LABEL_RE.match(line)
        if label_match:
            symbol = label_match.group(1)
            if symbol in starts and symbol not in labels:
                labels[symbol] = index
            continue

        size_match = ASM_SIZE_RE.match(line)
        if size_match:
            symbol = size_match.group(1)
            if symbol in starts and symbol in labels:
                start_line = starts[symbol]
                blocks.append(
                    FunctionBlock(
                        header=symbol,
                        start_line=start_line,
                        lines=lines[start_line - 1 : index],
                    )
                )
                del starts[symbol]
                del labels[symbol]

    return blocks


def choose_block(blocks: Sequence[FunctionBlock], query: str, exact: bool) -> FunctionBlock:
    if exact:
        matches = [block for block in blocks if block.header == query]
    else:
        matches = [
            block
            for block in blocks
            if block.header == query or block.header.endswith(query) or query in block.header
        ]

    if not matches:
        raise DumpToolError(f"Function not found in dump: {query}")
    if len(matches) > 1:
        options = "\n".join(f"  - {block.header}" for block in matches[:10])
        more = "" if len(matches) <= 10 else f"\n  ... (+{len(matches) - 10} more)"
        raise DumpToolError(
            f"Function query matched multiple dump blocks: {query}\n{options}{more}"
        )
    return matches[0]


def resolve_stage_file(path_value: str, stage: str, base_name: Optional[str]) -> Path:
    path = Path(path_value)
    if path.is_file():
        return path
    if not path.is_dir():
        raise DumpToolError(f"Dump path is neither a file nor a directory: {path}")

    candidates: List[Path] = []
    if base_name:
        candidate = path / f"{base_name}.{stage}"
        if candidate.exists():
            return candidate
    candidates = sorted(path.glob(f"*.{stage}"))
    if not candidates:
        raise DumpToolError(f"No *.{stage} file found under {path}")
    if len(candidates) > 1:
        names = "\n".join(f"  - {candidate.name}" for candidate in candidates[:10])
        more = "" if len(candidates) <= 10 else f"\n  ... (+{len(candidates) - 10} more)"
        raise DumpToolError(
            f"Multiple *.{stage} files found under {path}; pass --base-name to disambiguate.\n"
            f"{names}{more}"
        )
    return candidates[0]


def format_block_lines(
    block: FunctionBlock, line_numbers: bool, grep: Optional[re.Pattern[str]], context: int
) -> List[str]:
    if grep is None:
        if not line_numbers:
            return list(block.lines)
        return [
            f"{block.start_line + index}: {line}"
            for index, line in enumerate(block.lines)
        ]

    marks = [False] * len(block.lines)
    for index, line in enumerate(block.lines):
        if grep.search(line):
            start = max(0, index - context)
            end = min(len(block.lines), index + context + 1)
            for mark_index in range(start, end):
                marks[mark_index] = True

    if not any(marks):
        return []

    output: List[str] = []
    previous_selected = False
    for index, (line, selected) in enumerate(zip(block.lines, marks)):
        if selected:
            prefix = f"{block.start_line + index}: " if line_numbers else ""
            output.append(f"{prefix}{line}")
        elif previous_selected:
            output.append("--")
        previous_selected = selected

    while output and output[-1] == "--":
        output.pop()
    return output


def parse_register_preferences(block: FunctionBlock) -> Dict[int, str]:
    prefs: Dict[int, str] = {}
    for line in block.lines:
        match = REGISTER_PREF_RE.match(line.strip())
        if not match:
            continue
        reg = int(match.group(1))
        prefs[reg] = match.group(2).rstrip(".").strip()
    return prefs


def parse_register_assignments(block: FunctionBlock) -> Dict[int, str]:
    assignments: Dict[int, str] = {}
    for line in block.lines:
        match = REGISTER_ASSIGN_RE.match(line.strip())
        if not match:
            continue
        assignments[int(match.group(1))] = match.group(2).strip()
    return assignments


def iter_block_entries(block: FunctionBlock) -> List[tuple[int, str]]:
    entries: List[tuple[int, str]] = []
    current: List[str] = []
    current_start = block.start_line

    for index, line in enumerate(block.lines):
        if not line.strip():
            if current:
                entries.append((current_start, "\n".join(current)))
                current = []
            continue
        if not current:
            current_start = block.start_line + index
        current.append(line)

    if current:
        entries.append((current_start, "\n".join(current)))
    return entries


def update_summary_stats(mapping: Dict, key, line: int) -> None:
    stats = mapping.get(key)
    if stats is None:
        mapping[key] = SummaryStats(count=1, first_line=line, last_line=line)
        return
    stats.count += 1
    stats.last_line = line


def read_paren_expr(text: str, start_index: int) -> tuple[str, int]:
    index = start_index
    while index < len(text) and text[index].isspace():
        index += 1
    if index >= len(text) or text[index] != "(":
        raise DumpToolError(f"Expected '(' while parsing compare expression: {text[start_index:]}")

    depth = 0
    end = index
    while end < len(text):
        char = text[end]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return text[index : end + 1], end + 1
        end += 1
    raise DumpToolError(f"Unbalanced compare expression: {text[start_index:]}")


def simplify_compare_operand(expr: str) -> str:
    expr = expr.strip()

    hard_match = HARD_REG_RE.fullmatch(expr)
    if hard_match:
        flags_match = USER_PSEUDO_RE.fullmatch(expr)
        if flags_match:
            return (
                f"reg/{flags_match.group('flags')}:{flags_match.group('mode')}"
                f"#{flags_match.group('num')}/r{hard_match.group('hard')}"
            )
        return f"reg:{hard_match.group('mode')}#{hard_match.group('num')}/r{hard_match.group('hard')}"

    pseudo_match = USER_PSEUDO_RE.fullmatch(expr)
    if pseudo_match:
        return (
            f"reg/{pseudo_match.group('flags')}:{pseudo_match.group('mode')}"
            f"#{pseudo_match.group('num')}"
        )

    plain_reg_match = PLAIN_REG_RE.fullmatch(expr)
    if plain_reg_match:
        return f"reg:{plain_reg_match.group('mode')}#{plain_reg_match.group('num')}"

    const_match = re.fullmatch(r"\(const_int (-?\d+) \[[^]]+\]\)", expr)
    if const_match:
        return f"const:{const_match.group(1)}"

    frame_plus_match = FRAME_SLOT_PLUS_RE.fullmatch(expr)
    if frame_plus_match:
        return (
            f"frame:r{frame_plus_match.group('base')}"
            f"{format_offset(int(frame_plus_match.group('offset')))}"
        )

    frame_direct_match = FRAME_SLOT_DIRECT_RE.fullmatch(expr)
    if frame_direct_match:
        return f"frame:r{frame_direct_match.group('base')}+0x0"

    head_match = re.match(r"\(([A-Za-z0-9_/:+-]+)", expr)
    if head_match:
        return head_match.group(1)
    return expr


def parse_compare_signatures(entry_text: str) -> List[tuple[str, str, str]]:
    flat = " ".join(entry_text.split())
    signatures: List[tuple[str, str, str]] = []
    search_from = 0
    while True:
        index = flat.find("compare:", search_from)
        if index < 0:
            return signatures
        kind_start = index + len("compare:")
        kind_end = kind_start
        while kind_end < len(flat) and not flat[kind_end].isspace():
            kind_end += 1
        kind = flat[kind_start:kind_end]
        left_expr, next_index = read_paren_expr(flat, kind_end)
        right_expr, next_index = read_paren_expr(flat, next_index)
        signatures.append(
            (
                kind,
                simplify_compare_operand(left_expr),
                simplify_compare_operand(right_expr),
            )
        )
        search_from = next_index


def summarize_block(block: FunctionBlock) -> StageSummary:
    pseudos: Dict[tuple[int, str, str], SummaryStats] = {}
    hard_regs: Dict[int, SummaryStats] = {}
    frame_slots: Dict[tuple[int, int], SummaryStats] = {}
    compares: Dict[tuple[str, str, str], SummaryStats] = {}

    for start_line, entry_text in iter_block_entries(block):
        for match in USER_PSEUDO_RE.finditer(entry_text):
            key = (
                int(match.group("num")),
                match.group("flags"),
                match.group("mode"),
            )
            update_summary_stats(pseudos, key, start_line)
        for match in HARD_REG_RE.finditer(entry_text):
            update_summary_stats(hard_regs, int(match.group("hard")), start_line)
        for match in FRAME_SLOT_PLUS_RE.finditer(entry_text):
            key = (int(match.group("base")), int(match.group("offset")))
            update_summary_stats(frame_slots, key, start_line)
        for match in FRAME_SLOT_DIRECT_RE.finditer(entry_text):
            key = (int(match.group("base")), 0)
            update_summary_stats(frame_slots, key, start_line)
        for signature in parse_compare_signatures(entry_text):
            update_summary_stats(compares, signature, start_line)

    return StageSummary(
        pseudos=pseudos,
        hard_regs=hard_regs,
        frame_slots=frame_slots,
        compares=compares,
    )


def format_line_range(stats: SummaryStats) -> str:
    if stats.first_line == stats.last_line:
        return str(stats.first_line)
    return f"{stats.first_line}-{stats.last_line}"


def format_offset(offset: int) -> str:
    sign = "-" if offset < 0 else "+"
    return f"{sign}0x{abs(offset):X}"


def print_stage_summary(block: FunctionBlock) -> None:
    summary = summarize_block(block)
    if not summary.pseudos and not summary.hard_regs and not summary.frame_slots and not summary.compares:
        print("No summary data found.", flush=True)
        return

    if summary.pseudos:
        print("User pseudos:", flush=True)
        for reg_num, flags, mode in sorted(summary.pseudos):
            stats = summary.pseudos[(reg_num, flags, mode)]
            print(
                f"  - r{reg_num} ({flags}:{mode}): {stats.count} refs "
                f"[lines {format_line_range(stats)}]",
                flush=True,
            )
    if summary.hard_regs:
        print("Hard registers:", flush=True)
        for reg_num in sorted(summary.hard_regs):
            stats = summary.hard_regs[reg_num]
            print(
                f"  - r{reg_num}: {stats.count} refs [lines {format_line_range(stats)}]",
                flush=True,
            )
    if summary.frame_slots:
        print("Frame slots (mem/f):", flush=True)
        for base, offset in sorted(summary.frame_slots):
            stats = summary.frame_slots[(base, offset)]
            print(
                f"  - base r{base}{format_offset(offset)}: {stats.count} refs "
                f"[lines {format_line_range(stats)}]",
                flush=True,
            )
    if summary.compares:
        print("Compare signatures:", flush=True)
        for kind, left_operand, right_operand in sorted(summary.compares):
            stats = summary.compares[(kind, left_operand, right_operand)]
            print(
                f"  - {kind}: {left_operand} vs {right_operand}: {stats.count} refs "
                f"[lines {format_line_range(stats)}]",
                flush=True,
            )


def print_summary_changes(left: FunctionBlock, right: FunctionBlock) -> None:
    left_summary = summarize_block(left)
    right_summary = summarize_block(right)

    def changed_counts(left_map: Dict, right_map: Dict) -> List[tuple]:
        return sorted(
            (
                key,
                left_map.get(key).count if left_map.get(key) else 0,
                right_map.get(key).count if right_map.get(key) else 0,
            )
            for key in set(left_map) | set(right_map)
            if (left_map.get(key).count if left_map.get(key) else 0)
            != (right_map.get(key).count if right_map.get(key) else 0)
        )

    pseudo_changes = changed_counts(left_summary.pseudos, right_summary.pseudos)
    hard_changes = changed_counts(left_summary.hard_regs, right_summary.hard_regs)
    frame_changes = changed_counts(left_summary.frame_slots, right_summary.frame_slots)
    compare_changes = changed_counts(left_summary.compares, right_summary.compares)

    if not pseudo_changes and not hard_changes and not frame_changes and not compare_changes:
        return

    print("Stage summary changes:", flush=True)
    if pseudo_changes:
        print("  User pseudos:", flush=True)
        for (reg_num, flags, mode), left_count, right_count in pseudo_changes:
            print(
                f"    r{reg_num} ({flags}:{mode}): {left_count} -> {right_count}",
                flush=True,
            )
    if hard_changes:
        print("  Hard registers:", flush=True)
        for reg_num, left_count, right_count in hard_changes:
            print(f"    r{reg_num}: {left_count} -> {right_count}", flush=True)
    if frame_changes:
        print("  Frame slots:", flush=True)
        for (base, offset), left_count, right_count in frame_changes:
            print(
                f"    base r{base}{format_offset(offset)}: {left_count} -> {right_count}",
                flush=True,
            )
    if compare_changes:
        print("  Compare signatures:", flush=True)
        for (kind, left_operand, right_operand), left_count, right_count in compare_changes:
            print(
                f"    {kind}: {left_operand} vs {right_operand}: "
                f"{left_count} -> {right_count}",
                flush=True,
            )
    print(flush=True)


def print_register_change_summary(left: FunctionBlock, right: FunctionBlock) -> None:
    left_prefs = parse_register_preferences(left)
    right_prefs = parse_register_preferences(right)
    left_assign = parse_register_assignments(left)
    right_assign = parse_register_assignments(right)

    pref_changes = sorted(
        reg
        for reg in set(left_prefs) | set(right_prefs)
        if left_prefs.get(reg) != right_prefs.get(reg)
    )
    assign_changes = sorted(
        reg
        for reg in set(left_assign) | set(right_assign)
        if left_assign.get(reg) != right_assign.get(reg)
    )

    if not pref_changes and not assign_changes:
        return

    print("Register summary:", flush=True)
    if pref_changes:
        print("  Preference changes:", flush=True)
        for reg in pref_changes:
            print(
                f"    r{reg}: {left_prefs.get(reg, '<missing>')} -> "
                f"{right_prefs.get(reg, '<missing>')}",
                flush=True,
            )
    if assign_changes:
        print("  Final assignments:", flush=True)
        for reg in assign_changes:
            print(
                f"    r{reg}: {left_assign.get(reg, '<missing>')} -> "
                f"{right_assign.get(reg, '<missing>')}",
                flush=True,
            )
    print(flush=True)


def command_command(args: argparse.Namespace) -> None:
    info = resolve_compile_info(args.unit)
    base_name = args.base_name or derived_base_name(info)
    ii_path = Path(args.ii_path) if args.ii_path else Path(f"{base_name}.ii")
    dumpbase = Path(args.dumpbase) if args.dumpbase else Path(base_name)
    asm_path = Path(args.asm_path) if args.asm_path else Path(f"{base_name}.s")

    preprocess_cmd = derive_preprocess_command(info, ii_path)
    cc1plus_cmd = derive_cc1plus_command(info, ii_path, dumpbase, asm_path)

    print_section(f"ProDG command: {args.unit}")
    print(f"Source: {info.source_path}", flush=True)
    print(f"Target: {info.target_path}", flush=True)
    print(flush=True)
    print("ngccc:", flush=True)
    print(info.compile_shell, flush=True)
    print(flush=True)
    print("preprocess:", flush=True)
    print(shlex.join(preprocess_cmd), flush=True)
    print(flush=True)
    print("cc1plus:", flush=True)
    print(shlex.join(cc1plus_cmd), flush=True)


def command_dump(args: argparse.Namespace) -> None:
    info = resolve_compile_info(args.unit)
    out_dir = Path(args.out_dir).resolve()
    ensure_output_dir(out_dir, force=args.force)

    base_name = args.base_name or derived_base_name(info)
    ii_path = out_dir / f"{base_name}.ii"
    asm_path = out_dir / f"{base_name}.s"
    dumpbase = out_dir / base_name
    log_path = out_dir / f"{base_name}.log"

    preprocess_cmd = derive_preprocess_command(info, ii_path)
    cc1plus_cmd = derive_cc1plus_command(info, ii_path, dumpbase, asm_path)

    print_section(f"Dumping ProDG state: {args.unit}")
    print(f"Output directory: {out_dir}", flush=True)
    if args.print_commands:
        print(flush=True)
        print("preprocess:", flush=True)
        print(shlex.join(preprocess_cmd), flush=True)
        print(flush=True)
        print("cc1plus:", flush=True)
        print(shlex.join(cc1plus_cmd), flush=True)
        print(flush=True)

    run_stream(preprocess_cmd)
    cc1_result = subprocess.run(cc1plus_cmd, cwd=ROOT_DIR, text=True, capture_output=True)
    log_text = ""
    if cc1_result.stdout:
        log_text += cc1_result.stdout
    if cc1_result.stderr:
        if log_text:
            log_text += "\n"
        log_text += cc1_result.stderr
    log_path.write_text(log_text)
    if cc1_result.returncode != 0:
        raise DumpToolError(
            format_subprocess_error(
                cc1plus_cmd, cc1_result.returncode, cc1_result.stdout, cc1_result.stderr
            )
        )

    generated = sorted(path.name for path in out_dir.glob(f"{base_name}.*"))
    print("Generated files:", flush=True)
    for name in generated:
        print(f"  - {name}", flush=True)
    print(f"Log: {log_path}", flush=True)


def command_extract(args: argparse.Namespace) -> None:
    dump_path = resolve_stage_file(args.path, args.stage, args.base_name)
    blocks = load_function_blocks(dump_path)
    block = choose_block(blocks, args.function, exact=args.exact)
    grep = re.compile(args.grep) if args.grep else None
    lines = format_block_lines(
        block,
        line_numbers=args.line_numbers,
        grep=grep,
        context=args.context,
    )

    print_section(f"{dump_path.name}: {block.header}")
    if grep and not lines:
        print(f"No matches for {args.grep!r} inside {block.header}", flush=True)
        return
    print("\n".join(lines), flush=True)


def command_summary(args: argparse.Namespace) -> None:
    dump_path = resolve_stage_file(args.path, args.stage, args.base_name)
    blocks = load_function_blocks(dump_path)
    block = choose_block(blocks, args.function, exact=args.exact)

    print_section(f"{dump_path.name}: {block.header}")
    print_stage_summary(block)


def command_diff(args: argparse.Namespace) -> None:
    stages = parse_stages(args.stages)
    for stage in stages:
        left_path = resolve_stage_file(args.left, stage, args.left_base_name)
        right_path = resolve_stage_file(args.right, stage, args.right_base_name)
        try:
            left_block = choose_block(
                load_function_blocks(left_path), args.function, exact=args.exact
            )
            right_block = choose_block(
                load_function_blocks(right_path), args.function, exact=args.exact
            )
        except DumpToolError as e:
            if args.skip_missing:
                print_section(f"{stage.upper()} diff: skipped")
                print(f"{e}", flush=True)
                print(f"Left:  {left_path}", flush=True)
                print(f"Right: {right_path}", flush=True)
                continue
            raise DumpToolError(
                f"{e}\nStage: {stage}\nLeft: {left_path}\nRight: {right_path}"
            )

        print_section(f"{stage.upper()} diff: {left_block.header}")
        if stage == "lreg":
            print_register_change_summary(left_block, right_block)
        if args.summary or args.summary_only:
            print_summary_changes(left_block, right_block)
        if args.summary_only:
            continue

        diff_lines = list(
            difflib.unified_diff(
                left_block.lines,
                right_block.lines,
                fromfile=str(left_path),
                tofile=str(right_path),
                n=args.context,
                lineterm="",
            )
        )
        if not diff_lines:
            print("No differences.", flush=True)
            continue
        print("\n".join(diff_lines), flush=True)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Generate and compare ProDG compiler-state dumps using the exact "
            "ngccc command recovered from ninja."
        )
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    command = subparsers.add_parser(
        "command",
        help="Show the recovered ngccc command plus derived preprocess/cc1plus invocations",
    )
    command.add_argument("-u", "--unit", required=True, help="objdiff unit name")
    command.add_argument(
        "--base-name",
        help="base filename used when showing derived dump outputs (default: source stem)",
    )
    command.add_argument("--ii-path", help="override the displayed preprocessed .ii path")
    command.add_argument("--asm-path", help="override the displayed assembly output path")
    command.add_argument("--dumpbase", help="override the displayed cc1plus -dumpbase path")
    command.set_defaults(func=command_command)

    dump = subparsers.add_parser(
        "dump",
        help="Run ngccc -E and cc1plus -da for one unit into a dump directory",
    )
    dump.add_argument("-u", "--unit", required=True, help="objdiff unit name")
    dump.add_argument(
        "-o",
        "--out-dir",
        required=True,
        help="directory that will receive the .ii/.s/.rtl/.greg/.lreg dump files",
    )
    dump.add_argument(
        "--base-name",
        help="base filename for generated dumps (default: source stem)",
    )
    dump.add_argument(
        "--force",
        action="store_true",
        help="allow writing into a non-empty output directory",
    )
    dump.add_argument(
        "--print-commands",
        action="store_true",
        help="print the derived preprocess and cc1plus commands before running them",
    )
    dump.set_defaults(func=command_dump)

    extract = subparsers.add_parser(
        "extract",
        help="Extract one function block from a dump file or dump directory",
    )
    extract.add_argument(
        "path",
        help="dump file path or dump directory produced by this tool",
    )
    extract.add_argument(
        "--stage",
        default="lreg",
        help="dump stage when PATH is a directory (default: lreg)",
    )
    extract.add_argument(
        "--base-name",
        help="base filename used to disambiguate PATH when it contains multiple dump sets",
    )
    extract.add_argument(
        "-f",
        "--function",
        required=True,
        help="function header query; exact or substring match",
    )
    extract.add_argument(
        "--exact",
        action="store_true",
        help="require an exact function-header match",
    )
    extract.add_argument(
        "--grep",
        help="only print lines matching this regex, plus --context lines of surrounding dump text",
    )
    extract.add_argument(
        "-C",
        "--context",
        type=int,
        default=2,
        help="context lines to keep around --grep matches (default: 2)",
    )
    extract.add_argument(
        "--line-numbers",
        action="store_true",
        help="prefix output lines with their original dump file line numbers",
    )
    extract.set_defaults(func=command_extract)

    summary = subparsers.add_parser(
        "summary",
        help="Summarize one function's pseudos, hard regs, and frame-slot usage in a dump",
    )
    summary.add_argument(
        "path",
        help="dump file path or dump directory produced by this tool",
    )
    summary.add_argument(
        "--stage",
        default="rtl",
        help="dump stage when PATH is a directory (default: rtl)",
    )
    summary.add_argument(
        "--base-name",
        help="base filename used to disambiguate PATH when it contains multiple dump sets",
    )
    summary.add_argument(
        "-f",
        "--function",
        required=True,
        help="function header query; exact or substring match",
    )
    summary.add_argument(
        "--exact",
        action="store_true",
        help="require an exact function-header match",
    )
    summary.set_defaults(func=command_summary)

    diff = subparsers.add_parser(
        "diff",
        help="Diff one function across two dump files or dump directories",
    )
    diff.add_argument("left", help="left dump file or dump directory")
    diff.add_argument("right", help="right dump file or dump directory")
    diff.add_argument(
        "-f",
        "--function",
        required=True,
        help="function header query; exact or substring match",
    )
    diff.add_argument(
        "--exact",
        action="store_true",
        help="require an exact function-header match",
    )
    diff.add_argument(
        "--stages",
        default=",".join(DEFAULT_STAGES),
        help=f"comma-separated dump stages to diff (default: {','.join(DEFAULT_STAGES)})",
    )
    diff.add_argument(
        "--left-base-name",
        help="base filename used to disambiguate the left dump directory",
    )
    diff.add_argument(
        "--right-base-name",
        help="base filename used to disambiguate the right dump directory",
    )
    diff.add_argument(
        "-C",
        "--context",
        type=int,
        default=3,
        help="unified diff context lines (default: 3)",
    )
    diff.add_argument(
        "--summary",
        action="store_true",
        help="print per-stage pseudo/hard-reg/frame-slot count changes before the diff",
    )
    diff.add_argument(
        "--summary-only",
        action="store_true",
        help="print summary deltas only and skip the full unified diff text",
    )
    diff.add_argument(
        "--skip-missing",
        action="store_true",
        help="skip stages where the requested function is missing instead of failing",
    )
    diff.set_defaults(func=command_diff)

    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    try:
        args.func(args)
    except (DumpToolError, ToolError) as e:
        fail(str(e))


if __name__ == "__main__":
    main()
