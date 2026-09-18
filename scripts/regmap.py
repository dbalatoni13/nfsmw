#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""regmap.py - enfrenta local a local el reparto de registros del ORIGINAL contra el NUESTRO.

El fallo mas caro del proyecto es la funcion que se queda entre el 97% y el 99,9%
con un diff que solo cambia dos registros (f27<->f28, r26<->r27). Se suele echar la
culpa al asignador, pero muchas veces es estructura: una local que sobra, una que
falta, o una que vive en un ambito distinto.

symbols/mw_dwarfdump.nothpp trae, por funcion, las locales del ORIGINAL con el
registro que les asigno el compilador de EA. Y "dtk dwarf dump" sobre nuestro .o
produce EXACTAMENTE el mismo formato para lo nuestro (los cflags ya llevan
-gdwarf+). Esto empareja los dos y nombra la diferencia.

  python scripts/regmap.py zSim "OBB::SphereVsBox"          # una funcion
  python scripts/regmap.py zAI "AICopManager::UpdatePursuits" --all --build
  python scripts/regmap.py zCamera --scan --min 95 --minsize 100   # triaje
  python scripts/regmap.py zSim --list OBB::                # que funciones hay
  python scripts/regmap.py zCamera "X::Y" --obj /tmp/prueba.o  # probar un .o suelto

Salida: tabla local-a-local, ciclos/permutaciones de registros con nombre, y el
veredicto. Cuatro veredictos posibles:

  IDENTICO     mismas locales, mismos registros. Si sigue habiendo diff NO es una
               local: son constantes/literales/saltos, o TEMPORALES del compilador
               (ranuras de marco por encima de la ultima local; no salen en DWARF).
  PERMUTACION  mismas locales, mismo arbol de bloques, y los registros forman
               ciclo(s) cerrado(s). Aqui si tiene sentido pelear con el asignador.
  REPARTO      mismas locales pero los registros se mueven sin cerrar ciclo:
               algo mas cambio (un temporal de mas, otra clase de registro).
  ESTRUCTURA   falta/sobra una local, o vive en otro ambito, o el arbol de bloques
               anonimos no cuadra. Eso va PRIMERO: los registros se recolocan solos.

Nota: --scan cruza con build/GOWE69/report.json por prefijo mangled; con funciones
sobrecargadas puede coger la hermana equivocada (el % seria orientativo, el
veredicto no depende de el).
"""
import argparse
import os
import pickle
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))

ORIG_DUMP = os.path.join(ROOT, 'symbols', 'mw_dwarfdump.nothpp')
DTK = os.path.join(ROOT, 'build', 'tools', 'dtk.exe')
CACHE = os.environ.get('REGMAP_CACHE') or os.path.join(ROOT, 'build', 'regmap')


def _mkcache():
    os.makedirs(CACHE, exist_ok=True)
    return CACHE


RE_OPEN = re.compile(r'^(?P<sig>.*\))\s*(?:const\s*)?(?:volatile\s*)?(?:override\s*)?\{$')
RE_RANGE = re.compile(r'^//\s*Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)\s*$')
RE_DECL = re.compile(r'^(?P<decl>.*?);\s*(?://\s*(?P<loc>.*))?$')
RE_LOC_REG = re.compile(r'^([rf]\d+|ctr|lr)$')


def func_name_of(sig):
    """Saca Clase::Metodo de una firma completa."""
    depth = 0
    open_at = None
    for i in range(len(sig) - 1, -1, -1):
        c = sig[i]
        if c == ')':
            depth += 1
        elif c == '(':
            depth -= 1
            if depth == 0:
                open_at = i
                break
    if open_at is None:
        return None
    prefix = sig[:open_at].rstrip()
    if prefix.endswith('[]'):                # operator new []
        prefix = prefix[:-2].rstrip()
    lo = prefix.rfind('operator')
    if lo >= 0 and (lo == 0 or not (prefix[lo - 1].isalnum() or prefix[lo - 1] == '_')):
        return prefix[lo:].strip()
    depth = 0
    for i in range(len(prefix) - 1, -1, -1):
        c = prefix[i]
        if c == '>':
            depth += 1
        elif c == '<':
            depth -= 1
        elif depth == 0 and (c in ' \t*&'):
            return prefix[i + 1:].strip()
    return prefix.strip()


# ---------------------------------------------------------------- indice

def build_index(path, verbose=False):
    """-> {nombre: [(compile_unit, byte_offset, linea)]}. Se cachea por mtime."""
    _mkcache()
    st = os.stat(path)
    key = os.path.join(CACHE, 'idx_%s_%d_%d.pkl' % (
        os.path.basename(path), st.st_size, int(st.st_mtime)))
    if os.path.exists(key):
        try:
            with open(key, 'rb') as f:
                return pickle.load(f)
        except Exception:
            pass
    if verbose:
        sys.stderr.write('indexando %s (una sola vez)...\n' % os.path.basename(path))
    index = {}
    cu = '?'
    off = 0
    range_off = None           # el "// Range:" que precede a la firma
    with open(path, 'rb') as f:
        for raw in f:
            n = len(raw)
            line = raw.decode('utf-8', 'replace').rstrip('\r\n')
            if line.startswith('    Compile unit: '):
                cu = os.path.basename(line.split(': ', 1)[1])
                if cu.endswith('.cpp'):
                    cu = cu[:-4]
            elif line.startswith('// Range:'):
                range_off = off
                off += n
                continue
            elif line and line[0] not in ' \t}/#' and line.endswith('{'):
                m = RE_OPEN.match(line)
                if m:
                    nm = func_name_of(m.group('sig'))
                    if nm:
                        index.setdefault(nm, []).append(
                            (cu, off if range_off is None else range_off, line))
            range_off = None
            off += n
    try:
        with open(key, 'wb') as f:
            pickle.dump(index, f, 2)
    except Exception:
        pass
    return index


def read_block_at(path, offset):
    """Lee desde offset la definicion completa (hasta el } a columna 0)."""
    out = []
    with open(path, 'rb') as f:
        f.seek(offset)
        for raw in f:
            line = raw.decode('utf-8', 'replace').rstrip('\r\n')
            out.append(line)
            if len(out) > 1 and line == '}':
                break
            if len(out) > 200000:
                break
    return out


# ---------------------------------------------------------------- nuestro volcado

def our_dump(unit, do_build=False, verbose=False, obj=None):
    import build_direct as bd
    cwd = os.getcwd()
    os.chdir(ROOT)
    try:
        units = bd.parse_units()
        if unit not in units:
            raise SystemExit('unidad desconocida: %s (hay %d)' % (unit, len(units)))
        spec = units[unit]
        if obj is None:
            obj = os.path.join(ROOT, spec[1])
        elif not os.path.isabs(obj):
            obj = os.path.join(ROOT, obj)
        if do_build or not os.path.exists(obj):
            if verbose:
                sys.stderr.write('compilando %s...\n' % unit)
            name, rc, err = bd.compile_one(unit, spec, ROOT)
            if rc != 0:
                raise SystemExit('fallo compilando %s:\n%s' % (unit, err[:2000]))
    finally:
        os.chdir(cwd)
    _mkcache()
    st = os.stat(obj)
    tag = unit if obj.endswith(os.path.basename(spec[1])) else         unit + '-' + os.path.basename(obj)[:-2]
    out = os.path.join(CACHE, 'our_%s_%d_%d.nothpp' % (tag, st.st_size, int(st.st_mtime)))
    if not os.path.exists(out):
        for stale in os.listdir(CACHE):
            if stale.startswith('our_%s_' % tag) or stale.startswith('idx_our_%s_' % tag):
                try:
                    os.remove(os.path.join(CACHE, stale))
                except OSError:
                    pass
        if verbose:
            sys.stderr.write('dtk dwarf dump %s...\n' % unit)
        p = subprocess.run([DTK, 'dwarf', 'dump', obj, '-o', out, '--no-color'],
                           cwd=ROOT, capture_output=True, text=True)
        if not os.path.exists(out):
            raise SystemExit('dtk dwarf dump fallo:\n%s' % ((p.stderr or p.stdout)[:2000]))
        errs = [l for l in (p.stderr or '').split('\n') if 'ERROR' in l]
        if errs and verbose:
            sys.stderr.write('  (dtk aviso: %d tag(s) no procesados)\n' % len(errs))
    return out


# ---------------------------------------------------------------- parser de ambitos

class Scope(object):
    __slots__ = ('kind', 'title', 'lo', 'hi', 'vars', 'kids')

    def __init__(self, kind, title):
        self.kind = kind          # fn | block | inline
        self.title = title
        self.lo = self.hi = None
        self.vars = []            # [(tipo, nombre, loc)]
        self.kids = []


def split_decl(decl):
    """float dists[3] -> ('float[3]', 'dists')."""
    d = decl.strip()
    arr = ''
    m = re.search(r'(\[[^\]]*\](?:\s*\[[^\]]*\])*)$', d)
    if m:
        arr = re.sub(r'\s+', '', m.group(1))
        d = d[:m.start()].rstrip()
    m = re.search(r'([A-Za-z_$][A-Za-z_0-9$]*)$', d)
    if not m:
        return ('', '')
    name = m.group(1)
    typ = d[:m.start()].strip()
    if not typ:
        return ('', name)
    typ = re.sub(r'\s+', ' ', typ) + arr
    return (typ, name)


def parse_scopes(lines):
    """Parsea el bloque de una funcion en un arbol de ambitos."""
    root = None
    stack = []
    pending = None
    for line in lines:
        s = line.strip()
        if not s:
            continue
        m = RE_RANGE.match(s)
        if m:
            pending = (int(m.group(1), 16), int(m.group(2), 16))
            continue
        if s.startswith('//'):
            continue
        if s in ('}', '};'):
            if stack:
                stack.pop()
            continue
        if s.endswith('{}'):          # inline vacia: no abre ambito
            pending = None
            continue
        if s.endswith('{'):
            body = s[:-1].strip()
            if body == '/* anonymous block */':
                kind, title = 'block', 'block'
            elif root is None:
                kind, title = 'fn', body
            else:
                kind, title = 'inline', body
            sc = Scope(kind, title)
            if pending:
                sc.lo, sc.hi = pending
            pending = None
            if root is None:
                root = sc
            elif stack:
                stack[-1].kids.append(sc)
            stack.append(sc)
            continue
        if ';' in s:
            m = RE_DECL.match(s)
            if m and stack:
                typ, name = split_decl(m.group('decl'))
                if typ:
                    loc = (m.group('loc') or '').strip()
                    if 'offset 0x' in loc or loc.startswith('size:'):
                        loc = ''
                    stack[-1].vars.append((typ, name, loc))
            continue
    return root


def parse_params(sig):
    """-> [(tipo, nombre, loc)] de la lista de parametros."""
    depth = 0
    open_at = close_at = None
    for i in range(len(sig) - 1, -1, -1):
        c = sig[i]
        if c == ')':
            if depth == 0:
                close_at = i
            depth += 1
        elif c == '(':
            depth -= 1
            if depth == 0:
                open_at = i
                break
    if open_at is None:
        return []
    inner = sig[open_at + 1:close_at]
    parts, depth, cur = [], 0, ''
    for c in inner:
        if c in '(<[':
            depth += 1
        elif c in ')>]':
            depth -= 1
        if c == ',' and depth == 0:
            parts.append(cur)
            cur = ''
        else:
            cur += c
    if cur.strip():
        parts.append(cur)
    out = []
    for p in parts:
        p = p.strip()
        if not p or p in ('void', '...'):
            continue
        loc = ''
        m = re.search(r'/\*\s*([^*]+?)\s*\*/', p)
        if m:
            loc = m.group(1).strip()
            p = (p[:m.start()] + p[m.end():]).strip()
        typ, name = split_decl(p)
        if not typ:
            typ, name = p, ''
        out.append((typ, name, loc))
    return out


# ---------------------------------------------------------------- emparejado

def norm_type(t):
    t = re.sub(r'\b(const|volatile|struct|class|union|enum)\b', '', t)
    t = re.sub(r'\s+', '', t)
    t = t.replace('&', '*')          # una referencia y un puntero valen lo mismo aqui
    return t


def loc_class(loc):
    """'f' registro flotante, 'g' registro entero, 'm' marco, '' sin sitio."""
    if not loc:
        return ''
    if RE_LOC_REG.match(loc):
        return 'f' if loc[0] == 'f' else 'g'
    if re.match(r'^r\d+\s*[+-]', loc):
        return 'm'
    return 'o'


def pair_vars(a, b):
    """Empareja por nombre, luego por tipo+orden, luego por posicion (solo si el
    tipo o la clase de sitio son compatibles: un int en r31 no es un float en f31)."""
    ua, ub = set(range(len(a))), set(range(len(b)))
    pairs = []
    names_a, names_b = {}, {}
    for i, v in enumerate(a):
        names_a.setdefault(v[1], []).append(i)
    for j, v in enumerate(b):
        names_b.setdefault(v[1], []).append(j)
    for nm, ia in names_a.items():
        jb = names_b.get(nm)
        if nm and jb and len(ia) == 1 and len(jb) == 1:
            pairs.append((ia[0], jb[0]))
            ua.discard(ia[0])
            ub.discard(jb[0])
    by_t = {}
    for j in sorted(ub):
        by_t.setdefault(norm_type(b[j][0]), []).append(j)
    for i in sorted(ua):
        cand = by_t.get(norm_type(a[i][0]))
        if cand:
            j = cand.pop(0)
            pairs.append((i, j))
            ua.discard(i)
            ub.discard(j)
    rb = sorted(ub)
    for i in sorted(ua):
        for j in list(rb):
            if loc_class(a[i][2]) == loc_class(b[j][2]):
                pairs.append((i, j))
                ua.discard(i)
                rb.remove(j)
                ub.discard(j)
                break
    for i in sorted(ua):
        pairs.append((i, None))
    for j in sorted(ub):
        pairs.append((None, j))
    pairs.sort(key=lambda p: (p[0] if p[0] is not None else 10000,
                              p[1] if p[1] is not None else 10000))
    return pairs


def blocks_only(scope):
    return [s for s in scope.kids if s.kind == 'block']


def walk_blocks(scope, prefix, out):
    for k, kid in enumerate(blocks_only(scope)):
        p = (prefix + '/' if prefix else '') + 'b%d' % k
        out.append((p, kid))
        walk_blocks(kid, p, out)


def rel(sc, base):
    if sc.lo is None or base is None:
        return None
    return (sc.lo - base, sc.hi - base)


def block_score(sa, sb, oo, no):
    """Cuanto se parecen dos bloques anonimos: solapamiento de rango + locales."""
    s = 0.0
    ra, rb = rel(sa, oo), rel(sb, no)
    if ra and rb:
        inter = max(0, min(ra[1], rb[1]) - max(ra[0], rb[0]))
        union = max(ra[1], rb[1]) - min(ra[0], rb[0])
        if union > 0:
            s += 2.0 * inter / union
    na = set(v[1] for v in sa.vars)
    nb = set(v[1] for v in sb.vars)
    if na and nb:
        s += 1.5 * len(na & nb) / float(max(len(na), len(nb)))
    ta = [norm_type(v[0]) for v in sa.vars]
    tb = [norm_type(v[0]) for v in sb.vars]
    if ta and tb:
        s += 0.5 * len(set(ta) & set(tb)) / float(max(len(ta), len(tb)))
    return s


GAP = 0.55


def align_children(ba, bb, oo, no):
    """Alineamiento que respeta el orden (Needleman-Wunsch) sobre bloques anonimos.
    -> [(ia|None, ib|None)]"""
    n, m = len(ba), len(bb)
    if not n or not m:
        return [(i, None) for i in range(n)] + [(None, j) for j in range(m)]
    D = [[0.0] * (m + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        D[i][0] = D[i - 1][0] - GAP
    for j in range(1, m + 1):
        D[0][j] = D[0][j - 1] - GAP
    for i in range(1, n + 1):
        for j in range(1, m + 1):
            D[i][j] = max(D[i - 1][j - 1] + block_score(ba[i - 1], bb[j - 1], oo, no),
                          D[i - 1][j] - GAP, D[i][j - 1] - GAP)
    out = []
    i, j = n, m
    while i > 0 or j > 0:
        if i > 0 and j > 0 and abs(D[i][j] - (D[i - 1][j - 1]
                                              + block_score(ba[i - 1], bb[j - 1], oo, no))) < 1e-9:
            out.append((i - 1, j - 1))
            i -= 1
            j -= 1
        elif i > 0 and abs(D[i][j] - (D[i - 1][j] - GAP)) < 1e-9:
            out.append((i - 1, None))
            i -= 1
        else:
            out.append((None, j - 1))
            j -= 1
    out.reverse()
    return out


def align_scopes(oa, ob, path, rows, notes, oo, no, ranges=None):
    ba, bb = blocks_only(oa), blocks_only(ob)
    if len(ba) != len(bb):
        notes.append('%s: el original abre %d bloque(s) anonimo(s) y nosotros %d'
                     % (path or 'nivel de funcion', len(ba), len(bb)))
    for k, (i, j) in enumerate(align_children(ba, bb, oo, no)):
        sub = (path + '/' if path else '') + 'b%d' % k
        if i is None:
            notes.append('el bloque %s SOBRA en el nuestro (declara: %s)'
                         % (sub, ', '.join(v[1] for v in bb[j].vars) or 'nada'))
            for v in bb[j].vars:
                rows.append((sub, None, v))
            extra = []
            walk_blocks(bb[j], sub, extra)
            for p, sc in extra:
                for v in sc.vars:
                    rows.append((p, None, v))
            continue
        if j is None:
            notes.append('el bloque %s FALTA en el nuestro (el original declara: %s)'
                         % (sub, ', '.join(v[1] for v in ba[i].vars) or 'nada'))
            for v in ba[i].vars:
                rows.append((sub, v, None))
            extra = []
            walk_blocks(ba[i], sub, extra)
            for p, sc in extra:
                for v in sc.vars:
                    rows.append((p, v, None))
            continue
        A, B = ba[i], bb[j]
        if ranges is not None:
            ra, rb = rel(A, oo), rel(B, no)
            if ra and rb:
                ranges.append((sub, ra, rb))
        for (x, y) in pair_vars(A.vars, B.vars):
            rows.append((sub, A.vars[x] if x is not None else None,
                         B.vars[y] if y is not None else None))
        align_scopes(A, B, sub, rows, notes, oo, no, ranges)


# ---------------------------------------------------------------- ciclos

def find_cycles(mapping):
    cycles, seen = [], set()
    for start in mapping:
        if start in seen:
            continue
        chain, cur = [], start
        while cur in mapping and cur not in chain:
            chain.append(cur)
            cur = mapping[cur]
        if cur in chain:
            c = chain[chain.index(cur):]
            if len(c) > 1 and frozenset(c) not in [frozenset(x) for x in cycles]:
                cycles.append(c)
        seen |= set(chain)
    return cycles


def is_reg(loc):
    return bool(loc) and bool(RE_LOC_REG.match(loc))


# ---------------------------------------------------------------- informe

def analyze(o_sig, o_lines, n_sig, n_lines):
    """-> dict con rows/notes/ranges/moved y las raices, o None si no parsea."""
    o_root = parse_scopes(o_lines)
    n_root = parse_scopes(n_lines)
    if o_root is None or n_root is None:
        return None
    o_par, n_par = parse_params(o_sig), parse_params(n_sig)

    rows, notes, ranges = [], [], []
    oo, no = o_root.lo, n_root.lo
    for (i, j) in pair_vars(o_par, n_par):
        rows.append(('param', o_par[i] if i is not None else None,
                     n_par[j] if j is not None else None))
    for (i, j) in pair_vars(o_root.vars, n_root.vars):
        rows.append(('fn', o_root.vars[i] if i is not None else None,
                     n_root.vars[j] if j is not None else None))
    align_scopes(o_root, n_root, '', rows, notes, oo, no, ranges)

    # --- reconciliacion entre ambitos: la misma local pero a otra profundidad
    moved = []
    only_o = [(k, r) for k, r in enumerate(rows) if r[1] and not r[2]]
    only_n = [(k, r) for k, r in enumerate(rows) if r[2] and not r[1]]
    used = set()
    for ko, ro in only_o:
        best = None
        for kn, rn in only_n:
            if kn in used:
                continue
            sc = 0
            if ro[1][1] == rn[2][1]:
                sc += 3
            if norm_type(ro[1][0]) == norm_type(rn[2][0]):
                sc += 2
            if ro[1][2] and ro[1][2] == rn[2][2]:
                sc += 2
            elif loc_class(ro[1][2]) == loc_class(rn[2][2]):
                sc += 1
            if sc >= 3 and (best is None or sc > best[0]):
                best = (sc, kn, rn)
        if best:
            used.add(best[1])
            moved.append((ro[0], ro[1], best[2][0], best[2][2]))
            rows[ko] = None
            rows[best[1]] = None
    rows = [r for r in rows if r is not None]
    return dict(rows=rows, notes=notes, ranges=ranges, moved=moved,
                o_root=o_root, n_root=n_root)


def tally(a):
    """Cuenta rapida sin imprimir: -> (ok, distintos, solo_orig, solo_nuestras)."""
    ok = diff = only_o = only_n = 0
    mapping = {}
    for (sc, vo, vn) in a['rows']:
        if vo and vn:
            lo, ln = vo[2] or '-', vn[2] or '-'
            if lo == ln:
                ok += 1
            else:
                diff += 1
                if is_reg(lo) and is_reg(ln):
                    mapping[lo] = ln
        elif vo:
            only_o += 1
        else:
            only_n += 1
    return ok, diff, only_o, only_n, mapping


def verdict_of(a):
    """-> (etiqueta, detalle) en una linea."""
    ok, diff, only_o, only_n, mapping = tally(a)
    moved, notes = a['moved'], a['notes']
    if not only_o and not only_n and not notes and not moved:
        if diff == 0:
            return 'IDENTICO', 'mismas locales y mismo reparto (si hay diff: temporales/constantes)'
        cyc = find_cycles(mapping)
        if cyc and sum(len(c) for c in cyc) == len(mapping):
            return 'PERMUTACION', 'ciclo(s) %s' % ' + '.join(
                '<->'.join(c) if len(c) == 2 else '->'.join(c + [c[0]]) for c in cyc)
        return 'REPARTO', '%d registro(s) movidos, sin ciclo cerrado' % diff
    piezas = []
    if only_o:
        piezas.append('%d local(es) solo del original' % only_o)
    if only_n:
        piezas.append('%d local(es) solo nuestras' % only_n)
    if moved:
        piezas.append('%d de ambito' % len(moved))
    if notes:
        piezas.append('%d desajuste(s) de bloques' % len(notes))
    return 'ESTRUCTURA', ', '.join(piezas)


def report(unit, query, o_sig, o_lines, n_sig, n_lines, show_all):
    a = analyze(o_sig, o_lines, n_sig, n_lines)
    if a is None:
        print('no he podido parsear uno de los dos bloques')
        return
    rows, notes, ranges, moved = a['rows'], a['notes'], a['ranges'], a['moved']
    o_root, n_root = a['o_root'], a['n_root']
    oo, no = o_root.lo, n_root.lo

    print('=' * 78)
    print('  %s   [%s]' % (query, unit))
    print('  original: %s' % o_sig)
    print('  nuestro : %s' % n_sig)
    print('=' * 78)
    print()
    print('  %-11s %-30s %-9s %-9s' % ('ambito', 'local', 'original', 'nuestro'))
    print('  %-11s %-30s %-9s %-9s' % ('-' * 11, '-' * 30, '-' * 9, '-' * 9))

    mapping = {}
    names = {}
    n_ok = n_diff = n_only_o = n_only_n = 0
    for (sc, vo, vn) in rows:
        if vo and vn:
            lo, ln = vo[2] or '-', vn[2] or '-'
            if is_reg(lo):
                names.setdefault(lo, vo[1])
            nm = vo[1] if vo[1] == vn[1] else '%s / %s' % (vo[1], vn[1])
            if lo == ln:
                n_ok += 1
                if not show_all:
                    continue
                tag = 'ok'
            else:
                n_diff += 1
                tag = '<-- DISTINTO'
                if is_reg(lo) and is_reg(ln):
                    mapping[lo] = ln
            print('  %-11s %-30s %-9s %-9s %s' % (sc, nm[:30], lo, ln, tag))
        elif vo:
            n_only_o += 1
            print('  %-11s %-30s %-9s %-9s %s'
                  % (sc, vo[1][:30], vo[2] or '-', '', '<-- SOLO EN EL ORIGINAL (%s)' % vo[0]))
        else:
            n_only_n += 1
            print('  %-11s %-30s %-9s %-9s %s'
                  % (sc, vn[1][:30], '', vn[2] or '-', '<-- SOLO NUESTRA (%s)' % vn[0]))
    if not show_all and n_ok:
        print('  ... y %d local(es) con el mismo registro (--all para verlas)' % n_ok)
    print()

    if moved:
        print('  AMBITO EQUIVOCADO (la local existe en los dos, pero a otra profundidad)')
        for (so, vo, sn, vn) in moved:
            nm = vo[1] if vo[1] == vn[1] else '%s / %s' % (vo[1], vn[1])
            print('    * "%s": el original la declara en %-11s (%s), nosotros en %-11s (%s)'
                  % (nm, so, vo[2] or 'sin sitio', sn, vn[2] or 'sin sitio'))
            if vo[2] and vn[2] and vo[2] != vn[2] and is_reg(vo[2]) and is_reg(vn[2]):
                mapping.setdefault(vo[2], vn[2])
                names.setdefault(vo[2], vo[1])
        print()

    cycles = find_cycles(mapping)
    if cycles:
        print('  PERMUTACIONES')
        for c in cycles:
            if len(c) == 2:
                print('    * %s y %s estan INTERCAMBIADOS: el original asigna %s a "%s"'
                      % (c[0], c[1], c[0], names.get(c[0], '?')))
                print('      y %s a "%s"' % (c[1], names.get(c[1], '?')))
            else:
                print('    * ciclo de %d: %s -> %s'
                      % (len(c), ' -> '.join(c), c[0]))
                print('      en el original: %s'
                      % ', '.join('%s="%s"' % (r, names.get(r, '?')) for r in c))
        print()
    incyc = set()
    for c in cycles:
        incyc |= set(c)
    lone = [(k, v) for k, v in mapping.items() if k not in incyc]
    if lone:
        print('  DESPLAZAMIENTOS SIN CICLO (no es permutacion pura)')
        for k, v in sorted(lone):
            print('    * "%s": original %s -> nuestro %s' % (names.get(k, '?'), k, v))
        print()

    if notes:
        print('  ESTRUCTURA DE AMBITOS')
        for x in notes:
            print('    * %s' % x)
        print()

    if oo is not None and no is not None:
        bad = [(p, ra, rb) for (p, ra, rb) in ranges if ra != rb]
        so, sn = o_root.hi - oo, n_root.hi - no
        print('  TAMANO: original 0x%X (%d B), nuestro 0x%X (%d B)  -> %s'
              % (so, so, sn, sn, 'igual' if so == sn else 'DISTINTO (%+d B)' % (sn - so)))
        if bad:
            print('  RANGOS DE BLOQUE que no cuadran (offset relativo al inicio):')
            for p, ra, rb in bad[:12]:
                print('    * %-10s original 0x%X..0x%X   nuestro 0x%X..0x%X'
                      % (p, ra[0], ra[1], rb[0], rb[1]))
        print()

    print('  VEREDICTO')
    same_set = (n_only_o == 0 and n_only_n == 0 and not notes and not moved)
    if same_set and n_diff == 0:
        print('    MISMO conjunto de locales, mismo arbol de bloques y MISMO REPARTO.')
        print('    Si aun asi hay diff, NO es una local: o son constantes/literales/saltos,')
        print('    o son TEMPORALES del compilador (las ranuras de marco por encima de la')
        print('    ultima local declarada). Los temporales no salen en el DWARF; se mueven')
        print('    cambiando cuantos resultados por valor se materializan y en que orden.')
    elif same_set:
        print('    MISMO conjunto de locales y mismo arbol de bloques; solo cambia el')
        print('    reparto en %d. Candidato REAL a permutacion del asignador: juega con' % n_diff)
        print('    el ORDEN DE DECLARACION y con el rango de vida de las de arriba.')
    else:
        print('    NO es una permutacion: hay %d local(es) solo en el original, %d solo'
              % (n_only_o, n_only_n))
        print('    nuestras, %d en el ambito equivocado y %d desajuste(s) del arbol de'
              % (len(moved), len(notes)))
        print('    bloques. ESO VA PRIMERO; los registros se recolocan solos.')
    print('    (%d iguales, %d con registro distinto, %d de ambito equivocado, %d solo'
          % (n_ok, n_diff, len(moved), n_only_o))
    print('     del original, %d solo nuestras)' % n_only_n)


# ---------------------------------------------------------------- triaje en lote

def mangle_prefix(name):
    """Clase::Metodo -> prefijo mangled de GCC 2.9 (Metodo__<n><Clase>)."""
    if '::' not in name:
        return name + '__F'
    cls, meth = name.rsplit('::', 1)
    if meth == cls:
        meth = '__ct'
    elif meth == '~' + cls:
        meth = '__dt'
    return '%s__%d%s' % (meth, len(cls), cls)


def load_report(path):
    """-> {unidad: {mangled: (pct, size)}}"""
    import json
    if not os.path.exists(path):
        return {}
    with open(path, encoding='utf-8') as f:
        d = json.load(f)
    out = {}
    for u in d.get('units') or []:
        nm = (u.get('name') or '').replace('\\', '/').split('/')[-1]
        m = {}
        for fn in u.get('functions') or []:
            m[fn.get('name', '')] = (float(fn.get('fuzzy_match_percent') or 0),
                                     int(fn.get('size') or 0))
        out[nm] = m
    return out


def scan(unit, lo, hi, obj, do_build, verbose, min_size):
    """Recorre todas las funciones de la unidad y da un veredicto por linea."""
    rep = load_report(os.path.join(ROOT, 'build', 'GOWE69', 'report.json')).get(unit, {})
    oidx = build_index(ORIG_DUMP, verbose)
    ndump = our_dump(unit, do_build, verbose, obj)
    nidx = build_index(ndump, verbose)
    cand = []
    for nm, ours in nidx.items():
        o = [c for c in (oidx.get(nm) or []) if c[0] == unit]
        if not o:
            continue
        pct, size = None, None
        if rep:
            pre = mangle_prefix(nm)
            for mn, (p, s) in rep.items():
                if mn.startswith(pre):
                    pct, size = p, s
                    break
            if pct is None:
                continue
            if not (lo <= pct < hi) or size < min_size:
                continue
        cand.append((-(size or 0), nm, o[0], ours[0], pct, size))
    cand.sort()
    print('%-9s %-46s %5s %6s  %s' % ('veredicto', 'funcion', 'B', '%', 'detalle'))
    print('-' * 118)
    tot = {}
    for _, nm, o, n, pct, size in cand:
        try:
            a = analyze(RE_OPEN.match(o[2]).group('sig'), read_block_at(ORIG_DUMP, o[1]),
                        RE_OPEN.match(n[2]).group('sig'), read_block_at(ndump, n[1]))
        except Exception as e:
            print('%-9s %-46s  (error: %s)' % ('?', nm[:46], e))
            continue
        if a is None:
            continue
        lab, det = verdict_of(a)
        tot[lab] = tot.get(lab, 0) + 1
        print('%-9s %-46s %5s %6s  %s'
              % (lab, nm[:46], size if size else '', '%.2f' % pct if pct else '', det[:60]))
    print('-' * 118)
    print('total: ' + ', '.join('%s=%d' % kv for kv in sorted(tot.items())))
    return 0


# ---------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('unit')
    ap.add_argument('func', nargs='?')
    ap.add_argument('--build', action='store_true', help='recompila la unidad antes')
    ap.add_argument('--all', action='store_true', help='muestra tambien las que casan')
    ap.add_argument('--list', metavar='PREFIJO', nargs='?', const='',
                    help='lista las funciones de la unidad')
    ap.add_argument('--ours', action='store_true',
                    help='con --list, lista las de NUESTRO volcado')
    ap.add_argument('--obj', help='usa este .o en vez del de la unidad')
    ap.add_argument('--raw', action='store_true', help='vuelca los dos bloques crudos')
    ap.add_argument('--scan', action='store_true',
                    help='triaje: un veredicto por linea para toda la unidad')
    ap.add_argument('--min', type=float, default=90.0, help='con --scan, %% minimo')
    ap.add_argument('--max', type=float, default=100.0, help='con --scan, %% maximo')
    ap.add_argument('--minsize', type=int, default=0, help='con --scan, bytes minimos')
    ap.add_argument('-q', '--quiet', action='store_true')
    a = ap.parse_args()
    verbose = not a.quiet

    if a.scan:
        return scan(a.unit, a.min, a.max, a.obj, a.build, verbose, a.minsize)

    if a.list is not None:
        if a.ours:
            idx = build_index(our_dump(a.unit, a.build, verbose, a.obj), verbose)
            hits = sorted(nm for nm in idx if a.list in nm)
        else:
            idx = build_index(ORIG_DUMP, verbose)
            hits = sorted(nm for nm, v in idx.items()
                          if any(cu == a.unit for cu, _, _ in v) and a.list in nm)
        for h in hits:
            print(h)
        print('# %d funciones' % len(hits))
        return 0

    if not a.func:
        ap.error('falta el nombre de la funcion (o usa --list)')

    index = build_index(ORIG_DUMP, verbose)
    cands = index.get(a.func)
    if not cands:
        near = sorted(nm for nm in index if a.func in nm)[:15]
        print('no encuentro "%s" en el volcado original.' % a.func)
        if near:
            print('parecidas: %s' % ', '.join(near))
        return 1
    same = [c for c in cands if c[0] == a.unit]
    if not same:
        print('aviso: "%s" no esta en la unidad %s del original; esta en: %s'
              % (a.func, a.unit, ', '.join(sorted(set(c[0] for c in cands)))))
        same = cands
    o_cu, o_off, o_sig_line = same[0]
    o_lines = read_block_at(ORIG_DUMP, o_off)
    o_sig = RE_OPEN.match(o_sig_line).group('sig')

    ndump = our_dump(a.unit, a.build, verbose, a.obj)
    nindex = build_index(ndump, verbose)
    ncands = nindex.get(a.func)
    if not ncands:
        near = sorted(nm for nm in nindex if a.func.split('::')[-1] in nm)[:10]
        print('la funcion NO aparece en nuestro volcado DWARF de %s.' % a.unit)
        print('  o no esta escrita, o se inlineo, o el nombre cambio.')
        if near:
            print('  parecidas: %s' % ', '.join(near))
        return 1
    n_lines = read_block_at(ndump, ncands[0][1])
    n_sig = RE_OPEN.match(ncands[0][2]).group('sig')

    if a.raw:
        print('----- ORIGINAL -----')
        print('\n'.join(o_lines))
        print('----- NUESTRO  -----')
        print('\n'.join(n_lines))
        print()

    report(a.unit, a.func, o_sig, o_lines, n_sig, n_lines, a.all)
    return 0


if __name__ == '__main__':
    sys.exit(main())
