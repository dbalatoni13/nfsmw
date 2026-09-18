# Ronda 25 — `_bOutput` (zBWare) y `_._14ESpawnFragment` (zMain)

## 0. Verificación del encargo

`build_direct.py zBWare zMain` + `triage.py --muro zBWare zMain`:

    MURO: 3 funciones, 9520 B
        5180 B   99.983%  zBWare  _bOutput__FP11bOutputInfoPCcP13__va_list_t   2 registros
        2176 B   99.972%  zMain   __12EPlayRaceNISP7GMarkerPCcT2iiT2T2         3 registros
        2164 B   98.869%  zMain   _._14ESpawnFragment                          23 filas

**Coincide con el encargo.** `audit.py` al empezar (una pasada, tras
`build_direct.py`): zBWare **238 ok / 0 FALLA**, zMain **1378 ok / 0 FALLA**.

`_bOutput` está **commiteado** en el estado de la r24 (`61a6d01c`); el árbol
estaba limpio en `bPrintf.cpp`, `ESpawnFragment.cpp` y `UVectorMathGC.hpp`.

---

## 1. Resultado

**+0 B, +0 funciones.** Ninguna variante cierra. Todo revertido; el árbol queda
como estaba. Sin commit.

Lo que sí sale de la ronda:

- **El modelo de `_bOutput` está CORREGIDO y probado con una medida**: la r24
  decía que la 2.ª ranura la decide una *antidependencia*; **no es eso** (§2.1),
  y el contraejemplo que usó (bb 168) no demuestra lo que decía. Lo que decide
  es el **LUID**, y hay una medida que lo prueba: materializando `&cvtbuf[0]`
  con LUID bajo dentro del bloque, el `addi` **cae en la fila correcta**.
- **18 formas nuevas medidas** en `_bOutput` (§2.3), ninguna cierra.
- **`ESpawnFragment`: la búsqueda conjunta queda acotada con medida** (§3): las
  **25 posiciones** de `result[3][3]` **rompen las tres funciones de
  `SimpleRigidBody.cpp`**, la mejor deja `RecalcOrientMat` en 85,67 % (100 % en
  la base natural).

---

## 2. `_bOutput` — las 2 filas, con el pase y el criterio al dígito

    913  addi r23, r1, 0x10   |  addi r5, r22, 0x1
    915  addi r5, r22, 0x1    |  addi r23, r1, 0x10

Bloque **bb 264** (el preencabezado del `while (precision > 0)` del caso `%z`),
cuatro insns más el salto:

    objetivo:  subi r0,r22,1 · addi r23,r1,0x10 · subf r3,r0,r3 · addi r5,r22,1
    nuestro :  subi r0,r22,1 · addi r5,r22,1    · subf r3,r0,r3 · addi r23,r1,0x10

### 2.1. Lo que decide, con el volcado delante (y la corrección a la r24)

Del `.sched` con `-fsched-verbose` (fichero `c24bw_sched_bout.txt` de la r24,
bloque 264):

    ;;  Ready list (t =  1):    7303  4729  4723
    ;;  clock 1:  4723 r1394=r90-0x1        4729 r1303=r90+0x1
    ;;  clock 2:  4725 r1305=r1305-r1394    7303 r2016=%31+0x10

- **4723** = `precision - 1`, **4725** = `p -= …` (depende de 4723),
  **4729** = `desiredPrecision = precision + 1`, **7303** = la inserción de
  **PRE** de `&cvtbuf[0]`.
- La lista sale ordenada **ascendente** y el planificador coge el ÚLTIMO. 4723
  gana por **prioridad 2** (4725 depende de él). Entre 4729 y 7303 hay empate en
  **prioridad** (los dos 1: su único dependiente es el salto, con
  antidependencia de coste 0), empate en **`INSN_REG_WEIGHT`** (los dos +1: un
  `set` y ninguna nota `REG_DEAD`/`REG_UNUSED` — comprobado en el RTL) y empate
  en número de dependientes. **Desempata el `INSN_LUID`**, y el orden previo a
  `sched1` (volcado `.regmove`) es

      4723 · 4725 · 4729 · 7303

  o sea la inserción de PRE es **la última del bloque**, porque
  `insert_insn_end_bb` la mete **delante del salto**. Por eso pierde.

**Corrección a la r24**: decía que el modelo salía de bb 168, «donde la
competidora tenía antidependencia y perdía la ranura». Mirado el volcado, en
bb 168 la lista de listos del ciclo 0 es `7238 6799` — **sólo dos insns**: la
inserción de PRE se lleva la 2.ª ranura porque **no hay competidora**, no por
ninguna antidependencia. El criterio de clase respecto a `last_scheduled_insn`
**no interviene** en ninguno de los dos bloques.

### 2.2. La medida que prueba el modelo

Si el modelo es cierto, basta con que `&cvtbuf[0]` nazca **dentro** del bloque
con LUID bajo (entre `p -=` y `desiredPrecision =`) para que se lleve la ranura.
Ensayo `r1` (diagnóstico, no aplicable: deja `base` sin inicializar en el camino
`lowerVal==0 && precision==0`):

```cpp
if (lowerVal != 0 || precision != 0) {
    p -= (precision - 1);
    base = cvtbuf;                 /* &cvtbuf[0] con LUID bajo */
    desiredPrecision = precision + 1;
    ...
Z_INT:
stringLength = static_cast<int>((base + 63 - p) - 1);
```

    r1  99,71274 %  18 filas  5.184 B
        fila 913:  addi r23,r1,0x10  |  addi r11,r1,0x10   <- MISMA RANURA
        fila 910:  addi r23,r1,0x10  |  (DELETE)

**El `addi` de `cvtbuf` cae en la fila 913, la del objetivo**: la permutación de
las dos filas **desaparece**. Lo que se rompe es lo demás: al usar la *variable*
en vez de la expresión, PRE ya no inserta nada en bb 263 (se pierde la fila 910)
y `base` vive todo el bucle, así que cae en r11 y se derrama (`stw r11,0xb8(r1)`).

**Conclusión**: en el original, el `addi r23,r1,0x10` de bb 264 **no puede ser
la inserción de PRE al final del bloque** — tiene que nacer con LUID menor que
el de `desiredPrecision = precision + 1`. La forma de fuente que lo consigue sin
coste extra (y dejando que PRE siga insertando en bb 263, que es la fila 910)
**no la he encontrado**.

### 2.3. Ensayos numerados (arnés `c25bw_sw.py`, **2,2 s** por variante)

Base: 99,98301 %, 5.180 B, **2 filas**.

    q1  `if (lowerVal != 0 || precision > 0)`             99,97915    3   PEOR
    q2  `int desiredPrecision =` declarada en el uso      99,98301    2   idéntico
    q3  llaves de más alrededor de las dos sentencias     99,98301    2   idéntico
    q4  las dos en una sentencia con coma                 99,98301    2   idéntico
    q5  `for (; precision > 0;)` en vez de `while`        99,98301    2   idéntico
    q6  declaración muerta entre el `{` y `p -=`          99,98301    2   idéntico
    q7  `char *pBase = cvtbuf;` + no-op con pBase         99,52355   17   5.188 B
    q8  `lowerVal` antes de `upperVal`                    99,828575   4   PEOR
    q9  `divisor` tras el `if (precision < 0)`            99,82471    5   PEOR
    q10 `int back = precision - 1; p -= back;`            99,98301    2   idéntico
    q11 `desiredPrecision` en las dos ramas de un if      99,98301    2   idéntico
    q12 `stringLength = &cvtbuf[62] - p`                  99,89343    5   5.176 B
    q13 `p = &cvtbuf[62]` justo antes del if grande       99,67027    7   PEOR
    q14 el `while` de ceros sin el `if` externo           99,98301    2   idéntico
    t1  `desiredPrecision = back + 2` (back=precision-1)  99,98301    2   idéntico (cse repliega)
    t2  `desiredPrecision = (&cvtbuf[63]-p) + 1`          99,35444   23   5.196 B
    r1  `base = cvtbuf` en bb264 + Z_INT con `base`       99,71274   18   5.184 B  (diagnóstico)
    r2  `base = cvtbuf` al principio del case             99,71274   14   5.176 B  (diagnóstico)

**Veda nueva** (18 formas, sobre las 31 de la r24 → **49**): barridas la forma
del `if (lowerVal != 0 || precision != 0)`, el sitio de `divisor`, el orden de
`upperVal`/`lowerVal`, el sitio de `p = &cvtbuf[62]`, la declaración de
`desiredPrecision` (dentro/fuera, con y sin inicializador), llaves y comas entre
las dos sentencias, `for` contra `while`, el `if` interior del bucle de ceros y
**dos formas que hacen depender `desiredPrecision` del temporal o de `p`**
(`t1` la repliega `cse`, `t2` cuesta 16 B).

**`q10` merece una nota**: `int back = precision - 1; p -= back;` da el objeto
**idéntico**, así que el temporal explícito no cambia el LUID. Y `t1` demuestra
que `cse` repliega `(precision-1)+2` a `precision+1`, o sea **no se puede crear
una dependencia de datos entre `desiredPrecision` y el temporal** manteniendo el
`addi r5,r22,1` del objetivo.

### 2.4. Lo que queda vivo en `_bOutput` (y por qué no lo he cerrado)

Sabemos **exactamente** qué hace falta: que `&cvtbuf[0]` nazca dentro de bb 264
con LUID menor que el de `desiredPrecision = precision + 1`, **y** que PRE siga
insertando la copia al final de bb 263 (la fila 910, que ya casa). Eso pide una
sentencia de fuente en el cuerpo del `if` que:

1. compile a **exactamente un `addi rX, r1, 0x10`** y nada más,
2. esté **entre** `p -= (precision - 1);` y `desiredPrecision = precision + 1;`,
3. y cuyo valor lo consuma la **expresión** `&cvtbuf[63] - p` de `Z_INT`, no una
   variable — si es una variable, PRE deja de insertar en bb 263 (medido en
   `r1`: la fila 910 desaparece).

Las tres a la vez no las he sabido escribir. La 3 es la que aprieta: en cuanto
la variable existe, `cse` la usa en `Z_INT` y la expresión deja de ser
parcialmente redundante.

**El mapa de líneas no lo decide** (comprobado): en el objetivo `addi r23` y
`addi r5` llevan **las dos la línea 1045**, pero eso sale igual con PRE al final
del bloque (la inserción hereda la última nota de línea, que es la de
`desiredPrecision`) que con una sentencia de fuente. **No sirve para
distinguir.** Lo que sí dice el mapa: el original usa **llaves en línea aparte**
(el `if` en 1034 y `precision = 2` en 1036 obligan a un `{` en 1035; lo mismo en
1050/1052), así que el corrimiento de líneas de esta zona es **formato, no
sentencias que falten**.

---

## 3. `_._14ESpawnFragment` — la búsqueda conjunta, acotada con medida

### 3.1. Cómo la he podado

Barrer las ~625 parejas midiendo las nueve funciones cuesta ~14 h. La poda:

- **`SimpleRigidBody.cpp` da TRES llamantes en una sola compilación**
  (`RecalcOrientMat`, `DoRBCollisions`, `DoSRBCollisions`), y `RecalcOrientMat`
  (228 B) es **el más sensible que se ha medido** (100 -> 68,8 % con la pareja de
  la r23). Es el oráculo barato: **una pareja sólo sirve si él sigue al 100 %.**
- Arnés `c25bw_q4pair.py`, ~45 s por punto con `ESpawnFragment` y ~25 s sin él.
  Control natural verificado antes de fiarme:

      BASE-NATURAL  ESpawnFragment=98.86876/23  RecalcOrientMat=100/0
                    DoRBCollisions=100/0  DoSRBCollisions=100/0
                    DoIntegration=100/0  SetCollision=100/0  AddNode=100/0

### 3.2. Las 25 posiciones de `result[3][3] = 1.0f;`, ahora con el oráculo

| posición de `[3][3]` | ESpawnFragment | RecalcOrientMat | DoRB | DoSRB |
|---|---|---|---|---|
| 01 (tras `scale`) | 99,89094 / 13 | **85,66666** | 95,72 | 95,17 |
| 02-09 | 99,89094 / 13 | **43-62 %** | 92-95 | 89-93 |
| 10-13 | 99,89094 / 13 | **84,70175** | 99,08 | 99,24 |
| 14-17 | 99,97597 / **3** | **68,80701** | 99,08 | 99,24 |
| 18-20 | 99,97782 / **2** | **68,80701** | 99,08 | 99,24 |
| **21-25** (tras `result[2][2]`) | 98,86876 / 23 | **100** | **100** | **100** |

**El corte es exacto y está en `result[2][2] = 1.0f - (xx + yy);`.** Mover
`result[3][3]` a cualquier sitio **por detrás** de esa sentencia es
**indistinguible del natural** para las cuatro funciones; moverlo **por delante**
mejora `ESpawnFragment` y **rompe las tres de `SimpleRigidBody.cpp`**, en el
mejor caso dejando `RecalcOrientMat` en 85,67 %.

### 3.3. La segunda sentencia NO repara la primera — dos bases barridas

`result[2][3] = 0.0f;` sobre la mejor base para `ESpawnFragment`
(`result[3][3]`@18, la de 2 filas), 24 posiciones, midiendo `SimpleRigidBody`:

    posiciones 01-13   RecalcOrientMat  41,56 - 68,81 %       (peor o igual)
    posiciones 14-24   RecalcOrientMat  68,80701 / 68,82456   (la de la base)

Y sobre `result[3][3]`@10 (la base con el oráculo más alto, 84,70 %),
posiciones 13-24:

    posicion  13       RecalcOrientMat  65,14 %               (peor)
    posiciones 14-24   RecalcOrientMat  84,70 / 84,72 %       (la de la base)

**En 36 parejas medidas sobre las dos bases que pueden dar el 100 % en
`ESpawnFragment`, `RecalcOrientMat` no vuelve al 100 % ni una vez.** El
porcentaje del oráculo lo fija **la posición de `result[3][3]`**; la de
`result[2][3]` sólo puede bajarlo.

### 3.4. Veda

**La búsqueda conjunta de `result[3][3]` + `result[2][3]` queda cerrada con
medida.** Las dos condiciones son incompatibles:

- para que `ESpawnFragment` se mueva, `result[3][3]` tiene que ir **delante** de
  `result[2][2]`;
- delante de `result[2][2]`, `RecalcOrientMat` cae a <= 85,67 % y **ninguna
  posición de `result[2][3]` lo repara**.

Sumado a lo que ya estaba medido (el orden natural deja las **ocho** al 100 %),
la conclusión de la r23 se refuerza: **el `VU0_quattom4` del original ES el
natural**, y la diferencia de `ESpawnFragment` está **aguas arriba, en su propio
bloque**.

---

## 4. Herramientas y convivencia

- **`c25bw_sw.py`** (scratchpad): extiende el catálogo de `c24bw_sw.py` con las
  18 variantes nuevas de `_bOutput`. **2,2 s** por variante, restaura siempre.
- **`c25bw_q4pair.py`**: barrido de posiciones en `VU0_quattom4` midiendo
  `ESpawnFragment` **y** los tres llamantes de `SimpleRigidBody.cpp` de una
  pasada (`FULL=1` añade `RigidBody.cpp` y `Bounds.cpp`; `SKIPMAIN=1` quita
  `zMain`). Acepta `--pos=a-b` para trocear.
- **AVISO DE CONVIVENCIA, con incidente**: este arnés **modifica
  `UVectorMathGC.hpp`, que es cabecera compartida**. Lanzado en segundo plano,
  el proceso **murió a media sin ejecutar su `finally`** y **dejó la cabecera
  tocada**. Se detectó con `git status` y se restauró en el acto. Corregido:
  ahora el arnés **restaura la cabecera en CADA punto**, no sólo al final, y se
  corre **en primer plano** en tramos que caben en el tiempo de espera. Al
  terminar se reconstruyeron `zPhysics`, `zPhysicsBehaviors`, `zWorld` y
  `zWorld2` por si algún agente compiló con la cabecera tocada.
  **Regla nueva: un barrido que toque una cabecera compartida NO se lanza en
  segundo plano.**

## 5. Cierre

`build_direct.py zBWare zMain` + `triage.py --muro` al terminar: **los mismos
tres muros y los mismos porcentajes** que al empezar. `audit.py` segunda pasada:
zBWare **238 ok / 0 FALLA**, zMain **1378 ok / 0 FALLA**.
`frozen.py chk Speed/Indep/SourceLists/zBWare`: **idéntico al congelado**.
Árbol limpio en `bPrintf.cpp`, `ESpawnFragment.cpp`, `UVectorMathGC.hpp`,
`Physics/` y `VehicleFragmentConn.cpp`. Sin commit.

## 6. Qué NO he probado

- **`_bOutput`**: ninguna forma que meta en bb 264 una sentencia que compile a
  un solo `addi r1+0x10` y cuyo valor consuma la **expresión** de `Z_INT` (§2.4)
  — es lo único que queda, y no he sabido escribirla; nada sobre las cuatro
  etiquetas de más (`OUTPUT`, `positioned`, `Z_INT`, `VECT_OUTPUT`), que la r24
  ya dejó fuera porque ninguna está en este bloque; nada sobre `GENERIC_FLOAT`
  ni sobre el `__asm__("" : "+m"(number))` (medido dos veces que hace falta);
  **no he vuelto a volcar el RTL**: los volcados de la r24
  (`c24bw_sched_bout.txt`, `c24bw_regmove_bout.txt`) ya traían
  `-fsched-verbose`, y son los que dan las listas de listos de §2.1.
- **`ESpawnFragment`**: **el barrido de las 16 sentencias** de `VU0_quattom4`
  (parejas que no sean `[3][3]`/`[2][3]`) — con la cabecera compartida y 45 s
  por punto son horas de árbol tocado, y el corte de §3.2 dice que cualquier
  sentencia movida por delante de `result[2][2]` va a romper lo mismo; no he
  medido `GetSceneryTransform` (`SmokeableInfo.cpp`) ni `UpdateModel`
  (`VehicleFragmentConn.cpp`), que **siguen sin compilar como stub suelto**
  (haría falta `build_direct.py zPhysics zWorld` + `pctsnap.py`), pero no hacía
  falta: el oráculo ya cierra la búsqueda; nada aguas arriba en el propio bloque
  de `ESpawnFragment` más allá de lo que la r23 y la r24 ya vedaron
  (`_V3c::Decompress`, `UMath::Vector4To3`, cuatro formas de
  `Bounds::GetTransform`, las seis permutaciones del orden de las matrices).
- **`__12EPlayRaceNIS`** (2.176 B, 99,972 %): no la he tocado, cerrada como muro
  en la r23 con catorce formas medidas y el orden del pool como prueba.
