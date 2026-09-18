#!/usr/bin/env python3
"""auditaportable.py -- lo que impide que este codigo sea decompilacion portable.

`censoasm.py` cuenta el asm. Esto cuenta LO DEMAS: los constructos que o bien no
compilan fuera de GameCube, o bien son de la lista que el repo oficial prohibe.
Los comentarios se quitan antes de contar (si no, cada nota de diagnostico suma).

Clases:
  ASM_GNU      asm / __asm__ en cualquier forma          -> no compila en MSVC
  ATRIB_GNU    __attribute__((...))                      -> no compila en MSVC
  EXPR_SENT    expresion-sentencia ({ ... })             -> extension de GCC
  TYPEOF       typeof / __typeof__                       -> extension de GCC
  BUILTIN      __builtin_*                               -> especifico del compilador
  OFFSET       *(T*)((char*)p + N) y reinterpret_cast a
               puntero-a-puntero sobre &obj              -> "struct members by
                                                            pointer offsets"
  CAST_VANO    static_cast<T>(x) donde x ya se declara T -> "casting variables to
                                                            the type they already are"

OFFSET y CAST_VANO son HEURISTICAS: senalan candidatos para mirar a mano, no
veredictos. Las otras cinco son exactas.

    python scripts/auditaportable.py              resumen + los 40 primeros
    python scripts/auditaportable.py --clase X    solo una clase, con detalle
    python scripts/auditaportable.py --json S     vuelca todo a S
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'src')
EXTS = ('.c', '.cpp', '.cc', '.h', '.hpp', '.inl', '.cxx')
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass


def sin_comentarios(t):
    """Quita comentarios respetando cadenas y caracteres. Conserva las lineas
    (mete '\\n' por cada salto comido) para que los numeros sigan valiendo."""
    out, i, n = [], 0, len(t)
    while i < n:
        c = t[i]
        if c == '/' and i + 1 < n and t[i + 1] == '/':
            j = t.find('\n', i)
            i = n if j < 0 else j
            continue
        if c == '/' and i + 1 < n and t[i + 1] == '*':
            j = t.find('*/', i + 2)
            if j < 0:
                break
            out.append('\n' * t.count('\n', i, j))
            i = j + 2
            continue
        if c in '"\'':
            cierre, j = c, i + 1
            while j < n:
                if t[j] == '\\':
                    j += 2
                    continue
                if t[j] == cierre:
                    break
                j += 1
            out.append(t[i:j + 1])
            i = j + 1
            continue
        out.append(c)
        i += 1
    return ''.join(out)


REGLAS = [
    ('ASM_GNU', re.compile(r'(?<![\w.])(?:__asm__|__asm|\basm)\s*(?:__volatile__|volatile)?\s*\(')),
    ('ATRIB_GNU', re.compile(r'__attribute__\s*\(\(')),
    ('EXPR_SENT', re.compile(r'=\s*\(\s*\{')),
    ('TYPEOF', re.compile(r'(?<![\w])(?:__typeof__|typeof)\s*\(')),
    ('BUILTIN', re.compile(r'__builtin_\w+')),
    ('OFFSET', re.compile(r'\*\s*\(\s*\w[\w:<>\s]*\*\s*\)\s*\(\s*\(\s*(?:char|unsigned char|u8|s8)\s*\*\s*\)'
                          r'|reinterpret_cast\s*<\s*[\w:]+\s*\*\s*\*\s*>\s*\(\s*&')),
]

CAST = re.compile(r'(?:static_cast|reinterpret_cast|const_cast)\s*<\s*([\w:]+(?:\s*\*)?)\s*>\s*\(\s*(\w+)\s*\)')


def declarado_como(texto, var):
    """Tipo con el que se declara `var` en este fichero, si es inequivoco."""
    m = re.findall(r'(?<![\w>])([\w:]+(?:\s*\*)?)\s+' + re.escape(var) + r'\s*(?:[;=,)])', texto)
    tipos = set(x.replace(' ', '') for x in m)
    return tipos.pop() if len(tipos) == 1 else None


def main():
    clase = None
    salida = None
    if '--clase' in sys.argv:
        clase = sys.argv[sys.argv.index('--clase') + 1]
    if '--json' in sys.argv:
        salida = sys.argv[sys.argv.index('--json') + 1]

    hallazgos = []
    for raiz, _, nombres in os.walk(SRC):
        for nom in sorted(nombres):
            if not nom.endswith(EXTS):
                continue
            ruta = os.path.join(raiz, nom)
            rel = os.path.relpath(ruta, ROOT).replace(os.sep, '/')
            bruto = open(ruta, encoding='utf-8', errors='surrogateescape').read()
            t = sin_comentarios(bruto)
            lineas = t.split('\n')
            for i, l in enumerate(lineas, 1):
                for nombre, rx in REGLAS:
                    if rx.search(l):
                        hallazgos.append({'clase': nombre, 'fichero': rel, 'linea': i,
                                          'texto': l.strip()[:120]})
                for m in CAST.finditer(l):
                    tipo, var = m.group(1).replace(' ', ''), m.group(2)
                    d = declarado_como(t, var)
                    if d and d == tipo:
                        hallazgos.append({'clase': 'CAST_VANO', 'fichero': rel, 'linea': i,
                                          'texto': l.strip()[:120]})

    import collections
    por = collections.Counter(h['clase'] for h in hallazgos)
    fich = collections.Counter()
    for h in hallazgos:
        fich[(h['clase'], h['fichero'])] += 1
    print('== LO QUE NO ES DECOMPILACION PORTABLE (comentarios ya quitados) ==')
    for c, n in por.most_common():
        nf = len(set(f for (cc, f) in fich if cc == c))
        print('  %-11s %5d en %4d ficheros' % (c, n, nf))
    print('  %-11s %5d' % ('TOTAL', len(hallazgos)))
    if salida:
        json.dump(hallazgos, open(salida, 'w'), indent=1)
        print('\nvolcado en %s' % salida)
    sel = [h for h in hallazgos if not clase or h['clase'] == clase]
    if clase:
        print('\n== %s (%d) ==' % (clase, len(sel)))
        for h in sel[:200]:
            print('%-62s:%-5d %s' % (h['fichero'][4:][-62:], h['linea'], h['texto']))
    return 0


if __name__ == '__main__':
    sys.exit(main())
