# r60 — `world` (zWorld, zTrack, zWorld2): 295.741 B de DOL, y la palanca no estaba en el codigo

Agente `world`. **Diez ficheros tocados, todos EXCLUSIVOS de mis unidades** (comprobado con
`grep -rl <fichero> src/Speed/*/SourceLists/*.cpp`, uno por uno):

    src/Speed/Indep/SourceLists/zTrack.cpp
    src/Speed/Indep/SourceLists/zWorld.cpp
    src/Speed/Indep/SourceLists/zWorld2.cpp        <- solo comentario (negativo medido)
    src/Speed/Indep/Src/World/World.cpp
    src/Speed/Indep/Src/World/CarRender.cpp
    src/Speed/Indep/Src/World/rain.cpp
    src/Speed/Indep/Src/World/VehiclePartDamage.cpp
    src/Speed/Indep/Src/World/HeliRenderConn.h
    src/Speed/Indep/Src/World/HeliRenderConn.cpp
    src/Speed/Indep/Src/World/OnlineManager.cpp

Ni `config/`, ni `splits.txt`, ni `keep.lst`, ni `configure.py`, ni una sola cabecera
compartida (`OnlineManager.hpp` sigue intacta; ver §6). **Cero sondas en `scripts/`.**

---

## 0. Titular

| unidad | `dolwhere` antes | despues | linkdelta | `fncmp` antes -> despues | sha1 del `.o` |
|---|---:|---:|---|---|---|
| **zWorld**  | **245.700** | **29.551** | IGUAL -> IGUAL | 4/582 -> **4/582** | `2fbe00844121d7cabd5cd1d2f1e4df08b8fd8b03` |
| **zTrack**  | **87.702** | **8.110** | IGUAL -> IGUAL | 1/259 -> **1/259** | `ee729af53687d7928e0ff7a3a6e1124ed1ec4e30` |
| zWorld2 | (secciones descuadradas) | igual | bss+64 -> bss+64 | 2/357 -> **2/357** | `3ff5ce83804d0f8185bdeeee8188e82e3b9c1fbd` |

**295.741 B menos de DOL. Cero regresiones de codigo en las tres.**
Sello: **tres** compilaciones seguidas del arbol final, mismo `sha1` las tres.

`trypromo` (ruta completa) da **DOL ROTO** en las tres: `zWorld 168fdd8a9efb`,
`zTrack 5351b5bdb326`, `zWorld2 a38849185f1e`. **Ninguna promociona** — y §5 dice por que
ninguna de las tres podia promocionar esta ronda por mucho orden que se arreglase.

`lcfix.py --check`: **311 correcciones pendientes, y NINGUNA es de mis unidades**
(142 `zFe.o`, 84 `zLua.o`, 43 `zSpeech.o`, 42 `zAI.o`; la cifra se movio de 251 a 311
mientras medía, porque hay agentes anadiendo literales en paralelo -- en mis tres unidades
sigue siendo CERO en las dos lecturas). **Nada de lo que entrego depende de que se
aplique `lcfix`**, y ninguna entrada pendiente rompe zWorld/zTrack/zWorld2.

---

## 1. LA PREMISA DEL ENCARGO ERA FALSA, Y ESO ES EL HALLAZGO

El encargo decia: «zWorld y zTrack tienen las NUEVE secciones a IGUAL, son candidatas
directas a promocion». Las nueve secciones estaban a IGUAL **y aun asi el DOL se
diferenciaba en 245.700 y 87.702 B**. La causa se ve en una linea:

    linkdelta compara el TAMANO de la seccion, y el DOL rellena cada seccion a 32 B.
    Una unidad a la que le falten 24 B de .data mide IGUAL y desplaza 88 kB de OTROS objetos.

**`linkdelta IGUAL` no significa «no falta contenido»: significa «no falta mas de 31 B».**
Es la ventana de 32 del dossier, pero leida al reves de como la lee el frente: no es que un
`-24` pueda estar bien, es que **un `IGUAL` puede estar mal por hasta 31 B en cada seccion**, y
ese error no cuesta 24 B de DOL sino **decenas de miles**, porque corre todo lo que va detras.

La herramienta que lo destapa en un minuto (esta en mi scratchpad, `runs.py`): enlaza dos
veces --base y con la unidad sustituida-- y **lista los desplazamientos por RACIMOS
CONTIGUOS**, no por simbolo. Un escalon que comparten 402 simbolos y arranca fuera de la
unidad es, sin ambiguedad, «nuestra aportacion mide de menos». `movidos.py` ya calcula ese
escalon pero lo imprime como «ESCALONES SIN EXPLICAR» y lo atribuye a un cambio de dueno;
**en las tres unidades no era cambio de dueno, era contenido que faltaba**.

---

## 2. zTrack: 24 B que `keep.lst` llevaba dos rondas pidiendo — 87.702 -> 8.110

`keep.lst` nombra `zTrack.o:lbl_8043710C` desde la r58. **El bloque de la r59 lo saltó**: los
otros catorce estan escritos, ese no. Son 24 B (`1.8f, 0.45f, 0, 1, 2, 3`) que en el objetivo
van entre los estaticos de `WeatherMan.cpp` y `debugflash` de `ScreenEffects.cpp`.

Sin ellos la `.data` de zTrack acababa 24 B antes, el DOL rellenaba a 32 y `linkdelta` decia
IGUAL mientras **705 simbolos --toda la cola de `.data`, 95 kB de otros objetos-- caian 24 B
mas abajo**. Una linea de `asm()`:

    dolwhere   87.702 -> 8.110 B      (.data 77.371 -> 360)
    y de paso el escalon de .bss (-8, 217 simbolos) desaparece solo

**Como se caza en un minuto**, y vale para cualquier unidad del frente:

    comm -23 <(grep "^<u>.o:" config/GOWE69/keep.lst | sed "s/^<u>.o://" \
                 | grep -E '^(lbl_|gap_|pad_)' | sort) \
             <(grep -oE '\.globl (lbl_|gap_|pad_)[A-Za-z0-9_]+' \
                 src/Speed/Indep/SourceLists/<u>.cpp | sed 's/.globl //' | sort -u)

En zTrack salian 2 (dos huecos de `.bss`), en **zWorld 40** y en **zWorld2 16**. La lista de
la compra de la r58 **no esta cobrada**.

---

## 3. zWorld: TRES palancas, 245.700 -> 29.551

### 3.1 Los 8 B de `lbl_80438F08` (245.700 -> 243.684)

El mismo caso que zTrack: `keep.lst` lo nombraba, nadie lo emitia. Fue solo la mitad del
deficit --dejo la cola a -8-- y esa otra mitad es el hallazgo que sigue.

### 3.2 `keep.lst` protege los ESTATICOS por el sufijo del objeto ORIGINAL (243.684 -> 174.473)

**Es una trampa nueva y afecta a todo el proyecto.** `keep.lst` tiene:

    zWorld.o:index.14943      zWorld.o:windState.14947      zWorld.o:changetime.14948

y son los tres estaticos de `rain.cpp` (`static ... index`, `windState`, `changetime`), datos
**MUERTOS**: nadie los referencia, ni en el objetivo ni aqui. `cc1plus` les pone **otro
sufijo** --`index.25654`, `windState.25658`, `changetime.25659`--, asi que
`-strip-unused-data` **se los lleva a nosotros y al objetivo no**. Doce bytes.

El sintoma no se parece en nada a la causa: la `.data` acababa 8 B antes (los otros 4 los come
la alineacion) y **402 simbolos de otros objetos caian desplazados, 74 kB de DOL**.

    dolwhere  243.684 -> 174.473

El arreglo sin tocar `keep.lst`: **definirlos en el `asm()` con el nombre exacto del
objetivo** (contenido leido del DOL en 0x80438A34 / 0x80438A40 / 0x80438A44). Los que emite
`cc1plus` con el otro sufijo siguen ahi y se estripan; no hay colision.

**Como se busca**: comparar los simbolos del ENLACE base contra los del nuestro **por nombre
BASE** (quitando `\.\d+$`). Por nombre exacto salen diez falsos positivos --los estaticos con
sufijo distinto que si sobreviven--; por nombre base salen los tres de verdad.

### 3.3 La funcion de CUATRO BYTES que corria 497 simbolos (175.335 -> 36.297)

`OnlineManager::InitQuantizers()` es un cuerpo vacio, **4 B**. Definida fuera de linea en
`OnlineManager.cpp` --el primer `#include` de zWorld-- salia **la primera de todo el `.text`
de la unidad** y empujaba **+4 B a los 497 simbolos siguientes**. El objetivo la tiene en
**0x802EBE24, la primera de la COLA de `finish_file`**.

    inline void OnlineManager::InitQuantizers() {}     <- una palabra

    dolwhere  175.335 -> 36.297      (-139.038 B por 7 caracteres)

`OnlineManager.cpp` es **exclusivo de zWorld**, asi que la palanca **no toca `OnlineManager.hpp`**,
que si la incluyen zAI, zFe, zMain, zMisc y zSim. Poner el cuerpo dentro de la clase habria
funcionado igual pero habria cambiado cinco unidades ajenas: **`inline` en la definicion fuera
de linea da el mismo sitio y no sale del fichero**. Y no cambia el llamante: `fncmp` da las
mismas 4 funciones antes y despues, `__static_initialization_and_destruction_0` incluida.

Antes de esta, el mismo mecanismo con el destructor de `HeliRenderConn` (104 B, cuerpo a
dentro de la clase) mato el escalon de **+108 sobre 356 simbolos**; en bytes salio neutro
(+862, porque el cuerpo cae en la cola en un sitio que todavia no es el suyo) pero **es lo que
dejo el `.text` en un solo escalon de +4**, que es lo que hizo legible el caso de
`InitQuantizers`. **Las dos van juntas: la primera no paga sola y sin ella la segunda no se ve.**

### 3.4 Los once blobs a su hueco: la `.data` de zWorld queda PERFECTA (36.297 -> 29.551)

Los quince `lbl_`/`gap_` del bloque `asm()` estaban **todos al final del TU** y por eso la
`.data` salia en una escalera de once escalones (-4, -28, -32, -36, -40, -44, -56, -60, -68,
-76, -84). Un `asm()` de fichero se emite **DONDE ESTA ESCRITO**, asi que cada uno se ha
movido a su hueco real, entre las dos variables C++ que lo rodean en el objetivo:

| fichero | blobs |
|---|---|
| `World.cpp` | `gap_06_8043724C_data` (delante de `pCurrentWorld`), `lbl_80437270..84`, `gap_06_804372A4_data` |
| `CarRender.cpp` | `lbl_804372DC`, `gap_06_804372E8_data`, `gap_06_80437604_data`, `lbl_80437614` |
| `rain.cpp` | `index.14943` / `windState.14947` / `changetime.14948`, `lbl_80438AA4`, `lbl_80438AA8` |
| `VehiclePartDamage.cpp` | `lbl_80438F08` |
| `zWorld.cpp` | solo queda `gap_06_80438F18_data`, que es el que cierra la seccion |

    dolwhere  36.297 -> 29.551      y `runs` no deja UN SOLO simbolo de .data fuera de sitio

Es la receta que el dossier le pone a `zEcstasy` («los 59 del bloque `asm` salen juntos al
final en vez de intercalados»), **medida y confirmada**: 6,7 kB de los 7 que valia. Los
1.344 B que quedan en `.data` **no son orden**: son reubicaciones a la `.rodata`, que sigue
permutada (`CarSlotIDNames`, `CarEffectParameters`).

---

## 4. Lo que queda, con su cifra

### zWorld — 29.551 B

| seccion | B | que es |
|---|---:|---|
| `.rodata` | **13.862** | **el orden del pool**. Una sola tirada de 10.617 B desde `$LC533`. |
| `.text` | 14.336 | **6.929 son las dos VETADAS** (`UpdateWheelYRenderOffset` +4 B, `RenderFlaresOnCar` -4 B: el escalon de -4 sobre los 9 simbolos que van entre ellas). 2.172 son el ORDEN DE LA COLA (`__as__Q36Attrib3Gen5tires`, `OnGrowRequest`). El resto, ~5.000 B en 1.280 rangos de 1-4 B, son **reubicaciones a la `.rodata` movida**: se van solas cuando se arregle el pool. |
| `.data` | 1.344 | reubicaciones a la `.rodata`. **Cero problemas de orden.** |
| `.sdata` | 9 | arrastre del escalon de `.bss` (-24 en la cola). |

**El unico frente real que le queda a zWorld es el pool de `.rodata`.** Y `.bss` tiene tres
escalones (+40 / +36 / -24) y **once `gap_07_*_bss` en `keep.lst` sin emitir** (211 B): no es
una suma, porque emitirlos todos deja la cola en +187; hay que emparejarlos con lo que
nosotros ponemos de mas.

### zTrack — 8.110 B

| seccion | B | que es |
|---|---:|---|
| `.rodata` | **3.301** | 3.044 B en **una sola tirada desde 0x80409174**, justo donde acaba el prefijo escrito a mano. Causa nombrada en §5.1. |
| `.text` | 4.440 | ~206 de `GetLoadingPriority`, 300 de **un swap** (`ClearTable__9bBitTable` va delante de las cinco de `TSMemoryPool` en el objetivo y detras en el nuestro; las dos son de la COLA), y el resto reubicaciones al pool. |
| `.data` | 360 | la misma escalera de zWorld, **sin arreglar**: los quince blobs siguen al final del TU. Receta de §3.4, ~1 hora. |

### zWorld2 — sin tocar

`bss+64`. El censo esta hecho: nuestra `.bss` emite **7 pares `value.*`+guarda (56 B)** que
salen de los `static UCrc32 value = "..."` de las cabeceras de `Physics/` y que el objetivo
**no tiene en esta unidad**, mas `debugSpline` (108 B, que se estripa). El objetivo tiene 98 B
de huecos de alineacion entre sus estaticos que nosotros no reproducimos. **El camino es
apagar las 7 instanciaciones, no anadir relleno.**

---

## 5. Los dos negativos, medidos y escritos junto a la fuente

### 5.1 zTrack: `#define BWARE_PREFIX_GAMECUBE` — **NEGATIVO, y no por la fuente**

El diagnostico es correcto y esta confirmado byte a byte: el objetivo tiene `"GAMECUBE"` **solo
en el prefijo**, y nosotros emitimos una segunda copia justo delante de `"Attrib::Attribute"`,
cabecera de un bloque de **~470 B de etiquetas de AttribSys duplicadas** (`rodorden zTrack`,
fila `nue[23:50]`) que el objetivo no tiene ahi. Es exactamente la causa de los 3.044 B.

Pero quitar el literal **renumera los `$LC`** y `keep.lst` pasa a proteger otra cosa:

    linkdelta   IGUAL -> rodata-160        (y las secciones del DOL dejan de cuadrar)

**No es un negativo de fuente: es que la edicion OBLIGA a correr `lcfix`**, y eso reescribe una
entrada del ENLACE con agentes midiendo. Queda escrito en `zTrack.cpp` para la ventana en que
se pueda tocar `keep.lst`; **es la promocion de zTrack menos `GetLoadingPriority`.**

### 5.2 zWorld2: intercambiar `WPathFinder.cpp` y `WRoadNetwork.cpp` — **NEGATIVO**

El sintoma es exacto: el bloque `AStarSearch`/`PathFinder` (17 simbolos, 5.736 B) va en el
objetivo **delante** del de `WRoadNetwork` (105 simbolos, 50.292 B), y salen permutados
**+50.292 y -5.736 clavados**. Pero el swap paga caro por otro lado:

    fncmp      2/357 -> 3/357   (__static_initialization_and_destruction_0, 65 insn)
    linkdelta  bss+64 -> rodata-168 bss+64

El bloque hay que moverlo **sin** mover el orden de inicializacion estatica ni el pool: la
palanca no es el `#include`. Escrito en `zWorld2.cpp`.

---

## 6. Por que ninguna de las tres podia promocionar, y quien lo sabia

Las tres tienen codigo sin casar, y **las cuatro funciones son callejones documentados**:

| unidad | funciones | estado |
|---|---|---|
| zWorld | `RenderFlaresOnCar`, `UpdateWheelYRenderOffset` | **VETADAS con prueba** (`lo_sum (high` cero veces en 599 funciones) |
| zWorld | `DefragmentPool`, `SetMemoryPoolSize` | cuatro negativos cada una (r36/r46/r48/r49) |
| zTrack | `GetLoadingPriority` | **r46/r48/r49**: el temporal de 16 B sin nombre del bloque muerto; prohibido rellenarlo con una local inventada |
| zWorld2 | `HolePunchAvoidables` (4 insn), `InitAtSegment` (12 insn) | las unicas VIVAS del reparto |

`previo.py` las da todas en un segundo. **El encargo mandaba «candidatas directas a
promocion» a tres unidades cuyo codigo estaba bloqueado antes de empezar**; lo que si estaba
libre --y valia 295.741 B-- eran los datos y el orden.

**La unica de mis tres con codigo atacable es `zWorld2`**, y encima esta cerca: 4 instrucciones
en `HolePunchAvoidables` y 12 en `InitAtSegment`. Es la fila que hay que repartir, no zWorld.

---

## 7. Para la proxima ronda, en orden de valor

1. **Cobrar la lista de la compra en TODO el frente.** El `comm` de §2 sobre las 23 unidades
   que quedan. En mis tres habia 58 simbolos de `keep.lst` sin emitir y dos de ellos valian
   146.000 B. Es minutos por unidad.
2. **El barrido por nombre BASE de §3.2 en todo el frente.** `keep.lst` protege estaticos por
   el sufijo del objeto original; cualquier unidad con estaticos muertos tiene el mismo
   agujero, y el sintoma (`IGUAL` + decenas de kB) no se parece a la causa.
3. **`zTrack`: los quince blobs a su hueco** (receta §3.4, ~1 h) y el swap de `ClearTable`.
4. **`zWorld2`: las 4 instrucciones de `HolePunchAvoidables`.** Es lo unico promocionable que
   he tocado en toda la ronda.
5. **La ventana de `keep.lst`**: §5.1 deja zTrack a una funcion.
