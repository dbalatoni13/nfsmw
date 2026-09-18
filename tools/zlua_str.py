#!/usr/bin/env python3
"""Read strings from DOL and print bind-call sequence for an address range."""
import re
import struct
import sys

DOL = "orig/GOWE69/sys/main.dol"
BASE = 0x803C8C60
FOFF = 0x3C5C60

with open(DOL, "rb") as f:
    data = f.read()

def getstr(addr):
    off = FOFF + (addr - BASE)
    if off < 0 or off >= len(data):
        return None
    end = data.find(b"\x00", off)
    if end < 0:
        return None
    s = data[off:end]
    try:
        t = s.decode("ascii")
    except UnicodeDecodeError:
        return None
    return t

def main():
    start = int(sys.argv[1], 16)
    end = int(sys.argv[2], 16)
    pat_addr = re.compile(r"^/\* ([0-9A-F]{8}) [0-9A-F]{8}  ((?:[0-9A-F]{2} )+)")
    lis_re = re.compile(r"lis r(\d+), (\S+)@ha")
    addi_re = re.compile(r"addi r(\d+), r(\d+), (\S+)@l")
    bl_re = re.compile(r"bl (\S+)")
    with open("build/GOWE69/asm/Speed/Indep/SourceLists/zLua.s", encoding="utf-8", errors="replace") as f:
        his = {}
        pending = []
        for ln in f:
            mo = pat_addr.match(ln)
            if not mo:
                continue
            addr = int(mo.group(1), 16)
            if not (start <= addr < end):
                continue
            text = ln.split("*/", 1)[1].strip()
            m1 = lis_re.search(text)
            if m1:
                his[m1.group(1)] = m1.group(2)
                if not m1.group(2).startswith("lbl_"):
                    pending.append("HAr%s=%s" % (m1.group(1), m1.group(2)))
                continue
            m2 = addi_re.search(text)
            if m2:
                reg = m2.group(1)
                lbl = m2.group(3)
                if lbl.startswith("lbl_"):
                    s = getstr(int(lbl[4:], 16))
                    if s is not None:
                        pending.append("r%s=%r" % (reg, s))
                    continue
                src = m2.group(2)
                if src in his and his[src] == lbl:
                    continue  # completion of lis+addi pair
                pending.append("r%s=%s" % (reg, lbl))
                continue
            m3 = bl_re.search(text)
            if m3:
                name = m3.group(1)
                print("%s %-25s %s" % (mo.group(1), name[:25], "; ".join(pending)))
                pending = []
                continue

if __name__ == "__main__":
    main()
