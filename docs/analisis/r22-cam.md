# Ronda 22 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes ganados: 0.** Lo que sí se mueve: `_Storage` copia **96,1592 -> 97,8339**
(+1,675 pp) y tres funciones más quedan **estructuralmente idénticas al DWARF del
original** sin cambiar un byte. `pctsnap --cmp` sobre las **33 SourceLists /
15.493 funciones**: **EMPEORAN: ninguna. MEJORAN: 1.**

`measure.py --cmp` sobre las 33 SourceLists: **+0 B, +0 funciones, 0 unidades
cambian** (la cabecera tocada es compartida; ver §6).

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera` reproducen el brief clavado:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  96.159%  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2...  faltan 5, sobran 4, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 3868 Update__8ICEMoverf / 1192 TerrainVelocityNoise / 992 TrackCar / 948 TrackCop

## 1. `audit.py` — LIMPIO, dos pasadas, antes y después

`audit.py Speed/Indep/SourceLists/zCamera`: **446/446 ok, cero FALLA**, y la
segunda pasada da un fichero **byte a byte idéntico**. Repetido al terminar con
el árbol final: **446/446 ok, cero FALLA, pasadas idénticas.**
**Lista de FALLA: vacía.**

`pines.py`: **ningún pin de zCamera** está en función que no case (los dos que lo
están siguen siendo `DynamicLoader::Initialize` y `WRoadNav::InitAtSegment`).

## 2. LO QUE CAMBIA EL JUEGO: `dwbody.py` LEÍA UN VOLCADO RANCIO

`dwbody.py` saca el lado NUESTRO de `build/regmap/our_<unidad>_<tam>_<hash>.nothpp`
y **ese fichero no se regenera solo**: hay que lanzar `regmap.py <unidad> <func>`
antes. Con el volcado de hace dos días, `dwbody` decía «árbol idéntico» donde no
lo era. Al refrescarlo aparecieron **cuatro diferencias estructurales reales**
en tres funciones distintas, y una de ellas valió +1,675 pp.

**Y una segunda trampa, peor:** `dwbody.py` coge **`o[ho[0]][0]`, el PRIMER
cuerpo** con esa firma. Para `__static_initialization_and_destruction_0`, que
existe en las 34 unidades, eso significa que **estaba comparando la de otra
unidad**: el diff daba 1021 líneas contra 498 y era ruido puro. Hay que filtrar
por dirección. El extractor que sí funciona quedó en el scratchpad
(`c22cam_dw3.py`): localiza la cabecera por número de línea en
`symbols/mw_dwarfdump.nothpp` (las de static-init salen con
`grep -n "__static_initialization_and_destruction_0"` + la línea `// Range:`
anterior) y equilibra llaves.

## 3. `_Storage<Director*,2>` copia (1.156 B): 96,1592 -> 97,8339

`faltan 5, sobran 4, 1 SUST` -> **`faltan 1, sobran 2, 1 SUST`** (`cmpwi-1,
lwz-1, mr.+1`). De seis grupos de diff quedan dos.

**No hay hermana de la plantilla al 100 %:** en TODO el árbol hay **una sola**
copia de `_Storage`/`FixedVector` con símbolo propio (ésta). `reserve`,
`push_back` y `~FixedVector` sí están al 100 % en muchas unidades, y ésos NO se
tocan. Por eso la vía del brief (buscar la hermana ya cerrada) está cerrada, y
hubo que sacar la forma del DWARF y del mapa de líneas.

### El mapa de líneas del original reconstruye `UTLVector.h` entero

`lmap.py` da **la línea del `UTLVector.h` ORIGINAL** de cada instrucción. Sale
esto (anotado; sirve para cualquier ronda futura):

    118 size()        124 capacity()     145/148 begin()      157/160 end()
    172 push_back()   173 if(size()>=capacity())   174/175 reserve(GetGrowSize(size()+1))   178 mSize++
    183 push_back(const&)  184 if   185 reserve  188 new(...)T(val)  189 mSize++
    194/197 pop_back()
    203 reserve: if(num>capacity())   205/206 OnGrowRequest   208 oldSize=size()
    212 mBegin=AllocVectorSpace   215 mCapacity=num   218 if(oldBuffer!=mBegin)
    219/220 mSize=0   222 for(ii...)   224 push_back(oldBuffer[ii])   225 }   229 if(oldBuffer)   231 FreeVectorSpace
    241/242 resize: if(num>size())    247 reserve(num)    249 while(num>size())
    270/272/274 make_empty()
    301 minSize = srcEnd - srcBeg     306 if (srcIt == 0)     307 resize(minSize)   310 }
    314 if(minSize>capacity())  316 Contains(srcBeg)  317 make_empty()  318 reserve(minSize)
    320 destIt = begin()
    326 while (destIt != end() && srcIt != srcEnd)
    328 dest = *destIt++    329 src = *srcIt++     330 dest = src
    334 while (end() != destIt)   335 pop_back()   336 }
    340 while (srcIt != srcEnd)    344 push_back(*srcIt++)   345 }
    578-583 Init()   595-599 Contains()   721 FixedVector(const FixedVector&)

### Los cuatro cambios que SE QUEDAN (todos confirmados por DWARF o por lmap)

1. **`assign`, rama nula: `minSize > size()` en vez de `size() < minSize`.**
   Cierra el SENTIDO de las dos ramas (`ble`/`bgt` del objetivo contra
   `bge`/`blt` nuestro). 96,1592 -> 96,2111.
2. **El cuerpo del bucle de copia son TRES sentencias con dos referencias.**
   El DWARF del original trae un bloque anónimo con
   `Director *& dest; Director * const & src;` y `lmap` pone las líneas
   328/329/330 en `mr r11,r10`+`addi r10,r10,4` / `mr r9,r27`+`addi r27,r27,4` /
   `lwz`+`stw`. O sea:

        while (destIt != end() && srcIt != srcEnd) {
            reference dest = *destIt++;
            const_reference src = *srcIt++;
            dest = src;
        }

   Nuestro `*destIt++ = *srcIt++;` coalescía las dos copias. **Este solo cambio
   vale 96,2111 -> 97,7509** y cierra el bucle entero (10 filas de diff).
3. **`while (end() != destIt)` en el bucle de `pop_back`** (el objetivo emite
   `cmpw r0,r10` = end,destIt; nosotros `cmpw r11,r0`). 97,7509 -> 97,7855.
4. **La inicialización a cero vive en `Init()`, no en `Vector()`, y
   `FixedVector(const FixedVector&)` llama a `Init()` explícitamente.** El DWARF
   del original tiene DOS expansiones: `Vector()` (que contiene `Init()`) y un
   `Init()` **hermano** delante de `operator=`. Eso pone el `stw` del vtable
   ANTES de los tres ceros, que es el orden del objetivo (los ceros del ctor base
   mueren como almacenamientos muertos). 97,7855 -> **97,8339**.
   **AVISO: hay que quitar la llamada a `Init()` de `clear()` a la vez.** Sin eso
   `clear()` emite tres `stw` de más y **se caen 58.412 B / 115 funciones en 10
   unidades** (zWorld2 −14.888, zAI −12.768, zPhysics −11.928, zSim −9.380,
   zEAXSound2 −2.792, zEAXSound −2.224, zSpeech −2.096, zCamera −1.256,
   zMain −720, zWorld −360). Con `clear() { make_empty(); }` el delta es **+0 B**.
   Como `Init()` estaba vacío, quitar esa llamada no cambia una sola instrucción
   hoy.

### Lo que queda (2 grupos) y la palanca medida

    >>> 14-22  el objetivo funde `mr`+`cmpwi` en `mr. r27,r0` y el `subf` usa
               srcBeg (r0), no srcIt (r27)
    >>> 68-73  y 132-134: el objetivo mantiene size() en un pseudo (r4) a lo
               largo del bucle (`lwz r4` a la entrada, `mr r4,r9` al fondo);
               nosotros recargamos -> el `lwz` de más

**El DWARF dice que el original declara `minSize` ANTES que `srcIt`** (orden de
locales `minSize (r26)`, `srcIt (r27)`, `destIt (r10)`; el nuestro los tiene al
revés). Aplicarlo (**ensayo c17**) **SÍ produce el `mr. r27,r9`** que falta —
pero entonces `combine` pliega `(srcBeg+size*4)-srcBeg` a `size*4` y **desaparece
el `subf`** que el objetivo sí tiene. Medido: 97,8339 -> **96,6055**,
`faltan 2, sobran 1` (`lwz-1, subf+1, mr+1`), 1152 B contra 1156.
**Mecanismo entendido y anotado, revertido por dar peor diff.** Quien lo retome
necesita las dos cosas a la vez: el `mr.` (que pide `minSize` primero, para que
r27 no se use entre la copia y el test — `combine` rechaza cuando
`reg_used_between_p(i2dest,i2,i3)`) y el `subf` sin plegar.

## 4. `LoadCameraShakes` (168 B): estructura AL COMPLETO, sigue faltando el `li`

El DWARF del original da el cuerpo entero y **ahora casa entero** salvo el
registro de `warned_overflow`:

    void ICEManager::LoadCameraShakes(bChunk *set_chunk) {
        unsigned int id = set_chunk->GetID();   // muerta
        bool warned_overflow;                   // r0 en el original
        bChunk *chunk;                          // muerta
        {                                       // <-- BLOQUE ANONIMO, no lo teniamos
            unsigned int *p_handle = (unsigned int *) set_chunk->GetData();
            bPlatEndianSwap(p_handle);          // <-- NO bEndianSwap32 directo
            ICEShakeGroup *group = pShakeGroup;
            if (group) {
                warned_overflow = false;
                int num_tracks = *p_handle;
                ...
            }
        }
    }

Aplicado (**ensayo d3**, se queda). `regmap.py` pasa de tres veredictos a uno:

    fn        warned_overflow   r0  ->  -      (unica que falta)
    b0/b0     num_tracks        r28 -> r27     (permutacion)
    b0/b0/b0  i                 r27 -> r28     (permutacion)

y `dwbody` da **cero** diferencias de árbol. 168 vs 164 B, 96,667 % (sin cambio).

**El mecanismo, ya identificado:** el `li r0,0` que falta **es**
`warned_overflow = false`. CSE mete las dos constantes 0 en la misma «cantidad»
y `canon_reg` sustituye el `i` del test de ENTRADA por el pseudo de
`warned_overflow` (`qty_first_reg`), que así deja de estar muerto; el contador
conserva su propio `li r27,0`. **Nos falta que ese pseudo sobreviva**: GCC lo
borra por muerto. Barridas ya **diez** posiciones/formas (v1-v4, w1-w4 de la r21
más d1 y d3 de ésta). Sigue sin aparecer un uso de `warned_overflow` que no
emita código.

**Dato nuevo del mapa de líneas:** entre la `}` del `for` (6821) y la `}` de la
función (6826) el original tiene **cuatro líneas** que no emiten nada. Ahí es
donde tiene que estar el uso de `warned_overflow`. No he sabido inventarlo sin
falsificar fuente.

## 5. `TrackCopCameraMover::Update` (948 B): el `bFill` era un `bScale`

El diff sigue siendo **una sola instrucción intercambiada** (`stfs f30,0x58(r1)`
una ranura antes que `mr r5,r3`), pero el DWARF destapa que la fuente estaba mal:

    original:  bScale(bVector3 *dest, const bVector3 *v, float scale)  // locales x,y,z
    nuestro:   bFill (bVector3 *dest, float x, float y, float z)

Cambiado a **`bScale(&hcomp, &hcomp, vert_comp)`** (**ensayo c16, se queda**):
`dwbody` deja de señalarlo y el código sale **byte a byte idéntico** (con
`-ffast-math`, `x*0.0f` pliega a `0.0f` y las tres cargas mueren).

**Lo que el brief proponía NO vale:** mover el `bFill` detrás del `eMulVector`
haría que sus tres `stfs` cayeran **detrás** de la llamada (el planificador no
cruza un `call`), y el objetivo los tiene **delante**. Descartado sin gastar
compilación.

**Diferencias de DWARF que quedan** (todas expanden a cero código):

- **dos `bTan(unsigned short angle)`** justo antes de `bVector3::operator/=`
  (la local `unsigned short fov` del original no tiene registro: está muerta),
- **tres accesos más**: `GetGeometryPosition()`, `GetVelocity()`,
  `GetGeometryPosition()` entre nuestro `bDistBetween(...)` y `SetTargetDistance`.

Y el mapa de líneas dice que el original tiene **5 líneas de fuente más** entre
el `bCross` (su 361, nuestra 330) y el `look_offset.x` (su 370, nuestra 334).

**No sirven para el desempate**: el orden del planificador se decide por
`INSN_LUID`, que se recalcula **después** de la eliminación de código muerto, y
los dos operandos en litigio (`r3`, `r5`) los fija la ABI. Documentadas por
fidelidad, no como palanca.

## 6. `__static_initialization_and_destruction_0` (3.604 B): HIPOTESIS CERRADA

La ronda 21 dejó esto: «hay ~27 insns RTL que MUEREN en alguna sentencia anterior
de la unidad; busca cuál». **Ese frente está cerrado, con medida:**

**El árbol de inlines del original y el nuestro son IDÉNTICOS** — 500 líneas
contra 500, y la ÚNICA diferencia en toda la función son las seis expansiones
del constructor de la tabla:

    -  ICEReplayCategory::ICEReplayCategory(char *category_name, char *scene_name, ...)
    +  ReplayCategory   ::ReplayCategory   (const char *name,    const char *replay, ...)

(esto sale del extractor filtrado por dirección; el `dwbody.py` normal comparaba
la static-init de OTRA unidad, ver §2).

O sea: **no falta ninguna sentencia en ninguna de las decenas de globales que se
construyen antes de la tabla.** Buscar «la sentencia que emite menos RTL» no
tiene dónde entrar: el DWARF dice que están todas.

El struct del original, del volcado (`mw_dwarfdump:342447`):

    struct ICEReplayCategory {
        ICEReplayCategory(char *category_name, char *scene_name,
                          float (*score_function)(ICEAnchor *), bool (*mirror_function)(ICEAnchor *));
        float GetScore (ICEAnchor *car);
        bool  GetMirror(ICEAnchor *car);
        char *pCategoryName;   // 0x0     unsigned int nCategoryHash; // 0x4
        char *pSceneName;      // 0x8     unsigned int nSceneHash;    // 0xC
        float (*ScoreFunction)(ICEAnchor *);  // 0x10
        bool  (*MirrorFunction)(ICEAnchor *); // 0x14
    };

**Ensayo e1 (se queda):** `const char *` -> `char *` en los dos miembros y en los
dos parámetros del constructor. **Cero cambio** (una conversión de cualificación
no emite RTL), pero es la forma del original y ya no miente. **NO** he renombrado
la clase a `ICEReplayCategory` ni los miembros a
`pCategoryName/nCategoryHash/pSceneName/nSceneHash/ScoreFunction/MirrorFunction`
para no tocar `ICEReplay.cpp` e `ICEManager.cpp` en un árbol compartido; queda
apuntado.

**Cuánto hace falta, con la aritmética hecha** sobre las sondas de la r21
(`insns(N) = 901 + p(N) + extra(N)`, con `p(N) ≈ 3N+1`):

    N=0  905 = 901 + 0  + 4        N=6  925 = 901 + 19 + 5
    N=1  909 = 901 + 4  + 4        N=8  931 = 901 + 25 + 5
    N=2  913 = 901 + 7  + 5        N=10 932 = 901 + 31 + 0

La ventana mala termina entre **+25 y +31 insns RTL**. Y bajar no vale: r21 midió
`-fno-gcse` con **899** insns y los 4 `addi` seguían ahí.

**Veda nueva y firme:** *no* hay sentencia ausente en zCamera. Cualquier intento
futuro tiene que atacar **la forma de una expresión** que expanda 26-31 insns RTL
de más que mueran, o el pase de CSE directamente. Sin volcado RTL del original no
hay entrada, y el DWARF ya no puede señalar nada más.

## 7. Ensayos numerados, con su cifra

    c1   assign rama nula: `minSize > size()` (guarda y do-while)   96,1592 -> 96,2111   SE QUEDA
    c2   rama nula como `while (minSize > size())` pelado           96,1592 -> 94,0381   REVERTIDO
    c3   `minSize` declarada antes que `srcIt` (sobre c1)           96,2111 -> 95,4844   REVERTIDO
    c4   `if (srcBeg == 0)` en vez de `if (srcIt == 0)`             96,2111 -> 96,2111   REVERTIDO (sin efecto)
    c5   rama nula = `resize(minSize)` + `while(num>size())`        96,2111 -> 93,2076   REVERTIDO (+24 B: la rama `else` de resize NO pliega)
    c6   c5 + Init/Vector()                                         93,2076 -> 93,1661   REVERTIDO
    c7   c5 + `else while (num < size())`                           93,2076 -> 93,2249   REVERTIDO
    c8   c1 + `reference dest`/`const_reference src` en el bucle    96,2111 -> 97,7509   SE QUEDA
    c9   c8 + `while (end() != destIt)`                             97,7509 -> 97,7855   SE QUEDA
    c10  c9 + Init() zeroing + FixedVector(const&) llama Init()     97,7855 -> 97,8339   SE QUEDA (con clear() corregido)
    c11  c10 + `resize(minSize)`                                    97,8339 -> 94,7405   REVERTIDO (1184 B)
    c12  c11 + resize sin `else` (shrink suelto delante)            94,7405 -> 90,0173   REVERTIDO (1204 B)
    c13  c10 + rama nula como `while` pelado                        97,8339 -> 94,9654   REVERTIDO
    c14  c10 + `minSize` antes de `srcIt`                           97,8339 -> 96,6055   REVERTIDO
    c15  c10 + `Contains(srcBeg)` + `make_empty()` (lineas 316/317) 97,8339 -> 95,8270   REVERTIDO (1172 B: Contains SI emite)
    c16  TrackCop: `bFill(&hcomp,...)` -> `bScale(&hcomp,&hcomp,vert_comp)`
                                        99,15612 -> 99,15612, .text IDENTICA   SE QUEDA (cierra una diferencia de dwbody)
    c17  = c14 remedido sobre el estado final, evaluado por triage:
         `faltan 1,sobran 2` -> `faltan 2,sobran 1`; APARECE el `mr. r27,r9`,
         DESAPARECE el `subf`.  97,8339 -> 96,6055, 1152 B    REVERTIDO
    d1   LoadCameraShakes con id/warned_overflow/chunk + bloque anonimo +
         bPlatEndianSwap + parametro `set_chunk`               96,667 -> 96,667   base de d3
    d3   d1 con `warned_overflow` declarada en la funcion y asignada dentro
         del `if (group)`                                      96,667 -> 96,667   SE QUEDA (dwbody a cero)
    e1   ReplayCategory: `const char *` -> `char *` (miembros y ctor)
                                                               97,112 -> 97,112   SE QUEDA (forma del DWARF)

Y el A/B obligatorio de la cabecera compartida (`UTLVector.h`):

    variante c10 SIN corregir clear()   -> -58.412 B, -115 fns, 10 unidades BAJAN
    variante c10 CON clear() corregido  -> +0 B, +0 fns, 0 unidades cambian
    estado final, pctsnap por funcion   -> EMPEORAN: ninguna;  MEJORAN: 1 (+1,675 pp)

## 8. Vedas nuevas (con la sentencia barrida)

1. **No falta ninguna sentencia en `__static_initialization_and_destruction_0`.**
   Barrido: el árbol de inlines completo del original contra el nuestro,
   extrayendo la static-init **de zCamera** del volcado (no la de otra unidad).
   500 líneas contra 500; única diferencia, el nombre y la constancia de los
   parámetros del constructor de la tabla — y ésa no emite RTL (ensayo e1).
2. **`resize()` en la rama nula de `assign`.** El DWARF **confirma** que el
   original llama a `resize(minSize)`, pero en nuestro compilador la rama de
   encogimiento (`while (size() > num) pop_back();`) **no pliega**: el test de
   entrada del `while` abre bloque básico nuevo (dos predecesores) y CSE deja de
   saber que `mSize == 0`. Barridas cuatro formas (if/else, if/else con los
   operandos al revés, sin `else` con el shrink suelto, y `resize` sobre el
   estado c10): las cuatro **empeoran** entre 3 y 8 pp y añaden 24-48 B.
3. **`Contains(srcBeg)` + `make_empty()` en la ruta no nula.** Aparecen en el
   DWARF con rango CERO (líneas 316/317), pero escritos como
   `if (Contains(srcBeg)) make_empty();` **sí emiten código** (1172 B, −2 pp).
   La forma exacta que pliega a cero sigue sin encontrarse.
4. **`bool warned_overflow` de `LoadCameraShakes`, dos posiciones más** (d1: al
   principio con `id`/`chunk` y el bloque anónimo; d3: declarada fuera y asignada
   dentro del `if (group)`). Con las cuatro de la r21 van **seis**. GCC la borra
   por muerta en las seis.
5. **Mover el `bFill`/`bScale` de `TrackCop` detrás del `eMulVector`** (lo que
   proponía el brief): **ilegal por construcción**, no por medida — el
   planificador no cruza la llamada y el objetivo tiene los tres `stfs` delante
   de ella.

## 9. Qué NO he probado

- **`lreg.py`** sobre la ventana de 30 instrucciones de `ICEMover::Update`
  (3.868 B). Sigue sin tocarse, igual que en la r21. Es lo primero que haría la
  próxima ronda: es la mayor bolsa con pistas de DWARF agotadas.
- **`ICEMover::Update`**: `IsGameFlowPaused()` como envoltorio libre en
  `GameFlow.hpp` y `GetParameterLength()` **sin** `const`. Las dos tocan
  cabeceras fuera de zCamera.
- **`TerrainVelocityNoise`** (1.192 B) y **`TrackCarCameraMover::Update`**
  (992 B): **no las he abierto**. Con el hallazgo de §2 (el volcado de `regmap`
  rancio), las dos merecen una pasada de `regmap.py` + `dwbody.py` FRESCOS antes
  que nada: en TrackCop eso destapó el `bScale` en un minuto.
- **El renombrado completo de `ReplayCategory` a `ICEReplayCategory`** con los
  nombres de miembro del DWARF (toca `ICEReplay.cpp` e `ICEManager.cpp`).
- **`_Storage`**: la forma que dé **a la vez** el `mr.` y el `subf` sin plegar
  (ver §3). Y el mecanismo por el que el objetivo mantiene `size()` en un pseudo
  a lo largo del bucle de crecimiento.
- **static-init**: nada más. Ver la veda 1: no hay por dónde sin volcado RTL.

## 10. Notas de convivencia y estado del árbol

Ficheros modificados (todos dentro de zCamera **salvo la cabecera compartida**):

    src/Speed/Indep/Libs/Support/Utility/UTLVector.h        <-- COMPARTIDA (A/B hecho, +0 B)
    src/Speed/Indep/Src/Camera/Movers/TrackCop.cpp
    src/Speed/Indep/Src/Camera/ICE/ICEManager.cpp
    src/Speed/Indep/Src/Camera/ICE/ICEReplay.hpp

**`frozen.py chk Speed/Indep/SourceLists/zCamera` da «HA CAMBIADO»** — es
esperado: `_Storage` sube 1,675 pp. Comprobado restaurando los cuatro ficheros:
con el árbol original vuelve a decir «idéntico al congelado». **No lo he vuelto a
congelar** (no he cerrado ninguna función); decisión tuya.

**Aviso aparte, no es mío:** `frozen.py chk` da «HA CAMBIADO» en **zAI, zAnim,
zEAXSound, zEcstasy, zFEng, zFe2, zPlatform, zSim, zSpeech, zTrack, zWorld y
zWorld2** *con mi cabecera revertida y sus objetos recién construidos*.
Comprobado explícitamente con zAI/zAnim/zSim. Eso venía de antes de esta ronda.

**El disco se llenó a media sesión** (`C: 476G/476G, 29 MB libres`) y eso hizo
fallar siete compilaciones seguidas que se leían como «la variante no cambia
nada» — exactamente la trampa del documento de herramientas. Se liberó borrando
los `.s`/`.json` de la ronda 21 y las cachés `d_*.json` del scratchpad
(quedan 11 GB). Vale la pena mirar `df` cuando una tanda falle en bloque.

`zOnline` falló a compilar tres veces con
`error: Could not open output file .../zOnline.o` (bloqueo de fichero por otro
agente); a la cuarta salió bien y está construido. No tiene ni una función con
símbolo de `Vector`.

Todo lo temporal quedó en el scratchpad con prefijo `c22cam_`.
