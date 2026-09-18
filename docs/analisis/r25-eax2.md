# Ronda 25 — zEAXSound2: la deuda del `while+break`, resuelta y sustituida

**Resultado: `ResolveCarBanks` 99,60842 % → 99,80821 % (+0,200 pp), APLICADO al
árbol.** El diff pasa de **96 filas a 22**, y las 22 que quedan son **dos racimos
limpios y ya identificados** (el de `Vector::erase`/`indexof` de `UTLVector.h` y
el del `for` final sobre `CarSoundConn`). `triage.py` la mantiene en **MURO**
(`faltan 0, sobran 0, 0 SUST`).

`measure.py --cmp` (base y medida seguidas, `build_direct.py` delante de las
dos): **+0 B, +0 funciones, 0 unidades cambian** — `matched_code` es todo-o-nada.
`pctsnap.py --cmp`: **MEJORAN 1, EMPEORAN ninguna.**

`audit.py Speed/Indep/SourceLists/zEAXSound2`: **0 FALLA**, dos pasadas (una al
empezar y otra tras el cambio) y **la salida de las dos es idéntica byte a byte**.
Congelada con `frozen.py cong` (huella `bc92e2b2d718be63`); `chk` posterior:
«idéntico al congelado».

Unidad: **158.400/170.256 B = 93,0364 %, 921 funciones al 100 %** (igual).

    ANTES   7508 B  99.60842%  ResolveCarBanks   96 filas (85 = permutacion r23/r24)
    AHORA   7508 B  99.80821%  ResolveCarBanks   22 filas
     392 B  94.93877%  Play…RoadNoise   — no cerrada (6 formas nuevas medidas)
     340 B  95.88236%  BindToData       — no cerrada (mecanismo precisado)

---

## 1. La deuda de la r24: **NO hay ruta sin bucle. Demostrado con medida.**

La r24 subió a 99,608 % metiendo `while (n >= 0) { … break; }` —un `if`
disfrazado— y dejó anotado que *«el original llega a +0x110C sin bucle, por una
ruta que no encontró»*. **Esa ruta no existe**, y ahora se sabe por qué al dígito.

### 1.1 El mecanismo, completo

`loop_optimize` (`find_and_verify_loops`) reubica el bloque `then` de una cola
interior en el **hueco muerto que precede a la etiqueta a la que salta su salto
terminal**. El punto de inserción se calcula **recorriendo hacia atrás desde la
etiqueta destino** y **parando en el primer BARRIER que no esté dentro de un
bucle**; los bucles se saltan enteros (`NOTE_INSN_LOOP_END` → `NOTE_INSN_LOOP_BEG`).

Con polaridad base los dos bloques acaban en `b LoadRemainingEngines`, así que el
recorrido sale de `LoadRemainingEngines:` hacia atrás. Por el camino hay:

| qué | ¿lo salta? |
|---|---|
| limpieza de cierre de bloque de BeginRule4 | sí (no hay BARRIER) |
| **BARRIER del `b BeginRule4` de la cola EXTERIOR** | **NO → aquí para (+0x17B8)** |
| barriers de las dos colas interiores | sí: están dentro del `for (m)` |
| bloque entero de BeginRule3 | sí: es un nido de `for` |
| BARRIER del `b BeginRule2` | — es el destino bueno: **+0x110C** |

**El único obstáculo es el BARRIER de la cola exterior**, y ese BARRIER es
inevitable: el `then` del `if` de la cola lleva la limpieza de `wantstoload`
(`mr r3,rX; li r4,0; bl ~Instance`) delante del `b BeginRule4`, así que `jump.c`
no puede colapsar `ble .L1; …; b BR4; .L1:` a una sola rama. El *cross-jump* que
en el objetivo deja sólo `bgt +0x16b4` **ocurre en `jump2`, DESPUÉS de `loop`**.

**Conclusión: para que la pastilla caiga en +0x110C, el BARRIER de la cola
exterior tiene que estar DENTRO de un bucle. No hay alternativa.**

### 1.2 Las cuatro medidas que lo cierran

Oráculo `c23eax2_probe.py` (insns / bytes / palabras distintas contra el objeto
objetivo / offset de la pastilla) + `c22eax2_pct.py` para el %.
Control `ctrl` = árbol de la r24 = **1877 insns, 7508 B, 311 palabras, +0x110c**.

| # | forma de BeginRule4 (sin bucle) | insns | B | palabras | pastilla | % |
|---|---|---|---|---|---|---|
| `c2_if_base` | `if (n>=0)` + cola base (`if(<=4) goto LRE; goto BR4;`) | 1878 | 7512 | **906** | +0x17b8 | 98,44433 |
| `c1_if_fall` | `if (n>=0)` + **caída implícita** (`if(>4) goto BR4;` y se acaba el bloque) | 1878 | 7512 | **906** | +0x17b8 | 98,44433 |
| `c7_if_invexp` | `if (n>=0)` + `if(>4) goto BR4; goto LRE;` | 1878 | 7512 | 906 | +0x17b8 | — |
| `g4_before_tail` | bucle real que **CIERRA antes** de la cola exterior | 1878 | 7512 | **906** | +0x17b8 | — |

`c1_if_fall` es la que faltaba por medir en las cinco rondas anteriores (la
«cola exterior» se había barrido siempre con un `goto` terminal explícito).
**Veda definitiva: sin bucle que envuelva el `goto BeginRule4` exterior, la
meseta es 906 y la pastilla se queda en +0x17B8.**

### 1.3 Y la deuda se puede pagar en la mitad: el `break` sobra

Tres formas de bucle **sin `break`** dan el objeto EXACTO de la r24:

| # | forma | insns | B | palabras | % |
|---|---|---|---|---|---|
| `c3_for_base` | `int n = …; for (; n >= 0; --n) { … }` (espejo de BeginRule3) | 1877 | 7508 | 311 | 99,60842 |
| `c4_ford_base` | `for (int n = …; n >= 0; --n) { … }` | 1877 | 7508 | 311 | 99,60842 |
| `c5_wh_base` | `while (n >= 0) { … }` **sin `break`** | 1877 | 7508 | 311 | 99,60842 |

El cuerpo **siempre sale por un `goto`**, así que `--n` y la arista de retorno son
inalcanzables: son semánticamente idénticas a `if (n >= 0)`. O sea, el `break` de
la r24 era gratuito. **Pero ninguna de las tres hace falta ya (§2).**

---

## 2. Lo nuevo de esta ronda: **el ALCANCE del bucle decide el reparto r23/r24**

### 2.1 La aritmética, al dígito

`lreg.py` (corregido esta sesión) sobre el pseudo grande —la base de
`AIEnginesWeWantToLoad`—, comparando **la misma función con y sin el bucle**:

| forma | pseudo | n_refs | live_len | prioridad | reg |
|---|---|---|---|---|---|
| **sin bucle** (`c2_if_base`) | 3277 | **136** | 3818 | **2493** | **r23** ← el del objetivo |
| **bucle en todo el cuerpo** (r24) | 3278 | **148** | 3818 | **2713** | r24 |
| vecino que decide | 2868 | 10 | 115 | **2608** | el otro de los dos |

`floor_log2(n)*n/live*10000`: el bucle sube `n_refs` **+12** porque
`propagate_block` de `flow.c` pesa cada referencia por la **profundidad de
bucle**, y el bucle de la r24 mete bajo su nota **las 6 referencias de la cola
exterior Y las 6 del cuerpo del `for (m)`**. Con 148 la prioridad cruza los 2608
del vecino, se asigna antes y se lleva r24: **de ahí las ~85 filas de permutación
global**.

El umbral es exacto: **`n_refs ≤ 142`** (`7*n/3818*10000 < 2608` → `n < 142,25`).

### 2.2 La palanca: un bucle que envuelva la cola pero NO el `for (m)`

Barrido del alcance (todas con `if (n >= 0)` y cola base dentro del bucle):

| # | qué envuelve el bucle | palabras | % | filas |
|---|---|---|---|---|
| `ctrl` (r24) | el cuerpo entero (262-301) | 311 | 99,60842 | 96 |
| `j4_from288` | desde `mapping.Start = …` | 251 | 99,35802 | — |
| `m1_hoist` | desde 288 con `mapping`/`first` **fuera** del bucle | 251 | — | — |
| `j5_from287` = `d1` = `g1/g2/g3` | desde `EngineMappingPair mapping;` (287-301) | 244 | 99,78956 | 29 |
| `j3_from294` | desde `AddMapping(…)` | 243 | 99,58444 | — |
| `j2_from296` | desde `std::find(…)` | 247 | 99,46191 | — |
| **`j1_only_if` = `k2` = `k3` = `m4`** | **sólo la cola (299-301)** | **237** | **99,80821** | **22** |

`j1` deja `n_refs = 137`, prioridad 2511 < 2608 → **el pseudo se lleva r23, como
el objetivo**, y las 85 filas de permutación desaparecen de golpe.

**Meseta de `j1`: cuatro grafías dan el mismo objeto** — `for (;;)`,
`while (true)`, `do {…} while (0)` y `do {…} while (n >= 0)`. Aplicada
**`do {…} while (0)`**, que es la que hace VISIBLE que no es un bucle de verdad.

### 2.3 Lo que APLIQUÉ, y la deuda que queda marcada

```c
BeginRule4: {
    int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
    if (n >= 0) {                       // ← vuelve el `if`; fuera el while+break
        ...                              // (262-298 sin tocar)
        do {                             // ← DEUDA: el original no puede tener esto
            if (FinalEngines.size() + AIEnginesWeWantToLoad.size() <= 4) {
                goto LoadRemainingEngines;
            }
            goto BeginRule4;
        } while (0);
    }
}
```

**LO DIGO CLARO: el `do {…} while (0)` es un constructo que el original no
tenía.** Es más pequeño que la deuda que sustituye (3 líneas en vez de envolver
las 40 del cuerpo), no toca el árbol de ámbitos (§2.4) y vale **+0,200 pp y 74
filas**. Sigue siendo una invención y hay que quitarla cuando aparezca la forma
buena.

### 2.4 Por qué el bucle NO puede llevar declaraciones dentro — el DWARF lo dice

`dwbody.py … both` da el árbol de bloques del ORIGINAL para BeginRule4:

    /* anonymous block */ {          0x800D247C -> 0x800D2770     <- BeginRule4: { … }
        int n;  // r0
        /* anonymous block */ {      0x800D2488 -> 0x800D2770     <- cuerpo del if
            struct engineaudio wantstoload;   // r1+0x98
            struct EngineMappingPair mapping; // r1+0x138   <- las de la COLA,
            unsigned int * first;             // r3            en ESTE mismo bloque
            /* anonymous block */ {  0x800D24D0 -> 0x800D2664     <- for (int m …)
                int m;  // r28
            }
        }
    }

Tres hechos que valen como veda:

1. **La cola NO está dentro del `for (m)`** (el bloque de `m` cierra en
   `0x800D2664` = +0x16D8, justo donde empieza la cola). Queda descartada la
   forma «`for (int m = 0; ; ++m)` extendido que trate el caso agotado dentro».
2. **`mapping` y `first` de la cola viven en el MISMO bloque que `wantstoload`**:
   cualquier bucle que las encierre añade un bloque léxico que el original no
   tiene. Por eso `d1`/`j5` (bucle desde la línea 287) es estructuralmente peor
   que `j1` aunque los dos muevan la pastilla.
3. `regmap.py` sigue diciendo **«MISMO conjunto de locales y mismo árbol de
   bloques»** con la forma aplicada: `j1` no mete ningún bloque (su cuerpo no
   declara nada).

### 2.5 El mapa de líneas del original, leído para este trozo

`lmap.py` sobre el objetivo (`c25eax2_lmap.txt`), con la lista de líneas de
`STATEMGR_CarState.cpp` que emiten código:

| línea orig | dónde | qué es |
|---|---|---|
| 479 / 481 | +0x1130 / +0x1140 | BeginRule3: `int n = size()-1;` y `int m = n-1;` (**una línea entre medias** = nuestro `for (; n>=0; --n) {`) |
| **522** | +0x14f0, +0x14f4 | BeginRule4: `size()` y el `-1`, **en `subic. r0`** — `n` en r0, NO en un callee-saved: no sobrevive a ninguna arista de retorno |
| 528 | +0x1544 | siguiente sentencia (nuestro `for (int m …)`): el original tiene **5 líneas** donde nosotros 3 |
| 551 / 554 | +0x1694 / +0x16a8 | cola interior: `if (…<=4)` y `goto BeginRule4` (**+3**, igual que nosotros) |
| **578 / 581** | +0x17c0 / +0x1130 | cola exterior: `if (…)` y **`goto BeginRule4;` como sentencia propia** → el original tiene **polaridad BASE con `goto` incondicional explícito** |
| 582 / 587 | +0x17e4 | cierre y `while (AIEngines.size() != 0)` de LRE |

Que exista la línea 581 con código propio confirma §1.1: en el original también
hay un `b BeginRule4` con su BARRIER en el momento en que corre `loop`.

---

## 3. Lo que queda de `ResolveCarBanks`: 22 filas, dos racimos

`c22eax2_rows2.py` sobre el objeto aplicado: **22 filas, 19 `ARG_MISMATCH` + 1
`REPLACE` + 1 `DELETE` + 1 `INSERT`** (el trío ya estaba con la forma de la r24 y
`triage` lo normaliza a `faltan 0, sobran 0`).

1. **Filas 1418-1443 (7 filas)** — el racimo `srawi r5/r6` contra `r4/r5` de
   `Vector::erase`/`indexof` (`UTLVector.h`, fichero de otro agente). Es el que la
   r20 anotó, que con la forma de la r24 estaba **tapado** por la permutación
   global y ahora vuelve a verse.
2. **Filas 1659-1777 (15 filas)** — el `for` final sobre `CarSoundConn`:
   `eax_car` (objetivo r22 / nuestro r24) y `found` (r20 / r22), más el
   desplazamiento de una instrucción entre `li r20,0x1` y `addi r27,r1,0x150`.
   `dwbody` lo ve como `eax_car; // r22` contra `// r23` y `found; // r20` contra
   `// r22`. **Es por donde seguiría**: es código limpio, aislado y con nombres.

---

## 4. `Play__16CARSFX_RoadNoise` — eje NUEVO abierto, y agotado: el clamp de `SetId`

**Corrección a las rondas 21 y 22: el clamp que sobrevive en `Play` es el de
`SetId`, no el de `SetType`.** `id` es el único argumento variable
(`Play(FXROADNOISE_LOOP ID, …)` → `new FX_ROADNOISE(ID, 0, 0x1000, 0, …)`); los
otros diez son constantes y sus clamps se pliegan. Las ~13 formas de clamp de las
r21/r22 se barrieron sobre `SetType`.

Y **`SetId` NO lo blinda `InitSFX`**: `InitSFX` construye el objeto con `id`
constante (`FXROADNOISE_LOOP_WETROAD`), así que su clamp se pliega igual sea cual
sea la forma. Medido con `c22eax2_hsw.py` (árbol sombra, los dos símbolos a la
vez): **`InitSFX` sale 100,00000 % en las SEIS formas nuevas.** Es un grado de
libertad que nadie había usado.

No sirve de nada, pero ahora está medido:

| # | forma de `FX_ROADNOISE::SetId` | Play insns | B | palabras | % Play | % InitSFX |
|---|---|---|---|---|---|---|
| `ctrl` | `if (x<0) x=0; else if (x>0xF) x=0xF;` | 97 | 388 | **55** | **94,93877** | 100 |
| **`s3_swap`** | **ramas intercambiadas** (`if (x>0xF) … else if (x<0) …`) | **98** | **392** | **44** | **93,53061** | 100 |
| `s1_twoif` | dos `if` sin `else` | 93 | 372 | 70 | 86,62245 | 100 |
| `s2_direct` | asignación directa a `mData.id` en las tres ramas | 100 | 400 | 63 | 88,58164 | 100 |
| `s4_tmp` | temporal `int t = x;` | 97 | 388 | 55 | — | 100 |
| `s5_le` | `x <= -1` / `x >= 0x10` | 97 | 388 | 55 | — | 100 |
| `s6_else` | `else { if (…) }` anidado | 97 | 388 | 55 | — | 100 |

**`s3_swap` es el DECIMOTERCER caso de «el tamaño exacto miente», y el tercero en
que también miente el recuento de palabras**: da los **392 B clavados** y **44
palabras distintas contra 55**, y objdiff lo puntúa **1,4 pp PEOR**. Mirado fila a
fila es peor de verdad: el clamp pasa de `mr. r8,r29` a `cmpwi`+`li` (una
instrucción más) y **sigue habiendo un solo `li 0`**.

El corte real, releído entero con el diff de filas del árbol:

    objetivo  li r29,0  -> volume(0x8), azimuth(0x10) y el refCount=0 de GetRefCount
                           (vive a traves de las dos CreateInstance -> callee-saved)
              li r0,0   -> wetFX(0x2c), type(0x14), secondaryNoise(0x18),
                           speed(0x1c), hiPass(0x24)   (muere antes de la llamada)
    nuestro   li r30,0  -> los siete campos Y el refCount

Es decir: **al objetivo `cse2` le funde los siete ceros en DOS grupos y a nosotros
en UNO**, y el corte cae entre `azimuth` (4.º argumento) y `type` (5.º). Sigue sin
aparecer la forma de fuente que lo parta. **No he medido nada que cambie eso.**

---

## 5. `BindToData` — mecanismo precisado con el `lreg.py` corregido

No la he movido (95,88236 %), pero el diagnóstico de la r22/r23 («al `-1` le nace
tarde en r0») estaba **incompleto**. Con `lreg.py` ya arreglado:

    pseudo  n_refs  live_len  prio   reg
    160     5       13        7692   r8    <- copia de mCyclePos en el precabezal del for
    142     4       17        4705   r5    <- mCyclePos (add r5,r10,r9)
    153     2       44         454   r8    <- el -1 de mCurrentBlock

La instrucción que sobra, `mr r8,r5`, está **en el precabezal del bucle** (fila
59, justo detrás del `bge` de guardia): es la copia que `loop` crea al izar
`this->mCyclePos` fuera del `for`. En el objetivo **esa copia también existe en
RTL pero los dos pseudos acaban en el MISMO registro duro (r8) y queda como
no-op**, que `flow2` borra. En el nuestro el pseudo 153 (el `-1`) ocupa r8 desde
la instrucción 33 hasta la 55 —solape completo con el rango de 142—, así que 142
cae en r5 y la copia sobrevive.

O sea: **no es «dónde nace el `-1`» sino «el `-1` ocupa r8 mientras vive
`mCyclePos`»**. Las nueve posiciones de `this->mCurrentBlock = -1;` ya barridas
(r21/r23) no lo mueven porque el `stw …,0xac` ya está en la fila correcta y el
`li` se iza igual. Queda por probar cualquier cosa que **acorte el rango del `-1`
por el lado del `li`** sin mover el `stw`, o que **quite la copia del precabezal**
(un puntero local para `mCyclePos` está medido en la r22: idéntico).

---

## 6. Ensayos numerados de esta ronda (33), con su cifra

Control `ctrl` = 1877 insns / 7508 B / 311 palabras / pastilla +0x110c / 99,60842 %.

### Ruta sin bucle (§1)
`c1_if_fall` 906 · `c2_if_base` 906 · `c7_if_invexp` 906 · `g4_before_tail` 906
· `n1_only_goto` 906 · `n2_only_goto_f` 906 (**bucle cuyo cuerpo es sólo un
`goto`: se borra antes de `loop`, como el bucle vacío de la r24**).

### Bucles equivalentes al de la r24 (§1.3)
`c3_for_base` 311 · `c4_ford_base` 311 · `c5_wh_base` 311 · `g5_forn` 311 ·
`d3_do_all` 311.

### Alcance del bucle (§2.2)
`j1_only_if` **237 / 99,80821** · `k2_do0` 237 · `k3_wtrue` 237 · `m4_dowhn` 237
· `j5_from287` 244 · `d1_do_tail` 244 · `d2_do_285` 244 · `g1_forever` 244 ·
`g2_wtrue` 244 · `g3_do` 244 · `j3_from294` 243 · `j2_from296` 247 ·
`j4_from288` 251 · `m1_hoist` 251.

### Vedas nuevas (todas medidas, todas revertidas)
| # | qué | cifra |
|---|---|---|
| `c6_for_fall` | `for(;n>=0;--n)` + cola con caída implícita (**cambia la semántica**: el `--n` pasa a ser alcanzable) | 1881, 7524, **1414** |
| `p2_do_fall` / `p3_for_fall` | bucle sobre la cola con polaridad **invertida** dentro | 1878, 7512, **619** |
| `n3_inv_goto` | polaridad invertida + bucle sólo sobre el `goto` | 1878, 7512, 619 |
| `k1_while_gt` | `while (…>4) { goto BeginRule4; }` (condición REAL en el bucle) | 1878, 7512, **619** — GCC duplica la prueba |
| `k4_forcond` | `for (; …>4; ) { goto BeginRule4; }` | 1878, 7512, 619 |
| `k5_forn_plus` | los dos bucles a la vez | 1878, 7512, 649 |
| `h1_forext` | `for (m)` extendido englobando la cola | error de sintaxis; **descartada por el DWARF (§2.4)** |

**Veda fuerte y nueva: dentro del bucle de la cola la polaridad tiene que ser la
BASE** (`if (<=4) goto LRE; goto BeginRule4;`). Con la invertida son 619 palabras
y una instrucción de más.

---

## 7. Lo que NO he probado

- **`ResolveCarBanks`**
  - **El racimo final (filas 1659-1777)**, que ahora es el 68 % de lo que queda y
    es el único trozo con nombres (`eax_car`, `found`) en el DWARF. **Es por donde
    seguiría.**
  - **El racimo `srawi` de `UTLVector.h`** (filas 1418-1443): es cabecera de otro
    agente y no la he tocado.
  - La forma de fuente natural que ponga un `NOTE_INSN_LOOP_END` detrás del
    `goto BeginRule4` exterior sin envolver el `for (m)`. He medido 14 alcances y
    grafías; **la meseta de 237 sólo la alcanzan bucles cuya condición no emite
    nada** (`for(;;)`, `while(true)`, `do…while(0)`, `do…while(n>=0)`).
  - Bajar `n_refs` del pseudo grande por debajo de 143 **sin** bucle en la cola
    (haría falta quitar 6 referencias reales a `AIEnginesWeWantToLoad`).
  - Subir la prioridad del vecino (pseudo 2868, 10 refs / 115 de vida): con **una
    referencia más** (11 refs → 2869 > 2713) también giraría el reparto, pero no
    he identificado qué local es ni he encontrado dónde referenciarla sin emitir.
  - El permutador, ni guiado ni ciego.
- **`Play`**: no he probado nada que parta la cadena de `cse2` entre `azimuth` y
  `type`; ni `lreg` guiado sobre los pseudos de los ceros; ni el permutador. Las
  seis formas de `SetId` están medidas y **ninguna sirve** (pero `InitSFX`
  aguanta las seis: el eje está abierto).
- **`BindToData`**: no he medido ninguna variante. Queda acortar el rango del `-1`
  por el lado del `li`, y quitar la copia del precabezal.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, los bloques
  `__ANDROID__`, ni ningún fichero de otro agente. **`ENVIRO_AEMS.h` NO está
  modificado**: el barrido de `SetId` se hizo con árbol sombra. **Sin commit.**

## 8. Herramientas dejadas en el scratchpad (prefijo `c25eax2_`)

| | |
|---|---|
| **`c25eax2_lreg.py <cpp> <tag>`** | la tabla de allocnos de `lreg.py` **para un `.cpp` suelto** (stub + cflags reales + `cc1plus -dl -dg`), reusando el parser de `scripts/lreg.py` sin ejecutar su `main()`. Es lo que permite comparar `n_refs`/`live_length` entre dos variantes **sin tocar el árbol** |
| `c25eax2_helper.py` | líneas exactas de `c25eax2_base.cpp` (finales mixtos: 395 CRLF + 4 LF) |
| `c25eax2_v1..v9.py` | las nueve tandas de variantes de esta ronda |
| `c25eax2_h1.py` | el barrido de `SetId` para `c22eax2_hsw.py` (mide `Play` **e** `InitSFX`) |
| `c25eax2_lmap.txt` | `lmap.py` del objetivo, base de la tabla de §2.5 |
| `c25eax2_dwboth.txt` | los dos cuerpos DWARF enteros con rangos (§2.4) |
| `c25eax2_rows*.txt` | filas del diff de `ctrl`, `d1` y `j1` |

Se reusan sin cambios `c23eax2_probe.py`, `c22eax2_pct.py`, `c22eax2_rows2.py`,
`c22eax2_hsw.py`, `c21eax2_var.py` y `c21eax2_fn.py`.

## 9. Convivencia

Entré con el disco al 98 % (14 GB libres) y salgo igual (14 GB). Borrados: los
volcados `.ii*` por pase de la r22 (110 MB), los `.o` de variante de las rondas
r21/r22/r23 ya cerradas (**602 MB**) y mi propio `rtl25/` (87 MB). El scratchpad
baja de 3,4 GB a **2,8 GB** — el resto es de otros agentes y no lo he tocado.

El único fichero del árbol que he tocado es
`src/Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.cpp`, más
`docs/congelado/Speed__Indep__SourceLists__zEAXSound2.json` y este documento.
Al cerrar, `git status src/Speed/Indep/Src/EAXSound/` sólo lista ese `.cpp`: la
huella congelada no lleva trabajo en curso de nadie más.
