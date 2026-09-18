# r55 — medida de `zGameplay`

Unidad: `Speed/Indep/SourceLists/zGameplay` — 141.472 B de `.text` que guarda.
Reconstruida con `python scripts/build_direct.py Speed/Indep/SourceLists/zGameplay`:
**1 ok, 0 fallidas al primer intento** (no hizo falta reintentar).

Todas las cifras salen de dos enlaces completos hechos en esta sesión (base con el objeto
extraído, y el mismo enlace cambiando sólo `zGameplay.o` por el nuestro).

---

## 1. ¿Orden o contenido? — **ORDEN**

    zGameplay: NUESTROS bytes recolocados en el ORDEN DEL OBJETIVO

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .data          27       64        0          0       18
    .rodata       295      200       42          3      267   (1 de otro TAMANO)
    .text         767    35289     3534          0        1

    quedan 3 palabras de CONTENIDO real:
       .rodata     1 pal de   48 B   _vt.11GRaceStatus
       .rodata     2 pal de   48 B   _vt.8GManager
       .rodata  TAMANO   lbl_803ED288                                         obj 16  nue 4

**`.text` 0 de 35.289 palabras y `.data` 0 de 64.** No queda una sola instrucción por
escribir: los 3.534 desacuerdos crudos de `.text` son todos campo de reubicación.

**Las 3 palabras de `.rodata` tampoco son contenido**, y está comprobado palabra a palabra:

    _vt.11GRaceStatus  A 803ED118 48 B   B 803ED060 48 B
       +12  * 8019DB1C (_._11GRaceStatus)             8019DF60 (_._11GRaceStatus)
       +20  * 801BC0F0 (GetCacheName__C11GRaceStatus) 8019E300 (GetCacheName__C11GRaceStatus)
       +28  * 8019E858 (OnQueryVehicleCache__C11GRaceStatus...) 8019E490 (el mismo)
       +36  * 8019EA04 (OnRemovedVehicleCache__11GRaceStatus...) 8019E63C (el mismo)

Las palabras marcadas son punteros `ADDR32` a **la misma función con el mismo nombre**, que
ha cambiado de dirección. `reorden.py` enmascara la mitad baja de las formas D y el
desplazamiento de las ramas, pero **no enmascara una reubicación de palabra entera**, que es
lo único que hay dentro de una vtable. Ver «Sorpresas».

`lbl_803ED288`: el objetivo tiene una etiqueta de 16 B donde nosotros tenemos una de 4
(`42C80000` = 100.0f, idéntica en las dos). Es granularidad del pool, no contenido distinto.

**Veredicto: ORDEN_PURO.** El trabajo de fuente de esta unidad es cero.

---

## 2. `movidos.py` — permutaciones y cambio de dueño

    zGameplay: 39953 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
       16292 cambian de direccion, pero 15760 es ARRASTRE (tres terminos: donde
       empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
       cambia de dueno).
       .bss: base-256 delta+24, .data: base-128 delta-152, .rodata: base+0 delta+352,
       .sbss: base-256 delta+0, .sbss2: base-256 delta+0, .sdata: base-256 delta+0,
       .sdata2: base-256 delta+0, .text: base+0 delta+12144

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .bss       nuestro .o     +24, seccion enlazada      +0  ->      -24 de otro
          .data      nuestro .o    -152, seccion enlazada    -128  ->      +24 de otro
          .rodata    nuestro .o    +352, seccion enlazada    -136  ->     -488 de otro
          .text      nuestro .o  +12144, seccion enlazada      +0  ->   -12144 de otro

       ESCALONES SIN EXPLICAR (5): un desplazamiento que comparten muchos
       simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
       NO se cuenta como permutacion. Que exista uno es la firma de que un
       simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
          .text           +120   4590 simbolos
          .text            +56    114 simbolos
          .text          +2052     41 simbolos
          .text           +176     26 simbolos
          .text           +336     20 simbolos

    **532 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

    seccion       movidos      bytes
    .text             493     98,624
    .rodata            28        816
    .data               8        232
    ?                   3          0

**Permutaciones reales: 532.** El cambio de dueño literal suma 12.680 B en valor absoluto
(24 + 24 + 488 + 12.144) — pero **verificado símbolo a símbolo son 120 B**, ver «Sorpresas».

---

## 3. El símbolo dominante — y es uno solo

    -852004    120 B  .text  80286528 -> 801B6504
    clear__Q24_STLt10_List_base2Z23WGridManagedDynamicElemZQ33UTL3Stdt9Allocator2Z23WGrid...

* **Tamaño**: 120 B.
* **Desplazamiento**: **−852.004 B**. En el enlace base vive en `0x80286528`, que cae dentro
  de la ventana de **`zSim`** (`0x802728EC..0x8028A17C`). En el nuestro cae en `0x801B6504`,
  dentro de `zGameplay` (`0x8019A26C..0x801BCB0C`).
* **Arrastre**: el escalón `+120` de **4.590 símbolos**, que cubre `801BC814..80286468`:
  **826.452 B de `.text` corridos 120 B**.
* **Bytes de DOL que explica**:

      BYTES DISTINTOS entre el enlace BASE y el NUESTRO (por direccion):
         .text       869329  (dentro del rango del simbolo dominante: 747144)
         .rodata     117333
         .data       136592
         .sdata          32
         .sdata2          1
         .ctors          16
         TOTAL     1123303   dentro de 801B6504..802865A0: 747144 (66,5 %)

**Un símbolo de 120 bytes explica 747.144 de los 1.123.303 B distintos: el 66,5 %.**
No es un residuo difuso: es el residuo más concentrado del bloque A medido hasta ahora.
(La r54 dio 1.173.534 B para esta unidad; la diferencia es la reconstrucción de hoy.)

Es **M4 puro** (cambio de dueño), no M1: el objetivo **no emite ese símbolo en `zGameplay`**.
Nuestro `zGameplay.o` lo define, `zGameplay` enlaza antes que `zSim`, y le gana el turno.
Los vecinos en los dos enlaces lo confirman — en los dos casos está en la cola de plantillas
diferidas de su objeto, o sea que la POSICIÓN dentro de la unidad es la correcta; lo que está
mal es la unidad:

    NUESTRO:  ...Advance__t15GObjectIterator1Z9GActivity | >> clear__...WGrid... | reserve__...vector<ISimable*>
    BASE:     ...OnTask__16GameplayActivity              | >> clear__...WGrid... | find_if__...GarbageNode<Sim::Activity,40>

Los otros cuatro escalones (`+56`/114 símbolos, `+2052`/41, `+176`/26, `+336`/20) son
traslaciones internas de bloques de `zGameplay` y suman menos de 64 kB de rango entre los
cuatro.

Fuera del rango dominante quedan **376.159 B**, y ahí sí es difuso: el déficit de
`.rodata −136` y `.data −128` arrastra sus secciones enteras (117.333 + 136.592 B), más
122.185 B de `.text` repartidos entre las 493 funciones permutadas, sin cabecilla.

---

## 4. `linkdelta`, `permorden`, `trypromo`

    DISTANCIA REAL AL ENLACE
    Speed/Indep/SourceLists/zGameplay                  +0   rodata-136 data-128

    zGameplay: objetivo 768 funciones, nuestro 963, comunes 767
    solo nuestras (las estripa el enlazador): 196
    SOLO DEL OBJETIVO (1): Compare__Q38GManager48GatherVisibleIcons
    en su sitio (subsecuencia creciente mas larga): 497 de 767
    DESPLAZADAS: 270
    ciclos no triviales: 9  (longitudes: 652, 10, 9, 6, 3, 3, 2, 2, 2)

    Speed/Indep/SourceLists/zGameplay            DOL ROTO (b97ac07fcd02)

El ciclo de 652 no es un desorden de 652 cosas distintas: es el escalón `+120` visto por
`permorden`, que compara **posiciones**, no direcciones.

---

## 5. El siguiente paso concreto — probado por ablación

**Causa raíz, medida con el compilador, no supuesta.** Una TU vacía cuyo único contenido es

    #include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"

compilada con las cflags exactas de `zGameplay`, ya emite el símbolo:

    simbolos con WGridManagedDynamicElem: 2
        0 B  _23WGridManagedDynamicElem.fgManagedDynamicElemList
      120 B  clear__Q24_STLt10_List_base2Z23WGridManagedDynamicElemZ...

Lo instancian los dos cuerpos **en clase** de `WGridManagedDynamicElem.h:19-25`:

    static void Init()     { fgManagedDynamicElemList.clear(); }
    static void Shutdown() { fgManagedDynamicElemList.clear(); }

**Ablación 1** (cabecera sombreada con un símbolo de control para probar que se lee la
copia): declarando `Init()`/`Shutdown()` sin cuerpo, el símbolo **desaparece**. Confirmado
que el sombreado estaba activo — el símbolo de control sale en la tabla del `.o`.

**Y por dónde entra en `zGameplay`**: `WCollisionAssets.h` incluye
`WGridManagedDynamicElem.h` **dos veces** (líneas 7 y 9) y **no nombra el tipo en ninguna
parte de su cuerpo**. De ahí lo cogen `GRaceStatus.cpp`, `GRaceDatabase.cpp` y
`GTrigger.cpp`, que tampoco lo usan (`grep -rn WGridManagedDynamicElem
src/Speed/Indep/Src/Gameplay/`: cero aciertos).

**Ablación 2, la propuesta, compila y da cero:**

    en src/Speed/Indep/Src/World/WCollisionAssets.h
      linea 7:  #include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
             -> #include "Speed/Indep/Src/World/Common/WGridNode.h"
      linea 9:  #include "WGridManagedDynamicElem.h"     -> BORRAR

    rc=0   simbolos con WGridManagedDynamicElem: 0

`WGridNode.h` es la dependencia de verdad: sin ella la cabecera no compila
(`WGridNodeElemTag' was not declared`, líneas 37-39, 65 y 69 de `WCollisionAssets.h`), y es
lo único que estaba sacando de `WGridManagedDynamicElem.h`.

**Por qué esto reproduce el original.** Orden de enlace:

    zGameplay 8019A26C < zMisc 801FAB64 < zPhysics 8020DF98 < zSim 802728EC < zWorld2 802ECA00

Quien incluye `WCollisionAssets.h`: `zGameplay` (`GRaceStatus`, `GRaceDatabase`, `GTrigger`),
`zMisc` (`ResourceLoader.cpp`), `zPhysics` (`SmackableTrigger.cpp`) y `zWorld2`
(`WCollisionAssets.cpp`, `WCollisionMgr.cpp`, `WTrigger.cpp`, `WWorld.cpp`). Las **tres
primeras enlazan antes que `zSim`** — y son exactamente las tres unidades que la r54 midió
envenenadas por este símbolo (−852.004 / −499.872 / −388.752). Quitado el include, los únicos
definidores que quedan son `Simulation.cpp` (**`zSim`**, que incluye la cabecera
directamente, `Simulation.cpp:88`) y los `.cpp` de `zWorld2`, que enlaza después: **gana
`zSim`, que es el dueño del original**.

Un cambio de dos líneas en una cabecera que toca `zGameplay` + `zMisc` + `zPhysics` a la vez.
**No lo he aplicado: esta ronda es de medida, y es cabecera compartida.** Es un paquete
atómico de cinco unidades (las tres más `zSim` y `zWorld2`) y hay que verificarlo con
`movidos`/`trypromo` en las cinco.

Después de eso, lo que queda en `zGameplay` son 376.159 B difusos.

---

## 6. Sorpresas

1. **`reorden.py` no enmascara las reubicaciones de palabra entera**, y por eso marca como
   «CONTENIDO» los punteros de una vtable cuyo destino se ha movido. Las 3 palabras de
   `.rodata` de `zGameplay` son eso, comprobado una a una: `A` y `B` apuntan al **mismo
   nombre** en distinta dirección. En una unidad con vtables, el número «CONTENIDO» de
   `.rodata` es un techo, no una cuenta. **Arreglo**: `mascara()` no tiene caso para las
   secciones de datos; habría que comparar `.rodata` contra la tabla de reubicaciones, o como
   mínimo tratar como reubicación toda palabra que sea una dirección válida en los dos
   enlaces y cuyo símbolo destino tenga el mismo nombre.

2. **La resta M4 de `movidos.py` sobre-declara el robo de `.text` por un factor de 100.**
   Dice `-12144 de otro`; verificado símbolo a símbolo, **el robo real es 120 B**. La resta
   confunde tres cosas distintas, y aquí están las tres:

   * nuestro `.o` tiene **153.616 B** de `.text` contra los **141.472 B** del original: +12.144;
   * **4.652 B** en 129 símbolos que el enlazador **no mete en el enlace** (nadie los
     referencia): no se los quita a nadie, se van a la basura;
   * **7.792 B** en 67 símbolos que **otro objeto también define y sigue suministrando**: su
     dirección es **idéntica** en los dos enlaces (`reserve__...vector<WCollisionTri>` en
     `8003B72C`, `_Rebalance__...` en `8003D378`, `_._13IVehicleCache` en `80042694`…).
     Duplicados nuestros que pierden, no bulto robado;
   * **120 B**, un símbolo, el único que **cambia de dirección al entrar en nuestra ventana**.

   Lo mismo en `.rodata`: dice `-488 de otro`, y son **100 B** en 4 símbolos, los cuatro a la
   misma dirección en los dos enlaces (`_vt.13IVehicleCache`, `_vt.Q33UTL3COM8IUnknown`,
   `_vt.3MD5`, `lbl_803EBE90`). **Arreglo**: el criterio correcto no es una resta de tamaños,
   es «símbolo que nuestro `.o` define, presente en los dos enlaces, y cuya dirección en el
   enlace base cae **fuera** de la ventana de la unidad **y cambia**». Cuesta un bucle sobre
   los dos ELF que ya se enlazan.

3. **El censo de dueños de la r54 da `24` para `zGameplay` y el bueno es `1`.** El umbral
   «salto > 20 kB» cuenta como cambio de dueño 23 permutaciones **internas** de la unidad:
   `GetCacheName__C11GRaceStatus` (−122.352), los seis `Set*__11GRaceStatus` (−104.3xx), los
   `*Timers__8GManager` (−25.200), `RefreshProgress__8GRaceBin` (−21.372), los
   `Get*__C11GRaceCustom` (−20.888)… Todos son símbolos propios de `zGameplay`, presentes en
   su ventana en los dos enlaces. La amplitud no distingue M1 de M4; la **ventana** sí. Esto
   obliga a releer la columna `dueño` de la tabla del bloque A (`zFe` 47, `zFe2` 49) como
   **cota superior**, no como cuenta.

4. `build_direct.py` con `zGameplay` **no falló** — el aviso de fallo intermitente del brief
   no se reprodujo aquí.

## Sondas

En el scratchpad de la sesión, no en `scripts/`; todas de sólo lectura sobre los dos ELF
enlazados y los `.o`, más dos compilaciones privadas de una TU de una línea con las cflags de
`zGameplay`. Borradas al terminar. Ninguna escribe en el árbol.
