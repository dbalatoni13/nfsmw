# r66 — lote `juego-resto`: andamios de código de juego que NO son «código que falta»

**Encargo**: dejar fuente legítima. Clases del lote: PERMUTACION, REPARTO, IDENTICO y
ESTRUCTURA con una local de más (la nuestra). Las de ESTRUCTURA con locales que sólo
tiene el original (`CDActionDrive::Update`, `TrackStreamer::HandleLoading`,
`QuickGame::CreateCars`, `EPlayRaceNIS`) son de `juego-oculto` y no se han tocado.

Test único, sin excepciones: digest de las secciones ALLOC (`.text`, `.rodata`,
`.data`, `.bss`, `.sdata*`, `.ctors` y sus `.rela.*`), ignorando `.line`, `.debug*`,
`.comment` y `.stab*`, compilando a un `.o` **privado** (`scratchpad/resto66/o/`),
nunca a `build/GOWE69`. Idéntico → se queda. Distinto → se revierte en el acto.

## 1. Cifras

| | |
|---|---|
| censo `src/Speed` (juego: `Src`, `bWare`, `GameCube`), expresiones del encargo | **97** (62 pines + 35 `__asm__("")`) |
| barreras escritas `asm("")`, fuera de esa expresión | 45 (total real 142) |
| **retirados** | **0** |
| intentos medidos (compilación + digest + diff contra el `.o` original) | **34**, en 14 funciones |
| andamios con diagnóstico NUEVO de la r66 escrito junto a ellos | **21** (19 del censo oficial + 2 `asm("")`), en 13 funciones |
| revertidos | los 34 intentos (ninguno salió idéntico) |
| regresiones | **0** — las 8 unidades tocadas reproducen su digest base exacto |
| propuestas de `configure.py` / `config/GOWE69` | ninguna |

El censo del encargo decía «97 de `src/Speed`»: son exactamente pines + `__asm__("")`
de `Src`, `bWare` y `GameCube` (`scratchpad/resto66/censo.py`, que salta comentarios).

## 2. Método y herramientas (`scratchpad/resto66/`)

* `cc.py`, `tool.py`, `d.py` — los de `cola65` con la salida redirigida a mi directorio.
* `prueba.py <spec>` — **nuevo**. El spec da unidad, fichero, símbolo y una lista de
  reemplazos exactos (cada uno debe aparecer UNA vez, con conversión CRLF automática).
  Guarda, aplica, compila a `.o` privado, compara con el digest base de la unidad,
  imprime las filas distintas de la función contra el `.o` ORIGINAL extraído y
  **revierte solo si sale distinto**. Los 34 intentos pasaron por aquí.
* `triaje.py` — función contenedora y veredicto de `regmap` (con `--obj` del `.o`
  privado) para los 116 sitios de las unidades sin promocionar; `triaje.txt`.
* `sym.py` — nombres mangled de un ELF (hacían falta para `d.py`: dos intentos
  salieron «DISTINTO» sin filas porque el nombre llevaba namespace).

El control PUEDE fallar y falla: los 34 intentos cambian el digest, y los comentarios
no lo cambian (§5).

## 3. Los 13 diagnósticos nuevos

Todos siguen el mismo patrón y es el hallazgo principal de la ronda (§4.1): **la
local que «sobra» es la portadora del pin**, y quitarla deja REPARTO puro.

| función (unidad) | andamios | clase | mejor forma sin andamio | filas |
|---|---|---|---|---|
| `SteeringWheelDevice::ConvertWheelRotation` (zMain) | 2 pines FPR | REPARTO | `originalVal*0.8f + x^3*0.2f` | 8 (rotación f0/f13/f12) |
| `GameDevice::PollDevice` (zMain) | 1 `__asm__("")` | IDENTICO (sched) | quitarla; `di++; value++` da el mismo `.o` | 2 |
| `FEngine::ProcessPadsForPackage` (zFEng) | 1 pin | PERMUTACION r30↔r31 | `(JoyMask & (1 << PadIndex))` | 12 |
| `QuickGame::OnManageTime` (zSim) | 2 pines FPR | ESTRUCTURA−3 → REPARTO | V2, sin `game_speed` | 2 |
| `SFXCTL_AccelTrans::UpdateParams` (zEAXSound) | 1 pin + 1 `asm("")` | PERMUTACION r10/r11/r0 | 3 formas, el mismo `.o` | 6 |
| `ScheduledSpeechEvent::sort_nested_priority` (zEAXSound) | 1 pin + 1 `asm("")` | ESTRUCTURA−2 | sin locales | 7 (+4 B) |
| `startnextrequest` (zEAXSound) | 1 pin | IDENTICO (preservados) | declarada después de `lockstate` | 21 |
| `SFXObj_Collision::InitSFX` (zEAXSound2) | 1 pin | REPARTO de 1 registro | constante en línea | 2 |
| `NFSMixMapState::CreateSubMixChannels` (zEAXSound2) | 3 pines | REPARTO | `int offset` sin pin, sin `zeroAdded`/`numCh` | 8 |
| `FnRawStateChan::FindTime` (zEagl4Anim) | 1 pin | REPARTO de 1 temporal | `k = c->GetKeyData(i)` | 4 (r0/r9) |
| `MemoryPoolManager::NewBlockAux` (zEagl4Anim) | 3 pines | REPARTO + sched | global en línea | 8 |
| `ArrayScroller::ScrollVer` (zFe2) | 1 pin | ESTRUCTURA−1 → copia que el original SÍ hace | `new_index + width` | 54 |
| `LGWheels::PlayFrontalCollisionForce` (zPlatform) | 1 pin | PERMUTACION r27↔r28 | parámetro renombrado `magnitude` | 12 |

Cada fila lleva en la fuente, junto al andamio, las formas probadas con su digest.
Lo más útil de cada una:

* **ConvertWheelRotation** — el DWARF sólo tiene `val` y `originalVal` (f0): el `0.8f`
  es el pseudo de la constante. Con sólo el pin `fr0` quedan 5 filas; hacen falta los
  dos a la vez.
* **PollDevice** — mapa de líneas: `value++` en la 796, `di++` en la 797, test en la
  798, el orden de nuestra fuente. Invertir las dos sentencias da el MISMO objeto:
  sched reordena igual, la posición en la fuente no le llega.
* **ProcessPadsForPackage** — el bloque original (`0x80186954-0x80186A40`) sólo
  declara `PadIndex`; `slw` en la línea 1071 y `mulli` en la 1073. El temporal del
  shift gana la prioridad (3 refs a `loop_depth` 2, vida corta) y se lleva r31.
* **OnManageTime** — cinco variantes (V0-V5). El mapa de líneas pone las dos cargas y
  el `fcmpu` en la 493 y `target_speed` en la 496; V4 escribe exactamente eso y da 5
  filas. V2 (una sentencia más) acierta los registros y sólo cruza el orden de las dos
  cargas: 2 filas. V5 con el literal `1.0f` da 28 filas y confirma la nota de la r51.
  Del `.greg` de V4 (`rtldump zSim OnManageTime -dg`): la carga de `Tweak_GameSpeed`
  es el pseudo 89 y la del `1.0f` el 90; `allocno_compare` pone el 90 delante (el 89
  conflicta además con el 88) y se lleva f0.
* **InitSFX** — con la constante en línea y el orden de stores intacto sólo cambia
  `li r11,0xd` por `li r10,0xd`: en el original r11 está ocupado en ese tramo y en el
  nuestro no. Reordenar los stores empeora (6 filas).
* **CreateSubMixChannels** — **`offset` NO sobra**: el DWARF la tiene (`// r0`). Las
  nuestras son `zeroAdded` y `numCh`. Sin pin, el 0 (un solo bloque, `local_alloc`) se
  lleva r0 y `offset`, que cruza el `if`, cae en r9.
* **FindTime** — el DWARF lista `GetKeySize` como HERMANO de `GetKeyData`, no anidado:
  el original sí llama a `GetKeyData(0)` y multiplica aquí, como nuestra fuente.
  `GetKeyData(i)` y `k += ...` dan el mismo objeto, 4 filas.
* **ScrollVer** — el objetivo hace `mr r11,r31` antes del `cmpwi dir,-1`, o sea que la
  copia existe en el original sin ser local. `GetCurrentDatumNum()` es
  `TraversebList` (una llamada), así que recalcularlo empeora a 79 filas.
* **PlayFrontalCollisionForce** — el DWARF dice `(int channel r31, unsigned char
  magnitude r27)` con UNA local (`ret`): el parámetro se llama `magnitude` y la copia
  es nuestra. Renombrado, sólo se cruzan el parámetro y `&this->periodic`; quitar
  también `playing`/`periodic` empeora a 32.

## 4. Hallazgos

### 4.1 «Sobra una local nuestra» no se cura quitándola cuando la local es el pin

En los 12 sitios de ESTRUCTURA−N del lote la local sobrante era **la que llevaba el
`register ... asm`**. Quitarla y escribir la sentencia en línea (el patrón que retiró
el pin de `ef_pow.c` en la r64b) deja siempre el mismo tipo de resultado: **mismo
tamaño y 2 a 12 filas de puro reparto**. Allí la local cambiaba la estructura; aquí
sólo transporta el registro. El veredicto de `regmap` es auto-referencial, como ya
advertía la r65 (§4.3 de `r65-cola.md`), y además **no es ni una pista**: la columna
útil es qué registros cruza el diff sin la local.

Corolario práctico: en este lote la forma sin local es ya la forma legítima (lo
confirma el DWARF en todos los casos) y lo que falta es un **empate del asignador**
que la fuente no mueve. Para esa clase la herramienta útil es el `.greg`, no más
formas de fuente.

### 4.2 El DWARF corrige el veredicto de regmap en tres sitios

* `NFSMixMapState::CreateSubMixChannels`: una de las tres «locales de más» (`offset`)
  es del original.
* `LGWheels::PlayFrontalCollisionForce`: la «local de más» es un **parámetro con otro
  nombre**.
* `FnRawStateChan::FindTime`: el orden de inlines prueba que la forma
  `GetKeyData(0)` + `GetKeySize()` es la original.

### 4.3 INCIDENTE: `QuickGame.cpp` compartido con `juego-oculto`

`QuickGame.cpp` tiene `OnManageTime` (mío) y `CreateCars` (de `juego-oculto`). Mis
variantes se aplicaban y revertían en segundos, pero durante la compilación de V5
(unos 60 s) el otro agente tomó su copia de seguridad del fichero. A las 10:14 la
restauró. Resultado medido:

| copia | sha1 normalizada (LF) | contenido |
|---|---|---|
| HEAD | `3f8c316bed85` | la buena |
| mi `bak` | `3f8c316bed85` | = HEAD |
| árbol a las 10:30 | `330a3dd3891e` | **mi variante V5** (sin pines, literal `1.0f`) |
| `juego66/bak/QuickGame.q1.cpp` | `330a3dd3891e` | la misma V5 |
| `juego66/priv/QuickGame.base.cpp` | `330a3dd3891e` | la misma V5 |
| `juego66/bak/QuickGame.cpp` | `be1e64d258b9` | otro estado intermedio, ni HEAD ni V5 |

Consecuencias y arreglo:

* **La «base» de zSim de `juego-oculto` para `QuickGame.cpp` es mi variante V5**, no
  HEAD. Cualquier medida suya de `CreateCars` contra esa base es inválida y hay que
  repetirla.
* El árbol se ha devuelto a HEAD con mi comentario encima: el código, quitado el
  comentario r66, es byte a byte HEAD (normalizado), y zSim da **`6913d4cebd1c34a7`,
  el digest base**.
* Mi comentario de `OnManageTime` se había perdido con esa restauración; está puesto
  de nuevo y verificado (25 líneas añadidas, 0 quitadas, todas comentario).

**Trampa nueva para `TRAMPAS.md`**: dos agentes que prueban variantes en funciones
distintas **del mismo fichero** se capturan las variantes transitorias el uno al
otro, y restaurar una instantánea hace retroceder el trabajo del vecino. Sin error y
sin aviso. Remedio: aplicar y deshacer por **parche** (reemplazo exacto y su inverso),
no por copia del fichero entero; o repartir los lotes por fichero, no por función. Por
eso no he tocado `TrackStreamer.cpp`, donde vive `HandleLoading` (de `juego-oculto`).

### 4.4 Lo que se revisó y NO se tocó, porque su nota ya lo agota

`bStrNCmp`/`bStrNICmp` (cinco y seis formas en la fuente), `Geometry::SphereVsBox`
(barrera selectiva, siete vedas), `CustomizeMain::NotificationMessage` y
`CustomizeParts::Setup` (r65-fe, con prioridades), `eLightE`, `EmitterSystem`,
`CARSFX_PreColWoosh` (r65-ecs), `WRoadNetwork`, `WeatherMan`, `GManager` (r65-world),
`AIPursuit::AssignClosestOffsets` (r36d), `EAXDispatch` ×2 (sched), `GenerateIndex`
(r36f), `WTriggerManager::CheckCollideSRB` (r29), `cStichWrapper::Play` (r47),
`SubTitler::GetElapsedTime`, `GinsuSynthData::BindToData` y
`SFXObj_PFEATrax::TestToLicensed` (sus `asm` escriben instrucciones reales: `li`,
`cmpwi`), `CARSFX_RoadNoise` (r50/r61), `CarRenderInfo::CreateCarLightFlares` (nota
de cabecera). La barrera `__asm__("")` de `EcstasyE.cpp:3384` se volvió a medir
quitada: `5560b1e1739c0073` contra la base `83b9a78ea8e2fd0d`, irreducible como decía
su nota.

## 5. Sellos: las 8 unidades tocadas, antes y después

Sólo se han añadido comentarios. Cada unidad recompilada al final contra su base
(misma herramienta, `.o` privado):

| unidad | ficheros con comentario r66 | ALLOC base | ALLOC final |
|---|---|---|---|
| zMain | `SteeringWheelDevice.cpp`, `InputDeviceGC.cpp` | `cc5a538171d2ac53` | `cc5a538171d2ac53` |
| zFEng | `FEngine.cpp` | `3869f721cf92634e` | `3869f721cf92634e` |
| zSim | `QuickGame.cpp` | `6913d4cebd1c34a7` | `6913d4cebd1c34a7` |
| zEAXSound | `SFXCTL_AccelTrans.cpp`, `EaxSoundTypes.cpp`, `realstream/src/stream.cpp` | `9e92525a5436ae95` | `9e92525a5436ae95` |
| zEAXSound2 | `SFXObj_Collision.cpp`, `NFSMixMapState.cpp` | `904281697182401c` | `904281697182401c` |
| zEagl4Anim | `RawStateChan.cpp`, `MemoryPoolManager.cpp` | `4016eb04ac44d293` | `4016eb04ac44d293` |
| zFe2 | `feArrayScrollerMenu.cpp` | `d8f49a3064f13f92` | `d8f49a3064f13f92` |
| zPlatform | `LGWheels.cpp` | `7411942baadf95ea` | `7411942baadf95ea` |

`git diff -U0` de los 13 ficheros: sólo líneas `+` de comentario, **cero `-`**. Ni una
cabecera compartida, ni `#if defined(__ANDROID__)`, ni `configure.py`, ni
`config/GOWE69/*`. Los bytes originales se conservan (lectura y escritura en binario,
CRLF respetado).

## 6. Veredicto

**PARCIAL en retiradas (0), COMPLETO en diagnóstico.** De los 97 andamios del censo
oficial, este lote tiene 13 funciones con 21 andamios medidos de nuevo, todos
irreducibles hoy y con la mejor forma escrita, su digest y su número de filas. No hay
regresiones.

Para la próxima ronda:

1. **Los 2-filas son los candidatos**: `OnManageTime` V2, `InitSFX`, `PollDevice`.
   Para los dos primeros, y para `ConvertWheelRotation`, el camino es leer el `.greg`
   de la forma buena y buscar la ref o la vida que falta. No hacen falta más formas de
   fuente.
2. **Repetir las medidas de `CreateCars` de `juego-oculto`** con la base de HEAD
   (§4.3).
3. **Añadir la trampa del fichero compartido** a `TRAMPAS.md`, y repartir los lotes
   por fichero.
