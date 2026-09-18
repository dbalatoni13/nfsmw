#!/usr/bin/env python3
"""extrasym.py -- QUE simbolos emitimos que el objeto EXTRAIDO no tiene, y de que son.

`seccdiff.py` dice CUANTOS bytes sobran; esto dice DE QUE son, y los parte en las
dos familias que hay que distinguir:

  A) el simbolo SI existe en `orig/GOWE69/NFSMWRELEASE.ELF`, en el rango de OTRA
     unidad. Es una instanciacion de plantilla (o un inline fuera de linea) que
     el original emitio UNA sola vez -- en la PRIMERA unidad del orden de enlace
     que la usa, comprobado en el 93,8 % de los casos-- y que nosotros emitimos
     en hasta 24 unidades. `ngcld -strip-unused-data` tira las copias sobrantes
     al enlazar, asi que estos bytes son INOCUOS.

  B) el simbolo NO existe en NINGUNA parte del original. Eso si es codigo que el
     original no tiene: accesores en clase de clases polimorficas, plantillas
     que el original inlino siempre (`Attrib::TAttrib<T>::Get`: el original solo
     define TRES, nosotros hasta 23 tipos por unidad), y funciones muertas.

Y separa SOURCELIST de BIBLIOTECA: en una biblioteca el objeto extraido solo
trae lo que quedo en el DOL, asi que el exceso es esperado (probado: `mtx.c`
promociona con +5.376 B y el DOL casa byte a byte).

    python scripts/extrasym.py                       # todas las SourceLists
    python scripts/extrasym.py zLua zPhysics -v      # con los simbolos gordos
    python scripts/extrasym.py --lib                 # bibliotecas
"""
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


class Elf:
    def __init__(self, path):
        self.d = d = open(path, 'rb').read()
        if d[:4] != b'\x7fELF':
            raise ValueError('no es ELF')
        shoff = struct.unpack('>I', d[0x20:0x24])[0]
        shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
        raw = [struct.unpack('>10I', d[shoff + i * shent:shoff + i * shent + 40])
               for i in range(shnum)]
        so = raw[shstr][4]

        def nm(o):
            e = d.index(b'\0', so + o)
            return d[so + o:e].decode('utf-8', 'replace')
        self.sh = [{'name': nm(r[0]), 'type': r[1], 'off': r[4], 'size': r[5],
                    'link': r[6]} for r in raw]

    def syms(self):
        d = self.d
        for s in self.sh:
            if s['type'] != 2:
                continue
            st = self.sh[s['link']]['off']
            for i in range(s['size'] // 16):
                o = s['off'] + i * 16
                nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', d[o:o + 16])
                if not nameo:
                    continue
                e = d.index(b'\0', st + nameo)
                yield {'name': d[st + nameo:e].decode('utf-8', 'replace'), 'value': val,
                       'size': size, 'bind': info >> 4, 'type': info & 0xF, 'shndx': shndx}

    def code_idx(self):
        return set(i for i, s in enumerate(self.sh)
                   if s['name'] in ('.text', '.over') or s['name'].startswith('.gnu.linkonce.t'))

    def funcs(self):
        ci = self.code_idx()
        return {s['name']: s for s in self.syms() if s['type'] == 2 and s['shndx'] in ci}


def main():
    if '--help' in sys.argv or '-h' in sys.argv:
        print(__doc__)
        return 0
    verbose = '-v' in sys.argv
    lib = '--lib' in sys.argv
    want = [a for a in sys.argv[1:] if not a.startswith('-')]

    orig = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
    if not os.path.exists(orig):
        print('falta %s: sin el ELF original no se pueden separar las dos familias' % orig)
        return 1
    O = set(s['name'] for s in Elf(orig).syms()
            if s['type'] == 2 and s['shndx'] not in (0, 65521))

    base = os.path.join(ROOT, 'build', 'GOWE69', 'src')
    unidades = []
    for dp, dn, fn in os.walk(base):
        for f in fn:
            if not f.endswith('.o'):
                continue
            rel = os.path.relpath(os.path.join(dp, f), base)[:-2].replace(os.sep, '/')
            sl = '/SourceLists/' in rel
            if lib == sl:
                continue
            if want and not any(w == rel or w == rel.split('/')[-1] or w in rel for w in want):
                continue
            unidades.append(rel)

    tA = tB = nA = nB = 0
    print('%-26s %6s %9s | %6s %9s | %6s %9s'
          % ('unidad', 'sobran', 'B', 'A:otra', 'B', 'B:no ex', 'B'))
    for rel in sorted(unidades):
        ours = os.path.join(base, rel.replace('/', os.sep) + '.o')
        targ = os.path.join(ROOT, 'build', 'GOWE69', 'obj', rel.replace('/', os.sep) + '.o')
        if not os.path.exists(targ):
            continue
        try:
            a, b = Elf(ours).funcs(), Elf(targ).funcs()
        except Exception:
            continue
        sob = {k: v for k, v in a.items() if k not in b}
        if not sob:
            continue
        A = {k: v for k, v in sob.items() if k in O}
        B = {k: v for k, v in sob.items() if k not in O}
        sa, sb = sum(v['size'] for v in A.values()), sum(v['size'] for v in B.values())
        tA += sa; tB += sb; nA += len(A); nB += len(B)
        print('%-26s %6d %9d | %6d %9d | %6d %9d'
              % (rel.split('/')[-1][:26], len(sob), sa + sb, len(A), sa, len(B), sb))
        if verbose:
            for k, v in sorted(B.items(), key=lambda x: -x[1]['size'])[:8]:
                print('        B %6d B %-5s %s' % (v['size'],
                      {0: 'local', 1: 'glob', 2: 'WEAK'}.get(v['bind']), k[:76]))
            for k, v in sorted(A.items(), key=lambda x: -x[1]['size'])[:4]:
                print('        A %6d B %-5s %s' % (v['size'],
                      {0: 'local', 1: 'glob', 2: 'WEAK'}.get(v['bind']), k[:76]))
    print()
    print('A) existe en el original en OTRA unidad : %5d simbolos %8d B  <- inocuo, ngcld lo tira'
          % (nA, tA))
    print('B) NO existe en ninguna parte           : %5d simbolos %8d B  <- codigo que el original no tiene'
          % (nB, tB))
    return 0


if __name__ == '__main__':
    sys.exit(main())
