# r52 · `rodata2`: zMain cierra su `.rodata` al byte de tamaño; zLua se queda a 800 B

**Ninguna de las dos promociona**, y lo digo con la cifra antes que nada. Lo que
sí ha pasado:

| | antes (r51) | ahora | `.text` | `fncmp` | `permorden` |
|---|---|---|---:|---|---:|
| `zMain` | `rodata−1720 data−32` | **`rodata±0 data−32`** | +0 | 0 de 1.380 | 19 → **15** |
| `zLua` | `rodata−1656 data−320` | **`rodata−800 data−320`** | +0 | 0 de 537 | 1 → **1** |

`trypromo` de las dos, con el `keep.lst` corregido y ampliado de §6:

    zLua   rodata-800 data-320   DOL 642c57c4034d   ROTO
    zMain             data-32    DOL e32cd0fdf10f   ROTO
    las dos juntas                DOL 9e8a480e7441   ROTO

**El `.rodata` de `zMain` mide ya exactamente lo que el objetivo**, y de sus
21.240 B quedan **255 distintos** (eran 2.648 al empezar: 1.820 que faltaban más
828 que sobraban). De esos 255, la inmensa mayoría son **mitades bajas de
punteros de vtable**, o sea consecuencia de las 15 funciones que `permorden`
sigue dando desplazadas, no de `.rodata`. Lo que le queda de dato propio son
~84 B (§4).

Todo lo que he tocado se compila **sólo desde `zMain.cpp` o `zLua.cpp`**: no hay
ni una cabecera compartida en el cambio (§7). `configure.py`, `config/GOWE69/*`,
`splits.txt` y `keep.lst` **sin tocar**.

---

## 1. El método, que es el de zSim generalizado

El brief pedía generalizar `r51-zsim`. Lo que ha funcionado es partirlo en tres
medidas distintas, y **usar la tercera para decidir**:

1. `linkdelta` (tamaño de sección) → dice si sobra o falta, nada más.
2. `dolrod`/difflib sobre la sección **enlazada** → el script de edición con los
   bytes exactos y su dirección en el objetivo.
3. **El censo de cadenas por CONTENIDO en la ventana de `splits.txt`**, objetivo
   contra nuestro enlace, contando ocurrencias. Esto es lo que ha desatascado la
   ronda: difflib alinea globalmente y una cadena movida sale a la vez como
   `FALTA` en un sitio y `SOBRA` en otro, así que **la lista de ediciones
   sobreestima el trabajo por 3×**. El censo dice la verdad en una línea:
   *zMain necesitaba 1.426 B de contenido nuevo y sobraban 434, no 1.820/828.*

En `zMain` el censo pasó de esto:

    FALTAN  x1 Attrib::Gen::rigidbodyspecs, <unknown>, Scheduler ... (1.426 B)
    SOBRAN  x1 MEnterRaceOverFlow, MFlowReadyForOutro, MEnterFreeRoam,
               SuspensionParams, RBComplexParams, SmackableParams,
               ExplosionParams, RBSimpleParams, EngineParams, DamageParams,
               AIParams ... (434 B)

a esto:

    FALTAN  x1 'Scheduler'   (10 B)
    SOBRAN  x1 'r.cpp'       ( 6 B)

Las sondas están en el scratchpad y **borradas** al cerrar; ninguna en
`scripts/`.

## 2. `zMain`: las cinco palancas, en orden de tamaño

### 2.1 Los 956 B de `Attrib::Gen::*` entre `codegen.cpp` y `action.cpp` (+960)

Es la palanca grande y la más barata. `zMain.cpp` define `ATTRIB_TAGS_HAND_POOL`,
o sea `ATTRIB_TAG(s)` → `NULL`, así que el `USE_ATTRIB_ALLOC` de las 45 clases
generadas **no emite ninguna cadena**; el original las tiene todas seguidas en
`803F125D..803F1619`, entre `"Param"` (último literal de `MUnspawnCop.h`, que
sale en `EventDefs.cpp`) y `"NULL_ACTION"` (el primero de `action.cpp`).

Un `asm()` de fichero con los 956 B crudos, bajo `ZMAIN_HAND_POOL`, detrás de
`#include ".../AttribSys/codegen.cpp"`. Sin símbolo → el estripado no lo puede
tocar, y **no desplaza ningún `$LC`** (un `asm()` no consume el contador de
constantes de cc1plus). Medido: `rodata−1720 → −760`.

### 2.2 `WorldConn.h` cambia de sitio, y los dos bloques que lo rodean (+212)

El objetivo tiene en `803EF738..803EF894`, delante de `"MAIEngineRev"`:

    Attrib::Gen::effects, audioimpact, audioscrape        (80 B)
    WorldBodyConn, Pkt_Body_Open, Pkt_Body_Service,
    World_UpdateBody, Pkt_Body_Send, World_OneShotEffect,
    Pkt_Effect_Send, WorldEffectConn, Pkt_Effect_Open,
    Pkt_Effect_Service                                    (176 B)
    Attrib::Gen::speech, pvehicle, engine, engineaudio    (92 B)

Los 176 del medio **ya los emitíamos**, pero 1.900 B más adelante, porque
`WorldConn.h` nos llegaba por `EMissShift.cpp → VehicleRenderConn.h`. Es
exactamente lo que `r51-orden.md` §3.5 dejó diagnosticado y sin aplicar
(*«`WorldConn::Pkt_Body_Send`: nuestro #104, el objetivo #168 → el original lo
parsea entre `class EAddSMS` y `class EAIEngineRev`»*). La `.rodata` lo confirma
al byte.

Arreglo: `#include "Speed/Indep/Src/World/WorldConn.h"` en `zMain.cpp` entre
`EAddSMS.cpp` y `EAIEngineRev.cpp` —la guarda de cabecera convierte el include
tardío en un no-op— con los dos `asm()` de 80 y 92 B a los lados. Los 176 B
dejan de sobrar donde sobraban y aparecen donde faltaban: **coste de tamaño
cero, 176 B de contenido correcto**.

Más `Attrib::Gen::simsurface` + `Attrib::TAttrib` (40 B, entre `E911Call.cpp` y
`EAccelerate.cpp`), `Attrib::Gen::rigidbodyspecs` (28 B, entre
`EJumpToStrategyFlow.cpp` y `EKillJoint.cpp`) y `<unknown>` (12 B, entre
`action.cpp` y `actionqueue.cpp`).

### 2.3 Tres cadenas DUPLICADAS que dejó la r51 (−56)

**Es una regresión de la ronda pasada y conviene que quede escrita.** La r51 §3.4
metió `MEnterRaceOverFlow.h` en `EHideRaceOverMessage.cpp` y
`MEnterFreeRoam.h` + `MFlowReadyForOutro.h` en `EShowRaceOverMessage.cpp` para
colocar el `.text` de sus `BuildMessageTable`. Pero esas tres cadenas **ya se
escribían a mano** en dos andamios de la c36an1:

    EEngineBlown.cpp:11   asm() 20 B  "MEnterRaceOverFlow"
    EShowResults.cpp:14   asm() 36 B  "MEnterFreeRoam" + "MFlowReadyForOutro"

y desde la r51 salían **dos veces**. El objetivo las tiene una vez, y en el sitio
del andamio (`803EFE7C`, delante de `"EEngineBlown"`).

Arreglo: **el include se muda al fichero del andamio y el andamio se borra**.
`EEngineBlown.cpp` y `EShowResults.cpp` están los dos dentro del rango que la
r51 midió, así que `permorden` no empeora —de hecho **baja de 19 a 15**—.

Es el caso de [[nfsmw-andamios-caducan]] con una vuelta de tuerca: el andamio no
caducó solo, **lo caducó el cambio de otra ronda**, y nadie lo comprobó porque
`linkdelta` no se movió (una cadena de más y otra de menos suman cero cuando la
de menos la estripa el enlazador).

### 2.4 Ocho cadenas más duplicadas, y la palanca que las cierra (−86)

Mismo patrón, distinta causa: `AIParams`, `RBComplexParams`, `RBSimpleParams`,
`SuspensionParams`, `EngineParams`, `DamageParams` (andamio de 96 B en
`zMain.cpp`), `SmackableParams` (`EChangeState.cpp`) y `ExplosionParams`
(`ERandomExplosion.cpp`). Aquí el segundo ejemplar **no es dead**: lo referencia
el `TypeName()` de `VehicleBehaviors.h` / `Smackable.h` / `Explosion.h`, que
`zMain` sí emite, así que quitarlo de `keep.lst` sólo se lleva `size & ~7` y deja
las colas (`'rams'`, `'Params'`).

La palanca que lo cierra **sin tocar la cabecera compartida** es la de
`r51-zsim` §3.4: **cambiar el `asm()` por una `static inline` muerta que
devuelve las mismas cadenas.** cc1plus interna el literal AL PARSEARLA —en el
punto exacto donde estaba el andamio— y el uso de más abajo **reutiliza el mismo
`$LC`**, porque el literal es idéntico. Cero instrucciones, cero bytes de
`.text`, y la cadena deja de estar duplicada:

```c
#ifdef ZMAIN_HAND_POOL
static inline const char *zmain_pool_params(int i) {
    switch (i) {
    case 0: return "AIParams";
    ...
    case 5: return "DamageParams";
    }
    return 0;
}
#endif
```

El relleno sale igual que el del `asm()` porque cc1plus alinea cada `$LC` a 4 y
el andamio ya estaba escrito con esa regla (9→12, 16, 15→16, 17→20, 13→16,
13→16 = 96 B).

**Con esto la `.rodata` de `zMain` mide exactamente lo que el objetivo.**

### 2.5 La tabla, paso a paso

| paso | `linkdelta` de `zMain` | `fncmp` |
|---|---|---|
| partida (r51) | `rodata−1720 data−32` | 0/1380 |
| + 56 entradas de `keep.lst` (§6) | `rodata−1016 data−32` | 0/1380 |
| + los 956 B de `Attrib::Gen::*` (§2.1) | `rodata−56 data−32` | 0/1380 |
| + `WorldConn.h` y los bloques A/B (§2.2) | `rodata+152 data−32` | 0/1380 |
| + `rigidbodyspecs` y `<unknown>` (§2.2) | `rodata+192 data−32` | 0/1380 |
| + las tres duplicadas de la r51 (§2.3) | `rodata+128 data−32` | 0/1380 |
| + las ocho `static inline` (§2.4) | **`data−32`** | **0/1380** |

## 3. `zLua`: −1656 → −800, y el `.text+196` acotado

### 3.1 Las dos entradas de `keep.lst` que resucitan 196 B de `.text`

La r50 avisaba de que en `zLua` una entrada devolvía 196 B de `.text` y lo
atribuía a una vtable. **No es una vtable: son dos cadenas de corrutina**, y
están medidas una a una (61 enlaces, uno por entrada candidata):

    zLua "cannot resume dead coroutine"           ($LC783) -> text+196 rodata+24
    zLua "cannot resume non-suspended coroutine"  ($LC784) -> text+196 rodata+32

Las dos resucitan **el mismo** bloque de 196 B. Las otras 79 entradas cuestan
`.text +0`. **Están excluidas del paquete de §6** y hay que dejarlas fuera: 56 B
de `.rodata` no valen 196 B de `.text` cuando el `.text` está a `+0`.

### 3.2 `ZMAIN_MESSAGES_LUA_INLINE` (la palanca 6.1 de la r50, aplicada)

`zLua` no la definía. El objetivo tiene, justo detrás de sus ocho `M*.h` y en el
mismo orden que la r51 fijó, **los nombres de parámetro** de cada mensaje
(`NISName`, `TrapActivity`, `Racer`, `SpeedKmh`, `Placing`, `MinHeatLevel`,
`CopsEnabled`, `RepPoints`, `Licenced`, `PathEvent`, `PathControl`,
`SpeedDefault`, `SpeedHighway`, `FixSpeed`, `CarID`, `CarState`,
`EAX_CarState`, `PatterToPlay`, `ChallengeName`, `Sender`, `Position`,
`Vector`, `Velocity`, `AttribKey`, `hSimable`…), que sólo nacen si se parsean los
cuerpos de `BuildMessageTable` de las cabeceras. Con la guarda, las entradas
candidatas de `deadlink` pasan de 31 a 61 y el déficit de **−1024**.

### 3.3 El bloque c34dat2 entre `LuaAttributes.cpp` y `LuaGameHooks.cpp` (+224)

El objetivo tiene en `803EE2D0`, justo detrás de `"__eq"` (el último literal de
`LuaAttributes.cpp`) y justo delante del bloque de `LuaGameHooks.cpp`:
`Attrib::Gen::ecar`, `camerainfo`, `effects`, `audioimpact`, `audioscrape` y las
diez cadenas de `WorldConn`. Es la receta c34dat2 que ya usan `zFe`, `zFe2`,
`zPhysics` y `zSpeech`: seis `#include` en `zLua.cpp`. `permorden` sigue en 1 y
`fncmp` en 0 de 537.

**Negativo con su cifra**: de las cinco clases de AttribSys sólo entran dos
(`ecar` y `camerainfo`, 42 B). `effects`, `audioimpact` y `audioscrape` (71 B) ya
estaban parseadas antes por otra cadena de includes, así que el `#include` es un
no-op y el literal no nace. Para esas tres hace falta `asm()` crudo o cortar la
cadena de arriba; no lo he perseguido.

### 3.4 La tabla

| paso | `linkdelta` de `zLua` | `.text` | `fncmp` |
|---|---|---:|---|
| partida (r51) | `rodata−1656 data−320` | +0 | 0/537 |
| + 29 entradas de `keep.lst` (sin las dos de §3.1) | `rodata−1328 data−320` | +0 | 0/537 |
| + `ZMAIN_MESSAGES_LUA_INLINE` y sus keeps | `rodata−1024 data−320` | +0 | 0/537 |
| + el bloque c34dat2 (§3.3) y sus keeps | **`rodata−800 data−320`** | **+0** | **0/537** |

### 3.5 Los 8 B que la r51 se dejó: **están de vuelta, y lo he medido en el sitio**

`r51-orden.md` §6 dejó apuntado que `zLua` perdía 8 B de `.rodata` en relleno de
alineación en la frontera cadenas/floats, bisecado a
`…"MomentStrm\0"` + 5 ceros + `3e4ccccd`. Con las cadenas de §3.2 y §3.3 dentro,
esa frontera sale **byte a byte igual** que el objetivo:

    OBJ  ..Challenge 00 00 00 | MomentStrm 00 00 00 00 00 00 | LANGUAGE_ 00 00 00 | 3e4ccccd 00000000 3f800000
    NUE  ..Challenge 00 00 00 | MomentStrm 00 00 00 00 00 00 | LANGUAGE_ 00 00 00 | 3e4ccccd 00000000 3f800000

Los cinco ceros de relleno están. **No hay nada que revertir de la r51**: el
relleno se re-decidió solo en cuanto entraron las cadenas, que es lo que su
autor predijo.

## 4. Lo que le queda a cada una, medido

**`zMain`** — `.rodata` del tamaño exacto, 255 B distintos de 21.240:

* ~170 B son **mitades bajas de punteros de vtable** (`_vt.24Schedule_*`,
  `_vt.12EEngineBlown`, `_vt.9ECellCall`, `_vt.Q29WorldConn13Pkt_Body_Send`,
  `_vt.Q23UTLt6Vector2ZP11ActionQueuei16`…) y de `device_infos`. Son
  consecuencia de las **15 desplazadas de `permorden`**, no trabajo de `.rodata`.
* ~84 B de dato propio, todo diagnosticado:
  - `"EventSequencerSystems"` sale **delante** de `"EventBuffer"` y el objetivo
    lo tiene detrás (`803F2303`); 28 B que sobran y 24 que faltan. Es lo mismo
    que `r51-orden.md` §6 apuntó con el offset `0x2978` contra `0x2D84`.
  - `"Scheduler"` (10 B) contra nuestro `"…r.cpp"` (6 B): el objetivo usa el tag
    corto donde nosotros pasamos un `__FILE__`.
  - tres huecos de 4-5 B de constantes de coma flotante (`803EFF72`,
    `803F060D`, `803F0615`).
* **`.data −32`**, que es lo que de verdad le bloquea y **no es `.rodata`**: el
  objetivo tiene 4 B más en `prev_mode.30816`, 4 B más en
  `_6Attrib.kTypeHandlerCount`, 4 en `previousVelocity.39053`, 2 en
  `_9Scheduler.fgScheduler` y 18 de relleno en `gap_06_8041E50C_data`; y todas
  las entradas de `_6Attrib.kTypeHandlers` apuntan **0x20 más arriba**. Huele a
  un manejador de tipo de más en `codegen.cpp`. **No lo he tocado.**

**`zLua`** — `rodata−800`, 810 B de cadenas que el objetivo tiene y nosotros no,
74 B que sobran (colas de `size & 7` del estripado, todas de 4-7 B). Lo que falta,
por familias:

    los tags de pool de LuaRuntime      "LuaRuntime memory pool", "LuaRuntime",
    (112 B, asm crudo: no existen        "Lua emergency allocation", "LuaRuntime temp
     en nuestro arbol)                   compression buffer", "LuaRuntime VM reset snapshot"
    los nombres de tipo de lmem.h       NIL BOOL LUD NUMBER STRING TABLE FUNC UD THREAD (?)
    (94 B, idem)                        CHAR INT STATE GLOBALSTATE TOBJECT LOCVAR TSTRINGPTR
                                        GCOBJECTPTR INSTRUCTION CALLINFO CLOSURE UPVAL PROTO
                                        PROTOPTR UDATA NODE INVALID
    los nombres de fichero              "LuaPostOffice" "LuaBindery" "LuaAttributes" (39 B)
    Attrib::Gen::* que no nacen         effects, audioimpact, audioscrape, milestonetypes,
    (140 B)                             speechtune, world
    el bloque Car/Heli sound conn       CarSoundConn, Pkt_Car_Open, Pkt_Car_Service,
    (87 B)                              HeliSoundConn, Pkt_Heli_Open, Pkt_Heli_Service
    las dos de corrutina (67 B)         inalcanzables por keep.lst (§3.1)
    resto                               MAudioReflection, MLoadingComplete, SmackableParams,
                                        MEnterFreeRoam, VehicleParams, PlayerNum, Covered,
                                        MGeneric, Dist, High/Medium/Low/Reflection, 16.1.0...

y `.data −320`, que **no he investigado**: el diff de esa ventana es todo
punteros a `.rodata` desplazados (`accessorTable.13348`), o sea se resolverá
cuando cierre `.rodata`, pero el **tamaño** de −320 sigue sin explicar.

## 5. Negativos, con su cifra

* **Ninguna de las dos llega a `DOL OK`.** `zLua` `642c57c4034d`, `zMain`
  `e32cd0fdf10f`, juntas `9e8a480e7441`.
* **`zMain` no promociona aunque su `.rodata` mida ya lo justo**: le faltan las
  15 de `permorden` y los −32 B de `.data`.
* **`prefijotu.py` sobreestima en `zMain`**: decía 46 cadenas / 1.116 B y el
  déficit real de contenido eran 1.426 B, porque no ve las que emitimos DOS
  veces ni las que emitimos fuera de sitio. Lee el objeto; el censo de la ventana
  ENLAZADA es el que manda.
* **La lista de ediciones de difflib sobreestima por 3×** (1.820+828 contra
  1.426+434 reales). Una cadena movida sale dos veces.
* **`vtord` sigue sin servir para decidir** (r51 §8.2): no lo he usado.
* En `zLua`, tres de los cinco `#include` de AttribSys de §3.3 son no-ops.

## 6. Lo que hace falta de fuera: `keep.lst`

**1 · Las 28 correcciones que `lcfix` hace solo.** Mis cambios de fuente
desplazan los `$LC` de las dos unidades. `python scripts/lcfix.py zLua zMain`
las aplica; son exactamente éstas (comprobado con `--check`):

    linea 1368: zLua.o:$LC466 -> zLua.o:$LC482      linea 1452: zMain.o:$LC520 -> zMain.o:$LC433
    linea 1370: zLua.o:$LC467 -> zLua.o:$LC483      linea 1454: zMain.o:$LC521 -> zMain.o:$LC434
    linea 1372: zLua.o:$LC468 -> zLua.o:$LC484      linea 1456: zMain.o:$LC522 -> zMain.o:$LC435
    linea 1374: zLua.o:$LC469 -> zLua.o:$LC485      linea 1458: zMain.o:$LC523 -> zMain.o:$LC436
    linea 1376: zLua.o:$LC558 -> zLua.o:$LC574      linea 1460: zMain.o:$LC524 -> zMain.o:$LC437
    linea 1378: zLua.o:$LC559 -> zLua.o:$LC575      linea 1462: zMain.o:$LC525 -> zMain.o:$LC438
    linea 1380: zLua.o:$LC560 -> zLua.o:$LC576      linea 1464: zMain.o:$LC541 -> zMain.o:$LC230
    linea 1382: zLua.o:$LC561 -> zLua.o:$LC577      linea 1466: zMain.o:$LC542 -> zMain.o:$LC231
    linea 1384: zLua.o:$LC568 -> zLua.o:$LC584      linea 1468: zMain.o:$LC543 -> zMain.o:$LC232
    linea 1386: zLua.o:$LC615 -> zLua.o:$LC664      linea 1470: zMain.o:$LC546 -> zMain.o:$LC235
    linea 1388: zLua.o:$LC628 -> zLua.o:$LC677      linea 1472: zMain.o:$LC547 -> zMain.o:$LC236
    linea 1390: zLua.o:$LC629 -> zLua.o:$LC678      linea 1474: zMain.o:$LC548 -> zMain.o:$LC237
    linea 1392: zLua.o:$LC633 -> zLua.o:$LC682      linea 1476: zMain.o:$LC549 -> zMain.o:$LC238
    linea 1394: zLua.o:$LC755 -> zLua.o:$LC804      linea 1478: zMain.o:$LC550 -> zMain.o:$LC239

Las de `zPhysics` que `lcfix --check` también saca **no son mías** (`rodata1`).

**2 · 150 entradas nuevas.** El procedimiento, en este orden exacto:

    python scripts/lcfix.py zLua zMain            # primero: si no, deadlink no ve
                                                  # que entradas ya estan cubiertas
    python scripts/deadlink.py zMain --keep >> config/GOWE69/keep.lst   # 71 entradas
    python scripts/deadlink.py zLua  --keep >> config/GOWE69/keep.lst   # 81 entradas
    # y BORRAR de ese ultimo bloque las DOS entradas de §3.1:
    #   # @lc zLua "cannot resume dead coroutine"
    #   zLua.o:$LC783
    #   # @lc zLua "cannot resume non-suspended coroutine"
    #   zLua.o:$LC784
    python scripts/lcfix.py --check               # tiene que salir limpio (salvo zPhysics)

Los `.o` del árbol están recompilados y al día, así que `deadlink` da esas cifras
tal cual. El fichero resultante es el que he medido; sin él, el árbol tal como lo
dejo da `zMain rodata−880` y `zLua rodata−1624` —mejor que la partida, pero
845 B por debajo de lo que puede dar—.

**3 · La promoción**: **no la pidáis todavía**, ninguna da `DOL OK`.

## 7. Ficheros tocados

| fichero | qué | quién lo compila |
|---|---|---|
| `SourceLists/zMain.cpp` | los cuatro `asm()` nuevos (956+80+92+40+28+12 B), el `#include WorldConn.h`, y el andamio de 96 B convertido en `static inline` | sólo zMain |
| `Generated/Events/EEngineBlown.cpp` | fuera el andamio de 20 B, dentro `#include MEnterRaceOverFlow.h` | sólo zMain |
| `Generated/Events/EHideRaceOverMessage.cpp` | fuera el `#include` de la r51 | sólo zMain |
| `Generated/Events/EShowResults.cpp` | fuera el andamio de 36 B, dentro los dos `#include` | sólo zMain |
| `Generated/Events/EShowRaceOverMessage.cpp` | fuera los dos `#include` de la r51 | sólo zMain |
| `Generated/Events/EChangeState.cpp` | andamio de 24 B → `static inline` | sólo zMain |
| `Generated/Events/ERandomExplosion.cpp` | andamio de 16 B → `static inline` | sólo zMain |
| `SourceLists/zLua.cpp` | `#define ZMAIN_MESSAGES_LUA_INLINE` y los seis `#include` de §3.3 | sólo zLua |

**Cero cabeceras compartidas**, comprobado uno a uno con `grep -rl` sobre
`src/Speed/Indep/SourceLists/`: los seis `Generated/Events/*.cpp` los incluye
únicamente `zMain.cpp`. Ninguna unidad ajena cambia, así que no hace falta la
medida de inercia de `r51-orden.md` §4.

Nada de `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`. Ninguna
herramienta nueva en `scripts/`; las sondas (`probe.py`, `rod.py`, `edits.py`,
`censo.py`, `strwin.py`, `mkkeep.py`, `find2.py`, `gen.py`, `dmp.py`) se
quedaron en el scratchpad y están borradas con sus volcados.

## 8. Para la ronda siguiente

1. **`zMain` está a `.data−32` y 15 desplazadas de promocionar.** Los −32 B de
   `.data` son un manejador de tipo de `codegen.cpp` (§4) y valen la pena antes
   que nada: `.rodata` ya no le estorba.
2. **Buscad andamios de la c36an1 duplicados en las demás unidades.** El patrón
   de §2.3/§2.4 —`asm()` a mano *más* el `$LC` que cc1plus emite ahora— no lo ve
   ninguna herramienta actual, porque `linkdelta` no se mueve. `dupstr.py` lo ve
   sólo cuando la copia de más está viva. **La prueba barata es el censo de
   cadenas por contenido de §1**, y merece un `scripts/censo_cadenas.py` con
   docstring: en `zMain` valió 142 B y tres regresiones de la ronda pasada.
3. **La `static inline` muerta sustituye a cualquier andamio de `asm()` que el
   compilador ya sabe emitir**, y es estrictamente mejor: no duplica, no hay que
   escribir el relleno a mano y `lcfix` la cubre. En `zMain` cerró tres andamios
   de tres rondas distintas.
4. En `zLua` quedan 810 B en seis familias (§4) y ninguna necesita tocar código:
   son `asm()` crudos y `#include` colocados. Es trabajo mecánico de una tarde.
5. **`ZMAIN_MESSAGES_LUA_INLINE` sigue sin encender en once unidades**
   (`zCamera`, `zEAXSound2`, `zEcstasy`, `zTrack`, `zWorld`, `zWorld2`,
   `zPlatform`, `zPhysicsBehaviors`, `zEagl4Anim`, `zFeOverlay`, `zGameModes`).
   En `zLua` valió 304 B con los keeps.
