#!/usr/bin/env python3
"""lcmap.py -- empareja el pool `$LCn` que emite cc1plus con el `lbl_XXXXXXXX`
del objeto EXTRAIDO, comparando las REUBICACIONES instruccion a instruccion.

Por que hace falta
------------------
Seis SourceLists escriben su pool de constantes a mano en un `asm()` y ese pool
casa byte a byte con el del objetivo. Lo que sobra es el pool `$LC` que cc1plus
emite ADEMAS, porque nuestro codigo escribe la constante donde el original
REFERENCIABA la etiqueta. La cura es declarar `extern const float lbl_XXXXXXXX;`
y usarlo, pero para eso hace falta saber QUE etiqueta va en CADA sitio.

**El emparejamiento NO es posicional.** En zDynamics cuadra en las primeras 24 y
se desalinea porque nuestro pool mete un `4503601774854144.0` (la magia de
conversion int->double) que el original no tiene. Un mapa posicional es un error
SILENCIOSO: el codigo sigue siendo correcto, objdiff sigue dando 100% (ignora los
nombres de reubicacion) y **el DOL se rompe**.

Como lo saca
------------
El `.text` casa al 100%, asi que dentro de cada funcion los indices de
instruccion se alinean. Se recorren las dos listas de reubicaciones de cada
funcion; donde el objetivo referencia `lbl_X` y nosotros `$LCn` en LA MISMA
instruccion, ese es el par. Se cotejan ademas los BYTES del dato apuntado en los
dos lados (`.rodata` nuestra contra la extraida): si no coinciden, el par se
marca `!!` y no hay que fiarse.

Uso
---
    python scripts/lcmap.py zDynamics              # el mapa
    python scripts/lcmap.py zDynamics --insn       # + cada referencia con su
                                                   #   funcion e indice de insn
    python scripts/lcmap.py zDynamics --conflict   # solo lo dudoso
    python scripts/lcmap.py zDynamics --json f.js  # para consumirlo desde otro guion

Salida: una fila por `$LCn` con el `lbl_` que le toca, cuantas veces aparece, el
valor y en que funciones. Los `$LCn` SIN pareja son pool que el objetivo no tiene
como simbolo aparte (o esta dentro de otra etiqueta con desplazamiento).
"""
import json
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# --- reubicaciones PPC que nos interesan -------------------------------------
R_PPC_ADDR32 = 1
R_PPC_ADDR16_LO = 4
R_PPC_ADDR16_HI = 5
R_PPC_ADDR16_HA = 6
R_PPC_SDAREL16 = 32
RNAMES = {1: 'ADDR32', 4: 'LO', 5: 'HI', 6: 'HA', 10: 'REL24', 32: 'SDAREL16'}


class Elf(object):
    def __init__(self, path):
        self.path = path
        d = open(path, 'rb').read()
        self.d = d
        assert d[:4] == b'\x7fELF', path
        E = '>' if d[5] == 2 else '<'
        self.E = E
        shoff, = struct.unpack(E + 'I', d[0x20:0x24])
        shentsize, shnum, shstrndx = struct.unpack(E + 'HHH', d[0x2E:0x34])
        secs = []
        for i in range(shnum):
            o = shoff + i * shentsize
            (name, typ, flags, addr, off, size, link, info,
             align, entsize) = struct.unpack(E + '10I', d[o:o + 40])
            secs.append(dict(idx=i, nameoff=name, typ=typ, flags=flags, addr=addr,
                             off=off, size=size, link=link, info=info,
                             align=align, entsize=entsize))
        shstr = secs[shstrndx]

        def cstr(base, off):
            e = d.index(b'\0', base + off)
            return d[base + off:e].decode('utf-8', 'replace')

        for s in secs:
            s['name'] = cstr(shstr['off'], s['nameoff'])
        self.secs = secs
        self.byname = {}
        for s in secs:
            self.byname.setdefault(s['name'], s)

        # tabla de simbolos
        self.syms = []
        for s in secs:
            if s['typ'] != 2:            # SHT_SYMTAB
                continue
            strt = secs[s['link']]
            for i in range(s['size'] // 16):
                o = s['off'] + i * 16
                nameoff, value, size, info, other, shndx = struct.unpack(
                    E + 'IIIBBH', d[o:o + 16])
                self.syms.append(dict(
                    idx=i, name=cstr(strt['off'], nameoff), value=value,
                    size=size, bind=info >> 4, type=info & 0xF, shndx=shndx))
            break

    def sec_data(self, s):
        if s['typ'] == 8:                # SHT_NOBITS
            return b'\0' * s['size']
        return self.d[s['off']:s['off'] + s['size']]

    def relocs(self, secname):
        """[(offset, symidx, type, addend)] de la seccion `secname`."""
        tgt = self.byname.get(secname)
        if tgt is None:
            return []
        out = []
        for s in self.secs:
            if s['typ'] not in (4, 9):   # RELA / REL
                continue
            if s['info'] != tgt['idx']:
                continue
            rela = s['typ'] == 4
            step = 12 if rela else 8
            for i in range(s['size'] // step):
                o = s['off'] + i * step
                if rela:
                    off, info, add = struct.unpack(self.E + 'IIi', self.d[o:o + 12])
                else:
                    off, info = struct.unpack(self.E + 'II', self.d[o:o + 8])
                    add = 0
                out.append((off, info >> 8, info & 0xFF, add))
        out.sort()
        return out

    def funcs(self, secname='.text'):
        """{nombre: (inicio, tamano)} de los simbolos de funcion de `secname`."""
        tgt = self.byname.get(secname)
        if tgt is None:
            return {}
        out = {}
        for sy in self.syms:
            if sy['shndx'] != tgt['idx'] or not sy['name']:
                continue
            if sy['type'] not in (2, 0):   # FUNC / NOTYPE
                continue
            if sy['size'] == 0:
                continue
            out[sy['name']] = (sy['value'], sy['size'])
        return out


def sym_bytes(elf, symname, addend, n=8):
    """Los `n` bytes del dato al que apunta `symname+addend`, o None."""
    for sy in elf.syms:
        if sy['name'] != symname:
            continue
        if sy['shndx'] in (0, 0xFFF1, 0xFFF2):
            return None
        s = elf.secs[sy['shndx']]
        d = elf.sec_data(s)
        o = sy['value'] + addend
        if 0 <= o < len(d):
            return d[o:o + n]
        return None
    return None


def fmt_val(b):
    if b is None:
        return '?'
    f, = struct.unpack('>f', b[:4])
    if len(b) >= 8:
        dd, = struct.unpack('>d', b[:8])
        return '%-14.7g /d %-14.7g  %s' % (f, dd, b[:8].hex())
    return '%-14.7g  %s' % (f, b[:4].hex())


def resolve(unit):
    cands = [unit, 'Speed/Indep/SourceLists/' + unit]
    for c in cands:
        p = c.replace('/', os.sep) + '.o'
        ours = os.path.join(ROOT, 'build', 'GOWE69', 'src', p)
        theirs = os.path.join(ROOT, 'build', 'GOWE69', 'obj', p)
        if os.path.exists(ours) and os.path.exists(theirs):
            return ours, theirs
    sys.exit('no encuentro los dos objetos de %r (compila con build_direct.py)' % unit)


def build_map(unit, section='.text'):
    po, pt = resolve(unit)
    ours, theirs = Elf(po), Elf(pt)
    fo, ft = ours.funcs(section), theirs.funcs(section)
    ro = ours.relocs(section)
    rt = theirs.relocs(section)

    def by_func(elf, funcs, rl):
        """{func: [(insn_idx, tipo, simbolo, addend)]}"""
        items = sorted(funcs.items(), key=lambda kv: kv[1][0])
        out = {}
        for name, (start, size) in items:
            lst = []
            for off, si, ty, add in rl:
                if start <= off < start + size:
                    lst.append(((off - start) // 4, ty, elf.syms[si]['name'], add,
                                elf.syms[si]['shndx']))
            out[name] = lst
        return out

    mo, mt = by_func(ours, fo, ro), by_func(theirs, ft, rt)

    pairs = {}     # (lc, add_ours) -> {(lbl, add_theirs): [(func, insn, tipo)]}
    unpaired = {}  # (lc, add) -> [(func, insn, tipo, lo_que_tiene_el_objetivo)]
    missing = []   # funciones sin pareja
    for name in sorted(set(mo) & set(mt)):
        a, b = mo[name], mt[name]
        bi = {}
        for insn, ty, sym, add, shndx in b:
            bi[(insn, ty)] = (sym, add)
        for insn, ty, sym, add, shndx in a:
            if not sym.startswith('$LC'):
                continue
            other = bi.get((insn, ty))
            if other is None:
                unpaired.setdefault((sym, add), []).append((name, insn, ty, None))
                continue
            osym, oadd = other
            pairs.setdefault((sym, add), {}).setdefault((osym, oadd), []).append(
                (name, insn, ty))
    for name in sorted(set(mo) - set(mt)):
        if any(s.startswith('$LC') for _, _, s, _, _ in mo[name]):
            missing.append(name)
    return dict(ours=ours, theirs=theirs, pairs=pairs, unpaired=unpaired,
                missing=missing, mo=mo, mt=mt, po=po, pt=pt)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    flags = [a for a in sys.argv[1:] if a.startswith('-')]
    if not args:
        sys.exit(__doc__)
    unit = args[0]
    section = args[1] if len(args) > 1 else '.text'
    want_insn = '--insn' in flags
    only_conf = '--conflict' in flags
    jout = None
    if '--json' in sys.argv:
        jout = sys.argv[sys.argv.index('--json') + 1]

    R = build_map(unit, section)
    ours, theirs, pairs = R['ours'], R['theirs'], R['pairs']

    rows = []
    for (lc, add), cands in sorted(pairs.items(),
                                   key=lambda kv: (kv[0][0].lstrip('$LC').zfill(9),
                                                   kv[0][1])):
        tot = sum(len(v) for v in cands.values())
        conflict = len(cands) > 1
        for (lbl, oadd), uses in sorted(cands.items(), key=lambda kv: -len(kv[1])):
            ob = sym_bytes(ours, lc, add)
            tb = sym_bytes(theirs, lbl, oadd)
            ok = (ob is not None and tb is not None and ob[:4] == tb[:4])
            rows.append(dict(lc=lc, lc_add=add, lbl=lbl, lbl_add=oadd,
                             n=len(uses), tot=tot, conflict=conflict,
                             bytes_ok=ok, val=fmt_val(ob), tval=fmt_val(tb),
                             uses=[(u[0], u[1], RNAMES.get(u[2], str(u[2])))
                                   for u in uses]))

    if jout:
        json.dump(rows, open(jout, 'w'), indent=1)
        print('escrito %s (%d filas)' % (jout, len(rows)))

    print('%-10s %-16s %-16s %5s %4s  %s' %
          ('', 'nuestro', 'objetivo', 'usos', '', 'valor'))
    nconf = nbad = 0
    for r in rows:
        mark = ''
        if r['conflict']:
            mark += ' CONFLICTO'
            nconf += 1
        if not r['bytes_ok']:
            mark += ' !!BYTES'
            nbad += 1
        if only_conf and not mark:
            continue
        lcs = r['lc'] + ('+%d' % r['lc_add'] if r['lc_add'] else '')
        lbs = r['lbl'] + ('+%d' % r['lbl_add'] if r['lbl_add'] else '')
        print('%-10s %-16s %-16s %5d       %s%s' %
              ('', lcs, lbs, r['n'], r['val'], mark))
        if want_insn:
            for f, i, t in r['uses']:
                print('%-10s   %s  insn %d  %s' % ('', f, i, t))

    if R['unpaired']:
        print('\nSIN PAREJA (el objetivo no referencia nada ahi):')
        for (lc, add), uses in sorted(R['unpaired'].items()):
            print('   %s%s   %d usos   %s' %
                  (lc, '+%d' % add if add else '', len(uses),
                   ', '.join('%s:%d' % (u[0], u[1]) for u in uses[:4])))
    if R['missing']:
        print('\nFUNCIONES CON $LC QUE NO EXISTEN EN EL EXTRAIDO: %s' %
              ', '.join(R['missing'][:10]))

    lcs = set(r['lc'] for r in rows)
    print('\n%d simbolos $LC emparejados, %d filas, %d con CONFLICTO, %d con bytes distintos'
          % (len(lcs), len(rows), nconf, nbad))


if __name__ == '__main__':
    main()
