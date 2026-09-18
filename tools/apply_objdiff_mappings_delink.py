#!/usr/bin/env python3
"""Apply objdiff target->base mappings to a delink model JSON."""

from __future__ import annotations

import json
from pathlib import Path
import sys


SKIP_TARGET = ("TODO")
SKIP_BASE = ("__save", "__rest")


def apply(game_version: str) -> tuple[int, int]:
    config_path = Path(f"config/{game_version}/symbols.json")
    model = json.loads(config_path.read_text(encoding="utf-8"))
    objdiff = json.loads(Path("objdiff.json").read_text(encoding="utf-8"))

    mappings: dict[str, str] = {}
    for unit in objdiff.get("units", []):
        mappings.update(unit.pop("symbol_mappings", {}) or {})

    valid: dict[str, str] = {
        target: base
        for target, base in mappings.items()
        if not target.startswith(SKIP_TARGET)
        and not any(marker in base for marker in SKIP_BASE)
    }

    changed = 0
    for entry in model.get("functions", []):
        if isinstance(entry, dict):
            replacement = valid.get(entry.get("name"))
            if replacement is not None:
                entry["name"] = replacement
                changed += 1

    for entry in model.get("names", []):
        if isinstance(entry, dict):
            replacement = valid.get(entry.get("name"))
            if replacement is not None:
                entry["name"] = replacement
                changed += 1

    config_path.write_text(json.dumps(model, indent=2) + "\n", encoding="utf-8")
    Path("objdiff.json").write_text(json.dumps(objdiff, indent=2) + "\n", encoding="utf-8")
    return len(valid), changed


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Expected usage: {sys.argv[0]} <game version>")
        sys.exit(1)
    mappings, changed = apply(sys.argv[1])
    print(f"Applied {mappings} mappings ({changed} delink entries changed)")
