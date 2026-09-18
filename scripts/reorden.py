#!/usr/bin/env python3
"""reorden.py -- contrafactual: si emitieramos en el ORDEN del objetivo, casaria?

La medida que faltaba para el bloque A del frente (unidades con el codigo al
100 %, `linkdelta` IGUAL, `fncmp` 0 y el DOL roto igual). Ninguna de las que
teniamos distingue "me falta contenido" de "lo tengo todo, mal colocado":

  * `fncmp` compara el OBJETO funcion a funcion y no ve la posicion;
  * `linkdelta` compara TAMANOS de seccion;
  * `dolwhere` cuenta bytes del DOL en la direccion en que caen, asi que un
    bloque desplazado sale como "contenido distinto" de punta a punta;
  * `permorden` da el numero de funciones fuera de sitio, pero no dice si el
    CONTENIDO de esas funciones es el correcto.

Esto enlaza dos veces (con el objeto extraido y con el nuestro), toma la
SECUENCIA DE SIMBOLOS DEL OBJETIVO y vuelve a colocar NUESTROS bytes en ese
orden; luego compara palabra a palabra **enmascarando el campo de reubicacion**
(la mitad baja de una forma D, el desplazamiento de una rama), porque ese campo
lo reescribe el enlazador en cuanto el simbolo cae donde debe.

    python scripts/reorden.py zMain                  # todas las ventanas de splits.txt
    python scripts/reorden.py zMain .text            # solo una seccion
    python scripts/reorden.py zMain 801F844C 801FA8F4  # una ventana concreta
    python scripts/reorden.py zMain --o <ruta.o>     # con un .o privado

Lectura del resultado:

    distintas ignorando reubicacion: 0   -> NO QUEDA CONTENIDO QUE ESCRIBIR.
                                            Todo lo que separa la unidad del
                                            DOL es ORDEN DE EMISION.
    distintas ignorando reubicacion: N   -> quedan N palabras de contenido real;
                                            eso si es trabajo de fuente.

DOS AVISOS, los dos medidos:

  * **El `.rodata` empareja por NOMBRE y el objetivo no tiene nombres.** Sus
    literales son `lbl_xxxxxxxx` (los inventa el troceado) y los nuestros
    `$LCnnn`: la columna "simbolos que no tenemos" cuenta esos, no un fallo.
    Las vtables y los simbolos con nombre si se comparan.
  * **`$LCnnn` COLISIONA entre objetos.** Dos unidades tienen su `$LC62`; si
    alguna vez se empareja por nombre sin filtrar por ventana, sale un delta de
    cien kilobytes que no existe (es lo que ensucia el `.rodata` de
    `movidos.py`).
"""
import atexit
import os
import shutil
import struct
import sys
import types

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, 'scripts')
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

import linkdelta as ld                                    # noqa: E402
from movidos import simbolos                              # noqa: E402

# Formas D: el operando de 16 bits es la mitad baja de una reubicacion.
DFORM = set([14, 15, 24, 25, 26, 27, 28, 29, 32, 33, 34, 35, 36, 37, 38, 39,
             40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
             56, 58, 60, 62])


def mascara(w):
    """Palabra con el campo que reescribe el enlazador puesto a cero."""
    v, = struct.unpack('>I', w)
    op = v >> 26
    if op in DFORM:
        return ('d', v >> 16)
    if op == 18:                       # b / bl / ba / bla
        return ('b', v & 3)
    if op == 16:                       # bc
        return ('c', v & 0xFFFF0003)
    return ('r', v)


def secciones(elf):
    """-> (bytes del fichero, [(dir_ini, dir_fin, offset)]) de las secciones con datos."""
    d = open(elf, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    shent, shnum, _ = struct.unpack('>HHH', d[0x2E:0x34])
    out = []
    for i in range(shnum):
        h = struct.unpack('>10I', d[shoff + i * shent:shoff + i * shent + 40])
        if h[3] and h[5] and h[1] == 1:                    # SHT_PROGBITS con direccion
            out.append((h[3], h[3] + h[5], h[4]))
    return d, out


def leer(d, secs, ad, n):
    for lo, hi, off in secs:
        if lo <= ad < hi:
            return d[off + (ad - lo):off + (ad - lo) + n]
    return None


def ventanas(unidad):
    """-> {seccion: (ini, fin)} de splits.txt."""
    cur, out = None, {}
    for L in open(os.path.join('config', 'GOWE69', 'splits.txt'), encoding='utf-8'):
        if L and not L[0].isspace() and L.rstrip().endswith(':'):
            cur = L.rstrip()[:-1]
        elif cur and cur.endswith('/' + unidad + '.cpp') and L[:1].isspace():
            t = L.split()
            if t and t[0].startswith('.') and len(t) >= 3 and t[1].startswith('start:'):
                out.setdefault(t[0], (int(t[1].split(':')[1], 16),
                                      int(t[2].split(':')[1], 16)))
    return out


def compara(da, sa, A, db, sb, Bs, lo, hi):
    lista = sorted((v[0], k, v[1]) for k, v in A.items() if lo <= v[0] < hi and v[1])
    tot = crudas = reales = falta = distinto = 0
    peores = []
    for ad, nm, sz in lista:
        if nm not in Bs:
            falta += 1
            continue
        if Bs[nm][1] != sz:
            distinto += 1
            peores.append(('TAMANO', nm, sz, Bs[nm][1]))
            continue
        x = leer(da, sa, ad, sz)
        y = leer(db, sb, Bs[nm][0], Bs[nm][1])
        if x is None or y is None:
            continue
        n = 0
        for i in range(0, sz - 3, 4):
            tot += 1
            if x[i:i + 4] != y[i:i + 4]:
                crudas += 1
                if mascara(x[i:i + 4]) != mascara(y[i:i + 4]):
                    reales += 1
                    n += 1
        if n:
            peores.append(('CONTENIDO', nm, n, sz))
    return len(lista), tot, crudas, reales, falta, distinto, peores


def main():
    argv = list(sys.argv[1:])
    alt = None
    if '--o' in argv:
        i = argv.index('--o')
        alt = argv[i + 1]
        del argv[i:i + 2]
    args = [a for a in argv if not a.startswith('--')]
    if not args:
        sys.exit(__doc__)
    unidad = args[0]
    base = ld.tp.objetos_del_enlace()
    ruta = unidad if '/' in unidad else 'Speed/Indep/SourceLists/' + unidad
    o, n = ld.tp.resolver(ruta, base)
    if o is None:
        print('%s no esta en la lista de enlace (ya promocionada?)' % unidad)
        return 1
    n = alt or n
    if not os.path.exists(n):
        print('%s: falta el .o (%s)' % (unidad, n))
        return 1
    pa = os.path.join(ld.tp.TMP, 'ro_a.elf')
    pb = os.path.join(ld.tp.TMP, 'ro_b.elf')
    if ld.enlazar(base, pa) is None or ld.enlazar(
            [n if x == o else x for x in base], pb) is None:
        print('el enlace falla')
        return 1
    da, sa = secciones(pa)
    db, sb = secciones(pb)
    A, Bs = simbolos(pa), simbolos(pb)

    if len(args) >= 3 and all(c in '0123456789abcdefABCDEF' for c in args[1]):
        rangos = [('(a mano)', int(args[1], 16), int(args[2], 16))]
    else:
        v = ventanas(os.path.basename(ruta))
        pedida = args[1] if len(args) > 1 else None
        rangos = [(s, lo, hi) for s, (lo, hi) in sorted(v.items())
                  if s not in ('.bss', '.sbss') and (pedida is None or s == pedida)]
    if not rangos:
        print('sin ventanas que comparar')
        return 1

    print('%s: NUESTROS bytes recolocados en el ORDEN DEL OBJETIVO' % unidad)
    print()
    print('%-9s %7s %8s %8s %10s %8s' % ('seccion', 'simbs', 'palabras',
                                         'crudas', 'CONTENIDO', 'sin par'))
    total_real = 0
    detalle = []
    for sec, lo, hi in rangos:
        ns, tot, crudas, reales, falta, distinto, peores = compara(
            da, sa, A, db, sb, Bs, lo, hi)
        print('%-9s %7d %8d %8d %10d %8d%s'
              % (sec, ns, tot, crudas, reales, falta,
                 '   (%d de otro TAMANO)' % distinto if distinto else ''))
        total_real += reales
        detalle += [(sec,) + p for p in peores]
    print()
    if total_real == 0:
        print('CONTENIDO 0: no queda nada que escribir. Lo que separa a %s del'
              % unidad)
        print('DOL es ORDEN DE EMISION, y las diferencias crudas son campos de')
        print('reubicacion que el enlazador reescribe solo al colocar el simbolo.')
    else:
        print('quedan %d palabras de CONTENIDO real:' % total_real)
        for d in detalle[:40]:
            if d[1] == 'TAMANO':
                print('   %-8s TAMANO   %-52s obj %d  nue %d' % (d[0], d[2][:52], d[3], d[4]))
            else:
                print('   %-8s %4d pal de %4d B   %s' % (d[0], d[3], d[4], d[2][:52]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
