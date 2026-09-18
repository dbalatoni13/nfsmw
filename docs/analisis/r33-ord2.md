# Ronda 33 — `ord2`: terminar el ORDEN del `.text` (zSim, zMisc)

Encargo: el §2 del brief de la r33. Las dos unidades enlazan, tienen el `.text`
del tamaño exacto y cero funciones ausentes; sólo les falta el ORDEN.

**Resultado: 91 de las 151 funciones descolocadas, colocadas.** Nueve ediciones
de `src/`, dos de ellas en cabecera compartida y **las dos con guarda**, así que
su radio de explosión es exactamente CERO unidades. **Sin perder un byte**:
`measure.py --cmp` da `+0 B, +0 funciones, 0 unidades cambian` sobre las 604.

| unidad | descolocadas antes | después | saltos | `trypromo` antes | después |
|---|---|---|---|---|---|
| `zSim` | 123 | **37** | 33 → **16** | DOL ROTO | DOL ROTO |
| `zMisc` | 28 | **23** | 9 → 10 | DOL ROTO | DOL ROTO |
| `zFoundation` (control, no tocada) | 0 | 0 | 0 | DOL ROTO `97a3f7d2b3cb` | DOL ROTO `97a3f7d2b3cb` |

Ninguna promociona: **el `.text` no aporta ni un byte de diferencia de tamaño**
(§7.2, A/B por secciones). Lo que falta es dato, y **no es mío** (§6).

---

## 0. Las cuatro frases

1. **El bloque diferido sale en el orden en que se COMPLETA la definición de
   cada clase**, y la palanca es el **orden de los `#include` del `.cpp`**, no el
   de la SourceList. Medido cuatro veces en zSim; **una sola edición
   (reordenar diez `#include` de `Simulation.cpp`) valió 45 funciones**.
2. **`symbols/debug_lines.txt` te da GRATIS el orden de parseo del original.**
   El fichero y la línea de cada función del bloque diferido del objetivo, leídos
   en orden de dirección, *son* el orden en que el original parseó sus cabeceras.
   Eso convierte «adivinar los includes» en «copiar una lista». Ver §2.
3. **`#pragma interface` / `#pragma implementation` sobre `driver.h` era el
   mecanismo equivocado**: sacaban las nueve inline de `RealFile::DeviceDriver`
   juntas. El original usa **COMDAT a secas**, que las reparte en dos pasadas de
   `finish_file` — exactamente como el objetivo (§3, m2').
4. **Una guarda `#ifndef` en la cabecera compartida + un `#define` en la
   SourceList da el cambio con radio de explosión CERO POR CONSTRUCCIÓN**, y es
   lo que salvó los dos ensayos que tocaban cabecera compartida. Está medido:
   el barrido de las 526 unidades sale idéntico al de la r32 salvo mis dos.

---

## 1. El mecanismo, ya no es teoría

La r32 dejó dicho que el bloque diferido se ordena «por orden de parse de la
definición de la clase». **Esta ronda lo ha usado como herramienta y ha
funcionado cuatro veces seguidas.** El protocolo, que es reproducible:

1. `python scripts/textorder.py <unidad> --lista` para ver el desorden.
2. Sacar el **fichero y la línea** de cada función del bloque diferido **del
   objetivo** con `symbols/debug_lines.txt` (el `.text` del objeto extraído se
   mapea a direcciones sumando el `Code range` de la CU en
   `symbols/mw_dwarfdump.nothpp`). Eso es el orden de parseo del original.
3. Sacar el **nuestro** preprocesando la unidad (`ngccc -E`, con
   `SN_NGC_PATH` puesto) y leyendo las marcas `# N "fichero"`: la primera
   aparición de cada cabecera, con su cadena de inclusión.
4. Cotejar y **reordenar los `#include` del `.cpp`**, respetando el grafo
   transitivo (si `A.h` incluye `B.h`, `B` se completa siempre antes).

Guiones del scratchpad que lo automatizan: `c33ord2_files.py` (objetivo:
símbolo + fichero:línea + índice nuestro), `c33ord2_idx.py` (las dos secuencias
por índice), `c33ord2_iiorder.py` (orden de primera aparición en un `.ii`),
`c33ord2_incltree.py` (**la cadena de inclusión** de esa primera aparición, que
es lo que dice a QUIÉN hay que tocar), `c33ord2_perm.py`, `c33ord2_secs.py`.

### 1.1 Lo que el modelo NO explica

El orden es de parseo **sólo para las inline no virtuales**. Las virtuales salen
con **su vtable**, y ahí manda otra cosa (§5.1). En zMisc conviven los dos
regímenes y por eso queda más desorden proporcional que en zSim.

## 2. La lista de parseo del original de zSim, leída del mapa de líneas

Del `.text` del objetivo (índices 261-305), con `debug_lines.txt`:

    IPlayer.h · IEntity.h · IServiceable.h · ITaskable.h · SimObject.h ·
    IActivity.h · SimActivity.h · Simulation.h · SimEntity.h · SimEffect.h ·
    SimModel.h

El nuestro (del `.ii`) era:

    Simulation.h · ITaskable.h · INIS.h · IActivity.h · IPlayer.h ·
    SimObject.h/IServiceable.h · SimActivity.h · SimEntity.h/IEntity.h ·
    SimModel.h/SimEffect.h

y **el orden del objetivo es alcanzable**: las cuatro restricciones del grafo
(`ITaskable.h`⊂`Simulation.h`, `IServiceable.h`⊂`SimObject.h`,
`IEntity.h`⊂`SimEntity.h`, `SimEffect.h`⊂`SimModel.h`) las cumple.

## 3. Los ensayos

### m1 — `zMisc`: `HighAttribAlloc` vive en `GameFlow.cpp`, no en `AttribAlloc.h` → 28 → 28 (y −5 con m2')

`debug_lines` pone `Allocate__15HighAttribAlloc` en **`gameflow.cpp:3521`** y
`Free` en **3526**; nosotros la teníamos en `AttribAlloc.h`, que `Hermes.h`
incluye y por tanto se parsea de las primeras. Nadie más usa la clase
(`grep`: sólo `GameFlow.cpp:833` y `:853`), así que se mueve entera al `.cpp`,
delante de `LoadFrontEndVault`.

Sola no cambia la cuenta; **con m2' vale 5 funciones** (28 → 23, medido
revirtiéndola y volviéndola a poner).

### m2' — `zMisc`: `driver.h` por COMDAT, no por `#pragma implementation` → 28 → 23

El objetivo emite `GetName`/`GetOptimalReadSize` en la **pasada 1** de
`finish_file` (índices 432-433, entre `QueuedFile` y `bFile`) y las otras siete
(`~`, `Init`, `Restore`, `Write`, `QueryLocation`, `Remove`, `Getspace`) en la
**pasada 2**, al final del todo (443-449). Con `#pragma interface` en `driver.h`
+ `#pragma implementation "driver.h"` en `zMisc.cpp` salían **las nueve juntas y
al principio**: el pragma las marca «emítelas todas aquí», en orden de
declaración.

Quitando los dos pragmas salen exactamente repartidas. **Pero quitar el
`#pragma interface` a secas le mete a `filesys`, `dvd_device` y `hd_device`
9 funciones y una vtable de más cada uno** (medido: `hd_device` pasa de 14
funciones/820 B a 23/936 B). Por eso va **con guarda**:

    driver.h:   #ifndef REALCORE_DRIVER_H_COMDAT_INLINES
                #pragma interface
                #endif
    zMisc.cpp:  #define REALCORE_DRIVER_H_COMDAT_INLINES   (antes del 1er include)

Comprobado tras la guarda: `filesys` 75 funciones/10.868 B, `dvd_device`
19/2.680, `hd_device` 14/820 — **idénticos a antes de tocar nada**.

Nota de por qué el original puede permitírselo: sus `filesys`/`dvd_device`/
`hd_device` **también** emitían las copias WEAK; el enlazador se queda con la de
zSim/zMisc porque va antes en el orden de enlace (los símbolos viven en
`0x8020D9CC`, rango de zMisc, y el constructor no-inline en `0x80381A94`, rango
de `filesys`). Nuestro `#pragma interface` era un apaño para que el **objeto**
saliera limpio, a costa de desviarse del original.

### s2 — `zSim`: `Sim::Internal::CDispatcher` más abajo en `Simulation.cpp` → 123 → 111

El bloque de plantillas `0x13CB4..0x14934` (12 funciones, 3 saltos) estaba
invertido: el objetivo pone los cuatro `find_if<GarbageNode<…>>` **antes** del
grupo `reserve`/`lower_bound`/`upper_bound`/`find_if` de `CDispatcher::Node`.
`instantiate_pending_templates` emite en **orden de primera exigencia**, y el
cuerpo de `CDispatcher` (definido en `Simulation.cpp:79`, con un
`// TODO right place? or more down in the file?` encima que ya lo sospechaba)
exigía sus plantillas antes de que `SimSystem::CollectGarbage` (línea 593)
exigiera las de `GarbageNode`.

Movida la `struct` entera a justo antes de `namespace Sim { namespace Collision`
(su primer uso está en la línea 762): **el bloque de plantillas queda PERFECTO**,
236-247 con delta 0.

### s3 — `zSim`: reordenar diez `#include` de `Simulation.cpp` → 111 → 66

**El ensayo más rentable de la ronda: 45 funciones con un bloque de includes.**
La lista de `Simulation.cpp` estaba **ordenada alfabéticamente** (convención del
decomp), que es justo lo que el original no puede tener. Se antepone el orden
del §2. Los índices 261-305 (45 funciones) pasan a delta 0.

### s4 — `zSim`: los dos `ClassKey` a `Simulation.cpp` → 66 → 64

El objetivo los pone en 306-307, pegados al bloque de `SimModel.h`. Estaban
definidos en `SimModel.cpp` (fichero 12 de la lista). Movido el bloque
`#ifdef ATTRIB_NO_INLINE_CLASSKEY` entero a `Simulation.cpp`, justo detrás del
`#include SimModel.h`.

### s5 — `zSim`: `WorldConn.h` al final del bloque de includes de `Simulation.cpp` → 64 → 60

El objetivo emite `WorldConn::Pkt_Effect_*` en 308-315, **delante** de
`SimTask`/`SimSystem` (`Simulation.cpp:214` y `:460`). Llegando por
`SimEffect.cpp` (fichero 9) salían 30 posiciones más tarde.

### s6' — `zSim`: `Camera.hpp` no debe traer `INIS.h` → 60 → 40

`INIS.h` llegaba por `INIS.h ← Camera.hpp ← CameraMover.hpp ← Simulation.cpp`,
o sea dentro del fichero 2; el objetivo lo tiene entre `LocalPlayer.hpp` y
`IGameState.h` (índice 338), es decir dentro del fichero 5. `Camera.hpp` **no
usa `INIS` para nada** (`grep`: sólo la línea del `#include`).

**Quitar el include a secas rompe `zCamera` y `zMisc`** (§5.3), así que va con
guarda: `#ifndef CAMERA_HPP_NO_INIS` en `Camera.hpp` y `#define CAMERA_HPP_NO_INIS`
en `zSim.cpp`. **20 funciones.**

### s7 — `zSim`: `INIS.h` antes de `IGameState.h` en `LocalPlayer.cpp` → 40 → 37

Las dos venían seguidas y **ordenadas alfabéticamente**; el objetivo emite
`_._4INIS` (338) delante de `_IHandle__10IGameState` (339). `LocalPlayer.cpp`
sólo lo incluye `zSim`: radio cero.

### s1 y s8 — REVERTIDOS, ver §5.2

## 4. Lo que queda, con su diagnóstico

### 4.1 `zSim`: 37 en dos racimos exactos

**Racimo A — 8 funciones, 0x16394..0x164F4.** El destructor de
`WorldConn::Pkt_Effect_Send`/`_Open` debe salir **DELANTE** de
`ConnectionClass`/`Size`/`Type`; el nuestro sale detrás. Nosotros seguimos el
orden de declaración de la clase derivada (`DECLARE_WORLDPACKET` declara
`ConnectionClass`, luego `DECLARE_SIMPACKET` declara `Size`, `SType`, `Type`, y
el `~` va detrás del constructor). **El DWARF del original tiene EXACTAMENTE la
misma lista de declaración** (`ConnectionClass, Size, SType, Type, ctor, ~`) y
aun así emite `~` primero: allí manda la **ranura de vtable**, no la
declaración. Para reproducirlo habría que llevar `virtual ~Packet()` al
principio de `Sim::Packet` (`SimConn.h`) — **eso cambia el reparto de ranuras de
todas las subclases de `Packet` y por tanto sus vtables**. NO LO HE HECHO: es
dato de otros y el riesgo no compensa 8 funciones.

**Racimo B — 29 funciones, 0x171FC..0x177D0.** La cola de destructores de
`UTL::Collections::_Storage`, `UTL::FixedVector` y `UTL::Vector` sobre
`GarbageNode<Sim::Activity/Sim::Entity/Sim::Model>`. Objetivo:
`_._Storage`×3 · `SType`×2 · `OnGrowRequest`×3 · el bloque
`AllocVectorSpace/FreeVectorSpace/GetGrowSize/GetMaxCapacity` · y luego
**alternando** `_._Vector`/`_._FixedVector` por tipo. Nuestro: `SType`×2 ·
`OnGrowRequest`×3 · **parejas** `_._FixedVector`+`_._Storage` por tipo · el
bloque · los tres `_._Vector` al final. Es orden de instanciación de plantilla;
la palanca de zSim.cpp está agotada (§5.2).

### 4.2 `zMisc`: 23, y la mitad son vtables

Orden nuestro (índice objetivo entre paréntesis):

    GetName(432) GORS(433) LoadHandler(424) map(422) RebuildTable(423)
    QueuedFile×4(428-431) tShaker(427) HighAttribAlloc×2(425-426)
    bFile×5(434-438 OK) DeviceDriver×7(443-449) AttribAsset×2(439-440)
    AttribAlloc×2(441-442)

Cuatro diferencias:

- **`map`+`RebuildTable` deberían ir los PRIMEROS** (son instanciaciones de
  plantilla: en el objetivo la pasada 1 de `finish_file` las emite antes que
  nada; en el nuestro nacen en la pasada 2).
- **`GetName`/`GORS` deberían ir detrás de `QueuedFile`**: los arrastra la vtable
  de un `DeviceDriver` derivado. El original tiene un
  `struct BigVFS : public DeviceDriver` en `bfile.cpp` (está en el volcado DWARF,
  línea 75102 de la CU) **que nosotros no tenemos**; es abstracta (sólo ctor y
  `~`), así que no es un simple «escribir la clase».
- **`HighAttribAlloc` y `tShaker` están intercambiados** respecto de
  `QueuedFile`: el objetivo es `HAA · tShaker · QueuedFile` (que es el orden de
  primera exigencia por fichero: `gameflow.cpp` · `rumble.cpp` ·
  `queuedfile.cpp`) y el nuestro `QueuedFile · tShaker · HAA`. Nuestro
  `QueuedFile.hpp` se parsea en `Main.cpp` (fichero 3) porque `Main.cpp`,
  `ResourceLoader.cpp` y `GameFlow.cpp` lo incluyen; en el original no puede ser
  así.
- **`DeviceDriver`×7 debería ir DETRÁS de `AttribAsset`/`AttribAlloc`**: una
  pasada más tarde.

## 5. Vedas

### 5.1 El orden de las vtables no es orden de parseo (y no lo he cerrado)

En `zMisc` el `.rodata` sale con las vtables en este orden: `AverageWindow`,
`Average`, `AverageBase`, `DisculatorDriver`, `HighAttribAlloc`, `EasterEggs`,
`DefaultAttribAllocator`, `VaultGarbageCollector`, `FileGarbageCollector`,
`DeviceDriver`. **No es ni el orden de definición ni su inverso**, y las
funciones virtuales salen con su vtable, no con el parseo. Mientras eso no esté
medido con un caso mínimo, los cuatro racimos de §4.2 y el racimo A de §4.1 no
tienen palanca fiable. **Es el siguiente experimento de `cc1plus -O1 -S` que hay
que hacer**, y es barato: una clase con virtuales, un objeto estático y un
derivado, mirando `.rodata`.

### 5.2 `zSim`: la posición de los estáticos de plantilla NO mueve la cola (s1, s8)

- **s1**: mover los tres `GarbageNode<…>::_mCollector` de `zSim.cpp` **delante de
  `Simulation.cpp`** → **no compila** (`Sim::Activity`/`Entity`/`Model` no están
  declarados todavía; `OBB.cpp` no los trae). Revertido.
- **s8**: moverlos a **justo detrás de `Simulation.cpp`** (en vez de detrás de
  `LocalPlayer.cpp`) → **37 descolocadas, exactamente las mismas**, mismos 16
  saltos. Revertido para dejar el diff mínimo.

Conclusión: el racimo B de §4.1 **no** se mueve desde `zSim.cpp`. Hay que
buscarlo en el primer uso dentro de `UCollections.h`/`UTLVector.h`.

### 5.3 `Camera.hpp`: quitar `#include INIS.h` de verdad rompe el subsistema ICE

Es lo limpio y es casi seguro lo que el original tiene, pero cuesta cuatro
arreglos y uno no es trivial. Medido, compilando:

- `CameraMover.cpp:838` y `CameraAI.cpp:347`: `INIS` undeclared → basta añadir el
  `#include` en los dos `.cpp`.
- `ResourceLoader.cpp:68/69` (¡de zMisc!): `LoaderAnimSceneData` /
  `UnloaderAnimSceneData` → llegaban por `INIS.h → AnimScene.hpp`; basta añadir
  `AnimScene.hpp`.
- **`ICEData.cpp:185`, `ICEMover.cpp:321/537`, `ICEManager.cpp:52/448/491`,
  `ICEAnimScene.cpp:66`: `type ICEScene is not a base type for type
  ICE::ICEScene`.** Ésta no es un include que falte: sin `ICEAnimScene.hpp` por
  esa vía, la declaración adelantada `ICE::ICEScene` de `ICEManager.hpp:358` gana
  y el `ICEScene` global deja de resolverse. **Es un bug real de nuestras
  cabeceras del subsistema ICE**, y arreglarlo es trabajo de zCamera.

Probado y revertido: con los tres primeros arreglos puestos, `zMisc` vuelve a
compilar pero `zCamera` sigue cayendo por el cuarto. **Por eso va la guarda.**

### 5.4 Lo que NO he probado

1. El caso mínimo de vtables del §5.1. **Es lo que desbloquea zMisc.**
2. El racimo B de zSim (29 funciones): primer uso de `_Storage`/`FixedVector` en
   `UCollections.h`/`UTLVector.h`.
3. Las 30 unidades restantes del barrido (§7.3). **`ppc2D2` (15/16, 7 saltos),
   `dvd_device` (15/19, 1 salto), `pathnode` (4/12, 1), `steering` (5/36, 1),
   `madidct`, `vm`, `lbmpeg` (1 cada uno) siguen sin mirar**, y ahora hay un
   protocolo (§1) para atacarlas.
4. `zFEng` aparece en el barrido con **47 descolocadas y 23 saltos** y **no
   estaba en el de la r32**. No es mío (§7.4) — que alguien lo mire.

## 6. Los datos que les faltan, para el agente de datos

`promodist.py --sl` con el árbol al final de mi turno:

| unidad | `.rodata` | `.data` | `.bss` |
|---|---|---|---|
| `zSim` | **+128** | −240 | +168 |
| `zMisc` | **+4.768** | +20 | +288 |
| `zFoundation` | **−1.240** | −200 | 0 |

**Ninguno de esos números es mío.** El A/B del §7.2 demuestra que mis nueve
ediciones dejan las cinco secciones de las dos unidades **byte a byte iguales**.
Los deltas han cambiado respecto de la r32 (zSim iba en −1.600 y zMisc en
−2.208) porque otro agente está escribiendo pool a mano en esas unidades; ahora
**se han pasado**: a zMisc le sobran 4.768 B de `.rodata` y a zSim 128.

## 7. Verificación

### 7.1 Lo estándar

- `build_direct.py` de las 33 SourceLists: **33 ok, 0 fallidas**; de las 42 de
  `realcore`: **42 ok, 0 fallidas**.
- `measure.py zSim zMisc zMiscSmall zFoundation`: **211.176/211.176 B, 100 %,
  1.048 funciones**, igual que al empezar. `--cmp` base/final: **+0 B, +0
  funciones, 0 unidades cambian**.
- `measure.py` de **las 604 unidades**, base y medida con el árbol construido
  entero las dos veces: **3.866.260/3.946.204 B, 97,9742 %, 18.337 funciones**
  antes y después, `--cmp` = **+0 B, 0 unidades cambian**.
- `audit.py` en `zSim`, `zMisc` y `zFoundation`: **0 FALLA** en las tres.
- `keepchk.py`: **759 ok, 21 RANCIAS, 0 sin objeto**. Las 21 son de
  `zEAXSound2` (14), `asd2` (2), `zDynamics` (2), `sndfxbus` (2) y `GXTexture`
  (1): **ninguna en mis unidades ni en `realcore`**. No he movido ninguna
  frontera.
- `config/GOWE69/{splits.txt,symbols.txt,keep.lst,config.yml}` y `configure.py`:
  **`git status` vacío**. No propongo ningún cambio de configuración.
- Los `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- Enlace: `trypromo.py` enlaza bien las tres. **`zFoundation`, que no he tocado,
  da el mismo DOL (`97a3f7d2b3cb`) al empezar y al terminar** — o sea que mis
  ediciones no cambian ni un byte del DOL enlazado.

### 7.2 A/B por secciones: mis cambios son SÓLO orden

Compilando la unidad con mis ediciones y sin ellas, seguidas:

| unidad | `.text` | `.rodata` | `.data` | `.bss` | `.ctors` |
|---|---|---|---|---|---|
| `zSim` con / sin | 114.492 / 114.492 | 8.008 / 8.008 | 108 / 108 | 8.180 / 8.180 | 4 / 4 |
| `zMisc` con / sin | 86.368 / 86.368 | 12.632 / 12.632 | 2.184 / 2.184 | 2.560 / 2.560 | 4 / 4 |

### 7.3 El barrido completo, contra el de la r32

`textorder.py --todas`: **33 de 526 unidades desordenadas, 9.880 funciones**
(la r32: 32 de 525 y 9.924). Fila a fila **todas las demás salen con la cifra
exacta de la r32** — `zFe2` 1159, `zFe` 921, `zPhysicsBehaviors` 817, `zAI` 800,
`zEAXSound` 730, `zPhysics` 701, `zSpeech` 701, `zGameplay` 694, `zEAXSound2`
513, `zMain` 501, `zWorld` 454, **`zCamera` 429**, `zEcstasy` 315, `zEagl4Anim`
273, `zWorld2` 272, `zLua` 166, `zPlatform` 135, **`filesys` 58**, `zAttribSys`
47, `zAnim` 27, `ppc2D2` 15, **`dvd_device` 15**, `gc_interface` 10, `zTrack` 6,
`steering` 5, `pathnode` 4, `zBWare` 2, `madidct`/`vm`/`lbmpeg` 1.

Cambian tres filas: **`zSim` 123 → 37** y **`zMisc` 28 → 23** (mías),
`rcmp_vp6_codec` desaparece y **`zFEng` aparece con 47/23** (no mías, §7.4).
`zFoundation` sigue sin aparecer.

### 7.4 Radio de explosión de las dos cabeceras compartidas: CERO, por construcción

`driver.h` y `Camera.hpp` sólo cambian de tokens si está definida
`REALCORE_DRIVER_H_COMDAT_INLINES` o `CAMERA_HPP_NO_INIS`, y
`grep -rn` sobre todo `src/` da **exactamente dos definiciones**:
`zMisc.cpp:15` y `zSim.cpp:21`. Para cualquier otra unidad el preprocesado es
idéntico byte a byte. Confirmado además por las tres medidas independientes de
arriba (`zCamera` 429 igual que la r32, las tres de `realcore/file` con su cuenta
exacta, y el `--cmp` global a cero).

Por eso `zFEng` **no puede ser mío**: no define ninguna de las dos guardas.

### 7.5 Aviso de convivencia (no es mío, pero me costó una medida)

**Otro agente reescribió `src/Speed/Indep/SourceLists/zMisc.cpp` a mitad de mi
turno** (movió el bloque `asm()` del pool) y se llevó por delante mi
`#define REALCORE_DRIVER_H_COMDAT_INLINES`, devolviendo el
`#pragma implementation "driver.h"`. Se detectó porque `textorder` volvió a dar
28 en vez de 23. **Reaplicado quirúrgicamente sobre el fichero nuevo**, sin tocar
su bloque de datos. Si vuelve a pasar, las dos líneas están en el §8.

Un `build_direct.py` completo también me falló una vez con
`Could not open output file .../zDynamics.o`: es otro proceso escribiendo el
mismo `.o`, no un error de compilación.

## 8. Ficheros de `src/` que dejo modificados

| fichero | ensayo | qué |
|---|---|---|
| `Speed/Indep/Src/Misc/AttribAlloc.h` | m1 | **quitada** la clase `HighAttribAlloc` |
| `Speed/Indep/Src/Misc/GameFlow.cpp` | m1 | **puesta** ahí, delante de `LoadFrontEndVault` |
| `Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h` | m2' | `#pragma interface` bajo `#ifndef REALCORE_DRIVER_H_COMDAT_INLINES` |
| `Speed/Indep/SourceLists/zMisc.cpp` | m2' | fuera `#pragma implementation "driver.h"`; `#define REALCORE_DRIVER_H_COMDAT_INLINES` antes del primer include |
| `Speed/Indep/Src/Sim/Common/Simulation.cpp` | s2,s3,s4,s5 | `CDispatcher` movida abajo; bloque de 10 `#include` ordenados delante; bloque `ClassKey`; `#include WorldConn.h` al final |
| `Speed/Indep/Src/Sim/Common/SimModel.cpp` | s4 | **quitado** el bloque `ATTRIB_NO_INLINE_CLASSKEY` |
| `Speed/Indep/Src/Camera/Camera.hpp` | s6' | `#include INIS.h` bajo `#ifndef CAMERA_HPP_NO_INIS` |
| `Speed/Indep/SourceLists/zSim.cpp` | s6' | `#define CAMERA_HPP_NO_INIS` antes del primer include |
| `Speed/Indep/Src/Sim/Entities/LocalPlayer.cpp` | s7 | `INIS.h` antes de `IGameState.h` |

`Simulation.cpp`, `SimModel.cpp`, `LocalPlayer.cpp` y `zSim.cpp` los usa **una
sola SourceList** (`grep`). `GameFlow.cpp` y `zMisc.cpp`, también una.
`AttribAlloc.h` la incluyen `Hermes.h`, `AttribSys.h` y `GVault.cpp`, pero la
clase retirada **no la usa nadie más** (`grep`) y el barrido lo confirma.
`driver.h` y `Camera.hpp` van con guarda (§7.4).

Cada edición lleva su comentario en el fuente diciendo **que es por POSICIÓN**
y, en los tres sitios donde el orden alfabético de `#include` era la causa, un
aviso explícito de **no volver a ordenarlos**.

## 9. Guiones del scratchpad (`c33ord2_`)

| guion | qué hace |
|---|---|
| `c33ord2_files.py <unidad> <base_hex> [desde] [lista\|grupos]` | la secuencia del OBJETIVO con **fichero:línea** de `debug_lines.txt` y el índice nuestro. `grupos` la pliega por fichero: es el §2 en una orden |
| `c33ord2_idx.py <unidad> [desde]` | las dos secuencias por índice, en paralelo |
| `c33ord2_perm.py <unidad> [lo] [hi]` | igual pero filtrando por dirección |
| `c33ord2_iiorder.py <ii> [ficheros…]` | orden de **primera aparición** de cada fichero en un `.ii` |
| `c33ord2_incltree.py <ii> [ficheros…]` | **la cadena de inclusión** de esa primera aparición: dice a QUIÉN hay que tocar |
| `c33ord2_secs.py <objeto>` | tamaño de cada sección (el A/B del §7.2) |
| `c33ord2_bak/`, `c33ord2_mios/` | copias de seguridad antes/después de cada fichero tocado |
