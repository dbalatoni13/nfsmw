#!/usr/bin/env python3
"""checksplits.py -- valida `splits.txt` ANTES de re-extraer.

El troceador aborta a la primera pega y **canta una sola por ejecucion**, asi que
descubrir cinco problemas cuesta cinco `ninja` completos. Este script los saca
todos de golpe, leyendo `splits.txt` y `symbols.txt`:

  1. **Solapes.** No basta comparar con el rango contiguo: un rango grande puede
     contener a **tres** vecinos pequenos. Se barre con el maximo acumulado.
  2. **Rangos que terminan (o empiezan) DENTRO de un simbolo.** El troceador dice
     «ends within symbol» y para. Suele significar que el troceador fundio varios
     de nuestros datos en un solo `lbl_`, asi que nuestra seccion es mas corta
     que el bloque real.

    python scripts/checksplits.py

Si sale limpio, `rm build/GOWE69/config.json && python -m ninja` re-extrae. Y
recuerda: **editar `splits.txt` NO reejecuta el troceado por si solo** -- la regla
`split` depende de `config.yml`.
"""
import bisect
import io
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
SPLITS = os.path.join(ROOT, "config", "GOWE69", "splits.txt")
SYMBOLS = os.path.join(ROOT, "config", "GOWE69", "symbols.txt")


def rangos():
    cur, out = None, []
    for line in io.open(SPLITS, encoding="utf-8", errors="replace"):
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            cur = line.strip()[:-1]
            continue
        m = re.match(r"\s*\.(\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", line)
        if m and cur:
            out.append((int(m.group(2), 16), int(m.group(3), 16), "." + m.group(1), cur))
    return out


def simbolos():
    out = []
    for line in io.open(SYMBOLS, encoding="utf-8", errors="replace"):
        m = re.match(r"\s*(\S+)\s*=\s*\.(\w+):0x([0-9A-Fa-f]+);.*?size:0x([0-9A-Fa-f]+)",
                     line)
        if m:
            out.append((int(m.group(3), 16), int(m.group(4), 16), m.group(1)))
    out.sort()
    return out


def main():
    rs = sorted(rangos())
    sym = simbolos()
    inicios = [s[0] for s in sym]

    def parte(addr):
        i = bisect.bisect_right(inicios, addr) - 1
        if i < 0:
            return None
        a, sz, n = sym[i]
        return (a, sz, n) if a < addr < a + sz else None

    # 1) solapes, con maximo acumulado (la contencion no la ve el par contiguo)
    solapes, maxfin, duena = [], 0, None
    for r in rs:
        if r[0] < maxfin:
            solapes.append((r, duena))
        if r[1] > maxfin:
            maxfin, duena = r[1], r
    # 2) cortes de simbolo
    cortes = []
    for a, b, sec, u in rs:
        for etiqueta, addr in (("inicio", a), ("fin", b)):
            p = parte(addr)
            if p:
                cortes.append((u, sec, a, b, etiqueta, p))

    print("rangos en splits.txt: %d" % len(rs))
    print()
    print("SOLAPES: %d" % len(solapes))
    for r, d in solapes:
        print("   0x%08X..0x%08X %-8s %-44s" % (r[0], r[1], r[2], r[3][-44:]))
        print("        dentro de 0x%08X..0x%08X %s" % (d[0], d[1], d[3][-44:]))
    print()
    print("RANGOS QUE CORTAN UN SIMBOLO: %d" % len(cortes))
    for u, sec, a, b, et, p in cortes:
        print("   %-44s %-8s 0x%08X..0x%08X" % (u[-44:], sec, a, b))
        print("        el %s cae dentro de %s (0x%08X + 0x%X)" % (et, p[2], p[0], p[1]))
    print()
    if not solapes and not cortes:
        print("LIMPIO: se puede re-extraer.")
    else:
        print("%d problemas. El troceador solo cantaria UNO por ejecucion."
              % (len(solapes) + len(cortes)))
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
