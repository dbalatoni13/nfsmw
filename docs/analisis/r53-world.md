# r53 — zWorld / zWorld2

Territorio: 6 funciones (8.568 B) y el `.rodata` de las dos unidades.

**Lo que sale de la ronda: 4.064 B de déficit de `.rodata` cerrados**, el mayor
del frente, con `git status` de `src/Speed/Indep/Src/World/` **vacío** (el único
cambio de fuente son las dos SourceLists) y `fncmp` **idéntico** en las dos
unidades antes y después. Cero bytes de código cerrados; a cambio,
`HolePunchAvoidables` pasa de «una desigualdad» a **la traza de `sched1` insn a
insn, con la condición escrita en POSICIONES y las dos únicas salidas que
quedan**, y la palanca que la r52 dejó apuntada para `InitAtSegment` queda
**probada y refutada con cuatro cifras**.

| unidad | `.text` | resto ANTES | resto con la fuente | resto con la fuente **+ `keep.lst`** |
|---|---|---|---|---|
| zWorld  | +0 | `rodata-4136 data-128` | `rodata-920 data-128` | **`rodata-472 data-128`** |
| zWorld2 | +0 | `rodata-808 data-256 bss+64` | `rodata-576 data-256 bss+64` | **`rodata-408 data-256 bss+64`** |

`linkdelta` base de la ronda en `scratchpad/r53_world/` · `fncmp` antes/después
en `base_fncmp_*.txt` / `final_fncmp_*.txt` (diff vacío).

---

## 1. Parte 1 — el `.rodata` (hecha, medida y sin regresiones)

### 1.1 Lo aplicado a la fuente

Bloque de `prefijotu.py --asm` **al final** de `zWorld.cpp` (138 cadenas,
3.212 B) y de `zWorld2.cpp` (17 cadenas, 239 B). Después de aplicarlo,
`prefijotu` da **0 cadenas ausentes en las dos**.

Rinde `rodata −4136 → −920` y `−808 → −576`. El bloque es anónimo (sólo
`.asciz`, sin `.globl`/`.size`), así que **el estripado no se lo lleva**: los
3.216 B que baja zWorld son exactamente los 3.212 del bloque más el alineado.

`lcfix.py --check zWorld zWorld2`: **todas al día** (las dos unidades no tenían
ninguna entrada `@lc`, y el bloque no desplaza ningún `$LC`, ver §1.3).

### 1.2 Lo que tienes que aplicar tú a `keep.lst`

**zWorld — 25 líneas.** Las 24 primeras son `deadlink.py zWorld --keep`; la 25ª
la encontré aparte y vale ella sola 88 B (§1.4).

```
# zWorld: cadenas muertas que el enlazador se lleva y el objetivo SI tiene (deadlink.py)
# @lc zWorld "Attrib::Gen::pvehicle"
zWorld.o:$LC166
# @lc zWorld "Attrib::Gen::simsurface"
zWorld.o:$LC175
# @lc zWorld "Attrib::Gen::tires"
zWorld.o:$LC230
# @lc zWorld "16.2.1"
zWorld.o:$LC254
# @lc zWorld "1.2.3"
zWorld.o:$LC255
# @lc zWorld "Attrib::Gen::ecar"
zWorld.o:$LC267
# @lc zWorld "VehicleParams"
zWorld.o:$LC269
# @lc zWorld "Attrib::Gen::emittergroup"
zWorld.o:$LC305
# @lc zWorld "DebugWorld"
zWorld.o:$LC462
# @lc zWorld "CarRenderInfo"
zWorld.o:$LC549
# @lc zWorld "VehiclePartDamageBehaviour"
zWorld.o:$LC567
# @lc zWorld "RideInfo"
zWorld.o:$LC692
# @lc zWorld "Remove Duplicates"
zWorld.o:$LC1085
# @lc zWorld "CarLoaderPool"
zWorld.o:$LC1092
# @lc zWorld "TestAlloc"
zWorld.o:$LC1099
# @lc zWorld "Defragment Textures"
zWorld.o:$LC1100
# @lc zWorld "Defragment Solids"
zWorld.o:$LC1101
# @lc zWorld "CarLoaderDefragment"
zWorld.o:$LC1108
# @lc zWorld "Parameter Accessor Blend Buffer"
zWorld.o:$LC1199
# @lc zWorld "ParameterMapLayer"
zWorld.o:$LC1206
# @lc zWorld "Attrib::Gen::visuallook"
zWorld.o:$LC1207
# @lc zWorld "Attrib::Gen::visuallookeffect"
zWorld.o:$LC1208
# @lc zWorld "VisualLookEffect"
zWorld.o:$LC1226
# @lc zWorld "VisualLookEffectTarget"
zWorld.o:$LC1228
# c53wld: el nombre viejo `pad_05_80409D58_rodata` YA NO EXISTE en nuestro objeto
zWorld.o:lbl_80409D58
```

**zWorld2 — 15 líneas** (`deadlink.py zWorld2 --keep`, tal cual):

```
# zWorld2: cadenas muertas que el enlazador se lleva y el objetivo SI tiene (deadlink.py)
# @lc zWorld2 "Attrib::RefSpec"
zWorld2.o:$LC170
# @lc zWorld2 "Attrib::TAttrib"
zWorld2.o:$LC176
# @lc zWorld2 "CollisionInstanceList"
zWorld2.o:$LC235
# @lc zWorld2 "CollisionObjectList"
zWorld2.o:$LC236
# @lc zWorld2 "WCollisionPack"
zWorld2.o:$LC237
# @lc zWorld2 "WCollisionAssets"
zWorld2.o:$LC238
# @lc zWorld2 "WCollisionObject"
zWorld2.o:$LC240
# @lc zWorld2 "WGrid Nodes"
zWorld2.o:$LC328
# @lc zWorld2 "WGrid"
zWorld2.o:$LC329
# @lc zWorld2 "16.2.1"
zWorld2.o:$LC415
# @lc zWorld2 "1.2.3"
zWorld2.o:$LC416
# @lc zWorld2 "WRoadNav CookieTrail"
zWorld2.o:$LC494
# @lc zWorld2 "WRoadNav Path Buffer"
zWorld2.o:$LC496
# @lc zWorld2 "Attrib::Gen::effects"
zWorld2.o:$LC739
# @lc zWorld2 "FastVector"
zWorld2.o:$LC776
```

Medido con un `keep.lst` temporal (`scratchpad/r53_world/mide.py`, que **no toca**
`config/GOWE69/keep.lst`; enlaza el proyecto entero con `keep.lst` + las líneas
extra):

```
zWorld     hoy rodata-920 data-128            +25 lineas -> rodata-472 data-128
zWorld2    hoy rodata-576 data-256 bss+64     +15 lineas -> rodata-408 data-256 bss+64
```

Ninguna de las dos mueve `.text`, `.data`, `.bss` ni ninguna otra sección.

### 1.3 **EL PAQUETE NO ES ATÓMICO** — comprobado, no supuesto

Compilé las dos unidades **con y sin** el bloque `asm()` y comparé la tabla
entera de símbolos `$LC` (nombre + contenido):

    diff lc_sin.txt lc_con.txt  ->  VACIO

O sea: **el bloque no desplaza ni un `$LC`** (va al final del `.cpp` y sus
cadenas son `.asciz` crudas, no literales de C, así que `cc1plus` no les asigna
número). Las 40 líneas de `keep.lst` y los dos bloques de fuente **se pueden
aplicar por separado y en cualquier orden**; los `$LC` que nombran son los
mismos en los dos árboles. Lo que no se puede es esperar el `−472`/`−408` con
sólo una de las dos mitades.

### 1.4 El hallazgo suelto: `keep.lst` nombraba un símbolo que ya no existe

`zWorld.o:pad_05_80409D58_rodata` (línea 601 de `keep.lst`) es el nombre que le
daba el troceador al prefijo de bWare. Pero `zWorld.cpp` lo escribe a mano desde
hace rondas y lo llama **`lbl_80409D58`** (`.globl lbl_80409D58`), así que la
entrada no casa con nada y **el enlazador se lleva 88 B** (`size & ~7` de 92).
`zWorld2` sí tiene la línea buena (`zWorld2.o:lbl_8040DFE8`, línea 791).

Se ve con la sonda `scratchpad/r53_world/probe_strip.py`, que lista los símbolos
de `.rodata` que el estripado se lleva **y que `deadlink` NO propone** (porque
sólo propone `$LC`). Vale la pena pasarla por las otras unidades: es una clase
de fallo silencioso que `keepchk.py` da por buena porque el símbolo del nombre
viejo no existe y el del nuevo sí.

### 1.5 Lo que NO vale (medido)

- **La vtable `_vt.Q33UTL11Collectionst8_Storage2ZP17VehicleRenderConni10`** (64 B,
  se estripa hoy). Salvarla da `rodata −472 → −408` **pero `text +180`**: el
  símbolo mantiene vivas sus virtuales. Es el caso de `zLua` del docstring de
  `deadlink`, otra vez. **Rechazada.** Además el objetivo no tiene ese nombre:
  tiene `_vt.Q23UTLt6Vector2ZP17VehicleRenderConni16` y
  `_vt.Q43UTL11Collectionst8Listable2Z17VehicleRenderConni10_4List`, o sea que
  es materia de `mangfix.py`, no de `keep.lst`.
- Los otros 12 símbolos estripados de zWorld y los 7 de zWorld2 que `deadlink`
  descarta son **correctos**: son cadenas (`Attrib::Gen::chassis`,
  `Attrib::Gen::transmission`, `EventSequencerSystems`…) que el objeto objetivo
  **no tiene**, más dos `$LC` de 4 B (`1.0f` y `0`) cuyo `size & ~7` es 0.

### 1.6 Lo que queda de `.rodata`, con la cifra

| | zWorld | zWorld2 |
|---|---:|---:|
| `.rodata` del OBJETO, nuestro − objetivo | **−192 B** | **−160 B** |
| déficit del ENLACE con el paquete puesto | −472 B | −408 B |
| ⇒ se estripa de más | 280 B | 248 B |

O sea que lo que queda **no es una sola cosa**: faltan 192/160 B de contenido que
el objetivo tiene y nosotros no emitimos (`rodatagaps` los reparte en 32 huecos
PREFIJO de zWorld y 21 de zWorld2 — pools de flotantes muertos entre funciones,
`genrodata … --anonimo`), y además se estripan 280/248 B que el objetivo
conserva. **No lo he tocado**: son huecos que hay que escribir uno a uno en su
sitio (entre dos definiciones de función del `.cpp`) y el frente barato ya está
agotado.

**Y el ORDEN sigue abierto**, que es lo que de verdad separa de la promoción:

    zWorld   objetivo 637 cadenas, nuestro 645, en secuencia 470 (fuera 167)
    zWorld2  objetivo  93 cadenas, nuestro 100, en secuencia  58 (fuera  35)

El bloque de `prefijotu` va al final, así que sus 138/17 cadenas están todas
fuera de sitio por construcción. La palanca es el primer de pool (§brief), y
**sólo mueve hacia atrás**, o sea que las filas `delete` de `rodorden` son las
accionables. No es trabajo de esta ronda pero es el siguiente escalón.

---

## 2. `HolePunchAvoidables` (2.980 B, 4 filas, 99,97315 %)

Las 4 filas siguen siendo el swap cr2/cr3 y **nada más**: filas 176 y 178
(`cmpwi crX,r29` = is_racer, `cmpwi crY,r31` = is_drag) y sus dos `beq` (241 y
una más abajo). Las posiciones son IDÉNTICAS a las del objetivo; sólo cambia el
registro CR.

### 2.1 La traza de `sched1`, insn a insn (herramienta nueva)

`-fsched-verbose-5` sobre la TU reducida escupe 18,8 MB a stderr y **no lleva
cabeceras de función**; el modo de localizar el bloque es: sacar los UID de las
tres comparaciones del `.lreg` (`-dl`) y buscarlos en el volcado. Los UID son
estables entre pases. Sonda: `scratchpad/r53_world/sv.py`.

El preencabezado de `HolePunchAvoidables` es **un bloque de 47 insns en 24
ciclos** (`ISSUE_RATE` 2). Cola del horario, tal cual sale del volcado:

```
 t=13   3319 lsu        3338 iu2
 t=14   3335 lsu        3254 [iu   <== RACER  (reg:CC 334)   posicion 27
 t=15   3341 lsu        3342 [iu   <== DRAG   (reg:CC 901)   posicion 29
 t=16   3325 lsu        3235 iu2
 t=17   3236 iu2        3237 iu2
 t=18   3238 iu2        3239 iu2
 t=19   3253 iu2        3268 iu2
 t=20   3283 [iu  <== TRAFFIC (reg:CC 505) posicion 38        3303 iu2
 t=21   3304 iu2        3305 iu2
 t=22   3306 iu2        3307 iu2
 t=23   3320 iu2        3321 iu2
 t=24   3343 iu2                                    (fin del bloque: posicion 46)
```

Y la lista de listos, ordenada de peor a mejor, en t=14:

```
3343 3321 3320 3307 3306 3305 3304 3303 3283 3268 3253 3239 3238 3237 3236 3235 3342 3254 3325 3341 3335
                                          ^traffic                          ^drag ^racer  <-- mejor -->
```

Se lee entero con la escalera de `rank_for_schedule` (leída del árbol,
`orig/prodg/NGC_GNU_SRC/NGC/gcc/haifa-sched.c:4158`):

1. `INSN_PRIORITY` (mayor primero) — los `movsf`/`lfs` valen 2, los `elf_high`
   con dependiente 3, y **las tres comparaciones y todos los `addi`/`high` sin
   dependiente valen 1**.
2. `INSN_REG_WEIGHT` (menor primero, sólo con `!reload_completed`) — racer y
   drag pesan **0** (su operando muere ahí), los `addi r31+K` y traffic pesan
   **1**. Por eso las dos comparaciones se cuelan delante de los nueve `addi`.
3. clase respecto al último programado.
4. **nº de dependientes (más gana)** — el escalón que faltaba en mi cabeza:
   *cualquier insn del preencabezado con un dependiente EN el preencabezado
   adelanta a `drag`*.
5. `INSN_LUID` (menor gana) — y es el que ordena el grupo de peso 1:
   3235 < 3236 < … < 3253 < 3268 < 3283(traffic) < 3303 < … < 3343.

### 2.2 La condición, ahora en POSICIONES (y es más estrecha de lo que parecía)

Del volcado sale la aritmética exacta:

    live(x) = T + cuenta(x),   T = 475 (insns del cuerpo del bucle)
    cuenta(x) = insns del bloque que van DESPUES de def(x)

    racer   pos 27 -> cuenta 19 -> live 494 -> pri int(30000/494) = 60
    drag    pos 29 -> cuenta 17 -> live 492 -> pri int(30000/492) = 60   EMPATE
    traffic pos 38 -> cuenta  8 -> live 483 -> pri int(30000/483) = 62

`allocno_compare` desempata por **número de allocno menor**, y son 334 (racer),
505 (traffic), 901 (drag). El objetivo necesita el orden de reparto
**traffic → drag → racer**. O sea que basta con

> **`pri(drag) > pri(racer)`**, y con `pri(traffic) ≥ pri(drag)` vale el empate
> (505 < 901 desempata a favor de traffic).

`int(30000/L)` vale 61 para `L ∈ [484,491]` y 60 para `L ∈ [492,500]`. Con
`T = 475`:

> **`cuenta(drag) ≤ 16` y `cuenta(racer) ≥ 17`.** Hoy son 17 y 19.

Y de ahí salen las DOS únicas salidas, con la demostración:

- **Añadir insns al preencabezado NO sirve.** Un insn nuevo entre racer y drag
  sube `cuenta(racer)` a 20 (live 495 → sigue 60) y deja `cuenta(drag)` en 16
  **sólo si empuja a drag un CICLO entero** (dos posiciones). Si sólo lo empuja
  una, drag se queda en 17. Para invertirlo por el lado de racer harían falta
  **7** insns (live(racer) ≥ 501).
- **Mover un insn del bucle al preencabezado (o al revés) NO sirve**:
  `live = T + cuenta` y las dos mitades se compensan exactamente.
- **Quitar un insn del bucle SÍ sirve** (`N = −1`): `live 493/482/491`,
  `pri 60/62/61`, orden **T,D,R correcto**, y el empate FP se resuelve también
  (1028/1032 → 97/96). Es lo que hace quitar la barrera. **Comprobado esta
  ronda**: `sin_barrera` da `CC-OK` y `FP-OK`, 2980/2980, **46 filas**.
  Y `N = −1` es el ÚNICO valor que arregla los dos empates: `N = −2` empata el
  FP (1026/1030 → 97/97) y `N = +7/+8` también (95/95).
- **Reordenar el preencabezado sin añadir nada SÍ sirve**: si un solo insn de
  los 17 que van detrás de drag adelantara a drag, éste caería en t=16 (posición
  31, cuenta 15 → live 490 → pri 61) y `racer` no se movería. Por §2.1 eso pide
  **un insn del preencabezado con prioridad ≥ 2 o con un dependiente dentro del
  preencabezado**. Los nueve `addi rN,r31,K` y los dos `elf_high` sin
  dependiente que hay detrás de drag no lo son, y no hay forma de dárselo desde
  la fuente sin emitir instrucciones.

**Y el código está congelado**: la función mide 2.980/2.980 B y su flujo de
instrucciones es idéntico al del objetivo salvo el registro CR, así que `N` está
clavado en 0 y las posiciones también. Por eso el único camino vivo es:

> **quitar la barrera (`N = −1`, que arregla el reparto entero) y sustituirla
> por un ancla de CERO insns para el `fmuls` de `extra_width`.**

### 2.3 Un dato nuevo que cierra media pregunta de la r52

La r52 apuntó, sin medir, que «`REG_LIVE_LENGTH` sólo cuenta insns de clase 'i',
así que las NOTE y los CODE_LABEL no cuentan». **Queda medido**: la variante
`bar_goto` (etiqueta de goto calculado, `&&L`, metida justo donde está la
barrera) da **`L r/t/d = 494/483/492`, idénticos a la base**. Una etiqueta
dentro del bucle **no suma un byte al `live` de nadie**.

Lo malo es la otra mitad: **crear esa etiqueta desde C no sale gratis**. El
`void *p = &&L;` materializa la dirección (`lis`+`addi`) y GCC 2.9 no la borra
aunque `p` esté muerta:

| ensayo | filas | B |
|---|---:|---:|
| `bar_goto` (barrera + etiqueta) | 66 | **2984** (+4) |
| `nobar_goto` (etiqueta en lugar de la barrera) | 77 | **2988** (+8) |

Y encima **la etiqueta no ancla el `fmuls`** ni siquiera cuando llega
(`nobar_goto` sale peor que `sin_barrera`: 77 contra 46). O sea que el corte de
bloque básico, por sí solo, no reproduce lo que hace la dependencia de datos de
la barrera sobre `offset_change`.

### 2.4 Las variantes de esta ronda, todas con su cifra

Todas con `p.py` (live/pri por PAPEL) + recuento de filas con la TU reducida.
Base: `L 494/483/492 p 60/62/60 TRD · FP 1030/1034 p97/96 FP-OK · 4 filas · 2980/2980`.

| ensayo | live r/t/d | pri | orden CC | FP | filas | B |
|---|---|---|---|---|---:|---:|
| **base** | 494/483/492 | 60/62/60 | TRD | OK | **4** | 2980 |
| `sin_barrera` | 493/482/491 | 60/62/61 | **TDR ✔** | **OK** | 46 | 2980 |
| barrera `+ "r"(is_drag)` | 494/484/476 | 60/61/63 | DTR | OK | 64 | **2996** |
| barrera `+ "r"(is_racer)` | 485/483/494 | 61/62/60 | TRD | OK | 51 | **2996** |
| **barrera `+ "r"(is_traffic)`** | 494/483/492 | 60/62/60 | TRD | OK | **4** | **2980** |
| `sin_barrera` + pin `fr2` en `extra_width` | 493/482/491 | 60/62/61 | TDR ✔ | OK | 46 | 2980 |
| `sin_barrera` + el `if(closest_cookie<…)` bajado hasta el ancla | 495/484/493 | 60/61/60 | TRD | **no** | 285 | 2984 |
| barrera + ese mismo `if` bajado | 496/485/494 | 60/61/60 | TRD | **no** | 319 | 2980 |
| `sin_barrera` + `extra_width` delante de `cut_to_position.x` | 493/482/491 | 60/62/61 | TDR ✔ | OK | 66 | 2980 |
| `sin_barrera` + `extra_width = offset_change * 0.2f` | 493/482/491 | 60/62/61 | TDR ✔ | **no** | 82 | **2968** |
| `sin_barrera` + `extra_width = oc - oc*0.8f` | 494/483/492 | 60/62/60 | TRD | **no** | 41 | **2972** |
| `sin_barrera` + `extra_width` fundido en `avoidable_half_width` | 493/482/491 | 60/62/61 | TDR ✔ | OK | 72 | 2980 |
| `sin_barrera` + etiqueta (`&&L`) | 493/482/491 | 60/62/61 | TDR ✔ | OK | 77 | 2988 |
| barrera + etiqueta (`&&L`) | 494/483/492 | 60/62/60 | TRD | OK | 66 | 2984 |

**Veda nueva y útil fuera de este territorio:** *una entrada de más en un `asm`
que ya existe es GRATIS si el valor ya está vivo en un registro en ese punto*.
`asm("" : "+f"(offset_change) : "r"(is_traffic))` da **objeto idéntico** a la
base (4 filas, 2980 B) porque el operando de `is_traffic` ya sobrevive a su
comparación. Con `is_drag` o `is_racer`, que mueren ahí, cuesta **+16 B** y un
preservado. O sea: la palanca «añade la variable como entrada a un `asm` que ya
existe» de la r52 **sólo es gratis cuando no cambia nada**, y la regla operativa
es mirar antes si el operando muere en su uso.

### 2.5 Lo que hay que hacer en la r54, en una frase

Un ancla para el `fmuls` de `extra_width` que **no sea un insn y no sea una
etiqueta**. El diff sin barrera (`scratchpad/r52_world/d_nobar.txt`, filas
522-611) dice exactamente qué pasa: el objetivo hace `lis/addi` del literal 0,2f
en las filas 565/567 y **retrasa el `lfs` a la 588 y el `fmuls f2,f10,f0` a la
590**; nosotros ponemos `lis/addi` en 561/562, el `lfs` en 566 y el `fmuls` en
570 — **22 filas antes**. O sea que lo que hay que atar no es la dirección: es
la **carga** del literal. Ninguna de las cinco formas de la sentencia probadas
esta ronda lo mueve.

---

## 3. `InitAtSegment` (816 B, 12 filas) — la palanca que pedías, **refutada**

La r52 dejó abierto: *«la palanca buena es añadir la variable como entrada
`"r"(x)` a un `asm` que ya existe; eso no se ha probado aquí»*. Probado. La
condición de victoria de la r49 es `pri(212) > pri(176)` en `QTY_CMP_PRI`, con
`176` = el valor de `fNodes` (5 refs / 31 insns → 3225) y `212` = el
`(high $LC472)` del `0.0f` (2 refs / 8 insns → 2500); subir `n_refs(212)` a 3
daría 3750 y ganaría.

`InitAtSegment` tiene **dos** `asm` ya escritos (`asm("" : "=r"(guard))` y
`asm("" : "+f"(endOffset) : "r"(guard))`). Resultados (base: 12 filas, 816 B):

| ensayo | filas | B |
|---|---:|---:|
| base | 12 | 816 |
| `asm("" : "+f"(endOffset) : "r"(guard), "f"(0.0f))` | 27 | **824** |
| `asm("" : "=r"(guard) : "f"(0.0f))` | 23 | **816** |
| `asm("" : "+f"(endOffset) : "r"(guard), "f"(fSegTime))` | **12** | **816** (nulo) |
| `asm("" : "+f"(endOffset) : "r"(guard), "r"(fNodeInd))` | 14 | 820 |
| `asm("" : "+f"(endOffset) : "r"(guard), "f"(fLaneOffset))` | 15 | 820 |
| `asm("" : : : "r0")` delante de `fStartPos` (control r52) | **7** | 816 |

**La palanca no llega.** Meter el `0.0f` como entrada de un `asm` o **lo
materializa de verdad** (+8 B, 27 filas) o **no comparte el `high`** y sólo
revuelve el reparto (816 B pero 23 filas). Y la entrada que sí es gratis
(`"f"(fSegTime)`) es gratis porque **no cambia nada**. Queda en pie lo de la
r52: el único ensayo que baja de 12 a 7 filas es el clobber de `r0`, y sigue sin
aplicarse porque **cero bytes con `asm` puesto es deuda** y rompe cuatro filas
nuevas mientras arregla siete.

---

## 4. Lo que NO he tocado, y por qué

- **`UpdateWheelYRenderOffset` (876 B, 7 filas) y `RenderFlaresOnCar` (2.908 B,
  18 filas)**: sin tocar. La hipótesis viva de la r52 sigue siendo la puerta
  «potential lossage» de `scan_loop` (mirar el volcado `.cse`, no el `.loop`, y
  contar los usos del `(high Rear)`), y no me ha dado tiempo. El frente de
  cflags está cerrado con control desde la r52; **no lo reabras**.
- **`SetMemoryPoolSize` (304 B) y `DefragmentPool` (684 B)**: sin tocar.
  Comprobado que **`DefragmentPool` no tiene ningún `asm` escrito** al que
  añadirle una entrada, así que la palanca corregida de la r52 no tiene dónde
  agarrarse; lo que hay en el fuente son catorce clobbers ya medidos y
  negativos. Para `SetMemoryPoolSize` sigue en pie el diagnóstico de la r49:
  hace falta alargar en uno la cadena `lis → … → llamada` y ahí no se puede sin
  bytes.

---

## 5. Artefactos y sondas

`scratchpad/r53_world/` (429 kB; borrados los volcados RTL de 19 MB y los json
de objdiff):

- `base_fncmp_*.txt` / `final_fncmp_*.txt` — el antes/después de las dos
  unidades (**diff vacío**).
- `keep_zWorld.txt`, `keep_zWorld2.txt` — las líneas de `keep.lst` de §1.2 tal
  como las escupe `deadlink --keep`.
- `pref_zWorld.txt`, `pref_zWorld2.txt` — los bloques `asm()` aplicados.
- `lc_sin.txt` / `lc_con.txt` — el control de §1.3 (mapa `$LC` con y sin bloque).
- `mide.py` — **candidata a `scripts/`**: mide el delta de secciones del ENLACE
  de una unidad con `keep.lst` + N líneas extra **sin escribir en `keep.lst`**.
  Es `deadlink --medir` pero aceptando cualquier lista de líneas, que es lo que
  hace falta cuando el agente no puede tocar `keep.lst`. Con eso se prueba una
  entrada suelta (la vtable de §1.5, el `lbl_80409D58` de §1.4) en 40 s.
- `probe_strip.py` — lista los símbolos de `.rodata` que el estripado se lleva y
  que `deadlink` **no** propone, con su contenido y si el objetivo los tiene.
  Es lo que destapó el `pad_05_…` rancio. **Vale la pena pasarlo por las 22.**
- `sv.py` — `-fsched-verbose-N` de una TU con los cflags de la unidad, a
  fichero. El volcado no lleva cabecera de función: se localiza el bloque por
  los UID que da el `.lreg`.
- `rosize.py` — `.rodata` del objeto nuestro contra el del objetivo, en una
  línea. Es la mitad que le falta a `linkdelta` para separar «falta contenido»
  de «se estripa de más» (§1.6).
- `p.py`, `b.py`, `h.py`, `v.py`, `ft.py` — heredadas de la r52, repuntadas a
  este directorio; `p.py` ahora usa `wrn_only.cpp` como TU.
- `dj.py` — imprime las filas que difieren del json que deja `h.rows`.
- `x1.py`, `x2.py`, `x3.py` — los tres bancos, con las variantes exactas.
- `*.cpp.HEAD` / `*.cpp.P1` — copias de partida. **Aviso vigente**: `b.py`
  restaura desde `.HEAD`; los anclas de `run()` llevan **CRLF**, con `\n` no
  casan (me costó una tanda).
