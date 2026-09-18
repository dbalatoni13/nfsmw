#!/usr/bin/env python3
"""rodatagaps.py -- el mapa de HUECOS de dato muerto de una unidad, con su sitio.

`datadiff.py` dice QUE falta pero resincroniza a ojo y reparte mal los bloques.
Esto hace lo que la r36 dejo probado que es exacto: **empareja los literales
VIVOS por reubicacion** (el mismo emparejamiento de `lcmap.py`, instruccion a
instruccion) y, entre dos vivos consecutivos, compara lo que hay en medio. Ese
"en medio" es el dato MUERTO, que es justo lo que se escribe a mano con
`genrodata.py --anonimo` (ver docs/analisis/r36-feng.md y r36-an1.md).

Lo importante de la salida es la etiqueta de cada hueco:

  PREFIJO  lo nuestro es prefijo EXACTO de lo del objetivo: el hueco se cierra
           escribiendo el rango que se indica como bloque anonimo, sin tocar
           nada mas. Es el caso barato y el que hay que agotar primero.
  otro     hay ademas cadenas nuestras de mas, o en otro orden: eso no es dato
           ausente, es un problema de ORDEN DE PARSEO (que cabecera entra antes)
           o de simbolo, y no se arregla con un asm().

**Falso positivo conocido**: un PREFIJO de 4 B justo delante de un doble
(`43300000 80000000` y companyia) NO es dato ausente: es el relleno que el
ensamblador mete para alinear el doble a 8, y aparece solo porque lo de delante
esta corrido. Escribirlo mete 4 B de mas. Comprueba siempre que detras del hueco
no venga un valor de 8 B alineado antes de escribirlo.

Y dice ENTRE QUE DOS FUNCIONES cae el hueco. Ojo: son las funciones que USAN los
dos literales vivos, no necesariamente las que los emiten -- cc1plus emite cada
literal donde se PARSEA, asi que un literal de cabecera sale antes que el pool
de la primera funcion del .cpp que la incluye. Si un bloque no cae donde debe,
casi siempre es eso: hay que meterlo DENTRO del .cpp (detras de sus #include),
no delante de su #include en la SourceList.

    python scripts/rodatagaps.py zSim
    python scripts/rodatagaps.py zMain --detalle     # + el contenido de cada hueco
    python scripts/rodatagaps.py zSim .data
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
import lcmap


def txt(b):
    return ''.join(chr(c) if 32 <= c < 127 else '.' for c in b)


def pares_vivos(R, sec):
    """[(off_objetivo, off_nuestro, lbl, lc, (funciones,))] ordenado por el objetivo."""
    ours, theirs = R['ours'], R['theirs']
    so, st = ours.byname.get(sec), theirs.byname.get(sec)
    if so is None or st is None:
        return None, None, []
    vo = {s['name']: s['value'] for s in ours.syms if s['shndx'] == so['idx']}
    vt = {s['name']: s['value'] for s in theirs.syms if s['shndx'] == st['idx']}
    fo = ours.funcs('.text')
    orden = {n: i for i, (n, _) in enumerate(sorted(fo.items(), key=lambda kv: kv[1][0]))}
    out = []
    for (lc, add), cands in R['pairs'].items():
        if lc not in vo:
            continue
        for (lbl, oadd), uses in cands.items():
            if lbl not in vt:
                continue
            fns = tuple(sorted({u[0] for u in uses}, key=lambda n: orden.get(n, 1 << 30)))
            out.append((vt[lbl] + oadd, vo[lc] + add, lbl, lc, fns))
    return ours.sec_data(so), theirs.sec_data(st), sorted(set(out))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    detalle = '--detalle' in sys.argv
    if not args:
        sys.exit(__doc__)
    unit = args[0]
    sec = args[1] if len(args) > 1 else '.rodata'

    R = lcmap.build_map(unit, '.text')
    bo, bt, pares = pares_vivos(R, sec)
    if bo is None:
        sys.exit('%s no tiene %s en los dos objetos' % (unit, sec))
    print('%s %s: objetivo %d B, nuestro %d B, %d literales vivos emparejados'
          % (unit, sec, len(bt), len(bo), len(pares)))

    pt = po = 0
    prev = ('<inicio del fichero>',)
    n = pref = prefb = 0
    for (ot, oo, lbl, lc, fns) in pares:
        T, N = bt[pt:ot], bo[po:oo]
        if T != N:
            n += 1
            if len(N) < len(T) and T[:len(N)] == N:
                pref += 1
                prefb += ot - pt - len(N)
                print('PREFIJO hueco %-3d  genrodata %s %X %X --anonimo  (%d B)'
                      % (n, unit, pt + len(N), ot, ot - pt - len(N)))
            else:
                print('  otro  hueco %-3d  objetivo %d B / nuestro %d B  (%+d)'
                      % (n, len(T), len(N), len(T) - len(N)))
            print('           entre %s' % prev[0])
            print('               y %s   (%s)' % (fns[0], lbl))
            if detalle:
                for k in range(0, len(T), 48):
                    print('             T %s' % txt(T[k:k + 48]))
                for k in range(0, len(N), 48):
                    print('             N %s' % txt(N[k:k + 48]))
        pt, po, prev = ot, oo, fns
    print('cola: objetivo %d B desde +0x%04X, nuestra %d B desde +0x%04X'
          % (len(bt) - pt, pt, len(bo) - po, po))
    print('%d huecos, %d de ellos PREFIJO (%d B de escritura directa)' % (n, pref, prefb))


if __name__ == '__main__':
    main()
