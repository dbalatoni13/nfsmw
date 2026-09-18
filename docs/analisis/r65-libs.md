# r65 -- lote `libs`: retirar andamios de `filesys`, `csis`, `spchpick` y `pathaction`

Encargo: dejar fuente legitima. No se buscan bytes de DOL. Por cada andamio, la
pregunta es *cual es la forma de fuente que lo hace innecesario*, y la retirada es el test.

Criterio de aceptacion usado (mas fuerte que el del encargo, ver sec. 1.2):
**sello ALLOC identico Y sello SYM identico, o se revierte**.

---

## 0. Resumen en una tabla

| fichero | unidad | andamios HEAD | andamios ahora | retirados |
|---|---|---|---|---|
| `Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `filesys` | 16 barreras + 5 pines = **21** | 5 barreras = **5** | **16** |
| `Libs/csis/dev/source/library/cmn/csis.cpp` | `csis` | 2 barreras + 9 pines = **11** | 2 barreras + 3 pines = **5** | **6** |
| `Libs/spch/dev/source/library/cmn/spchpick.c` | `spchpick` | 3 barreras + 3 pines = **6** | 2 barreras + 3 pines = **5** | **1** |
| `Libs/path/5.01.04/source/cmn/pathaction.cpp` | `pathaction` | 2 barreras + 2 pines = **4** | **4** | **0** |
| | | **42** | **19** | **23** |

Y ademas, sin retirar andamio pero quitando invencion de fuente:
**6 locales inventadas** (4 en `csis`, 2 en `filesys`) y **2 veredictos de `regmap`
que pasan de ESTRUCTURA a IDENTICO** (`FILE_init`, y `FILE_overhead` que ya no
tiene nada que declarar).

Verificacion final, compilando **la version de HEAD y la mia en la misma sesion**:

| unidad | ALLOC HEAD | ALLOC ahora | SYM HEAD | SYM ahora | |
|---|---|---|---|---|---|
| `filesys` | `4a26cc845d60ef1fbc302bef6edfe8248b90dab6` | **igual** | `c10e88e6dcae91c3987c3fca72e762dcdc146d99` | **igual** | OK |
| `csis` | `c03aab080e9d80e65084ffaf0b1f1faa7f14d3b7` | **igual** | `76fde2022fdd4cafe3df49c760ced08b15493d52` | **igual** | OK |
| `spchpick` | `1f2aad346f5ceab188dc4361ced77c69ad818855` | **igual** | `332f58f66040e5746f17fb52e54c5dc83d8375c6` | **igual** | OK |
| `pathaction` | `a941a5ff14501908a9e8c4bce1c76d5b0ab823ff` | **igual** | `1a453820091ff25fe40bd9578c60dcf6652d3ee9` | **igual** | OK |

`WHOLE` (sha1 del `.o` entero) cambia en los cuatro --`.line` y `.debug*`--:
`6e29dfc2`->`29376c42`, `506f020b`->`834a1966`, `b2e7e34c`->`2163009b`,
`0745d103`->`31146368`. Ninguna de esas secciones es SHF_ALLOC: no entran en
ningun PT_LOAD y `dtk elf2dol` no las ve. **El DOL no se puede mover.**

---

## 1. Dos correcciones de metodo, las dos medidas

### 1.1 El censo del encargo cuenta 29 en este lote. Son 42

El censo del encargo define barrera como `__asm__("" ...)`. En C++ el arbol usa
tambien la grafia corta `asm("" ...)`, **que es exactamente la misma
construccion** y el censo no la ve. En este lote esconde **13 barreras**:

| fichero | `__asm__("")` | `asm("")` | pines | total real |
|---|---|---|---|---|
| `filesys.cpp` | 5 | **+11** | 5 | 21 |
| `csis.cpp` | 0 | **+2** | 9 | 11 |
| `spchpick.c` | 3 | 0 | 3 | 6 |
| `pathaction.cpp` | 2 | 0 | 2 | 4 |

Contado sobre el fuente **con los comentarios quitados** (`scratchpad/libs65/censo65.py`,
que tokeniza; un `grep` crudo cuenta de mas porque medio fichero de `filesys` son
comentarios de vedas que citan barreras).
**Si el censo global de 362 se hizo con esa regexp, el frente real es mayor.**

Y hay un tercer tipo que **ningun** censo cuenta y que es el andamio *menos*
legitimo de todos: `asm` con **plantilla no vacia que emite instrucciones**.
Quedan 3 en el lote: `csis.cpp:26` (`asm("stw %1,4(%2)" ...)`) y los dos `asm` de
nivel de fichero de `filesys.cpp` (4 y 1979) que inyectan bytes en `.rodata` y
`.sdata` para colocar el pool.

### 1.2 El sello ALLOC de la r64b tiene un agujero, y es el que tumbo `filesys`

La r64b dio `filesys.cpp` por identico y **la verificacion lo rechazo por `.bss`**.
La causa: **una reordenacion de `.bss`/`.sbss` no toca un byte de ninguna seccion
SHF_ALLOC** --su contenido no esta en el fichero-- y por tanto un sello que solo
hashee {nombre, tamano, contenido} de las ALLOC **no la puede ver**.

Sello de la r65 (`scratchpad/libs65/sello.py`), dos partes:

* **ALLOC**: {nombre, tamano, contenido} de toda seccion SHF_ALLOC y de sus
  `.rela`. Para `SHT_NOBITS` el contenido no existe pero **el tamano si entra**.
* **SYM**: la tabla de simbolos **restringida a las secciones ALLOC** --(seccion,
  nombre, valor, tamano, bind/type) en el orden del fichero--. Esto es lo que caza
  una reordenacion de `.bss`: los simbolos cambian de `st_value` y el sello se
  mueve.

Que funciona esta medido: SYM se mueve en cuatro de los intentos de esta ronda
--dos pines de `csis` (`0xb8eb515c`, `0x12e978e1`), la quinta barrera de
`AddToQueue` (`0x82c93e14`) y el experimento de los estaticos de `pathaction`
(sec. 3.5)-- y en ninguno de ellos lo delata el contenido de `.bss`, porque `.bss`
no tiene contenido en el fichero.
Recomendacion: **que la verificacion de la ronda use las dos partes**. Cualquier
sello sin la parte SYM puede volver a aprobar lo que la r64b aprobo.

### 1.3 `build_direct.py` me colo tres medidas falsas seguidas

La trampa 3 del encargo es real y me paso: un error de sintaxis en un intento de
`csis.cpp` dejo el `.o` anterior en disco, y mi comparador sello **ese** `.o`.
Tres formas de `NullAllocatorFlags` salieron "IDENTICO". Repetidas con el build
comprobado, las tres **CAMBIAN** (`0x40062fae`, `0xbe34095c`, `0xbf477a75`).
El runner de la r65 (`scratchpad/libs65/m.sh`) **aborta si la ultima linea no
acaba en `, 0 fallidas`**; sin eso, esta ronda habria entregado un falso positivo.

---

## 2. Lo retirado, con los digests

### 2.1 `filesys.cpp` -- 21 andamios a 5

Base `ALLOC 4a26cc84...` / `SYM c10e88e6...`; **las cuatro retiradas dan los dos
sellos identicos** y solo mueven `WHOLE`.

| funcion | que se quito | forma de fuente que lo sustituye | `WHOLE` tras el cambio |
|---|---|---|---|
| `FILE_overhead` | **2 pines + 5 barreras** (queda LIMPIA) | acumulacion en dos locales normales: `int operations = ...; int handles = ...; handles += operations; handles += ...; return handles + 24;` | `338ee687...` |
| `FileOperationQueue::Find` | **1 pin + 2 barreras** | la local inventada `queue` fuera y `this->` en su lugar; barrera de `Itr.node` fuera | `c6c01f04...` |
| `FILE_nametodevice` | **1 barrera** (`Itr.node`) | nada: sobraba | `0f240c2d...` |
| `FILE_init` | **2 pines + 3 barreras** | ver abajo | `92fddfa5...` |

`FILE_init`, en detalle, porque **la r64b se quedo a un paso**:

* `paths`: `register char **paths asm("r11") = (char **)(gpFileSysInfo + 1);` +
  barrera -> una sola sentencia, `gpFileSysInfo->psearchpaths = (char **)(gpFileSysInfo + 1);`
* `nOps`: la r64b quito las dos barreras y **midio que el pin hacia falta**
  (`0xa5205926`). Hacia falta *porque conservaron la local*. `regmap filesys
  FILE_init` dice que **el original NO TIENE la local `nOps`**, y con la
  expresion entera sin local:

  ```cpp
  gpFileSysInfo->FileSysHandleMemory =
      (FILESYSHANDLE *)(gFileSysOpts.MaxFileOps * sizeof(FILEOPERATION) +
                        (unsigned int)gpFileSysInfo->OpMemory);
  ```

  **sello ALLOC y SYM identicos**. `regmap` pasa de *"1 local solo nuestra, ESO VA
  PRIMERO"* a *"MISMO conjunto de locales, mismo arbol de bloques y MISMO
  REPARTO"*.

  Leccion generalizable: **desclavar un pin dejando la local es la prueba
  equivocada**. La local ES el andamio; el pin solo la sujeta.

NEGATIVO medido en el camino: `FILE_overhead` como **una sola expresion de
`return`** cambia el sello (`0x9c795f0c`); hacen falta las dos locales y las cinco
sumas sueltas. Y `nOps` escrito como **aritmetica de punteros**
`(FILESYSHANDLE *)(gpFileSysInfo->OpMemory + gFileSysOpts.MaxFileOps)` --que es el
idioma de la linea siguiente-- tambien cambia (`0x0717be26`).

### 2.2 `csis.cpp` -- 11 andamios a 5, y 4 locales inventadas fuera

Base `ALLOC c03aab08...` / `SYM 76fde202...`; identicos tras los dos pasos.

1. **6 pines desclavados** (lineas 23, 70, 78, 83, 296, 384 de HEAD): sello
   identico (`WHOLE 52e98815...`). Los seis eran redundantes.
2. **4 de esas locales eran invencion pura** y salen tambien (sello identico,
   `WHOLE 13653526...`):
   * `ICoreToIAllocatorAdaptor::CreateInstance`: `allocator = pAllocator` fuera ->
     `new (pAllocator->Alloc(...))`
   * los dos `ICoreToIAllocatorAdaptor::Alloc`: `allocator = gpAllocator` fuera ->
     `return gpAllocator->Alloc(size, NullAllocatorFlags());`
   * `System::Init`: `coreAllocator = gpCoreAllocator` fuera -> `if (gpCoreAllocator == NULL)`

   Los dos pines que quedan sin local (`tvp`, `next`) se desclavaron en el paso 1 y
   sus locales **si** hacen falta.

### 2.3 `spchpick.c` -- una barrera que habia CADUCADO

`iSPCH_MakeSampleRequests`, barrera `__asm__("" : "+r"(phraseOffset) : "r"(currentBankHandle))`
de la r38, **medida entonces como imprescindible**. Sello ALLOC y SYM identicos sin
ella (`WHOLE 0a905d88...`). Es el caso de libro de `nfsmw-andamios-caducan.md`: el
arbol cambio por debajo.

### 2.4 Extra gratis: `spchpick.c` ya es ASCII puro

Tenia dos bytes `0xAB`/`0xBB` (comillas latinas en un comentario) que lo hacian
**no-UTF-8** y que hicieron que la r64b se negara a tocar su gemelo `sfir.c`.
Cambiados a `"`: `ALLOC`, `SYM` **y `WHOLE`** identicos (`2163009b...` antes y
despues). Eso reconfirma el control de la r64b: **un cambio de comentario que no
cambia el numero de lineas da un `.o` bit a bit identico**, asi que el compilador
es determinista y la unica causa de un `WHOLE` distinto es una sentencia.
`Libs/snd/9/source/library/mix/sfir.c` se puede desbloquear igual.

---

## 3. Los irreducibles, con su diagnostico

Los diagnosticos estan escritos **en el comentario pegado al andamio**, donde
`previo.py` los encuentra. Aqui el resumen.

### 3.1 `filesys.cpp` / `FILEOPERATION::AddToQueue` -- 5 barreras

`regmap`: **ESTRUCTURA**. El original no tiene la local `newid`, y su
`curpriority` (r9) vive en un **bloque anonimo** que aqui no existe. Locales del
original: `device` r30, `Itr/current` r11, `prev` r10, `newoppriority` r8,
`b0{curpriority r9}`.

**Lo nuevo de la r65**: escrita la forma natural --cero locales, cero barreras,
`newoppriority = (GetPriority() << 24) | (GetId() & 0x00FFFF00);`-- salen
**252 B EXACTOS y 96,83 %** con **UNA sola fila**: el `slwi r9,r9,24` dos ranuras
antes que en el objetivo, que mete en medio el `lwz` del `Head()`.

| forma | sello ALLOC |
|---|---|
| base (5 barreras) | `4a26cc84` |
| sin la barrera 1 / 2 / 3 | `549c4511` / `e1474c34` / `aede5c88` |
| sin las del bucle (4 / 5) | `d8fa5d47` / `82c93e14` (**esta mueve SYM**) |
| natural (0 locales, 0 barreras), `Head()` antes o despues | `e1474c34` (el MISMO: el orden de sentencias no manda) |
| 1 barrera sobre `newpriority` entre `Head()` y el desplazamiento | `c0de70ba` |
| 1 barrera `"+m"(pending)` con `"r"(GetId())`, sin local `newid` | `8c95d43d` |
| quitar solo la local `newid`, barreras intactas | `aede5c88` |

Las cinco son load-bearing y no se reducen a menos. Ya llevaba r32, r36e y r37;
la r65 no la abre pero **cierra la via "quitar las locales inventadas"**.

### 3.2 `csis.cpp` / `System::Subscribe` y `System::Unsubscribe` -- 3 pines + 2 barreras

`regmap`: **ESTRUCTURA** en las dos. El original no tiene `node`, `nullNode` ni
`next`. **Y la forma del original esta encontrada**: son las inline
`CListDStack::Push` y `CListDStack::Remove` de `snd/9/source/library/cmn/slinklist.h`.

| funcion | forma | tamano | % | sello |
|---|---|---|---|---|
| `Subscribe` | `gSystems.Push(&pSystemDesc->linkNode);` | 392/392 **exacto** | **98,96** | `cecc31d8` |
| `Unsubscribe` | `gSystems.Remove(&pSystemDesc->linkNode); return RESULT_OK;` | 232/232 **exacto** | **99,02** | `4dea306a` |

Y **queda una sola causa, la misma en las dos**: `cse` elige otra **base** para el
acceso a `pprev`.

```
objetivo  stw r0, 0x20(r3)      <- next, base pSystemDesc
          stw r9, 0x4(r7)       <- prev, base NODO
nuestro   stw r0, 0x24(r3)      <- prev  } las dos con base r3: GCC demuestra que no
          stw r11, 0x20(r3)     <- next  } se solapan y mete una en la ranura de
                                          espera del lwz del head
```

Con dos bases distintas el planificador **no puede** reordenar y sale el orden del
objetivo; plegadas las dos a `r3`, si puede. Lo mismo en `Unsubscribe`:
`lwz r9,0x4(r10)` contra nuestro `lwz r9,0x24(r3)`.

**El arreglo esta en `slinklist.h`, no en `csis.cpp`** --hay que impedir que GCC
pliegue `pnode` en `pSystemDesc + off`--, y esa cabecera la comparten `csis` y
`snd` (via `sndcmn.h`, decenas de unidades). **Regla 4: no la he tocado. Lo
propongo como encargo propio**, con el barrido de todas las unidades de `snd`.

Dato util: `Unsubscribe` con `gSystems.Remove(...)` da **el mismo objeto** que
quitar solo la barrera (`4dea306a`), o sea que **el pin `asm("r10")` no aporta nada
por su cuenta: el trabajo lo hace la barrera**. Sacar el nodo a una local
declarada antes o despues de `i` da el mismo objeto en los dos casos (`0379b876`).

### 3.3 `spchpick.c` / `iSPCH_MakeSampleRequests` -- 1 pin

`regmap`: **ESTRUCTURA**, el bloque `b0` sobra entero (el original no declara
`phraseOffset`, ni `phrases`, ni `currentBankHandle`).

La forma natural --la misma que el **segundo** bucle de la propia funcion-- es:

```c
phraseChoice = &gEventChoice[channel].phrases[i];
bankIndex = phraseChoice->bankIndex;
if (gVoxBanks[bankIndex].bankHandle != phraseChoice->bankHandle) {
    phraseChoice->bankIndex = iSPCH_FindBankIndexFromHandle(phraseChoice->bankHandle);
```

**524 B EXACTOS y 98,32 %** --mejor que las nueve formas de la r36f, cuyo techo era
97,94 %-- y **una sola fila**: un empate de `sched1` entre `add r30,r31,r9`
(materializar `phraseChoice`) y `lwzx r3,r31,r9` (leer el `bankHandle`). El
objetivo carga primero y suma despues; nosotros al reves, porque el desempate lo
da el orden de emision RTL, que sigue al de la fuente, y en la fuente el puntero
va primero. Poner la lectura antes obliga a re-leer el array y se pierde el CSE.

| forma | sello | tamano / % |
|---|---|---|
| la de arriba, y sus variantes `phrases + i` y sin la local `bankIndex` | `1dd2804a` (**las tres el MISMO objeto**) | 524, 98,32 |
| igual con la comparacion al reves | `4778c61c` | 524, 98,24 |
| `bankHandle = ...phrases[i].bankHandle;` antes del puntero | `7b5d7c36` | 536, 96,43 |
| `bankHandle = phraseChoice->bankHandle;` (local de funcion, r27) | `d6865aef` | 528, 96,56 (sobra un `mr r3`) |
| leer el handle por la expresion completa, puntero dentro del `if` | `83fd7a38` | 532, 95,08 |

### 3.4 `spchpick.c` / `iSPCH_ChooseSamples` -- 2 pines + 2 barreras

`regmap`: **ESTRUCTURA**, dos locales solo nuestras (`activeSentence` r25,
`sampleTable` r17) y el parametro `sentence` desplazado (original r25, nuestro r4);
los 17 restantes casan. La r64b ya barrio siete formas y dejo la deuda
identificada: **el temporal `bank + 0xC` que el objetivo mantiene vivo en r17 sin
declararlo** (no sale en el DWARF, luego no es una local del fuente).

Aportacion r65: `VOXBANKHDR` mide exactamente `0xC` (`spchi.h`), asi que `bank + 1`
*es* ese puntero, y en `spchi.h` no hay ningun `BANKHDR_Get...` que lo devuelva
--los cuatro que hay calculan offsets mucho mayores--. Los cuatro andamios siguen
siendo load-bearing uno a uno: `57ee1635`, `37c8b9fc`, `94e05a06`, `fc20f7a1`.

### 3.5 `pathaction.cpp` / `PATHI_serviceaction` -- 2 pines + 2 barreras, y una via cerrada

Esta es la entrega mas util del lote aunque no retire nada, porque **mata un lead
que parecia el arreglo estructural obvio**.

`regmap pathaction PATHI_serviceaction` lista `lastwhile` y `lastendif` como
**locales DE LA FUNCION** en el original, con direccion fija (0x804FF608 /
0x804FF60C) --o sea `static` de bloque, y de ahi el sufijo `.78`/`.79` del
`DECL_UID`--. Nuestro fuente las declara `extern` a nivel de fichero con enlace de
nombre. Parecia claro que los cuatro andamios eran consecuencia de eso.

**NO LO SON.** Escritas como `static unsigned int` dentro de la funcion **y
quitados los cuatro andamios**: `PATHI_serviceaction` sale a **3252 B EXACTOS y
99,32 %**, con **las mismas diez filas en los mismos dos sitios**. El `.text`
completo pasa a `5d712408` y `ALLOC` a `69758cdc`.

Y encima la forma "correcta" rompe el enlace por otro lado: GCC las emite en
**`.sbss` (8 B nuevos)** mientras el objeto original **las referencia como `UND`**
(`objdump -t` sobre `build/GOWE69/obj/.../pathaction.o`) y el troceador las pone en
`.sdata`. Arreglarlo pide `splits.txt`, que esta fuera del encargo, y por
`nfsmw-rango-no-basta.md` mover el dato rompe el DOL.

**Via cerrada por las dos puntas. No reintentar desde la fuente.**

Lo que si queda por probar en esa funcion, y NO es esto: los dos sitios son el
mismo patron --GCC adelanta un `lwz` por encima de un `stw` porque demuestra que
el global sin direccion tomada no puede aliasar el `*action`--. La r18 ya barrio
orden de sentencias, `volatile` en el estatico, lectura `volatile`, clobber
`"memory"`, `!action->done` y `? 0 : 1`. La palanca no barrida es **hacer que la
direccion de `Path::milliseconds` escape** en la unidad. Los sellos de cada
andamio suelto: `ae9b61fd`, `74f3b060`, `199c58a3`, `493dad42`.

Y dos locales mas que `regmap` marca y nadie ha tocado, por si abren la funcion:
`ms` (original r30, nuestro en pila) y `pitchbits` (**solo nuestra**, r20).

---

## 4. Veredicto

**FUNCIONA, y mejor de lo que la r64b estimaba, pero por una razon distinta a la
que el encargo supone.**

* **23 de 42 andamios retirados (55 %)**, con los dos sellos identicos. La r64b
  midio 19 % de exito; la diferencia no es suerte: es que **la r64b probaba
  "desclavar el pin" y la r65 prueba "quitar la local que el pin sujeta"**. Cuatro
  de las cinco retiradas grandes salen de ahi (`FILE_init`, `FILE_overhead`,
  `Find`, los cuatro `allocator` de `csis`).
* **6 locales inventadas fuera** y **`FILE_init` pasa de ESTRUCTURA a IDENTICO** en
  `regmap` sin mover un byte de codigo.
* **19 irreducibles, los 19 con diagnostico escrito junto al andamio** y con el
  sello de cada forma probada, para que nadie los reintente a ciegas. Dos de ellos
  tienen la forma del original **encontrada y medida** (`csis` Push/Remove al
  98,96/99,02 % con tamano exacto; `spchpick` `MakeSampleRequests` al 98,32 % con
  tamano exacto), y en los dos casos la causa restante esta **localizada al byte**.
* `pathaction` sale a cero retiradas, pero con una via estructural **cerrada con
  medida** en vez de abierta y tentadora.

Los tres avisos que valen mas que la cifra:

1. **El censo global de 362 puede estar corto**: cuenta `__asm__("" ...)` y no
   `asm("" ...)`, y en este lote eso son 13 de 42. Hay que recontarlo.
2. **La verificacion de la ronda necesita la parte SYM del sello**, o volvera a
   aprobar lo que la r64b aprobo y la verificacion rechazo.
3. **`build_direct.py` me colo tres medidas falsas seguidas.** Cualquier runner que
   selle sin comprobar la ultima linea del build esta produciendo ruido.

---

## Apendice: como reproducir

Herramientas en `scratchpad/libs65/`:

* `sello.py <fichero.o> [-v]` -- imprime `ALLOC`, `SYM` y `WHOLE`.
* `m.sh <unidad>...` -- compila y sella; **aborta** si el build no acaba en
  `, 0 fallidas`.
* `barrido.py <fuente> <unidad> <objeto>` -- prueba cada andamio por separado
  (pin: le quita el `asm("rN")` y deja la local; barrera: borra la linea),
  compila, sella y revierte siempre.
* `censo65.py <fuente>...` -- censo con los comentarios quitados, con las dos
  definiciones de barrera y contando aparte el `asm` con plantilla no vacia.
* `blk.py` / `ed.py` -- edicion por bloques y por lineas **en bytes**, respetando
  el final de linea de cada linea. Hace falta: `pathaction.cpp` esta en CRLF
  **menos las lineas 3 a 9**, y `spchpick.c` estaba en Latin-1.

Comparacion honesta: compilar `git show HEAD:<fichero>` y la version actual **en la
misma sesion** y enfrentar los dos sellos. Es lo unico que descarta que otro
agente haya movido el `.o` por debajo (en esta sesion habia cuatro ficheros de
`Libs/` modificados por otros agentes).
