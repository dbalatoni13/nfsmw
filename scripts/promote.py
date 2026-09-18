#!/usr/bin/env python3
"""promote.py -- comprobacion previa a marcar una unidad como `Matching`.

`matched_code` mide instrucciones; `complete_code` (lo que la gente llama
"linked") solo cuenta las unidades marcadas `Matching` en `configure.py`, que
son las que el enlazador toma de NUESTRO objeto en vez del extraido.

Una unidad puede casar al 100% y aun asi no poder promocionarse. Los modos de
fallo, por orden de frecuencia:

  1. **Conjunto de secciones distinto.** Nuestro objeto emite `.rodata` o
     `.sdata` que el extraido no tiene (o al reves). El enlazador coloca lo que
     sobra y el DOL se desplaza.
  2. **Simbolos que no cuadran.** Nos falta un simbolo global que otro objeto
     importa, o exportamos uno de mas que colisiona.
  3. **Reubicaciones a simbolos con sufijo de direccion** (`gcc2_compiled._80370048`).
     El troceador los inventa al partir un objeto; solo resuelven si el objeto
     hermano sigue siendo el extraido. Por eso hay que promocionar **el racimo
     entero o ninguno**.

Esto lo comprueba LEYENDO los dos ELF, sin construir nada -- importante cuando
hay agentes compilando en el mismo arbol.

    python scripts/promote.py                 # ranking de candidatas
    python scripts/promote.py -v              # con el motivo de cada rechazo
    python scripts/promote.py <unidad>        # el detalle de una
"""
import json
import os
import pickle
import re
import struct
import sys
from collections import defaultdict

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
OBJ = os.path.join(ROOT, "build", "GOWE69", "obj")
SRC = os.path.join(ROOT, "build", "GOWE69", "src")
REPORT = os.path.join(ROOT, "build", "GOWE69", "report.json")

RE_ADDRSUF = re.compile(r"_8[0-9a-fA-F]{7}$")


def elf(path):
    """(secciones {nombre: tam}, globales definidos, undefs) de un ELF PPC."""
    d = open(path, "rb").read()
    if d[:4] != b"\x7fELF":
        return None
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    shstrndx, = struct.unpack(e + "H", d[50:52])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    shstr = secs[shstrndx]

    def sname(off):
        b = d[shstr[4] + off:]
        return b.split(b"\0")[0].decode("utf-8", "replace")

    sections = {}
    for s in secs:
        n = sname(s[0])
        if s[1] in (1, 8) and n and not n.startswith((".debug", ".comment",
                                                      ".line", ".stab")):
            sections[n] = s[5]          # SHT_PROGBITS / SHT_NOBITS

    st = [x for x in secs if x[1] == 2]
    if not st:
        return sections, set(), set(), {}, set()
    st = st[0]
    strt = secs[st[6]]

    def nm(off):
        b = d[strt[4] + off:]
        return b.split(b"\0")[0].decode("utf-8", "replace")

    defined, undef, sitios, locales = set(), set(), {}, set()
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + "IIIBBH", d[o:o + 16])
        name = nm(nameo)
        if not name:
            continue
        bind = info >> 4
        if shndx == 0:
            if bind != 0:               # no LOCAL
                undef.add(name)
            continue
        if bind in (1, 2):              # GLOBAL / WEAK
            defined.add(name)
        else:
            locales.add(name)
        # el SITIO del simbolo dentro de su seccion, para TODOS: uno que se mueve
        # repatcha las reubicaciones de OTROS objetos que lo referencian.
        if shndx < len(secs):
            sitios[name] = (sname(secs[shndx][0]), value)
    return sections, defined, undef, sitios, locales


# MEDIDO en el enlace real: estas cinco pasan la comprobacion previa (mismas
# secciones, mismos globales) y AUN ASI rompen el enlace, porque referencian
# simbolos que ningun objeto fuente define. `_sn_iobf` y `_sn_stat_g` viven en
# `.bss` del DOL sin duena; las otras llaman a funciones que solo existen en el
# objeto extraido. La comprobacion previa mira los simbolos DEFINIDOS; esto es lo
# contrario --los UNDEF-- y solo se ve enlazando. Promocionar una de estas
# convierte el `main.dol: OK` en `L0039: Reference to undefined symbol`.
_REF_CACHE = None


def _referenciados():
    """Todos los simbolos UNDEF del juego: los unicos que pueden romper el enlace."""
    global _REF_CACHE
    if _REF_CACHE is not None:
        return _REF_CACHE
    _quien_referencia("")          # fuerza (y cachea) el indice
    _REF_CACHE = set(_QUIEN)
    return _REF_CACHE


_QUIEN = None

# El indice de UNDEF cuesta parsear los 619 objetos extraidos, y hasta la r48 se
# pagaba ENTERO en cada invocacion: barrer 28 unidades de una en una eran 28
# barridos del arbol. Se cachea en disco, invalidado por el numero de `.o` de
# `obj/` y su mtime mas nuevo --que solo cambian al re-extraer--. Si algo falla,
# se recalcula sin avisar.
_CACHE = os.path.join(ROOT, "scratchpad", ".promote_undef.pkl")


def _sello():
    n = t = 0
    for root, _, fs in os.walk(OBJ):
        for f in fs:
            if f.endswith(".o"):
                n += 1
                t = max(t, os.path.getmtime(os.path.join(root, f)))
    return (n, int(t))


def _objetos_del_enlace():
    """Los `.o` que entran de verdad en main.elf, leidos de build.ninja.

    OJO: `os.walk(OBJ)` cuenta objetos RANCIOS que ya no estan en el enlace, y
    eso da falsos positivos en «no definimos N simbolo(s) que el extraido
    exporta». Medido en la r49: `auto_01_8037A2B8_text`, `auto_01_8031E868_text`
    y `auto_01_8031EC3C_text` no estan en `build.ninja`, y por ellos TRES de las
    cuatro unidades que el censo de la r48 puso en esa familia estaban mal
    diagnosticadas.
    """
    ninja = os.path.join(ROOT, "build.ninja")
    if not os.path.exists(ninja):
        return None
    L = open(ninja, encoding="utf-8", errors="replace").read().split(chr(10))
    i = [k for k, x in enumerate(L) if "main.elf" in x and x.startswith("build ")]
    if not i:
        return None
    k = i[0]
    tr = []
    while True:
        tr.append(L[k])
        if not L[k].rstrip().endswith("$"):
            break
        k += 1
    txt = " ".join(x.rstrip("$").strip() for x in tr)
    vivos = set()
    for t in txt.split():
        if t.endswith(".o"):
            vivos.add(os.path.normcase(os.path.abspath(
                os.path.join(ROOT, t.replace(chr(92), os.sep)))))
    return vivos


def _construye_quien():
    q = {}
    vivos = _objetos_del_enlace()
    for root, _, fs in os.walk(OBJ):
        for f in fs:
            if not f.endswith(".o"):
                continue
            p = os.path.join(root, f)
            if vivos is not None and os.path.normcase(os.path.abspath(p)) not in vivos:
                continue
            r = elf(p)
            if not r:
                continue
            rel = os.path.relpath(p, OBJ).replace(os.sep, "/")[:-2]
            for u in r[2]:
                q.setdefault(u, set()).add(rel)
    return q


def _quien_referencia(sym):
    """Objetos que tienen ese simbolo UNDEF. Se construye una sola vez."""
    global _QUIEN
    if _QUIEN is None:
        sello = _sello()
        try:
            with open(_CACHE, "rb") as fh:
                g = pickle.load(fh)
            if g.get("sello") == sello:
                _QUIEN = g["quien"]
        except Exception:
            pass
        if _QUIEN is None:
            _QUIEN = _construye_quien()
            try:
                os.makedirs(os.path.dirname(_CACHE), exist_ok=True)
                with open(_CACHE, "wb") as fh:
                    pickle.dump({"sello": sello, "quien": _QUIEN}, fh)
            except Exception:
                pass
    return _QUIEN.get(sym, set())


VETADAS = {
    "libc/fopen",                                                # _sn_iobf, _sn_stat_g
    "libc/sn_buf",                                               # idem
    "cluttype",                                                  # _9RealShape
    "Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/hlsfile",  # FILESYS_atomic
    "Speed/Indep/Libs/spch/dev/source/library/cmn/spchcsis",      # iSPCH_*
}


def revisa(unit):
    if unit in VETADAS:
        return None, ["VETADA: rompe el enlace (referencia un simbolo que ningun "
                      "objeto fuente define)"]
    a = os.path.join(OBJ, unit.replace("/", os.sep) + ".o")
    b = os.path.join(SRC, unit.replace("/", os.sep) + ".o")
    if not (os.path.exists(a) and os.path.exists(b)):
        return None, ["sin objeto construido"]
    ea, eb = elf(a), elf(b)
    if not ea or not eb:
        return None, ["no es ELF"]
    sa, da, ua, pa, la = ea
    sb, db, ub, pb, lb = eb
    motivos = []
    # una seccion de tamano CERO es solo una cabecera; GCC las emite siempre y el
    # enlazador no coloca nada por ellas. Solo cuentan las que llevan bytes.
    sa = {k: v for k, v in sa.items() if v}
    sb = {k: v for k, v in sb.items() if v}
    solo_a = {k: v for k, v in sa.items() if k not in sb}
    solo_b = {k: v for k, v in sb.items() if k not in sa}
    if solo_a:
        motivos.append("secciones que el extraido tiene y nosotros no: "
                       + ", ".join("%s(%dB)" % (k, v) for k, v in sorted(solo_a.items())))
    if solo_b:
        motivos.append("secciones que emitimos de mas: "
                       + ", ".join("%s(%dB)" % (k, v) for k, v in sorted(solo_b.items())))
    for k in sorted(set(sa) & set(sb)):
        if sa[k] != sb[k]:
            motivos.append("%s mide %d B y el extraido %d B" % (k, sb[k], sa[k]))
    falta = da - db
    # MEDIDO: un simbolo que el extraido exporta y nosotros no SOLO bloquea
    # el enlace si alguien lo REFERENCIA. De 93 que rechazaba esto en 27
    # unidades, solo 11 estaban referenciados: los otros 82 son datos que el
    # troceador nombro por su direccion y que no usa nadie.
    falta = {x for x in falta if x in _referenciados()}
    sobra = db - da
    if falta:
        # Decir QUIEN lo referencia convierte «bloqueada» en accionable: si todos
        # los que lo usan son objetos comodin del troceador (`auto_*`) o unidades
        # VETADAS --que nunca se promocionan y por tanto siempre salen del lado
        # extraido--, basta un ALIAS de ensamblador en nuestra fuente con el
        # nombre que inventa el troceador. MEDIDO: los bloqueos por
        # `gcc2_compiled._ADDR` y por los `lbl_XXXXXXXX` de datos ya reclamados
        # son exactamente ese caso.
        # MEDIDO: buena parte de estos NO es «no lo definimos», sino que lo
        # definimos con enlace INTERNO (`static`) donde el objetivo lo exporta.
        # Con el simbolo LOCAL el enlace falla igual, y la cura es quitar el
        # `static` (el nombre mangled lo fija el `__asm__`). Un caso asi dejo
        # `snd/srandom` LIMPIA con una palabra.
        solo_local = sorted(x for x in falta if x in lb)
        if solo_local:
            motivos.append("%d simbolo(s) definidos LOCAL que el objetivo exporta GLOBAL (quita el `static`): %s"
                           % (len(solo_local), ", ".join(solo_local[:4])))
            falta = falta - set(solo_local)
        if not falta:
            return (ub, sorted(x for x in ub if RE_ADDRSUF.search(x))), motivos
        detalle = []
        for x in sorted(falta)[:4]:
            usan = sorted(_quien_referencia(x))[:3]
            detalle.append("%s <- %s" % (x, ", ".join(usan) or "?"))
        motivos.append("no definimos %d simbolo(s) que el extraido exporta: %s"
                       % (len(falta), "; ".join(detalle)))
    if sobra:
        motivos.append("exportamos %d simbolo(s) de mas: %s"
                       % (len(sobra), ", ".join(sorted(sobra)[:4])))
    # MEDIDO, y es lo que rompio el enlace con `systemvars`: el extraido tenia un
    # hueco de 8 B (`gap_08_804FF650_sdata`) delante de dos punteros de `.sdata` y
    # nosotros no. Las secciones median igual y los simbolos eran los mismos, asi
    # que la comprobacion pasaba -- pero `base.cpp` y `fontcreate.cpp`, que salen
    # del objeto EXTRAIDO, los alcanzan por reubicacion, y el enlazador les
    # parcheo `0x90` donde el original pone `0x98`. Cinco bytes en todo el DOL.
    # Un simbolo que se mueve dentro de su seccion repatcha a QUIEN LO REFERENCIA.
    movidos = []
    for k in sorted(set(pa) & set(pb)):
        if k.startswith(("gap_", "pad_", "gcc2_compiled")):
            continue
        if pa[k] != pb[k]:
            movidos.append("%s: %s+%d -> %s+%d" % (k[:38], pa[k][0], pa[k][1],
                                                   pb[k][0], pb[k][1]))
    if movidos:
        motivos.append("%d simbolo(s) en OTRO SITIO de su seccion (repatchan a "
                       "quien los referencia): %s"
                       % (len(movidos), "; ".join(movidos[:3])))

    suf = sorted(x for x in ub if RE_ADDRSUF.search(x))
    if suf:
        motivos.append("%d reubicacion(es) a simbolo con sufijo de direccion "
                       "(racimo): %s" % (len(suf), ", ".join(suf[:3])))
    return (ub, suf), motivos


def candidatas():
    d = json.load(open(REPORT))

    def num(x):
        try:
            return int(x)
        except (TypeError, ValueError):
            return 0
    out = []
    for u in d["units"]:
        m = u.get("measures") or {}
        if m.get("complete_units"):
            continue
        tc, mc = num(m.get("total_code")), num(m.get("matched_code"))
        td, md = num(m.get("total_data")), num(m.get("matched_data"))
        if (tc or td) and tc == mc and td == md:
            name = u["name"]
            out.append((tc, td, name[5:] if name.startswith("main/") else name))
    out.sort(key=lambda x: -x[0])
    return out


def funciones_abiertas(unit):
    """-> (cuantas, bytes) funciones de esa unidad que NO casan, segun report.json.

    `revisa()` solo mira SECCIONES Y DATOS. Sin este contraste, una unidad con el
    dato perfecto y una funcion sin cerrar salia como
    "LIMPIA: se puede marcar Matching" -- y marcarla produce medio DOL distinto
    con el informe diciendo que esta bien, que es la trampa que tiene memoria
    propia en el proyecto (`nfsmw-promocion-sin-su-cierre`).

    Caso que lo destapo (r55): `criticalpath` daba LIMPIA con
    `VP6_PredictFilteredBlock` a 37 instrucciones. `trypromo` decia DOL ROTO.
    """
    try:
        d = json.load(open(REPORT))
    except Exception:
        return None, None
    clave = unit.replace("\\", "/").rstrip("/")
    for u in d.get("units", []):
        n = (u.get("name") or "").replace("main/", "")
        if n == clave or n.endswith("/" + clave) or os.path.basename(n) == os.path.basename(clave):
            ab = [f for f in (u.get("functions") or [])
                  if float(f.get("fuzzy_match_percent", 0) or 0) < 100.0]
            return len(ab), sum(int(f.get("size", 0) or 0) for f in ab)
    return None, None


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    verbose = "-v" in sys.argv
    if args:
        for unit in args:
            info, motivos = revisa(unit)
            print("=== %s" % unit)
            nfn, nb = funciones_abiertas(unit)
            if not motivos and nfn:
                print("    LIMPIA DE DATOS, pero NO se puede marcar Matching:")
                print("    le faltan %d funcion(es), %d B. Pasa `trypromo` antes." % (nfn, nb))
            elif not motivos and nfn is None:
                print("    LIMPIA de datos. No he podido leer report.json para")
                print("    comprobar las funciones: pasa `trypromo` antes de marcar.")
            elif not motivos:
                print("    LIMPIA: se puede marcar Matching")
            for m in motivos:
                print("    - %s" % m)
        return

    escribir = "--write" in sys.argv
    cands = candidatas()
    limpias, sucias = [], []
    for tc, td, unit in cands:
        info, motivos = revisa(unit)
        (limpias if not motivos else sucias).append((tc, td, unit, motivos))
    print("candidatas (100%% en codigo y datos, sin promocionar): %d" % len(cands))
    print()
    print("LIMPIAS: %d unidades, %d B de codigo, %d B de datos"
          % (len(limpias), sum(x[0] for x in limpias), sum(x[1] for x in limpias)))
    for tc, td, unit, _ in limpias[:25]:
        print("   %8d B  %s" % (tc, unit))
    if len(limpias) > 25:
        print("   ... y %d mas" % (len(limpias) - 25))
    print()
    if escribir:
        cfgp = os.path.join(ROOT, "configure.py")
        cfg = open(cfgp, encoding="utf-8", newline="").read()
        open(cfgp + ".bak_promote", "w", encoding="utf-8", newline="").write(cfg)
        hechas, fallidas = 0, []
        for tc, td, unit, _ in limpias:
            puesto = False
            for ext in (".cpp", ".c"):
                una = 'Object(NonMatching, "%s%s")' % (unit, ext)
                if una in cfg:
                    cfg = cfg.replace(una, una.replace("NonMatching", "Matching"), 1)
                    puesto = True
                    break
                marca = '"%s%s"' % (unit, ext)
                i = cfg.find(marca)
                if i != -1:
                    j = cfg.rfind("NonMatching,", max(0, i - 300), i)
                    if j != -1:
                        cfg = cfg[:j] + "Matching," + cfg[j + len("NonMatching,"):]
                        puesto = True
                        break
            if puesto:
                hechas += 1
            else:
                fallidas.append(unit)
        open(cfgp, "w", encoding="utf-8", newline="").write(cfg)
        print()
        print("promocionadas %d de %d   (copia en configure.py.bak_promote)"
              % (hechas, len(limpias)))
        if fallidas:
            print("NO encontradas en configure.py: %d" % len(fallidas))
            for f in fallidas[:10]:
                print("   %s" % f)
        print()
        print("ahora:  python configure.py && python -m ninja")
        print("y VERIFICA que dice 'main.dol: OK'.")
        print("si falla: cp configure.py.bak_promote configure.py")
        return

    grupos = defaultdict(list)
    for tc, td, unit, motivos in sucias:
        grupos[motivos[0].split(":")[0]].append((tc, unit, motivos))
    print("CON PEGAS: %d unidades, %d B retenidos" % (len(sucias), sum(x[0] for x in sucias)))
    for k in sorted(grupos, key=lambda g: -sum(x[0] for x in grupos[g])):
        v = grupos[k]
        print("   %6d B  %3d unidades  %s" % (sum(x[0] for x in v), len(v), k))
        if verbose:
            for tc, unit, motivos in sorted(v, reverse=True)[:6]:
                print("        %7d B  %s" % (tc, unit))
                for m in motivos:
                    print("                 %s" % m)


if __name__ == "__main__":
    main()
