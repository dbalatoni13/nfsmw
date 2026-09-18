# Ronda 37 — zTrack / `GetLoadingPriority`

## Resultado

No se cerro `GetLoadingPriority__13TrackStreamerP21TrackStreamingSectionP22StreamingPositionEntryb`.
El punto de partida real de `HEAD` (`590dc4a0`) era 708 B, 177 instrucciones y
97.18079 %. La premisa antigua del marco `0x120` con `pos2` ya no era vigente:
`9de121ee` elimino ese relleno por no tener respaldo en el DWARF. Esta ronda
conserva el marco honesto de `0x110` y no reintroduce `pos2`.

Se conserva una mejora estructural de fuente, no una ganancia de medicion: el
calculo pasa a expresar por separado el factor angular, su producto con
`speed_factor`, el factor `0.66999996f` y finalmente `adjusted_distance`. El
objeto final mide 708 B y 94.72317 %, por lo que el delta de decompilacion de la
ronda es **0 B / 0 funciones**. No queda ninguna barrera ASM ni pin de registro.

## Evidencia original

`lmap.py` coloca las operaciones del original en lineas fuente separadas:

- 3938: `(90 - clamp(angle)) * 0.014285714f`;
- 3940: producto por `speed_factor`;
- 3942: producto por `0.66999996f`;
- 3945: formacion de `adjusted_distance`;
- 3947: conversion de prioridad con `0.013333334f`.

La forma secuenciada reproduce exactamente esa asociacion de seis operaciones
FP, pero GCC 2.95.3 mantiene otro reparto: el original conserva `speed` y
`speed_factor` en `f31`, la constante uno en `f30` y el acumulador angular pasa
por `f0/f12/f11`; la reconstruccion conserva `speed_factor` en `f30`, uno en
`f31` y el acumulador en `f13`. El DWARF tambien confirma que esos escalares son
locales del ambito de la funcion; introducir bloques artificiales no tiene base.

El marco distinto explica 43 diferencias mecanicas (prologo, epilogo y offsets
de locales). El resto es la rotacion del asignador. Los pines o restricciones
no recuperan esa asignacion de manera legitima.

## Ensayos acotados

| Ensayo | Hipotesis | Resultado |
|---|---|---:|
| baseline | Expresion heredada de `HEAD` | 97.18079 %, 71 filas |
| c1/c13 | Tres productos FP secuenciados, sin barrera | 94.72317 %, 75 filas |
| c2/c3/c6-c8 | Barreras `+f` sobre `speed`, `angle_factor` o dependencia cruzada | identico a c1 |
| c4 | pin `angle_factor` a `fr0` | 94.44068 %, 78 filas |
| c5 | pin `adjusted_distance` a `fr0` | 94.83616 %, 75 filas |
| c9 | pin `speed_factor` a `fr31` | 91.22034 %, 716 B, 95 filas |
| c10 | declaraciones y asignaciones escalares separadas | identico a c1 |
| c11 | pines `speed` y `speed_factor` a `fr31` | 92.12429 %, 704 B, 91 filas |
| c12 | pin `speed` a `fr31` y `adjusted_distance` a `fr0` | 89.58757 %, 712 B, 96 filas |
| c14/c17/c18 | barrera tras productos parciales o finales | 94.75141 %, 75 filas |
| c15/c16 | combinaciones de barreras sobre vida util | sin mejora causal |
| c19/c20 | dependencia input-only de `speed` | 84.09605 %, 189 instrucciones |
| c21 | temporal explicito antes de `bMin` | identico a c17 |

Las restricciones que parecian mover una constante tambien ampliaban vidas
utiles y empeoraban el codigo. Se descartaron todas. No se probaron mas bloques
de ambito porque contradicen el arbol de ambitos del DWARF y no aportan evidencia
nueva.

## Auditoria final

- `build_direct.py zTrack`: correcto.
- Unidad: 63.036/63.744 B, 98.8893 %, 258 funciones exactas (sin cambio).
- Inventario objdiff: 291 funciones emitidas; las otras 290 son byte/relo
  identicas entre el snapshot inicial y el final.
- `fncmp.py Speed/Indep/SourceLists/zTrack`: una unica funcion con codigo
  distinto, la candidata de 708 B; ninguna exacta regresa.
- `audit.py Speed/Indep/SourceLists/zTrack`: todas las 258 exactas correctas.
- `litpos.py`: 178 referencias emparejadas, 0 valores distintos y 0 sin
  resolver.
- Datos sin cambios: `.rodata` 2144 B, `.data` 4428 B, `.bss` 47132 B y
  `.ctors` 4 B, incluidas sus relocaciones.
- `lcfix.py --check`: limpio.

Artefactos principales:

- `scratchpad/codex_r37_track_before.json`
- `scratchpad/codex_r37_track_after.json`
- `scratchpad/codex_r37_track_before_measure.json`
- `scratchpad/codex_r37_track_after_measure.json`
- `scratchpad/codex_r37_track_before_pct.json`
- `scratchpad/codex_r37_track_after_pct.json`
- `scratchpad/codex_r37_track_audit.py`
- `scratchpad/codex_r37_track_c1.json` a `codex_r37_track_c21.json`

## Proxima evidencia necesaria

Para buscar el cierre sin repetir esta tanda hace falta evidencia nueva del
asignador: rangos de vida/registros mas precisos del compilador original o un
local DWARF omitido que justifique el marco `0x120`. Sin eso, los pines y las
barreras solo fabrican presion de registros y no reconstruyen la fuente.
