#!/usr/bin/env python3
"""fnobj.py -- compara dos objetos FUNCION A FUNCION, sin el ruido de los bl.

Quitar una funcion de .text (p.ej. un operator new de clase que el original no
emite) desplaza todas las de detras, y los `bl` que el ensamblador resuelve
dentro de la misma seccion cambian de desplazamiento sin que cambie nada del
codigo. Aqui se enmascara el campo de 24 bits de cada `b`/`bl` (opcode 18) y
se comparan los cuerpos por nombre de simbolo. Con --orig se compara ademas
cada funcion que cambia contra el objeto ORIGINAL de la unidad.

    python tools/fnobj.py <antes.o> <ahora.o> [--orig <unidad>]
"""
import os
import struct
import sys

from elftools.elf.elffile import ELFFile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def funcs(p):
    e = ELFFile(open(p, 'rb'))
    idx = [i for i, s in enumerate(e.iter_sections()) if s.name == '.text'][0]
    d = bytearray(e.get_section_by_name('.text').data())
    # Los campos reubicados: el ensamblador deja en ellos el desplazamiento
    # dentro de la seccion destino, que cambia si se inserta una cadena antes.
    rel = e.get_section_by_name('.rela.text')
    if rel is not None:
        for r in rel.iter_relocations():
            off, tipo = r['r_offset'], r['r_info_type']
            if tipo in (4, 5, 6, 109):          # ADDR16_LO / _HI / _HA / EMB_SDA21
                d[off:off + 2] = b'\0\0'
            elif tipo == 10:                    # REL24
                w = struct.unpack('>I', d[off:off + 4])[0]
                d[off:off + 4] = struct.pack('>I', w & 0xFC000003)
            elif tipo == 11:                    # REL14 (bc a otro punto de la seccion)
                w = struct.unpack('>I', d[off:off + 4])[0]
                d[off:off + 4] = struct.pack('>I', w & 0xFFFF0003)
            elif tipo == 1:                     # ADDR32
                d[off:off + 4] = b'\0\0\0\0'
    d = bytes(d)
    out = {}
    for s in e.get_section_by_name('.symtab').iter_symbols():
        if s['st_info']['type'] == 'STT_FUNC' and s['st_shndx'] == idx and s['st_size']:
            cuerpo = bytearray(d[s['st_value']:s['st_value'] + s['st_size']])
            for i in range(0, len(cuerpo) - 3, 4):
                w = struct.unpack('>I', cuerpo[i:i + 4])[0]
                if w >> 26 == 18:
                    cuerpo[i:i + 4] = struct.pack('>I', w & 0xFC000003)
            out[s.name] = bytes(cuerpo)
    return out


def main():
    a, b = funcs(sys.argv[1]), funcs(sys.argv[2])
    o = None
    if '--orig' in sys.argv:
        u = sys.argv[sys.argv.index('--orig') + 1]
        o = funcs(os.path.join(ROOT, 'build', 'GOWE69', 'obj', 'Speed', 'Indep', 'SourceLists', u + '.o'))
    for n in sorted(set(a) - set(b)):
        print('se va:   %-60s %s' % (n, '' if o is None else ('(el original NO la tiene)' if n not in o else '(el original SI la tiene)')))
    for n in sorted(set(b) - set(a)):
        print('aparece: %-60s %s' % (n, '' if o is None else ('(el original SI la tiene)' if n in o else '(el original NO la tiene)')))
    dif = sorted(n for n in set(a) & set(b) if a[n] != b[n])
    for n in dif:
        extra = ''
        if o is not None and n in o:
            extra = 'antes %s al original, ahora %s' % ('IGUAL' if a[n] == o[n] else 'distinta', 'IGUAL' if b[n] == o[n] else 'distinta')
        print('cambia:  %-60s %d -> %d B  %s' % (n, len(a[n]), len(b[n]), extra))
    print('%d funciones comparadas, %d cambian, %d se van, %d aparecen'
          % (len(set(a) & set(b)), len(dif), len(set(a) - set(b)), len(set(b) - set(a))))


if __name__ == '__main__':
    main()
