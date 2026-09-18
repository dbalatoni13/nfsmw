# Ronda 34 — `ord3`: terminar el ORDEN del `.text` (zSim, zMisc, zFEng)

Encargo: el §4 del brief de la r34. Tres SourceLists al 100 %, con el `.text` del
tamaño exacto, que no promocionan porque sus funciones **están en otro orden**.

**Resultado: 86 de las 107 funciones descolocadas, colocadas. `zMisc` y `zFEng`
quedan con el ORDEN PERFECTO.** Veintiuna ediciones de `src/` (18 ficheros de codigo mas las 3
SourceLists), nueve de ellas en cabecera compartida y **las nueve con guarda**, así que su radio de explosión es
exactamente CERO. **Sin perder un byte**: `measure.py --cmp` sobre las 607
unidades da `+0 B, +0 funciones, 0 unidades cambian`.

| unidad | descolocadas antes | después | saltos | `trypromo` antes | después |
|---|---|---|---|---|---|
| `zMisc` | 23 | **0 — PERFECTO** | 10 → **0** | DOL ROTO `a867f6af1d52` | DOL ROTO `1c0f67ba4e54` |
| `zFEng` | 47 | **0 — PERFECTO** | 23 → **0** | DOL ROTO `1a7afc9c5719` | DOL ROTO `88459b3071db` |
| `zSim` | 37 | **21** | 16 → **7** | DOL ROTO `76be2455b83b` | DOL ROTO `3318b17021ad` |
| `zFoundation` (control, no tocada) | 0 | 0 | 0 | DOL ROTO **`bfa851ccca08`** | DOL ROTO **`bfa851ccca08`** |

Ninguna promociona: a las tres les falta **dato**, no orden (§6).

---

## 0. Las cuatro frases

1. **Un `~X` cuyo fichero:línea de `debug_lines` es un `.cpp` mientras la clase
   vive en una cabecera es un DESTRUCTOR IMPLÍCITO**, y GCC 2.9 lo emite **por
   delante** de todas las inline declaradas de esa clase. Si nosotros lo
   declaramos, sale en orden de declaración. Probado con `cc1plus` (§2) y valió
   **16 funciones en zSim**, además de explicar tres racimos más.
2. **La veda del §5.1 de la r33 era falsa: el orden de emisión de las vtables NO
   decide dónde van las funciones virtuales.** El caso mínimo lo mide: las
   vtables salen en bloque, en orden inverso de definición, **antes** que todas
   las funciones; y las funciones salen en su propio orden. Los cuatro racimos
   de `zMisc` que la r33 atribuyó a vtables eran orden de parseo y destructores
   implícitos, y la unidad ha cerrado **sin tocar una sola vtable**.
3. **La causa más rentable de la ronda son las cabeceras que incluyen de más.**
   `ResourceLoader.hpp` y `bFile.hpp` incluyen `driver.h` **sin usarlo**, y sólo
   por eso `RealFile::DeviceDriver` se parseaba en el fichero 3 de zMisc en vez
   de en el 17: **13 funciones de zMisc en dos `#define`**. Lo mismo en zFEng con
   `FEGameInterface.h`→`FECodeListBox.h` y `FEPackage.h`→`FEGroup.h`.
4. **`zFEng` NO es una regresión de esta sesión**: lleva con 47/23 desde la
   ventana r25, la que la cerró al 100 %. El barrido de la r32 simplemente no la
   listó (§4).

---

## 1. El protocolo, que ha vuelto a funcionar tres veces

El del §4 del brief, sin cambios: sacar del objetivo la secuencia del bloque
diferido con `fichero:línea` (`c33ord2_files.py`, que lee `debug_lines.txt`),
sacar la nuestra del `.ii` (`c33ord2_iiorder.py` / `c33ord2_incltree.py`), y
cotejar. Lo que la ronda añade es **cómo se lee la columna `fichero:línea`**:

| lo que se ve en `debug_lines` | lo que significa |
|---|---|
| `~X` en la **cabecera** donde está la clase | destructor declarado; sale en orden de declaración |
| `~X` en un **`.cpp`**, con la clase en una cabecera | **destructor implícito**; sale el PRIMERO de la clase, y la línea es su punto de USO |
| un método en un `.cpp` con la clase en una cabecera | el original lo define **fuera de clase con `inline` en ese `.cpp`**: su posición es la del `.cpp`, no la de la clase |
| dos métodos con **la misma línea** | vienen de una **macro** (`DECLARE_WORLDPACKET`, `DECLARE_SIMPACKET`) |

Y el modelo se confirma otra vez: **dentro de cada pasada de `finish_file` el
bloque diferido va en orden de parseo de la definición de la clase**, y las
plantillas en orden de instanciación. En `zMisc` la frontera entre la pasada 1 y
la 2 cae en el mismo índice (439) en el objetivo y en nosotros; sólo estaba mal
el orden **dentro** de cada pasada.

## 2. El caso mínimo que el brief pedía — y su resultado es un NEGATIVO

`c34ord3_lab2.cpp`, 15 líneas, `cc1plus -O1 -S`, un segundo:

```cpp
struct Base { virtual ~Base() {} virtual int v() { return 0; } };
struct D1 : Base { virtual int g(){return 1;} virtual int h(){return 2;} virtual ~D1() {} };
struct D2 : Base { virtual int g(){return 3;} virtual int h(){return 4;} };
D1 a; D2 b;
```

Salida, en orden:

    _vt.2D2  _vt.2D1  _vt.4Base | _._4Base v__4Base | g__2D1 h__2D1 _._2D1 | _._2D2 g__2D2 h__2D2

Tres cosas medidas de una vez:

- **Las vtables salen en bloque y en orden INVERSO de definición** (confirma la
  r32) **y por delante de todas las funciones**. No reparten funciones: la
  posición de una virtual no la decide su vtable. **La veda del §5.1 de la r33
  queda cerrada en negativo.**
- `D1`, con **destructor declarado** el último: sale `g h ~` — orden de
  declaración.
- `D2`, con **destructor implícito**: sale `~ g h` — **el destructor primero.**

Y con `-gdwarf+` (`c34ord3_lab4.cpp`, la clase en `cab.h` y los objetos en
`usa.cpp:200`) sale además la mitad que hace de diagnóstico:

| símbolo | línea que le pone GCC |
|---|---|
| `_._2D1` (declarado en `cab.h:8`) | **cab.h:8** |
| `_._2D2` (implícito) | **usa.cpp:201** — el punto de uso |

Eso es exactamente lo que `debug_lines` enseña en
`_._Q29WorldConn15Pkt_Effect_Send` → `SimEffect.cpp:33` y en
`_._10FESlotPool` → `FESlotPool.cpp:117`. **La regla de lectura del §1 está
probada con el compilador, no inferida.**

Aviso del predecesor, comprobado: con clases normales el caso **sí** reproduce
(no hacía falta plantilla); lo que no reproduce es si las inline se pliegan
enteras — hay que hacerlas `virtual` o tomarles la dirección para que se emitan.

## 3. Los ensayos

### zMisc: 23 → 0 (ORDEN PERFECTO)

Todo el desorden eran **cuatro grupos**, y los cuatro se explican con el orden de
parseo. La secuencia del objetivo (índices 422-450) es, fichero a fichero:
`Hermes.h` · `vechashmap64.h` · `GameFlow.cpp:1978` · `GameFlow.cpp:3521` ·
`Rumble.cpp:741` · `QueuedFile.cpp:123` · `driver.h` · `bFile.cpp` **‖ pasada 2 ‖**
`AttribAsset.cpp` · `AttribAlloc.cpp` · `driver.h`.

- **m3 — `ResourceLoader.hpp` incluye `driver.h` y no lo usa** (`grep`: cero
  menciones de `RealFile`/`DeviceDriver` en el fichero). Sólo necesita el
  `typedef void ASYNCFILE_CALLBACK(intptr_t)` de `driver.h:19`, que va en el
  `#else` de la guarda. **23 → 23**, pero sin ella m5 no vale nada (medido: m5
  sola da 23).
- **m5 — `bFile.hpp` incluye `driver.h` y no lo usa**, y `bFile.cpp:3` ya lo
  incluye por su cuenta. Con m3, **23 → 10 y 10 → 6 saltos: 13 funciones**.
  Coloca `GetName`/`GetOptimalReadSize` en 432-433 y **la pasada 2 entera**
  (439-449) a delta 0.
- **m4 — `Gameplay/GManager.h` declara `bFile *` y le basta la
  declaración adelantada**: probada, da 23 → 23 con m3 y **es innecesaria con
  m5**. REVERTIDA (`git diff` vacío) para dejar el diff mínimo.
- **m6/m7/m8 — tres cuerpos que el original tiene en el `.cpp`.** `debug_lines`
  los sitúa en `Rumble.cpp:741` (`tShaker::Reset`), `GameFlow.cpp:1978`
  (`RegionLoader::LoadHandler(intptr_t)`) y `QueuedFile.cpp:123/131/339/349`
  (`QueuedFile::SortByPriority`, `QueuedFile::ReadDoneCallback(void*)`,
  `QueuedFileBundle::operator delete`, `QueuedFileBundle::ReadCallbackBridge`);
  nosotros los teníamos dentro de la clase, o sea parseados en el fichero 3.
  Movidos a su `.cpp` con `inline` (mecanismo o1/o2 de la r32) y bajo guarda:
  **10 → 0, ORDEN PERFECTO.**

Comprobado que los seis símbolos los emite **sólo `zMisc.o`** (`c34ord3_whohas.py`
sobre los 607 objetos), así que el movimiento no puede dejar a nadie sin
definición; aun así van con guarda.

### zFEng: 47 → 0 (ORDEN PERFECTO)

Diez cambios; **ocho son `.cpp` de una sola SourceList (radio cero sin guarda)** y
dos son cabecera con guarda.

| ensayo | qué | efecto |
|---|---|---|
| **f1** | `FECodeListBox.cpp` incluye `FEString.h`: el objetivo emite `~FEString`/`Clone` (307-308) pegados a `FEListBox::GetPCellData` (306), no en el fichero 21 | 47 → 42 |
| **f2** | `FEngine.cpp` **no** incluye `FEColoredImage.h` (`grep`: la única mención era el include). Por ahí le llegaba `FEImage.h` al fichero 16 | — |
| **f3** | `FEObject.cpp` incluye `FEMultiImage.h` **antes** de `FESlotPool.h`: el objetivo emite `feimage.h`(317-318) y `FEMultiImage.h`(319-320) **juntas** y delante de `FESlotPool.h`(321), o sea que `FEImage.h` le llega por `FEMultiImage.h` | f2+f3: 42 → **22** |
| **f4** | `FEMath.h` el primero de `FEButtonMap.cpp`: `__ml__12FEQuaternion` (296) va delante de los cuatro de `FEList.h` | — |
| **f5** | las dos `FEInterpNone` de `FEKeyInterpNone.cpp`, cambiadas de orden (el objetivo: la de `FEScript*` primero) | — |
| **f6** | `FindReferencedObject` al final de `FEPackageReader.cpp`, detrás de `ReadMessageTargetListChunk` | — |
| **f7** | `FEAnimImage.h` antes que `FESimpleImage.h` en `FEPackageReader.cpp` | f4-f7: 22 → **8** |
| **f8** | `FEButtonMap.cpp` incluye `FEPackage.h` antes de `FEGameInterface.h`: ahí es donde el objetivo parsea `PackageInitStateCB` (destructor implícito, `FEngine.cpp:200`) | 8 → 6 |
| **f9** | `FEGameInterface.h` usa `FECodeListBox` **sólo como puntero** (único uso: el parámetro de `SetCellData`) → declaración adelantada, con guarda | 6 → 6, 5 → 3 saltos |
| **f10** | `FEPackage.h` usa `FEGroup` **sólo como puntero** (`UpdateGroup`, `ForAllChildren`) → declaración adelantada, con guarda | **6 → 0, ORDEN PERFECTO** |

Que las 344 funciones queden **todas** a delta 0 es la prueba de que la cadena
f1-f10 reconstruye el grafo de `#include` del original, no una casualidad.

**f8' — REVERTIDA:** `FEPackage.h` tampoco puede soltar `FEGameInterface.h`,
porque usa `FEResourceRequest`, que es un `typedef struct {…}` **anónimo** de
`FEGameInterface.h:26` y no se puede declarar adelantado.

### zSim: 37 → 21

- **s9 — el racimo A NO era un problema de ranuras de vtable (8 funciones).** La
  r33 lo vetó por eso; `debug_lines` dice otra cosa: `_._Pkt_Effect_Send` está en
  **`SimEffect.cpp:33`** y `_._Pkt_Effect_Open` en **`SimEffect.cpp:96`**,
  mientras `ConnectionClass`/`Size`/`Type` están en `WorldConn.h:86` y `:123`
  (los tres en la misma línea: son la macro). O sea que el original **no declara
  esos destructores**. Quitando los dos `~Pkt_Effect_*() override {}` de
  `WorldConn.h` (con guarda): **37 → 29, 16 → 11 saltos.**
- **s10 — lo mismo con `UTL::Collections::_Storage` (8 funciones).** Quitando
  `~_Storage() override {}` de `UCollections.h` (con guarda), los tres
  `_._Storage` pasan a la **cabeza** del bloque de plantillas (370-372) como en el
  objetivo, en vez de emparejados con `_._FixedVector`: **29 → 21, 11 → 7
  saltos**, y los índices 370-377 y 381-402 quedan **todos a delta 0**.

## 4. `zFEng`: desde cuándo y por qué — NO es de esta sesión

Medido, no razonado. Tres compilaciones en `git worktree` desechables:

| árbol | `zFEng` |
|---|---|
| HEAD de hoy (`c76cf6ef`) | 47 descolocadas, 23 saltos |
| **`789ddbc4`** (el commit de `r32/ord`, el barrido que no la listó) | **47 / 23** |
| **`ab548730`** (ventana r25: «zFEng CERRADA 65,28 → 100 %») | **47 / 23** |

Y el `asm()` de `.rodata` que `f578866c` (r32/pre) le metió a `zFEng.cpp`
**tampoco es la causa**: compilada con ese bloque retirado da **47 / 23** otra
vez. El `.text` de `splits.txt` para `zFEng` no se toca desde 2025 (sólo cambió
su `.data`, en `1aa41669`), así que el objeto extraído tampoco ha cambiado.

**Conclusión: `zFEng` lleva con las 47 desde la ronda 25, que es cuando alcanzó
el 100 % y empezó a ser comparable. El barrido de la r32 no la listó** — su tabla
tiene 26 filas más «y siete más», y ni la fila ni la cuenta cuadran con las 32
que declara. No hay ningún cambio de esta sesión detrás.

## 5. Vedas

### 5.1 CERRADA la veda del §5.1 de la r33 (el orden de las vtables)

No hacía falta y además no era el mecanismo: §2. Las vtables salen en bloque,
antes que las funciones, en orden inverso de definición. **Los cuatro racimos de
`zMisc` y el racimo A de `zSim` que dependían de esa veda están cerrados sin
tocar una vtable.**

### 5.2 `zSim`: lo que queda son TRES funciones, no 21

Con s9+s10, el bloque 370-402 sólo tiene un error: los tres
`_._Q23UTLt11FixedVector3Z…` salen en 378-380 y el objetivo los pone en
**394, 396 y 398**, alternados detrás de cada `_._Vector`. Las otras 18
descolocadas son **deslizamiento** de esas tres. El objetivo:

    _Storage×3 · SType×2 · OnGrowRequest×3 · [Alloc/Free/GGS/GMC]×3 ·
    GetGrowSize__Vector×3 · (~Vector,~FixedVector)×3 · GetMaxCapacity__Vector×3

y el nuestro es idéntico salvo que los tres `~FixedVector` están hoisted a 378-380
(los arrastra el `~_Storage` sintetizado, que llama al destructor de su base).
En el objetivo `~FixedVector` viene con `~Vector`, o sea **una pasada más tarde**.

Barrido y REVERTIDOS:

- **s11**: mover la **declaración** de `~FixedVector` al final de la clase
  `UTL::FixedVector` (en el original está la primera: `UTLVector.h:704` frente a
  `748`-`766` de las otras cuatro) → **21 descolocadas, exactamente las mismas**,
  mismos 7 saltos. La posición de la declaración no mueve una instanciación.
- **s12**: hacer implícito también el **constructor** `_Storage() {}` → **21, las
  mismas**. La sinterización del constructor no cambia el orden del destructor.

`~FixedVector` **no se puede hacer implícito**: tiene cuerpo
(`Vector<T,Alignment>::clear()`), y el objetivo también lo emite con sus 180 B.

### 5.3 `FEPackage.h` no puede soltar `FEGameInterface.h`

`FEResourceRequest` es un `typedef struct {…}` anónimo: no admite declaración
adelantada. Probado, no compila (`FEPackage.h:120: syntax error before '*'`).

### 5.4 Lo que NO he probado

1. Los tres `~FixedVector` de `zSim` (§5.2). Es orden de instanciación de
   plantilla y hay que buscarlo en quién exige `~Vector<T>` antes que
   `~FixedVector<T,N>` en el objetivo.
2. **Las 28 unidades restantes del barrido.** La regla del §1 (destructor
   implícito) y la de las cabeceras que incluyen de más son **directamente
   aplicables** y no se han probado en ninguna: `zAttribSys` (47/18),
   `zAnim` (27/7), `ppc2D2` (15/7), `dvd_device` (15/1), `filesys` (58/1),
   `gc_interface` (10/8), `zTrack` (6/3), `steering` (5/1), `pathnode` (4/1),
   `zBWare` (2/3), `madidct`/`vm`/`lbmpeg` (1 cada una).

## 6. Los datos que les faltan, para el agente de datos

`promodist.py --sl` con el árbol al final de mi turno:

| unidad | `.rodata` | `.data` | `.bss` |
|---|---|---|---|
| `zSim` | **−16** | −240 | +168 |
| `zMisc` | **+4.576** | +28 | +288 |
| `zFEng` | **−648** | −76 | 0 |
| `zFoundation` | −1.144 | −200 | 0 |

Ninguno es mío: el A/B del §7.2 demuestra que mis veintiuna ediciones dejan las cinco
secciones de las tres unidades **byte a byte del mismo tamaño**. A `zMisc` le
**sobran** 4.576 B de `.rodata` (otro agente escribe pool a mano ahí).

## 7. Verificación

### 7.1 Lo estándar

- `build_direct.py` de las 33 SourceLists: **33 ok, 0 fallidas**, repetido tres
  veces (dos fallos intermedios de `Could not open output file` son otro proceso
  escribiendo el mismo `.o`, no errores de compilación; a la segunda pasan).
- `measure.py zSim zMisc zMiscSmall zFEng zFoundation`: **282.636/282.636 B,
  100 %, 1.391 funciones**, igual que al empezar.
- `measure.py` de **las 607 unidades**, base y medida con el árbol construido
  entero las dos veces: **3.866.216/3.946.204 B, 97,9730 %, 18.336 funciones**
  antes y después; `--cmp` = **+0 B, +0 funciones, 0 unidades cambian**.
- `audit.py` en `zSim`, `zMisc`, `zFEng` y `zFoundation`: **0 FALLA** en las cuatro.
- `keepchk.py`: **759 ok, 21 RANCIAS, 0 sin objeto** — exactamente lo mismo que
  al cerrar la r33; ninguna frontera movida.
- `config/GOWE69/*` y `configure.py`: **`git status` vacío**. Ninguna propuesta de
  configuración.
- Los `#if defined(__ANDROID__)`: `src/types.h`, `bMath.hpp` y `UVectorMath.hpp`
  salen **sin modificar** en `git status`. Intactos.
- **Control de enlace: `zFoundation`, que no he tocado, da el MISMO DOL
  (`bfa851ccca08`) al empezar y al terminar.** Mis ediciones no cambian ni un
  byte del DOL de las unidades ajenas.

### 7.2 A/B por secciones: mis cambios son SÓLO orden

`c33ord2_secs.py` sobre cada variante contra la base, seguidas:

| unidad | `.text` | `.rodata` | `.data` | `.bss` | `.ctors` |
|---|---|---|---|---|---|
| `zMisc` con / sin | 86.368 / 86.368 | 12.440 / 12.440 | 2.192 / 2.192 | 2.560 / 2.560 | 4 / 4 |
| `zSim` con / sin | 114.492 / 114.492 | 7.864 / 7.864 | 108 / 108 | 8.180 / 8.180 | 4 / 4 |
| `zFEng` con / sin | 73.756 / 73.756 | 1.984 / 1.984 | 308 / 308 | 332 / 332 | 4 / 4 |

### 7.3 El A/B global de las nueve cabeceras compartidas

No basta el `grep`: he hecho la medida. Poniendo **las nueve cabeceras en su
versión de base**, construyendo las 33 SourceLists y midiendo; y volviendo a
poner las mías, reconstruyendo y midiendo — **seguidas**:

    base : 607 unidades  3866216/3946204 B  97,9730 %  18336 funciones
    final: 607 unidades  3866216/3946204 B  97,9730 %  18336 funciones
    --cmp: +0 B, +0 funciones, 0 unidades cambian

(La base se tomó **sin restaurar las SourceLists de nadie**, sólo mis nueve
cabeceras: `zMisc.cpp` la está editando `pre2` y restaurar una copia mía de hace
horas le habría borrado el trabajo — es la trampa del §7.5 de la r33 al revés.)

### 7.4 Radio de explosión de las nueve cabeceras: CERO, por construcción

Las nueve sólo cambian de tokens si está definida su macro, y `grep -rn` sobre
todo `src/` da **exactamente nueve `#define`, todos en una SourceList**:

    zMisc.cpp : MISC_RESOURCELOADER_HPP_NO_DRIVER, MISC_BFILE_HPP_NO_DRIVER,
                MISC_GAMEFLOW_HPP_LATE_INLINES, MISC_RUMBLE_HPP_LATE_INLINES,
                MISC_QUEUEDFILE_HPP_LATE_INLINES
    zSim.cpp  : WORLDCONN_H_IMPLICIT_PKT_EFFECT_DTORS,
                UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR
    zFEng.cpp : FENG_FEGAMEINTERFACE_H_FWD_CLB, FENG_FEPACKAGE_H_FWD_GROUP

Para cualquier otra unidad el preprocesado es idéntico salvo comentarios (que el
preprocesador borra). Los `.cpp` que toco sin guarda —seis de `FEng/` y tres de
`Misc/`— los usa **una sola SourceList cada uno** (`grep` sobre
`src/Speed/*/SourceLists/`), y en los tres de `Misc/` lo añadido va dentro del
`#ifdef`.

### 7.5 El barrido completo, contra el de esta mañana

`textorder.py --todas`, tomado al empezar y al terminar:

    antes:  33 de 526 unidades desordenadas, 9.880 funciones
    después:31 de 526 unidades desordenadas, 9.794 funciones   (-86)

Fila a fila **todas las demás salen con la cifra exacta de antes** — `zFe2` 1159,
`zFe` 921, `zPhysicsBehaviors` 817, `zAI` 800, `zEAXSound` 730, `zPhysics` 701,
`zSpeech` 701, `zGameplay` 694, `zEAXSound2` 513, `zMain` 501, `zWorld` 454,
`zCamera` 429, `zEcstasy` 315, `zEagl4Anim` 273, `zWorld2` 272, `zPlatform` 135,
`filesys` 58, `zAttribSys` 47, `zAnim` 27, `ppc2D2` 15, `dvd_device` 15,
`gc_interface` 10, `zTrack` 6, `steering` 5, `pathnode` 4, `zBWare` 2,
`madidct`/`vm`/`lbmpeg` 1. **`zMisc` y `zFEng` desaparecen** y `zSim` baja de 37 a
21.

La única fila ajena que se mueve es **`zLua`, cuyo `total` pasa de 538 a 537**
(sus 166 descolocadas y sus 79 saltos no cambian): **no es mía** — el A/B del
§7.3, que reconstruye zLua las dos veces, da 0 unidades que cambian.

## 8. Ficheros de `src/` que dejo modificados

| fichero | ensayo | qué | guarda |
|---|---|---|---|
| `Src/Misc/ResourceLoader.hpp` | m3 | `#include driver.h` bajo guarda; en el `#else`, el `typedef ASYNCFILE_CALLBACK` | `MISC_RESOURCELOADER_HPP_NO_DRIVER` |
| `Src/Misc/bFile.hpp` | m5 | `#include driver.h` bajo guarda | `MISC_BFILE_HPP_NO_DRIVER` |
| `Src/Misc/QueuedFile.hpp` / `.cpp` | m6 | cuatro cuerpos de la clase al `.cpp`, con `inline` | `MISC_QUEUEDFILE_HPP_LATE_INLINES` |
| `Src/Misc/Rumble.hpp` / `.cpp` | m7 | `tShaker::Reset` al `.cpp`, con `inline` | `MISC_RUMBLE_HPP_LATE_INLINES` |
| `Src/Misc/GameFlow.hpp` / `.cpp` | m8 | `RegionLoader::LoadHandler(intptr_t)` al `.cpp`, con `inline` | `MISC_GAMEFLOW_HPP_LATE_INLINES` |
| `Src/World/WorldConn.h` | s9 | fuera los dos `~Pkt_Effect_*() override {}` | `WORLDCONN_H_IMPLICIT_PKT_EFFECT_DTORS` |
| `Libs/Support/Utility/UCollections.h` | s10 | fuera `~_Storage() override {}` | `UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR` |
| `Src/FEng/FEGameInterface.h` | f9 | `FECodeListBox` adelantado en vez de `#include` | `FENG_FEGAMEINTERFACE_H_FWD_CLB` |
| `Src/FEng/FEPackage.h` | f10 | `FEGroup` adelantado en vez de `#include` | `FENG_FEPACKAGE_H_FWD_GROUP` |
| `Src/FEng/FECodeListBox.cpp` | f1 | `#include FEString.h` | — (sólo zFEng) |
| `Src/FEng/FEngine.cpp` | f2 | fuera `#include FEColoredImage.h` (no la usa) | — |
| `Src/FEng/FEObject.cpp` | f3 | `#include FEMultiImage.h` antes de `FESlotPool.h` | — |
| `Src/FEng/FEButtonMap.cpp` | f4, f8 | `FEMath.h` el primero; `FEPackage.h` antes de `FEGameInterface.h` | — |
| `Src/FEng/FEKeyInterpNone.cpp` | f5 | las dos `FEInterpNone` cambiadas de orden | — |
| `Src/FEng/FEPackageReader.cpp` | f6, f7 | `FindReferencedObject` al final; `FEAnimImage.h` antes que `FESimpleImage.h` | — |
| `SourceLists/zMisc.cpp`, `zSim.cpp`, `zFEng.cpp` | — | los nueve `#define`, antes del primer `#include` | — |

Cada edición lleva su comentario en el fuente diciendo **que es por POSICIÓN** y,
en los cinco sitios donde el orden alfabético de `#include` era la causa, un aviso
explícito de **no volver a ordenarlos**.

## 9. Guiones del scratchpad (`c34ord3_`)

| guion | qué hace |
|---|---|
| `c34ord3_cc.py <unidad> [salida.o] [--src=otro.cpp] [--E]` | compila **una** unidad con los cflags exactos de ninja, a la salida que quieras y desde el `.cpp` que quieras. Es lo que permite el A/B de SourceList **sin tocar el árbol** |
| `c34ord3_ccwt.py <worktree> <unidad> <salida.o>` | igual pero **desde otro árbol** (un `git worktree` de un commit viejo): es la arqueología del §4 |
| `c34ord3_to.py <unidad> <nuestro.o> [--ops]` | `textorder.py` contra un objeto nuestro arbitrario |
| `c34ord3_idx.py <unidad> <nuestro.o> [desde] [--solo-nuestro]` | las dos secuencias por índice contra un objeto arbitrario |
| `c34ord3_whohas.py <subcadena…>` | **qué objetos nuestros definen un símbolo**: dice si mover un cuerpo a un `.cpp` puede dejar a alguien sin definición |
| `c34ord3_m678.py`, `c34ord3_f4567.py` | los parches de m6/m7/m8 y f4-f7, en binario (finales de línea mixtos) |
| `c34ord3_lab2.cpp`, `c34ord3_lab4.cpp` | **el caso mínimo del §2**, un segundo cada uno |
| `c34ord3_bak/`, `c34ord3_mios/` | copias antes/después de los 18 ficheros tocados |

Reutilizados de la r33: `c33ord2_files.py`, `c33ord2_idx.py`, `c33ord2_iiorder.py`,
`c33ord2_incltree.py`, `c33ord2_secs.py`.
