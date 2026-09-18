#!/usr/bin/env python3
"""movidos.py -- que simbolos caen en OTRA DIRECCION al meter una unidad.

La medida que faltaba, y la pidieron dos agentes por separado en la r53 despues
de construirsela cada uno.

`linkdelta.py` compara **tamanos de seccion**, y eso no dice si una unidad esta
cerca. En la r53 me equivoque dos veces por creerlo:

  * `zSpeech` daba `rodata-8` y emitia **83 de sus 703 funciones en otra
    direccion**, con 333.417 B distintos en el DOL;
  * `zAI` daba `rodata-240` y tenia **781 de 1.030 descolocadas**, 176.928 B de
    272.796. Los 240 B eran el 0,09 % del problema.

Y `dolrod.py` tampoco vale para esto: hace `difflib` byte a byte --mas de diez
minutos sobre 272 kB-- y sobre una seccion permutada devuelve trozos de
instruccion sin decir que el problema es de POSICION.

Esto enlaza dos veces --con el objeto extraido y con el nuestro-- y compara la
DIRECCION de cada simbolo comun. En segundos.

    python scripts/movidos.py Speed/Indep/SourceLists/zAI
    python scripts/movidos.py <unidad> --todos    # lista todos, no solo el resumen

DOS AVISOS:

  * **Separa ARRASTRE de PERMUTACION.** Si una seccion sale corta, todo lo que
    va detras se desplaza por igual y eso no es un problema de orden. En zAI son
    18.128 simbolos en otra direccion, de los que **17.143 son arrastre** y solo
    985 estan permutados de verdad. Sin esa resta la cifra asusta y no dice nada.
  * **Cuenta SIMBOLOS, no funciones**: entran etiquetas y alias. Para el numero
    de funciones usa `permorden.py`, que empareja por simbolo de funcion. Este
    sirve para TRIAJE --que seccion y que magnitud-- y para ver el histograma de
    desplazamientos, que es lo que delata si hay uno o varios racimos.
"""
import collections
import os
import struct
import sys
import types

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, 'scripts')
try:
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    pass

NL = chr(10)
_src = open('scripts/linkdelta.py', encoding='utf-8').read().split(NL + 'def main()')[0]
ld = types.ModuleType('ld')
ld.__dict__['__file__'] = os.path.abspath('scripts/linkdelta.py')
exec(compile(_src, 'linkdelta.py', 'exec'), ld.__dict__)


def cabeceras(p):
    """-> {nombre de seccion: (direccion, tamano)}. Vale para .o y para enlazado."""
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
    hs = [struct.unpack('>IIIIII', d[shoff + i * shent:shoff + i * shent + 24])
          for i in range(shnum)]
    so = hs[shstr][4]
    out = {}
    for nm, ty, fl, ad, off, sz in hs:
        e = d.index(bytes([0]), so + nm)
        out[d[so + nm:e].decode('ascii', 'replace')] = (ad, sz)
    return out


def simbolos(p):
    """-> {nombre: (direccion, tamano, indice de seccion)} del ELF enlazado."""
    d = open(p, 'rb').read()
    shoff, = struct.unpack('>I', d[0x20:0x24])
    shent, shnum, shstr = struct.unpack('>HHH', d[0x2E:0x34])
    hs = [struct.unpack('>IIIIII', d[shoff + i * shent:shoff + i * shent + 24])
          for i in range(shnum)]
    so = hs[shstr][4]
    nombres = {}
    for i, (nm, ty, fl, ad, off, sz) in enumerate(hs):
        e = d.index(bytes([0]), so + nm)
        nombres[i] = d[so + nm:e].decode('ascii', 'replace')
    sym = strt = None
    for i, (nm, ty, fl, ad, off, sz) in enumerate(hs):
        if nombres[i] == '.symtab':
            sym = (off, sz)
        if nombres[i] == '.strtab':
            strt = (off, sz)
    if not sym:
        return {}
    out = {}
    for k in range(sym[1] // 16):
        o = sym[0] + k * 16
        nm, val, tam, info, other, shndx = struct.unpack('>IIIBBH', d[o:o + 16])
        if not nm:
            continue
        e = d.index(bytes([0]), strt[0] + nm)
        n = d[strt[0] + nm:e].decode('ascii', 'replace')
        if n:
            out[n] = (val, tam, shndx, nombres.get(shndx, '?'))
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not args:
        sys.exit(__doc__)
    todos = '--todos' in sys.argv
    u = args[0]
    base = ld.tp.objetos_del_enlace()
    o, n = ld.tp.resolver(u, base)
    if o is None:
        print('%s no esta en la lista de enlace (ya promocionada?)' % u)
        return 1
    if not os.path.exists(n):
        print('%s: falta nuestro .o' % u)
        return 1
    pa = os.path.join(ld.tp.TMP, 'mv_a.elf')
    pb = os.path.join(ld.tp.TMP, 'mv_b.elf')
    if ld.enlazar(base, pa) is None or ld.enlazar(
            [n if x == o else x for x in base], pb) is None:
        print('el enlace falla')
        return 1
    A, B = simbolos(pa), simbolos(pb)
    # `$LCnnn` es un contador POR OBJETO: el `$LC62` de un ELF no tiene por que
    # ser el del otro. Emparejarlos por nombre fabrica desplazamientos enormes y
    # falsos --el agente `control` midio saltos de +124.000 B inventados, y
    # `forense2` encontro un `$LC62` del enlace base fuera de la ventana de
    # zMain--. Se cuentan aparte y no entran en la permutacion.
    todos_com = [k for k in A if k in B]
    com = [k for k in todos_com if not k.startswith('$LC')]
    nlc = len(todos_com) - len(com)
    # OJO: si una seccion sale corta, TODO lo que va detras se desplaza por igual.
    # Eso no es permutacion, es arrastre.
    #
    # CORREGIDO (r54, lo encontro el agente `control`). Aqui tomaba el arrastre
    # como la MODA de los desplazamientos, y era una conjetura donde hay un dato
    # exacto. Peor: `crudo` sólo contiene simbolos que SE MOVIERON, asi que
    # **cuando el arrastre real es 0 la moda no puede valer 0 nunca** -- coge el
    # mayor racimo de permutaciones de verdad y lo cancela como si fuera
    # arrastre, y de paso convierte en "movidos" a los que estaban en su sitio.
    # En `zMain`, con las nueve secciones a delta 0, escondia 100 de 183.
    #
    # El arrastre de una seccion tiene TRES terminos, y con uno solo no sale:
    #   base  = cuanto se mueve el PRINCIPIO de la seccion de salida. Si .rodata
    #           encoge 616 B, .data/.bss/.sbss empiezan 616 B mas abajo AUNQUE
    #           nuestra aportacion a ellas no cambie. Sale de las cabeceras de
    #           los dos ELF ENLAZADOS.
    #   delta = cuanto crece NUESTRA aportacion a esa seccion, de las cabeceras
    #           de los dos .o. Solo lo sufren los simbolos que van DETRAS.
    #   robado = lo que cambia de DUENO. Lo que crece la seccion ENLAZADA no
    #           tiene por que ser lo que crece nuestra aportacion: la diferencia
    #           es bulto que se le quita a otro objeto (M4). Lo sufre lo que va
    #           detras de ese otro objeto.
    # Asi que el arrastre legitimo toma exactamente TRES valores --`base`,
    # `base+delta` y `base+enlazado`--, y cualquier otro desplazamiento es
    # permutacion. No hace falta saber donde cae ninguna de las dos aportaciones.
    crudo = [(B[k][0] - A[k][0], k, A[k]) for k in com if A[k][0] != B[k][0]]
    ca, cb = cabeceras(pa), cabeceras(pb)
    oa, ob = cabeceras(o), cabeceras(n)
    base = {s: cb[s][0] - ca[s][0] for s in ca if s in cb}
    delta = {s: ob.get(s, (0, 0))[1] - oa.get(s, (0, 0))[1] for s in set(oa) | set(ob)}
    enlazado = {s: cb[s][1] - ca[s][1] for s in ca if s in cb}

    def es_arrastre(d, snm):
        # TRES valores legitimos, no dos: el principio de la seccion (`base`),
        # mas lo que crece NUESTRA aportacion (`delta`) para lo que va detras de
        # ella, mas lo que crece la seccion ENLAZADA entera (`enlazado`) para lo
        # que va detras tambien de lo que no sobrevivio al enlace. En zGameModes
        # el tercer escalon --6.358 simbolos a -744-- es exactamente
        # delta(-616) + (-128), y sin ese termino salian como 6.358
        # permutaciones que no existen.
        b = base.get(snm, 0)
        return d in (b, b + delta.get(snm, 0), b + enlazado.get(snm, 0))
    # Red de seguridad: si aun asi queda un escalon sin explicar, no lo cuento
    # como permutacion -- **un desplazamiento que comparten miles de simbolos es una
    # TRASLACION, no una permutacion**, porque el orden relativo entre ellos no
    # cambia. Solo el residuo disperso es permutacion de verdad.
    resto = [(d, k, a) for d, k, a in crudo if not es_arrastre(d, a[3])]
    porsec_d = {}
    for d, k, a in resto:
        porsec_d.setdefault(a[3], collections.Counter())[d] += 1
    UMBRAL = 20
    pasos = {s: {d for d, c in cnt.items() if c >= UMBRAL}
             for s, cnt in porsec_d.items()}
    escalones = [(s, d, porsec_d[s][d]) for s in pasos for d in pasos[s]]
    mov = [(d - base.get(a[3], 0), k, a) for d, k, a in resto
           if d not in pasos.get(a[3], ())]
    print('%s: %d simbolos comunes (%d `$LC` fuera: su nombre colisiona entre objetos)'
          % (os.path.basename(u), len(com), nlc))
    print('   %d cambian de direccion, pero %d es ARRASTRE (tres terminos: donde'
          % (len(crudo), len(crudo) - len(mov)))
    print('   empieza la seccion, cuanto mide nuestra aportacion, y cuanto de')
    print('   ella no sobrevive al enlace).')
    vis = [s for s in sorted(set(base) | set(delta))
           if (base.get(s) or delta.get(s)) and not s.startswith(
               ('.debug', '.rela', '.line', '.comment', '.note', '.sh', '.str', '.sym'))]
    print('   %s' % ', '.join('%s: base%+d delta%+d' % (s, base.get(s, 0), delta.get(s, 0))
                              for s in vis) or '   ninguna seccion se mueve')

    # LO QUE EMITIMOS DE MAS Y NO SOBREVIVE AL ENLACE.
    #
    # OJO, CORREGIDO EN LA r55 Y ES IMPORTANTE: esto NO es "cambio de dueno".
    # Lo etiquete asi en la r54 y era falso. La resta `enlazado - delta` mezcla
    # TRES cosas y solo una es robo:
    #   (1) bulto que le quitamos a otro objeto  <- el cambio de dueno de verdad
    #   (2) codigo y datos NUESTROS que nadie referencia y el enlazador tira.
    #       `-strip-unused-data` trabaja a granularidad de SIMBOLO, no de
    #       seccion, asi que esto es lo NORMAL, no la excepcion.
    #   (3) relleno de alineacion (los +-8 B sueltos).
    # Medido: en zLua el termino (1) vale CERO y el (2) vale 17.676 --y ya lo
    # decia la docstring de linkdelta.py--. La prueba limpia es zTrack: cero
    # robos y esta resta sigue imprimiendo .text -3.536.
    #
    # El cambio de dueno de verdad se mide por SIMBOLO, no por seccion, y su
    # predicado esta en `docs/analisis/r55-censo-dueno.md`: quien define cada
    # simbolo entre los objetos originales y entre los nuestros, con el ganador
    # por (rango, indice) donde rango es GLOBAL < WEAK < COMMON. Ojo, que GLOBAL
    # gana AUNQUE VAYA DESPUES en el orden de enlace.
    # Para saber si un simbolo concreto cambia de dueno: mira su DIRECCION en
    # los dos ELF enlazados. Si no cambia, no hay robo.
    robado = {s: enlazado.get(s, 0) - delta.get(s, 0)
              for s in vis if enlazado.get(s, 0) != delta.get(s, 0)}
    if robado:
        print()
        print('   EMITIMOS DE MAS Y NO SOBREVIVE AL ENLACE (NO es cambio de dueno:')
        print('   casi todo es codigo sin referenciar que -strip-unused-data tira)')
        for s in sorted(robado):
            print('      %-10s nuestro .o %+7d, seccion enlazada %+7d  ->  %+7d no sobrevive'
                  % (s, delta.get(s, 0), enlazado.get(s, 0), robado[s]))
        print('   Para el cambio de dueno de verdad: docs/analisis/r55-censo-dueno.md')
    if escalones:
        print()
        print('   ESCALONES SIN EXPLICAR (%d): un desplazamiento que comparten muchos'
              % len(escalones))
        print('   simbolos es una TRASLACION -- su orden relativo no cambia --, asi que')
        print('   NO se cuenta como permutacion. Que exista uno es la firma de que un')
        print('   simbolo cambio de DUENO: otro objeto tambien cambio de tamano.')
        for s, d, c in sorted(escalones, key=lambda x: -x[2]):
            print('      %-10s %+9d  %5d simbolos' % (s, d, c))
    print()
    print('**%d simbolos DE VERDAD permutados** (desplazados respecto a su seccion)'
          % len(mov))
    if not mov:
        print('todos en su sitio.')
        return 0
    porsec = collections.Counter()
    bytes_ = collections.Counter()
    for d, k, (ad, tam, sh, snm) in mov:
        porsec[snm] += 1
        bytes_[snm] += tam
    print()
    print('%-12s %8s %10s' % ('seccion', 'movidos', 'bytes'))
    for s in sorted(porsec, key=lambda x: -bytes_[x]):
        print('%-12s %8d %10s' % (s, porsec[s], '{:,}'.format(bytes_[s])))
    print()
    hist = collections.Counter(d for d, _, _ in mov)
    print('los desplazamientos mas repetidos:')
    for d, c in hist.most_common(8):
        print('   %+9d  %d simbolos' % (d, c))
    if todos:
        print()
        for d, k, (ad, tam, sh, snm) in sorted(mov, key=lambda x: -abs(x[0]))[:60]:
            print('   %+9d  %-8s %6d B  %s' % (d, snm, tam, k[:58]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
