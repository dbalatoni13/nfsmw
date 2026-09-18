#!/usr/bin/env python3
"""lcfix.py -- vuelve a resolver por CONTENIDO los `$LC` que fuerza keep.lst.

`-strip-unused-data` se lleva `size & ~7` de cada simbolo muerto, y la unica
manera de salvar una cadena muerta que cc1plus emite y nadie referencia es
nombrarla en `keep.lst`. Pero el nombre que le da cc1plus es `$LC<n>`, y **n se
desplaza en cuanto se anade un literal ANTES en la unidad** -- una constante de
coma flotante nueva en un `.cpp` anterior basta. Cuando eso pasa la entrada
apunta a otra cosa, la cadena se estripa y el DOL se rompe EN SILENCIO:
`keepchk.py` la sigue dando por buena porque el simbolo existe.

Paso tres veces en la r36. La red es esta: en `keep.lst`, cada entrada `$LC`
lleva encima una directiva con la cadena que DEBE nombrar,

    # @lc zBWare "SlotPool::GetAllocatedSlots"
    zBWare.o:$LC243

y esto la vuelve a resolver contra NUESTRO objeto recien compilado.

    python scripts/lcfix.py            # comprueba y corrige
    python scripts/lcfix.py --check    # solo comprueba (rc=1 si hay desfase)
    python scripts/lcfix.py zSim zFe   # solo esas unidades

Y desde la r51 comprueba tambien los `$LC` puestos A FUEGO EN LA FUENTE, que
tienen el mismo problema y ninguna red: `QuickGame.cpp` lleva un
`.set lbl_80404864, $LC526` dentro de un `asm()`, y si alguien anade un literal
antes en zSim, `$LC526` pasa a ser otra constante y el alias apunta en silencio
al valor equivocado. Se declaran con una directiva al lado del `asm()`:

    // @lcsrc zSim $LC526 3f800000

--unidad, simbolo, y los bytes que DEBE tener en hexadecimal-- y esto verifica
que en nuestro objeto recien compilado ese `$LC` empieza por esos bytes. Aqui no
hay correccion automatica posible --el nombre esta en un `asm()`--: si no cuadra,
sale FALLO y hay que editar la fuente.
"""
import io
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KEEP = os.path.join(ROOT, 'config', 'GOWE69', 'keep.lst')


def rodata_y_simbolos(obj):
    """(bytes de .rodata, [(offset, nombre)]) del objeto."""
    d = open(obj, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, n, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(n)]
    stro = S[sx][4]
    nom = lambda o: d[stro + o:d.index(b'\0', stro + o)].decode()
    rod = [i for i, s in enumerate(S) if nom(s[0]) == '.rodata']
    if not rod:
        return b'', []
    rod = rod[0]
    blob = d[S[rod][4]:S[rod][4] + S[rod][5]]
    st = [s for s in S if s[1] == 2][0]
    strt = S[st[6]][4]
    sy = []
    for o in range(st[4], st[4] + st[5], 16):
        nm, val, sz, info, other, shn = struct.unpack('>IIIBBH', d[o:o + 16])
        if shn == rod:
            sy.append((val, d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1')))
    return blob, sorted(sy)


def revisa_fuente(cache, filtro):
    """Los `$LC` que la FUENTE nombra a fuego, declarados con `// @lcsrc`."""
    fallos, ok = [], 0
    pat = re.compile(r'@lcsrc\s+(\S+)\s+(\$LC\d+)\s+([0-9A-Fa-f]+)')
    for base, _, fs in os.walk(os.path.join(ROOT, 'src')):
        for f in fs:
            if not f.endswith(('.c', '.cpp', '.h', '.hpp')):
                continue
            ruta = os.path.join(base, f)
            try:
                txt = io.open(ruta, encoding='utf-8', errors='replace').read()
            except Exception:
                continue
            if '@lcsrc' not in txt:
                continue
            for unidad, sym, hexb in pat.findall(txt):
                if filtro and unidad not in filtro:
                    continue
                obj = os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep',
                                   'SourceLists', unidad + '.o')
                if not os.path.exists(obj):
                    fallos.append('%s: no esta compilado, no puedo comprobar %s'
                                  % (unidad, sym))
                    continue
                if obj not in cache:
                    cache[obj] = rodata_y_simbolos(obj)
                blob, sy = cache[obj]
                off = [o for o, nm in sy if nm == sym]
                if not off:
                    fallos.append('%s: %s no existe en el objeto, y %s lo nombra a fuego'
                                  % (unidad, sym, os.path.basename(ruta)))
                    continue
                quiero = bytes.fromhex(hexb)
                hay = blob[off[0]:off[0] + len(quiero)]
                if hay != quiero:
                    fallos.append('%s: %s vale %s y deberia valer %s -- %s apunta al '
                                  'literal equivocado'
                                  % (unidad, sym, hay.hex(), quiero.hex(),
                                     os.path.basename(ruta)))
                else:
                    ok += 1
    return fallos, ok


def main():
    solo_check = '--check' in sys.argv
    # Filtro por unidad. Sin el, `lcfix.py zEcstasy --check` ignoraba el nombre y
    # listaba los desfases de TODAS las unidades: un agente veia como suyos los de
    # otro y los perseguia. Ahora el argumento filtra de verdad.
    filtro = [a for a in sys.argv[1:] if not a.startswith('-')]
    s = io.open(KEEP, encoding='utf-8', newline='').read()
    fin = '\r\n' if '\r\n' in s else '\n'
    L = s.split(fin)
    cache = {}
    cambios, fallos = [], []
    for i, l in enumerate(L):
        m = re.match(r'#\s*@lc\s+(\S+)\s+"(.*)"\s*$', l)
        if not m or i + 1 >= len(L):
            continue
        unidad, cadena = m.group(1), m.group(2)
        if filtro and unidad not in filtro:
            continue
        obj = os.path.join(ROOT, 'build', 'GOWE69', 'src', 'Speed', 'Indep',
                           'SourceLists', unidad + '.o')
        if not os.path.exists(obj):
            fallos.append('%s: no esta compilado (%s)' % (unidad, obj)); continue
        if obj not in cache:
            cache[obj] = rodata_y_simbolos(obj)
        blob, sy = cache[obj]
        # OJO: la primera ocurrencia puede caer DENTRO de otra cadena mas larga
        # --`GAMECUBE` dentro del prefijo escrito a mano, `DamageParams` dentro de
        # otro simbolo-- y entonces no hay `$LC` en ese offset y se daba un FALLO
        # falso. Hay que recorrerlas TODAS y quedarse con la que empieza justo
        # donde hay un simbolo. Me costo borrar 41 entradas buenas.
        pat = cadena.encode('latin1') + b'\0'
        k = blob.find(pat)
        if k < 0:
            fallos.append('%s: la cadena %r NO esta en su .rodata' % (unidad, cadena)); continue
        aqui = []
        while k >= 0 and not aqui:
            aqui = [nm for off, nm in sy if off == k and nm.startswith('$LC')]
            if not aqui:
                k = blob.find(pat, k + 1)
        if not aqui:
            fallos.append('%s: %r no tiene simbolo $LC propio' % (unidad, cadena)); continue
        bueno = '%s.o:%s' % (unidad, aqui[0])
        if L[i + 1].strip() != bueno:
            cambios.append((i + 1, L[i + 1].strip(), bueno))
            L[i + 1] = bueno
    ffuente, nok = revisa_fuente(cache, filtro)
    fallos += ffuente
    for u in fallos:
        print('  FALLO   ', u)
    for n, viejo, bueno in cambios:
        print('  CORRIGE  linea %d: %s -> %s' % (n + 1, viejo, bueno))
    if cambios and not solo_check:
        io.open(KEEP, 'w', encoding='utf-8', newline='').write(fin.join(L))
        print('keep.lst actualizado (%d)' % len(cambios))
    if not cambios and not fallos:
        print('todas las entradas @lc estan al dia (y %d @lcsrc de fuente)' % nok)
    sys.exit(1 if (fallos or (cambios and solo_check)) else 0)


main()
