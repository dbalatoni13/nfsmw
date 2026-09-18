#!/usr/bin/env python3
"""callcheck.py -- el detector general de "el 100% que miente".

objdiff con `functionRelocDiffs=none` NO compara a que simbolo apunta cada
`bl`. Por eso una funcion puede medir 100% llamando al simbolo EQUIVOCADO, y
el DOL romperse igual. Ya han aparecido tres casos distintos de esto:

  * `__builtin_new` donde el objetivo llama a `__builtin_vec_new`
    (623 contra 19 en todo el juego; 115 contra 1 solo en zFe2)
  * `_IHandle__Q214EventSequencer7IEngine` donde el objetivo llama a
    `_IHandle__7IEngine` -- un miembro declarado con el namespace equivocado
  * la sobrecarga equivocada (`GetMatrix(bMatrix4 &)` contra `(bMatrix4 *)`),
    que ademas BORRA un simbolo que ya estaba al 100%

Los tres son el mismo problema y se ven de golpe comparando, por unidad, el
multiconjunto de simbolos EXTERNOS llamados:

  * en el objetivo: los `bl <simbolo>` del `.s` del troceador que no estan
    definidos en ese mismo `.s`
  * en el nuestro: las reubicaciones R_PPC_REL24 de nuestro `.o` a simbolos
    UNDEF

    python scripts/callcheck.py                 # todas las SourceLists
    python scripts/callcheck.py zFe2 zAI        # unidades sueltas
    python scripts/callcheck.py --all           # incluye middleware
    python scripts/callcheck.py zFe2 -v         # lista todas las diferencias

Sin argumentos imprime un ranking por numero de llamadas descuadradas, que es
el orden en el que conviene atacarlas.
"""
import os
import re
import struct
import sys
from collections import Counter

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
ASM = os.path.join(ROOT, "build", "GOWE69", "asm")
OBJ = os.path.join(ROOT, "build", "GOWE69", "src")

R_PPC_REL24 = 10
RE_BL = re.compile(r"^\s*/\*[^*]*\*/\s*bl\s+([A-Za-z_$.][\w$.@]*)\s*$")
RE_FN = re.compile(r"^\.fn\s+([\w$.]+)")


def target_calls(spath):
    """Simbolos llamados con `bl` en el .s, menos los definidos ahi mismo."""
    calls = Counter()
    local = set()
    with open(spath, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            m = RE_FN.match(line)
            if m:
                local.add(m.group(1))
                continue
            m = RE_BL.match(line)
            if m:
                calls[m.group(1)] += 1
    for name in local:
        calls.pop(name, None)
    # las etiquetas internas del troceador no son llamadas a nada
    for name in [k for k in calls if k.startswith(".L")]:
        calls.pop(name, None)
    return calls


def our_calls(opath):
    """Reubicaciones REL24 de nuestro .o a simbolos UNDEF."""
    d = open(opath, "rb").read()
    if d[:4] != b"\x7fELF":
        return None
    e = ">" if d[5] == 2 else "<"
    shoff, = struct.unpack(e + "I", d[32:36])
    shes, = struct.unpack(e + "H", d[46:48])
    shn, = struct.unpack(e + "H", d[48:50])
    secs = [struct.unpack(e + "10I", d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    symtabs = [s for s in secs if s[1] == 2]
    if not symtabs:
        return None
    st = symtabs[0]
    strt = secs[st[6]]

    def sname(off):
        s = d[strt[4] + off:]
        return s[: s.index(b"\0")].decode("utf-8", "replace")

    syms = []
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + "IIIBBH", d[o:o + 16])
        syms.append((sname(nameo), shndx))

    calls = Counter()
    for s in secs:
        if s[1] != 4 or not s[9]:      # SHT_RELA con entsize
            continue
        for i in range(s[5] // s[9]):
            o = s[4] + i * s[9]
            off, info, add = struct.unpack(e + "IIi", d[o:o + 12])
            if (info & 0xFF) != R_PPC_REL24:
                continue
            si = info >> 8
            if si < len(syms) and syms[si][1] == 0:   # SHN_UNDEF
                calls[syms[si][0]] += 1
    return calls



def _dueno(porsec, sec, off):
    """Que funcion contiene ese desplazamiento, por biseccion."""
    import bisect
    v = porsec.get(sec)
    if not v:
        return None
    i = bisect.bisect_right(v, (off, float("inf"), "")) - 1
    if i >= 0 and v[i][0] <= off < v[i][1]:
        return v[i][2]
    return None


def where(unit, sym):
    """En que FUNCIONES llama cada lado a `sym`. Localiza el sitio a arreglar."""
    spath, opath = unit_paths(unit)
    # objetivo: el .s del troceador nombra la funcion que contiene cada bl
    cur, tgt = None, {}
    with open(spath, encoding='utf-8', errors='replace') as fh:
        for line in fh:
            m = RE_FN.match(line)
            if m:
                cur = m.group(1)
                continue
            m = RE_BL.match(line)
            if m and m.group(1) == sym and cur:
                tgt[cur] = tgt.get(cur, 0) + 1
    # nuestro: mapear el offset de cada reubicacion a la funcion que lo contiene
    d = open(opath, 'rb').read()
    e = '>' if d[5] == 2 else '<'
    shoff, = struct.unpack(e + 'I', d[32:36])
    shes, = struct.unpack(e + 'H', d[46:48])
    shn, = struct.unpack(e + 'H', d[48:50])
    secs = [struct.unpack(e + '10I', d[shoff + i * shes: shoff + i * shes + 40])
            for i in range(shn)]
    st = [x for x in secs if x[1] == 2][0]
    strt = secs[st[6]]

    def nm(off):
        b = d[strt[4] + off:]
        return b.split(bytes([0]))[0].decode("utf-8", "replace")

    syms, funcs = [], []
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + 'IIIBBH', d[o:o + 16])
        syms.append((nm(nameo), shndx))
        if (info & 0xF) == 2:      # STT_FUNC
            funcs.append((value, value + size, nm(nameo), shndx))
    # `where` se llamaba una vez por simbolo y hacia un barrido LINEAL de las
    # funciones por cada reubicacion: O(n*m), mas de 2 min por simbolo en una
    # unidad grande. Con el indice ordenado por seccion y biseccion es inmediato,
    # y por eso `--where <unidad>` sin simbolo puede hacerlos TODOS de una pasada.
    porsec = {}
    for lo, hi, fname, fsec in funcs:
        porsec.setdefault(fsec, []).append((lo, hi, fname))
    for k in porsec:
        porsec[k].sort()

    ours = {}
    for sec in secs:
        if sec[1] != 4 or not sec[9]:
            continue
        target_sec = sec[7]
        for i in range(sec[5] // sec[9]):
            o = sec[4] + i * sec[9]
            off, info, add_ = struct.unpack(e + 'IIi', d[o:o + 12])
            if (info & 0xFF) != R_PPC_REL24:
                continue
            si = info >> 8
            if si >= len(syms) or syms[si][0] != sym:
                continue
            f = _dueno(porsec, target_sec, off)
            if f:
                ours[f] = ours.get(f, 0) + 1
    print('--- %s en %s ---' % (sym, unit))
    print('%-6s %-6s  funcion' % ('obj', 'nuestro'))
    for f in sorted(set(tgt) | set(ours), key=lambda x: -(ours.get(x, 0))):
        a, b = tgt.get(f, 0), ours.get(f, 0)
        mark = '  <<<' if a != b else ''
        print('%-6d %-6d  %s%s' % (a, b, f[:64], mark))

def unit_paths(name):
    if "/" in name:
        rel = name
    else:
        rel = "Speed/Indep/SourceLists/" + name
    return (os.path.join(ASM, rel.replace("/", os.sep) + ".s"),
            os.path.join(OBJ, rel.replace("/", os.sep) + ".o"))


def units_from(args):
    if args:
        return args
    out = []
    sl = os.path.join(ROOT, "src", "Speed", "Indep", "SourceLists")
    for f in sorted(os.listdir(sl)):
        if f.endswith(".cpp"):
            out.append(f[:-4])
    return out


# ---------------------------------------------------------------------------
# --real : separa el descuadre REAL de los tres artefactos de medida.
#
# callcheck compara nuestro .o (pre-enlace) contra el .s del troceador, que sale
# del DOL (post-enlace). El enlazador SN hace tres cosas que el .o todavia no:
#   1. deduplica los simbolos WEAK y se queda con UNA copia, en UN objeto;
#   2. por eso los demas objetos del original los ven como externos y nosotros
#      no (los definimos en casa) -> ART_OBJ;
#   3. tira las funciones muertas, asi que las llamadas que salen de ellas no
#      pueden aparecer en el .s -> ART_STRIP.
# Y nuestras copias weak duplicadas emiten llamadas de mas -> ART_DUP.
# Medido: de 2.460 descuadres, 893 + 853 + 372 son artefacto y 338 son reales.
# ---------------------------------------------------------------------------

def _elf_syms():
    import struct as _s
    p = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
    d = open(p, 'rb').read()
    e = '>'
    shoff, = _s.unpack(e + 'I', d[32:36]); shes, = _s.unpack(e + 'H', d[46:48]); shn, = _s.unpack(e + 'H', d[48:50])
    secs = [_s.unpack(e + '10I', d[shoff + i * shes: shoff + i * shes + 40]) for i in range(shn)]
    st = [x for x in secs if x[1] == 2][0]; strt = secs[st[6]]
    sd = d[strt[4]:strt[4] + strt[5]]
    out = set()
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = _s.unpack(e + 'IIIBBH', d[o:o + 16])
        if shndx:
            out.add(sd[nameo:sd.index(b'\0', nameo)].decode('utf-8', 'replace'))
    return out


def _target_local(spath):
    local = set()
    with open(spath, encoding='utf-8', errors='replace') as fh:
        for line in fh:
            m = RE_FN.match(line)
            if m:
                local.add(m.group(1))
    return local


def _our_detail(opath, tlocal, elf):
    """Devuelve (llamadas UNDEF, simbolos definidos weak, llamadas atribuibles a
    funciones que solo estan en nuestro .o: weak duplicadas y muertas)."""
    import bisect
    d = open(opath, 'rb').read()
    e = '>'
    shoff, = struct.unpack(e + 'I', d[32:36]); shes, = struct.unpack(e + 'H', d[46:48]); shn, = struct.unpack(e + 'H', d[48:50])
    secs = [struct.unpack(e + '10I', d[shoff + i * shes: shoff + i * shes + 40]) for i in range(shn)]
    st = [x for x in secs if x[1] == 2][0]; strt = secs[st[6]]
    sd = d[strt[4]:strt[4] + strt[5]]
    names = []; undef = set(); defw = set(); bysec = {}; fweak = set()
    for i in range(st[5] // 16):
        o = st[4] + i * 16
        nameo, value, size, info, other, shndx = struct.unpack(e + 'IIIBBH', d[o:o + 16])
        n = sd[nameo:sd.index(b'\0', nameo)].decode('utf-8', 'replace')
        names.append(n)
        if not n:
            continue
        if shndx == 0:
            undef.add(n); continue
        if (info >> 4) == 2:
            defw.add(n)
        if (info & 0xF) == 2:
            bysec.setdefault(shndx, []).append((value, value + size, n))
            if (info >> 4) == 2:
                fweak.add(n)
    for k in bysec:
        bysec[k].sort()
    starts = dict((k, [x[0] for x in v]) for k, v in bysec.items())
    calls = Counter(); dup = Counter(); strip = Counter()
    for sec in secs:
        if sec[1] != 4 or not sec[9]:
            continue
        lst = bysec.get(sec[7]); ss = starts.get(sec[7])
        for i in range(sec[5] // sec[9]):
            o = sec[4] + i * sec[9]
            off, info, add_ = struct.unpack(e + 'IIi', d[o:o + 12])
            if (info & 0xFF) != R_PPC_REL24:
                continue
            si = info >> 8
            if si >= len(names) or names[si] not in undef:
                continue
            calls[names[si]] += 1
            if lst:
                j = bisect.bisect_right(ss, off) - 1
                if j >= 0 and lst[j][0] <= off < lst[j][1]:
                    fn = lst[j][2]
                    if fn not in tlocal:
                        if fn not in elf:
                            strip[names[si]] += 1
                        elif fn in fweak:
                            dup[names[si]] += 1
    return calls, defw, dup, strip


def real(args, verbose=False):
    elf = _elf_syms()
    rows = []; tot = Counter(); DET = {}
    for name in units_from(args):
        spath, opath = unit_paths(name)
        if not (os.path.exists(spath) and os.path.exists(opath)):
            continue
        tc = target_calls(spath)
        tlocal = _target_local(spath)
        oc, defw, dup, strip = _our_detail(opath, tlocal, elf)
        ao = ad = ast = r = 0
        for sym in set(tc) | set(oc):
            t, o = tc.get(sym, 0), oc.get(sym, 0)
            dd = abs(t - o)
            if not dd:
                continue
            if o == 0 and t > 0 and sym in defw:
                ao += dd
                DET.setdefault('ART_OBJ', Counter())[sym] += dd
            elif o > t:
                s_ = min(dd, strip.get(sym, 0)); rem = dd - s_
                a_ = min(rem, dup.get(sym, 0)); rem -= a_
                ast += s_; ad += a_; r += rem
                if s_: DET.setdefault('ART_STRIP', Counter())[sym] += s_
                if a_: DET.setdefault('ART_DUP', Counter())[sym] += a_
                if rem: DET.setdefault('REAL', Counter())[sym] += rem
            else:
                r += dd
                DET.setdefault('REAL', Counter())[sym] += dd
        tot['art_obj'] += ao; tot['art_dup'] += ad; tot['art_strip'] += ast; tot['real'] += r
        rows.append((ao + ad + ast + r, name, ao, ad, ast, r))
    rows.sort(reverse=True)
    print('%-22s %8s %8s %8s %9s %8s' % ('unidad', 'total', 'ART_OBJ', 'ART_DUP', 'ART_STRIP', 'REAL'))
    for t, n, ao, ad, ast, r in rows:
        print('%-22s %8d %8d %8d %9d %8d' % (n, t, ao, ad, ast, r))
    print('%-22s %8d %8d %8d %9d %8d' % ('TOTAL', sum(tot.values()), tot['art_obj'], tot['art_dup'], tot['art_strip'], tot['real']))
    if verbose:
        print()
        for k in ('REAL', 'ART_OBJ', 'ART_DUP', 'ART_STRIP'):
            print('=== %s ===' % k)
            for s, n in DET.get(k, Counter()).most_common(40):
                print('%6d  %s' % (n, s[:100]))
            print()


def main():
    if '--real' in sys.argv:
        a = [x for x in sys.argv[1:] if not x.startswith('-')]
        return real(a, '-v' in sys.argv)
    if '--where' in sys.argv:
        i = sys.argv.index('--where')
        unidad = sys.argv[i + 1]
        if len(sys.argv) > i + 2 and not sys.argv[i + 2].startswith('-'):
            return where(unidad, sys.argv[i + 2])
        # sin simbolo: TODOS los descuadrados de esa unidad, en una pasada
        spath, opath = unit_paths(unidad)
        tc, oc = target_calls(spath), our_calls(opath)
        difs = sorted(((abs(tc.get(k, 0) - oc.get(k, 0)), k)
                       for k in set(tc) | set(oc)
                       if tc.get(k, 0) != oc.get(k, 0)), reverse=True)
        print('%d simbolos descuadrados en %s' % (len(difs), unidad))
        for n, sym in difs:
            print()
            where(unidad, sym)
        return
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    verbose = "-v" in sys.argv
    rows = []
    for name in units_from(args):
        spath, opath = unit_paths(name)
        if not (os.path.exists(spath) and os.path.exists(opath)):
            continue
        tc = target_calls(spath)
        oc = our_calls(opath)
        if oc is None:
            continue
        difs = []
        for sym in set(tc) | set(oc):
            a, b = tc.get(sym, 0), oc.get(sym, 0)
            if a != b:
                difs.append((abs(a - b), sym, a, b))
        difs.sort(reverse=True)
        total = sum(x[0] for x in difs)
        rows.append((total, name, difs))
        if args:
            print("=== %s: %d llamadas descuadradas en %d simbolos" % (name, total, len(difs)))
            for n, sym, a, b in difs[: (10000 if verbose else 15)]:
                print("   objetivo %4d  nuestro %4d   %s" % (a, b, sym))
            print()
    if not args:
        rows.sort(reverse=True)
        print("%-22s %10s %10s" % ("unidad", "descuadre", "simbolos"))
        for total, name, difs in rows:
            print("%-22s %10d %10d" % (name, total, len(difs)))
        print()
        print("TOTAL descuadrado:", sum(r[0] for r in rows))


if __name__ == "__main__":
    main()
