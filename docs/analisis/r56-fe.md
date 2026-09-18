# r56 · `fe` — zFe baja 90.306 B: el frente de zFe NO era uiPause, era el ORDEN DENTRO DEL CUERPO

Unidades: `zFe`, `zFe2`. Ni un commit, ni un `git add`. `configure.py`, `config/GOWE69/*`,
`splits.txt` y `keep.lst` **sin tocar** (`git status config/ configure.py` vacío al cerrar).

## Titular

| unidad | antes | después | Δ | DOL |
|---|---:|---:|---:|---|
| **zFe** | 457.829 B | **367.523 B** | **−90.306** | ROTO (`a2d3633f709b`) |
| zFe2 | 525.173 B | 525.173 B | 0 | ROTO (`c5c7e06f8fc6`) |

(la cifra es `.text + .rodata + .data` de bytes distintos del ELF enlazado contra la base,
la misma métrica de `r55-orden`; zFe pasa de `.text 188.016 / .rodata 148.234 / .data 121.579`
a **`.text 97.811 / .rodata 148.133 / .data 121.579`**)

`fncmp` **antes y después**: `0 de 921` y `0 de 1307` funciones con el código distinto.
**Cero regresiones.** Tres compilaciones seguidas con el mismo sello:
`zFe 44793ec04e74…`, `zFe2 f3cb7f9418a7…`.

Y hay **una propuesta medida que no puedo aplicar** (§5): valdría **43.876 B más** en zFe,
pero renumera cuatro entradas de `keep.lst` que son de zWorld y zEAXSound2.

---

## 1 · El encargo estaba mal dirigido, y se ve en la primera medida

El brief mandaba empezar por `uiPause.hpp` (las nueve clases `pm_*` a `uiPause.cpp` con `React`
dentro), con el modelo de `r55-med-zFe` dando 129.084 B / 74,2 % para el bloque de las 49
funciones adelantadas.

**Lo hice, lo medí, y sale NEGATIVO por sí solo**: `.text 188.016 → 188.172` (+156),
`.rodata −42`, **total +114 B**. El propio modelo de la r55 ya lo decía —«uiPause (9 React):
9 fns, 176 B, 0,1 %»— y la razón es la que el informe explicaba: **la recompensa es acumulativa
de delante hacia atrás**, y `uiPause` está en la cola, no en el prefijo.

Lo que el modelo no vio es **dónde estaba de verdad el primer error**. Con `uiMain` ya cerrado
por la r55, la secuencia común de `.text` de zFe casaba **sólo 11.492 B** y se rompía aquí:

```
objetivo[69] = NotificationMessage__13UIOptionsMainUlP8FEObjectUlUl (0x002CE4)
nuestro [69] = ExitOptions__13UIOptionsMainPCc                      (0x002CE4)
```

`uiOptionsMain.cpp` define ctor / **ExitOptions** / Setup / **NotificationMessage** y el objetivo
emite ctor / **NotificationMessage** / Setup / **ExitOptions**. Un intercambio de dos funciones
en un `.cpp` de 130 líneas estaba tapando **162 kB** de unidad.

**La lección de método**: el modelo de r55 midió *cuánto vale recolocar un grupo* pero no
*dónde está el primer corte*. Con `seq.py` (difflib sobre la secuencia de símbolos de `.text`
de los dos `.o`) el primer corte sale en un segundo y es lo único que hay que mirar.

---

## 2 · Lo que se ha hecho, en orden de valor

El ciclo fue siempre: `seq` → arreglar el primer corte → compilar → `seq` otra vez. Cuatro
tandas, y el prefijo común pasó de **11.492 B a 158.392 B** (el `.text` de zFe mide 174.156 B;
lo que queda es la cola de `finish_file`).

### 2.1 Orden dentro del `.cpp` (siete ediciones, ninguna toca cabecera)

| fichero | qué | evidencia |
|---|---|---|
| `uiOptionsMain.cpp` | ctor / NotificationMessage / Setup / ExitOptions | zFe.o 0x002C58…0x0032DC |
| `uiOptionsScreen.cpp` | `Get/SetPlayerToEditForOptions` del final del fichero al principio | 0x003358, entre ExitOptions y el ctor de UIOptionsScreen |
| `uiOptionsController.cpp` | el bridge `MyFinishLoadingControllerTextureCallbackBridge` del final a entre `FinishLoadingTexCallback` y `CalcControllerTextureToLoad` | 0x0059A0 |
| `uiRapSheetUS.cpp` | `ToggleView` detrás de `RefreshHeader` | 0x00A298 |
| `uiRepSheetBounty.cpp` | `BountyDatum::NotificationMessage` delante del ctor | 0x010A28 |
| `uiSMS.cpp` | `SortSMS` entre el ctor de `uiSMS` y `uiSMS::Setup` | 0x015908 |
| `MemoryCard.cpp` | `ShouldDoAutoSave` detrás de `BootupCheck`; y el `namespace RealmcIface` **partido en dos** para meter el puente `__Q211RealmcIface8GameInfoPCwUibT3` entre `Delete` y `Load` | 0x020130 y 0x01F5E8 |
| `uiMemcardBase.cpp` | `UIMemcardList::AddItem` detrás de `NotificationMessage` | 0x0267BC |

El del `namespace` partido vale la pena contarlo aparte: eran **32 B** de desplazamiento y
tapaban 30 kB de secuencia.

### 2.2 Cuerpo A LA CLASE (a la cola de `finish_file`)

Trece funciones que nosotros emitíamos en el punto del parseo y el objetivo difiere:

* `~UIOptionsTrailers` (uiOptionsTrailers.hpp)
* `RapSheet{US,VD,CTS}Datum::NotificationMessage` y `~uiRapSheet{US,VD,CTS}` (seis)
* `uiRepSheet{Main,Rival}::TextureLoadedCallback` (dos)
* los tres bridges de `uiRepSheetRivalStreamer`
* `Realmc::SystemInterface::Clear` (MemoryCardHelper.hpp)
* `~uiCareerCrib`, `~uiCareerManager`, `~UIProfileManager`, `~UIDeleteProfile`

### 2.3 Cuerpo FUERA de la clase (el caso contrario, y el que nadie había anotado)

`UIMemcardBoot::NotifySoundMessage` (8 B) estaba **dentro** de la clase en `uiMemcard.hpp` y el
objetivo la emite **en el cuerpo**, detrás de `NotificationMessage` (0x023028). Es la simétrica
de la receta de uiMain y funciona igual de bien.

### 2.4 Clases bajadas al `.cpp` con `React` dentro (la receta de uiMain)

* `uiPause.hpp` → `uiPause.cpp`: las nueve `pm_*`.
* `uiCareerMain.hpp` → `uiCareerMain.cpp`: `CResumeFreeRoam`, `CCarSelect`, `CRapSheet`,
  `CTop15`, `CSave`; y → `uiCareerManager.cpp`: `CResumeCareer`, `CStartNewCareer`,
  `CLoadCareer`.
* `uiProfileManager.hpp` → `uiProfileManager.cpp`: `PMSave`, `PMLoad`, `PMDelete`,
  `PMCreateNew`, `PMPopDelete` (queda `struct PMSave;` adelantada: `UIProfileManager` guarda un
  `PMSave*`).

---

## 3 · Dónde está zFe ahora, exactamente

Medido **sobre el ENLACE**, que es lo único que cuenta (`linkdirs.py`, en el scratchpad):

```
zFe .text ENLAZADO: en su direccion 17.388 fns, FUERA 508 fns (65.420 B)
```

De esos 508, **282 son el arrastre de +124 B** que mete la propuesta de §5. Con esa propuesta
aplicada quedan **226 funciones y 15.412 B**, y **todas están en la COLA**
(0x8013EDDC..0x80142A94, los últimos 16 kB del `.text` de zFe). **El cuerpo de zFe casa entero.**

La cola es un problema distinto —el orden en que `finish_file` drena `saved_inlines`,
`pending_templates` y las vtables— y **no lo he tocado**. Lo que sé de él:

* Nuestra cola empieza por `ClassKey__Q36Attrib3Gen8frontend` (12 B) y
  `RaiseToPower__H1i10_i_i` (56 B); en el objetivo la primera va en el 94 % de la cola y la
  segunda es **la última función del `.text`**. Esos 68 B desplazan la cola entera.
* Después el objetivo quiere `SetTime__5Timerf` antes que `_._16FEObjectCallback` y nosotros lo
  damos al revés.
* `vtord zFe` da **157 vtables contra 157** (ni una de más ni de menos) pero casi todas en otra
  posición. Y ahí hay una pista concreta: el orden de vtables es el **REVERSO del orden de
  declaración de las clases**. Comprobado con el grupo de uiProfileManager: declaramos
  `PMSave, PMLoad, PMDelete, PMCreateNew` y luego `UIDeleteProfile, UIProfileManager`, y salen
  `PMCreateNew, PMDelete, PMLoad, PMSave, UIProfileManager, UIDeleteProfile`. El objetivo quiere
  `UIDeleteProfile, UIProfileManager, PMCreateNew, PMDelete, PMLoad, PMSave`, o sea que su orden
  de declaración era `PMSave, PMLoad, PMDelete, PMCreateNew, UIProfileManager, UIDeleteProfile`:
  **las PM* delante (como en HEAD) y UIProfileManager declarada ANTES que UIDeleteProfile.**
  Es un experimento de una línea para la próxima ronda.

---

## 4 · zFe2: cero movimiento, y una deuda de mantenimiento que sí paga

zFe2 mide **exactamente lo mismo** que antes de la ronda (`.text 233.691 / .rodata 153.208 /
.data 138.274`). Ninguna de mis ediciones le afecta al código: comprobado leyendo su `.o`, no
define **ni un solo símbolo `pm_*`, `C*` ni `PM*`**.

Lo que sí le pasó, y hay que saberlo:

> **Sacar clases de una cabecera renumera los nombres privados de la unidad, y `zFe2.cpp` lleva
> 16 alias `asm()` con ese número.** Tras la tanda, el enlace de zFe2 **fallaba** con 16
> `Reference to undefined symbol …0_7NumCarsRC14FECareerRecord.32168`. `mangfix.py zFe2` lo
> arregla (los 16 se desplazaron −25, que es el número de declaraciones de clase que quité de
> las cabeceras que zFe2 parsea). Está corregido y `mangfix --check` da «sin alias desfasados».

`src/Speed/Indep/SourceLists/zFe2.cpp` queda modificado **sólo** por esos 16 números.

---

## 5 · PROPUESTA MEDIDA Y NO APLICADA: `FEngInterfaceFEStrings.hpp` vale 43.876 B

`FEngSetGroupLanguageHash::Callback` (60 B) y `FEngGroupFEPrintf::Callback` (64 B) se emiten en
el cuerpo y el objetivo las difiere a la cola. La receta es la de siempre: bajar las dos clases
a `FEngInterfaceFEStrings.cpp` con el `Callback` dentro (no se puede dejar la clase en la
cabecera: `FEngGroupFEPrintf::Callback` llama a `DoFEngPrintf`, que es `static` del `.cpp`).

**Lo hice, lo medí y lo REVERTÍ**:

| | `.text` | `.rodata` | `.data` | total |
|---|---:|---:|---:|---:|
| con la edición | 53.894 | 148.176 | 121.577 | **323.647** |
| sin ella (estado actual) | 97.811 | 148.133 | 121.579 | 367.523 |
| | | | | **−43.876 B** |

**Por qué está revertida.** `FEngInterfaceFEStrings.hpp` la parsean **17 SourceLists** (todas
menos zGameModes, zOnline y las de biblioteca). Quitarle dos clases **desplaza el contador de
nombres privados de GCC en −12** en todas ellas. Medido con una sonda de tres líneas:

```
cabecera de HEAD  -> LastRandom.882
cabecera editada  -> LastRandom.870
```

Y `config/GOWE69/keep.lst` tiene **cuatro entradas con nombre privado**, todas ajenas:

```
265: zEAXSound2.o:LastRandom.25127
671: zWorld.o:changetime.14948
682: zWorld.o:index.14943
758: zWorld.o:windState.14947
```

Las cuatro quedarían apuntando a otra cosa y esas dos unidades perderían dato **en silencio**.
zWorld es del jefe y zEAXSound2 tiene agente. **No es mío para romperlo.** El paquete completo
es: la edición de `FEngInterfaceFEStrings.{hpp,cpp}` (guardada en el scratchpad como
`festrings_r56.{hpp,cpp}`) **+** renumerar esas cuatro entradas de `keep.lst` **+** volver a
pasar `mangfix zFe2`.

Comprobado que **`$LC` NO depende de ese contador** (la sonda da `$LC22` en las dos versiones),
así que las 1.256 entradas `$LC` de `keep.lst` no se ven afectadas por este eje.

### Las otras cabeceras compartidas que sí he tocado, y por qué son seguras

`uiRepSheetMain.hpp` (la parsea **zMain**) y `uiRepSheetRivalStreamer.hpp` (**zMain** y
**zFeOverlay**) también desplazan el contador (+3 y +9, medido con la misma sonda). Es seguro
porque:

* `keep.lst` no tiene ni una entrada de nombre privado de zMain ni de zFeOverlay (las suyas son
  `$LC`, `lbl_*` y `gap_*`, todas escritas a mano);
* el único fichero del árbol con alias `asm()` numerados es `zFe2.cpp`, que es mío;
* el desplazamiento sólo cambia **nombres de símbolo local**, no bytes de sección.

Las demás cabeceras que toco (`uiPause.hpp`, `uiCareerMain/Manager.hpp`, `uiProfileManager.hpp`,
`uiOptionsTrailers.hpp`, `uiRapSheet{US,VD,CTS}.hpp`, `uiMemcard.hpp`,
`MemoryCardHelper.hpp`) sólo las parsean zFe y zFe2, que son mías.

---

## 6 · PROPUESTA: 139 entradas de `keep.lst` desfasadas (no las he tocado)

Las ediciones renumeran los `$LC` de las dos unidades en **±3**:

| unidad | entradas a corregir | desplazamiento |
|---|---:|---|
| `zFe` | 101 | 94 de `−3`, 7 de `+3` |
| `zFe2` | 38 | todas `−3` |

Sin corregirlas el enlazador estripa **616 B** de `.rodata` de zFe (`linkdelta` pasa de
`rodata−48` a `rodata−640`) y **400 B** de zFe2 (`−880` → `−1280`). **Es una trampa de medida,
no una regresión**: con `keep.lst` re-resuelto por contenido las dos unidades vuelven a su
déficit de siempre (`zFe rodata−48 data−32`, `zFe2 rodata−880 data−160`), y todas las cifras de
este informe están tomadas así.

`python scripts/lcfix.py zFe zFe2` las corrige solo. **No lo he ejecutado** porque reescribe
`keep.lst`, que es entrada del enlace de los otros seis agentes. La copia ya corregida está en
`…/scratchpad/fe/keep.lst` para quien cierre la ventana.

Con `keep.lst` del proyecto (sin corregir), `trypromo` da `zFe → a2d3633f709b`; con el
corregido, `a1e7ff77cbf3`. Los dos ROTO: **falta la cola**.

---

## 7 · SORPRESAS

### 7.1 `bloque_funcion` y la definición de UNA LÍNEA: dos regresiones autoinfligidas

Mi utilidad de recorte buscaba el cierre de una función con `^\}` (llave en columna 0). Una
definición de una línea —`uiCareerCrib::~uiCareerCrib() {}`— **no acaba así**, así que el
recorte se comía **la función siguiente entera**. Perdí `NotificationMessage__12uiCareerCrib`
(512 B), `NotificationMessage__15uiCareerManager` (248 B), `Refresh__16UIProfileManager` (256 B)
y `Setup__15UIDeleteProfile` (240 B), y las cuatro veces el síntoma fue el mismo y es el que hay
que vigilar: **`seq.py` bajó de `comunes 921` a `comunes 917`**. Restauradas desde
`git show HEAD:` y confirmado `comunes 921`.

**Regla**: en cualquier edición masiva de fuente, el contador de símbolos comunes contra el
objetivo es la red. Si baja, has borrado código.

### 7.2 El prefijo común NO es la métrica; el desplazamiento acumulado sí

Arreglar el `namespace RealmcIface` subió el prefijo común de 128.488 a 158.392 B y el `.text`
sólo bajó **6 B**. No es contradicción: era un **intercambio de dos funciones de 32 B**, así que
todo lo de detrás ya estaba en su dirección. El prefijo de `seq.py` sirve para saber **qué
arreglar a continuación**; para saber **cuánto vale** hace falta el enlace.

### 7.3 `dirs.py` sobre el `.o` miente: hay que mirar el ENLACE

Comparando direcciones en el `.o` salían **766 funciones fuera de sitio (137.776 B)** y un
desplazamiento de +56 desde `uiRapSheetLogin` por culpa de
`CreateUIEATraxScreen__FP21ScreenConstructorData`, que el objetivo no tiene. En el **enlace** ese
símbolo se estripa y el desplazamiento **no existe**: son 226 funciones y 15.412 B. Es la misma
familia que `nfsmw-linked-es-la-metrica`, pero aplicada a *posiciones*, no a tamaños.

### 7.4 `build_direct.py zFe` compila TRES unidades — el atajo que sí funciona

Confirmo el aviso de la r55. El atajo sin sonda propia: **`python scripts/build_direct.py
zFe.cpp`** compila exactamente `zFe` (empareja por `w in spec[0]`, y `zFe.cpp` no es subcadena
de `zFe2.cpp` ni de `zFeOverlay.cpp`). Igual con `zFe2.cpp`.

### 7.5 `MemoryCard.cpp` no es UTF-8

Tiene un `0xA0` en la posición 8325. Cualquier guion de edición que abra con `utf-8` revienta;
hay que ir por `latin-1` para conservar el byte.

---

## 8 · SIGUIENTE

1. **El paquete de `FEngInterfaceFEStrings`** (§5): 43.876 B en zFe, y sólo cuesta cuatro
   entradas de `keep.lst` y un `mangfix zFe2`. Es trabajo de ventana, no de agente: hay que
   hacerlo cuando nadie esté midiendo zWorld ni zEAXSound2.
2. **`lcfix zFe zFe2`** (§6): 139 entradas, mismo momento.
3. **La cola de zFe**, 15.412 B: empezar por sacar `ClassKey__Q36Attrib3Gen8frontend` y
   `RaiseToPower__H1i10_i_i` de la cabeza de la cola (68 B que desplazan las 226 funciones), y
   probar el experimento de vtables de §3 (`UIProfileManager` declarada antes que
   `UIDeleteProfile`, y las `PM*` de vuelta al principio del `.cpp`).
4. **zFe2 no se ha atacado.** Sus 222.271 B siguen enteros; el brief los describe como siete
   escalones que suman 944 B mal colocados. Con `seq.py` sobre zFe2 debería salir el primer
   corte en un minuto — no me ha dado la ronda.
