# r55 — cierre

Escrito en la ventana de la r60, recuperando el registro que `serie.py` necesita y
que llevaba cinco rondas sin escribirse.

## Progreso oficial verificado

| medida | antes r55 | despues r55 |
|---|---:|---:|
| unidades completas | 518 / 616 | **519 / 616** |
| bytes de DOL distintos eliminados | — | **2.014.611 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**PROMOCIONA: zGameModes.**

La frontera de splits que la desbloqueo y el prefijo de 540 B. Ademas: WCollisionAssets.h quito 1.447.438 B de DOL en tres unidades al dejar de instanciar un COMDAT de 120 B en toda TU que la incluyera.

## Las funciones exactas NO se pueden dar

`report.json` solo se regenera construyendo, y no se ha reconstruido desde el
09-09 a las 21:54. Prefiero dejar la fila vacia a rellenarla con una cifra vieja
disfrazada de actual: es exactamente el fallo que `serie.py` detecta desde la r59.
