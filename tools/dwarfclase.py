#!/usr/bin/env python3
"""dwarfclase.py -- el parecido de DWARF de todas las funciones de unas clases.

Saca del volcado partido (symbols/Dwarf/functions.nothpp) las funciones
definidas (con rango real, no 0xFFFFFFFF) cuyo nombre empieza por alguno de los
prefijos dados, las pasa a tools/dwarf-compare.py en un solo lote y ordena por
parecido. Lo que el porcentaje de codigo no ve: nombres de parametros y
locales, bloques, constantes locales e inlines que el original llama por su
nombre.

    python tools/dwarfclase.py <unidad> <prefijo> [<prefijo> ...] [--todo]

    unidad: p.ej. zFe  (main/Speed/Indep/SourceLists/<unidad>)
    prefijo: p.ej. WorldMap::  ItemTypeToggle::
    --todo: imprime tambien las que estan al 100%
"""
import json
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FUNCS = os.path.join(ROOT, 'symbols', 'Dwarf', 'functions.nothpp')


def firma(linea):
    """`void A::f(unsigned int x /* r3 */, enum E e) const {` -> `A::f(unsigned int, E)`."""
    linea = re.sub(r'/\*.*?\*/', '', linea)
    m = re.match(r'^(?:static\s+|virtual\s+|inline\s+)*(?:[\w:<>,\s\*&]+?\s+)?([\w:~<>]+(?:::operator\s*\S+?)?)\s*\((.*)\)\s*(const)?\s*(?:override)?\s*\{', linea)
    if not m:
        return None
    nombre, params, const = m.group(1), m.group(2).strip(), m.group(3)
    tipos = []
    if params and params != 'void':
        prof, cur = 0, ''
        for c in params:
            if c in '<(':
                prof += 1
            elif c in '>)':
                prof -= 1
            if c == ',' and prof == 0:
                tipos.append(cur)
                cur = ''
            else:
                cur += c
        tipos.append(cur)
    limpios = []
    for t in tipos:
        t = re.sub(r'\b(struct|enum|class|union)\s+', '', t.strip())
        # quita el nombre del parametro (ultimo identificador si no es parte del tipo)
        mm = re.match(r'^(.*?[\*&\s])(\w+)$', t)
        if mm and mm.group(2) not in ('int', 'char', 'float', 'double', 'long', 'short', 'bool', 'unsigned', 'signed', 'const'):
            t = mm.group(1)
        t = re.sub(r'\s+', ' ', t).strip()
        t = re.sub(r'\s*\*', ' *', t).replace('* *', '**')
        t = re.sub(r'\s*&', ' &', t)
        limpios.append(t)
    return '%s(%s)%s' % (nombre, ', '.join(limpios), ' const' if const else '')


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    todo = '--todo' in sys.argv
    unidad, prefijos = args[0], args[1:]
    nombres, rango = [], None
    for linea in open(FUNCS, encoding='utf-8', errors='replace'):
        if linea.startswith('// Range:'):
            rango = linea
            continue
        if linea[:1].isspace() or not linea.strip() or linea.startswith('//'):
            continue
        if any(p in linea for p in prefijos) and linea.rstrip().endswith('{') and rango and '0xFFFFFFFF' not in rango.split('->')[0]:
            f = firma(linea.strip())
            if f and any(f.startswith(p) for p in prefijos) and f not in nombres:
                nombres.append(f)
        rango = None
    fd, ruta = tempfile.mkstemp(suffix='.json')
    os.close(fd)
    json.dump(nombres, open(ruta, 'w'))
    r = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'dwarf-compare.py'), '-u',
                        'main/Speed/Indep/SourceLists/' + unidad, '--functions-file', ruta, '--json'],
                       capture_output=True, text=True, errors='replace', cwd=ROOT)
    try:
        datos = json.loads(r.stdout)
    except ValueError:
        print(r.stdout[-2000:], r.stderr[-2000:])
        return 1
    filas = []
    for d in datos:
        nombre = d.get('function') or d.get('query') or '?'
        pct = d.get('normalized_match_percent', d.get('match_percent'))
        if pct is None:
            filas.append((-1, nombre, d.get('error', 'sin datos')))
            continue
        firma_ok = d.get('signature_match', d.get('signature_matches'))
        filas.append((pct, nombre, '' if firma_ok in (True, None) else 'FIRMA'))
    filas.sort()
    for pct, nombre, nota in filas:
        if todo or pct < 100:
            print('%6.1f%%  %-5s %s' % (pct, nota, nombre))
    ok = sum(1 for f in filas if f[0] >= 100)
    print('\n%d funciones, %d al 100%% de DWARF' % (len(filas), ok))
    return 0


if __name__ == '__main__':
    sys.exit(main())
