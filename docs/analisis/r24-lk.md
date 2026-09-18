# Ronda 24 — el frente de `linked`

Base medida al empezar (`report.json` de las 19:33, `main.dol: OK`):
**`linked` 10,0710 % — 406/590 unidades**. El universo real de candidatas
(código al 100 %, con objeto nuestro y presentes en el enlace) son **81
unidades, 543.964 B**.

**Barrido de partida**: `trypromo.py` sobre las **81** con los objetos recién
construidos → **cero verdes**. Ninguna estaba promocionable «por accidente»:
todo lo que sigue es trabajo de fuente de esta ronda.

## 1. Resultado: 11 unidades promocionables, +11 ficheros, 7.088 B

**`linked` 10,0710 % → ~10,2506 % (+0,18 pp), 406 → 417 de 590.**

Las 11 dan **DOL byte a byte idéntico** — cada una suelta **y las 11 juntas** —
sobre objetos **borrados y reconstruidos** antes de la pasada final.
`audit.py`: **0 FALLA** en las 11 (dos pasadas). `measure.py` sobre las 19
unidades tocadas: **10.828/10.828 B, 100 %**; `--cmp` sobre las 13 de `input/`
(la cabecera compartida): **+0 B, ninguna unidad baja**.

### Las líneas de `configure.py` — cambiar `NonMatching` por `Matching`

| línea | objeto | B código | qué hizo falta |
|---|---|---|---|
| 1162 | `…/realcore/…/input/gc/gc_pad.cpp` | 1.620 | ✳I `#pragma interface` + `GetEffect` fuera de línea + reordenar `CreateEffect` |
| 1294 | `Packages/realmemcard/…/cmn/memcard_utilities.cpp` | 1.012 | cadena `"&\|!~^"` a `extern char lbl_80414400[]` |
| 1146 | `…/realcore/…/input/cmn/interfaceimp.cpp` | 988 | ✳I `#pragma interface` + `GetPad/GetMouse/GetKeyboard` fuera de línea |
| 864 | `…/snd/9/…/cmn/sst3dpos.c` | 692 | quitar un `extern` que **nadie definía** (`lbl_804128BC`) |
| 1134 | `…/realcore/…/input/cmn/eventqueue.cpp` | 608 | ✳I `#pragma interface` |
| 1154 | `…/realcore/…/input/gc/gc_effect.cpp` | 608 | ✳I `#pragma interface` |
| 839 | `…/snd/9/…/cmn/sndfxbus.cpp` | 528 | alineación 8 de `.rodata` |
| 2118 | `libc/sf_atan.c` | 528 | `atanhi/atanlo/aT` a `extern` con el nombre del troceador |
| 1138 | `…/realcore/…/input/cmn/effectimp.cpp` | 292 | ✳I `#pragma interface` |
| 1122 | `…/realcore/…/input/cmn/effect.cpp` | 108 | ✳I `#pragma interface` |
| 1118 | `…/realcore/…/input/cmn/event.cpp` | 104 | ✳I `#pragma interface` |

`✳I` = el racimo de `realcore/input`: **una sola cabecera** los abre a todos.

En 1118, 1122, 1134, 1138, 1146, 1154, 1162 y **1294** el `NonMatching` está
**en su propia línea** (la llamada a `Object(` va partida). En 839, 864 y 2118
va dentro del `Object(...)` de una sola línea.

**Nota para 839**: el comentario de `configure.py` (líneas 836-838) dice que
`sndfxbus` «compila a 0x25c frente a los 0x210 del original y desplaza todo el
`.text` posterior». Eso ya no es cierto: los 76 B de más son
`SetOutputLevel__Q23Snd17GlobalFxProcessorf`, que **nadie llama** y que el
enlazador retira; el DOL sale idéntico. Conviene reescribir el comentario.

## 2. El racimo de `realcore/input`: `#pragma interface`, y 7 de 10

Las diez unidades daban el mismo hash roto `0543d0562f81`. La causa medida:
**las once vtables `_vt.Q29RealInput*` viven TODAS en el comodín
`auto_05_804147EC_rodata`** (volcado completo: 2.408 B, con las 11 vtables de
RealInput más 3 de Realmc y un montón de cadenas), y nuestros objetos las
emitían otra vez.

`#pragma interface` en **`input.h` y en las ocho cabeceras privadas**
(`deviceimp.h`, `effectimp.h`, `eventqueue.h`, `interfaceimp.h`, `gc_device.h`,
`gc_effect.h`, `gc_interface.h`, `gc_pad.h`) hace que ninguna se emita.
Con eso `event`, `effect`, `eventqueue`, `effectimp` y `gc_effect` quedan
**idénticos en secciones y símbolos** y dan DOL OK a la primera.

### 2.1 Lo que el pragma se lleva de más, y cómo devolverlo

El comodín **referencia** los virtuales inline como símbolos externos
(`GetPad__Q29RealInput12InterfaceImp`, `Acquire__Q29RealInput9DeviceImp`,
`GetEffect__Q29RealInput5GcPad`, `_._Q29RealInput9Interface`…), así que alguien
tiene que definirlos: con el pragma dejan de emitirse y el enlace **falla**
(`L0039` desde `auto_05_804147EC_rodata`).

La cura es de fuente y **no cuesta un byte**: sacar de la clase, al `.cpp` y **al
final**, exactamente esos virtuales inline. GCC 2.9 emite las copias de inline
pendientes al final de la TU, así que el desplazamiento coincide solo.

- `interfaceimp`: `GetPad`/`GetMouse`/`GetKeyboard` (3 × 8 B, `.text+964/972/980`).
- `gc_pad`: `GetEffect` (8 B, `.text+1612`) **y reordenar** `CreateEffect`
  detrás de `Update` (el objetivo va ctor, dtor, `Update`, `CreateEffect`,
  `GetEffect`).

### 2.2 `#pragma implementation` con nombre: cómo no perder porcentaje

`#pragma interface` en `input.h` deja a `interface.cpp` sin
`_._Q29RealInput9Interface` (−52 B) y el pragma de `deviceimp.h` deja a
`gc_device` sin sus cuatro inline de `DeviceImp` (−108 B). **Las dos pérdidas se
recuperan con `#pragma implementation`, pero sólo si el nombre casa**:

- `#pragma implementation "../../../include/common/realcore/input.h"` **no
  funciona** (GCC compara la cadena del `implementation` con la que registró el
  `interface`, y ésa es la ruta con la que se abrió el fichero).
- `#pragma interface "input.h"` + `#pragma implementation "input.h"` **sí**.

Con eso las 13 unidades de `input/` siguen a **6.544/6.544 B, 100 %** y
`measure.py --cmp` da **+0 B, 0 unidades cambian**.

`gc_device.cpp` lleva `#pragma implementation "deviceimp.h"` por lo mismo, más
el reorden (ctor, `~GcDevice`, `GetPortNum`, `SetPortNum`), que quita tres
símbolos «movidos».

**Ojo con la dirección del intento**: probar `~Interface()` fuera de línea (en
vez del `#pragma implementation`) devuelve los 52 B de `interface` pero **rompe
`interfaceimp`**: su destructor de 168 B deja de casar porque el objetivo
**inlinea** el cuerpo vacío de la base en vez de llamarlo (verificado en el asm:
`_._Q29RealInput12InterfaceImp` guarda `_vt.Q29RealInput9Interface` y llama a
`FreeMemSize`, sin `bl _._Q29RealInput9Interface`). Veda anotada.

### 2.3 Las tres que NO cierran, y por qué (con dueño único comprobado)

| unidad | B | qué falta | dueño único |
|---|---|---|---|
| `gc_device` | 284 | `.rodata` 160 B: `_vt.Q29RealInput8GcDevice` (0x80414C40) + `_vt.Q29RealInput9DeviceImp` (0x80414C90) | **sí**: los dos símbolos sólo los referencia `gc_device.s` |
| `interface` | 204 | `.rodata` 80 B: `_vt.Q29RealInput9Interface` (0x80414BF0); y `.sdata` 8 B contra 4 (alineación) | lo referencian `interface.s` e `interfaceimp.s`; lo define el comodín |
| `gc_interface` | 1.008 | `.bss` 776 contra 48: faltan `FatalParam` (12 B) y `FatalContext` (712 B), **locales y sin referencias** en el objetivo; sobra `_9RealInput.gResetBit` en `.sbss` | — |

**La forma exacta del original de `gc_device` está identificada y medida**:
`#pragma interface` en `deviceimp.h` y `gc_device.h`, `#pragma implementation`
de las dos en el `.cpp` y **`-fno-implement-inlines`** dan
`.text` de **284 B con los ocho símbolos en el desplazamiento exacto del
extraído** y `.rodata` de 160 B = `_vt.GcDevice`(offset 0) + `_vt.DeviceImp`(80),
que es **exactamente** la disposición del DOL desde 0x80414C40. Es la misma
condición `DECL_THIS_INLINE && !flag_implement_inlines && !DECL_VINDEX` que la
r23 identificó en `hd_device`: **dos hallazgos independientes apuntando a que
`realcore` se compiló con `-fno-implement-inlines`**. No lo propongo porque sin
tocar `splits.txt` no promociona nada, y es un cambio de cflags de toda la
biblioteca (aunque su radio real es mínimo: el flag **sólo** hace algo en TUs
que ven un `#pragma interface`/`implementation`, y en `realcore` eso son hoy
`hd_device.cpp` y `gc_device.cpp`).

## 3. Los otros cuatro cierres

### 3.1 `sst3dpos` — un `extern` que nadie definía

`sst3dpos.c` declaraba `extern const float lbl_804128BC;` y lo usaba en cuatro
sitios. **Nadie define ese símbolo**: `lbl_804128BC` es la propia `.rodata` de
`sst3dpos` (4 B, `-1000000.0f`), y el comodín `auto_05_804128B0_rodata` acaba
justo antes. Al promocionar, `L0039` seis veces.

Quitando la declaración y volviendo al literal `-1000000.0f` (que ya estaba en
dos sitios del mismo fichero): **100 %, DOL OK**. Es un `extern` de más de una
ronda anterior, no un problema de enlace. Regla que vale la pena barrer:
un `extern` a un `lbl_` **que define el propio objeto extraído de esa unidad**
es siempre un error.

### 3.2 `sndfxbus` — la alineación de `.rodata`, sin objeto donde colgarla

`.rodata` extraída 16 B **alineada a 8**; la nuestra 12 B alineada a 4, y el
enlace se corre 8 B (5.475 bytes distintos en el DOL, todos `@l` desplazados;
ninguna sección cambia de tamaño). Medido y **descartado**: dejarla en 16 B con
alineación 4 (añadiendo un objeto de 4 B al final) **no basta** — sigue el mismo
hash roto. Lo que decide es la **alineación**, no el tamaño.

Como en esa sección no hay ningún objeto con nombre (sólo `$LC` del pool), la
alineación se consigue con una constante de 8 B alineada que **el enlazador
retira** (`-strip-unused-data`), y el DOL byte a byte idéntico lo demuestra. Es
un apaño y lo digo como tal: la forma limpia sería lo que en el original daba
`.rodata` alineada a 8, y no la he identificado.

Los 76 B de `.text` de más (`SetOutputLevel`, sin llamantes) **no molestan**: el
enlazador se los come. El `TODO` del fuente («¿de verdad conservamos rodata de
una función eliminada?») queda contestado: **sí**, y es correcto.

### 3.3 `sf_atan` — el `L1001` no era un problema de rango SDA

`sf_atan` y `kf_tan` fallaban con
`L1001: Failed to create small data reference to address 0x80439f4c`. La causa
no es el rango de `_SDA_BASE_`: el fuente **definía** `atanhi[4]`, `atanlo[4]` y
`aT[11]` (76 B) con `static … __asm__("atanhi")`, y con `-G8` esos tamaños se van
a **`.data`** (0x80439F4C), fuera de la ventana SDA; la referencia sí sale
`@sda21` porque la declaración miente el tamaño a `[2]` a propósito.

En el objetivo esos tres viven en **`.sdata2`** (0x805003A8/B8/C8) dentro del
comodín `auto_10_80500390_sdata2`, y `sf_atan.o` **no los define**. Con
`extern const float atanhi[2] __asm__("atanhi_805003A8")` (y sus dos hermanos)
la unidad sigue al 100 % y da **DOL OK**.

`kf_tan` necesitaba lo mismo con `T_805003F8` **y** convertir sus cinco
`SDA_FLOAT` a `SDA_FLOAT_X` (`c23lk_sda.py` los empareja solo). Hecho: le queda
`.sdata` de **8 B** que es la constante de sesgo (§5.1).

### 3.4 `memcard_utilities` — 8 B de cadena

`.rodata` de 8 B (`"&|!~^"`), que en el objetivo es `lbl_80414400`, del comodín
`auto_05_804130A0_rodata`. `extern char lbl_80414400[] __asm__("lbl_80414400")`
en vez del literal → **DOL OK**. Dos trampas: `strrstr` toma `char*` (no
`const char*`), y la declaración cae dentro de `namespace RealmcUtils`, así que
**hace falta el `__asm__`** o el manglado la convierte en
`_11RealmcUtils.lbl_80414400` y el enlace falla.

## 4. Correcciones reales que NO cierran (se quedan: +0 B, nada baja)

- **`sserver`** (992 B): el `static unsigned int lastTick` de función no lo
  define el objetivo — lo referencia como `lastTick.126_804502F4`, de un comodín
  de `.bss` (mecanismo de `pathaction`, r23 §3.4). Sacado a `extern … __asm__`,
  la `.bss` baja de 16 a 12 B y la unidad sigue al 100 %. No promociona por §6.
- **`kf_tan`** (532 B): cinco `SDA_FLOAT` → `SDA_FLOAT_X`, `.sdata` de 32 a 8 B.

## 5. Lo que sólo se arregla en `splits.txt` (VEDADO), con dueño único

Comprobación de dueño único: buscar **quién referencia** cada símbolo en los
`.s` de `build/GOWE69/asm/` (comodines incluidos). Si sólo lo referencia una
unidad, y sus bytes coinciden con los que nosotros emitimos, el dueño es ésa.

### 5.1 La familia del sesgo `int→double` de la libm de SN — dueño único comprobado

El sesgo `0x4330000080000000` lo genera el compilador y no sale de ningún
literal: no hay forma de fuente. Pero **el bloque entero de `.sdata` de estas
unidades es byte a byte el nuestro y lo referencia una sola unidad**:

| unidad | rango de `.sdata` | tamaño | comodín que lo tiene hoy | dueño único |
|---|---|---|---|---|
| `libc/kf_tan` | 0x804FF348..0x804FF350 | 8 B | `auto_08_804FF330_sdata` | **sí**: sólo `kf_tan.s` |
| `libc/sf_log` | 0x804FF120..0x804FF128 | 8 B | `auto_08_804FF0B8/0E0_sdata` | **sí**: sólo `sf_log.s` |
| `libc/sf_exp` | 0x804FF0C8..0x804FF110 | 72 B | `auto_08_804FF0B8_sdata` | **sí**: sólo `sf_exp.s` |
| `libc/sf_expm1` | 0x804FF288..0x804FF2EC | 104 B | `auto_08_804FF288_sdata` | **sí**: sólo `sf_expm1.s` |

En `sf_exp` he comprobado además que **los 72 bytes son idénticos**:
`00000000 42b17180 7f800000 c2cff1b5 3fb8aa3b 00000000 4330000080000000 7149f2ca
3f800000 …` en nuestro objeto y en el DOL desde 0x804FF0C8.

Con `kf_tan` y `sf_log` bastan **8 B por unidad**: las demás constantes ya están
convertidas a `SDA_FLOAT_X`.

Mismo patrón y misma causa, sin comprobar uno a uno: `stagpat` (`.rodata` 16 B),
`svol` (16 B: `lbl_804129F0` = sesgo + `3c010204`), `pathinit` (32 B, **dos**
sesgos), `pathvol`, `bigyuvswizzler` (80 B), `libgcc2_4`, `e_pow` (272+48),
`ef_pow` (136+24), `e_exp` (120+48), `k_rem_pio2` (64+136),
`kf_rem_pio2` (40+84), `e_rem_pio2` (88+392), `ef_rem_pio2` (56+920),
`math_support` (224+152).

### 5.2 Las vtables de `realcore/input` — dueño único comprobado

`_vt.Q29RealInput8GcDevice` (0x80414C40, 80 B) y `_vt.Q29RealInput9DeviceImp`
(0x80414C90, 80 B): **sólo `gc_device.s` los referencia**, y con
`-fno-implement-inlines` nuestro objeto los emite en ese orden exacto (§2.3).
`_vt.Q29RealInput5GcPad` sólo lo referencia `gc_pad.s`.

### 5.3 `sstvol` — el caso INVERSO: `splits.txt` le asigna 440 B de MÁS

`sstvol` es el único de la ronda donde el extraído tiene **más** datos que
nosotros: `.rodata` 480 B contra 40. Los 40 son suyos
(`_3Snd.gChannelToVoiceIndexLut` 36 B + un `$LC` de 4 B). Los otros 440 son **el
pool de otras cuatro unidades**, y cada trozo tiene su propio dueño:

| trozo | tamaño | quién lo referencia |
|---|---|---|
| `lbl_8041292C` | 136 B | sólo `sstvol.s` (dato propio, sin reclamar) |
| `lbl_804129B4` | 28 B | `ssys.s` |
| `lbl_804129D0/D4/D8` | 12 B | `ssysinit.s` |
| `lbl_804129E0/E8/EC` | 16 B | `stagpat.s` |
| `lbl_804129F0/F8` | 12 B | `svol.s` |
| `lbl_804129FC` | 236 B | `svol.s` |

El arreglo es **recortar** el rango de `sstvol` a
`.rodata start:0x80412908 end:0x80412930` y repartir el resto. Mientras no se
haga, `sstvol` no puede promocionar por fuente: tendríamos que **escribir los
440 B de otras unidades** dentro de `sstvol.c`.

### 5.4 `satospkr` — bloqueada por `stagpat`, no por sí misma

`satospkr` (1.796 B) tiene secciones y tamaños **idénticos**; sólo cambian los
nombres de su pool (`$LCn` contra `lbl_80412C7x`), que r23 §4 ya dijo que no
importan… salvo uno: **`lbl_80412CC2` sí lo referencian otras unidades**, y es
`_3Snd.gAzimuthFoldDownLut − 6` (el ADDEND plegado de `lut[x][n-1]`, r23 §3.2:
`gAzimuthFoldDownLut` está en 0x80412CC8 y el símbolo cae dentro del relleno
final de la `.rodata` de `satospkr`).

Medido: con `sst3dpos` promocionada sólo queda `stagpat.o` reclamándolo, y
`satospkr + sst3dpos + stagpat` **enlaza** pero da `DOL ROTO 9e7dee55273d`
(porque `stagpat` es del §5.1). Es decir: **`satospkr` cierra el día que cierre
`stagpat`**, y `stagpat` necesita 16 B de `splits.txt`.

## 6. Muros de fuente identificados (no son `splits.txt`)

- **`sserver`** (992 B): (a) el orden de `.bss` — el objetivo tiene
  `gVariableTimerList, Tick, Period` y GCC nos da siempre
  `List, Period, Tick` **sea cual sea el orden del fuente** (probado en los dos
  sentidos y con `__attribute__((section(".bss")))` sobre `Tick`: las tres
  variantes dan lo mismo, porque las dos salen por `.lcomm` al final del `.s`);
  (b) 4 B de `.rodata` (`0.0f` = `lbl_80412880`, de un comodín): la forma
  `extern const float lbl_80412880[]` con `lbl_80412880[0]` **baja `measure` de
  100 % a 38,7 %** (se lleva `SNDSYSI_100hzserver`, 608 B). **Veda anotada.**
- **`bigswizzler`**: veda de r23 confirmada, no reintentada.
- **`public`/`tasks` de realmemcard** (racimo `fc9e604fe105`): `public` (5.024 B)
  emite 28 B de `.rodata` que son cinco literales (`"s"`, `"ss"`, `"sss"`,
  `"ssss"`, `"ssdd"`) de las funciones **inline de una cabecera compartida**
  (`gc_interface_impl.h`, los `LC_msg`), y en el objetivo son `lbl_80414790`,
  del comodín `auto_05_804130A0_rodata`, con los mismos 28 bytes exactos.
  `public.cpp` **ya** usa `extern "C" char lbl_80414790[]` para el primero. La
  cura sería parametrizar el símbolo por TU (patrón `LIBGCC2_CLZ_TAB_SYM` de
  r23), porque las otras cinco unidades de realmemcard tienen **su propia copia
  en otra dirección**. **No lo he intentado.**

## 7. Lo que NO he probado (por orden de lo que yo atacaría)

1. **`pathsnd`** (8.452 B) y **`gc_driver`** (8.952 B): `.rodata` de 1.128 y
   184 B, que r22 identificó como **vtables**. Es la misma firma que
   `realcore/input`, así que **`#pragma interface` en sus cabeceras** es la
   extrapolación más prometedora que queda. No me dio tiempo.
2. **`public`** (5.024 B) y el resto del racimo de realmemcard: la
   parametrización del símbolo de las cinco cadenas (§6).
3. **`csis`** (5.728 B), **`rcmp_vp6_codec`** (2.828), **`rcmp_mad_codec`**
   (2.592), **`gc_blockcalculator`** (312): todas con `.rodata` de más (vtables)
   **y** `.text` de más (los `operator new`/`delete` de clase). Firma exacta de
   `#pragma interface`. En `csis` el pool es una cadena (`"CsisAlloc"`,
   `lbl_804130A0`) más 158 B sin reclamar y 28 B de `.data`.

   **`rcmpbase` (1.184 B) SÍ lo he probado, y merece la pena contarlo**: sus
   clases están definidas **dentro del `.cpp`**, y ahí `#pragma interface`
   **funciona igual** (`interface_strcmp` devuelve 1 mientras no haya un
   `#pragma implementation` que case). Con una línea al principio del fichero
   el `.text` baja de 1.260 a 1.132 B con **los quince símbolos en el
   desplazamiento exacto del extraído hasta 960**, y las dos vtables
   (`_vt.Q24RCMP7DECODER`, `_vt.Q24RCMP11RCMP_SYSTEM`) desaparecen de `.rodata`.
   Lo que se lleva de más es `_._Q24RCMP11RCMP_SYSTEM` (52 B), que el objetivo
   emite **en la zona de inline pendientes** (1044, entre
   `__static_initialization_and_destruction_0` y `_GLOBAL_.I.`), así que
   definirlo fuera de línea en el `.cpp` no lo pone ahí. Mismo muro que
   `gc_device` y `interface` (§2.3). **Revertido** (deja `measure` en 1.132 de
   1.184). Queda además `.rodata` de 16 B ($LC0 = `"RCMP::CHUNK"`, $LC1 =
   `lbl_80410154`) y `.bss` con alineación 4 contra 8 (16 B, múltiplo: gratis).
4. **`avplayer`** (4.224 B): 72 B de `.rodata` que son **cadenas** con nombre
   (`lbl_8040FEFC` = `"AV::VideoStreamBuffer"`, `lbl_8040FF14` =
   `"AV::AudioStreamBuffer"`) más dos dobles y el sesgo `4330000000000000`. Las
   cadenas se curan como `memcard_utilities`; el sesgo, no.
5. **`vfprintf`** (6.948) y **`vfprintf_1`** (5.972): `.bss` 128 + `.rodata`
   144/108 + `.sdata` 8. Sin mirar.
6. **`itoa`** (1.548): `.bss` 164 + `.rodata` 64. Sin mirar.
7. **`quantize`** (896): le **faltan** 40 B de `.rodata` (1.320 contra 1.280) y
   sobra `.sbss` 4. Caso inverso, sin diagnosticar.
8. **`pathreal`** (496) y **`pathreal6`** (916): 240 y 144 B de `.rodata`. La
   divergencia de reubicación de r23 §3.2 en `pathreal` sigue sin resolver.
9. **`gc_interface`** de `realcore/input` (1.008 B): los 724 B de `.bss` locales
   (`FatalParam`, `FatalContext`) que el objetivo tiene y nosotros no.
10. **`mbtowc_r`** (796): `.data` 864 B de más. **`strtod2`** (812): `.data` 72.
11. **Las SourceLists**: confirmo por tercera vez el veredicto — `zSim` mide
    114.712 B de `.text` contra 96.400, `zLua` 113.172 contra 94.264, `zMisc`
    86.368 contra 78.008, `zAnim` 51.368 contra 42.292. No es enlace.

## 8. Cosas para decidir fuera de este encargo

1. **`public.cpp` (realmemcard) tiene ensamblador escrito a mano**: un bloque
   `asm(".section .bss …")` en las líneas 188-196 que fabrica datos. No lo he
   tocado (la unidad no promociona), pero es lo mismo que se retiró en
   `hd_device` y lo que sigue en `sndvd`.
2. **`-fno-implement-inlines` para `realcore`** (§2.3): segunda evidencia
   independiente de que la biblioteca original lo usaba. Radio mínimo (sólo TUs
   con `#pragma interface`/`implementation`), y con él `gc_device` reproduce el
   objeto extraído al byte salvo la `.rodata` de `splits.txt`.
3. `scripts/audit.py` lo ha modificado otro agente durante la ronda; mis 13
   auditorías (0 FALLA) se hicieron con la versión que había en el árbol.

## 9. Herramientas (scratchpad, prefijo `c24lk_`)

| script | qué hace |
|---|---|
| `c24lk_link.py` | como `trypromo.py` pero **imprime el error de enlace completo**: el de `trypromo` se corta a 90 caracteres y ahí es justo donde está el nombre del símbolo que falta |
| `c24lk_scan.py` | pasa `c24lk_link.py` por las 81 candidatas |
| `c24lk_secs.py` | tabla rápida **tamaño + alineación** de cada sección, extraído contra nuestro, para las 81. Es lo que hace visible que varias unidades sólo difieren en la ALINEACIÓN |

Heredadas y reutilizadas: `c22link_full.py`, `c22link_at.py` (ojo: su segundo
argumento es `ext`/`nuestro`/`both`, y con cualquier otra cosa **no imprime
nada, sin error**), `c22link_cc.py`, `c23lk_cand.py`, `c23lk_pool.py`,
`c23lk_sda.py`, `c23lk_doldiff.py` (a éste le he puesto el PID en el directorio
temporal, que era compartido entre agentes).

## 10. Ficheros de fuente tocados

`input.h`, `deviceimp.h`, `effectimp.h`, `eventqueue.h`, `interfaceimp.h` (+`.cpp`),
`gc_device.h` (+`.cpp`), `gc_effect.h`, `gc_interface.h`, `gc_pad.h` (+`.cpp`),
`interface.cpp` — todos en `realcore/6.24.00`; `sst3dpos.c`, `sndfxbus.cpp`,
`sserver.c` en `snd/9`; `sf_atan.c` y `kf_tan.c` en `libc`;
`memcard_utilities.cpp` en `realmemcard`.

**Cabecera compartida**: `input.h` la incluyen 13 unidades (las de
`realcore/…/input/`, ni una más — comprobado por `grep` de la ruta del
`#include`), y las 13 miden **6.544/6.544 B, 100 %** antes y después:
`measure.py --cmp` da **+0 B, 0 unidades cambian**.

Las 13 unidades cerradas o corregidas quedan congeladas con `frozen.py cong`.
