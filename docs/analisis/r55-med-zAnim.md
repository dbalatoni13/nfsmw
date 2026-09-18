# r55 — medida de `zAnim` (42.292 B): **MIXTO**, y `reorden` tiene un punto ciego

Unidad: `Speed/Indep/SourceLists/zAnim`. Reconstruida con `build_direct.py` (**1 ok a la
primera**, sin reintento). Nada de fuente tocado: ronda de medida.

## Veredicto

**MIXTO.** El `.text` es **orden puro**. El `.rodata` es **contenido**: faltan **760 B** de
pool de literales que no existen en ningún sitio del árbol.

`reorden.py zAnim` da 0 en las tres secciones — y **eso no basta para decir ORDEN_PURO**.
Ver «La sorpresa» abajo.

## 1. `reorden.py zAnim`

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .data          61      213        0          0        4
    .rodata       199      439       24          0      172
    .text         314    10426      578          0        0

`.text`: 314 símbolos, **0 sin par**, 0 de otro tamaño, y las 578 palabras crudas son todas
campo de reubicación. El `.text` de `zAnim` **no tiene nada que escribir**.

## 2. `movidos.py` — 80 permutaciones reales

    .rodata  20 movidos  1.024 B
    .text    27 movidos    460 B
    .data    30 movidos    372 B
    ?         3 movidos      0 B

### CAMBIO DE DUEÑO (M4), literal

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .bss       nuestro .o     +12, seccion enlazada      +0  ->      -12 de otro
          .data      nuestro .o    -128, seccion enlazada    -192  ->      -64 de otro
          .rodata    nuestro .o    -712, seccion enlazada   -1112  ->     -400 de otro
          .text      nuestro .o   +7604, seccion enlazada      +0  ->    -7604 de otro

**La línea de `.text` es un FALSO POSITIVO, y es el hallazgo de herramienta de esta medida.**

Censo directo sobre las tablas de símbolos de los dos `.o`: definimos **188 símbolos de
`.text` que el `zAnim.o` original no define**, y se parten en dos mitades exactas:

| | símbolos | bytes |
|---|---:|---:|
| otro objeto **también** los define | 50 | **5.496** |
| **nadie** más los define | 138 | **2.108** |
| | | **7.604** |

Y los 5.496 B disputados **no los ganamos nunca**: `zAI.o` es la **posición 0** del enlace y
`zAnim.o` la **1**. De los 50, los 27 que aparecen en los dos enlaces tienen desplazamiento
**exactamente 0** — siguen en `0x8003xxxx`, dentro de la ventana de `zAI`:

    .text  444  +0  8003B72C  reserve__...vector<WCollisionTri,...>   zAI.o,zMiscSmall.o,zRender.o,zSim.o
    .text  388  +0  8003B5A8  reserve__...vector<Hermes::Handler,...> zAI.o,zSim.o
    .text  176  +0  8003B4F8  find__...IVehicle...                    zAI.o,zMiscSmall.o,zRender.o,zSim.o

Los otros 2.108 B los estripa el enlazador por muertos. **Bulto que `zAnim` le quita de
verdad a otro objeto: 56 B**, no 8.080.

### Lo que SÍ cambia de dueño: 56 B, y ya tenían nombre

Los 14 globales de `CarCustomize.hpp:686`, todos `.data`, todos de 4 B, todos saltando
**−147.420 B** desde `zFeOverlay.o` a nuestra ventana:

    .data  4  -147420  8043918C -> 804151B0  g_bCustomizeManagerHasControl   zFeOverlay.o
    .data  4  -147420  80439190 -> 804151B4  g_bTestCareerCustomization      zFeOverlay.o
    .data  4  -147420  80439194 -> 804151B8  g_pCustomizeMainPkg             zFeOverlay.o
    ... y 11 mas, identicos

Confirma la r54 al byte. La puerta de entrada en `zAnim` es **un solo `.cpp`**:
`src/Speed/Indep/Src/Animation/AnimChooseArrest.cpp` es el único fichero de `Animation/` que
incluye `uiQRCarSelect.hpp`/`CarCustomize.hpp`.

Y **cuesta más de lo que la r54 contó**: además de los 14 símbolos de `.data`, mete
**12 cadenas de `.rodata` que el original no tiene** (201 B) — los inicializadores:
`CustomizeMain.fng`, `CustomizeGenericTop.fng`, `CustomizePerformance.fng`,
`CustomizeCategory.fng`, `CustomizeParts.fng`, `CustomHUDColor.fng`, `CustomHUD.fng`,
`ShoppingCart.fng`, `Spoilers.fng`, `Decals.fng`, `Paint.fng`, `Rims.fng`.

## 3. Dónde está de verdad el residuo: el `.rodata`

Secciones de los dos objetos y de los dos enlaces:

| | `.text` | `.rodata` | `.data` | `.bss` |
|---|---:|---:|---:|---:|
| `zAnim.o` ORIGINAL | 42.292 | 5.688 | 1.048 | 496 |
| `zAnim.o` NUESTRO | 49.896 | 4.976 | 920 | 508 |
| delta del objeto | +7.604 | **−712** | −128 | +12 |
| delta **enlazado** | **+0** | **−1.112** | **−192** | +0 (`.over` −16) |

Dos restas que hay que hacer y nadie hacía:

1. **Objeto −712 contra enlace −1.112**: el enlazador **estripa 400 B de nuestro `.rodata`**
   por no referenciado. 4.976 − 400 = 4.576 = 5.688 − 1.112. Cuadra exacto.
2. **Vtables contra pool**: de los 5.688 B originales, 1.088 son vtables; de nuestros 4.976,
   **1.136**. O sea el bloque de vtables nos sobra **+48 B** y el **resto** del `.rodata`
   nos falta **−760 B**.

### Las 67 cadenas que faltan (1.193 B), en 7 racimos contiguos

Todos **por delante** del bloque de vtables — por eso nuestro bloque empieza 760 B antes.

    017C  184 B   7 cad.  MGeneric | MNISComplete | NISName | Attrib::Gen::milestonetypes
                          | Perpetrator | ISimable | Attrib::Gen::speech
    02A4  593 B  29 cad.  Attrib::Gen::{ecar,camerainfo,effects,audioimpact,audioscrape,
                          engineaudio,audiosystem} | WorldBodyConn | Pkt_Body_{Open,Service,Send}
                          | World_UpdateBody | World_OneShotEffect | Pkt_Effect_{Send,Open,Service}
                          | WorldEffectConn | MAudioReflection | PlayerNum | Dist | Covered
                          | 16.1.0 | 1.8.1 | GRaceStatus | GManager
                          | SMS_MESSAGE_%d{,_FROM,_VOICE,_SUBJECT}
    067C  277 B  10 cad.  IntroNisBL | %s%s, | AC_PLAY_TYPE_FLAG_{LINEAR,PINGPONG}
                          | AC_PLAY_FLAG_{LOOP,RANGE} | AC_START_FLAG_{DELAY_MASTER,DELAY_LOCAL,
                          RANDOM_DELAY,RANDOM_START_FRAME}
    0880   31 B   4 cad.  High | Medium | Low | Reflection
    095C  132 B   4 cad.  CAnimResourceFileProxy | CAnimScene | CAnimWorldScene
                          | Anim_CAnimPart_SlotPool
    0A2C   86 B   4 cad.  VehicleParams | CAnimEntityData | CAnimSceneData | CAnimProperty
    0CD8  298 B   9 cad.  GenericNISControlScenario | WAC_PLAY_TYPE_FLAG_{LINEAR,PINGPONG}
                          | WAC_PLAY_FLAG_{LOOP,RANGE} | WAC_START_FLAG_{DELAY_MASTER,DELAY_LOCAL,
                          RANDOM_DELAY,RANDOM_START_FRAME}

`AC_PLAY_TYPE_FLAG_LINEAR` y toda su familia **no aparecen ni una vez en `src/`** —
`grep -r` sobre `.cpp`/`.h`/`.hpp` da cero. No es orden: es fuente que no está escrita.

**El sitio está identificado.** `AnimCtrl.cpp` (11 `// STRIPPED`) y `WorldAnimCtrl.cpp`
(10 `// STRIPPED`) son los dueños por nombre (`AC_` = `CAnimCtrl`, `WAC_` = `CWorldAnimCtrl`),
el racimo `WAC_` sale pegado a la cadena `GenericNISControlScenario` y `WorldAnimCtrl.cpp` es
justo el `#include` siguiente a `GenericNISControlScenario.cpp` en `zAnim.cpp`. Y el consumidor
del `%s%s,` está a la vista en `AnimWorldScene.cpp:91-97`, **andamiado vacío**:

    if (PrintWorldAnimationStuff) {
        if (begin_range == 0xFFFFFFFF || end_range == 0xFFFFFFFF) {
            for (int i = 0; i < 4; i++) {
            }
        }
    }

`PrintWorldAnimationStuff` es un `bool` de `.data` (4 B) que el original también tiene. El
cuerpo que imprimía los nombres de flag se quitó; las cadenas se fueron con él.

### Y 28 cadenas que emitimos de más (602 B)

12 son las `*.fng` de `CarCustomize.hpp` (201 B). **3 son `__FILE__`** de asserts que el
original no tiene (139 B): `src/Speed/Indep/Src/Animation/{ControlScenario,AnimPlayer,AnimScene}.cpp`.
El resto es un juego de `Attrib::Gen::*` **equivocado**: emitimos `transmission`,
`emittergroup`, `emitterdata`, `presetride`, `induction`, `chassis`, `tires`, `nos` y nos
faltan `milestonetypes`, `speech`, `ecar`, `camerainfo`, `effects`, `audioimpact`,
`audioscrape`, `engineaudio`, `audiosystem`. Es una cabecera de `Attrib` mal incluida.

### `.data`: dos huecos huérfanos

El original define 9 símbolos de `.data` que nosotros no (194 B), y los dos gordos son
`gap_06_804155F4_data` (124 B) y `gap_06_804158B4_data` (36 B), **todo ceros y sin ninguna
reubicación**. No los toco: `splits.txt` es zona vedada y la memoria ya avisa de que asignar
un rango huérfano rompe el DOL.

## 4. Símbolo dominante: **ninguno**

No hay uno. El residuo **no es un desplazamiento, es un déficit de sección**: `.rodata`
enlazado sale −1.112 B, así que **los 279.648 B de `.rodata` que van detrás de la ventana de
`zAnim` se corren enteros**, y con ellos cada reubicación que los apunta. Eso, más `.data`
−192, es lo que produce los 472.031 B de DOL distinto que midió la r54.

El bloque coherente más grande que se puede atacar son los **dos racimos de nombres de flag**
(`AC_` 277 B + `WAC_` 298 B = **575 B**, el **52 %** del déficit de 1.112 B).

## 5. `linkdelta`, `permorden`, `vtord`, `trypromo`

    linkdelta   .text +0    rodata-1112  data-192  over-16
    permorden   objetivo 315, nuestro 503, comunes 315 -> tenemos TODAS las funciones
                en su sitio 302 de 315, DESPLAZADAS 13, un solo ciclo de 27
                (thunks y accesores de 8-12 B: _._11IAnimEntity, GetTypeID__25CBasicCharacterAnimEntity,
                 _IHandle__12INISLISTENER, GetWorldModel__11IAnimEntity ...)
    vtord       objetivo 22, nuestro 24, posiciones que no casan 23 de 24
                SOLO NUESTRAS: 11CAnimMarker, UTL::COM::IUnknown
    rodorden    objetivo 207 cadenas, nuestro 171, en secuencia 131 (76 fuera)
    trypromo    DOL ROTO (3c6d28dd876d)

`rodorden` **falla con la ruta completa** (`sin rango de .rodata en splits.txt`) y funciona
con el nombre pelado `zAnim`. Bug de ruta, anotado.

El bloque de vtables mide 1.056 B en el objetivo (`0xED8`–`0x12F8`) y 1.104 en el nuestro
(`0xBE0`–`0x1030`): **+48 B, que son exactamente las dos vtables de más**. El hueco entre el
final del bloque y `_vt.16IControlScenario` mide **800 B en los dos**. Lo único que casa es la
posición 0 (`_vt.25GenericNISControlScenario`).

## 6. Siguiente paso

**Escribir las dos tablas de nombres de flag**, que son 575 B de los 1.112 del déficit y no
existen en el árbol:

* `src/Speed/Indep/Src/Animation/AnimCtrl.cpp`, en el bloque de `// STRIPPED` de
  `CAnimCtrl::UpdateAnim()` (~línea 230): el racimo `IntroNisBL | %s%s, | AC_*` que el
  objetivo emite en **`.rodata+0x067C`** (10 cadenas, 277 B).
* `src/Speed/Indep/Src/Animation/WorldAnimCtrl.cpp`, en su `// STRIPPED` equivalente
  (~línea 296): el racimo `WAC_*` que el objetivo emite en **`.rodata+0x0CD8`** (9 cadenas,
  298 B), **inmediatamente antes del bloque de vtables en `0x0ED8`** — que es la evidencia de
  que va al final del pool de esa TU.

Vale una tabla muerta (`static const char *const s_names[] = {...}`): el enlazador estripa el
símbolo y el pool se queda, que es la palanca de `nfsmw-primer-de-pool.md`. Medir con
`rodorden zAnim` (nombre pelado) y comprobar que el bloque de vtables deja de empezar 760 B
antes.

Segundo, y **más barato pero atómico de tres unidades**: `CarCustomize.hpp:686` a `extern`.
En `zAnim` quita los 14 cambios de dueño (56 B de `.data`) **y 201 B de `.rodata` de más**.
Toca `zAnim`, `zFe2` y `zFeOverlay` a la vez, así que no es de un agente solo.

## Sorpresas

1. **`reorden.py` tiene un punto ciego y `zAnim` es el caso que lo demuestra.** Empareja
   **por nombre**, y los literales del objetivo se llaman `lbl_xxxxxxxx` mientras los
   nuestros se llaman `$LCnnn`: **172 de los 199 símbolos de la ventana de `.rodata` salen
   como «sin par» y no se comparan nunca**. Su propio docstring avisa de que esa columna «no
   es un fallo» — en `zAnim` **sí lo es**: esconde **760 B de contenido que no está escrito**.
   *Una unidad puede dar `CONTENIDO 0` en las tres secciones y aun así necesitar fuente.* La
   columna «sin par» del `.rodata` hay que leerla **junto al tamaño de la sección**, que es lo
   único que ve el pool anónimo.
2. **La línea `.text` de CAMBIO DE DUEÑO de `movidos.py` es un falso positivo.** La resta
   «nuestro `.o` crece, la sección enlazada no» **no distingue robar de que te estripen
   código muerto**. En `zAnim` marca 7.604 B y lo robado real son 56. Se arregla mirando la
   **posición en el enlace**: si el otro dueño va antes (aquí `zAI.o` en la 0), no se roba
   nada. Mismo tipo de fallo que el que tres agentes le encontraron a `movidos` en la r54.
3. **Se refuta el dominante que la r54 asignó a `zAnim`.** `r54-control` dice «símbolo
   dominante: el bloque de vtables, −1.112 B repartidos en 14 saltos». El bloque de vtables
   **sobra +48 B**, no falta: emitimos 24 vtables contra 22. Los −1.112 son el pool de
   cadenas. Los «14 saltos» que vio la r54 estaban en el bloque de vtables porque **las
   vtables son los únicos símbolos con nombre del `.rodata`** — artefacto de nomenclatura,
   no medida.
4. **El enlazador estripa 400 B de nuestro `.rodata`** por no referenciado (objeto −712,
   enlace −1.112). Ninguna herramienta lo dice; sale de restar las dos cosas.
5. `rodorden.py` no acepta la ruta completa, sólo el nombre pelado.
