#!/usr/bin/env python3
"""vtord.py -- el ORDEN DE EMISION de las vtables de una unidad, lado a lado.

`permorden.py` dice que funciones salen fuera de sitio; esto dice POR QUE. El
orden del bloque de plantillas de `.text` es una CONSECUENCIA del orden en que
`finish_file` escribe las vtables:

  decl2.c:3708  walk_globals (vtable_decl_p, finish_vtable_vardecl)
  decl.c:2012   walk_globals_r recorre NAMESPACE_LEVEL(ns)->names, que pushdecl
                construye ANTEPONIENDO: la cadena es el orden INVERSO de
                finish_struct_1 (class.c:769 / :914 pushdecl_top_level).
  decl2.c:2760  finish_vtable_vardecl escribe la vtable solo si
                TREE_SYMBOL_REFERENCED(su nombre) ya esta puesto --para una
                instanciacion de plantilla import_export_vtable (decl2.c:2609)
                NO pone DECL_INTERFACE_KNOWN--, o sea que la vuelta del bucle
                do/while de finish_file en que sale depende de quien la haya
                nombrado antes.
  decl2.c:2507  mark_vtable_entries hace mark_used de cada entrada EN ORDEN DE
                RANURA, y eso es lo que instancia los cuerpos.

Asi que dos objetos con el mismo codigo y distinto orden de `.text` se separan
aqui, y la lista de este script dice en que clase exactamente.

    python scripts/vtord.py zSim
    python scripts/vtord.py Speed/Indep/SourceLists/zMain
    python scripts/vtord.py --solo-distintas zLua
"""
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load(path):
    d = open(path, 'rb').read()
    if d[:4] != b'\x7fELF':
        raise ValueError('no es ELF: ' + path)
    shoff = struct.unpack('>I', d[0x20:0x24])[0]
    shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
    raw = [struct.unpack('>10I', d[shoff + i * shent:shoff + i * shent + 40])
           for i in range(shnum)]
    so = raw[shstr][4]

    def nm(o):
        e = d.index(b'\0', so + o)
        return d[so + o:e].decode('utf-8', 'replace')
    sh = [{'name': nm(r[0]), 'type': r[1], 'off': r[4], 'size': r[5], 'link': r[6]}
          for r in raw]
    return d, sh


def vtables(path):
    """-> [(offset, tam, nombre)] de los simbolos _vt. en orden de emision."""
    d, sh = load(path)
    out = []
    for s in sh:
        if s['type'] != 2:
            continue
        st = sh[s['link']]['off']
        for i in range(s['size'] // 16):
            o = s['off'] + i * 16
            nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', d[o:o + 16])
            if not nameo or not shndx or shndx >= 0xff00:
                continue
            e = d.index(b'\0', st + nameo)
            n = d[st + nameo:e].decode('utf-8', 'replace')
            if n.startswith('_vt.'):
                out.append((sh[shndx]['name'], val, size, n[4:]))
    out.sort()
    return [(v, sz, n) for _, v, sz, n in out]


def main():
    a = [x for x in sys.argv[1:] if not x.startswith('--')]
    solo = '--solo-distintas' in sys.argv
    u = a[0] if a else 'zSim'
    if '/' not in u:
        u = 'Speed/Indep/SourceLists/' + u
    obj = os.path.join(ROOT, 'build', 'GOWE69', 'obj', *u.split('/')) + '.o'
    nue = os.path.join(ROOT, 'build', 'GOWE69', 'src', *u.split('/')) + '.o'
    for p in (obj, nue):
        if not os.path.exists(p):
            sys.exit('falta ' + p)
    A, B = vtables(obj), vtables(nue)
    na = [n for _, _, n in A]
    nb = [n for _, _, n in B]
    print('%s: objetivo %d vtables, nuestro %d' % (u.split('/')[-1], len(A), len(B)))
    fuera = [n for n in nb if n not in na]
    if fuera:
        print('SOLO NUESTRAS (%d): %s' % (len(fuera), ', '.join(x[:40] for x in fuera[:6])))
    print()
    print('%-4s %-46s | %-46s' % ('#', 'OBJETIVO', 'NUESTRO'))
    dist = 0
    for i in range(max(len(A), len(B))):
        x = A[i] if i < len(A) else (0, 0, '')
        y = B[i] if i < len(B) else (0, 0, '')
        m = '' if x[2] == y[2] else ' <<<'
        if m:
            dist += 1
        if solo and not m:
            continue
        print('%-4d %06X %4d %-38s | %06X %4d %-38s%s'
              % (i, x[0], x[1], x[2][:38], y[0], y[1], y[2][:38], m))
    print()
    print('posiciones que no casan: %d de %d' % (dist, max(len(A), len(B))))


main()
