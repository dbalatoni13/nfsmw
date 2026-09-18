# r27 — zWorld + zAI + zGameplay + zPhysicsBehaviors

Encargo: 10 near-miss, 16.164 B. **Cierres: 0. Aporte neto +0 B, +0 funciones.**

Lo que traigo: **un diagnóstico de la r26 desmentido con medida** (los tres `goto`
de `AssignClosestOffsets` **SÍ los tiene el original**), **la causa exacta a nivel
RTL** de esa función, de `RenderFlaresOnCar` (el volcado `-dG` que pedía el brief)
y de `SetMemoryPoolSize` (`sched1` ya da el orden del objetivo y lo deshace
`sched2` por una WAR que nace en el asignador), **la primera mejora medida en
`RenderFlaresOnCar` desde la r20** (+0,09 pp, no cierra), y **21 ensayos
numerados** más **15 banderas**, todos con su cifra.

    b1..b5   AssignClosestOffsets      g1 g2 h2 h5   CullParts
    f1       RenderFlaresOnCar         k1 k2         UpdateLoaded
    m1..m5   SetMemoryPoolSize         n1            DefragmentPool
    o1..o3   UpdateWheelYRenderOffset

---

## 0. Verificación del encargo — reproduce EXACTO

`build_direct.py zWorld zAI zGameplay zPhysicsBehaviors` + `triage.py --muro`:
las 10 funciones del encargo con su porcentaje y su tamaño, más `SetMemoryPoolSize`
(304 B) que también sale en el muro. Al cerrar, tras `build_direct.py` de las
cuatro unidades, `triage.py` da **las once líneas idénticas al arranque**.

**Trampa de herramienta (nueva)**: la columna de multiconjunto de `triage.py`
**no es estable entre pasadas**. Con el objeto sin tocar, `UpdateLoaded` salió
`stmw+1, lwz-1, lmw+1, lis-1`, luego `lis-1, lwz-1, lmw+1, stmw+1` y luego
`stmw+1, lis-1, stw-1, lmw+1` — cambia el orden **y el conjunto** (`lwz-1` contra
`stw-1`), mientras `faltan/sobran`, el porcentaje, el tamaño y las 21 filas del
diff siguen idénticos. **Se lee como una regresión y no lo es**: confírmalo con
las filas (`fndiff` / `c26wag_rows.py`), no con esa columna.

## 1. `audit.py` — lista de FALLA: **VACÍA**

| unidad | símbolos auditados | FALLA |
|---|---|---|
| zWorld | 577 | **0** |
| zAI | 1028 | **0** |
| zGameplay | 765 | **0** |
| zPhysicsBehaviors | 1118 | **0** |

Sin ningún FALLA no hay nada que confirmar con una segunda pasada. No he congelado
nada (no he cerrado nada) ni re-congelado unidades ajenas. `git status -- src/` al
cerrar **no lista ninguno de los cinco ficheros que he tocado**
(`CarRender.cpp`, `CarLoader.cpp`, `AIPursuit.cpp`, `SuspensionTraffic.cpp`,
`SAP.h`); los 18 que salen son de otros agentes. Todos los arneses restauran en el
`finally` y **verifican por md5**.

---

## 2. `AssignClosestOffsets` (1.684 B) — la r26 se equivocaba: **los `goto` son CORRECTOS**

### 2.1 El ensayo que lo demuestra

`dwbody` decía que el original **no** tiene los tres `goto`
(`search_offsets`/`found_offset`/`searched_offsets`) y la r26 lo dejó como «lo que
yo haría primero». **Lo he hecho y es al revés.**

| # | sentencia barrida | resultado |
|---|---|---|
| b0 | control | **99,66746 %**, 1684/**1680**, 8 filas |
| **b1** | **quitar los tres `goto`** (bucle `for`+`break` natural, `if (currentCop < 0) continue;` delante) | **98,432 %**, 1676 B, **15 filas** |

Y las filas nuevas de b1 dicen **por qué**: el objetivo coloca el bloque
`found_offset` **fuera de línea, ANTES del bucle de búsqueda**:

    objetivo  fila 285  bge  0x2e744          fila 286  b 0x2e8d0
              fila 287  mr r30, r11           fila 288  b 0x2e788
    b1        fila 285  blt  <+0x60c>         filas 303/304 mr r30,r11 / b <+0x4c4>

Ese es exactamente el código que produce nuestro `goto found_offset` y que b1
destruye. **Las etiquetas `// Labels` del `dwbody` no prueban ausencia de `goto`
en el original: prueban que su DWARF no las nombra.** Contraejemplo medido del
límite 2 del brief, y **corrección del titular del encargo de esta ronda**.

**VEDA (b1)**: quitar los tres `goto`, −1,24 pp y +7 filas.

### 2.2 La causa real, al nivel del RTL (volcados por pase)

Con `c27wag_rtl.py` (variante mía de `c26wag_rtl.py` con `PRELUDE`, ver §9) se
puede compilar `AIPursuit.cpp` **suelto**: el error de la r26 (`ipv` sin declarar)
es una cascada de `VehicleClass` sin declarar en la línea 638, y basta anteponer
`#include "Speed/Indep/Src/Physics/PVehicle.h"` al stub. El `.s` reproduce el
`mr. 9,28` al dígito (421 insns con `-fno-expensive-optimizations`, 420 en el
control).

Las tres fotos del mismo par de insns:

| pase | insn 1812 | insn 1813 |
|---|---|---|
| **jump** | `(set (reg/v 418) (plus (reg/v 418) -1))` | `(set cc (compare (reg/v 418) 0))` |
| **cse** | igual | igual |
| **gcse** | `(set (reg/v 418) (reg 658))` ← **PRE lo parte** | `(set cc (compare (reg/v 418) 0))` |
| **loop** | igual (+ `(set 658 (plus 418 -1))` movido a la cima del bucle) | igual |
| **cse2** | igual | igual |
| **combine** | `(parallel [(set cc (compare (reg 658) 0)) (set (reg/v 418) (reg 658))])` = **`mr.`** | fusionada |

O sea:

1. `expand` emite **una sola** insn `n = n - 1` y compara **la variable**.
2. **`gcse`/PRE** la parte en `(set 658 (plus 418 -1))` (izada a la cima del
   bucle) más una **copia** `(set 418 658)` al fondo.
3. `combine` funde la copia con el `compare` porque el `compare` lee **el destino
   de la copia**.

El objetivo compara **el temporal** (`cmpwi r29,0` con `mr r9,r29`), y por eso
`combine` no puede fundir: el `compare` no depende del `mr`. **`regmap` confirma
que `copsToAssignOffsets` vive en r9 en los dos**, así que r29 es el temporal en
los dos lados: la única diferencia es qué pseudo lee el `compare`.

`cse2` no puede darle la vuelta: `make_regs_eqv` hace canónico a 418 porque su
última mención (el propio `compare`) es posterior a la de 658 (la copia). Es un
punto fijo: quien esté en el `compare` se queda.

### 2.3 Ensayos (arnés `c27wag_rtlpatch.py`, ciclo **10 s** — no hace falta la unidad)

| # | sentencia barrida | resultado |
|---|---|---|
| b2 | `} while ((copsToAssignOffsets = copsToAssignOffsets - 1) > 0);` | **IDÉNTICO** (420 insns, `mr. 9,28`) |
| b3 | `} while ((copsToAssignOffsets -= 1) > 0);` | **IDÉNTICO** |
| b4 | `} while (0 < (copsToAssignOffsets = copsToAssignOffsets - 1));` | **IDÉNTICO** |
| b5 | los tres `continue` → `goto next_cop;` + etiqueta + `copsToAssignOffsets--;` antes del `while` | **IDÉNTICO** |

**VEDA (b2/b3/b4)**: `store_expr` devuelve el **target**, no el temporal, cuando la
asignación se usa por valor en C++. Las tres formas dan el mismo objeto que el
control. (Era la hipótesis con más fundamento que tenía; queda cerrada con cifra.)

**VEDA (b5)**: la etiqueta al final del cuerpo con el decremento detrás **no**
reproduce el reparto de bloques del objetivo: GCC pliega el `goto` y sale el mismo
`mr.`. Esto también explica por qué a1/a3 de la r26 daban 11 filas: ahí la
etiqueta cae **entre** el `mr` y el `cmpwi` (los `continue` se saltan el
decremento), mientras que el objetivo la tiene **delante** del `mr`.

### 2.4 Barrido de banderas: **15, ninguna rompe la fusión**

    -fno-gcse                    409 insns, `addic. 29,29,-1` (sin copia)
    -fno-rerun-cse-after-loop    427 insns, mr.
    -fno-expensive-optimizations 421 insns  <-- TAMAÑO EXACTO, y sigue mr.
    -fno-move-all-movables       413, mr.      -fno-rerun-loop-opt   423, mr. 9,29
    -fno-strength-reduce 420 · -fno-thread-jumps 420 · -fno-cse-follow-jumps 421
    -fno-cse-skip-blocks 422 · -fno-force-mem 420 · -fno-regmove 420
    -fno-schedule-insns 417 · -fno-caller-saves 420 · -fno-peephole 420
    -fno-delayed-branch 420

**Décimo caso de «el tamaño exacto miente»**: `-fno-expensive-optimizations` da
**1.684 B clavados** y lo único que cambia respecto del control es un `mr 6,7`
alrededor de la llamada a `AssignCopOffset` — una instrucción que el objetivo
**no** tiene. El `mr.` sigue ahí.

---

## 3. `RenderFlaresOnCar` (2.908 B) — el volcado `-dG` que pedía el brief

### 3.1 Lo que dice PRE, literal

`cc1plus -dG` sobre `CarRender.cpp` (7 s). En la tabla de expresiones de la
función:

    Index 175   (high:SI (symbol_ref:SI ("lbl_8040AD04")))
    Index 176   (mem/u/f:SF (lo_sum:SI (reg:SI 564) (symbol_ref "lbl_8040AD04")))
    Index 221   (mem/u/f:SF (lo_sum:SI (reg:SI 649) (symbol_ref "lbl_8040AD04")))
    Index 222   (mem/u/f:SF (lo_sum:SI (reg:SI 651) (symbol_ref "lbl_8040AD04")))

y las decisiones:

    PRE: redundant insn 1958 (expression 175) in bb 192, reaching reg is 1010
    PRE: redundant insn 1965 (expression 175) in bb 193, reaching reg is 1010
    PRE/HOIST: end of bb 141, insn 2614, copying expression 175 to reg 1010

**Y el bloque 141 YA calcula esa misma expresión 25 insns antes:**

    (insn 1570  (set (reg:SI 564) (high (symbol_ref "lbl_8040AD04"))))   <- `intensity`
    (insn 1573  (set (reg/v:SF 562) (mem (lo_sum (reg 564) sym))))
    ... 1576, 1579, 1582 ...
    (insn 2614  (set (reg:SI 1010) (high (symbol_ref "lbl_8040AD04"))))  <- PRE, al FINAL del mismo bb

Ése es el `lis` que sobra, con nombre y número de insn: **PRE inserta una segunda
materialización de `high(lbl_8040AD04)` al final del bloque que ya la calcula**.
Los tres `mem` (176/221/222) hashean distinto porque llevan **registros
distintos** dentro del `lo_sum`, así que gcse no los ve como la misma expresión y
sólo unifica los `high`. El objetivo tiene **un solo pseudo** (r16, fila 123) para
los tres usos.

### 3.2 La primera mejora medida desde la r20

| # | sentencia barrida | resultado |
|---|---|---|
| — | control | **98,61761 %**, 2908/**2912**, 18 filas |
| **f1** | `float intensity = lbl_8040AD04;` movida **detrás** de los dos `if` del cuerpo del bucle (justo antes del `switch`) | **98,70702 %**, 2912 B, **17 filas** |

f1 deja los dos `lis` **adyacentes** (filas 479 y 482) en vez de separados por 9
insns. **No cierra y no la aplico**: además de no dar bytes, contradice el orden
de declaración del DWARF del original (igual que la e6 de la r26).

### 3.3 Un hallazgo de la r26 que hay que retirar: **el 0.0f duplicado NO es una anomalía**

Leídos los 4 B del ELF original en cada dirección:

    lbl_8040AA84 = 00000000 = 0.0f      lbl_8040AD04 = 00000000 = 0.0f
    lbl_8040ACF0 = 3f800000 = 1.0f      lbl_803CA86C = bf800000 = -1.0f
    lbl_803CA870 = 3e800000 = 0.25f     lbl_803CA874 = 00000000 = 0.0f
    lbl_803FB6B8 = 3f800000 = 1.0f

**El original tiene TRES `0.0f` distintos en la misma `.rodata`.** No es un
descuido nuestro: el `const_rtx_hash_table` de GCC es **por función**
(`varasm_status` vive en `struct function`), así que cada función que use `0.0f`
se lleva su propio `$LC`. Que nuestro objeto tenga `$LC606` **y** `lbl_8040AD04`
es lo mismo que hace el original con `lbl_8040AA84` y `lbl_8040AD04`. **Lo que sí
es real** es que dentro de `RenderFlaresOnCar` el objetivo usa **un solo símbolo**
(36 apariciones de `lbl_8040AD04` en su `lmap`, 17 `lis` rematerializados + el
izado a r16) y nosotros dos. Las dos direcciones de unificarlo están vedadas con
cifra por la r26 (e1/e9/e10 = 98,054 %; e2/e3/e4 = 97,552 %).

---

## 4. `CullParts` (836 B) — el `asm("" : : : "r11")` **hace falta** (con cifra)

El diff completo son 5 filas y **una** causa: el objetivo compara la guarda del
bucle contra **r9** (el 0 de `debug_print`, `li r9,0` en la fila 96 — que
nosotros **también** emitimos) y luego hace `mr r9, r0` (r9 = `NumPlanes`);
nosotros comparamos contra **r11** (el 0 que ya trae `Polarity` tras el `beq`) y
no hacemos la copia.

| # | sentencia barrida | resultado |
|---|---|---|
| — | control | **99,42584 %**, 836/832, 5 filas |
| g1 | quitar `asm("" : : : "r11")` (línea 307) | **99,330 %**, 832 B, **7 filas** |
| g2 | `int debug_print = 0;` declarada antes de `bVector3 v = *camera_eye - ...` | **97,057 %** |
| h2 | `if (debug_print == n)` en vez de `if (n == debug_print)` | **99,330 %**, 6 filas |
| h5 | `int n = 0;` declarada antes de `debug_print` (fuera del `if` de `Polarity`) | **98,445 %** |

**VEDA (g1)**: la barrera de clobber sobre r11 **no es un pin de más**: quitarla
cuesta 0,10 pp y mete dos filas nuevas (`culldiv@ha` cambia de r18 a r17).

**Y un frente pequeño, contado**: `pines.py` **no ve las barreras de clobber**. Su
regex es `asm[ \t]*[(][ \t]*"(r[0-9]+|f[0-9]+)"[ \t]*[)]`, que sólo casa
`register X asm("rN")`. En todo el árbol hay **seis** `asm("" : : : "...")`:

    gc_driver.cpp:360  "r25"     gc_driver.cpp:613  "cr4"
    pathsnd.cpp:795    "memory"  SFXCTL_3DObjPos.cpp:134 y :155  "r9"
    CarRender.cpp:307  "r11"     <- medida: HACE FALTA (g1)

Son pocas y la única de mi territorio está justificada con cifra, así que **no es
un frente**: es una línea que añadir a `pines.py` para que el censo no mienta.

**VEDA (h2)**: invertir el `==` mueve `Polarity` a r9 y **arregla la fila 101**,
pero rompe cuatro filas más abajo (r9/r11 en el bucle). Cinco filas → seis.

**VEDA (g2/h5)**: mover cualquiera de las dos declaraciones, −2,4 y −1,0 pp.

---

## 5. `UpdateLoaded__SuspensionTraffic::Tire` (856 B) — `dwbody`: **CERO diferencias**

`dwbody.py zPhysicsBehaviors "Tire::UpdateLoaded" diff "lat_vel /* f30 */"` (hay
**cinco** `Tire::UpdateLoaded` y `regmap`/`dwbody` cogen la primera: **sin el
discriminante se lee la de `SuspensionRacer`, que está al 100 %, y sale un
`slip_ang` «SOLO NUESTRA» que no existe**). Con el discriminante correcto el
cuerpo casa entero: sólo cambian las anotaciones `// f13` de dos temporales.

La única diferencia del objeto: el objetivo iza **un** `lbl_803FB6B8@ha` (1.0f) a
**r30** justo antes de `bl VU0_Atan2` y lo usa en dos sitios (nuestras líneas 305
y 312, a los dos lados de `VU0_sqrt`), pagando `stmw`/`lmw` y 8 B de marco;
rematerializa los otros dos usos. Nosotros rematerializamos **los cuatro**.

El `-dG` dice que **PRE sí unifica**:

    Index 40   (high:SI (symbol_ref/u:SI ("*$LC251")))     <- el 1.0f
    PRE: redundant insn 454/596/643/733/784 (expression 40), reaching reg is 362
    PRE/HOIST: end of bb 15, insn 848, copying expression 40 to reg 362
    PRE/HOIST: end of bb 19, insn 851, copying expression 40 to reg 362

o sea un solo pseudo (362) con **cinco** usos y **dos** puntos de inserción. Lo
que lo deshace es posterior a gcse (`local-alloc`/`update_equiv_regs`
rematerializa): el `.s` final tiene **cuatro** `lis $LC251@ha`; el objetivo tiene
**tres**.

| # | sentencia barrida | resultado |
|---|---|---|
| — | control | **97,21028 %**, 856/**860**, 21 filas |
| k1 | `if (0.0f < mEBrake && 1.0f < Abs(fwd_vel))` partido en dos `if` con `mSlip = 0.0f` delante | **94,220 %**, **848 B** |
| k2 | `if (skid_speed > 1.0f && mEBrake > 0.5f)` (operandos del `&&` intercambiados) | **96,182 %**, 860 B |

**VEDA (k1/k2)**: cambiar el CFG de los dos `if` que usan el 1.0f no reduce los
puntos de inserción de PRE; las dos empeoran.

---

## 6. `SetMemoryPoolSize` (304 B) — sin mirar por nadie hasta ahora: **es UNA ranura de `sched2`**

Todo el diff son dos filas, y es un intercambio puro:

    objetivo  lis r6,LC@ha · addi r6,r6,LC@l · stw r0,CarLoaderMemoryPoolNumber@l(r29) · bl
    nuestro   lis r6,LC@ha · stw r0,...@l(r29) · addi r6,r6,LC@l                       · bl

El verbose de `sched2` (`-dR -fsched-verbose-5`, **con guion**) lo da con nombre:

    ;;  Ready list (t = 15):    256  232  250
    ;;      --> scheduling insn <<<250>>> on unit iu2      (250 = %3=%0)
    ;;      --> scheduling insn <<<232>>> on unit lsu      (232 = el stw)
    ;;  Ready list (t = 16):    256                        (256 = el addi del literal)

En el ciclo 15 la **iu2** elige la insn **250** (`mr r3,r0`, que además desaparece
del `.s` final) por delante de la **256**, y el `stw` se cuela en la lsu del mismo
ciclo. El objetivo mete el `addi` en el 15 y el `stw` en el 16.

**Y el desempate está identificado al dígito.** La tabla de dependencias del mismo
volcado (columnas: UID, INSN_CODE, bb, dep_count, **PRIORITY**, cost) dice:

    ;;      247   506     0     4    12     1  iu2 : 256
    ;;      256   507     0     5    11     1  iu2 : 282 270 258            <- 3 dependientes
    ;;      250   512     0     2    11     1  iu2 : 288 282 270 266 258    <- 5
    ;;      232   512     0     5    11     2  lsu : 288 282 270 258        <- 4

Las tres candidatas tienen **la misma prioridad (11)**. `rank_for_schedule`
desempata **antes de llegar al LUID** por «cuántas insns dependen de ella»
(`depend_count`), y la lista `256 232 250` que imprime el verbose es exactamente
el orden ascendente 3 < 4 < 5. Se coge por el final: 250, luego 232, y el `addi`
(256) se queda para el ciclo 16.

**La insn que le sobra a 232 es la 288** (`li r0,0` de
`this->NumSpongeAllocations = 0;`): es una **antidependencia WAR sobre r0**,
porque el `stw` LEE r0 y la 288 lo ESCRIBE. Sin ella, `depend_count(232) = 3` y
empataría con 256, y el desempate pasaría al LUID. **Ésa es la palanca**: quitarle
al `stw` un dependiente, o darle uno más al `addi`.

| # | sentencia barrida | resultado |
|---|---|---|
| m1 | `bInitMemoryPool(CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum(), ...)` (asignación dentro del argumento, para que `precompute_arguments` cambie el orden de LUID) | **IDÉNTICO** (97,368 %, mismas 2 filas) |
| m2 | `int n;` (muerta) declarada **detrás** del `bMalloc` | **IDÉNTICO** |
| m3 | `int n;` (muerta) **borrada** | **IDÉNTICO** |
| m4 | `this->NumSpongeAllocations = 0;` movida **delante** de `bInitMemoryPool` | 90,579 %, **300 B** |
| **m5** | `this->NumSpongeAllocations = 0;` movida **entre** `bInitMemoryPool` y `bSetMemoryPoolDebugFill` | 94,579 %, 304 B — **pero la fila del `stw` CASA** |

**m5 valida el mecanismo**: al sacar la insn 288 de detrás del `stw`, la
antidependencia desaparece, `depend_count(232)` baja y **el `stw` de
`CarLoaderMemoryPoolNumber` cae en la ranura del objetivo** — las dos filas del
diff original se van. Lo que rompe es otra cosa (el `stw` de
`NumSpongeAllocations` se adelanta y `lwz r4`/`lwz r5` se cruzan): 6 filas nuevas.
**El mecanismo está probado; falta una forma que quite la WAR sin mover la
sentencia.**

### 6.1 Y el remate: **`sched1` YA da el orden del objetivo; lo deshace `sched2`**

Volcado de `sched1` (`-dS -fsched-verbose-5`) del mismo bloque:

    ;;   13   254 %5=[r82+0x64]      | 228 r132=%3
    ;;   14   252 %4=[r82+0x60]      | 247 r143=high(`*$LC260')
    ;;   15                          | 256 %6=r143+low(`*$LC260')   250 %3=r132
    ;;   16   232 [r134+low(`CarLoaderMemor...
    ;;   17   258 call bInitMemoryPool

y el RTL que sale de `sched1` va en el orden **`254, 252, 247, 256, 250, 232,
258`** — o sea `lis`, **`addi`**, `mr`, **`stw`**, `call`: **exactamente el del
objetivo**. Es `sched2` (post-reload) el que lo vuelve a cruzar.

**Y se ve por qué**: antes de reload el `stw` es
`(set (mem (lo_sum (reg 134) sym)) (reg 132))` —lee **pseudos**— y **no hay
antidependencia** con el `li` de `NumSpongeAllocations`. Después de reload lee
**r0**, y la 288 (`li r0,0`) escribe r0: **la WAR nace en el asignador**, sube
`depend_count(232)` de 3 a 4 y le da la ranura del ciclo 15.

O sea que el frente correcto no es la fuente del bloque de `bInitMemoryPool`
—`sched1` ya lo coloca bien— sino **impedir que el `li` de
`this->NumSpongeAllocations = 0;` caiga en r0** (o que el `stw` lea r0). m5 lo
confirma por el otro lado: quitando la 288 de detrás, la fila casa.

**`dwbody` destapa lo único estructural que le queda**: el original tiene **DOS**
expansiones inline de
`bMalloc(int size, const char *debug_text, int debug_line, int allocation_params)`
—una a **nivel de función** y otra dentro del bloque anónimo— y nosotros sólo la
del bloque. Las dos con rango **cero** (el wrapper de `bWare.hpp:97` no emite
código: reenvía a `bMalloc__Fii`).

**VEDA (m2/m3)**: mover o borrar la `int n;` muerta que abre ese bloque **no
cambia un byte** del objeto. O sea que el segundo registro inline **no depende del
anidamiento de bloques** y la palanca, si existe, es una segunda llamada a
`bMalloc` en el original — que no puede ser (el `.text` cuadra al byte y sólo hay
un `bl bMalloc__Fii`). **Lectura: es ruido de DWARF, no una sentencia que falte.**

---

## 7. El `blt`→`b` (4.396 B) — la estructura exacta, y la pista de SAP.h **corregida**

No he hecho ensayos (cerrado por la r25 con 27 formas y 30 banderas), pero sí he
leído el objetivo, y **la lectura de la r25 sobre dónde va la sentencia que falta
es errónea**.

La forma exacta de los dos bucles (`Add__6RBGrid`, filas 39-50):

    objetivo   fcmpu (preencabezado)
               .top: cror un,eq,gt · bso salida        <- el test VIVE en el bucle
                     mr r6,r9 · lwz r9,0x4(r9) · cmpwi · beq salida
                     lfs · lfs · fcmpu · b .top        <- salto INCONDICIONAL
    nuestro    fcmpu · cror · bso salida (preencabezado, ejecutado una vez)
               .top: mr r6,r9 · lwz · cmpwi · beq salida
                     lfs · lfs · fcmpu · blt .top      <- test rotado al fondo

Mismo número de instrucciones (`cror`+`bso`+`b` contra `cror`+`bso`+`blt`): es una
**rotación de `expand_end_loop` que el objetivo no hace**, no un `blt` suelto.

Y el mapa de líneas del constructor de `SAP.h` del original:

    SAP.h:87   this->mRoot = this        (nuestra 21)
    SAP.h:90   node = nullptr            (nuestra 23)
    SAP.h:94   el fcmpu del `while`      (nuestra 26)
    SAP.h:95   mr r6,r9  = node = head   (nuestra 27)  ... y el `b .top`
    SAP.h:99   lwz r9,0x4(r9) = head = head->mTail   (nuestra 28)
    SAP.h:97 + SAP.h:101  cmpwi r6,0 = if (node != nullptr)   (nuestra 31)

**La corrección**: la nota de la línea 97 **no cae entre la 95 y la 99** (es decir,
no está en medio del cuerpo del bucle, como suponía la r25) sino **pegada a la de
la 101, en el bloque de salida**. Una sentencia dentro del cuerpo dejaría su nota
entre el `mr` y el `lwz`, y no está ahí. La lectura que encaja es un `break`/salto
en la 97 cuyo destino se fundió con la salida por cross-jump — o sea que la
sentencia que falta es **una salida del bucle**, no una sentencia intermedia.
No la he reconstruido.

---

## 8. Lo que NO he probado

- **`AssignClosestOffsets`**: no he encontrado ninguna forma de fuente que haga
  que el `compare` lea el temporal en vez de la variable, y por el §2.2 creo que
  **no la hay dentro de C++** (`store_expr` devuelve el target y `expand_increment`
  también). Lo que **no** he probado es tocar el bucle `for (j...)` interior o el
  cuerpo del `do` para cambiar los puntos de inserción de PRE.
- **`RenderFlaresOnCar`**: no he probado combinaciones de f1 con las e* de la r26,
  ni mover el `float sizescale = lbl_8040ACF0;` (está atado por `sizescale = 10.0f`
  de la línea 2744).
- **`UpdateLoaded`**: no he tocado los cuatro `1.0f` restantes (320, 324, 327,
  331) ni he mirado por qué `update_equiv_regs` deshace el pseudo 362 de PRE
  (haría falta el volcado `-dl` del `.lreg` y contar `REG_N_REFS`).
- **`SetMemoryPoolSize`**: sé que el desempate es `depend_count` y que el
  dependiente de más del `stw` es la WAR sobre r0 de
  `this->NumSpongeAllocations = 0;` (§6), pero **no he barrido ninguna forma que
  la quite**: mover esa asignación delante de las tres llamadas emite su `stw`
  antes (rompería las filas 70-71), así que hace falta algo más fino —
  probablemente que el 0 de esa asignación no caiga en r0. **Es lo que yo haría
  primero**: 304 B a una ranura, y el mecanismo ya está identificado.
- **`CullParts`**: el bloque de depuración de 22 líneas (líneas 635-656 del
  original) sigue sin reconstruirse.
- **El `blt`→`b`**: cero ensayos. La pista viva ahora es «un `break` en la línea 97
  del ctor de `SAP.h`», no «tres sentencias en medio del bucle» (§7).
- **zGameplay** (`GTrigger`, `GenerateIndex`, 4.224 B): cero ensayos, negativo
  firme de la r25 confirmado por el brief.
- **`permuter.py`**: sobre ninguna. **Ninguna variante con restricciones de
  registro** en ninguna.

## 9. Herramientas dejadas en el scratchpad (prefijo `c27wag_`)

- **`c27wag_rtl.py`** — como `c26wag_rtl.py` pero acepta `PRELUDE` en el entorno
  (líneas separadas por `|`) que se antepone al stub. **Es lo que hace compilable
  suelto `AIPursuit.cpp`** (`PRELUDE='#include "Speed/Indep/Src/Physics/PVehicle.h"|'`):
  los errores posteriores (líneas 1531/1684/1701) caen **detrás** de la función y
  cc1plus ya ha emitido sus volcados y el `.s`.
- **`c27wag_rtlpatch.py`** — parche + volcado RTL + restauración por md5. Ciclo
  **10 s** contra los 30 s de `c26wag_unit.py`.
- **`c27wag_tail.py`** — cuenta insns y enseña la cola del bucle de un símbolo en
  un `.s`. Con los dos anteriores, un ensayo de forma cuesta 10 s sin tocar
  `build/`.
- **`c27wag_patchrows.py`** — parche + `mn_repro` + **filas del diff** (lo que
  `c26wag_patch.py` no daba). Ciclo 12 s.

Borrados al cerrar los cuatro directorios de volcados RTL (43 MB).
