# r62 - world (zWorld, zWorld2)

**Titular. `dolwhere zWorld` baja de 29.551 a 24.884 B (-4.667 B, -15,8 %) por
DOS lineas de fuente, y el hallazgo que las justifica es de metodo: en zWorld
habia dos funciones con el TAMANO mal en direcciones opuestas (-4 y +4) que se
compensaban, de modo que `linkdelta` decia `.text +0` y nadie las habia contado
como un problema de colocacion. Entre las dos habia un tramo de `.text` corrido
-4 B que `dolwhere` veia como UN SOLO bloque de 6.929 B distintos.**

Ninguna unidad promociona (las dos siguen lejos: zWorld tiene ~12,8 kB de
`.rodata` desordenada y 53 funciones descolocadas; zWorld2, 255 de 358
descolocadas). **Cero correcciones de `lcfix` pendientes, cero entradas
venenosas, ninguna propuesta para `splits.txt` / `symbols.txt` / `keep.lst` /
`configure.py`.**

De paso, un aviso de higiene: `WRoadNetwork.cpp` tenia **cuatro bytes latin-1
sueltos** (0xAB y 0xBB, las comillas angulares) metidos en un comentario de la
r61 -- justo el fallo de la regla 7. Corregidos; el `.o` no se mueve.

---

## 1. El hallazgo: dos errores de tamano que se tapan mutuamente

`fncmp zWorld` da cuatro funciones, y DOS de ellas son de **tamano**:

```
   2908  RenderFlaresOnCar__13CarRenderInfo...      tamano (2908 objetivo / 2912 nuestro)   +4
    876  UpdateWheelYRenderOffset__13CarRenderInfo  tamano ( 876 objetivo /  872 nuestro)   -4
```

Los cuatro bytes se cancelan, asi que `linkdelta zWorld` da `.text +0` y las dos
llevaban rondas catalogadas como "near-miss de reparto". Pero **el `.text` entre
ellas queda corrido -4 B**, y eso es lo que `dolwhere` estaba contando:

```
6929 B  802CA12F  UpdateWheelYRenderOffset +0x177
```

`UpdateWheelYRenderOffset` empieza en 0x802C9FB8 (tam. 0x36C) y
`RenderFlaresOnCar` en 0x802CB51C (tam. 0xB5C): el bloque va de
`UWYRO+0x177` a 0x802CBC40 = `RenderFlaresOnCar+0x724`, o sea desde donde
empieza a faltar el byte hasta donde sobra. **6.929 B de DOL por 4 bytes.**

Consecuencia operativa, y es la que hay que apuntar en el catalogo:

> **Arreglar UNA SOLA de las dos EMPEORA el DOL**, porque rompe el total de
> `.text` de la unidad y descoloca todo lo que va detras hasta el final del TU.
> Hay que corregirlas **a la vez**. Esa es la razon por la que la r46 midio la
> variante buena de `RenderFlaresOnCar` (2.908 B exactos), la anoto y **la
> revirtio**: sola no valia nada.

### 1.1 Lo aplicado

**`UpdateWheelYRenderOffset` (`CarRender.cpp:2109`)** -- la veda r36e decia que
`extern const float lbl_8040AA84` en el `width` da los 876 B exactos. Se aplica
con **`lbl_8040AD04`** en vez de `lbl_8040AA84`, porque:

* `lbl_8040AA84` **no lo define nadie**: declararlo dejaria zWorld sin enlazar,
  y emitirlo a mano cuesta 4 B de `.rodata` (`linkdelta` pasaria a `rodata+4`).
* `lbl_8040AD04` **ya esta definido** en el propio `CarRender.cpp` (el `asm()`
  de `.rodata` del final, valor `0x00000000`) y ya se usa en `RenderFlaresOnCar`.
* Coste: la `lfs` carga de 0x8040AD04 y no de 0x8040AA84 -- el `lis` casa igual
  (los dos `@ha` son 0x8041) y solo cambia el desplazamiento de una insn.

```c
extern "C" const float lbl_8040AD04;      // delante de la funcion
...
if (model_width > lbl_8040AD04 && desired_width > lbl_8040AD04) {
```

**`RenderFlaresOnCar` (`CarRender.cpp:2686`)** -- la tercera veda de la r46,
aplicada tal cual:

```c
float base_headlight_intensity = lbl_8040AD04;   // era 0.0f
```

### 1.2 El barrido que hay detras (banco `bench.py`, 8-12 s por variante)

`UpdateWheelYRenderOffset`, base 872 B / 99,3836 % / 7 filas:

| variante | B | % | filas |
|---|---:|---:|---:|
| U1 `lbl_8040AA84` en las DOS mitades del `width` | **876** | 98,9269 | 8 |
| **U7 `lbl_8040AD04` en las dos mitades del `width`** | **876** | **98,9269** | **8** |
| U8 `lbl_8040AD4C` en las dos mitades del `width` | **876** | 98,9269 | 8 |
| U4 en las dos mitades del `radius` | **876** | 98,8813 | 10 |
| U5 en el ternario `<= 0.0f` | **876** | 98,7443 | 14 |
| U9 en el `width` Y en el `radius` | **876** | 98,7900 | 12 |
| U2 solo en `model_width >` | 884 | 96,1005 | 46 |
| U3 solo en `desired_width >` | 884 | 96,1233 | 46 |
| U6 solo en `model_radius >` | 884 | 96,3059 | 48 |

**Regla que sale**: los DOS operandos del mismo `&&` o nada -- con uno solo el
otro `0.0f` sigue compartiendo `@ha` y la funcion crece 12 B.

`RenderFlaresOnCar`, base 2912 B / 98,6176 % / 18 filas:

| variante | B | % | filas |
|---|---:|---:|---:|
| **R1 `base_headlight_intensity = lbl_8040AD04`** | **2908** | 98,0536 | 62 |
| R2 `base_brakelight_intensity = lbl_8040AD04` | 2908 | 98,0536 | 62 |
| R3 las dos | 2908 | 98,0536 | 62 |
| RB R1 declarado detras de `is_traffic_car` | 2908 | 98,0536 | 62 |
| R4 `float zzz = lbl; asm("" : : "f"(zzz));` antes del bucle | 2916 | 96,6451 | 96 |
| R5 `asm("" : : "f"(lbl_8040AD04));` | 2920 | 97,8060 | 68 |
| RH izado manual del 0.0f a un FPR vivo en el bucle | 2928 | 97,2435 | 120 |
| RI RH + los dos `eRenderLightFlare` | 2912 | 96,4319 | 172 |

**Se cambia 18 filas por 62 filas a proposito.** Las dos funciones ya contaban
como "codigo distinto" en `fncmp` antes y despues, asi que **`matched` no se
mueve ni un byte** (`nfsmw-matched-code-todo-o-nada`) y `linked` gana 4.667 B.

### 1.3 La medida final de zWorld

```
build_direct  x3   ->  sha1  2b35b5c0b5a0994e6af25433d1e48126397fcda8  (identico las 3)
fncmp   zWorld     ->  4 de 582 (las MISMAS cuatro; las dos de tamano ya al byte)
linkdelta zWorld   ->  .text +0   resto IGUAL      (sin cambio)
dolwhere zWorld    ->  29.551 B  ->  24.884 B      (-4.667)
trypromo zWorld    ->  DOL ROTO (60faea1e56a3)
lcfix --check zWorld / zWorld2 -> todas las entradas @lc al dia
```

El reparto del deficit restante, para quien recoja:

```
10.617 B  $LC533 +0xC        \
 1.914 B  $LC227 +0xD         |  ORDEN de las cadenas de .rodata: 12.774 B
   243 B  $LC57  +0x0        /
 1.896 B  RenderFlaresOnCar +0x1EC   (las 62 filas de reparto)
 1.700 B  __as__Q36Attrib3Gen5tires  (funcion PERMUTADA, textorder)
 1.106 B  CarSlotIDNames +0x6   \
   226 B  CarEffectParameters    |  punteros de .data a esas cadenas: ~1,4 kB
    ~10 B CarMemoryInfoTable    /
   472 B  OnGrowRequest__...Vector2ZP17VehicleRenderConni16Ui (permutada)
   305 B  UpdateWheelYRenderOffset +0x177  (las 8 filas)
```

O sea: **de los 24.884 B que quedan, ~14.200 son UN solo problema** -- el orden
del pool de cadenas -- y arrastra la `.data` detras. `rodorden zWorld` da 637
cadenas en el objetivo contra **669** nuestras, 146 fuera de secuencia, y el
bulto es que nuestro bloque `nue[391:518]` (128 cadenas: el `asm()` de `.rodata`
escrito a mano en `zWorld.cpp:121-260`) va donde el objetivo tiene `obj[492:546]`.

---

## 2. El negativo mas util de la ronda: la cantidad fantasma SI reproduce el mecanismo del objetivo, y cuesta 8 B

El `fndiff` de `RenderFlaresOnCar` con R1 dice exactamente que le falta:

* al **objetivo** le sobran dos insns que nosotros no tenemos: `mfcr r9` +
  `stw r9,0x1c(r1)` y su `lwz r0,0x1c(r1)` -- **el objetivo VUELCA CR A PILA**;
* a **nosotros** nos sobran dos `lis lbl_8040AD04@ha` (filas 450 y 598/600) que
  el objetivo no tiene, porque sus tres cargas comparten el `@ha` de larga vida
  que nace en la fila 123 y vive hasta la 693 en **r16**.

Leido junto: **el objetivo no tiene ningun preservado libre -- se lo come el
`@ha` -- y por eso vuelca CR; nosotros si lo tenemos y guardamos CR en r14.**
Es la misma lectura que la r36e hizo para `UpdateWheelYRenderOffset` ("el banco
r14..r31 esta entero en las dos columnas"), pero aqui con el sintoma al reves.

Probado ocupar ese preservado con una **cantidad fantasma**
(`register int _g asm("rN"); asm("" : "=r"(_g));` detras de `is_traffic_car` y
`asm("" : : "r"(_g));` detras del bucle de flares):

| variante | B | % | filas |
|---|---:|---:|---:|
| base | 2912 | 98,6176 | 18 |
| **RJ = R1 + fantasma r16** | **2916** | **98,6644** | **34** |
| RM = R1 + fantasma r14 | 2916 | 98,5887 | 45 |
| RK = fantasma r16 sin R1 | 2920 | 97,6314 | 92 |
| RL = fantasma r14 sin R1 | 2920 | 97,6726 | 86 |
| RN = fantasma r30 | 2920 | 97,0124 | 171 |
| RO = fantasma r17 | 2920 | 97,6107 | 94 |

**RJ es el mejor porcentaje que ha tenido nunca esta funcion** (98,6644 % contra
98,6176 % de la base, con 19 rondas de historial) y **reproduce el vuelco de CR
del objetivo**: la fantasma hace exactamente lo que se le pide. Lo que no hace
es borrar los dos `lis` de mas, y por eso sale +8 B. **Quien borre esos dos
`lis` con la fantasma puesta cierra la funcion.** Todo escrito en
`CarRender.cpp` junto a la funcion; `previo.py RenderFlaresOnCar` lo encuentra.

---

## 3. zWorld2 -- `HolePunchAvoidables`: la tercera rama EVALUADA, y por que no se movia

Estado, y conviene tenerlo delante: **la funcion esta a 99,97315 %, 2.980/2.980 B,
y sus UNICAS CUATRO diferencias son el intercambio cr2/cr3** (filas 176, 178,
241 y 645). Vale **2.980 B de `matched`** de una sola vez.

### 3.1 EL HALLAZGO: el orden del preencabezado lo pone `sched1`, no `loop.c`

La r61 dejo escrita la lista de los ocho movables entre `def(901)` y `def(505)`
y la tercera rama: "sacar UNO de los ocho de la ventana adelantandolo por
delante del test que define 901, **y como loop.c iza en el orden en que aparecen
en el CUERPO, eso se consigue reordenando el fuente**".

**Esa premisa es falsa, y por eso la rama no se movia.** El volcado `.lreg` es
**posterior a `sched1`** (`-fschedule-insns` corre antes del reparto), asi que la
cadena de doce insns del preencabezado es **una lista YA PLANIFICADA**. Se
comprueba sin ambiguedad con los uids: el orden del preencabezado

```
  ..., def(334), [0,2f $LC435], def(901), [-6,0f], +8, +184, +232, +248, +216,
  high IHandle, high TRAILER, def(505), +344, +424, +432, +440, +392, +448, +456,
  high $LC423, NOTE_INSN_LOOP_BEG
```

no es ni el orden de aparicion en el cuerpo (uids del primer uso: `+8`@574,
`+184`@598, `+232`@625, `IHandle`@791, `TRAILER`@1007, `+216`@1056, `+248`@1689,
`-6,0f`@2507) ni el de los uids de definicion (3325, el `-6,0f`, es **el mas
alto de los ocho y va el PRIMERO**). Es el orden que deja el planificador de
lista, que con prioridades empatadas desempata por `INSN_LUID`.

**Cinco reordenaciones de fuente medidas, y cuatro son IDENTICAS a la base**
(334=494, 901=492, 505=483), banco de 10 s por variante con
`rtldump --file ... -dl` y sin compilar zWorld2:

| variante | 334 / 901 / 505 |
|---|---|
| base | 494 / 492 / 483 |
| E1 `GetDimension` delante del test de `elevation` | 494 / 492 / 483 |
| E2 `QueryInterface` delante del test de `elevation` | 495 / 493 / 484 (+1 insn) |
| E3 `GetLinearVelocity` detras de `GetDimension` | 494 / 492 / 483 |
| E4 `avoidable_forward` declarado antes que `_right` | 494 / 492 / 483 |
| E5 `GetLinearVelocity` delante del test de `elevation` | 494 / 492 / **482** |

### 3.2 El modelo, cerrado en forma explicita y CONFIRMADO por experimento

Con `T = live(505)`, `d1 = live(901) - T` y `d2 = live(334) - live(901)` (base
`d1 = 9`, `d2 = 2`), el empate se rompe si y solo si

```
int(30000 / (T + d1))  >  int(30000 / (T + d1 + d2))
```

Con `d1=9, d2=2` eso da `T` en **{473, 474, 481, 482, 490, 491}** -- exactamente
la lista de longitudes que la r47 habia dado a mano.

**Confirmado experimentalmente.** La variante **F2** -- sacar la declaracion de
`his_velocity` fuera del bucle y poner `asm("" : : "r"(&his_velocity));` justo
delante del `for` -- da `T = 482` y las prioridades salen **62 / 61 / 60**, que
es **el orden del objetivo** (505 -> cr4, 901 -> cr3, 334 -> cr2).
**Pero cuesta +8 B (2.988/2.980) y descoloca el prologo entero** (el `mfcr` y
las siete `lis` del pool). Es un negativo -- **pero un negativo que prueba el
modelo: quien mueva `T` a 482 por CERO bytes cierra la funcion.**

**Lo que NO mueve `T`** (las siete identicas a la base, 494/492/483):

| variante | |
|---|---|
| F1 | lo mismo que F2 con `avoidable_dimension` |
| F3 | con `his_vehicle` |
| F4 | con `tranform` |
| F5 | con `closing_speed` |
| F2b | solo sacar la declaracion de `his_velocity`, sin `asm` |
| F6 | el `asm` con restriccion `"=m"` (no materializa el `addi`) |
| F8 | el `asm "r"` puesto detras de `is_drag` (regla r48: cae **delante** de los movables y no cuenta) |

**Y la otra palanca de `T`, tambien negativa**: izar `my_extent` fuera del bucle
(**G1**) mueve `T` a 479 -- pero **cambia `d1` y `d2` a 8 y 4**, con lo que 901 y
334 vuelven a empatar (61 y 61), y ademas rompe el control de tamano:
**2.984/2.980 y 85 filas**.

### 3.3 Lo que hereda la ronda que viene

La rama viva es **`sched1`**, no la fuente: hay que bajar el `INSN_LUID` de uno
de los ocho movables por debajo del de la comparacion de `is_drag` (o subir el
de esa comparacion), y eso se ataca con **el volcado ANTERIOR a sched1**, no con
el `.lreg`. El banco esta escrito y cuesta 10 s por variante:
`scratchpad/world62/hp2.py` (saca las tres CC del `.lreg` con su `n_refs`,
`live` y prioridad, sin compilar la unidad).

Todo esto queda en `WRoadNetwork.cpp` junto a la funcion; `previo.py HolePunch`
lo encuentra (bloque L1971, ahora con `r62`).

`InitAtSegment` (816 B, 12 filas, las doce el mismo swap r10/r11) **no se ha
tocado**: la r61 dejo su rama de ocupacion refutada con control byte a byte y no
habia palanca nueva que probar.

---

## 4. Lo que NO he hecho, y por que

### 4.1 Los dos hunks de upstream de `CarLoader.cpp`: el informe r61b se queda corto en un punto

**Los dos literales YA LOS EMITIMOS.** `zWorld.cpp:255` tiene
`.asciz "SpongeAllocation"` y `zWorld.cpp:257`
`.asciz "CarLoaderDefrag but with a really long debug name!!"`, los dos dentro
del `asm(".section .rodata")` que va **detras de los 29 `#include`**. Asi que
los hunks **no anaden 52 + 17 B de dato**: lo que hacen es **mover esas dos
cadenas del bloque de la cola al pool del compilador, en el punto de
`CarLoader.cpp`**, que es donde el objetivo las tiene. El observable del informe
r61b ("`zWorld.o` debe contener la secuencia exacta ...") sigue valiendo; el que
NO vale es "52 B de `.rodata`".

Van en la direccion correcta del problema de la seccion 1 (el orden del pool),
pero **renumeran los `$LC`** y por tanto **exigen `lcfix.py`**, que la regla 2
me prohibe correr. Se dejan como propuesta:

* `CarLoader.cpp:2123`: `bMalloc(128, "CarLoaderDefrag but with a really long debug name!!", 0, ...)` + quitar el `.asciz` de `zWorld.cpp:257`.
* `CarLoader.cpp:634`: el bucle de sponge con `"SpongeAllocation"` + quitar el `.asciz` de `zWorld.cpp:255`.
* Despues, **obligatorio**: `python scripts/lcfix.py zWorld`. Las 254 entradas
  `zWorld.o:*` de `keep.lst` son de este objeto y de ningun otro, asi que el
  desfase **no envenena a ninguna otra unidad**; pero deja zWorld sin enlazar
  bien hasta que se corra.
* Control de no-regresion: `SetMemoryPoolSize__9CarLoaderi` tiene que seguir en
  **304 B / 97,36842 %** y `DefragmentPool__9CarLoader` en **684 B / 23 filas**.

### 4.2 `zWorld2 bss+64`: lo cubre el paquete P3-A, no lo duplico

`docs/analisis/r62-p3.md` ya trae la raya
(`zWorld2.cpp .bss start:0x804AA560 end:0x804AAA88`), la entrada de `keep.lst`
que se parte (`zWorld.o:gap_07_804AA55C_bss` se queda con 4 B + una NUEVA
`zWorld2.o:<pad_ en 0x804AA560>` de 56 B) y el aviso de que **el nombre del
hueco hay que LEERLO del `.o` re-extraido**. Su control dice que `zWorld2` pasa
de `bss+64` a `bss+0` de tamano y `-56` de contenido. **No propongo nada que se
solape con eso.**

### 4.3 `DefragmentPool` y `SetMemoryPoolSize`

No tocadas. La r61 dejo `DefragmentPool` con el criterio de descarte sin
compilar (`rtldump -dl`, 4 s) y la conclusion escrita: hace falta una forma que
acorte `live(222)` **sin anadir insns**, y las cuatro reordenaciones puras son
neutras al byte. `SetMemoryPoolSize` tiene diez vedas de r36e + tres de r48/r49
sobre el mismo desempate de `sched2` y ninguna palanca nueva.

---

## 5. Estado final, sellado

| unidad | sha1 del `.o` (3 compilaciones) | `fncmp` | `linkdelta` | `dolwhere` | `trypromo` |
|---|---|---|---|---:|---|
| zWorld | `2b35b5c0b5a0994e6af25433d1e48126397fcda8` | 4 de 582 | `.text +0  resto IGUAL` | **24.884** (era 29.551) | DOL ROTO |
| zWorld2 | `3df7abca402210c486032811393a31f15f07b167` | 2 de 357 | `.text +0  bss+64` | *secciones descuadradas* | DOL ROTO |

`zWorld2` sale con **el `.text` byte a byte identico al de la r61**
(`96f30384...`): el sha1 cambia solo porque el bloque de comentario mueve
`.debug_line`. Comprobado con `fncmp` (las mismas 2 funciones, las mismas 4 y 12
insns) y `linkdelta`.

**Aviso sobre `build_direct.py` y el sello.** El objeto que cuenta esta en
`build/GOWE69/src/.../zWorld.o`. En el arbol hay ademas un
`build/GOWE69/obj/.../zWorld.o` **con fecha de agosto que build_direct NO
reescribe**: sellar contra el da un sha1 estable y FALSO (te devuelve siempre el
mismo). Cuidado con eso, que es una variante nueva de
`nfsmw-build-direct-miente`.

**Regresiones comprobadas, no supuestas:**

```
lcfix --check zWorld / zWorld2   -> todas las entradas @lc estan al dia  (0 pendientes, 0 venenosas)
checksplits                      -> 0 solapes, 0 rangos que corten un simbolo, LIMPIO
gapchk                           -> zWorld 5 muertos sin keep.lst, 20 B  (PREVIOS, r61 ya los dejo asi)
keepchk / keepchk2               -> ni una entrada de zWorld / zWorld2
cabeceras compartidas            -> NINGUNA tocada; CarRender.cpp solo lo incluye zWorld.cpp
                                    y WRoadNetwork.cpp solo zWorld2.cpp (grep sobre SourceLists/)
UTF-8                            -> los dos ficheros decodifican UTF-8; CarRender.cpp con CERO
                                    bytes >127, y WRoadNetwork.cpp con 4 (dos parejas 0xC2 0xAB/0xBB
                                    legitimas) tras corregir los 4 latin-1 sueltos de la r61
finales de linea                 -> CRLF puro en los dos (5.134 y 3.887 lineas, LF == CRLF)
```

**Propuestas para el jefe: NINGUNA** de `configure.py`, `config/GOWE69/*`,
`splits.txt` ni `symbols.txt`. Las unicas dos cosas que quedan escritas y sin
aplicar son los hunks de upstream de la seccion 4.1 (necesitan `lcfix`) y el
paquete P3-A de `r62-p3.md` para el `bss+64` de zWorld2.

## 6. La cola de zWorld, ordenada

| que | B de DOL | bloqueo |
|---|---:|---|
| orden de las cadenas de `.rodata` (`$LC533`/`$LC227`/`$LC57`) | 12.774 | necesita `lcfix`; 669 cadenas nuestras contra 637, 146 fuera de sitio |
| punteros de `.data` a esas cadenas (`CarSlotIDNames`, `CarEffectParameters`, `CarMemoryInfoTable`) | ~1.400 | cae solo con lo anterior |
| `RenderFlaresOnCar`, 62 filas de reparto | 1.896 | **RJ a 8 B** (sec. 2) |
| `__as__Q36Attrib3Gen5tires` y `OnGrowRequest__...` permutadas | 2.172 | `textorder`: 53 descolocadas de 583 |
| `UpdateWheelYRenderOffset`, 8 filas | 305 | mismo eje que RJ |
| `DefragmentPool` 23 filas + `SetMemoryPoolSize` 2 filas | ~200 | vedas de r49/r61 y r36e |
