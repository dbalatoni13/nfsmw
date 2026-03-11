#!/usr/bin/env python3

import json
import os
import shutil
import subprocess
import sys
import tempfile
from typing import Any, Dict, Optional, Sequence


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
BUILD_NINJA = os.path.join(ROOT_DIR, "build.ninja")
OBJDIFF_JSON = os.path.join(ROOT_DIR, "objdiff.json")


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


def run_objdiff_json(
    objdiff_cli: str,
    unit_name: str,
    *,
    base_obj: Optional[str] = None,
    extra_args: Optional[Sequence[str]] = None,
    root_dir: str = ROOT_DIR,
) -> Dict[str, Any]:
    ensure_project_prereqs()

    cmd = [objdiff_cli, "diff"]
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
        result = subprocess.run(
            cmd,
            cwd=cwd,
            text=True,
            capture_output=True,
        )
        if result.returncode != 0:
            raise ToolError(
                format_subprocess_error(cmd, result.returncode, result.stdout, result.stderr)
            )
        try:
            return json.loads(result.stdout)
        except json.JSONDecodeError as e:
            raise ToolError(f"objdiff-cli returned invalid JSON: {e}")
    finally:
        if tmpdir is not None:
            shutil.rmtree(tmpdir, ignore_errors=True)
