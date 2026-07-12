"""Shared normalized line comparison helpers for repository comparison tools."""

import difflib
from typing import Dict, List, Sequence, Tuple


def count_lines_for_opcodes(
    opcodes: Sequence[Tuple[str, int, int, int, int]],
) -> Dict[str, int]:
    matching = 0
    original_only = 0
    rebuilt_only = 0
    changed_groups = 0
    for tag, i1, i2, j1, j2 in opcodes:
        if tag == "equal":
            matching += i2 - i1
            continue
        changed_groups += 1
        if tag in ("replace", "delete"):
            original_only += i2 - i1
        if tag in ("replace", "insert"):
            rebuilt_only += j2 - j1
    return {
        "matching_lines": matching,
        "original_only_lines": original_only,
        "rebuilt_only_lines": rebuilt_only,
        "changed_groups": changed_groups,
    }


def build_diff_lines(
    original_lines: Sequence[str],
    rebuilt_lines: Sequence[str],
    item_name: str,
    context: int,
    collapse: bool,
) -> List[str]:
    if list(original_lines) == list(rebuilt_lines):
        return []

    rendered: List[str] = [
        f"--- original:{item_name}",
        f"+++ rebuilt:{item_name}",
    ]
    matcher = difflib.SequenceMatcher(a=original_lines, b=rebuilt_lines)
    for group in matcher.get_grouped_opcodes(
        context if collapse else max(len(original_lines), len(rebuilt_lines))
    ):
        first = group[0]
        last = group[-1]
        a_start = first[1] + 1
        a_len = last[2] - first[1]
        b_start = first[3] + 1
        b_len = last[4] - first[3]
        rendered.append(f"@@ -{a_start},{a_len} +{b_start},{b_len} @@")

        for tag, i1, i2, j1, j2 in group:
            if tag == "equal":
                for idx in range(i1, i2):
                    rendered.append(f"  L{idx + 1:04d} {original_lines[idx]}")
                continue
            if tag in ("replace", "delete"):
                for idx in range(i1, i2):
                    rendered.append(f"- L{idx + 1:04d} {original_lines[idx]}")
            if tag in ("replace", "insert"):
                for idx in range(j1, j2):
                    rendered.append(f"+ L{idx + 1:04d} {rebuilt_lines[idx]}")
    return rendered
