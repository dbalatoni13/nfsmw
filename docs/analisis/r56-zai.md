# r56 — `zAI`: la cola de `finish_file` tiene DOS colas, y una de las dos es mía

Unidad: `Speed/Indep/SourceLists/zAI` (272.796 B de `.text`). Ficheros tocados: **`zAI.cpp`** y
**`Gps.cpp`** (este último sólo comentarios). Ninguna cabecera. Ninguna unidad ajena.

## Resumen en una línea

**El `.text` de `zAI` pasa de 45.164 B descolocados a 26.532: +18.632 B puestos en su sitio en
el ENLACE, del 83,4 % al 90,3 %, con CERO regresiones y sin mover un byte de `.rodata`.** Tres
ediciones, todas en `zAI.cpp`, todas de una línea o de mover un bloque. No promociona: sigue
`DOL ROTO`, y le quedan `.rodata −560` y `.data −384`.

---

## 0. Lo que hay que saber antes de volver a esta unidad

La cola de `finish_file` **no es una cola**: son dos, con **reglas de orden distintas**, y
separadas por un mojón que se ve a simple vista. Leído en el fuente del compilador
(`orig/prodg/NGC_GNU_SRC/NGC/gcc/cp/decl2.c:3643` `finish_file`), el bucle `do/while` drena por
turnos y **en este orden**:

    1. instantiate_pending_templates()      <- REGION A
    2. vtables (walk_globals vtable_decl_p) <- va a .rodata
    3. static_aggregates -> SSDF            <- __static_initialization_and_destruction_0
    4. synthesize_method de los artificiales
    5. wrapup_global_declarations(saved_inlines)  <- REGION B
    6. walk_namespaces(wrapup_globals_for_namespace)
    7. pending_statics

En el `.text` enlazado de `zAI` eso se ve literalmente:

| | rango (objetivo) | tamaño | qué es | qué manda el orden |
|---|---|---:|---|---|
| **zona de parseo** | `0x800034A0`–`0x8003A9F4` | 226.644 B | código normal | ya está al 100 %, **un solo tramo a d=+0** |
| **región A** | `0x8003A9F4`–`0x8003F5DC` | 19.432 B | `find<T>`, `clear<T>`, `reserve<T>`, `lower_bound<T>`, `CreateInstance<T>` | **FIFO de `add_pending_template`** (`pt.c:3559`): el punto de la PRIMERA petición |
| **SIAD** | `0x8003F5DC` | 9.380 B | `__static_initialization_and_destruction_0` | paso 3, va solo |
| **región B** | `0x80041A80`–final | 17.340 B | `_IHandle__X`, `_._X`, `ClassKey__X`, `_GetKind__X`, accesores en clase | **orden de empuje en `saved_inlines`**, o sea el orden en que se PARSEA el cuerpo (para un miembro en clase: cuándo se completa la clase = **orden de `#include`**) |

**La consecuencia práctica, y es la que paga**: la región A se controla **escribiendo la
petición donde toca** (una instanciación explícita ES una petición y se puede escribir entre
dos `#include` de `zAI.cpp`), y la región B se controla **con el orden de los `#include`**.
La primera es mía. La segunda vive casi entera en `Interfaces/`, y no la poseo (§5).

---

## 1. Las tres ediciones

### (1) `zAI.cpp:1` — retirar `ATTRIB_NO_INLINE_CLASSKEY` → **+9.380 B**

La guarda apagaba la definición `inline` de `Gen::<clase>::ClassKey()` de las cabeceras
generadas y la suministraba `Gps.cpp:440` **fuera de línea**. Un cuerpo fuera de línea sale
**en el punto del parseo**, y `Gps.cpp` es el último `#include`: los tres `ClassKey` salían
pegados al final de la zona de parseo, o sea en la **cabeza de la cola**. El objetivo los tiene
en `0x80041E14` y `0x80042574`, **dentro de la región B**, entre los `_IHandle__*`.

Con la definición inline de la cabecera los tres se difieren a `saved_inlines` y salen donde el
objetivo los pone. **`fncmp` da 0 regresiones y los tres símbolos se siguen emitiendo**: GCC no
inlina las llamadas, así que el `bl ClassKey__` del objetivo se conserva.

Los 9.380 B ganados son, **al byte**, `__static_initialization_and_destruction_0` pasando de
`d=+36` a `d=+0`: los 36 B de los tres `ClassKey` que sobraban por delante. Y sus posiciones
pasan de `−29.728` / `−31.592` a **`+1.340` / `+1.316`**.

### (2) `zAI.cpp:40` — el bloque de instanciaciones explícitas, del final al hueco tras `AITrafficManager.cpp` → **+3.244 B**

Los cuatro `template … _STL::find<T**,T*>` y el `Factory<AudioEventParams…>::CreateInstance`
estaban escritos **al final del fichero**, así que sus cinco peticiones entraban las últimas en
`pending_templates` y los cinco símbolos salían de **+2.800 a +18.112 B** por detrás de su
sitio. Subidos al hueco entre `AITrafficManager.cpp` y `AICopManager.cpp`:

| símbolo | antes | después |
|---|---:|---:|
| `CreateInstance<const AudioEventParams&, AudioEvent, Ui>` | +12.220 | **−456** |
| `find<EAX_HeliState**>` | +11.600 | **−1.076** |
| `find<Sim::IEntity**>` | +18.112 | +5.612 |
| `find<ISimpleBody**>` | +17.224 | +4.548 |

Lo que paga no son ellos: es que al quitar 796 B de delante, el bloque
`find<ISimable**>`+SIAD (3.244 B) cae a `d=+0`.

**`.rodata` no se mueve**: una instanciación explícita no emite literales, así que **no renumera
los `$LC`**. Esto es lo que la distingue de mover un `#include` (§3).

### (3) `zAI.cpp:135` — `find<IDebugWatchCar**>` detrás de `AIGoal.cpp` → **+6.008 B**

De los cinco de (2), éste **no va con los otros**: el objetivo lo emite en `0x8003E880`, detrás
del bloque de `CreateInstance<ISimable*,AIGoal,UCrc32>` y delante del de `find<ISimable**>`.
Escrita la petición entre `#include AIGoal.cpp` y `#include AIPursuit.cpp`, los 176 B que
sobraban por delante desaparecen y **`CreateInstance<AIGoal>` (5.776 B) cae a `d=+0`**, y con él
`get_sample<time_delay_filter>` (800 B).

Le quedan **568 B**: el objetivo lo pone delante de `find<ISimable*const*>`, `find<ISimable**>`,
`clear<List_base<AIAction*>>` y `CreateInstance<AIActionParams*,AIAction>`, y nosotros detrás.
Vale 744 B más y hay que escribirlo **dentro** de `AIGoal.cpp`, no entre los `#include`.

---

## 2. La medida

### `fncmp` — ANTES y DESPUÉS, idénticos

```
ANTES     0 de 1030 funciones con el CODIGO distinto -- 0 B
          337 mas solo con nombres de simbolo distintos (199388 B)
DESPUES   0 de 1030 funciones con el CODIGO distinto -- 0 B
          337 mas solo con nombres de simbolo distintos (199388 B)
```

Cero regresiones, en las cinco pasadas intermedias y en la final.

### El censo de desplazamiento ENLAZADO, símbolo a símbolo

Enlace base (todos los `obj/`) contra enlace con nuestro `.o` sustituido, desplazamiento de cada
uno de los 1.030 símbolos de `.text` del objetivo. **`dolwhere` no arranca en `zAI`** (aborta con
`LAS SECCIONES NO COINCIDEN` porque `.rodata`/`.data` difieren de tamaño), así que hay que medirlo
a mano; la sonda queda en el scratchpad (§7).

| estado | EN SITIO | FUERA | % | Δ |
|---|---:|---:|---:|---:|
| base de la ronda | 227.632 B | 45.164 B | 83,4 % | — |
| + (1) ClassKey | 237.012 | 35.784 | 86,9 % | **+9.380** |
| + (2) bloque explícito | 240.256 | 32.540 | 88,1 % | +3.244 |
| + (3) IDebugWatchCar | **246.264** | **26.532** | **90,3 %** | +6.008 |
| **total** | | | | **+18.632 B** |

Transiciones: 140 → **139**, pero eso no es la métrica: lo que cuenta es que ahora hay **cuatro
tramos grandes a `d=+0` dentro de la cola** (5.776 + 12.056 + 800 + los 226.644 de la zona de
parseo) donde antes no había ninguno.

### Sellos y verdictos

```
sha1 del .o    d215c8b0ee5bcbed9d9771d174925e28e52b4846  (TRES compilaciones, idéntico)
linkdelta      Speed/Indep/SourceLists/zAI   .text +0   rodata-560 data-384
permorden      en su sitio 868 de 1030 (era 866);  ciclos 10  (174,131,59,24,21,5,3,3,3,3)
trypromo       Speed/Indep/SourceLists/zAI   DOL ROTO (d755955573f5)
```

---

## 3. Lo medido y REVERTIDO (dos negativos que ahorran una tarde)

**a) `inline` en las tres definiciones de `Gps.cpp` en vez de retirar la guarda.** Misma idea,
sin tocar `ATTRIB_NO_INLINE_CLASSKEY`. Funciona —los tres se difieren— pero los deja **al final
de la región B**, pegados al bloque de cola: `d=+15.804`. Vale **+8.976 B**, o sea **404 B menos**
que (1). Revertida.

**b) `trafficpattern.h` movido detrás de `ITrafficMgr.h`.** El objetivo emite
`__as__/ClassKey__` de `trafficpattern` en `0x80042544`/`0x80042574`, detrás de
`_IHandle__11ITrafficMgr`; nosotros los sacamos los cuartos de la región B. Dos ensayos:

* moverlo **en `AITrafficManager.cpp`**: **CERO**, ni un byte. La cabecera ya entraba por
  `AITrafficManager.hpp:10` y el `#include` del `.cpp` era redundante. Revertido.
* moverlo **en `AITrafficManager.hpp`** (el sitio de verdad): `.text` **+12 B** y `.rodata`
  **−160 B**. Revertido, y es la trampa de la ronda:

> **Mover un `#include` en `zAI` renumera los `$LC` y cuesta más `.rodata` de lo que gana en
> `.text`.** El pool de literales sigue el orden de parseo, `keep.lst` los nombra por número, y
> `lcfix.py` está vedado durante la ronda (reescribe una entrada del ENLACE bajo los pies de los
> otros seis agentes). **El frente de orden por `#include` de `zAI` está bloqueado por
> `keep.lst`, no por la fuente.** Las instanciaciones explícitas de §1 no tienen ese problema
> porque no emiten literales: por eso son la palanca buena mientras haya agentes midiendo.

---

## 4. Sorpresas

1. **La base se movió debajo de mí a mitad de ronda, y `.rodata` con ella.** Con `zAI.cpp` y
   `Gps.cpp` PRISTINOS y reconstruidos, `linkdelta` daba `rodata-240` a las 01:10 y `rodata-560`
   a las 01:40. El `.o` cambia de sha1 sin que yo toque nada porque cambian cabeceras
   compartidas: `src/Speed/Indep/Src/Physics/PhysicsObject.h` (01:32) y `Physics/Smackable.h`
   (01:43), las dos de otro agente. **Ninguna medida de `.rodata` de `zAI` tomada antes de las
   01:30 vale**, la de `r55-zai.md` incluida. Las tres ediciones de esta ronda dejan `.rodata`
   exactamente donde la encontraron: **−560 antes y −560 después**.
2. **Y ese cambio ajeno le cuesta a `zAI` 320 B de `.rodata` VIVA.** `lcfix.py --check` da hoy
   **30 entradas de `zAI` desfasadas**, todas un desplazamiento de −3 a partir de
   `zAI.o:$LC329 -> $LC326`. Son 30 literales que `keep.lst` nombra por el número equivocado, así
   que el enlazador estripa los que debía conservar. **No es trabajo de fuente: es una pasada de
   `lcfix.py` en la ventana.** Y es un aviso general: **una edición de cabecera compartida en la
   unidad A puede costarle `.rodata` a la unidad B sin que nadie se entere.**
3. **El aviso del encargo sobre `build_direct.py` no se reprodujo.** Vi una vez la misma fuente
   dando dos sha1 (`7b3c1800` y `6bf80ee7`) y **no era el compilador**: era la deriva de la
   sorpresa 1. Con el árbol quieto, tres compilaciones seguidas dan el mismo sha1 al byte. La
   regla del sello sigue siendo buena, pero **antes de acusar al build hay que mirar los mtime de
   las cabeceras compartidas**.
4. **`permorden` sigue sin medir esto.** Subió de 866 a 868 símbolos mientras los bytes en su
   sitio subían 18.632: cuenta SÍMBOLOS, y una subsecuencia creciente es ciega al tamaño. Lo
   mismo con las transiciones: 140 → 139 para +18.632 B.
5. **La primera petición de una plantilla no está donde uno cree.** `find<IDebugWatchCar**>`
   escrito ANTES de `#include AIPursuit.cpp` sale DETRÁS de cuatro plantillas que el objetivo
   emite después de él. O sea que esas cuatro se piden todavía dentro de `AIGoal.cpp`: el hueco
   entre dos `#include` es una posición **gruesa**, y los últimos cientos de bytes hay que
   ganarlos escribiendo la petición dentro del `.cpp`.

---

## 5. Lo que le queda a `zAI`, y de quién es

### a) Región A — 14.476 B, y casi todo es mío

Lo que queda descolocado, por bloques:

```
 +6.760  find<IPlayer**>            +6.852  reserve<vector<int, Speech::_type_voiceIDs>>
 +6.220  reserve<Speech::copPair>   +5.612  find<Sim::IEntity**>
 +4.548  find<ISimpleBody**>        -9.980  find<IExplosion**>
 -4.812  clear<List_base<char*>>    -3.708  find<IModel**>
 -2.724  reserve<WCollisionTri>+<WCollisionTriBlock*>   -864  clear<TrafficList> (2.560 B)
```

Los dos primeros grupos son **peticiones de cabecera**, no explícitas: `find<IPlayer**>`,
`reserve<voiceIDs>` y `reserve<copPair>` se piden hoy al empezar las cabeceras de
`AICopManager.cpp` y el objetivo las pide en las de `AITrafficManager.cpp`. `find<IExplosion**>`
es el caso simétrico: lo pedimos **el segundo de toda la región A** y el objetivo lo pide justo
antes del bloque de `AIGoal`. Los tres se arreglan con el orden de `#include` de
`AITrafficManager.cpp`/`AICopManager.cpp`, **que son míos**, pero pagan el peaje de `$LC` (§3).

Dos pasos **libres de peaje** y sin cabeceras:

* `find<Sim::IEntity**>` y `find<ISimpleBody**>` bajados **dentro** de `AITrafficManager.cpp`,
  detrás de su bloque de `#include`: los deja a ~+1,3 kB y ~+0 en vez de +5.612 y +4.548.
* `find<IDebugWatchCar**>` **dentro** de `AIGoal.cpp` en vez de entre los `#include`: 744 B.

### b) Región B — 12.056 B, y **NO es mía**

El orden de la región B es el orden en que se completan las clases, o sea el de `#include`, y el
recorrido lo dice sin ambigüedad. Ordenando por el ELF objetivo:

```
  OBJETIVO   VU0_v3unit, ~IUnknown+bVector3, IAttachable, ISimable, push_back<_KeyedNode>,
             Sim::IActivity, ~SAP::Grid<AIAvoidable>, ClassKey pursuitlevels, ClassKey
             pursuitescalation, IVehicleAI, IHumanAI, IPerpetrator, IRacer, IPursuit,
             IRoadBlock, IPursuitAI, ITrafficAI, ICollisionBody, IBody, IRBVehicle,
             IRigidBody, ITrafficMgr, __as trafficpattern, ClassKey trafficpattern, Behavior…

  NUESTRO    ~IUnknown+bVector3, VU0_v3unit, __as trafficpattern, IAttachable, ICause,
             push_back<_KeyedNode>, ISimable, …
```

Tres lecturas, cada una con su fichero y su dueño:

| lo que dice el objetivo | el fichero | vale |
|---|---|---:|
| `ICause` va **mucho después** de `ISimable` → **el `ISimable.h` original NO incluía `ICause.h`** antes de la clase | `Interfaces/Simables/ISimable.h:8` | **624 B** en la cabeza de la región B (`_IHandle__6ICause`+`_._6ICause` 280 B y `push_back<_KeyedNode>` 344 B, que hoy salen delante de `ISimable`) |
| las clases de `IAI.h` van en el orden **IVehicleAI, IHumanAI, IPerpetrator, IRacer, IPursuit, IRoadBlock, IPursuitAI, ITrafficAI**; hoy están **IRoadBlock(31), IVehicleAI(62), ITrafficAI(124), IHumanAI(131), IPursuit(182), IPursuitAI(268), IPerpetrator(309), IRacer(357)** | `Interfaces/Simables/IAI.h` | los siete tramos `_IHandle__*`/`_._*` de +372 a +2.348, ~3,5 kB |
| `pursuitlevels.h` antes que `pursuitescalation.h`, y **`ITrafficMgr.h` DESPUÉS de `IAI.h`** | `Interfaces/Simables/IAI.h:6-11`, `AITrafficManager.hpp:14` | 24 B los `ClassKey`, y `ITrafficMgr` |
| `VU0_v3unit` antes del destructor de `UTL::COM::IUnknown` → **UMath antes que UCOM** | `Libs/Support/Utility/UCOM.h` vs `UMath.h` | 180 B, un intercambio adyacente limpio |

**Ninguno de esos ficheros es de `AI/`**, y `ISimable.h`/`IAI.h`/`UCOM.h` los incluyen media
docena de unidades: cambiarlos con seis agentes midiendo es exactamente lo que veta la regla 4.
Van como **propuesta con la medida**, no aplicados, y con el precedente que ya existe en el
árbol: `ISimable.h:13` lleva un `#ifdef EVENTSEQ_FWD_IENGINE` de la r51 que hace justo esto —una
guarda inerte en la cabecera, encendida desde el `SourceList`—.

### c) `.rodata −560` y `.data −384`

* **`.rodata`**: 320 de los 560 son la sorpresa 2 —30 entradas `$LC` desfasadas por un cambio
  ajeno—, y se pagan con `lcfix.py` en la ventana. Los otros 240 son el orden del pool más
  `_vt.14AIGoalPullOver` (48 B de vtable que el objetivo no tiene), como midió la r55.
* **`.data −384`**: **el encargo dice que "sí son contenido que falta" y eso está REFUTADO en el
  árbol.** `pad_06_80415180_data` (172 B), `lbl_80415438` (68 B), `gap_06_80415230_data` (44 B) y
  los once bloques restantes son nombres del troceador; la nota de la r52
  (`nfsmw-huecos-leer-el-dol`) midió esta misma unidad y encontró **0 B de dato con nombre sin
  escribir**: es relleno de alineación, que **no se escribe, se consigue** cerrando antes la
  `.rodata`. Nuestro `.data` mide 444 B contra 832, pero la diferencia es hueco anónimo.

---

## 6. Siguiente paso, en orden de rentabilidad

1. **`ISimable.h`: `ICause.h` fuera** (guarda tipo `EVENTSEQ_FWD_IENGINE`, encendida desde
   `zAI.cpp`). 624 B, y es la cabeza de la región B: todo lo que va detrás está a `+300…+2.400`,
   así que puede arrastrar mucho más. **Hay que dárselo a quien posea `Interfaces/`.**
2. **`IAI.h`: el orden de las ocho clases** y `pursuitlevels.h` antes de `pursuitescalation.h`.
   ~3,5 kB. Mismo dueño.
3. **Mío y sin peaje**: `find<Sim::IEntity**>`/`find<ISimpleBody**>` dentro de
   `AITrafficManager.cpp` y `find<IDebugWatchCar**>` dentro de `AIGoal.cpp`. ~1,1 kB.
4. **`lcfix.py` sobre las 30 entradas `$LC` de `zAI`** (trabajo de ventana): 320 B de `.rodata`.
5. **Sólo con `keep.lst` bajo control**: el orden de `#include` de `AITrafficManager.cpp` y
   `AICopManager.cpp` para `find<IPlayer**>`, `reserve<voiceIDs>`, `reserve<copPair>` y
   `find<IExplosion**>`. ~5 kB, pero cada ensayo obliga a `lcfix`.

---

## 7. La sonda

`linkdelta` da secciones, `permorden` da símbolos sin peso, y `dolwhere` **no arranca cuando las
secciones difieren de tamaño, que es justo cuando hace falta**. El censo de desplazamiento
ENLAZADO por símbolo —con tramos y con la frontera de la cola— sigue sin existir en `scripts/`;
la r55 ya lo propuso y la r56 ha tenido que volver a escribirlo. Queda en el scratchpad de la
sesión:

    …/scratchpad/zai_censo.py    censo enlazado por simbolo: EN SITIO / FUERA, tramos, --seq
    …/scratchpad/zai_pares.py    las dos secuencias con TAMAÑO, lado a lado, por ventana

Es candidato a `scripts/` **como herramienta general**: enlaza dos veces, agrupa por
desplazamiento y encuentra sola la frontera entre la zona de parseo y la cola.
