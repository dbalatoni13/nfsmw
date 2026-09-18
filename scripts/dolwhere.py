#!/usr/bin/env python3
"""dolwhere.py -- enlaza con una unidad promocionada y dice DONDE difiere el DOL.

`trypromo.py` da un sha1 y un veredicto; esto da la lista de rangos de direccion
que no casan, con el simbolo al que caen segun la tabla del ELF enlazado.

    python dolwhere.py zBWare
"""
import os, struct, subprocess, sys, shutil, atexit

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LD = os.path.join('build', 'compilers', 'ProDG', '3.9.3', 'ngcld.exe')
DTK = os.path.join('build', 'tools', 'dtk.exe')
LDFLAGS = '-strip-unused-data -keep config/GOWE69/keep.lst'
ORIG = os.path.join('orig', 'GOWE69', 'sys', 'main.dol')
TMP = os.path.join(os.environ.get('TEMP', ROOT), 'dolwhere_%d' % os.getpid())
os.makedirs(TMP, exist_ok=True)
atexit.register(lambda: shutil.rmtree(TMP, ignore_errors=True))


def objetos_del_enlace():
    L = open(os.path.join(ROOT, 'build.ninja'), encoding='utf-8', errors='replace').read().split('\n')
    J, i = [], 0
    while i < len(L):
        s = L[i]
        while s.endswith('$') and i + 1 < len(L):
            i += 1
            s = s[:-1] + L[i].strip()
        J.append(s); i += 1
    for s in J:
        if s.startswith('build ') and 'main.elf' in s and ': link' in s:
            return [x for x in s.split(': link', 1)[1].split() if x.endswith('.o')]
    sys.exit('no encuentro el edge de main.elf')


def dolsecs(d):
    off = struct.unpack('>18I', d[0x00:0x48])
    adr = struct.unpack('>18I', d[0x48:0x90])
    siz = struct.unpack('>18I', d[0x90:0xD8])
    return [(off[i], adr[i], siz[i]) for i in range(18) if siz[i]]


def symbols(elf):
    d = open(elf, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, sn, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    out = []
    for s in S:
        if s[1] != 2:                       # SHT_SYMTAB
            continue
        stro = S[s[6]][4]
        for o in range(s[4], s[4] + s[5], 16):
            nm, val, sz, info = struct.unpack('>IIIB', d[o:o + 13])
            if not sz or (info & 0xF) not in (1, 2):
                continue
            e = d.index(b'\0', stro + nm)
            out.append((val, val + sz, d[stro + nm:e].decode('latin1')))
    out.sort()
    return out


def quien(S, a):
    lo, hi = 0, len(S) - 1
    best = '?'
    while lo <= hi:
        m = (lo + hi) // 2
        if S[m][0] <= a:
            if a < S[m][1]:
                return '%s +0x%X' % (S[m][2], a - S[m][0])
            best = S[m][2] + ' (fuera)'
            lo = m + 1
        else:
            hi = m - 1
    return best


def main():
    os.chdir(ROOT)
    units = sys.argv[1:]
    base = objetos_del_enlace()
    sub = {}
    for u in units:
        for c in (u, 'Speed/Indep/SourceLists/' + u):
            o = os.path.join('build', 'GOWE69', 'obj', *c.split('/')) + '.o'
            if o in base:
                sub[o] = o.replace(os.path.join('GOWE69', 'obj'), os.path.join('GOWE69', 'src'))
                break
        else:
            n = os.path.join('build', 'GOWE69', 'src', 'Speed', 'Indep',
                             'SourceLists', u + '.o')
            if n in base:
                continue          # ya promocionada: el enlace ya usa el nuestro
            sys.exit('%s no esta en el enlace' % u)
    rsp, elf, dol = (os.path.join(TMP, x) for x in ('t.rsp', 't.elf', 't.dol'))
    open(rsp, 'w').write('\n'.join(sub.get(x, x) for x in base) + '\n')
    r = subprocess.run([LD] + LDFLAGS.split() + ['-T', 'config/GOWE69/ldscript.ld', '-o', elf, '@' + rsp],
                       capture_output=True, text=True)
    if not os.path.exists(elf):
        sys.exit('ENLACE FALLA: ' + (r.stdout + r.stderr)[-400:])
    subprocess.run([DTK, 'elf2dol', elf, dol], capture_output=True, text=True)
    A = open(ORIG, 'rb').read()
    B = open(dol, 'rb').read()
    if A == B:
        print('DOL OK'); return
    SA, SB = dolsecs(A), dolsecs(B)
    S = symbols(elf)
    if [x[1:] for x in SA] != [x[1:] for x in SB]:
        print('LAS SECCIONES NO COINCIDEN:')
        for x, y in zip(SA, SB):
            if x[1:] != y[1:]:
                print('  obj a=%08X s=%X   nue a=%08X s=%X' % (x[1], x[2], y[1], y[2]))
        return
    tot = 0
    for off, adr, siz in SA:
        a, b = A[off:off + siz], B[off:off + siz]
        i = 0
        while i < siz:
            if a[i] == b[i]:
                i += 1; continue
            j = i
            while j < siz and (a[j] != b[j] or any(a[k] != b[k] for k in range(j, min(j + 12, siz)))):
                j += 1
            tot += j - i
            print('  %08X  %4d B  %-52s' % (adr + i, j - i, quien(S, adr + i)))
            print('           obj %s' % a[i:min(i + 24, j)].hex())
            print('           nue %s' % b[i:min(i + 24, j)].hex())
            i = j
    print('total %d B distintos' % tot)


if __name__ == "__main__":
    main()
