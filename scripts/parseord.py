#!/usr/bin/env python3
"""parseord.py -- el orden de PARSEO de las clases, que es lo que decide las vtables.

LA REGLA, encontrada y controlada en la r56: **el orden en que se emiten las
vtables de una unidad es el INVERSO del orden en que se COMPLETAN las clases**.
`walk_globals` recorre una lista que GCC construye ANTEPONIENDO, asi que sale al
reves. En `zEagl4Anim` el control salio 30 de 30.

Y LA TECNICA, que es lo que lo hace practico: **el orden de parseo se mide
preprocesando con `-E`, en SEGUNDOS y sin compilar**. Eso convierte un problema
de busqueda en algo iterable -- pruebas un orden de `#include`, mides, y solo
compilas al final para verificar con `vtord`/`fncmp`/`linkdelta`. Asi paso
`zEagl4Anim` de 26 vtables descolocadas de 31 a CERO.

    python scripts/parseord.py zEagl4Anim        # cuantas posiciones fallan
    python scripts/parseord.py zEagl4Anim -v     # la tabla, objetivo contra nuestro
    python scripts/parseord.py zCamera --ii out  # deja el preproceso para inspeccion

LO QUE MAS CUESTA VER, y esta medido: **la posicion de la CLASE y la de los
CUERPOS las mandan cosas distintas.** La clase se completa donde se parsea su
CABECERA; los cuerpos de sus metodos se emiten donde esta su `.cpp`. Para
colocar las dos hay que separar los disparadores: el `.cpp` donde el objetivo
emite los cuerpos, y la cabecera arrastrada por OTRO fichero, el que caiga en la
posicion donde el objetivo completa la clase.

Y OJO CON EL ORDEN DE LAS EDICIONES: mover una clase que arrastra a otras ANTES
de colocar esas otras EMPEORA. Medido dos veces en zEagl4Anim (11 -> 16 y
9 -> 13). Coloca primero la que va mas arriba en el orden del objetivo.
"""
import io
import os
import re
import subprocess
import sys
import types

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

NL = chr(10)


def _mod(path, corta):
    src = open(path, encoding='utf-8').read().split(NL + corta)[0]
    m = types.ModuleType('m')
    m.__dict__['__file__'] = os.path.abspath(path)
    exec(compile(src, os.path.basename(path), 'exec'), m.__dict__)
    return m


def clase_de(mang):
    """Q29EAGL4Anim7FnCycle -> `FnCycle`; `13CarRenderInfo` -> `CarRenderInfo`.

    Solo hace falta el ULTIMO componente, que es el nombre con que la clase se
    escribe en la cabecera y por tanto lo que hay que buscar en el preproceso.
    """
    s = mang
    if s.startswith('Q'):
        # OJO: el contador de `Qn` es UN SOLO DIGITO. Con `Q(\d+)` el regex se
        # come tambien el largo del primer componente --`Q29EAGL4Anim` es Q2 mas
        # 9EAGL4Anim, no Q29-- y la funcion devuelve basura en silencio.
        m = re.match(r'Q(\d)(.*)', s)
        if not m:
            return None
        s = m.group(2)
    ult = None
    while s:
        m = re.match(r'(\d+)', s)
        if not m:
            break
        n = int(m.group(1))
        ini = m.end()
        ult = s[ini:ini + n]
        s = s[ini + n:]
    return ult


def vtables(p):
    """-> [nombre de clase] en orden de DIRECCION dentro de `.rodata`."""
    sys.path.insert(0, 'scripts')
    from extrasym import Elf
    e = Elf(p)
    sec = {i: s['name'] for i, s in enumerate(e.sh)}
    out = []
    for s in e.syms():
        n = s['name'] or ''
        # OJO: en el objeto EXTRAIDO las vtables no siempre caen en .rodata --
        # por eso vtord.py no filtra por seccion--. Basta con que este definido.
        if n.startswith('_vt.') and s['shndx'] and s['shndx'] < 0xFF00:
            # `_vt.A.B` es la vtable de A para la base B: la clase es A
            c = clase_de(n[4:].split('.')[0])
            if c:
                out.append((s['value'], c))
    vis, ord_ = set(), []
    for _, c in sorted(out):
        if c not in vis:
            vis.add(c)
            ord_.append(c)
    return ord_


def preprocesa(unidad, guardar=None):
    bd = _mod('scripts/build_direct.py', 'def compile_one')
    u = bd.parse_units()
    k = [x for x in u if os.path.basename(x) == unidad or x == unidad]
    if not k:
        sys.exit('no encuentro la unidad %r en build.ninja' % unidad)
    src, out, cf, tc, rn = u[k[0]]
    cc = os.path.abspath(os.path.join('build', 'compilers', tc.replace('\\', os.sep),
                                      'ngccc.exe'))
    if not os.path.exists(cc):
        sys.exit('no existe el compilador %s' % cc)
    env = dict(os.environ)
    env['SN_NGC_PATH'] = os.path.dirname(cc)
    p = subprocess.run([cc] + [c for c in cf if c.strip()] + ['-E', src],
                       capture_output=True, text=True, errors='replace', env=env)
    if p.returncode:
        sys.exit('el preproceso falla:' + NL + (p.stderr or '')[:800])
    if guardar:
        io.open(guardar, 'w', encoding='utf-8', errors='replace').write(p.stdout)
    return p.stdout, src


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not args:
        sys.exit(__doc__)
    unidad = args[0]
    verbose = '-v' in sys.argv
    guardar = None
    if '--ii' in sys.argv:
        i = sys.argv.index('--ii')
        guardar = sys.argv[i + 1] if i + 1 < len(sys.argv) else 'preproceso.ii'

    ld = _mod('scripts/linkdelta.py', 'def main()')
    base = ld.tp.objetos_del_enlace()
    rutas = [x for x in (unidad, 'Speed/Indep/SourceLists/' + unidad)]
    o = n = None
    for r in rutas:
        o, n = ld.tp.resolver(r, base)
        if o:
            break
    if not o:
        sys.exit('%s no esta en la lista de enlace (ya promocionada?)' % unidad)

    obj_vt = vtables(o)
    # LA REGLA: el orden de parseo es el INVERSO del de vtables
    objetivo = list(reversed(obj_vt))

    txt, src = preprocesa(os.path.basename(unidad), guardar)
    pos = {}
    for c in set(objetivo):
        m = re.search(r'\b(?:class|struct)\s+%s\b\s*(:|\{)' % re.escape(c), txt)
        if m:
            pos[c] = m.start()
    actual = [c for _, c in sorted((p, c) for c, p in pos.items())]
    sinloc = [c for c in objetivo if c not in pos]

    # comparar solo las que se han podido localizar, en el orden del objetivo
    obj_f = [c for c in objetivo if c in pos]
    mal = sum(1 for a, b in zip(actual, obj_f) if a != b)

    # EL CONTROL, y es POR UNIDAD: la regla dice que invertir el orden de parseo
    # da el orden de vtables. Se comprueba contra NUESTRO objeto, cuyo orden de
    # vtables conocemos. Si no cuadra, la medida NO vale para esta unidad y hay
    # que decirlo -- en `zMain` esta herramienta daba 179 de 181 mientras
    # `vtord` daba 21 de 189, y la causa es que el ultimo componente del nombre
    # mangled no identifica la clase cuando hay homonimas en varios espacios de
    # nombres, o cuando una clase tiene varias vtables.
    control = None
    if n and os.path.exists(n):
        nue_vt = vtables(n)
        pred = [c for c in reversed(actual)]
        comunes = [c for c in nue_vt if c in pos]
        if comunes:
            ok = sum(1 for a, b in zip(comunes, pred) if a == b)
            control = (ok, len(comunes))
    print('%s: %d vtables en el objetivo, %d clases localizadas en el preproceso'
          % (unidad, len(obj_vt), len(actual)))
    print('posiciones de PARSEO que no casan: %d de %d' % (mal, len(obj_f)))
    if control:
        ok, tot = control
        if ok < tot:
            print()
            print('!! CONTROL FALLIDO: invertir el orden de parseo reproduce solo')
            print('   %d de %d vtables de NUESTRO objeto. La regla no se puede' % (ok, tot))
            print('   aplicar aqui por nombre de clase: probablemente hay clases')
            print('   homonimas en varios espacios de nombres, o alguna con mas de')
            print('   una vtable. **NO uses esta cifra para repartir trabajo.**')
        else:
            print('CONTROL OK: invertir el parseo reproduce las %d vtables de nuestro objeto'
                  % tot)
    if sinloc:
        print('sin localizar (%d): %s' % (len(sinloc), ', '.join(sinloc[:8])))
        print('  -- suelen ser plantillas o clases declaradas de otra forma;')
        print('     no entran en la cuenta, pero SI ocupan sitio en la realidad.')
    if verbose:
        print()
        print('%-3s %-30s %-30s' % ('#', 'PARSEO OBJETIVO', 'PARSEO NUESTRO'))
        for i, (a, b) in enumerate(zip(obj_f, actual)):
            print('%-3d %-30s %-30s%s' % (i, a[:30], b[:30], '  <<<' if a != b else ''))
    return 0


if __name__ == '__main__':
    sys.exit(main())
