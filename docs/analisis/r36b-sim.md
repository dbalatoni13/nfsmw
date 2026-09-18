# r36b-sim — zAI, zGameplay, zPhysics, zPhysicsBehaviors (14.280 B repartidos)

**Resultado: `GTrigger::GTrigger` CERRADA (2.544 B, 99,583 → 100 %).**
`zGameplay` pasa de **137.232 a 139.776 B** casados (765 → 766 funciones).
Las otras seis quedan diagnosticadas con precisión de pasada del compilador; tres
de ellas resultan ser **el mismo muro**, y una de esas tres no era la que decía el
encargo.

Base verificada antes de tocar nada: las siete reproducen su cifra exacta
(`triaje.py` y `m.py` contra el último commit).

---

## 0. La herramienta que hizo posible todo lo demás: la mini-TU

Un `.cpp` de UNA línea que incluye el `.cpp` de la clase compila con los cflags
de la SourceList y **reproduce la función byte a byte**:

    echo '#include "Speed/Indep/Src/Gameplay/GTrigger.cpp"' > mini.cpp

`GTrigger::GTrigger` sale 99,583336 % / 2.544 B / **las mismas 4 filas** que en
`zGameplay`. Coste: **11 s contra 29 s**, y —lo que de verdad importa— permite
lanzar `cc1plus` a mano sobre el `.ii` con `-dS -dR -fsched-verbose-5`, cosa que
`ngccc` no deja hacer. Lo mismo con `RigidBody.cpp` (18 s) para `RBGrid::Add`.

**Esto convierte «barrer formas de fuente» en «leer la decisión del compilador».**
Recomiendo dejarlo como herramienta del proyecto.

---

## 1. `GTrigger::GTrigger` — 100 %. La **barrera de RANURA**

### El diagnóstico, leído del volcado

Las cuatro filas eran una sola cosa. Alrededor de `UMath::MultYRot(boxmat,
-Rotation()/360.0f, boxmat)`:

    objetivo:  lfs f1 | lis | lfs f0 | fneg | mr r3 | fmuls | mr r4 | bl | li r22
    nuestro:   lfs f1 | lis | lfs f0 | mr r3 | fneg | mr r4 | fmuls | li r22 | bl

`-fsched-verbose-5` da el reloj exacto. **El bloque emite DOS instrucciones por
ciclo** (`issue_rate` = 2, visible en la «scheduling visualization»):

    nuestro   c1{lfs f1, lis}  c2{lfs f0, mr r3}  c3{fneg, mr r4}  c4{fmuls, li r22}  c5{bl}
    objetivo  c1{lfs f1, lis}  c2{lfs f0}         c3{fneg, mr r3}  c4{fmuls, mr r4}   c5{bl}  c6{li r22}

**El objetivo deja VACÍA la segunda ranura del ciclo 2 y por eso todo lo demás se
corre una ranura.** No es reparto de registros ni una local: es una ranura de
emisión de más.

### La palanca

Hace falta un insn de **cero bytes** que gane la ranura del ciclo 2. La tabla de
dependencias del volcado da los números:

    insn  código  dep  prio  coste  unidad
    2153  (lfs f0)  1    27     2    lsu     <- primera elección del ciclo 2
    2165  (mr r3)   0    25     1    iu2     <- la que sobra
    2167  (mr r4)   0    25     1    iu2
    2192  (li r22)  0    22     1    iu2

`rank_for_schedule` compara, **en este orden**: prioridad, `INSN_REG_WEIGHT`
(sólo en `sched1`), clase respecto al último emitido, **número de dependientes**
y por último `INSN_LUID`. Un `asm("" : "+m"(boxmat))` a secas empata en prioridad
(25) y en peso, pero **sólo tiene DOS dependientes** (la llamada y el fin de
bloque) contra los **TRES** de `mr r3`, y pierde ahí — antes de llegar al orden.

**La solución es darle el tercer dependiente con un `clobber`:**

```c
float rot = -Rotation() / 360.0f;
asm("" : "+m"(boxmat) : : "r0");
UMath::MultYRot(boxmat, rot, boxmat);
```

`r0` es volátil y está libre en ese punto: el `clobber` **no cuesta un byte** y le
añade la dependencia de salida contra el `lwz r0, 0x38(r31)` posterior. Con eso
el `asm` gana la ranura, `fneg` y `mr r3` se corren al ciclo 3, `fmuls` y `mr r4`
al 4, y `li r22` cae detrás de la llamada. **4 filas → 0.**

El temporal `rot` **no es decorativo**: sin él el `asm` cae en el bloque
ANTERIOR (la llamada a `Rotation()` parte el bloque básico) y el objeto sale
**idéntico** — medido.

### Barrido negativo del camino (todo medido, para no repetirlo)

| forma | resultado |
|---|---|
| `asm("" : "+m"(boxmat))` **antes** de `Rotation()` | idéntico (cae en el bloque anterior) |
| `asm("" : "+m"(boxmat))` sin `clobber` | 99,678 % (3 filas): coloca bien `li r22`, no `mr r3` |
| `asm("" : "+r"(bm), "+m"(*bm))` con puntero | 99,678 %: `regmove` convierte `mr r4,r27` en `mr r4,r3` |
| lo anterior + mantener el puntero vivo tras la llamada | **94,72 %, +4 B** (se lleva un registro salvado) |

**La regla nueva**: cuando el diff es *«todo corrido una ranura»* y el volcado
enseña un ciclo con una sola instrucción en el objetivo, la palanca es un `asm`
extendido **con `clobber` de un registro volátil libre**, no la barrera de
`"+f"`/`"+r"` de la r36. Son dos palancas distintas: aquella ataba un VALOR
(planificador contra asignador), ésta **ocupa una RANURA**.

---

## 2. Tres funciones, UN solo muro: la rotación del bucle de `SAP.h`

`RBGrid::Add` (1.488 B, 8 difs) y **8 de las 14 difs de
`AIAvoidable::UpdateAllAvoidables`** (2.908 B) son **la misma línea de código**:
el bucle de inserción del constructor `SAP::Grid<T>::Axis::Node::Node`
(`SAP.h:25`), instanciado cuatro veces en cada una (min/max × X/Z).

El encargo decía que en `UpdateAllAvoidables` los cuatro `b`/`blt` venían de
`Sweep`. **No: vienen del constructor `Node`**, y por eso el mismo diff aparece
clavado en `RBGrid::Add`, que no llama a `Sweep`.

### Qué difiere, exactamente

    objetivo   ... fcmpu | cror | bso out | mr r6,r9 | lwz r9,0x4(r9) | cmpwi | beq out | lfs | lfs | fcmpu | b →cror
    nuestro    ... fcmpu | cror | bso out | mr r6,r9 | lwz r9,0x4(r6) | cmpwi | beq out | lfs | lfs | fcmpu | blt →mr

Mismo tamaño (1.488 B los dos) y misma cuenta de instrucciones. Dos diferencias
por bucle: (a) el `lwz` lee `r9` (head) en el objetivo y la copia `r6` (node) en
el nuestro; (b) **la arista de retorno**: el objetivo salta incondicionalmente al
`cror` compartido y nosotros usamos un `blt` propio a la primera del cuerpo.

### El mecanismo, leído del RTL (`-dr -dj -dL`)

`jump.c:2577 duplicate_loop_exit_test` **sí dispara** en el nuestro: el volcado
`.jump` enseña los insns copiados (1294-1301) **delante** de `NOTE_INSN_LOOP_BEG`
y el `NOTE_INSN_LOOP_VTOP` que la función inserta, con la prueba del final ya
invertida a `(ge ...)` → `cror; bso`. El objetivo tiene esa copia también (se ve
en el `reg_map`: los operandos del `fcmpu` de entrada y del de dentro están
**intercambiados**, que es la firma de los pseudos nuevos que crea esa función).
Lo único que cambia es que **el salto propio del bucle del objetivo es
incondicional**, cosa que ese RTL no puede producir.

Y el fuente del compilador está en el árbol
(`orig/prodg/NGC_GNU_SRC/NGC/gcc/jump.c`): la lista de motivos por los que
`duplicate_loop_exit_test` aborta es `CALL_INSN`, `CODE_LABEL`,
`NOTE_INSN_LOOP_BEG/CONT`, **`NOTE_INSN_BLOCK_BEG/END` cuando `optimize < 2`**
(compilamos a `-O1`), más de 20 insns, `REG_RETVAL/REG_LIBCALL` y
**`asm_noperands(PATTERN(insn)) > 0`**. Cualquiera de esos corta la duplicación —
pero **no es eso lo que hay que cortar**: cortarla nos aleja.

### Vedas medidas (banco de 18 s sobre `RBGrid::Add`; base 99,30108 %, 8 difs)

| forma / bandera | resultado |
|---|---|
| `for (head = mRoot; head && head->mPosition < mPosition; head = head->mTail)` | **binario idéntico** a la base |
| `head = node->mTail;` en vez de `head = head->mTail;` | **binario idéntico** |
| `if (cond) { for(;;) { …; if(!head) break; if(!(cond)) break; } }` | **binario idéntico** |
| `while (head->mPosition < mPosition) { …; if (!head) break; }` | 88,27 %, **1.440 B** (−48): `loop.c` saca `this->mPosition` del bucle |
| `head->GetPosition() < this->GetPosition()` (accesores inline) | 86,28 %, 1.456 B: corta la duplicación pero deja el bucle sin prueba de entrada |
| `-fno-thread-jumps` | idéntico (no es `thread_jumps`) |
| `-fno-cse-follow-jumps` | 81,35 %, 1.584 B |
| `-fno-cse-skip-blocks` | 85,86 %, 1.560 B |
| `-fno-force-addr` | 91,89 %, 1.500 B |
| `-fno-gcse` | 82,95 %, 1.468 B |

O sea: **el front-end canonicaliza `while`, `for` y `for(;;)`+`break` a la misma
RTL**, y las banderas que cambian algo empeoran mucho. La forma de fuente NO
controla esta rotación. Lo que falta es la construcción que deje la comparación
en el fondo del bucle y sólo el `cror; bso` en la cabecera — y no la he
encontrado. **Premio pendiente: 1.488 B enteros de `RBGrid::Add` más ocho de las
catorce filas de `UpdateAllAvoidables`.**

Las **seis filas restantes de `UpdateAllAvoidables`** son una permutación
r17↔r18↔r19 de los tres registros que llevan `mRootX`/`mRootZ` y el nodo: eso sí
es material de permutador, pero sólo paga si antes cae el bucle.

---

## 3. `AIPursuit::AssignClosestOffsets` (1.684 B, 8 difs) — dos cosas

- **Seis filas: permutación limpia r28↔r29.** El objetivo deja el `@ha` del
  literal en r28 y `count-1` en r29; nosotros al revés. Nada más.
- **Dos filas y CUATRO BYTES** (ours 1.680): el objetivo emite `mr r9,r29` +
  `cmpwi r29,0` y nosotros **`mr. r9,r28`** — `combine` funde la copia y la
  comparación en la forma con registro (`or.`). Somos **una instrucción más
  cortos**. Es el patrón inverso del habitual: aquí sobra optimización, no falta
  código.

## 4. `GRaceParameters::GenerateIndex` (1.680 B, 5 difs)

Confirmo el diagnóstico que ya estaba escrito en la fuente y **no lo reabro**:
cuatro filas son dos `li r31,0` adelantados y una es
`ori r0,r0,0x200` contra `ori r0,r30,0x200` (propagación de copias). Las dos
primeras son **exactamente el mismo tipo de problema que cerró `GTrigger`**
—ranura de emisión— pero aquí el ciclo del objetivo NO tiene hueco: el objetivo
emite `{li r27, lhz}` y nosotros `{li r27, li r31}`, o sea que hay que **bajar el
rango de `li r31`**, no ocupar una ranura. La barrera de ranura no aplica.

## 5. `Smackable::Smackable` (3.120 B, 26 difs)

Territorio agotado por las rondas r27-r29: la fuente lleva **la receta acumulativa
de cuatro piezas** que la deja en 6 difs y 99,839745 %, y dice —con razón— que dos
de esas piezas son `asm` vacíos que el original no tenía y que la función **no
cierra**. Lo que falta son 6 difs y 4 B: un `mr r27,r30` que el objetivo tiene y
nosotros no, porque llevamos el par de direcciones de las `UCrc32` en (r28,r29) y
él en (r27,r28). No he abierto frente nuevo aquí: con `matched_code` todo-o-nada
y cinco formas ya barridas alrededor de ese `mr`, la relación esfuerzo/premio es
peor que la de `SAP.h`.

Nota: ahora que `GTrigger` cerró con un `asm` de cero bytes **y sí cerró**, la
objeción de la r29 («los `asm` vacíos no se dejan puestos porque la función no
cierra») deja de ser una objeción de principio: si las cuatro piezas más una
quinta llegasen a 100 %, se quedarían.

## 6. `SuspensionTraffic::Tire::UpdateLoaded` (856 B, 21 difs) — sin explorar hasta hoy

**Las 21 filas son UNA causa.** El objetivo hoista `lis r30, lbl_803FB6B8@ha`
(el `@ha` del literal `1.0f`) **una sola vez**, antes de `bl VU0_Atan2`, y lo usa
desde r30 en los dos sitios; nosotros lo recalculamos en cada rama
(`lis r9, $LC917@ha` dos veces). Consecuencias en cadena:

- el objetivo salva **dos** GPR (`stmw r30, 0x10(r1)`) y nosotros **una**
  (`stw r31, 0xc(r1)`);
- marco 0x30 contra 0x28, y todos los desplazamientos de `psq_st`/`psq_l`
  corridos 8 B;
- ours **860 B contra 856**: dos `lis` de más menos el prólogo/epílogo ahorrado.

Es `gcse` (elevación de la expresión `high(symbol)` al dominador común), no
reparto ni planificación. `-fno-gcse` y `-fno-force-addr` empeoran mucho
(comprobado en el banco de `SAP.h`), así que las banderas están bien y la
diferencia es de forma de fuente o de una pasada que no dispara. **Frente nuevo y
limpio, con una sola causa: buen sitio para la próxima ronda.**

---

## 7. Lo que dejo puesto

Un solo cambio, en `src/Speed/Indep/Src/Gameplay/GTrigger.cpp`: el temporal `rot`,
el `asm("" : "+m"(boxmat) : : "r0")` y **21 líneas de comentario** con el
diagnóstico del planificador y las cuatro formas negativas.

`SAP.h` queda **exactamente como estaba** (restaurada desde copia byte a byte;
`git status` sólo marca `GTrigger.cpp`). Ojo con esa cabecera: tiene **finales de
línea mezclados** (el bloque del `while` es LF dentro de un fichero CRLF) y un
parche que normalice a CRLF no casa.

Comprobado tras el cambio: `lcfix.py --check` no señala nada de `zGameplay`
(sí de `zMain`, que no es mío y no he tocado); el tamaño de la función es idéntico
(2.544/2.544) y no se añade ningún literal, así que el pool no se desplaza.
Sin commits.
