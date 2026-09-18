# r63-orden-plat -- zPlatform, zFe y zGameplay

**Titular.** `zPlatform` pasa de **134 funciones descolocadas de 137 a CERO**
(`>>> ORDEN PERFECTO`, 0 saltos de delta) y su `.data` pasa a ser **identica
simbolo a simbolo a la del objetivo**. Medido sobre el mismo arbol, con el
enlace completo: `dolwhere zPlatform` **95.754 B -> 6.699 B**, o sea
**-89.055 B de DOL**. No promociona: **lo unico que queda es el ORDEN de la
`.rodata`**, y la seccion 4 lo deja con nombre, cifra y receta.

`zFe` y `zGameplay` **no se han tocado**: la seccion 5 dice por que --su desorden
NO es del mismo tipo que el de zPlatform-- y deja el censo exacto de lo que hay
que mover en cada una.

Cero ediciones en `configure.py`, `config/GOWE69/*`, `splits.txt` o `symbols.txt`.
**Cero correcciones de `lcfix` pendientes en zPlatform** (`lcfix --check zPlatform`
= al dia). Cero re-extracciones. Ningun fichero compartido tocado: los nueve
ficheros editados son **exclusivos de zPlatform** (comprobado con `grep -rl`
sobre todas las SourceLists).

---

## 0. Estado final de zPlatform, con sellos

```
build_direct  x3            -> sha1 d3eadbd1264b507370fb9af726a887189bb43637 (los tres)
   (sin los bloques de comentario daba 910e66fcdfbf7bb8c735d54db9e2b27ca0f3576f:
    una linea de comentario mueve .debug_line, el .text es identico)
fncmp     zPlatform         -> 0 de 136 funciones con el CODIGO distinto
textorder zPlatform         -> DESCOLOCADAS 0 de 137, SALTOS DE DELTA 0, >>> ORDEN PERFECTO
LIS "a mover"               -> 0   (era 48 en la r61)
linkdelta zPlatform         -> .text +0   resto IGUAL
dolwhere  zPlatform         -> 6.699 B distintos   (base del mismo arbol: 95.754 B)
lcfix --check zPlatform     -> todas las entradas @lc estan al dia
keepchk2                    -> ninguna entrada de zPlatform sin simbolo
mangfix zPlatform --check   -> sin alias desfasados
checksplits / gapchk        -> LIMPIO / total 0
trypromo  zPlatform         -> DOL ROTO   (sigue: ver seccion 4)
```

Sello de la BASE, medido de vuelta (revirtiendo mis nueve ficheros con
`git checkout --` y recompilando): `5cdb764ca70d5c2db32d471add8de3172b0fc6b2`,
`134 de 137 descolocadas`, `dolwhere 95.754 B`. Las dos cifras de la comparacion
salen del **mismo arbol y el mismo dia**, no del informe de la r61.

---

## 1. El `.text`: seis unidades de traduccion reordenadas, cero instrucciones movidas

La r61 dejo escrito que el desorden de zPlatform era "intra-fichero" y "por punto
de uso". **La primera mitad es cierta; la segunda es falsa para 84 de los 85
saltos.** El orden emitido de zPlatform es, casi entero, **el orden en que estan
escritas las funciones dentro de cada `.cpp`** --la palanca 2 del docstring de
`textorder.py`--, y basta con moverlas de sitio. Se comprueba en un segundo: el
orden emitido de `Platform_G.cpp` en la base era, literalmente, el de sus lineas
51, 55, 104, 143, 146, 148, 154, 166, 183, 201, 213, 330.

Escalones medidos (`.text` clavado en 35.044 B en TODOS ellos, y `fncmp` en
`0 de 136` en todos ellos: no se ha movido una sola instruccion):

| paso | fichero | descolocadas | saltos |
|---|---|---:|---:|
| base | -- | 134 de 137 | 85 |
| 1 | `SunE.cpp`, `AcidFX_G.cpp`, `JoyE.cpp`, `Movie_GC.cpp` | 134 de 137 | 71 |
| 2 | `Platform_G.cpp` | 126 de 137 | 62 |
| 3 | `VMStatsManager::Init` -> bloque diferido | 70 de 137 | 60 |
| 4 | `TextureInfoPlat.cpp` | 54 de 137 | 44 |
| 5 | `LGWheels.cpp` | 20 de 137 | 10 |
| 6 | `xSparks.cpp` + `~CGEmitter` | **0 de 137** | **0** |

Los cinco reordenamientos de `.cpp` son mecanicos. Los dos interesantes son los
otros dos, y los dos son la misma tecnica con dos mandos distintos.

### 1.1 `VMStatsManager::Init`: `inline` + alias `__asm__` (paso 3)

El objetivo emite `Init__14VMStatsManagerPCc` **detras de
`__static_initialization_and_destruction_0` y de `Init__7VMStats`**, o sea en el
bloque DIFERIDO. Nosotros la teniamos como definicion normal en `Platform_G.cpp`
y salia 128 B antes de `DVDErrorTask`, empujando toda la unidad.

**Negativo medido, y es el que hay que conocer**: marcarla `inline` a secas la
manda al bloque diferido... **y la borra**. GCC 2.9 la pliega en las tres
llamadas del ctor de `VMStatsGlobalsInit` y no emite copia fuera de linea:

```
inline void VMStatsManager::Init(...)  ->  __static_init  612/340 B  (era 340/340)
                                           Init__14VMStatsManagerPCc  AUSENTE (0/128)
```

**Lo que si funciona** es lo que este mismo fichero ya hacia con `VMStats::Init`:
dejarla `inline` y llamarla por un **alias `__asm__` sin definicion visible**, que
el frente no puede plegar:

```c
inline void VMStatsManager::Init(const char *name) { ... }
void VMStatsManager_InitReal(VMStatsManager *self, const char *name)
    __asm__("Init__14VMStatsManagerPCc");
...
    VMStatsManager_InitReal(fe, name);   // en vez de fe->Init(name)
```

Resultado: la copia COMDAT se emite (137 funciones otra vez), `.text` vuelve a
35.044 B, `fncmp` sigue en `0 de 136` y la funcion cae **exactamente** donde el
objetivo la tiene. Es la receta de `memory/nfsmw-decl-comdat-inline`, con el
matiz que faltaba: **`inline` a secas no basta cuando la funcion tiene llamantes
en la misma unidad; hay que romper ademas el punto de uso.**

### 1.2 `~CGEmitter`: `inline` DEFINIDO DESPUES de su unico llamante (paso 6)

Mismo problema, otra solucion, y esta es nueva. El objetivo emite `_._9CGEmitter`
(108 B) en el bloque diferido, **la ultima funcion de la unidad**. Nuestro
`CGEmitter::~CGEmitter() {}` estaba definido fuera de clase junto al constructor
y salia ahi.

- Meterlo DENTRO de la clase: se pliega en `NGEffect::NGEffect`
  (`__8NGEffect` 292/280 B) y **no se emite** (`_._9CGEmitter` AUSENTE). NEGATIVO.
- Dejarlo fuera de clase y marcarlo `inline` **al final del fichero, detras de
  `NGEffect::NGEffect`**: cuando el frente compila `NGEffect::NGEffect` solo tiene
  la DECLARACION, asi que emite el `bl` (`__8NGEffect` se queda en 280 B) y en
  `finish_file` saca el cuerpo en la cola diferida. **POSITIVO**, y con el la
  unidad cierra el orden.

La regla general que sale de aqui, y sirve para cualquier unidad:
**`inline` decide la COLA en la que sale la funcion; la POSICION DE LA DEFINICION
respecto del llamante decide si se pliega.** Con los dos mandos por separado se
coloca una funcion en el bloque diferido sin cambiar una instruccion.

### 1.3 El orden que hay que copiar

El del objetivo en `LGWheels.cpp` no es "por familias" como lo teniamos (todos
los `Stop*`, luego todos los `Same*`): es `ctor, InitVars, ReadAll, StopForce,
IsConnected, IsPlaying, ButtonIsPressed, PedalsConnected` y luego **terna a
terna** `Play/Stop/Same` por cada efecto. 34 funciones movidas de golpe: de 44
saltos a 10.

---

## 2. La `.data`: de "descolocada entera" a IDENTICA

Este frente no estaba ni abierto. `linkdelta` daba `resto IGUAL` --que compara
TAMANOS-- y por eso nadie habia mirado el CONTENIDO. La `.data` de zPlatform son
90.176 B y **casi todos son un solo simbolo**, `bin_globala_bun` (89.716 B): si
arranca en el offset equivocado, el DOL sale distinto de punta a punta. Eso
explicaba los 173 rangos que `dolwhere` le imputaba.

Estado inicial contra objetivo (offsets dentro de la `.data` de la unidad):

| | objetivo | nosotros (base) |
|---|---|---|
| `bin_globala_bun` | +0x19C | **+0xD4** |
| tamano de seccion | 90.176 | 90.180 |

Seis causas, las seis medidas y las seis arregladas:

1. **`#include globala_bun.cpp` estaba en segundo lugar de la SourceList.**
   En el objetivo el blob va **detras de `SunE.cpp`** (arranca justo detras de
   `lbl_8041F8C3`, el byte 0xF4 que emite SunE). El propio comentario de
   `globala_bun.cpp` ya lo decia desde hace rondas; el `#include` no lo respetaba.
2. **`FEngDiscErrorPackage` estaba en `.data`.** El DWARF del original la declara
   `static const char FEngDiscErrorPackage[14]` **en `.rodata`, 0x80403B1C**. Le
   faltaba un `const`; sin el metia 16 B delante de `s_OpenCover_ErrorText`.
3. **`softwareResetStartTick` estaba en `.data`.** El DWARF la pone en **`.bss`
   0x8048DD18**: en el original se declara SIN inicializador. Los otros cinco
   estaticos de `DVDErrorTask` si tienen direccion de `.data`
   (0x8041F7F4..0x8041F804); este no.
4. **`egAlphaA..F` estaban en `SunE.cpp`.** El DWARF los declara entre `arenaLo`
   y `s_OpenCover_ErrorText`, y sus direcciones (0x8041F738..0x8041F748) caen
   **delante** de `s_OpenCover_ErrorText` (0x8041F74C): viven en `Platform_G.cpp`.
   `SunE.cpp` no los usa, solo los definia.
5. **Tres bloques `asm()` de `keep.lst` estaban todos al final de la unidad.**
   `gap_06_8041F72C_data` va detras de `snProfilerEnable`; `lbl_8041F818` y
   `gap_06_8041F82C_data` van **entre** las variables de `JoyE.cpp`, cuyo orden
   ademas era otro (el del objetivo: `plat_lgwheels`, `lbl_8041F818`,
   `JoystickRingBufferTop`, `JoystickRingBufferBottom`, `gap_06_8041F82C_data`,
   `JoystickInitialized`, `lbl_8041F834`).
6. **`lbl_8041F834` (12 B), `gap_06_8041F84E_data` (2 B) y
   `gap_06_8043573C_data` (4 B) no los emitia nadie**, aunque `keep.lst` los
   nombra desde hace rondas. Contenido leido VERBATIM del ELF original
   (`lbl_8041F834` = `00000000 0000003C 0000001E`).

### 2.1 Y la septima, que es la unica sutil: `SunHalfScale`

`static float SunHalfScale = 0.5f;` no lo referencia nadie. Como simbolo estatico
muerto de 4 B **`-strip-unused-data` se lo lleva ENTERO** --no deja cadaver-- y
con el se iban 4 bytes: a partir de 0x8041F84C toda la `.data`,
`bin_globala_bun` incluido, salia **4 B antes**. En NUESTRO OBJETO estaba en su
sitio; solo se veia en el ENLACE. Es la version de datos de
`nfsmw-linked-es-la-metrica`.

El objetivo tiene esos 4 B partidos en dos simbolos con nombre (`lbl_8041F84C`
= `0x3F00` y `gap_06_8041F84E_data` = `0x0000`), **los dos ya en `keep.lst`**.
Sustituyendo la variable muerta por ese `asm()`, sobreviven.

**Ese solo cambio: `dolwhere` 60.453 B -> 6.699 B.**

### 2.2 Resultado

La `.data` de nuestro objeto es ahora, simbolo a simbolo y offset a offset, la
del objetivo extraido: `snProfilerEnable`, `gap_06_8041F72C_data`, `egAlphaA..F`,
`s_OpenCover_ErrorText`, los cinco estaticos de `DVDErrorTask`, los tres de
`TextureInfoPlat`, los siete de `JoyE`, los seis de `SunE`, `lbl_8041F858`,
`lbl_8041F8C3`, `bin_globala_bun` **en +0x19C**, `randomSeed`,
`gap_06_8043573C_data`, `gComment1`, `gap_06_80435744_data`. Tamano 90.176 =
90.176.

---

## 3. La serie del DOL, en el mismo arbol

| estado | `dolwhere zPlatform` |
|---|---:|
| base (arbol revertido y recompilado) | 95.754 B |
| `.text` en ORDEN PERFECTO | 67.000 B |
| `globala_bun` detras de `SunE` | 63.559 B |
| resto de la `.data` + `const` + `.bss` (con la `.rodata` recompensada) | 60.453 B |
| **`SunHalfScale` -> `lbl_8041F84C`** | **6.699 B** |

---

## 4. Lo unico que queda: el ORDEN de la `.rodata` (6.699 B)

`dolwhere` reparte asi los 6.699 B que quedan. La `.rodata` de la unidad son
6.712 B: o sea **coinciden 13 bytes**.

| B | rangos | simbolo |
|---:|---:|---|
| 4.798 | 1 | `lbl_80402108` (el bloque prefijo y todo lo que le sigue) |
| 1.077 | 1 | `$LC486` |
| 166 | 1 | `s_OpenCover_ErrorText` (tabla de punteros a esas cadenas) |
| ~660 | 130 | reubicaciones dentro de funciones que apuntan a esas cadenas |

**El diagnostico, y es lo que hay que llevarse de aqui.** El objetivo NO tiene un
bloque de datos escrito a mano de 1.104 B: `pad_05_80402108_rodata` es el **pool
de literales que cc1plus emite**, y el troceador le pone ese nombre porque
ninguna reubicacion apunta dentro. Lo unico irreproducible de verdad son los
**92 B iniciales** (`GAMECUBE`, el `d:/mw/...` de la maquina de EA, `%f,%f,%f`,
`bad_alloc`, `STL`): a partir de **0x5C** empieza `"Attrib::Attribute"`, que es
un literal que **cc1plus tambien nos emite a nosotros** (`$LC151`).

O sea: nuestro `asm()` prefijo de 820 B **duplica 728 B de literales que el
compilador ya emite**, y `keep.lst` fuerza ademas 33 `$LC` (640 B) vivos que en
el objetivo estan una sola vez dentro del pool. Las dos cosas juntas cuadraban el
TAMANO --por eso `linkdelta` decia `IGUAL`-- y ninguna cuadra el CONTENIDO.

Comprobacion directa, cadena a cadena:

```
objetivo 0x05C.. : Attrib::Attribute, Attrib::Instance, Attrib::Definition,
                   Attrib::Class, Attrib::Database, Attrib::TypeDesc,
                   Attrib::RefSpec, Attrib::Blob, Attrib::Gen::simsurface,
                   Attrib::TAttrib, Attrib::Gen::ecar, ...
nuestro  0x334.. : GAMECUBE, _EU, Attrib::Attribute, Attrib::Instance,
                   Attrib::Definition, Attrib::Class, Attrib::Database,
                   Attrib::TypeDesc, Attrib::RefSpec, Attrib::Blob,
                   Attrib::Gen::pvehicle, Attrib::TAttrib,
                   EventSequencerSystems, Attrib::Gen::simsurface, ...
```

Son **casi** la misma secuencia (`rodorden`: 80 de 97 cadenas en secuencia), pero
la nuestra intercala `pvehicle` y `EventSequencerSystems` donde el objetivo pone
`simsurface` y `ecar`. **El orden del pool de literales es el orden de PARSEO de
las cabeceras**: es el mismo frente que `parseord.py` ataca para las vtables.

El segundo sintoma lo confirma: el objetivo pone `"Frontend"`, `"InGame"`,
`"Loading"` (los nombres de `VMStatsGlobalsInit`) **en 0x450, justo detras del
pool** --son los primeros literales "propios" del TU--; nosotros los emitimos en
0x11A4, detras de los textos de error del DVD.

### Receta para quien lo retome (NO aplicada: toca `keep.lst`, regla 3)

1. Recortar el `asm()` prefijo de `zPlatform.cpp` de **820 B a 92 B** (hasta
   `"STL"` inclusive, offset 0x5C). Es lo unico que nuestro arbol no puede
   generar.
2. **Borrar de `keep.lst` las 33 entradas `@lc zPlatform`** (640 B): fuerzan vivas
   copias duplicadas de cadenas que el pool ya trae. Lineas 1278, 1280, 1282,
   1284, 1286, 1288, 1290, 1292, 1294, 1296, 1298, 1300, 1302, 1304, 1306, 1308,
   1310, 1312, 1314, 1316, 1318, 1320, 1322, 1324, 1326, 1328, 1330, 2881, 2883,
   2885, 2887, 2889, 2891 (con su comentario `# @lc` de encima).
3. Borrar del bloque de 38 cadenas del final de `zPlatform.cpp` las **22 que ya
   estan en el prefijo** (375 B: `Attrib::Attribute`, `Attrib::Gen::ecar`,
   `camerainfo`, `effects`, `audioimpact`, `audioscrape`, `WorldBodyConn`,
   `Pkt_Body_Open`, `Pkt_Body_Service`, `World_UpdateBody`, `Pkt_Body_Send`,
   `World_OneShotEffect`, `WorldEffectConn`, `Pkt_Effect_Open`,
   `Pkt_Effect_Service`, `Attrib::Gen::speech`, `MGeneric`, `MAudioReflection`,
   `PlayerNum`, `Dist`, `Covered`, `Attrib::Gen::milestonetypes`).
4. Y entonces si: mover los `#include` de cabeceras de `zPlatform.cpp` y de
   `Platform_G.cpp` hasta que el pool salga en el orden del objetivo, midiendo
   con `rodorden.py` (que ya da la permutacion cadena a cadena) y con el volcado
   de `.rodata` del ELF original.

**Aviso**: los pasos 1-3 mueven ~750 B de `.rodata`; hasta que el 4 este hecho la
seccion NO cuadrara de tamano y el enlazador empujara `.data` 32 B (la ventana de
32). Es trabajo de una tanda entera y **no se puede hacer por partes**.

### Un aviso menor y util

`pad_05_80402108_rodata`, `lbl_80403B3C` y los cuatro `gap_07_*_bss` de
`keep.lst` **no los define nuestro objeto** (son parte de las 227 entradas
fantasma que conto la r62/p3). No dan error; simplemente no hacen nada.

---

## 5. zFe y zGameplay: por que NO se han tocado, y que es lo que les pasa

Lo primero que hice al llegar fue medir, y la medida cambia el encargo. El
encargo dice "mismo metodo que los otros dos encargos de orden: `textorder` +
`parseord` iterando sobre los `#include` de la SourceList". **No vale para estas
dos**: su desorden y el de zPlatform son de naturaleza distinta.

```
zPlatform  primer salto de delta en 0x0020   (la 2a funcion de la unidad)
zFe        primer salto de delta en 0x26AB8  (158.392 B de 174.200: el 91 %)
zGameplay  primer salto de delta en 0x1B6DC  (112.348 B de 153.376: el 73 %)
```

**En zFe y zGameplay TODO el codigo normal esta ya en el orden del objetivo.** Lo
unico descolocado es el **bloque diferido** --el que `finish_file` escribe al
final--, y ahi el orden no lo decide el fuente sino el punto de uso y las
pasadas. Reordenar los `#include` de la SourceList **no puede tocar ni una de las
89 funciones descolocadas**, porque todas salen detras del ultimo `.cpp`.

Censo, con la subsecuencia creciente mas larga (minimo de funciones a cambiar de
sitio) y el reparto real de los bytes de DOL:

| unidad | emparejadas | LIS | **a mover** | `linkdelta` | `dolwhere` |
|---|---:|---:|---:|---|---:|
| zPlatform | 137 | 137 | **0** | `.text +0` `IGUAL` | 6.699 B |
| zFe | 922 | 870 | **52** | `.text +0` `IGUAL` | 34.384 B |
| zGameplay | 768 | 731 | **37** | `.text +0` `IGUAL` | 35.438 B |

### 5.1 zGameplay: 37 funciones, y son cuatro racimos

| racimo | n | que es |
|---|---:|---|
| accesores de `GRaceStatus` (`SetIsLoading`, `EnterSuddenDeath`, `SetTaskTime`, `SetActivelyRacing`, `SetHasBeenWon`, `GetCacheName`) | 6 | los emitimos **los primeros** del bloque diferido; el objetivo los pone en la posicion 729 de 769 |
| `Advance__t15GObjectIterator1Z*` + `__t15GObjectIterator1Z*Ui` + `FindObject__H1Z*` | 12 | los emitimos en bloque; el objetivo los **intercala** entre las instanciaciones STL (obj#606,607 / #612,613 / #616,617 / #672,673) |
| `GetType__C*`, `Attach/Detach/IsAttached__10GCharacter`, `IsFlagSet` | 9 | dispersos |
| `_GetKind__*` de mensajes, `ClearAll__10GRacerInfo`, `ClassKey`, `EnsureLoaded` | 10 | dispersos |

Reparto del DOL: `SetAttribute__H1Zb_11GRaceCustom...` 19.701 B en UN rango y
`SetIsLoading__11GRaceStatusb` 8.236 B en otro --los dos son ventanas de `.text`
imputadas al simbolo anterior al corte--, mas `$LC221` 4.247 B de `.rodata`. O
sea: **~28 kB de los 35 kB son el orden del `.text` diferido** y ~4,5 kB la
`.rodata`.

Hay un segundo frente medido: `vtord zGameplay` da **17 de 19 posiciones que no
casan**, con tres vtables SOLO NUESTRAS (`3MD5`, `13IVehicleCache`,
`Q33UTL3COM8IUnknown`). Descontando esas tres el orden sigue siendo otro (nuestro:
`LuaMessageDeliveryInfo, GRaceCustom, GRaceStatus, GRaceParameters, GTrigger,
GMarker, GManager, GCharacter...`; objetivo: `GCharacter.IAttachable, GCharacter,
GHandler, GState, GRaceStatus, GRaceCustom, GRaceParameters, GActivity,
LuaMessageDeliveryInfo, GTrigger...`). **`parseord.py zGameplay` da CONTROL
FALLIDO** (invertir el orden reproduce 2 de 15), asi que su cifra no vale para
repartir: hay que ir por `vtord` y `-E` a mano.

### 5.2 zFe: 52 funciones, y son casi todas destructores

De las 52, **31 son `_._<clase>`** y 7 mas son
`React__<clase>PCcUiP8FEObjectUiUi`. El patron mas claro es el de las pantallas:
el objetivo emite en pareja `_._10MainCareer` + `React__10MainCareer`,
`_._9Challenge` + `React__9Challenge`, `_._13MainQuickRace` + ... (obj#745..756,
doce seguidos) y nosotros los sacamos en el mismo orden pero **35 posiciones mas
tarde** (nue#777..788), detras del racimo de `ScrollerSlot`.

Reparto del DOL: `$LC258` 9.375 B (`.rodata`), `_._24uiRapSheetRankingsDetail`
7.013 B y `React__9Challenge` 6.608 B (`.text`), `_._13UIMemcardMain` y
`MemcardGetCurrentUIOperation` ~1,2 kB, mas **once vtables con rango propio**
(`_vt.13UIMemcardMain` 634 B, `_vt.9PauseMenu` 428 B, ...). `vtord zFe`: **111 de
157 posiciones no casan**. En zFe el frente de vtables es tan grande como el de
funciones.

### 5.3 Lo que yo haria con ellas

La palanca de la seccion 1.2 (`inline` para elegir la cola + posicion de la
definicion respecto del llamante para elegir si se pliega) **es exactamente la
que necesitan estas dos**: un `_._<clase>` que sale pronto se retrasa
definiendolo `inline` mas abajo del fichero, y uno que sale tarde se adelanta
sacandolo de la clase. Cada racimo es una edicion y una medida de `textorder`
(milisegundos, sin enlazar). Con 52 y 37 funciones y racimos de 6 a 12, son dos
encargos de una tanda cada uno --pero de **funciones**, no de `#include`.

---

## 6. Ficheros tocados

Los nueve son **exclusivos de zPlatform** (ninguna otra SourceList los incluye;
comprobado con `grep -rl` sobre `src/Speed/Indep/SourceLists/`). Ninguna cabecera
compartida. Todos en **UTF-8 puro y LF** (tres estaban en CRLF en el arbol de
trabajo con el indice en LF; los he normalizado a LF, que es lo que git guarda).

| fichero | que se ha hecho |
|---|---|
| `src/Speed/Indep/SourceLists/zPlatform.cpp` | `#include globala_bun.cpp` detras de `SunE.cpp`; el `asm()` de `.data` partido (solo se queda `gap_06_80435744_data`); fuera la cadena duplicada `"Pkt_Effect_Send"` |
| `src/Speed/GameCube/Src/Platform_G.cpp` | orden de las 11 funciones; `VMStatsManager::Init` `inline` + alias; entran `egAlphaA..F` y `gap_06_8041F72C_data`; `FEngDiscErrorPackage` a `const`; `softwareResetStartTick` a `.bss` |
| `src/Speed/GameCube/Src/Ecstasy/TextureInfoPlat.cpp` | orden de las 17 funciones |
| `src/Speed/GameCube/Src/JoyE.cpp` | orden de las 6 funciones; orden de las variables + `lbl_8041F818`, `gap_06_8041F82C_data`, `lbl_8041F834` |
| `src/Speed/GameCube/Src/Render/SunE.cpp` | `eInitSunPat` al final; `egAlpha*` fuera; `SunHalfScale` -> `lbl_8041F84C` + `gap_06_8041F84E_data` |
| `src/Speed/GameCube/Src/Render/AcidFX_G.cpp` | los tres `Plat*` detras de los cuatro `afx*` |
| `src/Speed/GameCube/Src/Movie_GC.cpp` | `RCMP_GetMaxFramesOutStanding` y `~GCHW_VD` de sitio |
| `src/Speed/GameCube/Src/Logitech/LGWheels.cpp` | orden de las 34 funciones |
| `src/Speed/GameCube/Src/xSparks.cpp` | orden de las 10 funciones; `~CGEmitter` `inline` al final; `gap_06_8043573C_data` |

Los tres negativos medidos (1.1 el `inline` a secas, 1.2 el destructor dentro de
la clase, y el `SunHalfScale` como `static float`) estan escritos **en comentario
junto a su funcion**, que es donde `previo.py` los encuentra.

---

## 7. Propuestas para el jefe

- **`lcfix.py`: CERO correcciones pendientes en zPlatform.** `lcfix --check
  zPlatform` = "todas las entradas @lc estan al dia". Ninguna de mis ediciones ha
  renumerado un `$LC` que `keep.lst` nombre. **Ojo**: `lcfix --check` GLOBAL si
  falla, pero en `zAnim` (4 cadenas sin `$LC` propio) y `zEagl4Anim`
  (`$LC78 -> $LC77`); **no es mio** --no he tocado ni sus fuentes ni `keep.lst`--
  y ya fallaba antes de empezar.
- **`keep.lst` / `splits.txt` / `symbols.txt` / `configure.py`: CERO propuestas
  que aplicar esta ronda.** La unica que tengo (borrar las 33 entradas
  `@lc zPlatform`) **solo tiene sentido junto con los cuatro pasos de la seccion
  4** y no se puede validar por partes: aplicada sola rompe el DOL.
- **Ninguna entrada venenosa creada para otra unidad**: mis nueve ficheros no los
  incluye ninguna otra SourceList, y `zPlatform` no esta en el enlace, asi que el
  DOL base es literalmente el mismo antes y despues de mis ediciones.
- `checksplits` LIMPIO (0 solapes, 0 cortes), `gapchk zPlatform` total 0,
  `mangfix zPlatform --check` al dia, `keepchk2` sin entradas rotas de zPlatform.

---

## 8. Para la memoria del proyecto

1. **`linkdelta IGUAL` no dice nada del CONTENIDO de `.data`, y en una unidad con
   un blob grande eso es TODO el DOL.** zPlatform tenia `resto IGUAL` desde hace
   rondas con los 89.716 B de `bin_globala_bun` arrancando 200 B antes de su
   sitio. El chequeo barato es comparar la lista de simbolos de `.data` del
   objeto extraido con la del nuestro, offset a offset: son 30 lineas y se ve al
   vuelo.
2. **Un `static` muerto de 4 B se lo lleva ENTERO `-strip-unused-data`, no deja
   cadaver de 4 B.** Corrige el matiz de `nfsmw-ventana-de-32` ("uno de 4 B se
   queda"): eso vale para el RESTO de un simbolo vivo, no para un simbolo muerto.
   Y solo se ve en el ENLACE, nunca en el objeto.
3. **`inline` elige la COLA; la posicion de la definicion respecto del llamante
   elige si se PLIEGA.** Son dos mandos independientes y con los dos se coloca
   cualquier funcion en el bloque diferido sin mover una instruccion. Amplia
   `nfsmw-decl-comdat-inline` y `nfsmw-en-clase-es-inline`.
4. **Antes de dar por "por punto de uso" el desorden de una unidad, mira si el
   orden emitido es simplemente el orden de las lineas del `.cpp`.** En zPlatform
   lo era en 84 de 85 saltos y la r61 lo habia dado por lo otro.
5. **El SITIO del primer salto de delta dice de que TIPO es el desorden**: si cae
   pronto es orden de fuente (barato); si cae despues del 70-90 % del `.text` es
   el bloque diferido, y reordenar `#include` no puede hacer nada.
6. **El "bloque prefijo" escrito a mano de una unidad puede ser un ANDAMIO que
   duplica lo que el compilador ya emite.** En zPlatform, 728 de sus 820 B son
   literales que cc1plus tambien produce; cuadraban el tamano y ninguno cuadra el
   contenido. Antes de alargar un prefijo, comprueba desde que offset el objetivo
   deja de ser irreproducible (aqui: 0x5C, los 92 B de bWare/STL).
