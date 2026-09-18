#!/usr/bin/env python3
"""mnemo.py -- triaje por MULTICONJUNTO DE MNEMONICOS: separa fuente de asignador.

De todas las funciones near-miss de una unidad, dice **que opcode sobra o falta**
respecto al objetivo. Eso parte el trabajo en dos clases que se atacan de forma
completamente distinta:

  * **MISMO MULTICONJUNTO** -> las mismas instrucciones en otro orden o con otros
    registros. **No falta codigo**: es el asignador o el planificador, y no vas a
    encontrar nada leyendo la fuente. En una unidad clasifico **7 de 19** asi, y
    eso evita mandar a nadie a buscar codigo que no falta.
  * **`stfs +1, lfs +1, mr -1`** y demas -> **falta o sobra una SENTENCIA**, y el
    opcode dice cual. En `RenderVisibleSectionBoundary` esto nombro la causa en
    **una sola pasada** donde el diff no la enseñaba: faltaba `position.z = 0.0f;`.
    95,43% -> **100% a la primera compilacion**.

Es mas barato que leer diffs y mucho mas barato que compilar variantes.

    python scripts/fndiff.py zTrack zz     # genera el volcado
    python scripts/mnemo.py zTrack         # y lo triaja

Ordena por TAMANO, que es lo que decide los bytes (`matched_code` es
todo-o-nada: una funcion al 99,99% aporta cero).
"""
import collections
import json
import os
import sys

SCR = os.environ.get(
    "SCRATCH",
    "C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/"
    "3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad")


def reales(ins):
    """Instrucciones DE VERDAD, no ranuras.

    objdiff alinea los dos lados metiendo ranuras VACIAS donde a uno le falta
    algo, y `len(instructions)` las cuenta. Medido en `Setup__RoadblockFlow`:
    decia `n=151/151` cuando el objetivo tiene **149** reales y el nuestro 142.
    O sea que anunciaba «mismo numero» justo donde faltaban 7 instrucciones,
    que es el caso que este script existe para encontrar.
    """
    return sum(1 for e in ins or []
               if ((e.get("instruction") or {}).get("formatted") or "").strip())


def mnemonicos(ins):
    c = collections.Counter()
    for e in ins or []:
        f = ((e.get("instruction") or {}).get("formatted", "") or "").strip().split()
        if f:
            c[f[0]] += 1
    return c


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    unit = sys.argv[1]
    # fndiff.py deja el volcado como d_<unidad>.json
    for nombre in ("d_%s.json" % unit, "jf_d_%s.json" % unit):
        p = os.path.join(SCR, nombre)
        if os.path.exists(p):
            break
    else:
        print("no encuentro el volcado; corre antes:  python scripts/fndiff.py %s zz" % unit)
        return
    d = json.load(open(p))
    izq = {s.get("name"): s for s in d["left"]["symbols"]}
    der = {s.get("name"): s for s in d["right"]["symbols"]}

    filas = []
    for n, s in izq.items():
        if not n or s.get("kind") != "SYMBOL_FUNCTION":
            continue
        o = der.get(n)
        if o is None:
            continue
        pct = o.get("match_percent") or 0
        if pct >= 100.0:
            continue
        a, b = mnemonicos(s.get("instructions")), mnemonicos(o.get("instructions"))
        delta = {k: a.get(k, 0) - b.get(k, 0)
                 for k in set(a) | set(b) if a.get(k, 0) != b.get(k, 0)}
        filas.append((int(s.get("size") or 0), pct, n, delta,
                      reales(s.get("instructions")), reales(o.get("instructions"))))
    filas.sort(reverse=True)

    iguales = sum(1 for f in filas if not f[3])
    print("%d funciones near-miss; %d con MISMO MULTICONJUNTO (asignador, no fuente)"
          % (len(filas), iguales))
    print()
    for sz, pct, n, delta, na, nb in filas:
        if delta:
            tag = "objetivo-nuestro: " + str(sorted(delta.items(), key=lambda x: -abs(x[1])))
        else:
            tag = "MISMO MULTICONJUNTO -> asignador/planificador, NO busques codigo"
        print("%7d B %9.4f%%  n=%d/%d  %s" % (sz, pct, na, nb, n[:70]))
        print("          %s" % tag)


if __name__ == "__main__":
    main()
