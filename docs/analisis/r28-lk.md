# Ronda 28 — enlace: ONCE promociones nuevas (35.264 B)

Base: `linked` 432/595, `main.dol: OK` (`9619ba57`), paquete de la r27 ya aplicado.

**Resultado: ONCE unidades promocionan, sueltas y las once juntas, con el DOL
byte a byte.** `linked` 432 → **443**, y el enlace pasa de 595 a **599** objetos.

| unidad | B (.text) | qué necesitaba |
|---|---|---|
| **`snddrv`** | **11.920** | orden de funciones (1 permuta), `aligned(32)` en `snddrv`, `aligned(8)` en `araminited`, grupo **K2** |
| **`gc_driver`** | **8.952** | quitar los 8 `operator new/delete` **redeclarados**, orden de funciones, grupo **R** + partición de `lbl_80414400` |
| **`pathsnd`** | **8.452** | las 4 virtuales **puras**, grupo **L2** (−8 B en la frontera) |
| **`pathinit`** | **2.332** | literal muerto `"%s  %s"` + grupo **V1** |
| **`pathreal6`** | **916** | literal muerto `"%s  %s"` **delante** + grupo **V2** |
| **`libc/sf_expm1`** | **804** | rango de `.sdata` (grupo **VA**) |
| **`libc/e_exp`** | **636** | rangos de `.sdata` y `.sdata2` (grupo **V8**) |
| **`libc/sf_exp`** | **556** | rangos de `.sdata` y `.sdata2` (grupo **V9**) |
| **`systask`** | **508** | el parche de `locale.cpp` de la r26 §5 (aplicado) |
| **`srender`** | **188** | nada: ya estaba verde y nadie lo había probado |
| **`avsubtitle`** | 0 | objeto **vacío** en el objetivo; sólo hay que marcarla |

**Control puro (sin promocionar nada, reparto real, 599 objetos): DOL OK.**
**Las once juntas: DOL OK.** Y un barrido de enlace sobre **las 94 candidatas**
con el paquete puesto no da ninguna verde más: éstas son todas.

> **Aviso de convivencia:** esta ronda hay más agentes tocando el árbol
> (`WRoadNetwork.cpp`, `Geometry.cpp`, `SpeechManager.cpp`, `JoyE.cpp`…). Todas
> las medidas de aquí son **enlaces del DOL completo**, que es absoluto en el
> instante en que se hacen, pero el control hay que **repetirlo al aplicar**.

---

## 1. Los dos barridos del encargo, pasados a las 94 candidatas

`c28lk_sweep.py` compara, para cada unidad extraída que tiene `.o` nuestro,
(a) la lista de símbolos de `.text` **ordenada por desplazamiento** y (b) el
tamaño y el contenido de todas las secciones de datos.

### 1.1 Orden de funciones: mal en 31 de 94

25 son SourceLists, y ahí el desorden es **consecuencia** de los símbolos que
sobran (§1 del brief), no una causa aparte. Las seis de middleware sí eran
accionables:

| unidad | diagnóstico |
|---|---|
| `snddrv` | difiere desde #11: `restoremixer` va **antes** de `fillbufwithpackets`. **CERRADO** |
| `gc_driver` | difiere desde #5 + 8 símbolos de más. **CERRADO** |
| `gc_interface` | difiere desde #18, 12 de más y 1 de menos (`__static_initialization_and_destruction_0_803906C0`) |
| `pathreal` | difiere desde #5, 4 de más (`IPathToReal::*`) |
| `filesys` | difiere desde #2, 9 de más (accesores de `FILEOPERATION`) |
| `ppc2D2` | difiere desde #8, **faltan 7** (`__div2i`, `__div2u`, `__mod2i`, `__mod2u`, `__shl2i`, `__shr2i`…) |

El barrido está en `c28lk_orden.txt`. **Vale la pena repetirlo en cada ronda:
cuesta 20 s y es el único diagnóstico que ve el fallo que objdiff no puede ver.**

### 1.2 Literales muertos: es un patrón de BIBLIOTECA, no un caso suelto

`"%s  %s"` aparece **once veces** en la `.rodata` de la zona de `path`
(0x80413110…0x80413A68), **una por TU**. Nadie lo referencia. La r27 lo encontró
en `pathvol`; esta ronda lo encuentra además en `pathsnd`, `pathtrack`,
`pathinit`, `pathbank`, `pathreal6`, `pathnode` y en al menos cuatro TU cuya
`.rodata` es **sólo** ese literal (candidatas: `pathaction`, `pathcontrol`,
`pathdebug`, `pathevent`, `pathrand`, `pathserv`).

**El hallazgo de método, y cambia dónde hay que ponerlo:**

> **GCC 2.95 emite TODAS las cadenas ANTES que las vtables.** En una TU con
> vtable el literal muerto **no puede ir al final**: sólo cabe **delante**.

Medido con seis variantes sobre `pathtrack` (`c28lk_ptvar.py`): `static inline`,
`inline`, miembro de `struct`, `static` a secas y `static const char[]` puestas
al final dan **todas** el literal en `+8`, delante de la vtable. Puesta arriba
del fichero da `+0`, que es lo que tiene el objetivo.

Corolario que **corrige la lectura de la r27**: la frontera de `pathsnd` no es
0x80413770 sino **0x80413768**; el literal de 0x80413768 es el **primero de
`pathtrack`**, no el último de `pathsnd`. Por eso `pathsnd` promociona **sin
tocar la fuente**, sólo bajando 8 B el `end` (grupo L2).

Y el literal **no está en `pathi.h`**: si lo pones ahí, las seis TU ya
promocionadas emiten 8 B que su (inexistente) rango no cubre. Va fichero a
fichero, y queda anotado como deuda en cada uno.

### 1.3 La herramienta que faltaba: `c28lk_find.py`

Busca **el contenido** de una sección nuestra dentro del ELF original,
ignorando las posiciones con reubicación, y devuelve el rango. Con eso, una
unidad cuyo único problema es «no tiene rango» pasa de investigación a un
número. Dio **17 rangos únicos** de un tirón; los cinco del paquete salen de ahí.

---

## 2. El paquete: qué hay que aplicar

### 2.1 `config/GOWE69/splits.txt` — 8 cambios

```
Speed/Indep/Libs/snd/9/source/library/gc/snddrv.c:
    .rodata     start:0x80412AF0 end:0x80412B78     (K2, era ...end:0x80412B7C)
Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp:
    .rodata     start:0x80413300 end:0x80413768     (L2, era ...end:0x80413770)
Speed/Indep/Libs/path/5.01.04/source/cmn/pathinit.cpp:
    .rodata     start:0x80413140 end:0x80413168     (V1, NUEVO)
Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal6.cpp:
    .rodata     start:0x80413258 end:0x804132F0     (V2, NUEVO)
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp:
    .rodata     start:0x80414408 end:0x804144C0     (R,  NUEVO)
libc/e_exp.c:
    .sdata      start:0x804FF6E0 end:0x804FF758     (V8, NUEVO)
    .sdata2     start:0x80500A18 end:0x80500A48     (V8, NUEVO)
libc/sf_exp.c:
    .sdata      start:0x804FF0C8 end:0x804FF110     (V9, NUEVO)
    .sdata2     start:0x80500370 end:0x80500388     (V9, NUEVO)
libc/sf_expm1.c:
    .sdata      start:0x804FF288 end:0x804FF2F0     (VA, NUEVO)
```

### 2.2 `config/GOWE69/symbols.txt` — LA PARTICIÓN, con su verificación

`lbl_80414400` junta 8 B de `memcard_utilities` (`"&|!~^"`) y los 28 del bloque
`LC_msg` de `gc_driver`. Sustituir la línea

```
lbl_80414400 = .rodata:0x80414400; // type:object size:0x24
```

por

```
lbl_80414400 = .rodata:0x80414400; // type:object size:0x8
lbl_80414408 = .rodata:0x80414408; // type:object size:0x1C
```

**Verificado**: con la partición y las dos líneas de `keep.lst` del §2.3, el
**control** (sin ningún grupo, sin promocionar nada) da **DOL OK**, y el grupo R
también. Sin ellas da `ROTO be7e3ea70cfe`.

**Y aquí está la trampa, que costó tres intentos:**

- `dtk` **ignora el `size:0x8`** y parte el símbolo en `lbl_80414400` (6 B,
  cadena) + `gap_05_80414406_rodata` (2 B): detecta la cadena por su contenido.
  El contenido del objeto sale **idéntico byte a byte**; lo que cambia es que
  aparecen dos símbolos nuevos.
- **`-strip-unused-data` se lleva los dos**: nadie referencia el relleno, y
  **`lbl_80414408` está MUERTO** (ninguna reubicación del DOL apunta al bloque
  `LC_msg`). Al desaparecer 28 + 2 B, todo lo que va detrás se desplaza y el DOL
  sale roto con diferencias de −0x18 en los `@l` de `gc_driver`.
- La variante «fiel a `gc_mci`» (`size:0x6 data:string` + `0x2` + `0x18`) falla
  igual y por lo mismo.

**Regla general que sale de aquí: al partir un símbolo de `symbols.txt` hay que
comprobar si alguna de las mitades queda sin referencias, y meterla en
`keep.lst`.**

### 2.3 `config/GOWE69/keep.lst` — 2 líneas nuevas

```
gc_driver.o:lbl_80414408
auto_05_804143F8_rodata.o:gap_05_80414406_rodata
```

(el dueño de `lbl_80414408` es `gc_driver.o` **con** el grupo R; sin él es
`auto_05_804143F8_rodata.o`.)

### 2.4 `configure.py` — 8 comodines que añadir, 4 que quitar (595 → 599)

| añadir | detrás de |
|---|---|
| `auto_05_80412B78_rodata` | `snddrv.c` |
| `auto_05_80413168_rodata` | `pathinit.cpp` |
| `auto_05_804132F0_rodata` | `pathreal6.cpp` |
| `auto_05_80413768_rodata` | `pathsnd.cpp` |
| `auto_05_804144C0_rodata` | `gc_driver.cpp` |
| `auto_08_804FF110_sdata` | `sf_exp.c` |
| `auto_10_80500388_sdata2` | `sf_exp.c` |
| `auto_10_80500A48_sdata2` | `e_exp.c` |

Quitar **`auto_05_80413770_rodata`**, **`auto_08_804FF288_sdata`**,
**`auto_10_80500390_sdata2`** y **`auto_10_80500A28_sdata2`** (los cuatro los
sustituyen los de arriba). `c28lk_go.py` coloca los nuevos **por sí mismo**
emparejando `auto_XX_<dir>_<sec>` con el cambio cuyo `end` es esa dirección;
la regla, como en la r27, es que el comodín va **detrás de la unidad que carvó
el rango**.

Y marcar `Matching` las once unidades del encabezado.

---

## 3. La fuente: lo que hay aplicado en el árbol

### 3.1 `pathsnd` — las cuatro virtuales eran PURAS, no faltaban

El encargo decía «`Path::IPathTrack::{Stop, CheckStatus, TimeRemaining, Play}`
declaradas y no escritas». **No hay que escribirlas: en el objetivo son
`__pure_virtual`.** Las vtables del original lo dicen sin margen:
`_vt.Q24Path10IPathTrack` (0x80413778) tiene `__pure_virtual` (0x8031AB80) en
**19** ranuras, y `_vt.Q24Path12PathTrackSnd` lo conserva en las cuatro de
`Stop`, `CheckStatus`, `TimeRemaining` y `Play`.

Las 19 son exactamente las 19 declaraciones **sin cuerpo** de `IPathTrack.h`, y
exactamente los 19 símbolos `IPathTrack::` que `pathtrack.o` tenía indefinidos.
Poniéndoles `= 0`:

- `pathsnd` `.text` pasa a **8.452 = 8.452**, mismos símbolos y mismo orden;
- `pathtrack` `.text` ya estaba en **2.472 = 2.472** y sigue igual;
- ninguna otra TU del árbol usa `Path::IPathTrack` (el `IPathTrack` de
  `SFXCTL_Pathfinder5.hpp` es **otro struct**, declarado allí mismo).

### 3.2 `gc_driver` — la regla 4 de la r27 §4.2 NO valía aquí

El diagnóstico de la r27 («meter los cuerpos dentro de la clase y dejan de
emitir símbolo») **está probado y es falso para esta clase**: con los cuerpos
dentro, con y sin la palabra `inline`, GCC sigue emitiendo los ocho operadores.

La causa es la del §1 del brief: **`GCDriver` es polimórfica y su *key method*
(`~GCDriver`) se define en este TU**, así que GCC 2.95 emite fuera de línea
todos sus miembros inline. `DeviceDriver`, que declara **los mismos ocho
operadores** con cuerpo en clase, no los emite porque su destructor es inline y
la clase no se «implementa» en ningún TU.

**La cura es quitar la redeclaración**: `GCDriver` hereda los ocho operadores de
`DeviceDriver`, que son idénticos línea a línea. `.text` 9.144 → **8.952 = 8.952**.

Después, `c28lk_reord.py` reordena las 40 definiciones de `GCDriver::` al orden
del objeto extraído (difería desde el #5). Con eso los 46 símbolos salen en el
mismo orden y con el mismo tamaño.

### 3.3 `snddrv` — tres cosas, ninguna era el `1.0f`

1. **Orden**: una sola permuta, `SNDDRV_restoremixer` delante de
   `SNDDRV_fillbufwithpackets`.
2. **`.bss`**: `SNDDRVSTATE snddrv` necesita `__attribute__((aligned(32)))`.
   El DWARF del original lo confirma al dígito: `ptr` @0x804C03A0 y `snddrv`
   @**0x804C03C0**, 32 bytes después. `.bss` 49.352 → **49.376 = 49.376**.
3. **El `1.0f` de `sfxrevc.c:952` NO es de `snddrv`.** El DWARF del original
   dice que la `.data` de `snddrv` la forman **sólo** `everyother` (0x80450D58),
   `curTick` (0x80450D5C) y `araminited` (0x80450D60): **acaba en 0x80450D64**.
   El `1.0f` de 0x80412B78 es el primer literal de `sfxrevc`, y basta con bajar
   el `end` de la `.rodata` a 0x80412B78 (grupo K2). El `L0039: undefined
   lbl_80412B78` desaparece porque el comodín nuevo lo define.
4. **`.data`: 4 B de relleno de alineación.** `araminited` lleva
   `__attribute__((aligned(8)))` para que la sección mida 16 y `sfxrevc` caiga
   en 0x80450D68. **Es una palanca mía**: el original consigue esos 4 B porque
   su `sfxrevc.o` tiene la `.data` alineada a 8 y el nuestro a 4; alinear
   `tablecopyto` a 8 también funciona pero **engorda la sección de `sfxrevc` a
   1600** y rompe lo que va detrás. Queda anotado como deuda.

### 3.4 `systask` — el parche de `locale.cpp` de la r26 §5, aplicado

Borradas las líneas 16-23 (`statInfo`, `sMsg` y el `__asm__` que las anulaba) y
puesto `__attribute__((aligned(32)))` en `gTrcMsgBuffer`. Con el grupo B ya en
el árbol, `systask` promociona: **DOL OK**. Y el control sigue OK, así que la
retirada de la falsificación **ya no rompe nada**.

### 3.5 Los literales muertos (deuda declarada)

Añadidos, cada uno con su comentario `DEUDA r28`:

- `pathsnd.cpp` — **retirado**: no hacía falta (§1.2).
- `pathinit.cpp`, `pathbank.cpp` — al **final** (sin vtable).
- `pathtrack.cpp`, `pathreal6.cpp` — al **principio** (con vtable).

Forma: `static inline const char *X() { return "%s  %s"; }`. **No emite código**
(`.text` idéntica) pero sí interna el literal. **Es la deuda del §4.4 de la r27
repetida cuatro veces; la forma original se desconoce.**

`pathtrack.cpp` y `pathbank.cpp` quedan con el literal puesto **sin rango**: son
`NonMatching`, el enlace usa el objeto extraído y el control da DOL OK. Es
trabajo adelantado, no trabajo a medias.

---

## 4. Lo que queda medido y NO cerrado

| unidad | B | lo que falta, ya contado |
|---|---|---|
| `pathtrack` | 2.472 | rango **S** `.rodata 0x80413768..0x804138A0` (verificado inocuo, control DOL OK) **y 8 B de marco de pila** en `PATH_createstreamimp`: el objetivo hace `stwu r1,-40` y nosotros `-32`. Sólo **8 bytes** de diferencia en todo el DOL |
| `pathbank` | 2.156 | rango `0x8041399C..0x80413A40` **bloqueado por `lbl_80413A2C`** (0x1C, junta `"%.*s%d.mus"` + dos `"%s  %s"`) **y 48 B** de planificación en una función (`0x8037BE04`) |
| `csis` | 5.728 | emite de más `_._Q32EA9Allocator10IAllocator` (52 B de `.text`) y `_vt.Q32EA9Allocator10IAllocator` (56 B de `.rodata`), que en el objetivo viven en **otro TU** (0x80064D14 y 0x803D1230). Y le faltan rangos de `.data` (28 B) y `.bss` (28 B) |
| `gc_interface` | 6.152 | 12 símbolos de más, falta `__static_initialization_and_destruction_0_803906C0` (hay que fundir `asd2`), orden desde el #18, y 712 B de `.rodata` sin rango (no localizable con `c28lk_find`: el contenido es todo reubicaciones) |
| `avplayer` | 4.224 | dos cosas: sobra un `$LC0` **cadena vacía** de 4 B usado por `GetFirstFrame`, y `IsTimeForDecode` genera `0x4330000000000000` (unsigned→double) donde el objetivo tiene `0x41E0000000000000` **repetido** (dos conversiones double→unsigned). La fuente actual es `(float)(CurTimeMs + m_VideoLatencyInMs)` |
| `pathnode` | 4.204 | `.text` **−8 B** (nos falta código) + literal muerto + rango `0x80413A48..0x80413A70` |
| `postproc` | 3.268 | rango `.rodata 0x80411458..0x804114A8` **solapa** con un split existente, y le faltan 768 B de `.bss` y 108 de `.sbss` |
| `filesys` | 10.872 | 9 accesores de más y orden desde el #2 |
| `ppc2D2` | 2.448 | **faltan 7 funciones** (`__div2i`, `__div2u`, `__mod2i`, `__mod2u`, `__shl2i`, `__shr2i`, +1): son de `libgcc` y están sin escribir |
| `libc/ef_rem_pio2` | 848 | los dos rangos localizados (`.sdata 0x804FF350..0x804FF388`, `.sdata2 0x8050042C..0x805007C4`) pero el segundo **rompe el enlace**: `L0039: undefined @961_80500810` desde `steering.c` |
| `libc/e_pow`, `ef_pow`, `e_rem_pio2`, `k_rem_pio2`, `kf_rem_pio2` | ~7.800 | a cada una `c28lk_find.py` le encuentra **una** de sus dos secciones y la otra **no aparece en el ELF**: su contenido difiere de verdad, no es sólo falta de rango |
| `libc/sf_log`, `kf_tan` | ~1.500 | `.sdata` de 8 B con **13 coincidencias**: el bloque es demasiado corto para localizarlo así |
| `libgcc2_4` | 176 | rango `0x8040FE28..0x8040FE40` **bloqueado por `lbl_8040FE38`** (0x8040FE38..0x8040FED0). Partido en 0x8040FE40 el control da `ROTO` con y sin `keep.lst` |
| `bigswizzler` | 1.208 | rango `0x8041010C..0x80410124` **bloqueado por `lbl_80410120`** (0x80410120..0x80410154). Partido, el enlace pierde `_vt.18VP6_CODEC_INTERNAL`: ese trozo es de `rcmp_vp6_codec` |

---

## 5. Lo que NO he probado

1. **El barrido de orden de funciones sobre las 25 SourceLists.** Sale «mal» en
   todas, pero ahí el orden lo decide el `SourceList` de `configure.py` y el
   ruido de los símbolos que sobran. **No lo he separado.**
2. **`csis`, `gc_interface`, `avplayer`, `filesys`, `pathnode`, `postproc`**:
   diagnosticadas en el §4, ninguna tocada.
3. **Las cinco `libc` con una sección sin localizar** (`e_pow`, `ef_pow`,
   `e_rem_pio2`, `k_rem_pio2`, `kf_rem_pio2`, ~7,8 kB): no he mirado **por qué**
   su `.sdata`/`.sdata2` no aparece en el ELF. Con `c28lk_find.py` ampliado para
   dar la coincidencia parcial más larga se sabría en un minuto.
4. **Las particiones de `lbl_80413A2C`, `lbl_8040FE38` y `lbl_80410120`**: las
   tres probadas y las tres **descartadas** (§4). La de `lbl_80413A2C` deja el
   control en DOL OK con cinco líneas de `keep.lst`, pero `pathbank` sigue rojo
   por los 48 B de código.
5. **`audit.py` / `frozen.py` sobre las once verdes**: no los he pasado. El DOL
   byte a byte es una prueba más fuerte, pero **hay que pasarlos al aplicar**,
   porque el reparto propuesto no está en el árbol.
6. **De dónde sale de verdad el `"%s  %s"`** de las once TU de `path`. Sé dónde
   NO está (`pathi.h`) y sé que en las TU con vtable tiene que emitirse antes
   que ellas; no sé qué construcción lo produce.
7. **Los 4 B de `.data` de `snddrv`** por la vía buena (alineación de `sfxrevc`)
   en vez de por `aligned(8)` en `araminited`.
8. **El `1.0f`** ya no es un problema, pero **no he comprobado si el mismo
   patrón** (frontera de rango que se come el primer literal del vecino) explica
   los otros `L0039` de la ronda.

---

## 6. Herramientas nuevas (scratchpad, prefijo `c28lk_`)

| script | qué hace |
|---|---|
| **`c28lk_sweep.py`** | los dos barridos: orden de símbolos de `.text` y tamaño/contenido de las secciones de datos, nuestro contra extraído. Sin argumentos barre las 94 candidatas |
| **`c28lk_find.py`** | **la más productiva**: busca el contenido de una sección nuestra en el ELF original ignorando reubicaciones y devuelve el rango. 17 aciertos únicos |
| `c28lk_go.py` | el `c27lk_go.py` con sandbox propio, la tabla de grupos de la r28 y `--addkeep obj:sym,…` para añadir líneas a `keep.lst` |
| `c28lk_cand.py` | las unidades extraídas del enlace que tienen `.o` nuestro |
| `c28lk_reord.py` | el reordenador de `gc_driver.cpp` |
| `c28lk_rodump.py` | vuelca una sección nuestra y la extraída en paralelo, con símbolos |
| `c28lk_symdiff.py` | qué símbolos del ELF enlazado no caen en la dirección del original |
| `c28lk_ptvar.py` | el banco de variantes que demostró que las cadenas van antes que las vtables |

**Aviso de convivencia:** el `c27lk_try/` del predecesor (100 MB) **había
desaparecido** al empezar la ronda; los scripts sobrevivieron. El sandbox nuevo
(`c28lk_try/`) necesita sus copias de `config/GOWE69/symbols.txt` y de
`orig/GOWE69/sys/main.dol` porque `dtk` resuelve las rutas contra el CWD.

**Y una trampa que me costó media hora, mía:** dejé un `sfxrevc.o` rancio de un
ensayo revertido y `snddrv` salió `ROTO` con 33 kB de diferencia. **Revertir la
fuente no basta: hay que recompilar.**
