#!/usr/bin/env python3
"""claimdata.py -- reclama para su unidad los datos que el troceador dejo sueltos.

Casi ninguna unidad de middleware puede promocionarse a `Matching` aunque case al
100%, y el motivo casi siempre es el mismo: **`config/GOWE69/splits.txt` solo le
declara `.text`**. Su `.rodata` existe en el DOL, pero sin duena, asi que
decomp-toolkit lo mete en los objetos comodin `auto_*_rodata.o`. Al enlazar,
nuestro objeto aporta un `.rodata` que el extraido no tenia y el DOL se desplaza.

La direccion se puede averiguar **sin adivinar**: nuestros bytes de `.rodata` ya
son correctos (la unidad casa al 100%), asi que basta buscarlos dentro del monton
sin reclamar. Si aparecen **una sola vez**, esa es la direccion, y la linea que
falta en `splits.txt` se escribe sola.


AVISO DOBLE, aprendido rompiendo el troceado:

1. **Reclamar un rango NO basta: hay que RETROCEAR.** La regla `split` de ninja
   depende de `config/GOWE69/config.yml`, **no de `splits.txt`**, asi que editar
   los rangos no reextrae nada y el cambio no tiene ningun efecto -- ni bueno ni
   malo, y el `main.dol: OK` que salga **no los ha validado**. Hay que borrar
   `build/GOWE69/config.json` para forzarlo.
2. **El rango tiene que terminar en un LIMITE DE SIMBOLO.** De 16 rangos escritos
   a ojo por coincidencia de bytes, **cinco terminaban dentro de un simbolo** y el
   troceador aborta con «ends within symbol». Se detectan solo al retrocear.
3. Y aunque se reclame bien, **eso NO desbloquea la promocion por si solo**: el
   objeto extraido pasa a exportar un `lbl_XXXXXXXX` por cada dato y el nuestro
   tiene `$LC` anonimos, asi que `promote.py` lo rechaza por simbolos que no
   definimos. Reclamar el dato es condicion necesaria, no suficiente.

    python scripts/claimdata.py                 # que se puede reclamar
    python scripts/claimdata.py --write         # lo escribe en splits.txt
    python scripts/claimdata.py --seccion .data # otra seccion

Tras `--write` hay que reconfigurar y reconstruir, y **verificar `main.dol: OK`**.
Guarda copia: `splits.txt.bak`.
"""
import glob
import json
import os
import re
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
OBJ = os.path.join(ROOT, "build", "GOWE69", "obj")
SRC = os.path.join(ROOT, "build", "GOWE69", "src")
SPLITS = os.path.join(ROOT, "config", "GOWE69", "splits.txt")
REPORT = os.path.join(ROOT, "build", "GOWE69", "report.json")


def secbytes(path, want):
    try:
        d = open(path, "rb").read()
    except IOError:
        return None
    if d[:4] != b"\x7fELF":
        return None
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    sx, = struct.unpack(e + "H", d[50:52])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    st = secs[sx]

    def nm(o):
        b = d[st[4] + o:]
        return b.split(b"\0")[0].decode("utf-8", "replace")

    for s in secs:
        if nm(s[0]) == want and s[1] == 1 and s[5]:
            return d[s[4]:s[4] + s[5]]
    return None


def monton(seccion):
    """Los objetos comodin de esa seccion, con su direccion base."""
    tipo = seccion.lstrip(".")
    out = []
    for p in sorted(glob.glob(os.path.join(OBJ, "auto_*_%s.o" % tipo))):
        m = re.search(r"_(8[0-9A-Fa-f]{7})_", os.path.basename(p))
        if not m:
            continue
        b = secbytes(p, seccion)
        if b:
            out.append((int(m.group(1), 16), b, os.path.basename(p)))
    return out


ASM = os.path.join(ROOT, "build", "GOWE69", "asm")
RE_LBL = re.compile(r"lbl_(8[0-9A-Fa-f]{7})")


def por_etiquetas(unit, seccion):
    """La base de la seccion, deducida de las etiquetas del troceador.

    Es el metodo EXACTO y no depende de que los bytes sean unicos: el `.s` del
    troceador nombra por su direccion cada dato que el original referencia
    (`lbl_804147C8`), y nuestro `.o` trae el desplazamiento de esa misma
    referencia dentro de la seccion. **La diferencia es la base.** Con varias
    referencias se vota: la base correcta explica el maximo de etiquetas.
    """
    spath = os.path.join(ASM, unit.replace("/", os.sep) + ".s")
    opath = os.path.join(SRC, unit.replace("/", os.sep) + ".o")
    if not (os.path.exists(spath) and os.path.exists(opath)):
        return None, 0
    labs = set(int(x, 16) for x in
               RE_LBL.findall(open(spath, encoding="utf-8", errors="replace").read()))
    if not labs:
        return None, 0
    d = open(opath, "rb").read()
    if d[:4] != b"ELF":
        return None, 0
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    sx, = struct.unpack(e + "H", d[50:52])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    shstr = secs[sx]

    def sn(o):
        o2 = shstr[4] + o
        return d[o2:d.index(bytes([0]), o2)].decode("utf-8", "replace")

    names = [sn(x[0]) for x in secs]
    sts = [i for i, x in enumerate(secs) if x[1] == 2]
    if not sts:
        return None, 0
    st = secs[sts[0]]
    strt = secs[st[6]]

    def nm(o):
        o2 = strt[4] + o
        return d[o2:d.index(bytes([0]), o2)].decode("utf-8", "replace")

    syms = []
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + "IIIBBH", d[o:o + 16])
        syms.append((value, shndx))
    adds = set()
    for x in secs:
        if x[1] != 4 or not x[9]:
            continue
        for k in range(x[5] // x[9]):
            o = x[4] + k * x[9]
            off, info, a = struct.unpack(e + "IIi", d[o:o + 12])
            si = info >> 8
            if si < len(syms):
                val, shndx = syms[si]
                if shndx < len(names) and names[shndx] == seccion:
                    adds.add(val + a)
    if not adds:
        return None, 0
    votos = {}
    for L in labs:
        for a in adds:
            if L - a > 0:
                votos[L - a] = votos.get(L - a, 0) + 1
    if not votos:
        return None, 0
    mejor = max(votos.values())
    ganadoras = [b for b, v in votos.items() if v == mejor]
    if len(ganadoras) != 1 or mejor < 2:
        return None, mejor
    return ganadoras[0], mejor


def declaradas():
    """{unidad: {seccion: (ini, fin)}} tal como esta hoy en splits.txt."""
    d, cur = {}, None
    for ln in open(SPLITS, encoding="utf-8", errors="replace"):
        if ln and not ln[0].isspace() and ln.rstrip().endswith(":"):
            cur = ln.strip()[:-1]
            d[cur] = {}
            continue
        m = re.match(r"\s+(\.\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", ln)
        if m and cur:
            d[cur][m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return d


def candidatas():
    """Unidades que casan al 100% en codigo y datos y no estan promocionadas."""
    rep = json.load(open(REPORT))

    def num(x):
        try:
            return int(x)
        except (TypeError, ValueError):
            return 0
    out = []
    for u in rep["units"]:
        m = u.get("measures") or {}
        if m.get("complete_units"):
            continue
        tc, mc = num(m.get("total_code")), num(m.get("matched_code"))
        td, md = num(m.get("total_data")), num(m.get("matched_data"))
        if (tc or td) and tc == mc and td == md:
            n = u["name"]
            out.append((tc, n[5:] if n.startswith("main/") else n))
    return out


def textaddr(decl, clave):
    """Direccion de .text de esa unidad segun splits.txt, o 0."""
    return (decl.get(clave) or {}).get(".text", (0, 0))[0]


def main():
    seccion = ".rodata"
    if "--seccion" in sys.argv:
        seccion = sys.argv[sys.argv.index("--seccion") + 1]
    escribir = "--write" in sys.argv

    pool = monton(seccion)
    print("%s sin reclamar: %d objetos comodin, %d B"
          % (seccion, len(pool), sum(len(x[1]) for x in pool)))
    decl = declaradas()

    # DONDE viven hoy esos bytes, si no estan en el monton: en el .rodata de otro
    # objeto extraido, lo que significa que su rango en splits.txt es demasiado largo.
    dueno = []
    for u, ss in decl.items():
        if seccion in ss:
            raiz = u[:-4] if u.endswith((".cpp", ".c")) else u
            b = secbytes(os.path.join(OBJ, raiz.replace("/", os.sep) + ".o"), seccion)
            if b:
                dueno.append((ss[seccion][0], b, u))

    unicas, ambiguas, perdidas, ya = [], [], [], 0
    for tc, unit in candidatas():
        ours = secbytes(os.path.join(SRC, unit.replace("/", os.sep) + ".o"), seccion)
        if not ours:
            continue
        clave = unit + ".cpp"
        if clave not in decl:
            clave = unit + ".c"
        if seccion in decl.get(clave, {}):
            ya += 1
            continue
        b_lbl, votos = por_etiquetas(unit, seccion)
        if b_lbl is not None:
            unicas.append((tc, unit, clave, b_lbl, len(ours)))
            continue
        found = []
        for base, blob, nm_ in pool:
            i = blob.find(ours)
            while i != -1:
                found.append(base + i)
                i = blob.find(ours, i + 1)
        if len(found) == 1:
            unicas.append((tc, unit, clave, found[0], len(ours)))
        elif found:
            ambiguas.append((tc, unit, clave, len(ours), found))
        else:
            quien = []
            for base, blob, u in dueno:
                i = blob.find(ours)
                while i != -1:
                    quien.append((u, base + i))
                    i = blob.find(ours, i + 1)
            perdidas.append((tc, unit, len(ours), quien))

    # DESAMBIGUAR POR ORDEN DE ENLACE: dentro de una biblioteca el troceador coloca
    # el .rodata en el mismo orden que el .text. Si conozco el .text de la unidad,
    # sus vecinas YA resueltas acotan por arriba y por abajo.
    fijas = sorted((textaddr(decl, clave), a, a + n)
                   for tc, unit, clave, a, n in unicas
                   if textaddr(decl, clave))
    resueltas = 0
    aun = []
    for tc, unit, clave, n, cands in ambiguas:
        t = textaddr(decl, clave)
        if not t:
            aun.append((tc, unit, n, len(cands)))
            continue
        lo = max([r for tt, _, r in fijas if tt < t] or [0])
        hi = min([l for tt, l, _ in fijas if tt > t] or [0xFFFFFFFF])
        viables = [a for a in cands if lo <= a and a + n <= hi]
        if len(viables) == 1:
            unicas.append((tc, unit, clave, viables[0], n))
            resueltas += 1
        else:
            aun.append((tc, unit, n, len(viables) or len(cands)))
    ambiguas = aun
    if resueltas:
        print("desambiguadas por orden de enlace: %d" % resueltas)

    unicas.sort(reverse=True)
    print()
    print("RECLAMABLES SIN AMBIGUEDAD: %d unidades, %d B de codigo que desbloquean"
          % (len(unicas), sum(x[0] for x in unicas)))
    for tc, unit, clave, addr, n in unicas[:30]:
        print("   %7d B cod   %s start:0x%08X end:0x%08X (%d B)   %s"
              % (tc, seccion, addr, addr + n, n, unit))
    if len(unicas) > 30:
        print("   ... y %d mas" % (len(unicas) - 30))
    print()
    print("ambiguas (los bytes salen mas de una vez): %d, %d B"
          % (len(ambiguas), sum(x[0] for x in ambiguas)))
    for tc, unit, n, k in sorted(ambiguas, key=lambda x: -x[0])[:8]:
        print("   %7d B  %4d B de %s en %d sitios  %s" % (tc, n, seccion, k, unit))
    print("no estan en el monton: %d, %d B  (sus bytes ya se los quedo otra unidad)"
          % (len(perdidas), sum(x[0] for x in perdidas)))
    for tc, unit, n, quien in sorted(perdidas, key=lambda x: -x[0])[:12]:
        d = ("  -> hoy dentro de %s @0x%08X" % quien[0]) if len(quien) == 1 else (
            "  -> en %d sitios" % len(quien) if quien else "  -> no aparece en ningun .rodata")
        print("   %7d B  %4d B de %s   %s%s" % (tc, n, seccion, unit, d))
    if ya:
        print("ya la tenian declarada: %d" % ya)

    if not escribir:
        print()
        print("(nada escrito; --write para aplicarlo a splits.txt)")
        return

    # comprobacion de solapes ANTES de tocar nada
    ocupado = []
    for u, ss in decl.items():
        for s, (a, b) in ss.items():
            ocupado.append((a, b, u, s))
    choques = []
    for tc, unit, clave, addr, n in unicas:
        for a, b, u, s in ocupado:
            if addr < b and a < addr + n:
                choques.append((unit, u, s))
    if choques:
        print("ABORTO: %d solapes con rangos ya declarados" % len(choques))
        for x in choques[:5]:
            print("   %s pisa %s %s" % x)
        return

    txt = open(SPLITS, encoding="utf-8", errors="replace").read()
    nl = "\r\n" if "\r\n" in txt else "\n"
    n_ok = 0
    for tc, unit, clave, addr, n in unicas:
        cab = clave + ":" + nl
        if cab not in txt:
            continue
        i = txt.index(cab) + len(cab)
        j = txt.index(nl + nl, i) + len(nl)      # fin del bloque
        linea = "\t%-11s start:0x%08X end:0x%08X%s" % (seccion, addr, addr + n, nl)
        txt = txt[:j] + linea + txt[j:]
        n_ok += 1
    open(SPLITS + ".bak2", "w", encoding="utf-8", newline="").write(
        open(SPLITS, encoding="utf-8", errors="replace").read())
    open(SPLITS, "w", encoding="utf-8", newline="").write(txt)
    print()
    print("escritas %d lineas en splits.txt (copia en splits.txt.bak2)" % n_ok)
    print("ahora: python configure.py && python -m ninja   -> verifica 'main.dol: OK'")


if __name__ == "__main__":
    main()
