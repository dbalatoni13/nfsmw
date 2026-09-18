# Iter4 2026-09-08 — auditoría de negativas `steering`

## Resultado

Las siete sombras se confirman negativas y quedan sólo como evidencia. Ninguna
modificó la fuente ni el objeto de producción:

- `src/LibSN/steering.c` y `baseline.c`:
  `2BCB96407F59B0EF86058018DE6F4075243F1F4AD5F270B88BD4E68E08DE331C`.
- `build/GOWE69/src/LibSN/steering.o` y `production_before.o`:
  `0AF542803AD473598986073ADF69D2148CFD468AAF232821F89762AEFCBABD3E`.

No se incluye huella de `configure.py`, `build.ninja` ni otra configuración: root
estaba integrando promociones autorizadas en paralelo y esos archivos no forman
parte del baseline de esta auditoría.

`audit_negative.py` vuelve a cotejar los JSON contra el objeto original y compara
cada sombra directamente con `baseline.o`, incluyendo las relocaciones de función.
Resultado: **PASS**, 36/36 funciones presentes en todas las variantes; las 35
vecinas al experimento son exactas respecto al baseline privado en cada caso.

## `Effect_Init` — 276 B

| Variante | Tamaño fuente | Coincidencia | Filas distintas |
|---|---:|---:|---:|
| `effect_seed` | 276 B | 91,14493 % | 32 |
| `effect_clear` | 276 B | 95,0 % | 23 |
| `effect_slot` | 276 B | 94,710144 % | 25 |

`effect_seed` reconstruye explícitamente la conversión entero→float con una unión
de 8 B, el sesgo doble y un productor `lis` del exponente. Esto conserva la
semántica y la ranura compartida, pero mantiene seis GPR vivos en el bucle y rota
la asignación de `sine`, `ramp`, índice, fase y offset. También desplaza la carga de
la constante final y deja distinta la arista de retorno del bucle.

`effect_clear` añade un cero real producido fuera del bucle de borrado. Cierra la
colocación de `li 0` y toda la cabecera/los ocho stores del clear, pero el salto de
retorno sigue apuntando a una disposición distinta y la segunda mitad conserva la
rotación de seis GPR. Es la mejor forma por porcentaje, pero no un cierre.

`effect_slot` expresa también los temporales FP y sus multiplicaciones para acercar
el uso de la ranura. El coste de sus vidas simultáneas cambia los registros FP y no
resuelve la rotación GPR; queda por debajo de `effect_clear`.

Hecho causal retenido: el productor explícito del exponente reproduce la ranura de
conversión de 8 B, y el cero fuera del bucle reproduce el encabezado del clear. La
grieta restante no es la conversión ni el cero: es la asignación conjunta de los
seis GPR y la planificación de la arista de vuelta.

## `CookValues` — 260 B objetivo

| Variante | Tamaño fuente | Coincidencia | Filas distintas |
|---|---:|---:|---:|
| `cook_bytes` | 260 B | 98,46154 % | 15 |
| `cook_extsb` | 264 B | 93,23077 % | 22 |
| `cook_lo` | 260 B | 99,07692 % | 9 |
| `cook_order` | 260 B | 99,07692 % | 9 |

Separar el byte crudo (`_bits`) de su interpretación con signo y hacer explícito
el `extsb` permite controlar las conversiones que el macro `AUTOCAL` fusionaba.
`cook_extsb` fuerza además la extensión del mínimo y crece una instrucción, por lo
que se descarta. `cook_lo` conserva 260 B y deja sólo nueve operandos: un único
intercambio coherente `r4`/`r6` a lo largo de carga, comparaciones, `mr`, `subf` y
`add`.

`cook_order` cambia únicamente el orden declarativo de `_v`/`_mn`. Su contenedor
ELF no es byte-idéntico por metadatos de línea, pero la comparación directa confirma
36/36 cuerpos y relocaciones de función al 100 % frente a `cook_lo`; no aporta una
palanca nueva.

Hecho causal retenido: los bytes separados y el `extsb` localizado cierran la forma
de las instrucciones y mantienen la lectura como byte. La grieta restante es sólo
la asignación `r4`/`r6`; los cambios de orden ya medidos no la alteran.

## Evidencia reproducible

- Auditor: `scratchpad/codex_20260908_iter4_steering/audit_negative.py`.
- Resultado: `audit_negative_result.json` (`status: PASS`).
- Comparaciones directas completas: `audit_pair_*.json`.
- Entradas: `baseline.c`, `baseline.o`, `production_before.o`, siete fuentes
  privadas y sus siete objetos/JSON.

No se recompiló producción ni se ejecutaron nuevas variantes durante esta revisión.
