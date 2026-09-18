#!/usr/bin/env python3
"""relocfantasma.py -- funciones que solo fallan porque el TROCEADOR simbolizo mal.

GCC materializa una constante grande con `lis`/`ori`. Si esa constante cae dentro
del rango de un simbolo de datos, el troceador cree que es un puntero y emite
reubicaciones `@h`/`@l`; el objeto de referencia guarda entonces el inmediato a
CERO, con lo que el `ori` sale como `nop` y objdiff cuenta dos instrucciones mal
**para siempre**. Nuestro `.o` no puede reproducirlo: emitimos el literal.

El delator es inequivoco: **los dos lados tienen los MISMOS BYTES y aun asi
objdiff los marca distintos**. Eso no puede pasar por ninguna otra causa.

La cura es una entrada en `block_relocations` de `config/GOWE69/config.yml` por
instruccion, y **re-extraer** (editar config.yml SI dispara la regla `split`,
editar `splits.txt` NO).

    python scripts/relocfantasma.py            # barrido global
    python scripts/relocfantasma.py --yaml     # + las entradas listas para pegar

Ojo: solo cuenta la funcion si los bytes iguales son **todo** lo que falla. Si
ademas hay codigo distinto, arreglar la reubicacion no la cierra -- pero sigue
mereciendo la entrada, porque son dos instrucciones menos de ruido.
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


def raw(e):
    """Los 4 bytes de la instruccion, como entero."""
    i = e.get("instruction") or {}
    for k in ("opcode", "code", "raw", "bytes"):
        if k in i and isinstance(i[k], int):
            return i[k]
    return None


def analiza(unit):
    base = "Speed/Indep/SourceLists/" + unit if "/" not in unit else unit
    a = os.path.join(ROOT, "build", "GOWE69", "obj", base.replace("/", os.sep) + ".o")
    b = os.path.join(ROOT, "build", "GOWE69", "src", base.replace("/", os.sep) + ".o")
    if not (os.path.exists(a) and os.path.exists(b)):
        return None
    out = os.path.join(SCR, "rf_%s.json" % unit.replace("/", "_"))
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
        pct = float(o.get("match_percent") or 0)
        if pct >= 100:
            continue
        li, ri = s["instructions"], o["instructions"]
        if len(li) != len(ri):
            continue                    # tamano distinto: no es solo la reubicacion
        malos, fantasma = 0, []
        for x, y in zip(li, ri):
            kx = x.get("diff_kind") or "DIFF_NONE"
            ky = y.get("diff_kind") or "DIFF_NONE"
            if kx == "DIFF_NONE" and ky == "DIFF_NONE":
                continue
            malos += 1
            tx = ((x.get("instruction") or {}).get("formatted") or "").strip()
            ty = ((y.get("instruction") or {}).get("formatted") or "").strip()
            bx, by = raw(x), raw(y)
            # el delator: MISMOS BYTES marcados distintos, y el objetivo lleva @h/@l
            if bx is not None and bx == by and re.search(r"@(h|l|ha)\b", tx):
                fantasma.append(((x.get("instruction") or {}).get("address"), tx, ty))
        if fantasma and len(fantasma) == malos:
            filas.append((int(s.get("size") or 0), pct, n, fantasma))
    filas.sort(reverse=True)
    return filas


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    yaml = "--yaml" in sys.argv
    if not args:
        sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
        args = sorted(f[:-4] for f in os.listdir(sl) if f.endswith(".cpp"))
    total, tb, ent = 0, 0, []
    for unit in args:
        filas = analiza(unit)
        if not filas:
            continue
        total += len(filas)
        tb += sum(f[0] for f in filas)
        print("== %-14s %d funcion(es) que SOLO fallan por la reubicacion (%d B)"
              % (unit, len(filas), sum(f[0] for f in filas)))
        for sz, pct, n, fantasma in filas:
            print("   %7d B %8.4f%%  %s" % (sz, pct, n[:60]))
            for addr, tx, ty in fantasma:
                print("        %08X  objetivo: %-40s nuestro: %s"
                      % (addr or 0, tx[:40], ty[:34]))
                ent.append((n, addr))
    print()
    print("TOTAL: %d funciones, %d B que se recuperan con block_relocations" % (total, tb))
    if yaml and ent:
        print()
        print("# pegar en block_relocations de config/GOWE69/config.yml:")
        ult = None
        for n, addr in ent:
            if n != ult:
                print("# %s" % n)
                ult = n
            print("- source: .text:0x%08X" % addr)
            print("  end: .text:0x%08X" % (addr + 4))


if __name__ == "__main__":
    main()
