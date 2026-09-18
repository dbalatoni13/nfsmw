#!/usr/bin/env python3
"""textorder.py -- el ORDEN de las funciones de `.text`, el frente que objdiff NO VE.

`objdiff` empareja las funciones POR NOMBRE, asi que una unidad puede dar 100 %
con el `.text` del tamano exacto, cero funciones ausentes y cero funciones por
debajo del 100 %... y tener las funciones EN OTRO ORDEN. Entonces el DOL sale
del mismo tamano pero distinto, y `trypromo.py` dice `DOL ROTO` sin decir por
que. Esta herramienta lo dice.

Compara la SECUENCIA de simbolos de `.text` por direccion entre el objeto
EXTRAIDO del original (`build/GOWE69/obj/<ruta>.o`, que trae el orden bueno) y
el NUESTRO (`build/GOWE69/src/<ruta>.o`). No enlaza nada, asi que es inmune a
los otros agentes y tarda milisegundos.

    python scripts/textorder.py zFoundation
    python scripts/textorder.py zSim --saltos          # solo los saltos de delta
    python scripts/textorder.py zMisc --lista          # las dos secuencias en paralelo
    python scripts/textorder.py Speed/Indep/SourceLists/zMisc

Como se lee la salida
---------------------
* **delta** de una funcion = su desplazamiento (en bytes) respecto de donde
  deberia estar, una vez alineadas las dos secuencias por su tamano acumulado.
  Un bloque entero de funciones con el MISMO delta esta simplemente empujado por
  algo que hay antes: no es una causa, es una consecuencia.
* **SALTO de delta** = el sitio exacto donde el delta CAMBIA. **Ahi esta la
  causa.** N funciones descolocadas suelen ser 3-5 saltos.
* La linea de un salto dice `+A -> +B  <simbolo>`: el simbolo nombrado es la
  primera funcion del bloque nuevo, y `B-A` es cuanto vale la causa en bytes.

El modelo de emision de GCC 2.9, MEDIDO con el compilador (r32)
---------------------------------------------------------------
Una unidad se emite en dos tramos:

* **el codigo normal**, en el orden en que estan las funciones en el fuente; y
* **el bloque de inlines DIFERIDOS**, que `finish_file` escribe al FINAL. Ahi va
  todo lo que sea `inline`: un metodo con el cuerpo DENTRO de la clase, un
  `inline` explicito en una definicion fuera de clase, y las instanciaciones de
  plantilla.

Dentro del bloque diferido el orden lo deciden dos cosas, las dos comprobadas
con `cc1plus -O1 -S` sobre casos minimos:

1. **El orden en que se PARSEAN las definiciones de clase** -- no el orden en
   que se usan. Definiendo `C`,`B`,`A` y usando `a`,`b`,`c` salen `C`,`B`,`A`.
   Dentro de una clase, el orden de declaracion de los miembros.
2. **Pasadas**: `finish_file` repite la lista hasta que no nazca nada nuevo. Lo
   que solo hace falta POR HABER EMITIDO algo de la pasada N sale entero DETRAS
   de la pasada N. Por eso los metodos de UNA MISMA clase pueden salir partidos
   en dos sitios lejanos (pasa en `RealFile::DeviceDriver` de zMisc).

Las vtables salen en orden INVERSO al de definicion de las clases.

Las palancas, de mas barata a mas cara
--------------------------------------
1. **Mover un metodo al cuerpo de la clase, o marcar `inline` su definicion
   fuera de clase.** Es la mas potente y la mas segura: no cambia una
   instruccion. `inline` en la definicion vale cuando el cuerpo usa cosas que la
   cabecera no ve, que es lo que bloqueaba `CARP::CarpResolver::StartGroup`.
   Con esto zFoundation paso de 68 descolocadas a **ORDEN PERFECTO**.
2. **El orden de las funciones dentro de un `.cpp`** (codigo normal).
3. **El orden de los `#include`**, que decide que clase se parsea antes. **Ojo:
   esta acotado por el grafo transitivo.** Medido en zMisc: `Hermes.h` incluye
   `AttribAlloc.h`, y `GameFlow.hpp` incluye `ResourceLoader.hpp` -> `driver.h`,
   asi que ninguna reordenacion de zMisc.cpp puede poner `HighAttribAlloc`
   detras de las clases de Hermes. Si el orden del objetivo es inalcanzable por
   aqui, lo que difiere es la lista de `#include` de una CABECERA nuestra.

`--tu` agrupa los saltos por fichero fuente con los STT_FILE de nuestro objeto;
en una SourceList todo cae en el mismo y no sirve -- ahi usa `--porque`.
"""
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def leer_simbolos(path):
    """[(valor, tamano, nombre, seccion)] de un ELF de 32 bits big-endian."""
    f = open(path, 'rb').read()
    if f[:4] != b'\x7fELF':
        sys.exit('%s no es un ELF' % path)
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    shentsize, shnum, shstrndx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * shentsize:shoff + i * shentsize + 40])
         for i in range(shnum)]
    shstr = S[shstrndx][4]

    def nm(off, base):
        e = f.index(b'\0', base + off)
        return f[base + off:e].decode('utf-8', 'replace')

    secn = [nm(s[0], shstr) for s in S]
    out = []
    for s in S:
        if s[1] != 2:                       # SHT_SYMTAB
            continue
        strt = S[s[6]][4]
        for o in range(s[4], s[4] + s[5], 16):
            nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', f[o:o + 16])
            n = nm(nameo, strt)
            if not n:
                continue
            if (info & 0xF) == 4:           # STT_FILE
                continue
            sec = secn[shndx] if shndx < len(secn) else ''
            out.append((val, size, n, sec))
    return out


def texto(path):
    """La secuencia de funciones de `.text` ordenada por direccion, sin duplicados."""
    vistos = {}
    for val, size, n, sec in leer_simbolos(path):
        if sec != '.text':
            continue
        if n in vistos:                     # LOCAL + GLOBAL del mismo nombre
            continue
        vistos[n] = (val, size)
    return sorted(((v, s, n) for n, (v, s) in vistos.items()))


def mapa_tu(path):
    """{simbolo: fichero fuente} leido del DWARF de nuestro objeto, si lo trae.

    No es imprescindible: sin DWARF se devuelve vacio y `--tu` no imprime nada.
    Se apoya en los STT_FILE del symtab, que `ngcc` emite delante de los
    simbolos de cada unidad de traduccion incluida.
    """
    f = open(path, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    shentsize, shnum, shstrndx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * shentsize:shoff + i * shentsize + 40])
         for i in range(shnum)]
    shstr = S[shstrndx][4]

    def nm(off, base):
        e = f.index(b'\0', base + off)
        return f[base + off:e].decode('utf-8', 'replace')

    secn = [nm(s[0], shstr) for s in S]
    m = {}
    for s in S:
        if s[1] != 2:
            continue
        strt = S[s[6]][4]
        actual = '?'
        for o in range(s[4], s[4] + s[5], 16):
            nameo, val, size, info, other, shndx = struct.unpack('>IIIBBH', f[o:o + 16])
            n = nm(nameo, strt)
            if not n:
                continue
            if (info & 0xF) == 4:
                actual = n
                continue
            sec = secn[shndx] if shndx < len(secn) else ''
            if sec == '.text':
                m.setdefault(n, actual)
    return m


def rutas(unidad):
    u = unidad.replace('\\', '/')
    cands = [u] if '/' in u else ['Speed/Indep/SourceLists/' + u, u]
    for c in cands:
        a = os.path.join(ROOT, 'build', 'GOWE69', 'obj', *c.split('/')) + '.o'
        b = os.path.join(ROOT, 'build', 'GOWE69', 'src', *c.split('/')) + '.o'
        if os.path.exists(a) and os.path.exists(b):
            return a, b, c
    sys.exit('no encuentro los dos objetos de %s\n  esperaba build/GOWE69/{obj,src}/%s.o'
             % (unidad, cands[0]))


def barrido():
    """`--todas`: pasa la medida por CADA unidad que tenga los dos objetos.

    Incluye el middleware, que no esta en `SourceLists`. Una unidad que no
    aparece en la tabla tiene el orden PERFECTO.
    """
    import io
    import contextlib
    base = os.path.join(ROOT, 'build', 'GOWE69', 'obj')
    units = []
    for root, _d, fs in os.walk(base):
        for f in fs:
            if not f.endswith('.o'):
                continue
            rel = os.path.relpath(os.path.join(root, f), base).replace(os.sep, '/')[:-2]
            if os.path.exists(os.path.join(ROOT, 'build', 'GOWE69', 'src',
                                           *rel.split('/')) + '.o'):
                units.append(rel)
    res = []
    guardado = sys.argv
    for u in units:
        try:
            buf = io.StringIO()
            sys.argv = ['textorder.py', u]
            with contextlib.redirect_stdout(buf):
                main()
            out = buf.getvalue()
            d = [l for l in out.splitlines() if 'DESCOLOCADAS' in l]
            s = [l for l in out.splitlines() if 'SALTOS' in l]
            if d and s:
                res.append((int(d[0].split(':')[1].split('de')[0]),
                            int(d[0].split('de')[1]),
                            int(s[0].split(':')[1]), u))
        except (SystemExit, Exception):
            pass
    sys.argv = guardado
    res.sort(key=lambda x: -x[0])
    print('%5s %6s %6s  %s' % ('desc', 'total', 'saltos', 'unidad'))
    mal = fns = 0
    for n, tot, s, u in res:
        if n:
            mal += 1
            fns += n
            print('%5d %6d %6d  %s' % (n, tot, s, u))
    print('\n%d de %d unidades comparables llevan el .text DESORDENADO '
          '(%d funciones fuera de sitio)' % (mal, len(res), fns))
    return 0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    ops = set(a for a in sys.argv[1:] if a.startswith('--'))
    if '--todas' in ops:
        return barrido()
    if not args:
        print(__doc__)
        return 0
    obj, nues, ruta = rutas(args[0])
    O = texto(obj)
    N = texto(nues)
    on = {n: (v, s) for v, s, n in O}
    nn = {n: (v, s) for v, s, n in N}
    comun = set(on) & set(nn)
    solo_o = [n for v, s, n in O if n not in nn]
    solo_n = [n for v, s, n in N if n not in on]

    print('%s' % ruta)
    print('  objetivo: %d funciones, .text %d B' % (len(O), sum(s for v, s, n in O)))
    print('  nuestro : %d funciones, .text %d B' % (len(N), sum(s for v, s, n in N)))
    if solo_o:
        print('  SOLO EN EL OBJETIVO (%d): %s' % (len(solo_o), ', '.join(solo_o[:6])))
    if solo_n:
        print('  SOLO NUESTRAS (%d): %s' % (len(solo_n), ', '.join(solo_n[:6])))

    # Alineamos las dos secuencias por el tamano ACUMULADO de las funciones
    # comunes: asi el delta de una funcion es su desplazamiento real dentro de la
    # unidad y no arrastra el origen distinto de los dos objetos.
    def acumulado(seq):
        pos, off = {}, 0
        for v, s, n in seq:
            if n in comun:
                pos[n] = off
                off += s
        return pos

    po = acumulado(O)
    pn = acumulado(N)
    filas = sorted(((on[n][0], on[n][1], pn[n] - po[n], n) for n in comun))
    fuera = sum(1 for v, s, d, n in filas if d != 0)
    print('  DESCOLOCADAS: %d de %d' % (fuera, len(filas)))
    if not fuera:
        print('  >>> ORDEN PERFECTO')

    saltos = []
    prev = None
    for v, s, d, n in filas:
        if prev is not None and d != prev:
            saltos.append((v, s, prev, d, n))
        prev = d
    print('  SALTOS DE DELTA: %d' % len(saltos))

    tu = mapa_tu(nues) if '--tu' in ops else {}
    for v, s, p, d, n in saltos:
        extra = ('   [%s]' % os.path.basename(tu[n])) if n in tu else ''
        print('   %08X %6d  %+7d -> %+7d   %s%s' % (v, s, p, d, n[:60], extra))

    # `--movidas`: las funciones que mas se han desplazado EN RANGO (no en bytes),
    # que es como se cazan las de causa 1. Una funcion que el objetivo pone al
    # final de la unidad y nosotros a la mitad esta definida FUERA de su clase
    # cuando el original la tenia DENTRO (o sin `inline`): es el caso mas barato
    # de cerrar, y cada uno arrastra decenas de vecinas.
    if '--movidas' in ops:
        ro = {n: i for i, (v, s, n) in enumerate(x for x in O if x[2] in comun)}
        rn = {n: i for i, (v, s, n) in enumerate(x for x in N if x[2] in comun)}
        mov = sorted(comun, key=lambda n: -abs(rn[n] - ro[n]))[:20]
        print('\n  LAS QUE MAS SE MUEVEN EN RANGO (de %d comunes):' % len(comun))
        print('  %6s %6s %7s  %s' % ('objet.', 'nuestro', 'salto', 'simbolo'))
        for n in mov:
            d = rn[n] - ro[n]
            if not d:
                break
            print('  %6d %6d %+7d  %s' % (ro[n], rn[n], d, n[:64]))

    # `--porque SIM`: la vecindad de SIM en NUESTRO objeto con la posicion que el
    # objetivo le da a cada vecina. Un salto se convierte en una causa mirando
    # que funciones emitimos justo ANTES de SIM que el objetivo emite mucho
    # despues (candidatas a `inline`/cuerpo en clase) -- o al reves.
    if '--porque' in ops:
        pedidos = args[1:]
        if not pedidos:
            pedidos = [n for v, s, p, d, n in saltos]
        idx = {n: i for i, (v, s, n) in enumerate(N)}
        for sim in pedidos:
            cand = [n for n in idx if n.startswith(sim) or sim in n]
            if not cand:
                print('\n  %s: no esta en nuestro objeto' % sim)
                continue
            n0 = min(cand, key=len)
            i = idx[n0]
            print('\n  vecindad de %s en NUESTRO objeto (T = donde lo pone el objetivo):' % n0[:60])
            for v, s, n in N[max(0, i - 6):i + 3]:
                t = ('%06X' % on[n][0]) if n in on else 'AUSENTE'
                mark = '>>' if n == n0 else '  '
                print('  %s %06X %6d  T=%-8s %s' % (mark, v, s, t, n[:64]))

    if '--lista' in ops:
        print('\n  %-58s | %s' % ('OBJETIVO', 'NUESTRO'))
        so = [n for v, s, n in O]
        sn = [n for v, s, n in N]
        for i in range(max(len(so), len(sn))):
            a = so[i] if i < len(so) else ''
            b = sn[i] if i < len(sn) else ''
            print('  %-58s %s %s' % (a[:58], '=' if a == b else '!', b[:58]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
