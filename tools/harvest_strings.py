#!/usr/bin/env python3
"""Cosecha las cadenas de .rodata y las liga a quien las usa.

El camino que NO funciona: cruzar los hashes del manifiesto de assets contra
constantes del binario. NFSMW no precomputa esos hashes; guarda el path como
.string y llama a Attrib::StringHash32 en tiempo de ejecucion.

El camino que si: cruzar el texto. Una cadena tiene dos clases de consumidor y
cada una abre un frente distinto.

  - Una funcion que la carga con lis/addi. El contenido dice que hace esa
    funcion. Rinde poco: casi todas las que cargan paths de asset ya casan.
  - Un global de .data con nombre que la apunta (.4byte $LC...). Ese si es
    trabajo pendiente: cada uno es un `const char* Nombre = "...";` que falta
    escribir, y el nombre del simbolo viene regalado por el ELF.

Salida: tools/string_map.csv (cadena -> consumidor) y, con --globals,
tools/string_globals.csv (los punteros con nombre, listos para declarar).
"""
import csv
import json
import os
import re
import sys

ASM_ROOT = os.path.join('build', 'GOWE69', 'asm')
REPORT = os.path.join('build', 'GOWE69', 'report.json')
MANIFEST = os.path.join('port', 'zzdata', 'assets', 'manifest.json')
OUT_MAP = os.path.join('tools', 'string_map.csv')
OUT_GLOBALS = os.path.join('tools', 'string_globals.csv')

RE_FN = re.compile(r'^\.fn\s+([^,]+)')
RE_ENDFN = re.compile(r'^\.endfn')
RE_OBJ = re.compile(r'^\.obj\s+([^,]+)')
RE_ENDOBJ = re.compile(r'^\.endobj')
RE_STRING = re.compile(r'^\s*\.string\s+"(.*)"\s*$')
RE_REF = re.compile(r'(\$LC\d+)@(?:ha|l)\b')
RE_PTR = re.compile(r'^\s*\.4byte\s+(\$LC\d+)\s*$')
RE_ADDR = re.compile(r'^/\* ([0-9A-Fa-f]{8}) ')
RE_SECTION = re.compile(r'^# \.(\w+):')

BACKSLASH = chr(92)


def load_matched():
    """symbol -> matched_code_percent, desde el reporte de objdiff."""
    out = {}
    if not os.path.exists(REPORT):
        return out
    report = json.load(open(REPORT))
    for unit in report.get('units', []):
        for fn in unit.get('functions', []) or []:
            name = fn.get('name')
            if name:
                out[name] = float(fn.get('fuzzy_match_percent', 0) or 0)
    return out


def load_manifest_paths():
    """Los paths del manifiesto, normalizados, como diccionario de assets."""
    out = set()
    if not os.path.exists(MANIFEST):
        return out
    for path in json.load(open(MANIFEST)):
        norm = path.upper().replace('/', BACKSLASH)
        out.add(norm)
        out.add(norm.rsplit(BACKSLASH, 1)[-1])
    return out


def scan(path):
    """Recorre un .s y devuelve (cadenas, usos por funcion, punteros con nombre)."""
    strings = {}            # label -> texto
    fn_refs = []            # (funcion, label, direccion)
    ptr_refs = []           # (global, label, seccion)
    cur_fn = cur_obj = None
    section = ''
    for line in open(path, encoding='utf-8', errors='replace'):
        m = RE_SECTION.match(line)
        if m:
            section = m.group(1)
            continue
        m = RE_FN.match(line)
        if m:
            cur_fn = m.group(1).strip()
            continue
        if RE_ENDFN.match(line):
            cur_fn = None
            continue
        m = RE_OBJ.match(line)
        if m:
            cur_obj = m.group(1).strip()
            continue
        if RE_ENDOBJ.match(line):
            cur_obj = None
            continue
        m = RE_STRING.match(line)
        if m and cur_obj:
            strings.setdefault(cur_obj, m.group(1))
            continue
        m = RE_PTR.match(line)
        if m and cur_obj and not cur_obj.startswith('$LC'):
            ptr_refs.append((cur_obj, m.group(1), section))
            continue
        if cur_fn:
            m = RE_REF.search(line)
            if m:
                addr = RE_ADDR.match(line)
                fn_refs.append((cur_fn, m.group(1), addr.group(1) if addr else ''))
    return strings, fn_refs, ptr_refs


def unescape(text):
    """El .s escapa la barra; el manifiesto no."""
    return text.replace(BACKSLASH * 2, BACKSLASH)


def main():
    want_globals = '--globals' in sys.argv
    matched = load_matched()
    assets = load_manifest_paths()

    map_rows, global_rows = [], []
    for root, _dirs, files in os.walk(ASM_ROOT):
        for fname in sorted(files):
            if not fname.endswith('.s'):
                continue
            full = os.path.join(root, fname)
            strings, fn_refs, ptr_refs = scan(full)
            if not strings:
                continue
            unit = os.path.relpath(full, ASM_ROOT).replace(os.sep, '/')[:-2]

            seen = set()
            for fn, label, addr in fn_refs:
                text = strings.get(label)
                if text is None or (fn, label) in seen:
                    continue
                seen.add((fn, label))
                pct = matched.get(fn)
                norm = unescape(text).upper()
                map_rows.append({
                    'unit': unit,
                    'consumer': fn,
                    'kind': 'function',
                    'matched': '' if pct is None else '%.1f' % pct,
                    'is_asset': '1' if norm in assets or norm.rsplit(BACKSLASH, 1)[-1] in assets else '',
                    'string': text,
                    'addr': addr,
                })

            for name, label, section in ptr_refs:
                text = strings.get(label)
                if text is None:
                    continue
                norm = unescape(text).upper()
                is_asset = norm in assets or norm.rsplit(BACKSLASH, 1)[-1] in assets
                row = {
                    'unit': unit,
                    'consumer': name,
                    'kind': 'global',
                    'matched': '',
                    'is_asset': '1' if is_asset else '',
                    'string': text,
                    'addr': '',
                }
                map_rows.append(row)
                global_rows.append({
                    'unit': unit,
                    'symbol': name,
                    'section': section,
                    'is_asset': row['is_asset'],
                    'value': unescape(text),
                    'declaration': 'const char* %s = "%s";' % (name, text),
                })

    with open(OUT_MAP, 'w', newline='', encoding='utf-8') as fh:
        w = csv.DictWriter(fh, fieldnames=['unit', 'consumer', 'kind', 'matched', 'is_asset', 'string', 'addr'])
        w.writeheader()
        w.writerows(map_rows)

    fn_rows = [r for r in map_rows if r['kind'] == 'function']
    pending = [r for r in fn_rows if r['matched'] in ('', '0.0')]
    print('cadenas ligadas a una funcion : %d' % len(fn_rows))
    print('  en funciones sin casar      : %d (%d funciones)'
          % (len(pending), len({r['consumer'] for r in pending})))
    print('punteros con nombre en datos  : %d (%d de assets)'
          % (len(global_rows), sum(1 for r in global_rows if r['is_asset'])))
    print('-> %s' % OUT_MAP)

    if want_globals:
        with open(OUT_GLOBALS, 'w', newline='', encoding='utf-8') as fh:
            w = csv.DictWriter(fh, fieldnames=['unit', 'symbol', 'section', 'is_asset', 'value', 'declaration'])
            w.writeheader()
            w.writerows(sorted(global_rows, key=lambda r: (r['unit'], r['symbol'])))
        print('-> %s' % OUT_GLOBALS)


if __name__ == '__main__':
    main()
