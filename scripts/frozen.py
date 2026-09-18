#!/usr/bin/env python3
"""frozen.py -- congela el diff de una unidad y comprueba que no ha cambiado.

Tomado del arnes que jferr tiene en `scratchpad/phase13_refresh_and_check.py`.
Resuelve dos cosas que `pctsnap.py` no ve:

  1. **`pctsnap` solo mira el porcentaje.** Una funcion puede cambiar de codigo
     sin mover el porcentaje, y eso pasa desapercibido. Comparar el diff ENTERO
     caza cualquier cambio.
  2. **`line_number` cambia al anadir un comentario.** Sin quitarlo, cada nota
     que escribes en el fuente hace que el objeto "difiera" aunque no cambie ni
     una instruccion -- me ha costado bisecar varias veces esta sesion.

    python scripts/frozen.py cong <unidad> [<unidad>...]   # congela
    python scripts/frozen.py chk                           # comprueba todas
    python scripts/frozen.py chk <unidad>                  # comprueba una
    python scripts/frozen.py ls                            # que hay congelado

La unidad se escribe como la ruta del objeto sin extension, tal como la usan
objdiff y measure.py:

    Speed/Indep/SourceLists/zWorld
    Speed/Indep/Libs/snd/9/source/library/gc/ssdfx
    ppc2D2
"""
import hashlib
import json
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
CONG = os.path.join(ROOT, 'docs', 'congelado')
SCR = os.environ.get('SCRATCH', os.path.join(ROOT, 'build'))


# Secciones que NO dicen nada del codigo emitido y ensucian la huella. Nuestro
# objeto se compila con `-g` y el objetivo troceado no, asi que nuestro lado
# lleva `.debug` (2,8 kB en sndvd), `.line` (1,7 kB) y cinco `.rela.debug*`:
# todas crecen o menguan al anadir un comentario o mover una linea, con CERO
# cambio de instrucciones. Con ellas dentro, la huella cambiaba con codigo
# identico y `chk` gritaba en falso -- en la r79 casi todo docs/congelado
# estaba rancio por esto.
METADATOS = ('.debug', '.line', '.rela.debug', '.rela.line', '.comment',
             '.note', '.symtab', '.strtab', '.shstrtab')


def sin_lineas(v):
    """Quita `line_number` en todo el arbol: cambia al mover una linea de fuente
    y no dice nada sobre el codigo emitido."""
    if isinstance(v, dict):
        return {k: sin_lineas(x) for k, x in v.items() if k != 'line_number'}
    if isinstance(v, list):
        return [sin_lineas(x) for x in v]
    return v


def sin_metadatos(d):
    """Deja fuera de la huella las secciones de depuracion y de tablas."""
    for lado in ('left', 'right'):
        s = d.get(lado)
        if isinstance(s, dict) and isinstance(s.get('sections'), list):
            s['sections'] = [x for x in s['sections']
                             if not str(x.get('name', '')).startswith(METADATOS)]
    return d


def diff(unit):
    a = os.path.join(ROOT, 'build', 'GOWE69', 'obj', unit.replace('/', os.sep) + '.o')
    b = os.path.join(ROOT, 'build', 'GOWE69', 'src', unit.replace('/', os.sep) + '.o')
    if not os.path.exists(a):
        return None, 'no hay objeto OBJETIVO'
    if not os.path.exists(b):
        return None, 'no hay objeto NUESTRO (compila antes)'
    out = os.path.join(SCR, 'frozen_%s.json' % unit.replace('/', '_'))
    r = subprocess.run([CLI, 'diff', '-1', a, '-2', b,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false',
                        '--format', 'json', '-o', out],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(out):
        return None, 'objdiff falla'
    try:
        d = json.load(open(out, encoding='utf-8'))
    finally:
        try:
            os.remove(out)
        except OSError:
            pass
    return sin_lineas(sin_metadatos(d)), None


def huella(d):
    return hashlib.sha256(json.dumps(d, sort_keys=True).encode()).hexdigest()


def ruta(unit):
    return os.path.join(CONG, unit.replace('/', '__') + '.json')


def congela(units):
    os.makedirs(CONG, exist_ok=True)
    for u in units:
        d, err = diff(u)
        if err:
            print('  %-58s %s' % (u, err))
            continue
        with open(ruta(u), 'w', encoding='utf-8') as f:
            # El diff entero de una unidad grande pesa 22 MB, asi que se guarda
            # su HUELLA. Para saber "ha cambiado?" basta, y si cambia se vuelve
            # a diffear para ver que.
            json.dump({'sha256': huella(d), 'unidad': u}, f)
        print('  congelado  %-52s %s' % (u, huella(d)[:16]))


def comprueba(units):
    if not units:
        if not os.path.isdir(CONG):
            print('no hay nada congelado todavia')
            return 0
        units = [f[:-5].replace('__', '/') for f in sorted(os.listdir(CONG))
                 if f.endswith('.json')]
    malas = 0
    for u in units:
        p = ruta(u)
        if not os.path.exists(p):
            print('  SIN CONGELAR           %s' % u)
            continue
        d, err = diff(u)
        if err:
            print('  %-22s %s' % (err.upper(), u))
            malas += 1
            continue
        if huella(d) == json.load(open(p, encoding='utf-8')).get('sha256'):
            print('  identico al congelado  %s' % u)
        else:
            print('  *** HA CAMBIADO ***    %s' % u)
            malas += 1
    return malas


def main():
    a = sys.argv[1:]
    if not a or a[0] in ('-h', '--help'):
        print(__doc__)
        return 0
    cmd, units = a[0], a[1:]
    if cmd == 'cong':
        if not units:
            print('di que unidades congelar')
            return 2
        congela(units)
        return 0
    if cmd == 'chk':
        return 1 if comprueba(units) else 0
    if cmd == 'ls':
        if not os.path.isdir(CONG):
            print('no hay nada congelado')
            return 0
        for f in sorted(os.listdir(CONG)):
            if f.endswith('.json'):
                print('  %s' % f[:-5].replace('__', '/'))
        return 0
    print('orden desconocida: %s' % cmd)
    return 2


if __name__ == '__main__':
    sys.exit(main())
