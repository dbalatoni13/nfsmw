# r59 · `zMain`: **750 B -> 0. DOL OK.** La primera promocion desde la r55

Unidad: `Speed/Indep/SourceLists/zMain`. Ocho ediciones en siete ficheros, `fncmp`
**0 de 1.380 antes, despues y tras cada fichero**, `linkdelta` **IGUAL en las nueve
secciones en todo momento**. Ni un commit, ni un `git add`, ni una linea de
`config/GOWE69/*`.

    dolwhere   750 B  ->  DOL OK            (0 B, -100 %)
    permorden  1.379 / 1.380  ->  1.380 / 1.380   (cero desplazadas, cero ciclos)
    vtord      6 de 190 fuera de sitio -> 2, y las 2 son simbolos que el objetivo
               NO define (los estripa el enlace: _vt.22LuaMessageDeliveryInfo y
               _vt.Q33UTL3COM8IUnknown)
    fncmp      0 / 1.380 en todo momento
    linkdelta  .text +0, resto IGUAL
    trypromo   Speed/Indep/SourceLists/zMain    DOL OK

    sha1 .o    antes    e251eb920af42a38c7aaea3696181e7e64edb642
               despues  1bae95dcbd45dc30565ed3b37020a33760fe53fd
                        (TRES compilados seguidos, el mismo sha1 los tres,
                         y `trypromo` corrido DESPUES de los tres)

**`lcfix.py --check`: 242 correcciones pendientes, CERO de `zMain`.** Ninguna de las
242 nombra un simbolo de esta unidad, asi que **ninguna puede romperla**. Estan
repartidas en zSpeech 62, zAI 50, zWorld 39, zPhysicsBehaviors 30, zCamera 21,
zWorld2 19, zPlatform 13, zAnim 5 y zGameplay 3, y hay 5 `FALLO` (cadenas sin `$LC`
propio en zLua y zAnim). Aviso general, no de zMain: aplicarlas cambia el enlace de
TODOS y hay que reverificar el DOL despues.

---

## Lo que cambia el manual

### 1. NUEVO — **la semilla de pool: `if (0)` mueve el pool de constantes por CERO bytes**

Es el hallazgo de la ronda y vale para cualquier unidad.

El orden del pool de flotantes de una funcion lo fija `force_const_mem`, o sea el
orden de **EXPANSION**; el reparto de registros lo fija el orden de **DEF** que
`loop` iza al preencabezado. Se daba por hecho que eran el mismo orden --lo son
cuando las constantes nacen en declaraciones-- y por eso la R28 dejo `EPlayRaceNIS`
con la funcion al 100 % y el pool permutado: el objetivo quiere el pool
`1.0/0.5/0.25/0.75` y el reparto `f26=0.25, f27=1.0, f28=0.75, f29=0.5`, que exige
el orden de def `0.25/1.0/0.75/0.5`. **Los dos ordenes estan DESACOPLADOS en el
original**, y desde las declaraciones no se puede: reordenarlas arregla el pool y
rompe la funcion (medido: `fncmp` 0 -> 1, 3 insn de 2.176 B).

La palanca es meter las entradas que faltan **antes** con codigo que se expande y
se muere:

    float distTresh = 80.0f;
    if (0) { distTresh = 1.0f; distTresh = 0.5f; }     // <- una linea

`expand_expr` llama a `force_const_mem` para `1.0` y `0.5` y las mete en el pool en
ese orden; `jump.c` se lleva el bloque entero **antes** de `loop`, asi que no queda
ni un `set` que izar ni una instruccion que contar; y `mark_constant_pool` las marca
igual porque el bucle de mas abajo las sigue usando. `fncmp` sigue en 0 de 1.380.

**Valia los ultimos 34 B del DOL** (14 de `$LC633` y 20 de sombras de `sda21` en el
ctor de `EPlayRaceNIS`). Sin esto no hay promocion.

### 2. NUEVO — **`sizeof()` coloca la vtable de una plantilla, y cuesta cero**

El orden de vtables es el inverso del de **completado de clase**, y una clase
plantilla se completa en su primer USO. `sizeof(UTL::FixedVector<ActionQueue *, 20, 16>)`
en un `enum` **completa la clase y su base y no emite ni un byte** --no hay vtable
nueva: 190 en el objeto antes y despues--.

El objetivo completa `Vector<KN>, Vector<AQ>, FixedVector<AQ>, FixedVector<KN>` y
nosotros `Vector<KN>, FixedVector<KN>, Vector<AQ>, FixedVector<AQ>`. La causa es
que `Vector<_KeyedNode,16>` se completa en `class ICause` (su
`Instanceable<HCAUSE,ICause,10>` instancia `FixedVector<KN,10,16>`, cuya vtable
nadie necesita y por eso no se ve) y `FixedVector<KN,434,16>` en `class IModel`,
39 lineas de preproceso mas abajo. **Entre las dos hay hueco**, y ahi va el primer:

    src/Speed/Indep/Src/Interfaces/SimModels/IModel.h, justo tras #include ICause.h
    #ifdef IMODEL_ZMAIN_AQVECTOR_PRIMER
    class ActionQueue;
    enum { _zmain_r59_aqvector_primer = sizeof(UTL::FixedVector<ActionQueue *, 20, 16>) };
    #endif

Las cuatro vtables caen en su sitio **a la primera**: `dolwhere` 750 -> 632 B.

### 3. NUEVO — **una llamada CUALIFICADA a la base instancia UNA sola virtual**

El andamio de la r54 (`p->~List()`, linea 278 de `zMain.cpp`) instancia `~List` y
con el toda la cadena de bases (`~_Storage` -> `~FixedVector<AQ*,20>` ->
`~Vector<AQ*>`), asi que `~Vector<AQ*>` se queda con un indice de `saved_inlines`
**menor** que `Vector<AQ*>::GetGrowSize`, que no se instancia hasta el ctor de
`ActionQueue`. El objetivo los tiene al reves. Era **la unica funcion de `.text`
fuera de sitio de las 1.380**, y 260 B.

`GetGrowSize` es `protected`, asi que no se puede marcar usada desde fuera. La
forma que funciona es una clase derivada con una llamada **cualificada**, que es
no-virtual y marca usada exactamente esa funcion:

    struct _zmain_r59_prime_grow : public UTL::Vector<ActionQueue *, 16> {
        unsigned int Go() { return this->UTL::Vector<ActionQueue *, 16>::GetGrowSize(1); }
    };

y va **obligatoriamente delante** del andamio de la r54. La clase sigue siendo
abstracta, asi que no sale vtable. `permorden` cierra: **1.380 de 1.380**.

**NO vale hacerlo con `push_back`**: su cuerpo llama tambien a `reserve` y eso
adelanta `OnGrowRequest<Vector<AQ*>>` **94 posiciones** (medido).

### 4. El bloque `asm()` de pool tambien tiene POSICION, y se biseca en dos pasos

Los 40 B muertos de `EventSequencer.cpp` estaban escritos **detras** de las
cabeceras y salian detras de `"EventSequencerSystems"`; el objetivo los tiene
**delante** (803F22E0, justo tras `$LC1040`) y con **8 ceros de cabeza, no 12**.
Moverlo delante del `#include EventSequencer.h` y quitarle un `.4byte` de ceros
vale **137 B de una edicion** (180 -> 43 B): arregla `$LC1041`, `$LC1045` y **todas**
las sombras de 1 B de `EventSequencer::*` y `Scheduler`.

Para el bloque nuevo de `Scheduler.cpp` la posicion se biseco en dos compilaciones:
al final del fichero cae **40 B tarde**, al principio **4 B pronto** (falta el
`0.0f` del ctor), **detras del ctor** es exacto.

### 5. Un CADAVER se mata cambiando el argumento, no la cadena

`BNEW` es `new (__FILE__, __LINE__)` y esa sobrecarga de `bWare.hpp` es inline y
**tira los dos argumentos**: el literal nace muerto, `-strip-unused-data` se lleva
`size & ~7` (40 de 46 B) y deja **6 B** (`"r.cpp\0"`) en 803F235C, justo donde el
objetivo tiene `"Scheduler"` **entera y viva** (10 B). Un cadaver no da mas de 7 B
--`len & 7`--, asi que la cadena del objetivo **no puede salir de un literal
muerto**. La receta:

  * `new ((const char *)0, 0)` bajo guarda -> el literal no se emite (**codigo
    identico**: el inline ya tiraba el argumento). Solo eso da `rodata-8`.
  * y los 10 B se escriben a mano en un `asm()` de ambito de fichero, que **no
    tiene simbolo** y el estripado no lo toca. `linkdelta` vuelve a IGUAL.

### 6. Confirmado en zMain: la regla del inverso, y **DOS grupos**

`parseord.py` da CONTROL FALLIDO aqui, pero la regla se comprueba a mano y sale
exacta. Las vtables salen en **dos bloques separados por ~1,9 kB de `.rodata`**
--las dos vueltas de `finish_file`--, y **dentro de cada bloque** el orden es el
inverso del de parseo. Control de la primera: preproceso `Event`(26088),
`E911Call`(26149), `EAccelerate`(26830), `InputDevice`(32183), `EAddSMS`(36546);
vtables 170 EAddSMS, 171 InputDevice, 172 EAccelerate, 173 E911Call, 174 Event.
**Exacto.** Con eso se colocan tres pares por `#include`:

| par | edicion | vale |
|---|---|---:|
| `InputDevice` / `EAccelerate` | `#include Input/InputDevice.h` entre E911Call.cpp y EAccelerate.cpp en `zMain.cpp` (sus tres cabeceras ya estan parseadas: solo adelanta `DeviceScalar` e `InputDevice`) | 132 B |
| `EEngineBlown` / `EShowRaceOverMessage` | subir `#include EShowRaceOverMessage.hpp` **por encima** de la cabecera propia en `EEngineBlown.cpp` | 30 B |
| `ECellCall` / `ESndGameState` | lo mismo en `ECellCall.cpp` | 26 B |

---

## La escalera, edicion a edicion

| # | edicion | fichero | dolwhere |
|---|---|---|---:|
| 0 | base | | 750 |
| 1 | primer `sizeof(FixedVector<AQ*,20,16>)` | `IModel.h` (+ `#define` en zMain.cpp) | 632 |
| 2 | `#include InputDevice.h` entre E911Call y EAccelerate | `zMain.cpp` | 500 |
| 3 | `_zmain_r59_prime_grow` delante del andamio de la r54 | `zMain.cpp` | 236 |
| 4 | orden de `#include` | `EEngineBlown.cpp` | 206 |
| 5 | orden de `#include` | `ECellCall.cpp` | 180 |
| 6 | bloque `asm()` delante de las cabeceras y 12 -> 8 ceros | `EventSequencer.cpp` | 43 |
| 7 | matar el cadaver de `BNEW` + `"Scheduler"` a mano tras el ctor | `Scheduler.cpp` | 34 |
| 8 | semilla de pool `if (0)` | `EPlayRaceNIS.cpp` | **DOL OK** |

`fncmp` medido **despues de cada una**: 0 de 1.380 en las ocho.

## Negativos medidos (escritos junto a la funcion, donde `previo.py` los encuentra)

| prueba | resultado |
|---|---|
| mover `IMPLEMENT_LISTABLE(ActionQueue)` al final de `actionqueue.cpp` | **cero cambio**: la posicion del estatico no instancia `~List`. Revertido. |
| quitar el andamio de la r54 (`p->~List()`) | `~List` se va **98 posiciones**; ciclo de 98 en `permorden`. Sigue haciendo falta. |
| primer con `push_back` en vez de la llamada cualificada | arregla `GetGrowSize` y adelanta `OnGrowRequest` 94 posiciones |
| declarar `dot, dotHalf, minTresh, dotScale` en el orden del POOL | arregla el pool y **rompe** la funcion: `fncmp` 0 -> 1, 3 insn (2.176 B) |
| `asm()` de `"Scheduler"` al final de `Scheduler.cpp` | cae 40 B tarde (303 B) |
| `asm()` de `"Scheduler"` en cabeza de `Scheduler.cpp` | cae 4 B pronto (48 B) |
| `new ((const char*)0,0)` sin el `asm()` de repuesto | `rodata-8`, 152.385 B |

## Regresiones: CERO, y esta PROBADO con el metodo del `.o` privado

De los siete ficheros tocados, **seis solo los incluye `zMain.cpp`** (comprobado con
`grep` sobre `src/`): `zMain.cpp`, `ECellCall.cpp`, `EEngineBlown.cpp`,
`EPlayRaceNIS.cpp`, `EventSequencer.cpp` y `Scheduler.cpp`.

El septimo, **`IModel.h`, es compartido**. Su edicion vive dentro de
`#ifdef IMODEL_ZMAIN_AQVECTOR_PRIMER`, y el macro se define en **un solo sitio**
(`zMain.cpp:30`; igual `SCHEDULER_ZMAIN_NO_FILE` en `zMain.cpp:66`). Pero el `grep`
no es la prueba: la prueba es compilar unidades AJENAS a un `.o` **privado** (nunca
a `build/GOWE69/src`, que es de su agente) con la cabecera **prístina** en un
directorio de sombra (`-I <sombra>` delante) y comparar contra el mismo compilado
con la cabecera editada.

    control: la misma sombra con un `#error` dentro FALLA la compilacion  -> la
             sombra si intercepta el include

| unidad | secciones | simbolos | contenido |
|---|---|---|---|
| `zTrack` | 22, ninguna distinta | 899 comunes, 0 con otro valor | `.text`/`.rodata`/`.data`/`.ctors` IGUAL |
| `zPhysics` | 23, ninguna distinta | 2.170 comunes, 0 con otro valor | IGUAL |
| `zAnim` | 22, ninguna distinta | 1.118 comunes, 0 con otro valor | IGUAL |

Los tres `.o` salen **identicos byte a byte**: la cabecera es inerte fuera de zMain.

## Ficheros

    src/Speed/Indep/SourceLists/zMain.cpp                        (mio)
    src/Speed/Indep/Src/Generated/Events/ECellCall.cpp           (solo zMain)
    src/Speed/Indep/Src/Generated/Events/EEngineBlown.cpp        (solo zMain)
    src/Speed/Indep/Src/Generated/Events/EPlayRaceNIS.cpp        (solo zMain)
    src/Speed/Indep/Src/Main/Common/EventSequencer.cpp           (solo zMain)
    src/Speed/Indep/Src/Main/Common/Scheduler.cpp                (solo zMain)
    src/Speed/Indep/Src/Interfaces/SimModels/IModel.h            COMPARTIDA,
        guarda IMODEL_ZMAIN_AQVECTOR_PRIMER, tres unidades ajenas medidas

## Lo siguiente

1. **Promocionar `zMain`** (`configure.py` + el flag Matching): `trypromo` da DOL OK
   con `keep.lst` tal cual esta en el arbol. `linked` 519 -> 520.
2. Las tres palancas nuevas se aplican tal cual a **las otras cuatro unidades con
   las nueve secciones a IGUAL** (`zAnim` 2.697 B, `zFeOverlay` 6.211, `zEcstasy`
   21.138, `zPhysics` 35.673): el primer de `sizeof` para vtables de plantilla, la
   llamada cualificada para colocar UNA virtual, y la semilla `if (0)` para el pool
   de flotantes. Las tres cuestan **cero bytes** y ninguna toca `keep.lst`.
3. Aviso de coordinacion: **el directorio de scratchpad esta COMPARTIDO entre los
   agentes de la tanda** --otro agente me sobrescribio una sonda a mitad de la
   ronda--. Trabajar en un subdirectorio propio.
