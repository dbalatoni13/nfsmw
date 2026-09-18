#!/usr/bin/env python3
"""schedtrace.py -- la TRAZA del planificador, ciclo a ciclo, con lista de listos.

`rtldump.py` solo deja pasar banderas `-d*`, y la que hace falta para dejar de
adivinar por que sched coloca una instruccion donde la coloca es `-fsched-verbose-N`
(haifa-sched.c). Con ella el volcado `.sched` / `.sched2` trae, por bloque basico:

    ;;      Ready list (t = 15):    270  244  264
    ;;              --> scheduling insn <<<264>>> on unit iu2
    ;;              --> scheduling insn <<<244>>> on unit lsu

es decir: en que CICLO entra cada insn, en que UNIDAD, y contra quien competia.
Eso distingue las tres causas que se confunden siempre --prioridad, clase
respecto al ultimo programado, y simple relleno de ranura de latencia-- y en la
r48 desmintio el diagnostico "desempate por LUID" de `SetMemoryPoolSize`: las dos
instrucciones que salen en orden distinto se emiten EN EL MISMO CICLO, en
unidades distintas.

    python scripts/schedtrace.py <unidad> <fnfiltro> [-dS] [-dR] [-fsched-verbose-2]
    python scripts/schedtrace.py --file <ruta.cpp> --like <unidad> <fnfiltro> [...]

Por defecto pasa `-dS -dR -fsched-verbose-2` (`-dS` = tras sched1, `-dR` = tras
sched2). Recorta la funcion pedida a `scratchpad/schedtrace/<fn>.<pasada>`.

AVISO MEDIDO (r48): con `-fsched-verbose-N` cc1plus se cae con "Internal compiler
error" en algunas unidades grandes (zWorld en una plantilla de STL, zWorld2 en
WCollider.cpp), y entonces el volcado se corta antes de tu funcion. La salida
avisa. La vuelta es `--file` con una TU REDUCIDA: coge el preambulo de la
SourceList (los `#define`/`asm` de cabecera, que son load-bearing) y deja un solo
`#include` del .cpp que te interesa. En la r48 la TU reducida de WRoadNetwork.cpp
reprodujo `HolePunchAvoidables` BYTE A BYTE (46 filas / 2980 B / 97,25638 %,
identico al build entero) y compila en 11 s en vez de 40.
"""
import os
import re
import subprocess as SP
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')).replace(os.sep, '/')
sys.path.insert(0, ROOT + '/scripts')
import build_direct as BD  # noqa: E402

TOOLS = os.path.join(ROOT, 'build', 'compilers')
TMP = os.path.join(ROOT, 'scratchpad', 'schedtrace')
KEEP = re.compile(r'^-(O|f|m|G|g|W|std=|ansi|pedantic)')
PASSES = {'-dS': 'sched', '-dR': 'sched2', '-dg': 'greg', '-dl': 'lreg', '-dc': 'combine'}


def main():
    a = sys.argv[1:]
    if not a:
        sys.exit(__doc__)
    srcfile = None
    if a[0] == '--file':
        srcfile = a[1]
        a = a[2:]
        if a[0] != '--like':
            sys.exit('--file exige --like <unidad> (de donde salen los cflags)')
        like = a[1]
        a = a[2:]
    else:
        like = a[0]
        a = a[1:]
    fnfilter = a[0] if a and not a[0].startswith('-') else ''
    flags = [x for x in a if x.startswith('-')]
    if not any(f in PASSES for f in flags):
        flags += ['-dS', '-dR']
    if not any(f.startswith('-fsched-verbose') for f in flags):
        flags += ['-fsched-verbose-2']

    os.chdir(ROOT)
    units = BD.parse_units()
    key = [k for k in units if k.endswith(like) or k == like]
    if not key:
        sys.exit('unidad no encontrada: ' + like)
    spec = units[key[0]]
    src_rel, cflags, toolchain = spec[0], spec[2], spec[3]
    if srcfile:
        src_rel = srcfile
    tools = os.path.join(TOOLS, toolchain)
    env = dict(os.environ)
    env['SN_NGC_PATH'] = tools
    os.makedirs(TMP, exist_ok=True)
    tag = os.path.basename(src_rel).replace('.', '_')
    base = os.path.join(TMP, tag)

    p = SP.run([os.path.join(tools, 'ngccc.exe')] + cflags + ['-E', '-o', base + '.i', src_rel],
               cwd=ROOT, env=env, capture_output=True, text=True)
    if p.returncode:
        sys.exit('preproceso fallido: ' + p.stderr[-1500:])
    keep = [f for f in cflags if KEEP.match(f)]
    p = SP.run([os.path.join(tools, 'cc1plus.exe')] + keep + flags + ['-quiet', '-o', base + '.s', base + '.i'],
               cwd=TMP, env=env, capture_output=True, text=True)
    if 'Internal compiler error' in p.stderr:
        print('*** cc1plus SE CAYO con -fsched-verbose (bug conocido). El volcado esta CORTADO.')
        print('*** Prueba con --file sobre una TU reducida; mira la cabecera de este script.')
    print('cflags:', ' '.join(keep + flags))

    found = False
    for flag, suf in PASSES.items():
        f = base + '.i.' + suf
        if flag not in flags or not os.path.exists(f):
            continue
        t = open(f, encoding='utf-8', errors='replace').read().split('\n')
        heads = [i for i, l in enumerate(t) if l.startswith(';; Function') and fnfilter in l]
        print('  %-7s %8d B, %d funcion(es) con "%s"' % (suf, os.path.getsize(f), len(heads), fnfilter))
        for i in heads:
            name = t[i].replace(';; Function', '').strip()
            e = len(t)
            for j in range(i + 1, len(t)):
                if t[j].startswith(';; Function'):
                    e = j
                    break
            out = os.path.join(TMP, '%s.%s' % (re.sub(r'[^A-Za-z0-9_]', '_', name)[:60], suf))
            open(out, 'w', encoding='utf-8').write('\n'.join(t[i:e]))
            trace = [l for l in t[i:e] if l.startswith(';;') and
                     ('Ready list' in l or 'scheduling insn' in l or 'Q-->Ready' in l)]
            print('     %s  (%d lineas, %d de traza)' % (out, e - i, len(trace)))
            found = True
    if not found:
        print('  *** no aparece ninguna funcion con ese filtro: el volcado se corto antes.')


if __name__ == '__main__':
    main()
