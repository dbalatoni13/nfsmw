# r61-plat — zPlatform y zCamera

**Titular 1.** `ActualReadJoystickData__Fv` (1.588 B) **cierra al 100 %**, y con ella
`zPlatform` se queda en **0 de 136 funciones con el código distinto**. Era la última.

**Titular 2, y es el que cambia el plan de la ronda.** Ni `zPlatform` ni `zCamera`
estaban «a una función» de promocionar. Las dos tienen el `.text` **en otro orden**:
`zPlatform` 134 descolocadas de 137, `zCamera` 427 de 453. Medido, con control sobre
tres unidades ya promocionadas (`zBWare`, `zDynamics`, `zRender`: **ORDEN PERFECTO** las
tres). El censo de las 19 unidades no enlazadas está en la sección 4: **882 funciones
por recolocar**, y las que el frente creía más cerca son las peores.

Nada de esto toca `configure.py`, `config/GOWE69/*` ni `splits.txt`. **Cero correcciones
de `lcfix.py` pendientes** (ninguna edición ha movido un solo byte de `.rodata`).

---

## 1. zPlatform — `ActualReadJoystickData` de 9 filas a CERO

Base de la ronda (árbol limpio, `a136a111` ya en HEAD):
`1.588/1.588 B, 99,88665 %, 9 filas`, `.o` sha1 `f840e5a03e2c54e9e822e4f72f84507e6bfd74fc`.

Las nueve filas eran de registro: el objetivo da **r8 / r10 / r11** a los tres `data`
de los ejes (RightX / RightY / LeftX) y nosotros dábamos **r8 a los tres**; la novena
era su consecuencia (`stb r8` contra `stb r0`).

### 1.1 Pieza (1): el `asm` con restricciones — la receta 4.1 del informe de vedas, CONFIRMADA

El `asm volatile("extsh 0,8; mr 9,0")` de la r60 cableaba r8 **y no producía el valor
que se guarda**. Pasado a restricciones:

```c
asm volatile("extsh %0,%1; mr 9,%0" : "=r"(v) : "r"(data) : "r9");
```

(y borrando la línea `asm volatile("extsh 0,8; mr 9,0");`, que ya no hace falta)

| ensayo | tamaño | fuzzy | filas |
|---|---:|---:|---:|
| base | 1.588 | 99,88665 % | 9 |
| **T7** | **1.588** | **99,89925 %** | **8** |

La fila 191 pasa de `stb r8,0x4(r29)` a `stb r0,0x4(r29)`, que es **exactamente** el
observable que pedía la auditoría. `.o` sha1 `513ae9d4307539aa2d536d4b47fce6da390cd4f3`
(dos compilaciones idénticas).

### 1.2 Pieza (2): la cantidad fantasma sobre r8

Con r8 ocupado desde el final del bloque de RightX hasta el `extsh`, RightY y LeftX ya no
lo pueden reutilizar y caen en r10/r11. El cierre del fantasma se mete como **operando
extra del `asm` que ya existe**, para no gastar un insn:

```c
register int guard asm("r8");
asm("" : "=r"(guard));                       // tras el bloque de RightX
...
asm volatile("extsh %0,%1; mr 9,%0" : "=r"(v) : "r"(data), "r"(guard) : "r9");
```

| ensayo | tamaño | fuzzy | filas | qué queda mal |
|---|---:|---:|---:|---|
| **G4** = T7 + fantasma r8 | **1.588** | **99,92443 %** | **6** | sólo el grupo r18/r19 |
| G1 = T7 + fantasma cerrado con un `asm` propio | 1.588 | 99,86146 % | 11 | rompe r14/r15 y f25/f26/f27 |
| G5 = fantasma abierto tras RightY | 1.588 | 99,874054 % | 10 | RightY sin arreglar |

**El pin directo sigue siendo negativo, y ahora con la causa:** `register int data
asm("r11")` en LeftX da **1.584 B / 19 filas** y `asm("r10")` en RightY da **1.576 B /
23 filas**. No fallan por el asignador: **encogen la función**, porque con un registro
duro GCC hace coalesce del `mr rX,r0` que el objetivo SÍ tiene. El fantasma no toca el
RTL y por eso el tamaño no se mueve.

### 1.3 Pieza (3): el relleno de rango de vida, calibrado con `lreg.py`

G4 rompía el grupo r18/r19 (el empate histórico de la r47). `lreg.py` lo explica al
número: pseudo **403** = la constante `0x43300000` del conversor int→float y pseudo
**400** = `high(RealTimer)`, los dos con `n_refs 5` y sus `live_length` separados por 2.
La prioridad de `allocno_compare` es `floor(100000/live)` y hace falta que la de 403
sea **estrictamente mayor**.

Cada `asm("" : "+r"(guard));` suma **2** al `live_length` de todo lo que esté vivo ahí.
Lo medido con `lreg.py`, escalón a escalón (los `live` son de la posición **tras el
bloque de RightY**, que es donde los saqué):

| estado | live 403 / 400 | prio | 382 / 232 | prio | filas |
|---|---|---|---|---|---:|
| T7 (sin fantasma) | 636 / 638 | **157 / 156** OK | 718 / 722 | **111 / 110** OK | 8 |
| G4 (fantasma, 0 rellenos) | 638 / 640 | 156 / 156 EMPATE | 720 / 724 | **111 / 110** OK | 6 |
| +1 relleno | 640 / 642 | **156 / 155** OK | 722 / 726 | 110 / 110 EMPATE | 11 |
| +2 | 642 / 644 | 155 / 155 EMPATE | 724 / 728 | **110 / 109** OK | 14 |
| +3 | 644 / 646 | **155 / 154** OK | 726 / 730 | **110 / 109** OK | 8 |
| **final (+5, otra posición)** | **648 / 650** | **154 / 153** OK | **730 / 734** | **109 / 108** OK | **0** |

La prioridad de `allocno_compare` es `floor_log2(n_refs)*n_refs/live*10000`, o sea
`floor(100000/live)` para 403/400 y `floor(80000/live)` para 382/232. Como los `live` de
cada pareja van separados por una constante (2 y 4), **el empate aparece y desaparece de
forma periódica al añadir rellenos**, y hay que caer en un escalón donde las DOS parejas
salgan estrictas. En el estado final las cuatro prioridades son estrictas: 154>153 y
109>108, y `lreg.py` da `403→r19, 400→r18, 382→r15, 232→r14`, que es el reparto del
objetivo.

**La posición del relleno importa además de N.** Los tres barridos completos, mismo `N`,
mismos `asm`, sólo cambia dónde van (filas, con el tamaño clavado en 1.588 B en los 22
ensayos):

| N | delante del `extsh` (la buena) | tras el bloque de RightY | delante del bloque de RightY |
|---:|---:|---:|---:|
| 1 | 11 | 11 | 17 |
| 2 | 12 | 14 | 17 |
| 3 | 6 | 8 | 11 |
| 4 | 11 | 13 | — |
| **5** | **0** | 2 | — |
| 6 | 6 | 8 | — |
| 7 | 5 | 7 | — |
| 8 | 17 | 19 | — |

El mejor de la familia «tras RightY» (N=5, **dos filas**) no deja ni una diferencia de
registro: sólo el `stb r10,0x7(r29)` seis ranuras tarde. Colocados **detrás** del
`extsh` no hacen nada: `guard` ya está muerto y DCE se lleva la pareja entera (N=1,2,3:
los tres **idénticos a G4**, `live` sin moverse). Es la regla 1 de
`memory/nfsmw-cantidad-fantasma.md`, reconfirmada aquí con tres medidas.

### 1.4 Estado final de zPlatform

```
fndiff  zPlatform ActualReadJoystickData__Fv -> target=100.0%  ours=100.0%  size=1588/1588
fncmp   Speed/Indep/SourceLists/zPlatform    -> 0 de 136 funciones con el CODIGO distinto
linkdelta zPlatform                          -> .text +0, resto IGUAL
mangfix zPlatform --check                    -> sin alias desfasados
```

**Sello (regla 7): TRES compilaciones consecutivas →
`5cdb764ca70d5c2db32d471add8de3172b0fc6b2`** (con el bloque de comentario dentro;
sin él daba `5b74ee77bad7f244b606fd2dc837a165be79466d` — una línea de comentario mueve
`.debug_line` y con ella el sha1, el `.text` es idéntico).

**No se ha movido un byte de datos.** `dolwhere.py zPlatform` da **95.754 B distintos
ANTES y 95.754 B DESPUÉS** (medido las dos veces, con el árbol base restaurado para la
primera). Ningún `$LC` se ha desplazado, así que **cero correcciones de `lcfix`
pendientes y cero entradas venenosas para otras unidades**.

---

## 2. El titular incómodo: zPlatform y zCamera no promocionan, y no es por código

`trypromo.py zPlatform` sigue dando **DOL ROTO**, con la función al 100 % y la unidad a
0/136. `dolwhere` señala un bloque contiguo de **29.300 B** que empieza exactamente en
`bDoWithStack +0x0` — el principio del `.text` de la unidad — y se re-sincroniza justo
al final de ella. Ahí el objetivo empieza con `stwu r1,-8(r1); mflr r0; ...` y nosotros
con `li r3,0; blr`: **es otra función**.

```
textorder.py zPlatform -> DESCOLOCADAS: 134 de 137, SALTOS DE DELTA: 85
textorder.py zCamera   -> DESCOLOCADAS: 427 de 453, SALTOS DE DELTA: 111
```

**Control obligatorio, y pasa**: las mismas herramientas sobre tres unidades ya
promocionadas dan `>>> ORDEN PERFECTO` — `zBWare` (0 de 240), `zDynamics` (0 de 42),
`zRender` (0 de 6) — y eso **con 126, 9 y 23 funciones «SOLO NUESTRAS»** de más. O sea
que las funciones sobrantes no ensucian la medida: el desorden es real.

El desorden **no es de ficheros**: la secuencia del objetivo respeta el orden de los
`#include` del SourceList. Es **intra-fichero**, y las que bailan son las pequeñas
(`FlushCaches` 32 B, `bDoWithStack` 8 B, `ServicePlatform` 4 B, `FinishedRenderingFEngLayer`
4 B): la firma del orden de emisión por **punto de uso** (`memory/nfsmw-orden-de-emision`).

---

## 3. zCamera — dos negativos medidos, y un racimo que sí cede

Base: `.o` sha1 `e74ba2da8c97e6b172d671c2b4916fd348af2d4b`,
`fncmp` = 3 de 453 (8.464 B), `linkdelta` `.text +0 resto IGUAL`.

| función | tamaño | fuzzy | filas |
|---|---:|---:|---:|
| `Update__8ICEMoverf` | 3.868 | 99,90693 % | 15 |
| `__static_initialization_and_destruction_0` | 3.604 | 99,839066 % | 6 |
| `Update__19TrackCarCameraMoverf` | 992 | 99,629036 % | 15 |

### 3.1 `__static_init` — la receta 1 de la auditoría r60b es NEGATIVA

Aplicada literalmente (`static inline bAngle _cam_pin(bAngle a) { asm("" : "+r"(a)); return a; }`
sobre `Cubic.cpp:386`):

```
base      3.604/3.604 B   99,839066 %    6 filas (549/557/558/561/562/564)
receta 1  3.604/3.604 B   98,234184 %   58 filas
```

El **control de tamaño se cumple** (3.604 B clavados, el `asm` no gasta ranura), así que
la medida vale — y **falla su propio observable**: la fila 549 NO pasa a `li r9,0x38e`;
el `li 0x38e` se va a la fila **533** y arrastra el racimo entero de literales de
Demo1/Demo2/PreviousEye (521..614). Sello del ensayo:
`38a0b9fb358bcb4215634b68c7d811d48a5959da`; árbol devuelto a la base.

**Por qué falla, con la medida que lo explica.** `lreg.py zCamera
__static_initialization_and_destruction_0` da **437 pseudos**, y r9/r10/r11 los comparten
**decenas** de allocnos de vida corta (sólo con prioridad 3333 hay veintitantos). Meter
un insn de `asm` no sube `n_refs` de la cantidad que importa: le cambia el `live_length`
a todos los vecinos a la vez y rebaraja el racimo entero. **La palanca de la sección 1.3
sí sirve aquí, pero exige identificar antes los TRES allocnos (SYM/LIT/CONST) en el
`.greg`** — en una función de 437 pseudos eso no se hace de oído, y es el trabajo que
queda. El negativo está escrito junto a la función en `Cubic.cpp`.

### 3.2 `Update__19TrackCarCameraMover` — cinco negativos, y una anatomía nueva

Lo primero: **las 15 filas de hoy no son las 4 que da por supuestas la auditoría**
(«PASO 1 reproducir r51+r52 → 4 filas»); esos andamios no están en el árbol.

Las 15 se separan en tres racimos de **naturaleza distinta**, cosa que ningún informe
anterior dice:

- **R1** (107/109/111, `Look = *GetGeometryPosition()`) — las tres cargas salen en el
  mismo orden en los dos lados y los valores caen en los **mismos** registros; sólo
  difiere el **orden de los tres `stfs`**. Permutación pura de `sched2`.
- **R2** (113/116/119/120/121/123, `displacement /= distance`) — igual: mismos
  registros, otro orden.
- **R3** (134..143, `look_offset.x/y/z`) — al revés: **mismo orden, otros registros**
  (objetivo f10/f11/f12, nosotros f12/f0/f13). Con el `REG_ALLOC_ORDER` de rs6000
  (`f0, f13, f12, f11, f10, ...`) eso significa que en el objetivo **f0 y f13 están
  ocupados** en ese punto y en el nuestro no; y además su orden de prioridad es
  `y > x > z` y el nuestro `x > y > z`.

Medido (992/992 B en las cinco — ninguna gasta ranura):

| ensayo | fuzzy | filas |
|---|---:|---:|
| base | 99,629036 % | 15 |
| T1 fantasmas `fr0`+`fr13` sobre R3, cerrados en `dT` | 97,97177 % | 19 |
| T1a sólo `fr0` | 98,145164 % | 13 |
| T1b sólo `fr13` | 98,145164 % | 13 |
| T2 `look_offset` en orden z,x,y | 99,616936 % | 15 |
| T6 `look_offset` en orden y,x,z | 99,620964 % | 15 |

**El dato que sí sirve:** T1a y T1b **borran R2 entero** (las filas 119/120/121/123
desaparecen) sin mover una sola instrucción. R2 cede a la cantidad fantasma aunque el
fantasma esté colocado *detrás* de él. Lo que hunde las dos es R3: al bloquear un solo
registro el reparto de `look_offset` se descoloca más y además desplaza el
`lwz r4,0xa0(r31)`. Quien lo retome tiene que bloquear f0 **y** f13 a la vez **sin meter
el insn de cierre dentro de R3** — en `zPlatform` eso se resolvió metiendo el consumo del
fantasma como operando extra de un `asm` que ya existía, y aquí **no hay ninguno: hay que
fabricarlo fuera de R3**. Los cinco negativos están escritos junto a la función en
`TrackCar.cpp`.

### 3.3 `Update__8ICEMoverf` — NO tocada

15 filas, 3.868/3.868 B, 99,90693 %. El andamio `_r60_ec[20]` de `ICEReplay.cpp:196-226`
sigue en pie (verificado: los tres números de zCamera son idénticos antes y después de
mis ediciones, que son sólo comentarios). La auditoría la manda por `bMath.hpp`, que es
**cabecera compartida**: regla 6, no se toca.

---

## 4. El censo del orden — lo que hay que llevarse de esta ronda

`textorder.py` sobre las 19 unidades no enlazadas que tienen los dos `.o`. «a mover» es
`nº emparejadas − subsecuencia creciente más larga`: el mínimo de funciones que hay que
cambiar de sitio.

| unidad | funciones | descolocadas | **a mover** |
|---|---:|---:|---:|
| zPhysics | 718 | 718 | 157 |
| zAI | 1030 | 1030 | 136 |
| zFe2 | 1307 | 1290 | 98 |
| **zCamera** | 453 | 453 | **79** |
| zEAXSound | 773 | 773 | 78 |
| zPhysicsBehaviors | 1120 | 1120 | 59 |
| zFe | 921 | 921 | 52 |
| zEagl4Anim | 318 | 318 | 49 |
| **zPlatform** | 136 | 136 | **48** |
| zGameplay | 768 | 767 | 37 |
| zSpeech | 703 | 703 | 25 |
| zEAXSound2 | 930 | 930 | 24 |
| zWorld2 | 357 | 357 | 24 |
| zWorld | 582 | 582 | 11 |
| zEcstasy | 539 | 539 | **3** |
| zLua | 537 | 537 | **1** |
| zTrack | 259 | 259 | **1** |
| zMisc | 450 | 450 | **0** |
| madidct | 3 | 3 | **0** |
| **TOTAL** | | | **882** |

Tres consecuencias, y las tres son de reparto de agentes:

1. **La cola de «bytes de función» y la cola de «unidades que promocionan» son colas
   distintas.** `zPlatform` está en código al 100 % y le faltan 48 movimientos;
   `zLua` tiene 1, `zTrack` 1, `zEcstasy` 3, `zMisc` y `madidct` **cero**. Si la métrica
   es `linked`, el siguiente encargo barato no es zPlatform: es mirar qué le falta a
   `zMisc` y a `zTrack`, que ya tienen el orden.
2. **Ninguna promoción va a salir de cerrar una función más** en zPlatform, zCamera,
   zWorld2, zEagl4Anim, zPhysics o zAI. Cerrarlas sigue valiendo (`matched`), pero el
   informe de cada agente debería decirlo para que no se prometa un `linked` que no llega.
3. **El orden se mide en milisegundos y no enlaza nada** (`textorder.py` abre los dos
   `.o` y ya está). Debería ser el **primer** comando de cualquier encargo de unidad,
   antes que `fncmp`.

---

## 5. Propuestas para el jefe

- **`lcfix.py`: CERO correcciones pendientes.** Ninguna edición ha tocado `.rodata`
  (`dolwhere` idéntico antes/después en zPlatform; en zCamera sólo he dejado comentarios).
  Ninguna entrada venenosa para otra unidad.
- **`keep.lst` / `splits.txt` / `configure.py`: CERO propuestas.** `gapchk` da 1 muerto
  sin keep.lst en zCamera (4 B) y ninguno en zPlatform; con `linkdelta` en `IGUAL` no
  hay nada que cobrar ahí, y tras restar lo que ya está en `keep.lst` no queda frente.
- **`checksplits` LIMPIO** (0 solapes, 0 cortes), **`prefijochk` LIMPIO**,
  **`mangfix --check` al día** en las dos unidades.
- Ficheros tocados (los tres son de mis unidades en exclusiva; ninguna cabecera
  compartida):
  - `src/Speed/GameCube/Src/JoyE.cpp` — **cambio de código** (la función al 100 %) + bloque r61.
  - `src/Speed/Indep/Src/Camera/Movers/Cubic.cpp` — **sólo comentario** (negativo r61).
  - `src/Speed/Indep/Src/Camera/Movers/TrackCar.cpp` — **sólo comentario** (5 negativos r61).
- **Corrección al informe de vedas r60b**: su entrada 1 (`__static_init`, coste BAJO)
  pasa a **negativa con cifra**; su entrada 4 (`ActualReadJoystickData`) queda **cerrada**;
  su entrada 8 (`TrackCar`) hay que rebasearla — la base son 15 filas, no 4.
