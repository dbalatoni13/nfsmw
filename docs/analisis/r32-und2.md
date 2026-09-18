# Ronda 32 — `und2`: las tres SourceLists gordas YA ENLAZAN

Encargo: `zPhysics` (146.124 B), `zSpeech` (178.888 B) y `zFe` (174.200 B) —
**499.212 B** bloqueados por símbolos indefinidos.

**Resultado: las tres enlazan.** Los 470 errores de enlace (12 + 318 + 140) están
cerrados con **nueve definiciones de datos y dos `static` retirados**, todo en
`src/`, **sin tocar `configure.py` ni `config/GOWE69/*`**, **sin una sola
regresión** (`measure.py --cmp` +0 B / +0 funciones en las tres, `pctsnap --cmp`
**EMPEORAN: ninguna**) y con el **control `DOL OK`**.

Ninguna promociona: las tres salen **`DOL ROTO` por dato que falta**, y digo
cuánto y en qué sección (§4). No he entrado ahí.

**Y el hallazgo que cierra el frente de datos con nombre:** después de estas
definiciones, **a las tres unidades no les falta NI UN símbolo de datos con
nombre** (§5). Todo lo que queda es `lbl_` (el pool anónimo del §2/§3 del brief),
relleno `gap_`/`pad_` y estáticos `.NNNN` **renombrados, no ausentes**.

---

## 0. Las cinco frases

1. **Las tres enlazan**, y ninguno de los 470 errores hacía falta arreglarlo con
   ensamblador: son **datos que nadie definía** más **dos funciones que estaban
   `static` y el objetivo tiene globales**.
2. **`zFe` estaba a tres definiciones, como decía el encargo, y las tres cayeron
   en su sitio exacto de `.bss`/`.data`** — comprobado por desplazamiento contra
   `symbols.txt`, no por fe (§2).
3. **`zPhysics` tenía 14 símbolos, no 9**: los `lbl_803F74xx` son **once**, no
   siete (faltaban `74CC`, `74D0`, `74E0`, `74E4`). El encargo se quedó corto
   porque `trypromo.py` recorta a dos errores.
4. **El `.bss` de GCC 2.9 tiene TRES grupos, y el orden lo decide el grupo, no el
   fichero** (§3). Es lo que sitúa `theMarker` al byte… y lo que demuestra que
   **`TextureInfo` tenía un constructor en el original**, cosa que nuestro árbol
   no puede reproducir (única deuda declarada, §6.1).
5. **`SPEECHFLOW_DISPLAY` está en `.rodata`: era `const`.** Cambiadas sus dos
   declaraciones a `extern const int` y definido **detrás de todos sus usos**
   (regla de la r31): 0 regresión.

---

## 1. Verificación del encargo — reproduce, y en `zPhysics` se queda corto

`trypromo.py` sobre el árbol de partida, con **todos** los errores
(`c31und_try.py` de la r31; el original imprime `err[:2]`):

| unidad | veredicto ANTES | errores | símbolos distintos |
|---|---|---|---|
| `zFe` | **ENLACE FALLA** | 12 | **3** |
| `zSpeech` | **ENLACE FALLA** | 318 | **4** |
| `zPhysics` | **ENLACE FALLA** | 140 | **14** (el encargo decía 9) |

Desglose por símbolo y por objeto que lo pide (`c32und2_tally.py`):

**`zFe`** — `theMarker` ×6 (nuestro `zFe.o`), `MovieTextureInfo` ×4 (nuestro
`zFe.o`), `_13MemoryCardImp.gEntryType` ×2 (`zPlatform.o` extraído).

**`zSpeech`** — `Singleton<SoundAI>::mInstance` ×300 (266 nuestros + 34 de
`zAI`/`zEAXSound2`/`zMain`/`zCamera`/`zLua`/`zSim` extraídos),
`SoundAI::mRefCount` ×10, `SPEECHFLOW_DISPLAY` ×4, `MUSICFLOW_DISPLAY` ×2
(`zEAXSound2.o`).

**`zPhysics`** — `Factory<const BehaviorParams&,Behavior,UCrc32>::Prototype::mHead`
×112 (**84 de `zPhysicsBehaviors.o` extraído**, 23 de `zAI.o`, 5 nuestros),
`LoaderBounds__FP6bChunk` y `UnloaderBounds__FP6bChunk` ×1 (`zMisc.o` extraído),
y **once** `lbl_803F74xx`: `748C 7490 7494 7498 74A8 74B8 74BC 74CC 74D0 74E0 74E4`.

**Ninguno era un duplicado** (regla 1 del encargo): busqué los nueve nombres por
valor y por dirección en `symbols.txt` y en el árbol antes de escribir nada. El
único duplicado que sí aparece es al revés y **no es mío**: §6.2.

---

## 2. Las nueve curas, con su comprobación de posición

Todas las ediciones **quedan puestas en `src/`**.

### 2.1 `zFe` — `theMarker` (`.bss:0x80472B18`, 4 B, global)

`uiRepSheetBounty.cpp:36` lo declaraba `extern unsigned int theMarker;` y **nadie
lo definía**. Regla 3 del encargo: sin inicializador → `.bss`.

    -extern unsigned int theMarker;
    +unsigned int theMarker;

**Cae al byte.** Nuestro `.bss` mapea con base `0x80472A34` (por `gMemcardSetup`):
`theMarker` en `0xE4` → **`0x80472B18`** y `FEPrintf_Buffer` en `0xE8` →
**`0x80472B1C`**, los dos exactos.

### 2.2 `zFe` — `MovieTextureInfo` (`.bss:0x80472A58`, 0x7C B, global)

`MoviePlayer.cpp:45` lo declara `extern TextureInfo MovieTextureInfo;` y
**`TextureInfo` es un tipo INCOMPLETO en todo el árbol** (sólo declaraciones
adelantadas en seis cabeceras; ni una definición). No se puede escribir
`TextureInfo MovieTextureInfo;`. Se emite la reserva con el **nombre de
ensamblador del original**, el mismo idioma que ya usa `MemoryCardImp.cpp` para
`_13MemoryCardImp.gEntryType`:

    int MovieTextureInfo_storage[0x7C / 4] __asm__("MovieTextureInfo");

Sale `.bss 124 B GLB` con alineación 4, que es la del objetivo (`0x80472A58` va
detrás de un objeto de 4 B en `0x80472A54`). **La posición NO cae bien**: §3 y
§6.1.

### 2.3 `zFe` — `MemoryCardImp::gEntryType` (`.data:0x8041BBB8`, 0xC B)

El valor leído del ELF es `{0x80472F1C, 0x80472F5C, 0x80472F9C}` =
**`{gSaveType0, gSaveType1, gSaveType2}`**, los tres arrays de `MemoryCard.cpp`.
Y la dirección va **pegada delante de `_10MemoryCard.s_pThis`**
(`MemoryCard.cpp:19`), así que la definición del original estaba ahí. Subidos los
tres arrays por encima de `s_pThis` y añadido:

    unsigned short *MemoryCardImp::gEntryType[3] = {gSaveType0, gSaveType1, gSaveType2};

**Cae al byte**: nuestro `.data` da `gCurrentSubtitler_ 0x5D8`, `gEntryType
0x5DC`, `s_pThis 0x5E8`; el objetivo `0x8041BBB4` / `0x8041BBB8` / `0x8041BBC4`.
Mismo desplazamiento `0x8041B5DC` en los tres.

### 2.4 `zSpeech` — `MUSICFLOW_DISPLAY` (`.data:0x80435E98`, valor 0)

Va pegado detrás de `gXMP_DOWNSTATE` (`MusicFlow.cpp:12`), así que ahí lo pongo:
`int MUSICFLOW_DISPLAY = 0;`. Sólo lo **lee** `SFXObj_Pathfinder.cpp`
(zEAXSound2), que lo declara `extern`.

### 2.5 `zSpeech` — `SoundAI::mRefCount` y `Singleton<SoundAI>::mInstance`

`.data:0x80435E9C` y `.data:0x80435EA0`, valor 0 los dos, **en ese orden y justo
detrás de `MUSICFLOW_DISPLAY`**. Puestos delante de
`int FORCE_VOICE_RANDOMIZATION = 0;` (`SoundAI.cpp:44`):

    int SoundAI::mRefCount = 0;
    template <> SoundAI *UTL::Collections::Singleton<SoundAI>::mInstance = NULL;

(la segunda línea es exactamente lo que expande `IMPLEMENT_SINGLETON`).

**Caen al byte**: nuestro `.data` `datapts 0x8F8 · spkrID911 0x8FC ·
gXMP_DOWNSTATE 0x900 · MUSICFLOW_DISPLAY 0x904 · mRefCount 0x908 · mInstance
0x90C` contra el objetivo `E8C · E90 · E94 · E98 · E9C · EA0`. Los seis con el
mismo desplazamiento.

### 2.6 `zSpeech` — `SPEECHFLOW_DISPLAY` estaba en `.rodata`: era `const`

`symbols.txt` lo da en **`.rodata:0x80407A7C`**, `scope:global`, valor 0, y
**pegado delante de `_7SoundAI.heat_cutoffs`** (`SoundAI.cpp:982`). En C++ un
`const` de ámbito de fichero tiene enlace interno, así que el original escribía
`extern const int`. Cambiadas las dos declaraciones (`PursuitFlow.cpp:14`,
`RoadblockFlow.cpp:14`) de `extern int` a `extern const int` y definido justo
delante de `heat_cutoffs`:

    extern const int SPEECHFLOW_DISPLAY = 0;

**El sitio es la regla de la r31**: los tres usos (`PursuitFlow`, `StrategyFlow`,
`RoadblockFlow`) están en ficheros ANTERIORES del TU, así que el inicializador no
es visible en el punto de uso y GCC no pliega la lectura. `pctsnap --cmp`:
**EMPEORAN: ninguna**.

**Cae al byte**: nuestro `.rodata` `SPEECHFLOW_DISPLAY 0x1C44` +
`_7SoundAI.heat_cutoffs 0x1C48` contra `0x80407A7C` + `0x80407A80`.

### 2.7 `zPhysics` — `LoaderBounds` / `UnloaderBounds` eran `static`

`.text:0x8021EA94` (0x84 B) y `.text:0x8021EB18` (0xE4 B), los dos
**`scope:global`** en el objetivo. En `Bounds.cpp:455/466` estaban **`static`**, y
quien los pide es la `LoaderTable`/`UnloaderTable` de `ResourceLoader.cpp`, que
está en **otra unidad** (zMisc). Es la regla 2 del encargo. Quitado el `static` de
las dos. **Las dos ya casaban al 100 %** (objdiff empareja por nombre aunque el
símbolo sea local) y siguen igual: `measure --cmp` +0 B / +0 funciones.

### 2.8 `zPhysics` — el `mHead` de la `Factory` de `Behavior`

`.data:0x8041F054`. Aplicada al árbol la línea que `ua` dejó probada en la r31,
detrás del `#include` de `Behavior.cpp` (donde el objetivo la emite: entre
`VehicleSystem::PAD_DEAD_ZONE` y `SceneryModel::mSceneryCount`):

    template <>
    UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype
        *UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype::mHead = NULL;

Cierra **112 de los 140 errores**, y 84 de ellos venían de `zPhysicsBehaviors.o`
**extraído**, que no habría enlazado nunca sin esto.

### 2.9 `zPhysics` — los once `lbl_803F74xx`, al FINAL de la unidad

Mismo caso que los 21 `lbl_803EBxxx` de zFoundation en la r31:
`PhysicsUpgrades.cpp:12-25` los declara **a mano** con el nombre del objetivo y
**nadie los define**; mientras la unidad no promocionaba los resolvía el `.o`
extraído. Valores leídos del ELF original (coinciden uno a uno con los
comentarios que ya había en el fuente):

| símbolo | valor | | símbolo | valor |
|---|---|---|---|---|
| `lbl_803F748C` | 0.0f | | `lbl_803F74BC` | 0.0f |
| `lbl_803F7490` | 1.0f | | `lbl_803F74CC` | 0.0f (`volatile`) |
| `lbl_803F7494` | 0.0f | | `lbl_803F74D0` | 1.0f (`volatile`) |
| `lbl_803F7498` | `"junk_upgrade"` (0xD B) | | `lbl_803F74E0` | 0.0f |
| `lbl_803F74A8` | `"part_upgrade"` (0xD B) | | `lbl_803F74E4` | 1.0f |
| `lbl_803F74B8` | 1.0f | | | |

**Van AL FINAL de `zPhysics.cpp`**, detrás de
`IMPLEMENT_SINGLETON(Smackable::Manager)` — la regla medida en la r31 (con el
inicializador visible en el punto de uso GCC pliega la carga y rompe la función).
`measure --cmp` **+0 B, +0 funciones**; `pctsnap --cmp` **EMPEORAN: ninguna**.

**Aviso de tamaño, como el de la r31:** `lbl_803F74BC` mide `0x10` en el objetivo
(el `0.0f` más la cadena `"<unknown>"` en `0x803F74C0`, que dtk agrupa bajo la
misma etiqueta). Sólo se referencia el primer flotante, así que defino un escalar;
el resto es dato del §4.

---

## 3. Descubrimiento: el `.bss` de GCC 2.9 tiene TRES grupos

Es lo que ha permitido colocar `theMarker` al byte y lo que **prueba** el §6.1.
Medido comparando el `.bss` de nuestro `zFe.o` con el rango del objetivo
(`0x804729B8` en adelante): **la correspondencia es exacta símbolo a símbolo**,
así que el mecanismo es el mismo en los dos lados. Y el orden **no es el del
fichero**: son tres grupos, y dentro de cada uno sí manda el orden del TU.

| grupo | qué entra | en zFe |
|---|---|---|
| **1** | estáticos **locales de función** (los `nombre.NNNN`), en orden de serie | `0x00`…`0x68` |
| **2** | objetos de ámbito de fichero **CON constructor** | `gNormal`, `gTint`, `gRapsheet`, `MessengerCreationTimer`, `gShapeMemoryAllocator`, `gMemcardCallbacks`, `gMemcardSetup` (`0x6C`…`0xB0`) |
| **3** | objetos de ámbito de fichero **SIN inicializador** (definición tentativa, diferida a `finish_file`) | `theMarker`, `FEPrintf_Buffer`, `gSaveType0/1/2` |

La prueba de que no es orden de fichero: `gMemcardSetup` sale de
`uiMemcardInterface.cpp`, que es el **último** `#include` de `zFe.cpp` (línea 64),
y aun así va **antes** de `FEPrintf_Buffer`, que sale de la línea 48. Y
`iSystem.29619` / `sMemcardImp.29621` salen de `MemoryCard.cpp` (línea 60) y van
en `0x38`/`0x4C`, delante de `gNormal` (línea 52).

**Consecuencia práctica**: para colocar un símbolo de `.bss` en su sitio no basta
con elegir el fichero — hay que elegir el **grupo**, y el grupo lo decide la forma
de la declaración (`static` de función / con constructor / sin inicializador). Es
la versión `.bss` de la regla «`int X = 0;` va a `.data` y `int X;` a COMMON».

---

## 4. Lo que bloquea ahora las tres: dato que falta, y cuánto

Veredicto de `scripts/trypromo.py` **antes y después**, y el DOL sección a sección
(`c31und_doldiff.py`; secciones 9 = `.rodata` y 10 = `.data` de la cabecera):

| unidad | ANTES | DESPUÉS | tamaño DOL | `d.rodata` (data2) | `d.data` (data3) | total |
|---|---|---|---|---|---|---|
| `zFe` | ENLACE FALLA (12) | **DOL ROTO** `1073fe5a1273` | 4.538.464 | **−3.296** | **−128** | **−3.424** |
| `zSpeech` | ENLACE FALLA (318) | **DOL ROTO** `050e84349fc2` | 4.540.192 | **−1.632** | **−64** | **−1.696** |
| `zPhysics` | ENLACE FALLA (140) | **DOL ROTO** `7867d32cb7d5` | 4.540.096 | **−1.632** | **−160** | **−1.792** |
| las tres juntas | — | **DOL ROTO** `1c873dae301b` | 4.534.912 | | | −6.976 |
| **control, sin promocionar** | DOL OK | **DOL OK** (`9619ba57…`, 4.541.888 B) | | | | |

**El `.text` no aporta ni un byte de diferencia de tamaño en las tres**: `text0`,
`text1` y `text2` salen del mismo tamaño en las tres promociones. En `zPhysics`
además `text2`/`data0`/`data1` se **corren** (−8 / −0x10 / −0x10) sin cambiar de
tamaño: eso viene de una sección intermedia del ELF (`.ctors`/`extab`), no del
código.

**No me he metido ahí**: el pool `$LC` es de otro agente y el orden del `.text` de
un tercero.

---

## 5. Y el frente de datos CON NOMBRE queda cerrado en las tres

Con las nueve definiciones puestas, `c31ua_datadiff.py` + clasificación
(`c32und2_named2.py`) sobre los símbolos de datos que el `.o` extraído define y el
nuestro no:

| unidad | símbolos | bytes | `lbl_` | `gap_`/`pad_` | estático `.NNNN` | **OTRO CON NOMBRE** |
|---|---|---|---|---|---|---|
| `zFe` | 803 | 11.134 | 550 / **10.441 B** | 240 / 617 B | 13 / 76 B | **0 / 0 B** |
| `zSpeech` | 660 | 8.578 | 510 / **7.599 B** | 128 / 314 B | 22 / 665 B | **0 / 0 B** |
| `zPhysics` | 391 | 5.271 | 306 / **4.068 B** | 60 / 615 B | 25 / 588 B | **0 / 0 B** |

Y los `.NNNN` **no faltan, están renombrados** (la regla de `ua` en la r31),
verificado con tres muestras del mismo tamaño exacto:

    zPhysics  tunings.28622   (28 B)  <->  nuestro tunings.20323   (28 B)
    zPhysics  fix.31644       (64 B)  <->  nuestro fix.23806       (64 B)
    zSpeech   speed_test.28362 (44 B) <->  nuestro speed_test.23882 (44 B)

**Lectura**: lo único que le falta de datos a estas tres unidades es el **pool
anónimo** (`lbl_` contra nuestro `$LC`, §2/§3 del brief) y el relleno. No queda ni
una definición con nombre que escribir.

---

## 6. Deuda declarada y avisos

### 6.1 `MovieTextureInfo` cae en el grupo 3 del `.bss` y el objetivo lo tiene en el 2

Único punto en el que no llego al byte, y **está demostrado por qué**. El objetivo
pone `MovieTextureInfo` en `0x80472A58`, **entre `MessengerCreationTimer` y
`gShapeMemoryAllocator`**, o sea en el **grupo 2** del §3 — el de los objetos con
constructor. Nuestra definición no tiene inicializador, así que cae en el grupo 3
y aterriza en `0x4E8` en vez de `0xA0`.

**Eso prueba que `TextureInfo` tenía un CONSTRUCTOR en el original**, y nuestro
árbol no tiene la clase (sólo declaraciones adelantadas). No es reproducible sin
ella, y darle un constructor postizo añadiría una llamada al `_GLOBAL_.I.` que el
original no tiene en ese sitio.

**Coste medido: una permutación pura, cero bytes.** El tamaño del `.bss` del DOL
no cambia (`698168` en los dos lados), `gSaveType0` cae en su dirección exacta
(`0x564` → `0x80472F1C`) y lo único que se desplaza son los **7 símbolos** entre
`gShapeMemoryAllocator` y `FEPrintf_Buffer`, −0x7C cada uno.

**Vedada la vía del estático local**: pensé colocarlo como
`static … __asm__("MovieTextureInfo")` dentro de una función de `MoviePlayer.cpp`,
pero el §3 lo descarta sin gastar una compilación — los estáticos locales son el
**grupo 1**, que va aún más lejos del sitio bueno.

### 6.2 `Tweak_TuningAero` / `Tweak_UseTweakerTunings`: duplicado PREEXISTENTE

Al enlazar `zPhysics` el enlazador avisa (`L0019`, **aviso**, no error) de que los
dos están **doblemente definidos**: los emite a mano un `asm()` de
`zMiscSmall.cpp:561-570` y además los emite nuestro `zPhysics.o`. El enlazador se
queda con el de `zMiscSmall`.

`splits.txt` le da a zMiscSmall `.data 0x8041ED7C..0x8041EFE8`, que **incluye**
`Tweak_TuningAero` (`0x8041EFDC`) y `Tweak_UseTweakerTunings` (`0x8041EFE0`),
mientras zPhysics empieza en `0x8041EFE8`. O sea: la atribución dice zMiscSmall y
nuestro fuente lo emite en zPhysics.

**No es mío y no lo he tocado** (tocaría `splits.txt`, vedado). Es candidato a
explicar parte de los −160 B de `data3` de zPhysics: son 8 B que nuestro objeto
reserva y el enlazador descarta. Para quien ataque el §4: `dupdata.py`.

### 6.3 Los nombres `lbl_` y la posición siguen siendo un constructo nuestro

Como en la r31: los once `lbl_803F74xx` se definen **con el nombre que dtk le da a
la etiqueta anónima del objetivo** y **en un sitio elegido para que GCC no
pliegue** (el final del TU), no en el fichero al que pertenecen. Son datos, no
ensamblador, y el valor sale del ELF original, pero hay que sustituirlos cuando se
sepa qué sentencia del original los emitía. Lo mismo vale para el
`MovieTextureInfo_storage` del §2.2.

### 6.4 Aviso de convivencia: dos carreras medidas con otro agente

1. **`globalini.py` da 45 correctas + 1 «no emitimos ninguno»** (la r31 dejó 46).
   La que falta es **`zFEng`** (objetivo `_GLOBAL_.I.FEDirection_Message`,
   nosotros ninguno). **No es mía**: nunca he compilado zFEng y su `.o` es de las
   21:06, catorce minutos después del mío. Sigue en pie la mala conocida
   (`gc_interface` de realmemcard).
2. **El control estuvo `ENLACE FALLA` durante cuatro minutos** con
   `_SetMsgOptions__Q26Realmc9GCMessagei` / `_LcGetSlotString…` indefinidos en
   `trctasks.o`. Otro agente estaba editando
   `realmemcard/…/impl/gc_interface_impl.h` **en ese instante** (mtime 21:19;
   `_SetMsgOptions` está definido `inline` en `gc_memcard_interface_impl.cpp`: el
   frente «en clase = inline en GCC 2.9»). Repetido a las 21:20: **DOL OK**. Es el
   §3 de `HERRAMIENTAS.md` al pie de la letra.

---

## 7. Verificación

- `build_direct.py` de las unidades tocadas: **6 ok, 0 fallidas** (zFe, zFe2,
  zFeOverlay, zSpeech, zPhysics, zPhysicsBehaviors).
- **`measure.py --cmp`, base y medida SEGUIDAS en cada caso** (para zFe se tomó la
  base revirtiendo mis tres ediciones, recompilando y volviendo a poner las copias
  del scratchpad, para que la base fuese la del árbol sin mí):

  | unidad | base → medida | resultado |
  |---|---|---|
  | zFe + zFe2 + zFeOverlay | 562.060/566.156 B → 562.060/566.156 B | **+0 B, +0 funciones, 0 unidades cambian** |
  | zSpeech | 177.976/178.888 B → idem | **+0 B, +0 funciones** |
  | zPhysics + zPhysicsBehaviors | 376.836/382.300 B → idem | **+0 B, +0 funciones** |

- **`pctsnap.py --cmp`** en las tres: **EMPEORAN: ninguna** · MEJORAN: 0.
- **`audit.py`, DOS pasadas** en `zFe`, `zSpeech`, `zPhysics`,
  `zPhysicsBehaviors`, `zFe2`, `zFeOverlay`: **0 FALLA** en las doce pasadas.
- Estado final por unidad: `zFe` 173.400/174.200 B (918/921 fn), `zSpeech`
  177.976/178.888 B (701/703 fn), `zPhysics` 143.004/146.124 B (717/718 fn) —
  **las mismas cifras con las que empezó la r31**.
- `globalini.py`: el `_GLOBAL_.I.` de **mis seis unidades casa por nombre con el
  objetivo** (comprobado uno a uno; ninguna de mis definiciones se lo ha robado —
  la trampa de la r31 §2.5).
- `keepchk.py`: **758 ok, 19 RANCIAS, 0 sin objeto** — idéntico a como empezó.
- `config/GOWE69/*`, `configure.py` y `build.ninja`: `git status` **vacío**.
- Los `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- **Control (sin promocionar nada) con todas mis ediciones puestas: `DOL OK`,
  4.541.888 B.** No dejo ningún fichero que rompa el enlace.
- **Sin commit.** Sin ensamblador de instrucciones. Sin un solo `asm()` de código:
  los dos `__asm__("nombre")` que uso son **nombres de símbolo**, no
  instrucciones.

### 7.1 Ficheros de `src/` que dejo modificados

| fichero | qué | § |
|---|---|---|
| `Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetBounty.cpp` | `theMarker` definido | 2.1 |
| `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.cpp` | reserva de `MovieTextureInfo` | 2.2 |
| `Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.cpp` | `MemoryCardImp::gEntryType` + los 3 arrays subidos | 2.3 |
| `Speed/Indep/Src/Speech/MusicFlow.cpp` | `MUSICFLOW_DISPLAY` | 2.4 |
| `Speed/Indep/Src/Speech/SoundAI.cpp` | `mRefCount`, `mInstance`, `SPEECHFLOW_DISPLAY` | 2.5, 2.6 |
| `Speed/Indep/Src/Speech/PursuitFlow.cpp` | `extern int` → `extern const int` | 2.6 |
| `Speed/Indep/Src/Speech/RoadblockFlow.cpp` | `extern int` → `extern const int` | 2.6 |
| `Speed/Indep/Src/Physics/Common/Bounds.cpp` | quitado `static` a los dos loaders | 2.7 |
| `Speed/Indep/SourceLists/zPhysics.cpp` | `mHead` + los once `lbl_803F74xx` al final | 2.8, 2.9 |

---

## 8. Lo que haría el siguiente

1. **El pool anónimo de las tres** (§5): `zFe` 10.441 B, `zSpeech` 7.599 B,
   `zPhysics` 4.068 B en `lbl_`. Es el ÚNICO frente de datos que les queda, y
   ahora se sabe que no hay nada más escondido detrás.
2. **`zSpeech` es la que más cerca está** (−1.696 B), luego `zPhysics` (−1.792 B)
   y `zFe` la más lejos (−3.424 B). `zPhysics` tiene además el duplicado del §6.2,
   que son 8 B gratis en cuanto se decida de quién son.
3. **`TextureInfo`** (§6.1): si aparece la clase real, `MovieTextureInfo` cae en su
   sitio y el `.bss` de zFe queda exacto.
4. **NO volver** a comprobar si estos nueve símbolos son duplicados: están
   buscados por nombre, por valor y por dirección.

## 9. Herramientas (scratchpad, prefijo `c32und2_`)

| guion | qué hace |
|---|---|
| **`c32und2_tally.py <unidades…>`** | pasa `c31und_try.py` y **agrupa los errores por (símbolo, objeto que lo pide)** con su cuenta. Convierte «318 errores» en «4 símbolos» en un segundo |
| **`c32und2_named2.py <unidades…>`** | los símbolos de datos que el objetivo define y nosotros no, **clasificados en `lbl_` / `gap_`-`pad_` / estático `.NNNN` / OTRO CON NOMBRE**. El último cubo es el único accionable, y sale a 0 en las tres |
| `c32und2_raw.py <unidad>` | el enlace con la salida **cruda** del enlazador, para ver los **avisos** `L0019` que `c31und_try.py` filtra (sólo mira líneas con «error») |
| `c32und2_gi.py` | qué unidad concreta es la que `globalini.py` cuenta como «no emitimos ninguno» |

Reutilizados de la r31: `c31und_try.py`, `c31und_keep.py`, `c31und_doldiff.py`,
`c31und_syms.py`, `c31und_val.py`, `c31ua_datadiff.py`.

**Disco**: 19 GB libres al empezar, 18 GB al acabar (el gasto es de otros
agentes: los `mnd_r_zPhysicsBehaviors_*.json` de 18 MB no son míos). Mis
temporales ocupan **14 MB**: los cuatro guiones, tres `.dol` y once `.json` de
medida. Borrados los tres `.elf` de 16 MB que deja `c31und_keep.py`.
