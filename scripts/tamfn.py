#!/usr/bin/env python3
"""tamfn.py -- compila UN fichero con sus cflags reales y da el tamano de cada
funcion, comparado con el objetivo. Sin pasar por ninja ni por report.json.

El bucle de trabajo de un near-miss es: tocar la fuente, compilar, mirar si el
tamano se acerca, revertir si no. Con `ninja` cada vuelta cuesta minutos y
arrastra el resto del arbol; con esto cuesta un segundo y no toca nada.

    python scripts/tamfn.py src/LibSN/FSasync.c
    python scripts/tamfn.py src/LibSN/FSasync.c -fno-gcse    # con flags de mas
    python scripts/tamfn.py src/.../sfir.c --solo calcFIRCoeffs__FP11SNDFIRSTATEi

Saca solo las funciones cuyo tamano no coincide, y el delta total. Los cflags
salen de `build.ninja`, que es la unica fuente fiable: `configure.py` los monta
por biblioteca y copiarlos a mano ha costado ya varias medidas malas.
"""
import os
import shlex
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def texto(p):
    """{nombre: tamano} de los simbolos STT_FUNC de .text."""
    try:
        d = open(p, 'rb').read()
    except IOError:
        return None
    if len(d) < 0x34 or d[:4] != b'\x7fELF':
        return None
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, n, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(n)]
    stro = S[sx][4]
    nom = lambda o: d[stro + o:d.index(b'\0', stro + o)].decode()
    ix = [i for i, s in enumerate(S) if nom(s[0]) == '.text']
    if not ix:
        return {}
    ix = ix[0]
    st = [s for s in S if s[1] == 2][0]
    strt = S[st[6]][4]
    out = {}
    for q in range(st[4], st[4] + st[5], 16):
        nm, val, sz, info, other, shn = struct.unpack('>IIIBBH', d[q:q + 16])
        if shn == ix and (info & 0xF) == 2 and sz:
            out[d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1')] = sz
    return out


def cflags_de(rel_o):
    """Los cflags de la regla de build.ninja que produce ese objeto."""
    p = os.path.join(ROOT, 'build.ninja')
    L = open(p, encoding='utf-8', errors='replace').read().split('\n')
    # OJO: la sentencia `build` puede venir PARTIDA en varias lineas con `$`,
    # y entonces el objeto no esta en la linea que empieza por `build `. Hay que
    # buscar la linea que lleva el objeto y retroceder hasta el `build`.
    marca = rel_o.replace('/', os.sep) + ':'
    ini = [i for i, x in enumerate(L) if x.strip().startswith(marca)
           or x.strip().startswith('build ' + marca)]
    if not ini:
        return None, None, None
    i = ini[0]
    j = i
    while j > 0 and not L[j].startswith('build '):
        j -= 1
    # y la sentencia sigue DESPUES del objeto: el nombre de la regla va en la
    # linea siguiente. Hay que juntar hasta la primera que no acabe en `$`.
    k = i
    while k < len(L) - 1 and L[k].rstrip().endswith('$'):
        k += 1
    cab = ' '.join(x.rstrip('$').strip() for x in L[j:k + 1])
    regla = cab.split(': ', 1)[1].split()[0] if ': ' in cab else ''
    buf = ''
    tool = ''
    for x in L[i:i + 40]:
        if x.strip().startswith('toolchain_version ='):
            tool = x.split('=', 1)[1].strip()
        if x.strip().startswith('cflags ='):
            buf = x.split('=', 1)[1]
        elif buf and x.startswith('    ') and 'basedir' not in x and 'basefile' not in x:
            buf += ' ' + x.strip()
        elif buf and ('basedir' in x or not x.startswith('  ')):
            break
    # shlex y no split(): mwcc lleva `-pragma "cats off"` y partir por
    # espacios convierte una opcion en tres y aborta el compilador.
    return [t for t in shlex.split(buf.replace('$', '')) if t], regla, tool


def main():
    args = [a for a in sys.argv[1:]]
    if not args:
        sys.exit(__doc__)
    fuente = args.pop(0).replace('\\', '/')
    solo = None
    if '--solo' in args:
        k = args.index('--solo')
        solo = args[k + 1]
        del args[k:k + 2]
    extra = args
    rel = fuente[4:] if fuente.startswith('src/') else fuente
    rel_o = 'build/GOWE69/src/' + rel.rsplit('.', 1)[0] + '.o'
    cf, regla, tool = cflags_de(rel_o)
    if cf is None:
        # algunas unidades cuelgan de la raiz del objeto aunque la fuente este
        # en un subdirectorio (LibSN/FSasync.c -> build/GOWE69/src/FSasync.o)
        rel_o = 'build/GOWE69/src/' + os.path.basename(rel).rsplit('.', 1)[0] + '.o'
        cf, regla, tool = cflags_de(rel_o)
    if cf is None:
        sys.exit('no encuentro la regla de %s en build.ninja' % rel_o)
    # EL COMPILADOR SALE DE LA REGLA. No todo el arbol es ProDG: media LibSN y
    # los paquetes van con mwcceppc, y darle los cflags de uno al otro falla con
    # `unknown command line option '-nodefaults'`. Peor: si quien llama solo mira
    # la ultima linea, un fallo de compilacion se lee como "sin diferencias".
    exe = 'mwcceppc.exe' if regla.startswith('mwcc') else 'ngccc.exe'
    comp = os.path.join(ROOT, 'build', 'compilers', tool.replace('/', os.sep), exe)
    if not os.path.exists(comp):
        sys.exit('no existe el compilador %s (regla %s)' % (comp, regla))
    # ngccc necesita SN_NGC_PATH; la regla de ninja lo pone antes de invocarlo,
    # asi que lo ponemos aqui y no dependemos de que lo exporte quien llame.
    env = dict(os.environ)
    env['SN_NGC_PATH'] = os.path.dirname(comp)
    tmp = os.path.join(os.environ.get('TEMP', '.'), 'tamfn_tmp.o')
    if os.path.exists(tmp):
        os.remove(tmp)
    r = subprocess.run([comp] + cf + extra + ['-c', '-o', tmp, os.path.join(ROOT, fuente)],
                       capture_output=True, text=True, cwd=ROOT, env=env)
    if not os.path.exists(tmp):
        print('FALLA la compilacion:', ' '.join([os.path.basename(comp)] + extra))
        print((r.stdout + r.stderr).strip()[-1600:])
        sys.exit(1)
    nuestro = texto(tmp)
    os.remove(tmp)
    ref = os.path.join(ROOT, 'build', 'GOWE69', 'obj', *rel.rsplit('.', 1)[0].split('/')) + '.o'
    if not os.path.exists(ref):
        ref = os.path.join(ROOT, 'build', 'GOWE69', 'obj',
                           os.path.basename(rel).rsplit('.', 1)[0] + '.o')
    obj = texto(ref)
    if obj is None:
        print('sin objeto de referencia; %d funciones compiladas' % len(nuestro))
        for k in sorted(nuestro):
            if not solo or solo in k:
                print('  %-46s %5d' % (k[:46], nuestro[k]))
        return
    delta = 0
    for k in sorted(obj, key=lambda x: -obj[x]):
        if solo and solo not in k:
            continue
        if nuestro.get(k, 0) != obj[k]:
            delta += abs(nuestro.get(k, 0) - obj[k])
            print('  %-46s objetivo %5d   nuestro %5d  %s'
                  % (k[:46], obj[k], nuestro.get(k, 0), 'AUSENTE' if k not in nuestro else ''))
    sobra = [k for k in nuestro if k not in obj]
    for k in sobra:
        print('  %-46s %13s   nuestro %5d  SOBRA' % (k[:46], '-', nuestro[k]))
    print('delta %d B   (%d/%d funciones al tamano exacto)'
          % (delta, sum(1 for k in obj if nuestro.get(k, 0) == obj[k]), len(obj)))


main()
