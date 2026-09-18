# r57 — `zAI`: la region A cae ENTERA, y el mecanismo es «quien pide primero»

Unidad: `Speed/Indep/SourceLists/zAI` (272.796 B de `.text`).

## Resumen en una linea

**El `.text` descolocado de `zAI` pasa de 26.532 B a 16.352 B: +10.180 B puestos en su sitio
en el ENLACE, del 90,27 % al 94,01 %, con CERO regresiones (`fncmp` 0 de 1030 antes y
despues).** Los 10.180 B son, al byte, **toda la region A**: de `0x800034A0` a `0x80041A80`
—zona de parseo, cola de plantillas y `__static_initialization_and_destruction_0`— hay ahora
**un solo tramo a `d=+0` de 255.456 B**. Lo unico que queda descolocado es la **region B**
(los cuerpos en clase, 16.352 B). No promociona: sigue `DOL ROTO`, con `rodata` y `data`
pendientes.

    ANTES   1030 simbolos  EN SITIO 246.264 B   FUERA 26.532 B  (90,27 %)  139 tramos
    DESPUES 1030 simbolos  EN SITIO 256.444 B   FUERA 16.352 B  (94,01 %)  102 tramos
    permorden  894 de 1030 (era 868);  ciclos 5 (161,132,41,21,3) -- eran 10
    sha1 .o    097e7cede7bc14e7d58bd87065bc6167218d5a80  (TRES compilaciones, identico)
    linkdelta  .text +0   rodata-456  data-384      trypromo  DOL ROTO (bd6d01db8276)

---

## 0. El mecanismo, medido con sonda

La r56 dejo dicho que la region A es el FIFO de `add_pending_template` y que una
instanciacion explicita es una peticion. **Lo que faltaba es donde vale escribirla.** Medido
con una sonda (`_STL::find<IPlayer**>`, que en la base salia la #46 de la region A):

| donde se escribe la instanciacion explicita | puesto en la region A |
|---|---|
| detras del bloque de `#include` de `AITrafficManager.cpp` | **#22** |
| detras del `#include` de `AITrafficManager.hpp` (1a linea) | **#13** |
| **en `zAI.cpp`, DELANTE del primer `#include`** | **#1** |

O sea: **la cabeza de la cola se escribe entera desde el SourceList**. Y con eso viene la
regla que hace falta para usarlo:

> **Una cabecera pide sus propias plantillas al parsearse, y esa peticion GANA a cualquier
> instanciacion explicita que venga despues.** De modo que el bloque de cabeza no es «unas
> instanciaciones»: es una **lista intercalada de cabeceras mudas y peticiones**, y hay que
> saber de cada cabecera si pide algo.

Para eso queda `hdrprobe.py` (§7): compila una TU con solo esa cabecera y lista los simbolos
de `.text`. `UStandard.h`, `SimTypes.h`, `AttribSys.h`, `UMath.h`, `GReflected.h`, `UCOM.h`,
`UTypes.h` y `UVectorMath.hpp` piden **0**; `Hermes.h` pide 2; `WGridNode.h` pide **1**
(justo la que hacia falta); `IAI.h` pide **39**; `SoundAI.h` con `SpeechManager.hpp` delante,
**49**.

**La consecuencia practica**: cuando una cabecera pide EXACTAMENTE lo que toca y en el orden
que toca, no se escribe nada — se pone la cabecera en el renglon. Tres de las 24 posiciones
de la cabeza se colocaron asi (`Hermes.h` a `0x8003B5A8`, `WCollisionSTL.h` a `0x8003B72C` y
`0x8003B8E8`, `WGridNode.h` a `0x8003CD70`).

---

## 1. Las siete ediciones, en orden de lo que valen

### (1) `ICause.h`: `IExplosion.h` fuera, y entra por `AIVehicle.cpp` -> **+720 B**

`ICause` solo usa `IExplosion *` (un puntero en `OnCausedExplosion`), pero arrastraba
`IExplosion.h` entera, y con ella `Listable<IExplosion,96>` y `_STL::find<IExplosion**>`.
Como `ICause.h` cuelga de `IModel.h` <- `ISimable.h` <- `IVehicle.h` <- `AITrafficManager.hpp`,
eso ponia `find<IExplosion**>` la **#2 de toda la cola**; el objetivo la tiene la **#63**
(`0x8003D140`), la ultima de la region A.

Guarda inerte `ICAUSE_FWD_IEXPLOSION` (patron `EVENTSEQ_FWD_IENGINE` de la r51). Al apagarla,
**el compilador dice solo quien necesita de verdad el tipo completo**: `AIVehicle.cpp:2177`,
`AIPerpVehicle::OnCausedExplosion`. Se incluye `IExplosion.h` ahi y la peticion cae **exacta**
en `0x8003D140`.

### (2) La cabeza de la region A escrita entera -> **+5.336 B**

24 posiciones, `0x8003A9F4`..`0x8003BB4C`, en el orden del objetivo, delante del primer
`#include` de `zAI.cpp`. Se ponen 21 instanciaciones explicitas y 3 cabeceras:

    Get<TAttrib<UMath::Vector4>>   __lower_bound<const Ui*>   __lower_bound<const f*>
    _List_base<IAttachable*>::clear   find<IPlayer**>   find<Sim::IEntity**>
    Get<TAttrib<bool>>   vector<int,Alloc<int,Speech::_type_voiceIDs>>::reserve
    [SpeechContainers.h]  ->  vector<Speech::copPair,...>::reserve
    find<IPursuit**> IRoadBlock ICollisionBody ISimpleBody IRigidBody ITrafficCenter
    IInputPlayer IVehicleCache IVehicle
    [Hermes.h]         ->  vector<Hermes::Handler,...>::reserve
    [WCollisionSTL.h]  ->  vector<WCollisionTri,...>::reserve + <WCollisionTriBlock*>
    Get<TAttrib<GCollectionKey>>   find<IModel**>

La 25a (`__lower_bound<PatternKey>`) **no se escribe**: la pide `AITrafficManager.hpp`, que es
lo siguiente que se parsea, y con las 24 de arriba ya consumidas cae sola en `0x8003BB04`.

**Los tags de contenedor (`_type_*`) valen declarados y sin definir**: `Allocator<T,Tag>` no
usa `Tag` para nada. `Speech::_type_voiceIDs` va como `struct` adelantado y no arrastra nada.

### (3) `SpeechContainers.h`: sacar `Speech::copPair` de `SoundAI.h` -> hace posible la #9

La unica de las 24 que necesita un tipo COMPLETO que no estaba a mano.
`vector<Speech::copPair>::reserve` necesita `copPair`, que vivia en `SoundAI.h` — y `SoundAI.h`
arrastra `IAI.h`, `IVehicle.h` e `ISimable.h`, o sea 49 peticiones que destrozan la cabeza.
El propio comentario de `SoundAI.h` dice que **en la rama estos contenedores viven fuera**
(`EAXSound/EAXSoundTypes.h`).

Traslado literal de `copPair` + `DECLARE_CONTAINER_TYPE(copMap)` + `copMap` a
`Src/Speech/SpeechContainers.h`, que `SoundAI.h` incluye **en el renglon exacto donde estaba
el bloque**. Y no hay que escribir la instanciacion: **el cuerpo en clase de
`copMap::copMap(int)` hace `reserve(size)`, asi que la cabecera la pide ella sola** — por eso
va en el renglon 9 y no arriba con las mudas (arriba, la peticion se adelanta a las ocho de
encima y sale la #1: medido, 250.188 B con `copPair` en `0x8003A9F4`).

### (4) `IGroundSupport.h`: sacar `GroundSupportRequest` de `IAI.h` -> **+2.680 B**

`_List_base<char*>::clear()` salia la #25 de la cola y el objetivo la tiene la #43
(`0x8003C54C`), **detras del cuerpo entero de `AITrafficManager.cpp`**. La pide el destructor
**en clase** de `GroundSupportRequest` (`IAI.h`), que destruye `mVehicleGoals`
(`UTL::Std::list<char*>`); e `IAI.h` entra por `AIVehicle.h`, el 3er `#include` de
`AITrafficManager.cpp` — y **`AITrafficManager.cpp` no puede prescindir de `IAI.h`**: usa
`IVehicleAI`, `ITrafficAI` y `IPursuit::Count` (medido quitando los dos `#include`, no
supuesto).

`IAI.h` solo usa `GroundSupportRequest *` (`IPursuit::RequestGroundSupport`). Traslado literal
a `IGroundSupport.h` con guarda inerte `IAI_H_FWD_GROUNDSUPPORT`, y `zAI.cpp` la incluye entre
`AITrafficManager.cpp` y el bloque de instanciaciones: la peticion cae en `0x8003C54C`.

**Antes de esto probe la version tonta** —quitar el destructor en clase y dejar el implicito—
y **REGRESA**: `_._9AIPursuit` baja de 936 a 784 B (`fncmp` 1 de 1030). El destructor en clase
es codigo del objetivo; lo que sobra es su POSICION, no el.

### (5) `find<EAX_HeliState**>` de `zAI.cpp` a `AICopManager.cpp` -> **+620 B**

El objetivo lo emite en `0x8003C6D0`, **entre** `find<EAX_CarState**>` (lo pide
`SpeechManager.hpp`) y `find<IHud**>` (lo pide `FeReputation.hpp`), las dos cabeceras de
`AICopManager.cpp`. Escrito entre las dos. Con eso `0x8003C5C4`..`0x8003C830` casa.

### (6) `AIActionPursuitOffRoad.cpp`: `WGridNode.h` y `reset()` delante de `get_sample` -> **+976 B**

`0x8003CD70`..`0x8003D140`: el objetivo pide `clear<WGridNodeElem>`, `reset()`, `get_sample()`
y `add_sample()` **en ese orden**; el arbol ya tenia escritas las explicitas de `get_sample` y
`add_sample` (lineas 102-103) pero no las otras dos, que salian detras. `WGridNode.h` pide
`clear<WGridNodeElem>` y **nada mas** (hdrprobe: 1 simbolo), asi que basta incluirla; `reset`
va como instanciacion explicita.

### (7) `find<IDebugWatchCar**>` delante de `AITarget.cpp` -> **+528 B**

La r56 lo dejo a 568 B, con la nota «hay que meterlo DENTRO de `AIGoal.cpp`». **La nota estaba
equivocada**: los cuatro simbolos que el objetivo pone detras (`find<ISimable*const*>`,
`find<ISimable**>`, `clear<List_base<AIAction*>>`, `CreateInstance<AIActionParams,AIAction>`)
se piden **antes** de `AIGoal.cpp`. Barrido de cinco posiciones: dentro de `AIGoal.cpp` da
216 B; delante de `AIAction.cpp`, 216 B; delante de `AISteer.cpp`, 216 B; **delante de
`AITarget.cpp`, 744 B**, y de ahi hacia atras no mejora. Se deja en la mas tardia que paga.

---

## 2. La medida

### `fncmp` — ANTES y DESPUES, identicos

```
ANTES     0 de 1030 funciones con el CODIGO distinto -- 0 B
          337 mas solo con nombres de simbolo distintos (199388 B)
DESPUES   0 de 1030 funciones con el CODIGO distinto -- 0 B
          337 mas solo con nombres de simbolo distintos (199388 B)
```

Corrido **despues de cada fichero**, no solo al final: es lo que caza la regresion de §1(4).

### El censo de desplazamiento ENLAZADO, paso a paso

| paso | EN SITIO | FUERA | % | delta |
|---|---:|---:|---:|---:|
| base r57 (= cierre r56) | 246.264 | 26.532 | 90,27 % | — |
| (1) `ICause` / `IExplosion` en `AIVehicle.cpp` | 246.984 | 25.812 | 90,54 % | **+720** |
| (2)+(3)+(5) la cabeza de la region A y `EAX_HeliState` | 252.044 | 20.752 | 92,39 % | **+5.060** |
| (6) `WGridNode.h` + `reset()` | 253.020 | 19.776 | 92,75 % | **+976** |
| (7) `find<IDebugWatchCar**>` | 253.764 | 19.032 | 93,02 % | **+744** |
| (4) `IGroundSupport.h` | **256.444** | **16.352** | **94,01 %** | **+2.680** |
| **total** | | | | **+10.180 B** |

### Lo que queda: **la region B, y nada mas**

```
  800034A0  d=+0   255.456 B  x651   <-- zona de parseo + region A + SIAD, TODO en su sitio
  80041A80 .. final          16.352 B  x379   <-- region B, permutada casi entera
```

---

## 3. Lo medido y REVERTIDO (cuatro negativos que ahorran una tarde)

**a) Quitar el destructor en clase de `GroundSupportRequest`** en vez de mover la clase.
Coloca la peticion igual de bien, pero **regresa `_._9AIPursuit` de 936 a 784 B**. El fix bueno
es el traslado con guarda (§1.4). *Un destructor vacio en clase no es ruido: es codigo.*

**b) Hoistar las cabeceras pesadas al principio y escribir las instanciaciones DEBAJO.**
Primer intento del bloque de cabeza: `UMath.h`, `GReflected.h`, `Hermes.h`, `SpeechManager.hpp`,
`SoundAI.h`, `WCollisionSTL.h`, `AttribSys.h` arriba del todo y las 24 explicitas detras.
Resultado: **+188 B**, porque las cabeceras piden lo suyo primero y la explicita que llega
despues no mueve nada. La version buena (cabeceras mudas arriba, las que piden **en su
renglon**) da +5.060. *Es la regla del §0 y costo dos compilaciones descubrirla.*

**c) `#include <algorithm>` al principio de `zAI.cpp` sin `_STLP_NO_FORCE_INSTANTIATE`.**
No enlaza: STLport fuerza sus instanciaciones globales. Con el `#define` delante es
**exactamente neutro** (246.264 antes y despues), y por eso el bloque de cabeza lo lleva.

**d) La cabeza de la region B** (`UVectorMath.hpp`/`UCOM.h` hoistadas para poner
`VU0_v3unit`, `~IUnknown` y el ctor de `bVector3` en el orden del objetivo, 180 B): **no
compila**. `UVectorMath.hpp` pide `eetypes.h`, que es de PS2, y `UVectorMathGC.hpp` **no tiene
guarda de inclusion** (se redefine entera al segundo `#include`). Revertido.

---

## 4. Las cabeceras ajenas: **la prueba de que son inertes**

Cinco ficheros fuera de `AI/`. Los dos con guarda no cambian **ni un token** para nadie mas,
y esta MEDIDO, no razonado: preprocesando (`-E`) la cabecera contra una copia intacta en el
scratchpad,

| cabecera | guarda | lineas de diff en el preprocesado |
|---|---|---:|
| `Interfaces/Simables/ICause.h` | `ICAUSE_FWD_IEXPLOSION` | **0** de 15.032 |
| `Interfaces/Simables/IAI.h` | `IAI_H_FWD_GROUNDSUPPORT` | **0** de 37.309 |
| `Speech/SoundAI.h` | *(traslado, sin guarda)* | **14** de 44.682 |

Las 14 de `SoundAI.h` son: los comentarios del traslado (lineas en blanco), un
`class EAXCop;` **redundante** (en ese punto `EAXCop` ya esta declarada) y un `}` +
`namespace Speech {` de cerrar y reabrir el espacio de nombres. Ninguna puede cambiar codigo.

`grep -rn` confirma que los dos macros de guarda **solo se definen en `zAI.cpp`**.

**No he compilado las unidades ajenas a proposito**: `build_direct.py` escribe en
`build/GOWE69/src/<unidad>.o`, que es justo lo que otro agente puede estar midiendo con
`trypromo`. La prueba por preprocesado es mas fuerte (flujo de tokens identico implica `.o`
identico) y no pisa a nadie. Las unidades afectadas, para la ventana: `SoundAI.h` la incluyen
`zSim`, `zCamera`, `zEAXSound`, `zEAXSound2`, `zLua`, `zSpeech` y `zAI`; `IAI.h` e `ICause.h`,
media docena mas.

---

## 5. Lo que le queda a `zAI`: **16.352 B, y son TODOS de la region B**

La region B es el orden de empuje en `saved_inlines`, o sea **el orden en que se COMPLETAN las
clases y se parsean los cuerpos en clase**. De sus 17.340 B solo 988 estan en su sitio.

**El hallazgo de la ronda para la region B, y es de mis ficheros**: dentro de una clase, los
cuerpos se parsean **en el orden de DECLARACION**, al llegar a la llave de cierre. En
`AIGoal.cpp` las trece clases `AIGoalX` declaran

    static AIGoal *Construct(ISimable *);   // cuerpo en clase
    AIGoalX(ISimable *);
    ~AIGoalX() override {}                  // cuerpo en clase

y salen `Construct__X` y luego `_._X`; **el objetivo las emite al reves** (`_._X` primero) en
los 2.184 B de `0x80044F60`..`0x800457E8`. Basta **subir el destructor por encima de
`Construct`** en las trece. NO lo he aplicado: la region B es una permutacion y un bloque
solo pasa a `d=0` cuando el PREFIJO entero esta bien, asi que hoy paga **cero** — y la regla
8 dice que lo que no paga se revierte. Queda escrito para el dia que se ataque el prefijo.

El mapa de la region B, con dueño:

| tramo | B | d | de quien es |
|---|---:|---:|---|
| `_IHandle__10IVehicleAI` .. `_IHandle__10ITrafficAI` | 1.648 | +408..+2.760 | **`Interfaces/Simables/IAI.h`**: el orden de sus ocho clases |
| `Call<Hermes::MemberHandler<...AICopManager>>` (x57) | 3.188 | +1.212 | **mio** (`AICopManager.hpp`) |
| `GetNumCops__C9AIPursuit` y vecinas (x68) | 1.060 | +1.180 | **mio** (`AIPursuit.h`) |
| bloque `AIGoalX` (`_._`/`Construct`) | 2.184 | +336/+508 | **mio** (`AIGoal.cpp`, ver arriba) |
| accesores de `AIVehicle` (x24+x11+x9) | ~880 | -324..-532 | **mio** (`AIVehicle.h`) |
| `_IHandle__6ICause` + `_._6ICause` | 280 | -4.440 | `ISimable.h`/`IModel.h` |
| `__as`/`ClassKey` de `trafficpattern` | 60 | -2.576 | `AITrafficManager.hpp` (mio) + generada |
| `GetBits`/`GetBitsSigned<WRoadLane>` | 52 | +9.436 | `World/WRoadNetwork.h` |
| `_IHandle__10IRBVehicle` | 12 | +10.220 | `Interfaces/Simables/IRBVehicle.h` |
| el resto, ~60 tramos | ~7.000 | | repartido |

Y `.rodata -456` / `.data -384`: la de `.rodata` **NO se puede comparar con el -240 de la
base**, porque dejo **20 entradas `$LC` de `zAI` desfasadas** en `keep.lst` (§6). La de
`.data` sigue siendo hueco anonimo de alineacion, como midio la r52 (`nfsmw-huecos-leer-el-dol`):
`pad_06_80415180_data`, `gap_06_80415230_data` y compañia son nombres del troceador, no dato
que escribir. **El encargo de esta ronda repetia lo contrario y sigue REFUTADO.**

Dato suelto para quien mire `.rodata`: **emitimos `_vt.14AIGoalPullOver` (48 B) y el objetivo
no** — es la unica vtable de diferencia (172 contra 171). Su `Construct` devuelve
`new AIGoalRam`, asi que en el objetivo la clase nunca se construye y el enlazador se lleva
ctor, dtor y vtable; en el nuestro la vtable sobrevive al estripado.

---

## 6. `lcfix`: **20 correcciones pendientes de `zAI`**

`python scripts/lcfix.py --check` da hoy **109** correcciones: **20 de `zAI.o`** (mias, del
movimiento de literales que provoca el bloque de cabeza) y **89 de `zSpeech.o`** (que no son
mias: `zSpeech` esta viva bajo otro agente — a mitad de ronda eran 70). **No lo he corrido**,
como manda el encargo.

Mientras `keep.lst` este rancio, `linkdelta` miente sobre `.rodata` de `zAI`: da `-456` donde
la base daba `-240`, y la diferencia (~200 B) es del orden de lo que costaron las 30 entradas
desfasadas de la r56 (320 B). **La cifra de `.rodata` de este informe no vale hasta que la
ventana pase `lcfix`.**

`prefijochk` sale LIMPIO.

---

## 7. Herramientas

Dos que merecen `scripts/` y siguen en el scratchpad de la sesion:

    hdrprobe.py <cabecera> [<previa>...]
        Que plantillas PIDE una cabecera, y en que orden. Compila una TU minima y lista
        .text. Es lo que convierte el bloque de cabeza de adivinanza en receta: sin esto no
        se sabe si una cabecera hay que ponerla arriba (muda) o en su renglon (pide).

    zai_c2.py [--seq A B] [--rebase]
        El censo de desplazamiento ENLAZADO por simbolo que la r55 y la r56 pidieron y
        nadie ha escrito en scripts/. Ahora con el ENLACE BASE cacheado: 8 s por medida en
        vez de 15. `dolwhere` sigue sin arrancar en zAI (`LAS SECCIONES NO COINCIDEN`), y
        `permorden` cuenta SIMBOLOS: subio de 868 a 894 mientras los bytes subian 10.180.

**Trampa de medida nueva**: comparar el orden de emision con los `.o` SIN ENLAZAR no vale.
Nuestro `zAI.o` tiene 1142 simbolos de `.text` y el objetivo 1030; los 112 de mas los estripa
el enlazador, asi que el `.o` da 59,99 % «en sitio» donde el enlace da 94,01 %. La medida
buena es siempre sobre el ELF enlazado.

---

## 8. Siguiente paso

1. **La region B, 16.352 B** — es lo unico que le queda al `.text` de `zAI`. Se ataca por el
   PREFIJO (`0x80041A80`: `VU0_v3unit`, `~IUnknown`, ctor de `bVector3`, `IAttachable`,
   `ISimable`, ...), y hasta que el prefijo no este, ningun bloque de detras paga.
2. **`IAI.h`: el orden de sus ocho clases** (~1,6 kB en `zAI`) y **`ISimable.h`/`IModel.h`:
   `ICause.h` fuera** (280 B en la cabeza). Son de `Interfaces/`; con el patron de guarda
   inerte de esta ronda se pueden hacer sin tocar a nadie mas, y la prueba por `-E` esta
   escrita.
3. **`AIGoal.cpp`: destructor por encima de `Construct` en las trece clases** (2.184 B), el dia
   que 1 y 2 esten.
4. **Ventana**: `lcfix.py` sobre las 20 entradas de `zAI`, y volver a leer `.rodata`.
