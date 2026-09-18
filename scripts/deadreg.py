#!/usr/bin/env python3
"""deadreg.py -- caza accesores con CUERPO VACIO por su firma en el asm.

Un metodo no-`void` cuyo cuerpo es `{}` devuelve basura, y el llamante pierde
codigo en silencio. Buscarlos por NOMBRE da falsos positivos por homonimia
(`GetHead` de `FEHashNode` no lo usa nadie en `Frontend/`), asi que el barrido
util es el otro:

    GCC inlina "nada", el pseudo nace indefinido y reload le da cualquier
    registro. El resultado es una instruccion que LEE un registro que NADIE
    ESCRIBE en toda la funcion.

Ese es el detector, y no depende de nombres. Cazo 704 B en una sola pasada
cuando se descubrio a mano.

    python scripts/deadreg.py zFe            # una unidad
    python scripts/deadreg.py                # todas las SourceLists
    python scripts/deadreg.py zFe --all      # incluye las que ya casan al 100%

Lee el JSON de objdiff (lo genera si no existe) y mira NUESTRO lado.
Excluye r1/r2/r13 (sp, toc, sda), los registros de argumento r3-r10 y f1-f8, y
r0 (que GCC usa como scratch sin definir en algunos prologos).
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
OBJDIFF = os.path.join(ROOT, "objdiff-cli-windows-x86_64.exe")

# registros que pueden leerse legitimamente sin escribirse en la funcion
IGNORA = {"r0", "r1", "r2", "r13"} | {"r%d" % i for i in range(3, 11)} \
         | {"f%d" % i for i in range(1, 9)}

RE_REG = re.compile(r"\b([rf]\d{1,2})\b")
# instrucciones que NO escriben su primer operando
NO_ESCRIBE = ("stw", "stb", "sth", "stfs", "stfd", "stmw", "stwu", "stwx", "stbx",
              "sthx", "stfsx", "stfdx", "stfsu", "stwux", "cmp", "cmpw", "cmpl",
              "cmplw", "cmpwi", "cmplwi", "fcmpu", "fcmpo", "b", "bl", "blr",
              "bctr", "bctrl", "mtlr", "mtctr", "mtcrf", "mtspr", "mtmsr",
              "psq_st", "psq_stx", "dcbf", "dcbi", "dcbst", "dcbt", "dcbz",
              "icbi", "sync", "isync", "eieio", "nop", "twi", "tw")


def analiza(sym):
    """Devuelve los registros leidos y nunca escritos en esta funcion."""
    escritos, leidos = set(), set()
    for e in sym.get("instructions") or []:
        ins = (e.get("instruction") or {})
        txt = (ins.get("formatted") or "").strip()
        if not txt:
            continue
        mnem = txt.split()[0]
        regs = RE_REG.findall(txt)
        if not regs:
            continue
        if mnem.startswith(("b", "cmp", "fcmp", "mt", "st", "psq_st", "dcb", "icbi")) \
                or mnem in NO_ESCRIBE:
            leidos.update(regs)
            continue
        if mnem in ("lmw",):                # escribe desde el primero hasta r31
            n = int(regs[0][1:])
            escritos.update("r%d" % i for i in range(n, 32))
            leidos.update(regs[1:])
            continue
        escritos.add(regs[0])               # el resto escribe su primer operando
        leidos.update(regs[1:])
    return sorted(leidos - escritos - IGNORA, key=lambda r: (r[0], int(r[1:])))


def json_unidad(unit):
    # fichero propio: `fndiff.py` usa d_<unit>.json y hay agentes escribiendolo
    out = os.path.join(SCR, "dr_%s.json" % unit)
    if not os.path.exists(out):
        base = "Speed/Indep/SourceLists/%s.o" % unit
        r = subprocess.run(
            [OBJDIFF, "diff",
             "-1", os.path.join(ROOT, "build", "GOWE69", "obj", base.replace("/", os.sep)),
             "-2", os.path.join(ROOT, "build", "GOWE69", "src", base.replace("/", os.sep)),
             "-c", "function_reloc_diffs=none",
             "-c", "ppc.calculatePoolRelocations=false",
             "-o", out, "--format", "json", "zz"],
            capture_output=True, text=True)
        if r.returncode != 0 or not os.path.exists(out):
            return None
    return json.load(open(out))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    todas = "--all" in sys.argv
    if not args:
        sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
        args = sorted(f[:-4] for f in os.listdir(sl) if f.endswith(".cpp"))
    total = 0
    for unit in args:
        d = json_unidad(unit)
        if not d:
            continue
        izq = {s["name"]: s for s in d["left"]["symbols"]
               if s.get("kind") == "SYMBOL_FUNCTION"}
        hits = []
        for s in d["right"]["symbols"]:
            if s.get("kind") != "SYMBOL_FUNCTION":
                continue
            o = izq.get(s.get("name"))
            if o is None:
                continue
            if not todas and (o.get("match_percent") or 0) >= 100.0:
                continue
            malos = analiza(s)
            if malos:
                hits.append((int(o.get("size") or 0), s["name"], malos,
                             o.get("match_percent") or 0))
        hits.sort(reverse=True)
        if hits:
            print("=== %s: %d funciones leen un registro que nadie escribe" % (unit, len(hits)))
            for size, name, malos, pct in hits[:20]:
                print("  %7d B  %6.2f%%  %-52s %s" % (size, pct, name[:52], ",".join(malos)))
            print()
            total += len(hits)
    print("TOTAL de funciones sospechosas:", total)


if __name__ == "__main__":
    main()
