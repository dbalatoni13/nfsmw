# r26 — zWorld + zAI + zGameplay + zPhysicsBehaviors

Encargo: 11 near-miss, 16.768 B. **Cierres: 0. Aporte neto +0 B, +0 funciones.**
Lo que traigo son **tres diagnósticos corregidos** —los tres del informe r25 que
mandaban el trabajo de esta ronda— y **22 ensayos numerados** con su cifra.

- **`DefragmentPool` NO es «tres locales de más»**: es una **permutación pura de
  registros** de 7 filas sobre 171 instrucciones **idénticas** (§2). Las tres
  «locales de más» **existen en el original** como pseudos del compilador, dos de
  ellas en el mismo registro. Quitarlas cuesta **−6,8 pp y +8 B**.
- **`RenderFlaresOnCar` NO es un contraste de `allocno_compare`** y bajarle
  `n_refs` a `flashHeadlights` **no puede** funcionar: el `@ha` gana su registro
  **173 puestos antes** que `flashHeadlights` en la lista real del compilador. La
  causa está en `cse`/`gcse`, no en el asignador (§3).
- **`AssignClosestOffsets` es UNA fusión de `combine`** (`mr.` contra
  `mr`+`cmpwi`), no una sentencia que falte (§4). Y el DWARF destapa que el
  original **no tiene los tres `goto`** que nosotros usamos.

---

## 0. Verificación del encargo — reproduce EXACTO

`build_direct.py zWorld zAI zGameplay zPhysicsBehaviors` + `triage.py --muro`:
las 11 funciones con el porcentaje y el tamaño del encargo, función a función.

**Calibración del árbol: CORRECTA.** `RenderFlaresOnCar` da
`faltan 0, sobran 1, de ellas 1 SUST · lis-1` y `Add__6RBGrid` da
`faltan 4, sobran 4, de ellas 4 SUST`. `triage.py` **no está roto**.

Al cerrar, tras `build_direct.py` de las cuatro unidades, `triage.py` da **las
once líneas idénticas al arranque**: ninguna regresión, ningún cambio.

## 1. `audit.py` — lista de FALLA: **VACÍA**

Una pasada sobre las cuatro unidades tras `build_direct.py`:

| unidad | FALLA |
|---|---|
| zWorld | **0** |
| zAI | **0** |
| zGameplay | **0** |
| zPhysicsBehaviors | **0** |

Sin ningún FALLA no hay nada que confirmar con una segunda pasada. No he
congelado nada (no he cerrado nada) ni re-congelado unidades ajenas.

**El árbol queda limpio**: `git status -- src/` no lista ninguno de los cuatro
`.cpp` que he tocado (todos los arneses restauran y verifican por md5). Los ocho
ficheros modificados que salen son de otros agentes.

---

## 2. `DefragmentPool` (684 B) — **la tabla vieja Y la nueva estaban mal**

### 2.1 Es una permutación pura: 171 instrucciones idénticas, 7 registros

`fndiff.py` completo: **mismo tamaño (684/684), mismos opcodes, mismos
inmediatos, mismos destinos**. Las únicas 7 filas distintas son nombres de
registro:

| pseudo | original | nuestro |
|---|---|---|
| `this` | r21 | r22 |
| `allocation_num + 1` (temporal) | r22 | r21 |
| `num_hole_filling_allocations` | r27 | r25 |
| `ChunkMovementOffset@ha` | r25 | r27 |
| `table` (base de `allocation_table`) | r18 | r17 |
| `zero` | r16 | r18 |
| `CarLoaderMemoryPoolNumber@ha` | r17 | r16 |

Son **tres grupos cerrados**: dos trasposiciones y un 3-ciclo.

### 2.2 Las «tres locales SOLO NUESTRAS» de `regmap` SÍ están en el original

`regmap --ours` marca `params` (r26), `table` (r17) y `zero` (r18) como «SOLO
NUESTRA» porque el DWARF del original no las nombra. Pero el desensamblado del
original **las tiene las tres**, como pseudos del compilador:

    addi r26, r9, DefragmentParams@l     <- params, MISMO registro en los dos
    addi r18, r1, 0x8                    <- table
    li   r16, 0x0                        <- zero

Es el límite 2 del brief («los temporales del compilador no salen en DWARF») en
su forma más cara: **`regmap` las señala, y disolverlas empeora**.

### 2.3 La lista de prioridades REAL (`scripts/alloc.py` sobre el `.greg`)

66 pseudos en el `.lreg`, 27 los asigna `local-alloc`, **39 llegan a
`global_alloc`**. La fórmula reproduce el orden del compilador (verificado por la
propia herramienta):

| # | pseudo | n_refs | live | pri | reg |
|---|---|---|---|---|---|
| 22 | 226 `ChunkMovementOffset@ha` | 8 | 54 | **4444** | r27 |
| 26 | 144 `params` | 11 | 120 | **2750** | r26 |
| 27 | 142 `num_hole` | 14 | 184 | **2282** | r25 |
| 30 | 82 `this` | 9 | 162 | **1666** | r22 |
| 31 | 227 `alloc_num+1` | 4 | 51 | **1568** | r21 |
| 35 | 148 `zero` | 4 | 118 | **677** | r18 |
| 36 | 147 `table` | 3 | 61 | **491** | r17 |
| 37 | 228 `pool@ha` | 4 | 246 | **325** | r16 |

El objetivo necesita `num_hole > params > chunk@ha` y `alloc_num+1 > this`: en
los tres grupos el orden va **al revés** del nuestro, y **no hay ni un empate**
(4444 / 2750 / 2282 son distintos). Para invertirlos harían falta cambios
grandes de `n_refs`/`live_length`, que sólo salen de cambiar el código — y el
código ya es idéntico. **Ésa es la contradicción que define el muro.**

Nota accionable: `this` (1666) y `alloc_num+1` (1568) están a un 6 %. Con
`n_refs(this) = 8` en vez de 9 la prioridad cae a 1481 y **la pareja se
invierte**. No he encontrado forma de fuente que quite una referencia a `this`
(las siete son necesarias y la de dentro del bucle pesa doble).

### 2.4 Ensayos (arnés `c26wag_inplace.py`, ciclo 7 s — `CarLoader.cpp` **sí**
compila suelto y reproduce el objeto de la unidad al dígito)

| # | sentencia barrida | resultado |
|---|---|---|
| c0 | control | **99,269005 %**, 684/684, 23 filas — reproduce |
| c1 | quitar `params`/`table`/`zero` (`DefragmentParams.`, `allocation_table[`, `0`) | **92,474 %**, **692 B** |
| c2 | orden de declaración `zero, table, params` | **98,626 %** |
| c3 | `params`/`table`/`zero` declaradas FUERA del `while` | **93,889 %** |
| c4 | orden `params, zero, table` | **98,626 %** |
| c5 | orden `table, params, zero` | 99,269 % (igual) |
| c6 | orden `table, zero, params` | 99,269 % (igual) |
| c7 | orden `zero, params, table` | **98,626 %** |

**VEDA (c1)**: disolver las tres locales del cuerpo del `while` cuesta
**−6,8 pp y +8 B**. Tercer caso medido del límite 1 del brief, y el que mata el
plan «lo más barato que queda» del informe r25.

**VEDA (c2-c7)**: **las seis permutaciones** de la declaración de
`params`/`table`/`zero` dan sólo **dos estados** (99,269 % o 98,626 %). El orden
de declaración SÍ mueve registros, pero su recorrido está agotado.

**VEDA (c3)**: subirlas al ámbito de función, −5,4 pp.

### 2.5 Lo que sí queda vivo: el bloque de `hole`

El DWARF del original tiene **un nivel de bloque más** y el ámbito de `hole`
**termina antes**:

    original   while-body(83..127) -> anon(92..126, SIN locales) -> anon(102..113) { hole }
    nuestro    while-body(83..127) ------------------------------> anon(102..126) { hole }

El bloque de `hole` del original cubre **sólo** el `bMalloc`, el test y la rama
que guarda el agujero (102..113); la rama del `else` (114..126, `bFree(hole)`,
`pNewAllocation = hole`, `movement = hole - pAllocation`) **queda fuera de su
ámbito** aunque lea `hole` — y esas tres filas usan r31, que es también el
registro de `movement`. La lectura que encaja es que el original **no lee `hole`
ahí**, sino una variable del bloque de fuera (`movement`) donde había guardado el
valor. **No lo he probado** (§8).

---

## 3. `RenderFlaresOnCar` (2.908 B) — la hipótesis de la r25 está **refutada**

### 3.1 La palanca con nombre de la r25 no puede funcionar

`scripts/alloc.py` sobre el `.greg` real (319 pseudos, 119 los coge
`local-alloc`, **200 llegan a `global_alloc`**):

| puesto | pseudo | qué es | n_refs | live | pri | reg |
|---|---|---|---|---|---|---|
| **171** | 1010 | **el `@ha` de `lbl_8040AD04`** | 6 | 264 | **454** | **r30** |
| 176 | 560 | `is_brakelight` | 4 | 260 | 307 | r17 |
| 177 | 561 | `is_headlight` | 4 | 264 | 303 | r16 |
| **199** | 267 | **`flashHeadlights`** | 3 | 512 | **58** | **r16** |

`flashHeadlights` (pseudo 267, identificado en el `.lreg`: `li 0` en la insn 570,
`li 1` en la 1326, un solo `compare`) se reparte **28 puestos y 396 unidades de
prioridad DESPUÉS** que el `@ha`. **No le quita nada a nadie**: cuando le toca,
el `@ha` ya tiene r30 desde hace mucho. Bajarle `n_refs` no puede mover el `@ha`.

### 3.2 La causa real: `cse` deja TRES `high`, `gcse` funde sólo DOS

Volcados RTL por pase (`cc1plus -dsGLtlgJ`, contando los `(set (reg N)
(high:SI (symbol_ref "lbl_8040AD04")))` dentro de la función):

| pase | pseudos de `high(lbl_8040AD04)` |
|---|---|
| **cse** | **564, 649, 651** (uno por uso) |
| **gcse** | **564, 1010** ← funde 649+651, deja 564 |
| loop | 564, 1010 |
| cse2 | 564, 1010 |
| lreg | 564, 1010 |

Y los bloques del `.lreg` cierran el porqué de que **ya no se puedan fundir**:

- **bloque 138** (preencabezado, antes del `NOTE_INSN_LOOP_BEG`): `564 = high(AD04)`.
  `update_equiv_regs` le baja la definición hasta su uso → sale como `lis r9`
  local (filas 452/454).
- **bloque 140** (dentro del bucle, tras `code_label` de la arista de retorno):
  `1010 = high(AD04)` → gana **r30** (fila 461) y sirve a los usos de las filas
  600/601.

Entre los dos hay una **etiqueta con arista de retorno**, así que `cse2` corta el
bloque extendido ahí y no puede ver que 564 ya vale. El objetivo tiene **UN solo
pseudo en r16 creado en la fila 123** (línea 4061, `is_traffic_car`) que sirve a
las tres cargas. Los registros de `flashHeadlights` / `is_headlight` /
`is_brakelight` son **consecuencia**, no causa.

### 3.3 El 0.0f está DUPLICADO en nuestra `.rodata` (y `audit.py` no lo ve)

`lmap` del objetivo: `lbl_8040AD04` (0.0f) aparece **~20 veces** en la función.
Nosotros usamos `lbl_8040AD04` en **5** sitios (el apaño `extern "C"`) y `0.0f`
—que GCC mete en `$LC606`— en los otros ~17. O sea que **nuestro objeto tiene dos
direcciones distintas para 0.0f**. `audit.py` no lo denuncia porque compara el
**valor** del literal (0.0f == 0.0f), y objdiff tampoco. Es legítimo, pero
conviene saberlo: **el objetivo tiene un solo 0.0f en toda la función**.

### 3.4 Ensayos (arnés `c26wag_patch.py`, ciclo 10 s — `CarRender.cpp` compila
suelto y reproduce las tres funciones al dígito)

| # | sentencia barrida | resultado |
|---|---|---|
| — | control | **98,61761 %**, 2908/**2912**, 18 filas |
| e1 | **todos** los `lbl_8040AD04` del cuerpo → `0.0f` | 98,054 %, **2908 B**, 62 filas |
| e2 | **todos** los `0.0f` del cuerpo → `lbl_8040AD04` (37 sitios) | 97,552 %, **2908 B**, 121 filas |
| e3 | sólo los `0.0f` ANTERIORES al bucle (22 sitios) | 97,552 %, 2908 B, 121 filas |
| e4 | e3 + los dos `base_*_intensity` | 97,552 %, 2908 B, 121 filas |
| e5 | sólo los tres `0.0f` de las líneas 2589/2596/2600 | 97,091 %, 2912 B, 88 filas |
| e6 | `sizescale` declarada antes que `intensity` en la cima del bucle | **98,62448 %**, 2912 B, 18 filas |
| e8 | `if (lbl_8040AD04 < intensity)` en vez de `intensity > lbl_...` | 98,597 %, 20 filas |
| e9 | las 4 usos posteriores → `0.0f` (queda sólo el de la cima) | 98,054 %, 2908 B, 62 filas |
| e10 | sólo el uso de la cima → `0.0f` | 98,054 %, 2908 B, 62 filas |

**VEDA (e2/e3/e4)**: el comentario del fichero dice que pasar los `0.0f` a
`lbl_8040AD04` «NO COMPILA». **Es falso**: compila perfectamente (sólo hay que
respetar `10.0f`, que contiene `0.0f` como subcadena). Lo que pasa es que
**empeora**: 97,55 %, 121 filas. La veda ahora tiene cifra en vez de una excusa.

**VEDA (e1/e9/e10)**: los tres dan **exactamente el mismo objeto** (98,054 %,
2908 B, 62 filas). En cuanto el uso de la cima del bucle y los posteriores dejan
de ser el mismo símbolo, PRE no funde nada y salen **tres** rematerializaciones;
el tamaño cuadra por casualidad (se ahorran 2 instrucciones en el guardado de CR).
Caso número nueve de «el tamaño exacto miente».

**e6 es la única variante que mejora**, y sólo **+0,007 pp** sin mover una fila:
no la aplico porque además contradice el orden de declaración del DWARF del
original (`intensity` antes que `sizescale`).

### 3.5 Barrido de banderas sobre el fichero suelto (10 banderas)

Ninguna mejora el control (98,618 %):

    -fno-gcse                    93,242 %  (2940 B, 224 filas)
    -fno-rerun-cse-after-loop    87,646 %
    -fno-cse-follow-jumps        97,882 %
    -fno-cse-skip-blocks         95,006 %
    -fno-force-addr              96,924 %
    -fno-force-mem               98,494 %
    -fno-move-all-movables       90,070 %
    -fno-rerun-loop-opt          98,618 %  (idéntico)
    -fno-expensive-optimizations 98,618 %  (idéntico)

`dwbody.py` del cuerpo: **casa entero** salvo los tres registros. No queda fuente
que corregir; queda una decisión de PRE.

---

## 4. `AssignClosestOffsets` (1.684 B) — es **una fusión de `combine`**

Sin tocar por nadie hasta ahora. El diff son 8 filas y **dos** causas:

    objetivo   mr  r9, r29 · cmpwi r29, 0 · bgt <cima>
    nuestro    mr. r9, r28 ·               bgt <cima>

`mr.` es el patrón `[set cc (compare op1 0)] [set op0 op1]`: `combine` fundió el
`cmpwi` con el `mr`. Para poder hacerlo necesita que el `compare` use el registro
que define el `mr`; o sea que **nuestro `compare` es sobre la variable (destino
del `mr`) y el del objetivo sobre el temporal (origen)**, que lo define el `subi`
de la cima del bucle — demasiado lejos para que `combine` alcance. Las otras 6
filas son el par r28/r29 intercambiado, que es consecuencia.

`lmap` del objetivo confirma que el `subi r29, r9, 1` **es la primera
instrucción del bucle** (`.L_80031B7C`, la etiqueta a la que salta el `bgt`), y
que ahí van las líneas 1427/1432/1434.

### 4.1 Ensayos (arnés `c26wag_unit.py`, unidad entera, ciclo 30 s —
`AIPursuit.cpp` **no** compila suelto: `ipv` sin declarar en la línea 655)

| # | sentencia barrida | resultado |
|---|---|---|
| a0 | control | **99,66746 %**, 1684/**1680**, 8 filas |
| a1 | `copsToAssignOffsets--;` + `} while (copsToAssignOffsets > 0);` | 99,268 %, **1684 B**, 11 filas |
| a2 | `} while (copsToAssignOffsets-- > 1);` | 99,059 %, **1688 B** |
| a3 | `copsToAssignOffsets = copsToAssignOffsets - 1;` + `while (>0)` | 99,268 %, 1684 B, 11 filas |
| a4 | decremento al PRINCIPIO del cuerpo del `do` | 97,553 %, **1700 B** |
| a5 | `} while (--copsToAssignOffsets >= 1);` | **IDÉNTICO** al control |
| a9 | `} while (0 < --copsToAssignOffsets);` | **IDÉNTICO** al control |
| a12 | temporal `remaining` declarado fuera, `while (remaining > 0)` | 97,784 %, **1692 B** |

**a1/a3 dan el TAMAÑO EXACTO (1684 B) y rompen la fusión** (aparecen el `mr` y
el `cmpwi` por separado), pero mueven el `subi` al fondo del bucle: 11 filas
contra 8. Es la primera forma que rompe el `mr.`, y **es por donde seguiría**.

**VEDA (a5/a9)**: `>= 1` y `0 <` son la misma forma canonizada; cero efecto.
**VEDA (a2/a4/a12)**: las tres crecen el tamaño.

### 4.2 Hallazgo nuevo del DWARF: **el original no tiene los tres `goto`**

`dwbody.py` da **una sola diferencia** en toda la función, y es nuestra:

    + // Labels
    + search_offsets:     // 0x2E754
    + found_offset:       // 0x2E74C
    + searched_offsets:   // 0x2E798

El bloque anónimo con `goto search_offsets / found_offset / searched_offsets`
—el que lleva el comentario `// TODO issue with this loop`— **no existe en el
original**. Es la única diferencia estructural viva de esta función y **no la he
tocado** (§8).

---

## 5. `UpdateLoaded__SuspensionTraffic::Tire` (856 B) — mismo mecanismo, al revés

Sin tocar por nadie. `lmap` del objetivo:

- `lbl_803FB6B8` es **1.0f** y se usa **cuatro** veces.
- El objetivo **iza UNO** de sus `@ha` a **r30** (callee-saved, y por eso paga
  `stmw r30`/`lmw r30` y 8 B de marco) para exactamente **dos** usos —
  `1.0f < UMath::Abs(fwd_vel)` (nuestra línea 305) y `skid_speed > 1.0f`
  (nuestra 312)— que están **a los dos lados de la llamada a `VU0_sqrt`**. Los
  otros dos usos los rematerializa con `lis r9`.
- Nosotros rematerializamos **los cuatro** (`$LC903`), no pagamos marco, y
  salimos **+1 instrucción**.

Es el mismo eje que `RenderFlaresOnCar` y `UpdateWheelYRenderOffset`, con el
signo cambiado: aquí **falta** un izado, allí **sobra** una rematerialización.

### 5.1 Ensayos (arnés `c26wag_patch.py`, ciclo 6 s — `SuspensionTraffic.cpp`
compila suelto y reproduce)

| # | sentencia barrida | resultado |
|---|---|---|
| — | control | **97,21028 %**, 856/**860**, 21 filas |
| u1 | línea 312 `skid_speed > 1.0f` → `1.0f < skid_speed` | **IDÉNTICO** |
| u2 | línea 320 `1.0f < skid_speed` → `skid_speed > 1.0f` | **IDÉNTICO** |
| u3 | línea 305 `1.0f < Abs(fwd)` → `Abs(fwd) > 1.0f` | **IDÉNTICO** |
| u6 | `extern "C" const float lbl_803FB6B8` en las líneas 305 y 312 | 97,140 %, 23 filas |
| u7 | ídem en 305, 312 y 320 | 95,224 %, **872 B**, 44 filas |
| u8 | ídem en los seis `1.0f` de la función | 96,369 %, 864 B, 33 filas |

**VEDA (u1/u2/u3)**: GCC canoniza el orden de los operandos de la comparación
flotante; las tres formas dan **el mismo objeto**. No hay nada que rascar ahí.

**VEDA (u6/u7/u8)**: darle símbolo propio al 1.0f (el apaño que sí usa
`CarRender.cpp`) **no** provoca el izado a callee-saved; empeora en las tres
dosis.

---

## 6. Lo que NO he reabierto (y por qué)

- **El `blt`→`b` compartido** (`UpdateAllAvoidables` + `Add__6RBGrid`, 4.396 B):
  cerrado por la r25 con 27 formas y 30 banderas. **Cero ensayos míos.** La única
  pista viva sigue siendo reconstruir las líneas 96-98 del ctor de `SAP.h`
  (§2.3 del informe r25) y **no la he seguido**.
- **zGameplay** (`GTrigger`, `GenerateIndex`, 4.224 B): negativo firme de la r25
  (orden de sentencias ya correcto por mapa de líneas, `regmap` idéntico,
  empates de `sched2`). **Cero ensayos míos.**
- **`CullParts`** (836 B): `regmap` da MISMO REPARTO y ya hay cinco vedas en el
  fichero. La fila que falta (`mr r9, r0`) es una elección de `canon_reg` dentro
  de la clase de equivalencia del 0: el objetivo escoge el registro de
  `debug_print` y nosotros el de `Polarity`. Lo que falta es el bloque de
  depuración de 22 líneas. **Cero ensayos míos.**
- **`UpdateWheelYRenderOffset`** (876 B): el fichero ya trae 12 vedas medidas y
  el mecanismo es el de §5 invertido (el objetivo quiere **dos** `@ha` del 0.0f
  vivos en el preencabezado). **Cero ensayos míos.**
- **`SetMemoryPoolSize`** (304 B): ni mirado.

## 7. Aporte neto

**+0 B, +0 funciones.** Los 22 ensayos están todos revertidos; los tres arneses
(`c26wag_inplace.py`, `c26wag_patch.py`, `c26wag_unit.py`) restauran el fichero
en el `finally` y **verifican la restauración por md5**. `git status -- src/`
al cerrar no lista ninguno de mis cuatro `.cpp`, y `triage.py` da las once
funciones con el porcentaje y el tamaño idénticos al arranque.

## 8. Qué NO he probado

- **`DefragmentPool`, el bloque de `hole`** (§2.5): meter el `else` del `for`
  fuera del ámbito de `hole` (guardando el valor en `movement`, que es el
  registro que usa el original en esas tres filas). Es la única diferencia
  estructural que le queda, y encaja con los rangos del DWARF.
- **`DefragmentPool`, quitarle una referencia a `this`** (§2.3): con `n_refs` 8
  en vez de 9 la pareja `this`/`alloc_num+1` se invierte. No he encontrado la
  forma; no la he barrido.
- **`AssignClosestOffsets`, quitar los tres `goto`** (§4.2): el original no los
  tiene y es su única diferencia estructural. **Es lo que yo haría primero.**
- **`AssignClosestOffsets`, seguir por a1/a3**: dan el tamaño exacto y rompen la
  fusión `mr.`; falta subir el `subi` a la cima del bucle.
- **`RenderFlaresOnCar` / `UpdateLoaded`**: no he mirado el volcado `-dG` de
  `gcse` en sí (la tabla hash de expresiones y los conjuntos LCM), que es lo
  único que diría **por qué** PRE elige un punto de inserción y no otro. Con el
  arnés de §3.2 son 12 s por volcado.
- **`permuter.py`**: sobre ninguna de las once, ni guiado ni ciego.
- **Ninguna variante con restricciones de registro** en ninguna.
- **`CullParts`, `UpdateWheelYRenderOffset`, `SetMemoryPoolSize`, zGameplay y las
  dos del `blt`→`b`**: cero ensayos (§6).

## 9. Herramientas dejadas en el scratchpad (prefijo `c26wag_`, nada en el árbol)

- `c26wag_rtl.py` — preprocesa **cualquier** `.cpp` con los cflags exactos de su
  unidad y llama a `cc1plus` a mano con `-d<letras>`; deja los volcados RTL por
  pase. Es lo que dio §2.3 y §3.2. (`-dsGLtlgJ` = cse, gcse, loop, cse2, lreg,
  greg, jump2 sin el `.rtl`, que pesa 24 MB.)
- `c26wag_patch.py` / `c26wag_unit.py` — sustitución EN EL ÁRBOL guiada por un
  fichero de parche en Python (`patch(text)->text`), compilación (fichero suelto
  / unidad entera), medida y **restauración verificada por md5**. Aguantan
  finales de línea mixtos (`AIPursuit.cpp` es LF, `CarLoader.cpp` es CRLF).
- `c26wag_rows.py` — las filas del diff de un símbolo entre dos `.o` cualesquiera
  (lo que `fndiff.py` hace sólo contra el objeto de la unidad).
- `c26wag_m.py` / `c26wag_inplace.py` / `c26wag_flags.py` — porcentaje+tamaño+
  filas de un símbolo, sustitución por bloque de texto, y barrido de banderas
  sobre un fichero suelto.

He borrado los tres directorios de volcados RTL (97 MB) al cerrar.
