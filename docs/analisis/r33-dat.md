# Ronda 33 — `dat`: el `.rodata` y el `.data` que faltan en zFe, zSpeech y zPhysics

Encargo: las tres SourceLists que la r32 dejó **enlazando** (499.212 B) y que dan
`DOL ROTO` sólo por dato.

**Ninguna llega a `DOL OK`** — lo digo lo primero. Pero el déficit **baja de
6.912 B a 4.272 B con lo que dejo puesto en `src/`, y a 1.536 B con la propuesta
de `keep.lst` del §3**, que está medida y no resucita ni un byte de código.

**El `.data` de las tres queda en delta 0** (estaba en −128 / −64 / −160).

---

## 0. Las cinco frases

1. **El `.data` de las tres cierra**: `zFe` −128 → **0**, `zSpeech` −64 → **0**,
   `zPhysics` −160 → **0**. Son 188 B de dato muerto escritos con el nombre que
   ya tiene `keep.lst`, más un array nuestro de 16 B de más (§4).
2. **La cabecera de bWare/STL de 92 B faltaba también en estas tres** (prefijo
   común de `.rodata` **0 B** en las tres). Puesta: **+96 / +96 / +64 B** de DOL,
   exactamente el patrón de la r32 (§2).
3. **El daño de `-strip-unused-data`, medido símbolo a símbolo contra el ELF
   enlazado** (no con un modelo): **2.624 B en `zFe`, 1.280 en `zSpeech`, 2.856
   en `zPhysics`**. Es el **77 %** del déficit de `.rodata` de las tres (§1).
4. **Se recupera entero con `keep.lst`, y sin efecto lateral**: forzando sólo los
   símbolos MUERTOS de `.rodata` que **no llevan reubicaciones dentro**, el DOL
   pasa a **−640 / −448 / −448** con **`.text` intacto en las tres** (§3). El
   `keep.lst` a lo bruto de la r32 sí resucitaba código; éste no puede.
5. **Lo que queda no son «funciones muertas» sino cabeceras generadas de
   AttribSys y una plantilla compartida**: las cadenas que faltan son
   `Attrib::Gen::<clase>` (los `USE_ATTRIB_ALLOC` de
   `src/Speed/Indep/Src/Generated/AttribSys/Classes/*.h`) y un racimo
   `WorldBodyConn`/`Pkt_Body_*`/`World_UpdateBody` que falta **en las tres a la
   vez** (§5). Es un frente distinto del que apuntaba `r32-pre`.

### Estado por unidad

| unidad | ANTES (r32) | AHORA (árbol) | + propuesta `keep.lst` |
|---|---|---|---|
| `zFe` | `.rodata` −3.296 · `.data` −128 · **−3.424** · `1073fe5a1273` | −3.200 · **0** · **−3.200** · `95ca0f094541` | **−640** · 0 · **−640** · `0d3caccf1186` |
| `zSpeech` | −1.632 · −64 · **−1.696** · `050e84349fc2` | −1.536 · **0** · **−1.536** · `09a88fd16457` | **−448** · 0 · **−448** · `ec5db50cad08` |
| `zPhysics` | −1.632 · −160 · **−1.792** · `7867d32cb7d5` | −1.536 · **0** · **−1.536** · `338e524270ae` | **−448** · 0 · **−448** · `014b8ccf6348` |
| **suma** | **−6.912** | **−6.272** (−4.272 sin contar `.data`, ya a 0) | **−1.536** |

Las tres siguen **`DOL ROTO`**. El encargo reproducía al byte: los tres sha1 de
partida son los del brief.

**Aviso sobre los sha1 de esta tabla**: al cerrar la ronda, `zFe` daba
`49c21ac7480b` en vez de `95ca0f094541` **con los mismos deltas de sección al
byte**. La causa está medida: `build.ninja` **cambió de md5 entre dos órdenes
seguidas mías** porque otro agente lo regeneraba al medir. **El sha1 de una
promoción sólo vale contra un `build.ninja` dado; la medida estable es el delta
por sección.**

---

## 1. Cuánto del déficit es estripado — medido, no modelado

`r32-pre` dejó la regla (`-strip-unused-data` le quita a un símbolo de datos
muerto `size & ~7` y deja los `size % 8` finales) y una estimación. **La
estimación por reubicaciones se queda corta**: contar «vivo = citado por alguna
reubicación» daba 872 B en `zFe` y el enlazador se lleva 2.624.

La medida buena es directa: **`c33dat_cut3.py` empareja POR NOMBRE los símbolos
de nuestra sección con los del ELF ya enlazado y lee el desplazamiento acumulado
de cada uno.** Cada salto es un símbolo estripado y dice cuánto perdió.

| unidad | `.rodata` nuestra | del objetivo | d(objeto) | **estripado real** | cuadra con el DOL |
|---|---|---|---|---|---|
| `zFe` | 19.312 | 19.992 | −680 | **2.624** | −680−2.624 = −3.304 ≈ **−3.296** ✔ |
| `zSpeech` | 12.360 | 12.712 | −352 | **1.280** | −352−1.280 = **−1.632** ✔ exacto |
| `zPhysics` | 17.264 | 16.064 | **+1.200** | **2.856** | +1.200−2.856 = −1.656 ≈ **−1.632** ✔ |

**Lo que se estripa son las mismas cadenas que el objetivo conserva.** El
mecanismo es de GRANULARIDAD, no de contenido: dtk le da al objetivo **una
etiqueta `lbl_` por tramo contiguo** (`lbl_803DF5C4` mide 732 B), y basta que una
parte esté referenciada para que sobreviva el tramo entero; `cc1plus` emite **un
`$LC` por literal**, así que cada cadena no referenciada muere sola. Ejemplos de
`zFe`: un salto de 536 B antes de `$LC284` que se lleva
`Attribute`/`Attrib::Instance`/`Attrib::Definition`…, otro de 232 B, ocho de 72-80 B
que son las rutas `src/Speed/Indep/Src/Frontend/…/uiRapSheet*.cpp`.

### 1.1 `zPhysics`: de sus 2.856 B, ~1.768 son vtables DUPLICADAS, no estripado

`c33dat_cut3.py` los caza sueltos: `_vt.Q23UTLt11FixedVector3ZP…` ×26 de 64 B,
`_vt.11IRenderable` (128), `_vt.11IDisposable` (72),
`_vt.Q43UTL11Collectionst12Instanceable3ZP10HSIMABLE…` (192). **Nuestro objeto las
define y otro objeto anterior en el orden de enlace también, así que el enlazador
se queda con la primera y tira las nuestras.** Es el frente de `vtdup.py` (falta
de *key method*), visto desde el DOL. Por eso `zPhysics` tiene **1.200 B más** de
`.rodata` que el objetivo y aun así sale corto.

---

## 2. La cabecera de bWare/STL: faltaba también aquí

Las tres tenían **prefijo común de `.rodata` = 0 B**, y el objetivo abre con los
mismos 92 B que las 31 unidades de la r32:

```
"GAMECUBE\0\0\0\0"  "d:/mw/speed/indep/bware/inc/bware.hpp\0\0\0"
"bad_alloc\0\0\0"   "%f,%f,%f\0\0\0\0"  "%f,%f,%f,%f\0"  "STL\0"
```

Los nombres que trae cada objeto extraído **ya están en `keep.lst`**:
`zFe.o:lbl_803DF568`, `zSpeech.o:lbl_80405E10`,
`zPhysics.o:pad_05_803F68F0_rodata`. Escritos con un `asm()` de datos antes del
primer `#include` (`c32pre_apply.py`, reutilizado tal cual):

| unidad | prefijo común | `.rodata` del DOL |
|---|---|---|
| `zFe` | 0 → **96 B** | −3.296 → **−3.200** |
| `zSpeech` | 0 → **92 B** | −1.632 → **−1.536** |
| `zPhysics` | 0 → **92 B** | −1.632 → **−1.568** |

`measure.py --cmp` con base y medida seguidas: **+0 B, +0 funciones, 0 unidades
cambian**.

---

## 3. La propuesta de `keep.lst`: 4.768 B, y NO resucita código

**`config/GOWE69/keep.lst` lo dejo INTACTO** (`git status` vacío). Lo que sigue
está medido pasando el fichero por `--ldflags`, como manda el brief.

El problema es de nombres: `keep.lst` fuerza el dato muerto de los objetos
**extraídos** (`lbl_…`, `gap_…`), y al sustituir el objeto por el nuestro esas
entradas dejan de existir — nuestro pool se llama `$LC…`. `zDynamics` y
`zMission` lo resolvieron escribiendo a mano los 2-5 `lbl_` que les faltaban;
aquí son **cientos de cadenas**, y no caben a mano.

**`c33dat_keeppool.py`** genera un `keep.lst` que añade, por unidad, sólo los
símbolos de `.rodata` que cumplen las cuatro condiciones: **muertos**, **locales**
(nunca una vtable global duplicada), **no forzados ya**, y **sin ninguna
reubicación dentro de su rango** — o sea cadenas y flotantes puros, que no pueden
mantener viva a ninguna función.

| unidad | entradas | `.rodata` antes | después | `.text` |
|---|---|---|---|---|
| `zFe` | 122 | −3.200 | **−640** | **sin cambio** |
| `zSpeech` | 65 | −1.536 | **−448** | **sin cambio** |
| `zPhysics` | 74 | −1.536 | **−448** | **sin cambio** |

Compárese con el `keep.lst` a lo bruto de `c32pre_keepall.py` sobre estas mismas
unidades, que **sí** rompe: `zFe` `.text` **+128 B**, `zPhysics` `.text`
**+3.232 B** y `.rodata` **+1.216** (pasarse), `zSpeech` `.data` **+960**. El
filtro de «sin reubicaciones dentro» es lo que separa una medida de una cura.

**Cuándo aplicarla**: a la vez que la promoción, **nunca antes**. Mientras la
unidad no esté promocionada esas 261 entradas serían RANCIAS y `keepchk.py`
pasaría de 21 a 282. Y **son frágiles**: los números `$LC` cambian con cualquier
edición de los `.cpp` incluidos, así que hay que regenerarlas (el guion tarda un
segundo) y volver a pasar `keepchk.py`.

**Veda medida**: `keep.lst` **no admite comodines**. `zFe.o:$LC*` da exactamente
el mismo DOL que sin la línea (`c6cdfdec7604`). Hay que enumerar.

---

## 4. Lo que sí queda escrito: el `.data` de las tres a delta 0

### 4.1 `zPhysics` −160 → **0** (ensayo c1)

Los seis símbolos de dato muerto del objetivo **ya están en `keep.lst`**
(`zPhysics.o:lbl_8041EFF0`, `lbl_8041F00C`, `lbl_8041F024`,
`gap_06_8041F058_data`, `gap_06_8041F160_data`, `gap_06_8041F168_data`), así que
basta emitirlos con ese nombre. **188 B** en cinco `asm()` de datos colocados en
`zPhysics.cpp` por la regla de la r32 (entre dos `#include`), cada uno cerrado con
**`.previous`** para no desincronizar la sección de GCC:

| bloque | B | dónde |
|---|---|---|
| `lbl_8041EFF0` | 24 | tras `#include Smackable.cpp` |
| `lbl_8041F00C` + `lbl_8041F024` | 20 + 48 | tras `#include VehicleSystem.cpp` |
| `gap_06_8041F058_data` | 32 | tras la definición de `Factory<…>::mHead` |
| `gap_06_8041F160_data` | 4 | delante de `#include PhysicsInfo.cpp` |
| `gap_06_8041F168_data` | 60 | al final (ocupa el sitio del `IMPLEMENT_SINGLETON`) |

Además:

- **`IMPLEMENT_SINGLETON(Smackable::Manager)` movido** del final del fichero a
  detrás de `#include Smackable.cpp`: el objetivo emite `mInstance` en `.data`
  **el segundo**, pegado a `Smackable_RigidCount`.
- **`lbl_803F74CC` y `lbl_803F74D0` sacados de `.data` a `.rodata`.** La r32 los
  definió como `extern const volatile float …` — el `volatile` es necesario
  (lo pide `PhysicsUpgrades.cpp` para que GCC no pliegue la carga) pero manda el
  objeto a `.data`, y el objetivo los tiene en `.rodata:0x803F74CC/D0`. Ahora los
  emite un `asm()` con su sección; la declaración `extern const volatile` no se
  toca. Son 8 B que cambian de sección.

`.data` **444 / 444 B, delta 0**; `.rodata` −1.568 → −1.536.

### 4.2 `zFe` −128 → **0**

Dos cosas:

- **`MovieVolumeArray` medía 16 B de más**: `MoveVolumeInfo MovieVolumeArray[40]`
  con **38** inicializadores → 320 B; el objetivo tiene **304**. El original lo
  declaraba sin tamaño. `GetMovieCategoryVolume` recorre con el literal `0x26`,
  **no** con `sizeof`, así que quitar el `[40]` **no toca una sola instrucción**
  (`measure --cmp` +0 B, `pctsnap --cmp` EMPEORAN: ninguna). Es un caso de manual
  de «cabeceras que mienten».
- **`gap_06_8041BBD8_data` (136 B)**, el último dato de la unidad, escrito con un
  `asm()` al final de `zFe.cpp`. Ya estaba en `keep.lst`.

### 4.3 `zSpeech` −64 → **0**

- **`gap_06_80435F24_data` (36 B)**, el último dato de la unidad, al final de
  `zSpeech.cpp`.
- **`gap_06_80435E78_data` (20 B)**, entre los estáticos de `Csis`
  (`EAXCharacter.cpp`) y el primer estático local de `Observer.cpp`: `asm()`
  delante del `#include Observer.cpp`.

### 4.4 Lo que NO he tocado del `.data`, y por qué

- **`zSpeech` emite 1.032 B que el objetivo no tiene y el enlazador tira**:
  `_Q26Speech32_GLOBAL_.N.VALIDATE_SED_GENERATE.sQueuedEvents` (1.024) y
  `.sQueuedEventCount` (4). No están en `symbols.txt` ni en el ELF enlazado. Es
  una cola de validación que el original compilaba apagada. No la quito porque
  hoy es neutra (el enlazador ya la descarta) y quitarla toca código.
- **Rellenos que caen DENTRO de un fichero o entre estáticos locales de función**:
  `zSpeech` `gap_06_804359BE_data` (2), `gap_06_804359C4_data` (12),
  `gap_06_80435E8D_data` (3), `gap_06_80435EA4_data` (4); `zFe`
  `gap_06_8041B5DC/5FC/63C/648_data`, `lbl_8041B998`, `gap_06_8041BA74_data`,
  `lbl_8041BBCC`. Un `asm()` de ámbito de fichero no llega, y varios además
  necesitan reordenar variables dentro del `.cpp` (en `zFe`,
  `SceneryModel::mSceneryCount` va antes de `TheSmackableClass` en el objetivo y
  al revés en `SmokeableInfo.cpp`; en `zSpeech`, `DESTROY_COPS_ON_INACTIVITY`
  antes de `FORCE_VOICE_RANDOMIZATION`). Como el `.data` ya está en delta 0, eso
  es trabajo de **orden**, no de tamaño.
- **`zFe` emite `gButtonIDs` y `gButtonTextIDs` (24 B) que el objetivo no tiene**
  en esta unidad. Compensados hoy por los rellenos que faltan; apuntado.

---

## 5. Lo que queda: NO son «funciones muertas», son cabeceras generadas

`r32-pre` §4 dejó dicho que el resto era «escribir las funciones muertas». Con las
tres unidades medidas, **el contenido dice otra cosa**. `c33dat_str.py` compara el
**multiconjunto de cadenas** de las dos `.rodata` (así se separa «falta» de «está
en otro orden», que es lo que confunde a un diff de bloques):

| unidad | cadenas del objetivo | nuestras | **FALTAN** | SOBRAN |
|---|---|---|---|---|
| `zFe` | 591 | 498 | **146 (2.295 B)** | 53 (1.980 B) |
| `zSpeech` | 294 | 272 | **52 (775 B)** | 30 (609 B) |
| `zPhysics` | 184 | 164 | **40 (675 B)** | 20 (339 B) |

Tres familias, y las tres tienen dueño identificado:

1. **`Attrib::Gen::<clase>` — cabeceras generadas que nuestro TU no arrastra.**
   La cadena la emite `USE_ATTRIB_ALLOC("Attrib::Gen::pvehicle")` dentro de
   `src/Speed/Indep/Src/Generated/AttribSys/Classes/<clase>.h`. Las que faltan
   existen en el árbol y no se incluyen: `milestonetypes`, `audioimpact`,
   `audioscrape`, `camerainfo`, `effects`, `speech`, `world`, `explosion`,
   `engineaudio`, `audiosystem`, `ecar`, `speechtune`, `aud_moment_strm`. Son
   **~142 B en `zFe`, ~170 en `zSpeech`, ~214 en `zPhysics`** — entre el 22 % y el
   48 % de lo que falta. **La lista de cadenas que faltan ES la lista de
   cabeceras generadas que el TU original incluía.**
2. **Un racimo compartido que falta EN LAS TRES A LA VEZ** (152 B):
   `MAudioReflection`, `GRaceStatus`, `PlayerNum`, `MGeneric`, `Covered`,
   `16.1.0`, `1.8.1`, `Dist` + los tres `Attrib::Gen::` de arriba. Y en dos de
   tres (418 B): `WorldBodyConn`, `Pkt_Body_Open/Send/Service`,
   `World_UpdateBody`, `World_OneShotEffect`, `WorldEffectConn`,
   `Pkt_Effect_Open/Send/Service`, `PathControl`, `PathEvent`, `Licenced`,
   `PartID`. **Una sola cabecera/plantilla arregla las tres.**
3. **Nombres de globales que nuestro árbol no tiene.** En `zPhysics` faltan
   `ENABLE_ROLL_STOPS_THRESHOLD`, `PAD_DEAD_ZONE`, `DAMAGE_SCALE_COLLISION`,
   `MAX_WHEEL_SPIN_RATE_AI`, `POST_BRAKE_ACCEL_COUNT/SCALE/MIN`,
   `WHEEL_SPIN_EXTRA_RPM`, `ROLLING_RESISTANCE`, `TYRE_DAMAGE_RADIUS`,
   `BASE_FRICTION_MASS`, `SKID_AUDIO_SCALE`, `MIN_BUTTON_VALUE`, `EMP_LIFETIME`,
   `SHRED_DRAG`. Las dos primeras **son exactamente los dos únicos globales que
   nuestro `VehicleSystem.cpp` define**, y los 92 B de dato muerto que le he
   escrito a mano en el §4.1 (`lbl_8041EFF0`/`F00C`/`F024`) **son los valores de
   los que faltan** — `{120, 2.0f, 1.25f, 2500.0f, 50, 10}`, `{8.0f,0,0,2.0f,0}`,
   `{0.15f, 0.1f, 0.1f, 4000.0f, 0.3f, 1.0f, …}`. Es decir: el `namespace
   VehicleSystem` del original tenía ~15 globales y una función que los registra
   por nombre; nosotros tenemos dos. **Ahí sí hay código que escribir**, y el
   dato y la cadena caen juntos.

En `zFe` hay además ~40 cadenas de una sola familia (`STATUS_*` de la tarjeta de
memoria, `pm_*` del menú de pausa, `RapSheet*ArraySlot/Datum`, `JukeBox*`,
`Assert: %s:%i (%s)`, `CreateUIProfileManager`, `MC_DeleteProfile.fng`) que sí
huelen a las funciones muertas de `r32-pre`.

---

## 6. Verificación

- `build_direct.py`: **6 ok, 0 fallidas** (zFe, zFe2, zFeOverlay, zSpeech,
  zPhysics, zPhysicsBehaviors).
- **`measure.py --cmp` con base y medida SEGUIDAS** (base tomada revirtiendo mis
  cuatro ficheros, recompilando y volviéndolos a poner desde el scratchpad):
  `1.116.872/1.127.344 B`, **+0 B, +0 funciones, 0 unidades cambian**.
- **`pctsnap.py --cmp`** sobre las seis: **EMPEORAN: ninguna** · MEJORAN: 0.
- **`audit.py`, DOS pasadas** en `Speed/Indep/SourceLists/{zFe,zSpeech,zPhysics}`:
  **0 FALLA** en las seis pasadas.
- `keepchk.py`: **759 ok, 21 RANCIAS, 0 sin objeto** — idéntico a como empecé.
- **Control (sin promocionar nada, con mis cambios puestos): `main.dol de la lista
  de enlace VIVA: OK`.**
- `git status --porcelain config/ configure.py splits.txt build.ninja`: **vacío**.
- `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- **Sin commit.** Sin ensamblador de instrucciones: los seis `asm()` que añado son
  `.4byte` de datos con su `.section`/`.size`/`.previous`.

### 6.1 Ficheros que dejo modificados (cuatro)

| fichero | qué | § |
|---|---|---|
| `src/Speed/Indep/SourceLists/zFe.cpp` | cabecera 92 B + `gap_06_8041BBD8_data` (136 B) | 2, 4.2 |
| `src/Speed/Indep/SourceLists/zSpeech.cpp` | cabecera 92 B + `gap_06_80435E78_data` (20 B) + `gap_06_80435F24_data` (36 B) | 2, 4.3 |
| `src/Speed/Indep/SourceLists/zPhysics.cpp` | cabecera 92 B + los seis bloques de `.data` (188 B) + `IMPLEMENT_SINGLETON` movido + `lbl_803F74CC/D0` a `.rodata` | 2, 4.1 |
| `src/Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.cpp` | `MovieVolumeArray[40]` → `[]` (320 → 304 B) | 4.2 |

Respaldos del estado de partida en
`…/scratchpad/c33dat_backup/{zFe,zSpeech,zPhysics}.cpp.orig` y
`MoviePlayer.cpp.orig`.

---

## 7. Vedas y trampas nuevas

1. **`keep.lst` no admite comodines** (`zFe.o:$LC*` no hace nada, ni error ni
   efecto). §3.
2. **Contar «vivo» por reubicaciones NO mide el estripado.** Dos errores
   distintos: (a) `.rela.debug` referencia **todos** los símbolos y deja «vivo»
   hasta el dato muerto — hay que saltarse `.rela.debug*` y `.rela.line`; (b) aun
   así se queda corto, porque no ve las vtables que el enlazador descarta por
   duplicadas. **La medida buena es contra el ELF enlazado** (`c33dat_cut3.py`).
3. **El prefijo común de una sección del DOL no localiza la unidad.** Promocionar
   `zSpeech` cambia **820 bytes sueltos** repartidos por todo `data2` **antes** de
   su rango, y `zPhysics` **472**: son bytes bajos de punteros que se corren
   porque `.bss`/`text2` se desplazan. En `zFe` son **2 bytes**. Todos benignos:
   hay que mirar el rango de la unidad, no el primer byte distinto.
4. **Un `asm()` con `.section` en medio del fichero necesita `.previous`.** GCC
   2.9 lleva su propia cuenta de la sección activa y no vuelve a emitir la
   directiva; sin `.previous` el siguiente dato del compilador cae en la sección
   del `asm()`.
5. **Los símbolos del ELF enlazado conservan el `st_size` de antes del
   estripado**, así que parecen solaparse (`pad_05_803F6890_rodata` de 92 B con un
   `$LC60` dentro). La dirección sí es la buena; el tamaño no.
6. **`const volatile` manda el objeto a `.data`.** Si el objetivo lo tiene en
   `.rodata` y el `volatile` hace falta para el codegen, la salida es emitirlo
   como dato con su `.section` (§4.1), no quitar el `volatile`.
7. **El sha1 de `trypromo.py` no es reproducible entre agentes.** `build.ninja`
   me cambió de md5 **entre dos órdenes seguidas**, y con él el sha1 de una
   promoción cuyos deltas de sección eran idénticos al byte. Antes de acusar a un
   cambio propio de mover el hash, compara los **deltas por sección**; y si vas a
   citar un sha1, cita el md5 del `build.ninja` con el que salió.

---

## 8. Lo que haría el siguiente

1. **Aplicar la propuesta del §3 junto con la promoción**, no antes. Vale
   **4.768 B** y está medida. Regenerar con `c33dat_keeppool.py` y pasar
   `keepchk.py` justo después.
2. **Las cabeceras generadas de AttribSys** (§5.1): la lista de cadenas
   `Attrib::Gen::*` que faltan es la lista de `Classes/*.h` que el TU original
   incluía. Es lo más barato que queda y toca las tres.
3. **El racimo compartido `WorldBodyConn`/`Pkt_Body_*`/`MAudioReflection`**
   (§5.2): falta en las tres a la vez, así que sale de una cabecera común.
4. **El `namespace VehicleSystem` completo** (§5.3): los ~13 globales que faltan,
   con los valores ya leídos del ELF en los bloques del §4.1. Cierra dato y cadena
   a la vez, y deja de hacer falta el `asm()`.
5. **Las vtables duplicadas de `zPhysics`** (§1.1, 1.768 B): es `vtdup.py`. Hoy no
   cuestan bytes al DOL (el enlazador las tira) pero enmascaran la medida.
6. **El orden**: con `.data` en delta 0 en las tres, lo que le queda a esa sección
   es puro orden (§4.4). Y en `.rodata` el orden del pool también está cambiado
   desde el byte 96 — nuestro `$LC` empieza por `Attrib::CollectionHashMap` y el
   objetivo por `Attrib::Attribute`. **No me he metido ahí.**

---

## 9. Herramientas (scratchpad, prefijo `c33dat_`)

| guion | qué hace |
|---|---|
| **`c33dat_cut3.py <u> <addr>`** | **la que cierra el diagnóstico.** Empareja por nombre nuestros símbolos de datos con los del ELF enlazado y lee el desplazamiento acumulado: cada salto es un símbolo estripado (o una vtable descartada) y dice cuánto perdió |
| **`c33dat_keeppool.py <u…>`** | el `keep.lst` del §3: sólo símbolos de `.rodata` muertos, locales y **sin reubicaciones dentro**. No puede resucitar código |
| **`c33dat_str.py <u…>`** | multiconjunto de cadenas de las dos `.rodata`: separa «falta» de «está en otro orden», que es lo que un diff de bloques no distingue |
| **`c33dat_symdiff.py <u> --sec .data`** | las dos secciones en columna emparejadas por nombre: qué símbolo falta, cuál sobra y cuál tiene otro TAMAÑO (así salió `MovieVolumeArray`) |
| `c33dat_try.py [--ldflags F] [--out N] <u…>` | como `trypromo.py` pero **conserva** el `.elf`/`.dol` e imprime la tabla de secciones contra `main.dol` |
| `c33dat_strip.py` / `c33dat_strip2.py` | la estimación por reubicaciones (simple y con cierre transitivo). **Se quedan cortas**: valen para ver el reparto vivo/muerto, no para medir |
| `c33dat_live.py <u> [--obj] [--que]` | los símbolos de una sección con su estado y su contenido, saltándose `.rela.debug` |
| `c33dat_span.py <u> --sec data2` | prefijo/sufijo común de una sección del DOL (ojo con la trampa 3) |
| `c33dat_zphys_c1.py` | el parche del §4.1, idempotente |

Reutilizados de la r32: `c32pre_genasm.py`, `c32pre_apply.py` (con la marca
cambiada a `// c33dat:`), `c32pre_pref.py`, `c32pre_ins.py`, `c32pre_keepall.py`,
`c32pre_base.py`.

**Disco**: 18 GB libres al empezar y al acabar. Borrados mis `.elf` de trabajo
(16 MB cada uno) y los `.elf`/`.dol` que la r32 dejó en este mismo scratchpad.
