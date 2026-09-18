# Ronda 27 — mx: zEAXSound2 / zFe2 / zSpeech / zPlatform

**Cero funciones cerradas. Un cambio APLICADO** (`UnlockPalette`
92,67442 → **99,41860 %**, de **16 filas a 2**, medido `EMPEORAN: ninguna`),
**tres mecanismos nuevos con volcado del compilador detrás**, y **63 ensayos
numerados** con su cifra.

El resultado que más vale es el de la §1: **el orden de los operandos de un `|`
decide el árbol que construye `fold`, y ese árbol decide qué registro es el
acumulador**. Es una palanca de fuente barata (24 permutaciones = 2 min) que
nadie había barrido, y en la primera función a la que se aplica se lleva
**14 de las 16 filas**.

## 0. Verificación del encargo y estado de auditoría

`build_direct.py zEAXSound2 zFe2 zSpeech zPlatform` → 4 ok.
`triage.py … --muro` reproduce el encargo **exactamente** (4 en «falta o sobra»,
14 de muro, 7.740 B).

`audit.py` sobre las cuatro unidades, **pasada de entrada**:

| unidad | funciones `ok` | FALLA |
|---|---|---|
| zEAXSound2 | 922 | **0** |
| zFe2 | 1304 | **0** |
| zSpeech | 698 | **0** |
| zPlatform | 134 | **0** |

Como no hubo ni un FALLA, no hay nada que confirmar con segunda pasada. Tras el
cambio de la §1, `audit.py Speed/Indep/SourceLists/zPlatform` vuelve a dar
**134 ok / 0 FALLA**.

---

## 1. `zPlatform::UnlockPalette` — 92,674 → 99,4186 % (APLICADO), y el mecanismo

**Aplicado al árbol**, un solo fichero:
`src/Speed/GameCube/Src/Ecstasy/TextureInfoPlat.cpp`, dentro de
`Convert32To16` (líneas 175-181). **Sin commit.**

```c
     if (a > 0xEF) {
-        result = 0xFFFF8000 | ((g >> 3) << 5) | ((r >> 3) << 10) | (b >> 3);
+        result = 0xFFFF8000 | (b >> 3) | ((r >> 3) << 10) | ((g >> 3) << 5);
     } else {
-        result = ((r >> 4) << 8) | ((a >> 5) << 12) | ((g >> 4) << 4) | (b >> 4);
+        result = (a & 0xE0) << 7;
+        result |= (r >> 4) << 8;
+        result |= (g >> 4) << 4;
+        result |= b >> 4;
     }
```

Medida con base y medida seguidas, `build_direct.py` delante de las dos:
`measure.py --cmp`: **+0 B, +0 funciones** (no llega al 100 %, así que
`matched_code` no da nada — es progreso de diagnóstico, no de bytes).
`pctsnap.py --cmp`: **MEJORAN 1 (+6,744 pp), EMPEORAN ninguna.**
`Convert32To16` es `static inline` con **un solo llamante**, así que el cambio no
puede tocar otra función; el `--cmp` lo confirma.

### 1.1 La primera mitad: por qué `combine` fundía el término del alfa

Diagnóstico de la r26 confirmado y **resuelto**: el objetivo hace
`rlwinm r11, r0, 7, 17, 19` **desde `a` ya materializado**, y nosotros
`rlwinm r0, r10, 15, 17, 19` **desde `entry`** —`combine` funde
`((entry>>24)>>5)<<12` en un solo `rlwinm`, y eso **mantiene `entry` vivo** en la
rama del `else`, donde el objetivo ya lo ha matado (reusa su registro para `r`).

**La forma que lo bloquea es escribir el mismo valor con la máscara delante:**

    ((a >> 5) << 12)   ->   ((a & 0xE0) << 7)      92,67442 -> 95,34884 %  (16 -> 8 filas)

No es que `& 0xE0` sea incombinable en abstracto (`(entry>>24 & 0xE0)<<7`
también es un `rlwinm`); es que con esa forma **`combine` ya no encuentra el
patrón de dos desplazamientos encadenados** que dispara la fusión.

### 1.2 La segunda mitad, y es la generalizable: `fold` y el orden del `|`

Con la fusión bloqueada quedaban 8 filas, **todas de qué registro acumula la
cadena de `or`**. La causa no es el asignador: es **el árbol que construye
`fold`**, y el árbol lo decide **el orden de los operandos en el fuente**.

Barrido **exhaustivo de las 24 permutaciones** de `{0xFFFF8000, r, g, b}` en la
rama `if` (`c27mx_tex4.py`, 2 min de reloj):

| orden | % | filas |   | orden | % | filas |
|---|---|---|---|---|---|---|
| **Cbrg / bCrg** | **99,06977** | **4** | | Cgrb / gCrb | 96,27907 | 6 |
| Crgb / rCgb | 97,90698 | 7 | | Crbg / rCbg | 95,23256 | 11 |
| bgCr | 93,37209 | 6 | | rbCg | 92,32558 | 8 |
| gbCr | 91,97675 | 7 | | rgCb | 91,39535 | 8 |
| rgbC | 84,30232 | 10 | | grCb | 80,34884 | 24 |
| bgrC | 71,51163 | 25 | | (las 12 restantes, 73-93 %) | | |

La regla que sale del barrido, y que se cumple en las 24: para un fuente
`C | X | Y | Z`, **`fold` produce `((X | (C | Y)) | Z)`** — o sea, mete la
constante con el **tercer** operando y deja el resultado como operando
**derecho**. El objetivo tiene `((C|r)|g)|b`, con el nodo de la constante a la
**izquierda**: **ninguna permutación lo alcanza** (§1.4).

Luego la rama `else`: pasarla de una expresión a **una cadena de `|=`** cierra
sus filas por completo, porque hace que `result` sea el acumulador desde el
primer término (y por eso choca con `r` y coge r11 en vez de r9):

| forma del `else` | % | filas |
|---|---|---|
| expresión `a\|r\|g\|b` | 99,06977 | 4 |
| **cadena de `result \|= …`** | **99,41860** | **2** |

Y las 24 permutaciones del `else` (`c27mx_tex6.py`) confirman que `a,r,g,b` es el
orden bueno: ninguna otra baja de 6 filas.

### 1.3 Lo que queda: 2 filas, y están medidas al árbol

```
objetivo   or r0,  r0, r11      <- ((C|r) | g) en un TEMPORAL (r0)
           or r11, r0, r9       <- | b, y esto ya es `result` (r11)
nuestro    or r9,  r9, r0       <- (b | (C|r))
           or r11, r9, r11      <- | g
```

Todo lo demás —los cuatro `rlwinm`/`srwi`, el `oris`+`ori`, las seis
instrucciones del `else`, y `result` en **r11**— casa. Falta **sólo** que el
nodo que lleva la constante sea el operando **izquierdo** del `or` exterior.

### 1.4 Veda medida: no hay forma de expresión que ponga la constante a la izquierda

Barridas, todas con el `else` ya bueno: **24 permutaciones** del `|`,
`result = C|r|g; result |= b`, `result = C|r; result |= g; result |= b`,
`result = C; result |= r; …`, `(C|r)|g|b` con paréntesis explícitos en tres
agrupaciones, `((r>>3)<<10)` escrito como `((r & 0xF8) << 7)`, y `(g>>3)<<5`
como `(g & 0xF8) << 2`. **34 formas, ninguna baja de 2 filas.** `fold` canoniza
el nodo con constante a la derecha y el paréntesis no sobrevive.

**Lo que NO he probado aquí:** que `0xFFFF8000` deje de ser una constante para
`fold` (una variable, un `extern const`, una expresión-sentencia). Es la única
palanca que queda y **es exactamente el patrón de `eProgressiveScan`** de la
r23 («si el objetivo NO pliega una redundancia que a nosotros se nos pliega, el
original usaba una variable o un helper»), sólo que aquí al revés: el objetivo
**no reasocia** donde a nosotros `fold` sí nos reasocia.

### 1.5 Vedas de esta función (con su sentencia)

| ensayo | qué se barrió | cifra |
|---|---|---|
| `u1_tern`, `z1_tern` | `return a > 0xEF ? (…) : (…)` sin `result` | 84,65 / **96,74** |
| `z2_2ret` | dos `return`, sin `result` (repite la veda de la r26) | **96,74** con el orden bueno (84,30 con el viejo) |
| `u2/z5_shortres` | `unsigned short result;` | 84,65 / 96,74 |
| `u3_resfirst`, `z3_terninit`, `z7_retdirect` | `result` declarada antes / inicializada con ternario / `return (unsigned short)result` | **objeto idéntico** |
| `u6_aparen` | `a = (entry & 0xFF000000) >> 24` | 88,14 (**176 B**, +1 insn) |
| `u7/z6_presetlo` | `result = LO; if (a>0xEF) result = HI;` | 55,93 / 56,63 |
| `z4_swapbr` | invertir la condición a `if (a <= 0xEF)` | 60,81 |
| `t3_both_oreq` | cadena de `\|=` **también** en la rama `if` | 98,14 (11 filas) |
| `t6_lo_partial` | `else` partido en dos mitades de dos términos | 81,51 |
| `xa_rgbmask` | los tres términos con máscara (`(r&0xF8)<<7`, …) | 86,40 (**176 B**) |

**Corolario que corrige la lectura de la r26:** «quitar `result` → 84,30 %» era
cierto **sobre el fuente viejo**; con el orden de los `|` ya arreglado, quitarlo
sube a **96,74** — sigue siendo peor que dejarlo (99,42), pero **la penalización
se reduce de 8,4 pp a 2,7 pp**. Es la sexta confirmación de la regla del §2 del
brief leída al derecho: *el orden en que se arreglan las diferencias importa*.

---

## 2. `zPlatform::ActualReadJoystickData` (1.588 B) — dos hallazgos de DWARF que
   nadie había mirado, y el pase que borra las dos instrucciones

No cerrada (97,35516 %, −8 B). Pero el diagnóstico de la r26 («`mr r9,r0` +
`andi.`, y el objetivo reutiliza r11 como cero») está ahora **cerrado hasta el
pase de GCC que lo decide**, y hay **dos hechos del volcado DWARF del original
que cambian el planteamiento**.

### 2.1 El original declara `short data;`, y **no tiene `v`**

`dwbody.py zPlatform "ActualReadJoystickData" both`:

```
ORIGINAL                         NUESTRO
{ short data;  // r0 }           { int data; // r5
                                   int v;    // r0 }
```

O sea: **nuestra `v` (r0) es la `data` del original**, y nuestra `data` (r5) es
un temporal del compilador. Medido:

| ensayo | qué | insns | B | % |
|---|---|---|---|---|
| `ctrl` | árbol actual | 395 | 1580 | 97,35516 |
| `c1_short` | `int data` → `short data` | **391** | **1564** | 96,19647 |
| `c2`…`c5` | `short data` + quitar los `(short)` + reescribir la cola | 391 | 1564 | 96,07-96,20 |
| `c7_shortv` | `short data; short v;` | 391 | 1564 | 94,82368 |
| `c8_intdata_shortv` | `short v` sólo | 398 | 1592 | 96,47355 |

**`short data` COALESCE `data` y `v` y borra los tres `mr`** (`mr r8,r0`,
`mr r10,r0`, `mr r11,r0`) que el objetivo **sí** tiene. Con nuestro `int data;
int v;` esos tres `mr` salen bien. Es decir: **la forma del original tiene un
`short` que no coalesce**, y nosotros no sabemos escribirla; el objetivo son
**c1 + 6 insns** (los 3 `mr`, el `extsh`, el `mr r9,r0` y el `andi.`), y nosotros
estamos en c1 + 4.

### 2.2 El struct del original es `JoyData` / `PadData`, con `ThePadData[slot]`

El mismo volcado trae el interfaz completo (`symbols/mw_dwarfdump.nothpp:1851146`):

```c
struct JoyData {                     struct PadData {           // 0xA bytes
    struct PadData ThePadData[4];        unsigned short DigitalButtons; // 0x0
    unsigned char  Bytes[4];             unsigned char  LTrigger;  // 0x2
    int            RegularControllerType;unsigned char  RTrigger;  // 0x3
    struct PADStatus padSTATUS;          unsigned char  AnalogLeftX;  // 0x4
};                                       unsigned char  AnalogLeftY;  // 0x5
                                         unsigned char  AnalogRightX; // 0x6
                                         unsigned char  AnalogRightY; // 0x7
                                         unsigned char  Error;     // 0x8
                                         unsigned char  Type;      // 0x9
                                     };
```

Nuestro `JoystickSnapshot` es un equivalente plano con los mismos
desplazamientos, **y por eso `slot` existe y por eso el objetivo indexa**
(`sthx r9, r28, r30` en la línea 291 del original, que nosotros **ya casamos**).
No lo he cambiado —el bloque `pad_state == 0` sale idéntico— pero **queda
apuntado**: la rama del volante usa `add r31, r29, r27` donde nosotros hacemos
`add r31, r27, r29` (orden de operandos del `+`), y eso **sí** es el orden de la
expresión en el fuente: `joy_data + slot` contra `slot + joy_data`.

### 2.3 El pase que borra las dos instrucciones que faltan: `flow`

Volcados RTL por pase (`-drjsftc` sobre un `.ii` propio). El cuarto
`if (v & 0x8000) { v = 0; }` **existe hasta `cse2`**:

```
(insn 505 (set (reg:SI 294) (and:SI (reg/v:SI 126) (const_int 32768))))
(insn 506 (set (reg:CC 295) (compare:CC (reg:SI 294) (const_int 0))))
(jump_insn 507 … (eq (reg:CC 295) 0) -> label 513)
(insn 512 (set (reg/v:SI 126) (const_int 0)))     <- `v = 0`, VIVA en .cse y .cse2
```

**`flow` la borra** (el bloque 17 queda vacío), `combine` funde entonces el
`and` con el `compare` en `*extzvsi_internal1` y `jump2` se lleva el salto: −2
instrucciones. En el objetivo el `andi.` **sobrevive con destino vivo** (r11) y
sus cinco copias de cero salen de él en vez del `andi.` anterior.

Los cinco `mr rX, <cero>` **existen en los dos lados** (r11,r10,r8,r6,r7 en el
nuestro; r9,r10,r8,r6,r7 en el objetivo): lo único que cambia es **cuál de los
cuatro `and` es el «cero disponible» que `cse` sustituye**.

### 2.4 Vedas de esta función (todas con la sentencia barrida)

| ensayo | qué se barrió | cifra |
|---|---|---|
| `a1`,`a2`,`a4` | el clamp como ternario en el `store` (1, 2 y 4 bloques) | 88,89 / 85,87 / **86,67** (1.592 B) |
| `a3`,`a5` | ídem con una sola variable | fallo de compilación / — |
| `b1_yv` | `v = 0x80 - stickY*1.75f; joy_data->stickY = v;` | 97,19144 |
| `b2_castst` | `joy_data->stickX = (short)data;` + `v = (short)data;` | 97,16625 (**1.576 B**: el `stb` no necesita `extsh`) |
| `b4_yv_thendata` | `v = …; data = v; stickY = data;` | **objeto idéntico** |
| `b6_datastick2` | toda la cola con `data`, sin `v` | 97,32997 |
| `b7_vkeep` | mantener `v` viva sumándola (`… + v`) | 95,28967 (**1.596 B**) |
| `b8_noif` | quitar el cuarto `if` | 96,58690 (1.576 B) |
| `e1_datazero` | `if (v & 0x8000) { data = 0; }` (no toca `v`) | **objeto idéntico al control** |
| `e2_empty` | `if (v & 0x8000) { }` | 96,58690 |
| `e4_testdata_setv` | `if (data & 0x8000) { v = 0; }` | 97,32997 |
| `e6_slotzero` | `if (v & 0x8000) { slot = 0; }` | 96,58690 |

**La veda que más informa es `e1`**: cambiar el cuerpo del `if` para que **no
toque `v`** da el **objeto idéntico**. Es decir, la hipótesis «`cse` invalida la
equivalencia porque `v = 0` machaca `v`» **es falsa**; lo que decide es qué
elemento de la clase de equivalencia del cero elige `cse`, y eso no se mueve
desde el cuerpo del `if`.

**Lo que NO he probado:** escribir el bloque entero con `short data` **y** los
tres `mr` (o sea, encontrar la forma que impide la coalescencia con un `short`);
el permutador; y el orden de operandos `joy_data + slot` de la rama del volante.

---

## 3. `zSpeech::PursuitEscalation` (552 B) y `BreakAway` (252 B) — una sola
   decisión del planificador, medida con la tabla de dependencias

Son **la misma causa**, y está cerrada al ciclo. En las dos, el objetivo emite
`stw <dir>, 0x18/0x14(r1)` (`data.direction = dir;`) **como primera instrucción
del bloque de unión**, y nosotros la dejamos caer 3 (PE) o 7 (BA) ranuras.

`-fsched-verbose-5` sobre el bloque de unión de `PursuitEscalation`:

```
 insn  code  dep prio cost units
  549   512   0    3    2   lsu   <- stw data.direction
  552   506   0    4    1   iu2   <- lis  …Id@ha
  554   506   0    4    1   iu2   <- lis  …Handle@ha
  557    52   0    3    1   iu2   <- mr r6
  559   507   1    3    1   iu2   <- addi …@l
  561   507   1    3    1   iu2
  563   512   0    3    1   iu2   <- addi r3,r1,8
  565   615   0    2    1   bpu   <- bl
   Ready list (t=1): 557 563 549 554 552   -> escoge 552 y 554 (iu2, 2 por ciclo)
   Ready list (t=2): 557 563 561 559 549   -> escoge 549 (lsu) y 559
```

**El `stw` tiene prioridad 3 y los dos `lis` tienen 4**, porque el `lis` encadena
con su `addi` (2 enlaces) y el `stw` sólo depende del `bl` (1 enlace, coste 1).
Con 2 instrucciones por ciclo, los dos `lis` se llevan el ciclo 1 y el `stw` cae
al 2. **En el objetivo el `stw` va primero**, luego allí su prioridad es ≥ 4.

En `BreakAway` es lo mismo con más ruido: el bloque tiene además tres parejas
`lwz`+`stw` (`speaker_id`, `location_region`, `location`) de prioridad **5**, y
el `stw` de `direction` (prioridad 3) cae a la séptima posición. Y **de ahí sale
la única diferencia de registro de la función**: `alloc.py` da a `dir`
(pseudo 121, 8 refs / 17 de vida, prioridad **14117**) el **primer** turno de
`global_alloc`, y aun así se lleva **r10** en vez de r0 —porque con el `stw`
retrasado su rango de vida se solapa con la reutilización de r0 en
`lwz r0, 0xc(r28)`—. Con el `stw` en su sitio, r0 queda libre: **la fila de
registro es consecuencia de la fila de planificación, no otra diferencia**.

`regmap.py zSpeech "EAXDispatch::BreakAway" --all`: **mismo conjunto de locales,
mismo árbol de bloques, y una sola diferencia**, `dir` r0 → r10.

### 3.1 Vedas (con la sentencia)

| ensayo | qué se barrió | PE | BA |
|---|---|---|---|
| `h1_ccast` | `(Csis::Type_direction)dir` en vez de `static_cast` | idéntico | idéntico |
| `h2_intdir` | `int dir` en vez de `unsigned int dir` | idéntico | idéntico |
| `h3_typedir` | `dir` declarada ya como `Csis::Type_direction` (4 `static_cast` dentro) | idéntico | idéntico |
| `h4_dirlast_ba` | `data.direction = …` **detrás** de speaker/region/location | — | 95,746 → **96,032** (15 → 12 filas) pero el `stw` se va **al final** |
| `h5_declsep` | `unsigned int dir;` y asignación aparte | idéntico | idéntico |
| `h6_elseif` | los tres `if` anidados desplegados en tres `if` sueltos | idéntico | — |

**Conclusión medida: la posición del `stw` no se mueve desde el fuente mientras
el bloque de unión contenga los mismos `lis`+`addi`.** El único camino que queda
—y **no lo he probado**— es **bajar la prioridad de los `lis`**, o sea que la
dirección de `Csis::…Id` / `…Handle` salga en **una** instrucción
(`addi rX, r13, sym@sda21`), que es cosa de `.sdata`, no del fuente de la
función.

---

## 4. `zSpeech::LoadSpeechBank` (316 B) — el `mr` que falta es un segundo puntero

`faltan 1, sobran 0` (312 B contra 316). El objetivo mantiene **dos** registros
con la base `r12+0x58` y copia **en los dos sentidos**:

```
objetivo   addi r4, r12, 0x58 …  mr r11, r4 (fila 16) …  mr r4, r11 (fila 26)
nuestro    addi r7, r12, 0x58 …  mr r4, r7  (fila 14) …  (nada)
```

Es el patrón clásico de un puntero de bucle que vive en un registro en la
cabecera y en otro en el cuerpo. **No lo he atacado** (la veda de la r26 —quitar
la etiqueta `found` y los cuatro `goto`, 95,316 → 85,506— sigue en pie y el
diagnóstico dice que la etiqueta es correcta).

## 5. `zSpeech::Setup` (596 B) — 3 filas, y es el thunk de una llamada virtual

```
objetivo   lwz r9,0(r31) · lwz r0,0x2e4(r9) · lha r3,0x2e0(r9) · mtlr r0 · add r3,r31,r3 · blrl
nuestro    lwz r9,0(r31) · lha r3,0x2e0(r9) · lwz r0,0x2e4(r9) · add r3,r31,r3 · mtlr r0 · blrl
```

Las seis instrucciones son las mismas; cambian **dos parejas de ranuras** dentro
del bloque. Es `sched`, y las seis las genera el compilador para el `blrl` con
ajuste de `this` — **no hay sentencia de fuente que las separe**. No la he
tocado.

## 6. `zFe2::FEngFont::RenderString` (1.572 B) — el modelo de la r26 se queda corto

`alloc.py` sobre el `.greg` real (**183 pseudos en `.lreg`, 107 los asigna
`local-alloc` y sólo 80 llegan a `global-alloc`**) reproduce las tres filas de la
r26 al dígito (#37 pseudo 133 `c` 4806 → r28, #38 pseudo 84 `pcString` 4761 →
r27, #60 pseudo 86 `matrix` 434 → r26), y añade la que faltaba: **#30 pseudo 143
(`pGlyph`, 20 refs / 95, prioridad 8421) → r29, y en el original es r28**. O sea
que **los cuatro registros están corridos uno entero**, no permutados.

Pero **el diff no son cuatro registros**: en el tramo de `PrintCharacter`
(filas 250-300) hay **planificación distinta y registros distintos**
(`lwz r30, 0x8(r31)` contra `lwz r7, 0x8(r31)`, tres `INSERT`/`DELETE` de
`addi`/`lha`/`xoris`, `lis r9, $LC97@ha` donde el objetivo tiene `lfd`). **El
modelo de «una cascada con una sola causa» de la r26 no cubre eso**, y los tres
umbrales que dejó (`pcString` con 35 refs, vida ≤ 353, vida de `c` ≥ 236) atacan
un empate que **no es la única diferencia**. **No he gastado ensayos ahí**: antes
de mover umbrales hay que explicar el tramo 250-300.

## 7. `zEAXSound2::BindToData` y `Play` — no tocadas

Sin ensayos nuevos. El mecanismo de `BindToData` está cerrado al insn en la r26
y la única vía abierta (que el pseudo del `-1` **no reciba registro** para que
`reload` lo rematerialice en r0) pide subir la presión del bloque o disparar
`update_equiv_regs`, y **no he encontrado cómo hacerlo desde el fuente**.
`Play` sigue con el eje de la cabecera abierto. **Prioricé el resto porque
`UnlockPalette` dio señal a la primera y el mecanismo de la §1.2 es
extrapolable.**

---

## 8. Ensayos numerados (63)

- **JoyE / `ActualReadJoystickData` (28)**: `a1_ternsubx` 88,89168 · `a2_ternsub2`
  85,86902 · `a4_ternall_nov` 86,67255 · `b1_yv` 97,19144 · `b2_castst` 97,16625 ·
  `b3_castst_yv` 97,02771 · `b4_yv_thendata` 97,35516 · `b5_datastick` 97,19144 ·
  `b6_datastick2` 97,32997 · `b7_vkeep` 95,28967 · `b8_noif` 96,58690 ·
  `c1_short` 96,19647 · `c2_short_nc` 96,19647 · `c3_vdata` 96,19647 ·
  `c4_stdata` 96,19647 · `c5_stdata_yv` 96,07053 · `c6_short_yv` 96,44836 ·
  `c7_shortv` 94,82368 · `c8_intdata_shortv` 96,47355 · `e1_datazero` 97,35516 ·
  `e2_empty` 96,58690 · `e3_datazero_yv` 97,19144 · `e4_testdata_setv` 97,32997 ·
  `e5_vst_datazero` 96,41058 · `e6_slotzero` 96,58690 (+3 fallos de compilación).
- **EAXDispatch (6)**: `h1`…`h6` (cifras en §3.1).
- **UnlockPalette (21 + 24 + 24 = 69 compilaciones, 21 formas distintas)**:
  `u1`…`u8`, `v1`…`v6`, `x3`…`xa`, `y_*` (24 permutaciones), `w_*` (24
  permutaciones), `z0`…`z7`, `t1`…`t6`, `q1`…`q8`, `s_*` (26). Las cifras que
  importan están en §1.2 y §1.5.
- **ToggleCapsLock (8)**: `k1_shift0` 91,08696 (idéntico) · `k2_shiftafter`
  71,17391 (**100 B**) · `k3_notcaps` 76,60870 (**80 B**) · `k4_ifelse`
  76,69566 (**96 B**) · `k5_capsvar` 91,08696 (idéntico) · `k6_shiftfalse_int`
  91,08696 (idéntico) · `k7_capslast` 80,65218 (repite la veda de la r26) ·
  `k8_neq` 91,08696 (idéntico).
  **Diagnóstico**: el cero de `mbShift = false` necesita **r10** y se lleva
  **r0**, que es **el primero de `REG_ALLOC_ORDER` de rs6000**
  (`0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30, …`). Para que caiga en r10 tienen
  que asignarse antes tres pseudos (los de `mnMode` r9, `mbCaps` r11 y el `1` de
  `mbCaps = true` r0). **Ninguna de las 8 formas cambia el orden de asignación.**

## 9. Lo que NO he probado

- **`UnlockPalette`**: que `0xFFFF8000` deje de ser constante para `fold` (§1.4).
  Es lo único que queda y son 2 filas.
- **`ActualReadJoystickData`**: la forma con `short data` que **no** coalesce con
  el temporal; el orden `joy_data + slot` de la rama del volante; el permutador.
- **`PursuitEscalation`/`BreakAway`**: mover `…Id`/`…Handle` a `.sdata` para que
  el `lis` desaparezca y el `stw` gane el ciclo 1. **Toca `configure.py`/enlace,
  así que no lo he hecho**; es la única palanca que queda y vale **804 B**.
- **`RenderString`**: nada. Antes hay que explicar el tramo 250-300 (§6).
- **`IconScroller`** (384 B) y **`Manager::NotifyEventCompletion`** (504 B): sólo
  las he leído. En `NotifyEventCompletion` la única diferencia real es que el
  objetivo tiene `lis r9, mLastSpeakerID@ha` **antes** del `cmplwi`/`ble` (o sea,
  en el bloque dominante) y nosotros **después**: es `gcse`/PRE moviendo un
  `elf_high`, exactamente el mecanismo del §8 de HERRAMIENTAS.
- **`BindToData`**, **`Play`**, **`LoadSpeechBank`**, **`Setup`**: sin ensayos.

## 10. Convivencia

- **Ficheros del árbol tocados: UNO**,
  `src/Speed/GameCube/Src/Ecstasy/TextureInfoPlat.cpp` (7 líneas). **Sin commit.**
- Todos los barridos se hicieron sobre **copias en el scratchpad**, nunca sobre
  `src/`. `git status` de mis cinco ficheros candidatos: sólo ése aparece.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt` ni los bloques
  `__ANDROID__`.
- Entré con 15 GB libres y salgo con **16 GB**; borrados los volcados RTL
  (`rtl26/`) y los `.o`/`.ii` de variante. Queda 6,2 MB con prefijo `c27mx_`.
- **Aviso**: `scratchpad/rtl26/` de la r26 **ya no existe** (lo borró otro
  agente a mitad de mi ronda); si un informe anterior lo cita, hay que
  regenerarlo con `c26eax2_dump.py`.

## 11. Herramientas dejadas (prefijo `c27mx_`)

| | |
|---|---|
| **`c27mx_run.py <variantes.py> [nombre]`** | compila cada variante sobre una **copia** del `.cpp` y da `% / B / insns / filas` de uno o varios símbolos. El módulo define `BASE_FILE`, `UNIT`, `SYM`/`SYMS`, `PRE` (texto del stub) y `EXTRA` (flags, p. ej. el `-I` del directorio del fuente, que hace falta cuando el `.cpp` tiene `#include` relativos). Los edits admiten `(viejo, nuevo)` o `(viejo, nuevo, 'all')`. **~2 s por variante** en JoyE/TextureInfoPlat, ~5 s en EAXDispatch |
| `c27mx_range.py <unidad> <sym> <lo> <hi>` | vuelca un RANGO del diff con las dos columnas, no sólo las filas que difieren |
| `c27mx/c27mx_dump.py` | copia de `c26eax2_dump.py` con `work` bajo `c27mx/`, para no volver a perder los volcados |
| `c27mx_joy*.py`, `c27mx_disp.py`, `c27mx_tex*.py`, `c27mx_kbd.py` | los cinco ficheros de variantes de esta ronda |
| `c27mx/audit1_*.txt`, `audit2_zPlatform.txt` | las pasadas de `audit.py` |
