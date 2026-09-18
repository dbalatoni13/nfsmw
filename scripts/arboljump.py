#!/usr/bin/env python3
"""arboljump.py -- compara el ARBOL DE SALTOS, que es lo que `mnemo` no ve.

`mnemo.py` compara el **multiconjunto** de mnemonicos: si los dos lados tienen
las mismas instrucciones, dice «asignador» y te manda a otra cosa. Pero un
`switch` con un `case` de menos puede tener **exactamente los mismos opcodes** y
un arbol de despacho distinto — mismas comparaciones, otras constantes, otra
polaridad.

Este barrido enfrenta, funcion a funcion, la secuencia
`(constante del cmpwi/cmplwi, mnemonico del salto siguiente)` de los dos lados.
Sobre 54 near-miss de una biblioteca saco **7 funciones con el arbol distinto**, y
de ahi salieron dos cierres — **y `mnemo` decia «mismo multiconjunto» en varias
de ellas**. Cuesta lo mismo y ve lo otro: **pasa los dos**.

    python scripts/arboljump.py zEAXSound        # el detalle de una unidad
    python scripts/arboljump.py --todas          # barrido global, resumen

Lectura tipica: si al objetivo le falta una comparacion que nosotros emitimos, el
nodo esta **acotado por sus dos ancestros** y `emit_case_nodes` no la emite — o
sea que **falta un `case`** por arriba o por abajo. La aritmetica del pivote del
playbook (un nodo de rango resta 2 y un singleton 1) dice cual.

Borra su volcado en el acto: con agentes compilando, el disco es el recurso
escaso y un volcado rancio miente en silencio.
"""
import json
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
SCR = os.environ.get(
    "SCRATCH",
    "C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/"
    "3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad")
CLI = os.path.join(ROOT, "objdiff-cli-windows-x86_64.exe")

RE_CMP = re.compile(r"^(cmpwi|cmplwi)\s+(?:cr\d+,\s*)?r\d+,\s*(-?0x[0-9a-f]+|-?\d+)$")
SALTOS = ("beq", "bne", "bgt", "blt", "bge", "ble")
VENTANA = 16   # cuantas instrucciones puede meter el planificador entre ambos


def texto(e):
    return ((e.get("instruction") or {}).get("formatted") or "").strip()


def secuencia(sym):
    """[(salto, constante)] en orden: el arbol de despacho tal como se emite."""
    ins = [texto(e) for e in sym.get("instructions") or []]
    out = []
    for i, t in enumerate(ins):
        m = RE_CMP.match(t)
        if not m:
            continue
        # El salto NO tiene por que ir pegado al cmpwi: el planificador mete
        # instrucciones en medio, y cuando pasa el nodo se caia de la secuencia y
        # la herramienta cantaba un `case` que faltaba. Paso medido: cmpwi en el
        # indice 119 y su ble en el 127 hicieron creer que a SphereVsBox le
        # faltaba un test de limite, cuando los dos lados tienen los mismos 11
        # cmpwi. Se busca hacia delante hasta el primer salto, y se corta si
        # aparece otra comparacion (eso ya seria otro nodo).
        for j in range(i + 1, min(i + 1 + VENTANA, len(ins))):
            if RE_CMP.match(ins[j]):
                break
            nx = (ins[j].split() or [""])[0]
            if nx in SALTOS:
                out.append("%s%s" % (nx, m.group(2)))
                break
    return out


def analiza(unit):
    base = "Speed/Indep/SourceLists/" + unit if "/" not in unit else unit
    a = os.path.join(ROOT, "build", "GOWE69", "obj", base.replace("/", os.sep) + ".o")
    b = os.path.join(ROOT, "build", "GOWE69", "src", base.replace("/", os.sep) + ".o")
    if not (os.path.exists(a) and os.path.exists(b)):
        return None
    out = os.path.join(SCR, "arbol_%s.json" % unit.replace("/", "_"))
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
            os.remove(out)
        except OSError:
            pass
    der = {s.get("name"): s for s in d["right"]["symbols"]}
    filas = []
    for s in d["left"]["symbols"]:
        n = s.get("name")
        if not n or s.get("kind") != "SYMBOL_FUNCTION":
            continue
        o = der.get(n)
        if not o or not o.get("instructions") or not s.get("instructions"):
            continue
        if float(o.get("match_percent") or 0) >= 100:
            continue
        sa, sb = secuencia(s), secuencia(o)
        if sa != sb:
            filas.append((int(s.get("size") or 0), o.get("match_percent") or 0, n, sa, sb))
    filas.sort(reverse=True)
    return filas


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    resumen = "--todas" in sys.argv
    if resumen:
        sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
        args = sorted(f[:-4] for f in os.listdir(sl) if f.endswith(".cpp"))
    if not args:
        print(__doc__)
        return

    total, tb = 0, 0
    todas = []
    for unit in args:
        filas = analiza(unit)
        if filas is None:
            continue
        if filas:
            total += len(filas)
            tb += sum(f[0] for f in filas)
            print("== %-14s %d funciones con el ARBOL DE SALTOS distinto (%d B)"
                  % (unit, len(filas), sum(f[0] for f in filas)))
            for sz, pct, n, sa, sb in (filas if not resumen else filas[:4]):
                print("   %7d B %8.4f%%  %s" % (sz, pct, n[:62]))
                print("      objetivo: %s" % (" ".join(sa) or "(sin arbol)"))
                print("      nuestro : %s" % (" ".join(sb) or "(sin arbol)"))
                todas.append((sz, unit, n))
    print()
    print("TOTAL: %d funciones con el arbol distinto, %d B" % (total, tb))
    if resumen and todas:
        todas.sort(reverse=True)
        print()
        print("LAS 12 MAYORES:")
        for sz, u, n in todas[:12]:
            print("   %7d B  %-12s %s" % (sz, u, n[:60]))


if __name__ == "__main__":
    main()
