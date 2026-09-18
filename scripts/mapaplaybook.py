#!/usr/bin/env python3
"""mapaplaybook.py -- el mapa de `PLAYBOOK.md`, para no tener que cargarlo entero.

`docs/PLAYBOOK.md` pesa **442 kB en 6.239 lineas**. Nada de ese tamano se lee
entero, asi que el conocimiento que guarda no llega a quien trabaja: es una de
las tres causas de quema de rondas que midio `docs/ESTRATEGIA.md` §3.

Partirlo con siete agentes leyendolo a la vez es arriesgado. Un indice no: dice
que hay en cada seccion, cuanto pesa y en que linea empieza, y con eso se lee
solo el trozo que toca --`sed -n '3194,3676p' docs/PLAYBOOK.md`--.

    python scripts/mapaplaybook.py            # reescribe docs/PLAYBOOK-MAPA.md
    python scripts/mapaplaybook.py --check    # dice si esta rancio
"""
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'docs', 'PLAYBOOK.md')
DEST = os.path.join(ROOT, 'docs', 'PLAYBOOK-MAPA.md')


def mil(n):
    return '{:,}'.format(n).replace(',', '.')


def genera():
    L = io.open(SRC, encoding='utf-8', errors='replace').read().split('\n')
    cab = []
    for i, ln in enumerate(L):
        m = re.match(r'^(#{2,4})\s+(.*)', ln)
        if m:
            cab.append((i + 1, len(m.group(1)), m.group(2).strip()))
    # tamano de cada seccion = hasta la siguiente cabecera del mismo nivel o menor
    out = []
    for k, (ln, niv, tit) in enumerate(cab):
        fin = len(L)
        for ln2, niv2, _ in cab[k + 1:]:
            if niv2 <= niv:
                fin = ln2 - 1
                break
        out.append((ln, fin, niv, tit))

    R = []
    R.append('# Mapa de PLAYBOOK.md')
    R.append('')
    R.append('**Generado por `scripts/mapaplaybook.py`. No editar a mano.**')
    R.append('')
    R.append('`docs/PLAYBOOK.md` son **%s lineas**. No lo abras entero: busca aqui la '
             'seccion y lee solo su rango.' % mil(len(L)))
    R.append('')
    R.append('```')
    R.append('sed -n \'<desde>,<hasta>p\' docs/PLAYBOOK.md')
    R.append('```')
    R.append('')
    R.append('| lineas | n | seccion |')
    R.append('|---|---:|---|')
    for ln, fin, niv, tit in out:
        sang = '&nbsp;&nbsp;&nbsp;&nbsp;' * (niv - 2)
        R.append('| `%d,%dp` | %d | %s%s |' % (ln, fin, fin - ln + 1, sang,
                                               tit.replace('|', '\\|')))
    R.append('')
    R.append('Las %d secciones suman %s lineas.'
             % (len(out), mil(sum(f - l + 1 for l, f, n, _ in out if n == 2))))
    R.append('')
    return '\n'.join(R)


def main():
    nuevo = genera()
    if '--check' in sys.argv:
        viejo = io.open(DEST, encoding='utf-8', errors='replace').read() if os.path.exists(DEST) else ''
        if viejo.replace('\r\n', '\n') == nuevo:
            print('PLAYBOOK-MAPA.md al dia')
            return 0
        print('PLAYBOOK-MAPA.md RANCIO: corre `python scripts/mapaplaybook.py`')
        return 1
    io.open(DEST, 'w', encoding='utf-8', newline='\n').write(nuevo)
    print('escrito %s (%d B)' % (DEST, len(nuevo)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
