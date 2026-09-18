# r55 — medida de `zMisc`: ORDEN PURO, y todo cuelga de **un símbolo de 120 B**

Unidad: `Speed/Indep/SourceLists/zMisc` — 78.008 B de código que guarda.
Reconstruida con `build_direct.py` (2 ok, 0 fallidas, al primer intento; ojo: la
herramienta empareja por PREFIJO y compiló también `zMiscSmall`, ver «sorpresas»).

## Veredicto: **ORDEN_PURO**

    reorden.py zMisc  ->  .text 0    .rodata 0    .data 6

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .data         194      290       15          6       24   (58 de otro TAMANO)
    .rodata       295      123       20          0        0   (282 de otro TAMANO)
    .text         449    19141     1051          0        0

**Las 6 palabras de `.data` son punteros, no contenido.** Volcadas byte a byte:

    _11AttribAlloc.mAllocator   obj 8047FEBC -> nue 80480A28   (+0xB6C)
    sChromeCars +04/+16/+28/+40/+52  obj 8041F738.. -> nue 80420278..  (+0xB40)

Son reubicaciones puras, desplazadas por el crecimiento de `.bss`/`.rodata`.
`reorden.py` las cuenta como CONTENIDO porque su máscara es de INSTRUCCIÓN: en
`.data` interpreta la palabra como opcode, ve una forma D y sólo enmascara los
16 bits bajos, así que un puntero que cruza un límite de 64 kB en su mitad baja
sale como diferencia real. **Residuo de contenido real de `zMisc`: CERO.**

Corroborado por `fncmp`: **0 de 450 funciones con el código distinto**; 112 más
con sólo nombres de símbolo distintos (reubicaciones), 26.000 B.

## `movidos.py Speed/Indep/SourceLists/zMisc`

    zMisc: 40247 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
       12002 cambian de direccion, pero 11847 es ARRASTRE (tres terminos: donde
       empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
       cambia de dueno).
       .bss: base+2880 delta+288, .data: base+2848 delta+28, .rodata: base+0 delta+3392,
       .sbss: base+2880 delta+0, .sbss2: base+2880 delta+0, .sdata: base+2880 delta+0,
       .sdata2: base+2880 delta+0, .text: base+0 delta+6888

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .bss       nuestro .o    +288, seccion enlazada      +0  ->     -288 de otro
          .data      nuestro .o     +28, seccion enlazada     +32  ->       +4 de otro
          .rodata    nuestro .o   +3392, seccion enlazada   +2840  ->     -552 de otro
          .text      nuestro .o   +6888, seccion enlazada      +0  ->    -6888 de otro

       ESCALONES SIN EXPLICAR (4): un desplazamiento que comparten muchos
       simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
       NO se cuenta como permutacion. Que exista uno es la firma de que un
       simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
          .text           +120   2255 simbolos
          .bss           +2924     47 simbolos
          .data          +3920     24 simbolos
          .data          +2136     21 simbolos

    **155 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

    seccion       movidos      bytes
    .data             124      1,751
    .rodata            18        328
    .bss                9        224
    .text               1        120
    ?                   3          0

**155 permutaciones reales.** El `.text` sólo tiene **una**, y es el símbolo dominante.

## El símbolo dominante

    -499872   120 B  .text
    clear__Q24_STLt10_List_base2Z23WGridManagedDynamicElemZQ33UTL3Stdt9Allocator2
    Z23WGridManagedDynamicElemZ10_type_list

Barrido de saltos > 20 kB sobre las dos tablas de símbolos ENLAZADAS: **1 de
40.247**. Nuestro `zMisc.o` lo define; el `zMisc.o` extraído no; en el enlace
base lo suministra `obj/zSim.o`. Orden de enlace: **zAI 0, zCamera 4, zMisc 19,
zMiscSmall 21, zRender 26, zSim 27** — zMisc gana el turno a zSim y el símbolo
salta de `0x80286528` a `0x8020C488`, arrastrando **2.255 símbolos a +120 B**.

**Cuánto explica**: comparando los dos ELF enlazados byte a byte por dirección,
**587.327 B** distintos, de los cuales **439.210 B (74,8 %)** caen dentro del
rango `[8020C488, 80286528)` que abre ese salto. La cifra de 439.304 B de
`r54-control` se reproduce casi al byte; mi total es menor que su 767.221 porque
cuento sólo secciones cuya dirección de inicio coincide en los dos enlaces.

## La causa, PROBADA POR ABLACIÓN

`src/Speed/Indep/Src/Misc/ResourceLoader.cpp:9` incluye
`Speed/Indep/Src/World/WCollisionAssets.h`, que en sus líneas 7 y 9 incluye
`WGridManagedDynamicElem.h`. Ahí, **líneas 19-25**:

    static void Init()     { fgManagedDynamicElemList.clear(); }
    static void Shutdown() { fgManagedDynamicElemList.clear(); }

Dos cuerpos DENTRO de la clase. Sonda compilada con los cflags exactos de zMisc
sobre una TU que sólo incluye esa cabecera y declara un `int`:

    120 .gnu.linkonce.t.clear__..._List_base<WGridManagedDynamicElem,...>   <-- sale
    120 .gnu.linkonce.t.clear__..._List_base<WGridNodeElem,...>
     76 .gnu.linkonce.t.__lower_bound__<const float*,...>
     72 .gnu.linkonce.t.__lower_bound__<const Ui*,...>

Con la cabecera sombreada (`-I <sombra>` delante de `-I src`, control con
`#error` verificado) y `Init`/`Shutdown` **declarados en vez de definidos**:

    120 .gnu.linkonce.t.clear__..._List_base<WGridNodeElem,...>
     76 .gnu.linkonce.t.__lower_bound__<const float*,...>
     72 .gnu.linkonce.t.__lower_bound__<const Ui*,...>

**Desaparece.** Nadie en zMisc llama a `Init`/`Shutdown`: los únicos llamantes
del árbol son `WCollisionAssets.cpp:82` y `:113`, que zMisc **no** compila.
El hermano `WGridNodeElem` sigue saliendo (viene de `WGridNode.h`, mismo patrón)
pero es inofensivo aquí: lo define también `zAI.o`, que enlaza en la posición 0
y gana siempre.

## El resto de las medidas

| medida | resultado |
|---|---|
| `linkdelta` | `.text +0`, `rodata +2840`, `data +32` |
| `permorden` | **450 de 450 en su sitio, 0 desplazadas**; 91 sólo nuestras (el enlazador las estripa) |
| `rodorden` | 307 de 307 **en secuencia**, más `insert nue[307:457]` = **150 cadenas de más** (`_EU`, `done`, `19.8.31`, `16.2.1`, `1.2.3`, `GManager`, `SMS_M…`) |
| `vtord` | **10 de 10 posiciones mal**: el grupo `13AverageWindow`/`7Average`/`11AverageBase` sale ANTES de `16DisculatorDriver` en vez de después, con `11AverageBase` (32 B) de más |
| `fncmp` | 0 de 450 con código distinto |
| `dolwhere` | no aplica (`SECCIONES NO COINCIDEN`) |
| `trypromo` | **DOL ROTO (0426ff4c9e9d)** |

## Por qué el `.text -6888` de M4 no son 6.888 B robados

Nuestro `zMisc.o` define **94 símbolos globales (6.928 B) que el `zMisc.o`
extraído no define** — casi todos COMDAT de plantilla (`reserve__vector<…>`,
`find__<…>`, `clear__List_base<…>`, `Get__TAttrib<…>`). De esos 94, **93 los
descarta el enlazador**: los suministra un objeto que enlaza ANTES (zAI en la
posición 0, zCamera en la 4). `permorden` lo confirma desde el otro lado: «solo
nuestras (las estripa el enlazador): 91».

Sólo uno gana el turno, y es el único cuyo otro definidor está DETRÁS de zMisc:
`clear__…WGridManagedDynamicElem…`, cuyo único rival es `zSim.o` (posición 27).

**Por tanto la resta de `movidos.py` sobreestima M4 por un factor de 57 en esta
unidad: dice 6.888 B de `.text` y el bulto que de verdad cambia de dueño son
120 B.** La resta mide tamaños de `.o`, no colocación, y no sabe que el
enlazador tira los COMDAT duplicados.

## Siguiente paso concreto

**No es un problema de posición dentro del fichero: es un cambio de dueño, y se
arregla dejando de emitir el símbolo, no moviéndolo.**

1. **Ahora, y sólo para zMisc** (patrón que este mismo fichero ya usa cinco
   veces): guardar los dos `#include "…/WGridManagedDynamicElem.h"` de
   `src/Speed/Indep/Src/World/WCollisionAssets.h` (líneas **7** y **9**, están
   duplicados) con un `#ifndef MISC_WCOLLISIONASSETS_NO_WGRID`, y poner
   `#define MISC_WCOLLISIONASSETS_NO_WGRID` en
   `src/Speed/Indep/SourceLists/zMisc.cpp` **antes del primer `#include`**,
   junto a `MISC_RESOURCELOADER_HPP_NO_DRIVER` (líneas 28-36). zMisc sólo usa
   `WCollisionAssets::Get().LoadCollisionPack/UnLoadCollisionPack`
   (`ResourceLoader.cpp:645/653`), que no necesita el tipo completo.
   Coste esperado: `.text` de zMisc **−120 B**, `linkdelta .text` sigue en +0
   (zSim recupera sus 120 B), y **439.210 B de DOL** dejan de diferir.
2. **Arreglo de raíz, para las tres unidades**: sacar `Init()`/`Shutdown()` del
   cuerpo de la clase en `WGridManagedDynamicElem.h:19-25` y definirlos en
   `src/Speed/Indep/Src/World/Common/WGridManagedDynamicElem.cpp`. Vale
   439 kB (zMisc) + 389 kB (zPhysics) + 852 kB (zGameplay) de DOL distinto.
   **Riesgo medido**: los únicos llamantes están en `WCollisionAssets.cpp`, que
   compila **`zWorld2`** (`zWorld2.cpp:48`), mientras que el original coloca el
   símbolo en **`zSim.o`**. O sea, el arreglo de raíz no elimina el cambio de
   dueño: lo **traslada** de zMisc a zWorld2. Hay que hacerlo con zWorld2 y zSim
   en el mismo paquete atómico, y averiguar antes por qué el original lo emite
   desde zSim. **Para la r55 recomiendo el punto 1**, que es local, y dejar el 2
   para una ronda que pueda mover cuatro unidades a la vez.
3. **Lo que queda después** (148.117 B, el 25 %): 150 cadenas de más en
   `.rodata` (superávit puro y ORDENADO: hay que **dejar de emitir**, no
   reordenar), la vtable `11AverageBase` de más con el grupo `Average*`
   adelantado, y 124 permutaciones de `.data` que suman 1.751 B.

## Sorpresas

1. **`reorden.py` da falsos positivos de CONTENIDO en `.data`.** Su máscara es
   de instrucción: sólo enmascara los 16 bits bajos de una forma D, así que un
   puntero de datos cuya mitad ALTA cambia (>64 kB de desplazamiento) se cuenta
   como contenido real. Las 6 palabras de zMisc son exactamente ese caso.
   Arreglo propuesto: en secciones de datos, comparar con la reubicación
   deshecha, o al menos marcar la fila como «puntero» en vez de «CONTENIDO».
2. **`reorden.py` no compara el 96 % del `.rodata` de zMisc.** 282 de 295
   símbolos salen «de otro TAMANO» con `nue 0` porque el pool escrito a mano en
   `zMisc.cpp` (líneas 50-3400) emite `.globl` y `.type` pero **no `.size`**.
   Su «`.rodata` CONTENIDO 0» descansa sobre 13 símbolos. Añadir `.size` a los
   bloques `asm()` del pool haría medible esa sección — y es una pega que
   afectará a toda unidad con pool a mano.
3. **El `.text -6888` de la sección M4 de `movidos.py` es 57 veces el bulto real**
   (120 B). Ver arriba: la resta no distingue un COMDAT duplicado que el
   enlazador tira de un símbolo que de verdad cambia de dueño.
4. **El censo de cambio de dueño con umbral de 20 kB puede dejarse casos.** Aquí
   no (comprobado bajando el umbral a 2 kB y siguiendo los 23 símbolos de
   plantilla uno a uno), pero un robo a un objeto VECINO en el orden de enlace
   produce un salto de pocos kB indistinguible del arrastre. En zMisc el rival
   está a 8 posiciones y el salto es de medio megabyte; en `zFe`/`zFe2`, con 47
   y 49 robos, ese umbral es una apuesta.
5. **`build_direct.py` empareja por PREFIJO**: `build_direct.py
   Speed/Indep/SourceLists/zMisc` compiló `zMisc` **y `zMiscSmall`**. Con trece
   agentes en paralelo eso puede pisar el `.o` de otro. Nadie tenía zMiscSmall
   esta ronda, pero conviene saberlo.
6. **`rodorden.py` no acepta la ruta completa** (`Speed/Indep/SourceLists/zMisc`
   → «sin rango de .rodata en splits.txt»); hay que pasarle el nombre corto.
   `linkdelta`, `movidos`, `permorden`, `vtord`, `fncmp` y `trypromo` sí la
   aceptan.
