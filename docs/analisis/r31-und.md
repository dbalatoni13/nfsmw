# Ronda 31 — `und`: las tres unidades ya ENLAZAN, y ninguna promociona

Encargo: `zSim` (96.400 B), `zFoundation` (36.000 B) y `zMisc` (78.008 B), las
tres con el `.text` al 100 % y cero funciones pendientes, que **fallaban el
enlace por símbolo indefinido**.

**Resultado: las tres enlazan.** Los seis símbolos indefinidos están cerrados con
ediciones de `src/` que **dejo puestas**, sin tocar `configure.py` ni
`config/GOWE69/*`, sin una sola regresión de `matched` y con el **control
`DOL OK`**.

**Ninguna promociona**, y el motivo NO era el que decía el encargo: una vez
resuelto el enlace aparecen **dos bloqueos más, medidos y cuantificados**, que
son el trabajo real. Los detallo en el §3 y §4 con su cifra.

---

## 0. Las cinco frases

1. **El enlace estaba a seis definiciones, y ninguna era del tipo que decía el
   brief.** Los 61 `lbl_803EBxxx` de zFoundation NO los referencia un objeto
   extraído: **los referencia NUESTRO propio objeto**, porque `UMath.cpp`,
   `UVectorMath.cpp` y `USpline.cpp` llevan escritos `extern const float
   lbl_803EB444;` a mano. Mientras la unidad no promocionaba los resolvía el
   objeto extraído; al promocionar, no los define nadie. **No es el caso
   `lbl_` contra `$LC` del §1 del brief: es una declaración sin definición.**
2. **`bin_globala_bun_marker` era un DUPLICADO.** `SunE.cpp` ya define
   `unsigned char lbl_8041F8C3 = 0xf4;` —el nombre exacto del objetivo, y el que
   el zMisc extraído referencia—. `globala_bun.cpp` tenía una segunda copia con
   nombre inventado. Borrada la copia y renombrada la referencia de
   `GameFlow.cpp`: zMisc enlaza. (Mi primer intento renombró la copia en vez de
   borrarla y **rompió la compilación de zPlatform**; queda como veda.)
3. **Definir un `lbl_` en C SIN romper la función se hace poniendo la definición
   al FINAL de la unidad de traducción.** Si el inicializador es visible en el
   punto de uso, GCC pliega la carga a un inmediato: probado, `OnManageTime` cae
   de 100 % a 97,61 % (−564 B). Con `extern const float lbl_80404864 = 1.0f;` al
   final del fichero: **100 % intacto y el enlace resuelto**.
4. **El `.text` de las tres tiene el TAMAÑO exacto y cero funciones ausentes,
   pero el ORDEN está mal**: 169 funciones descolocadas en zFoundation, 208 en
   zSim, 28 en zMisc. En zFoundation la causa se reduce a **cinco** sitios; he
   cerrado **cuatro** (de 9 saltos de delta a 3, y de 18 funciones en su sitio a
   120) sin perder un byte de `matched`.
5. **Lo que queda en las tres es DATO MUERTO que nuestro fuente no produce**, y
   el `keep.lst` no lo arregla: forzando la conservación de TODOS los símbolos de
   datos de nuestro objeto, zFoundation sólo recupera 32 B de los 1.248.

---

## 1. Verificación del encargo: reproduce, pero los errores son otros

`trypromo.py` sobre el árbol de partida, con la lista completa de errores (el
script recorta a dos; el mío del scratchpad los imprime todos):

| unidad | errores | de quién es el objeto que referencia |
|---|---|---|
| `zMisc` | 1 × `bin_globala_bun_marker` | **NUESTRO** `zMisc.o` |
| `zSim` | `lbl_80404864` | **NUESTRO** `zSim.o` |
| | `bSawLoadingScreen` | zFe2.o y zMain.o **extraídos** |
| `zFoundation` | 21 × `lbl_803EBxxx` | **NUESTRO** `zFoundation.o` |
| | `UFoundation_AssertMessage` | zMisc.o **extraído** |
| | `SN_DSI`, `SN_ISI`, `SN_ALIGNMENT` | `metrotrk.o` y `sndvd.o` (LibSN) |

Cinco de los seis grupos son **definiciones que faltan de verdad** (cura 2 del
brief). **Ninguno se arregla cediendo el rango al comodín** (cura 1): el símbolo
que hace falta lo tiene que exportar un objeto que esté en el enlace, y en cuatro
casos quien lo pide es nuestro propio objeto.

---

## 2. Las seis curas, con su verificación

Todas las ediciones **quedan puestas en `src/`**. `configure.py`,
`config/GOWE69/splits.txt`, `symbols.txt`, `keep.lst` y `config.yml` están
**byte a byte como estaban** (comprobado con `cmp` contra la copia de seguridad).

### 2.1 `zMisc` — `bin_globala_bun_marker` → `lbl_8041F8C3` (símbolo ya existente)

`config/GOWE69/symbols.txt` da `lbl_8041F8C3 = .data:0x8041F8C3` (1 B) y el
`zPlatform.o` extraído **lo exporta como GLB OBJ**; el `zMisc.o` extraído lo
referencia como UND. O sea: el nombre correcto ya existía, y además
**`SunE.cpp:57` ya lo define** (`unsigned char lbl_8041F8C3 = 0xf4;`).
`globala_bun.cpp` tenía una **segunda** definición del mismo byte con nombre
inventado.

- `src/Speed/GameCube/Src/globala_bun.cpp`: **borrada** la línea
  `extern "C" unsigned char bin_globala_bun_marker[1] = {0xF4};` (y su comentario
  reescrito para decir de dónde sale el byte).
- `src/Speed/Indep/Src/Misc/GameFlow.cpp`: la declaración pasa a
  `extern unsigned char lbl_8041F8C3;` (escalar, como la definición de SunE), y
  los dos usos a `&lbl_8041F8C3` y `&lbl_8041F8C3 + 1`.

`zMisc` 78.008/78.008 = **100 %**, `zPlatform` 27.792/29.380 = **94,595 %**
(A/B con y sin el cambio: **idéntico**), `zMisc` **ENLACE OK**.

### 2.2 `zSim` — `lbl_80404864` definido al final del fichero

`QuickGame::OnManageTime` carga un `1.0f` **por dirección** (con `register float
initial_speed asm("fr13")` y barrera, deuda ya declarada en el fuente por otro
agente). Valor leído del ELF original: `0x80404864 = 3f800000 = 1.0f`.

- **c1 (REVERTIDA)**: sustituir el `extern` por el literal `1.0f`.
  `OnManageTime` **100 % → 97,6099 %**, −564 B. GCC pliega y deja de emitir la
  carga desde el pool.
- **c2 (PUESTA)**: al FINAL de `QuickGame.cpp`, tras la última función:

      extern const float lbl_80404864;
      extern const float lbl_80404864 = 1.0f;

  Como el inicializador **no es visible en el punto de uso**, GCC sigue emitiendo
  la carga por dirección. `zSim` **96.400/96.400 = 100 %**, `pctsnap --cmp`
  **EMPEORAN: ninguna**.

### 2.3 `zSim` — `bSawLoadingScreen` era un `static` que debía ser global

`symbols.txt`: `bSawLoadingScreen = .data:0x804358AC` (4 B, **scope:global**),
dentro del `.data` de zSim (0x80435814..0x80435970), **pegado a
`Tweak_GameSpeed` (0x804358B4), que está en `QuickGame.cpp`**. Los zFe2.o y
zMain.o extraídos lo tienen UND. Valor en el ELF: **0**.

- `src/Speed/Indep/Src/Sim/Activities/QuickGame.cpp`: `int bSawLoadingScreen = 0;`
  justo delante de `float Tweak_GameSpeed = 1.0f;` (la regla de la r30-da:
  `int X = 0;` va a `.data`, `int X;` a COMMON).
- `src/.../Frontend/MenuScreens/Loading/FELoadingScreen.cpp`:
  `static bool bSawLoadingScreen;` → `extern int bSawLoadingScreen;`.

`zFe2` 248.684/250.732 **sin cambio**, `zSim` 100 %, `zSim` **ENLACE OK**.

### 2.4 `zFoundation` — los 21 `lbl_803EBxxx`, definidos al final de la unidad

`zFoundation.cpp` (la SourceList) recibe **al final**, después de todos los
`#include`, las 20 definiciones con su valor leído del ELF original. Mismo
mecanismo que 2.2: la definición va detrás de todos los usos para que GCC no
pliegue.

| símbolo | valor | | símbolo | valor |
|---|---|---|---|---|
| `lbl_803EB444` | 0.0027777778f | | `lbl_803EB53C` | 0.0f |
| `lbl_803EB448` | 0.0f | | `lbl_803EB540` | 1.0f |
| `lbl_803EB44C` | 1.0f | | `lbl_803EB544` | 0.5f |
| `lbl_803EB450` | 6.2831855f | | `lbl_803EB55C` | −1.0f |
| `lbl_803EB518` | 0.0f | | `lbl_803EB560` | 1.0f |
| `lbl_803EB51C` | 1.0f | | `lbl_803EB564` | 0.0f |
| `lbl_803EB520` | 0.5f | | `lbl_803EB568` | 0.001f |
| `lbl_803EB524` | 0.0f | | `lbl_803EB73C` | −0.0001f |
| `lbl_803EB528` | 1.0f | | `lbl_803EB740` | 0.0001f |
| `lbl_803EB534` | 6.2831855f | | `lbl_803EB744` | 1000.0f |

**Aviso:** el objetivo tiene varios de estos como bloques grandes
(`lbl_803EB450` = 0xA0 B, `lbl_803EB744` = 0xA8 B, `lbl_803EB544` = 0x18 B): sólo
se referencia su PRIMER flotante, así que defino un escalar. El resto del bloque
es dato muerto del §4.

### 2.5 `zFoundation` — `SN_DSI` / `SN_ISI` / `SN_ALIGNMENT` / `SN_FPE`

Es el punto 4 del «lo que NO he probado» de la r30-da, resuelto. El objetivo los
tiene en `.data 0x8041D1C0..0x8041D1D0` **con valor 1**; los macros
`SN_LEAVE_*_FOR_OS()` de `libsn.h` ponen **0**, y nadie en el árbol usa los
macros: **el original los declaraba a pelo con valor 1** (el stub de depuración
SÍ engancha esas excepciones). Los referencian `metrotrk.s` y `sndvd.c`, que
están en el enlace.

`src/Speed/Indep/Libs/Support/Utility/UBezierLite.cpp` (primer fichero de la
lista, que es donde el offset 0 del `.data` los pone):

    extern "C" { long SN_DSI = 1; long SN_ISI = 1; long SN_ALIGNMENT = 1; long SN_FPE = 1; }

**Trampa medida, la misma de la r30-da §2.1**: puestos ANTES de la primera
función, el `_GLOBAL_.I.` pasa a llamarse `_GLOBAL_.I.SN_DSI` en vez de
`_GLOBAL_.I.Evaluate__11UBezierLite…` → **−44 B y −1 función**. Van **detrás** de
`UBezierLite::Evaluate`. `globalini.py` vuelve a dar **46 correctos**.

### 2.6 `zFoundation` — `UFoundation_AssertMessage`

`.data:0x8041D1E0`, valor 0, justo detrás de `gMasterVideoMode` (que ya está
anotado `// .data:0x8041D1DC` en `UFoundationBody.cpp`). Lo asigna `Main.cpp`
(zMisc) y el zMisc extraído lo referencia UND.

`src/Speed/Indep/Libs/Support/Utility/UFoundationBody.cpp`:
`void (*UFoundation_AssertMessage)(const char *, ...) = 0;`

---

## 3. Bloqueo nuevo A: el ORDEN del `.text` — medido y en parte cerrado

Con el enlace resuelto se puede comparar el ELF enlazado contra
`orig/GOWE69/NFSMWRELEASE.ELF` función a función. En las tres:
**el `.text` tiene el TAMAÑO exacto y cero funciones ausentes**, pero el orden no.

| unidad | funciones | descolocadas (antes) | saltos de delta |
|---|---|---|---|
| `zFoundation` | 187 | 169 → **67** | 9 → **3** |
| `zSim` | 400 | 208 | 42 |
| `zMisc` | 449 | 28 | 9 |

Ninguna función de fuera del rango se mueve: **el desorden es interno a la
unidad**, y por eso `objdiff` no lo ve.

### 3.1 zFoundation: cinco causas, cuatro cerradas

Los «saltos de delta» aíslan la causa exacta. Las cinco eran:

| # | causa | efecto | estado |
|---|---|---|---|
| 1 | `BuildRotate` y `OrthoInverse` **en orden inverso** en `UMath.cpp` | +168 / −384 | **CERRADA** |
| 2 | `VU0_Atan2` **delante** de `VU0_MATRIX4setyrot`/`m4toquat`/`Matrix4ToEuler` en `UVectorMath.cpp` | +212 / −1056 | **CERRADA** |
| 3 | `StringPool::StringPool` **delante** de `StringRegistry::StringRegistry` | +216 / +148 | **CERRADA** |
| 4 | `UGroup::Processor::StartGroup/ProcessData/EndGroup` definidos **fuera** de la clase | +20 en 102 funciones | **CERRADA** |
| 5 | `CARP::CarpResolver::StartGroup` definido **fuera** de la clase | +176 en 66 funciones | abierta (§6.1) |

**El mecanismo de la 4 y la 5 es el que vale para toda la ronda:** en GCC 2.9 un
virtual con el cuerpo **DENTRO de la clase** es inline y se emite en
`finish_file`, o sea **al final de la unidad de traducción**; definido fuera se
emite donde está. El objetivo tiene esos cuatro símbolos en las últimas 200 B de
zFoundation (0x8019A100..0x8019A1C4), justo antes del `_GLOBAL_.I.`. Mover los
tres de `UGroup::Processor` al cuerpo de la clase pasó **18 funciones en su sitio
a 120**.

Las cuatro juntas: `zFoundation` sigue **36.000/36.000 = 100 %**, `pctsnap --cmp`
**EMPEORAN: ninguna**.

**Riesgo del cambio de cabecera compartida (`UGroup.hpp`, la incluyen 24
SourceLists), acotado con dos medidas:**
- `grep` en todo el árbol: los ÚNICOS ficheros que nombran `UGroup::Processor`,
  `ProcessBreadthFirst` o `CarpResolver` son `UGroup.hpp/.cpp` y `CARP.h/Carp.cpp`,
  y los dos `.cpp` **sólo los incluye `zFoundation.cpp`**.
- A/B por objetos sobre 9 unidades (zAI, zLua, zWorld, zWorld2, zMisc,
  zMiscSmall, zSim, zEcstasy, zFoundation; 1.024.464 B): `measure.py --cmp`
  **+0 B, +0 funciones, 0 unidades cambian**.

### 3.2 zMisc: 28 funciones, un solo bloque, 9 saltos

Las 421 primeras están en su sitio. Las 28 descolocadas son **el bloque de
inlines diferidos del final de la unidad** (0x8020D504..0x8020DBF0, 1.772 B), y
el `_GLOBAL_.I.BasisMatrixInitDone` que lo cierra **cae en su dirección exacta**:
es una PERMUTACIÓN de nueve grupos, sin bytes de más ni de menos.

Además nuestro objeto emite **once** símbolos extra en ese bloque
(`__8bVector3RC8bVector3`, `SetTime__5Timerf`, `__16DisculatorDriver`,
`Get__16DisculatorDriver`, `GetGiantDataFileName__16DisculatorDriveri`,
`_._11AverageBase`, `Recalculate__11AverageBase`, `GetValue__7Average`,
`GetTotal__7Average`, `GetOldestValue__13AverageWindow`,
`GetOldestTimeValue__13AverageWindow`) que el enlazador descarta: el original los
integra en todos sus llamantes y nosotros no.

### 3.3 zSim: 42 saltos, no es un patrón

Es el peor de los tres. Los saltos no se agrupan en pocas causas: van de +2.584 a
−32.432 y tocan destructores, `_IHandle__`, `ClassKey__`, `_vt`… **No lo he
atacado.**

---

## 4. Bloqueo nuevo B: el dato muerto que nuestro fuente NO produce

Con el enlace resuelto, el DOL de cada promoción (secciones 9 = `.rodata` y
10 = `.data` de la cabecera):

| unidad | `d.rodata` | `d.data` | `d.DOL` |
|---|---|---|---|
| `zFoundation` | **−1.248** | −192 | −1.440 |
| `zSim` | **−1.568** | −224 | −1.792 |
| `zMisc` | **−2.208** | +32 | −2.176 |

**El `.text` no aporta ni un byte de diferencia de tamaño en las tres.**

### 4.1 El `keep.lst` NO lo arregla, y está medido

Construí un `keep.lst` que conserva **todos** los símbolos de `.rodata`, `.data`
y `.bss` de NUESTRO objeto (119 entradas en zFoundation, 474 en zMisc) y volví a
enlazar:

| unidad | sin keep-all | con keep-all |
|---|---|---|
| `zFoundation` | −1.440 B | **−1.408 B** (rodata −1.216, data −192) |
| `zSim` | −1.792 B | **−64 B** (rodata **+160**, data −224) |
| `zMisc` | −2.176 B | **+4.800 B** (rodata **+4.768**, data +32) |

Lectura:

- **zFoundation: los bytes NO ESTÁN.** Conservarlo todo recupera 32 de 1.248. Su
  `.rodata` mide 1.008 B contra los 2.328 del objetivo.
- **zSim y zMisc SÍ tienen los bytes**, y de más: 160 y 4.768 B por encima. Lo
  que les pasa es que `-strip-unused-data` se lleva literales que en el objeto
  extraído sobreviven **pegados a un vecino vivo**. Ejemplo desmontado: la cadena
  `"InitializeEverything"` está en nuestro `zMisc.o` como `$LC502` y **ninguna
  reubicación la apunta**; en el objetivo vive en el offset +0xC de
  `$LC2151632864` (0x803F4FE0, 36 B), símbolo que `InitBigFiles` sí referencia.
  Es dato muerto que dtk salva por adherencia.
- Pero **el tamaño no basta**: con keep-all, el ORDEN de los datos de zSim sigue
  mal —sus 74 vtables se mueven entre −1.056 y +1.744, y sus 27 símbolos de
  `.data` entre +40 y +224—. **zSim no está a 64 B: está a 64 B de tamaño y a un
  reordenamiento completo de contenido.**

### 4.2 Qué es exactamente lo que falta en zFoundation (los 1.312 B)

Volcado bloque a bloque del `.rodata` objetivo (0x803EB230..0x803EBB48) contra el
nuestro. Once bloques no aparecen:

| bloque | B | qué es |
|---|---|---|
| `pad_05_803EB230_rodata` | 92 | `"GAMECUBE"` + `"d:/mw/speed/…"` (cadenas de compilación) |
| `lbl_803EB580` | 368 | `"Undefined"`, `"NTSC"`… |
| `lbl_803EB744` | 168 | `"Attrib::Attr…"` |
| `lbl_803EB450` | 160 | tabla de flotantes (sólo el primero se usa) |
| `lbl_803EB900` | 96 | `"StringStoreBlock…"` |
| `lbl_803EB3F0` | 84 | flotantes |
| `lbl_803EB818` | 72 | `"Null Reference"…` |
| `lbl_803EB28C` | 80 | flotantes |
| `lbl_803EB2F0` | 52 | flotantes |
| `lbl_803EB328` / `lbl_803EB340` | 48 | `"%04x"`, `"UData"`, `"UGroup"` |
| resto (`528`,`534`,`544`,`568`,`6F8`,`728`) | 92 | colas de bloques de flotantes |

**Casi todo son cadenas de diagnóstico que nuestro fuente no tiene.** No se
pueden ceder al comodín: **están INTERCALADAS con nuestros literales vivos** —el
déficit acumulado va 264 B → 272 B → 1.304 B → 1.320 B a lo largo del rango—, y
un objeto `auto_*` sólo se puede colocar antes o después del nuestro, no en medio.
El §1 del brief («ceder el rango al comodín») **no aplica a este caso**.

---

## 5. Verificación

- `build_direct.py` de las unidades tocadas: **7 ok, 0 fallidas**.
- `measure.py` unidad a unidad, estado final:

  | unidad | | | unidad | |
  |---|---|---|---|---|
  | `zFoundation` | 36.000/36.000 **100 %** (188 fn) | | `zFe2` | 248.684/250.732 99,1832 % (1.304 fn) |
  | `zSim` | 96.400/96.400 **100 %** (402 fn) | | `zPlatform` | 27.792/29.380 94,5950 % (135 fn) |
  | `zMisc`+`zMiscSmall` | 78.776/78.776 **100 %** (458 fn) | | `zMain` | 159.776/159.776 **100 %** (1.380 fn) |

  `zPlatform` verificado con **A/B** (con y sin mi cambio: **exactamente el mismo
  número**).
- `pctsnap.py --cmp` en zSim, zFe2 y zFoundation: **EMPEORAN: ninguna**.
- `audit.py` en zFoundation, zSim, zMisc, zFe2, zPlatform, zMain: **0 FALLA**.
- `globalini.py`: **46** unidades con el `_GLOBAL_.I.` correcto (la única mala,
  `gc_interface` de realmemcard, ya lo era antes y no la he tocado).
- `keepchk.py`: **758 ok, 19 RANCIAS, 0 sin objeto** — igual que al empezar.
- `config/GOWE69/{splits.txt,symbols.txt,keep.lst,config.yml}` y `configure.py`:
  **`cmp` idéntico** a la copia de seguridad del scratchpad. **No propongo ningún
  cambio de configuración**: ninguno de los que probé llega a `DOL OK`.
- Los `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- **Enlaces**, con `-strip-unused-data -keep config/GOWE69/keep.lst`:

  | | |
  |---|---|
  | control, sin promocionar nada | **DOL OK** (`9619ba57…`, 4.541.888 B) |
  | `zFoundation` | enlaza, DOL ROTO (−1.440 B) |
  | `zSim` | enlaza, DOL ROTO (−1.792 B) |
  | `zMisc` | enlaza, DOL ROTO (−2.176 B) |
  | las tres juntas | enlaza, DOL ROTO (−5.472 B) |

**No dejo ningún fichero que rompa el enlace**: el control da el DOL del objetivo
byte a byte con todas mis ediciones puestas.

### 5.1 Ficheros de `src/` que dejo modificados

| fichero | +/− | por qué |
|---|---|---|
| `Speed/GameCube/Src/globala_bun.cpp` | +4 −6 | §2.1 |
| `Speed/Indep/Src/Misc/GameFlow.cpp` | +7 −6 | §2.1 |
| `Speed/Indep/Src/Sim/Activities/QuickGame.cpp` | +8 −1 | §2.2, §2.3 |
| `Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.cpp` | +4 −2 | §2.3 |
| `Speed/Indep/SourceLists/zFoundation.cpp` | +48 −0 | §2.4 |
| `Speed/Indep/Libs/Support/Utility/UBezierLite.cpp` | +11 −0 | §2.5 |
| `Speed/Indep/Libs/Support/Utility/UFoundationBody.cpp` | +5 −0 | §2.6 |
| `Speed/Indep/Libs/Support/Utility/UMath.cpp` | +19 −19 | §3.1 causa 1 |
| `Speed/Indep/Libs/Support/Utility/UVectorMath.cpp` | +9 −9 | §3.1 causa 2 |
| `Speed/Indep/Libs/Support/Miscellaneous/StringRegistry.cpp` | +4 −4 | §3.1 causa 3 |
| `Speed/Indep/Libs/Support/Utility/UGroup.hpp` | +12 −3 | §3.1 causa 4 |
| `Speed/Indep/Libs/Support/Utility/UGroup.cpp` | +0 −11 | §3.1 causa 4 |

**Deuda declarada:** el §2.2 y el §2.4 definen **21 símbolos con el nombre
`lbl_<dirección>` que el original les da**, en un sitio de la unidad de
traducción (el final) elegido para que GCC no pliegue el valor, no en el fichero
al que pertenecen. Son datos, no ensamblador, y el valor sale del ELF original;
pero **el nombre y la posición son un constructo nuestro** y hay que sustituirlos
cuando se sepa qué sentencia del original los emitía.

---

## 6. Vedas — lo que probé y NO cierra

### 6.1 `CarpResolver::StartGroup` al cuerpo de la clase: NO COMPILA

Es la quinta causa del orden de zFoundation. Movido a `CARP.h` da cinco errores:
`gResolving`, `gDeltaAddress` y `ResolveData` **están declarados en `Carp.cpp`,
no en la cabecera**. Revertido (`CARP.h` y `Carp.cpp` intactos, `git diff` vacío).
Para cerrarla hay que subir esas tres declaraciones a `CARP.h`, que es un cambio
mayor de cabecera compartida y **no lo he hecho**.

### 6.2 Sustituir `lbl_80404864` por el literal `1.0f`: −564 B

§2.2, ensayo c1. `OnManageTime` 100 % → 97,6099 %. Revertido.

### 6.3 Renombrar `bin_globala_bun_marker` en `globala_bun.cpp`: ROMPE zPlatform

Primer intento del §2.1: renombrar la copia en vez de borrarla choca con la
definición de `SunE.cpp` (`conflicting types for 'unsigned char lbl_8041F8C3'`,
`unsigned char` contra `unsigned char[1]`) y **`zPlatform` deja de compilar**.
Corregido borrando la copia. **Si alguien vuelve a tocar ese byte: el dueño es
`SunE.cpp`.**

### 6.4 `keep.lst` con TODOS los símbolos de datos de nuestro objeto

§4.1. No llega a `DOL OK` en ninguna de las tres y en zMisc **empeora** (+4.800).
Además ese `keep.lst` nombra los seriales `$LCnnn` de GCC, que cambian con
cualquier edición del fuente: **es frágil por construcción**. No lo propongo.

### 6.5 Ceder el rango de `.rodata` al comodín en zFoundation

§4.2. Imposible por construcción: los 1.312 B que faltan están **intercalados**
con nuestros literales vivos a lo largo de todo el rango, y un `auto_*` sólo
puede ir antes o después del objeto, no en medio.

---

## 7. Lo que NO he probado

1. **El orden del `.text` de `zSim` (42 saltos) y de `zMisc` (9 saltos).** El de
   zMisc parece el más barato: son 28 símbolos en un solo bloque de 1.772 B con
   el `_GLOBAL_.I.` final ya en su dirección, o sea una permutación pura de nueve
   grupos de inlines diferidos. La palanca es la misma del §3.1: **qué se define
   dentro de la clase y qué fuera**.
2. **Escribir las cadenas muertas del §4.2.** Ni con `asm(".rodata …")` (que el
   brief permite para datos) ni buscando la sentencia del original que las emite.
   Para zFoundation son once bloques y 1.312 B; la posición dentro del pool la
   decide el punto del fuente donde nazca el literal, así que hay que ubicarlas
   una a una.
3. **El `.data` de zFoundation**, que es el más alcanzable de los tres: 248 B en
   total, **todo ceros salvo los cuatro `SN_*`**, y ya escribí 6 de sus 16
   símbolos. Quedan tres `gap_` (20 B), los tres globales de `CARP` **en otro
   orden que el objetivo**, y `lbl_8041D204` = **180 B de ceros** que habría que
   emitir como un array (y GCC manda un array de ceros a `.bss`, no a `.data`).
4. **El `.data` de zSim** (348 B, 27 símbolos con nombre, todos presentes pero
   descolocados entre +40 y +224). Es reordenar definiciones dentro de la unidad;
   no lo he intentado.
5. **`report.json` / `pctsnap` global.** Sólo he medido las 9 unidades del §3.1 y
   las 6 tocadas. No he pasado `measure.py` a las 33.
6. **La extrapolación a las otras doce SourceLists** que fallan por DOL ROTO. El
   §3 (orden del `.text` interno a la unidad, invisible para `objdiff`) y el §4
   (dato muerto que sobrevive por adherencia en el objeto extraído) **son
   candidatos a explicar también esas doce**, pero **no lo he contado en ninguna
   de ellas** y la ronda 30 ya avisó de tres extrapolaciones falsas. Medirlo es
   barato: los guiones del §8 aceptan cualquier unidad.

---

## 8. Herramientas (scratchpad, prefijo `c31und_`)

| guion | qué hace |
|---|---|
| **`c31und_try.py`** | `trypromo.py` que **imprime TODOS los errores** del enlace (el original recorta a dos y a 90 caracteres, y así no se ve ni cuántos hay ni de qué objeto son). Acepta `--keep <fichero>` |
| **`c31und_keep.py`** / `c31und_keep2.py` | enlazan una promoción dejando el `.elf` y el `.dol` en un fichero fijo, para poder analizarlos |
| **`c31und_doldiff.py`** | tabla de las 18 secciones del DOL de los dos lados + rachas de bytes distintos por sección |
| **`c31und_order.py`** | **la más útil**: compara el ELF enlazado con `NFSMWRELEASE.ELF` y da los **saltos de delta** del orden de `.text` dentro del rango de una unidad. Convierte «169 funciones descolocadas» en «cinco causas con nombre y apellidos» |
| `c31und_elfsym.py` | lector de ELF (símbolos, secciones, bytes) que usan los demás |
| `c31und_syms.py` | símbolos de un objeto con sección, valor, tamaño, binding |
| `c31und_secs.py` | tamaños por sección: objeto extraído contra el nuestro |
| `c31und_keepall.py` | genera el `keep.lst` del §4.1 |
| `c31und_val.py` | valor de una dirección del ELF original como hex / flotante / entero |
