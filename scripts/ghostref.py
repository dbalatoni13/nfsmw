#!/usr/bin/env python3
"""ghostref.py -- simbolos que referenciamos y que NUNCA existieron en el original.

Sale de un patron que aparecio tres veces seguidas en la r34: una funcion virtual
declarada en una cabecera, **no pura y sin definir en ningun sitio**. En GCC 2.9
eso no es inofensivo: la clase se queda sin "key method", el compilador **no emite
su vtable** y el `_vt.*` queda indefinido. El original las tenia PURAS
(`__pure_virtual` en la ranura), asi que el simbolo no existe alli.

En vez de intentar parsear C++, esto compara los dos lados:

  * los simbolos UNDEFINED de **nuestros** objetos, y
  * el `symtab` completo del ELF original.

Lo que referenciamos y el ELF no tiene **es codigo que nos hemos inventado**: o
una virtual que deberia ser pura, o una declaracion que sobra. Lo contrario --lo
que el ELF tiene y nosotros no definimos-- sale con `--faltan`, y ahi el ruido es
mucho mayor (todo lo que aun no esta escrito).

    python scripts/ghostref.py            # lo que referenciamos y no existio
    python scripts/ghostref.py --vt       # solo las vtables, que es donde duele
"""
import glob
import os
import struct
import sys
import collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')


def tabla(p, quiero_und=False):
    """-> (definidos, indefinidos) del objeto/ELF p."""
    try:
        f = open(p, 'rb').read()
    except OSError:
        return set(), set()
    if f[:4] != b'\x7fELF':
        return set(), set()
    try:
        shoff = struct.unpack('>I', f[0x20:0x24])[0]
        se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
        S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
        stro = S[sx][4]
    except (struct.error, IndexError):
        return set(), set()

    def nm(o):
        return f[stro + o:f.index(b'\0', stro + o)].decode(errors='replace')

    sym = next((s for s in S if nm(s[0]) == '.symtab'), None)
    if sym is None:
        return set(), set()
    st = S[sym[6]][4]
    dfn, und = set(), set()
    for i in range(sym[5] // 16):
        a, v, z, inf, _o, shn = struct.unpack('>IIIBBH', f[sym[4] + i * 16:sym[4] + i * 16 + 16])
        n = f[st + a:f.index(b'\0', st + a)].decode(errors='replace')
        if not n or (inf & 0xF) in (3, 4):        # ni SECTION ni FILE
            continue
        if shn == 0:
            und.add(n)
        else:
            dfn.add(n)
    return dfn, und


def main():
    orig, _ = tabla(ELF)
    print('%d simbolos definidos en el ELF original' % len(orig))
    quien = collections.defaultdict(set)
    ndef = set()
    for p in glob.glob(os.path.join(ROOT, 'build', 'GOWE69', 'src', '**', '*.o'), recursive=True):
        d, u = tabla(p)
        ndef |= d
        for s in u:
            quien[s].add(os.path.basename(p)[:-2])
    print('%d objetos nuestros; %d simbolos indefinidos distintos' % (
        len(glob.glob(os.path.join(ROOT, 'build', 'GOWE69', 'src', '**', '*.o'), recursive=True)),
        len(quien)))
    print()

    # lo que referenciamos y el original NO tenia
    fantasmas = {s: v for s, v in quien.items() if s not in orig}
    solo_vt = '--vt' in sys.argv
    vt = {s: v for s, v in fantasmas.items() if s.startswith('_vt.')}
    otros = {s: v for s, v in fantasmas.items() if not s.startswith('_vt.')}

    print('== VTABLES que referenciamos y el ELF no tiene (%d)' % len(vt))
    print('   Casi siempre: una virtual declarada no-pura y sin definir deja la clase')
    print('   sin key method y GCC 2.9 no emite la vtable. En el original era PURA.')
    for s, v in sorted(vt.items(), key=lambda x: -len(x[1]))[:20]:
        print('   %-56s %s' % (s[:56], ', '.join(sorted(v)[:3])))
    if solo_vt:
        return
    print()
    print('== OTROS simbolos que referenciamos y el ELF no tiene (%d)' % len(otros))
    for s, v in sorted(otros.items(), key=lambda x: -len(x[1]))[:30]:
        # los que nosotros mismos definimos en otra unidad no son fantasmas
        marca = '' if s not in ndef else '  (lo definimos en otra unidad)'
        print('   %-56s %s%s' % (s[:56], ', '.join(sorted(v)[:3]), marca))


if __name__ == '__main__':
    main()
