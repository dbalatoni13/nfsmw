# r57 · `fe` — 460.071 B: el frente de zFe/zFe2 no era el ORDEN, era la `.data`

Unidades: `zFe`, `zFe2`. Ni un commit, ni un `git add`. `configure.py`, `config/GOWE69/*`,
`splits.txt` y `keep.lst` **sin tocar** (`git status config/ configure.py` vacío al cerrar).
`lcfix --check` deja **cero** correcciones pendientes de mis dos unidades.

## Titular

| unidad | antes (r56) | después | Δ | DOL |
|---|---:|---:|---:|---|
| **zFe** | 368.918 B | **203.312 B** | **−165.606** | ROTO (`abedf8cad5aa`) |
| **zFe2** | 528.653 B | **234.188 B** | **−294.465** | ROTO (`176ca18d8eaa`) |
| | | | **−460.071** | |

(bytes distintos del ELF **enlazado** contra la base, todas las secciones; con la métrica de
r56 —`.text+.rodata+.data`— es **−458.465**)

Por sección, y aquí está la historia de la ronda:

| | zFe antes | zFe después | zFe2 antes | zFe2 después |
|---|---:|---:|---:|---:|
| `.text` | 97.811 | **52.368** | 233.691 | 77.602 |
| `.rodata` | 148.133 | 148.128 | 153.208 | 151.972 |
| **`.data`** | **121.579** | **1.659** | **138.274** | **2.502** |

`fncmp` **antes y después**: `0 de 921` y `0 de 1307` funciones con el código distinto.
**Cero regresiones.** Tres compilaciones seguidas con el mismo sello:
`zFe 946602b0594b…`, `zFe2 c41cd7e82a7f…`. `checksplits`, `prefijochk`, `gapchk zFe`,
`gapchk zFe2` y `mangfix --check` limpios.

---

## 1 · La palanca de la ronda: `inline` en la definición FUERA de línea

El mecanismo del brief dice «cuerpo EN CLASE = inline = va a la COLA; cuerpo FUERA = sale EN
EL PUNTO del parseo». Falta un tercer caso, y es el útil:

> **`inline` delante de una definición fuera de línea manda el cuerpo a la cola de
> `finish_file` sin tocar la cabecera.**

Es `DECL_COMDAT` puro. Y **resuelve la propuesta §5 de `r56-fe.md`, que valía 43.876 B y estaba
bloqueada**: la r56 quería bajar `FEngSetGroupLanguageHash` y `FEngGroupFEPrintf` de
`FEngInterfaceFEStrings.hpp` al `.cpp`, y eso desplazaba el contador de nombres privados de GCC
en las **17 SourceLists** que parsean la cabecera, dejando rancias cuatro entradas de
`keep.lst` de zWorld y zEAXSound2. Con `inline`:

* la cabecera **no se toca** —el contador de las otras 16 unidades no se mueve—;
* `FEngInterfaceFEStrings.cpp` lo incluye **sólo `zFe.cpp`**;
* `__FILE__` no cambia (el otro efecto secundario de mover el cuerpo a la cabecera);
* medido: **−44.060 B** en zFe, contra los 43.876 de la ruta bloqueada.

Aplicada **once veces** en la ronda, siempre con `fncmp` a cero después:

| función(es) | dónde estaban | dónde las quiere el objetivo |
|---|---|---|
| `FEngSetGroupLanguageHash::Callback`, `FEngGroupFEPrintf::Callback` | cuerpo de zFe | cola, `zFe.o 0x029D2C/0x029D9C` |
| `PostPursuitInfractionsScreen::{Create,TextureLoadedCallback}` | cuerpo | cola, `zFe2.o 0x03CAC4` |
| `RaceResultStat::{~RaceResultStat,Draw}` | cuerpo | cola, `0x03BD20` |
| `SillyTextureStreamerManager::{MakeSpaceInPoolCallbackBridge,LoadCallbackBridge}` | cuerpo | cola, `0x03B968` |
| `MovieScreen::~MovieScreen` | cuerpo | cola, `0x03CCF0` |
| `Attrib::Gen::{presetride,fecooling,infractions}::ClassKey` | cuerpo (¡en medio de FEPlayerCarDB!) | cola, `0x03CFC0` |
| `FEPackageManager::~FEPackageManager` | cuerpo | cola, `0x03B23C` |
| `IconPanel::{GetHead,IsHead,IsTail,IsEndOfList}`, `IconScroller::{IsHead,IsTail}` | cuerpo | cola, `0x03AA38` |
| `CustomTuningScreen::~CustomTuningScreen` | cuerpo | cola, `0x03D298` |
| `Physics::Tunings::Default` | cuerpo | cola, `0x03A4F0` |
| `Chyron::operator new(size_t)` | **cabecera** | cola (y ver §4) |

**AVISO**: `inline` **sí** desplaza el contador de nombres privados de la unidad. Siete de ellos
lo movieron −6 y `zFe2` dejó de enlazar con 16 `Reference to undefined symbol`. Se arregla con
`mangfix.py zFe2`, pero **hay que acordarse**: es el mismo síntoma de `nfsmw-clase-local-alias`.

---

## 2 · El cuerpo de zFe2 casa entero: 237.288 B de prefijo común

zFe2 no se había atacado nunca. Con `seq.py` (difflib sobre la secuencia de símbolos de `.text`)
el prefijo común pasó de **5.304 B a 237.288 B** de los 250.096 que mide su `.text`; los 12.808
que quedan **son todos de la cola de `finish_file`**. Los cortes bajaron de 53 a los de la cola.

Nueve ficheros reordenados, ninguno una cabecera (`fncmp` a cero después de **cada uno**):

| fichero | qué | evidencia |
|---|---|---|
| `FeMinimapStreamer.cpp` → `FeMinimap.cpp` | `LoaderMiniMap`/`UnloaderMiniMap` se definen detrás de `GetVehicleVectors`, no en el streamer | `zFe2.o 0x0017C0` |
| `FEPkg_Hud.cpp` | **las 30 definiciones reordenadas**: `HudResourceManager` entero ANTES de `FEngHud`, y dentro de `FEngHud` el orden ctor/dtor/Update/FadeAll/SetInPursuit/Joy*/JoyHandle/DetermineHudFeatures/… | bloque cerrado `0x009964..0x00D5F0`, mismo principio y mismo final en los dos |
| `FELoadingTips.cpp` | `GameTipInfoTable` al principio del fichero **y** las 15 funciones en orden del objetivo | `.rodata`: las `TIP_*` van delante de `"Loading_Tips.fng"` (`0x20DC` contra `0x2278`) |
| `FELoadingControllerScreen.cpp` | 11 definiciones, `SetupControllerConfig` al principio | `0x018338` |
| `feKeyboardInput.cpp` | 11 definiciones | `0x01BDC8` |
| `FEngFrontend.cpp` | `FindButtonNameHashForFEString` al principio, `FEngHashString` al final | `0x02DBEC`/`0x02DE2C` |
| `feIconScrollerMenu.cpp` | `IconScroller::GetOptionIndex` entre `RemoveAll` y `Scroll` | `0x0302E0` |
| `CustomTuning.cpp` | 15 definiciones, `NotificationMessage`/`ScrollTypes` detrás del ctor | `0x035DA4` |

**El bloque de `FEPkg_Hud.cpp` es el ejemplo que hay que recordar**: 15.500 B de `.text` con
el mismo primer byte y el mismo último byte en los dos objetos, y la única diferencia era el
orden de dos grupos de funciones dentro de UN fichero. Valió 20.436 B de prefijo de golpe.

---

## 3 · **EL HALLAZGO**: la `.data` de las dos unidades estaba a 32 y a 160 B, y los huecos ya estaban en `keep.lst`

`linkdelta` llevaba rondas diciendo `zFe … data-32` y `zFe2 … data-160` y nadie lo había mirado
porque parecía calderilla. **No lo es**: son **desplazamientos**, no bytes.

    zFe  .data ENLAZADO: en su direccion 0 (0 B), fuera 2880 (262.945 B)
       0x80415180  172  -> 0x80415160 (-32)  pad_06_80415180_data
       ... los 2.880 simbolos, TODOS a -32

Los **2.880 símbolos de `.data` que van detrás de zFe en la imagen** caían 32 B antes. Lo mismo
en zFe2 con −160. Y lo que faltaba no era código: eran **siete huecos anónimos en zFe y
dieciséis en zFe2**, y **las 25 entradas de `keep.lst` ya estaban escritas** (líneas 361-369 y
370-391). Nadie había escrito los `asm()`.

| unidad | `.data` bytes distintos antes | después |
|---|---:|---:|
| zFe | 121.579 | **1.659** |
| zFe2 | 138.274 | **2.502** |

`secdiff` da ahora `zFe .data: objetivo 54 símbolos 1684 B ; nuestro 54 símbolos 1684 B`, y la
disposición coincide **símbolo a símbolo y dirección a dirección**.

### Cómo se caza

1. `linkdirs.py <unidad> .data` — si dice «fuera N (muchos B)» con un delta CONSTANTE, es un
   déficit de tamaño, no de orden.
2. `scripts/shift.py` (nuevo, en el scratchpad) — imprime sólo **dónde CAMBIA** el
   desplazamiento acumulado: ahí es donde falta el dato.
3. `datalay.py <unidad> .data` (nuevo) — las dos disposiciones lado a lado, con el **contenido**
   de lo que sólo tiene el objetivo, listo para el `asm()`.
4. El `asm()` va **en el punto de parseo** que le toca: entre dos `#include` de la SourceList si
   los vecinos están en ficheros distintos, y dentro del `.cpp` si están en el mismo.

### La trampa que casi lo tira: `.balign 8`

Con los siete huecos de zFe puestos, `linkdelta` pasó de `data-32` a **`data+32`** y la medida
EMPEORÓ. La causa: el bloque `gap_06_8041BBD8_data` que ya existía en `zFe.cpp` llevaba
`".balign 8"`, y al meter `lbl_8041BBCC` delante el hueco cayó en `0x60C` —que no es múltiplo
de 8— y el ensamblador metió 4 B de relleno. Con `.balign 4` la sección mide **exactamente** lo
que el objetivo. **En un `asm()` de hueco, `.balign` nunca debe ser mayor que el alineamiento
real del sitio.**

---

## 4 · GCC 2.95 emite TODO `static` de fichero con inicializador, lo use alguien o no

Los 4 B que sobraban en cada unidad después de poner los huecos:

* **`ChyronScreenPtr`** — `static MenuScreen *ChyronScreenPtr = nullptr;` vive en
  `FEPkg_Chyron.hpp`. `FEManager.cpp` incluía esa cabecera **sólo para tres funciones libres**
  del final, y se llevaba el static a la `.data` de zFe, donde el objetivo no lo tiene. Quitado
  el `#include` y puestas las tres declaraciones a mano.
  De paso: el cuerpo de `Chyron::operator new(size_t)` estaba **dentro de la clase** y era lo
  único que referenciaba el static; ahora vive en `FEPkg_Chyron.cpp` con `inline`.
* **`MinimapDispY`** — `static float MinimapDispY = 0.0f;` en `FeMinimap.cpp`, **sin un solo
  uso en todo el árbol**. El objetivo no lo tiene y sí tiene `gap_06_8041BC98_data` (4 B) en su
  sitio exacto. Sustituido por el hueco.

La regla, para el dossier: **si una unidad tiene 4 B de `.data` de más, busca un `static` de
fichero con inicializador que no use nadie** — y muy probablemente venga de una cabecera.

---

## 5 · Orden de DATO: 21 reordenados más, todos leídos del objetivo

Ninguno mueve una función, así que el orden de `.text` no se toca. En zFe:
`uiRepSheetRivalFlow::mInstance` delante de `ScreenNames`, `RCMPDecodeBuffer` detrás de
`MovieVolumeArray`. En zFe2: los estáticos de `HudResourceManager`, `KeyboardActive`/
`gFEKeyboard`, los cuatro de `PhotoFinishScreen`, `IsDebugPlayMovie`, los tres de `LoadingTips`,
`InGameTutorialScreenName`, los cuatro de `Localize.cpp`, `pWideCharHistogram`,
`mpobFERenderObjectSlotPool`, `FEPackageData::mInScreenConstructor`, `FontReplacementTable`.

Y uno que es un **cambio de dueño de fichero**: `g_KBDelaySeconds` estaba en
`feUIWidgetMenu.cpp` (include #117 de zFe2) y el objetivo lo tiene en `.data:0x8041C470`,
pegado detrás de `gDialogHandle` de `feDialogBox.cpp` (include #101). Desde el #117 no puede
caer ahí: la definición se ha movido a `feDialogBox.cpp` con un `extern` donde estaba.

---

## 6 · Vtables de zFe: los seis del grupo de perfiles, cerrados

`vtord zFe` daba 157 contra 157 con las **quince primeras** descolocadas. Ahora son **nueve**.

El experimento que la r56 dejó apuntado (§3) era correcto y está aplicado:

* `UIProfileManager` se declara **antes** que `UIDeleteProfile` en `uiProfileManager.hpp`;
* las cinco `PM*` **vuelven a la cabecera**, delante de las dos `UI*`, con el cuerpo de `React`
  todavía dentro de la clase (o sea, siguen saliendo en la cola).

Eso deja los índices 9-14 **idénticos al objetivo**. Confirma de paso la regla del brief y la
matiza: **lo que coloca la vtable es dónde se COMPLETA la clase, y una clase en el `.cpp` nunca
puede completarse antes que una de su propia cabecera.** Si el objetivo las quiere antes, la
clase tiene que estar en la cabecera; el cuerpo va a la cola igual con tenerlo dentro.

Lo que falta (índices 0-8) es el grupo de memcard: el objetivo completa
`UIMemcardList, UIMemcardKeyboard, UIMemcardBase, UIMemcardMain`, luego
`MyMutex, MyThread, MemcardCallbacks`, y **`UIMemcardBoot` y `FEMemWidget` los últimos**.
Nosotros parseamos `MemoryCardCallbacks.cpp` antes que las cabeceras de `uiMemcard*`. Se
arregla adelantando los `#include` de las **cabeceras** (no de los `.cpp`) en `zFe.cpp`, que es
el truco de separar disparadores. **No lo he hecho**: son 896 B de vtables y quería el `.data`.

---

## 7 · Lo que queda, medido: **es `.rodata`, y cuesta 77.602 B de `.text` AJENO**

Las dos unidades quedan con `.text +0` y **sólo** `rodata-48` (zFe) y `rodata-872` (zFe2).
Ese déficit es ahora **todo el problema**, y no es sólo suyo:

    fedist zFe2 --rangos
    .text  ... 77.602 B distintos
       0x800034FA..0x80003520  38 B      <-- el PRINCIPIO de la imagen
       0x8000359A..0x800035C4  42 B
       ...

**Los 77.602 B de `.text` de zFe2 no están en zFe2**: empiezan en `0x800034FA`, a 3,5 MB de su
código. Son las instrucciones de TODAS las demás unidades que apuntan a cadenas colocadas
detrás de la `.rodata` de zFe2, desplazadas 872 B. Es exactamente la misma forma que tenía la
`.data` antes de esta ronda.

O sea: **cerrar `rodata-872` de zFe2 vale del orden de 77.602 B de `.text` + parte de los
151.972 de `.rodata`**, y `rodata-48` de zFe otro tanto a su escala. Es el frente de
`prefijotu.py` (el vocabulario compartido que `cc1plus` no emite porque nadie lo referencia):
`rodorden zFe2` da **940 cadenas del objetivo contra 948 nuestras, 648 en secuencia**, y entre
las que faltan hay nombres de clase (`PostPursuitInfractionsScreen`, `RaceResultStat`,
`LapStat`, `ConfirmDialog`, `InfoStat`, `BootFlowManager`…) que el árbol no genera.

**OJO**: eso renumera `$LC` y obliga a `lcfix`, que reescribe `keep.lst`. Es trabajo de
ventana, no de agente que corre en paralelo.

---

## 8 · SORPRESAS

### 8.1 Nuestro objeto tiene cinco rutas `__FILE__` que el original no tiene

`.rodata` de `zFe2.o`: el objetivo tiene **una sola** ruta de fichero
(`d:/mw/speed/indep/bware/inc/bware.hpp`, el prefijo de bWare) y nosotros **seis**:
`FEDatabase.cpp`, `FeMinimapStreamer.cpp`, `feIconScrollerMenu.cpp`, `FEPKg_PostRace.cpp`,
`uiInfractions.cpp`. Salen de `new (__FILE__, __LINE__)`. Es dato de más en la `.rodata`, y por
eso conviene NO mover esos cuerpos a cabeceras (cambiaría la cadena, no la quitaría).

### 8.2 El heredoc de Git Bash se come los `\n` de un `asm()`

Escribir los bloques `asm()` desde `python - <<'PYEOF'` dejó **saltos de línea de verdad dentro
de las cadenas** en vez de `\n`. Hay que escribir el guion a un fichero y ejecutarlo. De paso:
el bloque `gap_06_8041BBD8_data` que ya estaba en `zFe.cpp` **tiene ese defecto desde antes**
(`".previous` + salto + `"`); compila, pero es una cadena sin terminar.

### 8.3 `MemoryCard.cpp` y compañía no son UTF-8

Confirmado el aviso de la r56: todo guion de edición abre en `latin-1` y escribe en `latin-1`,
y **hay que respetar el final de línea de cada fichero** (`zFe.cpp` es LF; casi todos los
`.cpp` de Frontend son CRLF). `eol()` en los guiones del scratchpad.

### 8.4 Dos medidas que no pagaron, y las cuento

* Cambiar `UIProfileManager`/`UIDeleteProfile` de orden en la cabecera: **exactamente 0 B** de
  diferencia en la medida, pero deja los dos índices de vtable donde el objetivo los quiere.
  Se queda: es correcto aunque no pague hoy.
* Subir las cinco `PM*` a la cabecera: **−14 B**. Se queda por lo mismo (cierra los índices
  9-14 de `vtord`).

---

## 9 · Ficheros tocados y PROPIEDAD

23 ficheros. **Comprobado con cierre transitivo de `#include` que TODOS los parsean sólo `zFe`
o `zFe2`** (guion `quien.py` en el scratchpad): **cero unidades ajenas expuestas**, y por tanto
cero regresiones que medir bajo la regla 5. La única cabecera compartida que toco
(`FEPkg_Chyron.hpp`) ahora la parsea **sólo zFe2** justamente porque le he quitado el
`#include` a `FEManager.cpp`.

`config/GOWE69/keep.lst` **no hace falta tocarlo**: las 25 entradas de hueco que necesitaba ya
estaban escritas.

---

## 10 · SIGUIENTE

1. **`rodata-872` de zFe2 y `rodata-48` de zFe.** Es lo único que queda y vale ~230 kB en zFe2
   (77.602 de `.text` ajeno + buena parte de los 151.972 de `.rodata`). `prefijotu.py zFe2`
   genera el bloque; hay que correr `lcfix` después, así que es trabajo de ventana.
2. **Las nueve vtables del grupo memcard de zFe** (§6): adelantar los `#include` de las
   *cabeceras* `uiMemcard*.hpp` por delante de `MemoryCardCallbacks.cpp` en `zFe.cpp`.
3. **La cola de las dos unidades**: zFe tiene 226 funciones y 15.412 B fuera de sitio (todas en
   los últimos 16 kB), zFe2 12.808 B. El primer culpable de zFe sigue siendo
   `RaiseToPower__H1i10_i_i` (56 B, el objetivo la pone LA ÚLTIMA del `.text` y nosotros la
   primera de la cola).
4. **El frente está CONTADO, no supuesto** (regla de `nfsmw-extrapolar-frentes`). El guion
   `keepfalta.py` cruza las entradas `gap_*`/`lbl_*` de `keep.lst` con los símbolos que define
   nuestro `.o`, y da **258 huecos reservados en el enlace que la fuente nunca escribe**, en
   **29 unidades**. zFe queda a cero y zFe2 con cuatro (tres son cadenas del vocabulario
   compartido y el otro es relleno de alineamiento real). El reparto:

   | unidad | sin escribir | `linkdelta` hoy |
   |---|---:|---|
   | `zWorld` | 54 | `rodata-472 data-128` |
   | `zPhysicsBehaviors` | 26 | `rodata-40 data-224 bss-348` |
   | `zWorld2` | 22 | `rodata-408 data-256 bss+64` |
   | `zCamera` | 19 | `rodata-1384 data-224 bss+32` |
   | `zTrack` | 18 | `rodata-352 data-416 bss-160` |
   | `zPlatform` | 13 | `rodata-728 data-64` |
   | `zGameplay` | 11 | `rodata-136 data-128` |
   | `zAI` | 10 | `rodata-456 data-384` |
   | `zEAXSound2` | 10 | IGUAL |
   | `zEagl4Anim` | 9 | `rodata-640 data-96 bss-128` |
   | `zLua` | 6 | `rodata-720 data-320` |
   | `zSpeech` | 6 | `rodata-232` |
   | `zMain` | 5 | IGUAL |

   **Diez de las trece tienen el `data-N` que delata el hueco de `.data` sin escribir**, que es
   exactamente la forma que tenían zFe (−32) y zFe2 (−160). En estas dos ha valido **260 kB**.
   El procedimiento es el de §3 y son un par de horas por unidad como mucho.
   (`keepfalta.py`, `shift.py` y `datalay.py` quedan en el scratchpad de la ronda.)
