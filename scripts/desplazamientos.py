#!/usr/bin/env python3
"""desplazamientos.py <diff.json> [...] -- fallos de LAYOUT de struct. Busca cargas y almacenamientos que
solo difieren en el DESPLAZAMIENTO, con el mismo mnemonico, el mismo registro destino y el mismo registro
base: `lw v0, 0x58(s0)` contra `lw v0, 0x68(s0)`. Un miembro mal colocado tumba a TODAS las funciones que
tocan la clase, asi que cada par que se repite es un candidato de mucho peso. La pila (sp) no cuenta.

Agrupa por (original -> nuestro) y da la primera funcion donde aparece, para ir a la clase. Despues, para
saber QUE base o miembro mide distinto, no se razona: se le pregunta al compilador con scripts/sonda.py y se
compara con el volcado de tipos del original (symbols/PS2/PS2_types.nothpp). Asi salieron en la r81:

    CDAction* -16   -> UTL::COM::Object sin PS2ALIGN16 (la de IUnknown si lo tenia)
    SoundAI   +16   -> VoiceUsage::cs_Alpine, un miembro que el prototipo A124 no tiene

    python scripts/desplazamientos.py build/diffs/SLES-53558-A124/*.json"""
import collections
import json
import re
import sys

if len(sys.argv) < 2 or sys.argv[1] in ('-h', '--help'):
    print(__doc__)
    sys.exit(0)

MEM = re.compile(r'^(l[bhwdq]u?|s[bhwdq]|lwc1|swc1|lqc2|sqc2)\s+(\S+),\s*(-?(?:0x)?[0-9a-fA-F]+)\((\w+)\)$')
pares = collections.Counter()
delta = collections.Counter()
donde = collections.defaultdict(set)


def fmt(i):
    return ((i or {}).get('instruction') or {}).get('formatted') or ''


def num(s):
    return int(s, 16) if 'x' in s.lower() else int(s)


for p in sys.argv[1:]:
    d = json.load(open(p, encoding='utf-8'))
    R = {s['name']: s for s in d['right']['symbols']}
    for s in d['left']['symbols']:
        mp = s.get('match_percent')
        if mp is None or not (80 <= mp < 100):
            continue
        li = s.get('instructions') or []
        ri = (R.get(s['name']) or {}).get('instructions') or []
        for i, x in enumerate(li):
            if x.get('diff_kind') != 'DIFF_ARG_MISMATCH' or i >= len(ri):
                continue
            ma, mb = MEM.match(fmt(x)), MEM.match(fmt(ri[i]))
            if not (ma and mb):
                continue
            if ma.group(1) != mb.group(1) or ma.group(2) != mb.group(2) or ma.group(4) != mb.group(4):
                continue
            oa, ob = num(ma.group(3)), num(mb.group(3))
            if oa == ob or ma.group(4) == 'sp':
                continue
            pares[(ma.group(1), oa, ob)] += 1
            delta[ob - oa] += 1
            donde[(ma.group(1), oa, ob)].add(s['name'])

print('diferencia (nuestro - original) mas frecuente:')
for dd, n in delta.most_common(8):
    print('   %+5d  x%d' % (dd, n))
print('\npares (instr, original -> nuestro)          veces  funciones')
for (m, oa, ob), n in pares.most_common(24):
    fs = sorted(donde[(m, oa, ob)])
    print('   %-5s 0x%-4x -> 0x%-4x  %+4d   %5d  %4d   %s' % (m, oa, ob, ob - oa, n, len(fs), fs[0][:44]))
