#!/usr/bin/env python3
"""promodist.py -- QUE cuesta promocionar cada unidad y CUANTO desbloquea.

El porcentaje `matched` no dice si el proyecto avanza: lo dice `linked`, que es
cuanto del juego se construye desde nuestro codigo. Y una funcion no vale sus
bytes, vale **los bytes de la unidad que desbloquea**: `Smackable` mide 3.120 B
y abre zPhysics entera, 146.124 B.

Este guion ordena TODAS las unidades sin promocionar por lo que desbloquean, y
al lado pone lo que falta: funciones pendientes y delta de cada seccion de datos
contra el objeto extraido. Las filas con `fns=0` son codigo ya perfecto que no
enlaza -- ahi el trabajo es de datos, no de codegen.

    python scripts/promodist.py                # todas
    python scripts/promodist.py --sl           # solo SourceLists
    python scripts/promodist.py --libs         # solo bibliotecas/middleware
    python scripts/promodist.py --listas       # una unidad por linea (para barrer)
    python scripts/promodist.py -n 20          # las 20 primeras

Una unidad esta promocionada cuando `report.json` le da `complete_code > 0`; eso
NO depende de `build.ninja`, que los agentes regeneran mientras trabajan.

El juez de una promocion sigue siendo `trypromo.py`, que enlaza de verdad. Esto
solo ordena la cola.
"""
import io
import json
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SECS = ('.rodata', '.data', '.bss', '.sdata', '.sdata2', '.sbss')


def tam_secciones(path):
    """{nombre de seccion: tamano} de un ELF, sin dependencias externas."""
    try:
        f = open(path, 'rb').read()
    except OSError:
        return None
    if f[:4] != b'\x7fELF':
        return None
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    shentsize, shnum, shstrndx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * shentsize:shoff + i * shentsize + 40])
         for i in range(shnum)]
    stro = S[shstrndx][4]
    out = {}
    for s in S:
        e = f.index(b'\0', stro + s[0])
        n = f[stro + s[0]:e].decode()
        if n:
            out[n] = out.get(n, 0) + s[5]
    return out


def main():
    a = sys.argv[1:]
    solo_sl = '--sl' in a
    solo_lib = '--libs' in a
    listas = '--listas' in a
    tope = None
    if '-n' in a:
        tope = int(a[a.index('-n') + 1])

    r = json.load(io.open(os.path.join(ROOT, 'build', 'GOWE69', 'report.json'), encoding='utf-8'))
    filas = []
    for u in r['units']:
        m = u['measures']
        tc = int(m.get('total_code', 0) or 0)
        if tc == 0:
            continue
        if int(m.get('complete_code', 0) or 0) > 0:
            continue                                    # ya promocionada
        nombre = u['name'].replace('main/', '')
        es_sl = '/SourceLists/' in nombre
        if (solo_sl and not es_sl) or (solo_lib and es_sl):
            continue
        fns = [f for f in (u.get('functions') or [])
               if f.get('fuzzy_match_percent', 0) < 100 and int(f.get('size', 0) or 0) > 0]
        nuestro = tam_secciones(os.path.join(ROOT, 'build', 'GOWE69', 'src', *nombre.split('/')) + '.o')
        objetivo = tam_secciones(os.path.join(ROOT, 'build', 'GOWE69', 'obj', *nombre.split('/')) + '.o')
        d = {}
        if nuestro and objetivo:
            for s in SECS:
                v = nuestro.get(s, 0) - objetivo.get(s, 0)
                if v:
                    d[s] = v
        filas.append((tc, len(fns), sum(int(f['size']) for f in fns), d, nombre, nuestro is None))

    filas.sort(key=lambda f: (-f[0], f[1], f[4]))
    limpias = [f for f in filas if f[1] == 0 and not f[5]]
    ntot, total = len(filas), sum(f[0] for f in filas)
    nlim, blim = len(limpias), sum(f[0] for f in limpias)
    if tope:
        filas = filas[:tope]
    if listas:
        print('\n'.join(f[4] for f in filas))
        return

    print('%d unidades sin promocionar, %d B de codigo en juego' % (ntot, total))
    print('   de esas, %d con el .text YA PERFECTO (%d B): el trabajo es de DATOS' % (nlim, blim))
    print()
    print('%10s %5s %8s  %-34s %s' % ('desbloquea', 'fns', 'B fns', 'delta de datos vs objetivo', 'unidad'))
    for tc, nf, bf, d, nombre, sinobj in filas:
        marca = 'SIN OBJETO NUESTRO' if sinobj else ' '.join('%s%+d' % (s.lstrip('.'), v) for s, v in d.items())
        print('%10d %5d %8d  %-34s %s' % (tc, nf, bf, marca[:34], nombre))


if __name__ == '__main__':
    main()
