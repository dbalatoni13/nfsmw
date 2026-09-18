#!/usr/bin/env python3
"""scratchclean.py -- purga artefactos regenerables de scratchpad/.

El disco lleno se lee como "la variante no cambia nada" (medidas falsas).
Este script borra SOLO artefactos de build/volcado regenerables
(.json/.s/.rtl/.o/.elf/.dol/.greg/.lreg/.i grandes) y NUNCA:
  - fuentes, scripts, .md/.txt, .cpp/.c/.h
  - rutas citadas en src/, docs/ o scripts/ (evidencia de vedas)

Uso:
  python scripts/scratchclean.py            # dry-run (no borra)
  python scripts/scratchclean.py --go       # borra
"""
import os
import re
import glob
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GO = '--go' in sys.argv

refs = set()
for pat in ['src/**/*.cpp', 'src/**/*.h', 'src/**/*.hpp', 'src/**/*.c',
            'docs/**/*.md', 'scripts/*.py']:
    for p in glob.glob(os.path.join(ROOT, pat), recursive=True):
        try:
            txt = open(p, encoding='utf-8', errors='replace').read()
        except Exception:
            continue
        for m in re.finditer(r'scratchpad/[A-Za-z0-9_\-/\.]+', txt):
            refs.add(m.group(0).rstrip('./:;,)"').replace('/', os.sep))
dirrefs = sorted({r for r in refs if '.' not in os.path.basename(r)})

DEL_EXTS = {'.json': 5e6, '.s': 5e6, '.rtl': 1e6, '.o': 2e6, '.elf': 5e6,
            '.dol': 5e6, '.greg': 1e6, '.lreg': 1e6, '.i': 10e6}
freed = n = 0
biggest = []
for root, _, fs in os.walk(os.path.join(ROOT, 'scratchpad')):
    for f in fs:
        p = os.path.join(root, f)
        try:
            s = os.path.getsize(p)
        except Exception:
            continue
        ext = os.path.splitext(f)[1].lower()
        if ext in DEL_EXTS and s >= DEL_EXTS[ext]:
            rel = os.path.relpath(p, ROOT)
            if rel in refs:
                continue
            if any(rel == d or rel.startswith(d + os.sep) for d in dirrefs):
                continue
            biggest.append((s, rel))
            if GO:
                os.remove(p)
                freed += s
                n += 1
biggest.sort(reverse=True)
print('%s %d ficheros, %.1f GB' % ('borrados' if GO else 'candidatos:',
                                   n if GO else len(biggest),
                                   (freed if GO else sum(s for s, _ in biggest)) / 1e9))
if not GO:
    for s, r in biggest[:15]:
        print('  %7.1fMB %s' % (s / 1e6, r))
    print('repite con --go para borrar')
