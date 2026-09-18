#!/usr/bin/env python3
"""stripped.py -- que dato EMITIA el original y el enlazador descarto.

Ahora que el cuello de botella es la `.rodata` (ver `r35-jf-linkdelta.md`), hay
dos huecos que se parecen y se arreglan al reves:

  * **el original no tenia ese dato** -> hay que dejar de emitirlo;
  * **el original SI lo tenia y `-strip-unused-data` se lo llevo** -> hay que
    emitirlo igual, y ademas conseguir que **nadie lo referencie**, porque si
    algo lo referencia sobrevive y estorba.

El DWARF los distingue sin ambiguedad: cuando el enlazador de SN descarta un
simbolo, deja su DIE con **`AT_location` = OP_ADDR 0xFFFFFFFF**. Eso es un
marcador, no una direccion.

El caso testigo es `zGameModes`: sus tres tablas de nombres de depuracion
(`aEmotionalSummaryTypeStrings` y companeras) salen con `0xFFFFFFFF`, o sea que
**estaban en la fuente original**; el objeto extraido no tiene `.rodata` porque
se estriparon enteras, y a nosotros nos sobreviven 104 B de residuo.

    python scripts/stripped.py              # resumen por unidad
    python scripts/stripped.py zGameModes   # detalle de una
    python scripts/stripped.py --nuestro    # ademas, cual de esas emitimos aun
"""
import os
import struct
import sys
import types
import collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
_src = open(os.path.join(ROOT, 'scripts', 'dwarfmap.py'), encoding='utf-8').read().split('def duenyo_unico')[0]
dm = types.ModuleType('dm')
dm.__dict__['__file__'] = os.path.join(ROOT, 'scripts', 'dwarfmap.py')
exec(compile(_src, 'dwarfmap.py', 'exec'), dm.__dict__)

DESCARTADO = 0xFFFFFFFF


def nuestros_datos(unidad):
    """-> {nombre: (seccion, tamano)} de los simbolos de dato de NUESTRO objeto."""
    p = os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep', 'SourceLists', unidad + '.o')
    try:
        f = open(p, 'rb').read()
    except OSError:
        return {}
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]

    def nm(o):
        return f[stro + o:f.index(b'\0', stro + o)].decode(errors='replace')

    sec = {i: nm(s[0]) for i, s in enumerate(S)}
    sym = next((s for s in S if nm(s[0]) == '.symtab'), None)
    if sym is None:
        return {}
    st = S[sym[6]][4]
    out = {}
    for i in range(sym[5] // 16):
        a, v, z, inf, _o, shn = struct.unpack('>IIIBBH', f[sym[4] + i * 16:sym[4] + i * 16 + 16])
        n = f[st + a:f.index(b'\0', st + a)].decode(errors='replace')
        if n and sec.get(shn) in ('.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss'):
            out[n] = (sec[shn], z)
    return out


def main():
    _, variables = dm.recorrer()
    por_unidad = collections.defaultdict(list)
    for a, n, u in variables:
        if a == DESCARTADO:
            por_unidad[dm.corto(u)].append(n)

    args = [x for x in sys.argv[1:] if not x.startswith('--')]
    if args:
        for u in args:
            # las claves vienen del DWARF con su capitalizacion original
            v = sorted({n for k, ns in por_unidad.items()
                        if k.lower() == u.lower() + '.cpp' for n in ns})
            print('== %s: %d simbolos que el original emitio y el enlazador descarto' % (u, len(v)))
            mios = nuestros_datos(u) if '--nuestro' in sys.argv or True else {}
            for n in v:
                marca = ''
                if n in mios:
                    marca = '   <-- NOSOTROS LO SEGUIMOS EMITIENDO (%s, %d B)' % mios[n]
                print('   %-52s%s' % (n[:52], marca))
        return

    print('%d unidades de compilacion tienen dato descartado por el enlazador' % len(por_unidad))
    print()
    print('%-26s %8s   %s' % ('unidad', 'descart', 'de esos, los que aun emitimos'))
    filas = []
    for u, v in por_unidad.items():
        v = set(v)
        if not u.startswith('z'):
            continue
        mios = nuestros_datos(u[:-4])
        vivos = sorted(v & set(mios))
        filas.append((len(vivos), len(v), u, vivos))
    filas.sort(reverse=True)
    for nv, nd, u, vivos in filas:
        print('%-26s %8d   %d  %s' % (u, nd, nv, ', '.join(x[:22] for x in vivos[:4])))


if __name__ == '__main__':
    main()
