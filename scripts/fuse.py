#!/usr/bin/env python3
"""Funde el desensamblado del original con el mapa de lineas de EA.

Salio de escribir GManager desde cero: 58 de 60 funciones al 100%, y 32 de 36 a
la primera compilacion. La fuente decisiva no fue el DWARF, fue esto — ver la
linea de fuente de cada instruccion deja leer el if, el continue, el break y que
sentencias van juntas. Escribir una clase ausente pasa a ser trabajo de lectura.

  python scripts/fuse.py zGameplay GManager::Init     # una funcion
  python scripts/fuse.py zGameplay --list GManager    # que hay de esa clase
  python scripts/fuse.py zGameplay --files            # ficheros de EA de la unidad
"""
import os
import re
import subprocess
import sys
from collections import defaultdict

ASM_ROOT = os.path.join('build', 'GOWE69', 'asm', 'Speed', 'Indep', 'SourceLists')
LINES = os.path.join('symbols', 'debug_lines.txt')

RE_LINE = re.compile(r'^0x([0-9A-Fa-f]+): (\S+) \(line (\d+)\)')
RE_FN = re.compile(r'^\.fn (\S+?),')
RE_INSN = re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  ((?:[0-9A-F]{2} ){4})\*/\s*(.*?)\s*$')


def load_lines():
    """direccion -> (fichero de EA, linea)."""
    out = {}
    with open(LINES, encoding='utf-8', errors='replace') as fh:
        for l in fh:
            m = RE_LINE.match(l)
            if m:
                out[int(m.group(1), 16)] = (m.group(2), int(m.group(3)))
    return out


def scan_unit(unit):
    """-> {simbolo: [(direccion, bytes, texto)]} del .s del original."""
    path = os.path.join(ASM_ROOT, unit + '.s')
    if not os.path.exists(path):
        sys.exit('no existe %s' % path)
    fns, cur = defaultdict(list), None
    for l in open(path, encoding='utf-8', errors='replace'):
        m = RE_FN.match(l)
        if m:
            cur = m.group(1)
            continue
        if l.startswith('.endfn'):
            cur = None
            continue
        if cur:
            m = RE_INSN.match(l)
            if m:
                fns[cur].append((int(m.group(1), 16), m.group(2).strip(), m.group(3)))
    return fns


def demangled(unit):
    """simbolo -> nombre legible, desde el report de objdiff si esta."""
    import json
    p = os.path.join('build', 'GOWE69', 'report.json')
    out = {}
    if not os.path.exists(p):
        return out
    rep = json.load(open(p))
    for u in rep.get('units', []):
        if not u['name'].endswith('/' + unit):
            continue
        for f in u.get('functions') or []:
            out[f['name']] = (f.get('metadata', {}).get('demangled_name', f['name']),
                              f.get('fuzzy_match_percent', 0.0))
    return out


def show(unit, symbol, fns, addr2line, names):
    insns = fns.get(symbol)
    if insns is None:
        # buscar por nombre demangled
        for sym, (dem, _) in names.items():
            if symbol in dem and sym in fns:
                symbol, insns = sym, fns[sym]
                break
    if not insns:
        sys.exit('no encuentro %s en %s' % (symbol, unit))
    dem, pct = names.get(symbol, (symbol, None))
    print('%s' % dem)
    print('%s  |  %d instrucciones%s' % (symbol, len(insns),
                                         '' if pct is None else '  |  %.2f%%' % pct))
    print('-' * 78)
    last = None
    for addr, raw, text in insns:
        ent = addr2line.get(addr)
        if ent and ent != last:
            fich, ln = ent
            print('\n  --- %s:%d ---' % (fich.rsplit('/', 1)[-1], ln))
            last = ent
        print('  %08X  %-28s %s' % (addr, text, ''))


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    unit = sys.argv[1]
    rest = sys.argv[2:]
    fns = scan_unit(unit)
    names = demangled(unit)

    if '--files' in rest:
        addr2line = load_lines()
        per = defaultdict(int)
        for sym, insns in fns.items():
            for addr, _, _ in insns:
                e = addr2line.get(addr)
                if e:
                    per[e[0]] += 4
        for f, b in sorted(per.items(), key=lambda kv: -kv[1]):
            print('%9d B  %s' % (b, f))
        return

    if '--list' in rest:
        i = rest.index('--list')
        pref = rest[i + 1] if i + 1 < len(rest) else ''
        rows = []
        for sym, insns in fns.items():
            dem, pct = names.get(sym, (sym, 0.0))
            if pref and pref not in dem and pref not in sym:
                continue
            rows.append((len(insns) * 4, pct, dem))
        rows.sort(reverse=True)
        print('%d simbolos' % len(rows))
        for b, pct, dem in rows:
            print('%7d B  %6.2f%%  %s' % (b, pct, dem[:80]))
        return

    show(unit, rest[0], fns, load_lines(), names)


if __name__ == '__main__':
    main()
