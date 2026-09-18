# r63-cam -- zCamera: 30.671 -> 19.529 B de DOL

**Agente `cam`. Unidad en exclusiva: `zCamera`. Nada commiteado.**

| medida | ANTES (r62) | DESPUES (r63) |
|---|---:|---:|
| `dolwhere.py zCamera` | **30.671 B** | **19.529 B** (`-11.142`) |
| `textorder` descolocadas | 144 de 453 | **124 de 453** |
| `textorder` saltos de delta | 50 | **31** |
| `.data`: simbolos en su offset exacto | 2 de 56 | **56 de 56** |
| `fncmp` funciones abiertas | 3 (8.464 B) | 3 (8.464 B) -- sin cambio |
| `linkdelta` | `.text +0 resto IGUAL` | `.text +0 resto IGUAL` |
| `lcfix --check zCamera` | al dia | **al dia (0 pendientes)** |

`.o` sellado: **`d3d8c792a449e2cec3342ca24aa663c8afffa98f`**, TRES compilaciones
seguidas con el mismo sello (la base de la ronda era
`094bd7d95f993d504e7f13baec9097964f142720`). AVISO para el que selle: los
cflags llevan `-g -gdwarf+`, asi que **un comentario dentro de un `.cpp` de la
unidad cambia el sha1 del `.o`** (mueve los numeros de linea del DWARF) sin
mover ni un byte del DOL -- comprobado: el mismo `dolwhere` da 19.529 B antes y
despues. Un comentario en el SourceList no lo cambia, porque el DWARF de cada
funcion apunta a su fichero incluido.

---

## LO PRIMERO: UNA LINEA DE `keep.lst`, Y NO ES OPCIONAL

```
config/GOWE69/keep.lst   (insertar justo debajo de la linea 162,
                          `zCamera.o:gap_06_80416538_data`)

zCamera.o:gap_06_80416558_data
```

Es un simbolo NUEVO (4 B de `.data`, ceros) que ahora emite
`src/Speed/Indep/Src/Camera/ChaseCamAI.cpp`. Sin esa linea
`-strip-unused-data` se lo lleva y **todo el `.data` de zCamera a partir de
0x80416558 queda desplazado 4 B**. Medido, con el mismo objeto:

| enlace | DOL distinto |
|---|---:|
| con la linea propuesta | **19.529 B** |
| con el `keep.lst` de hoy | 25.359 B |

Sigue siendo mejor que los 30.671 de partida en los dos casos, pero **sin la
linea se tiran 5.830 B de los 11.142 ganados**. No crea ningun `pad_`, no toca
`splits.txt`, `symbols.txt` ni `configure.py`.

---

## 1. Seis reordenaciones de funciones dentro de su `.cpp`: -8.160 B

El tramo de **codigo normal** (todo lo anterior al bloque diferido de
`finish_file`) tenia seis saltos de delta, y los seis eran permutaciones de
bloques ADYACENTES: el objetivo define las mismas funciones en otro orden
dentro del mismo fichero. Se arreglan moviendo el cuerpo, sin tocar una
instruccion (`.text` sigue midiendo 135.960 B exactos despues de las seis).

| fichero | lo que habia | lo que pide el objetivo |
|---|---|---|
| `Actions/CDActionDrive.cpp` | MessageJumpCut, GetTrafficBasis, Update | **Update, GetTrafficBasis, MessageJumpCut** |
| `Actions/CDActionShowcase.cpp` | IsRightSide (l.19), ..., Construct | **Construct, IsRightSide**, ctor |
| `Actions/CDActionDebugWatchCar.cpp` | Reset, Update, GetTrafficBasis, GetSimable, ReleaseTarget, AquireTarget | **Reset, GetSimable, ReleaseTarget, AquireTarget, Update, GetTrafficBasis** |
| `Movers/TrackCar.cpp` | IsAnyCopNear, IsBeingPursued, FixWorldHeight, ctor, dtor | **ctor, dtor**, IsAnyCopNear, IsBeingPursued, FixWorldHeight |
| `Movers/TrackCop.cpp` | CrossXY, Bezier::Bezier, Bezier::GetPoint | Bezier::Bezier, Bezier::GetPoint, **CrossXY** |
| `ICE/ICEData.cpp` | GetParameter, GetKeyNumber | **GetKeyNumber, GetParameter** |

Marcha por paso (`textorder` saltos / descolocadas):
50/144 -> 46/141 -> 43/139 -> 40/134 -> 37/129 -> 34/126 -> **31/124**.
`dolwhere` 30.671 -> **22.511 B**.

En `TrackCar.cpp` el ctor/dtor se suben **detras** de los seis arrays
`static const float` (`Tweak_JumpCam*`, `TrackCar*Offset*`), no delante: esos
arrays son `.rodata` y su sitio ya casaba.

## 2. El `.data` entero, colocado al byte: -2.982 B mas

`secfull.py .data` da hoy los **56 simbolos comunes en el offset exacto del
objetivo**, de 0x000 a 0xC40, y las dos unicas filas distintas son nombres de
simbolos LOCALES (`cameralink` contra `cameralink.3469`, `old_pov` contra
`old_pov.29797`), que al enlazador le dan igual.

Antes, los nueve bloques `asm()` de huecos (r59) estaban TODOS al final del
SourceList "para no desplazar ningun simbolo nuestro ya colocado", y el efecto
era el contrario: dejaban 2.495 B de `.data` mal y arrastraban ~490 B mas de
mitades `@l` en OTRAS unidades (`IsPlayerCameraSelectable`, `_._11EMomentStrm`,
`SyncRoadblock`, `Update__16IVisualTreatment`... unos 50 sitios de 2 B).

Lo que se ha hecho, y **el metodo es transportable a cualquier unidad**:

1. Sacar la lista de offsets del `.data` del objeto EXTRAIDO y la nuestra
   (`scratchpad/cam63/secfull.py .data`, 20 lineas).
2. Para cada hueco, partir el `asm()` unico y ponerlo **en el punto de parseo
   que le da su direccion**, dentro del `.cpp` que corresponde. Un `asm()` de
   fichero se emite donde se parsea, igual que una variable inicializada.
3. Para cada variable descolocada, **moverla al fichero que el objetivo dice**.

Movimientos de variable que exigio el objetivo (todos dentro de la MISMA
unidad de traduccion, cero cambio de codigo):

* `CameraDebugWatchCar`, `Tweak_EnableICEAuthoring`, `Tweak_ForceICEReplay`:
  de `Actions/CDActionDebugWatchCar.cpp` y `Actions/CDActionIce.cpp` a
  **`CameraMover.cpp`**. El objetivo los pone en 0x8041652C/30/34, o sea
  DELANTE de `TheAvoidables` (`CameraAI.cpp`, 0x80416550): tenian que estar en
  un fichero parseado antes que `CameraAI.cpp`, y el unico que hay es
  `CameraMover.cpp`.
* `gCamCloseToRoadBlock`: de `Movers/TrackCar.cpp` a
  **`Actions/CDActionDrive.cpp`**, entre `gGameBreakerCamera` (0x80416564) y
  `old_pov` (0x8041656C).
* `cameralink`: de estatica de FUNCION (dentro de `Camera::SetCameraMatrix`) a
  **estatica de fichero** en `Camera.cpp`, justo detras de
  `Camera::StopUpdating`. El objetivo la tiene en 0x8041648C, el SEGUNDO hueco
  de la seccion; una estatica local se emite en el punto de parseo de su
  funcion y desde alli ese sitio es inalcanzable. El simbolo es LOCAL en los
  dos casos, asi que el nombre no cuenta -- con `old_pov` el proyecto ya hizo
  la jugada contraria.
* Los 4 B de 0x80416558 caen entre el prototipo de `UTL::COM::Factory`
  (`CameraAI.cpp`) y `kCinematicMomementSeconds` (`CDActionDrive.cpp`). Entre
  esos dos `#include` **solo esta `ChaseCamAI.cpp`, que tiene 0 bytes**: son
  datos del fichero que no esta escrito. Andamio + la linea de `keep.lst`.

Dos detalles que costaria volver a descubrir:

* `gap_06_80417085_data` va en 0x80417085, **sin alinear**, pegado a `gOverlay`
  (1 B). Nada de `.balign 4` ahi, o se pierden 3 B y la seccion baila. Y mide
  **67 B (0x43)**, no los 68 del andamio viejo: `.4byte` x16 + `.byte 0,0,0`.
* `lbl_804164C8` (0x3ECCCCCD) es `kJRCaffeineRate`, la `static const float` sin
  usar de `Camera.cpp:17` que GCC no llega a emitir.

## 3. El orden de vtables: MEDIDO, y NEGATIVO EN BYTES (revertido)

El `.rodata` de zCamera tiene **49 de sus 54 simbolos con nombre descolocados**,
y las vtables son la mayoria. El objetivo las emite en el **inverso del orden de
completado de clase**, y ahi los OCHO movers salen TODOS detras de las siete
acciones: en el original se completaban ANTES. Orden que pide (inverso de su
`_vt`): `CameraMover, DebugWorld, RearView, TrackCar, TrackCop, SelectCar,
Cubic, Showcase, IAttachable, Sim::Collision::IListener, ICEMover,
ITrafficCenter`.

Se probo metiendo esos `#include` en `zCamera.cpp` detras de `CameraMover.cpp`:

| variante | `.rodata` descolocadas | saltos | DOL |
|---|---:|---:|---:|
| base (tras el punto 1) | 49 de 54 | 31 | **22.511 B** |
| + los 7 movers | 45 de 54 | 27 | 22.570 B (`+59`) |
| + IAttachable/IListener/ITrafficCenter | 45 de 54 | 26 | 22.567 B (`+56`) |

**El bloque de movers SE JUNTA y queda contiguo como en el objetivo, y el DOL
empeora.** La causa esta medida: de los 7.292 B que difieren en `.rodata`,
~3.500 son el **pool de cadenas que va DELANTE** de las vtables
(`$LC57` +0x5C4 366 B, `$LC316` +0x741 3.127 B, `$LC1011` +0x1384 452 B --
"Body_Open", "Pkt_Body_Ser", "Attrib::Gen::engine", "pursuitescalation"...).
Mientras ese pool este permutado, mover las vtables solo cambia de sitio el
mismo error. **Orden de ataque para la r64: primero el pool de cadenas, DESPUES
las vtables.** Revertido; el analisis queda escrito en `zCamera.cpp`.

Dos topes duros que ahorran medio ensayo al siguiente:

* `IAttachable` y `Sim::Collision::IListener` **no se mueven por aqui**: entran
  ya por el grafo transitivo de `Camera.cpp`, asi que el `#include` es un no-op
  (guarda de cabecera). Ponerlos donde el objetivo los quiere exige tocar una
  cabecera compartida: **hay que proponerlo, no aplicarlo**.
* `#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"` ahi **rompe el enlace**:
  la `.rodata` del DOL sale 160 B mas corta (0x4C480 contra 0x4C520) y
  `dolwhere` contesta `LAS SECCIONES NO COINCIDEN`. 160 B es exactamente
  `_vt.8ICEMover`. El `.o` SIGUE teniendo el simbolo: lo pierde el enlace.

## 4. `Update__19TrackCarCameraMoverf`: los DOS "z,x,y", y los dos cerrados

El encargo traia como unico hallazgo de forma superviviente "`look_offset` sale
en orden 152,144,148 contra 144,148,152". **Son dos cosas mezcladas**, y separarlas
es lo que abrio el ensayo:

* Lo que dice la ficha de Ghidra (`r63-ghidra-27.md:455`) es que el original
  escribe **`look_offset` en z,x,y**. Eso es el racimo **R3**, y es
  **exactamente el ensayo T2 de la r61** que ya estaba apuntado en el fuente:
  99,616936 %, 15 filas. **Ya era negativo.**
* Los numeros **152/144/148 (0x98/0x90/0x94) no son de `look_offset`** --que es
  una local y va a `0x38/0x3c/0x40(r1)`--: son los tres `stfs` de
  `Look = *CarToFollow->GetGeometryPosition()`, o sea el racimo **R1**. Ese si
  estaba sin probar, porque T2 y T6 permutaban `look_offset` y no tocaban esa
  sentencia.

Probado R1 por primera vez, escribiendo los stores a mano en el orden del
objetivo:

```c
{ const bVector3 *geom_pos = CarToFollow->GetGeometryPosition();
  Look.z = geom_pos->z; Look.x = geom_pos->x; Look.y = geom_pos->y; }
```

* base: **992/992 B, 99,629036 %, 15 filas**
* con la forma: **992/992 B** (el tamano aguanta: no gasta ranura) pero
  **95,979836 % y 31 filas**.
* Y falla su propio observable: el `stfs ..,0x98(r31)` sube a la fila 107 como
  en el objetivo, pero los otros dos se van a las filas **116 y 119** (el
  objetivo los tiene en 109 y 111), y de paso descoloca el racimo del `bScale`
  (123..128) y las tres `lfsx` de `look_offset` (138..147).

**Queda cerrado el eje "forma de fuente" en R1 y en R3**: la permutacion es de
`sched2`, exactamente como decia la r61. Escrito junto a la funcion
(`Movers/TrackCar.cpp`, encima de `Update`) para que `previo.py` lo encuentre.

## 5. `__static_initialization_and_destruction_0`: la veda NO estaba caduca

El informe de vedas r60b la da por CADUCA con la receta del pin
(`_cam_pin(bDegToAng(5.0f))`). **Esa receta ya se probo en la r61 y esta
apuntada en el fuente** (`Movers/Cubic.cpp`, encima de `HydraulicsLookAngle`):
3.604 B clavados pero 98,234184 % y **58 filas**, y el observable falla -- la
fila 549 no pasa a `li r9,0x38e`.

Verificado hoy sobre el arbol de la r63: las **SEIS filas de siempre**
(549/557/558/561/562/564), 3.604/3.604 B, 99,839066 %, y **ni la reordenacion
del `.text` ni la del `.data` de esta ronda mueven ninguna**. Es una prueba mas
de lo que ya decia el fuente: la cerradura es del asignador. El siguiente paso
sigue siendo el que dejo escrito la r61 -- identificar los TRES allocnos
(SYM/LIT/CONST) en el `.greg` de una funcion de 437 pseudos antes de disparar
nada.

---

## Donde estan los 19.529 B que quedan

| region | bytes | que es |
|---|---:|---|
| `.text` (0x800xxxxx) | 12.117 | el bloque diferido de `finish_file`: 31 saltos, casi todos entre 0x1B788 y el final |
| `.rodata` (0x803Dxxxx) | 7.292 | ~3.500 el pool de cadenas + ~3.300 el orden de vtables |
| `.data` (0x8041xxxx) | 39 | residuo |
| otras unidades | 81 | mitades `@l` que apuntan a lo anterior |

Y el techo: aunque las tres funciones abiertas cerraran, zCamera **no puede
promocionar** hasta que caigan esos dos frentes de orden.

**El primer salto del bloque diferido, con su medida**: el objetivo emite
`find<const IVehicle**>` (176 B) DETRAS del racimo de `ConversionUtil` (880 B) y
nosotros DELANTE -- 1.052 B de DOL. Esa `find` no la llama nadie de zCamera: la
llama `IsNoLongerUseful__C10GCharacter` de **zGameplay**. El resto del bloque
son ~28 posiciones alrededor de `_._11IAttachable` (7.573 B, el renglon mas
gordo del informe), que el objetivo emite en la posicion 366 y nosotros en la
338.
