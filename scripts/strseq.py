#!/usr/bin/env python3
"""strseq.py -- las CADENAS que objdiff no compara, por funcion.

objdiff empareja instrucciones, no contenidos: un `addi` a "COLUMN2_DATA" y otro a
"COLUMN3_DATA" son **la misma instruccion** para el. Y NFSMW **hashea en runtime**,
asi que "smackable" por "Smackable" no da error: da un `Find` que falla en silencio.
Por eso una cadena mal vive para siempre en una funcion que marca **100%**.

DOS COSAS QUE COSTARON UN BARRIDO ENTERO APRENDER:

1. **La comparacion POSICIONAL miente.** Emparejar la cadena n-esima de cada lado
   da falsos en cuanto sobra o falta una instruccion, porque desplaza todas las
   ranuras siguientes. Se compara el **multiconjunto** (la lista ordenada), y
   aparte, con menos confianza, los casos con distinto NUMERO de cadenas.
2. **Solo una direccion es fiable.** «El objetivo la tiene y nosotros no» y las
   **sustituciones con el mismo numero** son senal; «nosotros la tenemos y el
   objetivo no» es **ruido del extractor**, que no decodifica todas las formas de
   simbolo del lado objetivo. Medido: **zAttribSys casa al 100% y cantaba 13
   falsos**; de 85 hallazgos brutos en todo el juego, **8 eran reales**.

    python scripts/strseq.py zFe2 zPhysics      # una o varias unidades
    python scripts/strseq.py --todas            # todas las SourceLists
"""
import json, os, re, subprocess, sys

ROOT = 'C:/Users/jferr/Desktop/nfsdecompiled'
SCR = os.environ.get('SCRATCH', 'C:/Users/jferr/AppData/Local/Temp/claude/'
                     'C--Users-jferr-Desktop-nfsdecompiled/'
                     '3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad')
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
import rodata  # noqa

unit = sys.argv[1]
out = f'{SCR}/r9s_{unit}.json'
if not os.path.exists(out):
    subprocess.run([f'{ROOT}/objdiff-cli-windows-x86_64.exe', 'diff',
                    '-1', f'{ROOT}/build/GOWE69/obj/Speed/Indep/SourceLists/{unit}.o',
                    '-2', f'{ROOT}/build/GOWE69/src/Speed/Indep/SourceLists/{unit}.o',
                    '-c', 'function_reloc_diffs=none',
                    '-c', 'ppc.calculatePoolRelocations=false',
                    '-o', out, '--format', 'json', 'zz'], capture_output=True, text=True)
d = json.load(open(out))

ed, eE, esecs = rodata.load(f'{ROOT}/orig/GOWE69/NFSMWRELEASE.ELF')
def elf_str(addr):
    for s in esecs:
        if s['addr'] and s['addr'] <= addr < s['addr'] + s['size'] and s['typ'] != 8:
            o = s['off'] + (addr - s['addr'])
            return ed[o:o + 200].split(b'\0')[0]
    return None

od, oE, osecs, osyms = rodata.syms(
    f'{ROOT}/build/GOWE69/src/Speed/Indep/SourceLists/{unit}.o')
lc = {}
for name, value, size, shndx in osyms:
    if name.startswith('$LC') and shndx < len(osecs):
        lc[name] = (osecs[shndx], value)
def our_str(name):
    if name not in lc: return None
    sec, val = lc[name]
    return od[sec['off'] + val: sec['off'] + val + 200].split(b'\0')[0]

RE_T = re.compile(r'\baddi\s+r\d+,\s*r\d+,\s*(lbl_[0-9A-Fa-f]{8})@l')
RE_N = re.compile(r'\b(?:addi|la)\s+r?\d+,\s*(?:r?\d+,\s*)?(\$LC\d+)@l')
RE_N2 = re.compile(r'\baddi\s+r\d+,\s*r\d+,\s*(\$LC\d+)@l')

def printable(b):
    if b is None: return None
    try: s = b.decode('latin-1')
    except Exception: return None
    if not s: return None
    # solo lo que parece una cadena de texto
    if all(32 <= ord(c) < 127 or c in '\n\t\r' for c in s): return s
    return None

R = {s['name']: s for s in d['right']['symbols']}
bad = 0; tot = 0; fns = 0; reord = 0
for s in d['left']['symbols']:
    if s.get('kind') != 'SYMBOL_FUNCTION': continue
    b = R.get(s['name'])
    if not b: continue
    A = []
    for i in (s.get('instructions') or []):
        t = ((i.get('instruction') or {}).get('formatted') or '')
        m = RE_T.search(t)
        if m: A.append(printable(elf_str(int(m.group(1)[4:], 16))))
    B = []
    for i in (b.get('instructions') or []):
        t = ((i.get('instruction') or {}).get('formatted') or '')
        m = RE_N2.search(t)
        if m: B.append(printable(our_str(m.group(1))))
    A = [x for x in A if x is not None]
    B = [x for x in B if x is not None]
    if not A and not B: continue
    fns += 1
    if sorted(A) != sorted(B):
        falta = sorted(set(A) - set(B))
        sobra = sorted(set(B) - set(A))
        # SUSTITUCION (mismo numero, contenido distinto) = bug casi seguro.
        # Distinto numero = casi siempre el extractor, que no ve todas las formas
        # de simbolo del lado objetivo. Va aparte.
        etiqueta = 'DISTINTA' if len(A) == len(B) else 'CUENTA  '
        print('%s %-56s obj=%d nuestro=%d' % (etiqueta, s['name'][:56], len(A), len(B)))
        if falta:
            print('   el objetivo la tiene y nosotros NO: %r' % (falta[:6],))
        if sobra:
            print('   nosotros la tenemos y el objetivo NO (RUIDO probable): %r'
                  % (sobra[:6],))
        if not falta and not sobra:
            print('   objetivo: %r' % (A[:8],))
            print('   nuestro : %r' % (B[:8],))
        bad += 1
    elif A != B:
        reord += 1
    tot += len(A)
print('%s: %d funciones con cadenas, %d comparadas, %d discrepancias, %d solo'
      ' reordenadas' % (unit, fns, tot, bad, reord))
