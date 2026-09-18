# r39 — sonido: cierre de `CARSFX_PreColWoosh::MsgBarrier`

Fecha: 2026-09-07. Unidad exclusiva:
`Speed/Indep/SourceLists/zEAXSound2`.

## Resultado

Se cierra una funcion real:

| funcion | antes | despues | ganancia |
|---|---:|---:|---:|
| `MsgBarrier__18CARSFX_PreColWooshRC16MAudioReflection` | 140 B / 94,28571 % | **140 B / 100 %** | **+140 B / +1 funcion** |

El `fndiff` final tiene las 35 instrucciones identicas, incluidos los nueve
sitios de reubicacion y los cuatro usos de literales. La unidad pasa de 6/930
funciones pendientes y 3.564 B no exactos a **5/930 y 3.424 B**. Las otras
929 funciones no sufren regresiones. No se toco `zEAXSound`.

## Causa y cambio retenido

La unica diferencia inicial estaba en la preparacion de la llamada a
`cInterpLine::Initialize`: el objetivo materializa `li r4,1` antes de guardar
`mDurationActive`, mientras que la fuente guardaba primero y materializaba el
argumento despues.

El cierre conserva exactamente los valores y accesos de la fuente:

- una dependencia read/write sobre el `bool` real ya escrito impide que el
  compilador retrase el store de `bGoingToCollide`;
- un alias real a `WooshFadeOut`, con dependencia read/write, fija la
  preparacion de `r3` sin inventar direcciones;
- la duracion de interpolacion real, de valor `1`, queda ligada a `r4` y viva
  antes del store de `mDurationActive`.

No hay cuerpo ASM, dato fantasma ni lectura indefinida. Las tres restricciones
vacias solo expresan dependencias sobre objetos y valores que participan en la
llamada. El resultado conserva las cuatro salidas condicionales, el prologo y
epilogo ABI y la llamada original.

Las dependencias se probaron por separado para confirmar que no son relleno:

| variante | resultado |
|---|---:|
| pin `r4` + dependencia del `bool`, sin alias `fadeOut` | 92,57143 % |
| alias `fadeOut` + dependencia del `bool`, sin dependencia viva de `r4` | 86,685715 % |
| las tres dependencias reales | **100 %** |

## Ensayos negativos retirados

Se estudio tambien `CARSFX_Turbo::ProcessUpdate`, cuyo baseline era
672/672 B y 97,5 %. Su primera cascada procede del `mr. r7,r3` que el objetivo
mantiene cerca de la ultima llamada a `GetDMixOutput`; la fuente asigna el
mismo valor mas tarde y el backend cambia los registros posteriores.

Todas estas variantes quedaron retiradas y el archivo volvio byte a byte a su
estado inicial:

| variante | medida |
|---|---:|
| dependencia `+r` directa sobre `Az` | 97,2619 % / 672 B |
| pin de `Az` a `r7` a traves de llamadas | 93,27381 % / 652 B |
| alias separado `azForControl` sin barrera | neutra, 97,5 % / 672 B |
| alias separado a `r7` con barrera | 96,69643 % / 676 B |
| resultado escalado fijado a `r3` | 92,8869 % / 652 B |

Los pines a registros volatiles a traves de llamadas eran ademas una
representacion inadecuada del lifetime real, por lo que no se conservaron. No
se reabrieron `cStichWrapper::Play` ni `GinsuSynthData::BindToData`: r37 ya
documenta esas vias agotadas y no aparecio evidencia causal nueva.

## Auditoria

- Compilacion directa de `zEAXSound2`; no se ejecuto `ninja` global.
- `fncmp.py`: solo desaparece `MsgBarrier`; quedan exactamente las cinco
  pendientes conocidas por 3.424 B.
- `pctsnap.py --cmp --umbral 0`: una mejora de +5,714 pp y cero regresiones.
- `fndiff.py`: 35/35 instrucciones iguales, 140/140 B.
- `scripts/audit.py`: salida 0; pasan 925 funciones exactas. Para el cierre
  informa 140 B, cuatro ramas, nueve reubicaciones y cuatro literales.
- Auditor de aislamiento propio: 2.808 simbolos y 23 secciones con inventario
  y tamanos runtime iguales; solo cambia el registro no-section de
  `MsgBarrier` y los otros 2.793 registros no-section son estructuralmente
  identicos normalizando metadatos de comparacion y linea. Las diferencias de
  `.debug`, `.line` y `.rela.debug` corresponden a las nuevas locales.
- No aparecen datos ni literales nuevos; las secciones runtime mantienen
  inventario, tamanos y alineacion.
- `scripts/lcfix.py --check`: todas las entradas `@lc` estan al dia.
- `git diff --check`: limpio para las dos fuentes inspeccionadas. Ambos
  archivos conservan CRLF completo; `CARSFX_Turbo.cpp` tiene diff cero.

## Artefactos

Directorio `scratchpad/codex_r39_sound/`:

- `before.json`, `after.json`: objetos completos antes y despues;
- `before_pct.json`, `after_pct.json`, `pct_compare.txt`: inventario de las
  930 funciones y comparacion sin umbral;
- `before_fncmp.txt`, `after_fncmp.txt`: pendientes de unidad;
- `official_audit.txt`: auditoria estandar de todas las exactas;
- `audit.py`: comprobacion de aislamiento por simbolo y seccion;
- `lcfix_check.txt`: verificacion de referencias `@lc`.

La unica fuente de sonido modificada y liberada es
`src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.cpp`.
