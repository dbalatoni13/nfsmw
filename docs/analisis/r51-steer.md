# r51 · `steer` — `steering` (Metrowerks) y `madidct`

**Cerrado: nada.** Cero bytes de `matched_code` ganados, cero regresiones. El árbol
queda **idéntico al último commit** en los dos ficheros del encargo
(`src/LibSN/steering.c`, `src/egami/rcmp/dev/source/decoder/cmn/madidct.cpp`):
verificado con `fncmp.py` antes y después, mismas seis funciones de `steering`
(2.568 B) y mismas dos de `madidct` (1.148 B), mismos porcentajes.

Lo que sí sale de la ronda son **tres negativos con control** y **el catálogo de
palancas de Metrowerks**, que es lo que pedía el encargo y no estaba escrito.

---

## 1. El catálogo de Metrowerks (§ el entregable)

`steering` va con **`mwcceppc.exe` de `GC/2.7`**, confirmado en
`scripts/build_direct.py` (`parse_units()['steering']`):

    -lang=c -O4,s -inline off -use_lmw_stmw on -enum int -char unsigned
    -fp hardware -fp_contract on -str reuse -RTTI off -Cpp_exceptions off
    -pragma "cats off" -requireprotos -nosyspath

Ninguna palanca del catálogo de GCC aplica: no hay `cse`, `sched1`,
`local_alloc`, volcados RTL, pin `register T x asm("rN")` (es sintaxis de GCC),
ni barreras `asm` extendidas. Lo medido en esta ronda, ~60 compilaciones:

### 1.1 Lo que MWCC PLIEGA — no-ops con objeto byte a byte idéntico

Esto es lo importante, porque son justo las palancas que uno trae de GCC:

| forma | medida |
|---|---|
| `register s32 x;` en una local | **no-op** (2 variantes, objeto idéntico) |
| partir una cadena de asignación (`out = mag = X` → `mag = X; out = mag;`) | **no-op** |
| **orden de declaración dentro de un bloque/macro interior** | **no-op** (4 permutaciones de las 5 locales de `AUTOCALX`, objeto idéntico) |
| meter un temporal redundante (`s8 _raw; _raw = f; _v = _raw;` sin usarlo en las ramas) | **no-op** |
| izar un invariante de bucle a una local (`sc = 4096.0f;`) | **no-op** |
| adelantar/atrasar una asignación independiente | casi siempre **no-op** |

**La regla general: MWCC pliega todo lo que no cambia el ÁRBOL DE EXPRESIONES ni
el grafo de control.** Renombrar, reordenar declaraciones internas, partir copias
y añadir temporales no llegan al generador de código. Es la diferencia grande
con GCC 2.9 y explica por qué las vedas de r36e/r47/r48 salen «neutro» tan a
menudo: no es que la hipótesis fuera mala, es que el cambio no llegó a compilarse
distinto. (Regla 12 del brief: en MWCC hay que exigir el control SIEMPRE.)

### 1.2 Los `#pragma` — cuáles existen y cuáles hacen algo

Control usado: `#pragma esto_no_existe on` da `Warning: illegal #pragma`; los
seis de abajo NO avisan, o sea **son pragmas reconocidos que no cambian nada**
bajo `-O4,s`:

    opt_lifetimes · opt_loop_invariants · opt_propagation
    opt_dead_assignments · opt_strength_reduction · optimization_level 3

Los que **sí** mueven el código (todos ya en el ajuste correcto; cualquier
cambio empeora, cifras sobre las 6 funciones abiertas + `HandlePedals` y
`LGRead`, que están al 100 %):

| pragma | efecto al apagarlo |
|---|---|
| `scheduling off` | destroza todo: `LGRead` 100 % → 80,58 %, `HandlePedals` → 93,67 % |
| `peephole off` | `HandleTriggers` 588 → 668 B, `HandlePedals` 512 → 556 B |
| `optimize_for_size off` | `SimThread_Init` 324 → **824 B** (0 %); confirma `-O4,s` |
| `use_lmw_stmw off` | `Effect_Init` 276 → 284 B, `LGRead` 236 → 244 B |
| `opt_common_subs off` = `global_optimizer off` | `HandleTriggers` 588 → 604 B, `LGRead` 100 % → 97,97 % |

O sea: **el optimizador global, el planificador, el peephole y `lmw/stmw` ya
están donde tienen que estar**, y eso queda demostrado por las 30 funciones que
siguen al 100 %.

### 1.3 Las palancas que SÍ existen en MWCC

1. **El orden de declaración de las locales DE LA FUNCIÓN** (no las de un bloque
   interior). Es la única palanca de declaración que funciona. Barrido completo:
   las **24 permutaciones** de `mxl,mxr,vl,vr` en `HandleTriggers` dan
   exactamente **tres** resultados —42, 50 o 58 filas distintas— y la regla es
   que los `mx*` vayan antes que los `v*`. El orden actual ya es óptimo: la veta
   está agotada, no hay que volver.
2. **El orden de las asignaciones DENTRO de una rama.**
   `{ _mx = _v; (mx) = _v; }` contra `{ (mx) = _v; _mx = _v; }` cambia el objeto.
   Vale +1 fila en `CookValues` (receta en §3.2).
3. **La forma de la expresión y del control**: ternario contra `if`
   (`Effect_PerformEnvelope` 93,27 % → 76,94 %), `for` contra `do-while`
   (`Effect_Init` 276 → 280 B), asociatividad (`i * (PI/128.0f)` cuesta 8 B
   contra `PI * (float)i / 128.0f`), cachear un campo contra releerlo.
4. **El tipo de una local** (`int` contra `s8`/`u8`): decide los `extsb`/`clrlwi`.
   Medido en `CookValues`: `s8 _v` sube a 20 filas (peor); `T _mx` a 21.

### 1.4 Lo que queda en las 6 funciones: es PERMUTACIÓN, no estructura

Diagnóstico por función, todo con `fndiff.py`:

| función | B | qué le queda exactamente |
|---|---:|---|
| `HandleTriggers` | 588/588 | **cero filas estructurales**: dos 3-ciclos puros de registros volátiles (`dz,mxl,min` → nuestro `r7,r8,r10` contra `r8,r10,r7`) |
| `CookValues` | 260/260 | 1 fila estructural (`extsb r0,r5` contra `mr r0,r6`) + rotación `r4/r5/r6` |
| `SimThread_Init` | 324/324 | 2 filas: el `lfs` de la escala va **detrás** del `mtctr` en el objetivo y delante en el nuestro; más un empate desnudo `r5/r6` en el último bucle |
| `Effect_Init` | 276/276 | el objetivo salva **seis** registros (`stmw r26`) y nosotros cinco (`stmw r27`): iza `lis 0x4330` a `r27` y nosotros lo rematerializamos cada vuelta |
| `Effect_PerformEnvelope` | 196/**192** | falta exactamente **un `mr r5,r9`**: el objetivo tiene `out` y `mag` en registros distintos y MWCC nos los fusiona |
| `SimThread_Step` | 924/**920** | tres ceros con un `li` + dos `mr` contra tres `li`; y el valor de la cola vive en `r26` (salvado) en el objetivo y en un volátil + copia en el nuestro |

Las cuatro de tamaño exacto **no tienen ni una instrucción de más ni de menos**.

---

## 2. `madidct` — tres negativos con control, y la fuente ya es la del original

Primero, dos correcciones al encuadre del encargo:

- **No es libmad.** `madidct.cpp` es el IDCT del códec de vídeo **MAD de EA**,
  dentro de la biblioteca `rcmp` (`egami/rcmp/dev/source/decoder/cmn/`), vecino
  de `maddec`/`maddeca`/`rcmp_mad_codec`. No hay fuente libre que transcribir.
- **«Escribir desde cero» no tiene nada que escribir**: el DWARF del ORIGINAL
  existe para esta unidad y **confirma que nuestra fuente ya es la suya**.

### 2.1 El DWARF del original (`symbols/mw_dwarfdump.nothpp`, línea 2.556.894)

    static void IdctColumn(int *src /* r31 */, int *dest /* r15 */)
        t1 r3 · t2 r14 · t3 r12 · t4 r17 · t5 r23 · t6 r0 · t7 r26 · t8 r30 · t9 r11
    static void IdctRow(int *src /* r3 */, int *dest /* r4 */)
        t1 r27 · t2 r31 · t3 r17 · t4 r15 · t5 r21 · t6 r0 · t7 r30 · t8 r3 · t9 r14

Nueve locales `int`, con esos nombres, ese orden y esas firmas: **exactamente
nuestra fuente**. No sobra ni falta una local. El 46 %/50 % no es estructura de
fuente; es reparto de registros.

### 2.2 Negativo 1 — el pin sobre los registros del PROPIO original: PEOR

Ocho ensayos con `register int tN asm("rNN")` usando los registros que dicta el
DWARF de arriba (la prior más fuerte que puede haber):

| ensayo | Column | Row |
|---|---|---|
| base | 632/632 50,449 % 152/188 | 516/**496** 46,372 % 137/154 |
| `col t8=r30` | 632/**676** 46,614 % 180 | — |
| `col t7=r26 t8=r30` | 632/**676** 46,709 % 180 | — |
| `col t5,t7,t8` | 632/**684** 47,266 % 180 | — |
| `col` FULL DWARF | 632/632 54,506 % **172** (peor en filas) | — |
| `row t8=r3` | — | 516/496 48,822 % 137 |
| `row t2,t7,t8` | — | 516/**544** 32,473 % 158 |
| `row` FULL DWARF | — | 516/**540** 45,643 % 144 |

Causa: cada `tN` guarda **dos o tres valores distintos** (`t1` es
`src[1]-src[7]` y luego `t3+t2`), y el DWARF sólo registra el hogar del ÚLTIMO.
El pin serializa todos los valores en un solo registro duro. **Eje cerrado.**

### 2.3 Negativo 2 — la VERSIÓN DEL COMPILADOR no es el diferenciador

El original de `madidct.cpp` dice `GNU C++ 2.95.3 SN BUILD **v1.72**`, y nosotros
compilamos con `ProDG/3.9.3` = **v1.76**. Parecía la pista de
`nfsmw-cflags-por-biblioteca`. No lo es:

- Las **13** unidades de `rcmp` son v1.72 y **12 casan** con nuestro v1.76.
- `idctcompute`, en el MISMO fichero y con los MISMOS cflags, casa al **100 %**.
- Compilado `madidct` con **los cinco ProDG del árbol**, el `.text` sale
  **byte a byte idéntico** (1.484 B, `sha1 3df2d512784e`) en los cinco:

      ProDG/3.9.3    v1.76   .text 1484  sha 3df2d512784e
      ProDG/3.8.1    v1.55   .text 1484  sha 3df2d512784e
      ProDG/3.7      v1.46   .text 1484  sha 3df2d512784e
      ProDG/3.5b140  v1.40   .text 1484  sha 3df2d512784e
      ProDG/3.5      V1.37   .text 1484  sha 3df2d512784e

  **Control (regla 12): los cinco objetos son distintos** —cadena `Producer`
  distinta y tamaño distinto (10.232 / 10.692 / 10.944 / 10.944 / 10.928 B)—, o
  sea que los cinco compiladores corrieron de verdad. El código no se mueve entre
  v1.37 y v1.76: v1.72 tampoco lo movería. **No hay que buscar ese compilador.**

  (De paso, censo para el proyecto: faltan del árbol **v1.72** —las 13 de
  `rcmp`— y **v1.83** —las 15 de `path/5.01.04`—. Para `rcmp` está demostrado
  que da igual; para `path` no lo he medido.)

### 2.4 Negativo 3 — la forma de `MULT`

    static inline int MULT(int a, int b) { ... }   632 / 496   <- BASE, la buena
    inline int MULT(...)  (sin static)             632 / 496   idéntico
    #define MULT(a,b) ...                          612 / 472
    #define ... con los dos casts a long long      612 / 472
    #define ... con >>16 sobre unsigned long long  612 / 472
    static int MULT(...) (sin inline)              432 / 324

`static inline` es la única forma que da el tamaño del objetivo.

### 2.5 Dos hechos estructurales nuevos, del diff de DWARF (`build/regmap/our_madidct_*.nothpp`)

1. **Nuestro `t8` no tiene registro** en ninguna de las dos funciones (el volcado
   lo deja en blanco); el del original es `r30` (Column) y `r3` (Row). Es la
   única local con esa asimetría, y es la única de las nueve con **una sola
   asignación**.
2. **Nuestro DWARF lleva un DIE `inline int MULT(int a, int b)` de rango cero**
   dentro de las dos funciones; **el del original no lleva ninguno**. Con el
   mismo compilador y la misma forma `static inline`. No sé explicarlo y no lo he
   perseguido, pero es la primera diferencia de DWARF que aparece en esta unidad
   y queda anotada.

### 2.6 Lo que le falta a `madidct` para promocionar: SÓLO estas dos funciones

`python scripts/promote.py egami/rcmp/dev/source/decoder/cmn/madidct`:

    - .text mide 1484 B y el extraido 1504 B
    - 1 simbolo(s) en OTRO SITIO de su seccion: idctcompute .text+1148 -> .text+1128

Los 20 B son exactamente el déficit de `IdctRow`, y el desplazamiento de
`idctcompute` es su consecuencia. **Cero trabajo de datos, cero secciones de
más.** `madidct` es la unidad más limpia de las 16 de biblioteca que quedan.

---

## 3. Ensayos de `steering` con su cifra (todos revertidos)

Base: `SimThread_Step` 924/920 95,67 % · `HandleTriggers` 588/588 98,30 % ·
`SimThread_Init` 324/324 94,69 % · `Effect_Init` 276/276 88,62 % ·
`CookValues` 260/260 95,77 % · `Effect_PerformEnvelope` 196/192 93,27 %.
(«filas» = instrucciones distintas contadas por `fndiff.py`.)

### 3.1 `Effect_PerformEnvelope` — 14 ensayos, todos neutros

Falta un `mr r5,r9`. `r5` **es el registro del parámetro `level`**, así que la
hipótesis natural es que el original no tiene `out` y reusa `level`. Medido:

    out==level, mr explicito ............ 196/192 93,27 % 31 filas (= base)
    out==level, cadena level=mag=abs .... 196/192 90,31 % 26 filas
    out==level, neg antes .............. 196/192 93,27 % 31
    mag/neg/out en tres sentencias ..... 196/192 93,27 % 31
    neg primero .......................  196/192 93,27 % 31
    register mag / register out / los dos  196/192 93,27 % 31  (NO-OP, ver §1.1)
    out declarado primero / v primero / sustain al final  93,27 % 31 (NO-OP)
    neg = level < 0 .................... 196/192 93,27 % 31
    mag via if (en vez de ternario) ..... 196/192 76,94 % 39   <- el único que mueve

MWCC fusiona `out` y `mag` pase lo que pase. **Veda: la coalescencia de
`out`/`mag` no se rompe desde la fuente.**

### 3.2 `CookValues` — la única mejora medida, y por qué NO la dejo puesta

`AUTOCAL` es compartida con `HandlePedals` (que está al **100 %**). Barrido de
la rama del máximo sobre la macro:

    _mx = (mx)              Cook 260/264 15 filas · HandlePedals 100 % -> 99,06 %  PEOR
    _mn = (mn)              Cook 260/264 15        · HandlePedals -> 99,06 %       PEOR
    _mx = _v ANTES del store Cook 260/260 13 filas · HandlePedals -> 96,88 %       PEOR
    (mx) = (vv) las dos      Cook 260/268 16       · HandlePedals 512 -> 520 B     PEOR
    T _mx                    Cook 260/264 21       · HandlePedals 512 -> 520 B     PEOR
    sin _mx (relee (mx))     Cook 260/264 17       · HandlePedals -> 98,98 %       PEOR

Los dos sitios de llamada quieren **órdenes opuestos**: los `u8` de
`HandlePedals` quieren `(mx) = _v; _mx = _v;` y el `s8` de `CookValues` quiere
`_mx = _v; (mx) = _v;`. Eso es evidencia de que **el original no compartía UNA
macro entre los dos**. Expandiendo `AUTOCAL` a mano SÓLO en `CookValues` con el
orden invertido:

    CookValues 260/260  95,77 % -> 97,92 %  (14 -> 13 filas)
    HandlePedals   100 %  ·  HandleTriggers  sin cambio

y las filas 20-21 (`_mx=` y el `stb`) pasan a caer **en el orden del objetivo**.
Receta exacta, por si otra ronda la quiere:

```c
/* en CookValues, en vez de la llamada a AUTOCAL(s8, raw->steering, ...) */
{
    int _mn; int _v; int _mx; s8 _dz; int _r; int _t;
    _v  = raw->steering;
    _mn = cal->min.steering;
    _dz = cal->dead.steering;
    _mx = cal->max.steering;
    if (_v < _mn) { cal->min.steering = _v; _mn = _v; }
    if (_v > _mx) { _mx = _v; cal->max.steering = _v; }   /* <- ESTE es el orden */
    _r = (_mx - _mn) - _dz * 2;
    if (_r == 0) _r = 1;
    _t = ((_v - (_mn + _dz)) * 255) / _r - (127);
    if (_t < (-128)) _t = (-128);
    if (_t > (127))  _t = (127);
    out->steering = _t;
}
```

**No la dejo puesta**: son 19 líneas y una macro bifurcada a cambio de **cero
bytes** (`matched_code` es todo-o-nada), y el brief manda no dejar trabajo a
medias. Queda aquí, es un minuto reaplicarla.

Lo que queda tras la receta (13 filas): una sola fila estructural,
`extsb r0,r5` contra `mr r0,r6`. Barrido para forzarla, todo peor o neutro:
`(mx) = raw->steering` (264 B), `_mx = raw->steering` (264 B), `s8 _v` (20
filas), `s8 _v` + `s8 _mx` (268 B), un `s8 _raw` intermedio usado en las ramas
(17-20 filas). Con `_raw` declarado pero **no** usado en las ramas el objeto es
idéntico al de la receta: MWCC lo pliega (control de §1.1).

### 3.3 `SimThread_Init` — 8 formas de bucle, ninguna baja de 9 filas

    a[n]*4096.0f en vez de 4096.0f*a[n] ... 324/324 94,69 %  9   (NO-OP)
    bucle ia do-while ..................... 324/328 85,19 % 18
    ultimo bucle do-while ................. 324/328 85,65 % 20
    ultimo bucle con n .................... 324/324 94,69 %  9   (NO-OP)
    ultimo bucle hist antes que coeff ..... 324/324 89,86 % 11
    histIndex = 0 antes del bucle ia ...... 324/320 91,11 % 15
    escala 4096.0f izada a una local ...... 324/324 94,69 %  9   (NO-OP)

### 3.4 `Effect_Init` — 20 ensayos, ninguno llega a `stmw r26`

    for en vez de do-while ............. 276/280 78,33 %
    (float)i a una local float ......... 276/284 84,32 %
    sin/(double) ....................... 276/276 81,38 %
    (float)i * PI / 128.0f ............. 276/276 88,62 %  (NO-OP)
    sin el cast (float) sobre sin() .... 276/272 79,91 %
    ramp antes de sine ................. 276/280 74,28 %
    i++ al final ....................... 276/276 88,62 %  (NO-OP)
    sin `phase`, con (i*1024)/63 ....... 276/272 79,28 %
    off += 2 antes de phase ............ 276/276 88,62 %  (NO-OP)
    sine[i] / ramp[i] indexados ........ 276/272 83,04 %
    i * (PI/128.0f) · (PI/128.0f) * i · (float)i * (PI/128.0f)  276/268 85,22 %
    PI * ((float)i / 128.0f) ........... 276/276 88,19 %
    off u32 / phase u32 / i declarada primero ... 276/276 88,62 % (NO-OP)

### 3.5 `HandleTriggers` — el eje del orden de declaración, AGOTADO

24/24 permutaciones de `mxl,mxr,vl,vr` (§1.3.1): 42 filas es el mínimo y el
orden actual ya lo alcanza. Además: 4 permutaciones de las locales de `AUTOCALX`
(objeto idéntico, no-op), locales `dzl`/`dzr` para `cal->dead.*` (no-op), y
`mxl = cal->max.left` antes de `vl = raw->left` (no-op).

---

## 4. AVISO IMPORTANTE: `steering` NO promociona cerrando las seis funciones

El encargo decía «cerrar todas las funciones la deja a un paso de `linked`».
Para `madidct` es cierto (§2.6). **Para `steering` no.**
`python scripts/promote.py LibSN/steering`:

    - secciones que emitimos de mas: .bss(40B), .sdata2(40B)
    - .sbss mide 20 B y el extraido 16 B
    - .text mide 8752 B y el extraido 8760 B
    - 7 simbolos en OTRO SITIO de su seccion (Effect_PolarToRect, Effect_UpdateDamper,
      Effect_UpdateSpring...)
    - 4 reubicaciones a simbolo con sufijo de direccion:
      g_iGammaLookup_804B9E78, g_iQuarterSineTable_804B9EFC, g_iRampUpTable_804B9F7C

Enfrentando las tablas de símbolos de los dos objetos:

| sección | original extraído | nuestro |
|---|---|---|
| `.text` | 8.760 | 8.752 (los 8 B son SimThread_Step 4 + Effect_PerformEnvelope 4) |
| `.data` | 40 (`a$1517`) | 40 (`a$723`) ✔ |
| `.sbss` | 16: `g_bGammaInitialized, jumbleeffectid$1040, createcount$1041, g_bWaveTablesInitialized` | 20: los cuatro **+ `g_bInitialized`**, y en otro orden |
| `.bss` | **no existe** | 40 (`ia$724`) |
| `.sdata2` | **no existe** | 40 (`@348`…`@756`, 9 constantes) |

Tres cosas de coordinación, no de fuente:

1. **`g_bInitialized`**: en el original resuelve a `lbl_804FF8C0` (`LGInit`
   fila 4), o sea que **cae fuera del rango `.sbss` que `splits.txt` le da a
   `steering`**. Los 4 B de exceso son eso.
2. **`ia$724` (.bss 40 B)** y **las 9 constantes de `.sdata2`**: el original las
   referencia en `0x804B9FFC` y `0x805007F8..0x80500828`, rangos que el troceador
   atribuye a otro sitio. Es el patrón de `nfsmw-bss-y-huecos-estripados` /
   `nfsmw-rango-no-basta`, y de `splits.txt` no toco nada.
3. Las 4 reubicaciones con sufijo de dirección son la familia de `mangfix.py`.

**Recomendación**: resolver 1–3 ANTES de gastar otra ronda en las seis funciones.
Tal como está, aunque salieran las seis (2.568 B de `matched_code`), `steering`
**seguiría sin enlazar**. `madidct`, en cambio, sólo necesita sus dos funciones.

---

## 5. Veredicto y qué haría la ronda siguiente

- **`madidct`** (1.148 B): la fuente ya es la del original (DWARF), el compilador
  da igual (5 builds, control pasado), los pins sobre los registros del propio
  original empeoran, y `MULT` sólo funciona como `static inline`. Es **reparto de
  registros de GCC 2.95 puro** en dos funciones con `long long` y presión alta.
  Lo único sin barrer es la pista de §2.5 (`t8` sin hogar; el DIE `inline MULT`
  que el original no tiene). Con eso agotado, **la unidad debería declararse en
  veda hasta que aparezca un permutador de registros**, no repartirse otra vez.
- **`steering`** (2.568 B): las cuatro funciones de tamaño exacto son
  permutaciones de registros, y MWCC pliega todas las palancas de fuente que un
  agente traería de GCC (§1.1). El orden de declaración —la única palanca de
  declaración que funciona— está **agotado** en `HandleTriggers` (24/24). Lo
  siguiente rentable en esta unidad **no son las funciones, es el bloqueo de
  §4**.
- Los cinco negativos con control de esta ronda (pin DWARF, versión de
  compilador, forma de `MULT`, `register`, orden de declaración interior) valen
  más que el 97,92 % de `CookValues`: cierran ejes que se estaban reabriendo cada
  ronda.

Volcados y objetos de prueba **borrados** (`scratchpad`); `scripts/` sin tocar.
