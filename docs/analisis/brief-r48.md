# Brief de la ronda 48

Sustituye a `brief-r47.md`. Leelo entero antes de tocar nada.

## Estado

`matched` **98,98 %** (3.905.708 / 3.946.048 B, 18.394 / 18.432 funciones).
`linked` **19,55 %** (493 / 619 unidades). El **SDK entero al 100 %**.
Quedan **40.340 B en 37 funciones de 17 unidades**.
DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## Reglas duras

1. **Nunca lances un `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el arbol.
2. **No hagas commits ni `git add`.** Deja los ficheros modificados.
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Proponlo con
   la medida que lo respalda.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques --salvo que el encargo te la asigne, y entonces el
   gate son TODAS las unidades que la incluyen.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Ancla cada parche con **dos o tres lineas de contexto**.
7. Muchos ficheros son **CRLF entero** (y alguno mixto). Un parche con saltos de
   linea Unix casa 0 de N sin avisar.
8. **Si retiras un ensayo, comprueba que la fuente vuelve de verdad**, y pasa
   `fncmp` al final. En la r47 una local imprescindible se perdio al restaurar
   una copia y nadie lo vio hasta la ronda siguiente: el `matched` global y el
   DOL **no se mueven** cuando una funcion ya pendiente empeora.
9. **Cero bytes con `asm` puesto es deuda**: si una funcion no llega al 100 %,
   revierte y deja la veda escrita, salvo que el avance sea estructural (el
   tamano pasa a exacto, desaparece el marco o el derrame) y lo digas claro.

## Herramientas

    python scripts/fncmp.py <unidad>              # que funciones NO son identicas
    python scripts/fncmp.py <unidad> <Nombre>     # detalle instruccion a instruccion
    python scripts/fndiff.py <unidad> <simbolo>   # confirmalo ANTES de empezar
    python scripts/triaje.py <unidad>             # PERMUTADOR / falta local / estructura
    python scripts/lmap.py <unidad> <simbolo>     # la linea de fuente de cada instruccion
    python scripts/rtldump.py <unidad> <fnfiltro> -dg -dl     # los volcados RTL
    python scripts/build_direct.py <unidad>

`matched_code` es **todo o nada**. Y el fuzzy **engana**: puede SUBIR bajando
las filas exactas. **Cuenta filas.**

`fncmp` ya lee la seccion `.over`, asi que **`zFeOverlay` y `zOnline` por fin se
miden** como las demas (llevaban rondas invisibles al triaje).

## LO PRIMERO: el volcado RTL, no probar formas

**El fuente de GCC 2.95.3 esta en el arbol**: `orig/prodg/NGC_GNU_SRC/NGC/gcc/`
(`global.c`, `local-alloc.c`, `sched.c`, `cse.c`, `gcse.c`, `jump.c`,
`combine.c`, `stmt.c`). No infieras el comportamiento del compilador: leelo.

`scripts/rtldump.py` da sus volcados. Sirve para una unidad entera (22 s) o para
un `.cpp` suelto que reproduzca la funcion byte a byte (0,4 s).

- **`.greg`**: el orden de asignacion **ya ordenado por `allocno_compare`**, la
  matriz de conflictos y el `reg_renumber` final.
- **`.combine`**: por que `combine` no pliega algo.
- **`.lreg`**, y `-dj`/`-dL` para contar rtx por bloque.

**En la r47 esto dio la vuelta a TRES diagnosticos que llevaban varias rondas**:
en `spchsamp` el conflicto con r0 lo causaba la planificacion y no al reves; en
`PATHI_calcwaitbeat` no era el permutador; y `sfir` es **PRE**, con el volcado
imprimiendolo literalmente (`PRE: redundant insn 533 (expression 7)`). Empieza
por ahi antes de escribir la primera variante.

## Las palancas

Todas son de GCC. **En `LibSN/steering` no funciona ninguna** (es mwcc); ahi lo
unico que ha funcionado es **leer a traves de un puntero `volatile`**.

### 1. Barrera selectiva --- y las DOS formas nuevas

`asm("" : "+r"(x))`, `"+f"`, `"+m"`. Extendida y NO volatil: ata solo lo que
nombra. Usos medidos: adelanto del planificador, pliegue de CSE, hundimiento
entre bloques, coalescing, orden de argumentos de una llamada (**un `asm` por
argumento DESCOLOCADO**, y **sin una local propia el resultado es 12 veces
peor**), y `"+m"` para fijar el `stw` de una local de PILA (un `"+r"` sobre una
variable que vive en la pila **se ignora en silencio**).

**NUEVO r47, CORREGIDO en r48 --- `asm("" : : "r"(x))` de solo entrada sube
`n_refs`, pero NO siempre es gratis**: un `asm` sin salidas es volatil, asi que
emite salvo que GCC lo absorba en una copia preexistente. Medido en las dos
direcciones: cero bytes en `cStichWrapper::Play` --donde cerro una veda de
cinco rondas-- y 67 -> 68 instrucciones en `eProject`. Es un caso, no una ley:
**midelo**.

Lo que si vale en general: **la veda de r36f --«cualquier `asm` en ese bucle
cuesta 4 B»-- valia solo para los `asm` CON SALIDA**, asi que conviene rehacer
con esta forma los near-miss de reparto que se dieron por cerrados con `"+r"`.

**NUEVO r47 --- `asm("")` a secas rompe empates EXACTOS.** En
`Play__16CARSFX_RoadNoise` los dos allocnos empataban a 1.111 en
`allocno_compare` y el desempate por numero caia del lado malo; una ranura de
cero bytes los puso en 1.071 contra 1.095.

**La cadena de N**: cuando el objetivo emite una secuencia concreta, escribe una
dependencia por eslabon y midelas **solo juntas**. Aisladas no mueven nada.

### 2. `set_preference` encadena locales, y manda la SEGUNDA

`global.c` hace `src = XEXP(src, 0)` para un `src` de formato `'e'`, asi que de
un `(set A (mult B C))` el allocno de `B` **hereda** el registro que
`local_alloc` le dio a `A`. En `HolePunchAvoidables` eso significaba que habia
que pinchar **la carga que le quita el registro**, no la variable que sale en el
diff. Bajo de 9 filas a 4 con cero `asm` netos.

### 3. Pin de registro

`register float x asm("fr7")`. Lo unico que llega a `local_alloc`. Se ignora si
se toma la direccion. **Si el pin empeora, el reparto es un SINTOMA.**

**Correccion r47**: el `REG_ALLOC_ORDER` de rs6000 **NO empieza por r31**: es
`0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30, ...` (`rs6000.h:932`). El «31 primero»
solo aparece en `global_alloc` para allocnos que cruzan llamadas. En FP el orden
es `f0, f13, f12, f11, f10, ...`.

**Y a veces el pin no mejora el porcentaje pero SI la estructura**: en
`PATHI_calcwaitbeat` mover un pin existente de fr11 a fr10 convirtio una
rotacion de cuatro registros en un swap limpio, y entonces el pin que faltaba
salio solo.

### 4. La cantidad fantasma

    register int guard asm("r19");
    asm("" : "=r"(guard));
    ...
    asm("" : "+r"(algo_vivo) : "r"(guard));

El `asm` de cierre **tiene que tocar un valor vivo**. **EL REGISTRO IMPORTA**.
Vedas: **no vale en FPR de trabajo** (f0..f13), medido 15 -> 228 filas; y en
`madidct` esta cerrada con 24 registros x 8 combinaciones, 32 ensayos que siguen
en 496 B --el fuzzy sube a 52,99 % sin mover un byte--.

### 5. `asm()` de fichero, y el `asm` CON CUERPO

Para un alias, un talon o una llamada de cola sin pasar la unidad a `.s`. Y el
`asm` que **emite la instruccion que falta**: en `EvalSQT` el `.combine` enseno
que la instruccion del objetivo ya estaba y solo sobraba un `zero_extend` que
`combine` no podia tirar; un `asm` cuyo operando de **entrada es el campo con su
tipo estrecho** (HImode, asi GCC pasa el registro crudo) lo cerro. **No es deuda
si el tamano pasa a exacto y el diff a cero.**

**Aviso**: un `asm` con cuerpo **no sirve de barrera de programacion** --emite la
instruccion exacta pero su `"+f"` no ancla--.

### 6. Las guardas de cabecera que nadie enciende

Busca `#ifdef` en las cabeceras de tu territorio que no aparezcan en ningun
`#define` ni en los cflags. **Escribir la llamada a mano NO equivale.**

### 7. El compilador equivocado, y el objeto que son varias TU

`OdemuExi2` resulto ser **TRES unidades de traduccion** en un objeto, cada una
con su version de MWCC. Se detecta barriendo versiones y viendo si los fallos se
parten en conjuntos **disjuntos y separados por direccion**. Si salen
**anidados**, es una sola TU (asi se cerro `steering`, 28 versiones).

**Y como NO se valida un corte**: comprobar el reparto de estaticos
**compilando** las mitades no basta; solo el ENLACE destapa que un `static` de
una mitad lo referencia la otra.

### 8. El mapa de lineas manda sobre `-ffast-math`

El ELF atribuye cada multiplicacion a una linea distinta. Escritas como
sentencias separadas, GCC ya no las reagrupa. Mira `lmap.py` antes de pelear con
el orden de un calculo flotante.

### 9. Lo que se reserva al EXPANDIR y sobrevive a su codigo

Dos casos medidos, y la firma es la misma: **mismas instrucciones, y al objetivo
le sobra algo que no emitimos**.

- **El area X del marco**: el temporal de conversion entero<->flotante, que GCC
  reserva al expandir y no libera aunque el optimizador se lleve su codigo.
  Cerro `pathtrack` (8 B de marco, cero `asm`).
- **El `lfd` del sesgo `0x4330000080000000`**: igual, pero la instruccion. Lo
  delata `lmap.py` **sobre el original**, que lo imputa a una sentencia donde no
  hay ninguna otra instruccion de conversion. Se reproduce multiplicando por una
  variable entera que siempre vale 1: la conversion se expande, `-ffast-math`
  pliega el producto y queda solo el `lfd`. Cerro `FindMatchTime`.
  **Frente barrido y VACIO**: contadas en las 42 pendientes de la r47, el conteo
  cuadra en todas. No busques un tercer caso de este.

### 10. Los dos grupos de peso de `sched1`

`rank_for_schedule` desempata por `INSN_REG_WEIGHT`, nivel que **solo existe en
`sched1`**. En una tirada de escrituras que leen el mismo registro constante, el
store que **mata** ese registro pesa -1 y los demas 0: salen en **dos grupos,
cada uno en orden de fuente**, y el `REG_DEAD` esta en el ultimo uso **en la
fuente**. Cerro `zFe2` sin un solo `asm`.
**Aviso**: solo desempata con prioridades iguales; si son 12 contra 2, no aplica.

### 11. El DWARF trae los TIPOS, no solo las locales

Lista las **struct del original con el offset de cada miembro**. Con eso cayeron
tres filas que llevaban cuatro rondas y 19 formas de fuente en
`ActualReadJoystickData`.

**Regla barrible**: si el objetivo pone la **BASE primero** en un `add` o en un
store indexado (`sthx`, `stwx`) y nosotros el indice, la fuente original **indexa
un array MIEMBRO**, no un puntero casteado. Y es la pertenencia lo que decide:
`((T*)p)[i].x` deja exactamente las mismas filas.

### 12. La forma del bucle decide el idioma de la rama

Con `while (a && b->x < y)` GCC emite `blt`; escrito como `while (a) { if (...) ... else break; }`
sale el idioma `fcmpu + cror + bso` que el original tiene bajo `-ffast-math`.
Cerro 4.396 B en dos funciones a la primera compilacion y tumbo una veda que la
ronda anterior habia dado por cerrada con analisis de `jump.c`.

## Avisos que costaron una ronda

- **La reciproca de «falta una local» es FALSA.** Que el DWARF no liste una
  local NO significa que sobre: cuatro contraejemplos medidos, y uno de ellos
  --`slipBoost`-- se perdio al restaurar una copia y costo una regresion.
- **El DWARF tambien se equivoca**: en `PATHI_calcwaitbeat` da `scalar // f11`
  **y** `fevery // f11`, y solo la primera es cierta.
- **`matched_code == total_code` NO implica que la unidad sea enlazable.**
  `zEAXSound` quedo con cero funciones pendientes y **no puede promocionar**:
  exporta 229 simbolos de mas y su `.text` mide 173.572 B contra 151.092.
- **«Permutador agotado» NO es techo**: su catalogo es de formas de FUENTE.
- **Las vedas caducan.** Cuatro han caido esta temporada, casi todas por haber
  probado N formas de **la sentencia equivocada**. Si una veda cita un
  porcentaje, **remidelo sobre la base actual**.
- **Nunca QUITES un literal** en una unidad con `$LC` en `keep.lst`. Rellenalo a
  multiplo de 8 y pasa `lcfix.py --check`.

## El eje de tamano (triaje rapido)

De las pendientes, la mayoria **mide exactamente lo que el objetivo**: esas solo
pueden ser planificacion o reparto, y la via es el `.greg`. Las que difieren
tienen causa estructural, y **las de +-4 B son una sola instruccion** --el caso
mas barato del censo, y el que cerraron `FindMatchTime` y `EvalSQT`--.

## El reparto

| # | clave | territorio | B | fn |
|---|---|---|---:|---:|
| 1 | `linked` | `zFeOverlay` + `pathi.h`/`pathnode` + el frente de simbolos de mas | 748 | 2 |
| 2 | `cam` | `zCamera` | 10.812 | 5 |
| 3 | `world` | `zWorld` + `zWorld2` + `zTrack` | 10.112 | 8 |
| 4 | `ecs` | `zEcstasy` + `zEagl4Anim` | 7.988 | 6 |
| 5 | `plat` | `steering` + `zPlatform` + `zPhysicsBehaviors` | 5.012 | 8 |
| 6 | `libs` | `madidct` + `sfir` + `criticalpath` | 2.824 | 4 |
| 7 | `snd` | `zEAXSound2` + `zSpeech` | 2.824 | 4 |

**El territorio 1 vale mas que todos los demas juntos** medido en `linked`:
145.428 B contra los 40.340 de codigo de todo el censo.

## Que entregar

1. Informe en `docs/analisis/r48-<tu-clave>.md`: lo cerrado (con `fndiff` al
   100 % citado), lo medido y negativo **con la cifra**, y las propuestas fuera
   de territorio.
2. Verificacion obligatoria: `fndiff` = 100.0 en lo que cierres; `fncmp`
   antes/despues sobre TODAS tus unidades sin que **ninguna** empeore;
   `lcfix.py --check` limpio; `git status` de tus ficheros revisado.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**.
