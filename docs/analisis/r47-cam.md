# r47 — zCamera: la «veta viva» de la r46 era una lectura equivocada, y el muro de `TrackCar` queda localizado en la fila 5 de `rank_for_schedule`

**Nada cerrado. Delta retenido: 0 B / 0 funciones.** El arbol queda exactamente
como estaba:

```
$ git status --porcelain src/Speed/Indep/Src/Camera src/Speed/Indep/bWare src/Speed/Indep/Libs/Support
(vacio)
$ python scripts/lcfix.py --check
todas las entradas @lc estan al dia
```

`fncmp` final identico al inicial:

```
       B  funcion                                        diferencia
    3868  Update__8ICEMoverf                             15 insn, 94 reub
    3604  __static_initialization_and_destruction_0      tamano (3620/3604)
    1192  TerrainVelocityNoise__11CameraMoverP8bMatrix4P 13 insn, 41 reub
    1156  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8 80 insn
     992  Update__19TrackCarCameraMoverf                 15 insn, 22 reub
5 de 453 funciones con el CODIGO distinto -- 10812 B que no cuentan
```

**Ninguna funcion empeora.** Todos los ensayos se hicieron con compilaciones
propias a `scratchpad/r47_cam/out/` (nunca a `build/GOWE69/src/`) y con copias
sombra de las cabeceras; `src/` no se toco en ningun momento.

**Aviso de arranque.** El `git status` del brief listaba `M` en
`src/Speed/Indep/Src/Camera/ICE/ICEReplay.cpp` y `.../Movers/Cubic.cpp`. Cuando
empece a medir, los dos ya coincidian con HEAD y `zCamera` daba exactamente la
base de cierre de la r46 (5 funciones, 10.812 B), asi que **no se perdio nada de
zCamera**; lo dejo anotado porque es el modo de fallo de la regla 7.

---

## 1. El encargo colateral: la auditoria de `bCross` en zCamera sale LIMPIA

zCamera tiene **cinco** llamadas a `bCross` y **las cinco son la version de tres
argumentos** `bVector3 *bCross(bVector3 *dest, const bVector3 *v1, const
bVector3 *v2)`, que **no es inline** (se declara en `bMath.hpp:477` y se define
en `bWare/Src/bVector.cpp:158`). Es decir: aqui el orden de argumentos **no** se
lee en un `fmuls`/`fmsubs` —esa firma es la del `bCross(a,b)` de `bVector2`, que
zCamera **no usa ni una vez**—, sino en el reparto de `r4`/`r5` antes del `bl`.
Y eso lo verifica el diff directamente:

| fichero:linea | funcion que la contiene | `fndiff` |
|---|---|---|
| `CameraMover.cpp:682` | `EnforceMinGapToWalls__11CameraMoverP9WColliderP8bVector3T2P8bVector4` | **100.0 %**, 1224/1224 B |
| `Movers/Cubic.cpp:460`, `:461` | `MakeSpace__16CubicCameraMoverP8bMatrix4` | **100.0 %**, 440/440 B |
| `Movers/Showcase.cpp:59`, `:60` | `BuildPhotoCameraMatrix__19ShowcaseCameraMover` | **100.0 %**, 600/600 B |
| `Movers/TrackCop.cpp:330` | `Update__19TrackCopCameraMoverf` | **100.0 %**, 948/948 B |
| `Movers/TrackCar.cpp:345` | `Update__19TrackCarCameraMoverf` | near-miss (ver §4) |

Cuatro de los cinco sitios estan dentro de funciones **byte a byte identicas**:
si el orden de argumentos estuviera invertido, `r4` y `r5` saldrian cambiados y
no casarian. Queda probado.

El quinto (`TrackCar.cpp:345`) es literalmente **la misma sentencia** que la de
`TrackCop.cpp:330` —`bCross(&hcomp, &displacement, &up)`— y su gemela casa al
100 %. Ademas, en el diff de `TrackCar::Update` las tres preparaciones de
argumento (`addi r4,r1,0x18` fila 102, `addi r5,r1,0x8` fila 106,
`addi r3,r1,0x28` fila 108) y el propio `bl bCross` (fila 125) son **identicas**;
las 15 filas distintas estan todas fuera de ese tramo.

**Conclusion: el orden de argumentos de `bCross` en zCamera es correcto en los
cinco sitios. Frente cerrado.**

---

## 2. `__static_init`: la «veta viva» de la r46 NO existe (medido en RTL)

La r46 dejo como unica pista viva del presupuesto de `cse` que «134 cargas de
literal usan una forma CARA de 3 rtx y 67 una BARATA de 2», y que aislar el
disparador valdria mas de los 107 rtx que faltan. **Lo reproduje y las dos
formas no son el mismo constructo.** Volcado `-dj` de `cc1plus` sobre
`zCamera.i`, bloque de la tabla (`scratchpad/r47_cam/lits.py`):

```
items 3651
block start item 1444 table at rtx 933 block total 1107
cheap(mem(lo_sum)) 70   expensive(set reg lo_sum) 140
```

(Las cifras de la r46 —67/134— son las mismas modulo el criterio de conteo.)
Mirando el contenido:

* Las **70 «baratas»** son todas de la forma
  `(set (reg/v:SF N) (mem/u:SF (lo_sum (reg) (symbol_ref/u "*$LCnnnn"))))`:
  **cargas del VALOR de una constante flotante** del pool. Dos rtx: `elf_high`
  mas la propia carga.
* Las **140 «caras»** son todas de la forma
  `(set (reg:SI N) (lo_sum (reg) (symbol_ref ...)))` con `REG_EQUAL`, y el
  simbolo **no es un literal flotante**: `aDriftData`, `gDriftSpeed`,
  `aCubicPovTables`, `_CDActionDebugWatchCar`,
  `Construct__21CDActionDebugWatchCar...`,
  `_Q43UTL3COMt7Factory3...Prototype.mHead`, y las cadenas `$LC695`.
  Son **materializaciones de DIRECCION**: la direccion tiene que acabar en un
  registro porque se usa como valor (argumento, base de varios stores, puntero
  guardado).

No son «dos formas de lo mismo» sino dos construcciones distintas, y las 140 no
tienen forma barata posible. **La veta esta cerrada en negativo: no hay ahi 134
rtx que recortar.**

### 2.1 `-fno-force-addr`: negativo definitivo, con la cifra

La r36b habia medido que ese flag lleva los `addi` a 0. Medido ahora sobre la
unidad entera (compilacion propia + `objdiff`, `scratchpad/r47_cam/try.py`):

| flags | funciones distintas | bytes | `__static_init` |
|---|---:|---:|---|
| base | **5** | **10.828** | 3620 B, 97,1121 % |
| `-fno-force-addr` | **138** | **96.520** | 3604 B (tamano EXACTO) pero **73,9689 %** |

El tamano se vuelve exacto y el codigo se rompe entero, ademas de destrozar otras
133 funciones. **No es la respuesta y no hay que volver a probarlo.**

### 2.2 El censo de la r46, ahora como SECUENCIA (no solo multiconjunto)

La r46 comparo el multiconjunto de las 192 referencias a globales. Lo he
repetido **como secuencia ordenada por offset**, que era el hueco:

```
globals T 299  O 298      (incluye repeticiones)
insert  O[166] HydraulicsLookAngle @0x876
delete  T[169] HydraulicsLookAngle @0x8b6
delete  T[259] __10ICEManager      @0xc30   (en nuestro lado, reubicacion contra seccion)
T ReplayCategoryTable primera referencia en indice 260, offset 0xc3a
O ReplayCategoryTable primera referencia en indice 259, offset 0xc3a
```

Es decir: **de 299 referencias solo UNA esta fuera de sitio**
(`HydraulicsLookAngle`, desplazada 0x40 dentro del bloque de
`Demo1/Demo2EyeOffset`), y **la tabla empieza en el MISMO offset 0xc3a en los dos
lados**. La geometria del `__static_init` esta confirmada: los 16 B de mas estan
integramente dentro de la tabla.

El diff del `__static_init` tiene **54 filas** (30 ARG_MISMATCH, 13 INSERT, 9
DELETE, 2 REPLACE) y se reparten asi: **7 filas antes de la tabla** (549, 557,
558, 561, 562, 564 y 796 — el `li rN,0x38e` y el `lis HydraulicsLookAngle@ha`
intercambiados 8 ranuras, **sin cambio de numero de instrucciones**) y **47
filas de la fila 818 en adelante**, todas de la tabla. La permutacion de
`HydraulicsLookAngle` no toca el presupuesto de rtx.

**Veredicto:** el diagnostico de la r46 (hay que quitar 107 rtx no-NOTE del
bloque antes de la tabla, limite 826) sigue en pie y **ahora sin ninguna veta
pendiente**. Lo unico del tamano adecuado que queda es el andamiaje de
`expand_vec_init`, que `cp/init.c:2894-2924` emite **sin guarda**.

---

## 3. Instrumento nuevo: la reproduccion MICRO (4-5 s por medida)

El brief pedia «un `.cpp` suelto que reproduzca la funcion byte a byte». Lo hay
para dos de los cinco near-miss, y esto es lo mas reutilizable que dejo:

```cpp
// scratchpad/r47_cam/micro_tc.cpp
#include "Speed/Indep/Src/Camera/Movers/TrackCar.cpp"
```

Compilado con los cflags reales de `build.ninja`, `Update__19TrackCarCameraMoverf`
sale **identica instruccion a instruccion** a la de la unidad completa (248
instrucciones, solo cambian los numeros de `$LC` y de etiqueta). Lo mismo con
`CameraMover.cpp` para `TerrainVelocityNoise`. El ciclo pasa de **~50 s a 4,3 s**,
y `mtry.py`/`mrows.py` diffean el micro `.o` **contra el `zCamera.o` objetivo**
sin pasar por ninja ni por `build/GOWE69/src/`.

Con esa base:

* `micro_tc.cpp` + `-dS -fsched-verbose-5` da **la lista de listos y la eleccion
  de `sched1` ciclo a ciclo**. Es el volcado que convierte «el planificador hace
  otra cosa» en una fila concreta.
* `stry.py` compila zCamera **con `-I scratchpad/r47_cam/inc` delante**, con una
  copia completa de `Libs/Support/Utility/` (hay que copiar el DIRECTORIO entero,
  no solo `UTLVector.h`: `UListable.h` la incluye con comillas y en forma
  relativa, y una sombra parcial se la salta en silencio).

---

## 4. `TrackCarCameraMover::Update` (992 B, 15 filas): el muro esta en el nivel 5 de `rank_for_schedule`, y `bFill` queda EXCULPADO por medida

### 4.1 Los tres racimos

Las 15 filas son ARG_MISMATCH puro en tres racimos:

| filas | sentencia | que pasa |
|---|---|---|
| 107-111 | `Look = *CarToFollow->GetGeometryPosition();` (`bMath.hpp:905`, `bFill`) | mismos valores y mismos registros; **cambia el ORDEN**: objetivo `z, x, y`, nosotros `x, y, z` |
| 113-123 | `displacement /= distance;` (`bMath.hpp:915`, `bScale`) | mismos registros (f11=x, f12=y, f13=z en los dos); **cambia el ORDEN**: objetivo `z, x, y`, nosotros `x, z, y` |
| 134-143 | `look_offset.x/y/z = TrackCarLookOffset*[CameraType]` (`TrackCar.cpp:609-611`) | mismo orden (`z, x, y` en los dos); **cambian los REGISTROS**: objetivo f11/f12/f10, nosotros f0/f13/f12 |

El patron es uno solo: **en las ternas x/y/z el objetivo saca la `z` primero y
nosotros el orden de fuente.**

### 4.2 Por que la `z` sale primero — y por que a nosotros no (mecanismo NUEVO)

`rank_for_schedule` de `haifa-sched.c` tiene cinco niveles: prioridad, luego
`INSN_REG_WEIGHT` (**solo `sched1`**, gana el menor), luego la clase respecto a
`last_scheduled_insn`, luego el numero de dependientes, y por ultimo `INSN_LUID`
(**gana el menor**, o sea el orden de fuente).

En las **CARGAS** el desempate cae en el nivel 2, y esto no estaba documentado en
el proyecto (la nota del proyecto solo cubria stores):

> Las tres cargas de `bCopy` leen el mismo puntero. La `REG_DEAD` de ese puntero
> esta en el **ultimo uso en la fuente**, que es la carga de `z`. `find_insn_reg_weight`
> le da entonces **peso 0** (pone 1 registro, mata 1) frente a **+1** de las de
> `x` e `y`. Menor gana → **la `z` se planifica primero**. Por eso las tres
> cargas salen `z, x, y` **en los dos lados**.

En los **STORES** ese mecanismo no discrimina: cada store pone 0 registros y mata
1 (su flotante), peso −1 los tres. Prioridad 52 los tres, 3 dependientes los tres
(`1070 638 558`), y ninguno depende de `last_scheduled_insn`. **Cuatro de los
cinco niveles empatan y decide el LUID**, que es el orden de `bFill`. Traza real
(`-dS -fsched-verbose-5`, insns 517=`0x90`/x, 520=`0x94`/y, 523=`0x98`/z):

```
;;  Ready list (t =  5):  ... 508
;;    --> scheduling insn <<<508>>> on unit lsu
;;    dependences resolved: insn 523 into ready
;;    dependences resolved: insn 520 into queue with cost=2
;;    dependences resolved: insn 517 into ready
;;  Ready list (t =  6):  495 1016 846 604 571 568 523 517
;;    --> scheduling insn <<<517>>> on unit lsu      <-- gana por LUID
```

### 4.3 `bFill` almacena `x, y, z` en el original — MEDIDO, no inferido

La r46 dejo la veda de no reordenar `bFill` por una regresion (10.812 → 66.548 B).
Ahora hay **prueba positiva**, y del mismo fichero: `Init__19TrackCarCameraMover`
(2.708 B, **codigo identico al objetivo**) contiene dos expansiones de `bFill`, y
el mapa de lineas del ORIGINAL las pone asi:

```
80075308  stfs f0,  0x338(r1)   bMath.hpp:905      <- x
80075314  stfs f13, 0x33c(r1)   bMath.hpp:905      <- y
80075318  stfs f12, 0x340(r1)                      <- z
80075320  stfs f0,  0x328(r1)   bMath.hpp:905      <- x
80075328  stfs f13, 0x32c(r1)   bMath.hpp:905      <- y
8007532C  stfs f12, 0x330(r1)                      <- z
```

Y `Update__19TrackCopCameraMoverf` (948 B, 100 %) igual: `0x58`, `0x5c`, `0x60`.

**El `bFill` del original es `x, y, z`. El orden `z, x, y` de `TrackCar::Update`
lo produce el planificador, no la fuente.** La veda de la r46 queda confirmada
por dos vias independientes y **no hay que volver a tocar `bFill`**.

### 4.4 Lo medido y negativo (cada ensayo con su cifra)

Base: **15 filas / 99,6290 %**. Todos los ensayos con el micro, revertidos.

Posicion de la sentencia en `TrackCar.cpp` (la actual es la correcta):

| ensayo | filas |
|---|---:|
| base | **15** |
| `displacement /= distance;` antes de `Look = ...` | 47 |
| `bCopy(&Look, ...)` explicito | 15 (neutro) |
| puntero cacheado `const bVector3 *p_geo` | 15 (neutro) |
| `Look = ...` antes del bloque `fov` | 52 |
| `Look = ...` despues de la division | 47 |

Helper local `CamCopy` (para mover el LUID **solo en este sitio**, sin tocar
`bMath.hpp`), 12 combinaciones de orden de cargas × orden de stores:

| cargas | stores `x,y,z` | stores `z,x,y` |
|---|---:|---:|
| `xyz` | **15** (=base) | 18 |
| `xzy` | 15 | 17 |
| `yxz` | 17 | 17 |
| `yzx` | 18 | 18 |
| `zxy` | 17 | 18 |
| `zyx` | 18 | **14** |

Con `stores z,x,y` **el ORDEN de las filas 107-111 casa exactamente** (es la
primera vez que ese racimo se pone en orden), pero los registros de las tres
cargas rotan (f0→f12→f13→f0). Anadiendo los pines de la palanca 3:

```cpp
register float x asm("fr0")  = s->x;
register float y asm("fr13") = s->y;
register float z asm("fr12") = s->z;
d->z = z; d->x = x; d->y = y;
```

**el racimo A cierra entero (filas 101-111 identicas) y la funcion baja a 10
filas / 99,7500 %.** Es el mejor resultado que ha tenido esta funcion.

Sobre esa base, el racimo de `bScale` (filas 119-123) **no se mueve desde la
fuente**: seis ordenes del helper `CamDiv` mas tres juegos de pines:

| ensayo | filas |
|---|---:|
| `CamCopy` con pines, sin tocar la division | **10** |
| `CamDiv` orden `xyz` / `yxz` / `zyx` sin pines | 10 (neutro) |
| `CamDiv` orden `xzy` sin pines | 13 |
| `CamDiv` orden `yzx` / `zxy` sin pines | 12 |
| `CamDiv` `zxy` con pines fr11/fr12/fr13 | 16 |
| `CamDiv` `xzy` / `zyx` con pines | 14 |

Y catorce banderas, ninguna mejora (la primera columna es el numero de filas):

```
-fno-schedule-insns 129 | -fno-schedule-insns2 68 | -fno-gcse 57
-fno-rerun-cse-after-loop 67 | -fno-cse-skip-blocks 51
-fno-expensive-optimizations 22
-fno-move-all-movables / -fno-force-mem / -fno-cse-follow-jumps /
-fno-rerun-loop-opt / -fstrength-reduce / -fno-peephole /
-fomit-frame-pointer / -fno-thread-jumps / -fno-defer-pop : 15 (neutras)
```

`-fno-expensive-optimizations` **no** reproduce aqui lo que hizo en `TrackCop`
(22 filas contra 15): la pista de `regmove`/`optimize_reg_copy_2` no aplica.

**Revertido todo** (regla 9: 10 filas con tres pines puestos es deuda).

**Lo que queda escrito para la proxima:** los racimos B y C son *sintoma*; la
unica diferencia que no es de registro es el orden de los tres stores de la fila
107, y para ganarla la `z` necesita batir a la `x` en uno de los cuatro primeros
niveles de `rank_for_schedule` —los cuatro empatan hoy—. En terminos de la
palanca: **al store de `z` le falta una segunda muerte de registro** (peso −2 en
vez de −1). Un store `stfs fN, off(r31)` solo puede matar su flotante porque
`r31` sigue vivo; para que muriese tambien la base habria que materializar el
destino en un pseudo propio, y eso mete un `addi` que el objetivo no tiene.

---

## 5. `_Storage<CameraAI::Director*,2>` (1.156 B): la fila del `mr r4,r9` CASA por primera vez

Es la copia-constructora, y su codigo sale de
`UTLVector.h::assign()` (bucle de crecimiento, fuera de territorio). Las 22
diferencias son 20 de permutacion `r24`↔`r25` y **dos estructurales**:

```
  66 lwz r4, 0x8(r31)      | lwz r0, 0x8(r31)      <- size() para `minSize > size()`
  67 cmplw r26, r4         | cmplw r26, r0
  70 lwz r0, 0x4(r31)      | lwz r4, 0x8(r31)      <- NOSOTROS RECARGAMOS size()
  71 (nada)                | lwz r0, 0x4(r31)      INSERT
 130 mr r4, r9             | cmplw r26, r9         REPLACE   <- al objetivo le sobra un `mr`
 132 cmplw r26, r4         | (nada)                DELETE
```

O sea: el objetivo tiene **un unico pseudo con `size()`** vivo desde antes del
bucle, reutilizado dentro de `push_back` (`cmplw r4, r0` contra `capacity()`) y
refrescado en el latch con `mr r4, r9`. Nosotros evaluamos `size()` dos veces
arriba y usamos `r9` directo abajo: +1 instruccion arriba, −1 abajo, y por eso
los tamanos coinciden (1156/1156) con dos errores compensados.

Medido con cabecera sombra (zCamera entero, `stry.py`):

| variante de `assign()` | `_Storage` | tamano | resto de zCamera |
|---|---:|---:|---|
| base | 98,6990 % | 1156 | 5 fn / 10.828 B |
| A: `size_type curSize = size();` + `curSize = size();` en el bucle | 98,6990 % | 1156 | **objeto IDENTICO al base** |
| B: `for (curSize = size(); minSize > curSize; curSize = size())` | 98,6990 % | 1156 | identico al base |
| C: A + `asm("" : "+r"(curSize));` tras el refresco | **99,2180 %** | **1160** | 5 fn / 10.832 B |
| G/H: `++curSize` como induccion en vez de `curSize = size()` | 97,4879 % | 1164 | 5 fn / 10.836 B |

**A y B reconfirman la medida de la r42** («objeto identico»): sin barrera, `cse`
colapsa `curSize` y no queda ni rastro. **C es lo nuevo**: con la barrera
selectiva en el latch **las filas 130/132 dejan de ser REPLACE/DELETE y pasan a
ser el mismo par de instrucciones que el objetivo** (`mr rN, r9` + `cmplw r26,
rN`, solo cambia el registro elegido, r0 en vez de r4). Es la primera vez que esa
fila casa.

Lo que impide cobrarlo: la barrera parte el pseudo, y entonces el `size()` de
dentro de `push_back` **vuelve a recargarse** (la fila 70/71 sigue), asi que el
tamano se va a 1160 en vez de a 1156. Hacen falta las dos cosas a la vez.

**Propuesta fuera de territorio (con la medida):** en
`src/Speed/Indep/Libs/Support/Utility/UTLVector.h`, `assign()`, el bucle
`reserve(minSize); if (minSize > size()) { do { push_back(); } while (minSize >
size()); }` necesita **un pseudo unico para `size()`, vivo desde antes del bucle
y refrescado en el latch sin que `cse` lo colapse**. La receta de la r42
(`curSize` a secas) esta descartada por dos medidas independientes; la barrera
sola llega a mitad de camino. El siguiente intento razonable es que el pseudo
sobreviva tambien dentro de `push_back` —por ejemplo fijandolo con la palanca 3
(`register size_type curSize asm("r4")`)— pero eso ya es un cambio en una
cabecera que consumen decenas de unidades y **no lo aplico**: la barrera de
`assign()` ya rompio zSim/zPhysics/zMain una vez (commit `6d623cb9`).

---

## 6. `TerrainVelocityNoise` (1.192 B, 14 filas): siete formas mas, todas neutras o peores

Con el micro de `CameraMover.cpp` (reproduce la funcion exacta, 4 s por medida).
Base **14 filas / 99,0772 %**:

| ensayo | filas |
|---|---:|
| base | **14** |
| tres locales (`dotp`, `acc0`, `accel`) separadas | 14 |
| `20.0f` literal en vez de `const float accel_max` | 14 |
| punteros `p_acc`/`p_fwd` cacheados | 14 |
| `register float accel asm("fr0")` | 14 (ya vive en fr0) |
| `register float accel asm("fr12")` | **22** |
| `register float accel asm("fr13")` | **21** |

El pin sobre `accel` empeora, o sea que —segun la palanca— el reparto es sintoma;
pero la unica diferencia que **no** es de registro es el par INSERT(127)/DELETE(129),
que es la MISMA instruccion (`lis rN, 0.0f@ha`) dos ranuras mas alla. Es decir:
tambien aqui todo es permutacion, y el origen es que el objetivo reparte **cuatro**
registros de direccion (r8..r11, reusando r9 para 20.0f y 0.0f) y nosotros
**cinco** (r7..r11), porque `sched1` nos iza el `lis` del 0.0f doce ranuras.
Sigue en pie todo lo negativo de r36e/r36f/r38/r46.

---

## 7. Lo que dejo, y que hacer con ello

En `scratchpad/r47_cam/`:

| fichero | que hace |
|---|---|
| `cc.py` | saca los cflags reales de `zCamera` de `build.ninja` y absolutiza los `-I` |
| `try.py` | compila **zCamera entero** a un `.o` de scratch con flags extra y lista las funciones que no casan (~50 s). Es lo que midio `-fno-force-addr` |
| `mtry.py` / `mrows.py` | **el instrumento de la ronda**: compila un micro `.cpp` y diffea UNA funcion contra el `zCamera.o` objetivo; `mrows` imprime las filas. **4-5 s** |
| `vsweep.py`, `s1..s7.py`, `tv.py` | bancos de variantes de `TrackCar` y de `TerrainVelocityNoise` (editan una COPIA en scratch, nunca `src/`) |
| `stry.py` / `srows.py` / `sh*.py` | lo mismo con **cabecera sombra** (`-I inc` delante, con el directorio `Utility/` copiado entero) |
| `rtl.py`, `lits.py`, `fnrtl.py`, `fnbytes.py` | volcados RTL de `cc1plus` (`-dj -dr -dS -dN -dc` y `-fsched-verbose-5`) y extraccion por funcion |

**`mtry.py`/`mrows.py` es lo que hay que promover a `scripts/`.** La receta —un
`.cpp` que incluye un solo fichero de la SourceList, compilado con los cflags de
la unidad y diffeado contra el `.o` objetivo— reduce el ciclo de 50 s a 4 s
**sin tocar ninja ni `build/GOWE69/src/`**, que es justo lo que hace falta con
seis agentes en el arbol. Funciona siempre que la funcion no dependa de
inlines que vengan de otro `.cpp` de la unidad; lo comprobe instruccion a
instruccion en los dos casos que use.

## 8. Abierto, por orden de premio

1. **`Update__19TrackCarCameraMoverf` (992 B)** — a **10 filas** con tres pines
   (§4.4), y el racimo A cierra. Lo que falta es que el store de `z` gane el
   desempate del planificador en el racimo de `bScale`. Con `-dS
   -fsched-verbose-5` se ve la fila exacta; no es barrido, es un dato.
2. **`_Storage` (1.156 B)** — §5: falta combinar el pseudo unico de `size()` con
   que sobreviva dentro de `push_back`. Fuera de territorio.
3. **`__static_init` (3.604 B)** — sin vetas pendientes ya (§2). Siguen faltando
   107 rtx y lo unico del tamano correcto es el andamiaje de `expand_vec_init`,
   que GCC 2.95 emite sin guarda.
4. **`Update__8ICEMoverf` (3.868 B)** — no lo toque. Sus 15 filas son un unico
   racimo en `ICEMath.hpp:132-153` + `bMath.hpp:232-241` (`SignedMod` y
   `bClamp`), y la diferencia estructural es que el objetivo **coalesce el
   parametro `a` de `SignedMod` con su valor de retorno en un solo registro**
   (r8: `lwz r8` / `subf r8` / `li r8`) y nosotros usamos dos (r9 y r11). Sigue
   la prohibicion de inventarle locales.
5. **`TerrainVelocityNoise` (1.192 B)** — §6, cuatro registros de direccion
   contra cinco.
