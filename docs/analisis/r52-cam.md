# r52 — zCamera: `_Storage` CERRADA (1.156 B) y el volcado del planificador como herramienta de diagnostico

**Delta en el arbol: `src/` sin tocar.** `git status --short
src/Speed/Indep/Src/Camera src/Speed/Indep/Libs/Support/Utility` sale vacio, y
`build_direct.py zCamera` + `fncmp` al final dan el listado exacto del principio
(5 funciones / 10.812 B). Todo se midio sobre sombras en el scratchpad.

Dos resultados:

1. **`__Q33UTL11Collectionst8_Storage2...` (1.156 B) casa al 100,0000 %**, cero
   filas, verificado sobre la unidad zCamera entera. La causa estaba en dos
   sitios del reparto de ENTEROS y se cerro con **cinco lineas** en
   `UTLVector.h`. Cabecera compartida -> **va como propuesta (§1.5)**, con el
   radio de accion medido en las **33 SourceLists** que no son zCamera: **30
   salen identicas (+0 B)**, 2 no compilan en el arbol (igual con y sin el
   cambio, por ediciones de otro agente) y 1 no tiene objetivo con el que
   comparar.
2. **`Update__19TrackCarCameraMoverf` (992 B): de 6 filas (r51) a 4**, con el
   mecanismo de las 4 restantes leido en el volcado del planificador, no
   adivinado. Sigue sin aplicarse: 4 filas siguen siendo 0 B (regla 7).

Y una herramienta que cambia el metodo: **`-fsched-verbose-5` sobre una mini-TU
convierte el reparto y el horario en una tabla que se lee** (§2). Las dos
palancas nuevas de esta ronda salieron de ahi, no de barrer a ciegas.

---

## 0. Estado, antes y despues

```
       B  funcion                                        antes -> despues
    3868  Update__8ICEMoverf                             15 filas  (no tocada)
    3604  __static_initialization_and_destruction_0      54 filas  (no tocada)
    1192  TerrainVelocityNoise__11CameraMoverP8bMatrix4P 14 filas  (clasificada, §4)
    1156  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8 15 filas -> 0   *CERRADA*
     992  Update__19TrackCarCameraMoverf                 15 filas -> 4
```

Con el cambio de §1.5 puesto, la unidad pasa de **5 funciones / 10.812 B** a
**4 funciones / 9.656 B**, y ninguna otra funcion de zCamera cambia (medido con
`allfn.py`, que diffea las 453 funciones de la unidad).

---

## 1. `_Storage<CameraAI::Director*,2>::_Storage(const&)`: 15 filas -> 0

### 1.1 De que estaban hechas las 15 filas

Dos permutaciones puras de reparto, ni una instruccion de mas ni de menos
(1.156/1.156 B):

| filas | que pasa |
|---|---|
| 8, 11, 12, 13, 17, 158, 180, 206, 271, 274 | **r24 <-> r25** (`global_alloc`) |
| 14, 15, 16, 18, 19 | **r0 <-> r9** (`local_alloc`) |

- El racimo **r0/r9**: el objetivo carga `mSize` en r9 y `mBegin` en r0;
  nosotros al reves.
- El racimo **r24/r25**: el objetivo pone la constante `0` de `Init()` en r25 y
  `srcEnd` en r24; nosotros al reves.

### 1.2 Las tres piezas, en el orden en que se encontraron

**(a) Una entrada mas de `srcBeg` arregla r0/r9.** `local_alloc` ordena las
cantidades por `QTY_CMP_PRI = floor_log2(n_refs)*n_refs*size/(muerte-nacimiento)`
y reparte por `REG_ALLOC_ORDER`, que en GPR empieza `0, 9, 11, 10, 8, 7, ...`.
Nuestra cantidad `mSize` ganaba a `mBegin` por una referencia. Anadir `srcBeg`
como **entrada** del `asm` que ya estaba sube su `n_refs` y le da r0:

| forma del `asm` de cabecera de `assign` | filas |
|---|---:|
| `asm("" : "+r"(srcEnd));` (la de r51) | 15 |
| `asm("" : "+r"(srcEnd) : "r"(srcBeg));` | **12** (cae r0/r9) |
| `asm("" : "+r"(srcBeg) : "r"(srcEnd));` | **2** (cae r0/r9 **y** r24/r25) |

La saturacion esta medida: con la salida en `srcEnd`, poner `srcBeg` 1, 2 o 3
veces como entrada da **exactamente el mismo objeto**; lo que decide r24/r25 no
es el numero de referencias sino **cual de las dos variables lleva la salida
`"+r"`** (que le parte el pseudo y le cambia el numero de allocno).

**(b) Un `_b` intermedio desengancha el `mr.` del `asm`.** Con la salida en
`srcBeg`, tanto `subf` (`minSize`) como `mr.` (`srcIt = srcBeg`) pasan a depender
del `asm`, y `sched1` los mete en el MISMO ciclo, donde `INSN_PRIORITY` manda y
el `subf` gana (prioridad 3 contra 2 — leido en el volcado, §2). Copiando
`srcBeg` a un temporal **antes** del `asm` y usando ese temporal para `srcIt`, el
`mr.` deja de depender del `asm`, se emite un ciclo antes y casa:

| variante | filas |
|---|---:|
| `asm("" : "+r"(srcBeg) : "r"(srcEnd));` | 2 (`mr.`/`subf` cambiados de orden) |
| + `const_iterator _b = srcBeg;` antes del `asm` y `srcIt = _b` | 2 (**otras dos**: el destino del `subf`) |

**(c) Alargar la vida de `srcBeg` da el ultimo registro.** Con (b), la unica
diferencia era `subf r9, r0, r24` (objetivo) contra `subf r0, r0, r24`
(nuestro): el destino reutilizaba r0 porque la cantidad `srcBeg` moria en esa
misma instruccion. Nombrando `srcBeg` como entrada de un segundo `asm` colocado
DESPUES del `subf`, su rango de vida pasa de largo y r0 deja de estar libre:

    asm("" : "+r"(minSize) : "r"(srcBeg));

**100,0000 %, 1.156 B, 0 filas.**

### 1.3 Barridos que NO valieron (para no repetirlos)

- **Posicion del `asm`**: 7 formas x 4 posiciones (antes de `minSize`, entre
  `minSize` y `srcIt`, tras `srcIt`, tras el `return` del `if`) = 28 ensayos.
  Fuera de la posicion de cabecera el `asm` o no toca el reparto o se lo lleva
  DCE: `A2` en las posiciones 3 y 4 da **exactamente** el resultado de no poner
  `asm` (15 filas), que es la firma de la muerte por DCE de [[nfsmw-cantidad-fantasma]].
- **Orden de las sentencias** (`srcIt` antes de `minSize`, `minSize = srcEnd -
  (srcIt = srcBeg)`, declaraciones separadas): las cuatro formas dan **las mismas
  8 filas**, porque el `mr.` se parte en `mr` + `cmpwi`.
- **Clobbers de entero** (la palanca nueva del brief) en el `asm` de cabecera:
  `r0` 14 filas, `r9` 22, `r11` 21, `r23` 87, `r24` 83, `r25` 89, `r26` 85,
  `r27` 75. **Ninguno mejora**; el clobber de un preservado hunde la funcion
  entera. La palanca util en enteros no fue el clobber sino **el operando de
  entrada**.
- `Init()` reescrito (orden inverso, temporal `_z`, `_z` con `asm`), ctor de
  copia sin `Init()`, `assign(src)` directo: los cuatro **neutros**.

### 1.4 Aviso de metrica, otra vez

El fuzzy manda al sitio equivocado en los dos sentidos:

| variante | fuzzy | filas |
|---|---:|---:|
| `asm("" : "+r"(srcEnd) : "r"(srcBeg))` | **99,7751 %** | 12 |
| `asm("" : "+r"(srcBeg) : "r"(srcEnd))` | 99,3080 % | **2** |
| la solucion final | 100,0000 % | 0 |

La segunda es la buena y tiene **medio punto MENOS** de fuzzy que la primera.

### 1.5 PROPUESTA (cabecera compartida: NO aplicada, regla 4 del brief)

`src/Speed/Indep/Libs/Support/Utility/UTLVector.h` (el fichero es **CRLF**:
aplicar preservandolo, cf. [[nfsmw-pr-upstream-workflow]]):

```diff
@@ -116,9 +116,11 @@
     void assign(const_iterator srcBeg, const_iterator srcEnd) {
-        asm("" : "+r"(srcEnd));
+        const_iterator _b = srcBeg;
+        asm("" : "+r"(srcBeg) : "r"(srcEnd));
         size_type minSize = srcEnd - srcBeg;
-        const_iterator srcIt = srcBeg;
+        const_iterator srcIt = _b;
+        asm("" : "+r"(minSize) : "r"(srcBeg));
         if (srcIt == 0) {
```

**Vale 1.156 B de `matched_code`** (`_Storage` pasa de 99,6713 % a 100 %) y deja
zCamera en 4 funciones abiertas / 9.656 B.

**Radio de accion**, con `radio.py` (cada unidad compilada dos veces, con y sin
la sombra; se comparan el conjunto de funciones != 100 % y los bytes, y se
comprueba por md5 que los objetos salen DISTINTOS, o sea que el cambio llega):

| resultado | unidades |
|---|---|
| **identico (mismo conjunto de funciones y mismos bytes)** | zAI, zAnim, zAttribSys, zBWare, zDebug, zDynamics, zEAXSound, zEAXSound2, zEagl4Anim, zFEng, zFe, zFe2, zFeOverlay, zGameModes, zGameplay, zLua, zMain, zMisc, zMiscSmall, zMission, zOnline, zPhysics, zPhysicsBehaviors, zPlatform, zRender, zSim, zSpeech, zTrack, zWorld, zWorld2 (**30**) |
| **no compilaba en el arbol, igual con y sin el cambio** | zEcstasy, zFoundation |
| sin objetivo con el que comparar | zComms |

Los 30 objetos salen distintos por md5 y **+0 B en los 30**. zEcstasy y
zFoundation hay que re-medirlos cuando el arbol vuelva a compilar (fallan por
`EmitterSystem.cpp` y `CARP.h`, no por esta cabecera: fallan igual **sin** la
sombra).

---

## 2. El metodo: leer el planificador en vez de barrerlo

Lo que desbloqueo las dos funciones fue dejar de adivinar y **volcar el
planificador**:

    ngccc <cflags> -fsched-verbose-5 -c -o x.o mini_tu.cpp     # a stderr

El volcado trae, por bloque basico y por pasada (`sched1` y `sched2`, este
ultimo marcado `-- after reload`), dos cosas que valen la ronda:

1. **La tabla de dependencias con `prio`**, que es el camino critico hasta el
   final del bloque:

   ```
   ;;      insn  code    bb   dep  prio  cost   blockage units
   ;;      118    64     0     3     3     1    1 - 13   iu2  : 127 120
   ;;      126   514     0     1     2     1    0 -  0   [iu iu2] : 127
   ```

   Ahi se ve de un vistazo por que `subf`(118, prio 3) se emite antes que
   `mr.`(126, prio 2): **no es un empate, es prioridad**, y ningun desempate
   (peso de registros, clase, numero de dependientes, LUID) llega a mirarse.

2. **El log de la lista de listos por ciclo**, que dice quien le robo la ranura
   a quien:

   ```
   ;;	Ready list (t = 38):    742  676  674  745  708  692  724
   ;;		--> scheduling insn <<<724>>> on unit lsu
   ;;		--> scheduling insn <<<674>>> on unit none
   ```

   La lista sale ordenada ASCENDENTE por preferencia y **se coge el ultimo**.
   `unit none` = un `asm` de cero bytes: **consume ranura de emision** (el
   limite es `ISSUE_RATE`=2, no las unidades funcionales).

**La mini-TU es imprescindible.** Con el TU real (`CameraAI.cpp`, 62.000 lineas
de volcado y una ICE en STLport con `-dS`) no hay quien encuentre el bloque. Un
`.cpp` de cuatro lineas que instancia la plantilla:

```cpp
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
namespace CameraAI { class Director; }
typedef UTL::Collections::_Storage<CameraAI::Director *, 2> S;
void force_st(void *p, const S &b) { new (p) S(b); }
```

compila en **3,8 s**, **reproduce el simbolo con el nombre mangled exacto** (no
hace falta la definicion de `Director`: basta declararla) y reproduce las filas
del racimo de cabecera. `-dS`/`-dR` **no escriben fichero** con este driver
(`ngccc`); todo sale por stderr.

---

## 3. `Update__19TrackCarCameraMoverf` (992 B): de 6 filas a 4

### 3.1 Lo que dijo el volcado

La receta de r51 (dos fantasmas fr0/fr13 + `camcopy xyz/xzy`) deja 6 filas: 2 del
orden de dos `stfs` de `Look` y 4 del `addi r3,r1,0x38` / `mr r5,r3` desplazados.
El volcado explica las 4 exactamente:

```
;;	Ready list (t = 38):    742  676  674  745  708  692  724
;;		--> scheduling insn <<<724>>> on unit lsu     (lfsx f10)
;;		--> scheduling insn <<<674>>> on unit none    (FANTASMA)
;;	Ready list (t = 39):    742  676  745  708  692
;;		--> scheduling insn <<<692>>> on unit lsu     (lfsx f11)
;;		--> scheduling insn <<<676>>> on unit none    (FANTASMA)
;;	Ready list (t = 40):    742  745  708
;;		--> scheduling insn <<<708>>> on unit lsu     (lfsx f12)
;;		--> scheduling insn <<<742>>> on unit iu2     (addi r3)  <- 2 ciclos tarde
```

`674`/`676` (los fantasmas) y `742` (el `addi`) tienen **la misma prioridad, 26**,
el mismo peso de registro, la misma clase y **los mismos 8 dependientes**. Lo
desempata `INSN_LUID`, y los fantasmas nacen antes en el RTL. El objetivo pone
`addi r3` y `mr r5` en esas dos ranuras.

### 3.2 La palanca que gano dos filas

Materializar `&look_offset` en una variable **con un `asm` que la fije**, antes
de los fantasmas: eso le da al `addi` un LUID mas bajo que los fantasmas y gana
la ranura del ciclo 38.

```c
bVector3 look_offset;
bVector3 *_plo = &look_offset;
asm("" : "+r"(_plo));                 // <- el addi pasa a nacer AQUI
register float _p0 asm("fr0");
register float _p13 asm("fr13");
asm("" : "=f"(_p0));
asm("" : "=f"(_p13));
float _lx = TrackCarLookOffsetX[CameraType];   // ... _ly, _lz
look_offset.x = _lx;                            // ... .y, .z
asm("" : "+m"(look_offset) : "f"(_p0), "f"(_p13));
eMulVector(_plo, CarToFollow->GetGeometryOrientation(), _plo);
```

**4 filas, 992 B.** `addi r3` ya esta en su sitio (fila 135 exacta). Notese que
con el cierre en `"+m"(look_offset)` la **extension de rango de `_lx/_ly/_lz`
de r51 deja de hacer falta** (misma cuenta de filas con y sin ella).

Solo `bVector3 *_plo = &look_offset;` **sin** el `asm` no vale (6 filas): sin el
`asm`, `cse` rematerializa la direccion en el punto de la llamada.

### 3.3 Por que no bajan de 4, con la cifra

Lo que falta es `mr r5, r3`, dos ranuras tarde. El volcado dice por que: el
tercer argumento sale del `asm` (`"+r"(_plo)`), asi que **`mr r5` depende de el**
y no puede estar listo hasta que el `asm` se planifique (ciclo 41). Barridos
para romper esa cadena, todos a 4 filas o peor:

| intento | filas |
|---|---:|
| `_plo3 = _plo` copiado ANTES del `asm`, `eMulVector(_plo,_,_plo3)` | 4 |
| `_plo3 = &look_offset` antes del `asm` | 4 |
| `asm` sobre `_plo3` y pasar `(_plo3,_plo)` | 4 |
| un solo `asm` con `"+r"(_plo), "=f"(_p0), "=f"(_p13)` | 10 |
| dos `asm` repartiendo `+r` y `=f` | 6 |
| `_plo` como ENTRADA de un fantasma (`"=f"(_p0) : "r"(_plo)`) | 6 |
| `asm volatile("" : : "r"(_plo))` | 35 |
| primer arg `&look_offset`, tercero `_plo` | 8 |
| `_plo` como entrada del cierre | 6 |

Y una **rejilla completa** de 48 combinaciones {sin `_plo` / `_plo` / `_plo`+`asm`}
x {orden de los dos fantasmas} x {8 formas de cierre: `+m(look_offset)`,
`+m(hcomp)`, `+m(up)`, `+m(Eye)`, `+f(dT)`, `+f(dT)`+`_l`, `+m`+`_l`,
`+f(distance)`}: **el minimo es 4 y lo alcanzan 8 combinaciones**, todas con
`_plo`+`asm`. El cierre por memoria no baja la prioridad de los fantasmas: la
escritura de memoria del `asm` genera dependencia con la llamada igual que
`look_offset`.

### 3.4 Las otras 2 filas (racimo A) siguen igual

Son los dos primeros `stfs` de `Look` (`0x98` y `0x90`) cambiados de orden, con
los registros YA correctos (f12=z, f0=x, f13=y). El volcado los pone en la misma
situacion: empate de `sched1` resuelto por LUID, o sea por el orden de las
sentencias de la fuente. Repetido el barrido completo de r51 (**36 ordenes de
declaracion x almacenamiento**, confirmando su tabla exacta: `xyz/xzy` = 6,
`zyx/zxy` = 8, resto 9-12) y anadida una familia nueva que r51 no probo —
asignacion **directa** campo a campo sin temporales, 6 ordenes— que da **29 filas
en los seis**. `xyz/xzy` sigue siendo el optimo.

**No se aplica nada de esto**: 4 filas siguen siendo 0 B (regla 7). Queda escrito
y reproducible; con el racimo A y el `mr r5` cerrados, la funcion vale 992 B.

---

## 4. `TerrainVelocityNoise` (1.192 B): clasificada, no tocada

Sus 14 filas son **dos racimos independientes**, no uno:

- **7 filas de coma flotante**: una permutacion **f0 <-> f12** en la cadena
  `lfs`/`fmuls`/`fmadds` de `0x38(r30)` y `0x3c(r30)` (filas 115, 116, 119, 120,
  121, 125, 128). Registros, no estructura.
- **7 filas de enteros**: el reparto y el orden de los `lis` de las direcciones
  del pool (r9/r10/r7/r8/r11) mas **un `lis` desplazado una ranura** (filas 126,
  127, 129, 130, 132, 133, 136).

El segundo racimo tiene la misma forma que el que se cerro en `_Storage`
(prioridad de `local_alloc` sobre GPR) y **la palanca del operando de entrada de
§1.2(a) no se ha probado ahi**. Es el candidato mas barato que queda en zCamera
despues de `TrackCar`.

---

## 5. Lo que NO se ha tocado

- **`Update__8ICEMoverf`** (3.868 B): veda de r51 vigente (1.606 compilaciones
  del permutador, 8 formas de `SignedMod`, 13 colocaciones de fantasma). No se ha
  gastado ni una compilacion en ella.
- **`__static_initialization_and_destruction_0`** (3.604 B): 54 filas y dos
  cerraduras, la segunda en el asignador. Sin ensayos nuevos.

---

## 6. Propuestas fuera de territorio

1. **`UTLVector.h`** — el cambio de §1.5, con radio medido en 31 unidades. Es el
   unico de este informe que vale bytes YA (1.156 B).
2. **`docs/PLAYBOOK.md` — `-fsched-verbose-5` + mini-TU como procedimiento
   estandar de diagnostico de reparto y horario** (§2). Convierte "probar N
   formas" en "leer la tabla": en esta ronda dijo con numeros que un racimo era
   prioridad (no empate, luego ningun desempate lo iba a mover) y que otro era
   LUID (luego habia que mover la sentencia en la fuente). Detalles que hay que
   dejar escritos:
   - la lista de listos sale **ascendente** y se coge el **ultimo**;
   - `unit none` es un `asm` de cero bytes y **si gasta ranura de emision**
     (`ISSUE_RATE` = 2);
   - `-dS`/`-dR` **no escriben fichero** con `ngccc`; todo va a stderr;
   - un `.cpp` de 4 lineas que instancia la plantilla reproduce el simbolo
     mangled exacto sin necesitar el tipo completo, y compila en 4 s.
3. **`docs/PLAYBOOK.md` — palanca nueva de enteros: el OPERANDO DE ENTRADA.** El
   brief traia `asm("" : : : "rN")` (clobber) como palanca de enteros; medida en
   `_Storage` **es la mala**: los ocho clobbers probados empeoran (de 14 a 89
   filas). La que funciona es **anadir la variable como entrada `"r"(x)` a un
   `asm` que ya existe**: sube su `n_refs`, sube su `QTY_CMP_PRI` y le cambia el
   registro sin gastar ranura. Y su hermana: **cual de dos variables lleva la
   salida `"+r"` decide el reparto de los PRESERVADOS** (r24/r25 aqui), porque le
   parte el pseudo y le cambia el numero de allocno.
4. **`docs/PLAYBOOK.md` — el temporal que desengancha.** Cuando un `asm` de
   andamio esta en la cadena de dos consumidores y solo uno debe depender de el,
   copiar el valor a un temporal **antes** del `asm` y usar el temporal en el
   consumidor que sobra. En `_Storage` valio dos filas; es el mismo truco que en
   `TrackCar` NO funciona porque alli `cse` funde las dos copias de la direccion.
5. **`docs/PLAYBOOK.md` — alargar el rango de vida para NEGAR un registro.**
   `asm("" : "+r"(otro) : "r"(x))` colocado detras del punto donde `x` moriria
   impide que el destino de esa instruccion reutilice su registro. Cerro la
   ultima fila de `_Storage`.

---

## 7. Herramientas de la ronda

En `scratchpad/r52_cam/` (208 kB; los 1,1 GB de objetos y volcados **borrados**):

| fichero | que hace |
|---|---|
| `w.py` | sweeper paralelo de variantes de UN `.cpp` que compila solo (TrackCar); imprime fuzzy, tamano, **numero de filas** e indices |
| `wh.py` | igual pero con **sombra de DIRECTORIO** (para cabeceras con includes relativos) y un TU a elegir |
| `radio.py` | radio de accion: compila cada SourceList con y sin la sombra, compara funciones y bytes, y controla por md5 que el cambio llega |
| `allfn.py` | lista TODAS las funciones != 100 % de una unidad compilada con una sombra |
| `frows.py` | filas de una funcion contra el objetivo, con contexto |
| `dump.py` | compila con `-fsched-verbose-5` y deja el volcado |
| `lst.py` | listado lado a lado de un rango de instrucciones |
| `mini_st.cpp` | la mini-TU de `_Storage` (4 lineas, 3,8 s) |
| `tcdefs.py` | generadores de variantes de `TrackCar::Update` |
