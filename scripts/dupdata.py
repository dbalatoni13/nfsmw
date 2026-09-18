#!/usr/bin/env python3
"""dupdata.py -- dato ESCRITO A MANO en un asm() que el compilador ya emite solo.

El hallazgo de zDebug (r31): el `asm()` de la SourceList escribe un literal que
`cc1plus` emite **tambien** por su cuenta, y las dos copias sobreviven al enlace.
Borrar la copia a mano dejo el `.text` byte a byte y la unidad PROMOCIONO.

Este guion lo cuenta en las 33. Para cada simbolo de datos que un `asm()` del
fuente define, mira si el objeto NUESTRO tiene otro simbolo distinto, en la
misma seccion, con el MISMO tamano y los MISMOS bytes: eso es el duplicado.

Y de paso marca los simbolos AJENOS: los que la unidad define a mano pero cuya
direccion no cae en ningun rango que `splits.txt` le da (en zDebug,
`kFloatScaleUp_8045B100` era de zDynamics).

    python scripts/dupdata.py                 # las 33
    python scripts/dupdata.py zAnim zLua      # solo esas
    python scripts/dupdata.py --detalle zAnim # simbolo a simbolo
"""
import glob
import io
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA = ('.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss')
RE_ETIQ = re.compile(r'"\s*([A-Za-z_$][A-Za-z0-9_$.]*):')
RE_GLOBL = re.compile(r'\.globl\s+([A-Za-z_$][A-Za-z0-9_$.]*)')
RE_DIR = re.compile(r'_(8[0-9A-Fa-f]{7})$')


def simbolos_asm(path):
    """Nombres de simbolo que los bloques asm() del fuente definen."""
    txt = io.open(path, encoding='utf-8', errors='replace').read()
    nombres = set()
    for m in re.finditer(r'asm\s*\(', txt):
        i = m.end()
        prof, j = 1, i
        while j < len(txt) and prof:
            if txt[j] == '(':
                prof += 1
            elif txt[j] == ')':
                prof -= 1
            j += 1
        bloque = txt[i:j]
        if '.text' in bloque and '.section .bss' not in bloque and '.rodata' not in bloque:
            continue                                   # asm de codigo, no de dato
        nombres |= set(RE_GLOBL.findall(bloque)) | set(RE_ETIQ.findall(bloque))
    return nombres


def objeto(path):
    """[(nombre, seccion, bytes)] de las secciones de datos."""
    try:
        f = open(path, 'rb').read()
    except OSError:
        return None
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]

    def nm(x, b):
        e = f.index(b'\0', b + x)
        return f[b + x:e].decode()

    names = [nm(s[0], stro) for s in S]
    st = [i for i, s in enumerate(S) if s[1] == 2][0]
    strt = S[S[st][6]][4]
    out = []
    for o in range(S[st][4], S[st][4] + S[st][5], 16):
        nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', f[o:o + 16])
        n = nm(nameo, strt)
        if not n or shndx >= len(names) or names[shndx] not in DATA:
            continue
        sec = names[shndx]
        datos = b'' if S[shndx][1] == 8 else f[S[shndx][4] + val:S[shndx][4] + val + size]
        out.append((n, sec, size, datos))
    return out


def rangos_splits():
    d, cur = {}, None
    for l in io.open(os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt'), encoding='utf-8'):
        if not l.startswith('\t') and l.rstrip().endswith(':'):
            cur = re.sub(r'\.(c|cpp|s)$', '', l.rstrip().rstrip(':'))
            d.setdefault(cur, [])
        elif l.startswith('\t') and cur:
            m = re.match(r'\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)', l)
            if m:
                d[cur].append((int(m.group(2), 16), int(m.group(3), 16)))
    return d


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    detalle = '--detalle' in sys.argv
    SP = rangos_splits()
    fuentes = sorted(glob.glob(os.path.join(ROOT, 'src', 'Speed', 'Indep', 'SourceLists', '*.cpp')))
    tot_dup = tot_aj = tot_u = 0
    print('%-16s %6s %8s %8s  %s' % ('unidad', 'a mano', 'DUPLICA', 'AJENOS', 'simbolos'))
    for src in fuentes:
        u = os.path.basename(src)[:-4]
        if args and u not in args:
            continue
        amano = simbolos_asm(src)
        if not amano:
            continue
        obj = objeto(os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists', u + '.o'))
        if obj is None:
            print('%-16s falta el objeto' % u)
            continue
        porsec = {}
        for n, sec, sz, datos in obj:
            porsec.setdefault(sec, []).append((n, sz, datos))
        rangos = SP.get('Speed/Indep/SourceLists/' + u, [])
        dup, ajeno = [], []
        for n, sec, sz, datos in obj:
            if n not in amano or sz == 0:
                continue
            for n2, sz2, d2 in porsec[sec]:
                # el gemelo tiene que ser del COMPILADOR: dos simbolos escritos
                # los dos a mano con los mismos bytes no son un duplicado, son
                # dos constantes iguales del original.
                if n2 != n and n2 not in amano and sz2 == sz and datos and d2 == datos:
                    dup.append((n, n2, sz))
                    break
            m = RE_DIR.search(n)
            if m:
                a = int(m.group(1), 16)
                if rangos and not any(lo <= a < hi for lo, hi in rangos):
                    ajeno.append((n, a))
        if dup or ajeno:
            tot_u += 1
            tot_dup += sum(x[2] for x in dup)
            tot_aj += len(ajeno)
            print('%-16s %6d %5d B/%d %8d  %s' % (
                u, len(amano), sum(x[2] for x in dup), len(dup), len(ajeno),
                ', '.join(x[0] for x in dup[:3])[:44]))
            if detalle:
                for n, n2, sz in dup:
                    print('      DUP  %-30s == %-24s %d B' % (n, n2, sz))
                for n, a in ajeno:
                    print('      AJENO %-30s 0x%08X fuera de sus rangos' % (n, a))
    print()
    print('%d unidades con duplicado o simbolo ajeno; %d B duplicados, %d ajenos' % (tot_u, tot_dup, tot_aj))


if __name__ == '__main__':
    main()
