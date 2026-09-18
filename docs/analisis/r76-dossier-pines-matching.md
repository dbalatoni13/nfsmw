# r76 — Los 15 pines que quedan en unidades Matching, interrogados al DWARF

Estos son los pines que sostienen el «100 % que miente»: viven en unidades
**Matching**, así que su objeto es el que se enlaza y su porcentaje es el que
cuenta. Antes de tocar ninguno, se le ha preguntado al oráculo
`python scripts/dwarf1.py fn <función>` — DWARF-1 de
`orig/GOWE69/NFSMWRELEASE.ELF`, emitido por el compilador exacto del build —
**qué locales tenía de verdad el original y en qué registro**. Si el nombre casa
con decenas de clases, se le pasa el **símbolo manglado**:
`dwarf1.py fn Setup__14CustomizeParts`.

El repo oficial prohíbe «Manually assigning registers to variables to force a
fake match». La pregunta que decide cada caso es una sola: **¿existe esa variable
en el original?**

## 1. Veredicto por pin

| # | fichero:línea | pin | ¿está en el DWARF? | qué dice el original | qué hay que escribir |
|---|---|---|---|---|---|
| 1 | `snd/9/.../gc/snddrv.c:599` | `void *mixSource` **r4** | **NO** | locales: `auxA` r31, `ptr` (estático), `pdst` (optimizada fuera), `i` r10, `j` r3, `psrc` r8, `old` r25. **r4 es el parámetro `context`** | la expresión en línea; la local sobra |
| 2 | `snd/9/.../gc/snddrv.c:617` | `void *mixSource` **r4** | **NO** | ídem | ídem |
| 3 | `snd/9/.../mix/sfsplit.c:66` | `void *copySource` **r4** | **NO** | params `pstate` r31, `frames` r29, `psrcp` r27, `pdstp` r28, `requester` r7; locales `pss` (fuera), `totalbufsize` r30, `retframes` r3. Ningún local en r4 | usar `psrcp` directamente |
| 4 | `csis.cpp:382` | `CListDNode *node` **r7** | **NO** | `Subscribe`: `pSystemDesc` r3, `i` r8, **`ptr` `unsigned int` r0** | el original trabaja con un `unsigned int ptr`, no con punteros a nodo |
| 5 | `csis.cpp:386` | `CListDNode *nullNode` **r9** | **NO** | ídem | la constante en línea |
| 6 | `csis.cpp:438` | `CListDNode *node` **r10** | **NO** | `Unsubscribe`: `pSystemDesc` r3, `i` r11 | ídem |
| 7 | `QuickGame.cpp:304` | `const Vector3 *collisionPos` **r4** | **NO** | `CreateCars`: `initialPos` [r1+8], `initialVec`, `initialVelocity` (fuera), `height`, **`SkipFEPlayerPerformance` / `SkipFEPlayerCar` / `SkipFE` (sin AT_location)**, `Heat` f29, `player_1`/`player_2`, `icar` r29, `ip` (fuera) | `&initialPos` en el sitio de uso. **Y ojo: tres locales del original que quizá no tengamos** |
| 8 | `gc_driver.cpp:600` | `int *nBytesRead` **r25** | **es el PARÁMETRO** | params: `this` r29, `handle` r26, `pBuffer` r31, `bytesToRead` r24, **`nBytesRead` r25** | renombrar nuestro parámetro `bytesReadOut` → `nBytesRead` y borrar la copia. No es «renombrar al azar»: es restaurar el nombre de los símbolos de depuración |
| 9 | `FEngine.cpp:618` | `unsigned long padMask` **r30** | **NO con ese nombre** | `ProcessPadsForPackage`: `Pressed` r18, `Released` r15, `Held` r19, **`Mask` r27**, `HeldFor`/`FromPadHeld`/`FromPadPressed`/`FromPadReleased` en pila, `PadIndex` r7, `i` r26, **`JoyMask` r20**, `bSomethingActive` r5 | el original tiene `Mask` y `JoyMask`; `padMask` es nuestro |
| 10 | `SteeringWheelDevice.cpp:317` | `float originalVal` **fr0** | **SÍ, y en f0** | `ConvertWheelRotation`: `val` f0, `originalVal` f0 | la local es real y el registro también: quitar solo el pin |
| 11 | `SteeringWheelDevice.cpp:327` | `float linearScale` **fr13** | **NO** | ídem (solo `val` y `originalVal`) | la expresión en línea |
| 12 | `gc_driver.cpp:275` | `ICardResult result` **r25** | **SÍ, y en r25** | `OpenFile`: `fd` r31, **`result` r25**, `coResult` r30, `convertNGCerror` r21 | la local es real y el registro también: quitar solo el pin |
| 13 | `CarCustomize.cpp:2780` | `vinyl_group_number` **r21** | **SÍ, y en r21** | `Setup__14CustomizeParts` (1708 B): `icon_hash` r29, `car_slot_id` r31, **`vinyl_group_number` `unsigned int` r21**, `is_vinyl` r30, `installed_part` r18, `part_found` r28, `part_list` [r1+8], `installed_index` r22, `current_part_index` r23, `original_icon_hash` r20, `part` r26 | la local es real y el registro también: quitar solo el pin |
| 14 | `LibSN/FSasync.c:301` | `block_high` **r30** | **sin DWARF** | `CompletePCreadAsync` no sale: el runtime de SN Systems no lleva depuración | sin oráculo; decidir por tamaño y forma |
| 15 | `LibSN/sndvd.c:273` | `OSContext *savedContext` **r29** = `context` | **sin DWARF** | `DSIHandler` tampoco sale | ídem. Pero es una copia literal de un parámetro: sobra por inspección |

## 2. Lo que esto demuestra

De los 15, **nueve pines sostienen una variable que el original no tiene**
(#1-#7, #9, #11) y **dos más son copias de un parámetro** (#8, #15). Es decir:
**once de quince son exactamente la práctica que el repo oficial prohíbe**, y se
puede demostrar con los símbolos de depuración del propio juego, no por opinión.

Los otros **tres** con oráculo (#10, #12, #13) son distintos y hay que decirlo: la local
existe y el registro que el pin fuerza **es el que el original le da**. Ahí el pin
no inventa nada; sólo le ahorra el trabajo al asignador. Salen igual —no son
portables— pero su retirada es una apuesta razonable a que el asignador acierte
solo.

Y el nombre del símbolo lo confirma en los tres: quien escribió esos pines
acertó la variable. El problema no es que mientan sobre el original, es que
`register T x asm("rN")` no compila en PS2 ni en Xbox 360.

## 3. Dos hallazgos secundarios que valen más que los pines

- **`CreateCars` tiene tres locales sin `AT_location`**: `SkipFEPlayerPerformance`
  (float), `SkipFEPlayerCar` (const char*) y `SkipFE` (int). «Sin location» en
  DWARF-1 quiere decir que el compilador las declaró y luego no les dio sitio:
  son variables de depuración/atajo que el original tenía en el fuente. Escribir
  las que falten es código real, y de las que mueven bloques.
- **`Subscribe` de csis usa un `unsigned int ptr` en r0**, no un puntero a
  `CListDNode`. Nuestra fuente modela esa parte con punteros a nodo; el original
  la modela con un entero sin signo. Eso no es un detalle de reparto: es otra
  forma de escribir la lista, y explica por qué 24 combinaciones de flags
  probadas en la r66 no movieron nada.

## 4. Protocolo para la tanda que los retire

1. El oráculo primero, siempre: la tabla de arriba ya lo trae hecho para 13.
2. Quitar el andamio es obligatorio; recuperar el porcentaje es opcional y sólo
   con C++ real.
3. Si la forma real no cierra al 100 %, **la unidad se degrada a NonMatching**
   (decisión del usuario, r76). Aviso medido en esta misma ronda: degradar una
   unidad que tenía dato muerto que sólo emitía nuestro objeto **mueve el DOL**
   (caso `filesys.cpp` → `"bad_alloc"` de `0x80413A70`); hay que mirar el
   `.rodata` de la unidad antes de degradar y, si hace falta, añadir la entrada a
   `keep.lst`.
4. El DOL manda: `python configure.py && python -m ninja build/GOWE69/ok` tiene
   que seguir dando `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.
