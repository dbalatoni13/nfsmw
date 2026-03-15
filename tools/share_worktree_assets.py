#!/usr/bin/env python3

"""
Share stable debug/tool assets across git worktrees.

This keeps branch-specific generated files (`build.ninja`, `objdiff.json`,
`compile_commands.json`, object files, etc.) local to each worktree while
deduplicating large immutable assets such as extracted game files, symbol dumps,
and downloaded tool binaries under the git common directory.

Examples:
  python tools/share_worktree_assets.py status
  python tools/share_worktree_assets.py status --all
  python tools/share_worktree_assets.py link --all
"""

import argparse
import filecmp
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Set

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))

SHARED_ROOT_NAME = "worktree-shared"


@dataclass(frozen=True)
class AssetSpec:
    relpath: str
    kind: str


FIXED_ASSETS = (
    AssetSpec("NFSMWRELEASE.ELF", "file"),
    AssetSpec("NFS.ELF", "file"),
    AssetSpec("NFS.MAP", "file"),
    AssetSpec(os.path.join("build", "tools"), "dir"),
    AssetSpec(os.path.join("build", "compilers"), "dir"),
    AssetSpec(os.path.join("build", "ppc_binutils"), "dir"),
)


def run_git(args: List[str], cwd: str) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=cwd,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(result.stderr.strip(), file=sys.stderr)
        sys.exit(result.returncode)
    return result.stdout


def git_common_dir(cwd: str) -> str:
    common = run_git(["rev-parse", "--git-common-dir"], cwd).strip()
    if os.path.isabs(common):
        return common
    return os.path.abspath(os.path.join(cwd, common))


def list_worktrees(cwd: str) -> List[str]:
    output = run_git(["worktree", "list", "--porcelain"], cwd)
    worktrees = []
    for line in output.splitlines():
        if line.startswith("worktree "):
            worktrees.append(line.split(" ", 1)[1])
    return worktrees


def tracked_paths(cwd: str) -> Set[str]:
    output = run_git(["ls-files"], cwd)
    return {line.strip() for line in output.splitlines() if line.strip()}


def lexists(path: str) -> bool:
    return os.path.lexists(path)


def same_symlink(path: str, target: str) -> bool:
    return os.path.islink(path) and os.path.realpath(path) == os.path.realpath(target)


def remove_path(path: str) -> None:
    if os.path.islink(path) or os.path.isfile(path):
        os.unlink(path)
    elif os.path.isdir(path):
        shutil.rmtree(path)


def ensure_parent(path: str) -> None:
    parent = os.path.dirname(path)
    if parent:
        os.makedirs(parent, exist_ok=True)


def merge_file(src: str, dst: str, relpath: str) -> None:
    ensure_parent(dst)
    if not os.path.exists(dst):
        shutil.copy2(src, dst)
        return
    if not filecmp.cmp(src, dst, shallow=False):
        raise RuntimeError(f"Conflicting file contents for {relpath}")


def merge_symlink(src: str, dst: str, relpath: str) -> None:
    resolved = os.path.realpath(src)
    if os.path.isdir(resolved):
        merge_tree(resolved, dst, relpath)
    elif os.path.isfile(resolved):
        merge_file(resolved, dst, relpath)
    else:
        raise RuntimeError(f"Broken symlink encountered while merging {relpath}: {src}")


def merge_tree(src: str, dst: str, relpath: str) -> None:
    for current_root, dirnames, filenames in os.walk(src):
        dirnames.sort()
        filenames.sort()
        rel_dir = os.path.relpath(current_root, src)
        target_root = dst if rel_dir == "." else os.path.join(dst, rel_dir)
        os.makedirs(target_root, exist_ok=True)

        next_dirnames = []
        for dirname in dirnames:
            src_dir = os.path.join(current_root, dirname)
            if os.path.islink(src_dir):
                dst_dir = os.path.join(target_root, dirname)
                rel_entry = os.path.join(relpath, os.path.relpath(src_dir, src))
                merge_symlink(src_dir, dst_dir, rel_entry)
                continue
            next_dirnames.append(dirname)
        dirnames[:] = next_dirnames

        for filename in filenames:
            src_file = os.path.join(current_root, filename)
            if os.path.islink(src_file):
                dst_file = os.path.join(target_root, filename)
                rel_entry = os.path.join(relpath, os.path.relpath(src_file, src))
                merge_symlink(src_file, dst_file, rel_entry)
                continue
            dst_file = os.path.join(target_root, filename)
            rel_file = os.path.join(relpath, os.path.relpath(src_file, src))
            merge_file(src_file, dst_file, rel_file)


def is_tracked_path(relpath: str, tracked: Set[str]) -> bool:
    prefix = relpath + os.sep
    return any(path == relpath or path.startswith(prefix) for path in tracked)


def discover_child_assets(
    worktrees: Iterable[str],
    parent_relpath: str,
    skip_names: Iterable[str],
    tracked: Set[str],
) -> Dict[str, AssetSpec]:
    specs: Dict[str, AssetSpec] = {}
    skip = set(skip_names)
    for worktree in worktrees:
        parent = os.path.join(worktree, parent_relpath)
        if not os.path.isdir(parent):
            continue
        for dirpath, dirnames, filenames in os.walk(parent):
            dirnames.sort()
            filenames.sort()
            rel_dir = os.path.relpath(dirpath, parent)
            if rel_dir == ".":
                children = list(dirnames) + list(filenames)
                for name in children:
                    if name in skip:
                        continue
                    child = os.path.join(dirpath, name)
                    relpath = os.path.join(parent_relpath, name)
                    if is_tracked_path(relpath, tracked):
                        continue
                    kind = "dir" if os.path.isdir(child) else "file"
                    specs[relpath] = AssetSpec(relpath, kind)
                dirnames[:] = []
            else:
                dirnames[:] = []
    return specs


def discover_assets(worktrees: Iterable[str], shared_root: str) -> List[AssetSpec]:
    tracked: Set[str] = set()
    for worktree in worktrees:
        tracked.update(tracked_paths(worktree))

    specs: Dict[str, AssetSpec] = {}
    for spec in FIXED_ASSETS:
        if not is_tracked_path(spec.relpath, tracked):
            specs[spec.relpath] = spec
    for source_root in list(worktrees) + [shared_root]:
        if os.path.isdir(os.path.join(source_root, "symbols")):
            specs.update(
                discover_child_assets(
                    [source_root], "symbols", skip_names=(), tracked=tracked
                ).items()
            )
        if os.path.isdir(os.path.join(source_root, "orig")):
            for worktree in [source_root]:
                orig_root = os.path.join(worktree, "orig")
                if not os.path.isdir(orig_root):
                    continue
                for version in sorted(os.listdir(orig_root)):
                    version_path = os.path.join(orig_root, version)
                    if not os.path.isdir(version_path):
                        continue
                    child_specs = discover_child_assets(
                        [worktree],
                        os.path.join("orig", version),
                        skip_names=(".gitkeep",),
                        tracked=tracked,
                    )
                    specs.update(child_specs.items())
    return [specs[key] for key in sorted(specs)]


def asset_status(path: str, shared_path: str) -> str:
    if same_symlink(path, shared_path):
        return "shared"
    if os.path.islink(path):
        return "other-symlink"
    if os.path.isdir(path):
        return "local-dir"
    if os.path.isfile(path):
        return "local-file"
    return "missing"


def ensure_shared_asset(spec: AssetSpec, worktrees: Iterable[str], shared_root: str) -> Optional[str]:
    shared_path = os.path.join(shared_root, spec.relpath)
    if spec.kind == "dir":
        os.makedirs(shared_path, exist_ok=True)
        found = False
        for worktree in worktrees:
            local_path = os.path.join(worktree, spec.relpath)
            if same_symlink(local_path, shared_path) or not os.path.isdir(local_path):
                continue
            merge_tree(local_path, shared_path, spec.relpath)
            found = True
        if found or os.listdir(shared_path):
            return shared_path
        return None

    found = False
    for worktree in worktrees:
        local_path = os.path.join(worktree, spec.relpath)
        if same_symlink(local_path, shared_path) or not os.path.isfile(local_path):
            continue
        merge_file(local_path, shared_path, spec.relpath)
        found = True
    if found or os.path.isfile(shared_path):
        return shared_path
    return None


def link_asset(worktree: str, spec: AssetSpec, shared_path: str) -> str:
    local_path = os.path.join(worktree, spec.relpath)
    if same_symlink(local_path, shared_path):
        return "already-shared"

    if spec.kind == "dir":
        if os.path.isdir(local_path):
            merge_tree(local_path, shared_path, spec.relpath)
            remove_path(local_path)
        elif os.path.isfile(local_path):
            raise RuntimeError(f"{spec.relpath}: expected directory in {worktree}")
        elif os.path.islink(local_path):
            remove_path(local_path)
    else:
        if os.path.isfile(local_path):
            merge_file(local_path, shared_path, spec.relpath)
            remove_path(local_path)
        elif os.path.isdir(local_path):
            raise RuntimeError(f"{spec.relpath}: expected file in {worktree}")
        elif os.path.islink(local_path):
            remove_path(local_path)

    ensure_parent(local_path)
    os.symlink(shared_path, local_path)
    return "linked"


def print_status(worktrees: List[str], shared_root: str) -> int:
    assets = discover_assets(worktrees, shared_root)
    print(f"Shared asset root: {shared_root}")
    for worktree in worktrees:
        print(f"\n[{worktree}]")
        for spec in assets:
            shared_path = os.path.join(shared_root, spec.relpath)
            shared_state = "seeded" if lexists(shared_path) else "unseeded"
            local_state = asset_status(os.path.join(worktree, spec.relpath), shared_path)
            print(f"  {spec.relpath:<40} {local_state:<12} {shared_state}")
    return 0


def link_assets(worktrees: List[str], shared_root: str) -> int:
    os.makedirs(shared_root, exist_ok=True)
    assets = discover_assets(worktrees, shared_root)
    for spec in assets:
        shared_path = ensure_shared_asset(spec, worktrees, shared_root)
        if shared_path is None:
            continue
        for worktree in worktrees:
            status = link_asset(worktree, spec, shared_path)
            print(f"{worktree}: {spec.relpath} -> {status}")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Share stable debug/tool assets across git worktrees while keeping "
            "generated build outputs local to each worktree."
        )
    )
    parser.add_argument(
        "command",
        choices=("status", "link"),
        help="Inspect or create shared asset symlinks.",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Operate on all worktrees for this repository (default: current worktree only).",
    )
    args = parser.parse_args()

    common_dir = git_common_dir(root_dir)
    shared_root = os.path.join(common_dir, SHARED_ROOT_NAME)
    worktrees = list_worktrees(root_dir) if args.all else [root_dir]

    try:
        if args.command == "status":
            return print_status(worktrees, shared_root)
        return link_assets(worktrees, shared_root)
    except RuntimeError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
