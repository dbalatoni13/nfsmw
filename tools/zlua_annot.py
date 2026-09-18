#!/usr/bin/env python3
"""Annotate zLua asm with DWARF line info for decomp work."""
import re
import sys

ASM = "build/GOWE69/asm/Speed/Indep/SourceLists/zLua.s"
LINES = "symbols/debug_lines.txt"

def load_lines():
    m = {}
    pat = re.compile(r"^0x([0-9A-Fa-f]+): (.+?) \(line (\d+)\)$")
    with open(LINES, "r", encoding="utf-8", errors="replace") as f:
        for ln in f:
            mo = pat.match(ln.strip())
            if mo:
                addr = int(mo.group(1), 16)
                m[addr] = (mo.group(2), int(mo.group(3)))
    return m

def main():
    start = int(sys.argv[1], 16)
    end = int(sys.argv[2], 16)
    lines = load_lines()
    apat = re.compile(r"^/\* ([0-9A-F]{8}) ")
    last = None
    with open(ASM, "r", encoding="utf-8", errors="replace") as f:
        for ln in f:
            mo = apat.match(ln)
            if mo:
                addr = int(mo.group(1), 16)
                if start <= addr < end:
                    info = lines.get(addr)
                    if info and info != last:
                        print("        ;;; %s : %d" % (info[0].split('/')[-1], info[1]))
                        last = info
                    print(ln.rstrip())
            elif start != end:
                pass

if __name__ == "__main__":
    main()
