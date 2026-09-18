#!/usr/bin/env python3
"""litpos.py <unidad>  -- compara el VALOR de cada literal del pool POSICION A
POSICION (el complemento de `litcheck.py`, que compara el MULTICONJUNTO y por eso
no puede ver dos constantes INTERCAMBIADAS entre si).

Empareja, instruccion a instruccion, las referencias `lbl_XXXXXXXX@l` del
objetivo con nuestros `$LCn@l` y compara los BYTES: los del objetivo se leen del
ELF (`orig/GOWE69/NFSMWRELEASE.ELF`) por direccion, los nuestros del `.rodata`
del `.o`. `matched_code` no ve un literal cambiado; el DOL si.

Dos filtros obligatorios, o te ahogas en falsos positivos:
  1. solo ranuras que objdiff marque SIN diferencia (con INSERT/DELETE la
     correspondencia por indice no vale);
  2. nada de `addi rX,rY,$LC@l`: una entrada de pool que es una DIRECCION vale 0
     en el `.o` (lleva reubicacion) y ya esta resuelta en el ELF.

Acierto de su primera pasada: `SFXCTL_AIPhysics::UpdateRPM` tenia 4000.0f y
8000.0f **cruzados** en una funcion que ya media 100%.
"""
import json
import os
import re
import struct
import subprocess
import sys

ROOT = 'C:/Users/jferr/Desktop/nfsdecompiled'
SCR = os.environ.get('SCRATCH', 'C:/Users/jferr/AppData/Local/Temp/claude/'
                     'C--Users-jferr-Desktop-nfsdecompiled/'
                     '3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad')
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
import rodata  # noqa

unit = sys.argv[1]
out = f'{SCR}/r4_c_{unit}.json'
if not os.path.exists(out):
    subprocess.run([f'{ROOT}/objdiff-cli-windows-x86_64.exe', 'diff',
                    '-1', f'{ROOT}/build/GOWE69/obj/Speed/Indep/SourceLists/{unit}.o',
                    '-2', f'{ROOT}/build/GOWE69/src/Speed/Indep/SourceLists/{unit}.o',
                    '-c', 'function_reloc_diffs=none',
                    '-c', 'ppc.calculatePoolRelocations=false',
                    '-o', out, '--format', 'json', 'zz'], capture_output=True, text=True)
d = json.load(open(out))

# --- ELF del objetivo: mapa direccion -> bytes
ed, eE, esecs = rodata.load(f'{ROOT}/orig/GOWE69/NFSMWRELEASE.ELF')


def elf_bytes(addr, n):
    for s in esecs:
        if s['addr'] and s['addr'] <= addr < s['addr'] + s['size'] and s['typ'] != 8:
            o = s['off'] + (addr - s['addr'])
            return ed[o:o + n]
    return None


# --- nuestro .o: mapa $LCn -> bytes
od, oE, osecs, osyms = rodata.syms(
    f'{ROOT}/build/GOWE69/src/Speed/Indep/SourceLists/{unit}.o')
lc = {}
for name, value, size, shndx in osyms:
    if name.startswith('$LC') and shndx < len(osecs):
        lc[name] = (osecs[shndx], value)


def our_bytes(name, n):
    if name not in lc:
        return None
    sec, val = lc[name]
    return od[sec['off'] + val: sec['off'] + val + n]


RE_T = re.compile(r'\b(lbl_[0-9A-Fa-f]{8})@l')
RE_N = re.compile(r'(\$LC\d+)@l')
SIZES = {'lfs': 4, 'lfd': 8, 'lwz': 4, 'lha': 2, 'lhz': 2, 'lbz': 1}

R = {s['name']: s for s in d['right']['symbols']}
tot = mal = sinval = 0
for s in d['left']['symbols']:
    if s.get('kind') != 'SYMBOL_FUNCTION':
        continue
    b = R.get(s['name'])
    if not b:
        continue
    LI, RI = s.get('instructions') or [], b.get('instructions') or []
    for k in range(min(len(LI), len(RI))):
        ta = ((LI[k].get('instruction') or {}).get('formatted') or '')
        tb = ((RI[k].get('instruction') or {}).get('formatted') or '')
        ma, mb = RE_T.search(ta), RE_N.search(tb)
        if not (ma and mb):
            continue
        # solo pares ALINEADOS: si objdiff marca diferencia en esa ranura, la
        # correspondencia instruccion-a-instruccion no es de fiar
        if (LI[k].get('diff_kind') or 'DIFF_NONE') != 'DIFF_NONE':
            continue
        if (RI[k].get('diff_kind') or 'DIFF_NONE') != 'DIFF_NONE':
            continue
        op = ta.split()[0]
        if op != tb.split()[0]:
            continue
        if op not in SIZES:
            continue
        n = SIZES[op]
        addr = int(ma.group(1)[4:], 16)
        A = elf_bytes(addr, n)
        B = our_bytes(mb.group(1), n)
        tot += 1
        if A is None or B is None:
            sinval += 1
            continue
        if op == 'addi':                       # cadena: comparar hasta el NUL
            A2 = A.split(b'\0')[0]
            B2 = B.split(b'\0')[0]
            if A2 != B2:
                mal += 1
                print('DISTINTO %s  %s  %s=%r  %s=%r' %
                      (s['name'], op, ma.group(1), A2[:24], mb.group(1), B2[:24]))
            continue
        if A != B:
            mal += 1
            print('DISTINTO %s  %s  %s=%s  %s=%s' %
                  (s['name'], op, ma.group(1), A.hex(), mb.group(1), B.hex()))
print('%s: %d referencias emparejadas, %d con VALOR distinto, %d sin resolver'
      % (unit, tot, mal, sinval))
