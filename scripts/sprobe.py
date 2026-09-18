#!/usr/bin/env python3
"""Compila UN .cpp de la unidad con -S usando los cflags reales de la unidad.

Reproduce una funcion en 1-4 s en vez de 60. El .cpp se compila solo, con las
mismas banderas e includes que la SourceList, y se deja el .s en el scratchpad.

  python scripts/sprobe.py zAI src/Speed/Indep/Src/AI/Common/AIPursuit.cpp
  python scripts/sprobe.py zAI <cpp> --fn ShouldEnd__C9AIPursuit
"""
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build_direct as bd

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCR = os.environ.get('SCRATCH', r'C:\Users\jferr\AppData\Local\Temp\claude'
                     r'\C--Users-jferr-Desktop-nfsdecompiled'
                     r'\3eb1ea2d-b037-4ced-b620-8e690908107f\scratchpad')


def main():
    unit = sys.argv[1]
    src = sys.argv[2].replace('\\', '/')
    fn = None
    if '--fn' in sys.argv:
        fn = sys.argv[sys.argv.index('--fn') + 1]

    os.chdir(ROOT)
    units = bd.parse_units()
    _, _, cflags, toolchain, _ = units[unit]
    cc = os.path.join(ROOT, 'build', 'compilers', toolchain, 'ngccc.exe')
    env = dict(os.environ)
    env['SN_NGC_PATH'] = os.path.join(ROOT, 'build', 'compilers', toolchain)
    os.makedirs(SCR, exist_ok=True)
    out = os.path.join(SCR, os.path.basename(src)[:-4] + '.s')
    cmd = [cc] + cflags + ['-S', '-o', out, src]
    p = subprocess.run(cmd, cwd=ROOT, env=env, capture_output=True, text=True)
    err = (p.stderr or '') + (p.stdout or '')
    if p.returncode != 0:
        print('FALLO')
        print(err.strip()[:4000])
        return 1
    if err.strip():
        print(err.strip()[:2000])
    print('ok -> %s' % out)
    if fn:
        show(out, fn)
    return 0


def show(path, fn):
    """Imprime el cuerpo de una funcion del .s (por simbolo mangled)."""
    lines = open(path, encoding='utf-8', errors='replace').read().split('\n')
    start = None
    for i, l in enumerate(lines):
        if l.startswith(fn + ':'):
            start = i
            break
    if start is None:
        print('no encontrado: %s' % fn)
        return
    print('--- %s ---' % fn)
    n = 0
    for l in lines[start:]:
        s = l.strip()
        if s.startswith('.size') or s.startswith('.Lfe'):
            break
        if s.startswith('.') and not s.startswith('.L'):
            continue
        if s.startswith('.L') and (s.endswith(':') or True) and (
                '4byte' in s or '2byte' in s or 'previous' in s):
            continue
        if s and not s.startswith('#'):
            print('  ' + s)
            n += 1
        if n > 400:
            break


if __name__ == '__main__':
    sys.exit(main())
