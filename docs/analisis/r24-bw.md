# Ronda 24 — `_bOutput` (zBWare) y `_._14ESpawnFragment` (zMain)

## 0. Verificación del encargo

`build_direct.py zBWare zMain` + `triage.py --muro zBWare zMain` al empezar:

    zBWare+zMain  186028/195548 B  95.1316%  1616 funciones al 100%

    MURO: 3 funciones, 9520 B
        5180 B  99.840%  _bOutput__FP11bOutputInfoPCcP13__va_list_tag   37 registros
        2176 B  99.972%  __12EPlayRaceNISP7GMarkerPCcT2iiT2T2            3 registros
        2164 B  98.869%  _._14ESpawnFragment                            23 filas

**Coincide con el encargo.** `audit.py` al empezar: zBWare **238 ok / 0 FALLA**,
zMain **1378 ok / 0 FALLA**. Repetido al terminar: **238 / 0** y **1378 / 0**.

---

## 1. Resultado

| función | antes | después |
|---|---|---|
| `_bOutput` (zBWare, 5.180 B) | 99,84016 % · **37 filas** | **99,98301 % · 2 filas** |
| `_._14ESpawnFragment` (zMain, 2.164 B) | 98,86876 % · 23 filas | sin cambio (todo revertido) |

`measure.py --cmp` (base y medida con `build_direct.py` delante de cada una):
**+0 B, +0 funciones, 0 unidades cambian** — `matched_code` es todo-o-nada y
`_bOutput` sigue sin cerrar por 2 filas. Las 238 funciones ya cerradas de zBWare
siguen las 238.

Ficheros tocados: **sólo `src/Speed/Indep/bWare/Src/bPrintf.cpp`** (que sólo
entra en zBWare: comprobado, no es cabecera compartida). Sin commit.
`frozen.py cong Speed/Indep/SourceLists/zBWare` aplicado (huella
`02a909247b3bc739`).

---

## 2. `_bOutput`: 37 filas → 2. **`size` eran DOS variables**

### El diagnóstico de la r23, confirmado al insn — y dado la vuelta

La r23 sospechaba que el `'0'` que el objetivo pone en **r11** (el último de
`reg_alloc_order`) era un **registro de recarga de `reload`**, no un pseudo de
`local-alloc`. **Confirmado en el volcado, y es cierto**, pero no era la palanca:

- En el `.lreg` el `'0'` de `*p-- = '0'` es el **pseudo 945**, nacido de un izado
  de `loop.c` al preencabezado del `while` externo, con
  `REG_EQUIV (const_int 48)`.
- En el `;; Register dispositions` del `.greg` **945 no aparece**: no recibe
  registro. `reload` lo **rematerializa** en cada uso con un insn nuevo
  (numeración 7xxx), y en nuestra propia compilación cae en **r9, r0, r11, r0 y
  r9** en los cinco sitios. Es decir, r11 es una elección de `reload`, no de
  `local-alloc`, y **no hay forma de fuente que la mueva directamente**.
- Lo mismo vale para los `lis $LC@ha` de los racimos B y C: los
  `(set (reg:SI 8 r8) (high:SI (symbol_ref ("*$LC63"))))` son **insns 76xx**,
  o sea rematerializaciones de `reload`. **De las 37 filas, ~34 eran elecciones
  de `reload`.**

**La causalidad iba al revés**: `reload` evita el registro que ya ocupa un pseudo
vivo. En el objetivo `size` vive en **r9**, así que la recarga del `'0'` tiene que
irse a otro sitio; en el nuestro `size` vivía en **r10** y la recarga se llevaba
r9, y a partir de ahí toda la ronda de `reload` quedaba desfasada respecto al
objetivo. **Una sola decisión de `global_alloc` arrastraba los cuatro racimos.**

### Por qué `size` no podía coger r9

En el `.greg`, la lista de conflictos del pseudo 431 (`size`) termina en
**registros duros: `0 1 9 11`**. Esos conflictos con r9/r11 los ponen los pseudos
que `local-alloc` ya había colocado en r9/r11 dentro del rango de vida de `size`
— y son **las expansiones de `va_arg`** (`reg_save_area + gpr*4`, `gpr = gpr+1`),
que caen justo entre `size = 16/32/64` y el `64 - size`.

### La medida que lo decide, y está en el diff

Las asignaciones del **ancho en bits** (`li r10, 0x10` / `0x40` / `0x20` y
`subfic r30, r10, 0x40`, filas 313/361/389/440) **CASAN en r10**. La del
**conteo de bytes** (`subf r9, r0, r23`, fila 550) NO casaba: objetivo r9,
nuestro r10.

**Mismo nombre no puede estar en dos registros: el original tiene DOS
variables.** Nosotros reutilizábamos `int size` para las dos cosas, lo que unía
los dos rangos de vida en un solo pseudo y le pegaba los conflictos de r9/r11 de
los `va_arg`.

### El cambio aplicado

```cpp
int size;      // ancho en bits: 16/32/64   -> r10 (ya casaba)
int textLen;   // &cvtbuf[63] - p           -> r9  (NUEVO)
...
textLen = static_cast<int>(&cvtbuf[63] - p);
stringLength = textLen - 1;
if (flags & FL_FORCEOCTAL) {
    if (stringLength == 0 || *(p + 1) != '0') {
        *p-- = '0';
        stringLength = textLen;
    }
}
```

### Ensayos numerados (arnés `c24bw_sw.py`, **6 s** por variante)

    base                                     99,84016   37 filas
    c1  `int textLen` declarada con las demás 99,98301    2   <- APLICADO
    c2  `int textLen` declarada en el uso     99,98301    2   (empata con c1)
    c3  `size = 16/32/64` DESPUÉS del va_arg  99,26872   16   5.192 B  PEOR
    c4  cast en el va_arg unsigned            99,84016   37   idéntico

`c3` es la otra forma de quitarle a `size` los conflictos de los `va_arg`, y es
**peor**: al ser un único pseudo, quitarle el conflicto le da r9 **en los dos
rangos** y rompe las cuatro filas que ya casaban.

### El `va_arg` del DWARF, cerrado con medida

La r22 anotó `int * __ptr` en el original contra `unsigned int * __ptr` nuestro.
Hay **dos** `va_arg(argList, unsigned int)`:

    n3  el de GENERIC_INT a `int`     98,75135   24 filas   5.136 B   MUCHO PEOR
    n1  el del caso '%z' a `int`      99,98301    2   OBJETO IDÉNTICO  <- APLICADO
    n2  los dos a `int`               98,75135   24   5.136 B

O sea: el de GENERIC_INT **tiene** que ser `unsigned int` y el del `%z` **tiene**
que ser `int`. `n1` no mueve un byte pero cierra la discrepancia de DWARF, así
que se queda (regla de la r23: una corrección estructural real que no cierre se
queda si no baja nada).

---

## 3. Las 2 filas que quedan en `_bOutput`: `sched1`, y el modelo está medido

    913  addi r23, r1, 0x10   |  addi r5, r22, 0x1
    915  addi r5, r22, 0x1    |  addi r23, r1, 0x10

Bloque del caso `'%z'`, preencabezado del `while (precision > 0)`
(`bPrintf.cpp:1044-1045` del original):

    objetivo:  subi r0,r22,1 · addi r23,r1,0x10 · subf r3,r0,r3 · addi r5,r22,1
    nuestro :  subi r0,r22,1 · addi r5,r22,1    · subf r3,r0,r3 · addi r23,r1,0x10

**Quién lo decide, con volcado**: `.regmove` trae el orden de fuente
(4723 `subi`, 4725 `subf`, 4729 `addi r5`, 7303 `addi r23`) y `.sched` sale
(4723, 4729, 4725, 7303). O sea **lo decide `sched1`**; `flow2` y `sched2` ya no
lo tocan. `insn 7303` es la inserción de **PRE (`-fgcse`) de `&cvtbuf[0]`** al
final del bloque, delante del salto — sitio forzado, no movible desde la fuente.

**El modelo, sacado de otro bloque de la MISMA función** (bb 168, donde `sched1`
sí sube la inserción de PRE a la 2.ª ranura): el bloque emite 2 insns por ciclo;
en el ciclo 0 entra el `subi` y la segunda ranura se disputa entre las insns
listas. En bb 168 la competidora tenía una **antidependencia** con la ya emitida
y perdía la ranura, y la inserción de PRE la ganaba. Aquí `addi r5` no tiene
ninguna dependencia con `subi r0` (las dos sólo LEEN r22), entra lista y gana por
LUID. **Para casar haría falta que `addi r5` no estuviera lista en el ciclo 0**, y
no he encontrado ninguna forma de fuente que se lo impida.

### Ensayos numerados — **26 formas, todas idénticas o peores**

    base                                                99,98301    2 filas
    d1  `desiredPrecision` antes de `p -=`              99,98301    2   idéntico
    d2  `p = p - (precision - 1);`                      99,98301    2   idéntico
    d3  `desiredPrecision` fuera del `if`               99,8417     3   PEOR
    d4  `p -= precision - 1;` (sin paréntesis)          99,98301    2   idéntico
    d5  `p -= desiredPrecision - 2;`                    99,98301    2   idéntico
    e1  `char *p` primero del bloque '%z'               99,98301    2   idéntico
    e2  `char *p` primero del bloque GENERIC_FLOAT      99,98301    2   idéntico
    e3  e1+e2                                           99,98301    2   idéntico
    e4  `char *p = &cvtbuf[62];` con inicializador      99,98301    2   idéntico
    e5  sin la etiqueta `Z_INT` (no la salta nadie)     99,98301    2   idéntico
    e6  `stringLength = &cvtbuf[62] - p`                99,89343    5   5.176 B
    e7  `stringLength = &cvtbuf[63] - stringOut`        99,89343    5   5.176 B
    f1  `while` sin el bloque `{ }` interior            99,98301    2   idéntico
    f3  `stringLength` ANTES de `stringOut` (§3-bis)    99,98301    2   idéntico
    f5  f1+f3                                           99,98301    2   idéntico
    g1  `desiredPrecision = 1 + precision;`             99,98301    2   idéntico
    g2  `desiredPrecision = precision; ++`              99,98301    2   idéntico
    g3  `p = p - precision + 1;`                        99,816986   6   PEOR
    g4  `p -= precision; p++;`                          99,816986   6   PEOR
    g5  `int adj = precision-1; … adj + 2`              99,98301    2   idéntico (cse repliega)
    g6  `divisor = 0x10000u` tras el `if (precision<0)` 99,82471    5   PEOR
    g7  `if (precision<0)` antes de `upperVal`          99,23784   25   PEOR
    g8  `if (precision != 0 || lowerVal != 0)`          99,47954   14   5.176 B
    g9  `if (precision < 0)` sin llaves                 99,98301    2   idéntico
    h1  `p = cvtbuf + 62;`                              99,98301    2   idéntico
    h2  `p = &cvtbuf[62]` tras el `va_arg`              99,67413    6   PEOR
    i1  `if (precision>0) do { } while (precision>0);`  99,98301    2   idéntico
    i3  `precision--` al final del cuerpo               99,4888    24   5.184 B
    m1  `desiredPrecision` primera declaración          99,98301    2   idéntico
    m2  `unsigned int desiredPrecision`                 99,98301    2   idéntico
    m3  `p += 1 - precision;`                           99,96757    4   PEOR
    m4  `desiredPrecision` última declaración           99,98301    2   idéntico
    o1  `p = &cvtbuf[0] + 62;`                          99,98301    2   idéntico
    o2  `p = cvtbuf; p += 62;`                          99,55058   52   5.176 B
    o3  `p = &cvtbuf[63] - 1;`                          99,98301    2   idéntico
    o4  `stringLength = 62 - (p - cvtbuf)`              99,72433    8   5.176 B
    o5  `(&cvtbuf[63] - p) - 1` sin el paréntesis       99,98301    2   idéntico

**Veda**: barridas **31 formas** del par
`p -= (precision - 1);` / `desiredPrecision = precision + 1;`, de la forma del
bucle `while (precision > 0)`, del orden de declaración del bloque `'%z'` y de la
expresión de `stringLength` en la cola de `Z_INT`, incluidas las que intentan
**dejar `&cvtbuf[0]` disponible en el bloque para que PRE no inserte nada**
(`o1`, `o3`: idénticas — GCC vuelve a plegar la dirección). **Ninguna mueve la
ranura de `sched1`**, y `d1`/`i1` demuestran que el orden de fuente de esas dos sentencias
y la forma `while`/`do-while` **no cambian el objeto** (GCC canonicaliza).

### 3-bis. Lo que sí dice el mapa de líneas y NO era accionable

`lmap.py` da, en la cola del caso `'%z'`:

    8005F648  subi r9, r3, 0x3f     bPrintf.cpp:1151     stringLength
    8005F64C  addi r27, r3, 0x1     bPrintf.cpp:1152     stringOut
    8005F650  subf r9, r9, r23      bPrintf.cpp:1151

O sea **el original escribe `stringLength` ANTES que `stringOut`** y nosotros al
revés. Corregido y medido (`f3`): **objeto idéntico**. No lo he dejado puesto
porque no paga; queda anotado para una limpieza de DWARF.

---

## 4. `_._14ESpawnFragment` — sigue en 23 filas, y hay dos frentes más cerrados

### Lo que confirmo de la r23

- El racimo es la rotación de tres FPR: **objetivo** `0.0→f3`, `1/32767→f2`,
  `1.0→f1`; **nuestro** `1.0→f3`, `0.0→f2`, `1/32767→f1`. Verificado en el
  `.regmove`/`.sched`: los pseudos son **203** (`1/32767`, `$LC241`), **321**
  (`1.0`, `$LC243`) y **363** (`0.0`, `$LC244`).
- La causa está localizada al insn: en `.regmove` los 16 `stfs` de `quattom4`
  van en orden de fuente y el `ST 156 <- p321` (`result[3][3] = 1.0f`) es **el
  último**; `sched1` lo sube a la **4.ª posición del bloque**, la `1.0` muere
  antes y `local-alloc` rota los tres FPR. El orden final de los siete `stfs` de
  constante es una **rotación pura**:

      objetivo: [3][0] [3][1] [3][2] [0][3] [1][3] [2][3] [3][3]
      nuestro : [3][3] [3][0] [3][1] [3][2] [0][3] [1][3] [2][3]

- `dwbody.py` sobre `ESpawnFragment::~ESpawnFragment`: el cuerpo casa **entero**
  salvo dos NOMBRES de local (`iboundable_frag`/`iboundable_parent` contra
  `ib_fragment`/`ib_owner`). **No falta ni sobra ninguna sentencia.**

### Frentes nuevos, todos cerrados con medida

    s1  GetTransform con fOrientation/fPosition.Decompress directos   23 filas
    s2  GetTransform con `UMath::Vector3 &pos` intermedia             23
    s3  GetTransform con `reinterpret_cast<Vector3&>(matrix.v3)`      23
    s4  GetTransform con `*UMath::Vector4To3(&matrix.v3)`             23
    s5  `_V3c::Decompress` con recíproco en local                     23
    s6  `_V3c::Decompress` con `this->`                               23
    s8  `bounds->GetTransform` antes de `model->GetTransform`         92,589645  141 filas
    k1  matrices `m_final, m_outer, m_inner`                          98,83549    40
    k2  matrices `m_outer, m_final, m_inner`                          98,84288    36
    k3  matrices `m_final, m_inner, m_outer`                          98,85952    28
    k4  matrices `m_inner, m_final, m_outer`                          98,83549    40
    k5  las tres en una sola declaración                              98,86876    23  idéntico

**Veda nueva**: `_V3c::Decompress`, `UMath::Vector4To3` y las cuatro formas de
`Bounds::GetTransform` **no tocan ni una fila** — la asimetría que la r23 señalaba
(`GetPosition(Vector4To3(matrix.v3))` escribiendo después en `matrix.v3`) **está
descartada como palanca**. Y el orden de declaración de las tres matrices que
tenemos (`m_outer, m_inner, m_final`) es **el mejor de las seis permutaciones**.

### La búsqueda conjunta en `quattom4`: primer tramo hecho, sin 100 %

La r23 encontró un 100 % moviendo **dos** sentencias de `VU0_quattom4`
(`result[3][3]` y `result[2][3]`) y midió que costaba **−12.808 B y ocho
funciones**. Falta saber si existe un `quattom4` que cierre las **nueve**.

Barrido nuevo (`c24bw_q4sw.py`, 6 s por posición) de **una sola** sentencia sobre
la base natural: **`result[2][3] = 0.0f;` en las 25 posiciones →
98,86691 - 98,87061 %, siempre 23 filas, ningún 100 %.** Con el barrido de
`result[3][3]` de la r23 (que tampoco da 100 % desde la base natural: lo mejor
son 2 filas en las posiciones 17-19), queda medido que **ninguna de las dos
sentencias por separado cierra**: hace falta el par, que es justo lo que rompe a
las otras ocho.

Arnés preparado y **verificado** para la búsqueda conjunta:
`c24bw_q4joint.py check` mide de una pasada `ESpawnFragment` **y siete de las
ocho** funciones que se rompen —

    ESpawnFragment=98.86876/23  RecalcOrientMat=100/0  DoRBCollisions=100/0
    DoSRBCollisions=100/0  DoIntegration=100/0  SetCollision=100/0  AddNode=100/0

84 s por candidato. **`GetSceneryTransform` (SmokeableInfo.cpp) y `UpdateModel`
(VehicleFragmentConn.cpp) no compilan como stub suelto** (les faltan
declaraciones que da su SourceList); esas dos hay que medirlas con
`build_direct.py zPhysics zWorld` + `pctsnap.py`.

---

## 5. Herramientas

- **`c24bw_sw.py`** (scratchpad): arnés multi-parche, compila el `.cpp` suelto
  con los cflags de su unidad, diffea y **restaura siempre**. **6 s** por
  variante para `bPrintf.cpp` y para `ESpawnFragment.cpp`. Verificado que el stub
  de `bPrintf.cpp` reproduce el `.o` real **fila a fila** (99,84016 % / 5.180 B
  en los dos lados) antes de fiarme.
- **`c24bw_rtl.py <target> <variante> <pases>`**: volcados RTL por pase con la
  receta de la r22 (preprocesar a `.ii` y llamar a `cc1plus` a mano). Acepta la
  lista de letras de `-d`, así que se piden **sólo los pases que hacen falta**
  (`lg` = `.lreg`+`.greg`, 1,6 MB; `SN` = `.sched`+`.regmove`) en vez de los 16
  con el `.rtl` de 45 MB.
- **`c24bw_q4sw.py`** / **`c24bw_q4joint.py`**: barren la POSICIÓN de una
  sentencia de `VU0_quattom4`; el segundo mide de una pasada los siete llamantes
  que reproducen como stub.

**Aviso**: `c24bw_q4sw.py` y `c24bw_q4joint.py` **modifican `UVectorMathGC.hpp`,
que es cabecera compartida**, durante ~6 s por punto (y la restauran siempre).
Con agentes en paralelo eso les puede envenenar una compilación: no lanzarlos sin
avisar, y nunca el barrido de las 16 sentencias (40 min con el árbol tocado).

---

## 6. Qué NO he probado

- **`_bOutput`**: nada que cambie el sitio donde **PRE** inserta
  `addi r23, r1, 0x10` (la única palanca que queda para las 2 filas, y en `.sched`
  su posición está forzada al final del bloque); nada sobre las etiquetas
  `OUTPUT`, `positioned` y `VECT_OUTPUT` que el DWARF dice que el original no
  tiene (quitarlas es reestructurar el `switch` de 5 kB, y ninguna de las tres
  está en el bloque de las 2 filas); no he tocado el bloque `GENERIC_FLOAT` ni el
  `__asm__("" : "+m"(number))` de la r22.
- **`ESpawnFragment`**: la **búsqueda conjunta** de un orden de `quattom4` que
  satisfaga a la vez a las nueve funciones — el arnés está hecho y verificado
  (`c24bw_q4joint.py`), faltan los dos stubs que no compilan y las ~625 parejas;
  nada sobre `VU0_MATRIX4_mult` (el intento `s7` de llamarlo directo no compila:
  no está en el espacio de nombres `UMath`); nada sobre `Matrix4::operator[]` /
  `Vector4::operator[]`; ninguna variante del `SmackableParams` ni del
  `WorldConn::Pkt_Body_Send`.
- **`__12EPlayRaceNIS`** (2.176 B, 99,972 %): no la he tocado, cerrada como muro
  en la r23 con catorce formas medidas y el orden del pool como prueba.
