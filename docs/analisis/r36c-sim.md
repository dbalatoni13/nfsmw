# r36c-sim — zAI, zGameplay, zPhysics, zPhysicsBehaviors (11.836 B repartidos)

**Resultado: `Smackable::Smackable` CERRADA (3.120 B, 99,26154 → 100 %).**
`zPhysics` se queda **sin una sola funcion por debajo del 100 %** (lo que sigue
en rojo son `[.rodata-0]`, `[.data-0]`, `_vt.13PhysicsObject.8ISimable`,
`put_maps` y `lbl_803F74BC`, todos igual que antes).

Las otras cinco no cierran, pero **tres bajan de categoria**: dos pasan de
«no se sabe» a «pasada del compilador identificada con nombre y numero de
linea», y una de ellas resulta estar **fuera de mi territorio**.

Base verificada antes de tocar nada: las seis reproducen su cifra exacta
(`triaje.py` sobre las cuatro unidades recien compiladas).

---

## 0. Bancos: la mini-TU vale tambien donde el .cpp no compila solo

La mini-TU de la r36b (un `.cpp` de una linea que incluye el `.cpp` de la clase)
funciona tal cual para `Smackable.cpp` (**6,5 s**), `SuspensionTraffic.cpp`
(11,7 s) y `RigidBody.cpp` (10,7 s), y reproduce la funcion **byte a byte**.

`AIPursuit.cpp` y `GRaceDatabase.cpp` **no compilan sueltos** (dependen de
declaraciones que trae la SourceList antes). Ahi vale la variante:

    sed -n '1,89p' src/Speed/Indep/SourceLists/zAI.cpp > mini_zai.cpp

es decir, **la SourceList truncada justo detras del include que interesa**:
28 s para zAI (contra ~31 de la unidad entera) y 12 s para zGameplay (contra
~30). Poco ahorro en zAI, mucho en zGameplay, y en los dos casos aisla la
funcion de lo que toquen los demas agentes.

### Aviso: el `%` de `triaje.py` es RANCIO, el `difs` no

`triaje.py` saca la lista de funciones **y la columna `%`** de
`build/GOWE69/report.json`, que solo lo regenera `ninja`; el `difs` lo cuenta al
vuelo con objdiff. Con `build_direct.py` (que es lo que usamos) el resultado es
que **una funcion recien cerrada sigue apareciendo con su porcentaje viejo** y
solo se delata por `difs=0`:

    3120 zPhysics __9Smackable...  99.262  0  PERMUTADOR  regs=0 falta=0 sobra=0 otro=0

Esa linea es una funcion **al 100 %**. Y al reves: una funcion que baje de 100
no aparece en `triaje` hasta que se regenere el report. Para verificar de verdad,
`pctsnap --cmp` (que diffea) o objdiff directo.

---

## 1. `Smackable::Smackable` — 100 %. La QUINTA pieza la dio `regmap.py`

Las r27-r29 dejaron escrita en la fuente una **receta acumulativa de cuatro
piezas** que baja de 26 a 6 diffs (99,839745 %, 3.116 B) y una nota diciendo que
no se dejaba puesta porque la funcion no cerraba. La reproduje exacta y luego
**pase `regmap.py` sobre el .o de la mini-TU con la receta ya aplicada** —no
sobre la base, que es lo que se habia hecho siempre—. Con la receta puesta el
veredicto cambia por completo:

    fn   active         r29   r29   ok          <- la receta ya lo habia arreglado
    b0   rbbehavior     r11   -     <-- DISTINTO
    b0   collision_mask       r11   <-- SOLO NUESTRA

Es decir: al original le queda **`rbbehavior` en r11** y a nosotros en **ningun
registro** (es variable muerta y `flow` nos la borra). Esa plaza de menos corria
el par de direcciones de las dos `UCrc32` de la tercera `LoadBehavior` de
**(r27,r28)** —el objetivo— a **(r28,r29)**, y por eso al objetivo le salia un
`mr r27,r30` (fila 660) que a nosotros nos faltaba: 3.116 B contra 3.120.

**La quinta pieza es un `asm` de cero bytes que mantiene viva la variable:**

```c
__asm__("" : : "r"(rbbehavior));
LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), UCrc32("EffectsSmackable"), Sim::Param());
```

6 diffs → **0**. Verificado sobre el build real de `zPhysics`: `pctsnap` da
+0,739 pp en esa funcion y **ninguna empeora**; las secciones de datos de la
unidad quedan byte a byte igual; `lcfix.py --check` no señala nada.

### La regla que sale de aqui

**`regmap.py` hay que volver a pasarlo DESPUES de cada pieza, no solo al
principio.** El diagnostico de la r29 («falta un `mr`, censo de preservados
identico, no se ve de donde sale») era correcto pero incompleto: el censo de
r14..r31 no ve una variable que el original SI aloja y nosotros no, porque en
nuestro lado esa variable no tiene registro que censar. Lo dice regmap en una
linea, pero solo si se le pregunta con la receta puesta.

Y el corolario para la veda de la r29: ahora que la funcion **cierra**, las
cuatro `asm` vacias se quedan. Se quedan las cinco piezas y **111 lineas de
comentario** con el mecanismo y las quince formas negativas medidas.

### Negativo nuevo, medido

Mantener vivo **`active`** en vez de `rbbehavior` al final del `else`
(`__asm__("" : : "r"(active))`): 6 → **12 diffs**. Mueve `active` de r29 a r28 y
solo acierta el PRIMERO del par (r27); el segundo se va a r29.

---

## 2. `SuspensionTraffic::Tire::UpdateLoaded` (856 B) — pasada localizada, sin palanca

La r36b lo dejo como «frente limpio: es `gcse`». Lo es, pero **gcse funciona
bien**: quien lo deshace es `cse2`. Leido de los volcados de `cc1plus`
(`-dG -ds -dt -dl -dg` sobre la mini-TU, 11 s):

1. **PRE hace exactamente lo que hay que hacer.** El volcado `.gcse` dice:

       PRE: redundant insn 528/694/751/860/920 (expression 40) ... reaching reg is 362
       PRE/HOIST: end of bb 15, insn 988, copying expression 40 to reg 362
       PRE/HOIST: end of bb 19, insn 991, copying expression 40 to reg 362

   y la insercion 988 cae **al final del bloque basico que contiene
   `bl VU0_Atan2`** — el sitio del objetivo, porque `sched1` luego la sube por
   delante de la llamada.

2. **`cse2` (`-frerun-cse-after-loop`) lo deshace entero.** Las cinco copias
   `(set (reg N) (reg 362))` vuelven a ser `(set (reg N) (high (*$LC252)))`.
   Contado por pasada, los `set` de `high(*$LC252)` en esa funcion:

       .jump 6 | .cse 6 | .gcse 3 | .cse2 7 | .lreg 4 | asm final 4

3. Al pseudo 362 le queda **un solo uso**, y entonces `update_equiv_regs`
   (local-alloc.c, «move the register initialization just before the use») lo
   **baja** hasta su uso; ahi ya no cruza ninguna llamada y `global_alloc` le da
   r9 en vez de un preservado. De ahi el `stw r31` contra `stmw r30`, el marco
   0x28 contra 0x30 y los 4 B.

**La fuente es la del original.** Comprobado contra el volcado DWARF: mismas tres
locales (`slip_speed` f11, `catchupfriction` sin registro, `skid_speed` f1),
mismos dos bloques anonimos con `brake_spec`/`bt` y `ebrake_spec`/`ebt`, y mismo
arbol de inlines (Max; BRAKES/At/FTLB2NM/ApplyTorque; EBRAKE/FTLB2NM/ApplyTorque;
cuatro Abs; Atan2a; Sqrt; GRIP_SCALE/At x2; Sina; Min). Las seis apariciones de
`1.0f` de la fuente son las seis del `.cse`.

Banderas barridas (mini-TU, 11 s cada una): `-fno-rerun-cse-after-loop` **848 B /
84,67 %** (conserva de mas: confirma el mecanismo pero no es el objetivo);
`-fno-cse-skip-blocks` 85,16 % / 892 B; `-fno-schedule-insns` 75,81 %;
`-fno-omit-frame-pointer` 96,05 % / 868 B; `-fno-force-mem` 96,98 %. **Identicas**:
`-fno-cse-follow-jumps`, `-fno-expensive-optimizations`, `-fno-move-all-movables`,
`-fno-rerun-loop-opt`, `-fcaller-saves`.

**Veredicto: no hay construccion de fuente en esta funcion que toque la decision
de cse2.** Queda escrito en la cabecera de la funcion.

---

## 3. `SAP.h`: la barrera de `head` se lleva la MITAD de las filas

Esto afecta a `RBGrid::Add` (1.488 B) y a `AIAvoidable::UpdateAllAvoidables`
(2.908 B), que comparten el bucle de insercion del constructor
`SAP::Grid<T>::Axis::Node::Node`.

```c
while (head != nullptr && head->mPosition < this->mPosition) {
    node = head;
    __asm__("" : "+r"(head));      // <- impide que cse sustituya head por node
    head = head->mTail;
}
```

Medido sobre el build real, **sin ninguna regresion**:

| funcion | antes | despues | filas |
|---|---|---|---|
| `Add__6RBGrid...` | 99,30108 % | **99,35484 %** | 8 → **4** |
| `UpdateAllAvoidables__11AIAvoidablef` | 99,6011 % | **99,6699 %** | 14 → **10** |

Arregla las cuatro filas `lwz r9, 0x4(r6)` → `lwz r9, 0x4(r9)` (una por
instancia del bucle) y **no mueve un solo byte de tamaño**. Como ninguna de las
dos cierra y `matched_code` es todo-o-nada, **he restaurado `SAP.h` byte a byte**
(sigue sin aparecer en `git status`). La linea esta aqui para la ronda que
resuelva lo que falta.

### Lo que falta: la arista de retorno, y **NO es cross-jumping**

Las 4 filas restantes son, una por bucle:

    objetivo   ... fcmpu | b →cror        (arista incondicional al `cror` compartido)
    nuestro    ... fcmpu | blt →cuerpo

Direcciones reales del objetivo (funcion en 0x734): el `b 0x7fc` apunta a
**0x7d8, que es el `cror` de la cabecera**; o sea el bucle del objetivo tiene la
**rama en la cabecera** y la arista de retorno incondicional.

La r36b se paro en «ese RTL no puede producir un salto incondicional». La pista
obvia es `jump.c:1945`, que hace **literalmente** eso:

```c
if (cross_jump && condjump_p (insn)) {
    rtx x = prev_real_insn (JUMP_LABEL (insn));
    if (x != 0 && ! jump_back_p (x, insn)) x = 0;
    if (x != 0) find_cross_jump (insn, x, 2, &newjpos, &newlpos);
    if (newjpos != 0) {
        do_cross_jump (insn, newjpos, newlpos);
        /* Make the old conditional jump into an unconditional one.  */
```

**Y esta descartada, con dos pruebas independientes:**

1. `find_cross_jump` exige `minimum` = **2 insns iguales** retrocediendo desde
   los dos saltos. Retrocediendo desde nuestro `blt` esta `fcmpu cr0,f13,f0`;
   retrocediendo desde el `cror` esta `addi r3,r3,0x18` (relleno de `sched2`).
   No casan ni el primero. **Y el objetivo tiene esas dos filas IGUALES que
   nosotros** (indices 40 y 49), o sea que tampoco casarian en su lado.
2. `do_cross_jump` **borra** las insns emparejadas del lado del salto. El flujo
   del objetivo conserva `lfs | lfs | fcmpu` intactos antes del `b`: no se borro
   nada, luego no hubo cross-jump.

`jump_back_p` si se cumple en nuestro lado (`blt` y `cror;bso` son la misma
decision invertida sobre cr0), asi que el unico freno es el `minimum = 2`.

**Conclusion nueva: el RTL del objetivo ya traia la arista incondicional**, con
la rama como cabecera del bucle y el `fcmpu` duplicado en el precabezal y en el
latch. No sale de `duplicate_loop_exit_test` + `jump_optimize` tal como los
tenemos. Formas de fuente barridas (banco de 10,7 s sobre `RBGrid::Add`, todas
**negativas**, y las cuatro con la barrera de `head` puesta):

| forma | resultado |
|---|---|
| `for (;;) { if (!(pos < mPos)) break; …; if (!head) break; }` | 88,33 %, **1.440 B** |
| `while (head->mPosition < this->mPosition) { …; if (!head) break; }` | 88,33 %, 1.440 B |
| `if (head) { test: if (pos < mPos) { …; if (head) goto test; } }` | 87,73 %, 1.440 B |
| lo mismo sin la barrera | 86,96 %, 1.448 B |

Las tres primeras pierden los 48 B porque `loop.c` saca `this->mPosition` del
bucle en cuanto se va el `head != nullptr &&` de la condicion.

---

## 4. `AIPursuit::AssignClosestOffsets` (1.684 B) — el tamaño exacto, ya

Ocho filas: **seis** son la permutacion r28↔r29 (el `@ha` de `-1.0f` contra el
temporal `copsToAssignOffsets - 1`) y **dos son cuatro bytes**: el objetivo emite
`mr r9,r29` + `cmpwi r29,0` y nosotros `mr. r9,r28`.

Localizado el punto exacto: **`combine.c:1699`**, el caso «arithmetic operation
and set the condition code». `i3` es `(set cc (compare cTAO 0))`, `i2` es la
copia `(set cTAO t)` y, como el operando del `compare` **ES** `i2dest`, combine
mete `i2src` dentro del compare y forma el `PARALLEL` `or.`.

**Hallazgo: hay una forma de fuente que rompe la fusion y acierta el tamaño.**

```c
        copsToAssignOffsets--;
    } while (copsToAssignOffsets > 0);
```

da **1.684 B exactos** (contra 1.680 de la base) **y ademas acierta la
permutacion r28/r29**: la fila 260 pasa a ser `lis r28, …@ha` como el objetivo, y
la 390 `cmpwi r29,0`. Pero sube de 8 a **11 filas**: al desaparecer el `mr.`
cambia el reparto entero y la actualizacion del contador se va del **tope** del
bucle (`subi r29,r9,1` en 0x2e6dc, con `mr r9,r29` abajo) al **cuerpo**
(`subi r5,r29,1` + `mr r29,r5`), con `copsToAssignOffsets` viviendo en un solo
registro. Ahi esta lo que falta: **romper la fusion sin mover la actualizacion
del biv**.

Barridas y medidas (28 s cada una), todas anotadas ya en la cabecera de la
funcion: `(x -= 1) > 0` y `0 < --x` dan **binario identico** a la base;
`__asm__("")`, `__asm__("" : "+r"(x))` y `__asm__("" : : "r"(x))` tras el
decremento dan **el mismo objeto que `x--`** (1.684 B / 11 filas: no aportan nada
sobre la forma de fuente limpia); `__asm__` antes del `--x` 99,050 % / 10 filas;
temporal explicito `int remaining` fuera del `do` 1.692 B / 21; el decremento al
principio del `do` 1.700 B / 21 —y ademas **incorrecto**, porque los `continue`
del cuerpo se lo saltarian—.

`regmap` dice **IDENTICO** (31 locales, mismo reparto), o sea que los dos pseudos
de la permutacion son temporales del compilador: **no hay nada que pinchar**.

### Y una prueba de que la FUENTE no es `--x > 0`

El bucle del objetivo, con direcciones: la arista de retorno (`bgt 0x2e6dc`)
apunta al **`subi r29,r9,1`**, y abajo estan `mr r9,r29` y `cmpwi r29,0`. O sea
que **el compare lee el TEMPORAL (r29), no la variable (r9, que es lo que dice el
DWARF)**. Y `expand_increment` (expr.c, rama final) hace:

```c
    temp = copy_rtx (value = op0);          /* op0 = la VARIABLE */
    op1 = expand_binop (mode, this_optab, value, op1, op0, ...);
    if (op1 != op0) emit_move_insn (op0, op1);
    return temp;                            /* == op0 */
```

es decir, **para un predecremento devuelve siempre la variable**. Con `--x` el
compare no puede leer el temporal: por eso combine lo alcanza y lo funde. La
fuente del original es otra construccion, y ninguna de las nueve que he probado
la reproduce.

---

## 5. `GRaceParameters::GenerateIndex` (1.680 B) — cuatro de sus cinco filas estan FUERA de mi territorio

Cinco filas: **cuatro** son la colocacion de dos `li r31,0` y **una** es
`ori r0,r30,0x200` contra `ori r0,r0,0x200`.

Lo nuevo: los `li r31,0` / `li r27,0` **no son de `GRaceDatabase.cpp`**. Son
`mantissa = 0` y `exponent = 0` del constructor inline de **`PackedDecimal`**
(`src/Speed/Indep/Src/Misc/PackedDecimal.h`), que en el objetivo se llama
`FloatingPoint<short int,10,3,5,11>::FloatingPoint(float)` y trae las locales
`neg` (r0), `man` (r31) y `exp` (r27) — **los mismos registros** que las nuestras.
O sea que solo cambia el orden de emision:

    objetivo  li r27,0 | lhz r0,0x8(r1) | li r31,0 | sth r0,0(r21)
    nuestro   li r27,0 | li r31,0       | lhz      | sth

`PackedDecimal.h` **no esta en mi lista**, asi que lo dejo escrito y no lo toco.
Y la quinta fila sola no cierra la funcion, asi que no he abierto ese frente.
`regmap` añade el dato que faltaba: al original le **sobran** nuestras dos
locales `pflags` y `flags` (r30); su volcado solo trae `topLeft` y `botRight`.

**Recomendacion para el reparto de la proxima ronda: `GenerateIndex` y
`PackedDecimal.h` tienen que ir al MISMO agente.**

---

## 6. Lo que dejo puesto

| fichero | que |
|---|---|
| `src/Speed/Indep/Src/Physics/Common/Smackable.cpp` | **la quinta pieza** (`__asm__("" : : "r"(rbbehavior))`) y el comentario reescrito: la receta de cinco piezas, por que hace falta cada una y quince formas negativas |
| `src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp` | solo comentario: PRE → cse2 → `update_equiv_regs`, con las cuentas por pasada y el barrido de banderas |
| `src/Speed/Indep/Src/AI/Common/AIPursuit.cpp` | solo comentario: `combine.c:1699`, la forma que acierta el tamaño y ocho negativos |
| `src/Speed/Indep/Src/Gameplay/GRaceDatabase.cpp` | solo comentario: las cuatro filas viven en `PackedDecimal.h` |

`SAP.h` queda **exactamente como estaba** (restaurada desde copia byte a byte; no
aparece en `git status`). Nada de `configure.py`, `config/GOWE69/*` ni
`splits.txt`. `lcfix.py --check`: «todas las entradas @lc estan al dia».
`pctsnap` sobre las cuatro unidades: **ninguna funcion empeora**. Temporales
borrados. Sin commits.
