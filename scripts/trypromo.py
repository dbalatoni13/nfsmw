#!/usr/bin/env python3
"""trypromo.py -- prueba una promocion SIN tocar el arbol ni esperar a la ventana.

Reconstruye la lista de objetos del enlace desde `build.ninja`, sustituye el `.o`
extraido de las unidades que le digas por el NUESTRO, enlaza a un temporal del
scratchpad y compara el DOL resultante con el original. No escribe en
`configure.py`, ni en `build/GOWE69/main.elf`, ni en `config/`.

    python scripts/trypromo.py Speed/Indep/SourceLists/zOnline
    python scripts/trypromo.py zOnline zMission            # atajo: SourceLists
    python scripts/trypromo.py libc/libgcc2_3 libc/libgcc2_6 libc/libgcc2_8
    python scripts/trypromo.py --ldflags "-strip-unused -keep config/GOWE69/keep.lst" zLua

Varias unidades a la vez se promocionan JUNTAS, que es lo que hace falta cuando
una define un simbolo que otra referencia (los `libgcc2_*` y su `__clz_tab`).

`promote.py` compara secciones y simbolos; esto compara el DOL, que es el juez.
Una unidad puede salir LIMPIA en `promote.py` y romper el DOL igual --paso con
`systemvars` por un hueco de 8 B en `.sdata`-- asi que conviene pasar las dos.
"""
import hashlib
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SHA_ORIG = '9619ba57c9919f95f7f2ac951a2166a3517f91e3'
LD = os.path.join('build', 'compilers', 'ProDG', '3.9.3', 'ngcld.exe')
DTK = os.path.join('build', 'tools', 'dtk.exe')
LDFLAGS = '-strip-unused-data -keep config/GOWE69/keep.lst'
# CARRERA MEDIDA: esto escribia su .rsp/.elf/.dol en %TEMP% COMPARTIDO, y con
# ocho agentes en paralelo dos ejecuciones se pisan y el veredicto sale al azar.
# Le costo una tanda entera a un agente de la ronda 23. Ahora cada proceso usa su
# propio subdirectorio, con el PID en el nombre, y lo limpia al salir.
TMP = os.path.join(os.environ.get('SCRATCH')
                   or os.environ.get('TEMP')
                   or os.path.join(ROOT, 'build'),
                   'trypromo_%d' % os.getpid())
os.makedirs(TMP, exist_ok=True)
import atexit
import shutil
atexit.register(lambda: shutil.rmtree(TMP, ignore_errors=True))


def objetos_del_enlace():
    """La lista `$in` del edge de main.elf, con las lineas continuadas rehechas."""
    L = open(os.path.join(ROOT, 'build.ninja'), encoding='utf-8', errors='replace').read().split('\n')
    J, i = [], 0
    while i < len(L):
        s = L[i]
        while s.endswith('$') and i + 1 < len(L):
            i += 1
            s = s[:-1] + L[i].strip()
        J.append(s)
        i += 1
    for s in J:
        if s.startswith('build ') and 'main.elf' in s and ': link' in s:
            return [x for x in s.split(': link', 1)[1].split() if x.endswith('.o')]
    sys.exit('no encuentro el edge de main.elf en build.ninja')


def resolver(u, base):
    """Nombre de unidad -> (objeto extraido, objeto nuestro). Acepta el atajo de SourceLists."""
    cand = [u, 'Speed/Indep/SourceLists/' + u]
    for c in cand:
        o = os.path.join('build', 'GOWE69', 'obj', *c.split('/')) + '.o'
        if o in base:
            return o, o.replace(os.path.join('GOWE69', 'obj'), os.path.join('GOWE69', 'src'))
    return None, None


def main():
    a = sys.argv[1:]
    ldflags = LDFLAGS
    if '--ldflags' in a:
        i = a.index('--ldflags')
        ldflags = a[i + 1]
        a = a[:i] + a[i + 2:]
    units = [x for x in a if not x.startswith('-')]
    if not units:
        sys.exit(__doc__)

    os.chdir(ROOT)
    base = objetos_del_enlace()
    sub = {}
    for u in units:
        o, n = resolver(u, base)
        if o is None:
            print('  %-44s NO esta en la lista de enlace' % u)
            return
        if not os.path.exists(n):
            print('  %-44s falta nuestro .o (compila la unidad primero)' % u)
            return
        sub[o] = n

    rsp = os.path.join(TMP, 'trypromo.rsp')
    elf = os.path.join(TMP, 'trypromo.elf')
    dol = os.path.join(TMP, 'trypromo.dol')
    open(rsp, 'w').write('\n'.join(sub.get(x, x) for x in base) + '\n')
    for f in (elf, dol):
        if os.path.exists(f):
            os.remove(f)

    r = subprocess.run([LD] + ldflags.split() + ['-T', 'config/GOWE69/ldscript.ld', '-o', elf, '@' + rsp],
                       capture_output=True, text=True)
    etiqueta = ' + '.join(units)
    if not os.path.exists(elf):
        err = [l for l in (r.stdout + r.stderr).splitlines() if 'error' in l.lower()]
        print('  %-44s ENLACE FALLA: %s' % (etiqueta, '; '.join(err[:2])[:90]))
        return
    subprocess.run([DTK, 'elf2dol', elf, dol], capture_output=True, text=True)
    if not os.path.exists(dol):
        print('  %-44s no se genero el DOL' % etiqueta)
        return
    h = hashlib.sha1(open(dol, 'rb').read()).hexdigest()
    print('  %-44s %s' % (etiqueta, 'DOL OK' if h == SHA_ORIG else 'DOL ROTO (%s)' % h[:12]))


if __name__ == '__main__':
    main()
