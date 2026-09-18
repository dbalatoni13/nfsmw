# Ronda 28 — `_bOutput` (zBWare), `SphereVsBox` (zDynamics) y `EPlayRaceNIS` (zMain)

## 0. Verificación del encargo

`build_direct.py zBWare zDynamics zMain` + `triage.py --muro`:

    MURO: 3 funciones, 9252 B
        5180 B   99.983%  zBWare     _bOutput__FP11bOutputInfoPCcP13__va_list_tag   2 registros
        2176 B   99.972%  zMain      __12EPlayRaceNISP7GMarkerPCcT2iiT2T2           3 registros
        1896 B   99.479%  zDynamics  SphereVsBox__...                               reorden local

**Reproduce el encargo.** `measure.py` de partida: 215.272/224.524 B, 95,8793 %,
1.657 funciones al 100 %.

---

## 1. Resultado: DOS de las tres cerradas

| función | antes | después |
|---|---|---|
| **`Geometry::SphereVsBox`** (zDynamics, 1.896 B) | 99,47890 % · 8 filas | **100 % · 0 filas** |
| **`EPlayRaceNIS::EPlayRaceNIS`** (zMain, 2.176 B) | 99,97243 % · 3 filas | **100 % · 0 filas** |
| `_bOutput` (zBWare, 5.180 B) | 99,98301 % · 2 filas | sin cambio — **techo confirmado** |

`measure.py --cmp` (base y medida con `build_direct.py` delante de cada una):
**+4.072 B, +2 funciones, 2 unidades**.

    +1896 B  +1 fn  Speed/Indep/SourceLists/zDynamics   27080 -> 28976
    +2176 B  +1 fn  Speed/Indep/SourceLists/zMain      157600 -> 159776

**`zDynamics` queda al 100 % (28.976/28.976 B) y `zMain` también
(159.776/159.776 B, 1.380 funciones).** `zBWare` sigue con `_bOutput` como único
muro.

`audit.py` **dos pasadas** al cerrar, sin un solo FALLA:
**zBWare 238/238 · zDynamics 41/41 · zMain 1380/1380.**
`frozen.py`: zDynamics `1b7d1926c09b5b15` y zMain `50639e1a62b76ced` congelados;
zBWare **idéntico al congelado** (no lo he tocado).

**Ficheros míos, dos:** `src/Speed/Indep/Src/Physics/Dynamics/Geometry.cpp` y
`src/Speed/Indep/Src/Generated/Events/EPlayRaceNIS.cpp`. Los otros 16 de
`git status` son de agentes de esta misma ronda. **Sin commit.**

### Comparación de secciones (regla 4 del brief)

| | `.text` objetivo | `.text` nuestro | resto |
|---|---|---|---|
| zDynamics | 28.976 | **28.976 (exacta)** | `.rodata` 544 vs 840 (**+296 B**) |
| zMain | 159.776 | 179.964 (**+20.188 B**) | `.rodata` +1.600, `.bss` +660, `.data` −160 |

**`trypromo.py` dice `DOL ROTO` en las dos** (`zDynamics 9947de7538f1`,
`zMain 8d2f56e5eee2`): el 100 % de código **no basta** para promocionarlas, y el
bloqueo es el frente §1 del brief. En zDynamics es pequeño y está identificado:
**286 B de `$LC*`** (cadenas: `$LC220` 24 B, `$LC219` 12 B, `$LC162` 10 B, cuatro
de 8 B y once de 4 B) más `kFloatScaleUp/Down` que salen renombrados
(`kFloatScaleUp_8045B100`) y un `gap_06_804170CC_data` de 40 B que nos falta.
**zDynamics está a 286 B de ser promocionable**; es el candidato más barato del
frente §1 que he visto.

---

## 2. EL HALLAZGO DE LA RONDA: la barrera SELECTIVA

**`__asm__("")` es una barrera TOTAL; un asm extendido NO VOLÁTIL no lo es.**

En GCC 2.95 `sched_analyze_2`, el caso `ASM_OPERANDS / ASM_INPUT` toma el camino
de «clobber all hard registers, all pseudo-registers and all of memory» —
`reg_pending_sets_all = 1`— **cuando el patrón es `ASM_INPUT` (asm sin operandos,
«tradicional») o cuando `MEM_VOLATILE_P` está puesto**. O sea:

- `__asm__("")` → sin operandos → `ASM_INPUT` → **barrera total**.
- `__asm__ __volatile__(...)` con operandos → volátil → **barrera total**.
- **`__asm__(... : "=..."/"+..." ...)` sin `volatile` → NO entra por ahí**: sólo
  crea las dependencias de lo que NOMBRA. Un `"memory"` en la lista de clobbers
  sigue valiendo (es un `(clobber (mem:BLK))` y `sched_analyze_1` lo trata como
  escritura de memoria), pero **sólo contra instrucciones de memoria**.

Verificado con los volcados (`-dSRgrj -fsched-verbose-5`): con
`__asm__ __volatile__("" : : "r"(a_lp) : "memory")` la insn del asm sale con
dependientes `458 457 452 2005 2002` (todo el bloque); sin `volatile`, no.

**Eso da una palanca nueva: retrasar UNA instrucción sin retrasar a sus vecinas.**
Es lo que cierra `SphereVsBox`, y es reutilizable en cualquier función donde el
diagnóstico sea «esta insn se adelanta y no debería».

**Corolario para el catálogo de vedas**: todas las vedas anteriores del proyecto
que dicen «probada la barrera en la posición X» se refieren a la barrera TOTAL.
**El eje de la barrera selectiva está sin barrer en todo el árbol.**

---

## 3. `SphereVsBox`: cerrada (1.896 B, `.text` exacta)

### 3.1. El cambio

    -        a_lp++;
    -        __asm__("");
    +        __asm__("" : "+r"(a_lp) : : "memory");
    +        a_lp++;
         } while (a_lp <= 2);

### 3.2. Por qué, con el volcado delante

El bloque básico 7 (el final del cuerpo del bucle) tiene **doce** instrucciones.
`sched2`, dependencias literales del volcado:

    insn  prio  dependientes
    419    12   458 452 420     fsubs (dists[a_lp] - radius)
    420    11   458 452 422     fsubs
    422    10   ...             stfsx penetration[a_lp]
    442     8   ...             lfsx  prev_dists[a_lp]
    443     6   ...             fsubs
    444     5   ...             fsubs
    446     4   458 452         stfsx prev_penetration[a_lp]
    450     4   458 457 452     addi r31,r31,1   <- a_lp++
    2002    2   458             addi r22,r1,0x68   <- insercion de PRE
    2005    2   458             addi r19,r1,0x78   <- insercion de PRE
    457     2   458             cmpwi
    458     1   —               ble

El objetivo pone **2002 y 2005 en las ranuras de relleno de la cadena FP**
(ciclos 1 y 2) y `a_lp++` **en el ciclo 10, exactamente un ciclo después del
último store (446)**. Es decir: en el objetivo `a_lp++` **depende de 446**.

Por prioridad no puede salir nunca: `a_lp++` vale 4 (llega al `ble` por el
`cmpwi`) y las inserciones 2, así que `sched2` adelanta siempre el incremento.
Y **no vale ninguna posición de barrera total**, porque gcse/PRE inserta
**siempre al final del bloque** (`insert_insn_end_bb`, delante del salto): toda
barrera que retrase a `a_lp++` retrasa también a las dos inserciones.

La barrera selectiva rompe justo eso:
- el clobber `"memory"` la ata al store 446;
- el operando `"+r"(a_lp)` hace que `a_lp++` dependa de ella;
- los dos `addi` de PRE **sólo leen r1**, así que no la ven.

Orden emitido: `fsubs, PRE1, fsubs, PRE2, stfsx, lfsx, fsubs, fsubs, stfsx,
a_lp++, cmpwi, ble` — **el del objetivo, byte a byte**, y el asm no emite nada.

### 3.3. Deuda declarada (regla 3 del brief)

**El original no tenía un `asm`.** Lo que hace falta es una dependencia real del
incremento con el último store que no toque a los `addi`. **El mapa de líneas
deja el sitio**: el objetivo tiene `prev_penetration[a_lp]` en la 468 y `a_lp++`
en la **471** (dos líneas sin instrucción, 469-470, justo donde va la barrera) y
el `} while` en la **474** (otras dos, 472-473). No he encontrado el constructo
de C que produzca esa dependencia.

### 3.4. Ensayos numerados (arnés `c28bw_var.py`, 3,2 s por variante)

Base r27: 99,47890 % / 8 filas / 1.896 B.

| # | forma | % | filas |
|---|---|---|---|
| **i1** | **`__asm__("" : "+r"(a_lp) : : "memory")` delante de `a_lp++`** | **100** | **0** ← APLICADA |
| i5 | `__asm__("" : "=r"(a_lp) : "0"(a_lp) : "memory")` idem | **100** | 0 |
| i7 | asm selectivo **detrás** de `a_lp++` (en el sitio de la r27) | 99,45781 | 10 |
| i2 | `__asm__("" : "+r"(a_lp) : "m"(prev_penetration[a_lp]))` | 99,52531 | 43 |
| i3 | idem con `"m"(penetration[a_lp])` | 99,50422 | 45 |
| i4 | i1 **más** el `__asm__("")` de la r27 detrás | 98,95570 | 18 |
| i6 | asm selectivo delante de `prev_penetration` | 98,75527 | 19 (**1.900 B**) |
| h1 | `__asm__ __volatile__("" : "+r"(a_lp) : : "memory")` | 98,95570 | 18 |
| h2 | `__asm__ __volatile__("" : : "r"(a_lp) : "memory")` | 98,95570 | 18 |
| h3 | `__volatile__("" : "=r"(a_lp) : "0"(a_lp) : "memory")` | 98,95570 | 18 |
| h4 | `__asm__ __volatile__("" : "+r"(a_lp))` | 98,95570 | 18 |
| h5 | `__asm__ __volatile__("" : : : "memory")` | 98,95570 | 18 |
| h6 | `__asm__ __volatile__("" : : "r"(a_lp))` | 98,95570 | 18 |
| h7/h8 | h1/h2 conservando además la barrera de la r27 | 98,95570 | 18 |

**Veda con su sentencia — el eje `volatile` está agotado**: barrí **ocho** formas
de asm **volátil** delante de `a_lp++` (con y sin `"memory"`, con `"+r"`, `"=r"`
+ `"0"`, sólo `"r"`, y sin operandos) y **las ocho** dan exactamente 98,95570 % /
18 filas, que es lo mismo que la barrera vacía de la r26. **Cualquier asm
volátil en ese punto es la misma barrera total.** Las vedas de la r27 (posición
de la barrera vacía, `++a_lp` en la condición, `!= 3`, `< 3`, orden de las
penetraciones y de los `dists`, y el eje `n_refs` de `b_dim`) siguen en pie y
están en el comentario del fuente.

---

## 4. `EPlayRaceNIS`: cerrada (2.176 B) — **y sin `asm`**

### 4.1. El cambio, todo C++ corriente

    -    float dot = (UMath::Dot(car_fwd, cop_dir) + 1.0f) * 0.5f;
    -    const float minTresh = 0.25f;
    -    distTresh = (dot * 0.75f + minTresh) * distTresh;
    +    float minTresh = 0.25f;
    +    float dot = UMath::Dot(car_fwd, cop_dir) + 1.0f;
    +    float dotScale = 0.75f;
    +    float dotHalf = 0.5f;
    +    distTresh = ((dot * dotHalf) * dotScale + minTresh) * distTresh;

El valor es idéntico redondeo a redondeo (`((Dot+1)*0,5)*0,75 + 0,25`) y la
secuencia emitida es la misma: `fadds`, `fmuls`, `fmadds`, `fmuls`.

### 4.2. El mecanismo, con los números

Las tres filas eran una permutación de FPR:

    301 fadds  f1, f1, f27  | f26      (+1.0f)
    302 fmuls  f1, f1, f29  | f27      (*0.5f)
    303 fmadds f1, f1, f28, f26 | f29  (*0.75f + 0.25f)

Las cuatro constantes son **invariantes del bucle**, `loop` las iza al
preencabezado y `global-alloc` reparte f29→f26 en orden de prioridad. `alloc.py`
sobre el `.greg` (todas con `n_refs` 3, o sea `pri = 30000/live_length`):

    pseudo 450  live 282  pri 106  -> f29   0.25f
    pseudo 446  live 286  pri 104  -> f28   0.75f
    pseudo 439  live 290  pri 103  -> f27   0.5f
    pseudo 436  live 296  pri 101  -> f26   1.0f

El objetivo quiere f26=0.25, f27=1.0, f28=0.75, f29=0.5, o sea `live_length`
decreciente **0.25 > 1.0 > 0.75 > 0.5**: el orden de definición tiene que ser
**0.25, 1.0, 0.75, 0.5**.

**Y aquí está la regla nueva, extrapolable:** el orden de izado **no es el del
fuente, es el del RTL**, y **un `const float` con inicializador constante se
propaga y NACE EN SU USO**, no en su declaración. Por eso mover
`const float minTresh = 0.25f;` a cualquier sitio no cambia nada. **Con locales
NO const cada una tiene su propio `set` en la declaración y el orden se elige
desde el fuente.**

### 4.3. Ensayos numerados (arnés `c28bw_mv.py`, ~45 s por variante)

Base 99,97243 % / 3 filas / 2.176 B.

| # | forma | % | filas |
|---|---|---|---|
| **c2** | **cuatro locales `float` en orden 0.25, 1.0, 0.75, 0.5** | **100** | **0** ← APLICADA |
| c1 | sólo `minTresh` como `float` (no `const`), declarada la primera | 99,98162 | **1** |
| c4 | tres locales `float` pero `dot` con la expresión entera | 99,98162 | 2 |
| c3 | c2 con `const float` en las cuatro | 98,94486 | 23 (**2.156 B**) |
| a1 | `(1.0f + Dot(...)) * 0.5f` | 99,97243 | 3 (idéntica) |
| a2 | `0.5f * (Dot(...) + 1.0f)` | 99,97243 | 3 (idéntica) |
| a3 | `(minTresh + dot * 0.75f) * distTresh` | 99,97243 | 3 (idéntica) |
| a4 | `distTresh * (dot * 0.75f + minTresh)` | 99,97243 | 3 (idéntica) |
| a5/b1/b2 | `minTresh` declarada antes (tres sitios distintos) | 99,97243 | 3 (idéntica) |
| a6 | `0.25f` en línea, sin `minTresh` | 99,97243 | 3 (idéntica) |
| a7 | `(0.75f * dot + minTresh)` | 99,97243 | 3 (idéntica) |
| a8 | `/ 2.0f` en vez de `* 0.5f` | 99,97243 | 3 (idéntica) |
| b4 | los cuatro como `const float` con nombre | 99,97243 | 3 (idéntica) |
| b5 | `dot` partida en dos sentencias | 99,97243 | 3 (idéntica) |
| b3/c3 | `((dot*0.5f)*0.75f...)` con `const` | 98,94486 | 23 (2.156 B) |

**Veda con su sentencia**: barrí **doce** formas del ORDEN DE OPERANDOS y de la
POSICIÓN de las constantes manteniéndolas `const`/literales
(`1.0f + x`, `0.5f * x`, `minTresh + …`, `distTresh * …`, `0.75f * dot`,
`/ 2.0f`, `minTresh` en tres posiciones distintas, `const float` con nombre,
`dot` partida): **las doce dan el mismo objeto exacto**. El reparto de FPR de
una constante **no se toca desde la forma de la expresión**; se toca **quitando
el `const`**. Y ojo con c3/b3: con `const`, GCC **pliega `0.5f*0.75f`** y sale un
`fmuls` de menos (2.156 B); con locales no `const` no lo pliega.

---

## 5. `_bOutput`: el techo se CONFIRMA, y ahora también contra la palanca nueva

Las dos filas son un intercambio adyacente en el bloque básico 264:

    912 subi r0, r22, 0x1      (4723)   precision - 1
    913 addi r23, r1, 0x10     (7303)   <- objetivo: la insercion de PRE
    914 subf r3, r0, r3        (4725)   p -= ...
    915 addi r5, r22, 0x1      (4729)   desiredPrecision = precision + 1

Nosotros emitimos 4723, **4729**, 4725, **7303**. Con ritmo de emisión 2 es una
sola decisión: **qué entra en la segunda ranura del ciclo 1**.

### 5.1. Lo que añade esta ronda: **lo decide `sched1`, no `sched2`**

Volcado literal de `sched1` (bloque 264, antes de reload):

    insn  prio  dependientes
    4723    3   6811 4725
    4725    2   6811
    4729    2   6811
    7303    2   6811
    ;;	Ready list (t =  1):    7303  4729  4723      (peor -> mejor)
    ;;		--> scheduling insn <<<4723>>> ... <<<4729>>>

4729 y 7303 **empatan en prioridad (2), en dependientes (1: el salto) y en
`INSN_REG_WEIGHT` (los dos +1: un registro nace, ninguno muere)**. Decide el
LUID, y el de PRE es el más alto del bloque. **`sched1` ya deja 4729 delante de
7303, y como el LUID que ve `sched2` es el que dejó `sched1`, `sched2` repite.**

Para darle la vuelta habría que subir `INSN_REG_WEIGHT(4729)` a 2 (imposible:
`find_insn_reg_weight` cuenta un registro por `SET`/`CLOBBER`, y un `addi` sólo
tiene uno) o bajar el de 7303 a 0 (necesitaría una nota `REG_DEAD`, y su fuente
es el puntero de marco, que no muere nunca).

### 5.2. La palanca nueva tampoco entra: cuesta 8 B

Con la barrera selectiva se puede retrasar a 4729, **pero la única forma de
atarla a `desiredPrecision` es ESCRIBIR `precision`**, y `precision` sigue viva
después: la restricción de registro obliga a copiar. **Las seis formas cuestan
+8 B (5.188 B) y 55 filas.**

| # | forma | % | filas |
|---|---|---|---|
| q1 | `__asm__("" : "+r"(precision) : "r"(p))` delante de `desiredPrecision` | 99,42857 | 55 (**5.188 B**) |
| q2 | `__asm__("" : "+r"(precision), "+r"(p))` | 99,42857 | 55 (5.188 B) |
| q3 | `__asm__("" : "+r"(precision) : : "memory")` | 99,42857 | 55 (5.188 B) |
| q4 | `__asm__("" : "+r"(precision))` | 99,42857 | 55 (5.188 B) |
| q5 | `__asm__("" : "+r"(precision) : "r"(p) : "memory")` | 99,42857 | 55 (5.188 B) |
| q6 | idem delante de `p -=` | 99,36911 | 55 (5.188 B) |
| q7 | `__asm__("" : "+r"(p))` detrás de `desiredPrecision` | 99,98301 | 2 (idéntica) |
| q8 | `__asm__("" : "=r"(desiredPrecision) : "r"(p))` delante | 99,98301 | 2 (idéntica, el asm muere) |

### 5.3. Ocho formas de fuente más, todas gratis y todas idénticas

| # | forma | % | filas |
|---|---|---|---|
| s1 | las dos sentencias intercambiadas, **sin** asm | 99,98301 | 2 |
| s2 | `desiredPrecision = precision; desiredPrecision++;` | 99,98301 | 2 |
| s3 | `desiredPrecision = 1 + precision;` | 99,98301 | 2 |
| s4 | `p = p - (precision - 1);` | 99,98301 | 2 |
| s5 | `p -= precision - 1;` (sin paréntesis) | 99,98301 | 2 |
| s6 | `desiredPrecision = precision - -1;` | 99,98301 | 2 |
| s7 | `p -= precision; p++;` | 99,81699 | **6** (peor) |
| s8 | `desiredPrecision = (precision + 1);` | 99,98301 | 2 |

**Van 87 formas medidas entre las r24, r25, r26, r27 y r28.** Sigue puesto lo
que entra gratis por DWARF: **`char *p` como primera declaración** del bloque
`%z` y **`va_arg(argList, unsigned int)`**.

**Conclusión: `_bOutput` no cierra por fuente.** El techo es
`LUID(PRE) < LUID(desiredPrecision)` en `sched1`, PRE lleva siempre el LUID más
alto del bloque, y las dos vías de desempate anteriores (`INSN_REG_WEIGHT` y el
número de dependientes) están las dos empatadas y son las dos inalcanzables
desde el fuente.

---

## 6. La deuda de `ESpawnFragment`: NO era una falsificación

El encargo pedía buscar «la forma verdadera» de `m_inner[3][3] = 1.0f;`. **He
medido la premisa y sale a favor del constructo de la r26.**

1. **`VU0_quattom4` de nuestro `UVectorMathGC.hpp` es línea a línea el del
   original.** Anclando el mapa de líneas del objetivo contra nuestro fichero,
   el desplazamiento es **constante y exacto (+211)** en cinco puntos
   independientes: `hpp:472`↔`result[0][0]` (nuestra 261), `hpp:475`↔`[0][3]`
   (264), `hpp:478`↔`[1][1]` (267), `hpp:484`↔`[2][2]` (273),
   `hpp:487/488/489`↔`[3][0..2]` (276/277/278) y **`hpp:490`↔`result[3][3] =
   1.0f` (279)**. O sea: **la referencia de más a `1.0f` NO está en la
   cabecera**, que era la otra hipótesis.
2. **El objetivo tiene TRES líneas libres donde va nuestra sentencia.** La
   llamada virtual `model->GetTransform(m_outer)` lleva `ESpawnFragment.cpp:77`
   y `UMath::Mult(...)` lleva `ESpawnFragment.cpp:82`; `bounds->GetTransform` se
   expande entero (todas sus instrucciones llevan `Bounds.h:*` y
   `UVectorMathGC.hpp:*`) y ocuparía la 78. **Quedan libres 79, 80 y 81.**

Que no haya líneas de `ESpawnFragment.cpp` entre la 77 y la 82 **es exactamente
lo que produce una sentencia que no emite código**, no una prueba de que no la
haya. **No la he cambiado**: el mapa de líneas la permite y el código emitido es
el del objetivo.

---

## 7. Qué NO he probado

- **`SphereVsBox`**: no he buscado el constructo de C que produzca la
  dependencia store→incremento sin `asm` (es la deuda). No he tocado el segundo
  ni el tercer bucle, ni el permutador, ni restricciones de registro.
- **`EPlayRaceNIS`**: no he probado si el mismo truco (locales no `const` para
  reordenar constantes izadas) mueve otras funciones; **no lo he contado en
  ninguna otra unidad**, así que no es un frente hasta contarlo (memoria
  «extrapolar frentes»).
- **`_bOutput`**: no he tocado las cuatro etiquetas de más (`OUTPUT`,
  `positioned`, `Z_INT`, `VECT_OUTPUT`), ni `int textLen` (35 filas, deuda de la
  r24), ni los dos bloques anónimos, ni `GENERIC_FLOAT`. La regla §3 del brief
  dice que quitarlas de una en una empeora y la reescritura completa no cabía.
- **El frente §1 (exceso)**: he medido las secciones y he identificado los 286 B
  de `$LC*` que rompen la promoción de zDynamics, **pero no los he perseguido**
  (no era mi encargo y toca `configure.py`).
- **La barrera selectiva en el resto del árbol**: es la palanca nueva y sólo la
  he usado en dos funciones.

## 8. Herramientas y trampas de esta ronda

- **`c28bw_rtl.py` necesita un prólogo**: `Geometry.cpp` compilado suelto **no
  ve `memset`** (lo declara otro `.cpp` de la misma SourceList) y `cc1plus`
  salía con `rc=33`; **los volcados se generaban igual y con el RTL
  equivocado**. El arnés acepta ahora `STUBPRE` para inyectar la declaración.
  `EPlayRaceNIS.cpp` suelto también falla (`StaticData` incompleto) pero ahí
  **sí** reproduce el `.s` de la unidad: verificado instrucción a instrucción
  antes de fiarme del `.greg`.
- **`alloc.py` necesita `.lreg` Y `.greg`**: hay que pasar `-dl` además de
  `-dg` o peta con `FileNotFoundError`.
- **La lista de `Ready list` se imprime de PEOR a MEJOR** y el planificador saca
  por el final. Confirmado tres veces en esta ronda contra el
  `--> scheduling insn` inmediatamente siguiente.
- `Geometry.cpp` sigue con finales **mezclados** (cuerpo CRLF, comentarios LF) y
  `EPlayRaceNIS.cpp` es **LF entero**: parcheo línea a línea en los dos arneses.
- **Disco**: borrados 446 MB de la ronda anterior (`c27lk_try/`, los trece
  `mnd_r_zMain_ESpawnFragment_*.json` de 11 MB y `c26wag_rows.json`). Quedan
  **16 GB** libres.
- **Arneses nuevos** (scratchpad, prefijo `c28bw_`): `c28bw_var.py`
  (SphereVsBox, 3,2 s), `c28bw_bv.py` (`_bOutput`, ~5 s), `c28bw_mv.py`
  (EPlayRaceNIS, ~45 s), `c28bw_mn.py` / `c28bw_dy.py` / `c28bw_bw.py` (diff por
  función), `c28bw_rtl.py` (volcados RTL, `UNIT=`/`SRC=`/`STUBPRE=`),
  `c28bw_rtlvar.py` (volcado RTL de una variante, restaura siempre).
