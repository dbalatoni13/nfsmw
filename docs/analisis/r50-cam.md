# r50 — zCamera: el `__static_init` esta MEDIDO y cerrado en tamano... y aun asi vale 0 B

**Delta retenido: 0 B de codigo. Los unicos cambios en `src/` son DOS
COMENTARIOS**: la veda de `ICEReplay.cpp:171-174`, que el encargo pedia
reescribir, y la de `Movers/Cubic.cpp:357-360`, que es la que de verdad manda.
Verificado: `build_direct` + `fncmp` dan el **mismo listado exacto** que antes
de tocar nada, y `lcfix.py --check` no tiene ni una entrada de `zCamera`
(las 37 que salen son de `zAI`, `zAnim`, `zEAXSound`, `zGameplay` y `zLua`, de
otros agentes).

```
$ git diff src/Speed/Indep/Src/Camera | grep -E '^[+-]' | grep -vE '^[+-][+-]|^[+-]//'
(vacio: solo cambian lineas de comentario)
```

El hallazgo de la ronda no es una funcion cerrada: es que **el `__static_init`
tiene DOS cerraduras, no una**, y la ronda las separa y mide las dos.

- La primera (los 16 B / 47 filas de los punteros de fila) queda **resuelta en
  el diagnostico y cuantificada al insn**: la hipotesis de `expand_vec_init` de
  la r49 es CIERTA y el presupuesto es **43 insn de RTL** (§1).
- La segunda (las 6 filas de `HydraulicsLookAngle`) es **inmune a cualquier
  relleno, por una razon mecanica que hay que dejar escrita**: `flow` borra el
  codigo muerto ANTES de `sched1` (§2). Y **queda localizada**: nuestro orden
  POST-`reload` **ya es el del objetivo**; lo rota **`sched2`**, y la palanca
  esta un piso mas arriba, en el asignador (§2.2.bis).

Y **`matched_code` es todo-o-nada**: mientras la segunda siga abierta, arreglar
la primera vale **0 B**. Eso baja este territorio de prioridad y hay que
decirlo.

---

## 0. Estado, antes y despues

`fncmp` inicial (identico al de la r49):

```
       B  funcion                                        diferencia
    3868  Update__8ICEMoverf                             15 insn, 94 reub
    3604  __static_initialization_and_destruction_0      tamano (3620/3604)
    1192  TerrainVelocityNoise__11CameraMoverP8bMatrix4P 13 insn, 41 reub
    1156  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8 80 insn
     992  Update__19TrackCarCameraMoverf                 15 insn, 22 reub
5 de 453 funciones con el CODIGO distinto -- 10812 B que no cuentan
```

| # | la r49 decia | lo medido en la r50 |
|---|---|---|
| 1 | «la hipotesis es andamiaje de `expand_vec_init`; hay que censarlo» | **CONFIRMADA y con umbral exacto**: un array de **20** elementos de una clase con constructor VACIO e inicializado con llaves (3+2N = **43 insn**, cero bytes emitidos) deja la funcion en **3.604 B EXACTOS / 99,8391 % / 6 filas** y el resto de la unidad **byte a byte igual**. Con 18 elementos (39 insn) no basta (§1.1) |
| 2 | «~66 rtx», con el relleno puesto en `ICEReplay.cpp` | El presupuesto **no es lineal en insns**: 21 cargas muertas fallan y 1 carga + 60 sumas pasan, con el mismo recuento nominal. Hay que presupuestar **en la construccion**, no en insns (§1.3) |
| 3 | «si lo consigues, quedan 6 filas de cero bytes» | Cierto, y **por eso no vale nada**: `matched_code` es todo-o-nada. Ademas **ningun relleno puede tocar esas 6 filas**, ni puesto DELANTE de ellas: medido con el relleno en la cabecera de `Movers/Cubic.cpp` (§2.1) |
| 4 | `TerrainVelocityNoise`: «solo `update_equiv_regs` puede bajar ese `lis`» | **Confirmado desde una fuente independiente**: en el mapa de lineas del ORIGINAL los cuatro `lis` de literal llevan la linea de su llamante (`CameraMover.cpp:499/501/502/503`) y **solo el del 0.0f lleva `bMath.hpp:240`**, la linea del cuerpo en linea. No lo coloco `sched1`: lo movio un pase posterior (§4) |

---

## 1. `__static_initialization_and_destruction_0`: el presupuesto, al insn

### 1.1 La medida que confirma la hipotesis de la r49

Instrumento: sombra completa de `src/Speed/Indep/Src/Camera/` con el `-I`
**delante**; `full.py` compila la unidad entera en 19 s y da tamano, % y filas;
`det.py` cuenta los punteros de fila **sin pasar por el diff** (control
independiente, el que exige la leccion de metodo de la r49).

Relleno: **andamiaje puro de `expand_vec_init`, cero bytes emitidos**, en la
cabecera de `Movers/Cubic.cpp`:

```c
struct EC { EC() {} int a; };
static EC gEC[N] = { EC(), EC(), ... };   // N veces
```

`cp/init.c::expand_vec_init` (linea 2857) emite para esto `rval`, `base` e
`iterator`, y por elemento `base = base + size` y `iterator = iterator - 1`:
**3 + 2N insn de RTL**, todas muertas; ninguna llega al objeto porque el
constructor esta vacio.

| N | 3+2N | `__static_init` |
|---:|---:|---|
| 16 | 35 | 3624 B, 96,1587 %, 87 filas |
| 18 | 39 | 3624 B, 96,1587 %, 87 filas |
| **20** | **43** | **3604 B EXACTO, 99,8391 %, 6 filas** |
| 24 | 51 | 3604 B, 99,8391 %, 6 filas |

Y el resto de la unidad **no se mueve**: `full.py` sigue diciendo
`139 funciones != 100%, 21816 B` en los cuatro casos, la misma cifra exacta que
la base.

**Traduccion**: al `__static_init` del original le sobran, respecto al nuestro,
**el andamiaje de ~20 elementos de array construido** dentro del bloque
extendido que empieza en `Movers/Cubic.cpp` y termina en la tabla de
`ICEReplay.cpp`. Con el, `cse_basic_block` purga su tabla hash
(`if (code != NOTE && num_insns++ > 1000) flush_hash_table();`,
`orig/prodg/NGC_GNU_SRC/NGC/gcc/cse.c:8942`) **antes** de la tabla en vez de en
mitad, la equivalencia `base ≡ r30 + K` sobrevive y los seis elementos salen
con desplazamiento completo.

Equivalencias medidas del mismo umbral, todas con el relleno en la cabecera de
`Cubic.cpp` (`det.py`, cuenta de punteros de fila):

| relleno | punteros de fila |
|---|---:|
| base | 4 |
| 21 cargas muertas | 5 |
| **22 cargas muertas** | **0** |
| `bVector3 v[4]` con constructores | 5 |
| **`bVector3 v[6]`** con constructores | **0** |
| `EC gEC[18] = {…}` | 5 |
| **`EC gEC[20] = {…}`** | **0** |

Y la **ventana** de la r49 queda revalidada con este instrumento (que es el que
no emite nada; el barrido de la r49 se hizo con el detector que fallaba por
registro base). `EC gEC[20] = {…}` colocado en:

| fichero | `__static_init` |
|---|---|
| `Camera.cpp` (antes de la ventana) | 3620 B, 97,1121 %, 54 filas — **no hace nada** |
| `Movers/Cubic.cpp` | **3604 B, 99,8391 %, 6 filas** |
| `Movers/Still.cpp` | **3604 B, 99,8391 %, 6 filas** |
| `ICE/ICEReplay.cpp` (delante de la tabla) | **3604 B, 99,8391 %, 6 filas** |

En los cuatro casos el resto de la unidad sigue en `139 funciones != 100%,
21816 B`. **Dentro de la ventana la posicion da igual; solo cuenta el
recuento.**

### 1.2 Y el censo mata la historia de «nos falta un array»

Los elementos de array CONSTRUIDO que ya tiene nuestra ventana:

| fichero | arrays con constructor | elementos |
|---|---|---:|
| `Movers/Cubic.cpp` | `aDriftData[2]`, `aCubicPovTables[7]`, `CameraSpeedHugData[5]`, `vCubicBirdsEyeOffset[2]` | 16 |
| `Movers/CopView.cpp` | `vCopViewDistanceFovBand[2]`, `vCopViewPoints[5]`, `vCopViewDistanceFov[2]` | 9 |
| `Movers/DebugWorld.cpp` | `spline_points[2]`, `fDebugCameraInputData[5]` | 7 |
| `ICE/ICEOverlays.cpp` | `gIceOverlays[5]` | 5 |
| `ICE/ICEReplay.cpp` | `ReplayCategoryTable[6]` | 6 |
| | | **43** |

(Los siete `CubicPovData Cubic*[2]` **no cuentan**: son agregados con llaves
anidadas, se inicializan en `.data` y no emiten un solo rtx.)

Para juntar 43 insn a 2 por elemento harian falta **20 elementos mas: un 47 %
de sobrante**. No hay nada en esa ventana que lo sugiera. Las dos formas que si
encajan con el perfil —RTL que no llega al objeto— son:

1. elementos de array cuyo constructor sea **vacio** (andamiaje puro), o
2. **~22 referencias duplicadas** a un global o a una constante del pool: con
   `-fforce-addr` cada una entra en el RTL como `high` + uso = 2 insn, y `cse`
   funde todos los `high` del mismo simbolo en uno, asi que el objeto **no
   cambia**.

La segunda es la unica que explica un sobrante de este tamano sin inventar
datos, y **el propio relleno de cargas muertas la mide**: las 22 cargas de la
tabla de arriba leen todas el MISMO array (`gQSrc[i]`), o sea son 22
referencias duplicadas a un simbolo, 44 insn de RTL — la misma cifra que los 43
del `EC gEC[20]`. Lo que hay que buscar en la r51, si alguien retoma esto, es
**una sentencia del original que repita ~22 veces una constante o un global que
ya esta cargado** dentro de la ventana.

### 1.3 Aviso: el presupuesto NO es lineal en insns

Contando «una carga de global = 3 insn (`high`, `lo_sum`, uso) y una suma = 1»,
dos rellenos del **mismo** recuento dan resultados opuestos:

| relleno | recuento nominal | resultado |
|---|---:|---|
| 21 cargas muertas | 63 | **falla** (3624 B, 87 filas) |
| 1 carga + 60 sumas (`q = q + K`) | 63 | **pasa** (3604 B, 6 filas) |
| 1 carga + 42 sumas | 45 | falla |
| 22 cargas muertas | 66 | pasa |

Ninguna asignacion de coste por carga (2 o 3 insn) hace cuadrar las cuatro
filas: **lo que decide no es solo `num_insns`**. Las cargas aportan ademas
pseudos nuevos y referencias a memoria, que mueven `max_reg` y `next_qty`
(`cse.c:8832-8844` y `9059`). **Presupuesta en la construccion que vayas a
usar, no en insns**, y no traslades umbrales entre formas. Las cifras firmes de
esta ronda son las de `EC gEC[N]` (§1.1), que es la construccion que interesa.

### 1.4 Dos instrumentos que NO valen (para que nadie los repita)

* **`int aN = q + K;` con `aN` sin usar emite CERO insn de RTL.** 21, 40 y 55 de
  ellos dejan la funcion en la base exacta (3620 B, 54 filas, 4 punteros). Un
  local sin leer cuya inicializacion es aritmetica pura sobre locales se cae en
  la expansion; uno cuya inicializacion es una LECTURA DE MEMORIA, no. (Este
  es el modo de fallo «18 ensayos identicos» de la r49 con otra cara: un
  barrido entero puede no compilar nada distinto.)
* **Un array de ctor vacio SIN lista de llaves** (`static EC gEC[N];`) emite un
  **bucle en tiempo de ejecucion** (con su salto): cambia la estructura de
  bloques, no el recuento, y su efecto **no depende de N** — de 8 a 128 da
  siempre 3 punteros de fila en vez de 4, y no arregla nada.

---

## 2. La SEGUNDA cerradura: las 6 filas de `HydraulicsLookAngle`

### 2.1 Ningun relleno las toca, y la razon es mecanica

El relleno de §1.1 se puso en la **cabecera de `Movers/Cubic.cpp`**, o sea
**delante** de las sentencias de `HydraulicsLookAngle`/`NOSFovWidening`. Aun
asi las 6 filas quedan **exactamente igual** para todos los N que funcionan
(20 y 24 elementos, 22 y 40 y 70 cargas, 60 sumas).

La razon: en GCC 2.95 el orden de pases es

    jump -> cse -> gcse -> loop -> cse2 -> FLOW -> combine -> SCHED1
         -> local_alloc -> global_alloc -> reload -> sched2

**`flow` borra el codigo muerto ANTES de que `sched1` decida.** Un
relleno muerto puede mover la purga de `cse` (§1) pero **nunca** puede tocar
una decision del planificador.

**Regla general que sale de aqui**: si el racimo que quieres mover lo decide
`sched1`, `local_alloc`, `reload` o `sched2`, **el codigo muerto no es un
instrumento**; solo sirve para los pases anteriores a `flow` (`jump`, `cse`,
`gcse`, `loop`, `cse2`).

Medido directamente sobre NUESTRO `.s` (`hyd.py`, saca el orden real de las
tres insns; el objetivo es `LIT, CONST, SYM`), con el relleno siempre en la
cabecera de `Cubic.cpp`, o sea DELANTE:

| relleno | orden de las tres insns |
|---|---|
| base | `SYM(r11) LIT(r9) CONST(r10)` |
| `EC gEC[20]` = 43 insn MUERTAS (arregla la tabla) | `SYM(r11) LIT(r9) CONST(r10)` — **identico** |
| `bVector3 v[6]` = codigo VIVO | `SYM(r29) … CONST(r27)`, y f24 ya ni se carga ahi |
| **objetivo** | **`LIT CONST SYM`** |

El codigo vivo si mueve el vecindario (cambian los registros y el uso de f24):
el control cambia, la medida no miente. El muerto no lo toca **nunca**.

### 2.2 Que son exactamente las 6 filas (leido entero)

Tres insns independientes rotan CICLICAMENTE entre tres ranuras. Los registros
que salen **por RANURA** son los mismos en los dos lados (r11, r9, r10); lo que
difiere es **que valor lleva cada uno** (y eso resulta ser la causa, §2.2.bis).

| ranura | objetivo | nuestro |
|---:|---|---|
| 541 | `lis r11, lbl_803D3A80@ha` (literal) | `lis r11, HydraulicsLookAngle@ha` |
| 549 | `li r9, 0x38e` (la constante) | `lis r9, $LC1125@ha` (literal) |
| 557 | `lis r10, HydraulicsLookAngle@ha` | `li r10, 0x38e` |

`objetivo = [LITERAL, CONSTANTE, SIMBOLO]`, `nuestro = [SIMBOLO, LITERAL,
CONSTANTE]`. Todo lo demas sale de ahi: el `lfs f24` de 558, la reutilizacion
del registro que muere para `NOSFovWidening` en 561/562, y el `sth` de 564.

**El literal esta identificado**: `lbl_803D3A80` = **12000.0f**, y se carga en
**f24 (preservado)** porque se consume **50 y 97 instrucciones despues**, en
`stfs f24, 0x4(r29)` (`vCopViewDistanceFovBand[0].y`, `0x800812DC`) y
`stfs f24, 0x4(r28)` (`vCopViewDistanceFov[0].y`, `0x80081398`), los dos de
`Movers/CopView.cpp`. Es decir: su cadena de dependencias es con mucho la mas
larga de las tres, los dos lados le dan la prioridad mas alta, y lo unico que
difiere es **si gana la primera ranura el literal o el `lis` del simbolo**.

### 2.2.bis Es `sched2`, y nuestro orden PRE-`sched2` ya es el del objetivo

Separacion hecha con la receta de la r48 §1.2 (una compilacion por bandera,
15 s, sin volcados):

| | orden de las tres insns |
|---|---|
| base | `SYM(r11) LIT(r9) CONST(r10)` |
| **`-fno-schedule-insns2`** (sched2 apagado) | **`LIT(r9) CONST(r10) SYM(r11)`** |
| `-fno-schedule-insns` (sched1 apagado) | `CONST(r0) SYM(r9)`, f24 ni se carga ahi |
| **objetivo** | **`LIT(r11) CONST(r9) SYM(r10)`** |

**Nuestro orden POST-`reload` / PRE-`sched2` es exactamente el del objetivo.**
`sched2` lo rota. Y como `sched2` corre despues de `reload`, apagarlo **no
cambia un solo registro**: el mapa valor->registro es el mismo en las dos filas
de arriba (SYM=r11, LIT=r9, CONST=r10). Lo que difiere del objetivo es ese
mapa: el objetivo tiene SYM=r10, LIT=r11, CONST=r9.

Eso cambia el diagnostico de sitio, igual que paso en el racimo B de `TrackCar`
(r48 §1.2): **en `sched2` el nivel 2 de `rank_for_schedule` (`INSN_REG_WEIGHT`)
no existe, y con la prioridad empatada decide `depend_count`** — o sea, cuantos
insns posteriores dependen del registro que cada una define. Con registros
distintos a los del objetivo, la cuenta sale distinta y la rotacion es
inevitable. **La palanca esta un piso mas arriba, en el ASIGNADOR** (palanca 3
del brief), no en `sched2` ni en la fuente de `Cubic.cpp`.

`-fno-schedule-insns2` es solo un diagnostico, no un arreglo: destroza la
unidad (386 funciones != 100 %, 123.908 B, y esta funcion se va a 677 filas).

**Y el punto de ataque para la r51 se puede escribir con numeros.**
`REG_ALLOC_ORDER` de GPR es `0, 9, 11, 10, 8, 7, …`, asi que el reparto va
`r9` al de mas prioridad, `r11` al segundo y `r10` al tercero:

| | 1.º (r9) | 2.º (r11) | 3.º (r10) |
|---|---|---|---|
| nuestro | LIT | SYM | CONST |
| objetivo | CONST | LIT | SYM |

Las tres cantidades tienen `n_refs = 2` (una definicion y un uso) y tamano 1,
asi que `QTY_CMP_PRI = floor_log2(2)*2*1 / (death - birth) = 2 / vida`: **gana
el que menos vive**, en el orden POST-`sched1`. Para reproducir al objetivo hay
que conseguir que el rango de vida de `li 0x38e` sea el mas corto de los tres, y
el de `lis HydraulicsLookAngle@ha` el mas largo. Eso ya no se decide en
`Cubic.cpp`, sino en donde `sched1` deja esas tres insns respecto a su `sth`.

### 2.3 Y la forma de la fuente esta CONFIRMADA como correcta

| ensayo (sobre `Movers/Cubic.cpp`) | `__static_init` | filas 520-600 |
|---|---|---:|
| base | 3620 B, 97,1121 %, 54 filas | **6** |
| `HydraulicsLookAngle = 910;` y `NOSFovWidening = 1638;` (enteros) | **3592 B**, 95,4739 %, 102 filas | 57 |
| los tres `bAngle` como enteros | **3576 B**, 92,3885 %, 162 filas | 79 |
| `HydraulicsLookAngle` y `NOSFovWidening` intercambiados (mueve datos) | 3620 B, 97,1110 %, 55 filas | 7 |

El entero **encoge la funcion** porque `= 910` SI es expresion constante: el
objeto pasa a `.data` y desaparece del `__static_init`. El objetivo tiene
`li 0x38e` + `sth`, o sea **inicializacion dinamica**: `bDegToAng(5.0f)` es la
forma correcta, y ahora esta demostrado en vez de supuesto.

**No hay palanca de fuente sin mover datos**: el orden de definicion fija el
orden de `.bss`/`.data`, y el unico ensayo que mueve el racimo (intercambiar los
dos `bAngle`) lo empeora ademas de mover los simbolos.

---

## 3. `Update__8ICEMoverf` (3.868 B): el permutador CIEGO, hecho y negativo

La r49 lo dejo escrito como lo siguiente que tocaba («con `regmap` dando
IDENTICO de verdad, lo que toca aqui es el permutador CIEGO con el catalogo
entero, no mas formas a mano»). Hecho:

```
python scripts/permuter.py zCamera "ICEMover::Update" -j 6 --rounds 2
  base    : 967 instrucciones  score=0.985522  opcodes=1.000000  1a divergencia=589
  catalogo: 738 variantes de un solo cambio
--- ronda 1: 738 variantes ---
  738 variantes probadas  (24/min)  45 no compilan
  ninguna mejora; paro
resultado: score=0.985522 opcodes=1.000000 1a divergencia=589  (967 insn vs 967)
```

**738 variantes, 693 compiladas, CERO mejoras.** El catalogo: `move_stmt` 172,
`block_wrap` 144, `swap` 79, `split_decl` 64, `hoist_temp` 41, `empty_then` 38,
`bind_ref` 27, `cse_temp` 24, `split_assign` 20, `commute` 18, `add_local` 16,
`hoist_decl` 14, `expand_use` 13, `sink_decl` 13, `invert_if` 10, `split_arg` 9,
`inline_temp` 8, `const` 7, `case_rotate`/`case_order` 6, `elseif` 4,
`empty_case` 3, `split_and` 2. Con las **30 formas a mano de la r49** son
**768 formas de fuente descartadas con cifra**.

El dato que hay que retener es **`opcodes=1.000000`**: la secuencia de opcodes
ya es la del original de punta a punta, y las 15 filas son solo OPERANDOS. Eso
es exactamente el perfil que el permutador no puede atacar por la fuente, y
coincide con el veredicto IDENTICO del DWARF (r49 §2.1).

Y la segunda tanda, **recocido de profundidad 2** (468 variantes sueltas del
catalogo restringido + **400 combinaciones de DOS cambios simultaneos**,
`--anneal 0.02`), tampoco:

```
--- ronda 1: 868 variantes ---
  868 variantes probadas  (19/min)  66 no compilan
  ~ paso lateral split_assign:split_assign@5079   score=0.985522 (mejor sigue 0.985522)
resultado: score=0.985522 opcodes=1.000000 1a divergencia=589  (967 insn vs 967)
```

**1.606 compilaciones entre las dos tandas, un solo paso lateral y CERO
mejoras** (`out/perm_ice.log`, `out/perm_ice2.log`). Con las 30 formas a mano de
la r49, **esta funcion esta agotada desde la fuente**: lo que queda es el
reparto de registros, no la forma. Quien la retome que no vuelva a escribir
variantes: que ataque el asignador, como en §2.2.bis.

---

## 4. `TerrainVelocityNoise` (1.192 B): el mapa de lineas confirma a la r49

`lmap.py` sobre el ORIGINAL da una confirmacion independiente del diagnostico
de la r49 (que salio del volcado de `sched1`):

```
8006642C  lis r9, lbl_803D1D88@ha   (20.0f)  CameraMover.cpp:499
80066434  lis r8, lbl_803D1D8C@ha   (0.05f)  CameraMover.cpp:501
8006643C  lis r10, lbl_803D1D94@ha  (0.5f)   CameraMover.cpp:503
8006644C  lis r11, lbl_803D1D90@ha  (0.15f)  CameraMover.cpp:502
80066458  lfs f9, lbl_803D1D88@l(r9)         bMath.hpp:240
80066460  lis r9, lbl_803D1D70@ha   (0.0f)   bMath.hpp:240   <- REUTILIZA r9
80066464  lfs f0, lbl_803D1D70@l(r9)
80066468  fsel f13, f13, f13, f0             bMath.hpp:230
80066474  fsubs f0, f9, f13                  bMath.hpp:231
```

Las lineas del original alinean **exactamente** con las nuestras con un
desplazamiento de +290 (`497` = nuestra 207, `499` = 209 `const float
accel_max = 20.0f;`, `501` = 211, `502` = 212, `503` = 213). Los cuatro `lis`
que coloco `sched1` llevan **la linea de su llamante**; el del 0.0f lleva
**`bMath.hpp:240`**, la linea del cuerpo en linea de `bClamp`, y va entre dos
insns de ese mismo cuerpo. **No lo puso ahi `sched1`: lo movio un pase
posterior**, que es justo lo que predijo la r49 (`update_equiv_regs` de
`local-alloc.c`, o la rematerializacion que hace `reload` de un pseudo con
`REG_EQUIV` que se queda sin registro duro).

Y la separacion por banderas (`tvn.py`, 8 s por compilacion sobre el micro de
`CameraMover.cpp`) dice de que pase es, sin volcados:

| | orden de los `lis` de literal | registros de direccion |
|---|---|---:|
| base | 20.0f, **0.0f**, 0.05f, 0.5f, `lfs`20.0f, 0.15f, `lfs`0.0f | 5 (r7..r11) |
| `-fno-schedule-insns2` | **el mismo** (solo cambian los registros) | 5 |
| `-fno-schedule-insns` | cada `lis` pegado a su `lfs`, sin izar | **2** (r9, r11) |
| objetivo | 20.0f, 0.05f, 0.5f, 0.15f, `lfs`20.0f, **0.0f**, `lfs`0.0f | 4 |

O sea: **`sched2` no tiene nada que ver** (apagarlo deja el mismo orden) y es
`sched1` quien iza los cinco `lis` y crea la presion de 5 registros — apagarlo
la baja a 2. Confirma r48 §6.bis con un instrumento distinto y de 8 s.

De paso queda confirmado que nuestro `bClamp(float)` tiene el **orden de
argumentos correcto** (`bMin(MAXIMUM, bMax(a, MINIMUM))`, `bMath.hpp:198-200`):
el objetivo hace `fsubs f0, f9, f13` con f9 = 20.0f, o sea `MAXIMUM - x`, que
es esa forma y no la otra.

Sigue en pie lo que hay que buscar: que el pseudo del `high` del 0.0f tenga
`REG_BASIC_BLOCK < 0`, es decir, que se use en **mas de un bloque basico**. En
la fuente el 0.0f aparece dos veces y en bloques distintos
(`CameraMover.cpp:192`, el ctor de `speed_table`, y `:211`); que `cse` no funda
esos dos `high` es lo que habria que romper.

---

## 5. `Update__19TrackCarCameraMoverf` y `_Storage`

Sin ensayos nuevos: la r48 y la r49 dejan cerradas las dos vias con medida (el
DWARF prohibe los tres pines de `TrackCar` y la receta de la gemela `TrackCop`
empeora; `_Storage` necesita que `size()` llegue a `push_back` por parametro, o
sea cambiar la firma de un metodo que consumen decenas de unidades). **No los
toque nadie sin leer antes r48 §1/§3/§6 y r49 §4.**

---

## 6. Propuestas fuera de territorio

0. **`docs/PLAYBOOK.md` — palanca nueva y barata: separar los pases con las
   BANDERAS, no con volcados.** La r48 gasto media ronda con `-dS
   -fsched-verbose-5` (y la traza de `sched2` que sale por stderr) para
   averiguar que racimo es de `sched1` y cual de `sched2`. **Dos compilaciones
   con `-fno-schedule-insns` y `-fno-schedule-insns2` contestan lo mismo en
   15 s**, sin volcados de 25 MB y sin leer una tabla ciclo x unidad:
   - si apagar `sched2` **cambia** el racimo -> es de `sched2`, y como corre
     despues de `reload` **los registros no se mueven**: lo que ves es el orden
     POST-`reload`, o sea el que vio el asignador;
   - si apagar `sched2` lo deja igual y apagar `sched1` lo cambia -> es de
     `sched1`.
   En esta ronda ha dado dos respuestas nuevas: el racimo de
   `HydraulicsLookAngle` es de `sched2` (§2.2.bis) y el de
   `TerrainVelocityNoise` es de `sched1` (§4). No es un arreglo —las dos
   banderas destrozan la unidad— es un DIAGNOSTICO.

1. **`docs/PLAYBOOK.md` — regla nueva, y es de prioridad**: antes de atacar un
   near-miss, **contar sus racimos independientes**. Una funcion con dos
   racimos de causas distintas vale 0 B hasta que se cierran LOS DOS, y el
   esfuerzo no se suma, se reparte. El `__static_init` de `zCamera` lleva
   **cuatro rondas** apuntando a un racimo que, aun resuelto, no promociona un
   byte. El diagnostico es barato: aplica la palanca que arregla el racimo A y
   mira si el B se mueve. Aqui son 20 s, y nadie lo habia hecho.

2. **`scripts/fncmp.py` — que diga los RACIMOS, no solo las filas.** Hoy dice
   «tamano (3620/3604)» y esconde que las 54 filas son 47 de una causa y 7 de
   otra, con 230 filas identicas entre medias. Bastaria agrupar las filas
   contiguas (con un hueco maximo) y decir «2 racimos: 7 filas @549, 47 filas
   @796».

3. **Ampliar el aviso del brief sobre el codigo muerto**: es instrumento valido
   solo para los pases **anteriores a `flow`**. Para `sched1`/`local_alloc`/
   `reload`/`sched2` no mide nada, aunque se ponga delante del racimo (§2.1).

4. **`scripts/regmap.py`** — sin novedad respecto a la propuesta 2 de la r49
   (el veredicto PERMUTACION es inseguro cuando los nombres difieren, y aqui dio
   un falso positivo que costo una ronda). Sigue pendiente.

---

## 7. Herramientas en `scratchpad/r50_cam/`

Heredadas de la r49 y reapuntadas (`cc.py`, `full.py`, `asm.py`, `det.py`,
`sh.py`, `mkinc2.py`, `mtry.py`/`mrows.py`/`mctx.py`, `fctx.py`), mas:

| fichero | que hace |
|---|---|
| `pad.py` / `pad2.py` | rellenos muertos parametricos (cargas, sumas sobre el mismo local, locales distintos) |
| `mapsum.py` | resume `lmap.py` en tiradas `fichero:linea` con su direccion; es lo que localizo la ventana y el literal de f24 |
| `e1..e11.py` | los barridos de §1 (umbral fino, formas de relleno, `expand_vec_init` puro, y `e11.py` la ventana revalidada) |
| `h1.py` | los cuatro ensayos de `HydraulicsLookAngle` de §2.3 |
| `hyd.py` | saca el ORDEN real de las tres insns del racimo desde nuestro `.s`; es lo que separo `sched1` de `sched2` (§2.2.bis) |
| `tvn.py` | lo mismo para los cinco `lis` de literal de `TerrainVelocityNoise`, con el valor de cada `$LC` y el recuento de registros de direccion (§4) |
| `out/perm_ice.log`, `out/perm_ice2.log` | las dos tandas del permutador ciego |
| `out/lmap_static.txt` | el mapa de lineas del ORIGINAL para el `__static_init` (61 kB) |

**Todos los ensayos se hicieron sobre la sombra `inc2/`; `src/` no se toco en
ningun momento salvo el comentario de la veda de `ICEReplay.cpp`.**
