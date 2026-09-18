# r61 — `resto` (zSpeech, zLua, zAI): el orden del pool es el orden del flujo preprocesado

Agente `resto`. **Seis ficheros tocados, los seis exclusivos de mis unidades**, ninguna
cabecera compartida, nada de `config/`, nada de `scripts/`:

    src/Speed/Indep/SourceLists/zSpeech.cpp
    src/Speed/Indep/SourceLists/zLua.cpp
    src/Speed/Indep/SourceLists/zAI.cpp
    src/Speed/Indep/Src/EAXSound/Stream/SpeechManager.cpp   <- solo la incluye zSpeech.cpp
    src/Speed/Indep/Src/Speech/SoundAI.cpp                  <- solo la incluye zSpeech.cpp
    src/Speed/Indep/Src/Speech/SpeechCache.cpp              <- solo la incluye zSpeech.cpp

(los tres `.cpp` de `Src/` los comprobé con `grep -rln` antes de tocarlos: **la única
SourceList que los incluye es `zSpeech.cpp`**.)

---

## 0. Titular

Métrica: **bytes distintos en los rangos de `splits.txt` de la unidad**, contra el DOL
original, enlazando el proyecto entero y sustituyendo **una** unidad, con una **copia
privada de `keep.lst`** re-resuelta por contenido (`lcfix`) más **7 entradas nuevas**
(§6). Entre paréntesis, la dirección de la **primera** diferencia de `.rodata`, que es
la métrica de escalera.

| unidad | antes | después | gana | prefijo exacto de `.rodata` | `fncmp` |
|---|---:|---:|---:|---|---|
| **`zSpeech`** | 11.753 | **6.886** | **−4.867** | 804062E4 → **804074EC** (**+4.616 B**) | 1/703 → 1/703 |
| **`zLua`** | 9.477 | **8.157** | **−1.320** | 803ED5B5 → **803ED5C1** (+12 B) | 0/537 → 0/537 |
| **`zAI`** | 37.443 | **35.184** | **−2.259** | 803C8E51 → 803C8E51 (0) | 0/1030 → 0/1030 |
| **total** | **58.673** | **50.227** | **−8.446** | | **cero regresiones de código** |

Las tres quedan con **las nueve secciones del ELF a delta 0** (antes: `zLua` rodata−480,
`zAI` rodata−272, `zSpeech` IGUAL). `zSpeech` además deja su `.data` en **0 B distintos**
(eran 199).

Sello, **tres compilaciones seguidas del árbol final, las tres iguales**:

    zSpeech  4f8c7b4e71f20b07d34b7b033ea9b4537d2ff544   (x3, y otras x3 antes de los comentarios: el mismo)
    zLua     0bf04a34791ca896c148f6c4596bf60ed7bd1927   (x3)
    zAI      d4dc241c02462a7fdc07621a97047d4edbefcddc   (x3)

**Ninguna promociona.** `trypromo zSpeech zLua zAI` → `DOL ROTO (7c3134c2639d)`.

> **AVISO QUE HAY QUE LEER ANTES DE MEDIR NADA MÍO.** Con la `keep.lst` que hay HOY en
> `config/` mis unidades salen **PEOR** que la base (`zSpeech` 16.315, `zLua` 9.454, y
> las dos con la sección `.rodata` corrida −304/−296). **Todo mi trabajo depende de dos
> cosas del §6: `lcfix.py` (133 correcciones mías) y 7 líneas nuevas de `keep.lst`.**
> Sin las dos, esto es una regresión, no una mejora.

---

## 1. EL HALLAZGO: `orden de los $LC` == `orden de primera aparición en el .i`

Es una ley, no una heurística, y se comprueba en un segundo. **El orden en que cc1plus
emite la `.rodata` de una unidad es exactamente el orden en que el literal aparece por
primera vez en la salida del preprocesador** — no el orden de las funciones en `.text`,
no el orden de emisión de las funciones que lo usan, no el orden de las vtables.

Medido en dos unidades, 18 literales, 18 de 18 monótonos:

| `zSpeech`, línea del `.i` | fichero:línea | offset en NUESTRA `.rodata` |
|---:|---|---:|
| 74.730 | `Classes/presetride.h:19` | 1152 |
| 77.684 | `Frontend/Database/FEDatabase.hpp:259` (`SMS_MESSAGE_%d`) | 1176 |
| 78.758 | `Classes/acceltrans.h:24` | 1260 |
| 79.005 | `Classes/shiftpattern.h:58` | 1284 |
| 79.603 | `Classes/turbosfx.h:26` | 1312 |
| 81.122 | `Messages/MGamePlayMoment.h:19` | 1336 |
| 81.205 | `Messages/MGamePlayMoment.h:102` (`Position`) | 1352 |
| 81.248 | `Messages/MPursuitBreaker.h:18` | 1408 |
| 86.185 | `Messages/MUnspawnCop.h:24` | 1440 |
| 88.746 | `Stream/SpeechManager.cpp:377` (`AUD:GameSpeech`) | 1488 |

| `zLua`, línea del `.i` | fichero | offset |
|---:|---|---:|
| 41.227 / 44.035 / 46.195 / 46.696 / 46.962 / 47.222 / 47.505 / 47.812 | pvehicle / simsurface / chassis / engine / induction / nos / tires / transmission | 640 / 664 / 688 / 712 / 732 / 756 / 776 / 796 |

**Lo que esto rompe:** `AUD:GameSpeech` lo usa `Speech::Manager::Init`, que está en
`.text+0xB22` (casi al principio); `MGamePlayMoment` lo usa `_GetKind__15MGamePlayMoment`,
que está en `.text+0x2C92A` (casi al final). Y sin embargo `MGamePlayMoment` se interna
**antes**. O sea que **el punto de uso dentro de `.text` no dice nada del orden del pool**:
manda dónde se PARSEA. Esto explica por qué el «primer de pool» funciona con una
`static inline` que no se emite, y por qué sólo mueve hacia atrás.

**Lo que esto abre:** la escalera de `.rodata` deja de ser adivinanza. El procedimiento
es mecánico:

1. `rodump.py <unidad> obj` y `rodump.py <unidad> src` → el orden que quiere el objetivo
   y el que tenemos.
2. `pp.py <unidad> '"literal"' ...` → dónde se parsea hoy cada literal que sobra o falta.
3. mover el `#include` (o bloquearlo con su propia guarda y re-incluirlo en su sitio).

`pp.py` preprocesa la SourceList con sus cflags REALES (los saca de `build_direct.parse_units`)
y da fichero+línea de la primera aparición. Cuesta ~20 s por unidad.

---

## 2. La palanca principal: **mover el `#include`, no la sentencia**

Tres de las cuatro ediciones de `zSpeech` son esto, y valen los 4.867 B.

### 2.1 `SFXObj_MomentStrm.hpp` (ocho literales, 104 B)

`SpeechManager.cpp:9` incluía `SFXObj_MomentStrm.hpp`, que arrastra `MGamePlayMoment.h`
y `MPursuitBreaker.h` (sus líneas 8 y 9). Sus ocho literales —`MGamePlayMoment`,
`Position`, `Vector`, `Velocity`, `hSimable`, `AttribKey`, `MPursuitBreaker`,
`StartBreaker`— caían entre `SMS_MESSAGE_%d_SUBJECT` y `AUD:GameSpeech`. El objetivo los
tiene 0x198 B más adelante, entre `"PlayStart"` (`SpeechManager.cpp:1899`) y
`"AUD: SED_NISSFX events"` (`:2201`).

**Receta, y las dos trampas que cuestan una compilación cada una:**

```c
/* :9, delante del include */
#define GENERATED_MESSAGES_MGAMEPLAYMOMENT_H
#define GENERATED_MESSAGES_MPURSUITBREAKER_H
class MGamePlayMoment;                 /* SFXObj_MomentStrm.hpp:43,44 las usa */
class MPursuitBreaker;                 /* SOLO POR REFERENCIA */
#include ".../SFXObj_MomentStrm.hpp"

/* :2123, delante del bloque de SED_NISSFX */
} // namespace Speech            <-- TRAMPA 2
#undef GENERATED_MESSAGES_MGAMEPLAYMOMENT_H
#undef GENERATED_MESSAGES_MPURSUITBREAKER_H
#include ".../MGamePlayMoment.h"
#include ".../MPursuitBreaker.h"
namespace Speech {
```

* **TRAMPA 1 (NEGATIVO MEDIDO):** mover el `#include` ENTERO de `SFXObj_MomentStrm.hpp`
  a :2123 **NO COMPILA**. `SoundAI.h` (que `SpeechManager.cpp:31` incluye) arrastra
  `Observer.h`, y `Observer.h:100` declara `MessageBlewPastCop(const MGamePlayMoment &)`.
  Hay que dejar la declaración adelantada arriba y bajar sólo las dos cabeceras.
* **TRAMPA 2 (NEGATIVO MEDIDO):** el `#include` de :2123 cae **dentro de
  `namespace Speech {` (1550..2465)**. Ahí GCC crea `Speech::MGamePlayMoment`, y
  `Observer.cpp:804` y `SoundAI.cpp:164` dejan de casar («prototype ... does not match
  any in class», «no method `MGamePlayMoment::_GetKind`»). Hay que cerrar y reabrir el
  namespace alrededor del include.

### 2.2 `MUnspawnCop.h` (48 B) — con una guarda que ya estaba en el árbol

`SoundAI.h:14` ya reserva `SOUNDAI_H_FWD_MUNSPAWNCOP` (la puso una ronda anterior para
zMain) y trae la declaración adelantada gratis. Basta con `#define`-arla en `zSpeech.cpp`
y añadir el `#include` en `SoundAI.cpp:37`, **detrás de `MPerpBusted.h` y `MRestartRace.h`,
que es el orden exacto del objetivo** (`.rodata` 0x80407A38..0x80407A70:
`MPerpBusted`, `Perpetrator`, `ISimable`, `MRestartRace`, `MUnspawnCop`, `CopHandle`, `Param`).

### 2.3 `zLua`: retrasar `pvehicle.h`/`engine.h` con su propia guarda

Mismo mecanismo, guardas `_attrib_gen_pvehicle_h` / `_attrib_gen_engine_h`, y la
declaración adelantada la escribo yo porque **`PhysicsInfo.hpp:63-79` usa
`Attrib::Gen::pvehicle &` y `engine &`** (sin ella son 17 `parse error before '&'`):

```c
namespace Attrib { namespace Gen { class pvehicle; class engine; } }
```

y el bloque de destino va **justo detrás de `#include WorldConn.h`**, en el orden del
objetivo: `speech.h`, `pvehicle.h`, `engine.h`, `engineaudio.h`, `MAudioReflection.h`.

---

## 3. La segunda palanca: **el argumento de depuración que el release tira**

Tres literales muertos de `zSpeech` que el objetivo tiene y nuestro árbol no emitía
salen de sobrecargas `inline` que en release **tiran el nombre y llaman a la versión
corta**. O sea: **mismo código, byte a byte, y el literal se interna en el punto exacto.**

| dónde | antes | después | qué literal coloca |
|---|---|---|---|
| `SpeechManager.cpp:1651` | `bMalloc(nfilesize, 0x1040)` | `bMalloc(nfilesize, "AUD_tmp: Game speech index", 0, 0x1040)` | 0x804063D4 |
| `SpeechManager.cpp:2255` | idem | `bMalloc(nfilesize, "AUD_tmp: SED_NISSFX index", 0, 0x1040)` | 0x80406500 |
| `SpeechCache.cpp:83` | `BNEW VoiceIDs` | `new ("Speech Cache speaker list", 0) VoiceIDs` | 0x80406568 |
| `SpeechCache.cpp:~253` | `const char *allocname;` (¡sin inicializar!) | `= "Speech::Cache::Alloc"` | 0x804065C4 |

Las declaraciones que lo permiten, y son de `bWare.hpp`, o sea que **valen para todo el
árbol**:

    bWare.hpp:97   inline void *bMalloc(int size, const char *debug_text, int debug_line, int params)
                   { return bMalloc(size, params); }
    bWare.hpp:145  inline void *operator new(size_t size, const char *file, int line)
    bWare.hpp:64   #define BNEW new (__FILE__, __LINE__)

**`BNEW` es un generador de parásitos.** En `SpeechCache.cpp` internaba el `__FILE__` de
la unidad —una cadena que el objetivo NO tiene— y `size & ~7` dejaba su cadáver `"pp"`
**exactamente en 0x80406568, que es donde el objetivo quiere `"Speech Cache speaker list"`**.
Cambiar `BNEW` por el `operator new` con nombre mata el parásito y coloca la cadena buena
de una vez. **Sospechoso en cualquier unidad con `BNEW` y un cadáver que acabe en `pp`.**

Y una regla general que sale de aquí: **un `bMalloc`/`bNewSlotPool`/`new` de nuestro
árbol al que le falte el argumento de nombre es un literal del objetivo perdido.** El
decompilador se los comió porque no generan código.

---

## 4. Las otras dos palancas, reutilizadas

* **Etiquetas de asignador parásitas** (receta r60 §3.1, `#undef ATTRIB_TAG` alrededor de
  los `#include` de las clases). Censo por unidad, contra la `.rodata` EXTRAÍDA del
  objetivo:

  | unidad | apagadas | cadáver que quitan |
  |---|---|---|
  | `zSpeech` | presetride, acceltrans, shiftpattern, turbosfx | `"n"` 2 B + `"bosfx"` 6 B en 0x804062E4 |
  | `zLua` | chassis, induction, nos, tires, transmission | `"ssis"`, `"uction"`, `"os"`, `"es"`, `"n"` en 0x803ED5D8 |
  | `zAI` | **sólo** induction y transmission | `"uction"` 7 B + `"n"` 2 B |

  **OJO en `zAI`:** de las seis de la cadena de `PhysicsInfo.hpp`, el objetivo **SÍ tiene
  cuatro** (chassis, engine, nos, tires). Apagarlas todas en bloque, como en zLua, sería
  destruir contenido bueno. **El censo hay que hacerlo por unidad**, con
  `live.py <unidad> "<cadena>"` (dice VIVA/MUERTA, si `keep.lst` la nombra, y si el
  objetivo la tiene).

* **Semilla de pool de la familia F** (`if (0) { <local float> = <constante>; }`), usada
  una vez en `SpeechManager.cpp` (`GameSpeech::Update`, detrás de `float fvol;`): el
  objetivo tiene el `0.0f` de esa función en `lbl_80406424`, DELANTE del `0x4330../0x8000..`
  del `int->float`; nosotros lo creábamos en el `< 0.0f` de :1860 y salía detrás de
  `100.0f`, metiendo 4 B entre `0x42c80000` y `"PlayStart"`. **Cero instrucciones, cero
  bytes, y la escalera salta +304 B.** La palanca de la familia F funciona igual fuera de
  zEAXSound2.

* **Relleno de compensación al final de la `.rodata`** (regla operativa de la r60). `zLua`
  estaba en rodata−480 y `zAI` en rodata−272, y eso corre `.data` y `.bss` enteras y
  fabrica miles de diferencias que no son de la unidad. Con el relleno puesto:

  | unidad | `.text` distintos | `.data` distintos | total |
  |---|---|---|---|
  | `zLua` sin relleno | 1.411 | 515 | 9.477 |
  | `zLua` con relleno | **730** | **35** | **8.210** |
  | `zAI` sin relleno | 21.015 | 623 | 37.443 |
  | `zAI` con relleno | **19.589** | **318** | **35.194** |

  **No es contenido bueno** y está escrito así en el fuente: cada byte de contenido de
  verdad que se coloque hay que descontarlo de ahí. Pero sin él ninguna otra cifra de
  esas dos unidades se puede leer, y son 3.526 B de DOL de regalo.

---

## 5. Negativos medidos (los tres están escritos en el fuente, donde `previo.py` los ve)

1. **`zSpeech` — colocar una cadena MUERTA sin su entrada de `keep.lst` es PEOR que
   dejarla en el bloque de cola.** `"Speech::Cache::Alloc"` en `Cache::Alloc`, sin
   entrada: **10.380 → 10.762 B** (el estripado deja el cadáver `"lloc"` y corre todo el
   pool de detrás). Con la entrada: **10.762 → 6.886 B**. Es el mismo hecho dos veces:
   la cadena buena y su entrada de `keep.lst` son **una sola edición**, y medir la mitad
   da el signo contrario.
2. **`zLua` — adelantar `speech.h`/`engineaudio.h`/`MAudioReflection.h` SIN retrasar
   `pvehicle.h`/`engine.h`: 8.202 → 8.279 B, PEOR en 77 B.** Y con las cinco pero en el
   orden equivocado (pvehicle/engine delante de speech): 8.224 B y la primera diferencia
   clavada en 0x803ED5B5. El orden correcto —speech, pvehicle, engine, engineaudio,
   MAudioReflection— da 8.157 y 0x803ED5C1. **El bloque es atómico y el orden dentro
   importa.**
3. **`zSpeech` — las dos trampas del §2.1**, las dos con su error de compilación exacto.

Y un negativo de la r60 que **queda CADUCO y hay que borrar de `zSpeech.cpp`**: «un
TERCER primer con SMS_MESSAGE / AUD:GameSpeech … no mueve la primera diferencia … hay que
sacarlo por otra vía, no con un primer». La otra vía era ésta, y está medida.

---

## 6. Lo que pido, con la cifra — **y sin esto mis unidades REGRESAN**

### 6.1 `lcfix.py` — **133 correcciones mías**

    zLua 69   zSpeech 33   zAI 31

Al empezar la ronda `lcfix --check zLua zAI zSpeech` daba **0 CORRIGE**, así que las 133
son mías. **Ninguna es venenosa para otra unidad**: `lcfix` sólo reescribe entradas cuya
directiva `# @lc` nombra a esa unidad. Lo que sí es venenoso es **no aplicarlas**: sin
`lcfix`, `zSpeech` pasa de 6.886 a 16.315 B y su `.rodata` del enlace sale −304.

En el mismo `--check` salen **153 correcciones que NO son mías** (zFe 88, zFe2 63,
zEagl4Anim 2). No las he mirado; son de otros agentes de esta ronda.

### 6.2 **SIETE líneas nuevas de `keep.lst`** (rule 4: las propongo, no las aplico)

Todas son cadenas MUERTAS que el objetivo SÍ tiene y que ahora se emiten en su dirección
exacta. Los `$LC` están resueltos contra el `.o` sellado de arriba, y llevan su directiva
`# @lc` para que `lcfix` los mantenga:

```
# @lc zSpeech "AUD_tmp: Game speech index"
zSpeech.o:$LC436
# @lc zSpeech "AUD_tmp: SED_NISSFX index"
zSpeech.o:$LC459
# @lc zSpeech "Speech Cache speaker list"
zSpeech.o:$LC464
# @lc zSpeech "SpeechLoadCBData"
zSpeech.o:$LC466
# @lc zSpeech "Speech::Cache::Alloc"
zSpeech.o:$LC469
# @lc zLua "Dist"
zLua.o:$LC475
# @lc zLua "Covered"
zLua.o:$LC476
```

De las siete, la que más pesa es `"Speech::Cache::Alloc"`: sola vale **−3.876 B** y
**+3.880 B de prefijo** (§5.1).

### 6.3 Limpieza opcional, MEDIDA COMO NEUTRA

Las tres entradas `# @lc <unidad> "GAMECUBE"` (zLua, zSpeech, zAI) siguen fallando
(«no tiene símbolo `$LC` propio»). **Borrarlas no cambia ni un byte**: medí `zSpeech` con
mi `keep.lst` privada con y sin ellas y da **6.886 B en los dos casos**. Es limpieza, no
arreglo — igual que dijo la r60.

### 6.4 Herramientas que merecen subir a `scripts/`

Están en `scratchpad/resto61/` (391 kB, cero volcados: los 123 MB de `.i` borrados):

| fichero | qué hace |
|---|---|
| **`pp.py`** | preprocesa una SourceList con sus cflags reales y da **fichero:línea de la primera aparición** de cada literal. Es la herramienta de la ley del §1. |
| **`rodump.py`** | la `.rodata` de un `.o` (`obj` = objetivo, `src` = nuestro) en orden de offset, con su símbolo. |
| **`live.py`** | por cadena: en qué `$LC` está, VIVA/MUERTA, si `keep.lst` la nombra, y **si el objetivo la tiene**. El censo de parásitos en una línea. |
| **`rangos.py`** | la métrica de este informe: enlaza con la unidad puesta y cuenta bytes distintos **dentro de sus rangos de `splits.txt`**, con la primera diferencia por sección. `KEEPLST=` para usar una `keep.lst` privada. |
| **`secs.py`** | delta de tamaño de las nueve secciones del ELF enlazado. Es el control obligatorio antes de leer `rangos`. |
| **`lcpriv.py`** | copia privada de `keep.lst` re-resuelta por contenido, **sin tocar `config/`**. Es lo que hace medible una unidad sin pisar a los demás agentes. |
| `cmpstr.py`, `hexcmp.py`, `whoref.py`, `deadsym.py`, `mkkeep.sh` | comparadores lado a lado y quién referencia un `$LC`. |

La r59 y la r60 ya pidieron subir `lcpriv/rangos`. **Van tres rondas reescribiéndolas.**

---

## 7. El siguiente paso de cada unidad, con la dirección exacta

* **`zSpeech` — 0x804074EC**, y el objetivo quiere `"GManager"` (9 B).
  Sale de `Gameplay/GManager.h:85`, `return DEAD_STR("GManager")`, o sea que es cuestión
  de dónde se parsea `GManager.h`. Detrás vienen `MForcePursuitStart`, `MinHeatLevel`,
  `MPerpBusted`, `Perpetrator` (0x804079DC..0x80407A38), que siguen en el bloque de cola.
* **`zLua` — 0x803ED5C0**, y el objetivo quiere `"16.1.0"`, `"1.8.1"`, `"GRaceStatus"`,
  `"LuaBindery"`. Las tres primeras salen de `Gameplay/GRaceDatabase.h:209-221`, donde
  nuestro árbol tiene TRES `DEAD_STR` y el original tiene CINCO, en otro orden
  (16.1.0, 16.2.1, 1.2.3, 1.8.1, 19.8.31). **`GetDDayStartRace()` devuelve
  `sDDayRaces[0]` y en el original casi seguro devolvía `DEAD_STR("16.1.0")`** — pero eso
  es cabecera COMPARTIDA (zFe, zSim, zGameplay, zMain, zAnim) y **no lo he tocado**: hay
  que decidirlo en ventana, o encontrarle una guarda inerte como `DEAD_STR_DONE`.
* **`zAI` — 0x803C8E44**, y el objetivo quiere `Attrib::Gen::world`, `MGeneric`,
  `MSpawnTraffic`, `InitialPos`, `InitialVec`, `VehicleName`, `gameplay`,
  `milestonetypes` y **luego** las versiones. Las seis primeras están HOY en el bloque
  `.asciz` de cola de `zAI.cpp` (offsets 7148..7224 de nuestra `.rodata`) y hay que
  partirlas y colocarlas con `.balign 4` en el punto de parseo — que es justo delante de
  donde se interna `Attrib::Gen::chassis` (offset 500). `zAI` es la unidad con más botín
  pendiente: 15.279 B de `.rodata` y 19.587 de `.text` que son **todos** desplazamiento
  de pool, porque su código está al 0 de 1030.

---

## 8. Sorpresas

1. **El orden del pool es el del `.i`, y punto.** Ni `.text`, ni orden de emisión, ni
   vtables. Se comprueba con `ngccc -E` en 20 s y convierte la escalera de `.rodata` en
   un procedimiento mecánico. Es el mismo hecho que hacía funcionar el «primer de pool»
   sin que nadie lo hubiera enunciado.
2. **Un `#include` en la línea equivocada vale 4.616 B de prefijo.** No hubo que escribir
   una sola línea de código: se movieron dos cabeceras y se cambiaron cuatro llamadas por
   su sobrecarga con nombre.
3. **`BNEW` fabrica parásitos.** `new (__FILE__, __LINE__)` interna la ruta del `.cpp`, y
   su cadáver `"pp"` cayó exactamente encima de la cadena que el objetivo quería ahí.
4. **Un argumento que el release tira es un literal del objetivo perdido.** `bMalloc` de
   4 argumentos, `operator new(size, name, line)`: el decompilador se los comió porque no
   generan código, y con ellos el pool se coloca solo.
5. **Un `const char *` sin inicializar era una cadena del objetivo.** `Cache::Alloc` tenía
   `const char *allocname;` a pelo. Era `"Speech::Cache::Alloc"`, y valía 3.876 B.
6. **Media edición tiene el signo contrario.** Colocar la cadena sin su `keep.lst` empeora
   382 B; con ella mejora 3.876. Cualquier medida de «colocar cadenas muertas» que no
   incluya la entrada de `keep.lst` en el mismo experimento está midiendo el revés.
7. **`deadstr.py` no sirve en estas cuatro unidades y ya lo avisaba el encargo**: propone
   62/41/68 cadenas y `comm -23` contra `keep.lst` deja **CERO nuevas en las tres**. La
   veta de cadenas muertas por contenido está agotada aquí; lo que queda es COLOCACIÓN.
8. **`#include` dentro de un `namespace` es una bomba silenciosa.** GCC crea el tipo en el
   namespace equivocado y el error sale 300 líneas más abajo, en otro fichero.
