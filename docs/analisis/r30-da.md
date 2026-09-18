# Ronda 30 — `.data`: el 88 % del dato VIVO era un inicializador que faltaba

Encargo: los **7.584 B de `.data` que faltan** en las 32 SourceLists, de los que
**4.190 estaban VIVOS**.

Resultado: **3.686 B de dato VIVO escritos** (4.190 → **504**, un 88 %), el
hueco de tamaño de `.data` de **7.584 → 4.332 B**, y **+1.992 B de `.rodata`
nueva y correcta** que arrastran las cadenas de esas tablas.
`.text` **sin una sola regresión**: `measure.py` da **+448 B, +1 función, 0
unidades que bajan**; `audit.py` **0 FALLA** en las 13 unidades tocadas;
`main.dol: OK` (`9619ba57…`).

---

## 0. Las cinco frases

1. **La causa raíz del frente es UNA sola y vale 1.328 B: el inicializador
   explícito.** GCC 2.9 manda `int X = 0;` a `.data` y la definición tentativa
   `int X;` a COMMON/`.bss`. El original inicializaba; nosotros no. El bucket
   COMMON+`.bss` pasa de **1.328 B a 4 B**.
2. **Y hay un segundo bucket igual de mecánico: 3.554 B de símbolos que
   declaramos `extern` y NO DEFINE NADIE** (`UND`). No es que estuvieran en otra
   sección: es que la definición no existe en el árbol. Se escriben con el valor
   que tiene el objetivo.
3. **Dos tablas nuestras llevan un `const` que el original no tiene**
   (`accessorTable` de zLua, 192 B; `kObjectTemplateKey` de zGameplay, 144 B):
   `const` las manda a `.rodata`, y el objetivo las tiene en `.data`. Quitar el
   `const` mueve 336 B a su sitio. **Baja `d.rodata` y sube `d.data` — es un
   ACIERTO, no una regresión.**
4. **Dos fronteras de `splits.txt` están mal, y se demuestra con el `.text`**
   (§5): 424 B de zFe son de zFe2 (`FeMinimap.cpp` + `FeRadarDetector.cpp`,
   cuyo código está en el rango de zFe2) y 32 B de zEAXSound2 son de zEAXSound
   (`SFXCTL_Helicopter.cpp`, cuyo código está en el rango de zEAXSound). Son
   suma cero para el total, pero **sin arreglarlas esas cuatro unidades no
   pueden promocionar nunca**.
5. **Lo que queda de VIVO son 504 B y casi todos son `lbl_` de 4 B**: flotantes
   *tweakable* que el original tiene como global escribible en `.data` y
   nosotros escribimos como literal dentro de la expresión. 364 de esos 504 son
   el bloque de zFe que arregla la frontera del §5.

---

## 1. La medida, antes y después

`c29ro_faltan.py --sec .data` (dato del objetivo que no reproducimos, partido en
VIVO/MUERTO) y `c30da_init.py` (dónde está cada símbolo del `.data` objetivo en
nuestro objeto):

| | antes | ahora |
|---|---|---|
| `.data` VIVO que falta | **4.190 B** | **504 B** |
| `.data` MUERTO que falta | 1.993 B | 1.993 B |
| hueco de TAMAÑO de `.data` (suma de `d.data` negativos) | **7.584 B** | **4.332 B** |
| símbolos en COMMON que el objetivo tiene en `.data` | 512 B | **0 B** |
| símbolos en `.bss` que el objetivo tiene en `.data` | 816 B | **0 B** |
| símbolos en `.rodata` que el objetivo tiene en `.data` | 32 B | 0 B |
| símbolos AUSENTES (`UND` o inexistentes) | 3.554 B | 1.580 B |

Por unidad, el `d.data` de `seccdiff.py`:

| unidad | antes | ahora | | unidad | antes | ahora |
|---|---|---|---|---|---|---|
| `zSpeech` | +780 | +936 (VIVO 1.216 → **0**) | | `zLua` | −496 | −304 |
| `zFe` | −1.548 | −568 | | `zGameplay` | −344 | −188 |
| `zFe2` | −580 | +312 | | `zTrack` | −480 | −436 |
| `zEAXSound` | −912 | −384 | | `zSim` | −256 | −244 |
| `zFeOverlay` | −748 | −96 | | `zPhysics` | −208 | −192 |
| `zMain` | −160 | −96 | | `zCamera` | −276 | −252 |

Y `d.rodata` sube donde nacen las cadenas nuevas: `zFe` −1.928 → **−680**,
`zFe2` −4.032 → **−3.408**, `zEAXSound` −944 → **−496**. Baja, correctamente, en
las dos del `const`: `zLua` −672 → −864 y `zGameplay` −1.776 → −1.936.

---

## 2. Los tres mecanismos, con su cifra

### 2.1 `int X = 0;` va a `.data`; `int X;` va a COMMON — 1.328 B

Es el simétrico exacto de lo que `globalini.py` ya sabía del `_GLOBAL_.I.`.
Verificado en el árbol antes de repartir nada: en `FeMinimap.cpp`,
`int MinimapShowNonPursuitCops = 0;` sale en **`.data`** de nuestro `zFe2.o`, y
`MovieVolumeArray[40]` sin inicializador sale en `.bss`.

Aplica a tres formas:

- **miembro estático de clase sin inicializador** → COMMON:
  `char FEAnyMovieScreen::MovieFilename[64];` → `= "unknown"` (el objetivo lleva
  literalmente `unknown` en los tres de zFe y los tres de zFe2: 384 B).
- **`static` de fichero** → `.bss`: `static MovieNameMap sMovieNameMap[42];`,
  `MoviePlayer *gMoviePlayer;`, `static SMSMessage *the_msg;`…
- **definición tentativa global** → COMMON.

**Trampa medida**: en `zSpeech` el inicializador nuevo se coló DELANTE de
`VALIDATE_SED_GENERATE` y el símbolo `_GLOBAL_.I.` cambió de nombre: −44 B y −1
función. Se arregla poniendo la definición nueva DETRÁS del primer global
público con inicializador. **Comprueba `globalini.py` después de cada tanda**
(ahora: 46 unidades con el nombre correcto, antes 45).

**Trampa 2**: el generador convierte el `extern` que encuentra primero, y en
tres sitios ya había una definición más abajo → *redefinition*. Pasó con
`FEDatabase` (Localize.cpp contra FEDatabase.cpp), `gGameBreakerCamera`
(CameraAI.cpp contra CDActionDrive.cpp) y el trío
`CameraDebugWatchCar/mToggleCar/mToggleCarList` (dos veces en el mismo fichero).
La regla que sale de las tres: **la definición va donde el ORDEN de offsets del
objetivo la pone**, no en el primer `extern`.

### 2.2 Símbolos `UND`: la definición no existe en el árbol — 3.554 B

`c30da_und.py` los separa de los que están en otra sección. En `zEAXSound` eran
**750 de 758 B**: `g_pEAXSound`, `NullPointer`, `g_fMasterSFXVolume`,
`g_CtlStateActions[18]`, `g_DataPaths[13]`, `RUN_SOUND_STATE`… todos declarados
`extern` en `EAXSound.cpp` y sin dueño.

Y quince `s_TypeInfo` de `SFXCTL_*` que faltaban enteros: el árbol tiene el macro
`TYPEINFO_IMPLEMENT` en `SndBase.hpp` y 47 clases de `zEAXSound2` lo usan vía
`DEFINE_CREATABLE`, pero las quince de `zEAXSound` escriben `GetTypeInfo()` y
`CreateObject()` a mano y **nadie define el dato**. Se escribe la expansión del
macro sin tocar las funciones:

```cpp
SndBase::TypeInfo SFXCTL_AccelTrans::s_TypeInfo =
    {(int)0x00020030, "SFXCTL_AccelTrans", &SFXCTL::s_TypeInfo, SFXCTL_AccelTrans::CreateObject};
```

### 2.3 `const` de más manda la tabla a `.rodata` — 336 B

`static const struct {…} accessorTable[]` (zLua, 192 B) y
`static const unsigned int kObjectTemplateKey[…]` (zGameplay, 144 B) salen en
`.rodata`; el objetivo las tiene en `.data`. Quitado el `const`, `d.data` de
zLua pasa de −496 a −304 y el de zGameplay de −344 a −188, y `d.rodata` baja lo
mismo. **`measure.py`: +0 B, +0 funciones.**

---

## 3. Las tablas transcritas — dato real, no adivinado

Todo esto sale del ELF original con los punteros RESUELTOS
(`c30da_tabla.py <unidad> <símbolo> <stride>`), no de una conjetura:

| tabla | B | dónde | qué era |
|---|---|---|---|
| `MovieVolumeArray` | 304 | `MoviePlayer.cpp` | 38 entradas `{"MOVIES\\…", volumen}`; el bucle ya iteraba `i < 0x26` |
| `sMovieNameMap` | 336 | `FEObjectCallbacks.cpp` | 42 `{"ealogo", 0}`… con el id = índice |
| `MarkerSelectInfos` | 588 | `uiMarkerSelect.cpp` | 21 × 7 hashes, con los enumeradores `MARKER_*` |
| `LanguageInfoTable` | 240 | `Localize.cpp` | 10 idiomas con sus dos `LANGUAGES\\*.BIN` |
| `EuropeanFontNameInfo` | 108 | `Localize.cpp` | 3 × 8 hashes de fuente |
| `FontSizeInfoTable` | 72 | `Localize.cpp` | 9 `{hash, tamaño}` |
| 10 × `bPrintfLocaleInfo` | 30 | `Localize.cpp` | `{'.' , ',', 3}` por idioma |
| `ColourHashToSoundColourMap` | 104 | `SoundAI.cpp` | 13 `{hash, bit}` |
| `g_CtlStateActions` / `g_DataPaths` | 124 | `EAXSound.cpp` / `EAXAemsManager.cpp` | 18 máscaras y 13 rutas `sound\…` |
| 15 × `s_TypeInfo` | 240 | `sfxctl/*.cpp` | §2.2 |
| 6 × `MovieFilename`/`PackageFilename` | 384 | zFe y zFe2 | `= "unknown"` |
| 139 × `InterfaceId` de Csis | 1.112 | `EAXCharacter.cpp` | §4 |

Y una **corrección de contenido** que el comparador no veía: `milestoneNames[9]`
de `GManager.cpp` tenía nueve nombres DISTINTOS de los del objetivo
(`"cost_to_state"` contra `"cost_to_state_in_pursuit"`, etc.). No salía como
diferencia porque las nueve palabras son reubicaciones y `c29ro_faltan.py`
enmascara los bytes con reubicación: **una tabla de punteros SIEMPRE le sale
"OK"**. Es una ceguera de la herramienta que hay que anotar.

---

## 4. Los 139 `InterfaceId` de Csis: 1.112 B que hay que TRANSCRIBIR

`EAXCharacter.cpp` define 139 `InterfaceId X = {"Nombre", 0, 0};`. El objetivo
tiene `{"Nombre", 0x5BA7, <crc>}`: `systemCrc` es **0x5BA7 en las 139** y
`interfaceCrc` va por nombre.

**No sale de ningún CRC16 estándar.** `c30da_crcbf.py` barre **los 32.768
polinomios impares × 2 inits × refin × refout × 2 xorout = 262.144
combinaciones** contra cinco pares (nombre, crc) conocidos y da **cero
coincidencias**; tampoco casan CRC32 truncado (alto o bajo) con cinco polinomios
ni `bStringHash`. Son valores que producía la herramienta CSIS al generar el
fichero, y en el árbol no queda ni la herramienta ni el algoritmo: se
transcriben, y así está declarado en el guion.

Tras hacerlo, **`zSpeech` queda con `f.VIVO = 0` y `f.MUERTO = 0`**: es la única
SourceList cuyo `.data` reproducimos entero.

---

## 5. Dos fronteras de `splits.txt` mal puestas — PROPUESTA, no aplicada

No he tocado `config/GOWE69/*`, `splits.txt` ni `configure.py`. Las dos se
demuestran con el `.text`, que sí sabemos dónde está.

### 5.1 zFe / zFe2 — 424 B

`splits.txt` da a **zFe** `.data 0x8041B5CC..0x8041BE08`. Los últimos 424 B de
ese rango son:

    0x8041BC60  TWK_RadarDetectorMinThreshold   (FeRadarDetector.cpp)
    0x8041BC64  RadarDetector::mStaticRange     (FeRadarDetector.cpp)
    0x8041BC68  gChoppedMiniMapManager          (FeMinimapStreamer.cpp)
    0x8041BC84  MinimapShow*/Pivot*/DispX/MaxSpeed  (FeMinimap.cpp)
    0x8041BCA0  Minimap::kGameplayIconInfo, 0x168 B (FeMinimap.cpp)

y el **código** de esos tres ficheros está en el rango de **zFe2**
(`__13RadarDetector…` = 0x80142E58, `__7MinimapPCci` = 0x801442D8; zFe2 `.text`
= 0x80142AC0..0x8017FE2C). Además el orden interno (RadarDetector →
MinimapStreamer → Minimap → `RaceOverFinishStrings` de FeRaceOverMessage, que ya
es el primer símbolo del rango de zFe2) coincide **exactamente** con el orden de
la SourceList de zFe2.

    PROPUESTA:  zFe   .data  end:0x8041BE08  ->  end:0x8041BC60
                zFe2  .data  start:0x8041BE08 -> start:0x8041BC60

Efecto medido sobre los objetos de hoy: `zFe` −568 → **−144** y `zFe2` +312 →
**−112** (suma constante: es reatribución). Los 136 B de
`gap_06_8041BBD8_data` se quedan sin dueño, que es lo que ya son.

### 5.2 zEAXSound / zEAXSound2 — 32 B

`SFXCTL_Helicopter.cpp` es el **último** `#include` de `zEAXSound.cpp` y su
código está en el rango de zEAXSound (`CreateObject__16SFXCTL_3DHeliPosUi` =
0x800C47C8, rango 0x800A6128..0x800CAF5C). Sus dos `s_TypeInfo` están en
0x80417D88 y 0x80417D98 — **los 32 primeros bytes del rango de zEAXSound2**.

    PROPUESTA:  zEAXSound   .data  end:0x80417D88  ->  end:0x80417DA8
                zEAXSound2  .data  start:0x80417D88 -> start:0x80417DA8

### 5.3 Un tercero que NO propongo, sólo apunto

`_13MemoryCardImp.gEntryType` está en 0x8041BBB8 (rango `.data` de **zFe**,
entre `SubTitler::gCurrentSubtitler_` y `MemoryCard::s_pThis`) y `gSaveType0..2`
en 0x80472F1C (rango `.bss` de **zFe**), pero las funciones
`…__13MemoryCardImp` están en 0x8027217C, dentro del `.text` de **zPlatform**, y
`MemoryCardImp.cpp` está hoy en la SourceList de zPlatform. Los dos rangos no
pueden ser ciertos a la vez con un solo TU: o el original partía el fichero, o
una de las dos fronteras está mal. **No lo he resuelto y no he tocado nada.**

---

## 6. `satospkr`: no le falta dato, le falta un NOMBRE

`c29lk_ready.py` lo daba «a una frontera» con `*** TODO IGUAL ***`. Medido:

- `.text` idéntico (1.796 B), `.bss` idéntico, y **`.rodata` byte a byte
  idéntica**: los 88 B salen con el mismo hexadecimal en los dos objetos.
- `trypromo.py satospkr` → **ENLACE FALLA**:
  `stagpat.c(1) : error: L0039: Reference to undefined symbol lbl_80412CC2`.

`lbl_80412CC2` son 6 B en el offset 0x52 de la `.rodata` de satospkr, que
**nosotros sí emitimos** pero como `$LC` anónimo; el objeto extraído lo exporta
con nombre y `stagpat.o` lo referencia. Es el patrón de `c29ro_prestado.py`.

Probado `trypromo.py satospkr stagpat` (los dos juntos): **enlaza, pero DOL ROTO
(5ebe0146…)** — `stagpat` emite 16 B de `.rodata` que su rango no tiene y le
falta su `.data` de 4 B.

**Conclusión: `satospkr` no está a una frontera de datos, está a un símbolo
exportado**, y no se puede dar nombre a una entrada del pool de constantes sin
escribir el dato a mano. Ninguna de las 26 unidades de esa lista se desbloquea
con el trabajo de esta ronda: son todas de middleware y el frente de `.data`
estaba en las SourceLists.

---

## 7. Herramientas nuevas (scratchpad, prefijo `c30da_`)

| guion | qué hace |
|---|---|
| **`c30da_init.py`** | el frente entero en una tabla: para cada símbolo del `.data` objetivo, si lo tenemos en COMMON / `.bss` / `.rodata` / AUSENTE / con otro serial `.NNNN`. **Es el que dice cuánto trabajo queda y de qué clase.** |
| **`c30da_gen2.py`** | escribe el inicializador: localiza la declaración en el fichero de la SourceList, le quita el `extern` y le pone el valor del objetivo (flotante si el tipo lo dice, cadena si la reubicación apunta a una, `nullptr`, `true/false`…). `--aplica`, `--salta`, `--solo` |
| `c30da_gen.py` | la versión previa, sólo para `extern` de línea completa |
| **`c30da_tabla.py`** | vuelca una tabla del `.data` objetivo con los punteros **resueltos contra el ELF** (cadena si lo es). Es lo que convierte transcribir en copiar |
| **`c30da_donde.py`** | para cada símbolo, en qué fichero de la SourceList está su `extern` y **en qué posición de la lista** — con eso el orden del `.data` sale solo |
| `c30da_und.py` | separa `UND` (se escribe) de «no aparece» (otra unidad, u otro nombre) |
| `c30da_map.py` | el `.data` objetivo símbolo a símbolo contra el nuestro |
| `c30da_ti.py` / `c30da_ti_apply.py` | los `s_TypeInfo` que faltan, generados y aplicados |
| `c30da_crcbf.py` | el barrido de 262.144 CRC16 del §4 |
| `c30da_csis.py`, `c30da_localize.py`, `c30da_marker.py`, `c30da_zfe.py`, `c30da_zfe2b.py`, `c30da_lote3.py` | los lotes de edición, cada uno con su `assert count == 1` |

---

## 8. Verificación

- `build_direct.py` completo: **33 ok, 0 fallidas**.
- `measure.py --cmp` base→final: **+448 B, +1 función, 0 unidades bajan**
  (97,9628 % → 97,9742 %, 18.336 → 18.337 funciones).
- `audit.py` en las 13 unidades tocadas (zSpeech, zFe, zFe2, zEAXSound, zLua,
  zGameplay, zFeOverlay, zCamera, zSim, zPhysics, zTrack, zAI, zMain):
  **0 FALLA**, 9.690 funciones auditadas.
- `globalini.py`: **46** unidades con el `_GLOBAL_.I.` correcto (antes 45).
- **`c29ro_link.py` CONTROL: `DOL OK`.**
- **No dejo ningún fichero que rompa el enlace.** No he tocado `configure.py`,
  `config/GOWE69/*`, `splits.txt`, `scripts/`, ni los `#if defined(__ANDROID__)`.

---

## 9. Lo que NO he probado

1. **Los 1.993 B de `.data` MUERTA.** Concentrados en `zLua` 300, `zAI` 244,
   `zSim` 200, `zFoundation` 176, `zWorld2` 172, `zPhysics` 140. Son `gap_` sin
   nombre: el equivalente en `.data` del dato muerto de la r29, y el banco de
   formas de §2 de aquel informe no vale aquí porque **estos ocupan sitio**: hay
   que emitir una variable con el tamaño exacto, y no sé de qué eran.
2. **Los 504 B de VIVO que quedan.** 364 se van solos con la frontera del §5.1.
   De los otros 140, **136 son `lbl_` de 4 B con un flotante** (`zTrack`
   0x80436030 = 0x470f…, `zAI` 0x804152C0 = 85.0f, `zPhysics`, `zCamera`,
   `zMain`, `zSim`, `zPlatform`): el original los tenía como global escribible y
   nosotros como literal dentro de la expresión. Se cierran uno a uno leyendo la
   función que los usa; **no he hecho ninguno**.
3. **`zFeOverlay`: `phys_type.30541` (28 B) y dos cadenas de `.data`**
   («Waiting for SN Debugger», «<< libsn version %d >>», 52 B). Las dos cadenas
   **no existen en el árbol** — son del arranque de libsn, y en el objetivo están
   en `.data` (arrays escribibles, no `const char *`).
4. **`zFoundation`: `SN_DSI`/`SN_ISI`/`SN_ALIGNMENT`/`SN_FPE`**, 16 B, los cuatro
   con valor **1**. Los macros de `libsn.h` que el árbol tiene
   (`SN_LEAVE_*_FOR_OS()`) los ponen a **0**, así que el original usaba otra
   variante. Van en el offset 0 de zFoundation, o sea en el primer fichero de la
   lista (`UBezierLite.cpp`), lo que no me cuadra: no lo he escrito.
5. **Los estáticos de clase sueltos que aún son `UND`**: `LuaRuntime::mObj`,
   `LuaPostOffice::fObj`, `LuaBindery::fObj` (zGameplay), `SoundAI::mRefCount` y
   `Singleton<SoundAI>::mInstance` (zSpeech), `cSTICH_PlayBack::mSampleRefSlotPool`
   y `mStitchSlotPool` (zEAXSound), `CustomizeParts::TexturePackLoaded` y tres
   más de zFeOverlay, `EAGL4Anim::MatrixMultiply` (zEagl4Anim). Son 4 B cada uno,
   ~48 B en total, y cada uno hay que ponerlo en su fichero a mano.
6. **`HudResourceManager::LoadingResourcesForHudType`** quedó con `= PHT_NONE`;
   no he comprobado si el objetivo distingue ese cero de otro enumerador.
7. **El ORDEN dentro de `.data` de las unidades con huecos muertos.** En
   `zEAXSound` las definiciones nuevas van donde estaba el `extern` (bloque de
   cabecera de `EAXSound.cpp`), no en el orden de offsets del objetivo. Da el
   valor y el tamaño correctos, **no la posición**. Como esas unidades tienen
   ~600 B de `gap_` que no sabemos escribir, la posición exacta no es alcanzable
   hoy y no he perdido tiempo en ella. En `zFe`, `zFe2` y `zSpeech` sí seguí el
   orden del objetivo.
8. **Las secciones `.sdata`/`.sdata2`.** `c29ro_faltan.py` las acepta pero
   ninguna SourceList las tiene; el middleware sí, y no lo he mirado.
9. **Las 26 unidades de `c29lk_ready.py`**: sólo he medido `satospkr` (§6).
