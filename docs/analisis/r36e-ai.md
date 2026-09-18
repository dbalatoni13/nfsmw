# r36e-ai — zAI: la cola era una funcion, no cuatro, y esa una esta cerrada por `jump.c`

**Resultado: cero bytes ganados y el frente del `b`/`blt` CERRADO con prueba de
imposibilidad.** No dejo ni una linea tocada en el arbol.

Dos cosas que valen mas que un cierre pequeno:

1. **La cola de zAI eran 2.908 B en UNA funcion, no 12.048 en cuatro.** Lo medi
   antes de tocar nada; el brief se corrigio a media ronda y coincide.
2. **El frente `b`/`blt` de `SAP.h` --- 4.396 B en las DOS unicas funciones del
   juego que tienen esa forma --- no se puede abrir desde la fuente, y ahora hay
   una prueba de por que**: cualquier cosa que meta entre el test de salida y el
   salto de vuelta para bloquear `jump.c:1760` bloquea TAMBIEN el cross-jump de
   `jump.c:1991` que es lo que produce la forma del objetivo. Las dos condiciones
   estan en el mismo fichero y son incompatibles.

---

## 0. La cola real de zAI

`report.json` (medida oficial, `matched_code` todo-o-nada):

```
main/Speed/Indep/SourceLists/zAI
  total_code 272.796   matched_code 269.888   -> 2.908 B sin casar
  total_functions 1030 matched_functions 1029
```

Las tres funciones que el encargo daba por rotas estan al 100 %:

| funcion | B | `fndiff` | lo que veia `fncmp` |
|---|---|---|---|
| `ComputePotentials__12AIActionRace` | 3.256 | **100.0** | 1 insn |
| `Update__12AIActionRacef` | 3.924 | **100.0** | 7 insn |
| `GetPotentialSpeed__C12AIActionRaceffb` | 1.960 | **100.0** | 2 insn |
| `UpdateAllAvoidables__11AIAvoidablef` | 2.908 | 99,6011 | 14 insn |

La causa, con el caso de `ComputePotentials` desmontado al byte: **el objeto
extraido deja algunos `lis` SIN reubicar**, con el `@ha` ya resuelto en el
inmediato, y el nuestro los trae con reubicacion y el campo a cero.

```
+0C8   obj 3EA08004  (sin reubicacion)   |   nue 3EA00000  R_PPC_ADDR16_HA(_IHandle__10IVehicleAI)
+344   obj 3EA00000  R_PPC_ADDR16_HA(_IHandle__10IVehicleAI)  |  nue lo mismo
```

`_IHandle__10IVehicleAI` esta en `main.elf` en **0x80041E2C**, luego
`ha = 0x8004`: la instruccion del objetivo en +0C8 y la nuestra son la MISMA
despues de enlazar. Y la prueba de que es el extractor y no la fuente esta dos
lineas mas abajo: la instruccion identica de +344 SI trae la reubicacion.

### Cuanto inflaba en todo el proyecto

Repeti la comparacion de los 359 pares de objetos contando y sin contar esa
rama. **19 unidades afectadas, 72.856 B fantasma** --- mas que los 61.132 B que
de verdad le faltan al proyecto entero:

| unidad | fncmp (antes) | real | de mas |
|---|---|---|---|
| zEcstasy | 8 fn / 15.908 | 5 fn / 5.876 | 10.032 |
| **zFoundation** | 1 fn / 9.908 | **0 / 0** | 9.908 |
| zAI | 4 fn / 12.048 | 1 fn / 2.908 | 9.140 |
| zSpeech | 4 fn / 8.560 | 2 fn / 912 | 7.648 |
| **zPhysics** | 3 fn / 6.520 | **0 / 0** | 6.520 |
| zFe | 6 fn / 6.292 | 1 fn / 172 | 6.120 |
| realmemcard/trctask | 1 fn / 3.780 | 0 / 0 | 3.780 |
| zEAXSound | 28 fn / 5.368 | 3 fn / 1.748 | 3.620 |
| zWorld2 | 3 fn / 7.252 | 2 fn / 3.796 | 3.456 |
| ... y 10 mas | | | 3.9 k |

De los ~140 desajustes de «reubicacion en un lado solo», **111 son `addis`/`lis`
del lado del objetivo**. El resto son de `LibSN/steering`, donde si son
diferencias de verdad.

Esto **ya esta arreglado** en `scripts/fncmp.py` (`a26b2ecc`, aterrizo mientras
yo medía) y la version de ahora da 1 funcion / 2.908 B para zAI, al byte con
`report.json`. Lo dejo escrito por la magnitud: la ronda se planifico con cifras
que en 19 unidades estaban infladas, y **dos unidades enteras (zFoundation,
zPhysics) no tienen nada pendiente**.

---

## 1. `UpdateAllAvoidables` (2.908 B, 99,6011 %) — las 14 filas son TRES cosas

Base reproducida con la unidad recien compilada y con un banco truncado de
`zAI.cpp` (cabeza hasta la linea 33 + `IMPLEMENT_SAP_GRID(AIAvoidable)`, 19 s por
prueba): **14 filas exactas en los dos**.

| filas | que | donde |
|---|---|---|
| 6 | permutacion de tres r17/r18/r19 (`mAll`, `mRootX`, `mRootZ`) | `AvoidableManager.cpp` |
| 4 | `lwz r9,4(r9)` contra `lwz r9,4(r6)` | `SAP.h` (barrera de `head` de la r36c) |
| 4 | `b`→cabecera compartida contra `blt`→cuerpo | `SAP.h` |

Los cuatro bucles son las cuatro instancias inlineadas de
`SAP::Grid<T>::Axis::Node::Node` (el ctor de `Grid` construye 2 `Axis` y cada
`Axis` construye 2 `Node`). **`matched_code` es todo o nada: sin las tres, cero
bytes.**

El mapeo de la permutacion (por si la proxima ronda lo necesita):

```
mAll    objetivo r17   nuestro r19
mRootX  objetivo r19   nuestro r18
mRootZ  objetivo r18   nuestro r17
```

---

## 2. El frente `b`/`blt` — mecanismo con nombre, linea y prueba

### 2.1 Lo que hay que producir

Direcciones del objetivo (funcion en 0xb674):

```
   0xb7bc  lfs   f0,8(r9)
   0xb7cc  fcmpu cr0,f0,f13
LTOP:                            <- 0xb7d8, hay una etiqueta: algo salta aqui
   0xb7d8  cror  un,eq,gt
   0xb7dc  bso   0xb800
   0xb7e0  mr r6,r9 ; lwz r9,4(r9) ; cmpwi r9,0 ; beq 0xb800
   0xb7f0  lfs f13,8(r9) ; lfs f0,8(r3) ; fcmpu cr0,f13,f0
   0xb7fc  b     LTOP
0xb800:
```

Primero, un dato que ahorra tiempo a quien retome esto: **`cror un,eq,gt` +
`bso` NO son dos insns de RTL, son UNA**. Salen del modificador `%C` de
`rs6000.c:3047-3056`, que imprime el `cror` delante del salto solo para `LE`/`GE`
en `CCFPmode`. O sea que la cabecera del bucle del objetivo es **un unico
`jump_insn`: `(if_then_else (ge (reg:CCFP cr0) 0) (label_ref fin) (pc))`**,
compartido entre el precabezal (que cae en el) y el latch (que salta a el).

Eso es exactamente lo que deja `do_cross_jump`: fundir la cola comun de dos
bloques poniendo una etiqueta delante de la copia superviviente.

### 2.2 Lo que hacemos nosotros, y en que pasada

Volcados RTL de `cc1plus` (`-dj -dJ -dL -dr` sobre el `.ii`; `ngccc` no pasa las
`-d`) sobre un banco mini de 0,5 s por prueba. **Despues de la PRIMERA pasada de
jump ya esta hecho el destrozo**:

```
(insn 993) (set (reg:CCFP 486) (compare:CCFP (reg:SF 484) (reg:SF 485)))
(jump 994) (if_then_else (ge (reg:CCFP 486) 0) (label_ref 88) (pc))   <- copia del precabezal
(note 62)  NOTE_INSN_LOOP_BEG
(label 66) LBODY
   ... cuerpo, test de nulo ...
(insn 78)  (set (reg:CCFP 114) (compare:CCFP (reg:SF 112) (reg:SF 113)))
(jump 79)  (if_then_else (lt (reg:CCFP 114) 0) (label_ref 66) (pc))   <- YA INVERTIDO
(label 88) FIN
```

- los uids > 900 son de **`duplicate_loop_exit_test`** (`jump.c:2577`), que copia
  el test al precabezal remapeando los pseudos --- por eso el `fcmpu` de arriba
  usa `(f0,f13)` y el de abajo `(f13,f0)`;
- la insn 79 ya es `lt → LBODY`: la hizo **«Detect a conditional jump jumping
  over an unconditional jump», `jump.c:1760-1808`**, que invierte el `ge → FIN`
  y **borra el `b LBODY`**.

Sin ese `b LBODY` no hay nada que cross-jumpear: la pasada de jump que lleva
`cross_jump = 1` es la ultima (`toplev.c:4567`), y para entonces el latch ya es
un salto condicional.

**El cross-jump esta activo en este compilador.** Comprobado con un caso de 15
lineas: dos ramas con la cola `f(20); f(30); h(); return` se funden en una sola
copia. No es que la pasada no corra.

### 2.3 Por que no se puede desbloquear desde la fuente — la prueba

La guarda de `jump.c:1760` solo falla si entre el salto condicional del test y el
`b LBODY` hay **una insn activa o una etiqueta** (`prev_active_insn (reallabelprev)
== insn` y `no_labels_between_p`). Y las dos cosas que se pueden meter ahi rompen
el cross-jump que necesitamos despues:

- **`asm`** — `find_cross_jump` pone `lose = 1` si el patron de cualquiera de las
  dos insns es `ASM_INPUT` o un `ASM_OPERANDS` volatil (**`jump.c:2966-2970`**,
  con el comentario «Don't allow old-style asm or volatile extended asms to be
  accepted for cross jumping purposes»). Un `asm("")` pelado ES `ASM_INPUT`, y un
  `asm` con operandos es volatil en cuanto no tiene salidas. Cero insns casadas,
  no funde.
- **etiqueta** — `if (GET_CODE (i1) == CODE_LABEL) { --minimum; break; }`
  (**`jump.c:2893-2897`**) sale del bucle con `last1 == 0`, y el remate
  `if (minimum <= 0 && last1 != 0 ...)` no dispara. Tampoco funde.

Medido, no razonado: con `asm("")` dentro de la condicion y `-O2` (a `-O1` no se
puede, ver abajo) el latch se queda en `fcmpu; cror; bge .L1533; b .L1531` ---
**la forma previa exacta al cross-jump, 409 insns contra 401 de la base --- y el
cross-jump NO la funde**.

### 2.4 Y a `-O1` ni siquiera se puede escribir el `asm`

En C++ el `asm` solo entra en la condicion como expresion-sentencia `({...})`, y
eso abre un bloque lexico. **`duplicate_loop_exit_test` se rinde ante un
`NOTE_INSN_BLOCK_BEG`/`END` cuando `optimize < 2`** (`jump.c:2628-2637`), asi que
se pierde la duplicacion:

| forma | insns | que pasa |
|---|---|---|
| base | 401 | duplicado + `blt` |
| `asm` en la condicion, **-O1** | 393 | **sin duplicar** (-2 insns por bucle) |
| `asm` en la condicion, -O2 | 409 | duplicado, sin invertir, sin fundir |

**Esos 393 son la explicacion de la medida `W4_asmcond` de la r36d**
(1.456 B = 1.488 - 4 bucles x 8 B en `RBGrid::Add`): no era «el objetivo si tiene
la duplicacion» por una via misteriosa, era esta linea de `jump.c`. La conclusion
de la r36d era correcta; ahora tiene numero de linea.

La misma regla mata la forma con accesores (`GetPosition()`/`GetTail()`): las
inline abren bloque y la duplicacion se cae igual (393 insns).

### 2.5 El alcance exacto: 2 funciones en todo el juego, 4.396 B

Barri **todos** los objetos extraidos buscando la firma (un `b` hacia atras cuyo
destino es un `cror`):

```
funciones del ORIGINAL con el latch compartido: 2
  Speed/Indep/SourceLists/zAI.o             UpdateAllAvoidables__11AIAvoidablef   99,6011  (2.908 B)
  Speed/Indep/SourceLists/zPhysicsBehaviors.o  Add__6RBGridUiR9RigidBody...       99,30108 (1.488 B)
```

Las dos son el mismo bucle de `SAP::Grid<T>::Axis::Node::Node`, y las dos son las
unicas. En nuestro objeto de zAI la firma aparece **cero** veces. Son 4.396 B, el
**7,2 %** de los 61.132 B que le faltan al proyecto.

### 2.6 Lo medido y negativo

**31 conjuntos de flags** sobre el banco (0,5 s cada uno). Ninguno cambia el
latch: `-fno-thread-jumps`, `-fno-rerun-loop-opt`, `-fno-expensive-optimizations`,
`-fno-move-all-movables`, `-fno-force-mem`, `-fno-strength-reduce`,
`-fno-peephole`, `-fno-function-cse`, `-fno-defer-pop`,
`-fno-optimize-register-move`, `-fno-regmove`, `-funroll-loops`,
`-fno-caller-saves`, `-fno-cse-follow-jumps`, `-fno-gcse`,
`-fno-rerun-cse-after-loop`, `-fno-cse-skip-blocks`, `-fno-schedule-insns`,
`-fno-force-addr`, `-fno-schedule-insns2`, `-fno-omit-frame-pointer`,
`-fno-inline`, `-fno-delayed-branch`, `-fno-strict-aliasing`, `-fno-exceptions`,
`-O2`. (`-O0` si la cambia, y rompe todo lo demas.)

**19 formas de fuente** en cabecera SOMBRA (el arbol nunca se toco). Las que dan
**objeto identico a la base** --- util saberlo, son intercambiables:

| forma | insns |
|---|---|
| `for (; cond; head = head->mTail)` con el cuerpo vaciado | 401 = base |
| `while (head) { if (pos >= mPos) break; ... }` | 401 = base |
| `do-while` guardado por un `if` | 401 = base |
| `while (head && ...)` sin `!= nullptr` | 401 = base |
| `!(mPosition >= ...)` | 401 = base |
| parentesis en cada operando | 401 = base |
| `&& head != nullptr` repetido al final (fold se lo come) | 401 = base |
| `&& (dummy = 1)` / `(dummy = 1) != 0` / `(dummy = 1, 1)` | 401 = base |

Y las que lo cambian, todas a peor:

| forma | insns | que rompe |
|---|---|---|
| `asm` en la condicion (-O1) | 393 | pierde la duplicacion (bloque lexico) |
| accesores `GetPosition()`/`GetTail()` | 393 | idem (la inline abre bloque) |
| `goto` explicito | 390 | ni duplica ni rota |
| `mPosition` en una local | 397 | misma forma, menos codigo |
| temporal `next` en el cuerpo | 408 | misma forma, mas codigo |
| condicion al reves (`< ` antes del nulo) | 404 | invierte el test entero |
| `&& this->mRoot != nullptr` redundante | 418 | bloquea 1760, no funde, y cuesta |
| `&& this != nullptr` redundante | 421 | idem |
| condicion `<` repetida | 417 | idem |

### 2.7 Un positivo de paso: `-ffast-math` esta bien

Hipotesis natural: el `cror` del precabezal sale de `can_reverse_comparison_p`
(`jump.c`), que con `flag_fast_math` devuelve 1 **siempre** y por eso GCC puede
darle la vuelta al `lt` y dejar un `ge`; sin el, se queda en `lt` y usa la forma
de salto invertido, que en PowerPC no lleva `cror`.

Medido sobre la unidad entera:

- **con `-ffast-math`** (lo actual): precabezal `cror un,eq,gt` + `bso`,
  **igual que el objetivo**;
- **`-fno-fast-math`**: precabezal `bge` pelado, sin `cror` --- **distinto del
  objetivo** --- y ademas **158 funciones y 138.596 B peores** en zAI, con
  `UpdateAllAvoidables` en 2.892 B (16 B de menos).

O sea que **el `cror` del binario original es prueba de que EA compilo esta
unidad con `-ffast-math`**. La bandera se queda como esta y la hipotesis queda
cerrada con cifra.

---

## 3. La permutacion r17/r18/r19 — la palanca de `n_refs` no llega

El encargo pedia explicitamente pasar por aqui el operando de ENTRADA que cerro
`AssignClosestOffsets` en la r36d. **No llega, y la razon es de alcance.**

Los tres pseudos en disputa guardan el `high(@ha)` de tres simbolos
(`_11AIAvoidable.mAll`, `mRootX`, `mRootZ`) y los crea `-fforce-addr`. No son
variables de C: `&mRootX` en fuente produce la direccion COMPLETA (`lis`+`addi`),
que es otro pseudo. Lo unico que toca el pseudo del `@ha` sin emitir un byte es un
operando **`"m"`**, porque la referencia va por `lo_sum(pseudo, simbolo)`.

Medido en el banco (19 s por prueba), todo sobre `AvoidableManager.cpp` en
sombra:

| variante | filas | tamano |
|---|---|---|
| base | 14 | 2.908 |
| `__asm__("" : : "m"(Grid::mRootX));` tras el `new Grid` | **30** | 2.908 |
| `__asm__("" : : "m"(Grid::mRootZ));` tras el `new Grid` | **30** | 2.908 |
| `... "m"(mRootX), "m"(mRootZ)` tras el `new Grid` | **28** | 2.908 |
| lo mismo al principio de la funcion | — | **2.916** (+8 B) |
| lo mismo al final | — | **2.916** (+8 B) |
| `"m"(mAll)` al final (diagnostico) | — | **2.912** (+4 B) |

Las dos reglas que salen:

- **dentro del bucle el `"m"` es de cero bytes** (el pseudo ya esta vivo ahi) pero
  mete al asignador tres pseudos nuevos y el reparto empeora de 14 a 28-30 filas;
- **fuera del bucle cuesta 4-8 B**, porque hay que materializar el `@ha` en un
  sitio donde no estaba.

No insisti mas porque **aunque las 6 filas cayeran la funcion no cerraria**: le
quedarian las 4 del `lwz` y las 4 del `b`/`blt`, y `matched_code` es todo o nada.
Dejar media funcion arreglada solo aporta riesgo (regla del proyecto: media
funcion no es neutra).

---

## 4. Lo que dejo puesto

**Nada.** Ni una linea del arbol tocada:

- `git status --short src/` no lista **ningun** fichero de
  `src/Speed/Indep/Src/AI/**`, ni `SAP.h`, ni `AvoidableManager.cpp` (los cuatro
  ficheros modificados que aparecen son de otros agentes: `JoyE.cpp`,
  `TrackCop.cpp`, `NFSMixMapState.cpp`, `stream.cpp`);
- todo el barrido fue con **cabecera sombra** (`-I <dir>` delante de los de
  `build.ninja`), como la r36d;
- nada de `configure.py`, `config/GOWE69/*` ni `splits.txt`;
- tampoco toque `scripts/fncmp.py`: cambio en disco a media sesion y el
  mantenedor ya metio la correccion.

### Verificacion

- `python scripts/build_direct.py zAI` -> ok;
- `python scripts/fncmp.py Speed/Indep/SourceLists/zAI` -> **1 de 1030 funciones
  con el codigo distinto, 2.908 B**, identico al informe oficial y a la base;
  **ninguna funcion empeora** (no hay cambios que puedan empeorarla);
- `python scripts/fndiff.py zAI UpdateAllAvoidables__11AIAvoidablef` -> 99,6011 %
  (la base), y las otras tres a **100.0 %**;
- `python scripts/lcfix.py --check` -> «todas las entradas @lc estan al dia»;
- sin commits.

---

## 5. Propuestas fuera de mi territorio

1. **`src/Speed/Indep/Libs/Support/Miscellaneous/SAP.h` — no gastar mas rondas en
   la forma del bucle.** Van dos rondas (r36c la barrera de `head`, r36d 22
   banderas y 6 formas, esta 31 banderas y 19 formas). El frente no es de fuente:
   es la incompatibilidad de `jump.c:1760` con `jump.c:1991`. Quien lo retome
   tiene que empezar por explicar **como sobrevive el `b LBODY` a la primera
   pasada de jump sin meter una insn ni una etiqueta en medio**, que es lo unico
   que no he sabido responder. Todo lo demas esta acotado con numero de linea.
2. **`scripts/fncmp.py`**: la correccion de `a26b2ecc` es la buena; anoto aparte
   la magnitud (72.856 B fantasma en 19 unidades) por si conviene revisar los
   encargos de esta ronda: **zFoundation y zPhysics no tienen codigo pendiente**
   (0 B las dos) y aparecian con 9.908 y 6.520 B.
3. Si alguien quiere los 4.396 B: la unica hipotesis viva es que el original NO
   paso por esta pasada de `jump` con esta guarda --- otra revision del compilador
   para esa cabecera, o una `#pragma`/flag por fichero. Se descarta ya
   `-ffast-math` (probado, y el `cror` del binario demuestra que SI estaba).

---

## 6. Herramientas que dejo en el scratchpad (no en el arbol)

| fichero | que |
|---|---|
| `probe.py` | compila una unidad con los cflags reales + extras y la compara instruccion a instruccion contra el extraido |
| `mini.py` / `sweep.py` | banco de 0,5 s: mini-TU que instancia `SAP::Grid<Dummy>` y saca la forma de los 4 bucles |
| `var.py` | variantes del `while` de `SAP.h` en cabecera sombra |
| `p2.py` / `am.py` | banco de 19 s: `zAI.cpp` truncado tras `AvoidableManager.cpp`, y variantes de ese fichero en sombra |
| `od.py` | `fndiff` contra un objeto arbitrario |
