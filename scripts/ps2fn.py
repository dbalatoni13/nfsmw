#!/usr/bin/env python3
"""ps2fn.py -- que hace una funcion, leido del build de PS2 (Alpha 124).

Cuando hay que escribir una funcion de cero y el DWARF de GameCube no basta,
el build de PS2 la tiene **con nombre**: `orig/SLES-53558-A124/NFS.MAP` trae
29.142 simbolos con direccion y tamano, y `NFS.ELF` el codigo.

Es MIPS, asi que el codigo NO se copia: se traduce. Pero lo que si se lee
directamente y vale mucho es **la lista ordenada de llamadas**, que es el
esqueleto de la funcion (el equivalente al arbol de inlines de
`plan.py --dwarf-only`, pero para lo que el DWARF de GameCube no cubre).

    python scripts/ps2fn.py Update__13AIPerpVehiclef      # por simbolo exacto
    python scripts/ps2fn.py AverageAir                    # por subcadena
    python scripts/ps2fn.py Update__13AIPerpVehiclef -n   # solo la ficha

MIPS: `jal` es opcode 3 y su destino es (pc & 0xF0000000) | (target << 2).
"""
import os
import re
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
MAP = os.path.join(ROOT, "orig", "SLES-53558-A124", "NFS.MAP")
ELF = os.path.join(ROOT, "orig", "SLES-53558-A124", "NFS.ELF")

RE_SYM = re.compile(r"^\s*([0-9a-f]{8})\s+([0-9a-f]{8})\s+\d+\s+(\S+)\s*$", re.M)


def carga_mapa():
    txt = open(MAP, encoding="latin1").read()
    porname, porarr = {}, []
    for a, sz, nm in RE_SYM.findall(txt):
        a, sz = int(a, 16), int(sz, 16)
        if not sz:
            continue
        porname.setdefault(nm, (a, sz))
        porarr.append((a, sz, nm))
    porarr.sort()
    return porname, porarr


def segmentos():
    d = open(ELF, "rb").read(64)
    e = "<" if d[5] == 1 else ">"
    phoff, = struct.unpack(e + "I", d[28:32])
    phes, = struct.unpack(e + "H", d[42:44])
    phn, = struct.unpack(e + "H", d[44:46])
    f = open(ELF, "rb")
    f.seek(phoff)
    hdr = f.read(phes * phn)
    segs = []
    for i in range(phn):
        o = i * phes
        t, off, va, pa, fs = struct.unpack(e + "5I", hdr[o:o + 20])
        if t == 1:
            segs.append((va, off, fs))
    return f, e, segs


def lee(f, segs, addr, n):
    for va, off, fs in segs:
        if va <= addr < va + fs:
            f.seek(off + (addr - va))
            return f.read(min(n, va + fs - addr))
    return b""


def resuelve(porarr, addr):
    lo, hi = 0, len(porarr) - 1
    while lo <= hi:
        m = (lo + hi) // 2
        a, sz, nm = porarr[m]
        if a <= addr < a + sz:
            return nm, addr - a
        if addr < a:
            hi = m - 1
        else:
            lo = m + 1
    return None, 0


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    q = sys.argv[1]
    solo_ficha = "-n" in sys.argv
    porname, porarr = carga_mapa()
    if q in porname:
        cands = [q]
    else:
        cands = sorted(n for n in porname if q.lower() in n.lower())
        if not cands:
            print("no encuentro ningun simbolo con", q)
            return
        if len(cands) > 1:
            print("%d simbolos coinciden; los primeros:" % len(cands))
            for c in cands[:15]:
                a, sz = porname[c]
                print("   %08x %6d B  %s" % (a, sz, c))
            if len(cands) > 15:
                return
    f, e, segs = segmentos()
    for nm in cands[:3]:
        a, sz = porname[nm]
        print("=== %s" % nm)
        print("    PS2: 0x%08x  %d bytes  (%d instrucciones)" % (a, sz, sz // 4))
        if solo_ficha:
            continue
        code = lee(f, segs, a, sz)
        llamadas = []
        for i in range(0, len(code) - 3, 4):
            w, = struct.unpack(e + "I", code[i:i + 4])
            op = w >> 26
            if op == 3:                       # jal
                dest = ((a + i) & 0xF0000000) | ((w & 0x03FFFFFF) << 2)
                d, delta = resuelve(porarr, dest)
                llamadas.append((i, d or "0x%08x" % dest))
        print("    %d llamadas directas, en orden:" % len(llamadas))
        vistas = 0
        for off, d in llamadas:
            print("      +0x%04x  %s" % (off, d))
            vistas += 1
            if vistas >= 60:
                print("      ... y %d mas" % (len(llamadas) - vistas))
                break
        print()
    f.close()


if __name__ == "__main__":
    main()
