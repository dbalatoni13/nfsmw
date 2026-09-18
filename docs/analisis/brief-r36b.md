# Ronda 36b — instrucciones comunes

Estado: **98,1043 % matched** (18.331 / 18.432 funciones), **17,41 % linked**
(475 / 617), `main.dol` `9619ba57c991` verificado byte a byte.

**Lo que queda son 91 funciones y 74.688 B.** Están todas medidas y repartidas.

## 0. LEE PRIMERO, en este orden

1. `docs/analisis/brief-r36.md` y `brief-r35.md` (el método, las herramientas).
2. **`docs/analisis/r36-jf-triaje-91.md`** — es tu mapa. Trae las 91 clasificadas
   por TIPO de diferencia, la segunda capa con el diagnóstico de `regmap` de las
   doce mayores, la palanca nueva y **once vedas medidas** de esta ronda.
3. `docs/analisis/r36-jf-frentes-abiertos.md` — qué bloquea a cada unidad.
4. `docs/analisis/r36-jf-zbware-dol.md` — cómo se cerró zBWare (cinco causas).

**No repitas nada de lo que esos cuatro dan por barrido.** Si tu encargo toca una
veda, dilo en el informe y busca otra vía.

## 1. El triaje, y cómo se usa

    python scripts/triaje.py            # las 40 mayores, por TIPO de diferencia
    python scripts/triaje.py zWorld     # una unidad entera

Da un veredicto por función:

- **PERMUTADOR** — todas las diferencias son de registro. Va al permutador sin
  que nadie lea nada. **OJO: el barrido de UN cambio ya está hecho en
  `ICEMover::Update` y `TrackCopCameraMover::Update`, y la profundidad 2 con
  recocido también en la segunda. No los repitas.**
- **falta código** — casi siempre UNA local. `regmap.py` la nombra en un minuto.
- **sobra código** — defensivo que el original no tiene.
- **ESTRUCTURA** — hay que leer con `fuse.py`.

Y la segunda herramienta, que es la que de verdad decide:

    python scripts/regmap.py <unidad> --scan --min 90 --minsize 300   # triaje
    python scripts/regmap.py <unidad> "Clase::Func" --all             # detalle

`regmap` distingue lo que `triaje` no puede: si hay **una local que el original
tiene y nosotros no**, si el **árbol de bloques** difiere, o si es sólo reparto.
Su veredicto manda: *«hay N locales sólo del original / el bloque X falta»* va
PRIMERO; los registros se recolocan solos.

## 2. La palanca nueva de la r36: la barrera SELECTIVA

    asm("" : "+f"(x));      // extendido, NO volátil: ata sólo lo que nombra

Cerró **34 de las 91 diferencias** de `WRoadNav::HolePunchAvoidables`:
**95,455 % → 98,130 %**, después de que cuatro rondas (r25-r28) la dieran por
atascada con seis formas de la sentencia probadas.

**La condición es estrecha, y está medida: una de siete.** Sólo vale cuando
`regmap` ve una local en un registro **PRESERVADO** (f26..f31, r14..r31) donde
el objetivo usa uno **VOLÁTIL** (f0..f13, r0..r12) **por haber cruzado una
llamada**. Colocación: **valor de coma flotante, atado ANTES de la sentencia que
lo consume**.

Los seis negativos, medidos, para que no los repitas: atar después de calcular
(neutro), atar un PUNTERO (−3,3 puntos y +12 B), atar la entrada de una
definición (−6,6 puntos), materializar una local plegada (−0,6 y −8 B), y
`InitAtSegment`, donde el diagnóstico parecía idéntico —tres `stfs` adelantados,
señalados por el propio permutador— y cuesta 3 puntos porque ahí los stores **no
cruzan ninguna llamada**.

**Y una consecuencia que vale para todos: las vedas de reparto CADUCAN cuando
cambia la base.** Con la barrera puesta, un cambio de la r28 que antes rompía dos
filas pasó a no romper ninguna. Si tu función tiene vedas de rondas anteriores y
consigues moverla, **vuelve a probar las vedas viejas**.

## 3. Herramientas nuevas de la r36

- `triaje.py` — clasifica por tipo de diferencia. **Empieza siempre por aquí.**
- `dolwhere.py <unidad>` — enlaza con la unidad sustituida y dice qué direcciones
  del DOL no casan, con su símbolo. Si las diferencias son desplazamientos
  constantes (todas −8, todas −12) es un símbolo estripado o un hueco que falta.
- `dolrod.py <unidad> [.sección]` — diff con resincronización de una sección
  **ENLAZADA**, acotado a la ventana de la unidad. **El objeto engaña**:
  `-strip-unused-data` se lleva los símbolos muertos pero CONSERVA el pool de la
  función descartada, así que `datadiff` decía +128 B donde el enlace decía −48.
- `gapchk.py` — huecos escritos a mano sin entrada en `keep.lst`. Un hueco con
  `.size` se lo lleva el enlazador ENTERO aunque mida 4 B.
- `lcfix.py` — los `$LC` de `keep.lst` se desplazan al añadir cualquier literal
  antes en la unidad, y el DOL rompe EN SILENCIO. **Pásalo después de cada
  recompilación de una unidad con entradas `@lc`.**

## 4. Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados con su cifra.** Si no cierra, **revierte** y anota la veda
  diciendo QUÉ barriste. Un negativo bien medido vale tanto como un positivo.
- **Mide contra el último commit**, no contra tu memoria.
- `build_direct.py <unidad>` para objetos sueltos; **nunca un `ninja` completo**.
- **Sólo toca los ficheros de tu lista.** Hay otros agentes en paralelo y un
  `git add` de directorio ajeno ha barrido trabajo en vuelo cinco veces.
- Informe en `docs/analisis/r36b-<grupo>.md`. **No commits.**

## 5. Prohibido

- **Escribir ensamblador de instrucciones.** Emitir **datos** sí es legítimo, y
  un `asm("")` extendido como barrera también (no emite un byte).
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación**,
  en el scratchpad, sin aplicar. Y ojo: asignar un rango huérfano a su unidad
  dueña **rompe el DOL** (medido: el dato cambia de posición de enlace).
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp`.
- **Borra tus temporales.**
