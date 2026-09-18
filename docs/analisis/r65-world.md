# r65 - lote `world`: retirada de andamios

Encargo: dejar fuente legitima, no bytes de DOL. Criterio de aceptacion: digests
**por seccion ALLOC** (`.text`, `.rodata`, `.data`, `.sdata*`, `.ctors` y sus
`.rela.*`) identicos antes y despues; si cambian, se revierte en el acto.

Ficheros y unidades: `Src/World/WeatherMan.cpp` (**zTrack**),
`Src/World/Common/WRoadNetwork.cpp` (**zWorld2**), `Src/Gameplay/GManager.cpp`
(**zGameplay**).

## 1. Censo

Censo limpio (`scratchpad/world65/censo_andamios.py`: pines
`register T x asm("rN")` + barreras de plantilla VACIA; NO cuenta el
`asm(".section ...")` de fichero de WeatherMan ni los `__asm__("simbolo")` de
enlace de nombre como `rsc_memset` o `lbl_8041D3AC`).

| fichero | antes | despues |
|---|---|---|
| WeatherMan.cpp | 5 pines + 1 barrera = **6** | 2 pines + 1 barrera = **3** |
| WRoadNetwork.cpp | 4 pines + 4 barreras = **8** | 4 pines + 4 barreras = **8** |
| GManager.cpp | 4 pines + 0 barreras = **4** | 2 pines + 0 barreras = **2** |
| **total** | **13 pines + 5 barreras = 18** | **8 pines + 5 barreras = 13** |

El brief contaba 13 (solo pines); con las barreras el lote eran 18. **Retirados 5,
todos pines.** Intentados los 13 sitios (cinco retirados, ocho remedidos).

## 2. Sellos ALLOC

Base = el arbol tal cual estaba al empezar, recompilado y sellado antes de tocar
nada. Las tres unidades, seccion a seccion (el digest es el mismo ANTES y
DESPUES en todas las casillas):

| unidad | `.text` | `.rodata` | `.data` | `.bss` | `.ctors` | `.rela.text` | `.rela.rodata` | `.rela.data` | `.rela.ctors` |
|---|---|---|---|---|---|---|---|---|---|
| zTrack | 53c6c4a7645e | a7dc0c28d36d | cd56439f089a | 9905dd0349cd | 9069ca78e745 | 5a3baada94e8 | - | d3e26dd11be1 | 39c945641691 |
| zWorld2 | f3ba20f9dd3c | 706a40b1fcc4 | 450285c58b58 | 9f2abd41234b | 9069ca78e745 | b615654521d2 | 913f1096f471 | - | 6768e810a915 |
| zGameplay | f83a7bb7e86a | fdc0b66e46d5 | 822b7b6f6c8e | db9a47725049 | 9069ca78e745 | 1c72252a0982 | 277881c1c18c | 5f95cd6929fb | 7582b51d44cf |

`diff` de los 25 pares base/final = **vacio**. Ninguna seccion ALLOC se movio en
ninguna de las tres unidades.

Control de determinismo hecho al empezar: recompilar zTrack sin tocar nada da los
mismos ocho digests, asi que el compilador es determinista y todo cambio
observado viene del cambio de fuente. (Por eso NO se usa el sha1 del `.o`
completo: `.line` cambia con cada sentencia retirada.)

## 3. Retiradas

### 3.1 `GManager::AllocateInstanceMap` -- 3 pines fuera (GManager.cpp)

`regmap zGameplay "GManager::AllocateInstanceMap"` daba **ESTRUCTURA**: dos
locales SOLO NUESTRAS (`vaultBase` r9, `vaultOffset` r0) que el original no tiene.

Lo que estaba escrito:

    register GVault *vaultBase asm("r9") = mVaults;
    register unsigned int vaultOffset asm("r0") = onVault << 6;
    register GVault *vault asm("r30") = (GVault *)((char *)vaultBase + vaultOffset);

El DWARF del original (`symbols/mw_dwarfdump.nothpp:1148155`) dice
`struct GVault & vault; // r30`: una **referencia**, igual que en
`MakeTempLoadData` justo arriba. Con `GVault &vault = mVaults[onVault];` las 70
instrucciones salen iguales (280/280) y solo queda una transposicion limpia
r30<->r31 entre `this` y el par `vault`/`tableSize`.

Esa transposicion la cierra **un** pin, y va sobre una local que el DWARF
confirma con ese mismo registro (`tableSize // r30`):

| forma | % | filas |
|---|---|---|
| natural, sin ningun pin | 98,07143 | 24 |
| + `register GVault &vault asm("r30")` | 99,71429 | 3 |
| + `register unsigned int tableSize asm("r30") = 256` | **100** | **0** |
| natural + declaracion de `tableSize` movida arriba | 98,07143 | 24 |

`.text` de zGameplay: **f83a7bb7e86a antes -> f83a7bb7e86a despues**; los otros
ocho digests, iguales. `regmap` pasa de "2 locales solo nuestras" a **6 de 6
iguales**.

### 3.2 `RegionQuery::CalculateRegionInfo` -- 3 pines fuera (WeatherMan.cpp)

`regmap zTrack` daba **ESTRUCTURA**: CUATRO locales SOLO NUESTRAS (`fogStart`
f11, `fogFalloff` f12, `fogFalloffX` f13, `fogFalloffY` f0). El original asigna
los miembros a pelo y compara contra el valor que le reenvia CSE (fila 54 del
objetivo: `fcmpu f0,f11`, no un `lfs` de `DistFogStart`). `fog_colour` y `retcol`
SI son locales del original, las dos en r11.

Barrido completo, todo 1208/1208:

| andamios | conjunto | % | filas |
|---|---|---|---|
| 0 | forma natural pelada | 98,54636 | 35 |
| 6 | los cinco pines + la barrera (como estaba) | **100** | **0** |
| 5 | sin `fo` / sin `fx` / sin `fy` / sin `retcol` | **100** | **0** |
| 5 | sin la barrera | 99,92053 | 4 |
| 5 | sin `start` | 99,6192 | 3 |
| 2 | `start` + barrera | 99,93378 | 4 |
| 3 | `start` + barrera + `fo` | 99,93378 | 4 |
| 3 | `start` + barrera + `retcol` | 99,93378 | 4 |
| **3** | **`start` + barrera + `fx`** | **100** | **0** |
| **3** | **`start` + barrera + `fy`** | **100** | **0** |
| 4 | `start` + barrera + `fo+fx` / `fo+fy` / `fx+fy` / `fx+retcol` | **100** | **0** |
| 4 | `start` + barrera + `fo+retcol` | 99,93378 | 4 |

Se deja `start` (fr11) + barrera + `fogFalloffY` (fr0): **3 andamios, 0 filas**.

`.text` de zTrack: **53c6c4a7645e antes -> 53c6c4a7645e despues**, y los otros
siete digests iguales.

**Por que uno basta, y por que tiene que ser fr0 o fr13.** En coma flotante
`REG_ALLOC_ORDER` (rs6000.h:932) es f0, f13, f12, f11, f10. Los tres valores de
los `Falloff` son cantidades LOCALES y se reparten primero; los dos que quedan
--el de `BaseWeatherFogStart` y el de `BaseWeatherFog`, vivos hasta los `fcmpu`
de los bloques siguientes-- son allocnos GLOBALES y cogen f11 y f10. Ocupando el
PRIMERO o el SEGUNDO de la lista, las tres locales caen en el orden del objetivo
(Falloff f12, FalloffX f13, FalloffY f0) y los dos globales detras. Pinchar fr12
(`fo`) llega tarde en ese orden y no sirve: 4 filas.

Negativos adicionales medidos sobre la forma natural (35 filas): las **120
permutaciones** del orden de las cinco asignaciones (minimo 10 filas, ninguna
cierra), las 3 formas de la condicion del `if` (comparar contra `retcol` /
`DistFogColour` / los `Base*`: objeto IDENTICO, CSE las normaliza) y las 2 formas
del bloque `oldDistFog* = ...` (con los `Base*` en vez de los miembros: 50 filas).

Con el pin de `fog_colour` a r11 en vez del de `retcol` el objeto sale identico:
son la misma cantidad (los dos aparecen en r11 en el DWARF del original).

## 4. Irreducibles, con diagnostico

Los ocho de `WRoadNetwork.cpp` y los dos que quedan en `GManager.cpp`. Todos
**remedidos en la r65 sobre el arbol de hoy** (las vedas caducan) y anotados en el
comentario pegado al andamio, donde `previo.py` los encuentra.

| sitio | andamio | regmap | base | sin el |
|---|---|---|---|---|
| `HolePunchAvoidables` | pin `wld_cutx` fr11 | ESTRUCTURA (1 local de mas) | 4 filas | **15** |
| `HolePunchAvoidables` | barrera `+f(offset_change)` | -- | 4 filas | **46** |
| `InitAtSegment` | pin `guard` r28 + sus 2 barreras | 10 de 10 iguales | 12 filas | **49** |
| `CookieTrailCurvature` | pines `mx__` fr13 / `apex_width` fr31 + barrera | 1 local de mas (`mx__`) | 0 filas (100 %) | **3** |
| `GetStrippedNameKey` | pin `lastSlash` r3 | **IDENTICO** | 0 filas | **5** |
| `AllocateInstanceMap` | pin `tableSize` r30 | 6 de 6 iguales | 0 filas | **24** |

Diagnostico por andamio:

* **`wld_cutx` fr11**: es el unico ESTRUCTURA que no cae, y el eje NO es la forma
  de la sentencia. Medido hoy: `cut_to_position.x = cut_to_position.x + ...` 15
  filas, `+=` 15, la sentencia de `.z` delante de la de `.x` 38. La cadena causal
  esta en el fuente desde la r47: `offset_change` es una cantidad local y
  `find_free_reg` coge f11 salvo que este ocupado; en el objetivo lo ocupa la
  carga de `cut_to_position.x`, cuya cantidad se reparte antes por
  `qty_compare_1`. Hace falta invertir esa prioridad, no reescribir la sentencia.
* **barrera `+f(offset_change)`**: es la otra mitad de la misma palanca; sin ella
  el pin de fr11 no vale nada (46 filas). Las dos son una sola cosa.
* **`guard` r28 + 2 barreras**: la cantidad fantasma de la r36d. `regmap` da
  **10 de 10 locales iguales**, o sea que NO es deuda de estructura: compensa una
  cantidad del original que el DWARF no puede listar (no tiene nombre y no emite
  ni una instruccion). No hay forma de fuente que la fabrique.
* **`mx__` fr13 / `apex_width` fr31 + barrera**: `apex_width` SI es local del
  original (su DWARF no le da location, pero el asm la deja en f31); `mx__` es el
  temporal VOLATIL del objetivo, invisible al DWARF por construccion. La forma
  natural `float apex_width = UMath::Max(...)` pierde el `fmr` (3 filas) -- el
  diagnostico de la r29, que ya barrio 15 formas de la sentencia con objeto
  identico.
* **`lastSlash` r3**: `regmap` **IDENTICO**: la local y el registro son los del
  original; el pin solo gana una carrera de `global_alloc`. Sin el son 5 filas y
  las cinco son el mismo r3 -> r9, con tamano 100/100 y ni una instruccion de mas.
  El eje, leido en `orig/prodg/NGC_GNU_SRC/NGC/gcc/global.c`: `lastSlash` es un
  allocno GLOBAL sin ninguna preferencia de registro, porque `set_preference` con
  un `(plus A B)` coge SOLO `XEXP(src,0)` -- y ahi esta `name`, otro pseudo sin
  numerar, no el hard r3 que devuelve `bStrLen`. Sin preferencia, `find_reg` cae
  en `REG_ALLOC_ORDER` (0, 9, 11, 10, 8, ...), r0 no vale de base y sale r9.
  **MEDIDO Y NEGATIVO**: `bStrLen(name) + name` y `&name[bStrLen(name)]` dan un
  `.o` identico al de "sin pin" -- el front-end de C++ canonicaliza `ptr + int` y
  vuelve a poner el puntero primero. Por forma de la sentencia NO sale.
* **`tableSize` r30**: local del original con SU registro del original; es el pin
  que paga los tres de `AllocateInstanceMap`. Fija el orden de reparto entre
  `this` y ese allocno, nada mas.

## 5. Veredicto

**FUNCIONA, parcial por lote**: 18 andamios -> 13 (**-5 pines, -28 %**), con los
25 digests ALLOC de las tres unidades identicos byte a byte. Cero regresiones y
cero reversiones: ninguna variante aceptada movio una sola seccion, y las que las
movian no se aceptaron.

Por fichero: GManager 4 -> 2, WeatherMan 6 -> 3, WRoadNetwork 8 -> 8.

Dos de los 13 que quedan (`tableSize` r30 y `lastSlash` r3) son pines sobre
locales que el DWARF confirma CON ESE MISMO REGISTRO: no inventan nada, solo
fuerzan un orden de reparto. Los otros cuatro sitios son los que siguen abiertos,
y el eje comun esta acotado: **el ORDEN en que `local_alloc` / `global_alloc`
reparten dos cantidades**, no la forma de la sentencia. Las rondas anteriores
(r36c/r36d, r47, r49, r61) agotaron ahi las palancas de ocupacion y de barrera;
lo que falta es una palanca de PRIORIDAD, y el `.lreg`/`.greg` de cc1plus ya sabe
medirla.

### Lo que este lote deja como metodo

1. **El DWARF dice si la local existe, y tambien de que TIPO es.** Los tres pines
   de `AllocateInstanceMap` cayeron porque la linea del volcado era
   `struct GVault & vault`, una referencia: la forma natural estaba a la vista.
2. **Un pin sobre una local REAL con su registro REAL no es la misma deuda que un
   pin que inventa una local.** `tableSize` r30 y `lastSlash` r3 son del primer
   tipo, y uno de ellos vale tres del segundo.
3. **Barrer el conjunto, no los elementos.** En `CalculateRegionInfo` ninguno de
   los seis andamios era individualmente imprescindible (quitar `fo`, `fx`, `fy`
   o `retcol` por separado da 100 %), pero tres si lo son en conjunto. Quitar de
   uno en uno decia "cuatro sobran" y quitarlos todos decia "hacen falta los
   seis": las dos lecturas son falsas. La reticula completa es lo unico que da el
   minimo.
