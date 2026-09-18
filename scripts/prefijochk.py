#!/usr/bin/env python3
"""prefijochk.py -- ningun rango de `.rodata` puede contener DOS prefijos de TU.

El prefijo de bWare --`GAMECUBE` seguido de `d:/mw/speed/indep/bware/inc/bware.hpp`--
marca el **principio de la `.rodata` de cada unidad de traduccion**. En el ELF
original hay **31**. Si un rango de `splits.txt` contiene dos, se ha tragado la
cabecera de la unidad siguiente, y esa se queda sin `.rodata`.

Esto vale por dos rondas de discusion, y la mia estaba mal:

  * la r50 propuso mover la frontera de `zGameModes` acotando la zona en
    `0x803EBE90`;
  * la r52 --yo-- lo refuto porque el unico UNDEF del objeto extraido,
    `lbl_803EBE90`, caia justo FUERA;
  * y el prefijo real esta en `0x803EBE98`, **ocho bytes mas alla**. Con la
    frontera bien puesta el UNDEF cae DENTRO, 8 B antes del final, y la
    propuesta de la r50 era buena.

Un criterio que se comprueba no se discute.

    python scripts/prefijochk.py
"""
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

ELF = os.path.join('orig', 'GOWE69', 'NFSMWRELEASE.ELF')
PRE = b'GAMECUBE' + bytes([0])


def rodata_del_elf():
    d = open(ELF, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
    hs = [struct.unpack('>IIIIII', d[shoff + i * shent:shoff + i * shent + 24])
          for i in range(shnum)]
    so = hs[shstr][4]
    for nm, ty, fl, ad, off, sz in hs:
        e = d.index(bytes([0]), so + nm)
        if d[so + nm:e].decode() == '.rodata':
            return ad, d[off:off + sz]
    return None, b''


def prefijos():
    ad, blob = rodata_del_elf()
    out, i = [], blob.find(PRE)
    while i >= 0:
        if b'bware.hpp' in blob[i:i + 80]:
            out.append(ad + i)
        i = blob.find(PRE, i + 1)
    return out


def rangos():
    s = open('config/GOWE69/splits.txt', encoding='utf-8', errors='replace').read()
    s = s.replace('\r\n', '\n')
    out = []
    for m in re.finditer(r'^([^\s:][^\n:]*):\n((?:\t\.\w+[^\n]*\n)+)', s, re.M):
        r = re.search(r'\t\.rodata\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)',
                      m.group(2))
        if r:
            out.append((int(r.group(1), 16), int(r.group(2), 16), m.group(1)))
    return sorted(out)


def main():
    P = prefijos()
    R = rangos()
    print('%d prefijos de TU en la .rodata del ELF original' % len(P))
    print('%d rangos de .rodata en splits.txt' % len(R))
    print()
    malos = []
    for lo, hi, u in R:
        d = [p for p in P if lo <= p < hi]
        if len(d) > 1:
            malos.append((u, lo, hi, d))
    if not malos:
        print('LIMPIO: ningun rango contiene dos prefijos.')
        return 0
    print('RANGOS QUE SE TRAGAN LA CABECERA DE LA SIGUIENTE UNIDAD: %d' % len(malos))
    for u, lo, hi, d in malos:
        print('   %-46s 0x%08X..0x%08X' % (os.path.basename(u), lo, hi))
        for k, p in enumerate(d):
            print('        prefijo %d en 0x%08X%s' % (k, p, '   <- el suyo' if k == 0 else
                                                      '   <- de la unidad SIGUIENTE'))
        sig = [x for x in R if x[0] >= hi]
        if sig:
            print('        la siguiente es %s (0x%08X): deberia empezar en 0x%08X'
                  % (os.path.basename(sig[0][2]), sig[0][0], d[1]))
    return 1


if __name__ == '__main__':
    sys.exit(main())
