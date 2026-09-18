# r62 — cierre

## Progreso oficial verificado

| medida | antes r62 | despues r62 |
|---|---:|---:|
| unidades completas | 523 / 617 | 523 / 617 |
| bytes de DOL distintos eliminados | — | **350.425 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**CERO promociones**, y aun asi es una de las rondas mas utiles de la serie,
porque el encargo de configuracion **refuto la premisa con la que se lanzo**.

## Lo que refuta, y es del jefe

1. **«El frente de atribucion es el mas rentable» era FALSO.** La premisa venia
   de que `inittmr` cerro con una raya de `splits.txt`. Medido: las nueve
   unidades sin codigo pendiente estan a **9.152 B (zLua) y 244.740 B (zAI)** del
   DOL, y las fronteras en disputa miden **8 a 216 B**. `inittmr` era el caso
   donde la unidad entera cabia dentro del error de atribucion; las SourceLists
   no.
2. **El paquete P3 de la mineria es CONTRAPRODUCENTE, y estuvo a punto de
   aplicarse.** De sus 32 fronteras «neutras», 14 son `NonMatching->NonMatching`
   y **las 14 ABREN un deficit del tamaño exacto de la frontera**, porque
   nuestra `.data` empieza EXACTAMENTE en la raya de hoy en **19 de 24**
   SourceLists. Medido: zEAXSound `.data` de +4 a -80, zPlatform de +4 a -132 y
   `.bss` de +64 a -48. Y las 6 `Matching->Matching` **romperian seis unidades ya
   promocionadas sin que el DOL avise**.

## El invariante nuevo

**EL ORDEN DE ENLACE ES EL ORDEN DE `splits.txt`**, 545 de 545 unidades. O sea
que colocar un bloque en `splits.txt` **es** decidir su posicion de enlace. Y uno
que `checksplits` no comprueba: toda unidad de `splits.txt` tiene su `Object()`
en `configure.py` (545 de 545); al reves no --76 `Object()` sin bloque, y esos no
enlazan--.

## Reparto

| encargo | bytes | nota |
|---|---:|---|
| cam | 302.862 | **zCamera de 333.533 a 30.671**, el mayor salto de una unidad de la serie |
| eax | 18.362 | zEAXSound sigue con `fncmp` 0 de 773 |
| ecs | 12.436 | |
| eagl | 9.510 | el `.text` de zEagl4Anim es PERMUTACION PURA: 318 contra 318 |
| world | 4.667 | |
| resto | 2.420 | |
| phys | 168 | |
| p3 | 0 | el que refuta (arriba) |

## Lo que abre

**`spchdata.c`**: existe en el ELF pero no en `splits.txt`. Dos lineas, una de
`configure.py` y una fuente de 20 definiciones. Sube el techo de 544 a **545**.
