# Ronda 30 — enlace: el paquete revalidado y **cinco promociones nuevas**

Base: `linked` 443/599, `main.dol: OK` (`9619ba57`). Árbol de HOY, con las siete
funciones cerradas y los 11.264 B de `.rodata` de la r29 ya dentro.

**Control puro sobre el árbol de hoy, 599 objetos: DOL OK.** El banco reproduce.

## Resumen

| | B | estado |
|---|---|---|
| paquete de la r29 (5 unidades) | **10.028** | **revalidado hoy**, suelto y junto |
| `satospkr` | **1.796** | **NUEVO** — 2 líneas de `add_relocations` |
| `itoa` | **1.548** | **NUEVO** — el `.bss` que faltaba, localizado por nombre |
| `quantize` | **896** | **NUEVO** — `-fno-common` + recortar 40 B de cola |
| `sstvol` | **588** | **NUEVO** — reparto del bloque de 480 B |
| `svol` | **424** | **NUEVO** — idem |
| **total** | **15.280** | **599 → 604 objetos, DOL OK byte a byte** |

Las diez juntas, sobre objetos borrados y reconstruidos (`build_direct.py --all`,
523 unidades, 0 fallos): **DOL OK**.

---

## 1. El paquete de la r29: REVALIDADO sobre el árbol de hoy

Grupo a grupo, cada uno con su promoción, y todos juntos:

| grupo | promoción | B | resultado |
|---|---|---|---|
| A1 | `postproc` (`-fno-common`) + `maddeca` recortado | 3.268 | **DOL OK** |
| A3 | `sstfxlev` | 252 | **DOL OK** |
| B1 | `pathreal` (`-fno-implement-inlines`) | 496 | **DOL OK** |
| B2 | `csis` | 5.728 | **DOL OK** |
| B5 | `gc_device` (`-fno-implement-inlines`) | 284 | **DOL OK** |
| A1,A2,A3,S,B1,B2,B4,B5 | **sin promocionar nada** | — | **DOL OK** (control) |
| A1,A2,A3,S,B1,B2,B4,B5 | **las cinco juntas** | 10.028 | **DOL OK** |

Las cinco tienen el `.text` **byte a byte idéntico** con las reubicaciones
enmascaradas (`c30lk_secs.py`): 5.728 + 3.268 + 496 + 284 + 252 = **10.028 B**.

El recuento de comodines del control coincide **exactamente** con el que dio la
r29: **6 nuevos y 5 retirados**, 599 → 600 objetos.

### 1-bis. El aviso de `maddeca`, reproducido

`maddeca.cpp` recortado **sin** el grupo A1:

```
CTRL use:maddeca   FALLA  postproc.c(1): error: L0039:Reference to
                          undefined symbol lbl_80411458 in file ...
```

Confirmado: **el cambio de fuente y el grupo A1 van juntos o no van**. Al revés
sí vale — A1 en `splits.txt` **con el `maddeca.cpp` intacto da DOL OK**, así que
`splits.txt` se puede aplicar primero y la fuente después.

**El árbol queda como estaba**: he hecho el recorte, compilado el `.o` a mi caja
y restaurado el fichero en el mismo comando; `md5` antes y después idénticos y
`git status` limpio para `maddeca.cpp`. **No dejo ningún fichero que rompa el
enlace.**

---

## 2. `-fno-implement-inlines`: la propuesta, UNIDAD A UNIDAD

Barrida sobre **las 117 unidades `.cpp` del middleware** (la r29 catató 22) y
sobre **las 33 SourceLists** (la r29 catató 2).

### 2.1 Middleware: 12 cambian, **0 regresan**

`.text` nuestro → con la bandera, contra el objetivo; «sob» = símbolos de más,
«fal» = símbolos que faltan, «ord» = el orden casa.

| unidad | antes | `-fnii` | objetivo | sob | fal | ord | |
|---|---|---|---|---|---|---|---|
| `interfaceimp` (realmemcard) | 1040 | **896** | 896 | 3 → 0 | 0 | sí | **EXACTA** |
| `pathreal` | 532 | **496** | 496 | 1 → 0 | 0 | sí | **EXACTA** |
| `hd_device` | 888 | **820** | 820 | 1 → 0 | 0 | sí | **EXACTA** |
| `gc_device` | 376 | **284** | 284 | 2 → 0 | 0 | sí | **EXACTA** |
| `rcmpbase` | 1260 | **1184** | 1184 | 3 → 0 | 0 | sí | **EXACTA** |
| `filesys` | 10960 | **10868** | 10872 | 9 → 0 | 0 | sí | MEJORA |
| `dvd_device` | 2760 | **2680** | 2704 | 1 → 0 | 0 | sí | MEJORA |
| `gc_interface` (realmemcard) | 6492 | **6352** | 6152 | 12 → 6 | 1 | no | MEJORA |
| `interface` (realcore) | 404 | **260** | 204 | 13 → 7 | 0 | no | MEJORA |
| `rcmp_mad_codec` | 3040 | **2780** | 2592 | 6 → 3 | 0 | no | MEJORA |
| `rcmp_vp6_codec` | 3140 | **2880** | 2828 | 4 → 1 | 0 | no | MEJORA |
| `gc_blockcalculator` | 384 | **364** | 312 | 2 → 1 | 0 | no | MEJORA |

**Las otras 105 no mueven un byte.** Ni una regresión, y **ningún UND nuevo**.

**Dos hallazgos que la r29 no tenía** (sólo barrió 22 unidades):
`interfaceimp` de realmemcard y `hd_device` pasan a **EXACTAS**. Las dos **ya
estaban promocionadas**, así que el `.text` de más que tenían se lo comía
`-strip-unused-data` en silencio. **Verificado con el enlace: las dos con la
bandera, sueltas y juntas, dan DOL OK.**

Y `interfaceimp` con la bandera casa **en todas sus secciones** (896 = 896,
`.rodata` 504 = 504, `.sdata` 4 = 4).

**Propuesta**: `-fno-implement-inlines` en los cflags de `realcore`, `path`,
`csis`, `realmemcard` y `rcmp`. Las 117 unidades `.cpp` del middleware están
barridas una a una: **12 mejoran, 105 no cambian, 0 empeoran**. Para `path` y
`realcore` la bandera es **condición** de las promociones B1 y B5.

### 2.2 SourceLists: **la extrapolación de la r29 es FALSA**

La r29 catató `zBWare` y `zFoundation` (las dos ganan) y concluyó «en las
SourceLists también gana». Barridas las 33:

| | nº | |
|---|---|---|
| MEJORA | **10** | `zAttribSys`, `zBWare`, `zCamera`, `zEAXSound2`, `zEcstasy`, `zFe2`, `zFoundation`, `zMisc`, `zPhysics`, `zSpeech` |
| IGUAL | **10** | `zDebug`, `zDynamics`, `zGameModes`, `zLua`, `zMiscSmall`, `zMission`, `zOnline`, `zPlatform`, `zRender`, `zTrack` |
| **PEOR** | **13** | `zAI`, `zAnim`, `zEAXSound`, `zEagl4Anim`, `zFEng`, `zFe`, `zFeOverlay`, `zGameplay`, `zMain`, `zPhysicsBehaviors`, `zSim`, `zWorld`, `zWorld2` |

**Y 12 de las 13 PEOR introducen símbolos INDEFINIDOS** (de 1 a 7): es
exactamente el riesgo que la r29 anunció sin poder medirlo — la bandera deja sin
emitir un inline no virtual que ningún llamante integra.

Ejemplos: `zPhysicsBehaviors` **UND+7**, `zEagl4Anim` **UND+5** (y de paso
*pierde* 5 símbolos que el objetivo sí tiene), `zGameplay`/`zSim`/`zWorld`
**UND+3**.

Un caso llamativo a favor: **`zAttribSys` pasa a 48.776 = 48.776 con cero
símbolos de más** — tamaño exacto del objetivo.

**Propuesta**: **NO poner la bandera a nivel de biblioteca ni en `cflags_base`.**
Ponerla **por `Object(...)`** en las 10 MEJORA del middleware + las 5 EXACTAS, y
en las 10 SourceLists MEJORA sólo si se quiere el porcentaje; **nunca** en las 13
PEOR. `zOnline` es la única SourceList que está en el enlace (124 B, `.over`) y
sale **IGUAL**, así que la bandera no la toca.

**Así se concilia con la r28**: la r28 midió la bandera **de biblioteca entera**
(«quita 43.488 B, rompe 32 funciones»); ese daño sale de las unidades PEOR
mezcladas con las buenas. **Por unidad gana o no hace nada; en bloque rompe.**

**Aviso de medida**: `c29lk_cc.py` **no aplica `rename_section.py`**, así que
`zFeOverlay` y `zOnline` salían con `.text` en vez de `.over` y su fila era
basura. Re-medidas a mano tras el renombrado: `zFeOverlay` 149.880 → 149.000
(objetivo 141.224), sob 104 → 73 pero **fal 0 → 3** = PEOR; `zOnline` 124 = 124 =
IGUAL.

---

## 3. `satospkr` (1.796 B): era **una reubicación mal leída por dtk**

La r29 lo dejó «a una frontera». No era una frontera:

- Nuestro `.rodata` de `satospkr` es **byte a byte idéntico** al extraído (88 B).
- El extraído tiene **19** símbolos; nosotros **18**. El de más es
  `lbl_80412CC2`, en el desplazamiento **0x52** — dentro de un `float`.
- Quien lo referencia es `stagpat.o`, con un par `lis`/`addi` en 0x803654C0.
  **Nuestro `stagpat` pone ahí `_3Snd.gAzimuthFoldDownLut-6`**, y
  `gAzimuthFoldDownLut` está en 0x80412CC8: `0x80412CC8 - 6 = 0x80412CC2`.

O sea: **dtk resolvió una referencia con desplazamiento NEGATIVO inventando una
etiqueta dentro del `.rodata` del vecino.** La cura son dos entradas de
`add_relocations` en `config/GOWE69/config.yml`:

```yaml
- source: .text:0x803654C0
  type: ha
  target: _3Snd.gAzimuthFoldDownLut
  addend: -6

- source: .text:0x803654C4
  type: l
  target: _3Snd.gAzimuthFoldDownLut
  addend: -6
```

**Verificado**: la corrección sola (sin promocionar) da **DOL OK**; con
`satospkr` promocionada, **DOL OK**. Ni `splits.txt` ni `keep.lst` ni fuente.

**Barrido general** (`c30lk_lblfix.py`): de los 30 `lbl_` de datos que un objeto
extraído referencia desde otro, **éste es el ÚNICO** con el patrón
«símbolo con nombre + addend». Los otros 28 son `$LC` — literales que el
objetivo atribuye a la unidad vecina, que es otro problema (§4).

---

## 4. `sstvol`, `svol`: repartir el bloque de 480 B

`sstvol` tiene en `splits.txt` un `.rodata` de 480 B (0x80412908..0x80412AE8) que
en realidad contiene los literales de varias TU. Medido:

| dirección | bytes | de quién |
|---|---|---|
| 0x80412908..0x80412930 | 40 | **`sstvol`** (`gChannelToVoiceIndexLut` + 1 float) |
| 0x804129E0..0x804129F0 | 16 | `stagpat` (idénticos a los nuestros) |
| 0x804129F0..0x80412A00 | 16 | **`svol`** (idénticos a los nuestros) |

Y el **orden de enlace lo permite**: `sstvol` 344 < `ssysinit` 346 < `stagpat`
349 < `svol` 353 < `mpegl3base` 356, con las direcciones crecientes.

Dos obstáculos, los dos resueltos:

1. **dtk rechaza cortar dentro de un símbolo**: `lbl_8041292C` mide 0x88 y
   `lbl_804129FC` mide 0xEC, y se tragan los huecos. Hay que **partirlos en
   `symbols.txt`** (`lbl_80412930` de 0x84 y `lbl_80412A00` de 0xE8).
2. **`-strip-unused-data` se lleva los comodines nuevos**: sin entrada en
   `keep.lst` el DOL sale **224 B corto**. Con
   `auto_05_80412930_rodata.o:lbl_80412930` y
   `auto_05_80412A00_rodata.o:lbl_80412A00`, **DOL OK**.

`sstvol` (588 B) y `svol` (424 B) promocionan, sueltas y juntas.

`stagpat` (2.340 B) **no**: además del `.rodata` le falta `.data` 0 contra 4 y le
sobra `.bss` 4 contra 0 — 4 bytes que en el original están **inicializados** y en
nuestra fuente no. Es una corrección de fuente, no de rangos.

---

## 5. `itoa` (1.548 B) y `quantize` (896 B)

- **`itoa`**: la r29 lo daba bloqueado porque su estático `str` «no tiene nombre
  en el ELF». `c29lk_locate.py` **sí lo encuentra**: `.bss` 164 B →
  **0x804B53AC..0x804B5450, COHERENTE**. Con el rango A2 (`.rodata`) que ya
  estaba verificado y este `.bss`: **DOL OK**.
- **`quantize`**: con **`-fno-common`** su `.sbss` pasa a 4 = 4, y el `.rodata`
  del reparto (1.320 B) sobra 40 respecto a los 1.280 nuestros. Recortando a
  `0x804114A8..0x804119A8` y dejando la cola en un comodín: **DOL OK**.
  `mk_keep` reasigna `lbl_804119A8` de `quantize.o` al comodín él solo.

---

## 6. El paquete: qué hay que aplicar

### `config/GOWE69/splits.txt` — 16 líneas (diff exacto)

```
libc/itoa.c:                       + .rodata start:0x8040FB78 end:0x8040FBB8   (A2)
                                   + .bss    start:0x804B53AC end:0x804B5450   (A2b, NUEVO)
egami/.../cmn/maddeca.cpp:           .rodata end:0x804114A8 -> 0x80411458      (A1)
Packages/vp6/.../cmn/postproc.c:   + .rodata start:0x80411458 end:0x804114A8   (A1)
Packages/vp6/.../cmn/quantize.c:     .rodata end:0x804119D0 -> 0x804119A8      (F1, NUEVO)
.../snd/9/.../cmn/sstfxlev.c:      + .rodata start:0x804128D8 end:0x804128E8   (A3)
.../snd/9/.../cmn/sstvol.c:          .rodata end:0x80412AE8 -> 0x80412930      (G1, NUEVO)
.../snd/9/.../cmn/svol.c:          + .rodata start:0x804129F0 end:0x80412A00   (G1, NUEVO)
.../csis/.../cmn/csis.cpp:         + .data   start:0x80451E88 end:0x80451EA4   (B2)
                                   + .bss    start:0x804CC6D0 end:0x804CC6EC   (B2)
.../path/.../cmn/pathreal.cpp:     + .rodata start:0x80413168 end:0x80413258   (B1)
                                   + .bss    start:0x804CC6EC end:0x804CC6F8   (B1)
                                   + .sdata  start:0x804FF604 end:0x804FF608   (B1)
.../path/.../cmn/pathtrack.cpp:    + .rodata start:0x80413768 end:0x804138A0   (S)
.../realcore/.../file/gc/dvd_device.cpp: + .rodata start:0x80413DB0 end:0x80413E30 (B4)
.../realcore/.../input/gc/gc_device.cpp: + .rodata start:0x80414C90 end:0x80414CE0 (B5)
```

### `config/GOWE69/symbols.txt` — 2 símbolos partidos (4 líneas)

```
lbl_8041292C = .rodata:0x8041292C; // type:object size:0x4  align:4 data:float   (era 0x88)
lbl_80412930 = .rodata:0x80412930; // type:object size:0x84 align:4 data:float   (NUEVO)
lbl_804129FC = .rodata:0x804129FC; // type:object size:0x4  align:4 data:float   (era 0xEC)
lbl_80412A00 = .rodata:0x80412A00; // type:object size:0xE8 align:4 data:float   (NUEVO)
```

### `config/GOWE69/keep.lst` — 2 entradas nuevas

```
auto_05_80412930_rodata.o:lbl_80412930
auto_05_80412A00_rodata.o:lbl_80412A00
```

(`lbl_804119A8` se reasigna solo de `quantize.o` al comodín; `mk_keep` lo hace.)

### `config/GOWE69/config.yml` — 2 entradas de `add_relocations`

Las de §3.

### `configure.py` — comodines: **10 añadir, 5 quitar** (599 → 604)

| añadir | detrás de |
|---|---|
| `auto_05_804119A8_rodata` | `quantize.c` |
| `auto_05_80412930_rodata` | `sstvol.c` |
| `auto_05_80412A00_rodata` | `svol.c` |
| `auto_05_804138A0_rodata` | `pathtrack.cpp` |
| `auto_05_80413E30_rodata` | `dvd_device.cpp` |
| `auto_05_80414CE0_rodata` | `gc_device.cpp` |
| `auto_06_80451EA4_data` | `csis.cpp` |
| `auto_07_804B5450_bss` | `itoa.c` |
| `auto_07_804CC6F8_bss` | `pathreal.cpp` |
| `auto_08_804FF608_sdata` | `pathreal.cpp` |

Quitar: `auto_05_804128D8_rodata`, `auto_05_80413168_rodata`,
`auto_05_80413768_rodata`, `auto_06_80451E88_data`, `auto_07_804CC6D0_bss`.

### `configure.py` — banderas

- **`-fno-implement-inlines`** en `realcore`, `path`, `csis`, `realmemcard` y
  `rcmp` (§2.1). **NO en `cflags_base` ni en las SourceLists** (§2.2).
- **`-fno-common`** en `Packages/vp6/1.0.6/source/decode/cmn/postproc.c` **y en
  `.../quantize.c`**.

### `configure.py` — marcar `Matching`

`csis.cpp`, `postproc.c`, `pathreal.cpp`, `gc_device.cpp`, `sstfxlev.c`,
`satospkr.c`, `itoa.c`, `svol.c`, `sstvol.c`, `quantize.c`.

### `src/` — un solo cambio

`src/egami/rcmp/dev/source/decoder/cmn/maddeca.cpp`: borrar las líneas **44-133**
(el `.align 3` que sigue a `zigzag` y las diez etiquetas `lbl_80411458 …
lbl_804114A0`), dejando el `"\t.previous\n"`. **Va con el grupo A1.**

---

## 7. La trampa que me costó cuatro enlaces

Dejé el `symbols.txt` **de la caja de pruebas** parcheado de un ensayo anterior
y las cuatro medidas siguientes salieron **ROTAS con el mismo hash** — incluido
el grupo A2, que estaba verificado. Se delata porque **el hash del DOL roto es
idéntico entre grupos que no tienen nada que ver**: eso no es tu cambio, es el
banco. Ahora el banco tiene `--syms <fichero>` y el `symbols.txt` base se queda
intacto.

Y el síntoma de un comodín sin `keep.lst`: **el DOL sale exactamente 224 B más
corto** (no falla el enlace).

---

## 8. Herramientas (scratchpad, prefijo `c30lk_`)

| script | qué hace |
|---|---|
| **`c30lk_go.py`** | el banco de la r29 con `--use unidad=obj.o` (sustituye una unidad **esté donde esté**, promocionada o no), `--syms`, `--yamladd` (añade entradas a `add_relocations`) y `--tag`. Grupos nuevos: `A2b`, `E1`, `F1`, `G1`, `D1`, `D2` |
| **`c30lk_fii.py`** | el A/B por unidad de una bandera contra el objetivo: `.text`, símbolos de más/de menos, orden y **UND nuevos**. Salida JSON |
| **`c30lk_lblfix.py`** | los `lbl_` de datos que un objeto extraído referencia desde otro, y **la entrada de `add_relocations` que los sustituye** |
| **`c30lk_order.py`** | orden de ENLACE contra dirección de la sección: dice si un bloque se puede repartir |
| `c30lk_secs.py` | tamaños por sección y si el `.text` es idéntico con relocs enmascaradas |
| `c30lk_rodcmp.py` | compara una sección de datos byte a byte y lista los símbolos de los dos lados |
| `c30lk_own.py` | si una unidad entra en el enlace con nuestro `.o` o con el extraído |
| `c30lk_list.py` | las unidades `.cpp` de middleware y SourceLists |

**Bug heredado corregido**: la `key()` de `c29lk_fii.py` busca por **basename**, y
hay tres repetidos (`gc_interface`, `locale`, `interfaceimp` salen en `realcore`
**y** en `realmemcard`). Medía la unidad equivocada y daba **5 PEOR falsos**
(`gc_interface` «de 1008 a 6352 con UND+77»). `c30lk_fii.py` busca por ruta.

---

## 9. Lo que NO he probado

1. **Las cinco `libc` con el sesgo `int→double`** (~7.800 B): sin tocar. El
   diagnóstico de la r29 sigue en pie y la maquinaria está en el árbol.
   `c29lk_locate` **no localiza** ninguno de sus `.sdata`/`.sdata2` por nombre
   (33 símbolos en `e_pow`, 32 en `ef_pow`…), así que la vía es la fuente.
2. **`ppc2D2`**: los siete *thunks* de 4 B, sin escribir.
3. **Las cinco particiones** `lbl_80410120`, `lbl_80413A2C`, `lbl_8040FE38`,
   `lbl_80413A7C`, `lbl_80413EC8`. Ninguna intentada. **Pero ahora hay receta**:
   §4 demuestra que un `lbl_` que se traga un hueco se **parte en
   `symbols.txt`** y el comodín se sostiene con `keep.lst`. `lbl_80410120`
   (bloquea `rcmpbase`, 1.184 B, **EXACTA con `-fnii`**, y `bigswizzler`,
   1.208 B) es el candidato obvio para la próxima ronda.
4. **`math_support`** (2.324 B): su `.rodata` de 224 B **sí** se localiza
   (0x8040FD30..0x8040FE10, COHERENTE, grupo `E1` ya en el banco), pero su
   `.sdata2` de 152 B no. No promociona todavía. **No he probado el grupo E1.**
5. **`sdspmix`** (2.120 B): `.bss` 5.636 B localizado
   (0x804BED88..0x804C038C, COHERENTE); le falta un `.rodata` de 4 B sin
   localizar. Sin probar.
6. **`stagpat`** (2.340 B): los 16 B de `.rodata` están medidos e idénticos
   (0x804129E0..0x804129F0, grupo `D2` en el banco), pero le faltan 4 B de
   `.data` y le sobran 4 de `.bss` — **corrección de fuente**, no probada.
7. **`ssysinit`** (1.668 B) y **`sserver`** (992 B): sus `.data` se localizan
   (0x80450530..0x80450544 y 0x804502F8..0x80450300, COHERENTES = grupos C1/C2
   del banco de la r29). **No los he probado esta ronda.**
8. **`-fno-implement-inlines` sobre las 10 SourceLists MEJORA**: medido el
   objeto, **no medido el efecto en `report.json`/`matched_code`**. No he pasado
   `measure.py` ni `pctsnap.py` en toda la ronda.
9. **`audit.py` y `frozen.py`**: no los he pasado. El DOL byte a byte es prueba
   más fuerte, pero **hay que pasarlos al aplicar**, porque el reparto propuesto
   no está en el árbol.
10. **De dónde sale el `"%s  %s"`** de las once TU de `path`: sigue abierto. Y
    los 4 B de `.rodata` de `filesys`: sin avance.
11. **El orden de funciones de `gc_interface` y `ppc2D2`**: sin tocar. Con
    `-fnii`, `gc_interface` baja de 12 a 6 símbolos de más pero el orden sigue
    mal.

## 10. Deuda y estado del árbol

- **Ninguna deuda de fuente nueva.** No he escrito ensamblador ni pines.
- **`src/` sin tocar por mí.** El recorte de `maddeca.cpp` se hizo y se
  deshizo en el mismo comando (md5 idéntico, `git status` limpio para ese
  fichero). **No dejo ningún fichero que rompa el enlace.**
- Aviso: durante la ronda **otros agentes han modificado 74 ficheros** bajo
  `src/Speed/Indep/Src/`. Ninguno entra en el enlace (sólo `zOnline` de las
  SourceLists está en él, y sus fuentes no están entre los 74), así que las
  medidas de esta ronda no dependen de ellos — pero **la reconstrucción de
  `build/GOWE69/src` es de las 17:33 de hoy**, anterior a esos cambios.
