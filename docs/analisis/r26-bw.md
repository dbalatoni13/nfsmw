# Ronda 26 — `_bOutput` (zBWare) y `_._14ESpawnFragment` (zMain)

## 0. Verificación del encargo

`build_direct.py zBWare zMain` + `triage.py --muro zBWare zMain` al empezar:

    MURO: 3 funciones, 9520 B
        5180 B   99.983%  zBWare  _bOutput__FP11bOutputInfoPCcP13__va_list_t   2 registros
        2176 B   99.972%  zMain   __12EPlayRaceNISP7GMarkerPCcT2iiT2T2         3 registros
        2164 B   98.869%  zMain   _._14ESpawnFragment                          23 filas

**Coincide con el encargo.** `audit.py` primera pasada: zBWare **238 ok / 0 FALLA**,
zMain **1378 ok / 0 FALLA**. Árbol limpio en `bPrintf.cpp`, `ESpawnFragment.cpp`
y `UVectorMathGC.hpp`.

---

## 1. Resultado

| función | antes | después |
|---|---|---|
| **`_._14ESpawnFragment`** (zMain, 2.164 B) | 98,86876 % · 23 filas | **100 % · 0 filas · 2.164 B** |
| `_bOutput` (zBWare, 5.180 B) | 99,98301 % · 2 filas | sin cambio (2 filas) |

`measure.py --cmp` (base y medida con `build_direct.py` delante de cada una):

    +2164 B, +1 funciones, 1 unidades cambian
         +2164 B    +1 fns  Speed/Indep/SourceLists/zMain   155436 -> 157600

zMain pasa de **97,2837 % a 98,6381 %** y de 1.378 a **1.379 funciones al 100 %**.
`audit.py` dos pasadas al cerrar: zMain **1379 ok / 0 FALLA**, zBWare
**238 ok / 0 FALLA**. `frozen.py cong Speed/Indep/SourceLists/zMain`
(huella `6906acd19d2ae332`); zBWare **idéntico al congelado**.

**`_bOutput` NO cierra**: zBWare sigue con esa función a 2 filas y **NO es
candidata a promoción**. Ficheros tocados: `ESpawnFragment.cpp` y `bPrintf.cpp`.
`UVectorMathGC.hpp` y `Bounds.h` quedan **limpios** (`git status` verificado).
Sin commit.

---

## 2. `ESpawnFragment`: el mecanismo, al insn, y la sentencia que lo cierra

### 2.1. Lo primero: el mapa de líneas VETA `VU0_quattom4` sin necesidad de medir

`lmap.py zMain "_._14ESpawnFragment"` da la línea de fuente **del original** para
cada uno de los 16 `stfs` del inline:

    472 [0][0]  473 [0][1]  474 [0][2]  475 [0][3]
    477 [1][0]  478 [1][1]  479 [1][2]  480 [1][3]
    482 [2][0]  483 [2][1]  484 [2][2]  485 [2][3]
    487 [3][0]  488 [3][1]  489 [3][2]  490 [3][3]

y los tres `fsubs` de `1.0f - (...)` en 472, 478 y 484. Es **exactamente** nuestro
orden natural, con las líneas en blanco entre grupos incluidas. Y las líneas del
mapa son **de antes de planificar** (`save_line_notes` guarda la última nota
anterior en el orden ORIGINAL, y `restore_line_notes` la reemite tras planificar),
así que no las mueve `sched1`.

**Prueba directa de que el `VU0_quattom4` del original es el natural**, sin las
625 parejas ni el oráculo de la r25. Confirma la r23/r25 y cierra el frente.

### 2.2. Quién decide: `INSN_REG_WEIGHT`, no «la 1.0 muere 15 ranuras antes»

Volcado de `sched1` (`-fsched-verbose-5`, **con guion**) del bloque 12 y volcado
`.regmove` del mismo:

    ;;      865   523     0     6    44     2   lsu : 1019 1008 993 989 978     [3][0]=0.0
    ;;      895   523     0     6    44     2   lsu : 1019 1008 1004 993 978    [3][1]=0.0
    ;;      925   523     0     6    44     2   lsu : 1019 1008 993 978         [3][2]=0.0
    ;;      955   523     0     6    44     2   lsu : 1019 1008 993 978         [3][3]=1.0
    ;;      599/717/835                             : 1019 1008 993 978         [0][3]/[1][3]/[2][3]

Los siete `stfs` de constante **empatan en prioridad (44)**. Los desempata
`rank_for_schedule`, y el orden de sus criterios es: prioridad → **`INSN_REG_WEIGHT`
(menor gana)** → clase respecto a `last_scheduled_insn` → nº de dependientes
(más gana) → `INSN_LUID` (menor gana). `INSN_REG_WEIGHT` **sólo se mira antes de
`reload`**, o sea en `sched1`.

`find_insn_reg_weight` suma +1 por registro que **nace** y resta 1 por cada nota
`REG_DEAD`/`REG_UNUSED`. Un `stfs` no hace nacer nada (el destino es MEM), así que:

- store que **mata** su fuente → peso **−1**,
- store que no la mata → peso **0**.

En el `.regmove` nuestro, el insn 955 (`result[3][3] = 1.0f`) lleva

    (reg:SF 321)) 523 {*movsf_hardfloat} (nil)
        (expr_list:REG_DEAD (reg:SF 321)

porque **es la última referencia al pseudo de `1.0f`** (los tres `1.0f - (...)`
están en las líneas 472/478/484, delante). Peso **−1** ⇒ `sched1` lo **adelanta**.

El modelo reproduce nuestro orden de `sched1` **al dígito**:

| | peso | dependientes | LUID | orden |
|---|---|---|---|---|
| 925 `[3][2]=0.0` | −1 (muere `0.0f`) | 4 | menor | 1.º |
| 955 `[3][3]=1.0` | −1 (muere `1.0f`) | 4 | mayor | 2.º |
| 865 `[3][0]` / 895 `[3][1]` | 0 | **5** | — | 3.º y 4.º |
| 599/717/835 `[0][3]/[1][3]/[2][3]` | 0 | 4 | — | 5.º-7.º |

(865 y 895 tienen un dependiente más: el `stfs` de la posición que reescribe esa
misma ranura, insns 989 y 1004.) `local-alloc` ve morir la `1.0f` la primera,
rota los tres FPR (1/32767, 1.0, 0.0) y `sched2` fija el store al principio.

**Corrección a la r24**: no es que «la 1.0 muera 15 ranuras antes» y de ahí salga
la rotación; es que **la nota `REG_DEAD` le da al store peso −1** y por eso
`sched1` lo adelanta — la muerte temprana y la rotación son la consecuencia.

### 2.3. La sentencia

```cpp
model->GetTransform(m_outer);
bounds->GetTransform(m_inner);
m_inner[3][3] = 1.0f;          // REDUNDANTE: no emite un byte
UMath::Mult(m_inner, m_outer, m_final);
```

`VU0_quattom4` ya deja `[3][3] = 1.0f`, así que la asignación es **semánticamente
nula**; `flow` borra el store del inline por muerto y deja el nuestro, que tiene
el LUID más alto de los siete y por tanto se planifica **el último**, que es lo que
hace el objetivo. **Mismo tamaño, 2.164 B, 0 filas, `audit.py` ok.**

### 2.4. Ensayos numerados (arnés `c26bw_sp.py`, ~6 s por variante)

Base: 98,86876 %, 2.164 B, 23 filas.

    w1  `bool simple_physics` fuera del if             98,10721   31   PEOR
    w2  `if (fNoPhysics == 0)`                         98,86876   23   idéntico
    w3  `bounds` tras `model->GetTransform`            93,40111  139   PEOR
    w4  `SmackableParams` como local con nombre        96,9353    43   PEOR
    w5  las tres matrices antes de coger `bounds`      98,86876   23   idéntico
    w6  `bounds->GetTransform` antes que `model->`     92,589645 141   PEOR (= s8 de la r24)
    w7  referencia intermedia en `UMath::Mult`         97,61183   97   2.168 B
    z1  `m_outer[3][3] = 1.0f` (SONDA)                 99,815155   1   2.168 B  <- prueba el modelo
    z2  `m_inner[3][3] = 1.0f`                        **100 %      0   2.164 B  <- APLICADO**
    z3  `if (m_inner[3][3] != 1.0f) { }`               98,86876   23   idéntico (se borra)
    z4  `m_final[3][3] = 1.0f`                         99,815155   1   2.168 B
    z5  `float one = 1.0f; __asm__("" : "+f"(one));`   98,86876   23   idéntico

**`z1` es la sonda que cerró el diagnóstico**: metiendo un uso de `1.0f` *después*
del inline, las **23 filas caen a 1**, y esa única fila es el `stfs` de más que
añade la propia sonda. O sea: todo el racimo —los tres FPR y los ocho `stfs`—
depende **sólo** de si la `1.0f` sigue viva detrás del store de `[3][3]`.

`z5` demuestra que **una barrera de registro no vale**: el `asm` no cuenta como
referencia viva del pseudo de la constante. `z3` demuestra que un `if` vacío se
borra antes de `life_analysis`.

### 2.5. Deuda declarada (regla 3 del brief)

`m_inner[3][3] = 1.0f;` **no es la sentencia del original**, aunque el objeto sea
byte a byte el mismo:

- el mapa de líneas del objetivo da al `stfs` superviviente
  **`UVectorMathGC.hpp:490`**, o sea en el original sobrevive el store *del inline*;
  en nuestra versión sobrevive el nuestro y se borra el del inline;
- el objetivo **no tiene ninguna línea de `ESpawnFragment.cpp` entre 77
  (`model->GetTransform`) y 82 (`UMath::Mult`)**, así que ahí el original no tiene
  ninguna sentencia.

Lo que el original tiene es **algún uso de `1.0f` posterior al store que no emite
código**, y no lo he encontrado: el objetivo carga `lbl_803F0B4C` (1.0f) **una sola
vez** y no la vuelve a leer detrás de `801E5454`. Mirados y descartados:
`SmackableParams`, `WorldConn::Pkt_Body_Send`, `UMath::Mult`, `UMath::Scale`,
`fInheritVelocity > 0.0f` (carga `0.0f`, no `1.0f`) y `Bounds::GetTransform`
(un `matrix.v3.w = 1.0f` ahí daría al store superviviente una línea de `Bounds.h`,
no de `UVectorMathGC.hpp`). **Queda abierto sólo como fidelidad de fuente: el
código emitido ya es el del objetivo.**

---

## 3. `_bOutput`: las 2 filas quedan, pero ahora hay una PRUEBA de por qué

    913  addi r23, r1, 0x10   |  addi r5, r22, 0x1
    915  addi r5, r22, 0x1    |  addi r23, r1, 0x10

Bloque **bb 264** (preencabezado del `while (precision > 0)` del caso `%z`):

    objetivo:  subi r0,r22,1 · addi r23,r1,0x10 · subf r3,r0,r3 · addi r5,r22,1
    nuestro :  subi r0,r22,1 · addi r5,r22,1    · subf r3,r0,r3 · addi r23,r1,0x10

### 3.1. El volcado, releído entero (y dos correcciones a la r25)

`cc1plus -dS -fsched-verbose-5` sobre `bPrintf.cpp`. **Ojo con dónde cae el
verbose**: con `-dS` el de `sched1` va al fichero de volcado (`x.ii.sched`) y el de
`sched2` a stderr. Leer sólo stderr hace creer que **no hay `sched1`** (400 bloques
«after reload» y ninguno «before reload»); son los dos, y deciden lo mismo.

    ;;      insn  code    bb   dep  prio  cost   units
    ;;     4723    52     0     0     3     1    iu2 : 6811 4725      subi  (precision-1)
    ;;     4725    64     0     1     2     1    iu2 : 6811           subf  (p -= ...)
    ;;     4729    52     0     0     2     1    iu2 : 6811           addi r5 (desiredPrecision)
    ;;     7303    52     0     0     2     1    iu2 : 6811           addi r23 (insercion de PRE)
    ;;     6811   763     0     4     1     1    bpu :

    ;;	Ready list (t =  1):    7303  4729  4723
    ;;		--> scheduling insn <<<4723>>>
    ;;		--> scheduling insn <<<4729>>>

**Correcciones a la r25**: las prioridades son **3/2/2/2**, no 2/1/1/1 (todas las
insns del bloque dependen del salto con coste 1); y `INSN_REG_WEIGHT` **sí es un
criterio vivo aquí** (esto es `sched1`, antes de `reload`), sólo que los dos
candidatos valen **+1** — cada uno hace nacer un pseudo y ninguno tiene nota
`REG_DEAD`. El orden real de `rank_for_schedule` es
**prioridad → peso → clase vs `last_scheduled_insn` → nº de dependientes → LUID**,
y la lista sale ordenada de peor a mejor: el planificador coge `ready[--n_ready]`.

### 3.2. La prueba de que el camino de la r24/r25 estaba cerrado por construcción

Los dos candidatos empatan en **prioridad (2)**, en **peso (+1)**, en **clase**
(ninguno depende de `4723`: los dos son clase 3) y en **dependientes (1: el salto)**.
Decide el **LUID**, y la inserción de PRE va **siempre delante del salto**
(`insert_insn_end_bb`), o sea con el LUID **máximo** del bloque. Y ninguna
reordenación de fuente lo arregla: si se intercambian las dos sentencias, PRE
sigue insertando al final (medido: `d1` de la r24, objeto idéntico).

Además, **la otra salida (retrasar `addi r5`) es imposible por medida directa del
objetivo**: el objetivo emite `addi r5, r22, 0x1`, que lee **sólo r22** —un valor
vivo a la entrada del bloque y que nadie escribe en él—, así que **no puede tener
ninguna dependencia de datos dentro de bb 264** y siempre estará lista en el
ciclo 0. Una dependencia habría cambiado el propio opcode.

**Conclusión, y es una veda fuerte**: en el original, el `addi r23, r1, 0x10` de
bb 264 **no puede ser una inserción de PRE al final del bloque**; tiene que nacer
con LUID menor que el de `desiredPrecision = precision + 1`. Y `pre_insert_copies`
descarta la vía obvia: una computación real disponible en el bloque hace que PRE
inserte además un `mr` (visto en el volcado de `u4`: `mr r23, r11`, +8 B).

### 3.3. Ensayos numerados (arnés `c26bw_sw.py`, 2,2 s por variante)

Base: 99,98301 %, 5.180 B, 2 filas. Sobre las **49** formas de la r24+r25, **13 más**:

    x1  `va_arg(argList, unsigned int)` en `%z`         99,98301   2   idéntico  <- APLICADO
    x2  `char *p` primera declaración del bloque `%z`   99,98301   2   idéntico  <- APLICADO
    x3  x1 + x2                                         99,98301   2   idéntico
    u1  `__asm__("" : : "r"(cvtbuf))` entre las dos     99,40154  21   5.192 B
    u2  el mismo asm delante de `p -=`                  99,427025 23   5.192 B
    u3  el mismo asm detrás de `desiredPrecision`       99,430885 22   5.192 B
    u4  `char *base = cvtbuf` en bb264, `q = base+62`   99,39459  18   5.188 B
    u5  igual con `base` declarada la última            99,39459  18   5.188 B
    v1  `for (; precision > 0; precision--)`            99,4888   24   5.184 B
    v2  `while (precision != 0)`                        99,97529   4   5.180 B
    v3  `else { }` vacío tras el if grande              99,98301   2   idéntico
    v9  `desiredPrecision` en dos ramas iguales         99,98301   2   idéntico (cross-jump las funde)
    v10 `desiredPrecision` en dos ramas con `precision` 99,79073   4   5.188 B
    v11 dos ramas con las sentencias en distinto orden  98,90502  63   PEOR
    v12 `if (!(lowerVal == 0 && precision == 0))`       99,98301   2   idéntico

**Veda nueva (13 formas, total 62)**: barridas la barrera de registro sobre
`cvtbuf` (en las tres posiciones del bloque), la variable real `base` materializada
en bb 264 (con y sin reordenar su declaración), la forma `for`/`while !=` del bucle,
el `else` vacío, la negación de De Morgan del `if` grande y **las tres formas que
intentan hacer `precision + 1` parcialmente redundante para que PRE lo inserte
también** (`v9` la funde el cross-jump; `v10`/`v11` parten el bloque y cuestan 8 y
20 B).

### 3.4. Las dos correcciones de DWARF que sí entran (coste cero)

`dwbody.py zBWare "_bOutput"` sobre el bloque de `case 'z'`:

    -            char * p; // r3          <- PRIMERA declaracion en el original
                 unsigned int tempNumber; ...
    -                unsigned int * __ptr; // r9
    +                int * __ptr; // r9

1. **`char *p` es la primera declaración del bloque** y nosotros la teníamos la
   última.
2. **El `va_arg` del `%z` es `unsigned int`**: el DWARF del original da
   `unsigned int * __ptr` **aquí** e `int * __ptr` en GENERIC_INT — o sea **la
   corrección `n1` de la r24 iba al revés** y dejó este `va_arg` en `int`.
   Deshecha.

Las dos dan el **objeto idéntico** (99,98301 %, 5.180 B, 2 filas) y
`frozen.py chk Speed/Indep/SourceLists/zBWare` sigue diciendo **idéntico al
congelado**. Aplicadas por corrección estructural, no por bytes.

Anotado y **no** aplicado: el `int textLen` que la r24 metió para partir `size`
(37 filas → 2) **no existe en el DWARF del original** — allí el valor es un
temporal del compilador. Se queda porque vale 35 filas, pero es deuda declarada.
Y tres locales del original **no tienen registro** en el DWARF (`desiredPrecision`,
`offset`, `decimalChr`) donde nosotros sí; dos de ellas están en código que casa al
100 %, así que **ese indicio no es accionable** (perdí un rato con él).

---

## 4. Herramientas

- **`c26bw_rtl.py <target> [variante] [pases] [flags…]`** (scratchpad): como
  `c24bw_rtl.py` pero saca los cflags de `build.ninja` **de la unidad del target**
  (el de la r24 usaba siempre los de zMain), acepta flags extra
  (`-fsched-verbose-5`) y guarda stdout/stderr de `cc1plus` en `cc1.log`.
  **Importante**: con `-dS` el verbose de `sched1` va al `.sched` y el de `sched2`
  a `cc1.log`; mirar sólo uno de los dos engaña.
- **`c26bw_sw.py`** (`_bOutput`, 2,2 s) y **`c26bw_sp.py`** (`ESpawnFragment`, 6 s):
  extienden el catálogo de `c24bw_sw.py`. **Ninguno toca cabecera compartida.**
- **`c26bw_q4.py`**: la única sonda que toca `UVectorMathGC.hpp`. Corrida **en
  primer plano**, un punto, y `git status` verificado inmediatamente después
  (`p1`: `[3][3]` delante de `[2][2]` → 99,97782 %, 2 filas, y **los tres FPR ya
  casan**, lo que confirmó el modelo antes de buscar la sentencia en el `.cpp`).
  La cabecera quedó **limpia**; no ha hecho falta reconstruir zPhysics/zWorld.

---

## 5. Qué NO he probado

- **`_bOutput`**: nada que consiga meter `&cvtbuf[0]` en bb 264 con LUID bajo
  **sin** que PRE meta además el `mr` de `pre_insert_copies` (§3.2) — es lo único
  que queda y ahora está acotado con prueba; nada sobre las cuatro etiquetas de
  más (`OUTPUT`, `positioned`, `Z_INT`, `VECT_OUTPUT`), que el DWARF dice que el
  original no tiene pero ninguna está en este bloque; nada sobre `GENERIC_FLOAT`
  ni sobre el `__asm__("" : "+m"(number))`; **no he barrido banderas** (`-fgcse`
  por función no existe en GCC 2.9 y quitarlo global rompería la unidad entera).
- **`ESpawnFragment`**: no he buscado más el uso de `1.0f` del original (§2.5) —
  la función ya casa byte a byte, es sólo fidelidad de fuente; no he medido
  `GetSceneryTransform` ni `UpdateModel` porque **no toco `UVectorMathGC.hpp`**
  (el cambio vive en `ESpawnFragment.cpp`, que **sólo entra en zMain**:
  comprobado en `SourceLists/`); no he tocado `Bounds.h` (poner ahí
  `matrix.v3.w = 1.0f` afectaría a todos los llamantes de `Bounds::GetTransform`
  y además contradice el mapa de líneas).
- **`__12EPlayRaceNIS`** (2.176 B, 99,972 %): no la he tocado, cerrada como muro
  en la r23.

## 6. Para la próxima ronda

El mecanismo de §2.2 es **general y barato de comprobar**: cuando un racimo de
`stfs`/`stw` de constante sale rotado respecto al objetivo, mirar en el
`.regmove` si el store que se adelanta lleva **`REG_DEAD` de su fuente**. Si lo
lleva, la palanca no es reordenar la fuente del inline (eso rompe a los demás
llamantes): es **mantener viva la constante una sentencia más en el llamante**.
