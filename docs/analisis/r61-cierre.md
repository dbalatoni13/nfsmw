# r61 — cierre

## Progreso oficial verificado

| medida | antes r61 | despues r61 |
|---|---:|---:|
| unidades completas | 522 / 616 | **523 / 617** |
| bytes de DOL distintos eliminados | — | **14.803 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**PROMOCIONA: inittmr**, y es la PRIMERA unidad que cierra por ATRIBUCION en vez
de por codigo: `fncmp` daba 0 de 4 desde siempre y lo unico que le faltaba era
una raya de `splits.txt`, que salio de `.debug_aranges`.

## La ronda de los decisores

Cuatro ensayos baratos delante, que respondieron por 19.952 B de historial:

| familia | veredicto | lo que decidio |
|---|---|---|
| D — la base envenenada | **MUERTA** | los andamios NO envenenan, SOSTIENEN: quitarlos da 23 filas contra 14 y 60 contra 28, y los cuatro numeros reproducen la tabla de la r53 sin desviacion. **Los ~200 negativos historicos siguen valiendo.** |
| A — `qty_const` de cse2 | VIVA | una sola linea de `-dG`: `COPY-PROP: Replacing reg 625 in insn 1210 with reg 753`, con 753 el pseudo de un `high`. La r54 midio CERO en todas sus variantes. |
| F — orden del pool | VIVA | |
| E — Metrowerks | VIVA | |

## `.debug_aranges`: la seccion que nadie habia minado

18.576 B que **son `splits.txt`**: por unidad de compilacion, `(TEXT_BEGIN, len)`,
`(DATA_BEGIN, len)`, `(RODATA_BEGIN, len)`, `(BSS_BEGIN, len)`, mas un par por
cada variable publica con definicion tentativa (`dwarfout.c:6504-6514` y
`:7344-7385`). **316 unidades. Control: 299 de 300 inicios de `.text` coinciden
AL BYTE con `splits.txt`.** Herramientas nuevas: `scripts/aranges.py` y
`scripts/pubnames.py` (21.431 direcciones -> unidad, control 187 de 187).

## Los dos paquetes aplicados

- **P1**, cuatro rayas de `.bss`: `zEAXSound` pasa de `bss+32` a **IGUAL**, y con
  las secciones alineadas `dolwhere` ARRANCA por primera vez en esa unidad.
  Leccion: el informe predijo DOS huecos de cabecera con nombre `gap_`; eran
  **TRES** y se llaman `pad_`. Con dos de tres, `DOL ROTO` y `.bss -96`.
- **P2**, `inittmr`: cuatro ficheros y con tres de los cuatro el DOL rompe. Los
  144 B de assert de Metrowerks no los referencia nadie y `-strip-unused-data` se
  los llevaba (`.rodata -124`).

## Trampa nueva

Hay ficheros con **DOS entradas en `configure.py`**, una con ruta completa y otra
con nombre suelto. **Manda la de ruta completa**; marcando la otra, `configure.py`
no protesta y `build.ninja` sigue diciendo `linked False`.
