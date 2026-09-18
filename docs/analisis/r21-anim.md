# Ronda 21 — zEagl4Anim: la familia `Eval*`

Base `c21anim_base.json`: 104.768/113.016 B = **92,7019 %**, 311 fns al 100 %.
Cierre `c21anim_post.json`: **107.520/113.016 B = 95,1370 %, 313 fns**.

    measure.py --cmp   +2752 B, +2 funciones, 1 unidad cambia
    pctsnap.py --cmp   EMPEORAN: ninguna · MEJORAN: 3

| función | antes | después |
|---|---|---|
| `EvalSQT` `FnStatelessF3` (1.468 B) | 99,34605 % | **100 %** |
| `EvalPose` `FnPoseAnim` (1.284 B) | 98,81620 % | **100 %** |
| `EvalSQT` `FnStatelessQ` (1.480 B) | 98,81081 % | 99,45946 % |
| `EvalState` `FnRawStateChan` (456 B) | 93,50877 % | 93,50877 % |
| `Initialize` `DynamicLoader` (2.352 B) | 91,33673 % | 91,33673 % |

`audit.py Speed/Indep/SourceLists/zEagl4Anim`: **313 ok / 0 FALLA** (una pasada;
no hubo ningún FALLA que confirmar). Las dos funciones nuevas auditan enteras:
`EvalPose` 27 ramas / 40 relocs / 12 literales, `EvalSQT F3` 35 / 52 / 16.
Unidad congelada con `frozen.py cong` (`c319fb83ce56404e`) y `chk` conforme.

**El encargo era correcto**: `triage.py` sobre el objeto recién construido dio
exactamente las cinco funciones y las cinco firmas del brief.

---

## 1. El mecanismo: el `if/else` de DOS RAMAS mantiene izada la carga del campo

Las cuatro `Eval*` comparten un preámbulo de «buscar la clave más cercana». La
firma era `mr+2` / `mr+3`: **nos faltaban copias que el original sí hace**.

El `dwbody.py` de las tres grandes dice lo mismo: **el original NO tiene la local
`unsigned short *times`**; lee `X->mTimes[...]` directamente. Pero quitarla sola
**empeora** (98,12 % en F3, 95,20 % en PoseAnim): GCC rematerializa el `lwz`
DENTRO del bucle (`update_equiv_regs` trata el pseudo como equivalente a un MEM).

Lo que lo abre es combinarla con **la forma de la sentencia que fija el índice**:

- con un `if` de UNA rama (`i = 0; if (mPrevKey) i = mPrevKey - 1;`) la carga se
  rematerializa dentro del bucle → 98,12 %;
- con un `if/else` de DOS ramas la carga se queda izada, y `gcse`/PRE crea el
  segundo pseudo de `mTimes` que produce los dos `mr` que faltaban.

La prueba está en `FnDeltaF3.cpp` / `FnDeltaF1.cpp` / `FnDeltaQ.cpp`, que ya
estaban al 100 % y llevan **exactamente ese patrón**: `int timeIndex;` +
`if (mPrevKey < 1) {...} else {...}` + `deltaF->mTimes[...]` sin local. Era la
respuesta escrita en el propio árbol.

### `FnStatelessF3::EvalSQT` — 99,34605 → **100 %**

    a0  control                                                99,34605
    a1  sin local `times` (campo en todo)                      98,11989   PEOR
    a2  local extra sólo en el `else`                          idéntico
    a3  local extra en cada rama                               idéntico
    a4  `const unsigned short *times`                          idéntico
    a5  los dos `while` como `for(;;)`                         idéntico
    a6  comparaciones invertidas (`floorTime >= times[i]`)     99,18256   PEOR
    a7  `times` reasignada dentro del `else`                   idéntico
    a8  dos punteros (`mt` + `times = mt`)                     idéntico
    b4  `slerpReqd` declarada antes que `scale` (lo que dice
        el DWARF del original)                                 idéntico
    e1  `int timeIndex;` + `timeIndex = 0;` + `if` de una rama
        + campo                                                98,11989   PEOR
    e2  `int timeIndex;` + `if (mPrevKey < 1){0}else{-1}`
        + campo  (la forma de FnDeltaF3)                       **99,65940**
    e3  condición con el campo, bucles con la local            idéntico
    e4  condición con la local, bucles con el campo            98,11989   PEOR
    e6  `int numKeys` local                                    94,89101   PEOR
    e7  `times` arriba del todo, usada también en el null-test 99,29155   PEOR

Con `e2` quedaba **una sola sustitución**: `bne` donde el objetivo pone `beq`
(`if (mPrevKey < 1)` emite `subi; cmpwi; bne; li 0`, el objetivo `li 0; cmpwi;
beq; subi`). Barrido el sentido del test:

    f0  `if (mPrevKey < 1) {0} else {-1}`      99,65940
    f1  `if (mPrevKey == 0) {0} else {-1}`     99,65940
    f2  `if (mPrevKey != 0) {-1} else {0}`     **100,00000**
    f3  `if (mPrevKey > 0)  {-1} else {0}`     **100,00000**
    f4  `if (mPrevKey >= 1) {-1} else {0}`     **100,00000**
    f5  `if (mPrevKey)      {-1} else {0}`     **100,00000**   <- APLICADA
    f6  `if (!mPrevKey)     {0}  else {-1}`    99,65940
    f7  `timeIndex = mPrevKey ? mPrevKey-1 : 0;`  **100,00000**
    f8  `timeIndex = mPrevKey < 1 ? 0 : mPrevKey-1;` **100,00000**
    f9  vuelta al `if` de una rama              98,11989

**Regla**: la rama que el compilador emite como *fall-through* es la del
`else`; hay que escribir primero la condición **verdadera** para el caso no
trivial. `if (X) {A} else {B}` y `if (!X) {B} else {A}` **no** son iguales.

### `FnStatelessQ::EvalSQT` — 98,81081 → 99,45946 %

Misma receta, aplicada tal cual. Queda `faltan 0, sobran 2 · clrlwi-2`: dos
`clrlwi r0, r0, 16` de más, uno detrás de cada
`subi r0, r5, 1` de `floorKey >= mNumKeys - 1` (líneas 74 y 132). El objetivo
hace `subi; cmpw` a secas.

Vienen del `static_cast<unsigned short>(statelessQ->mNumKeys - 1)` que ya había
en la fuente… **pero quitarlo es PEOR**, porque entonces GCC extiende la
ENTRADA (`clrlwi r9, r5, 16; subi r9, r9, 1`) en vez de enmascarar la salida:
sigue habiendo dos `clrlwi` y además se desordena el bloque. El pseudo de
`mNumKeys` es HImode en nuestro RTL y SImode en el del objetivo, y no he
encontrado forma de fuente que lo cambie. `FnStatelessF3` tiene la MISMA
sentencia sin cast y no emite `clrlwi`; la única diferencia entre las dos es que
`Q` usa `mNumKeys` una vez más (línea 155).

    g1  quitar el cast de la línea 74                    98,13243   PEOR
    g2  quitar el cast de la línea 132                   99,37838   PEOR
    g3  quitar los dos                                   98,05135   PEOR
    g4  `(int)mNumKeys - 1` en los dos                   98,05135   PEOR
    h1  `>= static_cast<int>(mNumKeys) - 1`              98,05135   PEOR
    h2  `floorKey + 1 >= mNumKeys`                       97,31081   PEOR
    h3  `floorKey > mNumKeys - 2`                        98,03243   PEOR
    h4  `!(floorKey < mNumKeys - 1)`                     98,05135   PEOR
    h5  `mNumKeys - 1 <= floorKey`                       98,01081   PEOR
    h7  cast a `unsigned int`                            96,97027   PEOR
    u1  `slerpReqd = slerpReqd && floorKey < …;`         92,01351   PEOR
    u2  `int numKeys = …;` delante de cada test          98,05135   PEOR
    u3  `unsigned short numKeys = …;`                    98,05135   PEOR
    u4  `static_cast<unsigned short>(mNumKeys) - 1`      98,05135   PEOR
    u5  `if (<) {} else { slerpReqd = false; }`          idéntico
    u6  `unsigned short lastKey = …;` fuera del `if`     idéntico
    u7  `static_cast<int>(static_cast<unsigned short>())` idéntico
    w1  cast también en la línea 66                      97,87838   PEOR
    w2  cast también en la línea 155                     99,18919   PEOR
    w3  `(int)mNumKeys` en la línea 64                   idéntico
    w4/w5/w6/w7  combinaciones sin cast + 66/155         96,2-97,8  PEOR

**Veda**: barridas **22 formas** de `floorKey >= statelessQ->mNumKeys - 1`
(las dos apariciones a la vez y por separado) más el sitio del cast en las otras
tres apariciones de `mNumKeys`. El cast actual es el óptimo local.

### `FnPoseAnim::EvalPose` — 98,81620 → **100 %**

Aquí `times` se usa también FUERA de los bucles (líneas 47, 51, 73 y 79), y el
objetivo mantiene los dos pseudos vivos a la vez (`r7` para todo y `r11` copiado
en cada bucle). Además faltaba un tercer `mr r8, r0` en `floorKey = mPrevKey;`.

    i1  campo sólo en el bloque `else`                    97,33645   PEOR
    i3  campo sólo en los dos tests de cabecera           idéntico
    i4  campo en todas partes (sin local)                 95,20249   PEOR
    j3  condición con el campo, bucles con la local       idéntico
    j5  `unsigned short *t = times;` dentro del `else`    idéntico
    j6  `unsigned short *t = poseAnim->mTimes;`           idéntico
    j10 `times` declarada dentro del `else`               idéntico
    j12/j13  `int prevKey = mPrevKey;`                    97,34 / idéntico
    k1..k7  la declaración de `times` en las 6 posiciones
            posibles del bloque, y con `const`            **las 8 idénticas**
    p2  `PoseLinearBlendF3` desplegado a mano             98,69159   PEOR
    r3  campo en todo + `if (mPrevKey) {mPrevKey} else {0}`  99,06542
    t1  `int prevKey` declarada arriba                    91,78816   PEOR
    t2  `int prevKey` dentro del `else`                   95,20249   PEOR
    t3  `unsigned short prevKey`                          95,20249   PEOR
    t4  `mPrevKey & 0xFFFF`                               95,20249   PEOR
    t5  `floorKey = mPrevKey; floorKey += 0;`             95,20249   PEOR
    t6  `if (mPrevKey == 0) {0} else {mPrevKey}`          **100,00000**  <- APLICADA
    t7  `static_cast<int>(mPrevKey)`                      95,20249   PEOR

En `r3` sobraban exactamente las 3 instrucciones de la rama (`li`, `cmpwi`,
`beq`); con `t6` GCC pliega la rama y **deja sólo la copia**, que es lo que hacía
falta. Nótese que aquí gana el sentido CONTRARIO al de `FnStatelessF3` (`== 0`
primero): la diferencia es que en `PoseAnim` las dos ramas asignan el MISMO
valor y el pliegue sólo ocurre con esa escritura.

**Veda k**: la posición de una declaración dentro del bloque de locales es
invisible (8 variantes idénticas). No volver a gastar ahí.

---

## 2. `FnRawStateChan::EvalState` — 456 B, sigue en 93,50877 %

`faltan 2, sobran 2, de ellas 2 SUST · add-2, mr+2`. Dos cosas:

1. el objetivo lee `mKeyIdx` **tarde** (`lwz r11, 0x10(r29)` justo antes de
   usarlo) y nosotros lo izamos al principio;
2. `mr r4, r11` en el objetivo contra `add r4, r30, r10` nuestro: el objetivo
   **reutiliza** el puntero base de la clave ya calculado; nosotros lo
   recalculamos. De ahí el `add-2`.

`dwbody.py` da dos diferencias estructurales reales, y **las dos son trampa**:

- el `GetKeyData` del original **no tiene la local `keyData`**; el nuestro sí.
  Quitarla se lleva la función por delante: `67,50 %` con ternario y `62,30 %`
  con dos `return`. La local es imprescindible.
- el original **no tiene la local `keyIdx`** de nuestra fuente (que un agente
  anterior añadió con un comentario). Quitarla: `89,87 %`.

Ensayos (base 93,50877):

    l1  `GetKeyData` con ternario, sin local              67,50000   PEOR
    l2  `GetKeyData` con dos `return`, sin local          62,29824   PEOR
    l3  sin la local `keyIdx` (usar `mKeyIdx` directo)    89,86842   PEOR
    l4  l1 + l3                                           64,78947   PEOR
    l5  l2 + l3                                           59,36842   PEOR
    x1  `i = GetNumKeys()-1;` antes del `Decode` final    89,82456   PEOR
    x2  `k = GetKeyData(GetNumKeys()-1);`                 idéntico
    x3  `mKeyIdx = …` antes del `Decode`                  69,90351   PEOR
    x4  `k = GetKeyData(0);` en la rama `else`            idéntico
    x5  el `while` como `for`                             93,50000   PEOR
    x6  `i = mKeyIdx;` sin local                          82,14035   PEOR
    x7  x6 + x1                                           78,28947   PEOR

**Veda**: barridas las dos formas de `GetKeyData` sin local, las tres formas del
`Decode` final de cada rama, el `while`→`for`, y la eliminación de `keyIdx`.
El orden de expansiones inline del DWARF dice que nos sobra **un `GetNumKeys`**
antes del `GetKeyData` de la línea 115 — es la pista viva y **no** la he podido
convertir en código: las tres reescrituras de esa sentencia empeoran.

---

## 3. `DynamicLoader::Initialize` — 2.352 B, sigue en 91,33673 %

No he tocado el árbol del `switch` (vedas 4 y 5 de `r20-muros.md`). He ido a por
«qué mantiene vivo el objetivo», que es lo que pedía el encargo.

`dwbody.py` da tres diferencias estructurales concretas:

1. el original tiene una **referencia** `HashPointer & h` además de `pHP`, y
   expande `HashPointer::operator new` y el **constructor** `HashPointer(DynamicLoader*)`;
   nosotros llamamos a `EAGL4Malloc` a pelo;
2. el `htotul` del original tiene una local `unsigned int ul;`;
3. `shstrtab` va en **r20** en el original y en **r22** en el nuestro — el
   desplazamiento de un registro que describe la ronda 20.

Ensayos (base 91,33673):

    m1  quitar el `register HashPointer *pHP asm("r31")`   80,73640   MUCHO PEOR
    m2  `htotul` con `unsigned int ul = …; return ul;`     idéntico
    m3  `htotul` con `ul` en dos sentencias                82,80612   PEOR
    m4  `ELFAddr` con el `if (offset >= mDataLen)` fundido 84,90306   PEOR
    m5  `ELFAddr` como cadena `else if`                    idéntico
    m6  m1 + m2                                            80,73640   PEOR
    m7  m1 + m4                                            77,89796   PEOR
    m8  m1 + m2 + m4                                       77,89796   PEOR
    q1  `new HashPointer(this)` + `HashPointer &h = *pHP;`
        (sin la asignación explícita de `mpDynamicLoader`)  91,30782   PEOR
    q2  q1 conservando la asignación explícita             78,58334   PEOR
    q3  q1 + los 32 `pHP->` reescritos a `h.`              85,43198   PEOR
    q4  q3 conservando la asignación explícita             74,11905   PEOR
    q5  sólo la referencia (sin `new`) + `h.`              85,45238   PEOR

**Resultado más importante, y va contra la regla 4 del brief**: el
`register HashPointer *pHP asm("r31")` **es necesario**. Quitarlo cuesta 10,6 pp.
Tercer caso medido de pin, y el primero en el que el pin es la solución y no la
causa.

**Veda**: barridas 2 formas de `htotul` con la local que dice el DWARF (una es
byte a byte idéntica, así que la diferencia del DWARF **no es accionable** aquí),
2 formas de `ELFAddr`, y 5 formas del `new`/referencia de `HashPointer`.

Lo que sí queda localizado al byte, para quien siga: en `ELFAddr` el objetivo
**funde por cross-jump los dos `return &mpData[offset];`** (el del primer `if` y
el del final) y nosotros emitimos los dos; ahí están el `sobran 2` (`add` +
`stw`) y parte de los `ble+2`. Es el mismo mecanismo de `jump.c` que la ronda 20
documentó en `ResolveCarBanks`: hay que conseguir que muera la copia temprana.

---

## 4. Qué NO he probado

- **`Initialize`**: el permutador (ni guiado ni ciego); reconstruir el
  cross-jump de `ELFAddr` escribiendo las dos copias del `return` de forma que
  `jump.c` funda la temprana; el `mtctr`/`mfctr` de `i*0x28` y la recarga de
  `0x98(r1)` (sé que están, no he encontrado la sentencia que los provoca);
  y **nada** del árbol del `switch`, por veda.
- **`FnStatelessQ`**: por qué el pseudo de `mNumKeys` es HImode aquí y SImode en
  `FnStatelessF3`, siendo la sentencia idéntica. La única diferencia de fuente
  entre ambas es el uso extra de la línea 155; no he probado a eliminarlo
  (cambiaría la semántica) ni a moverlo.
- **`EvalState`**: el `GetNumKeys` que sobra en el árbol de inlines; `lreg.py`
  sobre esta función; y la variante de `GetKeyData` con la local pero con el
  `keyIdx * GetKeySize()` factorizado fuera.
- **Cabeceras compartidas**: `htotul` con `ul` (m2) es idéntica en zEagl4Anim y
  correspondería al DWARF del original, pero `eagl4supportdlopen.h` lo incluye
  también `zAnim` (`AnimBank.cpp`, `AnimInternal.*`, `AnimSkeleton.hpp`).
  **No lo he aplicado** porque no aporta bytes y no he hecho el A/B de `zAnim`.
- `dwbody` señala dos diferencias más que no he tocado por no ser accionables
  según `triage`: en `FnStatelessQ` nos sobran 12 expansiones de
  `Uncompress2Bits`, y en `FnStatelessF3` el `GetData()` del original lleva una
  local `dofInfo` en vez de anidar `GetDofInfo()`. Las dos zonas ya casan.

## 5. Herramientas dejadas en el scratchpad

Prefijo `c21anim_`:

- `c21anim_it.py` / `c21anim_rows.py` — copias de las `mr20_*` de la ronda 20
  (porcentaje por símbolo y diff por `diff_kind`).
- **`c21anim_sweep2.py <spec.py> [nombres…]`** — arnés multi-parche: cada
  variante es una lista de `(fichero, viejo, nuevo)`, compila, mide y
  **restaura siempre**. Casa los bloques con `\r?\n` **por línea**, que hacía
  falta: `FnPoseAnim.cpp` tiene 147 finales CRLF y 14 LF y un `str.replace`
  normal no encuentra nada.
- `c21anim_spec_*.py` — los ~70 ensayos de arriba, reproducibles.
- Aviso: `c21anim_rows.py` lee el `.o` del disco; tras un barrido hay que
  **reconstruir** antes de mirar filas o se leen las de la última variante.
