#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""dwbody.py -- diff del CUERPO DWARF (locales + ARBOL DE INLINES + bloques) del
ORIGINAL contra el nuestro, funcion a funcion.

`regmap.py` compara locales y REGISTROS. Lo que no mira es el **arbol de
expansiones inline**, y ahi esta la mitad de los near-miss: un `IsEmpty()` que
escribimos como `GetHead() != EndOfList()`, tres `m[i][j]` de mas, un
`UMath::Dot` que el original no usa, un `Reset()` que falta, tres `goto` que el
original no tiene (salen como `// Labels`). Todo eso es invisible para regmap y
para objdiff, y salta a la vista enfrentando los dos volcados DWARF.

En una sola pasada sobre 32 near-miss de zWorld/zWorld2/zSpeech destapo
diferencias ESTRUCTURALES en 13 de ellos.

    python scripts/dwbody.py zWorld  "CarRenderInfo::DrawAmbientShadow"
    python scripts/dwbody.py zSpeech "StrategyFlow::ReqBackup" both
    python scripts/dwbody.py zWorld2 "WRoadNav::"              list

  diff (por defecto)  unified diff sin las lineas `// Range:`
  both                los dos cuerpos enteros, con rangos (util para ver que
                      inlines tienen rango CERO: existen pero no emiten codigo)
  list                que firmas casan con el patron a cada lado

El lado nuestro sale de `build/regmap/our_<unidad>_*.nothpp`, que genera
`regmap.py`; si no existe, pasa antes `python scripts/regmap.py <unidad> --list`.

COMO LEERLO
  - linea `-` = SOLO en el original;  `+` = SOLO nuestra.
  - `// Labels` en el lado nuestro y no en el original = tenemos `goto` que el
    original no tiene.
  - un inline de mas en nuestro lado = estamos llamando a algo que el original
    no llamaba ahi (aunque el codigo muera despues: GCC lo apunta igual).
  - un inline de menos = nos falta esa capa; a veces basta un envoltorio
    (`OrthoInverseInline`) para recuperar el reparto de registros.
  - una local de mas/de menos manda SIEMPRE por delante de los registros.
"""
import difflib
import glob
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
ORIG = os.path.join(ROOT, 'symbols', 'mw_dwarfdump.nothpp')


def bodies(path):
    """{firma: [cuerpo, ...]} de todas las funciones de un volcado .nothpp."""
    txt = open(path, encoding='utf-8', errors='replace').read().split('\n')
    out = {}
    i = 0
    while i < len(txt):
        line = txt[i]
        # cabecera de funcion: en columna 0, con parentesis y abriendo llave
        if (line and not line.startswith((' ', '\t', '}', '/', '#'))
                and line.rstrip().endswith('{') and '(' in line):
            j, depth, buf = i, 0, []
            while j < len(txt):
                buf.append(txt[j])
                depth += txt[j].count('{') - txt[j].count('}')
                if depth <= 0 and j > i:
                    break
                j += 1
            out.setdefault(line.strip(), []).append('\n'.join(buf))
            i = j + 1
            continue
        i += 1
    return out


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 1
    unit, name = sys.argv[1], sys.argv[2]
    mode = sys.argv[3] if len(sys.argv) > 3 else 'diff'

    o = bodies(ORIG)
    cands = sorted(glob.glob(os.path.join(ROOT, 'build', 'regmap',
                                          'our_%s_*.nothpp' % unit)))
    if not cands:
        print('no hay volcado nuestro de %s; lanza antes:\n'
              '  python scripts/regmap.py %s --list' % (unit, unit))
        return 1
    nuestro = cands[-1]
    # EL LADO NUESTRO ES UNA CACHE, y solo la regenera
    #     regmap.py <unidad> "<Clase::Func>" --ours
    # NI dwbody NI `regmap.py --list` la refrescan --`--list` solo lista las del
    # original--. Ha enganado TRES VECES: una cache de un dia enseñaba una local
    # que ya no existe en el arbol Y ESCONDIA un hallazgo real. Por eso la fecha
    # se imprime SIEMPRE, no solo cuando se puede probar que esta rancia.
    import time
    print('# volcado nuestro: %s  (%s)'
          % (os.path.basename(nuestro), time.ctime(os.path.getmtime(nuestro))))
    obj = os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep',
                       'SourceLists', unit + '.o')
    if os.path.exists(obj) and os.path.getmtime(nuestro) < os.path.getmtime(obj):
        print('*** AVISO: el volcado es MAS VIEJO que el .o de la unidad.')
        print('***   regeneralo:  python scripts/regmap.py %s "<Clase::Func>" --ours'
              % unit)
    n = bodies(nuestro)

    ho = [k for k in o if name in k]
    hn = [k for k in n if name in k]
    if mode == 'list':
        print('ORIGINAL:')
        for k in ho:
            print('  ', k)
        print('NUESTRO:')
        for k in hn:
            print('  ', k)
        return 0
    if not ho:
        print('no encontrada en el ORIGINAL (prueba `list`)')
        return 1
    if not hn:
        print('no encontrada en el NUESTRO (prueba `list`)')
        print(o[ho[0]][0])
        return 1

    # AMBIGUEDAD: bodies() agrupa por firma y puede haber VARIAS firmas que casen
    # el patron y VARIOS cuerpos con la misma firma. Coger el primero EN SILENCIO
    # hizo que se comparase el __static_initialization_and_destruction_0 de OTRA
    # UNIDAD --existe en las 34-- durante tres rondas. Ahora se dice y se elige.
    def elegir(lado, hits, mapa, sel):
        cuerpos = [c for k in hits for c in mapa[k]]
        if len(cuerpos) == 1:
            return cuerpos[0]          # sin ambiguedad: el selector sobra
        if sel:
            m = [c for c in cuerpos if sel.lower() in c.lower()]
            if len(m) == 1:
                return m[0]
            print('el selector %r casa %d cuerpos en el lado %s'
                  % (sel, len(m), lado))
            cuerpos = m or cuerpos
        if len(cuerpos) == 1:
            return cuerpos[0]
        print('*** AMBIGUO en el lado %s: %d cuerpos casan %r'
              % (lado, len(cuerpos), name))
        for c in cuerpos[:12]:
            r = re.search(r'// Range:[ 	]*(\S+)', c)
            print('***   %-18s %s' % (r.group(1) if r else '(sin rango)',
                                      c.split(chr(10))[0].strip()[:88]))
        print('*** afina el patron, o pasa una subcadena discriminante como',
              'cuarto argumento (p.ej. una direccion del rango).')
        return None

    sel = sys.argv[4] if len(sys.argv) > 4 else None
    ca = elegir('ORIGINAL', ho, o, sel)
    cb = elegir('NUESTRO', hn, n, sel)
    if ca is None or cb is None:
        return 1
    a = ca.split(chr(10))
    b = cb.split(chr(10))
    if mode == 'both':
        print('===== ORIGINAL =====')
        print('\n'.join(a))
        print('===== NUESTRO  =====')
        print('\n'.join(b))
        return 0
    # los rangos son direcciones absolutas en el original y relativas en el
    # nuestro: comparandolos no se ve mas que ruido.
    strip = lambda L: [re.sub(r'// Range:.*', '', x).rstrip() for x in L if x.strip()]
    for line in difflib.unified_diff(strip(a), strip(b), 'orig', 'ours',
                                     lineterm='', n=2):
        print(line)
    return 0


if __name__ == '__main__':
    sys.exit(main())
