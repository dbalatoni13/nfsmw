# r55 — medida de `zFeOverlay`

Unidad: `Speed/Indep/SourceLists/zFeOverlay`.
Ventanas (`splits.txt:235`): `.over` 0x803A4294–0x803C6A3C (**141.224 B**),
`.over rename:.rodata` 0x803C6A40–0x803C8B58 (8.472 B), `.ctors` 4 B,
`.data` 0x804390E4–0x80439490 (940 B), `.bss` 0x804AAA90–0x804AC6C0 (7.216 B).

Reconstruida con `build_direct.py` a la primera (`1 ok, 0 fallidas`).

## Veredicto: MIXTO, pero el código es ORDEN PURO

| sección | palabras comparadas | crudas | CONTENIDO real |
|---|---:|---:|---:|
| `.over` (código, 141.224 B) | 35.243 | 4.268 | **0** |
| `.over rename:.rodata` (8.472 B) | 754 | 165 | **0** (ver abajo) |
| `.data` (940 B) | 191 | 15 | **2 palabras = 8 B** |
| `.ctors` | 0 | 0 | 0 |
| `.bss` | — | — | **faltan 4.104 B** |

`reorden.py zFeOverlay` da **0 de 35.243 palabras** en el código: no queda ni una
instrucción que escribir en 141 kB. Todo lo que separa el `.over` del DOL es **orden de
emisión**.

Contenido real que sí queda, y es poco:

- `.data`, 2 palabras (8 B), las dos son inicializadores inventados en una ronda anterior y
  marcados como tales en el propio fuente (`CarCustomize.cpp:28-40`, «Siete definiciones que
  faltaban en todo el arbol»):
  - `CarCustomize.cpp:34` — `const char *CustomizeShoppingCart::pParentPkg = nullptr;`
    El objetivo tiene **0x803C6A9C**, que es una **cadena vacía** `""` (offset 92 de su
    `.rodata`: la cadena `STL` mas su NUL acaba en 92 y `Attrib::Attribute` empieza en 96,
    o sea hay un objeto de 4 B en medio). Debe ser `= "";`.
  - `CarCustomize.cpp:36` — `uint32 CustomizeDecals::CurrentDecalLocation = 0;`
    El objetivo tiene **0x501**, que es `CC_DECAL_WINDSHIELD` (el primer `case` del
    `switch (to_cat)` de `CarCustomize.cpp:1407`). Debe ser `= CC_DECAL_WINDSHIELD;`.
  - AVISO: `CarCustomize.cpp/.hpp` es fichero **compartido** (llega a `zAnim` y `zFe2` por
    `uiQRCarSelect.hpp`). No es un arreglo local de esta unidad.
- `.bss`: nuestra aportación mide 3.144 B contra 7.216 B del objetivo (**−4.072 B**). El
  objetivo tiene un hueco anónimo **`gap_07_804AB6B8_bss`, 4.104 B**, y cae **exactamente al
  final de la ventana** (0x804AB6B8 + 4104 = 0x804AC6C0). Es un búfer estático de ~4 kB
  declarado el último de la unidad, que no existe en el árbol.
- `.bss`: además **`gPlayerNum` (4 B) está declarado dos posiciones tarde**. El objetivo lo
  pone entre `TopOrFullScreenLoadingReason` y `CustomizeHUDTexPackResources`; nosotros
  detrás de `CustomizeHUDTexTextureResources`. Es el `+264 .bss` de `movidos`.

Las **3 palabras** que `reorden` marca en el `.rodata` **no son contenido**: son la ranura
del destructor de `_vt.9UIQRBrief`, `_vt.12UIQRMainMenu` y `_vt.14UIQRModeSelect`. Apuntan
al **mismo símbolo** en los dos enlaces, sólo que a otra dirección (0x803C5B14 contra
0x803A6C70), y la máscara de `reorden` sólo esconde la mitad baja: la mitad alta pasa de
0x803C a 0x803A y por eso salen. Verificado ranura a ranura: el resto de la vtable es
idéntico.

## `movidos.py Speed/Indep/SourceLists/zFeOverlay`, literal

```
zFeOverlay: 39932 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
   18476 cambian de direccion, pero 18373 es ARRASTRE (tres terminos: donde
   empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
   cambia de dueno).
   .bss: base-1120 delta-4072, .ctors: base-1136 delta+0, .data: base-1120 delta-16,
   .dtors: base-1136 delta+0, .over: base+0 delta+1304, .rodata: base-1120 delta+304,
   .sbss: base-5216 delta+0, .sbss2: base-5216 delta+0, .sdata: base-5216 delta+0,
   .sdata2: base-5216 delta+0

   CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
      .bss       nuestro .o   -4072, seccion enlazada   -4096  ->      -24 de otro
      .data      nuestro .o     -16, seccion enlazada      +0  ->      +16 de otro
      .over      nuestro .o   +1304, seccion enlazada   -1136  ->    -2440 de otro
      .rodata    nuestro .o    +304, seccion enlazada      +0  ->     -304 de otro
      .sbss      nuestro .o      +0, seccion enlazada      +4  ->       +4 de otro

   ESCALONES SIN EXPLICAR (5): un desplazamiento que comparten muchos
   simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
   NO se cuenta como permutacion. Que exista uno es la firma de que un
   simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
      .over           +488    321 simbolos
      .bss           -5224     45 simbolos
      .over          +3444     30 simbolos
      .over          -2352     24 simbolos
      .over          -2364     21 simbolos

**103 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

seccion       movidos      bytes
.over              62      7,164
.bss                4      4,372
.data              34        136
?                   3          0
```

### El M4 de esta unidad es un FALSO POSITIVO: 0 B cambian de dueño

Comprobado símbolo a símbolo. Definimos **59 símbolos globales que el objetivo no**
(1.640 B); de ésos, **23 (732 B) los define también otro objeto original**: 500 B en
`.over`, 216 B en `.rodata`, 12 B y 4 B sueltos. Casi todos son inlines y vtables de
`zFe.o`:

```
  .rodata 128 B  _vt.8FEWidget                     tambien en zFe.o
  .over    52 B  _._12ArrayScripts / _._10ArrayDatum / _._10IconOption / _._8FEWidget (x4)
  .over    48 B  SetTime__5Timerf                  tambien en zFe.o
  .over    48 B  __as__Q36Attrib3Gen8pvehicleRCQ26Attrib8Instance   tambien en zPhysics.o
  .rodata  32 B  _vt.10IconOption / _vt.10ArrayDatum
  .over    32 B  SetPos/SetPosX/SetPosY__8FEWidget
  .over    24 B  Default__Q37Physics4Info11Performance   tambien en zAI.o y zSim.o
  .over    12 B  _IHandle__14ISteeringWheel        tambien en zMain.o y zSim.o
  .bss      4 B  g_pEAXSound                       tambien en zEAXSound.o
```

**Y ninguno gana el enlace.** Mirando dónde resuelven en los dos ELF enlazados, los 23
caen FUERA de la ventana de `zFeOverlay` en el enlace base **y** en el nuestro
(`_._8FEWidget` @80142878 en los dos, `_vt.8FEWidget` @803E4300 → @803E3EA0, que es
`.rodata` global, etc.). **Bytes que cambian de dueño de verdad: 0.**

El `-2440` de `.over` es coherente con que el enlazador **estripe nuestras copias
duplicadas y no referenciadas** (500 B de duplicados + 908 B de símbolos extra que no
define nadie más y no llama nadie), no con que otro objeto pierda bulto. Es decir: en esta
unidad la resta `enlazado − delta` mide **estripado**, no propiedad. No tengo los 2.440 B
atribuidos al byte: 1.408 B están nombrados, el resto no lo he medido.

## El símbolo dominante: un trío de destructores, 488 B, que arrastra 119.944 B

| símbolo | tamaño | objetivo | nuestro | desplazamiento |
|---|---:|---|---|---:|
| `_._9UIQRBrief` | 184 B | 0x803C5B14 | 0x803A6C70 | **−126.628** |
| `_._12UIQRMainMenu` | 152 B | 0x803C5BCC | 0x803A7A98 | **−123.188** |
| `_._14UIQRModeSelect` | 152 B | 0x803C5D9C | 0x803A7EA0 | **−122.620** |

184 + 152 + 152 = **488**, que es exactamente el escalón `.over +488` que sufren **321
símbolos**. Y los escalones menores lo confirman: `+184` (8 símbolos, sólo Brief),
`+336` = 184+152 (4 símbolos, Brief+MainMenu), `+488` (321, los tres). El tramo que abarca
el escalón `+488` va de `RefreshHeader__14UIQRModeSelect` @803A7D50 a
`GetCarTypeInfo__F7CarType` @803C51C4: **119.944 B, el 84,9 % del `.over`**, todo
desplazado por 488 B y nada más.

O sea: **488 B de código mal colocado explican ~120 kB de DOL distinto.**

### Dónde los emite el objetivo, y por qué

En el objetivo los tres destructores están en la **cola de inlines diferidos**, al final
del `.over`, entre los destructores de las clases locales:

```
   803C5AF4     32B TextureLoadedCallbackAccessor__24QRCarSelectBustedManagerUi
   803C5B14    184B _._9UIQRBrief          <-- aqui
   803C5BCC    152B _._12UIQRMainMenu      <-- aqui
   803C5C64     52B _._9QuickPlay
   803C5C98     52B React__9QuickPlayPCcUiP8FEObjectUiUi
   803C5CCC     52B _._10CustomRace
   ...
   803C5D68     52B React__11SplitScreenPCcUiP8FEObjectUiUi
   803C5D9C    152B _._14UIQRModeSelect    <-- aqui
   803C5E34     52B _._8MSOption
```

En el nuestro cada uno sale **justo detrás de su propio constructor**, o sea en el punto
del parseo:

```
   803A6B2C    324B __9UIQRBriefP21ScreenConstructorData
   803A6C70    184B _._9UIQRBrief          <-- aqui
   803A6D28   1032B RefreshHeader__9UIQRBrief
```

`QuickPlay`, `CustomRace` y `SplitScreen` son clases **locales de `uiQRMainMenu.cpp`** con
todos los cuerpos dentro de la clase (`uiQRMainMenu.cpp:14-44`): son inlines diferidos y
salen en la cola. Que `_._12UIQRMainMenu` salga **antes** que `_._9QuickPlay`, cuando en el
fichero las tres clases locales se declaran **antes** de la línea del destructor, sólo se
explica si el destructor se marcó para salida **al parsear la cabecera** (línea 1 del
`.cpp`), no al llegar a su definición. Lo mismo con `_._14UIQRModeSelect` justo delante de
`_._8MSOption`, la clase local de `uiQRModeSelect.cpp`.

Y en nuestro árbol los tres están **declarados en la clase y definidos fuera**, vacíos:

```
src/.../quickrace/uiQRBrief.hpp:46       ~UIQRBrief() override;
src/.../quickrace/uiQRBrief.cpp:45       UIQRBrief::~UIQRBrief() {}
src/.../quickrace/uiQRMainMenu.hpp:15    ~UIQRMainMenu() override;
src/.../quickrace/uiQRMainMenu.cpp:52    UIQRMainMenu::~UIQRMainMenu() {}
src/.../quickrace/uiQRModeSelect.hpp:15  ~UIQRModeSelect() override;
src/.../quickrace/uiQRModeSelect.cpp:29  UIQRModeSelect::~UIQRModeSelect() {}
```

Un cuerpo **fuera** de la clase no es inline: GCC 2.95 lo emite donde está la definición.
Un cuerpo **dentro** de la clase es `DECL_INLINE`, va a `saved_inlines`
(`cp/decl2.c:2140`) y lo drena `finish_file` (`cp/decl2.c:3643`). La vtable lo referencia,
así que se emite igual — pero al final, que es donde lo quiere el objetivo.

## Residuo después del trío

`permorden`: objetivo 467 funciones, nuestro 516, comunes 467, **436 en su sitio, 31
desplazadas**, 3 ciclos (148/137/121 — son rotaciones, no desorden). Las 49 «sólo nuestras»
las estripa el enlazador.

Los 62 símbolos `.over` realmente permutados (7.164 B) quedan en racimos pequeños y
**difusos**, ninguno domina:

- `+3392` (10 símbolos): `_._14SelectablePart` y sus ocho accesores de 8 B, más
  `_._16ShoppingCartItem`.
- `-2212` (6): destructores y `React` de `QuickPlay` / `CustomRace` / `SplitScreen`.
- `-1128` / `-1160` (10): vtables (`_vt.13MyCarsManager`, `_vt.16GarageMainScreen`,
  `_vt.19UIQRChallengeSeries`, `_vt.8Showcase`…).
- `+264` / `+312` / `+336` / `+408`: vtables de las opciones de carrera
  (`_vt.7AISkill`, `_vt.7NumLaps`, `_vt.12TrafficLevel`… 136 B cada una).
- `-64` (14 símbolos), el más repetido, y `.data` con 34 símbolos y sólo 136 B.

## `linkdelta` y `trypromo`

```
Speed/Indep/SourceLists/zFeOverlay      .text +0    bss-4096 sbss+4 over-1136
Speed/Indep/SourceLists/zFeOverlay      DOL ROTO (d2e208d4dd88)
```

Tamaños de objeto, objetivo contra nuestro:
`.over` 141.224 → 142.528 (+1.304), `.rodata` 8.472 → 8.776 (+304),
`.data` 940 → 924 (−16), `.bss` 7.216 → 3.144 (**−4.072**), `.ctors` 4 → 4.

## Siguiente paso concreto

**1. Mover los tres destructores vacíos a dentro de la clase** — es el paso que vale
~120 kB y son tres líneas:

- `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp:46`
  → `~UIQRBrief() override {}` y borrar `uiQRBrief.cpp:45`.
- `.../uiQRMainMenu.hpp:15` → `~UIQRMainMenu() override {}` y borrar `uiQRMainMenu.cpp:52`.
- `.../uiQRModeSelect.hpp:15` → `~UIQRModeSelect() override {}` y borrar
  `uiQRModeSelect.cpp:29`.

Es más barato y más directo que un cebo de instanciación: aquí no hay plantilla que cebar,
sólo una definición fuera de clase que hay que volver inline. Prueba de que el sitio es ése
y no otro: en el objetivo `_._12UIQRMainMenu` sale **antes** que las tres clases locales de
su propio `.cpp`, o sea en el momento en que se parsea la cabecera.

Comprobar después con `reorden.py` (debe seguir en 0), `movidos` (los escalones `+184`,
`+336` y `+488` deben desaparecer) y `permorden` (31 → esperado <10).

**2.** Los 4.104 B de `.bss` que faltan al final (`gap_07_804AB6B8_bss`) y `gPlayerNum`
declarado tarde: es lo único que puede mover el resto del `.bss` del juego (−4.096 en el
enlace).

**3.** Los 8 B de `.data`: `pParentPkg = ""` y `CurrentDecalLocation = CC_DECAL_WINDSHIELD`
(`CarCustomize.cpp:34` y `:36`). Fichero compartido — coordinarlo con quien lleve `zAnim` y
`zFe2`.

## Sorpresas

1. **El M4 de `movidos` es falso positivo aquí.** Los 23 símbolos duplicados resuelven
   fuera de `zFeOverlay` en los dos enlaces: **0 B cambian de dueño**. La resta
   `enlazado − delta` está midiendo el **estripado de nuestras copias muertas**, no
   propiedad. Con estas cifras, un `-2440` no autoriza a decir «le quita bulto a otro».
2. **`reorden.py` no mide el `.rodata` de esta unidad.** `ventanas()` (`reorden.py:100`)
   hace `out.setdefault(t[0], ...)`, y las dos filas de `splits.txt` empiezan por `.over`
   (la segunda con `rename:.rodata`): la segunda ventana se descarta **en silencio**. Hay
   que pasarla a mano (`reorden.py zFeOverlay 803C6A40 803C8B58`). Afecta a exactamente
   **dos** unidades del árbol, `zFeOverlay` y `zOnline` (son las únicas dos filas
   `rename:` de `splits.txt`).
3. **La máscara de `reorden` no vale para datos.** En `.rodata` decodifica cada palabra
   como instrucción; una ranura de vtable que apunta al mismo símbolo en otra dirección
   sale como «contenido» si cambia la mitad alta. Las 3 «palabras de contenido» de esta
   unidad son eso, y son **consecuencia** del desplazamiento, no una causa aparte.
4. **`zFeOverlay` sí es representativa de lo que el brief esperaba, y `zMain` no lo era**:
   0 palabras de contenido en 141 kB de código y un solo racimo de 488 B que explica el
   85 % de la sección.
