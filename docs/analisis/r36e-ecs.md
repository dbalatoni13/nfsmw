# r36e — zEcstasy: la cantidad fantasma abre `EmitterSystem::Render`, y la mitad del encargo ya estaba al 100 %

Encargo: la cola de `zEcstasy` ordenada por instrucciones distintas, empezando
por `eMotionBlurEffect` («2.188 B a UNA instruccion, la mejor relacion de todo el
proyecto ahora mismo»), luego `eDisplayFrame` (3.300 B a 3) y
`GenerateHorizonFog` (796 B a 2).

**Dos resultados, y el primero es de contabilidad:**

1. **Las TRES primeras de mi cola ya estaban al 100 %.** `eMotionBlurEffect`,
   `eDisplayFrame` y `__static_initialization_and_destruction_0` —10.032 B de la
   cola— son **falsos positivos de `fncmp.py`**, y la causa es la misma en las
   tres. No hay nada que hacer en ellas. §1.
2. **`EmitterSystem::Render` (696 B) pasa de 98,074715 % / 34 filas / 688 B a
   98,78161 % / 4 filas / 696/696 B**, con el TAMANO ya exacto, gracias a la
   **cantidad fantasma** (palanca 2 del brief), que es justo la que el
   diagnostico de la r29/r30 pedia y la unica de las cinco que nadie habia
   probado aqui. Era un muro de seis rondas. §2.

Estado de la unidad, `pctsnap --cmp` sobre las 539 funciones:

    EMPEORAN: ninguna
    MEJORAN: 1 funciones
       +0.707 pp  zEcstasy  Render__13EmitterSystemP5eView  98.0747 -> 98.7816  696 B (+8)

`lcfix.py zEcstasy --check`: «todas las entradas @lc estan al dia». Un solo
fichero tocado, dentro de mi territorio: `src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp`.
**Sin commit.** No he tocado `configure.py`, `config/GOWE69/*` ni `splits.txt`.

**No he cerrado ninguna funcion al 100 %**, asi que no hay `fndiff` al 100 % que
citar: `matched_code` de `zEcstasy` sigue en 140.008 / 145.884 B (5.876 sin
casar). Lo que dejo es un near-miss de 696 B a **dos instrucciones transpuestas**
con el mecanismo escrito y medido.

---

## 1. La cola del encargo estaba mal: `fncmp.py` sobreestima cuando dtk pierde una reubicacion

Las tres primeras filas de mi cola no existen. `report.json` y `fncmp` coinciden
en que a `zEcstasy` le faltan **5.876 B**, y esa cifra es exactamente
`epCalculate` + `UpdatePlatInfo` + `GenerateHorizonFog` + `Render` + `eProject`.
Las tres del encabezado estan al 100 % y `fndiff` lo confirma al primer intento:

    # eMotionBlurEffect__FP5eView                 target=100.0%  ours=100.0%  size=2188/2188
    # eDisplayFrame__Fv                           target=100.0%  ours=100.0%  size=3300/3300
    # __static_initialization_and_destruction_0   target=100.0%  ours=100.0%  size=4544/4544

**La causa, y es una NORMALIZACION QUE LE FALTA A `fncmp`.** La rama
`if (pa is None) != (pb is None): malas += 1` cuenta como instruccion distinta
todo `lis` cuyo `@ha` esta separado de su `@l` por el planificador: dtk no puede
emparejarlos, no emite reubicacion, y deja el valor **ya resuelto**. Los cinco
casos de la unidad, sacados con una copia instrumentada de `fncmp` (esa rama es
la unica que no imprime detalle, por eso `fncmp <unidad> <funcion>` no ensena
nada y parece que no hay diferencia):

| funcion | offset | objetivo | nuestro |
|---|---|---|---|
| `eMotionBlurEffect` | +0D0 | `3D208042` sin reubicacion | `lis r9, SmearSubStageEnable@ha` |
| `eDisplayFrame` | +148/14C/150 | `3D008042`, `3D208043`, `3D608042` | `EnableLODZ@ha`, `DrawCarsReflections@ha`, `EnableMinimalEnvMap@ha` |
| `__static_init...` | +210/2A4/3C4/448/554/620 | `3D60803E`, `C187F474`, ... | `$LC994@ha`, `$LC994@l`, `$LC1000`, `$LC1010` |

Que son la misma instruccion se prueba sin compilar nada. En
`eMotionBlurEffect`, el `lis r9` de +0x0D0 esta en una rama que **salta a
+0x100**, o sea al `addi r9, r9, SmearSubStageEnable@l` que hay alli; para que el
codigo funcione, el `lis` TIENE que valer `SmearSubStageEnable@ha`, y `0x8042` lo
es. Lo mismo en `__static_init`: `3D60803E` + `C187F474` da
`0x803E0000 − 0x0B8C = 0x803DF474`, y `@ha` de `0x803DF474` es `0x803E`. En
`eDisplayFrame` los tres son `lis` de globales `int` cuyo `@l` va tres o cuatro
ranuras mas abajo.

**Propuesta (fuera de mi territorio, `scripts/fncmp.py`, y ademas otro agente lo
esta editando esta misma ronda, asi que no lo toco):** cuando un lado tenga
reubicacion `@ha` (tipo 6) y el otro no, comparar el `lis` **resuelto** —tomar la
direccion del simbolo del lado que si la trae y calcular su `@ha`— en vez de
contarlo como instruccion distinta. Con eso `fncmp` sobre `zEcstasy` daria las
cinco reales en vez de ocho. Coste del fallo esta ronda: la cola que se me dio
tenia **10.032 B de trabajo inexistente en las tres primeras filas**, que es
donde el encargo mandaba empezar.

Comprobacion de que la correccion es real y no una excusa: despues de mi cambio,
`fncmp` lista exactamente las cinco y suma **5.876 B**, la misma cifra que
`report.json`.

---

## 2. `EmitterSystem::Render`: 34 filas -> 4, y el tamano ya exacto

### 2.1 El diagnostico ya estaba escrito; faltaba la palanca

La r29 dejo el censo hecho en la propia fuente y era correcto:

> objetivo `r14..r18, r20..r31` -> DIECISIETE registros + 1 derramado ·
> nuestro `r14..r31` -> DIECIOCHO registros, 0 derrames. Los DOS tienen 18
> valores de larga vida. Al objetivo le queda un HUECO en r19 (...) **hay que
> conseguir que un pseudo NO QUEPA**.

Leido con la palanca 2 del brief en la mano, eso dice literalmente: **al objetivo
le sobra un rango de vida INVISIBLE que ocupa r19**. 19 rangos para 18 registros,
y por eso `reload` derrama el centinela `&elist` a `0xc8(r1)` con el
`stw r0`/`lwz r0` consecutivos que nos faltaban, mas los 8 B de marco.

La r36b probo doce colocaciones de la barrera selectiva y la r36d diez pines, y
la fuente concluia «con esto las CINCO palancas del brief estan medidas contra
esta funcion». **No lo estaban: faltaba la cantidad fantasma**, que es la unica
que fabrica un rango de vida sin emitir un byte.

### 2.2 La medida

```c
    register int e36guard asm("r19");
    __asm__("" : "=r"(e36guard));
    ...
    __asm__("" : "+r"(num_textures) : "r"(e36guard));
    total_num_textures = GetNumParticleTextures();
```

| variante | fuzzy | filas | eq | tamano |
|---|---|---|---|---|
| base | 98,074715 | 34 | 140 | **688**/696 |
| **fantasma r19** | 99,655174 | **9** | 165 | **696/696** |
| fantasma r19 + barrera de ranura | 98,78161 | **4** | **171** | **696/696** |

El fantasma solo ya arregla **todo el bloque estructural**: el marco (0x120 ->
0x128), el `addi r0`/`stw`/`lwz`, y el corrimiento entero de r14..r19 (`lis
r14`/`addi r15,r16,r17`/`addi r18`). De 34 filas quedan 9.

**El registro NO es un empate, es EL registro.** Barrido de los 18 salvados,
todos con el mismo fantasma y la misma barrera de ranura:

| reg | r19 | r18 | r20 | r17 | r16 | r15 | r14 | r21 | r22 | r23 | ... | r30 | r31 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| filas | **4** | 6 | 7 | 8 | 10 | 12 | 14 | 12 | 15 | 22 | ... | 53 | 35 (688 B) |

La curva es limpia a los dos lados de r19 y solo r31 pierde el tamano. Esto
contrasta con la regla que la r36c/r36d dieron para el **pin** («el registro no
importa, lo que cuenta es sacar el pseudo del reparto»): **para la cantidad
fantasma el registro SI importa, porque lo que se elige es a quien se le quita
el hueco.** Regla nueva y reutilizable.

Tambien medido: el `asm` de cierre **tiene que tocar un valor vivo**, tal como
dice el brief. Con `total_num_textures` (que aun no esta asignado ahi) el par
entero desaparece por DCE y sale **el objeto de la base, 34 filas y 688 B** —
identico, que es la firma de «me lo ha comido DCE», no de «no funciona».

### 2.3 Las 9 filas que quedaban: barrera de ranura, y `.lreg` que lo explica

Las 9 son **una sola rotacion f9/f8/f7** en los tres primeros componentes del
`bScale` inline de `xbasis *= world_size`. `regmap --all` dice **IDENTICO** (39
locales, 0 con registro distinto), o sea que son **temporales del compilador** y
no hay pin posible. La barrera de ranura del brief si llega:

```c
    asm("" : "+m"(xbasis));
    xbasis *= world_size;
```

`.lreg` con y sin ella, los cuatro pseudos del `bScale` de `xbasis`:

    sin barrera   223,225,227,229  live 12,12,12,12  ->  f8, f7, f9, f10
    con barrera   223,225,227,229  live 10,10,12,12  ->  f9, f8, f7, f10   (= objetivo)

Acorta en 2 el rango de los dos primeros y con eso `local_alloc` los reparte en
el orden del objetivo. **Tiene que ir DETRAS de la barrera de `world_size`**:
delante da 9; y `"+m"(ybasis)`, `"+m"(vposition)` o anadir `: : "r0"` dan lo
mismo, o sea que **lo que cuenta es la ranura, no el operando** — exactamente lo
que dice el brief de la palanca 4.

### 2.4 Lo que queda: 4 filas, y es un muro con numeros

    objetivo   lfs f6,0xc(r30) · lfs f9,0x98(r1) · ori r24,r24,2 · lfs f8 · lfs f7
    nuestro    lfs f6,0xc(r30) · ori r24,r24,2 · lfs f9,0x98(r1) · lfs f8 · lfs f7

Una transposicion. El `.sched2` (`-dR -fsched-verbose=5`, bloque basico 18,
despues de reload) la explica entera:

    t=12  858  %6=[%30+0xc]        <- lfs f6, world_size
    t=13  ready={861}              <- asm("" : : "f"(world_size))
    t=14  ready={977, 863}         <- el `ori` y la barrera de ranura
    t=15  ready={878,881,884,...}  <- las OCHO cargas, todas de golpe

**Los dos `asm` gastan un ciclo cada uno en la columna `no-unit` y van
serializados entre si.** Las cargas dependen de 863, asi que no salen antes de
t=15, y el `ori` sale en t=14 pase lo que pase. Para casar harian falta las dos
cosas (registros y horario) con **un solo ciclo de `asm`**, y fundir los dos en
uno da 27 filas / 84,99 % — el de `world_size` hace su trabajo en `sched1`, no
aqui, y quitarlo cuesta 57 filas.

Negativos de esta parte, todos a 4 filas o peor (~130 medidas): 8 formas del
`asm` de ranura (`"=m"`, con entrada `"f"(world_size)`, con `"r"(particle)`,
clobbers `cc`/`r0`/`r11`, `"+m"` doble), 6 sitios, 8 rellenos solo de registro
(`"+r"(e36guard)`, `"+r"(num_textures)`, `"+f"(world_size)`...), 5 fusiones de
los dos `asm`, 4 sitios del `sprite_hack_flags |= 2` (**la fuente no lo mueve**:
antes de `xbasis`, antes de la ranura o entre `xbasis` e `ybasis` dan los mismos
4; antes del pin o de `eMulVector` dan 32), 9 registros del pin de `world_size`
(fr6 es el mejor; sin pin 31), fantasmas flotantes en el bloque (18 filas y
**+4 B**) y `asm volatile` en cuatro colocaciones.

---

## 3. `UpdatePlatInfo` (2.044 B, 28 filas): el eje «a quien» con ENTRADA, agotado en 132 medidas

`regmap --all` da el diagnostico exacto y limpio, **un ciclo de 3 y nada mas**
(51 locales iguales, 0 de ambito equivocado):

    diffuse_rng_a   original f5   nuestro f1
    envmap_min_g    original f1   nuestro f31
    envmap_max_g    original f31  nuestro f5

Con `alloc.py` sobre el `.greg` los tres se identifican al pseudo y la formula
reproduce el orden del compilador:

| pseudo | variable | n_refs | live | pri | reg |
|---|---|---|---|---|---|
| 165 | `envmap_power` | 14 | 418 | 1004 | f2 (ok) |
| 115 | `diffuse_rng_a` | 10 | 358 | 837 | f1 (objetivo f5) |
| 191 | `envmap_min_g` | 7 | 317 | 441 | f31 (objetivo f1) |
| 202 | `envmap_max_g` | 7 | 328 | 426 | f5 (objetivo f31) |

**El eje del brief funciona y esta calibrado**: `__asm__("" : "+f"(envmap_power)
: "f"(envmap_min_g))` x2 sube `envmap_min_g` de 7 a 9 refs, su prioridad de 441 a
851, lo adelanta de la posicion #108 a la #99 **y le da f1, el registro del
objetivo**. Pero la cascada estropea los otros dos (115 se va a f8, 202 se queda
en f5) y la funcion pasa de 28 a 60 filas.

Barrido completo del eje, que es lo que el encargo pedia que no estuviera hecho:

| barrido | medidas | resultado |
|---|---|---|
| `asm("" : "+f"(envmap_power) : "f"(V))` x1/x2/x3 sobre las **29 locales float** | 88 | **ninguna mejora**; x1 es siempre IDENTICO a la base (no cruza el escalon de `floor_log2`) |
| pin `fr5` de `envmap_min_scale` x pines de `diffuse_rng_a` (fr5/fr1/fr31) x el eje | 14 | mejor 32; `pin rng_a fr5` pierde 4 B (2.040), el falso positivo de siempre |
| mover el calculo de `diffuse_rng_a` debajo del bloque envmap (5 sitios + declaracion partida + barrera) | 7 | **objeto IDENTICO en 6 de 7**; reconfirma «mover sentencias dentro de un mismo bloque basico no cambia nada» |
| **cantidad fantasma flotante** `register float asm("frN")` con el rango cubriendo la funcion, N = f0..f13 y f24..f31 | 23 | **las 23 emiten +8 B** (2.052) y la mejor da 153 filas |

**Por que el eje no basta aqui, con numero.** El objetivo tiene `diffuse_rng_a`
en f5 y `envmap_min_scale` TAMBIEN en f5 (el DWARF los da a los dos), o sea que
en su RTL post-`sched1` los dos rangos **no se solapan**. En el nuestro si: en la
lista de alloc, cuando le toca al pseudo 115 estan ocupados f13..f2 **incluido
f5**, y por eso se lleva f1. Mientras `diffuse_rng_a` siga viva por encima del
bloque `envmap_min_*` en la RTL que ve `global_alloc`, f5 le esta prohibido y
ningun reparto de refs lo cambia. **Eso es lo que hay que atacar en la proxima
ronda: no el orden de los allocnos, sino el solape.** Las 7 formas de mover la
sentencia no lo tocan (`sched1` la recoloca igual); haria falta algo que ate el
`fsubs` por debajo del bloque envmap sin emitir bytes.

La cantidad fantasma **no vale aqui** y la razon es concreta: el `asm` de cierre
tiene que atar una variable de vida larga (`"+f"(diffuse_min_r)`) y eso si emite
un `fmr`. La palanca solo es gratis cuando hay un valor de un solo uso o una
memoria a la que agarrarse — en `EmitterSystem` lo habia (`"+r"(num_textures)`),
aqui no.

---

## 4. `GenerateHorizonFog` (796 B, 2 filas): idea nueva, negativo nuevo

El muro esta documentado con numeros desde la r30: `srawi` (el `i/2`) y `andi.`
(el `i&1`) se emiten en el mismo ciclo, `rank_for_schedule` mira INSN_PRIORITY
antes que nada, y prio(216)=12 contra prio(231)=2. La fuente no lo mueve (16
formas), ni el permutador ciego (445 variantes), ni la barrera de ranura (10
colocaciones, las diez emiten bytes).

Idea nueva de esta ronda: **subir la prioridad del `andi.` colgandole una cadena
de dependientes de coste cero**, ahora que sabemos que un `asm` no volatil de
registro no emite un byte. Materializando `int odd = i & 1;` y colgandole
1/2/4/8 `asm("" : "+r"(e36g))` encadenados sobre un fantasma en r11:

| variante | fuzzy | filas | tamano |
|---|---|---|---|
| base | 98,994970 | 2 | 796/796 |
| solo `int odd = i & 1;` (control) | 98,994970 | 2 | **objeto identico** |
| cadena de 1 / 2 / 4 / 8 `asm` | 97,462310 | 24 | 796/796 (las cuatro) |

**Negativo, y el dato que vale para todo el arbol: la longitud de la cadena NO
cambia nada** —1, 2, 4 y 8 dan el MISMO objeto—, o sea que **una cadena de `asm`
no propaga INSN_PRIORITY hacia atras** en `sched2`. Y confirma por otra via que
el constructo es gratis: 796 B en las cuatro. El control reconfirma la veda de la
r24: nombrar `odd` da objeto identico.

---

## 5. Reglas nuevas que se lleva el arbol

1. **`fncmp.py` sobreestima cuando el planificador separa un `@ha` de su `@l`**:
   dtk no emite reubicacion y la rama `(pa is None) != (pb is None)` lo cuenta
   como instruccion distinta **sin imprimir detalle**. Tres funciones y 10.032 B
   de trabajo fantasma solo en `zEcstasy`. Contrastar SIEMPRE contra
   `matched_code` de `report.json` antes de repartir una cola: aqui las dos
   cifras cuadran al byte (5.876) en cuanto se quitan los falsos positivos.
2. **En la cantidad fantasma el REGISTRO importa** (curva limpia r19 -> 4 filas,
   r18 6, r20 7, r17 8, r16 10, r15 12, r14 14, r21 12 ... r30 53). Es lo
   contrario del pin, donde cuatro registros distintos dan el mismo objeto.
3. **El `asm` de cierre del fantasma tiene que atar algo gratis.** Si solo hay
   variables de vida larga, el par cuesta instrucciones y la palanca no sirve
   (medido: 23 fantasmas flotantes en `UpdatePlatInfo`, +8 B los 23).
4. **Cada `asm` no volatil gasta un ciclo entero en la columna `no-unit` de
   `sched2`, y dos `asm` seguidos van serializados.** Se lee en el
   `.sched2`. Cuando hagan falta dos palancas `asm` en la misma sentencia, el
   horario del objetivo puede quedar fuera de alcance por construccion — es lo
   que deja `EmitterSystem::Render` a dos instrucciones.
5. **Una cadena de `asm` no propaga INSN_PRIORITY**: 1, 2, 4 y 8 eslabones dan el
   mismo objeto. Cierra un frente que parecia abierto contra los empates de
   `rank_for_schedule`.
6. `-dR -fsched-verbose=5` funciona con `ngccc` y deja el `.sched2` (con listas
   de listos y la visualizacion por ciclo y unidad) en `%TEMP%`; **termina con un
   «Internal compiler error» despues de volcar**, asi que hay que ignorar el
   codigo de salida y quedarse con el fichero.

## 6. Metodo y herramientas

- **Banco por fichero SIN tocar el arbol** (`e36_var.py` del scratchpad): copia
  el `.cpp` al scratchpad, aplica los reemplazos y monta un banco cuyo ultimo
  `#include` es la copia por ruta absoluta. Cero riesgo para los vecinos —esta
  ronda habia agentes editando `TrackCop.cpp`, `stream.cpp` y `WRoadNetwork.cpp`
  a la vez. Tiempos: `eLightE.cpp` 2,6 s, `EmitterSystem.cpp` 8-16 s,
  `EcstasyEx.cpp` 21 s (necesita el prefijo de 10 ficheros; con menos falla por
  `testc0`, que vive en `EcstasyE.cpp:3371`).
- `bench.py` retrocede de mas en `zEcstasy`: da el prefijo de 12 ficheros (10,2 s)
  cuando el de 1 basta (0,8 s), porque el unico tamano que no cuadra es
  `__static_initialization_and_destruction_0`, que el propio script declara no
  fiable. **Propuesta (fuera de mi territorio):** excluir esa funcion del
  criterio de parada.
- `EmitterSystem.cpp` suelto necesita, ademas del prefacio de la r30
  (`Profiler.hpp` + `IPlayer.h`), **`-I src/Speed/Indep/Src/Ecstasy`**; y
  `EcstasyEx.cpp`, `-I src/Speed/GameCube/Src/Ecstasy`.
- `alloc.py` sobre `<base>.lreg`/`.greg` da la tabla de allocnos ya ordenada por
  `allocno_compare` y con `pri`; cruzada con `regmap --all` (que da
  variable -> registro duro) identifica **cada pseudo con su variable de fuente**
  sin adivinar. Es lo que convirtio `UpdatePlatInfo` en aritmetica.

## 7. Lo que NO he tocado

`epCalculateLocalDirectionalPOS16` — el encargo la daba por acotada con 447
medidas y no la he tocado. `eProject` sigue en 93,970146 % / 13 filas como la
dejo la r36d. No he repetido ninguna veda: ni las 165+56 barreras de la r36c en
`epCalculate`, ni el permutador ciego de `GenerateHorizonFog`, ni las 12 barreras
de la r36b y los 10 pines de la r36d en `Render`.

**Total de medidas nuevas de la ronda: ~280** (132 en `UpdatePlatInfo`, ~130 en
`EmitterSystem::Render`, 6 en `GenerateHorizonFog`, mas el triaje).
