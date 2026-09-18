#!/usr/bin/env python3
"""deadstr.py -- cadenas que EMITIMOS, el objetivo TIENE, y el enlazador se lleva.

`-strip-unused-data` quita `size & ~7` de cada simbolo muerto. Una cadena `$LC`
que nadie referencia pierde casi todo su cuerpo y deja una cola de 1..7 bytes:
en el enlace se ve como «falta la cadena entera y sobra un fragmento». La unidad
mide bien en el objeto --`datadiff` las encuentra todas-- y aun asi el DOL sale
corto. Le paso a `zFe`: 2.880 B de los 3.168 de deficit eran esto.

Esto cruza tres cosas y dice cuales hay que salvar:

  1. los simbolos `$LC` de NUESTRA `.rodata`, con su contenido;
  2. cuales de ellos no aparecen en ninguna reubicacion -> MUERTOS;
  3. cuales de esos contenidos estan en la `.rodata` del objeto EXTRAIDO.

Los que cumplen las tres son bytes que el objetivo tiene y nosotros perdemos.

    python scripts/deadstr.py zFe                 # informe
    python scripts/deadstr.py zFe --keep          # genera las lineas de keep.lst

Las lineas que genera llevan la directiva `# @lc` que necesita `lcfix.py`, que es
lo que impide que el numero de `$LC` se quede rancio en la siguiente compilacion.

DOS TRAMPAS, las dos medidas en la ventana de la r60:

1. **Casi todo lo que propone YA ESTA EN `keep.lst`.** El barrido de las 17
   SourceLists pendientes daba 927 cadenas y 15.096 B, que parecia un frente
   nuevo entero. Restando las entradas que ya existen quedan **51 en cuatro
   unidades** (zMisc 37, zCamera 6, zEcstasy 5, zFe2 3) y CERO en las otras
   trece. Control barato: `comm -23` entre lo que propone y lo que ya hay.
   Si no lo restas, repartes trabajo hecho.

2. **Empareja por CONTENIDO, asi que no distingue «al objetivo le falta esta
   cadena» de «el objetivo la tiene por otra via».** En `zMisc` propone 37 y las
   37 cambian el DOL --de a2394557abb5 a d857909df50a-- pero la unidad esta en
   `rodata+2840`, o sea que YA emite de mas, y su problema conocido son 150
   cadenas DUPLICADAS: el objetivo tiene cada una una sola vez y dentro de su
   bloque escrito a mano. Ahi salvar la copia `$LC` empuja en el sentido
   contrario. **Antes de aplicar, mira el signo de `linkdelta`.**
"""
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def lee(p):
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    se, n, sx = struct.unpack('>HHH', d[0x2E:0x34])
    S = [struct.unpack('>10I', d[shoff + i * se:shoff + i * se + 40]) for i in range(n)]
    stro = S[sx][4]
    nom = lambda o: d[stro + o:d.index(b'\0', stro + o)].decode()
    return d, S, nom


def rodata(p, sec='.rodata'):
    d, S, nom = lee(p)
    for s in S:
        if nom(s[0]) == sec:
            return d[s[4]:s[4] + s[5]]
    return b''


def todos_los_datos(p):
    """Todas las secciones de datos del objetivo juntas: una cadena puede estar
    en `.data` o en `.over` y seguir siendo la misma cadena."""
    d, S, nom = lee(p)
    out = b''
    for s in S:
        if s[1] == 1 and s[5] and nom(s[0]) in ('.rodata', '.data', '.over', '.sdata', '.sdata2'):
            out += d[s[4]:s[4] + s[5]] + b'\0'
    return out


def analiza(unidad):
    rel = os.path.join('Speed', 'Indep', 'SourceLists', unidad + '.o')
    obj = os.path.join(ROOT, 'build', 'GOWE69', 'obj', rel)
    nue = os.path.join(ROOT, 'build', 'GOWE69', 'src', rel)
    if not (os.path.exists(obj) and os.path.exists(nue)):
        sys.exit('%s: falta alguno de los dos objetos' % unidad)
    # `--ancho` acepta la cadena si esta en CUALQUIER seccion de datos del
    # objetivo, no solo en su .rodata. Es opt-in porque amplia los falsos
    # positivos: la copia de .rodata puede deber estriparse aunque el objetivo
    # tenga esa misma cadena en .data por otra via.
    objetivo = todos_los_datos(obj) if '--ancho' in sys.argv else rodata(obj)
    d, S, nom = lee(nue)
    idx_rod = [i for i, s in enumerate(S) if nom(s[0]) == '.rodata']
    if not idx_rod:
        return [], b''
    idx_rod = idx_rod[0]
    blob = d[S[idx_rod][4]:S[idx_rod][4] + S[idx_rod][5]]
    st = [s for s in S if s[1] == 2][0]
    strt = S[st[6]][4]
    sim = {}
    for k, q in enumerate(range(st[4], st[4] + st[5], 16)):
        nm, val, sz, info, otro, shn = struct.unpack('>IIIBBH', d[q:q + 16])
        if shn == idx_rod:
            sim[k] = (d[strt + nm:d.index(b'\0', strt + nm)].decode('latin1'), val, sz)
    vivos = set()
    for s in S:
        if s[1] != 4:                       # SHT_RELA
            continue
        # OJO: solo cuentan las reubicaciones de secciones ALLOC. `.rela.debug` y
        # `.rela.line` NOMBRAN los $LC, asi que sin este filtro salen TODOS vivos
        # y la herramienta contesta "0 cadenas muertas" sin fallar. Tuvo ciega a
        # zFeOverlay cinco rondas: 370 vivos donde hay 44 muertas (544 B). Se ve
        # solo en las unidades cuyo .debug nombra los literales -- en zFe las
        # encontraba igual, y por eso el fallo nunca salto.
        if not (S[s[7]][2] & 0x2):          # SHF_ALLOC de la seccion destino
            continue
        for q in range(s[4], s[4] + s[5], 12):
            off, info, add = struct.unpack('>IIi', d[q:q + 12])
            vivos.add(info >> 8)
    fuera = []
    for k, (nm, val, sz) in sorted(sim.items(), key=lambda x: x[1][1]):
        if not nm.startswith('$LC') or not sz or k in vivos:
            continue
        cuerpo = blob[val:val + sz]
        if b'\0' not in cuerpo:
            continue                        # no es cadena terminada
        texto = cuerpo.split(b'\0')[0]
        if not texto or not all(32 <= c < 127 for c in texto):
            continue
        if (texto + b'\0') not in objetivo:
            continue                        # el objetivo no la tiene: no la salvamos
        pierde = sz & ~7
        if pierde:
            fuera.append((nm, val, sz, pierde, texto.decode('latin1')))
    return fuera, blob


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    os.chdir(ROOT)
    unidad = sys.argv[1]
    keep = '--keep' in sys.argv
    fuera, blob = analiza(unidad)
    if keep:
        vistos = set()
        for nm, val, sz, pierde, texto in fuera:
            if texto in vistos:
                continue                    # contenido repetido: `lcfix` no lo puede resolver
            vistos.add(texto)
            print('# @lc %s "%s"' % (unidad, texto))
            print('%s.o:%s' % (unidad, nm))
        return
    tot = sum(p for _, _, _, p, _ in fuera)
    print('%-8s %5s %5s  cadena' % ('$LC', 'tam', 'pierde'))
    for nm, val, sz, pierde, texto in fuera:
        print('%-8s %5d %5d  %s' % (nm, sz, pierde, texto[:60]))
    print('\n%d cadenas muertas que el objetivo SI tiene -- %d B perdidos al enlazar'
          % (len(fuera), tot))


main()
