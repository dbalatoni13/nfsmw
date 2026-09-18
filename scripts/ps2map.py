#!/usr/bin/env python3
"""ps2map.py -- el mapa de enlazador del alpha de PS2, que trae el orden de emision.

`orig/SLES-53558-A124/NFS.MAP` (2,78 MB) es el mapa del enlazador del prototipo
«Alpha 124» de PlayStation 2, y lista **por objeto** todos sus simbolos con
direccion y tamano. La ruta de compilacion es

    d:\\p4_apex1666_d1001856\\mw\\speed\\psx2\\obj\\mweuropemilestone\\sourcelists\\zai.cpp.obj

o sea que **el original ya se compilaba por SourceLists**, tambien en PS2.

Para que sirve: ordenando los simbolos de un objeto **por direccion** sale el
orden de emision de ESE build. Como la fuente es la misma, cotejarlo con el
orden de GameCube separa dos cosas que a simple vista se confunden:

  * si PS2 y GameCube coinciden, el orden lo decide **la fuente** (orden de
    parseo, `finish_file`, destructores implicitos...) y hay que buscarlo ahi;
  * si difieren, es **codigo condicional de plataforma**, y no hay nada que
    arreglar en la nuestra por ese lado.

    python scripts/ps2map.py                 # resumen por objeto
    python scripts/ps2map.py --orden zSim    # PS2 contra GameCube, funcion a funcion
    python scripts/ps2map.py --sym <nombre>  # que objeto lo define
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAPA = os.path.join(ROOT, 'orig', 'SLES-53558-A124', 'NFS.MAP')
SIMS = os.path.join(ROOT, 'config', 'GOWE69', 'symbols.txt')
SPLITS = os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt')
BARRA = chr(92)


def base(ruta):
    return ruta.replace(BARRA, '/').rsplit('/', 1)[-1].lower()


def hexa(s):
    return len(s) == 8 and all(c in '0123456789abcdefABCDEF' for c in s)


def leer_mapa():
    """-> {objeto: [(direccion, tamano, nombre)]} en orden de listado."""
    d, obj = {}, None
    for l in open(MAPA, encoding='latin1'):
        t = l.split()
        if len(t) < 4 or not hexa(t[0]) or not hexa(t[1]):
            continue
        resto = ' '.join(t[3:])
        if resto.endswith('.obj'):
            obj = base(resto)[:-4]          # zai.cpp.obj -> zai.cpp
            d.setdefault(obj, [])
            continue
        if obj is None or resto.startswith('.'):
            continue
        d[obj].append((int(t[0], 16), int(t[1], 16), resto))
    return d


def gc_funciones(unidad):
    """-> [(direccion, nombre)] de las funciones de esa SourceList en GameCube."""
    lo = hi = None
    cur = None
    RE = re.compile(r'\s+\.text\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)')
    for l in open(SPLITS, encoding='utf-8'):
        if not l.startswith('\t') and l.rstrip().endswith(':'):
            cur = l.rstrip().rstrip(':')
            continue
        m = RE.match(l)
        if m and cur and base(cur) == unidad.lower() + '.cpp':
            lo, hi = int(m.group(1), 16), int(m.group(2), 16)
            break
    if lo is None:
        return []
    out = []
    RS = re.compile(r'^(\S+) = \.text:(0x[0-9A-Fa-f]+); // type:function')
    for l in open(SIMS, encoding='utf-8'):
        m = RS.match(l)
        if m:
            a = int(m.group(2), 16)
            if lo <= a < hi:
                out.append((a, m.group(1)))
    out.sort()
    return out


def descolocadas(a, b):
    """Cuantos elementos de `b` hay que quitar para que quede en el orden de `a`
    (o sea, len - la subsecuencia comun mas larga). Es la misma medida que usa
    textorder.py, para poder compararlas."""
    pos = {x: i for i, x in enumerate(a)}
    seq = [pos[x] for x in b if x in pos]
    import bisect
    cola = []
    for v in seq:
        i = bisect.bisect_left(cola, v)
        if i == len(cola):
            cola.append(v)
        else:
            cola[i] = v
    return len(seq) - len(cola), len(seq)


def nuestras_funciones(unidad):
    """-> [(direccion, nombre)] de las funciones de NUESTRO objeto, por direccion."""
    import struct
    p = os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists', unidad + '.o')
    try:
        f = open(p, 'rb').read()
    except OSError:
        return []
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]

    def nm(o):
        return f[stro + o:f.index(b'\0', stro + o)].decode(errors='replace')

    ti = [i for i, s in enumerate(S) if nm(s[0]) in ('.text', '.over')]
    sym = next((s for s in S if nm(s[0]) == '.symtab'), None)
    if sym is None:
        return []
    st = S[sym[6]][4]
    out = []
    for i in range(sym[5] // 16):
        a, v, z, inf, _o, shn = struct.unpack('>IIIBBH', f[sym[4] + i * 16:sym[4] + i * 16 + 16])
        n = f[st + a:f.index(b'\0', st + a)].decode(errors='replace')
        if (inf & 0xF) == 2 and shn in ti and n:
            out.append((v, n))
    out.sort()
    return out


def modo_tres(M, unidades):
    """Las funciones que NOSOTROS tenemos descolocadas, y si PS2 corrobora el
    orden del objetivo. Si lo corrobora, el orden lo decide la fuente en DOS
    plataformas y vale la pena buscar la causa; si no, es ruido de version."""
    for u in unidades:
        gc = [n for _, n in gc_funciones(u)]
        nu = [n for _, n in nuestras_funciones(u)]
        ps2 = M.get(u.lower() + '.cpp')
        if not gc or not nu:
            print('%-14s sin datos (objetivo %d, nuestro %d)' % (u, len(gc), len(nu)))
            continue
        o_ps2 = [n for _, _, n in sorted(ps2)] if ps2 else []
        pos_gc = {n: i for i, n in enumerate(gc)}
        pos_ps2 = {n: i for i, n in enumerate(o_ps2)}
        # las nuestras que rompen el orden del objetivo
        seq = [(pos_gc[n], n) for n in nu if n in pos_gc]
        import bisect
        cola, quien = [], []
        for v, n in seq:
            i = bisect.bisect_left(cola, v)
            if i == len(cola):
                cola.append(v); quien.append(n)
            else:
                cola[i] = v; quien[i] = n
        buenas = set(quien)
        malas = [n for _, n in seq if n not in buenas]
        # de esas, cuales corrobora PS2
        corrobora = []
        for n in malas:
            if n not in pos_ps2:
                continue
            # vecinos en el objetivo que tambien estan en PS2
            i = pos_gc[n]
            ant = next((m for m in reversed(gc[:i]) if m in pos_ps2), None)
            sig = next((m for m in gc[i + 1:] if m in pos_ps2), None)
            ok = True
            if ant and not (pos_ps2[ant] < pos_ps2[n]):
                ok = False
            if sig and not (pos_ps2[n] < pos_ps2[sig]):
                ok = False
            if ok:
                corrobora.append(n)
        print('== %-12s %d comunes, %d descolocadas nuestras, %d con PS2 de testigo'
              % (u, len(seq), len(malas), len(corrobora)))
        for n in corrobora[:14]:
            print('     %s' % n[:96])
        if len(corrobora) > 14:
            print('     ... y %d mas' % (len(corrobora) - 14))


def main():
    M = leer_mapa()
    args = [a for a in sys.argv[1:] if not a.startswith('--')]

    if '--tres' in sys.argv:
        modo_tres(M, args or ['zTrack'])
        return

    if '--sym' in sys.argv and args:
        for o, v in sorted(M.items()):
            for a, s, n in v:
                if args[0] in n:
                    print('   %-24s 0x%08X %6d  %s' % (o, a, s, n[:70]))
        return

    if '--orden' in sys.argv:
        unidades = args or sorted(o[:-4] for o in M if o.startswith('z'))
        print('%-14s %7s %7s %7s   %s' % ('unidad', 'comunes', 'desord', '%', 'lectura'))
        for u in unidades:
            ps2 = M.get(u.lower() + '.cpp')
            if not ps2:
                print('%-14s no esta en el mapa de PS2' % u)
                continue
            gc = gc_funciones(u)
            if not gc:
                print('%-14s sin rango .text en splits.txt' % u)
                continue
            o_ps2 = [n for _, _, n in sorted(ps2)]
            o_gc = [n for _, n in gc]
            d, c = descolocadas(o_gc, o_ps2)
            if not c:
                print('%-14s %7d   (ningun nombre en comun)' % (u, 0))
                continue
            pct = 100.0 * (c - d) / c
            lect = 'el orden lo decide la FUENTE' if pct > 97 else (
                   'casi todo fuente' if pct > 85 else 'difieren: mirar codigo condicional')
            print('%-14s %7d %7d %6.1f%%   %s' % (u, c, d, pct, lect))
        return

    print('%d objetos en el mapa, %d simbolos' % (len(M), sum(len(v) for v in M.values())))
    zs = sorted(o for o in M if o.startswith('z'))
    print('%d objetos de SourceList:' % len(zs))
    for o in zs:
        v = M[o]
        print('   %-22s %5d simbolos  0x%08X-0x%08X' % (o, len(v), min(x[0] for x in v), max(x[0] for x in v)))


if __name__ == '__main__':
    main()
