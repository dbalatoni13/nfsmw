#!/usr/bin/env python3
"""linkdelta.py -- la distancia REAL de cada unidad al enlace, seccion por seccion.

`promodist.py` y `datacmp.py` comparan los tamanos de seccion de los OBJETOS, y
eso enganya: el enlazador lleva `-strip-unused-data` y **se lleva todo lo que no
se referencia**, codigo incluido. Medido: `zLua` tiene el `.text` del objeto
+18.908 B y, una vez enlazado, el `.text` del ELF sale **exactamente igual que el
de la base**. Lo mismo `zAttribSys`, que emite tres funciones de mas (156 B) que
el objetivo no tiene y no le cuestan un byte.

Esto enlaza el proyecto entero sustituyendo **una** unidad cada vez y compara las
secciones del ELF resultante contra las del enlace base. Eso es lo que de verdad
separa a una unidad de `DOL OK`, y cambia el orden de la cola por completo: con
esta medida **20 de 28 SourceLists tienen el `.text` a delta 0** y lo que queda
es casi todo `.rodata`.

Cuesta unos 2,6 s por unidad.

    python scripts/linkdelta.py            # todas las SourceLists
    python scripts/linkdelta.py --all      # tambien las bibliotecas
    python scripts/linkdelta.py zLua zFe   # solo esas
"""
import os
import subprocess
import struct
import sys
import types

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
_src = open('scripts/trypromo.py', encoding='utf-8').read().split('def main()')[0]
tp = types.ModuleType('tp')
tp.__dict__['__file__'] = os.path.abspath('scripts/trypromo.py')
exec(compile(_src, 'trypromo.py', 'exec'), tp.__dict__)

SEC = ('.text', '.rodata', '.data', '.sdata', '.sdata2', '.bss', '.sbss', '.over', '.ctors')


def secciones(p):
    f = open(p, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]
    d = {}
    for s in S:
        n = f[stro + s[0]:f.index(b'\0', stro + s[0])].decode(errors='replace')
        if n in SEC:
            d[n] = s[5]
    return d


def enlazar(objetos, salida):
    rsp = os.path.join(tp.TMP, os.path.basename(salida) + '.rsp')
    open(rsp, 'w').write('\n'.join(objetos) + '\n')
    if os.path.exists(salida):
        os.remove(salida)
    subprocess.run([tp.LD] + tp.LDFLAGS.split() + ['-T', 'config/GOWE69/ldscript.ld',
                                                   '-o', salida, '@' + rsp],
                   capture_output=True, text=True)
    return secciones(salida) if os.path.exists(salida) else None


def main():
    base = tp.objetos_del_enlace()
    B = enlazar(base, os.path.join(tp.TMP, 'ld_base.elf'))
    if B is None:
        falta = [x for x in base if not os.path.exists(x)]
        print('el ENLACE BASE falla, asi que no se puede medir nada.')
        if falta:
            print('%d objetos de la lista de enlace no existen en disco:' % len(falta))
            for x in falta[:8]:
                print('   %s' % x)
            print()
            print('Eso NO suele ser una rotura: significa que alguien acaba de anadir')
            print('unidades a splits.txt/configure.py y aun no las ha construido.')
            print('Comprueba `git status config/ configure.py` antes de tocar nada, y')
            print('espera a que el arbol este quieto: esta medida enlaza el proyecto')
            print('entero y no vale de nada a medias.')
        return 1
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    todas = '--all' in sys.argv
    # la RUTA relativa a build/GOWE69/obj, no el basename: `resolver` construye
    # build/GOWE69/obj/<u>.o, y con el basename ninguna unidad de biblioteca
    # resuelve nunca (`avplayer` -> obj/avplayer.o, que no existe).
    pre = 'build/GOWE69/obj/'
    uni = sorted({x.replace(os.sep, '/').split(pre, 1)[-1][:-2] for x in base
                  if todas or '/SourceLists/' in x.replace(os.sep, '/')})
    if args:
        uni = [u for u in uni
               if any(a == u or a == u.rsplit('/', 1)[-1] for a in args)]
    filas = []
    saltadas = 0
    for u in uni:
        o, n = tp.resolver(u, base)
        if o is None or not os.path.exists(n):
            # `resolver` solo encuentra lo que viene de build/GOWE69/obj: una
            # unidad ya promocionada entra al enlace desde src/ y esta a delta 0
            # por definicion, no hay nada que medir.
            saltadas += 1
            continue
        E = enlazar([n if x == o else x for x in base], os.path.join(tp.TMP, 'ld_u.elf'))
        if E is None:
            filas.append((9e9, u, None, ''))
            continue
        dt = E.get('.text', 0) - B.get('.text', 0)
        resto = [(k, E.get(k, 0) - B.get(k, 0)) for k in SEC if k != '.text']
        dd = sum(abs(v) for _, v in resto)
        det = ' '.join('%s%+d' % (k[1:], v) for k, v in resto if v)
        # el .text pesa mas: un byte de codigo es mas caro de arreglar que uno de dato
        filas.append((abs(dt) * 10 + dd, u, dt, det))
    filas.sort()
    print('DISTANCIA REAL AL ENLACE -- delta de secciones del ELF completo contra la base')
    print('%d unidades medidas, %d saltadas (ya promocionadas: delta 0 por definicion)'
          % (len(filas), saltadas))
    print('%-44s %8s   %s' % ('unidad', '.text', 'resto'))
    for _, u, dt, det in filas:
        if dt is None:
            print('%-44s %8s' % (u, 'ENLACE FALLA'))
        else:
            print('%-44s %+8d   %s' % (u, dt, det or 'IGUAL'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
