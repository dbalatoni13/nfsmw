#!/usr/bin/env python3

import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from typing import Any, Dict, List, Optional, Sequence


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
BUILD_NINJA = os.path.join(ROOT_DIR, "build.ninja")
OBJDIFF_JSON = os.path.join(ROOT_DIR, "objdiff.json")
OBJDIFF_DEFAULT_CONFIG_ARGS = [
    "-c",
    "functionRelocDiffs=none",
    "-c",
    "ppc.calculatePoolRelocations=false",
]
RELOC_DIFF_CHOICES = ("none", "function", "data", "all")


class ToolError(RuntimeError):
    pass


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    sys.exit(1)


def format_subprocess_error(
    cmd: Sequence[str], returncode: int, stdout: str = "", stderr: str = ""
) -> str:
    message = [f"Command failed (exit {returncode}): {' '.join(cmd)}"]
    stdout = stdout.strip()
    stderr = stderr.strip()
    if stdout:
        message.append(f"stdout:\n{stdout}")
    if stderr:
        message.append(f"stderr:\n{stderr}")
    return "\n".join(message)


def ensure_exists(path: str, hint: str) -> None:
    if not os.path.exists(path):
        raise ToolError(f"Missing {path}\nHint: {hint}")


def ensure_project_prereqs(require_build_ninja: bool = False) -> None:
    ensure_exists(OBJDIFF_JSON, "Run: python configure.py")
    if require_build_ninja:
        ensure_exists(BUILD_NINJA, "Run: python configure.py")


def build_objdiff_config_args(reloc_diffs: str = "none") -> List[str]:
    if reloc_diffs not in RELOC_DIFF_CHOICES:
        raise ToolError(
            f"Invalid relocation diff mode: {reloc_diffs} "
            f"(expected one of {', '.join(RELOC_DIFF_CHOICES)})"
        )
    return ["-c", f"functionRelocDiffs={reloc_diffs}", *OBJDIFF_DEFAULT_CONFIG_ARGS]


def load_json_file(path: str, description: str) -> Any:
    try:
        with open(path) as f:
            return json.load(f)
    except FileNotFoundError:
        raise ToolError(f"Missing {description}: {path}")
    except json.JSONDecodeError as e:
        raise ToolError(f"Failed to parse {description}: {e}")


def load_objdiff_config() -> Dict[str, Any]:
    ensure_project_prereqs()
    data = load_json_file(OBJDIFF_JSON, "objdiff.json")
    if not isinstance(data, dict):
        raise ToolError("objdiff.json does not contain a JSON object")
    return data


def find_objdiff_unit(config: Dict[str, Any], unit_name: str) -> Optional[Dict[str, Any]]:
    for unit in config.get("units", []):
        if unit.get("name") == unit_name:
            return unit
    return None


def make_abs(path: Optional[str], base: str = ROOT_DIR) -> Optional[str]:
    if path is None:
        return None
    if os.path.isabs(str(path)):
        return str(path)
    return os.path.abspath(os.path.join(base, str(path)))


def apply_base_obj_override(
    config: Dict[str, Any], unit_name: str, base_obj: str, root_dir: str = ROOT_DIR
) -> bool:
    found = False
    for unit in config.get("units", []):
        target_path = make_abs(unit.get("target_path"), root_dir)
        if target_path is not None:
            unit["target_path"] = target_path

        if unit.get("name") == unit_name:
            unit["base_path"] = os.path.abspath(base_obj)
            found = True
        else:
            base_path = make_abs(unit.get("base_path"), root_dir)
            if base_path is not None:
                unit["base_path"] = base_path

        metadata = unit.get("metadata") or {}
        source_path = make_abs(metadata.get("source_path"), root_dir)
        if source_path is not None:
            metadata["source_path"] = source_path

        scratch = unit.get("scratch") or {}
        ctx_path = make_abs(scratch.get("ctx_path"), root_dir)
        if ctx_path is not None:
            scratch["ctx_path"] = ctx_path

    return found


def classify_objdiff_symbol(sym: Dict[str, Any]) -> str:
    """Classify an objdiff symbol as 'function', 'object', or 'section'."""
    kind = sym.get("kind", "")
    if kind == "SYMBOL_FUNCTION":
        return "function"
    if kind == "SYMBOL_OBJECT":
        return "object"
    if kind == "SYMBOL_SECTION":
        return "section"
    if "instructions" in sym:
        return "function"
    if "data_diff" in sym:
        return "object"
    return "unknown"


def objdiff_symbol_section(sym: Dict[str, Any], sections: List[Dict[str, Any]]) -> str:
    """Determine which section a symbol belongs to."""
    name = sym.get("name", "")
    if name.startswith("[."):
        return name[1:].split("-")[0].rstrip("]")
    if classify_objdiff_symbol(sym) == "function":
        return ".text"
    for sec in sections:
        kind = sec.get("kind", "")
        if kind in ("SECTION_DATA", "SECTION_BSS"):
            return sec["name"]
    return ".data"


def estimate_unmatched_bytes(
    size: int, match_percent: Optional[float], status: str
) -> int:
    """Estimate remaining unmatched bytes for a symbol."""
    size = max(int(size), 0)
    if size == 0:
        return 0
    if status in ("missing", "extra", "no_target", "no_source"):
        return size
    if status in ("match", "matching", "complete"):
        return 0
    if match_percent is None:
        return size

    clamped = max(0.0, min(float(match_percent), 100.0))
    if clamped >= 100.0:
        return 0

    unmatched = int(round(size * (100.0 - clamped) / 100.0))
    unmatched = max(1, unmatched)
    return min(size, unmatched)


def build_objdiff_symbol_rows(diff_data: Dict[str, Any]) -> List[Dict[str, Any]]:
    """Build normalized overview rows from objdiff JSON for both left and right symbols."""
    left_syms = diff_data.get("left", {}).get("symbols", [])
    right_syms = diff_data.get("right", {}).get("symbols", [])
    left_sections = diff_data.get("left", {}).get("sections", [])
    right_sections = diff_data.get("right", {}).get("sections", [])

    rows: List[Dict[str, Any]] = []

    for sym in left_syms:
        sym_type = classify_objdiff_symbol(sym)
        if sym_type in ("section", "unknown"):
            continue

        size = int(sym.get("size", "0"))
        if size == 0:
            continue

        name = sym.get("demangled_name", sym.get("name", "?"))
        section = objdiff_symbol_section(sym, left_sections)
        target_symbol = sym.get("target_symbol")
        match_percent = sym.get("match_percent")

        if target_symbol is None:
            status = "missing"
        elif match_percent is not None and match_percent >= 100.0:
            status = "match"
        elif match_percent is not None:
            status = "nonmatching"
        else:
            status = "missing"

        rows.append(
            {
                "status": status,
                "match_percent": match_percent,
                "size": size,
                "unmatched_bytes_est": estimate_unmatched_bytes(
                    size, match_percent, status
                ),
                "section": section,
                "type": sym_type,
                "name": name,
                "symbol_name": sym.get("name", "?"),
                "side": "left",
                "left_symbol": sym,
                "right_symbol": right_syms[target_symbol]
                if target_symbol is not None and target_symbol < len(right_syms)
                else None,
            }
        )

    for sym in right_syms:
        if sym.get("target_symbol") is not None:
            continue

        sym_type = classify_objdiff_symbol(sym)
        if sym_type in ("section", "unknown"):
            continue

        size = int(sym.get("size", "0"))
        if size == 0:
            continue

        name = sym.get("demangled_name", sym.get("name", "?"))
        section = objdiff_symbol_section(sym, right_sections)
        rows.append(
            {
                "status": "extra",
                "match_percent": None,
                "size": size,
                "unmatched_bytes_est": estimate_unmatched_bytes(size, None, "extra"),
                "section": section,
                "type": sym_type,
                "name": name,
                "symbol_name": sym.get("name", "?"),
                "side": "right",
                "left_symbol": None,
                "right_symbol": sym,
            }
        )

    return rows


def run_objdiff_json(
    objdiff_cli: str,
    unit_name: str,
    *,
    base_obj: Optional[str] = None,
    extra_args: Optional[Sequence[str]] = None,
    reloc_diffs: str = "none",
    root_dir: str = ROOT_DIR,
) -> Dict[str, Any]:
    ensure_project_prereqs()

    cmd = [objdiff_cli, "diff"]
    cmd.extend(OBJDIFF_DEFAULT_CONFIG_ARGS)
    if extra_args:
        cmd.extend(extra_args)
    cmd.extend(["-u", unit_name, "-o", "-", "--format", "json"])

    cwd = root_dir
    tmpdir = None
    if base_obj is not None:
        config = load_objdiff_config()
        if not apply_base_obj_override(config, unit_name, base_obj, root_dir=root_dir):
            raise ToolError(f"Unit not found in objdiff.json: {unit_name}")

        tmpdir = tempfile.mkdtemp(prefix="nfsmw_objdiff_")
        tmp_config = os.path.join(tmpdir, "objdiff.json")
        with open(tmp_config, "w") as f:
            json.dump(config, f)
        cwd = tmpdir

    try:
        try:
            result = subprocess.run(
                cmd,
                cwd=cwd,
                text=True,
                capture_output=True,
            )
        except FileNotFoundError:
            raise ToolError(
                f"Missing objdiff-cli: {objdiff_cli}\n"
                "Hint: ensure build/tools is populated in this worktree "
                "(for example via the shared worktree assets setup)."
            )
        if result.returncode != 0:
            stderr = result.stderr
            hint_lines = []
            missing_path = None

            if "No such file or directory" in stderr:
                match = re.search(r"Failed:\s+Loading\s+(.+)", stderr)
                if match:
                    missing_path = match.group(1).strip()

            if missing_path is not None:
                if base_obj is not None:
                    hint_lines.extend(
                        [
                            f"Hint: the requested base object is missing: {missing_path}",
                            "Rebuild that object or point --base-obj at an existing file.",
                        ]
                    )
                else:
                    hint_lines.extend(
                        [
                            f"Hint: the shared build output for {unit_name} is missing: {missing_path}",
                            "Fastest fixes:",
                            f"  python tools/decomp-workflow.py build -u {unit_name}",
                            "Wrapper flows for inspection after rebuilding:",
                            f"  python tools/decomp-workflow.py unit -u {unit_name}",
                            f"  python tools/decomp-workflow.py diff -u {unit_name} ...",
                            "Or rebuild shared outputs with: ninja all_source",
                        ]
                    )

            message = format_subprocess_error(
                cmd, result.returncode, result.stdout, result.stderr
            )
            if hint_lines:
                message += "\n" + "\n".join(hint_lines)
            raise ToolError(
                message
            )
        try:
            return json.loads(result.stdout)
        except json.JSONDecodeError as e:
            raise ToolError(f"objdiff-cli returned invalid JSON: {e}")
    finally:
        if tmpdir is not None:
            shutil.rmtree(tmpdir, ignore_errors=True)
