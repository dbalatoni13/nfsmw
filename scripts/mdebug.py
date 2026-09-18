#!/usr/bin/env python3
"""mdebug.py -- lee el .mdebug (ECOFF/MIPS) del build de PS2 (SLES-53558-A124).

El ELF de PS2 trae 84,5 MB de debug info ECOFF con los stabs de MWCC/GCC:
para cada funcion del juego hay simbolos con los PARAMETROS (`this:P29203`),
los BLOQUES anidados ($LBB/$LBE), las etiquetas de linea ($LMnnnn) y las
rutas de los ficheros en el orden de expansion inline. El codigo MIPS no se
copia, pero esa estructura ES el fuente original compartido con GameCube.

Los offsets de las tablas salen de la cabecera HDRR, que sigue el orden
estandar de IRIX. En este fichero:

    PDR   0x55f804  31957 x 52 B
    SYMR  0x6f5348  1896029 x 12 B
    ss    0x1ce6f08, 0x3797bd6 B (issMax es el TAMANO; las cadenas externas
          empiezan justo detras, en 0x547eade)
    lineas 0x55f3ac (bytes delta, ver decodifica_lineas)

Los `iss` de los simbolos son absolutos: el issBase de los 559 FDR es 0.

    python scripts/mdebug.py fn <nombre>       # estructura de una funcion
    python scripts/mdebug.py fn <nombre> -L    # + tabla de lineas
    python scripts/mdebug.py files [patron]    # rutas de fuente unicas
"""
import os
import struct
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
ELF = os.path.join(ROOT, "orig", "SLES-53558-A124", "NFS.ELF")

def _hdrr():
    """Offsets de las tablas segun la cabecera HDRR de la seccion .mdebug."""
    with open(ELF, 'rb') as f:
        e = f.read(0x34)
        shoff = struct.unpack_from('<I', e, 0x20)[0]
        shentsize, shnum, shstrndx = struct.unpack_from('<HHH', e, 0x2E)
        f.seek(shoff)
        sh = f.read(shentsize * shnum)
        secs = [struct.unpack_from('<IIIIIIIIII', sh, i * shentsize) for i in range(shnum)]
        f.seek(secs[shstrndx][4])
        nombres = f.read(secs[shstrndx][5])
        for s in secs:
            if nombres[s[0]:nombres.index(b'\0', s[0])] == b'.mdebug':
                f.seek(s[4])
                h = struct.unpack('<HH23I', f.read(0x60))
                return dict(cbLineOffset=h[4], ipdMax=h[7], cbPdOffset=h[8], isymMax=h[9],
                            cbSymOffset=h[10], issMax=h[15], cbSsOffset=h[16])
    raise SystemExit('%s no tiene .mdebug' % ELF)


_H = _hdrr()
PDR_OFF, PDR_N = _H['cbPdOffset'], _H['ipdMax']
SYM_OFF, SYM_N = _H['cbSymOffset'], _H['isymMax']
SS_OFF, SS_SIZE = _H['cbSsOffset'], _H['issMax']
LINE_OFF = _H['cbLineOffset']

ST = {0: 'nil', 1: 'global', 2: 'static', 3: 'param', 4: 'local', 5: 'label',
      6: 'proc', 7: 'block', 8: 'end', 11: 'file', 14: 'staticproc'}

_cache = {}


def carga():
    if _cache:
        return
    f = open(ELF, 'rb')
    f.seek(PDR_OFF)
    praw = f.read(PDR_N * 52)
    pdrs = []
    for i in range(PDR_N):
        o = i * 52
        adr, isym, iline, regmask, regoffset, iopt, fregmask, fregoffset, \
            frameoffset, framereg, pcreg, irline, lnLow, lnHigh, cbLineOffset = \
            struct.unpack('<IiiiiiiiihhIhh I'.replace(' ', ''), praw[o:o + 52])
        pdrs.append(dict(adr=adr, isym=isym, irline=irline,
                         lnLow=lnLow - 65536 if lnLow > 32768 else lnLow,
                         lnHigh=lnHigh, cbLineOffset=cbLineOffset,
                         frameoffset=frameoffset))
    _cache['pdrs'] = pdrs
    del praw

    f.seek(SYM_OFF)
    sraw = f.read(SYM_N * 12)
    syms = []
    for i in range(SYM_N):
        o = i * 12
        iss, value, bits = struct.unpack('<III', sraw[o:o + 12])
        syms.append((iss, value, bits & 0x3f, (bits >> 6) & 0x1f))
    _cache['syms'] = syms
    del sraw

    f.seek(SS_OFF)
    ss = f.read(SS_SIZE)
    _cache['ss'] = ss
    f.close()

    # indice: nombre de funcion -> (symidx, ...)
    por_nombre = {}
    for i, (iss, value, st, sc) in enumerate(syms):
        if st == 6:
            nom = _cadena(iss)
            if nom:
                por_nombre.setdefault(nom, i)
    _cache['por_nombre'] = por_nombre


def _cadena(iss):
    ss = _cache['ss']
    if iss >= len(ss):
        return ''
    e = ss.find(b'\0', iss)
    if e < 0:
        e = len(ss)
    return ss[iss:e].decode('latin-1')


def decodifica_lineas(pdr):
    """Bytes delta de la tabla de lineas. Formato: cada byte avanza una
    instruccion (4 B); valores 0x80+... calibrado empiricamente."""
    f = open(ELF, 'rb')
    f.seek(LINE_OFF + pdr['cbLineOffset'])
    raw = f.read(8192)
    f.close()
    out = []
    line = pdr['lnLow']
    addr = pdr['adr']
    for b in raw:
        if b == 0:
            break
        if b < 0x80:
            line += b
        else:
            line -= (0x100 - b)
        out.append((addr, line))
        addr += 4
        if line > pdr['lnHigh'] + 1 or len(out) > 2000:
            break
    return out


def cmd_fn(nm, con_lineas=False):
    carga()
    pn = _cache['por_nombre']
    claves = [k for k in pn if nm.lower() in k.lower()]
    if not claves:
        print('no hay funciones que casen %r' % nm)
        return
    syms = _cache['syms']
    for k in claves[:3]:
        i0 = pn[k]
        iss, value, st, sc = syms[i0]
        print('===== %s  @0x%x' % (k, value))
        # el PDR asociado
        pdr = None
        for p in _cache['pdrs']:
            if p['isym'] == i0:
                pdr = p
                break
        if pdr:
            print('  lineas %d..%d (irline %d)  frame %d' % (
                pdr['lnLow'], pdr['lnHigh'], pdr['irline'], pdr['frameoffset']))
        # volcar los simbolos de la funcion hasta el stEnd que la cierra
        prof = 0
        fichero = ''
        for j in range(i0, min(i0 + 4000, SYM_N)):
            jiss, jval, jst, jsc = syms[j]
            nom = _cadena(jiss)
            if jst == 8 and jval == j - i0 - 1 and prof == 0:
                break
            if jst == 6:
                continue
            if jst == 5:  # labels: lineas y ficheros
                if nom.startswith('$LM'):
                    print('    %-8s 0x%08x' % (nom, jval))
                elif nom.endswith(('.cpp', '.h', '.hpp', '.c', '.inl')) or '/src/' in nom:
                    fichero = nom
                    print('    FILE     %s' % nom)
                continue
            if jst in (0, 2, 3, 4):
                if nom.startswith('$LB'):
                    print('    %-14s 0x%x' % (nom, jval))
                elif ':' in nom and len(nom) < 120:
                    print('    %-14s %-8s val=%-6d %s' % (
                        ST.get(jst, jst), '', jval, nom))
        if con_lineas and pdr:
            print('  --- lineas (PDR) ---')
            for a, l in decodifica_lineas(pdr)[:80]:
                print('    0x%08x  linea %d' % (a, l))


def cmd_files(pat):
    carga()
    vistos = set()
    for iss, value, st, sc in _cache['syms']:
        if st == 5:
            nom = _cadena(iss)
            if '/src/' in nom or nom.endswith(('.cpp', '.h', '.hpp')):
                if nom not in vistos:
                    vistos.add(nom)
    for v in sorted(vistos):
        if not pat or pat.lower() in v.lower():
            print(v)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(0)
    cmd = sys.argv[1]
    if cmd == 'fn':
        cmd_fn(sys.argv[2] if len(sys.argv) > 2 else '', '-L' in sys.argv)
    elif cmd == 'files':
        cmd_files(sys.argv[2] if len(sys.argv) > 2 else None)
    else:
        print(__doc__)
