#!/usr/bin/env python3
"""Cambia las referencias `extern const float lbl_XXXX` por el literal que valen.

El truco de declarar la constante agrupada como `extern const float` fuerza la
referencia al simbolo exacto, pero **cambia el reparto de registros**: GCC crea
el pseudo en otro punto que con un literal. Sintoma: misma cuenta de
instrucciones, mismos opcodes, solo cambian los numeros de registro. Y el truco
no hacia falta, porque con `function_reloc_diffs=none` el `lfs` contra el
literal agrupado casa igual que contra el simbolo.

Ojo: el literal casa donde el original TENIA un literal. Donde tenia otra cosa,
empeora. Hay que medir fichero a fichero (y aun asi puede repartirse en las dos
direcciones dentro del mismo fichero).

  python scripts/pool2lit.py <fichero.cpp>            # solo enseña el mapa
  python scripts/pool2lit.py <fichero.cpp> --apply    # lo escribe
"""
import io
import os
import re
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
MAGIC = bytes.fromhex('4330000000000000')
RE_DECL = re.compile(r'extern\s+const\s+(?:volatile\s+)?(float|double)\s+(lbl_[0-9A-Fa-f]+)\s*;')


def segments():
    d = open(ELF, 'rb').read()
    e = '>' if d[5] == 2 else '<'
    phoff, = struct.unpack(e + 'I', d[28:32])
    phentsize, = struct.unpack(e + 'H', d[42:44])
    phnum, = struct.unpack(e + 'H', d[44:46])
    segs = []
    for i in range(phnum):
        o = phoff + i * phentsize
        ptype, off, vaddr, paddr, filesz = struct.unpack(e + '5I', d[o:o + 20])
        if ptype == 1:
            segs.append((vaddr, off, filesz))
    return d, segs


def read(d, segs, addr, n):
    for v, o, fs in segs:
        if v <= addr < v + fs:
            return d[o + (addr - v):o + (addr - v) + n]
    return None


def literal(kind, raw):
    if raw is None:
        return None
    if kind == 'float':
        v, = struct.unpack('>f', raw[:4])
        # el patron de bits manda: se busca el texto mas corto que lo reproduce
        for txt in ('%g' % v, '%.7g' % v, '%.9g' % v, repr(v)):
            try:
                if struct.pack('>f', float(txt)) == raw[:4]:
                    return txt + 'f' if ('.' in txt or 'e' in txt or 'E' in txt) else txt + '.0f'
            except ValueError:
                pass
        return None
    v, = struct.unpack('>d', raw[:8])
    for txt in ('%g' % v, '%.17g' % v, repr(v)):
        try:
            if struct.pack('>d', float(txt)) == raw[:8]:
                return txt if ('.' in txt or 'e' in txt) else txt + '.0'
        except ValueError:
            pass
    return None


def main():
    path = sys.argv[1]
    apply_it = '--apply' in sys.argv
    s = io.open(path, encoding='utf-8', newline='').read()
    d, segs = segments()
    mapping = {}
    for kind, name in RE_DECL.findall(s):
        addr = int(name[4:], 16)
        raw = read(d, segs, addr, 8)
        if kind == 'double' and raw and raw[:8] == MAGIC:
            # 0x4330000000000000 no es una constante de la fuente: la emite GCC
            # para convertir int a float. Sustituirla por un literal es un error.
            continue
        lit = literal(kind, raw)
        if lit:
            mapping[name] = lit
    for k in sorted(mapping):
        print('%s = %s' % (k, mapping[k]))
    if not apply_it:
        print('\n(%d constantes; --apply para escribirlas)' % len(mapping))
        return
    out, n = [], 0
    for line in s.split('\n'):
        if line.lstrip().startswith('extern const'):
            out.append(line)
            continue
        orig = line
        # nunca dentro de una cadena: el asm inline referencia estas etiquetas
        # por nombre, y sustituirlas ahi rompe el ensamblado.
        parts = re.split(r'("(?:[^"\\]|\\.)*")', line)
        for i in range(0, len(parts), 2):
            for k, v in mapping.items():
                parts[i] = re.sub(r'\b%s\b' % k, v, parts[i])
        line = ''.join(parts)
        if line != orig:
            n += 1
        out.append(line)
    io.open(path, 'w', encoding='utf-8', newline='').write('\n'.join(out))
    print('\n%d lineas reescritas en %s' % (n, os.path.basename(path)))


if __name__ == '__main__':
    main()
