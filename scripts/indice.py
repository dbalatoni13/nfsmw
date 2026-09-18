#!/usr/bin/env python3
"""indice.py -- regenera `docs/HERRAMIENTAS.md` leyendo los docstrings.

El indice escrito a mano se quedo en **40 de las 110 herramientas reales**, y
entre las 70 que faltaban estaban justo las que deciden el frente: `linkdelta`,
`permorden`, `promote`, `triaje`, `fncmp`, `refs`, `dolwhere`. Eso tiene un
coste medido: la r48 entera se construyo comparando OBJETOS mientras
`linkdelta.py` llevaba dos dias en el arbol con un docstring que avisaba de ese
error exacto. Un indice a mano vuelve a quedarse rancio a la ronda siguiente;
uno generado, no.

Lee la primera linea del docstring de cada `scripts/*.py`, los agrupa por
familia y escribe la tabla. Lo que NO tenga docstring sale listado aparte, que
es la lista de deberes.

    python scripts/indice.py            # reescribe docs/HERRAMIENTAS.md
    python scripts/indice.py --check    # solo dice si esta rancio (para CI)
"""
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEST = os.path.join(ROOT, 'docs', 'HERRAMIENTAS.md')

# Las sondas de un solo uso de rondas pasadas ya no viven aqui: se sacaron del
# repo en b50c9d74. El filtro se queda por si vuelve a colarse alguna, pero
# acotado al prefijo que de verdad las marca -- `mn_`, `s_z` y `zcmp` NO son
# sondas (mn_* barre formas y flags, y a zcmp.py y s_zlua.py los cita la
# memoria), y clasificarlas asi era mentir sobre herramientas vivas.
DESECHABLE = re.compile(r'^(agent_|jf\d|fe\d\d_|c\d\d|s_wld|sweep_wld|zae_|_)')

FAMILIAS = [
    ('Medir el frente', """estado censo measure linkdelta promodist promomap promopred
        promote trypromo report pctsnap zstat"""),
    ('Comparar codigo', """fncmp fndiff audit diffsym mnemo arboljump triaje triage regmap
        symdiff symtabdiff tamfn stripped stripmap"""),
    ('Datos, .rodata y el pool', """claimdata claimlbl claimrange lcfix lcmap lcpool deadstr
        deadlink dupdata dupstr genrodata rodata rodatagaps strseq pool2lit datadiff
        datacmp seccdiff dolrod"""),
    ('El enlace y el DOL', """dolwhere dolcmp refs extrasym ghostref missingcalls phantom
        relocfantasma residuo checksplits rangechk gapchk"""),
    ('Orden de emision', """permorden vtord vtable_audit globalini stmtorder textorder
        missinline"""),
    ('El compilador por dentro', """rtldump schedtrace lreg alloc regalloc deadreg pines
        sprobe swsweep casediff"""),
    ('El original como fuente', """dwarfmap dwbody dwblocks lmap ps2fn ps2map zdump derotate
        x360ref"""),
    ('Construir y mantener', """build_direct bench frozen scratchclean mangfix indice"""),
]


def docstring(p):
    try:
        s = io.open(p, encoding='utf-8', errors='replace').read()
    except Exception:
        return ''
    m = re.search(r'"""(.*?)(?:\n|""")', s, re.S)
    if not m:
        return ''
    d = m.group(1).strip()
    base = os.path.basename(p)
    for sep in (' -- ', ' - ', ' — '):
        if d.startswith(base + sep):
            d = d[len(base) + len(sep):]
    if d.startswith(base[:-3] + ':'):
        d = d[len(base) - 3 + 1:].strip()
    return d.strip()


def genera():
    tools = {}
    for f in sorted(os.listdir(os.path.join(ROOT, 'scripts'))):
        if not f.endswith('.py') or DESECHABLE.match(f):
            continue
        tools[f[:-3]] = docstring(os.path.join(ROOT, 'scripts', f))
    desechables = [f for f in sorted(os.listdir(os.path.join(ROOT, 'scripts')))
                   if f.endswith('.py') and DESECHABLE.match(f)]

    L = []
    L.append('# Herramientas')
    L.append('')
    L.append('**Generado por `scripts/indice.py`. No editar a mano: se regenera.**')
    L.append('')
    L.append('%d herramientas en `scripts/`, mas %d sondas de un solo uso de rondas '
             'pasadas que no son herramientas (ver el final).'
             % (len(tools), len(desechables)))
    L.append('')
    L.append('La descripcion es la primera linea del docstring de cada una. Si una fila '
             'dice `(sin docstring)`, esa herramienta no se puede usar sin abrirla: es '
             'un deber pendiente, no una herramienta.')
    L.append('')

    puestas = set()
    for titulo, nombres in FAMILIAS:
        ns = [n for n in nombres.split() if n in tools]
        if not ns:
            continue
        L.append('## %s' % titulo)
        L.append('')
        L.append('| herramienta | que hace |')
        L.append('|---|---|')
        for n in ns:
            puestas.add(n)
            d = tools[n].replace('|', '\\|') or '*(sin docstring)*'
            L.append('| `%s.py` | %s |' % (n, d))
        L.append('')

    resto = [n for n in sorted(tools) if n not in puestas]
    if resto:
        L.append('## Sin clasificar')
        L.append('')
        L.append('Nadie las ha metido en una familia de `indice.py`. Si usas una y '
                 'resulta util, anadela a `FAMILIAS` para que se encuentre.')
        L.append('')
        L.append('| herramienta | que hace |')
        L.append('|---|---|')
        for n in resto:
            d = tools[n].replace('|', '\\|') or '*(sin docstring)*'
            L.append('| `%s.py` | %s |' % (n, d))
        L.append('')

    sin = [n for n in sorted(tools) if not tools[n]]
    if sin:
        L.append('## Deberes: %d sin docstring' % len(sin))
        L.append('')
        L.append(', '.join('`%s.py`' % n for n in sin))
        L.append('')

    L.append('## Sondas de un solo uso (%d)' % len(desechables))
    L.append('')
    L.append('Guiones que un agente escribio para una ronda concreta y nadie borro. '
             'No son herramientas y no se mantienen; estan aqui solo para que se sepa '
             'que no hay que leerlas.')
    L.append('')
    L.append(', '.join('`%s`' % f for f in desechables))
    L.append('')
    return '\n'.join(L)


def main():
    nuevo = genera()
    if '--check' in sys.argv:
        viejo = io.open(DEST, encoding='utf-8', errors='replace').read() if os.path.exists(DEST) else ''
        if viejo.replace('\r\n', '\n') == nuevo:
            print('HERRAMIENTAS.md al dia')
            return 0
        print('HERRAMIENTAS.md RANCIO: corre `python scripts/indice.py`')
        return 1
    io.open(DEST, 'w', encoding='utf-8', newline='\n').write(nuevo)
    print('escrito %s (%d B)' % (DEST, len(nuevo)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
