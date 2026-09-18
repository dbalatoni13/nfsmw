# r55 — medida de `zGameModes`

Unidad: `Speed/Indep/SourceLists/zGameModes`. Fuente: `src/Speed/Indep/SourceLists/zGameModes.cpp`
(nueve líneas: `UVectorMath.h` + `#include ".../Gamemodes/EmotionManager.cpp"`).
Reconstruida con `build_direct.py` a la primera (`1 ok, 0 fallidas`).

## Veredicto: CONTENIDO, no orden. Y `reorden.py` dice lo contrario porque aquí no mide nada.

`reorden.py zGameModes`:

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .rodata         3        0        0          0        3
    .text           1       11        0          0        0
    CONTENIDO 0: no queda nada que escribir. ... es ORDEN DE EMISION.

**Ese "0" es vacío.** El objetivo tiene 212 palabras de `.rodata` y `reorden` comparó **0**:
sus tres símbolos (`pad_05_803EBB48_rodata`, `lbl_803EBE90`, `lbl_803EBE94`) salen los tres
como *sin par*, porque nuestro objeto no define ninguno de esos nombres. Y en `.text` comparó
11 palabras de las 31 del objetivo. O sea: **de las 243 palabras del objetivo, `reorden`
comparó 11 y concluyó "no queda nada que escribir"**. Ver Sorpresas.

Lo que sí queda por escribir, medido byte a byte: **744 B de `.rodata`**.

## Las secciones, una por una

| sección | objetivo (`obj/`) | nuestro (`src/`) | aporta al enlace | veredicto |
|---|---:|---:|---:|---|
| `.text`   | 124 B | 620 B | **+0** | **EXACTA** |
| `.ctors`  | 4 B | 4 B | +0 | exacta |
| `.data`   | 0 B | 64 B | **+0** | exacta (el enlazador estripa nuestros 64 B) |
| `.bss`    | 8 B | 8 B | +0 | exacta |
| `.rodata` | **848 B** | 232 B | **−744** | **el único defecto** |

- `linkdelta Speed/Indep/SourceLists/zGameModes`: `.text +0   rodata-744`
- `permorden`: objetivo 2 funciones, nuestro 20, **2 de 2 en su sitio, 0 desplazadas**
  (las 18 de más son los cuerpos de `EmotionManager`, que el enlazador estripa).
- `fncmp`: **0 de 2 funciones con el código distinto**; una más sólo con nombres de símbolo
  distintos (`__static_initialization_and_destruction_0`, 80 B, 2 reubicaciones).
- `_GLOBAL_.I.aEmotionalSummaryTypeStrings` cae en **0x8019A240 en los dos enlaces**. El
  `.text` de esta unidad está terminado.
- `trypromo Speed/Indep/SourceLists/zGameModes` -> **DOL ROTO (6e62a075ec00)**.
- `dolwhere` -> `LAS SECCIONES NO COINCIDEN` (rodata 0x4C520 contra 0x4C240 = los 744 B).
- `vtord`: objetivo **0 vtables**, nuestro 1 (`_vt.14EmotionManager`, 24 B). Cuesta 0 porque
  el enlazador se la lleva, pero es de más.

## `movidos.py`: 3 permutaciones, y el M4 es un FALSO POSITIVO

    zGameModes: 40270 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
       11327 cambian de direccion, pero 11324 es ARRASTRE (tres terminos: donde
       empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
       cambia de dueno).
       .bss: base-736 delta+0, .data: base-736 delta+64, .rodata: base+0 delta-616,
       .sbss: base-736 delta+0, .sbss2: base-736 delta+0, .sdata: base-736 delta+0,
       .sdata2: base-736 delta+0, .text: base+0 delta+496

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .data      nuestro .o     +64, seccion enlazada      +0  ->      -64 de otro
          .rodata    nuestro .o    -616, seccion enlazada    -744  ->     -128 de otro
          .text      nuestro .o    +496, seccion enlazada      +0  ->     -496 de otro

    **3 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

    seccion       movidos      bytes
    ?                   3          0

    los desplazamientos mas repetidos:
            -736  3 simbolos

**3 permutaciones reales, 0 bytes, sección desconocida, las tres al arrastre −736.**
Prácticamente cero.

Y el bloque M4 **no es un cambio de dueño**. Censo hecho sobre las tablas de símbolos de los
616 objetos del enlace: de los **22 símbolos globales que definimos de más**, **ninguno lo
define ningún otro objeto**. Son los cuerpos de `EmotionManager` / `EmotionalResponse` /
`EmotionalSummary` (496 B de `.text`), las tres tablas de cadenas (64 B de `.data`) y
`_vt.14EmotionManager` (24 B). Nadie los referencia y **el enlazador los estripa enteros**;
por eso `enlazado − delta` da −496 / −64. Los 688 B de "robo" son **código muerto estripado**,
no bulto quitado a otro objeto. El censo de `control` en la r54 ya daba `dueño = 0` para
zGameModes, y sigue siendo cierto.

(Recordatorio de por qué esto es esperable: `obj/zGameModes.o` está EXTRAÍDO DEL DOL, así que
sólo contiene lo que sobrevivió al enlace original. Que nuestro `.text` y `.data` lleven de
más lo que el enlazador tira no es un defecto.)

## El símbolo dominante: `pad_05_803EBB48_rodata`, 840 B, explica 744 de 744

`splits.txt:121` da a zGameModes `.rodata start:0x803EBB48 end:0x803EBE98` = **848 B**. El
objeto extraído los tiene como `pad_05_803EBB48_rodata` (840 B, local) + `lbl_803EBE90` +
`lbl_803EBE94` (4 B cada uno). Nuestro objeto **no define ninguno de los tres** y sólo aporta
**104 B** a esa ventana. Los 744 B de diferencia son el 100 % del delta de enlace, y el
desplazamiento de −736 que provocan mueve `.data`, `.bss`, `.sbss`, `.sdata` y `.sdata2`
enteras y todo el `.rodata` a partir de 0x803EBBB0. **No hay residuo difuso: es un solo
bloque.**

Contenido del bloque en el DOL original (volcado de la `.rodata` enlazada en 0x803EBB48):

    +0x000  540 B  "GAMECUBE" "d:/mw/speed/indep/bware/inc/bware.hpp" "bad_alloc"
                   "%f,%f,%f" "%f,%f,%f,%f" "STL" + una palabra a 0
                   "Attrib::Attribute" "Attrib::Instance" "Attrib::Definition"
                   "Attrib::Class" "Attrib::Database" "Attrib::TypeDesc"
                   "Attrib::RefSpec" "Attrib::Blob" "Attrib::Gen::effects"
                   "Attrib::TAttrib" "Attrib::Gen::audioimpact"
                   "Attrib::Gen::audioscrape" "Attrib::Gen::simsurface"
                   "WorldBodyConn" "Pkt_Body_Open" "Pkt_Body_Service"
                   "World_UpdateBody" "Pkt_Body_Send" "World_OneShotEffect"
                   "Pkt_Effect_Send" "WorldEffectConn" "Pkt_Effect_Open"
                   "Pkt_Effect_Service"
    +0x21C  180 B  "Embarrassed" .. "Final"  <- las 15 que SI emitimos, en el mismo orden
    +0x2D0    8 B  dos palabras a 0
    +0x2D8   20 B  "EmotionManagerImpl"
    +0x2EC   68 B  constantes float
    +0x330   20 B  "EmotionalResponse"
    +0x344   12 B  floats; los dos ultimos son lbl_803EBE90 (1.0f) y lbl_803EBE94

`rodorden zGameModes` lo confirma sin ambigüedad:

    objetivo 45 cadenas, nuestro 15, en secuencia 15 (fuera 30)
    delete   obj[0:29]  GAMECUBE d:/mw/speed/indep/bware/inc/bware.h...
    delete   obj[44:45] EmotionManagerImpl

Las 15 cadenas de emoción están **bien y en secuencia**. Faltan las 29 del prefijo de
cabeceras y `EmotionManagerImpl` (más `EmotionalResponse` y los floats, que `rodorden` no
lista). Y de los 232 B que sí emitimos, el enlace sólo conserva **104**: sin referencia ni
entrada de `keep.lst`, `-strip-unused-data` se lleva la mayoría, y lo que queda en el DOL es
un revoltijo compactado (`sed.Smug....egins...ddsCar..lock....nds....RampUp..Peak....Final`).

## SORPRESAS

**1. `reorden.py` da un falso ORDEN_PURO cuando el objetivo tiene un `pad_*` anónimo.**
Empareja por nombre; el troceado nombra la `.rodata` del objetivo `pad_05_...` / `lbl_...` y
nosotros emitimos `$LCnnn`, así que **nunca hay par** y la columna CONTENIDO sale 0 por
construcción. Aquí anunció "no queda nada que escribir" sobre 744 B que faltan. Su docstring
avisa de que el `.rodata` empareja por nombre, pero **el veredicto impreso no lo tiene en
cuenta**: debería imprimir la cobertura (palabras del objetivo comparadas / totales) y callarse
cuando es baja. Aquí fue **11 de 243 palabras = 4,5 %**.

**2. La sección `CAMBIO DE DUENO (M4)` de `movidos.py` no distingue "robado" de "estripado".**
`robado = enlazado − delta` vale lo mismo cuando otro objeto pierde el símbolo que cuando el
enlazador tira nuestro código muerto. zGameModes imprime los tres renglones del M4 y tiene
**cero** cambios de dueño. Arreglo: cruzar los símbolos que definimos de más con las tablas de
símbolos del resto de objetos antes de imprimir el bloque (es lo que hacía `r54ctl_robo.py`;
tarda segundos y no enlaza).

**3. `lbl_803EBB48` está en el fichero equivocado, y hay dos entradas fantasma en `keep.lst`.**
`src/Speed/Indep/SourceLists/zGameplay.cpp:4-17` emite el prefijo de 92 B de bWare con el
nombre **`lbl_803EBB48`**, que es la dirección de arranque de la `.rodata` de **zGameModes**;
el arranque de zGameplay es 0x803EBE98, y el objeto extraído `obj/zGameplay.o` define
`lbl_803EBE98`, no `lbl_803EBB48`. Medido en las tablas de símbolos:

    build\GOWE69\obj\...\zGameModes.o   pad_05_803EBB48_rodata  840 .rodata loc
    build\GOWE69\obj\...\zGameModes.o   lbl_803EBE90              4 .rodata GLB
    build\GOWE69\obj\...\zGameModes.o   lbl_803EBE94              4 .rodata GLB
    build\GOWE69\obj\...\zGameplay.o    lbl_803EBE98             92 .rodata GLB
    build\GOWE69\src\...\zGameplay.o    lbl_803EBB48             92 .rodata GLB   <- nombre de zGameModes
    build\GOWE69\src\...\zGameplay.o    lbl_803EBE90              4 .rodata GLB   <- simbolo de zGameModes

Consecuencias:

- `keep.lst:409 zGameplay.o:lbl_803EBE98` — **fantasma**: nuestro `zGameplay.o` no lo define,
  así que sus 92 B de prefijo hoy no los protege nadie.
- `keep.lst:2694 zGameModes.o:lbl_803EBB48` — **fantasma**: nuestro `zGameModes.o` tampoco lo
  define. (`keepchk2` detecta entradas que sobran, no entradas que no casan: el mismo hueco de
  herramienta que anotó `frontera` en la r54.)
- `src/Speed/Indep/Src/Gameplay/GRaceStatus.cpp:2207` afirma que `lbl_803EBE90` "cae en la
  `.rodata` de zGameplay". **Es falso**: 0x803EBE90 está dentro de la ventana de zGameModes
  (0x803EBB48–0x803EBE98) y quien lo define es `obj/zGameModes.o`. Hoy no rompe nada porque
  las dos unidades siguen sin promocionar, pero **si se promociona zGameplay antes que
  zGameModes, `lbl_803EBE90` queda definido dos veces**.
- El `asm()` de 100 cadenas de `zGameplay.cpp` ("r52-jf") arranca con las **29 cadenas que
  pertenecen a zGameModes** (`Attrib::Gen::effects` .. `EmotionManagerImpl`), puestas al final
  de la `.rodata` de zGameplay. Compensan el tamaño total del enlace, no la posición.

*(Todo el punto 3 es de zGameplay, que esta ronda lleva otro agente: lo dejo escrito y no lo
toco.)*

**4 (menor).** `rodorden.py` sólo acepta el nombre corto (`zGameModes`); con la ruta completa
que pide el brief responde `sin rango de .rodata en splits.txt`, y el rango está en
`splits.txt:121`.

## Siguiente paso concreto

**Escribir el prefijo de 540 B como `asm()` de fichero al principio de
`src/Speed/Indep/SourceLists/zGameModes.cpp`, ANTES del `#include ".../UVectorMath.h"`**, con
el símbolo **`lbl_803EBB48`** — el nombre que `keep.lst:2694` ya le reserva a `zGameModes.o` y
que hoy no define nadie con ese objeto.

Por qué ahí y no en otro punto: en el objetivo ese bloque está en el **byte 0** de la ventana
(0x803EBB48) y las 15 cadenas de emoción empiezan en +540; GCC 2.95 emite el `asm()` de
fichero en el punto del parseo, antes que cualquier `$LC` (que sale al cerrar cada función),
así que un `asm()` en la primera línea reproduce exactamente ese orden. Es el patrón que ya
usan 13 SourceLists (`zAI.cpp:12 lbl_803C8C60`, `zBWare.cpp:9`, `zCamera.cpp:14`,
`zDebug.cpp:19`, `zEcstasy` x16, ...), y los primeros 92 B —`GAMECUBE` .. `STL`— son **byte a
byte los mismos** que ya están escritos en `zGameplay.cpp:10-15`.

Presupuesto: 540 B de los 744. Los 204 restantes son `"EmotionManagerImpl"` (20 B),
`"EmotionalResponse"` (20 B), los floats del final y las cadenas de emoción que hoy se
estripan; van en el mismo `asm()` o, las que cc1plus sí emite, con entradas `@lc` en
`keep.lst` (el patrón `zPhysics.o:$LC59` de las líneas 2696 y siguientes).

**Precondición, hay que coordinarla con el agente de zGameplay:** renombrar el `asm()` de
`zGameplay.cpp:7-16` de `lbl_803EBB48` a **`lbl_803EBE98`** (su dirección real, y la que
`keep.lst:409` ya espera). Si no, las dos unidades definirán el mismo símbolo global.
Beneficio colateral para zGameplay: hoy sus 92 B de prefijo no los cubre ninguna entrada de
`keep.lst`.

Coste estimado: **una ronda** (el prefijo es transcripción; el residuo de 204 B y la
coordinación con zGameplay es donde se va el tiempo).
