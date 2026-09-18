# r53-cerca3: zAI y zGameModes

**Las dos premisas del encargo eran falsas, y las dos por la misma razón: la
métrica de `.rodata` no era el frente.**

- En **zAI** los 240 B de `.rodata` son el residuo más pequeño de la unidad. El
  bloqueo real es el **orden de emisión**: 781 de 1.030 funciones se enlazaban
  en otra dirección, 176.928 B de `.text` de 272.796. Cerrado a **688 / 135.560 B**
  con siete arreglos de fuente, sin tocar una sola función (`fncmp` 0 de 1.030
  antes y después). El octavo está medido y sin aplicar porque es cabecera
  compartida con zWorld2.
- En **zGameModes** el `+104` es real pero mide contra un objetivo equivocado:
  **`splits.txt` le quita 848 B de `.rodata` que son suyos y se los da a
  zGameplay.** La refutación de la r52 está a su vez refutada, con la prueba
  fuera de discusión (el prefijo de bWare) y con un segundo caso idéntico en
  `zEAXSound2`/`zEcstasy`. Mientras la frontera no se mueva, **zGameModes no
  puede promocionar nunca**.

---

## 1. zGameModes: `splits.txt` parte una unidad en dos

### La prueba: el prefijo de bWare marca el principio de cada TU

Cada TU que toca `bware.hpp` abre su `.rodata` con 92 B fijos
(`GAMECUBE\0\0\0\0d:/mw/speed/indep/bware/inc/bware.hpp\0…bad_alloc…STL\0`). Es
el bloque que zAI.cpp, zGameplay.cpp, zDebug.cpp y zOnline.cpp ya escriben a
mano. **Un prefijo = una unidad de traducción.**

Censo sobre la `.rodata` del ELF original (`803c8c60..8041516c`): **31 prefijos**
contra 114 rangos de `.rodata` en `splits.txt`. De los 30 rangos de SourceList,
28 empiezan exactamente en un prefijo. **Los dos que no cuadran son los dos
únicos que contienen DOS:**

```
803d9050..803dd6d8  18056 B  zEAXSound2.cpp    prefijos: 803d9050  803dd658
803dd6d8..803df568   7824 B  zEcstasy.cpp      prefijos: NINGUNO
803ebb48..803ed338   6128 B  zGameplay.cpp     prefijos: 803ebb48  803ebe98
                             zGameModes.cpp    sin rango de .rodata
```

Y las tres unidades sin rango de `.rodata` son `zGameModes`, `zOnline` y
`zFeOverlay`.

### Las tres confirmaciones independientes

1. **El contenido de cada bloque.** Tras el prefijo, cada TU emite sus claves de
   Attrib en orden. El bloque de `803EBB48` sigue con
   `Attrib::Gen::effects`, `audioimpact`, `audioscrape`, `simsurface`,
   `WorldBodyConn`, `Pkt_Body_*`, `World_UpdateBody`, `WorldEffectConn`,
   `Pkt_Effect_*`, y después `Embarrassed`…`Final`, `EmotionManagerImpl`, un
   pozo de flotantes, `EmotionalResponse` y `3f800000`.
   El bloque de `803EBE98` sigue con **`Attrib::Gen::gameplay`** — y nuestro
   objeto de zGameplay emite exactamente esa secuencia (`$LC158
   'Attrib::Gen::gameplay'` justo detrás de `Attrib::Blob`).

2. **El objeto extraído de zGameplay lo dice él solo.** Su primer símbolo de
   `.rodata` es `pad_05_803EBB48_rodata` de **840 B** — un bloque que `dtk` no
   supo atribuir a nadie — seguido de `lbl_803EBE90` (4 B) y de `lbl_803EBE94`,
   96 B que **empiezan por `00000000 47414d45 43554245`**: el relleno más el
   `GAMECUBE` de zGameplay.

3. **El único UNDEF de zGameModes cae DENTRO del bloque, no después.** La r52
   escribió: «`lbl_803EBE90` es la dirección justo DESPUÉS de la zona
   disputada». La zona disputada estaba mal acotada: la r50 la cerró en
   `0x803EBE90` y en realidad termina en `0x803EBE98`, donde empieza el
   siguiente `GAMECUBE`. Con el final bueno, `lbl_803EBE90` está **8 B antes del
   final del propio bloque de zGameModes** — es su último literal vivo:

   ```
   # fndiff zGameModes __static_initialization_and_destruction_0   (100 %)
     8 lis  r9, lbl_803EBE90@ha        | lis  r9, $LC79@ha
    11 lfs  f0, lbl_803EBE90@l(r9)     | lfs  f0, $LC79@l(r9)
   ```

   Nuestro `$LC79` es el **último** símbolo de nuestra `.rodata` (offset 224 de
   232) y vale `3f800000`. El del objetivo es el último del bloque de 848 B.
   Es la misma constante en la misma posición relativa.

   El argumento de la r52 —«no referencia ni uno de esos 840 B, luego no son
   suyos»— se invierte: **son 848 B de residuo muerto, y lo muerto no se
   referencia por definición.** Lo único vivo del TU es el `1.0f`, y a ése sí
   apunta.

### Por qué esto bloquea la promoción

`zGameModes` emite 232 B de `.rodata`; el estripado se lleva `size & ~7` = 128 y
quedan 104 (68 B de colas, 32 de relleno entre símbolos y los 4 B vivos del
`1.0f`). Eso está bien medido por la r50/r52. Lo que no estaba medido es contra
qué: el objeto extraído tiene **cero** `.rodata` porque `splits.txt` no le da
rango, así que **el único delta 0 posible sería emitir cero bytes** — y no se
puede, porque el `1.0f` está vivo. Con la frontera mal puesta, `+104` no es un
resto: es un suelo.

### Las líneas para `splits.txt` (son tuyas)

```
Speed/Indep/SourceLists/zGameModes.cpp:
	.text       start:0x8019A1F0 end:0x8019A26C
	.ctors      start:0x803C8BA0 end:0x803C8BA4
	.rodata     start:0x803EBB48 end:0x803EBE98      <-- NUEVA
	.bss        start:0x80473EF8 end:0x80473F00

Speed/Indep/SourceLists/zGameplay.cpp:
	.rodata     start:0x803EBE98 end:0x803ED338      <-- era 0x803EBB48
```

y el segundo caso, misma forma (la cola de un rango es la cabeza del siguiente):

```
Speed/Indep/SourceLists/zEAXSound2.cpp:
	.rodata     start:0x803D9050 end:0x803DD658      <-- era 0x803DD6D8
Speed/Indep/SourceLists/zEcstasy.cpp:
	.rodata     start:0x803DD658 end:0x803DF568      <-- era 0x803DD6D8
```

En zEAXSound2 la prueba es idéntica y aún más directa: el **último** símbolo de
`.rodata` de su objeto extraído es `lbl_803DD658`, **128 B que empiezan por
`GAMECUBE`** y siguen con `High`, `Medium`, `Low`, `Reflection`. Y `zEcstasy.cpp`
ya escribe a mano bloques a partir de `lbl_803DD6D8`, o sea 128 B por detrás de
donde empieza de verdad.

**El movimiento de frontera no toca el DOL**: las dos parejas son adyacentes en
el orden de enlace y los bytes se quedan donde están; sólo cambia de qué objeto
extraído salen. No es el caso de `nfsmw-rango-no-basta.md` (allí el rango era
huérfano y cambiaba de posición).

### Aritmética después de la corrección

| unidad | `.rodata` objetivo hoy | de verdad | linkdelta hoy | por qué |
|---|---:|---:|---:|---|
| zGameModes | 0 | **848** | `+104` | pasaría a `−744`: le faltan 616 B de vocabulario |
| zGameplay | 6128 | **5280** | `−896` | de esos 896, **848 son ajenos**: lo suyo son 48 B |
| zEAXSound2 | 18056 | **17928** | `−176` | 128 son ajenos: lo suyo son 48 B |
| zEcstasy | 7824 | **7952** | `−288` | pasaría a `−416`, y 128 son su prefijo |

Y hay trabajo hecho en el sitio equivocado: el bloque a mano de
`zGameplay.cpp` (100 cadenas) tiene **29 cadenas, 409 B, que están en el bloque
de zGameModes** (`Attrib::Gen::effects`, `WorldBodyConn`, `Pkt_Body_*`,
`Embarrassed`…`Final`…), y su prefijo de bWare está clavado en
`lbl_803EBB48`, que es la dirección de zGameModes. Si se mueve la frontera, esas
29 cadenas y el prefijo tienen que volver a zGameModes o habrá símbolo duplicado.

### La receta para cerrar zGameModes (una ronda, si se mueve la frontera)

El TU tiene que emitir los 848 B exactos y nada más. Como cc1plus emite sus
propios `$LC` desde el código, hay que hacer lo que ya hace `zMisc.cpp`: un
bloque `asm` con los 848 B **al principio del fichero** y macros de
desplazamiento para que el código no cree literales nuevos. Son 15 cadenas y una
constante:

```
lbl_803EBB48 +0x000  prefijo bWare (92 B)
             +0x21C  "Embarrassed"     +0x228 "Anxious"    +0x230 "Smug"
             +0x238  "Ecstatic"        +0x244 "PursuitBegins"
             +0x254  "PursuitAddsCar"  +0x264 "PursuitAddsHeli"
             +0x274  "PursuitAddsRoadblock"  +0x28C "PursuitEnds"
             +0x298  "Arrested"        +0x2A4 "Initial"    +0x2AC "RampUp"
             +0x2B4  "Peak"            +0x2BC "RampDown"   +0x2C8 "Final"
             +0x348  1.0f   <-- el unico VIVO: es lbl_803EBE90
```

El `+0x348` sale igual escrito como `lbl_803EBB48 + 0x348`, así que
`__static_initialization_and_destruction_0` queda byte a byte. Los 848 B enteros
necesitan entrada en `keep.lst` (el estripado se los llevaría: el original los
conserva igual que conserva `lbl_803C8C60` en zAI).

Además, `src/Speed/Indep/Src/Gamemodes/EmotionManagerImpl.cpp` **está vacío** (0
líneas) y no está incluido en `zGameModes.cpp`. La cadena `EmotionManagerImpl`
está en el bloque, en `+0x2D8`.

**No he tocado nada de zGameModes.** `fncmp` 0 de 2 antes y después,
`linkdelta` `rodata+104` sin cambios.

---

## 2. zAI: el frente no era la `.rodata`, era el orden de emisión

### La medida que faltaba

`.text +0` y `fncmp` 0 de 1.030 **no quieren decir que el `.text` case**: es una
permutación, y el total no cambia. Enlazando y comparando la dirección de cada
función contra el objeto extraído:

```
                                  antes    ahora   (con el parche de zWorld2)
funciones en otra direccion        781      688       592   de 1030
bytes de .text que arrastran   176.928  135.560   111.512   de 272.796
fuera del orden (permorden)        182      169         -   de 1030
```

Los 240 B de `.rodata` son el 0,09 % del problema. **zAI no está a 240 B de
promocionar; está a 688 funciones.**

Cómo se reproduce (los dos guiones están en mi scratchpad, no en `scripts/`,
porque son de un solo uso; el segundo es `permorden.py` con el detalle por
función):

```
dirfn.py <u>     enlaza y compara la DIRECCION de cada funcion  -> el numero que manda
invorden.py <u>  lista, en orden nuestro, las que caen fuera    -> la lista de trabajo
ventana.py <u> A B   los dos ordenes lado a lado entre A y B    -> el arreglo exacto
```

### Las dos formas del error, y las dos son de fuente

`invorden` separa solas las 169 que quedan:

- **delta pequeño (±1..±10): el orden de definición dentro de un `.cpp`.** GCC
  2.9 emite en orden de fuente; si el original define `A` antes que `B` y
  nosotros al revés, se ve. Se arregla moviendo el bloque.
- **delta grande (+100..+550): la función tenía que ser `inline`.** Un cuerpo
  dentro de la clase no emite símbolo ahí: sale al final, en el bloque de
  `finish_file`, entre los `_IHandle__*` y los `_._` de plantilla. Cuando
  `invorden` dice `nue#241 -> obj#938` y en `obj#939` está una función que
  nosotros **ya** tenemos en clase, el diagnóstico es directo.

Es `nfsmw-en-clase-es-inline.md` leído al revés: aquella nota costó −18
funciones por meter cuerpos en la clase; aquí el mismo hecho **localiza** las que
faltan por meter.

### Lo aplicado (7 arreglos, todo en ficheros que sólo incluye zAI)

| # | qué | dónde |
|---|---|---|
| 1 | `IsPlayerSteering__14AIVehicleHuman` **a la clase** (obj#938, justo antes de `GetAiControl`, que ya estaba en clase) | `AI/AIVehicle.h` + `AI/Common/AIVehicle.cpp` |
| 2 | quitar `inline` a `AIVehicle::SetReverseOverride` (el objetivo la emite en su sitio de fichero, obj#272) | `AI/Common/AIVehicle.cpp` |
| 3 | reordenar `UpdateTargeting`, `WorldCollision`, `OnCollision`, `GetWorldAvoidanceInfo` | `AI/Common/AIVehicle.cpp` |
| 4 | `SetTrafficPattern` antes de `RandomSortTC` | `AI/Activities/AITrafficManager.cpp` |
| 5 | `ComputeSpeed`, `UpdateNavPos`, `ShouldPullOver`, `Update` | `AI/Actions/AIActionTraffic.cpp` |
| 6 | `Construct` antes de `OnBehaviorChange`/`OnTask`; `UpdateNavPos` antes de `CheckSpeedTraps` | `AI/Actions/AIActionRace.cpp` |
| 7 | tres intercambios: `CanSeeTarget`/`WatchForPerps`, `Update`/`UpdateSiren`, `UpdateFuel`/`CanSeeTarget`; y `AIGoalRacer::Construct` y `AIPursuit::AttemptingToReAquire` **a la clase** | `AIVehicleCopCar.cpp`, `AIVehiclePursuit.cpp`, `AIVehicleHelicopter.cpp`, `AIGoal.cpp`, `AIPursuit.h`+`AIPursuit.cpp` |

Ningún cuerpo de función cambia: sólo se mueven. Comprobado en cada paso:
`fncmp` **0 de 1.030**, `linkdelta` **`.text +0 rodata−240 data−384`** y
`rodorden` **97 fuera de 217**, los tres idénticos al principio y al final.

**Aviso de cabecera compartida**: `AI/AIVehicle.h` lo incluye también
`World/Common/WRoadNetwork.cpp`, que es de **zWorld2**. El cambio es inerte allí:
ni `zWorld.o` ni `zWorld2.o` contienen una sola referencia a `14AIVehicleHuman`
(`grep -ac` = 0 en los dos), y GCC sólo emite la copia fuera de línea de una
`inline` cuando se usa. `AI/AIPursuit.h` no se incluye fuera de `Src/AI/`.

### Lo medido y NO aplicado: `WRoadElem.h` (vale 96 funciones y 24 kB)

`GetBits__C9WRoadLaneii` y `GetBitsSigned` son hoy `nue#323/324 -> obj#707/708`,
y **son el arranque de la cascada de direcciones**: todo lo que va detrás de
`0x80024a6c` se desplaza por ellas. En el objetivo caen entre `TypeName__13VehicleParams`
y `_._19WRoadNavWithCookies`, o sea en el bloque de `finish_file`: eran `inline`.
La propia cabecera lo dice — `// TODO inline`.

```c
// src/Speed/Indep/Src/World/WRoadElem.h, dentro de struct WRoadLane
    unsigned int GetBits(int n_offset, int n_bits) const {
        return (nBits >> n_offset) & ~(~0 << n_bits);
    }
    int GetBitsSigned(int n_offset, int n_bits) const {
        n_bits = 32 - (n_offset + n_bits);
        return (static_cast<int>(nBits) << n_bits) >> (n_offset + n_bits);
    }
```
(y borrar los dos cuerpos de `AI/Common/AIVehicle.cpp:2369`).

**Medido con el cambio puesto**: `688 -> 592` funciones mal colocadas,
`135.560 -> 111.512 B`, `fncmp` sigue 0 de 1.030, `linkdelta` sin cambios.
**Revertido** porque `WRoadElem.h` es de zWorld2 y `world` la tiene esta ronda.

Lo que hay que comprobar antes de aplicarlo (yo he llegado hasta aquí):

- Un TU de prueba que sólo llama a `WRoadLane::GetType()` **sí** emite su propia
  copia de `GetBits__C9WRoadLaneii`, con **binding WEAK** (no hay símbolo
  duplicado; el enlazador se queda con una).
- El objeto extraído de zWorld2 tiene `GetBits__C9WRoadLaneii` **UNDEF**, o sea
  que en el original la definición que sobrevivió es la de zAI — que va **antes**
  en el orden de enlace. La copia sobrante de zWorld2 queda sin referenciar y el
  estripado se la lleva. Encaja, pero **hay que verificarlo reconstruyendo
  zWorld2**, y eso pisa al agente `world`.

### Lo que queda en zAI después de esto

169 funciones fuera de orden (con el parche de `WRoadElem.h`, 167 y la cascada
arrancaría mucho más tarde). El reparto por forma:

- **~120 son plantillas e `inline` del bloque de `finish_file`**
  (`find__H2Z…`, `reserve__Q24_STLt6vector2…`, `clear__Q24_STLt10_List_base2…`,
  `ClassKey__Q36Attrib3Gen…`, los `_._` de destructores). Ahí el orden lo fija
  **cuál las instancia primero**, no el orden de fichero: es el frente de
  `nfsmw-orden-de-emision.md` y no se arregla moviendo bloques.
- **~45 son de las dos formas ya resueltas** (orden de definición y cuerpo a la
  clase), y `invorden.py` + `ventana.py` las dan una a una con el arreglo exacto.
  Es transcripción.

`trypromo` sigue **DOL ROTO**; era imposible que no lo estuviera con 688
funciones en otra dirección. La cifra que mide el avance es `dirfn`.

---

## 3. Lo que NO he tocado

- `splits.txt`, `keep.lst`, `configure.py`, `config/GOWE69/*`: nada. Las líneas
  de `splits.txt` están arriba; **el paquete de zGameModes es atómico** (los tres
  cambios —frontera, bloque de 848 B en `zGameModes.cpp` y devolución del prefijo
  y las 29 cadenas por parte de `zGameplay.cpp`— van juntos o el símbolo
  `lbl_803EBB48` queda duplicado).
- `zGameModes`: ni una línea. No tenía sentido quitarle 104 B contra un objetivo
  que no es el suyo.
- `WRoadElem.h`: revertida tras medir; el fichero está como estaba.
- No he añadido ni quitado un solo literal en zAI, así que **no hace falta
  `lcfix`** (comprobado: `rodorden` da los mismos 97 fuera de 217 antes y
  después, y `linkdelta` el mismo `rodata−240`).

## 4. Números finales

```
zAI          .text +0  rodata-240  data-384   (SIN CAMBIO, como debe ser)
             fncmp 0 de 1030                  (SIN CAMBIO)
             direcciones malas   781 -> 688   (-93 funciones, -41.368 B de .text)
             fuera de orden      182 -> 169
             con WRoadElem.h            592   (medido y revertido)

zGameModes   .text +0  rodata+104             (SIN CAMBIO: no la he tocado)
             fncmp 0 de 2
             objetivo REAL de .rodata: 848 B, no 0
```
