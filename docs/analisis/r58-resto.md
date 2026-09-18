# r58 — `resto` (zAI, zSpeech, zLua): las dos `.data` que faltaban, escritas y cerradas

Agente `resto`. Propiedad: `zAI`, `zSpeech`, `zLua` y los ficheros de `AI/` y `Speech/`.
**Seis ficheros tocados, los seis exclusivos de su unidad** (comprobado con `grep -rl`:
`LuaGameHooks.cpp` sólo lo incluye `zLua.cpp`; `AICopManager.cpp`, `AIActionRace.cpp` y
`AIVehicleHelicopter.cpp` sólo `zAI.cpp`). **Cero cabeceras compartidas.** Ni `config/`,
ni `splits.txt`, ni `keep.lst`, ni `configure.py`.

---

## 0. Titular

    zLua    .text +0  rodata-720 data-320   ->   .text +0  rodata-728
    zAI     .text +0  rodata-240 data-384   ->   .text +0  rodata-240
    zSpeech .text +0  IGUAL                 ->   .text +0  IGUAL   (no la he tocado)

**Las dos `.data` que quedaban en mis unidades están CERRADAS: 712 B de contenido escrito,
704 B de déficit de sección a cero.** Las dos secciones `.data` del ELF enlazado miden ya
exactamente lo que el objetivo (`0x413E0` en las dos, medido con `dolwhere`), y con ellas
desaparece el desplazamiento que zAI y zLua le metían a todo lo que va detrás en el DOL.

`fncmp` **idéntico antes y después en las tres**: `zAI` 0 de 1030, `zLua` 0 de 537,
`zSpeech` 1 de 703 (`Setup`, la vetada de siempre). **Cero regresiones.**

Sello, con **tres** compilaciones seguidas del árbol final:

    zAI      3602380a546d9b83e77e32821cbb9afb5e6d2ea3   (x3)
    zLua     f717971f62e0070d925edc723a5aefccbf81441e   (x3)
    zSpeech  fd6a1f3f332cbda61f6ad3e0da5f7ee20fa24bda   (x3)

Ninguna promociona: `trypromo` da `DOL ROTO` en las tres
(`834945dac9df` / `4c0adc2d1364` / `dbb397947eb9`).

---

## 1. AVISO DE MEDIDA: sin `lcfix`, `linkdelta` MIENTE sobre estas dos unidades

Mover los estáticos de `LuaGameHooks.cpp` renumeró **105** entradas `$LC` del `keep.lst`.
Con el fichero del árbol tal cual, `linkdelta` decía `rodata-1184` de `zLua` donde la
cifra real es `-728`: **456 B de mentira**. No he corrido `lcfix` (es entrada del enlace y
hay otros agentes midiendo); **todas las medidas de este informe van contra una copia
corregida en el scratchpad**, enlazando con `--ldflags "-strip-unused-data -keep <copia>"`.

**Las cifras de este informe no se reproducen hasta que la ventana pase `lcfix`.**

---

## 2. `zLua`: la `.data` cierra al byte, y el mapa de la r55 era exacto

La `.data` del objetivo mide 620 B y la nuestra medía 296. Los 324 B que faltaban son
**cinco huecos de ceros que `keep.lst` nombra desde hace rondas y nadie había escrito**
(`gap_06_8041D584/8041D594/8041D664/8041D720/8041D72C_data`), más **una permutación**.

### 2.1 Los cinco huecos son ceros de verdad, y está comprobado

`.rela.data` del objeto extraído tiene **41 reubicaciones y las 41 caen dentro de
`accessorTable` y `flagMapping`**: ninguna toca los huecos. O sea que no son punteros que
dtk haya puesto a cero (la trampa de `nfsmw-huecos-leer-el-dol`), son ceros. Escritos como
`asm()` de ámbito de fichero, todos con `.balign 4` —las cinco direcciones son 4-alineadas
y **no** 8-alineadas, y un `.balign 8` habría movido tres de ellos (el error que la r57
cazó en `zSpeech`)—, en el punto exacto del flujo de parseo:

| símbolo | B | dónde va escrito |
|---|---:|---|
| `gap_06_8041D584_data` | 12 | `zLua.cpp`, entre `LuaRuntime.cpp` y `LuaPostOffice.cpp` |
| `gap_06_8041D594_data` | 8 | entre `LuaPostOffice.cpp` y `LuaBindery.cpp` |
| `gap_06_8041D664_data` | 108 | delante de `LuaGameHooks.cpp` |
| `gap_06_8041D720_data` | 4 | detrás de `LuaGameHooks.cpp` (antes de `lmem.c`) |
| `gap_06_8041D72C_data` | 192 | el final de `zLua.cpp` |

### 2.2 La permutación: `kDisableNIS` era un estático LOCAL, y el sufijo lo delata

El objetivo emite `flagMapping.32877`, luego **`kDisableNIS.32896`**, luego
`kPrintScriptMessages` (sin sufijo). Nosotros emitíamos los dos flags primero y
`flagMapping` al final.

**El sufijo `.N` de GCC 2.95 sólo lo llevan los estáticos de FUNCIÓN.** Así que en el
original:

* `flagMapping` es el estático local de `Audio_SetFlag` (ya lo era);
* **`kDisableNIS` es un estático local de `NIS_Play`**, no de fichero — por eso lleva
  sufijo en el objetivo y no lo llevaba en el nuestro;
* `kPrintScriptMessages` sí es de fichero, pero **declarado detrás de `NIS_Play`**, justo
  delante de sus dos usuarios (`Debug_Print`/`Debug_PrintInstance`).

Movidas las tres declaraciones a esos sitios, la `.data` de `zLua` sale **byte a byte con
la disposición del objetivo: 620 B y los quince símbolos en su desplazamiento exacto**.
`fncmp` no se mueve (un `static volatile bool` local genera el mismo `lis/lbz`).

> **La regla, y sirve para todo el árbol**: un `.N` en el nombre de un dato de `.data` del
> objetivo dice que en el original era una variable declarada DENTRO de una función. Es un
> dato de forma de fuente que se lee gratis de la tabla de símbolos.

### 2.3 `BWARE_PREFIX_GAMECUBE`: una línea, y la cabeza del pool casa entera

`zLua.cpp` ya escribía a mano el prefijo de bWare (`pad_05_803ED338_rodata`, que trae
`"GAMECUBE"` en +0) pero sólo definía `BWARE_PREFIX_BADALLOC` y `BWARE_PREFIX_STL`. Sin
`BWARE_PREFIX_GAMECUBE`, `bGetPlatformName()` internaba una **segunda** copia de 12 B en
0x5C y corría el pool entero 8 B.

Con `#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)` la cabeza del pool pasa de

    == 92 B  +  SOBRA 12 B "GAMECUBE"  +  == 152 B          (Attrib::Attribute en 0x68)

a

    == 244 B                                                (Attrib::Attribute en 0x60)

que es el 0x60 del objetivo. **Es la misma cura que la r57 midió en `zSpeech`, y `zLua`
era una de las 13 unidades de la lista.** En bytes de sección PAGA NEGATIVO (`rodata`
−720 → −728, porque `keep.lst` salvaba esos 8 B) pero es **estructuralmente obligatorio**:
sin ella todo lo que hay detrás en el pool está 8 B corrido y no puede casar nunca.
Lo dejo puesto y lo digo con la cifra.

### 2.4 Lo que le queda a `zLua`: 704 B de `.rodata`, con el mapa al byte

`dolwhere` ya sólo se queja de una sección: `.rodata` `0x4C260` contra `0x4C520`.

* En el OBJETO faltan **432 B** de cadenas.
* Y el enlazador se lleva **224 B** nuestros de más (17 `$LC` muertos que el objetivo NO
  tiene: `EventSequencerSystems`, `Attrib::Gen::induction/transmission/emitterdata/`
  `emittergroup/presetride`, `done`, `EAGL4::SymbolEntry`, `SpeechSampleMap node`,
  `ScrollerDatumNode`, `ScrollerSlotNode`…), contra 88 B que se lleva de la base.
* `deadstr.py zLua --keep` **no aporta ni una línea nueva**: las 60 cadenas muertas que el
  objetivo sí tiene están ya en `keep.lst` (101 símbolos salvados). Esa veta está agotada.

El orden de la cabeza del pool, medido hoy con las cadenas en orden de dirección:

| objetivo | nuestro |
|---|---|
| `MGeneric` | — |
| `Attrib::Gen::gameplay` | `Attrib::Gen::gameplay` |
| `Attrib::TAttrib` | `Attrib::TAttrib` |
| `Attrib::Gen::milestonetypes` | `done` *(sobra)* |
| `LuaRuntime memory pool`, `LuaRuntime` | — |
| `__index` | `__index` |
| `Lua emergency allocation`, `LuaRuntime temp compression buffer`, `LuaRuntime VM reset snapshot` | — |
| `NIL BOOL LUD NUMBER STRING TABLE FUNC THREAD (?)` + separadores (96 B) | — |
| `%d/-%d nil true false %.2f %08X` | `%d/-%d nil true false %.2f %08X` |
| `LuaPostOffice` | — |
| `Attrib::Gen::simsurface` | `Attrib::Gen::pvehicle`, `EventSequencerSystems`, `Attrib::Gen::simsurface` |
| `16.1.0 16.2.1 1.2.3 1.8.1 19.8.31` | `chassis engine induction nos tires transmission` *(sobran)*, `19.8.31 16.2.1 1.2.3` |
| `GRaceStatus` | `emitterdata emittergroup` *(sobran)* |
| `GRuntimeInstance ISimable` | `GRuntimeInstance ISimable` |
| `LuaBindery` | — |
| `Randomize Table Run` | `Randomize Table Run` |

Y la familia D de la r55 (`Attrib::Gen::speech/pvehicle/engine/engineaudio`,
`MAudioReflection`, `PlayerNum`, `Dist`, `Covered`, `Attrib::Gen::audiosystem`,
`VehicleParams`, `Attrib::Gen::speechtune/pursuitlevels/world/visuallook`, `GManager`,
`done`) sigue **entera sin escribir**, entre `Pkt_Effect_Service` y `SMS_MESSAGE_%d`.

**El orden de ataque está cuadrado y es de una ronda entera**: `ATTRIB_TAGS_HAND_POOL`
para matar los parásitos + primers para volver a emitir lo que hace falta + la familia D.
Aplicado a medias es negativo (la r57 lo midió en `zSpeech`: 243.722 B, peor que el punto
de partida). **No lo he empezado a propósito.**

---

## 3. `zAI`: la `.data` cierra, y el encargo tenía razón contra el informe de la r57

El informe de la r57 dice que la `.data` de `zAI` es «hueco anónimo de alineación,
nombres del troceador, no dato que escribir» y que el encargo estaba **REFUTADO**.
**Está medido que no**: los catorce huecos son 388 B escribibles y escribirlos cierra
`data-384`.

| símbolo | B | contenido | dónde |
|---|---:|---|---|
| `pad_06_80415180_data` | 164 | ceros | `zAI.cpp`, delante de `AITrafficManager.cpp` |
| `gap_06_80415230_data` | 44 | ceros | delante de `AICopManager.cpp` |
| `gap_06_80415260_data` | 4 | ceros | **dentro** de `AICopManager.cpp`, entre `mDisableCops` y `mCopMinSpawnDist` |
| `gap_06_804152A8_data` | 4 | ceros | delante de `AvoidableManager.cpp` |
| `lbl_804152C0` | 4 | **85.0f** | delante de `AIActionHeliPursuit.cpp` |
| `gap_06_804152C8_data` | 4 | ceros | delante de `AIActionHeliExit.cpp` |
| `lbl_804152DC` | 8 | **{0, 0.5f}** | delante de `AIActionRace.cpp` |
| `lbl_80415348` | 12 | **{20.0f, 0, 0}** | **dentro** de `AIActionRace.cpp`, delante de `aHumanNavLookAheadData` |
| `gap_06_80415370_data` | 8 | ceros | delante de `AIVehicleRacecar.cpp` |
| `gap_06_804153F8_data` | 16 | ceros | delante de `AIVehicleTraffic.cpp` |
| `gap_06_80415414_data` | 4 | ceros | delante de `AIVehicleHelicopter.cpp` |
| `lbl_8041541C` | 8 | **{2.0f, 6.0f}** | **dentro** de `AIVehicleHelicopter.cpp`, detrás de `kHeliVisualSphere` |
| `lbl_80415438` | 68 | **{1, 13 ceros, 120.0f, 0, 0}** | delante de `AISpawnManager.cpp` |
| `gap_06_80415498_data` | 40 | ceros | el final de `zAI.cpp` |

**Los cinco `lbl_` NO son ceros: llevan constantes.** Escribirlos como relleno habría dado
el tamaño bien y los bytes mal. El contenido está leído del objeto extraído y verificado
contra el símbolo vecino (`kHeliVisualSphere` vale 75.0f en la fuente y el objetivo tiene
75.0f justo donde el mapa dice: la lectura cuadra).

**`pad_06_80415180_data` mide 172 B en el objetivo pero se escriben 164**: los 8 que
faltan son `AITrafficManager::mTrafficMinSpawnDist`/`mTrafficMaxSpawnDist` (225.0f y
300.0f), que ya emitíamos y que el troceador metió dentro del pad porque nadie los
referencia por nombre en el DOL.

Resultado: `.data` 444 → **832 B, el tamaño exacto**, y **los primeros 300 B casan símbolo
a símbolo** (pad, los dos `mTraffic*`, `RandomSortTCDir`, el hueco de 44,
`ICopMgr::mDisableCops`, el de 4, `mCopMin/MaxSpawnDist`, `TheOneCopManager`, los siete
`*goals` y el de 4 de `804152A8`).

### 3.1 Lo que le queda a la `.data` de `zAI`: seis símbolos en otra dirección

El tamaño está, el orden interno no. Del byte 300 en adelante hay seis desplazados, y
todos por el mismo motivo —se emiten donde se instancia su plantilla o donde se compila su
`.cpp`, no donde el objetivo los quiere—:

| símbolo | nuestro | objetivo |
|---|---:|---:|
| `SAP::Grid<AIAvoidable>::mRootX` / `mRootZ` | +768 / +772 | **+300 / +304** |
| `bIgnoreHeliSheet` | +316 | **+680** |
| `AIPerpVehicle::mStagger` | +784 | **+492** |
| `gHeliVehicle` | +788 | **+656** |
| `Factory<AIActionParams,AIAction>::mHead` | +776 | **+684** |
| `Factory<ISimable,AIGoal>::mHead` | +780 | **+688** |

### 3.2 Y la `.rodata −240`: **192 de esos 240 son once cadenas muertas nuestras**

El objeto mide **exactamente lo mismo** que el objetivo (26.344 B los dos) y aun así el
enlace sale 240 B corto. La causa está medida con una sonda: en nuestra `.rodata` hay
**once `$LC` muertos que el objetivo NO tiene** y que `-strip-unused-data` se lleva
(`size & ~7`):

    $LC193 EventSequencerSystems      $LC220 Attrib::Gen::induction
    $LC223 Attrib::Gen::transmission  $LC302 Attrib::Gen::emitterdata
    $LC303 Attrib::Gen::emittergroup  $LC340 done
    $LC346 EAGL4::SymbolEntry         $LC426 SpeechSampleMap node
    $LC427 Attrib::Gen::presetride    $LC1389 ScrollerDatumNode
    $LC1391 ScrollerSlotNode

**Prueba directa**: añadidos los once a mi copia del `keep.lst`, `linkdelta` pasa de
`rodata-240` a **`rodata-48`**. O sea que 192 de los 240 son exactamente esto.

**No lo propongo como arreglo**: el objetivo no tiene esas cadenas, así que salvarlas
acierta el TAMAÑO con los BYTES mal, y es andamio que habría que quitar después. Lo dejo
como diagnóstico, que es lo que cuesta media ronda averiguar. El arreglo bueno es dejar de
emitirlas, y vienen de cinco cabeceras ajenas que `zAI` arrastra:
`Main/EventSequencer.h`, `EAGL4Anim/eagl4supportdef.h`, `Speech/SpeechCache.h`,
`Frontend/MenuScreens/Common/feScrollerina.hpp` y los `Generated/AttribSys/Classes/*.h`.

`deadstr.py zAI --keep` tampoco aporta líneas nuevas: las 30 que encuentra están las 30 en
`keep.lst` (51 entradas `@lc` de `zAI`). **Veta agotada también aquí.**

---

## 4. `zSpeech`: **está IGUAL en las NUEVE secciones, y el aviso del encargo está caducado**

No la he tocado: el encargo la traía con `rodata +8` pendiente, y **eso ya está cerrado**.
`linkdelta` da `IGUAL` y `dolwhere` cuenta **18.091 B** de DOL distinto, todo de orden
(`.text` de COMDAT y orden del pool).

### 4.1 El aviso de `lcfix`: es COSMÉTICO, no un bloqueo

`lcfix --check` dice `FALLO zSpeech: 'GAMECUBE' no tiene simbolo $LC propio`, y la r57
pidió **borrar `keep.lst:1668-1669` como OBLIGATORIO**. **Medido hoy: ya no lo es.**
Después de que `lcfix` corrija la línea siguiente, las dos entradas apuntan al MISMO
símbolo vivo:

    1668: # @lc zSpeech "GAMECUBE"        1670: # @lc zSpeech "Attrib::Attribute"
    1669: zSpeech.o:$LC58                 1671: zSpeech.o:$LC58

y `$LC58` es `"Attrib::Attribute"` (+0x0060, 18 B), que es el símbolo correcto y el
desplazamiento correcto. Por eso `zSpeech` mide `IGUAL` **con la línea puesta**. Borrarla
es limpieza, no arreglo. **`zLua` queda hoy en la misma situación exacta**
(`keep.lst:1332-1333`, también `$LC58` = `Attrib::Attribute`).

> Es un caso de `nfsmw-andamios-caducan` y de `nfsmw-promocion-sin-su-cierre` a la vez: la
> propuesta de la r57 era correcta cuando se escribió y dejó de serlo al pasar `lcfix`,
> pero el informe seguía diciendo «OBLIGATORIO».

### 4.2 Lo que le queda, sin tocar

Lo de la r57 sigue en pie y **es trabajo de ventana, no de ronda**: la rotación de los
cuatro destructores finales del volcado de COMDAT pide mover `class SampleReqList` detrás
de `struct SPCHEventList` en `SpeechManager.hpp`, **que la incluyen 17 `.cpp` de seis
unidades**. No se puede medir la regresión sin compilar unidades de otros agentes, y
compilarlas les pisa el `.o`.

---

## 5. La medida, paso a paso

| paso | zAI | zLua | zSpeech |
|---|---|---|---|
| base | `rodata-240 data-384` | `rodata-720 data-320` | `IGUAL` |
| flags de `LuaGameHooks` a su sitio | — | `data-320` (orden ya exacto) | — |
| los cinco huecos de `zLua` | — | **`rodata-720`** (data a 0) | — |
| `BWARE_PREFIX_GAMECUBE` en `zLua` | — | `rodata-728` (−8, y la cabeza del pool casa) | — |
| los catorce huecos de `zAI` | **`rodata-240`** (data a 0) | — | — |

`fncmp` corrido **después de cada fichero**, no sólo al final. Sello `sha1` estable en tres
compilaciones seguidas al cerrar.

---

## 6. `lcfix`: **172 pendientes al cerrar, 95 mías**

    python scripts/lcfix.py --check
       94  CORRIGE  zLua.o        <- mías (mover los estáticos renumeró sus $LC)
       71  CORRIGE  zMain.o       <- de otro agente
        4  CORRIGE  zFe2.o        <- de otro agente
        1  CORRIGE  zAnim.o       <- de otro agente
        1  FALLO    zLua: 'GAMECUBE' no tiene simbolo $LC propio      <- cosmético (§4.1)
        1  FALLO    zSpeech: 'GAMECUBE' no tiene simbolo $LC propio   <- cosmético (§4.1)

**No lo he corrido.** Y aviso fuerte: **mis unidades no se pueden medir ni promocionar sin
pasarlo antes**. Con el `keep.lst` del árbol tal cual, `linkdelta` dice `rodata-1184` de
`zLua` donde son `-728`.

---

## 7. Sorpresas

1. **Un `.N` en el nombre de un dato de `.data` dice que era una variable de FUNCIÓN.**
   `kDisableNIS.32896` contra nuestro `kDisableNIS`: el objetivo lo declaraba dentro de
   `NIS_Play`. Es forma de fuente que se lee gratis de la tabla de símbolos y que hasta
   ahora nadie estaba mirando. Colocó tres símbolos de golpe.
2. **`gap_*`/`pad_*`/`lbl_*` en `keep.lst` es una lista de la compra sin cobrar.** Los
   diecinueve que he escrito estaban nombrados desde hacía rondas. **Merece un barrido del
   árbol**: `grep -E "\.o:(gap|pad|lbl)_" config/GOWE69/keep.lst` contra lo que hay escrito
   en los `.cpp`, unidad por unidad.
3. **Un `lbl_` no es relleno.** Cinco de los catorce huecos de `zAI` llevan constantes de
   coma flotante (85.0f, 0.5f, 20.0f, 2.0f, 6.0f, 120.0f). Escribir ceros habría dado el
   tamaño bien, los bytes mal y `linkdelta` diciendo que está arreglado.
4. **Las dos vetas de `deadstr` están agotadas en `zAI` y en `zLua`**: 30 y 60 cadenas
   encontradas, cero líneas nuevas. Lo que queda es el problema contrario —cadenas
   nuestras que el objetivo no tiene—, y ésas no se arreglan con `keep.lst`.
5. **El árbol se movió debajo de mí.** El `sha1` de `zSpeech.o` cambió (`30efe7be` ->
   `fd6a1f3f`) sin que yo tocara un solo fichero suyo: otros agentes editaron `IModel.h`,
   `IPlayer.h`, `INIS.h` y `WorldConn.h` en mitad de la ronda. Medido: `linkdelta` sigue
   dando `IGUAL` y `fncmp` sigue dando 1 de 703, así que para `zSpeech` es limpio. Es la
   segunda ronda seguida que le pasa a este agente: **el `sha1` del `.o` no vale como sello
   de fuente**, el sello bueno es el hash del DOL.
6. **`dolwhere` no arranca si falta una sección, y con la `.data` cerrada sigue sin
   arrancar**: ahora dice `LAS SECCIONES NO COINCIDEN` sólo por la `.rodata`, pero el
   volcado que imprime ya sirve de medida —`80415180 s=413E0` en los dos lados—.

---

## 8. Lo que pido, con la cifra

| propuesta | vale | quién |
|---|---:|---|
| **`python scripts/lcfix.py`** — 105 correcciones, casi todas de `zLua`. **Sin esto mis dos unidades no se pueden medir**: `linkdelta` miente 456 B | obligatorio | ventana |
| borrar `keep.lst:1332-1333` (`# @lc zLua "GAMECUBE"`) y `1668-1669` (el de `zSpeech`) | 0 B, es limpieza — **la r57 lo pedía como obligatorio y ya no lo es** (§4.1) | ventana |
| `#define BWARE_PREFIX_GAMECUBE` en las 11 unidades que quedan con prefijo a mano | 12 B de pool mal colocado por unidad; **paga negativo en bytes y positivo en posición** | los dueños |
| `SampleReqList` detrás de `SPCHEventList` en `SpeechManager.hpp` (17 `.cpp`, 6 unidades) | la rotación final del `.text` de `zSpeech` | ventana |
| barrido de `gap_*`/`pad_*`/`lbl_*` de `keep.lst` sin escribir, por unidad | 388 B en `zAI` y 324 en `zLua` sólo esta ronda | ventana |

## 9. Siguiente paso, por unidad

1. **`zAI`, `rodata −240`** — 192 son las once cadenas muertas de §3.2. El arreglo bueno es
   dejar de arrastrar `EventSequencer.h`, `eagl4supportdef.h`, `SpeechCache.h` y
   `feScrollerina.hpp` (patrón de guarda inerte de la r57), y luego los 48 que quedan.
   Con eso `zAI` queda **IGUAL en las nueve** y sólo le sobra el orden.
2. **`zAI`, el orden de la `.data`** — los seis símbolos de §3.1, con dirección de destino
   escrita.
3. **`zLua`, `rodata −704`** — 432 B de cadenas que faltan más 224 de parásitos.
   `ATTRIB_TAGS_HAND_POOL` + primers + la familia D, **en un solo paquete**: a medias es
   negativo y está medido en `zSpeech`.
4. **`zSpeech`** — sigue esperando la ventana para `SpeechManager.hpp`.
