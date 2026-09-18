# r62 -- agente `ecs` (unidad zEcstasy)

## Resumen

| | al empezar la ronda | al acabar |
|---|---:|---:|
| **BYTES DISTINTOS DEL DOL** | **19.961** | **7.525** (**-12.436**) |
| `text1` | 13.882 | **1.814** |
| `text0` / `text2` / `data4` / `data5` | 7 / 301 / 8 / 1 | **0 / 0 / 0 / 0** |
| `data2` (`.rodata`) | 5.698 | 5.697 |
| `data3` (`.data`) | 64 | 14 |
| `linkdelta zEcstasy` | `.text +0  bss-96` | `.text +0` **IGUAL** |
| `desplaza` `.bss` DENTRO mal | **109 de 153** | **0 de 154** |
| `fncmp` | 3 de 539, 4.384 B | 3 de 539, 4.384 B (sin regresion) |
| `sha1` del `.o` | `0a4d26eb82dd3decbce2e0c12c864dfb2eef0ce7` | **`4135718963475f9c38efd15707b35b5940d8380e`**, tres compilaciones iguales |

Referencia: la r60 dejo el DOL en **8.207 B** y la r61 no toco datos. La raya de
`splits.txt` que aplico el cierre de la r61 (paquete P1 de `r61c-aranges.md`) le
dio a `zEcstasy` **112 B de cabeza de `.bss` que no emitiamos**, y con ellos el
DOL subio a **19.961**. Esta ronda cierra ese agujero y ademas deja la `.bss`
**exacta simbolo a simbolo**.

**NO promociona**: siguen las tres funciones con codigo distinto (4.384 B) y
`doldiff`/`trypromo` dan `DOL ROTO`. No he tocado ninguna de las tres.

---

## 1. La cabeza de `.bss`: 112 B, y el `.balign` que casi los tira

`splits.txt` da hoy `.bss start:0x8045EC40` para `zEcstasy`, y `.debug_aranges` lo
confirma (`aranges --list zEcstasy` da `.bss start:0x8045EC40`). El `.o` extraido
abre con `pad_07_8045EC40_bss` de **112 B**, que `keep.lst:3495` protege.
Nosotros no emitiamos nada ahi: nuestro primer simbolo de `.bss` en el enlace era
`mhL2V` en `0x8045EC40` y **toda la `.bss` de la unidad salia 112 B arriba**.

Nuestros tres estaticos de esa zona (`value`, `_.tmp_0` y `_vfilter`, 36 B) **no
cuentan**: los tres cuelgan de `eUpdateCopyFilter2`, que **el objetivo ni siquiera
tiene** (en el `.o` extraido no hay simbolo entre `eSetCopyFilter` y
`eDrawStartup`), asi que el enlazador se los lleva enteros.

La cura es un `asm()` de fichero delante del primer `#include`, con el nombre que
`keep.lst` ya protege: `.section .bss`, `.balign 4`, `.globl
pad_07_8045EC40_bss`, la etiqueta, `.space 0x70` y `.section ".text"`.

**TRAMPA MEDIDA, y es generica**: con `.balign 8` en vez de `.balign 4` la
alineacion de la SECCION `.bss` del objeto pasa de 4 a 8, y entonces
`-strip-unused-data` deja de llevarse enteros los simbolos muertos de 4 B
(`size & ~7` en vez de `size & ~3`). Con `.balign 8`: `bss+64` y **DOL 16.830**.
Con `.balign 4`: `IGUAL` y **DOL 8.175**. Un solo digito.

---

## 2. LA PRIMERA PARTICION DE `finish_file`, ABIERTA (veto de la r59 roto)

La r59 (`r59-ecs.md` seccion 5) dejo dos huecos de `.bss` sin colocar y escribio:

> **La palanca obvia --declarar el hueco como un tipo con constructor-- esta VETADA
> por medida indirecta**: emitiria una llamada mas en
> `__static_initialization_and_destruction_0`.

**Es falso, y el control cabe en nueve lineas.** Eso solo pasa con **ARRAYS**.
Un **objeto suelto** de un tipo con constructor vacio cae en la primera particion
y **no anade ni una instruccion**:

```c
struct Ctor { float a[2]; Ctor(){} };
Ctor cA[2];
struct One  { char b[8]; One(){} };
One gOne;              /* <-- objeto suelto */
char gPOD[8];
Ctor cB[2];
int f1(){ static int sA[2]; return sA[0]++; }
```
```
.lcomm sA.9,8,4                                     <- estatico local, punto de parseo
.lcomm cA,16,4   .lcomm gOne,8,1   .lcomm cB,16,4   <- PRIMERA particion
__static_initialization_and_destruction_0: ...      <- DOS bucles, no tres
.lcomm gPOD,8,4                                     <- SEGUNDA particion
```

El `diff` del `.s` con `gOne` y sin el son **dos lineas**: `.globl gOne` y
`.lcomm gOne,8,1`. `__static_initialization_and_destruction_0` sale identico.

En el arbol: `struct _r62_pad8 { int b[2]; _r62_pad8(){} };` y `_r62_pad16`
(definidos en `zEcstasy.cpp` delante de los `#include`), y con ellos
`gap_07_80461F54_bss` y `gap_07_80466F98_bss` caen por fin en su sitio.

### Y de paso: `gap_07_80467048_bss` NO estaba

La r59 anoto en `eLightE.cpp:36` que esos 16 B "ya los pone `eLspec`, que el
objetivo no tiene y el enlazador no estripa". **Refutado con la medida**: nadie
referencia `eLspec`, mide 48 B (multiplo de 8) y el enlazador se lo lleva ENTERO
--en el enlace no existe--, asi que donde el objetivo tiene 16 B nosotros
teniamos cero. Es un andamio caducado (`memory/nfsmw-andamios-caducan`). Ahora se
emite aparte, con `_r62_pad16`.

---

## 3. EL HALLAZGO: el sitio de un global de `.bss` lo fija su PRIMERA DECLARACION

No su definicion. Control con `ngccc -S`:

```c
extern int A[4];   int B[4];   int A[4];   int C[4];
```
```
.lcomm A,16,4    .lcomm B,16,4    .lcomm C,16,4      <- A PRIMERO
```

Y el reciproco, que es la otra mitad de la palanca: **un `extern` de AMBITO DE
BLOQUE no fija nada**; el simbolo cae donde este su definicion.

```c
int B[4];   int g(){ extern int A[4]; return A[0]; }   int A[4];   int C[4];
```
```
.lcomm B,16,4    .lcomm A,16,4    .lcomm C,16,4      <- A DONDE SE DEFINE
```

Las dos palancas, aplicadas sin mover ni una definicion:

| simbolo | estaba a | palanca | queda a |
|---|---:|---|---:|
| `SunTextures` | +5.056 B | `extern TextureInfo *SunTextures[5];` en `Ecstasy.cpp` | 0 |
| `pContrastRampTextureInfo` | -27.068 B | el `extern` de fichero de `EcstasyE.cpp:102` baja a ambito de bloque | 0 |
| `eLightFlareTextureInfos`, `PoolOfFlaresXcludeView` | +204 / -8 | dos `extern` de fichero delante de `intensity` (`eLight.cpp`) | 0 |
| `pt` | +588 B | `extern float pt[2];` delante de `tT` (`EcstasyEx.cpp`) | 0 |

**Esto vale para todo el arbol**, y explica de un plumazo la clase de "simbolo
descolocado" que `movidos.py` y `desplaza.py` llevan rondas senalando sin causa:
si una CABECERA declara `extern X`, el sitio de `X` en `.bss` o `.data` lo manda
la cabecera, no el `.cpp` que lo define.

### El caso que lo prueba, y que ademas no obliga a tocar la cabecera

`LoadedSolidStats` salia **2.784 B por delante** porque `Ecstasy.hpp:508` trae
`extern eLoadedSolidStats LoadedSolidStats;` y esa cabecera se lee en el primer
`#include`. La cabecera es COMPARTIDA (regla 5). La salida es un **alias**:

```c
eLoadedSolidStats _r62_LoadedSolidStats asm("LoadedSolidStats");   // eSolid.cpp
```

La declaracion de la cabecera se queda sin definicion (no emite nada) y el
simbolo sale donde esta el alias. Control comprobado con `ngccc -S`: las
referencias siguen resolviendo a `LoadedSolidStats@ha` / `@l`. Es el patron de
`char _zBWareBssTail[12] asm("gap_07_8045A968_bss");`, usado ahora para **mover
un simbolo declarado en una cabecera ajena sin tocarla**.

---

## 4. Tres cosas mas, chicas y medidas

* **`-strip-unused-data` deja `size & 3` de cada simbolo muerto de `.bss`.**
  `volatile Bool bHangRecovery;` (1 B) y `volatile unsigned short load_sync;`
  (2 B) no se usan en NINGUN sitio del arbol (grep entero: solo su propia
  declaracion) y le costaban **4 B CADA UNA** al enlace (el resto mas su relleno
  de alineacion). Retiradas, comentadas en su sitio. Eran la mitad del `bss+32`.
* **Un `char[N]` suelto se lo lleva `DATA_ALIGNMENT` a align 4** (`.lcomm
  arr21,21,4`), pero **el mismo array dentro de un struct POD sale con align 1**
  (`.lcomm s21,21,1`); `__attribute__((aligned(1)))` **no** hace nada en GCC 2.9.
  Hacia falta para `gap_07_8046D14B_bss`, que el objetivo tiene en una direccion
  impar: `struct _r62_raw21 { char b[21]; };`.
* **Orden dentro de la particion POD**: los tres `numCopsActive*` y los tres de
  `eLight.cpp` iban al reves de como estan escritos; el `.bss` de `EcstasyE.cpp`
  pedia ademas subir `g_ScreenPositionMatrix`, `Player1SpecularProjection`,
  `Player2SpecularProjection` y `RenderTargets` delante de `_rmode`, y bajar los
  cuatro `scis_*` delante de `g_InitPad`.

---

## 5. LO QUE QUEDA: la `.rodata`, con la ARITMETICA CERRADA

**5.697 B, el 76 % de lo que falta.** El diagnostico de la r60 esta recontado y
ahora **cuadra al byte**, que es lo que le faltaba para poder ejecutarse:

* los **584 `$LC` que sobreviven** al enlace suman **4.419 B**, y los 584 se
  mapean a su direccion del objetivo: 552 leyendo la reubicacion de la MISMA
  instruccion en los dos `.o` (539 funciones emparejadas, **0 conflictos**) y los
  otros 32 **por CONTENIDO**, buscando sus bytes en el intervalo que dejan sus dos
  anclas;
* los **6 simbolos reales de la fuente** (las cuatro vtables `c*Map`, `PADMASKS`,
  `TweakSphereMapClr`) ponen **116 B**;
* luego el bloque `asm` tiene que aportar **EXACTAMENTE 3.417 B en 128 huecos**...
* ...y **3.417 es EXACTAMENTE lo que aporta hoy**: 397 B de los 23 `lbl_` de
  `keep.lst` mas 3.020 B de restos `size & 7` de los `lbl_` estripados.

**La cuenta cierra**, o sea que **no falta ni sobra contenido**: esta todo en el
sitio de menos. Los huecos mayores:

```
0x803DEE08 +908   0x803DE944 +412   0x803DDBA4 +256   0x803DE025 +199
0x803DD9BC +168   0x803DD658 +128   0x803DDF9A +118   0x803DE4AC +84
0x803DD8AA +82    0x803DDD74 +76    0x803DD824 +64    0x803DDE75 +63
```

**Plan** (el de la r60, ahora con los numeros): partir el bloque en 128 fragmentos
de simbolos de **<=7 B** --que `-strip-unused-data` no estripa, hallazgo de la
r58-- y colocar cada uno **entre las dos funciones cuyos `$LC` lo rodean**.
Sobre la cuenta vieja de 111 huecos: **56 caian entre funciones DISTINTAS
(3.971 de 4.105 B)** y **13 entre FICHEROS distintos (2.578 B)**, o sea que la
mayor parte se coloca sin salir de `zEcstasy.cpp`. Un fragmento colocado "pronto"
solo desordena SU vecindad, asi que el error esta acotado y el trabajo se puede
medir por trozos.

**Lo que NO se puede hacer es a medias**: mover un hueco al sitio bueno sin quitar
del bloque exactamente los mismos bytes supervivientes cambia el tamano de
`.rodata` y descoloca el DOL entero (`linkdelta` deja de dar `IGUAL`). Por eso no
se ha tocado esta ronda: o se regenera el bloque entero, o no se toca.

### `.text`: 1.814 B

Unos 660 B son las tres funciones (`epCalculate` 394, `UpdatePlatInfo` 55,
`eProject` 35, y el resto reubicaciones). Unos 300 B son el **ciclo de 6 de la
cola de `finish_file`** que `permorden` sigue senalando
(`Flush__14eTextureBucket` 105, los dos `_type_map` 97+96). Leido el bucle
(`cp/decl2.c:3688`), cada vuelta drena **en este orden**:
`instantiate_pending_templates`, vtables, `static_aggregates` + ssdf,
`synthesize_method`, `wrapup_global_declarations(saved_inlines)`,
`wrapup_globals_for_namespace`. El objetivo saca los dos `_type_map` en la
**SEGUNDA** vuelta --o sea que nadie los pide hasta que la primera emite
`ClassKey`-- y nosotros en la **PRIMERA**. **No lo he tocado**: no tengo medida de
que forma de fuente retrasa la instanciacion.

---

## 6. Un negativo, y una mina desactivada

**NEGATIVO MEDIDO** (anotado tambien junto al `asm` en `zEcstasy.cpp`, donde
`previo.py` lo encuentra): `.balign 8` en el `asm` de la cabeza de `.bss` da
`bss+64` y **DOL 16.830**, contra `IGUAL` y **8.175** con `.balign 4`. No es
cosmetico: cambia la alineacion de la SECCION y con ella el `size & ~(align-1)`
que `-strip-unused-data` aplica a TODOS los simbolos muertos de la unidad.

**MINA**: `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp` y
`src/Speed/GameCube/Src/Ecstasy/eMathE.cpp` traian **seis bytes latin-1 cada uno**
(`0xAB` y `0xBB`, las comillas angulares) en comentarios de la r61 --exactamente
la trampa del brief, la que mata `configure.py` con `UnicodeDecodeError` justo
cuando hace falta promocionar--. `eLightE.cpp` **ni siquiera era UTF-8 valido**.
Sustituidos por comillas rectas; los dos ficheros son de esta unidad y el `.o` no
cambia.

**Barrido del arbol entero**: quedan **seis** ficheros de `src/` que no son UTF-8
valido, y **NO son mios**:

```
src/Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.cpp              5 bytes>127
src/Speed/Indep/Libs/snd/9/source/library/mix/sfir.c                4
src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c         2
src/Speed/GameCube/bWare/GameCube/SN/include/va-ppc.h               1
src/Packages/xenonsdk/2.0.2135.2/installed/include/xbox/xbox.h      2
src/Packages/xenonsdk/2.0.2135.2/installed/include/xbox/d3d9gpu.h   1
```

---

## 7. Administrativo

* **Ficheros tocados (los ocho son de zEcstasy en exclusiva; comprobado uno a uno
  contra `src/Speed/Indep/SourceLists/*`)**:
  `src/Speed/Indep/SourceLists/zEcstasy.cpp`,
  `src/Speed/Indep/Src/Ecstasy/Ecstasy.cpp`,
  `src/Speed/Indep/Src/Ecstasy/eSolid.cpp`,
  `src/Speed/Indep/Src/Ecstasy/eLight.cpp`,
  `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp`,
  `src/Speed/GameCube/Src/Ecstasy/EcstasyEx.cpp`,
  `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp` (solo bytes latin-1),
  `src/Speed/GameCube/Src/Ecstasy/eMathE.cpp` (solo bytes latin-1).
  **Ninguna cabecera. Ningun `config/`. Ningun `splits.txt`. Ningun commit.**
* **`lcfix.py --check`: 1 correccion pendiente, y NO es mia**
  (`zEagl4Anim.o:$LC109 -> $LC124`). La `.rodata` de esta unidad no se ha movido
  un byte, asi que **no dejo ninguna entrada venenosa** para nadie.
* **Propuestas para `keep.lst` / `splits.txt` / `symbols.txt` / `configure.py`:
  NINGUNA.** La entrada `zEcstasy.o:pad_07_8045EC40_bss` que el cierre de la r61
  anadio a `keep.lst` **ya vale, y ahora tiene contenido nuestro que proteger**.
* **Regresiones: cero.** `fncmp` 3 de 539 / 4.384 B antes y despues; `desplaza`
  DETRAS = 0 en las cuatro secciones; `linkdelta` `IGUAL`.
* `.o` sellado con **tres** compilaciones:
  `4135718963475f9c38efd15707b35b5940d8380e`.
* Scratchpad `scratchpad/ecs62/`: volcados (`.elf`, copias de `.o`) borrados;
  quedan solo los guiones de medida.
