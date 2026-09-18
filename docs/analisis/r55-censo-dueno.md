# r55 — censo global del CAMBIO DE DUEÑO, hecho por el predicado de DEFINICIÓN

2026-09-09. Ronda de medida: **no he tocado una línea de fuente**. Todo lo que sigue sale de
leer tablas de símbolos y de seis enlaces de comprobación.

**Resumen en tres líneas.** Hay **29 símbolos** que cambian de dueño de verdad, **1.128 B**
en total, y desplazan **3.524.848 B** repartidos entre **10** de las 22 SourceLists del
bloque A. Los otros **26** que el predicado saca **no cuestan nada**: nadie los referencia y
`-strip-unused-data` los tira. Y la cifra que `movidos.py` imprime bajo «CAMBIO DE DUEÑO»
**no mide esto**: en `zLua` dice −17.676 B de `.text` y el censo dice **cero**.

---

## 0. El predicado, y la regla del ganador que hubo que descubrir

Para cada símbolo definido (`shndx != 0`, binding no LOCAL, sin `$LC`, sin `STT_SECTION`
ni `STT_FILE`) en los 616 objetos del enlace, en sus **dos** versiones — el extraído
(`build/GOWE69/obj/…`) y el nuestro (`build/GOWE69/src/…`) —, se calcula quién gana el
enlace. Cambio de dueño = **cambia el índice de unidad ganadora**.

La regla del ganador **no es «el primero del orden de enlace»**. Son tres rangos, y los tres
están comprobados con enlaces reales, no supuestos:

| rango | qué es | prueba |
|---:|---|---|
| **0** | definición real **GLOBAL** | `clear__…_List_base<SPCHType_1_EventID>` lo dan `src/zSim.o` (idx **27**, WEAK) y `obj/zSpeech.o` (idx **28**, GLOBAL). En el ELF base cae en `0x802AE948`, **dentro de la región de zSpeech** `[0x8028A1D0,0x802B5C18)`. **GLOBAL gana aunque vaya después.** |
| **1** | definición real **WEAK** (plantilla / inline: GCC 2.95 las emite weak) | entre weaks gana la primera del orden |
| **2** | **SHN_COMMON** (definición tentativa) | ver abajo — es el error que casi arruina el censo |

Gana el de **menor `(rango, índice)`**.

### El SHN_COMMON: 20 cambios de dueño y 4,7 MB que NO existen

La primera versión del censo trató `shndx != 0` como «define». **Está mal.** `SHN_COMMON`
(0xFFF2) es una definición *tentativa*: el enlazador sólo le reserva sitio si **nadie** da
una definición real, así que **nunca le quita el símbolo a otro objeto**.

Sin esa corrección el censo inventaba una familia entera —«estáticos de clase reclamados al
blob `auto_*`»— de **20 símbolos, 42.742 B y 4.679.328 B de radio**, encabezada por
`_20GrandSceneryCullInfo.SceneryDrawInfoTable` (**42.000 B**, zTrack).

**Refutado enlazando.** Promocionando `zTrack`:

    _20GrandSceneryCullInfo.SceneryDrawInfoTable   base 0x804F42F0  promo 0x804F3F50  delta -928
    _12bChunkLoader.sLoaderTable                   base 0x804F4108  promo 0x804F3D68  delta -928

−928 B es el arrastre de sección, exactamente igual que el resto de `.bss`. **El símbolo no
se mueve**: nuestro `zTrack.o` lo declara COMMON y `obj/auto_07_804F4040_bss.o` lo define de
verdad. Nueve unidades del bloque A tienen símbolos COMMON (zTrack 1, zWorld2 10,
zPhysicsBehaviors 1, zFe2 2, zEAXSound2 2, zFeOverlay 2, zAI 1, zFe 1, zMisc 1) y **ninguno
de ellos roba nada**.

Y el caso recíproco sí cuenta, y también está enlazado: `auto_07_804FEA4C_bss.o` declara
`_t10ScratchPtr1ZQ215SimpleRigidBody8Volatile.mPointer` **COMMON** y nuestro
`zPhysicsBehaviors.o` lo define de verdad en `.data` → **se mueve −915.760 B**.

**Si tu herramienta lee símbolos de objetos y no filtra `SHN_COMMON`, miente.**

---

## 1. El control obligatorio

Los dos `clear__` del brief salen, y salen con su respuesta correcta — que no es la misma
para los dos:

    clear__Q24_STLt10_List_base2Z23WGridManagedDynamicElemZ…  (120 B, .text)
       original : zSim                                        (un solo objeto)
       nuestros : zGameplay, zMisc, zPhysics, zSim, zWorld2    (CINCO)
       gana     : original=zSim   todo-nuestro=zGameplay   enlace ACTUAL=zSim
       cambia de dueño al promocionar: zGameplay, zMisc, zPhysics

    clear__Q24_STLt10_List_base2Z13WGridNodeElemZ…            (120 B, .text)
       original : zAI                                         (un solo objeto)
       nuestros : zAI, zGameplay, zMisc, zPhysics, zSim, zWorld2  (SEIS)
       gana     : original=zAI    todo-nuestro=zAI        enlace ACTUAL=zAI
       cambia de dueño al promocionar: NINGUNA

El segundo **está sobre-definido igual que el primero pero cuesta CERO**, porque su dueño
original, `zAI`, es el **objeto número 0** del orden de enlace y no puede perder nunca. Es la
diferencia entre «lo definimos de más» y «cambia de dueño», y por eso el informe lleva las
dos capas separadas.

Y los radios reproducen la tabla de la r54 dentro del ruido del árbol en vuelo:

| | r54 (`control`) | r55 (este censo) |
|---|---:|---:|
| `clear__…WGrid…` en zGameplay | −852.004 | **−847.152** |
| ídem en zMisc | −499.872 | **−493.816** |
| ídem en zPhysics | −388.752 | **−382.240** |
| `g_bCustomizeManagerHasControl` en zAnim | −146.332 | **−146.332** |

---

## 2. Las cifras

* **17.996** símbolos tienen distinto conjunto de definidores; en **3.824** definimos **de más**.
  Ése es el fenómeno, y es enorme — pero casi todo es inofensivo.
* **50** símbolos cambian de ganador al promocionar **una sola** unidad contra la lista de
  enlace actual. De ellos **26 los estripa el enlazador** (nadie los referencia) →
  **24 vivos**.
* **29** símbolos cambian de ganador en el mundo **todo-promocionado**, y **los 29 están
  vivos**. Los 5 que no aparecen en la vista de una unidad son **latentes**: hoy los tapa una
  definición GLOBAL de un `obj/*.o` sin promocionar.
* **Total: 29 símbolos distintos, 1.128 B de símbolo, 3.524.848 B desplazados.**

### 2.1 Familias (sólo los vivos)

| familia | símbolos | unidades | bytes | desplazamiento |
|---|---:|---:|---:|---:|
| **A.** `CarCustomize.hpp`: una cabecera que **DEFINE** 14 globales | 14 | 2 | 56 | 264.488 |
| **B.** `_STL::_List_base<T>::clear` instanciada por un método **en clase** | 2 | 4 | 240 | 1.875.704 |
| **C.** estático de clase que hoy da un blob `auto_*` (COMMON contra real) | 2 | 1 | 640 | 916.004 |
| **D.** global suelto **definido en dos SourceLists** | 6 | 3 | 22 | 468.652 |
| **E.** plantilla / plantilla de función que sólo salta en el mundo todo-promocionado | 5 | 4 | 804 | latente |
| **(F.)** `Attrib::TAttrib<T>::Get` — **25 símbolos, 2.000 B, coste CERO** | 25 | 15 | 2.000 | **0** |

**La sospecha del brief se confirma sólo a medias.** Sí, las plantillas de contenedor
(`_List_base`, `_Rb_tree`, `_STL::vector`) son el grupo más numeroso — 32 de los 55 símbolos
del censo —, pero **26 de esos 32 no cuestan un byte**, y el desplazamiento de verdad se lo
reparten a partes casi iguales una **cabecera que define globales** (A), **dos métodos en
clase** (B) y **seis globales duplicados entre dos ficheros** (C+D).

### 2.2 Coste por unidad — el número que decide el reparto

`región unión` = unión de los rangos de direcciones que se desplazan al promocionar **sólo**
esa unidad (sin doble contar los 14 símbolos que van juntos). `símbolos` = cuántos símbolos
del ELF caen dentro.

| unidad | símbolos que roba | bytes de símbolo | **región unión** | símbolos desplazados |
|---|---:|---:|---:|---:|
| **zPhysicsBehaviors** | 2 | 640 | **916.004** | 3.094 |
| **zGameplay** | 1 | 120 | **847.152** | 4.716 |
| **zMisc** | 1 | 120 | **493.816** | 2.216 |
| **zPhysics** | 1 | 120 | **382.240** | 1.877 |
| **zEcstasy** | 3 | 12 | **328.484** | 2.372 |
| **zSpeech** | 1 | 120 | **152.496** | 475 |
| **zAnim** | 14 | 56 | **146.384** | 2.294 |
| **zEAXSound** | 2 | 6 | **123.368** | 1.424 |
| **zFe2** | 14 | 56 | **118.104** | 1.269 |
| **zEAXSound2** | 1 | 4 | **16.800** | 610 |
| zAI, zCamera, zEagl4Anim, zFe, zFeOverlay, zGameModes, zLua, zMain, zPlatform, zTrack, zWorld, zWorld2 | 0 | 0 | **0** | 0 |

**Doce de las veintidós unidades del bloque A no tienen ni un cambio de dueño vivo.** Para
ellas M4 no es el frente y hay que buscar en otro sitio.

### 2.3 Los símbolos, uno a uno

| bytes | secc. | símbolo | original | nuestros | gana ahora → gana después |
|---:|---|---|---|---|---|
| 384 | .bss | `_t10ScratchPtr1ZQ215SimpleRigidBody8Volatile.mPointer` | `auto_07_804FEA4C_bss` (COMMON) | + zPhysicsBehaviors | blob → **zPhysicsBehaviors** (−915.728) |
| 256 | .bss | `_t10ScratchPtr1ZQ29RigidBody8Volatile.mPointer` | `auto_07_804F4040_bss` | + zPhysicsBehaviors | blob → **zPhysicsBehaviors** (−915.236) |
| 120 | .text | `clear__…_List_base<WGridManagedDynamicElem,…>` | **zSim** | zGameplay, zMisc, zPhysics, zSim, zWorld2 | zSim → **zGameplay** / zMisc / zPhysics |
| 120 | .text | `clear__…_List_base<SPCHType_1_EventID,…>` | **zSpeech** | zAI, zEAXSound, zEAXSound2, zGameplay, zLua, zSim, zSpeech | zSpeech → **zSim** (hoy) / **zAI** (todo promocionado) |
| 4×14 | .data | `g_bCustomizeManagerHasControl`, `g_bTestCareerCustomization`, `g_pCustomize{Main,Sub,SubTop,Parts,Perf,Decals,Paint,Rims,Hud,HudColor,Spoiler,ShoppingCart}Pkg` | **zFeOverlay** | + zAnim, zFe2 | zFeOverlay → **zAnim** (−146.332) / **zFe2** (−118.052) |
| 4 | .bss | `pTexPrev` | **zPlatform** | + zEcstasy | zPlatform → **zEcstasy** (+313.344) |
| 4 | .bss | `eAnimTextureSlotPool` | **zPlatform** | + zEcstasy | zPlatform → **zEcstasy** (+312.664) |
| 4 | .data | `EnableParticleSystem` | **zMiscSmall** | + zEcstasy | zMiscSmall → **zEcstasy** (−15.136) |
| 4 | .data | `g_MaxSongs` | **zFe2** | + zEAXSound2 | zFe2 → **zEAXSound2** (−16.800) |
| 4 | .data | `_Q43UTL3COMt7Factory3ZRC…Sim::ConnectionData….mHead` | **zSim** | + zEAXSound | zSim → **zEAXSound** (−123.088) |
| 2 | .data | `_Q26Speech7Manager.m_frameindex` | **zSpeech** | + zEAXSound | zSpeech → **zEAXSound** (−123.364) |
| 284 | .text | `reserve__…_STL::vector<EAX_CarState*,…>` | **zEAXSound** | + zAI (y otros) | **latente**: zEAXSound → zAI |
| 284 | .text | `reserve__…_STL::vector<Ui,…>` | **zEAXSound** | + zAI | **latente**: zEAXSound → zAI |
| 176 | .text | `find__H2ZPP11IDisposableZP11IDisposable_…` | **zPhysics** | + zMain | **latente**: zPhysics → zMain |
| 56 | .text | `RaiseToPower__H1i10_i_i` | **zFe** | + zAI | **latente**: zFe → zAI |
| 4 | .rodata | `lbl_803EBE90` | **zGameModes** | + zGameplay | **latente**, y probablemente ruido de extracción |

**Los cinco «latentes»** cuestan cero hoy porque la definición fuerte la sigue dando un
`obj/*.o` sin promocionar (artefacto de extracción: `dtk` marca GLOBAL lo que el compilador
emitía WEAK). **Saltarán en cuanto se promocione la unidad que hoy los ancla**, y en dos de
ellos el nuevo dueño sería `zAI`, que es el objeto 0: irreversible por orden.

---

## 3. Las causas de fuente

### 3.1 La familia B — un método **en clase** instancia la plantilla en cada TU

`src/Speed/Indep/Src/World/WGridManagedDynamicElem.h:19-25`:

```cpp
static void Init()     { fgManagedDynamicElemList.clear(); }
static void Shutdown() { fgManagedDynamicElemList.clear(); }
```

Los dos cuerpos están **dentro de la clase**, o sea son inline, y el uso de `.clear()` está
**dentro de una función** — que es justo la condición de `instantiate_decl`
(`cp/pt.c:9439-9461`, `nested = in_function_p()`): GCC instancia
`_STL::_List_base<WGridManagedDynamicElem,…>::clear` **en el punto del parseo, en toda TU que
incluya la cabecera**. Por eso lo emiten cinco de nuestros objetos y el original sólo uno.

Y el segundo `clear__` de la familia tiene la **misma forma por otra puerta**, la herencia:

* `src/Speed/Indep/Src/World/Common/WGridNode.h:28`
  `struct WGridNodeElemList : public UTL::Std::list<WGridNodeElem, _type_list> { … };`
* `src/Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp:142`
  `struct SPCHEventList : public UTL::Std::list<SPCHType_1_EventID, _type_list>, public AudioMemBase { … };`

Heredar de la lista hace que el destructor implícito de la derivada llame al de la base, y el
destructor implícito **es** una función: misma instanciación en el punto de parseo, misma
propagación por cabecera. `WGridNodeElem` sale gratis por pura suerte (`zAI` es el objeto 0);
`SPCHType_1_EventID` **no**.

### 3.2 La familia A — la cabecera que DEFINE (confirmada, y son 14, no 15)

`src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp:686-698`
define **catorce** globales con inicializador (2 `bool` + 12 `char*`) en vez de declararlas
`extern`. Llega a `zAnim` y a `zFe2` por `uiQRCarSelect.hpp`. Confirmado símbolo a símbolo.

**Busqué más cabeceras así y no las hay.** El barrido por el dato — símbolo con definición
real GLOBAL en **varios** objetos nuestros y en **menos** originales — da **34** símbolos, y
al localizarlos en el árbol **sólo los 14 de `CarCustomize.hpp` viven en una cabecera**. Los
otros 20 son otra cosa, la familia D.

### 3.3 La familia D — el mismo global escrito en **dos ficheros .cpp** de listas distintas

Ésta no la había nombrado nadie y es tan barata de arreglar como la A:

| símbolo | definición que sobra | definición buena (la del original) |
|---|---|---|
| `pTexPrev` | `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp:241` `TextureInfo *pTexPrev;` | `TextureInfoPlat.cpp:90` (zPlatform) |
| `eAnimTextureSlotPool` | `EcstasyE.cpp:168` `SlotPool *eAnimTextureSlotPool;` | `TextureInfoPlat.cpp:7` (zPlatform) |
| `EnableParticleSystem` | `src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp:803` `bool EnableParticleSystem = true;` | el `asm` de `zMiscSmall.cpp:313-317` |
| `g_MaxSongs` | `src/Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.cpp:61` `int g_MaxSongs = -1;` | `FEDatabase.cpp:35` (zFe2) |
| `Speech::Manager::m_frameindex` | `src/Speed/Indep/Src/EAXSound/EaxSoundTypes.cpp:26` | `SpeechManager.cpp:105` (zSpeech) |
| `RoadNoiseVolumes` (36 B) | `CARSFX_Roadnoise.cpp:141` | `EAXVolumes.cpp:3` (zEAXSound) |

`RoadNoiseVolumes` **no** cambia de dueño hoy (zEAXSound idx 8 gana a zEAXSound2 idx 9) pero
es el mismo defecto y son 36 B de `.data` duplicados. Aviso: `SpeechManager.cpp` y
`CARSFX_Roadnoise.cpp` están **modificados en el árbol ahora mismo** por otros agentes; el
duplicado es de `HEAD`, no de esta ronda, pero conviene avisarles.

### 3.4 La familia F — 25 símbolos que parecen un problema y no lo son

`Get__CQ26Attribt7TAttrib1Z<T>Ui` — `Attrib::TAttrib<T>::Get(unsigned)`, 80 B cada uno,
**25 tipos distintos**. **Ningún objeto original los define**: el original no los emite
nunca. Nosotros los emitimos WEAK en 14-22 unidades cada uno y **el enlazador los tira
enteros**, porque nadie los referencia. Comprobado: al promocionar `zAI` aparecen 50 símbolos
nuevos en el ELF y **ninguno** es de esta familia.

Cuesta cero en el DOL, pero **infla nuestros `.o` y envenena la aritmética de `movidos`**
(sección siguiente). Lo mismo el `_M_erase__…_Rb_tree<Hermes::_h_HHANDLER__>` de 104 B.

---

## 4. El contraste con `movidos.py` — y no se acercan

El brief pedía cruzar el censo con la sección «CAMBIO DE DUEÑO» de `movidos.py` en dos o tres
unidades, y avisaba: *si no se acercan, dilo — eso sería el hallazgo*. **No se acercan, y en
`zLua` el error es de infinito a uno.**

| unidad | robo REAL (este censo) | `movidos.py` «CAMBIO DE DUEÑO» |
|---|---|---|
| **zAnim** | `.data` **−56** (14 globales a zFeOverlay) | `.data −64`, `.rodata −400`, `.text −7.604`, `.bss −12` |
| **zPhysics** | `.text` **−120** (un `clear__` a zSim) | `.text −42.228`, `.rodata −1.800`, `.bss +8` |
| **zEcstasy** | `.data` **−4**, `.bss` **−8** | `.data −40`, `.bss −832`, `.rodata −5.688`, `.text −7.492` |
| **zTrack** | **CERO** | `.text −3.536`, `.rodata −136`, `.data +20` |
| **zLua** | **CERO** | **`.text −17.676`** |

**El porqué, y es una identidad, no una conjetura.** `movidos` calcula
`robado = enlazado(sec) − delta(sec)`, o sea *lo que crece la sección enlazada* menos *lo que
crece nuestra aportación*. Esa resta mezcla **tres** cosas y sólo una es cambio de dueño:

1. bulto que le quitamos a otro objeto (M4 de verdad);
2. **código y datos NUESTROS que nadie referencia y el enlazador tira** — `-strip-unused-data`
   funciona a granularidad de **símbolo**, no de sección;
3. relleno de alineación (los ±8 B de zAnim y zPhysics).

En `zLua` el término (1) vale **0** y el (2) vale **17.676**: nuestro `zLua.o` emite 17.676 B
de `.text` de más y el `.text` enlazado sale **idéntico**. Eso ya estaba escrito en la
docstring de `linkdelta.py` («zLua tiene el `.text` del objeto +18.908 B y, una vez enlazado,
el `.text` del ELF sale exactamente igual que el de la base») — sólo que `movidos` lo estaba
reetiquetando como robo. **La afirmación del brief de que «zLua se lleva 17.676 B de `.text`
de otros objetos» queda refutada.**

`zTrack` es la prueba limpia: **cero** cambios de dueño y `movidos` sigue imprimiendo
`.text −3.536`.

**Qué sí sirve de `movidos`.** Los **ESCALONES SIN EXPLICAR**. En `zPhysics` imprime
`.text +120, 1.500 símbolos`: son exactamente los 120 B del `clear__` y el racimo que
arrastra. El escalón es la firma buena; la resta de secciones no.

### Y la columna `dueño` de la r54 tampoco vale

La r54 censó por **salto de dirección > 20 kB** y dio `zFe 47` y `zFe2 49`, «el doble que
nadie». Comprobado símbolo a símbolo:

    React__11MainOptionsPCcUiP8FEObjectUiUi   (zFe, -161.700 B)  -> lo define UN SOLO objeto
    _IHandle__15IGenericMessage               (zFe2, -222.516 B) -> lo define UN SOLO objeto

No pueden cambiar de dueño: **son permutaciones dentro de la unidad (M1)**, con el mismo
tamaño de salto. La columna `dueño` de la r54 mezcla M1 y M4 y **no debe usarse para el
reparto**. El censo por definición da `zFe 0` y `zFe2 14`.

---

## 5. El arreglo, por orden de rentabilidad (descrito, **NO aplicado**)

**1. `WGridManagedDynamicElem.h:19-25` — sacar `Init()` y `Shutdown()` de la clase.**
   **1.723.208 B** de desplazamiento en tres unidades a la vez (zGameplay 847.152 + zMisc
   493.816 + zPhysics 382.240) con **una** edición de cabecera.
   Dueño correcto: **zSim** (es quien lo tiene en el original). Con los cuerpos fuera de la
   clase, sólo los instancian la TU de zSim que usa la lista de verdad y
   `WGridManagedDynamicElem.cpp` — que está en **zWorld2 (idx 31)**, detrás de zSim (27), así
   que zSim sigue ganando. **Hay que medirlo antes de creérselo.**
   **Es atómico**: arreglar sólo zGameplay se lo entrega a zMisc (19), y arreglar zMisc se lo
   entrega a zPhysics (23). Sólo la cabecera los arregla a los tres de golpe. zWorld2 (31) no
   puede robarlo nunca porque va detrás de zSim.

**2. `EcstasyE.cpp:168` y `:241` a `extern`, y `EmitterSystem.cpp:803` a `extern`.**
   **328.484 B** en zEcstasy, tres líneas. Cuidado con `EnableParticleSystem`: el dueño bueno
   es el `asm` a mano de `zMiscSmall.cpp:313-317`, no un `.cpp`.

**3. `CarCustomize.hpp:686-698` a `extern`, con la definición en un `.cpp` de zFeOverlay.**
   **264.488 B** (zAnim + zFe2). El aviso de la r54 sigue en pie: hay que elegir el `.cpp`
   **y el punto** que reproduzca el orden de `.data` de zFeOverlay, que hoy está a delta 0.
   **Paquete atómico de tres unidades.**

**4. `SpeechManager.hpp:142` — que `SPCHEventList` deje de heredar de `UTL::Std::list`**
   (composición en vez de herencia, o el destructor declarado y definido en
   `SpeechManager.cpp`). **152.496 B** en zSpeech. **Urgente por otra razón**: en el mundo
   todo-promocionado el dueño pasa a ser **zAI**, que es el objeto 0 y no se le puede quitar
   por orden de enlace.

**5. `EaxSoundTypes.cpp:26` — borrar la segunda definición de `Speech::Manager::m_frameindex`,
   y la instanciación de `UTL::COM::Factory<Sim::ConnectionData…>` que zEAXSound duplica.**
   **246.452 B** en zEAXSound, y una de las dos es una línea.

**6. `SFXObj_Pathfinder.cpp:61` a `extern int g_MaxSongs;`** — 16.800 B en zEAXSound2.

**7. Latentes, sin coste hoy pero con fecha de caducidad**: los dos `reserve__…vector<…>`
   (284 B) que zAI le va a quitar a zEAXSound, `find__…IDisposable…` (176 B) que zMain le va
   a quitar a zPhysics, y `RaiseToPower__H1i10_i_i` (56 B) que zAI le va a quitar a zFe.
   **Conviene arreglarlos antes de promocionar zAI**, porque después no hay orden de enlace
   que los recupere.

**8. `CARSFX_Roadnoise.cpp:141` a `extern`** — 36 B duplicados, coste 0 hoy, higiene.

---

## 6. Sorpresas

1. **`SHN_COMMON` invalida el predicado ingenuo.** 20 símbolos y 4,7 MB de radio inventados,
   incluido uno de **42.000 B**. Refutado con un enlace.
2. **GLOBAL gana a WEAK aunque vaya después en el orden de enlace**, y como `dtk` marca
   GLOBAL lo que el compilador emitía WEAK, los `obj/*.o` sin promocionar **enmascaran**
   cambios de dueño que aparecerán solos según avance el bloque A. Son 5, y en dos de ellos
   el ladrón sería `zAI`, el objeto 0.
3. **`-strip-unused-data` es a granularidad de símbolo.** Por eso 26 de los 50 cambios de
   dueño cuestan literalmente cero, y por eso el `.text` total no se mueve aunque nuestro
   objeto emita 17.676 B de más.
4. **La sección «CAMBIO DE DUEÑO» de `movidos.py` no mide el cambio de dueño.** Mide
   `enlazado − delta`, que es sobre todo **nuestro código muerto estripado**. En `zTrack` y
   `zLua` el robo real es **cero** y `movidos` imprime −3.536 y −17.676.
5. **La columna `dueño` de la r54 mezcla M1 con M4.** `zFe 47` y `zFe2 49` no eran cambios de
   dueño: `React__11MainOptions…` y `_IHandle__15IGenericMessage` los define un solo objeto.
6. **La familia de plantillas más numerosa —25 `Attrib::TAttrib<T>::Get`— no existe en el
   original en absoluto** y no cuesta nada. La sospecha «esto son plantillas de contenedor»
   es cierta en número y falsa en peso.
7. **`_IHandle__15IGenericMessage` mide 12 B en `obj/zFe2.o` y 0 B en el nuestro.** No es
   cambio de dueño, pero es un símbolo sin tamaño y no lo estaba mirando nadie.

---

## 7. Método, y lo que este censo NO cubre

Seis enlaces de comprobación (base + zAnim + zPhysics + zEcstasy + zTrack + zLua +
zPhysicsBehaviors + zAI), todos de sólo lectura sobre el árbol. Ninguna fuente tocada,
ningún `.o` reconstruido, ningún commit.

Huecos honestos:

* **Sólo mide promociones de UNA unidad** contra la lista actual, más el mundo
  todo-promocionado. Un símbolo que sólo cambie de dueño al promocionar **dos** unidades a la
  vez no sale.
* Los radios son **estimados** (región del objeto en el ELF base + desplazamiento dentro de
  nuestro `.o`); contra los dos que enlacé de verdad el error fue de **32 B sobre 915.760**.
  Los dos de `.sbss` no tienen radio porque nuestra unidad no gana ningún otro símbolo de esa
  sección; el techo es el tamaño de `.sbss`, **1.656 B**.
* El árbol estaba **en vuelo**: `src/*.o` de trece agentes. Las cifras son la instantánea del
  9-sep ~23:00 y por eso los radios de la r54 y los míos difieren en un 1 %.
* `zGameModes`, que la r54 dejó como enigma (317.834 B de DOL distinto con `dueño 0`),
  **sigue siendo un enigma**: cero cambios de dueño confirmados por este censo también.
