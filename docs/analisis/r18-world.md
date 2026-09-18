# Ronda 18 — zWorld, zWorld2, zSpeech, zTrack

Linea base `base_r18_world.json`: **527.248/542.976 B (97,1034%)**, 1.885 funciones
al 100%. Estado final `despues_r18_world.json`: **527.248/542.976 B (97,1034%)**.
**+0 B**. Lo que se ha movido esta ronda es *correccion*, no bytes: dos funciones
que objdiff daba al 100% referenciaban simbolos equivocados y ahora no.

---

## 1. Fase de preparacion — auditoria con `audit.py`

`audit.py` sobre las cuatro unidades, **antes**:

| unidad | funciones auditadas | FALLA |
|---|---:|---:|
| zWorld | 543 | 14 |
| zWorld2 | — | 6 |
| zSpeech | — | 14 |
| zTrack | — | 3 |

**Despues: zWorld 0, zWorld2 1, zSpeech 1, zTrack 1** — y los tres que quedan son
falsos positivos de la herramienta, demostrados abajo.

### 1.1 `UnloaderScenery__FP6bChunk` (zTrack) — REAL, CORREGIDA

Identidad: 896 B / 224 instrucciones, `objdiff = 100%` en los dos lados.
`audit.py`: `reubicacion a otro simbolo: LightTable contra MaxSceneryLightContexts`.

El asm dice exactamente que pasa (indices del diff):

    objetivo                                    nuestro
    211 li   r0, 0                              li   r0, 0
    212 lis  r11, LightTable@ha                 lis  r11, MaxSceneryLightContexts@ha
    213 lis  r10, SceneryLightContextTable@ha   lis  r10, LightTable@ha
    214 lis  r9,  MaxSceneryLightContexts@ha    lis  r9,  SceneryLightContextTable@ha
    215 stw  r0, MaxSceneryLightContexts@l(r9)  stw  r0, SceneryLightContextTable@l(r9)
    216 li   r3, 1                              li   r3, 1
    217 stw  r0, LightTable@l(r11)              stw  r0, MaxSceneryLightContexts@l(r11)
    218 stw  r0, SceneryLightContextTable@l(r10)stw r0, LightTable@l(r10)

GCC 2.9 iza los `lis` en **orden de fuente** (r11, r10, r9) y emite los `stw` en el
orden **(3, 1, 2)**. Nuestro fuente era `Max, Light, SLCT` -> emite `SLCT, Max, Light`.
Para que el objetivo emita `Max, Light, SLCT` el fuente tiene que ser
`Light, SLCT, Max`. Aplicado en `src/Speed/Indep/Src/World/Scenery.cpp:557`:

    LightTable = nullptr;
    SceneryLightContextTable = nullptr;
    MaxSceneryLightContexts = 0;

`audit.py` pasa. **Coste 0 B, tres reubicaciones que apuntaban al global equivocado.**

### 1.2 `Init__5WGridPC6UGroup` (zWorld2) — REAL, CORREGIDA, y es un frente

`audit.py`: `reubicacion a otro simbolo: __builtin_new contra __builtin_vec_new`.
El objetivo llama a `__builtin_new` (o sea `operator new(size_t)`) donde nosotros
llamamos a `__builtin_vec_new` (`operator new[]`), en el `else` de `WGrid::Init`.

La causa es `bWare.hpp:136`:

    #ifndef BWARE_NO_INLINE_GLOBAL_NEW
    inline void *operator new(size_t size) { return ::operator new[](size); }
    #endif

Con ese inline visible, **todo** `new T` plano acaba en `__builtin_vec_new`. El
comentario del propio fichero afirma que sin el fallarian 623 sitios y con el solo
19. **Eso esta medido al reves.** Prueba con el compilador del proyecto
(`scratchpad/wnewA.cpp` / `wnewB.cpp`, cflags de zWorld2):

| forma | `new T(x)` plano | `new ("x",0) T(x)` colocado |
|---|---|---|
| sin el inline, colocado NO inline | `__builtin_new` | `__nw__FUiPCci` |
| **sin el inline, colocado SI inline** (= bWare.hpp real) | **`__builtin_new`** | **`__builtin_vec_new`** |
| con el inline (arbol actual) | `__builtin_vec_new` | `__builtin_vec_new` |

Los 623 sitios `vec_new` del objetivo vienen del `operator new(size_t, const char*, int)`
**colocado**, que si es inline y cuyo cuerpo es `new char[size]`. No los produce el
inline del `new` plano.

**Medida decisiva sobre zWorld2** (`objdump -dr`, sitios de llamada a new/vec_new,
funcion a funcion, ordenados):

    objetivo                11 sitios
    nuestro (con inline)    11 sitios  -> 1 discrepancia (Init__5WGrid)
    nuestro (-DBWARE_NO_INLINE_GLOBAL_NEW)  11 sitios -> **diff VACIO**

Aplicado como guarda de unidad al principio de `zWorld2.cpp` (igual que ya hacia
`zMisc.cpp`). Efecto colateral medido: **desaparecen tambien las otras 5 FALLA de
zWorld2** (`_.tmp_7` contra `_.tmp_0`, etc.), porque el contador de temporales
anonimos del TU se alinea con el del original. Es corroboracion independiente.

**Censo game-wide del sintoma**: en los objetos objetivo hay **19** `bl __builtin_new`
y en los nuestros **0**. Reparto: zEcstasy 5, zMain 6, zMisc 1, zSim 1, zFEng 2,
zFe2 1, zFeOverlay 1, zPhysicsBehaviors 1, zWorld2 1. Las 18 que no son mias siguen
mal.

**Segundo punto de medida, zMain** (compilado al scratchpad, sin tocar su fuente ni
su `.o`): 10 sitios en el objetivo.

    con el inline (arbol actual)  -> 6 mal (los seis Clone__Q26Attrib*_TypeHandler)
    con -DBWARE_NO_INLINE_...     -> 1 mal (__10GameDevicei)

O sea la guarda arregla 6 y rompe 1. Ese 1 es
`InputDeviceGC.cpp:347  this->mWheelDevice = ::new SteeringWheelDevice(deviceIndex);`,
donde el objetivo emite `__builtin_vec_new`: **ahi el fuente nuestro es el que esta
mal**, no la guarda. Con los dos puntos de medida, quitar el inline de `bWare.hpp`
arregla ~18 sitios y descubre 1 bug de fuente. Pero es cabecera compartida y pide
A/B por objetos sobre todas las unidades: queda **como hallazgo para el
coordinador**, no aplicado. Enlace comprobado: `__builtin_new` lo DEFINE `zMisc.o`
(`T __builtin_new`), asi que la referencia nueva de zWorld2 resuelve.

### 1.3 Los cinco falsos positivos de `audit.py`

Cada uno demostrado, no supuesto:

1. **`_.tmp_N.MMMMM` contra `_.tmp_K.LLLLL`** (10 de las 14 de zWorld, 4 de zWorld2,
   6 de zSpeech). `audit.py` normaliza solo el sufijo `.\d+`; el primer numero es el
   **contador de temporales del TU** y no dice nada de la funcion. Sugerencia:
   normalizar `_\.tmp_\d+\.\d+` entero.
2. **Literal "distinto" que en realidad lleva reubicaciones dentro.** `__7SoundAI`,
   `__Q26Speech11PursuitFlow`, ... dan `b'\x00\x00\xff\xff\x00\x00\x00\x00'` contra
   `b'\x00\x00\xff\xff\x80*Qh'`: es una **entrada de vtable** de GCC 2.x
   (`{short delta; short index; void *pfn;}`) sin relocar en nuestro `.o` y relocada
   en el ELF. Igual `AllocateDefragmentStorage` (20 B = 5 punteros a cadena).
   `audit.py` compara bytes crudos de un literal que a su vez tiene relocs.
3. **Alias de simbolo de dtk.** `Init__Q26Speech7Manager`: `gcc2_compiled.` contra
   `CSISAllocatorMemAlloc__FUi`. `nm` del objeto objetivo: los dos estan en
   **.text+0**. Es la misma direccion.
4. **Reubicacion que falta en el objetivo porque el inmediato ya es el mismo.**
   `SyncPursuit`: `lis r7, 0x8048` contra `lis r7, WorldTimer@ha`, y
   `WorldTimer = 0x804800C0` -> `@ha = 0x8048`. Mismos bytes. Igual `OnCollision`
   con `_IHandle__8IVehicle = 0x80042A2C` -> `0x8004`.
5. **Addend distinto que resuelve a la misma direccion.** `DoCulling` (zTrack):
   objetivo `_14EAXAemsManager.m_RequiredSlots+0`, nuestro
   `_20GrandSceneryCullInfo.SceneryDrawInfoTable+0xa410`. En `symbols.txt`
   `SceneryDrawInfoTable = 0x804F42F0` con `size 0xA410`, y
   `m_RequiredSlots = 0x804FE700 = 0x804F42F0 + 0xA410`. **Es el mismo byte.**
   `audit.py` compara tipo/addend *antes* de resolver simbolo+addend a direccion
   absoluta; deberia comparar `addr[na]+add_a == addr[nb]+add_b`.
6. **Colision de nombre en `symbols.txt`.** `__tcf_0` (zWorld2, 4 B, `blr`):
   hay **7** `__tcf_0` distintos en `symbols.txt` (tamanos 0x4 y 0x2C);
   `direcciones()` se queda con el ultimo y compara contra otra funcion.

Con esos seis criterios, **las cuatro unidades quedan limpias**.

### 1.4 Lo que `audit.py` SI ha delatado y no es una funcion

Nuestro `zTrack.o` tiene un `_.tmp_0.16070` de 4 B en `.bss` que **nadie referencia**
y el objetivo no tiene. No es `matched_code` (es `.bss`), pero es codigo de mas en
alguna parte del TU.

---

## 2. `CookieTrailCurvature__8WRoadNav...` — MURO medido, y se ha quitado un asm falso

Identidad: 1.284 B objetivo / 1.280 B nuestro, 321 instrucciones, `0x80304A??`.
Estado de partida: el arbol tenia **ensamblador escrito** —lo que el brief prohibe—:

    asm("fmr %0,%1" : "=&f"(apex_width) : "f"(selected_width));

Fabricaba el `fmr` que falta, daba **99,657%** y **no cerraba** la funcion (el `fmr`
salia a f28 en vez de a f31, y arrastraba 19 discrepancias de registro). Se ha
retirado. Coste en `matched_code`: **0 B** (la funcion no estaba al 100% ni antes ni
ahora). Coste en `pctsnap`: **-0,125 pp**, unica regresion de porcentaje de la ronda,
y es deliberada.

### El diagnostico, con el DWARF del original

`symbols/mw_dwarfdump.nothpp:2382640` da las locales y sus registros:

    float road_curvature; // f25      float apex; // f29
    struct Vector3 current_to_apex; // r1+0x8
    const Vector3 & nav_position; // r28    const Vector3 & apex_position; // r29
    float dist_to_apex; // f31        int apex_cookie_index; // r3
    /* bloque anonimo */ const NavCookie & apex_cookie;  float apex_width;
                         struct Vector3 apex_to_nav; // r1+0x18   float sina;

**No existe `selected_width`**: el bloque anonimo tiene exactamente
`apex_cookie, apex_width, apex_to_nav, sina`, y ese es el orden de nuestro fuente
tras el arreglo. Ademas el DWARF fecha el inline `Max` en el rango
`0x80304C04 -> 0x80304C28` (9 instrucciones) y el `fmr f31, f13` que falta cae
**dentro** de ese rango: es la copia de retorno del inline `UMath::Max`, no una
sentencia de fuente que nos falte.

Lo que hace el objetivo: `VU0_floatmax` deja el resultado en f13 (volatil, no cruza
llamada) y `Max` lo copia a f31 (`apex_width`, que si cruza llamadas). Nosotros
fundimos los tres pseudos (retorno interno, retorno externo, variable) en uno solo.
Efecto de arrastre: objetivo `apex_width=f31, sina=f29`; nuestro al reves.

### Ensayos (todos medidos, `mn_repro` + `mn_diff` sobre WRoadNetwork.cpp, 56 s/uno)

| # | forma | resultado |
|---|---|---|
| c0 | el `asm("fmr")` del arbol | 99,657% · 1.284 B · **19** diffs |
| c1 | `float sel = Max(...); float apex_width = sel;` | 99,533% · 1.280 B · 10 |
| c2 | `float apex_width = UMath::Max(dist_to_apex, bAbs(R-L));` (**la del DWARF**) | 99,533% · 1.280 B · 10 |
| c3 | declaracion y asignacion separadas | 99,533% · 1.280 B · 10 |
| c4 | `bAbs` izado a un local previo | 99,533% · 1.280 B · 10 |
| c5 | `VU0_floatmax` directo, sin `UMath::Max` | 99,533% · 1.280 B · 10 |
| c6 | `const float apex_width = ...` | 99,533% · 1.280 B · 10 |
| c7 | `UMath::Abs` en vez de `bAbs` | 95,623% · **1.304 B** · 30 — peor |
| c8 | operandos del `bAbs` invertidos | 99,526% · 1.280 B · 12 — peor |
| c10 | segundo local que copia el resultado | 99,533% · 1.280 B · 10 |
| c9 | `register float apex_width asm("f31")` | **no compila** (`invalid register name`) |
| c11 | `register float apex_width asm("fr31")` | 97,402% · **1.292 B** · 54 — mucho peor |

**Se queda c2**, la forma que confirma el DWARF, con el comentario en el fuente.
La restriccion de registro —el ultimo recurso que el brief autoriza— **se ha probado
y empeora**: no hay cierre por esa via tampoco. Sumando las 15 formas de rondas
anteriores, son ~25 formas barridas.

**Lo que NO he probado**: tocar `UMath::Max` o `VU0_floatmax` en `UMath.h` /
`UVectorMath.h` para que el retorno externo no se funda (p.ej. un local con nombre
dentro de `Max`). Son cabeceras compartidas por todo el juego y el DWARF del original
dice que `Max` **no tiene locales**, asi que la forma seria falsa aunque cerrase.

---

## 3. `InitAtSegment__8WRoadNavscf` (816 B) — pista concreta sin cerrar

Mismo tamano en los dos lados (816 B), 42 discrepancias, todas de registro/pila.
La raiz **no** es el reparto de registros, es el marco:

    objetivo   stwu r1,-0x90   stmw r24,0x58(r1)   buffer int->float en 0x50
    nuestro    stwu r1,-0x98   stmw r25,0x64(r1)   buffer int->float en 0x58

El objetivo usa **un GPR salvado mas** (r24..r31 contra r25..r31) y **8 B menos de
locales**. Las dos partes usan exactamente los mismos slots accedidos
(`0x8,0xc,0x10,0x18,0x1c,0x20,0x28,0x2c,0x30,0x38,0x3c,0x40`) y el mismo
`addi rN, r1, 0x48` para `nodePtr`. Nos sobran **8 B en 0x50..0x58** que no
referencia ninguna instruccion: un objeto reservado y eliminado.

El DWARF del original (`2380796`) da los offsets exactos y confirma el fuente:
`vec // r1+0x8`, los dos `Vector3Make::c` en `r1+0x28` y `r1+0x38`,
`nodePtr[2] // r1+0x48`, `laneInd // r24`, `segment // r25`, `timeStep // f31`.

Ensayo medido: **quitar** `register const WRoadSegment *segment asm("r25")`
-> 99,005% y 49 diffs (peor que 99,152% y 42). La restriccion se queda.

**No probado**: de donde salen los 8 B. La hipotesis viva es que GCC nos reserva
**dos** buffers de conversion int->float (uno por `GetRawLaneOffset`) y solo usa uno;
habria que mirar `WRoadProfile::GetLaneOffset` / `GetLaneNumber`.

---

## 4. El resto de la lista, y por que no se ha tocado

| B | funcion | unidad | estado |
|---:|---|---|---|
| 2.980 | `HolePunchAvoidables` | zWorld2 | 92 diffs, mismo tamano. Hallazgo suelto: en idx 490 el objetivo emite `fmsubs f0,f31,f30,f25` y nosotros `f30,f31` — **orden de operandos de un producto en el fuente**. En idx 564 el objetivo **derrama** la direccion de un literal a `0x1f4(r1)` y la recarga; nosotros la rematerializamos. No cerrable en el tiempo de la ronda |
| 2.908 | `RenderFlaresOnCar` | zWorld | `lis-1`, dmax 313: izado real, no jitter |
| 1.068 | `UpdateLightStateTextures` | zWorld | 17 vedas ya anotadas en el fuente; lleva `register int t asm("r8")` + `asm volatile("" : "+r"(t))` |
| 1.044 | `CheckCollideSRB` | zWorld2 | 45 registros, no mirada |
| 876 | `UpdateWheelYRenderOffset` | zWorld | vedas anotadas (el objetivo tiene DOS pseudos `@ha` del mismo 0.0f vivos en el preencabezado) |
| 836 | `CullParts` | zWorld | 4 vedas anotadas; falta un `mr` del preencabezado que sale de `loop.c`. Lleva `asm("" : : : "r11")` |
| 708 | `GetLoadingPriority` | zTrack | **vedada** |
| 684 | `DefragmentPool` | zWorld | permutacion global de GPR |
| 596 | `Setup__RoadblockFlow` | zSpeech | no mirada |
| 552 | `PursuitEscalation` | zSpeech | UNA `stw` a 3 ranuras de distancia. Ver abajo |
| 504 | `NotifyEventCompletion` | zSpeech | no mirada |
| 316 | `LoadSpeechBank` | zSpeech | **vedada** |
| 304 | `SetMemoryPoolSize` | zWorld | UNA `stw` a 2 ranuras. Ver abajo |
| 252 | `BreakAway` | zSpeech | Ver abajo |

### Las tres de "una `stw` mal colocada": diagnostico comun

`PursuitEscalation`, `BreakAway` y `SetMemoryPoolSize` tienen **una sola** `stw`
desplazada 2-7 ranuras y todo lo demas identico. En `BreakAway` se ve la causa:

    objetivo   lwz r0, 0x1cc(r31) ... slw r0,r0,r3 ; stw r0,0x14(r1)  <- r0 se pisa
    nuestro    lwz r10,0x1cc(r31) ... slw r10,r0,r3 ; ... stw r10,0x14(r1)

El objetivo mete `dir` en **r0**, que el `lwz r0,0xc(r28)` siguiente machaca: la
antidependencia **obliga** al planificador a poner la `stw` antes. Nosotros lo
metemos en r10, no hay dependencia y `sched2` la baja. O sea: la `stw` desplazada es
**sintoma**, la causa es la eleccion de registro, y `sched2` corre *despues* del
reparto. Perseguir la `stw` desde el fuente es perseguir el sintoma. **Lo dejo
anotado, no barrido.**

---

## 5. Verificacion

- `build_direct.py zWorld zWorld2 zSpeech zTrack` -> 4 ok.
- `measure.py --cmp base_r18_world.json despues_r18_world.json` -> **+0 B, +0 funciones**.
- `audit.py` en las cuatro -> 3 FALLA, las tres falsos positivos demostrados en 1.3.
- `pctsnap --cmp` contra `base_r18_pct_TOTAL.json`: **EMPEORA 1**
  (`CookieTrailCurvature` -0,125 pp, la retirada del `asm` fabricado, 0 B),
  MEJORAN 5 (ninguna mia: venian de commits posteriores a esa instantanea).
- `frozen.py cong` sobre las cuatro unidades.
