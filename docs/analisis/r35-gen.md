# Ronda 35 — `gen`: las cuatro grandes, y por qué el `DOL OK` no estaba a 4 kB

Encargo: rematar `zSpeech` (−192 B), `zPhysics` (−384), `zFe` (−512) y `zFe2`
(−3.552) hasta `DOL OK`. **750.000 B esperando dato.**

**Ninguna llega a `DOL OK`, y la razón no es el dato: es que las cuatro tienen
funciones por debajo del 100 % y `DOL OK` es el sha1 del DOL entero.** Lo digo lo
primero porque cambia el encargo de la r36 (§1).

Lo que sí dejo: **el frente de `BuildMessageTable` cerrado y medido en las 33**
(§3), **la medida honesta del dato que falta**, que es casi el doble de la que
daba la r34 (§4), **el orden del pool de `zSpeech` puesto en el del objetivo**
(20 cadenas seguidas, §5) y **dos trampas de medida** que falseaban el
diagnóstico anterior (§7).

---

## 0. Verificación del encargo: reproduce EXACTO

Con la base tal cual la dejó la r34, y `main.dol` de control **`DOL OK`,
4.541.888 B, las nueve secciones a delta 0**:

| unidad | `.rodata` nuestra | del objetivo | DOL con `keep.lst` de `dat2` | sha1 de `dat2` |
|---|---|---|---|---|
| `zSpeech` | 12.568 | 12.712 | **−192** | `054dc852d5ab` ✔ |
| `zPhysics` | 17.384 | 16.064 | **−384** | `dbb933d67190` ✔ |
| `zFe` | 19.496 | 19.992 | **−512** | `8e478468a8fb` ✔ |
| `zFe2` | 22.536 | 25.704 | **−3.552** | `263cfa268399` ✔ |

Los cuatro sha1 salen **idénticos** a los de `r34-dat2.md`, y
`c34dat2_keeppool.py` regenera los cuatro `keep.lst` **byte a byte iguales** a
los que dejó `dat2`. La contabilidad de la r34 es correcta y reproducible.

`zFe2` volvió a necesitar el baile del `DECL_UID` (r34 §1) y hay un matiz que
falta en aquel informe: **`c33und3_uid.py --write` reescribe el `.cpp`, así que
hay que COMPILAR OTRA VEZ detrás**. La secuencia buena son tres pasos, no dos:

    python scripts/build_direct.py zFe2
    python scratchpad/c33und3_uid.py --write
    python scripts/build_direct.py zFe2      # <-- este faltaba

Sin el tercero el enlace da `ENLACE FALLA (16 errores)` y parece un fallo del
cambio que acabas de hacer. Me costó una vuelta entera.

---

## 1. `DOL OK` no estaba a 4 kB, y esto hay que decirlo

`trypromo.py` compara el **sha1 del DOL completo** contra
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`. Eso exige el `.text` **byte a byte**,
no del mismo tamaño. Y las cuatro tienen funciones sin cerrar (`pctsnap`):

| unidad | funciones < 100 % | B |
|---|---|---|
| `zFe` | `__17UIMemcardKeyboardP21ScreenConstructorData` 92,98 · `GetElapsedTime__9SubTitler` 95,52 · `FoundEntry__16MemcardCallbacks…` 97,97 | 796 |
| `zFe2` | `ToggleCapsLock__10FEKeyboard` 91,09 · `RenderString__8FEngFont…` 93,32 · `__12IconScrollerPCcN31f` 97,77 | 2.048 |
| `zPhysics` | `__9SmackableRCQ25UMath7Matrix4…` 99,26 | 3.112 |
| `zSpeech` | `LoadSpeechBank__Q26Speech7Manager…` 95,32 · `Setup__Q26Speech13RoadblockFlow` 98,26 | 908 |

Medido además de otra forma: con `zSpeech` a **−192 B** y el `keep.lst` de la
r34, el `.text1` del DOL difiere del objetivo en **1.024.477 B repartidos en
9.450 tramos**. Casi todo es consecuencia de que la `.rodata` va corta —cada
`lis/addi` que apunta detrás del hueco cambia—, pero el resto son esas funciones.

**Consecuencia para la r36**: ajustar el `keep.lst` para que la `.rodata` mida
exactamente lo mismo **no promociona nada**. El encargo de estas cuatro es
«cerrar N funciones **y** el dato», y las que están de verdad cerca son las que
`promodist.py` da con `fns=0`.

---

## 2. Lo que dejo modificado (5 ficheros, y ninguno compartido)

| fichero | qué |
|---|---|
| `src/Speed/Indep/SourceLists/zPhysics.cpp` | `#define ZMAIN_MESSAGES_LUA_INLINE` |
| `src/Speed/Indep/SourceLists/zFe.cpp` | ídem |
| `src/Speed/Indep/SourceLists/zFe2.cpp` | ídem |
| `src/Speed/Indep/SourceLists/zSpeech.cpp` | ídem **+ el bloque de `#include` reordenado** (§5) |
| `src/Speed/Indep/Src/Frontend/Database/VehicleDB.cpp` | **sólo** las 16 etiquetas alias que resincroniza `c33und3_uid.py` |

**No toco ninguna cabecera compartida ni `EventDefs.cpp`** — ver §3.1, que es
una nota de convivencia importante. Respaldos del estado de partida en
`scratchpad/c35gen_orig/`.

---

## 3. `BuildMessageTable`: el mecanismo está probado, y la palanca ya existía

El encargo pedía cerrar esto («si lo cierras, es el hallazgo de la ronda»).
**Está cerrado, y el mecanismo es exactamente el que describía `r34-dat2` §5.1.**

### 3.1 Convivencia: `r35ord4` ya lo había escrito, y por eso reverti lo mío

Escribí y apliqué mi propia versión (cuerpo **dentro** de la clase, guarda
`HERMES_INLINE_MESSAGE_TABLE`, `lua.h` en `Hermes.h`, 48 de los 65 cuerpos) y
funcionaba: `zPhysics` `.rodata` 17.384 → 17.400 y **`AnchorWorldID` nace justo
detrás de `MJumpCut`, igual que en el objetivo, con el `.text` de hash
IDÉNTICO**.

Al ir a por los 17 cuerpos que necesitan `LuaBindery`/`ISimable` descubrí que
**otro agente de esta misma ronda (`r35ord4`) ya tenía los 130 cuerpos
(`BuildMessageTable` **y** `HandleMessage_LuaBinding`) en las 66
`Generated/Messages/M*.h`**, bajo `#ifdef ZMAIN_MESSAGES_LUA_INLINE`, con sus
`#include` de `LuaBindery.h`/`LuaPostOffice.h`, y `EventDefs.cpp` envuelto en el
`#ifndef` correspondiente. Su marca es `/* r35ord4-z1 */`.

**Revertí quirúrgicamente mis 48 cabeceras y mi `EventDefs.cpp`** (no con una
copia en bloque, sino quitando sólo mi bloque de cada fichero, por si `ord4`
había seguido editando), dejé `Hermes.h` byte a byte como estaba, y **reutilicé
su guarda**. `git diff --stat` sobre `Generated/` vuelve a ser exactamente su
trabajo: 66 ficheros, 1.833 inserciones, **0 borrados**.

Aviso para quien integre: **la guarda se llama `ZMAIN_MESSAGES_LUA_INLINE` pero
ya no es sólo de `zMain`** — la definen también `zSpeech`, `zPhysics`, `zFe` y
`zFe2`. El comentario de las cabeceras («el único `#define` está en
`SourceLists/zMain.cpp`») ha dejado de ser cierto.

### 3.2 Lo que vale, medido

Con la guarda puesta en las cuatro, **base y medida seguidas**:

| unidad | `.rodata` base | con la guarda | `.text` |
|---|---|---|---|
| `zSpeech` | 12.568 | 12.800 (**+232**) | `191268/8e8c25b1` = **idéntico** |
| `zFe2` | 22.536 | 22.624 (+88) | `264280/347bac46` = **idéntico** |
| `zFe` | 19.496 | 19.568 (+72) | `189040/93dbe89a` = **idéntico** |
| `zPhysics` | 17.384 | 17.400 (+16) | `190484/964d7e89` = **idéntico** |

No es «mismo tamaño»: es el **mismo hash** de `.text`, y `.data` y `.bss` también.
Es el cambio más limpio que he visto en este proyecto: **sólo nace dato**. (La
tabla es la de la guarda sola; el reordenado del §5, que va sólo en `zSpeech`, sí
mueve bytes del `.text` sin cambiar su tamaño — ver §5.)

### 3.3 Extrapolación a las 33 (la regla de «un hallazgo no es un frente»)

Con la guarda ya activa en mis cuatro, quedan **196 literales de
`BuildMessageTable` que el objetivo tiene y nosotros no, 1.805 B en 17
unidades**. Sale sin compilar, comparando los literales de las cabeceras contra
las dos `.rodata`:

| unidad | cadenas | B | | unidad | cadenas | B |
|---|---|---|---|---|---|---|
| `zAI` | 34 | **332** | | `zWorld` | 11 | 95 |
| `zLua` | 29 | **281** | | `zCamera` | 10 | 89 |
| `zEAXSound` | 22 | 199 | | `zSim` | 8 | 69 |
| `zEAXSound2` | 21 | 193 | | `zFe2` | 8 | 67 |
| `zGameplay` | 17 | 175 | | `zAnim` | 6 | 52 |
| `zPhysicsBehaviors` | 13 | 117 | | resto (7) | 27 | 136 |

**Recomendación concreta**: `#define ZMAIN_MESSAGES_LUA_INLINE` en `zAI.cpp`,
`zLua.cpp`, `zEAXSound.cpp`, `zEAXSound2.cpp`, `zGameplay.cpp` y
`zPhysicsBehaviors.cpp` vale ~1,3 kB de las seis, y en mis cuatro no costó ni un
byte de código. Es cota superior: la guarda sólo obra sobre las cabeceras que el
TU ya parsea.

---

## 4. La medida honesta del dato que falta: no son 4.640 B, son 6.720

Esta es la corrección más importante que traigo del informe anterior.

El `keep.lst` de `c34dat2_keeppool.py` fuerza **todo** el dato muerto que cumple
las cuatro condiciones, **incluido el que el objetivo no tiene** (`dat2` lo dejó
avisado en su §3.1, pero la tabla de titulares no lo descontaba). Escribí
**`c35gen_keeptune.py`**, que descarta un candidato cuando su contenido es una
cadena entera y **el objetivo ya no tiene tantas copias**. Con eso:

| unidad | r34 (`keep` máximo) | ahora, `keep` máximo | ahora, **`keep` honesto** |
|---|---|---|---|
| `zSpeech` | −192 | **+96** (se pasa) | **−192** |
| `zPhysics` | −384 | −384 | **−448** |
| `zFe` | −512 | −448 | **−2.048** |
| `zFe2` | −3.552 | −3.456 | **−4.032** |
| **suma** | −4.640 | −4.192 | **−6.720** |

`zFe` es el caso claro: su `keep.lst` máximo fuerza **1.765 B en 44 símbolos que
el objetivo no tiene**, y por eso parecía estar a 512 B. **Su hueco real es de
2 kB.** La columna de la derecha es la que hay que perseguir; la de en medio
sólo dice «cuánto puedo tapar».

Y `zSpeech` **ya se pasa** con el `keep` máximo: se puede aterrizar en 0 exacto
quitando entradas a mano, pero **es cosmético** (§1) y no lo he hecho.

Ficheros: `scratchpad/c35gen_keep_<u>.lst` (máximo) y `c35gen_keept_<u>.lst`
(honesto). Los cuatro se pasan por `--ldflags` y **no mueven el `.text` del DOL**
(`text0/1/2` a 0 y desplazamiento 0 en las ocho medidas).

---

## 5. El orden del pool: `zSpeech` ya sale en el orden del objetivo

`r34-dat2` §8.4 lo dejó como «el siguiente eje». Está hecho para `zSpeech`.

La cabeza de la `.rodata` del objetivo dice qué cabeceras parseaba el TU y **en
qué orden**, y el bloque que dejó la r34 tenía otro:

    OBJETIVO                          NOSOTROS (r34)
    Attrib::Blob                      Attrib::Blob
    MGeneric                          STL
    Attrib::Gen::simsurface  <- r9    Attrib::Gen::milestonetypes  <- r9 (mal)
    Attrib::TAttrib                   Attrib::TAttrib
    Attrib::Gen::speech               Attrib::Gen::audioimpact
    Attrib::Gen::ecar                 Attrib::Gen::audioscrape
    Attrib::Gen::camerainfo           Attrib::Gen::camerainfo
    Attrib::Gen::effects              Attrib::Gen::effects
    Attrib::Gen::audioimpact          Attrib::Gen::ecar
    Attrib::Gen::audioscrape          Attrib::Gen::emitterdata   <- sobra
    WorldBodyConn … Pkt_Effect_…      Attrib::Gen::emittergroup  <- sobra
    Attrib::Gen::pvehicle             WorldBodyConn … Pkt_Effect_…
    Attrib::Gen::engine               EventSequencerSystems      <- sobra
    Attrib::Gen::engineaudio          Attrib::Gen::simsurface
    MAudioReflection                  Attrib::Gen::engineaudio
    PlayerNum · Dist · Covered        Attrib::Gen::pvehicle
    Attrib::Gen::audiosystem          Attrib::Gen::chassis       <- sobra
    Attrib::Gen::gameplay             Attrib::Gen::engine
    Attrib::Gen::milestonetypes

Reescribiendo el bloque en el orden del objetivo (16 `#include`, con
`MGeneric.h` delante de todo y `MAudioReflection.h` entre `engineaudio` y
`audiosystem`), **las 20 cadenas salen en el orden exacto del objetivo**,
`PlayerNum`/`Dist`/`Covered` incluidas — la ranura 9 pasa a ser `simsurface`,
que es lo que pide `r34-pre2`.

Métrica de orden (§7.2): `zSpeech` **234 → 257 cadenas en orden (79,6 → 87,4 %
del objetivo)**, con un tramo de **139 cadenas seguidas**. `zPhysics` 105→106,
`zFe2` 633→639, `zFe` igual.

Coste, y aquí sí hay uno: `zSpeech` queda en `.text=191268/5a563f05` — **el mismo
tamaño y otros bytes** (el `.bss` sube 16 B, de 7.444 a 7.460). Es el aviso de la
r34 §7.5: reordenar `#include` mueve el bloque diferido de plantillas. **No es
una regresión** —`measure.py --cmp` da +0 B / +0 funciones y `pctsnap --cmp`
`EMPEORAN: ninguna`— pero invalida el md5 del `.o` como prueba de «no he tocado
nada». Su `.rodata` acaba en 12.864 B (objetivo 12.712) y el DOL en **+96**.

**Lo que le queda a la cabeza de `zSpeech` son cuatro cadenas duplicadas, y
tienen dueño**: `bad_alloc` (0x60) y `GAMECUBE` (0x6c) **están dos veces** —una
en el prefijo de bWare escrito a mano y otra que emite `cc1plus`—, y lo mismo
`STL` y `TODO`. Son **24 B justo delante de `Attrib::Attribute`**, y por eso la
primera diferencia de `.rodata` está en `+0x5C`/`+0x60` **en las 31 unidades que
llevan el prefijo**. El objetivo no los duplica: en él esas cadenas SON las del
prefijo. Es el siguiente eje y no es de esta ronda (§8).

---

## 6. Verificación

- `build_direct.py`: **6 ok, 0 fallidas** (zFe, zFe2, zFeOverlay, zSpeech,
  zPhysics, zPhysicsBehaviors), repetido cinco veces.
- **`measure.py --cmp`, base y medida SEGUIDAS** (base = restaurar mis cuatro
  `.cpp` desde `scratchpad/c35gen_orig` y recompilar; medida = volver a ponerlos
  y recompilar): `1.116.872/1.127.344 B` en las dos, **+0 B, +0 funciones,
  0 unidades cambian**.
- **`pctsnap.py --cmp`** sobre las cuatro (3.649 funciones): **`EMPEORAN:
  ninguna`** · `MEJORAN: 0`.
- **`audit.py`, DOS pasadas** en las cuatro: **0 `FALLA`** en las ocho.
- `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- `git status --porcelain config/ configure.py build.ninja objdiff.json`:
  `keep.lst`, `splits.txt` y `configure.py` salen modificados **y no son míos**
  (el diff de `keep.lst` son renombres `zAI.o:gap_08_…` →
  `auto_08_….o:pad_08_…`, de otro agente). **No he tocado ninguno.**
- `keepchk.py`: **26 RANCIAS** (la r34 dejó 21). Las cinco nuevas son de
  `zEAXSound2`, `zLua` y `crt0` — de quien está tocando `keep.lst`, no mías: mis
  listas viven en el scratchpad y se pasan por `--ldflags`.
- **Sin commit. Sin una sola instrucción de ensamblador.**

### 6.1 El control se rompió a mitad de ronda, y NO es mío

`c34dat2_try.py --control` daba `DOL OK` al empezar y **`DOL ROTO
(0a7cb069f2e2)` al acabar**, con **las nueve secciones a delta 0 y
desplazamiento 0**: mismo tamaño, otro contenido. El control **no sustituye
ningún objeto**, así que no puede depender de mis cuatro unidades (ninguna está
promocionada). La causa está a la vista: en los 25 minutos anteriores otro
agente recompiló **48 objetos nuestros**, entre ellos los promocionados
`zDynamics`, `zMission`, `zDebug`, `zOnline` y varios de `libc`.

**Qué queda validado y qué no**: todas mis cifras son **deltas de TAMAÑO por
sección**, y el control confirma que los tamaños siguen bien (9 secciones a 0).
Lo que no se puede juzgar mientras el árbol esté así es un sha1. Antes de la
promoción de la r36 hay que **repetir el control** y exigir `DOL OK`.

Durante la ronda me encontré además con `zPhysicsBehaviors` fallando a compilar
por una edición en vuelo de `Main/EventSequencer.h`; a los dos minutos compilaba.
**Mira la base antes de acusar a tu cambio** sigue siendo la regla que más paga.

---

## 7. Trampas y vedas nuevas

### 7.1 `c34dat2_str.py` pierde cadenas y falsea el diagnóstico

Parte la sección por `\0` y descarta el trozo si no es imprimible **entero**.
Toda cadena precedida por bytes de pool flotante se pierde: en `zPhysics` daba
`'CameraMessagePort'` como **AUSENTE** teniéndola (está en `0x628`, detrás de
`3e e4 e1 1e`). Lo mismo con `'AUD:Relocated speech headers'`, que aparecía a la
vez en FALTAN y en SOBRAN (como `'oAUD:…'`).

**`scratchpad/c35gen_str.py`** lo hace como `strings`: una cadena empieza tras
**cualquier** byte no imprimible. En `zSpeech` la cuenta pasa de «faltan 36»
a «faltan 18». **Las listas de «cadenas que faltan» de la r34 llevan ruido.**

### 7.2 `datacmp.py` mide POSICIÓN, no orden: no sirve para juzgar este frente

`r34-dat2` §4.3 usó la subida de `datacmp` (41,0 → 71,3 % en `zSpeech`) como
prueba de que sus `#include` eran los buenos. El argumento no aguanta al revés:
`datacmp` compara byte a byte **por posición**, así que **meter 16 B correctos en
medio hunde el porcentaje** aunque el orden mejore. Mi `zSpeech`, con el orden
puesto en el del objetivo y 20 cadenas seguidas coincidiendo, **baja de 71,5 %
a 43,2 %**.

Métrica sustituta, invariante al desplazamiento: **`scratchpad/c35gen_ord.py`**,
subsecuencia común más larga (LCS) de las **secuencias de cadenas** de las dos
`.rodata`. Estado actual:

| unidad | en orden | del objetivo | tramo mayor | `datacmp` |
|---|---|---|---|---|
| `zSpeech` | 257 | **87,4 %** | 139 | 43,2 % |
| `zFe` | 440 | 74,5 % | 49 | 48,0 % |
| `zFe2` | 639 | 66,7 % | 47 | 21,8 % |
| `zPhysics` | 106 | 57,6 % | 37 | 71,9 % |

`zPhysics` es la prueba de que las dos métricas miden cosas distintas: la mejor
en `datacmp` y **la peor en orden**.

### 7.3 Vedas que confirmo o abro

- **`c33und3_uid.py --write` exige recompilar detrás** (§0). No estaba escrito.
- **La ganancia de un `#include` que sólo interna cadenas es invisible en el DOL
  sin `keep.lst`**: las cadenas nacen muertas y el enlazador se las lleva. Ya
  estaba en la r34 §4.2 y lo confirmo en las cuatro.
- **Los 15 nombres de `VehicleSystem` (`ROLLING_RESISTANCE`, `PAD_DEAD_ZONE`…)
  no son un `#include` que falte**: contados sobre los 33 objetos extraídos,
  **sólo `zPhysics` los tiene**, en un bloque contiguo en `0x830..0x954`, y lo
  mismo `SceneryOverrideConn`/`Pkt_SceneryOverride_Open`. Son de
  `VehicleSystem.cpp`, cuyos `InitializeGlobals()`/`InitializeVehicleGlobals()`
  son cuerpos vacíos en nuestro árbol y a los que `debug_lines` atribuye **10
  instrucciones** en el rango de `zPhysics`. **Es código, no un `#include`**;
  no gastéis un ensayo buscando la cabecera.
- **`GRaceStatus` sí es un cuerpo que debería estar en la cabecera**:
  `GRaceStatus.cpp:301` hace `return "GRaceStatus";` y `debug_lines` mete
  `src/Gameplay/GRaceStatus.h` (8 instrucciones) en el rango de `zPhysics`.
  Mismo patrón que `BuildMessageTable`, distinto fichero.

---

## 8. Lo que haría el siguiente, por orden

1. **Las cuatro grandes son encargo de CÓDIGO + dato, no de dato.** Siete
   funciones (§1) y 6.720 B honestos (§4). La de mejor relación es `zPhysics`:
   una sola función al 99,26 % y 448 B.
2. **`#define ZMAIN_MESSAGES_LUA_INLINE` en `zAI`, `zLua`, `zEAXSound`,
   `zEAXSound2`, `zGameplay` y `zPhysicsBehaviors`** (§3.3): ~1,3 kB medidos, y
   en mis cuatro costó **cero bytes de `.text`**. Es lo más barato que queda.
3. **El duplicado del prefijo de bWare** (§5): `bad_alloc`, `GAMECUBE`, `STL` y
   `TODO` salen dos veces, 24-28 B **justo delante de `Attrib::Attribute`**, y
   son la razón de que la primera diferencia de `.rodata` esté en `+0x5C` **en
   las 31 unidades que llevan el prefijo**. Si se consigue que `cc1plus` los
   emita una sola vez y en el orden del objetivo —receta de la r33 §1: escribir
   a mano **sólo** lo que el compilador no genera, que aquí es la ruta
   `d:/mw/…/bware.hpp` y las dos cadenas de formato—, se realinea la cabeza de
   la `.rodata` de medio proyecto de una vez.
4. **Repetir el bloque de `#include` en el orden del objetivo para `zFe`,
   `zFe2` y `zPhysics`**, como hice con `zSpeech` (§5). Se lee directamente del
   volcado de cadenas del objeto extraído; `c35gen_ord.py --dif` dice dónde
   rompe.
5. **Medir con `c35gen_ord.py`, no con `datacmp.py`**, mientras se toque el
   orden (§7.2).
6. **No** volver a dar por buena una lista de «cadenas que faltan» hecha con
   `c34dat2_str.py` (§7.1).

---

## 9. Herramientas nuevas (scratchpad, prefijo `c35gen_`)

| guion | qué hace |
|---|---|
| **`c35gen_ord.py <u> [--dif]`** | calidad del **orden** del pool: LCS de las dos secuencias de cadenas, invariante al desplazamiento. La métrica que hay que usar cuando se mueve un `#include` |
| **`c35gen_keeptune.py <u> [-o F] [--ver]`** | `keep.lst` **honesto**: como `c34dat2_keeppool.py` pero sin forzar el dato muerto que el objetivo no tiene. Da el hueco real |
| **`c35gen_str.py <u> [--faltan] [--sobran]`** | multiconjunto de cadenas con extracción tipo `strings` (§7.1) |
| `c35gen_dolcmp.py <dol> [sec]` / `c35gen_dolcmp2.py` | dónde difiere nuestro DOL del original, sección a sección y por tramos |
| `c35gen_strord.py <u> obj\|src [lo] [hi]` | volcado de las cadenas de una `.rodata` **en orden de dirección**: es lo que se lee para reordenar los `#include` |
| `c35gen_bmt.py [--todos] [--revert]` | mueve `X::BuildMessageTable` de `EventDefs.cpp` a dentro de la clase. **Escrito y revertido** (§3.1): lo dejo porque el patrón sirve para el mismo trabajo con otras cabeceras generadas |

Reutilizadas de la r34: `c34dat2_try.py`, `c34dat2_gap.py`, `c34dat2_keeppool.py`,
`c34dat2_files.py`, `c34dat2_elf.py`, y `c33und3_uid.py --write` (con el tercer
paso del §0).
