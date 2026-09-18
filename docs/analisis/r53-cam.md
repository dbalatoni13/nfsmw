# r53 — zCamera: `TerrainVelocityNoise` CERRADA (1.192 B) y la palanca de la constante que cruza de bloque

**Resultado en el arbol**: un solo fichero tocado,
`src/Speed/Indep/Src/Camera/CameraMover.cpp`, **dos lineas** (una declaracion y
un argumento). `git status --short src/Speed/Indep/Src/Camera
src/Speed/Indep/Libs` no lista nada mas.

```
                          antes           despues
zCamera                   4 fn / 9.656 B  3 fn / 8.464 B
linkdelta                 .text +16  rodata-1384  data-224  bss+32   (IDENTICO)
```

- **`TerrainVelocityNoise__11CameraMoverP8bMatrix4P12CameraAnchorff` (1.192 B)
  casa al 100,0000 %**, cero filas, verificado con `build_direct.py zCamera` +
  `fncmp` + `fndiff` sobre la unidad entera (`target=100.0% ours=100.0%
  size=1192/1192`). No cambia ninguna otra funcion de zCamera ni ninguna seccion.
- **`Update__19TrackCarCameraMoverf` (992 B) sigue en 4 filas y NO se aplica**
  (regla 7). Las 4 quedan diagnosticadas hasta el escalon exacto de
  `rank_for_schedule` que las decide, con el volcado delante, y **la veda de r52
  se confirma con 40 compilaciones nuevas**.

La palanca que cerro `TerrainVelocityNoise` es **nueva** y **no es un `asm`**: es
la posicion de una declaracion. Va en §1.3 y §5.

---

## 1. `TerrainVelocityNoise`: de 14 filas a 0 con una declaracion movida

### 1.1 El cambio

```diff
     float f_speed_frequency = v_speed_terrain_freq.z;
+
+    const float accel_min = 0.0f;      // <- FUERA del `if` que la usa
 
     if (p_car->IsDragRace()) { ... }
 
     if (p_car->GetVelocityMagnitude() > speed_tresh) {
         const float accel_max = 20.0f;
-        float accel = bClamp(bDot(...), 0.0f, accel_max) * 0.05f;
+        float accel = bClamp(bDot(...), accel_min, accel_max) * 0.05f;
```

**La posicion es el cambio.** Declararla **dentro** del `if`, junto a
`accel_max` —que es donde la pondria cualquiera— **no vale**: da exactamente las
mismas 14 filas (variante `04_zero_local`). Y declararla en la cabecera de la
funcion, junto a `speed_tresh`, **tampoco** (`Z2_top`, 14 filas). Solo funciona
en un bloque basico **que domine al que la usa y no sea el de entrada**: medido
en dos sitios (`Z1`, antes del `if (IsDragRace())`, y `Z3`, entre los dos `if`),
**los dos dan 0 filas**.

### 1.2 De que estaban hechas las 14 filas

Dos racimos, con la misma causa (§1.3):

| filas | que pasa |
|---|---|
| 116, 120, 121, 125, 128 | permutacion pura **f0 <-> f12** en el `bDot` |
| 115, 119, 126, 127, 129, 130, 132, 133, 136 | los 5 `lis` del pool: **nosotros gastamos CINCO registros de direccion (r10,r8,r7,r11,r9) y el objetivo CUATRO** (r9,r8,r10,r11) porque reusa r9 |

Las cinco constantes del bloque son `20.0f` (A), `0.0f` (B), `0.05f` (C),
`0.15f` (D=`0.5f`, E=`0.15f`). El orden de los `lis`:

```
objetivo   A  C  D  E  ...  B    <- el `lis` de 0.0f, PEGADO a su `lfs`
nuestro    A  B  C  D  E         <- los cinco izados, B el segundo
```

### 1.3 El mecanismo, leido en el codigo de GCC (no adivinado)

Tres piezas encajan y explican **los dos racimos a la vez**:

1. **`bMin`/`bMax` son `asm("fsel ...")`** en GameCube (`bMath.hpp:118,138`), asi
   que el `0.0f` de `bClamp` es un **OPERANDO DE ASM**, no de un patron de la
   maquina.
2. `update_equiv_regs()` (`local-alloc.c`) le pone `REG_EQUIV` a todo pseudo
   cargado de la pool y luego intenta **sustituir el pseudo por la MEM en su
   unico uso** con `validate_replace_rtx`. Contra un `asm` **eso siempre casa**
   (los operandos de `asm` no tienen predicado). Pero la sustitucion esta
   condicionada a
   ```c
   if (REG_N_REFS (regno) == 2 && REG_BASIC_BLOCK (regno) < 0 && ...)
       reg_equiv_replace[regno] = 1;
   ```
   **`REG_BASIC_BLOCK < 0` = el pseudo se referencia en MAS DE UN bloque
   basico.** Con la constante declarada dentro del `if`, el pseudo vive en un
   solo bloque -> la condicion falla -> el par `lis`/`lfs` se queda donde
   `sched1` lo puso.
3. Declarada fuera, el pseudo **cruza de bloque**, la sustitucion entra, las dos
   instrucciones se **borran** y **`reload` rematerializa el `lis` pegado a su
   uso** (para satisfacer la restriccion `"f"` con una MEM). `sched2` recompacta
   y sale el horario del objetivo.

Y de ahi sale **tambien** la permutacion f0/f12: al desaparecer un pseudo del
RTL previo a `reload`, cambia la numeracion de allocnos y con ella el desempate
de dos cantidades de coma flotante **de la misma longitud de vida** (4). No son
dos problemas, es uno.

### 1.4 Como se llego: el volcado, no el barrido

`-fsched-verbose-5` sobre el TU dio la tabla exacta del bloque (`bb15`):

```
;;      481   506  ...  prio 7  iu2  : 484        <- lis de 0.0f
;;      484   523  ...  prio 6  lsu  : 498        <- lfs de 0.0f
;;      498    -1  ...  prio 5  none : 515 513    <- el asm de bMax
;;      533   506  ...  prio 5  iu2  : 536        <- lis de 0.05f
;;      540   506  ...  prio 4  iu2  : 543        <- lis de 0.15f
```

Con `prio 7` el `lis` de `0.0f` **no puede** caer el ultimo: no es un empate, es
prioridad, y por tanto **ningun desempate lo iba a mover**. Eso descarto de un
golpe todas las reescrituras de la sentencia (11 formas probadas, §1.5) y
obligo a buscar la causa fuera de `sched1`. Simulando el bloque **sin** ese
`lis` sale, ranura a ranura, el orden del objetivo — de ahi la hipotesis, y de
ahi el codigo de `local-alloc.c`.

### 1.5 Lo que NO vale (para no repetirlo)

- **11 reescrituras de la sentencia**: temporal para el `bDot`, `bClamp`
  desplegado a `bMin(accel_max, bMax(...))`, `bMax(bMin(...))` (24 filas,
  peor), `0.0f` como local *dentro* del `if`, sin `accel_max`, `accel` partido
  en dos sentencias, `+=` escrito como `x = x + ...`, `accel_max` no-`const`,
  temporal para `(0.5f - f)`, y el clamp escrito a mano replicando los dos
  `asm("fsel")`. **Las 11 dan exactamente 14 filas** (una da 14 en otras
  posiciones, otra 24).
- **14 fantasmas de presion de registro** (`register T x asm("frN"/"rN")` +
  pareja de `asm` de apertura y cierre) sobre fr0, fr8-fr12, r0, r5-r11:
  **las 14 empeoran**, de 21 a 25 filas. El par de `asm` gasta dos ranuras de
  emision (`unit none` cuenta, `ISSUE_RATE`=2) y descoloca el bloque entero.
- `volatile float` para la constante: 98 filas.

---

## 2. `TrackCar` (992 B): sigue en 4 filas, y ahora se sabe por que exactamente

La receta de r51+r52 se reproduce al byte: **4 filas, `[107, 109, 137, 141]`**.
Las 40 compilaciones nuevas de esta ronda **no bajan de ahi**. Las dos parejas:

### 2.1 Racimo A — los dos primeros `stfs` de `Look` (filas 107/109)

```
objetivo   107 stfs f12,0x98(z)   109 stfs f0,0x90(x)   111 stfs f13,0x94(y)
nuestro    107 stfs f0, 0x90(x)   109 stfs f12,0x98(z)  111 stfs f13,0x94(y)
```

Los tres `stfs` empatan en prioridad (53), en `INSN_REG_WEIGHT` (-1 los tres: el
registro de valor muere ahi), en clase y en numero de dependientes. **Decide
`INSN_LUID`, o sea el orden de las sentencias.** Y ahi esta la trampa:

| orden de las asignaciones | resultado |
|---|---|
| `Look.x; Look.z; Look.y` | **registros correctos** (z→f12, x→f0, y→f13) pero orden de `stfs` **x,z,y** |
| `Look.z; Look.x; Look.y` | **orden correcto** (z,x,y) pero registros **z→f0, x→f13, y→f12** |

Son **excluyentes**, y la razon esta medida: `local_alloc` reparte por
`QTY_CMP_PRI = floor_log2(n_refs)*n_refs*size/(muerte-nacimiento)`. Con el orden
`xzy` las tres vidas miden 4, 6 y 8 y el desempate lo da la longitud (x>y>z ->
f0, f13, f12 = **el objetivo**). Con `zxy` las tres miden **6** y el desempate lo
da el numero de cantidad, o sea el orden de las CARGAS (z,x,y) -> z→f0.

El objetivo tiene las dos cosas, luego **su `local_alloc` ve el orden `xzy` y su
`sched2` emite `zxy`**. En `sched2` el empate de los dos `stfs` ya no puede
romperse por `INSN_REG_WEIGHT` (el escalon esta guardado por
`if (!reload_completed && ...)`, `haifa-sched.c`), asi que cae en `INSN_LUID` =
el orden que dejo `sched1`. **Por eso el barrido no lo alcanza.**

Barridos hechos esta ronda sobre la base de 4 filas, todos >= 4:
**24 combinaciones** declaracion x almacenamiento (6x4), **5 familias** con un
miembro cargado en linea sin temporal, y **4 reordenaciones** de
`Look = ...` contra `displacement /= distance` (esas ultimas, 38-46 filas).

**Hallazgo nuevo y util del volcado**: las tres CARGAS salen en orden z,x,y —no
en el de la declaracion— porque `INSN_REG_WEIGHT` (SET − REG_DEAD) vale **0**
para la carga que mata el puntero base `_cp` (la ultima declarada) y **1** para
las otras dos, y gana la menor. **La carga que mata su puntero base se planifica
la primera.** Eso explica la mitad de la tabla de 36 ordenes de r51 sin probarla.

### 2.2 Racimo B — `mr r5, r3` cuatro ranuras tarde (filas 137/141)

```
t=39  --> 730 (lsu, lfsx)   --> 674 (iu2, addi r3,r1,0x38)
t=40  --> 698 (lsu, lfsx)   --> 676 (none, EL asm "+r"(_plo))   <- el objetivo pone aqui `mr r5,r3`
t=41  --> 714 (lsu, lfsx)   --> 763 (iu2, mr r5,r3)
```

El `asm("" : "+r"(_plo))` —imprescindible para que el `addi` gane la ranura de
t=39— **ocupa exactamente la ranura donde el objetivo emite `mr r5, r3`**, y
ademas se la gana por prioridad: `prio(asm) = max(prio(call), prio(mr)) + 1`, o
sea **siempre una mas que el `mr` mientras el `mr` dependa de el**.

Y si se rompe esa dependencia (copiar el puntero a otro pseudo **antes** del
`asm`, y pasar la copia como tercer argumento) la cuenta sale peor de lo que
promete: el `mr` sube a la fila 140 (una ranura mas cerca, **sigue habiendo 4
filas**) porque la copia extra que uno escribe **desaparece en `reload`** —los
dos pseudos acaban en el mismo registro fisico—, y la que sobrevive es el
`mr` de montaje del argumento, con prioridad 23 contra las 24 de la copia que se
borro. Confirmado en el volcado (`678` copia, `767` asm, `771` `mr`).

Probado esta ronda, todo >= 4 filas: 6 formas de romper la dependencia del
tercer argumento (`_plo3 = _plo` antes, `_plo3 = &look_offset` antes, `&look_offset`
crudo en el 3.o, en el 1.o, en los dos, copia despues del `asm`), quitar el `asm`
de `_plo` (6 filas) y moverlo detras de los fantasmas (4).

**Conclusion**: `TrackCar` no cierra con estas piezas. Cerrarlo pide que
`sched1` deje el orden `xzy` y `sched2` emita `zxy` —o sea, una palanca que
actue **entre** `local_alloc` y `sched2`—, mas quitar el `asm` de `_plo` sin
perder la ranura del `addi`. No se aplica nada: 4 filas son 0 B.

---

## 3. Extrapolacion, contada (no supuesta)

La palanca de §1.3 solo se dispara si una **constante de la pool** es
**operando de un `asm`**. En GameCube eso son, en `bMath.hpp`, `bMin` (118),
`bMax` (138), `bAbs` (161) y `bInvSqrt` (72-87) — y por herencia `bClamp(float)`.

Contadas las llamadas a `bMin`/`bMax`/`bClamp` en el cuerpo de **12 de las
funciones abiertas del proyecto**:

| funcion | unidad | llamadas | con literal |
|---|---|---:|---:|
| `HolePunchAvoidables` | zWorld2 (2.980 B, 99,97 %) | 5 | **3** |
| `DefragmentPool` | zWorld | 1 | 0 |
| `Update__19TrackCarCameraMover` | zCamera | 1 | 1 (`bClamp` de **int**, sin `asm`) |
| las otras 9 | — | 0 | 0 |

**El frente es UNA funcion: `HolePunchAvoidables`** (territorio de `world`). No
es un frente ancho; conviene decirlo con la cifra para que nadie lo persiga en
las otras nueve.

---

## 4. Herramientas

En `scratchpad/r53_cam/` (187 kB; los volcados y objetos borrados). Heredadas de
r52 con la ruta cambiada: `cc.py`, `w.py`, `wh.py`, `frows.py`, `dump.py`,
`allfn.py`, `radio.py`, `lst.py`. Nueva:

| fichero | que hace |
|---|---|
| `pblk.py` | **parsea un volcado `-fsched-verbose-5`** y saca, por bloque basico y por pasada, la tabla de dependencias (`prio`, `cost`, unidades, sucesores) y el **orden de emision**; imprime la **firma de unidades** del bloque (`L`=lsu, `I`=entero, `F`=fpu, `N`=asm, `B`=salto) y **busca por firma** |
| `mkcut.py` | trunca el `.cpp` justo detras de la funcion objetivo |

**Las dos juntas resuelven el problema real de leer el planificador**: el
volcado **no trae nombres de funcion**, asi que en un TU de verdad no hay forma
de encontrar el bloque. Truncar `CameraMover.cpp` detras de la funcion baja el
volcado de **5,8 MB a 46.000 lineas** y reproduce la funcion **al byte** (14
filas, mismos indices); y `pblk.py <volcado> "LILILILILIL"` la localiza en **un
tiro** (un unico bloque casa). Es mas barato y mas fiable que la mini-TU de r52,
que aqui no servia (la funcion es un metodo con toda la cabecera detras).

---

## 5. Propuestas para `docs/PLAYBOOK.md`

1. **La constante que cruza de bloque (palanca nueva, cero `asm`, 1.192 B).**
   Si una constante literal se pasa a `bMin`/`bMax`/`bClamp`/`bAbs`/`bInvSqrt`
   —que en GameCube son `asm`— y el `lis`/`lfs` de la pool sale izado y comiendo
   un registro de direccion de mas, **declarala como local `const` en un bloque
   basico que domine al que la usa** (no en el mismo, no en el de entrada).
   `update_equiv_regs` exige `REG_BASIC_BLOCK < 0` para sustituir el pseudo por
   la MEM, y contra un `asm` la sustitucion **siempre** casa porque los operandos
   de `asm` no tienen predicado; entonces `reload` rematerializa el `lis` pegado
   a su uso. Arregla de golpe el reparto de enteros **y** el de coma flotante,
   porque quita un pseudo de la numeracion de allocnos.
2. **Un `prio` alto en el volcado CIERRA una via.** Si la instruccion que sobra
   tiene mas prioridad que las que deberian ganarle, **no es un empate**: ningun
   desempate —ni `INSN_REG_WEIGHT`, ni clase, ni dependientes, ni LUID— la va a
   mover, y reordenar la fuente es tiempo perdido. Hay que sacarla del RTL, no
   reordenarla. En esta ronda ahorro 11 variantes y las senalo como la firma de
   «la causa esta fuera de `sched1`».
3. **La carga que mata su puntero base se planifica la primera.**
   `INSN_REG_WEIGHT` = SET − REG_DEAD, y gana el menor: la ultima carga
   declarada de un `p->x/p->y/p->z` mata `p`, vale 0 contra 1, y sale delante.
   Explica los ordenes de carga «raros» sin barrer.
4. **`INSN_REG_WEIGHT` solo existe en `sched1`.** Esta guardado por
   `if (!reload_completed && ...)`. En `sched2` el orden de dos instrucciones
   empatadas lo fija `INSN_LUID`, **que es el orden que dejo `sched1`**. Cuando
   el objetivo pide un reparto que sale de un orden y un horario que sale de
   otro (caso `TrackCar` §2.1), **la fuente no puede darte los dos**.
5. **Como leer el planificador en un TU real**: el volcado de
   `-fsched-verbose-5` **no trae nombres de funcion**. Truncar el `.cpp` detras
   de la funcion (comprobando antes que la funcion sale identica) y localizar el
   bloque **por firma de unidades** con `pblk.py`. Un TU de 966 lineas pasa de
   5,8 MB de volcado a 46 k lineas y el bloque sale de un tiro.

---

## 6. Lo que NO se ha tocado

- **`Update__8ICEMoverf`** (3.868 B): veda de r51 vigente (1.606 compilaciones
  de permutador, 8 formas de `SignedMod`, 13 colocaciones de fantasma). **Cero
  compilaciones gastadas.**
- **`__static_initialization_and_destruction_0`** (3.604 B): las dos cerraduras
  de r52 siguen. Cero ensayos.
