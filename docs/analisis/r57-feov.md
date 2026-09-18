# r57 — agente `feov`: zFeOverlay y zAnim

Unidades: `Speed/Indep/SourceLists/{zFeOverlay,zAnim}`.
**Ocho ediciones, las ocho pagan. Ninguna promociona todavia, pero las dos pasan de
medio megabyte de DOL distinto a 8.299 B.**

    fncmp ANTES y DESPUES:  zFeOverlay 0 de 467 distintas, zAnim 0 de 315.  Cero regresiones.
    sha1 sellado en TRES compilaciones seguidas, identico las tres:
       zAnim.o      3f37b646377df9b03a310d446775e73a13a67e2d
       zFeOverlay.o 382a1aee86981e80ab9ddb56c09612dc37a5e39f
    trypromo: DOL ROTO en las dos (96a50b267c00 / 8eea6c90e6b0).
    lcfix.py --check: 109 correcciones pendientes, y NINGUNA es mia (20 zAI, 89 zSpeech).

| unidad | linkdelta ANTES | linkdelta DESPUES | DOL distinto ANTES | DESPUES |
|---|---|---|---:|---:|
| zFeOverlay | `.text +0  sbss+4 over-1136` | `.text +0  **over-8**` | 535.617 | **5.060** |
| zAnim | `.text +0  rodata-1336 data-192` | `.text +0  **IGUAL**` | 465.512 | **3.239** |
| las dos juntas | | | 546.894 | **8.299** |

**Bytes de DOL ganados: 538.595.**

Y de camino, `permorden zFeOverlay` pasa de **465 de 467** a **467 de 467** (cero desplazadas,
cero ciclos) y `parseord zFeOverlay` de **44 posiciones mal de 50** a **0 de 50, con CONTROL OK**.

---

## 1. Lo que de verdad estaba pasando: tres trampas de medida encadenadas

Las dos unidades llevaban dos rondas dando `.text +0` y un deficit pequeno de seccion, y el
deficit **no era el que decia la herramienta**. Tres cosas, las tres medidas:

### 1.1 `keep.lst` empareja por NOMBRE EXACTO y el nuestro estaba en mayusculas

`zFeOverlay.cpp` emitia el prefijo de `.rodata` como `asm()` con
`.globl lbl_803C6A40` **en mayusculas**, y `keep.lst:804` lo lista como
`zFeOverlay.o:lbl_803c6a40` **en minusculas** (que es como lo nombra el objeto original).
Sin casar el nombre, `-strip-unused-data` se llevaba el bloque entero **menos el resto de
`size & ~7`**: de los 92 B quedaban 4 (`"STL\0"`) y `.over` perdia 88 B **en silencio**.

Cambiar una letra: `over-1136` -> `over-1048`, y **8.775 B de DOL**.

Es la version «de simbolo escrito a mano» de `nfsmw-lc-se-desplaza`: `keepchk`/`lcfix` no
lo ven porque no es un `$LC`, y `prefijotu` tampoco porque mira el OBJETO, donde el bloque
si esta.

### 1.2 `linkdelta` es CIEGO a un deficit de `.data` de 16 B... que corre 25 kB

`linkdelta zFeOverlay` decia `.data` perfecto. Enlazando y comparando la `.data` **simbolo a
simbolo** contra el ELF original: **338 simbolos desplazados -16 B**, desde `8043914C` hasta
`8044E608` — o sea **toda la `.data` que va detras de la ventana de zFeOverlay**. El tamano de
la seccion salia igual porque el relleno de alineacion del ultimo objeto se comia los 16 B.

Lo mismo en zAnim: `.data -192` de linkdelta se cerro con 176 B de huecos con nombre, y
**seguian faltando 16 B** en cuatro huecos ANONIMOS de 4 B que linkdelta daba por buenos
(`IGUAL`) mientras **2.599 simbolos** seguian a -16.

**Regla nueva: cuando `linkdelta` diga que una seccion esta bien, compruebalo enlazando y
comparando las DIRECCIONES de los simbolos de detras, no el tamano.** Es la version `.data`
de `nfsmw-huecos-leer-el-dol`.

### 1.3 Un hueco ANONIMO no se puede estripar

Los cuatro huecos de 4 B de zAnim no tienen simbolo en el objetivo, asi que **no pueden estar
en `keep.lst`**. Se emiten como

    asm(".section .data\n" "  .space 4\n" ".previous\n");

y sobreviven **porque no pertenecen a ningun simbolo**: `-strip-unused-data` va por simbolo y
esos bytes no son de nadie. Los cuatro juntos valieron **116.990 B de DOL** en zAnim
(120.229 -> 3.239).

---

## 2. zFeOverlay: 535.617 -> 5.060 B, en cinco pasos medidos

| # | edicion | linkdelta | DOL |
|---|---|---|---:|
| 0 | punto de partida (r56) | `sbss+4 over-1136` | 535.617 |
| 1 | `CarDatum` a `MyCarsManager.hpp` | igual | 504.453 |
| 2 | `lbl_803C6A40` -> `lbl_803c6a40` | `over-1048` | 495.678 |
| 3 | el bloque de cabecera a 1.140 B con la `.rodata` del objetivo | `over 0` | 56.201 |
| 4 | los cuatro huecos de `.data` (16 B) | `over 0` | 10.039 |
| 5 | `SelectableCar`/`SelectableTrack` a su pantalla | `over 0` | 6.930 |
| 6 | `_overlay_end` al bloque de cabecera | `over-8` | 5.875 |
| 7 | `Showcase::FromPackage` inicializado | **`over-8`** | **5.060** |

### 2.1 `CarDatum` (el encargo (a)): 31.164 B

`struct CarDatum` vivia en `MenuScreens/Common/feArrayScrollerMenu.hpp` y su clase se
completaba en el **puesto 3 de 50** del parseo; el objetivo la completa en el **46**, pegada a
`MyCarsManager` (47). Como el cuerpo de `~CarDatum()` esta EN CLASE, va a la cola
`saved_inlines`, que sale ordenada por **cuando se cierra la clase**: por eso `_._8CarDatum`
salia en el puesto 402 de 467 y descolocaba 5,6 kB de `.over`.

Se ha movido a `Safehouse/customize/MyCarsManager.hpp`, que es **su unico consumidor**
(`MyCarsManager.cpp`, 13 usos) y el fichero que el DWARF nombra `FeMyCarsManager.hpp`.
`permorden`: 465/467 -> 466/467.

### 2.2 El encargo (b) — «1.360 B de `.over` que hay que localizar»: **eran 1.136 y son cadenas muertas**

Enlazando y comparando las cadenas del tramo `rename:.rodata` (objetivo
`803C6A40..803C7F80`, 5.440 B):

    objetivo 378 cadenas, nuestro 324.  Faltan 93 (1.309 B); neto 1.060 B + relleno = 1.136.

Los 327 simbolos «que no emparejan por nombre» de la r56 son en efecto nomenclatura
(`lbl_` contra `$LC`), pero **debajo hay 93 cadenas que no existen en nuestro objeto**, y son
de dos familias:

* **vocabulario compartido**: `Attrib::Gen::{effects,audioimpact,audioscrape,speech,
  milestonetypes}`, `WorldBodyConn`, `Pkt_Body_*`, `World_UpdateBody`, `Pkt_Effect_*`,
  `MGeneric`, `MAudioReflection`, `PlayerNum`, `Dist`, `Covered`, `GRaceStatus`,
  `MMiscSound`, `SoundID`, `16.1.0`, `1.8.1`...
* **nombres de clase**: `SelectableCar`, `SelectableTrack`, `CustomRace`, `QuickPlay`,
  `SplitScreen`, `MSOption`, `TrackDirection`, `NumLaps`, `TrafficLevel`, `NumOpponents`,
  `AISkill`, `CatchUp`, `ImageArraySlot`, `ChallengeDatum`, `ShoppingCartItem`,
  `FEShoppingCartItem`, `CustomizeMainOption`, `SetStockPartOption`, `CustomizePartOption`,
  `HUDLayerOption`, `HUDColorOption`, `ArraySlot`, `CustomizePaintDatum`, `CarDatum`,
  `DebugCar`, `DebugCarOption`, `FEGeometryModels`, `FrontendRenderingCar`,
  `SelectCarCameraMover` **y `SelectablePart 1` ... `SelectablePart 14`**.

Esa segunda familia **no la genera nada del arbol** y no he identificado el mecanismo del
original (no es `BNEW`, que es `new(__FILE__, __LINE__)`). Queda en §6 como frente.

**Lo que si se ha hecho**: como `keep.lst` protege `lbl_803c6a40` por NOMBRE y no por tamano,
el bloque de cabecera se ha alargado de 92 B a **1.140 B con los bytes VERBATIM del objetivo**
(`803C6A40..803C6EAC`). Resultado: `.over` mide **exactamente lo que debe**, los primeros
1.132 B del pool son byte a byte los del objetivo, y todo lo que va detras de `.over` en el
DOL vuelve a su sitio. **439.477 B de DOL en una sola edicion** (495.678 -> 56.201).

### 2.3 Los cuatro huecos de `.data`, y el orden que los acompana

Faltaban `gap_06_80439154_data`, `gap_06_80439234_data`, `gap_06_8043948C_data` y
`lbl_8043923C` (4 B cada uno, todos ya en `keep.lst:805-808`). Ademas el objetivo emite
`ForceCar` y `bPlayerJustGotBusted` **delante** de los seis `Cheat*`, y nosotros al reves.
Las dos cosas juntas: **46.162 B** (56.201 -> 10.039).

### 2.4 El orden de parseo cierra: `SelectableCar` y `SelectableTrack` a su pantalla

El objetivo completa las clases en el orden `QRCarSelectBustedManager` (25),
`UIQRCarSelect` (26), `UIQRTrackSelect` (27), `UIQRBrief` (28). Nosotros teniamos
`UIQRBrief` en el 25 porque **`uiQRCarSelect.hpp` y `uiQRTrackSelect.hpp` incluian
`uiQRBrief.hpp`** — solo para ver `SelectableCar` y `SelectableTrack`.

La dependencia estaba invertida. `SelectableCar` se usa en `UIQRCarSelect` y `SelectableTrack`
en `UIQRTrackSelect`: se han movido cada una a SU pantalla y ahora es `uiQRBrief.hpp` quien
incluye las dos (necesita ambas para `UIQRBrief`). `uiQRTrackSelect.hpp` gana un
`#include GRaceDatabase.h` (`GRaceParameters`, que antes le llegaba por `uiQRBrief.hpp`).

    parseord zFeOverlay:  44 de 50 mal  ->  0 de 50, CONTROL OK
    permorden zFeOverlay: 466 de 467    ->  467 de 467, cero ciclos
    el bloque de vtables enlazado queda en el ORDEN EXACTO del objetivo

### 2.5 `_overlay_end`: no hay palanca de fuente, y el sitio correcto cuesta 8 B

El objetivo tiene `_overlay_end` **detras** de las vtables y del pool de floats
(`lbl_803C8B40` = 1.0f/4000.0f/0.5f/0). Un `asm()` de ambito de fichero sale **en el punto del
parseo**, o sea DELANTE de las vtables, y esos 8 B corrian el bloque entero **+8** — 3.017 B
de DOL. **Probado y negativo**: una definicion de C++ (`extern const unsigned int
_overlay_end[2] = {0,0};`) cae exactamente en el mismo sitio (puesto 380 de 441), y la
variante `static ... __asm__("_overlay_end")` **no se emite siquiera**.

Solucion: `_overlay_end` se emite **dentro del bloque de cabecera**, detras de
`lbl_803c6a40`, que baja a 1.132 B. Las vtables caen en su direccion exacta y `.over` se queda
en **-8**, que es el unico deficit que le queda a la unidad. Neto: **1.055 B a favor**.

Es deuda declarada: el marcador «fin del overlay» esta al principio del overlay. Se cierra
sola el dia que se sepa emitir 8 B DETRAS de las vtables (§6.2).

### 2.6 `Showcase::FromPackage`: el `sbss+4` de dos rondas

`const char *Showcase::FromPackage;` sin inicializador cae en `.sbss`. El objetivo lo tiene
en `.data` (`0x80439178`) apuntando a la cadena vacia de `0x803C6A9C`. Con
`= ""` desaparece el `sbss+4` **y** se tapa uno de los huecos de la ventana de `.data`.
**815 B** (5.875 -> 5.060).

---

## 3. zAnim: 465.512 -> 3.239 B, y `linkdelta` da **IGUAL**

### 3.1 La `.rodata` (el encargo): 1.336 B, sin tocar `keep.lst`

El encargo pedia transcribir las dos tablas de nombres de flag (`AC_*` / `WAC_*`, 575 B).
Se ha hecho algo mas barato y completo: **`keep.lst:78` ya protege `lbl_803CF388`**, que es el
ultimo simbolo del prefijo de `.rodata` de zAnim, y `keep.lst` no mira el tamano. Alargandolo
de `0x1C` a `0x554` con los **1.336 B verbatim del objetivo** (`0x803CF3A4` en adelante):

    rodata-1336 -> rodata 0        DOL 465.512 -> 197.691   (267.821 B)

El tramo copiado es dato puro (una sola palabra parece direccion y es un `-0.0f`), acaba en
frontera de cadena (`"ArrestM07"`), y **un `asm()` no renumera ningun `$LC`** — `lcfix --check`
sigue limpio para zAnim.

### 3.2 La `.data`: 176 B con nombre + 16 B anonimos

* `gap_06_804155F4_data` (124 B) en `AnimChooseArrest.cpp`, delante de `g_TriggerMomentNISTime`
* `lbl_80415678` (8 B, `{0, 0.08f}`) detras de `Tweak_TriggerMomentAlways`
* `gap_06_8041577C_data` (8 B) en `AnimScene.cpp`, entre `AnimPartSlotPool` y `bEnableNisTextDisplay`
* `gap_06_804158B4_data` (36 B) al final de `WorldAnimInstanceDirectory.cpp`

-> `data-192` -> `IGUAL`, DOL 197.691 -> 120.229.

Y los **cuatro huecos ANONIMOS de 4 B** (delante de `TheAnimCandidateData`, `Anim_Apply_Trans`,
`CAnimScene::mHandleCounter` y `NumWorldAnimCtrls`), que linkdelta ya no ve:
DOL **120.229 -> 3.239**.

### 3.3 Lo que le queda a zAnim: 3.239 B, todo en su ventana de `.rodata`

    803CF8DC..803D0136   2.170   el pool de cadenas mas alla del bloque copiado
    803D024E..803D0640   1.002   el bloque de vtables: MISMO TAMANO, OTRO ORDEN
    8004xxxx (dispersos)    ~70   palabras de codigo que apuntan a esas cadenas

**El bloque de vtables empieza y acaba en la direccion exacta** (`803D0220..803D0640`): solo
esta permutado. El orden del objetivo, listo para trabajar (`parseord zAnim` da
**CONTROL FALLIDO** y su cifra NO vale):

    GenericNISControlScenario, CAnimWorldScene, CWorldAnimEntityTree, WorldAnimEntityTreeInfo,
    CWorldAnimEntity, CAnimScene, CAnimSceneData, CPropAnimEntity, CBasicCharacterAnimEntity,
    IAnimEntity, CAnimPlayer, NISListenerActivity{.12INISLISTENER,.11IAttachable,
    .Q23Sim9IActivity,.Q23Sim9ITaskable,-}, CAnimChooser, CNFSAnimBank, CAnimBank,
    Q29EAGL4Anim17FnDefaultAnimBank, CAnimProperty

---

## 4. Regresiones: medidas una a una (regla 5)

Cabeceras tocadas que NO son de mis unidades y quien las incluye:

| cabecera | unidades ajenas | veredicto |
|---|---|---|
| `feArrayScrollerMenu.hpp` | zFe, zFe2, zSim | ver abajo |
| `MyCarsManager.hpp` | zFe, zFe2 (arrastrada) | idem |
| `uiQRBrief.hpp`, `uiQRCarSelect.hpp`, `uiQRTrackSelect.hpp` | zFe2 | idem |

**Metodo**: compilar cada unidad ajena a un `.o` PRIVADO (para no pisarle el objeto a su
agente), primero con mis cabeceras y luego con las cabeceras de HEAD, y comparar. Control de
churn: la misma compilacion repetida antes y despues da el MISMO sha1 en zFe y zFe2, asi que
la comparacion es limpia.

* **`zSim` (PROMOCIONADA, se enlaza desde `src/`)**: secciones identicas, **todos los simbolos
  con el mismo valor**; solo cambian los contadores de estatico local (`k.32665` contra
  `k.32671`). Enlazado con una y otra version, **el DOL sale con el MISMO sha1**
  (`c764e4c06e40`). **Cero regresion, demostrado en el enlace.**
* **`zFe`**: solo cambian las secciones de DEPURACION (`.debug`, `.line`, `.rela.debug`,
  `.debug_srcinfo`). Todas las secciones asignables y todos los simbolos, identicos.
  **Cero impacto.**
* **`zFe2`**: secciones asignables identicas; cambian `.symtab`/`.strtab`. Y **mejora**:
  con las cabeceras de HEAD el objeto de zFe2 **NO ENLAZA** (cuatro
  `L0039:Reference to undefined symbol Callback__CQ313FEPlayerCarDB...`, el alias mangled de
  clase local de `nfsmw-clase-local-alias`); con las mias enlaza y da 383.662 B de DOL.
  No es merito mio, es que el desplazamiento de contador vuelve a casar el alias — pero
  **conviene que lo sepa el agente `fe`**.

Ninguna otra unidad toca los ficheros que he editado: los seis `.cpp` de `Animation/` y
`uiQRCarSelect.cpp` / `uiShowcase.cpp` / `DebugCarCustomize.cpp` son **exclusivos** de zAnim y
zFeOverlay (comprobado con el prefijo de ruta; un `grep` por basename da falsos positivos,
`zCamera.cpp` incluye `ICEAnimScene.cpp`, no `Animation/AnimScene.cpp`).

### 4.1 Un negativo medido y revertido

Quitar `#include uiQRBrief.hpp` de `uiQRCarSelect.hpp` **regresa zAnim**: su `.rodata`
enlazada pasa de `-1336` a `-1408` y `lcfix` le abre 9 correcciones (los `$LC` se renumeran).
Se arregla anadiendo el `#include` explicito en `AnimChooseArrest.cpp`, que es por donde zAnim
entra a esa cabecera. Con eso vuelve a `-1336` y `lcfix` limpio. **Aplicado.**

---

## 5. `lcfix.py --check`: **0 correcciones mias**

Al terminar hay **109 pendientes**: **20 de `zAI.o` y 89 de `zSpeech.o`**, de otros agentes
(zAI no incluye ninguno de mis ficheros). **zAnim y zFeOverlay quedan al dia.** No lo he
aplicado.

---

## 6. Lo que queda, con su medida

1. **zAnim, 3.239 B**: el orden del bloque de vtables (§3.3, 1.002 B) y 2.170 B de pool de
   cadenas. El bloque de vtables **mide exacto**: es puro orden de completado de clase, o sea
   `#include` de `zAnim.cpp`. `parseord` no sirve aqui (CONTROL FALLIDO); el orden objetivo
   esta transcrito arriba.
2. **zFeOverlay `over-8`**: hacen falta 8 B **detras** de las vtables. El objetivo tiene ahi
   `lbl_803C8B40` (16 B: 1.0f, 4000.0f, 0.5f, 0) y nosotros emitimos `$LC767` (1.0f, 4 B) y
   **estripamos `$LC770`/`$LC771`/`$LC772`** (0.0f, 4000.0f, 0.5f), que son EXACTAMENTE las
   otras tres. **Propuesta de `keep.lst`: tres lineas** y `_overlay_end` puede volver a su
   sitio:

       # @lc zFeOverlay ""   (float 0.0)
       zFeOverlay.o:$LC770
       zFeOverlay.o:$LC771
       zFeOverlay.o:$LC772

   (las tres son constantes de coma flotante, no cadenas, asi que la directiva `@lc` habria
   que darla en la forma `@lcsrc`; lo dejo al jefe.)
3. **zFeOverlay, 4.175 B de pool**: las 93 cadenas de §2.2. La familia de **nombres de clase**
   (`SelectablePart 1`..`14`, `QuickPlay`, `MSOption`, ...) es un mecanismo del original que no
   he identificado; si aparece, cierra la unidad casi entera.
4. **zFeOverlay, 282 B de `.data`**: `phys_type`/`markers`/`slot_id`/`markers` (96 B de
   estaticas de funcion de `CarCustomize.cpp`) van DELANTE de
   `_14CustomizeParts.TexturePackLoaded` en el objetivo y DETRAS en el nuestro.

---

## 7. Sorpresas

1. **Un `asm()` con el simbolo mal escrito se estripa sin avisar**, y `size & ~7` deja el
   resto como basura: 88 B perdidos de 92 y `.over` corta durante dos rondas.
   `keep.lst` empareja por nombre EXACTO, mayusculas incluidas.
2. **`linkdelta` da `IGUAL` con la seccion 16 B corta.** El relleno de alineacion del ultimo
   objeto se lo come. Hay que mirar las DIRECCIONES de los simbolos de detras.
3. **Un hueco anonimo (`.space N` sin simbolo) es inestripable**, y es la unica forma de
   emitir los huecos que el objetivo no nombra. Cuatro de 4 B valieron 116.990 B de DOL.
4. **`keep.lst` protege por nombre, no por tamano**: alargar un `lbl_` ya listado mete todo el
   dato que quieras sin tocar el fichero. Es la palanca que cerro `.rodata` de zAnim y `.over`
   de zFeOverlay.
5. **No hay palanca de fuente para emitir detras de las vtables**: ni `asm()` de fichero, ni
   definicion de C++, ni `static` con `__asm__()` (esta ni se emite).
6. **`uiQRCarSelect.hpp` incluia `uiQRBrief.hpp` al reves de como debia**, y eso solo valia
   3.017 B de vtables mas el accesor. La dependencia correcta la dice el orden de parseo del
   objetivo.
7. **Cambiar un COMENTARIO cambia el sha1 del `.o`** (va en `.debug`/`.line`): sellar por sha1
   marca falsos positivos. Lo que hay que comparar es `linkdelta` + el hash del DOL.
8. **`zSim` esta promocionada y hoy NO se reconstruye a su objeto**: el `.o` en disco mide
   `.text 109.380 / .bss 8.180` y recompilando la fuente de hoy sale `109.324 / 8.164`, con
   **234.266 B de DOL distinto**. No es mio (con mis cabeceras y sin ellas sale el mismo
   objeto), pero **si alguien reconstruye zSim, pierde la promocion**. Aviso para el cierre.
