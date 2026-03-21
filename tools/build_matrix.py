#!/usr/bin/env python3

"""
Run sequential build checks across supported platforms.

Examples:
  python tools/build_matrix.py
  python tools/build_matrix.py --version GOWE69 --version SLES-53558-A124
  python tools/build_matrix.py --all-source
"""

import argparse
import os
import subprocess
import sys
import time
from dataclasses import dataclass
from typing import List, Optional, Sequence


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
DEFAULT_RESTORE_VERSION = "GOWE69"


@dataclass(frozen=True)
class PlatformCheck:
    version: str
    label: str
    required_assets: Sequence[str]


@dataclass
class StepResult:
    name: str
    command: List[str]
    returncode: int
    elapsed: float
    log_path: str
    output: str

    @property
    def ok(self) -> bool:
        return self.returncode == 0


@dataclass
class PlatformResult:
    platform: PlatformCheck
    configure: Optional[StepResult] = None
    build: Optional[StepResult] = None
    preflight_error: Optional[str] = None

    @property
    def ok(self) -> bool:
        return (
            self.preflight_error is None
            and self.configure is not None
            and self.configure.ok
            and self.build is not None
            and self.build.ok
        )


PLATFORMS = (
    PlatformCheck(
        version="GOWE69",
        label="GameCube",
        required_assets=("orig/GOWE69/NFSMWRELEASE.ELF",),
    ),
    PlatformCheck(
        version="EUROPEGERMILESTONE",
        label="Xbox 360",
        required_assets=("orig/EUROPEGERMILESTONE/NfsMWEuropeGerMilestone.xex",),
    ),
    PlatformCheck(
        version="SLES-53558-A124",
        label="PS2",
        required_assets=("orig/SLES-53558-A124/NFS.ELF",),
    ),
)

PLATFORM_BY_VERSION = {platform.version: platform for platform in PLATFORMS}


def print_section(title: str) -> None:
    print(f"\n== {title} ==", flush=True)


def tail_lines(text: str, count: int) -> str:
    lines = text.rstrip().splitlines()
    if len(lines) <= count:
        return "\n".join(lines)
    return "\n".join(lines[-count:])


def run_logged(command: List[str], log_path: str) -> StepResult:
    start = time.monotonic()
    try:
        completed = subprocess.run(
            command,
            cwd=ROOT_DIR,
            capture_output=True,
            text=True,
            errors="replace",
        )
        output = completed.stdout
        if completed.stderr:
            if output and not output.endswith("\n"):
                output += "\n"
            output += completed.stderr
        returncode = completed.returncode
    except OSError as exc:
        output = str(exc)
        returncode = 127
    elapsed = time.monotonic() - start

    os.makedirs(os.path.dirname(log_path), exist_ok=True)
    with open(log_path, "w", encoding="utf-8") as log_file:
        log_file.write(output)

    return StepResult(
        name=os.path.basename(log_path),
        command=command,
        returncode=returncode,
        elapsed=elapsed,
        log_path=log_path,
        output=output,
    )


def missing_assets(platform: PlatformCheck) -> List[str]:
    missing = []
    for rel_path in platform.required_assets:
        abs_path = os.path.join(ROOT_DIR, rel_path)
        if not os.path.exists(abs_path):
            missing.append(rel_path)
    return missing


def describe_failure(step: StepResult, tail_count: int) -> None:
    print(f"FAIL {step.name}: exit {step.returncode} in {step.elapsed:.2f}s")
    print(f"Command: {' '.join(step.command)}")
    print(f"Log: {step.log_path}")
    if step.output.strip():
        print("--- output tail ---")
        print(tail_lines(step.output, tail_count))


def run_platform(
    platform: PlatformCheck, build_target: Optional[str], jobs: int, tail_count: int
) -> PlatformResult:
    result = PlatformResult(platform=platform)
    logs_dir = os.path.join(ROOT_DIR, "build", platform.version, "logs")

    print_section(f"{platform.label} ({platform.version})")

    missing = missing_assets(platform)
    if missing:
        result.preflight_error = (
            "Missing required assets: "
            + ", ".join(missing)
            + " (hint: seed shared assets or run worktree bootstrap first)"
        )
        print(f"FAIL preflight: {result.preflight_error}")
        return result

    configure_cmd = [sys.executable, "configure.py", "--version", platform.version]
    configure_log = os.path.join(logs_dir, "build-matrix-configure.log")
    print(f"RUN configure: {' '.join(configure_cmd)}")
    result.configure = run_logged(configure_cmd, configure_log)
    if result.configure.ok:
        print(f"OK  configure: {result.configure.elapsed:.2f}s ({configure_log})")
    else:
        describe_failure(result.configure, tail_count)
        return result

    build_cmd = ["ninja", "-j", str(jobs)]
    if build_target is not None:
        build_cmd.append(build_target)
    build_name = build_target or "default"
    build_log = os.path.join(logs_dir, f"build-matrix-{build_name}.log")
    print(f"RUN build: {' '.join(build_cmd)}")
    result.build = run_logged(build_cmd, build_log)
    if result.build.ok:
        print(f"OK  build: {result.build.elapsed:.2f}s ({build_log})")
    else:
        describe_failure(result.build, tail_count)

    return result


def restore_version(version: str, tail_count: int) -> bool:
    print_section(f"Restore {version}")
    log_path = os.path.join(ROOT_DIR, "build", version, "logs", "build-matrix-restore.log")
    step = run_logged([sys.executable, "configure.py", "--version", version], log_path)
    if step.ok:
        print(f"OK  restore: {step.elapsed:.2f}s ({log_path})")
        return True

    describe_failure(step, tail_count)
    return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Check sequential builds across all supported platforms."
    )
    parser.add_argument(
        "--version",
        dest="versions",
        action="append",
        choices=sorted(PLATFORM_BY_VERSION.keys()),
        help="Limit the run to one or more versions (default: all platforms).",
    )
    parser.add_argument(
        "--all-source",
        action="store_true",
        help="Run `ninja all_source` instead of the default full `ninja`.",
    )
    parser.add_argument(
        "--jobs",
        type=int,
        default=1,
        help="Parallelism passed to ninja (default: 1).",
    )
    parser.add_argument(
        "--tail",
        type=int,
        default=40,
        help="How many output lines to print when a step fails (default: 40).",
    )
    parser.add_argument(
        "--restore-version",
        default=DEFAULT_RESTORE_VERSION,
        choices=sorted(PLATFORM_BY_VERSION.keys()),
        help=f"Version to restore at the end (default: {DEFAULT_RESTORE_VERSION}).",
    )
    parser.add_argument(
        "--no-restore",
        action="store_true",
        help="Leave the worktree configured for the last checked version.",
    )
    return parser.parse_args()


def print_summary(
    results: Sequence[PlatformResult], restore_version_name: str, restore_ok: Optional[bool]
) -> None:
    print_section("Summary")
    for result in results:
        if result.preflight_error is not None:
            print(f"FAIL {result.platform.version}: {result.preflight_error}")
            continue
        if result.configure is None or not result.configure.ok:
            assert result.configure is not None
            print(
                f"FAIL {result.platform.version}: configure exit {result.configure.returncode} "
                f"({result.configure.elapsed:.2f}s)"
            )
            continue
        if result.build is None or not result.build.ok:
            assert result.build is not None
            print(
                f"FAIL {result.platform.version}: build exit {result.build.returncode} "
                f"({result.build.elapsed:.2f}s)"
            )
            continue
        total = result.configure.elapsed + result.build.elapsed
        print(f"OK   {result.platform.version}: {total:.2f}s")

    if restore_ok is not None:
        status = "OK" if restore_ok else "FAIL"
        print(f"{status:4} restore: {restore_version_name}")


args = parse_args()


def main() -> int:
    selected_versions = args.versions or [platform.version for platform in PLATFORMS]
    platforms = [PLATFORM_BY_VERSION[version] for version in selected_versions]
    build_target = "all_source" if args.all_source else None
    results: List[PlatformResult] = []
    restore_ok: Optional[bool] = None

    print(f"Root: {ROOT_DIR}")
    print(f"Build target: {build_target or 'ninja default'}")

    try:
        for platform in platforms:
            results.append(run_platform(platform, build_target, args.jobs, args.tail))
    finally:
        if not args.no_restore:
            restore_ok = restore_version(args.restore_version, args.tail)

    print_summary(results, args.restore_version, restore_ok)

    if restore_ok is False:
        return 1
    if any(not result.ok for result in results):
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
