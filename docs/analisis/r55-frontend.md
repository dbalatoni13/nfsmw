# r55 — el paquete del frontend: zAnim, zFe2, zFeOverlay

Agente `frontend`. Unidades: `Speed/Indep/SourceLists/{zAnim,zFe2,zFeOverlay}`.
Cabeceras poseídas: `CarCustomize.hpp`, `uiQRBrief.hpp`, `uiQRMainMenu.hpp`, `uiQRModeSelect.hpp`,
`uiQRCarSelect.hpp`.

**Las tres ediciones del encargo pagan. Ninguna promociona.** Y hay una cuarta, que no estaba
en el encargo, que vale más que dos de ellas: **una línea de `#include`**.

## El resumen, con las dos cifras de antes y de después

`fncmp` de las tres unidades: **0 funciones con el código distinto, antes y después.**
315 / 1.307 / 467. Ninguna regresión de código.

| unidad | B desplazados ANTES | DESPUÉS | símbolos en su sitio |
|---|---:|---:|---:|
| zAnim | 3.430 | 3.430 | 291 → 291 |
| zFe2 | 250.859 | **222.271** | 55 → **150** |
| zFeOverlay | 139.320 | **14.996** | 60 → **403** |
| **total** | **393.609** | **240.697** | 406 → **844** |

**−152.912 B de desplazamiento**, y 438 símbolos más caen en la dirección del objetivo.
Medido con el mismo enlace antes y después (`fe_desp`, ver §6). El mismo número sale del enlace
del paquete completo —los tres `.o` nuestros a la vez—: **393.786 → 240.874**.

Y aparte, **el cambio de dueño se cierra**: los 264.488 B del censo (146.384 de zAnim +
118.104 de zFe2) pasan a **cero**. Esa cifra no la ve la tabla de arriba porque el enlace de una
unidad usa los objetos ORIGINALES para las demás y ellos ya definen el símbolo disputado; se
verifica aparte (§1).

`trypromo` de las tres: **DOL ROTO**. Ninguna estaba cerca y ninguna lo está ahora; lo que
queda en cada una está nombrado en §5.

## 1. `CarCustomize.hpp:686-698` a `extern` — el cambio de dueño, cerrado

La cabecera **definía** catorce globales (2 `bool` + 12 `char*` con inicializador). Llega a las
tres unidades: a zFeOverlay por `CarCustomize.cpp`/`CustomizeManager.cpp`/`DebugCarCustomize.cpp`,
a zAnim por `AnimChooseArrest.cpp` y a zFe2 por `FEPackageData.cpp`, las dos últimas vía
`uiQRCarSelect.hpp:8`. Como zAnim es el objeto 1 del enlace, **zAnim se los quedaba**.

**Dónde va la definición, y por qué ahí.** El objetivo los emite en `.data:0x8043918C..0x804391C0`,
**pegados delante de `CustomizeShoppingCart::pParentPkg` (0x804391C4)** y detrás de los estáticos
de `Showcase` (`_8Showcase.FromFilter`, 0x80439188). `pParentPkg` se define en `CarCustomize.cpp`,
y entre `uiShowcase.cpp` y `CarCustomize.cpp` sólo hay `CustomizeManager.cpp`, que no aporta
`.data`. O sea: **al principio del `.data` de `CarCustomize.cpp`, justo delante de `pParentPkg`**.
Eso es lo que hay puesto.

Verificado en el `.data` de zFeOverlay, símbolo a símbolo:

    ANTES   g_bCustomizeManagerHasControl .. g_pCustomizeShoppingCartPkg   delta -1184
            _21CustomizeShoppingCart.pParentPkg                            delta -1128
    AHORA   los catorce                                                    delta -1128
            _21CustomizeShoppingCart.pParentPkg                            delta -1128

Los catorce pasan de ser una permutación de 56 B a compartir el desplazamiento uniforme de sus
vecinos. Y en el enlace de zAnim sola, **todo** el `.data` de zFeOverlay se mueve a la vez
(`carPosX`, los catorce, `pParentPkg` y `MarkerSelectInfos`, los cuatro a −1760): zAnim ya no le
quita nada a nadie.

Los inicializadores del objetivo salen del propio DOL (`0x80439194` → `803C76D0` → `"CustomizeMain.fng"`),
así que la forma de la fuente está confirmada, no supuesta.

**Coste medido, y hay que contarlo:** zAnim deja de emitir 12 cadenas `.fng` que el original no
tiene (224 B con el relleno), así que su `.rodata` **enlazada** pasa de `−1112` a `−1576`. **No es
una regresión de contenido**: el déficit real del objeto era 936 B antes y sigue siendo 936 B
(5.688 objetivo contra 4.976−224); lo que había era 224 B de basura tapando 224 B de agujero.
Sí es un `−464` más de tamaño en el DOL, y 240 de esos 464 son el `keep.lst` desfasado (§4).

## 2. Los tres destructores vacíos de zFeOverlay — **−124.056 B**, la edición que más paga

    uiQRBrief.hpp:46      ~UIQRBrief() override {}        y fuera uiQRBrief.cpp:45
    uiQRMainMenu.hpp:15   ~UIQRMainMenu() override {}     y fuera uiQRMainMenu.cpp:52
    uiQRModeSelect.hpp:15 ~UIQRModeSelect() override {}   y fuera uiQRModeSelect.cpp:29

Tres líneas. `zFeOverlay` pasa de **139.320 B desplazados a 15.264**, y de 60 símbolos en su
sitio a **393**. El `.over` (141 kB de código) pasa de 458 símbolos descolocados a 125.

Es el mecanismo del brief tal cual: fuera de clase se emite en el punto del parseo (nuestro
`_._9UIQRBrief` estaba en `0x803A6C70`, 122 kB antes de su sitio); dentro de clase es inline, va
a `saved_inlines` y `finish_file` lo vuelca en la cola, que es donde el objetivo lo tiene
(`0x803C5B14`).

## 3. Los cinco `asm()` de fichero de zFe2 al final de `zFe2.cpp` — **−28.588 B**

Movidos detrás del último `#include` (`Careers/UnlockSystem.cpp`) y delante del `asm` de
`.rodata` de 249 cadenas, en el orden del objetivo: primero `_IHandle__15IGenericMessage`
(12 B, de `FeGenericMessage.cpp:87`) y después los cuatro bloques de `VehicleDB.cpp:416-660`
(624 B, los 16 cuerpos de las clases locales de `FEPlayerCarDB`).

| símbolo | objetivo | ANTES | AHORA |
|---|---|---:|---:|
| `_IHandle__15IGenericMessage` | 0x8017CCF4 | −222.516 | **−600** |
| bloque `FEPlayerCarDB` (16 símbolos) | 0x8017FAA4 | −110.480 | **−12.076** |
| `_GLOBAL_.I.__10HudElementPCcUx` | 0x8017FE00 | — | **+0** |

Los dos escalones mayores de la escalera desaparecen: los 636 B (12 + 624) se restan de **todos**
los escalones que había detrás, y el histograma de desplazamientos de `.text` pasa de
`+12 / +100 / +440 / +504 / +584 / +620 / +1244 / +1384 / +1496 / +1704` a
`+0 / +88 / +428 / +492 / +572 / +608 / +748 / +860 / +1068`. **95 símbolos más caen a delta 0**
(54 → 149) y `.text` desplazado baja de 235.456 a 206.868 B.

Los 12.076 B que le quedan al bloque de `FEPlayerCarDB` **son estructurales y no se pueden quitar
con un `asm`**: el objetivo lo emite al FINAL de la cola de `finish_file`, y un `asm()` de fichero
se emite en el parseo, o sea siempre ANTES de esa cola. El final del `.cpp` es lo más tarde que
existe. Es el techo de esta palanca, no un residuo por trabajar.

Aviso operativo: mover el `asm` desplaza el contador `DECL_UID` y **zFe2 deja de enlazar**.
`python scripts/mangfix.py zFe2` lo arregla, pero hay que iterar
`build_direct` → `mangfix` **hasta que diga «todo al dia»** (a mí me hicieron falta dos vueltas
la primera vez: la corrección de la primera pasada movía el contador otros 3).

## 4. La cuarta edición, que no estaba en el encargo: **un `#include`**

`FEPkg_GarageMain.cpp:2` → `#include ".../customize/CarCustomize.hpp"`.

Después de §2 quedaba en zFeOverlay una permutación limpia y grande: **los destructores del
grupo `Customize*` y los del grupo `UIQR*` estaban INTERCAMBIADOS** (los QR a −2.364, los
Customize a +3.444), y **las vtables de los dos grupos también, en sentido contrario**
(Customize a −2.352, QR a +264/+312). Esa simetría invertida es la firma de dos colas que se
recorren en sentidos opuestos: `saved_inlines` hacia delante y `walk_globals` hacia atrás.

Un cuerpo definido dentro de la clase se encola **cuando se cierra la clase**. Las clases de
`CarCustomize.hpp` se cerraban en el include 56 (`uiQRCarSelect.cpp`), detrás de `uiQRBrief.hpp`
(include 51). El objetivo las quiere delante. Adelantar la cabecera al include 48 lo arregla:

    suma de |desplazamiento| en la ventana .over de zFeOverlay:  291.504 -> 88.632 B
    simbolos exactamente en su sitio:                                393 -> 403
    bloque de destructores Customize/QR:            -2364 / +3444 -> +20 / +52
    bloque ENTERO de vtables:      permutado en tres grupos -> desfase UNIFORME -1.352

Después de esto el bloque de vtables de zFeOverlay **está en el orden del objetivo salvo tres
símbolos** (`_vt.8CarDatum`, `_vt.15UIQRTrackSelect`, `_vt.13UIQRCarSelect`); el −1.352 uniforme
ya no es orden, es el déficit de tamaño de `.over` (`linkdelta over−1360`).

Probé las dos colocaciones. Meterlo en `zFeOverlay.cpp` **antes** de `FEPkg_GarageMain.cpp` da la
misma cifra de bytes, pero deja las vtables a −1.560 en vez de −1.352 y saca
`_vt.16GarageMainScreen` del grupo uniforme; el objetivo la tiene **la última** de todas, que es
lo que sale si `GarageMainScreen` es la primera clase que se cierra. Por eso el include va
**dentro** de `FEPkg_GarageMain.cpp`, detrás de su propia cabecera, y no en la SourceList.

## 5. Qué le queda a cada unidad, y no es orden

* **zAnim — 3.430 B, y el `.text` está intacto.** Los 27 símbolos de `.text` desplazados suman
  460 B. Lo que le falta son **760 B de pool de literales que no existen en el árbol**
  (`AC_PLAY_TYPE_FLAG_*`, `WAC_*`, los nombres de `CAnimScene`...): fuente sin escribir, no orden.
  Sigue en pie el diagnóstico de `r55-med-zAnim.md`. **Mis tres ediciones no le mueven ni un byte
  de desplazamiento** — su ganancia es la del dueño (§1), que sólo se ve en el DOL completo.
* **zFe2 — 222.271 B, una escalera de siete escalones pequeños.** Ya no hay ningún escalón grande:
  `+88` (`SetMapHeader__21ChoppedMiniMapManager`), `+340` (`__10StatsPanel`), `+64`
  (`__17PhotoFinishScreen`), `+80`, `+36`, `+140` (`BroadcastMessage__16FEPackageManager`),
  `+196` (`__12ArrayScripts`). Suman 944 B mal colocados que arrastran 200 kB. Más dos zonas
  revueltas de verdad: `HudResourceManager`/`FEngHud` (0x8014C424..0x801500B0) y
  `LoadingTips`/`FEngTextInputObject`. Cada escalón es un destructor o un `ClassKey` que sale en
  el punto de la clase en vez de en la cola — **la misma palanca de §2**, una por una.
* **zFeOverlay — 14.996 B, y 5.620 de ellos son `.bss`.** Le falta el hueco anónimo
  `gap_07_804AB6B8_bss` de **4.104 B** al final de la ventana (`linkdelta bss−4096`), que es un
  búfer estático que no existe en el árbol. Del `.over` quedan 8.608 B, casi todo el desfase
  uniforme de −1.352 (déficit de tamaño, no orden) y tres vtables permutadas.

## 6. La medida, y su control

`fe_desp` enlaza dos veces —con el `.o` extraído y con el nuestro— y cuenta, **dentro de la
ventana de `splits.txt`**, los bytes de símbolos comunes que caen en otra dirección. Es la misma
cuenta de `r55-med-zFe2.md` y reproduce su 235.456 al byte, así que es comparable con el triaje.

**El control que tenía que cambiar, y cambió**: la columna «en su sitio» (406 → 844 símbolos,
67.096 → 220.008 B). Una medida que sólo bajase «desplazados» podría estar perdiendo símbolos.

**Y el control que NO cambió, que es un aviso de herramienta**: `fe_desp` de **una** unidad no ve
el cambio de dueño de §1 —sus cifras son idénticas antes y después—, porque en ese enlace los
demás objetos son los ORIGINALES y ellos definen el símbolo. Para el dueño hay que mirar el
desplazamiento **relativo a los vecinos** dentro de la ventana, que es lo que hice. Quien mida un
cambio de dueño con un enlace de una sola unidad no verá nada y creerá que la edición no paga.

Segundo aviso: `fe_desp` es **binaria** —un símbolo a +20 cuenta igual que uno a +3.444—, así que
para §4 hace falta la suma de |desplazamiento|. Con la cuenta binaria, §4 «sólo» vale 268 B; con
la suma, vale 202.872. Las dos son ciertas y hay que dar las dos.

`dolwhere` **no sirve en estas tres unidades**: da `LAS SECCIONES NO COINCIDEN` en las tres, antes
y después, porque las secciones del DOL no cuadran mientras el árbol tenga unidades sin promocionar.

## 7. Lo que hay que hacer con esto, y es ATÓMICO

1. **Las tres unidades van juntas.** Poner `extern` en `CarCustomize.hpp` sin la definición en
   `CarCustomize.cpp` deja las tres sin enlazar.
2. **`python scripts/lcfix.py zAnim zFeOverlay`** — **27 entradas de `keep.lst` desfasadas**
   (15 de zAnim, 12 de zFeOverlay). Quitar las 12 cadenas `.fng` de zAnim corre su contador `$LC`
   12 posiciones, y moverlas dentro de zFeOverlay lo corre otras 12. **No lo he aplicado: no
   poseo `keep.lst`.** Sin esto el DOL se rompe en silencio (`keepchk` lo da por bueno) y ~240 B
   de las cifras de `.rodata` de zAnim de arriba son ese desfase, no mi edición.
3. **`python scripts/mangfix.py zFe2`** después de cualquier recompilación de zFe2, iterando
   hasta «todo al dia». Ya está al día en el árbol.

## 8. Propuestas que no puedo aplicar (no poseo el fichero)

* **`feArrayScrollerMenu.hpp:137`, `struct CarDatum`.** En zFeOverlay `_._8CarDatum` sale
  **−5.540 B** de su sitio y su vtable **+1.376**: es el único desorden grande que le queda al
  `.over`. La clase se cierra demasiado pronto porque la cabecera se incluye antes de tiempo.
  La cabecera llega a zFe y zFEng, que no son mías.
* **`zFeOverlay`: el hueco de `.bss` de 4.104 B.** Reproducirlo pide un estático de ese tamaño
  declarado el último de la unidad **y** una entrada en `keep.lst` (`-strip-unused-data` va por
  símbolo y nadie lo referencia). Vale `bss−4096` y es condición necesaria para promocionar
  zFeOverlay.

## 9. Sorpresas (contradicen el encargo o el brief)

1. **La edición que más paga no estaba en el encargo.** El `#include` de §4 vale 202.872 B de
   suma de desplazamiento en zFeOverlay, más que las tres asignadas juntas por esa métrica.
2. **`CarCustomize.hpp` a `extern` no le quita ni una cadena a zFe2.** El encargo daba 264.488 B
   entre zAnim y zFe2; el `.rodata` de zFe2 **no cambia** (`−88` en el objeto, antes y después)
   porque zFe2 ya tenía esas doce cadenas en su pool por otro sitio y GCC las funde dentro de la
   TU. Sólo cambia su `.data` (−56).
3. **La cifra de zFe2 del encargo (111.344 B por el bloque de `FEPlayerCarDB`) no es alcanzable
   con un `asm`.** El objetivo lo emite dentro de la cola de `finish_file` y un `asm()` de fichero
   no puede llegar ahí. Lo cobrado son 28.588 B, y el techo de la palanca son los 12.076 B que
   quedan.
4. **Mover el `asm` rompe el enlace de zFe2** por el `DECL_UID`, y `mangfix` necesita **dos**
   pasadas, no una. El aviso del propio fichero decía una.
5. **`fe_desp`/`movidos` de una sola unidad son ciegos al cambio de dueño** (§6). El encargo pedía
   medir con ellos las tres ediciones; para la primera no valen.
