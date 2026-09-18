# Ronda 34 — `dat2`: las cuatro SourceLists bloqueadas sólo por datos

Encargo: `zFe2` (250.732 B), `zSpeech` (178.888), `zFe` (174.200) y `zPhysics`
(146.124) — **750.000 B** que enlazan y dan `DOL ROTO` sólo por `.rodata`.

**Ninguna llega a `DOL OK`, y lo digo lo primero.** Pero el déficit de las cuatro
**baja de −11.520 B a −4.640 B**, y **`zSpeech` se queda a 192 B**, `zPhysics` a
384 y `zFe` a 512. Todo con **`measure.py --cmp` +0 B / +0 funciones**,
**`pctsnap --cmp` EMPEORAN: ninguna** y **el `.text` del DOL intacto** (las tres
secciones `text0/1/2` a delta 0 y desplazamiento 0 en las cuatro).

---

## 0. Las seis frases

1. **El déficit se parte en dos y ya está medido al byte**: `-strip-unused-data`
   se lleva **2.448 B en `zFe`, 1.104 en `zSpeech`, 2.680 en `zPhysics` y 1.440
   en `zFe2`**; el resto es dato ausente. En `zPhysics` el estripado es **el
   168 %** del déficit (su `.rodata` es 1.064 B **más grande** que la del
   objetivo) y **1.696 de esos 2.680 B son vtables duplicadas**, no estripado.
2. **Los objetos del OBJETIVO no pierden ni un byte al enlazar** (medido:
   0 B en las cuatro). Todo el daño es nuestro, y por eso `keep.lst` lo recupera.
3. **La propuesta de `keep.lst` vale 6.656 B y NO toca el `.text`**: 157/102/87/97
   entradas (`zFe`/`zSpeech`/`zPhysics`/`zFe2`) de símbolos de `.rodata`
   **muertos, locales y sin reubicaciones dentro**. Verificada enlazando: las
   tres secciones de código salen a **0 de tamaño y 0 de desplazamiento**.
4. **El dato ausente es un problema de `#include`, y se arregla en la
   SourceList**: las cadenas `Attrib::Gen::<clase>` que faltan son exactamente
   las cabeceras generadas de AttribSys que el TU original arrastraba y el
   nuestro no. **Puestas: +328 / +328 / +232 / +304 B de `.rodata`, +0 B y +0
   funciones de match.**
5. **Y no sólo añaden bytes: RECOLOCAN el pool.** La igualdad byte a byte de
   `.rodata` contra el objeto extraído sube **41,0 → 71,3 %** en `zSpeech`,
   45,0 → 50,3 en `zFe`, 69,0 → 71,1 en `zPhysics` y 19,3 → 21,6 en `zFe2`. Es
   la señal de que el arreglo es el bueno y no un relleno.
6. **Lo que queda tiene dueño con nombre y prueba**: el racimo compartido
   (`MGeneric`, `MAudioReflection`, `PlayerNum`, `Dist`, `Covered`, `PathEvent`,
   `Licenced`, `PartID`, `16.1.0`, `1.8.1`…) sale de que en el original
   **`BuildMessageTable` estaba DENTRO de las cabeceras `Generated/Messages/*.h`**
   y en nuestro árbol está sacada a `Generated/Events/EventDefs.cpp` (§5.1).

### Estado por unidad (todo con `.text` a delta 0)

| unidad | inicio r34 | + los `#include` (§4, **puesto**) | + `keep.lst` (§3, **propuesto**) |
|---|---|---|---|
| `zFe2` | `.rodata` −4.896 · `.data` −160 · **−5.056** | −4.832 · −160 · **−4.992** | −3.392 · −160 · **−3.552** |
| `zFe` | −3.264 · 0 · **−3.264** | −3.200 · 0 · **−3.200** | **−512** · 0 · **−512** |
| `zSpeech` | −1.600 · 0 · **−1.600** | −1.536 · 0 · **−1.536** | **−192** · 0 · **−192** |
| `zPhysics` | −1.600 · 0 · **−1.600** | −1.568 · 0 · **−1.568** | **−384** · 0 · **−384** |
| **suma** | **−11.520** | **−11.296** | **−4.640** |

Los sha1 de la última columna, con el `build.ninja` de md5 vivo en ese momento:
`zFe2 263cfa268399`, `zFe 8e478468a8fb`, `zSpeech 054dc852d5ab`,
`zPhysics dbb933d67190`. (Aviso de la r33 §7.7 vigente: **el sha1 no es
reproducible entre agentes; la medida estable es el delta por sección.**)

---

## 1. Verificación del encargo: reproduce, con un aviso

Las tres que la r33 dejó medidas salían **64 B peor** que en su informe
(`zFe` −3.264 contra −3.200, `zSpeech` y `zPhysics` −1.600 contra −1.536). No es
un error de nadie: entre las dos rondas cambió el árbol (el §2 del brief-r34
retiró 232 B de etiquetas `Attrib::*` en las cuatro unidades medidas).

Y **`zFe2` no enlazaba al empezar**: `ENLACE FALLA (16 errores)`, los ocho pares
de clases locales de `VehicleDB.cpp` con el `DECL_UID` caducado (`.31025` contra
`.31028`). Es exactamente la fragilidad declarada en `r33-und3` §3.4 y se cierra
con los dos comandos que ella misma documenta:

    python scripts/build_direct.py zFe2
    python scratchpad/c33und3_uid.py --write

Hecho, y **`zFe2` pasa a `DOL ROTO`**. Lo dejo resincronizado. **Cualquier
edición futura de los `#include` de `zFe2.cpp` lo vuelve a romper**: los míos del
§4 lo rompieron y los volví a sincronizar; el `git diff` de `VehicleDB.cpp` son
**16 etiquetas alias y nada más**.

---

## 2. Cuánto es estripado y cuánto dato ausente — medido, no modelado

La receta buena de la r33 (emparejar por nombre nuestros símbolos con los del ELF
ya enlazado y leer el desplazamiento acumulado) reescrita en
**`scratchpad/c34dat2_cut.py`** y **`c34dat2_gap.py`**, con **una trampa nueva que
falsea la medida si no se ve** (§7.1).

Estado **al empezar la ronda**:

| unidad | nuestra `.rodata` | del objetivo | d(contenido) | **estripado** | de él, vtables dup | déficit DOL | cuadra |
|---|---|---|---|---|---|---|---|
| `zFe` | 19.168 | 19.992 | **−824** | **2.448** | 32 | −3.264 | 19.168−2.448−19.992 = −3.272 ✔ (8 B de alineación) |
| `zSpeech` | 12.216 | 12.712 | **−496** | **1.104** | 24 | −1.600 | −1.600 ✔ **exacto** |
| `zPhysics` | 17.128 | 16.064 | **+1.064** | **2.680** | **1.696** | −1.600 | −1.616 ✔ (16 B) |
| `zFe2` | 22.144 | 25.704 | **−3.560** | **1.440** | 240 | −4.992 | −5.000 ✔ (8 B) |

Reparto del déficit: **75 % estripado en `zFe`, 69 % en `zSpeech`, 29 % en
`zFe2`**; en `zPhysics` el estripado (2.680) es **mayor que el déficit** porque su
`.rodata` es 1.064 B más grande que la del objetivo.

**Y el otro lado de la resta, que la r33 no midió: los objetos EXTRAÍDOS pierden
0 B.** Pasados los cuatro por la misma herramienta contra el ELF de control:
`0 B en 0 huecos` en las cuatro. `keep.lst` los protege enteros. Es lo que
convierte la asimetría en una cura: no hay que adivinar qué conserva el
objetivo, **lo conserva todo**.

---

## 3. La propuesta de `keep.lst`: 6.656 B y el `.text` intacto

**`config/GOWE69/keep.lst` queda INTACTO** (`git status` vacío). Los cuatro
ficheros propuestos están en `scratchpad/c34dat2_keep_<unidad>.lst` y se
regeneran en un segundo con

    python scratchpad/c34dat2_keeppool.py <unidad> -o <fichero>

Añade, por unidad, sólo los símbolos de `.rodata` que cumplen las cuatro
condiciones de la r33: **muertos** (caen en un hueco de estripado), **locales**,
**no forzados ya** y **sin ninguna reubicación dentro de su rango** — cadenas y
flotantes puros, que no pueden mantener viva una función.

| unidad | entradas nuevas | `.rodata` del DOL antes | después | `.text` (`text0/1/2`) |
|---|---|---|---|---|
| `zFe` | 157 | −3.200 | **−512** | **0 / 0 / 0**, desplazamiento 0 |
| `zSpeech` | 102 | −1.536 | **−192** | **0 / 0 / 0** |
| `zPhysics` | 87 | −1.568 | **−384** | **0 / 0 / 0** |
| `zFe2` | 97 | −4.832 | **−3.392** | **0 / 0 / 0** |

Medido pasando el fichero por `--ldflags`, como manda el brief, y repetido dos
veces con el mismo resultado al byte.

**Cuándo aplicarla: a la vez que la promoción, nunca antes.** Las entradas son
`zFe.o:$LC1234` y ese símbolo **sólo existe en NUESTRO objeto**; mientras la
unidad no esté promocionada, `keepchk.py` las contaría todas como RANCIAS (hoy:
**759 ok, 21 RANCIAS, 0 sin objeto**, idéntico a como empecé). Y **son frágiles**:
los números `$LC` se mueven con cualquier edición de los `.cpp` incluidos.

### 3.1 Aviso metodológico sobre esta propuesta

El filtro fuerza **todo** el dato muerto que cumple las cuatro condiciones,
incluido el que **el objetivo no tiene**. En `zSpeech` eso son ~250 B de
`Attrib::Gen::{transmission,chassis,tires,nos,induction,turbosfx,acceltrans,
shiftpattern,presetride,emitterdata,emittergroup}` que nosotros emitimos de más.
Hoy no hay sobrepasamiento (las cuatro siguen cortas), pero **para un `DOL OK`
byte a byte el conjunto forzado tendrá que ser el exacto**, no el máximo.

---

## 4. Lo que dejo escrito: los `#include` que al TU original le faltaban

Cuatro ficheros, un bloque cada uno, delante del primer `#include`:

| fichero | cabeceras añadidas |
|---|---|
| `src/Speed/Indep/SourceLists/zFe.cpp` | `Classes/{milestonetypes,audioimpact,audioscrape,speechtune,effects,speech}.h` + `World/WorldConn.h` |
| `…/zSpeech.cpp` | `Classes/{milestonetypes,audioimpact,audioscrape,camerainfo,effects,ecar}.h` + `World/WorldConn.h` |
| `…/zPhysics.cpp` | `Classes/{milestonetypes,audioimpact,audioscrape,engineaudio,audiosystem,camerainfo,explosion,speech,world}.h` |
| `…/zFe2.cpp` | `Classes/{milestonetypes,audioimpact,audioscrape,effects,speech}.h` + `World/WorldConn.h` |

La lista sale de `c34dat2_str.py`, que compara el **multiconjunto de cadenas** de
las dos `.rodata`: **la lista de cadenas `Attrib::Gen::*` que faltan ES la lista
de cabeceras generadas que hay que incluir**, una a una y sin excepción.

### 4.1 El mecanismo, comprobado

`USE_ATTRIB_ALLOC("Attrib::Gen::milestonetypes")` es un `operator new` **en
clase**. Nadie lo llama y aun así **basta con incluir la cabecera para que la
cadena nazca en `.rodata`**: ensayo c1 con una sola cabecera en `zSpeech`,
`.rodata` 12.216 → 12.248 (+32) y **una cadena menos en la lista de las que
faltan**. Lo mismo con `WorldConn.h`, cuyo `DECLARE_WORLDPACKET` mete un
`static UCrc32 hash(#_HANDLER_)` en una virtual **en clase**: +176 B y diez
cadenas (`WorldBodyConn`, `Pkt_Body_{Open,Send,Service}`, `World_UpdateBody`,
`World_OneShotEffect`, `WorldEffectConn`, `Pkt_Effect_{Open,Send,Service}`).

### 4.2 Lo que cuesta y lo que no

- **`.text` del objeto**: crece (`zSpeech` 191.028 → 191.268 B) porque nacen
  instanciaciones nuevas, **pero son código muerto**: el DOL no se mueve ni un
  byte (`text0/1/2` a 0) y **`measure.py --cmp` da +0 B / +0 funciones** y
  **`pctsnap --cmp` `EMPEORAN: ninguna`** sobre las cuatro unidades.
- **`.bss` del objeto** también crece con `WorldConn.h` (los `static UCrc32`);
  igual de muerto, igual de invisible en el DOL.
- **Ganancia directa en el DOL: sólo +64/+64/+32/+64 B**, porque las cadenas
  nuevas nacen muertas y el enlazador se las lleva. **La ganancia real aparece
  junto al `keep.lst` del §3**, y ahí es donde `zFe` pasa de −3.200 a −512.

### 4.3 Y lo que de verdad demuestra que es el arreglo bueno

No es el tamaño, es la **igualdad byte a byte** de `.rodata` contra el objeto
extraído (`datacmp.py`), que no puede subir por rellenar:

| unidad | antes | después |
|---|---|---|
| `zSpeech` | 41,0 % | **71,3 %** |
| `zPhysics` | 69,0 % | **71,1 %** |
| `zFe` | 45,0 % | **50,3 %** |
| `zFe2` | 19,3 % | **21,6 %** |

Las cadenas nuevas **recolocan el pool**, no lo engordan.

---

## 5. Lo que queda, con dueño y con prueba

### 5.1 El racimo compartido: `BuildMessageTable` estaba EN LA CABECERA

Lo que falta en las cuatro a la vez (`MGeneric`, `MAudioReflection`, `PlayerNum`,
`Dist`, `Covered`, `PathControl`, `PathEvent`, `Licenced`, `PartID`,
`Perpetrator`, `MinHeatLevel`, `StartBreaker`, `AnchorWorldID`, `Reflection`,
`High`, `Medium`, `Low`, `16.1.0`, `1.8.1`, `GRaceStatus`…) **no son nombres de
clase sueltos: son los nombres de CAMPO que `BuildMessageTable` mete en la tabla
de Lua**, y están en el objetivo **en el orden exacto del código**:

    objetivo zPhysics .rodata:  0x494 'MAudioReflection'
                                0x4a8 'PlayerNum'
                                0x4b4 'Dist'
                                0x4bc 'Covered'

    src/…/Generated/Events/EventDefs.cpp:1075  MAudioReflection::BuildMessageTable
        lua_pushstring(luaState, "PlayerNum");
        lua_pushstring(luaState, "Dist");
        lua_pushstring(luaState, "Covered");

Es decir: **en el original ese cuerpo vivía DENTRO de
`Generated/Messages/MAudioReflection.h`** (regla «en clase = inline en GCC 2.9»),
así que sus cadenas nacían en **todas** las unidades; nuestro árbol lo tiene
sacado a `EventDefs.cpp` y por eso no nacen en ninguna.

**Prueba de que sólo viajaban las cadenas y no el código**: `debug_lines.txt` no
atribuye **ni una instrucción** de `EventDefs.cpp` al rango `.text` de `zPhysics`
ni al de `zSpeech`; sí atribuye instrucciones a `Generated/Messages/MJumpCut.h`,
`MControlPathfinder.h`, `MNotifySpeechStatus.h`… — o sea, esas cabeceras **sí**
emitían código inline en el original.

**Lo que frena aplicarlo hoy**: `Misc/Hermes.h` sólo lleva `struct lua_State;`
adelantada, así que meter el cuerpo en la cabecera exige la API de Lua en todos
los TU. Es un frente propio (66 `Messages/*.h` + 323 `Events/*`), y la vía segura
es la del brief §4: **`#ifndef GUARDA` en la cabecera + `#define` en la
SourceList**, radio de explosión cero.

### 5.2 `zPhysics`: el `namespace VehicleSystem` sigue vacío

`src/Speed/Indep/Src/Physics/Common/VehicleSystem.cpp` define **dos** globales y
tiene `InitializeGlobals()` e `InitializeVehicleGlobals()` **como cuerpos
vacíos**. Las 14 cadenas que le faltan a `zPhysics` (`ENABLE_ROLL_STOPS_THRESHOLD`,
`PAD_DEAD_ZONE`, `DAMAGE_SCALE_COLLISION`, `MAX_WHEEL_SPIN_RATE_AI`,
`POST_BRAKE_ACCEL_{COUNT,SCALE,MIN}`, `WHEEL_SPIN_EXTRA_RPM`,
`ROLLING_RESISTANCE`, `TYRE_DAMAGE_RADIUS`, `BASE_FRICTION_MASS`,
`SKID_AUDIO_SCALE`, `MIN_BUTTON_VALUE`, `EMP_LIFETIME`, `SHRED_DRAG`) son los
nombres con los que ese cuerpo los registraba, y **los valores ya están escritos
a mano** en `zPhysics.cpp` (`lbl_8041EFF0`/`F00C`/`F024`, r33 §4.1). **Es código,
no dato**: fuera de mi encargo, pero es lo que le queda a `zPhysics` de sus 384 B.

### 5.3 `zFe2` es la que más lejos está, y por dos motivos

- **`.rodata` −3.392**: le faltan ~280 cadenas (~3,5 kB) y **la mayoría son
  nombres de clase de pantalla y de elemento de HUD** (`uiRapSheet*`,
  `uiRepSheet*`, `Customize*`, `UIQR*`, `Hud_Speedometer`, `Hud_Tachometer`,
  `PostRaceResultsScreen`…). En el objetivo **ninguna tiene una sola reubicación
  apuntándola** (comprobado): son dato muerto que sobrevive porque cae dentro de
  un `lbl_` vivo. `Hud_Speedometer` **no existe en nuestro árbol**, así que ahí
  falta código, no un `#include`.
- **`.data` −160**: al objetivo le faltan 17 huecos de dato muerto (162 B:
  `gap_06_8041BC6C_data` 24 B, `lbl_8041CBB0` 16, `gap_06_8041D01C_data` 36…) y
  **nosotros emitimos 56 B que él no tiene en esta unidad**: `MinimapDispY` y los
  trece `g_bCustomize*`/`g_pCustomize*` (que en el objetivo viven en
  `zFeOverlay`, el mismo caso que `gTradeInFactor` de la r33). Escribir los 162 B
  sin quitar los 56 **se pasaría**.
- **`TuningSlider.cpp`** (r33-und3 §7.3): confirmo que **no está en ninguna
  SourceList**, y añado el dato que faltaba — `debug_lines.txt` **no le atribuye
  ni una instrucción** al rango `.text` de `zFe2`. Sus cadenas están en el rango
  de `.rodata` del objetivo, pero su código no: era un fichero del TU **con todo
  el código muerto**. Meterlo en la SourceList no es gratis y no cierra `.text`.

### 5.4 Un falso positivo que dejo cerrado: `COPSPEECH.cpp`

`debug_lines.txt` atribuye **139 instrucciones** del rango `.text` de `zSpeech` a
`src/eaxsound/snd_gen/copspeech.cpp`, y ese fichero **existe en el árbol y no
está en ninguna SourceList**. Parece la nota «SourceLists con ficheros de menos»,
y **no lo es**: `zSpeech` está al 99,4902 % y sus **912 B que faltan son
exactamente dos funciones** (`Speech::RoadblockFlow::Setup` 596 B al 98,26 % y
`Speech::Manager::LoadSpeechBank` 316 B al 95,32 %). Esas 139 instrucciones son
**expansiones inline**, no un fichero ausente. No gastéis la ronda ahí.

---

## 6. Verificación

- `build_direct.py`: **6 ok, 0 fallidas** (zFe, zFe2, zFeOverlay, zSpeech,
  zPhysics, zPhysicsBehaviors), varias veces.
- **`measure.py --cmp` con base y medida SEGUIDAS**, base tomada restaurando mis
  cuatro `.cpp` desde el scratchpad y recompilando, medida tomada volviéndolos a
  poner y recompilando: `1.116.872/1.127.344 B` en las dos, **+0 B, +0 funciones,
  0 unidades cambian**.
- **`pctsnap.py --cmp`** sobre las cuatro (3.649 funciones): **`EMPEORAN:
  ninguna`** · `MEJORAN: 0`.
- **`audit.py`, DOS pasadas** en `Speed/Indep/SourceLists/{zFe,zSpeech,zPhysics,zFe2}`:
  **0 FALLA** en las ocho.
- **CONTROL, sin promocionar nada y con mis cambios puestos: `DOL OK`,
  4.541.888 B, todas las secciones a delta 0.**
- `keepchk.py`: **759 ok, 21 RANCIAS, 0 sin objeto** — idéntico a como empecé.
- `git status --porcelain config/ configure.py build.ninja objdiff.json`: sólo
  `config/GOWE69/splits.txt`, **y no es mío**: el diff son dos líneas
  (`.sdata`/`.sdata2` de `libc/e_rem_pio2.c`) de otro agente de esta ronda. No lo
  he tocado ni revertido.
- `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- **Sin commit. Sin una sola instrucción de ensamblador**: lo único que toco de
  un `asm()` son las 16 etiquetas alias que resincroniza `c33und3_uid.py`.

### 6.1 Ficheros que dejo modificados (5)

| fichero | qué | § |
|---|---|---|
| `src/Speed/Indep/SourceLists/zFe.cpp` | 7 `#include` | 4 |
| `src/Speed/Indep/SourceLists/zSpeech.cpp` | 7 `#include` | 4 |
| `src/Speed/Indep/SourceLists/zPhysics.cpp` | 9 `#include` | 4 |
| `src/Speed/Indep/SourceLists/zFe2.cpp` | 6 `#include` (**además** del prefijo de bWare que dejó `c34pre2` en la misma ronda) | 4 |
| `src/Speed/Indep/Src/Frontend/Database/VehicleDB.cpp` | **sólo** las 16 etiquetas alias resincronizadas | 1 |

Respaldos del estado de partida en `scratchpad/c34dat2_<unidad>.cpp.orig` y
`c34dat2_VehicleDB.cpp.orig`.

**Convivencia**: otro agente (`c34pre2`) trabajó sobre `zFe2.cpp` durante mi
encargo (le puso el prefijo de bWare de 92 B, que vale +96 B de DOL y **ya está
contado en mi línea de partida**). Su trabajo está intacto; mi parche es binario
y usa como ancla **un `#include` a principio de línea**, porque su comentario
lleva la palabra `#include` dentro y una búsqueda simple inserta en medio de él
(me pasó, §7.3).

---

## 7. Trampas y vedas nuevas

1. **Emparejar por nombre contra el ELF enlazado exige unicidad EN LOS DOS
   LADOS.** Los `$LC*` son locales y se repiten en cada objeto NUESTRO ya
   promocionado; sin filtrar por «el nombre aparece una sola vez en el ELF», la
   medida del estripado de `zFe` sale con un salto fantasma de **46.748 B**. Es
   la trampa que más cerca estuvo de invalidar el diagnóstico entero.
2. **El `st_size` de los símbolos del ELF enlazado es el de ANTES del estripado**
   (ya estaba en la r33 §7.5): se solapan y no sirven para acotar; **la dirección
   sí**.
3. **`b.find(b'#include')` no es un ancla.** En `zFe2.cpp` la palabra aparece
   dentro de un comentario de otro agente y el bloque se insertó **partiendo el
   comentario**: `#include' expects "FILENAME"`. El ancla es `\n#include`.
4. **`pctsnap.py` sin unidades toma las 33 y escribe `pct.json` en la raíz** — y
   `-o` sí acepta ruta, pero los nombres de unidad son **cortos** (`zFe`), no la
   ruta completa que quieren `audit.py` y `measure.py`. Mezclarlos aborta con
   `unidad desconocida` **después** de haber pisado `pct.json`.
5. **Añadir un `#include` cambia bytes del `.text` del objeto sin cambiar su
   tamaño**: 1.315 B en `zSpeech`, todos en el bloque diferido de plantillas
   (`reserve__…`, `find__…`, `Get__…`). **No es una regresión** (`measure --cmp`
   +0), pero invalida el md5 del `.o` como prueba de «no he tocado nada».
6. **`-strip-unused-data` no le quita ni un byte a los objetos EXTRAÍDOS.** Si
   una medida dice lo contrario, es que el ELF con el que se compara no es el de
   control.

---

## 8. Lo que haría el siguiente

1. **Aplicar el `keep.lst` del §3 junto con la promoción.** Vale 6.656 B, está
   medido dos veces y **no mueve el `.text`**. Regenerar con
   `c34dat2_keeppool.py` y pasar `keepchk.py` justo después.
2. **`BuildMessageTable` a la cabecera** (§5.1), con la guarda del brief §4. Es
   el único frente que toca **las cuatro a la vez** y además las otras 29
   unidades, y ya está probado que el mecanismo (cuerpo en clase → cadena en cada
   TU) funciona con `USE_ATTRIB_ALLOC` y con `DECLARE_WORLDPACKET`.
3. **`zSpeech` está a 192 B y `zPhysics` a 384.** Son las dos que pueden cerrar
   primero. En `zPhysics` los 384 son casi todos el `namespace VehicleSystem`
   (§5.2), que es **código con el dato ya escrito**.
4. **El orden del pool.** Con las cuatro por debajo de −4.640 B, el bloqueo pasa
   a ser la posición: la primera diferencia de `.rodata` sigue en **+0x5C** en
   las cuatro, justo detrás del prefijo de bWare. El siguiente eje es **qué
   `#include` reordenar** para que el pool salga en el orden del objetivo, con el
   mismo protocolo que el brief §4 da para el `.text`.
5. **`zFe2`, `.data`**: hay que **quitar** los trece `g_pCustomize*` de esta
   unidad antes de escribir los 162 B de huecos (§5.3), o se pasa.
6. **No volver** a buscar un fichero ausente en `zSpeech` por `COPSPEECH.cpp`
   (§5.4) ni a meter `TuningSlider.cpp` en `zFe2` esperando `.text` (§5.3).

---

## 9. Herramientas (scratchpad, prefijo `c34dat2_`)

| guion | qué hace |
|---|---|
| **`c34dat2_try.py [--ldflags F] [--out N] [--control] <u…>`** | como `trypromo.py` pero **conserva** el `.elf`/`.dol` e imprime **la tabla de las 18 secciones del DOL** contra `orig/GOWE69/sys/main.dol`, con tamaño y desplazamiento. `--control` enlaza la lista base sin sustituir nada |
| **`c34dat2_cut.py <u> [--sec S] [--obj] [--elf F]`** | el estripado real: empareja por nombre nuestros símbolos con los del ELF enlazado y lee el desplazamiento acumulado. `--obj` lo hace sobre el objeto **extraído** (que pierde 0 B) |
| **`c34dat2_gap.py <u> [--sec S] [--detalle]`** | lo mismo repartido **símbolo a símbolo**, diciendo de cada uno si es local, si lleva reubicaciones dentro y si ya está en `keep.lst`. Separa las vtables duplicadas del estripado |
| **`c34dat2_keeppool.py <u…> [-o F]`** | el `keep.lst` del §3 |
| **`c34dat2_str.py <u> [--faltan] [--sobran]`** | multiconjunto de cadenas de las dos `.rodata`: **la lista de las que faltan es la lista de `#include` que hay que añadir** |
| `c34dat2_symdiff.py <u> [--sec S]` | símbolos de las dos secciones emparejados por nombre: cuál falta, cuál sobra, cuál tiene otro tamaño |
| `c34dat2_files.py <u> [--todos]` | los ficheros de fuente que el ORIGINAL tiene dentro del rango `.text` de la unidad (por `debug_lines.txt`), marcando los `.cpp` **que no están en la SourceList** |
| `c34dat2_apply.py <u…>` | pone el bloque de `#include` del §4, en binario e idempotente |
| `c34dat2_h.py <u…>` | huella y tamaño de cada sección de nuestro `.o` (ciclo rápido de A/B) |
| `c34dat2_elf.py` | lector de ELF que usan todos (secciones, símbolos, reubicaciones) |

Reutilizado de la r33: **`c33und3_uid.py --write`**, obligatorio detrás de
cualquier `build_direct.py zFe2`.

**Disco**: 18 GB libres al empezar y al acabar; borrados los 267 MB de `.elf`
de trabajo.
