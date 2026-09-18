# r36 · grupo `attx` — zAttribSys: sólo le quedaba el ORDEN

**Resultado: 43 → 20 descolocadas, 15 → 3 saltos.** Los 3 saltos y las 20
funciones son **una sola causa**, aislada y con nombre:
`Transfer__Q26Attrib7HashMapRQ26Attrib4Node`. Todo lo demás del `.text` de
zAttribSys está ya en el orden del objetivo.

Sin perder un byte de dato: `.rodata` 1176/1176, `.data` 96/96, `.bss` igual,
`measure` 48.776/48.776 B al 100 %, `stripmap` 0/0/0.

---

## 0. Verificación del encargo (antes de tocar nada)

| medida | esperado | obtenido |
|---|---|---|
| `textorder.py zAttribSys` | 43 de 197, 15 saltos | **43 de 197, 15 saltos** |
| `measure.py zAttribSys` | 100 % | **48.776/48.776 B, 100,0000 %, 196 fns** |
| `datadiff.py zAttribSys` | dato cerrado | **1176/1176, falta 0, sobra 0** |

Reproduce exacto. `Attribute::~Attribute` ya estaba movido a `Attribute.cpp`
(commit `a8cdba3e`), o sea que partí del estado post-r35 como decía el encargo.

---

## 1. Los cuatro ensayos, todos medidos por separado

Cada uno compilado a un `.o` del scratchpad y medido con `textorder` encima;
ninguno tocó el árbol hasta estar medido.

| ensayo | qué | desc | saltos |
|---|---|---|---|
| base | | 43 | 15 |
| **e1** | `class Array` y `class Node` de `AttribSys.h` → `AttribHashMap.h` | **35** | 12 |
| **e2** | `ClassTable` + `DatabaseLoadData` + `DatabasePrivate` de `AttribPrivate.h` → `AttribDatabase.cpp` | **32** | 11 |
| **e3** | los cuatro estáticos de `DatabasePrivate` (`QueueForDelete` ×2, `CollectGarbageBag` ×2) **detrás** del ctor y el dtor | **24** | 8 |
| **e4** | `TypeDescPtrVec` + `TypeTable` + `CollectionList` + `ClassList` de `AttribSys.h` → `AttribDatabase.cpp`, detrás de `ClassTable` | **20** | **3** |

Con e1..e4 caen **doce de los quince saltos**: el racimo entero de plantillas
(`__lower_bound`/`ScanForValidKey`/`find`/`_M_erase`/`clear`/`reserve`/
`_M_insert`/`insert_unique`), el racimo de `Definition::GetFlag` contra
`Array`/`Node`, y la cola completa
(`CollectionHashMap(Ui)`, `TypeTable::TypeTable`, `~DatabasePrivate`,
`~ClassTable`, `~TypeTable`).

---

## 2. El hallazgo de la ronda: **`debug_lines` no sólo dice el orden, dice DÓNDE VIVE CADA CLASE**

La r35 usaba `debug_lines` para leer el orden de parseo. Esta ronda lo usa para
algo más fuerte: **el `fichero:línea` de una función dice en qué fichero está
definida su clase en el original**, y eso reordena por sí solo bloques enteros.
Tres lecturas dieron los cuatro ensayos, y las tres se verifican con aritmética
de líneas antes de compilar:

1. **`Array` y `Node` no están en `attribsys.h`.**
   `GetPad__Q26Attrib5Array` es `attribhashmap.h:76`, `IsLaidOut__Q26Attrib4Node`
   es `attribhashmap.h:424` y `HashMap::RebuildTable` es `attribhashmap.h:656`,
   mientras `Definition::GetFlag` es `attribsys.h:572`. Como `attribhashmap.h`
   incluye `attribsys.h`, eso obliga al orden **Definition → Array → Node →
   HashMap** que tiene el objetivo; nosotros teníamos las tres clases dentro de
   `AttribSys.h` y salía **Array → Node → Definition → HashMap**.
   (El árbol ya llevaba dos `// TODO move to AttribHashMap.h` encima de las dos
   clases. Nadie los había cobrado.)

2. **`attribprivate.h` NO USA NI UNA LÍNEA POR ENCIMA DE LA 252**, y
   `ScanForValidKey` es la `:247`. La nuestra estaba en la **338**: sobraban 91
   líneas justo delante, que son exactamente `ClassTable` + `DatabaseLoadData` +
   `DatabasePrivate`. Movido el bloque, `ScanForValidKey` cae en la **247
   clavada**. Es la comprobación más barata que hay: se hace con `grep -n` y sin
   compilar.

3. **`DatabasePrivate`, `TypeTable` y `ClassTable` viven en el `.cpp`.**
   `_._Q26Attrib15DatabasePrivate` es `attribdatabase.cpp:62`, y
   `__Q26Attrib9TypeTable`, `_._Q26Attrib9TypeTable` y `_._Q26Attrib10ClassTable`
   son `attribdatabase.cpp:36` — un `.cpp`, no una cabecera. Nosotros las
   teníamos en `AttribPrivate.h` y en `AttribSys.h`.

### Por qué e2 vale tanto: el `std::find` de `QueueForDelete`

`DatabasePrivate::QueueForDelete` es quien pide
`find<_List_iterator<const Collection*>>`. Con la clase en `AttribPrivate.h`,
esa cabecera se parsea **arriba del primer `.cpp`** (`AttribClass.cpp` la incluye
en su línea 2) y el `find` era la PRIMERA plantilla de la lista pendiente. En el
objetivo el `find` sale el ÚLTIMO del bloque, porque en el original se pide desde
el **tercer** `.cpp`. Mover la clase al `.cpp` reordena el racimo entero.

### Por qué e3: el orden de demanda dentro de la clase

Aun con la clase en el `.cpp`, el `find` seguía delante de
`reserve`/`_M_insert`/`insert_unique` (ctor) y `_M_erase`/`clear` (dtor). El
objetivo los quiere al revés. Poniendo los cuatro estáticos **detrás** del ctor y
del dtor, el racimo de once plantillas queda **idéntico al objetivo**.

### Por qué e4: `ClassTable` tiene que definirse ANTES que `TypeTable`

Los destructores implícitos se sintetizan en el orden en que sus clases entran en
`saved_inlines`, o sea **orden de definición de la clase**. El objetivo emite
`~ClassTable` antes que `~TypeTable`; con `TypeTable` en `AttribSys.h` (lo
primero que se parsea) y `ClassTable` en el `.cpp`, nos salía al revés. Movidas
las cuatro clases derivadas de STL al `.cpp` detrás de `ClassTable`, la cola
entera (5 símbolos) casa.

---

## 3. El modelo de `finish_file`, ahora LEÍDO en el fuente del compilador

`orig/prodg/NGC_GNU_SRC/NGC/gcc/cp/decl2.c:3643` (`finish_file`) y
`toplev.c:2940` (`wrapup_global_declarations`) dan el algoritmo exacto. Vale la
pena escribirlo porque **predice** el orden y porque acota lo que se puede
intentar:

**Por cada pasada del `do { ... } while (reconsider)`, en este orden:**

1. `instantiate_pending_templates()` — las instanciaciones pendientes, en orden
   de DEMANDA.
2. las vtables (`finish_vtable_vardecl`).
3. los agregados estáticos → `__static_initialization_and_destruction_0`.
4. `synthesize_method` de los métodos artificiales pendientes.
5. **el filtro**: `DECL_EXTERNAL(decl) = 0` sólo si
   `DECL_NOT_REALLY_EXTERN && DECL_INITIAL && (TREE_SYMBOL_REFERENCED || !DECL_COMDAT)`.
6. `wrapup_global_declarations(saved_inlines, n)` — barre el array **en orden de
   índice** y emite las que tengan `DECL_EXTERNAL == 0`. Las demás **no se
   emiten en esta pasada por mucho que se referencien durante el barrido**.

Y dos hechos que se deducen de ahí y que son los que mandan:

* **`saved_inlines` está en orden de `mark_inline_for_output`**, que se llama en
  `finish_function` (decl.c:14701) y en `cons_up_default_function`
  (lex.c:2045). O sea: **orden de compilación del CUERPO** para las inline
  normales, y **orden de definición de la CLASE** para las sintetizadas. No es
  orden de declaración.
* **`TREE_SYMBOL_REFERENCED` sólo lo pone `assemble_name` (varasm.c:1855)**, es
  decir cuando el nombre se ESCRIBE en el `.s`. Y `assemble_external` no sirve:
  `ASM_OUTPUT_EXTERNAL` no está definido para este target (sólo en `aix*.h` y
  `beos.h`).

### Corolario medido: una inline sólo se puede expandir si su cuerpo ya está compilado

Ensayo **T4** (revertido): moviendo `Transfer` al PRINCIPIO de `class HashMap`,
`RebuildTable` se la **inlina** y `Transfer` desaparece del objeto
(`SOLO EN EL OBJETIVO (1): Transfer`). Con `Transfer` detrás (como está), su RTL
todavía no existe cuando se compila `RebuildTable` y sale una llamada. **El orden
textual dentro de la clase decide si hay símbolo o no**, no sólo dónde sale.

---

## 4. Lo único que queda: `Transfer`, y por qué no ha caído

El objetivo emite, en un solo barrido:

    ... IsLaidOut, RebuildTable, **Transfer**, UpdateSearchLength, PreFlightAdd, PostFlightAdd, ...

Nosotros emitimos ese mismo bloque **sin `Transfer`**, y `Transfer` sale ~6,7 kB
más tarde, delante de `CollectionHashMap(Ui)`. Eso arrastra las 20 funciones y
los 3 saltos que quedan.

Lo que está **medido y descartado**:

* **El grafo de referencias es IDÉNTICO al del objetivo.** Barrido de
  reubicaciones de las dos `.o` (script `c36attx_who.py`): `Transfer` la
  referencia **sólo** `RebuildTable`, en las dos; y `RebuildTable`,
  `UpdateSearchLength`, `PreFlightAdd`, `PostFlightAdd`, `GetKey` y
  `~CollectionHashMap` tienen **exactamente los mismos llamantes** en las dos.
* **`RebuildTable` NO se inlina en ningún sitio en el objetivo**: las líneas
  `attribhashmap.h:656-688` no aparecen dentro de ninguna otra función del
  volcado. Tampoco las de `Transfer` (`:750-759`).
* Por el §3, para que `Transfer` salga en ese barrido hace falta
  `DECL_EXTERNAL(Transfer) == 0` **antes** del paso 6, o sea
  `TREE_SYMBOL_REFERENCED` puesto por algo emitido antes: código normal,
  plantillas de la pasada 1, vtables o el `ssdf`. **Esas cuatro cosas son byte a
  byte iguales a las del objetivo y ninguna la nombra.**

O sea: con la forma de fuente actual **no hay manera**, y la causa tiene que ser
una diferencia estructural en `class HashMap` que todavía no he encontrado
(número o reparto de sus miembros, o la clase que la contiene).

### Vedas — sentencias barridas, no reabrir sin una idea nueva

* **VEDA T2**: *«una inline sin usar que llame a `Transfer` la marca como
  referenciada»*. Falso. Puesta **detrás** de `Transfer`, GCC le inlina el cuerpo
  y no queda referencia (`.text` idéntico byte a byte, 20/3 sin mover). Puesta
  **delante** no puede funcionar por construcción: `RebuildTable` ya es esa
  función y tampoco lo consigue — `assemble_name` sólo corre al EMITIR, y una
  inline sin usar no se emite.
* **VEDA T3**: *«basta con que `Transfer` se referencie desde código normal»*.
  Probado con un diagnóstico (`HashMap::_c36attxDbg` llamada desde
  `Collection::Clear`): **no la mueve**, porque el envoltorio estaba detrás de
  `Transfer` y se la comió inlinándola (`Clear` creció 264 B). Diagnóstico mal
  montado; la sentencia sigue sin barrer del todo, pero para barrerla hay que
  meter una reubicación que el objetivo NO tiene.
* **VEDA T4**: *«`Transfer` delante de `RebuildTable`»*. Desaparece la función
  entera (se inlina). Revertido.
* **CERRADA (§3)**: `assemble_external` no marca nada en este target
  (`ASM_OUTPUT_EXTERNAL` sin definir para rs6000/sysv4).

### El bit que decide todo esto: `DECL_COMDAT`, y las tres funciones que nos SOBRAN

Para que `Transfer` salga en el primer barrido sin estar referenciada de
antemano sólo hay una vía en el §3 paso 5: **`!DECL_COMDAT`**. Y el fuente dice
exactamente cuándo pasa eso, con una prueba viva en nuestra propia unidad:

> `import_export_class` (decl2.c:2699) fija el interfaz de una clase **según su
> primera función virtual no-inline y no-abstracta**. `Attrib::Database` tiene
> `virtual ~Database();` definida en esta misma TU → `import_export = 1` →
> `CLASSTYPE_INTERFACE_KNOWN`. Y entonces `import_export_decl` (decl2.c:2890)
> llama a **`maybe_make_one_only`, que en un target con símbolos débiles NO HACE
> NADA** (decl2.c:2589) → **`DECL_COMDAT` se queda a 0** → el filtro del paso 5
> las desmarca siempre y `wrapup` las emite **aunque no las llame nadie**.

**Comprobado**: `textorder` dice `SOLO NUESTRAS (3): Get__Q26Attrib8Database`,
`__dl__Q26Attrib8DatabasePvUi`, `IsInitialized__Q26Attrib8Database` (156 B; el
enlace las estripa, por eso `stripmap` sigue dando 0), y el barrido de
reubicaciones dice que **NO LAS REFERENCIA NADIE, ni en su objeto ni en el
nuestro**. Se emiten a la fuerza, que es justo lo que predice el párrafo de
arriba. Como el objetivo **no las emite**, en el original `class Database`
**no tiene funciones inline**: `Get()`, `IsInitialized()` y `operator delete`
no están definidas dentro de la clase allí. (Arreglarlo tiene radio enorme —
`Database::Get()` se usa en todo el árbol — y no hace falta: el enlazador ya se
las lleva.)

**Y ese bit NO es la palanca de `Transfer`, medido**: si
`CLASSTYPE_INTERFACE_KNOWN(Attrib::HashMap)` estuviera puesto en el original, se
emitirían **las ~20 funciones miembro** de `HashMap` (`Add`, `Find`, `Remove`,
`Size`, `FindIndex`, el ctor, el dtor...), y el objetivo emite exactamente
**cinco**. `HashMap` es COMDAT en los dos lados. Queda descartado.

Lo que queda por buscar, entonces, es **de dónde sale la referencia a `Transfer`
antes de `finish_file` en el original**, sabiendo que no puede ser una llamada
(no hay reubicación) ni `assemble_external` (no existe en este target). El
siguiente que lo mire debería empezar por ahí y no por el orden.

---

## 5. Verificación

* `textorder.py zAttribSys` → **20 de 197, 3 saltos** (los tres, `Transfer`).
* `measure.py zAttribSys` → **48.776/48.776 B, 100,0000 %, 196 funciones**.
* `datadiff.py zAttribSys` → `.rodata` **1176/1176**, falta 0, sobra 0.
* `audit.py Speed/Indep/SourceLists/zAttribSys` → **0 FALLA**.
* `stripmap.py zAttribSys` → `test 0  +keep 0  estripado 0`, y la base del
  enlace en la misma ejecución: `9619ba57c991 OK`.
* `keepchk.py` → 22 RANCIAS, **ninguna de zAttribSys** (no he movido ninguna
  frontera de `keep.lst`).
* **`measure.py` de las 617 unidades: `3.871.396/3.946.204 B, 98,1043 %, 18.337
  funciones`** — exactamente la cifra del brief. Cero regresión.
* **`textorder.py --todas` antes y después de reconstruir las 33 SourceLists
  afectadas: `diff` VACÍO.** Ni una fila ajena se mueve — incluidas `zLua` (166)
  y `zGameplay` (694), que son las otras dos unidades que emiten
  `GetFlag__CQ26Attrib10DefinitionUi`.
* `build_direct.py` de las 33 SourceLists que ven estas cabeceras: **33 ok, 0
  fallidas** en la primera pasada.
* `trypromo.py zAttribSys` → **DOL ROTO (`b3209b6c8d3c`)**. No promociona: le
  faltan los 300 B de `Transfer` en su sitio.
* `config/GOWE69/*`, `configure.py` y `splits.txt`: **no los he abierto para
  escribir**. Los `#if defined(__ANDROID__)` de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp`: **sin tocar**.
* **No he hecho ningún commit.**

### Radio de explosión, medido

Las cuatro clases movidas se usan **sólo** dentro del árbol de AttribSys:
`grep -rn` de `Attrib::Array|Attrib::Node|DatabasePrivate|ClassTable|TypeTable|
TypeDescPtrVec|CollectionList|ClassList` fuera de `AttribSys.h`,
`AttribPrivate.h` y `AttribDatabase.cpp` no da un solo uso real. Y los
`.cpp` que las necesitan (`AttribClass.cpp`, `AttribCollection.cpp`,
`Attribute.cpp`, `AttribSupport.cpp`, `AttribInstance.cpp`,
`AttribLoadAndGo.cpp`) los incluye **sólo `zAttribSys.cpp`**. Por eso no ha hecho
falta ni una guarda: las 33 unidades compilan y `textorder --todas` no se mueve.

---

## 6. AVISO SERIO: **HEAD no compila — un commit ajeno se llevó media edición mía**

El commit **`fe41fc23` («ATTRIB_TAGS_HAND_POOL: la guarda de att, generalizada a
cualquier unidad»), que no es mío, se llevó mi `AttribSys.h` a medias**: subió el
fichero con las declaraciones adelantadas de `class Array;` / `class Node;` (mi
comentario `c36attx e1` está literalmente en el commit, −335 líneas) **pero NO
subió `AttribHashMap.h`, que es donde yo había puesto los cuerpos.**

    git show HEAD:.../AttribSys.h        -> 909 lineas, tiene "// c36attx e1" y "class Array;"
    git show HEAD:.../AttribHashMap.h    -> 327 lineas, NO tiene `class Array`

O sea que **en el árbol commiteado `Attrib::Array` y `Attrib::Node` no están
definidos en ningún sitio**. Mi copia de trabajo sí está completa y compila (33
de 33), pero cualquiera que haga `git checkout` de ese fichero, o que clone,
tiene el árbol roto. **Se arregla commiteando los otros tres ficheros de la §7**
(los tengo listos, sin commitear, como manda el encargo).

### Y un aviso menor de convivencia

`build_direct.py` de las 33 dio **32 ok, 1 fallida** en la segunda pasada:
`zBWare` con `src/Speed/Indep/bWare/Src/bMemory.cpp:145: parse error before
string constant`. **No es mío**: es un `asm()` del grupo `c36bw` a medio escribir
(saltos de línea crudos dentro de las comillas). En mi primera pasada, veinte
minutos antes, `zBWare` compilaba. Es el «la base parpadea» de la r35.

---

## 7. Ficheros de `src/` que dejo modificados (4, sin commitear)

| fichero | ensayo | qué |
|---|---|---|
| `Tools/AttribSys/Runtime/AttribSys.h` | e1, e4 | fuera `class Array`, `class Node` (326 líneas) y `TypeDescPtrVec`/`TypeTable`/`CollectionList`/`ClassList` (17); quedan las declaraciones adelantadas |
| `Tools/AttribSys/Runtime/Common/AttribHashMap.h` | e1 | entran `class Array` y `class Node`, delante de `class HashMap` |
| `Tools/AttribSys/Runtime/Common/AttribPrivate.h` | e2 | fuera `ClassTable`, `DatabaseLoadData` y `DatabasePrivate` (98 líneas); `ScanForValidKey` cae en la 247, que es la del objetivo |
| `Tools/AttribSys/Runtime/Common/AttribDatabase.cpp` | e2, e3, e4 | entran las siete clases, delante de `class DatabaseExportPolicy`; los cuatro estáticos de `DatabasePrivate` detrás del ctor y el dtor |

Cada edición lleva su comentario en el fuente diciendo **de qué línea de
`debug_lines` sale**. `SourceLists/zAttribSys.cpp` no se ha tocado: el pool
escrito a mano de la r35 sigue igual y el dato sigue al 100 %.

## 8. Guiones del scratchpad (`c36attx_`)

| guion | qué hace |
|---|---|
| `c36attx_who.py [nuestro.o] [simbolos...]` | **la herramienta de la ronda**: compara los LLAMANTES de un símbolo entre el objeto extraído y el nuestro, leyendo TODAS las secciones de reubicación (`.text`, `.data`, `.rodata`). Es lo que demostró que el grafo de referencias de `Transfer` es idéntico |
| `c36attx_fnlines.py <unidad> <base> <patrones>` | todas las líneas de `debug_lines` dentro de cada función que casa, sin repetir: dice qué se inlinó dentro de qué |
| `c36attx_lab.py <lab.cpp> [flags]` | compila un `.cpp` de laboratorio con los cflags EXACTOS de zAttribSys y saca el orden de `.text` del `.s`. Con `-finline-limit-1` se ve el modelo de pasadas en 5 s |
| `c36attx_cc.py`, `c36attx_to.py` | copias de los de la r34/r35: compilar una unidad a un `.o` suelto y pasarle `textorder` |
| `c36attx_bak/` | **copia de los cuatro ficheros ANTES de tocarlos.** No la borro a propósito: por la §6, `AttribSys.h` **ya no se puede recuperar de HEAD** (ahí está mi versión a medias); su original está en `fe41fc23^` o en esta carpeta |

Borrados los `.o`, los laboratorios y los volcados intermedios
(`c36attx_ours.py` era una copia literal de `c35ord4_ours.py`, que sigue en el
scratchpad).
