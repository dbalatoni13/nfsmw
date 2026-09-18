# Ronda 34 — `pre2`: el prefijo de bWare/STL y las cadenas `Attrib::Gen::*`

**Ninguna unidad llega a `DOL OK`.** Las siete siguen en `DOL ROTO`, igual que en
la base — ninguna ha pasado a `ENLACE FALLA`.

Lo que sí cambia es el dato: **15.409 B menos de diferencia en todo el árbol**
medidos **inmediatamente** después de la parte 1 (`datacmp.py` global: 345.789 B
distintos → 330.380). Al cerrar la ronda el global está en **324.796 (72,7 % ya
igual)**, pero esa última bajada la comparto con `dat2`, que trabajaba
`zFe`/`zFe2` a la vez: la cifra limpia es la de las 15.409. Cuatro unidades pasan
del **0 %** al **95-100 %** de `.rodata` igual.

---

## 0. Las cuatro frases

1. **El encargo estaba mal planteado y la causa era otra.** De las once unidades,
   sólo **tres** no tenían el prefijo escrito (`zSim`, `zFe2`, `zFoundation`).
   `zMisc`, `zMiscSmall`, `zRender` y `zEcstasy` **ya lo tenían escrito**, y
   `datacmp` decía `+0x0` igual: su bloque `asm()` estaba **DETRÁS de todos los
   `#include`**, así que GCC 2.9 lo emitía al **final** de la `.rodata` en vez de
   al principio. **Moverlo delante del primer `#include` es todo el arreglo**, y
   vale mucho más que el prefijo: `zRender` y `zMiscSmall` pasan a **100 % byte a
   byte**, `zEcstasy` a **99,9 %** y `zMisc` a **94,7 %**.
2. **`zEcstasy` NO lleva prefijo, y no es un fallo nuestro**: su objeto original
   **no tiene ni «GAMECUBE», ni la ruta, ni «bad_alloc», ni «STL»** en toda la
   `.rodata` (comprobado por búsqueda de subcadena en el objeto extraído). Es el
   **segundo de los dos prefijos** que menciona el brief. Su `.rodata` empieza en
   `lbl_803DD6D8`, 4 B a cero, y luego `Attrib::Attribute`.
3. **`zGameModes` no tiene `.rodata` en el objetivo** (`splits.txt` no le declara
   el rango): no hay prefijo que escribir. Lo que sobra son **248 B nuestros**.
4. **El bloqueo que queda en `zFe2`/`zFoundation`/`zSim` está medido y es el
   mismo**: emitimos **`"bad_alloc\0"` + `"GAMECUBE\0"` DUPLICADOS** justo
   detrás del prefijo — **24 B exactos** —, y a partir de ahí todo el bloque
   `Attrib::*` del objetivo aparece en el nuestro **con delta +24**. Quitar esos
   24 B alinearía de golpe todo lo que sigue. **Probado y refutado el camino
   obvio** (§4).

---

## 1. Parte 1 — el prefijo: siete unidades, antes y después

`datacmp.py <unidad>`, columna `.rodata`. Lo que mide el avance es el
**desplazamiento de la primera diferencia**; el porcentaje va al lado porque en
estas siete **también subió**.

| unidad | 1ª dif ANTES | 1ª dif DESPUÉS | `.rodata` igual antes → después | qué se hizo | `trypromo.py` |
|---|---|---|---|---|---|
| `zRender` | `+0x0` | **`+0x408`** (= 1032, todo el objetivo) | 6,4 % → **100,0 %** | mover el `asm()` delante del 1er `#include` | DOL ROTO |
| `zMiscSmall` | `+0x0` | **`+0x160`** (= 352, todo el objetivo) | 10,8 % → **100,0 %** | ídem | DOL ROTO |
| `zEcstasy` | `+0x4` | **`+0x1E44`** (7748 de 7824) | 9,9 % → **99,9 %** | ídem | DOL ROTO |
| `zMisc` | `+0x0` | **`+0x1C7B`** (7291 de 7864) | 7,2 % → **94,7 %** | ídem + 15 `.balign` (§2) | DOL ROTO |
| `zSim` | `+0x0` | **`+0x64`** | 54,7 % → 57,2 % | prefijo de 92 B escrito | DOL ROTO |
| `zFe2` | `+0x0` | **`+0x5C`** | 19,3 % → 21,6 % | prefijo de 92 B escrito | DOL ROTO |
| `zFoundation` | `+0x0` | **`+0x5C`** | 14,9 % → 16,3 % | prefijo de 92 B escrito | DOL ROTO |
| `zGameModes` | — | — | el objetivo **no tiene `.rodata`** | nada que escribir | DOL ROTO |

Y el resto del encargo, **verificado y sin tocar**: `zFe` ya estaba en `+0x60`,
`zSpeech` y `zPhysics` en `+0x5C`. **Ya tenían el prefijo.**

**`zFe2` es de `dat2` en esta ronda**: le he escrito **sólo** el bloque `asm()`
del prefijo antes del primer `#include` (símbolo `lbl_803E4380`, 0x5C B) y **no
he tocado nada más de su `.rodata`**. `zFe`, `zSpeech` y `zPhysics` no las he
tocado en absoluto.

### El símbolo, por unidad

Sale del `start` del rango `.rodata` de `splits.txt` y **hay que leerlo del
objeto extraído**, porque unas veces es `lbl_<hex>` y otras
`pad_05_<hex>_rodata`:

| unidad | símbolo | tamaño | ¿en `keep.lst`? |
|---|---|---|---|
| `zSim` | `pad_05_80403F48_rodata` | 0x5C | sí |
| `zFe2` | `lbl_803E4380` | 0x5C | sí |
| `zFoundation` | `pad_05_803EB230_rodata` | 0x5C | sí |
| `zMisc` | `lbl_803F4878` | 0x5C | no (está vivo) |
| `zMiscSmall` | `lbl_803F6730` | 0x70 | sí |
| `zRender` | `pad_05_80403B40_rodata` | 0x400 | no (está vivo) |
| `zEcstasy` | `lbl_803DD6D8` | **0x4** | — (prefijo distinto) |

### Barrido: ya no queda ninguna con el bloque mal colocado

De las 34 SourceLists, hoy **32 tienen su `asm()` de `.rodata` delante del primer
`#include`** y dos no: **`zDebug`** y **`zOnline`**.

- **`zDebug` NO hay que tocarla**: está promocionada y el DOL sale `OK` con ese
  orden; su `+0x0` viene de un `"default\0"` que el compilador emite antes y que
  el estripado se lleva. Moverle el bloque es riesgo puro sin premio.
- **`zOnline`** tiene la `.rodata` al **100 %** (96 de 96 B): tampoco hay nada
  que ganar.

---

## 2. El segundo hallazgo de la parte 1: los `.balign` que faltan

Con el bloque ya delante, `zMisc` se quedó en `+0x8FD`. La causa: **el bloque
escrito a mano no reproduce el relleno de alineación del objetivo**. Entre
`$LC2151633252` (`"Free memory %dK\n"`, 0x11 B) y `$LC2151633272` (un `float`) el
objetivo mete **3 B a cero sin símbolo** y nosotros pegábamos el `float` sin
alinear.

La regla para encontrarlos todos, sin compilar: **el nombre del símbolo lleva su
dirección** (`$LC<decimal>`, `lbl_<hex>`, `gap_NN_<hex>_sec`), así que la
distancia entre dos consecutivos menos el `.size` que declaramos **es el hueco**.
Y hay que **mirar el contenido del objetivo en ese hueco**: si está a cero es
relleno (se arregla con `.balign`); si trae datos es algo que **emite el
compilador** y ahí no hay nada que rellenar.

En `zMisc`: 24 huecos, **14 a cero y de menos de 8 B**. Con sus `.balign`:

    zMisc  .rodata  36,9 %  ->  58,3 %  (+1 balign)  ->  94,7 %  (+14 balign)
                    +0x8FD  ->  +0xF03              ->  +0x1C7B

Herramienta en el scratchpad: `c34pre2_holes2.py <unidad> <fichero.cpp> [--fix]`.
Merece la pena pasarla por **todas** las unidades con bloque escrito a mano.

### Y lo que corta a `zMisc` en `+0x1C7B`: vtables, no relleno

Los 248 B que faltan ahí **están a cero en el objeto, pero llevan 21
reubicaciones**: son las vtables de `DisculatorDriver`, `HighAttribAlloc`,
`EasterEggs`, `AverageWindow` y `Average`. **Las emite el compilador**, y caen
detrás de nuestro bloque escrito a mano. Rellenar con ceros sería falsificarlo.
La vía sería **partir el bloque** y colocar los trozos entre los `#include` que
corresponden (mecanismo 1 del brief de la r33). No lo he hecho.

---

## 3. Parte 2 — el censo completo de `Attrib::*`, las 33 unidades

`Attrib::Gen::<clase>` sale de **`USE_ATTRIB_ALLOC("Attrib::Gen::<clase>")`**, que
está **una vez por cabecera generada** en
`src/Speed/Indep/Src/Generated/AttribSys/Classes/<clase>.h` (macro definida en
`Tools/AttribSys/Runtime/AttribSys.h:65`). O sea: **la cadena aparece si y sólo
si el TU parsea esa cabecera**, y su posición la decide el orden de parseo.

Contado byte a byte contra el objeto extraído (`c34pre2_attrib.py`):

**TOTAL del árbol: faltan 3.976 B de cadenas `Attrib::*`, sobran 3.470 B.**

| unidad | objetivo | nuestro | FALTAN (Gen, B) | SOBRAN (Gen, B) |
|---|---|---|---|---|
| `zMain` | 68 | 39 | 29 (29, **736**) | 0 |
| `zAttribSys` | 35 | 19 | 16 (0, 317) | 0 |
| `zTrack` | 26 | 20 | 11 (11, 278) | 5 (5, 106) |
| `zWorld` | 34 | 31 | 9 (9, 218) | 6 (6, 134) |
| `zPhysics` | 38 | 31 | 9 (9, 214) | 2 (2, 51) |
| `zAnim` | 26 | 25 | 9 (9, 211) | 8 (8, 181) |
| `zGameplay` | 34 | 30 | 10 (7, 204) | 6 (6, 130) |
| `zLua` | 30 | 30 | 8 (8, 184) | 8 (8, 181) |
| `zPlatform` | 31 | 30 | 7 (7, 161) | 6 (6, 130) |
| `zFoundation` | 9 | **0** | 9 (**0**, 149) | 0 |
| `zCamera` | 29 | 30 | 6 (6, 148) | 7 (7, 160) |
| `zAI` | 35 | 34 | 6 (6, 143) | 5 (5, 124) |
| `zFe` | 28 | 33 | 6 (6, 143) | 11 (11, 253) |
| `zSpeech` | 29 | 34 | 6 (6, 142) | 11 (11, 253) |
| `zPhysicsBehaviors` | 38 | 35 | 6 (6, 142) | 3 (3, 75) |
| `zWorld2` | 26 | 27 | 5 (5, 124) | 6 (6, 126) |
| `zFe2` | 31 | 36 | 5 (5, 119) | 10 (10, 229) |
| `zFeOverlay` | 29 | 30 | 5 (5, 119) | 6 (6, 130) |
| `zEAXSound2` | 32 | 36 | 4 (4, 95) | 8 (8, 181) |
| `zSim` | 32 | 37 | 3 (3, 78) | 8 (8, 181) |
| `zEAXSound` | 32 | 38 | 2 (2, 51) | 8 (8, 181) |
| `zRender` | 13 | 22 | 0 | 9 (9, 193) |
| `zMiscSmall` | 8 | 17 | 0 | 9 (8, 188) |
| `zMisc` | 33 | 40 | 0 | 7 (7, 153) |
| `zEcstasy` | 32 | 38 | 0 | 6 (6, 130) |
| `zDebug`, `zDynamics`, `zMission`, `zOnline`, `zBWare`, `zEagl4Anim`, `zFEng`, `zGameModes` | — | — | 0 | 0 |

### Y el dato que ordena el frente: **la ranura 9**

Tras las ocho etiquetas base (ya en orden por `916962fd`) el objetivo pone
**siempre** una `Attrib::Gen::*` y **luego** `Attrib::TAttrib` — que es una
plantilla y por eso se interna en la primera instanciación, o sea **después** de
la primera cabecera generada. Esa ranura 9 es **la primera cabecera generada que
el TU parsea**, y es un cotejo de una línea por unidad:

```
                   ranura 9 OBJETIVO                        ranura 9 NUESTRA
   zAI             Gen::simsurface  TAttrib  Gen::speech    Gen::trafficpattern  TAttrib  Gen::pvehicle
   zAnim           Gen::gameplay    TAttrib  Gen::milest…   Gen::pvehicle        TAttrib  Gen::presetride
   zCamera         Gen::simsurface  TAttrib  Gen::ecar      Gen::gameplay        TAttrib  Gen::pvehicle
OK zDebug          Gen::emittergroup TAttrib Gen::emitte…   Gen::emittergroup    TAttrib  Gen::emitterdata
   zEAXSound       Gen::simsurface  TAttrib  Gen::ecar      Gen::pvehicle        TAttrib  Gen::simsurface
   zEAXSound2      Gen::simsurface  TAttrib  Gen::ecar      Gen::pvehicle        TAttrib  Gen::simsurface
OK zEcstasy        Gen::gameplay    TAttrib  Gen::milest…   Gen::gameplay        TAttrib  Gen::milestonetypes
   zFe             Gen::simsurface  TAttrib  Gen::ecar      Gen::milestonetypes  TAttrib  Gen::audioimpact
   zFe2            Gen::effects     TAttrib  Gen::audioi…   Gen::milestonetypes  TAttrib  Gen::audioimpact
   zFeOverlay      Gen::simsurface  TAttrib  Gen::ecar      Gen::pvehicle        TAttrib  Gen::simsurface
   zGameplay       Gen::effects     TAttrib  Gen::audioi…   Gen::gameplay        TAttrib  Gen::pvehicle
OK zLua            Gen::gameplay    TAttrib  Gen::milest…   Gen::gameplay        TAttrib  Gen::pvehicle
OK zMain           Gen::simsurface  TAttrib  Gen::gameplay  Gen::simsurface      TAttrib  Gen::gameplay
OK zMisc           Gen::gameplay    TAttrib  Gen::milest…   Gen::gameplay        TAttrib  Gen::milestonetypes
   zPhysics        Gen::simsurface  TAttrib  Gen::pvehicle  Gen::milestonetypes  TAttrib  Gen::audioimpact
   zPlatform       Gen::simsurface  TAttrib  Gen::ecar      Gen::pvehicle        TAttrib  Gen::simsurface
   zSim            Gen::simsurface  TAttrib  Gen::rigidb…   Gen::pvehicle        TAttrib  Gen::simsurface
   zSpeech         Gen::simsurface  TAttrib  Gen::speech    Gen::milestonetypes  TAttrib  Gen::audioimpact
   zTrack          Gen::effects     TAttrib  Gen::audioi…   Gen::pvehicle        TAttrib  Gen::simsurface
   zWorld          Gen::effects     TAttrib  Gen::audioi…   Gen::pvehicle        TAttrib  Gen::simsurface
OK zWorld2         Gen::simsurface  TAttrib  Gen::world     Gen::simsurface      TAttrib  Gen::world
```

**7 de 22 ya casan; 15 no.** Y el objetivo sólo usa **cuatro** primeras
cabeceras en todo el juego: `simsurface` (14 unidades), `gameplay` (4),
`effects` (4), `emittergroup` (1). La nuestra es `pvehicle` en nueve unidades y
`milestonetypes` en cuatro — o sea, **entra por un camino de `#include` que el
original no tenía**.

**El DWARF no sirve para esta lista** (medido): el mapa de líneas sólo atribuye
código a 0-15 cabeceras generadas por unidad, porque el `operator new`/`delete`
que la macro define **casi nunca se emite**. La lista buena es **la propia
`.rodata` del objeto extraído**, que es la que está arriba.

---

## 4. Ensayo e7: el prefijo emitido por `cc1plus` — REFUTADO, revertido

La hipótesis merecía la pena porque explicaría de golpe los 24 B duplicados de
`zFe2`/`zFoundation`/`zSim`: si el prefijo lo emitiera **el compilador** en vez de
un `asm()`, reutilizaría el `$LC` de `"GAMECUBE"` y `"bad_alloc"` y el duplicado
desaparecería.

**Montaje** (receta de radio cero de la r33): un `inline` **sin usar** al
principio de `bWare.hpp`, bajo `#ifdef BWARE_EMIT_PREFIX_POOL`, que nombra los
seis literales en el orden del objetivo — incluida **la ruta
`"d:/mw/speed/indep/bware/inc/bware.hpp"` escrita como literal**, que sí se puede
escribir aunque `__FILE__` no la genere. En `zSim.cpp`, el `#define` y el `asm()`
del prefijo dentro de un `#if 0`.

**Resultado medido en `zSim`:**

- **Funciona a medias, y eso es información**: los tres literales que el árbol no
  tiene (la ruta y las dos cadenas de formato) **aparecen**, exactamente **64 B**
  (`.rodata` 7.776 → 7.840), y **`.text` sale IDÉNTICA byte a byte** — el
  `inline` sin usar no emite código, tal y como decía la r33.
- **Pero la posición es la equivocada**: salen en **`+0x28`**, detrás del pool de
  flotantes de la primera función, no en el byte 0. GCC 2.9 saca los literales de
  un `inline` **donde emite ese `inline`**, y eso no es la cabecera del fichero.
- Medida: `.rodata` **57,2 % → 55,6 %**, 1ª diferencia **`+0x64` → `+0x0`**.

**Revertido**: `bWare.hpp` restaurado byte a byte (`git diff` vacío), `zSim.cpp`
con su `asm()` otra vez, recompilado y vuelto a **57,2 % / `+0x64`**.

**Veda, con la sentencia barrida**: *un `inline` de ámbito de fichero al
principio de `bWare.hpp` NO coloca sus literales en el byte 0 de la `.rodata`.*
El truco de `_AttribAllocTagOrder` (r33) sirve para **ordenar entre sí** cadenas
que ya se emiten, no para **adelantarlas al principio de la sección**. Quien
retome esto tiene que buscar un emisor que corra **antes de la primera función**:
un inicializador de dato global, no un `inline`.

---

## 5. Lo que queda apuntado, con su medida

1. **Los 24 B duplicados.** En `zFe2`, `zFoundation` y `zSim` emitimos
   `"bad_alloc\0"`+`"GAMECUBE\0"` detrás del prefijo, y **todo el bloque
   `Attrib::*` del objetivo aparece en el nuestro con delta exactamente +24**
   (`c34pre2_diff.py` lo dice solo). Es el siguiente bloqueo de las tres, y es
   **el mismo en las tres**.
2. **`zEcstasy`, `zMisc`, `zRender`, `zMiscSmall`: el problema ya no es el
   contenido, es el DOBLE.** Su `.rodata` escrita a mano casa al 95-100 %, pero
   nuestro objeto mide 2.000/12.440/13.224 B contra 1.032/7.864/7.824 del
   objetivo: **el compilador emite otra vez su propio pool detrás**. Ahí la
   receta de la r32 (borrar el pool escrito y dejar que `cc1plus` lo emita) está
   **vedada por la r33** para `zMisc`/`zEcstasy` (sólo genera el 61 %). El camino
   que queda es **partir el bloque escrito a mano y colocar los trozos entre los
   `#include`**, que además es lo que colocaría las vtables de §2.
3. **`zGameModes`**: el objetivo no tiene `.rodata` ni `.data`; nosotros emitimos
   248 B de `.rodata` y 64 de `.data`. Su trabajo es **quitar**, no escribir.
4. **`zMain` es la más rentable del frente `Attrib`**: le faltan **29 cadenas
   `Attrib::Gen::*` = 736 B** y no le sobra ninguna. Es la única unidad donde el
   arreglo es puro «incluir la cabecera generada que falta».

---

## 6. Estado del árbol

- **Modificados siete ficheros, sólo con datos y `#include`-order cero**:
  `src/Speed/Indep/SourceLists/{zEcstasy,zFe2,zFoundation,zMisc,zMiscSmall,zRender,zSim}.cpp`.
  Marcados `// c34pre2:`. Respaldos en el scratchpad, `c34pre2_backup/`.
- **`.text`, `.data`, `.bss` y `.ctors` IDÉNTICAS byte a byte en las siete**,
  comprobado compilando el respaldo a un `.o` aparte con los cflags exactos de
  cada unidad (`c34pre2_ab.py`, sin tocar el árbol compartido). Sólo cambia
  `.rodata`.
- **`trypromo.py` de las siete: `DOL ROTO`** — el mismo veredicto que en la base,
  ninguna pasó a `ENLACE FALLA`.
- **`keepchk.py`: 21 RANCIAS, ninguna de mis unidades** (son de `zEAXSound2`,
  `sndfxbus`, `asd2`, `zDynamics`, `GXTexture`).
- **`main.dol` intacto**: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, marca de
  tiempo 2026-09-06 23:29 — anterior a esta sesión. No he lanzado `ninja`.
- **`configure.py` y `config/GOWE69/*`: no los he tocado.** Aviso: durante la
  ronda apareció en `config/GOWE69/splits.txt` un `.sdata2` nuevo para
  `libc/ef_pow.c` que **no es mío** (otro agente).
- **No he escrito ensamblador de instrucciones.** Sólo `.4byte`/`.balign` de
  datos. Los `#if defined(__ANDROID__)` de `types.h`, `bMath.hpp` y
  `UVectorMath.hpp` están intactos. **Sin commits.**

## 7. Herramientas (scratchpad, prefijo `c34pre2_`)

| | |
|---|---|
| `c34pre2_dump.py <unidad> [sec] [n]` | vuelca los primeros N B de una sección, objetivo y nuestro, uno debajo del otro |
| `c34pre2_diff.py <unidad> [sec]` | la primera diferencia con su contexto **y el desfase de resincronización** (dice «tu bloque sale +24 B más allá») |
| `c34pre2_syms.py <unidad> [sec] [n]` | los primeros símbolos del objetivo con su desplazamiento y tamaño — de aquí sale el nombre del símbolo del prefijo |
| `c34pre2_holes2.py <unidad> <fichero.cpp> [--fix]` | los huecos de alineación que le faltan a un bloque `asm()`, diciendo si el objetivo los tiene **a cero** (arreglable) o **con datos** (lo emite el compilador) |
| `c34pre2_rela.py <unidad> <sec> <lo> <hi>` | las reubicaciones del objetivo en un rango: distingue «248 B de ceros» de «una vtable» |
| `c34pre2_attrib.py [unidad]` | el censo `Attrib::*` objetivo vs nuestro, con el orden |
| `c34pre2_ab.py <unidad>` | **A/B de `.text` sin tocar el árbol compartido**: compila el respaldo a un `.o` del scratchpad con los cflags de la unidad y compara sección a sección |
| `c34pre2_prefijo.py` / `c34pre2_move.py` / `c34pre2_balign.py` | los tres parches, en binario y respetando CRLF/LF |
