# r55 — medida de `zFe2` (250.732 B): es ORDEN, y el orden lo fija un `asm()` de fichero

2026-09-09. Unidad: `Speed/Indep/SourceLists/zFe2`. Reconstruida con
`python scripts/build_direct.py Speed/Indep/SourceLists/zFe2` — **1 ok, 0 fallidas al primer
intento**, sin necesidad de reintento.

**Veredicto corto: `zFe2` NO tiene contenido que escribir. Le quedan 8 bytes de dato
(dos palabras, las dos localizadas y con el valor del objetivo delante) y TODO lo demás es
orden de emisión. Y no tiene ningún cambio de dueño: los 12.076 B que `movidos.py` le imputa
como robados a otros objetos son un artefacto — se estripan o los gana el otro objeto.**

---

## 1. `reorden.py zFe2` — la pregunta principal

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .ctors          0        0        0          0        0
    .data         125     1122      401          3       22
    .rodata      1221     1930      529         20     1091
    .text        1306    62404     6763          0        0   (17 de otro TAMANO)

Literalmente es **MIXTO** (0 / 20 / 3). Pero las 23 palabras hay que abrirlas una a una, y al
hacerlo **21 de las 23 son reubicación, no contenido**:

* **`.rodata`: las 20 palabras son CERO contenido.** Son trece vtables (las ocho de las
  clases locales de `FEPlayerCarDB`, `_vt.11MovieScreen`, `_vt.14RaceResultStat`,
  `_vt.16FEPackageManager`). Comparé palabra a palabra los dos ELF enlazados: **cada una de
  las 20 apunta al MISMO símbolo en los dos enlaces**, sólo que a otra dirección porque ese
  símbolo se ha movido. 4 casan por nombre exacto; las otras 16 casan ignorando el sufijo
  `.NNNNN` del mangle (`...NumInfraction.35650` contra `...NumInfraction.32045`), que es el
  alias doble que pone `mangfix`. Ejemplo, `_vt.16FEPackageManager` (24 B, 6 palabras):

      +12  8017DCFC _._16FEPackageManager | 8016D978 _._16FEPackageManager   <<<

  El slot del destructor. Cambia porque el destructor está 66.436 B antes, no porque el dato
  sea otro.
* **`.data`: 1 de las 3 también es reubicación.** `ScreenFactoryData+996` apunta a
  `Create__28PostPursuitInfractionsScreen`, que se ha movido −188.800 B. Contenido cero.

**El contenido REAL de `zFe2` son dos palabras, 8 bytes, y las dos están identificadas:**

| símbolo | offset | objetivo | nuestro | qué es |
|---|---:|---:|---:|---|
| `Minimap::kGameplayIconInfo` | +304 | `01000000` | `00000000` | elemento 15 (`GII_CHECKPOINT`), campo `mItemType` |
| `gDialogHandle` | +0 | `00000001` | `00000004` | valor inicial |

`mItemType` es una **máscara de bits**, no un ordinal — elem 1 = `0x100`
(`WMIT_SPRINT_RACE`), elem 14 = `0x100000` (`WMIT_SPEED_TRAP`), elem 15 = `0x1000000`. Y
`0x01000000 = 16777216 = WMIT_TOKEN` (`uiWorldMap.hpp:37`).

    src/Speed/Indep/Src/Frontend/HUD/FeMinimap.cpp:41
      { GII_CHECKPOINT, WMIT_NONE,  "MMICON_CHECKPOINT", 0x0, 0x0 },   <- nuestro
      { GII_CHECKPOINT, WMIT_TOKEN, "MMICON_CHECKPOINT", 0x0, 0x0 },   <- el objetivo

    src/Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.cpp:19
      static int gDialogHandle = 4;   <- nuestro
      static int gDialogHandle = 1;   <- el objetivo

**Aviso de herramienta (ver §7): `reorden.py` no enmascara las reubicaciones `ADDR32` de
`.rodata`/`.data`.** Su `mascara()` sólo entiende codificación de instrucción PowerPC, así
que un puntero de vtable se compara en crudo y sale como «contenido» en cuanto la mitad alta
cambia (`0x8017...` contra `0x8016...`). Los punteros que se mueven dentro de los mismos
64 kB no salen. Por eso el 20 de `.rodata` no es una medida de contenido: es una medida de
cuántos punteros cruzaron una frontera de 64 kB.

## 2. `movidos.py Speed/Indep/SourceLists/zFe2`

    zFe2: 39149 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
       12960 cambian de direccion, pero 12425 es ARRASTRE (tres terminos: donde
       empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
       cambia de dueno).
       .bss: base-1024 delta+180, .ctors: base-16 delta+0, .data: base-864 delta-100,
       .dtors: base-16 delta+0, .rodata: base+0 delta-88, .sbss: base-1024 delta+0,
       .sbss2: base-1024 delta+0, .sdata: base-1024 delta+0, .sdata2: base-1024 delta+0,
       .text: base+0 delta+12076

       CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
          .bss       nuestro .o    +180, seccion enlazada      +0  ->     -180 de otro
          .data      nuestro .o    -100, seccion enlazada    -160  ->      -60 de otro
          .rodata    nuestro .o     -88, seccion enlazada    -880  ->     -792 de otro
          .text      nuestro .o  +12076, seccion enlazada      +0  ->   -12076 de otro

       ESCALONES SIN EXPLICAR (11): un desplazamiento que comparten muchos
       simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
       NO se cuenta como permutacion. Que exista uno es la firma de que un
       simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
          .data           -968   1215 simbolos
          .text           +584    223 simbolos
          .text          +1244    208 simbolos
          .text          +1496    125 simbolos
          .text          +1384    103 simbolos
          .text            +12     95 simbolos
          .text          +1704     59 simbolos
          .text           +440     58 simbolos
          .text           +100     39 simbolos
          .data           -896     26 simbolos
          .text           +504     24 simbolos

    **535 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

    seccion       movidos      bytes
    .text             318     42,476
    .rodata           127      7,548
    .data              87      3,612
    ?                   3          0

**Permutaciones reales: 535.** (La r54 anotaba «9.959 permutados» para `zFe2`; esa cifra
venía de la sonda `r54ctl_probe.py`, no de `movidos` corregido.)

### El cambio de dueño de `zFe2` es CERO, y lo he comprobado símbolo a símbolo

Los `+12076` de `.text` **no son bulto robado**. Censo directo sobre las dos tablas de
símbolos de los `.o` y sobre los dos enlaces:

* nuestro `zFe2.o` define **399 símbolos que el `zFe2.o` objetivo no define**
  (`.text 12.076 B`, `.data 468`, `.bss 220`, `.rodata 240`);
* de esos 399, **321 no existen en el enlace base**: nadie los referencia y el enlazador los
  estripa (`.text 6.260 B`);
* **78 sí existen en el enlace base**, 77 fuera de la ventana de `zFe2` — o sea que los
  suministra otro objeto (`reserve__...vector<WCollisionTri>`, la familia
  `find__H2ZPP*I*`, los `Get__CQ26Attrib...TAttrib`, `_GetKind__*`, `_vt.8FEWidget`...);
* **y en los 77 el desplazamiento es `+0`**: siguen en la misma dirección en los dos enlaces.
  **El otro objeto los gana; `zFe2` no le quita nada a nadie.**

      6.260 (estripados) + 5.784 (los gana el otro objeto) + 32 (dentro de ventana) = 12.076

que es exactamente el `delta` de `.text` de nuestro `.o`. **Bytes que cambian de dueño de
verdad: 0.** Lo mismo en `.bss` (220 estripados explican los +180). En `.rodata` y `.data` el
déficit es real (`−880`, `−160`) pero es déficit de las dos partes, no traspaso.

## 3. El residuo de `.text`: una ESCALERA, no un racimo

Dentro de la ventana de `zFe2` (`0x80143F78..0x8017FE00`) hay 1.283 símbolos comunes de
`.text`, **1.252 desplazados, 235.456 B de código en otra dirección**, con **144
desplazamientos distintos y 156 rachas contiguas**. Recorriendo la sección en el orden del
objetivo, el desplazamiento sube en escalones y vuelve a bajar al final:

    +0 -> +12 -> [bloque HudResourceManager/FEngHud, revuelto] -> +12 -> +100 -> +440
       -> +504 -> +584 -> +620 -> +1244 -> +1384 -> +1496 -> +1704 -> ... -> +0

Cada escalón positivo es **un grupo de símbolos que nosotros metemos ahí y el objetivo emite
al final de la unidad**. Las rachas mayores:

| desp | rango objetivo | abarca | simbs | primer símbolo |
|---:|---|---:|---:|---|
| +1244 | `80164B10..8016D49C` | 35.212 B | 208 | `ForAllCareerRecordsSum__13FEPlayerCarDB...` |
| +584 | `8015ED60..801647E4` | 23.172 B | 136 | `__7cSlider` |
| +12 | `801467C0..8014C424` | 23.652 B | 82 | `__15RaceOverMessage...` |
| +440 | `80153174..801580C8` | 20.308 B | 58 | `__10StatsPanel` |
| +1496 | `80173D6C..80178864` | 19.192 B | 125 | `__12ArrayScripts` |
| +584 | `8015B220..8015E8F8` | 14.040 B | 72 | `__Q219nsEngageEventDialog...` |
| +1384 | `8016D49C..801706AC` | 12.816 B | 55 | `BroadcastMessage__16FEPackageManagerUl` |
| +1704 | `80179724..8017C7C8` | 12.452 B | 59 | `DefaultUnlockData__Fv` |

El objetivo tiene, de `0x8017C9A8` a `0x8017FE00`, un **bloque de cola de 13,4 kB**: todos
los `_._Clase`, todos los `_IHandle__IClase`, los `ClassKey__`, las clases locales y, al
final del todo, `_GLOBAL_.I.`. Es la purga de `finish_file`. Nosotros emitimos la mayoría ahí
—`permorden` da 1.146 de 1.290 en su sitio— pero **doce grupos salen fuera**: unos ~110-220 kB
antes (dentro de su `.cpp`) y cinco ~9,2 kB después.

## 4. El símbolo dominante: el bloque de clases locales de `FEPlayerCarDB` — 624 B

**Existe símbolo dominante, y cuadra al byte.**

Son 16 símbolos: los ocho destructores y los ocho `Callback` de las clases **locales**
declaradas dentro de los métodos de `FEPlayerCarDB` (`NumInfraction`, `TotalNumInfractions`,
`Bounty`, `EvadedPursuits`, `BustedPursuits`, `IsImpounded`, `Fines`, `NumCars`).

* tamaño del bloque: **624 B** (52+48, 52+52, 52+8, 52+8, 52+8, 52+24, 52+52, 52+8);
* el objetivo los emite en `0x8017FAA4..0x8017FD14`, dentro de la cola de `finish_file`,
  justo detrás de `ClassKey__Q36Attrib3Gen11infractions` y justo delante de
  `_._12TuningSlider`;
* nosotros los emitimos en `0x80164D7C..0x80164FEC`: **−109.656 a −110.480 B**;
* y el escalón de la escalera en ese punto es **exactamente +624**:

      0x801647E4  +584 -> +620   escalon  +36   antes de GetNumInfraction__13FEPlayerCarDB...
      0x80164B10  +620 -> +1244  escalon +624   antes de ForAllCareerRecordsSum__13FEPlayerCarDB...

**Bytes que explica: 111.344.** Es la distancia de `0x80164B10` al final del `.text` de la
unidad (`0x8017FE00`): todo eso está desplazado +624 B por este bloque y sólo por él. Si se
arreglara **sólo** este escalón, **35.212 B / 208 símbolos** caerían a delta 0 (la racha
`+1244`), que es el 15 % de los 235.456 B desplazados. **Es el mayor de los 156 escalones,
pero no cierra la unidad: el residuo es una escalera de una docena de causas del mismo tipo.**

El segundo es `_IHandle__15IGenericMessage`: **12 B**, se va **−222.516 B**
(`0x8017CCF4` → `0x801467C0`) y su escalón `+12` arrastra los 235.072 B de `.text` que van
detrás; arreglarlo devuelve 28.588 B (95 símbolos) a delta 0.

**Y la causa de los dos es la MISMA y es mecánica**: los dos están escritos como `asm()` de
ámbito de fichero, y GCC 2.9 emite un `asm()` de fichero **exactamente donde está escrito**,
así que **no se puede diferir a la cola**.

    src/Speed/Indep/Src/Frontend/HUD/FeGenericMessage.cpp:87
        asm(".globl _IHandle__15IGenericMessage\n"  ...)        <- 12 B, sale en 0x801467C0
    src/Speed/Indep/Src/Frontend/Database/VehicleDB.cpp:416-660
        cuatro bloques asm() con los 16 cuerpos de FEPlayerCarDB  <- 624 B, sale en 0x80164D7C

Los otros diez grupos descolocados **no** son `asm` — son destructores que cc1plus emite en
el punto de la clase en vez de en la cola (`_._16FEPackageManager` −66.436,
`_._14RaceResultStat` −177.672, `_._11MovieScreen` −152.464,
`Create__28PostPursuitInfractionsScreen` −188.800,
`MakeSpaceInPoolCallbackBridge__27SillyTextureStreamerManager` −156.072,
`_._18CustomTuningScreen` −28.444, `Default__Q27Physics7Tunings` −12.812, los tres
`ClassKey__Q36Attrib3Gen*` −110.676, el grupo `IconPanel`/`IconScroller` −37,5 kB) y cinco
que salen **9,2 kB tarde** (`_._9IconPanel`, `_._12IconScroller`, `_._16IconScrollerMenu`,
`_._16FEScrollyBookEnd`, `_._12UIWidgetMenu`).

## 5. Las otras tres medidas

    linkdelta   .text +0    rodata-880 data-160 over-16
    permorden   objetivo 1307 funciones, nuestro 1591, comunes 1290
                solo nuestras (las estripa el enlazador): 301
                sin tamano en nuestro objeto (alias de asm(), SI definidas): 17
                en su sitio: 1146 de 1290   DESPLAZADAS: 144
                ciclos no triviales: 13  (856, 184, 37, 14, 14, 10, 7, 5, 4, 3, 3, 2)
    trypromo    DOL ROTO (5c1960d57070)

Los **17 «sin tamaño»** de `permorden` son exactamente los **17 «de otro TAMANO»** de
`reorden` y exactamente los **17 símbolos definidos por `asm()`**:
`_IHandle__15IGenericMessage` + los 16 de `FEPlayerCarDB`. No es un problema, es la firma del
andamio.

Los dos ciclos gigantes de `permorden` (856 y 184) **no son 856 funciones mal**: son la
rotación que produce la escalera. La cifra que sirve es `DESPLAZADAS: 144`.

## 6. El siguiente paso concreto

**Mover los cinco `asm()` de fichero al final de `zFe2.cpp`**, en el orden del objetivo:
primero el de `_IHandle__15IGenericMessage` (`FeGenericMessage.cpp:87`) y después los cuatro
bloques de `FEPlayerCarDB` (`VehicleDB.cpp:416-660`), pegados detrás del último `#include`
(`Careers/UnlockSystem.cpp`, línea 121) y **delante** del `asm` de `.rodata` que ya hay ahí.

*Por qué ahí y qué se espera*: un `asm()` de fichero se emite en el punto del parseo, así que
al final del `.cpp` cae **justo antes** de la cola de `finish_file`, en `~0x8017C9A8`. Eso no
es su sitio exacto (`0x8017CCF4` y `0x8017FAA4`), pero pasa de estar **222.516 y 110.000 B
antes** a estar **~13 kB antes**, y sobre todo **borra los dos escalones mayores de la
escalera**: los 63.800 B de `.text` (290 símbolos) de las rachas `+12` y `+1244` caen a
delta 0 y el resto de la escalera baja 636 B. Coste `.text +0` (no se añade ni se quita nada,
sólo se mueve) y riesgo de codegen nulo: es texto de ensamblador, no fuente.

Dos avisos antes de medirlo:

1. `VehicleDB.cpp` sigue necesitando las **declaraciones** que hoy acompañan a esos `asm`
   (los alias mangled dobles); mover el `asm` sin mover lo que lo referencia deja símbolos
   sin definir. Hay que comprobar `mangfix --check` después.
2. `zFe2.cpp` ya termina con un `asm(".section .rodata" ...)` de 249 cadenas puesto **al final
   a propósito para no desplazar ningún `$LC`**. Los bloques nuevos son de `.text`, así que no
   deberían tocarlo, pero hay que pasar `lcfix --check` igual.

**Y las dos palabras de contenido, que son 8 B y no dependen de nada de lo anterior:**
`FeMinimap.cpp:41` `WMIT_NONE` → `WMIT_TOKEN`, y `feDialogBox.cpp:19` `= 4` → `= 1`.

## 7. Sorpresas (contradicen el brief)

1. **`zFe2` no tiene cambio de dueño.** El brief la daba como una de las dos peores (49
   cambios, `_IHandle__15IGenericMessage` −222.516). Medido símbolo a símbolo: **0 bytes
   cambian de dueño**. Los 77 símbolos que definimos de más y que otro objeto también define
   **se quedan en el otro objeto** (desplazamiento +0), y los otros 321 los estripa el
   enlazador. El censo de la r54 contaba «salto > 20 kB» como cambio de dueño, y en `zFe2`
   esos saltos son **reordenaciones DENTRO de la unidad**, no traspasos. Si el mismo criterio
   se usó en `zFe` (47), esa cifra también hay que rehacerla.
2. **La sección «CAMBIO DE DUENO (M4)» de `movidos.py` sobreimputa.** Su segunda resta
   (`enlazado − delta de nuestro .o`) supone que todo lo que crece nuestro `.o` llega al
   enlace. No llega: lo estripado y lo que pierde el duelo de duplicados no cuenta. En `zFe2`
   imputa **−12.076 B de `.text` robados a otros** donde hay **0**. La corrección es restar
   antes los símbolos que no aparecen en el enlace y los que aparecen con desplazamiento 0
   fuera de la ventana; es el mismo censo que hice y tarda segundos, sin enlazar dos veces.
3. **`reorden.py` no enmascara `ADDR32`.** Sus 20 palabras de «contenido» en `.rodata` son
   **cero** contenido: 20 punteros de vtable al mismo símbolo movido. La máscara actual sólo
   entiende instrucciones, y un puntero `0x8017DCFC` cae en la tabla `DFORM` por casualidad
   (opcode 32), así que se compara por la mitad alta: sale «distinto» sólo si el símbolo cruzó
   una frontera de 64 kB. Con la comparación por SÍMBOLO DESTINO (y normalizando el sufijo
   `.NNNNN` del mangle, que `mangfix` cambia entre las dos compilaciones) `zFe2` queda en
   **2 palabras de contenido real, 8 bytes**.
4. **La cifra de permutaciones cambió de 9.959 a 535.** No es una mejora del árbol: son
   herramientas distintas. La comparable entre unidades es la de `movidos` corregido.
5. `build_direct.py zFe2` **no falló** — el aviso de reintento del brief no aplicó aquí.

## 8. Sondas

En el scratchpad de la sesión, no en `scripts/` (todas de sólo lectura sobre los dos ELF
enlazados y los dos `.o`): `probe.py` (enlaza y vuelca las tablas), `trans.py`
(transiciones), `racimo.py` (histograma y rachas), `pasos.py` (qué metemos en cada escalón),
`escalera.py`, `m4.py` (censo de cambio de dueño real), `rod.py`/`dat.py`/`vt.py`/`mm.py`
(contenido contra reubicación). Si algo merece quedarse, mi voto es **`m4.py`**: contesta
«¿esta unidad roba algo?» sin enlazar y hoy `movidos` contesta que sí donde es que no.
