#!/usr/bin/env python3
"""estado.py -- el frente entero en una pantalla, y sin creerse las metricas.

Cada ronda se rehacia este censo a mano, y cada ronda salia distinto porque las
dos cifras que da `report.json` no miden lo que parece:

  * **`Data 34,54 %` no mide datos.** Es un proxy de cuantas unidades enlazan.
    Una unidad que no enlaza tiene los nombres de sus datos desemparejados
    --contador de declaracion, `$LC` contra `lbl_`, relleno `gap_`/`pad_`-- y
    saca un 0,4 % que no significa nada. Y una que SI enlaza produce el DOL
    correcto por definicion, asi que lo que le "falte" ahi es nomenclatura.
    Ver `docs/analisis/r50-jf-metrica-datos.md`.
  * **`497/618 unidades` no tiene techo 618.** 74 de las que faltan son
    comodines `auto_*`: rangos que el troceador no atribuye a nadie, no son
    fuente y no pueden enlazar nunca. El techo real es 544.

Esto imprime las dos cifras corregidas, la lista COMPLETA de funciones que no
casan --que es la definicion de "lo que falta"-- y las unidades que quedan
partidas por lo que de verdad las bloquea.

    python scripts/estado.py             # el frente
    python scripts/estado.py --fn        # solo las funciones que faltan
    python scripts/estado.py --uds       # solo las unidades que faltan
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass


def carga():
    p = 'build/GOWE69/report.json'
    if not os.path.exists(p):
        sys.exit('no hay build/GOWE69/report.json: construye antes')
    return json.load(open(p, encoding='utf-8'))


def splits():
    """-> {basename: {seccion: (ini, fin, skip)}}, para saber que unidad esta vacia."""
    s = open('config/GOWE69/splits.txt', encoding='utf-8', errors='replace').read()
    s = s.replace('\r\n', '\n')
    out = {}
    for m in re.finditer(r'^([^\s:][^\n:]*):\n((?:\t\.\w+[^\n]*\n)+)', s, re.M):
        k = os.path.basename(m.group(1)).rsplit('.', 1)[0]
        d = {}
        for r in re.finditer(r'\t(\.\w+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)([^\n]*)',
                             m.group(2)):
            d[r.group(1)] = (int(r.group(2), 16), int(r.group(3), 16), 'skip' in r.group(4))
        out[k] = d
    return out


def funciones(r):
    out = []
    for u in r.get('units', []):
        n = u['name'].replace('main/', '')
        for f in (u.get('functions') or []):
            p = float(f.get('fuzzy_match_percent', 0) or 0)
            if p < 100.0:
                out.append((int(f.get('size', 0) or 0), p, n, f.get('name', '?')))
    out.sort(reverse=True)
    return out


def main():
    r = carga()
    m = r['measures']
    sp = splits()
    solo_fn = '--fn' in sys.argv
    solo_ud = '--uds' in sys.argv

    fn = funciones(r)
    sinenl, enl = [], []
    for u in r.get('units', []):
        (enl if u.get('metadata', {}).get('complete') else sinenl).append(
            u['name'].replace('main/', ''))
    auto = [x for x in sinenl if os.path.basename(x).startswith('auto_')]
    sl = [x for x in sinenl if '/SourceLists/' in x]
    lib = [x for x in sinenl if x not in auto and x not in sl]

    # datos, partidos por estado de enlace
    dd = {True: [0, 0], False: [0, 0]}
    for u in r.get('units', []):
        me = u.get('measures', {})
        td = int(me.get('total_data', 0) or 0)
        if not td:
            continue
        k = bool(u.get('metadata', {}).get('complete'))
        dd[k][0] += int(me.get('matched_data', 0) or 0)
        dd[k][1] += td

    if not solo_fn and not solo_ud:
        print('=' * 72)
        print('HECHO    %s / %s B   %.2f %%   <- codigo que sale de NUESTRA fuente'
              % ('{:,}'.format(int(m.get('complete_code', 0))),
                 '{:,}'.format(int(m['total_code'])),
                 m.get('complete_code_percent', 0.0)))
        print('         Es la unica cifra que no sube al cerrar un comodin, ni al cerrar')
        print('         una funcion de una unidad que no enlaza. Las dos de abajo si.')
        print()
        print('CODIGO   %s / %s B   %.4f %%' %
              ('{:,}'.format(int(m['matched_code'])), '{:,}'.format(int(m['total_code'])),
               m['matched_code_percent']))
        print('         faltan %s B en %d funciones -- la lista esta abajo, es TODO'
              % ('{:,}'.format(sum(x[0] for x in fn)), len(fn)))
        print()
        print('DATOS    la cifra de report.json (%.2f %%) NO mide datos. Partida:'
              % m['matched_data_percent'])
        for k, et in ((True, 'enlazadas  '), (False, 'sin enlazar')):
            a, b = dd[k]
            print('           %s %s / %s B  %5.1f %%%s' %
                  (et, '{:,}'.format(a), '{:,}'.format(b), 100.0 * a / b if b else 0,
                   '   <- nomenclatura: el DOL ya es correcto' if k else
                   '   <- sin sentido: nombres desemparejados'))
        print()
        print('ENLACE   %d / %d unidades' % (len(enl), len(enl) + len(sinenl)))
        # `prodg_fixes` lleva `skip` en sus tres rangos: tampoco puede enlazar nunca.
        # Hasta la r53 el techo salia uno de mas por no restarlo.
        imposibles = len(auto) + sum(1 for x in lib if 'prodg_fixes' in x)
        print('           techo REAL %d: %d comodines auto_* + prodg_fixes, que no enlazan nunca'
              % (len(enl) + len(sinenl) - imposibles, len(auto)))
        print('           QUEDAN %d unidades de verdad: %d SourceLists + %d de biblioteca'
              % (len(sl) + len(lib), len(sl), len(lib)))
        print('=' * 72)
        print()

    if not solo_ud:
        print('LAS %d FUNCIONES QUE NO CASAN (%s B)'
              % (len(fn), '{:,}'.format(sum(x[0] for x in fn))))
        print('%9s %8s  %-20s %s' % ('bytes', 'fuzzy', 'unidad', 'funcion'))
        for sz, p, u, f in fn:
            print('%9s %7.2f%%  %-20s %s'
                  % ('{:,}'.format(sz), p, os.path.basename(u)[:20], f[:58]))
        print()

    if not solo_fn:
        print('LAS %d UNIDADES QUE FALTAN' % (len(sl) + len(lib)))
        pct = {}
        for u in r.get('units', []):
            me = u.get('measures', {})
            tc = int(me.get('total_code', 0) or 0)
            pct[u['name'].replace('main/', '')] = (
                100.0 * int(me.get('matched_code', 0) or 0) / tc if tc else 100.0, tc)
        conf = {}
        for _, _, u, _ in fn:
            conf[os.path.basename(u)] = conf.get(os.path.basename(u), 0) + 1
        print()
        print('  SourceLists (%d) -- el frente duro: orden de emision + .rodata' % len(sl))
        for u in sorted(sl, key=lambda x: -pct[x][1]):
            b = os.path.basename(u)
            print('     %-20s codigo %6.2f%%  %9s B  %s'
                  % (b, pct[u][0], '{:,}'.format(pct[u][1]),
                     '%d funcion(es) abiertas' % conf[b] if b in conf else 'codigo COMPLETO'))
        print()
        print('  Biblioteca (%d)' % len(lib))
        for u in sorted(lib, key=lambda x: -pct[x][1]):
            b = os.path.basename(u)
            d = sp.get(b, {})
            if d and all(v[2] for v in d.values()):
                nota = 'IMPOSIBLE: todos sus rangos llevan `skip`'
            elif not os.path.exists(os.path.join('build', 'GOWE69', 'src',
                                                 *(u.split('/')))+'.o'):
                nota = 'SIN FUENTE: no hay .c/.cpp que compilar'
            elif b in conf:
                nota = '%d funcion(es) abiertas' % conf[b]
            else:
                nota = 'codigo COMPLETO -> le bloquea un DATO'
            print('     %-20s codigo %6.2f%%  %9s B  %s'
                  % (b, pct[u][0], '{:,}'.format(pct[u][1]), nota))
    return 0


if __name__ == '__main__':
    sys.exit(main())
