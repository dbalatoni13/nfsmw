# r43 - zWorld / CarRenderInfo

## Resultado

No hay cierre nuevo: **+0 B / +0 funciones**. Se probaron tres hipótesis
causales nuevas y acotadas; las tres empeoran inequívocamente el objeto y se
retiraron. `CarRender.cpp` queda byte por byte igual al baseline de la ronda
(SHA-256 `EA13C69F...9693E5`, CRLF puro), y el objeto final se recompiló con la
ruta exacta de `zWorld`.

Baseline reproducido:

| función | objetivo | fuente | fuzzy |
|---|---:|---:|---:|
| `UpdateWheelYRenderOffset` | 876 B | 872 B | 99,38356 % |
| `RenderFlaresOnCar` | 2908 B | 2912 B | 98,61761 % |

La unidad completa conserva **577/582 funciones exactas**. Las otras tres
pendientes preexistentes (`CullParts`, `DefragmentPool`, `SetMemoryPoolSize`)
no se tocaron.

## Diagnóstico vigente

La primera divergencia que no es una simple elección de registro sigue siendo
la vida de la parte alta de la dirección de un cero flotante:

- `RenderFlaresOnCar`: el objetivo crea un solo
  `lis r16,lbl_8040AD04@ha` en la fila 123 y lo reutiliza en tres cargas muy
  posteriores. El fuente genera dos materializaciones, una local y otra
  preservada, por lo que paga 4 B más. Los cambios posteriores de
  `flashHeadlights`, `force_light_state` y `renderFlareFlags` son consecuencia.
- `UpdateWheelYRenderOffset`: el objetivo conserva dos pseudos `@ha` distintos
  para el mismo 0,0 y rematerializa `TweakKitWheelOffsetRear@ha` dentro del
  bucle. El fuente comparte el cero e iza el `Tweak`, ahorrando exactamente el
  `lis` que falta. Las dos filas de threading de ramas son secundarias.

Esto confirma el diagnóstico de r36f: no falta una expresión aritmética, un
argumento ni un inline del programa. La discrepancia nace en PRE/CSE2 y en el
hundimiento de pseudos con un solo uso. El DWARF ya casa en locales e inlines.

## Tres hipótesis nuevas medidas

1. **Puntero real común a los cinco usos de `lbl_8040AD04`.** Se declaró junto a
   `GetCarTypeInfo`, exactamente donde el objetivo crea su `@ha`, y se usó en
   las cinco expresiones existentes. Era la forma C más directa de dar una sola
   identidad y una vida dominante a la dirección. Resultado:
   **97,31087 %, 2924 B** (+12 B frente a la base). GCC conserva más estado del
   puntero, no el `high` aislado del objetivo. Revertida.
2. **Puntero real dedicado al cero de las dos comparaciones de width.** En
   `UpdateWheelYRenderOffset` se creó antes del bucle y sólo sustituyó los dos
   `0.0f` del `if` de width, dejando el radius con su literal. Pretendía separar
   dos identidades legítimas sin presión artificial. Resultado:
   **94,79452 %, 880 B**. Aparecen `lis` + `addi` para construir la dirección
   completa dentro del bucle; `TweakKitWheelOffsetRear` continúa izado.
   Revertida.
3. **Referencia C++ limitada a las tres cargas que comparten el `@ha` objetivo.**
   Se dejó intacto el cero usado por la asignación de preview y la comparación,
   para no repetir la unificación global vedada. Resultado:
   **94,801926 %, 2944 B**. Aumenta el marco de `0xf8` a `0x100` y añade el
   guardado de `f17`; es una cantidad viva nueva que el objetivo no tiene.
   Revertida.

No se probaron comparaciones con operandos invertidos, literales globalmente
unificados, presión fantasma, pines, barreras, flags ni reordenamientos ya
vedados en r26-r36f.

## Verificación final

- `build_direct.py build/GOWE69/src/Speed/Indep/SourceLists/zWorld.o`: una sola
  unidad, PASS.
- `pctsnap` antes/después: JSON idénticos (mismo SHA-256).
- `fncmp`: exactamente las mismas cinco pendientes; 577/582 exactas.
- `audit.py Speed/Indep/SourceLists/zWorld`: PASS en las 577 funciones exactas, sin
  fallos de ramas, relocaciones ni literales.
- `lcfix.py zWorld --check`: PASS.
- `git diff --check`: sin errores atribuibles a esta fuente; el fuente quedó
  restaurado byte por byte y con CRLF puro.

Artefactos: `scratchpad/codex_r43_world_before.json`,
`codex_r43_world_after.json`, `codex_r43_world_trials.json` y
`codex_r43_world_audit.py`.
