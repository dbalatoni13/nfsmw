# r51 — zCamera: el bucle que el `-O1` no rota, y los fantasmas que cierran el racimo C de `TrackCar`

**Delta retenido: 0 B de codigo, y `src/` sin tocar** (`git status --short
src/Speed/Indep/Src/Camera src/Speed/Indep/Libs/Support/Utility
src/Speed/Indep/bWare` sale vacio; `fncmp` da el mismo listado exacto del
principio: 5 funciones / 10.812 B). Todo se midio sobre sombras de cabeceras en
el scratchpad.

Dos avances reales, los dos medidos al insn:

1. **`_Storage` (1.156 B): 98,6990 % -> 99,6713 %, de 22 filas a 15**, y la
   causa esta identificada en el COMPILADOR, no adivinada: el bucle de
   crecimiento de `UTL::Vector::assign` lee `mSize` **directamente**, no
   `size()`. Es una palanca general del proyecto (§1.3). La cabecera es
   compartida, asi que va como **propuesta con medida** (§1.5), incluido el
   radio de accion: **11 SourceLists compiladas con y sin el cambio, +0 B en
   todas**.
2. **`TrackCar` (992 B): de 15 filas a 6**, con las **dos** cerraduras que r47,
   r48 y r49 no pudieron abrir (racimos B y C) **cerradas del todo** (§2). La
   receta es la **cantidad fantasma** + extension de rangos de vida, cero bytes
   emitidos. **No se aplica**: 6 filas siguen siendo 0 B y andamio que no paga
   es deuda (regla 7).

Y un frente que se cierra con cifra: **`ICEMover` no se mueve ni por la fuente de
`SignedMod` (8 formas) ni por pines en su llamante (13 colocaciones)** (§3).

---

## 0. Estado, antes y despues

```
       B  funcion                                        diferencia
    3868  Update__8ICEMoverf                             15 insn, 94 reub
    3604  __static_initialization_and_destruction_0      tamano (3620/3604)
    1192  TerrainVelocityNoise__11CameraMoverP8bMatrix4P 13 insn, 41 reub
    1156  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8 80 insn
     992  Update__19TrackCarCameraMoverf                 15 insn, 22 reub
5 de 453 funciones con el CODIGO distinto -- 10812 B que no cuentan
```

Identico antes y despues. Lo que cambia es lo que se sabe.

---

## 1. `_Storage<CameraAI::Director*,2>::_Storage(const&)`: 98,70 % -> 99,67 %

### 1.1 Que es la funcion

Es el **constructor de copia IMPLICITO** de
`UTL::Collections::_Storage<CameraAI::Director*, 2>`, que arrastra inline
`FixedVector(const FixedVector&)` -> `operator=` -> `Vector::assign(const
Vector&)` -> `Vector::assign(begin, end)` -> `reserve` + `push_back`. 1.156 B de
plantilla instanciada, ni una linea de `Camera/`.

### 1.2 Las 22 filas son DOS racimos, no uno

`fndiff` con el objeto actual:

| filas | que pasa |
|---|---|
| 8, 11, 12, 13, 159, 181, 207, 272, 275 | permutacion r24 <-> r25 (reparto) |
| 14-19 | permutacion r0 <-> r9 (reparto) |
| **66-72** | **el objetivo guarda `size()` en r4 desde el guardian; nosotros lo RECARGAMOS** |
| **130-132** | **el objetivo tiene un `mr r4, r9` que nosotros no** |

Los dos ultimos son el mismo racimo visto por sus dos puntas, y son estructura,
no reparto.

### 1.3 La causa, leida del mapa de lineas del ORIGINAL

`lmap.py` sobre el objetivo (`out/lmap_storage.txt`) da la prueba:

```
80081938  lwz r4, 0x8(r31)   UTLVector.h:249      <- test de entrada
8008193C  cmplw r26, r4
80081940  ble  .L_80081C88
   ...cuerpo del bucle (push_back, UTLVector.h:172-178)...
80081A2C  lwz r9, 0x8(r31)   UTLVector.h:175 118
80081A30  addi r9, r9, 0x1   UTLVector.h:178
80081A34  mr r4, r9          UTLVector.h:249      <- MISMA LINEA que el de entrada
80081A38  stw r9, 0x8(r31)   UTLVector.h:178
80081A3C  cmplw r26, r4      UTLVector.h:249
80081A40  bgt  .L_80081948
```

**La misma linea 249 aparece DOS veces, antes del bucle y al fondo.** Eso no lo
escribe nadie: es `jump.c::duplicate_loop_exit_test` copiando el test de salida
delante del bucle. Y el `mr r4, r9` existe porque las dos copias comparten
pseudo: por eso `size()` sobrevive en registro dentro del cuerpo (`cmplw r4, r0`
de `push_back` no vuelve a leer memoria) y por eso la cabeza del bucle del
objetivo es la carga de `capacity`, no la de `size`.

Y la condicion para que eso ocurra esta en el fuente de GCC que hay en el arbol,
`orig/prodg/NGC_GNU_SRC/NGC/gcc/stmt.c::expand_end_loop`:

```c
	  if (GET_CODE (insn) == NOTE)
	    {
	      if (optimize < 2
		  && (NOTE_LINE_NUMBER (insn) == NOTE_INSN_BLOCK_BEG
		      || NOTE_LINE_NUMBER (insn) == NOTE_INSN_BLOCK_END))
		/* ... we refuse to move code containing BLOCK notes at low
		   optimization levels.  */
		break;
```

`expand_end_loop` **rota** el bucle (`start: if(test) goto end; body; goto start`
-> `goto start; newstart: body; start: if(test) goto end; goto newstart`) y esa
rotacion es la que deja un salto incondicional detras del `NOTE_INSN_LOOP_BEG`,
que es **la precondicion exacta** de `duplicate_loop_exit_test`
(`jump.c:2577`). Pero el barrido para el test **se corta en la primera nota de
bloque cuando `optimize < 2`**, y **compilamos con `-O1`**.

**Traduccion, y esto vale para todo el proyecto**: a `-O1`, si la condicion de un
`while`/`for` contiene una **llamada inline** (que emite `NOTE_INSN_BLOCK_BEG`),
el bucle NO se rota y el test NO se duplica. Si la condicion lee un **miembro o
una variable directamente**, si.

### 1.4 La medida

Todas sobre sombra de `UTLVector.h` (control duro hecho: cambiar
`GetMaxCapacity` de `0x7FFFFFFF` a `0x7FFFFFFE` pasa la unidad de 9 a 11
funciones != 100 %, o sea la sombra esta viva).

| ensayo sobre la rama `srcIt == 0` de `assign` | `_Storage` |
|---|---|
| base (`reserve` + `if` + `do/while` con `size()`) | 98,6990 %, 1156 B, 22 filas |
| `if (n > size()) { reserve; while (n > size()) push_back; }` | 95,9343 %, 1156 B |
| `while (size() < n)` | 95,8824 % |
| `if` + `do/while` con guardian delante | 97,1799 %, **1144 B** |
| `for(;;) { push_back(); if (n <= size()) break; }` | 97,1799 %, 1144 B |
| `if (size() < n)` + `do/while (size() < n)` | 96,9516 %, 1152 B |
| **`if (n > size()) { reserve(n); while (n > mSize) push_back(); }`** | **99,6713 %, 1156 B, 15 filas** |
| lo mismo con `if (n > mSize)` en el guardian | 99,6713 % (identico) |

Con `mSize` en la condicion, **los dos racimos estructurales desaparecen**: las
15 filas que quedan son solo r24<->r25 y r0<->r9, reparto puro.

**Refutado de paso** (era la hipotesis natural leyendo las lineas 241/242/246/
247/249 del objetivo): `assign` **no llama a `resize()`**. Sustituir la rama por
`resize(minSize)` con `resize` ya arreglado da **1.188 B / 96,8858 %**: los 32 B
de mas son la rama `else { while (mSize > num) pop_back(); }`, que el objetivo no
tiene ahi. `assign` lleva su propio bucle de crecimiento en linea.

**Andamio re-medido (regla 10).** El `asm("" : "+r"(srcEnd));` que ya estaba en
`assign` **sigue pagando** con el arbol de hoy:

| andamio | `_Storage` |
|---|---|
| quitarlo | 98,0796 %, 1152 B |
| sobre `srcBeg` en vez de `srcEnd` | 99,4568 %, 1160 B |
| sobre los dos | 99,1176 %, 1156 B |
| **como esta (`srcEnd`)** | **99,6713 %, 1156 B** |

Y cuatro perturbaciones mas, todas neutras o peores: mover `const_iterator srcIt
= srcBeg` delante de `minSize` (99,2734 %, 1160 B), poner el `asm` detras de
`minSize` (98,8408 %, 1152 B), `if (srcBeg == 0)` en vez de `if (srcIt == 0)`
(identico), `end()` escrito `begin() + size()` (identico).

**Aviso de metrica**: `assign(src.begin(), src.begin() + src.size())` sube el
fuzzy a **99,7059 %** y **son las mismas 15 filas**. Lo unico que cambia es que
el `add` casa por TEXTO con los operandos intercambiados de sitio (nuestro r0
lleva `size*4` donde el objetivo lleva `mBegin`). Es el modo de engano de
[[nfsmw-medidas-que-enganan]]: **contar filas, no mirar el fuzzy**.

### 1.5 Propuesta (cabecera compartida: NO aplicada)

`src/Speed/Indep/Libs/Support/Utility/UTLVector.h`, dentro de
`Vector::assign(const_iterator, const_iterator)`, rama `if (srcIt == 0)`:

```c
-            reserve(minSize);
-            if (minSize > size()) {
-                do {
-                    push_back();
-                } while (minSize > size());
-            }
+            if (minSize > size()) {
+                reserve(minSize);
+                while (minSize > mSize) {
+                    push_back();
+                }
+            }
             return;
```

**Radio de accion medido** (`radio.py`: cada unidad compilada dos veces, con y
sin la sombra, y comparadas funciones != 100 % y bytes):

| unidad | base | con el cambio | delta |
|---|---|---|---|
| zAI, zFe, zFe2, zWorld, zWorld2, zEcstasy, zPhysics, zSim, zGameplay, zLua, zMain | 10/10/28/9/5/10/5/3/6/6/5 fn | las mismas | **+0 B en las once** |

Control de la regla 12: los **once objetos salen DISTINTOS** entre las dos
compilaciones (md5), o sea el cambio llega a todas y aun asi no mueve la metrica.
(`zTrack`, `zSpeech` y `zEAXSound` no compilaban en el arbol en ese momento, por
otro agente, ni con el cambio ni sin el.)

Por coherencia habria que cambiar igual los dos bucles de `resize()`
(`while (num > mSize)` / `while (mSize > num)`); en zCamera es **neutro**
(99,6713 % con y sin), pero es la misma sentencia y el mismo motivo.

### 1.6 Regalo: el mapa de la `UTLVector.h` del original

Del `lmap` del objetivo, por si alguien reconstruye la cabecera:

| linea del original | que hay |
|---:|---|
| 118 | `size()` |
| 124 | `capacity()` |
| 148 | `begin()` |
| 160 | `end()` |
| 172-178 | `push_back()` (173 el `if (size() >= capacity())`, 178 el `mSize++`) |
| 203-231 | `reserve()` |
| 241-251 | el helper que `assign` llama en la rama nula (242 el guardian, 247 el `reserve`, **249 el `while`**) |
| 270-274, 296-330 | `insert_sequence` / `assign` (301 `srcEnd - srcBeg`, 306 el `if`, 307 la llamada, 310 el `return`) |
| 578-583 | `Init()` — o sea **al final de la clase**, no al principio como en la nuestra |
| ~721 | la vtable de `FixedVector` |

---

## 2. `Update__19TrackCarCameraMoverf` (992 B): de 15 filas a 6

### 2.1 La veda de la r48/r49 esta CADUCADA

La r48 dejo escrito que el DWARF cierra la via de los pines porque «el original
NO tiene locales ahi», y la r49 que la funcion esta agotada desde la fuente. Lo
primero es cierto **para los valores del racimo C**, pero no cubre la palanca que
faltaba: **la cantidad fantasma** ([[nfsmw-cantidad-fantasma]]), que no declara
ninguna local del calculo, sino un registro ocupado.

La r48 lo habia predicho con numeros y ahora esta comprobado: *«forzando el
racimo C a f11/f12/f10 el racimo B cierra entero»*.

### 2.2 La receta, y lo que vale cada pieza

Todo en `Movers/TrackCar.cpp` (sombra), **cero bytes emitidos**: 992/992 en
todos los ensayos.

```c
    bVector3 look_offset;
    register float _p0 asm("fr0");
    register float _p13 asm("fr13");
    asm("" : "=f"(_p0));                 // abre el fantasma de fr0
    asm("" : "=f"(_p13));                // abre el de fr13
    float _lx = TrackCarLookOffsetX[CameraType];
    float _ly = TrackCarLookOffsetY[CameraType];
    float _lz = TrackCarLookOffsetZ[CameraType];
    look_offset.x = _lx;
    look_offset.y = _ly;
    look_offset.z = _lz;
    asm("" : "+f"(dT) : "f"(_p0), "f"(_p13), "f"(_lx), "f"(_ly), "f"(_lz));
    eMulVector(&look_offset, CarToFollow->GetGeometryOrientation(), &look_offset);
```

| ensayo | filas |
|---|---:|
| base | **15** |
| barrera dura `asm("" : : : "fr0","fr13")` (control: TIENE que cambiar) | 15, fuzzy 93,43 % |
| solo los dos fantasmas (cierre sobre `dT`) | 13 |
| solo la extension de rangos, sin fantasmas | **15 (= base exacto)** |
| **fantasmas + extension de `_lx`,`_ly`,`_lz`** | **9** |
| lo mismo con UN solo `asm` de apertura de dos salidas | 15 |
| un fantasma solo (fr0, o fr13) | 13 |
| tres fantasmas (fr0, fr13, fr12) | 15 |
| fantasmas abiertos ANTES de `bCross` (mueren en la llamada) | 12 |

Y el efecto en el reparto del racimo C (`lfsx` de `TrackCarLookOffset*`):

| | z | x | y |
|---|---|---|---|
| base | f12 | f0 | f13 |
| **fr0+fr13 ocupados** | **f10** | f12 | f11 |
| **objetivo** | **f10** | **f11** | **f12** |

Con la extension de los tres rangos de vida el orden de prioridad de
`local_alloc` pasa a ser el del objetivo y **el racimo C casa entero** (filas
134-145 identicas), y con el **el racimo B tambien** (las 4 filas de `fmuls`/
`stfs` de `displacement /= distance` desaparecen).

### 2.3 El racimo A: barrido completo de 36 ordenes

Con el racimo C ya cerrado, `Look = *CarToFollow->GetGeometryPosition()` escrito
a mano (`CamCopy`: tres locales `_cx/_cy/_cz` y tres asignaciones) y barridos los
**6 ordenes de declaracion x 6 ordenes de almacenamiento**:

| decl / store | filas |
|---|---:|
| **xyz / xzy** | **6** |
| zyx / zxy | 8 |
| xyz / xyz (= la forma actual) | 9 |
| xzy / xyz, yxz / yzx, xyz / yxz | 9 |
| yzx / yxz, zxy / zyx | 10 |
| los 28 restantes | 11 a 12 |

El `CamCopy` **solo** (sin fantasmas) da 12 filas y fuzzy 99,7016 %; o sea la
combinacion no es aditiva y hay que medir el par, no las piezas.

### 2.4 Lo que queda: 6 filas, y por que no se aplica

```
>>>  107 stfs f12, 0x98(r31)   | stfs f0, 0x90(r31)     <- las dos primeras de Look, cambiadas
>>>  109 stfs f0, 0x90(r31)    | stfs f12, 0x98(r31)
>>>  135 addi r3, r1, 0x38     |                        <- &look_offset, desplazado 4 ranuras
>>>  137 mr r5, r3             |
>>>  139                       | addi r3, r1, 0x38
>>>  142                       | mr r5, r3
```

- Las **2 filas de A** son el ultimo empate de `sched1` (registros ya correctos:
  f12=z, f0=x, f13=y). Meter cualquier `asm` en ESE bloque lo destroza: 6 ensayos
  de extension de rango dentro del `CamCopy` dan 19, 22, 23, 25, 25 y 27 filas.
- Las **4 filas del `addi`** las causa el propio andamio: son 3 insns de `asm`
  de mas en el bloque, y `sched1` deja de rellenar las latencias de los `lfsx`
  con el `addi`/`mr` de los argumentos de `eMulVector`. Probadas 12 colocaciones
  (antes de `bCross`, entre `bCross` y `bScale`, delante de cada asignacion,
  cierre sobre `_lz`, sobre un `bVector3 *_plo` con `"+r"`, con y sin puntero
  explicito): ninguna la recupera.

**Por eso no se aplica.** 6 filas siguen siendo **0 B** y la regla 7 del brief
dice que andamio que no paga es deuda. Queda escrito y reproducible para quien
cierre el racimo A: **con A cerrado, la funcion vale 992 B enteros.**

---

## 3. `Update__8ICEMoverf` (3.868 B): localizado al fichero, y dos vetas agotadas

### 3.1 Donde esta exactamente

Las 15 filas son **un solo racimo** (589-617) y el mapa de lineas del objetivo lo
sitúa fuera de `ICEMover.cpp`: en **`ICEMath.hpp:132-153`** (`FloatToInt` y
`SignedMod`), **`ICEData.hpp:307-309`** (`GetNumKeys` / `GetKey`) y
**`bMath.hpp:232-241`** (`bClamp`), todo en linea en la sentencia que en nuestro
arbol es `ICEMover.cpp:675`:

```c
int frame = ICE::SignedMod(ICE::FloatToInt(fParam * length * current_sec), pShake->GetNumKeys());
ICEShakeData *pKey = pShake->GetKey(frame);
```

### 3.2 El diagnostico, en una tabla

Cinco cantidades, y **el objetivo usa un registro mas que nosotros**:

| valor | objetivo | nuestro |
|---|---|---|
| `GetNumKeys()` en crudo (`mr` de 589) | **r7** | r8 |
| `FloatToInt(...)` (= `a` de `SignedMod`) | r8 | r9 |
| resultado de `SignedMod` | r8 | r11 |
| `bClamp` inferior | r11 | r10 |
| `bClamp` superior | r10 | r9 |

`REG_ALLOC_ORDER` de GPR es `0, 9, 11, 10, 8, 7, ...`: llegar a **r7** significa
que en el objetivo hay **cinco** registros ocupados en esa ventana y en el
nuestro **cuatro**. Es la firma de [[nfsmw-cantidad-fantasma]] (regla 2: «al
objetivo le sobra una cantidad»). Ademas el objetivo mete `a` y el resultado de
`SignedMod` en el MISMO registro (r8) y nosotros en dos.

### 3.3 Las dos vetas, agotadas con cifra

**A. La fuente de `SignedMod`** (8 formas; ninguna la habia tocado nadie: el
permutador de la r50 opera sobre `ICEMover.cpp`, no sobre `ICEMath.hpp`):

| forma | filas |
|---|---:|
| base | 15 |
| `while (a < 0) a += b;` sin llaves | 15 (objeto **identico**) |
| `if (a<0) do { a+=b; } while (a<0);` | 15 (identico) |
| `for (; a < 0; a += b) {}` | 15 (identico) |
| `a = a % b` en vez de `c = a/b; c = c*b; a -= c;` | 15 (identico) |
| `a = a + b` y `a = a - c*b` | 15 (identico) |
| `int c;` declarada arriba | 15 (identico) |
| `if (b <= 0) a = 0; else {...}` (invertido) | 19 |
| variable de bucle separada (`int t = a; ... a = t - c;`) | 20 |

**B. Fantasmas en el llamante** (`ICEMover.cpp:675`), 13 colocaciones: r7, r8,
r9, r10, r11 sueltos y los pares r9+r8, r9+r10, r9+r11, con el cierre detras de
`SignedMod` o detras de `GetKey`. **Todas peores**: 17, 19, 19, 19, 21, 22, 22,
22, 32, 35, 36, 37 y 40 filas. El fantasma mueve el reparto (con r9 pinchado, `FloatToInt` pasa de r9 a
r11 y desaparece un `mr`), pero la cascada cuesta mas de lo que gana.

**Veda nueva**: `ICEMover` no cede ni por la forma de `SignedMod` ni por pines en
su llamante. Lo que hay que buscar es **la quinta cantidad viva** de §3.2 en
`GetKey`/`bClamp` (`ICEData.hpp`), que es el unico sitio de la ventana que no se
ha tocado.

### 3.4 Y un fallo de metodo que hay que dejar escrito

El primer barrido de `SignedMod` dio **nueve resultados identicos**. No era que
la fuente no importara: era que la sombra **no se estaba usando**.
`ICEData.hpp:8` incluye `"ICEMath.hpp"` **en forma RELATIVA**, y una `#include`
entre comillas se resuelve primero contra el directorio del fichero que incluye,
asi que ganaba la del arbol y el guardia `CAMERA_ICE_ICEMATH_H` tapaba la mia
despues.

**Regla**: para sombrear una cabecera que alguien incluye en forma relativa hay
que copiar el **directorio entero** (aqui `src/Speed/Indep/Src/Camera`, 460 kB /
50 ficheros). Y el control que lo destapa es el de la regla 12 del brief: meter
**un error de sintaxis** en la sombra; si compila, la sombra no esta viva.

---

## 4. `TerrainVelocityNoise` y el `__static_init`: no tocados

Sin ensayos nuevos. Siguen como los dejo la r50: `TerrainVelocityNoise` es
`sched1` (r50 §4) y el `__static_init` tiene su segunda cerradura en el
asignador (r50 §2.2.bis). La palanca de §2 de este informe —cantidad fantasma
sobre GPR mas extension de rangos— es aplicable a los dos y **no se ha probado**;
en el `__static_init` con la dificultad anadida de que el codigo lo genera el
compilador y no hay donde meter el `asm` sin cambiar lo que emite.

---

## 5. Incidencia del arbol compartido

Durante ~40 minutos **ninguna compilacion de zCamera fue posible**: otro agente
dejo `src/Speed/Indep/Src/Main/EventSequencer.h` a medias (`class Engine : public
UTL::COM::Object, public IEngine {` -> `parse error before '{'` en la linea 182,
y detras 40 errores en cascada que apuntaban a `IContext`). Se resolvio sin tocar
el arbol: `git show HEAD:src/Speed/Indep/Src/Main/EventSequencer.h` a una sombra
`inc_fix/` puesta con `-I` delante en **todas** las medidas, base incluida, y
comprobado que reproduce la linea base exacta (9 funciones != 100 %, 23.036 B, y
los cinco porcentajes iguales). Al final de la ronda el arbol ya estaba sano y la
verificacion final se hizo con `build_direct.py` + `fncmp` normales.

---

## 6. Propuestas fuera de territorio

1. **`docs/PLAYBOOK.md` — palanca nueva y general: el `while` que a `-O1` no se
   rota.** `stmt.c::expand_end_loop` corta su barrido en la primera nota de
   bloque cuando `optimize < 2`; una **llamada inline en la condicion** de un
   `while`/`for` emite esa nota y mata la rotacion, y con ella
   `jump.c::duplicate_loop_exit_test`. Consecuencias practicas:
   - **La huella en el objetivo es visible con `lmap.py`**: la MISMA linea de
     fuente aparece dos veces, delante del bucle y al fondo, y al fondo suele
     haber un `mr` de mas. Si la ves, la condicion del original **no** llama a un
     accesor.
   - **El arreglo es escribir la condicion con el miembro directo**
     (`while (n > mSize)` en vez de `while (n > size())`). Aqui valio casi un
     punto y dos racimos.
   - Y al reves: si NOSOTROS duplicamos un test que el objetivo no duplica, sobra
     una llamada inline en su condicion... o falta.
2. **`UTLVector.h`** — el cambio de §1.5, con su radio de accion medido.
3. **`docs/PLAYBOOK.md` — la cantidad fantasma sirve para FLOTANTES igual que
   para GPR**, y ademas se combina con una segunda palanca que no estaba escrita:
   **extender el rango de vida** de un valor sacandolo a una local y nombrandola
   en el `asm` de cierre. La primera decide **que registros** quedan libres; la
   segunda decide **en que orden** los pide `local_alloc` (`QTY_CMP_PRI =
   2/vida`). Por separado dan 13 y 15 filas; juntas, 9. En `TrackCar` esa pareja
   abrio dos racimos que llevaban tres rondas cerrados.
4. **`scripts/fncmp.py` / `fndiff.py` — que impriman el RECUENTO DE FILAS.** Esta
   ronda ha tenido tres casos en que el fuzzy baja y las filas tambien (el fuzzy
   castiga un `INSERT`/`DELETE` mucho mas que un operando distinto): 15 filas al
   99,63 % contra 9 filas al 98,27 %. Sin contar filas se descarta lo bueno. Lo
   he tenido que hacer con una sonda de scratchpad (`rows.py`, 30 lineas).

---

## 7. Herramientas de la ronda (scratchpad, ya borradas las salidas pesadas)

`scratchpad/r51_cam/`: `cc.py` (compila zCamera con sombras y banderas extra sin
tocar el arbol), `msr.py` (mide por funcion y el total de la unidad), `rows.py`
(cuenta FILAS, no fuzzy), `fd.py`/`lst.py` (diff y listado contra un `.o` del
scratchpad), `ord.py` (saca el orden real de un patron de instrucciones, con
`--extra` para separar pases), `radio.py` (radio de accion de un cambio de
cabecera), `v.py`/`v2.py`/`v3.py` (variantes de `UTLVector.h`), `w.py`/`wpar.py`
(variantes de `TrackCar.cpp`, en paralelo), `i.py` (fantasmas en `ICEMover.cpp`),
`k.py` (formas de `SignedMod`).

**Los 146 objetos de prueba y los volcados estan borrados** (3,6 GB -> 552 kB).
Quedan solo los `lmap` en texto, que son lo unico que merece releer.
