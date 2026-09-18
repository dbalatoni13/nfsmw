#!/usr/bin/env python3
"""serie.py -- la tasa REAL de cierre por ronda, reconstruida del historial.

En la r53 dije que el pronostico "no se puede estrechar con honestidad", y la
razon que di era que el riesgo por funcion-ronda salia de CUATRO cierres y el
intervalo de Poisson sobre cuatro eventos va de 1 a 10.

Esa cuenta usaba cinco rondas porque era lo que tenia a mano. El historial
entero estaba escrito y sin leer: cada `rNN-cierre.md` trae una tabla
`antes | despues` con las funciones exactas y las unidades completas, y cada
`brief-rNN.md` publica el estado con el que ARRANCA la ronda -- que es el
cierre de la anterior. Entre las dos fuentes sale la serie completa desde la
r37, diecisiete transiciones en vez de cinco.

Y la serie no es estacionaria, que es lo que de verdad importa: no se puede
extrapolar su pendiente porque tiene tres regimenes distintos, y el ultimo va a
cero. Ver `docs/analisis/r54-jf-serie.md`.

    python scripts/serie.py           # la serie, los deltas y los regimenes
    python scripts/serie.py --crudo   # ademas, de que fichero sale cada cifra

EL CONTROL: la ultima fila TIENE que coincidir con `build/GOWE69/report.json`.
Si no coincide, la herramienta lo grita y no te deja usarla -- un historial que
no reproduce el presente no sirve para pronosticar nada.
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

ANA = os.path.join('docs', 'analisis')
TOTAL_FN = 18432

# "18.373" -> 18373.  El punto es separador de millares en todo el proyecto.
NUM = re.compile(r'(\d[\d.]*)\s*/\s*18\.432')
# La celda del "despues" a veces se escribe SIN denominador (`**18.378**`), asi
# que sobre la linea etiquetada hay que buscar cualquier 18.3xx/18.4xx y
# descartar el 18.432, que es el total. Sin esto la serie leia el "antes" y se
# comia los cierres de tres rondas enteras.
FN_SUELTA = re.compile(r'18\.[34]\d{2}')
UDS = re.compile(r'(\d{3})\s*/\s*(6[0-9]{2})')
FALTAN = re.compile(r'faltan\s+(\d+)\s+funciones')


def entero(s):
    return int(s.replace('.', ''))


def de_cierre(path):
    """Un rNN-cierre.md -> (funciones, unidades) al CERRAR la ronda.

    Los cierres vienen en TRES formatos distintos -- tabla `antes | despues`
    (r38-r41, r46+), tabla de dos columnas con solo el valor (r43, r44) y un
    bullet en prosa (r42) --. La regla que vale para los tres: **de la linea
    etiquetada, la ULTIMA cifra es el estado**, porque el `antes` siempre va
    antes. La unica excepcion es `igual`, que es como el proyecto escribe
    "no se movio": entonces la ultima cifra esta en la celda anterior.
    """
    fn = uds = None
    for linea in open(path, encoding='utf-8', errors='replace'):
        etq = linea.lower()
        # PRIMERA linea gana, en las dos. La tabla de progreso oficial va
        # siempre arriba; mas abajo varios cierres citan un "censo directo
        # independiente" que mide con OTRO denominador (3.946.204 B) y da otra
        # cifra. Tomar la ultima mezclaba los dos censos y sacaba deltas
        # NEGATIVOS -- que es justo el sintoma que lo delato.
        if 'funciones exactas' in etq and fn is None:
            ms = [x for x in FN_SUELTA.findall(linea) if x != '18.432']
            if ms:
                fn = entero(ms[-1])
        if 'unidades completas' in etq or 'linked' in etq:
            ms = UDS.findall(linea)
            if ms and uds is None:
                uds = (int(ms[-1][0]), int(ms[-1][1]))
    return fn, uds


def de_brief(path):
    """Un brief-rNN.md -> el estado con el que ARRANCA la ronda N.

    Que es, por definicion, el cierre de la N-1. Los briefs viejos dan las
    funciones como `18.373 / 18.432`; los nuevos, como `faltan 32 funciones`.
    """
    s = open(path, encoding='utf-8', errors='replace').read()
    m = NUM.search(s)
    fn = entero(m.group(1)) if m else None
    if fn is None:
        m = FALTAN.search(s)
        if m:
            n = int(m.group(1))
            # CAMBIO DE DEFINICION en la r54: hasta la r53 los documentos
            # contaban las 29 entradas de `report.json`; desde que se descubrio
            # que `pad_00_8000348C_init` son 20 B de relleno del DOL, el brief
            # dice "28 funciones REALES". Sin normalizar, la serie se apunta un
            # cierre que nadie hizo. Toda la serie va en entradas de report.json.
            if 'reales' in m.group(0) or re.search(
                    r'faltan\s+%d\s+funciones\s+reales' % n, s):
                n += 1
            fn = TOTAL_FN - n
    m = UDS.search(s)
    uds = (int(m.group(1)), int(m.group(2))) if m else None
    return fn, uds


def encargos():
    """-> {ronda: cuantos informes de agente}. La unidad de trabajo REAL.

    La "ronda" no mide nada: la r42 fueron 2 informes y la r52 fueron 10, y la
    r46 llevaba dentro seis iteraciones -- entre su `cierre` y el brief de la
    r47 hay cinco commits de trabajo, uno de ellos "El SDK entero al 100%".
    Pronosticar "en N rondas" es medir con una regla que cambia de largo.

    Cuenta informes `rNN-*.md` sin el cierre. Es un PROXY: un agente que no
    entrego informe no se ve, y un informe partido en dos cuenta doble.
    """
    out = {}
    for f in os.listdir(ANA):
        m = re.match(r'r(\d+)-(.+)\.md$', f)
        if m and m.group(2) != 'cierre':
            out[int(m.group(1))] = out.get(int(m.group(1)), 0) + 1
    return out


def recoge():
    """-> ({ronda: {'fn':n, 'uds':(a,b), 'src':...}}, [conflictos])"""
    est, conf = {}, []

    def pon(n, fn, uds, src):
        d = est.setdefault(n, {'fn': None, 'uds': None, 'src': []})
        for k, v in (('fn', fn), ('uds', uds)):
            if v is None:
                continue
            if d[k] is not None and d[k] != v:
                conf.append('r%d %s: %s dice %s, %s decia %s'
                            % (n, k, src, v, '/'.join(d['src']), d[k]))
                continue
            d[k] = v
        d['src'].append(src)

    for f in sorted(os.listdir(ANA)):
        m = re.match(r'r(\d+)-cierre\.md$', f)
        if m:
            fn, uds = de_cierre(os.path.join(ANA, f))
            pon(int(m.group(1)), fn, uds, f)
            continue
        # brief-r36b/c/... son sub-rondas de la 36: no son transiciones propias
        m = re.match(r'brief-r(\d+)\.md$', f)
        if m:
            n = int(m.group(1))
            fn, uds = de_brief(os.path.join(ANA, f))
            pon(n - 1, fn, uds, f)          # el brief de N es el cierre de N-1
    return est, conf


def control(est):
    """La ultima fila contra report.json. Si no cuadra, la serie no vale."""
    p = 'build/GOWE69/report.json'
    if not os.path.exists(p):
        return None, ['no hay report.json: el control no se puede correr']
    # UN CONTROL CONTRA UNA REFERENCIA RANCIA NO ES UN CONTROL.
    # `report.json` solo se regenera construyendo, y `configure.py` cambia en
    # cuanto se promociona una unidad. Si el informe es mas viejo, la ultima
    # fila de la serie coincide con el porque LOS DOS estan atrasados, y el
    # control pasa sin comprobar nada. Paso en la r59: el control cantaba OK con
    # 518 unidades tres horas despues de que zGameModes promocionara a 519.
    trep = os.path.getmtime(p)
    rancio = [q for q in ('configure.py', 'config/GOWE69/keep.lst',
                          'config/GOWE69/splits.txt')
              if os.path.exists(q) and os.path.getmtime(q) > trep]
    if rancio:
        return None, ['report.json es MAS VIEJO que %s.' % ', '.join(rancio),
                      'El control NO VALE: comparas la serie contra una referencia',
                      'atrasada, y coincide porque los DOS estan igual de viejos.',
                      'Reconstruye antes de fiarte de la ultima fila.']
    r = json.load(open(p, encoding='utf-8'))
    abiertas = [f.get('name', '?') for u in r.get('units', [])
                for f in (u.get('functions') or [])
                if float(f.get('fuzzy_match_percent', 0) or 0) < 100.0]
    # Una de esas entradas, `pad_00_8000348C_init`, NO es una funcion: son los
    # 20 B con que el contenedor DOL alinea `.init` a 32 B, ceros que nuestro
    # DOL ya emite. `objdiff` los cuenta como codigo porque viven en seccion de
    # texto. La serie va en ESTA unidad -- entradas de report.json -- y quien
    # normaliza es `de_brief`, para que el cambio de definicion de la r54 no se
    # apunte un cierre que nadie hizo.
    hoy_fn = TOTAL_FN - len(abiertas)
    hoy_ud = sum(1 for u in r.get('units', [])
                 if u.get('metadata', {}).get('complete'))
    ult = max(est)
    d = est[ult]
    fallos = []
    if d['fn'] is not None and d['fn'] != hoy_fn:
        fallos.append('funciones: la serie cierra en %d, report.json dice %d'
                      % (d['fn'], hoy_fn))
    if d['uds'] is not None and d['uds'][0] != hoy_ud:
        fallos.append('unidades: la serie cierra en %d, report.json dice %d'
                      % (d['uds'][0], hoy_ud))
    return (hoy_fn, hoy_ud), fallos


def main():
    crudo = '--crudo' in sys.argv
    est, conf = recoge()
    if not est:
        sys.exit('no encuentro ni cierres ni briefs en %s' % ANA)

    hoy, fallos = control(est)
    print('=' * 76)
    if fallos:
        print('LA SERIE NO REPRODUCE EL PRESENTE -- no la uses para pronosticar:')
        for x in fallos:
            print('   ' + x)
    elif hoy:
        print('CONTROL OK: la ultima fila coincide con report.json (%d fn, %d uds)' % hoy)
    else:
        print('CONTROL NO CORRIDO: %s' % fallos)
    print('=' * 76)
    print()

    rondas = sorted(est)
    print('%-6s %10s %6s   %10s %6s   %s'
          % ('ronda', 'funciones', 'd', 'unidades', 'd', 'de donde'))
    pfn = puds = ptot = None
    dfn, duds = {}, {}
    for n in rondas:
        d = est[n]
        fn, uds = d['fn'], d['uds']
        a = b = ''
        if fn is not None and pfn is not None:
            dfn[n] = fn - pfn
            a = '%+d' % dfn[n]
        if uds is not None and puds is not None:
            duds[n] = uds[0] - puds
            b = '%+d' % duds[n]
        # El DENOMINADOR se mueve (616..619): el troceador anade y quita
        # entradas al censo. Un delta de `linked` medido a traves de un cambio
        # de censo NO es comparable con los demas, y hay que verlo.
        nota = ''
        if uds and ptot is not None and uds[1] != ptot:
            nota = '  <- censo %d -> %d: el delta de al lado no es comparable' % (ptot, uds[1])
        print('r%-5d %10s %6s   %10s %6s   %s%s'
              % (n,
                 '{:,}'.format(fn) if fn is not None else '?', a,
                 ('%d/%d' % uds) if uds else '?', b,
                 ', '.join(d['src']) if crudo else '', nota))
        if fn is not None:
            pfn = fn
        if uds is not None:
            puds, ptot = uds

    if conf:
        print()
        print('CONFLICTOS entre fuentes (%d) -- dos documentos dan cifras distintas:' % len(conf))
        for x in conf:
            print('   ' + x)
        print()
        print('   El de la r46 esta RESUELTO y gana el brief: entre el commit que anade')
        print('   `r46-cierre.md` (8853122e) y el que anade `brief-r47.md` (1b6cd105) hay')
        print('   cinco commits de trabajo -- uno es «El SDK entero al 100%: 98 de 98» --')
        print('   y otro se titula «iteraciones 2 a 6». El cierre se escribio a mitad de')
        print('   ronda. Es la prueba de que la ronda no es una unidad de trabajo.')

    # Los regimenes. No se inventan: se parten donde el proyecto cambio de
    # objetivo (brief-r31: "el objetivo pasa de matched a linked") y donde se
    # agoto el stock de unidades ya correctas.
    print()
    print('LA PENDIENTE NO SE PUEDE EXTRAPOLAR: son tres regimenes, no una tasa')
    enc = encargos()
    tramos = [('r37-r45  perseguir `matched`', 37, 45),
              ('r46-r52  drenar el atasco  ', 46, 52),
              ('r53-...  solo queda bloque A', 53, 99)]
    print('  %-28s %7s %7s %8s %8s' % ('', 'rondas', 'encarg', 'fn/enc', 'uds/enc'))
    for et, a, b in tramos:
        rs = [n for n in rondas if a <= n <= b and (n in dfn or n in duds)]
        if not rs:
            continue
        sf = sum(dfn.get(n, 0) for n in rs)
        su = sum(duds.get(n, 0) for n in rs)
        e = sum(enc.get(n, 0) for n in rs)
        print('  %-28s %7d %7d %8.2f %8.2f   (%+d fn, %+d uds)'
              % (et, len(rs), e, sf / e if e else 0, su / e if e else 0, sf, su))
    print()
    print('  El ENCARGO es la unidad honesta, no la ronda: la r42 fueron 2 informes')
    print('  y la r52 fueron 10. Y las medias siguen enganando en el tramo del medio,')
    print('  porque las promociones no llegaron de una en una: llegaron en cuatro')
    print('  ventanas de 6, 8, 8 y 12 unidades. Eso fue drenar un ATASCO, no un ritmo.')

    # Lo que queda, contra los dos ritmos que de verdad acotan
    hoy_fn, hoy_ud = hoy if hoy else (None, None)
    if hoy_fn:
        qf, qu = hoy_fn is not None and (TOTAL_FN - hoy_fn), 544 - hoy_ud
        print()
        print('QUEDAN %d entradas de funcion y %d unidades hasta el techo de 544.' % (qf, qu))
        for et, a, b in (('con el ritmo de TODA la serie', 37, 99),
                         ('con el ritmo de las dos ultimas', 52, 99)):
            rs = [n for n in rondas if a <= n <= b and (n in dfn or n in duds)]
            e = sum(enc.get(n, 0) for n in rs)
            sf = sum(dfn.get(n, 0) for n in rs)
            su = sum(duds.get(n, 0) for n in rs)
            print('  %-32s %s' % (et, (
                'fn: %.0f encargos   uds: %s' % (
                    qf * e / sf if sf else 0,
                    ('%.0f encargos' % (qu * e / su)) if su else
                    'CERO en %d encargos -> sin mecanismo no hay ritmo que extrapolar' % e))))
    return 0


if __name__ == '__main__':
    sys.exit(main())
