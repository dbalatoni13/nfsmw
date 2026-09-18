#!/usr/bin/env python3
"""pines.py -- censo de los `register X asm("rN")` y si estan donde importan.

Un pin de registro es el ultimo recurso del proyecto, y DOS VECES ha resultado
ser **la causa** de que una funcion no casara:

    CarRender.cpp   la barrera hacia falta, el `asm("r8")` no; quitarlo cerro
                    la funcion (ronda 20, +1.068 B)
    GTrigger.cpp    el pin Y la barrera juntos daban el objeto IDENTICO a no
                    poner nada; quitar solo el registro duro era PEOR, que es
                    lo que hacia pensar que servia (ronda 21)

Por eso el brief manda quitar el pin lo primero cuando una funcion con pin no
casa. Pero un pin dentro de una unidad que YA esta entera al 100% no hay que
tocarlo, y probarlo cuesta un ensayo a cada agente. Esto separa unos de otros.

    python scripts/pines.py            # los que estan en unidades sucias
    python scripts/pines.py --todos    # tambien los de unidades ya al 100%
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RE_PIN = re.compile('[^a-zA-Z_]asm[ \t]*[(][ \t]*"(r[0-9]+|f[0-9]+)"[ \t]*[)]')
RE_INC = re.compile('^[ \t]*#[ \t]*include[ \t]*"([^"]+)"', re.M)
RE_DEF = re.compile('^[A-Za-z_][A-Za-z0-9_:<>,*& \t]*[A-Za-z0-9_:~]+[ \t]*[(][^;]*$')


def unidades():
    """{ruta de fuente sin extension -> unidad}.

    OJO: build.ninja NO sirve para esto. Las SourceLists compilan UN .cpp que
    hace #include de los demas, asi que ninja no conoce `Gameplay/GTrigger.cpp`
    y la primera version de este script dio "limpio" a 44 de los 45 ficheros
    con pin. Se resuelve por los #include de cada SourceList, y el middleware
    por la existencia del .s del troceador.
    """
    out = {}
    sl = os.path.join(ROOT, 'src', 'Speed', 'Indep', 'SourceLists')
    if os.path.isdir(sl):
        for f in sorted(os.listdir(sl)):
            if not f.endswith('.cpp'):
                continue
            u = 'Speed/Indep/SourceLists/' + f[:-4]
            txt = open(os.path.join(sl, f), encoding='utf-8', errors='replace').read()
            for m in RE_INC.finditer(txt):
                out.setdefault(os.path.splitext(m.group(1))[0], u)
    asm = os.path.join(ROOT, 'build', 'GOWE69', 'asm')
    for dp, _, fs in os.walk(asm):
        for f in fs:
            if not f.endswith('.s'):
                continue
            rel = os.path.relpath(os.path.join(dp, f), asm).replace(os.sep, '/')[:-2]
            out.setdefault(rel, rel)
    return out


def pcts():
    """{unidad -> {funcion: pct}} del report.json."""
    p = os.path.join(ROOT, 'build', 'GOWE69', 'report.json')
    if not os.path.exists(p):
        return {}
    d = json.load(open(p, encoding='utf-8'))
    out = {}
    for u in d['units']:
        n = u['name']
        n = n[5:] if n.startswith('main/') else n
        out[n] = {f['name']: f.get('fuzzy_match_percent', 0.0)
                  for f in (u.get('functions') or [])}
    return out


def mangla(firma):
    """Prefijos de nombre manglado (GCC 2.9) que puede tener esta firma.

    report.json NO trae `demangled_name`, asi que sin esto solo se puede decir
    "la unidad esta sucia" --que es demasiado grueso: la mayoria de los pines de
    una unidad sucia estan en funciones que YA casan-- y cada agente acaba
    gastando un ensayo en comprobarlo.
    """
    m = re.search('([A-Za-z_][A-Za-z0-9_]*)[ 	]*::[ 	]*(~?[A-Za-z_][A-Za-z0-9_]*)[ 	]*[(]', firma)
    if m:
        cls, met = m.group(1), m.group(2)
        suf = '%d%s' % (len(cls), cls)
        if met == cls:
            return ['__' + suf]                 # constructor
        if met == '~' + cls:
            return ['_._' + suf]                # destructor
        return [met + '__' + suf, met + '__C' + suf,
                met + '__Q', met + '__H']       # metodo, y las formas anidada/plantilla
    m = re.search('([A-Za-z_][A-Za-z0-9_]*)[ 	]*[(]', firma)
    if m:
        return [m.group(1) + '__', m.group(1) + '$']
    return []


def funcion_de(lineas, i):
    """El nombre de fuente de la funcion que contiene la linea i."""
    for k in range(i, -1, -1):
        t = lineas[k].rstrip()
        if t and t[0] not in ' \t#/*}' and RE_DEF.match(t):
            return t.strip()[:88]
    return '?'


def main():
    todos = '--todos' in sys.argv
    umap = unidades()
    P = pcts()
    filas = []
    for dp, _, fs in os.walk(os.path.join(ROOT, 'src')):
        for f in fs:
            if not f.endswith(('.c', '.cpp', '.h', '.hpp')):
                continue
            fp = os.path.join(dp, f)
            try:
                ls = open(fp, encoding='utf-8', errors='replace').read().split('\n')
            except IOError:
                continue
            for i, ln in enumerate(ls):
                if not RE_PIN.search(' ' + ln) or ln.lstrip().startswith(('*', '//', '/*')):
                    continue
                rel = os.path.relpath(fp, os.path.join(ROOT, 'src')).replace(os.sep, '/')
                u = umap.get(os.path.splitext(rel)[0], '?')
                fn = P.get(u, {})
                firma = funcion_de(ls, i)
                pref = mangla(firma)
                cands = [v for k, v in fn.items()
                         if any(k.startswith(x) for x in pref)] if pref else []
                if not fn:
                    est, pct = '?', None       # sin datos NO es lo mismo que limpia
                elif cands:
                    pct = min(cands)
                    est = 'sucia' if pct < 100.0 else 'limpia'
                elif any(v < 100.0 for v in fn.values()):
                    est, pct = 'dudosa', None  # no se cual es su funcion
                else:
                    est, pct = 'limpia', 100.0
                filas.append((est, u, rel, i + 1, firma, pct))
    orden = {'sucia': 0, 'dudosa': 1, '?': 2, 'limpia': 3}
    filas.sort(key=lambda x: (orden[x[0]], x[5] if x[5] is not None else 999, x[2], x[3]))
    print('PINES EN FUNCIONES QUE NO CASAN - quitalos LO PRIMERO:')
    n = 0
    for est, u, rel, ln, fn, pct in filas:
        if est != 'sucia':
            continue
        n += 1
        print('  %8.4f%%  %-52s %s' % (pct, '%s:%d' % (rel, ln), u))
        print('              %s' % fn)
    print('  --> %d pines en %d ficheros' %
          (n, len({x[2] for x in filas if x[0] == 'sucia'})))
    print()
    du = [x for x in filas if x[0] == 'dudosa']
    if du:
        print('No se a que funcion pertenecen, y su unidad esta sucia: %d pines' % len(du))
        for est, u, rel, ln, fn, pct in du:
            print('    %-52s %s' % ('%s:%d' % (rel, ln), fn[:60]))
    print()
    print('En funciones que YA casan al 100%%, NO tocar: %d pines' %
          sum(1 for x in filas if x[0] == 'limpia'))
    sd = [x for x in filas if x[0] == '?']
    if sd:
        print('SIN DATOS en report.json, no se puede decidir: %d pines' % len(sd))
        for est, u, rel, ln, fn, pct in sd:
            print('    %s:%d  [%s]' % (rel, ln, u))
    if todos:
        print()
        for est, u, rel, ln, fn, pct in filas:
            if est == 'limpia':
                print('  limpia  %s:%d  [%s]' % (rel, ln, u))


if __name__ == '__main__':
    main()
