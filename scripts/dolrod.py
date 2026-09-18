#!/usr/bin/env python3
"""dolrod.py -- diff de una seccion ENLAZADA en la ventana de una unidad.

Lo que ve el juez, no lo que hay en el objeto: `-strip-unused-data` se lleva los
simbolos muertos pero CONSERVA el pool de la funcion descartada, asi que el
objeto y el enlace NO dicen lo mismo. La ventana sale de `splits.txt`.

    python dolrod.py zBWare [.rodata|.data|.text]
"""
import difflib, os, struct, subprocess, sys, shutil, atexit

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LD = os.path.join('build', 'compilers', 'ProDG', '3.9.3', 'ngcld.exe')
DTK = os.path.join('build', 'tools', 'dtk.exe')
LDFLAGS = '-strip-unused-data -keep config/GOWE69/keep.lst'
ORIG = os.path.join('orig', 'GOWE69', 'sys', 'main.dol')
TMP = os.path.join(os.environ.get('TEMP', ROOT), 'dolrod_%d' % os.getpid())
os.makedirs(TMP, exist_ok=True)
atexit.register(lambda: shutil.rmtree(TMP, ignore_errors=True))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dolwhere import objetos_del_enlace, dolsecs, symbols, quien


def rango(unidad, sec):
    cur = None
    for L in open(os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt'), encoding='utf-8'):
        if L and not L[0].isspace() and L.rstrip().endswith(':'):
            cur = L.rstrip()[:-1]
        elif cur and cur.endswith('/' + unidad + '.cpp') and L.strip().startswith(sec + ' '):
            t = L.split()
            return int(t[1].split(':')[1], 16), int(t[2].split(':')[1], 16)
    sys.exit('no encuentro %s de %s en splits.txt' % (sec, unidad))


def main():
    os.chdir(ROOT)
    u = sys.argv[1]
    sec = sys.argv[2] if len(sys.argv) > 2 else '.rodata'
    ini, fin = rango(u, sec)
    rel = 'Speed/Indep/SourceLists/' + u
    oobj = os.path.join('build', 'GOWE69', 'obj', *rel.split('/')) + '.o'
    base = objetos_del_enlace()
    sub = {oobj: oobj.replace(os.path.join('GOWE69', 'obj'), os.path.join('GOWE69', 'src'))}
    rsp, elf, dol = (os.path.join(TMP, x) for x in ('t.rsp', 't.elf', 't.dol'))
    open(rsp, 'w').write('\n'.join(sub.get(x, x) for x in base) + '\n')
    subprocess.run([LD] + LDFLAGS.split() + ['-T', 'config/GOWE69/ldscript.ld', '-o', elf, '@' + rsp],
                   capture_output=True, text=True)
    subprocess.run([DTK, 'elf2dol', elf, dol], capture_output=True, text=True)
    A, B = open(ORIG, 'rb').read(), open(dol, 'rb').read()
    S = symbols(elf)
    for (oa, aa, sa), (ob, ab, sb) in zip(dolsecs(A), dolsecs(B)):
        if not (aa <= ini < aa + sa):
            continue
        print('%s %s  obj %08X..%08X (%d B)   seccion: obj %d nue %d (%+d)'
              % (u, sec, ini, fin, fin - ini, sa, sb, sb - sa))
        lo = ini - aa - 64
        wa = A[oa + lo: oa + (fin - aa) + 1024]
        wb = B[ob + lo: ob + (fin - aa) + 1024]
        for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, wa, wb, autojunk=False).get_opcodes():
            if tag == 'equal':
                if i2 - i1 >= 24:
                    print('   ==%6d B  hasta obj %08X' % (i2 - i1, aa + lo + i2))
                continue
            if tag in ('delete', 'replace'):
                print('   FALTA obj %08X %4d  %-46s %s' % (aa+lo+i1, i2-i1, wa[i1:i1+23].hex(), quien(S, ab+lo+j1)))
            if tag in ('insert', 'replace'):
                print('   SOBRA nue %08X %4d  %-46s %s' % (ab+lo+j1, j2-j1, wb[j1:j1+23].hex(), quien(S, ab+lo+j1)))
        return


if __name__ == '__main__':
    main()
