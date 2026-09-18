# r77 — Plan para que el código case en las tres versiones

Escrito sobre medidas de la r76, no sobre intenciones. Todo lo que aparece aquí
con una cifra está medido con los compiladores reales del proyecto.

## 0. De dónde partimos, con números

| | GameCube (GOWE69) | Xbox 360 (EUROPEGERMILESTONE) | PS2 (SLES-53558-A124 / SLUS-21351) |
|---|---|---|---|
| binario original | `NFSMWRELEASE.ELF` + ISO | `NFS.exe` + `.xex` | `NFS.ELF` + **`NFS.MAP`** / dos ELF |
| `config.yml` | sí | sí | sí |
| `symbols.txt` | sí | sí | sí |
| **`splits.txt`** | **sí** | sí | **NO** |
| **`build.sha1`** | **sí** | **NO** | sí |
| SourceLists que COMPILAN | 34/34 | **1/34** | **6/34** |

Sólo GameCube tiene la cadena completa (splits + keep + ldscript + sha1). Las
otras dos tienen media.

## 1. Lo que bloquea cada plataforma (medido, primer error de cada unidad)

**PS2 — ninguna unidad la bloquea un `asm`.** Bloquean:
- cabeceras de GameCube metidas en código independiente: `dolphin/mtx.h` desde
  `Speed/Indep/Src/Camera/CameraMover.hpp`, `dolphin/types.h` desde
  `zOnline.cpp`;
- firmas desfasadas del camino PS2: `VU0_MATRIX4Init` en `UVectorMath.hpp:382`;
- varias unidades fallan por **un solo error**.

**Xbox 360 — 29 de 34 bloqueadas por `asm` de GNU, pero NO por pines.** MSVC da
el mismo `C2143`/`C2290` para cualquier `asm`, y lo que hay son los **468 bloques
de datos** y los **556 alias**. Más **950 cadenas partidas** (un salto de línea
real dentro de la cadena) que dan `C2001`. Pines quedan 18 en todo el árbol y
bajando.

Conclusión que cambia el orden del trabajo: **quitar pines no abre Xbox 360**. La
puerta de X360 es el frente de datos y el de alias.

## 2. Las cuatro fases, en orden de coste

### F1 — PS2 compilando entero (barato, y es el mejor testigo de que el código es real)

No busca match todavía: busca que las 34 unidades compilen con `ee-gcc`. Es el
cheque de realidad más barato que tenemos, porque un código escrito contra el
DWARF de GameCube que además compila en MIPS es difícil que sea un parche.

1. Sacar las cabeceras de Dolphin del código `Indep`. Dos focos conocidos
   (`CameraMover.hpp`, `zOnline.cpp`); barrer el resto con
   `scratchpad/barridoplat.py`.
2. Arreglar el camino PS2 de `UVectorMath.hpp` (`VU0_MATRIX4Init` y vecinos).
3. Repetir el barrido hasta 34/34. La herramienta ya está y no toca `build.ninja`.

### F2 — Xbox 360: el frente de datos y el de alias

1. **Las 950 cadenas partidas** (54 ficheros). Mecánico y con control ya hecho en
   zDebug (secciones idénticas), pero **no es neutro por construcción**: quita una
   línea por cadena y `__LINE__` se usa en 7 de los 54. Se valida contra el sha1
   del DOL y, si rompe, se bisecta por fichero.
2. **468 bloques de datos, 38.562 B.** Por capas, no de una pasada: la primera
   capa son los ~113 que quedan de los 131 que el plan de la r68 dejó medidos.
   Dos tercios están en unidades NonMatching, donde no pueden mover el DOL.
3. **556 alias.** Está probado que un miembro estático declarado dentro de su
   clase emite exactamente el nombre manglado que el alias fabrica a mano, así que
   son sustituibles por declaraciones reales.

### F3 — El match de PS2 de verdad

Aquí está la pieza que falta: **no hay `splits.txt` para ninguna versión de PS2**.
Sin él no hay objetos extraídos, ni relink, ni porcentaje. Lo que sí tenemos y hay
que aprovechar:
- `orig/SLES-53558-A124/NFS.MAP`, el mapa del enlazador: da el orden de símbolos,
  que es justo lo que en GameCube tuvimos que adivinar a base de sondas;
- `symbols/PS2/` con tipos, globales, etiquetas y ficheros: **da la visibilidad de
  los miembros y el orden de declaración de las virtuales**, que el DWARF de
  GameCube no trae;
- el toolchain, ya instalado y probado.

Orden: `splits.txt` derivado de `NFS.MAP` → extracción con dtk → `build.sha1` →
primer relink. Ojo con el error que ya cometimos en GameCube: **los offsets de PS2
no valen para GameCube y viceversa** (`CameraAnchor` mide 0x130 allí y 0x124 aquí).

### F4 — PC: qué haría falta

No lo tenemos. En `orig/` no hay ningún binario de PC, y `configure.py` sólo
declara cuatro versiones (GameCube, Xbox 360 y las dos de PS2). Lo único que
apunta a que es viable es que el archivo de compiladores de upstream trae
`Win32/7.1` (MSVC 7.1, 184 ficheros), o sea que alguien lo tiene previsto.

Para intentarlo harían falta, por este orden: el ejecutable de PC en `orig/`, una
entrada nueva en `VERSIONS`, sus `cflags`, su `symbols.txt` y su `splits.txt`. Es
un frente entero, no un experimento de una tarde. **Recomendación: después de F1
y F2, y sólo si PS2 ya da porcentaje** — el valor de PC es confirmar fidelidad, y
para eso PS2 ya sirve y está mucho más cerca.

## 3. Lo que la auditoría destapó y hay que limpiar en paralelo

`scripts/auditaportable.py` cuenta lo que el censo de `asm` no veía:

| clase | casos | ficheros | qué es |
|---|---:|---:|---|
| ASM_GNU | 1198 | 275 | `asm`/`__asm__` en cualquier forma |
| BUILTIN | 252 | 17 | `__builtin_*` |
| ATRIB_GNU | 133 | 53 | `__attribute__((...))` |
| CAST_VANO | 54 | 29 | candidatos a «cast al tipo que ya tiene» |
| OFFSET | 19 | 11 | «struct members by pointer offsets» |
| TYPEOF | 3 | 3 | extensión de GCC |

De los 19 `OFFSET`, **10 son el patrón prohibido de verdad** y 9 son idiomas
legítimos. Uno ya resuelto (`TrackCar.cpp`, cuatro accesores que leían
`CameraAnchor` por su offset «porque los miembros son privados» — hoy son
públicos: era una veda caducada). Quedan:

- `rcmp_mad_codec.cpp:629/633/637` — `chunk + 0x15` y `+ 0x18`
- `pb_globals.c:133/140` — `pbi + 0x1b0` y `+ 0x1a8`
- `Movie_GC.cpp:143` — `mShape + 8` (y dos más en las líneas siguientes que el
  patrón no cazó: `GetTexture() + 0x18/4`)
- `CarCustomize.cpp:2424/2428/2435` — `gms + 0x8c`

Se resuelven con `python scripts/orac.py tipo <T>`, que da los miembros con su
offset. `CAST_VANO` es heurístico y hay que mirarlo a mano: la mayoría de los 54
son conversiones de enum a int, que sí significan algo.

## 4. Herramientas que esta ronda deja listas

- `scripts/orac.py` — el oráculo en un grep (5 s en vez de 30): locales con
  registro, tamaños de array, **árbol de inlines con rangos**, y miembros con
  offset. Lee `symbols/mw_dwarfdump.nothpp`, que llevaba meses sin usarse.
- `scripts/auditaportable.py` — el censo de lo no portable.
- `scratchpad/compilaplat.py` y `barridoplat.py` — compilan con los compiladores
  reales de PS2 y X360 sin tocar `build.ninja`.
- `scratchpad/newlinecad.py` y `ctrlnewline.py` — censo y control de las cadenas
  partidas.
- `scratchpad/barreramuerta.py` — ¿esta barrera cambia algo? Sella nuestro objeto
  antes y después.
- `scratchpad/r76_plat/rdline.py` — el cruce `.line` contra `debug_lines.txt`:
  **el orden de las sentencias**, que ya evitó sellar un 100 % falso.
- `scratchpad/r76_dol/shift.py` — perfil de desplazamiento del enlace en un paso.

Las cuatro primeras de `scratchpad/` se promocionan a `scripts/` en la
reestructuración.

## 5. Lo que NO se va a hacer, y por qué

- **No se degradan las ocho unidades grandes en bloque.** Suman 611.900 B, la
  mitad de todo el código nuestro que se enlaza. Por 18 andamios no se tira media
  conquista sin haber buscado antes la forma real, una a una, con los dos
  oráculos.
- **No se adopta una forma «más parecida» que cambie lo que hace la función.**
  Caso vivo: en `sfir` hay formas sin asm que dan 95-97 % en vez del 89,25 %
  actual, pero exigen subir una escritura `halfLen = 4` por delante de una resta,
  y eso puede cambiar qué coeficiente se resta. Queda anotado y sin adoptar.
- **No se sella un 100 % sin pasar el cruce `.line`.** Ya pasó una vez: una forma
  daba 100,0 % exacto y era una función que no es la de EA.
