#!/usr/bin/env python3
"""mangfix.py -- vuelve a resolver los ALIAS de nombres mangled de clase local.

Una clase declarada DENTRO de una funcion recibe nombres con un sufijo numerico
(`....0_7NumCarsRC14FECareerRecord.35688`) que sale de un contador global de
declaraciones de la unidad. GCC 2.9 emite la VTABLE de esa clase --que referencia
su destructor y sus virtuales-- pero NO los cuerpos, porque estan definidos
dentro de la clase y eso en GCC 2.9 no emite simbolo. Resultado: referencias
indefinidas al enlazar, y la SourceList entera no puede promocionar.

El apano del proyecto es escribir esos cuerpos con un `asm()` que declara DOS
etiquetas: la del OBJETIVO (con `.globl`, que es la que tiene que salir en el
DOL) y un ALIAS con el numero que pide NUESTRO compilador. Y ese numero **se
desplaza en cuanto cambia cualquier declaracion anterior de la unidad**: la
unidad deja de enlazar y no hay forma de verlo hasta intentar promocionarla.

Paso en zFe2: los alias decian `.32509` y el compilador pedia `.32083`.

    python scripts/mangfix.py zFe2           # comprueba y corrige
    python scripts/mangfix.py zFe2 --check   # solo comprueba (rc=1 si desfasa)

Compila la unidad ANTES de pasarlo, y vuelve a compilar despues.
"""
import glob
import io
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BS = chr(92)          # el heredoc de bash se come los backslashes; asi no hay


def indefinidos(obj):
    """Nombres de simbolos SHN_UNDEF del objeto."""
    d = open(obj, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, n, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(n)]
    st = [s for s in S if s[1] == 2][0]
    strt = S[st[6]][4]
    out = set()
    for q in range(st[4], st[4] + st[5], 16):
        nm, val, sz, info, shn = struct.unpack('>IIIBxH', d[q:q + 16])
        if shn == 0 and nm:
            out.add(d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1'))
    return out


SUF = re.compile(r'^(.*)[.](\d+)$')


def etiqueta(base, num):
    """La linea de fuente que declara la etiqueta: `"<base>.<num>:\\n"`."""
    return '"' + base + '.' + num + ':' + BS + 'n"'


def main():
    solo_check = '--check' in sys.argv
    unidades = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not unidades:
        sys.exit(__doc__)
    os.chdir(ROOT)
    corregidos, fallos = 0, 0
    for u in unidades:
        obj = os.path.join('build', 'GOWE69', 'src', 'Speed', 'Indep',
                           'SourceLists', u + '.o')
        if not os.path.exists(obj):
            print('  %s: no esta compilado' % u)
            fallos += 1
            continue
        pend = {}
        for s in indefinidos(obj):
            m = SUF.match(s)
            if m and ('.0_' in m.group(1) or m.group(1).startswith('_._Q3')):
                pend[m.group(1)] = m.group(2)
        if not pend:
            print('  %s: sin alias desfasados' % u)
            continue
        resueltos = set()
        ficheros = glob.glob('src/**/*.cpp', recursive=True)
        for p in ficheros:
            try:
                s = io.open(p, encoding='utf-8', newline='').read()
            except Exception:
                continue
            if 'asm(' not in s:
                continue
            orig = s
            for base, num in list(pend.items()):
                if etiqueta(base, num) in s:      # ya esta al dia
                    resueltos.add(base)
                    continue
                # busca la etiqueta suelta con OTRO numero, saltando la del objetivo
                pat = re.compile(re.escape('"' + base + '.') + r'(\d+)' + re.escape(':' + BS + 'n"'))
                for m in list(pat.finditer(s)):
                    viejo = m.group(1)
                    esObjetivo = ('.globl ' + base + '.' + viejo) in s
                    if esObjetivo:
                        continue                  # esa es la que tiene que salir en el DOL
                    s = s.replace(etiqueta(base, viejo), etiqueta(base, num))
                    print('  CORRIGE  %-18s ...%-38s .%s -> .%s'
                          % (os.path.basename(p), base[-38:], viejo, num))
                    resueltos.add(base)
                    corregidos += 1
                    break
            if s != orig and not solo_check:
                io.open(p, 'w', encoding='utf-8', newline='').write(s)
        for base in sorted(set(pend) - resueltos):
            print('  SIN ALIAS  %s.%s' % (base[-70:], pend[base]))
            fallos += 1
    if corregidos and not solo_check:
        print('%d alias corregidos -- RECOMPILA la unidad' % corregidos)
    if not corregidos and not fallos:
        print('todo al dia')
    sys.exit(1 if (fallos or (corregidos and solo_check)) else 0)


main()
