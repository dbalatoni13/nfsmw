# r58 · `zMain`: **6.757 -> 750 B**, y el primer de la r57 era la palanca EQUIVOCADA

Unidad: `Speed/Indep/SourceLists/zMain`. Ocho ediciones, `fncmp` **0 de 1.380 antes y
despues de cada una**, `permorden` **1.371 -> 1.379 de 1.380**, `linkdelta` **IGUAL en las
nueve secciones** (con `lcfix` aplicado). Ni un commit, ni un `git add`.

    dolwhere   6.757 -> 750 B      (-6.007, -89 %)
    permorden  9 desplazadas -> 1
    movidos    45 simbolos -> 3
    fncmp      0 / 1.380 en todo momento
    sha1 .o    e251eb920af42a38c7aaea3696181e7e64edb642  (TRES compilados identicos)

**AVISO QUE INVALIDA LA MEDIDA OFICIAL**: `keep.lst` esta rancio para zMain --**71
correcciones pendientes**, 0 en la base--. Con el `keep.lst` del arbol, `linkdelta` dice
`rodata-632` y `dolwhere` ni siquiera empareja las secciones. Las cifras de arriba estan
tomadas con una copia corregida en el scratchpad; **hay que correr `python scripts/lcfix.py
zMain` al cerrar**. Yo no lo he corrido: es entrada del enlace.

---

## 1. Lo que cambia el manual: **el `_IHandle`/`TypeName` se coloca por el CIERRE DE CLASE**

La r57 dejo escrito un negativo: el primer (funcion muerta que usa el simbolo) para
`_IHandle__10IResetable` no pagaba en tres posiciones, y la leccion fue «lo que cuesta es la
CABECERA que hay que incluir». **Es falso, y era al reves.**

`_IHandle__X` y `TypeName__X` son `static` con cuerpo EN CLASE. Su indice en la cola de
`finish_file` lo fija **donde se PARSEA su cabecera**, no donde esta el primer uso. La prueba
es de una compilacion:

    #include "Speed/Indep/Src/Interfaces/Simables/IResetable.h"   <- justo antes de WorldConn.h
    (sin ninguna funcion muerta)

    movidos:  _IHandle__10IResetable   +3.212 B  ->  -12 B

Lo que hundia el intento de la r57 no era la cabecera: era **el cebo**, que ademas de la
cabecera mete una funcion y desplaza a los vecinos. Con el `#include` a secas y en el sitio
exacto sale gratis (`linkdelta` IGUAL: el enlazador no ve nada nuevo).

**Y el sitio exacto NO SE BISECA: se LEE.** El `.o` extraido trae la cola entera en orden.
Basta listar los vecinos del simbolo en el objetivo y buscar en la fuente quien emite a esos
vecinos. Las seis colocaciones de esta ronda salieron **a la primera**, sin tanteo:

| simbolo | vecino del objetivo | edicion | antes -> despues |
|---|---|---|---|
| `TypeName__15SmackableParams` | detras de `_IHandle__13ISceneryModel` | `#include Smackable.h` + primer, junto al primer de la r57 | +3.412 -> 0 |
| `TypeName__15ExplosionParams` | entre `MNotifyMovieFinished` y `MMiscSound` | `#include Explosion.h` + primer tras `ERandomExplosion.cpp` | +1.828 -> 0 |
| `_IHandle__10IResetable` | delante de `_._Pkt_Body_Send` | `#include IResetable.h` antes de `WorldConn.h` | +3.212 -> 0 |
| `_IHandle__9IFeedback` | entre `MEnterRaceOverFlow` y `MSetTrafficSpeed` | guarda en `IPlayer.h` + `#include IFeedBack.h` tras `EEngineBlown.cpp` | -1.396 -> 0 |
| `_IHandle__7IPlayer`, `_IHandle__Q23Sim13IStateManager` | delante de `IResetable` | `#include IPlayer.h` + `Simulation.h` antes de `IResetable.h` | +732 -> 0 |
| `_IHandle__Q217CollisionGeometry10IBoundable` | detras de `MAudioReflection` | guarda en `IModel.h` + `#include Bounds.h` tras `EAudioWorldTest.cpp` | -744 -> 0 |

Las dos guardas de cabecera son el patron ya establecido en el arbol
(`IPLAYER_H_FWD_ISTEERINGWHEEL`, `EVENTSEQ_FWD_IENGINE`): el tipo se usa **solo por
puntero**, asi que la cabecera se cambia por una declaracion adelantada y se incluye a mano
donde el objetivo la parsea.

## 2. El destructor SINTETIZADO va delante de los declarados

`_._Q29WorldConn13Pkt_Body_Send` salia el ULTIMO de los cuatro simbolos de la clase; el
objetivo lo saca el PRIMERO. La causa ya estaba escrita en `WorldConn.h:170` para
`Pkt_Effect_Send` (c34ord3, zSim), y ahora esta medida en zMain: **los metodos implicitos
entran en la cola de inlines diferidas antes que los declarados**. Quitando
`~Pkt_Body_Send() override {}` bajo guarda, los cuatro caen en su sitio.

    dolwhere  2.690 -> 2.499 B      permorden  2 desplazadas -> 1

## 3. **CUATRO BYTES MUERTOS VALIAN 1.749 B** -- y la r55 leyo mal el par

`-strip-unused-data` se lleva `size & ~7` de un simbolo muerto: de los 19 B del literal
`"EAGL4::SymbolEntry"` (`eagl4supportdef.h:42`) sobreviven **4**. Ese resto corria el pool de
cadenas +4 B durante 1.606 B.

La r55 describio el sintoma como **un par atomico**: «el literal muerto (+4) **mas** el `0.0f`
de pool que falta en `803EFF74` (-4)». **No son dos cosas.** Esos cuatro ceros no son un
`0.0f`: son el **RELLENO DE ALINEACION** del doble `0x4330000080000000` --la constante magica
de conversion entero->flotante de GCC, que pide 8 B--. Con los 4 B de basura delante, el
`100.0f` caia en una direccion 8-alineada y el relleno desaparecia; sin ellos aparece solo.

    #ifdef EAGL4_SYMBOLENTRY_DEAD          <- solo zMain.cpp lo define
            ... EAGL4Malloc(..., (const char *)0);
    #else
            ... EAGL4Malloc(..., HAND_POOL_TAG("EAGL4::SymbolEntry"));
    #endif

    dolwhere  2.499 -> 750 B   (-1.749 de una linea)

La veda de la r55 (`HAND_POOL_TAG` a secas = `.rodata -512` y `fncmp` 0 -> 1) sigue en pie:
lo que paga es la guarda **de un solo sitio**, no el macro global.

## 4. Regresiones: CERO, y esta probado

Cuatro de las cinco cabeceras tocadas son compartidas. Las cuatro ediciones estan dentro de
`#ifdef`/`#ifndef` cuyo macro **solo define `zMain.cpp`** (`grep` sobre `src/`, `config/`,
`tools/` y `scripts/`: cuatro definiciones, todas entre las lineas 26 y 64 de `zMain.cpp`).

La prueba no es el `grep`, es la medida: **devolviendo SOLO `zMain.cpp` a la version del
commit, con las cuatro cabeceras editadas en su sitio, el arbol reproduce exactamente**

    linkdelta  .text +0   resto IGUAL
    dolwhere   6.757 B
    fncmp      0 de 1.380
    lcfix      0 correcciones de zMain

o sea el estado de `41c2bc13` al byte. Las cabeceras son inertes para toda TU que no defina
los macros; lo unico que cambia en otras unidades son los numeros de linea del `.debug`, que
no llegan a `.text`/`.rodata`/`.data`. **No he recompilado ninguna unidad ajena** (regla 4).

## 5. Lo que queda: 750 B en 54 rangos

| B | que es | palanca |
|---:|---|---|
| 260 | `GetGrowSize__CQ23UTLt6Vector2ZP11ActionQueuei16Ui` se instancia DESPUES de los dos destructores de ActionQueue; el objetivo la pone justo detras de la de `_KeyedNode`. **Es el UNICO simbolo de `.text` fuera de sitio** (rotacion de 3 en una ventana de 264 B). | orden de instanciacion en `finish_file`; el disparador es la posicion de `ActionQueue.h` (ajustada en la r53, cuidado) |
| 126 | `_vt.11InputDevice` y `_vt.11EAccelerate` intercambiadas | orden de completado de clase |
| 117 | las cuatro `_vt` de `FixedVector`/`Vector` de `ActionQueue*` y `_KeyedNode`: el objetivo completa `Vector<KN>, Vector<AQ>, FixedVector<AQ>, FixedVector<KN>`; nosotros `Vector<KN>, FixedVector<KN>, Vector<AQ>, FixedVector<AQ>` | la misma familia que los 260 B: atacarlas juntas |
| 116 | `803F22E0`: el objetivo pone `"EventBuffer"` ANTES de `"EventSequencerSystems"` y tiene 8 B de pool (un doble) que no tenemos | R4 de la r55, sin tocar |
| 44 | `_vt.12EEngineBlown`<->`_vt.20EShowRaceOverMessage` y `_vt.9ECellCall`<->`_vt.13ESndGameState` | pares de vtable |
| 14 | `803F0608`: orden del pool de flotantes (`1.0/0.5/0.25/0.75` contra `0.25/1.0/0.75/0.5`) | |
| ~73 | sombra de reubicacion de todo lo anterior | cae sola |

Las tres primeras filas son **la misma familia** (`Vector`/`FixedVector` de `ActionQueue*` y
`_KeyedNode`, mas `InputDevice`) y suman 503 B de los 750: es por donde entra la proxima
ronda, con `vtord` y `permorden` en el bucle.

## 6. Ficheros

    src/Speed/Indep/SourceLists/zMain.cpp                     (mio)
    src/Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h      guarda IPLAYER_H_FWD_IFEEDBACK
    src/Speed/Indep/Src/Interfaces/SimModels/IModel.h         guarda IMODEL_FWD_BOUNDS
    src/Speed/Indep/Src/World/WorldConn.h                     guarda WORLDCONN_H_IMPLICIT_PKT_BODY_DTOR
    src/Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h           guarda EAGL4_SYMBOLENTRY_DEAD

**Al cerrar: `python scripts/lcfix.py zMain` -- 71 lineas.** Sin eso el DOL rompe en silencio
y `linkdelta` miente (`rodata-632`).
