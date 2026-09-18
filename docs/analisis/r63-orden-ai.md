# r63 -- orden-ai: zAI y zFe2

Agente `orden-ai`. Unidades en exclusiva: `zAI`, `zFe2`.
Fichero tocado ademas del informe: `src/Speed/Indep/SourceLists/zAI.cpp` (codigo
y comentarios) y `src/Speed/Indep/SourceLists/zFe2.cpp` (SOLO comentarios: el
`.o` sale con el mismo sha1, comprobado).
**Nada commiteado. `lcfix.py` NO se ha corrido y no deja nada pendiente**
(ningun literal nuevo, ningun `$LC` renumerado: los cambios son de `.data` y de
comentarios; `keepchk.py`/`keepchk2.py` no se quejan de zAI).

---

## 1. El resultado: zAI de 244.740 B a 53.874 B de `dolwhere` (-190.866 B)

zAI era la unidad mas lejos del DOL de todo el proyecto. **No era orden: era
TAMANO.** `desplaza.py zAI` lo decia con todas las letras y llevaba rondas
diciendolo:

```
  seccion     DENTRO      mal |   DETRAS      mal
  .data           60       59 |     2756     2596   <== TAMANO MAL
  .data DETRAS: 2596 simbolos movidos, desplazamientos [(-16, 2485), (-20, 111)]
     el primero: AnimBankSlotPool  804154C0 -> 804154AC (-20)
  VEREDICTO: el TAMANO de la unidad esta MAL (2596 simbolos ajenos movidos)
```

La `.data` de zAI salia **20 B corta**, y eso bajaba 16-20 B a **2.596 simbolos
AJENOS** repartidos por todo el juego. De los 244.740 B de `dolwhere`, **168.173
estaban en `.data`** y casi todos eran tablas de punteros de OTRAS unidades
apuntando 16 B mas abajo.

Los 20 B son dos cosas distintas, y **hay que arreglar LAS DOS**:

### 1a. Los 8 B del `pad_06_80415180_data` (codigo, ya aplicado)

El hueco anonimo de cabecera de la unidad medía 164 B (`0xA4`) y el objetivo
tiene **172** (`0xAC`). Los dos ultimos words no son cero: leidos del DOL
original en `0x80415224` y `0x80415228` valen `0x43610000` (225.0f) y
`0x43960000` (300.0f). No llevan simbolo en `NFSMWRELEASE.ELF` --el hueco
`80415180..8041522C` es anonimo entero, lo comprobe volcando su tabla de
simbolos--, asi que solo se ven leyendo el DOL. Con 164 B `RandomSortTCDir`
caia en `0x80415224` en vez de `0x8041522C`.

Aplicado en `zAI.cpp`:

```
    "  .4byte 0x00000000, 0x43610000, 0x43960000\n"
    ".size pad_06_80415180_data, 0xAC\n"
```

### 1b. Los 12 B de tres huecos muertos -- LINEAS PARA `keep.lst` (PROPUESTA)

`gapchk.py zAI` los nombra desde hace rondas y nadie los habia metido:

```
zAI              3 muerto(s) sin keep.lst,    12 B en juego
    gap_06_80415260_data               .data     +00E8     4 B
    gap_06_804152A8_data               .data     +0130     4 B
    gap_06_804152C8_data               .data     +014C     4 B
```

**Lineas exactas a anadir en `config/GOWE69/keep.lst`, detras de
`zAI.o:gap_06_80415498_data` (linea 73 hoy):**

```
zAI.o:gap_06_80415260_data
zAI.o:gap_06_804152A8_data
zAI.o:gap_06_804152C8_data
```

(Fichero CRLF: escribirlas con CRLF como el resto. No crean ningun `pad_`
nuevo, los tres simbolos ya existen: `gap_06_80415260_data` esta en
`AICopManager.cpp:71` y los otros dos en `zAI.cpp`.)

### 1c. Y por que hay que aplicar las dos: son MULTIPLICATIVAS

Medido las cuatro combinaciones con `dolwhere` (mismo arbol, mismo sello):

| estado | `dolwhere zAI` | delta |
|---|---:|---:|
| base (r62) | 244.740 B | -- |
| **solo** las 3 lineas de `keep.lst` | 237.773 B | -6.967 |
| **solo** `pad ... 0xA4 -> 0xAC` | 237.813 B | -6.927 |
| **LAS DOS** | **53.874 B** | **-190.866** |

Cada mitad sola deja el deficit en pie (12 B o 8 B) y el desplazamiento de los
2.596 simbolos ajenos sigue entero. Es el aviso mas importante de este informe:
**si se aplica solo el codigo y no `keep.lst`, la unidad se queda en 237.800 B y
parece que el arreglo no vale nada.** Lo he escrito tambien en la cabecera de
`zAI.cpp` para que no se pierda.

### 1d. Un extra de `.data`: tres definiciones subidas a su sitio (-6 B)

`AIPerpVehicle::mStagger`, `gHeliVehicle` y los dos
`UTL::COM::Factory<>::Prototype::mHead` estaban escritos al final del fichero y
salian en `0x8041547C`, `0x80415480` y `0x80415474/78`; el objetivo los tiene en
`0x8041536C`, `0x80415410` y `0x8041542C/30`. Subidos a los renglones que les
tocan (justo delante de `gap_06_80415370_data`, de `gap_06_80415414_data` y de
`lbl_80415438` respectivamente). Vale solo **-6 B** (53.880 -> 53.874) porque lo
que queda de `.data` lo mandan `bIgnoreHeliSheet`, `height.*` y `lastSpawnLane.*`
que estan mal por otra razon; pero la colocacion ahora es la correcta y esta
documentada al lado de cada una. `gHeliVehicle` necesita `class
AIVehicleHelicopter;` adelantada: es un puntero, basta.

### 1e. Comprobaciones

* `fncmp Speed/Indep/SourceLists/zAI`: **0 de 1030 con el codigo distinto**,
  antes y despues. Cero regresiones.
* `textorder zAI`: **359 descolocadas / 101 saltos antes y despues**. El arreglo
  es de `.data`, no toca el `.text`.
* `linkdelta zAI`: `.text +0, resto IGUAL` antes y despues.
* Sello triple del `.o`: `4e37a464c1c90c392b14c01c17f18bfa6e5e28dd`, tres
  compilaciones seguidas, identicas.
* `trypromo`: sigue DOL ROTO (quedan 53.874 B). **No promociona.**

---

## 2. zFe2: la cabecera de la r59 esta REFUTADA

`zFe2.cpp` empieza desde la r59 con *"EL BLOQUEO DE ESTA UNIDAD ES DE DUENO, NO
DE ORDEN [...] Mientras esten [las cinco vtables de mas], la .rodata entera de
zFe2 va corrida y los 47.147 B de dolwhere no bajan por reordenar nada."*

**Es falso, y se mide en un enlace.** Enlazando con zFe2 sustituida y leyendo la
tabla de simbolos del ELF ENLAZADO en la ventana `0x803E4380..0x803EA7E8` salen
**125 simbolos `_vt.` -- exactamente los 125 del objetivo --** y `8FEWidget`,
`10IconOption`, `10ArrayDatum`, `12ArrayScripts` y `Q33UTL3COM8IUnknown` **no
aparecen**: el enlazador se queda con la copia de zFe, igual que hizo el
original. Cuestan **CERO bytes**. `vtord.py` compara OBJETOS y por eso las ve;
el DOL no las ve. Es el mismo error que la nota `nfsmw-el-cero-que-miente`
avisa en otra forma: la metrica de objeto no es la metrica del enlace.

Consecuencia: **el frente de "dueno" de zFe2 no existe y lo que queda es orden
puro.** Queda escrito en la cabecera del fichero.

### 2a. La descomposicion de los 46.850 B de zFe2

| seccion | B distintos | de que |
|---|---:|---|
| `.text` | 20.637 | 14.220 en UN sitio: `0x8017C670..0x8017FE2C` |
| `.rodata` | 23.956 | 16.632 en `$LC387` + 793 en `$LC72` (pool) y el resto vtables |
| `.data` | 1.859 | **todo** son punteros a `.rodata` que se movieron; cero dato malo |
| resto | 398 | |

`desplaza zFe2`: *"el tamano es EXACTO; lo que queda es ORDEN"*. `gapchk zFe2`:
**0**. No hay aqui el arreglo barato que tenia zAI.

### 2b. El cuerpo del `.text` de las dos unidades esta PERFECTO

Comparando **direcciones ENLAZADAS** (no offsets de objeto: `textorder` cuenta
las 112/315 funciones "solo nuestras", que el enlazador tira y por tanto no
existen en el DOL):

| unidad | funciones comunes en el enlace | solo original | solo nuestras | descolocadas |
|---|---:|---:|---:|---:|
| zAI | 1.036 | 0 | 0 | 359 |
| zFe2 | 1.290 | 17 (alias `.NNNNN` de clase local) | 0 | 199 |

Y las descolocadas estan TODAS al final:

* zAI: delta 0 clavado de `0x800034A0` a `0x80041A80` -- **255 kB, 677
  funciones, cero descolocadas** -- y los 359 saltos viven en los ultimos
  16.316 B.
* zFe2: delta 0 clavado de `0x80142AC0` a `0x8017C670` -- **236 kB** -- y las
  199 en los ultimos 14.268 B.

Ese tramo final es la **cola de inlines diferidos de `finish_file`**
(`_IHandle__*`, `_._*`, `push_back__*`, `ClassKey__*`, `_GetKind__*`,
`Construct__AIGoal*`, los accesores en clase de `AIVehicle`...). O sea: el orden
de los `#include` de las dos SourceLists **ya es el bueno**; mover un `.cpp`
solo puede empeorarlo. Lo que falta es el orden DENTRO de la cola diferida, que
lo manda el punto de primer uso.

### 2c. La palanca dirigida que zFe2 SI tiene y zAI NO

```
python scripts/parseord.py zFe2  ->  CONTROL OK: invertir el parseo reproduce
                                     las 89 vtables de nuestro objeto
                                     posiciones de PARSEO que no casan: 87 de 89
python scripts/parseord.py zAI   ->  !! CONTROL FALLIDO: invertir el orden de
                                     parseo reproduce solo 18 de 73
```

En **zFe2 se puede iterar con `-E` en segundos, sin compilar**. La cabeza del
orden de parseo del objetivo es:

```
HudElement, MenuScreen, IHud, ITachometer, GetAwayMeter, RadarDetector,
MyCallback, HeatMeter, CostToState, Reputation,
  cSlider, TwoStageSlider, FEButtonWidget, FEStatWidget, FEToggleWidget,
  FESliderWidget, IconPanel, FEScrollyBookEnd, IconScroller, IconScrollerMenu,
  ArraySlot, ImageArraySlot, ArrayScroller, ArrayScrollerMenu, UIWidgetMenu,
FEngHud, HudResourceManager, Minimap, Countdown, GenericMessage, ...
```

y la nuestra mete `Minimap` y `Countdown` en los puestos 9-10 y deja el bloque
de widgets en los 34..44. La receta es subir las **CABECERAS** de widgets (los
`.cpp` ya estan bien colocados, ver 2b), midiendo con `parseord` entre edicion y
edicion, y respetando la regla de la r56: **colocar primero la que va mas arriba
en el orden del objetivo**. No me ha dado tiempo a estrenarla; queda escrita en
la cabecera de `zFe2.cpp`.

En **zAI** esa palanca NO se puede dirigir por nombre de clase (control fallido:
`parseord` avisa de que probablemente hay homonimas o clases con mas de una
vtable). Ahi la `.rodata` esta en 25.943 B: ~8.000 de orden del pool `$LC` y
~17.000 de vtables cuyo CONTENIDO son direcciones de la cola diferida --o sea,
que la mitad larga de la `.rodata` de zAI se arregla sola el dia que se arregle
la cola del `.text`.

---

## 3. Negativos y no-arreglos medidos esta ronda

1. **El "primer de pool" para zAI, descartado sin gastar compilacion.** El pool
   de zAI casa CLAVADO en sus 23 primeras cadenas (`GAMECUBE` ... `VehicleParams`,
   `0x803C8C60..0x803C8E44`) y diverge en la 24a: el objetivo pone
   `Attrib::Gen::world` y nosotros `Attrib::Gen::chassis`. Las seis siguientes
   del objetivo (`world`, `MGeneric`, `MSpawnTraffic`, `InitialPos`,
   `InitialVec`, `VehicleName`) son, en ese orden, las seis primeras del bloque
   `asm()` de 56 cadenas del final del fichero, asi que un primer las colocaria.
   **Pero no paga:** el objetivo NO las tiene contiguas (`SuspensionSimple` esta
   en `0x803CA244` y `ZoneType` en `0x803CA634`, a 4 kB de distancia) y a partir
   de la 45a el objetivo intercala FLOATS en el pool, que un primer no puede
   mover (`nfsmw-primer-de-pool`: solo hacia atras y no vale para floats).
   Colocar seis cadenas mueve el punto de divergencia 84 B y no baja el total.
2. **`BWARE_PREFIX_GAMECUBE` en zFe2, descartado.** zFe2 SI tiene la segunda
   copia (`$LC71` = "" de 1 B en `0x803E4854` y `$LC72` = "GAMECUBE" en
   `0x803E4858`), que es donde esta su primera diferencia de `.rodata`. Pero el
   objetivo pone ahi un FLOAT (`0x3E99999A` = 0.3f) seguido de `"Snd"`: matar
   las dos copias adelanta el pool 16 B y **no** mueve el punto de divergencia,
   ademas de exigir compensacion. Encaja con el negativo de la r60/r61, que ya
   midio la variante empaquetada y dejo veda. No he gastado compilacion.
3. **Reordenar los `#include` de zAI o de zFe2: contraindicado.** Ver 2b: el
   cuerpo del `.text` de las dos esta a delta 0 en 255 kB y 236 kB. Cualquier
   movimiento de `.cpp` rompe eso. El trabajo esta en la cola diferida y en las
   CABECERAS, no en los `.cpp`.

---

## 4. Herramientas que use y que no estaban

Todas mis sondas viven en `scratchpad/ordenai63/` y las borro al acabar, pero
**dos merecen subir a `scripts/`** si el jefe quiere (no las he promovido para no
tocar `scripts/`):

* comparar el `.text` **ENLAZADO** de una unidad contra `NFSMWRELEASE.ELF` en vez
  de comparar objetos. `textorder.py` dice 359 descolocadas por unos motivos y el
  enlace por otros: en zAI las 112 "SOLO NUESTRAS" del objeto son **0** en el
  enlace. La cifra que manda es la del enlace.
* `dolwhere` con **resumen por seccion** y con `keep.lst` parametrizable. Sin lo
  primero no se ve que el 69 % de los 244.740 B de zAI estaban en `.data`; sin lo
  segundo no se puede medir una propuesta de `keep.lst` sin editar el fichero
  compartido (que ademas esta modificado por otro agente ahora mismo).
