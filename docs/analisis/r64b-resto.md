# r64b -- "resto": el censo de andamios de los ficheros libres y que pasa si se quitan

Encargo: los ficheros libres (los que NO arrastra ninguna de las veinte unidades
en vuelo) con pocos andamios, mas `spchpick.c` y `filesys.cpp`.
Pregunta a contestar: **no** "¿ayuda quitar el andamio?" (eso ya se midio y sale
que no), sino **"¿existe una forma de fuente que lo haga innecesario?"**.

---

## 0. Lo primero: el criterio del encargo NO SIRVE tal cual, y aqui esta la prueba

El encargo dice: *el sha1 del `.o` tiene que quedar identico*. Ese criterio
**hace imposible por construccion cualquier arreglo de ESTRUCTURA**, que es
justo lo que venia a buscar: quitar una local invented cambia `.debug`,
`.debug_pubnames`, `.debug_srcinfo` y `.line`, y por tanto el sha1 del `.o`,
**sin tocar una sola instruccion**.

Medido, `filesys.cpp` (11 barreras y 4 pines menos), seccion a seccion,
HEAD contra ahora:

| seccion | HEAD | ahora | |
|---|---|---|---|
| `.text` | 10872 / `5fd57c16dbb0` | 10872 / `5fd57c16dbb0` | igual |
| `.rodata` | 632 / `74762f2b09dd` | 632 / `74762f2b09dd` | igual |
| `.data` `.bss` `.sdata` `.sbss` `.ctors` `.dtors` | | | iguales |
| `.rela.text` | 6720 / `df1a353e95fa` | 6720 / `df1a353e95fa` | igual |
| `.rela.rodata` `.rela.ctors` `.rela.dtors` | | | iguales |
| `.symtab` | 2352 / `1b80ad2caeb7` | 2352 / `1b80ad2caeb7` | igual |
| `.strtab` `.shstrtab` | | | iguales |
| `.debug` | 53576 | 53540 | **CAMBIA** |
| `.debug_pubnames` `.debug_srcinfo` `.line` `.rela.debug` `.rela.debug_pubnames` | | | **CAMBIAN** |

El DOL sale de `dtk elf2dol` sobre el ELF enlazado, y `elf2dol` solo mira los
segmentos cargables. `.debug*` y `.line` no son SHF_ALLOC: no entran en ningun
PT_LOAD y **no pueden mover el DOL**. `.symtab` tampoco cambia (las locales de
una funcion no son simbolos ELF), asi que `-strip-unused-data` y `keep.lst`
tampoco ven nada distinto.

**Criterio que he usado en su lugar (SELLO ALLOC)**: sha1 del conjunto
{nombre, tamano, contenido} de **todas** las secciones SHF_ALLOC del `.o` **y de
sus `.rela`**. Es estrictamente mas fuerte de lo que el DOL necesita y es lo que
he exigido: **sello ALLOC identico o se revierte**. En el informe doy ademas el
sha1 del `.o` entero de cada fichero, antes y despues, para que se vea exactamente
que se ha movido y que no.

### Y el sello ingenuo tiene un agujero que casi me cuela un cambio malo

Mi primera version del sello tenia una lista fija de secciones con `.text`
dentro. **`zFeOverlay` no tiene `.text`: su codigo vive en `.over`**
(`section_rename` en `build.ninja`). Con el sello ingenuo, tres barreras de
`CarCustomize.cpp` daban "IDENTICO" con `codigo=0`. Con el sello corregido
(todas las SHF_ALLOC):

```
HEAD   zFeOverlay  ALLOC=dca707aed59b7f26fce173080b0461a80519010a  .over=142528
MIA    zFeOverlay  ALLOC=ce9f9bb6c18cd1f1bfdd41b079d88bee7666b3b9  .over=142528
=> DISTINTO
```

Revertido. **Cualquier herramienta de este proyecto que mida `.text` para decidir
si un cambio es seguro esta CIEGA en `zFeOverlay` y en `zOnline`.**

---

## 1. EL CENSO (el resultado principal)

Contado sobre `src/` entero **quitando comentarios primero** (hay decenas de
andamios citados dentro de comentarios de vedas que un `grep` cuenta como reales):

| tipo | forma | total |
|---|---|---|
| BARRERA | `__asm__("" ...)` / `asm volatile("")` | 199 |
| PIN | `register T x asm("rN")` | 290 |
| MWASM | `asm { ... }` de Metrowerks | 76 |
| | **total** | **565** en 116 ficheros |

Reparto por dueño (arrastrado o no por una de las veinte unidades en vuelo):

| | andamios | ficheros |
|---|---|---|
| LIBRES | 424 | ~68 |
| PROHIBIDOS (las 20 unidades) | 141 | 48 |

### 1.1 Los 76 `asm { }` de Metrowerks NO SON ANDAMIOS

Estan **todos** en codigo original del SDK: `dolphinsdk/src/mtx/{mtx,mtx44,vec,quat}.c`
(PSMTX*, PSVEC*, PSQUAT*), `gx/GXTransform.c`, `gx/GXLight.c`, `base/PPCArch.c`,
`include/dolphin/os/OSFastCast.h`, `SN/include/libsn.h`, `LibSN/vmbase.c` y
`LibSN/VMMapping.c`. Son los pares-simples y los accesos a SPR que el SDK trae
escritos en ensamblador. **No hay nada que retirar ahi: son la fuente fiel.**
Eso baja el frente real de 687 (o de mis 565) en 76 de un plumazo.

### 1.2 El veredicto de `regmap`, funcion a funcion

169 funciones con andamio en ficheros libres. Veredicto:

| veredicto | funciones |
|---|---|
| **ESTRUCTURA** | **64** |
| IDENTICO | 13 |
| PERMUTACION | 6 |
| MWASM-SDK (codigo original) | 25 |
| NO-EN-ORIGINAL (la funcion no esta en el volcado DWARF del original) | 28 |
| SIN-FUNCION (cabeceras, plantillas, `asm` a nivel de fichero) | 33 |

### 1.3 Y AQUI ESTA LA TRAMPA: `regmap` es CIRCULAR sobre un pin

De las **64 ESTRUCTURA, 45 lo son SOLO PORQUE EL PIN INVENTA LA LOCAL.**
`register T x asm("rN")` declara una variable que el original no tiene; `regmap`
la ve como "SOLO NUESTRA" y dicta ESTRUCTURA. El andamio **causa** el veredicto
que se usa para justificar atacarlo.

| ESTRUCTURA | funciones |
|---|---|
| por el propio pin (la unica local que sobra ES la del pin) | **45** |
| real (falta o sobra algo que el pin no explica) | **19** |

Las 19 reales: `AV_PLAYER::GetFirstFrame`, `powf`, `log10f`, `GCDriver::ReadFile`,
`PATHI_serviceaction`, `PATHI_serviceevent`, `PATHI_queuenode`,
`FileOperationQueue::Find`, `FILEOPERATION::AddToQueue`, `SYSTEM_addexit`,
`TIMER_init`, `iSNDrandom`, `iSPCH_MakeSampleRequests`, `iSPCH_EACrandom`,
`VU0_m4toquat`, `FEPackage::IssueScriptMessages`, `CustomizeMain::NotificationMessage`,
`EPlayRaceNIS::EPlayRaceNIS`, `QuickGame::OnManageTime`.

**Corolario practico**: cuando `regmap` diga ESTRUCTURA sobre una funcion que
lleva pines, hay que mirar si la lista de "SOLO NUESTRA" es exactamente la lista
de pines. Si lo es, el veredicto no dice nada.

---

## 2. EL BARRIDO: cuantos andamios sobran de verdad

Por cada andamio de ficheros libres con unidad compilable: se borra **esa sola
linea**, se recompila y se compara el sello. Se revierte siempre.

| resultado | andamios |
|---|---|
| HACE-FALTA (el sello se mueve) | 39 |
| SOBRA (el sello no se mueve) | 23 |
| NO-COMPILA en solitario (pin que otras lineas usan) | 62 |

De los 23 "SOBRA", **3 eran falsos positivos** del sello ingenuo (los de
`CarCustomize.cpp`, seccion `.over`). **20 confirmados.**

---

## 3. LO RETIRADO: 24 andamios, con el sello antes y despues

Seis ficheros. En todos, **sello ALLOC identico** (medido compilando la version
de `HEAD` y la mia en la misma sesion); el sha1 del `.o` entero cambia solo por
`.debug*`/`.line` y se da tambien.

| fichero | unidad | retirados | sha1 `.o` HEAD | sha1 `.o` ahora | sello ALLOC (identico) |
|---|---|---|---|---|---|
| `Packages/realmemcard/.../gc/trctasks.cpp` | trctasks | **3 barreras** (33, 72, 1243) -- el fichero queda SIN andamios | `744d2e2a0873115c7c8e83a339ebc7c20dfe297c` | `af23f9e60108f72c893602cae7bc02194d368edd` | `b2f02b79317f690b58e1cba254951ea8dee7dafb` |
| `Libs/realcore/.../file/cmn/filesys.cpp` | filesys | **11 barreras + 4 pines** | `6e29dfc25f3c3bcf2c019fbe6fc15c05a9065b48` | `dcf063c5e7436b38c9a72ddc272509dbd338ec5b` | `0a967d34225217f618288021ae2d6d4062d64813` |
| `Libs/snd/9/.../gc/snddrv.c` | snddrv | **2 barreras** (577, 583) | `8b985bca709887b7ea29b967c0d5b0a497206d84` | `d875e4cc629b7ca601f89ff496f8424b98c78f45` | `032f6e7b0993f9801455283c9d9bfb0968dc72dd` |
| `Libs/path/5.01.04/source/cmn/pathevent.cpp` | pathevent | **1 barrera** (279) | `f809808ea79cd06d6b179f73238e0bb2fc5f0945` | `2a83644060140b8ebc84e49580ba3a93f246a84a` | `8ec3932a80db26bc30071cb3577939f16d9841db` |
| `Src/Sim/Activities/QuickGame.cpp` | zSim | **2 barreras** (405, 462) | `a0c415b1b97c4db9fc5bf8ae3e6e42d6cee9b26f` | `a091c60cf0e315eb608683dcd7260f72d1a70b37` | `aa56aedba1217f8142bde6b1134298d9269f526e` |
| `bWare/Src/Strings.cpp` | zBWare | **1 barrera** (207) | `515e1b7f7e7e5b227b1b98deb834eaba17e42e25` | `decf1b1bd02a4f877546a3ba5dbf9b09cdaa65bf` | `e46500d0ead33d3288aa685587a2c625b407d28a` |

### `filesys.cpp` en detalle (el mayor destrozo de andamios de la ronda)

- `FILE_overhead`: **queda sin andamios**. Fuera las 5 barreras sobre
  `operations`/`handles` y los 2 pines (`register int operations asm("r11")`,
  `register int handles asm("r3")` -> `int`).
- `FILE_init`: fuera 3 barreras (`paths`, `nOps` x2) y el pin de `paths`
  (`register char **paths asm("r11")` -> `char **paths`). Queda el pin de `nOps`
  (**medido**: quitarlo mueve el sello a `a520592622291d7fa230f02962c2fbef3bc51736`).
- `FileOperationQueue::Find`: fuera las 2 barreras **y la local inventada
  `queue` entera** -- ahora la funcion usa `this->` como la escribiria un humano.
- `FILE_nametodevice`: fuera la barrera sobre `Itr.node`.
- Quedan 5 barreras y 1 pin, todos en `FILEOPERATION::AddToQueue` y `FILE_init`,
  todos medidos como imprescindibles.

### Dos locales inventadas retiradas, sin retirar el andamio

Estas dos NO retiran andamio pero **convierten un ESTRUCTURA en un IDENTICO**,
que es exactamente lo que el encargo pedia averiguar:

| fichero | unidad | cambio | sha1 `.o` HEAD | sha1 `.o` ahora | sello ALLOC |
|---|---|---|---|---|---|
| `Libs/snd/9/.../cmn/srandom.c` | srandom | fuera `unsigned int result`; ademas `sum` declarada antes que `carry` para casar el reparto del original (`ax`=r3, `c`=r6) | `42904412ae0a5c6b7f1f825f8b45def11697e531` | `d0a2923ed93bc1d575e91e6ef75809ca03d314cb` | `a9c354e9b960517a6465f980522eb5ab91161f78` |
| `Libs/spch/.../cmn/spchrand.c` | spchrand | fuera `unsigned int result` | `e59dfe9d0a399ed33029175e785ee1b653798126` | `e0aab0d567e0238113a20b1a83074f4905008f4f` | `c22b31b867b963d789ca772dc70aea9eb4977513` |

`regmap srandom iSNDrandom` pasa de *"1 local solo nuestra, 2 con registro
distinto, ESO VA PRIMERO"* a **"MISMO conjunto de locales, mismo arbol de bloques
y MISMO REPARTO"**. La barrera `asm volatile("" : : : "r3")` sigue haciendo falta
(sin ella son 312 B contra 316: se pierde el `mr r3, r0`).

---

## 4. LOS NEGATIVOS (formas probadas que NO reproducen el objeto)

### 4.1 `spchpick.c` / `iSPCH_ChooseSamples` -- el caso mejor instrumentado

`regmap`: ESTRUCTURA. Dos locales solo nuestras (`activeSentence` pinada a r25,
`sampleTable` pinada a r17) y el parametro `sentence` desplazado a r4.
Base del `.o`: `b2e7e34cb9efa7b34cf0fc2beb9372e3294abb51` (estable, 3 medidas).

| forma probada | resultado |
|---|---|
| quitar los dos pines y las dos barreras; cola = `(unsigned char *)bank + 0xC + secondChoice * sampleSize` | `.text` 7832 (**-4 B**), 97,95 %. GCC pliega `0xC+2` en `0xE` y pierde el `addi r17` |
| `sampleTable` local sin pin declarada ANTES de `sampleData` (`sampleData = sampleTable + 2`) | 7836 B, 95,48 %: el `addi r17` sale el PRIMERO y en el objetivo sale el QUINTO |
| lo mismo con `sampleTable` declarada DESPUES de `sampleSize` | **byte a byte igual que la anterior**: el orden de sentencias NO mueve nada aqui |
| `inline unsigned char *BANKHDR_GetSampleTable(hdr) { return (unsigned char *)(hdr + 1); }` usada dos veces (para que CSE saque el temporal) | 7832 B: se pliega entero, no hay CSE |
| sin `activeSentence`, `sampleTable` sin pin, las dos barreras puestas | 99,42 %: la planificacion casa; queda una permutacion de cuatro registros |
| sin `activeSentence`, `sampleTable` **pinada a r17**, las dos barreras | 99,66 %: solo queda el ciclo `sentence` r25<->`sampleSize` r24 |
| lo anterior + `register int sampleSize asm("r24")` (pin sobre una local REAL del original) | 99,71 %: `sentence` ya cae en r25, pero `numParms` se va de r9 a r24 |

Todo revertido; `git status` de `spchpick.c` limpio y el `.o` vuelve a
`b2e7e34cb9efa7b34cf0fc2beb9372e3294abb51`.

**Lo que si queda demostrado**: el pin `activeSentence asm("r25")` NO hace falta
para colocar `sentence` en r25 -- basta con pinar `sampleSize` a r24, que es una
local que el original SI tiene y SI esta en r24. La deuda de fuente de esa
funcion es una sola: el temporal `bank + 0xC` que el original mantiene vivo en
r17 sin declararlo (no sale en el DWARF).

### 4.2 `srender.c` / `SNDI_validrendermode`

La barrera `__asm__("")` dentro del `continue` existe para que jump.c no funda el
bloque vacio. Probado: **`enum { X = 0 };` dentro del bloque** (la palanca de
`nfsmw-etiqueta-bloque-vacio.md`) -> 184 B, exactamente lo mismo que quitar la
barrera. **El `enum` no bloquea a jump.c**; solo fuerza el `lexical_block` del
DWARF. Es un negativo util: esa palanca no vale para bloques vacios de salto.

### 4.3 Pines que se han probado a desclavar y hacen falta (sello medido)

| funcion | pin | sello con el pin quitado |
|---|---|---|
| `FILE_init` | `nOps asm("r9")` | `a520592622291d7fa230f02962c2fbef3bc51736` |
| `SNDDRV_audiocallback` | los dos `mixSource asm("r4")` | `d2d15fe987b98c9d2d4dca087ee8724e886394b9` |
| `SNDSTRMI_parsedata` | `intTemp1 asm("r10")` | `4e5350753324af8ea8e1afd101ceefff9fdbff49` |
| `QuickGame::OnManageTime` | `initial_speed asm("fr13")` | `092910b815ed30e47bfbde4507bea0e19f3777b9` |
| `QuickGame::OnManageTime` | `release_speed asm("fr0")` | `e30bf0ecc44d83f9bfd0a6305e4421cd2d22e986` |
| `bStrNICmp` | la local `c1wide asm("r0")` (fundida en `return c1 - c2`) | `81abc46b2aa6ece05e3e0cad20970a34e23d3713` |
| `FileOperationQueue::Find` | -- (`queue` SI se pudo quitar) | -- |
| `PATHI_serviceevent` | la local `actionOffset` (metida en la expresion) | `19d3b9358f238a6f7ae878fc1d13cdbf3960e04f` |

### 4.4 Barreras individuales medidas como imprescindibles (39)

`gc_driver.cpp` 325 y 583; `criticalpath.c` 779; `csis.cpp` 352 y 376;
`pathaction.cpp` 332 y 386; `pathbank.cpp` 191; `pathevent.cpp` 260;
`pathnode.cpp` 624 y 649; `pathserv.cpp` 29; `pathsnd.cpp` 26, 89, 518 y 795;
`filesys.cpp` 724, 726, 728, 802 y 807; `inittmr.cpp` 131; `srender.c` 28;
`spchsamp.c` 80 y 83; `avplayer.cpp` 597; `sfsplit.c` 42; `syscalls.c` 32;
`itoa.c` 78 y 81; `memset.c` 24; `strstr.c` 15; `FastMem.cpp` 77;
`AnimEntity_WorldEntity.cpp` 169; `FEPackage.cpp` 235; `FEngine.cpp` 327 y 465;
`EPlayRaceNIS.cpp` 76; `InputDeviceGC.cpp` 507; `Geometry.cpp` 418;
`QuickGame.cpp` 286 y 424; `Strings.cpp` 209; `bList.cpp` 45.

### 4.5 No tocados por regla propia

- `snd/9/source/library/mix/sfir.c` **no es UTF-8** (byte 0xAB): la regla 4 del
  encargo prohibe dejarlo escrito por una herramienta que lo reescriba en UTF-8.
  Sus 2 barreras quedan sin medir.
- `LibSN/sndvd.c`, `libc/ef_pow.c`, `libc/sf_log10.c`,
  `Libs/Support/Utility/UCollections.h` y `UListable.h` estaban **modificados por
  otros agentes** al empezar la ronda: no los he tocado.

---

## 5. VEREDICTO: ¿funciona este frente?

**PARCIAL, y con un techo mucho mas bajo de lo que la cifra de 687 sugiere.**

Con cifras, sobre los **424 andamios de ficheros libres**:

1. **76 (18 %) no son andamios**: son los `asm { }` originales del SDK de
   GameCube y de LibSN. Restarlos del frente es gratis y hay que hacerlo ya.
2. **24 andamios retirados** en esta ronda con sello ALLOC identico, sobre 124
   probados uno a uno: **19 % de exito**. Concentrados: 15 de los 24 estan en un
   solo fichero (`filesys.cpp`), y otros 3 en `trctasks.cpp`, que queda limpio.
3. **39 barreras y 62 pines medidos como imprescindibles**: no se caen solos.
4. El frente "una forma de fuente los sustituye" **existe pero es estrecho**: de
   las siete formas que probe en `iSPCH_ChooseSamples` (el caso mejor
   diagnosticado del lote, ESTRUCTURA de libro), **ninguna** reprodujo el objeto,
   aunque dos redujeron el problema de cuatro andamios a uno.
5. Lo que **si** funciona de forma reproducible es mas modesto y mas barato:
   **quitar la local inventada que el pin arrastra** (medido en `srandom` y
   `spchrand`: ESTRUCTURA -> IDENTICO sin mover un byte de codigo) y
   **quitar barreras redundantes que quedaron de rondas anteriores**
   (20 de 23 candidatas confirmadas).

Y dos avisos que valen mas que la cifra:

- **`regmap` no puede arbitrar este frente sobre funciones con pin**: 45 de sus
  64 ESTRUCTURA son el propio pin. Hay que filtrar antes de repartir trabajo.
- **cualquier medida de seguridad basada en `.text` esta ciega en `zFeOverlay` y
  `zOnline`** (`.over`). Me colo tres barreras como "identicas" y no lo eran.

---

## Apendice: como reproducir

- Censo: quitar comentarios y buscar `__asm__("" ...)`, `register T x asm("rN")`
  y `asm {`. Un `grep` crudo cuenta unas 120 apariciones de mas, todas dentro de
  comentarios de vedas.
- Sello: sha1 de {nombre, tamano, contenido} de toda seccion con SHF_ALLOC y de
  sus `.rela`. **No** una lista fija con `.text` dentro.
- Comparacion honesta: compilar `git show HEAD:<fichero>` y la version actual en
  la misma sesion y enfrentar los dos sellos. Es lo unico que descarta que otro
  agente haya movido el `.o` por debajo.
