# Ronda 24 — zEAXSound2: `ResolveCarBanks`, la SUST cerrada

**Resultado: `ResolveCarBanks` 99,12360 % → 99,60842 % (+0,485 pp), APLICADO al
árbol.** `triage.py` la saca de «FALTA O SOBRA CODIGO» y la mete en **MURO**: de
`faltan 1, sobran 1, de ellas 1 SUST` a **`faltan 0, sobran 0, 0 SUST`**. La
secuencia de instrucciones es ahora **la del objetivo, opcode a opcode**; lo que
queda es **exclusivamente reparto de registros** (96 filas, todas
`DIFF_ARG_MISMATCH`, contra las ~451 de antes).

`measure.py --cmp` (base y medida seguidas, con `build_direct.py` delante de las
dos): **+0 B, +0 funciones, 0 unidades cambian** — `matched_code` es todo-o-nada
y la función sigue sin cerrar. `pctsnap.py --cmp`: **MEJORAN 1, EMPEORAN
ninguna**.

`audit.py Speed/Indep/SourceLists/zEAXSound2`: **921 ok / 0 FALLA**, dos pasadas
(una al empezar y otra tras el cambio) y **la salida de las dos es idéntica byte
a byte**. Congelada con `frozen.py cong` (huella `786df30a89effbd8`).
`STATEMGR_CarState.cpp` sólo lo incluye `zEAXSound2.cpp`: no hay cabecera
compartida que medir.

Unidad: **158.400/170.256 B = 93,0364 %, 921 funciones al 100 %** (igual).

    ANTES   7508 B  99.12360%  faltan 1, sobran 1, 1 SUST   bgt-1, ble+1
    AHORA   7508 B  99.60842%  MURO (0 faltan, 0 sobran, 0 SUST)
    340 B  95.88236%  BindToData      — no tocada
    392 B  94.93877%  Play…RoadNoise  — no tocada

---

## 1. Lo primero del encargo: los volcados `-dj`/`-dJ`. Y el pase NO es `jump.c`

Receta del brief (preproceso propio a `.ii` + `cc1plus` a mano, porque `ngccc`
borra el temporal de `-da`): `c24eax2_rtl.py <variante.cpp> <tag> <letras>`.

**El hueco de `+0x110C` es el bloque sintético que `expand_fixup` crea para el
`goto LoadRemainingEngines` de la línea 225** (cola de BeginRule2). Se ve al
dígito en el `.jump` de las dos formas:

    5053 jump  COND (leu) ->          <- if (size+size <= 4)
    5054 note  NOTE_INSN_BLOCK_BEG
    5055 LINE  225                    <- goto LoadRemainingEngines
    5057 note  NOTE_INSN_DELETED      <- fixup->before_jump
    5064 LINE  227 / 5065 jump -> BeginRule2
    5066 barrier
    5067..5072  BLOCK_END             <- EL HUECO (+0x110C)
    5073 LABEL BeginRule3

Ese `goto` sale de un ámbito **sin destructores**, así que `fixup_gotos` no le
mete nada: en `.jump`, en `.cse` y en `.gcse` el hueco está **vacío en las dos
formas**.

### El pase que lo llena es `loop`, no el cross-jumping

`c24eax2_hole.py` / `c24eax2_hole2.py` miran qué hay entre el barrier del
`b BeginRule2` y la etiqueta `BeginRule3` en cada volcado por pase:

| pase | forma `f1` (colas invertidas) | forma base |
|---|---|---|
| `.jump` | 0 insns | 0 |
| `.cse` | 0 | 0 |
| `.gcse` | 0 | 0 |
| **`.loop`** | **20 insns reales, 3 llamadas a `~Instance`** | 0 |
| `.cse2` … `.jump2` | las mismas 20 | 0 |

**Corrige la hipótesis de la r23**: el hueco no lo llena `expand` ni el
*cross-jumping*; lo llena **`loop_optimize` (`find_and_verify_loops`)**, que
**reubica bloques enteros** y deja una etiqueta nueva en el destino
(`code_label 11948`, `11949` — uids > 11000, creados por ese pase).

### Qué reubica y adónde — medido en las dos formas

`loop` mueve **el bloque `then` de cada una de las dos colas interiores** (el que
cuelga justo detrás del salto condicional, con las limpiezas del `goto`), y lo
mete **en el hueco muerto (detrás de un BARRIER) que precede a la etiqueta a la
que salta ese bloque**:

| forma | `then` de BeginRule3 | destino | `then` de BeginRule4-int | destino |
|---|---|---|---|---|
| base | limpieza de `goto LRE`, acaba en `b LRE` | **justo antes de `LoadRemainingEngines` = +0x17B8** | ídem | ídem |
| f1 (r22) | limpieza de `goto BeginRule3`, acaba en `b BeginRule3` | **+0x110C** | limpieza de `goto BeginRule4` | +0x110C |

Y en el objetivo la pastilla (limpieza fundida de los dos `goto LRE`, acaba en
`b +0x17e4`) está en **+0x110C**. O sea: **el objetivo tiene la polaridad BASE y
la reubicación que la base manda a +0x17B8 acaba en +0x110C.**

El caso mínimo de la r23 (`c24eax2_min.cpp`, 1 s por variante) **no reproduce
esto**: ahí `goto BeginRule2;` seguido de `BeginRule2:` se borra, no queda
BARRIER delante de la etiqueta de BeginRule3, y los bloques se van al final de la
función en las cuatro polaridades (135 insns las cuatro). **El hueco muerto
delante de la etiqueta destino es condición necesaria.** Veda: el caso mínimo no
sirve para este frente.

---

## 2. La palanca: un BUCLE de verdad entre la cola exterior de BeginRule4 y `LoadRemainingEngines:`

Si en el camino que `loop` recorre hacia atrás desde la etiqueta destino aparece
un `NOTE_INSN_LOOP_END`, el punto de inserción **salta el bucle entero** y sigue
más atrás. Con la base, ese camino es `<limpieza de cierre de bloque>` y para en
el BARRIER del `b BeginRule4` de la línea 300 → +0x17B8. Metiendo un bucle real
que **acabe entre esa cola y `LoadRemainingEngines:`**, el punto de inserción se
va hasta **+0x110C**.

**Aplicado (`d1`)**, y es semánticamente idéntico:

    BeginRule4: {
        int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
        while (n >= 0) {                     // antes: if (n >= 0) {
            ...
            if (FinalEngines.size() + AIEnginesWeWantToLoad.size() > 4) {
                goto BeginRule4;
            }
            break;                           // nuevo
        }
    }

…**más la vuelta de las dos colas interiores a la polaridad BASE** (deshace la
inversión de la r22, que era un parche para mover la pastilla y que el objetivo
no tiene: el objetivo hace `ble +0x110c` y `ble +0x1114`).

El objetivo, byte a byte, queda reproducido:

    objetivo  +0110c mr r3,r20 ; b +0x1118 ; mr r3,r31 ; li ; bl ; mr r3,r17 ; li ; bl ; b +0x17e4
    ahora     lo mismo, en +0x110c
    tail BR3  ble +0x110c   ; luego mr r3,r20 … b BeginRule3   (limpieza EN LINEA)
    tail BR4i ble +0x1114   ; luego mr r3,r31 … b BeginRule4   (limpieza EN LINEA)
    tail BR4e bgt +0x16b4   (comparte la cola TEMPRANA, como el objetivo)

### La meseta: 311 palabras distintas, cinco formas del bucle, objeto idéntico

Oráculo `c23eax2_probe.py` (insns / bytes / palabras distintas contra el objeto
objetivo / offset de la pastilla) + `c22eax2_pct.py` para el %.

| # | qué | insns | B | palabras | pastilla | % |
|---|---|---|---|---|---|---|
| `a0_f1` | árbol de la r23 (control) | 1877 | 7508 | **609** | NO | 99,12360 |
| `a1_bb` | base pura | 1878 | 7512 | 906 | +0x17b8 | 98,44433 |
| **`b3_do_r4`** | base + `BeginRule4: { do { … } while (0); }` | **1877** | **7508** | **311** | **+0x110c** | **99,60842** |
| `c3_n_out` | ídem con `int n` fuera del `do` | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| `c4_forbrk` | ídem con `for (;;) { … break; }` | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| `c6_do_if` / `d4_do_if` | `do { if (n>=0) {…} } while (0);` | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| **`d1_while_brk`** | **`while (n >= 0) { … break; }`  ← APLICADO** | 1877 | 7508 | **311** | +0x110c | **99,60842** |
| `d2_for_brk` | `for (; n >= 0; ) { … break; }` | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| `e1_do_in_if` | el `do` dentro del `if`, envolviendo el ámbito del objeto | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| `e3_for_decl` | `for (int n = …; n >= 0; ) { … break; }` | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| `e6_wtrue` | `while (true) { if (n<0) break; … break; }` | 1877 | 7508 | 311 | +0x110c | 99,60842 |
| `e7_do_body` | `do { if (n<0) break; … } while (0);` | 1877 | 7508 | 311 | +0x110c | 99,60842 |

**Siete formas distintas del bucle dan el MISMO objeto de código.** La meseta es
exactamente **311**, igual que la r22 encontró la de 609 y la de 906.

### Vedas nuevas de esta ronda (todas medidas, todas revertidas)

| # | qué | cifra |
|---|---|---|
| `a5_lre_nop` | sentencia vacía entre `LoadRemainingEngines:` y el `while` | 1878, 7512, **906** = base |
| `a6_lre_for` | el `while` de LRE como `for(;;)` con `break` | 906 = base |
| `c1_empty_in` | `do { } while (0);` **vacío** antes del cierre de BeginRule4 | 906 = base (se borra antes de `loop`) |
| `c2_empty_out` | ídem entre el cierre de BeginRule4 y `LoadRemainingEngines:` | 906 = base |
| `b1_do_all` | `do{…}while(0)` envolviendo BeginRule3+BeginRule4+LRE+el `while` | 1878, 7512, 920, pastilla +0x17b8, **98,34576** |
| `b4_do_lre` | `do{…}while(0)` sólo alrededor del `while` de LRE | 1878, 7512, 920, +0x17b8 |
| `d6_lre_do` | ídem con otro anidamiento | 920, +0x17b8 |
| `b2_do_r34` | `do{…}while(0)` envolviendo BeginRule3+BeginRule4 (LRE fuera) | 1882, 7528, **1390**, +0x1110, **96,08418** |
| `e8_cont` | `goto BeginRule4` exterior como `continue` de un `while(true)` | 1883, 7532, **1414** |

**Veda fuerte: el bucle tiene que CERRAR entre la cola exterior de BeginRule4 y
`LoadRemainingEngines:`, y tiene que contener código real.** Un bucle vacío se
borra antes de `loop`; uno que abarca también LRE o sólo LRE no mueve nada; uno
que abarca BeginRule3 y BeginRule4 lo empeora mucho.

---

## 3. Lo que queda: 96 filas, y son DOS cosas

`c22eax2_rows2.py` sobre el objeto del árbol: **96 filas, las 96
`DIFF_ARG_MISMATCH`.** Ni una `REPLACE`, `INSERT` ni `DELETE`.

1. **Una permutación GLOBAL r23 ↔ r24** (~85 filas, de la fila 17 a la 1870).
   Fila 17: objetivo `mr r23,r31`, nuestro `mr r24,r31`. Fila 118: objetivo
   `mr r24,r23`, nuestro `mr r23,r24`.
2. **El racimo final** (filas 1659-1777, en el `for` sobre `CarSoundConn` de
   después de LRE): `r22`↔`r23`, `r20`↔`r22`, `r21`↔`r20`, `r24`↔`r21`. Es el que
   `dwbody.py` ve como `eax_car; // r22` (orig) contra `// r24` (nuestro) y
   `found; // r20` contra `// r22`.

**Los dos racimos que la r20 dejó anotados YA NO ESTÁN**: las filas 1418-1443
(`srawi r5/r6` contra `r4/r5`) no aparecen en el diff nuevo. Del censo DWARF sólo
sobrevive `unsigned int num; // r5` (orig) contra `// r4` (nuestro), dentro de
`Vector::indexof`, y la expansión de `~engineaudio` de más del original (que es
consecuencia de la colocación, no causa).

### La permutación r23/r24, con la tabla del compilador

`lreg.py Speed/Indep/SourceLists/zEAXSound2 "CSTATEMGR_CarState::ResolveCarBanks"`
(nombre **desmanglado**) — 1.046 pseudos. Los que importan y sus vecinos por
prioridad:

    pseudo  n_refs  live_len  prioridad  confl   reg
    2682    8       82        2926       36      r26
    3274    8       84        2857       55      r25
    1104    3       11        2727       21      r4
    3278    148     3818      2713       1057    r24   <- el grande (la base del vector)
    2868    10      115       2608       78      r23

`floor_log2(148)*148/3818*10000 = 2713` ✓ — el modelo de `global_alloc` de la r23
cuadra al dígito. `3278` se asigna **antes** que `2868` y coge r24; el objetivo lo
quiere en r23. Para girarlo hace falta **o** que la prioridad de `3278` baje de
2608 (`n_refs ≤ 142`, o `live_length ≥ 3972`) **o** que aparezca un allocno más
con prioridad entre 2857 y 2713 que se lleve r24. No he encontrado forma de
fuente que lo haga.

**Y la permutación la introduce el bucle**: la forma base (sin bucle) tiene
`mr r23,r31` como el objetivo, y `f1` también. O sea, **el bucle es un sustituto
funcional, no el mecanismo del original**: reproduce la colocación exacta del
objetivo pero paga una permutación de dos callee-saved que la base no pagaba. El
neto es muy favorable (451 filas → 96), pero el original consigue +0x110C **sin**
bucle, por una vía que no he encontrado.

### Diagnóstico de pases sobre la forma NUEVA — ninguno decide

Ocho banderas medidas sobre el árbol ya cambiado, **las ocho idénticas** (1877
insns, 7508 B, 311 palabras): `-fno-rerun-loop-opt`, `-fno-move-all-movables`,
`-fno-strength-reduce`, `-fno-gcse`, `-fno-rerun-cse-after-loop`,
`-fno-force-addr`, `-fno-expensive-optimizations`. Con las 24 de la r22 y las 22
de `BindToData`, **la permutación r23/r24 no es de banderas**.

---

## 4. Trampa confirmada: los números de etiqueta son un contador global

Reconfirmada la advertencia de la r23. Aquí los **uids de insn SÍ son estables**
entre pases de la *misma* compilación (5065, 5066, 5073 valen en `.jump`,
`.gcse`, `.loop`, `.cse2` y `.jump2`), y por eso `c24eax2_hole.py` puede anclarse
en ellos; pero **entre dos compilaciones distintas no sirven**, y por eso
`c24eax2_hole2.py` ancla por **nombre de etiqueta** (`"BeginRule3"`).

---

## 5. Lo que NO he probado

- **`ResolveCarBanks`**
  - **La permutación r23/r24**: formas de fuente que bajen `n_refs` del pseudo
    3278 por debajo de 143 o suban su `live_length` por encima de 3971. No he
    identificado siquiera qué variable es (148 refs, vive toda la función, base
    de un `Vector`); haría falta cruzar el `.greg` con el RTL.
  - **El racimo final** (filas 1659-1777, `eax_car`/`found` en el `for` sobre
    `CarSoundConn`). Es el único trozo con nombre de variable en el DWARF, y es
    código limpio y aislado: es por donde yo seguiría.
  - **`unsigned int num; // r5` contra `// r4`** en `Vector::indexof`
    (`UTLVector.h`, de otro agente): no lo he tocado.
  - Una vía para poner la pastilla en +0x110C **sin** meter un bucle. Sigue sin
    aparecer; lo que sé es que hace falta que desaparezca el BARRIER que deja el
    `b BeginRule4` de la línea 300, y las tres formas de esa cola exterior están
    vedadas desde la r23 (12 medidas).
  - El permutador, ni guiado ni ciego.
- **`BindToData` (340 B, 95,88236 %)**: **no la he tocado.** La meseta de la r23
  está completa (tres posiciones posibles, la actual es la mejor); queda el
  permutador y identificar los pseudos 160/142 en el RTL.
- **`Play__16CARSFX_RoadNoise` (392 B, 94,93877 %)**: **no la he tocado.** Sigue
  vigente el AVISO: quitar el clamp de `SetType` da los 392 B clavados y rompe
  `InitSFX`.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, los bloques
  `__ANDROID__`, ni ningún fichero de otro agente. **No he hecho commit.**

## 6. Herramientas dejadas en el scratchpad (prefijo `c24eax2_`)

| | |
|---|---|
| **`c24eax2_rtl.py <cpp> <tag> <letras>`** | stub + preproceso propio + `cc1plus` a mano con los volcados por pase (`jJsGLtfckSlgR`). Deja `rtl24/<tag>/` |
| **`c24eax2_hole.py <dump…>`** | qué hay entre el barrier 5066 y la etiqueta 5073 en cada volcado — **es lo que señala el pase culpable** |
| **`c24eax2_hole2.py <etiqueta> <dump…>`** | igual pero anclando por **nombre** de etiqueta, que es lo único comparable entre compilaciones |
| `c24eax2_ctx.py` / `c24eax2_reg.py` | contexto de cadena alrededor de un uid, y todo (notas incluidas) entre dos uids |
| `c24eax2_seq.py` | secuencia de uids reales, para diferenciar dos pases y ver qué se ha movido |
| `c24eax2_ext.py` | extrae `ResolveCarBanks` de un volcado por pase |
| `c24eax2_tgt.py` | desensamblado completo del objetivo (o de cualquier `.o` con `OBJ=`), listo para `diff` |
| `c24eax2_msw.py` + `c24eax2_min.cpp` | barrido de casos mínimos (1 s). **Aviso: no reproduce este frente** |
| `c24eax2_v1..v6.py` | las tandas de variantes de esta ronda |

## 7. Convivencia

Scratchpad: entré con el disco al 98 % (14 GB libres) y salgo igual (13 GB); he
borrado los `.s` de 15 MB de mis propios volcados y dejo `rtl24/` en 34 MB.

**Congelado y aviso.** Congelé zEAXSound2 con mi cambio dentro
(`786df30a89effbd8`). Inmediatamente después, un `build_direct.py` de control
vuelve a dar **«HA CAMBIADO»** y **no es mío**: durante la sesión ha aparecido
`src/Speed/Indep/Src/EAXSound/EAXFrontEnd.cpp` modificado por otro agente, y ese
fichero está en esta unidad. Medido: `measure.py` sigue en **158.400/170.256 B,
921 funciones**, y `pctsnap.py --cmp` entre mi instantánea posterior al cambio y
la de cierre da **«MEJORAN 0, EMPEORAN ninguna»** — el cambio ajeno no mueve
ninguna función. **No he vuelto a congelar**, para no meter en la huella el
estado en curso de otro agente. Que lo decida quien cierre la ronda.

Los únicos ficheros del árbol que he tocado yo son
`src/Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.cpp` y
`docs/congelado/Speed__Indep__SourceLists__zEAXSound2.json`, más este documento.
**Sin commit.**
