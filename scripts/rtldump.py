#!/usr/bin/env python3
"""rtldump.py -- volcados RTL (.lreg/.greg/.combine/...) de una unidad o de un .cpp suelto.

  python scripts/rtldump.py <unidad> <fnfilter> [-dg -dl ...]
  python scripts/rtldump.py --file <ruta.cpp> --like <unidad> <fnfilter> [flags]
  ... --extra "-DFOO -fno-schedule-insns2"      (repetible)

`--extra` anade opciones a las de la unidad. Van al preprocesador enteras, y a
cc1plus solo las que este filtra por KEEP --las de codegen--, igual que hace con
los cflags de la unidad. Sirve para probar una guarda (`-DZMAIN_...`) o un paso
del compilador sin tocar configure.py.

El fuente de GCC 2.95.3 esta en orig/prodg/NGC_GNU_SRC/NGC/gcc/: el `.greg`
trae el orden de asignacion YA ordenado por allocno_compare (global.c), la
matriz de conflictos y el reg_renumber final, y el `.combine` dice por que
combine no pliega algo. Un near-miss de reparto deja de ser adivinanza.

Un `.cpp` suelto que reproduzca la funcion byte a byte tarda 0,4 s contra los
22 s de una SourceList entera: usa --file para eso.
"""
import os, re, sys, subprocess as SP
# La ruta NO va a fuego: ocho scripts del arbol la llevan y desde un worktree
# diffean los .o del repo principal sin avisar. Ver el aviso de
# nfsmw-build-y-diff-workflow.
ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')).replace(os.sep, '/')
sys.path.insert(0, ROOT + '/scripts')
import build_direct as BD
TOOLS = os.path.join(ROOT, 'build', 'compilers')
TMP = os.path.join(ROOT, 'scratchpad', 'rtl')
KEEP = re.compile(r'^-(O|f|m|G|g|W|std=|ansi|pedantic)')

def main():
    a = sys.argv[1:]
    extra = []
    while '--extra' in a:
        i = a.index('--extra')
        extra += a[i + 1].split()
        a = a[:i] + a[i + 2:]
    srcfile = None; like = None
    if a[0] == '--file':
        srcfile = a[1]; a = a[2:]
        assert a[0] == '--like'
        like = a[1]; a = a[2:]
    else:
        like = a[0]; a = a[1:]
    fnfilter = a[0] if a else ''
    dumps = [x for x in a[1:] if x.startswith('-d')]
    if not dumps: dumps = ['-dg', '-dl']
    os.chdir(ROOT)
    units = BD.parse_units()
    key = [k for k in units if k.endswith(like) or k == like]
    if not key: sys.exit('unidad no encontrada: ' + like + ' (' + ', '.join(list(units)[:5]) + ')')
    spec = units[key[0]]
    src_rel, out_rel, cflags, toolchain = spec[0], spec[1], spec[2], spec[3]
    if srcfile: src_rel = srcfile
    tag = os.path.basename(src_rel).replace('.', '_')
    base = os.path.join(TMP, tag)
    os.makedirs(TMP, exist_ok=True)
    tools = os.path.join(TOOLS, toolchain)
    env = dict(os.environ); env['SN_NGC_PATH'] = tools
    ii = base + '.i'
    p = SP.run([os.path.join(tools, 'ngccc.exe')] + cflags + extra + ['-E', '-o', ii, src_rel],
               cwd=ROOT, env=env, capture_output=True, text=True)
    if p.returncode != 0: sys.exit('preproc fail ' + p.stderr[-2000:])
    keep = [f for f in cflags if KEEP.match(f)] + [f for f in extra if KEEP.match(f)]
    # r67 (libc): el front end por extension, como hace ngccc; antes forzaba cc1plus
    # tambien para los .c y los volcados de libc salian de otro compilador.
    cc1 = 'cc1.exe' if src_rel.lower().endswith('.c') else 'cc1plus.exe'
    p = SP.run([os.path.join(tools, cc1)] + keep + dumps + ['-quiet', '-o', base + '.s', ii],
               cwd=TMP, env=env, capture_output=True, text=True)
    if p.returncode != 0:
        sys.stderr.write(p.stderr[-3000:] + '\n')
    print('cflags:', ' '.join(keep))
    if extra: print('extra :', ' '.join(extra))
    print('base:', base)
    for f in sorted(os.listdir(TMP)):
        if f.startswith(tag + '_i.') or f.startswith(os.path.basename(ii) + '.'):
            print('  dump:', os.path.join(TMP, f), os.path.getsize(os.path.join(TMP, f)))
    print('FN FILTER:', fnfilter)

if __name__ == '__main__':
    main()
