#!/usr/bin/env python3
"""Desrota la tirada de stores del original y propone el orden de sentencias.

El planificador de GCC 2.9 no emite las asignaciones independientes en el orden
en que estaban escritas: rota la tirada, y la cantidad varia. Cuatro funciones se
cerraron desrotando a mano (`VisibleSectionManager::Unloader`,
`IVisualTreatment::Reset`, `RenderEffects`, `WRoadNetwork::Init`), dos de ellas
prediciendo el orden entero antes de compilar.

Esto lo automatiza, y con una fuente mas fuerte que la rotacion:
**`symbols/debug_lines.txt` trae la linea de fuente de cada instruccion del
original**, asi que el orden de sentencias no se adivina, se lee. La rotacion
pasa a ser lo que es — la descripcion de lo que hizo el planificador — y se
reporta como diagnostico (`--validate` mide cuanto la explica en todo el arbol).

Sin mapa de lineas queda el modelo de rotacion pura: rotar 1 a la izquierda
deshace el `E = [S(n), S(1)..S(n-1)]` del planificador.

  python scripts/derotate.py zWorld IVisualTreatment::Reset
  python scripts/derotate.py zTrack Unloader__21VisibleSectionManagerP6bChunk
  python scripts/derotate.py --scan zSim               # todas las de la unidad
  python scripts/derotate.py --scan zSim --all         # incluidas las ya en orden
  python scripts/derotate.py --sweep zSim zMain --min 96 --minsize 40
  python scripts/derotate.py --validate                # tasa de acierto, arbol entero
  python scripts/derotate.py --validate zPhysics zMain

`--sweep` es el triaje: lista las funciones sin cerrar que tienen alguna tirada
desordenada, por bytes. Validada contra las cuatro que se cerraron a mano y
contra funciones ya al 100% que no habia visto (`SimRandom::Reset`,
`OBB::Reset`): reproduce su orden de fuente, incluido el contraintuitivo.

Lectura: `ORDEN PROPUESTO` es el orden en que hay que escribir las sentencias en
el .cpp. `emision` es el orden en que salen los stores del original. Si difieren,
un fuente escrito siguiendo la emision esta mal ordenado.

Las tiradas se parten por bloque basico (etiqueta, salto o llamada) y luego por
**grupo de lineas**: los cuerpos inline viven en lineas lejanas del fichero y no
son sentencias del sitio donde se expanden, asi que se separan. El grupo marcado
`<= nivel superior` es el del cuerpo de la funcion; los demas son inlines.
"""
import os
import pickle
import re
import sys
from collections import defaultdict

ASM_ROOT = os.path.join('build', 'GOWE69', 'asm')
LINES = os.path.join('symbols', 'debug_lines.txt')
LINE_CACHE = os.path.join('build', 'GOWE69', 'derot_lines.pkl')
SYM_CACHE = os.path.join('build', 'GOWE69', 'derot_syms.pkl')
REPORT = os.path.join('build', 'GOWE69', 'report.json')

# Un salto de mas de estas lineas dentro de una tirada es otro cuerpo (un inline).
GAP = 12

RE_LINE = re.compile(r'^0x([0-9A-Fa-f]+): (\S+) \(line (\d+)\)')
RE_FN = re.compile(r'^\.fn (\S+?),')
RE_INSN = re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  (?:[0-9A-F]{2} ){4}\*/\s*(.*?)\s*$')

# Un store es una sentencia terminada. `stwu`/`stmw` son marco, no sentencia.
RE_STORE = re.compile(r'^(stw|sth|stb|stfs|stfd|stwx|sthx|stbx|stfsx|stfdx|'
                      r'stwbrx|sthbrx|psq_st|psq_stx|stswi)\b')
# Barreras: una llamada o un salto cortan la tirada; el planificador no cruza.
RE_CALL = re.compile(r'^(bl|bctrl|blrl|bla)$')
RE_BRANCH = re.compile(r'^(b|ba|bc|bdnz|bdz|beq|bne|blt|bgt|ble|bge|bso|bns|'
                       r'bctr|blr|bt|bf)')


# --------------------------------------------------------------------------
# carga

def load_lines():
    """direccion -> (fichero de EA, linea). 40 MB de texto, se cachea."""
    if (os.path.exists(LINE_CACHE)
            and os.path.getmtime(LINE_CACHE) > os.path.getmtime(LINES)):
        with open(LINE_CACHE, 'rb') as fh:
            return pickle.load(fh)
    out = {}
    with open(LINES, encoding='utf-8', errors='replace') as fh:
        for l in fh:
            m = RE_LINE.match(l)
            if m:
                a = int(m.group(1), 16)
                if a not in out:          # una direccion puede traer varias lineas
                    out[a] = (m.group(2), int(m.group(3)))
    try:
        with open(LINE_CACHE, 'wb') as fh:
            pickle.dump(out, fh)
    except OSError:
        pass
    return out


def asm_files():
    for dp, _dn, fn in os.walk(ASM_ROOT):
        for f in fn:
            if f.endswith('.s'):
                yield os.path.join(dp, f)


def scan_file(path):
    """-> {simbolo: [(direccion|None, texto)]}, con las etiquetas como marcas."""
    fns, cur = {}, None
    with open(path, encoding='utf-8', errors='replace') as fh:
        for l in fh:
            m = RE_FN.match(l)
            if m:
                cur = m.group(1)
                fns[cur] = []
                continue
            if l.startswith('.endfn'):
                cur = None
                continue
            if cur is None:
                continue
            s = l.strip()
            if s.endswith(':') and (s.startswith('.L') or s.startswith('lbl_')):
                fns[cur].append((None, s))
                continue
            m = RE_INSN.match(l)
            if m:
                fns[cur].append((int(m.group(1), 16), m.group(2).strip()))
    return fns


def sym_index():
    """simbolo -> ruta del .s. Se cachea contra el mtime del arbol de asm."""
    if os.path.exists(SYM_CACHE):
        newest = max(os.path.getmtime(p) for p in asm_files())
        if os.path.getmtime(SYM_CACHE) > newest:
            with open(SYM_CACHE, 'rb') as fh:
                return pickle.load(fh)
    idx = {}
    for p in asm_files():
        with open(p, encoding='utf-8', errors='replace') as fh:
            for l in fh:
                m = RE_FN.match(l)
                if m:
                    idx.setdefault(m.group(1), p)
    try:
        with open(SYM_CACHE, 'wb') as fh:
            pickle.dump(idx, fh)
    except OSError:
        pass
    return idx


def demangle_map():
    """nombre legible -> simbolo, del report de objdiff si esta."""
    import json
    out = {}
    if not os.path.exists(REPORT):
        return out
    try:
        with open(REPORT, encoding='utf-8') as fh:
            rep = json.load(fh)
    except ValueError:
        return out
    for u in rep.get('units', []):
        for f in u.get('functions') or []:
            d = (f.get('metadata') or {}).get('demangled_name')
            if d:
                out.setdefault(d.split('(')[0], f['name'])
    return out


# --------------------------------------------------------------------------
# el modelo

def blocks_of(insns, lines):
    """Tiradas de stores por bloque basico: corta en etiqueta, salto o llamada.

    El mapa de EA solo anota una direccion **cuando cambia la linea**: las
    instrucciones sin entrada heredan la de la anterior, que es la semantica de
    una tabla de lineas. Sin heredar, la mitad de los stores salen sin linea y
    la herramienta se cae a adivinar por rotacion sin necesidad.
    """
    out, cur = [], []
    last = (None, None)
    for addr, text in insns:
        if addr is None:                       # etiqueta: entra otro flujo
            if len(cur) > 1:
                out.append(cur)
            cur = []
            continue
        last = lines.get(addr, last)
        op = text.split(None, 1)[0] if text else ''
        if RE_STORE.match(text):
            f, ln = last
            cur.append({'addr': addr, 'text': text, 'file': f, 'line': ln})
            continue
        if RE_CALL.match(op) or RE_BRANCH.match(op):
            if len(cur) > 1:
                out.append(cur)
            cur = []
    if len(cur) > 1:
        out.append(cur)
    return out


def groups_of(run, min_size=2):
    """Parte una tirada en grupos de lineas cercanas del mismo fichero.

    Los cuerpos inline caen en lineas lejanas (el `Reset()` que se expande en la
    linea 279 tiene su cuerpo en la 82) y no son sentencias del sitio donde se
    expanden: mezclarlos con las de alrededor da un orden absurdo.
    """
    known = [s for s in run if s['line'] is not None]
    if len(known) != len(run):
        return [(None, run)] if len(run) >= min_size else []
    by = defaultdict(list)
    for s in run:
        by[s['file']].append(s)
    out = []
    for f, ss in by.items():
        ss = sorted(ss, key=lambda s: s['line'])
        cur = [ss[0]]
        for a, b in zip(ss, ss[1:]):
            if b['line'] - a['line'] > GAP:
                out.append((f, cur))
                cur = []
            cur.append(b)
        out.append((f, cur))
    # el orden de salida es el de la emision del primer store del grupo
    for f, g in out:
        g.sort(key=lambda s: s['addr'])
    # Un grupo que son k copias exactas del mismo juego de lineas es el mismo
    # inline expandido k veces, con el planificador entrelazandolas. Cada
    # expansion es su propia tirada: mezclarlas daria un orden que no existe.
    # Ojo: varias lineas IGUALES con cuentas distintas no son eso, son una
    # asignacion encadenada (`a = b = c = 0`), y esa va entera.
    split = []
    for f, g in out:
        cnt = defaultdict(int)
        for s in g:
            cnt[s['line']] += 1
        ks = set(cnt.values())
        if len(ks) == 1 and len(cnt) > 1 and ks.pop() > 1:
            seen = defaultdict(int)
            inst = defaultdict(list)
            for s in g:
                i = seen[s['line']]
                seen[s['line']] += 1
                inst[i].append(s)
            for i in sorted(inst):
                split.append((f, inst[i]))
        else:
            split.append((f, g))
    split.sort(key=lambda p: min(s['addr'] for s in p[1]))
    return [(f, g) for f, g in split if len(g) >= min_size]


def rotation_of(emission, source):
    """k tal que emision == rotar la fuente k puestos a la derecha, o None.

    Rotar a la derecha k = las k ultimas sentencias saltan al principio, que es
    justo lo que hace el planificador (`E = [S(n), S(1)..S(n-1)]` es k=1).
    """
    n = len(source)
    for k in range(n):
        if [source[(i - k) % n] for i in range(n)] == emission:
            return k
    return None


def derotate(tie, order):
    """Desrota un empate de linea leyendo la contiguidad de la emision.

    Aqui esta la cantidad de rotacion, y por eso variaba. El planificador sube
    al principio los stores de las **ultimas** sentencias de la tirada: quedan
    despegados del bloque contiguo que forma el resto. La cantidad k es
    justamente cuantos stores de cabeza no pegan con la cola contigua, y esos k
    son los que vuelven al final.

    `WRoadNetwork::Init`: seis stores en la misma linea, el primero emitido muy
    lejos de los otros cinco -> k=1, y `nRoadMemoryUsage` se va al final. Que es
    el orden con el que la funcion casa al 100%.
    """
    n = len(tie)
    if n < 2:
        return tie, 0
    pos = [order[s['addr']] for s in tie]
    j = n - 1
    while j > 0 and pos[j] == pos[j - 1] + 1:
        j -= 1
    k = j                                   # cuantos de cabeza estan despegados
    if k == 0 or k >= n:
        return tie, 0
    return tie[k:] + tie[:k], k


def analyse(group, has_lines, order=None):
    """-> orden propuesto, rotacion detectada y en que se apoya."""
    if has_lines:
        order = order or {s['addr']: i for i, s in enumerate(group)}
        source, ks = [], []
        by = defaultdict(list)
        for s in group:
            by[s['line']].append(s)
        for ln in sorted(by):
            rot, k = derotate(by[ln], order)
            source.extend(rot)
            if k:
                ks.append(k)
        basis = 'lineas' + (' + desrotacion k=%s' % ','.join(map(str, sorted(set(ks))))
                            if ks else '')
    else:
        # Sin mapa queda el modelo: rotar 1 a la izquierda deshace la rotacion
        # del planificador.
        source = group[1:] + group[:1]
        basis = 'rotacion supuesta k=1 (sin mapa de lineas)'
    return {'emission': group, 'source': source, 'basis': basis,
            'rot': rotation_of(group, source),
            'reordered': [s['addr'] for s in source] != [s['addr'] for s in group]}


def entry_line(insns, lines):
    for addr, _t in insns:
        if addr is not None and addr in lines:
            return lines[addr]
    return (None, None)


# --------------------------------------------------------------------------
# salida

def fmt_rot(rot):
    if rot == 0:
        return 'sin rotacion (emision == fuente)'
    if rot is None:
        return 'NO es rotacion pura'
    return 'ROTACION k=%d' % rot


def block_view(bi, run, groups):
    """Vista de bloque: la desrotacion cruda, sin mirar de que fichero es cada store.

    Es lo que se hizo a mano en las cuatro funciones que cerraron. Vale justo
    cuando el bloque mezcla el cuerpo de la funcion con expansiones inline y la
    vista por lineas lo parte en trozos: ahi la sentencia que sobra al principio
    de la emision es la que va al final del fuente.
    """
    if len(groups) < 2 or len(run) < 2:
        return
    src = run[1:] + run[:1]
    print('\n  bloque %d, VISTA DE BLOQUE - %d stores en %d grupos - '
          'desrotacion k=1 (mueve el 1.o al final)' % (bi + 1, len(run), len(groups)))
    for j, s in enumerate(src):
        where = ('%s:%d' % (os.path.basename(s['file']), s['line'])
                 if s['line'] else '-')
        print('      %2d  <- emision #%-2d  %-34s  %s'
              % (j + 1, run.index(s) + 1, s['text'], where))


def show(sym, path, insns, lines, show_all=False):
    ef, el = entry_line(insns, lines)
    blocks = blocks_of(insns, lines)
    allg = [(bi, f, g)
            for bi, run in enumerate(blocks)
            for f, g in groups_of(run)]
    # El cuerpo de la funcion es el grupo cuyas lineas caen sobre la linea de
    # entrada; los demas son inlines expandidos ahi.
    best = None
    if el is not None:
        cand = [(min(abs(s['line'] - el) for s in g), i)
                for i, (_b, f, g) in enumerate(allg) if f == ef]
        if cand:
            best = min(cand)[1]
    printed = 0
    blkorder = {}
    for run in blocks:
        for i2, s in enumerate(run):
            blkorder[s['addr']] = i2
    for i, (bi, f, g) in enumerate(allg):
            has = all(s['line'] is not None for s in g)
            info = analyse(g, has, blkorder)
            if not info['reordered'] and not show_all:
                continue
            if not printed:
                print('== %s   [%s]' % (sym, os.path.relpath(path, ASM_ROOT)))
            printed += 1
            top = '   <= nivel superior' if i == best else ''
            print('\n  bloque %d, grupo %s - %d stores - base: %s - %s%s'
                  % (bi + 1, os.path.basename(f) if f else '?', len(g),
                     info['basis'], fmt_rot(info['rot']), top))
            print('    emision:')
            for j, s in enumerate(g):
                print('      %2d  %08X  %-34s  %s'
                      % (j + 1, s['addr'], s['text'],
                         'linea %d' % s['line'] if s['line'] else '-'))
            if not info['reordered']:
                print('    ORDEN PROPUESTO: el mismo (nada que mover)')
                continue
            pos = {s['addr']: j for j, s in enumerate(g)}
            print('    ORDEN PROPUESTO para el .cpp:')
            for j, s in enumerate(info['source']):
                print('      %2d  <- emision #%-2d  %-34s  %s'
                      % (j + 1, pos[s['addr']] + 1, s['text'],
                         'linea %d' % s['line'] if s['line'] else '-'))
    for bi, run in enumerate(blocks):
        gs = groups_of(run, min_size=1)
        if len(gs) < 2:
            continue
        if not printed:
            print('== %s   [%s]' % (sym, os.path.relpath(path, ASM_ROOT)))
            printed += 1
        block_view(bi, run, gs)
    return printed


# --------------------------------------------------------------------------
# validacion masiva

def collect(paths, lines):
    for p in paths:
        for sym, insns in scan_file(p).items():
            for run in blocks_of(insns, lines):
                order = {s['addr']: i for i, s in enumerate(run)}
                for f, g in groups_of(run):
                    yield p, sym, f, g, order


def validate(units):
    """Mide cuanto explica el modelo de rotacion, contra el mapa de lineas."""
    lines = load_lines()
    paths = sorted(asm_files())
    if units:
        paths = [p for p in paths
                 if os.path.splitext(os.path.basename(p))[0] in units]
    tally = defaultdict(int)
    stores = defaultdict(int)
    fns = set()
    for p, sym, f, g, order in collect(paths, lines):
        fns.add(sym)
        if not all(s['line'] is not None for s in g):
            tally['sin mapa de lineas'] += 1
            continue
        rot = rotation_of(g, analyse(g, True, order)['source'])
        key = ('ya en orden (k=0)' if rot == 0 else
               'rotacion pura k=%d' % rot if rot is not None else
               'permutacion, no rotacion')
        tally[key] += 1
        stores[key] += len(g)
    total = sum(tally.values())
    if not total:
        print('sin grupos que medir')
        return
    print('funciones con tirada: %d   grupos de >=2 stores: %d\n' % (len(fns), total))
    for k in sorted(tally, key=lambda x: -tally[x]):
        print('  %-34s %7d  %5.1f%%   (%d stores)'
              % (k, tally[k], 100.0 * tally[k] / total, stores[k]))
    con = total - tally['sin mapa de lineas']
    if not con:
        return
    ordered = tally['ya en orden (k=0)']
    puras = sum(v for k, v in tally.items() if k.startswith('rotacion pura k='))
    otras = tally['permutacion, no rotacion']
    print('\n  de los %d grupos con mapa de lineas:' % con)
    print('    ya en orden ....................... %7d  %5.1f%%'
          % (ordered, 100.0 * ordered / con))
    print('    reordenados ....................... %7d  %5.1f%%'
          % (con - ordered, 100.0 * (con - ordered) / con))
    if con - ordered:
        print('      de esos, rotacion pura .......... %7d  %5.1f%%'
              % (puras, 100.0 * puras / (con - ordered)))
        print('      de esos, otra permutacion ....... %7d  %5.1f%%'
              % (otras, 100.0 * otras / (con - ordered)))


# --------------------------------------------------------------------------

def fn_status():
    """(unidad, simbolo) -> (%, tamano) del report de objdiff."""
    import json
    out = {}
    if not os.path.exists(REPORT):
        return out
    with open(REPORT, encoding='utf-8') as fh:
        rep = json.load(fh)
    for u in rep.get('units', []):
        unit = u['name'].rsplit('/', 1)[-1]
        for f in u.get('functions') or []:
            out[(unit, f['name'])] = (f.get('fuzzy_match_percent') or 0.0,
                                      int(f.get('size') or 0))
    return out


def sweep(units, lo, hi, minsize):
    """Candidatas: funciones sin cerrar con alguna tirada de stores desordenada.

    Ordenadas por bytes, que es lo que se cobra: `matched_code` es todo-o-nada,
    asi que una funcion al 99,9% vale lo mismo que una al 0 hasta que cierra.
    """
    lines = load_lines()
    st = fn_status()
    paths = sorted(asm_files())
    if units:
        paths = [p for p in paths
                 if os.path.splitext(os.path.basename(p))[0] in units]
    rows = []
    for p in paths:
        unit = os.path.splitext(os.path.basename(p))[0]
        for sym, insns in scan_file(p).items():
            pct, size = st.get((unit, sym), (None, 0))
            if pct is None or not (lo <= pct < hi) or size < minsize:
                continue
            nre, ks, ng = 0, set(), 0
            for run in blocks_of(insns, lines):
                order = {s['addr']: i for i, s in enumerate(run)}
                gs = groups_of(run)
                ng += len(gs)
                for _f, g in gs:
                    info = analyse(g, all(s['line'] is not None for s in g), order)
                    if info['reordered']:
                        nre += 1
                        if info['rot']:
                            ks.add(info['rot'])
            if nre:
                rows.append((size, pct, unit, sym, nre, ng, sorted(ks)))
    rows.sort(reverse=True)
    print('%-52s %-12s %6s %6s  %s' % ('funcion', 'unidad', 'bytes', '%', 'tiradas desordenadas'))
    tot = 0
    for size, pct, unit, sym, nre, ng, ks in rows:
        tot += size
        print('%-52s %-12s %6d %6.2f  %d de %d%s'
              % (sym[:52], unit[:12], size, pct, nre, ng,
                 '  k=%s' % ','.join(map(str, ks)) if ks else ''))
    print('\n%d funciones, %d bytes en juego' % (len(rows), tot))


def resolve(name, idx, dm):
    if name in idx:
        return name
    if name in dm and dm[name] in idx:
        return dm[name]
    short = name.split('::')[-1]
    hits = [s for s in idx if s == short or s.startswith(short + '__')]
    if len(hits) == 1:
        return hits[0]
    if '::' in name:
        cls = name.split('::')[0]
        hits2 = [s for s in hits if cls in s]
        if len(hits2) == 1:
            return hits2[0]
        hits = hits2 or hits
    if len(hits) == 1:
        return hits[0]
    if hits:
        sys.exit('ambiguo, elige uno:\n  ' + '\n  '.join(sorted(hits)[:25]))
    return None


def main():
    args = sys.argv[1:]
    if not args or args[0] in ('-h', '--help'):
        print(__doc__)
        return
    if args[0] == '--validate':
        validate(set(args[1:]))
        return
    if args[0] == '--sweep':
        rest = args[1:]
        lo, hi, minsize = 90.0, 100.0, 0
        us = []
        i = 0
        while i < len(rest):
            if rest[i] == '--min':
                lo = float(rest[i + 1]); i += 2
            elif rest[i] == '--minsize':
                minsize = int(rest[i + 1]); i += 2
            else:
                us.append(rest[i]); i += 1
        sweep(set(us), lo, hi, minsize)
        return
    show_all = '--all' in args
    args = [a for a in args if a != '--all']
    if args[0] == '--scan':
        unit = args[1]
        lines = load_lines()
        hits = [p for p in asm_files()
                if os.path.splitext(os.path.basename(p))[0] == unit]
        if not hits:
            sys.exit('no encuentro la unidad %s' % unit)
        n = 0
        for p in hits:
            for sym, insns in sorted(scan_file(p).items()):
                n += show(sym, p, insns, lines, show_all)
        print('\n%d grupos reordenados en %s' % (n, unit))
        return

    unit, name = (args + [None, None])[:2]
    if name is None:
        unit, name = None, unit
    full = sym_index()
    idx = full
    if unit:
        idx = {k: v for k, v in full.items()
               if os.path.splitext(os.path.basename(v))[0] == unit} or full
    sym = resolve(name, idx, demangle_map())
    if sym is None:
        sys.exit('no encuentro %s' % name)
    path = idx[sym]
    lines = load_lines()
    if not show(sym, path, scan_file(path)[sym], lines, show_all):
        print('== %s: ninguna tirada de stores fuera de orden' % sym)


if __name__ == '__main__':
    main()
