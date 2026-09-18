# R40 - `LibSN/steering`

Fecha: 2026-09-07. Territorio exclusivo: `src/LibSN/steering.c`.

## Resultado

Se cerró `Effect_Update`: **992/992 B, 100 %, 248/248 instrucciones**.
La unidad pasó de 9 a 8 funciones con código distinto y de 3964 a 2972 B
pendientes. El inventario de objdiff pasó de 27 a 28 funciones al 100 %. El
cierre previo de `VDevice_GetFreeEffect` (116 B) y todos los demás símbolos de
la unidad permanecen byte por byte iguales al baseline de esta ronda.

La diferencia inicial estaba limitada a seis operandos de registro en la rama
`EF_RAMP`. El código original necesita conservar a la vez el valor inicial de
la rampa, la duración y un acumulador de redondeo. La forma retenida expresa
esas vidas con datos reales:

```c
s32 v;
s32 rampv;
...
int d = e->e.duration;
int start = *(volatile s16 *)&e->e.u.ramp.start;
int n = e->e.u.ramp.end - start;
int m = e->elapsed;
rampv = d / 2;
if (n < 0) rampv = -rampv;
n *= m;
n += rampv;
rampv = n / d;
Effect_PolarToRect(start + rampv, e->e.u.ramp.direction, &e->force);
```

La lectura `volatile` está localizada en el campo real `start`, que se lee una
sola vez también en el objeto original. `rampv` tiene alcance de función para
dar al acumulador la misma vida de registro que el original sin reutilizar el
pseudo `v` de la rama `EF_PERIODIC`. No hay instrucciones ASM, valores
indefinidos, símbolos fantasma ni cambios de compilador o configuración. La
aritmética continúa siendo la interpolación entera redondeada existente:
`start + ((end - start) * elapsed +/- duration/2) / duration`.

## Hipótesis y ensayos acotados

Se partió del bloque equivalente denominado `b5` en las notas históricas y se
ensayaron únicamente combinaciones motivadas por la vida de sus temporales:

- `b5_volatile_start`: la carga localizada de `start` redujo el diff a cinco
  operandos; el ciclo restante era `h`/producto (`r8`/`r0` frente a
  `r0`/`r3`).
- `b5_volatile_duration`: conservó el ciclo viejo de seis operandos y se
  descartó.
- `b5_volatile_start_fused`: fusionar la expresión empeoró a once diferencias
  y se descartó.
- `b5_volatile_start_reuse_v`: cerró completamente la rama `EF_RAMP`, pero
  trasladó cinco diferencias a `EF_PERIODIC` porque prolongaba la vida de su
  pseudo compartido; se descartó.
- `b5_volatile_start_function_rampv`: separar ese pseudo como `rampv` mantuvo
  exacta la rama periódica y cerró `EF_RAMP`. Es la variante retenida.

Los snapshots de todos los ensayos se conservaron bajo
`scratchpad/codex_r40_steering/`. No se repitieron barridos históricos de
flags, pines globales ni reestructuraciones completas.

## Auditoría final

Artefactos reproducibles:

- baseline: `scratchpad/codex_r40_steering/before.json`
- resultado: `scratchpad/codex_r40_steering/after.json`
- variante exacta: `scratchpad/codex_r40_steering/b5_volatile_start_function_rampv.json`
- auditor estricto: `scratchpad/codex_r40_steering/audit.py`

Comandos finales:

```text
python scripts/build_direct.py LibSN/steering
objdiff-cli-windows-x86_64.exe diff -1 build/GOWE69/obj/LibSN/steering.o -2 build/GOWE69/src/LibSN/steering.o -c function_reloc_diffs=none -c ppc.calculatePoolRelocations=false -o scratchpad/codex_r40_steering/after.json --format json
python scripts/fncmp.py LibSN/steering
python scripts/fndiff.py LibSN/steering Effect_Update
python scripts/audit.py LibSN/steering Effect_Update
python scratchpad/codex_r40_steering/audit.py
python scripts/lcfix.py --check
git diff --check -- src/LibSN/steering.c
```

El gate propio comprueba:

- 992/992 B y 248/248 instrucciones idénticas;
- los 55 destinos relativos de rama;
- las 27 reubicaciones, con tipo, destino y addend cero idénticos;
- cero literales y los bytes resueltos contra `NFSMWRELEASE.ELF` mediante
  `scripts/audit.py`;
- que el target extraído no cambió;
- que todos los demás símbolos fuente, incluida transitivamente
  `VDevice_GetFreeEffect`, y todas las secciones/datos son idénticos al
  baseline r40;
- el incremento exacto del inventario de 27 a 28 funciones.

Salida:

```text
PASS steering r40 strict audit
  Effect_Update: 992/992 bytes, 248/248 instructions
  branches: 55/55 relative destinations
  relocations: 27/27, identical types/destinations, addends zero
  literals: 0; bytes resolved against NFSMWRELEASE.ELF by scripts/audit.py
  exact functions: 27 -> 28
  every other source symbol and every section/data diff unchanged
```

La auditoría estándar de la TU da OK para `Effect_Update` y las otras 26
funciones exactas normales. Sólo mantiene la falsa diferencia nominal ya
documentada en r39 para los estáticos locales de `VDevice_GetFreeEffect`
(`createcount$1041` frente a `createcount$427`); el auditor estricto r39 ya
validó sus cinco relocaciones, addends, destinos ELF y bytes resueltos, y el
gate r40 demuestra que esa función no cambió.

`lcfix.py --check` y `git diff --check` pasan. `steering.c` conserva CRLF puro
(1443 terminadores, cero LF desnudos). SHA-1 final de la fuente:
`812ED175269E4A688DBB3BA3A8E3D395507BD36E`.

## Pendientes conservados

Quedan ocho funciones / 2972 B con código distinto: `SimThread_Step` (924 B),
`HandleTriggers` (588 B), `SimThread_Init` (324 B), `Effect_Init` (276 B),
`CookValues` (260 B), `VDevice_RecalcGammaTable` (216 B),
`Effect_PerformEnvelope` (196 B) y `VDevice_DownloadEffect` (188 B).
`LGInit` y `VDevice_GetFreeEffect` aparecen sólo en la lista separada de alias
locales de `fncmp`, no como diferencias de código.
