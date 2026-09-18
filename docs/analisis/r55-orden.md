# r55 · `orden` — zGameModes CIERRA, zMain y zFe bajan, zLua es un negativo medido

Unidades: `zGameModes`, `zMain`, `zLua`, `zFe`. Ni un commit, ni un `git add`. `configure.py`,
`config/GOWE69/*`, `splits.txt` y `keep.lst` sin tocar. Ninguna sonda en `scripts/`.

## Titular

**`trypromo Speed/Indep/SourceLists/zGameModes` -> `DOL OK`.** La unidad cierra: de **187.756 B
de DOL distintos a CERO**, y de 25.678/37.251 símbolos en su sitio a **37.254/37.254**. Es una
promoción, y no necesita ni una línea de `keep.lst`: las tres entradas que hacen falta
(`zGameModes.o:pad_05_803EBB48_rodata`, `:lbl_803EBE94`, `:lbl_803EBB48`) ya están puestas.

| unidad | antes | después | Δ | veredicto |
|---|---:|---:|---:|---|
| **zGameModes** | 187.756 B | **0 B** | **−187.756** | **DOL OK — promociona** |
| zFe | 468.194 B | 457.829 B | **−10.365** | DOL ROTO (`984aa1042e87`) |
| zMain | 7.077 B | 6.601 B | **−476** | DOL ROTO (`42ad9fcc5af3`) |
| zLua | 262.958 B | 262.958 B | 0 | **revertida: la edición del brief es NEGATIVA** |
| | | | **−198.597 B** | |

`fncmp` de las cuatro, **antes y después**: `0 de 2`, `0 de 1380`, `0 de 537`, `0 de 921`
funciones con el código distinto. **Cero regresiones.** Las líneas de «sólo nombres de símbolo
distintos» son las mismas de la base salvo en zGameModes, donde **desaparece la única que
había** (`__static_initialization_and_destruction_0`, 80 B): ahora casa también por nombre.

---

## 1 · zGameModes: 744 B, y por qué el brief se quedaba corto

El encargo decía «escribir el prefijo de **540 B**». El bloque entero de la ventana son **848 B**
(`splits.txt:121`, `0x803EBB48..0x803EBE98`) y hay que escribir **840** de ellos; los 8 últimos
salen del propio compilador. Escribir sólo 540 deja `rodata +96` y no cierra nada.

Lo que hay en `src/Speed/Indep/SourceLists/zGameModes.cpp`, en la primera línea y antes de
todo `#include`:

```cpp
asm(
    ".section .rodata\n"
    ".balign 8\n"
    ".globl pad_05_803EBB48_rodata\n"
    "pad_05_803EBB48_rodata:\n"
    "  ... 840 B ...\n"                 // prefijo bWare/STL/Attrib/WorldConn (+0x000)
                                        // 15 cadenas de emocion            (+0x21C)
                                        // "EmotionManagerImpl", floats,
                                        // "EmotionalResponse"              (+0x2D8..)
    ".size pad_05_803EBB48_rodata, 0x348\n"
    ".globl lbl_803EBE90\n  lbl_803EBE90:\n  .4byte 0x3F800000\n"   // el 1.0f
    ".globl lbl_803EBE94\n  lbl_803EBE94:\n  .4byte 0x00000000\n"
);
```

**El nombre es `pad_05_803EBB48_rodata`, no `lbl_803EBB48`.** Los dos tienen entrada en
`keep.lst` (líneas 407 y 2694) y **`-keep` funciona sobre símbolos LOCALES** —el objeto extraído
lo tiene `loc` y el enlace base reproduce el DOL—, pero `lbl_803EBB48` **ya lo define
`src/.../zGameplay.cpp:7-16`**, que hoy no entra al enlace porque zGameplay no está promocionada.
Con `pad_05_` no hay choque posible y además el objeto queda con **la misma tabla de símbolos que
el extraído**.

### Las dos palancas que faltaban, y sin ellas el bloque SOBRA 96 B

Con el `asm()` solo, `linkdelta` pasa de `rodata−744` a **`rodata+96`**: cc1plus interna su
propia copia de todo y el enlazador deja las colas.

1. **`#define UVECTORMATH_ONE lbl_803EBE90`** (`UVectorMath.h:92` ya trae el gancho, e
   `zRender.cpp:78` ya lo usa). Sin esto el `1.0f` de `kFloatScaleDown` sale como `$LC79`
   **detrás** del bloque escrito a mano. Con esto, `__static_initialization_and_destruction_0`
   referencia `lbl_803EBE90` — **exactamente la reubicación del objetivo**
   (`000022 ADDR16_HA lbl_803EBE90`).
2. **`EMOTION_STR(off, s)` y `EMOTION_ZEROF`** en `EmotionManager.cpp`, inertes por defecto
   (`#ifndef` -> el literal de siempre) y redefinidas desde `zGameModes.cpp` para apuntar dentro
   del bloque. Matan `$LC60..$LC74` (las 15 cadenas) y `$LC75..$LC78` (cuatro `0.0f`).

**Por qué hacen falta las dos: `-strip-unused-data` no borra el símbolo entero, deja `size % 8`.**
Medido, símbolo a símbolo: `$LC60` (12 B) deja 4 (`"sed\0"`), `$LC62` (5 B) deja 5 (`"Smug\0"`),
`$LC67` (21 B) deja 5 (`"lock\0"`)… y **los cuatro `0.0f` de 4 B se quedan enteros** (4 % 8 = 4).
Sumado con el relleno: **96 B de basura** dentro de la ventana. Es el mismo mecanismo que ya
produce el revoltijo `sed.Smug....egins...ddsCar..lock....nds....RampUp..Peak....Final` que hoy
hay en 0x803EBB48.

### Y los 4 últimos bytes son relleno del enlazador, no nuestros

Nuestra `.rodata` acaba en +0x34C (840 + 4 + 4 = 848 justos). No hace falta nada más: el objeto
siguiente (`zGameplay.o:lbl_803EBE98`) está alineado a 8 y el enlazador rellena con ceros — que
es lo que el DOL tiene ahí.

### Coordinación con zGameplay (NO es atómico, pero hay que anotarlo)

`src/.../zGameplay.o` define **`lbl_803EBB48`** (que es la dirección de arranque de
**zGameModes**, no la suya: la suya es 0x803EBE98) y **`lbl_803EBE90`** (que es de zGameModes).
Promocionar zGameModes hoy no rompe nada — zGameplay entra al enlace desde `obj/` — y probado:
`trypromo zGameModes zGameplay` **enlaza** (da DOL ROTO por el estado de zGameplay, no por un
símbolo duplicado). Pero **cuando zGameplay promocione**, `lbl_803EBE90` estará definido dos
veces. Propuesta para el agente de zGameplay, que además le arregla un agujero propio:

* renombrar su `asm()` de `lbl_803EBB48` a **`lbl_803EBE98`** — su dirección real, y la que
  `keep.lst:409` ya espera (hoy sus 92 B de prefijo no los cubre ninguna entrada);
* quitar de ahí `lbl_803EBE90`.

---

## 2 · zMain: el `#include` de `INISCarControl.h` vale −476 B, y el andamio de la r54 SIGUE HACIENDO FALTA

Hecho: `#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"` justo detrás de
`WorldConn.h`, con la evidencia del enlace base en el comentario.

`_IHandle__13INISCarEngine`: **0x801F2AB0 (+2.280) -> 0x801F21B0 (−24)** de su ranura del
objetivo (0x801F21C8). Los otros dos `_IHandle__` que el informe de medida señalaba
(`IResetable` +3.000, `ISceneryModel` +4.016) no se mueven: son otros dos `#include`.

| | `.text` dist | `.rodata` dist | total | `permorden` |
|---|---:|---:|---:|---:|
| base | 5.328 | 1.749 | **7.077** | 11 desplazadas |
| con el `#include` | 4.853 | 1.748 | **6.601** | **10 desplazadas** |

Los 7.077 B de la base coinciden **al byte** con la medida de `r55-med-zMain`: la métrica está
calibrada. `linkdelta`: `.text +0  resto IGUAL` antes y después. `dolwhere` 7.961 -> **7.388**.

### El andamio `_zmain_r54_prime_aq`: probado a quitarlo, y es una regresión de +8.293 B

| | total dist |
|---|---:|
| `#include` + andamio | **6.601** |
| `#include` sin andamio | 14.894 |

**No caduca todavía.** Se queda.

---

## 3 · zLua: la edición del brief es un NEGATIVO medido. REVERTIDA

El encargo era continuar el bloque de `#include` de `zLua.cpp:56-58` con las clases generadas
que emiten el pool `803EE3FC..803EE560`. Está medido y **empeora**:

| variante | `linkdelta` | `.text` dist | `.rodata` dist |
|---|---|---:|---:|
| base | `rodata−720 data−320` | 44.637 | 120.829 |
| +9 cabeceras (speech, pvehicle, engine, engineaudio, audiosystem, speechtune, pursuitlevels, world, visuallook) | **`rodata−1064`** | 59.332 | 122.178 |
| +4 (sólo speech, pvehicle, engine, engineaudio) | **`rodata−920`** | 59.374 | 122.010 |
| +4 **y** `keep.lst` ampliado con sus cuatro `$LC` | **`rodata−880`** | — | — |

Las cuatro cadenas SÍ nacen (`$LC480`, `$LC170`, `$LC230`, `$LC481`, comprobado por contenido en
el `.o`), pero al emitirlas antes se juntan con más material no referenciado y el enlazador se
lleva **más** de lo que aportan: el déficit crece 200 B con cuatro cabeceras y 344 con nueve.
**Ni con las entradas de `keep.lst` puestas** (medido sin tocar `config/`, con un `keep.lst`
ampliado en el scratchpad y `-keep` apuntando ahí) se recupera la base: −880 contra −720.

`zLua.cpp` está **revertida a HEAD** (`git status` limpio) y vuelve a dar `rodata−720 data−320`
y `fncmp 0 de 537`.

**Lo que sí propongo para zLua, y es lo barato**: los **324 B de `.data`** que `r55-med-zLua`
mapea uno a uno (huecos de 12, 8, 108, 4 y 192 B, más `flagMapping` declarada ANTES de
`kDisableNIS`/`kPrintScriptMessages`). Eso es contenido que falta, no orden, no toca cabecera
compartida y cierra el `data−320`. No me ha dado tiempo esta ronda.

---

## 4 · zFe: los seis `React` de uiMain, −10.365 B

Hecho, y **con un cambio de plan que el encargo no preveía**: sólo `MainCareer` y `Challenge`
estaban en `uiMain.hpp`; `MainQuickRace`, `MainCustomize`, `MainProfileManager` y `MainOptions`
se declaran **dentro de `uiMain.cpp`**.

Meter los cuerpos de `MainCareer`/`Challenge` en la cabecera **no compila**: necesitan
`FEDatabase`, `IsMemcardEnabled`, `eFE_GAME_MODE_*`, `cFEng` y `MemcardEnter`, y `uiMain.hpp` no
los ve (10 errores de «undeclared»). Meter esos `#include` en la cabecera, o subirlos por encima
de `#include "uiMain.hpp"` en el `.cpp`, mueve el parseo de medio frontend.

**La solución que sí paga: bajar las dos clases a `uiMain.cpp`, delante de `MainQuickRace`.**
Queda el orden exacto del objetivo (MainCareer, Challenge, MainQuickRace, MainCustomize,
MainProfileManager, MainOptions) y los seis cuerpos van **dentro de su clase**, detrás del
`~X() {}`, que es lo que produce la alternancia destructor/React de la cola de `saved_inlines`.

| | `.text` dist | `.rodata` dist | `.data` dist | símbolos en su sitio |
|---|---:|---:|---:|---:|
| base | 198.377 | 148.238 | 121.579 | 22.675/36.674 |
| con los seis en clase | **188.016** | 148.234 | 121.579 | **22.745**/36.674 |

**−10.365 B** (la predicción de `r55-med-zFe` era 11.736; la diferencia es que el modelo suponía
las seis funciones en su ranura exacta y quedan a la distancia que marca el prefijo anterior).
`linkdelta` no se mueve (`rodata−48 data−32`, el déficit de datos que ya estaba).
`fncmp` **0 de 921** antes y después.

**`permorden` no ve nada**: sigue diciendo `105 DESPLAZADAS`. Su LIS es ciega a este cambio
—el ciclo grande pasa de 610 a 806 y la longitud de la subsecuencia no se mueve— mientras la
medida byte a byte baja 10 kB. Otra métrica que manda al sitio equivocado.

### zFe2: comprobado, no le afecta (y no he reconstruido su `.o`)

`uiMain.hpp` lo incluye también `FEPackageData.cpp`, que compila **zFe2**, del agente `frontend`.
Comprobado **leyendo** su `.o` (sin reconstruirlo): ni `build/GOWE69/src/.../zFe2.o` ni
`obj/.../zFe2.o` definen o referencian `MainCareer`, `Challenge`, `MainQuickRace`,
`MainCustomize`, `MainProfileManager` ni `MainOptions`. Y `FEPackageData.cpp` sólo usa `UIMain`
de esa cabecera (línea 136). Aun así **el agente de zFe2 debería re-medir**: la cabecera ha
cambiado.

### Lo siguiente en zFe, y es una PROPUESTA porque la cabecera no es mía

`uiPause.hpp` tiene **exactamente la misma forma**: nueve clases `pm_*` con `~pm_X() override {}`
en clase y `React` declarada, y los nueve cuerpos fuera, en `uiPause.cpp`. La receta es la que
acaba de pagar: **bajar las nueve clases a `uiPause.cpp` con el cuerpo dentro**. `uiPause.hpp` la
incluye también `FEPackageData.cpp` (zFe2), así que **no la toco**: el brief me da `uiMain.hpp` y
ninguna otra cabecera compartida. Después vienen `uiCareerMain`/`uiCareerManager` (10),
`uiProfileManager` (6) y `uiRapSheet*` (7), hasta los 129.084 B / 74,2 % del bloque.

---

## 5 · SORPRESAS — y tres son trampas de medida

### 5.1 `-keep` empareja por NOMBRE DE FICHERO del objeto: copiar el `.o` con otro nombre le quita sus entradas

Perdí media hora con esto. Copié `zMain.o` al scratchpad como `zMain_before.o` / `zMain_after.o`
para compararlos, y `linkdelta` daba `rodata−512` mientras mi comparación daba `rodata−1080`.
No es el árbol: **`keep.lst` tiene 62 líneas `zMain.o:$LCnnn` y un objeto llamado
`zMain_before.o` no casa con ninguna**. 568 B que el enlazador se lleva sólo por el nombre.

**Regla**: si copias un `.o` para compararlo, ponlo en un **directorio** distinto con el
**mismo basename**. Afecta a cualquiera que mida así, y esta ronda somos siete.

### 5.2 El árbol se mueve bajo los pies: mi «regresión de 512 B» era otro objeto, no mi edición

Con la trampa 5.1 encima, llegué a creer que un commit de otro (`4925e9e2 ScratchPtr.h`) había
metido a zMain una regresión de `.rodata −512`. **Es falso**: `linkdelta` da `resto IGUAL` antes
y después de mi edición, y los 7.077 B de la base coinciden al byte con `r55-med-zMain`.
Lo que sí es cierto y conviene saber: **durante la ronda han entrado tres commits** (`4925e9e2`
`ScratchPtr.h`, `571459d4` `FnAnim.h`, `f969f645`) y varios agentes han revertido ficheros
compartidos. **Una medida tomada al principio de la ronda no es comparable con una del final**:
hay que reconstruir la base justo antes de comparar. Mis cifras de arriba son todas
cabeza-a-cabeza, con los dos objetos enlazados **en la misma ejecución**.

### 5.3 `-strip-unused-data` deja `size % 8`, y eso hace que emitir de más CUESTE

Está dicho en el catálogo para `.text`, pero para el pool de `.rodata` tiene una consecuencia que
no estaba escrita: **una unidad que interna una copia de una cadena que ya está en su bloque
escrito a mano no paga 0, paga `size % 8` por símbolo**. En zGameModes eran 96 B repartidos en
19 símbolos, y sin quitarlos el bloque de 840 B dejaba la unidad en `rodata +96` en vez de en 0.
Los cuatro `0.0f` de 4 B son el caso peor: se quedan **enteros**.

Corolario para quien escriba un pool a mano: **no basta con escribir los bytes; hay que apagar
la copia que el compilador interna**, y el catálogo ya tiene los tres ganchos —
`BWARE_PREFIX_*`, `UVECTORMATH_ONE`, `DEAD_STR`/`HAND_POOL_TAG` — más los que haga falta añadir
en la propia unidad (`EMOTION_STR`, `EMOTION_ZEROF`).

### 5.4 `reorden.py` decía `CONTENIDO 0` sobre 744 B que faltaban, y tenía razón en la letra

Confirmo lo que anotó `r55-med-zGameModes`: aquí comparó **11 de 243 palabras**. El veredicto
debería callarse cuando la cobertura es baja. Lo mismo `permorden` en zFe (§4).

### 5.5 `build_direct.py Speed/Indep/SourceLists/zFe` compila TRES unidades

Confirmado: empareja por subcadena, así que `zFe` arrastra `zFe2` y `zFeOverlay`, que son del
agente `frontend`. **He compilado zFe con una sonda propia del scratchpad** que empareja por
nombre exacto. Con siete agentes en paralelo esto no es cosmético: reconstruir el `.o` de otro
le corrompe la medida. Arreglo para `build_direct.py`: probar primero la igualdad exacta
(`w == k`) y sólo caer al emparejado por subcadena si no hay ningún acierto exacto.

---

## 6 · Estado del árbol al cerrar

Ficheros tocados (sin commit):

* `src/Speed/Indep/SourceLists/zGameModes.cpp` — el `asm()` de 848 B, `UVECTORMATH_ONE`,
  `EMOTION_STR`, `EMOTION_ZEROF`.
* `src/Speed/Indep/Src/Gamemodes/EmotionManager.cpp` — las dos macros, inertes por defecto.
* `src/Speed/Indep/SourceLists/zMain.cpp` — un `#include`.
* `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/uiMain.hpp` — quita `MainCareer` y
  `Challenge` (y explica dónde han ido).
* `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/uiMain.cpp` — las seis clases con el
  cuerpo de `React` dentro, en el orden del objetivo.

`src/Speed/Indep/SourceLists/zLua.cpp` **revertida**, sin residuo. Cero `asm` de andamio nuevos.
Las sondas están en el subdirectorio propio del scratchpad y se borran; ninguna en `scripts/`.

**Para promocionar zGameModes hace falta la línea de `configure.py` — que no toco.** El paquete
**no es atómico**: `trypromo Speed/Indep/SourceLists/zGameModes` sola da `DOL OK`.
