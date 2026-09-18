#!/usr/bin/env python3
"""frontcoste.py -- que CARGA y que PRECIO tiene cada frontera de `aranges.py --faltan`.

`scripts/aranges.py --faltan` lista 113 fronteras de unidad que `splits.txt` no
tiene, y `--cotejo` 48 que tiene CORRIDAS. Las dos salidas son ciertas sobre
DONDE empieza cada seccion de cada unidad, pero **ninguna de las dos dice si ahi
hay algo**. Este fichero contesta las dos preguntas que deciden si una frontera
se puede tocar:

  --carga    que se MUEVE de dueno si se aplica: simbolos de `symbols.txt` en el
             rango, y bytes no nulos del ELF original. Una frontera que mueve
             CERO simbolos y CERO bytes no nulos es relleno de alineacion: el
             troceador no puede nombrar nada ahi y cambiarla no cambia nada.
  --precio   solo para las fronteras que hoy caen en un comodin `auto_*`: cuantos
             bytes tiene el rango contra cuantos emite HOY nuestro objeto
             compilado (`build/GOWE69/src/<unidad>.o`). Si nuestro objeto emite
             menos, sacar el rango del `auto_*` BORRA esos bytes del enlace: es
             el mecanismo exacto de `memory/nfsmw-rango-no-basta`, con su precio.
             Imprime tambien el criterio de la r36e/f: el rango tiene que caber
             entre los rangos de las dos vecinas en el ORDEN DE ENLACE.

EL ORDEN DE ENLACE se lee del edge de `build/GOWE69/main.elf` en `build.ninja`,
NO del orden de `Object(...)` de `configure.py`. Medido: contra las direcciones
de `splits.txt`, el del enlace da 0 inversiones en `.text`, `.rodata` y `.data`
(1 en `.bss`); el de `configure.py` da 101, 23 y 14.

LO QUE MIDIO LA PRIMERA PASADA, y es la razon de que exista el fichero:
  - de las 44 fronteras de `.data`/`.bss` entre SourceLists, **43 mueven cero
    simbolos y cero bytes no nulos**. La unica con carga es `zPhysics .data`
    (8 simbolos), y esos 8 estan en la MISMA direccion en nuestro `main.elf` que
    en el original, o sea que ahi tampoco hay nada que mover.
  - de las 54 fronteras de `auto_*` que superan la ventana de enlace, **solo 2**
    tienen nuestro objeto emitiendo el numero exacto de bytes. Las otras 52
    costarian entre 4 y 43.404 B de enlace cada una.

No escribe nada. Solo lee el ELF original, `config/GOWE69/*`, `build.ninja` y
`build/GOWE69/{src,obj}/*.o`.
"""
import bisect
import glob
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import aranges

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BARRA = chr(92)
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass


def _secs(path):
    """-> (fichero abierto, {nombre: [cabecera de seccion]}) del ELF `path`."""
    fh = open(path, 'rb')
    c = fh.read(0x34)
    so = struct.unpack('>I', c[0x20:0x24])[0]
    e, n, x = struct.unpack('>HHH', c[0x2E:0x34])
    fh.seek(so)
    cr = fh.read(e * n)
    S = [struct.unpack('>10I', cr[i * e:i * e + 40]) for i in range(n)]
    fh.seek(S[x][4])
    nm = fh.read(S[x][5])
    d = {}
    for s in S:
        q = nm.index(b'\0', s[0])
        d.setdefault(nm[s[0]:q].decode('latin1'), []).append(s)
    return fh, d


def simbolos_txt():
    out = []
    p = os.path.join(ROOT, 'config', 'GOWE69', 'symbols.txt')
    for l in open(p, encoding='utf-8'):
        m = re.match(r'([^\s=]+)\s*=\s*(\.\w+):(0x[0-9A-Fa-f]+)', l)
        if m:
            out.append((int(m.group(3), 16), m.group(2), m.group(1)))
    out.sort()
    return out


def orden_de_enlace():
    """-> {unidad sin extension: indice} del edge de main.elf en build.ninja."""
    L = open(os.path.join(ROOT, 'build.ninja'), encoding='utf-8').read().splitlines()
    i = next(k for k, l in enumerate(L) if l.startswith('build ') and 'main.elf: link' in l)
    trozos, j = [], i
    while j < len(L):
        trozos.append(L[j].rstrip())
        if not L[j].rstrip().endswith('$'):
            break
        j += 1
    objs = [t for t in ' '.join(x.rstrip('$').strip() for x in trozos).split() if t.endswith('.o')]
    d = {}
    for k, o in enumerate(objs):
        p = o.replace(BARRA, '/')
        for pre in ('build/GOWE69/src/', 'build/GOWE69/obj/'):
            if p.startswith(pre):
                d.setdefault(p[len(pre):-2], k)
    return d


def autos():
    """-> [(inicio, fin, nombre.o, seccion)] de cada comodin auto_*."""
    out = []
    for p in glob.glob(os.path.join(ROOT, 'build', 'GOWE69', 'obj', 'auto_*.o')):
        n = os.path.basename(p)
        m = re.match(r'auto_(\d+)_([0-9A-Fa-f]{8})_(\w+)[.]o$', n)
        if not m:
            continue
        st, kind = int(m.group(2), 16), m.group(3)
        fh, d = _secs(p)
        fh.close()
        tot = sum(s[5] for k, v in d.items() if k.lstrip('.') == kind for s in v)
        out.append((st, st + tot, n, kind))
    out.sort()
    return out


def faltan():
    """Reproduce `aranges.py --faltan` -> [(sec, inicio, siguiente, [unidades])]."""
    U = aranges.unidades()
    SP = aranges.splits()
    ini = {}
    for (u, s, a, b) in SP:
        ini.setdefault(s, set()).add(a)
    cod = set(a for _, s, a, _ in SP if s in aranges.SEC_CODIGO)
    out = []
    for key, sec in (('text', '.text'), ('rodata', '.rodata'), ('data', '.data'), ('bss', '.bss')):
        por = {}
        for u in U:
            por.setdefault(u[key][0], []).append((u, u[key][1]))
        orden = sorted(a for a, l in por.items() if any(t > 0 for _, t in l))
        for i, a in enumerate(orden):
            if (a in cod) if key == 'text' else (a in ini.get(sec, set())):
                continue
            nxt = orden[i + 1] if i + 1 < len(orden) else None
            out.append((sec, a, nxt, [u for u, t in por[a] if t > 0]))
    return out


def _cajon(sec, a, nxt, SP, AU):
    dentro = [(u, x, y) for (u, s2, x, y) in SP if s2 == sec and x <= a < y]
    au = [z for z in AU if z[3] == sec.lstrip('.') and z[0] <= a < z[1]]
    if dentro:
        return 'b', dentro[0][0].split('/')[-1], min(nxt or dentro[0][2], dentro[0][2])
    if au:
        return 'a', au[0][2], min(nxt or au[0][1], au[0][1])
    return 'c', '(nada)', nxt or a


def cmd_carga():
    SY = simbolos_txt()
    V = [s[0] for s in SY]
    SP = aranges.splits()
    AU = autos()
    fh, D = _secs(aranges.ELF)

    def nocero(a, n):
        for k, v in D.items():
            for s in v:
                if s[3] and s[1] != 8 and s[3] <= a and a + n <= s[3] + s[5]:
                    fh.seek(s[4] + a - s[3])
                    return sum(1 for c in fh.read(n) if c)
        return None

    print('%-3s %-8s %-10s %6s %4s %7s  %-42s %s'
          % ('cj', 'sec', 'inicio', 'bytes', 'sym', 'no-cero', 'que lo tiene hoy',
             'dueno segun aranges'))
    tot = {'a': 0, 'b': 0, 'c': 0}
    vacias = 0
    for sec, a, nxt, us in faltan():
        caja, ctx, fin = _cajon(sec, a, nxt, SP, AU)
        i = bisect.bisect_left(V, a)
        ns = 0
        while i < len(SY) and SY[i][0] < fin:
            ns += 1 if SY[i][1] == sec else 0
            i += 1
        z = nocero(a, fin - a) if fin > a else 0
        tot[caja] += 1
        if ns == 0 and z in (0, None):
            vacias += 1
        print('%-3s %-8s 0x%08X %6d %4d %7s  %-42s %s'
              % (caja, sec, a, fin - a, ns, 'bss' if z is None else z, ctx,
                 ','.join(os.path.basename(u['ruta']) for u in us)))
    fh.close()
    print('')
    print('%d fronteras: %d en comodin auto_* (a), %d dentro de un rango (b), %d sin dueno (c)'
          % (sum(tot.values()), tot['a'], tot['b'], tot['c']))
    print('%d mueven CERO simbolos y CERO bytes no nulos: son relleno, no contenido.' % vacias)


ALIAS = {'gc_public.cpp': 'public.cpp', 'gc_tasks.cpp': 'tasks.cpp',
         'gc_trctasks.cpp': 'trctasks.cpp', 'svecreal6.cpp': 'svecreal.cpp',
         'sdfx.c': 'ssdfx.c', 'smemcpy.cpp': 'smemcpy.c'}


def cmd_precio():
    SP = aranges.splits()
    AU = autos()
    ordn = orden_de_enlace()
    base, vis, rng = {}, set(), {}
    for (u, s, a, b) in SP:
        k = ordn.get(u.rsplit('.', 1)[0])
        if k is not None:
            rng.setdefault(s, []).append((k, a, b, u))
        if u not in vis:
            vis.add(u)
            if k is not None:
                base.setdefault(os.path.basename(u), []).append(u)
    for s in rng:
        rng[s].sort()
    print('%-8s %-10s %7s %8s %8s  %-8s %s'
          % ('sec', 'inicio', 'rango', 'nuestro', 'precio', 'ventana', 'unidad receptora'))
    cabe = exacto = coste = 0
    for sec, a, nxt, us in faltan():
        caja, ctx, fin = _cajon(sec, a, nxt, SP, AU)
        if caja != 'a':
            continue
        for u in us:
            bn = os.path.basename(u['ruta'])
            bn = ALIAS.get(bn, bn)
            cand = base.get(bn, [])
            if len(cand) != 1:
                print('%-8s 0x%08X %7d %8s %8s  %-8s %s (%d candidatos)'
                      % (sec, a, fin - a, '?', '?', 'AMBIGUO', bn, len(cand)))
                continue
            ruta = cand[0]
            k = ordn[ruta.rsplit('.', 1)[0]]
            lst = rng.get(sec, [])
            lo = max([x[2] for x in lst if x[0] < k] or [0])
            hi = min([x[1] for x in lst if x[0] > k] or [0xFFFFFFFF])
            ok = lo <= a and fin <= hi
            p = os.path.join(ROOT, 'build', 'GOWE69', 'src',
                             ruta.rsplit('.', 1)[0].replace('/', os.sep) + '.o')
            if os.path.exists(p):
                fh, d = _secs(p)
                fh.close()
                t = sum(s[5] for kk, v in d.items()
                        if kk == sec or kk.startswith(sec + '.') for s in v)
            else:
                t = None
            if ok:
                cabe += 1
                if t == fin - a:
                    exacto += 1
                else:
                    coste += max(0, (fin - a) - (t or 0))
            print('%-8s 0x%08X %7d %8s %8s  %-8s %s'
                  % (sec, a, fin - a, 'SIN .o' if t is None else t,
                     '?' if t is None else '%+d' % (t - (fin - a)),
                     'CABE' if ok else 'NO CABE', ruta))
    print('')
    print('%d caben en su ventana de enlace; de esas %d con nuestro objeto emitiendo YA el tamano exacto.'
          % (cabe, exacto))
    print('Aplicar las otras costaria %d B de enlace: el DOL saldria corto, sin error.' % coste)


def main():
    if any(x in sys.argv[1:] for x in ('--precio', '--ventana')):
        cmd_precio()
    else:
        cmd_carga()


if __name__ == '__main__':
    main()
