# r60 - agente `feov`: zFeOverlay CIERRA (depende de 89 lineas NUEVAS en keep.lst)

    unidad        dolwhere ANTES   DESPUES    linkdelta ANTES  DESPUES   fncmp
    zFeOverlay        6.211 B       0 B *        +0 IGUAL      +0 IGUAL   0 de 467

    (*) con las 89 lineas nuevas de `keep.lst`.  Con el `keep.lst` de HOY la unidad
        mide `over-544` y `trypromo` da DOL ROTO (4f5f6957d106).

    sha1 sellado en TRES compilaciones LIMPIAS seguidas (borrando el .o antes),
    identico las tres:  zFeOverlay.o  795b62f8d584066d1e9a9aae16f1245d81802b75

    trypromo (RUTA COMPLETA, keep.lst + las 89 lineas):  DOL OK  x3
    trypromo (keep.lst de hoy):                          DOL ROTO

    Comparacion byte a byte del ELF enlazado contra la base, TODAS las secciones:
        .over 0 B   .data 0 B   .rodata 0 B   .text 0 B   .sdata 0 B
        .sdata2 0 B .ctors 0 B  .bss: 0 simbolos en otra direccion
    (antes: .over 6.211 B en 208 rangos, .data 32 B, 3 simbolos de .bss movidos)

    lcfix.py --check: 311 correcciones pendientes -- 142 zFe, 84 zLua, 43 zSpeech,
        42 zAI.  NINGUNA es mia y NINGUNA es venenosa para zFeOverlay.
        Ademas salen 28 `FALLO zFeOverlay: '<cadena>' no tiene simbolo $LC propio`:
        son las 28 lineas viejas de la unidad, que mis guardas dejan sin objeto.
        **MEDIDO: son inofensivas.** El enlace da DOL OK con ellas y sin ellas, asi
        que la edicion de `keep.lst` es puramente ADITIVA (ver seccion 6).
    gapchk 0 | prefijochk LIMPIO | checksplits LIMPIO | mangfix sin alias desfasados.
    config/GOWE69/*, splits.txt, keep.lst y configure.py SIN TOCAR.
    Ninguna sonda en `scripts/`: todas en el scratchpad `feov60/`.

---

## 1. Lo que estaba mal en el diagnostico de la r59

La r59 dejo la unidad "contada al byte" en tres numeros (480 / 664 / 184) y la
declaro trabajo de ventana. Los tres eran correctos como aritmetica y **el plan era
inaplicable**: el paso 2 pedia meter las 44 cadenas que faltan con `asm()` anonimos
"colocados por `#include`", y esas cadenas **no caen entre dos ficheros**: caen
DENTRO del pool de una funcion, y GCC 2.9 emite el pool de cada funcion ENTERO y
DELANTE de su cuerpo (comprobado en el `.s`). Un `asm()` de ambito de fichero solo
alcanza los huecos ENTRE pools.

Lo que si funciona es lo contrario de lo que decia el plan: **que las emita el
compilador, en su sitio exacto, y salvarlas con `keep.lst`** -- que es justo para lo
que existe `deadstr.py`.

## 2. La palanca grande: las guardas de pool a mano que esta unidad no tenia

zFeOverlay escribe a mano su bloque de cabecera desde la r57, pero **nunca definio
las guardas que impiden que cc1plus interne ADEMAS su copia** de cada cadena del
bloque. Las tienen zAnim (r59), zRender, zMisc, zSim, zAI y zFEng; aqui faltaban
las cinco:

    extern const char _zFeOverlayPrefix[] asm("lbl_803c6a40");
    #define BWARE_PREFIX_GAMECUBE (_zFeOverlayPrefix)
    #define BWARE_PREFIX_BADALLOC (_zFeOverlayPrefix + 0x34)
    #define ATTRIB_TAGS_HAND_POOL 1
    #define ATTRIB_TAG_ORDER_HAND_POOL 1
    #define DEAD_STR(s) ((const char *)0)
    #define HAND_POOL_TAG(s) ((const char *)0)

Efecto medido de una sola edicion:

    .over  149.920 -> 149.280 B   (-640, o sea 640 B de basura que sobraban)
    la cabeza del pool (0x803C6EB4) pasa de 720 B de duplicados a 4 B

Las 28 lineas `zFeOverlay.o:$LC*` de `keep.lst` resucitaban exactamente 480 de esos
640 B (563 B de cadena menos 83 B de cadaveres `size & 7`, cuadra al byte).

**Esto es extrapolable**: cualquier unidad que escriba su prefijo a mano y no defina
las cinco guardas esta pagando lo mismo. Es un `grep` de un minuto por unidad.

## 3. Las 44 cadenas que faltaban eran ETIQUETAS DE TIPO de `BNEW`

`#define BNEW new (__FILE__, __LINE__)`. El objetivo **no pasa `__FILE__`**: pasa el
NOMBRE DEL TIPO. Se ve porque su pool tiene `TrackDirection`, `NumLaps`,
`TrafficLevel`, `NumOpponents`, `AISkill`, `CatchUp` exactamente en el orden de
PRIMER USO de `uiQRTrackOptions.cpp`, y ocupando el sitio donde nosotros teniamos la
ruta `__FILE__` de ese fichero.

Sustituidos los 116 sitios de `BNEW` de los 12 `.cpp` de la unidad por
`new ("<etiqueta>", __LINE__)`, las 44 cadenas salen **en su posicion exacta** y las
6 rutas `__FILE__` desaparecen. `fncmp` 0 de 467: el argumento no se usa (el
`operator new` en linea de bWare.hpp lo tira), asi que **no cambia una instruccion**.

Detalles que hay que copiar del objetivo, no deducir:

  * la etiqueta NO siempre es el nombre de la clase: la clase es
    `FrontEndRenderingCar` y la etiqueta `"FrontendRenderingCar"`;
  * `SelectablePart` lleva numero (`"SelectablePart 1"` .. `"SelectablePart 14"`),
    y el numero se asigna por el ORDEN DEL POOL DEL OBJETIVO, no por el sitio;
  * `mModels = new eModel[n]` tambien lleva etiqueta (`"FEGeometryModels"`).

### 3.1 Y por eso hacen falta lineas de `keep.lst`, no `asm()`

Las 44 son cadenas MUERTAS (nadie las referencia). En el objeto ORIGINAL viajan de
POLIZON dentro de un `lbl_` vivo (`lbl_803C6D90` 0x13C, `lbl_803C6F8C` 0x24,
`lbl_803C6FE4` 0x34...), porque dtk solo pone simbolo donde hay referencia. En el
nuestro cada una es su propio `$LC`, y `-strip-unused-data` le quita `size & ~7`:
544 B perdidos.

## 4. `deadstr.py` NO VE ESTA UNIDAD, y ese es el motivo de que esto llevara 5 rondas

`python scripts/deadstr.py zFeOverlay` da **0 cadenas** (con `--ancho` tambien).
La causa: su conjunto de simbolos "vivos" recorre **todas** las secciones `SHT_RELA`
del objeto, incluidas `.rela.debug` y `.rela.line`. En zFeOverlay la informacion de
depuracion referencia los `$LC`, asi que **los 370 salen vivos y no queda ninguno**.
Acotando la busqueda a las secciones ALLOC (`.text .over .rodata .data .ctors
.sdata .sdata2`) salen las 44 exactas.

En `zFe` la herramienta si encuentra 44 (704 B), o sea que **el fallo no es visible
salvo en las unidades cuyo `.debug` nombra los literales**. La version corregida
esta en el scratchpad (`feov60/deadstr2.py`) y **merece entrar en `scripts/`**:
son dos lineas.

## 5. Los ultimos 366 B: cuatro mecanismos, todos de cero bytes

Con lo anterior la unidad bajaba de 6.211 a 366 B. El resto:

| que | donde | que era |
|---|---|---|
| 240 B | `.over` 0x803C6EB4 | el bloque de cabecera partido en DOS (ver 5.1) |
| 32 B | `.data` 0x804391C4 | `pParentPkg = ""` (no `nullptr`), `phys_type[]` en el orden del objetivo, y los tres estaticos de clase de `CarCustomize.cpp` DETRAS de `SetMarkerAmounts` |
| 20 B | `.over` pools de float | cuatro semillas `if (0) { float seed = C; }` |
| 12 B | `.bss` 0x804AB5A8 | `TopOrFullScreenLoadingReason` y `gPlayerNum` definidas ANTES del primer `#include` |
| 5 B | `.over` 0x803C8B44 | `#include Timer.hpp` en la primera linea de `FEPkg_GarageMain.cpp` |

### 5.1 El bloque de cabecera va PARTIDO EN DOS

El objetivo tiene, en 0x803C6A9C (offset 0x5C del bloque), la cadena vacia `""` que
referencian 20 funciones de la unidad -- y las 44 etiquetas de AttribSys **detras**.
Nuestro bloque escrito a mano era un `asm()` unico de 0x474 B al principio del
fichero, asi que el `$LC` de `""` que emite cc1plus caia DESPUES y todo el pool
salia 4 B corrido.

La cura: **partirlo**. Mitad A (0x00..0x5C, con el simbolo `lbl_803c6a40` que
`keep.lst` ya protege) al principio de `zFeOverlay.cpp`; mitad B (0x60..0x48C,
**anonima**, no hace falta simbolo) dentro de `FEPkg_GarageMain.cpp` detras de sus
`#include`. cc1plus emite el `""` al parsear `ActionQueue.h` (linea 29 de ese
fichero), o sea **entre las dos mitades**, que es donde el objetivo lo tiene.

La mitad B se alargo ademas 24 B con `"Attrib::Gen::frontend"`, que es lo que el
objetivo tiene en 0x803C6EB4 (dentro de `lbl_803C6D90`): eso es la palanca 7 del
dossier -- alargar un `asm()` en vez de tocar `keep.lst`.

### 5.2 NEGATIVO: la semilla de pool no CREA constantes muertas

`if (0) { costf = 0.25f; }` **no** mete un 0.25f muerto en el pool:
`output_constant_pool` solo emite lo que `mark_constant_pool` encuentra en la cadena
de insns VIVA, y `jump.c` ya se llevo el bloque. **La palanca A de zMain REORDENA
constantes que se usan; no las crea.** El 0.25f muerto de 0x803C7370 hubo que
escribirlo a mano con un `asm()` anonimo entre dos funciones (el pool sale delante
de su cuerpo, asi que un `asm()` entre `CommitChangeStartRace` y
`NotificationMessage` cae justo en medio). Ese solo valio 4.032 B de desplazamiento.

### 5.3 La cola de inlines de `finish_file` NO sale en orden de peticion

Los ultimos 5 B eran `Timer::SetTime` y `Physics::Info::Performance::Default`
intercambiados. **Medidos y negativos**: una `static inline` muerta que llame a
`SetTime`, y un `if (0)` con la llamada delante de la primera peticion de `Default`,
DENTRO de la misma funcion. Ninguno la mueve.

Lo que si la mueve: **el orden de PARSEO de la DEFINICION**, o sea el orden de los
`#include` de las cabeceras que las traen. `#include "Misc/Timer.hpp"` en la primera
linea de `FEPkg_GarageMain.cpp` deja `SetTime` delante. 5 B -> 0.

### 5.4 El `.bss`: solo la PRIMERA vuelta de `.lcomm` llega a la cabeza

`gPlayerNum` tiene que ir en 0x804AB5AC, detras de `TopOrFullScreenLoadingReason` y
delante de `CustomizeHUDTexPackResources`. Definida en `uiQRCarSelect.cpp` (donde
estaba) sale la ULTIMA de la ventana, 0x108 B mas alla; definida en
`FECarViewer.cpp` justo detras de `TopOrFullScreenLoadingReason`, **tambien**.
El unico sitio que la mete en la primera vuelta de la cola es **antes del primer
`#include` de `zFeOverlay.cpp`**. Y como ahi el enum `eSetRideInfoReasons` todavia
no existe, `TopOrFullScreenLoadingReason` se define alli con el alias
`asm("...")` sobre un `int` (mismo patron que `gap_07_804AB6B8_bss`).

## 6. LA EDICION DE VENTANA: 89 lineas, SOLO ANADIR

Es aditiva: **no hay que borrar las 28 lineas viejas** (medido: DOL OK con ellas y
sin ellas; sus cadenas ya no existen, asi que las entradas son inocuas). Regenerar
con la version corregida de `deadstr.py` -- o pegar tal cual:

    # zFeOverlay: cadenas muertas que el objetivo SI tiene (deadstr2).
    # @lc zFeOverlay "FEGeometryModels"
    zFeOverlay.o:$LC405
    # @lc zFeOverlay "FrontendRenderingCar"
    zFeOverlay.o:$LC412
    # @lc zFeOverlay "SelectCarCameraMover"
    zFeOverlay.o:$LC413
    # @lc zFeOverlay "SelectableCar"
    zFeOverlay.o:$LC457
    # @lc zFeOverlay "SelectableTrack"
    zFeOverlay.o:$LC458
    # @lc zFeOverlay "CustomRace"
    zFeOverlay.o:$LC461
    # @lc zFeOverlay "QuickPlay"
    zFeOverlay.o:$LC462
    # @lc zFeOverlay "SplitScreen"
    zFeOverlay.o:$LC463
    # @lc zFeOverlay "MSOption"
    zFeOverlay.o:$LC467
    # @lc zFeOverlay "TrackDirection"
    zFeOverlay.o:$LC494
    # @lc zFeOverlay "NumLaps"
    zFeOverlay.o:$LC495
    # @lc zFeOverlay "TrafficLevel"
    zFeOverlay.o:$LC496
    # @lc zFeOverlay "NumOpponents"
    zFeOverlay.o:$LC497
    # @lc zFeOverlay "AISkill"
    zFeOverlay.o:$LC498
    # @lc zFeOverlay "CatchUp"
    zFeOverlay.o:$LC499
    # @lc zFeOverlay "ImageArraySlot"
    zFeOverlay.o:$LC526
    # @lc zFeOverlay "ChallengeDatum"
    zFeOverlay.o:$LC532
    # @lc zFeOverlay "SelectablePart 9"
    zFeOverlay.o:$LC541
    # @lc zFeOverlay "SelectablePart 10"
    zFeOverlay.o:$LC542
    # @lc zFeOverlay "SelectablePart 11"
    zFeOverlay.o:$LC543
    # @lc zFeOverlay "ShoppingCartItem"
    zFeOverlay.o:$LC544
    # @lc zFeOverlay "SelectablePart 13"
    zFeOverlay.o:$LC547
    # @lc zFeOverlay "SelectablePart 12"
    zFeOverlay.o:$LC573
    # @lc zFeOverlay "FEShoppingCartItem"
    zFeOverlay.o:$LC622
    # @lc zFeOverlay "CustomizeMainOption"
    zFeOverlay.o:$LC633
    # @lc zFeOverlay "SelectablePart 14"
    zFeOverlay.o:$LC638
    # @lc zFeOverlay "SetStockPartOption"
    zFeOverlay.o:$LC639
    # @lc zFeOverlay "CustomizePartOption"
    zFeOverlay.o:$LC667
    # @lc zFeOverlay "SelectablePart 1"
    zFeOverlay.o:$LC669
    # @lc zFeOverlay "SelectablePart 2"
    zFeOverlay.o:$LC670
    # @lc zFeOverlay "HUDLayerOption"
    zFeOverlay.o:$LC688
    # @lc zFeOverlay "HUDColorOption"
    zFeOverlay.o:$LC692
    # @lc zFeOverlay "SelectablePart 3"
    zFeOverlay.o:$LC698
    # @lc zFeOverlay "SelectablePart 4"
    zFeOverlay.o:$LC699
    # @lc zFeOverlay "ArraySlot"
    zFeOverlay.o:$LC700
    # @lc zFeOverlay "SelectablePart 5"
    zFeOverlay.o:$LC701
    # @lc zFeOverlay "CustomizePaintDatum"
    zFeOverlay.o:$LC702
    # @lc zFeOverlay "SelectablePart"
    zFeOverlay.o:$LC706
    # @lc zFeOverlay "SelectablePart 6"
    zFeOverlay.o:$LC709
    # @lc zFeOverlay "SelectablePart 7"
    zFeOverlay.o:$LC710
    # @lc zFeOverlay "SelectablePart 8"
    zFeOverlay.o:$LC731
    # @lc zFeOverlay "CarDatum"
    zFeOverlay.o:$LC739
    # @lc zFeOverlay "DebugCar"
    zFeOverlay.o:$LC740
    # @lc zFeOverlay "DebugCarOption"
    zFeOverlay.o:$LC742

**Los numeros de `$LC` son los del `.o` con sha1 `795b62f8...`.** Si alguien toca
cualquiera de los 14 ficheros de la unidad hay que regenerarlos (`lcfix` los
mantiene: por eso llevan la directiva `# @lc`).

## 7. Propuestas

1. **Aplicar las 89 lineas y promocionar zFeOverlay.** `linked` 521 -> 522.
2. **Arreglar `deadstr.py`** (acotar la liveness a secciones ALLOC): es lo que ha
   tenido esta unidad ciega cinco rondas, y puede estar cegando a otras. Merece
   pasarlo por todas las unidades del frente ANTES de repartir la r61.
3. **Barrer las cinco guardas de pool a mano** (`ATTRIB_TAGS_HAND_POOL`,
   `ATTRIB_TAG_ORDER_HAND_POOL`, `BWARE_PREFIX_GAMECUBE`, `BWARE_PREFIX_BADALLOC`,
   `DEAD_STR`) en toda unidad que escriba su prefijo a mano y no las tenga. Aqui
   valian 640 B y la cabeza entera del pool.
4. **Barrer los `BNEW`**: cualquier unidad con rutas `__FILE__` muertas en su
   ventana esta perdiendo las etiquetas de tipo del objetivo.
   `agent_feov58_paths.py` las lista.

## 8. Ficheros tocados (14, todos de zFeOverlay; ninguno compartido)

    src/Speed/Indep/SourceLists/zFeOverlay.cpp
    src/Speed/Indep/Src/Frontend/FECarViewer.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/DebugCarCustomize.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/MyCarsManager.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRCarSelect.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRChallengeSeries.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRMainMenu.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRModeSelect.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackOptions.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackSelect.cpp

Ninguna cabecera compartida, ningun fichero de otro agente, nada en `scripts/`.
