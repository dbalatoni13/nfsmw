#!/usr/bin/env python3

"""
Safely merge a base branch into recent local branches and open-PR branches.

Examples:
  python tools/merge-main-rollout.py --base main --recent-hours 5 --pr-repo dbalatoni13/nfsmw --dry-run
  python tools/merge-main-rollout.py --base main --recent-hours 5 --pr-repo dbalatoni13/nfsmw
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Sequence, Set, Tuple

from _common import ROOT_DIR, format_subprocess_error


TRAILER = "Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"


class RolloutError(RuntimeError):
    pass


@dataclass
class TargetBranch:
    name: str
    reasons: Set[str] = field(default_factory=set)


@dataclass
class MergeResult:
    branch: str
    status: str
    detail: str


def run_capture(cmd: Sequence[str], cwd: str = ROOT_DIR) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        cmd,
        cwd=cwd,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        raise RolloutError(format_subprocess_error(cmd, result.returncode, result.stdout, result.stderr))
    return result


def run_stream(cmd: Sequence[str], cwd: str = ROOT_DIR) -> None:
    result = subprocess.run(cmd, cwd=cwd, text=True)
    if result.returncode != 0:
        raise RolloutError(format_subprocess_error(cmd, result.returncode))


def git_common_dir() -> str:
    return run_capture(["git", "rev-parse", "--git-common-dir"]).stdout.strip()


def get_worktrees() -> Dict[str, str]:
    result = run_capture(["git", "worktree", "list", "--porcelain"])
    worktrees: Dict[str, str] = {}
    current: Dict[str, str] = {}
    for line in result.stdout.splitlines():
        if not line:
            branch = current.get("branch", "")
            worktree = current.get("worktree")
            if branch.startswith("refs/heads/") and worktree:
                worktrees[branch.replace("refs/heads/", "", 1)] = worktree
            current = {}
            continue
        key, _, value = line.partition(" ")
        current[key] = value
    if current:
        branch = current.get("branch", "")
        worktree = current.get("worktree")
        if branch.startswith("refs/heads/") and worktree:
            worktrees[branch.replace("refs/heads/", "", 1)] = worktree
    return worktrees


def worktree_dirty(path: str) -> bool:
    result = run_capture(["git", "status", "--short"], cwd=path)
    return bool(result.stdout.strip())


def get_recent_local_branches(hours: float) -> List[str]:
    threshold = int(time.time() - hours * 3600.0)
    result = run_capture(
        [
            "git",
            "for-each-ref",
            "--format=%(refname:short)\t%(committerdate:unix)",
            "refs/heads",
        ]
    )
    recent: List[str] = []
    for line in result.stdout.splitlines():
        if not line.strip():
            continue
        name, _, ts = line.partition("\t")
        if not name or not ts:
            continue
        if int(ts) >= threshold:
            recent.append(name)
    return recent


def get_open_pr_branches(repo: str) -> List[Tuple[str, str]]:
    result = run_capture(
        [
            "gh",
            "pr",
            "list",
            "--repo",
            repo,
            "--state",
            "open",
            "--limit",
            "100",
            "--json",
            "headRefName,headRepositoryOwner",
        ]
    )
    data = json.loads(result.stdout)
    refs: List[Tuple[str, str]] = []
    for entry in data:
        branch = entry.get("headRefName")
        owner = ((entry.get("headRepositoryOwner") or {}).get("login") or "").strip()
        if branch:
            refs.append((branch, owner))
    return refs


def local_branch_exists(branch: str) -> bool:
    result = subprocess.run(
        ["git", "show-ref", "--verify", "--quiet", f"refs/heads/{branch}"],
        cwd=ROOT_DIR,
        text=True,
    )
    return result.returncode == 0


def discover_targets(base: str, recent_hours: float, pr_repos: Sequence[str]) -> Dict[str, TargetBranch]:
    targets: Dict[str, TargetBranch] = {}
    for branch in get_recent_local_branches(recent_hours):
        if branch == base:
            continue
        target = targets.setdefault(branch, TargetBranch(branch))
        target.reasons.add(f"recent<={recent_hours:g}h")

    for repo in pr_repos:
        for branch, owner in get_open_pr_branches(repo):
            if branch == base:
                continue
            target = targets.setdefault(branch, TargetBranch(branch))
            if owner:
                target.reasons.add(f"open-pr:{repo}:{owner}")
            else:
                target.reasons.add(f"open-pr:{repo}")
    return targets


def ensure_temp_worktree(branch: str, temp_root: str) -> str:
    os.makedirs(temp_root, exist_ok=True)
    path = tempfile.mkdtemp(prefix=f"{branch.replace('/', '_')}_", dir=temp_root)
    try:
        run_capture(["git", "worktree", "add", path, branch])
    except Exception:
        shutil.rmtree(path, ignore_errors=True)
        raise
    return path


def remove_temp_worktree(path: str) -> None:
    result = subprocess.run(
        ["git", "worktree", "remove", "--force", path],
        cwd=ROOT_DIR,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        shutil.rmtree(path, ignore_errors=True)


def merge_into_branch(branch: str, base: str, path: str) -> MergeResult:
    before = run_capture(["git", "rev-parse", "HEAD"], cwd=path).stdout.strip()
    merge_message = f"Merge {base} into {branch}\n\n{TRAILER}"
    result = subprocess.run(
        ["git", "merge", "--no-ff", base, "-m", merge_message],
        cwd=path,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        subprocess.run(["git", "merge", "--abort"], cwd=path, text=True, capture_output=True)
        return MergeResult(
            branch=branch,
            status="conflict",
            detail=(result.stderr.strip() or result.stdout.strip() or "merge failed"),
        )

    after = run_capture(["git", "rev-parse", "HEAD"], cwd=path).stdout.strip()
    if before == after:
        return MergeResult(branch=branch, status="up-to-date", detail="already contained base")
    return MergeResult(branch=branch, status="merged", detail=after)


def rollout(
    base: str,
    recent_hours: float,
    pr_repos: Sequence[str],
    dry_run: bool,
) -> List[MergeResult]:
    targets = discover_targets(base, recent_hours, pr_repos)
    worktrees = get_worktrees()
    temp_root = os.path.join(git_common_dir(), "merge-rollout-worktrees")
    results: List[MergeResult] = []

    for branch in sorted(targets):
        if branch == base:
            continue

        target = targets[branch]
        reason_text = ", ".join(sorted(target.reasons))

        if not local_branch_exists(branch):
            results.append(
                MergeResult(branch=branch, status="skipped", detail=f"no local branch ({reason_text})")
            )
            continue

        existing_path = worktrees.get(branch)
        temp_path: Optional[str] = None
        path = existing_path
        try:
            if existing_path:
                if worktree_dirty(existing_path):
                    results.append(
                        MergeResult(
                            branch=branch,
                            status="skipped",
                            detail=f"dirty worktree: {existing_path} ({reason_text})",
                        )
                    )
                    continue
            else:
                temp_path = ensure_temp_worktree(branch, temp_root)
                path = temp_path

            assert path is not None
            if dry_run:
                location = existing_path if existing_path else temp_path
                results.append(
                    MergeResult(
                        branch=branch,
                        status="would-merge",
                        detail=f"{location} ({reason_text})",
                    )
                )
                continue

            results.append(merge_into_branch(branch, base, path))
        finally:
            if temp_path is not None:
                remove_temp_worktree(temp_path)

    return results


def print_results(results: Sequence[MergeResult]) -> None:
    print(f"{'BRANCH':<40} {'STATUS':<12} DETAIL")
    print("-" * 120)
    for result in results:
        print(f"{result.branch:<40} {result.status:<12} {result.detail}")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Safely merge a base branch into recent local branches and local open-PR branches.",
    )
    parser.add_argument("--base", default="main", help="Base branch to merge from (default: main)")
    parser.add_argument(
        "--recent-hours",
        type=float,
        default=5.0,
        help="Include local branches with commits in the last N hours (default: 5)",
    )
    parser.add_argument(
        "--pr-repo",
        action="append",
        default=[],
        help="GitHub repo in OWNER/REPO form; include open PR head branches if they exist locally",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Discover and print targets without merging",
    )
    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    try:
        results = rollout(
            base=args.base,
            recent_hours=args.recent_hours,
            pr_repos=args.pr_repo,
            dry_run=args.dry_run,
        )
        print_results(results)
    except RolloutError as e:
        print(e, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
