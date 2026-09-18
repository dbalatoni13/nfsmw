# Ronda 26 — el frente de `linked`

Base al empezar: **`linked` 10,8773 % — 429/590 unidades**, `main.dol: OK`.
Universo de candidatas (código al 100 %, con objeto nuestro y en el enlace):
**59 unidades, 583.608 B** (`c23lk_cand.py`).

**Barrido de partida** sobre las 59 con `splits.txt` tal cual: **cero verdes**.

---

## 1. Resultado

**2 unidades, 1.404 B** — `systask` (508 B) e `interfaceimp` de realmemcard (896 B),
**sueltas Y juntas**, sobre objetos **borrados y reconstruidos**, DOL byte a byte
idéntico. `linked` 429 → **431 de 590**.

Pero eso es lo de menos. Lo que trae esta ronda es **el mapa**: el barrido de los
577 `STT_FILE` contra `splits.txt` entero (§2), **28 líneas de `splits.txt`
corregidas y verificadas neutras** (§3), y **la demostración de que la lista
«VEDADO, sólo `splits.txt`» de la r25 §4 SÍ es alcanzable** (§4).

**Nada de esto está aplicado.** `splits.txt`, `keep.lst`, `configure.py` y el
`locale.cpp` del §5 son **propuesta**; el árbol está como lo encontré
(`git diff` vacío en `config/`, `configure.py` y `src/Packages/realmemcard/`).

---

## 2. El barrido de los 577 `STT_FILE` contra `splits.txt`

`c26lk_split.py`. Para cada uno de los 577 símbolos `STT_FILE` del ELF original
agrupa sus locales, decide **de qué unidad de `splits.txt` es realmente** y
compara con **en qué rango cae**.

**La regla que decide el «dueño» del grupo, y que hay que respetar**: manda el
voto de `.text`. Una función estática —o el propio `gcc2_compiled.`— **siempre**
cae en la `.text` de su unidad; un dato puede estar mal atribuido, que es justo
lo que se busca. Con mayoría simple **se pierde un caso** (`gc_memcard_interface_impl`,
que tiene 2 datos mal colocados y sólo 1 símbolo de `.text`): la primera versión
de la herramienta lo daba como que el fichero *era* de `memcard_memvectors`.

### DIRECCIÓN A — el rango de la unidad contiene datos de OTRO fichero

Los 15 casos del árbol entero. Es **la tabla completa**: no hay más.

| unidad (splits) | fichero real (ELF) | símbolos | B |
|---|---|---|---|
| `zFEng.cpp` | **`zFe2.cpp`** | `.data` `unlockType.38610`(252)@8041CE90, `unlockType.38626`(144)@8041CF8C | **396** |
| `libc/vfprintf.c` | `vfprintf.c` (2ª copia = `vfprintf_1`) | `.text` `_vfwrite`@803158AC | 212 |
| `vp6/…/cmn/dering.c` | **`criticalpath.c`** | `.bss` `FData.84`@804BE56C | 144 |
| `realmemcard/…/cmn/locale.cpp` | **`gc_interface.cpp`** | `.bss` `sMsg.608`@804D8DAC | 120 |
| `realmemcard/…/cmn/locale.cpp` | **`gc_driver.cpp`** | `.bss` `statInfo.695`@804D8D40 | 108 |
| `asd2` | `gc_interface.cpp` | `.text` `_GLOBAL_.I./.D._6Realmc.ROOT_DIRECTORY_NAME` | 88 |
| `zMain.cpp` | **`zMisc.cpp`** | `.bss` `sprint_buffer.32547`@8047F8C4 | 80 |
| `realmemcard/…/gc/public.cpp` | `gc_tasks.cpp` | `.bss` `findFileInfo.635`@804D9648 | 32 (ya resuelto, r25 §2.1) |
| `zDebug.cpp` | **`zDynamics.cpp`** | `.bss` `kFloatScaleUp/Down`@8045B100 | 8 |
| `realcore/…/gc/timerthread.cpp` | **`systask.cpp`** | `.sdata` `reentry.39`, `lastsystemtask.46`@804FF638 | 8 |
| `realmemcard/…/cmn/memcard_memvectors.cpp` | **`gc_memcard_interface_impl.cpp`** | `.sdata` `slotA.384`, `slotB.385`@804FF668 | 8 |
| `snd/…/cmn/srandom.c` | **`sserver.c`** | `.data` `lastTick.126`@804502F4 | 4 |
| `snd/…/cmn/stpparse.c` | **`svecreal6.cpp`** | `.data` `systaskadded.168`@8045054C | 4 |
| `snd/…/gc/sdspmix.c` | **`snddrv.c`** | `.bss` `ptr.447`@804C03A0 | 4 |
| `snd/…/cmn/ssysinit.c` | **`stagpat.c`** | `.data` `freekey.126`@80450540 | 1 |

`asd2` es **deliberado** (`configure.py` 2270-2277 lo dice: GOWE69 separa los
envoltorios `_GLOBAL_.I./.D.` de `gc_interface` en su propia unidad). Los otros
14 son errores de reparto.

### DIRECCIÓN B — datos del fichero fuera de sus propios rangos

Es la misma relación vista del revés, más los que caen en **comodín**
(`auto_*`), que son la mayoría del volumen:

| fichero real | dónde caen | n | B |
|---|---|---|---|
| `allsrc.c` | comodín | 20 | 19.245 |
| `zFeOverlay.cpp` | comodín (`.over`) | 12 | 1.832 |
| `spchpick.c` | comodín | 2 | 1.312 |
| `fopen.c` | comodín | 2 | 972 |
| `ef_rem_pio2.c` | comodín | 2 | 920 |
| `mbtowc_r.c` | comodín | 2 | 864 |
| `zFe2.cpp` | **rango de `zFEng.cpp`** | 2 | 396 |
| `e_rem_pio2.c` | comodín | 2 | 392 |
| `vfprintf.c` | comodín | 7 | 369 |
| `libgcc2.c` ×3 | comodín | 1 c/u | 256 c/u |
| … | | | |

**Límite de la herramienta, medido**: el ELF sólo trae **3.832 símbolos locales**
y de ellos **112 en `.rodata`**. Los `$LC` del pool y las cadenas **no tienen
símbolo**, así que el barrido **no ve** el bloque de `.rodata` que le falta a una
unidad. Para eso sigue haciendo falta `c25lk_pool.py` (busca el bloque por
BYTES en el original). Las dos herramientas son complementarias y hay que pasar
las dos.

---

## 3. Las 28 líneas de `splits.txt` — cada grupo verificado por separado

Método: `dtk dol split --no-update` en un **sandbox del scratchpad**
(`c26lk_try/`, con su `config/GOWE69/symbols.txt` y `orig/GOWE69/sys/main.dol`
copiados, porque **dtk resuelve las rutas relativas del `.yml` contra el
directorio del `.yml`**, no contra el CWD — eso me costó tres intentos), y luego
enlace de los 590 objetos con `c26lk_link.py`. **El control (todo extraído) tiene
que dar DOL OK**: es lo que prueba que la línea es un reparto distinto de los
mismos bytes y no un cambio de binario.

| grupo | líneas | control (todo extraído) |
|---|---|---|
| **A** `zFe2`/`zFEng` `.data` 0x8041CE90 → **0x8041D040** | 2 | **DOL OK** |
| **B** `locale` `.bss` end → **0x804D8D40**; `gc_driver` `.bss` **NUEVA** 0x804D8D40..0x804D8DAC; `gc_interface`(rmc) `.bss` start → **0x804D8DAC** | 3 | **DOL OK** |
| **E** `sdspmix`/`snddrv` `.bss` 0x804C03C0 → **0x804C03A0** | 2 | **DOL OK** |
| **F** `ssysinit` `.data` end → **0x80450540**; `stagpat` `.data` **NUEVA** 0x80450540..0x80450544 | 2 | **DOL OK** |
| **F2** `stpparse` `.data` end → **0x8045054C**; `svecreal` `.data` **NUEVA** 0x8045054C..0x80450550 | 2 | **DOL OK** |
| **G** `srandom`/`sserver` `.data` 0x804502F8 → **0x804502F4** | 2 | **DOL OK** |
| **H** `dering` `.bss` end → **0x804BE56C**; `criticalpath` `.bss` **NUEVA** 0x804BE56C..0x804BE5FC | 2 | **DOL OK** |
| **I** `zMain`/`zMisc` `.bss` 0x8047F914 → **0x8047F8C4** | 2 | **DOL OK** |
| **J** `zDebug`/`zDynamics` `.bss` 0x8045B108 → **0x8045B100** | 2 | **DOL OK** |
| **K** `mpegl3base` `.rodata` end → **0x80412AF0**; `snddrv` `.rodata` **NUEVA** 0x80412AF0..0x80412B7C | 2 | **DOL OK** |
| **L** `pathsnd` `.rodata` **NUEVA** 0x80413300..0x80413770 | 1 | **DOL OK** (§4) |
| **M** `pathvol` **0x804138A8..0x80413960**, `avplayer` **0x8040FEFC..0x8040FF40**, `vfprintf` 0x8040FBB8..0x8040FC48, `interfaceimp` 0x804147F0..0x804149E8, todas `.rodata` **NUEVAS** | 4 | **DOL OK** (§4) |
| **N** `memcard_memvectors` `.sdata` end → **0x804FF668**; `gc_memcard_interface_impl` `.sdata` start → **0x804FF668** | 2 | **DOL OK** |

**Y las 28 juntas: DOL OK.** El fichero listo está en
`…/scratchpad/c26lk_try/splits_FIN.txt`; el diff completo contra el actual sale
con `diff config/GOWE69/splits.txt c26lk_try/splits_FIN.txt`.

**Dos correcciones de la r25 §4 estaban 4 B desviadas** y dtk las rechaza con un
mensaje exacto (`ends within symbol`): `pathvol` acaba en **0x80413960**, no en
0x80413958, y `avplayer` empieza en **0x8040FEFC**, no en 0x8040FEF8.

### `keep.lst`: dos líneas, y son OBLIGATORIAS

Mover una frontera **mueve el `gap_` de un objeto a otro**, y `keep.lst` lo
nombra `objeto.o:símbolo`. Si no se actualiza, `-strip-unused-data` se come el
relleno y **todo lo que va detrás se desplaza**:

    -zFEng.o:gap_06_8041D01C_data      +zFe2.o:gap_06_8041D01C_data
    -sdspmix.o:gap_07_804C03A4_bss     +snddrv.o:gap_07_804C03A4_bss

Me costó una hora: sin ellas el grupo A daba `DOL ROTO f1d598d4fea4` y parecía
que la frontera estaba mal. **`c26lk_keep.py` lo hace solo**: recorre los objetos
del reparto nuevo, mira quién define de verdad cada `gap_`/`pad_`/`lbl_` y
reescribe `keep.lst`. Sobre el reparto actual da **0 reasignaciones**, que es el
control de que no inventa.

### `configure.py`: 5 objetos que añadir, 1 que quitar

Carvar un rango de un comodín **parte el comodín en dos** y dtk emite un objeto
nuevo que `configure.py` no conoce (pasa de 590 a 594 objetos). Hay que
añadirlo **detrás de su unidad** en el orden de enlace:

| añadir | detrás de |
|---|---|
| `auto_05_8040FC48_rodata` | `libc/vfprintf.c` |
| `auto_05_8040FF40_rodata` | `avplayer.cpp` |
| `auto_05_80413770_rodata` | `pathsnd.cpp` |
| `auto_05_80413960_rodata` | `pathvol.cpp` |
| `auto_05_804149E8_rodata` | `interfaceimp.cpp` (realmemcard) |

Y **quitar `auto_05_804147EC_rodata`**: sus 4 B (0x804147EC..0x804147F0) pasan a
ser el relleno de alineación de la `.rodata` (al8) de `interfaceimp` y dtk ya no
emite el objeto. Verificado: sin quitarlo el control da `DOL ROTO dd698cb4fca2`;
quitándolo, **DOL OK**.

---

## 4. Lo que la r25 dio por VEDADO **sí se puede**: carvar `.rodata` del comodín

Es el hallazgo más rentable de la ronda. La r25 §4 listó seis unidades
«sólo arreglables en `splits.txt`, dueño único comprobado» y las dejó ahí.
**El mecanismo funciona y está medido**: añadir el rango + el objeto comodín
nuevo en `configure.py` da **DOL OK** con todo extraído, y una de las seis
promociona ya.

| unidad | B | rango `.rodata` añadido | resultado tras el arreglo |
|---|---|---|---|
| **`interfaceimp`** (realmemcard) | **896** | 0x804147F0..0x804149E8 | **PROMOCIONA** |
| `snddrv` | 11.920 | 0x80412AF0..0x80412B7C | falta fuente (§6) |
| `pathsnd` | 8.452 | 0x80413300..0x80413770 | falta fuente (§6) |
| `vfprintf` | 6.948 | 0x8040FBB8..0x8040FC48 | falta `.bss`+`.sdata` (r25 §2.2) |
| `pathvol` | 4.124 | 0x804138A8..0x80413960 | falta fuente |
| `avplayer` | 4.224 | 0x8040FEFC..0x8040FF40 | falta fuente |

**`csis` (5.728 B) es el caso que NO entra así**, y por una razón que conviene
anotar: su rango empieza **exactamente en el principio del comodín**
(0x804130A0), así que carvarlo no parte el comodín en dos sino que lo **renombra**
a `auto_05_80413110_rodata` — y ese comodín está en la posición **437** del
enlace, **antes** de `csis` (439). Para que los datos caigan donde toca habría
que **mover el objeto en el orden de enlace**, no sólo añadirlo. No lo he
probado.

---

## 5. `locale.cpp`: la SÉPTIMA falsificación, y lo que destapa

`src/Packages/realmemcard/3.04.01-layer2/source/lib/cmn/locale.cpp` líneas 16-23
fabrica **228 B que son de otras dos unidades**:

    int statInfo[27] __asm__("statInfo.695_804D8D40");
    int sMsg[30]     __asm__("sMsg.608_804D8DAC");
    __asm__("	.set _Q26Realmc6Locale.statInfo,0
	.set _Q26Realmc6Locale.sMsg,0
    ");

El ELF original lo desmiente sin lugar a dudas: `locale.cpp` tiene en `.bss`
**un solo símbolo**, `_Q26Realmc6Locale.gTrcMsgBuffer` de **10.240 B**
(0x804D6540..0x804D8D40); `statInfo.695` es de `gc_driver.cpp` y `sMsg.608` de
`gc_interface.cpp`. El apaño existía porque `splits.txt` le daba 10.468.

**El parche (va junto con el grupo B de §3, no vale por separado):**

1. borrar las líneas 16-23 (el comentario, las dos definiciones y el `__asm__`);
2. `gTrcMsgBuffer` con **`__attribute__((aligned(32)))`**.

El `aligned(32)` es lo que **abre `systask`**: sin él nuestro `systask.o` mide
320 B de `.bss` (el extraído, 328, incluye el `gap_07_804D6538_bss`) y `locale`
caería en 0x804D6538 en vez de 0x804D6540. Con la `.bss` de `locale` a al32 el
enlazador pone el relleno solo. `10240 % 32 == 0`, así que el tamaño no cambia.

**Prueba de que no toca el código**: la `.text` de `locale.o` es **byte a byte
la misma** (sha1 `5f23d39d60d4`, 1.132 B) en las tres variantes —con
fabricación, sin ella, y sin ella con `aligned(32)`—, y la `.sdata` también.
Sólo cambia `.bss`: 10.468 → **10.240**, que es lo que dice el ELF.

Y **corrige dos malas noticias del encargo**:

- La r25 §4 daba `systask` por «bloqueada hasta que `locale` deje de fabricar los
  228 B». Es exacto, y ya está: **`systask` promociona** (508 B).
- La r25 §6 sacó `reentry`/`lastsystemtask` de `systask.cpp` a `extern
  …__asm__("…_804FF638")` porque `splits.txt` los mete en `timerthread`. **El ELF
  dice que son de `systask.cpp`** (dirección A, 8 B). Como está funciona y no lo
  he tocado; la corrección limpia sería `timerthread` `.sdata` end → 0x804FF638
  y `systask` `.sdata` **nueva** 0x804FF638..0x804FF640, **más revertir el
  `extern` del fuente**. Las dos cosas a la vez o ninguna.

---

## 6. Los racimos: `zFEng`/`zFe2` NO es un racimo, es `splits.txt`

El encargo decía que `zFEng` (71.460 B) está al 100 % y no promociona porque
`zFe2` referencia `unlockType.38610_8041CE90` desde el objeto extraído, y que si
otro agente cierra `zFe2` habría que probar el par junto.

**Es un error de reparto, y el ELF lo dice sin ambigüedad.** Los locales de
`.data` de `zFe2.cpp` llegan hasta **0x8041D01C** (`unlockType.38626` incluido) y
el primero de `zFEng.cpp` está en **0x8041D060** (`PassWrapMode`); en medio,
`FEDirection_Message` (0x8041D040) es de `FEng/FEButtonMap.cpp`, que es de
**zFEng**, y `unlockType` sale de `Frontend/Careers/UnlockSystem.cpp`, que es de
**zFe2**. La frontera está 396 B por debajo de donde toca.

Consecuencias, todas medidas:

- Con el grupo A aplicado, **`zFEng` ya no falla con `L0039`**: pasa a
  `DOL ROTO 2633616507ed`, o sea el bloqueo es suyo, no de `zFe2`.
- **Sin el grupo A, `zFEng` no puede promocionar NUNCA**, ni siquiera con `zFe2`
  cerrada: exigiría que nuestro `zFEng.o` emitiera `unlockType` en su `.data`, y
  eso es de `zFe2`.
- Lo que le queda a `zFEng` es **suyo y es de fuente**, con las tres piezas
  contadas (`c25lk_diag.py`):
  1. `.data` 336 B nuestra contra **308** de la unidad. El total de los tramos
     coincide, **falla el orden**: el objetivo tiene un hueco de 4 B en
     `+56` (`lbl_8041D078`) y otro en `+268` (`lbl_8041D14C`); nosotros ponemos
     `FEngMemoryPoolUnknown` en `+264` y **`eFrameCounter` en `+280`**. Moviendo
     `eFrameCounter` entre `mpDefaultCallback` y `SysGUID` cuadran los 308.
     Sobran además **28 B MUERTOS** (`FEKeyInterpDefault`).
  2. `.rodata` 1.920 B nuestra contra **2.632**: **faltan 712 B** que no
     emitimos.
  3. `.text` 73.948 B nuestra contra **71.460**: **sobran 2.488 B** de código que
     objdiff no compara (por eso da 100 %).

**Y hay un racimo de verdad, nuevo**: `snddrv`. Con el grupo K su pool ya está
asignado, pero el enlace canta
`sfxrevc.c(952): L0039: undefined lbl_80412B78`: **los 4 B de 0x80412B78 (un
`1.0f`) son de `sfxrevc`, que YA está promocionada y no los emite**. Probé
dárselos por `splits.txt` (grupo O: `snddrv` hasta 0x80412B78 y `sfxrevc`
0x80412B78..0x80412B7C) y **rompe el control entero** — precisamente porque
`sfxrevc` usa nuestro objeto. **La cura es de fuente, en `sfxrevc.c:952`.**
Grupo O **revertido y vedado**.

---

## 7. Los otros diagnósticos exactos que deja el barrido

Con `splits_FIN` puesto, el error de enlace de cada candidata grande **nombra el
símbolo**, que es mucho más de lo que había:

| unidad | B | qué falta EXACTAMENTE |
|---|---|---|
| `snddrv` | 11.920 | el `1.0f` de `sfxrevc.c:952` (§6). Además `.bss` 49.352 nuestra contra 49.376 → `snddrv` necesita `aligned(32)`, y `.data` 12 contra 16 |
| `gc_memcard_interface_impl` | 10.984 | `_11RealmcIface.ALL_ENTRIES` (declarado `extern` en `memcard_interface_impl.h:1215`, **nadie lo define**; en el original es `.sdata` 0x804FF670, 4 B, GLOBAL). Y sus **88 B de `.rodata`** (los 5 `LC_msg` de `gc_interface_impl.h` + `"InsufficientSpaceMessage"` + `_vt.Q26Realmc9GCMessage`) no tienen rango: toca el patrón `REALMC_LCFMT_BASE` de la r25 §2.1 |
| `pathsnd` | 8.452 | `Path::IPathTrack::{Stop, CheckStatus, TimeRemaining, Play}` sin definir en ningún sitio: virtuales declaradas y no escritas |
| `gc_driver` | 8.952 | ya tiene su `.bss` (grupo B) pero nuestro `.text` mide **9.144** contra 8.952 (sobran 192 B) y emite **184 B de `.rodata`** con constantes de SESGO |
| `gc_interface` (rmc) | 6.152 | `asd2` referencia `__static_initialization_and_destruction_0_803906C0`: hay que fusionar `asd2` en `gc_interface` (es el corte deliberado de `configure.py` 2270) |
| `sdspmix`, `ssysinit`, `stagpat`, `satospkr` | | con los grupos E/F **dejan de dar `L0039`** y pasan a `ROTO`: el bloqueo ya es suyo |

---

## 8. Herramientas nuevas (scratchpad, prefijo `c26lk_`)

| script | qué hace |
|---|---|
| **`c26lk_split.py`** | **el barrido**: 577 `STT_FILE` × `splits.txt`, las dos direcciones. `-v` para el detalle símbolo a símbolo |
| **`c26lk_win.py`** | ventana `<sección> <ini> <fin>`: cada símbolo con su `STT_FILE`, su rango de splits y `<<<` si no cuadran. Es con la que se decide una frontera |
| **`c26lk_mk.py`** | genera `splits_X.txt` + `keep_X.lst` + `config_X.yml` a partir de la lista de cambios; `--grp A,B,…` para aislar un grupo, `--out` para el nombre |
| **`c26lk_keep.py`** | reasigna las entradas `objeto.o:gap_…` de `keep.lst` al objeto que de verdad las define en un reparto nuevo. **Imprescindible** |
| **`c26lk_link.py`** | como `trypromo.py` pero con `--obj DIR` (reparto alternativo), `--keep`, `--sub viejo=nuevo` y `--promo` |
| **`c26lk_scan.py`** / **`c26lk_fin.py`** | pasan la lista de candidatas por el enlace, una a una y luego todas juntas. **~2 min las 59** |
| `c26lk_order.py` | posición de cada objeto en el orden de enlace de `build.ninja` |

Ficheros listos: `c26lk_try/splits_FIN.txt`, `c26lk_try/keep_FIN.lst`,
`c26lk_try/out_FIN/` (los 594 objetos del reparto propuesto) y
`c26lk_locale_nofab_al32.o`.

**Cómo re-verificarlo todo en 2 minutos**:
`python …/c26lk_fin.py …/c23lk_cand.txt`.

---

## 9. Lo que NO he probado (por orden de lo que yo atacaría)

1. **`gc_memcard_interface_impl` (10.984 B)**: definir `RealmcIface::ALL_ENTRIES`
   en `gc_memcard_interface_impl.cpp` y aplicarle el `REALMC_LCFMT_BASE` de la
   r25 §2.1 para vaciar sus 88 B de `.rodata`. Es la más grande con el
   diagnóstico ya cerrado y **dos pasos concretos**.
2. **El `1.0f` de `sfxrevc.c:952`** — 4 B de fuente que valen `snddrv` (11.920 B),
   más `aligned(32)` en `snddrv` y los 4 B de `.data`.
3. **`csis` (5.728 B)**: mover `auto_05_804130A0_rodata` detrás de `csis` en el
   orden de enlace de `configure.py`, además del rango. Es el único de los seis
   de la r25 §4 que necesita reordenar, no sólo añadir.
4. **`zFEng` (71.460 B)**: las tres piezas del §6. La de `.data` es barata (mover
   `eFrameCounter`); las otras dos son trabajo de verdad.
5. **`.gnu.linkonce.r`** (lo primero que atacaría mi predecesor): **sigue sin
   medir**. Ya no la veo tan prometedora — el §4 demuestra que las vtables se
   colocan bien con un rango de `splits.txt`, que es más barato que tocar cflags
   de una biblioteca entera.
6. **La corrección limpia de `systask`/`timerthread`** (§5, última nota) y la de
   `vfprintf`/`vfprintf_1` (dirección A, `_vfwrite` de 212 B en el rango de la
   primera copia).
7. **`audit.py`/`frozen.py` sobre las dos verdes**: no los he pasado porque el
   reparto propuesto no está aplicado y leerían el `build/` compartido con el
   reparto viejo. **Hay que pasarlos al aplicar.**

## 10. Ficheros de fuente tocados

**Ninguno.** Todo lo que toqué (`locale.cpp`) está restaurado byte a byte
(`git diff` vacío). El parche del §5 va en el informe, no en el árbol, porque
**sin el grupo B de `splits.txt` rompe el enlace**.
