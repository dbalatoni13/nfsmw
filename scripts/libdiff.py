#!/usr/bin/env python3
"""libdiff.py — compila y mide una unidad de MIDDLEWARE, sin pasar por ninja.

`build_direct.py` necesita que la unidad esté dada de alta en `configure.py`;
esto no, así que sirve para iterar sobre un fichero que aún no has registrado.
Saca los cflags de la unidad de `build.ninja` si está, y si no los deduce de
otra unidad de la misma biblioteca.

    python scripts/libdiff.py Packages/vp6/1.0.6/source/decode/cmn/deblock
        compila src/<ruta>.c (o .cpp) y lista match% y tamaño por función.

    python scripts/libdiff.py <ruta> DeblockPlane
        diff lado a lado: izquierda = objetivo, derecha = el nuestro.

    python scripts/libdiff.py <ruta> DeblockPlane --only
        sólo las líneas que difieren.

    python scripts/libdiff.py <ruta> DeblockPlane --lines
        desensamblado del OBJETIVO con la línea de fuente de cada instrucción,
        sacada de `symbols/debug_lines.txt` (propaga la línea anterior, porque
        el mapa sólo anota cuando cambia). No todas las bibliotecas tienen mapa
        de líneas: si sale todo en la línea 0, esa unidad no lo trae.
"""
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJDIFF = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
TMP = os.path.join(ROOT, 'build', 'GOWE69', 'libdiff')


def cflags_for(unit):
    """cflags de la unidad leídos de build.ninja; si no está, los de una hermana.

    La hermana se busca por el DIRECTORIO MÁS LARGO que coincida, y bajando de
    uno en uno. Antes el criterio eran los DOS primeros componentes de la ruta
    (`Speed/Indep`), así que una unidad de middleware sin regla todavía en
    build.ninja se compilaba con los cflags del CÓDIGO DE JUEGO: la medida salía
    plausible y falsa (`ssysserv` daba 99,487% en vez del 100% real, y el
    `.s` culpaba a la fuente de un `lwzx` con los operandos al revés).
    """
    sys.path.insert(0, os.path.join(ROOT, 'scripts'))
    import build_direct
    cwd = os.getcwd()
    os.chdir(ROOT)
    try:
        units = build_direct.parse_units()
    finally:
        os.chdir(cwd)
    specs = [(spec[1].replace('\\', '/'), spec) for spec in units.values()]
    for out, spec in specs:
        if '/' + unit + '.o' in out:
            return spec[2], spec[3]
    parts = unit.split('/')
    for n in range(len(parts) - 1, 0, -1):
        pref = '/' + '/'.join(parts[:n]) + '/'
        for out, spec in specs:
            if pref in out:
                sys.stderr.write('AVISO: %s no tiene regla en build.ninja; uso los '
                                 'cflags de %s%s' % (unit, out, chr(10)))
                return spec[2], spec[3]
    sys.exit('no encuentro cflags para %s (ni para su biblioteca)' % unit)


def compile_unit(unit):
    src = None
    for ext in ('.c', '.cpp'):
        p = os.path.join(ROOT, 'src', *(unit + ext).split('/'))
        if os.path.exists(p):
            src = 'src/' + unit + ext
            break
    if src is None:
        sys.exit('no existe src/%s.c ni .cpp' % unit)
    cflags, toolchain = cflags_for(unit)
    out = os.path.join(TMP, unit.replace('/', '_') + '.o')
    os.makedirs(TMP, exist_ok=True)
    cc = os.path.join(ROOT, 'build', 'compilers', toolchain, 'ngccc.exe')
    env = dict(os.environ)
    env['SN_NGC_PATH'] = os.path.dirname(cc)
    p = subprocess.run([cc] + cflags + ['-c', '-o', out, src],
                       cwd=ROOT, env=env, capture_output=True, text=True)
    err = (p.stderr or '') + (p.stdout or '')
    if p.returncode != 0:
        sys.exit('COMPILE FAILED\n' + err)
    if err.strip():
        sys.stderr.write(err)
    return out


def run_objdiff(unit, ours):
    tgt = 'build/GOWE69/obj/' + unit + '.o'
    js = os.path.join(TMP, unit.replace('/', '_') + '.json')
    subprocess.run([OBJDIFF, 'diff', '-1', tgt, '-2', ours,
                    '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false', '-o', js, '--format', 'json'],
                   cwd=ROOT, capture_output=True, text=True)
    if not os.path.exists(js):
        sys.exit('objdiff no ha producido salida; falta %s?' % tgt)
    return json.load(open(js, encoding='utf-8'))


def report(d):
    rows, tot, mat = [], 0, 0
    for si in d['left']['symbols']:
        if si.get('kind') != 'SYMBOL_FUNCTION':
            continue
        sz = int(si.get('size', 0) or 0)
        pc = float(si.get('match_percent', 0.0) or 0.0)
        tot += sz
        if pc >= 100.0:
            mat += sz
        rows.append((pc, sz, si['name']))
    for pc, sz, n in sorted(rows, key=lambda r: -r[1]):
        print('%s%7.3f%%  %6d  %s' % ('OK  ' if pc >= 100.0 else '    ', pc, sz, n))
    print('---- code %d/%d = %.2f%%' % (mat, tot, 100.0 * mat / tot if tot else 0))


def side_by_side(d, fn, only):
    def rows(side):
        for si in d[side]['symbols']:
            if si.get('name') == fn and si.get('kind') == 'SYMBOL_FUNCTION':
                return [(i.get('diff_kind', '').replace('DIFF_', ''),
                         i['instruction']['formatted'] if i.get('instruction') else '')
                        for i in si.get('instructions', [])]
        return []
    L, R = rows('left'), rows('right')
    for i in range(max(len(L), len(R))):
        lk, lt = L[i] if i < len(L) else ('', '')
        rk, rt = R[i] if i < len(R) else ('', '')
        same = lk in ('', 'NONE') and rk in ('', 'NONE')
        if only and same:
            continue
        print('%-44s %s %-44s %s' % (lt, ' ' if same else '|', rt, lk or rk))


def show_lines(unit, fn):
    asm = os.path.join(ROOT, 'build', 'GOWE69', 'asm', *(unit + '.s').split('/'))
    if not os.path.exists(asm):
        sys.exit('no existe %s' % asm)
    re_insn = re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}\s+([0-9A-F ]+)\*/\s*(.*)$')
    insns, cur = [], None
    for l in open(asm, encoding='utf-8', errors='replace'):
        m = re.match(r'^\.fn (\S+?),', l)
        if m:
            cur = m.group(1)
            continue
        if l.startswith('.endfn'):
            cur = None
            continue
        if cur == fn:
            m = re_insn.match(l)
            if m:
                insns.append((int(m.group(1), 16), m.group(3).rstrip()))
            elif l.startswith('.L'):
                insns.append((None, l.rstrip()))
    if not insns:
        sys.exit('no encuentro %s en %s' % (fn, unit))
    lo = min(a for a, _ in insns if a)
    hi = max(a for a, _ in insns if a)
    lines, re_l = {}, re.compile(r'^0x([0-9A-Fa-f]+): (\S+) \(line (\d+)\)')
    with open(os.path.join(ROOT, 'symbols', 'debug_lines.txt'),
              encoding='utf-8', errors='replace') as fh:
        for l in fh:
            m = re_l.match(l)
            if m:
                a = int(m.group(1), 16)
                if lo <= a <= hi:
                    lines[a] = (os.path.basename(m.group(2)), int(m.group(3)))
    last = ('', 0)
    for a, txt in insns:
        if a is None:
            print('%-22s %s' % ('', txt))
            continue
        if a in lines:
            last = lines[a]
        print('%-16s %-5d  %s' % (last[0], last[1], txt))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args:
        sys.exit(__doc__)
    unit = args[0].replace('\\', '/').rstrip('/')
    if '--lines' in sys.argv:
        if len(args) < 2:
            sys.exit('uso: libdiff.py <ruta> <Funcion> --lines')
        return show_lines(unit, args[1])
    d = run_objdiff(unit, compile_unit(unit))
    if len(args) >= 2:
        side_by_side(d, args[1], '--only' in sys.argv)
    else:
        report(d)


main()
