#!/usr/bin/env python3
"""libdwarf.py — el volcado DWARF de una unidad de MIDDLEWARE, servido en C.

Las bibliotecas (vp6, snd, path, realcore, realmemcard, libc...) no salen en
`plan.py`/`fuse.py`/`regmap.py`, que sólo miran `Speed/Indep/SourceLists`. Pero
`symbols/mw_dwarfdump.nothpp` SÍ las trae enteras, y con el prototipo exacto de
cada función, el registro de cada parámetro y de cada local, y la definición
completa de cada struct.

    python scripts/libdwarf.py cu    vputil.c
        vuelca la unidad de compilación entera.

    python scripts/libdwarf.py fns   vputil.c
        sólo la lista de funciones: firma, registro de cada parámetro y locales,
        con los cuerpos de struct expandidos colapsados a STRUCT.

    python scripts/libdwarf.py struct decodemode.c PB_INSTANCE _BITREADER ...
        convierte esos structs en C COMPILABLE: hoista los struct/enum anónimos
        repetidos a typedefs deduplicados, ordena los typedefs topológicamente e
        **invierte las dimensiones de los arrays** (el volcado las imprime al
        revés: `int T[4][8]` en el volcado es `int T[8][4]` en la fuente; se
        comprueba con el desplazamiento del índice en el asm).

        Los structs referenciados y no pedidos salen como `struct X;` opaco: si
        el compilador se queja de "incomplete type", añádelos a la lista.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DUMP = os.path.join(ROOT, 'symbols', 'mw_dwarfdump.nothpp')


# --------------------------------------------------------------------------- cu

def cu_text(cfile):
    """Texto de la unidad de compilación cuyo fichero fuente es `cfile`."""
    out, started = [], False
    with open(DUMP, encoding='utf-8', errors='replace') as fh:
        for line in fh:
            if 'Compile unit:' in line:
                if started:
                    break
                if line.rstrip().endswith('/' + cfile):
                    started = True
                    out.append(line)
                    continue
            if started:
                out.append(line)
    if not out:
        sys.exit('no encuentro la unidad %s en el volcado' % cfile)
    return ''.join(out)


# -------------------------------------------------------------------------- fns

def show_fns(cfile):
    lines = cu_text(cfile).split('\n')
    i = 0
    while i < len(lines):
        if not lines[i].startswith('// Range:'):
            i += 1
            continue
        sig, j, depth = [], i + 1, 0
        while j < len(lines):
            sig.append(lines[j])
            depth += lines[j].count('{') - lines[j].count('}')
            if depth == 0 and lines[j].rstrip().endswith(('{', '{}')):
                break
            j += 1
        raw = re.sub(r'//[^\n]*?(?=(struct|\}|$))', ' ', ' '.join(sig))
        raw = re.sub(r'struct \{.*?\}', 'STRUCT', raw)
        print(lines[i])
        print(re.sub(r'\s+', ' ', raw))
        k, depth = j + 1, (1 if raw.rstrip().endswith('{') else 0)
        while k < len(lines) and depth > 0:
            depth += lines[k].count('{') - lines[k].count('}')
            if depth >= 0:
                print(lines[k])
            k += 1
        print()
        i = k


# ----------------------------------------------------------------------- struct

TYPES = []
SEEN = {}


def emit_type(kind, body, hint):
    key = kind + '\n' + body
    if key in SEEN:
        return SEEN[key]
    name, n = hint, 2
    while name in [t[0] for t in TYPES]:
        name = '%s%d' % (hint, n)
        n += 1
    SEEN[key] = name
    TYPES.append((name, 'typedef %s {\n%s} %s;' % (kind, body, name)))
    return name


def strip_comment(line):
    return re.sub(r'\s*//.*$', '', line)


def fix_dims(decl):
    """El volcado imprime las dimensiones AL REVES: `T a[2][8]` es `T a[8][2]`."""
    m = re.match(r'^(.*?)((?:\[\d+\])+)$', decl.strip())
    if not m:
        return decl
    dims = re.findall(r'\[(\d+)\]', m.group(2))
    if len(dims) < 2:
        return decl
    return m.group(1) + ''.join('[%s]' % d for d in reversed(dims))


def convert(block):
    out, i = [], 0
    while i < len(block):
        s = strip_comment(block[i]).strip()
        if s in ('', '// Members'):
            i += 1
            continue
        m = re.match(r'^(struct|enum|union)\s*\{$', s)
        if m:
            kind, depth, inner, j = m.group(1), 1, [], i + 1
            while j < len(block):
                depth += block[j].count('{') - block[j].count('}')
                if depth == 0:
                    break
                inner.append(block[j])
                j += 1
            decl = strip_comment(block[j]).strip()[1:].strip().rstrip(';').strip()
            base = re.sub(r'[^A-Za-z0-9_]', '', decl.split('[')[0].replace('*', '').strip())
            if kind == 'enum':
                body = '\n'.join('    ' + strip_comment(x).strip()
                                 for x in inner if strip_comment(x).strip()) + '\n'
                tname = emit_type('enum', body, base.upper() + '_TYPE')
            else:
                tname = emit_type(kind, convert(inner), base.upper() + '_TYPE')
            out.append('    %s %s;' % (tname, fix_dims(decl)))
            i = j + 1
            continue
        if s.endswith(';'):
            mm = re.match(r'^(.*?[\s\*])(\w+(?:\[\d+\])+);$', s)
            if mm:
                s = mm.group(1) + fix_dims(mm.group(2)) + ';'
            out.append('    ' + s)
        i += 1
    return '\n'.join(out) + '\n'


def find_struct(lines, name):
    for i, l in enumerate(lines):
        if re.match(r'^\s*struct %s \{\s*$' % re.escape(name), l):
            depth, block = 0, []
            for j in range(i, len(lines)):
                block.append(lines[j])
                depth += lines[j].count('{') - lines[j].count('}')
                if depth == 0 and len(block) > 1:
                    return block
    return None


def topo(text):
    """Reordena los typedef para que las dependencias salgan primero."""
    raw = re.findall(r'(typedef (?:struct|union|enum) \{.*?\n\} \w+;)', text, re.S)
    byname = {re.search(r'\} (\w+);\s*$', t).group(1): t for t in raw}
    order, seen = [], set()

    def visit(n, stack=()):
        if n in seen or n not in byname or n in stack:
            return
        inner = re.sub(r'\} \w+;\s*$', '', byname[n])
        for dep in sorted(set(re.findall(r'\b(\w+)\b', inner))):
            if dep in byname and dep != n:
                visit(dep, stack + (n,))
        seen.add(n)
        order.append(n)

    for n in byname:
        visit(n)
    return [byname[n] for n in order]


def show_struct(cfile, names):
    lines = cu_text(cfile).split('\n')
    todo, done, outs = list(names), [], []
    while todo:
        name = todo.pop(0)
        if name in done:
            continue
        done.append(name)
        block = find_struct(lines, name)
        if not block:
            outs.append('// no encontrado en el volcado: %s' % name)
            continue
        body = convert(block[1:-1])
        outs.append('typedef struct {\n%s} %s;' % (body, name))

    text = '\n\n'.join([t for _, t in TYPES] + list(reversed(outs))) + '\n'
    defined = set(re.findall(r'^\} (\w+);', text, re.M))
    for n in sorted(defined, key=len, reverse=True):
        text = re.sub(r'\b(?:struct|union) %s\b' % re.escape(n), n, text)
    missing = sorted(set(re.findall(r'\b(?:struct|union) (\w+)', text)) - defined)
    head = ''.join('struct %s;\n' % m for m in missing)
    print((head + '\n' if head else '') + '\n\n'.join(topo(text)))
    if missing:
        sys.stderr.write('opacos (anadelos a la lista si hacen falta completos): %s\n'
                         % ', '.join(missing))


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    cmd, cfile = sys.argv[1], sys.argv[2]
    if cmd == 'cu':
        sys.stdout.write(cu_text(cfile))
    elif cmd == 'fns':
        show_fns(cfile)
    elif cmd == 'struct':
        if len(sys.argv) < 4:
            sys.exit('uso: libdwarf.py struct <fichero.c> <Struct> [Struct...]')
        show_struct(cfile, sys.argv[3:])
    else:
        sys.exit(__doc__)


main()
