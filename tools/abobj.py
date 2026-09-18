#!/usr/bin/env python3
"""abobj2.py -- A/B por objeto cuando el cambio PUEDE anadir o quitar cadenas.

La version anterior exigia secciones identicas. Pero un `new ("MapItem", __LINE__)` que el
original tiene mete una cadena en .rodata y desplaza todo lo de detras: el
objeto cambia y, aun asi, se acerca al original. Aqui se mide:

  1. .text byte a byte (con RELA los campos reubicados valen cero) y la lista
     de reubicaciones de .text por (offset, tipo): el CODIGO no cambia.
  2. el conjunto de cadenas de .rodata frente al objeto ORIGINAL
     (build/GOWE69/obj/...): cuantas le faltan y cuantas le sobran, antes y
     despues.

    python tools/abobj.py <unidad> <fichero src/...> [mas ficheros]

Sale 0 si el codigo es identico y las cadenas no se alejan del original.
"""
import os
import re
import shutil
import subprocess
import sys

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
AQUI = os.path.join(ROOT, 'build', '_abobj')


def compila(obj):
    r = subprocess.run([sys.executable, '-m', 'ninja', obj], capture_output=True, text=True,
                       errors='replace', cwd=ROOT)
    if r.returncode != 0:
        print(r.stdout[-2000:])
        raise SystemExit('no compila')


def cadenas(e):
    s = e.get_section_by_name('.rodata')
    d = s.data() if s is not None else b''
    return set(m.group() for m in re.finditer(rb'[\x20-\x7e]{3,}(?=\x00)', d))


def codigo(e):
    t = e.get_section_by_name('.text').data()
    rel = e.get_section_by_name('.rela.text')
    lista = []
    if isinstance(rel, RelocationSection):
        lista = [(r['r_offset'], r['r_info_type']) for r in rel.iter_relocations()]
    return t, lista


def main():
    unidad, ficheros = sys.argv[1], sys.argv[2:]
    obj = 'build/GOWE69/src/Speed/Indep/SourceLists/%s.o' % unidad
    orig = ELFFile(open(os.path.join(ROOT, 'build/GOWE69/obj/Speed/Indep/SourceLists/%s.o' % unidad), 'rb'))
    co = cadenas(orig)
    os.makedirs(AQUI, exist_ok=True)
    con = os.path.join(AQUI, 'ab2_%s_con.o' % unidad)
    sin = os.path.join(AQUI, 'ab2_%s_sin.o' % unidad)
    compila(obj)
    shutil.copy(os.path.join(ROOT, obj), con)
    subprocess.run(['git', 'stash', 'push', '-q', '--'] + ficheros, cwd=ROOT, check=True)
    try:
        compila(obj)
        shutil.copy(os.path.join(ROOT, obj), sin)
    finally:
        subprocess.run(['git', 'stash', 'pop', '-q'], cwd=ROOT, check=True)
    compila(obj)
    es, ec = ELFFile(open(sin, 'rb')), ELFFile(open(con, 'rb'))
    ts, rs = codigo(es)
    tc, rc = codigo(ec)
    ok_codigo = ts == tc and rs == rc
    print('%s: .text %s (%d B), reubicaciones de .text %s (%d)' % (
        unidad, 'IDENTICO' if ts == tc else 'DISTINTO', len(tc), 'iguales' if rs == rc else 'DISTINTAS', len(rc)))
    if ts != tc:
        dif = [i for i in range(min(len(ts), len(tc))) if ts[i] != tc[i]]
        print('   primer byte distinto en 0x%X; tamanos %d -> %d' % (dif[0] if dif else min(len(ts), len(tc)), len(ts), len(tc)))
    cs, cc = cadenas(es), cadenas(ec)
    fs, fc = len(co - cs), len(co - cc)
    ss, sc = len(cs - co), len(cc - co)
    print('cadenas frente al original: faltan %d -> %d, sobran %d -> %d' % (fs, fc, ss, sc))
    for x in sorted((cs - co) ^ (cc - co)):
        print('   sobra %s: %s' % ('ahora' if x in cc else 'antes', x.decode()))
    for x in sorted((co - cs) ^ (co - cc)):
        print('   falta %s: %s' % ('ahora' if x not in cc else 'antes', x.decode()))
    return 0 if ok_codigo and fc <= fs and sc <= ss else 1


if __name__ == '__main__':
    sys.exit(main())
