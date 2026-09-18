# r49 — zCamera: los 16 B del `__static_init` son CUATRO `addi`, y estan a 22 rtx de cerrarse

**Nada cerrado en el arbol. Delta retenido: 0 B / 0 funciones.** El arbol queda
exactamente como estaba:

```
$ git status --porcelain src/Speed/Indep/Src/Camera src/Speed/Indep/bWare \
                        src/Speed/Indep/Libs/Support src/Speed/Indep/SourceLists
(vacio)
$ python scripts/lcfix.py --check
todas las entradas @lc estan al dia
```

`fncmp` final identico al inicial (ninguna funcion empeora):

```
       B  funcion                                        diferencia
    3868  Update__8ICEMoverf                             15 insn, 94 reub
    3604  __static_initialization_and_destruction_0      tamano (3620/3604)
    1192  TerrainVelocityNoise__11CameraMoverP8bMatrix4P 13 insn, 41 reub
    1156  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8 80 insn
     992  Update__19TrackCarCameraMoverf                 15 insn, 22 reub
5 de 453 funciones con el CODIGO distinto -- 10812 B que no cuentan
```

Todos los ensayos se hicieron sobre una **sombra completa** de
`src/Speed/Indep/Src/Camera/` en `scratchpad/r49_cam/inc2/` con `-I` **delante**;
`src/` no se toco en ningun momento.

---

## 0. Lo que cambia respecto a la r48

| # | la r46/r47/r48 decian | lo medido en la r49 |
|---|---|---|
| 1 | `__static_init`: «faltan 107 rtx para el presupuesto de `cse`, y no hay 107 rtx disponibles en `Camera/**`; sin vetas pendientes» | **Los 16 B son CUATRO `addi` concretos**, de la init de `ReplayCategoryTable`. Con **22 cargas muertas** en el sitio adecuado la funcion pasa a **3.604 B exactos / 99,8391 % / 6 filas**, y **el resto de la unidad no cambia ni un byte** (§1) |
| 2 | la veda escrita en `ICEReplay.cpp`: «es reparto del planificador, sin palanca de fuente a la vista» | **La fuente de `ICEReplay.cpp` es CORRECTA**: en una TU que solo la contiene, los seis elementos se guardan con desplazamiento completo sobre la base, igual que el objetivo. **Es un efecto de TAMANO DE UNIDAD**, no de forma (§1.2) |
| 3 | `regmap` sobre `ICEMover::Update`: «PERMUTACION r26 <-> r27» | **Falso positivo.** `regmap` empareja las locales por posicion-y-tipo y nuestro nombre/tipo difieren (`bool bLerpLag` contra `int bGeneric`). Nuestro DWARF da **exactamente** r27 y r26 igual que el original (§2.1) |
| 4 | `ICEMover`: «falta fundir el pseudo del `fctiwz` con el `a` de `SignedMod`» | Sigue en pie, pero **ni la forma de `SignedMod` (10 formas) ni la de `GetKey` (8) ni la del sitio de llamada (8) ni cuatro barreras sobre `frame` mueven una sola fila** (§2) |
| 5 | `TerrainVelocityNoise`: hay que mover el LUID del `elf_high` del 0.0f | **No es el LUID, es la PRIORIDAD**: el volcado de `sched1` da 7 para el `high` del 0.0f contra 5, 5 y 4 de los otros tres. Y el unico mecanismo de GCC que puede bajar un `lis` hasta su uso esta identificado con nombre y condicion: `update_equiv_regs` (§3) |

Y **dos metricas mias salieron mentirosas y las cace con un control**; estan en
§6 porque el modo de fallo es reutilizable.

---

## 1. `__static_initialization_and_destruction_0` (3.604 B): los 16 B tienen nombre

### 1.1 Que son los 16 bytes

3.620 - 3.604 = 16 B = **cuatro instrucciones**. Son estas, y solo estas:

```
>>>  796                                       | addi r27, r30, 0x30     INSERT
>>>  842 addi r10, r10, lbl_803D3960@l          | addi r27, r30, 0x48     ARG_MISMATCH
>>>  863 addi r10, r10, lbl_803D3978@l          | addi r27, r30, 0x60     ARG_MISMATCH
>>>  884 addi r10, r10, lbl_803D3994@l          | addi r27, r30, 0x78     ARG_MISMATCH
```

`r30` es `&ICE::ReplayCategoryTable` (fila 787, `addi r30, r10,
_3ICE.ReplayCategoryTable@l`), y cada elemento mide 0x18. El objetivo guarda los
seis campos de **todos** los elementos con el desplazamiento completo
(`stw r9, 0x40(r30)`, `stw r29, 0x30(r30)`, `stw r3, 0x34(r30)` ...). Nosotros lo
hacemos asi **solo para los elementos 0 y 1**; a partir del 2 materializamos un
**puntero de fila** (`addi r27, r30, 0x30`) y guardamos con `0x8(r27)`,
`0x10(r27)`, `0x14(r27)`, `0xc(r27)`, `0x4(r27)`.

Las 54 filas de diff de esta funcion son: **7 de la permutacion de
`HydraulicsLookAngle`** (r47, cero bytes) y **47 de este puntero de fila**.

De donde sale el puntero: `cp/init.c::expand_vec_init` inicializa un array con
constructor con un **puntero corriente** (`base = get_temp_regvar(...)`, y tras
cada elemento `base = base + size`). Cuando `cse` conserva la equivalencia
`base ≡ r30 + K` el puntero desaparece y quedan desplazamientos completos; cuando
la pierde, sobrevive el `addi`.

### 1.2 La fuente de `ICEReplay.cpp` es correcta — probado con la TU minima

Compilando **una TU que solo contiene `ICEReplay.cpp`** (preambulo de la
SourceList + un `#include`), la init de la tabla sale asi:

```
stw 3,28(30)    stw 9,64(30)    stw 11,68(30)   stw 29,48(30)   stw 28,56(30)
stw 3,60(30)    stw 3,52(30)    stw 9,88(30)    stw 11,92(30)   stw 10,80(30)
stw 29,72(30)   stw 3,84(30)    stw 3,76(30)    stw 9,112(30)   ...
```

**Cero punteros de fila, los seis elementos con desplazamiento completo: es
exactamente la forma del objetivo.** La veda escrita en `ICEReplay.cpp:171-174`
(«es reparto del planificador, sin palanca de fuente») **hay que reescribirla**:
no es el planificador y no es la forma de la fuente. Es el **tamano de la unidad**.

### 1.3 La medida: 22 rtx y la funcion cae

Instrumento: un relleno de codigo MUERTO (cargas de un global que `flow` borra
despues, **cero bytes emitidos**) colocado inmediatamente antes de la tabla, y un
detector que busca `addi/la R,B,K` con `K` multiplo de 24 donde `R` se usa luego
como base de un `stw` en la region de la tabla.

| relleno (cargas muertas) | punteros de fila | `__static_init` |
|---|---:|---|
| 0 (base) | **4** (elementos 2..5) | 3620 B, 97,1121 %, **54 filas** |
| 9 | 5 (elementos 1..5) | 3624 B, 96,1587 %, 87 filas |
| 12 / 16 / 20 / 21 | 5 | 3624 B, 96,1587 %, 87 filas |
| **22** | **0** | **3604 B (EXACTO), 99,8391 %, 6 filas** |
| 23 / 24 / 25 / 50 / 100 / 200 / 400 | 0 | 3604 B, 99,8391 %, 6 filas |

Y **el resto de la unidad no se mueve**: con el relleno de 22, `full.py` sigue
diciendo `139 funciones != 100%, 21816 B`, la misma cifra exacta que la base. El
relleno no emite un solo byte.

Las **6 filas que quedan** son la permutacion de la r47, ya vetada y de cero
bytes:

```
 549 li r9, 0x38e                     | lis r9, $LC1125@ha            REPLACE
 557 lis r10, HydraulicsLookAngle@ha  | li r10, 0x38e                 REPLACE
 558 lfs f24, lbl_803D3A80@l(r11)     | lfs f24, $LC1125@l(r9)        ARG
 561 lis r11, NOSFovWidening@ha       | lis r9, NOSFovWidening@ha     ARG
 562 sth r0, NOSFovWidening@l(r11)    | sth r0, NOSFovWidening@l(r9)  ARG
 564 sth r9, HydraulicsLookAngle@l(r10)| sth r10, HydraulicsLookAngle@l(r11) ARG
```

**Traduccion**: el original emite ~22 cargas (~66 rtx con `-fforce-addr` /
`-fforce-mem`) MAS que nosotros en el `__static_init`, en codigo que **no llega al
objeto**. Con ellas, esta funcion se cierra al tamano exacto y a 6 filas de
permutacion pura.

### 1.4 Y la ventana donde tienen que estar esta acotada a 12 ficheros

El relleno **no vale en cualquier sitio**. Colocandolo al principio de cada
fichero de la SourceList (una compilacion por sitio, 20 s):

| fichero donde va el relleno de 22 | punteros de fila |
|---|---:|
| `Camera.cpp` (el 1.º) | **4** (no hace nada) |
| `CameraAI.cpp` | **4** |
| `Actions/CDActionDrive.cpp` | **4** |
| `Actions/CDActionDebugWatchCar.cpp` | **4** |
| `Movers/Cubic.cpp` | **0** |
| `Movers/DebugWorld.cpp` | 0 |
| `Movers/Still.cpp` | 0 |
| `ICE/ICEMover.cpp` | 0 |
| `ICE/ICEManager.cpp` | 0 |
| `ICE/ICEOverlays.cpp` | 0 |
| `ICE/ICEReplay.cpp` (delante de la tabla) | 0 |

La frontera cae **exactamente entre `Actions/CDActionDebugWatchCar.cpp` y
`Movers/Cubic.cpp`**, que es donde el diff tiene su ultimo racimo de saltos
(filas 366-391: `beq`/`bge`/`blt`/`beq` + el `$LC695` del `Attrib` de
`CDActionDebugWatchCar`). Eso encaja con `cse_main`: `cse_end_of_basic_block`
corta el bloque extendido ahi y `cse_basic_block` **reinicia su contador**
(`if (code != NOTE && num_insns++ > 1000) flush_hash_table();`,
`orig/prodg/NGC_GNU_SRC/NGC/gcc/cse.c:8942`).

**Los ~66 rtx que faltan estan en los inicializadores estaticos de los ficheros
que van de `Movers/Cubic.cpp` a `ICE/ICEReplay.cpp`** — filas 391..787 del
`__static_init`, ~400 instrucciones, 12 ficheros. Como el codigo final de esa
ventana casa 1:1 con el objetivo, lo que falta es **andamiaje que se borra**:
casi con seguridad el de `expand_vec_init` (3 temporales + `base += size` y
`iterator -= 1` por elemento) de **un array con constructor que el original
inicializa y nosotros no**, o de uno con mas elementos.

Los candidatos de esa ventana, con su andamiaje (3 + 2N rtx):

| array | N | andamiaje |
|---|---:|---:|
| `aCubicPovTables[7]` (`Cubic.cpp:334`) | 7 | 17 |
| `CameraSpeedHugData[5]` (`Cubic.cpp:344`) | 5 | 13 |
| `vCopViewPoints[5]` (`CopView.cpp:17`) | 5 | 13 |
| `fDebugCameraInputData[5]` (`DebugWorld.cpp:41`) | 5 | 13 |
| `aDriftData[2]`, `vCubicBirdsEyeOffset[2]`, `vCopViewDistanceFovBand[2]`, `vCopViewDistanceFov[2]`, `spline_points[2]` | 2 | 7 cada uno |

**La otra direccion tambien vale y esta medida por la r47**: con
`-fno-force-addr` el tamano se vuelve **exacto (3.604)** —porque quita tanto rtx
que la purga de `cse` se va por detras del final de la tabla— aunque destroce
otras 133 funciones. Es decir, la ventana buena es ancha (~186 rtx) y estamos
dentro de la mala por poco.

### 1.5 Como reproducirlo en 20 s

```
python scratchpad/r49_cam/mkinc2.py                 # sombra de Camera/
# poner el relleno delante de `namespace ICE {` en inc2/.../ICEReplay.cpp
python scratchpad/r49_cam/asm.py tu_full49.cpp      # unidad entera a .s
python scratchpad/r49_cam/det.py out/tu_full49.s    # punteros de fila
python scratchpad/r49_cam/full.py                   # unidad entera, % y tamanos
```

`s6.py` / `s7.py` / `s9.py` hacen los tres barridos de arriba tal cual.

---

## 2. `Update__8ICEMoverf` (3.868 B, 15 filas): 30 formas mas, todas negativas

### 2.1 El DWARF: `regmap` da un FALSO POSITIVO y hay que dejarlo escrito

`regmap.py zCamera "ICEMover::Update"` sigue diciendo
«PERMUTACION: r27 y r26 estan INTERCAMBIADOS». **No lo estan.** Volcando nuestro
propio DWARF (`dtk dwarf dump` sobre el `.o` del micro) y poniendolo al lado del
original en orden de declaracion:

| # | original | reg | nuestro | reg |
|---|---|---|---|---|
| 1 | `struct ICETrack * p_track` | r28 | `pTrack` | r28 |
| 2 | **`bool bLerpLag`** | **r27** | **`int bGeneric`** | **r27** |
| 3 | `mSceneToWorld` | r1+0x8 | `mAnimToWorld` | r1+0x8 |
| … | … | | … | |
| 12 | **`int n_state`** | **r26** | **`int nState`** | **r26** |

Los registros son **identicos**. `regmap` empareja `bLerpLag` (bool) con `nState`
(int) porque no le cuadra el tipo en la posicion 2, y arrastra el desfase. La
unica diferencia real de tipo es que el original declara `bLerpLag` como **`bool`**
y nosotros `bGeneric` como **`int`** — y eso, medido, **no cambia un byte**
(§2.3, ensayo 6).

Queda pues **una sola** diferencia de DWARF viva: `int frame`, **r8 en el
original y sin registro en el nuestro**. (`current_sec` f0/f13 es el error de
volcado que ya diagnostico la r48.)

### 2.2 Lo que dice el diff, leido entero

Las 15 filas son las mismas de la r48 y son puras de registro. Traducido:

* **objetivo**: un unico pseudo `r8` = `frame` recibe el `lwz` del `fctiwz` (fila
  593), es lo que compara el `while (a < 0)` (596), recibe el `subf` (602) y el
  `li 0` (604), y es lo que multiplica el `mulli r9, r8, 0x18` de `&Keys[n]`
  (617). El bucle usa un pseudo aparte (r11).
* **nuestro**: el `lwz` cae en un temporal (r9) que **muere en la fila 594**, y el
  pseudo del bucle (r11) es el que hace todo lo demas; ademas el `mulli` de
  `&Keys[n]` usa **el resultado del clamp** (r9), no `n`.

O sea: al objetivo le sobra una cantidad viva (usa r0,r7,r8,r9,r10,r11 en esa
ventana; nosotros r0,r8,r9,r10,r11).

### 2.3 Todos los ensayos, con la cifra

Base: **15 filas / 99,9069 % / 3.868 B**. Sombra completa, 8 s por medida.

**`ICEMath.hpp::SignedMod` (10 formas)** — `SignedMod` tiene **un solo uso en
todo el arbol** (`ICEMover.cpp:675`), asi que es territorio puro:

| forma | filas |
|---|---:|
| base | **15** |
| `for (; a < 0; )` en vez de `while` | 15 |
| `if (a<0) { do { a += b; } while (a<0); }` | 15 |
| `a = a + b` en vez de `a += b` | 15 |
| `a = a - c` en vez de `a -= c` | 15 |
| `int c;` declarada antes del `while` | 15 |
| `c *= b` en vez de `c = c * b` | 15 |
| `a -= (a / b) * b` sin la variable `c` | 15 |
| **ramas invertidas** (`if (b <= 0) a = 0; else …`) | **19** |
| **`return a;` dentro y `return 0;` al final** | **19** |

**`ICEManager.hpp::GetKey` (8 formas)** — tambien de uso unico:

| forma | filas |
|---|---:|
| base `(n == ICE::Clamp(n,0,NumKeys-1)) ? &Keys[n] : 0` | **15** |
| `(ICE::Clamp(...) == n) ? …` (comparacion conmutada) | 15 |
| `if (n == Clamp(...)) return &Keys[n]; return 0;` | 15 |
| `Keys + n` en vez de `&Keys[n]` | 15 |
| `bClamp` directo en vez de `ICE::Clamp` | 15 |
| local `int m = ICE::Clamp(...)` | 15 |
| `GetNumKeys() - 1` en vez de `NumKeys - 1` | 15 |
| **`if (n != Clamp(...)) return 0; return &Keys[n];`** | **20** |

**El sitio de llamada (`ICEMover.cpp:675`), 8 formas**:

| forma | filas |
|---|---:|
| base | **15** |
| `int frame = FloatToInt(...); frame = SignedMod(frame, GetNumKeys());` | 15 |
| lo anterior + `asm("" : "+r"(frame))` entre las dos sentencias | 15 |
| `int num_keys = GetNumKeys();` antes | 15 (99,8087: **codigo distinto, mismas filas**) |
| las dos anteriores juntas | 15 (99,8087) |
| `float fpos = fParam*length*current_sec;` aparte | 15 |
| `static_cast<int>(...)` en vez de `FloatToInt` | 15 |
| **`fParam * (length * current_sec)`** | **26** |

**Barreras y pines sobre `frame`** (la r48 solo habia probado `register int frame
asm("rN")`; estas son nuevas):

| ensayo | filas | fuzzy |
|---|---:|---:|
| base | **15** | 99,9069 |
| `asm("")` suelto tras la llamada | 15 | 99,9069 (objeto identico) |
| `bool bGeneric` en vez de `int bGeneric` | 15 | 99,9069 (objeto identico) |
| `asm("" : : "r"(frame))` (solo entrada) | 17 | 99,7983 |
| `asm("" : "+r"(frame))` tras `SignedMod` | 22 | 99,3020 |
| `asm("" : "+r"(frame))` antes de `GetKey` | 22 | 99,3020 |
| las dos anteriores juntas | 22 | 99,3020 |
| `asm("" : "+m"(frame))` | 54 | 98,8780 |

**El tamano nunca se mueve de 3.868 B en ninguna de las 30 variantes.** Es
permutacion pura y ninguna palanca de fuente la toca. Con `regmap` dando
IDENTICO de verdad (§2.1), lo que toca aqui es el **permutador CIEGO** con el
catalogo entero (`scripts/permuter.py`), no mas formas a mano: el guiado apuntara
a `bLerpLag`/`n_state`, que ya estan bien.

---

## 3. `TerrainVelocityNoise` (1.192 B, 14 filas): no es el LUID, es la PRIORIDAD

La r48 dejo dicho que hay que «retrasar el LUID del `elf_high` del 0.0f por
debajo del de 0.15f». **Con la traza de `sched1` delante, eso no puede
funcionar**: el nivel 1 de `rank_for_schedule` (la prioridad) decide antes que el
5 (el LUID), y ahi el 0.0f gana.

Tabla de dependencias del bloque (`-dS -fsched-verbose-5`, columna 5 =
`INSN_PRIORITY`):

```
;;      481   506  0  0   7   1  1 - 13   iu2 : 484     high($LC336) = 0.0f
;;      533   506  0  0   5   1  1 - 13   iu2 : 536     high($LC344) = 0.05f
;;      548   506  0  0   5   1  1 - 13   iu2 : 551     high($LC346) = 0.5f
;;      540   506  0  0   4   1  1 - 13   iu2 : 543     high($LC345) = 0.15f
```

Y la eleccion, ciclo a ciclo: `t=1 -> 415` (20.0f), `t=2 -> 481` (**0.0f**),
`t=3 -> 533`, `t=5 -> 548`, `t=7 -> 540`. El objetivo saca
20.0f, 0.05f, 0.5f, 0.15f y **el 0.0f el ultimo**, pegado a su `lfs` (fila 129),
lo que le deja reutilizar el r9 que acababa de morir del 20.0f: **cuatro
registros de direccion contra nuestros cinco**.

La prioridad es el camino mas largo hasta el final del bloque, y el del 0.0f es
`481 -> 484(lfs) -> 498(fsel de bMax) -> 513(fsubs 20-x) -> 515(fsel de bMin) ->
537(x*0.05) -> 545`. Mientras el 0.0f sea el `else` del `fsel` de
`bMax(dot, 0.0f)`, ese camino es el mas largo y `sched1` lo subira siempre.

**El unico mecanismo de GCC 2.95 que baja un `lis` hasta su uso despues de
`sched1` es `update_equiv_regs`** (`local-alloc.c`), y su condicion esta leida
entera:

```c
if (REG_N_REFS(regno) == 2 && REG_BASIC_BLOCK(regno) < 0
    && rtx_equal_p (XEXP (note,0), SET_SRC (set)))    -> reg_equiv_replace[regno] = 1;
...
else if (depth == 0 && GET_CODE(equiv_insn)==INSN && GET_CODE(insn)==INSN
         && REG_BASIC_BLOCK (regno) < 0)
    emit_insn_before (copy_rtx (PATTERN (equiv_insn)), insn);  /* lo MUEVE */
```

Nuestro pseudo del `high` cumple `REG_N_REFS == 2` (fila 481 lo pone, 484 lo usa)
y `rtx_equal_p` (lleva su `REG_EQUAL (high (symbol_ref))`), pero **falla
`REG_BASIC_BLOCK(regno) < 0`**: def y uso viven en el mismo bloque basico (el
que abre el `bso` de la fila 113), asi que vale >= 0 y GCC no mueve nada. El
comentario del propio GCC lo dice: *«If the register is only used in one basic
block, this can't succeed or combine would have done it»*.

**Lo que hay que buscar en la r50 es una forma de fuente que ponga el `lis` del
0.0f y su `lfs` en BLOQUES BASICOS DISTINTOS**, no una que le cambie el LUID.

### 3.1 Nueve formas de fuente mas, todas neutras o peores

Base **14 filas / 99,0772 %** (micro de `CameraMover.cpp`, 8 s):

| ensayo | filas |
|---|---:|
| base | **14** |
| `bDot` en una local antes del `bClamp` | 14 |
| `const float accel_min = 0.0f;` como argumento | 14 |
| clamp y `*= 0.05f` en dos sentencias | 14 |
| `20.0f` literal, sin `accel_max` | 14 |
| `0.05f * bClamp(...)` (conmutado) | 14 |
| `float accel;` declarada arriba y asignada | 14 |
| **`bMin(bMax(dot, 0.0f), accel_max)`** | **16** |
| **`f_speed_frequency` antes que `f_speed_magnitude`** | **16** |

### 3.2 Aviso sobre el diff de esta funcion

`objdiff` con `function_reloc_diffs=none` **no marca** las filas 117 y 123
(`lis r8, 0.05f@ha` contra `lis r8, 0.0f@ha` y `lis r11, 0.15f@ha` contra
`lis r11, 0.5f@ha`): mismo mnemonico, mismo registro, distinta reubicacion. Las
14 filas del `fncmp` son un suelo, no el total; el mapa de literales completo
esta en §3 de este informe y hay que sacarlo con `mctx`, no con las filas.

Mapa de literales verificado (los dos lados en el mismo orden de pool):
`$LC335`=`lbl_803D1D6C`=5.0f, `$LC336`=`lbl_803D1D70`=**0.0f**,
`$LC342`=`lbl_803D1D84`=1.5f, `$LC343`=`lbl_803D1D88`=20.0f,
`$LC344`=`lbl_803D1D8C`=0.05f, `$LC345`=`lbl_803D1D90`=0.15f,
`$LC346`=`lbl_803D1D94`=0.5f, `$LC347`=`lbl_803D1D98`=0.7f.

---

## 4. `Update__19TrackCarCameraMoverf` (992 B, 15 filas): la receta de su gemela NO vale

`Update__19TrackCopCameraMoverf` (948 B, **100 %**) es la funcion gemela: mismo
`bCross(&hcomp, &displacement, &up)`, mismo `bScale(&hcomp,&hcomp,vert_comp)`,
mismo `eMulVector` sobre `look_offset`, mismo `Look += look_offset`. Y cerro con
**dos barreras** (`TrackCop.cpp:336` y `:344`). Ninguna ronda las habia probado
en `TrackCar`. Medido:

| ensayo | filas | fuzzy |
|---|---:|---:|
| base | **15** | 99,6290 |
| `asm("" : "+m"(hcomp));` antes del `bScale` (receta de `TrackCop:336`) | 16 | 98,8226 |
| `bVector3 *dst = &look_offset; asm("" : "+r"(dst));` + `eMulVector(dst,…,dst)` (receta de `TrackCop:344`) | 16 | 98,8427 |
| **las dos juntas** | **19** | 98,0161 |

**La receta de `TrackCop` no se transplanta.** Queda escrito porque es lo primero
que se le ocurre a cualquiera que mire las dos funciones al lado.

Doce barreras mas, ninguna mejora:

| ensayo | filas |
|---|---:|
| base | **15** |
| `asm("" : "+m"(displacement))` tras la division | 15 (objeto identico) |
| `asm("" : "+m"(look_offset))` tras `look_offset.z = …` | 15 (objeto identico) |
| `asm("" : "+m"(lookdir))` tras `bVector3 lookdir(...)` | 15 (objeto identico) |
| `asm("" : "+m"(Look))` tras `Look = *…` | 19 |
| `asm("" : "+m"(Look))` + `"+m"(look_offset)` | 19 |
| `asm("")` tras la division | 33 |
| `asm("")` antes de `bVector3 look_offset;` | 35 |
| `asm("")` tras `Look = *…` | 39 |
| `asm("" : "+m"(Look))` tras `Look += look_offset` | 44 |
| `asm("")` tras `look_offset.z = …` | 47 |
| `asm("" : : "r"(CameraType))` tras `look_offset.z = …` | 64 |

Sigue en pie todo el diagnostico de la r48 (§1: el racimo B es de `sched2` y lo
decide el reparto del racimo C; §3: el DWARF prohibe los tres pines). **No hay
nada nuevo que probar desde la fuente sin inventar locales.**

---

## 5. Propuestas fuera de territorio

1. **`ICEReplay.cpp:171-174` — reescribir la veda.** Dice «es reparto del
   planificador, sin palanca de fuente a la vista». Es falso en las dos mitades:
   ni es el planificador ni la palanca esta en ese fichero. La sustituye §1 de
   este informe. (Es territorio mio; no la toco porque la regla 2 me prohibe
   commitear y un comentario suelto es ruido en el diff de otro agente. **Que la
   cambie quien cierre el `__static_init`.**)

2. **`scripts/regmap.py` — el veredicto PERMUTACION es inseguro cuando los
   NOMBRES difieren.** Empareja por posicion y descuadra en cuanto un tipo no
   coincide (aqui `bool bLerpLag` contra `int bGeneric`), y entonces inventa una
   permutacion r26<->r27 que no existe y manda al permutador guiado a mutar las
   dos locales que **ya estan bien**. Propuesta minima: cuando dos locales
   consecutivas se emparejan con tipos distintos, decirlo (`<-- TIPO DISTINTO`) y
   **no** contarlas como permutacion; y ofrecer un `--align-by-reg` que empareje
   por registro cuando los nombres no ayuden.

3. **`scripts/fncmp.py` / `fndiff.py` — avisar de las filas enmascaradas.** En
   `TerrainVelocityNoise` hay dos `lis` con el mismo registro y distinto literal
   que no salen como diferencia (§3.2). Bastaria contar, y decir en la linea de
   resumen, cuantas filas «iguales» tienen reubicaciones distintas.

4. **`UTLVector.h` (`_Storage`, 1.156 B)** — sin novedad respecto a la r48 §6:
   hace falta que el `size()` llegue a `push_back` sin pasar por `cse`, o sea por
   parametro, y eso cambia la firma de un metodo que consumen decenas de
   unidades. **No lo toque.**

5. **`bMath.hpp`** — nada que proponer (la r48 cerro en positivo los registros de
   `bCopy` y `bScale`).

---

## 6. Dos metricas mias que mintieron (el modo de fallo es reutilizable)

**a) El `-I` de la sombra iba DETRAS de los cflags.** `mtry.py`/`mrows.py` de la
r48 construyen la orden como `[cc] + F + extra`, y `F` ya trae los `-I` reales de
`build.ninja`. Un `-I sombra` metido en `extra` **no gana nunca**: los diez
primeros ensayos de `SignedMod` y los ocho de `GetKey` salieron «todos
identicos», que se lee como «forma irrelevante» y es en realidad «no compilé
nada distinto». Lo cace con un control que TIENE que cambiar (`a -= c + 1;` ->
99,7983 / 16 filas). En `r49_cam` los dos scripts aceptan ya `--pre-I <dir>`, que
va delante de todo.

**b) El detector de punteros de fila filtraba por el registro BASE.** Buscaba
`addi rN, r30, K`. Con el relleno puesto, el asignador cambia la base a **r29** y
los cuatro `addi` seguian ahi tan campantes: dieron «arreglado» con 9 cargas
cuando en realidad habia **cinco**. La serie entera de umbrales (9, 12, 16, 20)
que saque con esa version es basura. El detector bueno (`det.py`) toma los
registros que se usan como base de `stw` en la region y solo entonces busca el
`addi`; y toda conclusion se contrasta contra el **tamano** que da `full.py`, que
es la verdad.

**La regla que sale de las dos: ningun barrido vale sin un control que obligue a
cambiar, y ninguna metrica indirecta vale sin contrastarla contra el tamano.**

---

## 7. Lo que dejo en `scratchpad/r49_cam/` (696 kB, sin volcados RTL)

| fichero | que hace |
|---|---|
| `cc.py` | los cflags reales de `zCamera` desde `build.ninja`, con `-I` absolutos |
| `mtry.py` / `mrows.py` / `mctx.py` | micro-TU: compila un `.cpp` y diffea UNA funcion contra el `zCamera.o` objetivo. **Ahora con `--pre-I`** |
| `pre.txt`, `tu_tc.cpp`, `tu_ice.cpp`, `tu_cm.cpp`, `tu_rep.cpp` | **la TU reducida**: preambulo de la SourceList + un solo `#include`. Reproduce las tres funciones byte a byte en **8 s** |
| `tu_full49.cpp` | la unidad entera reconstruida (preambulo + los 27 includes), para `asm.py` |
| `mkinc2.py`, `sh.py` | **sombra completa** de `Camera/` con `-I` delante, y el banco de variantes sobre ella |
| `vs.py` | banco de variantes sobre una copia suelta de un `.cpp` (sin sombra) |
| `full.py` | **la unidad entera** a un `.o` de scratch y la lista de lo que no casa, en **19 s** (la r47 tardaba 50) |
| `asm.py` | la unidad (o un micro) a `.s`, para leer el codigo sin objdiff |
| `det.py` | **el detector de punteros de fila** de la tabla de `ReplayCategory` |
| `fctx.py` | como `mctx` pero sobre el `fj.json` de `full.py` |
| `i1..i4.py` | bancos de `SignedMod`, `GetKey`, sitio de llamada y barreras de `ICEMover` |
| `c1.py` | banco de `TerrainVelocityNoise` |
| `t1.py`, `t2.py` | receta de `TrackCop` y las doce barreras de `TrackCar` |
| `s6.py`, `s7.py`, `s9.py`, `s9b.py` | los tres barridos del `__static_init`: umbral, tamano y **sitio** |

### 7.1 Censo de finales de linea de `Camera/` (los tres tipos conviven)

Un parche con una sola forma casa **0 de N sin avisar**. Censo completo:

* **LF puro** — `CameraMover.cpp/.hpp`, `ChaseCamAI.cpp`, `Camera.hpp`,
  `CameraAI.hpp`, `CameraNoise.hpp`, `Movers/TrackCar.cpp`, `Movers/Cubic.cpp`
  y `.hpp`, `Movers/CopView.cpp`, `Movers/SelectCar.cpp` y `.hpp`,
  `Movers/Still.cpp`, **`ICE/ICEReplay.cpp`** y `.hpp`, `ICE/ICEMath.hpp`,
  `ICE/ICEManager.hpp`, `ICE/ICEMover.hpp`, `ICE/ICEAnchor.cpp` y `.hpp`,
  `ICE/ICEAnimScene.cpp`, `ICE/ICEData.cpp` y `.hpp`, `ICE/ICEOverlays.cpp`,
  `ICE/ICEPoint.cpp`, `ICE/ICERender.cpp`.
* **CRLF entero** — `Camera.cpp`, `CameraAI.cpp`, `CameraInfo.hpp`, los **siete**
  `Actions/*.cpp`, `ICE/ICEMover.cpp`, `ICE/ICEManager.cpp`,
  `ICE/ICEAnimScene.hpp`, `ICE/ICEPoint.hpp`, `Movers/TrackCop.cpp` y `.hpp`,
  `Movers/TrackCar.hpp`, `Movers/Rearview.cpp` y `.hpp`, `Movers/Showcase.cpp` y
  `.hpp`, `Movers/CubicTweaks.hpp`.
* **MIXTOS** (los que muerden) — `Movers/DebugWorld.cpp` **300 CRLF / 8 LF** y
  `Movers/DebugWorld.hpp` **33 CRLF / 12 LF**.

Ojo con `ICE/ICEReplay.cpp`: es **LF**, aunque sus vecinos `ICEMover.cpp` e
`ICEManager.cpp` sean CRLF.

---

## 8. Abierto, por orden de premio

1. **`__static_initialization_and_destruction_0` (3.604 B)** — §1. Ya no es
   «107 rtx que no existen»: son **22 cargas / ~66 rtx**, y **la ventana esta
   acotada a los inicializadores estaticos de 12 ficheros**
   (`Movers/Cubic.cpp` .. `ICE/ICEReplay.cpp`). Con ellos la funcion queda en
   3.604 B exactos y 6 filas de permutacion ya vetada. Lo siguiente es censar el
   andamiaje de `expand_vec_init` de esa ventana contra los arrays con
   constructor del objetivo. **Es el mas tratable de los cinco y el segundo mas
   gordo.**
2. **`Update__8ICEMoverf` (3.868 B)** — §2. Treinta formas de fuente descartadas
   con cifra y el DWARF confirmado IDENTICO salvo `frame`. Toca **permutador
   ciego**, no mas formas a mano.
3. **`TerrainVelocityNoise` (1.192 B)** — §3. Objetivo nombrado y con condicion
   de codigo: que el `lis` del 0.0f y su `lfs` caigan en **bloques basicos
   distintos**, para que `update_equiv_regs` lo baje hasta el uso.
4. **`_Storage` (1.156 B)** — fuera de territorio, sin novedad.
5. **`Update__19TrackCarCameraMoverf` (992 B)** — §4. Agotado desde la fuente: la
   receta de la gemela `TrackCop` empeora y el DWARF prohibe los tres pines.
