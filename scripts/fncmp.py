#!/usr/bin/env python3
"""fncmp.py -- que funciones de una unidad NO son identicas, con o sin SourceList.

`tamfn.py` dice cuales tienen otro TAMANO y eso engana: `matched_code` es todo o
nada, asi que una funcion del tamano exacto con un registro distinto aporta cero
bytes igual. En FSasync el censo por tamano daba 1 funcion pendiente y son 2; en
filesys daba 1 y son 3.

    python scripts/fncmp.py LibSN/steering
    python scripts/fncmp.py FSasync CompletePCread     # detalle de una funcion

Abre los dos `.o` y ya esta, sin `objdiff.json` ni rutas de SourceLists, asi que
ve las unidades de middleware que `triaje.py` no ve (en `LibSN/steering` aquel da
0 funciones y esto da 11).

CUATRO normalizaciones, cada una descubierta por un falso positivo:

  1. Una reubicacion de 16 bits (`@ha`, `@l`, `@sda21`) apunta al CAMPO, no al
     principio de la instruccion: sale en 0x12 para la instruccion de 0x10.
     Indexando por el offset crudo se pierden todas y cada `lis`/`lwz` de una
     global sale como diferencia. Valia por si sola 25 funciones de filesys.
  2. Los saltos internos van reubicados contra el simbolo de SECCION en nuestro
     objeto y resueltos en el extraido. Hay que comparar el destino.
  3. Una llamada a otra funcion de la misma unidad aparece reubicada en un lado
     y resuelta en el otro; se resuelve el destino a (simbolo, desplazamiento).
  4. El objeto extraido nombra los simbolos `nombre_DIRECCION`.

Y lo que queda sin resolver se SEPARA en vez de sumarse: las funciones con las
mismas instrucciones pero reubicaciones a simbolos con otro nombre --el alias de
estatico local, `nombre$233_804FFD9C` contra `nombre$155`, que objdiff normaliza
por su cuenta-- se listan aparte. Inflaban la cifra un 30 %.

VALIDADO contra el informe oficial en las 30 unidades con codigo pendiente:
coincide AL BYTE en 28. Las dos que no son el INFORME yendo por detras --zCamera
y zGameplay, las dos tocadas despues de generarlo--, no la criba.

Aun asi, confirma con `fndiff <unidad> <simbolo>` antes de dar una funcion por
rota: la criba es para ELEGIR, no para juzgar.
"""
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# campos que una reubicacion PPC puede reescribir, por tipo
ENTERA = {1: 0xFFFFFFFF, 4: 0x0000FFFF, 5: 0x0000FFFF, 6: 0x0000FFFF,
          10: 0x03FFFFFC, 26: 0x0000FFFF, 27: 0x0000FFFF, 14: 0x0000FFFF,
          15: 0x0000FFFF, 16: 0x0000FFFF, 17: 0x0000FFFF}


# El troceador nombra por DIRECCION lo que no supo identificar (`fn_80310E00`,
# `lbl_804128B0`) y nuestra fuente le da su nombre de verdad (`PCread`). La
# equivalencia esta en `config/GOWE69/symbols.txt`, que es la lista oficial: sin
# cargarla, cada llamada a una de esas funciones sale como destino distinto.
DIR = re.compile(r'^(?:fn|lbl)_(8[0-9A-F]{7})$')


def _por_direccion():
    m = {}
    q = os.path.join(ROOT, 'config', 'GOWE69', 'symbols.txt')
    if not os.path.exists(q):
        return m
    pat = re.compile(r'^\s*(\S+)\s*=\s*\.\w+:0x([0-9A-Fa-f]{8})')
    for l in open(q, encoding='utf-8', errors='replace'):
        g = pat.match(l)
        if g and not DIR.match(g.group(1)):
            m.setdefault(g.group(2).upper(), g.group(1))
    return m


PORDIR = _por_direccion()


SUFIJO = re.compile(r'_(?:8[0-9A-F]{7})$')
# y el contador de clase local: `..._7NumCarsRC14FECareerRecord.35688`. El
# numero sale de un contador de declaraciones de la unidad y no coincide
# entre los dos objetos (es lo mismo que arregla `scripts/mangfix.py`).
CONTADOR = re.compile(r'[.][0-9]{3,}$')


def limpio(nmb):
    """El objeto extraido nombra los simbolos `nombre_DIRECCION`; el nuestro no.

    Sin quitar ese sufijo, cualquier funcion que solo toque una global sale
    «distinta» por el nombre. Me marco 15 de las 18 de LibSN/vm que `fndiff` da
    al 100 %: era el mismo enmascaramiento que ya documenta el proyecto para
    objdiff.
    """
    d = DIR.match(nmb or '')
    if d and d.group(1).upper() in PORDIR:
        return PORDIR[d.group(1).upper()]
    q = CONTADOR.sub('', SUFIJO.sub('', nmb or ''))
    # ...pero `fn_8030F7C8` ES un nombre de direccion entero: quitarle el sufijo
    # deja `fn` y colapsa TODAS las funciones anonimas de una unidad en una. Se
    # me quedaron en el mismo saco `fn_8030F79C` y `fn_8030F7C8`, y una llamada
    # de una a la otra pasaba por salto interno.
    return nmb if len(q) < 3 or q in ('fn', 'lbl', 'gap') else q


REL_SALTO = (10, 11)          # R_PPC_REL24, R_PPC_REL14


def desplazamiento(ins):
    """Desplazamiento con signo de un salto relativo, o None si no lo es."""
    op = ins >> 26
    if op == 18:                                  # b / bl
        if ins & 2:                               # AA=1: absoluto
            return None
        d = ins & 0x03FFFFFC
        return d - 0x04000000 if d & 0x02000000 else d
    if op == 16:                                  # bc
        if ins & 2:
            return None
        d = ins & 0x0000FFFC
        return d - 0x10000 if d & 0x8000 else d
    return None


def destino(ins, off, rel, mapa, va, mio=None):
    """A donde va un salto, en forma comparable: (simbolo, desplazamiento).

    Hay que resolverlo porque los dos objetos expresan lo mismo de formas
    distintas: nuestro compilador deja el salto interno como reubicacion
    relativa al simbolo de seccion, el objeto extraido lo trae resuelto, y una
    llamada a otra funcion de la MISMA unidad aparece reubicada en un lado y ya
    resuelta en el otro. Comparar los bits da falsos positivos en los tres
    casos: me marco 34 funciones de filesys que `fndiff` da al 100 %.
    """
    if desplazamiento(ins) is None:
        return None
    if rel is not None:
        if rel[1] not in REL_SALTO:
            return None
        if rel[0]:
            # el objeto extraido expresa un salto DENTRO de la funcion como su
            # propio simbolo mas addend, y el nuestro como simbolo de seccion.
            # Sin igualarlos, cualquier bucle salia distinto: 3 funciones de
            # metrotrk que `fndiff` da al 100 %.
            if mio is not None and limpio(rel[0]) == mio:
                return ('.', rel[2])
            return (limpio(rel[0]), rel[2])
        d = rel[2]                 # reubicacion contra el simbolo de SECCION
    else:
        d = off + desplazamiento(ins)
    nmb = mapa.get(d)
    return (limpio(nmb), 0) if nmb else ('.', d - va)


def lee(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, n, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(n)]
    stro = S[sx][4]
    nom = lambda o: d[stro + o:d.index(b'\0', stro + o)].decode()
    # zFeOverlay y zOnline son los DOS unicos objetos extraidos cuyo codigo va
    # en `.over` y no en `.text`. Sin esto `fncmp` abortaba y `triaje` contestaba
    # «0 funciones», o sea que llevaban rondas invisibles al triaje que reparte
    # el trabajo. Y hay que generalizar TAMBIEN la reubicacion de abajo: con
    # solo esta linea salen 404 falsos positivos de 467, porque la
    # normalizacion de los saltos internos va contra el simbolo de seccion.
    SECCODE = ('.text', '.over')
    ix = [i for i, s in enumerate(S) if nom(s[0]) in SECCODE]
    if not ix:
        return None
    ix = ix[0]
    txt = d[S[ix][4]:S[ix][4] + S[ix][5]]
    st = [s for s in S if s[1] == 2][0]
    strt = S[st[6]][4]
    sim = []
    for q in range(st[4], st[4] + st[5], 16):
        nm, val, sz, info, other, shn = struct.unpack('>IIIBBH', d[q:q + 16])
        sim.append((d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1'), val, sz, shn, info & 0xF))
    fn = {n_: (v, z) for n_, v, z, sh, t in sim if sh == ix and t == 2 and z}
    # Y los simbolos de .text SIN `.size`: los `asm()` que el proyecto usa
    # para los alias de clase local ponen la etiqueta pero no el `.size`, asi
    # que el cuerpo esta y el simbolo mide 0. Sin recogerlos, salen AUSENTES
    # 17 destructores de zFe2 que si emitimos (884 B).
    sinsize = {n_: v for n_, v, z, sh, t in sim if sh == ix and not z and n_}
    # El mapa de destinos lleva TODOS los simbolos de .text, no solo las
    # funciones con tamano: un ayudante local puede ser un simbolo propio en
    # el objeto extraido y una etiqueta sin tamano en el nuestro, y entonces
    # el mismo `bl` sale como destino distinto. Paso con `idctcolumn64`, que
    # daba por rota una funcion de 1.740 B que esta al 100 %.
    eti = {v: n_ for n_, v, z, sh, t in sim if sh == ix and n_}
    rel = {}
    for s in S:
        if s[1] == 4 and nom(s[0]) in ('.rela' + c for c in SECCODE):
            for q in range(s[4], s[4] + s[5], 12):
                off, info, add = struct.unpack('>IIi', d[q:q + 12])
                # OJO: una reubicacion de 16 bits (`@ha`, `@l`, `@sda21`) apunta
                # al CAMPO, no al principio de la instruccion: sale en 0x12 para
                # la instruccion de 0x10. Indexando por el offset crudo se
                # pierden todas y cada `lis`/`lwz` de una global sale como
                # diferencia. Me costo dar por distintas 25 funciones de filesys
                # que estan al 100 %.
                rel[off & ~3] = (sim[info >> 8][0], info & 0xFF, add)
    return txt, fn, rel, eti, sinsize


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    rel_u = sys.argv[1].replace('\\', '/')
    todas = '--todas' in sys.argv
    detalle = [a for a in sys.argv[2:] if not a.startswith('--')]
    def par(cual):
        # algunas unidades cuelgan de la RAIZ del objeto aunque la fuente este
        # en un subdirectorio (LibSN/inittmr.c -> obj/inittmr.o)
        q = os.path.join(ROOT, 'build', 'GOWE69', cual, *rel_u.split('/')) + '.o'
        if not os.path.exists(q):
            q = os.path.join(ROOT, 'build', 'GOWE69', cual,
                             rel_u.rsplit('/', 1)[-1]) + '.o'
        return q
    pa, pb = par('obj'), par('src')
    for q in (pa, pb):
        if not os.path.exists(q):
            sys.exit('no existe %s' % q)
    a, b = lee(pa), lee(pb)
    if not a or not b:
        sys.exit('%s: alguno de los dos objetos no tiene .text ni .over' % rel_u)
    ta, fa, ra, ma, _sa = a
    tb, fb, rb, mb, sb = b
    filas = []
    alias = []
    # el emparejamiento tambien va por nombre LIMPIO: el objeto extraido llama
    # `__static_initialization_and_destruction_0_8039A0F4` a lo que el nuestro
    # llama `__static_initialization_and_destruction_0`, y comparando en crudo
    # sale AUSENTE una funcion de 2.456 B que si esta.
    lb = {}
    for k, v in fb.items():
        lb.setdefault(limpio(k), v)
    for nmb, (va, za) in fa.items():
        if nmb not in fb and limpio(nmb) not in lb:
            if nmb in sb:                 # etiqueta sin `.size`: el cuerpo si esta
                vb, zb = sb[nmb], za
            else:
                filas.append((za, nmb, za, 0, 'AUSENTE'))
                continue
        else:
            vb, zb = fb[nmb] if nmb in fb else lb[limpio(nmb)]
        ver = bool(detalle) and any(x in nmb for x in detalle)
        if za != zb:
            filas.append((za, nmb, za, zb, 'tamano'))
            # ...pero si han PEDIDO el detalle de esta funcion, se enseña igual
            # hasta donde llega la mas corta. Las de +-4 B --una sola
            # instruccion, el caso mas barato del censo-- se quedaban mudas.
            if not ver:
                continue
            print('   tamano objetivo %d, nuestro %d: se comparan las primeras'
                  ' %d instrucciones' % (za, zb, min(za, zb) // 4))
        malas = 0
        dest = 0
        for k in range(0, min(za, zb), 4):
            ia, = struct.unpack('>I', ta[va + k:va + k + 4])
            ib, = struct.unpack('>I', tb[vb + k:vb + k + 4])
            pa, pb = ra.get(va + k), rb.get(vb + k)
            if desplazamiento(ia) is not None or desplazamiento(ib) is not None:
                mio = limpio(nmb)
                da = destino(ia, va + k, pa, ma, va, mio)
                db = destino(ib, vb + k, pb, mb, vb, mio)
                # y se normalizan los DOS al final: un salto a la propia
                # funcion sale como `('.', n)` en un lado y con su nombre en el
                # otro segun quien resolviera la reubicacion.
                if da and da[0] == mio:
                    da = ('.', da[1])
                if db and db[0] == mio:
                    db = ('.', db[1])
                # el campo BO/BI solo existe en `bc` (opcode 16); en `b` esos
                # bits SON el desplazamiento, y compararlos daba distinto
                # cualquier salto hacia atras contra uno hacia delante.
                cond = 0x03FF0000 if (ia >> 26) == 16 else 0
                if ((ia >> 26) != (ib >> 26) or (ia & 3) != (ib & 3)
                        or (ia & cond) != (ib & cond)):
                    malas += 1                    # otro tipo de salto o condicion
                    if ver:
                        print('   +%04X SALTO obj %08X | nue %08X' % (k, ia, ib))
                elif da != db:
                    malas += 1
                    if ver:
                        print('   +%04X DESTINO obj %s | nue %s' % (k, da, db))
                continue
            if (pa is None) != (pb is None):
                # una referencia que un lado trae RESUELTA y el otro con
                # reubicacion de 16 bits: `lis r4,0x8004` contra
                # `lis r4,sym@ha`. Misma instruccion y mismo registro, otra
                # forma de escribir el operando. Sale mucho en las SourceLists
                # --daba por rota `OnCollision`, 5.748 B al 100 %-- y objdiff
                # no lo cuenta, asi que va al saco de nombres.
                q = pa or pb
                m = ~ENTERA.get(q[1], 0) & 0xFFFFFFFF
                if m != 0xFFFFFFFF and (ia & m) == (ib & m):
                    dest += 1
                    continue
                malas += 1
                if ver:
                    print('   +%04X REUB obj %08X %s | nue %08X %s'
                          % (k, ia, pa, ib, pb))
                continue
            if pa is not None:
                if limpio(pa[0]) != limpio(pb[0]) or pa[1] != pb[1] or pa[2] != pb[2]:
                    dest += 1
                m = ~ENTERA.get(pa[1], 0xFFFFFFFF) & 0xFFFFFFFF
                ia, ib = ia & m, ib & m
            if ia != ib:
                malas += 1
                if ver:
                    print('   +%04X obj %08X %s | nue %08X %s' % (k, ia, pa, ib, pb))
        if malas:
            filas.append((za, nmb, za, zb, '%d insn%s' % (malas, ', %d reub' % dest if dest else '')))
        elif dest:
            alias.append((za, nmb, dest))
    filas.sort(reverse=True)
    alias.sort(reverse=True)
    print('%8s  %-46s %s' % ('B', 'funcion', 'diferencia'))
    for B, nmb, za, zb, q in filas:
        print('%8d  %-46s %s%s' % (B, nmb[:46], q, '' if za == zb else ' (%d/%d)' % (zb, za)))
    print('')
    print('%d de %d funciones con el CODIGO distinto -- %d B que no cuentan'
          % (len(filas), len(fa), sum(f[0] for f in filas)))
    if alias:
        # Mismas instrucciones y reubicaciones a simbolos con OTRO nombre. Casi
        # siempre es el alias del objeto extraido (`nombre$233_804FFD9C` contra
        # `nombre$155`), que objdiff normaliza por su cuenta y aqui no se puede
        # resolver sin adivinar. Se listan aparte para no inflar la cifra.
        print('%d mas solo con nombres de simbolo distintos (%d B) -- casi siempre alias:'
              % (len(alias), sum(a[0] for a in alias)))
        for B, nmb, dnum in alias[:8]:
            print('   %8d  %-46s %d reub' % (B, nmb[:46], dnum))


main()
