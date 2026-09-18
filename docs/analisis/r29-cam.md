# Ronda 29 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** El árbol queda **byte a byte como lo encontré**:
`git status src/Speed/Indep/Src/Camera/` **vacío**, `git status
src/Speed/Indep/bWare/` **vacío**, y los md5 de la r28 intactos (`ICEMover.cpp`
`edc3423a…`, `ICEMath.hpp` `ec9d9c20…`, `ICEManager.hpp` `4bf44755…`,
`TrackCop.cpp` `f7c2a733…`, `UTLVector.h` `d2d4c8e6…`). **Todos** los ensayos se
hicieron sobre copias parcheadas en un directorio-sombra del scratchpad con `-I`
delante; el árbol compartido no se tocó ni una vez. `audit.py`: **446/446 ok,
CERO FALLA**, dos pasadas.

(Aviso de convivencia: `src/Speed/Indep/SourceLists/zBWare.cpp` sale modificado
en `git status` — es de otro agente de esta ronda, `#pragma implementation
"iallocator.h"`. No lo he tocado ni afecta a zCamera.)

Lo que traigo:

1. **`ICEMover::Update` (3.868 B) baja de 15 a 10 filas** con la barrera
   selectiva: **99,90693 % → 99,93278 %**. Las cinco filas que caen son
   **exactamente el diagnóstico de la r28**: la guarda del `while (a<0)` pasa a
   comparar el ARGUMENTO, como el objetivo. Queda **una** cosa, medida y
   acotada: el operando de la barrera retiene `r8` hasta el final y el objetivo
   lo recicla en la fila 602. **24 ensayos numerados.** (§1)
2. **`TrackCar::Update` (992 B): el mecanismo cazado al dígito** con el volcado
   ciclo a ciclo de `sched1`. No es «prioridad + LUID»: es
   **prioridad → `INSN_REG_WEIGHT` → LUID**, y lo he verificado prediciendo el
   orden de las tres cargas Y de las tres tiendas. La palanca vive en `bFill` de
   `bMath.hpp` y **está medida como catastrófica** (−29 pp de unidad). (§2)
3. **`TerrainVelocityNoise` (1.192 B): la barrera selectiva NO aplica, por
   construcción.** Un `elf_high` **no tiene operandos de registro**, así que
   ningún `asm` puede crear una dependencia que lo retrase. Veda cerrada. (§3)
4. **`static-init` (3.604 B): probada por primera vez la hipótesis de la
   r21/r22/r25** («+26..31 insns RTL en el prefijo que mueran antes del código
   final»). **Los `asm` vacíos NO sirven**: nueve sondas, ninguna baja los 4
   `addi`, y a partir de K=10 emiten 4 B. (§4)
5. **Herramienta general nueva y verificada: un `__asm__("" : "+r"(x))` NO
   volátil con la salida sin usar sobrevive hasta `cse` y DESAPARECE antes del
   reparto de registros** (i6/i11: cambian el reparto, emiten 0 B). Es un
   «insn fantasma» que mueve decisiones de `cse` sin coste. (§1.3)

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera --muro` + `measure.py`, al empezar
y al terminar, idénticos al brief:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28Camer faltan 1, sobran 2, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1
    MURO: 3868 ICEMover (15) / 1192 TerrainVelocityNoise (14) / 992 TrackCar (15) / 948 TrackCop (2)
    measure.py zCamera   113080/125008 B  90,4582 %  446 al 100 %

Mi arnés (`c29cam_unit.py`, heredado de la r28, unidad entera contra objdiff,
15-20 s) reproduce la base de la r28 al dígito: **120941/145125 B, 83,3357 %,
615 fn al 100 % de 627**, con ICEMover 15 diffs, TVN 14, TrackCop 2, TrackCar 15,
static-init 54.

**Coordinación**: el brief da `TrackCop` al agente de las cuatro funciones del
eje de barrera. **No he gastado un solo ensayo en `TrackCop`**; sólo he
confirmado que sigue en 2 diffs al empezar y al terminar.

---

## 1. `ICEMover::Update` (3.868 B): de 15 filas a 10, y qué queda

### 1.1 La barrera selectiva cierra el diagnóstico de la r28

La r28 dejó demostrado que las 15 filas cuelgan de **una** diferencia de RTL: la
guarda del `while (a < 0)` compara el pseudo del **argumento** en el objetivo y
el de la **copia del parámetro** en el nuestro, y lo decide `make_regs_eqv` de
`cse`. El modelo de `make_regs_eqv` dice que el pseudo **nuevo** (la copia, 1045)
se queda de canónico si

    (ultimo_uso(1045) > cse_basic_block_end  ||  primer_uso(1045) < cse_basic_block_start)
    &&  ultimo_uso(1045) > ultimo_uso(1037)

y la única condición atacable es la segunda. **El ensayo `i4`** hace justo eso:

```c
int nf = ICE::FloatToInt(fParam * length * current_sec);
int frame = ICE::SignedMod(nf, pShake->GetNumKeys());
ICEShakeData *pKey = pShake->GetKey(frame);
__asm__ __volatile__("" : "+r"(nf));       /* BARRERA SELECTIVA — lo digo */
```

    base   3868 B  99,90693 %  15 diffs   filas 589 593 594 596 602 604 605 606 608 610 611 612 614 615 617
    i4     3868 B  99,93278 %  10 diffs   filas          602 604     606 608 610 611 612 614 615 617

Las cinco que caen son las del diagnóstico:

    589  mr r7, r0         <- NumKeys va a r7 como el objetivo (antes r8)
    593  lwz r8, 0x15c(r1) <- el argumento va a r8 como el objetivo
    594  mr r11, r8        <- la copia del parametro, con los registros del objetivo
    596  cmpwi r8, 0x0     <- LA GUARDA COMPARA EL ARGUMENTO  <-- el diagnostico de la r28
    605  extsh r9, r7

**La posición manda**: la misma barrera puesta **antes** de `GetKey` (`i2`) da la
base exacta, porque el último uso de 1045 está **dentro** de la expansión de
`GetKey` (la fila 615, `cmpw`). Tiene que ir **detrás** de `GetKey`.

### 1.2 Lo que queda: una sola cosa, y está acotada

Las 10 filas restantes son **estructuralmente idénticas**, un desplazamiento
uniforme de registros:

    objetivo   resultado=r8   temp1=r11  temp2=r10     (r8 se recicla en la fila 602)
    i4         resultado=r11  temp1=r10  temp2=r9      (r8 sigue ocupado por nf)

El objetivo tiene **tres** pseudos donde nosotros tenemos dos: argumento (r8,
593→596), `a` (r11, 594→602) y resultado (r8 otra vez, 602→617). La barrera
mantiene `nf` vivo de 593 hasta la fila ~617, así que `r8` no se puede reciclar
en la 602 y toda la cadena baja un puesto. **Ése es el único blocante que
queda**, y está medido: a partir de la fila 618 nuestro código es byte a byte el
del objetivo, y ninguno de los dos vuelve a usar `r8`.

Hace falta una barrera cuyo operando (a) tenga `REGNO_LAST_UID` **después** del
de 1045 —lo calcula `reg_scan`, antes de `cse`— y (b) **muera en la fila 602**
—lo calcula `flow`, después—. **Ninguna de las ocho formas que he probado lo
consigue** (§1.4).

### 1.3 Hallazgo general: el `asm` no volátil es un «insn fantasma»

`__asm__("" : "+r"(x))` **no volátil** con la salida sin usar:

- **sobrevive hasta `cse`** — `i6` e `i11` cambian el reparto de registros
  respecto a la base (el argumento cae en `r0` en vez de `r8`) y **giran la
  guarda igual que `i4`** (`cmpwi r0` = el argumento);
- **no llega al reparto**: en `i6` el `r0` que recibe `nf` lo machaca el
  `divw r0, r11, r10` de la fila 600, o sea que `nf` ya está muerto ahí;
- **emite 0 B**: el tamaño no se mueve.

O sea: **una insn RTL que `cse` ve y que no existe después**. Es el ingrediente
que pedía la veda de la r22/r25 para static-init — por eso lo he probado allí
(§4), con resultado negativo, pero la herramienta queda documentada y es
extrapolable a cualquier decisión de `cse` del árbol.

Y el corolario que ahorra ensayos: **un `register int x asm("rN")` NO sirve para
esto**. `make_regs_eqv` empieza con

    ... && (firstr < FIRST_PSEUDO_REGISTER || (<las dos condiciones de vida>))

o sea que **si `firstr` es un registro duro, el pseudo gana siempre**. Medido:
`pinr12` y `pinr6` (pin + barrera) vuelven a 15 filas.

### 1.4 Los 24 ensayos, con su cifra

Todos sobre la unidad entera (`c29cam_unit.py`, 15-20 s), contra
`ICEMover 3868 B 99,90693 % 15 diffs`:

    i1   `int nf` + asm NO volatil "+r" DESPUES de SignedMod       99,90693 %  15  = BASE
    i2   idem con __volatile__                                     99,90693 %  15  = BASE
    i3   `asm("" : : "r"(nf))` despues de SignedMod                99,79835 %  17  PEOR
    i4   `asm __volatile__("" : "+r"(nf))` DESPUES de GetKey       99,93278 %  10  <-- MEJOR
    i5   `asm("" : : "r"(nf))` despues de GetKey                   99,52430 %  26  PEOR
    i6   asm NO volatil "+r" despues de GetKey                     99,90693 %  15  (giro SI, registros NO)
    i7   i4 + SignedMod `return a - c;` / `return 0;`              99,93278 %  10  = i4
    i10  i4 + clobber "memory"                                     99,93278 %  10  = i4
    i11  asm NO volatil "+b" (excluye r0) despues de GetKey        99,90693 %  15  (nf sigue en r0)
    i12  i4 con "+b"                                               99,93278 %  10  = i4
    i13  e1 de la r28 (`frame = SignedMod(frame, ...)`)            99,90693 %  15  = BASE (confirma r28)
    i15  e1 + barrera volatil sobre `frame` tras GetKey            99,90693 %  15  = BASE
    i16  i4 + barrera sobre `frame` antes de GetKey                99,78284 %  19  PEOR
    i17  solo barrera sobre `frame` antes de GetKey                99,67425 %  21  PEOR
    i18  i4 + GetKey `(ICE::Clamp(n,0,N-1) == n)`                  99,93278 %  10  = i4
    i19  i4 + GetKey en forma `if (...) return 0; return &Keys[n]` 99,48294 %  20  PEOR
    i20  i4 + `nf = frame;` delante de la barrera (volatil)        99,90693 %  15  = BASE
    i21  idem no volatil                                           99,90693 %  15  = BASE
    i22  i4 + clobber "r8" en la barrera                           99,90693 %  15  = BASE
    i23  `asm __volatile__("" : "=r"(nf))` (solo escritura)        99,90693 %  15  = BASE
    i24  idem no volatil                                           99,90693 %  15  = BASE
    pinr12 / pinr6  `register int nf asm("rN")` + barrera i4       99,90693 %  15  = BASE

**Ninguno emite un byte de más ni de menos**: los 3.868 B se mantienen en todos.

**No lo he aplicado.** `matched_code` es todo-o-nada: 10 filas siguen siendo
**cero bytes**, y la barrera es un constructo que el original no tiene. Aplicarla
a medias es exactamente lo que la memoria del proyecto llama «trabajo a medias».

---

## 2. `TrackCar::Update` (992 B): el desempate de `rank_for_schedule`, al dígito

### 2.1 Las 15 filas son tres racimos, y los dos primeros son ORDEN puro

Los pares (registro, desplazamiento) son **idénticos** en los dos lados; sólo
cambia el orden de emisión:

    filas 107-111  `Look = *CarToFollow->GetGeometryPosition()`  (bCopy+bFill)
                   objetivo  stfs 0x98(f12), 0x90(f0), 0x94(f13)   <- orden de las CARGAS
                   nuestro   stfs 0x90(f0),  0x94(f13), 0x98(f12)  <- orden de la FUENTE
                   y las CARGAS van igual en los dos: 0x20, 0x18, 0x1c
    filas 113-124  `displacement /= distance`: dos cargas y dos tiendas permutadas
    filas 134-143  renombrado puro de f10/f11/f12 <-> f12/f0/f13 (local_alloc)

### 2.2 El desempate NO es «prioridad + LUID»: hay un tercer criterio

Con `c29cam_rtl.py TrackCar.cpp car -dS -fsched-verbose-5`, bloque `b 13`:

    ;;      insn  code  bb  dep  prio  cost   units
    ;;      393   523   0    1    54    2     lsu    <- lfs 0x18 (x)
    ;;      395   523   0    1    54    2     lsu    <- lfs 0x1c (y)   [lleva REG_DEAD del puntero]
    ;;      397   523   0    1    54    2     lsu    <- lfs 0x20 (z)
    ;;      400   523   0    3    52    2     lsu    <- stfs 0x90 (x)
    ;;      402   523   0    3    52    2     lsu    <- stfs 0x94 (y)
    ;;      404   523   0    3    52    2     lsu    <- stfs 0x98 (z)

Las tres tiendas dependen de **las tres** cargas (una verdadera + dos ANTI: el
alias entre `this+0x90` y `CarToFollow+0x18` no se puede descartar), así que
todas empatan en `prio 52`. El volcado ciclo a ciclo lo confirma:

    ;;  t=3   Ready list: ... 435 395 393 397     --> se elige 397   (z)
    ;;  t=4   Ready list: ... 435 395 393         --> se elige 393   (x)
    ;;  t=5   Ready list: ... 435 395             --> se elige 395   (y)
    ;;        dependences resolved: insn 404 into ready
    ;;        dependences resolved: insn 402 into queue with cost=2
    ;;        dependences resolved: insn 400 into ready
    ;;  t=6   Ready list: ... 435 404 400         --> se elige 400   (x)  <-- AQUI
    ;;  t=7   Ready list: ... 435 404 402         --> se elige 402   (y)
    ;;  t=8   Ready list: ... 435 404             --> se elige 404   (z)

**Regla verificada (se saca de la lista por el FINAL):**

1. **prioridad** más alta;
2. empate → **`INSN_REG_WEIGHT` más BAJO** gana
   (`REG_WEIGHT` = registros que nacen − notas `REG_DEAD`/`REG_UNUSED`);
3. empate → **`INSN_LUID` más bajo** (orden de fuente).

Predice **las dos** cosas:

- **Cargas**: en el orden previo a `sched1` (`x, y, z`, el de `bCopy`) el
  `REG_DEAD` del puntero `v` cae en la **última**, la de `z` → peso 0 contra 1 →
  **`z` sale primero**; luego `x` e `y` por LUID. **z, x, y**, que es lo que
  emiten los dos lados.
- **Tiendas**: las tres empatan a peso −1 (una `REG_DEAD` cada una, la del flotante
  que consumen) → LUID → **x, y, z**, que es lo que emitimos nosotros.

**Aviso al leer el volcado**: en el `.sched` (posterior al pase) el `REG_DEAD`
del puntero aparece ya en la carga de `y`, porque las notas se recalculan tras
planificar. Los pesos que decidieron son los de ANTES.

Para que el objetivo emita z, x, y con estos mismos pesos, la tienda de `z`
tendría que **matar un registro más** (peso −2) o llevar LUID menor.

### 2.3 La palanca vive en `bMath.hpp` y está MEDIDA como catastrófica

`lmap.py` sobre el objetivo sitúa las tres cargas en **`bMath.hpp:906`** y las
tres tiendas en **`bMath.hpp:905`** (una línea cada grupo): el original **sí**
pasa por `bCopy`+`bFill`. El ensayo directo:

    T1   bFill con `dest->z = z; dest->x = x; dest->y = y;`
         unidad 78377/145125 B  54,0065 %  577 fn (base: 83,3357 %, 615 fn)
         TrackCar 29 diffs (base 15) · ICEMover 69 (base 15) · static-init 311 (base 54)

**Veda con la medida: el orden `x, y, z` de `bFill` es el del original.** Un
cambio ahí cuesta **−29 pp de unidad** y toca las 100+ unidades que incluyen
`bMath.hpp`.

**Cero ensayos de fuente gastados en TrackCar**, a propósito: la única palanca
identificada está vedada y ninguna barrera puede bajar el peso de una tienda.

---

## 3. `TerrainVelocityNoise` (1.192 B): la barrera NO aplica, y es por construcción

Con los símbolos emparejados por sus `lfs`, la diferencia son **los cinco `lis`**
del pool, y sólo su ORDEN:

    objetivo   1D88, 1D8C, 1D94, 1D90, ... 1D70   <- el del 0.0f, PEGADO a su lfs (fila 129-130)
    nuestro    1D88, 1D70, 1D8C, 1D94,     1D90   <- orden de prioridad 7,7,5,5,4

**El eje de barrera está cerrado por construcción**: `(set (reg X) (high
(symbol_ref ...)))` **no tiene ningún operando de registro de entrada**. En
`sched_analyze` un `asm` extendido no volátil sólo crea dependencias de **lo que
nombra**; como no hay nada que nombrar que el `elf_high` lea, **ninguna barrera
selectiva puede retrasarlo**. Y atarlo por el otro lado (poner el `asm` entre el
`lfs` y su uso) **alarga** la cadena, o sea **sube** la prioridad: es el
movimiento contrario al que hace falta.

Ensayos gastados (los dos que sí tenían base teórica, por la regla del `const`
del brief §1):

    t2   `const float accel_max = 20.0f;` -> `float accel_max = 20.0f;`   99,07718 %  14  = BASE
    t3   `float zero = 0.0f;` y `bClamp(..., zero, accel_max)`            99,07718 %  14  = BASE

**Veda nueva:** ninguna barrera selectiva puede tocar un `elf_high`; queda en pie
la conclusión de la r28 (el `lis` del objetivo nació **después** de `sched1`, es
una rematerialización de `reload`).

---

## 4. `static-init` (3.604 B): probada la hipótesis de la r21/r22/r25, y es NO

La veda de la r25 decía: «la única posibilidad sigue siendo **+26..31 insns RTL
en el prefijo que mueran antes del código final**». El `asm` fantasma de §1.3 es
literalmente eso, así que lo he probado — **primera vez que se gastan ensayos
aquí** (la r28 gastó cero).

**Qué falla exactamente** (medido, y el brief no lo decía): nuestro
static-init mide **3.620 B contra 3.604 B del objetivo**, +16 B = los 4 `addi`.
El objetivo direcciona `ReplayCategoryTable` con desplazamiento grande sobre la
base (`stw r9, 0x40(r30)`) y nosotros materializamos un puntero por elemento
(`addi r27, r30, 0x30` + `stw r9, 0x10(r27)`), a partir del **elemento 2**.

**Sonda**: una `static inline` con K `asm` muertos, llamada en el primer
argumento del elemento 0 de la tabla (o sea, en el PREFIJO, como las sondas de la
r21), en `ICEReplay.cpp`.

    K   variante A (K variables, K asm)          variante B (1 variable, K asm encadenados)
    4   3620 B   97,11210 %  54 diffs  = BASE    3620 B  97,11210 %  54  = BASE
    10  3624 B   96,51942 %  64 diffs  PEOR      3624 B  97,01221 %  54
    16  3624 B   96,15871 %  87 diffs  PEOR      —
    26  —                                        3624 B  96,15871 %  87  PEOR
    31  3624 B   96,15871 %  87 diffs  PEOR      3624 B  96,15871 %  87  PEOR
    40  —                                        3624 B  96,15871 %  87  PEOR

**Nunca bajan los 4 `addi`** (el tamaño nunca baja de 3.620) y a partir de K=10
**emiten 4 B**, o sea que dejan de ser fantasmas. Compárese con la sonda de la
r21, que con globales reales llegaba a **0 `addi` en N=10** (+31 insns).

**Veda nueva, con la medida: las «+26..31 insns RTL» de la r21/r22 tienen que ser
insns REALES en el momento de `cse`; los `asm` vacíos no cuentan para la ventana
de `cse_end_of_basic_block`.** La única salida sigue siendo una **forma de
expresión** en alguno de los globales previos que expanda 26-31 insns RTL — y eso
el DWARF no lo puede señalar (veda 1 de la r22).

---

## 5. Lo que NO he probado

- **`TrackCop` (948 B): CERO ensayos**, por coordinación con el otro agente del
  eje de barrera. Confirmado en 2 diffs al empezar y al terminar.
- **`_Storage::assign` (1.156 B)**: cero ensayos. Cerrada por la r27 (aun
  casándola daría cero bytes) y el brief lo veta.
- **`LoadCameraShakes` (168 B)**: cero ensayos (cerrada por DWARF en la r26).
- **`TrackCar`: cero ensayos de fuente.** Sólo el diagnóstico de §2 y el
  contra-ensayo `T1` sobre `bFill`. **No he probado** permutar el orden de las
  tres cargas de `bCopy` (`float x/y/z = v->…`) ni sacar `dest` a un puntero
  vivo en `bFill` para que la tienda de `z` mate dos registros (peso −2): las dos
  son cambios en `bMath.hpp`, o sea A/B sobre 100+ unidades, y no caben en una
  ronda de un agente.
- **`ICEMover`: no he mirado el `.greg` ni `alloc.py`** para el racimo 602-617.
  El corte podría estar en `local_alloc` (que `alloc.py` no modela) o en
  `global_alloc`; **hay que comprobar antes la línea `;; N regs to allocate:`**,
  como avisa la r28.
- **`permuter.py`**: sigue sin usarse en zCamera.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**; el
  brief lo condiciona a cerrar algo con ganancia de bytes y no he ganado ninguno.
- **`configure.py` / cflags / `-fno-implement-inlines`**: no he tocado nada. El
  brief ya mide que el `.text` de más no llega al DOL.
- **La vtable `_vt.Q33Sim9Collision9IListener`**: sigue siendo el único símbolo
  ausente; no la he tocado.

---

## 6. Vedas nuevas, con la sentencia barrida

1. **La barrera selectiva SÍ gira `make_regs_eqv` en `ICEMover`, pero sólo
   DETRÁS de `GetKey`** (`i4`, 15→10 filas). Delante de `GetKey` (`i2`) no hace
   nada, porque el último uso del pseudo de la copia está dentro de la expansión
   de `GetKey`.
2. **Un `register int x asm("rN")` no puede sustituir a la barrera**:
   `make_regs_eqv` promociona al pseudo en cuanto `firstr` es un registro duro.
   Barridas: `pinr12` y `pinr6`, las dos vuelven a 15 filas.
3. **Un `asm` de sólo escritura (`"=r"`) no alarga `REGNO_LAST_UID`**: `i23`
   (volátil) e `i24` (no volátil) dan la base exacta.
4. **Un `asm` de sólo entrada (`"" : : "r"(x)`) es mucho peor que `"+r"`**: 17 y
   26 filas contra 15 y 10. Es implícitamente volátil y toma el camino de
   «clobber all».
5. **El orden `x, y, z` de `bFill` (`bMath.hpp:478-483`) es el del original**:
   la permutación `z, x, y` cuesta **83,3357 % → 54,0065 %** de unidad y sube
   TrackCar de 15 a 29 filas (ensayo `T1`).
6. **Ninguna barrera selectiva puede retrasar un `elf_high`**: no tiene operandos
   de registro de entrada, así que no hay nada que `sched_analyze` pueda atar.
   Cierra el «salvo, quizá, la barrera selectiva» del brief para
   `TerrainVelocityNoise`.
7. **Quitar el `const` a `accel_max` y sacar el `0.0f` a un local no mueven una
   instrucción** en `TerrainVelocityNoise` (`t2`, `t3`).
8. **Los `asm` vacíos NO cuentan para la ventana de `cse_end_of_basic_block`**:
   nueve sondas de K=4..40 en el prefijo de static-init, ninguna baja los 4
   `addi`, y a partir de K=10 emiten 4 B. Las «+26..31 insns RTL» de la r21/r22
   tienen que ser insns reales.

---

## 7. Regla nueva para `HERRAMIENTAS.md`

**El desempate del planificador de GCC 2.95 tiene TRES criterios, no dos**:
prioridad → `INSN_REG_WEIGHT` (gana el **menor**; peso = registros que nacen
menos notas `REG_DEAD`/`REG_UNUSED`) → `INSN_LUID`. Verificado prediciendo, en el
mismo bloque, el orden de tres cargas empatadas en prioridad **y** el de tres
tiendas empatadas en prioridad y peso. **Corolario práctico: si el objetivo
reordena un grupo de instrucciones empatadas y tú no, mira qué instrucción lleva
el `REG_DEAD` del puntero/base — no el orden de la fuente.**

Y la nota de uso: **con `-dS` el volcado ciclo a ciclo de `sched1`
(`-fsched-verbose-5`) va DENTRO del `.sched`**, detrás de la tabla
`--- Region Dependences ---` del bloque. Las líneas
`Ready list (t = N): ... a b c` con `--> scheduling insn <<<c>>>` dicen que **se
saca por el FINAL de la lista**.

---

## 8. Herramientas (scratchpad, prefijo `c29cam_`)

    c29cam_unit.py   compila zCamera ENTERA con un DIRECTORIO-SOMBRA (-I delante)
                     con copias parcheadas; la mide con objdiff y saca el % y el
                     numero de diffs de las cinco funciones vigiladas. 15-20 s.
                     El arbol NO se toca. (heredado de la r28, prefijos c29)
    c29cam_mk.py     genera un parche <tag> a partir de (fichero, viejo, nuevo).
                     **Convierte LF->CRLF solo, mirando el fichero destino**:
                     ICEMover.cpp es CRLF, ICEMath.hpp y bMath.hpp son LF.
    c29cam_fd.py     fndiff contra el .o de un ensayo del arnes (no contra build/).
                     Con ALL=1 imprime tambien las filas que casan.
    c29cam_rtl.py    preprocesa con los cflags de zCamera y llama a cc1plus a mano.
    c29cam_simk.py / c29cam_simkb.py   generan las sondas de K `asm` de §4.
    c29cam_p_*.py    los 30 parches de los ensayos.
    c29cam_car.diff / c29cam_ice.diff / c29cam_tvn.diff / c29cam_si.diff
    c29cam_car.lmap / c29cam_si.lmap   mapas de linea del objetivo.
    c29cam_audit1.txt / c29cam_audit2.txt   las dos pasadas de audit.

**Trampas nuevas:**

- `fndiff.py` formatea con `e['instruction']['formatted']`, no con
  `e['formatted']`. Un script propio que use la clave de arriba imprime `?`.
- El arnés de la r28 tiene el nombre del fichero de parche **hardcodeado con
  `assert new != txt`**: si el `OLD` no casa por finales de línea, el error sale
  como un `Traceback` truncado de dos líneas y parece un fallo del compilador.

---

## 9. Verificación final

    build_direct.py zCamera                       ok
    triage.py zCamera --muro                      identico al brief (3 near-miss, 4 muros)
    measure.py zCamera                            113080/125008 B  90,4582 %  446 al 100 %
    audit.py Speed/Indep/SourceLists/zCamera      446/446 ok, CERO FALLA  (dos pasadas)
    md5 ICEMover.cpp   edc3423a7989bf96b6ce5925412a7983   (= r28)
    md5 ICEMath.hpp    ec9d9c201fb6b4138fd210f42f97c8d6   (= r28)
    md5 ICEManager.hpp 4bf44755d305a929827e6f99f20f28d8   (= r28)
    md5 TrackCop.cpp   f7c2a733a30af8b7e83c25b92813a648   (= r28)
    md5 UTLVector.h    d2d4c8e66789362ea032f7af80a6c70a   (= r27/r28)
    md5 bMath.hpp      4190acb3b919f8bd4ece9d117a654126
    git status src/Speed/Indep/Src/Camera/        vacio
    git status src/Speed/Indep/bWare/             vacio
    git status src/Speed/Indep/SourceLists/       zBWare.cpp (de OTRO agente)

`frozen.py` no lo he tocado: no he cambiado nada. Disco: arranqué con 16 GB
libres y termino con 14 GB tras limpiar mis `.json` de objdiff, los `.o`, los
`.ii`, los volcados RTL y los directorios-sombra; lo que dejo ocupa menos de
1 MB.
