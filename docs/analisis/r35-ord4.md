# Ronda 35 — `ord4`: terminar el ORDEN del `.text` (zMain, zSim, la cola)

Encargo: la cola del frente de orden que dejó la r34. **Resultado: 342 de las
9.794 funciones descolocadas del barrido, colocadas — todas en dos unidades.**

| unidad | descolocadas antes | después | saltos antes | después |
|---|---|---|---|---|
| **`zMain`** (159.776 B) | **501** de 1381 | **163** | 194 | **51** |
| **`zAttribSys`** (48.776 B) | 47 de 197 | **43** | 18 | **15** |
| `zSim` (96.400 B) | 21 de 403 | 21 (una veda más) | 7 | 7 |
| barrido global | 9.794 fns / 31 unidades | **9.463 / 32** | | |

(El barrido baja 331 y no 342 porque aparece una fila nueva, `FSasync` con 11,
de otro agente: 9794 − 342 + 11 = 9463, exacto. **Todas las demás filas salen
con la cifra idéntica a la de esta mañana.**)

Sin perder un byte: `measure.py` de las 610 unidades da **3.866.216/3.946.204 B,
97,9730 %, 18.336 funciones**, la misma cifra con la que cerró la r34.

---

## 0. Las cuatro frases

1. **Los 130 cuerpos de `BuildMessageTable`/`HandleMessage_LuaBinding` viven en
   `Generated/Messages/M*.h` en el original, no en `EventDefs.cpp`.** Moverlos
   (bajo guarda) vale **103 funciones de golpe** y es la causa nº 1 de zMain.
   `debug_lines` lo dice y la aritmética de líneas lo confirma: el objetivo pone
   `BindMessagesToLua` en `EventDefs.cpp:1116`, justo detrás de `GetLuaBinding`
   (`:1099`); nuestro fichero mete 1.180 líneas de cuerpos entre las dos.
2. **La causa más rentable sigue siendo la cabecera que incluye de más**:
   `IPlayer.h` incluye `ISteeringWheel.h` y `SoundAI.h` incluye `MUnspawnCop.h`,
   los dos por PUNTERO/REFERENCIA. Dos declaraciones adelantadas: **127
   funciones**.
3. **Una posición de `#include` se puede DEDUCIR del hueco que deja el objetivo y
   la deducción se verifica en una compilación.** `MNotifyMovieFinished.h` sale
   en el objetivo entre `MGamePlayMoment.h` (de `EMomentStrm.cpp`) y
   `MMiscSound.h` (de `EReportInfraction.cpp`): el único fichero de zMain en ese
   hueco que puede traerlo es `EPlayRaceMovie.cpp`. Puesto ahí: **16 funciones**.
4. **La lista de `#include` del original va en orden alfabético SIN distinguir
   mayúsculas** (`MEnteringGameplay.h` antes que `MEnterPostRaceFlow.h`).
   Nuestro `EventDefs.cpp` iba en ASCII. Vale 2 saltos.

---

## 1. `zMain`: 501 → 163 (194 → 51 saltos)

Diez ensayos, todos medidos por separado con `c34ord3_cc.py` a un `.o` del
scratchpad (nunca al árbol) y `c34ord3_to.py` encima.

| ensayo | qué | descolocadas | saltos |
|---|---|---|---|
| base | | 501 | 194 |
| **z1** | los 130 cuerpos de `M*.h` a su cabecera, `inline`, fuera de clase, bajo guarda | **398** | 82 |
| **z2** | `inline` a los 24 métodos de `EventSequencer::Engine` en `EventSequencer.cpp` | **352** | 80 |
| **z3+z4+z5** | orden de `iomodule.cpp`; ctor/dtor de `GameDevice` detrás de `GetNumDeviceScalar`; `IsWheel`/`GetInterfaces`/`GetSecondaryDevice` al cuerpo de la clase, detrás de `Construct` | **308** | 66 |
| **z6** | `StopAllForces` antes que `ReadInput` en `SteeringWheelDevice.cpp` | **306** | 63 |
| **z7+z8** | `SoundAI.h` → `class MUnspawnCop;`; `IPlayer.h` → `class ISteeringWheel;` | **179** | 57 |
| **z9** | los `#include` de `EventDefs.cpp` en orden alfabético sin distinguir mayúsculas | 179 | **55** |
| **z10** | `MNotifyMovieFinished.h` en `EPlayRaceMovie.cpp` | **163** | **51** |

Con z1..z6 **el bloque de código normal entero (índices 0-1063) queda a delta 0**;
lo que sobrevive está todo dentro del bloque diferido.

### 1.1 z1, el mecanismo, y cómo se prueba sin compilar

`debug_lines` sitúa `HandleMessage_LuaBinding__12MAIEngineRev` en
`MAIEngineRev.h:69` y `BuildMessageTable__12MAIEngineRev` en `MAIEngineRev.h:75`
— **seis líneas de separación, y son las mismas seis en las 65 clases**, con
independencia de lo que mida `BuildMessageTable` (3 líneas en
`MAcceptEnterCareerEvent`, 25 en `MAIEngineRev`). Eso sólo cuadra si el orden en
la cabecera es `HandleMessage_LuaBinding` (4 líneas) y **detrás**
`BuildMessageTable`. Nuestro `EventDefs.cpp` los tiene al revés.

Y las dos salen en **pasadas distintas** de `finish_file` (objetivo: las 65
`HandleMessage` en 1077-1169, las 65 `BuildMessageTable` en 1268-1334) porque
`HandleMessage_LuaBinding` **toma la dirección de `BuildMessageTable`**: sólo
hace falta después de haber emitido la primera. Es el modelo de pasadas de la
r33/r34, confirmado otra vez.

**No hay riesgo de definición múltiple**: contados los 610 objetos EXTRAÍDOS, el
original emite estos símbolos en **`zMain.o` (130) y `zLua.o` (1), y en ningún
otro** — son `inline` y sólo nacen donde se usan. Aun así van bajo guarda
(`ZMAIN_MESSAGES_LUA_INLINE`), porque las cabeceras necesitan `LuaBindery.h` y
`LuaPostOffice.h` para compilar el cuerpo y no quiero meterlas en las decenas de
TUs que incluyen un `M*.h`.

### 1.2 z2 cuesta 852 B de `.text`… que SOBRABAN

Marcar `inline` los 24 métodos de `EventSequencer::Engine` hace desaparecer
**ocho funciones** (`__nw__`/`__dl__` de `Engine`, su constructor, `Context`,
`GetSystems`, `IsVerbose`). Comprobado: **ninguna de las ocho está en el
objetivo**, no falta ninguna función del objetivo, y **ningún objeto nuestro las
referencia** (barrido de indefinidos sobre los 610 `.o` de `build/GOWE69/src`).
`.text` pasa de 179.964 a 179.112 B, más cerca de los 159.776 del objetivo.

Que el original las tiene `inline` lo dice `debug_lines`: las 24 salen en el
bloque diferido (1236-1259) con `fichero:línea` en `EventSequencer.cpp:117-341`.
Con `inline` en su sitio actual quedan en **1240-1263: delta +4 constante**, o
sea colocadas; **no hizo falta moverlas** al principio del `.cpp`.

### 1.3 z7/z8: los dos `#include` de más, y su radio

* `IPlayer.h:51` — `virtual ISteeringWheel *GetSteeringDevice() = 0;` es el
  **único** uso de `ISteeringWheel` en la cabecera. El objetivo parsea
  `ISteeringWheel.h` en el índice 1260, pegado a `InputDeviceGC.cpp`; nosotros en
  el 1079.
* `SoundAI.h:206` — `void MessageUnspawnCop(const MUnspawnCop &message);` es el
  único uso. El objetivo parsea `MUnspawnCop.h` **el último** (1169), desde
  `EventDefs.cpp`; nosotros el 1074, arrastrado por `EBailPursuit.cpp`.

Las dos juntas: **306 → 179**.

---

## 2. `zAttribSys`: 47 → 43

**a1 — `Attribute::~Attribute()` está en el fichero equivocado.** Lo tenemos al
final de `AttribSupport.cpp` (línea 154); `debug_lines` lo pone en
`attribute.cpp:35`, **entre el constructor de tres argumentos (`:21`) y
`operator=` (`:41`)**. Movido ahí: **47 → 43, 18 → 15 saltos**. Los dos ficheros
son de `zAttribSys.cpp` y de nadie más: radio cero sin guarda.

Lo que queda son cuatro racimos pequeños, todos de orden de INSTANCIACIÓN, con el
diagnóstico ya hecho (§5.3).

---

## 3. `zSim`: sigue en 21, y una veda más

**s13 — REVERTIDO.** Hipótesis: como los cuatro miembros de `FixedVector` salen
en el objetivo en orden `Alloc, Free, GetGrowSize, GetMaxCapacity` —que es el
orden de DECLARACIÓN de los virtuales en `Vector`, no el de `FixedVector`—, la
ranura de vtable manda; y como `virtual ~Vector()` está declarado el primero
(`UTLVector.h:51`), su ranura 0 explicaría que los tres `~FixedVector` salgan
por delante. Sentencia barrida: **mover la declaración de `virtual ~Vector() {}`
al final de la clase `Vector`, detrás de `OnGrowRequest`** (bajo guarda
`UTLVECTOR_H_DTOR_LAST`, sólo `zSim.cpp`).

Resultado: **21 descolocadas, 7 saltos, `.text` 114.492 B — idéntico byte a
byte**. Ni siquiera cambia el tamaño, así que la posición de la declaración del
destructor **no mueve su ranura**. Revertido (`git diff` vacío en `UTLVector.h` y
`zSim.cpp`).

Con esto van **tres** sentencias barridas en este racimo (s11 y s12 de la r34,
s13 mía) y las tres dan exactamente 21/7. **El eje "posición de la declaración"
está agotado**; lo que queda es el orden de DEMANDA de la instanciación, no el de
declaración (§5.2).

Aviso útil: **zMain tiene el MISMO racimo en su cola** (`_._FixedVector` /
`_._Vector` de `ActionQueue` y `_KeyedNode`, últimos 7 saltos). Quien lo resuelva
cierra las dos unidades a la vez.

---

## 4. Lo que NO extrapola — medido, no supuesto

Las dos guardas de cabecera de z7/z8 son las candidatas obvias a barrer por todo
el árbol. **No sirven fuera de zMain.** Compiladas `zWorld` y `zGameplay` desde
una copia de su SourceList en el scratchpad con
`IPLAYER_H_FWD_ISTEERINGWHEEL` + `SOUNDAI_H_FWD_MUNSPAWNCOP` definidas:

| unidad | base | con las dos guardas |
|---|---|---|
| `zWorld` | 454 desc. / 25 saltos | **454 / 25** |
| `zGameplay` | 694 desc. / 253 saltos | **694 / 253** |

Idénticas. En esas unidades esas cabeceras no están en el camino crítico.

---

## 5. Vedas y diagnóstico pendiente

### 5.1 CERRADA: «las ranuras de vtable ordenan las plantillas»

Ni siquiera para plantillas. §3: cambiar la posición de la declaración del
destructor virtual no mueve una sola instrucción.

### 5.2 `zSim` (y la cola de `zMain`): lo que queda son TRES funciones

El objetivo, índices 370-402:

    _Storage×3 · SType×2 · OnGrowRequest__Vector×3 ·
    [Alloc,Free,GGS,GMC]__FixedVector ×3 · GetGrowSize__Vector×3 ·
    (~Vector, ~FixedVector)×3 · GetMaxCapacity__Vector×3

Nosotros somos idénticos salvo que los tres `~FixedVector` van en bloque en
378-380 en vez de intercalados en 394/396/398. El patrón del objetivo
(`~Vector<A>, ~FixedVector<A>, ~Vector<B>, …`) es exactamente lo que sale si la
DEPENDENCIA se emite delante de quien la pide; el nuestro es lo que sale si
`~FixedVector` se demanda una pasada antes. Lo que hay que buscar es **quién
demanda `~Vector<T>` antes que `~FixedVector<T,N>` en el original**, no cómo
están declarados.

### 5.3 `zAttribSys`: los cuatro racimos que quedan, ya diagnosticados

1. **`Attrib::HashMap::Transfer`** (1 fn). El objetivo lo emite en 171, pegado a
   `RebuildTable` (170) y en orden de línea; nosotros en 191, una pasada más
   tarde. En nuestra cabecera el orden de declaración YA es
   `RebuildTable(83) … Transfer(236) … UpdateSearchLength(245)`, o sea que la
   causa no es la declaración: en el objetivo `Transfer` ya estaba «necesitada»
   antes de la pasada.
2. **`Definition::GetFlag`** (`attribsys.h:572`): el objetivo lo emite el primero
   del bloque diferido (162), nosotros detrás del racimo de `AttribHashMap.h`
   (169). Es orden de parseo entre `attribsys.h` y `AttribHashMap.h`.
3. **`TypeTable` / `ClassTable`** (4 fns): el objetivo emite `~ClassTable` antes
   que `~TypeTable`, y el constructor de `TypeTable` DETRÁS del de
   `ClassPrivate::CollectionHashMap`. Nuestro `class TypeTable` está en
   `AttribSys.h:195` y `class ClassTable` en `AttribPrivate.h:241`, o sea que
   parseamos `TypeTable` antes. **Hipótesis sin probar: en el original
   `TypeTable` no está en `AttribSys.h`** — la usan `AttribPrivate.h`,
   `AttribDatabase.cpp` y `LuaAttributes.*`.
4. **El racimo de plantillas 135-145** (`__lower_bound`, `ScanForValidKey`,
   `find`): orden de instanciación, mismo tipo de problema que §5.2.

### 5.4 `zMain`: lo que queda (163 en 51 saltos)

Todo dentro del bloque diferido, y **todo es orden de parseo de cabeceras**. Los
que tienen el diagnóstico hecho pero no la causa:

* `MEnterRaceOverFlow.h` (objetivo 1088, nuestro 1111) y `MEnterFreeRoam.h` +
  `MFlowReadyForOutro.h` (1104/1106 contra 1107/1115): el objetivo los parsea
  ANTES de `EventDefs.cpp`, o sea desde algún `E*.cpp`. Para
  `MNotifyMovieFinished.h` el hueco alfabético dio el fichero a la primera
  (z10); para estos tres **ningún fichero del árbol los menciona salvo
  `EventDefs.cpp`**, así que no hay evidencia con la que elegir y no lo he
  adivinado.
* `ISceneryModel.h` (1084 contra 1102), `Smackable.h` (1085/1099),
  `Explosion.h` (1095/1098), `IResetable.h` (1071/1090), `IFeedBack.h`
  (1090/1074): el objetivo los parsea en un orden que **no es el orden de
  `#include` de nuestro `.ii`**, y ahí el modelo «orden de parseo» deja de
  predecir. Es el techo de esta ronda.
* La cola de plantillas `FixedVector`/`Vector` = §5.2.

---

## 6. Verificación

* **`build_direct.py` de las 33 SourceLists: 33 ok, 0 fallidas.** (Una pasada
  intermedia dio «3 fallidas» sin ningún mensaje de error: es otro proceso
  escribiendo el mismo `.o`; a la segunda pasan, igual que en la r34.)
* **`measure.py` de las 610 unidades: 3.866.216/3.946.204 B, 97,9730 %, 18.336
  funciones** — exactamente la cifra de cierre de la r34.
* `measure.py zMain zAttribSys`: **208.552/208.552 B, 100 %, 1.576 funciones al
  100 %**, igual que al empezar.
* `audit.py` en `zMain` y en `zAttribSys`: **0 FALLA** en las dos.
* `textorder.py --todas` antes y después: **9.794 → 9.463 funciones**, y fila a
  fila todas las ajenas idénticas (`zFe2` 1159, `zFe` 921, `zPhysicsBehaviors`
  817, `zAI` 800, `zEAXSound` 730, `zPhysics` 701, `zSpeech` 701, `zGameplay`
  694, `zEAXSound2` 513, `zWorld` 454, `zCamera` 429, `zEcstasy` 315,
  `zEagl4Anim` 273, `zWorld2` 272, `zLua` 166, `zPlatform` 135, `filesys` 58,
  `zAnim` 27, `zSim` 21, `ppc2D2` 15, `dvd_device` 15, `gc_interface` 10,
  `zTrack` 6, `steering` 5, `pathnode` 4, `zBWare` 2, `madidct`/`vm`/`lbmpeg` 1).
  La única fila nueva es `FSasync` (11), que no es mía.
* `trypromo.py`: `zMain` **DOL ROTO (49f29fba243f)**, `zAttribSys` **DOL ROTO
  (e7ab8780460a)**, `zSim` **DOL ROTO (d5a1439a9b01)**, control `zFoundation`
  (sin tocar) **DOL ROTO (a1823c4ad8b4)**. **Ninguna dice `ENLACE FALLA`**: el
  enlace sigue completo con las ocho funciones de `Engine` retiradas.
* `keepchk.py`: **755 ok, 25 RANCIAS, 0 sin objeto**; las 25 son de
  `zEAXSound2`(14), `zLua`(2), `zDynamics`(2), `sndfxbus`(2), `crt0`(2),
  `asd2`(2), `GXTexture`(1) — **ninguna de `zMain` ni de `zAttribSys`**: no he
  movido ninguna frontera.
* `config/GOWE69/*`, `configure.py` y `splits.txt`: **no los he tocado** (salen
  modificados en `git status`, pero por otros agentes; ni los he abierto para
  escribir).
* Los `#if defined(__ANDROID__)`: `src/types.h`, `bMath.hpp` y `UVectorMath.hpp`
  salen **sin modificar** en `git status`.
* **`main.dol`**: sigue con `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, marca de
  tiempo `2026-09-07 00:56`, o sea **de antes de mi última tanda** — no he
  lanzado un `ninja` completo (hay otros tres agentes en el mismo `build/`), así
  que ese hash NO es una verificación mía; la mía es el `trypromo.py` de arriba.

### 6.1 A/B por secciones de `zMain`

`c33ord2_secs.py` sobre el objeto base y el final, seguidos:

| | `.text` | `.rodata` | `.data` | `.bss` | `.ctors` |
|---|---|---|---|---|---|
| base | 179.964 | 20.344 | 3.260 | 47.860 | 4 |
| z1 (sólo las cabeceras) | 179.964 | **20.320** | 3.260 | 47.860 | 4 |
| final | **179.112** | 20.320 | 3.260 | 47.860 | 4 |

**Aviso honesto**: z1 cuesta **24 B de `.rodata`** (el delta contra el objetivo
pasa de `rodata−896` a `rodata−920`). Comprobado que **el contenido de cadenas es
idéntico** (diff de todas las cadenas imprimibles de `.rodata`: cero altas, cero
bajas), así que son relleno/pool recolocado, no dato perdido. Los −852 B de
`.text` son las ocho funciones muertas del §1.2 y **no cuesta ni una función del
objetivo**. `zAttribSys` no mueve ninguna sección.

---

## 7. Ficheros de `src/` que dejo modificados (76)

| fichero | ensayo | qué | guarda |
|---|---|---|---|
| `Src/Generated/Messages/M*.h` (**65**) | z1 | los dos cuerpos, `inline`, fuera de clase, al final de la cabecera, con `#include` de `LuaBindery.h`/`LuaPostOffice.h` | `ZMAIN_MESSAGES_LUA_INLINE` |
| `Src/Generated/Events/EventDefs.cpp` | z1, z9 | los 130 cuerpos bajo `#ifndef`; los `#include` en orden alfabético sin distinguir mayúsculas | — (sólo zMain) |
| `Src/Main/Common/EventSequencer.cpp` | z2 | `inline` a los 24 `EventSequencer::Engine::` | — |
| `Src/Input/Common/iomodule.cpp` | z3 | los ocho métodos en el orden del objetivo (119, 231, 243, 270, 357, 384, 403, 428) | — |
| `Src/Input/GC/InputDeviceGC.cpp` | z4, z5 | ctor+dtor de `GameDevice` detrás de `GetNumDeviceScalar`; `IsWheel`/`GetInterfaces`/`GetSecondaryDevice` al cuerpo de la clase, detrás de `Construct` | — |
| `Src/Input/Common/SteeringWheelDevice.cpp` | z6 | `StopAllForces` antes que `ReadInput` | — |
| `Src/Speech/SoundAI.h` | z7 | `class MUnspawnCop;` en vez del `#include` | `SOUNDAI_H_FWD_MUNSPAWNCOP` |
| `Src/Interfaces/SimEntities/IPlayer.h` | z8 | `class ISteeringWheel;` en vez del `#include` | `IPLAYER_H_FWD_ISTEERINGWHEEL` |
| `Src/Generated/Events/EPlayRaceMovie.cpp` | z10 | `#include MNotifyMovieFinished.h` | — |
| `Tools/AttribSys/Runtime/Common/AttribSupport.cpp` + `Attribute.cpp` | a1 | `Attribute::~Attribute()` al `.cpp` que le toca | — |
| `SourceLists/zMain.cpp` | — | los tres `#define`, delante del primer `#include` | — |

**Radio de explosión de las tres guardas: CERO por construcción.** `grep -rn`
sobre todo `src/` da **exactamente tres `#define`, los tres en
`SourceLists/zMain.cpp`** (líneas 14, 18 y 19). Para cualquier otra unidad el
preprocesado de las 67 cabeceras es idéntico salvo comentarios. Los cinco `.cpp`
sin guarda (`EventDefs`, `EventSequencer`, `iomodule`, `InputDeviceGC`,
`SteeringWheelDevice`, `EPlayRaceMovie`) los incluye **sólo `zMain.cpp`**, y
`AttribSupport.cpp`/`Attribute.cpp` **sólo `zAttribSys.cpp`** (`grep` sobre
`src/Speed/*/SourceLists/`).

Cada edición lleva su comentario en el fuente diciendo que es **por POSICIÓN**, y
en `EventDefs.cpp` un aviso explícito de **no reordenar los `#include` con un
sort ASCII**.

## 8. Guiones del scratchpad (`c35ord4_`)

| guion | qué hace |
|---|---|
| `c35ord4_ours.py <unidad> <base_hex> [desde] [hasta] [--obj] [--o=x.o]` | **la herramienta de la ronda**: NUESTRA secuencia (o la del objetivo con `--obj`) con el `fichero:línea` que `debug_lines` le da en el ORIGINAL, y el delta de índice al lado. Es lo que convierte un salto en una causa |
| `c35ord4_z1.py aplicar\|revertir [--sin-include] [--btfirst]` | mueve/devuelve los 130 cuerpos entre `EventDefs.cpp` y las 65 cabeceras |
| `c35ord4_edit.py prefijo\|mover\|swap\|restaurar` | ediciones por línea en binario, con copia de seguridad automática (finales de línea mixtos) |
| `c35ord4_bak/` | copia de los 76 ficheros antes de tocarlos |

Reutilizados de la r33/r34 sin cambios: `c34ord3_cc.py`, `c34ord3_to.py`,
`c33ord2_files.py`, `c33ord2_idx.py`, `c33ord2_incltree.py`, `c33ord2_secs.py`.
