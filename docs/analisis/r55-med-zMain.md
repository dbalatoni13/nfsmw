# r55 · medida de `zMain`: **ORDEN PURO**, 7.077 B, y el `CAMBIO DE DUENO` de `movidos` es FALSO

Unidad: `Speed/Indep/SourceLists/zMain` (159.776 B de codigo).
Arbol: `HEAD = 5e080ef3`, `zMain.cpp` sin modificar (lleva el andamio de `r54-forense1`,
commit `e3daccc2`). Reconstruida con `python scripts/build_direct.py Speed/Indep/SourceLists/zMain`
--**a la primera, `1 ok, 0 fallidas`**--. Ni un fichero del arbol tocado, ni un commit.

El enlace base reproduce `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, asi que toda cifra de
aqui esta tomada sobre una base sana.

---

## 1. La pregunta principal: `reorden.py` -> ORDEN, no CONTENIDO

    python scripts/reorden.py zMain

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .data          43      811        0          0       13
    .rodata       852     2118       12          0      661
    .text        1378    39617      371          0        0

    CONTENIDO 0: no queda nada que escribir.

**Cero palabras de contenido en las tres secciones.** Y las diferencias *crudas* han bajado
respecto a la medida de `forense2` sobre el `HEAD` previo (`.text` 809 -> **371**, `.rodata`
50 -> **12**): el andamio de la r54 no solo recoloco el destructor, tambien apago dos tercios
de la sombra de reubicacion.

## 2. El residuo, medido byte a byte (no `dolwhere`)

Enlazando dos veces y comparando **byte a byte por direccion** (no rangos):

| | pre-r54 (`forense2`) | **ahora** |
|---|---:|---:|
| bytes distintos del DOL | 15.369 | **7.077** |
| palabras distintas | 4.359 | **2.084** |
| simbolos afectados | -- | **297** |

La particion completa, en bloques (hueco > 256 B). Suman los 7.077 exactos:

| region | bytes | % | que es |
|---|---:|---:|---|
| `.text` `801F20E4..801F35B3` | **4.827** | **68,2** | **R2**: 40 simbolos `_IHandle__*` / `TypeName__*` / `HandleMessage_LuaBinding__*` / `_GetKind__*` / `_._Schedule_*` entrelazados de otra forma |
| `.rodata` `803EF930..803EFF75` | **1.536** | **21,7** | **R3**: el literal muerto `"EAGL4::SymbolEntry"` (+4 B) y el `0.0f` de pool que falta (-4 B) |
| `.text` `801FA930..801FAA8B` | 223 | 3,2 | el ciclo de 3 de `Vector<ActionQueue*,16>` |
| `.rodata` `803F3C6E`, `803F4786`, `803F39BF`, `803F36FD`, `803F2B3E` | 127 | 1,8 | cinco vtables permutadas por parejas |
| `.rodata` `803F22E0..803F2364` | 81 | 1,1 | **R4**: `"EventSequencerSystems"` y `"Scheduler"` |
| sombra dispersa (~125 bloques de 1-13 B) | 283 | 4,0 | mitades bajas `@l`/`@ha` y `bl` de las unidades VECINAS |
| | **7.077** | | |

R2 y R3 son el **90 %**. Son las mismas dos que dejo la r54; **no ha aparecido ninguna causa
nueva** y R1 (el destructor) ha desaparecido del mapa.

## 3. `movidos.py`: 52 permutaciones reales, y una seccion M4 que hay que descartar

    zMain: 39558 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
       52 cambian de direccion, pero 0 es ARRASTRE (tres terminos: donde
       empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
       cambia de dueno).
       .bss: base+0 delta+552, .rodata: base+0 delta+136, .text: base+0 delta+15408

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .bss       nuestro .o    +552, seccion enlazada      +0  ->     -552 de otro
          .rodata    nuestro .o    +136, seccion enlazada      +0  ->     -136 de otro
          .text      nuestro .o  +15408, seccion enlazada      +0  ->   -15408 de otro

    **52 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

    seccion       movidos      bytes
    .text              43      5,592
    .rodata             9        480

**Los 16.096 B de esa seccion son un FALSO POSITIVO, y esta probado en dos pasos.**

`robado = enlazado - delta` no distingue "le he quitado bulto a otro objeto" de "el enlazador
ha tirado el mio". Y `zMain` emite mucho codigo muerto:

1. **Censo.** Nuestro `zMain.o` define 561 globales que el `.o` extraido no. De ellas:
   * **146** las define tambien otro objeto del enlace (14.344 B de `.text` + 80 B de
     `.rodata`) -- los candidatos a M4: `reserve__..._STL::vector<...>`, `find__H2Z...`,
     `push_back__...`. **Las 146 son `WEAK`, ninguna `GLOBAL`.**
   * **415** no las define nadie mas (27.428 B de `.text`, 32 B de `.bss`, 16 B de `.data`):
     son los `E*_MakeEvent_LuaBinding__FP9lua_State` y compania, codigo muerto puro.
2. **Prueba decisiva.** De los 146 candidatos, **121 estan en el enlace base y CERO cambian de
   direccion** al meter nuestro `zMain`. Ninguno cambia de dueno: el enlazador se queda con la
   copia del objeto que ya la daba y estripa la nuestra.

Es exactamente lo que dice el docstring de `linkdelta.py` sobre `zLua` (`.text` del objeto
+18.908 B, `.text` enlazado identico). Con la formula actual, **cualquier unidad con codigo
muerto declara un cambio de dueno que no existe**, y once de las doce del bloque A tienen
codigo muerto. Coincide con el censo de `r54-control`, que daba **`dueño = 0`** para `zMain`
leyendo los objetos: `movidos` contradice a `control`, y quien tiene razon es `control`.

**El cambio de dueno REAL de `zMain` es 0 B.**

## 4. Simbolo dominante: **NO lo hay en `.text`.** El residuo es DIFUSO

Es lo contrario de la r54, donde una sola funcion valia el 55 %.

* **Por atribucion de bytes**: 297 simbolos afectados; el mayor es `lbl_803EF984` con **277 B**
  = 3,9 % del total. Los 16 `HandleMessage_LuaBinding__M*` (252 B cada uno) suman ~3.600 B,
  pero son **victimas**, no causa.
* **Por contrafactual aritmetico**: reconstrui la ventana R2 con las direcciones reales
  (40 simbolos, **sin un solo hueco de alineacion**: el modelo reproduce las dos disposiciones
  con **0/40 de error**) y probe a mover **un solo simbolo** a su ranura del objetivo:

      +544 B   ( 12 B, idx 21 -> 7)   _IHandle__13INISCarEngine
      +544 B   ( 12 B, idx 24 -> 2)   _IHandle__10IResetable
      +504 B   ( 12 B, idx 39 -> 15)  _IHandle__13ISceneryModel
      +268 B   (268 B, idx 12 -> 13)  _._Q214EventSequencer7IEngine
      +252 B   (252 B, idx  6 -> 9)   HandleMessage_LuaBinding__16MAudioReflection
       +12 B   ( 12 B, idx  8 -> 0)   _IHandle__7IPlayer

  **La mejor palanca de un solo simbolo vale 544 B de 5.328: el 10 % de R2 y el 7,7 % del
  total.** Un greedy necesita **once movimientos** para llegar a 0, y coincide con
  `permorden`, que da **11 DESPLAZADAS** de 1.380.

Los desplazamientos de los nueve causantes (todos de 12 o 100 B) son:

    +4016  12 B  _IHandle__13ISceneryModel
    +3400 100 B  TypeName__15SmackableParams
    +3000  12 B  _IHandle__10IResetable
    +2280  12 B  _IHandle__13INISCarEngine
    +1716 100 B  TypeName__15ExplosionParams
    -1408  12 B  _IHandle__9IFeedback
     -744  12 B  _IHandle__Q217CollisionGeometry10IBoundable
     +720  12 B  _IHandle__7IPlayer
     +720  12 B  _IHandle__Q23Sim13IStateManager
    (mas el orden INTERNO de los cuatro `Pkt_Body_Send`: el objetivo saca `_._` el PRIMERO)

y el arrastre que producen se descompone exacto: `-212 = -100 -100 -12`,
`-136 = -100 -12 -12 -12`, `-76 = -52 -12 -12`. No queda nada sin explicar.

**Lo unico con arrastre concentrado esta en `.rodata`**: el literal muerto
`"EAGL4::SymbolEntry"` (19 B, sobreviven 4 al estripado `size & ~7`) corre el pool de cadenas
+4 B durante 1.606 B y explica **1.536 B, el 21,7 %**. Verificado en los bytes del enlace, sin
cambios respecto a la r54:

    OBJ 803EF930  4D 41 75 64 69 6F 52 65 66 6C 65 63 74 69 6F 6E |MAudioReflection|
    NUE 803EF930  72 79 00 00 4D 41 75 64 69 6F 52 65 66 6C 65 63 |ry..MAudioReflec|

    OBJ 803EFF70  42 C8 00 00 00 00 00 00 43 30 00 00 80 00 00 00 |B.......C0......|
    NUE 803EFF70  73 74 00 00 42 C8 00 00 43 30 00 00 80 00 00 00 |st..B...C0......|

El `0.0f` que falta en `803EFF74` cierra el desfase. **El par es atomico**: quitar solo el
literal deja la `.rodata` 4 B corta y desplaza los 18,7 kB siguientes.

## 5. El resto de las medidas

    linkdelta zMain    .text +0   resto IGUAL   (las nueve secciones)

    permorden zMain    1369 de 1380 en su sitio; DESPLAZADAS 11
                       ciclos no triviales: 12, 10, 7, 5, 4, 3, 2
                       (el ciclo de 98 de la r54 ha DESAPARECIDO; lo sustituye el de 3
                        de `Vector<ActionQueue*,16>`)

    trypromo Speed/Indep/SourceLists/zMain    DOL ROTO (e889a6864efa)

`e889a6864efa` es el **mismo hash** que dejo `forense1`: la unidad esta exactamente donde la
r54 la dejo, y mi medida es reproducible.

## 6. El siguiente paso concreto

**El veredicto es `ORDEN_PURO` y el residuo de `.text` es difuso, asi que no hay una sola
sentencia que cierre `zMain`.** Lo que hay es una lista de nueve puntos de parseo, cada uno
medible solo. El mas barato y el que tiene la evidencia mas limpia:

> **Meter `#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"` en
> `src/Speed/Indep/SourceLists/zMain.cpp` justo DESPUES de la linea 118
> (`#include ".../World/WorldConn.h"`) y antes de la 136
> (`#include ".../Generated/Events/EAIEngineRev.cpp"`).**

**Por que ahi, con la evidencia del objetivo.** En el `.text` del enlace base:

    801F21A8   32 B  Type__Q29WorldConn13Pkt_Body_Send         <- ultimo de WorldConn.h
    801F21C8   12 B  _IHandle__13INISCarEngine                 <- LA RANURA
    801F21D4  252 B  HandleMessage_LuaBinding__12MAIEngineRev  <- primero de EAIEngineRev.cpp

y en el nuestro `_IHandle__13INISCarEngine` cae en `801F2AB0`, **+2.280 B**, entre
`HandleMessage_LuaBinding__15MGamePlayMoment` y `..._20MNotifyMovieFinished`. O sea: el
objetivo parsea `INISCarEngine` entre `WorldConn.h` y `EAIEngineRev.cpp`, y nosotros lo
parseamos mucho despues, cuando `ENISBrakelock.cpp` arrastra `INISCarControl.h` por primera
vez (es uno de los diez `Generated/Events/ENIS*.cpp` que lo incluyen).

`_IHandle__X` **no** es plantilla: es un `inline` de clase (`DECL_INTERFACE`, `UCOM.h:109`),
asi que su indice en `saved_inlines` es el `finish_function` del cierre de la clase, y para
moverlo basta con adelantar el `#include` --que es lo que ya hace `EVENTSEQ_FWD_IENGINE`
(`zMain.cpp:27`) con `EventSequencer::IEngine`, cuyos dos simbolos salen bien--. Es el
mecanismo 1 de la r54 **por la otra puerta**: la plantilla pide un uso dentro de una funcion,
el inline de clase se conforma con el punto del parseo.

**Vale 544 B** (10 % de R2, 7,7 % de los 7.077) por el contrafactual aritmetico de §4.

Tres comprobaciones ya hechas para que no cueste una tarde:

* `INISCarControl.h` **no tiene ni un literal de cadena** (sus dos comillas son los dos
  `#include`), asi que **no renumera ningun `$LC`** y no hace falta `lcfix` ni tocar `keep.lst`.
* El otro simbolo que emite la cabecera, `_IHandle__14INISCarControl`, esta en `80050110`
  **en los dos enlaces**: lo suministra un objeto muy anterior y nuestra copia (`WEAK`) se
  estripa. Adelantar el include no lo puede mover.
* Ni `INISCarControl` ni `INISCarEngine` emiten destructor (`_._13INISCarEngine` no existe en
  ninguno de los dos enlaces): no hay efecto colateral en `.text`.

**Segundo por valor, y es el unico con arrastre concentrado**: el paquete atomico de R3
(1.536 B, 21,7 %) --guarda selectiva en `eagl4supportdef.h:42` para apagar SOLO
`HAND_POOL_TAG("EAGL4::SymbolEntry")`, **mas** el `0.0f` de pool de `803EFF74`, **mas**
`python scripts/lcfix.py zMain` (62 lineas de `keep.lst`)--. Es mas rentable por byte pero
**toca cabecera compartida y `keep.lst`**, o sea que no cabe en una ronda de medida y no es
paralelizable con otros agentes. `HAND_POOL_TAG` a secas es regresion medida (`.rodata -512`
y `fncmp` 0 -> 1); no lo intenteis.

Y **lo que NO hay que hacer**: perseguir el `CAMBIO DE DUENO` de §3. No existe.

## 7. Sorpresas

1. **`movidos.py` inventa 16.096 B de cambio de dueno en `zMain`.** `robado = enlazado - delta`
   confunde el estripado del enlazador con el robo de simbolos. Probado con las direcciones:
   de los 146 candidatos, **0 se mueven**. A la formula le falta un tercer dato --si nuestra
   definicion *gana* el turno-- y se saca sin enlazar: si es `WEAK` y otro objeto anterior la
   da, no hay cambio de dueno. Afecta a toda unidad con codigo muerto, o sea a casi todo el
   bloque A: **antes de repartir la r55 por la columna `dueño`, verificad que los candidatos
   se mueven de verdad.**
2. **El residuo de `zMain` es 7.077 B, no 7.961.** `dolwhere` daba 7.961 porque cuenta rangos.
   La cifra byte a byte es 7.077, y es la comparable con los 15.369 de `forense2`: el andamio
   de la r54 valio un **-54 %**.
3. **`permorden` ya no miente aqui.** Sus 11 desplazadas coinciden exactamente con los 11
   movimientos que necesita el greedy. Lo que enmascaraba en la r54 era el ciclo de 98; sin
   el, la metrica y la realidad coinciden.
4. **`zMain` sigue siendo atipica.** 7.077 B contra una mediana de 564.000 en el bloque A, y
   ahora ademas con `dueño = 0` **probado**, no solo censado.

## 8. Reglas

Nada de `ninja` ni `configure.py`. Solo `build_direct.py` sobre `Speed/Indep/SourceLists/zMain`.
Ni un commit, ni un `git add`, ni una linea de fuente cambiada. `configure.py`,
`config/GOWE69/*`, `splits.txt`, `keep.lst` y las cabeceras compartidas, sin tocar.
Mis sondas estaban en el scratchpad, en un subdirectorio propio, y quedan borradas; ninguna
en `scripts/`.
