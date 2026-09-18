#!/usr/bin/env python3
"""Medida independiente por unidad, abriendo los objetos originales y actuales.

Suma los bytes de las funciones al 100%. Cuando una unidad tiene asociaciones
de nombres en objdiff.json, usa el contexto de proyecto para aplicarlas; abrir
solo los dos objetos ignora esas asociaciones y produce falsos pendientes.
Usar --raw para reproducir las mediciones historicas anteriores a esta correccion.
Tomar snapshots una vez terminadas las compilaciones, no durante una escritura.

  python scripts/measure.py -o antes.json            # todas las unidades
  python scripts/measure.py -o antes.json path snd   # solo las que casen
  python scripts/measure.py --raw -o antes_raw.json  # sin asociaciones de nombres
  python scripts/measure.py --cmp antes.json despues.json

El denominador directo puede incluir simbolos adicionales frente al informe
oficial. Comparar snapshots hechos con el mismo modo y separar una correccion
de medicion de un cambio real de codigo. Esta medida todo-o-nada no sustituye
la auditoria de porcentajes por funcion, literales, datos y reubicaciones.
"""
import json
import os
import subprocess
import time
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
SCRATCH = os.environ.get('TEMP', ROOT)
FALLIDAS = []
MAPPED_UNITS = set()
RAW_SYMBOL_NAMES = False


def units(filters):
    cfg = json.load(open(os.path.join(ROOT, 'objdiff.json'), encoding='utf-8'))
    MAPPED_UNITS.clear()
    out = []
    for u in cfg['units']:
        name = u['name']
        if u.get('symbol_mappings'):
            MAPPED_UNITS.add(name)
        if filters and not any(f in name for f in filters):
            continue
        tgt = u.get('target_path')
        base = u.get('base_path')
        if not tgt:
            continue
        if not base:
            # la convencion del proyecto: el nuestro cuelga de src/ en vez de obj/
            base = tgt.replace('build/GOWE69/obj/', 'build/GOWE69/src/', 1)
        tgt = os.path.join(ROOT, tgt)
        base = os.path.join(ROOT, base)
        if os.path.exists(tgt):
            out.append((name, tgt, base if os.path.exists(base) else None))
    return out


REPORT = os.path.join(ROOT, 'build', 'GOWE69', 'report.json')
_target_sizes = None


def target_size(name):
    """Sin objeto nuestro no hay nada que casar, pero sus bytes SI cuentan en el
    denominador: es la diferencia entre el 76% de lo que compilamos y el 73%
    real del proyecto. El tamano del objetivo lo da report.json y ese dato no
    se queda rancio, porque no depende de nuestro lado."""
    global _target_sizes
    if _target_sizes is None:
        _target_sizes = {}
        try:
            d = json.load(open(REPORT, encoding='utf-8'))
            for u in d['units']:
                m = u['measures']
                _target_sizes[u['name']] = (int(m.get('total_code', 0)),
                                            int(m.get('total_functions', 0)))
        except Exception:
            pass
    tc, tf = _target_sizes.get(name, (0, 0))
    return (0, 0, tc, tf)


def one(spec, tmp):
    name, tgt, base = spec
    if base is None:
        return name, target_size(name)
    # El volcado intermedio lleva el PID: sin eso dos agentes midiendo la MISMA
    # unidad a la vez se pisan el fichero, y uno puede leer lo que el otro esta
    # escribiendo. No da error: da un numero plausible y falso.
    out = os.path.join(tmp, 'm_%s_%d.json' % (name.replace('/', '_'), os.getpid()))
    # Con varios agentes compilando a la vez, objdiff falla por contencion sobre
    # el .o. Antes se descartaba la unidad EN SILENCIO y el total bajaba sin que
    # nada lo dijera: una unidad perdida se lee igual que una regresion. Ahora se
    # reintenta y, si aun asi falla, se avisa por stderr y se marca.
    d = None
    selection = (['-p', ROOT, '-u', name]
                 if name in MAPPED_UNITS and not RAW_SYMBOL_NAMES
                 else ['-1', tgt, '-2', base])
    for intento in range(3):
        r = subprocess.run([CLI, 'diff'] + selection + [
                            '-c', 'function_reloc_diffs=none',
                            '-c', 'ppc.calculatePoolRelocations=false', '-o', out,
                            '--format', 'json'], capture_output=True, text=True)
        if r.returncode == 0:
            try:
                d = json.load(open(out, encoding='utf-8'))
                # borrar YA: estos volcados son de 20-35 MB y llenaron el disco
                # dos veces en una sola sesion.
                try:
                    os.remove(out)
                except OSError:
                    pass
                break
            except Exception:
                d = None
        time.sleep(0.4 * (intento + 1))
    if d is None:
        sys.stderr.write('AVISO: no se pudo medir %s (3 intentos)%s' % (name, chr(10)))
        FALLIDAS.append(name)
        return name, None
    try:
        pass
    finally:
        try:
            os.remove(out)
        except OSError:
            pass
    fns = [s for s in d['left']['symbols'] if s.get('kind') == 'SYMBOL_FUNCTION']
    ok = [s for s in fns if (s.get('match_percent') or 0) >= 100.0]
    return name, (sum(int(s['size']) for s in ok), len(ok),
                  sum(int(s['size']) for s in fns), len(fns))


def snapshot(filters, dest):
    specs = units(filters)
    tmp = os.path.join(SCRATCH, 'objdiff_measure')
    os.makedirs(tmp, exist_ok=True)
    res = {}
    with ThreadPoolExecutor(max_workers=8) as ex:
        for name, v in ex.map(lambda s: one(s, tmp), specs):
            if v:
                res[name] = v
    json.dump(res, open(dest, 'w', encoding='utf-8'))
    mb = sum(v[0] for v in res.values())
    tb = sum(v[2] for v in res.values())
    if FALLIDAS:
        print('OJO: %d unidades NO se pudieron medir y quedan fuera del total: %s'
              % (len(FALLIDAS), ', '.join(FALLIDAS[:6])))
    print('%d unidades  %d/%d B  %.4f%%  %d funciones al 100%%'
          % (len(res), mb, tb, 100.0 * mb / tb if tb else 0,
             sum(v[1] for v in res.values())))
    return res


def compare(a_path, b_path):
    a = json.load(open(a_path, encoding='utf-8'))
    b = json.load(open(b_path, encoding='utf-8'))
    rows = []
    for k in sorted(set(a) | set(b)):
        x, y = a.get(k), b.get(k)
        if not x or not y:
            continue
        if x[0] != y[0]:
            rows.append((y[0] - x[0], y[1] - x[1], k, x[0], y[0]))
    rows.sort()
    tot = sum(r[0] for r in rows)
    print('%+d B, %+d funciones, %d unidades cambian'
          % (tot, sum(r[1] for r in rows), len(rows)))
    for d, dn, k, x, y in rows:
        print('  %+8d B  %+4d fns  %-56s %d -> %d'
              % (d, dn, k.replace('main/', ''), x, y))
    neg = [r for r in rows if r[0] < 0]
    if neg:
        print('\nOJO: %d unidades BAJAN (%d B)' % (len(neg), sum(r[0] for r in neg)))


def main():
    global RAW_SYMBOL_NAMES
    a = sys.argv[1:]
    RAW_SYMBOL_NAMES = '--raw' in a
    if '--cmp' in a:
        i = a.index('--cmp')
        return compare(a[i + 1], a[i + 2])
    dest = 'medida.json'
    if '-o' in a:
        i = a.index('-o')
        dest = a[i + 1]
        a = a[:i] + a[i + 2:]
    snapshot([x for x in a if not x.startswith('-')], dest)


if __name__ == '__main__':
    main()
