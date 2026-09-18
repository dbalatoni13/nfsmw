#!/usr/bin/env python3
"""Compile with EE-GCC in two steps so that `#pragma implementation` works.

Usage: ee_gcc_pp.py <preprocessed-dir> <compiler command...> -- <gcc args...> -c -o <out> <source>

The EE-GCC 2.9-ee-991111 driver preprocesses into a temporary .ii and hands that
file to cc1plus. The line markers name the real source, so cc1plus sees two
different file names and rejects every `#pragma implementation` ("can only appear
at top-level"). Here the source is preprocessed to <preprocessed-dir>/<source> and
then compiled as cpp-output from <preprocessed-dir>, under the same relative name
the line markers use. Line information is kept, and the object is otherwise the
same as with a single call.

DEPENDENCIES_OUTPUT (set by the caller) only applies to the preprocessing step.
"""

import os
import subprocess
import sys


def main() -> int:
    argv = sys.argv[1:]
    if "--" not in argv or len(argv) < 4:
        print(__doc__, file=sys.stderr)
        return 2
    sep = argv.index("--")
    ppdir = argv[0]
    # The second step runs from ppdir, so relative tool paths must be made absolute.
    compiler = [os.path.abspath(c) if os.path.exists(c) else c for c in argv[1:sep]]
    args = argv[sep + 1 :]

    source = args[-1]
    out_index = args.index("-o")
    output = args[out_index + 1]
    flags = [a for i, a in enumerate(args[:-1]) if a != "-c" and i not in (out_index, out_index + 1)]

    is_cxx = source.lower().endswith((".cpp", ".cc", ".cxx", ".cp"))
    for i, a in enumerate(flags[:-1]):
        if a == "-x":
            is_cxx = flags[i + 1] in ("c++", "c++-header")
    language = "c++-cpp-output" if is_cxx else "cpp-output"

    # A failed build must not leave the previous object behind: the report would
    # measure it as if it came from the current source.
    if os.path.exists(output):
        os.remove(output)

    preprocessed = os.path.join(ppdir, source)
    os.makedirs(os.path.dirname(preprocessed) or ".", exist_ok=True)
    result = subprocess.run(compiler + flags + ["-E", "-o", preprocessed, source])
    if result.returncode:
        return result.returncode

    env = dict(os.environ)
    env.pop("DEPENDENCIES_OUTPUT", None)
    result = subprocess.run(
        compiler + flags + ["-x", language, "-c", "-o", os.path.abspath(output), source],
        cwd=ppdir,
        env=env,
    )
    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
