# r36e — zCamera: cerrada `TrackCop::Update` (948 B) con DOS barreras, y el mecanismo de las otras cuatro

**Cerrado: `Update__19TrackCopCameraMoverf`, 948 B, `fndiff` = 100.0 % (948/948).**
Unidad: de **11.928 B pendientes a 10.980**, de 7 funciones con código distinto a 6.
`matched` de zCamera 90,458 % → **91,216 %** (114.028 / 125.008).

Verificación (obligatoria por el brief):

```
python scripts/fndiff.py Speed/Indep/SourceLists/zCamera Update__19TrackCopCameraMoverf
# Update__19TrackCopCameraMoverf  target=100.0%  ours=100.0%  size=948/948

python scripts/fncmp.py Speed/Indep/SourceLists/zCamera      # antes / después
    7 de 453 funciones ... 11928 B      ->   6 de 453 funciones ... 10980 B
    (ninguna función empeora; las otras seis salen con la MISMA cifra que antes)

python scripts/lcfix.py --check   ->  todas las entradas @lc estan al dia
```

Secciones del `.o` idénticas antes y después (`.text` 136.024, `.rodata` 8.488,
`.data` 2.884, `.bss` 1.928): el parche no mueve ni un literal.

Un solo fichero tocado: `src/Speed/Indep/Src/Camera/Movers/TrackCop.cpp`.
Sin commits.

---

## 1. `TrackCop::Update`: la barrera de ranura Y la de argumento, **las dos**

La r36d dejó esta función como «techo de la barrera de ranura»: 12 colocaciones,
todas en 2 filas, con el residuo migrando entre tres cálculos de dirección. Lo
que faltaba no era otra colocación: era **una segunda palanca, de otra familia**.

```c
bVector3 hcomp;
bCross(&hcomp, &displacement, &up);
asm("" : "+m"(hcomp));                 // 1. barrera de RANURA
bScale(&hcomp, &hcomp, vert_comp);

bVector3 look_offset;
look_offset.x = 0.0f;
look_offset.y = 0.0f;
look_offset.z = 0.0f;
bVector3 *dst = &look_offset;
asm("" : "+r"(dst));                   // 2. barrera de ARGUMENTO
eMulVector(dst, CarToFollow->GetGeometryOrientation(), dst);
```

Cero bytes las dos. La cadena medida, instrucción a instrucción:

| # | qué | filas | dónde queda el residuo |
|---|---|---|---|
| base | — | **2** | `mr r5,r3` / `stfs f30,0x58(r1)` intercambiados |
| u1 | sólo `"+m"(hcomp)` (= el k1 de la r36d) | 2 | `addi r4,r4,0x48` una ranura antes |
| u2/u3/u4 | u1 + otra ranura sobre `look_offset` | 2 | `addi r3,r1,0x68` una ranura antes |
| w1 | u1 + local + `"+r"` sobre el 2.º argumento (`ori`) | 2 | `addi r4,r4,0x48` dos ranuras antes |
| w3 | u1 + locales y `asm` para los DOS argumentos | 2 | `addi r4,r4,0x48` tres ranuras antes |
| **w2** | **u1 + local + `"+r"` sólo sobre el 1.er/3.er argumento (`dst`)** | **0** | — |
| w4 | w2 sin la barrera de `hcomp` | 4 | vuelven `stfs 0x58` y `stfs 0x60` |
| x1 | w2 con `asm("" : : "r"(&look_offset))` (sin local) | **25** | destroza el bloque |

Tres cosas que valen para todo el proyecto:

1. **El uso 5 de la barrera selectiva (r36d-fe, «orden de preparación de
   argumentos») vale también cuando el argumento NO es un literal.** Allí la
   firma era un `lis @ha` de una constante fuera de sitio; aquí es un
   `addi rN,r1,ofs` de una **dirección de pila**, y la cura es la misma: la
   local *más* su `asm` propio. Sin la local (x1, `asm` de sólo lectura sobre
   `&look_offset`) el resultado es 12 veces peor: la receta «una local por
   argumento» no es cosmética.
2. **No hacen falta las barreras de TODOS los argumentos: hacen falta las de los
   que están descolocados.** w3 (los dos argumentos) es peor que w2 (uno). Es el
   matiz que le falta a la regla de r36d-fe §1.3, que allí decía «hacen falta
   las tres»: lo que hace falta es una por argumento *descolocado*.
3. **Una palanca de ranura y una de argumento se suman.** Cada una por separado
   deja exactamente 2 instrucciones fuera de sitio y sólo cambia *cuál*; juntas,
   0. Es la primera vez que se documenta la combinación, y explica por qué la
   r36d se quedó atascada barriendo 12 colocaciones de una sola familia.

**Caveat honesto**: `regmap` dice ahora `dst  <-- SOLO NUESTRA` — la local no
está en el DWARF del original (que sí lista las 15 restantes con el mismo
registro/ranura). El objeto es idéntico byte a byte, así que la forma es
funcionalmente correcta, pero la fuente lleva una local de más, igual que
llevan los `asm`.

---

## 2. El permutador CIEGO: **1.450 variantes, cero mejoras** — y por qué

Lo pedía la r36d para `ICEMover::Update` («nunca se le ha pasado»). Lo pasé a las
cuatro funciones grandes, catálogo entero, sin `--guided`:

| función | variantes | resultado |
|---|---|---|
| `TrackCopCameraMover::Update` | 266 | ninguna mejora (score 0,995781) |
| `CameraMover::TerrainVelocityNoise` | 234 | ninguna mejora (0,959732) |
| `TrackCarCameraMover::Update` | 212 | ninguna mejora (0,939516) |
| `ICEMover::Update` | 738 | ninguna mejora (0,985522) |

> **El dato de método, y es caro**: `TrackCop::Update` **sí era cerrable**, y el
> permutador ciego con las 266 variantes de su catálogo no la tocó. El catálogo
> del permutador es de FORMAS DE FUENTE (mover sentencias, partir declaraciones,
> conmutar operandos...) y **no contiene ninguna variante con `asm`**. Los cinco
> ejes de barrera del brief son ORTOGONALES al permutador: que el permutador
> agote una función no dice nada sobre las barreras, y al revés. Conviene no
> volver a leer «permutador ciego agotado» como «techo».

(La r36d ya avisaba de lo simétrico en r36d-fe §1.2: `FoundEntry` había resistido
al permutador ciego con las 114 variantes de su catálogo y cayó con una barrera.)

---

## 3. `TerrainVelocityNoise` (1.192 B, 13 filas): **una sola causa, y es un registro de más**

Las 13 filas son un único racimo, índices 115-135, `CameraMover.cpp:212`
(`bClamp(bDot(...), 0.0f, accel_max) * 0.05f`). `regmap`: **IDÉNTICO**, 19
locales iguales, 0 distintas.

Los cinco literales del bloque y el registro que lleva su dirección:

| literal | `lis` objetivo | `lis` nuestro |
|---|---|---|
| 20.0f (`accel_max`) | 115 → **r9** | 115 → r10 |
| 0.05f | 117 → r8 | 119 → r7 |
| 0.5f | 119 → r10 | 123 → r11 |
| 0.15f | 123 → r11 | 127 → r9 |
| **0.0f** (el `MINIMUM` de `bClamp`) | **128 → r9 (REUSA)** | **117 → r8** |

El objetivo gasta **cuatro** registros para cinco direcciones: el `lis r9` de
128 pisa el r9 definido en 115, cuyo último uso es el `lfs f9,0(r9)` de 126. Esa
reutilización crea una **antidependencia WAR** que impide a `sched2` subir ese
`lis`, y por eso se queda pegado a su `lfs` (129). Nosotros damos cinco
registros distintos, no hay antidependencia, y `sched2` sube los cinco. El
intercambio `f0`↔`f12` dentro de `bDot` (índices 116/120/121/125) es la cola de
lo mismo.

O sea: **no es planificación, es UN REGISTRO DE MÁS en el reparto**; la
planificación es la consecuencia. Y por eso ni la barrera ni el permutador la
mueven — hay que quitarle un registro al bloque, no reordenar la fuente.

Medidas negativas (base 13 filas):

| # | qué | filas |
|---|---|---|
| e1 | `dotv` en local + `asm("" : "+f"(dotv))` | 23 |
| e2 | `dotv` y `amax` en locales + un `asm` por cada uno | 22 |
| e3 | sólo `amax` en local + `asm` | 33 |
| m1 | `asm("" : "+f"(accel))` tras la sentencia | 16 |
| m2 | `asm("" : "+f"(f_speed_magnitude))` antes | 20 |
| m3 | intercambiar las dos sentencias siguientes | 15 |

### Y de paso, resuelto el `// TODO is this order correct?` de `bMath.hpp:197`

`bClamp(float)` lleva ese TODO encima desde hace rondas. **Medido: el orden de
sus parámetros NO es observable aquí.** Escribí dos ayudantes `static inline`
locales en `CameraMover.cpp` con el mismo cuerpo y los parámetros en otro orden
—`(MINIMUM, a, MAXIMUM)` y `(MAXIMUM, a, MINIMUM)`— y llamé con los argumentos
reordenados: **las dos dan el mismo objeto que la base, 13 filas exactas**. GCC
2.9 sustituye las constantes en el cuerpo del `inline` y el orden de evaluación
de los formales no llega al código. El TODO puede cerrarse como «indecidible por
esta vía».

---

## 4. `ICEMover::Update` (3.868 B, 15 filas): el original tiene TRES pseudos en `SignedMod`, nosotros dos

Con la fuente que la r36d dejó confirmada por DWARF, las 15 filas son **una sola
permutación en una ventana de 30 instrucciones** (índices 589-617). Alineadas:

| papel | objetivo | nuestro |
|---|---|---|
| `NumKeys` crudo (el `lhz`, sin extender) | r7 | r8 |
| actual de `SignedMod` (el `lwz 0x15c(r1)` del `fctiwz`) | r8 | r9 |
| copia de trabajo de `a` (el `mr r11,·`) | r11 | r11 |
| **resultado de `SignedMod` (`frame`)** | **r8** (= el del actual) | **r11** (= el de la copia) |
| `NumKeys` extendido para `GetKey` | r9 | r9 |
| `b` extendido (`extsh r10,r0`) | r10 | r10 |
| `bMax` de `Clamp` | r11 | r10 |
| `bMin` de `Clamp` | r10 | r9 |

La diferencia de raíz está en dos instrucciones:

```
objetivo:  mr r11,r8 / cmpwi r8,0  ... subf r8,r0,r11   / li r8,0
nuestro:   mr r11,r9 / cmpwi r11,0 ... subf r11,r0,r11  / li r11,0
```

El objetivo prueba `while (a < 0)` sobre el registro del **actual** (r8) y
devuelve el resultado **a ese mismo registro**; nosotros probamos sobre la copia
(r11) y coalescemos el resultado con ella. Son tres cantidades vivas (actual,
copia, valor de retorno) contra nuestras dos: el objetivo gasta r7..r11 y
nosotros r8..r11. Es `cse` eligiendo otro representante de la clase de
equivalencia {actual, formal} para el primer test, y el coalescing del retorno
detrás.

Todo lo probado, negativo (base 15 filas de `fncmp` / 16 de `sbs`):

| # | qué | filas |
|---|---|---|
| a | `asm("" : "+r"(frame))` tras la sentencia | 23 |
| b | `asm("" : "+r"(frame) : "r"(pShake))` (eje «a quién») | 23 |
| c | `asm("" : "+r"(pShake) : "r"(frame))` | 23 |
| i1 | los dos argumentos de `SignedMod` en locales, sin barrera | **16 = base** |
| i2 | i1 + un `asm` por argumento | 26 |
| i3 | sólo el 1.er argumento en local + `asm` | **16 = base** |
| k1-k4 | cuatro formas de `ICEShakeTrack::GetKey` (local para el clamp, para `NumKeys-1`, comparación invertida, vía `GetNumKeys()`) | **16 = base las cuatro** |
| p1 | cantidad fantasma `asm("r7")`, cerrada sobre `pShake` | 23 |
| p2 | cantidad fantasma `asm("r7")`, cerrada sobre `pKey` | 32 |
| p3 | cantidad fantasma `asm("r12")`, cerrada sobre `pKey` | 32 |
| r1 | `SignedMod` con `return a;` dentro del `if` y `return 0;` fuera | 20 |
| r2 | igual pero `return a - c;` (sin asignar `a` en el último paso) | 20 |
| — | permutador ciego, 738 variantes | sin mejora |

**Las dos palancas nuevas del brief (cantidad fantasma y eje «a quién») están
medidas y son negativas aquí**, pese a que el criterio de la fantasma se cumple
al pie de la letra (al objetivo le sobra una cantidad viva). Y `GetKey`, que es
territorio nuestro (`Camera/ICE/ICEManager.hpp`), es indiferente a la forma: GCC
canonicaliza las cuatro al mismo RTL.

---

## 5. `TrackCar::Update` (992 B, 15 filas): tres racimos y una prueba de que NO es `bFill`

| racimo | índices | qué |
|---|---|---|
| A | 107-111 | `Look = *CarToFollow->GetGeometryPosition()`: el objetivo almacena **z, x, y**; nosotros **x, y, z**. Las tres CARGAS son z,x,y en LOS DOS. |
| B | 113-124 | `displacement /= distance`: el objetivo multiplica y almacena z,x,y; nosotros x,z,y |
| C | 134-143 | `lfsx`/`stfs` de `TrackCarLookOffset[XYZ]`: mismas instrucciones, otros FPR (f10/f11/f12 contra f12/f0/f13) |

**No es el orden de `bFill`**, y ahora se puede afirmar con una prueba directa:
`TrackCop::Update`, que esta ronda queda al 100 %, emite los tres `stfs` de
`bVector3 up(0.0f, 0.0f, 1.0f)` **en la misma línea `bMath.hpp:905`** y en orden
**x, y, z** — el nuestro. Misma inline, mismo fichero, misma línea, otro orden:
es un empate de planificación resuelto distinto según el bloque, no la fuente de
la cabecera. (Esto retira la sospecha que la r36b/r36d dejaban sobre
`bCopy`/`operator/=` en `bMath.hpp`.)

Medidas (base 21 filas de `sbs`):

| # | qué | filas |
|---|---|---|
| c1 | `asm("" : "+m"(Look))` después de la copia | 25 |
| c2 | `asm("" : "+m"(displacement))` antes del `/=` | 23 |
| c3 | las dos | 25 |
| c4 | `asm("" : "+m"(Look))` antes de la copia | 25 |
| e1 | la receta que cierra TrackCop (ranura sobre `hcomp` + argumento sobre `dst`) | 24 |
| d1 | *diagnóstico*: escribir a mano `Look.z=`, `Look.x=`, `Look.y=` | **arregla el racimo A** y sube a 37 (rompe B) |
| — | permutador ciego, 212 variantes | sin mejora |
| — | permutador `--depth 2 --anneal 0.02`, 612 variantes (ronda 1) | sin mejora: un solo paso lateral (`swap:swap@537`) con el MISMO score |

d1 confirma que lo que decide es el orden de emisión de los tres `stfs`, pero
escribirlo a mano contradice el mapa de líneas (que los pone dentro de
`bMath.hpp:905`) y sale más caro. Queda como el racimo que hay que atacar
primero: A es la causa y C el efecto.

---

## 6. `LoadCameraShakes` (168 B): **la r36d se equivocó de variable, y lo dice el mapa de líneas**

La r36d dio por localizada la causa: «`warned_overflow` está vivo en r0 en el
original y nosotros borramos el store muerto». **Es falso.** El mapa de líneas
del original atribuye las DOS instrucciones a la misma línea, y no es la de
`warned_overflow`:

```
11 8007EFD8  lwz r28, 0x8(r29)     <-- ICEManager.cpp:6814   (num_tracks)
12 8007EFDC  li  r0,  0x0          <-- ICEManager.cpp:6816   (el `for`)
13 8007EFE0  addi r30, r29, 0xc    <-- ICEManager.cpp:6815   (track)
14 8007EFE4  li  r27, 0x0          <-- ICEManager.cpp:6816   (el `for`)
15 8007EFE8  cmpw r0, r28          <-- ICEManager.cpp:6816
```

**No hay ni una instrucción en la línea de `warned_overflow = false`**: en el
original también está muerta y también la borra GCC. Los dos ceros son de la
línea del `for`: uno es el contador (r27) y el otro es el operando izquierdo de
la guarda de entrada. Que el DWARF le dé r0 a `warned_overflow` sólo dice que el
asignador le adjudicó ese registro a un rango vacío.

Comprobado con cinco formas de la asignación, **todas dan el objeto idéntico a
la base** (164 B, 6 filas): moverla detrás de `num_tracks`, detrás de `track`,
escribirla como `= 0`, y la forma `while` con `int i = 0;` fuera del bucle. La
única que cambia algo es la barrera de sólo lectura justo después
(`asm("" : : "r"(warned_overflow))`): **168 B pero 9 filas**, peor que las 4 de
la r36d.

Lo que queda de verdad: el objetivo compara `cmpw r0,r28` (un cero *aparte*
contra `num_tracks`) y nosotros `cmpw r28,r27` (el contador contra
`num_tracks`), o sea que en el original `i` tiene **dos pseudos** —uno para la
guarda de entrada y otro para el cuerpo— y en el nuestro uno solo. Medido
también: `for (int i = 0; num_tracks > i; i++)` pliega `i` a constante y da
`cmpwi r28,0` (18 filas), así que el plegado sí ocurre cuando el inmediato cabe;
el original NO plegó. Es una duplicación de la variable de inducción en `loop.c`,
no un store muerto.

**Vedas nuevas para esta función**: `warned_overflow` (cualquier forma), y la
barrera de sólo lectura pegada a la asignación.

---

## 7. Lo que cae fuera de mi territorio (propuestas con la medida)

### 7.1 `_Storage<CameraAI::Director*,2>::_Storage(const&)` — 1.156 B, `UTLVector.h`

26 filas, dos causas separadas:

* **Un intercambio `r24`↔`r25` que recorre la función entera** (índices 8, 11-13,
  17, 107, 158, 180, 206, 271, 274) más un `r0`↔`r9` local en los índices 14-19.
* **Una carga izada**: en el índice 70 emitimos un `lwz r4,0x8(r31)` que el
  objetivo no tiene, y en el 129 nos falta su `mr r4,r9`. El objetivo lee
  `mSize` una vez y luego la rederiva con un `mr`; nosotros la releemos. Saldo
  cero en bytes, 60 filas de desalineamiento.

Sale de `UTL::Collections::Vector::assign()` en
`src/Speed/Indep/Libs/Support/Utility/UTLVector.h` (líneas 117-149), que ya
lleva un `asm("" : "+r"(srcEnd))` de otra ronda y que comparten zSim, zPhysics,
zMain y zFe. **No la toco.** La instrucción concreta a perseguir es el
`lwz r4,0x8(r31)` del índice 70.

### 7.2 `__static_initialization_and_destruction_0` — los 16 B siguen, y hay dos datos nuevos

Sigue exactamente como la describió la r36d: cuatro `addi r27,r30,0x30/0x48/0x60/0x78`
de más en `ICE::ReplayCategoryTable[2..5]`, 40 de las 54 filas, umbral medido en
4 inicializadores dinámicos por delante. Su propuesta (orden de la SourceList)
sigue en pie y no la toco.

**Dato nuevo 1: la veda «un inicializador de ámbito de namespace no admite
barrera» está ROTA.** La r36d la cerró diciendo que un `asm` ahí sería un `asm`
de nivel superior. No hace falta: el constructor `ReplayCategory::ReplayCategory`
es un cuerpo de función normal y **admite `asm` perfectamente**, y ese cuerpo se
expande dentro del inicializador. La palanca existe, sólo que aquí no paga:

| # | qué (en `ICEReplay.hpp`) | tamaño | filas |
|---|---|---|---|
| base | — | 3.620 | 54 |
| s1 | `asm("" : "+m"(*this))` al final del cuerpo | 3.632 | — |
| s3 | `this` en local + `asm("" : "+r"(self_))` y stores vía `self_` | 3.648 | — |
| s4 | lista de init reordenada a `pScore, pMirror, pName, pReplayName` | 3.620 | 56 |
| s5 | los SEIS miembros en la lista, en el orden de stores del objetivo | **3.616** | 170 |

s4 vale de contraprueba: el objetivo almacena por elemento en el orden
`pScore(+16), pMirror(+20), pName(+0), pReplayName(+8), nReplayHash(+12),
nNameHash(+4)`, y **reproducir ese orden con la lista de inicialización no mata
los cuatro `addi`** (sigue en 3.620). O sea que el `addi` no lo decide el orden
de los stores dentro del elemento; la r36d ya había visto lo contrario con el
mismo orden escrito como CUERPO (ensayo 11: mata los `addi` pero 118 filas), así
que lo que lo decide es lista-contra-cuerpo, no el orden.

**Dato nuevo 2**: el `.o` de zCamera nos sale **corto de datos**
—`.rodata` 8.488 contra 9.040 (−552 B) y `.data` 2.884 contra 3.136 (−252 B)—.
O sea que faltan objetos con inicialización estática en los `.cpp` de zCamera, y
eso es exactamente lo que cambiaría el estado de `cse` en ese punto. El «frente
de fondo» de la unidad (19 de sus 28 `.cpp` casi vacíos) y los 16 B del
`__static_init` **son el mismo frente**, y el de datos se puede atacar sin
tocar la SourceList.

### 7.3 `bMath.hpp:197`, el `// TODO is this order correct?`

Resuelto por medida en §3: indecidible por esta vía. Sugiero cambiar el comentario
en vez de dejar el TODO abierto (es cabecera compartida: no lo hago yo).

### 7.4 11.004 B de `.text` que el original no tiene (587 funciones contra 453)

No cuenta en `matched_code`, pero es el frente «el 100 % que no compara» y bloquea
`linked`. Reparto:

| qué | símbolos | B |
|---|---|---|
| varios (82 símbolos sueltos: `__14ITrafficCenter`, `clear__...`, `_M_erase__...`, `_._Q33UTL3COM8IUnknown`...) | 82 | 3.056 |
| `_STL::find<T**, T*>` (12 tipos) | 12 | 2.112 |
| `Attrib::TAttrib<T>::Get` | 25 | 2.000 |
| `_STL::vector<...>::reserve` (5 tipos) | 5 | 1.748 |
| `_Storage<IDebugWatchCar*,2>` ctor + dtor + `_.` | 3 | 1.500 |
| `ICEMover::GetXxx(float)` (7 accesores) | 7 | 588 |
| **TOTAL** | **134** | **11.004** |

Nótese la simetría: el objetivo instancia `_Storage<CameraAI::Director*,2>`
(nuestro near-miss de 1.156 B) **pero no** `_Storage<IDebugWatchCar*,2>`, que
nosotros emitimos entero (1.500 B con su destructor). Quien mire el frente de
plantillas tiene ahí una pista de qué `#include` sobra en `Camera/**`.

---

## 8. Vedas nuevas de esta ronda (todas medidas)

| # | función | cambio | resultado |
|---|---|---|---|
| 1 | `TrackCop` | barrera de argumento **sin** local (`asm("" : : "r"(&look_offset))`) | 25 filas contra 2 |
| 2 | `TrackCop` | barrera sobre los DOS argumentos (w3) | 2 filas; una de más estorba |
| 3 | `TrackCop` | w2 sin la barrera de ranura de `hcomp` | 4 filas |
| 4 | `TVN` | `asm("+f")` sobre `dotv` / `amax` / los dos / `accel` / `f_speed_magnitude` | 23 / 33 / 22 / 16 / 20 contra 13 |
| 5 | `TVN` | orden de parámetros de `bClamp` (dos permutaciones vía ayudante local) | **objeto idéntico**: no observable |
| 6 | `ICEMover` | eje «a quién» en las tres direcciones | 23 filas contra 15 |
| 7 | `ICEMover` | cantidad fantasma (r7 y r12, tres colocaciones) | 23 / 32 / 32 |
| 8 | `ICEMover` | cuatro formas de `GetKey` | objeto idéntico las cuatro |
| 9 | `ICEMover` | argumentos de `SignedMod` en locales, con y sin barrera | 16 (=base) / 26 |
| 10 | `TrackCar` | `asm("+m")` sobre `Look` / `displacement` (4 colocaciones) | 23-25 contra 21 |
| 11 | `TrackCar` | la receta que cierra `TrackCop` | 24 |
| 12 | `LoadCameraShakes` | 5 formas de `warned_overflow = false` | objeto idéntico las cinco |
| 13 | `LoadCameraShakes` | `asm("" : : "r"(warned_overflow))` pegado a la asignación | 168 B pero 9 filas |
| 14 | `LoadCameraShakes` | `num_tracks > i` en la condición del `for` | 18 filas (pliega `i` a `cmpwi`) |
| 15 | `ReplayCategory` | `asm("+m"(*this))` / `this` en local + `asm` | 3.632 / 3.648 B contra 3.620 |
| 16 | `ReplayCategory` | lista de init en el orden de stores del objetivo | 3.620 B, 56 filas (no mata los `addi`) |
| 17 | permutador ciego | 4 funciones, 1.450 variantes | cero mejoras; **no toca el eje de las barreras** |

---

## 9. Lo que dejo abierto, por orden de premio

1. **`__static_init` (3.604 B)** — junto con los 552 B de `.rodata` y 252 B de
   `.data` que faltan. Es el mismo frente y el de datos no necesita tocar la
   SourceList (§7.2).
2. **`ICEMover::Update` (3.868 B)** — 15 filas, una permutación de 4 registros
   en 30 instrucciones. Causa nombrada (§4: tres pseudos contra dos en
   `SignedMod`); agotadas las cinco palancas del brief y el permutador ciego.
   Lo que queda sin probar es un `--depth 2 --anneal` largo sobre el cuerpo
   entero, o atacar la elección de representante de `cse` en el primer test del
   `while`.
3. **`TerrainVelocityNoise` (1.192 B)** — un registro de más en el bloque
   (§3). Lo que hay que conseguir es que dos direcciones de literal compartan
   registro; ninguna palanca conocida hace eso.
4. **`_Storage` (1.156 B)** — `UTLVector.h`, fuera de territorio (§7.1).
5. **`TrackCar::Update` (992 B)** — racimo A primero (§5); descartado que sea
   `bFill`.
6. **`LoadCameraShakes` (168 B)** — duplicación de la variable de inducción,
   no un store muerto (§6).
