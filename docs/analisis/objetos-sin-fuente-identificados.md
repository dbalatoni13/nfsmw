# Los objetos sin fuente, identificados con el mapa de PS2

`configure.py` declara objetos que **no tienen fuente en el arbol**, asi que ninja
ni lo intenta y no salen en ningun ranking de near-miss. El mapa del enlazador de
PS2 (`orig/SLES-53558-A124/NFS.MAP`) dice **de que biblioteca y de que fichero
salen realmente**, cruzando los simbolos del `.s` del troceador contra la
atribucion simbolo->objeto del mapa.

Metodo: para cada objeto sin fuente se toma la lista de `.fn` de su `.s` y se
busca que objeto de PS2 aporta esos simbolos. La coincidencia es por multiconjunto
de simbolos, no por nombre, asi que no depende de que los ficheros se llamen igual
—y de hecho **casi ninguno se llama igual**.

| nuestro nombre | objeto real (PS2) | coincidencia |
|---|---|---|
| `creates.cpp` | `librealshapez.a(shpcreate.cpp)` | 18/19 |
| `base.cpp` | `librealshapez.a(allocator.cpp)` | 3/4 |
| `model.cpp` | `librealshapez.a(shpelement.cpp)` | 3/3 |
| `cluttype.cpp` | `librealshapez.a(shpcreate.cpp)` | 2/2 |
| `fontkern.cpp` | `librealfontz.a(oldfontkern.cpp)` | 1/3 |
| `fontcreate.cpp` | `librealfontz.a(oldfontcreate.cpp)` | 2/2 |
| `fontchar.cpp` | `librealfontz.a(oldfontchar.cpp)` | 1/1 |
| `sfrsf.c` | `libsndz.a(sfrsf.c)` | 3/3 |
| `ssdfx.c` | `libsndz.a(sdfx.c)` | 2/3 |
| `ssysserv.c` | `libsndz.a(ssysserv.c)` | 3/3 |
| `slib.c` | `libsndz.a(slib.c)` | 1/1 |
| `sn_malloc.c` | `libc.a(malloc)` | 2/2 |
| `qsort.c` | `libc.a(qsort)` | 1/1 |
| `atexit.c` | `libc.a(atexit)` | 1/1 |
| `crt2D1.tmp` | `libgcc.a(__main.o)` | 2/2 |

## Lo que esto descubre

**Dos bibliotecas de middleware que el arbol NO tiene**: `realshape` y `realfont`.
Cuatro de los objetos frios (`creates`, `base`, `model`, `cluttype`) son
**RealShape**, y tres (`fontkern`, `fontcreate`, `fontchar`) son **RealFont** —
y el prefijo `old` de los tres de fuentes dice que el juego usa la API vieja.

Ademas confirma que `sn_malloc`, `qsort` y `atexit` son **newlib**, coherente con
lo que ya decian las entradas `STT_FILE` del ELF de GameCube.

## Como seguir

Para cada uno, `python scripts/ps2fn.py <simbolo>` da la lista ordenada de
llamadas de esa funcion en PS2, que es el esqueleto para reconstruirla. Es MIPS:
el codigo se traduce, no se copia.
