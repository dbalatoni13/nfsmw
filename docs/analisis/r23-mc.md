# Ronda 23 — realmemcard: los 28 B de `.rodata` que bloquean cuatro unidades

## 0. Estado de partida (medido, no heredado)

`build_direct.py realmemcard` (15 unidades; 3 fallaron la primera vez por
`Could not open output file` — otro proceso tenía el `.o` abierto; se
reintentaron y sólo `gc/trctasks.o` quedó bloqueado, ver §7).

    python scripts/measure.py -o c23mc_antes.json realmemcard
    15 unidades  69728/69728 B  100.0000%  253 funciones al 100%

`audit.py` sobre las 15 unidades, **una pasada** y **segunda pasada de
confirmación** sobre el único fallo:

| unidad | veredicto |
|---|---|
| las 14 restantes | todo `ok` |
| `gc/gc_interface` | **1 FALLA**, confirmado en segunda pasada |

    __Q26Realmc11GCInterfaceRCQ26Realmc15SystemInterface
      FALLA: reubicacion a otro simbolo: Clear__Q26Realmc9GCMessage contra Init__Q26Realmc9GCMessage

Es **preexistente** y ajeno al encargo (`gc_interface` está bloqueada por `asd2`
según r22 §6). No lo he tocado.

`trypromo.py` de partida (con `TEMP` privado, ver §7):

| prueba | resultado |
|---|---|
| enlace base sin sustituir nada | `DOL OK 9619ba57…` |
| las cuatro juntas | `DOL ROTO (351de08f0ed4)` |
| cada una suelta | `DOL ROTO (fc9e604fe105)` — **el mismo hash las cuatro** |

El DOL sale **32 B más largo** y con 16.490 tramos distintos: los 28 B de
`.rodata` (alineados a 32) se insertan y desplazan todo lo que va detrás.

## 1. El diagnóstico del encargo estaba a medias — y la dirección era otra

El encargo decía: «los 28 B viven en el DOL en 0x80414398, en **una sola
copia**». Buscando los bytes exactos en el ELF original:

    .rodata  0x80414398
    .rodata  0x80414408
    .rodata  0x804144C0
    .rodata  0x80414790
    .rodata  0x804147AC
    .rodata  0x804147C8

**SEIS copias, no una.** Y 0x80414398 no es la copia «buena»: es la de
`gc_memcard_interface_impl`, cuya `.rodata` entera (96 B, hasta
`_vt.Q26Realmc9GCMessage` en 0x804143E0) está sin atribuir en `splits.txt`.

La copia que **sí está atribuida** es la de **`gc/trctasks`**: es la única
unidad de realmemcard cuyo objeto extraído tiene `.rodata` (0x24 = 36 B), con
`lbl_804147C8`…`lbl_804147E8`, y **nuestro `trctasks.o` la emite ya byte a
byte idéntica**. No hay nada que mover ahí.

Reparto de las seis copias por el orden de `.rodata` del DOL:

| dirección | unidad dueña | prueba |
|---|---|---|
| 0x80414398 | `gc/gc_memcard_interface_impl` | su `.o` extraído referencia `lbl_80414398`, `lbl_8041439C`, `lbl_804143B4/B8/BC/D8`; acaba en `_vt.Q26Realmc9GCMessage` |
| 0x80414408 | `gc/gc_driver` | el bloque acaba en `_vt.Q26Realmc8GCDriver`/`GcFileDescriptor`/`CmnFileDescriptor`/`DeviceDriver` |
| 0x804144C0 | `gc/gc_interface` | acaba en `_vt.Q26Realmc11GCInterface` y las 10 vtables de tareas |
| 0x80414790 | `gc/public` | su `.o` extraído referencia `lbl_80414790` (2 veces) |
| 0x804147AC | `gc/tasks` | hueco entre `public` y `trctasks` |
| 0x804147C8 | `gc/trctasks` | atribuida: `lbl_804147C8`…`lbl_804147E8` |

## 2. La causa: no es «sacar los inlines», es QUIÉN VE LA CLASE

Caso mínimo, 6 líneas, un segundo de compilación (`c23mc/t1..t3.cpp`):

| forma | ¿emite la cadena sin que nadie llame a la función? |
|---|---|
| cuerpo **dentro** de la clase | **SÍ** (`.rodata 0x0c`) |
| cuerpo **fuera** de la clase con `inline`, mismo TU | **SÍ** (`.rodata 0x0c`) |
| **sólo la declaración** | **NO** (`.rodata 0`) |

GCC 2.95 genera RTL de todo cuerpo `inline` visible aunque nadie lo llame, y
al hacerlo materializa sus literales. Así que **sacar el cuerpo de la clase al
`.cpp` no arregla nada por sí solo**: lo que decide es si el TU ve el cuerpo.

Y el DWARF dice exactamente qué TU veía `struct GCMessage` en el original
(censo de `struct` por unidad de compilación sobre `mw_dwarfdump.nothpp`):

| ve `GCMessage` en el original | no lo ve |
|---|---|
| `gc_driver.cpp` | `memcard_interface.cpp` |
| `gc_interface.cpp` | `memcard_interface_impl.cpp` |
| `gc_memcard_interface_impl.cpp` | `memcard_taskmanager.cpp` |
| `gc_public.cpp` | `gc_memcard_taskmanager.cpp` |
| `gc_tasks.cpp` | `interfaceimp.cpp` |
| `gc_trctasks.cpp` | `gc_blockcalculator.cpp` |

**Seis y seis.** Los seis de la izquierda = las seis copias del DOL. Los seis
de la derecha = las seis unidades nuestras que emiten los 28 B de más.

Nuestro árbol mete `GCMessage`, `FindResult` y `GCInterface` en
`impl/memcard_interface_impl.h`, que **todos** incluyen. En el original vivían
en una cabecera aparte que sólo incluyen los seis de la izquierda.

Inventario de `.rodata` medido (nuestro contra extraído, bytes):

| unidad | nuestra | extraída | ¿bloque de 28 B? | ¿lo tiene el DOL? |
|---|---|---|---|---|
| `cmn/interfaceimp` | 536 | 0 | sí | **NO** |
| `cmn/memcard_interface` | 28 | 0 | sí | **NO** |
| `cmn/memcard_interface_impl` | 28 | 0 | sí | **NO** |
| `cmn/memcard_taskmanager` | 28 | 0 | sí | **NO** |
| `gc/gc_blockcalculator` | 128 | 0 | sí | **NO** |
| `gc/gc_memcard_taskmanager` | 28 | 0 | sí | **NO** |
| `gc/gc_driver` | 184 | 0 | sí | sí (0x80414408) |
| `gc/gc_interface` | 712 | 0 | sí | sí (0x804144C0) |
| `gc/gc_memcard_interface_impl` | 88 | 0 | sí | sí (0x80414398) |
| `gc/public` | 28 | 0 | sí | sí (0x80414790) |
| `gc/tasks` | 28 | 0 | sí | sí (0x804147AC) |
| `gc/trctasks` | 36 | 36 | sí | sí (0x804147C8) |
| `cmn/locale`, `cmn/memcard_memvectors`, `cmn/memcard_utilities` | 0/0/8 | 0 | no | — |

Son **seis** unidades de más, no cuatro: el encargo se dejaba `interfaceimp` y
`gc_blockcalculator` (que además tienen vtables sin atribuir y no promocionan
por eso, pero el bloque sobra igual).

## 3. El arreglo (ensayo c1) — partir la cabecera

`src/…/include/common/realmemcard/impl/gc_interface_impl.h` **(nuevo, 241 líneas)**

    #ifndef REALMEMCARD_IMPL_GC_INTERFACE_IMPL_H
    …
    #include "memcard_interface_impl.h"
    namespace Realmc {
      struct GCMessage : public Message { … };   // movido tal cual, 226 líneas
      struct FindResult { … };
      struct GCInterface : public InterfaceImp { … };
    } // namespace Realmc
    #endif

`impl/memcard_interface_impl.h`: se le quitan esas 226 líneas (1809 → 1583).
Nada más cambia: el bloque va **verbatim**, en el mismo orden.

Los seis `.cpp` que sí lo necesitan pasan a incluir la cabecera nueva:

| fichero | cómo |
|---|---|
| `gc/gc_driver.cpp` | `#include` añadido tras `gc_driver.h` (antes lo veía por `gc_driver.h → driver.h → memcard_interface_impl.h`) |
| `gc/gc_interface.cpp` | `#include` añadido tras `impl/interfaceimp.h` |
| `gc/gc_memcard_interface_impl.cpp` | el `#include` de `memcard_interface_impl.h` **sustituido** por el nuevo |
| `gc/public.cpp` | idem |
| `gc/tasks.cpp` | idem, **dentro** del `#define`/`#undef REALMC_GC_MESSAGE_INLINE` |
| `gc/trctasks.cpp` | idem, **dentro** del `#define`/`#undef REALMC_GC_MESSAGE_INLINE` |

**Cierre transitivo comprobado**: `memcard_interface_impl.h` sólo lo alcanzan
las 15 unidades de realmemcard — `grep -rn` no lo encuentra fuera del paquete, y
sólo esas 15 unidades llevan la `-I …/realmemcard/…/include/common` en
`build.ninja` (contado con `build_direct.parse_units`). El radio de la cabecera
es exactamente lo que he medido.

### Resultado

`.rodata` nuestra, antes → después (extraída entre paréntesis):

| unidad | antes | después | extraída |
|---|---|---|---|
| `cmn/memcard_interface` | 28 | **0** | 0 |
| `cmn/memcard_interface_impl` | 28 | **0** | 0 |
| `cmn/memcard_taskmanager` | 28 | **0** | 0 |
| `gc/gc_memcard_taskmanager` | 28 | **0** | 0 |
| `cmn/interfaceimp` | 536 | **504** | 0 (sin atribuir) |
| `gc/gc_blockcalculator` | 128 | **96** | 0 (sin atribuir) |
| `gc/gc_driver`, `gc/gc_interface`, `gc/gc_memcard_interface_impl`, `gc/public`, `gc/tasks`, `gc/trctasks` | — | **sin cambio** | conservan su copia, que el DOL sí tiene |

`.text`: **ninguna unidad cambia de tamaño**. La `.rodata` de `gc/trctasks`
sigue byte a byte idéntica a la extraída (`diff` de `objdump -s`).

### Medidas

    python scripts/measure.py --cmp c23mc_antes.json c23mc_c1.json
    +0 B, +0 funciones, 0 unidades cambian
    (15 unidades  69728/69728 B  100.0000%  253 funciones al 100%, igual que antes)

    python scripts/pctsnap.py --cmp c23mc_pct_antes.json c23mc_pct_c1.json
    EMPEORAN: ninguna
    MEJORAN: 0 funciones

`pctsnap.py` sólo sabe leer las 33 SourceLists (`unidades()` aborta con
`unidad desconocida`), así que la instantánea la toma un clon suyo del
scratchpad, `c23mc_pct.py`, que produce **el mismo formato JSON** y se compara
con el `--cmp` de la herramienta de verdad.

`audit.py`, las 15 unidades después del cambio: **el mismo y único FALLA** que
antes (`gc_interface`, `Clear` contra `Init`), confirmado con segunda pasada.
Ninguno nuevo.

## 4. Las promociones — `trypromo.py` en verde

    python scripts/trypromo.py <las cuatro juntas>      -> DOL OK   (dos veces)
    python scripts/trypromo.py <cada una suelta>        -> DOL OK   (las cuatro)
    enlace base sin sustituir nada, DESPUÉS del cambio  -> DOL OK

El enlace base es la prueba de que las tres unidades ya `Matching`
(`trctasks`, `locale`, `memcard_memvectors`, que entran por NUESTRO objeto) no
se han movido: sale `9619ba57…` igual que antes de tocar nada.

`promote.py` también da **LIMPIA** a las cuatro (esta vez coincide con el enlace).

### Líneas exactas de `configure.py` — cambiar `NonMatching` por `Matching`

| línea | objeto | B de código |
|---|---|---|
| **1286** | `Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_interface.cpp` | 1.736 |
| **1290** | `Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_interface_impl.cpp` | 1.716 |
| **1306** | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_memcard_taskmanager.cpp` | 304 |
| **1314** | `Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_taskmanager.cpp` | 6.624 |

**Total: 10.380 B y 4 ficheros.** Las cuatro son independientes: van juntas o
sueltas, en cualquier combinación.

Congeladas con `frozen.py cong` las cuatro **y `gc/trctasks`** (que entra en el
enlace y no puede moverse): `frozen.py ls` pasa de 64 a **69**.

## 5. Lo que sólo se arregla en `splits.txt` (VEDADO, no lo he tocado)

Toda la `.rodata` de realmemcard menos la de `trctasks` está sin atribuir
(`config/GOWE69/splits.txt:1791-1854`: sólo la línea 1846
`.rodata start:0x804147C8 end:0x804147EC`). El reparto se deduce del orden de
`.text` y de los símbolos de `symbols.txt`, y **cuadra al byte** con lo que
emiten nuestros objetos:

| unidad | rango de `.rodata` en el DOL | B | nuestra `.rodata` |
|---|---|---|---|
| `gc/gc_memcard_interface_impl` | `0x80414398`–`0x80414408` | 112 | 88 (faltan 3 literales, §6) |
| `gc/gc_driver` | `0x80414408`–`0x804144C0` | 184 | **184, exacto** |
| `gc/gc_interface` | `0x804144C0`–`0x80414790` | 720 | 712 |
| `gc/public` | `0x80414790`–`0x804147AC` | 28 | **28, exacto** |
| `gc/tasks` | `0x804147AC`–`0x804147C8` | 28 | **28, exacto** |
| `cmn/interfaceimp` | `0x804147F0`–`0x804149E8` | 504 | **504, exacto** |
| `gc/gc_blockcalculator` | `0x804149E8`–`0x80414A18` | 48 | 96 (emitimos `_vt.Q26Realmc15BlockCalculator` de más; el original lo pone en `gc_interface`, en `0x80414760`) |

Cuatro de las siete casan **al byte** ya. Si algún día se decide tocar
`splits.txt`, `gc_driver`, `public`, `tasks` e `interfaceimp` son las que menos
trabajo de fuente necesitan después.

## 6. Hallazgos de paso, medidos y NO aplicados

1. **`gc_memcard_interface_impl` no define `RealmcIface::ALL_ENTRIES`.**
   El objeto extraído lo tiene en `.sdata+0` (`_11RealmcIface.ALL_ENTRIES`,
   4 B) apuntando a `lbl_804143B4` = **`"*"`**; nuestra cabecera sólo lo declara
   (`memcard_interface_impl.h:1215`, `extern const char *ALL_ENTRIES;`) y quien
   lo usa es `memcard_taskmanager.cpp:30`. Por eso `trypromo` de esa unidad da
   `ENLACE FALLA: Reference to undefined symbol _11RealmcIface.ALL_ENTRIES`.
   La definición que falta es `const char *ALL_ENTRIES = "*";`.
2. **Y le sobran los estáticos de `_LcGetSlotString`**: emitimos `slotA.864` y
   `slotB.865` en su `.sdata` (8 B) mientras el original los tiene en la
   `.sdata` de `memcard_memvectors` (`0x804FF668`/`0x804FF66C`), que nuestro
   `memcard_memvectors.cpp` ya define con alias.
3. **A `gc_memcard_interface_impl` le faltan además dos literales que nadie
   referencia**: `lbl_804143F8` y `lbl_80414400`. Son otra vez el mismo
   mecanismo (un `inline` visible que nadie llama), esta vez a favor del
   original.
   **No he tocado nada de esto**: la unidad seguiría bloqueada por `splits.txt`
   (§5), así que sería trabajo con cero ficheros de rendimiento y riesgo de
   mover `.text`.
4. **`gc_driver` sigue bloqueada por `.text`**, no por datos: 9.144 B nuestros
   contra 8.952 extraídos. Son los 192 B de `operator new`/`delete` de clase que
   la r22 §9 ya midió como inexistentes en el DOL.
5. **`hd_device.cpp` ya no tiene ensamblador escrito a mano.** El encargo lo
   daba por pendiente; el fichero de hoy tiene el destructor en C
   (`GcHdFileDeviceDriver::~GcHdFileDeviceDriver() {}`) y el hueco de `.bss`
   como `int gap_07_804D5014_bss __attribute__((section(".bss")))`. **Otro
   agente lo está tocando en esta misma ronda** (`git diff` sobre el árbol de
   trabajo: −64 líneas, incluidos los `asm("\tstwu 1,-8(1)…")`). No lo he tocado.

## 7. Notas de herramienta (para quien venga detrás)

- **`trypromo.py` usa `%TEMP%` compartido** para su `.rsp`/`.elf`/`.dol`. Con
  ocho agentes eso es una carrera: la primera tanda me dio **el mismo hash roto
  (`fc9e604fe105`) para cuatro sustituciones distintas**. Ejecutarlo con
  `TEMP`/`TMP` apuntando al scratchpad propio lo arregla. (El hash resultó ser
  real —las cuatro unidades desplazan lo mismo— pero no se podía saber sin
  comprobarlo.)
- **`pctsnap.py` no acepta middleware**: `unidades()` sólo lista
  `src/Speed/Indep/SourceLists/*.cpp` y aborta. El `--cmp` sí vale para
  cualquier JSON con su formato.
- **Objetos bloqueados por otro proceso**: `build_direct.py` falló tres veces
  con `Could not open output file …/trctasks.o`, y `cp` daba
  `Device or resource busy`. El compilado en sí funcionaba (a otra ruta). Se
  resolvió solo al cabo de unos minutos.

## 8. Qué NO he probado

- **`splits.txt`** — prohibido por el encargo. Los siete rangos de §5 están
  calculados pero sin aplicar ni verificar con un enlace.
- **`gc_memcard_interface_impl`**: los tres literales que faltan, `ALL_ENTRIES`
  y los estáticos de `.sdata` de §6. Diagnosticados, no intentados.
- **`gc_driver`**: quitar los `operator new`/`delete` de clase. Es el mismo caso
  que la r22 dejó abierto para realcore/csis/rcmp y sigue sin desbloquear nada
  mientras la `.rodata` no esté atribuida.
- **`gc_blockcalculator`**: por qué emitimos `_vt.Q26Realmc15BlockCalculator`
  cuando el original lo emite en `gc_interface` (que lleva
  `#pragma implementation "gc_blockcalculator.h"`). Es un `#pragma interface`
  que no está haciendo su trabajo, pero la unidad está bloqueada por §5 igual.
- **`gc_interface`**: el FALLA preexistente de `audit.py`
  (`Clear__Q26Realmc9GCMessage` donde el objetivo llama a
  `Init__Q26Realmc9GCMessage`). La unidad está además bloqueada por `asd2`
  (r22 §6), así que la he dejado como estaba.
- **No he medido el proyecto entero**: sólo las 15 unidades de realmemcard, que
  son el cierre transitivo exacto de la cabecera tocada (comprobado, §3).
- **Un solo ensayo (c1) y ninguna veda**: el diagnóstico del DWARF señalaba una
  sola forma posible y salió a la primera. No hubo barrido de variantes.
