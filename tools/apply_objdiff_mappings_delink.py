#!/usr/bin/env python3
"""Apply objdiff target->base mappings to a delink model JSON."""

from __future__ import annotations

import json
from pathlib import Path
import sys


SKIP_TARGET = ("TODO")
SKIP_BASE = ("__save", "__rest", "$SG")


def _compact_json_dumps(value) -> str:
    """Keep record objects on one line while wrapping large address lists."""
    def inline(item) -> str:
        return json.dumps(item, ensure_ascii=False, separators=(", ", ": "))

    def is_scalar(item) -> bool:
        return not isinstance(item, (dict, list))

    def is_inline_object(item) -> bool:
        if not isinstance(item, dict):
            return False
        if all(is_scalar(child) for child in item.values()):
            return True
        if all(
            is_scalar(child)
            or (isinstance(child, list) and all(is_scalar(value) for value in child))
            for child in item.values()
        ):
            return len(inline(item)) <= 180
        return False

    def render(item, depth: int) -> str:
        pad = "  " * depth
        child_pad = "  " * (depth + 1)
        if is_scalar(item):
            return json.dumps(item, ensure_ascii=False)
        if isinstance(item, dict):
            if is_inline_object(item):
                return inline(item)
            if not item:
                return "{}"
            rows = [
                child_pad
                + json.dumps(key, ensure_ascii=False)
                + ": "
                + render(child, depth + 1)
                for key, child in item.items()
            ]
            return "{\n" + ",\n".join(rows) + "\n" + pad + "}"
        if not item:
            return "[]"
        if all(is_scalar(child) for child in item):
            tokens = [json.dumps(child, ensure_ascii=False) for child in item]
            if len(inline(item)) + len(pad) <= 110:
                return "[" + ", ".join(tokens) + "]"
            rows, row, row_len = [], [], 0
            for token in tokens:
                extra = len(token) + (2 if row else 0)
                if row and len(child_pad) + row_len + extra > 110:
                    rows.append(child_pad + ", ".join(row))
                    row, row_len = [], 0
                    extra = len(token)
                row.append(token)
                row_len += extra
            if row:
                rows.append(child_pad + ", ".join(row))
            return "[\n" + ",\n".join(rows) + "\n" + pad + "]"
        if all(
            isinstance(child, list) and all(is_scalar(value) for value in child)
            for child in item
        ) and len(inline(item)) + len(pad) <= 110:
            return inline(item)
        rows = [child_pad + render(child, depth + 1) for child in item]
        return "[\n" + ",\n".join(rows) + "\n" + pad + "]"

    return render(value, 0)


def _hex_address(value):
    if value is None:
        return None
    if isinstance(value, str):
        value = int(value, 0)
    return "0x%X" % int(value)


def _hexify_model_addresses(model: dict) -> None:
    for key in ("image_base", "min_ea", "max_ea"):
        model["meta"][key] = _hex_address(model["meta"][key])
    for segment in model.get("segments", []):
        for key in ("start", "end"):
            segment[key] = _hex_address(segment[key])
    for function in model.get("functions", []):
        for key in ("start", "end", "thunk_target"):
            if key in function:
                function[key] = _hex_address(function[key])
    for name in model.get("names", []):
        name["addr"] = _hex_address(name["addr"])
    for relocation in model.get("relocations", []):
        for key in ("addr", "target"):
            relocation[key] = _hex_address(relocation[key])
    for table in model.get("jump_tables", []):
        for key in ("owner", "dispatch", "dispatch_addr", "start"):
            table[key] = _hex_address(table[key])
        for entry in table.get("entries", []):
            for key in ("addr", "target"):
                entry[key] = _hex_address(entry[key])


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

    _hexify_model_addresses(model)
    config_path.write_text(_compact_json_dumps(model) + "\n", encoding="utf-8")
    Path("objdiff.json").write_text(json.dumps(objdiff, indent=2) + "\n", encoding="utf-8")
    return len(valid), changed


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Expected usage: {sys.argv[0]} <game version>")
        sys.exit(1)
    mappings, changed = apply(sys.argv[1])
    print(f"Applied {mappings} mappings ({changed} delink entries changed)")
