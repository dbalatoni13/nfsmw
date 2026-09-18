#!/usr/bin/env python3
"""lreg.py — la tabla de allocnos DEL PROPIO COMPILADOR, para las funciones
atascadas en «permutación de registros».

Cuando una función queda al 97-99 % y el diff sólo enseña `ARG_MISMATCH` con la
misma secuencia de instrucciones, probar formas de fuente a ciegas no converge:
lo que decide quién se lleva r9 y quién r0 es el ORDEN en que `global_alloc`
recorre los allocnos, y ese orden sale de dos números por pseudo, `n_refs` y
`live_length`. GCC los imprime si le pides los volcados RTL.

    python scripts/lreg.py <ruta-unidad> [Funcion]

Vuelca, por cada pseudo de la función:

    pseudo  n_refs  live_len  size  prioridad  ->  registro final   pref

ordenado por prioridad DESCENDENTE, que es el orden real de asignación
(`allocno_compare` de gcc/global.c 2.95):

    prioridad = floor_log2(n_refs) * n_refs / live_length * 10000 * size

Cómo se usa: mira en el `.s` del objetivo qué registro lleva cada valor, mira
aquí cuál lleva el tuyo, y localiza los dos pseudos que están intercambiados.
El que en el objetivo va ANTES tiene que subir de prioridad: más `n_refs` (usar
la variable una vez más, p.ej. un contador muerto) o menos `live_length`
(acortar el rango: declararla dentro del bloque, partir la sentencia, reasignar
un parámetro ya muerto en vez de crear un temporal). Luego vuelve a medir.

Con `--rtl` añade el cuerpo RTL de `.greg` (ya con registros duros asignados),
que es donde se ve el orden de operandos real de cada `insn`.
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TOOLS = os.path.join(ROOT, 'build', 'compilers')
TMP = os.path.join(ROOT, 'build', 'GOWE69', 'lreg')

# cc1 no acepta las opciones del driver (-I, -D, -c, -o, -x...): el fuente le
# llega ya preprocesado, así que sólo se le pasan las de generación de código.
# MEDIDO: sin la `G` este filtro se comia -G0, que es JUSTO la bandera que decide
# el reparto en varias unidades: sin ella lreg reproducia el reparto del OBJETIVO
# y no el del build, asi que sus registros no eran los del build. Caza en zEcstasy.
KEEP = re.compile(r'^-(O|f|m|G|g|W|std=|ansi|pedantic)')


def floor_log2(n):
    r = -1
    while n:
        n >>= 1
        r += 1
    return r


def cflags_for(unit):
    sys.path.insert(0, os.path.join(ROOT, 'scripts'))
    import build_direct
    cwd = os.getcwd()
    os.chdir(ROOT)
    try:
        units = build_direct.parse_units()
    finally:
        os.chdir(cwd)
    lib = unit.split('/')[0]
    best = None
    for _, spec in units.items():
        out = spec[1].replace('\\', '/')
        if '/' + unit + '.o' in out:
            return spec[0], spec[2], spec[3]
        if best is None and '/' + lib + '/' in out:
            best = (None, spec[2], spec[3])
    if best is None:
        sys.exit('no encuentro cflags para %s' % unit)
    return best


def dumps_for(unit):
    src, cflags, toolchain = cflags_for(unit)
    if src is None:
        for ext in ('.c', '.cpp'):
            p = os.path.join(ROOT, 'src', *(unit + ext).split('/'))
            if os.path.exists(p):
                src = 'src/' + unit + ext
                break
    if src is None or not os.path.exists(os.path.join(ROOT, src)):
        sys.exit('no existe el fuente de %s' % unit)

    base = os.path.join(TMP, unit.replace('/', '_'))
    os.makedirs(TMP, exist_ok=True)
    tools = os.path.join(TOOLS, toolchain)
    env = dict(os.environ)
    env['SN_NGC_PATH'] = tools

    i = base + '.i'
    p = subprocess.run([os.path.join(tools, 'ngccc.exe')] + cflags + ['-E', '-o', i, src],
                       cwd=ROOT, env=env, capture_output=True, text=True)
    if p.returncode != 0:
        sys.exit('el preprocesado ha fallado:\n' + (p.stderr or p.stdout))

    cc1 = 'cc1plus.exe' if src.endswith('.cpp') else 'cc1.exe'
    keep = [f for f in cflags if KEEP.match(f)]
    p = subprocess.run([os.path.join(tools, cc1)] + keep +
                       ['-quiet', '-dl', '-dg', '-o', base + '.s', i],
                       cwd=TMP, env=env, capture_output=True, text=True)
    err = (p.stderr or '') + (p.stdout or '')
    lreg, greg = i + '.lreg', i + '.greg'
    if not os.path.exists(lreg):
        sys.exit('cc1 no ha dejado el volcado .lreg:\n' + err[:2000])
    return lreg, greg


def split_functions(path):
    """-> {nombre: [lineas]} partiendo por ';; Function <nombre>'.

    El nombre es la LINEA ENTERA tras ';; Function ': en C++ el volcado imprime
    la firma demangled completa ("void Dynamics::Articulation::Joint::Resolve()"),
    asi que quedarse con el primer token colapsaba todas las funciones que
    devuelven el mismo tipo en una sola entrada (y se perdian todas menos la
    ultima). Si dos declaraciones repiten firma se acumulan en la misma entrada.
    """
    out, cur = {}, None
    for line in open(path, encoding='utf-8', errors='replace'):
        m = re.match(r'^;; Function (.+?)\s*$', line)
        if m:
            cur = m.group(1)
            out.setdefault(cur, [])
            continue
        if cur is not None:
            out[cur].append(line)
    return out


# MEDIDO: exigir `pref (\w+)` se comia TODOS LOS PSEUDOS DE COMA FLOTANTE,
# porque sus lineas dicen `pref FLOAT_REGS or none` y el `or none` no casa
# `(\w+)(?:, else (\w+))?`. En UpdateParticles eran 152 de 240, y son justo
# los que deciden zEcstasy: la herramienta mentia POR OMISION, sin dar error.
# Ahora la preferencia es OPCIONAL: lo que necesita `allocno_compare` son
# n_refs y live_length, que van antes.
RE_REG = re.compile(
    r'^Register (\d+) used (\d+) times? across (\d+) insns?'
    r'(?: in block (\d+))?; set (\d+) times?;(.*)$')
# La PREFERENCIA se saca aparte, no dentro del patron principal. Metida dentro
# como grupo opcional, el motor la salta y la pierde; y exigida como `pref
# (\w+)` se comia TODOS LOS PSEUDOS DE COMA FLOTANTE, cuyas lineas dicen
# `pref FLOAT_REGS or none`. En UpdateParticles eran 152 de 240, y son justo
# los que deciden zEcstasy: la herramienta mentia POR OMISION, sin dar error.
RE_PREF = re.compile(r'pref ([\w ]+?)(?:, else ([\w ]+?))?[.;]')


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args:
        sys.exit(__doc__)
    unit = args[0].replace('\\', '/').rstrip('/')
    want = args[1] if len(args) > 1 else None

    lreg, greg = dumps_for(unit)
    lfn = split_functions(lreg)
    gfn = split_functions(greg) if os.path.exists(greg) else {}

    if want:
        if want in lfn:
            names = [want]
        else:
            names = [n for n in sorted(lfn) if want in n]
            if not names:
                sys.exit('no encuentro la funcion ' + want + '; hay: ' +
                         ', '.join(sorted(lfn)))
    else:
        names = sorted(lfn)

    for name in names:
        rows = []
        for line in lfn.get(name, []):
            m = RE_REG.match(line.strip())
            if m:
                pseudo = int(m.group(1))
                refs = int(m.group(2))
                length = max(1, int(m.group(3)))
                mp = RE_PREF.search(m.group(6) or '')
                pref = ((mp.group(1).strip()
                         + ('/' + mp.group(2).strip() if mp.group(2) else ''))
                        if mp else '?')
                uservar = 'user var' in (m.group(6) or '')
                pri = int((float(floor_log2(refs) * refs) / length) * 10000)
                rows.append((pri, pseudo, refs, length, uservar, pref))

        disp = {}
        for line in gfn.get(name, []):
            if line.startswith(';; Register dispositions'):
                continue
            for pseudo, hard in re.findall(r'(\d+) in (-?\d+)', line):
                disp.setdefault(int(pseudo), int(hard))
        confl = {}
        for line in gfn.get(name, []):
            m = re.match(r'^;; (\d+) conflicts:(.*)$', line)
            if m:
                confl[int(m.group(1))] = len(m.group(2).split())

        print(';; %s  (%d pseudos)' % (name, len(rows)))
        print('   %-8s %-7s %-9s %-11s %-9s %-6s %s'
              % ('pseudo', 'n_refs', 'live_len', 'prioridad', 'confl', 'reg', 'pref'))
        for pri, pseudo, refs, length, uservar, pref in sorted(rows, reverse=True):
            hard = disp.get(pseudo)
            reg = ('r%d' % hard) if hard is not None and hard >= 0 else ('-' if hard is None else 'MEM')
            print('   %-8s %-7d %-9d %-11d %-9s %-6s %s%s'
                  % (pseudo, refs, length, pri,
                     confl.get(pseudo, '-'), reg, pref, '  (var)' if uservar else ''))
        print()

    if '--rtl' in sys.argv and want:
        print(';; ---- RTL de .greg (registros duros ya asignados) ----')
        for line in gfn.get(want, []):
            if line.strip():
                sys.stdout.write(line)


main()
