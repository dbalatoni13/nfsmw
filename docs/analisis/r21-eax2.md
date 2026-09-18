# Ronda 21 — zEAXSound2: los tres near-miss

Base `antes_r21_eax2.json` → cierre `despues_r21_eax2.json`: **+0 B, +0 funciones,
0 unidades cambian**. `zEAXSound2` sigue en **158.400/170.256 B = 93,0364 %, 921
funciones al 100 %**. **Nada aplicado al árbol.**

`audit.py Speed/Indep/SourceLists/zEAXSound2`: **921 ok / 0 FALLA** (dos pasadas,
la segunda para confirmar, como manda el brief). Lista de FALLA: **vacía**.

Encargo **verificado y vigente** (`build_direct.py` + `triage.py` de hoy):

    7508 B  98.44433%  ResolveCarBanks__18CSTATEMGR_CarState   faltan 1, sobran 2, 1 SUST  b-1 bgt+1 ble-1
     392 B  94.93877%  Play__16CARSFX_RoadNoise…LOOPi          faltan 1, 0 sobran, 1 SUST  li+1
     340 B  95.88236%  BindToData__14GinsuSynthDataPv          faltan 0, sobran 1          mr-1

---

## 0. Lo primero que hice: bajar el ciclo de 25 s a 8 s, y en paralelo

`mn_repro.py` reproduce **byte a byte** la función real desde un stub que sólo
incluye el `.cpp`, para los tres ficheros de este encargo (`CARSFX_Roadnoise.cpp`,
`STATEMGR_CarState.cpp`, `ginsudata.cpp`). Verificado contra el `.o` de la unidad
antes de fiarme, como manda el documento de herramientas.

Sobre eso, en el scratchpad, prefijo `c21eax2_`:

| | |
|---|---|
| `c21eax2_var.py <unidad> <variante.cpp> <tag> [flags…]` | compila un stub que incluye ESE fichero con los cflags exactos de la unidad. Los flags extra van **delante** de los `-I` reales, que es lo que permite ensombrecer una cabecera |
| `c21eax2_sweep.py <variantes.py>` | barrido **en paralelo** (6 a la vez) de variantes de un bloque de fuente |
| `c21eax2_hsweep.py <variantes.py>` | igual pero de **cabecera**: cada variante escribe su `ENVIRO_AEMS.h` en un árbol sombra propio |
| `c21eax2_fn.py <obj> <símbolo>` | saca los bytes de una función de un ELF32 BE (no hay `pyelftools` ni binutils en el árbol) |
| `c21eax2_apply.py` / `c21eax2_real.py` | aplican una variante al árbol de verdad, compilan, miden y **restauran** |
| `c21eax2_list.py` / `c21eax2_cmp.py` | foto del % **por función** de la unidad y su delta — es lo que delató que `h15` rompía `InitSFX` |

**El oráculo bueno**: `c21eax2_sweep.py` compara las palabras de la función contra
las del **objeto objetivo** (`build/GOWE69/obj/…/zEAXSound2.o`) y da el número de
palabras distintas. Cuesta lo mismo que contar instrucciones y ordena las
variantes de verdad; con sólo el tamaño me habría tragado dos trampas.

**11 variantes en 19 s.** En total 100 formas de fuente medidas en esta sesión.

---

## 1. `Play__16CARSFX_RoadNoise` (392 B, 94,93877 %) — mecanismo cerrado, función no

Toda la diferencia está **dentro del constructor inline `Csis::FX_ROADNOISE`**
(filas 45-93 de 99; las 44 primeras casan). Y es una sola cosa:

**el objetivo materializa DOS ceros y nosotros uno.**

    objetivo   li r29, 0    -> volume(0x8), azimuth(0x10)  y  refCount=0 (h:491)
               li r0,  0    -> type(0x14), secondaryNoise(0x18), speed(0x1c),
                               hiPass(0x24), wetFX(0x2c)
    nuestro    li r30, 0    -> los siete campos y refCount

El corte cae **exactamente en el 5.º argumento** (`type`), y de ahí salen las
demás diferencias: con r8/r29/r0/r9/r11/r10 en el objetivo y r10/r30/r29/r11/r9/r0
en el nuestro.

### Diagnóstico de pases (sólo informativo, los cflags no se tocan)

| flag | qué sale |
|---|---|
| `-fno-rerun-cse-after-loop` | **SIETE** ceros sueltos → **cse2 es el pase que los funde** |
| `-fno-cse-skip-blocks` | el clamp en **r8, como el objetivo**, y el cero de `refCount` separado — pero 99 instrucciones |
| las otras nueve | idénticas |

O sea: cse2 funde, `cse_skip_blocks` decide si `refCount` entra en el grupo. El
objetivo está **entre las dos configuraciones**, así que el corte no es de banderas:
es de fuente.

### Vedas nuevas (todas medidas, todas revertidas)

1. **10 formas de la sentencia `new Csis::FX_ROADNOISE(...)`** — llaves del bloque
   interior, puntero temporal, `this->`, el enum casteado a `(FXROADNOISETypeType)0`,
   las constantes en decimal, `refcnt` declarado / asignado / descartado,
   `SetCsisName` dentro del bloque. **Todas byte a byte idénticas** salvo el
   puntero temporal (94 instrucciones, peor).
2. **5 formas del CFG de `Play`** — `return` temprano en la rama `cStitchLoop`,
   condición invertida con los bloques intercambiados, dos `if` sueltos en vez de
   `if/else`, `!(ID > METAL)`, comprobación de nulo antes de `GetRefCount`.
   Idénticas o peores.
3. **5 formas del preámbulo** — `if (ptr) delete ptr;` en uno, en otro y en los dos,
   `ID > 7` literal, `ID >= STITCH_LOOP`, `attribID` en línea. Idénticas.
   Los dos `delete` en orden inverso: 98 instrucciones pero con un cero solo (mal).
4. **7 órdenes de llamada de los setters en el constructor** (SetId 3.º, 4.º, 5.º,
   último, orden inverso, volume+azimuth primero). Ninguno reproduce el reparto:
   en cuanto SetId deja de ser el primero, el primer cero nace **antes** del clamp,
   y en el objetivo los dos nacen **después**.

### La trampa gorda de esta función, y por qué `InitSFX` la cierra

**Quitarle el clamp a `SetType` da EXACTAMENTE 98 instrucciones, 392 B y dos ceros**
— el tamaño del objetivo clavado. Y es falso:

    zEAXSound2 base   158.400 B  921 fns    Play 94,93877 %
    zEAXSound2 h15    157.956 B  920 fns    Play 92,38776 %

`c21eax2_cmp.py` dice quién se rompe: **`InitSFX__16CARSFX_RoadNoise`, 444 B, que
está al 100 %, baja a 99,89 %.** `InitSFX` construye otro `FX_ROADNOISE` con la
misma tirada de constantes y **fija la forma del clamp de `SetType`**. La cabecera
`ENVIRO_AEMS.h` es correcta. Es el **noveno caso medido** de «el tamaño exacto
miente», y el primero en el que la prueba es otra función de la misma unidad.

Barridas además **6 formas más del clamp de `SetType`** (asignación directa a
`mData` en las tres ramas, `if`+`if` sin `else`, sólo cota inferior, sólo cota
superior, temporal local, ternario): el ternario cae en la misma trampa (98
instrucciones), el resto idénticas.

### Lo que sí saco en limpio: la cabecera original, leída del mapa de líneas

`class FX_ROADNOISE` empieza en la línea **311** del original (coincide con el
comentario `// Decl: 311` que ya traía la cabecera). Cada campo ocupa **16 líneas**
(setter+getter); las nuestras ocupan 13. El cuerpo del setter es **el mismo
sentencia por sentencia**; las 3 líneas de más son `{` en línea propia y el
`} else if (…) {` partido en `}` / `else if (…) {`. Sitios:

    313-328  SetId/GetId      (store en 321)      …  16k
    377-392  SetType/GetType  (store en 385)
    489-497  GetRefCount      (refCount=0 en 491)
    499-502  operator new     (Alloc en 501)
    504-507  operator delete  (Free en 505-507)
    509-535  constructor      (CreateInstance en 524, reintento 529-532)
    537-541  destructor

Todo cuadra con nuestra cabecera. **No hay nada que corregir ahí.**

---

## 2. `ResolveCarBanks` (7508 B, 98,44433 %) — la diferencia es COLOCACIÓN DE BLOQUE

Corrijo la descripción de la ronda 20: no es que falte código ni que sobre un
bloque. Las ~30 filas distintas de 1891 son **tres consecuencias de una sola cosa**.

### Dónde está, al byte

La **pastilla de limpieza compartida** de `goto LoadRemainingEngines` es
**byte a byte idéntica en los dos** (8 instrucciones, dos entradas):

    .L_800D2098:  mr r3, r20        engineaudio.h:25
                  b .L_800D20A4     STATEMGR_CarState.cpp:511
    .L_800D20A0:  mr r3, r31
    .L_800D20A4:  li r4, 0 ; bl ~Instance ; mr r3, r17 ; li r4, 0 ; bl ~Instance
                  b .L_800D2770     (LoadRemainingEngines)

- **objetivo**: en `0x800D2098`, **entre la última instrucción de BeginRule2
  (`0x2094`) y la cabecera de BeginRule3 (`0x20BC`)**.
- **nuestro**: en `+0x17B8`, pegada a `LoadRemainingEngines`.

De ahí salen las otras dos:

1. En nuestra línea 283 (`goto BeginRule4` dentro del `for` de BeginRule4) el
   objetivo **conserva** la limpieza entera (2 destructores + `b BeginRule4`) y
   `jump.c` le mete una etiqueta en medio, `.L_800D2640`. Nosotros **borramos** tres
   de esas instrucciones y saltamos a una copia del final (`b +0x17A8`).
2. Y por eso, en la línea 299, el objetivo emite **`bgt .L_800D2640`** (una
   instrucción) donde nosotros emitimos **`ble +0x17dc` … `b +0x14cc`** (dos).
   **Ésos son los 4 B.**

Es decir: el objetivo borra la copia **posterior** y el `ble X; b Y; X:` que queda
lo colapsa `jump.c` a `bgt Y`. Nosotros borramos la **anterior**, que es lo que
hace `do_cross_jump` recorriendo hacia delante — el mecanismo que ya cerró la
ronda 20.

### Vedas nuevas (12 formas, oráculo = md5 de los bytes de la función)

**Todas byte a byte idénticas** salvo donde se diga:

- Cola exterior invertida (`if (<=4) goto LoadRemainingEngines; goto BeginRule4;`),
  sin llaves, con `then` vacío y `else`, con `else goto LoadRemainingEngines`.
- Cola interior invertida (r09) y las dos a la vez (r10): mueven **un** `li r4,0`
  de sitio y nada más.
- `if (n < 0) goto LoadRemainingEngines;` en la cabecera de BeginRule4.
- `int m` sacada del `for`.
- **Estructura de ámbitos** (que es lo que coloca la pastilla): etiqueta fuera del
  bloque en BeginRule3, en BeginRule4, en los dos; llaves de más alrededor de los
  dos bloques; los dos dentro de un mismo bloque; `int n` partida en declaración y
  asignación. **Las siete idénticas.**
- `BeginRule4` como `for(;;)` con `continue`/`break`: **1880 instrucciones**, peor.

### Y la sugerencia n.º 1 de la ronda 20 queda **cerrada en falso**

**Los bloques `if (DEBUG_PRINT_CAR_BANK_RESOLVE)` cuestan exactamente cero.**
Moverlos detrás de `AddMapping` (r11) y **borrarlos** (r12) dan los dos el
**mismo md5** que la base. Código muerto, RTL borrado antes del cross-jump.

### Lo que sí dice el mapa de líneas, y nadie ha usado

El `ResolveCarBanks` original es **~220 líneas de fuente más largo** que el nuestro:
el delta va de **+57** en el prólogo a **+279** en la línea 299. Dos sitios medidos:

- Entre `mapping.Finish = …` y `AddMapping(…)` el original tiene **1 línea** en los
  dos sitios de BeginRule4 (536→537 y 561→562). **Nuestros bloques `if (DEBUG_…)`
  de las líneas 271-274 y 290-293 no pueden estar ahí.** Entre `AddMapping` y el
  `std::find` el original tiene **9 y 11 líneas** donde nosotros tenemos 2: ahí es
  donde viven. Como r11 mide el traslado **byte-neutro**, es una corrección
  estructural documentada, no bytes.
- BeginRule3 tiene un hueco de **4 líneas** entre `erase(second)` (500) y el
  `if (… <= 4)` (506) que en nuestra fuente no tiene nada: casi seguro un tercer
  bloque `if (DEBUG_…)`.

---

## 3. `BindToData` (340 B, 95,88236 %) — palanca encontrada, y no vale

Leído el diff, **todo se reduce a qué registro se lleva el `-1` de `mCurrentBlock`**:

    objetivo   li r5, 0    (i)  pronto
               add r8, r10, r9  (la base del bucle nace ya en r8)
               mr r7, r0        (minperiod = mSampleCount, libera r0)
               li r0, -1        tarde, muere en el stw de 0xac
    nuestro    li r4, 0    (i)
               li r8, -1        izado a la 2.ª instrucción del bloque
               add r5, r10, r9  (la base cae en r5 porque r8 está ocupado)
               …
               mr r8, r5        <- LA instrucción que sobra

**La palanca es la posición de `this->mCurrentBlock = -1;`**: con la asignación en
cualquier punto **igual o anterior** a `this->mSampleCount = memdata->sampleCount;`,
el `-1` se va a **r0**, la base nace en **r8** y el `mr` desaparece. Sale con
**340 B, el tamaño exacto del objetivo**, y con **32 palabras distintas contra el
objeto objetivo en vez de 55**.

**Y objdiff lo puntúa peor, así que NO se aplica:**

    base (mCurrentBlock al final, como ahora)   95,88236 %   344 B
    b06 (justo tras mSampleCount)              93,61176 %   340 B
    b07 (justo tras `int i = 0`)               91,15294 %   340 B

El motivo: el `stw` de `mCurrentBlock` se va con la asignación y aterriza **antes**
de `stw …, 0x14(r31)`; el objetivo lo tiene entre `stw 0x18` y `stw 0x1c`. Décimo
caso de «el tamaño exacto miente».

**Barrido**: 9 posiciones de inserción de `this->mCurrentBlock = -1;` × 2 de
`int minperiod` × 3 de `int i` (arriba, después de las copias de campos, dentro del
`for`) = **38 variantes**. Resultado:

- La posición de `int i` es **completamente irrelevante** (idéntica en las tres).
  Eso **matiza el commit `cc1c3cf8`**: sólo importaba mientras `mCurrentBlock`
  seguía al final.
- Meseta exacta: **32** palabras distintas para toda variante con `mCurrentBlock`
  en posición ≤ `mSampleCount`, **55** para toda variante con ella después.
- `int minperiod = memdata->sampleCount;` en vez de `this->mSampleCount`: peor (39/59).

**Veda**: barridas las 9 posiciones de la sentencia `this->mCurrentBlock = -1;` y
las 3 de `int i`. La forma que da el tamaño no da el orden de los `stw`.

---

## 4. Lo que NO he probado

- **`Play`**: el permutador (ni guiado ni ciego); `regmap.py`/`lreg.py` sobre los
  pseudos del constructor; cualquier cosa que derrote a `cse_skip_blocks` en ese
  punto sin tocar la cabecera (que `InitSFX` blinda).
- **`ResolveCarBanks`**: los **dos racimos de registro** que la ronda 20 dejó
  pendientes (filas 1418-1443, `srawi r5/r6` contra `r4/r5` en el `erase` de
  UTLVector, y 1672-1696, `r22`/`r24` y `r20`/`r22`); reconstruir las ~220 líneas
  que le faltan a nuestra fuente; las colas de **BeginRule2 y BeginRule3** (sólo he
  barrido BeginRule4 y los ámbitos que lo envuelven); el permutador.
- **`BindToData`**: formas que dejen el `stw` de `mCurrentBlock` tarde moviendo
  sólo el `-1` (un `int nb = -1;` local, `~0`, una barrera `__asm__`); el permutador.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, los bloques
  `__ANDROID__`, ni ningún fichero de zEAXSound.
