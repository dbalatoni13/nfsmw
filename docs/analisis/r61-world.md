# r61 — world (zWorld, zWorld2, zTrack)

**Resultado en una linea: zTrack pasa de 8.110 a 6.376 B de DOL distintos (−1.734 B,
−21,4 %) sin mover una sola funcion de su sitio, y la veda de 16 B de
`GetLoadingPriority` —abierta desde la r36f y visitada por r46, r48, r49 y r52— CAE.**

Ninguna unidad promociona. Lo que cambia es el orden de magnitud del frente de zTrack.

---

## 0. La foto de entrada, y por que el encargo apuntaba al sitio equivocado

Lo primero que hice fue medir las tres unidades en vez de ir a la lista de funciones:

| unidad | `linkdelta` | `fncmp` | `dolwhere` |
|---|---|---|---:|
| zWorld | `.text +0  resto IGUAL` | 4 de 582 | 29.551 B |
| zWorld2 | `.text +0  bss+64` | 2 de 357 | *secciones descuadradas* |
| **zTrack** | `.text +0  resto IGUAL` | **1 de 259** | **8.110 B** |

zTrack esta a UNA funcion de `fncmp` limpio. Pero sus 8.110 B de DOL no son esa
funcion: `dolwhere` los reparte en **1.287 sitios**, y los grandes no son de codigo.
`movidos.py zTrack` lo dice sin ambiguedad:

```
.data: 65 simbolos DE VERDAD permutados, 4.584 B
.bss:  dos escalones sin explicar, -8 (217 simbolos) y +160 (42 simbolos)
.text: 6 simbolos permutados, 328 B
```

O sea: **el frente de zTrack era de DATOS, no de codigo**, y ninguna herramienta del
triaje habitual lo decia — `linkdelta` daba `resto IGUAL` porque los TAMAÑOS cuadran.
Es exactamente el aviso de `movidos.py` en su propio docstring, aplicado a otra unidad.

---

## 1. zTrack: el orden de la `.data` (−813 B)

### 1.1 La causa

Los 16 bloques de `.data` escritos a mano de la r59 (`gap_06_*`, `lbl_*`) estaban
TODOS en un solo `asm()` **al final de `zTrack.cpp`**. La r59 los puso ahi a
proposito y lo dejo escrito: «Van al final del TU: cierran el TAMAÑO de la seccion
sin desplazar ningun simbolo nuestro ya colocado».

Cierran el tamaño, si. Pero el objetivo los tiene INTERCALADOS entre los simbolos
C++, y al ir todos al final cada uno de ellos empuja hacia atras a todo lo que
deberia ir detras suyo. Medido con la tabla de simbolos de los dos `.o`:

```
  +4400  gap_06_80435F60_data      -228  TUNHEIGHT
  +4392  gap_06_80435F70_data      -220  EventSlotPool / EventHandlerSlotPool / ...
  +4380  gap_06_80435F84_data      -152  VisibleGroupInfoTable / RegionCount / BaseFog*
  ...                              -140  SceneryGroupEnabledTable (4.096 B)
```

### 1.2 La palanca, y el detalle de GCC que la hace posible

Un `asm(".section \".data\"" ...)` de fichero **se emite EN SU POSICION del TU**,
intercalado con las variables C++, que GCC 2.95 emite en orden de declaracion.
Asi que los quince primeros bloques se pueden colocar exactamente donde toca:
cinco entre `#include`s de `zTrack.cpp` y diez dentro de los `.cpp` (todos
propiedad exclusiva de zTrack — verificado con grep sobre `SourceLists/`).

**Contraste que hay que apuntar en el catalogo**: eso NO vale para `.bss`. Las
variables de fichero sin inicializar salen por `.lcomm` y GCC las drena TODAS en
`finish_file`, o sea DETRAS de cualquier `asm()` de fichero. Por eso el `.space 160`
de la r59, escrito al final del TU, caia **en medio** de la `.bss` (justo detras del
ultimo static local y delante del primer `.lcomm`). Ver §1.4.

### 1.3 Lo que hubo que corregir ademas del asm

Con los quince bloques en su sitio quedaban cuatro desordenes de DECLARACION:

1. `SeeulatorToolActive`, `ScenerySectionToBlink` y `ShowSectionBoarder` se
   **definian en `TrackStreamer.cpp`** y el objetivo los emite en el tramo de
   `Scenery.cpp` (0x804360D8..E0), detras de los cuatro punteros a callback. Los
   `extern` que `Scenery.cpp:96-99` tenia puestos delante eran la pista: en el
   original las definiciones estan AHI y los `extern` van en `TrackStreamer.cpp`.
   Intercambiados.
2. `ForceAllSceneryDetailLevels` sube al principio de `Scenery.cpp`, por delante
   de `gap_06_80436004_data` y de `EnvMapShadowExtraHeight`.
3. `SeeulatorRefreshTrackStreamer` baja detras de los tres anteriores.
4. En `ScreenEffects.cpp`, el tramo `{lbl_8043712C, FACflush, gap_06_80437138_data,
   GlareFalloff, GlareFallon, lbl_80437144, TUNHEIGHT}` baja **detras de `TickSFX`**
   (la funcion del static `ticS`), porque el objetivo emite `debugflash` y `ticS`
   ANTES que `FACflush`. `debugflash` se queda donde estaba.
5. `static bool bPrintName = false;` (+ su `if (bPrintName) {}` vacio) de
   `Scenery.cpp:750` **no existe en el objetivo**: es el `+4` que `linkdelta` daba
   en `.data`. Quitado; `fncmp` no se mueve (`.text` identica).

### 1.4 El `.bss`: el relleno estaba en medio

`.space 160` en un `asm()` cae delante del bloque de `.lcomm`, o sea a 0x2C90 de la
seccion, y empuja **+168 B** a los 217 simbolos que van detras. Reemplazado por una
declaracion C++ al final de `EventManager.cpp`:

```c
// r61-world: relleno de .bss, al FINAL del TU.
char gap_07_804A5DB8_bss[160];
```

Dos avisos medidos:
* con nombre propio (`_r61_bss_tail`) `-strip-unused-data` **se lo lleva**:
  `linkdelta` dio `bss-160`. Con el nombre de `keep.lst` (`gap_07_804A5DB8_bss`,
  que ya estaba nombrado desde la r58 y nadie emitia) sobrevive y vuelve a `IGUAL`.
* `gap_07_804A59DA_bss` (2 B) de `keep.lst` **no hace falta emitirlo**: es el relleno
  de alineacion entre `gPrecullerBooBooManager` (que acaba en 0xB436) y
  `SE_PaletteFile` (que pide alineacion 4). El enlazador lo pone solo.

### 1.5 Resultado

```
.data permutada   65 simbolos / 4.584 B   ->   0 simbolos / 0 B
.bss escalones    -8 (217) y +160 (42)    ->   -8 (217)
dolwhere zTrack   8.110 B                 ->   6.582 B
```

La `.data` de zTrack es hoy **identica al objetivo en direccion y tamaño, simbolo a
simbolo** (`diff` de las dos tablas: vacio).

Lo que sobrevive del escalon de `.bss` son **8 B**: `value.15675` y `_.tmp_0.15676`,
el static de `VehicleParams::TypeName()` (`src/Speed/Indep/Src/Physics/PVehicle.h:49`,
`static UCrc32 value = DEAD_STR("VehicleParams");`) que GCC emite aunque la inline no
se emita ni haya una sola insn que lo toque (comprobado en el `.s`: solo aparece en
`.lcomm` y en `.debug`). Llega por `CameraMover.hpp` -> `WeatherMan.cpp`, y **quitar
ese include no compila** (`CameraAnchor` sin declarar, medido). Es cabecera
compartida: va como propuesta, no aplicado. Ver §5.

---

## 2. zTrack: `GetLoadingPriority`, los 16 B del marco (−206 B, y la veda cae)

### 2.1 El premio gordo estaba escrito y nadie lo habia probado

El bloque r49 de `TrackStreamer.cpp` termina diciendo «Lo que hay que encontrar es la
sentencia que crea ese temporal de 16 B» y la r60b lo concreta: «Candidato 1:
`FloatVector pos = { section->Centre.x, section->Centre.y, 0.0f };`». **Nunca se
habia compilado.** Compilado hoy, a la primera:

| | tamaño | filas | `stwu` |
|---|---|---:|---|
| base (tres asignaciones) | 708/708 | 71 | `-0x110` |
| **`FloatVector pos = {…};`** | **708/708** | **28** | **`-0x120` = el del objetivo** |

Con el marco cuadrado vuelven a su sitio `dest` (0xA8), `predict_pos` (0xB8),
`direction` (0xC0) y `v` (0xC8) — los cuatro desplazamientos +0x10 que r46 habia
enumerado uno a uno. Y **no es una local inventada**: es un inicializador agregado,
que es lo que crea el temporal sin nombre que el DWARF del original deja en r1+0x38.
La prohibicion de r49 (precedentes ICEMover y `pos2`) sigue en pie y esto no la roza.

### 2.2 El paso 1 de la r60b, confirmado

La forma de DOS sentencias con `(90 - clamp) * K1` fuera (`TrackStreamer.cpp:2207`):

* **el observable de la r60b se cumple**: el `fmuls f30,f30,f11` de nuestra fila 146
  —`speed_factor * 0.66999996f` adelantado, un arbol que el objetivo no tiene—
  DESAPARECE, y el racimo final queda instruccion por instruccion como el objetivo
  (`fsubs`, `*K1`, `*speed_factor`, `*K2`, `1-x`, `*distance`, `*K3`).
* como r46 y la propia r60b avisaban, **el numero de filas empeora**: 28 -> 32.
* el DOL las separa por **UN byte**: `dolwhere` 6.375 con la forma de una sentencia,
  **6.376** con la de dos.

Se aplica la de dos (el arbol correcto). Las 4 filas de diferencia son el orden de los
`lis $LC362@ha` / `lfs` del pool, no el arbol.

### 2.3 Negativos medidos sobre la base nueva

Con el marco ya cuadrado, el residuo de 32 filas es reparto de FP. **El pin no es su
palanca**, y los tres ensayos ROMPEN EL TAMAÑO, que era justo el control que la veda
pedia:

| variante | tamaño | filas |
|---|---|---:|
| `register float speed asm("fr31")` | 708/**712** | 68 |
| `register float speed_factor asm("fr30")` | 708/**716** | 65 |
| las dos | 708/**712** | 68 |

Por la regla de `nfsmw-si-el-pin-empeora`: hay que arreglar antes la primera
diferencia que NO es de registro, y es el orden de los `lis`/`lfs` del pool en las
filas 144-150.

---

## 3. zWorld — `DefragmentPool`: el modelo de la r49 es EXACTO, y aun asi no paga

Lo que pedia el encargo era el criterio de descarte barato. Sale mejor de lo esperado:
**no hace falta compilar zWorld en absoluto**. Banco nuevo, 4 s por variante:

```
python scripts/rtldump.py --file src/Speed/Indep/Src/World/CarLoader.cpp \
       --like zWorld DefragmentPool -dl
```

y del `.lreg` se leen las ocho columnas `Register N used R times across L insns`.
Verificado que el mini-TU da los MISMOS numeros que la SourceList entera.

BASE (identica a la de r49, o sea que el arbol no se ha movido):
`82=162 139=184 141=120 144=61 145=118 222=51 223=54 225=246`.
Umbral de r49/r60b para invertir el ciclo A: `live(222) <= 47` **o** `live(82) >= 173`.

| variante | resultado |
|---|---|
| R1 `movement` antes de `allocation_size` | **IDENTICA a la base** -> se tira sin compilar |
| R2 `movement` la primera del cuerpo | **IDENTICA** -> se tira |
| R3 `table` antes de `params` | 141 120->61, 144 desaparece; compilada = **la base exacta** |
| R4 `loop_number++` antes de `allocation_num++` | **IDENTICA** -> se tira |
| P1 asm delante de `allocation = table[…]` | 222=52 |
| P5 asm delante de `allocation_size` | 222=50 |
| P6 asm detras de `int zero = 0;` | 222=52 |
| P7 asm delante de `ChunkMovementOffset = zero` | 222=52, 223 54->56 |
| P3 asm delante de `gDefragFixer.Add` | 222=52 |
| **P2 asm ANTES de `int movement = 0;`** | **222=47 — LLEGA** |
| **P8 asm delante del `if (allocation > first_hole)`** | **222=46 — LLEGA** |

**Compiladas las dos que llegan, y las dos confirman la causa y empeoran el total:**

* P2: 684/684 B, 96,78362 %, **32 filas** (base 23). El ciclo A (`r21<->r22`, 9 filas)
  DESAPARECE — no queda una sola fila de r21/r22 — pero aparece un ciclo `r29<->r30`
  NUEVO de 12 filas, y `addi r22,r30,1` y `li r31,0` bajan cinco ranuras.
* P8: 684/684 B, 97,60234 %, **30 filas**. Mismo efecto, mismo precio.

**Conclusion, y es lo que hereda la ronda que viene**: `allocno_compare` de r49 predice
al insn — basta `live(222)<=47` para invertir el ciclo A. Pero las dos unicas formas
que lo consiguen lo hacen **metiendo una insn en el bucle**, y esa insn alarga +2 la
vida de TODOS los demas pseudos (82 162->163, 139 184->186, 141 120->122, 144 61->62,
145 118->120, 225 246->248) y rompe un reparto que ya estaba bien. Falta una forma que
acorte 222 **sin añadir insns**: las cuatro reordenaciones puras que he probado son
neutras al byte. 222 es el `allocation_num + 1` que loop.c iza (`addi r22,r30,1` de la
fila 84, usado en la 132): hay que atacar SU rango, no el del bucle.

Todo esto queda escrito en `CarLoader.cpp` junto a `DefragmentPool` (previo.py lo ve).
**El arbol de zWorld queda EXACTAMENTE como estaba** (23 filas, 99,269 %): solo se ha
añadido el comentario.

---

## 4. zWorld2

### 4.1 `HolePunchAvoidables` — PASO 0 HECHO: los ocho movables, escritos

El encargo lo marcaba obligatorio. Salen del `.lreg` (8,6 s, sin compilar zWorld2).
Base de hoy identica a la de r47/r48: `live 334=494, 901=492, 505=483`.
Orden real de la cadena de insns en el preencabezado:

```
insn 3254   (set (reg:CC 334) (compare (reg:SI 198) 0))     <- def(334)
insn 3341   (set (reg:SF 882) (mem *$LC435))
insn 3342   (set (reg:CC 901) (compare (reg:SI 216) 0))     <- def(901)
  1 insn 3325   (set (reg/v:SF 802) (mem *$LC425))    <- el literal 0,2f
  2 insn 3235   (set (reg:SI 969) (plus r31 8))
  3 insn 3236   (set (reg:SI 249) (plus r31 184))
  4 insn 3237   (set (reg:SI 252) (plus r31 232))
  5 insn 3238   (set (reg:SI 253) (plus r31 248))
  6 insn 3239   (set (reg/v:SI 261) (plus r31 216))
  7 insn 3253   (set (reg:SI 295) (high "_IHandle__8IVehicle"))
  8 insn 3268   (set (reg:SI 372) (high "_12VehicleClass.TRAILER"))
insn 3283   (set (reg:CC 505) (compare (reg:SI 212) 0))     <- def(505)
```

**Ocho exactos**, que es el numero que la r48 dio: cuadra al insn (492−483 = 9 = los
ocho mas el propio `def(505)`; 494−492 = 2 = insn 3341 mas `def(901)`).

**Cinco de los ocho son `addi rN, r31, K`**: la direccion de cinco locales de pila
cuyo puntero se toma dentro del bucle (r31+8, +184, +216, +232, +248). Los otros tres
son el literal 0,2f, `_IHandle__8IVehicle` y `_12VehicleClass.TRAILER`.

La tercera rama sigue **sin evaluar**, pero ahora tiene diana: sacar UNO de los ocho de
la ventana adelantandolo por delante del test que define 901 (loop.c iza en el orden
en que aparecen en el cuerpo). Observable de causa: `live(901)` 492 -> 491 **con
`live(505)` quieto en 483**; entonces pri = 30000/491 = 61, 30000/483 = 62 y
30000/494 = 60, que rompe el empate 60/60 de hoy. Control: 2980/2980.

### 4.2 `InitAtSegment` — la palanca de OCUPACION queda REFUTADA con control byte a byte

Base: 816/816, 99,65686 %, **12 filas y las doce son el mismo swap `r10<->r11`**.

| variante | tamaño | % | filas |
|---|---|---|---:|
| V1 clobber `"r11"` detras de `fEndPos` | 816/816 | 89,867645 | 58 |
| V2 clobber `"r10"` detras de `fEndPos` | 816/816 | 89,867645 | 58 |
| V3 clobber `"r11"` entre `SetLaneInd` y `SetLaneOffset` | 816/816 | 89,867645 | 58 |
| V4 clobber `"r10"` en el mismo sitio | 816/816 | 89,867645 | 58 |
| V5 clobber `"r11"` delante de `fStartPos` | 816/816 | 99,65686 | 12 = **la base** |
| V6 clobber `"r10"` delante de `fStartPos` | 816/816 | 99,65686 | 12 = **la base** |
| V7 clobber `"r11"` entre `fStartPos` y `fEndPos` | 816/816 | 89,46568 | 34 |
| **V8 `__asm__("");` PELADO, sin un solo clobber** | 816/816 | 89,867645 | 58 |

**V8 da EL MISMO sha1 que V1 y que V2** (`0c3da90522c4…`): los tres objetos son
identicos byte a byte. O sea que en esta funcion **la lista de clobbers no influye en
nada** — el unico efecto del `asm` es el corte de region (r51), y cuesta 46 filas.
La distincion «palanca de PRIORIDAD (refutada en r53) contra palanca de OCUPACION»
que sostenia la receta de la r60b **no se sostiene aqui**: GCC 2.95 no mete el clobber
de un `asm` sin operandos en `regs_live_at` de forma que llegue a `find_free_reg`.
**La rama esta AGOTADA.** El swap r10/r11 sigue abierto por otro eje (el ORDEN de
reparto de `local_alloc`).

Escrito en `WRoadNetwork.cpp` junto a las dos funciones. **El arbol de zWorld2 queda
EXACTAMENTE como estaba**: solo comentarios (`fncmp` 2 de 357, mismas 4 y 12 insn).

---

## 5. Lo que NO he hecho porque toca cosas ajenas — propuestas para el jefe

Nada de esto esta aplicado.

### 5.1 El frente que queda en zTrack es UNO, y esta bloqueado por `lcfix`

De los 6.376 B que quedan, **3.044 estan en un solo sitio**: `lbl_80408FB8 (fuera)`
a 0x80409174, o sea el ORDEN de las cadenas de `.rodata`. `rodorden.py zTrack`:
emitimos 167 cadenas contra 138 del objetivo y el bloque `CODEINE / Track Streaming
Buffer / TSMemoryPool / …` sale en `nue[57:110]` cuando el objetivo lo tiene en
`obj[86:97]`. **Todo lo que lo toca renumera los `$LC` y obliga a correr `lcfix.py`**,
que es justo lo que la regla 3 me prohibe. Son tres cambios que van JUNTOS y solo
tocan entradas `zTrack.o:$LC*` — **no envenenan a ninguna otra unidad** (las entradas
de `keep.lst` son por objeto):

1. quitar la segunda copia de `"GAMECUBE"` (el negativo r60 de `zTrack.cpp:49`, que
   se midio bueno y se revirtio SOLO por esto);
2. los ~470 B de `Attrib::*` duplicados que van detras;
3. reordenar el bloque de cadenas.

Hoy `python scripts/lcfix.py --check zTrack` dice **«todas las entradas @lc estan al
dia»**, y lo he dejado asi: **CERO correcciones pendientes por mi parte.**

### 5.2 Cabecera compartida — NO tocada (regla 6)

`src/Speed/Indep/Src/Physics/PVehicle.h:49`

```c
static UCrc32 value = DEAD_STR("VehicleParams");
```

es lo unico que queda del escalon de `.bss` de zTrack (8 B: `value` + `_.tmp_0`, que
GCC emite sin una sola insn que los use). Con esos 8 B fuera, la `.bss` de zTrack
seria **exacta**: bastaria subir `gap_07_804A5DB8_bss` de 160 a **168** B en
`EventManager.cpp` y todo cuadraria al byte (0xB814 + 168 = 0xB8BC). No lo aplico
porque la cabecera la comparten zCamera y compañia y eso ya hundio unidades sin agente
en la r56/r57/r58. **Los dos cambios van juntos o no van.**

### 5.3 `keep.lst` / `splits.txt` / `configure.py`

**Ninguna propuesta.** `keepchk`, `keepchk2`, `gapchk`, `prefijochk` y `checksplits`
no dan una sola linea de zTrack ni de zWorld2 (los 5 huecos de zWorld son previos y no
los he tocado). `gap_07_804A5DB8_bss`, que llevaba nombrado en `keep.lst` desde la r58
sin que nadie lo emitiera, **ahora existe**.

---

## 6. Estado final, sellado

Tres compilaciones consecutivas de las tres unidades, mismo sha1 las tres veces:

```
zTrack   48df47cb50e60640c37b3393e8a1a2d5dfb6576f
zWorld   5b305aa6b7ca1110ae80c6352ae1e70a7c716bab
zWorld2  96f30384eb78abd7247c733457d2c7e0ee28f346
```

| unidad | `linkdelta` | `fncmp` | `dolwhere` | `trypromo` |
|---|---|---|---:|---|
| zTrack | `.text +0  resto IGUAL` | 1 de 259 (`GetLoadingPriority`, 29 insn) | **6.376** (era 8.110) | DOL ROTO |
| zWorld | `.text +0  resto IGUAL` | 4 de 582 | 29.551 (igual) | DOL ROTO |
| zWorld2 | `.text +0  bss+64` | 2 de 357 | *secciones descuadradas* | DOL ROTO |

**Aviso de higiene**: `stage2` dejo 39 secuencias `
` en `ScreenEffects.cpp`
(el bloque movido ya venia con CRLF y la sustitucion lo volvio a convertir). Detectado
con `grep` de `` no seguido de `
` —no con `file`, que no lo ve— y **corregido**;
el `.o` sale con el MISMO sha1 antes y despues, asi que no toco el codigo, pero el
`git diff` pasaba de 410/370 lineas a 44/4. Los otros ocho ficheros: 0 CR sueltos.
(`WeatherMan.cpp` tiene 21 finales LF sueltos, pero son PREVIOS: el fichero ya venia
mezclado, 276 LF contra 255 CRLF, y mis 13 lineas son CRLF.)

Regresiones comprobadas, no supuestas: `gapchk` (nada de zTrack/zWorld2),
`prefijochk` LIMPIO, `checksplits` LIMPIO (0 rangos que corten un simbolo),
`keepchk`/`keepchk2` sin una sola entrada de mis unidades, `lcfix --check zTrack` al dia.

## 7. La cola de zTrack, ordenada, para quien la recoja

| que | B | bloqueo |
|---|---:|---|
| orden de las cadenas de `.rodata` | 3.044 | necesita `lcfix` (§5.1) |
| `GetLoadingPriority`, 32 filas de reparto FP | ~700 + arrastre | eje: orden de `lis/lfs` del pool, NO el pin |
| `.text`: 6 simbolos permutados | 328 | `textorder.py`, sin mirar todavia |
| `value` + `_.tmp_0` de `PVehicle.h` | 8 B de `.bss`, 217 simbolos desplazados | cabecera compartida (§5.2) |
