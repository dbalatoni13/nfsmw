#!/usr/bin/env python3
"""La permutacion exacta de una unidad: que funciones emite en otro orden.

`dolwhere` dice que las diferencias de `zSim` son entradas de vtable que apuntan
a direcciones distintas, con deltas de signo y magnitud distintos --o sea
permutacion, no deriva--. Esto saca la permutacion.

Compara el ORDEN de los simbolos de `.text` de los dos objetos, quedandose con
los que existen en los dos (los que solo emitimos nosotros los estripa el
enlazador y no cuentan). Sobre esa subsecuencia comun calcula:

  - la subsecuencia creciente mas larga: lo que YA esta en su sitio;
  - los ciclos de la permutacion, que es lo que dice si el arreglo es mover una
    declaracion o barajar la unidad entera.

    python permzsim.py [unidad]        # por defecto zSim
"""
import os
import sys

sys.path.insert(0, 'scripts')
from extrasym import Elf

ROOT = os.getcwd()


def funcs_por_offset(p):
    e = Elf(p)
    ci = e.code_idx()
    out = [(s['value'], s['name']) for s in e.syms()
           if s['type'] == 2 and s['shndx'] in ci and s['name'] and s['size']]
    out.sort()
    return out


def definidas(p):
    """TODOS los nombres de funcion del objeto, con tamano o SIN el.

    `funcs_por_offset` exige `st_size` a proposito --un alias no anade una
    posicion nueva al orden de emision y empataria con su funcion real--, pero
    esa misma exigencia miente al preguntar SI DEFINIMOS algo: un alias escrito
    con `asm()` no lleva directiva `.size`, asi que sale con `st_size == 0` y
    parecia no existir. Costo 17 falsos MISSING en zFe2.
    """
    e = Elf(p)
    ci = e.code_idx()
    # ni el tipo: hay dos formas de alias y solo una emite `.type @function`.
    # `_IHandle__15IGenericMessage` sale STT_NOTYPE y tambien esta definido.
    return {s['name'] for s in e.syms()
            if s['type'] in (0, 2) and s['shndx'] in ci and s['name']}


def lis(seq):
    """Indices de la subsecuencia creciente mas larga."""
    import bisect
    colas, padre, idx = [], [None] * len(seq), []
    for i, v in enumerate(seq):
        j = bisect.bisect_left([seq[k] for k in idx], v)
        if j == len(idx):
            idx.append(i)
        else:
            idx[j] = i
        padre[i] = idx[j - 1] if j else None
    out, k = [], idx[-1] if idx else None
    while k is not None:
        out.append(k)
        k = padre[k]
    return out[::-1]


def main():
    u = sys.argv[1] if len(sys.argv) > 1 else 'Speed/Indep/SourceLists/zSim'
    a = os.path.join(ROOT, 'build', 'GOWE69', 'obj', *u.split('/')) + '.o'
    b = os.path.join(ROOT, 'build', 'GOWE69', 'src', *u.split('/')) + '.o'
    A = funcs_por_offset(a)
    B = funcs_por_offset(b)
    nA = {n: i for i, (_, n) in enumerate(A)}
    nB = {n: i for i, (_, n) in enumerate(B)}
    comunes = [n for _, n in A if n in nB]
    print('%s: objetivo %d funciones, nuestro %d, comunes %d'
          % (u.split('/')[-1], len(A), len(B), len(comunes)))
    print('solo nuestras (las estripa el enlazador): %d' % (len(B) - len(comunes)))
    # para «lo definimos o no» vale cualquier simbolo, tenga tamano o no
    defB = definidas(b)
    solo_obj = [n for _, n in A if n not in defB]
    alias = [n for _, n in A if n not in nB and n in defB]
    if alias:
        print('sin tamano en nuestro objeto (alias de asm(), SI definidas): %d' % len(alias))
    if solo_obj:
        print('SOLO DEL OBJETIVO (%d): %s' % (len(solo_obj), ', '.join(x[:40] for x in solo_obj[:5])))

    # posicion de cada comun en NUESTRO objeto, en el orden del objetivo
    seq = [nB[n] for n in comunes]
    fijos = set(lis(seq))
    movidas = [i for i in range(len(seq)) if i not in fijos]
    print()
    print('en su sitio (subsecuencia creciente mas larga): %d de %d' % (len(fijos), len(seq)))
    print('DESPLAZADAS: %d' % len(movidas))

    # los ciclos, sobre el ranking relativo
    orden_obj = {n: i for i, n in enumerate(comunes)}
    por_nuestro = sorted(comunes, key=lambda n: nB[n])
    perm = [orden_obj[n] for n in por_nuestro]      # perm[i] = donde estaba en el objetivo
    vistos = [False] * len(perm)
    ciclos = []
    for i in range(len(perm)):
        if vistos[i] or perm[i] == i:
            vistos[i] = True
            continue
        c, j = [], i
        while not vistos[j]:
            vistos[j] = True
            c.append(j)
            j = perm[j]
        if len(c) > 1:
            ciclos.append(c)
    ciclos.sort(key=len, reverse=True)
    print('ciclos no triviales: %d  (longitudes: %s)'
          % (len(ciclos), ', '.join(str(len(c)) for c in ciclos[:12])))
    print()
    for c in ciclos[:6]:
        print('  ciclo de %d:' % len(c))
        for j in c[:8]:
            print('     %-64s obj#%-4d nue#%-4d' % (por_nuestro[j][:64], perm[j], j))
        if len(c) > 8:
            print('     ... y %d mas' % (len(c) - 8))


main()
