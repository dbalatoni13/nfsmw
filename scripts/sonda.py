#!/usr/bin/env python3
"""sonda.py <VERSION> <sonda.cpp> -- pregunta al compilador el LAYOUT real de una clase: tamanos,
alineaciones y desplazamientos, compilando una sonda con los flags REALES de esa version.

Cuando un miembro se lee a otro desplazamiento que en el original (scripts/desplazamientos.py) y las bases
y los miembros parecen bien declarados, el fallo esta en el tamano o la alineacion de algo, y eso no se
deduce leyendo cabeceras. La sonda deja cada dato en una variable global `int` cuyo nombre empieza por
`sonda_`, y este script los lee del .s:

    #define private public            // para poder mirar miembros privados
    #include "Speed/Indep/Src/Speech/SoundAI.h"
    #define OFFM(M) ((int)&((SoundAI *)0x1000)->M - 0x1000)
    #define OFFB(B) ((int)(static_cast<B *>((SoundAI *)0x1000)) - 0x1000)
    int sonda_sz_soundai = sizeof(SoundAI);
    int sonda_al_object  = __alignof__(UTL::COM::Object);
    int sonda_m_dispatch = OFFM(mDispatch);

Si la sonda hereda de bases sin constructor por defecto, hay que DECLARARLE uno (`Sonda();`): GCC 2.9 da
error si no. Nunca se instancia, asi que no hace falta definirlo.

Se compara con el volcado de tipos del original, symbols/PS2/PS2_types.nothpp, que da el tamano de cada
struct (`// 0x2d0`) y el desplazamiento de cada miembro. OJO: ahi las posiciones de las BASES van en BITS
(`/* 0x180 */ IAttachable` es el byte 0x30); las de los miembros, en bytes.

Hoy solo sirve para las versiones de PS2 (ee-gcc), que es donde han salido los fallos de layout. Configura
la version, saca los cflags de una unidad de juego del build.ninja, compila, y deja configurada GOWE69.

    python scripts/sonda.py SLES-53558-A124 build/sonda_soundai.cpp"""
import os
import re
import subprocess
import sys

if len(sys.argv) < 3 or sys.argv[1] in ('-h', '--help'):
    print(__doc__)
    sys.exit(0)

ROOT = os.getcwd()
V, FUENTE = sys.argv[1], sys.argv[2]
OUT = os.path.join(ROOT, 'build', 'sonda.s')
PY = sys.executable


def cflags_de(version):
    """Los cflags de zAI en el build.ninja recien configurado, con las continuaciones `$` unidas."""
    lineas = open(os.path.join(ROOT, 'build.ninja'), encoding='utf-8', errors='replace').read().split('\n')
    clave = '\\src\\Speed\\Indep\\SourceLists\\zAI.o: ee-gcc'
    for i, l in enumerate(lineas):
        if l.startswith('build build\\' + version) and clave in l:
            for j in range(i, min(i + 12, len(lineas))):
                if lineas[j].strip().startswith('cflags ='):
                    trozo, k = lineas[j].split('=', 1)[1], j
                    while trozo.rstrip().endswith('$'):
                        k += 1
                        trozo = trozo.rstrip()[:-1] + ' ' + lineas[k].strip()
                    return trozo.split()
    return None


try:
    herr = []
    for op, f in (('--dtk', 'dtk.exe'), ('--objdiff', 'objdiff-cli.exe')):
        if os.path.exists(os.path.join(ROOT, 'build', 'tools', f)):
            herr += [op, 'build/tools/' + f]
    subprocess.run([PY, 'configure.py', '--version', V] + herr, cwd=ROOT, capture_output=True)
    flags = cflags_de(V)
    if not flags:
        sys.exit('no encuentro los cflags de %s en build.ninja (es una version de PS2?)' % V)
    if os.path.exists(OUT):
        os.remove(OUT)
    cc = os.path.normpath('build/compilers/PS2/ee-gcc2.9-991111/bin/ee-gcc.exe')
    cmd = [PY, os.path.normpath('tools/ee_gcc_pp.py'), os.path.normpath('build/%s/pp' % V), cc, '--'] + \
        flags + ['-x', 'c++', '-S', '-o', OUT, FUENTE]
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, errors='replace')
    if not os.path.exists(OUT):
        print('NO COMPILA:')
        print('\n'.join(l for l in (r.stderr or r.stdout).split('\n')
                        if 'warning' not in l and l.strip())[-25:])
        sys.exit(1)
    s = open(OUT, encoding='utf-8', errors='replace').read()
    for m in re.finditer(r'^(sonda_\w+):\s*\n\s*\.word\s+(-?\w+)', s, re.M):
        v = m.group(2)
        n = int(v, 16) if v.startswith('0x') else int(v)
        print('  %-28s 0x%-5x (%d)' % (m.group(1), n, n))
finally:
    subprocess.run([PY, 'configure.py'], cwd=ROOT, capture_output=True)
