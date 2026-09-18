# r76 -- ¿compila esto fuera de GameCube? Medido con los tres compiladores

Toda la tabla de abajo está medida, no razonada: se compila con los binarios
reales del proyecto y se lee el código de salida.

| compilador | binario |
|---|---|
| GameCube | `build/compilers/ProDG/3.9.3/ngccc.exe` (GCC 2.95.3 SN) |
| PlayStation 2 | `build/compilers/PS2/ee-gcc2.9-991111/bin/ee-gcc.exe` |
| Xbox 360 | `build/compilers/X360/14.00.2110/cl.exe` |

El compilador de PS2 y `mips_binutils` se instalaron en esta ronda. El zip de
compiladores que usa `configure.py` (`files.decomp.dev`, etiqueta 20251015) **no
trae PS2**; el que sí lo trae es el del repo de upstream
(`github.com/dbalatoni13/compilers`, etiqueta 20260903). Se extrajo **solo el
subdirectorio `PS2/`** para no tocar el toolchain de GameCube, que es el que
produce el match: control ejecutado, 0 ficheros previos alterados.

## 1. La tabla de verdad de los andamios

Tres ficheros de cinco líneas, un solo cambio entre ellos:

| forma | GameCube | PS2 | Xbox 360 |
|---|---|---|---|
| `register int x asm("r9");` | compila | **error duro** | **error duro** |
| `__asm__("");` | compila | compila | **error duro** |
| la misma función sin nada | compila | compila | compila |

Los mensajes exactos:

- PS2: `invalid register name 'r9' for register variable`
- X360: `error C2143: syntax error : missing ';' before 'asm'` y
  `error C2290: C++ 'asm' syntax ignored. Use __asm.`
- X360 para la barrera: `error C3861: '__asm__': identifier not found`

Queda cerrado el debate: **un pin no es código portable, es un error de
compilación en dos de las tres plataformas.** La barrera sí pasa en PS2 (es
GCC) y no pasa en Xbox 360.

## 2. El barrido de las 34 SourceLists

`scratchpad/barridoplat.py` compila cada unidad con los dos compiladores sin
tocar `build.ninja` ni `build/`: saca los cflags leyendo el bloque de cada
plataforma de `configure.py` y escribe el objeto en un temporal.

| | compilan | cuáles |
|---|---|---|
| PS2 | **6 de 34** | zAttribSys, zComms, zDebug, zGameModes, zMission, zPlatform |
| Xbox 360 | **1 de 34** | zComms |

Qué bloquea a cada unidad (la clase de su **primer** error):

| | PS2 | Xbox 360 |
|---|---|---|
| `asm` de GNU (cualquier clase) | 0 | **29** |
| cabecera de otra plataforma | 13 | 1 |
| cadena partida / cadena gigante | 0 | 3 |
| otro (tipos, firmas, macros) | 15 | 0 |

**Aviso sobre la clasificación:** en Xbox 360, `C2143`/`C2290` los emite
*cualquier* `asm` de GNU. MSVC no distingue un pin de registro de un bloque de
datos ni de un alias de nombre, así que el contador «asm» de X360 **no** son
pines: son sobre todo los 468 bloques `asm(".section ...")` y los 556 alias.
Los pines que quedan son 24 en todo el árbol.

Consecuencias, por plataforma:

- **PS2 está mucho más cerca de lo que parecía.** Ni un solo `asm` bloquea la
  primera compilación de ninguna unidad; lo que bloquea son cabeceras de
  GameCube incluidas desde código independiente (`dolphin/mtx.h` desde
  `Speed/Indep/Src/Camera/CameraMover.hpp`, `dolphin/types.h` desde
  `zOnline.cpp`) y desajustes reales de firma en el camino de PS2
  (`VU0_MATRIX4Init` en `UVectorMath.hpp:382`). Varias unidades fallan por **un
  solo error**. Solo dos (`zEagl4Anim`, `zFEng`) mueren por un pin de verdad.
- **Xbox 360 está bloqueado por los datos, no por los pines.** Mientras los 468
  bloques `asm(".section ...")` sigan ahí, ninguna unidad que los contenga
  compilará para X360 aunque se quiten todos los pines. El frente F3 del plan no
  es opcional: es la puerta de Xbox 360.

## 3. Hallazgo nuevo: 950 cadenas partidas en 54 ficheros

Los bloques de datos se escribieron con un **salto de línea real dentro de la
cadena** en vez del escape `\n`:

```c
asm(
    ".section .rodata      <- aquí hay un CRLF de verdad, dentro de la cadena
"
    ".balign 8
"
```

GCC 2.95 lo acepta (por eso la build de GameCube nunca se quejó); MSVC lo
rechaza con `C2001: newline in constant`. Es el rastro de un heredoc que se comió
las barras al generar los bloques.

- **950** cadenas partidas en **54** ficheros (las peores: `EcstasyE.cpp` 181,
  `EcstasyEx.cpp` 84, `FEPkg_GarageMain.cpp` 72, `eLight.cpp` 60,
  `zEcstasy.cpp` 53).
- El arreglo (`\n` en vez del salto) produce una cadena **idéntica**, así que el
  ensamblador recibe el mismo texto.
- Control ejecutado en `zDebug` (15 cadenas): compilado antes y después en
  copias temporales, `.text`, `.rodata`, `.data`, `.bss` y `.ctors`
  **idénticas**.
- **Pero no es neutro por construcción:** el arreglo quita una línea por cadena,
  y `__LINE__` se usa en 7 de los 54 ficheros (y en `bWare.hpp` vía `BNEW`). Hay
  que validar el arreglo completo **contra el sha1 del DOL**, no por
  razonamiento.

## 4. Herramientas nuevas de esta ronda

- `scratchpad/compilaplat.py` -- compila un fuente con el compilador real de PS2
  o de X360 sin reconfigurar el proyecto.
- `scratchpad/barridoplat.py` -- lo anterior sobre las 34 SourceLists, con
  clasificación del motivo del fallo (`barridoplat.json`).
- `scratchpad/newlinecad.py` -- censo (y arreglo) de las cadenas partidas.
- `scratchpad/ctrlnewline.py` -- control byte a byte del arreglo anterior sobre
  una unidad, comparando sección a sección sin tocar `src/`.

Las cuatro se promocionan a `scripts/` en la reestructuración (F6) si sobreviven
a la ronda.
