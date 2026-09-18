#!/usr/bin/env python3
"""undefcheck.py -- toda llamada a un simbolo que NO EXISTE en el ELF.

Complementa a callcheck: no depende de que el objetivo llame o no al simbolo,
solo de que el simbolo EXISTA. Una reubicacion R_PPC_REL24 nuestra a un nombre
que no esta en la symtab de orig/GOWE69/NFSMWRELEASE.ELF es un bug seguro:
manglado mal, namespace mal, sobrecarga mal, o metodo declarado y nunca escrito.

    python scripts/undefcheck.py            # todos los .o del arbol
    python scripts/undefcheck.py zFe zLua   # filtra por ruta de unidad

Filtra a REL24 a proposito: sin eso salen cientos de `lbl_` y estaticos de
clase, que son datos y no dicen nada.

Aviso: los objetos NonMatching se enlazan desde el objetivo, asi que un fallo
aqui no rompe el DOL -- pero si el objeto llega a Matching, si.
"""
import collections
import os
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
OBJ = os.path.join(ROOT, "build", "GOWE69", "src")
ELF = os.path.join(ROOT, "orig", "GOWE69", "NFSMWRELEASE.ELF")
R_PPC_REL24 = 10


def symtab(path, want_undef):
    d = open(path, "rb").read()
    if d[:4] != b"\x7fELF":
        return None
    e = ">"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    sts = [x for x in secs if x[1] == 2]
    if not sts:
        return None
    st = sts[0]
    strt = secs[st[6]]
    sd = d[strt[4]:strt[4] + strt[5]]
    names, undef, defined = [], set(), set()
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + "IIIBBH", d[o:o + 16])
        n = sd[nameo:sd.index(b"\0", nameo)].decode("utf-8", "replace")
        names.append(n)
        if not n:
            continue
        (undef if shndx == 0 else defined).add(n)
    if not want_undef:
        return defined
    calls = collections.Counter()
    for sec in secs:
        if sec[1] != 4 or not sec[9]:
            continue
        for i in range(sec[5] // sec[9]):
            o = sec[4] + i * sec[9]
            off, info, add_ = struct.unpack(e + "IIi", d[o:o + 12])
            if (info & 0xFF) != R_PPC_REL24:
                continue
            si = info >> 8
            if si < len(names) and names[si] in undef:
                calls[names[si]] += 1
    return calls


def main():
    filters = [a for a in sys.argv[1:] if not a.startswith("-")]
    elf = symtab(ELF, False)
    tot = collections.Counter()
    per = collections.defaultdict(dict)
    for root, dirs, files in os.walk(OBJ):
        for f in files:
            if not f.endswith(".o"):
                continue
            p = os.path.join(root, f)
            unit = os.path.relpath(p, OBJ).replace(os.sep, "/")[:-2]
            if filters and not any(x in unit for x in filters):
                continue
            c = symtab(p, True)
            if not c:
                continue
            for n, k in c.items():
                if n in elf or n.startswith(".") or n.startswith("$"):
                    continue
                tot[n] += k
                per[n][unit] = k
    print("simbolos llamados que NO existen en el ELF: %d   llamadas: %d"
          % (len(tot), sum(tot.values())))
    for n, k in tot.most_common():
        us = ", ".join("%s(%d)" % (u, v) for u, v in
                       sorted(per[n].items(), key=lambda x: -x[1])[:4])
        print("%5d  %-92s  %s" % (k, n[:92], us))


if __name__ == "__main__":
    main()
