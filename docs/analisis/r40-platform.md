# r40 — zPlatform: auditoria de `ActualReadJoystickData`

Fecha: 2026-09-07. Territorio exclusivo:
`src/Speed/GameCube/Src/JoyE.cpp`.

## Resultado

No hay cierre ni parcial retenido. La fuente y el objeto final quedan
identicos al baseline de entrada:

| funcion | objetivo | fuente | similitud |
|---|---:|---:|---:|
| `ActualReadJoystickData__Fv` | 1.588 B | 1.580 B | 99,24433 % |

`zPlatform` conserva 135/136 funciones exactas y una pendiente de 1.588 B.
Ninguna de las otras 135 funciones, los datos, las secciones o las
reubicaciones cambia.

## Los cinco `mr` no son consumidores reales

El objetivo tiene, despues del store de `stickX`:

```text
mr     r9,r0
stb    r0,4(r29)
andi.  r11,r9,0x8000
mr     r9,r11
mr     r10,r11
mr     r8,r11
mr     r6,r11
mr     r7,r11
```

El mapa de lineas atribuye `mr r9,r0` y `andi.` a `JoyE.cpp:307`, la ultima
sentencia del ambito de la unica local original `short data`. Las copias
posteriores aparecen en las expansiones de conversion de las lineas 308, 317
y 318. Sin embargo, el seguimiento de cada GPR demuestra que ninguna consume
el resultado en la semantica de la funcion:

| copia | siguiente mencion del destino |
|---|---|
| `mr r9,r11` | `lwz r9,0x14(r1)`; sobrescribe antes de leer |
| `mr r10,r11` | `mulli r10,r26,0xa`; sobrescribe antes de leer |
| `mr r8,r11` | `lwz r8,0x14(r1)`; sobrescribe antes de leer |
| `mr r6,r11` | no vuelve a aparecer |
| `mr r7,r11` | `lwz r7,0x14(r1)`; sobrescribe antes de leer |

Esto confirma el diagnostico RTL de r31: las cinco copias proceden de los
`UNSPEC`/`*floatsidf2_loadaddr` de las conversiones y que compartan `r11` con
el `andi.` es una coincidencia de asignacion. No existe un uso C real que pueda
reconstruirse. Inventarlo solo para conservar las dos instrucciones seria un
valor fantasma y queda expresamente descartado.

## Hipotesis nuevas medidas y retiradas

Se probaron dependencias y lifetimes sobre valores definidos y usados de la
funcion; no se repitieron las 28 formas agotadas del propio `if`/store.

| variante | resultado | veredicto |
|---|---:|---|
| `v &= 0x8000`, usado como entrada de una dependencia sobre el `stickY` real | 97,65743 % / 1.584 B | perturba registros y argumentos |
| local `sign = v & 0x8000`, entrada ASM real antes de `stickY` | 95,21662 % / 1.592 B | emite `rlwinm` y rota GPR/FPR |
| temporales de recorte reales fijados a `r10` y `r11` | 98,589424 % / 1.572 B | elimina copias necesarias |
| solo el tercer temporal real fijado a `r11` | 98,82871 % / 1.576 B | elimina el `mr` del recorte |
| dependencia read/write sobre el tercer temporal de recorte | 99,18136 % / 1.580 B | mismo hueco de 8 B y peor reparto global |

La primera forma conseguia mantener un valor derivado vivo, pero no el patron
objetivo. Las variantes de pin confirman que los `r10`/`r11` del objetivo son
temporales asignados por el backend: fijarlos desde C reduce codigo en lugar de
recuperar el `mr` y el `andi.`. Todas fueron revertidas con parches acotados.

No se probaron el par independiente `r18/r19`, el orden de operandos de la rama
Logitech ni nuevas formas del bloque de `stickX`; las rondas r28-r31 y r36b-r36f
ya acotan esas familias, y no habia una causa nueva que justificase reabrirlas.

## Verificacion final

- `build_direct.py Speed/Indep/SourceLists/zPlatform`: correcto.
- `fncmp.py`: una sola funcion pendiente, igual que al entrar.
- `pctsnap.py --cmp --umbral 0`: cero mejoras y cero regresiones en las 136
  funciones.
- `scripts/audit.py`: las 135 funciones exactas pasan bytes, ramas,
  reubicaciones y literales.
- `scratchpad/codex_r40_platform/audit.py`: el JSON completo del objeto fuente
  antes/despues es identico, incluido inventario de 136 funciones, simbolos,
  secciones, datos y reubicaciones; tambien valida mecanicamente que las cinco
  copias no tienen una lectura antes de ser sobrescritas.
- `scripts/lcfix.py --check`: todas las entradas `@lc` estan al dia.
- `git diff --check`: correcto. `JoyE.cpp` tiene diff cero y conserva su formato
  LF original.
- No se tocaron configuracion, splits, cabeceras, flags ni fuentes de sonido.

## Artefactos

En `scratchpad/codex_r40_platform/`:

- `before.json`, `after.json`: objetos completos, identicos;
- `before_pct.json`, `after_pct.json`: las 136 funciones, identicas;
- `audit.py`: auditor de aislamiento y seguimiento de los cinco destinos.

La conclusion accionable es mantener la veda: sin un nuevo dato del fuente
original, no hay consumidor legitimo al que extender el lifetime del cuarto
`andi.`. La unidad queda restaurada y liberada.
