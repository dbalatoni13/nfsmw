# r52 — zWorld / zWorld2

Territorio: 6 funciones, 8.568 B. **CERO bytes cerrados.** Lo que sí sale de la
ronda es el mecanismo completo, leído del fuente de GCC y comprobado con el
volcado, de las dos cosas que bloquean cuatro de las seis: **`allocno_compare`**
y **`rank_for_schedule`**. Con eso `HolePunchAvoidables` deja de ser «4 filas y no
sabemos por qué» y pasa a ser **una desigualdad numérica con seis palancas
refutadas y una condición de victoria exacta**, y aparece una palanca nueva que
baja `InitAtSegment` de 12 a 7 filas (no aplicada: cero bytes con `asm` es deuda).

`git status` de `src/Speed/Indep/Src/World/`: **vacío**. `fncmp` antes/después de
las dos unidades: **idéntico** (`scratchpad/r52_world/{base,final}_fncmp_*.txt`).

| unidad | antes | después |
|---|---|---|
| zWorld | 4 funciones / 4.772 B | **igual** |
| zWorld2 | 2 funciones / 3.796 B | **igual** |

---

## 0. Lo que hay que llevarse de aquí (aunque no toques zWorld)

1. **`REG_LIVE_LENGTH` lo RECALCULA sched1.** `haifa-sched.c:5584`
   (`update_reg_usage`): `if (REG_LIVE_LENGTH (regno) >= 0) REG_LIVE_LENGTH
   (regno) = sched_reg_live_length[regno];`, alimentado por
   `find_pre_sched_live`/`find_post_sched_live`, que sólo corren con
   `reload_completed == 0`. **Refuta la frase de la r50** («`live` vale 8 porque
   lo fija el `life_analysis` ANTERIOR a sched1», §3 de `r50-world.md`, sobre
   `InitAtSegment`): el `live` que ven `local_alloc` y `global_alloc` es el del
   orden que dejó **sched1**, no el de la expansión. Todo umbral de `live` del
   proyecto es, por tanto, atacable moviendo el horario del bloque.
2. **`rank_for_schedule` tiene un escalón que no estaba en el catálogo:
   `INSN_REG_WEIGHT`, y va ANTES que la clase y que `INSN_DEPEND`.** La escalera
   completa (haifa-sched.c:4158) es:
   `INSN_PRIORITY` → **`INSN_REG_WEIGHT` (sólo si `!reload_completed`)** →
   [interbloque] → clase respecto al último programado → nº de dependientes →
   **`INSN_LUID` (gana el MENOR, o sea el orden original)**.
   `INSN_REG_WEIGHT` = **nº de SET/CLOBBER − nº de notas `REG_DEAD`/`REG_UNUSED`**
   (haifa-sched.c:5255-5339), y **gana el más pequeño**. Consecuencia práctica y
   contraintuitiva: **una comparación cuyo operando MUERE ahí pesa 0 y se programa
   antes que una cuyo operando sigue vivo, que pesa 1.** O sea: *cuántas veces usa
   la FUENTE una variable decide dónde cae su comparación izada.*
3. **`INSN_PRIORITY` de un insn sin dependientes EN EL BLOQUE es su propio
   `insn_cost`** (`priority()`: `if (INSN_DEPEND (insn) == 0) this_priority =
   insn_cost (insn, 0, 0);`). Por eso un `lfs` (coste 2-3) se programa antes que
   un `cmpwi` (coste 1) aunque los dos «no sirvan para nada» dentro del bloque.
   Con esto el horario de un preencabezado se predice a mano.
4. **`REG_N_REFS` se pondera por `loop_depth`** (`flow.c:3399`,
   `REG_N_REFS (regno) += loop_depth;`, con `loop_depth` = 1 fuera de bucles).
   Un valor definido fuera del bucle y usado una vez dentro tiene `n_refs = 3`,
   no 2; usado dos veces, 5. Es lo que explica los `pri = 30000/live` y
   `100000/live` de `allocno_compare` sin tener que adivinarlos.
5. **`scan_loop` tiene una puerta que puede DESHACER un movable** (loop.c, el
   bloque comentado «A potential lossage…»): si el bucle tiene llamadas y el
   pseudo se usa **exactamente una vez**, GCC sustituye el uso por la fuente del
   SET y **borra el insn**, con lo que deja de ser movable y no se iza. Las
   puertas son `loop_has_call`, `reg_single_usage != const0_rtx`,
   `set_in_loop == 1`, `no_labels_between_p` y `validate_replace_rtx`. Es la
   explicación candidata de por qué el objetivo NO iza
   `(high TweakKitWheelOffsetRear)` y nosotros sí (§3).

---

## 1. `HolePunchAvoidables` (zWorld2, 2.980 B, 4 filas) — el mecanismo entero

Las 4 filas son **un swap cr2/cr3**: `cmpwi crX, r29` (is_racer) y
`cmpwi crY, r31` (is_drag) en el preencabezado, más sus dos `beq`. Nada más.

### 1.1 Los cinco números que lo deciden, medidos

`allocno_compare` (global.c:627, leído del fuente del árbol):

```c
pri = (int)(((double)(floor_log2(n_refs) * n_refs) / live_length) * 10000 * size);
if (pri2 - pri1) return pri2 - pri1;      /* mayor pri primero */
return v1 - v2;                            /* empate: ALLOCNO menor primero */
```

Y el `.lreg` (`dump_flow_info` al principio del volcado de `-dl`) da:

| pseudo | qué es | n_refs | live | pri | le toca | debería |
|---|---|---:|---:|---:|---|---|
| 505 | `is_traffic` | 3 | 483 | **62** | cr4 | cr4 ✔ |
| 334 | `is_racer` | 3 | 494 | **60** | cr3 | **cr2** |
| 901 | `is_drag` | 3 | 492 | **60** | cr2 | **cr3** |
| 268 | `-5.0f` del `bClamp` | 5 | 1034 | 96 | — | — |
| 271 | `5.0f` del `bClamp` | 5 | 1030 | 97 | — | — |

`30000/492 = 60,97` y `30000/494 = 60,72`: **los dos truncan a 60, empatan y gana
el allocno menor (334)**. La lista `;; 187 regs to allocate:` del `.greg`
confirma el orden: `… 271 268 … 505 … 334 901 …`.

**CONDICIÓN DE VICTORIA, exacta**: `pri(901) > pri(334)`. Con `live(334) = 494`
(pri 60) basta `live(901) ≤ 491`. Con `live(901) = 492` (pri 60) basta
`live(334) ≥ 501`.

### 1.2 Por qué el preencabezado queda así: es el HORARIO de sched1

El orden del preencabezado en el `.lreg` (post-sched1) es

```
…368 movsf f←$LC438 | 369 def(334) | 370 movsf f←$LC437 | 371 def(901) |
 372 movsf f←$LC427 | 373-377 5×addi r31+K | 378-379 2×high(símbolo) | 380 def(505) | …
```

y **sin sched1** (`--extra "-fno-schedule-insns"`) el orden es el de
`move_movables`, o sea el de aparición en el cuerpo del bucle —
racer(97), traffic(125), drag(414)— con live 563/552/528. **Es sched1 quien sube
`def(901)` veintidós puestos y lo pega a `def(334)`.**

Y se predice al insn con la escalera de §0.2/§0.3:

- las tres comparaciones tienen `INSN_PRIORITY = 1` (coste del `cmpwi`), o sea
  que van al final, detrás de todos los `elf_high` y los `movsf`;
- `is_racer` y `is_drag` se usan **una sola vez** en la fuente → sus operandos
  MUEREN en la comparación → **weight 0**, y son las dos únicas piezas de
  relleno weight-0 disponibles cuando se acaba la cadena `high`/`movsf`: caen en
  369 y 371;
- `is_traffic` se usa **cuatro veces** → su operando NO muere → **weight 1**, y
  cae con los `addi`/`high` en 380. **Ahí está su ventaja: `live` 483 y `pri` 62.
  El objetivo la quiere ahí, y ahí está.**
- dentro de cada grupo manda `INSN_LUID` (el orden original), y por eso
  `movsf ←$LC427` (weight 1, porque su `high` sigue vivo: `-6.0f` se recarga
  dentro del bucle en `insn 1057`) cae detrás de `def(901)`.

Todo esto está comprobado insn a insn contra el volcado.

### 1.3 Las seis palancas, todas refutadas CON LA CIFRA

Herramienta: `scratchpad/r52_world/p.py`, que compila la TU reducida con `-dl` y
saca `live`/`n_refs`/`pri` de los cinco pseudos **identificados por PAPEL**
(las tres `compare:CC` contra 0 más pegadas al `NOTE_INSN_LOOP_BEG`, ordenadas por
su operando; y los dos `reg/v:SF` con `REG_EQUIV const_double [-5]`/`[5]`).
Ocho segundos por variante, sin objdiff. Identificarlos por número no vale:
**cualquier cambio de fuente los renumera**.

| ensayo | `live` r/t/d | orden CC | veredicto |
|---|---|---|---|
| base | 494/483/492 | T,R,D | — |
| **las 6 permutaciones de las tres declaraciones** | **494/483/492 las seis** | T,R,D las seis | La veda de r36b/r46 queda **EXPLICADA**: mover las declaraciones cambia el prólogo pero **no toca ni un insn del preencabezado de movables**. Filas: RDT 56, TRD 22, TDR 26, DRT 63, DTR 71 |
| `asm("")` en el bucle, ×N | +N/+N/+N (y FP +2N) | — | reproduce el modelo de la r48: CC pide N∈{−10,−9,−2,−1,7,8}, FP pide N∈{0,4,5,10,11}; **no se cortan salvo en −1** |
| `asm("")` antes del bucle | 494/483/492 | igual | los insns de fuente previos al bucle tienen **LUID menor** que los movables → sched1 los coloca **delante de `def(334)`** → no cuentan. Confirmada la medida de la r48, ahora con la causa |
| `asm("" : "+r"(x))` DENTRO del bucle sobre invariantes (×1…×6) | +N/+N/+N, FP +2N | igual | **`loop.c` NO iza los `asm`**: la firma es la de un insn de bucle (CC +1, FP +2). La idea de fabricar movables de cero bytes está **muerta** |
| reordenar sentencias del bucle (6 ensayos byte-neutros) | el hueco r−d sigue siendo **2** en los seis | igual | `fwd_first`, `md_first`, `gl_first`: objeto idéntico. `av_before_ld` +1 insn, `c2m_first` +2 |
| **mantener `is_drag` vivo tras la comparación** (`asm("" : : "r"(is_drag))` detrás del bucle) | **494/484/476** | **D,T,R** | **el mecanismo FUNCIONA**: al no morir el operando, la comparación pasa a weight 1 y se va 11 puestos más allá. Pero **se pasa de largo** (pri 63 > 62) y además **cuesta 12 B** (2992/2980: `is_drag` pide un preservado que cruza el bucle). Refutada por tamaño |
| el mismo, dentro del bucle (cabeza / detrás del ternario) | 495/485/477 | D,T,R | 45 y 47 filas, **2992 B**. Igual |
| `asm("" : : "r"(is_racer))` detrás del bucle | 494/483/492 | igual | **sin efecto en los `live`** y +8 B |

**Sin la barrera `asm("" : "+f"(offset_change))`**: `live` 493/482/491 →
pri 60/62/**61** → orden **T,D,R correcto**, y **el empate FP también se resuelve**
(1028/1032 → 97/96). O sea: **el reparto entero sale exacto**. Lo que queda son
**46 filas** de un solo racimo (filas 522-611): el `fmuls` de `extra_width` sube
~20 filas y se lleva consigo su `lis/addi/lfs`. Diff completo en
`scratchpad/r52_world/d_nobar.txt`.

### 1.4 Dónde está la función, en una frase

**Hace falta un ancla para ese `fmuls` que no sea un insn.** La barrera actual es
exactamente el `+1` que rompe los dos empates, y:

- no puede ir antes del bucle (LUID),
- no puede ser un movable (`loop.c` no iza asms),
- no puede colocarse en un bloque del bucle donde las CC estén muertas (quitarla
  baja los tres `live` en 1, o sea que **cuenta**),
- y `REG_LIVE_LENGTH` sólo cuenta insns de clase `'i'` (flow.c:2904, dentro del
  `else if (GET_RTX_CLASS (GET_CODE (insn)) == 'i')`): **las NOTE y los
  CODE_LABEL no cuentan**. Ésa es la única rendija que queda abierta —una
  frontera de bloque básico de cero bytes en ese punto exacto— y no sé generarla
  desde C. Es la línea de trabajo que dejo apuntada, no medida.

---

## 2. `InitAtSegment` (zWorld2, 816 B) — **12 → 7 filas** con una palanca nueva

Las 12 filas de la base son un swap limpio r10/r11 entre el valor de
`_12WRoadNetwork.fNodes` y el `(high lbl_8040E9A8)`.

**`asm("" : : : "r0");` inmediatamente delante de
`fStartPos = roadNetwork.GetNode(...)` deja 7 filas y 816/816** (99,65686 →
99,73039 %). Arregla las siete filas del `fNodes` y rompe cuatro nuevas: la
cadena de `fNodeInd == 0` (`lbz/subic/subfe/rlwinm/lhzx`) se va de r0 a r11 y el
`high` de r11 a r8. **NO LA APLICO**: cero bytes con `asm` puesto es deuda, y
media función regresa. Queda como escalón medido para la r53.

Sensibilidad de la posición y del registro (todo 816/816):

| ensayo | filas |
|---|---:|
| base | 12 |
| `asm("" : : : "r0")` **justo delante** de `fStartPos` | **7** |
| … repetido dos veces seguidas / precedido de un clobber de r10 | 7 / 7 |
| … **seguido** de otro clobber (r10 o r11) | **12** (se anula: tiene que ser el último insn antes de la cadena) |
| … entre `fStartPos` y `fEndPos` / detrás de `fEndPos` / delante de `SetLaneInd` | 39 / 58 / 58 |
| clobber de r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r26,r27,r29,`cc`,`memory` en el mismo sitio | **12 los quince** |
| clobber de r13 / r14 / r16 / r20 / r24 / r30 / r31 | 30 / 24 / 24 / 24 / 35 / 16 / 64 |
| `asm("")` a secas en el mismo sitio | 12 → **el efecto es el registro, no el insn** |
| `asm("" : : : "fr0" / "lr" / "ctr")` junto al de r0 | 12 |
| `register int i0 asm("r0"/"r11") = fNodeInd == 0;` (con y sin el clobber) | 15 / 7 — **el pin no llega**: idéntico a una local sin pin |

**Los dos andamios existentes siguen pagando** (regla de la r50, remedidos):
quitar `register int guard asm("r28")` + `asm("" : "=r"(guard))` y/o
`asm("" : "+f"(endOffset) : "r"(guard))` da **49 filas** en las tres
combinaciones. Y r28 es el mejor registro para el guard: r27 → 15, r29 → 16,
r26 → 18, r30 → 20, r25 → 28, r24 → 33. Añadir un clobber al `asm` que ya existe
(r0/r10/r11 como tercera sección) no mueve nada: 12 las tres.

---

## 3. `UpdateWheelYRenderOffset` (876 B, 7 filas, −4 B) y `RenderFlaresOnCar` (2.908 B, 18 filas, +4 B) — **el mismo frente**

Las dos son **materialización de direcciones**, y en sentidos opuestos:

- **UWY**: el objetivo emite `lis TweakKitWheelOffsetRear@ha` **DENTRO** del bucle
  (fila 97) y nosotros lo izamos al preencabezado (r16) → **nos falta un insn**.
  El objetivo sí iza el de `TweakKitWheelOffsetFront` (r14). El volcado `-dL`
  (`Loop from 159 to 852`) enseña nuestros movables uno a uno:
  `Insn 282: regno 151 (life 1) → 960` = `(high Front)`,
  **`Insn 361: regno 169 (life 1) → 962` = `(high Rear)`**, más el `0x43300000`,
  cuatro literales y dos copias; y dos pares marcados `done … matches`, que es
  `combine_movables`.
- **RFC**: el objetivo materializa `lbl_8040AD04@ha` **una sola vez** en r16 (fila
  123, `CarRender.cpp:4061` según `lmap`) y lo usa en las filas 460, 600 y 601;
  nosotros lo materializamos **dos veces** (r9 en la 452 y r30 en la 461) →
  **nos sobra un insn**. Las otras 15 filas son el corrimiento de registros que
  eso arrastra.

**Lo que sí queda cerrado con la cifra:**

- **La forma de la fuente NO es la palanca en UWY.** Cuatro reescrituras del
  acceso a `TweakKitWheelOffsetRear` (local intermedia; local en las dos ramas;
  ternario; condición invertida) dan **7 filas y 872 B las cuatro**: CSE las
  normaliza antes de `loop`.
- **Las cflags de `CarRender.cpp` están VERIFICADAS**, con el control obligatorio
  (el sha1 del objeto cambia en los tres casos) y midiendo las 599 funciones que
  reproduce la TU:

  | bandera quitada | mejoran | empeoran |
  |---|---:|---:|
  | `-fgcse` | 0 | 26 |
  | `-fforce-addr` | 0 | 26 |
  | `-fforce-mem` | 0 | 10 |
  | `-fmove-all-movables` (r50) | 0 | 9 |

  **Frente de cflags cerrado**: el problema no está ahí.

**La hipótesis viva**, y es concreta: la puerta de §0.5 en `scan_loop`. Si en el
objetivo `(high Rear)` se usa **una sola vez**, GCC lo funde en su uso y lo borra
→ no hay movable → se queda en el bucle, que es exactamente lo que se ve. En
nuestra RTL el pseudo llega a `loop` con más de un uso (o falla
`no_labels_between_p`). **Lo que hay que mirar en la r53 es el volcado `.cse`
(no el `.loop`) y contar los usos del `(high Rear)` antes de `loop`**; es el
mismo método que cerró `CullParts` («mira `.cse` y `.cse2` por separado»).

---

## 4. `SetMemoryPoolSize` (304 B, 2 filas) y `DefragmentPool` (684 B, 23 filas)

**`SetMemoryPoolSize`**: sigue siendo el `stw CarLoaderMemoryPoolNumber` que se
adelanta al `addi …@l`. Siete ensayos nuevos, todos negativos:

| ensayo | filas | B |
|---|---:|---:|
| pool por local + `asm("" : "+r"(pool))` | 2 | **308** |
| … con el `asm` repetido detrás de la asignación | 2 | **300** |
| `asm("" : "+m"(CarLoaderMemoryPoolNumber) : : "r0")` (barrera de ranura) | 5 | 304 |
| `asm("" : "+m"(CarLoaderMemoryPoolNumber))` | 5 | 304 |
| `asm("" : : : "r0")` / `"r11"` / `asm("")` detrás de la asignación | 5 | **300** |

El diagnóstico de la r49 se sostiene y ahora tiene el fuente detrás: en t=15 el
`addi` es **clase 1** (depende del `lis` programado en t=14) y el `stw` clase 3,
y la clase va **antes** que `INSN_DEPEND`, así que el truco de «darle un segundo
dependiente» no llega. Para invertirlo, el último insn de t=14 no puede ser el
`lis`; y el `lis` no gana su ciclo porque su prioridad
(`1 + 1 + P(call)`) es menor que la del `lwz` (`2..3 + P(call)`). **Palanca
pendiente: alargar en uno la cadena `lis → … → llamada`**, que en esta función no
se puede sin bytes.

**`DefragmentPool`**: las 23 filas son una **permutación de siete allocnos
globales** (r16↔r18, r17↔r16, r21↔r22, r22↔r21, r25↔r27, r27↔r25, r18↔r17), o sea
`allocno_compare` otra vez. Catorce clobbers barridos en dos posiciones
(delante de `bGetTicker()` y delante de `eWaitUntilRenderingDone()`), **todos
684/684 y todos iguales o peores**:

    tk_r0 23 · tk_r31 23 · tk_r27 49 · tk_r25 52 · tk_r21 53 · tk_r22 55 ·
    tk_r16 57 · tk_r17 59 · tk_r30 59 · tk_r18 60 · wt_r27 53 · wt_r25 56 ·
    wt_r16 61 · wt_r17 63

Sigue en pie la corrección de la r50 (`live(82) ≥ 173`, no 163). Y ahora que
sabemos que ese `live` lo fija **sched1**, el ataque correcto es el horario del
bloque, no la fuente.

---

## 5. Vedas nuevas y caducadas

**Nuevas (con la cifra):**

1. **Las seis permutaciones de `is_racer`/`is_traffic`/`is_drag` no pueden mover
   el reparto cr2/cr3**: los `live` salen **idénticos** en las seis. No es una
   veda empírica, es estructural.
2. **`loop.c` no iza los `asm`**: no se pueden fabricar movables de cero bytes.
3. **Un `asm` de fuente antes de un bucle nunca cae entre los movables** (LUID).
4. **Mantener vivo el operando de una comparación izada la retrasa** — palanca
   real y nueva, pero en `HolePunchAvoidables` **cuesta 12 B**.
5. **`-fgcse`, `-fforce-addr` y `-fforce-mem` verificadas** para `CarRender.cpp`.
6. En `InitAtSegment`, **sólo el clobber de r0 mueve algo**, y sólo pegado a
   `fStartPos`: otros 15 registros y `asm("")` a secas dan 12 filas.

**Caducada:** la frase de la r50 «el `live` lo fija el `life_analysis` anterior a
sched1» (§3, `InitAtSegment`). Es **falsa**: `update_reg_usage` lo sobreescribe.

---

## 6. Propuestas fuera de territorio

1. **`scratchpad/r52_world/p.py` debería subir a `scripts/` como
   `livepri.py`**: dada una unidad y una función, saca `n_refs`, `live` y el
   `pri` de `allocno_compare` de los pseudos que se le pidan, **identificados por
   papel y no por número**, y dice el orden que va a salir. Ocho segundos, sin
   objdiff. Todo near-miss de «dos registros intercambiados» se contesta con una
   orden en vez de con media ronda. La lección de identificación es genérica:
   **cualquier cambio de fuente renumera los pseudos**, así que un banco que
   cablee `334`/`901` se rompe en la primera variante (a mí me pasó).
2. **`scratchpad/r52_world/ft.py`**: es el `flagtest.py` de la r50 aceptando
   **varias banderas por invocación** y también `+bandera` para añadirlas. Cuatro
   frentes de cflags cerrados en una orden.
3. **Al catálogo, la escalera completa de `rank_for_schedule`** (§0.2) con
   `INSN_REG_WEIGHT` en el segundo escalón y la regla de que **el LUID menor
   gana**. `nfsmw-rank-for-schedule.md` tiene los niveles 1, 3, 4 y 5 pero le
   falta el 2, que es el que decide en las tres funciones de esta ronda.
4. **Al catálogo, `INSN_PRIORITY = insn_cost` para los insns sin dependientes en
   el bloque.** Es lo que explica que una comparación izada acabe siempre al
   final del preencabezado, detrás de todas las cargas.
5. **Corolario operativo del §0.1**: cuando un near-miss se cierre sobre un
   umbral de `live`, hay que atacar **el horario de sched1**, no la fuente. Y el
   experimento de control es de 15 s: `--extra "-fno-schedule-insns"` y comparar
   los `live` del `.lreg`.

---

## 7. Artefactos

`scratchpad/r52_world/` (1,1 MB; borrados los volcados RTL, los `.o` y los json
grandes, y también los de `scratchpad/rtl/` que había generado):

- `base_fncmp_*.txt` / `final_fncmp_*.txt` — el antes/después de las dos unidades.
- `d_hpa_base.txt`, `d_ias_base.txt`, `d_dp_base.txt`, `d_smp_base.txt`,
  `d_uwy_base.txt`, `d_rfc_base.txt` — los `fndiff` citados.
- `d_nobar.txt` — las 46 filas de `HolePunchAvoidables` **sin** la barrera, que
  es la lista de lo que habría que arreglar si aparece un ancla de cero insns.
- `lmap_rfc.txt` — el asm del objetivo de `RenderFlaresOnCar` con la línea de
  fuente al lado (de ahí sale `CarRender.cpp:4061`).
- `p.py` (sonda de `live`/`pri` por papel), `b.py` (banco que junta la sonda con
  el recuento de filas), `ft.py` (flagtest multibandera), `h.py`/`v.py`
  (heredados de la r50, repuntados a este directorio).
- `s1..s5.py`, `w1..wb.py` — los bancos de cada tanda, con las variantes exactas.
- `*.cpp.HEAD` — copias de partida de las cuatro fuentes. **Aviso vigente desde
  la r49**: `v.py` restaura desde `.HEAD`; refresca la copia antes de lanzar una
  tanda si has editado la fuente a mano.
