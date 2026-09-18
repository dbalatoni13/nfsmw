#!/usr/bin/env python3
"""previo.py -- que se ha PROBADO YA en una funcion, y con que resultado.

El trabajo previo de este proyecto no vive en `docs/`: vive en los COMENTARIOS
DEL FUENTE, junto a la funcion, y nadie lo tenia indexado. En una sola ventana
de la r55 estuve a punto de re-atacar dos funciones que ya tenian cuatro y once
variantes medidas y negativas:

  * `SetMemoryPoolSize` -- traza completa del planificador de la r48 y la r49,
    con el desempate de `rank_for_schedule` identificado y cuatro negativos.
  * `DefragmentPool` -- la r46 ya habia sacado los TRES MISMOS ciclos de
    registro que yo acababa de "descubrir", y cuatro variantes negativas.
  * `VP6_PredictFilteredBlock` -- r36f, r46 y r47, con una mejora de 40 a 29
    filas medida y DELIBERADAMENTE no aplicada porque no cierra.

Y el grep ingenuo no vale: cada ronda escribio el negativo con otras palabras
--"MEDIDO Y NEGATIVO", "NEGATIVOS de fuente", "UNSOLVED", "barridas y peores",
"veda"--, asi que buscar una sola formula se come la mitad.

    python scripts/previo.py                  # el indice entero, por fichero
    python scripts/previo.py Defragment       # solo lo que mencione eso
    python scripts/previo.py --fn             # solo los bloques pegados a una
                                              # funcion abierta de report.json

ANTES DE ATACAR UNA FUNCION, CORRE ESTO. Cuesta un segundo y la ronda que
ahorra es entera.
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

SRC = 'src'
EXT = ('.c', '.cpp', '.h', '.hpp')

# Las formulas con que las distintas rondas escribieron un resultado. Cada una
# salio de leer los comentarios de verdad; anadir aqui es barato y no anadir
# esconde trabajo.
MARCA = re.compile(
    r'(MEDIDO|NEGATIVO|NEGATIVOS|UNSOLVED|REFUTAD|barrid|veda|VEDA|peores|'
    r'sin efecto|no aplicado|NO aplicado|cero bytes|identico|IDENTICO|'
    r'\br\d{2}[a-f]?\s*[:.]|\br\d{2}[a-f]?,)', re.I)
RONDA = re.compile(r'\br(\d{1,2}[a-f]?)\b')
COMENT = re.compile(r'^\s*(//|/\*|\*)')


def bloques(path):
    """-> [(linea, [texto...])] de cada bloque de comentario contiguo."""
    try:
        lineas = open(path, encoding='utf-8', errors='replace').read().split('\n')
    except Exception:
        return []
    out, act, ini = [], [], 0
    for i, l in enumerate(lineas, 1):
        if COMENT.match(l):
            if not act:
                ini = i
            act.append(l.strip())
        else:
            if act:
                out.append((ini, act))
            act = []
    if act:
        out.append((ini, act))
    return out


def funcion_de(path, linea):
    """La definicion de funcion mas cercana DESPUES del bloque (o antes)."""
    try:
        ls = open(path, encoding='utf-8', errors='replace').read().split('\n')
    except Exception:
        return ''
    pat = re.compile(r'^[A-Za-z_][\w:<>,\s\*&]*\b(\w+)\s*\([^;]*\)\s*(const\s*)?\{')
    for j in range(linea, min(linea + 160, len(ls))):
        m = pat.match(ls[j])
        if m:
            return m.group(1)
    for j in range(linea - 2, max(linea - 60, 0), -1):
        m = pat.match(ls[j])
        if m:
            return m.group(1) + ' (arriba)'
    return ''


def abiertas():
    """-> {nombre de funcion sin adornos} de report.json, para --fn."""
    p = 'build/GOWE69/report.json'
    if not os.path.exists(p):
        return set()
    r = json.load(open(p, encoding='utf-8'))
    out = set()
    for u in r.get('units', []):
        for f in (u.get('functions') or []):
            if float(f.get('fuzzy_match_percent', 0) or 0) < 100.0:
                n = (f.get('name') or '').split('__')[0]
                if n:
                    out.add(n)
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    solo_fn = '--fn' in sys.argv
    filtro = args[0].lower() if args else None
    fns = abiertas() if solo_fn else set()

    total = 0
    for dirp, _, files in os.walk(SRC):
        for f in sorted(files):
            if not f.endswith(EXT):
                continue
            path = os.path.join(dirp, f)
            # El filtro va al lado SEGURO: si el nombre buscado aparece en
            # CUALQUIER sitio del fichero, se muestran sus bloques aunque el
            # comentario no lo nombre. El comentario de
            # `VP6_PredictFilteredBlock` esta 120 lineas por encima de la
            # funcion y no la menciona: un falso negativo aqui cuesta una
            # ronda entera, y un falso positivo cuesta una lectura.
            try:
                entero = open(path, encoding='utf-8', errors='replace').read().lower()
            except Exception:
                entero = ''
            en_fichero = bool(filtro) and filtro in entero
            hallados = []
            for ini, txt in bloques(path):
                cuerpo = ' '.join(txt)
                if not MARCA.search(cuerpo):
                    continue
                if len(cuerpo) < 60:      # una linea suelta no es un informe
                    continue
                fn = funcion_de(path, ini)
                if filtro and not en_fichero and filtro not in cuerpo.lower() \
                        and filtro not in fn.lower() and filtro not in path.lower():
                    continue
                if solo_fn and not any(x in fn or x in cuerpo for x in fns):
                    continue
                rondas = sorted(set(RONDA.findall(cuerpo)),
                                key=lambda x: (int(re.sub(r'\D', '', x) or 0), x))
                hallados.append((ini, fn, rondas, len(txt), cuerpo))
            if not hallados:
                continue
            print('=== %s' % path.replace('\\', '/'))
            for ini, fn, rondas, n, cuerpo in hallados:
                print('  L%-6d %-34s %2d lineas  rondas: %s'
                      % (ini, (fn or '(fichero)')[:34], n,
                         ', '.join('r' + x for x in rondas) or '-'))
                # el titular: hasta el primer punto util
                t = re.sub(r'^\s*(/\*|//|\*)\s*', '', cuerpo)
                t = re.sub(r'\s+', ' ', t)
                print('           %s' % t[:150])
                total += 1
            print()
    print('%d bloques de trabajo previo%s' % (total, ' (filtrado)' if filtro or solo_fn else ''))
    return 0


if __name__ == '__main__':
    sys.exit(main())
