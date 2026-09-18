#!/usr/bin/env python3
"""deadlink.py -- `deadstr.py`, pero preguntandole al ENLAZADOR quien esta muerto.

`deadstr.py` decide que un `$LC` esta vivo si **alguna reubicacion del objeto lo
nombra**, y por eso se salta justo las que hacen falta: el estripado de `ngcld`
es TRANSITIVO desde raices vivas, asi que una cadena referenciada solo desde una
funcion que el enlazador tira **tambien se va**. Medido en `zSpeech`:
`deadstr.py --keep` no propone ni una entrada nueva y el enlazador se lleva
igualmente 54 simbolos de `.rodata` (688 B), 36 de ellos cadenas que el objeto
extraido SI tiene.

Aqui la prueba de vida no se deduce: se mide. Se enlaza dos veces --con nuestro
`.o` y con nuestro `.o` mas un `keep.lst` que nombra TODOS nuestros simbolos de
`.rodata`-- y se compara la tabla de simbolos de los dos ELF. Lo que aparece en
el segundo y no en el primero es exactamente lo que el estripado se lleva.

Filtra igual que `deadstr.py`: solo cadenas ASCII terminadas en NUL que el objeto
EXTRAIDO tenga en su `.rodata`, y una sola entrada por contenido (`lcfix.py` no
sabe resolver un contenido repetido).

    python scripts/deadlink.py zSpeech            # informe
    python scripts/deadlink.py zSpeech --keep     # las lineas para keep.lst
    python scripts/deadlink.py                    # todas las SourceLists

Cuesta ~8 s por unidad (tres enlaces del proyecto entero).

AVISO: anadir estas entradas NO es gratis en todas. Medido en la r50: en `zSim`
la `.rodata` enlazada pasa de IGUAL a +56, en `zEcstasy` de -288 a +1608 y en
`zMisc` de +2840 a +3352 --son unidades con SUPERAVIT, y salvar mas dato las
aleja--; y en `zLua` resucita 196 B de `.text` porque uno de los simbolos
salvados es una vtable que mantiene vivas sus virtuales. Pasa `--medir` y mira
las tres cifras antes de escribir nada.

AVISO 2: cualquier cambio de fuente que anada o quite un literal DESPLAZA los
`$LC`, asi que despues hay que pasar `python scripts/lcfix.py`. Las dos cosas van
en el mismo commit o el DOL se rompe en silencio.
"""
import hashlib
import os
import struct
import subprocess
import sys
import types

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
_src = open('scripts/trypromo.py', encoding='utf-8').read().split('def main()')[0]
tp = types.ModuleType('tp')
tp.__dict__['__file__'] = os.path.abspath('scripts/trypromo.py')
exec(compile(_src, 'trypromo.py', 'exec'), tp.__dict__)

SEC = ('.text', '.over', '.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss')
KEEP = 'config/GOWE69/keep.lst'


def _elf(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, sn, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = []
    for i in range(sn):
        o = shoff + i * se
        nm, typ, fl, addr, off, size, link, info, al, ent = struct.unpack('>10I', d[o:o + 40])
        S.append(dict(i=i, typ=typ, addr=addr, off=off, size=size, link=link, nmo=nm))
    stro = S[sx]['off']
    for s in S:
        s['name'] = d[stro + s['nmo']:d.index(b'\0', stro + s['nmo'])].decode('latin1')
    return d, S


def _syms(d, S):
    st = [s for s in S if s['typ'] == 2]
    if not st:
        return []
    st = st[0]
    strt = S[st['link']]['off']
    out = []
    for i in range(st['size'] // 16):
        o = st['off'] + i * 16
        nmo, val, sz, info, other, shndx = struct.unpack('>IIIBBH', d[o:o + 16])
        n = d[strt + nmo:d.index(b'\0', strt + nmo)].decode('latin1')
        out.append(dict(name=n, val=val, size=sz, shndx=shndx))
    return out


def _rodata(p):
    """(bytes de .rodata, [simbolos de .rodata con tamano])"""
    d, S = _elf(p)
    r = [s for s in S if s['name'] == '.rodata']
    if not r:
        return b'', []
    blob = d[r[0]['off']:r[0]['off'] + r[0]['size']]
    return blob, [s for s in _syms(d, S) if s['shndx'] == r[0]['i'] and s['name'] and s['size']]


def enlazar(objetos, keep, salida, dol=False):
    rsp = os.path.join(tp.TMP, os.path.basename(salida) + '.rsp')
    open(rsp, 'w', newline='\n').write('\n'.join(objetos) + '\n')
    for f in (salida, salida + '.dol'):
        if os.path.exists(f):
            os.remove(f)
    subprocess.run([tp.LD, '-strip-unused-data', '-keep', keep,
                    '-T', 'config/GOWE69/ldscript.ld', '-o', salida, '@' + rsp],
                   capture_output=True, text=True)
    if not os.path.exists(salida):
        return None
    d, S = _elf(salida)
    r = dict(secciones={s['name']: s['size'] for s in S if s['name'] in SEC}, path=salida)
    n = {}
    for s in _syms(d, S):
        if s['name']:
            n[s['name']] = n.get(s['name'], 0) + 1
    r['nombres'] = n
    if dol:
        subprocess.run([tp.DTK, 'elf2dol', salida, salida + '.dol'], capture_output=True, text=True)
        r['sha'] = (hashlib.sha1(open(salida + '.dol', 'rb').read()).hexdigest()
                    if os.path.exists(salida + '.dol') else '?')
    return r


def analiza(u, base, keeplines, ya):
    """(entradas nuevas, simbolos estripados) de una unidad."""
    o, n = tp.resolver(u, base)
    if o is None or not os.path.exists(n):
        return None
    sub = [n if x == o else x for x in base]
    blob, sy = _rodata(n)
    if not sy:
        return ([], [], sub, o, n)
    T = enlazar(sub, KEEP, os.path.join(tp.TMP, 'dl_t.elf'))
    kp = os.path.join(tp.TMP, 'dl_all.lst')
    open(kp, 'w', newline='\n').write('\n'.join(
        keeplines + ['%s.o:%s' % (u, s['name']) for s in sy
                     if '%s.o:%s' % (u, s['name']) not in ya]) + '\n')
    K = enlazar(sub, kp, os.path.join(tp.TMP, 'dl_k.elf'))
    if T is None or K is None:
        return None
    estr = [s for s in sy if K['nombres'].get(s['name'], 0) > T['nombres'].get(s['name'], 0)]
    obj, _ = _rodata(o)
    nuevas, vistos = [], set()
    for s in estr:
        if not s['name'].startswith('$LC'):
            continue
        cu = blob[s['val']:s['val'] + s['size']]
        if b'\0' not in cu:
            continue
        txt = cu.split(b'\0')[0]
        if not txt or not all(32 <= c < 127 for c in txt):
            continue
        if (txt + b'\0') not in obj:
            continue                       # el objetivo no la tiene: no la salvamos
        ent = '%s.o:%s' % (u, s['name'])
        if ent in ya or txt in vistos:
            continue
        vistos.add(txt)
        nuevas.append((ent, txt.decode('latin1'), s['size'] & ~7))
    return (nuevas, estr, sub, o, n)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    solo_keep = '--keep' in sys.argv
    medir = '--medir' in sys.argv
    base = tp.objetos_del_enlace()
    keeplines = [l.rstrip('\n') for l in open(KEEP, encoding='utf-8', errors='replace')]
    ya = set(l for l in keeplines if l and not l.startswith('#'))
    uni = args or sorted({x.replace(os.sep, '/').rsplit('/', 1)[1][:-2] for x in base
                          if '/SourceLists/' in x.replace(os.sep, '/')})
    B = None
    if medir:
        B = enlazar(base, KEEP, os.path.join(tp.TMP, 'dl_b.elf'), True)
        print('base %s' % ('OK' if B['sha'] == tp.SHA_ORIG else 'ROTO ' + B['sha'][:12]))
    if not solo_keep and not medir:
        print('%-14s %6s %8s %6s %8s' % ('unidad', 'estrip', 'B', 'nuevas', 'B'))
    for u in uni:
        r = analiza(u, base, keeplines, ya)
        if r is None:
            continue
        nuevas, estr, sub, o, n = r
        if solo_keep:
            if nuevas:
                print('# %s: cadenas muertas que el enlazador se lleva y el objetivo SI tiene '
                      '(deadlink.py)' % u)
            for ent, txt, _p in nuevas:
                print('# @lc %s "%s"' % (u, txt))
                print(ent)
            continue
        if medir:
            kp = os.path.join(tp.TMP, 'dl_n.lst')
            extra = []
            for ent, txt, _p in nuevas:
                extra += ['# @lc %s "%s"' % (u, txt), ent]
            open(kp, 'w', newline='\n').write('\n'.join(keeplines + extra) + '\n')
            T = enlazar(sub, KEEP, os.path.join(tp.TMP, 'dl_t2.elf'))
            N = enlazar(sub, kp, os.path.join(tp.TMP, 'dl_n.elf'), True) if extra else T
            f = lambda X: (' '.join('%s%+d' % (k[1:], X['secciones'].get(k, 0) - B['secciones'].get(k, 0))
                                    for k in SEC
                                    if X['secciones'].get(k, 0) - B['secciones'].get(k, 0)) or 'IGUAL')
            print('%-14s hoy %-30s  +%d entradas -> %-30s' % (u, f(T), len(nuevas), f(N)))
        else:
            print('%-14s %6d %8d %6d %8d' % (u, len(estr), sum(s['size'] & ~7 for s in estr),
                                             len(nuevas), sum(p for _, _, p in nuevas)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
