#!/usr/bin/env python3
"""antesdegradar.py -- que va a pasar si degrado esta unidad a NonMatching.

Degradar cambia el objeto que se enlaza (el extraido en vez del nuestro) y eso NO
es neutro. Tres mecanismos medidos, y los tres se ven aqui antes de romper nada:

  1. TAMANO distinto en una seccion ALLOC -> el enlace corre.
  2. ALINEACION de SECCION distinta -> el enlazador coloca la unidad en otro
     sitio (caso snddrv: la nuestra a 32 y la extraida a 8, 16 B de diferencia).
     Se cura declarando `align:N` en esa seccion en splits.txt.
  3. SIMBOLO INDEFINIDO que solo el extraido referencia -> el enlace ni siquiera
     termina (caso snddrv/gCurRead).
  Y el cuarto, que aqui solo se insinua: dato MUERTO que solo emite nuestro
  objeto y que `-strip-unused-data` se lleva del extraido (caso filesys y su
  "bad_alloc"); se ve como una seccion mas pequena en el extraido.

    python scripts/antesdegradar.py <unidad> [<unidad> ...]
"""
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJDUMP = os.path.join(ROOT, 'build', 'ppc_binutils', 'powerpc-eabi-objdump.exe')
NM = os.path.join(ROOT, 'build', 'ppc_binutils', 'powerpc-eabi-nm.exe')
ALLOC = ('.text', '.over', '.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss', '.ctors', '.dtors')


def rutas(unidad):
    rel = unidad if '/' in unidad else 'Speed/Indep/SourceLists/' + unidad
    n = os.path.join(ROOT, 'build', 'GOWE69', 'src', rel.replace('/', os.sep) + '.o')
    e = os.path.join(ROOT, 'build', 'GOWE69', 'obj', rel.replace('/', os.sep) + '.o')
    if not os.path.exists(n):
        n = os.path.join(ROOT, 'build', 'GOWE69', 'src', unidad.replace('/', os.sep) + '.o')
        e = os.path.join(ROOT, 'build', 'GOWE69', 'obj', unidad.replace('/', os.sep) + '.o')
    return n, e


def secciones(o):
    r = subprocess.run([OBJDUMP, '-h', o], capture_output=True, text=True)
    out = {}
    for l in r.stdout.splitlines():
        c = l.split()
        if len(c) >= 7 and c[0].isdigit() and c[1] in ALLOC:
            out[c[1]] = (int(c[2], 16), c[-1])
    return out


def indefinidos(o):
    r = subprocess.run([NM, o], capture_output=True, text=True)
    return set(l.split()[-1] for l in r.stdout.splitlines() if ' U ' in l)


def main():
    for unidad in sys.argv[1:]:
        n, e = rutas(unidad)
        print('=' * 72)
        if not os.path.exists(n) or not os.path.exists(e):
            print('%-22s FALTA OBJETO (%s)' % (unidad, 'nuestro' if not os.path.exists(n) else 'extraido'))
            continue
        sn, se = secciones(n), secciones(e)
        problemas = []
        print('%-22s %-22s %-22s' % (unidad, 'NUESTRO', 'EXTRAIDO'))
        for s in sorted(set(sn) | set(se)):
            a, b = sn.get(s), se.get(s)
            marca = ''
            if a and b:
                if a[0] != b[0]:
                    marca = '  <- TAMANO'
                    problemas.append('%s: %d B contra %d B' % (s, a[0], b[0]))
                elif a[1] != b[1]:
                    marca = '  <- ALINEACION'
                    problemas.append('%s: align %s contra %s -> poner `align:%d` en splits.txt'
                                     % (s, a[1], b[1], 2 ** int(a[1].split('**')[1])))
            elif a and not b:
                marca = '  <- solo NUESTRO'
                problemas.append('%s solo la emite nuestro objeto' % s)
            elif b and not a:
                marca = '  <- solo EXTRAIDO'
                problemas.append('%s solo la emite el extraido' % s)
            f = lambda x: ('%6d  %s' % x) if x else '   --'
            print('  %-10s %-22s %-22s%s' % (s, f(a), f(b), marca))
        falta = indefinidos(e) - indefinidos(n)
        if falta:
            problemas.append('el extraido referencia y el nuestro no: %s' % ' '.join(sorted(falta)[:6]))
            print('  SIMBOLOS que solo el extraido referencia: %s' % ' '.join(sorted(falta)[:6]))
        print('  VEREDICTO: %s' % ('degradar es NEUTRO' if not problemas else '; '.join(problemas)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
