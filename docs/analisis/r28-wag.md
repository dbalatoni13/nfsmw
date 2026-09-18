# r28 — zWorld + zAI + zGameplay + zPhysicsBehaviors

Encargo: 11 near-miss, 16.164 B. **Cierres: 0. Aporte neto +0 B, +0 funciones.**

Lo que traigo:

1. **`SetMemoryPoolSize`: mecanismo PROBADO con un diagnóstico limpio** (`p2`), y la
   derivación completa —que **corrige la de la r27**: no basta con «quitarle un
   dependiente al `stw`», hay que saber que el desempate real es
   `depend_count` **y luego el LUID**, y que `ISSUE_RATE = 2`.
2. **El `blt`→`b` (4.396 B): el titular de la r27 queda DESMENTIDO.** No falta
   ninguna sentencia; **el multiconjunto de instrucciones es IDÉNTICO** y el
   diff son **2 filas por sitio × 8 sitios**, todas del **mismo bucle inline de
   `SAP.h`**. Es una inversión de `jump.c`, no un `break` que falte.
3. **Un caso mínimo de 27 líneas que reproduce los DOS defectos** y compila en 2 s.
4. **`UpdateWheelYRenderOffset`: lectura nueva y exacta** de qué `@ha` iza cada
   lado (la r27 lo miraba por el lado de los `0.0f`).
5. **22 ensayos numerados** (12 sobre el árbol real, 10 en el caso mínimo) más
   **18 banderas**, todos con su cifra.

```
p1 p2                              SetMemoryPoolSize
s0 s1 s2 t1 t2 t3 u2 u4..u9 ua ub  el bucle de SAP.h (Add__6RBGrid + UpdateAllAvoidables)
v1..v8 w1..w5 x1..x8 z1..z4        formas en el caso mínimo
```

---

## 0. Verificación del encargo — reproduce EXACTO

`build_direct.py zWorld zAI zGameplay zPhysicsBehaviors` + `triage.py --muro`:
las 11 líneas del encargo con su porcentaje y su tamaño. Al cerrar, tras un
`build_direct.py` de las cuatro unidades, `triage.py` da **las once líneas
idénticas al arranque**.

(La columna de multiconjunto **volvió a bailar** entre las dos pasadas
—`lis-1, lwz-1, lmw+1, stmw+1` contra `lis-1, lwz-1, stw-1, stmw+1` en
`UpdateLoaded`— con el objeto sin tocar. Confirmada por segunda ronda la trampa
que anotó la r27.)

Un `build_direct.py` intermedio dio **`4 ok, 1 fallidas`** y el siguiente, sin
tocar nada, **`5 ok`**: fallo transitorio (otro agente tocando una cabecera
compartida, o el `ngcas` que ya está documentado). **No es de mis ficheros.**

## 1. `audit.py` — lista de FALLA: **VACÍA**

| unidad | símbolos auditados | FALLA |
|---|---|---|
| zWorld | 491 | **0** |
| zAI | 842 | **0** |
| zGameplay | 632 | **0** |
| zPhysicsBehaviors | 985 | **0** |

Sin ningún FALLA no hay nada que confirmar con una segunda pasada. No he
congelado nada (no he cerrado nada). `git diff --stat` sobre los cuatro ficheros
que he tocado (`SAP.h`, `CarLoader.cpp`, `CarRender.cpp`, `RigidBody.cpp`) sale
**vacío**; todos los arneses restauran en el `finally` y **verifican por md5**.

---

## 2. `SetMemoryPoolSize` (304 B) — mecanismo PROBADO, y la r27 se quedaba corta

### 2.1 El diagnóstico que lo cierra como mecanismo

| # | sentencia barrida | resultado |
|---|---|---|
| — | control | **97,36842 %**, 304/304, 2 filas |
| p1 | `this->NumSpongeAllocations = size - size;` | **IDÉNTICO** (se pliega a `li r0,0`) |
| **p2** | **borrar `this->NumSpongeAllocations = 0;` del final** | **las dos filas del diff DESAPARECEN**; sólo quedan las dos DELETE de las insns borradas |

Con p2 las filas 59-62 casan al pie de la letra (`lis`, `addi`, `stw`, `bl`).
**Queda probado que quien empuja el `stw` a la ranura equivocada es el
`li r0,0` de esa sentencia.**

### 2.2 La derivación completa (volcado `-dR -fsched-verbose-5`, bb 8)

```
;;      247   506  0  4  12  1  iu2 : 256                          <- lis r6
;;      256   507  0  5  11  1  iu2 : 282 270 258                  <- addi r6  (3 dependientes)
;;      250   512  0  2  11  1  iu2 : 288 282 270 266 258          <- mr r3,r0 (5)
;;      232   512  0  5  11  2  lsu : 288 282 270 258              <- stw      (4)

;;  Ready list (t = 15):  256  232  250      <- orden ASCENDENTE por depend_count
;;      --> insn 250 on iu2      --> insn 232 on lsu
;;  Ready list (t = 16):  256
```

Tres cosas que la r27 no tenía:

1. **`ISSUE_RATE = 2`.** En todos los ciclos del volcado se emiten como mucho dos
   insns, aunque haya **dos unidades iu2**. Por eso 256 no cabe en el ciclo 15.
2. **El desempate tras `depend_count` es el LUID, y va a favor de 256.** El
   comparador devuelve `LUID(y) - LUID(x)`, o sea orden **descendente** por LUID,
   y se coge por el final ⇒ **gana el LUID MENOR**. Y `sched1` deja el orden
   `254, 252, 247, 256, 250, 232, 258` (r27 §6.1), luego **LUID(256) < LUID(232)**.
   **Un EMPATE a 3 dependientes ya bastaría**: no hace falta que 256 gane, basta
   que 232 no le saque ventaja.
3. **La insn 250 (`mr r3,r0`) NO llega al `.s`** (304 B = 76 insns, y con ella
   serían 77) pero **sí ocupa la ranura iu2 del ciclo 15**. Es una insn fantasma
   que gasta issue.

**El dependiente que le sobra a 232 es la 288** (`li r0,0`), y su
`LOG_LINKS` lo confirma literal:

    (insn:TI 288 282 289 (set (reg:SI 0 r0) (const_int 0))
      ... (insn_list:REG_DEP_ANTI 250 (insn_list:REG_DEP_ANTI 232 ...

O sea: **las tres llamadas intermedias NO limpian `reg_last_uses[r0]`** en
`sched_analyze` de GCC 2.95, y la WAR sobrevive a través de `bl bInitMemoryPool`,
`bl bSetMemoryPoolDebugFill` y `bl bSetMemoryPoolTopDirection`.

### 2.3 Por qué lo dejo como **negativo firme**

El objetivo tiene **exactamente las mismas 76 instrucciones**, con
`li r0, 0x0` + `stw r0, 0x68(r31)` **en el mismo bloque básico** (de `802DD404` a
`802DD478`, sin etiqueta intermedia). Luego en el objetivo el grafo de
dependencias de `sched2` es **idéntico** y el planificador es determinista:
**con esta fuente y estos cflags la ranura no se puede mover**. Las palancas
posibles son sólo dos y las dos exigen cambiar el juego de insns, que ya cuadra
al byte:

- quitarle un dependiente a 232 (los otros tres son las tres llamadas), o
- darle uno más a 256 (r6 no lo lee ni lo escribe nadie más).

**Lo que NO he probado**: nada que meta el `li r0,0` en otro bloque básico
(haría falta una etiqueta viva entre `bl bSetMemoryPoolTopDirection` y esa
sentencia, y no sé escribirla en C++ sin que `jump2` la borre).

---

## 3. El `blt`→`b` (4.396 B) — **el titular de la r27 estaba mal**

### 3.1 No falta ninguna sentencia: son 2 filas × 8 sitios

`Add__6RBGrid` (1.488 B) tiene **4** sitios y `UpdateAllAvoidables` (2.908 B)
otros **4**, y los ocho son **el mismo bucle inline** del constructor
`SAP::Grid<T>::Axis::Node::Node` (`SAP.h:26-29`). En cada sitio el diff son
**dos filas** y **ninguna más**:

```
  41 cror un, eq, gt      ==
  42 bso  <salida>        ==
  43 mr   r6, r9          ==
>>>44 lwz r9, 0x4(r9)   |  lwz r9, 0x4(r6)      <- leemos la COPIA
  45 cmpwi r9, 0x0        ==
  46 beq  <salida>        ==
  47 lfs f13, 0x8(r9)     ==
  48 lfs f0,  0x8(r8)     ==
  49 fcmpu cr0, f13, f0   ==
>>>50 b   .fila41       |  blt .fila43          <- el back-edge
```

**El multiconjunto de instrucciones es idéntico** (1 `cror`, 1 `bso`, 3 `lfs`,
2 `fcmpu` en los dos). Lo único que cambia es **dónde empieza el bucle**: en el
objetivo la etiqueta está en la fila 41 (el `cror`) y el back-edge es
incondicional; en el nuestro está en la 43 y el back-edge es `blt`. **El
`cror`+`bso` está COMPARTIDO entre el guardián del preencabezado y la cima del
bucle en el objetivo, y sólo en el guardián en el nuestro.**

Esto **desmiente el §7 de la r27** («la sentencia que falta es una SALIDA del
bucle»): no falta nada.

### 3.2 El pase que decide, con volcado

Volcados RTL por pase del caso mínimo (`c28wag_rtlmin.py`, letras `rjsGLtfcS`).
En el **`.rtl` (nada más expandir)** ya está la rotación:

```
(insn 67  (set (reg/v 94) (mem (reg/v 83))))     head = *rootp
(jump_insn 104 (set (pc) (label_ref 70)))        <- goto TEST
(code_label 89)                                  <- cima = CUERPO
(insn 94  (set (reg/v 93) (reg/v 94)))           node = head
(insn 100 (set (reg/v 94) (mem (reg 101))))      head = head->mTail
(code_label 70)                                  <- TEST
(jump_insn 73  if (eq) -> 86 salida)             A
(insn 82  (set (reg:CCFP 100) (compare ...)))    B
(jump_insn 83  if (lt) -> 89)                    <- YA es `blt cima`
```

O sea: **`expand_end_loop` rota el bucle DURANTE la expansión** y emite el test
de abajo en la forma **directa** (`blt cima`). Luego `jump.c`
(`duplicate_loop_exit_test`) copia el test al preencabezado — de ahí nuestro
`lfs;fcmpu;cror;bso`.

Para el objetivo hace falta la forma **invertida** abajo
(`cror; bso salida; b cima`), que es lo único que `jump.c` puede **cruzar
(cross-jump)** con la copia del preencabezado. Y `jump.c` sólo deja de invertir
`bso salida; b cima` cuando **el salto condicional NO está pegado al
incondicional** (`prev_active_insn` / `no_labels_between_p`), que es justo lo que
pasa en el objetivo: entre el `beq` (fila 46) y el `b` (fila 50) hay
`lfs;lfs;fcmpu`.

### 3.3 Ensayos sobre el árbol real (arnés `c28wag_pr.py`, ciclo 4 s)

Todos sobre `SAP.h`, medidos con `Add__6RBGrid`:

| # | sentencia barrida | % | tamaño |
|---|---|---|---|
| s0 | control (`mn_repro` suelto = la unidad) | **99,30108** | 1488 |
| s1 | los tres `goto` (`search_node:` + `if` + `goto`) | 84,032 | 1460 |
| s2 | `while (B) { … if (head == nullptr) break; }` | 88,272 | 1440 |
| **t1** | **`head->GetPosition()` en la condición** | 86,277 | **1456** |
| t2 | `head->GetPosition() < this->GetPosition()` | 86,277 | 1456 |
| t3 | `this->GetPosition()` en la condición | 86,304 | 1456 |
| **u2** | **guardián `if (B)` + `while (A && B)` con accesor** | **93,495** | **1520** |
| u4 | `head->GetTail()` sólo en el CUERPO | **99,30108** | 1488 (idéntico) |
| u5 | accesor en condición y cuerpo | 86,277 | 1456 |
| u6 | guardián + `do…while` + accesor | **99,30108** | 1488 (idéntico) |
| u7 | guardián + `for(;;)` con dos `break` + accesor | **99,30108** | 1488 (idéntico) |
| u8 | igual que u7 sin accesor | **99,30108** | 1488 (idéntico) |
| u9 | `Node *next = head->mTail;` antes de la copia | 94,704 | 1520 |
| ua | igual con `head->GetTail()` | 94,704 | 1520 |
| ub | la condición en un `bool keep_going` | 92,836 | 1544 |

**El hallazgo utilizable (t1)**: **un accesor inline EN LA CONDICIÓN bloquea la
rotación de `expand_end_loop`** y saca el `b` incondicional. **En el CUERPO no
hace nada** (u4 da el objeto idéntico al control). Es una regla nueva y
extrapolable.

**El hallazgo que casi cierra (u2)**: guardián + accesor da **el preencabezado
del objetivo fila a fila** (35-42, incluido el `cror`/`bso`) **y** el bucle sin
rotar; lo que sobra son **2 insns por sitio**, el `cror;bso` duplicado dentro del
bucle. Al objetivo le falta exactamente ese cruce.

### 3.4 Formas barridas en el caso mínimo (27 líneas, 2 s)

`c28wag_t3.cpp` reproduce **los dos defectos** (`lwz 9,4(11)` y `blt`). Sobre él:

    v1 while(A&&B)              v2 while(A){if(!B)break;…}     v3 for(;A&&B;adv)
    v4 goto/if                  v5 do{…}while(1)               v6 head=node->mTail
    v7 !(A||B)                  v8 while(1){if(…)break;}       w1..w5 (B en el while)
    x1 temporal `next`          x2 coma                        x3 for con break
    x4 node->mTail              x5 if+do/while                 x6 float p local
    x7 for(;;)+!(A&&B)          x8 continue                    z1..z4 accesores

**Todas menos las de accesor en la condición dan el MISMO objeto.** GCC 2.95
canonicaliza el bucle: no es cuestión de forma de fuente.

### 3.5 Barrido de banderas: **18, ninguna sirve**

    -fno-thread-jumps · -fno-cse-skip-blocks · -fno-gcse · -fno-rerun-cse-after-loop
    -fno-expensive-optimizations · -fno-strength-reduce · -fno-move-all-movables
    -fno-rerun-loop-opt · -fno-schedule-insns · -fno-regmove · -fno-force-mem
    -fno-peephole · -fno-delayed-branch · -fno-optimize-sibling-calls
    -fno-unroll-loops · -fno-caller-saves

La única que mueve el back-edge es **`-fno-cse-follow-jumps`**, y lo mueve **en
la dirección equivocada** (mete un `b .L11` de entrada al bucle y deja el `blt`).

### 3.6 Censo: **la forma del objetivo aparece 8 veces en TODO el DOL, y son las nuestras**

Barrido de `build/GOWE69/asm/**/*.s` (el desensamblado del objetivo) buscando
todo `b` hacia atrás cuya etiqueta destino empiece por `cror`:

```
zAI.s               b@0x8000ec9c 0x8000eddc 0x8000ef44 0x8000f084   (UpdateAllAvoidables)
zPhysicsBehaviors.s b@0x80232260 0x802323a8 0x8023251c 0x80232664   (Add__6RBGrid)
TOTAL 8
```

**No hay ninguna función ya casada con esta forma**, así que **no existe una
referencia de la que leer el idioma**. Eso cierra la vía «buscar el mismo patrón
en algo que ya case».

---

## 4. `UpdateWheelYRenderOffset` (876 B, falta UNA insn) — lectura nueva

El comentario del fuente lo achacaba a los `0.0f`. Con el `lmap` del objetivo la
cuenta es más concreta: **es qué `@ha` iza `loop.c` al preencabezado.**

```
objetivo  802CA03C lis r19, lbl_8040AA84@ha      (0.0f, antes del bucle)
          802CA060 lfs f26, lbl_8040AA84@l(r19)
          802CA074 lis r14, TweakKitWheelOffsetFront@ha
          802CA078 lis r15, 0x4330
          802CA07C lis r16, lbl_8040AA84@ha      <- SEGUNDO @ha DEL MISMO simbolo
          802CA13C lis r9, TweakKitWheelOffsetRear@ha   <- REMATERIALIZADO en el bucle
          802CA25C lfs f0, lbl_8040AA84@l(r16)   linea 3278 (el `if` del width)
          802CA2B4 lfs f0, lbl_8040AA84@l(r19)   linea 3288 (el `if` del radius)

nuestro   fila 48  lis r16, TweakKitWheelOffsetRear@ha  <- IZADO
          fila 98  lwz r0, TweakKitWheelOffsetRear@l(r16)  (sin `lis`: -1 insn)
          fila 169 lfs f0, $LC563@l(r19)          <- un solo @ha para los dos `if`
```

O sea: **el objetivo mantiene DOS pseudos `high(0.0f)` vivos** (r19 y r16) y por
eso a `loop.c` no le queda registro para izar `TweakKitWheelOffsetRear`, que
rematerializa dentro del bucle. Nosotros unificamos el `@ha` y con el registro
libre izamos el Tweak. **Los 4 B que faltan son ese `lis` de dentro del bucle.**

**Y hay una segunda diferencia que nadie había anotado**: la fila 24
(`beq`) del objetivo salta a la fila **27** (el segundo `if (pRideInfo != nullptr)`)
y la nuestra a la **32** (más allá de los dos): **`jump.c` nos enhebra el salto y
al objetivo no.** Es una fila de diff independiente del `lis`.

No he hecho ensayos aquí: la lista de vedas del propio fuente (líneas 1963-1975
de `CarRender.cpp`) ya cubre las nueve formas locales evidentes, y la r27 añadió
o1/o2/o3.

---

## 5. `RenderFlaresOnCar` (2.908 B) — mismo mecanismo, signo contrario

18 filas, y la raíz es la misma familia que el §4:

```
objetivo  fila 123  lis r16, lbl_8040AD04@ha     <- IZADO al principio, vive toda la funcion
          filas 600/601  lfs f1/f2, lbl_8040AD04@l(r16)
nuestro   fila 452  lis r9,  lbl_8040AD04@ha     <- materializado tarde
          fila 461  lis r30, lbl_8040AD04@ha     <- y OTRA VEZ
```

Es decir: **el objetivo tiene UN pseudo `high` con vida larga y nosotros DOS con
vida corta** — exactamente al revés que en `UpdateWheelYRenderOffset`, donde el
objetivo tiene dos y nosotros uno. Encaja con el `-dG` de la r27
(`PRE/HOIST: end of bb 141, insn 2614`).

Sin ensayos nuevos: la r26 dejó vedadas e1-e10 y la r27 midió f1 (98,707 %, no
cierra y contradice el DWARF).

---

## 6. Vedas nuevas, con la sentencia

- **s1** — los tres `goto` (`search_node:`/`if`/`goto`) en el bucle de `SAP.h`:
  **84,03 %**, 1460 B (−28 B). Es el mismo error que la `b1` de la r27 en otra
  función: quitar/añadir estructura sin la causa real **empeora**.
- **s2** — `while (B) { …; if (head == nullptr) break; }`: **88,27 %**, 1440 B.
  El `break` al final del cuerpo **bloquea la rotación** (el último insn del
  cuerpo es un salto condicional) y se pierden las 3 insns del preencabezado.
- **t1/t2/t3/u5** — accesor inline en la condición: **86,28-86,30 %**, 1456 B.
  Bloquea la rotación pero **pierde la duplicación del preencabezado**.
- **u4/u6/u7/u8** — accesor sólo en el cuerpo, guardián+`do…while`,
  guardián+`for(;;)` con dos `break` (con y sin accesor): **objeto IDÉNTICO al
  control**. `jump.c` invierte igual porque el salto de salida queda pegado al
  incondicional.
- **u9/ua** — `Node *next = head->mTail;` delante de `node = head;`:
  **94,70 %**, 1520 B (un `mr` de más por bucle).
- **ub** — la condición en un `bool keep_going` recalculado al fondo:
  **92,84 %**, 1544 B (el bool se materializa en GPR).
- **u2** — guardián `if (B)` + `while (A && B)` con accesor: **93,49 %**, 1520 B.
  **No es una veda del todo**: da el preencabezado del objetivo fila a fila y el
  bucle sin rotar; le sobra el `cror;bso` duplicado.
- **p1** — `NumSpongeAllocations = size - size`: se pliega, objeto idéntico.

## 7. Lo que NO he probado

- **`SetMemoryPoolSize`**: ninguna forma que meta `this->NumSpongeAllocations = 0;`
  en **otro bloque básico** (haría falta una etiqueta viva entre la última llamada
  y esa sentencia). Es la única palanca que queda y no sé escribirla en C++ sin
  que `jump2` borre la etiqueta.
- **El bucle de `SAP.h`**: no he probado ninguna forma que deje el
  **`cror;bso` fuera del alcance de la inversión de `jump.c`**, que es lo que
  pide el mecanismo del §3.2 — o sea, algo que ponga una **etiqueta viva** entre
  el `beq` de salida y el `b` de vuelta. Tampoco he probado `permuter.py` ni
  restricciones de registro sobre estas dos funciones.
- **`UpdateWheelYRenderOffset`**: ningún ensayo. La palanca identificada es
  **subir la presión de registros en una unidad** para que `loop.c` no pueda izar
  `TweakKitWheelOffsetRear`; nada de la lista de vedas del fuente ataca eso —
  todas tocan la forma de los `0.0f`. **Es lo que yo haría primero** de lo que
  queda: son 4 B y una sola instrucción.
- **`RenderFlaresOnCar`**: ningún ensayo nuevo. No he combinado la f1 de la r27
  con las e* de la r26.
- **`AssignClosestOffsets`**, **`CullParts`**, **`UpdateLoaded`**,
  **`DefragmentPool`**, **zGameplay**: cero ensayos. Negativos firmes de la r27 y
  del brief; no he encontrado ángulo nuevo y he preferido gastar el presupuesto
  en los 4.396 B del `blt`→`b`.

## 8. Herramientas dejadas en el scratchpad (prefijo `c28wag_`)

- **`c28wag_min.py`** — compila **un `.cpp` cualquiera** con los cflags exactos de
  una unidad y escupe el `.s`. **2 s por iteración.** Es lo que ha permitido
  barrer 25 formas de bucle.
- **`c28wag_shape.py`** — limpia el `.s` de todo el ruido de `.section .line` y
  `.debug_*`; deja sólo el cuerpo de cada función. Sin él el `.s` de un caso
  mínimo son 800 líneas de las que 780 son DWARF.
- **`c28wag_rtlmin.py`** — volcados RTL por pase de un `.cpp` suelto (`-d<letras>`).
  Con `rjsGLtfcS` salen `.rtl .jump .cse .gcse .loop .cse2 .flow .combine .sched`
  en 3 s. **Es lo que demostró que la rotación es de `expand_end_loop`.**
- **`c28wag_pr.py`** — parchea **cualquier fichero** (cabecera incluida), compila
  el `.cpp` suelto, restaura por md5 y enseña las filas del diff. Ciclo **4 s**.
  Es `c27wag_patchrows.py` pero desacoplando «qué parcheo» de «qué compilo».
- **`c28wag_t3.cpp`** — el caso mínimo de 27 líneas que reproduce los dos
  defectos del bucle de `SAP.h`. **Guárdalo**: es el banco de pruebas de 4.396 B.
- `c28wag_t2/t4/t5/t6/t7.cpp` — los lotes de formas barridas.
- `c28wag_p_*.py` — los parches de cada ensayo.
- `c28wag_audit_*.txt` — las cuatro auditorías.
