# r29 — zWorld + zAI + zGameplay + zPhysicsBehaviors

Encargo: 10 near-miss, 16.164 B. **Cierres: 0. Aporte neto +0 B, +0 funciones.**

Lo que traigo, en orden de valor:

1. **La premisa del encargo para `UpdateWheelYRenderOffset` está MEDIDA Y ES
   FALSA.** «La palanca es presión de registros» no lo es: quitarle un registro
   callee-saved al asignador **no cambia el tamaño** (872 B, 0 insns nuevas).
   El `lis` que falta lo decide **`gcse`/PRE + `cse2`**, muy por encima del
   asignador, y traigo el volcado que lo dice literalmente.
2. **`UpdateLoaded` (856 B) es la MISMA familia que `RenderFlaresOnCar`
   (2.908 B) — 3.764 B con un solo mecanismo** — y **tiene un hermano al 100 %**
   (`SuspensionTrailer::Tire::UpdateLoaded`, 840 B) del que leer el idioma.
   Nadie lo había juntado.
3. **Veda nueva y extrapolable a TODO el árbol**: en flotantes, `X > C` y
   `C < X` dan **objeto byte a byte idéntico**. 5 variantes, 0 diferencias.
   Cualquier barrido futuro que permute el orden de una comparación flotante
   está tirando el presupuesto.
4. **El mapa de líneas del bucle de `SAP.h`**, que nadie había leído: el cuerpo
   del original va de la línea **95 a la 99** (nosotros 27→28) y hay **una nota
   de línea, la 97, que aterriza en la SALIDA del bucle**. Con la advertencia de
   por qué la evidencia es más débil de lo que parece (§4.2).
5. **El «NO COMPILA» que bloqueaba `RenderFlaresOnCar` era un `replace` mal
   hecho** (rompía `10.0f`). Hecho bien, compila y **da los 2.908 B clavados**
   —aunque no cierra: octavo caso de «el tamaño exacto miente»— y deja el
   problema reducido a **un solo pseudo de vida larga** (§3.4).
6. **34 ensayos numerados** con su cifra, más 4 diagnósticos de bandera.

```
w1..w8   formas de fuente en UpdateWheelYRenderOffset
w9,wA,wB presion de registros por clobber (DIAGNOSTICO)
f1..f4   banderas de diagnostico
s6..sA   formas del bucle de SAP.h (Add__6RBGrid)
u1..u5   orden de las comparaciones en UpdateLoaded
r1..r6   unificacion del simbolo del 0.0f en RenderFlaresOnCar
```

---

## 0. Verificación — reproduce EXACTO, y cierra EXACTO

`build_direct.py zWorld zAI zGameplay zPhysicsBehaviors` → `5 ok, 0 fallidas`,
y `triage.py --muro` da **las once líneas del encargo con su cifra**. Al cerrar,
tras un segundo `build_direct.py`, **las once líneas idénticas**.

(La columna de multiconjunto **volvió a bailar** entre las dos pasadas —
`b+4, blt-4` contra `blt-4, b+4`, y en `UpdateLoaded` `stmw+1, stw-1, lis-1,
lwz-1` contra `lmw+1, lis-1, stw-1, stmw+1`— con el objeto sin tocar. **Tercera
ronda consecutiva que lo confirma**: la columna no es estable y **no se lee como
regresión**.)

`git status src/` al cerrar: **ninguno de mis cuatro ficheros aparece**
(`CarRender.cpp`, `SAP.h`, `RigidBody.cpp`, `SuspensionTraffic.cpp`). Todos los
arneses restauran en el `finally` y **verifican por md5**.

## 1. `audit.py` — FALLA: **VACÍA**, pero la primera pasada mintió

| unidad | símbolos `ok` | FALLA pasada 1 | FALLA pasadas 2 y 3 |
|---|---|---|---|
| zWorld | 577 | 0 | **0** |
| zAI | 1028 | **3** | **0** |
| zGameplay | 765 | 0 | **0** |
| zPhysicsBehaviors | 1118 | 0 | **0** |

Los tres FALLA de la primera pasada (`update__19performance_limiter`,
`walk_all_paths__11road_walker`, `walk_road__11road_walker`, los tres
«literal distinto») **no se reprodujeron en dos pasadas más**. Es exactamente el
fantasma documentado del `build/` compartido. **Cuarta confirmación de esa
trampa; confirmar con una segunda pasada no es opcional.**

---

## 2. `UpdateWheelYRenderOffset` (876 B) — la premisa del encargo es falsa

### 2.1 El diff, en limpio

```
      fila  OBJETIVO                              NUESTRO
>>>    24   beq -> fila 27                        beq -> fila 32     (thread_jumps)
>>>    48   lis r15, 0x4330                       lis r16, Rear@ha
>>>    49   lis r16, lbl_8040AA84@ha              lis r15, 0x4330
>>>    85   bne -> ...                            bne -> ...          (idem 24)
>>>    97   lis r9, TweakKitWheelOffsetRear@ha    (FALTA)
>>>    98   lwz r0, Rear@l(r9)                    lwz r0, Rear@l(r16)
>>>   169   lfs f0, lbl_8040AA84@l(r16)           lfs f0, $LC@l(r19)
```

876 contra 872 B. **Siete filas, un `lis`.**

### 2.2 El diagnóstico que lo cierra: NO es el asignador

`-dG` sobre `CarRender.cpp` (12 s) da la decisión literal:

```
PRE: redundant insn 449 (expression 51) in bb 27, reaching reg is 330
PRE: redundant insn 680 (expression 51) in bb 42, reaching reg is 330
PRE: redundant insn 729 (expression 51) in bb 46, reaching reg is 330
PRE/HOIST: end of bb 8, insn 868, copying expression 51 to reg 330
```

`expression 51` = `(high:SI (symbol_ref/u:SI ("*$LC489")))` = **el `@ha` del
0,0f**. PRE marca **las TRES apariciones** redundantes (el ternario de la línea
3248, el `if` del width y el `if` del radius) y **inserta UNA sola** al final del
bb 8. Después:

- el volcado `.loop` **sí** tiene dos `high($LC489)`: la de PRE (insn 868,
  reg 330) y la que `loop.c` iza con la cadena del `lfs f26` (insn 910, reg 345);
- **`cse2` funde las dos**: en `.cse2` todos los usos de reg 345 aparecen ya
  como reg 330 (líneas 493, 499, 1748, 1839) y el `set` de 345 queda muerto.

Con un registro libre, `loop.c` iza `TweakKitWheelOffsetRear@ha` (insn 898) y de
ahí las filas 48/97/98.

**El objetivo tiene DOS `high(0.0f)` vivas en el mismo bloque básico** (r19 en la
fila 33 y r16 en la 49, sin etiqueta ni salto entre medias) — o sea, **a él
`cse2` no se las fundió**. Ésa es toda la diferencia.

### 2.3 La medida que mata la hipótesis del encargo

Si el `lis` que falta lo produjese **reload rematerializando** un pseudo que se
queda sin registro, quitarle un registro al asignador tendría que hacerlo
aparecer. **No aparece:**

| # | palanca | % | tamaño | filas |
|---|---|---|---|---|
| — | control | 99,38356 | **872** | 7 |
| w9 | `__asm__("" : : : "r16")` en el cuerpo del bucle | 94,452 | **872** | 88 |
| wA | idem con `"r14"` | 94,475 | **872** | 87 |
| wB | idem con `"r17"` | 94,475 | **872** | 88 |

**El tamaño no se mueve: 872 en los tres.** El asignador simplemente baraja los
18 valores por otros registros y sigue cabiendo. **La presión de registros no
puede crear el `lis` que falta**, porque el número de `lis` queda fijado en
`gcse`/`loop`/`cse2`, mucho antes de `local_alloc`.

(**Barrera declarada**: w9/wA/wB son `asm` sin operandos y por tanto barrera
TOTAL además de clobber. Son **diagnóstico**, no propuesta; no quedan aplicados.
El DWARF no dice nada de ellos porque no hay `asm` en el original.)

### 2.4 Diagnósticos de bandera (no aplicables, sólo para localizar el pase)

| # | bandera | % | tamaño | filas |
|---|---|---|---|---|
| f1 | `-fno-gcse` | 84,443 | 868 | 114 |
| f2 | `-fno-rerun-loop-opt` | 99,38356 | 872 | 7 (idéntico) |
| f3 | `-fno-rerun-cse-after-loop` | 93,653 | **880** | 40 |
| f4 | `-fno-thread-jumps` | **99,406** | 872 | **6** |

Dos lecturas que valen:

- **f3 sube a 880 B**: sin `cse2` sobran DOS `lis`, no uno. Confirma que **`cse2`
  es quien funde** y que al objetivo se le escapó **exactamente una** fusión.
- **f4 arregla las filas 24 y 85**: esas dos filas son **`thread_jumps`**, que a
  nosotros nos enhebra el `beq` del primer `if (this->pRideInfo != nullptr)`
  directamente al preencabezado y al objetivo no. Es una diferencia **real e
  independiente del `lis`**, y no la había anotado nadie con su causa.

### 2.5 Ensayos de fuente (ocho, todos negativos)

| # | sentencia barrida | % | tamaño | filas |
|---|---|---|---|---|
| w1 | `&&` del width con los operandos cambiados | 99,338 | 872 | 9 |
| w2 | `&&` del radius con los operandos cambiados | 99,338 | 872 | 9 |
| w3 | los dos cambiados | 99,292 | 872 | 11 |
| w4 | `float zero = 0.0f` usada **sólo en el radius** | 98,744 | 868 | 16 |
| w5 | `desired_radius = tire_offset.w` detrás del `if` del width | 98,356 | 872 | 13 |
| w6 | temporal `float width_scale` para la división | 99,38356 | 872 | **7 (objeto idéntico)** |
| w7 | width como `if (a <= 0 \|\| b <= 0) else` (ramas cambiadas) | 97,900 | 868 | 14 |
| w8 | radius como `if (a <= 0 \|\| b <= 0) else` (ramas cambiadas) | 97,900 | 868 | 14 |

**w4 cierra el hueco de la veda del fuente**, que decía «`float zero` en el width
o en los tres»: **también falla sólo en el radius**.

### 2.6 Lo que queda por probar aquí

La única palanca coherente con el mecanismo es **impedir que `cse2` funda las dos
`high($LC489)` del preencabezado**, y para eso hacen falta **en bloques básicos
distintos**. No he encontrado forma de fuente que meta un límite de bloque entre
la inserción de PRE (fila 33) y el izado de `loop.c` (fila 49) sin emitir código.
**No he probado**: `permuter.py`, ni restricciones de registro no-clobber, ni
tocar las dos filas de `thread_jumps` por fuente.

---

## 3. `UpdateLoaded` + `RenderFlaresOnCar` — 3.764 B, UN mecanismo, y hay hermano al 100 %

### 3.1 Es la misma familia que el §2, con el signo contrario

```
UpdateLoaded (856 B)      objetivo  fila  86  lis r30, lbl_803FB6B8@ha   <- IZADA, callee-saved
                                    fila 101  lfs f0, ...@l(r30)
                                    fila 120  lfs f0, ...@l(r30)
                          nuestro   fila 100  lis r9, $LC@ha            <- materializada
                                    fila 119  lis r9, $LC@ha            <- y OTRA VEZ

RenderFlaresOnCar (2908)  objetivo  fila 123  lis r16, lbl_8040AD04@ha  <- UNA, vida larga
                          nuestro   filas 452 y 461: DOS lis del mismo simbolo
```

Y en `UpdateWheelYRenderOffset` es al revés (el objetivo tiene dos y nosotros
una). **Los tres son la misma decisión: cuántas `reaching_reg` de PRE sobreviven
para un `high` dado.** `UpdateLoaded` arrastra además el marco (0x30 contra 0x28)
y `stmw r30` contra `stw r31`: **son consecuencia**, no diferencias aparte.

El volcado `-dG` de `SuspensionTraffic.cpp` lo dice al dígito:

```
Index 40 (hash value 70)
  (high:SI (symbol_ref/u:SI ("*$LC251")))          <- el @ha del 1.0f
PRE: redundant insn 454 (expression 40) in bb 23, reaching reg is 362
PRE: redundant insn 596 (expression 40) in bb 26, reaching reg is 362
PRE: redundant insn 643 (expression 40) in bb 31, reaching reg is 362
PRE: redundant insn 733 (expression 40) in bb 36, reaching reg is 362
PRE: redundant insn 784 (expression 40) in bb 39, reaching reg is 362
PRE/HOIST: end of bb 15, insn 848, copying expression 40 to reg 362   <- DOS
PRE/HOIST: end of bb 19, insn 851, copying expression 40 to reg 362   <- inserciones
```

**Dos inserciones para el mismo pseudo.** El objetivo hace **una** en un bloque
que domina a los dos usos (la fila 86, dentro del bb del `bl VU0_Atan2`), y por
eso el pseudo tiene un solo `set`, vive largo y se lleva **r30**. Contadas en el
objeto, las `lis` de ese literal son **3 en el objetivo (86, 148, 199) y 4 en el
nuestro (100, 119, 148, 199)**: sobra exactamente una.

### 3.2 El hermano al 100 % que nadie había usado

**`UpdateLoaded__Q217SuspensionTrailer4Tireffff` (840 B) está al 100 %** y es la
misma función para otra clase de suspensión (`SuspensionTrailer.cpp:269` contra
`SuspensionTraffic.cpp:271`). **Es la referencia de idioma que a los 4.396 B del
`blt`→`b` le falta y que aquí SÍ existe.** No la he explotado; es lo primero que
haría el siguiente.

### 3.3 Ensayos (cinco, todos objeto IDÉNTICO) — y la veda extrapolable

El fuente mezcla dos estilos de comparación flotante (`0.0f < this->mEBrake`
frente a `this->mEBrake > 0.5f`), lo que parecía la pista obvia. **No lo es:**

| # | sentencia barrida | % | tamaño |
|---|---|---|---|
| — | control | 97,21028 | 860 |
| u1 | `mEBrake > 0.5f && skid_speed > 1.0f` → `0.5f < mEBrake && 1.0f < skid_speed` | 97,21028 | 860 |
| u2 | sólo `skid_speed > 1.0f` → `1.0f < skid_speed` | 97,21028 | 860 |
| u3 | sólo `mEBrake > 0.5f` → `0.5f < mEBrake` | 97,21028 | 860 |
| u4 | `1.0f < UMath::Abs(fwd_vel)` → `UMath::Abs(fwd_vel) > 1.0f` | 97,21028 | 860 |
| u5 | `1.0f < skid_speed` → `skid_speed > 1.0f` | 97,21028 | 860 |

**Las cinco dan el objeto byte a byte idéntico al control.** `fold` canonicaliza
la comparación flotante antes de expandir.

> **VEDA DE ÁRBOL: el orden de los operandos de una comparación flotante
> (`X > C` contra `C < X`) NO es una palanca.** Es gratis de escribir y por eso
> tienta; cuesta un ensayo por sitio y no mueve un byte. Anótese junto a la regla
> del `|`, donde el orden **sí** manda.

### 3.4 `RenderFlaresOnCar` (2.908 B) — el `-dG` y el símbolo PARTIDO

El mismo volcado (`c28wag_rtl_c29base/x.ii.gcse`, función 45) da:

```
Index 175 (hash value 232)
  (high:SI (symbol_ref:SI ("lbl_8040AD04")))
PRE: redundant insn 1958 (expression 175) in bb 192, reaching reg is 1010
PRE: redundant insn 1965 (expression 175) in bb 193, reaching reg is 1010
PRE/HOIST: end of bb 141, insn 2614, copying expression 175 to reg 1010
```

**UNA sola inserción, y muy tarde** (bb 141): cubre las filas 600/601 pero no la
carga de la fila 454, que se queda con su `lis` propio (fila 452). El objetivo
inserta en un bloque **mucho más temprano** (fila 123, dentro del bloque de
`CarTypeInfoArray`) y desde ahí sirve a las tres.

**Y aquí está lo que nadie había contado**: en nuestro objeto el 0,0f de esta
función está **PARTIDO EN DOS SÍMBOLOS**. Barriendo el diff con `--all`:

| | objetivo | nuestro |
|---|---|---|
| filas 148-396 y 670 (≈19 cargas) | `lbl_8040AD04` | **`$LC630`** |
| filas 452/454, 461, 600, 601 | `lbl_8040AD04` | `lbl_8040AD04` |

El objetivo tiene **un solo símbolo con ~23 usos**; nosotros tenemos el `extern
const float lbl_8040AD04` (4 usos) **y además** un literal de pool `$LC630`
(≈19 usos). No salen como diferencia porque el diff corre con
`function_reloc_diffs=none` y `audit.py` compara **valores** (los dos son 0,0f).

La veda del propio fuente decía que la dirección buena («pasar los 34 literales
0,0f del cuerpo a `lbl_8040AD04`») **«NO COMPILA»**. **Compila** si se acota al
cuerpo de la función y se respeta el límite de palabra (el intento anterior
rompía `10.0f` → `1lbl_8040AD04`):

| # | sentencia barrida | % | tamaño | filas |
|---|---|---|---|---|
| — | control | 98,61761 | 2912 | 18 |
| r1 | todos los `0.0f` del cuerpo, `replace` a pelo | — | — | **NO COMPILA** (rompe `10.0f`) |
| r2 | sólo los `= 0.0f;` de asignación | 97,552 | **2908** | 121 |
| **r4** | **todos los `0.0f` del cuerpo, con límite de palabra** | 97,552 | **2908** | 121 |
| r5 | sólo los `0.0f` en posición de valor (`;`, `,`, `)`) | 96,513 | 2932 | 132 |
| r6 | como r4 pero dejando los de `AddQuickDynamicLight` | 97,221 | 2916 | 128 |

**r4 da el tamaño del objetivo CLAVADO (2908 B, 727 insns) y sigue sin casar**:
es el **octavo** caso medido de «el tamaño exacto miente». Pero deja dos cosas:

1. **La unificación del símbolo SÍ quita el `lis` que sobra** (2912 → 2908).
2. Las 121 filas son **el fichero de registros entero corrido una posición**
   (`mr r15, r5` contra `mr r16, r5` ya en la fila 23, y las 15 `fmr`/`lfs` de
   flotantes desplazadas). Ésa es la firma de que **al objetivo le vive UN pseudo
   más que a nosotros** — justamente el `high` izado de la fila 123, que r4 sigue
   sin producir.

O sea: **la unificación del símbolo es necesaria pero no suficiente**; falta que
PRE inserte en un bloque que domine también la carga de la fila 454.

---

## 4. El `blt`→`b` (4.396 B) — el mapa de líneas del original

### 4.1 Lo que dice, y es nuevo

`lmap` sobre `Add__6RBGrid` da las mismas líneas en **los cuatro sitios**:

```
80232244  mr r6, r9        SAP.h:95     node = head
80232248  lwz r9, 0x4(r9)  SAP.h:99     head = head->mTail
80232254  lfs f13, 0x8(r9) SAP.h:94     la condicion
80232260  b .L_8023223C    SAP.h:95     el back-edge
80232264  cmpwi r6, 0x0    SAP.h:97  SAP.h:101    <- DOS notas en la salida
```

Calibrado contra nuestro `.s` (líneas 26/27/28/31 de nuestro `SAP.h`):

| nuestra | objetivo | desfase |
|---|---|---|
| 21 `this->mRoot = this` | 87 | +66 |
| 23 `node = nullptr` | 90 | +67 |
| 24 `head = this->mRoot` | 91 | +67 |
| 26 `while (...)` | 94 | +68 |
| 27 `node = head` | 95 | +68 |
| **28 `head = head->mTail`** | **99** | **+71** |
| 31 `if (node != nullptr)` | 101 | +70 |

O sea: **entre `node = head` y `head = head->mTail` el original tiene TRES líneas
que nosotros no tenemos**, y **la del medio (97) emite una nota que aterriza en
la salida del bucle**, junto a la del `if (node != nullptr)`. En nuestro `.s` la
salida (`.L14269`) lleva **sólo** la nota de la línea 31.

La lectura natural es un `break` condicional en el cuerpo (96 la condición,
97 el `break`, 98 la llave), con la condición fundida en insns que ya existen.

### 4.2 Por qué NO la doy por buena, y la medida

Dos avisos, y el segundo es una medida:

1. La regla ya documentada dice que **la línea de una instrucción replanificada
   es la de la última nota que la precede en el orden final**. Aquí `jump.c`
   **cruza (cross-jump) el `cror;bso`** del fondo del bucle con el del guardián:
   al borrar insns, las notas migran. **El `SAP.h:99` sobre el `lwz` puede ser
   arrastre del cross-jump y no la línea de su sentencia.**
2. Escrito tal cual, **no casa**:

| # | sentencia barrida | % | tamaño |
|---|---|---|---|
| — | control | 99,30108 | 1488 |
| s6 | `while (B)` + `if (head->mTail == nullptr) { break; }` + `head = head->mTail` | 91,653 | **1560** |
| s7 | igual pero conservando `head != nullptr &&` en la condición | 90,699 | 1536 |
| s8 | igual que s6 con el `break` sin llaves | 91,653 | 1560 |
| s9 | `while (B)` + `head = head->mTail` + `if (head == nullptr) break` | 88,272 | 1440 |
| sA | s6 con `head->GetTail()` en el test y en la asignación | 97,581 | 1500 |

El motivo del fallo está medido: con el `break` por `head->mTail`, **GCC 2.95 no
funde el `lwz` del test con el de la asignación** — en s8 salen las dos cargas
(`lwz r0, 0x4(r6)` para el test y `lwz r9, 0x4(r9)` para la asignación), +18
insns sobre los cuatro sitios.

**Queda como pista viva, no como diagnóstico.** Lo que probaría el siguiente:
una forma en la que el test del `break` y la asignación **compartan la carga**
(por ejemplo un temporal usado en las dos), que es lo único que reconciliaría el
mapa de líneas con el multiconjunto idéntico.

---

## 5. `CullParts` (836 B) — lo que falta es UNA copia, y es del asignador

Diff completo (cinco filas, `faltan 1: mr+1`):

```
      fila  OBJETIVO              NUESTRO
>>>    95   lwz r0, 0x8(r30)      lwz r11, 0x8(r30)
>>>    97   cmpwi r0, 0x0         cmpwi r11, 0x0
>>>   101   cmpw r9, r0           cmpw r11, r0
>>>   102   mr r9, r0             (FALTA)
>>>   144   cmpwi r0, 0x1         cmpwi r11, 0x1
```

El `lmap` del objetivo aclara la semántica: la fila 96 es `li r9, 0x0` de la
**línea 634** (muy anterior) y las filas 99-102 son el guardián del `for` de la
línea 661. En el objetivo, el cero de la línea 634 **sigue vivo en r9** y hay que
copiarle la cota (`mr r9, r0`); nosotros **reutilizamos r11**, que en esa rama ya
se sabe cero (`cmpwi r11, 0` + `bne` a la salida), y nos ahorramos la copia.

Es decir: **una coalescencia de copia que a nosotros nos sale y al objetivo no**,
sobre un valor cuya igualdad a cero sólo se conoce por dominancia. Sin ensayos:
es el patrón «sólo delta de copias» y no he encontrado forma de fuente que lo
ataque. **Confirmo el aviso del encargo**: la barrera `asm("" : : : "r11")` del
fuente **hace falta** y no la he tocado.

---

## 6. `SetMemoryPoolSize` (304 B) — NO lo he tocado, por coordinación

El encargo avisaba de que otro agente barría el eje del `asm` selectivo en cuatro
funciones **incluida ésta**. Para no duplicar ni pisar medidas, **no he hecho
ningún ensayo aquí**. Dejo apuntado lo único que añado al diagnóstico de la r28:

el desempate real es `depend_count` y luego el LUID, y el dependiente que sobra
al `stw` es la **anti-dependencia WAR sobre r0** del `li r0,0`. Como `sched2`
corre **después de reload**, r0 ya es un registro duro: **cualquier palanca de
fuente tiene que conseguir que el valor almacenado por
`this->NumSpongeAllocations = 0;` no nazca en r0**, y eso no lo decide la fuente
sino `local_alloc`. Es el mismo techo que el §2.3: **el `asm` selectivo ata
pseudos, no elige registros duros.**

---

## 7. Vedas nuevas, con la sentencia

- **w1/w2/w3** — permutar los operandos del `&&` en el `if` del width y/o del
  radius de `UpdateWheelYRenderOffset`: **99,29-99,34 %**, 872 B, 9-11 filas.
- **w4** — `float zero = 0.0f;` usada **sólo en el radius**: **98,744 %**, 868 B.
  (Cierra el hueco de la veda del fuente, que sólo cubría width y «los tres».)
- **w5** — `float desired_radius = tire_offset.w;` movida detrás del `if` del
  width: **98,356 %**, 872 B.
- **w6** — temporal `float width_scale` para `desired_width / model_width`:
  **objeto IDÉNTICO** (se pliega).
- **w7/w8** — width y radius reescritos como `if (a <= 0.0f || b <= 0.0f)` con
  las ramas cambiadas: **97,900 %**, 868 B.
- **w9/wA/wB** — clobber de un callee-saved (`r16`, `r14`, `r17`) dentro del
  bucle: **el tamaño NO se mueve (872 B)**. **Mata la hipótesis de presión de
  registros.**
- **u1..u5** — cualquier permutación del orden de los operandos de las
  comparaciones flotantes de `UpdateLoaded`: **objeto IDÉNTICO** en las cinco.
  **Extrapolable a todo el árbol.**
- **s6/s8** — `while (B)` con `if (head->mTail == nullptr) break;` antes de
  `head = head->mTail`: **91,653 %**, **1560 B** (+72). GCC no funde las dos
  cargas de `mTail`.
- **s7** — igual conservando `head != nullptr &&`: **90,699 %**, 1536 B.
- **s9** — `head = head->mTail;` seguido de `if (head == nullptr) break;`:
  **88,272 %**, 1440 B (reconfirma la s2 de la r28 con otra condición).
- **sA** — s6 con `head->GetTail()` en el test y en la asignación:
  **97,581 %**, 1500 B. Es la **mejor de las formas con `break`** y la única que
  se queda a +12 B; queda como cabo suelto.
- **r2/r4/r5/r6** — unificar el símbolo del 0,0f dentro del cuerpo de
  `RenderFlaresOnCar`: **96,51-97,55 %**. **r4 da 2908 B exactos** (el tamaño del
  objetivo) y aun así 121 filas. **No es una veda cerrada**: quita el `lis` que
  sobra y es condición necesaria; lo que falta es el pseudo de vida larga.
- **r1** — el mismo cambio con `str.replace('0.0f', ...)` a pelo: **NO COMPILA**,
  porque rompe `10.0f` → `1lbl_8040AD04`. **Es casi seguro el motivo del
  «NO COMPILA» que la veda del fuente daba por definitivo.**

## 8. Lo que NO he probado

- **`UpdateWheelYRenderOffset`**: nada que meta un **límite de bloque básico**
  entre la `high` insertada por PRE (fila 33) y la izada por `loop.c` (fila 49),
  que es la única palanca coherente con el mecanismo. Tampoco `permuter.py` ni
  restricciones de registro, ni atacar por fuente las dos filas de
  `thread_jumps` (24 y 85).
- **`RenderFlaresOnCar` (2.908 B)**: con r4 aplicado (símbolo unificado, 2908 B
  exactos) **no he vuelto a barrer nada**. Ahí es donde yo seguiría: con el
  símbolo ya unificado, el problema se reduce a **UN pseudo de vida larga que no
  nace**, y eso es lo mismo que el §2. Tampoco he combinado r4 con la `f1` de la
  r27 ni con las `e*` de la r26.
- **`UpdateLoaded`**: **no he comparado su fuente contra el hermano al 100 %**
  (`SuspensionTrailer::Tire::UpdateLoaded`). Es la vía más barata que dejo.
- **`AssignClosestOffsets` (1.684 B)**, **`GTriggerRCUi`**, **`GenerateIndex`**,
  **`DefragmentPool`**: cero ensayos (negativos firmes del encargo).
- **`SetMemoryPoolSize`**: cero ensayos, por coordinación (§6).
- **El bucle de `SAP.h`**: no he probado ninguna forma en la que el test del
  `break` y la asignación **compartan la carga de `mTail`**, que es lo que pide
  el §4.2. Tampoco he mirado los **cuatro sitios de `UpdateAllAvoidables`**
  (asumí que son idénticos a los de `Add__6RBGrid` porque el multiconjunto lo
  dice, pero **no lo he verificado fila a fila**).

## 9. Herramientas dejadas (prefijo `c29wag_`)

- **`c29wag_batch.py`** — aplica una LISTA de variantes de una tirada, compila
  cada una, restaura por md5 y da `filas=N` + porcentaje + tamaño. Un lote de 9
  variantes en 1 min 52 s. Es `c28wag_pr.py` pero por lotes, que es lo que hace
  falta cuando barres formas.
- `c29wag_vw.py`, `c29wag_vw2.py`, `c29wag_vs.py`, `c29wag_vu.py` — los lotes de
  variantes de cada sección (fuente y clobbers).
- `c29wag_p_s8.py` — el parche suelto para ver las filas de una variante
  concreta con `c28wag_pr.py`.
- `c29wag_ul_gcse.txt` — el volcado `-dG` recortado de `UpdateLoaded`.
- `c28wag_rtl_c29base/` — volcados `.gcse .loop .cse2 .lreg .greg` de
  **`CarRender.cpp` entero** (contiene también `RenderFlaresOnCar`, sin explotar).
- `c28wag_rtl_c29ul/` — lo mismo para `SuspensionTraffic.cpp`.
