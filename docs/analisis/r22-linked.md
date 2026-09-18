# Ronda 22 — el frente de `linked`

Estado de partida (medido hoy, `report.json` de las 15:05, `main.dol` con
sha1 `9619ba57…` = OK): **`linked` 8,1597 % — 321.984 B, 352/589 ficheros**.

## 1. Lo primero: el encargo estaba caducado

El encargo hablaba de **143 unidades** al 100 % sin promocionar, con 137
diagnosticadas. Regenerando `promote.py` sobre el árbol de hoy salen **114**, y
la diferencia **no** es sólo por las diez que la ronda 21 promocionó:

- 10 se fueron por estar ya `Matching` (fseek, memset, strstr, wcstring, maddec,
  borders, decodemv, FrameIni, sfsplit, pathserv).
- **22 se fueron porque `candidatas()` exige `matched_data == total_data`** y hoy
  el informe da los datos incompletos en unidades cuyo código sigue al 100 %
  (`zAnim`, `zAttribSys`, `zFoundation`, `zLua`, `zSim`, nueve de `vp6`,
  `sstvol`, `sdspmix`, `mbtowc_r`, `strtod2`…). Diez de esas 22 son exactamente
  las que la 21 promocionó **con el DOL byte a byte idéntico**, así que la
  medida de datos **no es un criterio válido** para decidir promociones.
- 3 entraron nuevas (`spchbank`, `spchevnt`, `spchrule`).

**El universo real es otro: 192 unidades tienen el CÓDIGO al 100 % y no están
promocionadas** (619.476 B), de las cuales 53 son objetos comodín `auto_*` (sin
fuente, nunca promocionables) y 4 no tienen regla en `build.ninja`
(`asd2`, `author`, `mpegl3base`, `smixvec`). Quedan **135 unidades reales**.

## 2. El método: `promote.py` no es el filtro, el enlace sí

`promote.py` es **demasiado conservador**. De las 114 candidatas sólo declaraba
3 «LIMPIA» (y una de ellas, `timerthread`, rompe el DOL). Enlazando de verdad
las 135 unidades **una a una** —2,1 s por enlace, siete minutos el barrido
entero— salieron **16 con el DOL idéntico**, muchas con «pegas» que `promote.py`
consideraba mortales (secciones de más, símbolos movidos). Motivo: `-strip-unused-data`
se come lo que no se referencia, y un hueco al final de sección no siempre
desplaza nada.

Herramientas escritas para esto (en el scratchpad, prefijo `c22link_`):

| script | qué hace |
|---|---|
| `c22link_scan.py` | lote de `trypromo`: enlaza y compara el DOL, N grupos por invocación, resultado en JSON |
| `c22link_greedy.py` | acumulación voraz: parte de un conjunto verde y prueba a añadir cada candidata; varias pasadas |
| `c22link_full.py` | diagnóstico completo por unidad **sin truncar** (secciones, faltan/sobran, movidos, undefs con sufijo) |
| `c22link_rodata.py` | clasifica la `.rodata` que emitimos de más: **pool del compilador** contra **cadenas** |
| `c22link_blobs.py` | dice **en qué dirección del DOL** vive esa sección de más y qué `auto_*` se la queda |
| `c22link_at.py` | símbolos de una unidad ordenados por sección+desplazamiento, nuestro contra extraído |
| `c22link_mangle.py` | barrido de desajustes de manglado C++ (`extern "C"` que falta) |

## 3. Resultado: 27 unidades promocionables

**+37.528 B de código, +27 ficheros. `linked` 8,1597 % → 9,1107 % (+0,951 pp),
352 → 379 de 589.** Las 27 juntas dan **DOL OK**, verificado dos veces con
`scripts/trypromo.py` sobre objetos recién construidos.

22 de ellas dan verde **sueltas**; las otras 5 sólo dan verde **en su racimo**.

### Racimo A — el despachador de vp6 (4 unidades, 18.100 B)

    doptsystemdependant + deblock + loopfilter + dering

`doptsystemdependant.c` alcanzaba tres funciones por el **nombre que inventa el
troceador** (`gcc2_compiled._8034F21C`, `._803537FC`, `._8035645C`), que es el
marcador de principio de unidad de traducción y coincide con la primera función
del `.c` hermano. Promocionar el hermano borraba ese nombre y el enlace caía.

Arreglo en fuente: usar el nombre real, que `symbols.txt` documenta en la misma
dirección (`SetupDeblockValueArray_Generic`, `DeringBlockStrong_C`,
`SetupBoundingValueArray_Generic`).

Y dos correcciones más, sin las cuales el DOL se desplazaba:

- `deblock.c` y `loopfilter.c` declaraban sus punteros globales como
  **definiciones tentativas** → el objeto los emitía **COMMON**, mientras el
  original los tiene en `.sbss`. `__attribute__((section(".sbss")))` los coloca
  exactamente donde el extraído (`= 0` NO vale: GCC los manda a `.sdata`).
  `-fno-common` da el mismo resultado pero es un cambio de cflags.
- `dering.c` no definía nada de su `.bss` (400 B): faltaban `DeringModifierV1`
  (COMMON) y los 144 B que el troceador llama `FData.84_804BE56C` y que
  `criticalpath` referencia. Se definen con `section(".bss")` y alias
  `__asm__("FData.84_804BE56C")`.

### Racimo B — snd (3 unidades, 1.884 B)

    svecreal + sexithndl + stpparse

`sexithndl` y `stpparse` no pueden ir sin `svecreal`, cuyo objeto extraído
referencia `gcc2_compiled._8036D248` y `systaskadded.168_8045054C`.

### Las 20 sueltas

| B código | unidad | qué hizo falta |
|---|---|---|
| 10.404 | `Packages/vp6/…/cmn/deblock` | (racimo A) |
| 6.132 | `Packages/vp6/…/cmn/dering` | (racimo A) |
| 3.724 | `Speed/Indep/Libs/spch/…/cmn/spchevnt` | nada |
| 2.520 | `Speed/Indep/Libs/path/…/cmn/pathevent` | **reordenar tres funciones** |
| 2.116 | `Packages/vp6/…/cmn/vfwpbdll_if` | mover `VP6_StopDecoder` al final + las cadenas |
| 1.820 | `Speed/Indep/Libs/spch/…/cmn/spchbank` | nada |
| 1.700 | `Speed/Indep/Libs/snd/…/cmn/stpparse` | (racimo B) |
| 1.288 | `Packages/vp6/…/cmn/decodemode` | **recuperar 1.040 B de `.rodata`** |
| 1.244 | `Speed/Indep/Libs/spch/…/cmn/spchrule` | nada |
| 1.220 | `Packages/vp6/…/cmn/loopfilter` | (racimo A) |
| 1.044 | `Packages/vp6/…/cmn/simpledeblocker` | **traer tres cadenas de `vfwpbdll_if`** |
| 780 | `egami/rcmp/…/av/cmn/audioplayer` | nada |
| 480 | `LibSN/syscalls` | nada |
| 432 | `Speed/Indep/Libs/realcore/…/debug/cmn/abortmsg` | nada |
| 420 | `Speed/Indep/Libs/realcore/…/input/cmn/memvectors` | nada |
| 364 | `Packages/vp6/…/gc/uoptsystemdependant` | nada |
| 360 | `Speed/Indep/Libs/spch/…/cmn/spchcsis` | nada (estaba en `VETADAS` de `promote.py` y ya no lo está) |
| 344 | `Packages/vp6/…/gc/doptsystemdependant` | (racimo A) |
| 344 | `model` | nada |
| 188 | `Packages/vp6/…/cmn/allocator` | nada |
| 188 | `Speed/Indep/Libs/snd/…/mix/smixptch` | nada |
| 152 | `Speed/Indep/Libs/snd/…/cmn/svecreal` | nada |
| 88 | `Speed/Indep/Libs/snd/…/mix/sx87d16` | nada |
| 48 | `Speed/Indep/Libs/snd/…/mix/smixc` | nada |
| 52 | `Speed/Indep/Libs/realcore/…/debug/cmn/debugger` | **`extern "C"` en `OSPanic`** |
| 44 | `Speed/Indep/Libs/realcore/…/debug/gc/printdrv` | **`extern "C"` en `OSReport`** |
| 32 | `Speed/Indep/Libs/snd/…/cmn/sexithndl` | (racimo B) |

## 4. Los mecanismos nuevos que salieron de aquí

1. **COMMON contra `.sbss`/`.bss`.** Una definición tentativa (`UINT32 *p;`)
   se emite COMMON; el original la tiene en `.sbss`. `= 0` la manda a `.sdata`
   (mal); `__attribute__((section(".sbss")))` la deja donde toca. Explica
   `deblock`, `loopfilter`, `dering`, `postproc` y `quantize`.
2. **El orden de las funciones en el `.c` es promocionable.** `pathevent`
   sólo tenía tres símbolos en distinto sitio de `.text`; reordenar las
   definiciones (`releaseevent`, `removeevent`, `moveevent`) cerró la unidad
   entera, y `measure.py --cmp` da `+0 B, 0 unidades cambian`.
3. **Cadenas en la unidad equivocada.** `vfwpbdll_if` emitía 36 B de literales
   que en el original viven en la `.rodata` de `simpledeblocker`. Moverlos
   (con `__asm__("lbl_80411AD0")` para que el objeto extraído los siga
   resolviendo) cerró las dos.
4. **`gcc2_compiled._ADDR` = la primera función de esa unidad.** No es una
   función distinta: es el marcador de principio de TU, que cae en la misma
   dirección. Sustituirlo por el nombre real disuelve el racimo.
5. **Un `extern "C"` que falta es una reubicación a un símbolo inexistente.**
   `snddrv.c` se compila con `-x c++` y declaraba nueve funciones de `smixer.c`
   y `slinkmix.c` sin `extern "C"`: llamaba a `MIX_play__Fi` cuando el original
   llama a `MIX_play`. `audit.py` pasa de **11 FALLA a 4**. La unidad sigue sin
   poder promocionarse por otras razones, pero el arreglo es real.

## 5. Lo que sólo se arregla en `splits.txt` (VEDADO)

Confirmado con `c22link_blobs.py`, que localiza en qué objeto comodín vive el
contenido que nosotros emitimos de más:

| unidad | sección de más | dónde está en el DOL | comodín que se la queda |
|---|---|---|---|
| `Speed/Indep/Libs/snd/…/cmn/slib` | `.rodata` 32 B | **0x80412DF8** | `auto_05_80412DF8_rodata` |
| `Speed/Indep/Libs/snd/…/cmn/sst` | `.rodata` 40 B | **0x80412888** | `auto_05_80412880_rodata` |
| `Speed/Indep/Libs/path/…/cmn/pathsnd` | `.rodata` 1.128 B (5 vtables) | **0x80413300** | `auto_05_804130A0_rodata` |
| `Packages/realmemcard/…/gc/gc_driver` | `.rodata` 184 B | **0x80414398** | `auto_05_80413958_rodata` |
| `Packages/realmemcard/…/cmn/memcard_taskmanager` y otras cuatro | `.rodata` 28 B (`"s"`,`"ss"`,`"sss"`,`"ssss"`,`"ssdd"`) | **0x80414398** | idem, compartido |
| `LibSN/sndvd` | `.rodata` 24 B (`"Unknown DVD cmd (%08X)"`) | **0x8040F1C0** | `auto_05_8040F1B8_rodata` |
| `Packages/vp6/…/cmn/postproc` | `.rodata` 80 B (pool de dobles) | — | ningún comodín lo tiene con esos bytes exactos |
| `Packages/vp6/…/cmn/quantize` | le FALTAN 40 B de `.rodata` (`lbl_804119A8`) | 0x804119A8 | el extraído los tiene y nuestro código no los genera |

El bloque de 28 B de `realmemcard` es el más rentable si se decide tocar
`splits.txt`: **`memcard_taskmanager` (6.624 B), `memcard_interface` (1.736 B),
`memcard_interface_impl` (1.716 B) y `gc_memcard_taskmanager` (304 B) no tienen
NINGUNA otra pega** — 10.380 B y 4 ficheros a cambio de atribuir 0x80414398.

## 6. Vedas medidas en esta ronda

- **`timerthread` sigue rompiendo el DOL** (`3c8a42c8a3ab` con `systask`,
  `9384074e9578` sola). Confirmado.
- **`libgcc2_4 + _6 + _8 + _9` juntas: `DOL ROTO 80da52e3b983`.** El racimo del
  `__clz_tab`/`lbl_804FFF40` no basta: `libgcc2_4` emite 24 B de `.rodata` de más.
- **`zSim` (96.400 B) es imposible por construcción**: nuestro `.text` mide
  114.688 B contra los 96.400 del extraído (18 kB de instanciaciones que en el
  original viven en otras unidades). Lo mismo `zLua` (113.172 contra 94.264),
  `zAnim` (51.368 contra 42.292), `zMisc` (86.368 contra 78.008). No es un
  problema de enlace.
- **`zFoundation`** está a 348 B de `.text` (36.348 contra 36.000) pero le
  faltan 1.320 B de `.rodata` y `UMath.cpp:93` referencia `lbl_803EB444`, que
  define el propio objeto extraído de `zFoundation`.
- **`sst3dpos`** (692 B) no puede ir: su fuente alcanza `lbl_804128B0..BC` por
  referencia externa y el que los define es su propio objeto extraído.
- **`gc_interface` de realmemcard** está bloqueada por `asd2`, que **no existe
  en `configure.py`**: es un objeto sólo-extraído que referencia
  `__static_initialization_and_destruction_0_803906C0`.
- **`snddrv`, `sdspmix`, `satospkr`, `sstvol`, `ssysinit`, `scrsfl`**: racimos de
  snd cuyo eslabón que falta es siempre una `.rodata`/`.bss` de más → `splits.txt`.

## 7. Dos cosas que hay que decidir fuera de este encargo

1. **`scripts/audit.py` da un FALSO FALLA con el racimo A.**
   `PostProcMachineSpecificConfig` sale
   `FALLA: reubicacion a otro simbolo: gcc2_compiled._8035645C contra SetupBoundingValueArray_Generic`.
   Las dos direcciones son la misma (`symbols.txt:16493-16494` pone las dos en
   `0x8035645C`), pero `DirsMW.__missing__` sólo resuelve los `@N_ADDR` de
   Metrowerks, no los `gcc2_compiled._ADDR` del troceador, así que
   `mismo_destino` sale falso. **El DOL byte a byte idéntico con la unidad
   promocionada es la prueba de que la reubicación es correcta.** El parche
   sería una línea en `DirsMW.__missing__`: aceptar también
   `^gcc2_compiled\._([0-9A-Fa-f]{8})$`. **No lo he aplicado**: es una
   herramienta compartida por ocho agentes.
2. **`src/Speed/Indep/Libs/realcore/…/file/gc/hd_device.cpp` tiene ensamblador
   escrito a mano**: el destructor `_._20GcHdFileDeviceDriver` está emitido con
   un bloque `asm(".text … stwu 1,-8(1) …")`, y hay otro `asm` que fabrica
   `gap_07_804D5014_bss`. Es exactamente lo que el brief prohíbe. No lo he
   tocado; la unidad (820 B) tampoco promociona: su `__static_initialization_and_destruction_0`
   está en `.text+616` y el original lo tiene en `.text+548`.

## 8. Las líneas exactas de `configure.py` (para aplicarlo)

Las 25, ya localizadas. Sólo hay que cambiar `NonMatching` por `Matching` en
esas líneas. **Las cinco del racimo A y el B van todas o ninguna** (marcadas ✳).

| línea | objeto |
|---|---|
| 745 | `egami/rcmp/dev/source/av/cmn/audioplayer.cpp` |
| 770 | `Packages/vp6/1.0.6/source/decode/cmn/allocator.cpp` |
| 776 ✳A | `Packages/vp6/1.0.6/source/decode/gc/doptsystemdependant.c` |
| 777 | `Packages/vp6/1.0.6/source/decode/gc/uoptsystemdependant.c` |
| 783 ✳A | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter.c` |
| 788 | `Packages/vp6/1.0.6/source/decode/cmn/vfwpbdll_if.c` |
| 792 | `Packages/vp6/1.0.6/source/decode/cmn/decodemode.c` |
| 794 ✳A | `Packages/vp6/1.0.6/source/decode/cmn/deblock.c` |
| 795 ✳A | `Packages/vp6/1.0.6/source/decode/cmn/dering.c` |
| 798 | `Packages/vp6/1.0.6/source/decode/cmn/simpledeblocker.c` |
| 896 ✳B | `Speed/Indep/Libs/snd/9/source/library/cmn/stpparse.c` |
| 899 ✳B | `Speed/Indep/Libs/snd/9/source/library/cmn/svecreal.cpp` |
| 902 | `Speed/Indep/Libs/snd/9/source/library/mix/sx87d16.c` |
| 946 | `Speed/Indep/Libs/snd/9/source/library/mix/smixc.c` |
| 950 | `Speed/Indep/Libs/snd/9/source/library/mix/smixptch.c` |
| 988 ✳B | `Speed/Indep/Libs/snd/9/source/library/cmn/sexithndl.c` |
| 1026 | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathevent.cpp` |
| 1060 | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank.c` |
| 1061 | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchevnt.c` |
| 1066 | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchrule.c` |
| 1071 | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchcsis.cpp` |
| 1131 | `Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/memvectors.cpp` |
| 1211 | `Speed/Indep/Libs/realcore/6.24.00/source/system/debug/cmn/abortmsg.cpp` |
| 1379 | `LibSN/syscalls.c` |
| 1254 | `Speed/Indep/Libs/realcore/6.24.00/source/debug/cmn/debugger.cpp` |
| 1258 | `Speed/Indep/Libs/realcore/6.24.00/source/debug/gc/printdrv.cpp` |
| 1428 | `model.cpp` |

`spchcsis` está en el conjunto `VETADAS` de `scripts/promote.py` y **ya no lo
merece**: enlaza y el DOL sale idéntico, suelta y en grupo. Conviene sacarla de
esa lista para que no siga saliendo bloqueada.

## 9. «Exportamos símbolos de más»: la premisa del encargo era falsa a medias

Los 34 casos se parten en dos, y lo he comprobado buscando cada símbolo en
**todos** los objetos extraídos:

- **Las vtables SÍ existen en el DOL**, dentro de objetos comodín
  (`_vt.Q29RealInput8GcEffect` → `auto_05_804147EC_rodata`,
  `_vt.Q26Realmc8GCDriver` → `auto_05_80413958_rodata`,
  `_vt.Q24Csis24IAllocatorToICoreAdaptor` → `auto_05_804130A0_rodata`).
  Eso es **`splits.txt`**, no fuente.
- **Los `operator new`/`delete` de clase NO existen en ninguna parte del DOL**
  (`__nw__Q26Realmc8GCDriverUi`, `__dl__Q29RealInput8GcEffectPvUi`,
  `__dl__Q24RCMP7DECODERPv`: **cero** apariciones en los 1.200 objetos
  extraídos). Ésos sí los emitimos de más, y explican que nuestro `.text` mida
  116-192 B más que el extraído en `gc_effect`, `gc_driver`, `gc_pad`,
  `interfaceimp`, `eventqueue`, `effectimp`, `gc_device`, `rcmpbase`, `csis`…

**Pero quitar los operadores NO desbloquea ninguna unidad**: todas esas unidades
llevan ADEMÁS la vtable en `.rodata` que el comodín ya tiene. Por eso no he
tocado ninguna: sería trabajo con cero ficheros de rendimiento hasta que se
arregle `splits.txt`.

## 10. Lo que NO he probado

- **Los 84 «secciones que emitimos de más» uno por uno.** Sólo he clasificado el
  contenido (pool contra cadenas) y localizado la dirección de los ocho más
  gordos. Los `libc/*_pow`, `*_rem_pio2`, `sf_*` (una veintena) emiten `.sdata`
  y `.sdata2` de más y no los he mirado con detalle: son el mismo patrón.
- **Quitar los `operator new`/`delete` de clase** en realcore, realmemcard, csis
  y rcmp. Demostré que el DOL no los tiene, pero también que la unidad seguiría
  bloqueada por la vtable, así que no toqué nada.
- **`scrsfl`** (196 B): sólo hace falta cambiar la línea 8 de
  `src/…/snd/9/source/library/mix/sfrsf.c`
  (`extern "C" RSFFN rsflc_local __asm__("gcc2_compiled._80372D10")`) por el
  nombre real de la primera función de `scrsfl`. **No lo hice porque otro agente
  está trabajando en `sfrsf` en esta misma ronda** (el commit `7620f8e8` es suyo).
  Es la misma cura del racimo A y debería cerrar las dos.
- **`uoptsystemdependant`** ya promociona, pero su `gcc2_compiled._8034E180`
  apunta a `vputil`, que **define ese símbolo con `asm()` a mano**
  (`src/Packages/vp6/…/cmn/vputil.c:50-52`). Si algún día se promociona `vputil`
  habrá que disolver ese alias igual que en el racimo A.
- **`hd_device`, `TokenEntropy`, `sst3dpos`, `quantize`, `postproc`,
  `gc_memcard_interface_impl`**: diagnosticadas, no intentadas.
- **`libgcc2_*`, `kf_tan`, `sf_atan`**: `sf_atan` y `kf_tan` fallan con
  `L1001: Failed to create small data reference to address 0x80439f4c`, un
  desbordamiento del rango de `_SDA_BASE_` — problema distinto, sin mirar.
- **No he vuelto a medir el proyecto entero**: sólo `vp6`, `path`, `snd` y
  `realcore` (las cuatro bibliotecas que toqué), y las cuatro dan
  `+0 B, +0 funciones, 0 unidades cambian`.

### Comprobación (índice completo de símbolos, no `grep`)

`c22link_index.py` construye el mapa símbolo → objeto de **los 1.200 objetos
extraídos** (37.945 símbolos definidos) y de los nuestros (26.463). Con él,
las dos afirmaciones del §9 quedan probadas, no inferidas:

- **10 de 10** `operator new`/`delete`/`new[]`/`delete[]` de clase muestreados
  (`__nw__Q26Realmc8GCDriverUi`, `__dl__Q29RealInput8GcEffectPvUi`,
  `__dl__Q24RCMP7DECODERPv`, `__dl__Q26Attrib8DatabasePvUi`…) **no los define
  NINGÚN objeto extraído**. Son código nuestro que el DOL no tiene.
- **6 de 6** vtables muestreadas las define **sólo un comodín**:
  `_vt.Q26Realmc8GCDriver` y `_vt.Q26Realmc11GCInterface` →
  `auto_05_804130A0_rodata` / `auto_05_80413958_rodata`;
  `_vt.Q29RealInput8GcEffect` → `auto_05_804147EC_rodata`;
  `_vt.Q24Path9PathToSnd`, `_vt.11PathToReal6` y
  `_vt.Q24Csis24IAllocatorToICoreAdaptor` → `auto_05_804130A0_rodata`.
