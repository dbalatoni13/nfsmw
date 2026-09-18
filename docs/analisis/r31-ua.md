# Ronda 31 — ua: las tres funciones que desbloquean una SourceList

Encargo: `Smackable::Smackable` (zPhysics), `_bOutput` (zBWare) y
`ActualReadJoystickData` (zPlatform), «211.276 B detrás de tres funciones».

## 0. Verificación del encargo — las tres cifras REPRODUCEN al dígito

`build_direct.py zPhysics zBWare zPlatform` (4 ok) + `triage.py --muro`:

    3120 B   99.262%  zPhysics   __9Smackable...            faltan 3, sobran 1 (1 SUST)
    1588 B   99.244%  zPlatform  ActualReadJoystickData__Fv faltan 2, sobran 0
    5180 B   99.983%  zBWare     _bOutput__FP11bOutputInfo… MURO, 2 registros

`measure.py` de cierre, y **cada unidad tiene EXACTAMENTE una función pendiente**,
que es la del encargo:

| unidad | casado | total | funciones |
|---|---|---|---|
| zPhysics | 143.004 | 146.124 | 717/718 |
| zBWare | 30.592 | 35.772 | 238/239 |
| zPlatform | 27.792 | 29.380 | 135/136 |

`audit.py`, **dos pasadas** en las tres: **0 FALLA** (717 / 238 / 135 ok).

**Ninguna función cerrada. Cero regresiones. NO he tocado `src/` en ningún
momento**: todos los ensayos se compilaron sobre copias en el scratchpad con la
SourceList reescrita al vuelo (§7). `git diff` de mis tres ficheros:
`bPrintf.cpp` y `JoyE.cpp` intactos; `Smackable.cpp` sale modificado, pero es el
`Smackable_RigidCount = 0` que ya estaba en el árbol al empezar (r30), **no mío**.

---

## 1. EL HALLAZGO DE LA RONDA: cerrar la función NO basta en NINGUNA de las tres

El encargo avisaba de que podía no ser suficiente. **Lo he medido, y no lo es en
ninguna.** Herramienta nueva: `c31ua_doldiff.py` (§7), que enlaza como
`trypromo.py` pero **dice qué sección del DOL falla y cuánto**.

| unidad | veredicto `trypromo.py` | lo que falta ADEMÁS de la función |
|---|---|---|
| **zPhysics** | **ENLACE FALLA** | **9 símbolos indefinidos** (abajo) |
| **zBWare** | DOL ROTO `7abe5da6b433` | `.data`(2) **−320 B**, `.data`(3) **−64 B** |
| **zPlatform** | DOL ROTO `05ca0f3c3f39` | `.data`(2) **−1.888 B**, `.data`(3) **−32 B** |

### 1.1. zPhysics: los 9 indefinidos, y UNO YA ESTÁ RESUELTO Y PROBADO

`trypromo.py` sólo imprime los dos primeros errores (`err[:2]`); con el listado
entero son **9 símbolos distintos**:

| símbolo | qué es |
|---|---|
| `_Q43UTL3COMt7Factory3ZRC14BehaviorParamsZ8BehaviorZ6UCrc32_9Prototype.mHead` | estático de plantilla que **zAI.o referencia** y nuestro zPhysics.o deja UNDEF |
| `LoaderBounds__FP6bChunk`, `UnloaderBounds__FP6bChunk` | dos funciones que **zMisc.o referencia** y no definimos |
| `lbl_803F748C`, `803F7490`, `803F7494`, `803F7498`, `803F74A8`, `803F74B8`, `803F74BC` | el frente §1 del brief: 7 literales que emitimos como `$LC` |

**El primero está arreglado y verificado.** Añadiendo al TU de zPhysics (probado
en una copia, **NO aplicado al árbol** — es encargo de `lk`/`und`):

```cpp
template <>
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype
    *UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype::mHead = NULL;
```

el error desaparece del enlace. Es el mismo patrón que ya usan `zAI.cpp:90` y
`zSim.cpp` («IMPLEMENT_FACTORY no vale: el argumento lleva comas»).
El objeto extraído lo define en `.data` (`shndx=4`); el nuestro lo tenía
`SHN_UNDEF`.

### 1.2. zBWare está a 384 B de datos — y su `.text` YA MIDE LO MISMO

Es el caso más cerca de promocionar de los tres:

    text0/text1/text2:  MISMO offset, MISMA dirección, MISMO tamaño
    data2  0004c3e0 / 0004c520   D = -320
    data3  000413a0 / 000413e0   D = -64

Los **38.339 rachas de 2 B** que salen en `.text1` son la consecuencia de esos
384 B: al acortarse `.data`(2) se corren todas las direcciones posteriores y
cambia el campo `@l` de cada referencia a dato. **No son 38.339 problemas: es
uno.**

### 1.3. Los símbolos de datos que faltan, clasificados (y un aviso)

`c31ua_datadiff.py <unidad>` (§7) compara las tablas de símbolos:

| unidad | símbolos | bytes | `lbl_` | CON NOMBRE | gap/pad |
|---|---|---|---|---|---|
| zBWare | 160 | 1.943 | **1.584** | 260 | 99 |
| zPlatform | 176 | 15.034 | **5.516** | 8.252 | 1.266 |
| zPhysics | 391 | 5.271 | **4.068** | 588 | 615 |

**AVISO, y corrige la lectura del bucket «CON NOMBRE»:** los dos mayores son
estáticos locales cuyo SUFIJO NUMÉRICO lo pone GCC y no coincide entre
compilaciones — **no faltan, están renombrados**:

    zPlatform  profdata.25844 (8.192 B, .bss)  <->  el nuestro es profdata.1470
    zBWare     text.4279        (256 B, .bss)  <->  el nuestro es text.3277

Descontados esos dos, «CON NOMBRE» son **60 B en zPlatform y 4 B en zBWare**. Es
la versión-datos de la regla «el 0 % que miente: es el nombre, no el código», y
conviene aplicarla antes de contar bytes de dato que escribir.

---

## 2. `Smackable::Smackable` (zPhysics, 3.120 B): 26 → **19 filas**, los DOS
   mecanismos estructurales eliminados, y SIN un solo `asm`

Base 99,261536 % / 26 filas / 3.112 B. Banco: la SourceList entera recompilada
con `Smackable.cpp` sustituido por una copia del scratchpad (17 s + 8 s de
medida). El control reproduce **143.004/146.124 B y 717/718 funciones**, o sea el
objeto de `ninja` exacto.

### 2.1. Primero, una veda que CADUCA: la receta de flags de la r27 es falsa

El comentario del fuente (r27) dice que `if (simple_physics)` pelado con
`-fno-cse-follow-jumps -fno-cse-skip-blocks` da «`faltan 0, sobran 0`, la única
vez que esta función llega a cero». **Medido sobre zPhysics entero:**

| ensayo | forma | Smackable | unidad |
|---|---|---|---|
| base | — | 99,26154 | 143.004 B, 717 fn |
| v1 | pelado | 98,01538 | 143.004 B, 717 fn |
| v2 | base + los dos `-fno-cse-*` | **94,61795** | **67.408 B, 600 fn** |
| v3 | pelado + los dos `-fno-cse-*` | **94,62436** | **67.408 B, 600 fn** |

O sea: los flags **cuestan 75.596 B y 117 funciones** en la unidad y **ni
siquiera arreglan Smackable** (el multiconjunto casa, el reparto de registros
no). La frase «llega a cero» hay que leerla como «el multiconjunto llega a cero»,
que no es lo mismo. **Eje cerrado con medida.**

### 2.2. El mecanismo, ya completo

Los 26 diffs son **tres** cosas, y las dos primeras son el MISMO fenómeno:

1. `cmpwi cr2,r17,1`+`bne` contra `cmpwi cr2,r17,0`+`beq` (2 filas) — el
   `== true` es constructo nuestro (deuda ya declarada en el fuente).
2. **El `0` compartido** (4 filas): `stw r29,0x58(r1)` y `stw r29,0x94(r1)`
   contra `stw r0,0x58(r1)` y `stw r11,0x94(r1)`, más `active` en r28 en vez de
   r29. `cse` funde el `const 0` de `UCrc32 smack_class` con el `0` del último
   argumento de `RBComplexParams`: el pseudo pasa a 3 referencias, `global_alloc`
   le da r29 y empuja `active` a r28.
3. La permutación (r27,r28) ↔ (r28,r29) de la tercera `LoadBehavior` (6 filas,
   4 B: un `mr r27,r30`).

**Lo nuevo es por qué (2) es la llave, y qué lo rompe.** `update_equiv_regs` sólo
borra la definición de un pseudo con `REG_EQUIV` constante y **UN solo uso**; con
la fusión de `cse` hay dos usos, el pseudo entra en `global_alloc` con
`prio = floor_log2(3)*3/46*10000 = 652` y se lleva un salvado. En el objetivo los
dos ceros nacen en **registros distintos y volátiles (r0 y r11)**, que es la
firma de dos pseudos de un uso rematerializados por `reload`.

### 2.3. La receta que llega a 19 filas (ensayo `r3`)

Dos cambios, **los dos naturales, cero `asm`, cero pines**:

```diff
+    unsigned int collision_mask = 0;
+
     bool active = !virginspawn || mPersistant;

     UCrc32 smack_class;
@@
-    if (simple_physics == true) {
+    if (simple_physics) {
@@
         RBComplexParams rbparams(..., dimension, geoms,
-                                 active, 0);
+                                 active, collision_mask);
```

`99,18205 % / 3.116 B / 19 filas`. **Desaparecen las filas de los apartados (1) y
(2) enteros**: `cmpwi 0`+`beq` correctos, `active` en r29, `li r0,0` +
`stw r0,0x58(r1)` como el objetivo y el `li` del argumento separado. Lo que queda
son la permutación de (3) —6 filas— y la **posición/registro** del `li` del
argumento (`li r11,0` en la fila 612 del objetivo contra nuestro `li r0,0` en la
619) más dos filas sueltas (`lwz r0,0xd8(r1)` contra `lwz r11,…`).

**El ORDEN de la declaración es esencial y está medido**: `collision_mask` tiene
que ir **ANTES de `bool active`** (o sea en otro bloque básico que el `0` del
argumento). Declarada después (`s4`) o entre `active` y `smack_class` (`s3`) se
vuelve a 65 filas.

### 2.4. Por qué NO lo dejo puesto

`matched_code` es todo-o-nada: `r3` aporta **0 bytes** igual que la base, y baja
el porcentaje de la función de 99,2615 a 99,1821 (`pctsnap` lo leería como
EMPEORA). Regla del brief: si no cierra, se revierte. **Queda aquí el parche
completo para que la r32 lo aplique en un paso.**

### 2.5. Ensayos numerados (34) — base 99,261536 % / 26 filas / 3.112 B

| # | forma | % | B | filas |
|---|---|---|---|---|
| v1 | `if (simple_physics)` pelado | 98,01538 | 3.120 | 65 |
| v2/v3 | los dos `-fno-cse-*` (±pelado) | 94,62 | — | — |
| p1 | `UCrc32 smack_class = UCrc32();` | **idéntico** | | |
| p2 | `UCrc32 smack_class((unsigned int)0);` | **idéntico** | | |
| p3 | último argumento `0u` | **idéntico** | | |
| p4 | `active = mPersistant \|\| !virginspawn` | 98,60000 | 3.112 | 39 |
| p5 | `collision_mask` local **detrás** de `smack_class` | **idéntico** | | |
| p6 | p5 + pelado | 98,01538 | 3.120 | 65 |
| p7 | segundo `UCrc32` + `GetValue()` de máscara | 99,00128 | 3.116 | 126 |
| **p8** | **`smack_class` declarada ANTES de `active`** | 99,15256 | 3.116 | **16** |
| p9 | p7 + pelado | 97,79230 | 3.124 | 138 |
| **pa** | **`UCrc32 smack_class(UCrc32::kNull);`** | **99,47308** | **3.120** | **12** |
| q1 | p8 + pelado | 97,48975 | 3.120 | 71 |
| q2 | pa + pelado | 97,81026 | 3.124 | 67 |
| q3 | llaves de más en la rama `then` | **idéntico** | | |
| q4 | pa + q3 | 99,47308 | 3.120 | 12 |
| q5 | `active` con `\|` en vez de `\|\|` | 98,16410 | 3.116 | 66 |
| q6 | p8 + q3 | 99,15256 | 3.116 | 16 |
| r1 | `collision_mask` **antes de `active`** | 99,17436 | 3.116 | 21 |
| r2 | `collision_mask` entre `active` y `smack_class` | **idéntico** | | |
| **r3** | **r1 + pelado** | **99,18205** | **3.116** | **19** |
| r4 | `collision_mask` antes de `active`, sin usarla | **idéntico** | | |
| r5 | `collision_mask` detrás de `smack_class` | **idéntico** | | |
| r6 | r1 sin `smack_class` | 98,50384 | 3.108 | 129 |
| s1 | r3 con `collision_mask` en la cabecera del cuerpo | 98,94487 | 3.112 | 25 |
| s2 | r3 + `smack_class` antes de `active` | 98,77564 | 3.112 | 25 |
| s3 | r3 con `const unsigned int` | 98,01538 | 3.120 | 65 |
| s4 | r3 con `collision_mask` **detrás** de `active` | 98,01538 | 3.120 | 65 |
| s5 | r3 con `smack_class` detrás de `collision_mask` | 98,77564 | 3.112 | 25 |
| t1 | r3 sin la asignación a `rbbehavior` (variable muerta) | **idéntico a r3** | | |
| t2 | r3 con `if (mPersistant)` invertido | 99,17564 | 3.116 | 20 |
| t3 | r3 con la `LoadBehavior` de EFFECTS delante del `if` | 94,38718 | 3.120 | 143 |
| t4 | r3 con `Sim::Param` en local | 97,64359 | 3.124 | 75 |

`pa` (99,47308 % / **3.120 B exactos** / 12 filas) es la mejor cifra que ha dado
esta función en todo el proyecto, pero **2 de sus 12 filas son el propio truco**
(`lis`+`lwz` de `kNull` donde el objetivo pone `li r0,0`): sirve como
demostración del mecanismo, no como fuente.

### 2.6. Lo que queda, dicho con precisión

Con `r3`, `cse` **sigue entrando** en la rama `else` (por eso el pelado deja de
doler sólo cuando el último argumento es una variable y no un `const_int`). En el
objetivo, `simple_physics` vive en r17 con **2 referencias** (`mr r17,r9` y
`cmpwi cr2,r17,0`) y muere ahí — o sea que **el objetivo no sustituye ni el
parámetro ni el pseudo de `smack_class`**, que es lo que pasaría si `cse` no
siguiera ese salto. Las dos condiciones de `cse_end_of_basic_block` para
seguirlo —`LABEL_NUSES(etiqueta del else) == 1` y un BARRIER delante— se cumplen
en el objetivo (comprobado: la etiqueta `.L_80219E94` tiene **una** referencia).
**No he encontrado la forma de fuente que lo corte, y es lo único que falta.**

---

## 3. `_bOutput` (zBWare, 5.180 B): sigue siendo muro, con una lectura nueva

Las 2 filas son un intercambio adyacente en el bloque de `bPrintf.cpp:1044-1045`:

    objetivo   subi r0,r22,1 | addi r23,r1,0x10 | subf r3,r0,r3 | addi r5,r22,1
    nuestro    subi r0,r22,1 | addi r5,r22,1    | subf r3,r0,r3 | addi r23,r1,0x10

`addi r23,r1,0x10` es **`&cvtbuf[0]`** (verificado: `&cvtbuf[62]` es
`addi r25,r1,0x4e`), y su único consumidor está en `bPrintf.cpp:1151`
(`stringLength = (&cvtbuf[63] - p) - 1`, que el objetivo emite como
`subi r9,r3,0x3f` + `subf r9,r9,r23`). O sea: **es una inserción de PRE**, y el
objetivo la mete en DOS bloques adyacentes (`8005F4EC` y `8005F4F8`), que es la
firma de `pre_edge_insert` en dos caminos.

Lo que añado al techo de la r28 (`LUID(PRE) < LUID(desiredPrecision)` en
`sched1`, con `INSN_REG_WEIGHT` y número de dependientes empatados):
`insert_insn_end_bb` inserta **inmediatamente delante del salto que cierra el
bloque**, y `desiredPrecision = precision + 1;` es la última sentencia del bloque
en el fuente. **Por construcción el LUID de la inserción de PRE es siempre el más
alto del bloque, se ponga como se ponga el fuente** — lo que explica por qué las
formas `s1`…`s8` de la r28 (incluido intercambiar las dos sentencias) dan objeto
idéntico. El único eje vivo sería **impedir la inserción de PRE**, es decir que
`&cvtbuf[0]` esté disponible al entrar en el bloque; eso quita la instrucción en
vez de moverla.

**Cero ensayos nuevos y cero cambios**: el techo es de construcción y no tengo
forma que lo rompa. Van 87 formas medidas entre r24-r28. `zBWare` queda como
estaba.

**Lo que sí aporta esta ronda para zBWare** es el §1.2: su `.text` ya mide lo
mismo que el objetivo y **el bloqueo real de la promoción son 384 B de datos**.

---

## 4. `ActualReadJoystickData` (zPlatform, 1.588 B): el mecanismo al nivel de la
   insn, y 8 formas más vedadas

Somos 1.580 B; faltan exactamente `mr r9,r0` y `andi. r11,r9,0x8000`, que en el
objetivo van en **`JoyE.cpp:307`**, o sea el `if (v & 0x8000) { v = 0; }` que
sigue a `joy_data->stickX = v;` (línea 306). Las otras 17 filas son dos problemas
independientes ya conocidos: r18/r19 permutados (6) y el orden de operandos del
`add` (3, vedado por `fold` en la r30), más su arrastre.

### 4.1. Qué pase la borra — medido con volcados RTL propios

`c30mx_rtl.py` sobre `JoyE.cpp` (stub con `Timer.hpp`; el objeto suelto
reproduce **99,24433 % / 1.580 B / 19 filas exacto**). Rastreando la insn 513/514
por pases:

| pase | ¿vive? |
|---|---|
| `.rtl` (expand), `.jump`, `.cse`, `.loop`, `.cse2` | sí, `(set (reg 296) (and (reg 125) 32768))` + compare + jump |
| **`.flow`** | **sí** — flow borra `(set (reg 125) 0)` (el `v = 0`, muerto) pero **no** el `and` |
| `.combine` | sí, fundidos en `*extzvsi_internal1` = `(parallel[(set CC (compare (zero_extract v 1 16) 0)) (clobber (scratch:SI))])` |
| `.regmove`, `.sched`, `.lreg`, `.greg` | sí (4 `extzv` en los cuatro clamps) |
| **`.sched2`, `.jump2`** | **3**: el cuarto ha desaparecido |

O sea: **muere entre `global_alloc` y `sched2`**, y el responsable es el
`jump_optimize` posterior a `reload`: al borrar el `beq` (que ya salta a la
etiqueta siguiente, porque flow se llevó el `v = 0`), `delete_computation` sigue
la nota `REG_DEAD` del CC hasta el `andi.` y lo borra también — y con él se
llevaría el `mr`.

**La condición que lo salvaría está en `delete_prior_computation`: un PARALLEL
sólo se borra si NINGUNO de sus `SET` escribe algo distinto del registro de la
nota.** El nuestro es `(set CC …) + (clobber …)` → un solo SET, al CC → se borra.
**Si el `and` tuviera un destino VIVO, `combine` emitiría la forma de dos SET
(`andi. rP,rX,0x8000` con `rP` real) y `delete_computation` NO podría tocarla.**
Eso explica el objetivo al byte, incluido por qué sobrevive también el `mr`: no
se borra porque nadie llega a él.

También queda aclarado un detalle que despistó a la r30: los cinco
`mr rN,r11` de las líneas 308/317/318 **están en los dos lados** y salen del
patrón `*floatsidf2_loadaddr` (`(set (reg 11) (unspec[0] 11))`, **cero bytes**)
más sus copias; que en el objetivo r11 lo escriba el `andi.` es coincidencia de
reparto, no una dependencia.

### 4.2. Ensayos numerados (20), todos idénticos o peores

Base 99,24433 % / 1.580 B / 19 filas.

| # | forma | % | B | filas |
|---|---|---|---|---|
| k1 | `stickY` con `v` en vez de un `data` local | 98,90428 | 1.580 | 38 |
| k2 | `joy_data->stickX = (short)data;` dentro del bloque | 99,05541 | 1.576 | 15 |
| k3 | `v &= 0x8000;` sin `if` | 98,47607 | 1.576 | 23 |
| k4 | k3 + k1 | 98,16121 | 1.576 | 41 |
| k5 | el `if (v & 0x8000)` **duplicado** | 96,04786 | **1.588** | 108 |
| k6 | `if ((v & 0x8000) != 0)` | **idéntico** | | |
| k7 | `v = v & 0x8000; if (v != 0)` | **idéntico** | | |
| k8 | el `if` sobre una variable `w` local | 98,47607 | 1.576 | 23 |
| k9 | k1 conservando el `if` | 98,90428 | 1.580 | 38 |
| k10 | `if (…) v = 0; else v = v;` | **idéntico** | | |
| k11 | `v = (short)v;` extra antes del `if` | **idéntico** | | |
| k12 | `w = v; if (w & 0x8000) w = 0;` con `w` declarada arriba | 98,47607 | 1.576 | 23 |
| w1 | `data = (short)data; stickX = data; v = data;` | 99,21915 | 1.580 | 20 |
| w2 | w1 con `v = data;` delante del `stb` | **idéntico** | | |
| w3 | `v = v & 0x8000; if (v != 0) v = 0;` | **idéntico** | | |
| w4 | `int w = v & 0x8000; if (w) v = 0;` en bloque propio | **idéntico** | | |
| w5 | `int v` declarada en el bloque de `slot` | **idéntico** | | |
| w6 | `int v` declarada junto a `port` (fuera del bucle) | **idéntico** | | |
| w7 | `if (…) v=0;` **más** `v = v & 0x8000;` detrás | **idéntico** | | |
| w8 | `w = v & 0x8000; if (w) v = 0;` con `int w = 0;` arriba | **idéntico** | | |

`k5` es el **décimo** caso documentado de la trampa del tamaño exacto: da los
1.588 B del objetivo y sube a 108 filas.

**Veda nueva, con la sentencia:** `if (v & 0x8000) { v = 0; }` de `JoyE.cpp:152`
y sus alrededores — **20 formas esta ronda**, más las 8 de la r30.
Ninguna consigue que el valor del `and` quede vivo, porque en todas `v` (o su
sustituto) muere antes de la siguiente sentencia y `flow` se lleva el `v = 0`.
**Lo que hace falta es un USO posterior del valor `v & 0x8000`, y todo uso
posterior emite instrucciones que el objetivo no tiene.**

---

## 5. Vedas nuevas y vedas que CADUCAN

### Nuevas

| función | sentencia barrida | resultado |
|---|---|---|
| `Smackable` | los flags `-fno-cse-follow-jumps -fno-cse-skip-blocks` sobre la unidad | **−75.596 B, −117 funciones**, y Smackable en 94,62 |
| `Smackable` | la declaración de `UCrc32 smack_class` — 6 formas (p1,p2,p5,r2,r4,r5) | 5 idénticas, 1 peor |
| `Smackable` | el sitio de `unsigned int collision_mask = 0;` — 6 sitios | sólo **antes de `bool active`** paga |
| `Smackable` | la tercera `LoadBehavior` y el `if (mPersistant)` — 4 formas sobre r3 (t1-t4) | 1 idéntica, 3 peores |
| `ActualReadJoystickData` | `if (v & 0x8000) { v = 0; }` (JoyE.cpp:152) — 14 formas | 8 idénticas, 6 peores |
| `_bOutput` | — | la inserción de PRE tiene el LUID más alto **por construcción** |

### Caducadas

1. **«`if (simple_physics)` pelado + los dos `-fno-cse-*` es la única vez que
   esta función llega a cero»** (r27, en el comentario del fuente). Falso como
   avance: `faltan 0, sobran 0` es el multiconjunto; el porcentaje **baja** a
   94,62 y la unidad pierde 117 funciones.
2. **«El `if` pelado cuesta el derrame del parámetro»** (r27/r29/r30). Cierto
   **sólo mientras el último argumento de `RBComplexParams` sea un `const_int`**.
   Con `collision_mask` en una variable declarada antes de `active`, el pelado
   pasa de 65 filas a 19 (§2.3).
3. **«Las dos vedas rotas de la r27 no se rompen entre sí»** (r30). Cierto para
   las combinaciones que probó, pero **la combinación buena existe** y es la de
   §2.3; lo que faltaba era el ORDEN de declaración.
4. **«El objetivo no sustituye ninguno de los dos ceros: son dos pseudos
   distintos»** (comentario del fuente, r27). Es una inferencia del asm; el
   mismo asm sale de **un** pseudo de un solo uso rematerializado por `reload`
   (`update_equiv_regs`). La distinción importa porque cambia la palanca: no hay
   que separar los ceros, hay que dejarlos con **una** referencia.

---

## 6. Lo que NO he probado

- **`Smackable`**: la forma de fuente que impida a `cse` seguir el salto (la
  única capa viva); las 6 filas de la permutación (r27,r28)↔(r28,r29) con
  `alloc.py` sobre el `.greg` —no he generado volcados RTL de zPhysics, que son
  de cientos de MB—; el permutador.
- **`_bOutput`**: cero ensayos; no he tocado el eje «que `&cvtbuf[0]` esté
  disponible al entrar en el bloque», que quita la instrucción en vez de moverla.
- **`ActualReadJoystickData`**: el par r18/r19 (6 filas) y la combinación
  `s8_calCL` + devolver el `li r21,1` a su sitio, que la r30 dejó pendiente.
- **Los datos**: no he tocado `splits.txt`, `keep.lst` ni `symbols.txt` (encargo
  de `lk`/`und`); tampoco he aplicado la línea del `mHead` al árbol.
- **`LoaderBounds`/`UnloaderBounds`**: localizados como indefinidos, no buscados
  en el fuente.

---

## 7. Herramientas nuevas (scratchpad, prefijo `c31ua_`)

| fichero | qué hace |
|---|---|
| **`c31ua_doldiff.py <unidad>[=obj]`** | como `trypromo.py` pero **compara el DOL sección a sección** contra `orig/GOWE69/sys/main.dol`: dice qué sección cambia de tamaño y, si no, agrupa las diferencias en rachas. Acepta `unidad=ruta.o` para enlazar con un objeto del scratchpad **sin tocar `build/`** |
| **`c31ua_datadiff.py <unidad>`** | símbolos de DATOS que el `.o` extraído define y el nuestro no, clasificados en `lbl_` / CON NOMBRE / gap-pad |
| `c31ua_ubuild.py <unidad> <salida.o> [--sub <include> <fichero>] [flags]` | compila **la SourceList entera** con sus cflags de `build.ninja`, sustituyendo un `#include` por una copia modificada. **No escribe en `src/` ni en `build/`**: es lo que ha permitido barrer zPhysics sin pisar a los otros agentes |
| `c31ua_unitmeas.py <unidad> <obj> [-o s.json] [--cmp a b]` | bytes al 100 % y % por función de un objeto suelto |
| `c31ua_smk_sweep.py <spec.py>` | barrido CRLF-safe sobre la SourceList con `c31ua_ubuild` |
| `c31ua_fnd.py <unidad> <sym> <obj> [lo hi]` | `fndiff.py` contra un objeto arbitrario |

Reutilizados: `c27mx_run.py` (JoyE, 15 s/variante) y `c30mx_rtl.py` (volcados
RTL; `-dsGLtfc` y `-dNSlgRJ` dan los 12 pases sin ICE).

## 8. Convivencia y estado del árbol

- Scratchpad con prefijo `c31ua_`. **Disco: 15 GB libres al empezar, 19 GB al
  acabar** (borrados mis objetos de 6 MB y un volcado JSON de 22 MB). Hay 1,7 GB
  de otros agentes en el scratchpad: **no he tocado nada que no sea mío**.
- **Ediciones en `src/`: NINGUNA.** No he tocado `configure.py`,
  `config/GOWE69/*`, `splits.txt`, `keep.lst` ni `symbols.txt`; **no he escrito
  ensamblador de instrucciones**, no he usado ni un `asm` en ningún ensayo, no he
  tocado los `#if defined(__ANDROID__)`, **no he congelado nada** (no he cambiado
  código) y **no he hecho commit**.
- Los ficheros de `git status` que tocan mi encargo (`Smackable.cpp` con
  `Smackable_RigidCount = 0`) venían de la r30 y **no los he revertido**.
