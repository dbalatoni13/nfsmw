# r58 — agente `anim`: zAnim, **4.214 -> 2.697 B**, y el ORDEN queda CERRADO

Unidad: `Speed/Indep/SourceLists/zAnim` (propiedad exclusiva).
`.o` final `538cf73f42a53d71e15eececbce4355a07518f7b`, **tres compilaciones con el mismo sello**.

    fncmp       ANTES 0 de 315 con el codigo distinto   DESPUES 0 de 315  -- cero regresiones
    linkdelta   .text +0   resto IGUAL   (las nueve secciones, antes y despues)
    dolwhere    4.214 -> 2.697 B     (-1.517 B, el 36 % del residuo)
    trypromo    DOL ROTO -- no promociona: lo que queda es POOL DE CADENAS

## Lo que se cierra del todo

| | antes | despues |
|---|---:|---:|
| vtables fuera de sitio (en el ENLACE) | **913 B**, 20 de 21 permutadas | **0 B, las 21 en su direccion EXACTA** |
| funciones fuera de sitio (`permorden`) | 1 desplazada, ciclo de 27 | **315 de 315 en su sitio, 0 ciclos** |
| palabras de codigo que apuntan mal | 1.131 B | 551 B |
| pool de cadenas | 2.170 B | 2.146 B |

**zAnim ya no tiene ni un problema de ORDEN.** Todo lo que queda son 2.146 B de contenido
de `.rodata` que nadie ha escrito, mas los 551 B de codigo que lo apuntan.

---

## 1. Las vtables: 913 -> 0 B, y el metodo es aritmetica, no tanteo

La regla de `nfsmw-orden-vtables-inverso` se sostiene entera en zAnim: **el orden de emision
es el INVERSO del de completado de clase**, y el completado se mide con `-E` en segundos.
`parseord.py zAnim` da **CONTROL FALLIDO** y su CIFRA no vale — pero su TABLA si, y la unica
clase que rompe el control es `IControlScenario`, cuya vtable sale en una vuelta posterior de
`finish_file` (esta 800 B detras del bloque, **igual en los dos lados**).
**Ignorando esa, la regla acerto 17 de 17.**

Orden de parseo del objetivo (= vtables al reves) y de donde sale cada clase:

    1  CAnimProperty              AnimScene.hpp        -> sacada a AnimProperty.hpp
    2  FnDefaultAnimBank          AnimBank.cpp
    3  CAnimBank                  AnimBank.hpp
    4  CNFSAnimBank               AnimBank.hpp
    5  CAnimChooser               INIS.h -> AnimChooser.hpp
    6  NISListenerActivity        AnimChooseArrest.cpp:229  (clase local: no se puede mover)
    7  CAnimPlayer                AnimPlayer.hpp
    8  IAnimEntity                AnimEntity.cpp -> AnimEntity_BasicCharacter.hpp
    9  CBasicCharacterAnimEntity  idem
    10 CPropAnimEntity            AnimEntity_Prop.hpp
    11 CAnimSceneData             AnimScene.hpp
    12 CAnimScene                 AnimScene.hpp
    13 CWorldAnimEntity           AnimEntity_WorldEntity.hpp
    14 WorldAnimEntityTreeInfo    idem
    15 CWorldAnimEntityTree       idem
    16 CAnimWorldScene            AnimWorldScene.hpp
    17 GenericNISControlScenario  GenericNISControlScenario.hpp

Cuatro ediciones, aplicadas **en el orden del objetivo** (regla de `parseord`: coloca primero
la que va mas arriba, o mover una que arrastra a otras EMPEORA):

1. **`AnimProperty.hpp` (nueva)**: `CAnimProperty`, el `enum eAnimProperty` y
   `typedef int AnimHandle` salen de `AnimScene.hpp`. `zAnim.cpp` la incluye **antes que
   nada**, que es lo que pone `CAnimProperty` en la posicion 1.
2. **`AnimEntity_WorldEntity.hpp`**: `WorldAnimEntityTreeInfo` y `CWorldAnimEntityTree`
   intercambiadas — el objetivo las completa al reves de como estan escritas. La declaracion
   adelantada de `CWorldAnimEntityTree` ya estaba, asi que el intercambio no necesita nada mas.
3. **`INIS.h`**: deja de arrastrar `AnimScene.hpp`. Solo usa `CAnimScene *`, o sea que basta
   una declaracion adelantada. Sin esto, `IAnimEntity`/`CAnimSceneData`/`CAnimScene` se
   completan **antes** que `NISListenerActivity`, y desde `AnimChooseArrest.cpp` no hay
   arreglo posible: la clase local esta en la linea 229 y el `#include` en la 17.
4. **`AnimPlayer.hpp`**: deja de arrastrar `AnimScene.hpp` y `AnimWorldScene.hpp`, que
   completaban SIETE clases antes que `CAnimPlayer`.

Las ediciones 3 y 4 (y la mitad de la 2) van **bajo guarda de macro que solo define
`zAnim.cpp`** — `INIS_NO_ANIMSCENE` y `ANIMPLAYER_NO_SCENE_INCLUDES`. `INIS.h` lo incluyen 29
ficheros mas y `AnimPlayer.hpp` otros nueve; con la guarda, el texto que ve `cc1plus` en las
otras catorce unidades **no cambia ni un token**. Ver §4.

## 2. El primer de cola: una funcion muerta vale 1 funcion y 613 B

Con las vtables colocadas quedaba **una** funcion desplazada, y arrastraba un ciclo de 27:

    obj#285  _IHandle__14INISCarControl      <- la PRIMERA inline de cola del objetivo
    nue#311  _IHandle__14INISCarControl      <- la ULTIMA de la nuestra

`_IHandle()` es un cuerpo **EN CLASE** (`DECL_INTERFACE`), o sea cola de `finish_file`, y
dentro de la cola manda el **PUNTO DE USO**. Nuestro unico uso esta en `AnimScene.cpp:727`
(el `#include` numero 17 de zAnim.cpp) y el de `INIS`/`INISLISTENER` en `AnimChooseArrest.cpp`
(el numero 3). El primer muerto va **justo detras de `AnimBank.cpp`**, porque el objetivo la
emite detras de las inlines de `FnDefaultAnimBank`:

    static HINTERFACE _zanim_r58_prime_iniscarcontrol() {
        return INISCarControl::_IHandle();
    }

`permorden` pasa de `314 de 315 / ciclo de 27` a **`315 de 315, 0 ciclos`**, y `dolwhere` de
3.310 a 2.697 B. El enlazador se lleva la funcion muerta; `INISCarControl.h` solo arrastra
`UCOM.h` y `UTypes.h`, asi que **la cabecera no cuesta nada** — que es lo que suele matar a
los primers.

## 3. El `$LC` que se movio, y por que la solucion NO es `lcfix`

Reordenar los `#include` **renumera los `$LC`**, y `keep.lst:1420` (`zAnim.o:$LC437`, la
cadena muerta `"Unnamed MyEAGLNewOverride"`) se quedo rancio. `-strip-unused-data` se lleva
`size & ~7` = **24 B** y la `.rodata` sale corta:

    keep.lst tal cual              rodata -24   ->  dolwhere  314.838 B   (roto en silencio)
    keep.lst con el lcfix          IGUAL        ->  dolwhere    2.721 B

**No he dejado la unidad dependiendo de eso.** El `asm()` de `zAnim.cpp` ya pega bytes
verbatim del objetivo colgados de `lbl_803CF388` (truco de la r57: `keep.lst` empareja por
NOMBRE, no por tamano), asi que alargarlo de `0x554` a `0x56C` con los **24 B siguientes del
objetivo** (`"ArrestM14"`/`"ArrestF14"`, 0x803CF8DC) compensa la cadena estripada **con
contenido correcto y en su sitio exacto**:

    keep.lst tal cual + asm 0x56C  IGUAL        ->  dolwhere    2.697 B   <- lo que dejo

Es **24 B mejor** que corregir el `keep.lst`, y no depende de nadie.

> **AVISO PARA EL CIERRE — la correccion de `lcfix` de zAnim NO SE DEBE APLICAR.**
> `lcfix.py --check` sigue cantando `linea 1420: zAnim.o:$LC437 -> zAnim.o:$LC424`.
> Aplicarla revive la cadena y deja la `.rodata` **+24**, o sea rompe la unidad.
> Medido: con la entrada **borrada** el resultado es identico al de dejarla rancia
> (`IGUAL`, 2.697 B). **La propuesta es BORRAR las lineas 1419-1420 de
> `config/GOWE69/keep.lst`:**
>
>     # @lc zAnim "Unnamed MyEAGLNewOverride"
>     zAnim.o:$LC437
>
> Es la unica peticion de `keep.lst` que dejo, y el aviso esta tambien en un comentario de
> `AnimEngineManager.cpp`, donde `previo.py` lo encuentra.

## 4. Regresiones: CERO, y medidas objeto a objeto

Cabeceras tocadas que no son solo de zAnim, y quien las ve:

| cabecera | unidades ajenas |
|---|---|
| `Interfaces/SimActivities/INIS.h` | 29 ficheros, 14 SourceLists |
| `Animation/AnimPlayer.hpp` | 9 ficheros ajenos |
| `Animation/AnimScene.hpp` | via `INIS.h`, las mismas 14 |
| `Animation/AnimEntity_WorldEntity.hpp` | via `AnimWorldScene.hpp` / `AnimWorldTypes.hpp` |

**Metodo** (el de la r57, endurecido): compilar cada unidad ajena a un `.o` **PRIVADO**
—nunca `build/GOWE69/src`, que es de su agente— con mis cabeceras y con las de HEAD, y
comparar **secciones asignables y el valor de CADA simbolo**, ignorando `.debug`/`.line`.

    zAI zCamera zEAXSound zEAXSound2 zFe zFe2 zGameplay zMain zMisc
    zPhysics zPhysicsBehaviors zRender zSim zWorld

    -> 14 de 14 IGUAL: 0 secciones distintas, 0 simbolos con otro valor.

**Y una regresion REAL cazada y arreglada por el camino.** La primera version metia
`#include "AnimScene.hpp"` **sin guarda** en `AnimEntity_WorldEntity.hpp` (zAnim lo necesita
porque `INIS.h` ya no la trae). Eso hacia crecer **zFe2** de `rodata 25.504` a `25.624`
— **+120 B**, con 1.230 simbolos desplazados. Con la guarda
`#ifdef ANIMPLAYER_NO_SCENE_INCLUDES`, zFe2 vuelve a ser identico byte a byte.

**Leccion de medida**: la primera comprobacion que hice fue comparar el TEXTO PREPROCESADO, y
dio «identico» en las 14 — pero solo porque habia revertido dos de las cuatro cabeceras.
**Comparar el preproceso es util pero hay que revertirlo TODO**; lo que no miente es compilar
y comparar el objeto.

## 5. Lo que queda: 2.697 B, y es un PAQUETE ATOMICO

Todo esta en la ventana de `.rodata`. El mapa completo esta transcrito en la cabecera de
`SourceLists/zAnim.cpp` (donde `previo.py` lo encuentra) y se regenera con

    python scripts/agent_anim_gaps.py zAnim 803CF3A4 803D0220

Tres piezas:

**(a) 780 B de DUPLICADO.** El `asm()` pega verbatim el prefijo del objetivo
(0x803CF3A4..0x803CF8F4) y **`cc1plus` vuelve a internar las mismas cadenas** mas abajo
(`EAGL4::*`, `Attrib::*`, `MPerpBusted`, `Arrest*`). El propio fichero ya resuelve el caso
pequeño con `extern const char _bwarePrefix[] asm("lbl_803CF348")` para `"GAMECUBE"`,
`"bad_alloc"` y `"STL"`: hay que hacer lo mismo con el resto del bloque. **Es la pieza grande
y sin ella no cuadra nada.**

**(b) once bloques que FALTAN, 1.031 B**, con su direccion del objetivo y delante de que
literal nuestro van. Los dos gordos son fuente que **no existe en el arbol**:

    803CF9E4  276 B  "%s%s," + los ocho AC_*                      -> AnimCtrl.cpp ($LC417)
    803D001C  340 B  "GenericNISControlScenario" + los ocho WAC_* -> WorldAnimCtrl.cpp

y los otros nueve son nombres de depuracion de `operator new` (`CAnimScene`,
`CAnimWorldScene`, `Anim_CAnimPart_SlotPool`, `VehicleParams`, `CAnimEntityData`,
`CAnimSceneData`, `CAnimProperty`, `CAnimResourceFileProxy`) y bloques de floats. Un `asm()`
**ANONIMO** los pone sin tocar `keep.lst` (un hueco sin simbolo es inestripable), y colocarlos
es fino porque el pool de una funcion sale pegado a la funcion: un `asm()` entre dos funciones
cae entre sus dos pools.

**(c) los CADAVERES, y por eso (a)+(b) no se pueden hacer a trozos.** `-strip-unused-data` se
lleva `size & ~7` y **deja el resto**: cada literal muerto que emitimos y `keep.lst` no protege
deja de 1 a 7 bytes donde el objetivo no tiene nada (`"ssis"` es la cola de
`Attrib::Gen::chassis`, `"uction"` la de `Attrib::Gen::induction`, y hay `"TODO"`, `".cpp"`,
`"cpp"`). **Un cadaver no se arregla con un `asm()`**: o se mantiene la cadena entera, o no se
emite. Medido: sin las 28 entradas `@lc` de zAnim el pool baja a 1.916 B pero aparecen
**459 B de cadaveres**; con ellas hay 563 B de cadenas completas **en el sitio equivocado**.
Ninguna de las dos configuraciones cuadra sola.

O sea: cerrar el pool de zAnim es **una edicion atomica** que toca (a), (b) y (c) a la vez, mas
unas 19 lineas de `keep.lst`. No es media tarde — pero **ya no hay nada mas que descubrir**:
el mapa esta al byte.

## 6. Herramientas nuevas (todas `scripts/agent_anim_*.py`)

| | |
|---|---|
| `agent_anim_gaps.py` | alinea la ventana de `.rodata` POR LITERAL: que falta, cuanto, y **delante de que simbolo nuestro** va. Con `--asm` lo escupe listo para pegar |
| `agent_anim_dw.py` | `dolwhere` con un `keep.lst` ALTERNATIVO — imprescindible para medir un cambio de `$LC` sin tocar el fichero compartido |
| `agent_anim_priv.py` + `agent_anim_cmp.py` | compila unidades ajenas a un `.o` privado y compara secciones asignables y valor de cada simbolo. **Es la prueba de la regla 5**, y es lo que caza el caso zFe2 |
| `agent_anim_syms.py` | direcciones ENLAZADAS de los simbolos de un rango (asi se ve el orden REAL de vtables, no el del objeto) |
| `agent_anim_stack.py` | pila de `#include` de cada clase con vtable, sobre el `-E` de `parseord` |
| `agent_anim_dump.py` | vuelca la ventana de `.rodata` de los dos lados como lista de cadenas |
| `agent_anim_pp.py` / `agent_anim_aff.py` | preproceso privado y censo de unidades afectadas por una cabecera |

## 7. Sorpresas

1. **`vtord.py` compara el OBJETO y por eso miente**: dice «13 de 24 no casan» con las 21
   vtables enlazadas en su direccion exacta. Las dos de mas (`CAnimMarker`,
   `UTL::COM::IUnknown`) **las estripa el enlazador**, y eso corre todos los indices. Lo que
   hay que mirar son las **direcciones enlazadas**.
2. **`parseord.py` con CONTROL FALLIDO sigue sirviendo**: la cifra no vale, pero la TABLA si.
   Aqui la unica clase que rompia el control era `IControlScenario`, que sale en otra vuelta de
   `finish_file` — y sale igual en los dos lados. Con ella fuera, 17 de 17.
3. **Un `#include` sin guarda en una cabecera compartida cuesta 120 B en otra unidad**, y el
   sintoma no es un fallo de compilacion: es `.rodata` mas grande y 1.230 simbolos desplazados.
   Solo se ve compilando la unidad ajena.
4. **Comparar el preproceso puede dar un falso «cero regresiones»** si no has revertido TODAS
   las cabeceras que tocaste. Me dio 14 de 14 identicas con una regresion real dentro.
5. **Alargar un `asm()` colgado de un `lbl_` de `keep.lst` es mejor que arreglar el
   `keep.lst`**: cuesta lo mismo, pone bytes del objetivo en su sitio, y **la unidad deja de
   depender de que alguien aplique un `lcfix`**.
