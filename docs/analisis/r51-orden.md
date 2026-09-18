# r51 · `orden`: el orden de emisión es ORDEN DE PARSEO, y se arregla en el fuente

`zLua` pasa de **68 funciones desplazadas a 1** y `zMain` de **30 a 19**, con
`fncmp` en **0 de 537** y **0 de 1.380** antes y después, `lcfix --check` limpio
en las dos y las secciones del objeto de `zMain` **idénticas byte a byte en
tamaño**. Ninguna de las dos promociona: lo que les queda es el déficit de
`.rodata`, y **`linkdelta` no se mueve ni se puede mover** —lo explico en §6—.

| | antes | ahora |
|---|---:|---:|
| `zLua` `permorden` DESPLAZADAS | 68 de 537 | **1** |
| `zLua` ciclos | 14 (50, 36, 18, 14, 9, 7, 7, 5, 4, 4, 2, 2) | **1 (de 2)** |
| `zLua` `textorder` DESCOLOCADAS | 166 de 538 | **2** |
| `zLua` `textorder` SALTOS DE DELTA | 79 | **3** |
| `zLua` `fncmp` | 0 de 537 | 0 de 537 |
| `zLua` `linkdelta` | `+0 rodata-1648 data-320` | `+0` **`rodata-1656`** `data-320` |
| `zMain` `permorden` DESPLAZADAS | 30 de 1.380 | **19** |
| `zMain` ciclos | 10 (87, 34, 15, 6, 3, 2×5) | 9 (97, 28, 5, 4, 3, 2×4) |
| `zMain` `textorder` DESCOLOCADAS | 157 de 1.381 | **145** |
| `zMain` `textorder` SALTOS DE DELTA | 47 | **28** |
| `zMain` `vtord` posiciones que no casan | 133 de 189 | **79** |
| `zMain` `fncmp` | 0 de 1.380 | 0 de 1.380 |
| `zMain` secciones del objeto | text 175.128 · rodata 20.360 · data 3.304 · bss 47.816 | **las mismas** |
| `zMain` `linkdelta` | `+0 rodata-1720 data-32` | **igual** |

DOL de `trypromo` al cierre: `zMain` `84905067ace7`, `zLua` `d953cf26ee78`; las
dos siguen **DOL ROTO**.

---

## 1. La corrección de encuadre: el sentido de la cadena de vtables

`r50-orden.md` §1.3 dejó dicho que la cadena de vtables es «el orden INVERSO en
que se completaron las clases». **Es exactamente así, y ahora está anclado**, no
deducido del fuente de GCC sino medido contra un caso con orden conocido:

`zMain.cpp` incluye los `E*.cpp` en orden alfabético. En el objeto del objetivo
`vtord` lista `EWorldMapOn` en el #4 y `E911Call` en el #172 — **alfabético
invertido**. Y con las posiciones exactas del preprocesado de nuestro árbol
(`ngccc -E`), en el estado de partida:

| clase | línea del `.i` (orden de PARSEO) | posición en `vtord` |
|---|---:|---:|
| `EAccelerate` | 26.811 | #171 |
| `EventSequencer::IEngine` | 31.213 | #170 |
| `EventSequencer::Engine` | 31.325 | #169 |
| `InputDevice` | 32.169 | #168 |
| `EAddSMS` | 37.178 | #167 |

Las cinco casan una a una con el inverso. **La cadena `.data` ascendente es el
inverso del orden de terminación de clase (`finish_struct_1`), y el orden de
terminación de clase es el orden de PARSEO del preprocesado.** Con eso, `vtord`
deja de ser un diagnóstico y pasa a ser una ecuación: *«esta vtable está N
posiciones abajo» = «esta cabecera se parsea N clases demasiado pronto»*.

Y el corolario que ha hecho todo el trabajo de esta ronda:

> **Para el `.text` de las plantillas y de las inline, el orden es el ORDEN DE
> PARSEO del fuente.** Para las funciones normales, el orden de DEFINICIÓN. Las
> dos se arreglan moviendo texto, no tocando codegen.

La herramienta que lo mide sin enlazar nada es `textorder.py --lista`: la
secuencia de `.text` de los dos objetos en paralelo. Es lo que ha decidido cada
paso de esta ronda.

---

## 2. `zLua`: 68 → 1. Era orden de DEFINICIÓN, no plantillas

`zLua` no tenía ningún problema de plantillas. Sus 68 desplazadas eran, en su
inmensa mayoría, **funciones normales definidas en otro orden que el original**.
`textorder --lista` lo enseñaba de un vistazo:

```
  OBJETIVO                          | NUESTRO
  Alloc__10LuaRuntimeUi             ! FreeResetSnapshot__10LuaRuntime
  Free__10LuaRuntimePvUi            ! BeginDelivery__10LuaRuntime
  TakeResetSnapshot__10LuaRuntime   ! EndDelivery__10LuaRuntime
  ...
```

y la lista de definiciones de `LuaRuntime.cpp` coincidía **exactamente** con
nuestra columna. El arreglo es reordenar los bloques del `.cpp`.

| fichero | qué se movió | ciclo que cierra |
|---|---|---|
| `LuaRuntime.cpp` | `Alloc/Free/TakeResetSnapshot` suben delante de `FreeResetSnapshot`; `FreeEmergencyAllocations` sube detrás de `ResetHeap`; `DumpStack/HandleGlobalIndex` bajan al final | el de 7 |
| `LuaPostOffice.cpp` | `RouteMessage` sube delante de `RegisterHandler`; `LuaMessageDeliveryInfo::BuildMessageTable` baja al final | 2 |
| `LuaBindery.cpp` | `Table_Randomize` baja hasta justo antes de `BindToGameCode` | el de 9 |
| `LuaAttributes.cpp` | `BindAccessors` baja al final | 4 |
| `LuaGameHooks.cpp` | **las 128 funciones de hooks reordenadas a la secuencia del objetivo** | los de 50, 36, 18 y 14 |

Las 128 de `LuaGameHooks.cpp` se hicieron en dos pasos medidos por separado:

1. **Izar las 12 declaraciones sueltas** (`static inline LockVehicleCache`,
   `IsSinglePlayer`, `SetHasBeenWonRaw`, `struct PerfMaximizer`,
   `DECLARE_CONTAINER_TYPE(ShuffleVector)`, los alias `asm()`,
   `gGPSDestination`…) al prólogo, para que ninguna función quede delante de
   algo que necesita. **Medido: neutro** (58 → 58 desplazadas, `fncmp` 0). Esto
   es el control que exige la regla 12 del brief: el ensayo compiló distinto
   —cambió el fichero entero— y no movió la métrica, así que el paso siguiente
   es atribuible.
2. **Permutar los 128 bloques de función al orden de `.text` del objetivo.**
   58 → **6**.

Después quedaban 6, todas en la cola de `finish_file`, y ésas sí son orden de
parseo:

| cambio | desplazadas |
|---|---:|
| reordenar los 8 `#include "Generated/Messages/M*.h"` de `LuaGameHooks.cpp` a `MNISComplete, MNotifySpeedTrap, MNotifyRacePlacement, MNotifyKnockedOut, MNotifyChallengePassed, MNotifyFinished` (los teníamos alfabéticos) | 6 → 3 |
| subir `Simables/IEngineDamage.h` delante del bloque de mensajes | 3 → 2 |
| `SimEntities/IOnlinePlayer.h` en `LuaBindery.cpp` (el objetivo lo parsea antes que `VoidBinder`) | 2 → **1** |

**Lo que queda (1 desplazada)**: el objetivo emite
`__…map<Ui,LuaAttribAccessors,_type_map>` (de `LuaAttributes.cpp`) **antes** que
`Thunk__10VoidBinder` (de `LuaBindery.cpp`), y `LuaBindery.cpp` se parsea antes
que `LuaAttributes.cpp` en `zLua.cpp`. O sea que ahí el orden **no** es el de
parseo: son dos entradas de `saved_inlines` con `mark_used` en vueltas
distintas del bucle de `finish_file`. No lo he perseguido: son 2 símbolos de
48 y 116 B contra un déficit de `.rodata` de 1.656 B.

---

## 3. `zMain`: la propuesta §7.3 de la r50, aplicada

r50 dejó escrito: *«Las dos vtables de `EventSequencer::Engine`/`IEngine` salen
fuera de sitio y corren todo el bloque de eventos `E*` una posición. Eso se
arregla moviendo dónde se parsea `EventSequencer.h`. No lo he tocado»*. Eso es lo
que se ha hecho, y vale **11 de las 30**.

### 3.1 El diagnóstico exacto, con el preprocesado

`EventSequencer.h` llegaba a `zMain` por
`EAccelerate.cpp → IRigidBody.h → ISimable.h → IModel.h → EventSequencer.h`, es
decir en la **segunda** unidad de la lista. El objetivo la parsea en dos sitios
distintos y muy lejos de ahí:

| clase | posición `vtord` del objetivo | lo que significa (§1) |
|---|---:|---|
| `EventSequencer::Engine` | #3, entre `SteeringWheelDevice` y `EWorldMapOn` | se completa en **`EventSequencer.cpp`** (línea 536 de `zMain.cpp`) |
| `EventSequencer::IEngine` | #152, entre `EChangeState` y `ECellCall` | se completa **durante `ECellCall.cpp`** |

Y `IModel.h`/`ISimable.h` **sólo usan `EventSequencer::IEngine *`**: un puntero.
No necesitan la cabecera.

### 3.2 El arreglo, en cuatro piezas

1. **`IModel.h` y `ISimable.h`**: el `#include` pasa a
   `#ifdef EVENTSEQ_FWD_IENGINE` → `namespace EventSequencer { class IEngine; }`,
   con la rama `#else` idéntica a lo que había. Sólo `zMain.cpp` enciende la
   guarda.
2. **`EventDynamicData.h`**: era un fichero de 12 líneas cuyo único contenido era
   `#include "EventSequencer.h"`, así que en cuanto se cortó (1) volvía a
   arrastrarla desde `EAccelerate.cpp`. Ahora **`struct EventDynamicData` y
   `gEventDynamicData` viven en `EventDynamicData.h`** (que es su sitio) y
   `EventSequencer.h` la incluye en el punto exacto donde estaba la definición;
   el `#include` de vuelta queda tras
   `#if !defined(EVENTSEQ_FWD_IENGINE) && !defined(MAIN_EVENTSEQUENCER_H)`.
   **El `!defined(MAIN_EVENTSEQUENCER_H)` corta la recursión**: sin él, la
   sección re-entrante del punto 3 se expande a mitad de `EventSequencer.h`,
   antes de declarar `IEngine`, y falla el parseo de cualquier unidad que **no**
   encienda la guarda. Es el único error que dio esta ronda y tardó una
   compilación en salir.
3. **`EventSequencer.h` se parte**: `class Engine` sale de la guarda de cabecera
   a una **sección RE-ENTRANTE**:

   ```cpp
   #endif /* MAIN_EVENTSEQUENCER_H */

   #if !defined(EVENTSEQ_DEFER_ENGINE) || defined(EVENTSEQ_WANT_ENGINE)
   #ifndef MAIN_EVENTSEQUENCER_ENGINE
   #define MAIN_EVENTSEQUENCER_ENGINE
   namespace EventSequencer { class Engine : … };
   #endif
   #endif
   ```

   Sin `EVENTSEQ_DEFER_ENGINE` se expande en el primer `#include` y la cabecera
   es la de siempre. `zMain.cpp` define `EVENTSEQ_DEFER_ENGINE`;
   `Main/Common/EventSequencer.cpp` define `EVENTSEQ_WANT_ENGINE` antes de
   incluirla y ahí es donde se completa la clase — el #3 del objetivo.
   `EA_PRAGMA_ONCE_SUPPORTED` **no está definido en ningún sitio del árbol**
   (comprobado), así que el `#pragma once` de la cabecera es inerte y la
   reentrada funciona. `EventSequencer::Engine` no tiene ningún otro usuario en
   el árbol.
4. **`ECellCall.cpp`** recibe un `#include "EventSequencer.h"` explícito, que es
   donde el objetivo completa `IEngine`; y los cuatro `E*.cpp` que de verdad
   usan `EventSequencer::IEngine` (`EProcessAreaStimulus`, `EProcessStimulus`,
   `EResetSequencer`, `EResetSystem`) lo incluyen también, porque ya no les
   llega por `EventDynamicData.h`.

Resultado: **30 → 27** desplazadas y `vtord` de **133 a 92**. `Engine` e
`IEngine` casan con el objetivo y con ellas los 165 `E*` que arrastraban.

### 3.3 Los cuatro pares de eventos invertidos

`vtord` dejaba cuatro parejas con el mismo síntoma: la clase que el objetivo
completa PRIMERO es la que nosotros completamos SEGUNDA, y siempre porque el
`.cpp` incluye la cabecera de su pareja **después** de la suya propia:

| fichero | qué se subió delante de su propio `.hpp` |
|---|---|
| `EQuitDemo.cpp` | `EQuitToFE.hpp` |
| `EPlayEndNIS.cpp` | `EPlayRaceNIS.hpp` |
| `EProcessAreaStimulus.cpp` | `EProcessStimulus.hpp` |
| `EScheduleEvent.cpp` | `Main/Scheduler.h` y `EScheduleEventUpdate.hpp` |

`vtord` **92 → 79**. `permorden` **no se mueve** (27): las virtuales de los `E*`
son todas out-of-line en su `.cpp`, así que mover su vtable no mueve `.text`.
**Es un caso limpio de métrica que no vale para lo que se está midiendo**: para
el `.text` hay que mirar `permorden`; `vtord` sólo manda cuando la clase tiene
cuerpos inline que `mark_vtable_entries` instancia.

### 3.4 Las tres cabeceras de mensaje: 27 → 19

El bloque de `BuildMessageTable__M*` y el de
`_GetKind__M*`/`HandleMessage_LuaBinding__M*` salen en dos tramos: primero los
mensajes cuya `M*.h` incluye algún `E*.cpp` (en orden alfabético de ese `.cpp`) y
después el bloque alfabético de `EventDefs.cpp`. Comparando los dos objetos, la
secuencia era **idéntica salvo tres mensajes** que el objetivo tiene en el tramo
temprano y nosotros en el alfabético:

| mensaje | dónde lo completa el objetivo |
|---|---|
| `MEnterRaceOverFlow` | entre `EEngineBlown.cpp` y `EJointDetached.cpp` |
| `MEnterFreeRoam`, `MFlowReadyForOutro` | entre `ESetCopAutoSpawnMode.cpp` y `ETerminateMusic.cpp` |

Dentro de cada rango **cualquier fichero da el mismo resultado**, porque lo único
que decide es el orden relativo de terminación de clase. He puesto
`MEnterRaceOverFlow.h` en `EHideRaceOverMessage.cpp` y `MEnterFreeRoam.h` +
`MFlowReadyForOutro.h` en `EShowRaceOverMessage.cpp`, que son los dos de cada
rango semánticamente más cercanos. **No sé cuál era el fichero original** y lo
digo: `debug_lines.txt` da el fichero de cada instrucción pero no la cadena de
`#include`, así que aquí la evidencia es el rango, no el fichero.

`permorden` **27 → 19**, y los bloques `M*` de los dos objetos quedan en la misma
secuencia.

### 3.5 Lo que le queda a `zMain` en `.text` (19)

Los ciclos que quedan (97 y 28) ya **no** son de mensajes: son el entrelazado de
los `_IHandle__X` de interfaz (`IPlayer`, `IStateManager`, `IResetable`,
`INISCarEngine`, `IBoundable`, `IDamageableVehicle`, `ISceneryModel`,
`IFeedback`, `IDynamicsEntity`), los cuatro miembros de
`WorldConn::Pkt_Body_Send` y los dos `TypeName__*Params`. Ésos **no** siguen el
orden de la cadena de vtables —`Pkt_Body_Send` está en el #104 nuestro contra el
#168 del objetivo y sin embargo sus cuerpos salen 13 posiciones más TARDE en
`.text`, no más pronto—, así que su sitio lo decide `mark_used` desde código
normal, no el recorrido de vtables. Es el siguiente frente y no está resuelto.

En `vtord` quedan tres cosas señaladas y sin arreglar, todas con el diagnóstico
hecho:

- `WorldConn::Pkt_Body_Send`: nuestro #104 (lo parsea `EMissShift.cpp` vía
  `VehicleRenderConn.h`), el objetivo #168 → el original lo parsea entre
  `class EAddSMS` y `class EAIEngineRev`.
- `InputDevice`: nuestro #170, objetivo #171 → el original lo parsea entre
  `class E911Call` y `class EAccelerate`; a nosotros nos llega por
  `ISimable.h → IModel.h → Bounds.h → SimSurface.h → simsurface.h →
  MWAttribUserTypes.h → InputDefParser.h → InputDevice.h`, o sea después de
  `EAccelerate.hpp`.
- La cola de `Instanceable`/`Listable`/`FixedVector`/`Vector` (#181-#186).

---

## 4. La inercia de las cabeceras compartidas, medida

Cuatro de los ficheros tocados son cabeceras que usa medio árbol
(`IModel.h`, `ISimable.h`, `EventSequencer.h`, `EventDynamicData.h`). **Inercia
comprobada, no supuesta**: compilando `zSim` y `zGameplay` con el árbol de antes
y con el de ahora —a un `.o` del scratchpad, sin tocar los objetos del árbol—,

```
=== zSim        .text IGUAL  .rodata IGUAL  .data IGUAL  .bss IGUAL  .ctors IGUAL
=== zGameplay   .text IGUAL  .rodata IGUAL  .data IGUAL  .bss IGUAL  .ctors IGUAL
```

y ninguna otra sección que no sea `.debug*` cambia (tampoco `.rela.*` ni
`.symtab`). Lo único que se mueve es la tabla de ficheros de DWARF, que el
enlace tira.

---

## 5. `lcfix --check`: limpio en las dos, y un aviso que NO es mío

`python scripts/lcfix.py --check` sale **limpio para `zLua` y `zMain`** después de
todos los cambios, que es lo que había que comprobar: he reordenado literales en
las dos unidades y ninguna entrada `@lc` de `keep.lst` ha quedado rancia. **No he
tocado `keep.lst`.**

Al cierre `lcfix --check` pide dos correcciones **en `zSim`**
(`zSim.o:$LC633 → $LC630`, `zSim.o:$LC634 → $LC631`). No es mío: `zSim.cpp` se
modificó a las 15:13:44 y su `.o` se recompiló a las 15:14:12, es decir mientras
yo escribía esto. Es exactamente el caso de `r50-orden.md` §8. **Que lo mire el
agente de `zSim`.**

---

## 6. Qué le queda a cada una para el DOL, medido con `linkdelta`

```
Speed/Indep/SourceLists/zMain     .text +0   rodata-1720 data-32
Speed/Indep/SourceLists/zLua      .text +0   rodata-1656 data-320
```

**`linkdelta` no se ha movido, y no podía moverse**: las dos unidades ya tenían
`.text +0` antes de empezar, y una permutación no cambia el tamaño de ninguna
sección. Lo digo con la cifra porque el brief lo pedía: *bajar la permutación no
acerca `linkdelta`; acerca el DOL de otra manera*, que es la posición de cada
símbolo dentro de la sección. Lo que sí demuestra que la unidad se acerca:

- `zMain`: las secciones del objeto miden **exactamente lo mismo** que antes
  (`text 175.128 · rodata 20.360 · data 3.304 · bss 47.816`), o sea cero coste; y
  el contenido de `.rodata` **sí** cambia, en la dirección buena. El literal
  `"EventSequencerSystems"` (el `HAND_POOL_TAG` de la inline de `Engine`) estaba
  en el offset **`0x10C`**, ahora está en **`0x2978`** y el objetivo lo tiene en
  **`0x2D84`**: la diferencia que queda, 1.036 B, es el déficit de `.rodata` de
  la unidad (880 B en el objeto). Antes estaba 11 kB fuera de sitio.
  **Esto es justo lo de [[nfsmw-censo-de-tamanos-no-basta]]: mismo tamaño,
  contenido distinto.**
- `zLua`: la secuencia de cadenas de `.rodata` pasa de coincidir en **332** a
  **339** de las 377 que emitimos, contra las 454 del objetivo.

Y el precio, que hay que decirlo: **`zLua` pierde 8 B de `.rodata`** (7.744 →
7.736; `linkdelta` de −1.648 a −1.656). Bisecado a la reordenación de funciones
de `LuaGameHooks.cpp`, y localizado: es **relleno de alineación** en la frontera
entre el pool de cadenas y el de floats (`…"MomentStrm\0"` + 5 ceros + `3e4ccccd`
antes; sin los ceros ahora). No es una cadena perdida —el censo de cadenas es el
mismo, 377— y se volverá a decidir en cuanto entren las 77 cadenas que nos
faltan. Lo dejo así porque la secuencia mejora; si el agente de `.rodata` prefiere
el estado anterior, el cambio que lo revierte está aislado en un solo fichero.

**Resumen honesto**: las dos unidades siguen bloqueadas por lo mismo que antes de
la ronda, `.rodata` (−1.720 y −1.656) y `.data` (−32 y −320), que es el paquete
del prefijo estándar de TU. El orden ya no es lo que las bloquea.

---

## 7. Ficheros tocados

| fichero | qué |
|---|---|
| `src/Speed/Indep/Src/Lua/LuaRuntime.cpp` | 25 bloques reordenados al orden del objetivo |
| `src/Speed/Indep/Src/Lua/LuaPostOffice.cpp` | `RouteMessage` / `BuildMessageTable` |
| `src/Speed/Indep/Src/Lua/LuaBindery.cpp` | `Table_Randomize` al final; `#include IOnlinePlayer.h` |
| `src/Speed/Indep/Src/Lua/LuaAttributes.cpp` | `BindAccessors` al final |
| `src/Speed/Indep/Src/Lua/LuaGameHooks.cpp` | 128 funciones reordenadas; 12 declaraciones izadas; orden de los 8 `M*.h`; `IEngineDamage.h` delante |
| `src/Speed/Indep/SourceLists/zMain.cpp` | `#define EVENTSEQ_FWD_IENGINE`, `#define EVENTSEQ_DEFER_ENGINE` |
| `src/Speed/Indep/Src/Interfaces/SimModels/IModel.h` | `EventSequencer.h` con guarda (rama `#else` idéntica) |
| `src/Speed/Indep/Src/Interfaces/Simables/ISimable.h` | igual |
| `src/Speed/Indep/Src/Main/EventDynamicData.h` | recibe `struct EventDynamicData` + corte de recursión |
| `src/Speed/Indep/Src/Main/EventSequencer.h` | cede la struct; `class Engine` a sección re-entrante |
| `src/Speed/Indep/Src/Main/Common/EventSequencer.cpp` | `#define EVENTSEQ_WANT_ENGINE` |
| `src/…/Generated/Events/ECellCall.cpp` | `#include EventSequencer.h` (donde el objetivo completa `IEngine`) |
| `…/EProcessAreaStimulus.cpp` `…/EProcessStimulus.cpp` `…/EResetSequencer.cpp` `…/EResetSystem.cpp` | `#include EventSequencer.h` explícito (usan `IEngine`) |
| `…/EQuitDemo.cpp` `…/EPlayEndNIS.cpp` `…/EProcessAreaStimulus.cpp` `…/EScheduleEvent.cpp` | la cabecera de la pareja, delante de la propia |
| `…/EHideRaceOverMessage.cpp` `…/EShowRaceOverMessage.cpp` | las tres `M*.h` del §3.4 |

Nada de `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`. Ninguna
herramienta nueva en `scripts/` (las sondas fueron al scratchpad y están
borradas, volcados incluidos).

---

## 8. Para la siguiente ronda

1. **La cola de `permorden` hay que rehacerla.** Con `zSim 0`, `zMain 19` y
   `zLua 1`, las siguientes son `zFe2 144`, `zAI 182` y `zGameplay 270`, y ahora
   se sabe que **la mayoría de esas desplazadas son orden de DEFINICIÓN**, que se
   arregla permutando bloques del `.cpp` y se automatiza: el procedimiento de §2
   (emparejar cada bloque de nivel superior con su símbolo en el objeto del
   objetivo y ordenar por él) es mecánico y tardó minutos en `zLua`. Si aguanta
   en `zFe2` merece una herramienta en `scripts/` con docstring.
2. **`vtord` no vale para el `.text` cuando la clase no tiene inline** (§3.3):
   bajó de 133 a 79 sin mover `permorden` ni una unidad. Úsese `permorden` para
   decidir y `vtord` para diagnosticar.
3. Lo de §3.5 (`Pkt_Body_Send`, `InputDevice`, la cola de `FixedVector`) está
   diagnosticado con el rango de parseo exacto y sin aplicar.
4. La guarda `UTL_IMPLEMENT_NO_COPY_INIT` de `r50-orden.md` §5 sigue sin medir en
   las seis unidades del censo (`zPhysics` 23 ctores, `zEAXSound` 5, …).
5. `EventSequencer.h` sigue teniendo dos usuarios de más que el objetivo:
   `Physics/Behaviors/RigidBody.cpp`, `Physics/Common/PhysicsObject.cpp`,
   `PVehicle.cpp`, `Sim/Common/SimModel.cpp`, `Simulation.cpp`,
   `World/Common/WTrigger.cpp` la incluyen directamente. En las unidades de esos
   ficheros la guarda `EVENTSEQ_FWD_IENGINE` no está encendida y no cambia nada,
   pero es la misma palanca si a alguna le sobra `Engine`.
