# r55 — medida de `zFe` (174.200 B)

Unidad: `Speed/Indep/SourceLists/zFe`. Reconstruida con
`python scripts/build_direct.py Speed/Indep/SourceLists/zFe` — **3 ok, 0 fallidas** al primer
intento (aviso al final: ese comando compila TRES unidades, no una).

**Veredicto: ORDEN_PURO.** No queda contenido que escribir. Lo que separa a `zFe` del DOL es
el **orden de emisión**, y el mecanismo está identificado, nombrado y cuantificado: son
**49 funciones que el original difiere a la cola de `saved_inlines` y nosotros emitimos en el
punto de parseo**, porque en nuestro fuente están definidas **fuera de la clase** y en el
original estaban **dentro**.

Y hay **dos refutaciones**: el `CONTENIDO` que marca `reorden.py` es falso, y el **CAMBIO DE
DUEÑO de `movidos.py` es 0**, no −13.368 B.

---

## 1 · `reorden.py zFe` — ¿orden o contenido?

```
zFe: NUESTROS bytes recolocados en el ORDEN DEL OBJETIVO

seccion     simbs palabras   crudas  CONTENIDO  sin par
.ctors          0        0        0          0        0
.data          54      407      109          0       10
.rodata       707     2287      465        113      547
.text         920    43495     6227          0        0

quedan 113 palabras de CONTENIDO real:  (todas `_vt.*`)
```

**`.text` 0. `.data` 0. `.rodata` 113 — y las 113 son un falso positivo, demostrado.**

Las 113 palabras están, **todas**, dentro de símbolos `_vt.*`. Una ranura de vtable es una
reubicación `R_PPC_ADDR32` de palabra entera, y `mascara()` (`reorden.py:70-79`) sólo
enmascara **formas D** (mitad baja de 16 bits) y **ramas**; cualquier otra palabra cae en la
rama `('r', v)` y se compara **en crudo**. Como la función apuntada está en otra dirección en
los dos enlaces, la palabra difiere y se cuenta como contenido.

Comprobado comparando las ranuras contra el **nombre destino de la reubicación** en los dos
`.o`, no contra el valor:

```
vtables objetivo 157, nuestras 158, comunes 157
slots identicos (crudos o mismo destino): 1443
slots con DESTINO distinto (contenido real): 0
slots con contenido NO reubicado distinto:  0
vtables con TAMANO distinto:                0
```

**Las 157 vtables comunes son idénticas al byte.** El contenido real de `zFe` es **0 palabras
en las tres secciones**.

---

## 2 · `movidos.py Speed/Indep/SourceLists/zFe`

```
zFe: 39706 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
   13672 cambian de direccion, pero 13202 es ARRASTRE (tres terminos: donde
   empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
   cambia de dueno).
   .bss: base-64 delta+136, .data: base-32 delta-36, .rodata: base+0 delta+696,
   .sbss: base-64 delta+0, .sbss2: base-64 delta+0, .sdata: base-64 delta+0,
   .sdata2: base-64 delta+0, .text: base+0 delta+13368

   CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
      .bss       nuestro .o    +136, seccion enlazada      +0  ->     -136 de otro
      .data      nuestro .o     -36, seccion enlazada     -32  ->       +4 de otro
      .rodata    nuestro .o    +696, seccion enlazada     -48  ->     -744 de otro
      .text      nuestro .o  +13368, seccion enlazada      +0  ->   -13368 de otro

   ESCALONES SIN EXPLICAR (11): un desplazamiento que comparten muchos
   simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
   NO se cuenta como permutacion. Que exista uno es la firma de que un
   simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
      .text          +4344    139 simbolos
      .text          +4220     79 simbolos
      .text           +420     70 simbolos
      .text          +4968     66 simbolos
      .text          +2800     66 simbolos
      .text          +4960     59 simbolos
      .text          +1908     49 simbolos
      .text          +2640     31 simbolos
      .text          +5724     27 simbolos
      .rodata         -376     26 simbolos
      .text          +2704     22 simbolos

**470 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

seccion       movidos      bytes
.text             307     52,032
.rodata           118      5,424
.data              36      1,464
.bss                6      1,220
?                   3          0

los desplazamientos mas repetidos:
        -112  24 simbolos
        +572  19 simbolos
       +2672  18 simbolos
         -24  16 simbolos
       +3376  15 simbolos
         ...
```

### El cambio de dueño de `.text` es CERO, no 13.368 B

Enlacé las dos veces y comprobé, símbolo a símbolo, dónde cae cada uno de los 53 candidatos
(los que **nosotros** definimos, el `zFe.o` original **no**, y **otro objeto del enlace sí**;
6.344 B en total, 6.280 de `.text`):

```
simbolos exclusivos de zFe: 917
zFe .text BASE   : 0x80118248..0x80142A94 (174156 B)
zFe .text NUESTRO: 0x80118248..0x80142A94 (174156 B)

ROBADOS de verdad (fuera del rango de zFe en base, DENTRO en el nuestro): 0, 0 B
NO robados: 28 (4320 B), de esos con direccion distinta: 1
AUSENTES del enlace (estripados): 25
```

La ventana de `zFe` es **la misma dirección y el mismo tamaño** en los dos enlaces. Los 185
símbolos que sólo emitimos nosotros (13.368 B) **desaparecen enteros**: 25 se estripan por no
referenciados y el resto los resuelve un objeto que va **antes** en el enlace, así que la
definición ganadora no cambia de dueño — la nuestra simplemente se descarta.

**`movidos.py` confunde «lo nuestro se descarta» con «le quitamos bulto a otro».** Los dos
casos dan la misma resta (nuestro `.o` crece, la sección enlazada no), y sólo el segundo mueve
nada. Para `zFe` el M4 correcto es:

| sección | M4 de `movidos.py` | M4 real |
|---|---:|---:|
| `.text` | −13.368 | **0** |
| `.rodata` | −744 | **0** (los +696 nuestros son basura estripada; el −48 del enlace es déficit propio) |
| `.bss` | −136 | **64 B**, un solo símbolo |
| `.data` | +4 | 0 |

El único cambio de dueño real es **`_16FEAnyMovieScreen.ReturnToPackageName`, 64 B**: lo
emitimos como COMMON y en el original lo aporta `auto_07_804F4040_bss.o`. Su dirección se mueve
0x804FE740 → 0x804FE700.

---

## 3 · Qué está descolocado, y por qué

`permorden`:

```
zFe: objetivo 921 funciones, nuestro 1106, comunes 921
solo nuestras (las estripa el enlazador): 185
en su sitio (subsecuencia creciente mas larga): 816 de 921
DESPLAZADAS: 105
ciclos no triviales: 8  (longitudes: 610, 138, 116, 37, 4, 4, 4, 2)
```

Medido **sobre el enlace** (que es lo que ve el DOL):

```
.text de zFe: 921 funciones comunes
   EN SU DIRECCION:    6 (   396 B)
   EN OTRA DIRECCION: 915 (173.804 B)   <- 99,8 %
   156 deltas distintos; mayor racimo +2800 con 19.596 B (11 %)
```

### La causa: la cola de `saved_inlines`

El objetivo emite un bloque al final de su `.text` (0x027550..0x02A878, 13.096 B) que es
**exactamente** la cola que `finish_file` drena: destructores, sobrecargas virtuales y
accesores diminutos, y de última `_GLOBAL_.I.gOnlineMainMenu`. Nosotros emitimos **49 de esas
funciones (5.160 B) en el cuerpo**, muy por delante.

La prueba está en la **alternancia**. En el objetivo:

```
0x027974    52 _._13pm_ResumeRace
0x0279A8    88 React__13pm_ResumeRacePCcUiP8FEObjectUiUi
0x027A00    52 _._17pm_ResumeFreeRoam
0x027A34    88 React__17pm_ResumeFreeRoamPCcUiP8FEObjectUiUi
0x027A8C    52 _._14pm_RestartRace
0x027AC0   152 React__14pm_RestartRacePCcUiP8FEObjectUiUi
   ... nueve pares seguidos, clase a clase ...
0x027F20    52 _._15pm_QuitRaceToFE
0x027F54   300 React__15pm_QuitRaceToFEPCcUiP8FEObjectUiUi
```

En el nuestro los nueve `React` van **juntos** en el cuerpo y los nueve destructores **juntos**
en la cola:

```
0x007514    88 React__13pm_ResumeRace...     0x02AF60  52 _._13pm_ResumeRace
0x00756C    88 React__17pm_ResumeFreeRoam... 0x02AFD8  52 _._17pm_ResumeFreeRoam
   ...                                          ...
0x007920   300 React__15pm_QuitRaceToFE...   0x02B32C  52 _._15pm_QuitRaceToFE
```

Y el fuente lo confirma sin margen:

* `src/Speed/Indep/Src/Frontend/MenuScreens/InGame/uiPause.hpp:96` → `~pm_QuitRaceToFE() override {}` **dentro de la clase** ⇒ `DECL_INLINE` ⇒ `mark_inline_for_output` ⇒ cola ⇒ sale al final. ✔ casa.
* `src/Speed/Indep/Src/Frontend/MenuScreens/InGame/uiPause.cpp:434` → `void pm_QuitRaceToFE::React(...)` **fuera de la clase** ⇒ se emite en el punto de parseo. ✘ no casa.

La alternancia destructor/React del objetivo **sólo es posible si los dos cuerpos estaban
dentro del cuerpo de la clase**, en el orden de declaración de `uiPause.hpp`.

Las 49 funciones, por fichero:

| fichero | funciones | bytes | familia |
|---|---:|---:|---|
| `uiMain` | 6 | 420 | `React__{MainCareer,Challenge,MainQuickRace,MainCustomize,MainProfileManager,MainOptions}` |
| `uiPause` | 9 | 1.336 | `React__pm_*` |
| `uiCareerMain` + `uiCareerManager` | 10 | 1.636 | `React__C*` + `_._12uiCareerCrib`, `_._15uiCareerManager` |
| `uiProfileManager` | 6 | 720 | `React__PM*` + `_._16UIProfileManager`, `_._15UIDeleteProfile` |
| `uiRapSheet US/VD/CTS` | 7 | 732 | `_._12uiRapSheet*`, `NotificationMessage__*Datum` |
| resto | 11 | 316 | `_._17UIOptionsTrailers`, `Callback__FEng*`, `TextureLoadedCallback__*`, `*CallbackBridge__*`, `Clear__Q26Realmc15SystemInterface`, `RaiseToPower__H1i10_i_i` |

Ablación sobre la permutación de `permorden` (105 desplazadas):

```
sin React__          : DESPLAZADAS 78   (React explica 27 de 105)
sin _._              : DESPLAZADAS 62   (los dtor explican 43 de 105)
sin React__ ni _._   : DESPLAZADAS 35   (los dos, 70 de 105 = 67 %)
```

---

## 4 · Símbolo dominante: **no hay uno; el pago es por prefijo**

Simulación de recolocación (rehago el reparto de direcciones moviendo un subconjunto a su
ranura del objetivo y cuento cuántos bytes caen ya en su dirección; el control con el orden
completo del objetivo da 100,0 %, y con el nuestro da lo que mide el enlace):

```
nuestro orden (base)                    5 fns      176 B   (0,1 %)
orden del objetivo (control)          921 fns  174.200 B (100,0 %)
solo React__ recolocado (27 fns)       75 fns   11.736 B   (6,7 %)
solo _._ recolocado (8 fns)             5 fns      176 B   (0,1 %)
las 49 adelantadas recolocadas        594 fns  129.260 B  (74,2 %)
```

Uno a uno **no paga ninguno** (el mejor, `RaiseToPower__H1i10_i_i`, 332 B = 0,2 %), y por
grupos tampoco:

```
uiMain (6 React)                 6 fns  11.736 B (6,7 %)
uiPause (9 React)                9 fns     176 B (0,1 %)
uiCareerMain/Manager            10 fns     476 B (0,3 %)
uiProfileManager                 6 fns     176 B (0,1 %)
uiRapSheet US/VD/CTS             7 fns     176 B (0,1 %)
```

**La lectura es la que importa para repartir:** el `.text` sólo casa hasta el **primer** error,
así que la recompensa es acumulativa **de delante hacia atrás**. El único grupo que paga solo
es `uiMain`, porque sus seis `React` son lo **primero** que emite nuestro `.text`
(0x0000F0..0x000368, contra 0x02759C..0x027844 en el objetivo: los sacamos ~161 kB antes de
tiempo). Y los 49 juntos valen **129.084 B, el 74,2 %**.

El 25,8 % que queda son los otros 27 bloques de cambio de la secuencia (76 en total): el
intercambio `Setup__13UIOptionsMain` ⇄ `ExitOptions__13UIOptionsMainPCc`,
`AddItem__13UIMemcardListPCcT1ii`, `ShouldDoAutoSave__10MemoryCardb`,
`MyFinishLoadingControllerTextureCallbackBridge__FUi`, `SortSMS__FP11SMSSortNodeT0` y el orden
interno de la propia cola de destructores.

---

## 5 · `linkdelta`, `permorden`, `trypromo`

```
DISTANCIA REAL AL ENLACE
Speed/Indep/SourceLists/zFe                        +0   rodata-48 data-32

permorden: 816 de 921 en su sitio, 105 desplazadas, 8 ciclos (610/138/116/37/4/4/4/2)

trypromo:  Speed/Indep/SourceLists/zFe    DOL ROTO (1b781a9a0264)
```

`rodorden` **no se puede ejecutar**: `Speed/Indep/SourceLists/zFe: sin rango de .rodata en
splits.txt`. Hueco de herramienta anotado.

`vtord`: **143 de 158 posiciones mal**, y **una vtable de más**, `_vt.11PMPopDelete` (32 B).
Es un eje **independiente** del de `.text` (`walk_globals`, orden de compleción de la CLASE,
no `saved_inlines`). El objetivo **no tiene `PMPopDelete` en absoluto**: ni `React__`, ni
`_._`, ni constructor, ni vtable — nosotros emitimos las cuatro cosas
(`src/Speed/Indep/Src/Frontend/Database/uiProfileManager.cpp:176`).

Déficit de datos, medido en los objetos:

* `.rodata` **−48** en el enlace (nuestro `.o` es +696, pero `-strip-unused-data` se lleva lo
  no referenciado). Vtables 9.032 → 9.064 (+32, `PMPopDelete`).
* `.data` **−32**: el objetivo tiene cinco rangos anónimos que no emitimos —
  `gap_06_8041B5DC`(4), `gap_06_8041B5FC`(**16**), `gap_06_8041B63C`(4), `gap_06_8041B648`(4),
  `gap_06_8041BA74`(4) = **32 B exactos**. Además `sOpName.36228` (8 B) es en el original un
  **static de función** y en el nuestro un `sOpName` de fichero, y sobra `ChyronScreenPtr` (4 B).
* `.bss`: 44 temporales `_.tmp_N` de más contra 10 del objetivo (+136 B en el `.o`, +0 en el
  enlace).

---

## 6 · El siguiente paso concreto

**No es un cebo de instanciación** — aquí no hay plantilla que primar: es **forma de fuente**.
Hay que devolver los cuerpos a **dentro de la clase**, empezando **por delante**, que es donde
el prefijo paga:

> **`src/Speed/Indep/Src/Frontend/MenuScreens/InGame/uiMain.hpp`** (y `uiMain.cpp`): mover los
> seis cuerpos `MainCareer::React`, `Challenge::React`, `MainQuickRace::React`,
> `MainCustomize::React`, `MainProfileManager::React` y `MainOptions::React` desde el `.cpp` al
> **cuerpo de su clase**, justo detrás del `~X() {}` que ya está en clase, y en ese mismo orden
> (MainCareer, Challenge, MainQuickRace, MainCustomize, MainProfileManager, MainOptions).

La evidencia de que ahí es donde va: el objetivo los emite alternando con su destructor en
0x027568..0x027844 —

```
0x027568  52 _._10MainCareer      / 0x02759C  80 React__10MainCareer
0x0275EC  52 _._9Challenge        / 0x027620 180 React__9Challenge
0x0276D4  52 _._13MainQuickRace   / 0x027708  40 React__13MainQuickRace
0x027730  52 _._13MainCustomize   / 0x027764  40 React__13MainCustomize
0x02778C  52 _._18MainProfileManager / 0x0277C0 40 React__18MainProfileManager
0x0277E8  52 _._11MainOptions     / 0x02781C  40 React__11MainOptions
```

— y nuestros destructores ya salen en la cola, en ese orden, mientras los `React` están en
0x0000F0.

Vale **11.736 B (6,7 %) por sí solo**, medido, y desbloquea el resto: después van
`uiPause.hpp` (9), `uiCareerMain`/`uiCareerManager` (10), `uiProfileManager` (6) y
`uiRapSheet*` (7), hasta los **129.084 B / 74,2 %** del bloque completo.

Coste esperado: `.text +0` en el enlace (mover un cuerpo no cambia su tamaño), y **ninguna
cabecera compartida**: `uiMain.hpp`, `uiPause.hpp`, `uiCareerMain.hpp`, `uiCareerManager.hpp`,
`uiProfileManager.hpp` y `uiRapSheet*.hpp` son de `zFe`/`zFe2`. Hay que comprobar `zFe2` en el
mismo cambio.

Segundo paso, independiente y barato: **quitar `PMPopDelete`** (`uiProfileManager.cpp:176`),
que el objetivo no tiene y nos mete una vtable de 32 B.

---

## Avisos de método

1. **`build_direct.py Speed/Indep/SourceLists/zFe` compila TRES unidades** — `zFe`, `zFe2` y
   `zFeOverlay` — porque empareja por prefijo de nombre. Con trece agentes en paralelo eso
   pisa el `.o` de dos compañeros. Salida literal: `compilando 3 unidades / ok zFe / ok zFe2 /
   ok zFeOverlay / 3 ok, 0 fallidas`.
2. **`reorden.py` sobreestima el `.rodata`** de toda unidad con vtables: `mascara()` no
   enmascara `R_PPC_ADDR32`. Léelo como «113 palabras, todas `_vt.*`» = **orden**, no
   contenido. Se confirma comparando destinos de reubicación por nombre.
3. **`movidos.py` da M4 falso** cuando nuestro `.o` trae símbolos de más que el enlazador
   descarta (estripados o resueltos por un objeto anterior). La resta es la misma; el efecto,
   ninguno. Confirmarlo cuesta un enlace doble y una comparación de direcciones.
4. **`rodorden.py` no corre sobre `zFe`**: no hay rango de `.rodata` en `splits.txt`.
5. Importar `permorden` como módulo **ejecuta su `main()`** (mide `zSim` por defecto) y ensucia
   la salida.
