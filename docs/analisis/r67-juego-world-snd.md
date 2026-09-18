# r67 — lote `juego-world-snd`: 25 andamios, 0 retirados, 25 con diagnóstico r67

**Encargo**: dejar fuente legítima. 12 ficheros en exclusiva, 7 unidades, todas
`NonMatching` (ninguna entra en el DOL). Test: digest de las secciones ALLOC del `.o`
compilado a un directorio privado, idéntico o se revierte.

## 1. Cifras

| | |
|---|---|
| andamios del lote | **25** (19 pines + 6 barreras `__asm__`) |
| retirados | **0** |
| andamios al final | 25 |
| variantes medidas (compilación + digest + diff contra el `.o` original extraído) | **14** |
| volcados `.lreg`/`.greg` analizados (orden de `allocno_compare` con la prioridad calculada) | 7 |
| revertidos | los 14 (ninguno salió idéntico) |
| regresiones | **0**: las 7 unidades reproducen su digest base exacto (§6) |
| con diagnóstico r67 escrito junto al andamio | **25** (en 18 bloques de nota) |
| propuestas | 1: `CarRenderInfo::CreateCarLightFlares` sin sus 3 pines (§4.1) |

## 2. Método (`scratchpad/jwsnd67/`)

* `cc.py`, `tool.py`, `d.py`, `sym.py`, `prueba.py`: los de `resto66` con la salida
  redirigida (y `d.py` con un JSON por proceso, para poder medir dos unidades a la vez).
  Bases compiladas **dos veces**, las dos iguales (`base_all.txt`, `base_all2.txt`).
* `rtl.py <spec> <fn>` — **nuevo**. Aplica los reemplazos de un spec, saca `.lreg`/`.greg`
  de la UNIDAD con una copia de `rtldump.py` que escribe en mi scratchpad (el
  `scratchpad/rtl` es compartido), **revierte siempre** y guarda la sección de la función.
* `pri.py <tag>` — **nuevo**. Junta el orden de `;; N regs to allocate` del `.greg` con
  `Register N used R times across L insns` del `.lreg` y calcula
  `pri = int(floor_log2(R)*R/L*10000)` (global.c:636) y el registro final. Convierte un
  «swap de 2 registros» en «le faltan K insns de vida o 1 referencia».

Cuando la variante se queda a filas de reparto, el `.greg` dice qué número habría que mover.
Con eso se descartan formas de fuente antes de compilarlas.

## 3. Resultado por función

| función (unidad) | andamios | intento r67 | digest | filas | diagnóstico |
|---|---|---|---|---|---|
| `TrackStreamer::HandleLoading` (zTrack) | 2 pines | `for (int n …)` como el DWARF, sin pines | `6905db5532fee1ec` | 12 | rotación num/n/constante; ver §4.3 |
| `TrackStreamer::GetPredictedZone` (zTrack) | pin + barrera | sin los dos | `c4a6f4d4e77761d3` | 10 | pri 1130 contra 1119: le falta 1 ref (§4.4) |
| `TrackPathManager::FindZone` (zTrack) | pin + barrera | `found_zone` declarado arriba (DWARF), sin los dos | `c7e024b7dd1a940a` | 5 | pri 735 contra 789 del `@ha`: 1 ref o −10 de vida |
| `RegionQuery::CalculateRegionInfo` (zTrack) | 2 pines + barrera | ninguno | — | — | el original no declara locales ahí; retícula r65 vigente |
| `CarRenderInfo::CreateCarLightFlares` (zWorld) | 3 pines | natural `GetModel()` | `13ba7d184f5c468d` | 8 | model pri 724, necesita ≤ 714 (vida ≥ 140) |
| ″ | ″ | + `type_name` muerto, sin asm `.rodata` | `647d3863969841be` | **0** | **función al 100 % sin pines**; sólo cambia `.rodata` (§4.1) |
| `GManager::AllocateInstanceMap` (zGameplay) | 1 pin | `minHashEntries`/`tableSize` declaradas arriba (DWARF) | `78e2b8eb1b8ecd87` | 24 | pasada 0 de `find_reg` (§4.3) |
| `GManager::GetStrippedNameKey` (zGameplay) | 1 pin | ninguno | — | — | diagnóstico r65 vigente (preferencia sólo de `XEXP(src,0)`) |
| `PackedDecimal::PackedDecimal` (zGameplay) | barrera | locales en el orden que lista el DWARF | `20713a6af65f81d7` | 21 | **peor**: el orden actual es el bueno (§4.2) |
| `GRaceParameters::GenerateIndex` (zGameplay) | barrera | ninguno | — | — | `rank`/`f` no están en el DWARF; deuda de reparto (r36f) |
| `EAXDispatch::PursuitEscalation` (zSpeech) | barrera | sin barrera | `ea7cbfc609d2b519` | 5 | sched; ver mapa de líneas |
| `EAXDispatch::BreakAway` (zSpeech) | barrera | ifs sucesivos y `result` reasignado (lmap), sin barrera | `06054b8f502f6448` | 15 | igual que la forma anidada |
| `AIPursuit::AssignClosestOffsets` (zAI) | barrera | `while (copsToAssignOffsets-- > 1)` | `7e1bee047a9261d2` | 11 | compara el valor viejo contra 1 |
| `MemoryPoolManager::NewBlockAux` (zEagl4Anim) | 3 pines | forma r66 remedida | `9dd3e31e015ab4be` | 8 | mismo `.o` que en la r66; sólo `idx`/`r` en el DWARF |
| `FnRawStateChan::FindTime` (zEagl4Anim) | 1 pin | `k += c->GetKeySize() * i` | `c8da47e39d3b9be7` | 4 | el objetivo carga `GetKeySize` en r9, no el producto |
| `WRoadNav::HolePunchAvoidables` (zWorld2) | pin (+ barrera) | palanca (a): `offset_change` antes de la copia de `cut_to_position` | `4357326980ac2933` | 15 | = sin pin; el nacimiento no llega a `qty_compare_1` |
| `WRoadNav::InitAtSegment` (zWorld2) | pin (+ 2 barreras) | ninguno | — | — | cantidad anónima: no hay declaración que mover |
| `WRoadNav::CookieTrailCurvature` (zWorld2) | 2 pines (+ barrera) | natural, remedido tras UVectorMath | `3f798bda657c5db0` | 3 | en GC `VU0_floatmax` es C, no asm |
| `WTriggerManager::CheckCollideSRB` (zWorld2) | barrera | ninguno | — | — | le falta 1 ref a `trig` (r29); misma clase que §4.4 |

Bases: zTrack `904e61b404a8ea8b`, zWorld `2d49d8ada788b07b`, zGameplay `8608d060553a271a`,
zSpeech `39d1580dcbc5228a`, zAI `f86c2656c7e38244`, zEagl4Anim `4016eb04ac44d293`,
zWorld2 `8e7ee9e293166fa6`. «Filas» = filas distintas de la función contra el `.o`
original extraído (`objdiff`, `calculatePoolRelocations=false`).

## 4. Hallazgos

### 4.1 `CreateCarLightFlares`: la función cierra sin pines con contenido muerto, y la base está mal en `.rodata`

El DWARF del original declara `const char *type_name` **sin location**, y justo debajo de la
función hay un `asm(".section .rodata")` de fichero (r64, «pool: obj 184») con quince
cadenas: `"Left Headlight"` … `"Right Reverse Light"`, `"<none>"`. Son la misma cosa: el
original asignaba el nombre en cada `case` y la asignación se murió.

1. **Forma natural** (`eModel *model = this->mCarPartModels[slotIndex][0][this->mMinLodLevel].GetModel();`,
   sin pines): 8 filas, rotación r19/r20/r21 entre `model` y dos constantes del `switch`
   izadas por `loop.c`. En el `.greg`: `model` refs 5, vida 138, **pri 724**; `0xa2a2fc7c`
   pri 722; `0x7adf7ef8` pri 718; el siguiente, 714. Con vida ≥ 140 (pri 714), el orden es
   148, 146, 140, 138, model: exactamente r23, r22, r21, r20, r19 del objetivo. Faltan
   **dos insns** de vida y ningún byte.
2. **Con el contenido muerto** (un `case` por cadena, en el orden de las cadenas, y
   `default: type_name = "<none>";`) y **sin** el asm `.rodata`:
   `CreateCarLightFlares` **100 %, 0 filas, sin ningún pin**.
3. **El test la rechaza**: zWorld pasa a `647d3863969841be`. Sólo cambian `.rodata`
   (17.320 → 17.344 B) y las reubicaciones que apuntan a ella (`.rela.text`,
   `.rela.rodata`, `.rela.data`, `.rela.ctors`); `.text` y `.data` son idénticos.
4. **Pero el original tiene las cadenas alineadas a 4**, como las emite GCC, y el asm de
   la r64 las pone pegadas con `.asciz`. Medido contra el `.o` extraído del original, desde
   `"Left Headlight"`: **la variante casa 276 B seguidos hacia delante y 283 hacia atrás;
   la base, 15 y 3**. Los 24 B son el relleno entre cadenas (250 B de texto → 272 alineados,
   más 2 de arranque). O sea: el `.rodata` de la base está mal justo ahí y la variante lo
   arregla.

Se queda revertida por la regla. **Propuesta en §5.**

### 4.2 Regla nueva: el volcado lista las locales de una COPIA INLINE en orden inverso

El DWARF de `GenerateIndex` da, en las tres copias inline de `FloatingPoint`, `bool neg // r0;
int man // r31; int exp // r27`. Nuestro `PackedDecimal` las declara al revés (`exponent`,
`mantissa`, `negative`) **y sale con esos mismos registros**. Declaradas en el orden del
volcado y sin barrera: 21 filas, `man`/`exp` cruzados. Segundo caso independiente:
`bNode::AddBefore` declara `new_prev`, `new_next` y el volcado dice `new_next; new_prev // r9`.

**Uso**: para la palanca (b), el orden del DWARF sirve directo para las locales de la
función, e **invertido** para las de una expansión inline.

### 4.3 Regla nueva: un pin pone el registro en `regs_ever_live` ANTES de `global_alloc`, y la pasada 0 de `find_reg` lo hereda

`find_reg` (global.c) prueba dos pasadas. En la 0 sólo acepta registros **ya usados**
(`regs_used_so_far` = `regs_ever_live` ∪ volátiles, global.c:390-402) y no preferidos por
otro. Un pin es un registro duro vivo desde el principio, así que cambia qué preservado
está «ya usado» para TODOS los allocnos que se reparten después. Por prioridad no se puede
imitar, y explica los dos casos donde el `.greg` descarta cualquier forma:

* **`AllocateInstanceMap`**: `vault` (pri 34.285) se reparte primero; sin preservados vivos
  cae a la pasada 1 y coge r31; `this` (10.322) coge r30. El objetivo (`vault`/`tableSize`
  r30, `this` r31) necesita r30 ya vivo al empezar `global_alloc`, y eso sólo lo da el pin.
* **`HandleLoading`**: constante izada pri 2.545, `n` 2.327, `num_sections_unactivated`
  1.166. La constante coge r30 en la pasada 0 porque local-alloc dejó ahí el `@ha` de
  `PostLoadFixupDisabled`. El objetivo necesita `num` antes que la constante: con vida 120
  haría falta refs ≥ 11 (tiene 7). La estructura del DWARF (`for (int n …)` con `section`
  anidado) ya está medida y deja sólo esa rotación (12 filas, mismo tamaño).

### 4.4 La clase «una referencia de más»

Tres andamios del lote son la misma cosa: un asm no volátil que suma **una** a `n_refs` y no
emite nada.

| función | local | pri | rival | pri | con +1 ref |
|---|---|---|---|---|---|
| `GetPredictedZone` | `predict_position_used` (refs 10, vida 268) | 1.119 | `predict_zone_number` | 1.130 | 1.231 |
| `FindZone` | `found_zone` (refs 5, vida 136) | 735 | `@ha` del 0.0f | 789 | 882 |
| `CheckCollideSRB` (r29) | `trig` | 5.482 | `srBody` | 5.619 | 5.800 |

En los tres el objetivo pone el `li`/la definición en el mismo sitio que nosotros, así que
acortar la vida no es una opción, y **no hay forma de C que añada una referencia sin
código**. Sirve para clasificar sin compilar: si el hueco del `.greg` se cierra con +1 ref y
la vida no se puede tocar, es de esta clase.

### 4.5 Trampa: `BNEW` usa `__LINE__`; un comentario en `GManager.cpp` mueve el `.text`

`GManager.cpp` tiene 10 `BNEW` (`new (__FILE__, __LINE__)`), dos de ellos (2539, 2601)
**después** de los dos pines del lote. Una línea de comentario de más en esas funciones
cambia el `li r5` de esas llamadas. Las notas r67 de `GManager.cpp` **reescriben** líneas de
comentario existentes (12 por 12) y el digest de zGameplay sale idéntico. Antes de comentar
un fichero hay que buscar `__LINE__`/`BNEW`.

### 4.6 Incidente propio, detectado y corregido antes del sello

La primera nota de `TrackPath.cpp` se anclaba en la línea de la barrera y **no la volvía a
escribir**: el `asm("" : : "r"(found_zone));` desapareció. Lo cazó
`git diff -U0 | grep '^-'` (una línea `-` que no era comentario). Restaurada antes del
sello. El control final sobre los 12 ficheros da **0 líneas de código añadidas y 0
quitadas** (sólo `GManager.cpp` sustituye 12 líneas de comentario por 12). **Trampa**: al
anclar una nota en el andamio, el `new_string` tiene que terminar con el andamio, y hay que
revisar las líneas `-` del diff, no sólo las `+`.

## 5. Propuestas

**Una**, fuera del test (lo cambia a propósito). Parche listo:
`scratchpad/jwsnd67/r67_carrender_type_name.patch` (129 líneas, LF; el fichero es CRLF),
generado sobre `CarRender.cpp` tal como queda tras esta ronda. Contenido exacto:

1. Quita los tres pines y las dos líneas intermedias de `CreateCarLightFlares`:
   `register unsigned int slot_base asm("r11") = …; slot_base += …;`,
   `register unsigned int raw_model asm("r0") = …;` y
   `register eModel *model asm("r19") = …;` → `eModel *model = this->mCarPartModels[slotIndex][0][this->mMinLodLevel].GetModel();`
2. En el `switch`, un `case` por cadena, con `type_name = "<cadena>";` delante de
   `flare_type` (en los dos `REVERSE`, delante del `if (…) continue;`), en este orden:
   LEFT/RIGHT/CENTRE_HEADLIGHT, LEFT/RIGHT/CENTRE_BRAKELIGHT, COPLIGHTRED, BLUE, WHITE,
   BRIGHTRED, BRIGHTBLUE, ORANGE, LEFT_REVERSE, RIGHT_REVERSE, `default: type_name = "<none>";`.
3. Borra el `asm(".section .rodata\n" /* r64 pool: obj 184 */ … ".previous\n");` de las 15
   cadenas.

Efecto medido: función 100 % con 0 filas y 3 pines + 1 asm de datos menos; zWorld
`2d49d8ada788b07b` → `647d3863969841be`, sólo `.rodata` +24 B y sus `.rela.*`. zWorld es
`NonMatching`, así que el DOL no se mueve. **Antes de aceptarla** hay que mirar el resto del
`.rodata` de zWorld contra el original (`dolwhere`/`rodatagaps`): los 24 B podrían estar
compensados más abajo por otro bloque de la r64.

**Auditoría sugerida** (no es de este lote): los bloques r64 de `.rodata` con `.asciz` son
muchos (p. ej. `CarLoader.cpp` 58, `CarInfo.cpp` 11, `SkyRender.cpp` 10, y cientos en las
SourceLists). Si alguno sustituye cadenas que GCC emitiría alineadas a 4, tiene el mismo
error que §4.1. Comprobación barata: buscar en el `.o` original si hay relleno de ceros
entre las cadenas del bloque.

Ninguna propuesta de `configure.py` ni de `config/GOWE69/*`.

## 6. Sellos: las 7 unidades, antes y después

Digest de las secciones ALLOC (`.text`, `.rodata`, `.data`, `.bss`, `.sdata*`, `.ctors` y
sus `.rela.*`), sin `.line`/`.debug*`/`.comment`/`.stab*`, compilado a `.o` privado. Base
dos veces al empezar; final una vez, en serie, con todas las notas puestas.

| unidad | ficheros del lote | ALLOC base | ALLOC final |
|---|---|---|---|
| zTrack | `TrackStreamer.cpp`, `TrackPath.cpp`, `WeatherMan.cpp` | `904e61b404a8ea8b` | `904e61b404a8ea8b` |
| zWorld2 | `WRoadNetwork.cpp`, `WTrigger.cpp` | `8e7ee9e293166fa6` | `8e7ee9e293166fa6` |
| zWorld | `CarRender.cpp` | `2d49d8ada788b07b` | `2d49d8ada788b07b` |
| zGameplay | `GManager.cpp`, `GRaceDatabase.cpp` | `8608d060553a271a` | `8608d060553a271a` |
| zSpeech | `EAXDispatch.cpp` | `39d1580dcbc5228a` | `39d1580dcbc5228a` |
| zAI | `AIPursuit.cpp` | `f86c2656c7e38244` | `f86c2656c7e38244` |
| zEagl4Anim | `MemoryPoolManager.cpp`, `RawStateChan.cpp` | `4016eb04ac44d293` | `4016eb04ac44d293` |

Cabeceras compartidas, `#if defined(__ANDROID__)`, `configure.py` y `config/GOWE69/*`: sin
tocar. Volcados RTL (79 MB), objetos y copias de seguridad borrados del scratchpad.

## 7. Veredicto

**PARCIAL: 0 retirados, 25 de 25 con diagnóstico r67**, casi todos con la cifra que falta
sacada del `.greg` en vez de otra forma de fuente. Un andamio triple (`CreateCarLightFlares`)
**cierra sin pines** con contenido muerto que el DWARF describe, pero no pasa el test
porque la base tiene un error de alineación en `.rodata` que la variante corrige; queda
como propuesta con el parche y la medida contra el original.

Lo que deja la ronda como método:

1. **`pri.py` antes de escribir formas**: si al rival le faltan K insns de vida o una ref,
   se sabe sin compilar si hay sentencia que lo pueda dar.
2. **Dos mecanismos que la prioridad no imita**: la pasada 0 de `find_reg` con el registro
   que el pin deja vivo (§4.3) y la referencia de más sin código (§4.4). Los andamios de esas
   dos clases son irreducibles por forma de C, y ya se reconocen en el volcado.
3. **Orden inverso** de las locales de una copia inline en el DWARF (§4.2).
4. **Los asm de datos de la r64 pueden estar mal alineados** y sostener andamios de código
   (§4.1).
