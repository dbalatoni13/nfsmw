# R47 — cierre

2026-09-09. Seis agentes en paralelo sobre los 42.308 B pendientes completos,
más la ventana. DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`
después de cada paso.

## Estado

| medida | r46 | ahora |
|---|---:|---:|
| código exacto | 3.903.740 / 3.946.048 B — 98,93 % | **3.905.708 — 98,98 %** |
| funciones exactas | 18.389 / 18.432 | **18.394** |
| unidades linked | 492 / 619 | **493 / 619** |

Quedan **40.340 B en 37 funciones de 17 unidades** —de 42 funciones y 19
unidades—. `zEAXSound` y `spchsamp` quedan **sin ninguna función pendiente**;
`zFeOverlay` a una sola.

## Cerrado: cinco funciones, 2.712 B

| unidad | función | B |
|---|---|---:|
| `zEagl4Anim` | `FnRunBlender::FindMatchTime` | 720 |
| `pathnode` | `PATHI_calcwaitbeat` | 336 |
| `spchsamp` | `iSPCH_GetSampleSizeData` | 136 |
| `zEAXSound` | `cStichWrapper::Play` | 384 |
| `zEAXSound2` | `Play__16CARSFX_RoadNoise` | 392 |

Y dos avances estructurales grandes sin llegar a cero: `HolePunchAvoidables`
de 9 filas a **4** (99,97315 %) y `ActualReadJoystickData` de 19 a **16**.

## Las palancas nuevas

### La conversión que se plegó pero dejó su `lfd`

Generaliza el área X de la r46, y es más fina. Los subproductos de una
conversión int↔float se reservan al **expandir** y sobreviven a la muerte de su
propio código: no sólo el temporal de pila, también el `lfd` del literal DFmode
del sesgo `0x4330000080000000`. Lo delata `lmap.py` **sobre el original**, que
imputaba ese `lfd` a una sentencia `n = FloatToInt(...)` donde no hay ninguna
otra instrucción de conversión. Se reproduce multiplicando por una variable
entera que siempre vale 1: la conversión se expande, `-ffast-math` pliega el
producto y queda sólo el `lfd`. Cero bytes emitidos, cero `asm`.

**Barrido y frente cerrado**: contadas `0x4330`/`lfd`/`fctiwz`/`fsub` en los dos
lados de las 42 pendientes, el conteo **cuadra en las 42**. `FindMatchTime` era
el único caso.

### El DWARF trae los TIPOS, no sólo las locales

`ActualReadJoystickData` llevaba **19 formas de fuente medidas en cuatro
rondas**. En el volcado estaba entera la `struct JoyData` con sus offsets, y con
ella se ve que `slot` indexa un **array miembro**, no un puntero casteado. Un
array miembro expande la dirección como `(plus base offset)` —la forma del
objetivo—; el cast no. Y es la pertenencia lo que decide: `((LGPosition*)p)[slot].X`
deja exactamente las mismas 19 filas.

**Regla barrible**: si el objetivo pone la **base primero** en un `add` o en un
store indexado y nosotros el índice, la fuente original indexa un array miembro.

### `asm("" : : "r"(x))` de SOLO ENTRADA no emite un byte

Y sube `n_refs`. La veda de la r36f —«cualquier `asm` en ese bucle cuesta 4 B»—
valía sólo para los `asm` **con salida**. Cerró `cStichWrapper::Play`, la veda
de cinco rondas. Conviene rehacer con esta forma los near-miss de reparto que
se cerraron en falso con `"+r"`.

### `asm("")` a secas rompe empates exactos

En `Play__16CARSFX_RoadNoise` los dos allocnos **empataban exactamente a 1.111**
en `allocno_compare` y el desempate por número caía del lado malo; una ranura
de cero bytes los pone en 1.071 contra 1.095.

### `set_preference` encadena locales, y manda la SEGUNDA

`global.c` hace `src = XEXP(src,0)` para un `src` de formato `'e'`, así que de
un `(set oc (mult adelta approach))` el allocno de `adelta` **hereda** el
registro que `local_alloc` dio a `oc`. En `HolePunchAvoidables` eso significaba
que había que pinchar **la carga que le quita el registro**, no la variable que
sale en el diff. Cero `asm` netos.

## Diagnósticos que estaban del revés

- **`spchsamp`**: tres rondas la trataron como reparto. El `.greg` dice
  `;; 89 conflicts: … 118 119 0 1` — `result` chocaba con r0 **porque
  `result = 1` se planificaba en la sombra de latencia del `mullw`**.
- **`PATHI_calcwaitbeat`**: dada por «permutador puro» dos rondas. Mover un pin
  existente de fr11 a fr10 no mejoró el porcentaje pero convirtió una rotación
  de cuatro registros en un **swap limpio**, y entonces el pin que faltaba salió
  solo.
- **`sfir`**: no es un `if` defensivo ni reparto. Es **PRE**, y el volcado lo
  imprime (`PRE: redundant insn 533 (expression 7)`). El interruptor está en
  `gcse.c:737` y `:1884`. Medido: `-Os` cuadra exactamente `addi`/`mr`/`slwi`
  con el objetivo pero rompe el reparto flotante (924 B / 57,98 %).
- **`Initialize`** (zEagl4Anim): el `ble` no lo emite `emit_case_nodes` sino
  **`jump.c:1831`**, la única transformación de GCC 2.95 que permuta dos bloques
  básicos e invierte la condición. Su condición exige que el nodo `{2}` sea
  acotado o un rango, y ninguna se da sin cambiar la semántica. **Inalcanzable**,
  con 52 barridos detrás.

## Correcciones al catálogo

1. **`REG_ALLOC_ORDER` de rs6000 NO empieza por r31**: es
   `0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30, …` (`rs6000.h:932`). El «31 primero»
   sólo aparece en `global_alloc` para allocnos que cruzan llamadas.
2. **El DWARF también se equivoca**: en `PATHI_calcwaitbeat` da `scalar // f11`
   **y** `fevery // f11`, y sólo la primera es cierta.
3. **La «veta viva» del presupuesto de `cse` de zCamera no existe.** Yo la pasé
   en el brief sin verificarla. Las 70 «baratas» son `(mem/u:SF (lo_sum …))`
   —cargas del *valor* de un literal— y las 140 «caras» son
   `(set (reg:SI) (lo_sum …))` sobre tablas y cadenas: **materializaciones de
   dirección**. No son dos formas del mismo constructo.
4. **`sizeof(FloatVector)` no explica el marco de `GetLoadingPriority`.** El
   DWARF dice `total size: 0xC`. Y el `face[4]` que parecía solaparse no es local
   de esa función: viene de un inline, y dos locales de bloques léxicos disjuntos
   **comparten ranura de pila por diseño**.
5. **`steering` es UNA sola unidad de traducción**: 28 versiones de compilador
   —las 19 de `GC` más las 9 de `Wii`, nunca barridas— y los conjuntos de fallos
   salen **anidados, nunca disjuntos**. No admite el corte de `OdemuExi2`.

## La ventana

- **`spchsamp` promociona** (184 B de `linked`). `trypromo` DOL OK.
- **`zEAXSound` NO puede promocionar** pese a quedar con cero funciones
  pendientes: exportamos **229 símbolos de más** y su `.text` mide 173.572 B
  contra los 151.092 del extraído. Es el frente de `-strip-unused`, no un
  problema de código. Es el recordatorio de que `matched_code == total_code`
  no implica que la unidad sea enlazable.

## Herramientas

- **`scripts/rtldump.py`** (nuevo): volcados `.greg`/`.lreg`/`.combine` de una
  unidad o de un `.cpp` suelto. Promovido del arnés de `world` **quitándole la
  ruta del repo a fuego**, que es la trampa que ya costó una ronda.
- **`scripts/fncmp.py` aprende a leer `.over`**: `zFeOverlay` y `zOnline` son
  los dos únicos objetos extraídos cuyo código no va en `.text`, y llevaban
  rondas invisibles al triaje que reparte el trabajo. No basta cambiar el nombre
  de la sección —así salen 404 falsos positivos de 467—: hay que generalizar
  también las reubicaciones. Resultado: **a `zFeOverlay` le falta 1 función de
  467**.

## Una regresión mía, reparada

En `e66b11e2` comprometí, dentro del trabajo de las iteraciones del usuario, la
retirada de `float slipBoost = 0.1f` de `GenerateRoadNoise` —una de las tres
locales que el DWARF **no lista** y que aun así hacen falta—. La función pasaba
de 1.240 B exactos a 1.232. No lo vi porque verifiqué el `matched` global y el
DOL, y **ninguno de los dos se mueve cuando una función ya pendiente empeora**.
El aviso estaba en el censo (el fuzzy bajaba de 97,1129 a 96,6097 %).

## Los dos encargos más rentables para la r48

1. **`pathi.h`**, el inline `PATHI_getnode` compartido por 13 unidades: hay que
   dar **más de dos referencias** al pseudo de `Path::pfstate` que va del inline
   al `pfstate->track` del llamante. Vale **4.204 B de `linked`** y es lo único
   que le falta a `pathnode`.
2. **`SetScreenNames`** (440 B): son 6 filas, y el `asm "+m"` que ya lleva
   reutiliza el pseudo de `high(g_pCustomizeSubPkg)`, subiéndole las refs de 2 a
   4 y el rango a 54, con lo que lo reparte el primero y se come los diez GPR
   previos a r30. 18 formas alternativas medidas, todas peores. Cerrarla
   promociona los **141.224 B** de `zFeOverlay`.
