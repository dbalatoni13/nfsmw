# Ronda 27 — `_bOutput` (zBWare) y `Geometry::SphereVsBox` (zDynamics)

## 0. Verificación del encargo

`build_direct.py zBWare zDynamics zMain` + `triage.py --muro`:

    MURO: 3 funciones, 9252 B
        5180 B   99.983%  zBWare     _bOutput__FP11bOutputInfoPCcP13__va_list_tag
        2176 B   99.972%  zMain      __12EPlayRaceNISP7GMarkerPCcT2iiT2T2
        1896 B   99.036%  zDynamics  SphereVsBox__...

**Reproduce el encargo.** `audit.py` primera pasada: zBWare **238 ok / 0 FALLA**,
zDynamics **40 ok / 0 FALLA**, zMain **1379 ok / 0 FALLA**.

---

## 1. Resultado

| función | antes | después |
|---|---|---|
| **`SphereVsBox`** (zDynamics, 1.896 B) | 99,03587 % · **14 filas** | **99,47890 % · 8 filas** |
| `_bOutput` (zBWare, 5.180 B) | 99,98301 % · 2 filas | sin cambio (2 filas) |

**Ninguna de las dos cierra.** `measure.py --cmp` (base y medida con
`build_direct.py` delante de cada una): **+0 B, +0 funciones, 0 unidades
cambian** — `matched_code` es todo-o-nada, así que las 6 filas no valen bytes,
pero el diagnóstico principal (el par f30/f31 y la posición del `cmpwi`) **queda
cerrado** y lo que sobra es un solo mecanismo acotado.

`audit.py` **dos pasadas** al terminar: zBWare 238/0, zDynamics 40/0, zMain
1379/0. `frozen.py chk` zBWare y zMain: **idénticos al congelado** (no los he
tocado). zDynamics **congelado** (`2701aea47cb5193a`).

**Único fichero mío: `src/Speed/Indep/Src/Physics/Dynamics/Geometry.cpp`.** Los
otros nueve de `git status` son de agentes de esta misma ronda. Sin commit.

---

## 2. `SphereVsBox`: la barrera va DETRÁS de `a_lp++`

### 2.1. El cambio, y por qué la r26 no lo vio

    -        __asm__("");
             penetration[a_lp] = b_dim - (dists[a_lp] - radius);
             prev_penetration[a_lp] = b_dim - (prev_dists[a_lp] - radius);
             a_lp++;
    +        __asm__("");
         } while (a_lp <= 2);

La r26 barrió la barrera **delante** de `a_lp++`, entre las dos penetraciones,
delante de cada `if`, al principio del bloque y dos y tres veces seguidas —
**nunca detrás del incremento**. Vale **6 filas**: 99,03587 → **99,47890 %**.

Con ella el bloque 7 se parte en dos grupos:
`{cadena FP + a_lp++}` · barrera · `{cmpwi + las dos inserciones de PRE}`.
El rango vivo de `b_dim` (pseudo 152) deja fuera al `cmpwi` y a los dos `addi`,
su prioridad sube de `300000/39 = 7692` por encima de los **8000** de `p_dot`
(157) y **`b_dim` coge f31 como el objetivo**: caen las **ocho** filas de
f30/f31 y las de la posición del `cmpwi`.

**Y el mapa de líneas la PERMITE.** La r26 declaró deuda porque el escalón
Δ77→76 dice que el original no tiene sentencia entre `in++` y `penetration`.
En la posición nueva no hay tal contradicción: el objetivo tiene `a_lp++` en la
**471** y el `} while` en la **474**, o sea **dos líneas sin instrucción justo
ahí (472-473)**. Sigue siendo un constructo que el original no tenía (regla 3
del brief), pero ya no lo desmiente el mapa de líneas.

### 2.2. Las 8 filas que quedan, y su mecanismo

    115 addi r22, r1, 0x68   | addi r31, r31, 0x1     <- a_lp++ delante
    117 addi r19, r1, 0x78   | (falta)
    123 addi r31, r31, 0x1   | addi r22, r1, 0x68     <- las inserciones detrás
    124 (falta)              | addi r20, r1, 0x78
     32 li    r20, 0x0       | li    r19, 0x0         }
    113 addi  r20, r20, 0x1  | addi  r19, r19, 0x1    } `in` y `&prev_penetration[0]`
    137 cmpwi r20, 0x2       | cmpwi r19, 0x2         } se cambian r19 y r20
    412 mr    r11, r19       | mr    r11, r20         }

Son **una sola causa**: el objetivo pone las **dos inserciones de PRE**
(`addi r22,r1,0x68` = `&penetration[0]`, `addi r19,r1,0x78` =
`&prev_penetration[0]`, ambas para usos POSTERIORES al bucle) en las ranuras de
relleno 2 y 4, y `a_lp++`/`cmpwi` **al final**; nosotros al revés. El reparto
r19/r20 es consecuencia de eso.

### 2.3. Por qué no se puede mover — probado con los volcados RTL

1. **gcse/PRE inserta siempre con `insert_insn_end_bb`**, delante del salto, con
   el LUID más alto del bloque. Verificado en el RTL: el orden pre-`sched1` del
   bloque 7 es `421, 422, 424, 444, 445, 446, 448, 452(a_lp++), 457(cmpwi),
   2002, 2005, 458`.
2. **En `sched2` manda la prioridad**: `a_lp++` vale **3** (a_lp++ → cmpwi → ble)
   y las dos inserciones **2**, así que `sched2` **siempre** adelanta el
   incremento. Una barrera sólo puede empujar las inserciones **más tarde**,
   nunca antes: por eso `{fp, a_lp++} | {cmpwi, addi, addi}` (8 filas) es el
   mejor punto del eje y `{fp} | {a_lp++, cmpwi, addi, addi}` da 18.
3. **En el bloque del objetivo NADA lee r31 antes del incremento** (las 12
   instrucciones están listadas arriba; el direccionamiento va por r30, el
   índice escalado), así que **no hay ninguna dependencia posible** que retrase
   `a_lp++`. Y **no hay etiqueta en `0x80089F3C`**: es un solo bloque básico de
   12 instrucciones, idéntico al nuestro.

### 2.4. La anomalía que queda abierta, y es LA pregunta

**`sched1` y `sched2` ordenan al revés, y `sched1` no ordena por prioridad.**
Volcado literal (`-dS -fsched-verbose-5`), bloque 7 antes de reload:

    ;;      421  ... prio 10 ... fpu : 458 422
    ;;      452  ... prio  3 ... iu2 : 458 457      (a_lp++)
    ;;     2002  ... prio  2 ... iu2 : 458          (insercion de PRE)
    ;;     2005  ... prio  2 ... iu2 : 458          (insercion de PRE)
    ;;	Ready list (t =  2):    452  421  2005  2002
    ;;		--> scheduling insn <<<2002>>> on unit iu2
    ;;		--> scheduling insn <<<2005>>> on unit iu2

La lista se imprime **de peor a mejor** (comprobado contra el volcado de
`_bOutput`, donde sí coincide con prioridad+LUID). Aquí el mejor es **2002, con
prioridad 2**, por delante de un `fsubs` de prioridad **10**. Ninguno de los
criterios documentados de `rank_for_schedule` lo explica: ni prioridad, ni
`INSN_REG_WEIGHT`, ni clase respecto a `last_scheduled_insn`, ni nº de
dependientes con su signo estándar. El único ajuste que reproduce la lista
exacta `452 421 2005 2002` es **«menos dependientes gana, luego LUID menor
gana»** — y ése **no** reproduce el volcado de `_bOutput`. O sea: **este
`cc1plus` de ProDG tiene una regla de ordenación en `sched1` que no es la de
GCC 2.95 documentada**, y el objetivo se comporta como si `sched2` usara esa
misma regla en vez de la de prioridad.

**Ahí es donde miraría el siguiente**: `sched1` ya produce el orden bueno (las
dos inserciones en el ciclo 2); es `sched2` quien lo deshace. Si alguien
encuentra qué hace que `sched2` no adelante `a_lp++` —o desmonta la regla real
de `rank_for_schedule` de este compilador— la función cierra.

### 2.5. Ensayos numerados (arnés `c27bw_var.py`, **3,2 s** por variante)

Base 99,03587 % / 14 filas. **1.896 B en todas salvo donde se indica.**

| # | forma | % | filas |
|---|---|---|---|
| **e7** | **`__asm__("")` DETRÁS de `a_lp++`** | **99,47890** | **8** ← APLICADO |
| e2 | asm en la posición r26 **y** detrás de `a_lp++` | 99,47890 | 8 (idéntico) |
| g5 | `} while (a_lp < 3)` + asm detrás de `a_lp++` | 99,47890 | 8 (idéntico) |
| e5 | asm delante de `a_lp++` (= c1 de la r26) | 98,95570 | 18 |
| e9/e10 | asm delante **y** detrás de `a_lp++` | 98,95570 | 18 |
| g3 | `} while (++a_lp <= 2)` + asm delante del `while` | 98,95570 | 18 |
| g2 | `} while (++a_lp <= 2)` + asm en la posición r26 | 99,03587 | 14 (= base) |
| d3/g1 | **sin barrera** (con y sin `++a_lp` en la condición) | 98,48945 | 23 |
| g4 | `} while (a_lp != 3)` + asm detrás | 98,52321 | 21 |
| d1 | `__asm__ __volatile__("" : : "f"(b_dim))` | 95,10127 | 78 (**1.900 B**) |
| d4 | `__asm__ __volatile__("" : : "f"(p_dot))` | 95,01688 | 85 (**1.900 B**) |
| d2 | `__asm__ __volatile__("" : "+f"(b_dim))` | **no compila** (`b_dim` es `const`) |
| e1 | `prev_penetration` antes que `penetration` | 95,66878 | 85 |
| e4 | `prev_dists` antes que `dists` | 96,40929 | 68 (1.888 B) |

**Veda con su sentencia — el eje `n_refs` de `b_dim` está cerrado.** La tabla de
allocnos (`alloc.py` sobre el `.greg`) da `b_dim` = pseudo 152, `n_refs` **10**,
`live_length` **39**, prioridad **7692**; `p_dot` = 157, `n_refs` 6, live 15,
prioridad **8000**. Subir `n_refs` a 12 bastaría (`360000/39 = 9230`), y como el
contador va **por dos** dentro del bucle hace falta **una referencia más a
`b_dim`**: la única forma sin cambiar la fuente es un operando de `asm`, y eso
**materializa una instrucción de más** (d1: +4 B). Bajar la prioridad de `p_dot`
por el mismo camino, igual (d4: +4 B).

**Lo que sigue puesto de la r26**: pasar `result->mCollision_point` en vez de
`point_1` a `RayExitSphere`.

---

## 3. `_bOutput`: el mecanismo, cerrado por los dos lados con el RTL

### 3.1. Corrección al encargo: **el `addi r23` del objetivo SÍ es de PRE**

El encargo pedía «¿de dónde sale el `addi r23` del objetivo si no es de PRE?».
La respuesta es que **sí lo es**, y la premisa de la r26 estaba mal planteada.
En el objetivo `addi r23, r1, 0x10` aparece **tres veces** (filas 499, 910 y
913) y en el nuestro también (499, 910, 915); r23 = `&cvtbuf[0]` y sólo se usa
en dos restas (filas 550 y 999). El RTL lo dice literalmente:

    ;; Start of basic block 263
    (insn 7298 ... (set (reg:SI 2016) (plus:SI (reg:SI 31 r31) (const_int 16))))
    (jump_insn 4715 ...)                       <- bb263: PRE al final
    ;; Start of basic block 264
    (insn 4723 ... (plus (reg/v:SI 90) (const_int -1)))     precision - 1
    (insn 4729 ... (plus (reg/v:SI 90) (const_int  1)))     desiredPrecision
    (insn 4725 ... (minus (reg/v:SI 1299) (reg:SI 1394)))   p -= ...
    (insn 7303 ... (set (reg:SI 2016) (plus (reg:SI 31 r31) (const_int 16))))
    (jump_insn 6811 ...)                       <- bb264: PRE al final, otra vez

**Las dos son inserciones de PRE** (`insert_insn_end_bb`, una por arista). La
del objetivo no es «otra cosa»: es **la misma**, planificada una ranura antes.

### 3.2. Lo que hace falta, y por qué la fuente no llega

`sched1` del bloque 264 (`-dS -fsched-verbose-5`):

    ;;     4723  prio 3  iu2 : 6811 4725      subi   (precision - 1)
    ;;     4725  prio 2  iu2 : 6811           subf   (p -= ...)
    ;;     4729  prio 2  iu2 : 6811           addi r5 (desiredPrecision)
    ;;     7303  prio 2  iu2 : 6811           addi r23 (PRE)
    ;;	Ready list (t =  1):    7303  4729  4723
    ;;		--> scheduling insn <<<4723>>> ... <<<4729>>>

Ritmo de emisión **2**. En el ciclo 1 entra 4723 (prioridad 3) y **el mejor de
{4729, 7303}**: empatan en prioridad (2), en peso, en clase y en dependientes
(1: el salto), así que **decide el LUID** — y el de PRE es siempre el más alto
del bloque.

**Para que salga el objetivo hace falta `LUID(7303) < LUID(4729)`, es decir que
la instrucción de `desiredPrecision = precision + 1` NAZCA DESPUÉS de la
inserción de PRE.** Como PRE inserta al final del bloque, eso sólo puede pasar
si ese `addi` lo **emite un pase posterior a `gcse`**, y los únicos que añaden
al final de un bloque son **`loop`** (izado de invariantes y guarda del
preencabezado — bb264 **es** el preencabezado del `while (precision > 0)`) y
`reload`. `precision` cambia dentro del bucle, así que `precision + 1` **no es
invariante** y `loop` no lo puede izar. **Ése es el techo.**

### 3.3. Ensayos numerados (arnés `c27bw_bv.py`, ~5 s por variante)

Base 99,98301 % / 2 filas / 5.180 B. **Nueve formas nuevas sobre las 62 de la
r24-r26.** El hueco que quedaba era la barrera **vacía** en bb264 (la r26 sólo
probó `__asm__("" : : "r"(cvtbuf))`, que materializa y cuesta 12 B):

| # | forma | % | filas |
|---|---|---|---|
| o1 | `__asm__("")` **entre** `p -=` y `desiredPrecision` | 99,82858 | **4** (peor) |
| o2 | `__asm__("")` detrás de `desiredPrecision` | 99,98301 | 2 (idéntico) |
| o3 | `__asm__("")` delante de `p -=` | 99,98301 | 2 (idéntico) |
| o4 | sentencias intercambiadas + `__asm__("")` detrás | 99,98301 | 2 (idéntico) |
| o5 | `const int dp = precision + 1;` (declaración con inicializador) | 99,98301 | 2 |
| o6 | `desiredPrecision` **fuera** del `if (lowerVal \|\| precision)` | 99,84170 | **3** (peor) |
| o9 | bloque anónimo `{ desiredPrecision = precision + 1; }` | 99,98301 | 2 |
| p1 | `if (desiredPrecision < 0) { desiredPrecision = precision + 1; }` | 99,98301 | 2 |

o1/o2/o3 son la **confirmación experimental del modelo**: la barrera sólo puede
empujar la inserción de PRE **más tarde** (o1 pasa de 2 a 4 filas), nunca antes.
**El eje de la barrera en bb264 queda agotado.**

### 3.4. Lo que dice el DWARF (volcado REGENERADO, la caché estaba rancia)

En el bloque del `case 'z'`:

    -            int desiredPrecision;        <- el ORIGINAL no le da registro
    +            int desiredPrecision; // r5

El original **no tiene localización DWARF** para `desiredPrecision` (ni para
`digit`, `offset` ni `decimalChr`) aunque el objetivo sí materializa el valor en
r5 (fila 915, usado en la 948 por `subf r3, r5, r3`). Es el mismo indicio que la
r26 declaró no accionable, y sigue siéndolo: apunta a que el pseudo del DECL
murió y el valor vive en un temporal, pero **no da una forma de fuente**.

Diferencias estructurales que seguimos teniendo (regla §2 del brief: quitarlas
de una en una empeora, seis medidas en contra): **4 etiquetas de más**
(`OUTPUT`, `positioned`, `Z_INT`, `VECT_OUTPUT`), **`int textLen`** (vale 35
filas, deuda declarada de la r24), un bloque anónimo `{ double tmpNum; }` en
`%f` y otro **`{ unsigned char c; }`** en el redondeo del `%z` que el original
no tiene. Ninguno está en bb264.

---

## 4. Qué NO he probado

- **`SphereVsBox`**: no he tocado el **segundo ni el tercer bucle** para alargar
  la vida del pseudo 627 (era el único camino que dejaba abierto la r26) —
  **porque con la barrera en su posición nueva ese camino ya no existe**: el
  problema del pseudo 627 sólo aparecía con la barrera DELANTE de `a_lp++`, y
  esa variante ahora está a 18 filas contra 8. No he gastado restricciones de
  registro (`register float b_dim asm("f31")`) ni el permutador. No he atacado
  por separado el reparto r19/r20 (`in` contra `&prev_penetration[0]`): son 4 de
  las 8 filas y son consecuencia de las otras 4.
- **`_bOutput`**: no he tocado las cuatro etiquetas de más, ni `textLen`, ni el
  bloque anónimo `{ unsigned char c; }` del redondeo — la regla §2 dice que
  quitarlas de una en una empeora y la reescritura completa de la función no
  cabía en esta ronda. No he barrido banderas (`-fgcse` no existe por función en
  GCC 2.9). No he tocado `GENERIC_FLOAT` ni el `__asm__("" : "+m"(number))`.
- **`__12EPlayRaceNIS`** (zMain, 2.176 B): muro desde la r23, no la he tocado.

## 5. Herramientas y trampas de esta ronda

- **`lmap.py` peta con `_bOutput`** (`TypeError: unsupported operand ... NoneType`
  en la línea 70): no encuentra el rango del símbolo. El nombre real es
  **`_bOutput__FP11bOutputInfoPCcP13__va_list_tag`** (con `tag` al final);
  `triage.py` lo imprime **truncado a 42 caracteres** y se lee como
  `..._t`, que es lo que rompe cualquier herramienta a la que se le pase copiado.
- **`Geometry.cpp` tiene finales de línea MEZCLADOS** (cuerpo CRLF, comentarios
  LF): el parcheo tiene que ser línea a línea conservando el terminador de cada
  una (§3.5 del brief). `bPrintf.cpp` es CRLF entero.
- La caché de `dwbody.py` estaba **rancia** y daba el `va_arg` del `%z` al revés
  del estado actual del árbol; `regmap.py zBWare "_bOutput" --ours` la arregla.
- **Arneses nuevos en el scratchpad** (prefijo `c27bw_`): `c27bw_var.py`
  (SphereVsBox, 3,2 s, restaura siempre), `c27bw_bv.py` (`_bOutput`, ~5 s),
  `c27bw_bw.py` / `c27bw_dy.py` (diff por función), `c27bw_rtl.py` (volcados RTL
  parametrizado por `UNIT=` y `SRC=`, sirve para cualquier unidad).
- **Disco**: borrados los volcados RTL míos y **417 MB** de JSON de rondas
  cerradas (`rtl26/`, `c26lk_try/`, `c26*.json`, `d_z*.json` > 5 MB). Quedan
  **14 GB** libres; `c27lk_try/` pesa **1,4 GB** y es de un agente de esta ronda.
