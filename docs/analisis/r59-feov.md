# r59 - agente `feov`: zFeOverlay, zFe y zFe2

    unidad        dolwhere ANTES   DESPUES    delta    linkdelta   fncmp
    zFe               37.472 B    35.855 B   -1.617    +0 IGUAL    0 de 921
    zFe2              47.147 B    47.147 B        0    +0 IGUAL    0 de 1307
    zFeOverlay         6.211 B     6.211 B        0    +0 IGUAL    0 de 467

    sha1 sellado en TRES compilaciones seguidas, identico las tres:
       zFe.o         efafd739a506462935022399750fa62d66080bff
       zFe2.o        4b40d3ef01154ce3df198375eeeda857d3ac0624
       zFeOverlay.o  a3a4619f236e20448e3cf8114011362106ea22c4
    trypromo: DOL ROTO (zFe e992bc1b0879, zFe2 8df18fee0d00, zFeOverlay a3a1d3536a97;
              los tres juntos d521e4540639).
    lcfix.py --check: 242 pendientes, NINGUNA mia y NINGUNA venenosa para mis unidades
              (50 zAI, 62 zSpeech, 39 zWorld, 30 zPhysicsBehaviors, 21 zCamera,
               19 zWorld2, 13 zPlatform, 5 zAnim, 3 zGameplay).
    mangfix zFe2 --check: sin alias desfasados.  gapchk (las tres): 0.
    prefijochk y checksplits: LIMPIO.
    config/GOWE69/*, splits.txt, keep.lst y configure.py SIN TOCAR.

---

## 1. Lo que ha pagado: `FEngInterfaceFEStrings`, y con el la r56 queda REFUTADA

`vtord zFe` daba **130 vtables descolocadas de 157**. Ahora da **111**, y los puestos
**9 a 38 casan EXACTOS** (antes casaban 9 a 16). El encargo pedia comprobar si el
corrimiento de dos posiciones era real: **lo era**, y la causa no era la que decia el
encargo.

### 1.1 NEGATIVO: quitar el `inline` NO mueve una vtable

El encargo decia que `FEngGroupFEPrintf`/`FEngSetGroupLanguageHash` «se fueron al final en
la r57 al ponerlas `inline`». Medido: **falso**.

    quitar solo el `inline` de las dos Callback (clases en la cabecera):
        vtord    130 -> 130   (las dos vtables NO se mueven del puesto 146/147)
        dolwhere 37.472 -> 87.576 B

Revertido, y el negativo anotado en `FEngInterfaceFEStrings.cpp`. Lo que coloca una vtable
en GCC 2.9 es **donde se COMPLETA la clase**, no donde se define su metodo clave.

### 1.2 Lo que si las coloca: bajar las clases al `.cpp`

`parseord zFe` (aunque su control falle, la TABLA sirve) dice que el objetivo completa
`FEngSetGroupLanguageHash` en el puesto **130** y `FEngGroupFEPrintf` en el **131**, entre
`ObjectVisibilitySetter` (129) y `cFEngGameInterface` (132). En el arbol se completaban en
el **8** y el **9**, porque su cabecera la arrastra `uiMain.cpp`, que es el primer
`#include` de `zFe.cpp`.

Bajadas las dos clases a `FEngInterfaceFEStrings.cpp` --con el `Callback` DENTRO de la
clase, y `FEngGroupFEPrintf` detras de `DoFEngPrintf` porque lo llama--:

    vtord    130 -> 135 (pero los puestos 9..27 pasan a EXACTOS)
    dolwhere 37.472 -> 36.679 B

### 1.3 LA R56 ESTABA EQUIVOCADA, y esto vale para todo el proyecto

`r56-fe.md` §5 midio esta misma edicion en **-43.876 B** y la dejo **sin aplicar** por esto:

> «`FEngInterfaceFEStrings.hpp` la parsean 17 SourceLists. Quitarle dos clases desplaza el
> contador de nombres privados de GCC en -12 en todas ellas. Y `keep.lst` tiene cuatro
> entradas con nombre privado, todas ajenas: `zEAXSound2.o:LastRandom.25127`,
> `zWorld.o:changetime.14948`, `zWorld.o:index.14943`, `zWorld.o:windState.14947`. Las
> cuatro quedarian apuntando a otra cosa. **No es mio para romperlo.**»

**El desplazamiento de -12 es real** (medido: `AutoParameterAccessors.25908` -> `.25896`,
`_.tmp_1.22581` -> `.22569`). **La consecuencia es falsa.** Esas cuatro entradas nombran a
los objetos ORIGINALES EXTRAIDOS, no a los nuestros:

    build/GOWE69/obj/.../zWorld.o       tiene index.14943, windState.14947, changetime.14948
    nuestro zWorld.o (HEAD)             tiene windState.25670, changetime.25671
    nuestro zWorld.o (con mi edicion)   tiene windState.25658, changetime.25659

O sea: **`keep.lst` nunca ha casado con nuestro zWorld, ni antes ni despues**, y el enlace
base usa el objeto extraido, cuyos nombres no cambian nunca. La regla general:

> **Una entrada de `keep.lst` con nombre privado (`nombre.NNNNN`) esta escrita contra el
> objeto ORIGINAL. El contador de NUESTRO compilador no la afecta mientras la unidad no
> este promocionada -- y si lo estuviera, ya estaria rota hoy.**

Eso reabre el eje entero de «sacar clases de una cabecera compartida», que la r56 veto.

### 1.4 Regresiones (regla 5): CUATRO unidades ajenas medidas, CERO regresiones

Metodo del `.o` privado (`privo.py`, en el scratchpad; nunca a `build/GOWE69/src`), con la
cabecera de HEAD y con la mia, **compilaciones consecutivas** y con el `sha1` del
SourceList comprobado antes y despues para descartar que otro agente lo moviera:

    unidad        secciones   simbolos comunes   con VALOR distinto   $LC afectados
    zWorld         IGUALES          2.787                 0                0
    zEAXSound2     IGUALES          2.738                 0                0
    zAI            IGUALES          3.322                 0                0
    zMain          IGUALES          3.128                 0                0

Lo unico que cambia son los nombres locales privados (79, 67, 74 y 366 simbolos, todos
`nombre.NNNNN` renumerados en -12). **Ni un `$LC` en la diferencia**, o sea que las 1.256
entradas `$LC` de `keep.lst` no se ven afectadas por este eje --lo que la r56 sospechaba con
una sonda y aqui queda comprobado en cuatro unidades--.

**La UNICA victima real fue `zFe2.cpp`**, que es mia: sus 64 alias `asm()` mangled con
numero dejaron de casar y la unidad no enlazaba (`L0039: Reference to undefined symbol`).
`mangfix zFe2` los recoloco (-12 en los 16 alias) y zFe2 vuelve a `IGUAL` y a 47.147 B.
**Barrido el arbol entero: `zFe2.cpp` es el unico fichero con alias mangled numerados**
(los otros seis ficheros que contienen `nombre.NNNNN` son comentarios, salvo
`Geometry.cpp:589`, cuyo `algos.3467` se define en el mismo `asm()` y no depende del
contador).

## 2. Lo otro que ha pagado en zFe: dos cabeceras que llegaban tarde

Las dos son puro orden de PARSEO, sin mover un literal (`linkdelta` IGUAL, `fncmp` 0,
`lcfix` sin correcciones de zFe):

| edicion | puesto de parseo obj / nuestro | vtord | dolwhere |
|---|---|---|---|
| `#include FEAnyMovieScreen.hpp` en `zFe.cpp`, delante de `uiPause.cpp` | 48 / 95 | 135 -> 130 | 36.679 -> 36.506 |
| `#include FEGameWonScreen.hpp` DENTRO de `uiPause.cpp`, detras de `uiPause.hpp` | 50 / 122 | 130 -> 111 | 36.506 -> **35.855** |

La segunda solo se alcanza desde dentro de `uiPause.cpp`: el objetivo completa
`FEGameWonScreen` **entre** `PauseMenu` (49, de `uiPause.hpp`) y `ArrayScripts` (51, de
`feArrayScrollerMenu.hpp`, que llega por `FEPkg_PostRace.hpp` mas abajo en ese mismo
fichero). `uiPause.cpp` solo lo incluye `zFe.cpp`.

## 3. El techo de zFe, medido y anotado en la fuente

Lo que queda de `vtord zFe` (111 de 157) es, casi entero, **un bloque**: el objetivo
completa las seis clases de `uiMain.cpp` (`MainCareer`, `Challenge`, `MainQuickRace`,
`MainCustomize`, `MainProfileManager`, `MainOptions`) en los puestos **39..44** --detras de
`uiOptionsMain.hpp` (38) y delante de `uiOptionsScreen.hpp` (45)-- y aqui salen en el
**8..13**, porque `#include uiMain.cpp` es la primera linea de la lista.

**MEDIDO**: bajar ese `#include` detras de `uiOptionsMain.cpp` las coloca de verdad
(`vtord` 111 -> 107, parseo 118 -> 108 de 149) **pero mueve los literales de todo
`uiMain.cpp` y RENUMERA los `$LC` de zFe**: `linkdelta` pasa de IGUAL a `rodata-312`,
`fncmp` pierde `_GLOBAL_.I.gOnlineMainMenu` y `lcfix --check` abre **82 correcciones de
zFe.o**. Con 153 entradas `$LC` de zFe en `keep.lst` eso es **trabajo de ventana**
(edicion + `lcfix` + `mangfix zFe2`), no de agente. Revertido y anotado en `zFe.cpp`.

Y un negativo mas pequeno, tambien anotado (en `uiOptionWidgets.cpp`): subir
`#include uiOptionsMain.hpp` a la linea 2 de ese fichero coloca `UIOptionsMain` en el
puesto de parseo 38 (el del objetivo) y deja las cuatro vtables de opciones en su orden
relativo bueno, pero **cero bytes**: los tres simbolos que se mueven suman lo mismo
(56+48+48) y el desplazamiento se compensa. **Solo paga junto con el bloque de arriba.**

## 4. zFeOverlay: su bloqueo esta CONTADO AL BYTE, y son tres numeros

`dolwhere` 6.211 B = **4.150 de pool** (803C6EB4..803C7EFC) + ~2.000 de punteros de
`.data`/`.text` que lo apuntan + 9 de orden de floats. El pool, con
`agent_feov58_pool.py 803C6EB4 803C8B40 0 zFeOverlay`:

    SOLO EN EL OBJETIVO : 44 cadenas, 664 B   (los 14 `SelectablePart N`,
                          CustomizeMainOption, CustomizePartOption, ArraySlot, CarDatum,
                          DebugCarOption, FEShoppingCartItem, ImageArraySlot, ...)
    SOLO EN NUESTRO     : 44 cadenas, 668 B   (26 duplicados del bloque de cabecera que
                          keep.lst resucita + 18 COLAS `N & 7` de cadenas muertas:
                          'uction', 'ct.cpp', 'hicle', 'stems', 'arams', ...)

Se compensan al byte: por eso `.over` mide EXACTO con el contenido cambiado.

**Los tres numeros para cerrarla, los tres medidos en esta ronda:**

1. **Borrar las 28 lineas `zFeOverlay.o:$LC*` de `keep.lst`** (1163..1214) libera
   **EXACTAMENTE 480 B**: `.over` 0x249A0 -> 0x247C0. Medido enlazando con una COPIA
   privada de `keep.lst` (no toque el fichero). **No son 668**: las colas `N & 7` se quedan.
2. **Las 44 cadenas que faltan valen 664 B y NO necesitan lineas nuevas de `keep.lst`.**
   Un `asm()` de `.rodata` SIN etiqueta sobrevive al estripado --lo prueba el bloque de 117
   cadenas del final de `zFe.cpp`, que no tiene `.globl` y cuenta entero-- y un `asm()` de
   ambito de fichero se emite EN EL PUNTO DEL PARSEO, o sea que puesto ENTRE dos `#include`
   de `zFeOverlay.cpp` cae entre los literales de esos dos `.cpp`. Cero `$LC` movidos.
   **Esto corrige la propuesta 2 de `r58-feov.md`, que pedia 44 lineas de `keep.lst`.**
3. **Faltan 184 B por cuadrar** (664 - 480). Salen de las **12 rutas `__FILE__` muertas**
   de la unidad (`agent_feov58_paths.py zFeOverlay 803C6EB4 803C7F00`): sustituir
   `__FILE__` por el nombre de tipo en su `BNEW` es gratis en `$LC` (r58 §3.1) y cambia una
   cola de 6-7 B por una de 3, doce veces.

O sea: **UNA edicion de `keep.lst` que solo BORRA**, 44 bloques `asm()` colocados por
`#include`, y 12 sustituciones de `__FILE__`. Es de ventana por el paso 1, y **no lo he
aplicado**: a medias deja la unidad +184 B y con el pool peor que hoy. Todo esto queda
escrito en un comentario en `zFeOverlay.cpp`, que es donde `previo.py` lo encuentra.

Y una correccion a la r58: el pool tiene **12** rutas `__FILE__`, no una por sitio de
`BNEW`. GCC 2.9 **si interna** el `__FILE__` dentro de una misma TU (una copia por `.cpp`),
asi que las 44 cadenas que faltan no se pueden sacar solo sustituyendo: 32 son literales
NUEVOS.

## 5. zFe2: su bloqueo es de DUENO, no de orden

`vtord zFe2`: el objetivo tiene **125** vtables y nosotros **130**. Las cinco de mas son
`8FEWidget`, `10IconOption`, `10ArrayDatum`, `12ArrayScripts` y `Q33UTL3COM8IUnknown`. Las
cuatro primeras el objetivo las emite en **zFe** (puestos 153..156 de su `.rodata`), no
aqui. Mientras esten, la `.rodata` de zFe2 va corrida entera y **los 47.147 B no bajan por
reordenar nada**: por eso esta unidad se ha quedado a cero esta ronda.

La causa esta a la vista en `feWidget.hpp`: `FEWidget` **no tiene metodo clave** --el
destructor es `{}` en clase, `Enable`/`Disable` tambien, y las demas virtuales son PURAS--,
asi que GCC 2.9 emite su vtable en toda TU que construya la clase, y `feWidget.cpp` (que
define el constructor) esta en esta SourceList. El arreglo es el reciproco de
`nfsmw-la-virtual-que-era-pura`: darle una virtual no-pura definida fuera de linea en un
`.cpp` que solo parsee zFe. **No lo he probado** porque `Enable`/`Disable` estan en linea y
sacarlas cambia el codigo de muchas unidades ajenas. Anotado en `zFe2.cpp`.

## 6. Sorpresas

### 6.1 Un `asm()` de `.rodata` SIN etiqueta no se estripa

`-strip-unused-data` va por SIMBOLO, y los bytes que no cubre ningun simbolo se quedan. El
bloque de 117 cadenas del final de `zFe.cpp` (r52-jf) no tiene `.globl` ni etiqueta y cuenta
entero en la `.rodata` de la unidad, que mide IGUAL. Contradice el resumen «un hueco anonimo
se estripa entero», y es lo que hace viable el paso 2 del §4 **sin tocar keep.lst**.

### 6.2 Sellar el `sha1` DESPUES de anotar, no antes

Anadir comentarios a un `.cpp` cambia el `sha1` del `.o` (informacion de lineas) sin mover
un byte del DOL: comprobado en `zFe.o` (`dolwhere` identico antes y despues). **El sellado
va sobre el estado FINAL de la fuente, no a mitad de las anotaciones.**

### 6.3 `build_direct.py zFe` compila TRES unidades

Ya lo aviso la r58 (§6.2) y sigue: empareja por subcadena. `zFe.cpp` si desambigua.

### 6.4 Colision de propiedad: otro agente ha editado una cabecera de `Frontend/`

`src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp` sale
modificado con un comentario firmado **`r59 (anim)`** (`CARCUSTOMIZE_EMPTY_STRING`). El
encargo me daba «Cabeceras de `Frontend/`» en propiedad EXCLUSIVA y `CarCustomize.cpp` esta
en zFeOverlay, que es mia. **No lo he revertido** --su valor por defecto es `""`, o sea el
codigo de antes, y lo he medido: `zFeOverlay` sigue en 6.211 B y `linkdelta` IGUAL-- pero el
reparto de la ronda tiene dos duenos para ese arbol.

### 6.5 El arbol se sigue moviendo, y una medida salio envenenada

Mi primera comparacion de `zWorld` daba `.rodata` -472 B y 1.016 simbolos desplazados: no
era mi edicion, era **otro agente alargando `lbl_80409D58` de 0x5C a 0x234 entre mis dos
compilaciones**. Se caza sellando el `sha1` del fuente AJENO antes y despues de las dos
compilaciones, no solo el del `.o`.

## 7. Propuestas para la ventana

1. **zFeOverlay** (§4): borrar 28 lineas de `keep.lst`, 44 bloques `asm()` anonimos
   colocados por `#include`, 12 sustituciones de `__FILE__`. Los tres numeros estan medidos
   (480 / 664 / 184) y el saldo cierra. Es lo unico que le queda de tamano a la unidad.
2. **zFe** (§3): `#include uiMain.cpp` detras de `uiOptionsMain.cpp` + `lcfix` (82
   correcciones de zFe.o) + `mangfix zFe2`. `vtord` 111 -> 107 y es la puerta del bloque
   `MainOptions..MainCareer`, 6 vtables y todo lo que va detras.
3. **Para el dossier y la memoria**: la nota de `r56-fe.md` §5 --y con ella el veto a sacar
   clases de cabeceras compartidas por los cuatro `keep.lst` de nombre privado-- **esta
   REFUTADA** (§1.3). Esas entradas nombran el objeto EXTRAIDO.
4. **zFe2** (§5): las cinco vtables de mas son el bloqueo. Antes de tocar `feWidget.hpp` hay
   que medir con `.o` privado las unidades ajenas que la parsean.

## 8. Ficheros tocados (ninguno en `scripts/`, ninguno en `config/`)

    src/Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp   clases fuera
    src/Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.cpp   clases dentro
    src/Speed/Indep/SourceLists/zFe.cpp                    +1 include, +1 negativo anotado
    src/Speed/Indep/Src/Frontend/MenuScreens/InGame/uiPause.cpp              +1 include
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.cpp
                                                                            solo el negativo
    src/Speed/Indep/SourceLists/zFe2.cpp                   mangfix (-12) + el hallazgo §5
    src/Speed/Indep/SourceLists/zFeOverlay.cpp             solo el plan medido del §4

Las sondas de la ronda (`privo.py`, `pooldump.py`, `link2.py`) se quedan en el scratchpad.
`link2.py` --enlazar con un `keep.lst` PRIVADO y volcar tamanos de seccion-- es la que
merece entrar en `scripts/`: es lo que convierte «propongo tocar keep.lst» en una medida.
