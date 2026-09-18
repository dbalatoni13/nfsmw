# r56 — cierre

Escrito en la ventana de la r60, recuperando el registro que `serie.py` necesita y
que llevaba cinco rondas sin escribirse.

## Progreso oficial verificado

| medida | antes r56 | despues r56 |
|---|---:|---:|
| unidades completas | 519 / 616 | **519 / 616** |
| bytes de DOL distintos eliminados | — | **305.705 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**Cero promociones.**

Orden de emision dentro de cada unidad. zPhysics baja su ciclo mayor de permutacion de 433 a 101. Se descubre que build_direct.py puede dejar un .o que no corresponde a la fuente.

## Las funciones exactas NO se pueden dar

`report.json` solo se regenera construyendo, y no se ha reconstruido desde el
09-09 a las 21:54. Prefiero dejar la fila vacia a rellenarla con una cifra vieja
disfrazada de actual: es exactamente el fallo que `serie.py` detecta desde la r59.
