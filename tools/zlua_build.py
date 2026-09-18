#!/usr/bin/env python3
"""Build zLua.o standalone and report result."""
import os
import subprocess
import sys

ROOT = r"C:\Users\jferr\Desktop\nfsdecompiled"
TMP = os.environ.get("TEMP", r"C:\Users\jferr\AppData\Local\Temp")
CMDTXT = os.path.join(TMP, "cmd.txt")
BAT = os.path.join(TMP, "build_zlua.bat")

r = subprocess.run(["python", "-m", "ninja", "-t", "commands",
                    "build/GOWE69/src/Speed/Indep/SourceLists/zLua.o"],
                   cwd=ROOT, capture_output=True, text=True)
lines = [l for l in r.stdout.splitlines() if "zLua.o" in l and "ngccc" in l]
if not lines:
    print("no command found")
    sys.exit(1)
line = lines[-1].replace("cmd /c ", "", 1)
setpart, comp = line.split("&&", 1)
# Keep output at the decomp (base) path; obj/ holds the reference object.
with open(BAT, "w") as f:
    f.write("@echo off\n" + setpart.strip() + "\n" + comp.strip() + "\n")
r2 = subprocess.run(["cmd", "/c", BAT], cwd=ROOT, capture_output=True, text=True)
out = r2.stdout + r2.stderr
tail = "\n".join(out.splitlines()[-15:])
print(tail)
sys.exit(r2.returncode)
