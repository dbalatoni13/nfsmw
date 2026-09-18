# r25 — zWorld + zAI + zGameplay + zPhysicsBehaviors

Encargo: 11 near-miss, ~16,2 kB. **Cierres: 0.** Lo que traigo es:

- el **veredicto medido** sobre el `blt`→`b` compartido (§2): *no* es un frente,
  y ahora está acotado con un barrido del binario original entero;
- un **frente nuevo, contado**: el `@ha` que se rematerializa — **3 funciones,
  4.640 B**, y en `RenderFlaresOnCar` con **el registro y la variable con
  nombre** (§4);
- **34 vedas nuevas** con cifra (27 formas de bucle + 30 banderas + 5 de fuente);
- una **regresión ajena en vuelo** que hay que mirar antes de cerrar la ronda (§7).

## 0. Estado verificado al empezar (no del papel)

`build_direct.py zWorld zAI zGameplay zPhysicsBehaviors` + `triage.py`: **el
encargo coincide exacto**, función a función y porcentaje a porcentaje.

    4 unidades (5 objetos)  927880/950788 B  97,5906%  3841 funciones al 100%

| B | % | unidad | función | firma triage |
|---|---|---|---|---|
| 2908 | 99,601 | zAI | `UpdateAllAvoidables` | 4 SUST · b+4 blt-4 |
| 1488 | 99,301 | zPhysicsBehaviors | `Add__6RBGrid` | 4 SUST · b+4 blt-4 |
| 2908 | 98,618 | zWorld | `RenderFlaresOnCar` | sobran 1, 1 SUST · lis-1 |
| 1684 | 99,667 | zAI | `AssignClosestOffsets` | faltan 2, sobran 1 · mr.-1 mr+1 cmpwi+1 |
|  876 | 99,384 | zWorld | `UpdateWheelYRenderOffset` | faltan 1 · lis+1 |
|  856 | 97,210 | zPhysicsBehaviors | `UpdateLoaded__SuspensionTraffic::Tire` | faltan 2, sobran 3 · stmw+1 lmw+1 stw-1 lwz-1 (**y lis-1**) |
|  836 | 99,426 | zWorld | `CullParts` | faltan 1 · mr+1 |
| 2544 | 99,583 | zGameplay | `GTrigger::GTrigger` | MURO: reorden local, dmax 6 |
| 1680 | 99,036 | zGameplay | `GenerateIndex` | MURO: reorden local, dmax 2 |
|  684 | 99,269 | zWorld | `DefragmentPool` | MURO: 23 registros |
|  304 | 97,368 | zWorld | `SetMemoryPoolSize` | MURO: reorden local, dmax 2 |

**Calibración de `triage.py`: CORRECTA.** Las dos funciones testigo dan lo que
dice el encargo — `RenderFlaresOnCar` **`sobran 1, 1 SUST, lis-1`** y
`Add__6RBGrid` **4 SUST**. Un matiz que conviene anotar y **no** es un fallo:
`RenderFlaresOnCar` **no** sale en la sección «BLOQUE MOVIDO» (que da 0
funciones) porque tiene delta de código y la clasificación es excluyente; el
`dmax 313` está en el docstring de la herramienta, no en su salida.

## 1. `audit.py` — lista de FALLA: **VACÍA**

Una pasada sobre las cuatro unidades, tras `build_direct.py`:

| unidad | funciones auditadas | FALLA |
|---|---|---|
| zWorld | 577 | **0** |
| zAI | 1028 | **0** |
| zGameplay | 765 | **0** |
| zPhysicsBehaviors | 1118 | **0** |

**No hay ningún FALLA que confirmar con una segunda pasada.** No he congelado
nada (no he cerrado nada) y no he re-congelado unidades ajenas.

---

## 2. El `blt`→`b` compartido — VEREDICTO: **misma causa, pero NO es un frente**

### 2.1 Que es la misma causa: sí, y ahora medido sobre el binario entero

`UpdateAllAvoidables` (zAI) y `Add__6RBGrid` (zPhysicsBehaviors) son **la misma
línea de `SAP.h`**, el bucle del ctor `SAP::Grid<T>::Axis::Node::Node`, inlineado
**4 veces en cada una** (mMin/mMax × ejes X/Z). 8 instancias, 8 diffs de `b`/`blt`
y 8 de `lwz` — **el diff entero de las dos funciones son esas 16 filas**
(`Add`: 8 filas de 372 · `UpdateAllAvoidables`: 14 de 727).

Barrido nuevo, sobre **todos** los `.s` del troceador (`build/GOWE69/asm`), del
patrón «`b` incondicional hacia atrás a una etiqueta que empieza en rama
condicional (permitiendo `cror`/`crand` delante)»:

    14 instancias en 8 funciones de TODO el DOL
      UpdateAllAvoidables   zAI                 x4   cror bso mr lwz cmpwi beq lfs lfs fcmpu b
      Add__6RBGrid          zPhysicsBehaviors   x4   cror bso mr lwz cmpwi beq lfs lfs fcmpu b
      VP6_FindNearestand…   criticalpath        x1   (100 %)
      UpdateSiren           zAI                 x1   (100 %)
      GetCurveOutput        zEAXSound2          x1   (100 %)
      GetNewParticle        zEcstasy            x1   (100 %)
      CreateEmitterGroup    zEcstasy            x1   (100 %)
      ChangeDragLanes       zWorld2             x1   (100 %)

**Las otras seis ya casan al 100 %** (comprobado en `report.json`), o sea que
tenemos seis ejemplos de fuente conocida que producen un retroceso `b`. Leí
`GetNewParticle` (EmitterSystem.cpp:760): su bucle es
`for (grp = …GetHead(); !done && grp != mEmitterGroups.EndOfList();)` y su forma
es **la NO rotada** (los dos tests en la cima y `b` al final), que **no** es la
del ctor de SAP. **El ctor de SAP es la única instancia del patrón en todo el
juego.** Un frente de una función.

### 2.2 El mecanismo, acotado por medida

Con un caso mínimo de 30 líneas (0,6 s por variante, `scratchpad/p/`) que
reproduce el `blt` exacto, `expand_end_loop` resulta ser **binario**:

| forma | insns | cima del bucle | retroceso |
|---|---|---|---|
| **nuestra** (`while (h && h->p < s->p)`) | 41 | `mr` (tras `cror;bge`) | **`blt`** |
| rotación **suprimida** (inline en la condición, `break` muerto…) | 39 | el `cmpwi` del null test | **`b`** |
| **el objetivo** | 41 | el **`cror`** | **`b`** |

O sea: el objetivo es una **rotación PARCIAL** —la *rama* del test flotante se
queda en la cima y su *cómputo* baja al final, con una copia en el preencabezado—
y **ni la rotación completa ni su ausencia la producen**. Ninguna de las 27
formas de fuente que he barrido la produce, y **ninguna de 30 banderas** (incluidas
`-O0`, `-O2`, `-g0`, `-fno-schedule-insns2`, `-fno-gcse`, `-fno-cse-follow-jumps`,
`-fno-rerun-loop-opt`, `-funroll-loops`) mueve la forma un ápice.

**Conclusión: 4.396 B detrás de UNA decisión de `expand_end_loop`/`jump.c` que no
sé provocar, no detrás de un frente.** No lo vendo como frente.

### 2.3 Lo que sí aporta el mapa de líneas (y no estaba anotado)

`lmap.py` sobre `Add__6RBGrid`, las cuatro copias idénticas:

    mr r6,r9        SAP.h:95     node = head
    lwz r9,0x4(r9)  SAP.h:99     head = head->mTail
    cmpwi r9,0 / beq             (sin nota: sigue en la 99)
    lfs/lfs/fcmpu   SAP.h:94     el test del while
    b               SAP.h:95     el retroceso

Nuestro `.s` da: `mr`→27, `lwz`→28, **y el bloque del fondo SIN nota** (hereda la
28). Dos datos duros:

1. **Entre `node = head` (95) y `head = head->mTail` (99) el original tiene TRES
   líneas más que nosotros**, y **la 97 emite una nota de línea sin código** que
   cae justo en la etiqueta de salida, junto a la 101 (`if (node != nullptr)`).
   Hay sentencias del original que no tenemos ahí.
2. **El bloque del fondo del objetivo conserva sus notas de línea (94 y 95) y el
   nuestro no.** Eso es huella de que las insns llegaron ahí por un camino
   distinto (movidas con sus notas) y no copiadas.

Ésa es la pista que yo seguiría: **reconstruir las líneas 96-98 del ctor**.

### 2.4 Ensayos (base 99,30108 % / 99,6011 %, ciclo 20 s las dos funciones)

Arnés `c25wag_var.py` (copia de SAP.h en el scratchpad + guard `SAP_H`, la real
queda vacía). **Reproduce los dos objetos exactos**: `Add` 1488 B / 8 diffs,
`UpdateAllAvoidables` 2908 B / 14 diffs.

| # | sentencia barrida | Add | UpdateAllAvoidables |
|---|---|---|---|
| b0 | control (fuente actual) | 99,30108 % | 99,6011 % |
| g1 | condición con accesor: `head->GetPosition() < this->mPosition` | **86,277 %** (1456 B) | **93,253 %** (2876 B) |
| g2 | `while (head->mPosition < …) { …; if (head == nullptr) break; }` | **88,272 %** (1440 B) | **94,417 %** (2860 B) |

**Veda (27 formas, en el caso mínimo, todas con su firma de bucle medida):**
`for` con avance en el incremento · `for` con la condición partida · `while(1)`
con `break` · `for(;;)` con los dos `break` · `for(;;)` con guarda `if` peleada
delante · `do/while` · `goto` explícito · condición con `&&`, con paréntesis, con
`>=` negado, con operandos invertidos, con `head` sin `!= 0` · cuerpo con bloque
anidado, con `continue`, con `node = node;`, con `if (0) break;`, con temporal
`next`, con `float p` cacheado, con `head->GetTail()`, con `node->mTail` ·
accesor `GetPosition()` en un lado, en el otro y en los dos. **Ninguna da la
forma del objetivo.** Las que dan `b` (accesor en la condición, `break` muerto,
`goto`) dan la forma NO rotada, que son **2 insns menos por copia** y bajan 11-13 pp.

**Veda de banderas (30, sobre el caso mínimo, mirando la FORMA y no el conteo):**
ninguna mueve la cima del bucle ni el opcode del retroceso. Extiende la de r21,
que miraba sólo el conteo de `b`/`blt`.

---

## 3. zGameplay — 4.224 B: **es `sched2`, y el orden de sentencias YA es el del original**

### 3.1 `GTrigger::GTrigger` (2.544 B, 99,583 %) — 4 filas, todas de planificación

    objetivo  … lis lfs | fneg  mr r3  fmuls  mr r4  bl  li r22,1  lfs 0x34(r1)
    nuestro   … lis lfs | mr r3  fneg   mr r4  fmuls  li r22,1  bl  lfs 0x34(r1)

Mismo multiconjunto; es una permutación. Datos:

- `lmap.py` del objetivo: `lfs f1`→GTrigger.cpp:113, `mr r3/r4`→**UMath.h:455**
  (o sea el original **también** llama a `MultYRot`, ya sin el `r = m;`),
  `li r22,1`→116, `lfs 0x34(r1)`→115, `li r11,0`→117, `lis r8,0x4`→130.
  **Nuestras líneas 68/70/71/72/73/86 casan una a una con 113/115/116/117/118/130**
  (desplazamiento +45): **el orden de sentencias del original es el nuestro**, así
  que reordenar contradice el mapa de líneas y no lo he hecho.
- `regmap.py --ours`: **15 locales con el mismo registro, 0 con registro
  distinto**. No es el asignador.
- Barrido de banderas: `-fno-schedule-insns2` mueve la función (94,241 %,
  73 filas) — **es `sched2`**. `-fno-expensive-optimizations` es idéntico.
- `rank_for_schedule` desempata por LUID, o sea por el orden pre-planificador; en
  el nuestro `mr r3` (UMath.h:214) precede al `fneg` (GTrigger.cpp:68) y en el
  objetivo al revés.

**Ensayos** (arnés `c25wag_inplace.py`, sustitución en el árbol + restauración
verificada por md5; ciclo 8 s porque GTrigger.cpp **sí** compila suelto):

| # | sentencia | resultado |
|---|---|---|
| h0 | control (fuente idéntica por el arnés) | 99,583336 %, 4 filas — **reproduce** |
| h1 | `*triggerWord = (*triggerWord & 0xff000000) \| 0x4810d;` (quita el local `triggerFlags`, que el DWARF del original NO tiene) | **97,973 %**, 51 filas, 2540 B |
| h2 | igual con los operandos del `\|` invertidos | **97,973 %**, 51 filas, 2540 B |

**Veda**: quitar el local `triggerFlags` para casar el DWARF **empeora 1,6 pp**.
El DWARF del original no lo lista, pero la forma sin él no es la del original —
apuntado como contraejemplo de «fíate del árbol de bloques del DWARF».

`regmap` sigue señalando **2 bloques anónimos de más** (los `halfWidth`/
`halfLength` de las dos ramas del `else if`), que la r22 ya midió como
necesarios. No los he vuelto a tocar.

### 3.2 `GenerateIndex` (1.680 B, 99,036 %) — 5 filas: 4 de `sched2` y 1 de `cse`

    filas 113/171:  li r31,0 adelantado una ranura (x2)   -> sched2
    fila  306:      objetivo `ori r0,r0,0x200` / nuestro `ori r0,r30,0x200`

La fila 306 es propagación de copias: tras `mr r0,r30` el objetivo **consume** la
copia y nosotros la puenteamos.

`GRaceDatabase.cpp` **NO compila suelto** (`mn_repro` falla con
`GetRaceFromActivity`), así que el ciclo es la unidad entera: 35 s con
`c25wag_unit.py` (sustituye, `build_direct.py zGameplay`, mide y **restaura
siempre**, con `assert` de md5).

| # | sentencia | resultado |
|---|---|---|
| j0 | control | 99,03571 %, 5 filas, 1680 B — **reproduce** |
| j1 | `*pflags = flags \| (GetRankPlayersByPoints() ? K : 0);` | **IDÉNTICO** (5 filas, 1680 B) |
| j2 | `if (…) { flags \|= K; } *pflags = flags;` | **98,774 %**, 7 filas, **1676 B** |
| j3 | temporal `rankflags` copiado antes del `if` | **98,536 %**, 8 filas |

**Veda**: con los cuatro barridos anotados ya en el fichero por la r22
(temporal antes del `if`, temporal con ternario, sin `pflags`, temporal
reutilizado) son **siete formas** de esa sentencia. La ternaria actual sigue
siendo la mejor y **j1 es exactamente equivalente**.

**Veredicto zGameplay**: los 4.224 B **no son «reparto o planificación» que la
fuente pueda dirigir**. El orden de sentencias ya casa con el mapa de líneas del
original, el reparto de registros es idéntico local a local, y lo que queda son
empates de la lista de listos de `sched2`. No es dinero de esta ronda.

---

## 4. EL FRENTE QUE SÍ SALE, Y CONTADO: el `@ha` que se rematerializa

**Tres funciones, 4.640 B**, con el mismo mecanismo: un pseudo `lis sym@ha` que
gana (o pierde) un registro callee-saved contra otro pseudo.

| B | función | objetivo | nosotros |
|---|---|---|---|
| 2908 | `RenderFlaresOnCar` (zWorld) | **un** `@ha` de `lbl_8040AD04` (0.0f) creado en la fila 123 y vivo 330 insns, sirve a 3 cargas | **dos**: uno local (fila 452) y otro izado (r30) |
|  876 | `UpdateWheelYRenderOffset` (zWorld) | **dos** `@ha` del 0.0f (r19 **y** r16) y rematerializa `TweakKitWheelOffsetRear@ha` | **uno** (r19) y iza `TweakKitWheelOffsetRear@ha` a r16 |
|  856 | `UpdateLoaded__SuspensionTraffic::Tire` (zPhysBeh) | **un** `@ha` en **r30** callee-saved (paga `stmw`/`lmw` y 8 B de marco) para 2 cargas | **cero**: rematerializa `lis` **dos** veces |

Dos de tres van en el mismo sentido (el objetivo iza y nosotros
rematerializamos), la tercera al revés — o sea que la palanca es **quién gana el
registro**, no «izar más».

### 4.1 `RenderFlaresOnCar`: `regmap.py` da el nombre y el registro

Nadie había pasado `regmap` por esta función (r21 fue por `lmap` y formas de
fuente). Da el diagnóstico completo:

    40 locales con el MISMO registro, mismo arbol de bloques, 3 distintas:
      flashHeadlights    original r14  -> nuestro r16
      is_headlight       original r17  -> nuestro r16
      is_brakelight      original r30  -> nuestro r17
    VEREDICTO: candidato REAL a permutacion del asignador

Y **r16 es exactamente el registro donde el objetivo guarda el `@ha`** del `0.0f`
(`lis r16, lbl_8040AD04@ha` en la fila 123 · `lfs f1, lbl_8040AD04@l(r16)` en las
460/600/601). O sea, **la cadena causal está cerrada**:

> el original le da **r14** a `flashHeadlights` y deja **r16** libre para el
> pseudo `@ha`; nosotros le damos **r16** a `flashHeadlights`, el `@ha` pierde y
> se rematerializa dos veces → **un `lis` de más**, y `is_headlight`/
> `is_brakelight` se desplazan detrás.

Es el mecanismo de `_bOutput` de la r20: un contraste de `allocno_compare`, con
`prioridad = floor_log2(n_refs) * n_refs / live_length * 10000`. `flashHeadlights`
(CarRender.cpp:2607) tiene 4 refs: la definición, el `= 1` de la línea 2697 y
**dos lecturas dentro del bucle de flares** (2750 y 2756, `n_refs` ponderado por
profundidad). **La palanca es bajarle `n_refs` o alargarle `live_length`.** No me
ha dado tiempo a probarlo.

### 4.2 Recuento honesto del frente

`triage.py` sobre **las 33 SourceLists**: 23 near-miss (29.872 B). Con delta de
`lis` visible: **2**. Añadiendo `UpdateLoaded` (cuyo `lis-1` no sale porque
`triage.py` **trunca el delta a los 4 opcodes de más peso** — es un punto ciego de
la herramienta que conviene saber): **3 funciones, 4.640 B, y las tres son de mi
encargo**. No he contado el middleware. **No lo vendo como frente del proyecto: es
un mecanismo con tres casos.**

---

## 5. `CullParts` (836 B, 99,426 %) — `regmap` lo cierra por el otro lado

    VEREDICTO: MISMO conjunto de locales, mismo arbol de bloques y MISMO REPARTO
    (20 iguales, 0 con registro distinto, 0 de ambito equivocado)

O sea que **no queda ninguna local que corregir**. El diff son 5 filas y la única
de código es el `mr r9, r0` (fila 102):

    objetivo   lwz r0,0x8(r30) [Polarity] · li r9,0 [debug_print] · cmpwi r0,0 · bne
               lwz r0,0xc(r30) [NumPlanes] · li r31,0 · cmpw r9,r0 · mr r9,r0 · bge
    nuestro    lwz r11,0x8(r30) · li r9,0 · cmpwi r11,0 · bne
               lwz r0,0xc(r30) · li r31,0 · cmpw r11,r0 · (nada) · bge

Los dos sustituyen un registro por la constante 0 en el `cmpw`: el objetivo el de
`debug_print` (r9) y nosotros el de `Polarity` (r11, que `cse` sabe que vale 0
tras el `bne`). Es una elección de `canon_reg` dentro de la clase de
equivalencia, no una local. El fichero ya trae **cinco** vedas medidas
(inicializar `debug_print` a `NumPlanes`, asignarlo también antes del bucle,
moverlo al principio del cuerpo, `for (n = 0; …)`, y quitar la barrera `r11` de
la r21). **No he añadido ninguna**: con `regmap` en IDÉNTICO y cinco formas
barridas, lo que queda es el bloque de depuración de 22 líneas que el mapa de
líneas del original sitúa entre la 634 y la 657 y que no tenemos.

## 6. `DefragmentPool` (684 B) — la tabla vieja de «23 registros» está mal

`lreg.py` **ya funciona** (66 pseudos, tabla completa con flotantes). Pero el
diagnóstico bueno no es de `lreg` sino de `regmap.py --ours`, que lo baja de
«23 registros» a **tres cosas concretas**:

    * 3 locales SOLO NUESTRAS: params (r26), table (r17), zero (r18)  [todas en b1]
    * 1 local en el AMBITO EQUIVOCADO: "hole" — el original la declara en
      b1/b0/b0 y nosotros en b1/b0
    * el bloque b1/b0/b0 FALTA en el nuestro (el original declara `hole` dentro)
    * 1 desplazamiento sin ciclo: num_hole_filling_allocations  r27 -> r25
    * `loop_number` está en memoria en los dos, pero en direcciones distintas

**Eso va primero, y no lo he tocado**: hay que meter `hole` en un bloque anónimo
un nivel más adentro y quitar los tres locales de más. Es trabajo de fuente, no
de asignador, y es lo más barato que queda en mi encargo.

## 7. AVISO — regresión ajena EN VUELO (no es mía)

`measure.py --cmp antes_r25_wag.json despues_r25_wag.json` al cerrar:

    -14828 B, -10 funciones, 3 unidades bajan
      -9172 B  -7 fns  zPhysicsBehaviors  233832 -> 224660
      -4464 B  -2 fns  zWorld2            133264 -> 128800
      -1192 B  -1 fns  zWorld             155348 -> 154156

**No es mío.** Mi árbol está limpio (`git status` sin ninguno de mis ficheros) y
`triage.py` da **las siete funciones de mi encargo con el porcentaje y el tamaño
idénticos al arranque**. La causa es un cambio en vuelo de otro agente en
`src/Speed/Indep/Libs/Support/Utility/UVectorMathGC.hpp` (reordena las
asignaciones de `VU0_quattom4`: sube `result[2][3] = 0.0f` y `result[3][3] = 1.0f`).
Rompe cinco funciones de zPhysicsBehaviors con delta `lfs-N`:

    SetOrientation__9RigidBody          84,418 %   (1176 B)
    DoInstanceCollision2d__9RigidBody   93,343 %   (2204 B)
    DoSRBCollisions__15SimpleRigidBody  89,215 %   (1508 B)
    DoRBCollisions__15SimpleRigidBody   92,268 %   (1508 B)
    RecalcOrientMat__C15SimpleRigidBody 45,930 %   ( 228 B)

**Hay que verificarlo antes del DOL de esta tanda.**

## 8. Aporte neto

**+0 B, +0 funciones.** Todos los ensayos revertidos; los arneses de sustitución
en el árbol comprueban la restauración con md5 y no dejan rastro.

## 9. Qué NO he probado

- **`RenderFlaresOnCar`: bajarle `n_refs` a `flashHeadlights`** (§4.1). Es lo
  primero que haría ahora: es la única palanca con nombre, registro y variable
  identificados, y valen 2.908 B.
- **`DefragmentPool`: el bloque anónimo de `hole` y los tres locales de más**
  (§6). 684 B, y es fuente pura.
- **`UpdateLoaded__SuspensionTraffic::Tire`**: sólo lo he diagnosticado
  (`regmap` no encuentra la firma en el volcado original — hay que buscarla a
  mano por `low_pc`). Cero ensayos.
- **`AssignClosestOffsets`** (1.684 B): no lo he tocado; la r21 dejó dos vedas y
  una idea sin probar (que el contador sobreviva al bucle para que `cse` no
  colapse las dos variables).
- **`SetMemoryPoolSize`** (304 B, MURO dmax 2): ni mirado.
- **Reconstruir las líneas 96-98 del ctor de `SAP.h`** (§2.3) — la única pista
  viva del `blt`→`b`.
- **`permuter.py`** sobre ninguna de las once, ni guiado ni ciego.
- **Ninguna variante con restricciones de registro** en ninguna.

## 10. Herramientas dejadas en el scratchpad (prefijo `c25wag_`, nada en el árbol)

- `c25wag_var.py` — variante de `SAP.h` sin tocar el árbol; mide `Add` y
  `UpdateAllAvoidables` a la vez (20 s).
- `p/probe.cpp` + `p/run.py` + `p/body_*.txt` — **el caso mínimo del bucle de
  SAP a 0,6 s**, con la firma del bucle impresa (preencabezado, cima, retroceso).
  27 variantes guardadas. `p/flags.py` hace el barrido de 30 banderas.
- `c25wag_inplace.py` / `c25wag_unit.py` — sustitución en el árbol + compilación
  (fichero suelto / unidad entera) + medida + **restauración verificada por md5**.
- `c25wag_m.py` — porcentaje, tamaño, nº de filas y delta de opcodes de un
  símbolo entre dos `.o`.
- `c25wag_s.py` + `c25wag_lm.py` — **`lmap` para NUESTRO lado**: compila a `.s` y
  funde cada instrucción con su nota de línea de la sección `.line`. Es lo que
  permitió comparar las notas del objetivo con las nuestras (§2.3, §3.1).
- `c25wag_scanb.py` — barre todos los `.s` del troceador buscando un patrón de
  saltos; es el que dio el recuento de §2.1 y sirve para cualquier otro patrón.
