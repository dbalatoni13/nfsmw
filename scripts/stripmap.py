#!/usr/bin/env python3
"""stripmap.py -- cuanto del deficit del DOL de cada unidad es DANO DE ESTRIPADO
y cuanto es DATO QUE FALTA de verdad.

`keep.lst` fuerza los simbolos de datos muertos con el nombre que les da `dtk` en
el objeto EXTRAIDO (`zAttribSys.o:$LC2151483872`). Nuestro objeto llama a esa
misma cadena `$LC60`, la entrada no casa con nada y `-strip-unused-data` se lleva
`size & ~7` de cada simbolo muerto: el DOL sale CORTO y el enlace no falla.
`keepchk.py` no lo ve, porque valida contra el objeto que hoy esta en el enlace.

Esto enlaza tres veces por unidad --base, con nuestro `.o`, y con nuestro `.o` mas
un `keep.lst` que nombra NUESTROS simbolos-- y separa las dos causas:

    +keep == 0     el dato ya esta entero: solo fallan los NOMBRES
    +keep == test  no hay dano de estripado: es dato ausente puro
    +keep > 0      a la unidad le SOBRA dato (pool escrito a mano por duplicado)

La solucion NO es ampliar `keep.lst`: es que el objeto traiga los nombres de
`dtk`, y eso solo lo da el dato escrito a mano (ver docs/analisis/r35-att.md).

    python scripts/stripmap.py              # todas las SourceLists
    python scripts/stripmap.py zFEng zAnim  # solo esas
"""
import hashlib, os, struct, subprocess, shutil, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LD = os.path.join('build', 'compilers', 'ProDG', '3.9.3', 'ngcld.exe')
DTK = os.path.join('build', 'tools', 'dtk.exe')
TMP = os.path.join(os.environ.get('TEMP', ROOT), 'c35att_mapa_%d' % os.getpid())
DATASEC = ('.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss')


def secs(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, sn, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = []
    for i in range(sn):
        o = shoff + i * se
        nm, typ, fl, addr, off, size, link, info, al, ent = struct.unpack('>10I', d[o:o + 40])
        S.append(dict(nm=nm, typ=typ, off=off, size=size, link=link, idx=i))
    stro = S[sx]['off']
    for s in S:
        e = d.index(b'\0', stro + s['nm'])
        s['name'] = d[stro + s['nm']:e].decode()
    return d, S


def datasyms(p):
    d, S = secs(p)
    dat = {s['idx'] for s in S if s['name'] in DATASEC}
    sym = [s for s in S if s['typ'] == 2][0]
    strt = S[sym['link']]
    out = []
    for i in range(sym['size'] // 16):
        o = sym['off'] + i * 16
        nmo, val, sz, info, other, shndx = struct.unpack('>IIIBBH', d[o:o + 16])
        if shndx in dat:
            e = d.index(b'\0', strt['off'] + nmo)
            n = d[strt['off'] + nmo:e].decode()
            if n:
                out.append(n)
    return out


def main():
    os.chdir(ROOT)
    os.makedirs(TMP, exist_ok=True)
    L = open('build.ninja', encoding='utf-8', errors='replace').read().split('\n')
    J, i = [], 0
    while i < len(L):
        s = L[i]
        while s.endswith('$') and i + 1 < len(L):
            i += 1
            s = s[:-1] + L[i].strip()
        J.append(s)
        i += 1
    for s in J:
        if s.startswith('build ') and 'main.elf' in s and ': link' in s:
            base = [x for x in s.split(': link', 1)[1].split() if x.endswith('.o')]
            break

    def link(sub, keep, tag):
        rsp = os.path.join(TMP, tag + '.rsp')
        elf = os.path.join(TMP, tag + '.elf')
        dol = os.path.join(TMP, tag + '.dol')
        for f in (elf, dol):
            if os.path.exists(f):
                os.remove(f)
        open(rsp, 'w', newline=chr(10)).write('\n'.join(sub.get(x, x) for x in base) + '\n')
        r = subprocess.run([LD, '-strip-unused-data', '-keep', keep, '-T', 'config/GOWE69/ldscript.ld',
                            '-o', elf, '@' + rsp], capture_output=True, text=True)
        if not os.path.exists(elf):
            if tag == 'b':
                err = [l for l in (r.stdout + r.stderr).splitlines() if 'error' in l.lower()]
                print('BASE FALLA: %s' % '; '.join(err[:3])[:300])
            return None
        subprocess.run([DTK, 'elf2dol', elf, dol], capture_output=True, text=True)
        if not os.path.exists(dol):
            return None
        return open(dol, 'rb').read()

    b0 = link({}, 'config/GOWE69/keep.lst', 'b')
    if b0 is None:
        sys.exit('la BASE no enlaza: otro agente ha roto el arbol')
    ok = hashlib.sha1(b0).hexdigest() == '9619ba57c9919f95f7f2ac951a2166a3517f91e3'
    print('base len %d sha %s %s' % (len(b0), hashlib.sha1(b0).hexdigest()[:12], 'OK' if ok else 'ROTO'))
    if not ok:
        print('  AVISO: la base NO es 9619ba57 -- las cifras relativas siguen valiendo')

    want = sys.argv[1:]
    sep = os.sep
    cand = []
    for x in base:
        xx = x.replace('\\', '/')
        if '/GOWE69/obj/Speed/Indep/SourceLists/' in xx:
            u = xx.rsplit('/', 1)[1][:-2]
            if not want or u in want:
                cand.append((u, x))
    print('%-14s %8s %8s %8s   %s' % ('unidad', 'test', '+keep', 'estrip', 'lectura'))
    for u, o in sorted(cand):
        n = o.replace(sep + 'obj' + sep, sep + 'src' + sep)
        if not os.path.exists(n):
            continue
        b1 = link({o: n}, 'config/GOWE69/keep.lst', 't')
        if b1 is None:
            print('%-14s ENLACE FALLA' % u)
            continue
        if hashlib.sha1(b1).hexdigest() == hashlib.sha1(b0).hexdigest():
            print('%-14s %8s %8s %8s   DOL IGUAL QUE LA BASE' % (u, '0', '-', '-'))
            continue
        k = [l.rstrip('\n') for l in open('config/GOWE69/keep.lst')]
        k = [l for l in k if not l.startswith(u + '.o:')] + [u + '.o:' + s for s in datasyms(n)]
        kp = os.path.join(TMP, 'keep.lst')
        open(kp, 'w', newline=chr(10)).write('\n'.join(k) + '\n')
        b2 = link({o: n}, kp, 'k')
        d1 = len(b1) - len(b0)
        d2 = (len(b2) - len(b0)) if b2 is not None else None
        lect = ''
        if d2 == 0:
            lect = 'SOLO nombres: el dato ya esta entero'
        elif d2 is not None and d1 != d2:
            lect = 'estripado %d B, dato %d B' % (d1 - d2, -d2)
        elif d2 is not None:
            lect = 'todo dato ausente'
        print('%-14s %8d %8s %8s   %s' % (u, d1, d2 if d2 is not None else 'falla',
                                          (d1 - d2) if d2 is not None else '-', lect))
    shutil.rmtree(TMP, ignore_errors=True)


main()
