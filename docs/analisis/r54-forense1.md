# r54 · `forense1`: el mecanismo se llama `instantiate_decl`, y el reparto de los 17.269 B

**El titular es el mecanismo.** Una plantilla *inline* se instancia **en el sitio del
parseo** si —y sólo si— su primer uso está **dentro de una función**; si no, se difiere a
`finish_file` y sale **al final de la unidad**. Eso es lo que separaba a
`~UTL::Collections::Listable<ActionQueue,20>::List` de su sitio.

    GCC 2.95.3, en el arbol:
    cp/pt.c:9299    instantiate_decl()
    cp/pt.c:9308      int nested = in_function_p ();
    cp/pt.c:9439-61   if (! pattern_defined
                          || (! (FUNCTION_DECL && DECL_INLINE (d) && nested) && ! at_eof))
                        { add_pending_template (d); goto out; }   <-- LA RAMA
    cp/decl.c:14701 finish_function() -> mark_inline_for_output (fndecl)
    cp/decl2.c:2140 mark_inline_for_output(): APPEND al array `saved_inlines`
    cp/decl2.c:3795 finish_file(): wrapup_global_declarations (saved_inlines, ...)
                    ESCRIBE EN ORDEN DE ARRAY, y una vuelta del do/while por capa
    cp/decl2.c:3789   ...filtrado por TREE_SYMBOL_REFERENCED || !DECL_COMDAT

Traducido: **la cola de una TU sale en el orden en que se ejecutó `finish_function`**, y
para una plantilla eso es *el punto de instanciación*, no el punto de declaración. Lo que
decide el punto de instanciación es `nested = in_function_p()`.

Y la cifra:

| | antes | ahora |
|---|---:|---:|
| símbolos del enlace en otra dirección | **183** (138 `.text`, 45 `.rodata`) | **88** (43 `.text`, 45 `.rodata`) |
| `dolwhere zMain` | **17.269 B** | **7.961 B** |
| `linkdelta` | IGUAL las nueve | IGUAL las nueve |
| `fncmp` | 0 de 1.380 | **0 de 1.380** |
| `trypromo` | ROTO | ROTO (`e889a6864efa`) |

Coste: **una función `static` muerta de una línea**, que el enlazador estripa (`.text +0`).
Cero `keep.lst`, cero cabeceras compartidas, cero `configure.py`. `lcfix --check` limpio.

---

## 1. La construcción de fuente, y por qué es ésa

`Listable<T,U>::List` (UListable.h:48) declara `~List() override {}` **dentro de la clase**:
es `DECL_INLINE` y es plantilla. Su único uso en zMain es el `_mTable` estático y la vtable,
y los dos ocurren **en `finish_file`**, o sea con `nested` irrelevante y `at_eof=1`: se
instancia entonces y `mark_inline_for_output` la clava al final del array.

El objetivo la tiene **entre `BuildMessageTable__16MAudioReflection` y
`BuildMessageTable__16MBreakerStopCops`** (índice 1269 de 1380 en el `.text` del objeto
extraído; nosotros la teníamos en el 1528 de 1542). O sea: el original **la usó dentro de una
función mientras parseaba esa zona**. Reproducirlo es literalmente eso:

```cpp
// zMain.cpp, justo antes de #include Generated/Events/EBreakerStopCops.cpp
class ActionQueue;
static void _zmain_r54_prime_aq(UTL::Collections::Listable<ActionQueue, 20>::List *p) { p->~List(); }
```

Tres cosas medidas sobre esa línea:

* **No hace falta `ActionQueue` completo.** `List` deriva de `FixedVector<T*,U,16>`: sólo usa
  `T*`. Una declaración adelantada basta y **evita mover el `#include ActionQueue.h`**.
* **Mover el include NO sirve** (§5, negativo con cifra): probado, deja las 138 igual.
* La función es `static` y nadie la llama, así que `-strip-unused-data` se la lleva: el
  `.text` enlazado no se mueve un byte. **No es un `asm`, no es deuda de la regla 5**; es un
  andamio, y como tal caduca ([[nfsmw-andamios-caducan]]): el original tendrá ahí un uso real
  de `~List` que no hemos encontrado.

### 1.1 La prueba de que el mecanismo es ése y no otro

`p->~List()` movió el destructor **exactamente** al hueco del objetivo, ni uno antes ni uno
después:

    OBJETIVO idx 1269/1380  BuildMessageTable__16MAudioReflection | >>~Listable<ActionQueue,20>::List<< | BuildMessageTable__16MBreakerStopCops
    NUESTRO  idx 1432/1543  BuildMessageTable__16MAudioReflection | >>~Listable<ActionQueue,20>::List<< | BuildMessageTable__16MBreakerStopCops

y con él se colocaron **solas las 97 que arrastraba** (`.text` 138 → 43). Sí: la pregunta 3
del encargo se contesta **sí**, y sin tocar nada más.

---

## 2. La contabilidad COMPLETA de los 17.269 B

Nadie la había hecho. `dolwhere` da 269 filas sueltas; agrupadas por rango dan cinco cosas y
sólo cinco:

| bloque | bytes | qué es |
|---|---:|---|
| `.text` 801F83F3–801FA8F0 | **9.264** | el destructor desplazado + sus 97 |
| `.text` 801F20F0–801F35E8 | **5.314** | el racimo B: orden de PARSEO de nueve clases |
| `.rodata` 803EF930 | **1.606** | el pool de cadenas, corrido 4 B |
| `.rodata` vtables y varios | 694 | 9 vtables permutadas + `803F0608`/`803F22E0` |
| `.text` disperso, 1–2 B × ~200 | 391 | **sombra de reubicación**: el `bl`/`@ha` de los que llaman |

**Los 17.269 B no esconden nada más.** No hay un sexto mecanismo. Y de los cinco, el que
pesa dos tercios era una función.

Lo que queda después (7.961 B), con la misma partición:

| bloque | bytes |
|---|---:|
| `.text` 801F20F0–801F35B0 — racimo B | **5.312** |
| `.rodata` 803EF930 — pool de cadenas | **1.606** |
| `.rodata` vtables (9, por parejas) | 306 |
| `.text` 801FA930–801FAA8C — el ciclo de 3 de `Vector<ActionQueue*,16>` | 261 |
| `.rodata` 803F22E0 + 803F0608 | 130 |
| sombras de reubicación | ~346 |

---

## 3. Las 41 restantes (pregunta 4): NO son plantillas, son includes

El racimo B **no tiene nada que ver con el de arriba**. Son funciones `inline` normales
—`_IHandle__X()` de `DECL_INTERFACE` (UCOM.h:109), `TypeName__X`, los cuatro de
`WorldConn::Pkt_Body_Send`— y su índice en `saved_inlines` es **el final del parseo de su
clase**. El orden relativo de los `HandleMessage_LuaBinding__M*` y los `BuildMessageTable__M*`
ya casa entero: lo único que baila es **dónde se intercalan las clases**.

Medido contra el preprocesado (`ngccc -E`, 134.299 líneas), con las `M*.h` como jalones:

| símbolo | su clase, línea del preprocesado NUESTRO | ventana que pide el objetivo | veredicto |
|---|---:|---|---|
| `_IHandle__7IPlayer` | `IPlayer` 88.748 | antes de `MAIEngineRev` (61.632) | **27k tarde** |
| `_IHandle__Q23Sim13IStateManager` | 89.083 | antes de 61.632 | **28k tarde** |
| `_IHandle__10IResetable` | 115.077 | antes de 61.632 | **53k tarde** |
| `_IHandle__13INISCarEngine` | 103.705 | antes de 61.632 | **42k tarde** |
| `~/ConnectionClass/Size/Type __Pkt_Body_Send` | 57.422 | antes de 61.632 | ventana OK, **orden INTERNO al revés** (el objetivo saca el destructor el primero) |
| `_IHandle__Q217CollisionGeometry10IBoundable` | 35.323 | 72.330–89.427 | **37k pronto** |
| `_IHandle__13ISceneryModel` | 120.416 | 89.427–97.166 | **23k tarde** |
| `TypeName__15SmackableParams` | 118.955 | 89.427–97.166 | **22k tarde** |
| `_IHandle__9IFeedback` | 88.238 | 97.296–99.229 | **9k pronto** |
| `TypeName__15ExplosionParams` | 118.079 | 110.667–114.740 | **4k tarde** |
| `_IHandle__Q214EventSequencer7IEngine`, `_._…IEngine` | 93.936 | 89.427–97.166 | **YA BIEN** |
| `_IHandle__18IDamageableVehicle` | 95.802 | 89.427–97.166 | **YA BIEN** |
| `_IHandle__15IDynamicsEntity` | 100.572 | 99.229–103.412 | **YA BIEN** |

O sea: **nueve clases que mover y un orden interno**, no 41 causas. Y la receta ya existe en
el propio `zMain.cpp`: `EVENTSEQ_FWD_IENGINE` puso `EventSequencer::IEngine` en su ventana y
por eso sus dos símbolos salen bien. Los tres «YA BIEN» de la tabla son la prueba de que el
método funciona; los otros nueve son el mismo trabajo repetido.

Es trabajo de cabecera compartida (`propone, no apliques`), y **cada una se mide sola**: mover
una clase sólo puede tocar los símbolos de esa clase.

---

## 4. El frente que ninguna herramienta veía: 1.736 B de pool de literales

`movidos`, `permorden` y `symtabdiff` daban el `.rodata` de zMain en «9 vtables». Es falso, y
la razón es que **el objeto extraído del original no tiene símbolos `$LC`**: las cadenas son
invisibles a cualquier comparador por nombre. `rodorden.py` tampoco vale aquí
(`zMain: sin rango de .rodata en splits.txt`).

Leyendo los bytes del enlace, la causa es de una línea:

    803EF920  ...EAudioSmackableTest.  <- los dos coinciden hasta aqui
    OBJ 803EF930  4D 41 75 64 69 6F 52 65 66 6C 65 63 74 69 6F 6E   |MAudioReflection|
    NUE 803EF930  72 79 00 00 4D 41 75 64 69 6F 52 65 66 6C 65 63   |ry..MAudioReflec|

Ese `ry\0\0` son **los 4 B que sobreviven al estripado** (`size & ~7`,
[[nfsmw-inline-es-posicion]]) del literal muerto **`"EAGL4::SymbolEntry"`** (20 B), que
`eagl4supportdef.h:42` mete vía `HAND_POOL_TAG` y **el objetivo no tiene**. Corre el pool 4 B
durante **1.606 B**, hasta que en `803EFF74` un hueco de alineación se lo come y vuelve a
sincronizar. En el objeto se ve entero:

    +03AC  est.EAGL4::SymbolEntry..MAudioRe...

**Y el arreglo obvio es una regresión, medida**: `#define HAND_POOL_TAG(s) ((const char *)0)`
—la receta de `zMisc`/`zRender`/`zSim`— mata también `"EventSequencerSystems"` de
`EventSequencer.h:200`, y ahí el objetivo **sí pasa un puntero de verdad**:
`fncmp` pasó de **0 a 1 de 1.380** (`Create__14EventSequencer` 784 → 780 B). Revertido.

Lo que hace falta es una guarda **selectiva** en `eagl4supportdef.h` (cabecera compartida, la
propongo, no la aplico):

```c
#ifdef EAGL4_NO_SYMBOLENTRY_TAG
#define EAGL4_SYMBOLENTRY_TAG ((const char *)0)
#else
#define EAGL4_SYMBOLENTRY_TAG HAND_POOL_TAG("EAGL4::SymbolEntry")
#endif
```

y `#define EAGL4_NO_SYMBOLENTRY_TAG` en `zMain.cpp`. **No es gratis**: renumera los `$LC` y
pide `python scripts/lcfix.py zMain` (**62 líneas** de `config/GOWE69/keep.lst`); sin esa
tanda, `linkdelta` se va a `rodata−512 .text−4`. El paquete **es atómico**. Predicción:
cierra los 1.606 B.

Los otros 130 B de `.rodata` son (a) `803F22E0`: emitimos `"EventSequencerSystems"` donde el
objetivo tiene floats — la cadena existe en los dos, es **problema de POSICIÓN**, exactamente
el falso positivo de `prefijotu` que documentó la r53; y (b) `803F0608`: 14 B de pool de
floats en otro orden.

---

## 5. Negativos, con su cifra

* **Mover `#include ActionQueue.h` no hace nada.** Lo probé primero (de `zMain.cpp:240` a
  antes de `EBreakerStopCops.cpp`): `.text` movidos **138 → 138**, destructor en el mismo
  1528, y encima `.rodata` cae a **312.592 (−16)** por renumeración de `$LC`. Revertido.
  Parsear la clase antes **no** instancia el método: lo instancia el USO.
* **`HAND_POOL_TAG` a secas es regresión de código** (§4): `fncmp` 0 → 1. Revertido.
* **`zMain` sigue `DOL ROTO`** (`e889a6864efa`). No pido promoción.
* **La vtable y su método clave no viajan juntos.** La r53 emparejó las dos vtables de `_List`
  y el destructor siguió tarde; ahora el destructor está en su sitio y **las 9 vtables
  descolocadas siguen siendo las mismas 9**. Son dos frentes independientes.

## 6. Tres herramientas que mienten, y cómo

1. **`movidos.py` restaba el racimo como «arrastre».** Tomaba la MODA del delta por sección
   entre los símbolos que cambian; en zMain la moda de `.text` era **−180 con 97 símbolos**, o
   sea justo el racimo que buscábamos, y lo restaba: decía «83 permutados» cuando eran 183.
   Con las nueve secciones a `IGUAL` el arrastre **es cero por construcción** y no hay nada que
   restar. (Otro agente ha reescrito el cálculo hoy mismo a partir de los tamaños de sección;
   con esa versión zMain da 183/88, que es lo correcto.)
2. **`movidos.py` cuenta `$LC` que no son suyos.** Indexa la tabla de símbolos por NOMBRE, y
   `$LC62` existe en decenas de objetos; el `dict` se queda con el último. En zMain eso
   inventaba **36 «cadenas movidas» con deltas de +112k/+124k** que son otro objeto. Descartar
   todo nombre con más de una definición en el ELF: bajan de 45 a 9, que son las vtables.
3. **`permorden` es CIEGO a esto.** Da **11 antes y 11 después** de colocar el destructor,
   porque su métrica es la subsecuencia creciente más larga y un símbolo que salta 259
   posiciones cuesta exactamente 1. Para un desplazamiento único y grande hay que mirar
   `movidos` o el índice del símbolo, no `permorden`. [[nfsmw-medidas-que-enganan]]

Mis sondas están en `scratchpad/r54_forense1/` (`probe.py` compara los dos ELF enlazados sin
el heurístico de arrastre; `pos.py` da el ÍNDICE de un símbolo en el `.text` de los dos
objetos **en 0,4 s y sin enlazar** — es lo que hace medible este frente; `orden.py` los pone
uno al lado del otro; `rodwin.py` tokeniza dos ventanas de `.rodata`). Ninguna en `scripts/`.

## 7. Ficheros tocados

| fichero | qué | quién lo compila |
|---|---|---|
| `src/Speed/Indep/SourceLists/zMain.cpp` | 17 líneas: el andamio de §1 y su comentario | sólo zMain |

**Nada más.** Ni `configure.py`, ni `config/GOWE69/*`, ni `splits.txt`, ni `keep.lst`, ni una
cabecera compartida, ni un commit, ni un `git add`. Ni `ninja` ni `configure.py`: sólo
`scripts/build_direct.py zMain`.

*(Aviso operativo: `build_direct.py zMain` falla de manera intermitente en este árbol
—`0 ok, 1 fallidas` sin ningún error, con el `.o` recién escrito— y a la segunda pasa. El
compilador devuelve 0 lanzado a mano. Si os sale, relanzad antes de investigar.)*

## 8. Concuerda con `forense2`, que llegó por el otro lado

`r54-forense2.md` partió los mismos bytes reordenando el contenido y enmascarando el campo de
reubicación, sin tocar el árbol. Sus tres causas grandes son **las mismas tres**, con los
mismos dueños: `R1` el destructor (55 %), `R2` los `_IHandle__*`/`TypeName__*` (31 %), `R3` el
literal `"EAGL4::SymbolEntry"` (10 %). Dos medidas independientes, misma partición. Él además
demuestra que **el contenido de zMain es 0 B distinto**: no queda una línea que escribir, sólo
sitio. Lo que aporta esto de aquí es el **porqué** de `R1` y su arreglo aplicado y medido.

## 9. Para la r55

1. **El racimo B, 5.312 B, es una lista de nueve clases** (§3) y cada una se mide sola. Es lo
   más grande que queda de zMain y no necesita ninguna idea nueva: la receta
   `EVENTSEQ_FWD_IENGINE` ya funciona en la misma unidad.
2. **El pool de literales, 1.606 B**, con la guarda selectiva de §4 + `lcfix zMain` (62 líneas
   de `keep.lst`). Paquete atómico. **No useis `HAND_POOL_TAG` a secas.**
3. **Comprobad si el mecanismo de §1 explica las otras doce del bloque A.** El síntoma es
   barato de buscar: un destructor o método de plantilla `inline` al FINAL del `.text` del
   objeto que el objetivo tiene a mitad. `scratchpad/r54_forense1/pos.py` lo dice en 0,4 s por
   unidad, sin enlazar.
4. **La regla general que sale de aquí**, y vale para cualquier unidad: si un símbolo de
   plantilla sale al final de la TU y el objetivo lo tiene en medio, **no es orden de
   declaración ni la vtable: es que el original lo usó dentro de una función en ese punto**
   (`cp/pt.c:9439`). Y al revés: si sale en medio y el objetivo lo tiene al final, sobra un
   uso nuestro dentro de una función.
