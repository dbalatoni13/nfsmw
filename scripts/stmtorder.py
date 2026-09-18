#!/usr/bin/env python3
"""stmtorder.py - busca el ORDEN DE FUENTE de un bloque de stores.

El planificador de GCC 2.9 reordena libremente los stores que van a la MISMA
base (`p->campo = ...`), asi que **el orden emitido en el .s NO es el del
fuente**: transcribirlo tal cual deja la funcion en el 85-99%.  Los stores a
bases DISTINTAS si conservan su orden relativo, y eso ancla el bloque.

Esta herramienta prueba ordenes del bloque y mide con objdiff (con las DOS
opciones, ver docs/PLAYBOOK.md seccion 2).  En la ronda de `snd` cerro 15 de
las 28 unidades.

  # permutacion exhaustiva (bloques de <= 6 sentencias)
  python scripts/stmtorder.py <fichero> <unidad-objdiff> <clave-build> A B [simbolo]

  # escalada por movimientos simples (bloques grandes: n*(n-1) por ronda)
  python scripts/stmtorder.py ... --hill

A y B son la primera y la ultima linea del bloque (1-based, inclusivas).  Cada
sentencia debe ocupar UNA linea; los grupos que siempre salen juntos (los cuatro
punteros nulos de un nodo de lista) se ponen en una sola linea para bajar n.

  <unidad-objdiff>  nombre tal cual en objdiff.json, p.ej.
                    main/Speed/Indep/Libs/snd/9/source/library/mix/suppf
  <clave-build>     lo que acepta build_direct.py, p.ej. suppf

Cuando ni la permutacion ni la escalada llegan al 100%, queda invertir la
permutacion observada: se lee del diff que posicion emitida viene de que
sentencia y se despeja el orden que produciria el del objetivo (PLAYBOOK 5).
"""
import itertools
import json
import os
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')


def unit_paths(name):
    cfg = json.load(open(os.path.join(ROOT, 'objdiff.json'), encoding='utf-8'))
    for u in cfg['units']:
        if u['name'] == name or u['name'].endswith('/' + name):
            tgt = u['target_path']
            base = u.get('base_path') or tgt.replace('build/GOWE69/obj/',
                                                     'build/GOWE69/src/', 1)
            return os.path.join(ROOT, tgt), os.path.join(ROOT, base)
    raise SystemExit('unidad no encontrada en objdiff.json: ' + name)


def score(tgt, base, filt, tag):
    """-> (bytes al 100%, media ponderada). La media solo desempata."""
    out = os.path.join(os.environ.get('TEMP', '.'), 'stmtorder_%s.json' % tag)
    r = subprocess.run([CLI, 'diff', '-1', tgt, '-2', base,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false',
                        '-o', out, '--format', 'json'],
                       capture_output=True, text=True)
    if r.returncode:
        return -1, -1
    d = json.load(open(out, encoding='utf-8'))
    fns = [s for s in d['left']['symbols'] if s.get('kind') == 'SYMBOL_FUNCTION']
    if filt:
        fns = [s for s in fns if filt in s['name']]
    total = sum(int(s['size']) for s in fns) or 1
    ok = sum(int(s['size']) for s in fns if (s.get('match_percent') or 0) >= 100)
    avg = sum((s.get('match_percent') or 0) * int(s['size']) for s in fns) / total
    return ok, avg


def write_retry(path, text):
    """El compilador tiene el fichero abierto a ratos; reintentar."""
    for _ in range(20):
        try:
            open(path, 'w', encoding='utf-8', newline='\n').write(text)
            return
        except OSError:
            time.sleep(0.25)
    raise OSError('no se pudo escribir ' + path)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    hill = '--hill' in sys.argv
    src, unit, key, a, b = args[0], args[1], args[2], int(args[3]), int(args[4])
    filt = args[5] if len(args) > 5 else None
    tgt, base = unit_paths(unit)
    lines = open(src, encoding='utf-8').read().split('\n')
    block = lines[a - 1:b]
    n = len(block)

    def try_order(order):
        write_retry(src, '\n'.join(lines[:a - 1] + list(order) + lines[b:]))
        subprocess.run([sys.executable, os.path.join('scripts', 'build_direct.py'), key],
                       cwd=ROOT, capture_output=True, text=True)
        return score(tgt, base, filt, key.replace('/', '_'))

    best = (try_order(block), block[:])
    print('inicio %d B %.3f%%' % best[0], flush=True)
    tries = 1
    if hill:
        while best[0][1] < 100:
            moved = False
            for i in range(n):
                for j in range(n):
                    if i == j:
                        continue
                    o = best[1][:]
                    o.insert(j, o.pop(i))
                    s = try_order(o)
                    tries += 1
                    if s > best[0]:
                        best = (s, o)
                        moved = True
                        print('  %d B %.3f%%  mv %d->%d' % (s[0], s[1], i, j), flush=True)
                        break
                if moved:
                    break
            if not moved:
                break
    else:
        for perm in itertools.permutations(range(n)):
            o = [block[i] for i in perm]
            s = try_order(o)
            tries += 1
            if s > best[0]:
                best = (s, o)
                print('  %d B %.3f%%  %s' % (s[0], s[1], perm), flush=True)
            if s[1] >= 100:
                break
    try_order(best[1])
    print('MEJOR %d B %.3f%% tras %d pruebas' % (best[0][0], best[0][1], tries))
    print('\n'.join(best[1]))


if __name__ == '__main__':
    main()
