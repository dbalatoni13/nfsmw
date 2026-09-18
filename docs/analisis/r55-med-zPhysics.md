# r55 — medida de `zPhysics`

**Veredicto: MIXTO, pero por los pelos — y el residuo NO es difuso: lo manda un símbolo de
120 B que ni siquiera es nuestro.**

`reorden.py` deja `.text` y `.data` a **0 palabras de contenido**, y de las 44 que marca en
`.rodata` **42 son punteros reubicados** (apuntan al mismo símbolo, movido de sitio) y sólo
**2 son contenido de verdad**: 8 bytes, en una vtable, con causa exacta y arreglo de dos
líneas.

Reconstruida con `python scripts/build_direct.py Speed/Indep/SourceLists/zPhysics`
(2 ok, 0 fallidas al primer intento).

---

## 1. `reorden.py zPhysics` — ¿orden o contenido?

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .data          15      111        7          0        0
    .rodata       458     2971      929         44      303   (1 de otro TAMANO)
    .text         717    34532     2468          0        0

**`.text` = 0 y `.data` = 0.** Los 146.124 B de código que guarda esta unidad están
escritos byte a byte: 2.468 palabras crudas distintas de 34.532 y **todas** son campo de
reubicación. No queda una sola instrucción que escribir.

Las 44 de `.rodata` las abrí una a una (sonda propia, sólo lectura: para cada palabra que
`reorden` da por contenido, mira a qué **símbolo** apunta el valor en cada uno de los dos
enlaces):

    palabras de CONTENIDO en .rodata: 44
       apuntan al MISMO simbolo (o sea REUBICACION, no contenido): 42
       apuntan a simbolo DISTINTO (contenido de verdad):            2

Las 42 son entradas de vtable cuyo `pfn` apunta a la misma función movida de dirección —
`_vt.16PlaceableScenery.6IModel+204` → `ReleaseModel__16PlaceableScenery` en los dos, a
0x8021FFE8 y 0x8022010C. **`reorden.py` no puede enmascararlas**: su `mascara()` sólo
neutraliza el inmediato de 16 bits de las formas D y el desplazamiento de las ramas, o sea
**codificaciones de instrucción**; en `.rodata` una entrada de vtable es un `R_PPC_ADDR32`
entero y cae al caso `('r', v)` — el valor crudo. Es una sobrecuenta sistemática de
`reorden` en `.rodata`, no un fallo de esta unidad (ver «Sorpresas»).

### Las 2 palabras de contenido real: `_vt.13PhysicsObject.8ISimable` +256 y +264

Volcando la vtable entera contra la del objetivo, entrada por entrada (8 B por ranura:
`{short delta; short index; void *pfn;}`, o sea sin `-fvtable-thunks`):

    ... +252  obj 80218CEC ProcessStimulus__13PhysicsObjectUi | nue 80218DD0 ProcessStimulus__13PhysicsObjectUi
        +256  obj FFD40000                                    | nue 00000000        <-- DISTINTO
        +260  obj 8031AB80 __pure_virtual                     | nue 8031AB80 __pure_virtual
        +264  obj FFD40000                                    | nue 00000000        <-- DISTINTO
        +268  obj 8031AB80 __pure_virtual                     | nue 8031AB80 __pure_virtual
        +272  obj FFD40000                                    | nue FFD40000
    ... +276  obj 802174AC SetCausality__13PhysicsObjectP8HCAUSE__f | nue 80217590 SetCausality__...

`0xFFD4` = **delta −44**, y es el delta que llevan **todas** las demás ranuras de esta
vtable secundaria, también en la nuestra. Las dos únicas que se nos quedan a **0** son las
dos ranuras cuyo `pfn` es `__pure_virtual`. Por orden de declaración de `ISimable.h:64-65`
esas dos ranuras son:

    virtual IModel *GetModel() = 0;
    virtual const IModel *GetModel() const = 0;

GCC 2.95 sólo estampa el delta ajustado de la clase derivada en la ranura que la derivada
**redeclara**. Si `PhysicsObject` no la nombra, la ranura hereda el delta 0 de `ISimable`.
`src/Speed/Indep/Src/Physics/PhysicsObject.h` declara `ProcessStimulus` (línea 94) y
`SetCausality` (línea 95) **y no declara `GetModel`**, que es justo el hueco de la vtable.

**Arreglo (descrito, NO aplicado): `PhysicsObject.h`, entre la línea 94 y la 95**

    virtual IModel *GetModel() = 0;
    virtual const IModel *GetModel() const = 0;

Vale 8 B y es la única deuda de contenido de toda la unidad.

---

## 2. `movidos.py Speed/Indep/SourceLists/zPhysics`

    zPhysics: 39940 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
       11597 cambian de direccion, pero 11058 es ARRASTRE (tres terminos: donde
       empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
       cambia de dueno).
       .bss: base+32 delta+24, .data: base+32 delta+0, .rodata: base+0 delta+1808,
       .sbss: base+64 delta+0, .sbss2: base+64 delta+0, .sdata: base+64 delta+0,
       .sdata2: base+64 delta+0, .text: base+0 delta+42228

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .bss       nuestro .o     +24, seccion enlazada     +32  ->       +8 de otro
          .rodata    nuestro .o   +1808, seccion enlazada      +8  ->    -1800 de otro
          .text      nuestro .o  +42228, seccion enlazada      +0  ->   -42228 de otro

       ESCALONES SIN EXPLICAR (7): un desplazamiento que comparten muchos
       simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
       NO se cuenta como permutacion. Que exista uno es la firma de que un
       simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
          .text           +120   1500 simbolos
          .text           +292    197 simbolos
          .bss             +48     66 simbolos
          .text           +228     36 simbolos
          .text           +412     31 simbolos
          .text            +72     20 simbolos
          .text           -108     20 simbolos

    **539 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

    seccion       movidos      bytes
    .text             389     57,176
    .rodata           143     10,546
    .data               4         32
    ?                   3          0

**539 permutaciones reales**, no las 7.317 de la tabla de la r54 (aquélla es de antes de la
corrección de `movidos`).

### El −42.228 de la sección M4 es falso, y lo he censado

`delta` sale de las **cabeceras del `.o`**, y el enlazador lleva `-strip-unused-data`. Censé
símbolo a símbolo los que **nuestro** objeto define y el original **no**, y miré cuánto salta
cada uno en el enlace:

    simbolos globales definidos: original 1293, nuestro 1098
    de MAS en el nuestro: 224 (43960 B)   -> .text 42140 B, .rodata 1812 B, .data 8 B

    CAMBIO DE DUENO REAL (salto > 20 kB en el enlace): 1 simbolo, 120 B
         -388752    120 B  .text  clear__Q24_STLt10_List_base2Z23WGridManagedDynamicElem...

    de mas pero ESTRIPADOS (no estan en el enlace base): 118, 34436 B
    de mas y QUIETOS (mismo sitio +-20 kB):              105,  9404 B

**El cambio de dueño real de `zPhysics` son 120 bytes, no 42.228.** De los 43.960 B de más,
34.436 los tira el enlazador y 9.404 se quedan donde estaban (los suministra ya otro objeto
que enlaza antes).

---

## 3. El símbolo dominante, y no es difuso

    clear__Q24_STLt10_List_base2Z23WGridManagedDynamicElemZQ33UTL3Stdt9Allocator2
      Z23WGridManagedDynamicElemZ10_type_list      120 B, .text
    0x80286528  ->  0x80227698      desplazamiento  -388.752 B

Se lo quitamos a quien lo suministra en el enlace base y lo metemos dentro de nuestra
ventana: los **1.500 símbolos** que quedaban entre las dos direcciones se desplazan **+120 B**,
exactamente su tamaño.

Bytes distintos entre los dos ELF enlazados, byte a byte por dirección (secciones con datos):

    .text     453.974 de 3.804.440
    .rodata    72.439 de   312.608
    .data         733 de   267.228
    .sdata         30 | .sdata2 2 | .ctors 7
    TOTAL     527.185

Repartidos:

| zona | bytes | % |
|---|---:|---:|
| **dentro del rango que arrastra `clear`** | **342.827** | **65,0 %** |
| dentro de la ventana `.text` de zPhysics (reorden interno) | 120.619 | 22,9 % |
| `.rodata` fuera de la ventana (rizo del `+8`) | 64.459 | 12,2 % |
| `.data` fuera de la ventana (arrastre `+32`) | 715 | 0,1 % |

(las dos primeras filas se solapan en 36.411 B: el tramo del rango que cae dentro de nuestra
propia ventana.)

**Un símbolo de 120 bytes explica 342.827 B — el 65 % del residuo de la unidad.**

### De dónde sale, con la cadena entera

Nuestro `.o` emite **dos** instanciaciones débiles (`WEAK`, `FUNC`) que el original no tiene:

    clear__..._List_base<WGridManagedDynamicElem, ...>   120 B   <- cambia de dueño
    clear__..._List_base<WGridNodeElem, ...>             120 B   <- ya lo suministra 0x8003CD70, inofensivo

y **ningún `.text` nuestro las referencia**: la única reubicación que menciona
`_23WGridManagedDynamicElem.fgManagedDynamicElemList` en todo el objeto está en `.debug`.
Son instanciaciones **muertas**, emitidas sólo por haber *parseado* la cabecera; el enlazador
las mantiene porque otro objeto sí llama a `clear`, y como `zPhysics` enlaza antes, se lleva
la definición.

La cabecera es `src/Speed/Indep/Src/World/WGridManagedDynamicElem.h:19-25`:

    static void Init()     { fgManagedDynamicElemList.clear(); }
    static void Shutdown() { fgManagedDynamicElemList.clear(); }

dos cuerpos **en clase** que tocan `clear()` de una plantilla. (La de `WGridNodeElem` sale
igual, de `WGridNode.h:42 ShutDown() { delete fDynElems; }`, que arrastra el destructor de
`list<WGridNodeElem>`.)

Y llega a `zPhysics` por un include que **no hace falta**:

    src/Speed/Indep/Src/Physics/SmackableTrigger.cpp:4
        #include "Speed/Indep/Src/World/WCollisionAssets.h"
    src/Speed/Indep/Src/World/WCollisionAssets.h:7
        #include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
    src/Speed/Indep/Src/World/WCollisionAssets.h:9
        #include "WGridManagedDynamicElem.h"          <- el MISMO, dos veces

`WCollisionAssets.h` **no usa el tipo en ningún sitio** (grep: sólo esas dos líneas), y
`SmackableTrigger.cpp` tampoco. Los `.cpp` que sí lo usan ya lo incluyen **directos**:

    src/Speed/Indep/Src/World/Common/WCollisionAssets.cpp:8   (y llama a Init/Shutdown en 82 y 113)
    src/Speed/Indep/Src/World/Common/WTrigger.cpp:8
    src/Speed/Indep/Src/Sim/Common/Simulation.cpp:88

---

## 4. `linkdelta`, `permorden`, `trypromo`

    linkdelta   Speed/Indep/SourceLists/zPhysics    .text +0   rodata+8 bss+32
    trypromo    Speed/Indep/SourceLists/zPhysics    DOL ROTO (0d27a7c17d3a)

    permorden   objetivo 718 funciones, nuestro 913, comunes 718
                solo nuestras (las estripa el enlazador): 195
                en su sitio (subsecuencia creciente mas larga): 520 de 718
                DESPLAZADAS: 198
                ciclos no triviales: 17  (longitudes: 433, 77, 36, 32, 31, 13, 10, 7, 7, 5, 5, 4)

El ciclo de **433** es el bloque de `PVehicle.cpp`: sus métodos salen barajados dentro de
±23 kB (`Launch__8PVehicle` +1.636, `ComputeHeading__8PVehicle` −22.996, y en medio
`sort_remove_resources__Q28PVehicle10ManageNode` −118.800 y `SimplifySort__9Smackable`
−88.624, que se van casi a la cabeza de la ventana). Los 17 primeros símbolos de la ventana
(`SmackableTrigger.cpp`) están a **+0**: la unidad empieza bien y se desordena al entrar
`PVehicle`.

`.rodata` es un barajado de vtables, **difuso**: casi cada vtable con su propio
desplazamiento, de −4.368 (`_vt.17ITriggerableModel`) a +2.184
(`_vt.Q43UTL...Listable<IDisposable,160>::List`). `rodorden` da 159 de 183 cadenas en
secuencia con 22 cadenas nuestras adelantadas al puesto 24. `.data`: 4 símbolos permutados,
32 B.

---

## 5. El siguiente paso concreto

**Quitar las líneas 7 y 9 de `src/Speed/Indep/Src/World/WCollisionAssets.h`** (las dos
copias del `#include ".../WGridManagedDynamicElem.h"`). La cabecera no usa el tipo y los
cuatro `.cpp` que sí lo usan ya lo incluyen directos, así que el árbol sigue compilando y
`zSim`/`zWorld2` conservan la definición de `clear`.

Vale **342.827 de los 527.185 B** de residuo de `zPhysics` (65 %).

Y es **paquete atómico de tres unidades**: los otros dos consumidores de esa cabecera que no
usan el tipo son

    src/Speed/Indep/Src/Misc/ResourceLoader.cpp        (zMisc)
    src/Speed/Indep/Src/Gameplay/GRaceDatabase.cpp     (zGameplay)
    src/Speed/Indep/Src/Gameplay/GRaceStatus.cpp       (zGameplay)
    src/Speed/Indep/Src/Gameplay/GTrigger.cpp          (zGameplay)

que son exactamente las dos unidades a las que la r54 midió el mismo símbolo robado
(`zMisc` −499.872, `zGameplay` −852.004). **No las he medido: es un `grep`, no una medida.**
Hay que rehacer `movidos`/`trypromo` de las tres después del cambio.

Segundo paso, barato e independiente: las dos líneas de `PhysicsObject.h` (punto 1). 8 B, y
deja la unidad **sin una sola palabra de contenido pendiente**.

Tercero, ya sin atajo: el orden de `.text` dentro de la ventana (120.619 B, ciclo de 433 en
`PVehicle.cpp`) y los 8 B de más de `.rodata` que rizan 64.459 B fuera.

---

## Sorpresas

1. **`reorden.py` sobrecuenta el contenido de `.rodata`, y por diseño.** Su `mascara()`
   neutraliza el inmediato de 16 bits de las formas D y el desplazamiento de las ramas —
   codificaciones de **instrucción**. Una entrada de vtable es un `R_PPC_ADDR32` de 32 bits
   y no la toca ninguna de las dos reglas, así que **cualquier puntero a una función movida
   cuenta como contenido**. En `zPhysics` son 42 de 44. Cualquier unidad con vtables va a
   leer «hay contenido» donde sólo hay orden. Arreglo: en `.rodata`, resolver el valor a
   símbolo en cada enlace y comparar **nombres**, no direcciones (es lo que hace mi sonda).
2. **La sección «CAMBIO DE DUEÑO (M4)» de `movidos.py` infla, y mucho.** Compara el tamaño
   de sección del `.o`, y el enlace lleva `-strip-unused-data`: dice **−42.228 B** en
   `.text` donde el censo símbolo a símbolo da **120 B**, un factor de 352. Los otros
   42.108 B son 34.436 de código muerto que el enlazador tira y 9.404 que ya suministraba
   otro objeto antes. La cifra de M4 sólo vale si se cruza con la tabla de símbolos del
   enlace; el censo tarda segundos y no necesita enlazar más de lo que ya enlaza.
3. **`build_direct.py` empareja por SUBCADENA de la ruta de salida** (`hit()`:
   `w in spec[1]`). Pasarle `Speed/Indep/SourceLists/zPhysics` — la ruta completa que pide
   el brief — compila **también `zPhysicsBehaviors`** («compilando 2 unidades»). Con trece
   agentes en paralelo eso pisa el `.o` de otro sin avisar. Cualquier unidad cuyo nombre sea
   prefijo de otra tiene el mismo problema.
4. **La cifra de DOL de la r54 para `zPhysics` (664.976 B) no se reproduce**: mido
   **527.185 B** byte a byte sobre las secciones con datos de los dos ELF enlazados. La
   diferencia puede ser método (yo no cuento `.bss` ni el relleno del DOL) o árbol movido;
   no la he perseguido, pero el reparto no debería usar las dos cifras como si fueran la
   misma.
5. `zPhysics` emite **dos** instanciaciones muertas de `clear`, no una. La de
   `WGridNodeElem` hoy no roba nada porque otro objeto la suministra a 0x8003CD70 en los dos
   enlaces — pero es la misma bomba con la espoleta quitada, y sale de la misma cabecera.

## Sondas

En el scratchpad de la sesión, borradas al terminar. Ninguna escribe en el árbol.
Si algo merece quedarse en `scripts/`, es la resolución **por nombre de símbolo** de las
palabras de `.rodata` dentro de `reorden.py` (sorpresa 1) y el censo de dueño real cruzado
con el enlace (sorpresa 2).
