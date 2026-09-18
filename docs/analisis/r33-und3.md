# Ronda 33 — `und3`: **zFe2 ENLAZA** (250.732 B), y por qué GCC 2.9 no emite las clases locales

Encargo: `zFe2`, la SourceList más grande bloqueada — **250.732 B**, `ENLACE
FALLA` con **101 errores**.

**Resultado: zFe2 enlaza.** Los 101 errores están cerrados con **siete
definiciones de datos, un `static` retirado y una etiqueta de alias**, todo en
`src/`, **sin tocar `configure.py` ni `config/GOWE69/*`**, con **`measure.py
--cmp` +0 B / +0 funciones** y **`pctsnap --cmp` EMPEORAN: ninguna**, base y
medida tomadas **seguidas**, y con el **control `DOL OK`**.

`trypromo.py zFe2`: **`ENLACE FALLA` (101 errores) → `DOL ROTO`**. No promociona:
le faltan **`.rodata` −4.928 B** y **`.data` −160 B**; el **`.text` sale del mismo
tamaño Y en el mismo desplazamiento** (§4). No me he metido ahí.

---

## 0. Las cinco frases

1. **Los 101 errores eran seis casos distintos, y cinco ya tenían receta**: dos
   definiciones tentativas de `.bss` (`TheUnlockData`, `gMaxPartLevels`), una de
   `.data` (`TWK_RadarDetectorMinThreshold`), dos `static const` de `.rodata`
   (`DriveConfigs`, `HudConfigs`), un `static` que el objetivo tiene global
   (`GetVehicleVectors`) y **una definición que estaba en la unidad equivocada**
   (`gTradeInFactor`, definida en zFeOverlay y que el objetivo pone en zFe2).
2. **Ninguno era un duplicado.** Buscados los siete por nombre, por valor y por
   dirección en `symbols.txt`, en el ELF y en el árbol antes de escribir nada.
3. **El caso nuevo tiene explicación medida y reproducida en 20 líneas: GCC 2.9
   NO emite el cuerpo de las funciones virtuales definidas DENTRO de una clase
   LOCAL cuando esa clase DERIVA de otra con vtable** (§3). Emite la vtable, que
   las referencia con sufijo `.<DECL_UID>`, y deja las dos sin definir. **Si la
   clase local no tiene clase base, sí las emite.**
4. **El bloque `asm()` que ya había en `VehicleDB.cpp` estaba caducado por
   deriva de `DECL_UID`** (`.35688` contra `.30940`): el UID es un contador de la
   unidad entera y cualquier edición de los includes 1..65 de `zFe2.cpp` lo mueve.
5. **Y la trampa que casi cuesta 624 B: renombrar esas etiquetas al UID de hoy
   HACE ENLAZAR pero cuesta −624 B y −16 funciones**, porque objdiff empareja por
   NOMBRE. La solución que da las dos cosas: **dos etiquetas en la misma
   dirección, la del original con `.globl` + `.type ...,@function` y la del UID de
   hoy como etiqueta LOCAL**. Sin el `.type`, objdiff se queda con una sola y **la
   unidad entera deja de medirse** (§3.3).

---

## 1. Verificación del encargo: reproduce al dígito

`scratchpad/c33und3_link.py` (imprime **todos** los errores agrupados por
símbolo, por objeto que lo pide y por **fichero y línea** del uso; `trypromo.py`
recorta a dos y a 90 caracteres).

| veces | símbolo | quién lo pide | dónde |
|---|---|---|---|
| **64** | `TheUnlockData` | **nuestro** `zFe2.o` | `UnlockSystem.cpp`, `FEDatabase.cpp` |
| 8 | `gTradeInFactor` | `zFeOverlay.o` **extraído** | — |
| 4 | `gMaxPartLevels` | **nuestro** `zFe2.o` | `UnlockSystem.cpp` |
| 3 | `GetVehicleVectors__FP8bVector2T0P8ISimable` | `zFe.o` **extraído** | — |
| 2 | `TWK_RadarDetectorMinThreshold` | **nuestro** `zFe2.o` | `FeRadarDetector.cpp:90` |
| 2 | `DriveConfigs` | **nuestro** `zFe2.o` | `FEDatabase.cpp:134` |
| 2 | `HudConfigs` | **nuestro** `zFe2.o` | `FEDatabase.cpp:137` |
| 8×2 | `Callback__CQ313FEPlayerCarDB…` + `_._Q313FEPlayerCarDB…` | **nuestro** `zFe2.o` | §3 |

Total **101**, exactamente lo que contó `nxt` en la r32.

---

## 2. Las siete curas con nombre, con su comprobación de posición

Todas las ediciones **quedan puestas en `src/`**.

### 2.1 `TheUnlockData` y `gMaxPartLevels` — `.bss`, grupo 3

`symbols.txt`: `.bss:0x80473AC4` (0x1C8 B) y `.bss:0x80473C8C` (0x39 B), las dos
`scope:global`, y **las DOS ÚLTIMAS del rango `.bss` de zFe2**, que acaba en
`0x80473CC8`. `UnlockSystem.cpp:12-13` las declaraba `extern` y **nadie las
definía**. Los tamaños casan al byte: `UnlockDatum` mide 8 B × 57 = 456 = 0x1C8,
y `NUM_UNLOCKABLES` = 57 = 0x39.

    -extern UnlockDatum TheUnlockData[57];
    -extern char gMaxPartLevels[NUM_UNLOCKABLES];
    +UnlockDatum TheUnlockData[57];
    +char gMaxPartLevels[NUM_UNLOCKABLES];

Sin inicializador → **grupo 3 del `.bss` de GCC 2.9** (definición tentativa
diferida a `finish_file`), que es justo el que se emite al final: la regla de la
r32 §3, aplicada tal cual.

**`TheUnlockData` cae en su dirección EXACTA** (§2.7): con base
`0x80472F6C`, offset `0xB58` → **`0x80473AC4`**.

### 2.2 `TWK_RadarDetectorMinThreshold` — `.data`, 0.1f

`.data:0x8041BC60`, 4 B, `scope:global`, `data:float`. Valor leído del ELF:
`0x3DCCCCCD` = **0.1f**. Es el **PRIMER símbolo del rango `.data` de zFe2** y va
pegado delante de `RadarDetector::mStaticRange` (`0x8041BC64`), o sea que la
definición del original estaba exactamente donde `FeRadarDetector.cpp:12` tenía
el `extern`. Puesto ahí: `float TWK_RadarDetectorMinThreshold = 0.1f;`.

**No es `const`**, así que GCC no puede plegar la carga aunque el inicializador
sea visible en los tres usos de más abajo (líneas 90/92/95). Comprobado:
`pctsnap --cmp` EMPEORAN: ninguna.

**Cae en el offset 0 de nuestro `.data`**, como el objetivo.

### 2.3 `DriveConfigs` y `HudConfigs` — `static const` en `.rodata`

`.rodata:0x803EA794` y `.rodata:0x803EA7BC`, 0x28 B cada uno, **`scope:local`**
(→ `static`) y **en `.rodata`** (→ `const`). `FEDatabase.cpp:38-39` los declaraba
`extern Attrib::Key …[5][2]` y nadie los definía.

Los diez hashes salen del ELF y **coinciden uno a uno con
`Attrib::Hash::controller`** (`controller_hash.h`), así que se escriben con sus
nombres, no con hexadecimales:

| | `[i][0]` | `[i][1]` |
|---|---|---|
| `DriveConfigs[0..4]` | `key_drive`, `key_drive2`…`key_drive5` | `key_drive_analog`…`key_drive5_analog` |
| `HudConfigs[0..4]` | `key_hud_analog` (×5) | `key_hud` (×5) |

**Aviso sobre `HudConfigs`**: el objetivo tiene los dos hashes **al revés** de lo
que sugiere el índice (`[config][analog]` con `analog`=0 → `key_hud_analog`), y
las cinco filas son idénticas. Está transcrito tal cual sale del ELF, no
"arreglado".

**Caen donde el objetivo los pone**: `DriveConfigs` en `0x5714` y `HudConfigs` en
`0x573C` de nuestro `.rodata`, que mide `0x5768` → **son los dos últimos símbolos
de la sección, con los mismos 4 B de relleno final que el objetivo**
(`0x803EA7E4` → `0x803EA7E8`). GCC 2.9 difiere los estáticos de fichero a
`finish_file` y por eso salen al final solos.

Hizo falta añadir `#include ".../Generated/AttribSys/Classes/controller_hash.h"`
a `FEDatabase.cpp` (el patrón que ya usan `CARSFX_Roadnoise.cpp`, `GManager.cpp`
y otros seis). Los `static const Key` que no se usan no emiten nada.

### 2.4 `GetVehicleVectors` — era `static` y el objetivo lo tiene global

`.text:0x80144198`, 0xE8 B, **`scope:global`**, dentro del rango `.text` de zFe2.
En `FeMinimap.cpp:55` estaba **`static`**, y quien lo llama desde fuera es
`uiWorldMap.cpp`, **que vive en zFe** (otra unidad de enlace) y ya lo declara
`extern` en su línea 26. Es la regla 2 del encargo y la r32 §2.7 clavada.
Quitado el `static`. **La función ya casaba al 100 % y sigue igual.**

### 2.5 `gTradeInFactor` — la definición estaba en la unidad equivocada

`.rodata:0x803E89B0`, 4 B, `scope:global`, valor **0.25f**. La dirección cae en
el rango de **zFe2** (`0x803E4380..0x803EA7E8`), pero nuestro árbol lo definía en
`CarCustomize.cpp:25` (`extern const float gTradeInFactor = 0.25f;`), **que es de
zFeOverlay**. Prueba: el `zFeOverlay.o` **extraído** lo tiene **UND**, y el
nuestro lo **define** en `.rodata` — al revés que el objetivo.

- `CarCustomize.cpp`: la definición pasa a **declaración**
  (`extern const float gTradeInFactor;`).
- `FECustomize.cpp` (include #78 de `zFe2.cpp`): la definición, **al FINAL del
  fichero**, regla de la r31 — con el inicializador visible en el punto de uso
  GCC pliega la carga de un `const`. Los únicos usuarios
  (`CarCustomize.cpp:2417` y `CustomizeManager.cpp:200/215`, vía el inline
  `FEShoppingCartItem::GetTradeInPrice` de `FECustomize.hpp:252`) están **los dos
  en zFeOverlay**, así que en zFe2 no hay ningún uso al que plegar.

Sitio elegido por las cadenas vecinas del ELF: `gTradeInFactor` va justo delante
de `"%2.1f"`/`"LEFT_%s"`/`"RIGHT_%s"` (de `TuningSlider.cpp`), que preceden a las
de `CustomTuning.cpp` (`"HELP_TEXT_LINE1"`) y `UnlockSystem.cpp` (`"carparts"`),
los includes #79 y #80. **`zFeOverlay` no se mueve**: `measure --cmp` +0 B.

### 2.6 Dos posiciones más, gratis (no hacían falta para enlazar)

`.data` del objetivo: `TWK`(0x60) · `mStaticRange`(0x64) · `gChoppedMiniMapManager`(0x68)
· hueco de 24 B · `MinimapShowNonPursuitCops`(0x84). Los dos de en medio estaban
en COMMON y en `.bss` en nuestro objeto, y el objetivo los tiene en **`.data` con
valor 0** — o sea con inicializador explícito (regla `X = 0;` → `.data`):

- `FeRadarDetector.cpp`: `float RadarDetector::mStaticRange = 0.0f;` (era COMMON)
- `FeMinimapStreamer.cpp`: `ChoppedMiniMapManager *gChoppedMiniMapManager = nullptr;` (era `.bss`)

Con eso **la cabeza de nuestro `.data` es la del objetivo símbolo a símbolo** y
`TheUnlockData` cae en su dirección exacta. `measure --cmp` de estos dos solos:
**+0 B, +0 funciones**.

### 2.7 Comprobación de posición, lado a lado

| símbolo | nuestro offset | dirección que da | objetivo |
|---|---|---|---|
| `TWK_RadarDetectorMinThreshold` | `.data 0x00` | — | `.data` +0 ✔ |
| `_13RadarDetector.mStaticRange` | `.data 0x04` | — | `.data` +4 ✔ |
| `gChoppedMiniMapManager` | `.data 0x08` | — | `.data` +8 ✔ |
| `DriveConfigs` | `.rodata 0x5714` | penúltimo | penúltimo ✔ |
| `HudConfigs` | `.rodata 0x573C` | último, +4 B relleno | último, +4 B relleno ✔ |
| `TheUnlockData` | `.bss 0xB58` | **0x80473AC4** | **0x80473AC4** ✔ |
| `gMaxPartLevels` | `.bss 0xD24` | 0x80473C90 | 0x80473C8C ✗ (4 B, §6.1) |

---

## 3. El caso nuevo: **clases locales que GCC 2.9 no emite**

Los 16 errores restantes son **ocho pares** (`Callback__…` + destructor) de ocho
clases declaradas **dentro** de funciones de `FEPlayerCarDB` (`VehicleDB.cpp`
271-383).

### 3.1 El mecanismo, reproducido en 20 líneas

```cpp
struct Rec { int a; int b; };
class Base { public: virtual ~Base() {}
             virtual unsigned Callback(const Rec &r) const = 0; };
extern unsigned RunIt(const Base &cb);

unsigned Total() {
    class Bounty : public Base {
        unsigned Callback(const Rec &r) const { return r.a; }
    };
    Bounty callback;
    return RunIt(callback);
}
```

Con `ngccc` y **los cflags exactos de zFe2**, el objeto sale con:

    _vt.Q211Total__Fv.0_6Bounty        emitida (WEK)
    __Q211Total__Fv.0_6Bounty.8        emitido (el constructor, LCL)
    Callback__CQ211Total__Fv.0_6BountyRC3Rec.6   **UND**
    _._Q211Total__Fv.0_6Bounty.7                 **UND**

**GCC emite la vtable de la clase local, que referencia las dos, y no emite
ninguna de las dos.** El constructor sí sale porque se llama directamente; las
otras dos sólo las alcanza la vtable.

### 3.2 Qué lo cambia y qué no — barrido completo

| variante | ¿emite? |
|---|---|
| base | **no** |
| `-fkeep-inline-functions` | **sí, las dos** (52 B + 8 B: **los tamaños exactos del objetivo**) |
| `-fno-default-inline` | sólo `Callback`; con destructor virtual explícito, **las dos** |
| `-fno-implement-inlines`, `-fno-weak`, `-fexternal-templates`, `-fno-implicit-templates`, `-gdwarf+`, `-fhandle-exceptions` | no |
| `#pragma implementation` / `#pragma interface` | no |
| destructor virtual explícito en la clase local | no |
| tomar `&B::Callback` y pasarlo a una función externa | no |
| objeto `static` local · llamada virtual en la misma función | no |
| base con virtual **no** pura · base **sin** destructor virtual · virtual propia extra | no |
| **clase local SIN clase base** (vtable propia) | **sí, las dos** |

O sea: **el disparador es tener clase base con vtable**, y no hay forma de fuente
que lo esquive manteniendo `class X : public FEPlayerCarDB::MyCallback`.

Las dos banderas que funcionan **no son viables**: `-fkeep-inline-functions`
emitiría además todos los inline muertos de la unidad, y `-fno-default-inline`
rompe la regla «en clase = inline» de la que depende medio árbol (y el `.text`
del objetivo mide exactamente los 250.732 B que medimos, así que el original no
llevaba ninguna de las dos).

### 3.3 La cura: dos etiquetas, y el `.type` no es decorativo

`VehicleDB.cpp` ya traía un bloque `asm()` de un agente anterior con los cuerpos
bajo los nombres del original (`.35649`…`.35774`, **los mismos que el `zFe2.o`
extraído**). Estaba **caducado**: nuestro compilador emite hoy `.30904`…`.31026`
—el sufijo es `DECL_UID`, un contador global de la unidad entera—, así que las
16 referencias no las resolvía nadie.

Tres ensayos, con su cifra:

- **c1 — renombrar al UID de hoy.** zFe2 **enlaza**, pero `measure --cmp` da
  **−624 B y −16 funciones**: objdiff empareja **por nombre**, y al perder el
  nombre del original los ocho cuerpos dejan de contar. **Revertido.**
- **c2 — cuerpo con el nombre del original + alias por `.set`.** El ensamblador
  de ProDG **no emite el símbolo del alias** (comprobado en un caso mínimo: el
  `.s` lleva el `.set` y el `.o` sólo tiene un símbolo). **No enlaza.**
- **c3 — dos etiquetas en la misma dirección (PUESTA).** Enlaza. Pero con las dos
  `.globl`, **objdiff aborta la unidad entera**: `Failed to find right side
  symbol for paired left side symbol`, y `measure.py` la descarta en silencio
  salvo por el aviso. **Poner el alias como etiqueta LOCAL no basta: sigue
  abortando.** Lo que lo arregla es **`.type <nombre del original>,@function`**:
  con él objdiff se queda con el símbolo tipado, mide, y el enlace resuelve por
  la etiqueta local.

Forma final de cada cuerpo:

    ".globl <nombre del original>.35773\n"
    ".type  <nombre del original>.35773,@function\n"
    "<nombre del original>.35773:\n"
    "<mismo nombre>.31025:\n"          <- etiqueta LOCAL, el UID de hoy
    …el cuerpo, sin tocar…

**Resultado: enlaza Y `measure --cmp` +0 B / +0 funciones.**

### 3.4 Fragilidad declarada, y el guion que la cierra en 5 s

El sufijo del alias **se mueve con cualquier edición de los includes 1..65 de
`zFe2.cpp`** y zFe2 deja de enlazar sin que cambie un byte de código. Queda
escrito en el propio `VehicleDB.cpp` y automatizado:

    python scripts/build_direct.py zFe2
    python scratchpad/c33und3_uid.py --write

El guion lee los nombres del original del **objeto extraído** (la verdad del
objetivo) y los UID de hoy de **nuestro `.o`**, y reescribe sólo la etiqueta
alias. Es idempotente y trabaja **en binario** (§6.3).

---

## 4. Qué bloquea ahora a zFe2, y cuánto

`trypromo.py zFe2`: **`ENLACE FALLA` (101 errores) → `DOL ROTO` (`0b3fc7c29892`)**,
4.536.800 B contra 4.541.888.

| sección | nuestro | objetivo | delta | qué es |
|---|---|---|---|---|
| `data2` (`.rodata`) | `0x4B1E0` | `0x4C520` | **−4.928** | **falta dato** (§2 del brief: el pool `lbl_`) |
| `data3` (`.data`) | `0x41340` | `0x413E0` | **−160** | falta dato |
| `text0/1/2` | | | **no aparecen** | **mismo tamaño Y mismo desplazamiento** |
| `data0/1/4/5` | | | +0 | sólo corridas |

**El `.text` no aporta ni un byte de diferencia de tamaño ni de posición**: la
unidad está bloqueada **sólo por datos**. Los −4.928 B de `.rodata` son el frente
del pool `$LC`/`lbl_`, que es de otro agente de esta ronda; **no me he metido
ahí**. `lcpool.py` ya lo tenía contado para zFe2: 13.275 contra 17.011.

Del `.data` (−160 B) hay 24 B identificados y sin nombre: el hueco
`gap_06_8041BC6C_data` entre `gChoppedMiniMapManager` y
`MinimapShowNonPursuitCops`.

---

## 5. Verificación

**Base y medida SEGUIDAS**, con la base tomada **deshaciendo sólo mis trozos**
(`scratchpad/c33und3_ab.py`): `git checkout` no valía porque **otro agente está
editando `FEDatabase.cpp`, `FECustomize.cpp` y `VehicleDB.cpp` en paralelo**
(§6.2) y le habría borrado su trabajo.

| | base (sin mí) | con mis cambios |
|---|---|---|
| `build_direct.py zFe2 zFeOverlay` | 2 ok, 0 fallidas | 2 ok, 0 fallidas |
| `measure.py` (las dos unidades) | **388.660/391.956 B, 1769 fn** | **388.660/391.956 B, 1769 fn** |
| `zFe2` | 248.684/250.732 B, 1304/1307 fn | **idéntico** |
| `zFeOverlay` | 139.976/141.224 B, 465/467 fn | **idéntico** |
| `trypromo.py zFe2` | **ENLACE FALLA, 101 errores** | **DOL ROTO** |

- **`measure.py --cmp` base → mío: `+0 B, +0 funciones, 0 unidades cambian`.**
  Repetido en dos tramos: base → estado tras las siete curas, y ése → estado tras
  las dos posiciones del §2.6. Los dos **+0**.
- **`pctsnap.py --cmp` base → mío: `EMPEORAN: ninguna` · `MEJORAN: 0`.**
- **`audit.py`, DOS pasadas** en `Speed/Indep/SourceLists/zFe2` y
  `…/zFeOverlay`: **0 FALLA** en las cuatro (1.304 + 465 = 1.769 funciones
  auditadas por pasada, que son exactamente las que están al 100 %).
- **CONTROL, sin promocionar nada, con todas mis ediciones puestas: `DOL OK`,
  4.541.888 B.** **No dejo ningún fichero que rompa el enlace.**
- `config/GOWE69/*`, `configure.py`, `build.ninja` y `objdiff.json`:
  `git status` **vacío**.
- Los `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- `keepchk.py`: **759 ok, 21 RANCIAS, 0 sin objeto**; **ninguna rancia es de
  zFe2/zFeOverlay** (sus 32 entradas casan).
- `globalini.py`: **46 unidades correctas, 0 «no emitimos ninguno»**.
- **Sin commit.** **Sin escribir ni una instrucción de ensamblador**: lo único
  que he tocado del bloque `asm()` son **etiquetas y una directiva `.type`**;
  ningún cuerpo cambia (`git diff` del bloque: sólo líneas `.globl`, `.type` y
  etiquetas).

### 5.1 Ficheros de `src/` que dejo modificados (8, +127 −19)

| fichero | qué | § |
|---|---|---|
| `…/Frontend/Careers/UnlockSystem.cpp` | `TheUnlockData` y `gMaxPartLevels` definidos | 2.1 |
| `…/Frontend/HUD/FeRadarDetector.cpp` | `TWK_RadarDetectorMinThreshold` + `mStaticRange` a `.data` | 2.2, 2.6 |
| `…/Frontend/Database/FEDatabase.cpp` | `DriveConfigs`/`HudConfigs` + el `#include` de `controller_hash.h` | 2.3 |
| `…/Frontend/HUD/FeMinimap.cpp` | quitado el `static` a `GetVehicleVectors` | 2.4 |
| `…/Safehouse/customize/CarCustomize.cpp` | la definición pasa a declaración | 2.5 |
| `…/Safehouse/customize/FECustomize.cpp` | `gTradeInFactor` definido al final | 2.5 |
| `…/Frontend/HUD/FeMinimapStreamer.cpp` | `gChoppedMiniMapManager` a `.data` | 2.6 |
| `…/Frontend/Database/VehicleDB.cpp` | etiqueta alias + `.type` + el porqué documentado | 3.3 |

---

## 6. Avisos y deuda declarada

### 6.1 `gMaxPartLevels` se queda a 4 B, y está demostrado por qué

Entre `TheUnlockData` y `gMaxPartLevels` nuestro `.bss` mete
**`gCarTypeNameHash`** (4 B, local), que sale de un `static uint32
gCarTypeNameHash;` **declarado en `CarCustomize.hpp:701`** — una cabecera
compartida. El objetivo **no tiene ese símbolo en el rango de zFe2**, así que en
el original esa cabecera no lo instanciaba ahí. **No lo he tocado**: es cabecera
compartida y el beneficio son 4 B de posición, no de tamaño (el `.bss` del DOL no
entra en el delta del §4).

### 6.2 Carrera con otro agente en TRES de mis ficheros

Durante mi encargo, otro agente ha editado `FEDatabase.cpp`
(`g_MaxSongs`, `FEDatabase`), `FECustomize.cpp` (los cuatro `g_bCustomize*`) y
`VehicleDB.cpp` (`g_MaximumMaximumTimesBusted`,
`g_fImpoundPercentageOfOriginalCost`) — el mismo frente de «definir la definición
tentativa». **Sus cambios están intactos**: mi A/B deshace sólo mis trozos y los
restaura desde copias propias. Pero **si esa edición sigue viva, moverá los
`DECL_UID` y zFe2 dejará de enlazar**: la cura son los dos comandos del §3.4.

### 6.3 Los finales de línea: me mordió, y queda apuntado

`VehicleDB.cpp` tiene los finales **MEZCLADOS** (1.418 CRLF + 222 LF: todo el
bloque `asm()` va con LF y el resto con CRLF). Mi primer parche lo leyó en modo
texto y lo reescribió: **convirtió el fichero entero y borró 1.418 bytes sin
avisar**. Restaurado de copia y rehecho **en binario**. Y ojo: **la herramienta
`Edit` normaliza el fichero a CRLF** — el comentario del §3.3 hubo que meterlo
con un parche binario para no convertir las 222 líneas del bloque `asm()`.

### 6.4 El nombre y la posición del alias siguen siendo un constructo nuestro

Como en la r31 §5 y la r32 §6.3: la etiqueta `.30940` no existe en el original.
Es un nombre que nos inventa el compilador y que fijamos a mano; **desaparece el
día que se sepa emitir el cuerpo desde C++**. El §3.2 dice que hoy no se puede
con este compilador y estas banderas.

---

## 7. Lo que haría el siguiente

1. **El pool de zFe2: `.rodata` −4.928 B.** Es el único frente grande que le
   queda y ya está aislado — el `.text` sale del tamaño y del sitio exactos.
2. **Los 24 B de `gap_06_8041BC6C_data`**, entre `gChoppedMiniMapManager` y
   `MinimapShowNonPursuitCops`: parte de los −160 B de `.data`.
3. **`zFe2.cpp` está incompleto**: el `.rodata` del objetivo tiene las cadenas de
   **`TuningSlider.cpp`** (`"%2.1f"`, `"LEFT_%s"`, `"RIGHT_%s"`, `"Tuning
   FEScrollBar"`) **dentro del rango de zFe2**, y ese fichero **no está en
   ninguna SourceList**. Es la nota «SourceLists: vienen con ficheros de menos»
   con un caso medido y una unidad con nombre.
4. **NO volver** a buscar la forma de fuente que emite las clases locales: §3.2
   es un barrido cerrado de 6 banderas y 9 formas.

---

## 8. Herramientas (scratchpad, prefijo `c33und3_`, 812 kB)

| guion | qué hace |
|---|---|
| **`c33und3_link.py [unidades…]`** | `trypromo.py` con **todos** los errores agrupados por símbolo, por objeto (**y dice si es NUESTRO o EXTRAÍDO**) y por **fichero:línea del uso**; si enlaza, el **DOL sección a sección** contra `orig/GOWE69/sys/main.dol`. Sin argumentos hace el **CONTROL**. Imprime también los avisos `L0019` |
| **`c33und3_uid.py [--write]`** | resincroniza la etiqueta alias de las ocho clases locales leyendo los UID del propio `.o`. §3.4 |
| **`c33und3_ab.py guardar\|base\|mio`** | A/B que deshace **sólo mis trozos** sin tocar lo que otro agente esté editando en los mismos ficheros |
| `c33und3_syms.py` | símbolos de un ELF/objeto (`--sec`, `--addr`, `--bytes` con lectura como float/int/cadena) |
| `c33und3_range.py <sec> <lo> <hi>` | los símbolos de `symbols.txt` dentro de un rango, en orden, **con los huecos marcados** |
| `c33und3_v.sh` | compila un caso mínimo con los cflags exactos de zFe2 y filtra los símbolos de clase local (§3.2, ~1 s por variante) |
