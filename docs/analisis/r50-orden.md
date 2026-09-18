# r50 · `orden`: el orden de emisión lo mandan las VTABLES

La permutación de `zSim` está **cerrada**: `permorden` pasa de **3 desplazadas a
0**, con `fncmp` en 0 de 402 y `lcfix --check` limpio. `zMain` baja de 32 a 30
por la misma causa. El DOL de `zSim` sigue roto, pero ya **no por orden**: lo que
queda son 8 B de pool de `.rodata`, localizados byte a byte más abajo.

| | antes | ahora |
|---|---:|---:|
| `zSim` desplazadas (`permorden`) | 3 | **0 de 402** |
| `zSim` símbolos sólo nuestros | 158 | 155 |
| `zSim` vtables que no casan (`vtord`) | 25 de 79 | **15** (las 5 de interfaz + la cola) |
| `zSim` `dolwhere` | 5.925 B | **4.688 B** |
| `zSim` DOL | `a3bb5b30f68a` | `c2871bf7de9b` |
| `zMain` desplazadas | 32 | **30** |
| `zMain` símbolos sólo nuestros | 165 | 162 |
| `zMain` DOL | `5a867e3b00ee` | `042d42dab84b` |

`fncmp` = **0 de 402** en `zSim` y **0 de 1.380** en `zMain`, antes y después.

## 1. Qué decide el orden, leído en el fuente de GCC

El bloque de plantillas de `.text` **no tiene orden propio**: es una consecuencia
del orden en que `finish_file` escribe las **vtables**. La cadena, con fichero y
línea (`orig/prodg/NGC_GNU_SRC/NGC/gcc/`):

1. **`cp/decl2.c:3643 finish_file`** — un `do { … } while (reconsider)`. Cada
   vuelta hace, en este orden: `instantiate_pending_templates()`, el recorrido de
   vtables, los agregados estáticos (`__static_initialization_and_destruction_0`),
   la síntesis de métodos y por último `wrapup_global_declarations`.

2. **`cp/decl2.c:3708`** `walk_globals (vtable_decl_p, finish_vtable_vardecl)`.

3. **`cp/decl.c:2012 walk_globals_r`** recorre `NAMESPACE_LEVEL (ns)->names`, que
   `pushdecl` construye **anteponiendo**. O sea que **la cadena de vtables es el
   orden INVERSO en que se completaron las clases** (`finish_struct_1` →
   `cp/class.c:769 build_vtable` / `:914 prepare_fresh_vtable`, los dos con
   `pushdecl_top_level`). Para una clase normal eso es el orden de PARSEO de su
   cabecera; para una instanciación, el de `instantiate_class_template`.

4. **`cp/decl2.c:2760 finish_vtable_vardecl`** escribe la vtable sólo si
   `! DECL_EXTERNAL && (DECL_INTERFACE_KNOWN || TREE_SYMBOL_REFERENCED (nombre) …)`.
   Y **`cp/decl2.c:2609 import_export_vtable`**, para una instanciación de
   plantilla llamada con `final = 1`, cae en la rama `comdat_linkage (decl);
   DECL_EXTERNAL (decl) = 0;` que **no pone `DECL_INTERFACE_KNOWN`**. Conclusión:
   *la vuelta del bucle en que sale cada vtable de plantilla depende
   exclusivamente de si alguien ya ha nombrado su símbolo*.

5. **`cp/decl2.c:2507 mark_vtable_entries`** recorre el `CONSTRUCTOR` de la vtable
   **en orden de ranura** y hace `mark_used (fn)` de cada entrada. Con `at_eof`
   puesto, `cp/pt.c:9440 instantiate_decl` ya no difiere
   (`… && ! at_eof` es falso): instancia **ahí mismo**, y `finish_function`
   (`cp/decl.c:14701`) mete el cuerpo en `saved_inlines`
   (`cp/decl2.c:2140 mark_inline_for_output`).

6. **`toplev.c:2940 wrapup_global_declarations`** emite recorriendo el array
   `saved_inlines` **en orden de array**, en varias pasadas, y sólo lo que ya
   tiene `TREE_SYMBOL_REFERENCED` (o es público y no comdat).

De ahí salen las dos reglas que explican todo lo que se veía en `zSim`:

- dentro de una vuelta, el orden de `.text` es el **orden del array**
  `saved_inlines` = el orden en que se terminó de instanciar cada cuerpo;
- **una vtable escrita una vuelta antes adelanta veinte posiciones a todos sus
  cuerpos**, porque los referencia una vuelta antes.

Y la firma se lee a simple vista en el objeto: los accesores de `FixedVector`
salen en el orden `AllocVectorSpace, FreeVectorSpace, GetGrowSize,
GetMaxCapacity`, que **no es el de la clase, es el de las ranuras de la vtable**.

## 2. La causa en `zSim`: un constructor de COPIA sintetizado

`zSim.cpp` definía los tres estáticos así:

```cpp
template <>
UTL::Collections::GarbageNode<Sim::Activity, 40>::Collector
    UTL::Collections::GarbageNode<Sim::Activity, 40>::_mCollector =
        UTL::Collections::GarbageNode<Sim::Activity, 40>::Collector();
```

Eso es **copia-inicialización**. Aunque la copia se elida, marca usado el
constructor de copia de `Collector` y con él el de su miembro `_Storage`, GCC 2.9
lo **sintetiza** y lo **emite ahí mismo**. Medido en el objeto: tres funciones que
el objetivo no tiene,

```
#323  1168 B  __Q33UTL11Collectionst8_Storage2Z…Activityi40_9Collector5_Nodei40RC…
#324  1156 B  __…Entity…
#325  1156 B  __…Model…
#326  1956 B  __static_initialization_and_destruction_0
```

—o sea que salen **antes** que la ssdf, es decir **antes del recorrido de vtables
de la primera vuelta**—. Y cada una lleva una reubicación a
`_vt.UTL::FixedVector<…>`, porque el constructor de copia de `FixedVector`
(`UTLVector.h:326`) pone el puntero de vtable.

Con `TREE_SYMBOL_REFERENCED` ya puesto, `finish_vtable_vardecl` escribe
`_vt.FixedVector<X>` en la **primera** vuelta en vez de la segunda. Comprobado con
`vtord.py`:

| | objetivo | nuestro (antes) |
|---|---|---|
| vuelta 1 | `_Storage<Mod>`, `_Storage<Ent>`, `_Storage<Act>` | `_Storage<Mod>`, **`FV<Mod>`**, `_Storage<Ent>`, **`FV<Ent>`**, `_Storage<Act>`, **`FV<Act>`** |
| vuelta 2 | `FV<Mod>`, `V<Mod>`, `FV<Ent>`, `V<Ent>`, `FV<Act>`, `V<Act>` | `V<Mod>`, `V<Ent>`, `V<Act>` |

Y de ahí los tres `~FixedVector` veinte posiciones antes y los doce accesores
corridos `+3`, que es exactamente lo que decía `permorden`.

### El arreglo, con las tres formas medidas

| forma | resultado |
|---|---|
| `template <> X Y::z = X();` | copia-inicialización → 3 desplazadas |
| `template <> X Y::z;` | con `template <>` y sin inicializador es sólo una **declaración**: no emite nada. Se pierden **30 funciones** y las nueve vtables |
| **`X Y::z;`** | definición por defecto: mismo `.bss`, misma llamada al constructor, **cero** constructor de copia → **0 desplazadas** |

Es la forma pre-estándar que GCC 2.9 acepta. ISO C++ pide `template <>`, así que
queda anotado en `zSim.cpp`: si algún día la unidad se compila con clang hay que
envolverla en una guarda. El port de Android **no** compila las SourceLists
(`PORT-ANDROID.md`: sólo UMath/Foundation/CARP), así que hoy no rompe nada.

## 3. La segunda pieza de `zSim`: dónde se parsea `class Sim::Connection`

Con lo anterior quedaba una vtable fuera de sitio, `_vt.Sim::Connection`, nueve
posiciones más abajo. Por el punto 3 de arriba eso es **orden de parseo**, y el
preprocesado lo confirma (`ngccc -E` sobre `zSim.cpp`):

```
67305 class ITaskable …
67395 class Connection …        <- nosotros
67447 class Object …
67494 class IActivity …
67528 class Activity …
67642 class ITimeManager …
67650 class IStateManager …
                                <- el objetivo lo parsea AQUI
67707 class Entity …
```

Lo arrastraba `SimObject.h:4`, `#include "SimServer.h"`. `SimObject.h` sólo
necesita de ahí `Packet`, `HSIMSERVICE` y `ConnStatus`. Arreglo:

- `enum ConnStatus` pasa de `SimServer.h` a `SimConn.h` —que **ya era el primer
  `#include` de `SimServer.h`**, así que el grafo de includes no cambia para
  nadie y un enum no emite un byte—;
- `SimObject.h` mete el `#include "SimServer.h"` detrás de
  `#ifdef SIMOBJECT_H_NO_SIMSERVER`, con `SimConn.h` + `SimTypes.h` en la otra
  rama;
- `SimEntity.h` lo incluye bajo la misma guarda, que es donde el objetivo lo
  parsea;
- sólo `zSim.cpp` define la guarda.

Resultado: **las vtables 0–63 casan una a una con el objetivo**. Las 15 que
quedan en `vtord` son las 5 de interfaz pura que emitimos de más
(`13IVehicleCache`, `6IModel`, `EventSequencer::IContext`, `11IAttachable`,
`UTL::COM::IUnknown`) y el corrimiento que provocan; el enlazador se las lleva.

**Inercia comprobada, no supuesta**: `zGameplay` compilado con y sin los cuatro
cambios de cabecera da `.text`/`.rodata`/`.data`/`.bss`/`.ctors` **byte a byte
idénticos**, y `lcfix --check` sigue limpio.

## 4. Lo que le queda a `zSim`: 8 B de pool de `.rodata`, localizados

`linkdelta` sigue diciendo `+0 IGUAL` y `fncmp` 0 de 402; el DOL falla por un
desplazamiento de **+8 B** en `.rodata` que llega al primer `_vt` de la unidad y
mueve todas las vtables. Segmentando el tramo `0x80403F48…0x80404BA0` del enlace
contra el DOL original salen **tres puntos** y ninguno es de orden:

| dirección | qué pasa | efecto |
|---|---|---|
| `0x80404868` | el objetivo tiene `00000000, 3c23d70a`; nosotros `3c23d70a, 00000000` (orden del pool de floats) y un `3f800000` de más | −4 |
| `0x80404900` | `$LC553 "MNISComplete"` (13 B) queda **muerto** tras el enlace y `-strip-unused-data` le quita `13 & ~7 = 8`: sobrevive sólo la cola `"lete\0"` | dentro de ese −4 |
| `0x80404950` | igual con `$LC558 "MNotifyMovieFinished"` (21 B): pierde 16 y deja `"shed\0"` | −4 |
| `0x80404B70` | el relleno escrito a mano del final de `zSim.cpp` (`.4byte 0x0 ×3` + `.balign 8`) aporta **16 B** justo antes de la primera vtable | +16 |

−4 −4 +16 = **+8**, que es exactamente el desplazamiento de las vtables.

**Y un aviso de herramienta**: `deadstr.py` **no ve estas dos**. Su prueba de
«muerta» es *no tener ninguna reubicación en NUESTRO objeto*, y estas dos sí la
tienen: las referencian `_GetKind__12MNISComplete` y
`_GetKind__20MNotifyMovieFinished`, que son funciones que sólo emitimos nosotros
y que el enlazador estripa. **Una cadena referenciada únicamente desde código
muerto también se pierde**, y `deadstr` la da por viva. Con eso, `zSim` tiene 26
cadenas fichadas y al menos **dos más** que nadie había visto.

Probado que **no** vale compensar: bajar el relleno de 3 palabras a 1 alinea mal
todo lo demás y `dolwhere` sube de 4.688 B a **70.888 B**. Hay que salvar las dos
cadenas *y* ajustar el relleno, no una cosa u otra.

## 5. Generaliza: 32 constructores de copia de más en siete unidades

El patrón no es de `zSim`: está en los macros de UTL.

```
UCollections.h  IMPLEMENT_INSTANCABLE(H,T)  … _mList = …::_List();
UListable.h     IMPLEMENT_LISTABLE(C)       … _mTable = …::List();
UListable.h     IMPLEMENT_LISTABLESET(...)  … _mLists = …::_ListSet();
```

(`IMPLEMENT_COUNTABLE` e `IMPLEMENT_SINGLETON` son escalares y no tienen el
problema.) Los tres primeros están ahora **detrás de `#ifdef
UTL_IMPLEMENT_NO_COPY_INIT`**, con la rama `#else` byte a byte la original —
comprobado con un test— así que **la cabecera es inerte para quien no encienda la
guarda**. Hoy sólo la enciende `zMain.cpp`.

Censo de constructores de copia que emitimos y el objetivo no:

| unidad | ctores | bytes |
|---|---:|---:|
| `zPhysics` | 23 | 26.552 |
| `zEAXSound` | 5 | 5.732 |
| `zEAXSound2` | 1 | 1.156 |
| `zWorld` | 1 | 1.156 |
| `zCamera` | 1 | 1.144 |
| `zAI` | 1 | 220 |
| **total (fuera de `zSim`/`zMain`)** | **32** | **35.960** |

En `zSim` valió las 3 desplazadas enteras; en `zMain`, 2 de 32. **No prometo el
mismo rendimiento en las otras seis**: encender la guarda cuesta una línea y
`vtord.py` dice en un segundo si mueve algo, pero hasta contarlo no es un frente
—esta es justo la lección de [[nfsmw-extrapolar-frentes]]—. Son las unidades de
los agentes `plat`, `snd`, `world`, `cam` y del frente de `zAI`.

## 6. `zMain`: qué queda

30 desplazadas, ciclos de 87, 34, 15 y siete pequeños. El ciclo de 8 de
`~FixedVector`/`~Vector` **ha desaparecido**: la cola 1.363–1.378 casa entera con
el objetivo. Lo que queda es otra cosa, y `vtord.py` la señala:

```
zMain: objetivo 187 vtables, nuestro 189 — posiciones que no casan: 133 de 189
#3   objetivo _vt.EventSequencer::Engine (208 B)   nuestro _vt.EWorldMapOn
#152 objetivo _vt.EventSequencer::IEngine (208 B)  nuestro _vt.ESndGameState
```

Las dos vtables de `EventSequencer::Engine`/`IEngine` salen fuera de sitio y
corren **todo** el bloque de eventos `E*` una posición; los ciclos de 87 y 34 de
`permorden` son el reflejo de eso en `.text`. Por el punto 3, eso se arregla
moviendo dónde se parsea `EventSequencer.h`, igual que se hizo con
`SimServer.h`. **No lo he tocado**: es una cabecera compartida y el diagnóstico
llegó al final de la ronda.

## 7. Propuestas fuera de territorio

1. **`config/GOWE69/keep.lst`** (no lo toco): añadir las dos cadenas muertas de
   `zSim`, resolviéndolas por contenido con `lcfix`:

   ```
   # @lc zSim "MNISComplete"
   zSim.o:$LC553
   # @lc zSim "MNotifyMovieFinished"
   zSim.o:$LC558
   ```

   Recuperan 8 + 16 = 24 B. Hay que ajustar a la vez el relleno del final de
   `zSim.cpp` (hoy aporta 16 B) y el `3f800000` sobrante de `0x80404868`: el
   objetivo es que el desplazamiento acumulado en `0x80404B70` sea 0. Es trabajo
   de la clave `rodata`, con `scratchpad`-tools ya no necesarias: basta
   `dolwhere` y el segmentador descrito en §4.

2. **`deadstr.py`**: ampliar la prueba de «muerta» a *referenciada sólo desde
   funciones que el enlace estripa*. Como está, se le escapan cadenas que rompen
   el DOL en silencio.

3. **`EventSequencer.h` en `zMain`** (§6): mover su punto de parseo, con la misma
   receta de guarda que `SIMOBJECT_H_NO_SIMSERVER`.

4. **Encender `UTL_IMPLEMENT_NO_COPY_INIT`** en las seis unidades del censo de
   §5, midiendo con `vtord.py` antes y después.

## 8. Aviso de método: carrera con otros agentes

Recompilar una unidad que no es tuya **puede dejar `keep.lst` rancio**. Recompilé
`zGameplay` para comprobar la inercia de las cabeceras y `lcfix --check` empezó a
pedir dos correcciones en esa unidad. No era mío: revertir mis cambios, volver a
compilar y aplicarlos otra vez da `todas las entradas @lc estan al dia` en los
tres estados. Era una edición de otro agente entrando entre dos compilados. Si
`lcfix` se queja de una unidad ajena, **compruébalo con ese ciclo antes de
atribuírtelo**.

## Herramienta nueva

`scripts/vtord.py` — el orden de emisión de las vtables de una unidad, lado a
lado con el objetivo, y el porqué escrito en su docstring con fichero y línea de
GCC. Es lo que convierte «`permorden` dice que hay N desplazadas» en «esta clase
se parsea en el sitio equivocado».

    python scripts/vtord.py zSim
    python scripts/vtord.py --solo-distintas zMain

## Ficheros tocados

| fichero | qué |
|---|---|
| `src/Speed/Indep/SourceLists/zSim.cpp` | los tres `_mCollector` sin `template <>` ni inicializador (con las tres formas medidas en comentario) y `#define SIMOBJECT_H_NO_SIMSERVER` |
| `src/Speed/Indep/SourceLists/zMain.cpp` | `#define UTL_IMPLEMENT_NO_COPY_INIT` |
| `src/Speed/Indep/Libs/Support/Utility/UCollections.h` | `IMPLEMENT_INSTANCABLE` con guarda (rama `#else` idéntica) |
| `src/Speed/Indep/Libs/Support/Utility/UListable.h` | `IMPLEMENT_LISTABLE` / `IMPLEMENT_LISTABLESET` con guarda |
| `src/Speed/Indep/Src/Sim/SimConn.h` | recibe `enum ConnStatus` |
| `src/Speed/Indep/Src/Sim/SimServer.h` | lo cede |
| `src/Speed/Indep/Src/Sim/SimObject.h` | `#include "SimServer.h"` con guarda |
| `src/Speed/Indep/Src/Sim/SimEntity.h` | lo incluye bajo la guarda |
| `scripts/vtord.py` | nuevo |

Nada de `configure.py`, `config/GOWE69/*` ni `splits.txt`. Volcados borrados.
