# r39 — zWorld2

## Resultado

No se cierra ninguna función y no se conserva ningún cambio de código. El
objeto final fue recompilado por integración desde la fuente restaurada y
reproduce exactamente la referencia de entrada:

| función | tamaño | entrada/final | `fncmp` final |
|---|---:|---:|---:|
| `HolePunchAvoidables__8WRoadNavP9NavCookieiff` | 2.980 B | 98,16913 % | 44 instrucciones |
| `InitAtSegment__8WRoadNavscf` | 816 B | 99,55882 % | 15 instrucciones |

`fncmp --todas` encuentra únicamente esas dos funciones distintas: 3.796 B de
357 funciones. `pctsnap --cmp --umbral 0` sobre las 357 da cero mejoras y cero
regresiones. Los artefactos compactos están en
`scratchpad/codex_r39_world2/{before_pct,after_pct}.json`.

## Hipótesis comprobada

El frente elegido fue `InitAtSegment`. La r36f dejó una variante de tres filas
de `fndiff`, pero su único `mr r4,r25` quedaba diecisiete ranuras tarde y por eso
empeoraba las métricas reales. La hipótesis nueva era que una referencia de sólo
entrada sobre `segment`, en vez del `+r` que fabrica un pseudo de salida, podía
subir sus referencias/vida y obtener r25 sin encadenar la copia del argumento a
la barrera.

No ocurre. Con el pin de `segment` retirado, el arreglo conocido de `fNodes`
activo y `asm("" : : "r"(segment))` antes de `GetSegmentNodes`, el reparto se
desordena hasta 89,583336 % y 62 instrucciones distintas. Sin esa referencia se
reproduce el estado N2 de r36f: 96,93137 % y 12 instrucciones distintas. El
`+r(segment)` anterior a la llamada reproduce asimismo el suelo conocido:
98,97059 %, tres filas de alineación pero 19 instrucciones distintas. La
hipótesis queda agotada y todas estas formas fueron retiradas.

## Ensayos estructurales acotados

Se comprobó además si expresar explícitamente el cálculo que ya hace
`GetSegment(segInd)` separaba el índice, la base `fSegments` y el resultado como
en las tres instrucciones objetivo (`mulli r8`; `lwz r10`; `add r25,r10,r8`).
La versión semánticamente fiel con registros r8/r10/r25 sí cierra ese primer
racimo de tres instrucciones, pero el bloque de `GetSegmentNodes` queda en el
estado N2 (96,93137 %, 12 instrucciones). Al añadir el `+r(segment)` vuelve al
suelo de tres filas/19 instrucciones, sin cierre.

Por último, se nombraron los tres argumentos ABI de `GetSegmentNodes` en r3,
r4 y r5, sobre la forma anterior. Quedan los tres `mr` correctos pero todos
exactamente dos ranuras tarde; el resultado es 97,62255 % y 13 instrucciones
distintas. Esto confirma que la cola es una decisión de prioridad de `sched2`,
no un ABI o una instrucción ausente. La continuación con dos dependencias
vacías separadas se midió después de la primera integración y queda agotada en
el apartado siguiente.

También fueron negativos mover el `+r(segment)` antes de `fStartPos`
(96,117645 %, 28 instrucciones), fijar sólo la base de `fSegments` a r10
(96,882355 %, 14) y añadir una barrera sobre esa base (95,90196 %, 16).

## Seguimiento acotado: doble dependencia separada

Se revalidó primero el baseline (99,55882 %, 15 instrucciones), se reconstruyó
la receta r8/r10/r25 + `fNodes` + argumentos ABI y se probaron exactamente seis
variantes. Todas mantuvieron 816/816 B:

| segunda dependencia, después del `asm` ABI triple | porcentaje | instrucciones distintas |
|---|---:|---:|
| otro `asm` `+r` sobre r3/r4/r5 | 96,81863 % | 24 |
| `+r(networkArg)` | 96,81863 % | 24 |
| `+r(networkArg)` con `nodes` como entrada | 95,338234 % | 23 |
| sólo entrada sobre r3/r4/r5 | 92,34314 % | 52 |
| `+r(nodesArg)` | 96,81863 % | 24 |
| `+r(segmentArg)` | 96,81863 % | 24 |

La evidencia causal es uniforme: cualquier segunda salida vuelve a permutar
`fNodes` de r10/r11 y altera el orden `addi`/stores antes de conseguir adelantar
la terna. Que r3, r4 o r5 produzcan el mismo objeto demuestra que el salto viene
del `n_refs`/rango añadido al bloque ABI, no del argumento elegido. La variante
de sólo entrada aumenta todavía más la presión y derrama/reparte de nuevo el
bloque posterior. No hay combinación selectiva dentro de esta hipótesis que
conserve el racimo ya correcto, así que las seis se retiraron.

Tras retirar el seguimiento se recompiló otra vez `zWorld2`: las dos funciones,
las 357 entradas de `pctsnap`, los datos y el blob de fuente vuelven exactamente
al baseline descrito arriba.

## Auditoría final

- `git diff --quiet -- WRoadNetwork.cpp`: 0. El blob filtrado del árbol y el de
  HEAD coinciden: `dafa04ecf98a3192291e1cfbe0a814f55bdeae12`. El estado `.M`
  que muestra Git es únicamente el stat/EOL de la reescritura; no hay delta de
  contenido y el fichero conserva CRLF.
- `scripts/audit.py Speed/Indep/SourceLists/zWorld2`: 355/355 funciones ya
  exactas pasan bytes, destinos de rama, relocaciones y literales.
- Datos actuales, sin cambio respecto a la fuente de entrada: `.bss`
  1.264/1.424 B, `.data` 396/140 B y `.rodata` 4.560/4.168 B. Son las diferencias
  persistentes ya documentadas; ningún ensayo quedó en el objeto final.
- `scripts/lcfix.py --check`: limpio.

No se tocó la SourceList, cabeceras, configuración, splits ni flags; no se lanzó
`ninja`, no se creó commit y no quedan compilaciones ni escrituras en curso.
