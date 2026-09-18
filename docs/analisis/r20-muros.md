# Ronda 20 — los tres muros grandes (zEAXSound2, zEagl4Anim, zBWare)

Línea base `base_r20_muros.json`: 293.760/319.044 B = **92,0751 %**, 1.470 fns.
Cierre `despues_r20_muros.json`: **+0 B, +0 funciones, 0 unidades cambian**.
`pctsnap --cmp` sobre zBWare: **1 función mejora (+0,050 pp), ninguna empeora**.

`audit.py` (una pasada, tras `build_direct.py`): zBWare **238 ok / 0 fallos**,
zEAXSound2 **921 ok / 0**, zEagl4Anim **311 ok / 0**. Las tres congeladas con
`frozen.py cong`.

Lo que sí sale de esta ronda:

- **`_bOutput`: uno de los cinco racimos cerrado** (99,59305 → 99,64324 %), con
  el mecanismo entero explicado y **medido con `lreg.py`**: un empate exacto de
  `allocno_compare` a 40000.
- **La herramienta que lo abrió es `lmap.py`**, y con una regla nueva:
  **GCC atribuye el incremento de un `for` a la línea de la LLAVE DE CIERRE**,
  no a la del `for`. Sin eso el mapa de líneas se lee al revés.
- Once vedas nuevas con cifra (nueve en ResolveCarBanks, dos en el switch).

---

## 1. `_bOutput` (zBWare, 5.180 B) — 99,59305 % → **99,64324 %**

**Es la ÚNICA función que le falta a zBWare** (238 de 239, 30.592 de 35.772 B).
Cerrarla pone la unidad al 100 %.

1.296 contra 1.296 instrucciones, delta de opcodes vacío. Antes: **cinco**
racimos de registro. Ahora: **cuatro**.

### El racimo de `q`: empate exacto de `allocno_compare`, roto por la fuente

`lreg.py Speed/Indep/SourceLists/zBWare _bOutput` da la tabla del compilador.
Los tres pseudos del bucle de redondeo (identificados en el volcado `.lreg`,
insns 4132/4142/4144) eran:

    pseudo  n_refs  live_len  prioridad  reg   qué es
    1227    20      20        40000      r9    char *q          (BASE_REGS, user var)
    1229    12       9        40000      r11   (char)*q         (GENERAL_REGS)
    1228     9      12        22500      r10   el lbz de *q

**Empate exacto a 40000.** `allocno_compare` de `global.c` desempata por número
de allocno, y los allocnos se numeran en orden de pseudo: gana el MENOR, o sea
`q`, que se lleva r9. El objetivo hace lo contrario (`q` en r11, el `extsb` en
r9), y el DWARF lo confirma: `char * q; // r11`.

Como el pseudo de `q` nace en su declaración, **siempre** será menor que el del
`*q`: por orden de pseudo el empate no se puede girar. Hay que romperlo por
prioridad, y `prioridad = floor_log2(n_refs) * n_refs / live_length * 10000`.

`n_refs` está **ponderado por la profundidad de bucle**: 6 refs a profundidad 3
más la definición a profundidad 2 = 20.

### La palanca: `lmap.py` dice que dos sentencias viven FUERA del bucle

    8005F324  subi r11, r5, 0x1     bPrintf.cpp:942  bPrintf.cpp:943   q = p - 1
    8005F328  cmpwi cr7, r30, 0x0   bPrintf.cpp:946                    (group_flag izado)
    8005F330  subi r11, r11, 0x1    bPrintf.cpp:960                    q--
    8005F334  lbz  r10, 0x0(r11)    bPrintf.cpp:949                    *q == '9'
    8005F374  addi r0, r10, 0x1     bPrintf.cpp:959  bPrintf.cpp:963   *q = *q + 1
    8005F378  cmplw r11, r27        bPrintf.cpp:966                    q < stringOut
    8005F384  subi r27, r27, 0x1    bPrintf.cpp:967                    stringOut--

Las líneas 963 y 966 son **posteriores** a la 960 (el `q--` del final del
cuerpo): en el original `*q = *q + 1;` y `if (q < stringOut) stringOut--;`
están **detrás del bucle**, alcanzadas por el `break`. Nosotros las teníamos
dentro del `if` más interno.

Sacarlas del bucle baja `n_refs` de `q` de 20 a 18 (dos refs pasan de peso 3 a
peso 2): 4·18/20 = 3,6 → **36000 < 40000**, gana el `extsb`, y el reparto
r11/r9/r0 sale igual que el objetivo.

    c1  `for (;;) { ...; break; ...; q--; }` + las dos sentencias detrás del bucle
        99,59305 % -> **99,64324 %**.  APLICADO.
        El racimo de las filas 797-819 desaparece entero (11 ARG_MISMATCH).

**Regla nueva y reutilizable**: en el mapa de líneas, el incremento de un `for`
lleva la línea de la **llave de cierre** del cuerpo, no la del `for`. Está
comprobado dos veces: aquí (960 entre el cuerpo 949-959 y el post-bucle 963) y
en `ResolveCarBanks` (511/512 = las llaves de cierre de los dos `for` de
BeginRule3, con delta de línea constante 256 respecto a nuestra fuente).

### Ensayos descartados (todos medidos)

    b1  `if (stringOut > q)`                       99,58533   peor
    b2  las dos sentencias en orden inverso        99,32664   peor
    b3  `*q += 1;`                                 idéntico
    b4  `++*q;`                                    idéntico
    b5  `q--` al final SIN sacar las sentencias    idéntico   <- la mitad no vale
    b6  `&&` en vez de `if` anidado                idéntico
    b7  `group_flag == 0`                          idéntico
    b8  `decimalChr != *q`                         99,58919   peor
    b9  `'9' == *q`                                idéntico

**Veda**: barridas nueve formas del bucle `for (;; q--)` de `bPrintf.cpp:577`.
Sólo mueve la aguja **sacar las sentencias del bucle** (c1); la forma del `if`,
el orden de los operandos y la grafía del incremento son invisibles.

### Los cuatro racimos que quedan, y qué se ha probado

| filas | qué | estado |
|---|---|---|
| 550-560 | `subf r9/r10` + el `'0'` en r11/r9 (`size`/`stringLength`, orig. 749-756) | c2/c3: fundir los dos `if` de `FL_FORCEOCTAL` en uno con `&&` → **idéntico** |
| 592-615 | `lis 0x7ff0` y la cascada `_nan_table` (r8↔r11) | c4 `stringLength=7` arriba → 99,37683 (peor) · c5 puntero temporal → idéntico · c6 `else{if}` → idéntico |
| 620-679 | rotación r8/r9/r11/r0 en los `lfd` del pool de `%f` | sin ensayos nuevos |
| 914/916 | dos `addi` **permutados, mismos registros** (planificador) | sin ensayos; el `addi r23,r1,0x10` es rematerialización de reload, no orden de fuente |

Sobre el `__asm__("" : : "r"(&number))` ya commiteado, remedido sobre la base c1:

    d1  quitarlo                          99,48417   peor
    d2  antes de `stringOut = nullptr;`   99,53513   peor
    d3  con el operando duplicado         idéntico

**El asm sigue siendo necesario** (confirma la medida de la ronda 19).

### Pista sin explotar

El mapa de líneas del original tiene **un hueco de 25 líneas** entre
`stringOut = nullptr;` (809) y `offset = ((unsigned*)&number)[1];` (834), y los
dos bloques de `%f` que preceden a los racimos B/C tienen **9 y 5 líneas de más**
que los nuestros. Son sentencias que no emiten código alcanzable pero **sí**
pesan en el RTL antes de plegarse. Nadie ha intentado reconstruirlas.

---

## 2. `ResolveCarBanks` (zEAXSound2, 7.508 B, 98,44433 %) — la `b` que sobra,
   localizada al byte

Con `mr20_rows` (diff por `diff_kind` real de objdiff, no por texto) el reparto
de la diferencia es:

- **una** instrucción de más, y está exactamente aquí:

      objetivo   800D2760  bgt .L_800D2640      ; > 4 -> comparte «~r17; b BeginRule4»
                 800D2764  mr r3,r17 ; li ; bl  ; <= 4 -> cae en LoadRemainingEngines
                 (12 instrucciones en el tramo)

      nuestro    0x77f8    ble 0x7830           ; <= 4
                 0x77fc    mr r3,r17 ; li ; bl
                 0x7808    b BeginRule4
                 0x7830    mr r3,r17 ; li ; bl  ; y cae en LoadRemainingEngines
                 (13 instrucciones)

- y **dos racimos de registro nuevos, no descritos en la ronda 19**: filas
  1418-1443 (`srawi r5/r6` contra `srawi r4/r5` en el `erase` de UTLVector) y
  filas 1672-1696 (`r22` contra `r24`, `r20` contra `r22`).

### El mecanismo, ahora completo

Los dos `goto BeginRule4` (el del bucle interno, línea 554 del original, y el
final, línea 578) terminan los dos en `mr r3,r17; li r4,0; bl ~Instance; b BeginRule4`.
`jump.c` funde uno de los dos con el otro, y **la dirección la fija el algoritmo**:
el bucle de *cross-jumping* recorre las insns hacia delante, toma como `insn` el
**primer** salto a esa etiqueta y busca los demás en `jump_chain`; `do_cross_jump`
**borra las insns de delante de `insn`** y lo redirige. O sea: **muere la copia
del salto MÁS TEMPRANO**. El nuestro es el del bucle interno, y por eso nos queda
la copia final con su `b` intacto.

En el objetivo muere la copia del salto FINAL: queda `ble ENDLBL; b .L_800D2640;`,
que `jump.c` colapsa a `bgt .L_800D2640` — y ahí se ahorra la instrucción.
Para reproducirlo hay que conseguir que el salto del bucle interno **no** sea
candidato cuando el pase lo visita. Ninguna de las formas probadas lo consigue.

**La cola compartida de `LoadRemainingEngines`** (9 instrucciones, dos entradas
`mr r3,r20` / `mr r3,r31`) vive en el objetivo en `0x800D2098`, **entre el final
del bloque `if (CopsCanBeInGame && …)` y la cabecera de BeginRule3**, y en el
nuestro en `+0x780c`, detrás del bloque final de BeginRule4. Su `b` final lleva
la línea 582 (`LoadRemainingEngines:`) en el objetivo y su entrada la 511.

### Ensayos de esta ronda (todos medidos; base 98,44433 %)

    r0  control (misma fuente, reescrita por el arnés)   98,44433   idéntico
    r1  BeginRule3 con `while` y `m--`/`n--` al final    98,31860   PEOR
    r2  BeginRule4 con `while` y `m++` al final          98,29995   PEOR
    r3  r1 + r2                                          98,27331   PEOR
    r4  BeginRule4 escrito ANTES de BeginRule3           82,15717   catastrófico
    r5  `goto LoadRemainingEngines;` explícito al final   98,44433   idéntico
    r6  `LowerPriority` declarada antes que `HighPriority` 98,25679  PEOR

**Veda 1 — y corrige una hipótesis de la ronda 19**: `r1/r2/r3` nacieron de leer
el mapa de líneas como si el `m--` de la línea 511 estuviera escrito al final del
cuerpo. **No lo está**: con delta de línea constante 256 respecto a nuestra
fuente, 511 y 512 son las **llaves de cierre** de los dos `for` de BeginRule3.
El original usa `for (; n >= 0; --n)` exactamente como nosotros. Barridas las dos
formas de los cuatro bucles de BeginRule3/BeginRule4.

**Veda 2**: barrido el orden de las dos declaraciones `Attrib::Gen::engineaudio`
de BeginRule3 (`r6`) y el orden de los bloques de reglas (`r4`) — los dos puntos
que la ronda 19 dejó anotados como «sin probar». Los dos empeoran.

**Veda 3**: `goto LoadRemainingEngines;` explícito detrás del `if` final de
BeginRule4 (`r5`) es byte a byte idéntico. Con los 19 de la ronda 19 y los 6 de
ésta son **25 formas de fuente medidas** sobre esta función.

### Lo que NO he probado

- **Dar cuerpo real a los dos `if (DEBUG_PRINT_CAR_BANK_RESOLVE)`.** El mapa de
  líneas mide el hueco: al original le sobran **9 líneas** en el primero
  (entre la 375 y la 391) y **5** en el segundo (entre la 404 y la 417). Es
  código muerto (`DEBUG_… == 0`) pero su RTL existe antes de plegarse. No lo he
  tocado porque un `bPrintf` mete literales en el pool y eso sí cambia el objeto:
  hay que buscar un cuerpo que use los objetos sin añadir `.rodata`.
- El permutador, ni guiado ni ciego.
- Los dos racimos de registro nuevos (filas 1418-1443 y 1672-1696).

---

## 3. `DynamicLoader::Initialize` (zEagl4Anim, 2.352 B, 91,33673 %)

### El árbol del switch, resuelto de lectura pero **inalcanzable desde la lista de `case`**

Objetivo (`lmap`, 0x8008F5FC):

    cmpwi  r0, 8   ; beq  D          (case 8 -> default)
    cmplwi r0, 8   ; ble  BAJO       <-- nosotros: bgt ALTO
    ALTO: cmplw r21 bgt D / cmplw r22 bge D / cmpwi 9 beq C9 / b D
    BAJO: cmpwi 3 beq C3 / cmplwi 3 bgt D / cmplwi 1 ble D / (cae) C2

    nuestro: raíz, luego BAJO (con `cmpwi 2; beq C2; b D`), luego ALTO.  **+1 insn**

El `cmplwi 1; ble D` del objetivo **no** es el `case 2`: es un nodo de rango
`[0..1]` con hijo DERECHO y sin hijo izquierdo, que `emit_case_nodes` emite como
`LE node->high -> node->code_label` (la rama «range, sólo hijo derecho» de
`stmt.c`); su etiqueta es la del `default`. Y con `[0..1]` como ancestro, el
`case 2` pasa a tener **las dos cotas** (`node_has_low_bound` encuentra
`high == 1`), o sea `node_is_bounded` → `emit_jump(C2)`, que `jump.c` borra por
ser la etiqueta siguiente: **por eso el objetivo cae en el cuerpo del `case 2`
sin comparar nada**.

Eso explica la mitad. La otra mitad —el `ble` de la raíz, que exige emitir el
subárbol ALTO primero— **no sale de ninguna lista de `case`**.

### El oráculo, y las dos vedas que da

Escrito `mr20_sw.py` / `mr20_sw2.py` / `mr20_sw3.py` / `mr20_sw4.py`: compilan un
`.cpp` suelto con los cflags de zEagl4Anim y sacan el árbol de despacho.
**0,35 s por variante, 8 en paralelo** — 352 combinaciones en 28 s.

**Veda 4 — la posición del `case` en la fuente es IRRELEVANTE.** 11 rangos × 8
posiciones = 88 variantes: los 8 árboles de cada rango son **idénticos**.
(`pushcase` ordena la lista; sólo cuentan los valores.) Esto explica por qué los
`i1`/`j1`/`k2` de la ronda 19 daban los tres el mismo resultado.

**Veda 5 — ninguna lista de `case` produce el `ble` de la raíz.** 352
combinaciones (rangos sueltos y pares, con `break` y con `continue`) más 160
variando el cuerpo del `default` y de los cuatro `case` vacíos: **512 árboles, y
en ninguno aparece un `le` en la raíz; en ninguno aparece un `le` siquiera.**
Todos dan `gt`/`ge`. El `ble` tiene que venir de otro sitio (tipo del índice,
banderas, o una transformación de `jump.c` que no reproduce este snippet).

**El coste de balance, medido y verificado en 4 casos**: `balance_case_nodes`
parte por la mitad de un coste de **1 por nodo + 1 extra por rango**. Con nuestra
lista (7 nodos, coste 10) la raíz es `[8..8]`; añadir `case 0 ... 1` (coste 2) la
mueve a `[4..7]` — ése es «el nodo que se perdía» de la ronda 19: **no se pierde
ninguno, se mueve el pivote**. Añadir un `case` de valor único (coste 1) por
debajo del 8 deja la raíz en `[8..8]`.

### Y `faltan 14` no es el switch

Confirmado lo de la ronda 19 y ampliado: el bucle de secciones entero va con los
registros **desplazados uno** (r22↔r23, r12↔r14, …) porque el objetivo tiene
**más presión de registros**: guarda `i*0x28` en **CTR** (`mtctr r8` en la fila
173, `mfctr r10` en la 316) y **recarga** `0x98(r1)` donde nosotros conservamos
el valor en un callee-saved. Son ~150 filas `ARG_MISMATCH` colgando de una sola
causa: **al original le falta un registro y a nosotros no**. Mientras no aparezca
el valor vivo de más que lo provoca, el árbol del switch no cierra la función.

---

## 4. Herramientas dejadas en el scratchpad

Prefijo propio `mr20_`:

- `mr20_it.py <unidad> [substr]` — compila y saca el % de esos símbolos
  (zBWare 3,1 s · zEAXSound2 24,7 s).
- `mr20_rows.py <unidad> <símbolo> [ctx]` — **el diff por `diff_kind` de objdiff**,
  no por texto. Sin esto los destinos de salto (símbolo+offset a un lado,
  dirección al otro) ensucian el 100 % de las filas y no se ve nada.
- `mr20_try.py` / `mr20_qsweep.py` / `mr20_rcbsweep.py` — arnés de sustitución de
  bloque, compilación, medida y **restauración** del fichero.
- `mr20_sw*.py` — el oráculo de árboles de switch (0,35 s por variante).
