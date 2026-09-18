# r37 — sonido: `cStichWrapper::Play` y `GinsuSynthData::BindToData`

Fecha: 2026-09-07. Base: `590dc4a0`. Unidades:
`Speed/Indep/SourceLists/zEAXSound` y `zEAXSound2`.

## Resultado

No hay cierre nuevo que contar como bytes o funcion exacta en este frente.
Las dos fuentes quedan restauradas byte a byte al estado de entrada:

| funcion | antes | despues | estado |
|---|---:|---:|---|
| `Play__13cStichWrapperPC10SND_Params` | 384 B / 99,6875 %, 4 filas | igual | fuente intacta |
| `BindToData__14GinsuSynthDataPv` | 344/340 B / 95,882355 %, 12 filas | igual | fuente restaurada |

No se presenta ningun parcial como cierre. `BindToData` sigue midiendo 344 B y
conserva el `mr r8,r5` extra de la base. No se tocaron cabeceras, flags,
configuracion, splits ni ensamblador de cuerpos.

## `cStichWrapper::Play`: hipotesis nueva y veda ampliada

El diff son cuatro operandos de una permutacion `r30`/`r31`: el objetivo quiere
el desplazamiento `i*4` en `r30` y la base `ActiveSamplesRefs` en `r31`; nuestra
forma base obtiene lo contrario. La ronda r36f ya habia medido 40 formas y
demostrado que una local `refs` asignada antes que `off` arregla los registros,
pero adelanta el `addi` a antes del `bl`.

Esta ronda probo 13 formas C distintas, guiadas por tipos y lifetime en vez de
repetir el orden de asignacion de r36f:

- puntero a primer elemento con indice y con aritmetica de punteros;
- referencia al array de 18 elementos y puntero `const`;
- indice `unsigned`, desplazamiento `unsigned` y `size_t`;
- suma conmutada, `&array[0] + i` y resultado del `new` en una local;
- declaraciones separadas de `refs` y del resultado.

Ninguna reduce las cuatro filas. Las formas simples son binarias identicas o
bajan a seis filas; las que materializan el desplazamiento reproducen la familia
E1 de r36f (registros correctos, orden incorrecto); las sumas explicitas
engordan a 392 B y 41 filas. Se eliminaron todos los ficheros hermanos del arnes
y `STICH_PlayBack.cpp` no tiene diff.

## `BindToData`: diagnostico y ensayos

`lmap.py` fija el orden original sin ambiguedad:

- linea 172: `mCurrentBlock = -1`;
- linea 174: `minperiod = mSampleCount`;
- linea 175: declaracion/condicion del `for`.

El DWARF original coloca `i` en el bloque del bucle y en `r5`, `minperiod` en
`r7`, `period` en `r0`, `ptr` en `r30` y no asigna registro a `memdata`. Declarar
`i` en el `for` y fijarla a `r5` reprodujo esas propiedades y mejoro temporalmente
de 12 a 8 filas (95,882355 -> 96,1647 %), pero siguio dejando el pseudo largo de
`-1`, la base equivocada y el `mr` extra. Por ser un pin de regalloc sin cierre,
se retiro antes del checkpoint.

Ensayos retirados:

- mover `mCurrentBlock=-1` despues de `minperiod`: binario neutro y contrario a
  la linea original;
- alias por referencia, puntero, `volatile`, offset de miembro, `signed char` y
  enum para el store: todos neutros;
- fijar el `-1` a `r0`: empeora a 95,752945 % y rota `r8/r10/r11`;
- fijar `cycle_count`, `cycle_pos` o `minperiod`: 336-344 B y 17-23 diferencias;
- local `cycle_pos`, `const`, referencia al campo y usos parciales o totales en
  `mSampleData`/bucle: neutros o 11 filas; no elevan `n_refs` del pseudo que debe
  recibir `r8`;
- grafias de constante, dependencia por coma y reordenamientos lexicales:
  neutros o rechazados por no compilar en el alcance real.

La veda queda mas precisa: desde C puro no se ha encontrado una forma que haga
rematerializable el `-1` en `r0` despues de liberar `sampleCount` sin alterar a
la vez la asignacion de `mCyclePos`. No se conserva ningun alias, pin adicional,
valor fantasma ni barrera de los ensayos.

## Auditoria

- Compilacion directa solamente de `zEAXSound2`; nada de `ninja`.
- `fncmp`: 6 de 930 funciones siguen distintas y los mismos 3.564 B quedan
  pendientes. No aparece ninguna funcion nueva en la lista.
- `scripts/audit.py Speed/Indep/SourceLists/zEAXSound2`: salida 0; todas las
  funciones exactas de la unidad pasan ramas, referencias y literales.
- `scratchpad/codex_r37_sound/audit.py`: 2.802 simbolos no-debug y 23 secciones;
  todas las funciones, todos los datos y todas las reubicaciones quedan
  identicos al objeto inicial. La funcion conserva byte a byte sus 86
  instrucciones, inventario de opcodes, prologo/epilogo ABI y cinco sitios
  externos de referencia.
- Los dos pares de referencias a `$LC984` y `$LC985` mantienen posicion, tipo y
  addend; no hay literales ni datos nuevos.
- 2.100 casos de maquina del bucle cubren `cycleCount <= 0`, conteos positivos y
  restas con wrap signed de 32 bits.
- `scripts/lcfix.py --check`: limpio.
- No quedan ficheros `__codex_r37_sound*` bajo `src/`.

## Artefactos

Directorio `scratchpad/codex_r37_sound/`:

- `before.json`: objeto base compilado desde la forma tracked;
- `after.json`: objeto real final;
- `summary.json`: las 13 variantes nuevas de `cStichWrapper::Play`;
- `bind_summary.json` y `bindbase_summary.json`: variantes de store y lifetime
  de `BindToData`;
- `audit.py`: auditoria de aislamiento, referencias, ABI y semantica del bucle.

Los arneses reproducibles son `scratchpad/codex_r37_sound_variants.py`,
`codex_r37_sound_bind_variants.py` y
`codex_r37_sound_bind_base_variants.py`. Todos escriben una fuente hermana por
variante y la borran en `finally`; no sobrescriben el archivo de trabajo.
