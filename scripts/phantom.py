#!/usr/bin/env python3
"""RESUELTO. Este script buscaba funciones que objdiff daba por debajo del 100%
por una reubicacion `R_PPC_NONE`. La causa real era otra y ya esta arreglada:
faltaba pasarle a `objdiff-cli diff` la opcion

    -c ppc.calculatePoolRelocations=false

que `report generate` SI usa (esta en build.ninja, `objdiff_report_args`). Sin
ella, objdiff inventa reubicaciones para los accesos al pool de constantes y
luego las cuenta como diferencias. Escondia 48.348 B en 28 unidades.

Se deja el barrido por si vuelve a aparecer algo del estilo; hoy da cero.
"""
import json
import io
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
TMP = os.path.join(os.environ.get('TEMP', ROOT), 'objdiff_phantom')


def text(ins):
    i = ins.get('instruction') or {}
    return i.get('formatted') or i.get('mnemonic') or ''


def reloc_type(ins):
    i = ins.get('instruction') or {}
    r = i.get('relocation') or {}
    return r.get('type_name')


def scan(spec):
    name, tgt, base = spec
    out = os.path.join(TMP, 'p_%s.json' % name.replace('/', '_'))
    r = subprocess.run([CLI, 'diff', '-1', tgt, '-2', base,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false', '-o', out,
                        '--format', 'json'], capture_output=True, text=True)
    if r.returncode != 0:
        return []
    try:
        d = json.load(open(out, encoding='utf-8'))
    except Exception:
        return []
    finally:
        try:
            os.remove(out)
        except OSError:
            pass
    right = {s['name']: s for s in d['right']['symbols']}
    hits = []
    for s in d['left']['symbols']:
        if s.get('kind') != 'SYMBOL_FUNCTION':
            continue
        p = s.get('match_percent') or 0
        if p >= 100.0 or p < 90.0:
            continue
        o = right.get(s['name'])
        if not o:
            continue
        L, R = s.get('instructions', []), o.get('instructions', [])
        if len(L) != len(R):
            continue
        phantom = True
        idxs = []
        for k, (a, b) in enumerate(zip(L, R)):
            if a.get('diff_kind') in (None, 'DIFF_NONE') and b.get('diff_kind') in (None, 'DIFF_NONE'):
                continue
            # unica diferencia admitida: mismo texto y una reubicacion nula
            if text(a) != text(b):
                phantom = False
                break
            if 'R_PPC_NONE' not in (reloc_type(a), reloc_type(b)):
                phantom = False
                break
            idxs.append(k)
        if phantom and idxs:
            hits.append((int(s['size']), p, name, s['name'], idxs))
    return hits


def addresses(unit, fn, idxs):
    """Direccion absoluta de las instrucciones i-esimas de `fn` en el .s del
    objetivo. block_relocations las quiere como `.text:0xXXXXXXXX`."""
    u = unit[5:] if unit.startswith('main/') else unit
    path = os.path.join(ROOT, 'build', 'GOWE69', 'asm', u.replace('/', os.sep) + '.s')
    if not os.path.exists(path):
        return []
    import re as _re
    cur, n, out = None, 0, []
    want = set(idxs)
    for line in io.open(path, encoding='utf-8', errors='replace'):
        m = _re.match(r'^\.fn (\S+?),', line)
        if m:
            cur, n = m.group(1), 0
            continue
        if cur != fn:
            continue
        m = _re.match(r'^/\* ([0-9A-F]{8}) ', line)
        if not m:
            continue
        if n in want:
            out.append(m.group(1))
        n += 1
    return out


def main():
    filters = [a for a in sys.argv[1:] if not a.startswith('-')]
    cfg = json.load(open(os.path.join(ROOT, 'objdiff.json'), encoding='utf-8'))
    specs = []
    for u in cfg['units']:
        n = u['name']
        if filters and not any(f in n for f in filters):
            continue
        t = u.get('target_path')
        if not t:
            continue
        b = u.get('base_path') or t.replace('build/GOWE69/obj/', 'build/GOWE69/src/', 1)
        t, b = os.path.join(ROOT, t), os.path.join(ROOT, b)
        if os.path.exists(t) and os.path.exists(b):
            specs.append((n, t, b))
    os.makedirs(TMP, exist_ok=True)
    hits = []
    with ThreadPoolExecutor(max_workers=8) as ex:
        for h in ex.map(scan, specs):
            hits.extend(h)
    hits.sort(reverse=True)
    salida = []
    for size, p, unit, fn, idxs in hits:
        addrs = addresses(unit, fn, idxs)
        print('%7d B  %7.3f%%  %-2d nula(s)  %-40s %s'
              % (size, p, len(idxs), unit.replace('main/', '')[-40:], fn[:52]))
        for a in addrs:
            salida.append('- source: .text:0x%s' % a)
    print()
    print('%d funciones, %d B escondidos por reubicaciones nulas'
          % (len(hits), sum(h[0] for h in hits)))
    if salida:
        print()
        print('# para block_relocations de config/GOWE69/config.yml:')
        for l in salida:
            print(l)


if __name__ == '__main__':
    main()
