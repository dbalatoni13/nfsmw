# r62 - phys: zMisc `.rodata +1048 -> +616`, 21 direcciones mal corregidas, y por que zPhysics da DOL ROTO con el codigo al 100 %

Tres resultados, los tres medidos y con su control:

1. **zMisc**: `.rodata` del enlace **+1048 -> +616** (-432 B), con `fncmp 0 de 450`,
   `linkdelta .text +0` y `textorder ORDEN PERFECTO` antes y despues.
2. **21 referencias de zMisc apuntaban a OTRA direccion** (misma cadena, otra copia).
   Son 42 instrucciones del DOL, **168 B**, que `fncmp` no puede ver porque normaliza
   los nombres `$LC`. Corregidas. Herramienta nueva: `scripts/reladdr.py`.
3. **zPhysics (36) y zPhysicsBehaviors (30) tienen reubicaciones a otra direccion de
   `.bss`**, estando las dos en `linkdelta ... IGUAL` y zPhysics ademas en `fncmp 0`.
   Es la primera explicacion medida de por que zPhysics da DOL ROTO con el codigo
   entero casando.

```
sellos (TRES compilaciones cada uno, regla 6)
  zMisc              a2a0182c21ad511b62e23b019050c6d287c65f5c  x3  (base 5e7ed0fcaf3289328f8f92268d1cae503100b573)
  zPhysics           8b212bff779a90bcb345e69fffdca0e838bb613e  x3  (base 0ab50993ec35d55cf8949e3b6689f1d240be3059, solo .debug_line)
  zPhysicsBehaviors  65a84fc774bc050b7b4f75212251e96a94211abc  x3  (base 729dc6bd28ef6b061099e8d4ee5b998d5e7496e4, solo .debug_line)

linkdelta  zMisc              rodata+1048 data+32  ->  rodata+616 data+32   (.text +0 en los dos)
           zPhysics           .text +0  IGUAL      (antes y despues)
           zPhysicsBehaviors  .text +4  IGUAL      (antes y despues)
fncmp      zMisc              0 de 450     antes y despues
           zPhysics           0 de 718     antes y despues
           zPhysicsBehaviors  1 de 1120 (UpdateLoaded, 856 B)  antes y despues
textorder  zMisc              0 de 451 descolocadas, ORDEN PERFECTO
reladdr    zMisc              1571 coinciden, DIRECCION DISTINTA 0  (eran 21)
           zPhysics           1377 coinciden, DIRECCION DISTINTA 36
           zPhysicsBehaviors  1826 coinciden, DIRECCION DISTINTA 30
           zBWare / zDynamics (promocionadas, control)   0 y 4 (las 4 son renombres)
trypromo   zMisc              DOL ROTO (23a7ce6707ed)
lcfix --check  1 pendiente, y es de zEagl4Anim.  CERO mias, NINGUNA venenosa.
checksplits LIMPIO (0 solapes, 0 cortes) | gapchk zMisc total 0
keep.lst / splits.txt / symbols.txt / configure.py:  NO TOCADOS.
```

---

## 1. La herramienta que faltaba: `scripts/reladdr.py`

`fncmp` normaliza los nombres `$LCnnn`, asi que **una referencia que apunta a otra
copia de la misma cadena pasa su control**. El `.o` extraido trae las reubicaciones
del objetivo con su simbolo real, y el nuestro tambien: basta comparar, para cada
offset de cada funcion emparejada, el **simbolo contenedor mas delta** en los dos
objetos.

    python scripts/reladdr.py zMisc

Salida: `coinciden N, DIRECCION DISTINTA M, pendientes P`. `pendientes` son los
`$LCnnn` cortos, o sea el pool que `cc1plus` internaliza y que todavia no se ha
sustituido (esos ya se sabe que apuntan mal).

**Control obligatorio y pasa**: `zBWare` (promocionada) da 0; `zDynamics`
(promocionada) da 4, y las cuatro son el mismo simbolo con otro nombre
(`kFloatScaleUp` contra `kFloatScaleUp_8045B100`). Ese es el suelo de falsos
positivos de la herramienta: renombres y los `gcc2_compiled.` que usa dtk cuando no
sabe nombrar el destino.

Barrido de las 29 SourceLists: ademas de las mias dan filas **zCamera 40, zWorld 46,
zEagl4Anim 4, zPlatform 4, zFe 2, zLua 2, zSpeech 2, zTrack 2**. No son mias y no
las he tocado, pero tres tienen pinta de ser reales, no falso positivo, porque
**cambian de seccion**:

```
zWorld  RenderFlaresOnCar__13CarRenderInfo... +0x027a  nue ForceHeadlightsOn+0 [.data]        obj lbl_8040AD04+0 [.rodata]
zWorld  SetMemoryPoolSize__9CarLoaderi        +0x00f2  nue CarLoaderMemoryPoolNumber+0 [.data] obj lbl_8040CAA4+0 [.rodata]
zCamera __static_initialization_and_...       +0x0876  nue HydraulicsLookAngle+0 [.bss]        obj lbl_803D3A80+0 [.rodata]
```

---

## 2. zMisc: las 21 direcciones mal, y de donde salian

| origen | refs | nuestro | objetivo | contenido |
|---|---:|---|---|---|
| `BWARE_PREFIX_STL` | 8 | `.rodata+0x058` | `.rodata+0x97C` | `"STL"` |
| `ZMISC_POOL(0xC14)` en `GameFlow.cpp:813` | 1 | `.rodata+0xC14` | `.rodata+0xFB8` | `"%s.bin"` |
| 12 `ZMISC_POOL(...)` en `PrintToString` (Timer.cpp) | 12 | `+0x1B1F`, `+0x1B2B`, ... | `+0x1B94`, `+0x1B9C`, ... | `"00.00"`, `"--.--"`, ... |

**El objetivo tiene esas cadenas DOS VECES** en la `.rodata` de zMisc y las
referencia siempre por la segunda copia. La deduplicacion de rondas anteriores
eligio la primera: la `.rodata` sale del mismo tamano, `dupstr` mejora, `fncmp` da 0
y **el DOL queda con 42 instrucciones (168 B) apuntando a otro sitio**. El aviso ya
estaba escrito para otra unidad (`zFoundation.cpp:30`, "el objetivo tiene STL DOS
VECES") y no se habia extrapolado.

Arreglado usando el simbolo propio de cada copia (`extern const char _zmsNNNN[]
asm("$LC2151638028")`, etc.), que es exactamente lo que hace el objetivo.
`reladdr zMisc` pasa de 21 a **0**.

---

## 3. zMisc: la palanca de la `.rodata`, y su suelo

El bloque a mano de `zMisc.cpp` reproduce la `.rodata` del objetivo, **y cc1plus
interna ADEMAS su propia copia de cada constante**. La equivalencia
`$LCnnn -> simbolo del bloque` no hay que adivinarla: sale de las reubicaciones
HA16/LO16 del `.o` extraido, **145 de 145 emparejadas y cero discrepancias**. Con
eso la sustitucion es mecanica: `extern const float _zmfNNNN asm("$LC21516...")` en
`zMisc.cpp` (antes de los `#include`) y el literal cambiado en el `.cpp`.

Camino recorrido, con `fncmp 0 de 450` en cada escalon:

| paso | fichero | constantes | `.rodata` del enlace |
|---|---|---:|---:|
| base | | | **+1048** |
| 1 | `Table.cpp` | 18 | +968 |
| 2 | `Timer.cpp` (menos la cadena de `if`) | 22 y 1 cadena | +904 |
| 3 | las 21 direcciones mal (no cambia el tamano) | 21 refs | +904 |
| 4 | `GameFlow.cpp`, `Joylog.cpp`, `Main.cpp`: 8 cadenas | 8 | +696 |
| 5 | `Rumble.cpp` | 10 | +656 |
| 6 | `Spline.cpp` (solo las 5 que ceden) | 5 | +632 |
| 7 | `Main.cpp` (5 de 7) | 5 | **+616** |

### El suelo, y por que no llega a cero

Del `.rodata +800` que queda en el objeto:

* **104 B: trece constantes `0x4330000080000000`** (la magica de `int -> float`).
  **No tienen forma de fuente**: las emite el compilador una por cada conversion y
  no hay literal que sustituir. Es un techo duro para esta palanca.
* **254 B: las 22 cadenas de cabecera compartida** (`SoundConn.h`,
  `GRaceDatabase.h`, `uiSMSMessage`). Regla 5: propuesta abajo, no aplicadas.
* **~110 B: los negativos medidos** de la seccion 5.
* El resto son entradas referenciadas desde `.data` y las de `QuickSpline`, que no
  he atacado por falta de ventana.

### El hallazgo que SI llega a cero (probado con una sonda)

Un `asm(".section .rodata ...")` de fichero puesto **ENTRE dos `#include`** de la
lista se emite **DENTRO del pool de cc1plus**, justo en la frontera entre las
constantes del fichero anterior y las del siguiente. Sonda `_r62probe` colocada
antes de `#include ".../Timer.cpp"`:

```
0x1f00 $LC488        <- ultima constante de Table.cpp
0x1f08 _r62probe     <- la sonda
0x1f10 $LC489        <- primera constante de Timer.cpp
```

O sea: **el bloque a mano se puede PARTIR en fragmentos por fichero**. Si cada
fragmento lleva solo lo que cc1plus no regenera, las constantes duplicadas
desaparecen solas Y caen en su direccion, incluidas las trece magicas y las 22
cadenas de cabecera compartida. Es la unica via conocida a `.rodata +0` en zMisc y
no toca ninguna cabecera. Queda escrito en `zMisc.cpp` junto al bloque.

---

## 4. zPhysics y zPhysicsBehaviors: la razon medida del DOL ROTO

Las dos estan en `linkdelta ... IGUAL` (mismo tamano en todas las secciones) y
zPhysics ademas en `fncmp 0 de 718`. Y aun asi:

**zPhysics, 36 reubicaciones (12 usos) a otra direccion de `.bss`:**

```
TypeName__8AIParams           nue .bss@0x00c   obj .bss@0x004
TypeName__15RBComplexParams   nue .bss@0x014   obj .bss@0x00c
TypeName__14RBSimpleParams    nue .bss@0x01c   obj .bss@0x014
TypeName__16SuspensionParams  nue .bss@0x024   obj .bss@0x01c
TypeName__12EngineParams      nue .bss@0x02c   obj .bss@0x024
Construct__8PVehicleG...      nue .bss@0x1e0   obj .bss@0x238
GetTunings__C8PVehicle        nue .bss@0x200   obj .bss@0x258
```

Las cinco primeras estan corridas **una ranura**: en el objetivo el primer `_.tmp_`
de la unidad es el de `AIParams` y en el nuestro hay uno antes.

**zPhysicsBehaviors, 30 reubicaciones (10 usos):**

```
ConnectionClass RenderConn::Pkt_Car_Open       nue .bss@0x0a4  obj .bss@0x004
SType           RenderConn::Pkt_Car_Open       nue .bss@0x0ac  obj .bss@0x00c
ConnectionClass RenderConn::Pkt_Heli_Open      nue .bss@0x0c4  obj .bss@0x024
SType           RenderConn::Pkt_Heli_Open      nue .bss@0x0cc  obj .bss@0x02c
ConnectionClass RenderConn::Pkt_VehicleUpdate  nue .bss@0x0e4  obj .bss@0x044
```

En el objetivo el bloque de `_.tmp_` **empieza** por las de `RenderConn`; en el
nuestro salen 19 ranuras antes. Es **orden de emision**, de la misma familia que las
157 y 59 funciones que `textorder.py` manda mover, pero con un observable nuevo y
barato de medir. **Ni `fncmp` ni `linkdelta` lo ven**: el primero normaliza los
nombres, el segundo solo mira tamanos.

Los dos ficheros llevan el detalle escrito en su SourceList, donde `previo.py` lo
encuentra.

`UpdateLoaded` NO se ha vuelto a tocar: la veda dura de la r61 (la desigualdad
`src_cost <= src_eqv_cost` de `cse.c:7191`, que es falsa para todo pseudo) sigue en
pie y este encargo no ha encontrado nada que la mueva. Y aunque cayera, la unidad no
promociona: `textorder` le pide 59 movimientos y ahora ademas hay 10 usos de `.bss`
en la ranura equivocada.

---

## 5. Negativos medidos (todos escritos junto a su funcion)

1. **`PrepareRealTimestep` (Timer.cpp), 20 B.** Las cinco constantes de la cadena de
   `if` (86400000.0f, 0.0f, 86400.0f, 1.0f, 10000.0f) sustituidas por externs:
   `fncmp 0 -> 1 de 450` (876 B), `f0/f13` cambiados en las filas 88..98,
   `.rodata +888`. Revirtiendo solo 86400.0f sigue rota, ahora en `f12/f13`.
   Revirtiendo las cinco: `fncmp 0 de 450`, `.rodata +904`. Causa: con la constante
   en un extern deja de ser rematerializable y cambia de prioridad frente a la
   variable comparada.
2. **`Spline.cpp` y `Point.cpp`, 28 B.** `GetdVal` (3.0f), `GetddVal` (6.0f),
   `tCubic1D::Update` (0.0f, 1e-05f, 1.0f) y `cPoint::SplineSeek` (0.0f, 1.0f):
   `fncmp 0 -> 5 de 450` (772 B) y **`linkdelta .text +0 -> +48`**
   (`Update` 340/324, `SplineSeek` 308/288 y 100/88).
3. **`const float` local, 12 B.** `minumum_time_step = 0.25f` en `main` y
   `fe_dt_fix = 0.01f` en `Main_AnimateFrame`: el `const` local **obliga a
   materializar la carga en la DECLARACION** mientras que el literal la materializa
   en el uso, asi que los dos `lfs` cambian de sitio (`main` 2 de 450) y la funcion
   crece (`Main_AnimateFrame` 316/312).
4. **`tShaker::Reset` (Rumble.cpp), 24 B, negativo ESTRUCTURAL.** El cuerpo se emite
   dos veces y **cada copia usa direcciones distintas del objetivo**: la que se mete
   inline en `ResetCameraShakers` va a `+0x130C/+0x1310/+0x1314` y la copia fuera de
   linea `Reset__7tShaker` a `lbl_803F662C/6630/6634`. Un solo texto de fuente no
   puede dar las dos.
5. **`deadstr.py zMisc` sigue con el signo al reves** (confirmado el negativo 2 de la
   r61): la unidad emite de mas, meter cadenas en `keep.lst` la aleja.

---

## 6. Propuestas (no las aplico)

1. **`scripts/reladdr.py`**: lo he dejado en `scripts/` porque los comentarios de
   `zPhysics.cpp` y `zPhysicsBehaviors.cpp` lo citan y el scratchpad se borra. Si el
   jefe lo quiere en otro sitio, hay que cambiar esas dos citas y la de este informe.
2. **Encargo para la proxima tanda, con la sonda ya hecha**: partir el bloque a mano
   de `zMisc.cpp` en fragmentos por `#include`. Es la unica via a `.rodata +0` y no
   toca ninguna cabecera compartida.
3. **Barrido de `reladdr.py` en las unidades de otros agentes**: zCamera (40 filas) y
   zWorld (46, dos de ellas cambiando de seccion) son las que mas prometen. Cuesta
   segundos por unidad y no enlaza nada.
4. **Las 22 cadenas de cabecera compartida de zMisc (254 B)** siguen pendientes, con
   el mismo patron que ya usan `zFe.cpp:134-156` y `zAnim.cpp:64`. Toca a zWorld,
   zSim, zFe y zAnim a la vez. Si sale la propuesta 2, **esta deja de hacer falta**.
5. **`.data +32` de zMisc no es solo tamano**: la secuencia de simbolos coincide en
   126 de 173 y hay movimientos reales (el bloque de Hermes, `RenderTimingStart`, los
   `*ModelPackFilename`). No es trabajo de ventana.

---

## 7. Sorpresas

1. **Una unidad puede estar en `fncmp 0`, `linkdelta IGUAL` y `textorder ORDEN
   PERFECTO` y aun asi tener decenas de instrucciones apuntando a otra direccion.**
   Ninguna de las tres herramientas de cabecera del proyecto mira eso.
2. **Deduplicar una cadena puede empeorar el DOL sin que ninguna medida lo diga.**
   `dupstr` mejora, `linkdelta` mejora, `fncmp` no se entera. La unica que lo ve es
   comparar las reubicaciones contra el `.o` extraido.
3. **El pool de cc1plus no es un bloque al final**: se emite intercalado con los
   `asm()` de fichero, en orden de fuente. Eso abre la reconstruccion exacta de la
   `.rodata` de cualquier unidad que escriba su pool a mano.
4. **Hay constantes sin forma de fuente.** Las trece `0x4330000080000000` de
   `int -> float` de zMisc son 104 B que ninguna sustitucion puede quitar.
5. **Sustituir un literal por un extern del pool no es una operacion neutra de
   codigo**, aunque salgan las mismas instrucciones: el extern no es
   rematerializable y eso cambia el reparto de registros. Cuatro de mis cinco
   negativos son de esa familia.
