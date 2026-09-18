# r36e — zWorld / zWorld2 / zGameplay / zPhysicsBehaviors

**Resultado: `HolePunchAvoidables` mejora de 55 a 49 filas (98,133 → 98,169 %) con
un PIN DE REGISTRO, y `Compare__...GatherVisibleIcons` (16 B) tiene arreglo
MEDIDO fuera de mi territorio.** Las otras siete siguen abiertas, con 30 vedas
nuevas medidas y dos diagnosticos cerrados al nivel del volcado del compilador.

Verificacion, con los cuatro objetos recien compilados:

```
pctsnap --cmp antes.json despues.json --umbral 0.001   (2.826 funciones, 4 unidades)
  EMPEORAN: ninguna
  MEJORAN:  1 funcion
      +0.036 pp  zWorld2  HolePunchAvoidables  98.1329 -> 98.1691  2980 B
lcfix.py --check: todas las entradas @lc estan al dia
```

Tres ficheros tocados, todos en territorio: `WRoadNetwork.cpp` (una linea de
codigo mas 17 de comentario), `CarLoader.cpp` (**solo comentarios**) y
`CarRender.cpp` (**solo comentarios**). Sin commits.

---

## 0. Lo que se lleva la ronda

1. **`HolePunchAvoidables`: el pin de registro SI llega, pero solo suelto.** Las
   r36c/r36d probaron los pines de flotante **encima de la reasociacion**, donde
   salen peor, y de ahi la conclusion de que «los pines no llegan». Sobre la base
   limpia, `register float avoidable_delta_offset asm("fr10")` —y solo ese pin—
   quita seis filas y sube el fuzzy. §1
2. **La regla que sale: un pin que empeora EN COMBINACION no queda descartado
   SUELTO.** Es una variante nueva de «las vedas caducan»: aqui no caduco la base,
   caduco el *acompanamiento*. §1.2
3. **`SetMemoryPoolSize` (304 B, 2 filas): diagnostico CERRADO al ciclo**, con el
   volcado `.sched2` de `cc1plus`. El `lo_sum` es la **ultima** insn del bloque en
   el RTL que ve sched2 —detras del store—, asi que el desempate no lo decide
   sched2 sino el orden que dejo **sched1**. Once vedas. §2
4. **Hallazgo reutilizable: la INSN FANTASMA de reload.** En ese mismo bloque hay
   un `(set (reg 3) (reg 0))` que **esta en el RTL post-sched2, ocupa una ranura
   de emision y no se emite**. Es una insn que cuesta una ranura y cero bytes; en
   cuanto una barrera la vuelve real aparecen 4 B de mas. Explica por que dos
   ensayos «buenos» salieron a 308 B. §2.2
5. **`DefragmentPool` (684 B): permutacion pura de SIETE registros que son TRES
   intercambios independientes entre vecinos del `REG_ALLOC_ORDER`.** Ni los
   pines ni el eje «a quien» los tocan: mueven los tres a la vez. Ocho vedas. §3
6. **`UpdateWheelYRenderOffset`: frente de la cantidad fantasma CERRADO con
   medida.** El objetivo y nosotros usamos el banco r14..r31 **entero**: no hay
   ningun preservado libre donde ponerla. §4
7. **`Compare__...GatherVisibleIcons` (16 B): los bytes son IDENTICOS.** No falta
   ninguna funcion; lo que no casa es el **sufijo de contador** del nombre. El
   informe oficial se los cuenta como pendientes y hay un arreglo de una linea en
   `configure.py`, **medido**: 139.776 → 139.792 B y 766 → 767 funciones. §5
8. `FindNodes` (3.456 B) confirmado como falso positivo de `fncmp` **por la via
   independiente del informe oficial** (`report.json`: zWorld2 135.592/139.388 =
   3.796 B pendientes, que son solo `HolePunchAvoidables` + `InitAtSegment`). §5.2

---

## 1. `HolePunchAvoidables` — 55 → 49 filas

### 1.1 Que se puso

```c
bVector2 avoidable_velocity(his_velocity.x, his_velocity.z);
register float avoidable_delta_offset asm("fr10") =
    bCross(&avoidable_velocity, reinterpret_cast<const bVector2 *>(&cookie.Forward));
```

| | filas | fuzzy | tamano |
|---|---|---|---|
| base r36d | 55 | 98,133 % | 2980/2980 |
| **pin de `fr10`** | **49** | **98,169 %** | 2980/2980 |

Casa el racimo de temporales flotantes de las filas 518-547 y 597-612 (`f10↔f9`,
`f9↔f8`, `f3↔f4` en `lfs`/`fmuls`/`fmsubs`/`fmr`). `regmap --all` despues del pin
baja de **cuatro** locales con registro distinto a **dos**:

```
  b0/b0/b1  avoidable_half_width        f9  ->  f8   <-- DISTINTO
  b0/b0/b1  hole_punch_safety_margin    f4  ->  f6   <-- DISTINTO
  (87 iguales, 2 con registro distinto, 0 de ambito equivocado, 0 solo del original)
```

Lo que queda son tres racimos: los dos locales de arriba, el par cr2/cr3 (filas
176/178/241/650, que la r36d dejo cerrado como sintoma de `allocno_compare`
empatando) y el racimo estructural 619-648.

### 1.2 La regla nueva: un pin que empeora ACOMPANADO no esta descartado SUELTO

La r36c y la r36d midieron los pines **encima de la reasociacion**
(`approach_time * 0.2f * delta_offset`) y anotaron «los pines no llegan»:

```
reasociacion sola                                  97,919 %   64 filas
  + pin de avoidable_half_width a fr9              98,230 %   65 filas
  + ademas los pines de fr10                       98,105 %
```

Suelto, sobre la base, el mismo pin de `fr10` **gana**. La reasociacion mete
tanto ruido de reparto que tapa el efecto del pin. Cuando el catalogo diga «esta
palanca no vale en esta funcion», hay que mirar **con que iba acompanada**.

### 1.3 Las seis combinaciones negativas, medidas (todas 2980/2980)

| ensayo | filas | fuzzy |
|---|---|---|
| pin `fr10` **solo** | **49** | **98,169 %** |
| + `register float offset_change asm("fr10")` | 67 | 98,001 % |
| + `register float avoidable_half_width asm("fr9")` | 57 | 98,085 % |
| + `register float avoidable_half_width asm("fr8")` | 58 | 98,058 % |
| + `register float hole_punch_safety_margin asm("fr4")` | 62 | 97,412 % |
| + reasociacion `approach_time * 0.2f * delta_offset` | 58 | 97,956 % |
| pin de `fr9` en `half_width` SOLO (base vieja) | 63 | 98,048 % |
| pin de `fr4` en `safety_margin` SOLO (base vieja) | 68 | 97,376 % |

Los dos ultimos dicen algo util: de las cuatro locales que `regmap` senalaba,
**solo una acepta el pin**. Las otras dos lo rechazan tanto sueltas como
acompanadas, asi que su registro es consecuencia del de `avoidable_delta_offset`
y no causa propia. Quien retome la funcion: el siguiente objetivo es
`avoidable_half_width` (f9 en el objetivo, f8 nuestro), y ya NO se ataca con un
pin.

---

## 2. `SetMemoryPoolSize` (304 B, 2 filas) — diagnostico cerrado al ciclo

```
59 lis r6, lbl_8040CAA4@ha        | lis r6, $LC1093@ha
60 addi r6, r6, lbl_8040CAA4@l    | stw r0, CarLoaderMemoryPoolNumber@l(r29)
61 stw r0, CarLoaderMemoryPoolNumber@l(r29) | addi r6, r6, $LC1093@l
62 bl bInitMemoryPool             | bl bInitMemoryPool
```

### 2.1 El banco y el volcado

Mini-TU: **`zWorld.cpp` truncado justo detras del `#include` de `CarLoader.cpp`**
(59 lineas). Reproduce la funcion byte a byte (304 B) y compila en **16 s**.
`ngccc` no pasa las `-d`, asi que hay que preprocesar con `ngccc -E` y llamar a
`cc1plus` a mano sobre el `.ii`; con `-dR` el volcado sale como `<tag>.ii.sched2`
y **ya trae la visualizacion por ciclos y las listas de listos** (no hace falta
`-fsched-verbose-5`).

Lo que dice el bloque:

```
;;  Ready list (t = 13):   247  252  254  228        <- el MEJOR va el ULTIMO
;;  Ready list (t = 14):   250  247  252
;;  Ready list (t = 15):   256  232  250
;;  13   lsu: 254 lwz r5        iu2: 228 mr r0,r3
;;  14   lsu: 252 lwz r4        iu2: 247 lis r6
;;  15   lsu: 232 stw           iu2: 250 mr r3,r0
;;  16                          iu2: 256 addi r6,r6,@l
```

El bloque emite **dos por ciclo**. En el ciclo 15 hay **tres** listos y el `addi`
(256) se queda fuera. Con las prioridades iguales, `rank_for_schedule` desempata
por `INSN_LUID` (el mas bajo, el mejor), asi que las tres listas dan el orden de
LUID **antes de sched2**:

```
228 mr r0,r3 < 254 lwz r5 < 252 lwz r4 < 247 lis < 250 mr r3,r0 < 232 stw < 256 addi @l
```

**El `lo_sum` es la ultima insn del bloque en el RTL**, detras del store. Eso ya
lo decidio **sched1**, no sched2: la firma «el `addi @l` una ranura tarde» que el
catalogo atribuye al planificador de salida es en realidad un orden heredado.
Para casar hace falta que el `addi` tenga LUID menor que el `stw`, y ninguna de
las nueve formas de fuente probadas lo consigue.

El mapa de lineas confirma que la fuente ya es la del original: `lmap` da **789**
al `bl bGetFreeMemoryPoolNum`, al `mr r0,r3` y al `stw`, y **790** a los tres
argumentos y al `bl bInitMemoryPool`.

### 2.2 La insn fantasma de reload (hallazgo reutilizable)

En el RTL **post-sched2** esta

```
(insn:TI 250 247 232 (set (reg:SI 3 r3) (reg:SI 0 r0)) 512 {movsi+1} ...)
```

y **no se emite**: la funcion mide 304 B = 76 instrucciones y en el `.s` no hay
ningun `mr 3,0` (r3 ya trae el valor desde el `bl`, asi que la copia es
redundante y algun paso posterior al volcado la borra). Pero **sched2 si la ve**:
ocupa la ranura `iu2` del ciclo 15 y por eso el `addi` se cae al 16.

Es el simetrico de la cantidad fantasma: alli fabricamos una cantidad de cero
bytes a proposito; aqui hay una **insn** de cero bytes que ya esta y que cuesta
una ranura. Consecuencia practica: **en cuanto una barrera la vuelve real,
aparecen 4 B de mas** — que es exactamente lo que pasa en dos de las vedas de
abajo (308 B).

### 2.3 Las once vedas, medidas

| ensayo | tamano | filas |
|---|---|---|
| temporal + `asm("":"+r"(n))` antes del store (la receta del «sexto uso») | **308 B** | 2 (mas un `mr r3,r0` real) |
| idem con copia explicita `int stored = n` | 308 B | 2 |
| idem con `pool_num` como primer argumento de la llamada | 304 B | **identica a la base** |
| `const char *nm = "Cars"; asm("":"+r"(nm))` detras del store | 304 B | 4 |
| idem con la barrera **antes** del store | 304 B | 4 |
| idem con el puntero declarado antes de `bGetFreeMemoryPoolNum()` | 308 B | 6 |
| `asm("":"+m"(CarLoaderMemoryPoolNumber))` antes del store | 304 B | identica a la base |
| idem con `clobber` de `r0` (barrera de ranura) | 304 B | identica a la base |
| la asignacion DENTRO de la lista de argumentos de `bInitMemoryPool` | 304 B | identica a la base |

Las tres «identicas a la base» son la trampa de DCE que documenta la r36d: el
`asm` de `+m` cuya salida se pisa con el store siguiente desaparece entero.

---

## 3. `DefragmentPool` (684 B, 23 filas) — permutacion pura, tres intercambios

`triaje` la clasifica **PERMUTADOR** (regs=23, falta=0, sobra=0, otro=0). Con el
`REG_ALLOC_ORDER` de rs6000 para preservados —r31, r30, r29, r28, r27, r26, r25,
r24, r23, r22, r21, r20, r19, r18, r17, r16— las siete cantidades que bailan son
**tres intercambios independientes entre vecinos de ese orden**:

| cantidad | nuestro | objetivo | posicion |
|---|---|---|---|
| `num_hole_filling_allocations` | r25 | r27 | 6 → 4 |
| `ChunkMovementOffset@ha` | r27 | r25 | 4 → 6 |
| temporal `addi rX,r30,1` | r21 | r22 | 10 → 9 |
| `this` | r22 | r21 | 9 → 10 |
| `zero` | r18 | r16 | 13 → 15 |
| `table` | r17 | r18 | 14 → 13 |
| `CarLoaderMemoryPoolNumber@ha` | r16 | r17 | 15 → 14 |

`regmap` avisa de tres locales que el DWARF del original **no tiene** (`params`
r26, `table` r17, `zero` r18) y la lectura natural —«la local que roba el
registro»— es quitarlas. **Es falsa aqui: las tres son de carga.**

| ensayo | tamano | fuzzy | filas |
|---|---|---|---|
| base | 684 | 99,269 % | 23 |
| quitar las tres locales | 692 | 92,474 % | 47 |
| quitar solo `zero` | 684 | 97,953 % | 29 |
| quitar solo `table` | 684 | 98,626 % | 23 (mismas filas, peor fuzzy) |
| quitar solo `params` | 692 | 94,140 % | 43 |
| `register int num_hole_filling_allocations asm("r27")` | 684 | 99,094 % | 29 |
| `register int zero asm("r16")` | 684 | 97,865 % | 30 |
| `register void **table asm("r18")` | 684 | 94,760 % | 69 |
| `zero` declarado antes del bucle | 684 | 96,550 % | 27 |
| `params` + `table` antes del bucle | 684 | 95,731 % | 30 |
| `asm("":"+r"(num_hole...))` tras el `++` (eje «donde») | 684 | 98,158 % | 24 |
| `asm("":"+r"(movement):"r"(num_hole...))` (eje «a quien») | 684 | 99,123 % | 28 |

**Los dos ejes de la barrera y el pin empeoran los tres intercambios a la vez.**
Es la misma conclusion que `InitAtSegment`: hace falta invertir el ORDEN de
reparto de `local_alloc`, no bloquear registros. Y ojo con la cuarta fila:
misma cuenta de filas y peor fuzzy — el aviso de `nfsmw-medidas-que-enganan`.

---

## 4. `UpdateWheelYRenderOffset` — la cantidad fantasma, cerrada con medida

Falta **una** instruccion (`lis r9, TweakKitWheelOffsetRear@ha`, fila 97), 872
contra 876 B. El objetivo iza el `@ha` del literal flotante a r16 y
**rematerializa** el `@ha` del global dentro del bucle en un volatil; nosotros
izamos el del global a r16 y compartimos un solo `@ha` del literal.

La r36d dejo escrito que la cantidad fantasma «va en la direccion buena pero se
come el registro del segundo `@ha`». **El censo lo cierra**: contando registros
preservados sobre las dos columnas del `fndiff`,

```
objetivo:  r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31
nuestro:   r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31
```

**el banco esta lleno en los dos lados.** No hay ningun preservado libre donde
poner la cantidad fantasma, asi que cualquier colocacion le roba el registro a
una cantidad viva — que es justo lo que midio la r36d en dos intentos. El frente
queda cerrado con la medida, y ademas se aclara el diagnostico: lo que el
objetivo tiene de mas **no es una cantidad preservada** sino un `lis` en un
volatil dentro del bucle. La palanca que falta es **impedir que CSE/loop icen la
direccion de `TweakKitWheelOffsetRear`**, no meter presion.

---

## 5. Contabilidad: dos fantasmas de la cola, uno de ellos con arreglo

La correccion del brief (commit `8ff47187`) llego a las dos funciones por su
cuenta; esto lo confirma por la via independiente del informe oficial y, en el
caso de `Compare`, **aporta el arreglo con su medida**.

### 5.1 `Compare__...GatherVisibleIcons` (16 B): son los bytes IGUALES y el NOMBRE distinto

```
objetivo  Compare__...T1.35326   +93036  16 B   80030004 80640004 7c630050 4e800020
nuestro   Compare__...T1.25193   +88184  16 B   80030004 80640004 7c630050 4e800020
```

**Byte a byte identicos.** El sufijo es el contador de declaraciones de la unidad
(el mismo que arregla `mangfix.py`) y no hay forma de alinearlo desde la fuente:
son 10.133 declaraciones de diferencia.

Pero **el informe oficial SI se los cuenta como pendientes**, porque objdiff
empareja por nombre:

```
report.json:  main/Speed/Indep/SourceLists/zGameplay  matched_code 139776 / 141472   fns 766 / 768
```

El proyecto ya tiene el mecanismo para esto —`OBJDIFF_SYMBOL_MAPPINGS` en
`configure.py`, usado hoy en `gc_interface` y `asd2`— y `measure.py` lo aplica
llamando a objdiff en modo proyecto (`-p . -u <unidad>`). **Medido** poniendo el
mapeo a mano y restaurando `objdiff.json` despues:

| | matched | funciones |
|---|---|---|
| sin mapeo | 139.776 / 141.472 | 766 / 768 |
| **con mapeo** | **139.792 / 141.472** | **767 / 768** |

**PROPUESTA (fuera de mi territorio, `configure.py`):**

```python
OBJDIFF_SYMBOL_MAPPINGS: Dict[str, Dict[str, str]] = {
    ...
    "main/Speed/Indep/SourceLists/zGameplay": {
        "Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.35326":
            "Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.25193",
    },
}
```

Con eso a `zGameplay` le queda **una sola** funcion (`GenerateIndex`, 1.680 B).

**Aviso de caducidad**: el `.25193` es el contador de NUESTRO compilador y se
desplaza en cuanto cambie cualquier declaracion anterior de la unidad — el mismo
problema que `mangfix.py` documenta para los alias de clase local. Si se acepta,
conviene una comprobacion tipo `mangfix --check` que valide el mapeo contra el
objeto recien compilado; si no, el dia que se desplace el mapeo pasara a apuntar
a un simbolo que no existe y los 16 B volveran a contarse mal **sin aviso**.

### 5.2 `FindNodes` (3.456 B): confirmado matched por el informe oficial

`report.json` da `zWorld2  matched_code 135592 / 139388`, o sea **3.796 B**
pendientes = `HolePunchAvoidables` (2.980) + `InitAtSegment` (816). `FindNodes`
no esta. Lo que `fncmp` contaba eran ocho `lis rN, 0x8041` / `lis rN, 0x8044` que
el troceador dejo con el inmediato COCIDO donde nosotros tenemos `@ha` con
reubicacion, mas 72 reubicaciones a estaticos locales con otro numero de
contador (`iMaxNumNodes.10751` contra `iMaxNumNodes.7370`).

---

## 6. Las otras cuatro, al dia

- **`GenerateIndex`** (1.680 B, 5 filas, zGameplay): sin cambios. Cuatro de las
  cinco filas son el mismo patron dos veces —`li r27,0 | lhz | li r31,0 | sth` en
  el objetivo contra `li r27,0 | li r31,0 | lhz | sth` nuestro—. Con lo aprendido
  en §2: el bloque emite dos por ciclo, y en el primer ciclo el objetivo empareja
  `li r27` con el `lhz` (lsu) mientras nosotros emparejamos los dos `li` (dos
  `iu2`). O sea que **la prioridad de `li r31,0` es mayor que la del `lhz` en la
  nuestra y menor en el objetivo**; el `lhz` tiene una cadena corta (`sth`) y el
  `li` alimenta el ctor entero. `PackedDecimal.h` esta **fuera de mi territorio**
  (`src/Speed/Indep/Src/Misc/`) y la r36d ya lo barrio con seis formas. La quinta
  fila (el `ori`) sigue cerrada por la r36d.
- **`CullParts`** (836 B, 5 filas): sin cambios. La primera diferencia que **no**
  es de registro sigue siendo la fila 102, el `mr r9, r0` del preencabezado que
  el objetivo tiene y nosotros no; hasta que caiga esa, el pin de `r0` sobre
  `Polarity` seguira siendo sintoma (regla `nfsmw-si-el-pin-empeora`).
- **`RenderFlaresOnCar`** (2.908 B, 18 filas, 2912/2908): sin cambios, mismo
  mecanismo que §4 pero al reves (nos SOBRA un `lis`).
- **`UpdateLoaded`** (856 B, 21 filas) y **`RBGrid::Add`** (1.488 B, 8 filas):
  frentes cerrados por la r36d-ai, no tocados.

---

## 7. Estado de las cuatro unidades

```
                  pendiente (informe oficial)     lo que queda
zWorld            5.608 B   5 funciones           RenderFlares 2908, UpdateWheelY 876,
                                                  CullParts 836, DefragmentPool 684,
                                                  SetMemoryPoolSize 304
zWorld2           3.796 B   2 funciones           HolePunchAvoidables 2980 (98,169 %),
                                                  InitAtSegment 816 (99,559 %)
zGameplay         1.696 B   2 funciones           GenerateIndex 1680 + Compare 16 (§5.1)
zPhysicsBehaviors 2.344 B   2 funciones           RBGrid::Add 1488, UpdateLoaded 856
```

`zWorld` y `zWorld2` son `NonMatching` en `configure.py`, asi que nada de esta
ronda toca el DOL.

---

## 8. Para quien siga

**Fuera de mi territorio, lo que vale para cualquiera:**

1. **Un pin (o cualquier palanca) que empeora EN COMBINACION no queda descartado
   SUELTO** (§1.2). Antes de dar por muerta una palanca en una funcion, mirar con
   que iba acompanada en la medida que la descarto. Aqui valio seis filas de una
   funcion de 2.980 B que llevaba cinco rondas.
2. **El volcado `.sched2` de `cc1plus` ya trae la visualizacion por ciclos y las
   listas de listos con `-dR` solo** (§2.1): no hace falta `-fsched-verbose-5`, y
   con una mini-TU truncada cuesta 16 s. Las listas se imprimen **peor primero**,
   asi que el orden de la lista, leido al reves, es el orden de reparto de
   ranuras — y con prioridades iguales ese orden ES el orden de `INSN_LUID`, o
   sea el orden del RTL de entrada. **Sirve para leer un desempate de
   `rank_for_schedule` sin conjeturas.**
3. **La insn fantasma de reload** (§2.2): un `(set (reg X) (reg Y))` redundante
   puede estar en el RTL post-sched2, ocupar una ranura y no emitirse. Si un
   ensayo con barrera sale +4 B con un `mr` nuevo, casi seguro que la barrera
   volvio real una insn que ya estaba robando la ranura.
4. **El censo de registros preservados decide antes de probar** si la cantidad
   fantasma tiene sitio (§4). Si los dos lados usan el banco entero, no lo tiene:
   un minuto de `grep` frente a dos compilaciones.
5. **Un simbolo con los bytes identicos puede contar como pendiente** (§5.1): el
   contador `.NNNNN` de GCC no se alinea nunca. El sitio del arreglo es
   `OBJDIFF_SYMBOL_MAPPINGS` en `configure.py`, y conviene una comprobacion de
   caducidad al estilo `mangfix --check`.
