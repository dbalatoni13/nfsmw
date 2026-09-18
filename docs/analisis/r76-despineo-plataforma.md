# r76 — Despineo de zPlatform y zTrack

Lote: los 3 andamios de asm (2 PIN + 1 BARRERA) de las dos unidades. **Retirados los 3.**
Ninguno de los tres ficheros contiene ya `asm(...)` de ninguna clase.

Medido con `build_direct.py` (1 ok, 0 fallidas en las dos unidades) y `fndiff.py` contra el DOL.

## Tabla

| Función | Andamio retirado | Qué dijo el DWARF-1 del original | % antes | % después | nuestro/objetivo | Formas de C++ real probadas y descartadas |
|---|---|---|---|---|---|---|
| `LGWheels::PlayFrontalCollisionForce` | PIN `register unsigned int magnitude asm("r27") = inputMagnitude` | `this` r29, `channel` r31, **`magnitude` (unsigned char) r27**, `ret` r30. **Cero locales más y cero bloques léxicos**: la copia pineada era una local inventada, y `playing` y `periodic` tampoco existen | 100,00 % | **99,4545 %** (12 filas) | 440 / 440 B | ceros literales en los argumentos en vez de reciclar `ret`: **idéntico** (adoptada, es más plausible); sin `playing` 97,68 %; `periodic` al principio 97,14 %; sin la variable `periodic` 95,95 % **y 444 B**; guarda temprana en vez de `else` **0 %** |
| `TrackPathManager::FindZone` | PIN `register TrackPathZone *found_zone asm("r26") = nullptr` | `found_zone` (TrackPathZone\*) **sí existe, y en r26**: la variable es real y el pin acertaba el registro. `zone_info` r30, `cache_valid` r10, árbol de bloques ya correcto | 100,00 % | **99,8494 %** (5 filas) | 664 / 664 B | `return found_zone` extra en cada rama: **100,0 % exacto**, descartada por fidelidad (ver abajo); sin `break`, `return zone;` 90,36 % **y 668 B**; condición del bucle mirando `found_zone` 88,98 % **y 680 B**; `&&` del bucle de caché invertido 96,66 %; reordenar las sentencias de cada rama: misma cifra (adoptada por fidelidad) |
| `TextureInfoPlatInterface::LockPalette` | BARRERA `asm("" : "+r"(Pal32))` | `Pal32` existe pero **optimizado fuera**, dentro de un bloque léxico (+0x34..+0x9c) con el de `j` (r7) anidado. `pTempPal` void\* r3, `gcPal` r31 | 100,00 % | **97,3864 %** (3 filas) | **172 / 176 B** | seis formas, las **seis** con las mismas 3 filas y los mismos 172 B: `Pal32` dentro del if (la del DWARF, adoptada), fuera del if, `Pal32 = (unsigned int *)(pTempPal = new ...)`, guarda envolvente en vez del return anticipado, el `new` asignado a `Pal32` con `pTempPal` de copia, y el `if` sobre `Pal32` |

Sin optimismo: **ninguna de las tres cierra al 100 % con código fiel**. Una pierde 4 B de tamaño.

## Lo que queda en cada una

- **PlayFrontalCollisionForce**: permutación r27↔r28 entre `magnitude` y el pseudo de `&this->periodic`.
  Prioridad de global-alloc 2673 contra 1250 (6 refs / 96 insns). La fórmula de GCC 2.9 es
  `refs * 10000 / live_length` y está confirmada al entero con las dos cifras: para invertir el orden
  harían falta **13 refs** o **la mitad de vida**, y no hay forma de fuente que lo dé.
- **FindZone**: permutación r25↔r26 entre `found_zone` (735 = 5 refs / 136 insns) y el pseudo `@ha`
  del literal `0.0f` (789 = 3 refs / 38). Bastaría con 6 refs (882) o con bajar la vida a 126.
- **LockPalette**: falta **una instrucción**. El objetivo copia el resultado del `new` a r6 antes del
  `cmpwi r3, 0` y recorre la paleta con r6; sin la barrera, cse propaga la copia, sale `mr. r3,r3`
  y el bucle usa el propio r3. No he encontrado la forma de fuente que impida esa propagación.

## Hallazgos

### 1. El pin de LGWheels escondía un parámetro mal nombrado (confirmado)

`register unsigned int magnitude asm("r27") = inputMagnitude` era una **copia de un parámetro**:
el DWARF da un único `magnitude`, de tipo `unsigned char`, ya en r27, y **ni una sola local más ni un
solo bloque léxico**. La cabecera `LGWheels.h:46` ya declaraba `unsigned char magnitude`; solo la
definición usaba `inputMagnitude`. Retirado el pin y renombrado el parámetro, la firma coincide con
el DWARF, con la cabecera y con el original.

**Efecto colateral medido:** `zPlatform.cpp` pasa de fallar en PS2 (`invalid register name`) a
**compilar entero, 0 errores**. `zTrack.cpp` sigue fallando en PS2, pero por `dolphin/mtx.h`, no por asm
(`de_asm=0` en las dos unidades). En X360 quedan errores de asm en las dos, todos de clase DATOS/ALIAS
en los propios SourceLists (`asm(".previous")`, `extern ... asm("lbl_...")`), ninguno de mi lote.

### 2. Reciclar `ret` como argumento-cero no aportaba nada

Las llamadas a `UpdateForce`/`DownloadForce` pasaban `ret` (que vale 0) en seis argumentos.
Sustituirlo por literales `0` da un diff **byte a byte idéntico**: mismo 99,4545 %, mismas 12 filas.
Es código más plausible por el mismo precio, así que se queda.

### 3. El `.line` de nuestros `.o` es un oráculo nuevo, y decidió FindZone

Los cflags del proyecto ya llevan `-gdwarf+`, así que nuestros objetos traen sección `.line` con las
notas de línea, en el mismo formato DWARF-1 que `symbols/debug_lines.txt` trae del ELF original.
Comparar las dos **da el orden de las sentencias del original**, que el DWARF de locales no da.
Lector en `scratchpad/r76_plat/rdline.py` (30 líneas, lee `.line` directamente del ELF; `objcopy` no
extrae esa sección porque no es ALLOC).

Lo que destapó en `FindZone`:

- **Una forma al 100 % que NO es la original.** Un `return found_zone;` extra al final de cada rama
  sube `found_zone` a 6 refs, invierte la prioridad contra el `@ha` y da **100,0 % exacto, 664/664 B,
  cero filas**, sin una sola línea de asm. Pero el original **no pone ninguna nota de línea en el
  epílogo** (+0x27c) y esa forma sí la pone; su `mr r3,r26` final lleva una sola línea (la 367)
  cuando en esa misma función el cross-jumping sí acumula tres notas en otras direcciones (+0x1f8).
  El original tiene **un solo `return`**. Descartada: es el 100 % de una función que no es la de EA.
- **Dos parejas de sentencias estaban en orden inverso.** El original hace `NumFullRebuilds++`
  (línea 312) **antes** de `first_zone` (314) y `last_zone` (315), y `NumCacheHits++` (343) antes de
  `first_zone_index` (345); nuestra fuente tenía las dos al revés, y además partía
  `TrackPathZone *first_zone;` de su asignación sin motivo. Corregido: misma cifra (99,8494 %), pero
  ahora el orden relativo de las notas de línea coincide con el del ELF original en las dos ramas.

En `LockPalette` el mismo cruce confirmó que la forma adoptada reproduce las notas del original **una
a una** (incluido el colapso de la línea 229 junto a la del `for` en la 231, que es lo que fija
`Pal32` dentro del `if` y `j` dentro del `for`); el único desajuste es el desplazamiento de 4 B a
partir del `new`, que es justo la instrucción que falta.

## Propuesta

Nada que decidir en esta ronda: los tres andamios salen y las tres unidades siguen NonMatching, así
que el DOL no se mueve. La propuesta es metodológica: **añadir el cruce `.line` contra
`symbols/debug_lines.txt` al triaje**, antes de dar por buena cualquier forma de fuente. En este lote
ha evitado sellar un 100 % falso y ha corregido dos órdenes de sentencias que llevaban rondas mal.
