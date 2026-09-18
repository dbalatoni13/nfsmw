# r63 - world (zWorld, zWorld2, zEagl4Anim)

**Titular. El `.text` de zEagl4Anim EN EL ENLACE pasa de 27 bloques de
permutacion a DOS, y los dos son la MISMA funcion de 8 B; con el unico cambio
que falta --y esta medido, con la prueba de que no toca a nadie mas-- se queda
en CERO: 318 simbolos contra 318, en el mismo orden, por primera vez. Nueve
ficheros tocados, los nueve exclusivos de la unidad, `fncmp` IDENTICO (2 de 318)
y `vtord` clavado en 0 de 31.**

Y un segundo resultado, este de metodo: **la mitad de la veda r60 de zWorld2 es
FALSA.** El `rodata-168` que la r60 midio al intercambiar `WRoadNetwork.cpp` y
`WPathFinder.cpp` **no existe: es `keep.lst` rancio**. Con las 20 correcciones
que da `lcfix.py --check` el `.rodata` vuelve a `dtam +0` y el swap **paga
1.156 B**, ademas de quitar el bloque de permutacion mas gordo de la unidad
(17 simbolos / 5.736 B). No lo aplico porque exige correr `lcfix` (regla 2).

| unidad | sha1 del `.o` (x3) | `fncmp` | `linkdelta` | metrica de bytes | `trypromo` |
|---|---|---|---|---|---|
| **zEagl4Anim** | `174482e0bb6a2dbdcdedda9a3b54a7b8ce3cf745` | **2 de 318** (igual) | `.text +4  resto IGUAL` | **83.769 -> 82.242** (-1.527) | DOL ROTO (7bef3db13a67) |
| zWorld | `2b35b5c0b5a0994e6af25433d1e48126397fcda8` | 4 de 582 (igual) | `.text +0  IGUAL` | 18.688 (sin cambio) | DOL ROTO (60faea1e56a3) |
| zWorld2 | `3df7abca402210c486032811393a31f15f07b167` | 2 de 357 (igual) | `.text +0  bss+64` | 86.620 (sin cambio) | DOL ROTO (a38849185f1e) |

Ninguna promociona. La metrica de bytes es `rangos.py` (bytes distintos DENTRO
de los rangos de `splits.txt` de la unidad, contra el ELF original, sustituyendo
SOLO esa unidad en el enlace). `dolwhere zWorld` sigue en **24.884 B**;
`dolwhere zEagl4Anim` y `dolwhere zWorld2` **no se pueden correr** (secciones
descuadradas por el `.text +4` y por el `bss+64`).

---

## 0. Lo que se ha aplicado, y donde

Nueve ficheros, **los nueve exclusivos de zEagl4Anim** (comprobado con
`grep -rln` sobre todo `src/`: `DeltaChan.h` y `CompoundChannel.h` solo las
incluyen `.cpp` de esta unidad, y los `.cpp` solo los incluye su SourceList):

    src/Speed/Indep/SourceLists/zEagl4Anim.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnCycle.cpp
    src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnStatelessF3.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnStatelessQ.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnTurnBlender.cpp
    src/Speed/Indep/Src/EAGL4Anim/DeltaChan.cpp   DeltaChan.h
    src/Speed/Indep/Src/EAGL4Anim/FnEventBlender.cpp
    src/Speed/Indep/Src/EAGL4Anim/CompoundChannel.cpp   CompoundChannel.h

Mas **un comentario** (cero bytes de objeto, sha1 identico antes y despues) en
`src/Speed/Indep/SourceLists/zWorld2.cpp` con la veda r60 corregida.

**Cero cabeceras compartidas tocadas. Cero `configure.py`, `config/`,
`splits.txt`, `symbols.txt`, `keep.lst`, `scripts/`.**

---

## 1. La herramienta que lo hace posible: `ltorder`, no `textorder`

`textorder.py` compara el orden del `.o`, y ahi zEagl4Anim emite **106 simbolos
que el enlazador TIRA**: sus deltas llevan ese ruido y dice "251 descolocadas"
tanto antes como despues. **`ltorder.py` (sonda de la r62, en
`scratchpad/eagl62/`) compara el orden en el ENLACE**, que es lo que mueve el
DOL, y da la unica cifra accionable:

| | bloques de permutacion (`ltorder`) | saltos de delta (`textorder`) |
|---|---:|---:|
| r62 (entrada) | 27 | 47 |
| r63 (salida) | **2** | **4** |

Los 2 que quedan son **la misma funcion**: `GetAttributes__CQ29EAGL4Anim6FnAnim`,
8 B. Ver la seccion 4.

---

## 2. La escalera de zEagl4Anim, paso a paso y con su cifra

Cada linea es una compilacion (`bd.py`, 6 s) mas una medida (`rangos.py`, 22 s).
`fncmp` se comprobo en **todos** los pasos: **2 de 318 en todos**, o sea
**ni una instruccion cambiada en toda la ronda**. `vtord` tambien: **0 de 31**.

| # | cambio | mecanismo | bytes | bloques |
|---|---|---|---:|---:|
| 0 | base r62 | | 83.769 | 27 |
| 1 | `inline` explicito a las 4 de `FnCycle` | codigo normal -> cola | 83.785 (+16) | 25 |
| 2 | `inline` explicito a las 3 de `FnRawStateChan` | codigo normal -> cola | **83.535** (-250) | 23 |
| 3 | `FnPoseBlender.cpp` delante de `FnRawPoseChannel.cpp`, con `FnRawPoseChannel.h` suelto delante | cuerpos y clase separados | **83.239** (-296) | 21 |
| 4 | `RawEventChannel.cpp` movido entre `PoseAnim.cpp` y `RawPoseChannel.cpp`, con sus dos `.h` sueltos en el sitio viejo | idem | **82.599** (-640) | 19 |
| 5 | `inline` a `GetAttributes` de `FnStatelessF3`/`FnStatelessQ` | codigo normal -> cola | 82.706 (+107) | 16 |
| 6 | `UseFPS` al final de `FnStatelessQ.cpp`; `FnTurnBlender.cpp` reordenado al orden del objetivo | orden dentro del `.cpp` | **82.416** (-290) | 9 |
| 7 | `#include "DeltaQ.h"` delante de `FnDeltaF1.cpp` | orden de PARSEO de la clase | 82.419 (+3) | 7 |
| 8 | los 6 cuerpos `inline` (`EulF3/QuatF4/TranF3` + `*Interp`) de `DeltaChan.cpp` al final de `FnEventBlender.cpp` | punto de la cola | **82.221** (-198) | 6 |
| 9 | los 2 `GetLength` fuera de la clase en `DeltaChan.h`, detras del ctor | orden dentro de la cola | 82.237 (+16) | 4 |
| 10 | `GetAttributes` y `Eval` de `FnCompoundChannel` fuera de la clase, a `CompoundChannel.cpp` con `inline` | pasada de `finish_file` | 82.242 (+5) | **2** |

**Neto -1.527 B y 25 bloques cerrados.** Los `+16`, `+107`, `+3`, `+5` son ruido
de coincidencia de byte y estan pagados de sobra: cada uno cierra bloques que
son condicion necesaria para promocionar.

### 2.1 Las tres palancas, y cual sirve para que

1. **`inline` EXPLICITO en la definicion fuera de clase** manda la funcion a la
   cola de `finish_file` sin cambiar una instruccion (pasos 1, 2, 5, 8, 10).
2. **Sacar el cuerpo de la clase al `.cpp`** hace lo contrario (pasos 9, 10 en la
   otra direccion; ya estaba en la r62 seccion 3).
3. **Separar los disparadores** --la cabecera donde el objetivo completa la
   clase, el `.cpp` donde emite los cuerpos-- es lo que convierte un movimiento
   "imposible" en gratis (pasos 3, 4). Ver la seccion 3.

### 2.2 NEGATIVO MEDIDO, y ahorra una ronda: meter el cuerpo DENTRO de la clase NO es lo mismo que `inline`

El paso 1 se intento primero metiendo los cuatro cuerpos de `FnCycle` **dentro
de la clase** en `FnCycle.h`. Resultado:

    Eval__FnCycle       128 / 448   (objetivo 448)
    EvalEvent__FnCycle  136 / 448
    EvalSQT__FnCycle     88 / 256
    EvalPhase__FnCycle   88 / 256
    linkdelta           .text -768   rodata-24

**GCC 2.9 aplaza el cuerpo de un metodo definido en clase hasta el final de la
clase, y para entonces ya no puede inlinar `GetInRangeTime`** (que esta definida
mas abajo en la misma clase): las cuatro se quedan en un tercio de su tamano. Con
`inline` explicito en el `.cpp` el cuerpo se parsea en su punto, `GetInRangeTime`
se inlina igual y el tamano sale **clavado**. Escrito en `FnCycle.cpp`, donde
`previo.py FnCycle` lo encuentra.

---

## 3. El hallazgo de metodo: la cabecera y el `.cpp` son DOS palancas distintas, y hay que usar las dos a la vez

`parseord.py` ya lo decia en su docstring ("la posicion de la CLASE y la de los
CUERPOS las mandan cosas distintas"), pero no habia una medida. Aqui hay dos, y
las dos con el mismo patron:

**Caso A -- `FnRawPoseChannel` / `FnPoseBlender`.** El objetivo emite los cuerpos
de `FnPoseBlender.cpp` DELANTE de los de `FnRawPoseChannel.cpp` (posiciones
136-140 de 318), pero completa la clase `FnRawPoseChannel` ANTES (su vtable va
DETRAS, y el orden de vtables es el inverso del de completado).

| variante | bytes | `vtord` |
|---|---:|---:|
| base (r62) | 83.535 | 0 de 31 |
| swap pelado de los dos `#include` | **83.196** | **2 de 31** |
| swap + `#include "FnRawPoseChannel.h"` suelto delante | 83.239 | **0 de 31** |

**43 B es lo que cuesta conservar el orden de vtables, y merece la pena**: la
alternativa deja dos vtables descolocadas en una unidad que las tenia perfectas.

**Caso B -- `RawEventChannel.cpp`.** Su unica funcion de codigo normal
(`RawEventChannel::Eval`, 480 B) va en el objetivo en la posicion 213, entre
`PoseAnim.cpp` y `RawPoseChannel.cpp`; su `.cpp` estaba en la posicion 153 de la
SourceList. Moverlo pelado: **82.601 B pero `vtord` 0 -> 4**. Moverlo dejando
`RawEventChannel.h` y `FnRawEventChannel.h` sueltos en el sitio viejo:
**82.599 B y `vtord` 0 de 31**. Aqui la separacion sale **gratis y ademas mejor**.

**La regla, para el catalogo:** cuando mover un `#include` arregla el `.text` y
rompe `vtord`, **no se descarta el movimiento: se parte en dos**. La cabecera se
queda (o se adelanta) sola con un `#include` explicito en la SourceList, y el
`.cpp` se va a su sitio. Cuesta entre 0 y 43 B.

---

## 4. Lo unico que le falta a zEagl4Anim para tener el `.text` PERFECTO, y esta medido

`GetAttributes__CQ29EAGL4Anim6FnAnim`, **8 B**. El objetivo la tiene en la
posicion **82** de 318 --codigo normal, justo donde entra `FnAnim.cpp` en la
SourceList-- y nosotros en la **244**, en la cola, porque su cuerpo esta DENTRO
de la clase en `FnAnim.h:105`.

`FnAnim.h` es COMPARTIDA (llega a `zAnim` y a `zMain` por
`eagl4AnimBank.h` -> `Animation/AnimBank.cpp` y `Animation/AnimCtrl.hpp`), asi
que **no la he tocado** (regla 5). La he medido con la tecnica de sombreado de
`nfsmw-sombrear-cabecera`: copia del directorio entero a
`scratchpad/world63/shadowsrc` con `-I` delante, **con el control de error de
sintaxis obligatorio** (comprobado: la compilacion falla con el control puesto y
pasa al quitarlo).

**El parche:**

    /* src/Speed/Indep/Src/EAGL4Anim/FnAnim.h : 105-107   -- QUITAR el cuerpo */
        virtual const AttributeBlock *GetAttributes() const;

    /* src/Speed/Indep/Src/EAGL4Anim/FnAnim.cpp : al final del namespace -- ANADIR */
    const AttributeBlock *FnAnim::GetAttributes() const {
        return nullptr;
    }

**Lo que da, medido con el sombreado:**

    ltorder zEagl4Anim   ->  318 simbolos contra 318, CERO bloques.  ORDEN PERFECTO.
                             (las posiciones 82 y 231-244 casan una a una)
    fncmp                ->  2 de 318   (sin cambio)
    linkdelta            ->  .text +4  resto IGUAL   (sin cambio)
    rangos               ->  82.268 B  (+26 sobre 82.242: ruido de coincidencia)

**Lo que hay que saber antes de aplicarlo, y son tres cosas:**

1. **Solo `zEagl4Anim.o` define ese simbolo, y ya era asi en el ORIGINAL.**
   Barrido de los objetos EXTRAIDOS y de los nuestros: una sola definicion, de
   8 B, en `zEagl4Anim.o`, en los dos lados. Nadie mas lo emite, asi que sacarlo
   de la clase no le quita el cuerpo a ninguna otra unidad.
2. **`zAnim` y `zMain` no cambian ni un byte de codigo ni de dato.** Compilados
   los dos con y sin el sombreado a un `.o` del scratchpad (sin tocar
   `build/`): `.text`, `.rodata`, `.data`, `.bss` y `.ctors` **identicos, hash a
   hash**; lo unico que se mueve es `.debug*` (y en el caso del sombreado, la
   ruta del fichero, que en la aplicacion real ni aparece). La llamada es
   VIRTUAL --`FnAnim::GetAttribute<T>` la hace por vtable-- asi que no hay nada
   que inlinar y no habia nada que perder.
3. **Aparece una vtable de 32.** `GetAttributes` pasa a ser el KEY METHOD de
   `FnAnim` (`nfsmw-la-virtual-que-era-pura` al derecho) y GCC 2.9 emite
   `_vt.Q29EAGL4Anim6FnAnim`, 144 B, que el objetivo NO tiene: `vtord` pasa de
   `0 de 31` a `2 de 32`. **No cuesta un byte de DOL**: nadie la referencia y
   `-strip-unused-data` se la lleva (por eso `linkdelta` sigue diciendo
   `resto IGUAL` y `.rodata dtam +0`). Es feo en el `.o` y transparente en el
   enlace; el que lo aplique que lo sepa.

---

## 5. zWorld2: la veda r60 partida por la mitad

`zWorld2.cpp:82-91` llevaba desde la r60 esta veda: intercambiar
`WRoadNetwork.cpp` y `WPathFinder.cpp` coloca bien el bloque
`AStarSearch`+`PathFinder` (17 simbolos, 5.736 B, de la posicion 191 a la 86)
**pero "PAGA CARO": `fncmp` 2 -> 3 y `linkdelta` `bss+64` -> `rodata-168 bss+64`.**

### 5.1 El `rodata-168` NO ES REAL

Es `keep.lst` rancio. Probado en los dos sentidos:

* **El `.o` mide exactamente lo mismo** con y sin swap, seccion a seccion leida
  del ELF: `.rodata 4808`, `.data 396`, `.bss 1408`. Lo que cambia es la
  NUMERACION: 51 simbolos `$LCnnn` distintos a cada lado (el swap renumera el
  pool entero de `$LC402` en adelante).
* Las 13 entradas `zWorld2.o:$LCnnn` de `keep.lst` pasan por tanto a proteger la
  cadena equivocada y `-strip-unused-data` se lleva 168 B que no debia. Con una
  **copia privada** de `keep.lst` con las **20 correcciones** que da
  `lcfix.py --check zWorld2`, `.rodata dtam` vuelve a **+0**.

### 5.2 Con keep.lst al dia, el swap PAGA

                                          bytes    bloques ltorder
    sin nada ...........................  86.620      10
    solo los dtores sinteticos (5.3) ...  86.614       9
    solo el swap .......................  85.464       9
    LOS DOS ............................  85.458       8      -1.162 B

y el bloque que desaparece es **el mas gordo de la unidad**: los 17 simbolos /
5.736 B de `AStarSearch`+`PathFinder`. A zWorld2 le quedarian **OCHO** simbolos
fuera de sitio, todos de la cola de inlines.

### 5.3 La segunda pieza: los destructores sinteticos, receta ya escrita en el arbol

`WorldConn.h` ya tiene DOS guardas de este tipo
(`WORLDCONN_H_IMPLICIT_PKT_BODY_DTOR` para zMain,
`WORLDCONN_H_IMPLICIT_PKT_EFFECT_DTORS` para zSim), con el mecanismo escrito al
lado: **el objetivo emite `_._Pkt_*` DELANTE de `ConnectionClass/Size/Type`, y
eso solo pasa si el destructor es SINTETIZADO** --los metodos implicitos entran
en la cola de inlines antes que los declarados--. A `zWorld2` le pasa lo mismo
con `Pkt_Body_Service` y `Pkt_Effect_Service`.

Neutralidad del guarda **probada**, no supuesta: compilados `zFe`, `zMain` y
`zSim` con y sin el, a `.o` del scratchpad. `zFe` sale con el **sha1 identico**;
`zMain` y `zSim` cambian **solo `.line`** (mismo tamano), que no llega al DOL.

### 5.4 Lo que SI cuesta el swap, y tampoco es lo que decia la r60

`__static_initialization_and_destruction_0` deja de casar: **1.472 B de
`matched`**. Pero **NO cambia el orden de los constructores**: `fndiff` da las
65 filas como el mismo codigo con **`r25` y `r31` intercambiados**
(`ARG_MISMATCH` de registro en las 65, y el resto de la funcion identica insn a
insn). La palanca que queda ahi es de REPARTO, no de orden --y por tanto es
atacable con un pin.

**Balance honesto: -1.162 B de `linked` contra -1.472 B de `matched`.** Con la
regla `nfsmw-linked-es-la-metrica` gana el swap; con `matched` pierde. Lo dejo
como propuesta, con la cifra, porque ademas **no lo puedo aplicar yo**: exige
correr `lcfix.py zWorld2`.

---

## 6. zWorld: nada aplicado, y por que

**El `.text` de zWorld ya esta en orden en los primeros 520 simbolos de 582.**
`ltorder` da 11 bloques y **todos caen en las posiciones 521-582**, o sea en la
cola de inlines diferidas. Los simbolos implicados son
`__as__Q36Attrib3Gen5tires` (48 B, el `operator=` implicito de una clase de
`Generated/AttribSys/Classes/tires.h`), `_._Q210RenderConn27Pkt_VehicleFragment_Service`
(52 B, `Render/RenderConn.h`), `OnGrowRequest__Q23UTLt6Vector2...` y
`_._Q23UTLt11FixedVector3...` (plantillas de `UTL`), `SetDirty__9SpaceNode`,
`ClassKey__Q36Attrib3Gen12emittergroup` y `__20eDynamicLightContext`.

**Las cabeceras que los generan son COMPARTIDAS** (`tires.h` llega a zFe,
zLua, zSpeech y a seis `.cpp` de Physics; `RenderConn.h` a zMain, zPhysics y
zWorld). Regla 5: no las toco. La de `Pkt_VehicleFragment_Service` es
**exactamente el mismo caso que 5.3** --el destructor tiene que ser sintetizado--
y va en la seccion 7 como propuesta.

Y el reparto del deficit de zWorld no ha cambiado: de sus **18.688 B**,
**12.214 son `.rodata`** --el orden del pool de cadenas, que necesita `lcfix` y
la regla 2 me lo prohibe--, 335 son la `.data` que cuelga de ese orden, y los
6.139 de `.text` son las cuatro funciones de `fncmp` (las cuatro con veda de
r36e/r46/r49/r61/r62; `previo.py` las tiene todas) mas los 11 bloques de cola.

---

## 7. Lo que pido, con la cifra

### 7.1 `FnAnim.h` -- cierra el orden de `.text` de zEagl4Anim (seccion 4)

El parche esta arriba, literal. **Vale: `ltorder` 2 bloques -> 0, orden
PERFECTO.** Coste medido: `+26 B` de coincidencia y una vtable de mas en el `.o`
que el enlace estripa. **Riesgo medido y NULO** para `zAnim` y `zMain`
(secciones de codigo y dato identicas hash a hash).

### 7.2 zWorld2 -- el swap + los dtores sinteticos + `lcfix` (seccion 5)

Los tres van JUNTOS o ninguno, y en este orden:

    1) src/Speed/Indep/SourceLists/zWorld2.cpp
       intercambiar las dos lineas
           #include "Speed/Indep/Src/World/Common/WRoadNetwork.cpp"
           #include "Speed/Indep/Src/World/Common/WPathFinder.cpp"
       y anadir arriba del todo
           #define WORLDCONN_H_IMPLICIT_PKT_SERVICE_DTORS

    2) src/Speed/Indep/Src/World/WorldConn.h   (guarda, byte-neutra para los demas)
       en class Pkt_Body_Service (linea 134) y en class Pkt_Effect_Service (237):
           #ifndef WORLDCONN_H_IMPLICIT_PKT_SERVICE_DTORS
               ~Pkt_XXX_Service() override {}
           #endif

    3) python scripts/lcfix.py zWorld2      <-- OBLIGATORIO, 20 lineas de keep.lst
       (2212, 2214, 2216, 2218, 2220, 2222, 2224, 2226, 2228, 2230, 2232, 2234,
        2236, 2238, 2240, 2242, 3268, 3270, 3272, 3274)

**Control de no-regresion**: `linkdelta zWorld2` tiene que quedar en
`.text +0  bss+64` (el `bss+64` lo cubre el paquete P3-A de `r62-p3.md`, no es
mio), `rangos` en **85.458**, `ltorder` en **8 bloques**, y `fncmp` en **3 de
357** --`HolePunchAvoidables` (4 insn), `InitAtSegment` (12 insn) y
`__static_initialization_and_destruction_0` (65 filas, todas `r25`/`r31`)--.
Si sale cualquier otra cosa, revertir los tres pasos.

### 7.3 `Render/RenderConn.h` -- el mismo dtor sintetico, para zWorld

Sin medir (no toco la cabecera). `Pkt_VehicleFragment_Service` (52 B) tiene el
destructor DECLARADO en `RenderConn.h:325` y el objetivo lo emite DELANTE de su
`ConnectionClass/Size/Type`, igual que los de `WorldConn.h`. La receta es la
misma: guarda `RENDERCONN_H_IMPLICIT_PKT_SERVICE_DTOR` y `#define` en
`zWorld.cpp`. **Quien lo mida que compruebe antes `zMain` y `zPhysics`**, que
tambien incluyen esa cabecera.

### 7.4 `lcfix` -- lo que dejo pendiente

* **zWorld: 0 pendientes. zWorld2: 0 pendientes.**
* **zEagl4Anim: 1 pendiente, mia, y NO es venenosa.**
  `keep.lst:2807  zEagl4Anim.o:$LC78 -> zEagl4Anim.o:$LC77` (la cadena `"C0"`).
  Medido con una copia privada corregida: `rangos` da **82.242 en los dos
  casos**, con el mismo reparto por seccion. Las entradas `zEagl4Anim.o:` son
  de este objeto y de ningun otro, asi que **no envenena a nadie mas**.

**Cero propuestas** para `configure.py`, `splits.txt` y `symbols.txt`.

---

## 8. Regresiones comprobadas, no supuestas

    build_direct x3, las tres iguales   zEagl4Anim 174482e0...  zWorld 2b35b5c0...  zWorld2 3df7abca...
    fncmp     zEagl4Anim  2 de 318   (las MISMAS dos: Initialize +4 B, EvalState 18 insn)
    fncmp     zWorld      4 de 582   (las MISMAS cuatro)
    fncmp     zWorld2     2 de 357   (las MISMAS dos)
    vtord     zEagl4Anim  0 de 31    (comprobado DESPUES DE CADA UNO de los 10 pasos)
    linkdelta zEagl4Anim  .text +4 IGUAL | zWorld .text +0 IGUAL | zWorld2 .text +0 bss+64
    checksplits                      0 solapes, 0 rangos que corten un simbolo, LIMPIO
    gapchk    zEagl4Anim  total 0    zWorld2 total 0    zWorld total 5 (los 20 B PREVIOS de la r61/r62)
    keepchk / keepchk2               ni una entrada rota de mis tres unidades
    lcfix --check                    zWorld 0, zWorld2 0, zEagl4Anim 1 (no venenosa, medida)
    UTF-8                            los ficheros tocados decodifican UTF-8 y tienen CERO bytes >127
    finales de linea                 restaurado CRLF en los cuatro que lo tenian y yo habia pasado a LF
                                     (DeltaChan.cpp, DeltaChan.h, RawStateChan.cpp, zWorld2.cpp);
                                     sha1 de los .o IDENTICO antes y despues de restaurarlo
    cabeceras compartidas            NINGUNA tocada; DeltaChan.h y CompoundChannel.h comprobadas
                                     privadas con grep -rln sobre todo src/

**Aviso de `build_direct`**: la PRIMERA compilacion de `zWorld` de esta ventana
dio `6a71370db2a65bef2cf6937b52eebdc41d0b35cd` y las tres del cierre dan
`2b35b5c0b5a0994e6af25433d1e48126397fcda8` --que es exactamente el sello que
documento la r62-- sin que yo tocara un solo fichero de zWorld. Las medidas no
cambian (`rangos` da 18.688 con los dos), pero es una instancia mas de
`nfsmw-build-direct-miente`: **sella siempre TRES veces seguidas, y al final**.

---

## 9. Sorpresas

1. **`textorder` y `ltorder` pueden ir en direcciones contrarias durante 10
   pasos seguidos.** `textorder` dijo "251 descolocadas" al principio y al final;
   `ltorder` dijo 27 bloques y 2. En una unidad que emite simbolos que el enlace
   tira, **`textorder` no sirve para decidir**.
2. **Un cuerpo DENTRO de la clase no es lo mismo que `inline`**: GCC 2.9 aplaza
   el primero al final de la clase y pierde inlines que el segundo conserva. Las
   cuatro funciones de `FnCycle` se quedan en un tercio (2.2).
3. **La cabecera y el `.cpp` son dos palancas, y la respuesta a "el swap rompe
   `vtord`" es partirlo en dos, no descartarlo** (3). Cuesta entre 0 y 43 B.
4. **Un `rodata-168` puede ser `keep.lst`, no codigo.** El `.o` medido seccion a
   seccion lo desmonta en un minuto, y ahi habia una veda de tres rondas (5.1).
5. **"Cambia el orden de los constructores" era un `r25`/`r31`.** Merece la
   pena pasar `fndiff` antes de escribir la causa de una veda (5.4).
6. **La ultima funcion que le falta a una unidad puede valer 8 B y estar en una
   cabecera compartida.** Y se puede medir sin tocarla: sombreado con control de
   sintaxis, mas un barrido de los objetos EXTRAIDOS para ver quien la definia
   en el ORIGINAL (4).
