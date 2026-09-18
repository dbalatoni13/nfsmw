# `zMisc`: por qué no se puede promocionar — 78.008 B de `linked`

`zMisc` casa al **100 % en `matched_code`** y es **la mayor unidad bloqueada del
árbol**. Diagnóstico completo, medido el 2-sep-2026.

## Lo que dice `promote.py`

```
.text    86.368 B  contra  78.008 del extraído   (+8.360)
.bss      2.560 B  contra   2.192                (+368)
.rodata  11.968 B  contra   7.864                (+4.104)
.data     2.184 B  contra   2.164                (+20)
exportamos 106 símbolos de más
953 símbolos en otro sitio de su sección
```

## La causa principal: instanciaciones de plantilla que el original hace en zAI

De los **106 símbolos de más**, los que se han inspeccionado son
**instanciaciones de plantilla emitidas como `WEAK`**:

```
Get__CQ26Attribt7TAttrib1ZQ25UMath7Vector4Ui
__lower_bound__H4ZPCUiZUiZQ24_STLt4less1ZUiZi_...
find__H2ZPP8IPursuitZP8IPursuit_...
find__H2ZPP10IRoadBlockZP10IRoadBlock_...
find__H2ZPP13IVehicleCacheZP13IVehicleCache_...
```

**40 de los 106 existen en el DOL** — y `splits.txt` los sitúa en
**`zAI.cpp`** (0x8003A9F4..0x8003B4F8). O sea: en el original esas
instanciaciones viven en zAI y **nuestro zMisc emite una copia `WEAK`**.

**Los otros 66 no existen en ninguna parte del DOL**: son instanciaciones que el
original **nunca hizo**.

Ser `WEAK` explica por qué el enlace no rompe (el enlazador se queda con una) y
por qué `matched_code` no lo ve — pero engordan `.text` 8.360 B y **eso sí
bloquea la promoción**.

## Lo que ya está descartado

- **No hay ficheros `.cpp` compartidos entre SourceLists**: comprobado sobre las
  33 listas, **cero** duplicados. zMisc incluye 28 ficheros y ninguno está en otra.
- **No es orden de enlace**: es composición de la unidad de traducción.

## Lo secundario, también medido

- **`.bss` +368 B**: nos falta `bFileMutex` (28 B, el extraído lo pone en +2160) y
  llevamos ~400 B de más antes de `LastQueuedFilename` (extraído +2060, nuestro
  +2460). Entre los nuestros que el extraído no tiene hay globales que en el
  original viven en otra unidad (`bShakeTest`, `PrintChunks`, `WorldTimer`,
  `LimitMinimumVideoTimeElapsed`).
  **Aviso**: los 73 símbolos «de más» que canta un diff crudo **no** son señal —
  son estáticos de función con sufijo `DECL_UID` que el troceador no nombra.
  **La señal es el tamaño, no la cuenta.**
- **953 símbolos en otro sitio de su sección**: casi todos `$LC…` de `.rodata`,
  consecuencia del `.rodata` +4.104 B, no causa independiente.

## Por dónde atacarlo

1. **Averiguar qué `.cpp` de zMisc fuerza cada instanciación.** Compilar los 28
   ficheros por separado y ver cuál emite `find__H2ZPP8IPursuit…` es un barrido de
   minutos y señala el fichero exacto.
2. Si el original no instancia ahí, la diferencia estará en **qué llama ese
   fichero** (una plantilla usada donde el original usaba un bucle a mano, o al
   revés), o en una **cabecera que arrastra un `#include` de más**.
3. El `.rodata` +4.104 B probablemente cae solo al arreglar lo anterior: son los
   `$LC` de las funciones de plantilla duplicadas.

**No es trabajo de ventana**: es análisis de fuente y cabe en una tanda de agente,
pero necesita a alguien con zMisc **y** zAI a la vez, porque hay que comprobar que
quitar la instanciación de zMisc no se la quita a zAI.
