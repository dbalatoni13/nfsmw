#!/usr/bin/env python3
"""Plan de reconstruccion de una funcion: asm + linea de fuente + arbol de inlines.

`fuse.py` funde dos capas (desensamblado y `symbols/debug_lines.txt`). Falta la
tercera, y es la que mas manda: el volcado DWARF trae **por funcion el arbol
completo de expansiones inline con rangos de direccion**, los bloques anonimos
con su rango, y las locales de cada ambito con el registro que el compilador
original les asigno. Son 163.904 rangos, 643.106 expansiones y 40.256 locales
con registro que estabamos leyendo a ojo.

Con las tres capas juntas, escribir una funcion deja de ser adivinar la forma de
la fuente y pasa a ser transcribir: se ve que inline se expandio en cada
instruccion, donde abre y cierra cada bloque, y que variable vive en que
registro.

  python scripts/plan.py <unidad> "Clase::Metodo"      # el plan
  python scripts/plan.py <unidad> "Clase::Metodo" --dwarf-only
  python scripts/plan.py --index                       # reconstruye el indice

El indice del volcado (105 MB) se cachea en build/GOWE69/dwarf_index.pkl.
"""
import io
import os
import pickle
import re
import sys
from collections import defaultdict

DUMP = os.path.join('symbols', 'mw_dwarfdump.nothpp')
INDEX = os.path.join('build', 'GOWE69', 'dwarf_index.pkl')
ASM_ROOT = os.path.join('build', 'GOWE69', 'asm', 'Speed', 'Indep', 'SourceLists')
LINES = os.path.join('symbols', 'debug_lines.txt')

RE_RANGE = re.compile(r'^\s*// Range: 0x([0-9A-Fa-f]+) -> 0x([0-9A-Fa-f]+)')
RE_ADDRLINE = re.compile(r'^0x([0-9A-Fa-f]+): (\S+) \(line (\d+)\)')
RE_FN = re.compile(r'^\.fn (\S+?),')
RE_INSN = re.compile(r'^/\* ([0-9A-F]{8}) [0-9A-F]{8}  (?:[0-9A-F]{2} ){4}\*/\s*(.*?)\s*$')
# El comentario de una local es su registro (`// f24`) o su ranura de pila
# (`// r1+0x28`). Sin la segunda forma se pierden justo los arrays y structs.
RE_LOCAL = re.compile(
    r'^\s+(.+?);\s*(?://\s*([rf]\d+(?:\s*\+\s*0x[0-9A-Fa-f]+)?))?\s*$')

# Aperturas a indent 0 que NO son funciones.
RE_NOT_FN = re.compile(r'^\s*(struct|class|union|enum|namespace|typedef|template)\b')


def is_fn_header(stripped):
    """Una definicion de funcion del volcado: abre bloque y lleva parentesis

    ANTES de la llave. Mirar solo el principio de la linea descartaba toda
    funcion que devuelve `struct T *`, `class T *` o un enum: nunca entraban en
    el indice. Un tipo (`struct T {`) no tiene parentesis delante de la llave;
    una funcion si.
    """
    if stripped.endswith(';'):
        return False
    if not (stripped.endswith('{') or stripped.endswith('{}')):
        return False
    head = stripped.split('{', 1)[0]
    return '(' in head


def build_index():
    """-> {clave: [(offset_de_linea, n_linea)]} para saltar directo a cada funcion."""
    idx = defaultdict(list)
    with io.open(DUMP, encoding='utf-8', errors='replace') as fh:
        n = 0
        while True:
            pos = fh.tell()
            line = fh.readline()
            if not line:
                break
            n += 1
            if line[:1] in (' ', '\t', '\n'):
                continue
            s = line.strip()
            if not is_fn_header(s):
                continue
            # el nombre es lo que va justo antes del parentesis de argumentos
            head = s[:s.index('(')]
            name = head.split()[-1] if head.split() else ''
            name = name.lstrip('*&')
            if not name:
                continue
            idx[name].append((pos, n))
            short = name.rsplit('::', 1)[-1]
            if short != name:
                idx[short].append((pos, n))
    os.makedirs(os.path.dirname(INDEX), exist_ok=True)
    with open(INDEX, 'wb') as fh:
        pickle.dump(dict(idx), fh, protocol=4)
    return dict(idx)


def load_index(rebuild=False):
    if not rebuild and os.path.exists(INDEX):
        if os.path.getmtime(INDEX) >= os.path.getmtime(DUMP):
            with open(INDEX, 'rb') as fh:
                return pickle.load(fh)
    sys.stderr.write('construyendo el indice del volcado (una vez, ~1 min)...\n')
    return build_index()


def read_fn_block(offset):
    """Lee desde `offset` hasta que se cierra el bloque de la funcion. -> [(indent, texto)]"""
    out = []
    with io.open(DUMP, encoding='utf-8', errors='replace') as fh:
        fh.seek(offset)
        first = fh.readline()
        out.append((0, first.rstrip('\n')))
        depth = 1
        while depth > 0:
            line = fh.readline()
            if not line:
                break
            s = line.rstrip('\n')
            ind = len(s) - len(s.lstrip())
            body = s.strip()
            depth += body.count('{') - body.count('}')
            out.append((ind, s))
            if depth <= 0:
                break
    return out


def parse_events(block):
    """-> (cabecera, [eventos]) donde cada evento lleva rango, tipo y texto."""
    header = block[0][1].strip()
    events = []
    pending = None          # rango leido, esperando al constructo
    stack = []              # (indent, indice_del_evento)
    fn_locals = []
    in_locals = False

    for ind, raw in block[1:]:
        body = raw.strip()
        if not body:
            continue

        while stack and ind <= stack[-1][0] and body.startswith('}'):
            _, ei = stack.pop()
            events[ei]['close_after'] = True

        m = RE_RANGE.match(raw)
        if m:
            pending = (int(m.group(1), 16), int(m.group(2), 16))
            # Los bloques anonimos llevan su rango DENTRO, en la primera linea.
            if stack and events[stack[-1][1]]['range'] is None:
                events[stack[-1][1]]['range'] = pending
                pending = None
            continue

        if body.startswith('// Local variables'):
            in_locals = True
            continue

        if body.startswith('//') or body.startswith('/*') and 'anonymous' not in body:
            continue

        kind = None
        if 'anonymous block' in body:
            kind = 'block'
        elif body.lstrip().startswith(('inline ', 'static inline ')):
            kind = 'inline'

        if kind:
            in_locals = False
            ev = {'kind': kind, 'text': body.rstrip('{').strip(),
                  'range': pending, 'indent': ind, 'locals': []}
            pending = None
            events.append(ev)
            if body.endswith('{') and not body.endswith('{}'):
                stack.append((ind, len(events) - 1))
            continue

        m = RE_LOCAL.match(raw)
        if m:
            decl, reg = m.group(1).strip(), m.group(2)
            target = events[stack[-1][1]]['locals'] if stack else fn_locals
            target.append((decl, reg, pending))
            if pending and stack:
                pass
            pending = None
            in_locals = False
    return header, fn_locals, events


def load_addr_lines():
    out = {}
    with io.open(LINES, encoding='utf-8', errors='replace') as fh:
        for l in fh:
            m = RE_ADDRLINE.match(l)
            if m:
                out[int(m.group(1), 16)] = (m.group(2), int(m.group(3)))
    return out


def unit_asm_path(unit):
    """Acepta el nombre corto de una SourceList (`zFe2`) o la ruta completa de
    una unidad del report (`main/Packages/vp6/.../deblock`). Las bibliotecas de
    middleware no viven bajo SourceLists."""
    u = unit[5:] if unit.startswith('main/') else unit
    if '/' in u or os.sep in u:
        return os.path.join('build', 'GOWE69', 'asm', u.replace('/', os.sep) + '.s')
    return os.path.join(ASM_ROOT, u + '.s')


def scan_unit(unit):
    path = unit_asm_path(unit)
    if not os.path.exists(path):
        sys.exit('no existe %s' % path)
    fns, cur = defaultdict(list), None
    for l in io.open(path, encoding='utf-8', errors='replace'):
        m = RE_FN.match(l)
        if m:
            cur = m.group(1)
            continue
        if l.startswith('.endfn'):
            cur = None
            continue
        if cur:
            m = RE_INSN.match(l)
            if m:
                fns[cur].append((int(m.group(1), 16), m.group(2)))
    return fns


def demangled(unit):
    import json
    p = os.path.join('build', 'GOWE69', 'report.json')
    out = {}
    if not os.path.exists(p):
        return out
    rep = json.load(open(p))
    for u in rep.get('units', []):
        if not u['name'].endswith('/' + unit):
            continue
        for f in u.get('functions') or []:
            out[f['name']] = (f.get('metadata', {}).get('demangled_name', f['name']),
                              f.get('fuzzy_match_percent', 0.0))
    return out


def pick_symbol(fns, names, query):
    """El primer substring que casaba se llevaba la funcion equivocada:
    `iSPCH_ChooseSentence` daba `iSPCH_ChooseSentenceIteratively`. Entre los
    candidatos gana el nombre mas CORTO, que es el que no lleva sufijo.
    """
    if query in fns:
        return query
    exact = [s for s, (dem, _) in names.items()
             if s in fns and dem.split('(')[0].strip().endswith(query)]
    if exact:
        return min(exact, key=lambda s: len(names[s][0]))
    part = [s for s, (dem, _) in names.items() if s in fns and query in dem]
    if part:
        return min(part, key=lambda s: len(names[s][0]))
    raw = [s for s in fns if query in s]
    if raw:
        return min(raw, key=len)
    return None


def show_dwarf(header, fn_locals, events):
    print(header)
    if fn_locals:
        print('  locales de la funcion:')
        for decl, reg, _ in fn_locals:
            print('     %-52s %s' % (decl, reg or '(pila)'))
    for ev in events:
        r = ev['range']
        pad = ' ' * ev['indent']
        rng = '%08X-%08X' % r if r else '        -        '
        print('  %s %s  %s' % (rng, pad, ev['text']))
        for decl, reg, _ in ev['locals']:
            print('  %s %s    · %-44s %s' % (' ' * 17, pad, decl, reg or '(pila)'))


def main():
    if '--index' in sys.argv:
        build_index()
        print('indice reconstruido')
        return
    if len(sys.argv) < 3:
        sys.exit(__doc__)

    unit, query = sys.argv[1], sys.argv[2]
    idx = load_index()

    key = query if query in idx else query.rsplit('::', 1)[-1]
    hits = idx.get(key, [])
    if not hits:
        cands = [k for k in idx if query in k][:10]
        sys.exit('no encuentro %r en el volcado.%s' % (
            query, ('\nParecidos: ' + ', '.join(cands)) if cands else ''))

    fns = scan_unit(unit)
    names = demangled(unit)
    sym = pick_symbol(fns, names, query)
    insns = fns.get(sym) if sym else None

    # De los homonimos hay que quedarse con el que de verdad cae en el rango de
    # ESTA funcion. Coger el primero que encaje daba las locales de otra clase
    # con el mismo nombre de metodo: se puntua cada candidato por cuantos de sus
    # rangos caen dentro y gana el mayor.
    lo, hi = (insns[0][0], insns[-1][0]) if insns else (0, 0)
    best, best_score = None, -1
    for off, _ in hits:
        cand = parse_events(read_fn_block(off))
        score = sum(1 for ev in cand[2] if ev['range'] and lo <= ev['range'][0] <= hi)
        if score > best_score:
            best, best_score = cand, score
    if insns and best_score == 0 and len(hits) > 1:
        sys.stderr.write(
            'aviso: %d homonimos en el volcado y ninguno cae en %08X-%08X; '
            'el bloque mostrado puede ser de otra clase\n' % (len(hits), lo, hi))
    header, fn_locals, events = best

    if '--raw' in sys.argv:
        # El bloque del volcado tal cual, sin interpretar: util cuando el
        # parser se deja algo o para leer una firma completa.
        for off, _ in hits:
            for _, raw in read_fn_block(off):
                print(raw)
            print()
        return

    if '--dwarf-only' in sys.argv or not insns:
        show_dwarf(header, fn_locals, events)
        return

    dem, pct = names.get(sym, (sym, None))
    addr2line = load_addr_lines()

    print('%s' % dem)
    print('%s  |  %d instrucciones%s' % (sym, len(insns),
                                         '' if pct is None else '  |  %.2f%%' % pct))
    if fn_locals:
        print('locales: ' + ', '.join(
            '%s=%s' % (d.split()[-1].lstrip('*&').split('[')[0], r or 'pila')
            for d, r, _ in fn_locals))
    print('-' * 96)

    # Dos cosas que hay que separar:
    #
    #  - El nivel de anidamiento sale de la INDENTACION del volcado, no de contener
    #    rangos. Muchos inline tienen rango degenerado (A -> A) y comparten
    #    direccion, asi que apilarlos por contencion los saca a treinta niveles.
    #  - Un rango degenerado significa "declarado inline pero NO expandido ahi":
    #    se marca con · y no abre ambito. Comprobado: el original emite un `bl`
    #    real en esas direcciones.
    ranged = [ev for ev in events if ev['range']]
    for ev in ranged:
        ev['level'] = max(0, ev['indent'] // 4 - 1)
        ev['degenerate'] = ev['range'][0] == ev['range'][1]

    spans = [ev for ev in ranged if not ev['degenerate']]
    points = defaultdict(list)
    for ev in ranged:
        if ev['degenerate']:
            points[ev['range'][0]].append(ev)

    def active_at(a):
        act = [ev for ev in spans if ev['range'][0] <= a <= ev['range'][1]]
        act.sort(key=lambda e: (e['level'], e['range'][0]))
        return act

    last = None
    prev = []
    for addr, text in insns:
        ent = addr2line.get(addr)
        if ent and ent != last:
            print('\n  --- %s:%d ---' % (ent[0].rsplit('/', 1)[-1], ent[1]))
            last = ent

        cur = active_at(addr)
        common = 0
        while common < len(prev) and common < len(cur) and prev[common] is cur[common]:
            common += 1
        for i in range(len(prev) - 1, common - 1, -1):
            ev = prev[i]
            print('  %s%s %s' % ('  ' * i, '}' if ev['kind'] == 'block' else '<',
                                 ev['text'].split('(')[0]))
        for i in range(common, len(cur)):
            ev = cur[i]
            print('  %s%s %s' % ('  ' * i, '{' if ev['kind'] == 'block' else '>', ev['text']))
            for decl, reg, _ in ev['locals']:
                print('  %s    · %-44s %s' % ('  ' * i, decl, reg or '(pila)'))

        for ev in points.get(addr, []):
            print('  %s· %s' % ('  ' * len(cur), ev['text']))

        print('  %08X  %s%s' % (addr, '  ' * len(cur), text))
        prev = cur

    for i in range(len(prev) - 1, -1, -1):
        ev = prev[i]
        print('  %s%s %s' % ('  ' * i, '}' if ev['kind'] == 'block' else '<',
                             ev['text'].split('(')[0]))


if __name__ == '__main__':
    main()
