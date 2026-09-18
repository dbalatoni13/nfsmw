#!/usr/bin/env python3
"""triage.py -- clasifica las near-miss por lo que de verdad les pasa.

HISTORIA, porque las dos versiones anteriores mintieron y conviene saber como:

  v1 comparaba el multiconjunto de mnemonicos. Punto ciego: **netea los bloques
  movidos**, asi que daba por muro funciones con codigo desplazado.

  v2 conto las filas INSERT/DELETE de objdiff en crudo. Punto ciego peor:
  **objdiff alinea sobre el TEXTO EXACTO de la instruccion**, asi que un tramo
  que solo difiere en un registro no puede emparejarse, y cualquier instruccion
  identica que caiga dentro hace de ANCLA y parte el tramo entero en un bloque
  DELETE mas un bloque INSERT. **Una sola instruccion desplazada se contabiliza
  como 2N filas movidas.** Medido sobre las 115 funciones que v2 marcaba como
  estructurales: 762 filas crudas contra **392 reales**, con **41 funciones
  (33.848 B) donde no falta ni sobra NADA** y solo hay reorden, y 21 funciones
  (47.032 B) infladas por 3 o mas.

Esta version mira tres cosas:

  falta/sobra  el multiconjunto de instrucciones NORMALIZADAS --sin registros,
               sin literales y sin inmediatos-- que hay en un lado y no en el
               otro. Esto si es codigo que falta o sobra.
  dmax         distancia de una fila movida a la fila del tipo contrario mas
               cercana. **<=20 es jitter del planificador; >60 es un bloque
               movido de verdad.** Verificado en los dos sentidos:
               `RenderFlaresOnCar` (dmax 313) iza un `@ha` 330 instrucciones
               antes, que es movimiento real y accionable; `ProcessPadsForPackage`
               tiene sus INSERT y DELETE a 2-5 filas, que es jitter y nada mas.
  delta        que opcode sobra o falta, para leerlo de un vistazo.

La regla de las copias --si el delta es solo `mr`/`fmr`/`mr.`, es muro-- iba
11/11 y **ya tiene contraejemplo**: `UpdateCameraMovers` cerro con 1.264 B siendo
"solo copias". El `mr` era el SINTOMA, no la causa: con la `eView *view` de ambito
de funcion **asignada** en sus tres sitios en vez de redeclarada, la variable tiene
tres sets, `find_base_value` da la base por desconocida, las tres `bCopy` dejan de
compartir la carga y la copia sale sola. Asi que aqui es **pista, no veto**.

Lo que SI se salva de v2: si no hay ninguna fila movida, objdiff emparejo todo
1:1, luego el orden es identico por construccion y en el muro **no puede
esconderse movimiento**. Esa direccion esta limpia y no hay que revalidarla.

    python scripts/triage.py                 # las 33 SourceLists
    python scripts/triage.py zWorld zSpeech
    python scripts/triage.py --muro          # ensena tambien el muro, con su razon
"""
import collections
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI = os.path.join(ROOT, 'objdiff-cli-windows-x86_64.exe')
SL = os.path.join(ROOT, 'src', 'Speed', 'Indep', 'SourceLists')
SCR = os.environ.get('SCRATCH', os.path.join(ROOT, 'build'))
COPIAS = {'mr', 'fmr', 'mr.'}

# Para saber si el CODIGO es el mismo aunque cambien registros, literales e
# inmediatos. Sin esto, un reparto distinto de registros se lee como codigo que
# falta, que es exactamente el error que costo una ronda.
RE_LIT = re.compile(r'lbl_[0-9A-Fa-f]{4,}|[$][A-Z]+[0-9]+|[.]LC[0-9]+')
RE_LBL = re.compile(r'[A-Za-z_$.][A-Za-z0-9_$.]*[+]0x[0-9a-fA-F]+|0x[0-9a-fA-F]{4,}')
RE_CR = re.compile(r'(?<![A-Za-z0-9_])cr[0-9]+(?![A-Za-z0-9_])')
RE_QR = re.compile(r'(?<![A-Za-z0-9_])qr[0-9]+(?![A-Za-z0-9_])')
RE_R = re.compile(r'(?<![A-Za-z0-9_])r[0-9]+(?![A-Za-z0-9_])')
RE_F = re.compile(r'(?<![A-Za-z0-9_])f[0-9]+(?![A-Za-z0-9_])')
RE_NUM = re.compile(r'-?0x[0-9a-fA-F]+|(?<![A-Za-z0-9_])-?[0-9]+')

# Las ramas LOCALES vienen formateadas distinto en cada lado --el objetivo como
# `beq 0xcf8` y nosotros como `beq Add__6RBGrid...+0x5c4`-- asi que sin esto
# TODA rama del arbol se cuenta como codigo que falta. `bl` no entra: ahi el
# destino es el nombre del llamado y si significa algo.
NO_BRANCH = ('bl', 'bla', 'blr', 'blrl', 'bctr', 'bctrl')


def norm(t):
    f = t.split(None, 1)
    if f and f[0][0] == 'b' and f[0] not in NO_BRANCH:
        return f[0] + ' T'
    t = RE_LIT.sub('LIT', t)
    t = RE_LBL.sub('L', t)
    t = RE_CR.sub('CR', t)
    t = RE_QR.sub('QR', t)
    t = RE_R.sub('R', t)
    t = RE_F.sub('F', t)
    return RE_NUM.sub('N', t)


def texto(ins):
    return [((e.get('instruction') or {}).get('formatted', '') or '').strip() for e in ins or []]


def mnem(ts):
    c = collections.Counter()
    for t in ts:
        f = t.split()
        if f:
            c[f[0]] += 1
    return c


def faltasobra(a, b):
    ca = collections.Counter(norm(t) for t in a if t)
    cb = collections.Counter(norm(t) for t in b if t)
    return sum((ca - cb).values()), sum((cb - ca).values())


def dmax(ins):
    """Con jitter del planificador el INSERT y el DELETE son vecinos; con un
    bloque de verdad movido estan lejos."""
    I = [i for i, e in enumerate(ins or []) if e.get('diff_kind') == 'DIFF_INSERT']
    D = [i for i, e in enumerate(ins or []) if e.get('diff_kind') == 'DIFF_DELETE']
    if not I or not D:
        return 0
    return max(min(abs(i - d) for d in D) for i in I)


def analiza(unit):
    base = 'Speed/Indep/SourceLists/' + unit if '/' not in unit else unit
    a = os.path.join(ROOT, 'build', 'GOWE69', 'obj', base.replace('/', os.sep) + '.o')
    b = os.path.join(ROOT, 'build', 'GOWE69', 'src', base.replace('/', os.sep) + '.o')
    if not (os.path.exists(a) and os.path.exists(b)):
        return []
    out = os.path.join(SCR, 'triage_%s.json' % unit.replace('/', '_'))
    r = subprocess.run([CLI, 'diff', '-1', a, '-2', b,
                        '-c', 'function_reloc_diffs=none',
                        '-c', 'ppc.calculatePoolRelocations=false',
                        '-o', out, '--format', 'json', 'zz'],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0 or not os.path.exists(out):
        return []
    try:
        d = json.load(open(out, encoding='utf-8'))
    finally:
        try:
            os.remove(out)
        except OSError:
            pass

    der = {s.get('name'): s for s in d['right']['symbols']}
    filas_out = []
    for s in d['left']['symbols']:
        if s.get('kind') != 'SYMBOL_FUNCTION':
            continue
        n = s.get('name')
        o = der.get(n)
        if not n or o is None:
            continue
        pct = float(o.get('match_percent') or 0)
        if pct >= 100.0:
            continue
        ta, tb = texto(s.get('instructions')), texto(o.get('instructions'))
        ma, mb = mnem(ta), mnem(tb)
        delta = {k: ma.get(k, 0) - mb.get(k, 0)
                 for k in set(ma) | set(mb) if ma.get(k, 0) != mb.get(k, 0)}
        crudas = collections.Counter(e.get('diff_kind') for e in s.get('instructions') or [])
        mov = crudas.get('DIFF_INSERT', 0) + crudas.get('DIFF_DELETE', 0)
        regs = crudas.get('DIFF_ARG_MISMATCH', 0)
        sust = crudas.get('DIFF_REPLACE', 0)
        falta, sobra = faltasobra(ta, tb)
        dm = dmax(s.get('instructions'))

        if falta or sobra:
            clase = 'FALTA CODIGO'
            # `sust` son SUSTITUCIONES: la misma fila con otro opcode. Sin este
            # numero, un `b` donde el objetivo pone `blt` se lee como "falta 1 y
            # sobra 1", o sea dos diferencias, cuando es UNA. Costo un lead malo:
            # dije que las gemelas tenian "4 que faltan y 4 que sobran, asi que
            # una forma de +4 y quitar 4 de otro sitio cierra". Son las MISMAS
            # cuatro filas, y no cierra nada.
            razon = 'faltan %d, sobran %d, de ellas %d SUST' % (falta, sobra, sust)
        elif dm > 60:
            clase = 'BLOQUE MOVIDO'
            razon = 'no falta nada; dmax %d' % dm
        elif delta and set(delta) <= COPIAS:
            # PISTA, NO VETO. Iba 11/11 hasta que `UpdateCameraMovers` cerro con
            # 1.264 B siendo "solo copias": el `mr` era el SINTOMA --con `view`
            # asignada en tres sitios en vez de redeclarada, las tres `bCopy`
            # dejan de compartir la carga y la copia sale sola--, no la causa.
            clase, razon = 'muro?', 'solo copias (pista, no veto: 1 contraejemplo)'
        elif mov:
            clase = 'muro'
            razon = 'reorden local, dmax %d (%d filas crudas, CERO reales)' % (dm, mov)
        elif not delta:
            clase, razon = 'muro', '%d registros' % regs
        else:
            clase, razon = 'muro', 'delta sin filas movidas'
        peso = (falta + sobra) * 1000 + dm
        filas_out.append((clase, peso, int(s.get('size') or 0), pct, unit, n, delta, razon))
    return filas_out


def main():
    a = [x for x in sys.argv[1:] if not x.startswith('-')]
    ver_muro = '--muro' in sys.argv
    units = a or sorted(f[:-4] for f in os.listdir(SL) if f.endswith('.cpp'))
    todo = []
    for u in units:
        todo += analiza(u)

    est = sorted((x for x in todo if x[0] == 'FALTA CODIGO'), key=lambda x: -x[1])
    mov = sorted((x for x in todo if x[0] == 'BLOQUE MOVIDO'), key=lambda x: -x[1])
    mur = sorted((x for x in todo if x[0].startswith('muro')), key=lambda x: -x[2])

    def linea(x):
        _, _, sz, pct, u, n, delta, razon = x
        d = ', '.join('%s%+d' % (k, v) for k, v in sorted(delta.items(), key=lambda y: -abs(y[1]))[:4])
        return '  %6d B %8.3f%%  %-12s %-42s %-38s %s' % (sz, pct, u, n[:42], razon, d)

    print('FALTA O SOBRA CODIGO (multiconjunto normalizado: sin registros ni inmediatos)')
    for x in est:
        print(linea(x))
    print('  --> %d funciones, %d B' % (len(est), sum(x[2] for x in est)))
    print()
    print('BLOQUE MOVIDO (no falta nada, pero esta lejos de su sitio: dmax > 60)')
    for x in mov:
        print(linea(x))
    print('  --> %d funciones, %d B' % (len(mov), sum(x[2] for x in mov)))
    print()
    print('MURO: %d funciones, %d B' % (len(mur), sum(x[2] for x in mur)))
    if ver_muro:
        for x in mur:
            print(linea(x))


if __name__ == '__main__':
    main()
