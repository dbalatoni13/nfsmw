# r63 — cierre

## Progreso oficial verificado

| medida | antes r63 | despues r63 |
|---|---:|---:|
| unidades completas | 523 / 617 | **525 / 619** |
| techo alcanzable | 544 | **545** |
| bytes de DOL distintos eliminados | — | **311.196 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**DOS promociones, y el techo se mueve por primera vez en toda la serie.**

## `criticalpath` — cerrada

`fncmp` **0 de 21**. Sello triple del `.o` (296d23bc6d4e las tres) y `trypromo`
DOL OK las tres. Era la unidad a 163 B, y los 163 estaban dentro de
`VP6_PredictFilteredBlock`. Su `textorder` ya daba ORDEN PERFECTO.

Y refuta la hipotesis que arrastraba: **el conteo de llamadas sale 4 y 4** --
nuestra fuente tiene CUATRO `FilterBlock` y el compilador las funde en una.

## `spchdata.c` — unidad NUEVA, techo 544 -> 545

No existia en `splits.txt`, asi que sus datos vivian en dos comodines que no
pueden enlazar nunca. Fuente nueva + 2 lineas de `splits.txt` + 1 de
`configure.py`. Cero huecos `pad_`. No tiene NI UNA instruccion: el original le
da `.text` de longitud cero.

**Y el hallazgo de METODO de la ronda: como verificar una promocion que necesita
re-extraccion, SIN re-extraer.** Cirugia ELF sobre los comodines, armar la `.rsp`
como quedaria despues, y enlazar con el mismo `ngcld` y el mismo `dtk elf2dol`.
Control 9619ba57, prueba DOL OK. Al aplicarlo de verdad cuadro a la primera: 619
objetos y los dos comodines nuevos con los tamaños predichos.

## La palanca del orden de parseo, cuantificada

Tau de Kendall contra el orden del objetivo: **PARSEO +0,953 / +0,998 / +0,989 /
+0,938 / +1,000** contra **grafo de llamadas +0,672 / +0,529 / +0,541 / +0,418 /
+0,469**. Y el mecanismo: **460 de las 718 funciones de zPhysics (64,1 %) no
tienen ni una `R_PPC_REL24`** -- 407 solo las referencia una vtable. Un grafo no
puede ordenar lo que nadie llama.

`zPlatform`: 134 descolocadas de 137 con 85 saltos -> **0 y 0**, con el `.text`
clavado.

## El mecanismo que destapa para la r64

**Los saltos que quedan estan TODOS en el BLOQUE DIFERIDO de `finish_file`, no en
el codigo normal.** En zPhysics los rangos 0..358 ya estaban a +0. Y en zCamera
los 19 saltos cerrados **no los movio ningun `#include`**: los movio el orden
dentro del diferido. Son dos mecanismos distintos.

## Correccion de metrica

**`textorder` no siempre es la cifra que manda.** En zAI no se movio (359
descolocadas antes y despues) mientras la distancia real bajaba un **78 %**
(244.740 -> 53.874 B). Y sus «descolocadas» pueden ser fantasma: `textorder`
compara el `.o`, y en zEagl4Anim emitimos 106 simbolos que el enlace tira.

## El negativo mas caro

**Incluir `spchi.h` rompe el orden del `.bss` EN SILENCIO**: adelanta
`gEventDats` al desplazamiento 0 y empuja `gCallbacks` a 64. Y la `.data` sale
PERFECTA en ese mismo intento, asi que no hay ninguna señal si solo se mira
`.data`. Anotado en la cabecera de `spchdata.c`.
