# Ronda 23 — el frente de `linked`

Base medida hoy (`report.json` 17:30, `main.dol` sha1 `9619ba57…` = OK):
**`linked` 9,5034 % — 375.008 B, 385/590 unidades.** (El encargo decía 9,24 % y
381: ya estaba caducado cuando empecé.)

## Resultado: 21 unidades promocionables, +22.400 B, +21 ficheros

**`linked` 9,5034 % → 10,0710 % (+0,5677 pp), 385 → 406 de 590.**

Las 21 dan **DOL byte a byte idéntico**: cada una suelta, y **las 21 juntas**
(`scripts/trypromo.py`, verificado dos veces sobre objetos recién construidos).
20 de ellas dan verde sueltas; `scrsfl` sólo con `sfrsf` (racimo).

### Las líneas de `configure.py` — cambiar `NonMatching` por `Matching`

| línea | objeto | B código | qué hizo falta |
|---|---|---|---|
| 1042 | `Speed/Indep/Libs/path/…/cmn/pathaction.cpp` | 5.512 | cadena + dos estáticos de función a `extern` |
| 848 | `Speed/Indep/Libs/snd/…/mix/sfxrevc.c` | 3.668 | **addend equivocado** + orden de `.bss` + 1 constante |
| 886 | `Speed/Indep/Libs/snd/…/cmn/ssys.cpp` | 2.368 | 1 constante del pool a `extern` |
| 1370 | `LibSN/sndvd.c` | 1.584 | 9 renombres `__asm__` + nombrar la cadena |
| 2046 | `libc/libgcc2_6.c` | 1.332 | ✳ nombre propio de `__clz_tab` |
| 2048 | `libc/libgcc2_8.c` | 1.252 | ✳ idem |
| 2049 | `libc/libgcc2_9.c` | 1.176 | ✳ idem |
| 990 | `Speed/Indep/Libs/snd/…/mix/sfrsf.c` | 828 | ✳A `gcc2_compiled._80372D10` → `rsflc` + 2 constantes |
| 1114 | `Speed/Indep/Libs/realcore/…/file/gc/hd_device.cpp` | 820 | **ensamblador retirado** (ver §2) |
| 2108 | `libc/fopen.c` | 648 | ✳B renombre `_sn_iobf`/`_sn_stat_g` |
| 2149 | `libc/sf_asin.c` | 556 | 16 constantes a `SDA_FLOAT_X` (2 valores corregidos) |
| 2152 | `libc/sf_fmod.c` | 456 | `Zero` a `extern` con nombre del troceador |
| 1221 | `Speed/Indep/Libs/realcore/…/system/gc/timerthread.cpp` | 352 | **alineación de `.sdata`** + 3 símbolos globales |
| 844 | `Speed/Indep/Libs/snd/…/gc/ssdfx.c` | 344 | 1 constante del pool |
| 2150 | `libc/sf_cosh.c` | 304 | 3 constantes a `SDA_FLOAT_X` |
| 2097 | `libc/sn_buf.cpp` | 268 | ✳B idem que `fopen` |
| 2155 | `libc/sf_log10.c` | 268 | 7 constantes a `SDA_FLOAT_X`/`SDA_DOUBLE_X` |
| 1106 | `Speed/Indep/Libs/realcore/…/file/cmn/hlsfile.cpp` | 216 | firma de `FILESYS_atomic` (manglado) |
| 774 | `Packages/vp6/…/cmn/TokenEntropy.c` | 196 | orden de `.rodata` + quitar `aligned(8)` |
| 998 | `Speed/Indep/Libs/snd/…/mix/scrsfl.c` | 196 | ✳A (sólo con `sfrsf`) |
| 1433 | `cluttype.cpp` | 56 | renombre `_9RealShape.gTexelTypeToBpp` |

`✳A` = racimo `sfrsf`+`scrsfl` (van juntas o ninguna). `✳B` = `fopen`+`sn_buf`
comparten la cura pero cada una da verde suelta. `✳` = los tres `libgcc2_*` dan
verde sueltos y juntos.

En 1042, 1106, 1114 y 1221 el `NonMatching` está **en su propia línea** (llamada
a `Object(` partida en varias); en las demás va en la misma línea del fichero.

## 2. `hd_device`: el ensamblador falsificado, retirado

El destructor `_._20GcHdFileDeviceDriver` estaba **escrito a mano** con
`asm(".text … stwu 1,-8(1) …")`, y otro `asm` fabricaba `gap_07_804D5014_bss`.
La unidad no promocionaba (`DOL_ROTO d93dffdcc3f2`).

La causa de la falsificación era el `#pragma interface` de la cabecera: con él,
un destructor definido **dentro de la clase** no emite símbolo (la nota
«en clase = inline en GCC 2.9»). Recorrido medido:

| ensayo | forma | resultado |
|---|---|---|
| c1 | dtor fuera de línea + `#pragma interface` | secciones y símbolos idénticos, pero **`measure.py` 85,85 %**: `__static_initialization_and_destruction_0` deja de plegar el cuerpo del dtor |
| c2 | `inline` en el `.cpp` | los tres virtuales desaparecen (nadie los usa: sin vtable no hay `mark_vtable_entries`) |
| c3 | **sin `#pragma interface`, dtor/Restore/Getspace EN CLASE** | **100 %, 13 funciones, y DOL OK** |

Con c3 el objeto emite 68 B de más (copia fuera de línea del ctor) y 128 B de
`.rodata` (`_vt.20GcHdFileDeviceDriver`) — y **aun así el DOL sale idéntico**,
porque el enlazador se come lo que no se referencia. Es la refutación medida de
la §9 de la ronda 22: *una vtable duplicada con un comodín NO bloquea por sí
sola*.

**Y la forma exacta del original está identificada**: `#pragma interface` +
`#pragma implementation` + **`-fno-implement-inlines`** da `.text` de 820 B con
**todos los símbolos en el desplazamiento exacto del extraído** (0 desajustes).
Es la condición `DECL_THIS_INLINE && !flag_implement_inlines && !DECL_VINDEX` de
`import_export_decl`: suprime la copia del ctor (no virtual) y **conserva** la de
los virtuales. Comprobado compilando a mano; **no lo propongo** porque es un
cambio de cflags y la unidad ya promociona sin él. El fichero queda con los dos
`#pragma` puestos, así que añadir la bandera es una línea.

`dvd_device` tiene el mismo patrón (56 B de ctor + 128 B de vtable de más) y no
está al 100 %, así que no entra.

## 3. Los cinco mecanismos nuevos

### 3.1 El pool del compilador vive en un comodín — nómbralo `extern`

El caso mayoritario, y el que más ficheros ha dado. El objeto extraído **no
emite su pool**: referencia `lbl_XXXXXXXX` como símbolo externo, y esos bytes
están en un comodín de `.rodata`/`.sdata`. Nosotros los emitimos → sección de
más → el DOL se desplaza.

La cura es de fuente: declarar el nombre del troceador y usarlo.

```c
extern "C" const float lbl_80412FE8[];   /* 0.0f del pool */
...
prss->history[0] = lbl_80412FE8[0];
```

**Trampa medida (`bigswizzler`)**: el MODO DE DIRECCIONAMIENTO depende de cómo se
declare.

- `extern const float X;` (escalar) → `@sda21` (r13). Es lo que quiere `libc`,
  donde el pool está en `.sdata`. Ahí la cabecera ya trae `SDA_FLOAT`; he añadido
  `SDA_FLOAT_X(name, lbl)` / `SDA_DOUBLE_X(name, lbl)` a `src/libc/fdlibm.h`.
- `extern const float X[];` (array de tamaño desconocido) → `lis @ha` + `lfs @l`.
  Es lo que quiere `.rodata`.

Elegir mal cuesta el porcentaje: en `bigswizzler` la forma escalar dio
`L1001: Failed to create small data reference` y la forma array bajó
`measure.py` de 100 % a **26,82 %**. **Revertida.**

Aplicado con éxito en `sfrsf`, `ssdfx`, `ssys`, `pathaction`, `sf_cosh`,
`sf_log10`, `sf_asin`, `sf_fmod`, y parcialmente en `sf_log`.

**Lo que NO se puede curar así**: la constante de sesgo `0x4330000080000000` /
`0x4330000000000000` que **genera el propio compilador** para `int → double`. No
sale de ningún literal del fuente, así que no hay nombre que ponerle. Bloquea
`stagpat`, `pathinit`, `pathvol`, `bigyuvswizzler`, `libgcc2_4`, `svol`,
`sstfxlev` y los 8 B que le quedan a `sf_log`. Esas son de `splits.txt` (§5) —
salvo que se reescriba la conversión con el `I2D_STORE`/`i2d_bias` que ya usa
`sf_log10`, que no he intentado.

### 3.2 El ADDEND de una reubicación es invisible para objdiff

**El hallazgo más importante de la ronda.** En `sfxrevc` nuestro código
apuntaba a `tablecopyto+0` donde el original apunta a `tablecopyto−252` (el
troceador lo llama `SNDDRV_dolbypl2balances+0x714`). `measure.py` daba
**100 %**, `audit.py` daba **0 FALLA**, y el DOL salía con **exactamente 2 bytes
distintos**.

La causa era de fuente: el original escribe

```c
&tablecopyto[validstructoutputs - 1][sndmix.mc.outputchannels - 1][i][0]
```

y GCC pliega los dos `−1` (−216−36 = −252) **dentro del addend del símbolo**.
Nosotros indexábamos desde `[validstructoutputs][0][0][0]` y sumábamos
`outputchannels * 36` en tiempo de ejecución: mismas instrucciones, misma
grafía, dirección distinta.

Herramienta nueva: `c23lk_reloc.py` (scratchpad) compara las reubicaciones
**por dirección resuelta** (símbolo de `symbols.txt` + addend) en vez de por
nombre. Barrido sobre las 95 candidatas de fuera de SourceLists: sólo
**`pathreal`** tiene otra divergencia real sin diagnosticar
(`_vt.Q24Path11IPathToReal` 0x804131E0 contra `_16PathToIAllocator.memimptags`
0x804CC6EC). El resto son `$LC` de pool (§3.1) o desplazamientos de símbolos
dentro de la propia unidad.

### 3.3 La ALINEACIÓN de sección desplaza el enlace

`timerthread` llevaba tres rondas saliendo **LIMPIA** en `promote.py` y
rompiendo el DOL. Secciones idénticas, tamaños idénticos, símbolos en el mismo
desplazamiento, sin COMMON, reubicaciones correctas. La única diferencia:

    extraido: .sdata (16 B, align 8)     nuestro: .sdata (16 B, align 4)

y el DOL salía con **36 bytes sueltos**, todos desplazamientos `@sda21`
corridos 4 B. `__attribute__((aligned(8)))` sobre el primer objeto de la sección
lo cierra: **DOL OK**.

**El contrapeso, y hay que respetarlo: `gas` redondea el TAMAÑO de la sección a
su alineación.** En `timerthread` sale gratis (16 ya es múltiplo de 8); en
`sfxrevc` `.data` de 1.596 B pasó a 1.600 y el DOL empeoró de 2 bytes a 162.761.
La regla: **sólo sirve si `tamaño % alineación == 0`.**

Y al revés, `TokenEntropy`: llevaba dos `__attribute__((aligned(8)))` **nuestros**
que subían `.rodata` de 660 a 664 B. Quitándolos (los desplazamientos ya los fija
el símbolo `gap_05_8041227E_rodata`) → **DOL OK**. Es un quinto caso de
«invención nuestra que era la causa».

Barrido completo: **32 de las 106 candidatas** tienen alguna sección con menos
alineación que el extraído; en 18 de ellas el tamaño ya es múltiplo, así que el
atributo es gratis. Ojo: la desalineación **no siempre es mortal** — `ssys`,
`hd_device`, `sndvd` y `sfxrevc` promocionan con ella.

### 3.4 Un símbolo `static` que el original exporta

`timerthread` declaraba `static` sus `TimerThread`, `TimerThreadMsgData` y
`TimerThreadStack`; el objeto extraído los exporta. Con enlace interno el
símbolo sale LOCAL y `promote.py` los canta como «faltan». Quitar el `static`
(mecanismo hermano del `extern "C"` de la r22).

Lo mismo por el otro lado en `pathaction`: dos `static` **dentro de una
función** (`lastwhile`, `lastendif`) que el original **no define aquí** —
referencia `lastwhile.78_804FF608` y `lastendif.79_804FF60C`. Se sacan a
`extern … __asm__("<nombre del troceador>")` de ámbito de fichero, y como son
escalares de 4 B salen con `@sda21`, igual que el objetivo.

### 3.5 Los `<nombre>_<DIRECCION>` del troceador — barrido automático

Extensión del mecanismo 5 de la r22 (`extern "C"` que falta): cuando nuestro
objeto tiene un UNDEF `X` y el extraído tiene `X_8xxxxxxx`, es el mismo símbolo
con el nombre que le puso el troceador. Barrido sobre las 95 candidatas de fuera
de SourceLists: **sólo tres** (`fopen`, `sn_buf`, `cluttype`), y las tres
cierran con `__asm__`. Con `sndvd` (9 renombres a mano) son cuatro.

Variante del mismo patrón en `libgcc2_6/_8/_9`: el DOL tiene **cuatro copias**
de `__clz_tab` (0x804FFF40 de `L_divdi3` y 0x80500040/140/240 de las otras tres)
y nuestra cabecera compartida hacía que las cuatro apuntaran a la primera.
Parametrizando el nombre (`LIBGCC2_CLZ_TAB_SYM` en
`src/libc/libgcc2_udivmoddi4.h`) cierran las tres.

Y una más, de manglado C++ puro: `hlsfile` declaraba
`FILESYS_atomic(int(*)(int,void*), void*, int, void*)` y el original es
`(…, FILEDEVICE*, int, void*)` → llamaba a `FILESYS_atomic__FPFiPv_iPviT1`
donde el DOL tiene `…__FPFiPv_iP10FILEDEVICEiPv`.

## 4. Los `$LC` que `promote.py` canta como «faltan» y no faltan

`scrsfl` sale con `falta ['gcc2_compiled._80372D10', 'lbl_80413090',
'lbl_80413098']`. Dos de esos tres **no son un problema**: son las constantes de
su pool, que nosotros llamamos `$LC0`/`$LC1`. Son símbolos **locales**, mismo
tamaño de sección y mismos bytes: el enlazador no los mira. Confirmado con el
DOL idéntico.

Regla práctica: **antes de perseguir un `falta` de `lbl_`, mira si el `.rodata`
tiene el mismo tamaño en los dos objetos.** Si lo tiene, es sólo el nombre.

## 5. Lo que sólo se arregla en `splits.txt` (VEDADO), con dueño único

Comprobación de dueño único hecha así: buscar quién **referencia** cada símbolo
en `build/GOWE69/asm/` (los objetos comodín incluidos). Si sólo lo referencia
una unidad y el comodín que lo contiene empieza justo ahí, el dueño es esa
unidad.

| unidad | rango | quién lo tiene hoy | dueño único |
|---|---|---|---|
| `Packages/vp6/…/cmn/quantize` | le **FALTAN** 40 B en 0x804119A8 | el extraído los define, nuestro código no los genera | — (caso inverso; sin diagnosticar) |
| `Speed/Indep/Libs/snd/…/cmn/stagpat` | 0x804129E0..0x804129F0 | comodín | sí: sólo `stagpat.s` referencia `lbl_804129E0/E8/EC` |
| `Speed/Indep/Libs/path/…/cmn/pathinit` | 0x80413140..0x80413160 | comodín | sí: sólo `pathinit.s` |
| `Speed/Indep/Libs/path/…/cmn/pathvol` | 0x804138A8..0x80413958 | comodín | sí: sólo `pathvol.s` |
| `egami/rcmp/…/vd/gc/bigyuvswizzler` | 0x804100A4..0x804100F0 | comodín | sí |
| `libc/libgcc2_4` | 0x8040FE28..0x8040FE40 | comodín | sí: sólo `libgcc2_4.s` |
| `libc/sf_log` | 0x804FF120 (8 B) | comodín | sí: sólo `sf_log.s` |
| `Speed/Indep/Libs/snd/…/mix/sfrsf` | 0x80412FE8..0x80412FF0 | `auto_05_80412FE8_rodata` **empieza ahí** | sí — **ya no hace falta**: cerrado por fuente |

Los ocho de la lista de la r22 (`slib`, `sst`, `pathsnd`, `gc_driver`, `sndvd`,
`postproc`…) siguen igual salvo `sndvd`, que **he cerrado por fuente** (era
renombre, no `splits.txt`).

**El bloque de 28 B de `realmemcard` no lo he tocado** (encargo de otro agente).

## 6. Racimos identificados

| racimo | unidades | por qué |
|---|---|---|
| **A** `sfrsf` + `scrsfl` | 2 | el extraído de `sfrsf` referencia `gcc2_compiled._80372D10`, que es `rsflc`, la primera función de `scrsfl`. `scrsfl` sola: `ENLACE FALLA` |
| **B** `fopen` + `sn_buf` | 2 | misma cura (`_sn_iobf`/`_sn_stat_g`); cada una da verde suelta |
| `libgcc2_6/_8/_9` | 3 | comparten cabecera; cada una necesita SU copia de `__clz_tab`. Verde sueltas y juntas |
| realcore/input | 10 | `effect`, `event`, `eventqueue`, `effectimp`, `interfaceimp`, `gc_effect` dan **el MISMO hash roto** `0543d0562f81`. Medido con `c23lk_doldiff.py`: cada una hace crecer `.rodata` **32 B** al emitir su `_vt.*`. Las 11 vtables viven en 0x80414B40..0x80414E98, en un comodín. Ver §7 |
| realmemcard | 6 | hash `fc9e604fe105` — **de otro agente** |
| realmemcard | 2 | `interfaceimp` + `gc_blockcalculator`, hash `20228d9ab78c` |

## 7. Lo que NO he probado (por orden de lo que yo atacaría)

1. **El racimo de `realcore/input`** (10 unidades, ~5.700 B). Diagnóstico hecho:
   cada objeto emite su `_vt.Q29RealInput*` y varios `operator new`/`delete` de
   clase. **Los operadores no molestan** (el enlazador los tira: el `.text` de
   salida no cambia de tamaño); **la `.rodata` sí**. Y `hd_device` demuestra que
   una vtable duplicada **puede** no molestar, así que hay que entender por qué
   ahí sí. No he probado poner `#pragma interface` en
   `include/common/realcore/input.h`, que es cabecera compartida por las diez.
2. **`sf_log`** (640 B): 16 de sus 17 constantes ya están convertidas; queda el
   sesgo `int→double` que genera el compilador. La cura sería copiar el
   `I2D_STORE`/`i2d_bias` de `sf_log10`, que tiene pines de registro y `asm`
   vacíos: tres sitios (`dk = (float)k`) y riesgo real de bajar el porcentaje.
3. **`sf_exp`** (556) y **`sf_expm1`** (804): 5 y 9 de sus constantes están
   declaradas con `SDA_FLOAT`; las otras 10 y 14 no aparecen en el fuente
   (`c23lk_sda.py` las lista). Hay que ver de dónde salen antes de tocar.
4. **`sserver`** (992): 4 B de pool (0.0f), `lastTick.126_804502F4` (estático de
   función, como `pathaction`), `.bss` 16 contra 12 y dos símbolos de `.bss`
   movidos. Tres arreglos conocidos en una sola unidad; no me dio tiempo.
5. **`sndfxbus`** (528), **`sstvol`** (588), **`satospkr`** (1.796),
   **`sst3dpos`** (692): racimo de `.rodata` de snd en 0x804128xx-0x80412Cxx.
   `sst3dpos` referencia `lbl_804128B0..BC` que define su propio objeto
   extraído; `sndfxbus` emite además 76 B de `.text` de más
   (`SetOutputLevel__Q23Snd17GlobalFxProcessorf`).
6. **`pathreal`** (496): la divergencia de reubicación de §3.2 sin resolver.
7. **`quantize`** (896): el caso inverso — le faltan 40 B de `.rodata`
   (`lbl_804119A8`) que el extraído sí tiene. No lo he mirado.
8. **Los 84 «secciones de más» uno a uno**: he clasificado los 40 que tienen
   `.rodata`/`.sdata`/`.sdata2` de más (`c23lk_pool.py` da, por unidad, cada
   `lbl_` externo con su valor leído del DOL y nuestro pool en hexadecimal, para
   emparejarlos a ojo en un minuto). Los que no he abierto son los grandes:
   `vfprintf` (144+8+128), `math_support` (224+152), `e_pow`, `ef_pow`,
   `k_rem_pio2`, `kf_rem_pio2`, `e_rem_pio2`, `ef_rem_pio2`, `e_exp`,
   `creates` (336), `csis` (168), `rcmp_vp6_codec` (360), `rcmp_mad_codec` (288),
   `pathsnd` (1.128).
9. **`sf_atan` y `kf_tan`** (`L1001`, desbordamiento de `_SDA_BASE_`): sin mirar.
   Nota: `sf_atan` tiene además cinco `lbl_804FF21C..2C` que **define el propio
   objeto extraído** y nuestro código no.
10. **`bigswizzler`** (1.208): probado y **revertido** (−73 pp en `measure.py`).
    Veda anotada: la forma `extern const float lbl[]` con `lbl[0]` en las tres
    LOD de `GXInitTexObjLOD` cambia el reparto de registros de la función.
11. **Las SourceLists** (`zSim`, `zLua`, `zMisc`, `zAnim`, `zAttribSys`,
    `zFoundation`, `zMiscSmall`, `zRender`, `zDebug`, `zGameModes`, `zMission`):
    confirmo el veredicto de la r22 — nuestro `.text` mide 18 kB más que el
    extraído en `zSim`, 19 kB en `zLua`… No es problema de enlace.

## 8. Cosas para decidir fuera de este encargo

1. **`audit.py` da un FALSO FALLA en `timerthread`**:
   `ttInit__Fv FALLA: tipo/addend de reubicacion distinto en systemtasksubs_804D63F8`.
   Nuestro objeto referencia `TimerThreadStack+0x1000` y el extraído
   `systemtasksubs_804D63F8+0`; `symbols.txt` pone `TimerThreadStack` en
   0x804D53F8 con 0x1000 de tamaño y `systemtasksubs` en **0x804D63F8**: es la
   misma dirección. Reproducido en dos pasadas. El DOL byte a byte idéntico es
   la prueba. Es la misma clase de fallo que el `gcc2_compiled._ADDR` que
   reportó la r22: `audit.py` compara el NOMBRE, no la dirección resuelta.
   El arreglo sería resolver ambos lados con `symbols.txt` antes de comparar
   —justo lo que hace `c23lk_reloc.py`—. **No lo he aplicado**: herramienta
   compartida.
2. **`LibSN/sndvd.c` tiene ensamblador escrito a mano** y sigue teniéndolo: un
   bloque `__asm__` de nivel de fichero con la función completa `DSIExcHandler`
   (~40 instrucciones, líneas 147+) y un **`__asm__(".long 1")`** que emite una
   palabra de datos dentro del `.text` (línea ~272). Lo segundo es falsificación
   pura. No lo he tocado porque la unidad **ya promociona** y reescribir un
   manejador de excepción sin prólogo no es trabajo de esta ronda; los `mtspr`
   de las líneas 87/96 sí parecen legítimos (no hay forma en C).
3. **`-fno-implement-inlines` para `hd_device`** (§2): cierra los 68 B del ctor
   y deja el `.text` con los desplazamientos exactos del extraído. Es un cambio
   de cflags en `configure.py` y no hace falta para promocionar.

## 9. Herramientas nuevas (scratchpad, prefijo `c23lk_`)

| script | qué hace |
|---|---|
| `c23lk_cand.py` | universo real de candidatas (código al 100 %, con objeto, en el enlace) |
| `c23lk_scan.py` | lote de `trypromo` (heredado de `c22link_scan.py`) |
| `c23lk_doldiff.py` | **enlaza y dice DÓNDE difiere el DOL**: secciones que cambian de tamaño y los rangos de bytes con su dirección. Es lo que convirtió «DOL ROTO» en un diagnóstico |
| `c23lk_reloc.py` | compara reubicaciones **por dirección resuelta**, no por nombre (§3.2) |
| `c23lk_pool.py` | por unidad: qué `lbl_` externos referencia el `.s` del objetivo, con su valor leído del DOL, contra nuestro pool |
| `c23lk_sda.py` | empareja por VALOR las `SDA_FLOAT/DOUBLE` de un `.c` de libc con esos `lbl_`, y aplica la conversión a `SDA_FLOAT_X` |
| `c23lk_patch.py` | parcheador con copia de seguridad y finales de línea del fichero |

## 10. Ficheros de fuente tocados

`hd_device.cpp` + `.h`, `sfrsf.c`, `ssdfx.c`, `sfxrevc.c`, `ssys.cpp`,
`TokenEntropy.c`, `sndvd.c`, `pathaction.cpp`, `hlsfile.cpp`, `timerthread.cpp`,
`cluttype.cpp`, `fopen.c`, `sn_buf.cpp`, `sf_cosh.c`, `sf_log10.c`, `sf_log.c`,
`sf_asin.c`, `sf_fmod.c`, `libgcc2_6.c`, `libgcc2_8.c`, `libgcc2_9.c`, y dos
cabeceras compartidas: `src/libc/fdlibm.h` (dos macros nuevas, nada existente
cambia) y `src/libc/libgcc2_udivmoddi4.h` (nombre del `__clz_tab`
parametrizado, con el valor de antes por defecto).

**Comprobación de la cabecera compartida**: las **34 unidades** que incluyen
`fdlibm.h` reconstruidas y medidas → `22.552/22.552 B, 100 %`. Las 4 que
incluyen `libgcc2_udivmoddi4.h` → `5.164/5.164 B, 100 %`.

`measure.py` global: **97,2573 % al empezar → 97,3503 % al terminar**
(3.837.972 → 3.841.640 B). Ninguna unidad baja. `audit.py` sobre las 21: **0
FALLA**, salvo el falso positivo de `timerthread` de §8.1, confirmado en segunda
pasada y explicado.

---

## 11. Apéndice: barrido de reubicaciones sobre las SourceLists

Al cerrar la ronda quedaba un hueco: el barrido de §3.2 cubría las 95 candidatas
de **fuera** de SourceLists, y las 11 SourceLists nunca se barrieron porque
`c23lk_reloc.py` no terminaba en 900 s.

**La causa era un fallo de la herramienta, no del tamaño**: la búsqueda del
nombre de un símbolo rebanaba `d[offset:]` —desde el offset **hasta el final del
fichero**— una vez por reubicación. En un objeto de SourceList con decenas de
miles de reubicaciones eso es cuadrático. Con la rebanada acotada a 512 B y un
memo por índice de símbolo: **900 s → 2,5 s** (y 1,1 s las 95 de antes).

Verificación de que la corrección no cambia veredictos: re-ejecutado sobre las
95, reproduce todos los hallazgos que siguen aplicando. Las diferencias contra
la pasada vieja se explican solas y **ninguna es del arreglo**:

- `libgcc2_6/_8/_9` **desaparecen** de la lista de divergentes: es mi propio
  arreglo de `__clz_tab` al haber entrado.
- `kf_tan` 21 → 1 y `sf_atan` 25 → 8 divergencias: sus `.o` estaban **rancios**;
  al reconstruir las 34 unidades de `fdlibm.h` se quedaron al día. Es la trampa
  del «`.o` rancio» del manual, en vivo.
- `gc_pad` desaparece porque **otro agente reconstruyó su objeto a las 21:15**.
  Mi hallazgo anterior sobre `gc_pad` era contra un objeto viejo: **no lo des
  por bueno.**

### Lo que encuentra en las SourceLists: 5 unidades, 21 divergencias reales

Ninguna cambia nada de esta ronda —las 11 SourceLists son impromocionables por
tamaño de `.text`, no por enlace— pero son «100 % que miente» para quien las
trabaje:

| unidad | dónde | qué pasa |
|---|---|---|
| **`zSim`** | `.rodata+0A84..0CDC`, **10 casos** | son entradas de **vtable**. Nuestra vtable de `NISActivity`, `CareerGame` y `QuickGame` lleva punteros a funciones distintas de las del original (p. ej. donde el DOL tiene `Detach__Q23Sim8Activity` nosotros ponemos `_._10CareerGame`). Es **orden de funciones virtuales**, y `measure.py` no lo ve |
| **`zMisc`** | `.data+0684..0734`, **6 casos** | el original guarda punteros a variables con nombre (`sCutiePetootie`, `sPixelClamp`, `bCartoonMode`, `sBigHead`, `sWireOrange`) y nosotros guardamos punteros a nuestras propias cadenas `$LC…` |
| **`zRender`** | `.text`, 3 casos | destino desplazado a la función vecina (`SmackableRender_Init` por `_Shutdown`, etc.) |
| **`zAnim`** | `.text+0334` | `bFree__FP8SlotPoolPv` donde el original llama a `bCountFreeSlots__FP8SlotPool` |
| **`zLua`** | `.text+0166` | `LuaRealloc` contra un `gcc2_compiled.` |

La de `zSim` es la más rentable de mirar: diez entradas de vtable mal es un
síntoma de una sola causa (orden de declaración de los virtuales en la clase).

## 12. Estado al cerrar: las 21 YA ESTÁN APLICADAS

Verificado después de escribir lo anterior: `configure.py` ha pasado de **389 a
410** `Matching` (exactamente mis 21) y `build.ninja` está regenerado. Las 21
unidades se toman ya de `build/GOWE69/src/`.

**Comprobación definitiva con el árbol tal cual está**: enlace sin ninguna
sustitución →
`main.dol` sha1 **`9619ba57c9919f95f7f2ac951a2166a3517f91e3`** = el original,
byte a byte. Los 590 objetos del enlace, 21 de ellos nuestros de más.

Aviso para la siguiente tanda: **hay otros agentes reconstruyendo en el mismo
árbol** (`gc_pad.o` cambió a las 21:15 mientras yo medía). Cualquier diagnóstico
guardado de esta ronda hay que reconfirmarlo contra objetos recién construidos
antes de fiarse.

## CORRECCION (r24, verificada a mano): el hallazgo de zSim NO se reproduce

Comprobado entrada a entrada, resolviendo las reubicaciones de nuestro `.o` por
nombre de simbolo contra la vtable del ELF:

```
_vt.10CareerGame.Q23Sim9IActivity   objetivo 0x80404F88
  [ 3] _._10CareerGame                    == nuestro
  [ 5] Release__Q23Sim8Activity           == nuestro
  [ 7] Attach__Q23Sim8ActivityPQ33UTL...  == nuestro
  [ 9] Detach__Q23Sim8ActivityPQ33UTL...  == nuestro
  [11] GetAttachments__CQ23Sim8Activity   == nuestro
```

**El ejemplo concreto del informe --«donde el DOL tiene `Detach__Q23Sim8Activity`
nosotros ponemos `_._10CareerGame`»-- es falso: las dos vtables son identicas.**

La causa probable es la que el agente del addend de la r24 identifico y corrigio
en su propia herramienta: **comparar por (seccion, desplazamiento) no vale en las
SourceLists**, porque nuestro `.text` mide decenas de kB mas y todo va corrido.
La llave tiene que ser el simbolo. Su barrido, que si usa el simbolo como llave,
cubrio 526 unidades incluidas las SourceLists --de ahi salieron los hallazgos
reales de `zFe`, `zEcstasy` y `zEAXSound2`-- y **zSim no aparece**.

Aviso para quien lea esta seccion: **no lances un agente contra zSim, zMisc,
zRender, zAnim ni zLua por esto** sin reproducir primero un caso a mano con la
llave por simbolo. Y ojo con `gcc2_compiled.`, que sale 389 veces en
`symbols.txt`: resolver una direccion a nombre a ciegas da falsos positivos en
masa (a mi me dio 119 de golpe al intentarlo).
