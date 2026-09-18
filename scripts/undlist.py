#!/usr/bin/env python3
"""undlist.py -- que simbolos INDEFINIDOS impiden promocionar una unidad.

`trypromo.py` dice `ENLACE FALLA` y trunca el mensaje. Esto enlaza igual pero
saca los `L0039` agrupados por simbolo, con cuantas veces sale cada uno y desde
que objeto se referencia -- que es justo lo que hace falta para repartir el
trabajo.

Recuerda que el `L0019: ... multiply defined` es un AVISO y sale en casi todas:
el error de verdad es el `L0039`.

    python scripts/undlist.py zAI zCamera
    python scripts/undlist.py --sl            # todas las SourceLists que fallan
"""
import os
import re
import subprocess
import sys
import types
import collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
src = open('scripts/trypromo.py', encoding='utf-8').read().split('def main()')[0]
tp = types.ModuleType('tp')
tp.__dict__['__file__'] = os.path.abspath('scripts/trypromo.py')
exec(compile(src, 'trypromo.py', 'exec'), tp.__dict__)

RE_UND = re.compile(r'Reference to undefined symbol (\S+)(?: in file (\S+))?')


def undefinidos(u, base):
    o, n = tp.resolver(u, base)
    if o is None or not os.path.exists(n):
        return None
    rsp = os.path.join(tp.TMP, 'u.rsp')
    elf = os.path.join(tp.TMP, 'u.elf')
    open(rsp, 'w').write('\n'.join(n if x == o else x for x in base) + '\n')
    if os.path.exists(elf):
        os.remove(elf)
    r = subprocess.run([tp.LD] + tp.LDFLAGS.split() + ['-T', 'config/GOWE69/ldscript.ld', '-o', elf, '@' + rsp],
                       capture_output=True, text=True)
    if os.path.exists(elf):
        return []                       # enlaza: el bloqueo es otro
    c = collections.Counter()
    origen = {}
    for l in (r.stdout + r.stderr).splitlines():
        m = RE_UND.search(l)
        if m:
            c[m.group(1)] += 1
            if m.group(2):
                origen.setdefault(m.group(1), set()).add(os.path.basename(m.group(2)))
    return [(n_, v, sorted(origen.get(n_, ()))) for n_, v in c.most_common()]


def main():
    base = tp.objetos_del_enlace()
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if '--sl' in sys.argv:
        args = sorted({x.replace(os.sep, '/').split('/SourceLists/')[1][:-2]
                       for x in base if '/SourceLists/' in x.replace(os.sep, '/')})
    total = 0
    for u in args:
        r = undefinidos(u, base)
        if r is None:
            print('%-14s no esta en el enlace' % u)
            continue
        if not r:
            continue                    # enlaza
        n_err = sum(v for _, v, _ in r)
        total += len(r)
        print('== %-12s %d errores, %d simbolos distintos' % (u, n_err, len(r)))
        for nom, v, orig in r[:12]:
            print('   %4dx  %-46s %s' % (v, nom[:46], ', '.join(orig[:3])))
        if len(r) > 12:
            print('   ... y %d simbolos mas' % (len(r) - 12))
    print()
    print('total de simbolos distintos por definir: %d' % total)


if __name__ == '__main__':
    main()
