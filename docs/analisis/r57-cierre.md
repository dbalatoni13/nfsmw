# r57 — cierre

Escrito en la ventana de la r60, recuperando el registro que `serie.py` necesita y
que llevaba cinco rondas sin escribirse.

## Progreso oficial verificado

| medida | antes r57 | despues r57 |
|---|---:|---:|
| unidades completas | 519 / 616 | **519 / 616** |
| bytes de DOL distintos eliminados | — | **1.708.793 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**Cero promociones.**

zPhysics llega a `linkdelta` IGUAL con CUATRO ficheros: 262.003 -> 26.892 B. Aparecen la VENTANA DE 32 BYTES y el cadaver de `size & ~7`.

## Las funciones exactas NO se pueden dar

`report.json` solo se regenera construyendo, y no se ha reconstruido desde el
09-09 a las 21:54. Prefiero dejar la fila vacia a rellenarla con una cifra vieja
disfrazada de actual: es exactamente el fallo que `serie.py` detecta desde la r59.
