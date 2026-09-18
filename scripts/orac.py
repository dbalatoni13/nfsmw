#!/usr/bin/env python3
"""orac.py -- el oraculo, en un grep: locales, registros, inlines y structs.

`symbols/mw_dwarfdump.nothpp` (105 MB) es el volcado DWARF-1 COMPLETO del ELF de
GameCube, ya renderizado por `dtk dwarf dump`. Trae, por funcion, el rango, las
locales con su REGISTRO o su hueco de pila, los tamanos de array y **los inlines
que llama con su rango**; y por tipo, los miembros con su offset.

Es la misma informacion que saca `scripts/dwarf1.py`, pero aquel parsea 92 MB de
`.debug` en cada consulta (~30 s) y esto es un grep (~1 s). Usa dwarf1.py solo
cuando necesites algo que el volcado no imprima.

    python scripts/orac.py fn <nombre>        ficha de una funcion (o varias)
    python scripts/orac.py tipo <nombre>      miembros de una struct/clase
    python scripts/orac.py grep <patron>      busqueda cruda con contexto
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DUMP = os.path.join(ROOT, 'symbols', 'mw_dwarfdump.nothpp')
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass


def lineas():
    with open(DUMP, encoding='utf-8', errors='replace') as f:
        for n, l in enumerate(f, 1):
            yield n, l.rstrip('\n')


def bloque(todas, i):
    """Desde la cabecera de una definicion hasta su `}` de cierre, con los
    comentarios de rango que la preceden."""
    ini = i
    while ini > 0 and (todas[ini - 1].startswith('//') or todas[ini - 1].startswith('// ')):
        ini -= 1
    prof, j = 0, i
    while j < len(todas):
        prof += todas[j].count('{') - todas[j].count('}')
        if prof <= 0 and j > i:
            break
        if prof == 0 and j == i and '{' not in todas[j]:
            break
        j += 1
    return ini, min(j + 1, len(todas))


def busca(patron, modo):
    todas = [l.rstrip('\n') for l in open(DUMP, encoding='utf-8', errors='replace')]
    if modo == 'fn':
        # el nombre seguido de '(' y con cuerpo: definicion de funcion
        rx = re.compile(r'(^|[\s*&:~])' + re.escape(patron) + r'\s*\(')
        cand = [i for i, l in enumerate(todas)
                if rx.search(l) and l.rstrip().endswith('{') and not l.lstrip().startswith('//')]
    elif modo == 'tipo':
        rx = re.compile(r'^(struct|class|union|enum)\s+' + re.escape(patron) + r'\b')
        cand = [i for i, l in enumerate(todas) if rx.match(l)]
    else:
        rx = re.compile(patron)
        cand = [i for i, l in enumerate(todas) if rx.search(l)]
    return todas, cand


def main():
    if len(sys.argv) < 3 or not os.path.exists(DUMP):
        print(__doc__ if os.path.exists(DUMP) else 'falta ' + DUMP)
        return 1
    modo, patron = sys.argv[1], sys.argv[2]
    todas, cand = busca(patron, modo)
    if not cand:
        print('sin resultados para %r' % patron)
        return 1
    if modo == 'grep':
        for i in cand[:60]:
            print('%7d  %s' % (i + 1, todas[i]))
        if len(cand) > 60:
            print('... y %d mas' % (len(cand) - 60))
        return 0
    print('%d definicion(es)' % len(cand))
    for i in cand[:12]:
        a, b = bloque(todas, i)
        print('=' * 74)
        print('linea %d de %s' % (a + 1, os.path.relpath(DUMP, ROOT)))
        for l in todas[a:b]:
            print(l)
    if len(cand) > 12:
        print('... y %d definicion(es) mas' % (len(cand) - 12))
    return 0


if __name__ == '__main__':
    sys.exit(main())
