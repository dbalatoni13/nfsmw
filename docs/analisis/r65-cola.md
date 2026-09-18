# r65 — lote «cola»: retirar andamios de los ficheros con 1 a 3

**Encargo**: dejar fuente legítima, no ganar bytes. Test único: los *digests por
sección* de las secciones ALLOC (`.text`, `.rodata`, `.data`, `.sdata`,
`.sdata2`, `.ctors` y sus `.rela.*`) antes y después, ignorando `.line`,
`.debug*`, `.comment` y `.stab*`. Idénticos → aceptar; distintos → revertir.

## 1. Cifras

| | |
|---|---|
| ficheros del lote | **60** |
| andamios al empezar | **88** (26 barreras + 62 pines) |
| andamios al terminar | **75** (26 barreras + 49 pines) |
| **retirados** | **13** |
| intentados (medidos uno a uno) | **88 + 30 = 118** |
| revertidos tras medir | **2** (`TrackPath.cpp:186`, `PhysicsObject.cpp:62`) |
| irreducibles anotados en la fuente | **64 sitios** |
| regresiones | **0** |

El lote del brief decía «64 ficheros, 100 andamios». El censo limpio con las dos
expresiones del brief, descontando los 26 ficheros de los otros nueve lotes, da
**60 ficheros y 88 andamios**: los 4 ficheros y 12 andamios de diferencia son
`Smackable.cpp`, `MemoryCardCallbacks.cpp`, `itoa.c` y `fseek.c`, que el brief
manda descartar y a la vez cuenta.

## 2. Método y herramientas (en `scratchpad/cola65/`)

* `censo.py` — censo con las dos expresiones del brief, **saltándose comentarios**
  (un grep crudo infla el recuento con los andamios citados en las notas).
* `cc.py` — compila una unidad **a un `.o` de scratch propio**
  (`scratchpad/cola65/o/<unidad>.o`) leyendo los cflags de `build.ninja`. Nunca
  toca `build/GOWE69/{obj,src}`, así que no compite con los otros nueve agentes
  por el fichero de salida ni puede envenenar sus medidas. Comprobado que el
  digest no depende del nombre de salida.
* `tool.py` — digests **por sección ALLOC** de un ELF32 BE, sin dependencias.
* `sweep.py` — barrido por unidad: digest base → quita → recompila → compara →
  restaura si cambia. Con `--bis`, bisecta andamio a andamio.
* `sello.py` — el sello final: para cada retirada, construye **CON** el andamio y
  **SIN** él *en el mismo instante* y enfrenta los dos digests.
* `d.py` — diff de una función con `objdiff-cli` contra el `.o` **original
  extraído**, con `ppc.calculatePoolRelocations=false`.
* `anota.py` — escribe el diagnóstico junto al andamio.

El orden importa: **primero el barrido de andamios caducados** (coste: 2
compilados por unidad, sin pensar), y sólo después buscar formas de fuente. De
los 13 retirados, **13 salieron del barrido**: ninguna de las formas de fuente
que probé a mano cerró un sitio.

## 3. Las 13 retiradas, con sus digests

Todas son pines `register T x asm("rN")` → declaración normal `T x`. Los digests
son los del sello final (`sello.py`), medidos CON y SIN en la misma pasada:

| unidad | fichero:línea | andamio retirado | digest CON | digest SIN |
|---|---|---|---|---|
| `…/lib/gc/tasks` | `tasks.cpp:415` | `register bool found asm("r28")` | `85c2564d6e2f2671` | `85c2564d6e2f2671` |
| `…/path/…/pathsnd` | `pathsnd.cpp:514` | `register int updateStatus asm("r9") = 1` | `342f247c8b7612ef` | `342f247c8b7612ef` |
| `…/snd/…/saemstimupdt` | `saemstimupdt.c:6` | `register AemsDef::TIMERCLIENT *pclient asm("r9")` | `6bbc2775659c1895` | `6bbc2775659c1895` |
| `…/snd/…/saemstimupdt` | `saemstimupdt.c:13` | `register CListDNode *pNode asm("r0")` | `6bbc2775659c1895` | `6bbc2775659c1895` |
| `…/snd/…/sfsplit` | `sfsplit.c:41` | `register unsigned int copySize asm("r5")` | `7de1da0a219d89b6` | `7de1da0a219d89b6` |
| `libc/memset` | `memset.c:9` | `register unsigned long buffer asm("r0")` | `2ff666ed8f8b5167` | `2ff666ed8f8b5167` |
| `zEAXSound` | `EaxSoundTypes.cpp:253` | `register unsigned int leftPriority asm("r9")` | `9e92525a5436ae95` | `9e92525a5436ae95` |
| `zEAXSound` | `SndStrmWrapper.cpp:19` | `register int overhead asm("r0")` | `9e92525a5436ae95` | `9e92525a5436ae95` |
| `zEAXSound` | `SFXCTL_AccelTrans.cpp:78` | `register bool oldIsAccelerating asm("r10")` | `9e92525a5436ae95` | `9e92525a5436ae95` |
| `zSim` | `SimEntity.cpp:11` | `register UTL::COM::Object *owner asm("r3")` | `6913d4cebd1c34a7` | `6913d4cebd1c34a7` |
| `zBWare` | `bList.cpp:43` | `register SortFunc cmp asm("r27")` | `51df0949941a0339` | `51df0949941a0339` |
| `zBWare` | `bList.cpp:44` | `register int did_swap asm("r28")` | `51df0949941a0339` | `51df0949941a0339` |
| `zBWare` | `bMath.cpp:351` | `register int quad asm("r7")` | `51df0949941a0339` | `51df0949941a0339` |

Las tres de `zEAXSound` se midieron una a una **y también las tres juntas**
(`>>> CONFIRMADO en conjunto`), porque un andamio puede ser neutro solo y no en
compañía. Igual las dos de `zBWare`.

**Las dos que se revirtieron, y por qué importa**:

* `TrackPath.cpp:186` (`register TrackPathZone *found_zone asm("r26") = nullptr`)
  midió **caducado** con base `03389f8d1ea226ff`. Media hora después, con base
  `5cea9795242687ee` —otro agente había tocado `zTrack`—, la misma retirada daba
  `.text` y `.rela.text` distintos. Revertida.
* `PhysicsObject.cpp:62` midió caducado con base `ff1fb3c41d3c588b` y en el
  sello final, con base `9e1d028532a8c977`, salía `99baede9bff4f98e`. Revertida
  (el fichero vuelve a estar idéntico a HEAD).

## 4. Hallazgos que valen más que las retiradas

### 4.1 El censo del brief se deja fuera 73 barreras

Las dos expresiones del brief (`__asm__\s*\(\s*""` y el pin) dan 362 andamios.
Pero **hay 73 barreras más escritas `asm("")`, sin `__asm__`**, en 37 ficheros
(`scratchpad/cola65/censo2.py`). Son el mismo andamio y cuentan igual:

```
  11 src/libc/vfprintf.c            6 src/Speed/GameCube/Src/JoyE.cpp
   6 .../sfxctl/SFXCTL_3DObjPos.cpp 4 .../path/.../pathsnd.cpp
   4 src/Speed/Indep/Src/World/Common/WRoadNetwork.cpp   ... (33 ficheros más)
```

**El censo real del proyecto es 435, no 362.** De esas 73, **30 caen en ficheros
de mi lote**; las barrí todas una a una con el mismo criterio y **ninguna está
caducada** (`censo_ext.py` + `sweep.py --bis` con `CENSO=censo_ext.json`).
Conviene rehacer el reparto de lotes con la expresión corregida.

### 4.2 «Caducado» es una medida con fecha de caducidad

Los dos revertidos del §3 no son fallos de medida: son la misma verdad de
`nfsmw-andamios-caducan.md` **en el otro sentido**. Un andamio innecesario hoy
puede volver a hacer falta mañana si cambia una vecina de su unidad. Corolario
operativo: **una retirada no está cerrada hasta que se sella contra el árbol con
el que se va a commitear**, y con nueve agentes en paralelo eso significa
*inmediatamente antes* de promocionar. `sello.py` es exactamente eso y debería
ser un paso fijo del protocolo, no algo que se hace al final si sobra tiempo.

### 4.3 Para un PIN, el veredicto ESTRUCTURA de regmap suele ser auto-referencial

Pasé `regmap.py` por las 88 funciones del lote
(`scratchpad/cola65/regmap.txt`). 34 de los sitios salen «ESTRUCTURA: 0 locales
sólo en el original, **N sólo nuestras**». En casi todos, **esas N locales son las
del propio pin**: `register bool wasPaused asm("r0") = true` es una local que el
original no tiene porque el original no tiene el andamio. Perseguir ese
veredicto es perseguir tu propio parche.

Lo que sí es una pista de verdad es la columna contraria, **«N sólo en el
ORIGINAL»**, y en el lote sólo la tienen cuatro funciones:

| función | sólo en el original |
|---|---|
| `CDActionDrive::Update` | **6** (un bloque entero) |
| `QuickGame::CreateCars` | 4 |
| `EPlayRaceNIS::EPlayRaceNIS` | 3 (+8 en ámbito equivocado) |
| `TrackStreamer::HandleLoading` | 1 |

Y una quinta con **ámbito equivocado**: `eSolid::GetPostionMarker`.

### 4.4 Un andamio puede estar tapando CÓDIGO QUE FALTA

`CDActionDrive.cpp:406` es el caso limpio y hay que saber reconocerlo:

```c
    isBeingPursued = false;
    __asm__("");
    mAnchor->SetCloseToRoadBlock(isBeingPursued);
```

`regmap` dice que al original le falta **un bloque anónimo completo** que declara
`iperp` (`IPerpetrator*`), `playerList` (`const List&`) y, anidados, `iter`,
`ip` (`IPlayer*`) y `simable` (`ISimable*`). O sea: el original **recorre la
lista de jugadores** para decidir `isBeingPursued`, y nosotros escribimos
`false`. La barrera sólo impide que ese `false` se propague hasta la llamada.

**No hay forma de fuente que sustituya esta barrera**: hay que escribir el
bloque que falta, y eso cambiará el objeto. Es trabajo de decompilación, no de
andamio, y por eso está anotado como tal y no como «irreducible del asignador».

### 4.5 `REG_ALLOC_ORDER` explica la mitad de los pines

`orig/prodg/NGC_GNU_SRC/NGC/gcc/config/rs6000/rs6000.h:932` da el orden de
reparto de GPR: **`{0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31…12}`**. Histograma de los
49 pines que quedan en el lote: **r0 ×9, r4 ×6, r11 ×5, r10 ×5, r9 ×3** — es
decir, **22 de 49 pinchan uno de los cuatro primeros de la lista**. Eso dice que
la mayoría de estos pines no piden «un registro raro»: piden **cambiar el ORDEN
en que se reparten dos pseudos**, que es lo que decide quién coge r0/r9/r11/r10.

El caso desmenuzado es `SYSTEM_addexit` (`exit.cpp:14`), donde el original tiene
**una sola** local (`j`, r11) y nosotros dos:

* probadas las tres formas de una sola local (`int j` compartido con el primer
  bucle; `int j` declarado antes de `pf`; `i` y `j` los dos fuera del `for`) y
  **las tres dan el mismo objeto**: 108 B, 6 filas, y un **SWAP r10↔r11**;
* la causa es el orden de arriba: el índice escalado vive en un solo bloque, lo
  coge `local_alloc` primero y se queda **r11**; `j` cruza bloques, le toca
  `global_alloc` y ya sólo hay **r10**. En el original es al revés, luego allí
  `j` se repartió antes;
* con el pin, 100,0 %. **Es una PERMUTACIÓN DE DOS y el pin es la única palanca
  que llega a `local_alloc`.** Documentado en la fuente para que nadie repita
  las tres formas.

### 4.6 `eSolid::GetPostionMarker`: el ámbito no basta

`regmap` dice ÁMBITO EQUIVOCADO: el original declara la local a nivel de
**función** y la llama `next_marker` (**r3**, el registro de retorno); nosotros
dentro del `if`. Subida la declaración, probadas las dos formas naturales:

| forma | `.text` de la unidad | filas |
|---|---|---|
| `next_marker = &tabla[n-1]`, `prev < next` | **−4 B** | — |
| `next_marker = prev_marker + 1`, `next <= &tabla[n-1]` | igual | **15** |

El diff contra el original destapa lo que falta de verdad: **el original saca el
parámetro de su registro de entrada** (`mr r11, r4` como primera instrucción)
para dejarle **r4** a `position_marker_table`. En C a secas pasa lo contrario
—el parámetro se queda en r4 y la tabla va a r11— porque r11 va antes que r4 en
`REG_ALLOC_ORDER`. El pin `asm("r4")` reproduce exactamente ese `mr`.

### 4.7 `SNDI_validrendermode`: no es cross-jumping, es HOISTING

La nota que había en `srender.c` culpaba al `continue`. El diff contra el
original dice otra cosa, y es un diagnóstico más fino: sin la barrera **se iza el
`lwz r9, 0(*prenderindex)` por encima del `and.`**, con lo que el `continue`
temprano se queda vacío y salta directo al incremento (`beq`). El objetivo **no
lo iza**: duplica el `lwz` dentro del camino del `continue` y usa la polaridad
contraria (`and.` / `bne fuera` / `lwz` / `b incremento`). Lo que hay que frenar
es el **hoisting de la carga común a los dos sucesores**, no una fusión de colas.

## 5. Irreducibles: 64 sitios anotados en la fuente

`anota.py` escribió, **junto a cada andamio** que no tenía ya nota, cuatro líneas
con (a) que la retirada a secas cambia el `.text` —medido, no supuesto—, (b) el
veredicto literal de `regmap` con su unidad y función, y (c) el aviso del §4.3
cuando el sitio es un pin. Cinco sitios llevan además el diagnóstico largo de los
§4.4, §4.5, §4.6 y §4.7. Verificado que **añadir esos comentarios no mueve ni un
byte de ninguna sección ALLOC** (7 unidades comprobadas contra su digest previo,
idénticas).

Los andamios de `pathnode.cpp` (rotación de cuatro `fr`), `EcstasyEx.cpp`,
`AIPursuit.cpp`, `pathbank.cpp`, `inittmr.cpp`, `avplayer.cpp`,
`FEpkg_MU_Keyboard.cpp`, `GRaceDatabase.cpp`, `ENVIRO_AEMS.h` y `ppc2D2.c` ya
traían su diagnóstico de rondas anteriores y se han dejado intactos: mi medida de
esta ronda los confirma NECESARIOS uno a uno.

Cuatro sitios no tienen oráculo de `regmap` porque la función no está en el
volcado DWARF del original (`__do_global_ctors`, `__cvt_fp2unsigned`, `memset`,
`FEKeyboard::ToggleCapsLock`, los dos constructores de `Path::PathTrackSnd*` y
`PackedDecimal::PackedDecimal`): son runtime de SN, libc de Metrowerks sin
depuración, o `inline`s sin entrada propia. Anotado también, para que nadie
espere un veredicto que no puede existir.

## 6. Verificación y regresiones

* Recompiladas **las 30 unidades** que toqué (anotaciones incluidas) al terminar.
  Las 13 de biblioteca dan **exactamente** el digest que tenían antes de que yo
  empezara: `tasks 85c2564d…`, `gc_driver cbf057ce…`, `pathsnd 342f247c…`,
  `pathnode 2e365817…`, `pathbank 7d8d7d17…`, `inittmr 62a71579…`,
  `saemstimupdt 6bbc2775…`, `snddrv 9eecad2a…`, `sfsplit 7de1da0a…`,
  `sfir 19973ea1…`, `avplayer b5ecdb18…`, `crt2D1 3ad109fd…`, `ppc2D2 18a59593…`.
* `git diff` del lote: **52 ficheros, 552 inserciones y 13 borrados**. Los 13
  borrados son **exactamente** las 13 líneas `register … asm("rN")` retiradas
  (comprobado uno a uno con `git diff -U0 | grep '^-[^-]'`); todo lo demás son
  las anotaciones. Ni un `#if defined(__ANDROID__)` tocado, ni una cabecera
  compartida, ni `configure.py`, ni `config/GOWE69/*`, ni `splits.txt`.
* **Un accidente propio, encontrado y corregido**: mi reescritor leía en modo
  texto con saltos universales y volvía a escribir siempre con `\n`, así que
  aplanó a LF seis ficheros que en el árbol de trabajo estaban en CRLF
  (`tasks.cpp`, `AIPursuit.cpp`, `SndStrmWrapper.cpp`, `bMath.cpp`,
  `SFXCTL_AccelTrans.cpp`, `bList.cpp`). Devueltos a CRLF y comprobado byte a
  byte que el único cambio real es el del andamio. Con `* text=auto` en
  `.gitattributes` no habría dado diff en el blob, pero sí un `git diff` de
  fichero entero para el siguiente agente.
* **Un segundo accidente propio, también corregido**: `sfir.c` **no es UTF-8
  válido en HEAD** — tiene cuatro bytes `0xAB`/`0xBB` (« » de Latin-1) dentro de
  un comentario. Leerlo con `errors='replace'` los convirtió en `U+FFFD` y me
  borró tres líneas de comentario ajenas. Restaurados los cuatro bytes
  originales; comprobado con un barrido que **ningún otro fichero del lote tiene
  mojibake nuevo** respecto a HEAD. Aviso para el resto del árbol: leer fuente
  con `errors='replace'` y volver a escribirla **destruye datos**; hay que usar
  `surrogateescape` o trabajar en bytes.

## 7. Veredicto

**PARCIAL, y la parte que no sale está medida.** 13 de 88 retirados (14,8 %) con
digest ALLOC idéntico sellado en la misma pasada, 2 revertidos, 0 regresiones, y
75 quedan. Los 13 salieron **todos** del barrido mecánico de andamios caducados;
de las formas de fuente que probé a mano —tres para `SYSTEM_addexit`, dos para
`eSolid::GetPostionMarker`, una para `SNDI_validrendermode`— **ninguna cerró un
sitio**, y eso es un dato: en este lote el andamio casi nunca compensa una
sentencia mal escrita, compensa el **orden de reparto** (§4.5) o, en el peor
caso, **código que todavía no está escrito** (§4.4).

Lo que se lleva la próxima ronda:

1. **Rehacer el censo con la expresión corregida** (§4.1): hay 73 andamios que
   nadie está mirando porque se escriben `asm("")`.
2. **Sellar justo antes de promocionar** (§4.2): con agentes en paralelo, un
   «caducado» de hace una hora puede ser falso.
3. **No perseguir «N locales sólo nuestras» en un sitio con pin** (§4.3).
4. Las cuatro funciones del §4.3 con locales **sólo en el original** son el
   único frente de este lote donde queda fuente por escribir, empezando por el
   bloque de `CDActionDrive::Update`.
