# r76 — Despineo del lote zGameplay: CERO pines y CERO barreras

Rama `agent/vedas-oraculo-r2`, HEAD `bf3d0dc0`. Ficheros tocados, y sólo éstos:

```
src/Speed/Indep/Src/Gameplay/GTrigger.cpp
src/Speed/Indep/Src/Gameplay/GManager.cpp
src/Speed/Indep/Src/Gameplay/GRaceDatabase.cpp     (contiene GRaceParameters::GenerateIndex)
```

Ni `configure.py`, ni `config/`, ni `keep.lst`, ni ficheros de otras unidades.
Sin commit. `zGameplay` es **NonMatching**: el DOL enlaza el objeto extraído, así
que ningún cambio de tamaño de aquí lo mueve.

---

## 0. El resultado en una tabla

Los **cinco** andamios del censo r76 en esta unidad están fuera. El censo
(`scripts/censoasm.py`) da hoy **0 PIN y 0 BARRERA** en los tres ficheros.

| función | fichero:línea | andamio ANTES | DESPUÉS | % antes | % después | filas | bytes |
|---|---|---|---|---:|---:|---:|---|
| `GTrigger::GTrigger` | `GTrigger.cpp:92` | 1 barrera `"+m"(boxmat)` + clobber `r0` (+ el temporal `float rot` que la sostenía) | 0 | 100 | **99,58334** | 4 | 2.544/2.544 |
| `GRaceParameters::GenerateIndex` | `GRaceDatabase.cpp:1078` | 1 barrera `"+r"(f)` | 0 | 100 | (sola: 99,98810) | 1 | 1.680/1.680 |
| `PackedDecimal::PackedDecimal` (inline, 3 copias dentro de `GenerateIndex`) | `GRaceDatabase.cpp:62` | 1 barrera `"+r"(negative)` | 0 | — | (suma: **98,96429**) | +7 | **+12 B** |
| `GManager::AllocateInstanceMap` | `GManager.cpp:997` | 1 pin `register unsigned int tableSize asm("r30")` | 0 | 100 | **98,07143** | 24 | 280/280 |
| `GManager::GetStrippedNameKey` | `GManager.cpp:1774` | 1 pin `register const char *lastSlash asm("r3")` | 0 | 100 | **99,60000** | 1 | 100/100 |

**La unidad entera** (`scripts/pctsnap.py`, 767 funciones, 141.468 B de `.text`):
**99,97602 %** ponderado, con **exactamente esas cuatro** funciones por debajo
del 100 % y las **763 restantes sin tocar** (comparadas una a una contra la
medida previa: cero diferencias). El precio total del despineo es **0,024 % de
la unidad** y **12 bytes**.

Sonda de portabilidad X360 (`tools/scratch/x360_sonda/sonda.py`), contra el
resultado del 15-sep (r71f):

| fichero | clase ANTES | clase DESPUÉS | qué bloquea ahora |
|---|---|---|---|
| `GTrigger.cpp` | GCCASM (`:92`, la barrera) | **OTRO** | `_STL::find` (espacio de nombres de STLport) |
| `GRaceDatabase.cpp` | GCCASM (`:62`, la barrera) | **OTRO** | `_STL::min` |
| `GManager.cpp` | GCCASM (`:81`) | GCCASM (`:81`) | los **dos pools `asm(".section .data")`** (`gap_06_8041D38C`, `gap_06_8041D3A0`) y dos ALIAS (`memset`, `lbl_8041D3AC`) |

Dos de los tres ficheros **salen de la clase GCCASM**. `GManager.cpp` sigue
dentro, pero ya **no por andamio**: lo que queda es el frente de DATOS/ALIAS
(los huecos que `keep.lst` espera por nombre), que es otro encargo.

---

## 1. `GTrigger::GTrigger` — 2.544 B, la barrera de ranura

### Qué decía el DWARF

`python scripts/dwarf1.py fn GTrigger::GTrigger` da, en ámbito de función y en
este orden:

```
param  this               const GTrigger*   r31
param  triggerKey         const unsigned&   r4
local  pos                                  (optimizada fuera)
local  dim                Vector3           [r1+16]
local  hasDimensions      boolean           r22
local  posSwizzled        Vector3           [r1+32]
local  dimSwizzled        Vector3           [r1+48]
local  mat                Matrix4           [r1+96]
local  radius             float             f30
local  pTriggerData       StaticData*       (optimizada fuera)
local  showIconBasedOnBin boolean           r29
bloque { rotMat Matrix4 [r1+16];  initialVec Vector3 [r1+80] }
```

**El censo r76 clasificó esta barrera en el grupo A («la variable no existe») y
eso es un FALSO POSITIVO del automatismo**: la barrera se llama `boxmat` porque
así se llama en *nuestra* fuente, pero la variable existe en el original con
otro nombre — es `mat`, en `[r1+96]`. El diccionario completo es `boxmat` =
`mat`, `center` = `posSwizzled`, `size` = `dimSwizzled`, `dims` = `dim`,
`rotmat` = `rotMat`, `dir` = `initialVec`. Lo que la barrera ataba no era una
variable inventada: era una **ranura del planificador**.

Lo que el DWARF **sí** desmiente es el temporal `float rot`, que no existe; y el
mapa de líneas (`scripts/lmap.py`) pone toda la sentencia en una sola línea
(`GTrigger.cpp:113` del original). Las dos cosas fuera, en una sentencia:

```cpp
UMath::MultYRot(boxmat, -Rotation() / 360.0f, boxmat);
```

### Lo que queda: cuatro filas, cero bytes

```
objetivo:  lis r9 / lfs f0 / fneg f1,f1 / mr r3,r27 / fmuls / mr r4,r27 / bl / li r22,1
nuestro:   lis r9 / lfs f0 / mr r3,r27  / fneg f1,f1 / mr r4,r27 / fmuls / li r22,1 / bl
```

Mismas instrucciones, mismos ciclos (dos por ciclo), **invertido el orden dentro
de cada ciclo**. El `li r22, 0x1` es la constante `1` que comparten
`mWorldTrigger.fType` y `mEventList.fNumEvents` (por eso vive en un preservado).

### Formas medidas en esta ronda

| forma | filas | bytes |
|---|---:|---|
| **sin barrera y sin `rot`, una sentencia (LA QUE QUEDA)** | **4** | 2.544/2.544 |
| `mWorldTrigger.fType = 1` detrás de `fFingerprint` | 15 | 2.544 |
| `mWorldTrigger.fType = 1` delante de `fHeight` | 8 | 2.544 |
| `mEventList.fNumEvents = 1` subido junto a `fType` | 74 | 2.544 |

Más las de rondas anteriores, ninguna por debajo de 4: `-Rotation() * (1.0f /
360.0f)` idéntico, `float rot` como sentencia aparte idéntico,
`-(Rotation() / 360.0f)` 8 filas.

---

## 2. `GRaceParameters::GenerateIndex` — 1.680 B, dos barreras

Esta función paga **las dos** barreras de `GRaceDatabase.cpp`: la suya (`"+r"(f)`)
y la del constructor inline de `PackedDecimal` (`"+r"(negative)`), que se inlinea
**tres veces** dentro de ella.

### 2a. La barrera de `f`: una fila, cero bytes

El DWARF de `GenerateIndex` declara **sólo** `topLeft` `[r1+16]` y `botRight`
`[r1+24]` (más `neg`/`man`/`exp` de las tres copias inline). `pflags`, `flags`,
`rank` y `f` son locales nuestras: grupo A **confirmado**.

Quitada la barrera queda **una sola fila**: `ori r0,r0,0x200` en el objetivo
contra `ori r0,r30,0x200` en el nuestro. Es `cse`: `f` y `flags` acaban en la
misma clase de equivalencia y `canon_reg` sustituye por `qty_first_reg`, que es
`flags` porque entró antes en la clase. La barrera sólo rompía esa equivalencia.

| forma | filas | bytes |
|---|---:|---|
| **con `rank` y `f`, sin barrera (LA QUE QUEDA)** | **1** | 1.680/1.680 |
| `f = f \| K` en vez de `f \|= K` | 1 | `.o` **idéntico** al anterior |
| **sin `rank` ni `f`** (la forma literal del DWARF) | 3 | 1.676 (**−4 B**) |

Aquí el oráculo **manda al sitio equivocado**: escribir la función con las
locales que el DWARF declara *pierde* el `mr r0,r30` y con él 4 bytes. El
objetivo tiene la copia y almacena por un puntero en `r17`, así que la estructura
con `pflags` y el temporal es la que reproduce el tamaño. Las locales de más son
C normal y portable, no andamio: se quedan.

### 2b. La barrera de `PackedDecimal`: 12 bytes, y es el único precio del lote

Es el único sitio donde el despineo paga tamaño: 1.680 → **1.692 B**, +7 filas.

Medido fila a fila: en las copias inline 1 y 2 el asignador mete el test de
`if (negative)` en **cr4** — un campo de CR **no volátil**, que el prólogo ya
salva (`mfcr r12` / `mtcrf 8,r12`) — y en la **tercera** se lo da a **cr7**, que
sí es volátil; entonces `reload` tiene que derramarlo a un GPR alrededor de las
llamadas a `GetNormalizedLower/Upper`:

```
objetivo:  cmpwi cr4, r0, 0
nuestro:   cmpwi cr7, r0, 0 ... mfcr r26 ... slwi r26,r26,28 ... mtcrf 128,r26
```

**No es una variable inventada**: el DWARF lista `neg` en `r0` en las tres copias
inline, y ahí está nuestro `negative` (`li r0,0` / `li r0,1`). Es deuda de
**reparto de un campo de CR**, que ninguna palanca de fuente alcanza.

El mapa de líneas confirma además que este cuerpo vivía en `PackedDecimal.h` y
que nuestro orden de sentencias es el del original: `:34` `exponent = 0`, `:35`
`mantissa = 0`, `:36` `negative = false`, `:38` `if (value < 0)`, `:40/:41`,
`:44` `if (value > 0)`, `:48/:49` primer `while`, `:52` `mantissa = (int)value`,
`:54..:57` segundo `while`, `:61/:62` `if (negative) mantissa = -mantissa`,
`:65` los dos campos.

| forma | filas | bytes |
|---|---:|---|
| **sin barrera, forma actual (LA QUE QUEDA)** | **8** (7 suyas + 1 de `f`) | 1.692 |
| `mantissa = negative ? -mantissa : mantissa;` | 8 | 1.692 |
| `bool negative;` + `else { negative = false; }` | 22 | 1.716 |

Más las ocho de la ronda anterior, todas a 1.692 B: sin llaves, `0 - mantissa`,
`0.0f - value`, comparación invertida, `negative != false`, `int negative`,
`negative = value < 0.0f`, intercambiar las dos sentencias del `if`.

---

## 3. `GManager::AllocateInstanceMap` — 280 B, el pin de `tableSize`

### Qué decía el DWARF

```
param  this                 r31
local  mostVaultObjects     [r1+8]
local  mostTransientVaults  (optimizada fuera)
local  minHashEntries       r9
local  tableSize            r30
bloque { onVault r29;  bloque { vault GVault& r30 } }
```

Grupo **B**: `tableSize` existe y su registro es r30. El pin no inventaba nada,
**ganaba una carrera de `global_alloc`**.

### Lo que queda: 24 filas que son UNA transposición

Las 24 filas son `r30 ↔ r31` y **nada más**: mismas instrucciones, mismos
desplazamientos, mismo tamaño (280/280). El objetivo pone `this` en r31 y el par
`vault`/`tableSize` en r30; nosotros al revés.

**El mecanismo**, leído en `orig/prodg/NGC_GNU_SRC/NGC/gcc/`: `REG_ALLOC_ORDER`
(`rs6000.h:932`) entrega los preservados en el orden **31, 30, 29…**, así que el
primer allocno que pide uno se lleva r31. `allocno_compare` (global.c) ordena por
`floor_log2(n_refs) * n_refs / live_length`, y `vault` —cuyas referencias van
**pesadas por estar dentro del bucle**— sale con prioridad 34285 contra 10322 de
`this`, así que va primero y arrampla con r31. En el objetivo el primero es
`this`. Con el código final byte a byte igual, `n_refs` y `live_length` son los
mismos en los dos lados: la diferencia está en el RTL previo, y no la alcanza
ninguna forma de fuente que se haya probado en tres rondas (r65, r67, r76).

Medido y negativo: declarar `minHashEntries`/`tableSize` arriba, como los lista
el DWARF → las mismas 24 filas.

**Éste es el caso que mejor ilustra el encargo**: 98,07 % con las instrucciones
exactas del original y un nombre de registro distinto, contra 100 % sostenido por
un `register asm("r30")` que no compila fuera de GCC.

---

## 4. `GManager::GetStrippedNameKey` — 100 B: **el pin sustituido por C portable**

Éste es el hallazgo aprovechable del lote.

### Qué decía el DWARF

```
param  this       const GManager*   r3
param  name       const char*       r31
local  lastSlash  const char*       r3
```

Grupo B otra vez: `lastSlash` existe y su registro es r3.

### El mecanismo, y la palanca

`lastSlash` es un allocno **global** (vive por todo el bucle). Sin preferencia de
registro, `find_reg` cae en `REG_ALLOC_ORDER` (`0, 9, 11, 10, …`); r0 no vale de
base, así que sale **r9** — 5 filas, todas el mismo cambio de nombre.

La preferencia la pone `set_preference` (`global.c:1551`), y su primera línea es:

```c
  if (GET_RTX_FORMAT (GET_CODE (src))[0] == 'e')
    src = XEXP (src, 0), copy = 0;
```

Ante un `(plus A B)` mira **sólo el primer sumando**. Si ese primer sumando ya
tiene `reg_renumber >= 0` (se lo dio `local_alloc`) y sale un registro duro, el
destino hereda la preferencia. Con `name + bStrLen(name)` el primer sumando es
`name` —pseudo global, sin numerar— y no hay preferencia ninguna.

**La solución, y es C portable:** escribir la suma en aritmética **entera**. Así
el front-end de C++ ya no aplica `pointer_int_sum` (que pone el puntero siempre
delante), el primer sumando pasa a ser el retorno de `bStrLen` —local, numerado a
r3 por `local_alloc`— y `lastSlash` hereda r3:

```cpp
const char *lastSlash = (const char *)(bStrLen(name) + (unsigned int)name);
```

El mismo idioma de casteo ya está en este fichero, en `GManager.cpp:688`.

| forma | % | filas |
|---|---:|---:|
| `name + bStrLen(name)` (la natural) | 98,8 | 5 |
| `(const char *)((unsigned int)name + bStrLen(name))` | 98,8 | 5 (`.o` **idéntico** al anterior) |
| **`(const char *)(bStrLen(name) + (unsigned int)name)`** | **99,6** | **1** |

La fila que queda es el orden de operandos del `add` conmutativo (objetivo
`add r3,r31,r3`, nuestro `add r3,r3,r31`), y **va atado**: el orden del árbol
decide a la vez la preferencia y lo que se imprime, así que no se pueden tener
las dos cosas desde la fuente.

### Por qué esto es un frente, no una anécdota

Cualquier pin sobre una variable que sea el **destino de `puntero + llamada()`**
—y en el censo r76 hay varios— puede intentar exactamente esta sustitución. Es
C estándar, compila en X360/PS2/PC, y aquí recuperó el 80 % de lo que sostenía
el pin.

---

## 5. Dos avisos

1. **`GManager.cpp` usa `BNEW` (= `new (__FILE__, __LINE__)`)** en las líneas
   2539 y 2601, después de los dos pines. Todos los comentarios nuevos de este
   informe están escritos **conservando el número de líneas del fichero**
   (3.138, verificado, y los dos `BNEW` siguen en 2539 y 2601). Cualquier retoque
   futuro de esos comentarios tiene que hacer lo mismo o moverá el `.text` de dos
   funciones que hoy están al 100 %.

2. **`OBJDIFF_SYMBOL_MAPPINGS` de zGameplay está rancio, y no es culpa de este
   lote.** `configure.py:2382` espera el sufijo `.25167` en
   `Compare__…_8IconSortPCvT1`, y el `.o` sale hoy con `.25164`. **Control
   hecho**: se recompiló la unidad con los tres ficheros míos en su versión de
   HEAD y el sufijo salía **también `.25164`**, o sea que mis cambios son neutros
   para ese contador. Lo digo y no lo toco, porque `configure.py` no es mío.

---

## 6. Reproducir

```
python scripts/build_direct.py zGameplay
SCRATCH=scratchpad/r76_gameplay python scripts/fndiff.py zGameplay __8GTriggerRCUi
python scratchpad/r76_gameplay/m.py "etiqueta"     # las cuatro de golpe
python scripts/pctsnap.py -o pct.json zGameplay    # la unidad
python scripts/dwarf1.py fn GManager::GetStrippedNameKey
python scripts/lmap.py zGameplay GenerateIndex__15GRaceParametersP14GRaceIndexData
python scripts/censoasm.py | grep Gameplay
python tools/scratch/x360_sonda/sonda.py src/Speed/Indep/Src/Gameplay/GTrigger.cpp
```

El histórico completo de medidas de la ronda, con la etiqueta y el sha1 del `.o`
de cada variante, está en `scratchpad/r76_gameplay/medidas.txt`.
