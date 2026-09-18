# Ronda 21 — zCamera

Base y final **idénticos**: `measure.py --cmp antes.json final.json` -> **+0 B, +0
funciones, 0 unidades cambian**. `frozen.py chk` -> **idéntico al congelado**
(`e2b057af9b3df178`, el de la r19). El árbol se queda **byte a byte como lo
encontré**; todo lo que se midió está abajo con su cifra.

    zCamera   113.080/125.008 B   90,4582 %   446 funciones al 100 %

## 0. El encargo ESTÁ VIGENTE (a diferencia de tres de la r20)

`build_direct.py zCamera` + `triage.py zCamera` reproducen el brief **clavado**:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST   addi-4
    1156 B  96.159%  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2...  faltan 5, sobran 4, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1, sobran 0, 0 SUST   li+1
     --> 3 funciones, 4.928 B

    MURO: 4 funciones, 7.000 B
    3868 B  99.907%  Update__8ICEMoverf                     15 registros
    1192 B  99.077%  TerrainVelocityNoise__11CameraMover... reorden local, dmax 2
     992 B  99.569%  Update__19TrackCarCameraMoverf         18 registros
     948 B  99.156%  Update__19TrackCopCameraMoverf         reorden local, dmax 2

4.928 + 7.000 = **11.928 B**, la cifra del brief.

## 1. `audit.py` — LIMPIO, dos pasadas

`audit.py Speed/Indep/SourceLists/zCamera`: **446/446 ok, cero FALLA**, y la
segunda pasada da un fichero **byte a byte idéntico** a la primera (sin
fantasmas del `build/` compartido). **Lista de FALLA: vacía.**

## 2. `__static_initialization_and_destruction_0` (3.604 B) — MECANISMO IDENTIFICADO

### Qué son exactamente las 4 instrucciones

Las 4 que sobran son `addi r27,r30,0x30 / 0x48 / 0x60 / 0x78`: la dirección de
`ICE::ReplayCategoryTable[2..5]` materializada en un pseudo. El objetivo pliega
el desplazamiento en cada `stw` (`stw r9,0x40(r30)`), nosotros guardamos la base
del elemento en r27 y usamos `0x10(r27)`. Los elementos **0 y 1 casan byte a
byte**; del 2 al 5 no. Todo lo demás de la función (incluida la permutación de
registros de los índices 549-564, `li r9,0x38e` contra `lis r9,$LC1114@ha`) es
consecuencia de esos 4 pseudos de más: son 4 allocnos extra que corren la
numeración.

### La causa: `-fforce-addr` MÁS un umbral POSICIONAL de CSE

**Barrido de banderas sobre el TU real** (compilando `zCamera.cpp` a `.s` en el
scratchpad, 14 s cada una; la cuenta es de `addi rN,<base>,K` dentro de la
ventana de la tabla):

| bandera | insns fn | addi de más |
|---|---|---|
| base | 905 | **4** |
| `-fno-gcse` | 899 | 4 |
| `-fno-rerun-cse-after-loop` | 1380 | 1 |
| `-fno-cse-follow-jumps` / `-fno-cse-skip-blocks` | 905 | 4 |
| `-fno-expensive-optimizations` | 905 | 4 |
| **`-fno-force-addr`** | 899 | **0** |
| `-fno-schedule-insns2` / `-fno-move-all-movables` / `-fno-rerun-loop-opt` / `-fno-force-mem` / `-fno-strength-reduce` / `-fno-thread-jumps` | 905 | 4 |

O sea: **`-fforce-addr` es quien crea el pseudo** (`copy_to_reg` de la dirección
del agregado). Pero eso no lo explica solo, porque CSE lo repliega en los
elementos 0 y 1 — y en un caso mínimo, en los seis.

**Sonda posicional sobre el TU real.** Insertando N globales de inicialización
dinámica (`int c21probe_k = c21probe_src + k;`) **justo antes** de la tabla, en
`ICEReplay.cpp`:

| sonda | insns fn | addi de más | elementos afectados |
|---|---|---|---|
| 0 | 905 | 4 | 2,3,4,5 |
| 1 | 909 | 4 | 2,3,4,5 |
| 2 | 913 | 5 | 1,2,3,4,5 |
| 3 | 916 | 5 | 1..5 |
| 4 | 919 | 5 | 1..5 |
| 6 | 925 | 5 | 1..5 |
| 8 | 931 | 5 | 1..5 |
| **10** | 932 | **0** | — |
| 12,14,16,18,20,24,28 | 940..1045 | **0** | — |

Y las **mismas** sondas puestas **DESPUÉS** de la tabla (4, 10, 16) dan **4, 4,
4**: ningún efecto. Es un umbral que sólo cuenta el **prefijo** de instrucciones
RTL de la función hasta la tabla.

**Reproducido fuera del árbol** (TU sintético con N globales `V3` seguidos de la
misma tabla de 6 `ReplayCategory`, compilado con los mismos flags): la ventana
mala existe **una sola vez**, en N=115..122 (1.125..1.199 insns), creciendo hacia
atrás igual que arriba (N=115 sólo el elemento 5; N=118 los 3,4,5; N=122 los
1..5) y desapareciendo en N=125. Fuera de esa ventana (N=0..110 y N=125..225) los
seis elementos pliegan. **No es periódico**: probados N=20..34 y N=205..225, cero.

La lectura: hay un punto fijo K en la cuenta de insns RTL de la función a partir
del cual CSE deja de saber que un registro contiene `ReplayCategoryTable` (el
`flush_hash_table` de `cse_basic_block`). Si K cae **antes** de que se materialice
la base (`addi r30,r10,table@l`) todo vuelve a plegarse, porque CSE la reaprende;
si cae **dentro** de la tabla, todos los elementos posteriores materializan su
`this`. Nosotros tenemos K entre el elemento 1 y el 2.

### Por qué NO se puede arreglar desde `ICEReplay.cpp`

Nuestra función **coincide instrucción a instrucción con el objetivo desde el
índice 0 hasta el 795** (donde empieza la tabla, que es lo último que hace la
función: el índice 899 es el último `stw` y detrás sólo viene el epílogo). Para
mover K hasta antes de la base harían falta **~27 insns RTL más antes de la
tabla** — insns que **mueren** antes del código final, porque el código final ya
casa 1:1. Añadir un global inventado para eso sería falsificar la fuente, igual
que escribir ensamblador. La diferencia real está en **cómo escribió el original
alguna sentencia anterior** (una forma que expande más RTL y se pliega al mismo
código), y `dwbody.py` ya dice que el **árbol de inlines es idéntico** (r19,
302 vs 302), así que no se ve por ahí.

**Veda, con la sentencia barrida:** barrido el conjunto y la forma de la propia
tabla `ICE::ReplayCategoryTable[6] = { ReplayCategory(...) x6 }` y de su
constructor — el caso mínimo con esa misma fuente da **la forma del objetivo**,
luego la fuente de la tabla NO es el problema. Lo que decide está **antes**, en el
prefijo de la función.

### La palanca, para quien la retome

Cualquier cosa que meta **~27 insns RTL que mueran** antes de `ICEReplay.cpp` en
el orden de construcción cierra los 3.604 B. Herramienta para verificarlo en 14 s
sin tocar el árbol: compilar `zCamera.cpp` con `-S` al scratchpad y contar
`addi rN,r30,{48,72,96,120}` dentro de `__static_initialization_and_destruction_0`.

## 3. `LoadCameraShakes` (168 B) — el DWARF CONFIRMA `warned_overflow`, y aun así no cierra

`mw_dwarfdump.nothpp:344295` da el cuerpo del original:

    void ICEManager::LoadCameraShakes(struct bChunk * set_chunk /* r29 */) {
        unsigned int id;
        bool warned_overflow;      // r0     <-- EXISTE, y vive en r0
        struct bChunk * chunk;
        inline unsigned int bChunk::GetID() {}          // rango CERO
        { unsigned int * p_handle;  ICEShakeGroup * group; // r31
          { int num_tracks; // r28
            ICEShakeTrack * track; // r30
            { int i; // r27   ... } } }

O sea: la hipótesis de la r19 era **correcta** (el parámetro se llama
`set_chunk`, hay un `id` muerto, un `chunk` muerto y un `warned_overflow` en r0).

**Pero el `li r0,0` que sobra lleva la línea `ICEManager.cpp:6816`, que es la
línea del `for`**, no la de una sentencia `warned_overflow = false`. Y `li r27,0`
lleva **la misma** línea. Son los dos ceros del `for`: el del contador y el del
test de entrada (`cmpw r0,r28`, sin canonicalizar, o sea comparación
registro-registro). `warned_overflow` acaba en r0 porque comparte cantidad con
ese cero, no porque su asignación sobreviva.

Ensayos (todos revertidos):

    v1  `int i = 0;` fuera del for, `for (; i < num_tracks; i++)`      96,6667  (sin efecto)
    v2  `int i = 0;` + `while (i < num_tracks) { ... i++; }`           96,6667  (sin efecto)
    v3  `for (int i = 0; num_tracks > i; i++)`                         91,3095  (PEOR)
    v4  v1 + `num_tracks > i`                                          91,3095  (PEOR)
    w1  `unsigned int id = set_chunk->GetID(); bool warned_overflow = false; bChunk *chunk;`
        al principio, y parámetro renombrado a `set_chunk`             96,6667  (sin efecto)
    w2  sólo `bool warned_overflow = false;` al principio              96,6667  (sin efecto)
    w3  `bool warned_overflow = false;` justo antes de `group`         96,6667  (sin efecto)
    w4  w1 + `warned_overflow = false;` repetido tras `group`          96,6667  (sin efecto)

**Veda ampliada.** Sentencias barridas: la **forma del bucle** (`for` con
declaración interna, `for` con `i` fuera, `while`, y el orden de la comparación
`i < n` contra `n > i` — este último **canonicaliza y empeora**), y la
**declaración e inicialización de `warned_overflow`** en cuatro sitios distintos.
GCC lo borra por muerto antes de que CSE pueda usar su cero, en los cuatro.
Sigue sin aparecer un uso real de `warned_overflow` que se pliegue a cero
instrucciones.

## 4. `_Storage<Director*,2>` copia (1.156 B) — releído, no reabierto

El diff actual tiene tres grupos:

1. **orden de los `stw` de cabecera**: el objetivo pone `stw r11,0xc(r31)`
   **antes** de los tres ceros (`0x0/0x4/0x8`), nosotros después. (muro 3 de r19)
2. **el pliegue de `minSize = srcEnd - srcBeg`** y el `mr. r27,r0` que se pierde
   (muro 1 de r19), más un `cmpwi r27,0` de más por nuestro lado.
3. **orden de operandos en dos comparaciones**: objetivo `cmplw r26,r4` + `ble` y
   `cmplw r26,r4` + `bgt` (con un `mr r4,r9` que materializa el operando);
   nosotros `cmplw r0,r26` + `bge` y `cmplw r9,r26` + `blt`. Es la diferencia de
   fuente `minSize > size()` contra `size() < minSize` que la r19 midió en su e1
   (96,1592 -> 97,7855) y revirtió por no cerrar.
4. la **rotación del bucle de crecimiento** y la rotación de registros del bucle
   de copia (muro 2 de r19).

No he reabierto esta función: los tres muros de la r19 están medidos con siete
ensayos y un barrido de banderas sobre repro mínimo. **Sin lead nuevo.**

## 5. Los muros — lo que se descubre al mirarlos de cerca

### `Update__19TrackCopCameraMoverf` (948 B, 99,156 %) — UNA instrucción

El diff entero son **dos filas**, y es un intercambio de dos instrucciones
adyacentes:

    objetivo:  addi r3,r1,0x68 | mr r5,r3          | stfs f30,0x58(r1) | stfs f30,0x5c(r1)
    nuestro:   addi r3,r1,0x68 | stfs f30,0x58(r1) | mr r5,r3          | stfs f30,0x5c(r1)

`lmap` sitúa `mr r5,r3` en `TrackCop.cpp:373` (el tercer argumento de
`eMulVector(&look_offset, ..., &look_offset)`) y los `stfs 0x58/0x5c/0x60` en
`bMath.hpp:915`, que es el `bFill(&hcomp, horiz_comp, horiz_comp, vert_comp)` de
nuestra línea 331. En orden de fuente el `bFill` va **antes** que el
`eMulVector`, o sea el desempate por `INSN_LUID` da **nuestro** orden; para que
salga el del objetivo, `mr r5,r3` tiene que ganar por **prioridad**, no por LUID.
Es un empate de `sched2` y no le he encontrado palanca. **No probado**: mover el
`bFill` detrás del `eMulVector` (`hcomp` está muerta después del `bFill`, así que
es legal) — cambiaría el orden de los `stfs` respecto de la llamada, que en el
objetivo van antes.

### `TerrainVelocityNoise__11CameraMover...` (1.192 B, 99,077 %)

Mismo perfil: un `lis` que sale dos ranuras antes (`lis r9,$LC456@ha` en el 127
contra `lis r9,lbl_803D1D70@ha` en el 129) y una permutación f0/f12 en
`fmuls/fmadds`. Reorden local, dmax 2. **No lo he tocado.**

### `Update__8ICEMoverf` (3.868 B, 99,907 %) — 15 registros, y las pistas del DWARF AGOTADAS

Los 15 diffs son **todos** `ARG_MISMATCH` en una ventana de 30 instrucciones
(índices 589-617) que `lmap` sitúa en `ICEMath.hpp:132-153` (`FloatToInt` +
`SignedMod`), `ICEData.hpp:307-309` (`GetNumKeys` / `GetKey`) y `bMath.hpp:232-241`
(`bMax`/`bMin` de `bClamp`). Es una permutación: objetivo r7/r8/r10/r11 contra
nuestro r8/r11/r9/r10, con una decisión real dentro — el objetivo compara el
valor **cargado** (`cmpwi r8,0` tras `lwz r8,0x15c(r1)`) y nosotros **la copia**
(`cmpwi r11,0` tras `mr r11,r9`).

`dwbody.py` (filtrando por `low_pc`) deja **cinco** diferencias estructurales
respecto del original. Estado de cada una:

| diferencia | estado |
|---|---|
| `RevToAng(float)` -> `FloatToAng(float)` donde nosotros ponemos `FloatToInt(GetDutch(fT)*65536.0f)` | **MEDIDA AQUÍ (c1)**, ver abajo: 99,90693 -> 99,90693, `.text` byte a byte idéntica |
| `IsUsingRealTime()` (devuelve `float`) + `IsGameFlowPaused()` contra nuestro `GetUseRealTimeRaw() const` + `TheGameFlowManager.IsPaused()` | **MEDIA MEDIDA AQUÍ (c2a)**: con `IsUsingRealTime()` 99,90693 -> 99,90693. Falta el envoltorio `IsGameFlowPaused()` |
| `Normalize(dst,src)` + dos `Copy(Vector4*,const Vector3*,float)` en el bloque de `mSpace` | ya medida y **documentada en el propio fuente** (`ICEMover.cpp:591-598`): `faltan 0, sobran 0` y el mismo 99,9069 |
| `Scale(Vector3*,...)` + `Clamp(Vector3*,...)` en el bloque de `accel_offset` | medida por la r19 (e8): 99,9069, sin efecto |
| `GetKey` sin expandir `GetNumKeys()` | medida por la r19 (d1): 99,9069, sin efecto |
| `GetParameterLength()` **sin** `const` | **NO probada** |

Dato de fuente que sí sale del mapa de líneas y que no cambia nada pero conviene
saber: el `SignedMod` del original tiene el `while (a < 0)` **sin llaves** (entre
`a += b` en la 139 y `int c = a / b` en la 140 no hay línea de llave de cierre), y
su `else` va en línea aparte. Y el DWARF del original (`mw_dwarfdump:195234`)
confirma que **no** hay ninguna local de más ahí: sólo el `int c` del bloque
anónimo, igual que nosotros. **No hay diferencia de fuente en `SignedMod`.**

### `Update__19TrackCarCameraMoverf` (992 B, 99,569 %)

No tocada. Las pistas de la r19 siguen en pie (el `fov` sin registro y los dos
`bTan`).

## 6. Ensayos numerados, con su cifra

    c1   `ICE::FloatToAng` + `ICE::RevToAng` en ICEMath.hpp y
         `dutch = ICE::RevToAng(GetDutch(fT))` en ICEMover.cpp:656
         Update__8ICEMoverf   99,90693 -> 99,90693
         measure --cmp sobre las 11 unidades que ven ICEMath.hpp
         (zAnim zCamera zFe zFe2 zFeOverlay zMain zMisc zSim zWorld zWorld2 ...):
         **+0 B, +0 funciones, 0 unidades cambian**
         Comparación de secciones ELF con y sin: `.text`, `.rela.text`, `.data`,
         `.rodata` **byte a byte idénticas**; sólo cambian `.debug*`, `.line`,
         `.strtab` (+1 B) y `.symtab`, y el cambio de `.symtab` es que **TODAS las
         etiquetas `$LC` de la unidad se corren en 1 desde `$LC393`**.
         **REVERTIDA**: cero bytes de ganancia y rompe la huella de
         `frozen.py` de zCamera (comprobado: sin c1 «idéntico al congelado», con
         c1 «HA CAMBIADO»). Es la forma del original y está confirmada por el
         DWARF y por el mapa de líneas (`ICEMath.hpp:102` = el `*65536.0f` de
         `RevToAng`; `ICEMath.hpp:98` = el `fctiwz`+`stfd`+`lwz` de `FloatToAng`);
         se aplica en dos ediciones si alguien la quiere junto con algo que sí
         gane bytes.

    c2a  `ICEManager::IsUsingRealTime()` devolviendo `float` y usada en
         ICEMover.cpp:515 en vez de `static_cast<float>(GetUseRealTimeRaw())`
         Update__8ICEMoverf   99,90693 -> 99,90693.  **REVERTIDA**

    v1..v4, w1..w4  ver seccion 3.  **REVERTIDAS**

    sondas p0..p28 y a4/a10/a16 (seccion 2): NUNCA tocaron el arbol compilado,
    solo generaron `.s` al scratchpad, y `ICEReplay.cpp` quedo byte a byte igual.

## 7. Vedas nuevas (con la sentencia barrida)

1. **`ICE::ReplayCategoryTable` y su constructor.** Barrida la forma de la tabla
   entera (lista de 6 `ReplayCategory(...)`) y del constructor: **un repro
   mínimo con esa misma fuente da la forma del objetivo**. La causa de los 4
   `addi` no está en `ICEReplay.cpp` sino en el prefijo de insns RTL de
   `__static_initialization_and_destruction_0`. Comprobado además que meter o
   quitar código **después** de la tabla no hace nada.
2. **La forma del bucle de `LoadCameraShakes`.** Barridas cuatro formas
   (`for` con declaración interna, `for` con `i` fuera, `while`, y la comparación
   con los operandos al revés). Ninguna produce el segundo `li`. La de operandos
   al revés **empeora 5,4 pp**: GCC canonicaliza `n > i` a `i < n` pero cambia el
   reparto.
3. **`bool warned_overflow = false;`** — barrida su declaración+inicialización en
   cuatro posiciones, sola y junto con `id`/`chunk`/`set_chunk`. GCC la borra por
   muerta en las cuatro. (Confirmado por DWARF que la variable existe y vive en
   r0; el `li r0,0` que sobra **no** es su asignación, lleva la línea del `for`.)
4. **`RevToAng`/`FloatToAng` y `IsUsingRealTime()`** en `ICEMover::Update`: son la
   forma del original (DWARF + mapa de líneas) y **no mueven ni un byte**.

## 8. Qué NO he probado

- **static-init**: no he buscado *cuál* sentencia anterior del original expande
  ~27 insns RTL de más que mueren. Es el único camino que queda y no sé por
  dónde entrar sin un volcado RTL del original, que no existe.
- **static-init**: no he probado a mover el orden de los `#include` de
  `zCamera.cpp` (cambiaría el orden de construcción y rompería los 795 índices
  que ya casan).
- **`_Storage`**: nada. No he reintentado los tres muros de la r19 ni la
  combinación e1 (97,79 %) con el orden de los `stw` de cabecera.
- **`TrackCopCameraMover::Update`**: no he probado a mover el
  `bFill(&hcomp, ...)` detrás del `eMulVector` (es legal: `hcomp` está muerta),
  ni ninguna otra reordenación que cambie el LUID de las dos instrucciones que se
  intercambian.
- **`TerrainVelocityNoise`**: no la he tocado en absoluto.
- **`TrackCarCameraMover::Update`**: no la he tocado; siguen vivas las pistas de
  la r19 (el `fov` sin registro, los dos `bTan`).
- **`ICEMover::Update`**: falta `IsGameFlowPaused()` como envoltorio libre (el
  original lo tiene en `GameFlow.hpp`, junto a `IsGameFlowLoadingGame()`, y
  nosotros no) y `GetParameterLength()` **sin** `const`. Las dos son cambios en
  cabeceras compartidas (`GameFlow.hpp` está fuera de zCamera) y, a la vista de
  que las otras cuatro formas confirmadas por DWARF dan exactamente cero, no
  esperaría nada.
- **`lreg.py`** sobre la ventana de 30 instrucciones de `ICEMover::Update` (la
  tabla de allocnos del propio compilador, que es la herramienta que la sección 8
  del documento de herramientas recomienda para justo este caso).

## 9. Nota de convivencia

Cuatro ficheros de `src/Speed/Indep/Src/Camera/` se editaron y se **restauraron
byte a byte** (`ICEMath.hpp`, `ICEMover.cpp`, `ICEManager.hpp`, `ICEManager.cpp`,
`ICEReplay.cpp`). `git status` no los lista y `frozen.py chk` da «idéntico al
congelado». Todo lo temporal quedó en el scratchpad con prefijo `c21cam_`. Las
11 unidades que se reconstruyeron para el A/B de cabecera (zAnim zCamera zFe zFe2
zFeOverlay zMain zMisc zSim zWorld zWorld2) se **volvieron a construir con el
árbol ya restaurado**, así que el `build/` compartido queda coherente:
1.335.160/1.368.752 B, 97,5458 %, 6.615 funciones, igual que antes de empezar.
