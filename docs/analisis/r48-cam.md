# r48 — zCamera: el racimo B de `TrackCar` no es `sched1`, es `sched2`; y lo decide el reparto del racimo C

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
propias a `scratchpad/r48_cam/out/` sobre COPIAS (`TrackCar_var.cpp`,
`ICEMover_var.cpp`) y con un arbol de includes sombra para `UTLVector.h`; `src/`
no se toco en ningun momento.

---

## 0. Resumen de lo que cambia respecto a la r47

| # | la r47 decia | lo medido en la r48 |
|---|---|---|
| 1 | las 15 filas de `TrackCar` son de `sched1`, y el racimo B se pierde en el nivel 5 de `rank_for_schedule` | **solo el racimo A es de `sched1`. El racimo B sale CORRECTO de `sched1` y lo estropea `sched2`**, en el nivel 4 (`depend_count`), no en el 5 |
| 2 | el racimo C es «cola de A y B» | **es al reves**: el racimo C es reparto de `local_alloc`, y arreglarlo cierra el racimo B por si solo (cinco medidas) |
| 3 | la ruta de los tres pines valia 10 filas y habia que retomarla | retomada y ampliada: **9 filas / 99,8105 %** es el techo de esa via, y el **DWARF prohibe la via entera** (ver §3) |
| 4 | `TerrainVelocityNoise`: «el objetivo reparte cuatro registros de direccion y nosotros cinco» | cierto, pero es **efecto**: el `elf_high` del 0.0f **ya esta izado al salir de `sched1`**, antes de repartir (§6.bis) |
| 5 | `ICEMover`: territorio agotado, sin angulo | el DWARF nombra la diferencia (`int frame; // r8` contra ninguno) y la ventana de 116 B cae dentro de su bloque; el pin la desmiente pero deja el diagnostico escrito (§5) |

Y una correccion de metodo: `regmap.py` **no encuentra las funciones de zCamera
por nombre mangled ni con la unidad larga**. Hay que llamarlo
`python scripts/regmap.py zCamera "TrackCarCameraMover::Update"` (nombre
desmangled, unidad corta). Con la forma que sale del `fncmp` responde
«no encuentro ... en el volcado original» y parece que no hay DWARF. **Lo hay, y
para las cinco pendientes.**

---

## 1. `TrackCarCameraMover::Update` (992 B, 15 filas): la separacion `sched1` / `sched2` / `local_alloc`

Instrumento: el micro de la r47 (`micro_tc.cpp`, 4-8 s por medida) mas los
volcados RTL de `cc1plus` (`-dg -dl -dS -dr -fsched-verbose-5`).

**Aviso util para todo el proyecto**: con `-dS` la traza verbosa de **`sched1`**
va al fichero `*.i.sched`, y la de **`sched2`** a **stderr** (porque no se pidio
`-dR` y su `dump_file` es NULL). Son dos trazas distintas y se confunden con
facilidad: la de stderr empieza literalmente por `-- basic block 0 ... after reload`.

Las 15 filas son tres racimos:

| filas | sentencia | que pasa |
|---|---|---|
| 107-111 | `Look = *CarToFollow->GetGeometryPosition();` (`bMath.hpp:480-482`, `bFill`) | **mismos registros**; el objetivo almacena `z, x, y` y nosotros `x, y, z` |
| 113-123 | `displacement /= distance;` (`bMath.hpp:504-509`, `bScale`) | **mismos registros** (f11=x, f12=y, f13=z en los dos); el objetivo multiplica y almacena `z, x, y` y nosotros `x, z, y` |
| 134-143 | `look_offset.x/y/z = TrackCarLookOffset*[CameraType]` | **mismo orden**; cambian los REGISTROS: objetivo f10/f11/f12 (z/x/y), nosotros f12/f0/f13 |

### 1.1 El racimo A es `sched1` (confirma la r47)

En el volcado `.sched` (post-`sched1`) de la version base, los tres `stfs` de
`bFill` salen en el orden 517 (`0x90`, x), 520 (`0x94`, y), 523 (`0x98`, z):
**ya esta mal antes de la asignacion de registros**. Sigue en pie el analisis de
la r47: los tres stores empatan en los cuatro primeros niveles de
`rank_for_schedule` y decide `INSN_LUID`.

### 1.2 El racimo B **NO** es `sched1`: sale bien y lo rompe `sched2`

Este es el hallazgo de la ronda. En el volcado `.sched` **nuestro**, el orden
post-`sched1` del racimo B es

```
554  (set 237 (mult 232 226))   z*escala
546  (set 233 (mult 228 226))   x*escala
555  (set (mem r31+0x20) 237)   store z
550  (set 235 (mult 230 226))   y*escala      <- aqui muere 226 (la escala)
547  (set (mem r31+0x18) 233)   store x
551  (set (mem r31+0x1c) 235)   store y
```

que es **exactamente** el del objetivo (filas 119-124: `fmuls f13`, `fmuls f11`,
`stfs f13,0x20`, `fmuls f12`, `stfs f11,0x18`, `stfs f12,0x1c`). Y el reparto
que hace `local_alloc` tambien es el del objetivo: `;; Register 228 in 43`
(f11, x), `230 in 44` (f12, y), `232 in 45` (f13, z), `226 in 32` (f0, escala).

Lo estropea `sched2`. Traza (stderr, `-fsched-verbose-5`):

```
;;	Ready list (t = 29):    550  554  546
;;		--> scheduling insn <<<546>>> on unit fpu
```

y la tabla de dependencias del bloque da la razon exacta:

```
;;      554   248     0     2    33     1    fpu : 1043 694 677 611 555          <- 5 dependientes
;;      546   248     0     2    33     1    fpu : 1043 762 741 677 611 547      <- 6 dependientes
;;      550   248     0     2    33     1    fpu : 1043 677 711 611 551          <- 5 dependientes
```

Los tres tienen **prioridad 33**. En `sched2` el nivel 2 de `rank_for_schedule`
—`INSN_REG_WEIGHT`— **no existe**: `haifa-sched.c` lo guarda tras
`if (!reload_completed && ...)`. Con la prioridad empatada y la clase respecto a
`last_scheduled_insn` igual, **decide el nivel 4, `depend_count`, y gana el que
mas dependientes tiene**: 546 (la `x`) con 6 contra 5.

**Consecuencia practica para todo el proyecto**: la palanca 10 del brief («los
dos grupos de peso de `sched1`») **no aplica en `sched2`**. Si una permutacion
sobrevive intacta a `sched1`, no hay que buscarle peso de registros: hay que
contar dependientes.

### 1.3 Los dos dependientes de mas los pone el racimo C

Los dependientes extra de 546 son `741` (la llamada a `eMulVector`, que clobbea
f11) y `762` (`(set (reg 295) (mem r82+0x98))`, que **tambien se reparte a f11**).
Es decir: **546 tiene un dependiente mas solo porque su registro destino (f11) lo
reutiliza el codigo POSTERIOR, y 554 (f13) no.** Cambiando el reparto del racimo
C cambia esa cuenta.

Y se comprueba en la maquina: **forzando el racimo C a f11/f12/f10 el racimo B
cierra entero** (las filas 119, 120, 121 y 123 desaparecen) en **cinco variantes
independientes** (§4, familias A, C y D). No es una correlacion: es la unica
intervencion que lo cierra en toda la ronda.

### 1.4 Y el racimo C es `local_alloc`, no `sched`

Los tres pseudos del racimo C (267 = x, 275 = y, 283 = z) **no aparecen** en la
lista `;; 41 regs to allocate` del `.greg`: los reparte `local_alloc`. El
`.lreg` da sus dos parametros:

```
Register 267 used 2 times across  6 insns in block 13   -> QTY_CMP_PRI = 2/6  = 3333
Register 275 used 2 times across  6 insns in block 13   -> QTY_CMP_PRI = 2/6  = 3333
Register 283 used 2 times across 11 insns in block 13   -> QTY_CMP_PRI = 2/11 = 1818
```

`QTY_CMP_PRI(q) = floor_log2(n_refs)*n_refs*size / (death-birth)`. 267 y 275 son
**las dos cantidades de mayor prioridad de todo el bloque** (la siguiente es 303,
3 refs / 11 insns = 2727), empatan, y `qty_compare_1` desempata por numero de `qty` -> primero
267, luego 275. `find_free_reg` recorre `REG_ALLOC_ORDER` (`rs6000.h:932-945`),
que en FP es **f0, f13, f12, f11, f10, f9, ...**, y les da f0, f13 y f12.

Para que el objetivo saque f11/f12/f10, f0 y f13 tienen que estar **ocupados en
ese tramo**, y en el codigo del objetivo **no hay ni una instruccion que use f0 o
f13 entre la fila 126 (el `lwz r0,0xd4(r31)` de despues del `bl bCross`) y la
fila 147 (el `bl eMulVector`)**. `local_alloc` solo marca ocupado
`regs_live_at[born..dead]`, asi que con su camino avaro es **imposible** llegar a
f11/f12/f10. O el objetivo los reparte en `global_alloc` —cuyo `find_reg` tiene
una pasada 0 que **solo considera registros ya usados** (`IOR_COMPL_HARD_REG_SET
(used, regs_used_so_far)`, `global.c`)— o hay una cantidad mas viva ahi que
nosotros no emitimos. **No consegui provocar ninguna de las dos cosas desde la
fuente.**

---

## 2. El reparto de las direcciones del racimo C ya es correcto, y es fragil

Un dato que hay que dejar escrito porque cuesta media ronda descubrirlo: en la
**base**, las tres parejas `lis`/`addi` de `TrackCarLookOffsetX/Y/Z` salen
**identicas al objetivo** (X->r11, Y->r10, Z->r9, y en el orden X, Y, Z). Eso no
es casualidad: `sched1` iza la carga de `z` (es la ultima en la fuente y por
tanto la que mata `r0`, el indice: peso -1 contra 0), y **despues** de esa
reordenacion los rangos de vida de los `lo_sum` quedan 4 (Z), 12 (X) y 14 (Y)
insns, o sea prioridades 2/4 > 2/12 > 2/14, y `local_alloc` reparte en ese orden
sobre `REG_ALLOC_ORDER` de GPR (`0, 9, 11, 10, 8, 7, ...`): Z->r9, X->r11,
Y->r10.

**Cualquier cosa que cambie el orden en que `sched1` deja esas tres cargas rompe
los seis `lis`/`addi`.** Todos los pines del racimo C lo hacen (5 filas nuevas),
y por eso ninguna variante con pines baja de 9.

---

## 3. El DWARF cierra la via de los pines: el original NO tiene locales ahi

`python scripts/regmap.py zCamera "TrackCarCameraMover::Update"` da **IDENTICO**
(12 locales, 0 con registro distinto, 0 de ambito equivocado, 0 solo del
original, 0 solo nuestras). Y el volcado
(`symbols/mw_dwarfdump.nothpp:335003`) da las locales de los inlines **con su
registro**:

```
inline struct bVector3 * bCopy(...) {      <- racimo A
    float x; // f0
    float y; // f13
    float z; // f12
}
inline struct bVector3 * bScale(...) {     <- racimo B (operator/=)
    float x; // f11
    float y; // f12
    float z; // f13
}
```

**Son exactamente nuestros registros.** Y el bloque de `look_offset` (racimo C)
**no declara ninguna local**: sus f10/f11/f12 son temporales del compilador.
O sea: meter `register float ox asm("fr11")` y companía es introducir tres
locales que el original no tiene, y ademas romper el reparto de §2. **Via
cerrada, no por el porcentaje sino por el volcado.**

(La otra cara: el DWARF confirma que el `bScale` de `hcomp` —el de
`vert_comp = 0.0f`— tiene sus tres locales **sin registro**, coherente con los
tres `stfs f30` del objetivo. Y ojo: **el orden en que el DWARF lista las
locales no es fiable**: el segundo `bScale` las lista `z, y, x` y el segundo
`bSub` `z2, y2, x2, z1, y1, x1`, mientras `bAdd` las lista `x1, y1, z1, x2, y2,
z2`. Los REGISTROS si lo son.)

---

## 4. Todo lo medido y negativo en `TrackCar`, con la cifra

Base: **15 filas / 99,6290 % / 992 B**. El micro reproduce la unidad exacta.
`CC` = helper local `CamCopy` en sustitucion de `Look = *ptr` (loads / stores);
`Cpin` = `register float o{x,y,z} asm("fr11"/"fr12"/"fr10")` + `look_offset.c = oc`,
con el orden de declaracion entre parentesis.

**A. Solo `Cpin`, por orden de declaracion**

| orden | filas |
|---|---:|
| zxy | **10** |
| yxz / zyx / xzy | 12 |
| xyz / yzx | 13 |
| locales sueltas sin pin (yxz) | **15 (neutro)** |

**B. Solo `CC` con pines fr0/fr13/fr12 (la ruta de la r47)** — 10 filas /
99,7500 %. Reproducida.

**C. `CC` sin pines x `Cpin(zxy)`**

| CC loads / stores | filas |
|---|---:|
| zyx / zxy | **9 (99,8105 %)** |
| xyz / xyz, xzy / xyz | 10 |
| yxz / xyz, yxz / zxy, xzy / zxy, zxy / xyz | 12 |
| xyz / zxy, yzx / xyz, yzx / zxy, zxy / zxy, zyx / xyz | 13 |

**D. `CC(zyx/zxy)` con pines parciales + `Cpin(zxy)`**

| pines de CC | filas |
|---|---:|
| ninguno | 9 |
| x=fr0 y z=fr12 | 9 |
| x=fr0 | 13 |
| y=fr13 | 13 |
| z=fr12 | 15 |

Con `x=fr0, z=fr12` **los seis ordenes de carga dan 9**: los pines dominan al
planificador.

**E. `CC` con pines fr0/fr13/fr12 + `Cpin`** (colision en fr12)

| Cpin | filas |
|---|---:|
| yxz | 10 |
| yzx | 12 |
| zxy | 13 |
| zyx / xzy | 15 |
| xyz | 16 |

**F. Formas de fuente sin un solo `asm` (las que podrian ser del original)**

| ensayo | filas |
|---|---:|
| `bVector3 look_offset(X[i], Y[i], Z[i])` (constructor) | **15**, fuzzy identico al base |
| `bFill(&look_offset, X[i], Y[i], Z[i])` | **15**, fuzzy identico al base |
| constructor + CC(zyx/zxy) | 14 |
| `bFill` + CC(zyx/zxy) | 14 |
| punteros `const float *pxo = TrackCarLookOffsetX;` + pines (3 ordenes) | 13, 13, 13 |
| helper `CamLook` (direcciones por parametro, pines en las cargas) | 15, 15 |
| helper `CamLook3` (pines sobre los PARAMETROS, direcciones evaluadas fuera) | 12, 12, 12; sin CC 13 |

El constructor y el `bFill` explicito **producen el mismo objeto que la base**:
el inliner los colapsa. Queda descartada la hipotesis de que el racimo C tuviera
mas pseudos en el original por venir de un constructor.

**Techo de la ronda: 9 filas / 99,8105 %**, con tres pines en el racimo C mas un
helper. Revertido por la regla 9 y, sobre todo, por §3.

**Lo que queda para la proxima, en orden:** el racimo C es reparto de
`local_alloc` sobre dos cantidades de 2 refs y 6 insns de vida. Lo unico que
puede moverlo sin inventar locales es **bajarles la prioridad**
(`QTY_CMP_PRI = 2/vida`) alargandoles el rango de vida, o **subir la de otras dos
cantidades del bloque 13** para que se lleven f0 y f13 antes. La segunda es la
unica compatible con §2, porque no toca el orden que deja `sched1`. Las
candidatas del bloque con prioridad inmediatamente inferior son 296 (2 refs / 8
insns = 2500), 303 (3/11 = 2727) y 302 (3/12 = 2500), todas del `bAdd` de
`Look += look_offset` y del `bSub` de `lookdir`.

---

## 5. `Update__8ICEMoverf` (3.868 B, 15 filas): el DWARF **si** dice algo, y el pin lo desmiente

La r46 y la r47 lo dieron por «territorio agotado». No lo esta del todo: hay un
diagnostico nuevo, aunque el ensayo obvio salga negativo.

`python scripts/regmap.py zCamera "ICEMover::Update" --all` (39 locales iguales)
señala **cuatro**:

| ambito | local | original | nuestro |
|---|---|---|---|
| fn | `n_state` / `bGeneric` | r26 | r27 |
| fn | (pareja) `bLerpLag` | r27 | r26 |
| b3/b0 | `current_sec` | f0 | f13 |
| b3/b0 | **`frame`** | **r8** | **(ninguno)** |

Las dos primeras son un ciclo cerrado r26<->r27 y **no producen ni una fila** de
diff (las 15 estan todas entre `+0x934` y `+0x9A4`). La tercera es **un error del
volcado**: en las filas 584-586 `current_sec` esta en **f13 en los dos lados**
(`lfs f13, 30.0f` + `fmuls f13, f30, f13`, identicas); el `f0` del DWARF
corresponde al `current_sec` de la otra rama. Un contraejemplo mas para
«el DWARF tambien se equivoca».

La cuarta es la buena y encaja con la ventana: el bloque anonimo del DWARF va de
`0x8007CC88` a `0x8007CE34`, o sea `+0x8C8`..`+0xA74` desde el arranque de la
funcion, y **las 15 filas caen dentro**. Lo que dice el diff es exactamente eso:

```
 589 mr r7, r0            | mr r8, r0          <- copia cruda de GetNumKeys()
 593 lwz r8, 0x15c(r1)    | lwz r9, 0x15c(r1)  <- a = FloatToInt(...)
 596 cmpwi r8, 0x0        | cmpwi r11, 0x0     <- el objetivo compara `a`, nosotros la copia
 602 subf r8, r0, r11     | subf r11, r0, r11  <- el objetivo devuelve a `a`
 617 mulli r9, r8, 0x18   | mulli r9, r9, 0x18 <- &Keys[n] con `n`, nosotros con el clamp
```

El objetivo mantiene **una sola cantidad (r8) desde el `lwz` hasta el `mulli`**:
el parametro `a` de `SignedMod`, su valor de retorno y la `frame` de
`GetKey(frame)` son el mismo pseudo. Nosotros partimos en dos (`a` en r9, viva
solo del 593 al 594, y la copia del bucle en r11 que hereda el resultado), y esa
`a` cortita se lleva r9 —el **segundo** de `REG_ALLOC_ORDER` de GPR— y desplaza
todo lo demas un puesto.

**Ensayos, todos negativos, con la cifra** (micro `micro_ice.cpp`, reproduce la
funcion exacta: 99,9069 % / 3.868 B / 15 filas):

| ensayo | filas | fuzzy |
|---|---:|---:|
| base | **15** | 99,9069 |
| `register int frame asm("r8")` | 16 | 99,8087 |
| `register int frame asm("r7")` | 16 | 99,7466 |
| `register int frame asm("r11")` | 21 | 99,7518 |
| `register int frame asm("r10")` | 21 | 99,7777 |
| `register int frame asm("r9")` | 22 | 99,6846 |
| `int frame = FloatToInt(...); frame = SignedMod(frame, ...)` | 15 | 99,9069 (**objeto identico**) |
| lo anterior + `int num_keys = pShake->GetNumKeys()` | 15 | 99,9069 (**objeto identico**) |
| solo `int num_keys = pShake->GetNumKeys()` | 15 | 99,8087 (codigo distinto, mismas filas) |

El pin en r8 **no une la cadena**: mete un `mr r8, r11` de mas (fila 607,
`DIFF_INSERT`) porque el resultado sigue naciendo en el pseudo del bucle. Segun
la palanca 3 eso significa que el reparto es sintoma; y el `.lreg` lo confirma:
la variable de usuario `a` es un unico pseudo `reg/v:SI 1045` que recibe tanto
`(minus 1045 1049)` como `(const_int 0)` —la union ya esta hecha en RTL— y lo
que difiere es **cual de los dos pseudos (el del `fctiwz` o el de `a`) sobrevive
hasta el `mulli`**.

**La prohibicion de la r36e sigue teniendo sentido, pero por otra razon**: no es
que inventar locales rompa, es que **el DWARF ya dice cuales hay** y son las
nuestras. Lo que falta no es una local: es que el pseudo del `lwz` y el de `a`
se fundan. Eso se ataca en `ICEMath.hpp::SignedMod` (territorio) o con la
palanca 2 (`set_preference`) sobre la copia del bucle, no con un pin sobre
`frame`.

---

## 6. `_Storage<CameraAI::Director*,2>` (1.156 B): las dos piezas, medidas juntas

Con cabecera sombra (copia completa de `Libs/Support/Utility/`, `-I inc`
delante; zCamera entero, ~50 s por medida). La r47 dejo que la barrera en el
latch hace casar por primera vez el par `mr r4,r9`/`cmplw` pero deja el tamano en
1.160 porque `size()` se recarga dentro de `push_back` (filas 70/71). Esta ronda
mide **la otra mitad y las dos juntas**:

| variante de `UTLVector.h` | `_Storage` | tamano | resto de zCamera |
|---|---:|---:|---|
| **S0** sombra sin tocar | 98,6990 % | **1156** | 5 fn / 10.828 B (= base) |
| **P1** `push_back()` cachea `size()` en una local (`size_type n = size();` y usarla en las tres apariciones) | 98,6990 % | **1156** | **objeto IDENTICO al base** |
| **P2** P1 + `curSize` con `asm("" : "+r"(curSize))` en el latch | 99,2180 % | 1160 | 5 fn / 10.832 B |
| **P3** solo `curSize` con la barrera (variante C de la r47) | 99,2180 % | 1160 | 5 fn / 10.832 B |

**P2 y P3 dan exactamente el mismo objeto**: la mitad que faltaba —cachear
`size()` DENTRO de `push_back()`— **no aporta un solo byte**, porque `cse` la
colapsa igual que colapso el `curSize` a secas de la r42. Es el mismo modo de
fallo, ahora medido en el otro lado del bucle.

Con eso, las tres recetas «obvias» estan descartadas con medida: `curSize` a
secas (r42, objeto identico), `++curSize` como induccion (r47, 97,4879 % /
1.164 B) y cachear dentro de `push_back` (r48, objeto identico). Lo unico que
mueve la fila es la barrera, y deja +4 B.

Lo que el objetivo hace y no hemos sabido reproducir: **un unico pseudo con
`size()` vivo a traves del bucle**, refrescado en el latch con `mr r4,r9` y
reutilizado dentro de `push_back` para el `cmplw r4,r0` contra `capacity()`. Con
la barrera se consigue lo primero y se pierde lo segundo (`cse` ya no puede
identificar los dos `size()`), y sin ella se consigue lo segundo y se pierde lo
primero. **Son excluyentes con las formas de fuente probadas**: hace falta que el
valor llegue a `push_back` sin pasar por `cse`, o sea **por parametro**, y eso es
cambiar la firma de un metodo que consumen decenas de unidades.

---

## 6.bis `TerrainVelocityNoise` (1.192 B, 14 filas): es `sched1`, medido en el volcado

No probe ninguna forma de fuente nueva (r36e/r36f/r38/r46/r47 dejan trece
descartadas). Si aplique la separacion de §1.2, que es lo que el brief pedia
hacer antes de escribir la primera variante, y **el resultado cambia el
diagnostico de sitio**: el par INSERT(127)/DELETE(129) —el `lis` del 0.0f dos
ranuras mas alla— **ya esta mal al salir de `sched1`**, o sea antes de repartir
registros.

Orden de los cinco `elf_high` de literal en el volcado `.sched` (post-`sched1`)
de nuestra version, y el del objetivo leido del `.o`:

| # | nuestro (post-`sched1`) | objetivo |
|---|---|---|
| 1 | 415 `$LC344` = 20.0f | 20.0f (r9) |
| 2 | 481 `$LC337` = **0.0f** | 0.05f (r8) |
| 3 | 533 `$LC345` = 0.05f | 0.5f (r10) |
| 4 | 548 `$LC347` = 0.5f | 0.15f (r11) |
| 5 | 540 `$LC346` = 0.15f | **0.0f (r9, reutilizado)** |

El objetivo materializa la direccion del 0.0f **la ultima**, justo antes de su
`lfs` (fila 130) y de su `fsel` (fila 131), y por eso puede reutilizar el r9 que
acababa de quedar libre del 20.0f: **cuatro registros de direccion**. Nosotros la
subimos a la segunda posicion y necesitamos **cinco** (r7..r11). La permutacion
de r7/r8/r9/r10/r11 de las filas 115-136 es la CONSECUENCIA, no la causa.

Es decir: aqui la palanca 10 del brief **si** aplica (es `sched1`), al reves que
en el racimo B de `TrackCar`. Lo que hay que mover es la prioridad o el LUID de
**un `elf_high` de literal**, no el reparto. Los cinco `elf_high` son
independientes entre si (`(nil)` en LOG_LINKS), asi que solo los separa el nivel
1 (prioridad = camino mas largo hasta el final) y el nivel 5 (LUID). En la fuente
el 0.0f entra por el `bMax` de `bClamp(dot, 0.0f, accel_max)` y el 20.0f por el
`bMin`: **el 0.0f se USA antes y se materializa despues**, que es justo lo que no
conseguimos. La forma de fuente que lo mueva tiene que retrasar el LUID del 0.0f
por debajo del de 0.15f sin cambiar el orden de los `fsel`.

---

## 7. Propuestas fuera de territorio

1. **`UTLVector.h`** — ver §6. No la aplico: la cabecera la consumen decenas de
   unidades y la barrera de `assign()` ya rompio zSim/zPhysics/zMain una vez
   (commit `6d623cb9`).

2. **`bMath.hpp`** — nada que proponer, y el DWARF **cierra en positivo** dos
   vedas que hasta ahora eran solo empiricas: los registros de `bCopy`
   (x=f0, y=f13, z=f12) y los de `bScale` (x=f11, y=f12, z=f13) del original son
   los nuestros. No hay que tocar esa cabecera para `TrackCar`.

3. **`scripts/regmap.py`** — que acepte el nombre **mangled** y la unidad larga
   (`Speed/Indep/SourceLists/zCamera`), que es como salen del `fncmp` y del
   `triaje`. Hoy responde «no encuentro ... en el volcado original» y «unidad
   desconocida», que se lee como «no hay DWARF» cuando si lo hay. Me costo una
   hora larga y la r47 dio por bueno que no habia volcado para estas funciones.

4. **`scripts/rtldump.py`** — que deje pasar `-fsched-verbose-N` (hoy filtra por
   `KEEP` solo los flags de los cflags, y los argumentos extra tienen que empezar
   por `-d`), y que avise de que **la traza de `sched2` sale por stderr** cuando
   no se pide `-dR`. Sin eso, quien busque el nivel de `rank_for_schedule` que
   decide mira la traza equivocada, como paso en la r47.

---

## 8. Herramientas que dejo en `scratchpad/r48_cam/`

| fichero | que hace |
|---|---|
| `cc.py`, `pre.py`, `mtry.py`, `mrows.py` | los de la r47, reapuntados a r48 |
| `mctx.py` | **nuevo**: imprime un RANGO de instrucciones de los dos lados (no solo las distintas) a partir del ultimo `mj.json`. Es lo que hace legible un racimo |
| `rtl.py`, `fnrtl.py` | volcados de `cc1plus` y extraccion por funcion; `rtl.py` guarda **stderr en `sched.log`**, que es donde vive la traza de `sched2` |
| `lib48.py`, `t1..t16.py` | banco de variantes de `TrackCar` (helper `CamCopy`, pines del racimo C, constructor, punteros, `CamLook`/`CamLook3`) |
| `ice.py`, `i1.py`, `i2.py` | banco de variantes de `ICEMover` |
| `stry.py`, `st.py`, `s1.py`, `inc/` | sombra de `Libs/Support/Utility/` y banco de `UTLVector.h` |
| `micro_tc.cpp`, `micro_cm.cpp`, `micro_ice.cpp`, `micro_tp.cpp` | micros de `TrackCar`, `CameraMover`, `ICEMover` y `TrackCop` |

**Aviso de la regla 7 que costo dos ensayos en blanco**: dentro de
`src/Speed/Indep/Src/Camera/` conviven los dos finales de linea.
`Movers/TrackCar.cpp` y `CameraMover.cpp` son **LF puro** (0 CRLF), y
`ICE/ICEMover.cpp` y `Libs/Support/Utility/UTLVector.h` son **CRLF entero**. Un
banco de variantes con `'\n'` fijo casa 0 de N en los segundos **sin avisar**
(el `assert` es lo unico que lo delata).

---

## 9. Abierto, por orden de premio

1. **`Update__8ICEMoverf` (3.868 B)** — §5. Ya no es «territorio agotado»: hay
   una diferencia nombrada (el pseudo del `fctiwz` contra el de `a` de
   `SignedMod`) y una ventana de 116 B. Lo siguiente es el `.greg`/`.lreg` sobre
   esa ventana y la palanca 2 sobre la copia del bucle.
2. **`__static_initialization_and_destruction_0` (3.604 B)** — sin vetas
   pendientes desde la r47; faltan 107 rtx no-NOTE y no estan en `Camera/**`.
   No lo toque.
3. **`TerrainVelocityNoise` (1.192 B)** — §6.bis. Ya esta hecha la separacion:
   es **`sched1`**, y lo que hay que mover es el LUID (o la prioridad) del
   `elf_high` del literal 0.0f, no el reparto de r7..r11.
4. **`_Storage` (1.156 B)** — §6, fuera de territorio.
5. **`Update__19TrackCarCameraMoverf` (992 B)** — §1 y §4. El camino esta
   nombrado hasta el final (racimo C -> racimo B) pero **la unica palanca que lo
   recorre son tres locales que el DWARF dice que no existen**.
