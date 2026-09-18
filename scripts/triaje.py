#!/usr/bin/env python3
"""triaje.py -- clasifica los near-miss por TIPO de diferencia, no por porcentaje.

El porcentaje no dice si una funcion esta a un `mr` o a un bucle entero. Esto
diffea cada unidad UNA vez y para cada funcion por debajo del 100 % cuenta:

    regs    solo cambia un registro en el operando (ARG_MISMATCH)   -> permutador
    falta   instrucciones de menos (DELETE)                          -> local/sentencia ausente
    sobra   instrucciones de mas (INSERT)                            -> codigo defensivo
    otro    REPLACE: opcode distinto                                 -> estructura

    python scripts/triaje.py            # las 40 mayores
    python scripts/triaje.py zWorld     # una unidad entera
"""
import json
import os
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')


def diff_unidad(rel):
    o1 = os.path.join(ROOT, 'build', 'GOWE69', 'obj', *rel.split('/')) + '.o'
    o2 = os.path.join(ROOT, 'build', 'GOWE69', 'src', *rel.split('/')) + '.o'
    if not (os.path.exists(o1) and os.path.exists(o2)):
        return None
    out = os.path.join(tempfile.gettempdir(), 'triaje_%s.json' % rel.replace('/', '_'))
    subprocess.run([CLI, 'diff', '-1', o1, '-2', o2, '-c', 'function_reloc_diffs=none',
                    '-c', 'ppc.calculatePoolRelocations=false', '-o', out, '--format', 'json'],
                   capture_output=True, text=True)
    if not os.path.exists(out):
        return None
    return json.load(open(out))


def main():
    os.chdir(ROOT)
    pedidas = sys.argv[1:]
    r = json.load(open('build/GOWE69/report.json'))
    obj = {}
    for u in r['units']:
        corto = u['name'].split('/')[-1]
        if pedidas and corto not in pedidas:
            continue
        for f in u.get('functions') or []:
            if f.get('fuzzy_match_percent', 100.0) < 100.0:
                obj.setdefault(u['name'].split('main/', 1)[-1], []).append(
                    (int(f.get('size', 0)), f.get('name'), f.get('fuzzy_match_percent')))
    filas = []
    for rel, fns in obj.items():
        d = diff_unidad(rel)
        if d is None:
            continue
        izq = {s.get('name'): s for s in d['left']['symbols']}
        der = {s.get('name'): s for s in d['right']['symbols']}
        for sz, nm, pct in fns:
            A, B = izq.get(nm), der.get(nm)
            if not A or not B:
                continue
            c = {'regs': 0, 'falta': 0, 'sobra': 0, 'otro': 0}
            LI, RI = A.get('instructions', []), B.get('instructions', [])
            for k in range(max(len(LI), len(RI))):
                a = LI[k] if k < len(LI) else None
                b = RI[k] if k < len(RI) else None
                ka = (a or {}).get('diff_kind', '') or ''
                kb = (b or {}).get('diff_kind', '') or ''
                kk = ka if ka not in ('', 'DIFF_NONE') else kb
                if kk in ('', 'DIFF_NONE'):
                    continue
                if 'ARG' in kk:
                    c['regs'] += 1
                elif 'DELETE' in kk:
                    c['falta'] += 1
                elif 'INSERT' in kk:
                    c['sobra'] += 1
                else:
                    c['otro'] += 1
            tot = sum(c.values())
            veredicto = ('PERMUTADOR' if c['regs'] == tot else
                         'falta codigo' if c['falta'] and not c['otro'] else
                         'sobra codigo' if c['sobra'] and not c['otro'] else
                         'ESTRUCTURA')
            filas.append((sz, rel.split('/')[-1], nm, pct, tot, c, veredicto))
    filas.sort(reverse=True)
    print('%7s %-12s %-42s %8s %5s  %s' % ('B', 'unidad', 'funcion', '%', 'difs', 'veredicto'))
    for sz, u, nm, pct, tot, c, v in filas[:40 if not pedidas else 200]:
        print('%7d %-12s %-42s %7.3f %5d  %-13s regs=%d falta=%d sobra=%d otro=%d'
              % (sz, u, nm[:42], pct, tot, v, c['regs'], c['falta'], c['sobra'], c['otro']))
    print('%d funciones, %d B' % (len(filas), sum(f[0] for f in filas)))


main()
