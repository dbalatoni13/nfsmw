# r36d-ai — zAI, zGameplay, zPhysicsBehaviors (8.616 B repartidos)

**Resultado: `AIPursuit::AssignClosestOffsets` CERRADA (1.684 B, 99,66746 → 100 %).**
Verificado sobre el build real de `zAI`: `pctsnap --cmp` da **+0,332 pp en esa
funcion y NINGUNA empeora**; `lcfix.py --check` limpio; 1.365 de 1.376 simbolos
de la unidad al 100 % (los 11 que no son los mismos datos de siempre:
`[.rodata-0]`, `[.data-0]`, `base_pos.31068`, los siete `*goals`, y
`UpdateAllAvoidables`).

Lo que la cerro es una **palanca nueva** que no estaba en el brief y que sirve
para cualquier permutacion de registros: **el operando de ENTRADA de un `asm`
sube `n_refs` del pseudo y con eso se voltea `allocno_compare`**. Es el eje «a
quien» de la palanca 2, y esta es la primera vez que se usa con exito.

Las otras cuatro no cierran. Dos de ellas quedan **cerradas como frente**: la
causa esta identificada con nombre de pasada, numero de linea y una **prueba
aritmetica** de por que las formas alternativas no pueden ser.

Base verificada antes de tocar nada: las cinco reproducen su cifra exacta
(`triaje.py` sobre las tres unidades recien compiladas).

---

## 0. Bancos

Tres bancos de mini-TU con **cabecera SOMBRA** (`scripts/_r36d_sweep.py`): se
escribe la variante en un directorio aparte y se compila con ese `-I` DELANTE de
los de `build.ninja`, asi que **el arbol real no se toca en ningun momento** y no
hay que restaurar nada. Tiempos por prueba:

| banco | fuente | simbolo | tiempo |
|---|---|---|---|
| `sw_sap.py` | `RigidBody.cpp` suelto | `RBGrid::Add` | 10 s |
| `sw_gp.py` | `zGameplay.cpp` truncado tras la linea 29 | `GRaceParameters::GenerateIndex` | 27 s |
| `sw_ap.py` | `zAI.cpp` truncado tras la linea 89 | `AIPursuit::AssignClosestOffsets` | 30-65 s |

Y volcados RTL de `cc1plus` con `-dL -dc -dt -dl` sobre el `.ii` preprocesado
(`scratchpad/r36d_rtl*.py`, 8-110 s), mas `scratchpad/r36d/ext.py`, que saca de
un volcado las insns de UNA funcion en una linea cada una. **`ngccc` no pasa las
`-d`**: hay que llamar a `cc1plus` a mano, y los volcados salen en el directorio
de trabajo como `<tag>.ii.<pasada>`.

---

## 1. `AIPursuit::AssignClosestOffsets` — 100 %

Las ocho filas eran **dos cosas** y hacian falta **dos piezas**, una para cada
una. Ninguna sirve sola.

### Pieza 1 — el temporal explicito rompe `combine.c:1699`

Leido del volcado: despues de `cse2` el fondo del bucle es

```
(insn 2102) (set (reg/v:SI 418) (reg:SI 658))                       ; mr  x, t
(insn 2103) (set (reg:CC 580) (compare:CC (reg/v:SI 418) (const_int 0)))  ; cmpwi x,0
```

y en `combine` se convierte en

```
(insn 2103) (parallel[ (set (reg:CC 580) (compare:CC (reg:SI 658) (const_int 0)))
                       (set (reg/v:SI 418) (reg:SI 658)) ])          ; mr. x,t
```

La guarda que lo permite es la ultima de las cuatro de `combine.c:1699`:

```c
      && rtx_equal_p (XEXP (SET_SRC (PATTERN (i3)), 0), i2dest)
```

o sea: **el compare tiene que leer i2dest**, que con `--x > 0` es la variable
(`expand_increment` devuelve siempre `op0` para un predecremento). Haciendo que
el compare lea el TEMPORAL la guarda falla y quedan las dos insns sueltas, que es
lo que tiene el objetivo. En C eso es una expresion-sentencia, porque una local
declarada dentro del `do` no se ve en el `while`.

### Pieza 2 — la barrera de DOS operandos

El temporal solo no basta: `cse` lo vuelve a fundir con la variable y regresa el
`mr.` (**medido: 1.680 B / 8 filas, identico a la base**). Hace falta

```c
__asm__("" : "+r"(copsToAssignOffsets) : "r"(next));
```

- el **`"+r"` sobre la variable** impide esa refusion — con el solo ya salen
  `mr r9,rX` + `cmpwi rX,0` y el tamaño exacto (1.684 B), pero con la
  permutacion r28/r29 todavia al reves;
- el **operando de ENTRADA `"r"(next)`** es el que voltea el reparto. Sube
  `n_refs` del pseudo del contador y `allocno_compare` lo pone por delante del
  pseudo `@ha` de `-1.0f`; como en `REG_ALLOC_ORDER` de rs6000 r29 va antes que
  r28, el que se asigna primero coge r29. Con eso el contador se lleva r29 y el
  `@ha` r28, **igual que el objetivo, y caen las seis filas de la permutacion**.

**Tiene que ser ENTRADA y no `"+r"`**: con `"+r"(next)` la salida del asm no se
puede quedar en r29 y aparece un `mr r0,r29` de mas.

### La escalera completa, medida

| forma | % | tamaño | filas |
|---|---|---|---|
| base `--x > 0` | 99,66746 | 1.680 | 8 |
| temporal solo, sin asm | 99,66746 | 1.680 | 8 |
| barrera sobre una copia de `--x` | 99,66746 | 1.688 | 9 |
| `"+r"(next)` sola | 99,66746 | 1.688 | 9 |
| dos `asm` separados (`next`, luego `x`) | 99,65558 | 1.688 | 9 |
| **`"+r"(x)` sola** | **99,90499** | **1.684** | **8** (solo la permutacion) |
| `"+r"(x), "+r"(next)` | 99,75060 | 1.688 | 2 |
| `"+r"(next), "+r"(x)` | 99,73872 | 1.688 | 3 |
| **`"+r"(x) : "r"(next)`** | **100** | **1.684** | **0** |
| lo mismo con el input repetido dos veces | 100 | 1.684 | 0 |

Puesto en `AIPursuit.cpp` con 22 lineas de comentario que explican las dos
piezas y dejan escritos los seis negativos.

### La regla que sale de aqui

**Para una permutacion limpia entre dos pseudos del compilador —los que
`regmap` no ve y por eso siempre se daban por intocables— el mando es
`n_refs`, y se sube con un operando de ENTRADA de un `asm`, que no emite ni un
byte.** Es exactamente lo que el brief de la r36c señalaba como no explorado en
seis rondas. Candidata inmediata: las seis filas r17/r18/r19 de
`UpdateAllAvoidables`.

---

## 2. `RBGrid::Add` (1.488 B) y 8 de las 14 filas de `UpdateAllAvoidables` — frente CERRADO

La barrera de `head` de la r36c se reproduce exacta en el banco (8 → **4** filas,
99,30108 → 99,35484 %, sin mover un byte). Lo que queda son cuatro filas, una por
instancia del bucle, y **son la misma**:

```
objetivo   fcmpu | b 0x7d8        (arista de retorno INCONDICIONAL al cror)
nuestro    fcmpu | blt →cuerpo    (arista de retorno CONDICIONAL)
```

### La pasada, con nombre y numero

Del volcado `.jump` de `RigidBody.cpp`, el bucle de `SAP::Grid<T>::Axis::Node::Node`
justo despues de `jump_optimize`:

```
1294: cc489 = compare(head,0)          <- uid > 1290: insns NUEVAS
1295: if (cc489==0) goto 126
1296-1300: ...  ccfp494 = compare(head->mPos, this->mPos)
1301: if (ccfp494 >= 0) goto 126
  98: NOTE_INSN_LOOP_BEG
 102: L12626:   104,107  (node=head; head=head->mTail)
 108: NOTE_INSN_LOOP_CONT
1304: NOTE_INSN_LOOP_VTOP             <- la firma de duplicate_loop_exit_test
 110,111: if (head==0) goto 126
 112-116: ccfp121 = compare(...)
 117: if (ccfp121 < 0) goto 102       <- el `blt`
```

Los uids 1294-1301 y el `NOTE_INSN_LOOP_VTOP` los pone
**`duplicate_loop_exit_test`, `jump.c:2577`** (copia el test de salida delante del
bucle y borra el salto incondicional de la cabecera). El `blt` de la insn 117 lo
hace despues **`jump.c:1779`**, «Detect a conditional jump jumping over an
unconditional jump»: invierte el condicional y borra el `jump 102`.

### Por que el objetivo NO es «sin duplicacion» — la prueba aritmetica

`duplicate_loop_exit_test` se apaga solo con tres cosas, y las tres estan en su
propio comentario: un `CODE_LABEL`/`CALL_INSN` en el codigo de salida, un
`NOTE_INSN_BLOCK_*` **con `optimize < 2`** (compilamos a `-O1`: vale), o
`asm_noperands (PATTERN (insn)) > 0`. Probado con un `asm` de dos operandos
metido en la condicion (expresion-sentencia):

    W4_asmcond   86,41 %   **1.456 B**   = 1.488 - 4 bucles x 8 B

**El objetivo mide 1.488 B, o sea que la duplicacion SI ocurrio en el
objetivo.** Con eso queda descartado todo el frente de «bloquear la duplicacion»,
que era la lectura natural.

Y por el mismo lado: si lo que fallara fuese la inversion de `jump.c:1779`, el
fondo del bucle quedaria en `cror;bge` + `b` = **tres** insns, y el objetivo tiene
**una**. Contando insns, el fondo del bucle del objetivo **no tiene ningun salto
condicional**: su unico test de `<` es la cabecera compartida, con el `fcmpu`
replicado en el precabezal y en el latch. Ese RTL no lo produce ninguna
combinacion de `expand_end_loop` + `duplicate_loop_exit_test` + `jump_optimize`.

### Lo barrido en esta ronda (todo negativo)

**22 banderas** sobre el banco, con la barrera de `head` puesta. Dejan las 4 filas
intactas: `-fno-thread-jumps`, `-fno-rerun-loop-opt`, `-fno-expensive-optimizations`,
`-fno-move-all-movables`, `-fno-force-mem`, `-fno-strength-reduce`, `-fno-peephole`,
`-fno-function-cse`, `-fno-defer-pop`, `-fno-optimize-register-move`, `-fno-regmove`,
`-funroll-loops`, `-fno-caller-saves`. Y rompen el resto:
`-fno-cse-follow-jumps` (81,41 %), `-fno-gcse` (83,01 %), `-fno-rerun-cse-after-loop`
(85,74 %), `-fno-cse-skip-blocks` (86,23 %), `-fno-schedule-insns` (89,79 %),
`-fno-force-addr` (91,93 %), `-fno-schedule-insns2` (96,56 %),
`-fno-omit-frame-pointer` (98,45 %), `-fno-inline` (4,38 %).

**Seis formas de fuente**, todas peores, y las tres primeras explicadas por la
PRIMERA transformacion de `expand_end_loop` (`stmt.c:2110`), que se dispara cuando
la ultima sentencia del cuerpo es un `break` y deja el `lfs|lfs|fcmpu|b` fuera:

| forma | % | tamaño |
|---|---|---|
| `for(;;)` con el test `<` primero y `break` por nulo | 88,33 | 1.440 |
| `goto` con el test arriba y `goto test` abajo | 87,73 | 1.440 |
| `do-while` guardado por un `if` | *(mismo objeto que la base)* | 1.488 |
| variable `bool go` recalculada al final | 81,25 | 1.552 |
| barrera tambien al principio del cuerpo | *(= base)* | 1.488 |
| `asm` de dos operandos en la condicion | 86,41 | 1.456 |

La barrera de `head` **NO se deja puesta** (mismo criterio que la r36c: ninguna de
las dos funciones cierra y `matched_code` es todo-o-nada). `SAP.h` no aparece en
`git status`.

**Aviso de manipulacion**: `SAP.h` tiene las lineas 26-29 —justo el bloque del
`while`— con **LF suelto dentro de un fichero CRLF**. Cualquier parche que
normalice rompe. El banco de sombra evita el problema entero.

---

## 3. `GRaceParameters::GenerateIndex` (1.680 B) — sigue en 5 filas

Con `PackedDecimal.h` ya en territorio, el frente de las cuatro filas del
`li r31,0` **queda descartado como problema de fuente**.

Lo primero, un hallazgo estructural del volcado DWARF que conviene anotar: el
tipo del original **no es una clase con un `unsigned short mPacked`** sino

```cpp
struct FloatingPoint<short int,10,3,5,11> { short mExp : 5; short mMan : 11; };
typedef FloatingPoint<short int,10,3,5,11> decimal16;   // y decimal8 con <signed char,10,1,3,5>
```

es decir **campos de bits**. El empaquetado a mano que tenemos
(`((exponent & 0x1F) << 11) | (mantissa & 0x7FF)`) da el mismo codigo
(`clrlslwi | clrlwi | or | sth`), asi que no es la causa de nada, pero si alguien
reescribe `PackedDecimal` que sea hacia esa forma.

El bloque basico donde caen las filas va de `0xc630` hasta **la llamada a
`GetCashValue` incluida**: los dos `li` son el prologo del ctor SIGUIENTE (r27 y
r31 son preservados, por eso el planificador los sube por delante de la llamada) y
el par `lhz`/`sth` es la copia del temporal de 2 B del ctor ANTERIOR. El objetivo
intercala `li r27 | lhz | li r31 | sth` —que tapa la latencia de la carga— y
nosotros `li r27 | li r31 | lhz | sth`. Es un desempate de `rank_for_schedule`.

Seis formas medidas, **ninguna lo mueve**:

| variante en `PackedDecimal.h` | % | filas |
|---|---|---|
| `mantissa` antes de `exponent` | 98,857 | 17 |
| `negative, mantissa, exponent` | 98,857 | 17 |
| `negative` entre los dos enteros | **identico a la base** | 5 |
| `mantissa` declarada tras el primer `if` | 97,829 | 42 |
| `__asm__("" : "+r"(mantissa))` | 98,714 | 10 |
| `__asm__("" : "+r"(exponent))` | 97,524 | 93 |

Las dos primeras dan el dato util: **el primer entero declarado se lleva r27 y el
segundo r31**, o sea que el orden actual (`exponent, mantissa`) ya es el del
objetivo y no hay nada que ganar por ahi. Y la barrera **adelanta** el `li r31`
(hasta el prologo de la funcion), justo lo contrario de lo que hace falta.

La quinta fila (el `ori`) suma tres negativos nuevos, todos por la misma razon:
**`flags` MUERE en la linea siguiente** (`flags = index->mFlags;` la recarga de
memoria), asi que GCC coalesce la copia y el `mr r0,r30` desaparece del todo
(1.676 B, 7 filas). **La barrera de dos operandos que cerro `AssignClosestOffsets`
no vale aqui precisamente por eso: su segundo operando esta muerto.**

Todo anotado en la cabecera de la funcion en `GRaceDatabase.cpp`.

---

## 4. `SuspensionTraffic::Tire::UpdateLoaded` (856 B) — la cuarta palanca no llega

El encargo era probar la barrera selectiva en su cuarto uso (impedir un plegado
de CSE). **No es aplicable, y la razon es de fondo, no de forma.**

Lo que hay que mantener vivo no es ninguna variable de la fuente: es **el pseudo
del compilador que guarda `high(*$LC917)`** (el `@ha` del `1.0f`), y a un pseudo
del compilador no se le puede colgar un `asm`. Lo unico nombrable desde C es el
FLOAT (`const float one = 1.0f` con `"+f"`), y eso lo dejaria en un FPR preservado
con **un** `lfs`; el objetivo hace `lfs f0, ...@l(r30)` **dos** veces desde un GPR
preservado. Es otra forma, no la del objetivo.

La condicion exacta que habria que romper esta escrita ahora en la cabecera de la
funcion: `update_equiv_regs` (local-alloc.c) solo hunde el pseudo **si le queda UN
uso**. Si a `cse2` se le escapasen DOS de los cinco plegados que deshace, el
pseudo no se hundiria y `global_alloc` le daria un preservado — que es el
`lis r30` de la fila 86, del que cuelgan las 21 filas (el segundo GPR salvado, el
`stmw r30` contra `stw r31`, el marco 0x30 contra 0x28 y los 4 B).

Frente abierto: hace falta una construccion que llegue a `cse2`, y no la hay en
esta funcion.

---

## 5. Lo que dejo puesto

| fichero | que |
|---|---|
| `src/Speed/Indep/Src/AI/Common/AIPursuit.cpp` | **la funcion cerrada**: el temporal explicito, la barrera de dos operandos, y 22 lineas con el mecanismo (`combine.c:1699`, `allocno_compare`) y los seis negativos |
| `src/Speed/Indep/Src/Gameplay/GRaceDatabase.cpp` | solo comentario: el barrido de `PackedDecimal.h`, el bloque basico exacto y los tres negativos nuevos del `ori` |
| `src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp` | solo comentario: por que la cuarta palanca no llega y cual es la condicion de `update_equiv_regs` |

`SAP.h` y `PackedDecimal.h` **no se han tocado** (todo el barrido fue por
directorio sombra; no aparecen en `git status`). Nada de `configure.py`,
`config/GOWE69/*` ni `splits.txt`. `lcfix.py --check`: «todas las entradas @lc
estan al dia». `pctsnap --cmp` sobre las tres unidades: **ninguna funcion
empeora**; la unica que se mueve es `AssignClosestOffsets`, +0,332 pp. Sin
commits.

## 6. Para la proxima ronda

1. **Pasar el operando de entrada por todas las permutaciones abiertas del
   proyecto.** Es barato (no emite bytes) y es el unico mando conocido sobre
   `allocno_compare`. La primera candidata es la permutacion de tres
   (r17/r18/r19) de `UpdateAllAvoidables`: con la barrera de `head` de `SAP.h`
   puesta se queda en 10 filas, de las que **6 son esa permutacion**; si caen,
   la funcion baja a las 4 del `b`/`blt`.
2. `RBGrid::Add` y las 4 filas de `b`/`blt`: **no es fuente y no es bandera**.
   Cualquier intento nuevo tiene que empezar explicando de que RTL sale una
   cabecera de bucle que solo contiene el salto condicional, con el `fcmpu`
   replicado en sus dos predecesores.
3. `GenerateIndex` y `UpdateLoaded` piden lo mismo desde dos sitios distintos:
   un mando sobre `rank_for_schedule` que **atrase** una insn (hoy solo sabemos
   adelantarla) y un mando sobre `cse2`.
