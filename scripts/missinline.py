#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
missinline.py - censo de inlines que el ORIGINAL expande y nosotros no tenemos.

El volcado DWARF (symbols/mw_dwarfdump.nothpp) contiene, por cada funcion del
original, el arbol completo de expansiones inline con rangos de direccion.
Esta herramienta lo parsea, cruza cada inline con nuestro arbol de fuentes y
con build/GOWE69/report.json, y ordena los inlines AUSENTES por impacto:
cuantas funciones los expanden y cuantas de esas siguen por debajo del 100%.

Uso
---
    python scripts/missinline.py rank [--top 40] [--unit zAI] [--all] [--dead]
        Ranking de inlines ausentes por impacto. Columnas:
          exp   veces que el original lo expande
          fn    funciones distintas que lo expanden
          sin   de esas, cuantas siguen por debajo del 100%  <-- ordena por aqui
          bytes suma del tamano de esas funciones sin casar

    python scripts/missinline.py who Class::Metodo
        Que funciones lo expanden, con su % y su tamano. Es la lista de
        trabajo que desbloquea reponer ese inline.

    python scripts/missinline.py func Class::Metodo
        Arbol de expansiones inline de UNA funcion del original, anidado y
        con direcciones, marcando las que nos faltan. Es una receta de
        decompilacion: dice que llamadas hizo el original y en que orden.

    python scripts/missinline.py unit zDynamics
        Los inlines ausentes de una unidad, ordenados.

    python scripts/missinline.py index | scan     # rehacen el cache a mano

Clasificacion (columna "estado")
--------------------------------
    PRESENT_INLINE  lo tenemos con cuerpo en la clase -> GCC 2.9 lo expande.
    VACIO           lo tenemos en la clase pero con cuerpo `{}`: GCC lo expande
                    y expande NADA. El llamante se queda sin la carga que el
                    original si hace, asi que NINGUNA funcion que lo use puede
                    casar. Hay 597 en el arbol; antes contaban como presentes.
                    Aviso: algunos son correctos (`bPlatEndianSwap(uint8 *)`
                    intercambia un solo byte, que es una no-operacion), asi que
                    se rellenan de uno en uno y se miden.
    NOT_INLINE      lo tenemos declarado en la clase y definido fuera SIN
                    `inline`: emitimos una llamada donde el original expandia.
                    Alta confianza, arreglo mecanico.
    MISSING         la clase existe en src/ pero el metodo no. Hay que
                    escribirlo; `func` da la forma del cuerpo.
    NO_CLASS        ni la clase esta en src/.
    MACRO           lo genera una macro; revisar a mano.
    PRESENT_MACRO   lo genera una macro CON cuerpo -> presente, se oculta.

Por defecto se ocultan los operadores del lenguaje (new/delete/=/[]), lo que
ya esta presente, y las unidades con 0% casado (--dead las incluye).

`index` y `scan` se ejecutan solos la primera vez (y con --refresh).
El cache vive en build/missinline/; borrarlo lo regenera (~60 s).
"""
from __future__ import print_function

import argparse
import json
import os
import re
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DUMP = os.path.join(ROOT, 'symbols', 'mw_dwarfdump.nothpp')
SRC = os.path.join(ROOT, 'src')
CACHE_DIR = os.path.join(ROOT, 'build', 'missinline')
DUMP_CACHE = os.path.join(CACHE_DIR, 'dump.json')
SRC_CACHE = os.path.join(CACHE_DIR, 'src.json')
REPORT = os.path.join(ROOT, 'build', 'GOWE69', 'report.json')

# Operadores del lenguaje: ruido puro (new/delete/=/[] son los 4 mas frecuentes
# del volcado y practicamente nunca son la causa de un near-miss).
NOISE_OPS = ('operator new', 'operator delete', 'operator=', 'operator []',
             'operator[]', 'operator new []', 'operator delete []')


# ---------------------------------------------------------------------------
# 1. Parseo del volcado DWARF
# ---------------------------------------------------------------------------

RANGE_RE = re.compile(r'^//\s*Range:\s*(0x[0-9A-Fa-f]+)\s*->\s*(0x[0-9A-Fa-f]+)')
CU_RE = re.compile(r'^\s*Compile unit:\s*(.+?)\s*$')
LOCAL_RE = re.compile(r'^(.*?)\b(\w+)\s*;\s*(?://\s*([a-z]\w*))?\s*$')


def _split_head(sig):
    """Devuelve la parte del prototipo anterior a la lista de parametros."""
    depth_ang = 0
    for i, ch in enumerate(sig):
        if ch == '<':
            depth_ang += 1
        elif ch == '>':
            if depth_ang:
                depth_ang -= 1
        elif ch == '(' and depth_ang == 0:
            return sig[:i]
    return None


def _trailing_qualified(head):
    """Extrae el nombre cualificado final (Clase::Metodo) de una cabecera."""
    head = head.rstrip()
    i = len(head)
    depth = 0
    while i > 0:
        ch = head[i - 1]
        if ch == '>':
            depth += 1
        elif ch == '<':
            if depth:
                depth -= 1
            else:
                break
        elif depth:
            pass
        elif ch.isalnum() or ch in '_:~,$.':
            pass
        else:
            break
        i -= 1
    return head[i:].strip()


def sig_name(sig):
    """Nombre cualificado de un prototipo del volcado ('' si no se reconoce)."""
    head = _split_head(sig)
    if head is None:
        return ''
    op = head.rfind('operator')
    if op != -1 and (op == 0 or not (head[op - 1].isalnum() or head[op - 1] == '_')):
        opname = 'operator' + head[op + len('operator'):].strip()
        opname = re.sub(r'\s+', ' ', opname)
        prefix = head[:op].rstrip()
        if prefix.endswith('::'):
            cls = _trailing_qualified(prefix[:-2])
            return cls + '::' + opname if cls else opname
        return opname
    return _trailing_qualified(head)


def base_class(name):
    """Clase de un nombre cualificado, sin argumentos de plantilla."""
    depth = 0
    cut = -1
    for i in range(len(name) - 1):
        ch = name[i]
        if ch == '<':
            depth += 1
        elif ch == '>':
            depth -= 1
        elif ch == ':' and name[i + 1] == ':' and depth == 0:
            cut = i
    if cut < 0:
        return ''
    return re.sub(r'<.*', '', name[:cut]).strip()


def base_method(name):
    depth = 0
    cut = -1
    for i in range(len(name) - 1):
        ch = name[i]
        if ch == '<':
            depth += 1
        elif ch == '>':
            depth -= 1
        elif ch == ':' and name[i + 1] == ':' and depth == 0:
            cut = i
    m = name if cut < 0 else name[cut + 2:]
    return re.sub(r'<.*', '', m).strip()


def is_noise(name):
    m = base_method(name)
    for op in NOISE_OPS:
        if m == op or m.replace(' ', '') == op.replace(' ', ''):
            return True
    return False


def parse_dump(path=DUMP, verbose=True):
    """Parsea el volcado en streaming. Devuelve el diccionario del cache.

    Estructura del volcado: indentacion de 4 espacios por nivel; un
    `// Range: A -> B` precede al constructo al que pertenece; los cuerpos
    `{}` vacios significan "el original lo tenia inline", no "vacio".
    """
    units = []
    funcs = []            # {addr, end, name, unit, inl: [[name, depth], ...]}
    unit_decls = {}       # unidad -> set de inlines declarados a nivel 0
    stats = dict(ranges=0, expansions=0, anon=0, locals=0, locals_reg=0,
                 depth_hist=defaultdict(int))

    cur_unit = None
    stack = []            # [(indent, kind)] kind: agg|func|inl|anon
    pend_range = None
    cur_func = None
    decls = None

    def in_function():
        for ind, kind in stack:
            if kind == 'agg':
                return False
        return any(k == 'func' for _, k in stack)

    fh = open(path, 'r', encoding='utf-8', errors='replace')
    for lineno, raw in enumerate(fh):
        line = raw.rstrip('\n').rstrip()
        if not line:
            continue
        stripped = line.lstrip()
        indent = len(line) - len(stripped)

        if stripped.startswith('Compile unit:'):
            m = CU_RE.match(line)
            if m:
                cur_unit = os.path.splitext(os.path.basename(
                    m.group(1).replace('\\', '/')))[0]
                units.append(cur_unit)
                decls = set()
                unit_decls[cur_unit] = decls
                stack = []
                cur_func = None
            continue

        if stripped.startswith('//'):
            m = RANGE_RE.match(stripped)
            if m:
                stats['ranges'] += 1
                pend_range = (int(m.group(1), 16), int(m.group(2), 16))
            continue

        if stripped.startswith('/*') and 'anonymous block' not in stripped:
            continue

        closes = stripped.startswith('}')
        opens = stripped.endswith('{') or stripped.endswith('{}')
        selfclosed = stripped.endswith('{}')

        if closes and not opens:
            while stack and stack[-1][0] >= indent:
                stack.pop()
            pend_range = None
            if not stack:
                cur_func = None
            continue

        if not opens:
            # posible declaracion de local (`float heat; // f30`)
            if in_function():
                m = LOCAL_RE.match(stripped)
                if m:
                    stats['locals'] += 1
                    if m.group(3):
                        stats['locals_reg'] += 1
                        if cur_func is not None:
                            cur_func['loc'].append([m.group(2), m.group(3)])
            pend_range = None
            continue

        while stack and stack[-1][0] >= indent:
            stack.pop()
        if not stack:
            cur_func = None

        body = stripped[:-2] if selfclosed else stripped[:-1]
        body = body.strip()

        if body.startswith('struct ') or body.startswith('class ') or \
           body.startswith('union ') or body.startswith('enum ') or \
           body == 'struct' or body == 'union':
            kind = 'agg'
        elif body.startswith('/* anonymous block */'):
            kind = 'anon'
            stats['anon'] += 1
        elif body.startswith('inline ') or body.startswith('static inline '):
            kind = 'inl'
        else:
            kind = 'func'

        inside_agg = any(k == 'agg' for _, k in stack)
        inside_func = in_function()

        if kind == 'inl':
            name = sig_name(re.sub(r'^(static\s+)?inline\s+', '', body))
            if inside_func and cur_func is not None and name:
                depth = sum(1 for _, k in stack if k in ('inl',))
                stats['expansions'] += 1
                stats['depth_hist'][depth] += 1
                # La anchura del rango decide si el inline se EXPANDIO ahi. Un
                # rango degenerado (A -> A) significa "declarado inline pero no
                # expandido": el original emite un `bl` real en esa direccion.
                # Sin este dato el ranking cuenta como expansiones cosas que no
                # lo son, y OrthoInverse encabezaba la lista valiendo cero.
                w = (pend_range[1] - pend_range[0]) if pend_range else 0
                cur_func['inl'].append([name, depth,
                                        pend_range[0] if pend_range else 0, w])
            elif not inside_agg and not inside_func and name and decls is not None:
                decls.add(name)
        elif kind == 'func' and not inside_agg and not inside_func:
            name = sig_name(body)
            if name:
                cur_func = dict(addr=pend_range[0] if pend_range else 0,
                                end=pend_range[1] if pend_range else 0,
                                name=name, unit=cur_unit, inl=[], loc=[])
                funcs.append(cur_func)

        if not selfclosed:
            stack.append((indent, kind))
        pend_range = None

        if verbose and (lineno % 500000 == 0) and lineno:
            sys.stderr.write('  ... %d lineas\n' % lineno)
    fh.close()

    return dict(units=units,
                funcs=funcs,
                unit_decls=dict((k, sorted(v)) for k, v in unit_decls.items()),
                stats=dict(ranges=stats['ranges'], expansions=stats['expansions'],
                           anon=stats['anon'], locals=stats['locals'],
                           locals_reg=stats['locals_reg'],
                           depth_hist=dict(stats['depth_hist'])))


# ---------------------------------------------------------------------------
# 2. Indice de nuestro arbol de fuentes
# ---------------------------------------------------------------------------

BLOCK_COMMENT = re.compile(r'/\*.*?\*/', re.S)
LINE_COMMENT = re.compile(r'//[^\n]*')
CONT = re.compile(r'\\\s*\n')
# `class ALIGN_16 IUnknown {`, `struct MW_API Foo : public Bar {` -> ultimo id
AGG_OPEN = re.compile(r'\b(class|struct|union)\s+((?:[A-Za-z_]\w*\s+)*)([A-Za-z_]\w*)'
                      r'\s*(?:<[^<>{;]*>\s*)?(?::[^{;()]*)?\{')
MEMBER_RE = re.compile(r'(operator\s*(?:\[\s*\]|\(\s*\)|[^\s(\w]{1,3}|[A-Za-z_]\w*(?:\s*\*)?)'
                       r'|~?[A-Za-z_]\w*)\s*\(')
OUTOFLINE_RE = re.compile(r'([A-Za-z_]\w*)\s*(?:<[^<>]*>)?\s*::\s*'
                          r'(operator\s*(?:\[\s*\]|\(\s*\)|[^\s(\w]{1,3}|[A-Za-z_]\w*(?:\s*\*)?)'
                          r'|~?[A-Za-z_]\w*)\s*\(')
KEYWORDS = set('if for while switch return sizeof catch do else new delete throw '
               'case typedef defined static_cast const_cast reinterpret_cast '
               'dynamic_cast and or not typeid alignof decltype'.split())

SRC_EXT = ('.h', '.hpp', '.hxx', '.inl', '.cpp', '.c', '.cc', '.inc')


def _logical_lines(txt):
    """Agrupa lineas fisicas en sentencias logicas.

    Este codigo parte prototipos en dos lineas fisicas (`inline void` /
    `__destroy(...)`) y un escaner por lineas los clasifica mal.
    Acumulamos hasta ver `;`, `{` o `}`.
    """
    buf = []
    for line in txt.splitlines():
        s = line.strip()
        if not s:
            continue
        if s.startswith('#'):
            if buf:
                yield ' '.join(buf)
                buf = []
            yield s
            continue
        buf.append(s)
        if ';' in s or '{' in s or '}' in s:
            yield ' '.join(buf)
            buf = []
    if buf:
        yield ' '.join(buf)


def _norm_meth(s):
    return re.sub(r'\s+', '', s)


def scan_src(root=SRC, verbose=True):
    """Indexa src/: por (Clase, Metodo) que tipo de declaracion tenemos.

    inclass_body : cuerpo dentro de la clase  -> INLINE de verdad en GCC 2.9
    inclass_decl : solo declarado en la clase -> emite simbolo, NO se expande
    ool_inline   : definicion fuera de clase con la palabra `inline`
    ool_def      : definicion fuera de clase sin `inline` -> llamada real
    macro        : el miembro lo genera una macro (DECL_INTERFACE y companeras)
    """
    idx = defaultdict(set)           # (cls, meth) -> set(kind)
    where = defaultdict(set)         # (cls, meth) -> set(fichero)
    free = defaultdict(set)          # nombre libre -> set(kind)
    macro_meths = defaultdict(set)   # metodo -> set(macro que lo genera)
    macro_body = set()               # metodos que la macro define con cuerpo
    classes = {}                     # nombre de clase -> fichero
    nfiles = 0

    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in ('.git', 'build')]
        for fn in filenames:
            if not fn.endswith(SRC_EXT):
                continue
            p = os.path.join(dirpath, fn)
            try:
                txt = open(p, 'r', encoding='utf-8', errors='replace').read()
            except IOError:
                continue
            nfiles += 1
            rel = os.path.relpath(p, ROOT).replace('\\', '/')
            txt = CONT.sub(' ', txt)          # une macros multilinea
            txt = BLOCK_COMMENT.sub(' ', txt)
            txt = LINE_COMMENT.sub('', txt)

            stack = []      # [(kind, name)] kind in class|func|other
            for s in _logical_lines(txt):
                if s.startswith('#'):
                    if s.startswith('#define'):
                        mm = re.match(r'#define\s+([A-Za-z_]\w*)', s)
                        macro = mm.group(1) if mm else '?'
                        for m in MEMBER_RE.finditer(s):
                            nm = _norm_meth(m.group(1))
                            if nm in KEYWORDS or nm == macro:
                                continue
                            if '{' in s[m.end():]:
                                macro_body.add(nm)
                            macro_meths[nm].add(macro)
                    continue

                cls = None
                for kind, name in reversed(stack):
                    if kind == 'func':
                        break
                    if kind == 'class':
                        cls = name
                        break
                in_func = any(k == 'func' for k, _ in stack)

                if not in_func:
                    if cls:
                        for m in MEMBER_RE.finditer(s):
                            nm = _norm_meth(m.group(1))
                            if nm in KEYWORDS:
                                continue
                            pre = s[:m.start()].rstrip()
                            if pre.endswith('::') or pre.endswith('.'):
                                continue
                            rest = s[m.end():]
                            # `Foo *Get() {}` es un ACCESOR VACIO: esta presente, asi que GCC lo
                            # expande, pero expande NADA. El llamante se queda sin la carga que el
                            # original si hace, y ninguna funcion que lo use puede casar. Hay 597 en
                            # el arbol y hasta ahora contaban como PRESENT_INLINE, o sea como hechos.
                            if re.search(r'\)\s*(const\s*)?\{\s*\}\s*$', rest.strip()) or \
                               re.search(r'^[^{]*\{\s*\}\s*(//.*)?$', rest.strip()):
                                k = 'inclass_empty'
                            elif '{' in rest:
                                k = 'inclass_body'
                            elif ';' in rest:
                                k = 'inclass_decl'
                            else:
                                k = 'inclass_open'   # cuerpo en lineas siguientes
                            idx[(cls, nm)].add(k)
                            where[(cls, nm)].add(rel)
                    else:
                        for m in OUTOFLINE_RE.finditer(s):
                            c = m.group(1)
                            nm = _norm_meth(m.group(2))
                            if nm in KEYWORDS:
                                continue
                            k = 'ool_inline' if re.search(r'\binline\b', s) else 'ool_def'
                            idx[(c, nm)].add(k)
                            where[(c, nm)].add(rel)
                        for m in MEMBER_RE.finditer(s):
                            nm = _norm_meth(m.group(1))
                            if nm in KEYWORDS:
                                continue
                            if s[:m.start()].rstrip().endswith(('::', '.', '>')):
                                continue
                            if re.search(r'\binline\b', s):
                                free[nm].add('free_inline')
                            elif '{' in s[m.end():]:
                                free[nm].add('free_def')

                am = AGG_OPEN.search(s)
                agg_pos = am.start() if am else None
                if am:
                    classes.setdefault(am.group(3), rel)
                first = True
                for i, ch in enumerate(s):
                    if ch == '{':
                        if first and am and agg_pos is not None and i >= agg_pos:
                            stack.append(('class', am.group(3)))
                        elif first and '(' in s[:i]:
                            stack.append(('func', None))
                        else:
                            stack.append(('other', None))
                        first = False
                    elif ch == '}':
                        if stack:
                            stack.pop()

    if verbose:
        sys.stderr.write('  %d ficheros, %d simbolos, %d clases, %d metodos de macro\n'
                         % (nfiles, len(idx), len(classes), len(macro_meths)))
    return dict(members=dict(('%s\x01%s' % k, sorted(v)) for k, v in idx.items()),
                where=dict(('%s\x01%s' % k, sorted(v)[:3]) for k, v in where.items()),
                free=dict((k, sorted(v)) for k, v in free.items()),
                macro=dict((k, sorted(v)[:3]) for k, v in macro_meths.items()),
                macro_body=sorted(macro_body),
                classes=classes)


# ---------------------------------------------------------------------------
# 3. Cache
# ---------------------------------------------------------------------------

def load_cache(refresh=False):
    if not os.path.isdir(CACHE_DIR):
        os.makedirs(CACHE_DIR)
    if refresh or not os.path.exists(DUMP_CACHE):
        sys.stderr.write('[missinline] parseando %s ...\n' % DUMP)
        d = parse_dump()
        json.dump(d, open(DUMP_CACHE, 'w'), separators=(',', ':'))
    else:
        d = json.load(open(DUMP_CACHE))
    if refresh or not os.path.exists(SRC_CACHE):
        sys.stderr.write('[missinline] indexando %s ...\n' % SRC)
        s = scan_src()
        json.dump(s, open(SRC_CACHE, 'w'), separators=(',', ':'))
    else:
        s = json.load(open(SRC_CACHE))
    return d, s


def load_report(path=None):
    """Devuelve (addr(int) -> (nombre, size, fuzzy), unidades_vivas).

    "unidad viva" = unidad cuyo matched_code_percent > 0, es decir, una que
    ya estamos escribiendo. Una unidad entera sin escribir (gc_tasks, filesys,
    metrotrk...) no aporta "inlines que faltan": le falta TODO, y meterla en
    el ranking solo hace ruido.
    """
    # OJO: `path=REPORT` como valor por defecto NO vale. Python lo fija al DEFINIR la funcion,
    # asi que `--version` reasignaba el global y esta seguia leyendo el informe de GameCube.
    path = path or REPORT
    by_addr = {}
    active = set()
    if not os.path.exists(path):
        return by_addr, active
    rep = json.load(open(path))
    for u in rep.get('units') or []:
        fs = u.get('functions') or []
        if not fs:
            continue
        pct = (u.get('measures') or {}).get('matched_code_percent') or 0.0
        unit = u['name'].replace('\\', '/').split('/')[-1]
        if pct > 0:
            active.add(unit)
        for f in fs:
            md = f.get('metadata') or {}
            va = md.get('virtual_address')
            if va is None:
                continue
            by_addr[int(va)] = (md.get('demangled_name') or f.get('name'),
                                int(f.get('size') or 0),
                                float(f.get('fuzzy_match_percent') or 0.0))
    return by_addr, active


# ---------------------------------------------------------------------------
# 4. Clasificacion
# ---------------------------------------------------------------------------

def classify(name, srcidx):
    """Clasifica un inline del original contra nuestro arbol.

    PRESENT_INLINE : lo tenemos con cuerpo en la clase (o `inline` fuera) ->
                     GCC 2.9 lo expande, no emite simbolo. No es problema.
    NOT_INLINE     : lo tenemos, pero declarado en la clase y definido fuera
                     sin `inline` -> emitimos una LLAMADA donde el original
                     expandia. Causa clasica de near-miss.
    MACRO          : el miembro lo genera una macro (DECL_INTERFACE...);
                     casi seguro presente, revisar a mano.
    MISSING        : la clase existe en src/ pero el metodo no aparece.
    NO_CLASS       : ni la clase esta en src/ (plantilla, namespace, o unidad
                     que aun no hemos escrito).
    """
    cls = base_class(name)
    meth = base_method(name)
    meth_key = meth.replace(' ', '')
    members = srcidx['members']
    macro = srcidx.get('macro') or {}
    classes = srcidx['classes']

    cands = []
    if cls:
        cands.append(cls)
        if '::' in cls:                       # UTL::Std::vector -> vector
            cands.append(cls.rsplit('::', 1)[-1])
    for c in cands:
        kinds = members.get('%s\x01%s' % (c, meth_key))
        if kinds:
            w = srcidx['where'].get('%s\x01%s' % (c, meth_key), [])
            if 'inclass_body' in kinds or 'inclass_open' in kinds or \
               'ool_inline' in kinds:
                return 'PRESENT_INLINE', w
            # Solo vacio, sin ninguna otra forma con cuerpo: esta, pero no emite nada.
            # OJO: un CONSTRUCTOR o un DESTRUCTOR con `{}` casi siempre es CORRECTO -- la base y
            # los miembros hacen el trabajo, y el original expande justamente esa llamada
            # implicita. Medido: los tres unicos VACIO que bloqueaban algo en GameCube
            # (bTList<SelectablePart>::bTList y dos ~dtor) eran eso, y bList() si inicializa
            # HeadNode. Se clasifican aparte para que no ensucien la lista de trabajo.
            if 'inclass_empty' in kinds:
                corto = c.rsplit('::', 1)[-1]
                if meth_key == corto or meth_key == '~' + corto:
                    return 'VACIO_CTOR', w
                return 'VACIO', w
            return 'NOT_INLINE', w
    if not cls:
        kinds = srcidx['free'].get(meth_key)
        if kinds:
            return ('PRESENT_INLINE' if 'free_inline' in kinds
                    else 'NOT_INLINE'), []
    if meth_key in macro:
        tag = 'PRESENT_MACRO' if meth_key in srcidx.get('macro_body', ())             else 'MACRO'
        return tag, ['#' + m for m in macro[meth_key]]
    if not cls:
        return 'MISSING', []
    for c in cands:
        if c in classes:
            return 'MISSING', [classes[c]]
    return 'NO_CLASS', []


def build_table(dump, srcidx, report, unit_filter=None, active=None):
    """inline -> estadisticas de impacto."""
    agg = {}
    for f in dump['funcs']:
        if unit_filter and f['unit'] != unit_filter:
            continue
        if active is not None and f['unit'] not in active:
            continue
        info = report.get(f['addr'])
        size = info[1] if info else 0
        fuzzy = info[0] and info[2] if info else None
        fuzzy = info[2] if info else None
        seen = set()
        for nm, depth, addr, w in (tuple(e) + (0,) * (4 - len(e)) for e in f['inl']):
            if not w:
                continue        # declarado inline pero no expandido aqui
            if nm in seen:
                continue
            seen.add(nm)
            e = agg.get(nm)
            if e is None:
                e = agg[nm] = dict(name=nm, exp=0, funcs=0, units=set(),
                                   bad=0, bad_bytes=0, bad_funcs=[],
                                   good=0, unknown=0, maxdepth=0)
            e['funcs'] += 1
            e['units'].add(f['unit'])
            if fuzzy is None:
                e['unknown'] += 1
            elif fuzzy < 100.0:
                e['bad'] += 1
                e['bad_bytes'] += size
                e['bad_funcs'].append((f['name'], f['unit'], size, fuzzy))
            else:
                e['good'] += 1
            if depth > e['maxdepth']:
                e['maxdepth'] = depth
        for nm, depth, addr, w in (tuple(e) + (0,) * (4 - len(e)) for e in f['inl']):
            if w and nm in agg:
                agg[nm]['exp'] += 1
    for e in agg.values():
        e['units'] = sorted(e['units'])
        e['bad_funcs'].sort(key=lambda t: -t[2])
    return agg


# ---------------------------------------------------------------------------
# 5. Comandos
# ---------------------------------------------------------------------------

def cmd_index(args):
    d = parse_dump()
    if not os.path.isdir(CACHE_DIR):
        os.makedirs(CACHE_DIR)
    json.dump(d, open(DUMP_CACHE, 'w'), separators=(',', ':'))
    st = d['stats']
    print('unidades          : %d' % len(d['units']))
    print('funciones         : %d' % len(d['funcs']))
    print('rangos            : %d' % st['ranges'])
    print('expansiones inline: %d' % st['expansions'])
    print('bloques anonimos  : %d' % st['anon'])
    print('locales           : %d (con registro: %d)'
          % (st['locals'], st['locals_reg']))
    print('profundidad       : %s' % sorted(st['depth_hist'].items()))


def cmd_scan(args):
    s = scan_src()
    if not os.path.isdir(CACHE_DIR):
        os.makedirs(CACHE_DIR)
    json.dump(s, open(SRC_CACHE, 'w'), separators=(',', ':'))
    print('simbolos indexados: %d' % len(s['members']))


def cmd_rank(args):
    dump, srcidx = load_cache(args.refresh)
    report, active = load_report()
    agg = build_table(dump, srcidx, report, args.unit,
                      None if getattr(args, 'dead', False) else active)

    rows = []
    for nm, e in agg.items():
        if not args.all and is_noise(nm):
            continue
        if e['bad'] < args.min_bad:
            continue
        kind, w = classify(nm, srcidx)
        if not args.all and kind in ('PRESENT_INLINE', 'PRESENT_MACRO'):
            continue
        e['kind'] = kind
        e['where'] = w
        rows.append(e)
    rows.sort(key=lambda e: (-e['bad'], -e['bad_bytes']))

    counts = defaultdict(int)
    for nm, e in agg.items():
        if is_noise(nm):
            counts['(operador de lenguaje)'] += 1
            continue
        k, _ = classify(nm, srcidx)
        counts[k] += 1
    print('== censo de inlines distintos que el original expande ==')
    for k in sorted(counts, key=lambda k: -counts[k]):
        print('  %-24s %6d' % (k, counts[k]))
    print()
    hdr = '%-52s %5s %5s %5s %9s  %s' % (
        'INLINE', 'exp', 'fn', 'sin', 'bytes', 'estado / unidades')
    print(hdr)
    print('-' * len(hdr))
    for e in rows[:args.top]:
        units = ','.join(e['units'][:4])
        if len(e['units']) > 4:
            units += ',+%d' % (len(e['units']) - 4)
        print('%-52s %5d %5d %5d %9d  %-13s %s'
              % (e['name'][:52], e['exp'], e['funcs'], e['bad'],
                 e['bad_bytes'], e['kind'], units))
    print()
    print('exp=expansiones  fn=funciones que lo expanden  '
          'sin=de esas, por debajo del 100%  bytes=suma de su tamano')


def cmd_who(args):
    dump, srcidx = load_cache(args.refresh)
    report, active = load_report()
    agg = build_table(dump, srcidx, report, args.unit,
                      None if getattr(args, 'dead', False) else active)
    e = agg.get(args.name)
    if e is None:
        cands = [k for k in agg if args.name.lower() in k.lower()]
        print('no encontrado. candidatos: %s' % ', '.join(sorted(cands)[:20]))
        return
    kind, w = classify(args.name, srcidx)
    print('%s  [%s]  %s' % (e['name'], kind, ' '.join(w)))
    print('expansiones %d en %d funciones (%d sin casar, %d bytes), unidades: %s'
          % (e['exp'], e['funcs'], e['bad'], e['bad_bytes'], ','.join(e['units'])))
    print()
    for nm, unit, size, fuzzy in e['bad_funcs'][:args.top]:
        print('  %6.2f%%  %6d  %-22s %s' % (fuzzy, size, unit, nm))


def cmd_func(args):
    dump, srcidx = load_cache(args.refresh)
    report, active = load_report()
    hits = [f for f in dump['funcs']
            if f['name'] == args.name or f['name'].endswith('::' + args.name)]
    if not hits:
        hits = [f for f in dump['funcs'] if args.name.lower() in f['name'].lower()]
        print('coincidencias parciales: %s'
              % ', '.join(sorted(set(f['name'] for f in hits))[:20]))
        return
    for f in hits:
        info = report.get(f['addr'])
        print('%s  [%s]  0x%08X  %s' % (
            f['name'], f['unit'], f['addr'],
            ('%.2f%% %d B' % (info[2], info[1])) if info else 'sin report'))
        for nm, depth, addr, width in (tuple(e) + (0,) * (4 - len(e)) for e in f['inl']):
            kind, _ = classify(nm, srcidx)
            flag = '' if kind in ('PRESENT_INLINE', 'PRESENT_MACRO')                 else '   <== %s' % kind
            # Sin anchura de rango no hubo expansion: se marca para no confundirlo
            # con codigo realmente inlineado ahi.
            mark = ' ' if width else '.'
            print(' %s%s0x%08X %s%s' % (mark, '  ' * depth, addr, nm, flag))
        print()


def cmd_unit(args):
    dump, srcidx = load_cache(args.refresh)
    report, active = load_report()
    agg = build_table(dump, srcidx, report, args.name)
    rows = []
    for nm, e in agg.items():
        if is_noise(nm):
            continue
        kind, w = classify(nm, srcidx)
        if kind in ('PRESENT_INLINE', 'PRESENT_MACRO'):
            continue
        e['kind'] = kind
        rows.append(e)
    rows.sort(key=lambda e: (-e['bad'], -e['bad_bytes']))
    print('unidad %s: %d inlines ausentes con impacto' % (args.name, len(rows)))
    for e in rows[:args.top]:
        print('%-52s fn=%-4d sin=%-4d %7d B  %s'
              % (e['name'][:52], e['funcs'], e['bad'], e['bad_bytes'], e['kind']))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--refresh', action='store_true', help='rehace el cache')
    ap.add_argument('--version', default='GOWE69',
                    help='contra que version se mide el "sin" (por defecto GOWE69). AVISO: el arbol '
                         'de inlines sale del DWARF de GameCube, que es el unico que lo tiene, y se '
                         'cruza con el informe POR DIRECCION. Las direcciones de PS2, X360 y PC son '
                         'otras, asi que con --version distinto de GOWE69 el censo si vale pero la '
                         'columna "sin" sale vacia. Cruzar por nombre esta sin hacer.')
    sub = ap.add_subparsers(dest='cmd')

    p = sub.add_parser('index'); p.set_defaults(fn=cmd_index)
    p = sub.add_parser('scan'); p.set_defaults(fn=cmd_scan)

    p = sub.add_parser('rank')
    p.add_argument('--top', type=int, default=40)
    p.add_argument('--unit')
    p.add_argument('--min-bad', type=int, default=1)
    p.add_argument('--all', action='store_true',
                   help='no filtra operadores ni los que ya existen')
    p.add_argument('--dead', action='store_true',
                   help='incluye unidades sin empezar (0%% casado)')
    p.set_defaults(fn=cmd_rank)

    p = sub.add_parser('who')
    p.add_argument('name')
    p.add_argument('--top', type=int, default=30)
    p.add_argument('--unit')
    p.set_defaults(fn=cmd_who)

    p = sub.add_parser('func')
    p.add_argument('name')
    p.set_defaults(fn=cmd_func)

    p = sub.add_parser('unit')
    p.add_argument('name')
    p.add_argument('--top', type=int, default=40)
    p.set_defaults(fn=cmd_unit)

    args = ap.parse_args()
    if not getattr(args, 'fn', None):
        ap.print_help()
        return 1
    global REPORT
    if args.version != 'GOWE69':
        # ninja deja el informe en build/<V>/report.json; pctall.py, en build/versiones/<V>.json.
        for cand in (os.path.join(ROOT, 'build', args.version, 'report.json'),
                     os.path.join(ROOT, 'build', 'versiones', args.version + '.json')):
            if os.path.exists(cand):
                REPORT = cand
                break
        else:
            print('no encuentro el informe de %s: corre antes '
                  '`python scripts/pctall.py %s`' % (args.version, args.version))
            return 2
    return args.fn(args) or 0


if __name__ == '__main__':
    sys.exit(main())
