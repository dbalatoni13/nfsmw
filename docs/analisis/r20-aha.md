# r20 — el mecanismo de los pseudos `@ha`: cuántos viven y durante cuánto

Encargo: un MECANISMO, no una lista. Cinco funciones fallan por el número de
pseudos `high(simbolo)` vivos; una sexta (`Place__16PlaceableScenery`) por lo
contrario.

## 0. Estado real al empezar (medido, no del brief)

`build_direct.py` + `measure.py`/`pctsnap.py` sobre las cuatro unidades:

| función | unidad | B | % | qué le pasa |
|---|---|---|---|---|
| `RenderFlaresOnCar` | zWorld | 2912 | 98,6176 | sobra 1 `lis` |
| `UpdateLightStateTextures` | zWorld | 1068 | 99,3258 | 12 diffs, **no es `@ha`**: es r6/r9 y un `li r0,4` adelantado |
| `UpdateWheelYRenderOffset` | zWorld | 872 | 99,3836 | falta 1 `lis` |
| `UpdateLoaded__Q217SuspensionTraffic4Tire` | zPhysicsBehaviors | 860 | 97,2103 | sobra 1 `lis` + marco 0x28/0x30 |
| `PreloadTransientVaults` | zGameplay | 200 | **100,0000** | **YA CERRADA** (`audit.py` ok) |
| `Place__16PlaceableScenery` | zPhysics | 600 | **100,0000** | **YA CERRADA** (`audit.py` ok) |

O sea: dos de las seis del encargo ya estaban cerradas; el brief venía de una
instantánea vieja. Quedan **5712 B en 4 funciones**, y una de ellas
(`UpdateLightStateTextures`) **no es del mecanismo**.

## 1. El mecanismo, con el volcado RTL delante

`lis sym@ha` no es una decisión del backend: es una expresión RTL de primera
clase, `(set (reg X) (high:SI (symbol_ref ...)))` — patrón `elf_high` (506) — y
su pareja `(set (reg Y) (lo_sum:SI (reg X) (symbol_ref ...)))` — `elf_low`
(507). Las emite el expansor **en pareja y adyacentes**. Todo lo que pasa
después lo deciden **cuatro pases**, en este orden:

| pase | flag | qué le hace al `high` |
|---|---|---|
| `cse` | siempre | unifica dentro de un camino de bloques |
| `gcse` (PRE) | `-fgcse` | **inserta** una copia al FINAL de un bloque que domine, y sustituye los usos redundantes. El volcado lo dice literalmente: `PRE: redundant insn N (expression E) in bb B, reaching reg is R` y `PRE/HOIST: end of bb B, insn M, copying expression E to reg R` |
| `loop` | `-fmove-all-movables` | iza los `high` invariantes al preencabezado como *movables*, y **`combine_movables` funde los que calculan lo mismo** |
| `cse2` | `-frerun-cse-after-loop` | vuelve a unificar **y también REMATERIALIZA**: si en su camino no tiene el `high` disponible, emite uno nuevo |
| `local-alloc` | siempre | `update_equiv_regs` convierte `REG_EQUAL` en **`REG_EQUIV`** y rematerializa el `high` en cada uso |

**La herramienta que faltaba**: `cc1plus -dG` imprime la tabla de expresiones de
PRE con su índice, las redundancias eliminadas y los puntos de inserción. Es la
lectura directa de la decisión. `-dL/-dt/-dl` dan el antes/después de loop, cse2
y local-alloc. Con eso se ve, insn a insn, **qué pase gana o pierde cada `@ha`**.

### Medido, función por función (qué pase decide)

**`UpdateWheelYRenderOffset`** — el `0.0f` (`$LC489` / `lbl_8040AA84`) tiene tres
usos: el `<= 0.0f` de la línea 3248, el `> 0.0f` del width (3278) y el del radius
(3288).

    gcse:  TRES `high` distintos (insns 481, 744, 793)
    loop:  combine_movables funde 744+793 -> insn 932 (izado pelado, sin lo_sum)
           y iza aparte el grupo high+lo_sum+lfs del 3248 -> insn 974
           => DOS en el preencabezado
    cse2:  reescribe el lo_sum de 974 a reg330 (el de 932): 974 muere
           => UNO. El objetivo tiene DOS.

**`Tire::UpdateLoaded`** — el `1.0f` (`$LC251`):

    gcse:  PRE une CINCO usos en reg 362, con dos puntos de copia
           (fin de bb15 y fin de bb19)
    cse2:  pasa de 3 sets a 7 (rematerializa)
    flow:  quedan 4  ->  el objetivo tiene 3

    Comprobado que aquí gcse NO decide nada: `-fno-gcse` da EXACTAMENTE los
    mismos 21 diffs. Quien manda es cse2.

**`RenderFlaresOnCar`** — PRE sí inserta (`insn 2814`, `reg 1010`), pero **al
final del primer bloque del bucle de flares** (nuestra línea 2741), así que sólo
sirve a los dos usos de 4310/4314; el de 4225 se queda con su `lis` local. El
objetivo inserta 324 instrucciones ANTES del bucle (línea 4061 del original) y
sirve a los tres.

## 2. Lo que NO es la palanca (barrido de flags, cifras)

Sobre `CarRender.cpp` suelto (13 s/variante) y `SuspensionTraffic.cpp` (6 s):

| flag | `UpdateWheelYRenderOffset` | `Tire::UpdateLoaded` |
|---|---|---|
| base | 99,384 % — 7 diffs | 97,210 % — 21 diffs |
| `-fno-rerun-cse-after-loop` | 93,653 % — 40 | 84,673 % — 79 |
| `-fno-move-all-movables` | 88,105 % — 79 | — |
| `-fno-gcse` | 84,443 % — 114 | **97,210 % — 21 (IDÉNTICO)** |
| `-fno-expensive-optimizations` | 99,064 % — 10 | — |
| `-fno-force-addr` | 95,452 % — 62 | — |
| `-fno-cse-follow-jumps` | — | 97,210 % — 21 (idéntico) |
| `-fno-cse-skip-blocks` | — | 85,164 % — 84 |
| `-fno-force-mem` | — | 96,977 % — 26 |
| `-fno-schedule-insns` | — | 75,813 % — 118 |

Ninguno mejora: los cflags de la unidad son los correctos y la diferencia es de
FORMA DE FUENTE. El dato útil es cuál es inocuo: **`-fgcse` no toca
`Tire::UpdateLoaded`**, así que ahí buscar formas que muevan PRE es tiempo
perdido; hay que mover **cse2**.

## 3. Descubrimiento de método: los tres fuentes compilan SUELTOS

`CarRender.cpp` y `SuspensionTraffic.cpp` compilados como TU propia con
`mn_repro.py` reproducen **exactamente** el mismo código que dentro de la
SourceList para estas funciones (mismo %, mismo tamaño, mismos diffs). Eso baja
el ciclo de **23 s (`build_direct` zWorld) a 9 s (CarRender) y 4 s
(SuspensionTraffic)**, y permite volcar el RTL de un fichero en vez del de una
SourceList entera.

Herramientas dejadas en el scratchpad (no en el árbol): `aha_lab.py`,
`aha_rtl.py`, `aha_fn.py`, `aha_diff.py`, `aha_var.py`, `aha_batch.py`,
`aha_sweep.py`. `aha_var.py` prueba una variante **sin tocar el árbol**: copia el
.cpp junto al original con nombre `_aha_<tag>.cpp`, lo compila y lo borra.

## 4. Ensayos numerados (todos medidos, ninguno en el arbol salvo el que cierra)

Ciclo: `aha_var.py` copia el .cpp junto al original, lo compila suelto y lo
borra. 6 s en SuspensionTraffic, 15 s en CarRender.

### `Tire::UpdateLoaded` (base 97,2103 %, 860 B, 21 diffs)

| # | forma | resultado |
|---|---|---|
| t1 | `UMath::Abs(fwd_vel) > 1.0f` en vez de `1.0f < UMath::Abs(fwd_vel)` | 21 diffs (IDENTICO: GCC canonicaliza) |
| t2 | `0.5f < mEBrake && 1.0f < skid_speed` | 21 (identico) |
| t3 | t1+t2 | 21 (identico) |
| t4 | el `&&` del primer `if` anidado en dos `if` | 94,220 % — 52 |
| t5 | `mSlip = 0.0f;` antes y solo el `if` positivo | 94,220 % — 52 |
| t6 | condicion invertida con `||` y ramas cambiadas | 94,757 % — 30 |
| v7 | `skid_speed` calculado ANTES del primer `if` | 21 (identico) |
| v8 | `UMath::Abs(fwd_vel)` en una local usada en los dos sitios | 21 (cse2 baja de 7 a 6 `high`, pero greg sigue en 4) |
| v11 | el segundo `if` como dos `if` anidados | 21 (identico) |
| v12 | `GRIP_SCALE().At()` en una local | 21 (identico) |
| v13 | las tres locales declaradas en su sitio en vez de arriba | 21 (identico) |
| L1 | `extern "C" const float lbl_803FB6B8` en los DOS usos | 94,7 %-ish — greg=4 |
| L2 | ...solo en el primero | **greg=3 (el numero del objetivo) pero 97,047 % — 26 diffs, 860 B** |
| L3 | ...solo en el segundo | greg=4 |

`L2` es el unico que iguala el numero de `@ha` del objetivo, y aun asi el codigo
sale peor: **el numero de `@ha` no basta, hace falta que sean los mismos usos**.

### `RenderFlaresOnCar` (base 98,6176 %, 2912 B, 18 diffs)

La hipotesis del `bAbs()` (una inicializacion MUERTA con el MISMO simbolo deja
vivo el `lis` para los usos de mas abajo) aplicada a
`float base_headlight_intensity = 0.0f;` / `base_brakelight_intensity = 0.0f;`,
que el `if/else` de `is_traffic_car` pisa en las dos ramas:

| # | forma | resultado |
|---|---|---|
| f1 | las dos inicializaciones a `lbl_8040AD04` | **2908 B (el tamano exacto)** pero 98,054 % — 62 diffs |
| f2 | solo la primera | idem, 2908/62 |
| f3 | solo la segunda | idem, 2908/62 |
| g1..g4 | lo mismo movido detras de `car_type_info` | idem (g2, que solo mueve sin cambiar el literal: 18 diffs, sin efecto) |

**Lo que pasa de verdad en f1**: el `-4 B` NO es el `lis`. Seguimos con `lis r9`
local para 4225 y ahora con DOS mas (`lis r9` y `lis r11`) para 4310/4314 —
peor. Lo que se ahorra es un `mfcr/stw/lwz/mtcrf` del guardado de CR. El literal
externo impide constant-folding de `base_headlight_intensity` y reordena TODO el
reparto (r14/r15/r16 rotan). **Veda: la inicializacion muerta con el simbolo del
literal no revive el `lis` aqui.**

### `UpdateWheelYRenderOffset` (base 99,3836 %, 872 B, 7 diffs)

Con el volcado de PRE delante, el camino esta cerrado por construccion:

    PRE: redundant insn 481 (expression 51) in bb 27, reaching reg is 330
    PRE: redundant insn 744 (expression 51) in bb 42, reaching reg is 330
    PRE: redundant insn 793 (expression 51) in bb 46, reaching reg is 330
    PRE/HOIST: end of bb 8, insn 932, copying expression 51 to reg 330

Los TRES usos del `0.0f` son la MISMA expresion (51) y bb8 (el preencabezado)
los domina a los tres, asi que PRE **siempre** los unifica: no hay forma de
fuente de las comparaciones que lo evite. Luego `loop` rematerializa el grupo
`high+lo_sum+lfs` del uso de 3248 como insn 974/975, y `cse2` reescribe la base
de 975 de reg345 a reg330 porque `canon_reg` coge `qty_first_reg` — **el que
entro ANTES en la clase de equivalencia**, y PRE inserta antes que loop
(PRE al final de bb8, loop justo antes del NOTE_INSN_LOOP_BEG).

Para tener DOS como el objetivo haria falta que el `high` de loop entrase en la
clase ANTES que el de PRE, y el orden de insercion de esos dos pases es fijo.

## 5. LO QUE CIERRA: `UpdateLightStateTextures` 99,3258 % -> **100 %** (1068 B)

No era del mecanismo `@ha`. Son dos cosas, y las dos salen de herramientas del
documento, no de barrer formas:

1. **`lmap.py`**: el objetivo pone la carga de `ReplaceHeadlightGlassHash`
   **izquierda** en la linea 3489 y la **derecha** en la 3490. Nosotros las
   teniamos al reves. Cambiar el orden de las dos sentencias: 12 -> 11 diffs y
   el orden emitido ya casa (pero el % BAJA a 99,08: el porcentaje miente otra
   vez).
2. **El pin era nuestro y era la causa.** El bloque llevaba
   `register int t asm("r8")` mas una barrera `asm volatile("" : "+r"(t))`. La
   barrera hace falta (sin ella `lights_always_on`, que es constante, pliega el
   indice y se pierden 28 B: 1040 contra 1068). **El registro duro no**: con r8
   FIJADO, `local-alloc` no puede reutilizar el indice moribundo como destino de
   `lwzx` y se lleva la base (`lwzx r9,r9,r8` en vez de `lwzx r8,r9,r8`), y
   arrastra 10 diffs de r6/r9. Quitando solo el `asm("r8")` y dejando la barrera
   vacia: **0 diffs, 1068/1068, `audit.py` ok**.

Ensayos del camino: u1 (solo el orden) 11 diffs · u2 (orden + pin ensanchado a
`left_light_state`) 4 diffs pero **+4 B** por un `mr r9,r8` · u9 (pin que absorbe
la carga) 14 · u12 (barrera repetida) 30 · **u14 (orden + barrera SIN registro
fijo) 0** · u15 (solo quitar el pin, sin reordenar) 2 diffs. **Hacen falta las
dos.**

`measure.py --cmp base_r20_aha.json despues_r20_aha.json`: **+1068 B, +1 funcion,
ninguna unidad baja**. `pctsnap --cmp`: **EMPEORAN: ninguna**.

## 6. Lo que NO he probado

- **`permuter.py`** sobre ninguna de las cuatro (seis rondas sin cerrar nada).
- **Tocar los otros pines de registro duro**: hay **29 ficheros** con
  `register X asm("rN")`. Los he censado contra `pctsnap` y **casi todos estan
  en funciones al 100 %**, asi que NO es un frente — es un caso a caso. El unico
  que estaba en una funcion por debajo del 100 % era el de `CarRender.cpp`, y
  quitarlo la cerro. Los ficheros son de otros agentes esta ronda.
- **`RenderFlaresOnCar` y `UpdateWheelYRenderOffset` con el mapa de lineas
  completo**: he mirado los tramos del `@ha` pero no he auditado sentencia a
  sentencia las 730 y 219 instrucciones como se hizo aqui con
  `UpdateLightStateTextures`. Dado que ahi es donde ha salido el unico cierre,
  **es lo que yo haria primero en la ronda siguiente**.
- **`Tire::UpdateLoaded` con `dwbody.py`/`regmap.py`**: el brief avisa de que
  `regmap` empareja mal con sobrecargas (le encaja la de `SuspensionRacer`), y
  no he filtrado por `low_pc` para sacar las locales de verdad.
- **El marco de `Tire::UpdateLoaded`** (0x30 con `stmw r30` contra 0x28 con
  `stw r31`) por separado: es consecuencia del `@ha` de larga vida, no causa,
  pero no lo he verificado forzandolo.
