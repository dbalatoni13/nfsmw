# r53 · `cerca1`: zMain cierra su `.data` (queda a CERO de secciones), zPhysics pasa de −40/−32 a +8/+32

**Ninguna de las dos promociona**, y va primero la cifra:

| | antes (r52) | ahora | `.text` | `fncmp` | `permorden` | `trypromo` |
|---|---|---|---:|---|---:|---|
| `zMain` | `data−32` | **IGUAL (las nueve secciones)** | +0 | 0 de 1.380 | 15 → **11** | ROTO `dab74065b7af` |
| `zPhysics` | `rodata−40 bss−32` | `rodata+8 bss+32` | +0 | 0 de 718 | — | ROTO `0d27a7c17d3a` |

`zMain` **ya no tiene deficit de ningun tamano**: `linkdelta` dice `IGUAL`. Lo
unico que le queda es **orden de emision**, y esta acotado: de 37.026 simbolos
comunes del enlace, los que caen en otra direccion pasaron de **1.619 a 147**
(138 de `.text`, 9 de `.rodata`, **0 de `.data`**), y el `dolwhere` de 165.896 B
a **17.269 B**.

Ninguna de las cinco fuentes que toco la compila otra unidad (§7).
`configure.py`, `config/GOWE69/*`, `splits.txt` y `keep.lst` **sin tocar**; hace
falta **una** linea de `keep.lst` (§6) y **no** es atomica: sin ella el arbol
mide lo mismo.

---

## 1. La medida que ha desatascado esto: los SIMBOLOS del ENLACE, no los bytes

`dolwhere`/`dolrod` dan bytes y sobre una seccion desplazada sacan cien lineas
de trozos. Lo que hace falta es **enfrentar la tabla de simbolos de los dos ELF
enlazados** (base contra base-con-la-unidad-sustituida) y quedarse con los que
**cambian de direccion**, agrupados por seccion y por delta:

    1619 simbolos movidos de 37025 comunes
    Counter({'.data': 1466, '.text': 142, '.rodata': 11})
    [(-16, 1439), (-180, 93), (-8, 12), (-4, 11), (-212, 10), ...]

Eso se lee de un vistazo: *«hay un desfase de 16 B en `.data` que arrastra 1.439
simbolos, un bloque de 93 funciones a −180 y once vtables sueltas»*. Y el
**primer** simbolo movido de cada seccion es el punto exacto donde se rompe.

Es `permorden` generalizado a datos y **con las direcciones del enlace**, no del
objeto. La sonda esta en el scratchpad (`movidos.py`, 45 lineas); si os parece,
va a `scripts/` — es lo unico con lo que este frente se lee.

**Trampa medida al hacerla**: `linkdelta` compara TAMANOS de seccion del ELF. Con
las nueve secciones a `IGUAL` **el DOL seguia teniendo 165.896 B distintos**,
porque el tamano de la `.data` de `zMain` era el correcto y el ORDEN de sus
simbolos no. `linkdelta IGUAL` es condicion necesaria, nada mas.

## 2. `zMain`: los −32 B de `.data` eran CINCO huecos, no un manejador de Attrib

La r52 dejo escrito que *«al objetivo le sobra un manejador de tipo de Attrib en
codegen.cpp: todas las entradas de `_6Attrib.kTypeHandlers` apuntan 0x20 mas
arriba»*. **Es falso, y conviene que quede escrito**: los `kTypeHandlers`
apuntan a `.bss`, y apuntaban 0x20 mas abajo **porque la `.data` de la imagen
salia 32 B corta y el `.bss` entero se desplazaba con ella**. Es la sombra del
propio deficit, no su causa. La causa se ve comparando **los simbolos de
`.data` de los dos objetos**, uno al lado del otro:

| offset | objetivo | nuestro | que es |
|---|---|---|---|
| `0x14` | `prev_mode` = **5** | `prev_mode` = 0 | inicializador distinto |
| `0x18` | `gap_06_8041D804_data` (4 B a cero) | — | hueco |
| `0x7B4` | `gap_06_8041DFA0_data` (4 B a cero) | — | hueco |
| `0xC6C` | — | `…Instanceable…IEngine…._mHNext` (4 B) | **definicion de mas** |
| `0xC84` | `lbl_8041E470` = `{0x2E28, 0, 0}` (12 B) | — | dato |
| `0xC90..0xCD4` | `inputsys, input_buzz, input_effects, GameDevice::mCount, gShowPortInfo, pad_ticker, pad_elapsed_ms` | **otro orden** | orden de declaracion |
| `0xCD8` | `lbl_8041E4C4` = `0x37800080` (4 B) | — | dato |
| `0xCFC` | `gap_06_8041E4E8_data` (32 B a cero) | — | hueco |

56 B que faltan − 4 B que sobran = **52 B en el objeto**, `−32` una vez enlazado.
Los cinco huecos estan en `keep.lst` (lineas 441-445), o sea **vivos en la base**.

### 2.1 Los cuatro huecos y el dato: `asm()` en `.data`, en su sitio

Un `asm(".section .data")` sin simbolo **no lo puede tocar `-strip-unused-data`**
y **cae en el punto del fichero donde esta escrito** (a diferencia del `.bss`,
que es diferido: [[nfsmw-bss-y-huecos-estripados]]). Comprobado uno a uno:

* 4 B a cero antes de `#include EventDefs.cpp` → aparecen en `0x18`.
* 4 B a cero antes de `#include codegen.cpp` → aparecen en `0x7B4`.
* 12 B `{0x2E28,0,0}` antes de `#include InputDeviceGC.cpp` → `0xC84`.
* 4 B `0x37800080` en `SteeringWheelDevice.cpp`, detras de `lgwheels` → `0xCD8`.
  Ahi **no vale ponerlo en `zMain.cpp`**: las dos estaticas de `UpdateForces`
  (`previousVelocity`, `timeAtCollision`) se emiten al terminar la funcion, o
  sea antes que cualquier `asm()` del final del `.cpp`.
* 32 B a cero al final de `zMain.cpp` → cierran la seccion.

Contenido leido del objeto extraido, no adivinado; ninguno lleva reubicacion.

### 2.2 `prev_mode = SND_STREETRACE`

`EPursuitBreaker.cpp` tenia `static eSndGameMode prev_mode = (eSndGameMode)0;` y
el objetivo guarda un **5** ahi, que es `SND_STREETRACE` en `eSndGameMode`
(`EAXAudioParams.hpp`). Cambia un dato, no una instruccion: `fncmp` sigue en 0.

### 2.3 El `_mHNext` que sobraba, y por que sobra

`IMPLEMENT_INSTANCABLE(EventSequencer::HENGINE, EventSequencer::IEngine)` de
`EventSequencer.cpp` define **dos** simbolos: `_mList` (que el objetivo SI tiene,
en su `.bss`) y `_mHNext = 0` (que el objetivo tiene **UNDEF**). Buscando el
simbolo en los 604 objetos extraidos sale **uno solo**:
`auto_09_804FFED8_sbss.o` — o sea que en el original vive en `.sbss` y `zMain`
solo lo REFERENCIA.

Arreglo, **sin tocar la cabecera compartida**: `#undef IMPLEMENT_INSTANCABLE` +
un `#define` propio en `zMain.cpp`, justo antes del `#include`, identico salvo
que el `_mHNext` va **sin inicializador** — con `template<>` y sin inicializador
es una declaracion y cc1plus no emite nada (ya lo tenia medido el comentario de
`UCollections.h`). Lleva un `#ifndef IMPLEMENT_INSTANCABLE / #error` al lado
para que no pase en silencio si algun dia cambia el orden de includes.

Si preferis la version limpia, es una guarda mas en `UCollections.h`
(`UTL_IMPLEMENT_NO_HNEXT`); **no la he aplicado** porque la cabecera es
compartida. El `#undef` local no toca a nadie mas.

### 2.4 El orden de `InputDeviceGC.cpp`

El objetivo declara `inputsys` **antes** de `input_buzz`, y `gShowPortInfo`,
`pad_ticker` y `pad_elapsed_ms` **detras** de `int GameDevice::mCount = 0;`.
Nuestro fichero los tenia en orden de fuente. Movidos (subiendo `inputsys` con un
`namespace RealInput { struct Interface; }` delante, y bajando los otros tres
detras de `mCount`), la `.data` casa **direccion a direccion**: de 1.466
simbolos de `.data` movidos a **0**.

### 2.5 La tabla

| paso | `linkdelta` de `zMain` | simbolos movidos en el enlace | `fncmp` |
|---|---|---|---|
| partida (r52) | `data−32` | — (no medible) | 0/1380 |
| + 32 B al final de `.data` | **IGUAL** | 1.619 (`.data` 1.466) | 0/1380 |
| + los 4 huecos/datos y `prev_mode` | IGUAL | 158 | 0/1380 |
| + fuera el `_mHNext` | IGUAL | 158 → 153 | 0/1380 |
| + el orden de `InputDeviceGC.cpp` | IGUAL | **153 → 147** | 0/1380 |

## 3. `zMain`: lo que queda es UNA funcion mal colocada y diez vtables

Los 147 que quedan son **138 de `.text` y 9 de `.rodata`**, y no son 147 causas:

**La grande, y esta identificada al simbolo.** 97 de los 138 son un bloque
contiguo a **−180 B** que empieza en `BuildMessageTable__16MBreakerStopCops`
(`0x801F8500`). Los 180 B son
`_._Q43UTL11Collectionst8Listable2Z11ActionQueuei20_4List`
(`~UTL::Collections::Listable<ActionQueue,20>::_List`), que el objetivo emite en
`0x801F844C` —justo delante de ese bloque— y nosotros **9.204 B mas tarde**.
Colocar esa unica funcion arregla 97 de las 138.

Lo que SI he conseguido mover, y como: el objetivo completa
`Listable<ActionQueue,20>::_List` **antes** que
`Instanceable<HENGINE,IEngine,434>::_List` (sus dos vtables salen al reves, ver
[[nfsmw-inline-es-posicion]]). En el preprocesado nuestro, `class IEngine` cae en
la linea 93.659 (`ECellCall.cpp` incluye `Main/EventSequencer.h`) y
`class ActionQueue` en la 94.205 (`EChangeState.cpp`): **dos includes de
diferencia**. Con un `#include "…/Input/ActionQueue.h"` delante de
`#include ECellCall.cpp` —la guarda de cabecera vuelve no-op el include de
despues— `permorden` baja de **15 a 11** y **las dos vtables se emparejan**.
Pero el DESTRUCTOR sigue tarde: la vtable y su metodo clave no viajan juntos
aqui.

**Negativo con su cifra**: ese `#include` sube `dolwhere` de 17.195 a **17.269 B**
mientras baja `permorden` 15→11, los simbolos movidos 153→147 y las vtables
descolocadas 11→9. El recuento de bytes de `dolwhere` **no es monotono** con el
orden (una funcion de 180 B que se mueve cambia cuantos bytes coinciden por
casualidad); me he quedado con las tres medidas estructurales. Cuesta **una**
linea de `keep.lst` (§6).

Las 9 vtables que quedan, por parejas intercambiadas:

    _vt.12EEngineBlown  +32  <->  _vt.20EShowRaceOverMessage  -32
    _vt.9ECellCall      +32  <->  _vt.13ESndGameState         -32
    _vt.11EAccelerate  +128       _vt.11InputDevice           -32
    _vt.FixedVector<_KeyedNode,434,16>  +128
    _vt.FixedVector<ActionQueue*,20,16> -64   _vt.Vector<ActionQueue*,16> -64

La ultima terna tiene la misma raiz que §3: `FixedVector<_KeyedNode,434,16>` nos
nace en `class IModel : … Instanceable<HMODEL,IModel,434>` (preprocesado 35.406),
much antes que las de `ActionQueue`; el objetivo la tiene despues.

## 4. `zPhysics`: la ventana de 32 B del DOL, que cambia el objetivo del trabajo

**Este es el hallazgo util de la unidad.** El DOL **rellena cada seccion a 32 B**.
Medido con el enlace, no deducido:

    ELF .rodata nuestra    DOL sec5 obj      DOL sec5 nue     sec6 (.data)
    312.568  (−40)         312.608           312.576  (−32)   desplazada −32
    312.584  (−24)         312.608           312.608  (=)     **misma direccion y tamano**
    312.616  (  +8)        312.608           312.640  (+32)   desplazada +32

O sea: **`zPhysics` no necesita `.rodata` a delta 0, necesita estar en `[−31, 0]`**.
En `−24` el DOL sale **del mismo tamano que el original** (4.541.888 B) y las
secciones `.init/.text/.over/.ctors/.dtors/.rodata/.data` salen **identicas en
direccion y tamano**; solo `.sdata` y `.sdata2` quedan a −32, y eso era el
`bss−32`. Es un objetivo mucho mas alcanzable que el `−40` del brief.

### 4.1 Las dos cadenas que faltaban de verdad

`prefijotu.py` daba tres (`CameraMessagePort`, `SceneryOverrideConn`,
`Pkt_SceneryOverride_Open`). **`CameraMessagePort` ya la emitiamos** (offset
`0x758` de nuestra `.rodata`; el objetivo la tiene en `0x638`): es un problema de
POSICION y `prefijotu` no lo distingue — **cuenta contenido, no sitio**. Anadirla
metia una copia duplicada.

Las otras dos si faltan de verdad: nuestro arbol no tiene la clase de conexion
`SceneryOverrideConn`, y las 14 cadenas del bloque
`CarRenderConn…Pkt_Smackable_Service` salen de un solo parseo de `RenderConn.h`
con estas dos **en medio** (posiciones 120-121 del objetivo). Van en un `asm()`
**al final del `.cpp`** —receta del vocabulario, cero desplazamiento de `$LC`,
`lcfix --check` limpio—, igual que ya hacen `zPhysicsBehaviors.cpp` y
`zWorld.cpp` con estas mismas dos.

`rodata` pasa de **−40 a +8**: contenido correcto, pero **8 B fuera de la
ventana**.

### 4.2 Como meterlo en la ventana, medido y NO aplicado

Sobran ~24 B de literales muertos de cabecera que el objetivo no tiene
(`dupstr`: `EventSequencerSystems` 22 B, `EAGL4::SymbolEntry` 19 B, `done` 5 B,
`Attrib::Gen::emitterdata` 25 B, `emittergroup` 26 B; del enlace sobreviven
`stems` 6 B y `done` 5 B por el `size & ~7`). Las dos guardas los quitan:

    #define DEAD_STR(s)       ((const char *)0)   -> .rodata del objeto -24 B  => -16, DENTRO
    #define HAND_POOL_TAG(s)  ((const char *)0)   -> .rodata del objeto -44 B  => -32, FUERA

**Ninguna de las dos la he dejado puesta**, y por dos razones distintas, las dos
medidas:

1. **Renumeran los `$LC`** y `keep.lst` deja de apuntar a las cadenas que debe:
   con `HAND_POOL_TAG` puesto y el `keep.lst` del arbol, `linkdelta` daba
   `rodata−272`. Hace falta `lcfix.py zPhysics` + `deadlink --keep`, y eso es
   `config/GOWE69/keep.lst`, que no es mio. Con seis agentes en el arbol tampoco
   podia dejarlo tocado un rato para medir.
2. `DEAD_STR` **cambia ademas el `.text` del objeto en −4 B**
   (`bStrCmp(Name(), DEAD_STR("done"))` pasa a comparar contra NULL). Cae en
   codigo que el enlazador estripa —`linkdelta` sigue a `.text +0`— pero no lo
   he dejado sin verificar funcion a funcion.

**Recomendacion**: `DEAD_STR` + `lcfix` + `deadlink` deja `zPhysics` en `−16`,
dentro de la ventana, con las dos cadenas puestas. Es el siguiente paso obvio y
cuesta una tanda de `keep.lst`.

### 4.3 El `bss−32`: no era 32, eran 80 que faltan y 32 que sobran

No lo habia mirado nadie. Comparando los simbolos **con nombre** de la `.bss` de
los dos objetos, el desfase es **constante y de un solo escalon**: `+16` a partir
de `kFloatScaleUp`, y despues nada mas. Lo que hay es:

* al objetivo le faltan a nuestro objeto los **80 B finales**
  (`gap_07_80484E68_bss`, ya listado en `keep.lst:509`, o sea vivo en la base);
* y nosotros metemos **16 B de mas** en la zona de estaticas anonimas de delante
  de `kFloatScaleUp` (cuatro ranuras de 4 B de pares `hash`/`_.tmp` de inlines
  que el objetivo no instancia ahi).

El `.bss` **es diferido**, asi que el hueco no se puede escribir con `asm()`
([[nfsmw-bss-y-huecos-estripados]]): hay que **declarar otra variable** detras de
la ultima. Puesto `char _zphys_bss_tail[80] asm("gap_07_80484E68_bss");` al final
de `zPhysics.cpp`, `bss` pasa de **−32 a +32**.

Y la medida que cuantifica el exceso: **con `[48]` en vez de `[80]`, `linkdelta`
da `bss` a CERO**. O sea que sobran exactamente **32 B** de `.bss` nuestra. He
dejado el **80**, que es el tamano real del hueco del objetivo y el que nombra
`keep.lst`; poner 48 haria que `linkdelta` mintiera
([[nfsmw-medidas-que-enganan]]). **Lo que queda es encontrar y quitar esas 32 B
de estaticas de mas**, y entonces `bss` cierra solo.

### 4.4 La tabla

| paso | `linkdelta` de `zPhysics` | `fncmp` |
|---|---|---|
| partida (r52) | `rodata−40 bss−32` | 0/718 |
| + las dos cadenas de `SceneryOverride` | `rodata+8 bss−32` | 0/718 |
| + `_zphys_bss_tail[80]` | **`rodata+8 bss+32`** | **0/718** |
| *(sonda, no aplicada)* `[48]` en vez de `[80]` | `rodata+8` (`bss` a CERO) | 0/718 |
| *(sonda, no aplicada)* solo una cadena | `rodata−24 bss−32` → **DOL del mismo tamano, `.rodata`/`.data` identicas** | 0/718 |

## 5. Negativos, con su cifra

* **Ninguna de las dos da `DOL OK`.** `zMain` `dab74065b7af`, `zPhysics`
  `0d27a7c17d3a`, juntas `bedf8a414fd0`.
* **`zMain` con las nueve secciones a `IGUAL` sigue con 17.269 B de DOL
  distintos**: 138 funciones y 9 vtables en otro orden. Es el frente de
  [[nfsmw-orden-de-emision]] y no lo he cerrado.
* **`zPhysics` sigue con las 24 cadenas permutadas** que diagnostico la r52
  (`rodorden`: 159 de 183 en secuencia, igual que antes). No he tocado el bloque
  de `zPhysics.cpp:79..151`; las ideas del brief (partirlo, mover solo los
  `#include`) chocan con que **son esos mismos `#include` los que crean las 22
  cadenas** y los `IMPLEMENT_LISTABLE` de debajo los necesitan completos.
* **`prefijotu.py` da falsos positivos de contenido** (`CameraMessagePort`):
  cuenta cadenas del objetivo que no ve en su sitio, no las que no emitimos.
  Verificar siempre buscando la cadena en el `.rodata` del objeto.
* **Mi censo por contenido de la ventana enlazada tambien miente** cuando la
  cadena va precedida de bytes binarios (un float): el troceo por `\0` mete el
  float dentro del token y lo descarta. Por eso daba `CameraMessagePort` como
  ausente. Corregido leyendo el objeto.
* **El `#include ActionQueue.h` de §3 sube `dolwhere` 74 B** mientras baja tres
  medidas estructurales. Si alguien prefiere el criterio de bytes, se quita en
  una linea (y entonces la linea de `keep.lst` de §6 sobra).

## 6. Lo que hace falta de fuera: UNA linea de `keep.lst`

    linea 2968:   # @lc zMain "done"
                  zMain.o:$LC457      ->      zMain.o:$LC458

Sale sola con `python scripts/lcfix.py zMain`. Es la unica: `lcfix --check` sobre
el arbol tal como lo dejo saca **esa y ninguna mas** de las mias (la de
`zSpeech.o:$LC915` es de otro agente). La causa es el `#include ActionQueue.h`
de §3, que interna un literal antes.

**El paquete NO es atomico**: sin esa linea el arbol mide exactamente lo mismo
(`zMain IGUAL`, `zPhysics rodata+8 bss+32`, `fncmp` 0 y 0). La linea solo evita
que `keep.lst` mantenga viva la cadena equivocada.

**No pidais promocion de ninguna de las dos.**

## 7. Ficheros tocados

| fichero | que | quien lo compila |
|---|---|---|
| `SourceLists/zMain.cpp` | tres `asm()` de `.data` (4+4+12 B), los 32 B del final, el `#undef IMPLEMENT_INSTANCABLE` de §2.3 y el `#include ActionQueue.h` de §3 | solo zMain |
| `Generated/Events/EPursuitBreaker.cpp` | `prev_mode = SND_STREETRACE` | solo zMain |
| `Input/Common/SteeringWheelDevice.cpp` | `asm()` de 4 B (`0x37800080`) detras de `lgwheels`, bajo `ZMAIN_HAND_POOL` | solo zMain |
| `Input/GC/InputDeviceGC.cpp` | orden de declaracion de siete globales (§2.4) | solo zMain |
| `SourceLists/zPhysics.cpp` | `asm()` con las dos cadenas de `SceneryOverride` y `_zphys_bss_tail[80]` | solo zPhysics |

**Cero cabeceras compartidas.** Comprobado con `grep -rl` sobre `src/`: los tres
`.cpp` de fuera de `SourceLists/` los incluye **unicamente** `zMain.cpp`.
Ninguna unidad ajena cambia. Nada de `configure.py`, `config/GOWE69/*`,
`splits.txt` ni `keep.lst`; ni un commit ni un `git add`; no se ha lanzado
`ninja` ni `configure.py` (solo `scripts/build_direct.py zMain zPhysics`).

## 8. Para la ronda siguiente

1. **`zMain` esta a CERO de tamanos y a UNA funcion de arreglar 97 de sus 138
   desplazadas**: `~Listable<ActionQueue,20>::_List`, que el objetivo emite en
   `0x801F844C` y nosotros 9.204 B despues. El resto son diez vtables por
   parejas. Es el frente de `linked` puro, sin dato de por medio.
2. **`zPhysics` necesita `.rodata` en `[−31, 0]`, no en 0** (§4). Con
   `DEAD_STR(s) = NULL` + `lcfix` + `deadlink` cae en `−16` con las dos cadenas
   puestas; y con las 32 B de `.bss` de mas quitadas, **las nueve secciones
   cierran**. Eso deja la unidad en el mismo sitio que `zMain`: solo orden.
3. **La ventana de 32 B del DOL vale para TODAS las unidades**: `linkdelta` mide
   el ELF, y el DOL rellena a 32. Cualquier unidad con `rodata`/`data` entre
   `−31` y `0` **ya tiene esa seccion del DOL bien**. Merece la pena pasar
   `dolsec.py` (scratchpad) por el frente entero: puede que varias de las que se
   dan por cortas ya esten dentro.
4. **`movidos.py` (§1) es la herramienta que falta**: enfrenta las tablas de
   simbolos de los dos ELF ENLAZADOS y dice, por seccion y por delta, que se
   mueve y desde donde. Es lo que convirtio «`zMain` tiene 165.896 B de DOL
   distintos» en «le falta un hueco de 4 B en `0x18`». Va a `scripts/` si os
   parece.
5. **Ojo con `prefijotu.py`**: da como ausentes cadenas que emitimos fuera de
   sitio (§5). Antes de escribir una a mano, buscarla en el `.rodata` del objeto.
