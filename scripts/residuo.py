#!/usr/bin/env python3
"""residuo.py -- cuanto de la `.rodata` del objetivo NO es dato, sino restos.

`-strip-unused-data` no borra el simbolo muerto entero: le quita `size & ~7` y
**deja sus ultimos `size % 8` bytes**, mas el relleno de alineacion. Con eso, una
cadena de 14 bytes como `"Attrib::Array\\0"` sobrevive en el binario final
convertida en `"Array\\0"`, seis bytes sueltos que no significan nada.

De ahi sale una consecuencia incomoda: **parte de los bytes que nos faltan en
`.rodata` no son dato que haya que escribir bien, sino el RESTO de un dato que el
original emitia y nosotros no escribimos nunca**. Para casar esas unidades hay
que emitir la cadena entera y ademas conseguir que nadie la referencie, para que
el enlazador la estripe y deje el mismo resto.

Esto lo mide. Metodo:

1. Se construye un vocabulario con **todas** las cadenas del arbol (objetos
   extraidos y nuestros), que es donde estan las candidatas completas.
2. Para cada cadena `S` de la `.rodata` del objetivo se busca una `T` mas larga
   del vocabulario tal que `len(T) % 8 == len(S)` y `T` acabe en `S`. Esa `S` no
   es una cadena: es el resto de `T`.
3. Se mira si nosotros emitimos `T`. Si no, esos bytes nos faltan **y no se
   arreglan escribiendo `S`**.

Falsos positivos: una cadena corta legitima puede ser por casualidad el final de
otra mas larga. Por eso se exige que la longitud cuadre **exactamente** con la
aritmetica del estripado, y aun asi conviene mirar la lista antes de creersela.

    python scripts/residuo.py            # todas las SourceLists
    python scripts/residuo.py zFEng      # detalle de una
"""
import glob
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJ = os.path.join(ROOT, 'build', 'GOWE69', 'obj')
SRC = os.path.join(ROOT, 'build', 'GOWE69', 'src')
CON = ('.rodata', '.data', '.sdata', '.sdata2', '.over')
RE_STR = re.compile(rb'[\x20-\x7e]{2,}\x00')


def secciones(p):
    try:
        f = open(p, 'rb').read()
    except OSError:
        return b''
    if f[:4] != b'\x7fELF':
        return b''
    try:
        shoff = struct.unpack('>I', f[0x20:0x24])[0]
        se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
        S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
        stro = S[sx][4]
    except (struct.error, IndexError):
        return b''
    out = b''
    for s in S:
        n = f[stro + s[0]:f.index(b'\0', stro + s[0])].decode(errors='replace')
        if n in CON and s[5]:
            out += f[s[4]:s[4] + s[5]]
    return out


def cadenas(d):
    return [m.group() for m in RE_STR.finditer(d)]


def vocabulario():
    """-> {resto: [cadena completa]} de todo el arbol.

    Se indexa por el resto que dejaria el estripado, que es lo unico que se ve
    en el binario final."""
    voc = {}
    todas = set()
    for base in (OBJ, SRC):
        for p in glob.glob(os.path.join(base, '**', '*.o'), recursive=True):
            for c in cadenas(secciones(p)):
                todas.add(c)
    for t in todas:
        r = len(t) % 8
        if r == 0 or r >= len(t):
            continue
        voc.setdefault(t[-r:], []).append(t)
    return voc, todas


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    print('construyendo el vocabulario de cadenas del arbol...')
    voc, todas = vocabulario()
    print('%d cadenas distintas; %d restos posibles' % (len(todas), len(voc)))
    print()

    filas = []
    for p in sorted(glob.glob(os.path.join(OBJ, 'Speed', 'Indep', 'SourceLists', '*.o'))):
        u = os.path.basename(p)[:-2]
        if args and u not in args:
            continue
        obj = cadenas(secciones(p))
        nuestro = set(cadenas(secciones(os.path.join(SRC, 'Speed', 'Indep', 'SourceLists', u + '.o'))))
        restos = []
        for s in obj:
            for t in voc.get(s, ()):
                if t != s and t.endswith(s):
                    restos.append((s, t, t in nuestro))
                    break
        b_tot = sum(len(s) for s, _, _ in restos)
        b_sin = sum(len(s) for s, _, hay in restos if not hay)
        filas.append((b_sin, b_tot, len(restos), u, restos))

    filas.sort(reverse=True)
    if args:
        for b_sin, b_tot, n, u, restos in filas:
            print('== %s: %d restos, %d B, de los que %d B son de cadenas que NO emitimos' % (u, n, b_tot, b_sin))
            for s, t, hay in restos:
                print('   %-22r  resto de %-46r %s'
                      % (s.rstrip(b'\0').decode('latin1'), t.rstrip(b'\0').decode('latin1')[:46],
                         '' if hay else '<-- no la emitimos'))
        return

    print('%-16s %8s %8s %8s   restos de cadenas que no emitimos' % ('unidad', 'restos', 'B', 'B sin'))
    for b_sin, b_tot, n, u, restos in filas:
        if not n:
            continue
        ej = ', '.join(repr(s.rstrip(b'\0').decode('latin1')) for s, _, hay in restos[:3] if not hay)
        print('%-16s %8d %8d %8d   %s' % (u, n, b_tot, b_sin, ej[:52]))
    print()
    print('TOTAL: %d B de resto en el objetivo, %d B de ellos de cadenas que no emitimos'
          % (sum(f[1] for f in filas), sum(f[0] for f in filas)))


if __name__ == '__main__':
    main()
