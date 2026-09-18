#!/usr/bin/env python3
"""auditecho.py -- audita los TECHOS documentados: cuales son falsos.

Un techo del playbook dice «no cede a estas formas», pero se lee como «no cede».
Ya se ha demostrado caro: `Moment::React(Plane)` llevaba anotado «8 variantes a
mano mas ~1.650 del permutador, cero mejora» y **se cerro entera (5.668 B) con
cuatro cambios de fuente**. Y un `= false` de mas disolvio 58 de 60 diffs que
parecian rotacion pura del asignador.

El discriminante barato es el **multiconjunto de mnemonicos**:

  * **mismo multiconjunto** -> las mismas instrucciones en otro orden o con otros
    registros. Es asignador o planificador: el techo probablemente es real, y
    leer la fuente no va a dar nada.
  * **EL SIGNO ES `OBJETIVO - NUESTRO`**: `+N` = NOS FALTAN N; `-N` = NOS SOBRAN N.
  * **delta de mnemonicos** (`stfs +1`, `mr -9`, `bl +4`...) -> **falta o sobra
    codigo**, y el opcode dice cual. **Ese techo es FALSO** y merece otra ronda.

    python scripts/auditecho.py zCamera zEAXSound      # audita esas unidades
    python scripts/auditecho.py --todas                # todas las SourceLists

Lee cada volcado y lo **borra en el acto**: con siete agentes compilando, el
disco es el recurso escaso y un volcado rancio miente en silencio.
"""
import collections
import json
import os
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
SCR = os.environ.get(
    "SCRATCH",
    "C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/"
    "3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad")
CLI = os.path.join(ROOT, "objdiff-cli-windows-x86_64.exe")


def mnem(ins):
    c = collections.Counter()
    for e in ins or []:
        f = ((e.get("instruction") or {}).get("formatted", "") or "").strip().split()
        if f:
            c[f[0]] += 1
    return c


def audita(unit):
    """[(tamano, pct, nombre, delta, n_izq, n_der)] de las near-miss."""
    base = "Speed/Indep/SourceLists/" + unit if "/" not in unit else unit
    a = os.path.join(ROOT, "build", "GOWE69", "obj", base.replace("/", os.sep) + ".o")
    b = os.path.join(ROOT, "build", "GOWE69", "src", base.replace("/", os.sep) + ".o")
    if not (os.path.exists(a) and os.path.exists(b)):
        return None
    out = os.path.join(SCR, "audit_%s.json" % unit.replace("/", "_"))
    r = subprocess.run([CLI, "diff", "-1", a, "-2", b,
                        "-c", "function_reloc_diffs=none",
                        "-c", "ppc.calculatePoolRelocations=false",
                        "-o", out, "--format", "json", "zz"],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0 or not os.path.exists(out):
        return None
    try:
        d = json.load(open(out))
    except Exception:
        return None
    finally:
        try:
            os.remove(out)          # borrar YA: el disco es el recurso escaso
        except OSError:
            pass
    der = {s.get("name"): s for s in d["right"]["symbols"]}
    filas = []
    for s in d["left"]["symbols"]:
        if s.get("kind") != "SYMBOL_FUNCTION":
            continue
        n = s.get("name")
        o = der.get(n)
        if not n or o is None:
            continue
        pct = o.get("match_percent") or 0
        if pct >= 100.0:
            continue
        ia, ib = s.get("instructions"), o.get("instructions")
        ma, mb = mnem(ia), mnem(ib)
        delta = {k: ma.get(k, 0) - mb.get(k, 0)
                 for k in set(ma) | set(mb) if ma.get(k, 0) != mb.get(k, 0)}
        # NO uses len(): objdiff mete ranuras VACIAS para alinear los dos lados
        # y entonces los dos cuentan igual justo donde falta codigo.
        filas.append((int(s.get("size") or 0), pct, n, delta,
                      sum(ma.values()), sum(mb.values())))
    filas.sort(reverse=True)
    return filas


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    if "--todas" in sys.argv:
        sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
        args = sorted(f[:-4] for f in os.listdir(sl) if f.endswith(".cpp"))
    if not args:
        print(__doc__)
        return

    falsos, reales, bytes_falsos = [], 0, 0
    for unit in args:
        filas = audita(unit)
        if filas is None:
            print("== %-14s (sin objetos o objdiff fallo)" % unit)
            continue
        con = [f for f in filas if f[3]]
        sin = [f for f in filas if not f[3]]
        bf = sum(f[0] for f in con)
        bytes_falsos += bf
        reales += len(sin)
        print("== %-14s %3d near-miss: %3d con DELTA (%6d B, falta codigo), %3d mismo multiconjunto"
              % (unit, len(filas), len(con), bf, len(sin)))
        for sz, pct, n, delta, na, nb in con[:6]:
            top = sorted(delta.items(), key=lambda x: -abs(x[1]))[:4]
            print("     %7d B %8.4f%%  %-44s %s"
                  % (sz, pct, n[:44], ", ".join("%s%+d" % (k, v) for k, v in top)))
            falsos.append((sz, unit, n, top))
    print()
    print("TOTAL: %d funciones con delta de mnemonicos (%d B) -> ahi FALTA CODIGO"
          % (len(falsos), bytes_falsos))
    print("       %d con el mismo multiconjunto -> asignador/planificador" % reales)
    if falsos:
        falsos.sort(reverse=True)
        print()
        print("LAS 12 MAYORES, por bytes:")
        for sz, u, n, top in falsos[:12]:
            print("   %7d B  %-12s %-46s %s"
                  % (sz, u, n[:46], ", ".join("%s%+d" % (k, v) for k, v in top)))


if __name__ == "__main__":
    main()
