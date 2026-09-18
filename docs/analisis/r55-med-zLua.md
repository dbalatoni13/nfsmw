# r55 · medida de `zLua` — el `.text` esta cerrado; lo que queda es POOL y `.data`

**Unidad:** `Speed/Indep/SourceLists/zLua` — 94.264 B de codigo.
Reconstruida con `python scripts/build_direct.py Speed/Indep/SourceLists/zLua`
(`1 ok, 0 fallidas`, a la primera). Nada mas del arbol se ha tocado: esta ronda es de medida.

## Titular

`zLua` **no** es un caso de orden de emision. Su `.text` esta practicamente terminado
—**una** transposicion de dos simbolos, 164 B— y **todo** el dano al DOL lo producen
**1.044 B de CONTENIDO que no emitimos**: 720 B en el pool de `.rodata` y 324 B en `.data`.

Y la seccion `CAMBIO DE DUENO (M4)` que imprime `movidos.py` para esta unidad es un
**falso positivo entero**: dice `-17.676 de otro` en `.text` y **ningun simbolo cambia de
objeto**. Detalle y prueba en «Sorpresas».

---

## 1. `reorden.py zLua`

```
seccion     simbs palabras   crudas  CONTENIDO  sin par
.ctors          0        0        0          0        0
.data          15        7        0          0        8
.rodata       406       42        9          0      402
.text         536    23546      753          0        0

CONTENIDO 0: no queda nada que escribir.
```

**Por la letra del criterio: `ORDEN_PURO`. Por lo que de verdad pasa: no.**
La columna que importa es `sin par`. `reorden` empareja **por nombre**, y en `zLua`:

* `.text` — empareja **536 de 536**, 23.546 palabras, **0 de contenido**. Esta medida **si**
  es solida: el codigo de `zLua` es correcto palabra a palabra.
* `.rodata` — **402 de 406 simbolos sin pareja**. Compara **cuatro**. El residuo de `zLua`
  esta justo ahi, en los `lbl_`/`$LC` que no se emparejan: la medida no lo puede ver.
* `.data` — **8 de 15 sin pareja**, y los 8 son exactamente los 324 B que faltan.

O sea: `CONTENIDO 0` aqui es **cierto y vacio**. Ver §6.

Aviso adicional, medido: `reorden` **enmascara la mitad baja de toda forma D**, y el par
`lis/addi` de una cadena son dos formas D. Una funcion que apunta a **otra** cadena
compara IGUAL. En `zLua` eso importa: `DumpStack__10LuaRuntimeP9lua_State` mide 472 B en los
dos y `reorden` la da por identica, pero el objetivo le cuelga once literales
(`NIL`, `BOOL`, `LUD`, …) que nuestra version no tiene (§4).

## 2. `movidos.py Speed/Indep/SourceLists/zLua`

```
zLua: 39851 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
   10611 cambian de direccion, pero 10597 es ARRASTRE (tres terminos: donde
   empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
   cambia de dueno).
   .bss: base-1024 delta+192, .data: base-704 delta-324, .rodata: base+0 delta-424,
   .sbss: base-1024 delta+0, .sbss2: base-1024 delta+0, .sdata: base-1024 delta+0,
   .sdata2: base-1024 delta+0, .text: base+0 delta+17676

   CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
      .bss       nuestro .o    +192, seccion enlazada      +0  ->     -192 de otro
      .data      nuestro .o    -324, seccion enlazada    -320  ->       +4 de otro
      .rodata    nuestro .o    -424, seccion enlazada    -720  ->     -296 de otro
      .text      nuestro .o  +17676, seccion enlazada      +0  ->   -17676 de otro

**14 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

seccion       movidos      bytes
.text               2        164
.rodata             3         79
.data               6         24
?                   3          0
```

**14 permutaciones reales**, 267 B en total. Es la cifra mas baja documentada del bloque A.

## 3. `linkdelta`, `permorden`, `trypromo`

```
linkdelta   Speed/Indep/SourceLists/zLua       +0   rodata-720 data-320
permorden   objetivo 537 funciones, nuestro 699, comunes 537
            solo nuestras (las estripa el enlazador): 162
            en su sitio: 536 de 537   DESPLAZADAS: 1
            ciclo de 2: Thunk__10VoidBinderP9lua_State (obj#514 -> nue#513)
                        __Q33UTL3Stdt3map3ZUiZ18LuaAttribAccessorsZ9_type_map (obj#513 -> nue#514)
trypromo    Speed/Indep/SourceLists/zLua       DOL ROTO (83f6e001fa68)
```

### Los bytes de DOL, por seccion (byte a byte sobre los dos ELF enlazados)

| seccion | tamano orig | tamano nuestro | bytes distintos |
|---|---:|---:|---:|
| `.data` | 267.228 | 266.908 | **140.204** |
| `.rodata` | 312.608 | 311.888 | **120.829** |
| `.text` | 3.804.440 | 3.804.440 | **44.637** |
| `.over` | 149.920 | 149.920 | 1.412 |
| `.sdata` / `.init` / `.sdata2` | — | — | 25 / 10 / 1 |
| **total** | | | **307.118** |

Los **44.637 B de `.text` son SOMBRA DE REUBICACION**, no codigo: de los 18.028 simbolos
comunes de `.text` **18.026 estan en la MISMA direccion** y solo los dos del ciclo se mueven
(164 B). Lo que difiere son las mitades `@ha`/`@l` de los `lis/addi` que apuntan a
`.rodata`/`.data`/`.sdata`, que se han corrido. **Arreglando el pool y el `.data`, esos
44.637 B desaparecen solos.**

## 4. Simbolo dominante: **ninguno**. El residuo es DIFUSO, y aqui esta el mapa

No hay un simbolo cuyo desplazamiento arrastre. Hay **1.044 B de contenido que falta**,
repartidos en unos veinte sitios. Mapa del deficit vivo a lo largo de la ventana
`.rodata` de `zLua` (`803ED338..803EF580`), con el acumulado:

```
803ED398   +9    +9   +"GAMECUBE"
803ED42C   -9    +0   -"MGeneric"
803ED460  -57   -57   -"Attrib::Gen::milestonetypes","LuaRuntime memory pool","LuaRuntime"  +"done"
803ED4A8 -144  -201   -"Lua emergency allocation","LuaRuntime temp compression buffer",
                       "LuaRuntime VM reset snapshot","NIL","BOOL","LUD","NUMBER","STRING",
                       "TABLE","FUNC","UD","THREAD","(?)",", "
803ED598  +14  -187
803ED5C0  +36  -151
803ED5D8  -26  -177   -"1.8.1","19.8.31","GRaceStatus"
803ED614  -11  -188   -"LuaBindery"
803EDED0  +12  -176
803EE150  -14  -190   -"LuaAttributes"
803EE3FC -274  -464   -16 cadenas: "Attrib::Gen::speech","...pvehicle","...engine",
                       "...engineaudio","MAudioReflection","PlayerNum","Dist","Covered",...
803EE560  +27  -437
803EE5D0 +185  -252   (bloque nuestro adelantado ~470 B)
803EE654 -230  -482   -18 cadenas: "MForcePursuitStart","MinHeatLevel","MSetCopsEnabled",...
803EE7A4   -3  -485
803EE820  +23  -462
803EEDB4  -56  -518   -"cannot resume dead coroutine","cannot resume non-suspended coroutine"
                       (las emitimos, el enlazador las estripa y deja "tine"/"utine")
803EEE80 -134  -652   -17 cadenas: los NOMBRES del enum LuaAllocType
803EF510   -6  -658   -"C0","C0"
```

`-658` de cadenas **mas los 60 B de `luaT_eventname` que no emitimos = −718 ≈ el −720 de
`linkdelta`.** La contabilidad cierra.

### Las cuatro familias, con su emisor identificado

**A · nombres del enum `LuaAllocType` — 156 B, los emite `lmem.c`.**
`803EEE80..803EEF1C`: `CHAR, INT, STATE, GLOBALSTATE, TOBJECT, LOCVAR, TSTRINGPTR,
GCOBJECTPTR, INSTRUCTION, CALLINFO, CLOSURE, UPVAL, PROTO, PROTOPTR, UDATA, NODE, INVALID`
— **el orden del enum de `lmem.h:15-35` (valores 1…17) y el 0 al final**: la firma de un
`switch` que devuelve literales. Cae justo antes de `luaO_nilobject` (`803EEF1C`, de
`lobject.c`), o sea entre `#include lmem.c` (`zLua.cpp:71`) y `#include lobject.c` (`:73`).
**Nadie lo referencia en el objeto original**: es codigo MUERTO cuyos literales se quedaron
en el pool.

**B · tipos cortos de `DumpStack` — ~64 B, los emite `LuaRuntime.cpp`.**
`803ED508..803ED54B`: `NIL, BOOL, LUD, NUMBER, STRING, TABLE, FUNC, UD, THREAD, (?)` y el
separador. Nuestro `DumpStack__10LuaRuntime` (`LuaRuntime.cpp:907`) mide **los mismos
472 B** pero no imprime tipos.

**C · etiquetas `HAND_POOL_TAG` de LuaRuntime — 112 B, `LuaRuntime.cpp`.**
`"LuaRuntime memory pool"`, `"Lua emergency allocation"`,
`"LuaRuntime temp compression buffer"`, `"LuaRuntime VM reset snapshot"`. En el original las
referencian `CreateState__10LuaRuntime`, `BindAccessors__13LuaAttributes` y
`LoadMetatable__10LuaBindery` (leido de `.rela.text` del objeto extraido). Es el mecanismo
de `forense1` **al reves**: alli sobraba una etiqueta, aqui faltan cuatro.

**D · nombres de mensaje y `Attrib::Gen::*` — ~330 B netos, entre `LuaAttributes.cpp` y
`LuaGameHooks.cpp`.** `803EE3FC..803EE900`. **Es la continuacion exacta del arreglo que ya
hay en `zLua.cpp:45-56`**, cuyo comentario dice «…y las once cadenas de WorldConn
(803EE2D0..**803EE3FC**)». El arreglo anterior llega hasta `803EE3FC` y **para justo ahi**.

### El `.data`: 324 B que no existen, y un orden

Las dos aportaciones a `.data`, en orden de direccion:

```
OBJETIVO (620 B)                          NUESTRO (296 B)
+  0   4  _10LuaRuntime.mObj              +  0   4  _10LuaRuntime.mObj
+  4  12  <hueco, CEROS>                    (falta)
+ 16   4  _13LuaPostOffice.fObj           +  4   4  _13LuaPostOffice.fObj
+ 20   8  <hueco, CEROS>                    (falta)
+ 28   4  _10LuaBindery.fObj              +  8   4  _10LuaBindery.fObj
+ 32   4  _13LuaAttributes.fObj           + 12   4  _13LuaAttributes.fObj
+ 36 192  accessorTable                   + 16 192  accessorTable
+228 108  <hueco, CEROS>                    (falta)
+336  72  flagMapping                     +208   4  kDisableNIS           <-- ORDEN
+408   4  kDisableNIS                     +212   4  kPrintScriptMessages
+412   4  kPrintScriptMessages            +216  72  flagMapping           <-- ORDEN
+416   4  <hueco, CEROS>                    (falta)
+420   4  sRealloc                        +288   4  sRealloc
+424   4  sFree                           +292   4  sFree
+428 192  <hueco, CEROS>                    (falta)
```

12 + 8 + 108 + 4 + 192 = **324 B, exactamente el deficit**. Y **los cinco huecos son CEROS
en el DOL de verdad** (leido de `orig/GOWE69/sys/main.dol`, no del objeto extraido, que pone
a cero lo reubicado): son estaticos con inicializador explicito a cero, que GCC 2.95 mete en
`.data` y no en `.bss` (`-fzero-initialized-in-bss` no existe hasta GCC 3.4).

Ademas hay **una permutacion de verdad**: el objetivo declara `flagMapping` **antes** que
`kDisableNIS`/`kPrintScriptMessages`; nosotros al reves.

## 5. Y las 162 funciones que sobran

`permorden` las llama «las estripa el enlazador» y es cierto, pero **no son inocuas**:

* Son 17.676 B de `.text` y **637 B de cadenas** en 35 literales.
* Al estriparlas, el enlazador quita `size & ~7` y **deja la cola**: en la ventana viva de
  `zLua` sobreviven `tine`, `utine`, `es`, `node`, `ssis`, `stems`, `uction` — **38 B de
  basura dentro del pool**, que corren todo lo que va detras.
* Son la API de Lua que el original **no** tiene: `lua_resume`/`lua_yield`/`resume` (0 en el
  objetivo, 3 en el nuestro), `luaL_*` (**8** en el objetivo, **30** en el nuestro), `Dump*`
  de `ldump.c` (**1** en el objetivo —`DumpStack__10LuaRuntime`—, **14** en el nuestro),
  `luaM_growaux`, y una decena de `reserve__`/`find__` de `_STL::vector` para tipos que
  `zLua` no usa (`WCollisionTri`, `Hermes::Handler`, `EAX_CarState`, `IVehicle`, `IPursuit`,
  `IPlayer`, `IModel`, `IHud`, `IVehicleCache`).

## 6. Sorpresas — tres, y la primera contradice el brief

### 6.1 El `CAMBIO DE DUENO (M4)` de `zLua` es un FALSO POSITIVO completo

`movidos` imprime `.text  nuestro .o +17676, seccion enlazada +0  ->  -17676 de otro`.
Suena a que `zLua` le roba 17,6 kB a otro objeto. **No le roba nada.** Prueba directa, sobre
los dos ELF enlazados:

```
histograma de desplazamientos .text (todos los simbolos comunes):
   +0     18028 simbolos
   +48        1 simbolo   Thunk__10VoidBinderP9lua_State
   -116       1 simbolo   __Q33UTL3Stdt3map3ZUiZ18LuaAttribAccessorsZ9_type_map
```

**Ni un solo simbolo de `.text` cambia de sitio** salvo el ciclo conocido. Los 17.676 B son
las **162 funciones muertas** que el enlazador estripa (§5).

La resta `enlazado − delta` **confunde dos cosas distintas**: bulto que cambia de dueno (M4
de verdad) y **bulto nuestro que el enlazador tira**. En `zLua` es 100 % lo segundo, en las
cuatro secciones (`.bss +192`, `.rodata −296`, `.data +4` son igualmente estripado y
alineacion). **Concuerda con el censo de `control` en la r54**, que daba `zLua: dueño = 0` —
es `movidos` el que se equivoca, no el censo.

**Recomendacion**: que `movidos` reste primero el bulto que el enlazador estripa (los
simbolos que estan en nuestro `.o` y **no** en el ELF enlazado) antes de llamar M4 al resto.
Con esa correccion `zLua` daria 0 en las cuatro lineas. Sin ella, **cualquier unidad con
instanciaciones de plantilla muertas va a declarar un M4 que no existe**, y son casi todas.

### 6.2 `reorden.py` es CIEGO exactamente donde `zLua` tiene el problema

`CONTENIDO 0` con **402 de 406** simbolos de `.rodata` sin pareja y **8 de 15** de `.data`.
Los 8 sin pareja de `.data` **son** los 324 B que faltan. `reorden` mide `.text`
estupendamente y no mide `.rodata` en absoluto. Para el bloque A, un `CONTENIDO 0` **solo es
concluyente si la columna `sin par` es pequena**. Propuesta barata: que imprima el
**porcentaje de bytes cubiertos** por seccion junto al veredicto.

### 6.3 El objetivo guarda cadenas MUERTAS en el pool

`803EEE80..803EEF1C` (17 cadenas, 156 B) y `803EEDB4` (las dos de corrutina) **no las
referencia nadie** en el objeto original: comprobado sobre **las cuatro** secciones `.rela`
(`.text`, `.ctors`, `.rodata`, `.data`). Estan en el DOL igual. O sea: el original compilo
ese codigo, el enlace se llevo el codigo y **dejo los literales**. Nuestro enlace, en
cambio, si estripa nuestros literales muertos (§5). Cerrar `zLua` exige que esas cadenas
**existan y sobrevivan**, y eso puede pedir entradas de `keep.lst` — que esta ronda no toca.

## 7. Siguiente paso concreto

**El de mayor relacion valor/coste es la familia D**, porque el arreglo ya existe en la
misma unidad y solo hay que continuarlo:

> En `src/Speed/Indep/SourceLists/zLua.cpp`, **entre la linea 56
> (`#include "Speed/Indep/Src/World/WorldConn.h"`) y la 58
> (`#include ".../LuaGameHooks.cpp"`)** — el mismo bloque cuyo comentario dice que llega
> «hasta 803EE3FC» — anadir los `#include` de las clases generadas que emiten las cadenas de
> `803EE3FC..803EE560`, **en el orden en que el objetivo las tiene**: `speech.h`,
> `pvehicle.h`, `engine.h`, `engineaudio.h`, `audiosystem.h`, `speechtune.h`,
> `pursuitlevels.h`, `world.h`, `visuallook.h`, `aivehicle.h`, `pursuitescalation.h`,
> `pursuitsupport.h` (de `Speed/Indep/Src/Generated/AttribSys/Classes/`), mas las cabeceras
> de mensaje que aportan `MAudioReflection`, `PlayerNum`, `Dist`, `Covered`,
> `VehicleParams`, `GManager`.
>
> **La evidencia de que van ahi** es la posicion en el pool del objetivo:
> `Attrib::Gen::speech` arranca en `803EE3FC`, exactamente donde termina el bloque de
> `WorldConn` que el `#include` de la linea 56 ya coloco bien, y termina en `803EE5D0`,
> justo donde empiezan los `MNISComplete`/`NISName` de `LuaGameHooks.cpp`. Vale **−274 B de
> los −720** (38 %) y ademas deshace el `+185` de `803EE5D0`, que es nuestro mismo bloque
> emitido ~470 B antes.

Segundo (independiente, mas barato de verificar y sin cabeceras compartidas): **los 324 B de
`.data`** — cinco estaticos con inicializador explicito a cero (12, 8, 108, 4 y 192 B) en
las posiciones de la tabla de §4, mas mover la declaracion de `flagMapping` **antes** de
`kDisableNIS`/`kPrintScriptMessages`. Cierra el `data-320` y con el los 140.204 B de `.data`
del DOL.

Tercero, y **es el que hay que estudiar antes de gastar rondas**: las 162 funciones de mas
(§5). Si `zLua.cpp` esta compilando mas Lua del que compilaba el original —`lauxlib.c`
entero, `ldump.c` entero, las corrutinas de `ldo.c`— quitarlo se lleva 17.676 B de `.text`
muerto **y los 38 B de colas de cadena que hoy corren el pool**. Ojo: hay que comprobarlo
primero, porque el original tambien tenia codigo muerto (§6.3) y ahi hace falta justo lo
contrario.

## 8. Herramientas usadas y sondas

`build_direct.py`, `reorden.py`, `movidos.py`, `linkdelta.py`, `permorden.py`,
`trypromo.py`. `rodorden.py` **no vale para `zLua`**: dice `sin rango de .rodata en
splits.txt` aunque la linea 134 de `splits.txt` lo tiene (empareja la unidad por otra
clave) — hueco de herramienta, anotado. `dolwhere` no se ha usado: infla por rangos.

Sondas de solo lectura en el scratchpad de la sesion, ninguna en `scripts/`, ninguna escribe
en el arbol: comparacion de tablas de simbolos de los dos `.o`, histograma de
desplazamientos por seccion sobre los dos ELF enlazados, diff de cadenas del pool a nivel de
secuencia y de multiconjunto, lectura de los huecos de `.data` en el DOL real y censo de
reubicaciones que apuntan a cada region.
