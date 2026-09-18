# r46 — zCamera: el censo del `__static_init` sale LIMPIO, y el presupuesto de `cse` queda MEDIDO EN RTL (933 contra 826)

**Nada cerrado. Delta retenido: 0 B / 0 funciones.** El arbol queda exactamente
como estaba: `git status --porcelain src/Speed/Indep/Src/Camera src/Speed/Indep/bWare`
sale **vacio**, `lcfix.py --check` dice «todas las entradas @lc estan al dia», y
el `fncmp` final es identico al inicial:

```
       B  funcion                                        diferencia
    3868  Update__8ICEMoverf                             15 insn, 94 reub
    3604  __static_initialization_and_destruction_0      tamano (3620/3604)
    1192  TerrainVelocityNoise__11CameraMoverP8bMatrix4P 13 insn, 41 reub
    1156  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8 80 insn
     992  Update__19TrackCarCameraMoverf                 15 insn, 22 reub
5 de 453 funciones con el CODIGO distinto -- 10812 B que no cuentan
```

Ninguna funcion empeora. Todos los ensayos de este informe se hicieron en
`scratchpad/r46_cam/` con un **arbol de includes sombra** (`-I scratchpad/r46_cam/inc`
antepuesto en una compilacion propia): en ningun momento se toco `src/`, ni el
`.o` de produccion se genero con una cabecera parcheada. Importa porque habia
otros siete agentes en el arbol.

---

## 1. El censo que pedia el brief: LIMPIO (no falta ni sobra ningun estatico)

El brief mandaba comparar «que estaticos con constructor tiene la SourceList y
en que orden» antes de mirar una sola instruccion. Hecho, contra las
**reubicaciones** de los dos `.o` (`scratchpad/r46_cam/relcensus.py`):

| medida | objetivo | nuestro |
|---|---:|---:|
| reubicaciones dentro de `__static_init` | 487 | 501 |
| llamadas (`R_PPC_REL24`) | **34** | **34** |
| referencias a simbolos NO literales (`@ha`/`@l`) | **192** | **192** |
| referencias a literales (`lbl_*` / `$LC*`) | 261 | 265 |
| relocs de rama local (artefacto de nuestro ensamblado) | 0 | 10 |

* **La secuencia de las 34 llamadas es identica**, en el mismo orden, con una
  sola diferencia de NOMBRE: donde el objetivo trae `__10ICEManager`, nosotros
  traemos una reubicacion contra la seccion (`.text+0x188b4`). Es el mismo
  destino.
* **El multiconjunto de los 192 simbolos globales referenciados es identico**
  (`diff` vacio, 192 lineas en cada lado): mismos globales, mismas
  multiplicidades.

Conclusion: **no falta ni sobra ningun estatico con constructor, y el orden es
el del objetivo.** Eso cierra la hipotesis dirigida del brief y coincide con lo
que ya habia medido la r38 por otra via (90 expansiones inline por lado, mismo
orden). Las 4 referencias a literal de mas son consecuencia de las 4
instrucciones de mas, no su causa.

Reproducible: `python scratchpad/r46_cam/relcensus.py`.

---

## 2. `__static_initialization_and_destruction_0`: el mecanismo, ahora MEDIDO EN RTL

Esta es la aportacion nueva de la ronda. La r36f nombro el mecanismo
(`flush_hash_table` de `cse.c` a las 1.000 insns) pero lo midio **contando
lineas del `.s`**, que es una proxy. Ahora esta medido donde ocurre.

### 2.1 El instrumento (nuevo, 15 s por medida)

**El fuente de GCC 2.95.3 esta en el arbol**: `orig/prodg/NGC_GNU_SRC/NGC/gcc/`.
`cse.c:8899` declara `int num_insns = 0;` **local a `cse_basic_block`** y
`cse.c:8942` hace `if (code != NOTE && num_insns++ > 1000) { flush_hash_table (); ... }`.
O sea: el contador **se reinicia en cada bloque** y cuenta **todo lo que no es
NOTE**.

`cc1plus.exe` acepta los volcados RTL de GCC. La receta:

```
ngccc <cflags reales> -E -o zCamera.i src/Speed/Indep/SourceLists/zCamera.cpp
cc1plus -quiet -O1 -gdwarf+ -mps-nodf -G0 -fno-static-dtors -ffast-math \
        -fforce-addr -fcse-follow-jumps -fcse-skip-blocks -fforce-mem -fgcse \
        -frerun-cse-after-loop -fschedule-insns -fschedule-insns2 \
        -fexpensive-optimizations -frerun-loop-opt -fmove-all-movables -w \
        -dj -dL zCamera.i -o m.s
```

`-dj` deja `zCamera.i.jump` (12 MB, lo que ve `cse1`) y `-dL` deja
`zCamera.i.loop` (11 MB, lo que ve `cse2`). Comprobado que el `.s` que sale de
`cc1plus` sobre el `.i` es **identico** al del driver salvo el nombre de fichero
en `.debug_sfnames` (12 bytes).

`scratchpad/r46_cam/rtlmetric.py` lo automatiza y saca las tres cifras:

```
[jump] hasta ReplayCategoryTable:  933   bloque entero: 1107   etiquetas: 7
[loop] hasta ReplayCategoryTable:  353   bloque entero: 1238   etiquetas: 7
```

### 2.2 La geometria exacta

`__static_init` **no** es un bloque basico gigante: tiene **7 `code_label`**.
Los seis primeros vienen de los dos bucles de `push_back`; el ultimo es el
final. El bloque que contiene la tabla empieza en la etiqueta del contador 714
y tiene **1.107 rtx no-NOTE**.

* la tabla empieza en el rtx **933** del bloque;
* el `flush_hash_table` salta en el **1.001**, o sea **87 rtx DENTRO de la tabla**;
* la tabla entera cuesta **174 rtx** (~29 por elemento).

Por eso los elementos 0 y 1 pliegan y el 2 es el primero que falla: el vaciado
cae justo al empezar el elemento 2. **La cifra que hay que bajar es el 933; el
limite es 826** (= 1.000 − 174), o dicho de otro modo, hay que quitar
**107 rtx no-NOTE** del bloque antes de la tabla.

### 2.3 La calibracion: TRES puntos medidos, el modelo se confirma

Ensayos en la sombra (globales pasados a construccion por defecto; **solo
diagnostico, revertidos**):

| variante | rtx hasta la tabla | rtx del bloque | `addi r27,r30,N` que quedan |
|---|---:|---:|---|
| **base** | **933** | **1107** | 48, 72, 96, 120 (**4**) |
| −3 globales de `Movers/Still.cpp` | 888 | 1062 | 72, 96, 120 (**3**) |
| −8 globales (`Still`, `Showcase`, `Rearview`, `ICEMover`) | 858 | 1032 | 120 (**1**) |
| objetivo | **≤826** | **≤1000** | **0** |

La progresion es exactamente la que predice el modelo: con el bloque a 1.032
(32 por encima del limite) falla **solo el ultimo** elemento. **El mecanismo de
la r36f queda confirmado, ya no por conteo de lineas sino por conteo de rtx en
el volcado del propio pase.**

### 2.4 De que estan hechos los 933 rtx (por si sirve de guia)

| rtx | % | que es |
|---:|---:|---|
| 211 | 22,6 | `high` (parte alta de una direccion) |
| 150 | 16,1 | `plus` (aritmetica de direccion) |
| 150 | 16,1 | store al global |
| 140 | 15,0 | carga de literal |
| 100 | 10,7 | `lo_sum` a registro propio (artefacto de `-fforce-addr`) |
|  71 |  7,6 | copia reg←reg |
|  67 |  7,2 | otros |
|  23 |  2,5 | `const_int` a registro |
|  13 |  1,4 | `CALL_INSN` |
|   8 |  0,9 | carga de memoria no literal |

Y por linea de fuente (las siete mayores):

| rtx | construccion |
|---:|---|
| 125 | `Cubic.cpp:334` `tTable<CubicPovData> aCubicPovTables[7]` |
|  91 | `CopView.cpp:17` `bVector3 vCopViewPoints[5]` |
|  56 | `Cubic.cpp:344` `bVector2 CameraSpeedHugData[5]` |
|  40 | `DebugWorld.cpp:39` `bVector3 spline_points[2]` |
|  34 | `Cubic.cpp:352` `bVector3 vCubicBirdsEyeOffset[2]` |
|  34 | `CopView.cpp:10` `bVector3 vCopViewDistanceFovBand[2]` |
|  30 | `CopView.cpp:25` `bVector2 vCopViewDistanceFov[2]` |

**La contabilidad de `expand_vec_init` son 81 rtx exactos** en ese tramo: 27
elementos de array × 3 (`base += sizeof`, `--iterator`, `this = base`), contados
uno a uno en el volcado. Mas ~4 rtx por array (son 8) de la direccion base
calculada DOS VECES: `expand_vec_init` (`cp/init.c:2884-2887`) hace
`get_temp_regvar` de `rval`, de `base` y de `iterator` antes de nada, y `rval`
solo lo usa la clausula de excepcion. Total ~113 rtx de andamiaje — justo del
orden de los 107 que faltan. **Pero no es palanca**: lei
`cp/init.c:2894-2924` y esa contabilidad la emite GCC 2.95 **incondicionalmente**
para cualquier `T arr[N] = { T(...), ... }` con constructor, sin ninguna guarda
que la evite. Si el original escribio los mismos arrays de la misma forma,
pago lo mismo.

### 2.5 Lo que YA NO hay que probar (medido esta ronda)

| # | ensayo | resultado |
|---|---|---|
| E1 | `bVector3::bVector3(f,f,f)` escribiendo miembros en vez de llamar a `bFill` | **933 rtx, identico**. Confirma en RTL el `f1` de la r36f: el inliner colapsa la cadena y no genera RTL de mas |
| E2 | `-fno-exceptions` | **933 rtx, identico**. `expand_vec_init_try_block` ya salia antes por `!TYPE_NEEDS_DESTRUCTOR` |
| E3 | sonda de rama (un global `extern volatile int` con `?:` justo antes de la tabla, para partir el bloque basico) | crea la etiqueta —el bloque de la tabla pasa a medir 176 rtx— **y aun asi salen los 4 `addi`**. La razon: con `-fcse-follow-jumps`, `cse_end_of_basic_block` **sigue el salto** y no corta ahi. Una etiqueta suelta NO reinicia el contador |

E3 importa como aviso: **no vale meter una rama para reiniciar el presupuesto**;
lo unico que lo mueve es quitar rtx.

### 2.6 Veredicto honesto

Los 107 rtx **no estan disponibles en `Camera/**`**. El precio medido de un
global es 10–15 rtx, y los 933 son todos codigo de inicializacion legitimo del
objetivo (el censo de §1 lo demuestra). Las dos unicas cosas del tamaño
adecuado son (a) el andamiaje de `expand_vec_init`, que GCC emite sin guarda, y
(b) los 100 `lo_sum` a registro propio de `-fforce-addr`.

Sobre (b) hay un dato nuevo que dejo apuntado porque es la unica pista viva:
**la misma construccion produce dos formas distintas segun el contexto**. Un
literal se carga en 2 rtx —`(mem/u:SF (lo_sum (reg) (symbol)))`— o en 3 —
`(set (reg) (lo_sum ...))` con `REG_EQUAL` + `(mem (reg))`—, y en el bloque hay
67 de la primera y 134 de la segunda. `CopView.cpp:10` (array de 2 `bVector3`)
sale **entera barata** y `CopView.cpp:17` (array de 5 `bVector3`, tres lineas mas
abajo) sale **entera cara**. La forma cara es la rama
`explow.c:memory_address()` → `if (!cse_not_expected && CONSTANT_P(x) && CONSTANT_ADDRESS_P(x)) x = force_reg(...)`.
No consegui aislar que la dispara (no es primera-vez-contra-repetida, no es
array-contra-suelto, no es el numero de pseudo). Si alguien lo aisla, pasar las
134 caras a baratas vale 134 rtx: **mas de los 107 que hacen falta**.

---

## 3. `TerrainVelocityNoise` (1.192 B, 13–14 filas): seis negativos nuevos

Diagnostico afinado con los literales identificados uno a uno
(`scratchpad/r46_cam/tvn.txt`):

`lbl_803D1D88` = 20.0f, `lbl_803D1D70` = 0.0f, `lbl_803D1D8C` = 0.05f,
`lbl_803D1D94` = 0.5f, `lbl_803D1D90` = 0.15f.

El orden de los cinco `lfs` es identico en los dos lados. Lo unico que cambia es
**a que ranura de `lis` va cada literal**:

| ranura | objetivo | nuestro |
|---|---|---|
| 115 | 20.0f (r9) | 20.0f (r10) |
| 117 | 0.05f (r8) | **0.0f (r8)** |
| 119 | 0.5f (r10) | 0.05f (**r7**) |
| 123 | 0.15f (r11) | 0.5f (r11) |
| 127 / 129 | **0.0f (r9, en 129)** | 0.15f (r9, en 127) |

El objetivo reparte **cuatro** registros (r8..r11, reusando r9) y nosotros
**cinco** (r7..r11). Con r9 reusado, la antidependencia WAR impide subir el `lis`
del 0.0f por encima de su `lfs`; con r7 libre, `sched` nos lo iza doce ranuras.
Es la misma lectura de la r36f, ahora con la tabla completa de literales.

Seis formas de fuente nuevas, todas en territorio (`CameraMover.cpp`), base
**13 filas**:

| # | cambio | filas |
|---|---|---|
| t2 | `float dot = bDot(...)` a local antes del `bClamp` | 13 = base |
| t3 | `const float accel_min = 0.0f;` y `bClamp(dot, accel_min, accel_max)` | 13 = base |
| t4 | intercambiar las dos sentencias `f_speed_magnitude +=` / `f_speed_frequency +=` | **15** (peor) |
| t5 | partir el `* 0.05f` en `accel *= 0.05f;` | 13 = base |
| t6 | `bMin(accel_max, bMax(dot, 0.0f))` escrito a mano en vez de `bClamp` | 13 = base |
| t7 | `asm("" : "+f"(accel))` detras del calculo | **16** (peor) |

Sigue en pie la veda de la r36f (la cantidad fantasma r6..r12 da 20–21 filas) y
la de la r38 (`asm("" : "+r"(p_car))` → 1.200 B).

---

## 4. `TrackCarCameraMover::Update` (992 B, 15 filas): el orden de stores de `bFill` es CORRECTO

`regmap` da **IDENTICO** (12 locales, mismo reparto), asi que son temporales.
Las 15 filas son dos racimos y un eco:

* **A (107–111)**: las tres cargas de `bCopy` salen en orden **z, x, y en LOS DOS
  lados**; los tres stores de `bFill` salen **z, x, y** en el objetivo y
  **x, y, z** en el nuestro.
* **B (113–124)**: lo mismo en `bScale` — el objetivo empieza por z.
* **C (134–143)**: cola de A y B, ya descartada por la r36f.

Ensayo nuevo (sombra sobre `bMath.hpp`, **diagnostico, revertido**):

| # | cambio | zCamera pendiente |
|---|---|---|
| base | — | 5 funciones, 10.812 B |
| **B2** | `bFill` almacena `dest->z; dest->x; dest->y;` | **280 funciones, 66.548 B** |

**Veda nueva y cara**: el orden `x, y, z` de `bFill` es el correcto; reordenarlo
para arreglar el racimo A rompe 275 funciones que hoy casan (incluidas
`Update__16CubicCameraMoverf`, `Init__19TrackCarCameraMover`,
`Update__21DebugWorldCameraMoverf` y el propio `__static_init`, que se hunde a
86,99 %). El orden z,x,y del objetivo lo produce el planificador, no la fuente.

**Correccion a la r36f §3.** El mapa de lineas del objetivo pone las tres cargas
de `bCopy` en `bMath.hpp:906` y los tres stores de `bFill` en `bMath.hpp:905`. La
r36f lo leyo como «el mapa MIENTE sobre los inlines». La lectura correcta es mas
simple y no requiere ninguna regla nueva: **el `bMath.hpp` original tenia esos
inlines escritos en UNA LINEA cada uno**, y en lineas consecutivas (905 = `bFill`,
906 = `bCopy`, 915 = `bScale`). GCC etiqueta cada insn de la expansion con la
linea de la SENTENCIA dentro de la definicion; si el cuerpo entero esta en una
linea, todas caen en la misma. Es formato del fichero original, no un artefacto
del atribuidor — y por tanto **no es una señal de codegen**: reformatear
`bMath.hpp` no cambiaria un byte. La veda de la r36f (no quitar las locales
intermedias de `bCopy`/`bScale`) sigue en pie por su propia medida.

---

## 5. Lo que NO toque, y por que

* `Update__8ICEMoverf` (3.868 B): PERMUTADOR puro, 15 filas, 0 falta / 0 sobra.
  La r36f agoto `ICEMath.hpp` (unico sitio de territorio, un solo llamante) con
  19 variantes y la r36e el permutador ciego. No repeti nada de eso y no
  encontre angulo nuevo. **Sigue prohibido inventarle locales** (precedente
  r36e / commit `9de121ee`): `regmap` con el DWARF del original da el mismo
  conjunto de locales.
* `_Storage<CameraAI::Director*,2>` (1.156 B): las 22 filas salen de
  `UTLVector.h`, fuera de territorio, y la r42 ya midio la receta obvia (cachear
  `size()`) con cabecera sombra: **objeto identico byte a byte**.

---

## 6. Propuestas fuera de territorio (con la medida que las respalda)

1. **`UTLVector.h::assign()`** — sigue viva la de la r36f §9.3: conseguir un
   `mr r4,r9` en vez de `lwz r4,0x8(r31)` en el bucle de crecimiento, o sea que
   `size()` tenga pseudo propio en la condicion del `while`. **Aviso**: la r42
   probo la version directa (`size_type curSize = size();` refrescada tras
   `push_back`) con cabecera sombra y el objeto salio **identico**; hace falta
   otra receta, no repetir esa.

2. **`bMath.hpp`** — nada que proponer, y dos vedas que dejar escritas en la
   propia cabecera si se quiere: (a) no quitar las locales intermedias de
   `bCopy`/`bScale` (r36f: 10.980 → 32.496 B), (b) **no reordenar los stores de
   `bFill`** (r46: 10.812 → 66.548 B).

3. **`configure.py` / cflags** — NO propongo tocarlos. Dejo la medida por si
   aparece contexto: la r36b midio que `-fno-force-addr` y
   `-fno-rerun-cse-after-loop` llevan los `addi` a 0 en zCamera; con el
   instrumento de §2.1 se puede ahora medir el efecto de un flag en el resto del
   proyecto antes de proponerlo, cosa que antes no se podia.

---

## 7. Herramientas que dejo (todas en `scratchpad/r46_cam/`)

| fichero | que hace |
|---|---|
| `relcensus.py` | censo de reubicaciones de una funcion en los dos `.o`, separando llamadas / globales / literales |
| `rtlmetric.py` | **el instrumento de la ronda**: compila zCamera con volcados RTL (`-dj`, `-dL`) y da el presupuesto de `cse` del `__static_init` y los `addi` que quedan. ~15 s |
| `cc.py`, `pre.py` | compilan zCamera con los cflags reales de `build.ninja`, con los `-I` absolutizados y `-I inc` (sombra) delante |
| `shadow.py` | monta/limpia el arbol de includes sombra: permite parchear una cabecera COMPARTIDA sin tocar `src/` ni molestar a los otros agentes |
| `shadowtest.py` | compila zCamera con la sombra a un `.o` de scratch y lo diffea contra el objetivo — la forma segura de medir un cambio en `bMath.hpp`/`UTLVector.h` |
| `tvn_var.py`, `run.sh` | banco de variantes de `TerrainVelocityNoise` |

`rtlmetric.py` y `shadowtest.py` son generalizables a cualquier unidad con un
cambio de rutas; si alguien los promueve a `scripts/`, el volcado RTL abre el
diagnostico de *todos* los near-miss del proyecto, no solo el de zCamera.

---

## 8. Lo que dejo abierto, por orden de premio

1. **`__static_init` (3.604 B)** — el mecanismo esta cerrado y calibrado (§2):
   hay que quitar **107 rtx no-NOTE** del bloque antes de la tabla, y el precio
   medido de un global es 10–15 rtx. La unica veta del tamaño correcto es la
   forma «cara» de carga de literal (134 casos × 1 rtx). **Lo primero que hay
   que hacer es aislar que dispara `force_reg` en `memory_address()`**; sin eso,
   seguir quitando globales es un barrido ciego que ademas rompe el codigo.
2. **`ICEMover::Update` (3.868 B)** — sin angulo nuevo. Territorio agotado.
3. **`TerrainVelocityNoise` (1.192 B)** — hace falta que el bloque reparta
   cuatro registros de direccion en vez de cinco; seis formas mas descartadas
   (§3) ademas de las de r36e/r36f/r38.
4. **`_Storage` (1.156 B)** y **`TrackCar::Update` (992 B)** — los dos fuera de
   territorio, y las dos recetas obvias ya medidas como neutras/catastroficas.
