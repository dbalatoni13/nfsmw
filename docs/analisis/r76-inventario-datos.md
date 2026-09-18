# r76 — Inventario del frente de datos: 468 bloques, 38.562 bytes

Los bloques `asm(".section ...")` que escriben `.rodata`/`.data`/`.bss` a mano son
el código **menos decompilado** del árbol y, medido en esta ronda, **la puerta de
Xbox 360**: 29 de las 34 SourceLists mueren ahí con `C2143`/`C2290` antes de
llegar a ningún pin.

Inventario con `scratchpad/inventadatos.py` (lee el bloque entero del fichero: el
censo sólo guarda su primera línea).

| clase | bloques | bytes | en Matching | en NonMatching |
|---|---:|---:|---:|---:|
| **TABLA** (`.4byte`/`.2byte` numéricos) | 304 | 22.112 | 110 | 190 |
| **MIXTO** (varias cosas en un bloque) | 17 | 10.988 | 8 | 9 |
| **CADENA** (texto en `.byte`) | 97 | 3.386 | 15 | 81 |
| **HUECO** (`.skip`, relleno) | 28 | 2.076 | 14 | 14 |
| **ETIQUETA** (sólo `.globl`/`.type`/`.set`) | 22 | 0 | 5 | 14 |

**Dos tercios están en unidades NonMatching**, cuyo objeto no se enlaza: ahí
convertir a C real **no puede mover el DOL**. Es el trabajo de mayor volumen y
menor riesgo que tenemos delante.

Las quince unidades con más bytes de parche:

| unidad | bloques | bytes | estado |
|---|---:|---:|---|
| zMisc | 3 | 8.068 | NonMatching |
| zEcstasy | 92 | 3.933 | NonMatching |
| zAnim | 22 | 2.336 | **Matching** |
| zMain | 32 | 2.294 | **Matching** |
| zCamera | 13 | 1.743 | NonMatching |
| zSim | 27 | 1.704 | **Matching** |
| zRender | 2 | 1.196 | **Matching** |
| zFeOverlay | 3 | 1.164 | **Matching** |
| zFe2 | 19 | 1.116 | NonMatching |
| zAI | 21 | 1.032 | NonMatching |
| zTrack | 18 | 980 | NonMatching |
| zMiscSmall | 2 | 968 | **Matching** |
| zLua | 19 | 948 | NonMatching |
| zAttribSys | 2 | 940 | **Matching** |
| zPlatform | 10 | 936 | NonMatching |

## Lo que hay dentro, y por qué no todo se convierte igual

Un ejemplo que enseña la dificultad real, `eViewPlat.cpp:27` (654 líneas, 908 B):
es un pool de cadenas escrito **en trozos de 7 bytes**, cada uno con su propio
símbolo `lbl_XXXXXXXX`. El troceado es deliberado: `-strip-unused-data` se lleva
`size & ~7` de cada símbolo sin referencias, o sea **cero** de trozos de 7 B.

Eso marca la frontera del frente:

- **Se puede convertir hoy**: lo que ya emite el compilador y el parche sólo
  reposiciona (cabezas de literal `$LC`, rellenos que pone el ensamblador,
  marcadores `gcc2_compiled.`), y las tablas numéricas cuyo tipo y nombre se
  pueden justificar (`static const int zigzag[64]` en `maddeca.cpp` fue así).
- **No se puede convertir todavía**: los pools de cadenas cuyo **consumidor no
  está escrito**. Su nombre real sólo aparece cuando exista el código que las
  usa. Convertirlos ahora obligaría a inventar nombres, que es justo lo que el
  criterio del usuario prohíbe (nombres dudosos sólo con evidencia de orden).

Es decir: el frente de datos no se cierra con una pasada mecánica, y quien diga
lo contrario no ha mirado dentro de los bloques. Se cierra por capas, y la
primera capa —la que el plan de la r68 dejó identificada y medida— son 131
bloques.

## Estado del plan de la r68

CORRECCION (r76): los tres lotes de la tanda 1 que llegaron a terminar estan
**aplicados**, incluido el de zSim. Lo di por pendiente al ver `ZSIM_HAND_POOL`
todavia en `zSim.cpp`, pero esos son OTROS bloques que el propio informe dejaba
fuera del lote a proposito (`zSim.cpp:211`, `NISActivity.cpp:54` y `:84`, y el
prefijo de 92 B). Comprobado hoy: las cuatro cabezas de literal ya no estan en
`GameplayActivity.cpp`, `NISActivity.cpp` ni `LocalPlayer.cpp`, y las cuatro
entradas `@lc` (`$LC478`, `$LC550`, `$LC555`, `$LC626`) estan en `keep.lst`.

Asi que del frente de datos ya estan retirados los 18 bloques de esa tanda; lo
que queda por atacar de la primera capa del plan son los otros ~113 de los 131.
