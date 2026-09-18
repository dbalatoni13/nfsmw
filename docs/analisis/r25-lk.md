# Ronda 25 — el frente de `linked`

Base medida al empezar (`report.json` de las 21:27, `main.dol: OK`):
**`linked` 10,2507 % — 417/590 unidades**, 404.496/3.946.048 B.
Universo de candidatas (código al 100 %, con objeto nuestro y en el enlace):
**70 unidades, 536.876 B** (`c23lk_cand.py`).

**Barrido de partida**: `c24lk_link.py` sobre las 70 con los objetos recién
construidos → **cero verdes**. Nada estaba promocionable por accidente.

## 1. Resultado: 12 unidades, +12 ficheros, 24.728 B

**`linked` 10,2507 % → 10,8773 % (+0,63 pp), 417 → 429 de 590.**

`trypromo.py`: **DOL OK las 12 juntas** y **cada una suelta** (salvo `public`,
que es racimo con `tasks`, §2.1), sobre objetos **borrados y reconstruidos**
antes de la pasada final. `audit.py`: **0 FALLA** en las 12 (dos pasadas).
`frozen.py cong` en las 12 + `systask`; `frozen.py chk` después: idénticas.

### Las líneas de `configure.py` — `NonMatching` → `Matching`

| línea | objeto | B | qué hizo falta |
|---|---|---|---|
| **1326** | `Packages/realmemcard/…/gc/public.cpp` | 5.024 | ✳L + **quitar el `asm()`** y definir `sCardName` en C (§2.1) |
| **2034** | `libc/vfprintf_1.c` | 5.972 | `VFPRINTF_SYM_*` por copia (§2.2) |
| **1334** | `Packages/realmemcard/…/gc/tasks.cpp` | 4.520 | ✳L (§2.1) |
| **1432** | `creates.cpp` | 3.320 | tabla + 3 cadenas a `extern` (el `__asm__` va en `realshape.hpp`) |
| **1102** | `…/realcore/…/file/cmn/hlafile.cpp` | 2.564 | 3 cadenas a `extern`, `request` inicializado, `mutex` a 8, y **cambiar el orden de dos sentencias** (§2.3) |
| **1158** | `…/realcore/…/input/gc/gc_interface.cpp` | 1.008 | **añadir** 724 B de `.bss` sin referencias, `gResetBit` a `extern`, reordenar dos funciones (§2.4) |
| **2096** | `libc/strtod2.c` | 812 | `powersOf10`/`maxExponent` a `extern` |
| **2038** | `libc/mbtowc_r.c` | 796 | las dos tablas JIS a `extern` |
| **1049** | `…/path/5.01.04/source/cmn/pathrand.cpp` | 264 | `seedPATH` a `extern` |
| **1206** | `…/realcore/…/system/debug/cmn/printstr.cpp` | 156 | `PRINTdevicelist` a `extern` |
| **959** | `…/snd/9/…/cmn/SNDI_sin.c` | 148 | los 7 coeficientes a `extern` |
| **957** | `…/snd/9/…/cmn/SNDI_root1x.c` | 144 | los 8 coeficientes a `extern` **y `t1 + 1.0f`, no `1.0f + t1`** |

`✳L` = el racimo de `gc_interface_impl.h`.
En **1102, 1158, 1206, 1326 y 1334** el `NonMatching,` está **solo en su línea**
(la llamada a `Object(` va partida). En **957** la llamada también va partida
pero `NonMatching,` comparte línea con la ruta. En **959, 1049, 1432, 2034, 2038
y 2096** va dentro del `Object(...)` de una sola línea.

**`public` y `tasks` hay que promocionarlas JUNTAS**: sueltas, `public` da
`L0039: findFileInfo.635_804D9648` (§2.1).

## 2. El mecanismo que ha abierto la ronda: `splits.txt` no asigna el dato, el
   comodín sí lo DEFINE, y se referencia por su nombre

Es el mecanismo de `memcard_utilities`/`sf_atan` de la r24, y **aplicado en
serie** vale mucho más de lo que parecía. La receta, en tres pasos:

1. **`c25lk_diag.py`** dice, por unidad, qué secciones sobran y **qué hay dentro
   de cada una**: cadena, `float` de fuente, vtable, o **constante de SESGO**
   (`0x4330000080000000` / `0x4330000000000000` / `2^31`), y si el símbolo está
   **MUERTO** (ninguna reubicación lo referencia).
2. **`c25lk_extern.py`** empareja cada objeto nuestro con el `.obj` del comodín
   —por nombre en el ELF original, aceptando el sufijo `.N` de los estáticos, o
   por bytes— y dice **si es `global`** (referenciable) o `local`.
3. Se declara `extern … __asm__("<nombre del troceador>")` y la sección
   desaparece.

**La regla que decide si una unidad es alcanzable**: si su pool contiene una
constante de SESGO, **no hay forma de fuente** y es `splits.txt`; si es todo
cadenas, literales de fuente y estáticos con nombre, **es alcanzable**. De las
70 candidatas, 12 cerraron con esta regla y **la mitad de las que quedan están
bloqueadas por el sesgo** (§4).

Dos detalles que cuestan una tarde:

- **Escalar → `@sda21`, array `[]` → `@ha/@l`** (ya estaba en la r24; me mordió
  en `bigswizzler`: `extern const float x;` bajó la unidad al 26,8 % y
  `extern const float x[];` la dejó en tamaño exacto).
- **Un literal se rematerializa; una lectura de memoria se hace CSE.** En
  `bigswizzler` el objetivo carga `lbl_80410120` **dos veces** (una por llamada
  a `GXInitTexObjLOD`) y con `extern const float[]` GCC lo sube a un registro
  salvado y lo carga **una**. Con `extern float[]` (sin `const`) vuelven las dos
  cargas. Ver la veda en §5.

### 2.1 `public` + `tasks`: la sexta falsificación, retirada

`public.cpp` llevaba un bloque `asm(".section .bss …")` (líneas 188-196) que
fabricaba **128 B**: `_6Realmc.sCardName` (0x60) y `findFileInfo.635_804D9648`
(0x20). **El ELF original dice que sólo los primeros 96 son de `public`**:

    gc_public.cpp:  .bss 804d95e8 sz=96  _6Realmc.sCardName
    gc_tasks.cpp:   .bss 804d9648 sz=32  findFileInfo.635
                    .sbss 804ffd5c sz=4  _.tmp_0.636

y `splits.txt` le da a `public.cpp` `.bss 0x804D95E8..0x804D9668` = **128**, o
sea **32 B de más** (el patrón inverso de `sstvol`, r24 §5.3). De ahí el `asm()`.

**Promocionando las dos juntas el apaño sobra**: `public.o` emite sus 96 y
`tasks.o` sus 32, y el segundo bloque cae exactamente en 0x804D9648. El `asm()`
está **reescrito en C** y medido:

    namespace Realmc {
    wchar_t sCardName[48] __asm__("_6Realmc.sCardName")
        __attribute__((section(".bss"), aligned(8)));

(`section(".bss")` porque sin él la variable se va a COMMON; `aligned(8)` porque
el bloque del objetivo lo está.) `public` queda con `.bss` de **96** y 100 % de
código. **Ya no queda ensamblador escrito a mano en `public.cpp`.**

Trampa medida y que casi me cuesta la tanda: **las dos sueltas daban DOL OK, y
juntas DOL ROTO** (`6afe85f94309`, el mismo hash que las 11 juntas). Era el
`findFileInfo` duplicado. Con el `asm()` retirado, las dos juntas dan DOL OK y
`public` sola ya no enlaza — **son racimo**.

Lo otro que necesitaban las dos: sus **28 B de `.rodata`** son las cinco cadenas
de formato de los `LC_msg` de `gc_interface_impl.h` (`"s"`, `"ss"`, `"sss"`,
`"ssss"`, `"ssdd"`), **MUERTAS** en las dos TUs (cero reubicaciones) y emitidas
igual: en el `.s` salen **entre un `.section .debug` y el `.previous`**, o sea
las escribe el generador de información de depuración del inline. El objetivo
tiene **seis copias** de esos 28 B (una por TU que incluye la cabecera).
`lbl_80414790` mide 0x38 = **dos copias**: la primera es de `public`, la segunda
de `tasks` (`trctasks` sí tiene su `.rodata` asignada, 0x804147C8..0x804147EC).

La cura es el patrón `LIBGCC2_CLZ_TAB_SYM`, en `gc_interface_impl.h`:

    #ifdef REALMC_LCFMT_BASE
    #define REALMC_LCFMT_S    (REALMC_LCFMT_BASE)
    #define REALMC_LCFMT_SS   (REALMC_LCFMT_BASE + 4)
    …
    #else
    #define REALMC_LCFMT_S    "s"
    …

con `#define REALMC_LCFMT_BASE lbl_80414790` en `public.cpp` y
`(lbl_80414790 + 28)` en `tasks.cpp`. Las otras cuatro TUs no definen la macro y
**no cambian**: `measure.py --cmp` sobre las 19 unidades de realmemcard da
**+0 B, 0 unidades cambian** (72.808/72.808 B, 100 %).

### 2.2 `vfprintf_1`: `vfprintf.c` se compila dos veces

`vfprintf.c` se compila tal cual y otra vez desde `vfprintf_1.c` con
`INTEGER_ONLY`. Cada copia tiene **su propio bloque** de datos y **ninguno está
asignado**: `.rodata` 0x8040FBB8 (144 B, `vfprintf`) y 0x8040FC48 (108 B,
`vfprintf_1`); `.bss` `pch.20`/`pch.3`; `.sdata`
`cumulative_written.21`/`.4` y `pch_pointer.22`/`.5`.

`vfprintf.c` lleva ahora `VFPRINTF_SYM_PCH`, `…_CUMULATIVE`, `…_PCHPOINTER`,
`…_BLANKS`, `…_ZEROES` y `VFPRINTF_STR_{LOWER,UPPER,NULL,BADBASE}`, todos con
rama `#else` que deja el fuente original; **`vfprintf_1.c` los define** y queda
sin `.bss`, sin `.rodata` y sin `.sdata`. `vfprintf` no cambia (mismo objeto).

**`vfprintf` (6.948 B) NO cierra**: su bloque acaba en
`0x4330000080000000` (0x8040FC40), la constante de sesgo. Quitando todo lo demás
quedarían 8 B. Es `splits.txt` (§4).

### 2.3 `hlafile`: dos errores de contenido que el 100 % escondía

`hlafile` estaba al 100 % con **dos cosas mal**:

- La segunda etiqueta de `Alloc` decía `"ASYNCFILE REQUESTS"` — **una cadena que
  no existe en el binario** (buscada byte a byte en el ELF original). El
  objetivo referencia `lbl_80413D34`, que es `"ASYNCFILE"`.
- El `__FILE__` es `"d:/packages/realcore/6.24.00/source/file/cmn/hlafile.cpp"`
  (`lbl_80413CF8`), no nuestra ruta `src/Speed/…`.

Las dos se arreglan con el mismo `extern` que vacía la `.rodata`. Lo demás:

- `static REQUESTSTRUCTtag *request;` → **`= 0`**: en el objetivo está en
  `.sdata` (0x804FF61C), no en `.sbss`. GCC 2.9 no tiene
  `-fzero-initialized-in-bss`: **cualquier inicializador manda la variable a
  `.data`/`.sdata`, aunque sea 0.**
- `mutex` con `__attribute__((aligned(8)))`: el objeto mide 28 B y **gas redondea
  el tamaño de la sección a la alineación** → `.bss` de 32, que es la del
  extraído.
- Y quedaban **4 bytes**: dos `stw` intercambiados. El objetivo hace
  `stw freequeue+4` / `stw request` / `stw freequeue`; nosotros dejábamos
  `request` el último. **La forma que lo cierra es escribir `head` ANTES que
  `tail`** —el planificador saca el `stw` de la cola delante igual—:

      freequeue.head = request;
      freequeue.tail = &request[requests - 1];

  Ensayos: c1 `&request[requests-1]` (base) y c2 `request + (requests - 1)` dan
  los mismos 4 B mal; c3 (head primero) da 0.

### 2.4 `gc_interface` de `realcore/input`: hacía falta AÑADIR datos

Es el caso inverso: al objeto le **faltaban** 724 B. El ELF original dice que
`gc_interface.cpp` tiene en `.bss` 0x804DA668:

    804da668 sz=48  _9RealInput.gPadstat
    804da698 sz=12  FatalParam        (local, SIN referencias)
    804da6a8 sz=712 FatalContext      (local, SIN referencias)

Con dos `static char[]` el enlazador se los come (`-strip-unused-data`: la `.bss`
total bajaba 704 B). **Declarándolos globales** (`char FatalParam[12]
__asm__("FatalParam");` y `FatalContext[712]` con `aligned(8)`) sobreviven y la
`.bss` mide 776/al8, la del extraído.

Además hicieron falta dos cosas más:

- **`gResetBit` fuera**: `splits.txt` le da a la unidad `.sdata`
  0x804FF6C0..0x804FF6C8, que es `gPadUpdate` **más 4 B de relleno**;
  `gResetBit` está en 0x804FF6C8, en el comodín `auto_08_804FF6C8_sdata`.
  Definiéndolo aquí, las **cinco** referencias `@sda21` de `gc_pad.cpp` salían
  4 B por debajo. `extern` y listo. (`gPadUpdate` con `aligned(8)` para que la
  sección quede 8/al8.)
- **Reordenar**: el objetivo va `~GcInterface` (344) y luego
  `GetUnusedEffectSlot` (536); el fuente los tenía al revés.

Sin `doldiff` no se ve ninguna de las dos: `measure.py` daba 100 % en las tres
formas intermedias.

## 3. `#pragma interface`: la extrapolación NO se sostiene, y por qué

El encargo la señalaba como lo más prometedor. **Medido, no vale en ninguna de
las cuatro candidatas** —y el motivo es el mismo en todas: *las vtables que
emitimos SÍ son de la unidad*, sólo que `splits.txt` no le asigna `.rodata`.

La prueba es el **orden de enlace**: el bloque de vtables cae exactamente donde
le toca a la unidad en la `.rodata` del objetivo. Comprobado con
`c25lk_owner.py` (dueño único) en los cuatro casos.

| unidad | B | qué pasó |
|---|---|---|
| `pathsnd` (8.452) | `.rodata` 1.128 | el pool trae **7 constantes de sesgo**: el pragma quitaría las 5 vtables y quedarían 112 B. Y el bloque 0x80413300..0x80413770 **es suyo** (dueño único: sólo `pathsnd.s`) |
| `csis` (5.728) | `.rodata` 168 | `#pragma interface` en el `.cpp` (las clases están ahí, patrón `rcmpbase`) quita las 3 vtables **y 504 B de `.text`**: las 8 virtuales en clase, que el objetivo emite en la **zona de inline pendientes** (entre `__static_initialization_and_destruction_0` y `_GLOBAL_.I.`). Definirlas fuera de línea NO las pone ahí — **el mismo muro que `rcmpbase` y `gc_device`** |
| `interfaceimp` de realmemcard (896) | `.rodata` 504 | `#pragma interface` en `memcard_interface_impl.h` la deja **idéntica en `.rodata`** y a 52 B de `.text` (le quita `_._Q26Realmc13BaseInterface`), pero **cuesta −1.132 B y −16 funciones** en `gc_interface` (−972) y `gc_memcard_interface_impl` (−108). Revertido. Y aunque cerrara el `.text`, sus 504 B de `.rodata` (0x804147F0, dueño único) son `splits.txt` |
| `gc_driver`, `rcmp_vp6_codec`, `rcmp_mad_codec` | | bloqueadas antes del pragma: `.bss`/pool con sesgo (§4) |

**Lo único que el pragma sí destapó**, y que dejo anotado como corrección
pendiente (no aplicada, porque no promociona nada por sí sola): **`csis` emite
una vtable de más**, `_vt.Q32EA9Allocator10IAllocator` (56 B) más su destructor
inline `_._Q32EA9Allocator10IAllocator` (52 B). El objetivo **no la tiene**: la
referencia en 0x803D18F0, donde la ponen `zBWare` y `rcmp_vp6_codec`. Su
`.rodata` real son 112 B (0x804130A0..0x80413110): `"CsisAlloc"` + las **dos**
vtables `Csis::*Adaptor`.

## 4. Lo que sólo se arregla en `splits.txt` (VEDADO), con dueño único comprobado

Comprobación: `c25lk_owner.py <ini> <fin>` lista los `.obj` del comodín en el
rango y **quién los referencia** en los 590 `.s` de `build/GOWE69/asm`.

| unidad | B código | rango que falta | dueño único |
|---|---|---|---|
| `pathsnd` | 8.452 | `.rodata 0x80413300..0x80413770` (1.128 B: pool 112 + 5 vtables) | **sí**, sólo `pathsnd.s` |
| `vfprintf` | 6.948 | `.rodata 0x8040FBB8..0x8040FC48` (144) + `pch.20` + `.sdata` | **sí**, sólo `vfprintf.s` |
| `csis` | 5.728 | `.rodata 0x804130A0..0x80413110` (112), `.data 0x80451E88..0x80451EA4` (28), `.bss 0x804CC6D0..0x804CC6EC` (28) | **sí**, sólo `csis.s` en los tres rangos |
| `avplayer` | 4.224 | `.rodata 0x8040FEF8..0x8040FF40` (72) | **sí**, sólo `avplayer.s` |
| `pathvol` | 4.124 | `.rodata 0x804138A8..0x80413958` (176) | **sí**, sólo `pathvol.s` |
| `interfaceimp` (realmemcard) | 896 | `.rodata 0x804147F0..0x804149E8` (504, las 2 vtables) | **sí**, sólo `interfaceimp.s` |

Y los dos **casos inversos**, donde `splits.txt` asigna de MÁS:

| unidad que sobra-asigna | de más | de quién es | a quién bloquea |
|---|---|---|---|
| `locale.cpp` (realmemcard) `.bss 0x804D6540..0x804D8E24` | 228 B | `statInfo.695` (108, **de `gc_driver.cpp`**) y `sMsg.608` (120) | **`gc_driver`** (8.952 B) y **`systask`** (508 B) |
| `public.cpp` `.bss` | 32 B | `findFileInfo.635`, de `tasks.cpp` | ya resuelto sin tocar `splits.txt` (§2.1) |

**`systask` merece explicación**: le sobran 8 B de `.bss` (328 contra 320) que
son **el relleno de alineación de la `.bss` de `locale.o`**, que en el original
mide 10.240 B (múltiplo de 8) y en el nuestro 10.468 por las dos fabricaciones.
Poniendo `aligned(8)` en `gTrcMsgBuffer`, gas redondea 10.468 a **10.472** y
rompe el enlace por el otro lado. Es decir: **`systask` cierra el día que
`locale` deje de fabricar los 228 B de otros**. Lo que sí he dejado hecho es
sacar sus dos estáticos de `.sbss` (§6).

La familia del sesgo `int→double` (r24 §5.1) sigue igual y **se ha ampliado**
con casos nuevos, todos con el sesgo dentro del pool y por tanto sin forma de
fuente: `snddrv` (11.920 B, `.rodata` 136), `gc_driver` (8.952),
`rcmp_vp6_codec` (2.828), `rcmp_mad_codec` (2.592), `postproc` (3.268),
`bigyuvswizzler` (2.212), `itoa` (1.548), `pathinit`, `stagpat`, `svol`,
`sstfxlev`, `libgcc2_4`, y las de la libm de SN.

## 5. Vedas nuevas, con la sentencia barrida

- **`bigswizzler`** (1.208 B): la cadena `"VD::tBigSwizzler"` **sí** se puede
  externalizar (`lbl_8041010C`, la unidad se queda al 100 %); el `0.0f` de las
  dos llamadas a `GXInitTexObjLOD` **no**. Con literal GCC encadena
  `lfs f1 / fmr f2,f1 / fmr f3,f2`; con `extern const float[]` sube la carga a un
  registro salvado (una sola `lfs` en toda la función) y con `extern float[]`
  vuelven las dos cargas pero el encadenado sale `fmr f3,f1`. **17 B de
  diferencia en 2 rangos** (`c23lk_doldiff.py`). Ensayos c1 escalar (26,8 %),
  c2 `const float[]`, c3 local `const float zeroLod`, c4/c5 `float[]` sin
  `const`: los cuatro dejan `.text` en 1.208 B exactos y el mismo diff.
- **`quantize`** (896 B): caso inverso, le faltan **40 B de `.rodata`** que son
  **pool del compilador** (sesgo + 1.0 + 65536.0 + 0.5 + sesgo2) de código que
  hoy no generamos, más `VP6_BuildQuantIndex` (4 B de `.sbss`). Sin forma de
  fuente.
- **`eathread_thread`** (84 B): `gThreadDynamicData` (26.624 B) y sus dos
  hermanos **no existen en el ELF original** y en nuestro objeto están MUERTOS;
  además emitimos un `AllocateThreadDynamicData` de 4 B que el objetivo no
  tiene. No lo he tocado: 84 B y toca borrar declaraciones que el fuente usa.
- **`memcard_interface_impl.h` con `#pragma interface`**: −1.132 B (§3).
  Revertido y medido a cero (`measure.py --cmp` sobre las 19 unidades: +0 B).

## 6. Correcciones que NO cierran (se quedan: +0 B, nada baja)

- **`systask`** (508 B): `reentry` y `lastsystemtask` sacados de `.sbss` a
  `extern … __asm__("reentry.39_804FF638")` / `("lastsystemtask.46_804FF63C")`,
  que es donde los pone el objetivo (`.sdata`, en el rango de `timerthread.cpp`,
  que ya los define). `.sbss` de 8 a 0 B, unidad al 100 %. Bloqueada por §4.
- **`realshape.hpp`**: el `__asm__("_9RealShape.gTexelTypeToBpp_80414010")` ha
  pasado de `cluttype.cpp` a la cabecera, que es lo coherente: **las 12
  referencias del objetivo usan ese nombre**. `measure.py --cmp` sobre las 4
  unidades que la incluyen: **+0 B, 0 unidades cambian**.

## 7. Lo que NO he probado (por orden de lo que yo atacaría)

1. **La `.gnu.linkonce.r` como salida al muro de las vtables.** El `ldscript`
   ya recoge `*(.gnu.linkonce.r*)`, y si GCC emitiera las vtables ahí el
   enlazador **descartaría el duplicado con su espacio** y `csis`,
   `interfaceimp`, `pathreal` y compañía dejarían de necesitar `splits.txt`.
   Es un cambio de cflags (`configure.py`) y **no lo he medido**: sería lo
   primero que probaría, con el A/B de la biblioteca entera.
2. **`interface` de `realcore/input`** (204 B): 3 de sus 4 vtables están
   MUERTAS y sobran 200 B de `.text`. Aun limpiándolo queda
   `_vt.Q29RealInput9Interface` (80 B), que es suya → `splits.txt`. No lo he
   intentado.
3. **`gc_blockcalculator`** (312 B): `.rodata` 96, `.text` +72. Sin diagnosticar
   a fondo.
4. **`sserver`** (992 B) y **`bigswizzler`**: vedas de r24 y de esta ronda.
5. **`pathreal`** (496) y **`pathreal6`** (916): 240 y 144 B de vtables, mismo
   patrón que `interfaceimp`.
6. **`snddrv`** (11.920 B) y **`gc_memcard_interface_impl`** (10.984 B), las dos
   más grandes que quedan: las dos con sesgo y con `.sdata`/`.bss` propias sin
   asignar. No he mirado si les sobra además algo de fuente.
7. **La segunda evidencia de `-fno-implement-inlines`**: no he encontrado una
   tercera. `csis` y `rcmpbase` apuntan al mismo muro (inline pendiente que hay
   que emitir DESPUÉS de `__static_initialization_and_destruction_0`) pero eso
   es lo contrario de lo que hace el flag, así que **no cuenta como tercera**.

## 8. Herramientas (scratchpad, prefijo `c25lk_`)

| script | qué hace |
|---|---|
| `c25lk_elf.py` | lector ELF32 BE mínimo: secciones, símbolos y reubicaciones. Lo usan todos los demás |
| **`c25lk_diag.py`** | **la que ha decidido la ronda**: por unidad, diff de secciones + cada objeto de las secciones que sobran clasificado (cadena / float / vtable / **SESGO**) y **MUERTO/usado**, más lo que le da `splits.txt` |
| **`c25lk_extern.py`** | para cada objeto nuestro, **cómo se llama en el comodín** y si es `global`. Es la que escribe media declaración sola |
| **`c25lk_owner.py`** | dueño único: `.obj` del comodín en un rango y quién los referencia en los 590 `.s` |
| `c25lk_files.py` | **agrupa los símbolos LOCALES del ELF original por su `STT_FILE`** — hay **577** símbolos de fichero en `NFSMWRELEASE.ELF`. Es la fuente de verdad para «este dato de quién es» |
| `c25lk_secs.py` | tamaño + alineación de cada sección, extraído contra nuestro, para las unidades que le pases |
| `c25lk_pool.py` | el pool de una unidad con el valor interpretado y **dónde está ese bloque en el ELF original** |
| `c25lk_scan.py` / `c25lk_cand.txt` | pasa `c24lk_link.py` por las 70 candidatas |
| `c25lk_rodaref.py` | qué objetos de datos están MUERTOS en un objeto nuestro |

**El hallazgo de herramienta de la ronda**: `orig/GOWE69/NFSMWRELEASE.ELF` trae
**577 símbolos `STT_FILE`** y, detrás de cada uno, sus símbolos locales. Eso da
la atribución **real** de cada estático a su `.cpp`, que es exactamente lo que
`splits.txt` intenta adivinar. Los dos casos inversos de §4 salieron de ahí en
un minuto.

## 9. Ficheros de fuente tocados

`gc_interface_impl.h`, `public.cpp`, `tasks.cpp` (realmemcard);
`vfprintf.c`, `vfprintf_1.c`, `strtod2.c`, `mbtowc_r.c` (libc);
`creates.cpp` y `realshape.hpp`; `pathrand.cpp` (path);
`hlafile.cpp`, `printstr.cpp`, `input/gc/gc_interface.cpp`, `system/cmn/systask.cpp`
(realcore); `SNDI_sin.c`, `SNDI_root1x.c` (snd).

**Cabeceras compartidas y su A/B**: `gc_interface_impl.h` la incluyen 6 unidades
(las 19 de realmemcard miden 72.808/72.808 B, 100 %, antes y después);
`realshape.hpp` la incluyen 4 (8.428/8.808 B antes y después, `--cmp` +0 B);
`vfprintf.c` lo incluye `vfprintf_1.c` y `vfprintf` no cambia.
Las 13 unidades de `realcore/…/input/` siguen todas al 100 %.
