#!/usr/bin/env python3
"""claimlbl.py -- de que direccion es el `.rodata` de cada unidad, SIN adivinar.

`claimdata.py` busca nuestros bytes de `.rodata` dentro del monton que el
troceador dejo sin duena. Funciona cuando los bytes son singulares, pero de 30
unidades deja **25 ambiguas**: un `.rodata` de 4 bytes que contiene `1.0f`
aparece en cientos de sitios y el metodo no puede decidir.

Este script no busca: **lee la respuesta**. Como la unidad casa al 100%, cada
instruccion nuestra que referencia un `$LCn` tiene enfrente la instruccion del
objetivo referenciando un `lbl_XXXXXXXX`, y el troceador ya calculo esa
direccion. **El emparejamiento de objdiff da la tabla `$LCn -> direccion` gratis**,
y con los tamanos de nuestra tabla de simbolos sale el rango entero.

Medido: en `sfxrevc` --que `claimdata` daba como «4 B en 4 sitios»-- sale
`lbl_80412B78`, y encaja pegado al rango de `smixer` (`0x80412B7C`), que se habia
deducido por bytes. Los dos metodos coinciden donde ambos opinan; este ademas
opina donde el otro se calla.

    python scripts/claimlbl.py                  # las candidatas y sus rangos
    python scripts/claimlbl.py --yaml           # lineas listas para splits.txt

**Esto NO escribe nada**: reclamar un rango exige re-extraer (borrar
`build/GOWE69/config.json`, porque la regla `split` depende de `config.yml` y no
de `splits.txt`) y eso regenera `obj/` y `asm/` para todos los agentes. Es trabajo
de ventana. Y el rango tiene que **terminar en un limite de simbolo** o el
troceador aborta con «ends within symbol».
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
SPLITS = os.path.join(ROOT, "config", "GOWE69", "splits.txt")

sys.path.insert(0, os.path.join(ROOT, "scripts"))
import rodata      # noqa: E402
import promote     # noqa: E402

ELF = os.path.join(ROOT, "orig", "GOWE69", "NFSMWRELEASE.ELF")
_ELF = None


def elf_bytes(addr, n):
    """Los n bytes del ELF original en esa direccion, o None."""
    global _ELF
    if _ELF is None:
        _ELF = rodata.load(ELF)
    d, _, secs = _ELF
    for s in secs:
        if s["addr"] and s["addr"] <= addr and addr + n <= s["addr"] + s["size"]                 and s["typ"] != 8:
            o = s["off"] + (addr - s["addr"])
            return d[o:o + n]
    return None


def seccion_objetivo(addr):
    """El nombre de la seccion del ELF ORIGINAL en esa direccion.

    IMPORTANTE: `splits.txt` habla en secciones del objetivo, no en las nuestras.
    Los `__clz_tab` de `libgcc2` viven en `.sdata2` alli y en `.rodata` aqui; una
    linea con nuestro nombre habria escrito el rango en la seccion equivocada.
    """
    global _ELF
    if _ELF is None:
        _ELF = rodata.load(ELF)
    d, _, secs = _ELF
    for s in secs:
        if s["addr"] and s["addr"] <= addr < s["addr"] + s["size"]:
            return s["sname"]
    return None


def comprueba(unit, sec, ini, fin):
    """VERIFICACION SIN RE-EXTRAER: nuestros bytes contra el ELF original.

    Deducir la direccion es una cosa y acertar es otra. Como la unidad casa al
    100%, sus datos tambien deben ser identicos byte a byte, asi que basta leer
    el ELF en el rango deducido y compararlo con nuestra seccion. Si cuadra, el
    rango es CIERTO y la ventana no es un salto de fe.
    """
    b = os.path.join(ROOT, "build", "GOWE69", "src", unit.replace("/", os.sep) + ".o")
    d, E, secs, syms = rodata.syms(b)
    for x in secs:
        if x["sname"] == sec and x["size"]:
            if x["typ"] == 8:              # .bss: sin contenido que comparar
                return None
            mios = d[x["off"]:x["off"] + x["size"]]
            suyos = elf_bytes(ini, len(mios))
            if suyos is None:
                return None
            return mios == suyos
    return None

RE_LBL = re.compile(r"\b(?:lbl|jumptable|gap|pad)_([0-9A-Fa-f]{8})@")
RE_REF = re.compile(r"[\s,(]([A-Za-z_$.][A-Za-z0-9_$.]*)@")


def rangos_declarados():
    """{unidad: {seccion: (ini, fin)}} tal como esta hoy en splits.txt."""
    out, cur = {}, None
    for line in open(SPLITS, encoding="utf-8", errors="replace"):
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            cur = line.strip()[:-1]
            out.setdefault(cur, {})
            continue
        m = re.match(r"\s*\.(\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", line)
        if m and cur:
            out[cur]["." + m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return out


def mapa(unit):
    """{$LCn: direccion} leyendo el emparejamiento de objdiff."""
    a = os.path.join(ROOT, "build", "GOWE69", "obj", unit.replace("/", os.sep) + ".o")
    b = os.path.join(ROOT, "build", "GOWE69", "src", unit.replace("/", os.sep) + ".o")
    if not (os.path.exists(a) and os.path.exists(b)):
        return None
    out = os.path.join(SCR, "cl_%s.json" % unit.replace("/", "_"))
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
    pares = {}
    for s in d["left"]["symbols"]:
        o = der.get(s.get("name"))
        if not o:
            continue
        for x, y in zip(s.get("instructions") or [], o.get("instructions") or []):
            fx = ((x.get("instruction") or {}).get("formatted") or "")
            fy = ((y.get("instruction") or {}).get("formatted") or "")
            mx, my = RE_LBL.search(fx), RE_REF.search(fy)
            if mx and my:
                pares.setdefault(my.group(1), set()).add(int(mx.group(1), 16))
    # un $LC que sale con DOS direcciones distintas es una contradiccion: fuera
    return {k: v.pop() for k, v in pares.items() if len(v) == 1}


def nuestras_secciones(unit):
    """{simbolo: (seccion, offset, tamano)} de NUESTRO objeto, solo DATOS.

    Al principio esto solo miraba `$LCn`, y por eso no veia las 20 unidades de
    `libc` cuyo `.sdata` lleva simbolos con nombre (`huge`, `zero`): el objetivo
    los alcanza con `lfd f0, lbl_804FF878@sda21(r0)` y la pareja se deduce igual.
    """
    b = os.path.join(ROOT, "build", "GOWE69", "src", unit.replace("/", os.sep) + ".o")
    d, E, secs, syms = rodata.syms(b)
    out, tam = {}, {}
    DATOS = (".rodata", ".sdata", ".sdata2", ".data", ".sbss", ".bss")
    for n, v, sz, sh in syms:
        if n and sh < len(secs) and secs[sh]["sname"] in DATOS:
            out[n] = (secs[sh]["sname"], v, sz)
    for s in secs:
        tam[s["sname"]] = s["size"]
    return out, tam


def solapa(decl, sec, ini, fin):
    """El rango pisa a otro ya declarado? El troceador ABORTA si eso pasa.

    MEDIDO en la ventana de la ronda 9: dos de 40 rangos caian DENTRO del de un
    vecino (`ssys` dentro de `sstvol`, `sfxrevc` dentro de `mpegl3base`), y el
    troceador para con «overlaps with previous split» -- una unidad por
    ejecucion, asi que sin esta comprobacion se descubren de uno en uno.
    Cuando pasa, el sospechoso es el rango GRANDE: se esta tragando los datos
    de su vecino.
    """
    # OJO: no basta comparar con el rango CONTIGUO. Un rango grande puede
    # contener a TRES vecinos pequenos, y el troceador solo canta uno por
    # ejecucion -- se descubren de uno en uno, a un `ninja` cada uno. Hay que
    # comparar contra TODOS los declarados.
    for unidad, secs in decl.items():
        for s2, (a, b) in secs.items():
            if s2 != sec:
                continue
            if ini < b and a < fin:
                return "%s 0x%08X..0x%08X" % (unidad[-40:], a, b)
    return None


def main():
    yaml = "--yaml" in sys.argv
    args = [x for x in sys.argv[1:] if not x.startswith("-")]
    decl = rangos_declarados()

    if not args:
        cands = promote.candidatas()
        args = []
        for tc, td, unit in cands:
            info, motivos = promote.revisa(unit)
            if any("emitimos de mas" in m for m in motivos):
                args.append((tc, unit))
        args.sort(reverse=True)
        args = [u for _, u in args]

    listos, sin, choques = [], [], []
    for unit in args:
        m = mapa(unit)
        if not m:
            sin.append((unit, "sin pares $LC/lbl"))
            continue
        nuestras, tams = nuestras_secciones(unit)
        porsec, desorden = {}, {}
        for lc, addr in m.items():
            sec, off, sz = nuestras.get(lc, (None, None, None))
            if sec is None:
                continue
            porsec.setdefault(sec, []).append((addr, sz or 4, off, lc))
        for sec, items in sorted(porsec.items()):
            ya = decl.get(unit + ".c", decl.get(unit + ".cpp", {})).get(sec)
            if ya:
                continue                       # ya declarado
            ini = min(a for a, _, _, _ in items)
            fin = max(a + z for a, z, _, _ in items)
            # si `direccion - offset` no es el mismo para todos, nuestro ORDEN
            # dentro de la seccion no es el del original. El rango sigue siendo
            # valido (min..max), pero promocionar exigira reordenar la fuente.
            bases = {a - o for a, _, o, _ in items}
            choca = solapa(decl, sec, ini, fin)
            osec = seccion_objetivo(ini) or sec
            ok = comprueba(unit, sec, ini, fin)
            if choca:
                ok = False
                choques.append((unit, sec, [ini, fin]))
            listos.append((unit, osec if osec == sec else "%s!=%s" % (osec, sec),
                           ini, fin, ok, len(bases) > 1,
                           sorted(items, key=lambda x: x[0])))

    listos.sort()
    ok = [x for x in listos if not x[5]]
    des = [x for x in listos if x[5]]
    print("RANGOS DEDUCIDOS DEL EMPAREJAMIENTO (sin adivinar): %d" % len(listos))
    print("  -- %d con NUESTRO ORDEN correcto (reclamables y promocionables)" % len(ok))
    for unit, sec, ini, fin, tam, _, items in ok:
        print("   %-54s %-8s 0x%08X..0x%08X %5d B  %s"
              % (unit[-54:], sec, ini, fin, fin - ini,
                 {True: "BYTES OK", False: "BYTES NO CUADRAN", None: "(sin verificar)"}[tam]))
    if des:
        print("  -- %d con el rango claro pero NUESTRO ORDEN distinto dentro de la"
              " seccion" % len(des))
        print("     (reclamables igual; promocionar exige reordenar la fuente)")
        for unit, sec, ini, fin, tam, _, items in des:
            print("   %-54s %-8s 0x%08X..0x%08X %5d B  %s"
                  % (unit[-54:], sec, ini, fin, fin - ini,
                     {True: "BYTES OK", False: "BYTES NO CUADRAN",
                      None: "(sin verificar)"}[tam]))
            print("        objetivo: %s" % ", ".join(
                "%s@0x%08X" % (n, a) for a, z, o, n in items[:5]))
            print("        nuestro : %s" % ", ".join(
                "%s@+%d" % (n, o) for a, z, o, n in sorted(items, key=lambda x: x[2])[:5]))
    if choques:
        print()
        print("CONTRADICTORIOS (dos bases para la misma seccion): %d" % len(choques))
        for unit, sec, bases in choques[:10]:
            print("   %-56s %-8s %s" % (unit[-56:], sec,
                                        ["0x%08X" % b for b in bases[:4]]))
    if sin:
        print()
        print("SIN PAREJAS $LC/lbl (no se puede deducir asi): %d" % len(sin))
        for unit, why in sin[:10]:
            print("   %-62s %s" % (unit[-62:], why))
    if yaml and listos:
        print()
        print("# lineas para config/GOWE69/splits.txt (VENTANA: exige re-extraer)")
        for unit, sec, ini, fin, tam, mal, items in listos:
            avisos = []
            if mal:
                avisos.append("ORDEN nuestro distinto: reordenar la fuente")
            if "!=" in sec:
                avisos.append("SECCION: el objetivo la pone en %s y nosotros en %s"
                              % tuple(sec.split("!=")))
            if tam is False:
                avisos.append("LOS BYTES NO CUADRAN -- NO ESCRIBIR")
            print("%s:%s" % (unit, ("   # " + "; ".join(avisos)) if avisos else ""))
            print("	%-11s start:0x%08X end:0x%08X"
                  % (sec.split("!=")[0], ini, fin))


if __name__ == "__main__":
    main()
