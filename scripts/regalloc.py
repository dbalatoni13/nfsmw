#!/usr/bin/env python3
"""regalloc.py -- vuelca la tabla de allocnos de GCC 2.9 (refs y LONGITUD DE
VIDA de cada pseudo) de NUESTRO codigo. Es el oraculo del reparto de registros.

    python scripts/regalloc.py <unidad> [SubcadenaDeFuncion]

    python scripts/regalloc.py smixer MIX_playinit
    python scripts/regalloc.py Speed/Indep/Libs/snd/9/source/library/mix/smixer
    python scripts/regalloc.py zDynamics Articulation

<unidad> acepta lo mismo que build_direct.py: nombre corto, ruta o subcadena.
Los cflags se sacan de build.ninja, asi que vale para CUALQUIER biblioteca o
SourceList, no solo para snd.

QUE IMPRIME
    ;; Function MIX_playinit__Fii...
      Register 88 used 2 times across 60 insns; ...
Un pseudo por linea. Se numeran POR ORDEN DE DECLARACION: primero los
parametros en orden de la firma, luego las locales. Se identifican contando.

PARA QUE SIRVE
GCC 2.9 (global.c) ordena los allocnos por
    prioridad = floor_log2(refs) * refs * size / live_length
y reparte de mayor a menor sobre la lista de registros salvados (r31, r30, ...).
    -> vida MAS CORTA = se reparte ANTES = registro MAS ALTO.
    -> los EMPATES se rompen por numero de pseudo = ORDEN DE DECLARACION.
Cuando el objetivo asigna, p.ej., r26/r25/r24 a los parametros 4/7/8 y nosotros
los damos permutados, la causa suele ser UNA instruccion de diferencia en la
longitud de vida: se lee aqui, se ve cuanta falta, y se mueve una sentencia del
fuente hasta que la vida del que debe ir primero sea <= la del otro (con que
empaten basta, porque el desempate ya es el orden de declaracion).
Convierte permutar a ciegas en medir: en MIX_playinit (532 B) 250 permutaciones
ciegas se quedaron en 99,91 % y 7 pruebas dirigidas dieron el 100 %.

NOTAS
- El volcado .lreg trae el RTL YA PLANIFICADO por sched1, que es donde se miden
  las vidas. NO es el orden final del .s: sched2 vuelve a reordenar despues.
- Solo mide NUESTRO codigo. El objetivo no tiene .lreg; lo que se compara es la
  asignacion final de registros que se ve en el desensamblado.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from build_direct import parse_units

ROOT = os.getcwd()
SCRATCH = os.environ.get(
    'SCRATCH',
    'C:/Users/jferr/AppData/Local/Temp/claude/C--Users-jferr-Desktop-nfsdecompiled/'
    '3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad')

# Los que cc1plus acepta directamente; el resto (-I, -D, -g...) los consume el
# preproceso. -dl es el que deja el volcado tras local-alloc.
CC1_KEEP = re.compile(r'^-(O\d|f[\w-]+|m[\w-]+|G\d|W[\w-]*)$')


def pick(units, want):
    # exacto primero: 'sst' no debe casar con sstautov, sstop, ...
    exact = {k: v for k, v in units.items()
             if want == k or want == os.path.splitext(os.path.basename(v[0]))[0]}
    hits = exact or {k: v for k, v in units.items()
                     if want in v[1] or want in v[0]}
    if not hits:
        sys.exit('no encuentro la unidad: %s' % want)
    if len(hits) > 1:
        sys.exit('ambiguo:\n  ' + '\n  '.join(sorted(hits)))
    return next(iter(hits.values()))


def dump(spec):
    src, out, cflags, toolchain, _rename = spec
    cc = os.path.join(ROOT, 'build', 'compilers', toolchain)
    env = dict(os.environ, SN_NGC_PATH=cc)
    tag = os.path.splitext(os.path.basename(src))[0]
    os.makedirs(SCRATCH, exist_ok=True)
    i = os.path.join(SCRATCH, '_ra_%s.i' % tag)
    r = subprocess.run([os.path.join(cc, 'ngccc.exe')] + cflags + ['-E', src, '-o', i],
                       cwd=ROOT, env=env, capture_output=True, text=True)
    if r.returncode:
        sys.exit('preproceso fallido:\n' + r.stdout + r.stderr)
    cc1 = ['-quiet'] + [f for f in cflags if CC1_KEEP.match(f)] + ['-dl']
    r = subprocess.run([os.path.join(cc, 'cc1plus.exe'), os.path.basename(i)] + cc1
                       + ['-o', '_ra_%s.s' % tag],
                       cwd=SCRATCH, env=env, capture_output=True, text=True)
    path = i + '.lreg'
    if not os.path.isfile(path):
        sys.exit('cc1plus no dejo .lreg:\n' + r.stdout + r.stderr)
    return open(path, encoding='utf-8', errors='replace').read()


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    text = dump(pick(parse_units(), sys.argv[1]))
    want = sys.argv[2] if len(sys.argv) > 2 else None
    shown = 0
    for chunk in text.split(';; Function')[1:]:
        head = chunk.split('\n', 1)[0].strip()
        if want and want not in head:
            continue
        shown += 1
        print(';; Function ' + head)
        for m in re.finditer(r'^Register \d+ used .*$', chunk, re.M):
            print('  ' + m.group(0))
        print()
    if want and not shown:
        sys.exit('ninguna funcion contiene %r en esta unidad' % want)
    return 0


if __name__ == '__main__':
    sys.exit(main())
