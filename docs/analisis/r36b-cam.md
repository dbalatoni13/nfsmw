# r36b — zCamera: las 7 funciones (11.928 B)

**Ficheros tocados en el arbol: NINGUNO.** Todos los ensayos se hicieron con una
copia de la fuente en el scratchpad y un `-I` que la antepone (ver seccion 7), asi
que `src/Speed/Indep/Src/Camera/**` queda exactamente como estaba. Medido al
final: `pctsnap --cmp` sobre zCamera dice **EMPEORAN: ninguna / MEJORAN: 0**.

## 0. Verificacion del encargo

`triaje.py zCamera` y `regmap.py zCamera --scan` **reproducen** el encargo fila
por fila. La unidad tiene 7 pendientes, no 6: falta en la lista del brief
`LoadCameraShakes__10ICEManagerP6bChunk` (168 B, 96,667 %).

```
   3868 Update__8ICEMoverf                          99.907  15  PERMUTADOR
   3604 __static_initialization_and_destruction_0   97.112  54  ESTRUCTURA
   1192 TerrainVelocityNoise__11CameraMover...      99.077  14  falta codigo
   1156 __Q33UTL11Collectionst8_Storage2Z...        97.834  16  ESTRUCTURA
    992 Update__19TrackCarCameraMoverf              99.629  15  PERMUTADOR
    948 Update__19TrackCopCameraMoverf              99.156   2  falta codigo
    168 LoadCameraShakes__10ICEManagerP6bChunk      96.667   6  falta codigo
```

---

## 1. `__static_initialization_and_destruction_0` (3.604 B) — RESUELTO EL DIAGNOSTICO

Es el segundo mayor del proyecto y **nadie lo habia mirado**. Las 54 diferencias
son **dos racimos y nada mas**; todo lo demas casa instruccion a instruccion.

### Racimo A (indice 549-566): 6 diferencias, permutacion pura

`r9`/`r10`/`r11` rotados entre tres usos (`HydraulicsLookAngle`, `NOSFovWidening`,
`$LC1125`). Mismo numero de instrucciones. Es desempate del asignador.

### Racimo B (indice 796-899): las 4 instrucciones de mas, y su causa exacta

Los 16 B que sobran (3.620 contra 3.604) son **cuatro `addi r27, r30, N`**
(N = 0x30, 0x48, 0x60, 0x78) en la construccion de `ICE::ReplayCategoryTable[6]`.

    objetivo:  stw r9, 0x40(r30)   stw r11, 0x44(r30)   stw r28, 0x38(r30) ...
    nuestro:   addi r27, r30, 0x30
               stw r9, 0x10(r27)   stw r11, 0x14(r27)   stw r28, 0x8(r27)  ...

Las entradas 0 y 1 (offsets 0x00 y 0x18) **casan**; las entradas 2..5 son las que
se apoyan en `r27`.

**La fuente NO tiene la culpa, y esta medido.** Escribi un repro minimo
(`struct ReplayCategory` + el array de 6, con los cflags exactos de zCamera) y
sale **identico al objetivo, fila por fila**, incluido el orden
`stw score / stw mirror / stw pName / stw pReplayName / bl bStringHash`. O sea que
la forma actual de `ICEReplay.hpp` (lista de inicializacion + dos `bStringHash` en
el cuerpo) es la correcta.

**Lo que decide es el CONTEXTO: cuanto codigo de inicializacion estatica hay
delante en el mismo bloque basico.** Tres medidas:

| unidad compilada | `addi rN,r30,X` |
|---|---|
| zCamera completa | **4** |
| zCamera menos `Movers/Still.cpp` | **2** |
| zCamera menos 7 ficheros de `Movers/` | **0** (forma del objetivo) |

Con menos estaticos por delante, la tabla se compila **exactamente como el
objetivo**. Es un efecto graduado y sensible: quitar UN fichero pequeno ya baja
de 4 a 2.

**El pase responsable, acotado con dos banderas** (solo diagnostico, no propongo
tocarlas):

| bandera | `addi` |
|---|---|
| `-fno-gcse` | 4 |
| `-fno-move-all-movables` | 4 |
| **`-fno-force-addr`** | **0** |
| **`-fno-rerun-cse-after-loop`** | **0** |

O sea: `-fforce-addr` mete cada direccion constante en un pseudo, y **`cse2`** lo
convierte en `(plus r27 8)` en vez de `(plus r30 56)`. El mecanismo esta en
`orig/prodg/NGC_GNU_SRC/NGC/gcc/cse.c:2036`, `use_related_value()`: recorre la
lista circular `related_value` del simbolo y **se queda con la PRIMERA clase que
tenga un REG**. Si la constante `table+56` ya esta en la tabla hash, `relt` es su
propia clase y el recorrido cae en `table+48` (que lleva `r27`) antes que en
`table` (que lleva `r30`). Con menos codigo delante la tabla hash llega mas limpia
y gana `r30`.

**Consecuencia practica para quien lo retome:** no hay nada que arreglar en
`ICEReplay.hpp` ni en `ICEReplay.cpp`. Lo que falta esta **aguas arriba**, y el
unico sintoma visible aguas arriba es el racimo A. Cerrar el racimo A muy
probablemente cierre tambien el B (y al reves: cualquier cambio que mueva unas
pocas instrucciones antes del indice 780 puede volcarlo).

### Ensayo 2 (NEGATIVO/NEUTRO, medido)

Cambiar el constructor de `ReplayCategory` de lista de inicializacion a **cuerpo
de asignaciones** (que es lo que pedia el comentario de `ICEReplay.hpp:38-41`):
**97,1121 % antes y despues, 54 diferencias, mismo tamano**. Binario identico. El
comentario de la cabecera esta obsoleto: las dos formas dan lo mismo.

---

## 2. `_Storage<CameraAI::Director*,2>::_Storage(const&)` (1.156 B) — FUERA DE MI TERRITORIO

Nadie lo habia mirado y **el diagnostico es de una linea**. Sobra 1 instruccion
(1.160 contra 1.156):

    objetivo:  lwz r9,0x8(r4) / lwz r0,0x0(r4) / slwi r9 / add r24,r0,r9
               mr. r27, r0            <- copia Y compara en UNA instruccion
               subf r9, r0, r24       <- resta desde r0 (srcBeg)
    nuestro:   ... mr r27, r9 / add r24,r9,r0
               subf r0, r27, r24      <- resta desde r27 (srcIt, la COPIA)
               cmpwi r27, 0x0         <- compare aparte

`combine` funde `mr` + `cmpwi` en `mr.` solo si el **primer uso** de `r27` es la
comparacion. En el nuestro el primer uso es el `subf`, porque la resta se hace
desde la copia y no desde el original.

El codigo esta en `src/Speed/Indep/Libs/Support/Utility/UTLVector.h`,
`Vector::assign(const_iterator, const_iterator)`:

```cpp
const_iterator srcIt = srcBeg;
size_type minSize = srcEnd - srcBeg;
if (srcIt == 0) {
```

**Ensayo 1 (NEGATIVO, medido).** Intercambiar las dos declaraciones (`minSize`
antes que `srcIt`): **97,834 % -> 96,606 %**, y el tamano baja a 1.152 (4 B de
MENOS). Consigue el `mr. r27, r9` que queria, pero de propina GCC **pliega**
`(begin + size*4) - begin` a `size*4` y se lleva por delante el `add`+`subf` que
el objetivo si tiene. Revertido.

**No lo toco**: `UTLVector.h` es cabecera compartida y esta fuera de mi lista de
ficheros. Lo dejo aqui documentado para quien tenga ese territorio: hace falta una
forma que ponga la comparacion como primer uso de `srcIt` **sin** dejar que GCC
pliegue la resta.

---

## 3. `LoadCameraShakes` (168 B) — diagnostico cerrado, cuatro negativos

Falta **una** instruccion. El objetivo tiene **dos** registros a cero, nosotros
uno:

    objetivo: li r0,0 / li r27,0 / cmpw r0,r28  ... addi r27,r27,1
    nuestro:  li r28,0           / cmpw r28,r27 ... addi r28,r28,1

`regmap` lo confirma contra el DWARF:

```
  fn        warned_overflow   original r0    nuestro -    <-- DISTINTO
  b0/b0     num_tracks        original r28   nuestro r27
  b0/b0/b0  i                 original r27   nuestro r28
```

Y `debug_lines.txt` dice que **las DOS `li 0` estan en la misma linea** (6816, la
del `for`) y que la asignacion `warned_overflow = false` (6813) **no genera
instruccion propia** (colapsa con 6814). O sea: el `r0` del DWARF es el registro
que el asignador le dio al pseudo muerto, y las dos constantes cero salen las dos
del `for`: el objetivo tiene **dos pseudos** distintos con valor 0 (el de la
guarda y el del contador) y nosotros los tenemos fundidos en uno.

Cuatro ensayos, **los cuatro negativos**, medidos:

| # | cambio | resultado |
|---|---|---|
| 3 | sacar `int i = 0;` fuera del `for` | **identico** (96,667 %, 164 B) |
| 4 | reproducir el mapa de lineas del original insertando 7 lineas en blanco (+3 antes de `group=`, +1 dentro del `for`, +2 tras el cierre) | **identico** |
| 5 | mover `warned_overflow = false;` a justo antes del `for` | **identico** |
| 6 | `int i = 0; if (0 < num_tracks) { do {...} while (i < num_tracks); }` | **96,667 % -> 91,667 %**, revertido |

El 6 ademas mata una hipotesis: con el literal en la guarda GCC emite `cmpwi r28,0`
(inmediato), no `cmpw rX,r28`, asi que **el cero del objetivo no viene de un
literal en la fuente**: viene de un pseudo duplicado (`duplicate_loop_exit_test`,
`jump.c:2577`).

**El 4 vale para todo el proyecto**: el mapa de lineas del original de esta
funcion tiene 7 lineas mas que el nuestro, y **reproducirlas no cambia ni un
byte**. Las NOTE de linea no mueven el codegen aqui; `debug_lines` sirve para
localizar sentencias que faltan, no para calcar el codigo.

---

## 4. Las cuatro de «reparto»: `ICEMover`, `TrackCar`, `TrackCop`, `TerrainVelocityNoise`

Las cuatro tienen la **misma firma**, y es la que ya esta documentada en
`nfsmw-rank-for-schedule`: el objetivo usa el registro **original** donde nosotros
usamos la **copia**, y quien lo hace es `regmove`/`optimize_reg_copy_*`
(`regmove.c`, solo activos con `-fexpensive-optimizations`, que es obligatoria).

- **`ICEMover::Update`** (3.868 B, 99,907 %): las 15 diferencias estan **todas** en
  el indice 589-617, un solo racimo. El patron exacto:
  `lwz r8` + `mr r11,r8` + **`cmpwi r8,0`** (objetivo) contra
  `lwz r9` + `mr r11,r9` + **`cmpwi r11,0`** (nuestro). El resto del racimo se
  recoloca solo detras de eso. **No es una local que falte** (regmap: 4 registros
  movidos, sin ciclo). El permutador guiado de un cambio ya se barrio en la r36.
- **`TrackCar::Update`** (992 B, 99,629 %): tres subracimos. Los indices 107-111
  (`Look = *GetGeometryPosition()`) y 113-124 (`displacement /= distance`) son
  **orden de los tres `stfs`**: el objetivo guarda z, x, y y nosotros x, y, z. El
  tercer subracimo (134-143) tiene el **mismo orden** y solo cambia el nombre del
  registro flotante, o sea que es consecuencia de los dos primeros.
  El mapa de lineas dice que los tres `stfs` salen de **`bMath.hpp:905`** y las
  tres cargas de **`bMath.hpp:906`** — es decir, en el original **los tres stores
  estan en UNA sola linea y las tres cargas en OTRA**. Nuestro `bCopy`+`bFill`
  reparte cada componente en su propia linea. Eso apunta a que la forma de
  `bCopy`/`bFill` del original no es la nuestra, pero **`bMath.hpp` esta fuera de
  mi territorio** y tocarlo mueve toda la decompilacion: lo dejo senalado.
- **`TrackCop::Update`** (948 B): ya barrido (permutador de 1 cambio, profundidad 2
  con recocido, y el constructor `bVector3 look_offset(0,0,0)`). Confirmo que sigue
  a 2 diferencias, un `stfs f30, 0x58(r1)` dos ranuras adelantado.
- **`TerrainVelocityNoise`** (1.192 B, 99,077 %): las 14 diferencias son un solo
  racimo (115-136) de cargas del pool literal. En 121/125 el objetivo carga
  `0x3c(r30)` en f12 y `0x38(r30)` en f0, y nosotros al reves; el `fmadds` de 125
  hereda el cambio. Es nombre de registro, no estructura.

---

## 5. Vedas nuevas de esta ronda (para no repetirlas)

1. `ReplayCategory`: cuerpo de asignaciones en vez de lista de inicializacion —
   **binario identico**. El comentario de `ICEReplay.hpp:38-41` esta obsoleto.
2. `LoadCameraShakes`: sacar `int i` del `for` — **identico**.
3. `LoadCameraShakes`: calcar el mapa de lineas del original con lineas en blanco —
   **identico**. (Generalizable: las NOTE de linea no mueven el codegen.)
4. `LoadCameraShakes`: mover `warned_overflow = false;` justo antes del bucle —
   **identico**.
5. `LoadCameraShakes`: `do/while` con guarda `0 < num_tracks` — **−5 puntos**.
   Con un literal GCC emite `cmpwi`, no `cmpw` con registro.
6. `Vector::assign`: declarar `minSize` antes que `srcIt` — **−1,2 puntos** y −4 B
   (GCC pliega la resta). Fuera de territorio, pero medido.

Y una veda que **NO** hace falta probar en zCamera: la **barrera selectiva**.
Ninguna de las siete cumple la firma (`regmap` no ve ningun valor en registro
preservado donde el objetivo use uno volatil por cruzar una llamada); las cuatro
de reparto son empates del asignador/`regmove`, que es justo donde la seccion 2
del brief dice que la barrera solo estorba.

---

## 6. Lo que dejo abierto, por orden de premio

1. **Racimo A del `__static_initialization_and_destruction_0`** (6 diferencias de
   registro alrededor del indice 549). Es pequeno, es permutacion pura, y si cae
   arrastra probablemente los 16 B del racimo B: **3.604 B por 6 diferencias**.
   Es el mejor sitio del proyecto donde meter permutador ciego ahora mismo.
2. **`_Storage`** (1.156 B): una instruccion, diagnostico cerrado, la fuente esta
   en `UTLVector.h`. Que lo coja quien tenga ese territorio.
3. **`bMath.hpp:905/906`**: el mapa de lineas dice que el `bCopy` del original
   tiene los tres stores en UNA linea y las tres cargas en OTRA. Si eso se
   confirma, no es solo `TrackCar`: toca a media decompilacion.

---

## 7. Herramienta: probar cabeceras compartidas sin tocar el arbol

Con seis agentes en paralelo, editar una cabecera compartida para medir envenena
las medidas de los demas. Lo resolvi asi (esta en el scratchpad, no en el repo):

    copia el fichero a  $SCR/inc/<misma ruta relativa>
    compila con         -I $SCR/inc  ANTEPUESTO al resto de -I
                        (y -I <dir del original> al final, para los #include "" locales)
    diffea el .o de scratch contra build/GOWE69/obj/.../zCamera.o

Compila la unidad entera en ~35 s, no toca ni un fichero del arbol, y el `.o` sale
byte a byte igual al del build normal (verificado contra la base). Si alguien lo
quiere permanente, son 30 lineas encima de `build_direct.parse_units()`.
