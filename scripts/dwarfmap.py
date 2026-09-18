#!/usr/bin/env python3
"""dwarfmap.py -- de quien es CADA dato, leido del DWARF del ELF original.

`rangechk.py` saca la atribucion de los simbolos LOCALES a partir de los 577
`STT_FILE` del `symtab`, pero **con los globales no puede**: en un `symtab` van
todos detras de los locales, asi que el `STT_FILE` que les toca es el del ultimo
fichero y miente. Y los globales son justo los que caen en los huecos entre
rangos, o sea donde esta el trabajo.

El ELF trae ademas **87,7 MB de `.debug`** en DWARF-1 (lo que emite
`GNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube` con `-g`), y ahi **cada
variable cuelga de su unidad de compilacion**, sea local o global. Eso cierra la
pregunta sin adivinar.

De paso, cada `TAG_compile_unit` trae `AT_low_pc`/`AT_high_pc`, que son el rango
`.text` EXACTO de esa unidad: comprobado contra `splits.txt`, zAI da
`0x800034A0-0x80045E3C` en los dos sitios.

Formato DWARF-1, por si hay que tocarlo: cada DIE es `long longitud`,
`short tag`, y luego pares `short atributo` + valor, donde **la forma esta en los
4 bits bajos del atributo** (1=ADDR, 2=REF, 3=BLOCK2, 4=BLOCK4, 5=DATA2,
6=DATA4, 7=DATA8, 8=STRING). Los que importan:

    0x0011  TAG_compile_unit      0x0038  AT_name        (STRING)
    0x0007  TAG_global_variable   0x0023  AT_location    (BLOCK2: 0x03 + dir)
    0x000C  TAG_local_variable    0x0111  AT_low_pc      (ADDR)
    0x0006  TAG_global_subroutine 0x0121  AT_high_pc     (ADDR)
    0x0014  TAG_subroutine        0x0258  AT_producer    (STRING)

    python scripts/dwarfmap.py              # resumen y contraste con splits.txt
    python scripts/dwarfmap.py --vars zLua  # las variables de una unidad
    python scripts/dwarfmap.py --dir 0x8041D580   # de quien es esta direccion
    python scripts/dwarfmap.py --dump       # vuelca el mapa a symbols/dwarfmap.txt
"""
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
CACHE = os.path.join(ROOT, 'symbols', 'dwarfmap.txt')

TAG_CU, TAG_GVAR, TAG_LVAR = 0x0011, 0x0007, 0x000C
AT_NAME, AT_LOC, AT_LOW, AT_HIGH = 0x0038, 0x0023, 0x0111, 0x0121


def seccion(nombre):
    f = open(ELF, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff + i * se:shoff + i * se + 40]) for i in range(sn)]
    stro = S[sx][4]
    for s in S:
        e = f.index(b'\0', stro + s[0])
        if f[stro + s[0]:e].decode(errors='replace') == nombre:
            return f[s[4]:s[4] + s[5]]
    return b''


def atributos(D, off, ln):
    p, at = off + 6, {}
    while p < off + ln:
        try:
            a = struct.unpack('>H', D[p:p + 2])[0]
        except struct.error:
            break
        p += 2
        fm = a & 0xF
        try:
            if fm in (1, 2, 6):
                v = struct.unpack('>I', D[p:p + 4])[0]; p += 4
            elif fm == 3:
                n = struct.unpack('>H', D[p:p + 2])[0]; v = D[p + 2:p + 2 + n]; p += 2 + n
            elif fm == 4:
                n = struct.unpack('>I', D[p:p + 4])[0]; v = D[p + 4:p + 4 + n]; p += 4 + n
            elif fm == 5:
                v = struct.unpack('>H', D[p:p + 2])[0]; p += 2
            elif fm == 7:
                v = D[p:p + 8]; p += 8
            elif fm == 8:
                e = D.index(b'\0', p); v = D[p:e].decode('latin1'); p = e + 1
            else:
                break
        except (struct.error, ValueError):
            break
        at[a] = v
    return at


def recorrer():
    """-> (unidades, variables). unidades: [(nombre, low, high)];
    variables: [(direccion, nombre, unidad)]."""
    D = seccion('.debug')
    if not D:
        return [], []
    unidades, variables = [], []
    cur = '(sin unidad)'
    off, n = 0, len(D)
    up = struct.unpack
    while off < n - 4:
        ln = up('>I', D[off:off + 4])[0]
        if ln < 6:
            off += 4 if ln < 4 else ln
            continue
        tag = up('>H', D[off + 4:off + 6])[0]
        if tag == TAG_CU:
            at = atributos(D, off, ln)
            cur = at.get(AT_NAME, cur)
            unidades.append((cur, at.get(AT_LOW, 0), at.get(AT_HIGH, 0)))
        elif tag in (TAG_GVAR, TAG_LVAR):
            at = atributos(D, off, ln)
            loc = at.get(AT_LOC)
            # una direccion es un bloque OP_ADDR (0x03) + 4 bytes; lo demas
            # (registros, marcos de pila) no nos interesa
            if isinstance(loc, bytes) and len(loc) == 5 and loc[0] == 0x03:
                variables.append((up('>I', loc[1:5])[0], at.get(AT_NAME, ''), cur))
        off += ln
    return unidades, variables


def corto(u):
    return os.path.basename(u.replace('\\', '/'))


def rangos_splits():
    d, cur = {}, None
    RE = re.compile(r'\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)')
    for l in open(os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt'), encoding='utf-8'):
        if not l.startswith('\t') and l.rstrip().endswith(':'):
            cur = l.rstrip().rstrip(':')
            continue
        m = RE.match(l)
        if m and cur:
            d.setdefault(cur, {})[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return d


def duenyo_unico(variables):
    """-> {direccion: (unidad, nombre)} solo con las direcciones de dueno CLARO.

    El DWARF lista una variable en **cada** unidad que la DECLARA: una estatica
    de clase en una cabecera compartida sale en las trece unidades que la
    incluyen. Sin este filtro salen 748 parejas mal atribuidas y casi todas son
    ruido; con el, 21 y todas reales.
    """
    from collections import defaultdict
    d = defaultdict(set)
    nom = {}
    for a, n, u in variables:
        d[a].add(corto(u))
        nom[a] = n
    return {a: (next(iter(s)), nom[a]) for a, s in d.items() if len(s) == 1}


def limites(variables):
    """Deriva el limite entre unidades vecinas: entre el ULTIMO dato atribuido a
    una y el PRIMERO de la siguiente. Es un intervalo, no un punto, pero acota."""
    uni = duenyo_unico(variables)
    R = rangos_splits()
    idx = sorted((a, b, corto(u), s) for u, secs in R.items() for s, (a, b) in secs.items())

    def sec_de(v):
        lo, hi = 0, len(idx)
        while lo < hi:
            m = (lo + hi) // 2
            if idx[m][0] <= v:
                lo = m + 1
            else:
                hi = m
        if lo and idx[lo - 1][0] <= v < idx[lo - 1][1]:
            return idx[lo - 1][2], idx[lo - 1][3], idx[lo - 1][1]
        return None, None, None

    filas = []
    orden = sorted(uni)
    for i, a in enumerate(orden):
        u, n = uni[a]
        du, sec, fin = sec_de(a)
        if du is None or du == u:
            continue
        # cuanto hay que mover el limite: hasta el primer dato del dueno legitimo
        anterior = None
        for b in reversed(orden[:i]):
            if uni[b][0] == du:
                anterior = b
                break
        filas.append((u, du, sec, a, n, anterior))
    return filas


def main():
    unidades, variables = recorrer()
    print('%d unidades de compilacion, %d variables con direccion' % (len(unidades), len(variables)))
    if not variables:
        return

    if '--limites' in sys.argv:
        from collections import defaultdict
        g = defaultdict(list)
        for u, du, sec, a, n, ant in limites(variables):
            g[(u, du, sec)].append((a, n, ant))
        print('%d direcciones con dueno unico' % len(duenyo_unico(variables)))
        print()
        print('== LIMITES A CORREGIR (el DWARF dice el dueno; splits.txt dice otro)')
        print('   El corte va entre el ultimo dato del dueno declarado y el primero del real.')
        for (u, du, sec), v in sorted(g.items(), key=lambda x: -len(x[1])):
            a, n, ant = min(v)
            hueco = ('0x%08X' % ant) if ant else '(ninguno antes)'
            print('   el rango %-8s de %-24s se queda %3d dato(s) de %s'
                  % (sec, du[:24], len(v), u))
            print('        el corte va entre %s y 0x%08X (%s)' % (hueco, a, n[:30]))
        return

    if '--dump' in sys.argv:
        os.makedirs(os.path.dirname(CACHE), exist_ok=True)
        with open(CACHE, 'w', encoding='utf-8') as fh:
            for a, n, u in sorted(variables):
                fh.write('0x%08X\t%s\t%s\n' % (a, n, corto(u)))
        print('mapa volcado en %s' % CACHE)
        return

    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if '--dir' in sys.argv and args:
        objetivo = int(args[0], 16)
        cerca = sorted(variables, key=lambda v: abs(v[0] - objetivo))[:6]
        print()
        for a, n, u in sorted(cerca):
            print('   0x%08X %-40s %s' % (a, n[:40], corto(u)))
        return
    if '--vars' in sys.argv and args:
        for a, n, u in sorted(variables):
            if any(x in corto(u) for x in args):
                print('   0x%08X %-44s %s' % (a, n[:44], corto(u)))
        return

    # contraste con splits.txt: variables cuya direccion cae en el rango de OTRA unidad
    R = rangos_splits()
    idx = sorted((a, b, u, s) for u, secs in R.items() for s, (a, b) in secs.items())

    def duenyo(v):
        lo, hi = 0, len(idx)
        while lo < hi:
            m = (lo + hi) // 2
            if idx[m][0] <= v:
                lo = m + 1
            else:
                hi = m
        if lo and idx[lo - 1][0] <= v < idx[lo - 1][1]:
            return corto(idx[lo - 1][2]), idx[lo - 1][3]
        return None, None

    from collections import defaultdict
    mal = defaultdict(list)
    fuera = 0
    for a, n, u in variables:
        du, sec = duenyo(a)
        if du is None:
            fuera += 1
            continue
        if du != corto(u):
            mal[(corto(u), du, sec)].append((a, n))
    print('%d variables sin rango declarado' % fuera)
    print()
    print('== VARIABLES QUE CAEN EN EL RANGO DE OTRA UNIDAD (el DWARF manda)')
    filas = sorted(mal.items(), key=lambda x: -len(x[1]))
    print('   %d parejas unidad->rango ajeno' % len(filas))
    for (uu, du, sec), v in filas[:30]:
        a, n = min(v)
        print('   %-26s -> rango de %-24s %-8s %3d vars, 1a 0x%08X %s'
              % (uu[:26], du[:24], sec, len(v), a, n[:26]))


if __name__ == '__main__':
    main()
