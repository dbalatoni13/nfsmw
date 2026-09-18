# R47 — `zEcstasy` + `zEagl4Anim`

## Resultado

**Cerrada `FnRunBlender::FindMatchTime` al 100 %: 720 B.** `zEagl4Anim` pasa de 3
funciones y 3.528 B pendientes a **2 funciones y 2.808 B**. `zEcstasy` queda byte
a byte como entró (4 funciones, 5.180 B) y su único fichero de territorio no se
ha tocado.

```text
python scripts/fndiff.py zEagl4Anim FindMatchTime__CQ29EAGL4Anim12FnRunBlenderRCQ29EAGL4Anim15MatchPhaseInputRf
# FindMatchTime__...15MatchPhaseInputRf  target=100.0%  ours=100.0%  size=720/720
```

| unidad | antes | después |
|---|---|---|
| `zEcstasy` | 4 fn / 5.180 B | 4 fn / 5.180 B (idénticas) |
| `zEagl4Anim` | 3 fn / 3.528 B | **2 fn / 2.808 B** |

Un solo fichero modificado: `src/Speed/Indep/Src/EAGL4Anim/FnRunBlender.cpp`
(el cierre). Sin `asm`, sin commit, sin `ninja`, sin tocar `configure.py`,
`config/GOWE69/*` ni `splits.txt`.

---

## 1. `FindMatchTime` (720 B) — CERRADA. La conversión que muere y deja su literal

### 1.1 El diagnóstico: el mapa de líneas dice de QUÉ SENTENCIA es el `lfd`

Base 97,361 % / 8 filas / 720 B. Las 8 filas eran **una sola cosa**: el objetivo
carga la constante mágica de int→double (`0x4330000080000000`) en el bloque de
ANTES del primer `bso`, y nosotros dentro del `if`.

```text
objetivo  ... fctiwz f11,f0 | lis r9,bias@ha | stfd f11 | fcmpu | lfd f12,bias@l
              | lis r23 | lwz r28 | stfs f13,0x8(r1) | cror | bso
              xoris | lis r9,0x4330 | stw | stw | lfd f0 | fsub f0,f0,f12 | frsp
nuestro   ... fctiwz f11,f0 | stfs f13,0x8(r1) | stfd f11 | fcmpu
              | lis r23 | lwz r28 | cror | bso
              xoris | lis r10,0x4330 | stw | LIS r9,$LC326@ha | LFD f13,$LC326@l
              | stw | lfd f0 | fsub f0,f0,f13 | frsp
```

Lo que r36e/r36f no habían usado: **`lmap.py` sobre el ORIGINAL** dice a qué
línea pertenece cada instrucción, y a las dos del sesgo les da
**`FnRunBlender.cpp:432`**, que es la sentencia `n = FloatToInt(...)` — NO la
línea 437 del `if`, que es donde vive el resto de la conversión (`xoris`, los dos
`stw`, el `lfd f0`, el `fsub` y el `frsp`, todos sin nota de línea propia detrás
del `cror`/`bso` de 437).

O sea: **en el original, la sentencia `n = FloatToInt(...)` expandía una
conversión int→float**, el optimizador se llevó su aritmética, y sobrevivió el
`force_reg (DFmode, CONST_DOUBLE)` que el expansor de `floatsidf2` emite EN EL
PUNTO DE EXPANSIÓN, porque `cse1` le colgó como consumidor la conversión del
`&&` de la línea 437 y `flow` ya no lo pudo matar.

### 1.2 La palanca: multiplicar por una variable que vale 1

`s` es `int s = 1;` (línea 400 de nuestro fuente, `li r25,1` en el objetivo) y
**no se reasigna nunca**; sólo se usa al final, en `time = (...) * s;`.

```cpp
n = FloatToInt((mCycles[0] + mWeight * (mCycles[1] - mCycles[0])) * 2.0f * s);
```

`float * int` expande `floatsidf2` sobre `s`: `xoris`/`stw`/`stw`/`lfd bias`/
`fsub`/`frsp` y después el `fmuls`. Con `-ffast-math`, `cse` propaga `s == 1` y
el producto se pliega; queda **sólo el `lfd` del sesgo, en el primer bloque**, y
`cse` lo comparte con la conversión de la línea 437. Diff a cero, 720/720 B.

Es semánticamente idéntico (`* 1`) y **no lleva un solo `asm`**.

### 1.3 Las cifras (base 97,36111 % / 8 filas / 720 B)

| forma en `n = FloatToInt(... * 2.0f)` | % | filas | tamaño |
|---|---:|---:|---:|
| **`* s`** | **100,0000** | **0** | **720** |
| `* static_cast<float>(s)` | 100,0000 | 0 | 720 |
| `/ s` | 100,0000 | 0 | 720 |
| `+ 0.0f * s` | 97,3611 | 8 | 720 |
| `* minIdx` (int, sí vivo) | 87,2611 | 35 | 752 |
| `float na = n;` y comparar contra `na` | 90,3889 | 35 | 724 |

`+ 0.0f * s` no vale porque `fold` mata el producto **antes** de expandirlo y la
conversión nunca llega a emitirse. `* minIdx` y la local `float` sí emiten la
conversión pero no la matan después.

### 1.4 Regla nueva para el árbol

> **Los subproductos de una conversión int↔float se reservan al EXPANDIR y
> sobreviven a la muerte de su propio código.** Esto vale para el temporal de
> pila (el «área X» de la r46) *y también* para el `lfd` del literal DFmode del
> sesgo. Si el objetivo carga la constante `0x4330000080000000` (o reserva un
> hueco de 8 B) en un sitio donde nuestro código no tiene ninguna conversión,
> **mira qué línea le da `lmap.py` al `lfd` en el ORIGINAL**: esa sentencia tenía
> una conversión que se plegó. Para reproducirla basta una operación con un
> entero que el compilador pueda demostrar constante (`* s` con `s == 1`), que no
> emite ni un byte pero sí expande la conversión.
>
> Firma para buscarla: el diff sólo enseña `INSERT`/`DELETE` de una pareja
> `lis 0x4330`+`lfd @l` (o de un `stwu` 8 B mayor) y el tamaño total NO cambia.

---

## 2. `DynamicLoader::Initialize` (2.352 B) — el mecanismo del `ble`, identificado

No cerrada (98,87585 % / 28 filas / 2.356 contra 2.352 B). Lo que sí queda
cerrado es **quién emite el `ble` y qué condición exacta hace falta**, que es lo
que las 39 medidas de la r46 buscaban a ciegas.

### 2.1 No es `emit_case_nodes`: es `jump.c`

Leído `stmt.c` (`emit_case_nodes`, `balance_case_nodes`, `group_case_nodes`,
`node_has_low_bound`/`high_bound`) y reproducido el árbol en un micro de 0,3 s
(`scratchpad/r47_ecs/micro.py`, que da **exactamente** nuestras 18 instrucciones).
Resultado: **con nuestra lista de `case`, `emit_case_nodes` sólo puede emitir
nuestra forma**. El árbol es

```text
raiz {8}  izq = {3}(izq {2}, der [4,7])   der = [LO+5,LO+6](izq {9}, der [LO+7,MAX])
```

y como ninguno de los dos hijos de la raíz es `node_is_bounded`, se toma la rama
«neither node is bounded», que emite `GT node->high -> test_label`, el subárbol
IZQUIERDO en línea, `jump default`, `test_label:`, el subárbol DERECHO. Eso es lo
nuestro, literal.

El `ble` del objetivo **no lo emite `stmt.c`**: es `jump.c`, la transformación
`/* Look for if (foo) bar; else break; */` (jump.c:1831), que **invierte el salto
condicional y PERMUTA los dos rangos de insns**. Comprobado en el micro: una
lista de `case` cuyo subárbol izquierdo acaba en salto incondicional da
`cmpwi 4/beq | cmplwi 4/ble | <DERECHO> | b | .L: <IZQUIERDO>`, que es la forma
del objetivo.

Sus condiciones (todas obligatorias):

```text
insn      = condjump a label1,  con label1 == next_label(insn)  y  NUSES(label1)==1
range1end = ultima insn activa antes de label1  ->  tiene que ser un SIMPLEJUMP
label2    = next_label(label1)
JUMP_LABEL(range1end) == label2                 <-- LA QUE NOS FALTA
range2end = ultima insn activa antes de label2, JUMP_INSN seguido de BARRIER
! first   (no en la primera vuelta de jump_optimize)
```

### 2.2 Por qué no se cumple, y qué haría falta

`label2` es la primera etiqueta después del subárbol ALTO, o sea **el cuerpo del
primer `case` en orden de FUENTE = el cuerpo de `SHT_SYMTAB`**. Nuestro subárbol
BAJO termina en `cmpwi 2; beq A` (hoja de valor único: `emit_case_nodes` sólo
emite el `do_jump_if_equal` y cae) y detrás va el `b default` de
`emit_jump_if_reachable`, cuyo `JUMP_LABEL` es el `default`, no `label2`.

En el objetivo, BAJO termina en `b <cuerpo de SHT_SYMTAB>` — que jump.c borra
luego por saltar a la insn siguiente — y por eso `JUMP_LABEL(range1end) == label2`.

Ese `emit_jump(node->code_label)` sólo lo emiten dos ramas de `emit_case_nodes`:

1. `node_is_bounded(nodo{2})` → hace falta `node_has_low_bound`, o sea un
   ANTECESOR con `high == 1` (o `low == TYPE_MIN`, que es 0).
2. el nodo {2} es un **RANGO** con cota alta conocida → hace falta un antecesor
   con `low == high+1`; los antecesores son {3} y {8}, luego `high` sería 2
   (y entonces no es rango) o 7 (y entonces se solapa con el `case 3`).

**Las dos son inalcanzables con esta lista de `case` sin cambiar la semántica**,
y eso explica de una vez las 39 medidas negativas de la r46 y las 60 de ésta.

### 2.3 Medido esta ronda (todo negativo)

Micro de 0,3 s, firma normalizada del árbol (`scratchpad/r47_ecs/sweep_sw.py`):

| barrido | n | resultado |
|---|---:|---|
| 32 combinaciones de `break`/`continue` en los 4 `case` vacíos + `default` | 32 | 4 firmas distintas, **ninguna con `ble`** |
| `default:` en las 8 posiciones posibles del cuerpo | 8 | árbol IDÉNTICO en las 8 |
| orden de los `case` en la fuente (6 permutaciones) | 6 | árbol idéntico salvo `case 9` primero |
| `case 2` como rango 2..3/2..7 | 6 | 5 no compilan (solapan), 2..2 idéntico |
| `case` extra en 0, 1 y 0..1 (con `break` y con `continue`) | 6 | mueven el pivote o añaden un `beq` |

Hallazgos colaterales confirmados con cifra:

- **`group_case_nodes` funde nodos consecutivos cuyas etiquetas llevan al mismo
  `next_real_insn`, o a dos saltos simples al mismo sitio.** Con `case 4...7` y
  `case 8` los DOS en `break` se funden en `[4,8]` y **el pivote pasa a `{9}`**;
  con `continue` + `break` no se funden y el pivote es `{8}`. Es lo que fija toda
  la forma del árbol, y es la razón de que este micro reproduzca el objetivo.
- El pivote sale de `balance_case_nodes` con `i = (nodos + rangos + 1) / 2` y la
  caminata resta 1 por nodo y 2 por rango.

### 2.4 Las otras diferencias de `Initialize`, medidas

Las 28 filas no son sólo el árbol. Se separan en tres grupos:

| grupo | filas | qué es |
|---|---:|---|
| árbol del `switch` | 14 | § 2.1-2.3 |
| orden de operandos `(offset, base)` | 6 | `add r30,r11,r10` / `lwzx r4,r11,r29` / `lwzx r11,r5,r4` / `add r30,r5,r4` / `stwx r9,r5,r4` / `lwzx r0,r5,r4` — el objetivo pone el ÍNDICE primero y nosotros la BASE |
| `&s[nameLength + 1]` | 8 | asociatividad + rotación r29/r30 |

- **Orden de operandos**: `pointer_int_sum` (c-typeck.c) normaliza SIEMPRE a
  `(PLUS ptr int)` sea cual sea el orden en la fuente, así que la forma C no lo
  toca. Medido: `sheader = (ELFSectionHeader*)p + i` y `sym = pHP->symtab + i`
  dan el **objeto idéntico** (28 filas). Donde el destino coincide con el primer
  operando (`add r9,r9,r29`, línea 367) el orden ya casa, o sea que lo decide el
  coalescing, no la fuente.
- **Asociatividad**: el objetivo hace `(s + nameLength) + 1` y nosotros
  `s + (nameLength + 1)`. Con `strcpy(s + nameLength + 1, typebuf)`,
  `strcpy(&s[nameLength] + 1, typebuf)` o una local `char *tail = s + nameLength`
  — **las tres dan el mismo objeto**: 98,70238 % / **30** filas / 2.356 B. Arregla
  4 filas (424, 425, 426, 427: `t` pasa a r29 y `nameLength` a r30, como el
  objetivo) pero **rompe 5** (409, 411, 413, 415, 416: se intercambian `&s[2]` y
  `type_separator`) y además GCC reutiliza r30 para el `add` en vez de un
  registro nuevo, así que el `add` ya no puede adelantarse al `stbx`. No
  aplicado, pero **la asociatividad correcta es la del objetivo**: quien retome
  esto debe partir de ahí y arreglar el reparto, no al revés.

---

## 3. `epCalculateLocalDirectionalPOS16` (2.072 B) — dos vedas nuevas

No cerrada (93,305 % / 155 filas, `stwu -0x178` contra `-0x170`). La r46 dejó
escrito el «siguiente paso acotado»: *«la ranura huérfana tiene que salir de una
sentencia con DOS conversiones de 8 B vivas a la vez (candidatas: las dos
`(sn_ps)vdotn` del `ps_sel` y las dos del `merge00` que alimenta `my_fpow3`)»*.
**Está medido y es que no.**

| forma | % | filas | marco |
|---|---:|---:|---|
| base | 93,305016 | 155 | 0x178 |
| `sn_ps vdotn2 = (sn_ps)vdotn;` y usarla dos veces en el `ps_sel` | 93,305016 | 155 | 0x178 (objeto idéntico) |
| ídem en el `merge00` de `my_fpow3` | 93,305016 | 155 | 0x178 (idéntico) |
| las dos a la vez | 93,305016 | 155 | 0x178 (idéntico) |
| una sola local `sn_ps` reutilizada en las dos sentencias | 93,305016 | 155 | 0x178 (idéntico) |
| `V2(vdotn)` en el `ps_sel` | 82,25676 | 287 | **0x1a0** |
| `V2(vdotn)` en el `merge00` | — | — | no compila |
| `V2(vdotn)` en los dos | 84,16795 | 284 | **0x1a0** |
| **inline manual de `my_fpow`** (las dos conversiones en sentencias sueltas del llamante) | 93,305016 | 155 | 0x178 (idéntico) |

O sea: GCC unifica los dos `(sn_ps)x` idénticos de una misma sentencia antes de
reservar nada, y meter `my_fpow` a mano no cambia un byte. **La ranura huérfana
NO es un `assign_stack_temp` duplicado por sentencia.** Con el dato de la r36d de
que `-fno-schedule-insns` sí da `0x170`, lo que queda como única hipótesis viva
es que sean los **8 B de `secondary_memlocs_elim[]` de `reload`**
(`get_secondary_mem`, reload.c): en rs6000 `SECONDARY_MEMORY_NEEDED` es cierto
para cualquier recarga GPR↔FPR, la ranura se reserva con `assign_stack_local` y
**no tiene dueño ni se libera aunque la recarga se resuelva de otra forma en una
iteración posterior**, que es exactamente la firma («8 B que ninguna instrucción
referencia, por encima de la última local»). Eso encaja con que `sched1` la
provoque, cosa que `assign_stack_temp` no puede.

Aviso para quien la retome: **aunque se arregle el marco la función no cierra**;
la cota medida en la r36d dice que bajaría a ~102 filas.

---

## 4. Lo que NO he tocado, y por qué

- **`UpdatePlatInfo`** (2.044 B, 27 filas). Sacado el `.greg` con `lreg.py`: las
  27 filas son **un ciclo de tres pseudos** y sólo tres:

  ```text
  pseudo 115  n_refs 10  live_len 358  prio 837  -> f1   (el objetivo le da f5)
  pseudo 191  n_refs  7  live_len 317  prio 441  -> f31  (el objetivo le da f1)
  pseudo 202  n_refs  7  live_len 328  prio 426  -> f5   (el objetivo le da f31)
  ```

  `REG_ALLOC_ORDER` de rs6000 (rs6000.h:932) pone los FP en el orden
  **f0, f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, f31, f30 … f14**.
  El ciclo entero se deshace **moviendo sólo el 115**: si el 115 se lleva f5, el
  191 encuentra f1 libre y el 202 se queda f31. La función **no tiene ni una
  llamada** (0 `bl`), así que no es el filtro de `call_used_regs`: f5 está en el
  conjunto de conflictos del 115 por otra vía que hay que leer en la matriz del
  `.greg`. Ése es el único eje que queda; los 86 ensayos de la r36f atacaban el
  reparto por prioridad y ninguno cambia el 115.
- **`GenerateHorizonFogDisplayList`** (796 B, 2 filas). Muro numérico ya escrito
  en el fuente desde la r30 y confirmado por la r36b con el permutador ciego
  entero (445 variantes) y por la r46 con el eje `asm`: `prio(srawi)=12` contra
  `prio(andi.)=2` y `rank_for_schedule` mira `INSN_PRIORITY` antes que nada. La
  palanca nueva de la r46 (`INSN_REG_WEIGHT`) **no aplica**: sólo desempata
  cuando las prioridades son iguales, y aquí no lo son. `lmap` del objetivo
  confirma además que el `andi.` es de la línea del `if` y el `srawi` de la del
  `/2`, o sea que el objetivo lo emite **contra el orden de fuente**: no hay
  forma C que lo produzca.
- **`eProject`** (268 B, 13 filas): 200+ medidas entre r30, r36d y r36f, con las
  cinco palancas. No he añadido nada.
- **`EvalState`** (456 B, 52 filas): sin lead nuevo. La veda de la r43 sigue
  remedida (95,26 % / 48 insn) y la de la r44 (helper con accesores públicos,
  66,89 % / 492 B) también.

---

## 5. Propuestas fuera de territorio

1. **Barrer el árbol con la firma de `FindMatchTime`.** Una pareja
   `lis rX,0x4330` + `lfd fY,@l` (o un `stwu` 8 B mayor) que el objetivo tiene en
   un bloque donde nosotros no tenemos conversión, con el **tamaño total igual**.
   El diagnóstico se cierra en un comando: `lmap.py <unidad> <simbolo>` y mirar
   qué línea le da el ORIGINAL al `lfd`. Si es una sentencia que en nuestro
   fuente no convierte nada, la palanca es `* v` con una variable entera que el
   compilador pueda plegar a 1 (o cualquier otra operación con un entero
   constante-por-propagación). **No es deuda: no emite un byte.**
2. **`lmap.py` sobre el ORIGINAL como primer paso de todo near-miss de
   colocación**, no sólo de sentencias partidas. Aquí ha convertido «8 filas de
   hundimiento entre bloques sin variable C a la que atarse» (r36e/r36f) en «esta
   sentencia tenía una conversión», y el cierre salió a la primera compilación.
3. **Documentar `jump.c:1831` (`if (foo) bar; else break;`) en el brief.** Es la
   única transformación de GCC 2.95 que **permuta dos bloques básicos** e
   invierte la condición, y explica cualquier near-miss del tipo «el objetivo
   pone la rama contraria y los dos brazos al revés». Sus cinco condiciones están
   en el § 2.1. Buscarla es barato: si al objetivo le sobra/falta un `b` justo
   antes de una etiqueta y la condición está invertida, es esto.
4. **`group_case_nodes`**: dos `case` CONSECUTIVOS cuyos cuerpos acaben en el
   mismo sitio se funden en un rango y eso mueve el pivote del árbol entero.
   Cambiar un `break` por un `continue` (o al revés) en un `case` vacío es un
   cambio semánticamente nulo cuando el `switch` es la última sentencia del
   bucle, pero **cambia el árbol**. Vale la pena barrerlo en cualquier `switch`
   que no case: son 2^N compilaciones de 0,3 s en un micro.

---

## 6. Verificación

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEcstasy Speed/Indep/SourceLists/zEagl4Anim
  -> 2 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEcstasy
  -> 4 de 539 distintas, 5.180 B   (IGUAL que a la entrada, funcion a funcion)
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> 2 de 318 distintas, 2.808 B   (entrada: 3 y 3.528 B)
python scripts/fndiff.py zEagl4Anim FindMatchTime__...15MatchPhaseInputRf
  -> target=100.0%  ours=100.0%  size=720/720
python scripts/audit.py Speed/Indep/SourceLists/zEcstasy     -> 0 FALLA
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim   -> 0 FALLA
python scripts/lcfix.py --check
  -> todas las entradas @lc estan al dia
git status --porcelain src/Speed/GameCube/Src/Ecstasy src/Speed/Indep/Src/EAGL4Anim
  -> M src/Speed/Indep/Src/EAGL4Anim/FnRunBlender.cpp     (y nada mas)
```

Ninguna función de las dos unidades empeora. Bancos y artefactos reproducibles en
`scratchpad/r47_ecs/` (`bench.py`, `ver.py`, `micro.py`, `sweep_sw.py`, `t_dl.py`,
`t_fmt.py`, `t_ep.py`, `lreg_upi.txt`, `micro/`).
