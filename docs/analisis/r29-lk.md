# Ronda 29 — enlace: cinco promociones y **la bandera que faltaba**

Base: `linked` 443/599, `main.dol: OK` (`9619ba57`), paquete de la r28 aplicado.
Control puro con el reparto real (599 objetos): **DOL OK**.

**Resultado: cinco unidades promocionan (10.028 B), sueltas y las cinco juntas,
con el DOL byte a byte.** Y sale de aquí un hallazgo que vale más que las cinco:

> ## `-fno-implement-inlines` es la bandera que le falta al middleware C++
>
> `import_export_decl` de GCC 2.95 decide así un miembro **inline**:
>
> ```c
> DECL_NOT_REALLY_EXTERN (decl) = !(CLASSTYPE_INTERFACE_ONLY (ctype)
>          || (DECL_THIS_INLINE (decl) && !flag_implement_inlines
>              && !DECL_VINDEX (decl)));
> ```
>
> Con la bandera, los inline **no virtuales** dejan de emitirse fuera de línea y
> **los virtuales se siguen emitiendo** (los necesita la vtable). Eso es
> exactamente lo que tiene el objetivo en TODAS las unidades que sobraban
> símbolos: los accesores no, los destructores sí.
>
> Barrida sobre 22 unidades de middleware: **8 mejoran, 0 regresan**, y tres
> pasan a EXACTAS (`pathreal` 496=496, `gc_device` 284=284, `rcmpbase`
> 1184=1184, con el orden de símbolos ya correcto).

---

## 1. Las cinco promociones verificadas

| unidad | B (`.text`) | qué necesitaba |
|---|---|---|
| **`csis`** | **5.728** | `#pragma interface` en `iallocator.h` + `#pragma implementation "iallocator.h"` en `zBWare.cpp`; rangos **B2** |
| **`postproc`** | **3.268** | **`-fno-common`**; retirar el bloque `__asm__` de `maddeca.cpp`; rangos **A1** |
| **`pathreal`** | **496** | 7 virtuales **puras**, `#pragma interface`/`implementation`, fuera `PATH_REAL_EMIT_METHODS`; **`-fno-implement-inlines`**; rangos **B1** |
| **`gc_device`** | **284** | **`-fno-implement-inlines`**; rango **B5** |
| **`sstfxlev`** | **252** | rango **A3** (nada de fuente) |

Verificado suelto y **las cinco juntas: DOL OK**. Control con los grupos puestos y
sin promocionar nada: **DOL OK**. El enlace pasa de 599 a **600** objetos.

## 1-bis. El paquete: qué hay que aplicar

### `config/GOWE69/splits.txt` — 12 líneas

```
egami/rcmp/dev/source/decoder/cmn/maddeca.cpp:
    .rodata   start:0x80411358 end:0x80411458   (A1, era ...end:0x804114A8)
Packages/vp6/1.0.6/source/decode/cmn/postproc.c:
    .rodata   start:0x80411458 end:0x804114A8   (A1, NUEVO)
libc/itoa.c:
    .rodata   start:0x8040FB78 end:0x8040FBB8   (A2, NUEVO)
Speed/Indep/Libs/snd/9/source/library/cmn/sstfxlev.c:
    .rodata   start:0x804128D8 end:0x804128E8   (A3, NUEVO)
Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp:
    .rodata   start:0x80413768 end:0x804138A0   (S,  NUEVO)
Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal.cpp:
    .rodata   start:0x80413168 end:0x80413258   (B1, NUEVO)
    .bss      start:0x804CC6EC end:0x804CC6F8   (B1, NUEVO)
    .sdata    start:0x804FF604 end:0x804FF608   (B1, NUEVO)
Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp:
    .data     start:0x80451E88 end:0x80451EA4   (B2, NUEVO)
    .bss      start:0x804CC6D0 end:0x804CC6EC   (B2, NUEVO)
Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device.cpp:
    .rodata   start:0x80413DB0 end:0x80413E30   (B4, NUEVO)
Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_device.cpp:
    .rodata   start:0x80414C90 end:0x80414CE0   (B5, NUEVO)
```

**Nadie de estos parte un símbolo de `symbols.txt`**: no hace falta tocar
`symbols.txt` ni `keep.lst` (`mk_keep` no propone ni un cambio).

### `configure.py` — 6 comodines que añadir, 5 que quitar (599 → 600)

| añadir | detrás de |
|---|---|
| `auto_05_804138A0_rodata` | `pathtrack.cpp` |
| `auto_05_80413E30_rodata` | `dvd_device.cpp` |
| `auto_05_80414CE0_rodata` | `gc_device.cpp` |
| `auto_06_80451EA4_data` | `csis.cpp` |
| `auto_07_804CC6F8_bss` | `pathreal.cpp` |
| `auto_08_804FF608_sdata` | `pathreal.cpp` |

Quitar `auto_05_804128D8_rodata`, `auto_05_80413168_rodata`,
`auto_05_80413768_rodata`, `auto_06_80451E88_data` y `auto_07_804CC6D0_bss`.

### `configure.py` — banderas (§3.1 y §3.2)

- **`-fno-implement-inlines`** en los cflags de `realcore`, `path`, `csis`,
  `realmemcard` y `rcmp`. Sin ella `pathreal` y `gc_device` **no promocionan**.
- **`-fno-common`** en `Packages/vp6/1.0.6/source/decode/cmn/postproc.c`
  (`Object(..., cflags=[...])`). Sin ella `postproc` **no promociona**.

Y marcar `Matching` las cinco: `csis.cpp`, `postproc.c`, `pathreal.cpp`,
`gc_device.cpp`, `sstfxlev.c`.

### `src/` — el único cambio que NO está en el árbol

`src/egami/rcmp/dev/source/decoder/cmn/maddeca.cpp`: borrar del bloque
`__asm__` todo lo que va **detrás del último `.long` de `zigzag`**, desde el
`"\t.align 3\n"` que sigue a `zigzag` hasta (sin incluir) `"\t.previous\n"`
— son 1.034 B de fuente, diez etiquetas `lbl_80411458 … lbl_804114A0`.
**Va con el grupo A1 o el enlace se queda con `L0039` indefinidos.**
Los otros cuatro ficheros de fuente (§3.4-3.6) **ya están aplicados** y el
control con el árbol tal cual da **DOL OK**.

---

## 2. Herramientas nuevas (scratchpad, prefijo `c29lk_`)

| script | qué hace |
|---|---|
| **`c29lk_locate.py`** | localiza una sección de datos nuestra en el ELF original **por NOMBRE de símbolo** (casa `x` con `x.NN`), comprueba la coherencia de los desplazamientos y propone el rango. **Es lo que `c28lk_find.py` no puede hacer: `.bss`/`.sbss` no tienen contenido que buscar.** 107 aciertos coherentes |
| **`c29lk_ready.py`** | las unidades cuyo `.text` **ya es idéntico** (relocs enmascaradas) y qué secciones de datos les faltan. **25 unidades, 40.264 B**: es la lista de trabajo del enlace |
| **`c29lk_fii.py`** | recompila con `-fno-implement-inlines` y compara símbolos/orden/tamaños contra el extraído |
| **`c29lk_common.py`** | unidades con símbolos `COMMON` que el objetivo tiene en `.bss`/`.sbss` (síntoma de que falta `-fno-common`) |
| **`c29lk_find2.py`** | como `c28lk_find` pero da la **coincidencia parcial más larga** y busca cada símbolo por separado |
| **`c29lk_align.py`** | alinea nuestro bloque de datos con el ELF en una dirección y dice **entrada a entrada** qué mete el objetivo de más |
| **`c29lk_win.py`** | acota las coincidencias a la **ventana** que dejan los rangos vecinos en `splits.txt` |
| **`c29lk_gaps.py`** | huecos sin asignar de una sección, con la unidad de antes y la de después |
| **`c29lk_secs.py`** / `c29lk_tcmp.py` / `c29lk_dsym.py` / `c29lk_snap.py` | tamaños por sección, comparación de `.text` con relocs enmascaradas, símbolos de datos, y huella de los 526 objetos para el A/B de cabecera compartida |
| `c29lk_go.py` | el `c28lk_go.py` con caja propia (`c29lk_try`) y la tabla de grupos de la r29 |

---

## 3. Los hallazgos, uno a uno

### 3.1 `-fno-implement-inlines` (§ del encabezado)

Medido, unidad a unidad (`.text` nuestro → con la bandera, contra el objetivo):

| unidad | antes | con `-fnii` | objetivo | símbolos de más |
|---|---|---|---|---|
| `pathreal` | 532 | **496** | 496 | 1 → **0** |
| `gc_device` | 376 | **284** | 284 | 2 → **0**, orden OK |
| `rcmpbase` | 1.260 | **1.184** | 1.184 | 3 → **0**, orden OK |
| `dvd_device` | 2.760 | 2.680 | 2.704 | 1 → **0**, orden OK |
| `filesys` | 10.960 | 10.868 | 10.872 | 9 → **0** |
| `gc_interface` | 6.492 | 6.352 | 6.152 | 12 → 6 |
| `interface` | 404 | 260 | 204 | 13 → 7 |
| `rcmp_mad_codec` | 3.040 | 2.780 | 2.592 | 6 → 3 |
| `rcmp_vp6_codec` | 3.140 | 2.880 | 2.828 | 4 → 1 |
| `gc_blockcalculator` | 384 | 364 | 312 | 2 → 1 |

Y **ninguna de las otras 12 barridas cambia un byte** (`pathbank`, `pathnode`,
`pathtrack`, `inittmr`, `avplayer`, `bigswizzler`, `bigyuvswizzler`, `madidct`,
`criticalpath`, `spchpick`, `csis`, `eathread_thread`).

**Y en las SourceLists también gana** (dos catas, `.text` nuestro → con la
bandera, contra el objetivo):

| unidad | antes | con `-fnii` | objetivo | símbolos de más |
|---|---|---|---|---|
| `zBWare` | 37.956 | **37.744** | 35.772 | −8 (se van los ocho constructores) |
| `zFoundation` | 36.348 | **36.280** | 36.000 | 77 → 75 |

En ninguna de las dos aparece un símbolo que falte. **Ni una regresión en las
24 unidades barridas.**

**Propuesta de `configure.py`**: añadir `-fno-implement-inlines` a `cflags_base`
(o al menos a `realcore`, `path`, `csis`, `realmemcard`, `rcmp` y las
SourceLists). Sólo se han catado 2 de las 33 SourceLists.

### 3.2 `-fno-common`: 12 unidades con símbolos COMMON

`c29lk_common.py` cruza los `SHN_COMMON` de nuestro `.o` con la sección donde
los tiene el objetivo. **`postproc` es exacto**: con `-fno-common` sale
`.bss 768 = 768` y `.sbss 108 = 108`, **los mismos 28 símbolos y los mismos
desplazamientos**. `quantize` (4 B de `.sbss`) y `zPhysicsBehaviors`
(2.304 B de `.bss`) mapean igual; las otras nueve son SourceLists cuyos COMMON
no aparecen en el objetivo.

### 3.3 Los 80 B de `.rodata` de `postproc` NO son de `maddeca`

`maddeca.cpp` lleva un bloque `__asm__` escrito a mano que emite diez dobles con
los nombres `lbl_80411458 … lbl_804114A0`. **Sólo el `postproc.o` EXTRAÍDO los
referencia** (barrido sobre los 526 objetos nuestros y los 161 extraídos: cero
referencias en el árbol). Son el **pool de literales de `postproc`**, que nuestra
compilación genera por su cuenta en su propia `.rodata`.

La cura es retirar el ensamblador y mover la frontera:
`maddeca .rodata end → 0x80411458`, `postproc .rodata 0x80411458..0x804114A8`.

**Aviso:** el bloque `__asm__` **no se puede retirar suelto**: sin el cambio de
`splits.txt` el enlace queda con `L0039: lbl_804114xx` indefinidos. Va con el
paquete o no va. Por eso `maddeca.cpp` se deja **como estaba** en el árbol.

### 3.4 Clases sin *key method*: `#pragma interface` / `#pragma implementation`

`EA::Allocator::IAllocator` y `Path::IPathToReal` no tienen *key method* (todas
sus virtuales son puras o inline), así que GCC 2.95 emite su vtable **en cada TU
que la usa**. El objetivo la tiene en **una sola**:

- `IAllocator`: sólo en `zBWare` (`_._Q32EA9Allocator10IAllocator` @0x80064D14,
  `_vt` @0x803D1230). Nuestros `csis`, `zEAXSound`, `zFe` y `rcmp_vp6_codec`
  también la emitían.
- `IPathToReal`: sólo en `pathreal`.

Con `#pragma interface` en la cabecera y `#pragma implementation` en la TU dueña
sale el reparto del objetivo. **Es el mecanismo, y ya se usaba en el árbol**
(`gc_blockcalculator.h`, `driver.h`, `input.h`).

**Coste medido de `iallocator.h`** (A/B sobre los 526 objetos, `c29lk_snap.py`):
cambian **cinco**. `csis` queda exacto; `zEAXSound` y `zFe` sueltan 52 B de
`.text` y 56 de `.rodata` que el objetivo no tiene (mejora); **`zBWare` gana
152 B de `.text` y 40 de `.rodata`** (los ocho `__Q22EA12TagValuePair*`,
`__Q32EA9Allocator10IAllocator`, `__Q32EA9Allocator14ICoreAllocator` y
`_vt.Q32EA9Allocator14ICoreAllocator`, que el objetivo **no** tiene). Es el
precio de conservar los dos que sí tiene.

**Y ese precio lo paga `-fno-implement-inlines`**: con la bandera puesta,
`zBWare` baja a **37.744** (212 B menos que con el `#pragma` solo, y **60 B
menos que antes de tocar nada**) y desaparecen los ocho constructores; sólo
sobra el `_vt.Q32EA9Allocator14ICoreAllocator` (una vtable, que la bandera no
filtra por diseño). O sea: **el `#pragma` sale gratis si la bandera va con él**.

### 3.5 `IPathToReal`: siete virtuales PURAS, medido en la vtable

`_vt.Q24Path11IPathToReal` (0x804131E0, 120 B) tiene **siete ranuras**
apuntando a `__pure_virtual` (0x8031AB80): `GetMilliseconds`,
`GetMinStreamBufferSize`, `LoadFile`, `LoadFileDone`, `LoadFileSync`,
`FileExists`, `FileSize` — **exactamente** los siete indefinidos que impedían
enlazar `pathreal`. Es el mismo caso que `IPathTrack` en la r28.

Y tres miembros que el objetivo **no tiene en ninguna parte** (`AbortMessage`,
`PrintMessage`, `LogMessage`, cuerpos vacíos) se han retirado de la cabecera.

**Ojo con el orden de aplicación**: poner los `= 0` **sin** el `#pragma
interface` **rompe `pathreal6`** (que está promocionada): al quedarse la clase
sin *key method*, `pathreal6` emite los cinco `Set*/GetSynchMode` y pasa de
916=916 a 956. Con el `#pragma` vuelve a 916. Verificado también en `pathsnd`
(8.452=8.452), `pathinit` (2.332=2.332), `pathtrack`, `pathbank`, `pathvol`.

### 3.6 `filesys`: de +88 B y 9 símbolos de más a **−4 B y cero**

Con `-fno-implement-inlines` desaparecen los nueve accesores de `FILEOPERATION`,
y con cuatro movimientos de definición el orden queda **idéntico al objetivo**:

1. `iStartDevice` entre `Find` e `iGetNextOpId`.
2. `FILE_init` delante de `FILE_overhead` (hace falta declarar `FILE_restore()`).
3. `AddSearchLocation` delante de `AddDevice`.
4. `FILESYSINFO::~FILESYSINFO` **`inline` y definido justo antes de
   `struct ExistOperation`**. Esto es fino y está medido: en clase (o antes de
   `FILE_restore`) GCC lo integra y `FILE_restore` pasa de 200 a 520 B; al final
   del fichero no se integra pero se emite el **último**; en ese punto exacto
   sale en la ranura #43, entre `_._13FILEOPERATION` y `_._14ExistOperation`,
   que es donde lo tiene el objetivo. **El orden de la tanda diferida es el
   orden de PARSEO de los cuerpos inline.**

Queda: `AddToQueue` 248 contra 252 y `.rodata` 616 contra 620.

### 3.7 Las cinco `libc`: **resuelto por qué su `.sdata` no aparece**

Era el punto 3 de «lo que no he probado» de la r28. `c29lk_find2.py` +
`c29lk_align.py` lo cierran en un minuto:

> **El objetivo mete la constante de sesgo int→double `0x4330000080000000`
> EN MEDIO del bloque, justo detrás de `cp`; nosotros la ponemos al FINAL.**

- `e_pow` `.sdata`: prefijo de **160 de 272 B** en 0x804FEEF0; `cp` a +152,
  el sesgo a +160, `mone` a +168. El bloque **mide lo mismo (272 B)**:
  `0x804FEEF0..0x804FF000`.
- `ef_pow` `.sdata`: igual en 0x804FF180, con 4 B de relleno delante del sesgo
  (los estáticos son `float`).
- `kf_rem_pio2 .sdata2` (60 de 84 B en 0x805007C4), `k_rem_pio2 .sdata2`
  (80 de 136 en 0x80500BD0), `e_rem_pio2 .sdata` (9 de 88 en 0x804FF244):
  mismo síntoma, sin desglosar.

El árbol ya tiene la maquinaria (`SDA_DOUBLE(i2d_bias, 4503601774854144.0)`,
`I2D_STORE`, `I2D_VALUE`, verificada al 100 % en `sf_log10.c`): hay que
declarar `i2d_bias` **entre `cp` y `mone`** y pasar la conversión a mano para
que el compilador no genere la suya. **No hecho.**

### 3.8 `ppc2D2`: las 7 funciones que faltan son **thunks de 4 B**

Cada una es **una sola instrucción `b`** con reubicación REL24:

```
__shr2u: b __lshrdi3    __div2i: b __divdi3     __shl2i: b __ashldi3
__mod2i: b __moddi3     __shr2i: b __ashrdi3    __div2u: b __udivdi3
__mod2u: b __umoddi3
```

Van entre `__cvt_fp2unsigned` y `__va_arg` (0x48..0x64). Son los alias de SN
sobre `libgcc`; **en el original son ensamblador**, y por eso no se han escrito.
La unidad tiene además: `__cvt_fp2unsigned` 68/72, `__va_arg` 232/244,
`DoFSReadHeader` 136/140, `CompletePCreadAsync` 396/**388**, y el orden
(`CompletePCreadAsync` va **antes** de `PCrwAsyncFSACK`, no después).

---

## 4. Los dos barridos del encargo, repasados

### 4.1 Orden de funciones: 29 mal de 94 (eran 31)

25 son SourceLists. De las cuatro de middleware que quedaban:

| unidad | estado |
|---|---|
| `pathreal` | **CERRADO** (orden exacto, 496=496) |
| `filesys` | **CERRADO** (orden exacto, 74 símbolos, sólo 4 B en `AddToQueue`) |
| `gc_interface` | sigue: difiere desde #18, 6 de más y el renombrado de `__static_initialization_and_destruction_0` → `..._803906C0` |
| `ppc2D2` | sigue: difiere desde #8 y le faltan los 7 thunks |

Y de propina, con `-fno-implement-inlines` quedan con el orden bien
`gc_device`, `dvd_device` y `rcmpbase`.

### 4.2 Literales muertos

**Sin avance.** No he encontrado de dónde sale el `"%s  %s"` de las once TU de
`path`; los cuatro `static inline` de la r28 siguen puestos como deuda. Lo que sí
sale de esta ronda es **otro** literal muerto: a `filesys` le faltan 4 B en
`.rodata` **en el desplazamiento +96**, entre `$LC9` y la primera vtable (el
objetivo tiene `_vt.13SizeOperation` en 0x80413AE0 y a nosotros nos cae en
0x80413ADC). Cuatro bytes a cero: la firma de una cadena vacía rellenada.

---

## 5. Lo demás que queda medido y NO cerrado

| unidad | B | lo que falta, ya contado |
|---|---|---|
| `filesys` | 10.872 | 4 B en `AddToQueue` (el `mr.`+`beq` del `if (node != 0)` del `operator++`, que GCC pliega porque sabe que `current != 0`; probados barrera selectiva `+r`, vista por `ListNodeS`, iterador explícito y `Itr.node`: 248, 256, 248, 248), 4 B de `.rodata`, y la frontera `.sbss` con `hlafile` (0x804FFD40..**0x804FFD48**, hoy `hlafile` empieza en 0x804FFD44) |
| `rcmpbase` | 1.184 | **objeto EXACTO** (orden y tamaños). Bloqueado sólo por `lbl_80410120` (0x80410120..0x80410154), que se come la frontera del rango `0x80410148..0x80410188`. Es el **mismo símbolo** que bloquea `bigswizzler` |
| `pathtrack` | 2.472 | rango **S** verificado inocuo; quedan **8 B de marco**: el objetivo abre `stwu r1,-40` con `stfd f31,0x20`/`stmw r30,0x18`/`stw r0,0x2c` y nosotros `-32`/`0x18`/`0x10`/`0x24`. Son 8 B de locales que **el objetivo reserva y no toca** (todos los accesos a pila son a `0x8(r1)` en los dos). Probadas 3 formas de local muerta: GCC las borra. Y el mapa de líneas dice que el original tenía la función en la **línea 85** (nosotros en la 34): faltan ~50 líneas de fichero |
| `pathbank` | 2.156 | 36 B en diez palabras seguidas (`0x05CC..0x05F0`); rango `0x8041399C..0x80413A40` bloqueado por `lbl_80413A2C` |
| `gc_interface` | 6.152 | con `-fnii` 6.352; 6 símbolos de más y el renombrado del `__static_initialization_and_destruction_0`. `.rodata` 712 B **no localizable** (todo reubicaciones) |
| `dvd_device` | 2.704 | con `-fnii` 2.680 (24 B de menos), orden OK; rango **B4** ya verificado en el control |
| `interface` | 204 | con `-fnii` 260; 7 símbolos de más |
| `avplayer` | 4.224 | `.text` **idéntico**; le falta el rango de `.rodata` (72 B, no localizado) |
| `pathnode` | 4.204 | `.text` −8 B; `.rodata` 32 B con 3 coincidencias (0x80413A48 es la buena) |
| `itoa` | 1.548 | `.text` **idéntico**; rango `.rodata` **A2** ya verificado; falta el `.bss` de 164 B (`str`, sin nombre en el ELF) |
| `vfprintf` | 6.948 | `.text` **idéntico**; falta `.bss` 128 (`pch`) y `.sdata` 8 |
| `math_support` | 2.324 | `.text` **idéntico**; `.rodata` 224 y `.sdata2` 152 **no aparecen** en el ELF |
| `bigyuvswizzler` | 2.212 | `.text` **idéntico**; `.rodata` 80 B no aparece |
| `sdspmix`, `stagpat`, `svol`, `ssysinit`, `sserver`, `sstvol`, `quantize`, `inittmr` | | `.text` **idéntico**; les falta mover una frontera (la ventana entre vecinos es de ancho cero) |

---

## 6. Lo que NO he probado

1. **`-fno-implement-inlines` sobre 31 de las 33 SourceLists.** Sólo he catado
   `zBWare` y `zFoundation` (las dos mejoran). Es donde puede haber otro salto
   grande, y también donde puede romper el enlace: la bandera deja indefinido
   cualquier inline no virtual que no se integre en algún llamante. **Con las
   SourceLists en `NonMatching` eso no se ve hasta promocionarlas.**
2. **Las cinco `libc` con el sesgo int→double** (§3.7): diagnosticadas al byte,
   ninguna tocada. ~7.800 B.
3. **`ppc2D2`**: no he escrito los siete thunks (son ensamblador) ni tocado las
   otras cuatro funciones ni el orden.
4. **Las particiones de `lbl_80410120`, `lbl_80413A2C`, `lbl_8040FE38`,
   `lbl_80413A7C` y `lbl_80413EC8`**: las cinco bloquean un rango y ninguna se
   ha intentado esta ronda (las tres primeras están descartadas desde la r28).
5. **`audit.py` / `frozen.py`**: no los he pasado. El DOL byte a byte es prueba
   más fuerte, pero **hay que pasarlos al aplicar**, porque el reparto propuesto
   no está en el árbol.
6. **El `.bss` de `itoa` y `vfprintf`**: sus estáticos (`str`, `pch`) no tienen
   nombre en el ELF original, así que `c29lk_locate` no los ve. Quedan por
   descarte contra el hueco `0x804B4DE8..0x804BA040`.
7. **De dónde sale el `"%s  %s"`** de las once TU de `path`. Sigue abierto.
8. **Los 4 B de `.data` de `snddrv`** por la vía buena (deuda de la r28).

---

## 7. Deuda declarada que dejo

- **Ninguna nueva de fuente.** Las construcciones de esta ronda (`= 0` sobre las
  virtuales puras, `#pragma interface`/`implementation`, `inline` en
  `FILESYSINFO::~FILESYSINFO`, quitar `PATH_REAL_EMIT_METHODS`) son mecanismos
  del compilador que el original usaba; ninguna es una falsificación.
- **Se retira ensamblador**: el bloque `__asm__` de `maddeca.cpp` (10 dobles con
  nombres `lbl_*`) — pero **sólo se puede retirar con el cambio de `splits.txt`
  del grupo A1**, así que va en el paquete y el árbol se deja como estaba.
- Sigue la deuda de la r28: los cuatro `static inline` del literal muerto en
  `path` y el `aligned(8)` de `araminited`.

---

## 8. Anexo: las 26 unidades cuyo `.text` YA es idéntico (45.992 B)

`c29lk_ready.py` (con el árbol tal cual, **sin** `-fno-implement-inlines`).
Tres de ellas son las que promocionan esta ronda; las otras 23 están bloqueadas
**sólo por un rango de datos**. Es la lista de trabajo del frente de enlace.

```
  6948  libc/vfprintf                             .bss 128/0; .sdata 8/0
  5728  .../csis/dev/source/library/cmn/csis      .data 28/0; .bss 28/0        <- promociona (B2)
  3268  .../vp6/.../decode/cmn/postproc           .rodata 80/0; .bss 0/768     <- promociona (A1)
  2340  .../snd/9/.../cmn/stagpat                 .rodata 16/0; .data 0/4; .bss 4/0
  2324  libc/math_support                         .rodata 224/0; .sdata2 152/0
  2212  egami/rcmp/.../vd/gc/bigyuvswizzler       .rodata 80/0
  2184  libc/k_rem_pio2                           .sdata 64/0; .sdata2 136/0
  2180  libc/kf_rem_pio2                          .sdata 40/0; .sdata2 84/0
  2120  .../snd/9/.../gc/sdspmix                  .rodata 4/0; .bss 5636/5656
  2000  libc/e_pow                                .sdata 272/0; .sdata2 48/0
  1796  .../snd/9/.../cmn/satospkr                *** TODO IGUAL ***
  1780  libc/ef_pow                               .sdata 136/0; .sdata2 24/0
  1668  .../snd/9/.../cmn/ssysinit                .rodata 12/0; .data 20/16; .bss 28/32
  1548  libc/itoa                                 .rodata 64/0 (A2 ya verificado); .bss 164/0
  1208  egami/rcmp/.../vd/gc/bigswizzler          .rodata 24/0
   992  .../snd/9/.../cmn/sserver                 .rodata 4/0; .data 8/12
   896  .../vp6/.../decode/cmn/quantize           .rodata 1280/1320; .sbss 0/4
   868  libc/e_rem_pio2                           .sdata 88/0; .sdata2 392/0
   848  libc/ef_rem_pio2                          .sdata 56/0; .sdata2 920/0
   640  libc/sf_log                               .sdata 8/0
   588  .../snd/9/.../cmn/sstvol                  .rodata 40/480
   532  libc/kf_tan                               .sdata 8/0
   472  .../realcore/.../system/gc/inittmr        .rodata 24/0; .sdata 4/8; .sbss 4/0
   424  .../snd/9/.../cmn/svol                    .rodata 16/0
   252  .../snd/9/.../cmn/sstfxlev                .rodata 16/0                 <- promociona (A3)
   176  libc/libgcc2_4                            .rodata 24/0
```

**`satospkr` (1.796 B) está a UNA frontera**: su objeto es idéntico en todas las
secciones, pero al sustituirlo el enlace pierde `lbl_80412CC2` — un literal que
vive en el rango `.rodata` de `satospkr` (0x80412C70..0x80412CC8) y que
**referencia el `stagpat.o` extraído**. O la frontera con `sdownmix`
(0x80412CC8) está mal, o hay que dejar la cola en un comodín.

Y con `-fno-implement-inlines` se suman a esta lista `rcmpbase` (1.184 B,
bloqueado por `lbl_80410120`) y `dvd_device` queda a 24 B.
