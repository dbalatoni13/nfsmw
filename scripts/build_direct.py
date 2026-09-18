#!/usr/bin/env python3
"""Compila las unidades invocando ngccc directamente, saltandose ninja.

Ninja entra en bucle regenerando su propio manifiesto ("still dirty after 100
tries") en cuanto varios procesos tocan objdiff.json a la vez, y ademas
reejecuta configure.py en cada invocacion. Esto lee build.ninja una sola vez,
saca los cflags **de cada unidad** (no valen unos genericos: zFoundation lleva
-mps-float -fvtable-thunks=1, y otras tienen lo suyo) y lanza los compilados en
paralelo.

  python scripts/build_direct.py            # todas las SourceLists
  python scripts/build_direct.py zFe2 zAI   # solo esas
"""
import os
import re
import shlex
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

NINJA = 'build.ninja'
RE_BUILD = re.compile(r'^build (\S+\.o): prodg \$?\s*$')
RE_CONT = re.compile(r'^\s+(\S.*?)\s*(\$)?$')


RULES = ('prodg', 'mwcc')
RULE = {}      # unidad -> regla de ninja
BASEDIR = {}   # unidad -> $basedir (mwcc escribe el .o en un DIRECTORIO)


def parse_units():
    """-> {nombre: (fuente, salida, [cflags], toolchain, rename)} leyendo build.ninja."""
    units = {}
    lines = open(NINJA, encoding='utf-8', errors='replace').read().split('\n')
    # Con rutas muy largas ninja deja la palabra `build` SOLA en su linea y
    # baja el objeto a la continuacion. Sin recomponerlo, realmemcard y las
    # demas bibliotecas de ruta profunda son invisibles: 228 unidades en vez
    # de 359.
    joined = []
    i = 0
    while i < len(lines):
        if lines[i].rstrip() == 'build $' and i + 1 < len(lines):
            joined.append('build ' + lines[i + 1].strip())
            i += 2
            continue
        joined.append(lines[i])
        i += 1
    lines = joined
    i = 0
    while i < len(lines):
        line = lines[i]
        m = re.match(r'^build (\S+\.o): (.*)$', line)
        if not m:
            i += 1
            continue
        rest = m.group(2).strip()
        # ninja parte la linea cuando la ruta es larga y el nombre de la
        # regla cae en la continuacion: las bibliotecas son todas asi.
        # tres unidades de LibSN son Metrowerks, no ProDG: regla mwcc_sjis.
        if not rest.startswith(RULES):
            nxt = lines[i + 1].strip() if i + 1 < len(lines) else ''
            if rest not in ('$', '') or not nxt.startswith(RULES):
                i += 1
                continue
            rule = nxt.split()[0]
        else:
            rule = rest.split()[0]
        out = m.group(1).replace('\\', '/')
        # la fuente esta en la continuacion
        src = None
        j = i - 1
        while j + 1 < len(lines) and src is None:
            j += 1
            cand = lines[j].strip().rstrip('$').strip()
            for tok in cand.split():
                # las bibliotecas de middleware (path, snd, vp6, libc) son .c
                if tok.endswith('.cpp') or tok.endswith('.c'):
                    src = tok.replace('\\', '/')
                    break
        # las variables van hasta la siguiente linea que no empiece por espacio
        cflags, toolchain, rename, basedir = [], None, None, None
        k = j
        while k + 1 < len(lines):
            k += 1
            l = lines[k]
            if l and not l[0].isspace():
                break
            s = l.strip()
            if s.startswith('toolchain_version = '):
                toolchain = s.split(' = ', 1)[1].rstrip(' $')
            elif s.startswith('basedir = '):
                basedir = s.split(' = ', 1)[1].rstrip(' $')
            elif s.startswith('section_rename = '):
                rename = s.split(' = ', 1)[1].rstrip(' $')
            elif s.startswith('cflags = '):
                acc = s.split(' = ', 1)[1]
                while acc.endswith('$'):
                    k += 1
                    acc = acc[:-1] + ' ' + lines[k].strip()
                cflags = acc.split()
        name = os.path.basename(out)[:-2]
        if name in units:
            # dos bibliotecas pueden traer el mismo basename (public.c,
            # tasks.c); el segundo se queda con su ruta para desempatar.
            name = out[:-2]
        if src:
            units[name] = (src, out, cflags, toolchain or 'ProDG\\3.9.3', rename)
            RULE[name] = rule
            BASEDIR[name] = basedir
        i = k
    return units


def compile_one(name, spec, root):
    src, out, cflags, toolchain, rename = spec
    rule = RULE.get(name, 'prodg')
    env = dict(os.environ)
    os.makedirs(os.path.dirname(out), exist_ok=True)
    if rule.startswith('mwcc'):
        # Metrowerks: sjiswrap delante, y -o toma el DIRECTORIO, no el fichero.
        cc = os.path.join(root, 'build', 'compilers', toolchain, 'mwcceppc.exe')
        wrap = os.path.join(root, 'build', 'tools', 'sjiswrap.exe')
        dest = BASEDIR.get(name) or os.path.dirname(out)
        # ninja guarda '-pragma "cats off"' y el .split() de arriba lo parte
        # en tres: hay que recomponer las comillas antes de pasarlo.
        cflags = shlex.split(' '.join(cflags))
        head = [wrap, cc] if rule.endswith('sjis') else [cc]
        cmd = head + cflags + ['-c', src, '-o', dest]
    else:
        cc = os.path.join(root, 'build', 'compilers', toolchain, 'ngccc.exe')
        env['SN_NGC_PATH'] = os.path.join(root, 'build', 'compilers', toolchain)
        cmd = [cc] + cflags + ['-c', '-o', out, src]
    p = subprocess.run(cmd, cwd=root, env=env, capture_output=True, text=True)
    err = (p.stderr or '') + (p.stdout or '')
    if p.returncode == 0 and rename:
        # zFeOverlay y zOnline viven en .over, no en .text. Sin esto la unidad
        # mide 0% entera y parece un desastre que no existe.
        r = subprocess.run([sys.executable, os.path.join('tools', 'rename_section.py'),
                            '-q', out, rename],
                           cwd=root, capture_output=True, text=True)
        if r.returncode != 0:
            return name, r.returncode, 'rename_section: ' + (r.stderr or r.stdout or '')
    return name, p.returncode, err


def main():
    root = os.getcwd()
    units = parse_units()
    want = [a for a in sys.argv[1:] if not a.startswith('--')]
    if want:
        def hit(k, spec):
            return any(w == k or w == os.path.basename(k)
                       or w in spec[1] or w in spec[0] for w in want)
        units = {k: v for k, v in units.items() if hit(k, v)}
    elif '--all' not in sys.argv:
        # por defecto solo las SourceLists: es la medida de siempre
        units = {k: v for k, v in units.items() if 'SourceLists' in v[1]}
    if not units:
        print('no hay unidades que compilar'); return 1
    print('compilando %d unidades' % len(units))
    fails = 0
    with ThreadPoolExecutor(max_workers=6) as ex:
        futs = [ex.submit(compile_one, n, s, root) for n, s in sorted(units.items())]
        for f in futs:
            name, rc, err = f.result()
            if rc != 0:
                fails += 1
                print('FAILED: %s' % name)
                for l in err.strip().split('\n')[:6]:
                    print('   %s' % l)
            else:
                print('ok  %s' % name)
    print('\n%d ok, %d fallidas' % (len(units) - fails, fails))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
