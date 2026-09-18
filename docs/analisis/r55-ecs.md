# r55 — zEcstasy: las tres líneas a `extern`, y la medida que no las ve

Agente `ecs`. Unidad en propiedad: **zEcstasy**. Ninguna cabecera compartida tocada.

**Resumen en tres líneas.** Las tres líneas del censo eran correctas y están **aplicadas**: los
tres símbolos han vuelto a su dueño original, medido símbolo a símbolo en el ELF enlazado
(de **+312.120 / +311.996 / −15.600 B fuera de sitio** a la deriva ambiental de −360). Y la
sorpresa, que afecta a cinco agentes más de esta ronda: **`linkdelta`, `dolwhere` y el conteo
de bytes distintos del DOL son todos CIEGOS a un arreglo de cambio de dueño** — los tres dan
exactamente el mismo número antes y después. zEcstasy sigue `DOL ROTO`, y no por esto.

---

## 1. Qué se ha aplicado

Tres palabras, en dos ficheros que sólo entran en zEcstasy (comprobado: `EcstasyE.cpp` y
`EmitterSystem.cpp` sólo los incluye `SourceLists/zEcstasy.cpp`; `TextureInfoPlat.cpp` sólo
`zPlatform.cpp`):

| fichero:línea | antes | después |
|---|---|---|
| `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp:168` | `SlotPool *eAnimTextureSlotPool;` | `extern SlotPool *eAnimTextureSlotPool;` |
| `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp:241` | `TextureInfo *pTexPrev;` | `extern TextureInfo *pTexPrev;` |
| `src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp:803` | `bool EnableParticleSystem = true;` | `extern bool EnableParticleSystem;` |

## 2. El dueño en el ORIGINAL, comprobado antes de tocar

Leyendo la tabla de símbolos de `build/GOWE69/obj/Speed/Indep/SourceLists/*.o`:

    obj/zEcstasy.o    eAnimTextureSlotPool  UND      <- el original NO lo define
    obj/zEcstasy.o    pTexPrev              UND
    obj/zEcstasy.o    EnableParticleSystem  UND
    obj/zPlatform.o   eAnimTextureSlotPool  .data +0xE0  sz=4
    obj/zPlatform.o   pTexPrev              .data +0xE4  sz=4
    obj/zMiscSmall.o  EnableParticleSystem  .data +0xBC  sz=4

El aviso del encargo se confirma: **`EnableParticleSystem` lo define `zMiscSmall`**, y ahí lo
escribe a mano el `asm()` de `SourceLists/zMiscSmall.cpp` (`.globl EnableParticleSystem` /
`.4byte 0x00000001`), no un `.cpp`. Y hay un detalle que no estaba en el censo: en el original
los dos punteros de `zPlatform` viven en **`.data`**, no en `.bss` — nosotros los teníamos en
`.bss` porque los escribíamos sin inicializador. La corrección los devuelve a `.data` de
`zPlatform`, que ya los tiene con `= 0`.

Después de la edición nuestro `src/zEcstasy.o` es **idéntico al extraído** en los tres:
`UND, bind=GLOBAL, type=NOTYPE`.

## 3. La medida — y por qué las herramientas de siempre no la ven

`fncmp` de zEcstasy, **antes y después, idéntico** (sin regresión):

    2072  epCalculateLocalDirectionalPOS16   164 insn, 10 reub
    2044  UpdatePlatInfo__27eLightMaterialPlatInterface   27 insn, 50 reub
     268  eProject__FfffPA3_fPfN44            13 insn,  6 reub
    3 de 539 funciones con el CODIGO distinto -- 4384 B

| medida | ANTES | DESPUÉS | ¿ve el arreglo? |
|---|---|---|---|
| `linkdelta zEcstasy` | `.text +0  rodata−416 data+64 bss+32` | **idéntico** | **NO** |
| `dolwhere` | `LAS SECCIONES NO COINCIDEN` + 4 líneas | **byte a byte idéntico** | **NO** |
| bytes distintos del DOL | 487.266 | 487.299 (**+33**) | **NO** (y engaña) |
| `trypromo` | DOL ROTO `fd06505ea73d` | DOL ROTO `d8ae4a621fd1` | sólo cambia el sha |
| dirección del símbolo en el ELF | ver abajo | ver abajo | **SÍ** |

La única medida que lo ve es comparar la **dirección de cada símbolo** de dos enlaces
promocionados contra `build/GOWE69/main.elf`:

| símbolo | base | ANTES | DESPUÉS |
|---|---|---|---|
| `pTexPrev` | `0x8041F80C` | `0x8046BB44` (**+312.120**) | `0x8041F6A4` (−360) |
| `eAnimTextureSlotPool` | `0x8041F808` | `0x8046BAC4` (**+311.996**) | `0x8041F6A0` (−360) |
| `EnableParticleSystem` | `0x8041EE38` | `0x8041B148` (**−15.600**) | `0x8041ECD0` (−360) |

Los −360 **no son error**: son la deriva ambiental que arrastra el déficit de `.rodata` que
sigue abierto (−416 B, redondeado por alineación); la comparten 1.671 símbolos de `.data`.

**Huella total de la edición**: 1.140 símbolos cambian de dirección entre el enlace ANTES y el
DESPUÉS, repartidos en el rango base `0x8041B148 .. 0x804B9FAC` (**650.852 B**). Los
desplazamientos son −8 (973 símbolos), −4 (121), +4 (43) y los tres saltos grandes. La cifra
de 328.484 B del censo era la *región unión* estimada; el radio real medido es mayor y el
número de símbolos afectados, 1.140.

**Por qué `linkdelta` no lo ve, y es una identidad, no mala suerte.** Antes de la edición el
símbolo lo definían dos objetos; ganaba el nuestro y la copia del dueño legítimo se quedaba sin
referencias, así que `-strip-unused-data` se la llevaba. Total de bytes: el mismo. Lo que
cambia es **en qué región del binario cae el símbolo**, y el tamaño de sección no tiene forma
de contarlo. **Aviso a `grid`, `frontend`, `eax`, `speech` y `zai`**: si vuestra edición de
dueño sale con `linkdelta` idéntico y `dolwhere` idéntico, **eso es lo esperado, no un
fracaso** — hay que comprobarlo con la dirección del símbolo.

### Cómo se mide (no he dejado sonda en `scripts/`)

Se enlaza dos veces con `trypromo.objetos_del_enlace()` sustituyendo la unidad, se leen las
`.symtab` de los dos ELF y de `build/GOWE69/main.elf`, y se compara la dirección de cada
símbolo común. Propuesta: si alguien la quiere en el árbol, es una opción `--dueno <simbolo>`
para `movidos.py`, que ya hace los dos enlaces.

## 4. El residuo de zEcstasy, y qué lo explica

`reorden.py zEcstasy`:

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .data         368      975       33          0       43
    .rodata       581      119        0          0      554
    .text         538    35335     3685        157        0

    157 palabras de CONTENIDO real, TODAS en las tres funciones conocidas.

O sea: **de código no falta ni una palabra fuera de esas tres funciones**. Y el `CONTENIDO 0`
de `.rodata` es el falso 0 que avisa el brief — 554 etiquetas sin pareja.

`permorden.py`: **505 de 539 funciones en su sitio, 34 desplazadas**, 18 ciclos no triviales
(el mayor de 199 funciones, arrancando en `InitSlotPoolsEx` / `eResetTevSwapStages`). Emitimos
107 funciones que el objetivo no tiene y el enlazador estripa.

`movidos.py`: 13.621 símbolos cambian de dirección, **13.098 son arrastre**; quedan **523
permutados de verdad** (.data 317, .text 100, .bss 80, .rodata 23).

`rodatagaps.py`: `.rodata` del objeto — objetivo 7.952 B, nuestro **13.224 B**, 553 literales
vivos emparejados. Hay 12 huecos `PREFIJO` accionables con `genrodata --anonimo` (780 B en
total) **pero también huecos donde emitimos muchísimo de más** (hueco 26: objetivo 20 B /
nuestro 1.764; hueco 29: 32 / 1.408; hueco 42: 4 / 1.148). El déficit de −416 B en el ENLACE es
el neto después del estripado, no una lista de cadenas que falten: es **orden de pool**, no
contenido ausente. **No lo he tocado**: cada bloque anónimo mueve los `$LC` y puede tirar
funciones que hoy casan, y zEcstasy no puede promocionar de todas formas mientras haya tres
funciones sin casar.

### Un falso positivo que conviene que quede escrito

El diff de símbolos *definidos* entre `obj/zEcstasy.o` y el nuestro dice que el original define
y nosotros no: `DefaultLightMaterial`, `TestMWPulseBrightness`, `__sync_token` y cuatro `lbl_`
de `.rodata`. **Es mentira**: los definimos los tres, pero con `bind=LOCAL`, porque en la
fuente son `static` (`eLight.cpp:28`, `eVisualTreatment.cpp:39`, `EcstasyE.cpp:255`). `dtk`
marca **GLOBAL** los estáticos del objeto extraído. Cualquier barrido de «símbolos que el
original define y nosotros no» tiene que **ignorar el binding** o inventará déficits.

## 5. Las tres funciones abiertas

* `epCalculateLocalDirectionalPOS16` (2.072 B) y `eProject` (268 B): **vetadas con prueba** en
  r54 y r53. No las he tocado.
* `UpdatePlatInfo__27eLightMaterialPlatInterface` (2.044 B) — la única sin veda. Está al
  **99,33 %** y sus 27 diferencias son **todas de reparto de registro de coma flotante**
  (`f1` contra `f31`, `f5` contra `f21`, `f2` contra `f30`…), más **un bloque de planificación**
  en `+0534..+0548`: el objetivo intercala `lis`/`lfs` por pares y nosotros agrupamos tres
  `lis` y luego tres `lfs`. La **primera** diferencia (`+0180`) ya es de registro, así que
  según la nota «si el pin empeora, es síntoma» aquí sí procede la palanca del **pin de
  registro** (`register float x asm("frN")`) o el permutador guiado. Es trabajo de permutador,
  no una línea, y no cabía en este encargo.

## 6. Lo que queda para cerrar zEcstasy

1. `UpdatePlatInfo` con pin de registro / permutador — 2.044 B de `matched_code`.
2. Las dos vedas (`epCalculate`, `eProject`) siguen en pie.
3. Los −416 B de `.rodata`, que son **orden de pool**, no contenido.
4. Las 34 funciones desplazadas de `.text` (18 ciclos), aunque `.text` mida `+0`.

Nada de esto es de una línea, y ninguna de las cuatro sola promociona la unidad.
