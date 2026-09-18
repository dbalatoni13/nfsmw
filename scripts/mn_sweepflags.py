#!/usr/bin/env python3
"""mn_sweepflags.py <unidad> <src.cpp> <sym>  -- un flag por compilacion, imprime el diff resumido."""
import json
import os
import subprocess
import sys

ROOT = 'C:/Users/jferr/Desktop/nfsdecompiled'
SCR = os.environ.get('SCRATCH')
unit, src, sym = sys.argv[1], sys.argv[2], sys.argv[3]

FLAGS = ['', '-fno-schedule-insns', '-fno-schedule-insns2', '-fno-gcse', '-fno-cse-follow-jumps',
         '-fno-cse-skip-blocks', '-fno-expensive-optimizations', '-fno-force-mem', '-fno-force-addr',
         '-fno-rerun-cse-after-loop', '-fno-rerun-loop-opt', '-fno-move-all-movables',
         '-fno-caller-saves', '-fno-strength-reduce', '-fno-thread-jumps', '-fno-peephole',
         '-fno-delayed-branch', '-fno-function-cse', '-fno-defer-pop', '-fno-inline',
         '-fno-optimize-sibling-calls', '-fno-if-conversion', '-fomit-frame-pointer',
         '-fno-fast-math', '-O2', '-O0', '-Os', '-O3']


def build(tag, flag):
    env = dict(os.environ)
    env['TAG'] = tag
    a = [sys.executable, os.path.join(SCR, 'mn_repro.py'), unit, src]
    if flag:
        a.append(flag)
    r = subprocess.run(a, capture_output=True, text=True, env=env, cwd=ROOT)
    if r.returncode != 0:
        return None
    return r.stdout.strip()


def diffsym(o):
    out = os.path.join(SCR, 'sw_%s.json' % os.path.basename(o))
    r = subprocess.run([ROOT + '/objdiff-cli-windows-x86_64.exe', 'diff',
                        '-1', '%s/build/GOWE69/obj/Speed/Indep/SourceLists/%s.o' % (ROOT, unit),
                        '-2', o, '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false', '-o', out, '--format', 'json', sym],
                       capture_output=True, text=True)
    if r.returncode != 0:
        return None
    d = json.load(open(out))
    res = {}
    for side in ('left', 'right'):
        for s in d[side]['symbols']:
            if s.get('name') == sym:
                res[side] = s
    if 'right' not in res:
        return None
    B = res['right']
    ins = B.get('instructions', [])
    nd = sum(1 for e in ins if (e.get('diff_kind') or 'DIFF_NONE') != 'DIFF_NONE')
    return (B.get('match_percent'), B.get('size'), nd)


for i, f in enumerate(FLAGS):
    o = build('_f%d' % i, f)
    if o is None:
        print('%-32s BUILD-FAIL' % (f or '(base)'))
        continue
    r = diffsym(o)
    if r is None:
        print('%-32s NO-SYM' % (f or '(base)'))
        continue
    print('%-32s %9.5f%%  size=%-8s diffs=%d' % (f or '(base)', r[0], r[1], r[2]))
    sys.stdout.flush()
