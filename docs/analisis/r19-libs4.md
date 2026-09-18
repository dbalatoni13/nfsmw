# Ronda 19 — grupo «cuatro bibliotecas» (LibSN/steering, spchpick, pathtrack, madidct)

Encargo: 9.716 B pendientes en cuatro unidades. Medida propia
`antes_r19_libs_g4.json` → `despues_r19_libs.json`, con `build_direct.py` antes
de **cada** medida (`ninja` no construye ninguna de las cuatro).

    antes:   10.856 / 20.572 B  52,7708 %   85 funciones al 100 %
    despues: 11.008 / 20.572 B  53,5096 %   86 funciones al 100 %

    measure.py --cmp base_r19_libs.json despues_r19_libs.json
        +152 B  +1 fn   .../spch/dev/source/library/cmn/spchpick   5.652 -> 5.804

La línea base del encargo (`base_r19_libs.json`) **coincide exactamente** con la
medida al empezar: 3.668+356+1.180+5.652 = 10.856 B. No estaba rancia.

Ficheros tocados (tres, ninguna cabecera, ninguna bandera, ni `configure.py`):

* `src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c`
* `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp`
* `src/LibSN/steering.c`

`python scripts/audit.py` da **rc=0 en las cuatro** (dos pasadas). Las cuatro
quedan congeladas con `frozen.py cong` y `chk` responde «idéntico al congelado».

---

## 0. `audit.py`: cero fallos, y una falsa alarma que se desactivó sola

Primera pasada (al empezar la ronda): **un solo FALLA**, en
`LibSN/steering::LGInit` — `reubicacion a otro simbolo: lbl_804FF8C0 contra
g_bInitialized`, exactamente la que `docs/analisis/r18-libsn.md` §8 describe como
falso positivo por el sufijo `_<addr>` de las locales anónimas.

Segunda pasada (al terminar): **ya no aparece**. En medio, otro agente cambió
`scripts/audit.py` (69 líneas, visible en `git diff --stat`), aparentemente
normalizando ese sufijo. Es decir: **la única FALLA de mis cuatro unidades era el
falso positivo conocido, y ya está arreglado en la herramienta.** No he tocado
`audit.py`.

Aviso operativo: durante la ronda pasaron de 2 a 25 ficheros modificados en el
árbol sin que yo tocara ninguno de los otros 22. Hay más agentes trabajando en
paralelo; todas mis cifras están medidas contra objetos que yo mismo recompilé
inmediatamente antes.

---

## 1. Lo que cierra: `iSPCH_SentenceLength`, 152 B (spchpick)

`iSPCH_SentenceLength(VoxSentence*, SentencePickInfo*)`, 152 B / 38 instrucciones,
`90,52631 % → 100 %`. Un llamante directo (`spchpick.c:937`, censo de `bl`; no
prueba que no haya punteros a función).

**El diagnóstico está en dos instrucciones.** El objetivo entra al bucle con

    srwi. r27, r0, 2      ; numPhrases = bitField >> 2, forma con registro CR
    beq   <fin>           ; if (numPhrases == 0) saltar el bucle

y nosotros con

    srwi  r30, r0, 2
    cmpw  r28, r30        ; 0 < numPhrases, con el 0 en un registro
    bge   <fin>

El `beq` sobre la forma-record del propio desplazamiento sólo sale si la guarda
de entrada es **`numPhrases != 0` como comparación contra cero**; con
`for (i = 0; i < numPhrases; i++)` GCC 2.95 se queda con `cmpw` porque `cse`
sustituye el literal 0 por el registro que ya vale 0 (el de `dataBytes = 0`)
antes de que `combine` pueda plegar el desplazamiento con la comparación.

### Ensayos

    c0  for (i = 0; i < numPhrases; i++)                    ->  90,52631 %
    c1  numPhrases asignado antes que dataBytes             ->  90,52631 %  (sin cambio)
    c2  if (numPhrases != 0) { for (...) }                  ->  91,71053 %  160 B
    c3  i = 0; if (numPhrases != 0) do {...} while (i<n);   -> 100 %        152 B  <- queda
    c4  while (i < numPhrases)                              ->  90,52631 %
    c5  for (i=0; i<(numPhrases=Get(...)); i++)             ->  85,00000 %  164 B

Es el mismo patrón que la ronda 18 midió con `while (cond);` frente a
`do {} while (cond);`: **GCC rota el `for` y deja una guarda que no es la del
original; el `do/while` con `if` explícito delante la reproduce.**

---

## 2. Correcciones de forma que **no** cierran pero quitan/añaden las
   instrucciones correctas

Las tres siguientes cambian la fuente hacia el objetivo con prueba de tamaño o de
multiconjunto de mnemónicos, y **no mueven `matched_code`** (siguen por debajo
del 100 %). Las dejo en el árbol porque las tres eliminan diferencias
estructurales reales, no de porcentaje.

### 2.1 `PATHI_inittrack` (pathtrack, 680 B): sobraban dos `xori`

`triage`/censo de mnemónicos: `mr-1 xori+2` — teníamos **dos `xori` que el
objetivo no tiene**. Vienen del bucle de voces:

```c
for (i = 0; i < PATH_MAX_VOICES; i++) {
    if (!((voiceID >> (i + 28)) & 1))   /* el ! se materializa: xori;andi.;bne */
        continue;
    break;
}
```

El objetivo hace `andi. r0, r9, 0x1 ; bne` y `andi. r8, r0, 0x1 ; beq`, sin
negación materializada.

    c0  if (!((voiceID >> (i+28)) & 1)) continue; break;   -> 81,100 %  684 B  (2 xori de más)
    c4  if (((voiceID >> (i+28)) & 1) == 0) continue;      -> 82,512 %  676 B  <- queda
    c5  if ((voiceID >> (i+28)) & 1) break;                -> 82,512 %  676 B  (equivalente a c4)
    c6  if (((voiceID >> (i+28)) & 1) != 0) break;         -> 82,512 %  676 B  (equivalente)

El mismo cambio sobre el **otro** bucle idéntico (el de `trackID`, línea 102) sí
regresa: ahí el objetivo ya emite `andi.` y nuestro código también.

    c1  el bucle de trackID con `if (...) break;`          -> 79,776 %  descartado
    c2  idem con `!= 0`                                    -> 79,776 %  descartado
    c3  idem con `== 0 continue`                           -> 79,776 %  descartado

Tras c4 el censo de mnemónicos queda en **`mr-1`**: una sola copia de registro de
diferencia. Lo que falta es el bloque de planificación de los `sth`/`stw` de
inicialización (líneas 120-144) y un `mr r9, r0` en la conversión de `c` del
bucle de `trackname`, donde el objetivo pasa por r0 y nosotros escribimos r9
directamente en las dos ramas.

### 2.2 `iSPCH_ChooseSamples` (spchpick, 412 B): faltaba `bank + 0xC` vivo

El objetivo guarda `(unsigned char *)bank + 0xC` en **un registro salvado más**
(`stmw r17` frente a nuestro `stmw r18`) desde antes del bucle, y en la cola hace
`add r31, r17, r0 ; addi r5, r31, 0x2`. Nosotros lo calculábamos en la cola y GCC
plegaba las constantes: `add r31, bank, r0 ; addi r5, r31, 0xe` — **una
instrucción menos, imposible de reproducir sin la variable viva**.

Parche: declarar `unsigned char *sampleTable`, asignarlo antes del bucle y usarlo
en la cola.

    k0  original (bank+0xC en la cola)      -> 98,000 %  408 B   4 mnem de diferencia
    k1  sampleTable antes del bucle,
        sampleData = sampleTable + 2        -> 95,476 %  412 B
    k3  sampleData = bank+0xE primero,
        sampleTable = bank+0xC después      -> 95,534 %  412 B  <- queda
    k4/k5/k6/k8/k9/kb/kc  seis permutaciones más del orden de
        numParms / sampleSize / sampleData / sampleTable   -> 95,534 % todas
    k7  sampleTable el primero               -> 95,476 %
    ka  sampleTable entre sampleSize y sampleData -> 95,476 %

**Con k3 el multiconjunto de mnemónicos es exacto (0 diferencias), el tamaño es
exacto (412 B) y `argdiff = 0`: ni un solo registro distinto.** Lo único que
queda es **una** instrucción mal colocada: `addi r17, r30, 0xc` va en la posición
5 del bloque preencabezado del objetivo y en la 2 del nuestro. Es un desempate
del planificador de GCC (prioridad igual, se rompe por orden de LUID); las nueve
permutaciones de sentencias no lo mueven.

Bajar de 98,00 a 95,53 % y estar **más cerca** es el tercer ejemplo medido de la
trampa del porcentaje que describe el brief.

### 2.3 `VDevice_RecalcGammaTable` (steering, 216 B): aplicado el c18 de la r18

`docs/analisis/r18-libsn.md` dejaba el parche escrito y sin aplicar. Aplicado:
**95,907 % → 99,796 %**, y el tamaño pasa a ser exacto (216 B). Quitar el puntero
`p` e indexar `g_iGammaLookup[i]` hace coincidir **las 54 instrucciones y todos
los registros**; `argdiff=11, otro=0`.

Lo único que falla es el marco: el objetivo usa **una** ranura de 8 B en `0x8(r1)`
para las tres conversiones (dos int→double y una double→int) y marco 0x20;
nosotros usamos tres (0x8, 0x10, 0x18) y marco 0x30.

    c18 (r18) quitar p, indexar el array      ->  99,796 %  216 B   <- queda
    d1  invertir el orden de las dos ramas    ->  40,352 %
    d2  valor absoluto y signo aparte         ->  17,241 %  228 B
    d3  la asignación duplicada en cada rama  ->  68,463 %  236 B
    d4  cast doble (u8)(s32)v                 ->  99,796 %  (idéntico)
    e1  temporal `t` compartido por las ramas ->  99,796 %  (idéntico)
    e2  temporal `n` para el (u8)             ->  99,796 %  (idéntico)
    e3  while en vez de for                   ->  99,796 %  (idéntico)
    e4  `v` como double                       ->  99,796 %  (idéntico)

**Cinco formas distintas dan exactamente el mismo reparto de ranuras.** El
asignador de temporales de pila de mwcc no lo mueve desde la fuente: el objetivo
reutiliza `0x8(r1)` para las dos ramas exclusivas y para el `fctiwz`, y nosotros
damos una ranura a cada uso. Muro medido, no supuesto.

---

## 3. `LibSN/steering`: el compilador y las banderas, re-verificados

El encargo decía «no repitas el barrido». No lo repetí como barrido de versiones,
pero sí medí dos ejes que la r18 dejó abiertos, porque `Effect_Init` los pedía:

**Eje `-inline`** (mismo compilador GC/2.7, mismo `-O4,s`):

    -inline off        3.668 B  24 fn  pond 92,799 %   Effect_Init 29,232 %
    -inline auto       2.820 B  21 fn  pond 85,739 %   Effect_Init 29,232 %
    -inline on         3.668 B  24 fn  pond 92,799 %   Effect_Init 29,232 %
    -inline all        2.820 B  21 fn  pond 85,739 %   Effect_Init 29,232 %
    -inline deferred   3.668 B  24 fn  pond 92,799 %   Effect_Init 29,232 %

**Eje `-opt`** (añadiendo una sub-bandera a las banderas actuales):

    base            3.668 B 24 fn  92,799 %
    -opt noloop     3.668 B 24 fn  92,799 %
    -opt nostrength 3.668 B 24 fn  92,799 %
    -opt sched      3.668 B 24 fn  92,799 %
    -opt level=3    3.668 B 24 fn  92,751 %
    -opt noprop     3.668 B 24 fn  91,840 %
    -opt nolifetimes 3.412 B 22 fn  92,410 %
    -opt nocse      3.268 B 22 fn  90,667 %
    -opt speed      1.772 B 14 fn  78,446 %
    -opt nopeephole   108 B  3 fn  80,500 %
    -opt noschedule    20 B  1 fn  74,948 %

**Las banderas actuales son el óptimo de los dos ejes, y ninguna toca
`Effect_Init`.** Confirmado: no hay nada que ganar por bandera en esta unidad.

### `Effect_Init` (276 B, 29,23 %) — el hallazgo que queda abierto

Censo: `stw-7 bl+1 subic.-2 stbu-1 stwu-1 …` y `lfd+4 psq_l+4 psq_st+4 stfd+4`.
Son **dos** diferencias independientes:

1. **El objetivo expande `memset` en línea** (bucle de 8 `stw` + cola de bytes,
   con `li r5,0x40 ; li r3,0x2`), y nosotros emitimos `bl memset`. No es
   `-inline`: los cinco valores del eje dan el mismo 29,232 %. Tampoco es la
   declaración:

       n0  void *memset(void*, int, u32);            -> 29,232 %
       n1  ... unsigned long len                     -> 29,232 %
       n2  ... unsigned int len                      -> 29,232 %
       n3  prototipo sin nombres de parámetro        -> 29,232 %
       n4  con `extern`                              -> 29,232 %

   No he encontrado la palanca. Es el mayor bloque de trabajo real de la unidad
   y **queda abierto**.

2. **Nosotros izamos cuatro constantes de coma flotante a f28-f31** (callee-saved)
   y por eso el marco pasa de 0x30 a 0x80 con cuatro `stfd`+`psq_st`+`lfd`+`psq_l`;
   el objetivo las recarga de `@sda21` dentro del bucle y **no salva ni un FPR**.
   Lo mismo con `li r0, 0x3f` (el 63 de `i*1024/63`), que el objetivo
   rematerializa dentro del bucle. `-opt noloop` no lo mueve.

### `HandleTriggers` (588 B, 94,898 %) — muro de coalescing, confirmado

El brief lo señalaba como «la mejor pieza siguiente». Lo es en tamaño, pero el
diagnóstico de la r18 es exacto y **no es de fuente**: las cuatro diferencias de
mnemónico son el mismo `mr` repetido cuatro veces, y sale porque nuestro `_mx`
de `AUTOCAL` cae en **r0** y hay que copiarlo, mientras el original lo coloca en
el mismo registro donde ya vivía la CSE de `cal->max.left` del `if` exterior.

Barrí los dos únicos ejes de fuente que quedaban:

**La macro `AUTOCAL`** (que la r18 pidió no tocar — la toqué midiendo también
`HandlePedals` y `CookValues`, que la comparten):

    m0  declaraciones _mn,_v,_mx,T _dz / asign _v,_mn,_dz,_mx   HandleTriggers 94,898  HandlePedals 100
    m1  asignaciones _v,_mn,_mx,_dz                             94,422   HandlePedals  99,812  <- REGRESA
    m2  declaraciones _v,_mn,_mx,...                            94,898   HandlePedals  98,984  <- REGRESA
    m3  asignaciones _mn,_v,_dz,_mx                             94,898   HandlePedals  99,688  <- REGRESA
    m4  asignaciones _v,_mx,_mn,_dz                             94,422   HandlePedals  99,812  <- REGRESA
    m5  m2+m4                                                   94,422   HandlePedals  98,797  <- REGRESA
    m6  declaraciones _mx,_mn,_v,...                            94,898   HandlePedals 100      (idéntico a m0)

**Confirmado con cifra: el orden de asignación de la macro es el del original**
(cualquier permutación tira `HandlePedals` del 100 %), y **ninguno mueve
`HandleTriggers`**.

**La condición del `if` exterior**, en los dos disparadores a la vez:

    t0  raw->left > cal->max.left - cal->dead.left * 2        -> 94,898 %
    t1  con los operandos de la comparación invertidos        -> 94,762 %
    t2  con paréntesis en el producto                         -> 94,898 %
    t3  - dead - dead en vez de - dead*2                      -> 92,109 %
    t4  (dead << 1)                                           -> 94,898 %

`HandleTriggers` **queda como muro medido de asignación de registros de mwcc**,
igual que `Effect_Update`, `LGDownloadForceEffect` y `VDevice_DownloadEffect`
(1.384 B con cero diferencias de mnemónico, ya vedados en la r18).

### `VDevice_GetFreeEffect` (116 B, 77,07 %) — dos diferencias reales, sin palanca

1. El objetivo **recarga** `jumbleeffectid` entre los dos almacenamientos
   (`stw ; lwz ; clrlwi ; stw`); nosotros reenviamos el valor en registro.
2. El objetivo monta el `id` empezando por `clrlslwi r0,r0,24,8`
   (`(createcount & 0xFF) << 8`) y va insertando con `rlwimi`; nosotros empezamos
   por `slwi r0,r0,16` (`chan << 16`) y el `rlwimi` final tiene como base `idx`.

Trece ensayos, ninguno cierra:

    g0  fuente actual                                        -> 77,069 %  112 B
    g1  las dos primeras asignaciones de id fundidas         -> 77,069 %  112 B
    g2  `jumbleeffectid &= 0x3F` y `id |= …`                 -> 77,069 %  112 B
    g3  `id &= ~0x3F;` e `id |= idx` en sentencias sueltas   -> 74,310 %  116 B (tamaño exacto)
    g4  máscara literal 0xFFFFFFC0                           -> 77,069 %  112 B
    g5  `jumbleeffectid = (idx + 1) & 0x3F;` en una          -> 80,345 %  108 B (pierde un stw: falso)
    g6  sin el `& ~0x3F`                                     -> 75,172 %  112 B
    h1  el `&= 0x3F` movido detrás del bloque de id          -> 71,552 %  116 B
    h2  el `&= 0x3F` intercalado                             -> 77,069 %  112 B
    h3  el bloque de id antes de jumbleeffectid              -> 69,138 %  112 B
    h4  `id |= idx` sin la máscara                           -> 74,310 %  116 B
    h5  con operadores compuestos                            -> 74,310 %  116 B

**Veda**: barrido el orden y la forma de las cinco sentencias del cuerpo del `if`.
**No** he probado cambiar el tipo de los dos estáticos (`s32` → `u32`), ni `%64`
en vez de `& 0x3F`.

### `CookValues` (260 B, 95,769 %) — sin ensayos

`extsb-1 mr+1`: el objetivo **vuelve a extender el byte** (`extsb r0, r5`) para
`_mx = _v` donde nosotros copiamos el valor ya extendido (`mr r0, r6`). Es la
misma familia que `HandleTriggers` (asignación de registros dentro de `AUTOCAL`)
y como la macro quedó demostrada correcta en §3, **no gasté ensayos aquí**.

---

## 4. `pathtrack`

### `PATHI_getmastertrack` (188 B, 66,51 %) — diagnóstico nuevo, sin cerrar

Nadie lo había abierto. **La diferencia es una sola cosa, repetida seis veces:**
el objetivo **recarga `Path::pfstate` de `@sda21` después de cada
almacenamiento** a través de un puntero, y nosotros lo cacheamos en un registro
(`mr r10, r9`) durante toda la función.

    objetivo: 6 × `lwz rX, _4Path.pfstate@sda21`   nuestro: 1
    censo: li-1 lwz-4 lwzx-1 mr+1  (188 B contra 168 B)

De ahí sale todo lo demás: como en el objetivo la base no es invariante de bucle,
**no hay reducción de fuerza** (`addi r11, r9, 0x40 ; lwzx r10, r11, r7` con `r7`
como desplazamiento en bytes) mientras nosotros convertimos el bucle en un
puntero que avanza (`addi r7, r10, 0x40 ; lwz r11, 0x0(r7) ; addi r7, r7, 0x4`);
y el orden de los tres almacenamientos iniciales del objetivo (0x4, 0x0, 0x8) es
el de la fuente porque cada recarga actúa de barrera, mientras el nuestro sale
permutado (0x0, 0x8, 0x4) por el planificador.

**Comprobado que no es la fuente ni las banderas**: compilé un caso mínimo con
las banderas reales de la unidad
(`scratchpad/r19g4_probe/t1.cpp`, `-O2 -G8 -fno-strict-aliasing -ffast-math`)
—un `extern S *p;` global, tres escrituras a campos y un bucle sobre `p->arr[i]`—
y ProDG 3.9.3 **cachea el puntero igual que nosotros** y reduce el bucle a un
puntero que avanza. Es decir: el análisis de alias de GCC 2.95 desambigua
«escritura en campo de estructura» contra «lectura de escalar global», y para que
recargue hace falta que una de las dos referencias deje de encajar en esa regla.

**Qué NO he probado**: no he tocado la declaración de `pfstate` en `pathi.h`
(línea 502) — la incluyen **13 unidades** y el brief exige A/B por objetos sobre
las trece, que no cabía en esta ronda. Es el siguiente paso natural y el único
que puede abrir estos 188 B.

### `PATH_createstreamimp` (424 B, 99,925 %) — cinco ensayos más, sin mover

Sigue siendo **sólo el marco**: 0x28 contra 0x20, con `stfd f31` en 0x20/0x18,
`stmw r30` en 0x18/0x10. El objetivo tiene **8 B más de área de locales**
(0x8..0x17 contra nuestro 0x8..0xf) y ni una instrucción los usa.

    q0  fuente actual                                   -> 99,925 %
    q1  todas las locales al ámbito de función          -> 99,925 %
    q2  `double pad;` inicializado a 0.0                -> 99,925 %  (GCC lo elimina)
    q3  `byterate` declarado y asignado por separado    -> 99,925 %
    q4  copia local `float bt = buffertime;`            -> 99,925 %

Ninguna declaración de local mueve el marco: GCC elimina las locales muertas,
incluida la `double` con almacenamiento muerto. Los 8 B son un **temporal de
compilador de 8 B alineado** que se generó y luego se optimizó dejando la ranura;
no he encontrado qué construcción de la fuente lo crea. La r18 ya había vedado
`int pad[2]` y `volatile int pad[2]`.

---

## 5. `madidct` (IdctRow + IdctColumn, 1.148 B): el DWARF confirma la fuente

**El volcado DWARF del original trae las dos funciones completas** —
`symbols/mw_dwarfdump.nothpp`, línea 2556894 — y **coincide con nuestra fuente
variable a variable**:

    // Range: 0x8034BAC4 -> 0x8034BD3C
    static void IdctColumn(int *src /* r31 */, int *dest /* r15 */) {
        int t1; // r3     int t2; // r14   int t3; // r12
        int t4; // r17    int t5; // r23   int t6; // r0
        int t7; // r26    int t8; // r30   int t9; // r11
    }
    // Range: 0x8034BD3C -> 0x8034BF40
    static void IdctRow(int *src /* r3 */, int *dest /* r4 */) {
        int t1; // r27    int t2; // r31   int t3; // r17
        int t4; // r15    int t5; // r21   int t6; // r0
        int t7; // r30    int t8; // r3    int t9; // r14
    }

Nueve locales `int t1..t9`, mismos nombres, mismos parámetros, `static`, y
`IdctColumn` mueve `src`→r31 y `dest`→r15 igual que nosotros. **No falta código y
no sobra ninguna variable**: lo que queda es reparto de registros.

    IdctColumn  632 B contra 632 B (tamaño exacto)   censo: lwz+2  mr-2
    IdctRow     516 B contra 496 B                   censo: mfctr-2 mtctr-1 mr-2

En `IdctRow` el objetivo tiene **más presión de registros que nosotros**: derrama
`src[6]` a **CTR** (`mtctr` + dos `mfctr`) *además* de derramar un bit de acarreo
a LR; nosotros sólo usamos LR. En `IdctColumn` el objetivo derrama `src[4]` a
`0x8(r1)` y **sigue teniendo el valor vivo en r5** cuando lo necesita para el
`or`; nosotros lo recargamos.

Ensayos (todos con las dos funciones medidas a la vez):

    i0  `static inline int MULT(int,int)`             IdctRow 46,372  IdctColumn 50,449  <- queda
    i1  MULT como macro                                       27,233             49,209
    i2  macro con los dos operandos a long long               27,233             49,209
    i3  `static int MULT` (sin inline)                        29,589             35,627
    i4  MULT con temporal `long long r`                       46,372             50,449  (idéntico a i0)
    r1  parte par antes que la impar                          24,659             50,449
    r2  MULT(t2,35468) antes que MULT(t1,85627)               34,628             50,449
    r3  `t7 = src[0]-src[4]` antes que `t6 = src[0]+src[4]`   51,287             50,449
    r4  t7 subido un sitio más                                51,287             50,449
    r5  t7 y t6 antes del bloque de t8/t9                     51,302             50,449
    r6  t9 = t7-t8 antes de t6 = t6+t9                        26,163             50,449
    r7  el par de t9/t7 antes del par de t2/t6                47,550             50,449

`r3`/`r5` suben `IdctRow` 5 pp **sin cambiar ni el tamaño ni el multiconjunto de
mnemónicos** (siguen faltando el `mtctr` y los dos `mfctr`): es porcentaje, no
avance, así que **los he revertido** y `madidct.cpp` queda idéntico a HEAD.

**Qué NO he probado en madidct**: el permutador (`permuter.py`) — es el caso de
libro para él (multiconjunto de instrucciones exacto en `IdctColumn`, sólo
reparto de registros), y no lo lancé por la advertencia del brief de que lleva
seis rondas sin cerrar; y no he tocado `idctprescale`/`idctinput` ni el orden de
las ocho llamadas de `idctcompute`, que ya está al 100 %.

---

## 6. `spchpick`: lo que queda es un mismo bloque repetido

`iSPCH_ChooseSentence` (848 B, 82,2 %), `iSPCH_SentenceIsContextMatch` (248 B,
32,9 %) y la cabeza de `iSPCH_ChooseSentence` **comparten literalmente el mismo
bloque**: la expansión en línea de `iSPCH_GetRuleDataAddr` →
`iSPCH_GetSentenceRulesAddr` → `iSPCH_GetContextDataAddr` de `spchi.h`. En las
tres el censo de mnemónicos es **exactamente `mr-1` o `mr-2`**: el objetivo copia
`event->numRules` a un segundo registro (`mr r10, r0`) porque sus dos usos quedan
lejos, y nosotros lo usamos directamente del registro de carga.

**La cabecera dice la verdad**: `numSentences` en 0x6 y `numRules` en 0x7 se
verifican en el objetivo (`add r10,r8,r8` sobre 0x6 para `numSentences*2`;
`slwi`+`add` sobre 0x7 para `numRules*3`). No es una «cabecera que miente».

`iSPCH_MakeSampleRequests` (524 B, 98,32 %) es el caso más cercano: **un solo
bloque, y dentro sólo el orden de dos instrucciones** — el objetivo hace
`lwzx r3, r31, r9` (leer `phraseChoice->bankHandle`) *antes* de
`add r30, r31, r9` (formar `phraseChoice`), y nosotros al revés. Es un desempate
del planificador por orden de LUID.

    j0  fuente actual                                            -> 98,321 %  524 B
    j1  sin la local `bankIndex`                                 -> 98,321 %  524 B
    j2  `bankHandle` en local antes de la comparación            -> 96,565 %  528 B
    j3  `bankHandle` leído del array antes de `phraseChoice`     -> 95,160 %  532 B
    j4  operandos de la comparación invertidos                   -> 98,244 %  524 B
    j5  el bucle como `if (n) do {...} while`                    -> 96,870 %  520 B

### `spchi.h`: hipótesis muerta, con A/B sobre las nueve unidades

La hipótesis era partir el `return data + ((…) & ~3);` de
`iSPCH_GetContextDataAddr` en sentencias sueltas para forzar que la dirección de
`sentenceRules` se terminase antes de empezar el bloque de `numRules+7`.
`spchi.h` lo incluyen **nueve** unidades (`spchbank`, `spchcsis`, `spchevnt`,
`spchinit`, `spchpick`, `spchrand`, `spchrslv`, `spchrule`, `spchsamp`), así que
el A/B recompila las nueve y mide las once de `Libs/spch/dev`:

    s0  spchi.h original                                14.404 / 17.384 B  82,8578 %  101 fn
    s1  GetContextDataAddr partido en `offset` + return 14.404 / 17.384 B  82,8578 %  101 fn
    s2  GetSentenceRulesAddr partido igual              14.404 / 17.384 B  82,8578 %  101 fn
    s4  GetContextDataAddr en tres sentencias           14.404 / 17.384 B  82,8578 %  101 fn

**Cero diferencia, ni una unidad se mueve**: GCC 2.95 genera exactamente el mismo
código para las cuatro formas. `spchi.h` queda **intacta** y la hipótesis vedada
con cifra. (`s3` no llegó a medirse: dos compilaciones fallaron con
`Could not open output file` por el `build/` compartido con otros agentes; s1, s2
y s4 sí compilaron las nueve y dan idéntico, que es lo que decide.)

**Qué NO he probado en spchpick**: el permutador; y no he tocado la declaración
de los campos de `VoxEvent`/`VoxSentence` (verificados correctos arriba).

---

## 7. Resumen de lo que NO he probado (además de lo dicho en cada apartado)

* **`permuter.py` y `regmap`**: ni un lanzamiento. `IdctColumn` (multiconjunto
  exacto, sólo registros) y `iSPCH_ChooseSamples` (multiconjunto exacto,
  `argdiff=0`, **una** instrucción mal colocada) son los dos mejores candidatos
  que he visto en la ronda para el permutador ciego.
* **Restricciones de registro** (`register … asm("rN")`): no he usado ninguna.
  Los tres candidatos naturales —`HandleTriggers`, `IdctRow`, `getmastertrack`—
  son de asignación, no de forma, y el brief las permite sólo tras agotar y
  documentar la búsqueda de fuente; documentada queda, pero no las he aplicado.
* **Cabeceras compartidas**: `pathi.h` (13 unidades) y `spchi.h` (9) siguen
  intactas en el árbol. La de `spchi.h` está **medida y descartada** (§6); la de
  `pathi.h` —la declaración de `Path::pfstate`— sigue viva y es la única
  hipótesis de la ronda que puede abrir bytes de golpe (§4).
* **La cadena completa**: ninguna de las cuatro unidades está enlazada
  (`linked False`), así que el DOL no puede cambiar; **no he ejecutado la
  construcción completa ni he verificado el DOL**.
* **No he ejecutado nada**: todas las afirmaciones son sobre bytes e
  instrucciones.
