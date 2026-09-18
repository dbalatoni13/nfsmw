#!/usr/bin/env python3
"""pctsnap.py -- instantanea y diff del PORCENTAJE POR FUNCION.

`measure.py` cuenta bytes de `matched_code`, que es todo-o-nada por funcion: una
funcion puede caer del 95,5% al 75,7% --y engordar 300 B-- sin mover ni un byte
la medida. Eso ya ha costado dinero: en la ronda 13 se commiteo `Waiting` en el
estado malo porque la verificacion decia «ninguna unidad baja».

Esta herramienta cubre ese hueco. Uso:

    python scripts/pctsnap.py -o antes.json zWorld zSpeech    # instantanea
    python scripts/pctsnap.py --cmp antes.json despues.json   # diff

Sin unidades toma las 33 SourceLists. El diff lista TODA funcion que se mueva
mas de --umbral puntos (por defecto 0,05) y separa las que EMPEORAN.
"""
import json
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
SL = os.path.join(ROOT, 'src', 'Speed', 'Indep', 'SourceLists')
OBJ = os.path.join(ROOT, 'build', 'GOWE69', 'obj', 'Speed', 'Indep', 'SourceLists')
OUR = os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists')


def unidades(sel):
    todas = sorted(f[:-4] for f in os.listdir(SL) if f.endswith('.cpp'))
    if not sel:
        return todas
    fuera = [u for u in sel if u not in todas]
    if fuera:
        sys.exit('unidad desconocida: %s' % ', '.join(fuera))
    return sel


def snapshot(sel, dest):
    out = {}
    for u in unidades(sel):
        a, b = os.path.join(OBJ, u + '.o'), os.path.join(OUR, u + '.o')
        if not (os.path.exists(a) and os.path.exists(b)):
            continue
        tmp = dest + '.tmp'
        subprocess.run([CLI, 'diff', '-1', a, '-2', b,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false',
                        '-o', tmp, '--format', 'json', 'zz'],
                       capture_output=True, text=True, cwd=ROOT)
        if not os.path.exists(tmp):
            continue
        d = json.load(open(tmp, encoding='utf-8'))
        os.remove(tmp)
        for s in d['right']['symbols']:
            if s.get('kind') != 'SYMBOL_FUNCTION' or not s.get('name'):
                continue
            p = s.get('match_percent')
            if p is None:
                continue
            # [porcentaje, tamano NUESTRO]
            out['%s|%s' % (u, s['name'])] = [round(float(p), 4), int(s.get('size') or 0)]
    json.dump(out, open(dest, 'w', encoding='utf-8'))
    print('%d funciones en %d unidades -> %s' % (len(out), len(set(k.split('|')[0] for k in out)), dest))


def compare(pa, pb, umbral):
    a = json.load(open(pa, encoding='utf-8'))
    b = json.load(open(pb, encoding='utf-8'))
    peor, mejor, nuevas = [], [], []
    for k, vb in b.items():
        va = a.get(k)
        if va is None:
            nuevas.append((k, vb))
            continue
        d = vb[0] - va[0]
        if d < -umbral:
            peor.append((d, k, va, vb))
        elif d > umbral:
            mejor.append((d, k, va, vb))
    peor.sort()
    mejor.sort(reverse=True)

    def fila(d, k, va, vb):
        u, n = k.split('|', 1)
        db = vb[1] - va[1]
        return '  %+8.3f pp  %-13s %-52s %8.4f -> %8.4f  %d B%s' % (
            d, u, n[:52], va[0], vb[0], vb[1], (' (%+d)' % db) if db else '')

    if peor:
        print('EMPEORAN: %d funciones' % len(peor))
        for r in peor:
            print(fila(*r))
    else:
        print('EMPEORAN: ninguna')
    print('MEJORAN: %d funciones' % len(mejor))
    for r in mejor[:40]:
        print(fila(*r))
    if len(mejor) > 40:
        print('  ... y %d mas' % (len(mejor) - 40))
    if nuevas:
        print('SIN PAREJA en la instantanea previa: %d' % len(nuevas))


def main():
    a = sys.argv[1:]
    umbral = 0.05
    if '--umbral' in a:
        i = a.index('--umbral')
        umbral = float(a[i + 1])
        a = a[:i] + a[i + 2:]
    if '--cmp' in a:
        i = a.index('--cmp')
        return compare(a[i + 1], a[i + 2], umbral)
    dest = 'pct.json'
    if '-o' in a:
        i = a.index('-o')
        dest = a[i + 1]
        a = a[:i] + a[i + 2:]
    snapshot([x for x in a if not x.startswith('-')], dest)


if __name__ == '__main__':
    main()
