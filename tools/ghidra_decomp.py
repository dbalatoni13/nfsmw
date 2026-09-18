#!/usr/bin/env python3
"""Decompile functions with Ghidra, headless and read-only.

Usage:
  python tools/ghidra_decomp.py [--project DIR/NAME] [--program NAME] [-o OUT] function|0xaddress...

Defaults: project ghidra_project/NFSMW, program NFSMWRELEASE.ELF (GameCube, with symbols).
Needs Ghidra in ghidra_*/ghidra_* at the repository root (or GHIDRA_HOME) and a JDK 21
(JAVA_HOME if it is 21 or newer, otherwise the first JDK 21+ under Program Files).
The project is opened read-only, so nothing is saved; it fails if the GUI has it open.
"""

import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def ghidra_home() -> str:
    """The newest Ghidra next to or inside the repository: a project saved by a newer
    Ghidra cannot be opened by an older one."""
    if os.environ.get("GHIDRA_HOME"):
        return os.environ["GHIDRA_HOME"]
    candidates = []
    for base in (ROOT, os.path.dirname(ROOT)):
        for pattern in ("ghidra_*", os.path.join("ghidra_*", "ghidra_*")):
            for home in glob.glob(os.path.join(base, pattern)):
                if glob.glob(os.path.join(home, "support", "analyzeHeadless*")):
                    m = re.match(r"ghidra_(\d+)\.(\d+)(?:\.(\d+))?", os.path.basename(home))
                    if m:
                        candidates.append((tuple(int(x or 0) for x in m.groups()), home))
    if not candidates:
        sys.exit("Ghidra not found: set GHIDRA_HOME")
    return max(candidates)[1]


def java_major(home: str) -> int:
    release = os.path.join(home, "release")
    if os.path.exists(release):
        m = re.search(r'JAVA_VERSION="(\d+)', open(release, encoding="utf-8", errors="replace").read())
        if m:
            return int(m.group(1))
    m = re.search(r"jdk-?(\d+)", os.path.basename(home.rstrip("\\/")))
    return int(m.group(1)) if m else 0


def java_home() -> str:
    current = os.environ.get("JAVA_HOME", "")
    if current and java_major(current) >= 21:
        return current
    pattern_roots = [os.environ.get("ProgramFiles", r"C:\Program Files"), "/usr/lib/jvm"]
    for base in pattern_roots:
        for home in sorted(glob.glob(os.path.join(base, "*", "jdk*")) + glob.glob(os.path.join(base, "jdk*")),
                           reverse=True):
            if java_major(home) >= 21:
                return home
    sys.exit("No JDK 21+ found: set JAVA_HOME")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("functions", nargs="+")
    parser.add_argument("--project", default=os.path.join(ROOT, "ghidra_project", "NFSMW"))
    parser.add_argument("--program", default="NFSMWRELEASE.ELF")
    parser.add_argument("-o", "--output", help="write here instead of stdout")
    parser.add_argument("--info", action="store_true",
                        help="print a program summary and the functions whose name contains each argument")
    args = parser.parse_args()

    home = ghidra_home()
    headless = os.path.join(home, "support", "analyzeHeadless" + (".bat" if os.name == "nt" else ""))
    project_dir, project_name = os.path.split(os.path.abspath(args.project))
    output = args.output or os.path.join(tempfile.mkdtemp(prefix="ghidra_decomp_"), "out.c")
    env = dict(os.environ, JAVA_HOME=java_home())
    cmd = [headless, project_dir, project_name, "-process", args.program, "-noanalysis", "-readOnly",
           "-scriptPath", os.path.join(ROOT, "tools", "ghidra"),
           "-postScript", "InfoPrograma.java" if args.info else "DecompilarFunciones.java",
           os.path.abspath(output), *args.functions]
    result = subprocess.run(cmd, env=env, capture_output=True, text=True, errors="replace")
    if result.returncode or not os.path.exists(output):
        sys.stderr.write(result.stdout[-3000:] + result.stderr[-3000:])
        return result.returncode or 1
    if not args.output:
        sys.stdout.write(open(output, encoding="utf-8").read())
    return 0


if __name__ == "__main__":
    sys.exit(main())
