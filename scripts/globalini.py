#!/usr/bin/env python3
"""globalini.py -- el simbolo `_GLOBAL_.I.<nombre>` de cada unidad, comparado.

GCC 2.9 bautiza el constructor de estaticos de una unidad con
`first_global_object_name`: **el primer global emitido CON inicializador**. Una
definicion tentativa (`int X;`) se difiere al final de la TU y **nunca gana el
nombre**, asi que basta cambiarla a `int X = 0;` para que el simbolo pase a
llamarse como el original.

Y como objdiff empareja **por nombre**, mientras el nombre no coincide la funcion
entera cuenta como **0%** aunque sus bytes sean identicos. Son 44 B por unidad,
gratis, y el barrido cuesta segundos.

    python scripts/globalini.py            # todas las unidades
    python scripts/globalini.py zFe2       # una

Leer el resultado: si el objetivo dice `_GLOBAL_.I.FOO` y nosotros
`_GLOBAL_.I.BAR`, hay que hacer que **FOO** sea el primer global con
inicializador de esa unidad -- normalmente dandole un `= 0` explicito, o moviendo
la definicion de BAR detras.
"""
import os
import re
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
OBJ = os.path.join(ROOT, "build", "GOWE69", "obj")
SRC = os.path.join(ROOT, "build", "GOWE69", "src")
RE_GI = re.compile(r"^_GLOBAL_\.[ID]\.")


def simbolos(path):
    try:
        d = open(path, "rb").read()
    except OSError:
        return None
    if d[:4] != b"\x7fELF":
        return None
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    st = [x for x in secs if x[1] == 2]
    if not st:
        return set()
    st = st[0]
    strt = secs[st[6]]
    # OJO: `d[strt[4] + nameo:].split(...)` copia el resto del fichero por cada
    # simbolo -- es O(n^2) y con 1.200 objetos no termina nunca. Se corta la tabla
    # de cadenas UNA vez y se busca el NUL dentro.
    tab = d[strt[4]:strt[4] + strt[5]]
    out = set()
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, = struct.unpack(e + "I", d[o:o + 4])
        fin = tab.find(bytes([0]), nameo)
        n = tab[nameo:fin if fin >= 0 else None].decode("utf-8", "replace")
        if RE_GI.match(n):
            out.add(n)
    return out


def main():
    filtros = [a for a in sys.argv[1:] if not a.startswith("-")]
    malas, iguales, sin = [], 0, 0
    for root, _, fs in os.walk(OBJ):
        for f in sorted(fs):
            if not f.endswith(".o"):
                continue
            a = os.path.join(root, f)
            unit = os.path.relpath(a, OBJ)[:-2].replace(os.sep, "/")
            if filtros and not any(x in unit for x in filtros):
                continue
            b = os.path.join(SRC, os.path.relpath(a, OBJ))
            sa, sb = simbolos(a), simbolos(b)
            if sa is None or sb is None:
                continue
            if not sa and not sb:
                continue
            if sa == sb:
                iguales += 1
            elif not sb:
                sin += 1
            else:
                malas.append((unit, sorted(sa), sorted(sb)))
    print("unidades con `_GLOBAL_.I.` en los dos lados y el MISMO nombre: %d" % iguales)
    print("unidades donde nosotros no emitimos ninguno: %d" % sin)
    print()
    print("NOMBRE DISTINTO -- 44 B cada una, y objdiff las cuenta como 0%%: %d"
          % len(malas))
    for unit, sa, sb in malas:
        print("   %-58s" % unit[-58:])
        print("        objetivo: %s" % ", ".join(sa))
        print("        nuestro : %s" % ", ".join(sb))


if __name__ == "__main__":
    main()
