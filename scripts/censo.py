#!/usr/bin/env python3
"""censo.py -- cuanto CODIGO SIN ESCRIBIR queda, por unidad y ordenado por bytes.

De todas las actividades del proyecto, escribir una funcion que **no existe** es
con diferencia la mas rentable: casa a la primera compilacion la mayoria de las
veces, mientras que pulir un near-miss al 99% puede costar 200 variantes y no
mover un byte. Medido en esta sesion:

    escribir de cero  ->  PVehicle::PVehicle 1.764 B y PVehicle::Construct
                          1.588 B, las dos a la PRIMERA compilacion
                          RealShape/RealFont: 4.644 B, 34 funciones, 100%
                          zAI: 6 simbolos de `loss.py miss`, los seis al 100%
    pulir near-miss   ->  una ronda entera de 192 llamadas por +0 B

Este censo enfrenta la lista de funciones del `.s` del troceador contra la tabla
de simbolos de nuestro `.o`. Lo que el objetivo tiene y nosotros no, no esta
escrito. No usa objdiff, asi que cuesta segundos y no llena el disco.

    python scripts/censo.py                # todas las SourceLists, resumen
    python scripts/censo.py zAI            # el detalle de una unidad
    python scripts/censo.py --todo         # incluye middleware y libc
"""
import os
import re
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
ASM = os.path.join(ROOT, "build", "GOWE69", "asm")
OBJ = os.path.join(ROOT, "build", "GOWE69", "src")

RE_FN = re.compile(r"^\.fn\s+([\w$.@]+)")
RE_ENDFN = re.compile(r"^\.endfn\s+([\w$.@]+)")
RE_INSN = re.compile(r"^\s*/\*")
# nombres que el troceador invento por no saber el real: objdiff no los empareja
RE_ANON = re.compile(r"^(?:sub|func|loc)_[0-9A-Fa-f]{6,}$")


def funcs_objetivo(spath):
    """{nombre: tamano aproximado en bytes} del .s del troceador."""
    out, cur, n = {}, None, 0
    with open(spath, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            m = RE_FN.match(line)
            if m:
                cur, n = m.group(1), 0
                continue
            if cur and RE_INSN.match(line):
                n += 4
                continue
            m = RE_ENDFN.match(line)
            if m and cur:
                out[cur] = n
                cur = None
    return out


def simbolos_nuestros(opath):
    """Nombres de funcion DEFINIDAS en nuestro .o."""
    try:
        d = open(opath, "rb").read()
    except IOError:
        return None
    if d[:4] != b"\x7fELF":
        return None
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    sts = [x for x in secs if x[1] == 2]
    if not sts:
        return set()
    st = sts[0]
    strt = secs[st[6]]
    out = set()
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + "IIIBBH", d[o:o + 16])
        if shndx == 0:
            continue
        o2 = strt[4] + nameo
        n = d[o2:d.index(bytes([0]), o2)].decode("utf-8", "replace")
        if n:
            out.add(n)
    return out


def unidades(todo):
    sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
    out = ["Speed/Indep/SourceLists/" + f[:-4]
           for f in sorted(os.listdir(sl)) if f.endswith(".cpp")]
    if todo:
        for root, _, fs in os.walk(ASM):
            for f in fs:
                if not f.endswith(".s"):
                    continue
                rel = os.path.relpath(os.path.join(root, f), ASM).replace(os.sep, "/")[:-2]
                if "/SourceLists/" not in rel:
                    out.append(rel)
    return out


def mapeos():
    """Los renombres de objdiff que declara `configure.py`, por unidad.

    MEDIDO, y costaba 2.544 B de trabajo FANTASMA de 3.596 (el 71% del censo):
    el troceador desambigua los simbolos locales homonimos anadiendoles la
    direccion (`__static_initialization_and_destruction_0_803906C0`) y el
    nuestro sale con el nombre pelado que emite GCC. Comparando nombres a pelo,
    `gc_interface` figuraba con 2.456 B "sin escribir" cuando report.json lo da
    al 100%. Es la misma trampa del `0% que miente`, pero del lado del censo.

    Devuelve {unidad: {nombre_del_objetivo: nombre_nuestro}} y ademas
    {unidad: unidad_cuyo_.o_hay_que_leer} para las unidades que el objetivo
    trocea aparte pero salen de la misma compilacion (`asd2`).
    """
    import ast
    src = open(os.path.join(ROOT, "configure.py"), encoding="utf-8").read()
    mapa, comparte = {}, {}
    for nodo in ast.parse(src).body:
        tg = None
        if isinstance(nodo, ast.AnnAssign) and isinstance(nodo.target, ast.Name):
            tg, val = nodo.target.id, nodo.value
        elif isinstance(nodo, ast.Assign) and len(nodo.targets) == 1                 and isinstance(nodo.targets[0], ast.Name):
            tg, val = nodo.targets[0].id, nodo.value
        if tg not in ("OBJDIFF_SYMBOL_MAPPINGS", "OBJDIFF_SHARED_SOURCES",
                      "OBJDIFF_SHARED_SOURCE_MAPPINGS") or val is None:
            continue
        try:
            d = ast.literal_eval(val)
        except ValueError:
            continue
        for k, v in d.items():
            k = k[5:] if k.startswith("main/") else k
            if tg == "OBJDIFF_SHARED_SOURCES":
                comparte[k] = v[5:] if v.startswith("main/") else v
            else:
                mapa.setdefault(k, {}).update(v)
    return mapa, comparte


MAPA, COMPARTE = None, None


def censa(rel):
    global MAPA, COMPARTE
    if MAPA is None:
        try:
            MAPA, COMPARTE = mapeos()
        except Exception:
            MAPA, COMPARTE = {}, {}
    spath = os.path.join(ASM, rel.replace("/", os.sep) + ".s")
    # una unidad troceada aparte puede salir de la compilacion de otra
    opath = os.path.join(OBJ, COMPARTE.get(rel, rel).replace("/", os.sep) + ".o")
    ren = MAPA.get(rel, {})
    if not os.path.exists(spath):
        return None
    tgt = funcs_objetivo(spath)
    # MEDIDO: objdiff empareja por NOMBRE, asi que una funcion que el troceador
    # no supo nombrar (`fn_80310750`, `gap_`, `pad_`) **no puede puntuar por bien
    # que se escriba**. Contarlas inflaba el censo: en `metrotrk` son 100 de 107
    # entradas, y los 12.136 B que yo daba como trabajo son en realidad 596.
    tgt = {k: v for k, v in tgt.items()
           if not (k.startswith(("fn_", "gap_", "pad_", "lbl_"))
                   or RE_ANON.match(k))}
    nuestros = simbolos_nuestros(opath)
    if nuestros is None:
        # sin objeto: TODO esta sin escribir
        return [(v, k) for k, v in tgt.items()], sum(tgt.values())
    falta = [(v, k) for k, v in tgt.items() if ren.get(k, k) not in nuestros]
    falta.sort(reverse=True)
    return falta, sum(v for v, _ in falta)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    todo = "--todo" in sys.argv
    if args:
        for rel in args:
            if "/" not in rel:
                # un nombre pelado puede ser una SourceList O una unidad de
                # biblioteca (`qsort`, `vfprintf`, `metrotrk`...). Antes solo se
                # buscaba en SourceLists y el script decia "sin .s" sin mas.
                cand = "Speed/Indep/SourceLists/" + rel
                if os.path.exists(os.path.join(ASM, cand.replace("/", os.sep) + ".s")):
                    rel = cand
                else:
                    hallado = None
                    for root, _, fs in os.walk(ASM):
                        if rel + ".s" in fs:
                            hallado = os.path.relpath(
                                os.path.join(root, rel + ".s"), ASM
                            ).replace(os.sep, "/")[:-2]
                            break
                    if hallado is None:
                        print("no encuentro ninguna unidad llamada %s" % rel)
                        continue
                    rel = hallado
            r = censa(rel)
            if not r:
                print("sin .s para", rel)
                continue
            falta, total = r
            print("=== %s: %d funciones sin escribir, %d B" % (rel.split("/")[-1], len(falta), total))
            for v, k in falta[:30]:
                print("   %7d B  %s" % (v, k[:96]))
            if len(falta) > 30:
                print("   ... y %d mas" % (len(falta) - 30))
        return

    filas = []
    for rel in unidades(todo):
        r = censa(rel)
        if r and r[1]:
            filas.append((r[1], len(r[0]), rel.split("/")[-1]))
    filas.sort(reverse=True)
    print("%-24s %10s %8s" % ("unidad", "B sin escribir", "funciones"))
    for b, n, u in filas[:30]:
        print("%-24s %10d %8d" % (u, b, n))
    print()
    print("TOTAL sin escribir: %d B en %d funciones, %d unidades"
          % (sum(f[0] for f in filas), sum(f[1] for f in filas), len(filas)))


if __name__ == "__main__":
    main()
