#!/usr/bin/env python3
"""Que simbolos de mas estan REFERENCIADOS, y desde donde.

La pregunta que decide el frente de `linked`: un simbolo que nadie referencia lo
estripa el enlazador --probado: zAttribSys y zBWare enlazan con 3.132 y 2.808 B
de simbolos que no existen en ninguna parte del original--. Uno referenciado, no.

Construye el indice de UNDEF de TODOS los objetos del enlace y cruza los
simbolos de mas de cada unidad sin enlazar contra el.
"""
import os
import sys
import json

sys.path.insert(0, 'scripts')
from extrasym import Elf

ROOT = os.getcwd()
OBJ = os.path.join(ROOT, 'build', 'GOWE69', 'obj')
SRC = os.path.join(ROOT, 'build', 'GOWE69', 'src')
BSLASH = chr(92)


def objetos_del_enlace():
    """Los .o que entran en main.elf, leidos de build.ninja."""
    L = open('build.ninja', encoding='utf-8', errors='replace').read().split('\n')
    i = [k for k, x in enumerate(L) if 'main.elf' in x and x.startswith('build ')]
    if not i:
        return []
    k = i[0]
    trozos = []
    while True:
        trozos.append(L[k])
        if not L[k].rstrip().endswith('$'):
            break
        k += 1
    txt = ' '.join(t.rstrip('$').strip() for t in trozos)
    return [t for t in txt.split() if t.endswith('.o')]


def undefs(p):
    try:
        e = Elf(p)
    except Exception:
        return set()
    return set(s['name'] for s in e.syms() if s['shndx'] == 0 and s['name'])


def defs_texto(p):
    e = Elf(p)
    ci = e.code_idx()
    return set(s['name'] for s in e.syms() if s['type'] == 2 and s['shndx'] in ci and s['name'])


def main():
    objs = objetos_del_enlace()
    print('objetos en el enlace: %d' % len(objs), flush=True)
    idx = {}
    for o in objs:
        p = os.path.join(ROOT, o.replace(BSLASH, os.sep))
        if not os.path.exists(p):
            continue
        corto = os.path.basename(o)
        for n in undefs(p):
            idx.setdefault(n, set()).add(corto)
    print('simbolos referenciados (UNDEF) distintos: %d' % len(idx), flush=True)

    r = json.load(open('build/GOWE69/report.json', encoding='utf-8'))
    sin = []
    for x in r.get('units', []):
        m = x.get('measures', {})
        tc = int(m.get('total_code', 0))
        mc = int(m.get('matched_code', 0))
        cc = int(m.get('complete_code', 0))
        if cc == 0 and tc and tc == mc:
            sin.append(x['name'].replace('main/', ''))

    print()
    print('%-34s %7s %7s %8s' % ('unidad', 'sobran', 'refs', 'B refs'))
    tot_s = tot_r = tot_b = 0
    filas = []
    for u in sorted(sin):
        a = os.path.join(OBJ, *u.split('/')) + '.o'
        b = os.path.join(SRC, *u.split('/')) + '.o'
        if not (os.path.exists(a) and os.path.exists(b)):
            continue
        try:
            A, B = defs_texto(a), defs_texto(b)
            eB = Elf(b)
            tam = {s['name']: s['size'] for s in eB.syms() if s['type'] == 2}
        except Exception as e:
            print('%-34s ERROR %s' % (u.split('/')[-1], e))
            continue
        sobra = B - A
        propio = os.path.basename(b)
        refs = sorted(n for n in sobra if n in idx and (idx[n] - {propio}))
        bytes_refs = sum(tam.get(n, 0) for n in refs)
        filas.append((u, len(sobra), len(refs), bytes_refs, refs[:3], idx))
        tot_s += len(sobra)
        tot_r += len(refs)
        tot_b += bytes_refs
    for u, s, nr, br, ej, _ in sorted(filas, key=lambda x: -x[2]):
        marca = '' if nr else '   <== NINGUNO referenciado'
        print('%-34s %7d %7d %8d%s' % (u.split('/')[-1], s, nr, br, marca), flush=True)
        for e in ej:
            print('%36s %-46s <- %s' % ('', e[:46], ', '.join(sorted(idx[e])[:3])))
    print()
    print('TOTAL: %d simbolos de mas, %d referenciados desde otro objeto (%d B)'
          % (tot_s, tot_r, tot_b))


main()
