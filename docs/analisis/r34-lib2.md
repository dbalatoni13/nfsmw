# Ronda 34 — bibliotecas (lib2): rangos de `splits.txt` y promociones

Base verificada al empezar **y al acabar**: `main.dol` **OK** con los 607 objetos
del enlace, `keepchk` **22 rancias** (no 21: la 22ª ya estaba antes de tocar
nada, es de `zEAXSound2`). `config/` y `configure.py` quedan **byte a byte como
me los encontré** (`diff -r` limpio, comprobado en la última orden).

---

## LO PRIMERO: SEIS UNIDADES DAN `DOL OK` — 12.768 B

| unidad | B | qué le faltaba | `trypromo` |
|---|---|---|---|
| **`libc/vfprintf`** | **6.948** | dos rangos (`.sdata` + `.bss`) | **DOL OK** |
| **`libc/e_pow`** | **2.000** | **corte de fuente** (ver §2) | **DOL OK** |
| **`libc/ef_pow`** | **1.780** | corte de fuente + dos rangos | **DOL OK** |
| **`libc/e_rem_pio2`** | **868** | reordenar + cortar la fuente + un rango | **DOL OK** |
| **`libc/sf_log`** | **640** | **un rango de 8 B** | **DOL OK** |
| **`libc/kf_tan`** | **532** | **un rango de 8 B** | **DOL OK** |

**Las seis juntas: `DOL OK`.** Y con el paquete completo aplicado (13 rangos +
los seis `Matching` en `configure.py`) el **enlace base de los 610 objetos sale
`DOL OK`** y `keepchk` sigue en **22**. Verificado tres veces, la última con
`configure.py` editado de verdad.

Ninguna de las seis salía en el barrido inicial: las seis daban `DOL ROTO`.

---

## 1. El barrido de las 40 (el encargo decía 43; hoy son 40)

`promodist.py --libs --listas | tr -d '\r'` da **40** unidades sin promocionar
(las 43 de la r33 menos `math_support`, `k_rem_pio2` y `gc_blockcalculator`, que
ya entraron). Pasado `trypromo.py` a las 40 **antes de tocar nada**:

**Cero promociones gratis.** El regalo de la r33 no se repite: el árbol ha
cambiado mucho, pero ninguna unidad de biblioteca ha quedado promocionable por
efecto colateral. 33 dan `DOL ROTO`, 5 `ENLACE FALLA` y 2 no tienen objeto
nuestro.

| unidad | veredicto (árbol de partida) |
|---|---|
| `criticalpath`, `filesys`, `steering`, `spchpick`, `vfprintf`, `avplayer`, `pathnode`, `rcmp_vp6_codec`, `dvd_device`, `rcmp_mad_codec`, `pathtrack`, `stagpat`, `bigyuvswizzler`, `kf_rem_pio2`, `pathbank`, `sdspmix`, `e_pow`, `ef_pow`, `ssysinit`, `madidct`, `bigswizzler`, `rcmpbase`, `sserver`, `sfir`, `e_rem_pio2`, `ef_rem_pio2`, `sf_log`, `kf_tan`, `inittmr`, `input/cmn/interface`, `spchsamp`, `libgcc2_4`, `eathread_thread` | **DOL ROTO** |
| `gc_interface`, `DebuggerDriver`, `LibSN/vmbase`, `LibSN/vm`, `ppc2D2` | **ENLACE FALLA** |
| `asd2`, `auto_00_8000348C_init` | falta nuestro `.o` |

**El `L0019: multiply defined` es un AVISO** y sale en casi todas (`PPCMtdec`,
los inline de `gc_blockcalculator.h`): quien trunque el mensaje de error como
hace `trypromo` verá siempre ese aviso y no el `L0039`, que es el error.
`undlist.py` da la lista buena.

---

## 2. El hallazgo de la ronda: **el pool va DETRÁS de la función**

Cinco de las seis promociones salen del mismo mecanismo, y no estaba descrito.

En los `.c` de fdlibm, `.sdata` lleva **dos cosas mezcladas**: los `static const`
con nombre (que GCC 2.9 emite **en orden de declaración, en el sitio de la
declaración**) y **la constante de sesgo `int→double`**
(`0x4330000080000000`), que el compilador genera y emite con
`output_constant_pool` **al terminar la función**.

Nosotros declarábamos las 33 constantes seguidas y luego la función, así que el
sesgo salía **el último**. El objetivo lo tiene **en medio**:

```
e_pow  .sdata:  one .. cp (20)  |  SESGO  |  mone .. two (13)
```

Eso sólo puede pasar si en la fuente original **13 de las constantes se declaran
DESPUÉS de la función**. Y eso se reproduce sin tocar una línea de código:

```c
extern const double mone;          /* antes de la funcion: solo la DECLARACION */
...
double pow(double x, double y) { ... }
/* despues: el ALMACENAMIENTO */
static double mone__store __asm__("mone") = -1.0;
```

que es exactamente `SDA_DOUBLE` de `fdlibm.h` **partido en dos**. El uso pasa por
la misma declaración `extern const`, así que **el `.text` no se mueve**: las tres
unidades siguen al **100,000 %** en `libdiff.py` y `audit.py` no da un solo
`FALLA`.

| unidad | `.sdata` antes | después | `.text` |
|---|---|---|---|
| `libc/e_pow` | 174/272 (64,0 %) | **272/272 (100 %)** | 2.000 B, 100 % |
| `libc/ef_pow` | 76/136 | **140/140 (100 %)** | 1.780 B, 100 % |
| `libc/e_rem_pio2` | 16/88 | **88/88 (100 %)** | 868 B, 100 % |

`e_rem_pio2` necesitó además **reordenar** las siete que quedan delante: el
objetivo las declara `zero, pio2_1, pio2_1t, pio2_2, pio2_2t, invpio2, half` y
nosotros `zero, half, two24, invpio2, …`.

**Los tres ficheros quedan aplicados en `src/`** (`e_pow.c`, `ef_pow.c`,
`e_rem_pio2.c`): son correcciones demostrables, no apuestas.

**Trampa que costó una compilación**: `src/libc/e_pow.c` tiene **finales de línea
MIXTOS** (CRLF general, **LF** en las tres líneas del comentario de `huge`). Un
script que parta por `'\r\n'` funde cuatro líneas en una y deja el
`static double huge` sin borrar → `Label huge multiply defined`. Hay que recorrer
con `splitlines(keepends=True)`.

---

## 3. El segundo hallazgo: **`claimrange.py` no ve los estáticos con sufijo**

`claimrange.py` busca en `symbols.txt` **el nombre exacto** del símbolo que
emitimos. Pero el troceador **desambigua los estáticos locales homónimos
añadiéndoles `.N`**, así que un `pch` nuestro se llama `pch.20` allí y
`claimrange` lo tira al cajón de **«sin nombre en symbols.txt»** — que la r31
dio por «hay que localizarlo por contenido».

**No hay que localizar nada: está en `symbols.txt` con sufijo.** Un `grep` por
`^<nombre>[.$][0-9]*` lo saca:

```
cumulative_written.21 = .sdata:0x804FEDD0;   pch.20 = .bss:0x804B5450; // size:0x80
pch_pointer.22        = .sdata:0x804FEDD4;   pch.3  = .bss:0x804B54D0;  (vfprintf_1)
```

Eso son **los dos rangos de `libc/vfprintf`**, la unidad de biblioteca más
grande de la cola (**6.948 B**, `.text` al 100 % y `.rodata` ya 144/144). Con los
dos: `DOL OK`. Con sólo el `.sdata`: `DOL ROTO`. **Hacen falta los dos.**

Este eje sigue abierto para otras unidades: `stagpat` tiene su `freekey` en
`freekey.126 = .data:0x80450540` (**pero en `.data`, y el nuestro está en
`.bss`**: eso es fuente, no rango); `nextARAMPageToCheck` (LibSN/vm) y los
`gThreadDynamicData*` (eathread) **no están** en `symbols.txt` ni con sufijo.

---

## 4. El tercer hallazgo: el rango de 8 B del sesgo

`sf_log` y `kf_tan` tenían `sdata+8` y **0 funciones pendientes**: los ocho bytes
son **el sesgo `int→double`** que el compilador genera. Sus constantes con
nombre ya se referencian como `SDA_FLOAT_X(x, lbl_…)`, así que el único dato que
emiten es ese. El objetivo también lo emite, y está **anotado como `double` con
`align:8` en `symbols.txt`**, en medio del bloque de `lbl_` de la unidad:

```
lbl_804FF120 = .sdata:0x804FF120; // size:0x8 align:8 data:double   -> sf_log
lbl_804FF348 = .sdata:0x804FF348; // size:0x8 align:8 data:double   -> kf_tan
```

Un rango de **ocho bytes** promociona una unidad de 640 y otra de 532.
**Receta**: en el bloque de `lbl_` que la unidad referencia por
`SDA_*_X`, el único `size:0x8 align:8 data:double` es su sesgo; ése es el rango.

Y un dato para el catálogo de efectos colaterales: **claimar
`ef_rem_pio2 .sdata 0x804FF350..0x804FF388` también hace que `kf_tan` promocione**
(`DOL OK`), aunque `ef_rem_pio2` siga sin poder. Al partir el comodín en
`0x804FF350`, el trozo `0x804FF330..0x804FF350` deja de tener dato vivo y
`-strip-unused-data` se lleva la copia muerta del sesgo. Los dos caminos están
medidos; el paquete lleva **los dos rangos**, que son correctos por separado.

---

## 5. `gc_interface`: el ciclo de dependencia, resuelto

El diagnóstico del encargo **reproduce exactamente** contra el árbol de hoy:

```
Cyclic dependency: gc_interface.cpp -> asd2 -> public.cpp -> tasks.cpp
                   -> trctasks.cpp -> interfaceimp.cpp
```

**El rango que sobra es `0x804149E8..0x80414A18`, y sobra ÉL SOLO**: puesto en
`gc_interface` sin el otro, el ciclo sale igual (ensayo B). Y no es un rango
malo: es **de otra unidad**.

`symbols.txt` lo dice al byte: `_vt.Q26Realmc18BlockCalculatorImp =
.rodata:0x804149E8; size:0x30`. En `.text` el orden es
`… trctasks(0x80393AB8) → interfaceimp(0x80398988) → **gc_blockcalculator**
(0x80398D08)`, y en `.rodata` `interfaceimp` acaba justo en `0x804149E8`.
**El dueño de esos 48 B es `gc_blockcalculator.cpp`**, que va detrás de
`interfaceimp` en las dos secciones. Reasignado así:

| ensayo | rangos | split | keepchk | enlace base |
|---|---|---|---|---|
| A | `gc_interface .rodata 0x80414530..0x80414790` | ok | 22 | **DOL OK** |
| B | `gc_interface .rodata 0x804149E8..0x80414A18` | **CICLO** | — | — |
| C | los dos en `gc_interface` | **CICLO** | — | — |
| D | `gc_blockcalculator .rodata 0x804149E8..0x80414A18` | ok | 22 | **DOL OK** |
| E | A + D | ok | 22 | **DOL OK** |

**Regla que confirma la técnica del encargo**: los rangos de datos fijan el orden
de enlace, así que **un ciclo señala el rango cuyo dueño contradice el orden del
`.text`**. Cotejar el extremo del rango contra el orden de `.text` en
`splits.txt` lo decide en un minuto, sin gastar un ensayo.

### El bloqueo real de `gc_interface`: `asd2` es SU PROPIO bloque

`trypromo` no dice `DOL ROTO` sino `ENLACE FALLA`, y el error (con `undlist`, no
con el mensaje truncado) es uno solo:

```
asd2(1) : error: L0039: Reference to undefined symbol
          __static_initialization_and_destruction_0_803906C0
```

**`asd2` no es una unidad: son los `_GLOBAL_.I/.D` de `gc_interface.cpp`.**
`symbols.txt` lo demuestra:

```
_GLOBAL_.I._6Realmc.ROOT_DIRECTORY_NAME = .text:0x80391518; size:0x2C scope:local
_GLOBAL_.D._6Realmc.ROOT_DIRECTORY_NAME = .text:0x80391544; size:0x2C scope:local
gcc2_compiled.                          = .text:0x80391570;   <- empieza el TU siguiente
```

Los dos son **locales** y el `gcc2_compiled.` que marca frontera de TU está en
`0x80391570`, no en `0x80391518`. `configure.py` ya lo sabía a medias: tiene
`OBJDIFF_SHARED_SOURCES` mapeando `main/asd2` a `gc_interface` **sólo para
objdiff**, y el enlazador no lee ese mapeo.

**Fundiendo `asd2` dentro de `gc_interface.cpp` en `splits.txt`, el enlace pasa:
0 errores.** Medido:

| | split | keepchk | enlace base | `gc_interface` |
|---|---|---|---|---|
| árbol de hoy | ok | 22 | DOL OK | **ENLACE FALLA** (`L0039`) |
| con `asd2` fundido | ok | 22 | **DOL OK** (607 objs) | **DOL ROTO** — *enlaza* |
| fundido + los 13 rangos | ok | 22 | **DOL OK** (608 objs) | **DOL ROTO** — *enlaza* |

El cambio es:

```
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp:
	.text       start:0x8038FD10 end:0x80391570      <- era 0x80391518
	.ctors      start:0x803C8C18 end:0x803C8C1C      <- de asd2
	.dtors      start:0x803C8C58 end:0x803C8C5C      <- de asd2
	.rodata     start:0x80414530 end:0x80414790
	...

asd2:                                                <- BLOQUE ENTERO BORRADO
```

y en `configure.py` habría que mover el par
`__static_initialization_and_destruction_0_803906C0` de
`OBJDIFF_SHARED_SOURCE_MAPPINGS["main/asd2"]` a
`OBJDIFF_SYMBOL_MAPPINGS[".../gc_interface"]` (ya está ahí), añadir
`_GLOBAL_.I/.D._6Realmc.ROOT_DIRECTORY_NAME` →
`_GLOBAL_.I/.D._Q26Realmc11GCInterface.mpDriver` y **borrar
`OBJDIFF_SHARED_SOURCES`**, que se queda sin destino.

**No lo he metido en el paquete** porque `gc_interface` sigue sin promocionar y
la fusión cambia cómo objdiff atribuye 88 B mientras otros agentes miden. Va
como propuesta aparte, verificada.

### Lo que le queda a `gc_interface` (con `asd2` fundido)

`promote.py` deja de quejarse de casi todo. Sólo quedan **112 B de `.text` y
104 B de `.rodata` de más**, y son lo mismo tres veces: cuerpos `inline`
definidos **dentro** de clases de cabecera, que GCC 2.9 emite en cada TU que usa
la clase.

```
.text  +112 B:  _._Q26Realmc18BlockCalculatorImp (52)
                GetResult__Q26Realmc18BlockCalculatorImp (8)
                _._Q26Realmc9GCMessage (52)
.rodata +104 B: $LC3..$LC7 (32, cadenas que el objetivo tiene en trctasks)
                _vt.Q26Realmc9GCMessage (24, es de gc_memcard_interface_impl)
                _vt.Q26Realmc18BlockCalculatorImp (48, es de gc_blockcalculator)
```

Y hay además un **orden de vtables distinto** (no un tamaño): el objetivo emite
`GCInterface, TaskTrc*(6), TaskTrc, GcTask, TaskManager, BlockCalculator` y
nosotros metemos `BlockCalculator`+`TaskManager` **antes** del racimo `TaskTrc*`.
GCC 2.9 emite las vtables **al revés del orden de parseo de la definición de la
clase**, así que eso es **orden de `#include` en `gc_interface.cpp`**, no
codegen. Con los tres cuerpos fuera y ese orden, la unidad debería cerrar: el
resto de `.text` ya casa símbolo a símbolo.

Las cinco funciones «de más» que decía el encargo se quedan en **tres**: los
`_GLOBAL_.I/.D` dejan de sobrar en cuanto `asd2` se funde (sólo cambia su
nombre local, `_6Realmc.ROOT_DIRECTORY_NAME` contra
`_Q26Realmc11GCInterface.mpDriver`, que **al enlazador le da igual**: son
locales; es sólo el orden de declaración de los estáticos con constructor).

---

## 6. Los ensayos, uno por uno

Ciclo por ensayo (~20 s): restaurar `splits.txt` desde copia → añadir **un**
rango → `dtk dol split` → `configure.py` → `keepchk` → **enlace base** →
`trypromo`. Herramientas en el scratchpad (`c34lib2_claim.py`, `c34lib2_base.py`,
`c34lib2_sweep.py`, `c34lib2_locate.py`, `c34lib2_vsdol.py`, `c34lib2_cut.py`,
`c34lib2_reorder.py`, `c34lib2_merge.py`).

| # | unidad | sección | rango | split | keepchk | enlace base | `trypromo` |
|---|---|---|---|---|---|---|---|
| A | `gc_interface` | `.rodata` | `0x80414530..0x80414790` | ok | 22 | DOL OK | — |
| B | `gc_interface` | `.rodata` | `0x804149E8..0x80414A18` | **CICLO** | — | — | — |
| C | `gc_interface` | `.rodata` | A + B | **CICLO** | — | — | — |
| D | `gc_blockcalculator` | `.rodata` | `0x804149E8..0x80414A18` | ok | 22 | DOL OK | — |
| E | A + D | | | ok | 22 | DOL OK | `gc_interface` ENLACE FALLA |
| F | fusión `asd2`→`gc_interface` | | | ok | 22 | DOL OK | `gc_interface` **enlaza** (DOL ROTO) |
| G | `ef_pow` | `.sdata2` | `0x80500390..0x805003A8` | ok | 22 | DOL OK | DOL ROTO |
| H | `rcmpbase` | `.rodata` | `0x80410158..0x80410188` | ok | 22 | DOL OK | DOL ROTO |
| I | `e_rem_pio2` | `.sdata2` | `0x80500A48..0x80500BD0` | ok | 22 | DOL OK | DOL ROTO |
| K | `input/cmn/interface` | `.rodata` | `0x80414BF0..0x80414C40` | ok | 22 | DOL OK | DOL ROTO |
| L | `input/cmn/interface` | `.rodata` | `0x80414DD0..0x80414E80` | **CICLO** | — | — | — |
| M | `ef_pow` | `.sdata` | `0x804FF180..0x804FF210` | ok | 22 | DOL OK | DOL ROTO |
| N | `ef_pow` | M + G | | ok | 22 | DOL OK | **DOL OK** |
| O | `e_rem_pio2` | `.sdata` | `0x804FF7E0..0x804FF838` | ok | 22 | DOL OK | **DOL OK** |
| P | `e_rem_pio2` | O + I | | ok | 22 | DOL OK | **DOL OK** |
| Q | `ef_rem_pio2` | `.sdata` | `0x804FF350..0x804FF388` | ok | 22 | DOL OK | DOL ROTO (**y `kf_tan` DOL OK**) |
| S | `kf_tan` | `.sdata` | `0x804FF348..0x804FF350` | ok | 22 | DOL OK | **DOL OK** |
| T | `sf_log` | `.sdata` | `0x804FF120..0x804FF128` | ok | 22 | DOL OK | **DOL OK** |
| U | `vfprintf` | `.sdata` | `0x804FEDD0..0x804FEDD8` | ok | 22 | DOL OK | DOL ROTO |
| V | `vfprintf` | U + `.bss 0x804B5450..0x804B54D0` | | ok | 22 | DOL OK | **DOL OK** |
| W | `libgcc2_4` | `.rodata` | `0x8040FE28..0x8040FE40` | **acaba dentro de símbolo** | — | — | — |
| R | **paquete de 13** | | | ok | **22** | **DOL OK (610)** | **las seis DOL OK, juntas también** |

`libc/e_pow` no aparece: **no necesita ningún rango nuevo** (la r33 ya le puso
los dos); lo único que le faltaba era el corte de fuente.

---

## 7. El paquete exacto, ya medido

### `config/GOWE69/splits.txt` — 13 líneas nuevas

Las marcadas **[P]** son las que hacen falta para las promociones; el resto son
rangos correctos y **medidos como inocuos** (enlace base `DOL OK`, `keepchk` sin
moverse).

```
libc/vfprintf.c:
	.bss        start:0x804B5450 end:0x804B54D0            [P]
	.sdata      start:0x804FEDD0 end:0x804FEDD8            [P]
libc/sf_log.c:
	.sdata      start:0x804FF120 end:0x804FF128            [P]
libc/ef_pow.c:
	.sdata      start:0x804FF180 end:0x804FF210            [P]
	.sdata2     start:0x80500390 end:0x805003A8            [P]
libc/kf_tan.c:
	.sdata      start:0x804FF348 end:0x804FF350            [P]
libc/ef_rem_pio2.c:
	.sdata      start:0x804FF350 end:0x804FF388
libc/e_rem_pio2.c:
	.sdata      start:0x804FF7E0 end:0x804FF838            [P]
	.sdata2     start:0x80500A48 end:0x80500BD0
egami/rcmp/dev/source/decoder/cmn/rcmpbase.cpp:
	.rodata     start:0x80410158 end:0x80410188
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp:
	.rodata     start:0x80414530 end:0x80414790
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_blockcalculator.cpp:
	.rodata     start:0x804149E8 end:0x80414A18
Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface.cpp:
	.rodata     start:0x80414BF0 end:0x80414C40
```

### `configure.py` — seis `NonMatching` → `Matching`

```
libc/vfprintf.c                                                   (linea 2047)
libc/e_pow.c        (conserva extra_cflags=["-msafe-sda","-mstrict-align"])  (2076)
libc/ef_pow.c       (idem)                                        (2079)
libc/e_rem_pio2.c   (idem)                                        (2142)
libc/kf_tan.c       (conserva extra_cflags=["-msafe-sda"])        (2144)
libc/sf_log.c       (idem)                                        (2169)
```

`keep.lst`: **sin cambios**. Ninguno de los 13 rangos mueve una frontera que
renombre un relleno — `keepchk` se quedó en 22 en **cada uno** de los 22 ensayos.

Copias del paquete en el scratchpad: `c34lib2_splits_FINAL.txt` y
`c34lib2_configure_FINAL.py`.

### Aviso honrado sobre `promote.py`

`promote.py libc/e_pow` se queja de «no definimos 35 símbolos que el extraído
exporta (`lbl_804FEEF0` … ← `auto_01_8031B558_text`)» y **es un falso positivo**:
el enlace real da **0 errores `L0039`** y el DOL sale con el sha1 exacto. Los
`lbl_` de un rango reclamado no los referencia nadie vivo. **El juez es
`trypromo`/el DOL, no `promote.py`.**

---

## 8. Lo que le falta hoy a cada unidad que ya tenía rango

| unidad | B | qué le falta ahora |
|---|---|---|
| `filesys` | 10.872 | **cerrado por alineación**: su `.rodata` real empieza en `0x80413A7C`, **4 mod 8** (veda r33). Más `AddToQueue` (tres ejes cerrados) |
| `pathbank` | 2.156 | el `.rodata` casa 164 B; faltan **8 B**: el original emite `"%s  %s\0"` **dos veces** y GCC funde los literales. Más `PATHI_…` |
| `DebuggerDriver` | 5.988 | `ENLACE FALLA`: `Reference to undefined symbol **Ecb**` (×3). Más 3 funciones (804 B) |
| `steering` | 8.760 | 10 funciones (4.080 B). Sus tres rangos candidatos **CHOCAN** (`SIBios`, `AXVPB`): son dato que emitimos dos veces |
| `e_pow` | 2.000 | **nada: PROMOCIONA** |
| `kf_rem_pio2` | 2.180 | **cerrado por alineación**: `.sdata2` en `0x805007C4`, 4 mod 8, y el contenido sólo casa 60/84 |
| `ppc2D2` | 2.448 | **5 símbolos** (no 7) que otros objetos importan: `__shr2i` (23×), `__div2i` (10×), `__shr2u` (2×), `__shl2i` (2×), `__mod2i` (1×). Son las rutinas de 64 bits de LibSN, **escritas en ensamblador**: fuera de mi alcance por la prohibición de la ronda |

Y dos que no estaban en el encargo y conviene anotar:

- **`LibSN/vm` (2.304 B) y `LibSN/vmbase` (3.180 B)** fallan por **símbolos de
  DATOS que exportamos como locales**: 13 y 7 `g_vm*` que sus propios objetos
  extraídos importan por nombre. Es el mismo patrón que `SDA_*_G` en `fdlibm.h`
  (quitar el enlace interno). No promocionarían igual —les falta una función a
  cada una— pero el trabajo está identificado y **es de datos, no de código**.
- **`bigswizzler`** tiene su `.rodata` **entero** en `0x8041010C`, **4 mod 8**:
  cae en la veda de la r33 igual que los dos códecs.

---

## 9. Vedas nuevas y medidas

1. **`claimrange.py` propone rangos por NOMBRE y hay que cotejarlos contra el
   orden de `.text` antes de gastar un ensayo.** Dos de sus propuestas de hoy
   eran de otra unidad y dan **ciclo de dependencia**:
   `gc_interface .rodata 0x804149E8` (es de `gc_blockcalculator`) e
   `input/cmn/interface .rodata 0x80414DD0..0x80414E80` (es de `device.cpp` +
   `effect.cpp`, que van **detrás** de `gc_device` en `.text`). En los dos casos
   la causa es la misma: **emitimos una vtable que el original emite en otra
   unidad**, y `claimrange` nos la atribuye a nosotros.
2. **`libc/libgcc2_4` no tiene rango que reclamar**: el bloque de 24 B aparece en
   `0x8040FE28`, pero `lbl_8040FE38` mide **152 B** y el rango «acabaría dentro
   de un símbolo». Además `audit.py` ya delata que su literal de aborto es otro
   (`*** Library error ***` contra nuestro `src/libc/libgcc2_4`).
3. **`libc/sf_log` y `libc/kf_tan` NO necesitaban escribir la conversión a mano**
   (la receta `I2D_STORE`/`I2D_VALUE` de `sf_log10.c`, con sus once pines de
   registro). Les bastaba **un rango de 8 B**. Antes de reescribir una función
   con pines, comprueba si el sesgo tiene su propio `lbl_` con
   `size:0x8 align:8 data:double`.
4. **La salida de `promodist.py` a fichero sigue llevando CRLF** (trampa de la
   r33, confirmada hoy): sin `tr -d '\r'`, `trypromo` contesta
   `NO esta en la lista de enlace` a las 40.
5. **`dtk dol split` falla con `os error 32`** cuando otro agente tiene abierto un
   `.o` de `build/GOWE69/obj`. Pasó dos veces y **el mensaje no tiene nada que
   ver con el rango**: es un fichero en uso. Reintentar y ya.

---

## 10. Estado al cerrar

- `config/` y `configure.py`: **idénticos a como los encontré** (`diff -r` limpio).
- `src/`: tres ficheros modificados y **verificados** —
  `libc/e_pow.c`, `libc/ef_pow.c`, `libc/e_rem_pio2.c` (100,000 % de código en
  `libdiff.py`, `audit.py` sin `FALLA`, datos byte a byte contra el DOL).
- Enlace base de los 607 objetos: **DOL OK**. `keepchk`: **22 rancias**.
