#!/usr/bin/env python3
"""pctall.py [--solo-informe] [VERSION ...] -- el porcentaje de TODAS las versiones, no solo de GameCube.

Es la medida del metodo de upstream: un informe de objdiff por version, con codigo real que compile en
todas. `ninja progress` y `scripts/estado.py` solo leen `build/GOWE69/report.json`, y eso deja sin
comprobar lo unico que distingue codigo real de un match apoyado en andamios -- que la MISMA fuente
compile tambien con el compilador de otra plataforma.

    python scripts/pctall.py                     # las cinco versiones: configura, compila y mide
    python scripts/pctall.py --solo-informe      # no compila: mide los objetos que ya haya
    python scripts/pctall.py SLUS-21351          # una sola

Para cada version: `configure.py --version V`, `ninja -k 0` (sigue aunque falle alguna unidad), informe de
objdiff con las opciones del proyecto, y una linea con codigo casado, funciones y CUANTOS OBJETOS NO
COMPILAN. Al terminar deja configurada GOWE69 otra vez. Salida: `build/versiones/<V>.json` (informe) y
`<V>.log` (build).

LA CIFRA NO VALE SI NO ES "0 objetos no compilan". Tres trampas medidas, todas tapadas aqui:

  1. objdiff cuenta al 100 % una unidad marcada `complete` que NO tiene objeto base. Una version donde
     medio arbol no compila salia con un porcentaje altisimo. Aqui cada unidad cuyo objeto falta o es de
     un build que fallo pasa a `complete: false` y se le quita `base_path`.
  2. Si el TROCEADO falla, `configure.py` no reescribe `objdiff.json` y queda el de la version anterior:
     se acaba midiendo una version como si fuera otra. Aqui se comprueba que las rutas del objdiff.json
     son de esta version antes de medir.
  3. `configure.py --version V` puede terminar OK y no emitir ni una regla de compilacion (si falta
     `build/<V>/config.json`, `build_config` queda en None y no se genera nada, sin aviso).

Y dos detalles de entorno: Xbox 360 trocea con `jeff.exe` (la variante de dtk para .xex), no con dtk; y
`cl.exe` da C1083 ("no abre una cabecera que existe") con mucha carga en paralelo, asi que ninja se
relanza hasta tres veces y solo recompila lo que fallo.
"""
import json
import os
import re
import subprocess
import sys

# Se ejecuta desde la raiz del repo (o de un worktree); la salida va a <repo>/build/versiones/.
ROOT = os.getcwd().replace(chr(92), '/') + '/'
S = ROOT + 'build/versiones/'
TODAS = ['GOWE69', 'SLES-53558-A124', 'SLUS-21351', 'EUROPEGERMILESTONE', 'SPEED_EXE_1_3']

if any(a in ('-h', '--help') for a in sys.argv[1:]):
    print(__doc__)
    sys.exit(0)

os.makedirs(S, exist_ok=True)
args = [a for a in sys.argv[1:] if not a.startswith('--')]
solo_informe = '--solo-informe' in sys.argv
versiones = args or TODAS


def corre(cmd, log=None):
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, errors='replace')
    if log:
        open(log, 'w', encoding='utf-8').write(r.stdout + r.stderr)
    return r


def herramientas(v):
    """Rutas de las herramientas ya presentes, para que ninja no las vuelva a descargar. Xbox 360 trocea con
    jeff (la variante de dtk para xex), no con dtk."""
    out = []
    dtk = 'jeff.exe' if v == 'EUROPEGERMILESTONE' else 'dtk.exe'
    for opcion, fichero in (('--dtk', dtk), ('--objdiff', 'objdiff-cli.exe'), ('--sjiswrap', 'sjiswrap.exe'),
                            ('--delink', 'delink.exe')):
        p = 'build/tools/' + fichero
        if os.path.exists(ROOT + p):
            out += [opcion, p]
    return out


filas = []
try:
    for v in versiones:
        r = corre([sys.executable, 'configure.py', '--version', v, *herramientas(v)])
        if r.returncode:
            filas.append((v, 'configure fallo', r.stderr[-200:]))
            continue
        if not solo_informe:
            for intento in range(3):
                b = corre([sys.executable, '-m', 'ninja', '-k', '0'], S + v + '.log')
                if 'FAILED:' not in b.stdout or 'C1083' not in b.stdout:
                    break
        log = open(S + v + '.log', encoding='utf-8', errors='replace').read() if os.path.exists(S + v + '.log') else ''
        fallos = re.findall(r'FAILED: (?:\[code=\d+\] )?(\S+)', log)
        fallidos = {os.path.normpath(f) for f in fallos}
        if any(f.endswith('config.json') for f in fallos):
            filas.append((v, 'fallo el troceado (config.json): no se mide con objetos de otro build', ''))
            continue
        pj = ROOT + 'objdiff.json'
        cfg = json.load(open(pj, encoding='utf-8'))
        propio = 'build/%s/' % v
        if not any(propio in (u.get('target_path', '') + u.get('base_path', '')).replace(chr(92), '/')
                   for u in cfg.get('units', [])):
            filas.append((v, 'objdiff.json no es de esta version (fallo el troceado o configure)',
                          ' '.join(f.split('/')[-1] for f in fallos[:12])))
            continue
        sin_objeto = 0
        for u in cfg.get('units', []):
            bp = u.get('base_path')
            if bp and (not os.path.exists(os.path.join(ROOT, bp)) or os.path.normpath(bp) in fallidos):
                del u['base_path']
                u.setdefault('metadata', {})['complete'] = False
                sin_objeto += 1
        json.dump(cfg, open(pj, 'w', encoding='utf-8'), indent=2)
        rep = S + v + '.json'
        if os.path.exists(rep):
            os.remove(rep)
        corre([ROOT + 'build/tools/objdiff-cli.exe', 'report', 'generate',
               '-c', 'functionRelocDiffs=none', '-c', 'ppc.calculatePoolRelocations=false', '-o', rep])
        if not os.path.exists(rep):
            filas.append((v, 'sin informe', ''))
            continue
        m = json.load(open(rep))['measures']
        filas.append((v, '%.2f %% codigo (%s/%s B), %s/%s funciones, %s unidades completas; %d objetos no compilan, %d unidades sin objeto' % (
            float(m.get('matched_code_percent', 0)), m.get('matched_code'), m.get('total_code'),
            m.get('matched_functions'), m.get('total_functions'), m.get('complete_units'), len(fallos), sin_objeto),
            ' '.join(f.split('/')[-1] for f in fallos[:12])))
finally:
    corre([sys.executable, 'configure.py'])

for v, res, extra in filas:
    print('%-20s %s' % (v, res))
    if extra:
        print('%-20s   %s' % ('', extra))
