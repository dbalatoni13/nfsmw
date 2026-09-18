#!/usr/bin/env python3
"""seccdiff.py -- el tamano de CADA seccion nuestra contra el objeto EXTRAIDO.

Es la medida que objdiff no da: objdiff empareja simbolos por nombre, asi que
todo lo que emitimos de mas no se compara y no penaliza. Una unidad puede leer
100 % con miles de bytes que el original no tiene.

    python scripts/seccdiff.py                # resumen + top 20 de cada familia
    python scripts/seccdiff.py --all          # todas las filas
    python scripts/seccdiff.py --sl           # solo SourceLists
    python scripts/seccdiff.py --lib          # solo bibliotecas / middleware

SEPARA SOURCELIST DE BIBLIOTECA, y esa distincion es la mitad del valor:
el objeto extraido solo contiene lo que quedo en el DOL, mientras que nosotros
compilamos la unidad entera.

LO QUE SE MIDIO EN LA r28, y corrige la lectura de la r27:

  * El exceso de `.text` NO bloquea la promocion, ni en biblioteca ni en
    SourceList. El enlace real lleva `-strip-unused-data` y `ngcld` TIRA los
    cuerpos weak duplicados: sustituyendo `zLua.o` (con +18.908 B) el `.text`
    enlazado sale en 3.804.440 B, EXACTAMENTE el del original. Y `zMiscSmall`
    con +3.996 B y con +0 B da el MISMO sha1 de DOL.
  * Lo que si desplaza el DOL son `.rodata`, `.data` y `.sdata`, y ahi la
    diferencia suele ser NEGATIVA (nos FALTAN constantes). Mira esas columnas.

El juez de una promocion sigue siendo `trypromo.py`, no este guion ni el
porcentaje.
"""
import json
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SECS = ('.text', '.rodata', '.data', '.bss', '.sdata', '.sdata2', '.sbss')


def secsizes(path):
    """{nombre de seccion: tamano} de un ELF32 big-endian, sin dependencias."""
    d = open(path, 'rb').read()
    if d[:4] != b'\x7fELF':
        raise ValueError('no es ELF')
    shoff = struct.unpack('>I', d[0x20:0x24])[0]
    shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
    raw = [struct.unpack('>10I', d[shoff + i * shent:shoff + i * shent + 40])
           for i in range(shnum)]
    so = raw[shstr][4]
    out = {}
    for r in raw:
        e = d.index(b'\0', so + r[0])
        out[d[so + r[0]:e].decode('utf-8', 'replace')] = r[5]
    return out


def es_sourcelist(rel):
    return '/SourceLists/' in rel


def recoge():
    pct = {}
    rp = os.path.join(ROOT, 'build', 'GOWE69', 'report.json')
    if os.path.exists(rp):
        try:
            for u in json.load(open(rp))['units']:
                pct[u['name'].split('/', 1)[1]] = u.get('measures', {}).get('matched_code_percent', 0)
        except Exception:
            pass
    base = os.path.join(ROOT, 'build', 'GOWE69', 'src')
    rows = []
    for dp, dn, fn in os.walk(base):
        for f in fn:
            if not f.endswith('.o'):
                continue
            rel = os.path.relpath(os.path.join(dp, f), base)[:-2].replace(os.sep, '/')
            targ = os.path.join(ROOT, 'build', 'GOWE69', 'obj', rel.replace('/', os.sep)) + '.o'
            if not os.path.exists(targ):
                continue
            try:
                a, b = secsizes(os.path.join(dp, f)), secsizes(targ)
            except Exception:
                continue
            # zFeOverlay/zOnline renombran .text a .over
            def g(m, n):
                v = m.get(n, 0)
                if n == '.text':
                    v += m.get('.over', 0)
                    v += sum(s for k, s in m.items() if k.startswith('.gnu.linkonce.t'))
                if n == '.rodata':
                    v += sum(s for k, s in m.items() if k.startswith('.gnu.linkonce.r'))
                return v
            d = {n: (g(a, n), g(b, n)) for n in SECS}
            if all(x == y for x, y in d.values()):
                continue
            rows.append({'rel': rel, 'sl': es_sourcelist(rel), 'd': d,
                         'dt': d['.text'][0] - d['.text'][1],
                         'pct': pct.get(rel, -1)})
    return rows


def tabla(sel, titulo, n):
    sel = sorted(sel, key=lambda r: -r['dt'])
    pos = sum(r['dt'] for r in sel if r['dt'] > 0)
    print('\n== %s ==\n   %d unidades con alguna seccion distinta; .text de mas: %d B'
          % (titulo, len(sel), pos))
    print('%9s %9s %9s %9s %8s  unidad' % ('d.text', 'd.rodata', 'd.data', 'd.bss', '%'))
    for r in sel[:n]:
        d = r['d']
        print('%+9d %+9d %+9d %+9d %8s  %s'
              % (r['dt'], d['.rodata'][0] - d['.rodata'][1],
                 d['.data'][0] - d['.data'][1], d['.bss'][0] - d['.bss'][1],
                 ('%.2f' % r['pct']) if r['pct'] >= 0 else '-', r['rel']))
    if len(sel) > n:
        print('   ... y %d mas (--all)' % (len(sel) - n))
    return pos


def main():
    if '--help' in sys.argv or '-h' in sys.argv:
        print(__doc__)
        return 0
    rows = recoge()
    n = 10 ** 9 if '--all' in sys.argv else 20
    sl = [r for r in rows if r['sl']]
    lb = [r for r in rows if not r['sl']]
    a = b = 0
    if '--lib' not in sys.argv:
        a = tabla(sl, 'SOURCELISTS', n)
    if '--sl' not in sys.argv:
        b = tabla(lb, 'BIBLIOTECAS / MIDDLEWARE', n)
    print()
    if '--lib' not in sys.argv:
        print('.text de mas en SOURCELISTS: %8d B en %3d unidades' % (a, len(sl)))
    if '--sl' not in sys.argv:
        print('.text de mas en BIBLIOTECAS: %8d B en %3d unidades' % (b, len(lb)))
    print('MEDIDO en la r28: ese exceso de .text NO llega al enlace')
    print('  (ngcld -strip-unused-data tira los cuerpos weak duplicados).')
    print('  Lo que desplaza el DOL son .rodata/.data/.sdata: mira esas columnas.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
