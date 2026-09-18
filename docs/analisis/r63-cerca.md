# r63 -- "cerca": zLua, zTrack, zSpeech, zMisc

**Resultado: -262 B de DOL en zTrack y `.rodata` de zMisc de +616 a +560, las dos
con sello triple y `lcfix --check` rc=0. Ninguna unidad promociona.** Ademas, la
palanca que valio zTrack esta ACOTADA en el codigo de GCC y es reutilizable: es
el orden de `saved_inlines`, y se mueve desde la fuente.

| unidad | dolwhere ANTES | DESPUES | linkdelta | textorder ANTES -> DESPUES |
|---|---|---|---|---|
| **zTrack** | 3.956 B | **3.694 B** | `.text +0`, resto IGUAL | 6 descolocadas -> **2** |
| **zMisc** | (secciones no cuadran) | idem | `rodata+616` -> **`rodata+560`**, `data+32` | 0 -> 0 (ORDEN PERFECTO) |
| zLua | 9.152 B | 9.152 B | `.text +0`, resto IGUAL | 2 descolocadas, 3 saltos |
| zSpeech | 11.264 B | 11.264 B | `.text +0`, resto IGUAL | 67 descolocadas, 19 saltos |

`trypromo zTrack zLua zSpeech zMisc` -> DOL ROTO (f600cfbc41f3). Sin promocion.

---

## 1. zTrack: el orden de la cola de inlines diferidos, y de donde sale

`dolwhere zTrack` daba un bloque de **300 B** en
`ReadyToMakeSpaceInPoolBridge__13TrackStreameri +0xC`. `textorder` lo nombraba:
el bloque de inlines diferidos del final de la unidad tenia
`ClearTable__9bBitTable` (48 B) al final en vez de al principio.

**El mecanismo, leido en el arbol de GCC 2.95 que tenemos** (`orig/prodg/NGC_GNU_SRC/NGC/gcc/cp/`):

* `finish_file` (`decl2.c:3794`) vuelca `saved_inlines` con
  `wrapup_global_declarations`, **en el orden del array**.
* El array se llena en `mark_inline_for_output` (`decl2.c:2140`), y a esa la
  llama `finish_function` (`decl.c:14701`): **el orden es el de la DEFINICION
  del cuerpo**, no el del uso ni el de la declaracion.
* Los cuerpos escritos DENTRO de la clase no cuentan donde se leen sino donde
  **se cierra la clase**: `do_pending_inlines` (`lex.c:1244`) los desapila (y los
  invierte, que por eso salen en orden de declaracion) desde las reglas del
  parser.

Con eso el orden de nuestro bloque diferido es exactamente el de definicion:
`ReadyToMakeSpaceInPoolBridge` (cuerpo en clase, `TrackStreamer.hpp:344`, o sea
la linea 1 de `TrackStreamer.cpp`), los cuatro `Override*` de `TSMemoryPool`
(cuerpos en clase, `TrackStreamer.cpp:136..160`) y por ultimo `ClearTable`
(`inline` fuera de clase, `TrackStreamer.cpp:512`). El objetivo pone `ClearTable`
**la primera**.

### Los dos negativos que hacen falta para entender la cura

* **A -- mover la definicion de `ClearTable` MAS TARDE** dentro del fichero
  (delante de `RemoveCurrentStreamingSections`, que es el otro sitio donde SI se
  inlinea): `textorder` **identico**, 6 descolocadas. La posicion dentro del
  fichero, entre dos puntos que no cruzan un cierre de clase, no manda.
* **B -- mover la definicion delante de `#include "TrackStreamer.hpp"`** (que es
  lo unico que la pone antes que `ReadyToMakeSpaceInPoolBridge`): entonces el
  constructor la **INLINEA**, el simbolo desaparece
  (`SOLO EN EL OBJETIVO: ClearTable__9bBitTable`), `TrackStreamer::TrackStreamer`
  crece 8 B y salen **189 de 259 descolocadas**.

O sea: adelantar la definicion y conservar la llamada fuera de linea son
INCOMPATIBLES por la regla de "definida despues del uso" de GCC 2.9.

### La cura (aplicada)

Definicion arriba del todo **mas** un alias mangled que fuerza la llamada fuera
de linea en el unico sitio donde el objetivo la hace:

```c
#include "Speed/Indep/bWare/Inc/bMath.hpp"
inline void bBitTable::ClearTable() { bMemZero(Bits, NumBits >> 3); }
extern void _ClearTable_ool(bBitTable *) asm("ClearTable__9bBitTable");
...
    _ClearTable_ool(&CurrentVisibleSectionTable);   // era CurrentVisibleSectionTable.ClearTable()
```

* `fncmp` **no se mueve**: la misma unica funcion distinta que antes
  (`GetLoadingPriority__13TrackStreamerP21TrackStr`, 708 B, y esa esta muerta en
  el enlace).
* `textorder` 6 -> **2** descolocadas.
* `dolwhere zTrack` **3.956 -> 3.694 B**.
* `lcfix zTrack --check` rc=0, **cero** correcciones. No renumera ningun `$LC`.
* Sello triple de `zTrack.o`: `f397b0c50b78bb9e5f400fe1033a0e6aea68ff25`.

Las **2 descolocadas que quedan** son `__8bVector3RC8bVector3`: nosotros la
DEFINIMOS (32 B) y el objetivo solo la referencia (`SHN_UNDEF`). **No cuesta un
byte**: no la referencia nadie (cero `R_PPC_REL24` a ella en los DOS objetos) y
`-strip-unused-data` se la lleva. Por eso el objeto sigue con delta +32 y el
enlace ya no.

## 2. zTrack: los 3.694 B que quedan son UN solo bloque de `.rodata`

`dolrod zTrack .rodata` lo dice de una: los primeros 604 B casan y despues

```
SOBRA nue 804091D4  348 B  47414d4543554245...  ("GAMECUBE", "Attrib::Attribute"...)
FALTA obj 804091DA    4 B  0000000043300000      (una constante de coma flotante)
```

**El primer volcado de pool del objetivo son EXACTAMENTE los 0x21C B que
escribimos a mano en `lbl_80408FB8`** (mapa hecho con
`scratchpad/cerca63/rodmap.py obj zTrack`: "GAMECUBE", el `__FILE__` de EA,
"bad_alloc", los dos "%f,%f,%f", "STL", las ocho etiquetas de AttribSys,
`Attrib::Gen::effects`, `Attrib::TAttrib`, ..., `Pkt_Effect_Service`, y en 0x21C
ya empiezan los floats). Nosotros escribimos ese bloque **y ademas** cc1plus
interna su propia copia de 26 cadenas detras. Todo lo demas del `dolwhere`
--los ~340 bloques de 2 B en `SetPoints`, `AddSegment`, `DoTunnelBloom`...-- son
las mitades bajas (`@l`) de esas mismas cadenas corridas 348 B.

Por contenido la unidad esta bien: `.rodata` del objetivo 138 cadenas, la
nuestra 166, y la diferencia de CONJUNTOS es **`Zone` de menos** y once de mas
(`EventSequencerSystems`, `Attrib::Gen::{chassis,induction,nos,tires,transmission,gameplay}`,
`done`, `GManager`, `EAGL4::SymbolEntry`, `VehicleParams`). **Es orden, no
contenido.**

### El fudge de 160 B que hay montado, medido

Once entradas `@lc` de `keep.lst` mantienen vivas cadenas muertas que **ya estan
dentro de `lbl_80408FB8`** (las ocho etiquetas, `Attrib::TAttrib` y
`Attrib::Gen::simsurface`) o que **el objetivo no tiene en ninguna parte**
(`Attrib::Gen::gameplay`). Quitandolas de una COPIA PRIVADA de `keep.lst`:

| quito | `.rodata` del ELF |
|---|---|
| las 10 duplicadas | **-128 B** |
| las 10 + `Attrib::Gen::gameplay` | **-160 B** |

Y `stripmap zTrack` dice `estripado -160 B, dato -160 B`. **Son la misma cifra:
hoy la seccion cuadra porque 160 B de cadaver compensan 160 B de dato del
original que el enlazador nos estripa.** `deadlink zTrack` no propone ni una
entrada nueva (0 B), asi que el dato que falta NO son cadenas muertas. Hasta que
aparezca de donde salen esos 160 B, quitar las once entradas descuadra las
secciones y `dolwhere` ni arranca. **No las propongo todavia**: apuntarlas aqui
es el trabajo, retirarlas sin el contrapeso seria trabajo a medias.

### Ensayo negativo de la ronda sobre lo mismo

`#define ATTRIB_TAG_ORDER_HAND_POOL 1` **SOLO** (la r62 probo las dos guardas a
la vez y la conclusion fue que "la guarda es una sola"; no lo es, son
`AttribSys.h:43` y `AttribSys.h:61`): el objeto cambia
(`63d67e50...` -> `779e638e...`) pero el `dolwhere` sale **byte a byte identico**
(3.956 B, diff de salidas vacio). Las ocho etiquetas que interna
`_AttribAllocTagOrder` estan tambien internadas por el `USE_ATTRIB_ALLOC` de las
clases de AttribSys, asi que apagar solo el ordenador no quita ninguna copia.

## 3. zMisc: 240 B de cadenas duplicadas, y 126 se van sin tocar cabecera

`dupstr zMisc` daba **21 cadenas de mas, 240 B**, todas con el patron
`['-', '$LCnnn']`: una copia en el bloque escrito a mano y otra internada por
cc1plus. Las cinco cabeceras de donde salen (`FEDatabase.hpp`, `GManager.h`,
`GRaceDatabase.h`, `GState.h`, `feScrollerina.hpp`) **ya traen la guarda
`#ifndef DEAD_STR` desde la r59**, asi que basta una linea en la SourceList:

```c
#define DEAD_STR(s) ((const char *)0)
```

Medido: `dupstr` 21 cadenas / 240 B -> **12 / 126 B**; `linkdelta zMisc`
`rodata+616` -> **`rodata+560`**; `fncmp` **0 de 450** con el codigo distinto;
`textorder` sigue en **ORDEN PERFECTO** (0 de 451). zMisc **no tiene ni una
entrada `@lc`** en `keep.lst`, asi que la renumeracion de `$LC` sale gratis
(`lcfix zMisc --check` rc=0). Sello triple:
`eef8b7c78b927b2903dc371dc662699c0b1f5900`.

**NEGATIVO**: anadir ademas `#define HAND_POOL_TAG(s) ((const char *)0)` no mueve
nada (`rodata+560` igual).

### PROPUESTA (cabeceras COMPARTIDAS: no la aplico)

Las 6 cadenas duplicadas que quedan (~66 B) salen de dos cabeceras sin guarda.
La forma es la misma que ya tienen las otras cinco:

* `src/Speed/Indep/Src/EAXSound/SoundConn.h`, delante de la primera clase:

```c
#ifndef DEAD_STR
#define DEAD_STR(s) s
#endif
```

  y en las lineas 63, 76, 97 y la de `Pkt_Car_Open`, cambiar
  `static UCrc32 hash = "HeliSoundConn";` por
  `static UCrc32 hash = DEAD_STR("HeliSoundConn");` (idem `Pkt_Heli_Open`,
  `CarSoundConn`, `Pkt_Car_Open`). La incluyen zEAXSound, zPhysicsBehaviors,
  zMisc y zWorld; sin `DEAD_STR` definido vale exactamente lo que vale hoy.
* `src/Speed/Indep/Src/World/DebugVehicleSelection.h:41`, igual, para
  `return DEAD_STR("DebugVehicleSelection");` (la usan zSim y zWorld).

Los otros tres sobrantes de zMisc (`MainLoop()`, `TODO2`, `%s - %s`) **no son
duplicados**: el objetivo NO los tiene (`objetivo x0`). Son diferencia de codigo
en `Main.cpp:386` y `ResourceLoader.cpp:712`, no de pool.

## 4. zSpeech: la unica funcion abierta, acotada a sched1

`Setup__Q26Speech13RoadblockFlow` (596 B, 98,255035 %) tiene **tres filas** y
son una permutacion:

```
objetivo:  lwz r9,0(r31) | lwz r0,0x2e4(r9) | lha r3,0x2e0(r9) | mtlr r0 | add r3,r31,r3 | blrl
nuestro :  lwz r9,0(r31) | lha r3,0x2e0(r9) | lwz r0,0x2e4(r9) | add r3,r31,r3 | mtlr r0 | blrl
```

**Las OTRAS CUATRO llamadas por puntero-a-miembro de esta misma funcion casan**,
y en las cuatro los dos lados emiten `lha` antes que `lwz`. Solo en la de
`primary->PursuitApproaching()` el objetivo ADELANTA la cadena de `lr`. Es
sched1 dando prioridad al camino `lwz -> mtlr -> blrl`.

Tres negativos nuevos sobre la veda de la r48 (anotados en `RoadblockFlow.cpp`
junto a la funcion, donde `previo.py` los encuentra):

1. **Quitar los tres `__asm__("# a"/"# b"/"# c")`** (los andamios caducan):
   596 -> **568 B** y 98,26 % -> **92,52 %**, 14 filas. Siguen haciendo falta.
2. `__asm__("# d");` detras de la llamada: 596/596, **98,255035 % exacto**, las
   mismas 3 filas.
3. Barrera de ranura con clobber de `r0`
   (`__asm__ __volatile__("" : : : "r0")`) delante Y detras de la llamada:
   596/596, 98,255035 %, las mismas 3 filas.

**Lo que falta es una palanca que ADELANTE, no una que frene**: las tres formas
de barrera del proyecto retrasan. En el DOL vale 16 B.

Ojo con la sintaxis: `__asm__("" ::: "r0")` **no compila** con este cc1plus; hay
que escribir `__asm__ __volatile__("" : : : "r0")`. `build_direct` lo canta con
`0 ok, 1 fallidas`.

## 5. Lo que queda en zSpeech y zLua, dimensionado

`dolwhere zSpeech` = 11.264 B: **4.803 de `.text`** y 6.461 de datos. De los
4.803, **3.476 estan en dos bloques** (`GetHandle__12EAXCharacter` 2.796 B y
`_._Q26Speech13SampleReqList` 680 B) y son el MISMO frente que zTrack: el orden
de la cola de inlines diferidos. El objetivo saca ahi el bloque entero de
accesores de `EAXCharacter` y `EAXCop` detras de los de `SpeechFlow` y de
`ClassKey__Q36Attrib3Gen10speechtune`; nosotros los sacamos ~49 posiciones
despues. Con el mecanismo de la seccion 1 eso se ataca moviendo el punto donde
se CIERRA cada clase, o sea el orden de los `#include` de `zSpeech.cpp`
(`EAXCharacter.cpp` en la 229, `EAXCop.cpp` en la 235, `SpeechFlow.cpp` en la
237). Son 19 saltos y 67 funciones: no cabe en una tanda, pero ya no es ciego.
El resto (todos los bloques de 13 y 33 B) son mitades bajas de `.rodata`.

`dolwhere zLua` = 9.152 B y es casi todo pool: los bloques de `.text` son
mitades bajas de 1-2 B en los `_GetKind__*`, y los grandes son `$LC229`,
`$LC231` (2.789 B), `$LC427`, `$LC587`, `$LC780`, mas `accessorTable.12144` y
`flagMapping.25652`, que son tablas de punteros A ESAS CADENAS y **se arreglan
solas** cuando el pool se ordene.

## 6. Sorpresa: un byte NUL dentro de `zSpeech.cpp`

`src/Speed/Indep/SourceLists/zSpeech.cpp` tiene un `0x00` LITERAL en el
desplazamiento 1009, dentro de un comentario (el `CADAVER "stems<NUL>"` de una
nota de ronda). El fichero es UTF-8 valido y cc1plus lo traga, pero `grep` lo
declara binario y se salta el fichero entero sin decir nada:
`grep -n "include" zSpeech.cpp` no imprime NADA y hay que usar `grep -a`. No lo
he tocado (es un comentario y cambiarlo mueve el mapa de lineas), pero conviene
limpiarlo: cualquier herramienta que barra la fuente con `grep` esta ignorando
esta unidad en silencio.

## 7. Estado del arbol al cerrar

Ficheros tocados (los tres UTF-8 puro, **cero bytes > 127**, sin commit):

* `src/Speed/Indep/Src/World/TrackStreamer.cpp` -- la palanca de `ClearTable`
  mas los dos negativos A y B. **Solo lo incluye `zTrack.cpp`** (comprobado con
  grep sobre `SourceLists/` y `GameCube/`).
* `src/Speed/Indep/SourceLists/zMisc.cpp` -- `#define DEAD_STR` y el negativo de
  `HAND_POOL_TAG`.
* `src/Speed/Indep/Src/Speech/RoadblockFlow.cpp` -- **solo comentario** (los
  tres negativos de la seccion 4).

`config/` sin tocar. Sellos (`build_direct.py`, tres veces identicos):

* `zTrack.o`  = `f397b0c50b78bb9e5f400fe1033a0e6aea68ff25`
* `zMisc.o`   = `eef8b7c78b927b2903dc371dc662699c0b1f5900`
* `zSpeech.o` = `7a883e990fbda7d6adb12b760a2da6a49bcfe249`
* `zLua.o`    = `0bf04a34791ca896c148f6c4596bf60ed7bd1927` (sin tocar)

**Correcciones de `lcfix` pendientes que dejo: CERO** (`lcfix.py zTrack --check`
y `zMisc --check`, las dos rc=0). Ninguna venenosa.

Sondas en `scratchpad/cerca63/` (`rodmap.py`, la unica que merece guardarse: el
mapa de `.rodata` de un objeto separando cadenas de constantes, con el simbolo
al lado).
