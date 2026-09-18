# r78 — Por qué parece que estamos estancados, y el plan para dejar de estarlo

Escrito tras revisar el repo oficial (`github.com/dbalatoni13/nfsmw`) rama por
rama y comparar su árbol con el nuestro. Todo lo que lleva cifra está medido hoy.

## 1. El hallazgo que lo explica todo

| | upstream | nosotros |
|---|---:|---:|
| `asm()` en las 34 SourceLists | **0** | **468** |
| ficheros `.s` en todo el repo | 1 (`crt0.s`) | 1 (`crt0.s`) |
| `Object(Matching, ...)` | **94** | **498** |
| `Object(NonMatching, ...)` | 330 | 124 |
| `keep.lst` | 37 KB | 94 KB |
| `splits.txt` | 58 KB | 76 KB |

No es que ellos escondan los datos en otro sitio: no hay más ficheros de
ensamblador. **Simplemente no escriben datos a mano.**

Y la razón es la que corrige tu intuición sobre zAttribSys: **en upstream
zAttribSys es `NonMatching`**. También zAnim, zDebug, zMisc, zGameplay, zSim y
zMain. Su `zAttribSys.cpp` son **20 líneas de `#include` y cero `asm`**; el
nuestro son **581 líneas con 8 `asm`**.

Ese 100 % de zAttribSys que recordabas es el de **objdiff** (comparación del
código), no un enlace. Y ahí está la diferencia de método entera:

- **Upstream** escribe código real, lo mide con objdiff, y deja la unidad
  `NonMatching`. El DOL les sale exacto porque una unidad `NonMatching` aporta el
  objeto **extraído**. Nunca necesitan escribir un dato a mano.
- **Nosotros** promocionamos la unidad a `Matching`, y eso obliga a que **nuestro
  objeto reproduzca el original byte a byte, datos incluidos**. Como los datos
  todavía no están decompilados, los escribimos en `asm`. De ahí salen los 468
  bloques, el `keep.lst` del doble de tamaño y las 498 unidades «Matching».

O sea: **nuestras 498 unidades enlazadas no son una ventaja sobre sus 94; son una
contabilidad distinta, sostenida por parches que el propio README del repo
prohíbe.** Esa es la sensación de estancamiento: llevamos rondas manteniendo un
edificio de parches en lugar de decompilar.

## 2. Lo que hay en el repo oficial y no estábamos usando

- **`SPEED_EXE_1_3` es ya una versión soportada** (commit del 14-sep) y existe
  `config/SPEED_EXE_1_3/`. Es el `speed.exe` de PC — el que tienes en `F:`, con
  fecha de enlace 2005-12-01 y `.text` sin cifrar. Y hay un commit
  «Port some frontend symbols to PC».
- **La rama `ps2_fixes` va 209 commits por delante de main**, 96 ficheros, y son
  exactamente lo que yo he estado haciendo a mano hoy: «Match A124 event pack
  guard», «Match A124 memory node accessors», «Remove dead memory pool
  placeholder». Está 69 commits por detrás de main, así que hay que rebasarla,
  pero **no tiene sentido rehacer ese trabajo**.
- Otras ramas vivas: `xbox_stuff` (símbolos de X360), `zFE-dev`, `eaxsound-clean`,
  `csis`, `physicsdev`, `bit_of_ecstasyy`.
- Nuestro último punto de referencia conocido era `c28db606` (13-sep). Hay
  commits posteriores sin revisar.

## 3. La regla, sin excepciones

**Fuera todo el `asm`.** No solo pines y barreras (esos ya están: 0 pines, 2
barreras). También:

| clase | cuántos | qué es | qué lo sustituye |
|---|---:|---|---|
| DATOS | 468 | `.section`/`.byte` escritos a mano | la unidad pasa a NonMatching; el dato lo pone el objeto extraído |
| ALIAS | 556 | `__asm__("simbolo_manglado")` | declaración real (probado: un estático de clase emite el mismo nombre) |
| INSTR | 162 | instrucciones PPC de verdad | código C, o queda anotado si de verdad no tiene forma en C (`.long 1` de sndvd) |
| BARRERA | 2 | `UTLVector.h` | pendiente |

## 4. El plan

### F1 — La medida de la verdad (primero, y es barata)

Quitar los 468 bloques de datos y degradar lo que deje de reproducir. Medir:
`HECHO`, `ENLACE` y el DOL. Eso nos dice **dónde estamos de verdad** sin el
edificio de parches. Es exactamente lo que pides: quitar los asm y ver dónde
estamos. La cifra bajará mucho y **esa cifra es la buena**.

### F2 — Reconciliar con upstream antes de escribir una línea más

Comparar fichero a fichero nuestro árbol con `main`. Donde ellos tengan código
limpio y nosotros un parche, gana el suyo. Donde nosotros tengamos una función
decompilada de verdad que ellos no, se prepara para PR. Sin esto seguiremos
divergiendo y nada será compartible.

### F3 — Traer `ps2_fixes` en vez de repetirlo

Rebasar esa rama sobre main y ver qué de lo mío de hoy ya está hecho allí.

### F4 — Cuatro objetivos de match, no uno

GameCube (GOWE69) como hoy; **PS2 Black Edition (SLUS-21351), que es la versión
final**, no solo la alpha A124; Xbox 360; y PC (`SPEED_EXE_1_3`). Upstream ya
tiene config para las cinco. Una función que case en varias a la vez es la prueba
de que el código es el real — que es justo lo que buscamos.

### F5 — Los alias, con la prueba que ya tenemos

Está medido que un miembro estático declarado dentro de su clase emite
exactamente el nombre manglado que el alias fabrica a mano. 556 alias
sustituibles por declaraciones reales.

## 5. Lo que NO propongo

- **No tirar nuestro trabajo.** Hay funciones decompiladas de verdad, y el
  oráculo de orden de sentencias (`.line` contra `debug_lines.txt`) es una
  herramienta que upstream no parece usar y que ya nos evitó sellar dos 100 %
  falsos. Eso se conserva y se ofrece.
- **No seguir promocionando unidades a `Matching` a base de escribir datos.** Es
  la práctica que nos ha traído hasta aquí.
