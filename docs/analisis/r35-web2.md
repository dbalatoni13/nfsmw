# r35-web2 — Contraste documental de las siete deducciones empiricas

Investigacion **en internet** (grupo `web2`, ronda 35). No se toco codigo. El objetivo era
contrastar siete reglas que dedujimos midiendo, contra documentacion primaria o practica
ajena.

Metodo: fuente primaria primero (fuente de GCC 2.95.3, fuente de decomp-toolkit y objdiff,
spec DWARF-1, manuales de SN Systems escaneados), practica ajena despues.

---

## Veredicto en una tabla

| # | Deduccion nuestra | Veredicto | Fuente decisiva |
|---|---|---|---|
| 1 | `-strip-unused-data` quita `size & ~7` y deja la cola | **SIN FUENTE** (la aritmetica). **CONFIRMADO** que descarta codigo | Manual ProDG NGC v3.5; manual ProDG PS2 (`-stripmin`) |
| 2 | Orden de emision de `finish_file` (4 sub-reglas) | **CONFIRMADA entera**, con un matiz que refina la regla | `gcc/cp/decl2.c` y `gcc/cp/decl.c` de `releases/gcc-2.95.3` |
| 3 | `#pragma interface` / `#pragma implementation` | **CONFIRMADA** | Manual GCC 2.95.3, seccion 5.4 |
| 4 | Regla del key method | **CONFIRMADA en sustancia**, **vocabulario mal** | `import_export_class` + `import_export_vtable` en `decl2.c` |
| 5 | DWARF-1 de GCC 2.9x + marcador `0xFFFFFFFF` | **REFUTADA a medias** / **SIN FUENTE** el resto | Spec DWARF 1.1.0 §4.17; `gcc/dwarfout.c`; MaskRay |
| 6 | dtk: rangos de datos, `gap_`/`pad_`, ciclos de enlace | **CONFIRMADA**, y hay **dos palancas documentadas que no usamos** | `docs/splits.md` de dtk-template; `src/util/split.rs` |
| 7 | objdiff: opciones y `data` por nombre | **CONFIRMADA**, mas **un hallazgo nuevo que nos afecta** | `objdiff-core/src/diff/data.rs`; `config-schema.json` |

---

## 1. `-strip-unused-data` de `ngcld`

### Lo que si esta documentado

El manual oficial *Getting Started with ProDG for Nintendo GameCube* v3.5 documenta **una
sola** opcion de estripado, en la tabla "Additional command line switches in ngcld":

> `-strip-unused` — "Removes unused function code"

Fuente: <https://archive.org/stream/sn_sys_consoles_2/NGC/prodgngc_gs_v35_djvu.txt> (Cap. 5,
"ProDG Linker", pag. 44-45).

Consecuencias directas:

- **`-strip-unused-data`, la bandera que pasamos nosotros, no aparece en ese manual.** Es de
  una version posterior (nosotros usamos ProDG v1.76 / v3.9x). Tampoco aparece `-keep`, que
  tambien pasamos. Ninguna de las dos esta documentada en el material publico que he
  encontrado.
- La unica opcion documentada dice explicitamente **"function code"**, lo que respalda
  nuestra observacion de que **el estripado alcanza al codigo y no solo a los datos**.

### La pista fuerte: la familia SN estripa con granularidad de 8

El manual hermano de PS2 si trae una seccion dedicada, "Dead-stripping", con las opciones de
ajuste del estripador de SN. Dos frases importan:

> `-stripmin n` — "Do not strip any item of n bytes or smaller. The default threshold is 16."

y, en el recuadro de nota inmediatamente debajo (**la frase esta cortada en el escaneo, se
pierde en el salto de pagina**):

> "We recommend that a -stripmin value less than 8 should not be used; below [...]"

Fuente: <https://archive.org/stream/sn_sys_consoles_2/PS2/ProDGforPS2_GS-V2.00_djvu.txt>
(Cap. 5, "Dead-stripping", pag. 69).

Esto **no confirma** nuestra regla `size & ~7`, pero es la primera evidencia documental de
que **el estripador de SN razona en unidades de 8 bytes** y de que tiene un umbral por
tamano. Nuestra medida (una cadena de 14 B sobrevive como sus 6 ultimos bytes, es decir se
lleva `14 & ~7 = 8`) encaja con esa granularidad. No he encontrado a nadie que haya descrito
el residuo.

La misma seccion tambien confirma otra cosa que damos por sabida:

> "Associated debug info is also removed."

Es decir, **SN documenta que el estripado se lleva tambien la informacion de depuracion del
simbolo**. Eso es el sustrato del punto 5.

### Practica ajena: no la hay

Encontre **un unico proyecto de decompilacion con exactamente nuestra cadena**: el decomp de
*The Sims 2* de GameCube, `natebag/Sims2DECOMP`, que declara "SN Systems ProDG (GCC 2.95.3,
SN BUILD v1.76)" — el mismo build de compilador que nosotros
(<https://github.com/natebag/Sims2DECOMP>).

Pero **no nos sirve para esta pregunta**: no enlazan con `ngcld`. Su `configure.py` usa el
`ld` de devkitPPC:

```
ldflags = -T $ldscript -nostdlib --allow-multiple-definition --no-check-sections --noinhibit-exec
```

<https://raw.githubusercontent.com/natebag/Sims2DECOMP/main/configure.py> (linea 333).

**Veredicto: la regla `size & ~7` sigue siendo solo nuestra, medida. El descarte de codigo
esta documentado. Seguimos con la medida empirica.**

### La via que lo cerraria (accionable, requiere tu permiso)

El **codigo fuente de ProDG for NGC v3.93 esta archivado publicamente**:

- `NGC/ProDGforNGCv393_Source_Code.zip` — 28.626.142 bytes (27,3 MB)
- item `sn_sys_consoles_2` en archive.org, servidor `ia600605.us.archive.org`, dir
  `/29/items/sn_sys_consoles_2`

Si ese zip contiene el fuente de `ngcld` (el nombre sugiere que si), **un grep resolveria de
un golpe el punto 1 y el punto 5**: la aritmetica exacta del estripado y el valor con el que
marca lo descartado en DWARF. Es la accion de mayor rendimiento de todo este informe.

No lo he descargado: es un fichero grande y una descarga necesita tu visto bueno explicito.
Dime y lo bajo. (Junto a el estan `ProDGforNGCv393.zip`, 14,2 MB, con los binarios y el
readme de esa version, que probablemente documente `-strip-unused-data` y `-keep`.)

---

## 2. Orden de emision de GCC 2.9x — **CONFIRMADA ENTERA**

Fuente primaria: el espejo de git de GCC **si llega a 2.95.3**. La rama es
`releases/gcc-2.95.3`, y los ficheros se leen en crudo:

- <https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-2.95.3/gcc/cp/decl2.c>
- <https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-2.95.3/gcc/cp/decl.c>
- <https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-2.95.3/gcc/cp/method.c>
- <https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-2.95.3/gcc/dwarfout.c>

**Esto por si solo vale la ronda: podemos leer el compilador exacto en vez de deducirlo.**

### 2a. "Todo lo inline sale en `finish_file`" — CONFIRMADA

`finish_file` (decl2.c:3666) empieza declarando su cometido:

> "We now have to write out all the stuff we put off writing out."

y el mecanismo del aplazamiento esta comentado sin ambiguedad unas lineas mas abajo:

> "We lie to the back-end, pretending that some functions are not defined when they really
> are. This keeps these functions from being put out unnecessarily."

Las inline viven en el array `saved_inlines` y se vuelcan con
`wrapup_global_declarations (&VARRAY_TREE (saved_inlines, 0), saved_inlines_used)`, en orden
**directo** del array (orden en que se guardaron).

### 2b. "Las vtables salen en bloque y en orden inverso de definicion" — CONFIRMADA, y ahora sabemos por que

Tres piezas encajan:

1. **En bloque, y aparte del resto.** Las vtables tienen su propia pasada,
   `walk_globals (vtable_decl_p, finish_vtable_vardecl, 0)`, y `wrapup_globals_for_namespace`
   las excluye a proposito (decl.c:2020):

   > "Temporarily mark vtables as external. That prevents wrapup_global_declarations from
   > writing them out; we must process them ourselves in finish_vtable_vardecl."

2. **La cadena de globales esta al reves.** `pushdecl` antepone (decl.c:1205):

   ```c
   TREE_CHAIN (decl) = b->names;
   b->names = decl;
   ```

3. **La pasada de vtables recorre esa cadena tal cual**, desde la cabeza
   (`walk_globals_r`, decl.c:1951: `t = &NAMESPACE_LEVEL (namespace)->names;`), **sin
   invertirla**.

   Y aqui esta el matiz que refina nuestra regla: `wrapup_globals_for_namespace` **si**
   invierte, y lo dice (decl.c:2008):

   > "Process the decls in reverse order--earliest first."

   Es decir: **los globales normales salen en orden de fuente; las vtables salen al reves.**
   No es que "todo salga al reves al final": es que la pasada de vtables es la unica que no
   deshace el apilado. Nuestra deduccion era correcta y ahora tiene causa.

### 2c. "`finish_file` corre en pasadas" — CONFIRMADA literalmente

El cuerpo entero de `finish_file` es un `do { ... } while (reconsider);`. La variable
`reconsider` se pone a 1 cada vez que una pasada produce trabajo nuevo. El comentario de
cabecera lo justifica:

> "All of these may cause others to be needed. For example, instantiating one function may
> cause another to be needed [...]"

**Orden dentro de cada pasada** (util para predecir el layout de una TU):

1. `instantiate_pending_templates ()`
2. sigtables (solo con `-fhandle-signatures`)
3. **vtables** (`walk_globals` + `finish_vtable_vardecl`)
4. `static_aggregates` -> funcion de duracion estatica (`__sinit`)
5. **sintesis de metodos implicitos** (`synthesize_method` / `synthesize_tinfo_fn`)
6. `wrapup_global_declarations (saved_inlines)` -> **las inline**
7. `walk_namespaces (wrapup_globals_for_namespace)` -> globales normales
8. `pending_statics` (miembros estaticos)

Fuera del bucle, al final: `generate_ctor_and_dtor_functions_for_priority` (ctors/dtors) y
`prune_vtable_vardecl`.

### 2d. "Un destructor implicito se emite POR DELANTE de las inline declaradas" — CONFIRMADA

El paso 5 va antes que el paso 6 dentro de la misma pasada. Y la sintesis **emite en el
acto**, no encola: `synthesize_method` (method.c:2612) termina llamando a
`finish_function (lineno, 0, nested)` (method.c:2659). El filtro del bucle es exactamente el
de un metodo implicito:

```c
if (DECL_ARTIFICIAL (decl) && ! DECL_INITIAL (decl) && TREE_USED (decl) ...)
```

Un destructor implicito es `DECL_ARTIFICIAL` sin cuerpo hasta que se sintetiza. Por eso sale
delante de las inline que el usuario si escribio. **Confirmada con el mecanismo exacto.**

---

## 3. `#pragma interface` / `#pragma implementation` — **CONFIRMADA**

Manual de GCC 2.95.3, "Extensions to the C++ Language", seccion 5.4
(<https://gcc.gnu.org/onlinedocs/gcc-2.95.3/gcc_5.html>).

Lo que `#pragma interface` suprime, textualmente, es:

> "backup copies of inline member functions, debugging information, and the internal tables
> that implement virtual functions"

y en su lugar

> "the object files will contain references to be resolved at link time."

`#pragma implementation` hace lo contrario: esas tres cosas "are all generated in
implementation files". Ademas, en una cabecera bajo `#pragma interface` una funcion inline se
trata como un `extern`:

> "the compiler emits no code at all to define an independent version of the function"

**Los tres efectos que nos importan (inline, vtables, debug) estan documentados juntos y
exactamente como los usamos.** Nota operativa: esto interactua con el punto 4 — un
`#pragma interface` pone `CLASSTYPE_INTERFACE_KNOWN` y **cortocircuita** la heuristica del key
method (ver abajo).

---

## 4. La regla del "key method" — **CONFIRMADA en sustancia, vocabulario mal**

La regla existe literalmente en GCC 2.95.3, en `import_export_class` (decl2.c:2679):

> "Base our import/export status on that of the first non-inline, non-abstract virtual
> function, if any."

y el filtro es exactamente ese:

```c
if (DECL_VINDEX (method) != NULL_TREE
    && !DECL_THIS_INLINE (method)
    && !DECL_ABSTRACT_VIRTUAL_P (method))
  {
    import_export = (DECL_REALLY_EXTERN (method) ? -1 : 1);
    break;
  }
```

Luego:

```c
SET_CLASSTYPE_INTERFACE_KNOWN (ctype);
CLASSTYPE_VTABLE_NEEDS_WRITING (ctype) = (import_export > 0);
```

y `import_export_vtable` traduce eso a:

```c
DECL_EXTERNAL (decl) = ! CLASSTYPE_VTABLE_NEEDS_WRITING (type);
```

con lo que `finish_vtable_vardecl` (cuya guarda es `if (! DECL_EXTERNAL (vars) && ...)`) **no
la escribe**.

### La correccion de vocabulario, que ademas es un diagnostico util

Nuestra frase era "una virtual declarada no-pura y sin definir **deja la clase sin key
method**". Es al reves, y la diferencia importa porque **los dos casos tienen sintomas
distintos**:

| Situacion | Que hace GCC 2.95.3 | Sintoma en el DOL |
|---|---|---|
| Hay una virtual no-inline y no-pura, **declarada pero nunca definida en ninguna TU** | Esa **si** es el key method. `DECL_REALLY_EXTERN` -> `import_export = -1` -> todas las TU la **importan** | **La vtable no existe en ningun sitio** |
| **No hay ninguna** virtual no-inline no-pura (todas inline o todas puras) | `import_export == 0`, no se fija `CLASSTYPE_INTERFACE_KNOWN`; en `finish_file` cae a `comdat_linkage` | **La vtable sale weak/COMDAT en cada TU que la use** |

Es decir: la clase **si** tiene key method; lo que falta es su **definicion**. El resultado
observable que dedujimos (la vtable no se emite en ninguna unidad) es correcto.

**Accionable:** si a una vtable no la emite nadie, busca la **primera** virtual no-inline y
no-pura de `TYPE_METHODS` (orden de declaracion en la clase) y define esa, no otra. Si en
cambio ves la vtable duplicada weak en muchas TU, es que **no hay ninguna** virtual
no-inline no-pura — todas son inline o puras.

Confirmacion de epoca desde fuera del fuente: el g++ FAQ de la era 2.x describe el mismo
truco ("since gcc-2.7.0 [...] the .o file containing the definition for the first non-inline
virtual function for the class will also contain the virtual function table"), y la
documentacion moderna de lld lo llama ya por su nombre, key function
(<https://lld.llvm.org/missingkeyfunction.html>).

---

## 5. DWARF-1 tal como lo emite GCC 2.9x — **REFUTADA a medias**

### 5a. Las secciones: REFUTADA (dos de las cinco no son DWARF-1)

Descargue la spec **DWARF 1.1.0** (UNIX International, 6 octubre 1992,
<https://dwarfstd.org/doc/dwarf_1_1_0.pdf>) y extraje su texto. La seccion §4.17
"Dependencies" enumera **exactamente cuatro** secciones:

> "The debugging information in this format is intended to exist in the .debug,
> .debug_aranges, .debug_pubnames, and .line sections of an object file."

**`.debug_sfnames` y `.debug_srcinfo` NO estan en la spec.** Busque los dos literales en el
texto completo: 0 ocurrencias.

Son **extension propia de GCC**, definidas en `gcc/dwarfout.c` (lineas 503-522):

```c
#define DEBUG_SECTION    ".debug"
#define LINE_SECTION     ".line"
#define SFNAMES_SECTION  ".debug_sfnames"
#define SRCINFO_SECTION  ".debug_srcinfo"
#define MACINFO_SECTION  ".debug_macinfo"
#define PUBNAMES_SECTION ".debug_pubnames"
#define ARANGES_SECTION  ".debug_aranges"
```

<https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-2.95.3/gcc/dwarfout.c>

**Consecuencia practica:** para entender `.debug_sfnames` / `.debug_srcinfo` **la spec no
sirve; el unico documento valido es `dwarfout.c`**, que ahora sabemos leer. Ahi esta tambien
`.debug_macinfo`, que quiza estemos ignorando.

### 5b. El marcador `0xFFFFFFFF`: SIN FUENTE para `ngcld`, pero tiene nombre propio

- **La spec DWARF-1 no dice nada.** Busque `0xffffffff`, `ffffffff`, `discard`, `removed`,
  `strip`: 0 ocurrencias en las tres primeras, 0 en las otras.
- **SN si documenta la causa**, aunque no el valor: "Associated debug info is also removed"
  (manual PS2, "Dead-stripping").
- **La practica tiene nombre estandar: *tombstone value*.** Es exactamente nuestro caso, y
  `0xFFFFFFFF` es exactamente el valor que eligio lld:

  > "There are, however, instances where discarded text sections can lead to some placeholder
  > values (sometimes referred to as tombstone values) in DWARF."

  y la tabla por enlazador de ese mismo articulo da, para `.debug_*`: GNU ld usa `0`, y
  "future ld.lld: 0xffffffff or 0xffffffffffffffff".

  Fuente: <https://maskray.me/blog/2021-02-28-linker-garbage-collection>. Ver tambien
  <https://reviews.llvm.org/D81784>.

**Veredicto:** nuestro marcador **no esta documentado para `ngcld`**, pero **no es una rareza
nuestra**: es la practica estandar del sector, con nombre propio, y `ngcld` eligio el mismo
valor que lld en vez del `0` de GNU ld. Seguimos usandolo como marcador con tranquilidad.

### 5c. Un contraste util: dtk conoce OTRO mecanismo, el de MWLD

`decomp-toolkit` tiene una bandera `--include-erased` en `dwarf dump`, descrita en su README
como servir para "include data that was stripped by MWLD"
(<https://github.com/encounter/decomp-toolkit>).

Pero **el mecanismo que reconstruye no es el nuestro**. Segun `src/util/dwarf/mod.rs`, MWLD
borra la entrada convirtiendola en **padding** (tag `0x0000`), y dtk tiene que adivinar
incluso el endianness:

> "Erased entries that have become padding could be either little-endian or big-endian, and we
> have to guess the length and tag of the first entry."

<https://raw.githubusercontent.com/encounter/decomp-toolkit/main/src/util/dwarf/mod.rs>

Es decir: **MWLD anula el tag; `ngcld` deja el tag y pone la direccion a `0xFFFFFFFF`.** Son
dos estrategias distintas. `dtk dwarf dump --include-erased` **no nos va a recuperar nada**
que ya no veamos, porque nuestras entradas descartadas no son padding. Nuestro marcador es
mas facil de explotar que el suyo, no menos.

---

## 6. decomp-toolkit — **CONFIRMADA**, y dos palancas documentadas que no usamos

Documentacion oficial del formato: `docs/splits.md` de `dtk-template`
(<https://github.com/encounter/dtk-template/blob/main/docs/splits.md>).

### Rangos de datos: es el uso normal y documentado

El ejemplo canonico del propio fichero declara rango por seccion y por unidad, datos
incluidos:

```
path/to/file.cpp:
	.text       start:0x80047E5C end:0x8004875C
	.ctors      start:0x803A54C4 end:0x803A54C8
	.data       start:0x803B1B40 end:0x803B1B60
	.bss        start:0x803DF828 end:0x803DFA8C
	.bss        start:0x8040D4AC end:0x8040D4D8 common
```

Atributos de seccion documentados: `start:`, `end:`, `align:`, `rename:`, `common`, `skip`.

### `gap_` / `pad_`: confirmados, y **no son lo mismo**

Los dos nombres salen del fuente de dtk, `src/util/split.rs`, y se generan en momentos
distintos:

- `gap_{section:02}_{addr:08X}_{sec}` — en `create_gap_splits`, para el hueco **entre dos
  splits existentes** (split.rs:1012).
- `pad_{section:02}_{addr:08X}_{sec}` — en `add_padding_symbols`, "Add padding symbols to fill
  in gaps between splits and symbols", para el **arranque de un split que no empieza en un
  simbolo** (split.rs:905).

<https://raw.githubusercontent.com/encounter/decomp-toolkit/main/src/util/split.rs>

Esto coincide exactamente con lo que dice nuestro `docs/HERRAMIENTAS.md` sobre `keepchk.py`
(`gap_` entre simbolos, `pad_` al principio o al final del rango). **Confirmado desde el
fuente.**

### Ciclos de orden de enlace: confirmados, con la causa y el mensaje exacto

El orden de enlace se resuelve con un **orden topologico** (`src/util/toposort.rs`,
implementacion DFS de Simon Lindholm) sobre un grafo donde cada seccion impone aristas:

> "The ordering of TUs inside of each section represents a directed edge in a DAG. We can use a
> topological sort to determine a valid global TU order."

y cuando no hay orden posible, el error es literalmente:

> "Cyclic dependency encountered while resolving link order: {a} -> {b} -> ..."

(split.rs:1296). **Por eso los rangos de datos provocan ciclos**: cada seccion adicional que
declaras en `splits.txt` mete aristas nuevas, y `.data` puede ordenar dos TU al reves que
`.text`.

dtk ya documenta un caso concreto de ese choque, para `extabindex` (split.rs:150):

> "To avoid contradictory ordering constraints (which cause cyclic dependency errors in link
> order resolution), we group such reversed entries and assign them a single TU name."

### Las dos palancas que **no** estamos usando

1. **`order:` en la linea de la unidad.** Documentado en `docs/splits.md`:

   > "Allows influencing the resolved link order of objects. This is **not required**, as
   > decomp-toolkit will generate the link order automatically. This can be used to fine-tune
   > the link order for ambiguous cases."

   Uso concreto, tal cual lo trae la doc:

   ```
   file1.cpp: order:0
   file2.cpp: order:1
   file3.cpp: order:2
   ```

   **Esto es la respuesta directa a los ciclos que nos dan los rangos de datos**: en vez de
   quitar el rango `.data` (que es lo que veniamos haciendo, y que se lleva por delante la
   medida), anclas las unidades implicadas con `order:` y el toposort deja de tener libertad
   para contradecirse. Recomiendo probarlo en el primer ciclo que vuelva a aparecer.

2. **`skip` en la linea de la seccion.** Documentado como:

   > "Skips this data when writing the object file. Used for ignoring data that's
   > linker-generated."

   Candidato natural para datos que el enlazador fabrica y que nosotros no debemos emitir.

### `keep.lst`: no es de dtk

`keep.lst` **no aparece en decomp-toolkit ni en dtk-template**. Es el fichero de nuestro
`-keep` de `ngcld`, que como dije en el punto 1 tampoco esta documentado en el manual v3.5.
Nuestro `scripts/keepchk.py` es, hasta donde alcanza esta busqueda, **la unica herramienta que
existe para validarlo**. No hay practica ajena que copiar aqui.

---

## 7. objdiff — **CONFIRMADA**, mas un hallazgo que nos afecta a la medida

Esquema oficial de opciones:
<https://github.com/encounter/objdiff/blob/main/objdiff-core/config-schema.json>

### Las dos opciones que preguntabas

| id | tipo | por defecto | descripcion oficial |
|---|---|---|---|
| `functionRelocDiffs` | choice | **`name_address`** | "How relocation targets will be diffed in the function view." Valores: `none`, `name_address`, `data_value`, `all` |
| `ppc.calculatePoolRelocations` | boolean | **`true`** | "Display pooled data references in functions as fake relocations." |

Confirma nuestra nota de memoria sobre `ppc.calculatePoolRelocations`: **por defecto esta
encendida y fabrica reubicaciones falsas** para las referencias a datos agrupados. Esas
reubicaciones inventadas entran luego en el diff y pueden marcar diferencia donde no la hay.
Ponerla a `false` las quita. La deduccion era correcta y ahora tiene la descripcion oficial
detras.

### Por que `data` empareja por nombre — CONFIRMADO en el fuente

`objdiff-core/src/diff/data.rs`, en `diff_data_section`, lo dice en el comentario:

> "Use the highest match percent between two options: - Left symbols matching right symbols by
> name - Diff of the data itself. We only do this when all relocations on the left side match."

<https://raw.githubusercontent.com/encounter/objdiff/main/objdiff-core/src/diff/data.rs>

O sea: el porcentaje de una seccion de datos es el **maximo** entre el emparejamiento por
nombre de simbolos y la comparacion de bytes cruda, y el segundo **solo se considera si todas
las reubicaciones del lado izquierdo casan**.

### Hallazgo nuevo: objdiff **recorta** la seccion al ultimo simbolo

Esto no lo buscaba y creo que es lo mas importante del punto 7. En la misma funcion, antes de
comparar:

```rust
let left_max = symbols_matching_section(&left_obj.symbols, left_section_idx)
    .filter_map(|(_, s)| s.address.checked_sub(left_section.address).map(|a| a + s.size))
    .max()
    .unwrap_or(0)
    .min(left_section.size);
...
let left_data = &left_section.data[..left_max as usize];
```

**Los bytes que quedan por detras del final del ultimo simbolo de la seccion no se comparan
en absoluto.** No cuentan ni como acierto ni como fallo: son invisibles para el porcentaje.

Esto es la explicacion a nivel de fuente de nuestra nota "El 100% que no compara". Y convierte
los simbolos `gap_`/`pad_` de dtk en algo que **no es solo cosmetico ni solo del enlace**: si
la cola de una seccion de datos no esta cubierta por ningun simbolo, esa cola **no se esta
midiendo**, y un "100%" de esa seccion puede estar escondiendo bytes. Merece un barrido:
comparar, por seccion de datos, `max(addr+size de sus simbolos)` contra `section.size`.

### Un ajuste gratis que quiza no tengamos puesto

El esquema trae:

| id | valor | descripcion oficial |
|---|---|---|
| `demangler` | `gnu_legacy` | "Use the old GNU mangling ABI. **Used up to g++ 2.9.x**" |

Es literalmente nuestro compilador. **Comprobado: nuestro `objdiff.json` no fija `demangler`,
o sea que esta en `auto`.** Si `auto` adivina mal, los nombres de simbolo saldran mal en la
vista y — mas importante, visto el apartado anterior — **el emparejamiento por nombre de la
seccion de datos se degrada**. Vale la pena fijarlo explicitamente a `gnu_legacy`.

---

## Resumen de lo accionable, por orden de rendimiento

1. **Pedir permiso para bajar `ProDGforNGCv393_Source_Code.zip` (27,3 MB) de archive.org.** Si
   trae el fuente de `ngcld`, cierra de golpe el punto 1 (aritmetica del estripado) y el punto
   5 (valor del marcador). Es la unica via que convierte dos "sin fuente" en documentacion.
2. **Barrer las colas no medidas de las secciones de datos** (`max(addr+size)` vs
   `section.size`, por seccion). objdiff no las compara; pueden estar escondiendo bytes detras
   de un 100%.
3. **Usar `order:` en `splits.txt` para romper ciclos** en vez de quitar el rango de datos.
   Esta documentado y es exactamente para eso.
4. **Fijar `demangler: gnu_legacy` en la config de objdiff** — es el ajuste hecho para g++
   2.9.x.
5. **Leer `gcc/cp/decl2.c`, `decl.c`, `method.c` y `dwarfout.c` de `releases/gcc-2.95.3`
   directamente** cuando dudemos del orden de emision. Ya no hace falta deducirlo: el
   compilador exacto se lee en crudo desde `raw.githubusercontent.com`.
6. **Revisar el diagnostico de vtables ausentes** con la tabla del punto 4 (falta la
   *definicion* del key method, no el key method).
7. **Mirar `.debug_macinfo`**, que GCC 2.95.3 emite y que no aparece en nuestras notas.

## Lo que queda sin fuente (y por tanto sigue siendo medida nuestra)

- La aritmetica `size & ~7` del estripado de `ngcld`.
- Que `ngcld` marque con `0xFFFFFFFF` (el *que* no esta documentado; el *tipo* de practica si,
  y se llama tombstone value).
- Las banderas `-strip-unused-data`, `-report-unused` y `-keep` de `ngcld`: ninguna aparece en
  el manual publico v3.5, que solo documenta `-strip-unused`.

## Aviso sobre una fuente que NO nos sirve

`natebag/Sims2DECOMP` tiene un `docs/matching-patterns.md` con un catalogo de trucos
(inline asm para forzar `r0`, `stmw`/`lmw` a mano, `lis+addi` en vez de `lis+ori`...). **Son
anti-patrones para nosotros.** Ese proyecto compila con GCC moderno (devkitPPC) y usa esos
hacks para imitar a SN; **nosotros tenemos `ngccc`, el compilador de verdad**. Copiar de ahi
seria meter inline asm donde basta con escribir C correcto. Lo unico aprovechable de ese
repo son sus flags de verificacion: `-O2 -fno-elide-constructors -msdata=eabi -G 8`.
**Comprobado: nosotros NO pasamos `-fno-elide-constructors`** (cero ocurrencias en
`configure.py` y en `build.ninja`). Otro juego retail de GameCube con el mismo build exacto
del compilador (SN BUILD v1.76) si lo necesito para casar. **Es una bandera que merece una
prueba**: afecta a la elision de copias de temporales, y una construccion de mas o de menos
en un retorno por valor es justo el tipo de diferencia que nos deja funciones al 90-y-pico
sin causa aparente.

---

*Fuentes principales: fuente de GCC 2.95.3 (`gcc-mirror/gcc`, rama `releases/gcc-2.95.3`);
`encounter/decomp-toolkit` y `encounter/dtk-template`; `encounter/objdiff`; spec DWARF 1.1.0
(dwarfstd.org); manuales ProDG de SN Systems (item `sn_sys_consoles_2` en archive.org); manual
de GCC 2.95.3 (gcc.gnu.org/onlinedocs); MaskRay, "Linker garbage collection".*
