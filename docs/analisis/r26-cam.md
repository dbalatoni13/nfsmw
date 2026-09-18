# Ronda 26 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** El árbol queda **byte a byte como lo encontré**
(md5 de `UTLVector.h` idéntico al de partida; `git status` de
`src/Speed/Indep/Libs/` y `src/Speed/Indep/Src/Camera/` vacío). `audit.py`:
**446/446 ok, cero FALLA**, dos pasadas.

Lo que traigo:

1. **La palanca nº 1 del brief está MEDIDA y NO funciona.** «Una rama sobre
   `srcBeg` detrás del último bucle» da **97,8166 %, 1160 B, 17 diffs** (la base
   son 16): el racimo de cabeza sale **idéntico**, sin `mr.`. Y ahora se sabe por
   qué al dígito. (§1)
2. **La corrección de la r25 al diagnóstico de `_Storage` era INCORRECTA.**
   `firstr` en la insn del copiado es **`reg100`** —el temporal de `begin()`, con
   `LAST_UID` = insn 63—, **no `srcBeg`**. La r24 tenía razón. Con el volcado
   `.cse` delante. (§1)
3. **La «contradicción sin resolver» de la r25 queda resuelta con el `.cse` y el
   `.combine` del caso mínimo de plantilla** (lo que pedía el brief y nadie había
   sacado): el objetivo usa `srcBeg` en el `subf` **y** `srcIt` en la comparación
   porque su RTL es **exactamente el de la variante `a1`**… que muere en
   `combine`. Las dos mitades son **mutuamente excluyentes**. (§2)
4. **Hallazgo nuevo y estructural: al original le sobran TRES sentencias en
   `assign` que nosotros no tenemos** — `resize()` en la rama `srcIt == 0`, y
   `Contains()` + `make_empty()` + `reserve()` bajo un `if (minSize >
   capacity())` en la otra. **`Contains` es un método de `Vector` que no existe
   en NINGUNA parte de nuestro árbol.** Lo destapó `dwbody.py` **con la caché
   regenerada**; con la caché rancia (como estaba al empezar la ronda) no sale
   nada. Medido: aplicarlo **cuesta +16 B**, así que **no se aplica**. (§3)
5. **`LoadCameraShakes`: el DWARF corrobora la r25 y cierra la puerta de la
   fidelidad.** El `warned_overflow` del original vive en **r0** y es de ámbito
   de función, y **no hay ninguna sentencia detrás del `if (group)`**. Además el
   hermano `LoadCameraSet` tiene en el ORIGINAL un `warned_overflow` **muerto (sin
   registro), igual que el nuestro**. (§4)
6. **Trampa de herramienta que decidió la ronda**: la caché de `dwbody.py` **se
   queda rancia después de CADA `build_direct.py`**, y el aviso se lee como ruido.
   (§6)

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera --muro` reproducen el brief clavado:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28Camer faltan 1, sobran 2, 1 SUST  cmpwi-1, mr.+1, lwz-1
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 3868 ICEMover::Update / 1192 TerrainVelocityNoise / 992 TrackCar / 948 TrackCop

`measure.py zCamera`: **113080/125008 B, 90,4582 %, 446 al 100 %**, al empezar y
al terminar. `audit.py`: **446/446 ok, 0 FALLA**, las dos pasadas.
`symtabdiff.py zCamera`: **sólo falta `_vt.Q33Sim9Collision9IListener`** (el
pendiente que dejó la r24); **0 funciones ausentes**.

## 1. `_Storage` (1.156 B): son DOS diferencias, no una

`fndiff` sobre los 1.160 B nuestros deja **16 filas** en **tres racimos**:

| racimo | objetivo | nuestro |
|---|---|---|
| **(a)** 14-22 | `add r24,r0,r9` · `mr. r27,r0` · `subf r9,r0,r24` | `mr r27,r9` · `add r24,r9,r0` · `subf r0,r27,r24` · `cmpwi r27,0` |
| **(b)** 68-73 y 130-134 | `lwz r4,8(r31)` reutilizado a través de la etiqueta del bucle; `mr r4,r9` al final | recarga `lwz r4,8(r31)` (el `lwz` de más) |
| (c) | — | (consecuencias de (a)/(b)) |

El presupuesto de `triage` (`mr.+1, cmpwi-1, lwz-1`) es exactamente **(a) + (b)**:
**hacen falta las DOS para ganar un solo byte**, y `matched_code` es todo o nada.

### 1.1 El mecanismo de (a), con el `.cse` delante — y la r25 se equivocaba

RTL de entrada (`.jump`) del constructor de copia, tal cual:

    62  reg100 = mem(reg83)          ; mBegin, de begin()
    63  reg101 = reg100              ; copia del valor de retorno
    72  reg103 = mem(reg83+8)        ; mSize
    74  reg105 = reg103 << 2
    75  reg106 = mem(reg83)          ; mBegin otra vez, de end()
    76  reg107 = reg106 + reg105     ; srcEnd
    77  reg108 = reg107
    83  reg109 = reg101              ; parametro srcBeg
    84  reg110 = reg108              ; parametro srcEnd
    87  reg111 = reg109              ; srcIt = srcBeg
    88  reg112 = reg110 - reg109     ; minSize = srcEnd - srcBeg
    93  cc116  = compare(reg111, 0)

y **tras `cse`** (volcado real, no deducción):

    76  reg107 = reg100 + reg105
    84  reg110 = reg107
    87  reg111 = reg100              <- reg109 canonizado a reg100
    88  reg112 = reg110 - reg111     <- reg109 canonizado a reg111  (LA COPIA)
    93  compare(reg111, 0)

O sea, `make_regs_eqv` **no promociona** `reg101` (insn 63) ni `reg109` (insn 83),
y **sí promociona `reg111`** (insn 87). Con `cse_basic_block_end` = 669 y los
`LAST_UID` que calcula `reg_scan` **antes** de CSE:

| pseudo | `LAST_UID` | ¿> 669? | promocionado |
|---|---|---|---|
| `reg101` | 83 | no | **no** |
| `reg109` (`srcBeg`) | 88 | no | **no** |
| `reg111` (`srcIt`) | ~965 | sí | **sí** |

**`firstr` en la insn 87 es `reg100`, cuyo `LAST_UID` es 63.** La r25 dijo que el
punto de parada era `srcBeg`; **no lo es**: `srcBeg` (reg109) ni siquiera llega a
ser canónico. La r24 lo tenía bien.

**Consecuencia directa**: alargar la vida de `srcBeg` **no toca ninguna de las dos
condiciones** de la insn 87. Lo que haría falta es alargar la de **`reg100`**, que
es el temporal que devuelve `begin()` y muere en la insn siguiente — **no hay
sentencia de fuente que lo alcance**.

### 1.2 La palanca del brief, medida

`p1` = base + `if (srcBeg == 0) { minSize = 0; }` **detrás del último bucle**
(sobre el árbol, con `build_direct.py` delante y restaurando siempre):

    p1   97,81661 %   1160/1156 B   17 diffs   (base: 97,83391 %, 1160 B, 16 diffs)

y el racimo (a) sale **fila por fila idéntico a la base**: sigue `mr r27,r9` +
`cmpwi r27,0`, sigue `subf r0,r27,r24`. **La rama no cierra nada.**

Dato de método que hay que anotar: **en el CASO MÍNIMO la misma sonda SÍ mueve la
canonización** (`.cse` da `reg111 = reg109` y `reg112 = reg110 - reg109`), pero
**en el árbol no**. El caso mínimo **no es fiel** en esa zona (§6).

## 2. La contradicción de la r25, resuelta: (a) es un muro con demostración

La r25 dejó abierto: «el objetivo usa `srcBeg` en el `subf` **y** `srcIt` en la
comparación — o CSE no los fundió, o el `mr.` sale por otro camino». Con los
volcados de la variante **`a1`** (`minSize` declarado antes que `srcIt`) queda
cerrado.

**`a1` produce en `.cse` EXACTAMENTE el RTL del objetivo:**

    76  reg107 = reg100 + reg105     ; srcEnd
    84  reg110 = reg107
    87  reg111 = reg110 - reg100     ; subf: usa el ORIGEN  <- lo que quiere el objetivo
    89  reg114 = reg111 >> 2
    91  reg115 = reg100              ; srcIt = srcBeg
    93  compare(reg115, 0)           ; PRIMER uso de reg115 -> hay LOG_LINK

`.flow` confirma el `LOG_LINKS: (insn_list 91)` en la insn 93, y `.combine`
**fabrica el `mr.`**:

    93 (parallel[ (set (reg:CC 116) (compare:CC (reg:SI 100) 0))
                  (set (reg/v:SI 115) (reg:SI 100)) ])

…y **en la misma pasada mata el `subf`**: `LOG_LINKS(87) = (insn_list 76)`, así
que `combine` mete el `add` dentro de la resta, `simplify_plus_minus` pliega
`(reg100 + reg105) - reg100` → `reg105`, y la insn 89 queda `reg114 = reg105 >> 2`.

    a1 sobre el ARBOL:  96,60554 %   1152/1156 B   24 diffs   (-8 B = subf + srawi)

**Las dos mitades son mutuamente excluyentes, y el motivo es de `flow`, no de
fuente:**

- si el `subf` va **detrás** del copiado (nuestra forma), CSE le canoniza el
  operando a `srcIt`, el `subf` es el **primer** uso de `srcIt`, la comparación se
  queda **sin `LOG_LINK`** y no hay `mr.`; el `subf` sobrevive porque
  `reg107 - reg111` **no** es plegable;
- si el `subf` va **delante** (a1), su operando es `reg100`, la comparación **sí**
  es el primer uso de `srcIt` y sale el `mr.`; pero entonces
  `reg107 - reg100` **sí** es plegable y `combine` se lo lleva.

Para tener las dos haría falta que el segundo operando del `subf` fuese un pseudo
**distinto de `reg100`** y **canónico** en la insn 87 — o sea una copia de
`srcBeg` nacida antes de `srcIt` y con `LAST_UID` **posterior al de `srcIt`**. Esa
copia viviría a través de las llamadas de los bucles y necesitaría un registro
**salvado**; el objetivo la tiene en **r0**, que es volátil. **Incompatible.**

**Ocho formas de la declaración barridas, todas sobre el ÁRBOL** (17 s cada una):

    a0  base (srcIt, luego minSize)                       97,83391 %  1160 B  16 diffs
    a1  minSize antes que srcIt                           96,60554 %  1152 B  24 diffs
    a2  a1 + `(size_type)(srcEnd - srcBeg)`               96,60554 %  1152 B  24 diffs
    a3  a1 + `const_iterator srcIt;` y asignacion aparte  96,60554 %  1152 B  24 diffs
    a4  a1 + `if (!srcIt)`                                96,60554 %  1152 B  24 diffs
    a5  `minSize = srcEnd - srcIt`  (control)             97,83391 %  1160 B  16 diffs (= base)
    a6  a1 + `const_iterator srcEndIt = srcEnd;` delante  96,60554 %  1152 B  24 diffs
    a7  a1 + `difference_type diff` en dos pasos          96,60554 %  1152 B  24 diffs

**Veda (con el volcado):** el racimo (a) de `_Storage` **no lo abre ninguna forma
del orden de declaración**. `firstr` es `reg100`, el temporal de `begin()`, que
muere en la insn 63; ninguna sentencia de `assign` puede alargarlo. Y cualquier
orden que dé el `mr.` pone el `subf` en la ventana donde `combine` lo pliega
(demostrado con `.cse`, `.flow` y `.combine`, no por razonamiento).

### 2.1 El racimo (b): seis formas del cuerpo, todas peores

El `lwz` de más es el `size()` de la guarda del bucle de `push_back()`: el
objetivo lo mantiene en un **pseudo que vive a través de la etiqueta del bucle**
(por eso tiene `mr r4,r9` al final y nosotros no), y nosotros lo recargamos.

    b0  base: reserve + if + do-while                     97,83391 %  1160 B  16 diffs
    b1  `while (minSize > size())`                        94,9654  %  1160 B  38 diffs
    b2  `resize(minSize)`                                 94,68858 %  1184 B  41 diffs
    b3  `while (size() < minSize)`                        94,9135  %  1160 B  40 diffs
    b4  `if (...) { while (...) }`                        95,04498 %  1172 B  40 diffs
    b5  `for (; minSize > size();)`                       94,9654  %  1160 B  38 diffs

**La forma actual es la mejor de las seis.**

## 3. HALLAZGO NUEVO: al original le sobran tres sentencias en `assign`

`dwbody.py zCamera "_Storage<CameraAI::Director *,2>::_Storage" both "_ctor_arg"`
**con la caché regenerada** da el árbol de inlines del original, y **no coincide
con el nuestro**:

| | ORIGINAL | NUESTRO |
|---|---|---|
| rama `srcIt == 0` | `resize(num)` → { `pop_back` (rango degenerado), `reserve`, `push_back()` } | `reserve` + `push_back()` **sin `resize`** |
| rama `srcIt != 0` | `capacity()` suelto · **`Contains(Director* const*)`** · **`make_empty()`** · `reserve` | sólo `reserve` |

`Contains` **no existe en ningún sitio del árbol** (`grep -rn Contains
src/.../Utility/` → nada). Su DWARF: `inline bool Contains(struct Director *
const * p)` con una local `int index` y **una sola llamada inline a
`capacity()`** (ni `size()` ni `indexof`). Con el objeto recién construido
(`mBegin = mCapacity = mSize = 0`) todo eso **se pliega a cero instrucciones**, y
el `ble .L_80081AF8` de 0x80081A4C salta por encima de las tres.

El mapa de líneas encaja: `assign` va de ~299 a 345 en el original, con
`minSize` en **301**, `if (srcIt == 0)` en **306**, la llamada a `resize` en
**307**, el `return` en **310**, y las sentencias **314** (`capacity()`),
**316** (`Contains`, que baja a 596→124→598), **317** (`make_empty`, que baja a
270→118→272→194→118→197→274) y **318** (`reserve`, 203→206).

**Medido** (con `Contains` escrito como `int index = p - mBegin; return index <
capacity();`, que es la forma que **sí** se pliega: la comparación se hace
`unsigned` y `simplify_relational_operation` mata `LTU(x, 0)`):

    d4  base en la rama 0 + `if (minSize>capacity()){ if(!Contains(srcBeg)) make_empty(); reserve(minSize); }`
                                                      96,411766 %  1176/1156 B  22 diffs
    d1  d4 + `resize(minSize)` en la rama 0            83,62976  %  1312 B  73 diffs
    d3  `resize(minSize)` + `reserve` pelado           94,740486 %  1184 B  39 diffs
    d5  d1 con `clear()` en vez de `make_empty()`      83,62976  %  1312 B  73 diffs
    d6  d1 sin el `if (minSize > capacity())` exterior 84,32526  %  1304 B  71 diffs
    (con la primera version de `Contains`, `index >= 0 && index < (int)capacity()`,
     que NO se pliega: d1 = 81,903 %, 1332 B, 78 diffs)

**`d4` cuesta +16 B (4 instrucciones) y NO se aplica.** El coste está localizado:
nuestro `if (minSize > capacity())` exterior **sí** se pliega a `cmplwi r26,0`
(igual que el objetivo), pero entonces el `if (num > capacity())` **de dentro de
`reserve`** deja de plegarse y emite `lwz r0,4(r31)` + `cmplw` + `ble` + un `lwz`
del vtable, donde el objetivo reutiliza el `r11` del constructor. En el objetivo
CSE reconoce que las dos comparaciones son la misma; en el nuestro no (la llamada
virtual `FreeVectorSpace` de `make_empty`, aunque su rama se pliegue después,
invalida la tabla de memoria de CSE cuando ésta pasa por encima).

**Esto es lo primero que retomaría la ronda siguiente**: la estructura del
original está ahora *escrita*, y sólo falta que el `reserve` interior vuelva a
plegar. Con `d4` el racimo (a) sigue igual, así que por sí solo no da bytes; pero
es la única diferencia de FUENTE encontrada en esta función en cuatro rondas.

## 4. `LoadCameraShakes` (168 B): el DWARF confirma la r25 y cierra la fidelidad

`dwbody.py zCamera "ICEManager::LoadCameraShakes" both`, caché fresca:

    ORIGINAL                                  NUESTRO
    unsigned int id;                          unsigned int id;
    bool warned_overflow;   // r0             bool warned_overflow;   (SIN registro)
    struct bChunk * chunk;                    struct bChunk * chunk;
    { p_handle; group r31 }                   { p_handle; group r31 }
      { num_tracks r28; track r30 }             { num_tracks r27; track r30 }
        { i r27 }                                 { i r28 }

Tres cosas, todas nuevas:

1. **`warned_overflow` del original ocupa `r0`** — un registro **volátil**. Su
   rango de vida **no cruza ninguna llamada**: sólo va del `li r0,0` (índice 12)
   al `cmpw r0,r28` de la guarda de entrada (índice 15). Eso confirma al dígito el
   «requisito exacto» de la r25 y **descarta** cualquier forma cuya referencia
   sobreviva al bucle (la de la r25 se llevaba **r26**, salvado, justamente por
   eso).
2. **Los bloques anónimos y el árbol de inlines son idénticos**, y `num_tracks`/`i`
   están **intercambiados** (r28/r27 contra r27/r28): la diferencia es de reparto,
   no de estructura.
3. **No hay ninguna sentencia detrás del `if (group)`**: el bloque exterior
   termina en 0x8007F040 y la función en 0x8007F050, o sea **16 B = las cuatro
   instrucciones del epílogo**. La forma ganadora de la r25 (`if (warned_overflow)
   { <store> }` detrás del bucle) **no está en el original**, y ahora está probado
   con el DWARF además de con el mapa de líneas.

**Control que vale tanto como lo anterior**: el hermano `LoadCameraSet` (344 B,
**al 100 %**) tiene en el ORIGINAL un `warned_overflow` **sin registro**, o sea
**muerto, exactamente como el nuestro**. Las dos funciones difieren de verdad: en
`LoadCameraShakes` el original mantiene la variable viva y en `LoadCameraSet` no.
(De paso: el `LoadCameraSet` original tiene además `unsigned int id; // r9`, que
nosotros no declaramos, y su parámetro se llama `set_chunk`; es fidelidad
byte-neutra y **no lo he tocado**, la función ya casa.)

**No he gastado ensayos** en esta función: las 40 compilaciones de la r25 más las
22 formas de las r21/r22/r23 cubren el espacio, y el DWARF acaba de decir que la
única forma que subía a 99,476 % no es la del original.

## 5. Static-init (3.604 B): la veda de la r22, reconfirmada con volcado fresco

`dwbody.py zCamera "__static_initialization_and_destruction_0" both "0x800809A8"`
(la de zCamera es 0x8008095C..0x80081770; el patrón casa **46** cuerpos en el DOL,
hay que discriminar por dirección):

    lineas ORIGINAL = 303     lineas NUESTRO = 303     diferencias: 16

y las 16 son **una sola cosa repetida seis veces**: el original llama a la clase
**`ICEReplayCategory`** y nosotros **`ReplayCategory`**, con los parámetros del
constructor nombrados `category_name, scene_name, score_function, mirror_function`
en vez de `name, replay, score, mirror`.

**Es byte-neutro**: el constructor está definido dentro de la clase (no emite
símbolo) y `symtabdiff.py zCamera` da **0 funciones ausentes**, así que el nombre
no llega a la tabla de símbolos. **No lo he aplicado** (renombrar la clase toca
`ICEReplay.cpp` e `ICEManager.cpp` y no gana un byte), pero queda anotado como
corrección de fidelidad con evidencia.

Lo demás de la static-init sigue como lo dejó la r25: gradiente descendente
monótono y descartado (−49 insns), y la única salida sigue siendo **+26..31 insns
RTL muertas hacia arriba**. **No he gastado ensayos ahí.**

## 6. Trampa de herramienta (nueva, y es la que decidió la ronda)

**La caché de `dwbody.py` se queda rancia después de CADA `build_direct.py`.**

Al empezar la ronda el volcado era de las 17:44 y el `.o` de las 23:15;
`dwbody.py` lo dice, pero en una línea de aviso que se lee como ruido y **sigue
imprimiendo un resultado**. Con la caché rancia, `dwbody` sobre `_Storage`
contestó literalmente **`no encontrada en el ORIGINAL`** — que es justo lo que
haría pensar «aquí no hay nada». Regenerándola con

    python scripts/regmap.py zCamera "_Storage<CameraAI::Director *,2>::_Storage" --ours

salieron las tres sentencias de la §3. **Y vuelve a quedarse rancia en cuanto
recompilas la unidad**, así que en un barrido hay que regenerarla otra vez antes
de volver a leer el DWARF.

Dos cosas más de método:

- **`dwbody.py` necesita un discriminante en casi todo lo interesante**: 46
  cuerpos casan `__static_initialization_and_destruction_0` y 2-3 casan
  `_Storage<...>::_Storage`. El cuarto argumento acepta una subcadena; con la
  dirección (`0x800809A8`) va bien, con el nombre de un parámetro (`_ctor_arg`)
  también.
- **El caso mínimo de plantilla de la r24 NO es fiel fuera del prefijo.** En
  `c26cam_stor.cpp` `push_back(const_reference)` se **expande en línea** y en
  zCamera es una **llamada** (`bl push_back__...`): 305 instrucciones contra 290.
  Casa fila por fila hasta el índice ~50 (o sea el racimo (a) y nada más). La
  sonda `p1` es el contraejemplo medido: en el caso mínimo mueve la canonización
  y **en el árbol no**. Conclusiones de la r24 sacadas de ahí sobre los bucles
  habría que rehacerlas; las del prefijo (a1) **sí** reproducen en el árbol.

## 7. Ensayos numerados, con su cifra

Todos **sobre el árbol**, con `build_direct.py zCamera` delante, evaluados con
`objdiff` sobre el símbolo, **en primer plano** y **restaurando `UTLVector.h` en
cada punto** (`finally` + copia de seguridad; md5 final idéntico al de partida).
~17 s por variante.

    base                                                97,83391 %  1160/1156 B  16 diffs

    a0..a7   orden de las declaraciones de assign       ver §2 (8 variantes)
    b0..b5   forma del cuerpo de la rama srcIt==0       ver §2.1 (6 variantes)
    d1..d6   estructura del DWARF (resize/Contains/
             make_empty), con las dos formas de
             `Contains`                                 ver §3 (7 medidas)
    p1       rama sobre srcBeg detras del ultimo bucle  97,81661 %  1160 B  17 diffs

Caso mínimo (1 s, `-Ipre` a una copia de `UTLVector.h` en el scratchpad; **el
árbol no se toca**): volcados `.rtl .jump .cse .flow .combine` de la base y de
`a1`, más la sonda `p1`. Son los de la §1.1 y la §2.

**Nada aplicado. Nada revertido a medias. Cero cambios en el árbol.**

## 8. Vedas nuevas (con la sentencia barrida)

1. **`_Storage` racimo (a): `firstr` es `reg100` y muere en la insn 63.** Alargar
   la vida de `srcBeg` **no toca la condición de `make_regs_eqv`** en la insn 87.
   Medido con `p1` sobre el árbol (17 diffs contra 16, racimo idéntico) y con el
   volcado `.cse`. **La palanca que proponía el brief de la r26 está descartada.**
2. **`_Storage` racimo (a): el `mr.` y el `subf` son incompatibles.** `a1` da el
   RTL del objetivo en `.cse` y `combine` mata el `subf` en la misma pasada
   (`LOG_LINKS(87) = 76`, `simplify_plus_minus`). 8 formas del orden de
   declaración barridas; las 6 que ponen `minSize` delante dan **las seis lo
   mismo** (1152 B, 24 diffs).
3. **`_Storage` racimo (b): la forma actual (`reserve` + `if` + `do-while`) es la
   mejor de seis.** `while`, `resize`, `size() < minSize`, `if`+`while` y `for`
   son **todas** peores (38-41 diffs contra 16).
4. **`LoadCameraShakes`: no hay sentencia detrás del `if (group)` en el
   original** (bloque exterior 0x8007EFBC-0x8007F040, función hasta 0x8007F050 =
   epílogo), y su `warned_overflow` vive en **r0**, o sea sin cruzar llamadas.
   La forma de la r25 que subía a 99,476 % **queda descartada por el DWARF**, no
   sólo por el mapa de líneas.
5. **Static-init: el árbol de inlines es idéntico con volcado FRESCO** (303/303).
   La única diferencia es el nombre de la clase `ICEReplayCategory`, que no llega
   a la tabla de símbolos. La veda de la r22 se sostiene.

## 9. Qué NO he probado

- **La `d4` con el `reserve` interior plegado.** Es lo único que queda vivo de
  `_Storage`: la estructura del original ya está escrita (§3) y cuesta 4
  instrucciones porque nuestro CSE no reconoce que `if (minSize > capacity())` y
  el `if (num > capacity())` de `reserve` son la misma comparación. **No he
  sacado el `.cse` de `d4`** para ver quién invalida la tabla de memoria (mi
  sospecha, sin medir, es la llamada virtual `FreeVectorSpace` de `make_empty`).
  Aun cerrándolo **no daría bytes por sí solo**: el racimo (a) es muro.
- **La forma exacta del `resize` del original.** El DWARF dice que contiene
  `pop_back` (rango degenerado, listado el primero), `reserve` y `push_back()`.
  Probé tres cuerpos (`r0` el nuestro, `r1` con la guarda `num > size()`, `r2`
  con la rama `pop_back` delante) y **ninguno baja de 1184 B**. No he barrido
  más.
- **`ICEMover::Update` (3.868 B)**: no lo he tocado. Con el modelo de
  `make_regs_eqv` de la §1.1 se podría decidir si su `mr r11,r8` es alcanzable —
  **no lo he hecho**, y la r25 ya midió que el origen no tiene otro uso en la
  fuente.
- **`TerrainVelocityNoise` (1.192 B)**: le he sacado el diff y **no es lo que dice
  `triage`**. Las 14 filas son (i) la **colocación de los `lis sym@ha`** de cuatro
  literales del pool consecutivos (`lbl_803D1D88/8C/90/94`, que se corresponden
  **uno a uno y en el mismo orden** con nuestros `$LC454/455/456/457`) y (ii) una
  permutación **f0 ↔ f12** en la cadena de `fmadds`. Los conteos coinciden
  (1 INSERT, 1 DELETE): es *dónde* se emite cada `lis`, que según HERRAMIENTAS §8
  lo deciden `gcse`/PRE y `loop`, y se lee con `cc1plus -dG`. **No he sacado ese
  volcado.**
- **`TrackCop` (948 B) y `TrackCar` (992 B)**: sólo he releído los diffs. Las
  vedas de la r22/r24 siguen en pie.
- **La vtable `_vt.Q33Sim9Collision9IListener`**: sigue siendo el único símbolo
  que falta (`symtabdiff`). No lo he intentado; es `.rodata`.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**. El
  brief lo condicionaba a cerrar algo que ganara bytes, y no he ganado ninguno.
- **La corrección de fidelidad de `LoadCameraSet`** (mover `warned_overflow =
  false;` dentro del `if (group)`), la de `ICEReplayCategory` y la del `id` de
  `LoadCameraSet`: confirmadas, **no aplicadas** (funciones al 100 %, ganancia
  cero).
- **`permuter.py`**: sigue sin usarse en zCamera.

## 10. Verificación final

    build_direct.py zCamera                       ok
    triage.py zCamera --muro                      identico al brief (3 near-miss, 4 muros)
    measure.py zCamera                            113080/125008 B  90,4582 %  446 al 100 %
    audit.py Speed/Indep/SourceLists/zCamera      446/446 ok, CERO FALLA (pasada 1)
      -> segunda pasada                           446/446 ok, CERO FALLA
    md5 UTLVector.h  ==  copia de seguridad       identico
    git status src/Speed/Indep/Libs/, src/.../Camera/   vacio
    symtabdiff.py zCamera                         solo falta _vt.Q33Sim9Collision9IListener

`frozen.py` no lo he tocado: no he cambiado nada.

## 11. Herramientas (scratchpad, prefijo `c26cam_`)

    c26cam_min.py        el c24cam_min2.py renombrado: compila un .cpp con los
                         cflags EXACTOS de zCamera; con --rtl deja los 20
                         volcados por pase. OJO: hace chdir a la raiz, hay que
                         pasarle RUTA ABSOLUTA del .cpp.
    c26cam_stor.cpp      caso minimo de _Storage<Director*,2>. Le he anadido
                         `#include <new>`: sin el, el `placement new` da error y
                         el arnes de la r24 lo tapaba con un `FALLA` enganoso.
    c26inc/              copia de UTLVector.h que tapa a la del arbol (-Ipre).
    c26cam_showrtl.py    imprime un rango de insns de un volcado RTL, ya
                         recortado a la funcion del constructor de copia.
    c26cam_sweep.py      barrido sobre el ARBOL: dos regiones de assign
                         (declaracion x cuerpo). Restaura SIEMPRE.
    c26cam_sweep2.py     barrido sobre el ARBOL: sustituye el cuerpo ENTERO de
                         assign, cambia `resize` e inserta `Contains`. Es el que
                         da la §3. Restaura SIEMPRE.
    c26cam_stscore.py    puntua un .s contra la secuencia del objetivo sacada
                         del lmap (util, pero ver el aviso de fidelidad de la §6).
    c26cam_stor_lmap.txt / c26cam_stor_diff.txt / c26cam_dwbody.txt /
    c26cam_dwshake.txt / c26cam_dwsi.txt / c26cam_d4.txt      los volcados leidos.

Disco: arranqué con 15 GB libres y termino con 13 GB; los 2 GB los han gastado
otros agentes (el scratchpad es compartido). **Mis ficheros ocupan 337 kB**: he
borrado los volcados RTL y los `.s` al terminar.
