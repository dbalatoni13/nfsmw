#!/usr/bin/env python3
"""desplaza.py -- el desplazamiento REAL que una unidad le impone al resto del enlace.

`linkdelta.py` compara el TAMANO de cada seccion del ELF y con eso decidimos si
una unidad esta `IGUAL`. **Esa medida redondea**: el enlazador alinea el arranque
de `.data`, `.bss`, `.sdata` y `.sdata2` a 32, asi que una unidad puede aportar
hasta 31 bytes de menos --o de mas-- y las nueve secciones seguir dando delta 0.
Medido en la r58 sobre zEcstasy: con `linkdelta` diciendo **IGUAL** en las nueve,
los simbolos de zFe estaban 12 bytes por debajo de su direccion en `.data` y 16
en `.bss`, y eso son ~100.000 bytes de DOL distintos.

Lo que de verdad hay que mirar es **la direccion de los simbolos que van DETRAS**
de la unidad. Esto enlaza dos veces --la base y la base con tu `.o`-- y empareja
los simbolos por nombre:

  * `DETRAS`  = simbolos de OTRAS unidades que cambian de direccion. Tiene que
                ser CERO. Si no lo es, la cifra es exactamente los bytes que le
                sobran (o le faltan) a tu unidad, y no la ves en `linkdelta`.
  * `DENTRO`  = simbolos de la unidad en otra direccion. Es el problema de ORDEN,
                y su detalle sale con `--detalle`.

    python scripts/desplaza.py zEcstasy
    python scripts/desplaza.py zEcstasy --detalle      # simbolo a simbolo
"""
import collections
import os
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
from trypromo import objetos_del_enlace, resolver, LD, TMP  # noqa: E402

LDFLAGS = '-strip-unused-data -keep config/GOWE69/keep.lst'
SECS = ('.rodata', '.data', '.bss', '.sdata', '.sbss', '.sdata2', '.text')


def enlazar(sub, salida):
    base = objetos_del_enlace()
    rsp = salida + '.rsp'
    open(rsp, 'w').write('\n'.join(sub.get(x, x) for x in base) + '\n')
    r = subprocess.run([LD] + LDFLAGS.split() + ['-T', 'config/GOWE69/ldscript.ld',
                                                 '-o', salida, '@' + rsp],
                       capture_output=True, text=True)
    if not os.path.exists(salida):
        sys.exit('ENLACE FALLA: ' + (r.stdout + r.stderr)[-400:])
    return salida


def leer(path):
    f = open(path, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff+i*se:shoff+i*se+40]) for i in range(sn)]
    stro = S[sx][4]
    nm = {i: f[stro+s[0]:f.index(b'\0', stro+s[0])].decode(errors='replace')
          for i, s in enumerate(S)}
    out = {}
    for s in S:
        if s[1] != 2:
            continue
        strt = S[s[6]][4]
        for k in range(s[5] // 16):
            o = s[4] + k * 16
            n_, val, sz, i2, o2, sh = struct.unpack('>IIIBBH', f[o:o+16])
            n = f[strt+n_:f.index(b'\0', strt+n_)].decode(errors='replace')
            if n and sh and sh < 0xff00:
                out.setdefault(n, []).append((val, nm.get(sh, '?')))
    return out


def rangos(unidad):
    """(seccion -> (ini, fin)) de `splits.txt` para la unidad."""
    p = os.path.join('config', 'GOWE69', 'splits.txt')
    cur, out = None, {}
    for line in open(p, encoding='utf-8', errors='replace'):
        if line and not line[0].isspace() and line.rstrip().endswith(':'):
            cur = line.strip()[:-1]
        elif cur and line.strip().startswith('.'):
            t = line.split()
            if len(t) >= 3 and unidad in cur:
                out[t[0]] = (int(t[1].split(':')[1], 16), int(t[2].split(':')[1], 16))
    return out


def main():
    a = [x for x in sys.argv[1:] if not x.startswith('-')]
    if not a:
        sys.exit(__doc__)
    det = '--detalle' in sys.argv
    base = objetos_del_enlace()
    sub = {}
    for u in a:
        o, n = resolver(u, base)
        if o is None or not os.path.exists(n):
            sys.exit('  %s: sin objeto (compila la unidad primero)' % u)
        sub[o] = n
    B = leer(enlazar({}, os.path.join(TMP, 'b.elf')))
    O = leer(enlazar(sub, os.path.join(TMP, 'o.elf')))
    R = {}
    for u in a:
        R.update(rangos(u.split('/')[-1]))

    tot = collections.Counter()
    det_rows = collections.defaultdict(list)
    for n, vb in B.items():
        if n not in O or len(vb) != 1 or len(O[n]) != 1:
            continue
        (ab, s1), (ao, s2) = vb[0], O[n][0]
        if s1 != s2 or s1 not in SECS:
            continue
        ini, fin = R.get(s1, (0, 0))
        dentro = ini <= ab < fin
        tot[(s1, 'DENTRO' if dentro else 'DETRAS', ab == ao)] += 1
        if ab != ao:
            det_rows[(s1, 'DENTRO' if dentro else 'DETRAS')].append((ab, ao - ab, n))

    print('  %-9s %8s %8s | %8s %8s' % ('seccion', 'DENTRO', 'mal', 'DETRAS', 'mal'))
    malo = 0
    for s in SECS:
        d_ok, d_mal = tot[(s, 'DENTRO', True)], tot[(s, 'DENTRO', False)]
        f_ok, f_mal = tot[(s, 'DETRAS', True)], tot[(s, 'DETRAS', False)]
        if d_ok + d_mal + f_mal == 0:
            continue
        malo += f_mal
        print('  %-9s %8d %8d | %8d %8d %s'
              % (s, d_ok + d_mal, d_mal, f_ok + f_mal, f_mal, '  <== TAMANO MAL' if f_mal else ''))
    for k, rows in sorted(det_rows.items()):
        if not rows:
            continue
        rows.sort()
        if k[1] == 'DETRAS':
            d = collections.Counter(x[1] for x in rows)
            print('  %s DETRAS: %d simbolos movidos, desplazamientos %s'
                  % (k[0], len(rows), d.most_common(4)))
            print('     el primero: %-40s %08X -> %08X (%+d)'
                  % (rows[0][2], rows[0][0], rows[0][0] + rows[0][1], rows[0][1]))
        elif det:
            print('  --- %s DENTRO (%d mal) ---' % (k[0], len(rows)))
            prev = None
            for ab, d, n in rows:
                if d != prev:
                    print('     %08X  %+6d   %s' % (ab, d, n))
                    prev = d
    print('  VEREDICTO: %s' % ('el TAMANO de la unidad esta MAL (%d simbolos ajenos movidos)'
                               % malo if malo else
                               'el tamano es EXACTO; lo que queda es ORDEN'))


if __name__ == '__main__':
    main()
