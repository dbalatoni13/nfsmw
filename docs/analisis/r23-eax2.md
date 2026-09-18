# Ronda 23 — zEAXSound2: los tres near-miss

**Resultado: +0 B, +0 funciones. NADA aplicado al árbol.** `zEAXSound2` sigue en
**158.400/170.256 B = 93,0364 %, 921 funciones al 100 %**. `frozen.py chk` decía
**«idéntico al congelado»** a mitad de sesión y al final dice **«HA CAMBIADO»**:
no es mío, es `GameFlow.hpp` de otro agente — **§8**, con el impacto medido (cero
en código).

`audit.py Speed/Indep/SourceLists/zEAXSound2`: **921 ok / 0 FALLA** (una pasada;
sin FALLA que confirmar con la segunda). Encargo verificado al dígito con
`build_direct.py` + `triage.py`:

    7508 B  99.124%  ResolveCarBanks__18CSTATEMGR_CarState  faltan 1, sobran 1, 1 SUST  bgt-1, ble+1
     340 B  95.882%  BindToData__14GinsuSynthDataPv         faltan 0, sobran 1          mr-1
     392 B  94.939%  Play__16CARSFX_RoadNoise…LOOPi         faltan 1, 0 sobran, 1 SUST  li+1

**83 variantes numeradas medidas.** Lo que sale de la ronda son **el mecanismo
completo de la colocación de bloque, con volcado RTL y ensamblador con marcas de
ámbito**, el **mapa exacto de las líneas que le faltan a nuestra fuente**, dos
vedas rotas y siete vedas nuevas.

---

## 0. Lo primero: el arnés miente si no se corrige la descodificación de ramas

**En un `.o` SIN ENLAZAR el campo de desplazamiento de una rama trae el OFFSET DE
SECCIÓN del destino, no el desplazamiento.** Medido en los tres objetos:

| objeto | `st_value` de la función | palabra en +0x1104 | destino real |
|---|---|---|---|
| OBJETIVO (`build/GOWE69/obj/…`) | 0x6030 | `0x408106e0` → campo 0x06e0 | relativo: +0x17e4 |
| NUESTRO (`build/GOWE69/src/…`) | 0x6054 | `0x40817838` → campo 0x7838 | **0x7838 − 0x6054 = +0x17e4** |
| stub de `c21eax2_var.py` | 0x0548 | `0x40811d2c` → campo 0x1d2c | **0x1d2c − 0x0548 = +0x17e4** |

El objeto del troceador viene ya resuelto (relativo); los nuestros no. Sin restar
`st_value` **todos los destinos de salto salen mal** y no se puede leer la
colocación. Ésa es la razón de que `ARBOL` y `STUB` difieran en **212 palabras y
las 212 sean ramas**: el código es el mismo.

## 1. Herramientas nuevas (scratchpad, prefijo `c23eax2_`)

| | |
|---|---|
| **`c23eax2_lines.py`** | **alinea fila a fila la línea de fuente del OBJETIVO (de `debug_lines.txt`) con la NUESTRA (el `line_number` que objdiff trae en el lado `right`).** Modo `delta` da la curva de desfase, modo `all` el listado emparejado. Es la que resuelve el punto 1 del encargo |
| **`c23eax2_asm.py`** | el ensamblador de `ResolveCarBanks` **con las etiquetas `.L_B*` de contorno de binding**. Es lo que deja ver a qué ámbito pertenece cada bloque de limpieza |
| `c23eax2_probe.py` | como `c22eax2_rcb.py` pero además **vuelca la región de código pedida** y detecta la **pastilla** del objetivo (`mr r3,r20 / b / mr r3,r31`). Descodificación de ramas corregida (punto 0) |
| `c23eax2_dis.py` | descodificador PPC mínimo compartido, con el parámetro `V` |
| `c23eax2_dump.py` / `c23eax2_dumpb.py` / `c23eax2_side.py` | vistas estructurales de un `.o` o del json de filas |
| `c23eax2_ms.py` / `c23eax2_mc.py` | compilan un `.cpp` **mínimo** con los cflags de la unidad: **1 s por variante** |
| `c23eax2_helper.py` / `c23eax2_gh.py` | extracción de líneas exactas con finales mixtos (`STATEMGR_CarState.cpp`: 394 CRLF + 4 LF) |

---

## 2. Punto 1 del encargo: las líneas que faltan, MAPEADAS — y son todas muertas

`c23eax2_lines.py delta` da la curva de desfase entre la línea del objetivo y la
nuestra, fila a fila. **La cifra exacta no es ~220: el cuerpo del original ocupa
las líneas 167..688 (522) y el nuestro 110..340 (231); le sobran 291 líneas.**

Dónde están (saltos del desfase, filtrando las filas cuya línea nuestra cae fuera
del cuerpo porque son expansiones de cabecera):

| nuestra línea | desfase | salto | qué hay ahí en nuestra fuente |
|---|---|---|---|
| 110 | +57 | — | cabecera de la función |
| 129 → 133 | +66 → +80 | **+14** | fin del `if(...CONTEXT...) continue;` → `GetEngineInfo()` |
| 143 → 148 | +87 → +96 | **+9** | `if (size()==0) return;` → `NumEnginesWeWantToLoad = …` |
| **156 → 162** | **+88 → +162** | **+74** | cierre de `if (DEBUG_CAR_BANK_TEST_CASE == -1)` → `std::sort` |
| 171 / 176 / 179 | +172 / +185 / +195 | +8 / +8 / +10 | dentro de BeginRule3-previo y del bloque `CopsCanBeInGame` |
| 187 / 206 / 213 | +204 / +223 / +238 | +9 / +8 / +10 | ídem |
| 245 / 277 / 296 | +254 / +269 / +277 | +7 / +7 / +9 | tras los `AddMapping` |
| 316 / 339 | +294 / +314 | +11 / +10 | en `LoadRemainingEngines` |

**Y todos esos huecos son código que NO EMITE NADA**: el mapa de líneas del
objetivo sólo tiene entradas para instrucciones emitidas, así que 291 líneas sin
una sola instrucción son comentarios, `#if 0`, o bloques bajo
`DEBUG_PRINT_CAR_BANK_RESOLVE` (que es `static const int = 0`). El hueco grande
(74 líneas entre nuestro 156 y nuestro 162) es el sitio del *else* de
`DEBUG_CAR_BANK_TEST_CASE`, y **no emite ni una instrucción**, así que en el
original no puede ser un `else` de verdad.

**Corolario medido**: reconstruir esas líneas no puede mover la colocación **por
la vía de añadir código**, y por la vía de añadir *ámbitos con destructores* está
medido abajo (ensayos `c1`-`c7`, `f1`-`f5`, `n6`, `n7`): o es neutro o empeora.
**El hilo está tirado; no lleva a ningún sitio.** Lo dejo cerrado.

---

## 3. Punto 3 del encargo: el mecanismo de la colocación, completo

### 3.1 Qué es exactamente `+0x110C`

Es el hueco entre el `b BeginRule2` que cierra el bloque
`if (CopsCanBeInGame && FinalCopV8Engines.size() == 0)` y la cabecera de
`BeginRule3`. Ahí es donde GCC **aparca bloques de limpieza muertos**, que luego
el *cross-jumping* convierte en vivos.

Lo prueba `c23eax2_asm.py`: los dos bloques aparcados de nuestro `f1` (`.L58605`
y `.L58604`) están **dentro** de los contornos `B647`/`B644`/`B428`/`B425`, justo
antes de sus marcas de cierre:

    .L_B719_e: .L_B714_e:
        b .L57967                          <- goto BeginRule2 (nuestra línea 227)
    .L58605:  mr 3,31 ; li 4,0 ; bl ~ ; b .L58615
    .L58604:  mr 3,20 ; li 4,0 ; bl ~ ; mr 3,17 ; li 4,0 ; bl ~
    .L_B647_e: .L_B644_e: .L_B428_e: .L_B425_e:
    .L58025:                               <- BeginRule3

`B644` abre en `BeginRule2:` y `B647` en `if (EnginesThatCanUpgradeToV8.size() != 0) {`.
Y los saltos que llegan a `.L58604`/`.L58605` están en las líneas **3928** y
**4508** del ensamblador, es decir **mucho después**: son saltos hacia atrás.

**`.L58604`/`.L58605` no existen en el RTL post-expand**: sus números son mayores
que cualquier etiqueta de la función. Los crea el *cross-jumping*.

### 3.2 El RTL lo confirma: son empalmes hacia atrás

Volcado `-dr` (receta de `r22-ae.md` §2; 25 s, 53 MB, luego borrado) y reducido a
la cadena de insns. Detectando los tramos donde el `uid` deja de crecer salen
**dos empalmes grandes**:

| posición en la cadena | uid | tamaño |
|---|---|---|
| idx 3077..3150 | 5427..5694 | 74 insns, **2 llamadas al destructor** |
| idx 3682..3714 | 7520..7584 | 33 insns, **1 llamada al destructor** |

El segundo empalme entra **entre el `uid` 6780 y el `uid` 6645**, y empieza con
`NOTE_INSN_DELETED` + dos `NOTE_INSN_BLOCK_BEG`: es exactamente el bloque
sintético que `expand_fixup` crea para un `goto` pendiente, con las limpiezas
metidas después con `emit_insns_after (cleanup_insns, f->before_jump)`. **Las
limpiezas se generan tarde y se empalman en el sitio del fixup**, que está al
final del bloque `CopsCanBeInGame`.

Y el sitio del salto real (`bgt .L58604` en la línea 3928 del `.s`) queda rodeado
de **~20 contornos `.L_B850`…`.L_B870_e` VACÍOS**: ahí no quedó ninguna limpieza.

### 3.3 La regla, medida sobre las 12 combinaciones de polaridad

Las tres colas (BeginRule3, interior de BeginRule4, exterior de BeginRule4) en
sus dos formas, 12 combinaciones (`e_<br3><br4in><br4out>`, `i`=invertida,
`b`=base, `n`/`i`/`e` = exterior normal/invertida/con `goto` explícito):

| variante | insns | B | palabras dist. | % objdiff | pastilla |
|---|---|---|---|---|---|
| `e_iin` = **f1 (árbol)** | 1877 | 7508 | **609** | **99,12360** | NO |
| `e_iii`, `e_iie` | 1877 | 7508 | 609 | — | NO |
| `e_ibn/i/e` (= `e0` de la r22) | 1877 | 7508 | 612 | 98,61748 | NO |
| `e_bin/i/e` (= `f2` de la r22) | 1878 | 7512 | 903 | — | NO |
| `e_bbn/i/e` (= base de la r22) | 1878 | 7512 | 906 | 98,44433 | **+0x17b8** |

**Veda nueva y fuerte: la forma de la cola EXTERIOR de BeginRule4 es
IRRELEVANTE.** Las tres formas dan resultados idénticos en las cuatro
combinaciones (12 medidas). Eso extiende y explica las 10 formas de la r22.

Y la regla que sale:

- **La limpieza del `goto` que va en el `then` de una cola INVERTIDA se aparca en
  `+0x110C`.** `e_ib` aparca ahí la de `goto BeginRule3` (`mr r3,r20; …; mr r3,r17; …`,
  6 insns, cae en BeginRule3); `e_bi` aparca la de `goto BeginRule4` (`mr r3,r31; …; b`,
  4 insns); `f1` aparca **las dos** (10 insns). La base no aparca nada.
- **La fusión de las dos limpiezas de `goto LoadRemainingEngines` (la PASTILLA de
  dos entradas, 9 insns) sólo existe cuando NINGUNA cola está invertida**, y
  entonces vive **justo antes de `LoadRemainingEngines`, en `+0x17b8`**.

### 3.4 Y por eso el objetivo no sale de ninguna de las 12

El objetivo tiene **la pastilla de la base**, byte a byte, pero **en `+0x110C`**:

    objetivo  +0110c  mr r3,r20 ; b +0x1118 ; mr r3,r31 ; li r4,0 ; bl ; mr r3,r17 ; li r4,0 ; bl ; b +0x17e4
    base      +017b8  mr r3,r20 ; b +0x17c4 ; mr r3,r31 ; li r4,0 ; bl ; mr r3,r17 ; li r4,0 ; bl ; b +0x17e8

y con las colas en polaridad de base: `ble +0x110c` en la cola de BeginRule3
(fila 1324, **la sustitución que queda**) y `ble +0x1114` en la interior de
BeginRule4, con las limpiezas de `goto BeginRule3`/`goto BeginRule4` **en línea y
completas**. Además el objetivo comparte la cola de `goto BeginRule4` **al revés
que la base**: la suya vive en `+0x16b4` (el sitio TEMPRANO, el de la cola
interior) y la de la base en `+0x17a8` (el TARDÍO) — el mismo efecto de
`do_cross_jump` que describe la r20, con el superviviente cambiado.

**O sea: el objetivo es la BASE con la pastilla movida de «antes de
LoadRemainingEngines» a «antes de BeginRule3». Ninguna forma de las tres colas lo
consigue.**

### 3.5 La forma base NO aparca nada en `+0x110C` — comprobado, y la trampa que casi me cuela una conclusión falsa

Repetí el volcado `-dr` sobre la forma **base** (`c23base_bb.cpp`, verificada:
1878 insns / 7512 B / 906 / pastilla en +0x17b8) para ver si la pastilla llegaba
a generarse en el hueco de `+0x110C` y luego se moría. **Primera lectura del RTL:
sí — hay una limpieza que termina en `b <etiqueta>` y la etiqueta parecía
`LoadRemainingEngines`. ES FALSA.**

**TRAMPA NUEVA, y que cuesta una conclusión entera: los NÚMEROS DE ETIQUETA
(`.L58025`) son un contador GLOBAL por compilación.** Entre dos compilaciones
distintas la misma etiqueta lógica cambia de número, así que **un `uid`/número de
etiqueta de un volcado NO se puede comparar con el de otro**. Todo el
razonamiento «la limpieza A está donde la pastilla» venía de emparejar
`L58025` entre los dos volcados.

Comprobado como se debe, con `c23eax2_asm.py` sobre la propia forma base — el
ensamblador con marcas de ámbito, que no depende de emparejar números:

    .L_B719_e: .L_B714_e:
    .L_LC906:
        b .L57974                <- goto BeginRule2
    .L_B647_e: .L_B644_e: .L_B428_e: .L_B425_e:
    .L58032:                     <- BeginRule3
        lwz 0,8(23)

**Entre el `goto BeginRule2` y la cabecera de BeginRule3 no hay NADA**: los
cuatro contornos cierran seguidos. La forma base **no aparca ninguna limpieza en
ese hueco**; la única pastilla que genera es la de `+0x17b8`. Coincide con el
binario, y con `f1`, donde en el mismo sitio sí hay dos bloques (`.L58605` /
`.L58604`) entre `b .L57967` y `.L_B647_e`.

Es decir: **el hueco de `+0x110C` sólo se llena cuando una cola está invertida, y
lo que cae ahí es siempre la limpieza del `goto BeginRuleN`.** Queda sin explicar
cómo el objetivo mete ahí la limpieza de `goto LoadRemainingEngines`, y ninguna
de las 83 formas medidas lo consigue.

---

## 4. Ensayos numerados (77), con su cifra

Oráculo: `c23eax2_probe.py` (insns / bytes / palabras distintas contra el objeto
objetivo / pastilla). Control `ctrl` = árbol actual = **1877 insns, 7508 B, 609,
99,12360 %** — reproduce al dígito las cifras de la r22.

### Retest sobre `f1` de vedas medidas sobre la BASE

| # | qué | cifra |
|---|---|---|
| `b1_declord` | **orden de declaración `HighPriority`/`LowerPriority` invertido** (r20 `r6`) | 1876 insns, 7504 B, 723, **98,91209 %** — peor |
| `b2_dbg_del` | **borrar los dos bloques `if (DEBUG_PRINT_CAR_BANK_RESOLVE)`** (r21 `r12`) | 1877, 7508, **642**, **99,10123 %** |
| `b3_dbg_move` | moverlos detrás de `AddMapping` (r21 `r11`) | idéntico (609) |
| `b4_lre_expl` | `goto LoadRemainingEngines;` explícito tras la cola exterior (r20 `r5`) | idéntico |

**VEDA ROTA, con matiz: los bloques `DEBUG_PRINT_CAR_BANK_RESOLVE` NO cuestan
cero.** Sobre `f1` borrarlos vale **−0,022 pp** (99,12360 → 99,10123) y **33
palabras**. La medida de la r21 («mismo md5») era sobre la base, donde sí eran
inertes. **Decimocuarta veda caída por estar medida sobre la sentencia (o la
base) equivocada.** El coste es pequeño, pero el bloque **no es libre**: la forma
que tenemos (**exactamente dos `engineaudio` en un bloque**) es la buena.

### Estructura de ámbitos (nuevo)

| # | qué | cifra |
|---|---|---|
| `b5_br3_inner` | llaves extra alrededor de los dos `engineaudio` de BeginRule3 | idéntico |
| `b7_no_blk3` / `b8_no_blk4` | etiqueta y `{` en líneas separadas | idénticos |
| `n1_emptyblk` | bloque `{}` vacío justo antes de `BeginRule3:` | idéntico |
| `n4_statout` | `static int LastV8Used` sacada del bloque | idéntico |

### Bloques `DEBUG_…` añadidos o cambiados

| # | qué | cifra |
|---|---|---|
| `c1_push2` | bloque DEBUG con 2 `engineaudio` tras el `push_back` de BeginRule3 | 1876, 7504, **906** |
| `c2_eras2` / `c3_if2` / `c4_push1` / `c6_p2mv` / `c7_add3` | el mismo bloque en otras cinco posiciones de BeginRule3 (1 y 2 objetos) | los cinco 1876, 7504, **906** |
| `c8_dbg4x3` | partir el bloque de BeginRule4 en uno de 2 + uno de 1 objeto | idéntico (609) |
| `n6_dbg3mv` / `n7_dbg3` | **3** `engineaudio` en el bloque de BeginRule4 | 1877, 7508, **642** (igual que borrarlos) |

**Veda: cualquier bloque `DEBUG_…` en BeginRule3 cuesta −1 instrucción y 297
palabras.** Y el de BeginRule4 sólo vale con **dos** objetos.

### Bloque muerto con `goto` para plantar un destino de cross-jump

Idea: sembrar en `+0x110C` un bloque muerto `dtor; dtor; b LoadRemainingEngines`
dentro de `if (DEBUG_PRINT_CAR_BANK_RESOLVE)` para que el *cross-jumping* funda
en él las dos limpiezas reales.

| # | qué | cifra |
|---|---|---|
| `f1_dead2` / `f2_dead1` / `f4_dead2b` / `f5_obj2` | 1, 2 objetos, con `goto LRE`, con `goto BeginRule4`, sin `goto` | los cuatro **idénticos** al control |
| `f3_dead3` | 3 objetos | 1876, 7504, 906 |

**Veda: el código dentro de `if (DEBUG_PRINT_CAR_BANK_RESOLVE)` se borra ANTES
del cross-jumping.** No se puede usar para plantar un destino de fusión.

### Etiqueta puente (`LRE_hop`)

Idea: `goto BeginRule3; LRE_hop: goto LoadRemainingEngines;` justo antes de
`BeginRule3:`, y las colas saltando a `LRE_hop`, para que la limpieza se aparque
delante de la nueva etiqueta.

| # | qué | cifra |
|---|---|---|
| `g1_b_hop3` … `g5_b_hop3b` (base) | 1878, 7512, 906, pastilla en +0x17b8 — **idéntico a la base** |
| `g2_i_hop3` / `g6_i_hop3a` (invertida) | 1877, 7508, 609 — **idéntico a `f1`** |

**Veda: la indirección de etiqueta es INVISIBLE**; `jump.c` la enhebra
(*jump threading*) antes de decidir nada. (Sin el `goto BeginRule3;` de guardia
la variante es semánticamente distinta y da 1438 insns: no cuenta.)

### Punto 2 del encargo: el `size()` que le falta al censo DWARF

`dwbody.py zEAXSound2 "CSTATEMGR_CarState::ResolveCarBanks"` (con `regmap.py
--list` antes, como manda el encargo) **confirma y precisa** el hallazgo de la
r22. En el bloque anónimo que contiene `int LastV8Used` (= el cuerpo de
`if (CopsCanBeInGame && FinalCopV8Engines.size() == 0)`):

    - orig:  int LastV8Used;  +  TRES  Vector<unsigned int,16>::size()
    + ours:  int LastV8Used;  +  UNA   size()      ... y otra UN NIVEL MÁS ADENTRO,
                                                       junto a `unsigned int *found`

O sea: **al original le sobran dos `size()` a nivel de bloque y a nosotros nos
sobra una un nivel más adentro** (la de
`LastV8Used %= EnginesThatAreV8.size();`, que en el original vive fuera del
`if (EnginesThatAreV8.size() != 0)`). Y encaja con el mapa de líneas: entre
nuestra 176 y nuestra 179 el original tiene **+10 líneas**.

| # | qué | cifra |
|---|---|---|
| `q1_two` | bloque DEBUG con **dos** `EnginesThatAreV8.size()` tras `static int LastV8Used` | **md5 idéntico** al control |
| `q2_loop` | bloque DEBUG con un `for` sobre `size()` y un `engineaudio` | **md5 idéntico** |
| `q3_one` | bloque DEBUG con **una** `size()` | **md5 idéntico** |
| `q4_before` | el bloque de dos justo antes del `if (EnginesThatAreV8…)` | **md5 idéntico** |
| `q6_upg` | ídem con `EnginesThatCanUpgradeToV8.size()` | **md5 idéntico** |
| `q5_modout` | sacar `LastV8Used %= …size();` del `if` (semánticamente distinto) | 1877, 7508, **626** — peor |

**Veda: reconstruir los dos `size()` que le faltan al censo cuesta EXACTAMENTE
CERO** (md5 `bdd324967dacf62d`, el mismo que `ctrl`, `b3_dbg_move` y
`n1_emptyblk`). Es la corrección estructural que documenta la r22, y **no mueve
un byte**. La única forma que sí mueve —sacar el `%=` del `if`— **rompe la
semántica** (división por cero cuando el vector está vacío) y empeora. **Punto 2
del encargo cerrado: no es un frente.**

### Colas de BeginRule2 y formas de doble `if`

| # | qué | cifra |
|---|---|---|
| `d1_inv2` / `d2_else2` / `d4_blk2` | cola de BeginRule2 invertida, con `else`, envuelta en `{}` | idénticos |
| `d3_nolre` | cola de BeginRule2 sin el `goto LoadRemainingEngines` (control negativo) | 887 |
| `h1_twoif` | los dos `goto` como **dos `if` separados** (los dos en el `then`) | 1878, 7512, 906 = base |
| `h2_belse` | base con `else` explícito | 1878, 7512, 906 = base |
| `h5_bdbgmv` | base + bloques DEBUG detrás de `AddMapping` | 1878, 7512, 906 = base |
| `h6_i_twoif` | doble `if` sólo en BeginRule3 | 1878, 7512, 903 |

**Veda: la meseta de la base es exactamente 906 y la de `f1` exactamente 609.**
Nada de la estructura del `if` la mueve; sólo la polaridad de las dos colas
interiores (§3.3).

---

## 5. `BindToData` (340 B, 95,88236 %) — duodécimo «el tamaño exacto miente»

El diff, releído fila a fila con `c23eax2_side.py`, es **una instrucción y una
causa**:

    objetivo  +000c4  mr r7,r0        (minperiod = mSampleCount, libera r0)
              +000c8  li r0,-1        <- nace TARDE, vive 4 insns, se lleva r0
              +000d8  stw r0,0xac(r31)
              +000e4  bge …           (la base del bucle YA está en r8: add r8,r10,r9 en +0xa4)

    nuestro   +00084  li r8,-1        <- nace en la 3.ª insn y OCUPA r8
              +000a8  add r5,r10,r9   (la base cae en r5)
              +000d8  stw r8,0xac(r31)  (el ORDEN de los stw ya coincide fila a fila)
              +000e8  mr r8,r5        <- LA instrucción que sobra

**Diagnóstico de pase: NINGUNO lo mueve.** `-fno-schedule-insns`,
`-fno-schedule-insns2`, `-fno-gcse` y `-fno-rerun-cse-after-loop` dan **los
cuatro** el `li r8,-1` en `+0x84`. O sea que la posición ya viene de la expansión
y no la decide el planificador: no es un frente de banderas.

Ensayos (oráculo: palabras distintas + `%` de objdiff):

| # | qué | insns | B | palabras | % |
|---|---|---|---|---|---|
| `k0_ctrl` | árbol | 86 | 344 | 55 | 95,88236 |
| `k1_after` | `mCurrentBlock = -1;` **detrás** de `int minperiod` | 86 | 344 | 55 | — |
| `k4_nb` / `k5_nbsplit` | local `int nb = -1;` (arriba y detrás de `minperiod`) | 86 | 344 | 55 | — |
| `k3_afterfor` | borrar la sentencia (control negativo) | 83 | 332 | 40 | — |
| **`k2_inloop`** | la asignación **dentro del `for`** (semánticamente distinta) | **85** | **340** | **29** | **91,62353** |
| `p1_tras146` | `-1` justo detrás de `mSampleCount = …` | 85 | 340 | 32 | 93,61176 |
| `p5_mp146` | `-1` + `int minperiod` los dos detrás de `mSampleCount` | 85 | 340 | 32 | **93,61176** |
| `p6_mp_cb146` | ídem con el orden inverso de esas dos sentencias | 85 | 340 | 32 | **93,61176** |
| `p2_tras147` | `-1` detrás de `mSampleRate = …` | 86 | 344 | 56 | **95,80000** |
| `p3_tras148` / `p4_tras149` | `-1` detrás de `mFreqPos` / de `mCyclePos` | 86 | 344 | 56 | — |

**`k2` da los 340 B clavados y baja el recuento de palabras de 55 a 29 — y
objdiff da 91,62 % contra 95,88 %.** Es el **duodécimo** caso medido de «el
tamaño exacto miente» y el **segundo** en que también miente el recuento de
palabras (el primero fue el `b06` de la r21, con 32). Confirma que en esta
función **el recuento de palabras no ordena**: hay que puntuar con objdiff.

Y cierra el único punto que la r22 dejaba abierto («formas que dejen el `stw`
tarde moviendo sólo el `-1`»): **`k1`, `k4` y `k5` mueven sólo el `-1` y las tres
son idénticas a la base.** El `li` no se puede retrasar desde la fuente sin
mover también el `stw`.

**La meseta, ahora completa y con `%` en vez de md5**: la posición de
`this->mCurrentBlock = -1;` sólo tiene **tres** resultados posibles —
`≤ mSampleCount` → 340 B / 32 palabras / **93,61176 %**; entre `mSampleRate` y
`mSampleData` → 344 B / 56 / **95,80000 %**; a partir de `mSampleData` (que es
donde está) → 344 B / 55 / **95,88236 %**. La posición actual es **la mejor de
las tres**. Y mover `int minperiod` con ella (`p5`, `p6`) no cambia nada.

`lreg.py Speed/Indep/SourceLists/zEAXSound2 "GinsuSynthData::BindToData"` — **ojo:
`lreg.py` quiere el nombre DESMANGLADO**, con el mangling falla y escupe la lista
de 900 símbolos. Da los 42 pseudos; los dos que importan son
`160 (n_refs 5, live_len 13, prio 7692, r8)` y `142 (n_refs 4, live_len 17, prio
4705, r5)`. **No hay empate**: la diferencia de prioridad es 3.000, así que la
palanca de `_bOutput` (bajar `n_refs` sacando sentencias de un bucle) no aplica
tal cual. Falta identificar qué pseudo es cada uno con el RTL.

---

## 6. `Play__16CARSFX_RoadNoise` — no tocada esta ronda

No he medido ninguna variante nueva. Sigue vigente el diagnóstico de la r21/r22
(dos ceros contra uno, corte en el 5.º argumento) y el **AVISO** del encargo:
quitar el clamp de `SetType` da los 392 B exactos y rompe `InitSFX`.

---

## 7. Lo que NO he probado

- **`ResolveCarBanks`**
  - Los **dos racimos de registro** (filas 1418-1443, `srawi r5/r6` contra
    `r4/r5`; filas 1672-1700, `r22`/`r24`, `r20`/`r22`, `r24`/`r21`). Siguen
    intactos desde la r20. **Y son 30 filas de las ~31 malas: aunque se cierren,
    la SUST de la fila 1324 sigue, y `matched_code` es todo-o-nada.**
  - El permutador, ni guiado ni ciego.
  - `lreg.py` sobre los pseudos de esta función.
  - Volcados RTL de los pases **posteriores** a `expand` (`.jump`, `.jump2`), que
    es donde se ve el cross-jumping decidir el superviviente. Sólo he sacado
    `-dr` (de `f1` y de la base).
  - **El pase que decide**: `-dr` da la foto post-expand, y ahí el hueco de
    `+0x110C` ya está lleno (en `f1`) o ya está vacío (en la base). Falta ver
    `jump.c` en acción (`-dj`, `-dJ`) para saber si el objetivo llena ese hueco
    en `expand` o lo llena el cross-jumping. **Es lo primero que haría en la r24**,
    y ahora hay dos volcados de referencia con los que contrastar.
  - Reescribir BeginRule3/BeginRule4 de forma que `goto BeginRule3`/`goto
    BeginRule4` sean saltos **hacia delante** (es lo que, según §3.3, decidiría
    qué limpieza se aparca). No se me ha ocurrido una forma que conserve la
    semántica sin meter un bucle, y los bucles ya están vedados por la r20
    (`r1`/`r2`/`r3`, y `for(;;)` con `continue`/`break` = 1880 insns).
- **`BindToData`**: el permutador; identificar los pseudos 160/142 en el RTL;
  restricciones de registro (último recurso, sin tocar).
- **`Play`**: todo — no la he tocado.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, los bloques
  `__ANDROID__`, ni ningún fichero de otro agente.

## 8. AVISO: `frozen.py` de zEAXSound2 ha cambiado, y NO es mío

Al cerrar, `frozen.py chk Speed/Indep/SourceLists/zEAXSound2` pasó de
**«idéntico al congelado»** (18:35) a **«*** HA CAMBIADO ***»** (18:47), con un
`build_direct.py` mío en medio y **sin que yo tocara un solo fichero del árbol**
(`git status src/Speed/Indep/Src/EAXSound/` sale vacío toda la sesión).

**Causa localizada**: otro agente ha añadido dos funciones a
`src/Speed/Indep/Src/Misc/GameFlow.hpp` (`IsGameFlowLoadingGame`,
`IsGameFlowPaused`, `+8 líneas`), y **zEAXSound2 incluye esa cabecera**:
preprocesando `zEAXSound2.cpp` con sus cflags reales, `GameFlow.hpp` aparece **2
veces** y `IsGameFlowLoadingGame` **1 vez**.

**Impacto medido — ninguno en el código:**

| | |
|---|---|
| `measure.py zEAXSound2` | 158.400/170.256 B = **93,0364 %, 921 fns** — idéntico |
| `triage.py` | los mismos 3 near-miss con los mismos %, y **MURO: 6 funciones, 3.616 B** — idéntico |
| `audit.py` | **921 ok / 0 FALLA**, dos pasadas (una antes del cambio y otra después) |
| símbolos emitidos | **no aparece ninguno nuevo**: GCC 2.95 no emite las dos `inline` libres sin usar (`GameFlow` en nuestro `.o`: sólo `g_EAXIsPaused__Fv` y `GameFlowSndState`, igual que antes) |

**NO he vuelto a congelar**, porque congelar ahora metería en la huella de mi
unidad el estado *en curso* de otro agente. Que lo decida quien cierre la ronda.
Y sirve de aviso general: **`GameFlow.hpp` llega a zEAXSound2**, así que quien la
toque tiene que medir esta unidad, como manda la regla de «cabecera compartida =
A/B por objetos».

## 9. Aviso de convivencia

El scratchpad estaba en **6,0 GB** al empezar (disco al 98 %, 9,7 GB libres).
Borrados los volcados de la ronda 22 ya cerrada (`*.rtl`, `mnd_r_zAI_*.json`,
`c22ae_sw_*.json`, `d_z*.json`, `tryu_*.json`): **6,0 → 2,5 GB**. El directorio
es **compartido entre agentes de la misma tanda** (había un `c23ecs_antes.json`
recién escrito), así que sólo he borrado ficheros con prefijo de la r22.
El volcado `-dr` de esta unidad ocupa **53 MB**: bórralo al acabar.
