# r57 — zSpeech: la `.data` cierra al simbolo, y una instanciacion explicita valia 4.974 B

Agente `speech`. Propiedad: `zSpeech`, ficheros de `Speech/` y `Stream/`.
**Cuatro ficheros tocados, los cuatro incluidos SOLO por `zSpeech.cpp`** (comprobado
con grep: `SpeechManager.cpp`, `SoundAI.cpp` y `SpeechCache.cpp` no los incluye
nadie mas; las tres coincidencias de `SpeechManager.cpp` en otros ficheros son
COMENTARIOS, no `#include`). **Cero cabeceras compartidas.** Ni `config/`, ni
`splits.txt`, ni `keep.lst`, ni `configure.py`.

---

## 0. Titular

    19.383 B de DOL distinto  ->  12.406      (-6.977 B, el 36 %)

| seccion | antes | despues | simbolos en su DIRECCION |
|---|---:|---:|---|
| `.text`   | 9.605 | **3.965** | 602/704 -> **637/704** |
| `.rodata` | 8.571 | **8.247** | — |
| `.data`   | 1.207 | **194**   | 2/189 -> **195 de 195** |

**La `.data` de zSpeech esta CERRADA**: los 195 simbolos del objetivo estan en su
direccion exacta. Los 194 B que quedan son los punteros de los 139 `_4Csis.*Id`
apuntando a cadenas que siguen descolocadas, o sea que **caen solos cuando cierre
la `.rodata`**.

`fncmp` ANTES y DESPUES: **identico**, 1 de 703 (`Setup`, 596 B). Cero regresiones.

Sello, con **tres** compilaciones seguidas:

    30efe7bed8b3ea41657d686b0108745805a7187c   (x3, misma fuente)
    DOL resultante: dbb397947eb9, 4.541.888 B (tamano EXACTO), 12.406 B distintos

---

## 1. AVISO PRIMERO: el encargo traia el diagnostico de la r55, no el de la r56

El punto (a) del encargo —«los 8 B de `.rodata` estan en el pool `$LC` o en su
relleno, ataca con `lcpool`/`rodorden`»— es el texto de la **r55**. La **r56** ya
lo resolvio y midio: no estaban en el pool, estaban en `keep.lst:1668-1669`
(`# @lc zSpeech "GAMECUBE"` + `zSpeech.o:$LC58`), y pidio borrar esas dos lineas.

**La ventana NO aplico esa propuesta**: la entrada sigue en el arbol. Medido hoy,
con el `.o` de esta ronda:

| `keep.lst` | bytes de DOL distintos | tamano del DOL |
|---|---:|---:|
| el del arbol, tal cual | **215.616** | 4.541.920 (32 B de mas) |
| corregido con `lcfix` y **sin** `zSpeech.o:$LC58` | **19.383** | 4.541.888 OK |

O sea que **borrar esas dos lineas seguia valiendo 196.233 B** al empezar la ronda.
Todas mis medidas van contra una copia corregida en el scratchpad; el fichero del
arbol no lo he tocado.

---

## 2. El `.rodata +8` ahora se arregla DESDE LA FUENTE, y la entrada de `keep` ya no vale

La r56 dejo el diagnostico a medias: dijo «`$LC58` es "GAMECUBE", esta muerta y la
salva `keep.lst`». Falta la mitad: **por que 8 y no 12**.

El pool de nuestro objeto empezaba asi (offsets dentro de `.rodata`):

    0x5C  $LC57  ""            1 B
    0x60  $LC58  "GAMECUBE"    9 B
    0x6C  $LC59  "Attrib::Attribute"

y el objetivo tiene `Attrib::Attribute` en **0x60**. `-strip-unused-data` se lleva
`size & ~7` de cada simbolo muerto: de `$LC57` (1 B) se lleva **0**, de `$LC58`
(9 B) se lleva **8**. Con la entrada de `keep` borrada quedaban 8 B de residuo y
la cadena caia en 0x64: **4 B tarde, y con ella TODA la `.rodata` de la unidad**.

`bWare.hpp:172` ya tenia el gancho —`BWARE_PREFIX_GAMECUBE`, que usan zAnim, zMain
y zFEng— y zSpeech definia solo `BWARE_PREFIX_BADALLOC`. Con

```cpp
#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)
```

`bGetPlatformName()` devuelve el prefijo escrito a mano y **el literal no nace**:
`$LC57` (1 B) se queda en 0x5C, no se estripa nada, y `Attrib::Attribute` cae en
**0x60**, que es el del objetivo. Medido: `.rodata` 8.571 -> 8.251.

**CONSECUENCIA PARA LA VENTANA**: ahora `lcfix.py --check` dice

    FALLO    zSpeech: 'GAMECUBE' no tiene simbolo $LC propio

y **no puede corregirlo solo**, porque la cadena ya no existe. Hay que **borrar a
mano `keep.lst:1668-1669`**. Sin ese borrado el DOL sale 32 B largo y con 215.616 B
distintos.

### Extrapolacion (CONTADA, no medida)

La r56 conto 13 unidades mas con `# @lc <u> "GAMECUBE"` y bloque de prefijo bWare a
mano. Para todas ellas **la cura es la misma linea de `#define`**, y es mejor que
borrar la entrada de `keep`: no depende del redondeo a 8 del estripado. Los duenos
de `zCamera`, `zEAXSound`, `zEAXSound2`, `zFe`, `zFe2`, `zGameplay`,
`zPhysicsBehaviors`, `zWorld`, `zWorld2` lo tienen a una linea.

---

## 3. La `.data`: de 2 simbolos en su sitio a 195 de 195

El deficit era **12 B** delante del array de `_4Csis.*Id`, y eso desplazaba los
139 identificadores y los 1.207 B de diferencia. Cinco causas, todas medidas:

### 3.1 Los estaticos de clase iban los tres juntos y el objetivo los REPARTE

`SpeechManager.cpp` definia `Manager::*`, `GameSpeech::*` y `SED_NISSFX::*` en un
bloque unico en la cabecera del fichero. El objetivo los tiene **entrelazados con
estaticos locales de funcion**:

    0x80435980  Manager::m_SpeechModule .. mLastSpeakerID
    0x804359C0  max_samplerequests      <- estatico local de Manager::Update (l.697)
    0x804359C4  hueco de 12 B
    0x804359D0  TRACKSTREAMER_BACKLOG_THRESH
    0x804359D4  GameSpeech::m_tempCharPtr ..
    0x804359E8  req_timer               <- estatico local de GameSpeech::Update (l.1730)
    0x804359EC  SED_NISSFX::m_tempCharPtr ..

**Un estatico local se emite al compilar SU funcion**, asi que ese entrelazado
solo puede salir si las definiciones de clase estan **repartidas por el fichero**,
cada bloque delante de los metodos de su clase. Es la forma del fuente original.
Movidos: `GameSpeech::*` delante de `GameSpeech::GameSpeech()`, `SED_NISSFX::*`
delante de `SED_NISSFX::SED_NISSFX()`.

**El detalle que cuesta media hora**: `TRACKSTREAMER_BACKLOG_THRESH` es GLOBAL (el
objetivo la nombra sin mangler de namespace) y su sitio cae DENTRO de
`namespace Speech`. Definirla ahi da `L0039: Reference to undefined symbol`
—queda como `Speech::TRACKSTREAMER_BACKLOG_THRESH`—. Hay que **cerrar y reabrir el
namespace** alrededor de la definicion.

### 3.2 Dos huecos que `keep.lst` nombra desde hace rondas y nadie habia escrito

`keep.lst` lista ocho `zSpeech.o:gap_06_*_data`. Faltaban por escribir
`gap_06_8043597C_data` (4 B, entre `SPEECH_DISPLAY_HISTORY` y los estaticos de
`Manager`) y `gap_06_804359C4_data` (12 B). Escritos como `asm()` de ambito de
fichero en `SpeechManager.cpp`, en el punto exacto.

### 3.3 Un `namespace {}` muerto que dejaba 4 B de residuo

```cpp
namespace {
ScheduledSpeechEvent *sQueuedEvents[256] = { 0 };
int sQueuedEventCount = 0;
void CompactQueuedEvents() { ... }
}
```

**Nadie referencia esos tres nombres** (grep: solo aparecen dentro de si mismos).
El enlazador se llevaba el array de 1.024 B entero (`1024 & ~7 = 1024`) pero
`sQueuedEventCount` mide 4 y `4 & ~7 = 0`: **dejaba 4 B de residuo justo delante de
`max_samplerequests`**. Borrado el bloque, `fncmp` no se mueve.

### 3.4 `.balign 8` sobre una direccion que NO esta 8-alineada

`gap_06_80435F24_data` (el relleno final de la unidad) llevaba `.balign 8` y
`0x80435F24` es 4-alineada: el bloque caia en `0x80435F28` y **la `.data` de la
unidad salia 8 B larga**, empujando a zTrack y creciendo la seccion 32 B. Con
`.balign 4` cae en su sitio. *Vale la pena barrer los demas `gap_*` del arbol
buscando el mismo error.*

### 3.5 Dos reordenaciones de contenido

* `SPEECH_CACHE_STATS` (`SpeechCache.cpp`) va **entre** `SpeechMemoryPool` y
  `PRINT_SPEECH_CACHE_IO` (0x80435A08), no al final del bloque.
* `ColourHashToSoundColourMap[]` + `NumberOfColourHashToSoundColourMaps`
  (`SoundAI.cpp`) van **delante de `SoundAI::GetCustomized`**, su unico usuario, no
  en la cabecera del fichero. El objetivo los tiene en 0x80435EB8, o sea **detras**
  de `prev_heat` (0x80435EB0) y `dir_tracking` (0x80435EB4), que son estaticos
  locales de funciones de las lineas 1641 y 2518. Definida arriba, la tabla salia
  delante de los dos y la `.data` de la unidad quedaba 8 B larga por el final.

---

## 4. EL HALLAZGO DE LA RONDA: una instanciacion explicita cuesta 34 posiciones

`SpeechManager.cpp` tenia, en la linea 98:

```cpp
template void std::vector<SpeechSampleData *, UTL::Std::Allocator<...> >::reserve(unsigned int);
```

**Una instanciacion explicita emite el cuerpo EN EL PUNTO DEL PARSEO**, o sea el
primero de los 34 COMDAT de la unidad. El objetivo lo tiene en `0x802AFE34`,
**entre `clear__deque<int>` y `_M_reallocate_map__deque<int>`**, que es donde cae
si se deja que lo instancie el USO.

    Quitando la linea:  .text 8.939 B distintos -> 3.965   (-4.974 B)
                        simbolos en su sitio 602/704 -> 637/704

La funcion **se sigue emitiendo** (`fncmp`: 703 de 703 presentes, 1 sola con el
codigo distinto, la de siempre). Las 34 funciones que iban 284 B desplazadas —toda
la familia `sort`/`heap`/`_Deque_base` de la unidad— caen en su sitio de golpe.

**Es un frente, no un caso**: cualquier `template void ...;` explicito en el arbol
esta clavando un cuerpo en el punto del parseo. Vale la pena un
`grep -rn "^template [a-z].*::" src/` y comprobar unidad por unidad si el objetivo
lo tiene ahi. Yo no lo he contado en las demas: hasta contarlo no es un frente.

---

## 5. El primer de pool, aplicado y medido en zSpeech

`MAudioReflection.h` abre su bloque `ZMAIN_MESSAGES_LUA_INLINE` con
`LuaBindery.h`/`LuaPostOffice.h`, y esos arrastran `gameplay.h` **entre** el literal
`"MAudioReflection"` (de `_GetKind`) y los nombres de campo de
`BuildMessageTable`. Resultado: `Attrib::Gen::gameplay` y `done` se internaban 32 B
antes de su sitio.

Como el primer **solo mueve hacia ATRAS**, se internan a mano las CINCO que van
delante, con una `static inline` muerta puesta justo antes del `#include`:

```cpp
static inline const char *_zSpeechPoolAud(int n) {
    switch (n) {
    case 0: return "MAudioReflection";
    case 1: return "PlayerNum";
    case 2: return "Dist";
    case 3: return "Covered";
    case 4: return "Attrib::Gen::audiosystem";
    }
    return 0;
}
```

Sale exactamente el orden del objetivo y `gameplay` cae detras sola. El
desplazamiento acumulado en esa zona pasa de **+40 a +16**. En bytes de DOL solo
paga 4 (la zona ya era distinta byte a byte), pero es estructural: sin esto no se
puede cerrar nada de lo que viene detras.

---

## 6. NEGATIVO MEDIDO Y REVERTIDO: el pool a mano (`ATTRIB_TAGS_HAND_POOL`)

Vale la pena contarlo entero porque **es el camino para cerrar la `.rodata`** y
porque he medido donde se atasca.

`AttribSys.h:43` tiene la guarda `ATTRIB_TAGS_HAND_POOL` que convierte todo
`ATTRIB_TAG(s)` en `NULL`: ninguna cadena `Attrib::Gen::*` se interna. La usan
zMain, zMisc, zRender y zSim. Aplicada a zSpeech:

* `fncmp` **no se mueve** (1 de 703): confirmado que `NULL` no cuesta una instruccion.
* Desaparecen **22 cadenas `Attrib::Gen::*` + `Attrib::TAttrib`**, 512 B de `.rodata`.
* **Y con ellas TODOS los residuos parasitos**: `'p'` (de `emitterdata`/
  `emittergroup`, que `WorldConn.h -> EmitterSystem.h` arrastra y el objetivo NO
  tiene), `'stems'/'ssis'/'uction'/'es'/'n'` (de `chassis`/`induction`/`nos`/
  `tires`/`transmission`), `'bosfx'`, `'pp'`... El pool queda **limpio**.

Con eso, la cabecera del pool nuestro pasa a tener **dos** extras contra el
objetivo (`done` y `EventSequencerSystems`, 16 B) en vez de una docena.

**Donde se atasca**: hay que volver a emitir las 22 con primers, y **los grupos D y E
no tienen costura en ficheros mios**:

| grupo | cadenas | va detras de | va delante de | sitio |
|---|---|---|---|---|
| A | simsurface, `Attrib::TAttrib`, speech, ecar, camerainfo, effects, audioimpact, audioscrape | `MGeneric` | `WorldBodyConn` | `zSpeech.cpp` OK |
| B | pvehicle, engine, engineaudio | `Pkt_Effect_Service` | `MAudioReflection` | `zSpeech.cpp` OK |
| C | gameplay, milestonetypes, `VehicleParams`, speechtune, pursuitlevels | `Attrib::Gen::audiosystem` | `SpeechSampleMap node` | `zSpeech.cpp` OK |
| D/E | 16.1.0, 16.2.1, 1.2.3, 1.8.1, 19.8.31, aivehicle, pursuitescalation, pursuitsupport, `GRaceStatus` | `SpeechSampleMap node` (`SpeechCache.h:27`) | `SMS_MESSAGE_%d` | **`SpeechManager.hpp`**, compartida |
| F | smackable | `Param` (0x80407A5C) | — | dentro de un `.cpp`, sin costura |

`SpeechCache.h` y `GRaceDatabase.h` se parsean **seguidas** dentro de la cadena de
`SpeechManager.hpp` (`$LC261` y `$LC262` consecutivos): no hay ni un fichero mio en
medio donde meter el primer. Aplicado a medias el resultado es **243.722 B**
—peor que el punto de partida—, asi que **lo he revertido**. Queda medido y con el
plan escrito.

---

## 7. Lo que queda, con cifra y con causa

    .text    3.965 B   orden de emision de los COMDAT (67 funciones)
    .rodata  8.247 B   orden del pool
    .data      194 B   punteros de los 139 _4Csis.*Id a cadenas descolocadas
    -----------------
            12.406 B

### 7.1 `.text`: los 67 que quedan estan TODOS en `[0x802B44E4, 0x802B5900)`

Es el volcado de COMDAT de `finish_file`. El orden del objetivo es

    _._SpeechSampleVec, _._VoiceUsage, _._SchedSpchEvents, _._copList,
    _._observations, Module::*, GameSpeech::*, SpeechFlow::*,
    ClassKey__speechtune, EAXCharacter::* (20), EAXCop::* (18),
    EAXAirSupport::IsHeli, SED_NISSFX::* (4), _._VecHashMap644,
    _._SpchSampleMap, RebuildTable, ... , _._SpeechHashIDMap,
    _._EventHistory, _._SPCHEventList, _._SampleReqList

y el nuestro empieza por `EAXCharacter::*`. **Las clases de las 67 viven todas
detras de `SpeechManager.hpp`** (que incluye `EAXCharacter.h`, `SpeechCache.h`,
`SpeechModule.hpp`), y esa cabecera **la incluyen 17 `.cpp` de seis unidades**
(zAI, zEAXSound, zEAXSound2, zGameplay, zLua, zSim). **No la he tocado**: no se
puede medir la regresion sin compilar unidades de otros agentes, y compilarlas
sobrescribe su `.o` en `build/GOWE69/src/` mientras ellos miden.

La rotacion de los cuatro destructores finales (`SampleReqList` del primero al
ultimo) sigue siendo la edicion mas limpia —**tres lineas**, mover
`class SampleReqList` detras de `struct SPCHEventList` en `SpeechManager.hpp`— y
sigue siendo **trabajo de ventana, no de ronda**.

### 7.2 `.rodata`: 72 B de cadenas que sobran y CERO que falten

Multiconjunto sobre `[0x80405E10, 0x80408FB8)`, cadenas de 4 o mas caracteres:

    SOBRAN 72 B:  'AUD:Relocated speech headers' (duplicada), 'uction', '?&ffB4',
                  'Speech', 'stems', 'bosfx', 'done', 'ssis'
    FALTAN   0 B

Y en el OBJETO, sin estripar, siguen duplicadas las tres de la r56:
`'SoundAI CarCustomization'` x2, `'AUD: SED_NISSFX events'` x2,
`'AUD:Relocated speech headers'` x2 — el bloque `asm()` de la r52 (`zSpeech.cpp`,
13 cadenas) es **andamio caducado**: el arbol ya tiene el codigo que las
referencia. Y siguen SIN emitirse `'Speech::Cache::Alloc'` (`SpeechCache.cpp:240`
pasa una local sin inicializar donde el original pasaba el literal) y
`'Attrib::Gen::aud_moment_strm'`.

**El orden de ataque de la r58** (lo he dejado escrito porque el balance tiene que
salir cuadrado o el DOL rompe):
1. `ATTRIB_TAGS_HAND_POOL` + primers A/B/C (seccion 6) — **quita los parasitos**, es
   lo unico que se puede hacer sin cabeceras ajenas... pero deja D/E/F sueltos, asi
   que hay que hacerlo **junto** con el punto 2.
2. Una costura para D/E: o un `#include` propio intercalado en `SpeechManager.hpp`
   (ventana, 6 unidades), o `DEAD_STR_GRACEDB`/`DEAD_STR_GSTATE` como el
   `DEAD_STR_MUNSPAWNCOP` que ya existe en `MUnspawnCop.h` — el patron esta
   inventado en el arbol y es de riesgo cero si el valor por defecto es `s`.
3. Desmontar el `asm()` de la r52 y repartir sus 13 cadenas con primers.

---

## 8. Sorpresas

1. **El `sha1` de mi `.o` cambio sin que yo tocara nada, y NO era el compilador.**
   Compilando el arbol con mis cuatro ficheros identicos byte a byte (`cmp` OK)
   salio `30efe7be` donde media hora antes salia `cac46538` tres veces seguidas.
   La causa: **otro agente edito una cabecera mia en mitad de la ronda** — el
   agente `zai` saco `Speech::copPair`/`copMap` de
   `src/Speed/Indep/Src/Speech/SoundAI.h` a un `SpeechContainers.h` nuevo
   (mtime 03:11:17, entre mis dos compilaciones). **Medido: el DOL enlazado no
   cambia** (`dbb397947eb9` antes y despues, 12.406 B distintos las dos veces), o
   sea que el traslado es limpio para zSpeech; lo unico que se movio es el DWARF
   del objeto.
   Dos lecciones: (a) **el `sha1` del `.o` no vale como sello de fuente** —lleva
   dentro cosas que no llegan al enlace—, el sello bueno es el hash del DOL de
   `trypromo`; (b) la regla 5 de la r56 **funciona en los dos sentidos**: si un
   agente edita una cabecera ajena, el dueno ve moverse su sello sin explicacion.
   Conviene que el que mueve la cabecera avise, no solo que mida.
2. **`linkdelta` dice `rodata -232` de una unidad cuya `.rodata` mide EXACTO.** Es
   el `keep.lst` rancio otra vez (89 entradas de zSpeech desplazadas por mis
   ediciones). Con el `keep` corregido, `+0` en las siete secciones.
3. **`previo.py` sigue sin desmanglar** (la trampa que canto la r56): con
   `Setup__Q26Speech13RoadblockFlow` dice `0 bloques`; con `RoadblockFlow::Setup`
   saca la veda de r36d/r36f/r45/r48. **Sin arreglar.**
4. La veda de `Setup` sigue en pie: 23 negativos acumulados. **No la he tocado.**
5. **Un hueco escrito a mano puede tener el `.balign` mal y nadie lo nota**: el
   simbolo existe, `keep.lst` lo salva, `checksplits` calla, y la seccion sale 8 B
   larga. Merece un barrido de los `gap_*_data` del arbol contra la alineacion de
   su direccion.

---

## 9. Lo que pido, con la cifra

Las dos primeras son **un paquete atomico** y ya no se pueden separar (al no existir
el literal, `lcfix` no puede resolver la entrada de `GAMECUBE`: la deja como
`FALLO`). Medido con el arbol tal como lo dejo:

    keep.lst del arbol, sin tocar        254.708 B distintos, DOL 4.541.664 (224 B corto)
    lcfix + borradas las dos lineas       12.406 B distintos, DOL 4.541.888 EXACTO
    ----------------------------------------------------------------------------
    el paquete vale                      242.302 B

| propuesta | vale | quien |
|---|---:|---|
| **borrar `keep.lst:1668-1669`** (`# @lc zSpeech "GAMECUBE"` + `zSpeech.o:$LC58`) — ahora es OBLIGATORIO, `lcfix` da `FALLO` y no puede solo | \ 242.302 B | ventana |
| correr `python scripts/lcfix.py` — **89 correcciones de zSpeech pendientes** (102 en total al cerrar; las otras 13 son de zAI) **mas 1 `FALLO`**, que es la entrada de arriba y hay que borrarla a mano | / juntas | ventana |
| `#define BWARE_PREFIX_GAMECUBE` en las otras 13 unidades con prefijo a mano | 8 B de `.rodata` por unidad | los duenos |
| `SampleReqList` detras de `SPCHEventList` en `SpeechManager.hpp` (6 unidades) | la rotacion final del `.text` | ventana |
| arreglar `previo.py` para nombres manglados | media ronda por agente | ventana |
