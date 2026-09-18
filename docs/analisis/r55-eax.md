# r55 — `zEAXSound` + `zEAXSound2`: −125.268 B de DOL, y la palanca nueva es una tercera cosa

Agente `eax`. Unidades: `Speed/Indep/SourceLists/zEAXSound` y `…/zEAXSound2`.
Cabeceras compartidas tocadas: **ninguna** (ver §4.1: `EAXCarState.hpp` se probó y se REVIRTIÓ).
Ni un `asm` nuevo, ni un andamio. `fncmp` limpio en las dos unidades, antes y después.

## 0. La medida, y su control

`dolwhere` se planta con «LAS SECCIONES NO COINCIDEN» en cuanto `.rodata`/`.data` no miden
igual, que es el caso de las dos unidades. Sonda propia (borrada al terminar, no vive en
`scripts/`): enlaza **BASE** (todo `obj/` para mis unidades) y **PROMO** en la misma corrida y
da (a) tamaños de sección del DOL, (b) **bytes distintos del DOL en las secciones que SÍ miden
igual** —`.init`, `.text`, `.extab`…—, (c) símbolos que caen en otra dirección, (d) el
histograma de escalones y (e) el volcado del escalón por rango de direcciones.

**El control obligatorio va dentro de la sonda**: imprime `BASE == DOL original`. En mitad de
la ronda salió **False** durante un rato (otro agente reconstruyendo lo suyo); con esa línea
delante se ve, sin ella habría apuntado −66 kB falsos. Todas las cifras de abajo están tomadas
con el control en **0 B de ruido**.

## 1. Lo que se hizo, con su cifra

### `zEAXSound` — 189.993 → **67.912 B** distintos (**−122.081**)

| # | edición | fichero | B distintos | Δ |
|---|---|---|---:|---:|
| — | base (HEAD) | | 189.993 | |
| 1a | fuera la 2.ª definición de `Speech::Manager::m_frameindex` | `EaxSoundTypes.cpp:26` | 189.994 | **+1** |
| 1b | fuera la 2.ª definición de `…Factory<Sim::ConnectionData…>::Prototype::mHead` | `SoundConn.cpp:99` | 179.957 | **−10.036** |
| 2 | `inline` al constructor de `EAX_CarState` (donde está, en el `.cpp`) | `EAXSound.cpp:850` | 170.997 | **−8.960** |
| 3 | `inline` a 17 de los 19 cuerpos de la cola | 4 ficheros | 170.446 | −551 |
| 4 | **`stBankSlot::Clear`: `inline` Y movido al FINAL de su fichero** | `EAXAemsManager.cpp` | 103.699 | **−66.747** |
| 5 | reorden de `SoundCollision.cpp` + permuta de los dos `UnloadSndData` | 2 ficheros | 101.466 | −2.233 |
| 6 | `GetClosestPlayerCar(3 args)`, `GetClosestCopCarToCamera`, `GetPlayerCarInRadius` → `EAXSndUtil.cpp` | | 101.406 | −60 |
| 7 | `IsWorldDataStreaming` + `AssignAudioStreamHandle` → `EAXS_StreamManager.cpp` | | 83.635 | **−17.771** |
| 8 | `SFXCTL_Physics::UpdateNIS` + `::MsgRevEngine` → `SFXCTL_NISReving.cpp` | | 73.775 | **−9.860** |
| 9 | `InitServices`/`RestoreServices` al principio y `UpdateServices` al final de `SoundConn.cpp` | | 68.801 | −4.974 |
| 10 | tres permutas locales (`AddSample`, `InitializeStreams`, ctor de `SFXCTL_HybridMotor`) | | 67.912 | −889 |

Símbolos en otra dirección: 15.099 → 14.519. Bytes de esos símbolos: 1.165.513 → 1.043.101.
El `.text` de la unidad está hoy **en su dirección exacta desde `800A6128` hasta `800B5F70`**
(antes se rompía ya en `800AAA6C`).

### `zEAXSound2` — 66.523 → **63.336 B** distintos (**−3.187**)

| # | edición | B distintos | Δ |
|---|---|---:|---:|
| — | base (HEAD) | 66.523 | |
| 11 | `g_MaxSongs` y `RoadNoiseVolumes` a `extern` (`SFXObj_Pathfinder.cpp:61`, `CARSFX_Roadnoise.cpp:141`) | 66.523 | **0 en bytes, pero devuelven el dueño** |
| 12 | reorden de las nueve `SFXObj_PFEATrax::*` al orden del objetivo | 63.336 | **−3.187** |

## 2. La palanca nueva: **`inline` NO basta, la POSICIÓN dentro del fichero manda**

Es el hallazgo de la ronda y vale 66.747 B él solo.

`stBankSlot::Clear` (48 B) el objetivo lo emite en la **cola** (`800C9098`) y sin embargo su
llamante `EAXAemsManager::RegisterSlots` hace una **llamada de verdad** (544 B en el objetivo).
Marcarlo `inline` donde estaba (línea 288, con llamantes en 426, 430, 920 y 1016) hace las dos
cosas mal: sí va a la cola, pero **GCC lo empotra en `RegisterSlots`, que baja a 508 B**. Eso
es una regresión de `fncmp` y se ve al instante.

La forma correcta es **`inline` + definición POR DEBAJO de todos los llamantes del fichero**:
en el punto de la llamada sólo hay declaración, así que se emite `bl`; y al terminar el fichero
`finish_function` (`cp/decl.c:14701`) la mete en `saved_inlines`, que
`wrapup_global_declarations` vuelca al final de la unidad. Es lo que hacía el original, y
explica por qué la cola del objetivo mezcla funciones que en nuestro árbol están definidas
arriba del todo.

**Es general y barata de probar**: cualquier cuerpo que el objetivo emita en la cola y que hoy
tengamos con llamantes en el mismo `.cpp` es candidato. El síntoma de haberlo hecho mal es una
sola fila de `fncmp` en el llamante, con `tamaño (X/Y)` e `Y < X`.

## 3. La segunda palanca: **el fichero al que pertenece la función**

Cuatro de las diez ediciones no son de orden dentro de un fichero, sino de **atribución de
fichero**, y se leen directo del volcado de escalones: un bloque entero con un desplazamiento
constante igual al TAMAÑO de lo que le falta delante.

* `IsWorldDataStreaming` (108 B) + `AssignAudioStreamHandle` (16 B) estaban al principio de
  `EAXSndUtil.cpp`; el objetivo las tiene **pegadas al constructor de `EAXS_StreamManager`**
  (`800B5FE0`, justo antes de `800B605C`). 124 B = el escalón `+124` que arrastraba 99
  funciones. **−17.771 B.**
* `SFXCTL_Physics::UpdateNIS` (1.592 B) + `::MsgRevEngine` (84 B) estaban en
  `SFXCTL_Physics.cpp`; el objetivo las tiene en **`SFXCTL_NISReving.cpp`**, delante de
  `MsgRevOff` —que ya estaba allí—. 1.676 B = el escalón `−1676` de todo el bloque
  NIS_RevManager / AccelTrans / HybridMotor. **−9.860 B.**
* `SoundConn::UpdateServices` va al **final** de `SoundConn.cpp` (detrás de
  `HeliSoundConn::UpdateState`) y `InitServices`/`RestoreServices` al **principio**, delante de
  `CarSoundConn::Construct`. **−4.974 B.**
* `GetClosestPlayerCar(const bVector3*, bool, int&)`, `GetClosestCopCarToCamera` y
  `GetPlayerCarInRadius` viven en **`EAXSndUtil.cpp`**, entre `GetClosestPlayerCar(1 arg)` e
  `IsCarInRadius`, no en `SoundCollision.cpp`. Sólo −60 B, pero deja el orden bien.

Coste: dos de los cuatro necesitan **añadir `#include`s** al fichero de destino (tres a
`EAXS_StreamManager.cpp`, seis a `SFXCTL_NISReving.cpp`). Medido: ninguno introdujo
regresiones de `fncmp` ni instanciaciones que muevan nada.

## 4. Sorpresas — dos contradicen el encargo

### 4.1 **Meter el constructor DENTRO de la clase es PEOR que marcarlo `inline` en el `.cpp`**

El encargo pedía mover el cuerpo de `EAX_CarState::EAX_CarState` a `EAXCarState.hpp:30`. Se
hizo, se midió y se **revirtió**. Las dos formas mandan el cuerpo a `saved_inlines`, pero:

| forma | dónde cae el ctor (objetivo `800C899C`) | `.rodata` enlazada | B distintos |
|---|---|---:|---:|
| `inline` en `EAXSound.cpp` | `800C91D4` (+2.104) | −96 | **170.997** |
| cuerpo dentro de la clase | `800C8BB4` (+536) | **−192** | 187.223 |

En clase coloca el cuerpo **1.568 B más cerca**, pero cuesta **96 B más de `.rodata`
enlazada**, y como `.rodata` va delante de `.data`, esos 96 B desplazan todo lo que viene
detrás y rompen todas las referencias del `.text` a datos: **16.226 B peor** en el DOL. La
forma que paga es `inline` sobre la definición fuera de clase.

Y hay un segundo motivo para no hacerlo: `EAXCarState.hpp` la incluyen
`EPlayerTriggeredNOS.cpp` (**zMain**) y `SoundCar.cpp`/`SoundHeli.cpp`
(**zPhysicsBehaviors**). Con el cuerpo en clase, cualquier reconstrucción de esas unidades
emitiría un símbolo débil de 1.308 B que hoy sólo define `zEAXSound` (índice 8). Comprobado
símbolo a símbolo sobre los 616 objetos del enlace: **hoy no hay fuga**, precisamente porque
está revertido.

La predicción del informe `r55-med-zEAXSound.md` se cumple **exacta** con la forma `inline`:
los escalones `+2192`, `+1356` y `+1760` bajaron **1.308 B cada uno** (a `+884`, `+48`, `+452`).
El mecanismo `saved_inlines` queda confirmado.

### 4.2 Las dos ediciones de `EaxSoundTypes.cpp:26` **no valen lo mismo**: una es +1 B

El encargo las daba juntas por 246.452 B. Medidas por separado:

* **`Speech::Manager::m_frameindex`** (2 B, `.data`): quitarlo devuelve el símbolo a la ventana
  de `zSpeech` (`80417768` → `804357BC`), pero el DOL queda en **+1 B**. La cifra de 123.364 B
  del censo es **radio de desplazamiento**, no bytes distintos, y aquí el arrastre de sección
  ya lo tapaba. Se queda porque es correcto por construcción —`obj/zEAXSound.o` lo tiene
  **UNDEF**— y porque en el mundo todo-promocionado `zEAXSound` (idx 8) le ganaría a `zSpeech`
  (idx 28) y rompería a los dos.
* **`Factory<Sim::ConnectionData…>::Prototype::mHead`** (4 B, `.data`, `SoundConn.cpp:99`):
  **−10.036 B**. La definición buena es `zSim.cpp:188`.

Lo mismo con `g_MaxSongs` y `RoadNoiseVolumes` en `zEAXSound2`: **0 B hoy**, dueño correcto
mañana. Los cuatro son de la familia D del censo y los cuatro están verificados contra la tabla
de símbolos del objeto original (`shndx == 0` en `obj/zEAXSound*.o`).

### 4.3 `reorden.py` decía **ORDEN_PURO** y el `.rodata` de `zEAXSound` **sí tiene contenido**

`dolrod.py zEAXSound .rodata` sobre el enlace da decenas de tramos `FALTA`/`SOBRA` con cadenas
partidas: la secuencia de literales está **permutada Y corta**. El déficit no se ha movido con
nada de lo de arriba (−104 al empezar, −136 al acabar; +32 los cuestan los 17 `inline`, y
revertir los cuatro que llevan `static UCrc32 hash = "…"` devuelve el `.rodata` a −96 pero
empeora el DOL en 152 B, así que se quedan). No es el frente de esta ronda pero **es el que
bloquea la promoción**.

## 5. Lo que queda, con cifra

`trypromo` (ruta completa) sigue dando **DOL ROTO** en las dos:

    Speed/Indep/SourceLists/zEAXSound     DOL ROTO (d48e24bb7f89)   .text +0  rodata-136 data-416 bss+32
    Speed/Indep/SourceLists/zEAXSound2    DOL ROTO (3ad60c8bbb65)   .text +0  rodata-48  data-288 bss-32

**El `.text` de las dos mide exactamente igual que el objetivo y su código es idéntico**
(`fncmp` 0 de 773 y 1 de 930). Lo único que impide promocionar son `.rodata` y `.data`:

1. **`zEAXSound`: `.rodata` −136 y `.data` −416.** Es el techo de la unidad. El `.data` incluye
   426 B de símbolos sin pareja (`lbl_*`/`gap_06_*` del troceado y dos estáticas de función
   renumeradas, `PlayerUpgrade.14976` y `counter.33705`).
2. **`zEAXSound2`: `.rodata` −48 y `.data` −288**, más `GenerateRoadNoise` (1.240 B, 20
   instrucciones). El diff de esa función está **localizado en los índices 88–125**:
   adelantamos la carga del literal `0.1f` (`slipBoost`) al bloque anterior al `bso` del segundo
   `bLength`; el objetivo la carga **después**, en el bloque de la acumulación, y de ahí sale la
   permuta `f8`/`f10` que recorre el resto. Es reparto/planificación, no estructura: candidato a
   pin de registro (`register float slipBoost asm("f10")`) en la próxima ronda.
3. **La cola de `finish_file`** de las dos (desde `800C866C` en `zEAXSound`, desde `800F2EE4`
   en `zEAXSound2`) sigue permutada, con escalones de ±100…±3.600 B. Son ~6,5 kB por unidad y
   es el siguiente trozo grande de `.text` desplazado.

## 6. Propuestas que NO he aplicado

* Nada de `configure.py`, `splits.txt` ni `keep.lst`. **No propongo tocarlos**: el `.text`
  cuadra y el déficit de datos no es de rangos.
* `SpeechManager.hpp` lo está editando el agente `speech`; mi edición de `m_frameindex` es
  **compatible** con la suya (la definición buena sigue en `SpeechManager.cpp:105`, que es de
  él), pero conviene que lo sepa.

## 7. Atomicidad

**Ninguna de mis ediciones es atómica con otro agente.** Las cuatro deduplicaciones dependen de
símbolos que definen `zSim`, `zSpeech`, `zFe2` y `zEAXSound`, y esas definiciones existen tanto
en `obj/*.o` como en `src/*.o`. Las diez reordenaciones son internas a mis dos unidades.
