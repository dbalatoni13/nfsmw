# `zMisc` y `zGameModes`: la receta del vocabulario no aplica, y por qué

Las dos son el caso **inverso** al que cierra la receta: no les falta `.rodata`,
les **sobra**. Medido antes de tocar nada:

```
zMisc       .text +0   rodata+2840  data+32     cadenas que faltan: 0
zGameModes  .text +0   rodata+104               sin rango de .rodata en splits.txt
```

La receta —pegar al final del `.cpp` las cadenas que el objetivo tiene y
`cc1plus` no emite— **no tiene nada que añadir en `zMisc`** (le faltan cero) y
**no tiene dónde ponerlo en `zGameModes`** (no hay rango). Aplicarla habría
empeorado la primera y no habría hecho nada en la segunda.

---

## 1. `zGameModes`: la propuesta de `splits.txt` de la r50 queda REFUTADA

La r50 propuso mover la frontera para dar a `zGameModes` el rango
`0x803EBB48..0x803EBE90` —840 B que hoy tiene `zGameplay`— con este argumento:
las cadenas que hay ahí (`Embarrassed`…`Final`, `EmotionManagerImpl`,
`EmotionalResponse`) son **exactamente** las que `zGameModes` emite.

El argumento es bueno pero no basta, y ahora hay medida en contra.

**Primera comprobación, `STT_FILE`**: `zGameModes.cpp` existe como entrada del
ELF original y tiene **4 locales**, ninguna en `0x803EB000..0x803EE000`.
`zGameplay.cpp` tiene 70, y sus cinco de esa zona están todas en `0x803ED2AC` o
más arriba. O sea que la zona disputada **no está atribuida a ninguna de las
dos**: son literales anónimos y `dtk` sólo nombra lo referenciado.

**Segunda, y ésta decide**: el objeto EXTRAÍDO de `zGameModes` tiene

```
UNDEF (1):    lbl_803EBE90
define (5):   _GLOBAL_.I.aEmotionalSummaryTypeStrings,
              __static_initialization_and_destruction_0,
              kFloatScaleDown, kFloatScaleUp, zGameModes.cpp
```

**Un solo UNDEF, y es `lbl_803EBE90` — la dirección justo DESPUÉS de la zona
disputada.** En el binario original, `zGameModes` no referencia ni uno de esos
840 B. Si fueran suyos, algo de su código apuntaría dentro.

**Conclusión: no se mueve la frontera.** Lo que pasa es lo que la propia r50
explicó al byte por otra vía: el `+104` es la granularidad del estripador
—emitimos 232 B, `size & ~7` se lleva 128, quedan las colas `size & 7` más 32 B
de relleno de alineación que no pertenece a ningún símbolo—. **No hay nada que
escribir ni que quitar.**

---

## 2. `zMisc`: los 2.840 B de superávit son 150 cadenas emitidas DOS VECES

`dupstr.py` las cuenta: **150 cadenas de más, 2.186 B** de los 2.840. Y la
columna de símbolos dice de qué va:

```
29 B  x2 (objetivo x1)  'GameFlowLoadingFrontEndPart1'  ['-', '$LC519']
29 B  x2 (objetivo x1)  'GLOBAL\WIDESCREEN_GLOBAL.BUN'  ['-', '$LC511']
28 B  x2 (objetivo x1)  'RenderConn::UpdateServices()'  ['-', '$LC438']
```

El `-` es una copia **sin símbolo propio**: está dentro del pool que `zMisc.cpp`
escribe a mano en las líneas 50-3393 (450 símbolos, 8.152 B). La otra es el
`$LC` que `cc1plus` emite porque el literal está en el código.

### Cuál de las dos sobra — y la respuesta no es la intuitiva

Habría sido natural quitarlas del bloque a mano. **Es al revés.** El objetivo
tiene cada una **una sola vez y dentro de la zona del bloque**:

| cadena | offset en la `.rodata` del objetivo | |
|---|---|---|
| `RenderConn::UpdateServices()` | `+0x7E4` | dentro del bloque |
| `ServiceResourceLoading` | `+0x8A8` | dentro |
| `GLOBAL\WIDESCREEN_GLOBAL.BUN` | `+0x10D4` | dentro |
| `GameFlowLoadingFrontEndPart1` | `+0x11C4` | dentro |

La `.rodata` entera de `zMisc` en el objetivo mide **7.864 B**
(`0x803F4878..0x803F6730`), y nuestro bloque a mano mide **8.152**: es más
grande que toda la sección del original.

**Así que la copia que sobra es el `$LC`, y la palanca es hacer que el código no
cree el literal**, apuntando al bloque — que es justo lo que `zMisc.cpp` ya hace
en sus líneas 3397-3406:

```c
extern const char _bwarePrefix[] asm("lbl_803F4878");
#define BWARE_PREFIX_BADALLOC (_bwarePrefix + 0x34)
#define BWARE_PREFIX_STL      (_bwarePrefix + 0x58)
#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)
```

Ya existe el mecanismo y ya está usado tres veces. Lo que falta es aplicarlo a
las 150 restantes, y **es transcripción, no investigación**: `dupstr.py` da la
lista y el offset de cada una sale de buscar sus bytes en la `.rodata` del
objetivo. Las cadenas vienen de ficheros distintos (`Misc/GameFlow.cpp`,
`Misc/Main.cpp`…), así que **no vale una guarda común** como
`ATTRIB_TAGS_HAND_POOL`: hay que ir una a una.

**Encargo listo para una ronda**: 2.186 B en 150 cadenas, con la lista y el
método hechos. Y los 654 B que quedarían (2.840 − 2.186) son el bloque a mano,
que mide 288 B más que la sección entera del original.
