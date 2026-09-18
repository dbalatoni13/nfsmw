#!/usr/bin/env python3
"""aranges.py -- las fronteras de unidad AUTORITATIVAS, leidas de `.debug_aranges`.

`config/GOWE69/splits.txt` dice donde empieza y acaba cada unidad de compilacion
en cada seccion. Hoy esas fronteras se **adivinan** (por eso hay 71 comodines
`auto_*`). Pero el ELF original trae 18.576 B de `.debug_aranges` que **nadie
habia parseado**, y ahi GCC 2.95 escribe esas mismas fronteras de su puno y letra:
`dwarfout.c:7359-7386` emite, al cerrar cada unidad, los cuatro pares

    (TEXT_BEGIN,   TEXT_END-TEXT_BEGIN)      <- .L_text_b   / .L_text_e
    (DATA_BEGIN,   DATA_END-DATA_BEGIN)      <- .L_data_b   / .L_data_e
    (RODATA_BEGIN, RODATA_END-RODATA_BEGIN)  <- .L_rodata_b / .L_rodata_e
    (BSS_BEGIN,    BSS_END-BSS_BEGIN)        <- .L_bss_b    / .L_bss_e

EL FORMATO ENTERO, palabras de 4 B big-endian, por unidad:

    [1 palabra]  offset en `.debug` del DIE `TAG_compile_unit` de la unidad
                 (es `.L_debug_b`, `dwarfout.c:7167` y `:7182`)
    [pares]      (direccion, tamano) de cada variable PUBLICA con definicion
                 TENTATIVA -- `DECL_INITIAL == NULL`, o sea COMMON/.bss
                 (`dwarfout.c:6502-6514`)
    [4 pares]    los terminadores de arriba, SIEMPRE en ese orden
    [(0,0)]      fin de unidad

Salen **316 unidades**; 194 no tienen ninguna tentativa.

EL NOMBRE sale gratis y **no hace falta `.debug_pubnames` ni el volcado de 105 MB**:
la primera palabra cae EXACTAMENTE en el `TAG_compile_unit` (0x0011) de esa
unidad -- medido, 316 de 316 -- porque `.L_debug_b` esta pegado delante de el
(`dwarfout.c:7182`). Ese DIE trae `AT_name` (0x0038, FORM_STRING) con la ruta
como la vio el compilador (`D:/mw/...`, `C:/MyWork/...`), y `normaliza()` le
quita la unidad de disco para poder cruzarla con `splits.txt`.

LA TRAMPA QUE ESTO EVITA, y es la razon de ser del fichero: **el TAMANO de esos
pares NO es el rango enlazado.** `ASM_OUTPUT_DWARF_DELTA4` resta dos etiquetas de
la MISMA seccion del MISMO objeto, asi que **el ensamblador lo resuelve antes de
enlazar**: es el tamano de la seccion EN EL OBJETO. El enlace lleva
`-strip-unused-data`, tira simbolos, y la aportacion enlazada sale mas pequena.
Resultado medido: 140 de los 315 finales de `.text` asi calculados **se solapan
con el inicio siguiente** (zAI: 305.892 B en el objeto contra 272.796 enlazados,
33.096 de mas). Los INICIOS, en cambio, son `ASM_OUTPUT_DWARF_ADDR`: una
reubicacion que resuelve el ENLAZADOR, y por eso valen al byte.

La prueba de que es eso y no otra cosa: el exceso `objeto - enlazado` sale
**positivo o cero en las 316 unidades, ni una negativa**, con 177 exactas y
1.219.784 B de diferencia total. Si el delta fuera el rango enlazado, las
diferencias irian en las dos direcciones.

Y LA SALIDA: el final bueno no hay que derivarlo del inicio siguiente. El DIE
`TAG_compile_unit` que senala la primera palabra trae `AT_low_pc`/`AT_high_pc`
(`dwarfout.c:4957-4958`), que son las MISMAS dos etiquetas `.L_text_b`/`.L_text_e`
pero emitidas como direcciones REUBICABLES, o sea resueltas por el enlazador.
Medido: `lo` coincide con el inicio de aranges en 316 de 316, y con `hi` los 316
rangos **tilan sin un solo solape** (310 pegados, 5 huecos, y los cinco son el
sitio donde viven las unidades de Metrowerks). Para `.text` usa SIEMPRE `lo/hi`;
para `.data/.rodata/.bss` solo hay inicio, y el final es el inicio del vecino.

LA SEGUNDA TRAMPA: una unidad que no aporta NADA a una seccion emite igualmente
su etiqueta BEGIN, y el enlazador amontona todas esas etiquetas vacias en el
mismo sitio. `0x804F4040` en `.bss` lo comparten SIETE unidades, seis con tamano
0. Un inicio compartido no es una frontera: es un vertedero. Por eso todo lo que
cuenta fronteras filtra por tamano > 0.

LOS CINCO CONTROLES, que corren solos y pueden fallar:
  1. inicios de `.text` contra los de codigo de `splits.txt` -> **299 de 300**.
     Si no da 299, el parseo esta mal y NINGUNA cifra vale; lo dice y no sigue.
  2. con `lo/hi` no puede haber ni un solape -> **0 de 315**.
  3. `AT_high_pc` contra los finales de `splits.txt` -> **298 de 299**.
  4. con el DELTA4 tiene que haber solapes -> **140 de 315** (es la trampa).
  5. `objeto - enlazado` nunca negativo -> **0 de 316**.
Los controles 1 y 3 fallan a la vez, y en la misma frontera: la unica de `.text`
que aranges tiene y `splits.txt` no, `0x803A2FEC`, donde `splits.txt` mete
`eathread.cpp` dentro de `eathread_semaphore.cpp`.

LO QUE NO CUBRE: 316 unidades contra 545 en `splits.txt`. Las 240 que faltan no
pasan por `dwarfout.c` -- son `libc` (109), el Dolphin SDK (96), `LibSN` (9, ahi
esta `steering.c`), `OdemuExi2` (3), `realmemcard` (3) y `Runtime.PPCEABI.H`:
todo Metrowerks. Y caen justo en los 5 huecos del control 2, que es la
confirmacion cruzada. `madidct.cpp`, `criticalpath.c` e `inittmr.cpp` SI estan.
GCC tampoco emite pares para `.sdata/.sdata2/.sbss/.ctors/.init`, que es la otra
confirmacion: de los 270 objetos comodin `auto_*`, los 68 que tienen su
direccion como inicio de unidad en aranges son 23 de `text`, 40 de `rodata`,
3 de `bss` y 2 de `data`, y **cero** de `sdata`/`sdata2`/`sbss`/`ctors`/`init`.

    python scripts/aranges.py                  # resumen + control
    python scripts/aranges.py --list           # las 316 con sus cuatro rangos
    python scripts/aranges.py --list zFe       # solo las que casan con el filtro
    python scripts/aranges.py --tent           # las definiciones tentativas
    python scripts/aranges.py --splits         # PROPUESTA en formato splits.txt
    python scripts/aranges.py --faltan         # las 113 que splits.txt no tiene
    python scripts/aranges.py --faltan --todo  # y las etiquetas vacias
    python scripts/aranges.py --sin            # unidades de splits.txt sin aranges
    python scripts/aranges.py --cotejo         # donde splits.txt lo tiene MAL
    python scripts/aranges.py --dir 0x803D0980 # de quien es esta direccion

`--cotejo` es lo que mas duele y lo que menos se ve: no son fronteras que falten
sino fronteras CORRIDAS. Salen **48, y las 48 en la misma direccion** -- aranges
pone el inicio ANTES que `splits.txt`, o sea que hoy le estamos regalando al
vecino de arriba entre 8 y 216 B de `.data`/`.rodata`/`.bss`. En `.text` no hay
ni una.

LAS TENTATIVAS, de propina y sin minar: 584 pares en 122 unidades, 302
direcciones distintas, y `splits.txt` hoy solo tiene **UN** bloque `common` de
128 B. De esas 584, 223 caen en `.bss` y 67 en `.sbss`; las otras 294 no son
COMMON de verdad sino externs en direccion fija (188 por debajo de `.init`, 98
registros de hardware en `0xCC......`).

OJO ANTES DE PEGAR NADA EN splits.txt: `memory/nfsmw-rango-no-basta.md`. Asignar
un rango huerfano a su dueno CAMBIA SU POSICION DE ENLACE y rompe el DOL. Cada
linea que sale de `--splits` es una hipotesis, no un parche.
"""
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, 'orig', 'GOWE69', 'NFSMWRELEASE.ELF')
SPLITS = os.path.join(ROOT, 'config', 'GOWE69', 'splits.txt')

TAG_CU = 0x0011
AT_SIBLING, AT_NAME, AT_LOW, AT_HIGH, AT_PRODUCER = 0x0012, 0x0038, 0x0111, 0x0121, 0x0258

# secciones de CODIGO en splits.txt: los tres nombres bajo los que puede caer un
# inicio de `.text` de aranges (el .over es codigo desplazado por el enlazador)
SEC_CODIGO = ('.init', '.text', '.over')

try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass


# ------------------------------------------------------------------ ELF crudo

def _tabla_secciones():
    """-> {nombre: (offset, tamano, direccion)} sin cargar el fichero entero."""
    fh = open(ELF, 'rb')
    cab = fh.read(0x34)
    shoff = struct.unpack('>I', cab[0x20:0x24])[0]
    se, sn, sx = struct.unpack('>HHH', cab[0x2E:0x34])
    fh.seek(shoff)
    crudo = fh.read(se * sn)
    S = [struct.unpack('>10I', crudo[i * se:i * se + 40]) for i in range(sn)]
    fh.seek(S[sx][4])
    nombres = fh.read(S[sx][5])
    d = {}
    for s in S:
        e = nombres.index(b'\0', s[0])
        d[nombres[s[0]:e].decode('latin1')] = (s[4], s[5], s[3])
    fh.close()
    return d


def _leer(sec):
    t = _tabla_secciones()
    if sec not in t:
        sys.exit('el ELF no trae %s' % sec)
    off, size, _ = t[sec]
    fh = open(ELF, 'rb')
    fh.seek(off)
    b = fh.read(size)
    fh.close()
    return b


# ------------------------------------------------------------ .debug_aranges

def _crudo():
    """-> [(offset_cu, [(dir, tam) tentativas], text, data, rodata, bss)].

    Cada rango es (direccion, tamano_en_el_objeto). El tamano NO es el enlazado:
    ver el docstring. Para `.text` usa el `hi` que anade `unidades()`.
    """
    D = _leer('.debug_aranges')
    W = struct.unpack('>%dI' % (len(D) // 4), D)
    i, out = 0, []
    while i < len(W):
        cu = W[i]
        i += 1
        pares = []
        cerrada = False
        while i + 1 < len(W):
            a, b = W[i], W[i + 1]
            i += 2
            if a == 0 and b == 0:
                cerrada = True
                break
            pares.append((a, b))
        if not cerrada or len(pares) < 4:
            sys.exit('.debug_aranges: unidad %d sin sus cuatro terminadores '
                     '(el formato no es el que cree la herramienta)' % len(out))
        out.append((cu, pares[:-4], pares[-4], pares[-3], pares[-2], pares[-1]))
    return out


# ---------------------------------------------------------- .debug (nombre)

def _atributos(D, off):
    ln = struct.unpack('>I', D[off:off + 4])[0]
    p, at = off + 6, {}
    while p < off + ln:
        a = struct.unpack('>H', D[p:p + 2])[0]
        p += 2
        fm = a & 0xF
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
        at[a] = v
    return at


def _nombres(offsets):
    """Resuelve nombre/low_pc/high_pc de cada DIE de unidad. La primera palabra
    de cada bloque de aranges cae EXACTAMENTE en el `TAG_compile_unit` (medido:
    316 de 316), asi que no hace falta recorrer los 92 MB de `.debug`."""
    D = _leer('.debug')
    out = []
    for off in offsets:
        tag = struct.unpack('>H', D[off + 4:off + 6])[0]
        if tag != TAG_CU:
            sys.exit('.debug+0x%X no es TAG_compile_unit sino 0x%04X: el offset '
                     'de aranges no apunta al DIE de unidad' % (off, tag))
        at = _atributos(D, off)
        out.append((at.get(AT_NAME, '(sin nombre)'), at.get(AT_LOW), at.get(AT_HIGH),
                    at.get(AT_PRODUCER, '')))
    return out


def normaliza(ruta):
    """`D:/mw/Speed/Indep/SourceLists/zAI.cpp` -> `Speed/Indep/SourceLists/zAI.cpp`.

    Los nombres del DWARF traen la unidad de disco de la maquina que compilo
    (`D:/mw/`, `C:/MyWork/...`), que es lo que `splits.txt` no lleva.
    """
    r = ruta.replace(chr(92), '/')
    if len(r) > 2 and r[1] == ':':
        r = r[2:].lstrip('/')
    for pre in ('mw/', 'MyWork/'):
        if r.startswith(pre):
            r = r[len(pre):]
    return r


def unidades():
    """-> lista de dicts, uno por unidad de compilacion.

    claves: nombre (ruta del DWARF), corto (basename), ruta (normalizada),
    cu (offset en .debug), lo/hi (el rango .text ENLAZADO, el bueno),
    text/data/rodata/bss = (inicio, tamano_objeto), tent = [(dir, tam)].
    """
    crudo = _crudo()
    nom = _nombres([c[0] for c in crudo])
    out = []
    for (cu, tent, t, d, r, b), (n, lo, hi, prod) in zip(crudo, nom):
        out.append(dict(nombre=n, corto=os.path.basename(normaliza(n)),
                        ruta=normaliza(n), cu=cu, lo=lo, hi=hi, prod=prod,
                        text=t, data=d, rodata=r, bss=b, tent=tent))
    return out


# --------------------------------------------------------------- splits.txt

def splits():
    """-> [(unidad, seccion, inicio, fin)] en el orden del fichero."""
    out, cur = [], None
    RE = re.compile(r'\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)')
    for l in open(SPLITS, encoding='utf-8'):
        if not l.startswith('\t') and l.rstrip().endswith(':'):
            cur = l.rstrip()[:-1]
            continue
        m = RE.match(l)
        if m and cur:
            out.append((cur, m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out


# ------------------------------------------------------------------- control

def control(U, S, verboso=True):
    """Cruza los inicios de `.text` de aranges contra los de codigo de splits.txt.

    Tiene que dar 299 de 300. Devuelve (aciertos, total, sobran).
    """
    codigo = set(a for _, s, a, _ in S if s in SEC_CODIGO)
    mios = sorted(set(u['text'][0] for u in U))
    faltan = [a for a in mios if a not in codigo]
    ok = len(mios) - len(faltan)
    if verboso:
        print('CONTROL  inicios de .text de aranges que splits.txt ya tiene: '
              '%d de %d' % (ok, len(mios)))
        for a in faltan:
            dueno = [u['corto'] for u in U if u['text'][0] == a]
            print('         no esta 0x%08X (%s)' % (a, ', '.join(dueno)))
        if (ok, len(mios)) != (299, 300):
            print('')
            print('  *** EL CONTROL NO DA 299/300. El parseo no es el que cree')
            print('  *** la herramienta y NINGUNA cifra de esta salida se puede')
            print('  *** usar. Arregla el parseo antes de mirar nada mas.')
        else:
            print('         299/300 -- el parseo vale.')
    return ok, len(mios), faltan


def control_tilado(U, verboso=True):
    """Segundo control: con lo/hi los 316 rangos de .text no pueden solaparse."""
    xs = sorted((u['lo'], u['hi'], u['corto']) for u in U)
    sol = [(a, b, c, a2) for (a, b, c), (a2, _, _) in zip(xs, xs[1:]) if b > a2]
    hue = [(b, a2, c) for (a, b, c), (a2, _, _) in zip(xs, xs[1:]) if b < a2]
    if verboso:
        print('CONTROL  .text con AT_low_pc/AT_high_pc: %d solapes, %d huecos, '
              '%d pegados' % (len(sol), len(hue), len(xs) - 1 - len(sol) - len(hue)))
        if sol:
            print('  *** con lo/hi NO puede haber solapes; el parseo esta mal')
        for b, a2, c in hue:
            print('         hueco 0x%08X..0x%08X (%7d B) tras %s'
                  % (b, a2, a2 - b, c))
    return sol, hue


def control_finales(U, S, verboso=True):
    """Cuarto control: los `hi` contra los finales de codigo de splits.txt.

    Es el control que separa las dos fuentes de final. Si un dia `hi` dejara de
    casar con splits.txt donde el inicio SI casa, es que `AT_high_pc` no es lo
    que esta herramienta cree.
    """
    fin = set(b for _, s, _, b in S if s in SEC_CODIGO)
    ini = set(a for _, s, a, _ in S if s in SEC_CODIGO)
    pares = [(u['lo'], u['hi'], u['corto']) for u in U if u['hi'] > u['lo']]
    ok = sum(1 for lo, hi, _ in pares if lo in ini and hi in fin)
    mal = [(lo, hi, n) for lo, hi, n in pares if lo in ini and hi not in fin]
    if verboso:
        print('CONTROL  AT_high_pc contra los finales de splits.txt: %d de %d '
              '(de los que tienen el inicio en splits.txt)' % (ok, ok + len(mal)))
        for lo, hi, n in mal:
            print('         0x%08X..0x%08X %s: el final no esta en splits.txt' % (lo, hi, n))
    return ok, mal


def control_estripado(U, verboso=True):
    """Quinto control, y el que PRUEBA el diagnostico de los solapes: el tamano
    del DELTA4 nunca puede ser MENOR que `hi-lo`. Si el delta fuera el rango
    enlazado, las diferencias irian en las dos direcciones; si es el tamano en el
    objeto y el enlazador solo QUITA, van todas en la misma. Medido: 0 negativas
    de 316, 177 exactas, y 1.219.784 B de diferencia total."""
    ex = [(u['text'][1] - (u['hi'] - u['lo']), u['corto']) for u in U]
    neg = [x for x in ex if x[0] < 0]
    if verboso:
        print('CONTROL  objeto - enlazado en .text: %d negativas de %d, '
              '%d exactas, %d B estripados en total'
              % (len(neg), len(ex), sum(1 for e, _ in ex if e == 0),
                 sum(e for e, _ in ex)))
        if neg:
            print('  *** una diferencia NEGATIVA refuta que el delta sea el')
            print('  *** tamano en el objeto. Revisa el diagnostico entero.')
    return neg


def control_delta(U, verboso=True):
    """Tercer control: el DELTA4 de aranges SI se solapa (140 de 315). Es la
    trampa documentada; si dejara de solaparse, es que el enlace ha cambiado."""
    xs = sorted((u['text'][0], u['text'][1]) for u in U)
    sol = sum(1 for (a, s), (a2, _) in zip(xs, xs[1:]) if a + s > a2)
    if verboso:
        print('CONTROL  .text con el DELTA4 de aranges: %d solapes de %d '
              '(es lo esperado: el delta es el tamano EN EL OBJETO)'
              % (sol, len(xs) - 1))
    return sol


# -------------------------------------------------------------------- salidas

def _fmt(u, sec, clave):
    a, s = u[clave]
    return '%-8s start:0x%08X  +%-7d' % (sec, a, s)


def cmd_list(U, filtro):
    for u in sorted(U, key=lambda x: x['lo']):
        if filtro and filtro.lower() not in u['ruta'].lower():
            continue
        print('%s' % u['ruta'])
        print('    .text    start:0x%08X end:0x%08X   (%d B enlazados; el objeto '
              'traia %d)' % (u['lo'], u['hi'], u['hi'] - u['lo'], u['text'][1]))
        for sec, k in (('.data', 'data'), ('.rodata', 'rodata'), ('.bss', 'bss')):
            a, s = u[k]
            print('    %-8s start:0x%08X            (el objeto traia %d B)'
                  % (sec, a, s))
        for a, s in u['tent']:
            print('    tentativa 0x%08X  %d B' % (a, s))


def cmd_tent(U):
    n = 0
    for u in sorted(U, key=lambda x: x['lo']):
        if not u['tent']:
            continue
        print('%s' % u['ruta'])
        for a, s in sorted(u['tent']):
            print('    0x%08X  %6d B' % (a, s))
            n += 1
    print('')
    print('%d definiciones tentativas en %d unidades (las otras %d no tienen)'
          % (n, sum(1 for u in U if u['tent']), sum(1 for u in U if not u['tent'])))


def cmd_splits(U):
    """La PROPUESTA en formato splits.txt. Los finales de .data/.rodata/.bss son
    el inicio del vecino en esa misma seccion, porque su tamano no es fiable."""
    sig = {}
    for k in ('data', 'rodata', 'bss'):
        xs = sorted(set(u[k][0] for u in U if u[k][1] > 0))
        for a, b in zip(xs, xs[1:]):
            sig[(k, a)] = b
        if xs:
            sig[(k, xs[-1])] = None
    print('# PROPUESTA -- cada linea es una HIPOTESIS, no un parche.')
    print('# Antes de pegar nada: memory/nfsmw-rango-no-basta.md.')
    print('# Los .text llevan final propio (AT_high_pc, medido: 0 solapes).')
    print('# Los demas llevan el inicio del vecino, porque su tamano es el del')
    print('# OBJETO y no el enlazado. Se omiten las aportaciones vacias.')
    for u in sorted(U, key=lambda x: x['lo']):
        print('%s:' % u['ruta'])
        if u['hi'] > u['lo']:
            print('\t.text       start:0x%08X end:0x%08X' % (u['lo'], u['hi']))
        for sec, k in (('.rodata', 'rodata'), ('.data', 'data'), ('.bss', 'bss')):
            a, s = u[k]
            if not a or s == 0:
                continue
            f = sig.get((k, a))
            print('\t%-11s start:0x%08X%s'
                  % (sec, a, ' end:0x%08X' % f if f else '   # sin final: es el ultimo'))


def cmd_faltan(U, S, todo=False):
    """Las fronteras que aranges da y splits.txt no tiene, con la unidad de
    splits.txt en cuyo rango cae hoy cada una.

    Por omision solo salen las de aportacion NO VACIA. Una unidad que no aporta
    nada a una seccion emite igualmente su etiqueta BEGIN, y el enlazador
    amontona todas esas etiquetas vacias en el mismo sitio -- 0x804F4040 en
    `.bss` lo comparten SIETE unidades, seis de ellas con tamano 0. Eso no es una
    frontera: es un vertedero. Con `--todo` salen tambien.
    """
    porsec = {}
    for u, s, a, b in S:
        porsec.setdefault(s, []).append((a, b, u))
    for v in porsec.values():
        v.sort()

    def contiene(sec, a):
        for x, y, u in porsec.get(sec, []):
            if x <= a < y:
                return u
        return None

    tot = 0
    for sec, k, alias in (('.text', 'text', SEC_CODIGO), ('.rodata', 'rodata', ('.rodata',)),
                          ('.data', 'data', ('.data',)), ('.bss', 'bss', ('.bss',))):
        tengo = set()
        for al in alias:
            tengo |= set(a for _, s, a, _ in S if s == al)
        por_dir = {}
        for u in U:
            a, s = u[k]
            if not a or a in tengo or (s == 0 and not todo):
                continue
            por_dir.setdefault(a, []).append('%s%s' % (u['ruta'], '' if s else ' [vacia]'))
        print('%-8s %d inicios que splits.txt NO tiene:' % (sec, len(por_dir)))
        for a in sorted(por_dir):
            dueno = None
            for al in alias:
                dueno = dueno or contiene(al, a)
            print('    0x%08X  hoy dentro de %s' % (a, dueno or '(ninguna unidad de splits.txt)'))
            for n in por_dir[a]:
                print('               %s' % n)
        tot += len(por_dir)
        print('')
    print('TOTAL %d fronteras autoritativas que splits.txt no tiene%s.'
          % (tot, '' if todo else ' (solo aportaciones no vacias)'))


def cmd_sin(U, S):
    """Las unidades de splits.txt que aranges no trae -- las de Metrowerks."""
    from collections import Counter
    mios = set(u['corto'] for u in U)
    dirs, sueltas = Counter(), []
    vistas = set()
    for u, _, _, _ in S:
        if u in vistas:
            continue
        vistas.add(u)
        if os.path.basename(u) not in mios:
            dirs[os.path.dirname(u) or '(sin directorio)'] += 1
            sueltas.append(u)
    print('%d unidades en splits.txt, %d en aranges (por basename), '
          '%d SIN entrada de aranges' % (len(vistas), len(vistas) - len(sueltas),
                                         len(sueltas)))
    print('')
    for k, v in dirs.most_common():
        print('%5d  %s' % (v, k))
    print('')
    huer = sorted(set(u['ruta'] for u in U
                      if u['corto'] not in set(os.path.basename(x) for x in vistas)))
    print('%d unidades de aranges que splits.txt NO conoce:' % len(huer))
    for h in huer:
        print('    %s' % h)


def cmd_cotejo(U, S):
    """Unidad por unidad: donde aranges y splits.txt DISCREPAN.

    No es lo mismo que `--faltan`. Ahi salen las fronteras que splits.txt no
    tiene; aqui las que tiene MAL, que son las peligrosas: el rango existe, el
    build lo usa, y esta corrido.
    """
    # una unidad puede tener VARIAS lineas de la misma seccion (zMisc.cpp lleva
    # dos `.bss`, la segunda `common`); hay que quedarse con todas, no con la
    # ultima, o la de COMMON se come a la de verdad
    porunidad, veces = {}, {}
    for u, s, a, b in S:
        porunidad.setdefault(os.path.basename(u), {}).setdefault(s, []).append(a)
        veces.setdefault(os.path.basename(u), set()).add(u)
    dup = {}
    for u in U:
        dup.setdefault(u['corto'], []).append(u)
    ambiguas = sorted(k for k in dup
                      if len(dup[k]) > 1 or len(veces.get(k, ())) > 1)
    filas = []
    for corto, us in sorted(dup.items()):
        if corto in ambiguas or corto not in porunidad:
            continue
        u, sp = us[0], porunidad[corto]
        for sec, k in (('.text', None), ('.rodata', 'rodata'),
                       ('.data', 'data'), ('.bss', 'bss')):
            if sec not in sp:
                continue
            if k is None:
                mio, tam = u['lo'], u['hi'] - u['lo']
            else:
                mio, tam = u[k]
            if tam == 0 or mio == 0 or mio in sp[sec]:
                continue
            filas.append((corto, sec, min(sp[sec]), mio, mio - min(sp[sec])))
    if ambiguas:
        print('(%d unidades saltadas por nombre ambiguo: %s)'
              % (len(ambiguas), ', '.join(ambiguas)))
    print('%d discrepancias de INICIO entre aranges y splits.txt '
          '(unidad emparejada por nombre, aportacion no vacia):' % len(filas))
    print('%-34s %-8s %-10s %-10s %s' % ('unidad', 'seccion', 'splits.txt', 'aranges', 'delta'))
    for corto, sec, suyo, mio, d in sorted(filas, key=lambda x: (x[1], x[2])):
        print('%-34s %-8s 0x%08X 0x%08X %+d' % (corto, sec, suyo, mio, d))
    return filas


def cmd_dir(U, val):
    a = int(val, 16)
    t = _tabla_secciones()
    nada = True
    for u in sorted(U, key=lambda x: x['lo']):
        if u['lo'] <= a < u['hi']:
            print('.text    %s  [0x%08X,0x%08X)' % (u['ruta'], u['lo'], u['hi']))
            nada = False
    for sec, k in (('.rodata', 'rodata'), ('.data', 'data'), ('.bss', 'bss')):
        fin = t[sec][2] + t[sec][1]
        # solo las aportaciones NO vacias: las vacias se amontonan y mienten
        xs = {}
        for u in U:
            if u[k][1] > 0:
                xs.setdefault(u[k][0], []).append(u['ruta'])
        orden = sorted(xs)
        for x, y in zip(orden, orden[1:] + [fin]):
            if x <= a < y:
                print('%-8s %s  [0x%08X,0x%08X)  (frontera derecha = el vecino)'
                      % (sec, ', '.join(xs[x]), x, y))
                nada = False
    for u in U:
        for x, s in u['tent']:
            if x <= a < x + s:
                print('tentativa de %s: 0x%08X +%d B' % (u['ruta'], x, s))
                nada = False
    if nada:
        print('0x%08X no cae en ningun rango de aranges (o es de Metrowerks)' % a)


def main():
    U = unidades()
    S = splits()
    av = sys.argv[1:]
    if '--dir' in av:
        cmd_dir(U, av[av.index('--dir') + 1])
        return
    if '--list' in av:
        i = av.index('--list')
        cmd_list(U, av[i + 1] if len(av) > i + 1 else None)
        return
    if '--tent' in av:
        cmd_tent(U)
        return
    if '--splits' in av:
        cmd_splits(U)
        return
    if '--faltan' in av:
        cmd_faltan(U, S, '--todo' in av)
        return
    if '--sin' in av:
        cmd_sin(U, S)
        return
    if '--cotejo' in av:
        cmd_cotejo(U, S)
        return

    print('%d unidades de compilacion en .debug_aranges' % len(U))
    print('%d con definiciones tentativas, %d sin ninguna'
          % (sum(1 for u in U if u['tent']), sum(1 for u in U if not u['tent'])))
    print('')
    control(U, S)
    control_tilado(U)
    control_finales(U, S)
    control_delta(U)
    control_estripado(U)
    print('')
    tot = 0
    for sec, k, alias in (('.text', 'text', SEC_CODIGO), ('.rodata', 'rodata', ('.rodata',)),
                          ('.data', 'data', ('.data',)), ('.bss', 'bss', ('.bss',))):
        mios = set(u[k][0] for u in U if u[k][1] > 0)
        vacias = set(u[k][0] for u in U if u[k][0] and u[k][1] == 0) - mios
        suyos = set()
        for al in alias:
            suyos |= set(a for _, s, a, _ in S if s == al)
        tot += len(mios - suyos)
        print('%-8s aranges %3d inicios con aportacion, splits.txt %3d, '
              'le faltan %3d  (+%d de etiqueta vacia, que no son frontera)'
              % (sec, len(mios), len(suyos), len(mios - suyos), len(vacias - suyos)))
    print('')
    print('%d fronteras autoritativas que splits.txt no tiene. `--faltan` las lista.' % tot)


if __name__ == '__main__':
    main()
