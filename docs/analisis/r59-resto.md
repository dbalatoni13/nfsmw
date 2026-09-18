# r59 — `resto` (zAI, zSpeech, zLua, zWorld): el CADAVER de un `$LC` muerto mueve el pool, y se caza con dos palancas de una linea

Agente `resto`. Cuatro ficheros tocados, **dos SourceLists mias y dos cabeceras con guarda
inerte**, las dos con la inercia DEMOSTRADA (no supuesta) contra una unidad ajena:

    src/Speed/Indep/SourceLists/zAI.cpp
    src/Speed/Indep/SourceLists/zSpeech.cpp
    src/Speed/Indep/Src/World/WorldConn.h        <- guarda inerte WORLDCONN_NO_EMITTERSYSTEM
    src/Speed/Indep/Src/Gameplay/GState.h        <- guarda inerte DEAD_STR_DONE

Ni `config/`, ni `splits.txt`, ni `keep.lst`, ni `configure.py`. **No he tocado zLua ni
zWorld** (§5: zWorld la esta editando otro agente EN PARALELO).

---

## 0. Titular

| unidad | linkdelta (keep ya corregido) | B distintos en SUS rangos | prefijo EXACTO de `.rodata` |
|---|---|---:|---|
| **zSpeech** | **IGUAL -> IGUAL** | **12.406 -> 11.765** | **440 -> 1.040 B** (0x80405FC8 -> 0x80406220) |
| **zAI** | rodata-240 -> rodata-**256** | **41.345 -> 40.284** | **96 -> 297 B** (0x803C8CC0 -> 0x803C8D89) |
| zLua | rodata-728 (sin tocar) | — | primera diferencia en +0xF4 |
| zWorld | rodata-472 data-128 (**de otro agente**) | 170.515 | — |

`fncmp` **IDENTICO antes y despues en las cuatro**: zAI 0/1030, zSpeech 1/703 (`Setup`, la
vetada de siempre), zLua 0/537, zWorld 4/582. **Cero regresiones de codigo.**

Sello, con **tres** compilaciones seguidas del arbol final:

    zAI      3dd4df6b3974997befe9dcd77b6edaf3bbaa3f2c   (x3)
    zSpeech  c10de8558d420324ddcf3ed82d18dd20fa1983b1   (x3)
    zLua     2d1ffd7537876b7a9029e65a47ed750a7bb08964   (fuente sin tocar; el .o cambia
                                                         solo por cabeceras ajenas)
    zWorld   09f3b05375f11c46c56c3f5988814b012515eb3c   (lo compila su agente)

Ninguna promociona: `trypromo` da **DOL ROTO** (`zSpeech 6738cb86c68a`, `zAI 1b60fa8d1f8d`).

---

## 1. EL MECANISMO: el CADAVER de un `$LC` muerto es un DESPLAZAMIENTO, no un sobrante

La r58 nombro el cadaver (`size & ~7` deja 1..7 B de cada literal muerto). Lo que **no** se
habia contado es que **ese resto ocupa sitio en medio del pool y corre TODO lo que va detras**,
y que por eso una unidad puede medir `IGUAL` en las nueve secciones y tener 8.000 B distintos.

Caso medido en zSpeech, con los bytes delante:

    0x80405FC0  obj  72 61 70 65 00 00 00 00 | 57 6F 72 6C 64 42 6F 64   "rape\0\0\0\0WorldBod"
                nue  72 61 70 65 00 00 00 00 | 00 00 00 00 70 00 00 00   "rape\0\0\0\0"  + 8 B

Esos ocho bytes son, exactamente:

* `$LC165 "Attrib::Gen::emitterdata"` — 25 B, `25 & ~7 = 24` estripados, **queda 1** (`\0`),
  alineado a 4;
* `$LC166 "Attrib::Gen::emittergroup"` — 26 B, quedan **2** (`'p'`, `\0` = `70 00`), a 4.

El objetivo **no tiene esas dos cadenas en zSpeech**. Resultado: el pool entero de la unidad
iba 8 B corrido desde `0x80405FC8`, y con el todos los `Pkt_*`, los `AUD:*`, las vtables...

### 1.1 El censo, unidad por unidad (`scratchpad/parasitos.py`)

`$LC` que NUESTRO objeto emite y **el objetivo NO tiene en el rango de esa unidad**, con el
cadaver que deja cada uno:

| unidad | cadenas | B | **cadaver** |
|---|---:|---:|---:|
| zSpeech (al empezar) | 14 | 499 | **68** |
| zLua | 18 | 421 | **96** |
| zAI | 11 | 274 | **56** |
| zWorld | 9 | 244 | **52** |

Y la lista se repite entre unidades: `EventSequencerSystems`, `Attrib::Gen::chassis /
induction / nos / tires / transmission / emitterdata / emittergroup / presetride`, `done`,
`EAGL4::SymbolEntry`, `SpeechSampleMap node`, `ScrollerDatumNode`, `ScrollerSlotNode`.
**Es un frente compartido, no una rareza de una unidad.**

### 1.2 La regla que faltaba: **un literal muerto de 7 B o menos sobrevive ENTERO**

`size & ~7` es **0** para cualquier tamano de 1 a 7. `"done"` mide 5 B, asi que el estripado
**no se lleva nada** y la cadena entera se queda en medio del pool. Los cadaveres de 0 B
(`presetride`, 24 B; `acceltrans`, 24 B) son inofensivos: se van enteros y no desplazan.
O sea que **el dano de un literal muerto NO es proporcional a su tamano**: lo da `size & 7`.

---

## 2. `zSpeech`: tres ediciones, 1.040 B de prefijo exacto y las nueve secciones intactas

### 2.1 `WORLDCONN_NO_EMITTERSYSTEM` — los 8 B del §1

`WorldConn.h` incluye `Ecstasy/EmitterSystem.h`, que arrastra `emitterdata.h` y
`emittergroup.h`, y sus `USE_ATTRIB_ALLOC` internan las dos cadenas **justo antes** de las de
`WorldConn` — que si son del objetivo. Guarda inerte en `WorldConn.h` y `#define` en
`zSpeech.cpp`, en la linea exacta donde el objetivo quiere ese bloque.

    prefijo exacto de .rodata   0x80405FC8 -> 0x80406124   (+348 B)
    B distintos                 12.406 -> 12.067
    secciones                   IGUAL, y siguen IGUAL

**AVISO**: sin `lcfix` esta edicion parece una REGRESION brutal (`rodata-48`, y
`WorldBodyConn`/`Pkt_Body_*` convertidos en cadaveres `yConn`/`_Open`/`_Send`). No lo era: al
quitar dos literales se renumeran los `$LC` y `keep.lst` pasa a proteger otra cosa. Todas las
medidas de este informe van contra una **copia privada de `keep.lst` ya corregida**.

### 2.2 `DEAD_STR_DONE` — el literal de 5 B que no se estripa

`"done"` sale de `Gameplay/GState.h` (`IsTerminalState`), viaja por
`MAudioReflection.h -> LuaPostOffice.h -> GActivity.h`, y cae **entre `Attrib::Gen::gameplay` y
`Attrib::Gen::milestonetypes`**, que es donde el objetivo no tiene nada. Guarda inerte por
literal, del mismo corte que el `DEAD_STR_MUNSPAWNCOP` de `MUnspawnCop.h` que ya estaba en el
arbol, con el valor por defecto `DEAD_STR(s)` para que zMain y zSim sigan viendo lo mismo.

**Apagar `DEAD_STR` ENTERO no vale y esta medido**: mata tambien `SMS_MESSAGE_%d*` y las
versiones de `GRaceDatabase`, que SI son del objetivo, y **cambia el codigo**
(`.text 3.804.440 -> 3.804.424 B`).

### 2.3 El primer de pool de la cabeza: **DOCE cadenas, y la treceava es un ESCALON**

Con `"done"` fuera, el primer si mueve: nombradas detras de `milestonetypes.h` y delante de
`SpeechManager.cpp`, las doce caen en el orden del objetivo (`0x80406140..0x80406220`).

| primer | `.rodata` del enlace | `.text` distintos | 1a dif. `.rodata` | total |
|---:|---|---:|---|---:|
| sin primer | -8 | 3.987 | 0x80406140 | 12.340 |
| 1 | -8 | 3.967 | 0x80406150 | 11.957 |
| 4 | -8 | 3.964 | 0x8040619C | 11.887 |
| 9 | -8 | 3.958 | 0x80406208 | 11.749 |
| **12** | **IGUAL** | **3.940** | **0x80406220** | **11.765** |
| 13 | **+8** | **5.947** | 0x80406228 | 20.588 |
| 14 / 26 | +8 / +16 | 5.947 / 5.947 | — | 20.580 / 20.588 |

Dos cosas que valen para todo el arbol:

1. **`"16.1.0"` (7 B) es lo que devuelve la unidad a `IGUAL`**: entra entera porque `7 & ~7 = 0`
   y compensa los 8 B que se llevo `"done"`. El elegido no es el mayor de la lista: es el que
   cuadra la aritmetica del estripado.
2. **La 13a (`"1.8.1"`) rompe de golpe y por otro sitio**: sin cambiar el tamano de ninguna
   seccion y sin mover `fncmp`, **el `.text` se REORDENA** (3.940 -> 5.947 B distintos, con la
   primera diferencia saltando de `0x8028A23A` a `0x8028A18F`). Es un escalon, no una
   pendiente. **Un primer hay que barrerlo cadena a cadena, no escribirlo entero de una vez.**
   La funcion muerta en si es GRATIS: con el primer VACIO la medida vuelve al byte.

---

## 3. `zAI`: `BWARE_PREFIX_GAMECUBE`, una linea, 201 B de prefijo

Nuestra `.rodata` tenia **dos** `"GAMECUBE"`: la del prefijo escrito a mano en `+0` y la que
`bGetPlatformName()` internaba, **delante de `"Attrib::Attribute"`**. Con la linea puesta el
bloque entero de las ocho etiquetas de AttribSys cae en su direccion exacta:

    prefijo exacto .rodata   0x803C8CC0 -> 0x803C8D89   (+201 B; casa hasta Attrib::TAttrib)
    .text distintos          21.297 -> 20.943
    .bss  distintos           3.130 ->  2.772
    .data distintos             651 ->    623
    total                    41.345 -> 40.284
    .rodata del enlace       -240 -> -256   (PAGA NEGATIVO EN BYTES, igual que en zLua r58)

Los 16 B de mas de deficit son el precio conocido (`keep.lst` salvaba la copia). **Es
estructuralmente obligatorio**: sin ella todo el pool de zAI esta 12 B corrido y no puede casar.

### 3.1 El primer de la cabeza de zAI: MEDIDO Y NEGATIVO (anotado en el fuente)

El objetivo pide, detras de `Attrib::TAttrib`: `speech, aivehicle, pursuitlevels,
pursuitescalation, pursuitsupport, trafficpattern, pvehicle, VehicleParams`. Un primer las
coloca —el prefijo exacto crece a `0x803C8D9D`, `0x803C8DB5`, `0x803C8DD1`— **pero no paga**:
`.rodata` cae a -264 en cuanto entra la primera, el `.text` se reordena (20.943 -> 21.377) y el
total sube a 41.108-41.136. Revertido, con las cifras en un comentario junto al `#include` de
`AttribSys.h`.

**Y con una trampa de ancla que merece la pena contar**: `Attrib::Gen::simsurface` y
`Attrib::TAttrib` se internan **DESPUES** de `#include AttribSys.h`, asi que un primer escrito
ahi que no los nombre PRIMERO los adelanta y **rompe la cabeza que ya casaba**
(1a dif. `0x803C8D89 -> 0x803C8D62`). Con solo esos dos, la medida es identica a no poner nada.

---

## 4. Como se mide esto (y por que `linkdelta` y `dolwhere` no valen tal cual)

Con `keep.lst` rancio, hoy mismo:

    linkdelta dice   zAI rodata-440     y son  -256
    linkdelta dice   zSpeech rodata-280 y es   IGUAL
    dolwhere  no arranca en ninguna de las dos

El metodo que he usado, y que propongo dejar en `scripts/` si la ventana quiere:

1. `lcpriv.py` — corre `lcfix` contra una **COPIA** de `keep.lst` en el scratchpad (no toca el arbol).
2. `link2.py <unidad> <salida> <keep>` — enlaza sustituyendo UNA unidad, con ese keep.
3. `rangos.py <unidad> <elf>` — compara, **rango a rango de `splits.txt`**, contra el enlace
   base, y da **cuantos bytes difieren y DONDE ESTA EL PRIMERO**.

**`primero` es la metrica de escalera; `distintos` es la de aire.** Las dos hacen falta: en
zSpeech la 13a cadena alarga `primero` y a la vez dispara `distintos`, y la buena es no ponerla.
Y el enlace base con la copia de `keep.lst` se comprueba contra el ELF original antes de creer
nada (`base_k == orig` en el rango de la unidad: SI).

---

## 5. `zLua` y `zWorld`: por que no las he tocado

**`zWorld.cpp` la esta editando OTRO AGENTE mientras yo mido** (`mtime` 10:01, ya con un bloque
`asm()` de 472 B y el comentario «r59 (prefijo)»; su `.o` cambio de `194f70a8` a `09f3b053` sin
que yo lo compilase). Tambien `zWorld2.cpp` y `World/Sun.cpp`. Mi encargo la daba como
PROPIEDAD EXCLUSIVA y no lo es: **el reparto de la ronda tiene un solape**.

`zLua` la dejo entera por la misma razon (mi encargo la manda a la «receta del bloque de
cabecera», que es lo que el agente `prefijo` esta aplicando en zWorld) y porque su problema no
es de 728 B: **8.039 de sus 8.776 B de `.rodata` difieren**, con el pool en otro orden desde
`+0xF4`. Lo que si dejo medido para quien la coja:

* le faltan **55 cadenas (633 B)** que el objetivo tiene (`prefijotu.py zLua`);
* emite **18 que el objetivo no tiene**, con **96 B de cadaver** — el mayor de las cuatro;
* incluye `WorldConn.h`, asi que **la guarda del §2.1 le vale tal cual** (sin medir);
* su primera diferencia es `MGeneric`, que falta en `0x803ED42C`.

`zWorld`, medido antes de que su agente empezara: la duplicidad de `"GAMECUBE"` del §3 la tiene
tambien —**y tres copias, no dos**—.

### 5.1 Las vetadas del encargo: `previo.py` confirma que siguen vetadas

* `SetMemoryPoolSize` — 7 rondas, «UNSOLVED, scheduling», 304/304 al 97,9 %.
* `DefragmentPool` — **19 rondas**, 684/684 al 99,269 %, 23 filas todas `ARG_MISMATCH`.
* `Setup__Q26Speech13RoadblockFlow` — r36d/r36f/r45 barren **15 formas** (incluido
  `__asm__("# d")`, helpers inline y PMF: 596 -> 676 B) con **11 binarios identicos**. «Sin
  palanca nueva». **El encargo la pedia; esta agotada.**

---

## 6. Las dos guardas son inertes, y esta DEMOSTRADO (no argumentado)

Regla 5. Prueba sobre `zLua`, unidad ajena que **parsea las dos cabeceras** (comprobado con
`-H`, igual que zMain, zSim, zFe, zFe2 y zWorld):

    .text .rodata .data .bss .ctors .rela.text .rela.data .rela.rodata .rela.ctors
      TODAS IGUALES, byte a byte, y los 1.299 simbolos de seccion real IDENTICOS

Y el punto que hacia falta cerrar antes: **el sufijo `.NNNNN` de GCC 2.9 es `DECL_UID`, un
contador de DECLARACIONES, no el numero de linea.** Caso minimo (`k.3` antes y despues de
meter ocho lineas de comentario y un `#ifndef`). Importa porque `zFe2.cpp` lleva alias
`....32137` a fuego y `keep.lst` cuatro entradas con sufijo (`LastRandom.25127`,
`changetime.14948`, `index.14943`, `windState.14947`): **una guarda inerte NO los caduca**.
Comprobado ademas que `zFe2` sigue enlazando.

> Corolario util: **una edicion de SOLO comentarios en un SourceList no cambia ni el `sha1` del
> `.o`** (medido: `c10de855` antes y despues de reescribir 10 lineas de comentario).

---

## 7. `lcfix`: **242 pendientes, 112 mias, NINGUNA venenosa**

    zSpeech  62      <- mias
    zAI      50      <- mias
    zWorld   39   zPhysicsBehaviors 30   zCamera 21   zWorld2 19   zPlatform 13
    zAnim     5   zGameplay 3            <- de otros agentes

**Sin pasarlo, mis dos unidades no se pueden medir ni promocionar.**

**Ninguna correccion romperia mis unidades**, y esta comprobado de dos maneras: (a) TODAS mis
medidas son ya el estado POST-`lcfix` (por eso zSpeech sigue `IGUAL`); (b) de las **51**
directivas `@lc zAI` y las **90** de `@lc zSpeech`, **cero** nombran una cadena que el objetivo
no tenga en el rango de la unidad — que es el patron de la que envenenaba a `zAnim` en la r58.

**FALLO nuevo, y es COSMETICO**: `zAI: 'GAMECUBE' no tiene simbolo $LC propio`. Es el mismo
caso que la r58 documento para zLua y zSpeech: al dejar de internar la segunda copia, la
entrada se queda sin `$LC` propio y **apunta ya al mismo simbolo vivo que su vecina**
(`keep.lst:1957-1958` -> `$LC145`, que tras `lcfix` es tambien el de `"Attrib::Attribute"`).
Borrarla es limpieza, no arreglo.

---

## 8. Lo que pido, con la cifra

| propuesta | vale | quien |
|---|---|---|
| **`python scripts/lcfix.py`** — 242 correcciones, 112 mias. **Sin esto `linkdelta` miente 184 B sobre zAI y 280 sobre zSpeech** | obligatorio | ventana |
| **`#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)` en las NUEVE unidades que aun duplican la cadena** — censo hecho contando `"GAMECUBE\0"` en el `.o`: **zCamera, zEAXSound, zFe, zFe2, zPhysicsBehaviors, zPlatform, zTrack, zWorld (x3), zWorld2**. `zFoundation` ya esta limpia | 12 B de pool mal colocado por unidad, **en el byte 0**: alinea el bloque entero de AttribSys | los duenos |
| **`WORLDCONN_NO_EMITTERSYSTEM` en zLua** (y en toda unidad con `emitterdata`/`emittergroup` parasitos): la guarda ya esta puesta y es inerte | 8 B de cadaver + el orden de detras | dueno de zLua |
| borrar `keep.lst:1957-1958` (`# @lc zAI "GAMECUBE"`), y las de zLua/zSpeech que la r58 ya pedia | 0 B, limpieza | ventana |
| **arreglar el solape de propiedad**: mi encargo daba zWorld/zLua como EXCLUSIVAS y `prefijo` las esta editando | media ronda de medidas invalidadas | jefe |
| `scratchpad/{lcpriv,link2,rangos,parasitos}.py` al arbol (§4) | son las cuatro que hacen medible una unidad con `keep.lst` sucio | ventana |

---

## 9. Sorpresas

1. **El cadaver no es un sobrante: es un DESPLAZAMIENTO.** Ocho bytes de resto en `0x1B8`
   corren 12 kB de pool. Por eso una unidad puede estar `IGUAL` en las nueve secciones y tener
   8.000 B distintos, y por eso `linkdelta` no basta como semaforo.
2. **El dano de un literal muerto lo da `size & 7`, no `size`.** `"done"` (5 B) desplaza 8;
   `"Attrib::Gen::presetride"` (24 B) desplaza CERO. Y **todo literal muerto de <= 7 B entra
   entero**: es la moneda suelta con la que se cuadra una seccion.
3. **Un primer de pool es un ESCALON.** Doce cadenas: `.rodata` IGUAL y `.text` mejor que al
   empezar. Trece: `.rodata` +8 y **2.007 B de `.text` reordenados** sin que cambie el tamano
   de nada ni `fncmp`. Hay que barrerlo de una en una.
4. **El primer tambien ROMPE hacia atras si el ancla esta mal.** En zAI, `simsurface` y
   `TAttrib` se internan DESPUES de `#include AttribSys.h`; un primer ahi que no los nombre
   primero los adelanta y estropea la cabeza que ya casaba.
5. **El sufijo `.NNNNN` es `DECL_UID`, no la linea.** Lo probe con un caso minimo porque
   `zFe2.cpp` avisa de que «cualquier edicion de los includes» le rompe el enlace. Comentarios
   y `#ifndef` no lo mueven; lo que lo movio 12 posiciones en zLua fue el `IModel.h` de otro
   agente.
6. **Con agentes en paralelo, el sello de tres compilaciones se tambalea sin que
   `build_direct` mienta**: mi pasada 2 dio otro `sha1` en las DOS unidades porque otro agente
   escribio `CarCustomize.hpp` (10:30:10) y `FEngInterfaceFEStrings.hpp` (10:31:20) en mitad.
   Las pasadas 4, 5 y 6 volvieron a coincidir. **El sello hay que repetirlo hasta ver tres
   IGUALES seguidas, no tres a secas.**
7. **La primera medida de la guarda de `WorldConn.h` decia REGRESION** (`rodata-48`, tres
   cadenas del objetivo convertidas en cadaver) y era el `keep.lst` rancio. La cifra buena
   estaba a un `lcfix` privado de distancia.

---

## 10. Siguiente paso, por unidad

1. **`zSpeech`** — la 1a diferencia esta en `0x80406220` y la produce `EventSequencerSystems`
   (`HAND_POOL_TAG`, `Main/EventSequencer.h`), seguida de `chassis/induction/nos/tires/
   transmission` (entran por `SpeechManager.hpp -> ... -> PhysicsInfo.hpp`, cabeceras AJENAS) y
   de `emitterdata`/`emittergroup`, que **tienen un segundo camino** ademas de `WorldConn.h`.
   Cada una es un cadaver de 4-8 B y hay que compensarla con cadenas del objetivo de <= 7 B
   para no perder el `IGUAL`. Lo de `SampleReqList` detras de `SPCHEventList` (r57/r58) sigue
   esperando ventana.
2. **`zAI`** — `rodata -256`. Le sobran 56 B de cadaver (11 cadenas) y le faltan las que el
   objetivo si tiene. El primer de la cabeza esta **medido y negativo** (§3.1): lo que queda es
   dejar de arrastrar `EventSequencer.h`, `eagl4supportdef.h`, `SpeechCache.h` y
   `feScrollerina.hpp`, que es la via de `HAND_POOL_TAG`/`DEAD_STR` y no del primer.
   `DEAD_STR_DONE` tambien le vale (aun lleva `"done"`, cadaver 5 -> 8 B): **sin medir**.
3. **`zLua`** — §5. Paquete entero, no a medias.
4. **`zWorld`** — que su agente cierre; el `BWARE_PREFIX_GAMECUBE` es lo primero y son tres
   copias.
