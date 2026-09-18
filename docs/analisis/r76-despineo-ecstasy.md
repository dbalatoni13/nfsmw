# r76 — Despineo de zEcstasy

Lote: 17 andamios de asm (6 pines + 11 barreras) en 6 ficheros de la unidad **zEcstasy**.
Unidad `NonMatching` en `configure.py`: el enlace usa el objeto extraído, el DOL no se mueve.

**Resultado: 17/17 andamios retirados. Ninguno vuelve en ninguna forma.**
Además salen 3 locales inventadas que sólo existían para colgar de ellas un andamio
(`half`, `e36guard`, `last_marker`).

Compilación: `build_direct.py zEcstasy` → `1 ok, 0 fallidas`.
PS2: `invalid register name` pasa de presente a **0 apariciones**; los 4 errores que
quedan son `dolphin.h`/`dolphin/mtx.h` (cabeceras de GameCube), ajenos a este lote.

## Tabla

| Función | Andamio retirado | Qué dijo el DWARF-1 | % antes | % después | nuestro/objetivo | Formas de C++ real probadas |
|---|---|---|---|---|---|---|
| `MaybeChangeViewMode()` | 1 barrera `asm volatile("":::"memory")` | 14 locales, ninguna se llama `cmp` | 100,0 % | **100,0 %** | 1236/1236 | ninguna necesaria: **la barrera estaba muerta** |
| `eViewPlatInterface::Render(ePoly*, TextureInfo*, int)` | 1 barrera seca `__asm__("")` | mismo conjunto de locales, mismo árbol de bloques y mismo reparto | 100,0 % | 99,44759 % | 1412/1412 | ninguna: las 31 filas son 2 temporales del CR, no hay local que tocar |
| `GenerateHorizonFogDisplayList()` | 1 barrera `"+r"(half)` **+ la local `half`** | **NO tiene `half`**; sólo `multiple` (r0), `i` (r31), `j` (r25) | 100,0 % | 98,99497 % | 796/796 | `multiple = i / 2` (adoptada); reordenar `grid_pointY` antes de `multiple` → 88,44 %, 800 B (descartada) |
| `eLightMaterialPlatInterface::UpdatePlatInfo()` | 1 pin `fr5` + 4 barreras (3 × `"+f"(envmap_power)`, 1 seca) | `envmap_min_scale` **sí existe y sí está en f5**; `envmap_power` en f2 | 99,96086 % | 98,85519 % | 2044/2044 | ninguna: 61 filas de rotación pura f5→f8→f7→f6, sin estructura que cambiar |
| `eProject()` | 1 barrera `"+f"(halfVP2)` | `halfVP2` = f0, `clipX` = f13; 6 locales a nivel de función, **0 bloques léxicos** | 93,970146 % | 86,1194 % | 268/268 | `eRecip` antes de `clipX`; `eRecip` después de `halfVP2/3`; orden del DWARF — **las tres dan 86,1194 % exacto** |
| `EmitterSystem::Render(eView*)` | 3 pines (`r19` `e36guard`, `r24` `sprite_hack_flags`, `fr6` `world_size`) + 3 barreras | `sprite_hack_flags` **sí, en r24**; `world_size` **sí, en f6**; **`e36guard` NO EXISTE** | 100,0 % | 82,12069 % | **688**/696 | ninguna: las 13 locales del bloque exterior del DWARF ya estaban todas declaradas; el hueco que falta es un temporal del compilador |
| `eSolid::GetPostionMarker(ePositionMarker*)` | 2 pines (`r4` tabla, `r9` `last_marker`) | `position_marker_table` **sí, en r4**; **`last_marker` NO EXISTE**, el original tiene `next_marker` (r3) a nivel de función | 100,0 % | 88,91304 % | **88**/92 | forma del DWARF con `next_marker` de función (adoptada, 88,91 %); `next_marker = prev+1` antes de comparar → 66,52 %; partir el `\|\|` en dos `if` → 71,30 %; `&&` combinado → 88,91 % (empate) |

Dos funciones pierden bytes: `EmitterSystem::Render` −8 B (se va el marco del derrame que
sostenía el fantasma r19) y `GetPostionMarker` −4 B. Las otras cinco mantienen el tamaño exacto.

## Detalle de los tres hallazgos del oráculo

### 1. `half` era `i / 2` escrito a mano
`int half = i + (int)(((unsigned int)i) >> 31);` seguido de `multiple = half >> 1;` es
literalmente la expansión que GCC 2.9 hace de `i / 2`. El DWARF no lista `half` porque en
el original no hay tal variable: el resultado intermedio del `add` no tiene nombre, y por eso
hacía falta inventarlo — para tener dónde enganchar el `"+r"`. Con `multiple = i / 2;`
quedan 2 filas: una transposición del `srawi` con el `andi.` de `(i & 1)`.

### 2. `e36guard` era un fantasma puro
`register int e36guard asm("r19");` sin inicializar, sin usar, consumido sólo por
`__asm__("" : "+r"(num_textures) : "r"(e36guard))`. No aparece en el DWARF. Su único
efecto era ocupar r19 y forzar un derrame de 8 B de marco que el original sí tiene.
Las 13 locales que el DWARF lista en el bloque exterior (`profile_node`, `num_textures`,
`last_emitter_data`, `last_emitter_data_atr`, `world_view`, `ed_drag`, `ed_gravity`,
`ed_life`, `ExtraBasis`, `ColourBasis`, `total_num_textures`) **ya estaban todas escritas**
en nuestra fuente, así que al original no le falta ninguna variable de fuente: el 19.º
rango de vida es un temporal del compilador y no hay C++ que lo fabrique.

### 3. `last_marker` no existe; el original declara `next_marker` a nivel de función
DWARF de `GetPostionMarker__6eSolidP15ePositionMarker`: `prev_marker` r11,
`position_marker_table` r4, `num_position_markers` r0, `next_marker` r3, **0 bloques léxicos**.
Nuestro `last_marker` era una local de bloque inventada, pineada a r9. Reescrita según el
DWARF, la función pasa de 66,52 % (borrado a secas) a 88,91 %. Lo único que queda es el
cruce r4↔r11: el original emite `mr r11,r4` para sacar el parámetro de su registro de
entrada y dejarle r4 a la tabla, y en C la tabla no puede *preferir* r4 porque en esta
función r4 sólo aparece como parámetro.

## Conclusiones extrapolables

1. **Una barrera puede estar muerta y nadie lo comprueba.** `MaybeChangeViewMode` cierra
   al **100 % sin la barrera**. No estaba justificada por ninguna nota: simplemente estaba
   ahí. Es la primera cosa que hay que medir en cualquier lote de despineo, y es gratis.
2. **El pin casi siempre era *fiel*, no inventado.** 4 de los 6 pines daban el registro que
   el DWARF confirma (`fr5`→f5, `r24`, `fr6`→f6, `r4`). El pin no fabricaba un reparto
   falso: forzaba el verdadero. Los 2 que sí eran falsos (`r19`, `r9`) son exactamente los
   que sostenían una local inexistente. **Regla: pin sobre local que el DWARF no lista =
   fake match; pin sobre local que sí lista = reparto real forzado a mano.**
3. **3 de 17 andamios sostenían una local inventada, y en los tres el borrado recupera
   casi todo** (98,99 %, 88,91 %) *después* de reescribir la forma. Borrar a secas da
   66,52 %; borrar + forma del DWARF da 88,91 %. La diferencia entre un informe honesto y
   uno derrotista es hacer el segundo paso.
4. **Buscar `x + ((unsigned)x >> 31)` en el árbol.** Ese patrón es la expansión de una
   división entera por potencia de dos; si aparece escrito a mano en una fuente, hay una
   local inventada y probablemente un andamio colgando de ella.
5. **El orden de sentencias no mueve un empate de `local_alloc`.** Tres variantes distintas
   de `eProject` dan 86,1194 % *exacto*. Cuando el diff es una rotación pura de registros FP
   sin cambio de tamaño, reordenar la fuente es tiempo perdido: hay que atacar la presión
   de registros, no el orden.
6. **Partir un `||` en dos `if` no es una palanca universal.** En `eSolid` empeora de
   88,91 % a 71,30 %. La palanca del catálogo sirve cuando duplica referencias de bucle; en
   una condición de salida temprana hace lo contrario.
7. **Una local "muerta" no siempre es un andamio.** En `MaybeChangeViewMode`,
   `CameraMover *cmp = viewp1->GetCameraMover();` no se usa nunca, pero **sin ella la función
   pierde 8 B y cae a 97,34 %**: es una carga real del original. Probada también la sentencia
   suelta `viewp1->GetCameraMover();` (sin local): misma pérdida, GCC la elimina. El nombre es
   nuestro; el código, del original. No confundir "local sin usar" con "andamio".
