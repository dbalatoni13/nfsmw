# r64-plat -- zPlatform y zGameplay

**Titular.** Ninguna de las dos promociona, y esta ronda dice **por que no**, con
la medida. Los dos frentes que la r63 dejo abiertos estaban mal diagnosticados:

- **zPlatform**: sus 6.699 B son la `.rodata`, si; pero **la receta de cuatro
  pasos de la r63 no puede funcionar**. La bloquean **once literales que
  `cc1plus` registra y el objetivo no tiene**, y que `-strip-unused-data` **no se
  lleva enteros**: dejan **cadaver**. `keep.lst` solo sabe CONSERVAR, asi que no
  hay entrada que los quite. El frente de verdad es **que cabeceras parsea el
  TU**, y esta identificada la primera: `Src/World/WorldConn.h` explica **diez**
  de las cadenas que nos faltan, **en el orden exacto del objetivo**.
- **zGameplay**: su `.data` esta a **UN simbolo** de ser identica. Quitando el
  `asm()` de la cola, los **seis** `kObjectTemplateKey` caen en las **seis
  direcciones exactas** del objetivo. Lo unico descolocado son 68 B de ceros que
  tienen que salir **detras del bloque diferido**, y hay **tres negativos
  medidos** de como no se consigue.

Cero regresiones: las dos unidades acaban exactamente donde empezaron
(`dolwhere` 6.699 y 35.438, `fncmp` 0/136 y 0/768, `textorder` de zPlatform sigue
en `ORDEN PERFECTO`). Las unicas ediciones son **dos bloques de comentario**
(50 lineas cada uno, `git diff --numstat` = `50 0` en las dos), puestos donde
`previo.py` los encuentra. **Cero** ediciones en `configure.py`,
`config/GOWE69/*`, `splits.txt` o `symbols.txt`. **Cero** correcciones de `lcfix`
pendientes: no he creado ni movido un solo `$LC`.

---

## 0. Sellos y estado, medidos hoy

```
zPlatform   build_direct x3  -> 780cb82a30068cf60faab16b9171747450416903  (los tres)
            (base d3eadbd1264b507370fb9af726a887189bb43637; la diferencia es
             SOLO .debug_line: 50 lineas de comentario)
            fncmp      -> 0 de 136 funciones con el CODIGO distinto
            textorder  -> DESCOLOCADAS 0 de 137, SALTOS 0, >>> ORDEN PERFECTO
            dolwhere   -> 6.699 B distintos  (identico a la r63)
            trypromo   -> DOL ROTO

zGameplay   build_direct x3  -> a94cd345d6f9368b316f4ec5ab5e9499a4a3add1  (los tres)
            fncmp      -> 0 de 768 funciones con el CODIGO distinto
            textorder  -> DESCOLOCADAS 168 de 768, SALTOS DE DELTA 35
            dolwhere   -> 35.438 B distintos  (identico a la r63)
            trypromo   -> DOL ROTO
```

Aviso de metodo: la r63 daba `168` como `35 descolocadas`; la cifra buena de
`textorder` hoy es **168 de 768 descolocadas con 35 SALTOS DE DELTA**. Son dos
columnas distintas y conviene no confundirlas al repartir trabajo.

---

## 1. zPlatform: el mapa completo de la `.rodata`, cadena a cadena

La `.rodata` de la unidad son **6.712 B** (`0x80402108..0x80403B40`) y difieren
**6.699**. Extraidos los dos tramos del ELF **enlazado** y troceados en cadenas:

| | objetivo | nosotros |
|---|---:|---:|
| cadenas | 120 | 161 |
| primer byte que difiere | -- | **0x334** |

Los **0x334 primeros bytes casan byte a byte**: son el `asm()` prefijo
`lbl_80402108` de zPlatform.cpp. A partir de ahi la unidad es una **permutacion**,
no un deficit de contenido: de las 120 cadenas del objetivo **tenemos 118**. Solo
faltan dos de verdad, y no existen en ningun sitio del arbol:

```
0x0FE4  "TempPalette"
0x12CC  "GCHW_VD from PlatSetFirstMovieFrame"
```

### 1.1 De donde sale cada tramo

```
objetivo : [0x000..0x05C]  prefijo irreproducible (bWare/STL: GAMECUBE,
                            d:/mw/.../bware.hpp, bad_alloc, %f,%f,%f,
                            %f,%f,%f,%f, STL)      92 B
           [0x060..0x1A38] UN SOLO POOL de cc1plus, alineado a 8 en 0x060

nosotros : [0x000..0x334]  asm() prefijo de 820 B  (los 92 B buenos + 728 B
                            que DUPLICAN literales que cc1plus ya emite)
           [0x338..     ]  pool de cc1plus (con los duplicados y los cadaveres)
           [0x17BA..    ]  asm() de cola con 38 cadenas escritas a mano
```

La alineacion cuadra sola y eso es util: **el pool arranca alineado a 8**
(nuestro prefijo acaba en 0x334 y el pool sale en 0x338; el del objetivo acaba en
0x5C y el pool sale en 0x60). O sea que el prefijo escrito a mano solo puede
acabar en un offset multiplo de 8 **donde el objetivo empiece una cadena**: los
candidatos son 0x060, 0x300, 0x318, 0x390, 0x3B0, **0x450** y **0x478** (el
comienzo de los textos de error del DVD).

### 1.2 El orden del pool es el orden de `$LC`, y el de `$LC` es el de PARSEO

`cc1plus` emite **166 `$LC`** en esta unidad y los vuelca **en orden numerico**.
Comprobado con `ngccc -S`: el pool de la unidad es, literalmente,
`$LC59, $LC60, $LC151..158, $LC162, $LC163, $LC170, $LC171, $LC222..227, $LC233..238,
$LC249..252, $LC321, $LC341..344, $LC351, $LC364, $LC367..` en ese orden.

Eso **cambia el frente**: no son los `#include` del `.cpp` (el `.text` ya esta en
`ORDEN PERFECTO` y el pool sigue mal), sino **que cabeceras ve el TU y en que
orden**. Es el mismo mecanismo que `parseord.py` mide para las vtables, pero
aplicado a los literales.

### 1.3 Lo que lo bloquea: once literales de mas, y su CADAVER

Estos once los registra `cc1plus` y el objetivo **no los tiene en ninguna parte**:

| `$LC` | cadena | de donde sale |
|---|---|---|
| 58 | `""` | -- |
| 170 | `EventSequencerSystems` | `Main/EventSequencer.h` |
| 222 | `Attrib::Gen::chassis` | `Src/Generated/AttribSys/Classes/chassis.h` |
| 224 | `Attrib::Gen::induction` | idem `induction.h` |
| 225 | `Attrib::Gen::nos` | idem `nos.h` |
| 226 | `Attrib::Gen::tires` | idem `tires.h` |
| 227 | `Attrib::Gen::transmission` | idem `transmission.h` |
| 321 | `Attrib::Gen::presetride` | idem `presetride.h` |
| 405 | `" "` | -- |
| 442 | `done` | -- |
| 443 | `GManager` | -- |
| 447 | `EAGL4::SymbolEntry` | -- |

Estan **muertos** (nada los referencia) y por eso no estan en `keep.lst`. Pero
`-strip-unused-data` **no borra un simbolo muerto entero**: borra `size & ~7`
bytes y **deja los `size & 7` ultimos EN SU SITIO**. Medido **seis veces** en el
enlace de esta misma unidad, leyendo los bytes del ELF enlazado:

| simbolo muerto | tamano | lo que queda |
|---|---:|---|
| `EventSequencerSystems` | 22 | 6 B -- `"stems\0"` |
| `Attrib::Gen::chassis` | 21 | 5 B -- `"ssis\0"` |
| `Attrib::Gen::induction` | 23 | 7 B -- `"uction\0"` |
| `Attrib::Gen::tires` | 19 | 3 B -- `"es\0"` |
| `Attrib::Gen::transmission` | 26 | 2 B -- `"n\0"` |
| `Attrib::Gen::presetride` | 24 | **0 B** |

Seis de seis. **La regla, ahora con DIRECCION**: el enlazador se lleva el
principio y **deja la COLA**; un simbolo de tamano multiplo de 8 desaparece
entero. (`memory/nfsmw-ventana-de-32` lo tenia como "cadaver de 1-7 B" sin decir
que mitad sobrevive; ahora esta medido, y ademas se ve **dentro** de un pool, no
solo al final de una seccion.)

Suma de cadaver dentro del pool: **~36 B** que el objetivo no tiene y que
**ninguna entrada de `keep.lst` puede quitar**. Mientras esos once literales se
registren, la `.rodata` **no puede cuadrar**, se ponga el prefijo donde se ponga.

### 1.4 Por que la receta de la r63 esta REFUTADA

La r63 proponia: (1) recortar el prefijo a 92 B, (2) borrar 33 entradas
`@lc zPlatform` de `keep.lst`, (3) quitar 22 cadenas duplicadas del bloque de
cola, (4) mover `#include` hasta que el pool salga en orden.

- El paso **2 empeora**: borrar una entrada `@lc` no hace desaparecer la cadena,
  la convierte en **cadaver** (1.3). 33 entradas -> ~120 B de basura nueva.
- El paso **4 no tiene palanca**: el orden lo fija el numero de `$LC`, y ese lo
  fija el parseo de **cabeceras**, no el orden de los `#include` de `.cpp` de la
  SourceList (que ya esta clavado: `textorder` = ORDEN PERFECTO).
- Los pasos **1 y 3** son correctos pero **no bastan**, y sueltos rompen el
  tamano de la seccion.

### 1.5 EL CAMINO, y es un hallazgo con firma

Las diez cadenas `World*`/`Pkt_*` que hoy escribimos a mano **no son
irreproducibles**. El objetivo las tiene en este orden:

```
WorldBodyConn, Pkt_Body_Open, Pkt_Body_Service, World_UpdateBody, Pkt_Body_Send,
World_OneShotEffect, Pkt_Effect_Send, WorldEffectConn, Pkt_Effect_Open,
Pkt_Effect_Service
```

Y `DECLARE_WORLDPACKET(_PKT_, _HANDLER_)` (`Src/World/WorldConn.h:29`) emite
**primero `#_HANDLER_`** (dentro de `ConnectionClass()`) **y luego `#_PKT_`**
(dentro de `DECLARE_SIMPACKET`). Recorriendo las clases de `WorldConn.h`
lineas 115..237 -- `Pkt_Body_Open`(WorldBodyConn), `Pkt_Body_Service`(idem, ya
deduplicada), `Pkt_Body_Send`(World_UpdateBody), `Pkt_Effect_Send`
(World_OneShotEffect), `Pkt_Effect_Open`(WorldEffectConn), `Pkt_Effect_Service`
(idem) -- sale **exactamente esa secuencia de diez**.

Es una firma, no una coincidencia: **el TU del objetivo parsea `WorldConn.h` y el
nuestro no lo ve**. Comprobado: `grep -c WorldConn` sobre nuestro `.s` de
zPlatform = **0**.

Lo mismo pasa con el bloque que lo rodea en el objetivo
(`Attrib::Gen::ecar`, `camerainfo`, `effects`, `audioimpact`, `audioscrape`
delante; `Attrib::Gen::speech` detras): son **una tirada contigua de 16 cadenas
del objetivo que nuestro parseo no genera**, y con `WorldConn.h` dentro. El
trabajo de la proxima ronda es **encontrar el `#include` que las trae** (y que
seguramente se lleva por delante alguno de los once literales de 1.3, porque
cambia el juego de cabeceras).

---

## 2. zGameplay: la `.data` esta a un simbolo, y el simbolo es el ultimo

Comparando **simbolo a simbolo el ELF enlazado** en `0x8041D2B8..0x8041D580`:

| | objetivo | nosotros (base) |
|---|---|---|
| `sNumSpawned` | 0x8041D4A8 | 0x8041D4A8 |
| `kObjectTemplateKey` x6 | 0x8041D4AC..0x8041D53C | **0x8041D4F0..0x8041D580** |
| 68 B de ceros | 0x8041D53C..0x8041D580 | **0x8041D4AC** (`gap_06_8041D53C_data`) |

Todo lo demas casa direccion a direccion. Los `gap_06_*_data` y `lbl_8041D3AC`
que nosotros nombramos y el objetivo no son relleno anonimo en el original: mismo
sitio, mismo tamano.

**Positivo medido**: quitando el `asm()` de la cola y reenlazando, los seis
`kObjectTemplateKey` caen en **0x8041D4AC, 0x8041D4C4, 0x8041D4DC, 0x8041D4F4,
0x8041D50C y 0x8041D524** -- las **seis direcciones exactas** del objetivo. Y los
68 B del objetivo son **ceros** (leidos del DOL en 0x8041D53C). O sea: el
contenido esta bien, el orden no.

**Por que no se arregla moviendo el `asm()`**: `kObjectTemplateKey` es un
estatico LOCAL de la plantilla `FindInstances<T>`
(`Src/Gameplay/GObjectBlock.cpp:60`) y sale en el **bloque diferido** de
`finish_file`; un `asm()` de ambito de fichero sale en el **punto de parseo**, o
sea siempre delante. Ya esta al final del `.cpp`.

### 2.1 Tres negativos medidos

1. `asm()` dentro de un miembro de plantilla + `template struct X<int>;`
   -> **cc1plus no emite nada**: ni la funcion ni el label. `grep
   gap_06_8041D53C_data` sobre el `.s` = 0.
2. `asm()` dentro de una plantilla de funcion + `template int f<int>();`
   (instanciacion explicita de **funcion**) -> **igual, cero**.
   **En este compilador la instanciacion explicita no emite; hace falta un USO
   real.** Es un hecho reutilizable: cualquier receta del arbol que se apoye en
   `template ...;` para colocar algo esta muerta antes de empezar.
3. Forzar el uso con un global (`int ref = X<int>::emit();`) **si** instancia,
   pero el global mete 4 B en `.data` y el enlace sale **+32** (la ventana de
   32): `sNumSpawned` pasa de 0x8041D4A8 a 0x8041D4C8. Peor que el punto de
   partida.

### 2.2 Lo que queda por probar, en orden

- **(a)** Un **estatico LOCAL de plantilla**, que es justo lo que hace
  `kObjectTemplateKey`: `static unsigned char pad[68]` con
  `__attribute__((section(".data")))` y alias `asm("gap_06_8041D53C_data")`,
  dentro de una plantilla instanciada por un **uso real** posterior al de
  `FindInstances<T>`. Es el unico mecanismo del arbol que ya se sabe que deja
  bytes en `.data` **detras** del bloque diferido.
- **(b)** `__static_initialization_and_destruction_0` se emite en la linea
  **1391574** del `.s`, **detras** de `kObjectTemplateKey.*` (1356138): un ctor
  `inline` de un global se pliega ahi dentro y su `asm()` saldria en el sitio
  correcto. Cuesta el almacenamiento del global y hay que compensarlo.
- **(c)** Que los 68 B sean **relleno de alineacion del enlace**: 0x8041D580 es
  multiplo de **128** y 0x8041D53C no. Nuestro `zLua.o` **extraido** tiene la
  `.data` con alineacion 4 (medido: sin el `asm()`, zLua arranca en 0x8041D540),
  asi que hoy no se puede comprobar. **Cuando zLua se compile de verdad hay que
  volver a mirar esto antes de dar por buena la cola escrita a mano.**

### 2.3 El resto de zGameplay, para quien lo retome

`dolwhere` reparte los 35.438 B asi (los mayores):

| B | simbolo | que es |
|---:|---|---|
| 19.701 | `SetAttribute__H1Zb_11GRaceCustom...` | ventana de `.text` -- orden del bloque diferido |
| 8.236 | `SetIsLoading__11GRaceStatusb` | idem |
| 4.247 | `$LC221` | `.rodata` |
| 287 | `$LC3` | `.rodata` |
| **212** | `gap_06_8041D53C_data` | **la seccion 2 de este informe** |
| 180+164+124+60 | `_vt.8GHandler`, `_vt.8GTrigger`, `_vt.11GRaceCustom`, `_vt.9GActivity` | orden de vtables |
| 106 | `typeTable.21825` | `.data` |

O sea: **~28 kB son el orden del `.text` diferido**, ~4,5 kB la `.rodata`, ~530 B
las vtables y **212 B esta cola**. El unico tramo con diagnostico cerrado y
receta concreta es el de 212 B.

---

## 3. Propuestas para el jefe

- **`keep.lst`: CERO propuestas, y una ADVERTENCIA.** La propuesta que la r63
  dejo escrita --borrar las 33 entradas `@lc zPlatform` (lineas 1278..1330 y
  2881..2891)-- **no debe aplicarse**: por la seccion 1.3 no quita las cadenas,
  las convierte en ~120 B de cadaver dentro del pool. Aplicarla empeora zPlatform
  y no arregla nada.
- **`splits.txt`, `symbols.txt`, `configure.py`: cero propuestas.**
- **Ninguna re-extraccion.**
- **Ninguna cabecera compartida tocada.** Los dos ficheros editados
  (`src/Speed/Indep/SourceLists/zPlatform.cpp` y
  `src/Speed/Indep/SourceLists/zGameplay.cpp`) son SourceLists, exclusivos de su
  unidad, y las dos ediciones son **solo comentario** (`git diff --numstat`:
  `50 0` cada una).
- **`lcfix`: 0 pendientes por mi parte.** No he creado, borrado ni movido un solo
  literal; los `.o` de las dos unidades tienen `.text` y `.rodata` identicos a los
  de la base (`dolwhere` da las mismas cifras al byte).

---

## 4. Para la memoria del proyecto

1. **`-strip-unused-data` deja la COLA, no el principio.** Borra `size & ~7`
   bytes por delante y deja los `size & 7` ultimos en su sitio. Medido seis veces
   en un solo pool. Un simbolo de tamano multiplo de 8 desaparece entero; los
   demas dejan un trozo de cadena reconocible ("stems", "ssis", "uction"), que es
   **la manera barata de cazar literales de mas**: si en el volcado de una
   `.rodata` aparece un trozo de palabra, ahi hubo una cadena que el objetivo no
   tiene.
2. **`keep.lst` solo sabe CONSERVAR.** No existe la operacion "quitar esta cadena
   del pool". Toda receta que diga "borra estas entradas `@lc` y el tamano
   cuadra" hay que comprobarla contra los cadaveres antes de creersela.
3. **El orden de la `.rodata` es el orden de `$LC`, y el de `$LC` es el orden de
   PARSEO DE CABECERAS.** Una unidad puede tener el `.text` en ORDEN PERFECTO y
   la `.rodata` permutada de punta a punta: son dos frentes independientes y
   `textorder` no ve el segundo.
4. **En GCC 2.95.3 / ProDG la INSTANCIACION EXPLICITA no emite.** Ni
   `template struct X<int>;` ni `template int f<int>();` producen una sola linea
   de asm. Para colocar algo en el bloque diferido hace falta un **uso real**.
5. **Una firma de macro vale como prueba de que cabecera parseaba el original.**
   El orden `HANDLER, PKT, PKT, HANDLER, PKT, ...` de diez cadenas seguidas
   identifico `WorldConn.h:29` sin compilar nada. Cuando en la `.rodata` del
   objetivo aparezcan cadenas "que nadie emite", buscar la **macro** que las
   genera y comprobar si su orden coincide: si coincide, no hay que escribirlas
   a mano, hay que **incluir la cabecera**.
