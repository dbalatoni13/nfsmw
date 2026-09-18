# r50 --- territorio `plat`

`LibSN/steering` (6 fn), `zPlatform`, `zPhysicsBehaviors`, `madidct`,
`criticalpath`. Entrada: **6.920 B en 11 funciones**.

## Resultado

**No cierra ninguna funcion y no se retiene ningun cambio de fuente**: las cinco
unidades quedan byte por byte como estaban (§8: `fncmp` antes y despues es
identico caracter a caracter, `git diff` vacio en las tres fuentes de mi
territorio). Lo que sale de la ronda:

1. **`HandleTriggers` (588 B) esta ESTRUCTURALMENTE CASADA**: sus 42 filas son
   **cero** `INSERT`/`DELETE`/`REPLACE` --- la secuencia de instrucciones es la
   del objetivo instruccion a instruccion--- y todo el diff es **una rotacion de
   tres registros**: objetivo `r8 -> r7`, `r10 -> r8`, `r7 -> r10`. Es la mejor
   pieza de reparto puro del arbol: **588 B a un solo cambio de asignacion**.
   18 formas de fuente y **22 pragmas** medidas, todas neutras o peores (§3).
2. **`CookValues` baja de 14 a 10 filas sin un solo `asm` y con la causa
   nombrada** (§2): el objetivo **conserva el byte CRUDO del `lbz` en un
   registro** y re-extiende (`extsb r0,r5`) donde nosotros copiamos el ya
   extendido. Y hay una variante (**P2**) que es **estructuralmente exacta** ---
   17 filas, todas `ARG_MISMATCH`, cero estructurales--- con una rotacion de
   cuatro registros. ~60 formas nuevas.
3. **Ley medida de mwcc, y explica cinco vedas viejas de golpe** (§5): en
   `LibSN/steering`, **cambiar QUE local sostiene un valor, el orden de las
   declaraciones o la forma sintactica del bucle da el objeto BYTE A BYTE
   IDENTICO**. Solo mueven el objeto los cambios que **anaden o quitan una
   OPERACION** --- y, medido aparte, el TIPO de una local (`T` contra `int`).
4. **`ActualReadJoystickData`: barrido de 20 banderas en sombra, entero
   negativo** y con control valido (§6). Ninguna baja de las 16 filas.
   `-fno-rerun-cse-after-loop` da el **tamano exacto 1588/1588** y 211 filas:
   tercer aviso de la temporada de que el tamano solo no dice nada.
5. **`Effect_PerformEnvelope`: H1 da el tamano EXACTO 196/196** (32 filas contra
   31 de la base) --- primera forma que produce la instruccion que falta--- pero
   la coloca en la rama de `fade`, no en la cabecera. 29 formas nuevas (van 70).
6. **El censo de mi territorio por TIPO de diferencia** (§1), que dice donde
   NO hay que buscar estructura.

| unidad | funcion | antes | despues |
|---|---|---:|---:|
| todas | las 11 | igual | **igual** |

---

## 1. El censo por tipo: cuantas filas NO son de registro

`fndiff` de las once, contando aparte las filas `INSERT`/`DELETE`/`REPLACE`
(estructura) y las `ARG_MISMATCH` (reparto):

```
funcion                          filas   no-registro   tamano
HandleTriggers                     42        0         588/588     <- REPARTO PURO
Effect_PerformEnvelope             31        1         196/192
CookValues                         14        2         260/260
ActualReadJoystickData__Fv         16        2         1588/1580
SimThread_Init                      9        4         324/324
UpdateLoaded__...SuspensionTraffic 21        6         856/860
Effect_Init                        31        7         276/276
SimThread_Step                     68        7         924/920
VP6_PredictFilteredBlock           40        9         740/740
IdctRow                           137       66         516/496
IdctColumn                        152       77         632/632
```

Dos lecturas:

- **Nueve de las once tienen 9 o menos diferencias estructurales.** El trabajo
  que queda en `plat` es casi todo de asignacion de registros, no de fuente.
- **`madidct` es de otra especie** (66 y 77 diferencias estructurales, la mitad
  de sus filas). Confirma la instruccion del encargo de no gastar la ronda ahi.

**El comando es de una linea** y merece entrar en el catalogo (propuesta 1):
`fndiff.py <unidad> <sim> | grep -c "INSERT\|DELETE\|REPLACE"`. Un near-miss con
0 ahi no admite ninguna forma de fuente: es reparto.

---

## 2. `CookValues` (260 B) --- de 14 a 10 filas, y la causa con nombre

Base `target=95.76923% size=260/260`, **14 filas**.

### 2.1 Lo que hace el objetivo y nosotros no

```
objetivo                        nuestro
 7 lbz   r5, 0x3(r4)            lbz   r6, 0x3(r4)      <- el byte crudo
 9 extsb r6, r5                 extsb r6, r6           <- nosotros lo PISAMOS
16 stb   r5, 0x3(r31)           stb   r6, 0x3(r31)     <- crudo contra extendido
20 extsb r0, r5                 (nada)                 <- RE-EXTIENDE
21 stb   r5, 0xd(r31)           stb   r6, 0xd(r31)
22 (nada)                       mr    r0, r6           <- copiamos el extendido
```

O sea: **el objetivo mantiene vivo el byte crudo del `lbz` y re-extiende**, y
nosotros extendemos en el sitio y copiamos. Las 12 filas de registro de la base
son cascada de eso: con el crudo vivo, `r5` (el parametro `cal`) muere antes y
el `lbz` de `cal->min` cae en `r4`; sin el, cae en `r5` y todo rota.

### 2.2 La receta de 10 filas (N8), sin `asm`

Copia acotada de la macro --- `AUTOCALC`, usada **solo** por `CookValues`, con
`HandlePedals` verificado al 100 % en las ~60 variantes---:

```c
#define AUTOCALC(T, vv, mn, mx, dz, bias, lo, hi, dst) \
    { \
        int _mn; int _v; int _mx; T _dz; T _raw; int _r; int _t; \
        _raw = (vv); \
        _v = _raw; \
        _mn = (mn); _dz = (dz); _mx = (mx); \
        if (_v < _mn) { (mn) = _raw; _mn = _v; } \
        if (_v > _mx) { _mx = _raw; (mx) = _raw; } \
        ...el resto igual... \
    }
```

**260/260 B, 98,15385 %, 10 filas** (base 14). Las 10 son **nueve de una
permuta r4<->r6** (`_v` en r6 en el objetivo y en r4 en el nuestro; `_mn` al
reves) **y una** del indice 20 (`extsb r0,r5` contra `mr r0,r4`).

### 2.3 La receta ESTRUCTURALMENTE EXACTA (P2), 17 filas

Cambiando `_v = _raw;` por `_v = (vv);` --- dos lecturas independientes: la carga
la comparte la CSE, la conversion NO---:

**260/260 B, 98,15385 %, 17 filas, y las 17 son `ARG_MISMATCH`**: aparece el
`extsb` del indice 20 y `_v` cae en r6 como en el objetivo. Lo que queda es una
**rotacion de cuatro registros**:

```
objetivo   ->  nuestro
  r5 (crudo)   r0
  r4 (_mn)     r5
  r0 (_mx)     r3
  r3 (dz)      r4
  r6 (_v)      r6   (casa)
```

Es una permutacion ciclica (5 0 3 4): mismas instrucciones, mismos operandos,
otros numeros.

### 2.4 Medido y negativo (todo 260/260 salvo nota)

```
K1 T _v ......................... 21 filas   K2 T _v,_mn,_mx ... 28 (276 B)
K3 T _v,_mx ..................... 22 (268)   K4 T _v + swap ..... 21
K5 int _v + swap ................ 14 = BASE  K6 T _v+_mx=(vv) ... 22 (264)
K7 macro identica (CONTROL) ..... 14 = BASE  <- el arnes es sano
L1/L2/L3  _mn = (mn) = _v encadenado ....... 14 = BASE
L4 releer el campo ......................... 16 filas (268 B)
L5/L6  _mx = (T)_v ......................... 14 = BASE
M1 _mx=(vv) delante 15 (264) - M2 18 (264) - M3 20 (268)
M4/M6/M8  _raw + stores crudos ............. 11 filas
M5 17 - M7 16
N1/N2 _raw declarado antes ................. 21   N3 _v=_raw=(vv) ... 16
N4/N5 _v tras _mn / al final ............... 11   N8 ................ 10
O1 11 - O2/O3 10 - O4/O5 u8 _raw+(T) 17 - O6 20 (264) - O7..OA 10
P1/P2 _v=(vv) .............................. 17 (estructura EXACTA)
P3/P4 11 - P5..P9 10 - PA 15
S1 17 (264) - S2/S3 17 - S4/S5 18 - S6 20 (264) - S7 19 (268) - S8 21 (264)
S9 _dz int 22 - SA 23 (264)
W1 18 (264) - W2 15 (268) - W3 base+volatil 14 = BASE - W4 10 - W5 11 - W6 23
register en cada una de las 7 locales y en 4 combinaciones ... 17 = P2 (IGNORADO)
10 ordenes de inicializacion x 7 ordenes de declaracion sobre P2 ... 17 TODOS
```

**Dos vedas nuevas con cifra**:

- **mwcc pliega `(T)_v` cuando `_v` ya viene de un `T`** (L5/L6 = BASE): el
  `extsb` del objetivo **no** puede salir de un cast redundante.
- **`register` es un no-op** en este compilador: las 7 locales, una a una y en
  combinacion, dan el objeto identico.

**No se retiene** (una funcion al 98 % aporta cero bytes y la macro duplicada es
deuda). Las dos recetas se reaplican en un minuto.

---

## 3. `HandleTriggers` (588 B) --- reparto puro, y el mapa de la rotacion

Base `target=98.29932% size=588/588`, **42 filas**, **cero estructurales**.

Los tres valores que rotan son `cal->dead.left` (indices 0, 2, 12, 33),
`mxl` = `cal->max.left` (1, 4, 29, 34) y `cal->min.left` (24, 25, 28, 34):

```
             objetivo   nuestro
dead.left      r8         r7
mxl            r10        r8
min.left       r7         r10
vl             r9         r9     (casa)
```

Nosotros asignamos **en orden de definicion y ascendente** (r7, r8, r10); el
objetivo da **r8, r10, r7**. La mitad derecha repite la misma rotacion.

### 3.1 Formas de fuente --- 8, todas neutras o peores

```
X1 dl/dr locales para dead ............. 42 = BASE
X2 mxl asignado antes que vl ........... 42 = BASE
X3 decl vl/mxl/vr/mxr .................. 58   PEOR
X4 umbral en una local ................. 42 = BASE
X5 dead*2 -> dead+dead ................. 42 filas, 97,92517 % (peor fuzzy)
X6 comparacion invertida ............... 44   PEOR
X7 decl mxl/vl/mxr/vr .................. 42 = BASE
X8 dl local solo en la izquierda ....... 42 = BASE
```

Y sobre la macro `AUTOCALX` (que **solo** usa esta funcion):

```
6 ordenes de declaracion de _mn/_v/_dz/_r/_t ... 42 = BASE los seis
T _v tipo T ... 62 filas (604 B)   -   T _mn tipo T ... 54 (604 B)
T _dz int ..... 42 filas pero 96,05442 % (peor)
Y1 condicion con != 0 ......................... 42 = BASE
```

### 3.2 Pragmas --- 22, y la veda que cierra el eje

Con `#pragma push` / `#pragma pop` acotados a la funcion:

```
no_register_coloring on / off ......... 42 = BASE (IDENTICO)  <- NO-OP
register_coloring on / off ............ 42 = BASE (IDENTICO)  <- NO-OP
opt_lifetimes on / off ................ 42 = BASE (IDENTICO)  <- NO-OP
scheduling 750 ........................ 42 = BASE
optimize_for_size on .................. 42 = BASE
opt_dead_assignments off .............. 42 = BASE
load_store_elimination off ............ 42 = BASE
opt_propagation off ................... 42 = BASE
opt_strength_reduction off ............ 42 = BASE
opt_unroll_loops off .................. 42 = BASE
opt_loop_invariants off ............... 42 = BASE
ppc_unroll_speculative off ............ 42 = BASE
optimization_level 3 / 4 .............. 42 = BASE
scheduling off ........................ 56   PEOR
opt_common_subs off ................... 54 (604 B)
global_optimizer off .................. 54 (604 B)
peephole off .......................... 80 (668 B)
optimization_level 1 .................. 85 (660 B)
```

**`no_register_coloring` / `register_coloring` / `opt_lifetimes` son NO-OPS en
este mwcc.** Es la veda importante: **no hay palanca de pragma para el reparto
de registros de mwcc**, ni aqui ni en `CookValues`-P2, ni en `SimThread_Init`.

---

## 4. Lo demas de `LibSN/steering`

### 4.1 `Effect_PerformEnvelope` (196 contra 192): H1 da el tamano exacto

29 formas nuevas (van **70**: r22 10, r46 1, r48 12, r49 18, r50 29).

```
H1  mag = mag + v; out = mag;  (en la rama de fade) ... **196/196 B** 32 filas
H2/H3 mag redefinido en otras ramas .................. 31 = BASE
H4/H5/H6 out = abs(level) repetido ................... 35 (204 B)
H7 out = level; if (out<0) out = -out ................ 33 (204 B)
H8 out asignado en las cuatro ramas .................. 34 (188 B)
H9/HA mag u32 / out u32 .............................. 31 = BASE
HB/HC out = mag en sentencia aparte .................. 31 = BASE
J1..J3, J5..J7  out escrito ANTES de leer mag ........ 31 = BASE (los seis)
J4 out reusado como sustain .......................... 34 (196/196 B)
J8 out reusado como duration ......................... 36 (188 B)
V1/V3/V4/V5 elapsed/attackLevel/duration/fadeTime en local ... 31 = BASE
V2 attackTime en local ............................... 32
V6 neg con shift aritmetico .......................... 31 filas, fuzzy peor
V7 neg = level < 0 ................................... 31 = BASE
V8/V9 abs con mascara explicita ...................... 35
```

**H1 es la primera forma que produce la instruccion que falta** (196/196) --- pero
mwcc la emite como `add r0,r6,r0` + `mr r6,r0` en la rama de `fade`, y sigue
fundiendo `out` y `mag` en r6 en la cabecera.

**La condicion, reescrita con lo medido**: J1/J2/J3/J5/J6/J7 crean interferencia
real entre `out` y `mag` en la fuente (escribo `out` y **despues** leo `mag`) y
las seis dan el objeto **identico**. La conclusion es mas fuerte que la de la
r49: **no es que el coalescing sea legal, es que mwcc renumera las locales antes
del reparto y la interferencia escrita en C no llega al asignador.**

### 4.2 `SimThread_Init` (324 B, 9 filas): dos causas, 15 formas nuevas

Las 9 filas son dos cosas y una cascada:

1. **indices 51-55**: el objetivo emite `li r3,0` + `mtctr r0` **delante** del
   `lfs f1` de la escala `4096.0f`; nosotros al reves. Arrastra la fila 63
   (`bdnz`).
2. **indices 67/71/73/74**: permuta r5<->r6 en el **cuarto** bucle (el cero de
   `st->hist[i]` y el temporal de direccion). El segundo bucle, identico en
   forma, casa.

```
A1 bucle 3 indexado con i ........ 13 filas PEOR   A2 a[n]*4096 ....... 9 = BASE
A3 temporal float ................ 9 = BASE        A4 while ........... 9 = BASE
A5 n++ al final del cuerpo ....... 9 = BASE        A6 temporal s32 .... 9 = BASE
A7 bucle 4 con hist primero ...... 11 PEOR         A8 bucle 4 con n ... 9 = BASE
A9 bucle 4 con temporal .......... 9 = BASE        AA/AB decl ......... 9 = BASE
AC a[n]*4096+0.0f ................ 9 = BASE        AD i y n cruzados .. 13 PEOR
AE cero en una variable .......... 9 = BASE
```

Van **40 formas** en esta funcion (r47 14, r48 11, r50 15).

### 4.3 `SimThread_Step` (924 contra 920): los 4 B tienen indice

El deficit es **una copia de division de rango de vida**, no una operacion:

```
objetivo  37 li r8, 0x0      nuestro  37 li r8, 0x0
          38 mr r31, r7               38 li r7, 0x0
          40 slwi r0, r0, 2           39 li r5, 0x0
          41 mr r5, r8                40 slwi r0, r0, 2
          43 mr r6, r8
```

Los tres ceros los tenemos (r8/r7/r5 contra r8/r5/r6). Lo que falta es
**`mr r31, r7`**: el objetivo copia el valor a un preservado **y lo deja tambien
en r7**, que reusa acto seguido como contador (`lwz r7, 0xe8(r28)`); nosotros lo
ponemos ya en r31 y usamos r6 de contador. Es **la misma forma que el
`mr r5,r9` de `Effect_PerformEnvelope`**: mwcc parte un rango de vida y nosotros
lo fundimos. Los otros dos puntos (indices 149 y 208/222) son de la misma
familia: el objetivo mantiene el valor en un preservado (`r26`) y copia a `r5`
para la llamada; nosotros lo tenemos en `r5` y copiamos a `r27` despues.

### 4.4 `Effect_Init` (276 B, 31 filas): confirmada la lectura de la r49

Las 31 filas son **una** causa (`stmw r26` contra `stmw r27`): el objetivo guarda
`lis r27,0x4330` --- la parte alta del sesgo int->float--- **fuera** del bucle de
la tabla de senos, que contiene un `bl sin`, luego el valor **tiene** que ir a un
preservado. Nosotros lo rematerializamos dentro (`lis r0,0x4330`, indice 38).
No he abierto ensayos: la r49 barrio `optimization_level` 0-4 y seis `opt_*`, y
§3.2 acaba de cerrar el eje de pragmas para el reparto de mwcc en general.

---

## 5. La ley de `LibSN/steering`, y las cinco vedas que explica

Con las 44 formas de esta ronda que dan **el objeto identico**, la regla queda
escrita y es falsable:

> En `LibSN/steering` (mwcc), **cambiar QUE local sostiene un valor, el orden de
> las declaraciones, el orden de inicializaciones independientes o la forma
> sintactica de un bucle da el objeto BYTE A BYTE IDENTICO**. Solo mueven el
> objeto (a) anadir o quitar una OPERACION y (b) cambiar el TIPO de una local.

Evidencia directa, cuatro bloques independientes:

- `Effect_PerformEnvelope`: J1/J2/J3/J5/J6/J7 (seis reescrituras del portador) y
  HB/HC/H9/HA = BASE.
- `SimThread_Init`: A2..A6, A8, A9, AA, AB, AC, AE = BASE (once).
- `CookValues`-P2: **10 ordenes de inicializacion x 7 de declaracion, 17 filas
  las diecisiete**.
- `HandleTriggers`: seis ordenes de declaracion de la macro, X1/X2/X4/X7/X8 y Y1
  = BASE.

Y la excepcion, medida: **el TIPO si manda** --- `T _v` (21 filas), `T _mx` (22),
`T _mn` (54), `T _dz` -> `int` (fuzzy peor)---, porque cambia el numero de
conversiones, o sea el numero de operaciones.

**Ahorra rondas**: cinco vedas historicas de esta unidad ("swap decl", "partir la
cadena", "orden de sentencias", "reusar `level`", "bucle como `while`") no son
cinco hechos, son **un solo hecho** y ya no hay que remedirlas.

---

## 6. `ActualReadJoystickData` (1.588 B) --- 20 banderas en sombra, entero negativo

Arnes de sombra de la r49: compila la unidad real con cflags extra a un `.o` de
scratch (no toca produccion). Base 1580 B / 16 filas / 99,3199 %:

```
BASE (control) ................. 1588/1580  99,3199 %    16 filas
-fno-schedule-insns (CONTROL) .. 1588/1536  77,79597 %  275   <- cambia: arnes sano
-fno-schedule-insns2 ........... 1588/1556  81,25441 %  178
-fno-cse-skip-blocks ........... 1588/1612  86,29219 %  160
-fno-cse-follow-jumps .......... 1588/1592  92,64232 %  119
-fno-rerun-cse-after-loop ...... 1588/1588  84,967255 %  211   <- tamano EXACTO
-fno-gcse ...................... 1588/1592  89,68262 %  136
-fno-expensive-optimizations ... 1588/1572  93,0932 %   134
-fno-force-addr ................ 1588/1576  97,2141 %    50
-fno-move-all-movables ......... 1588/1544  83,29471 %  202
IDENTICAS a la base: -fno-force-mem, -fno-rerun-loop-opt, -fno-thread-jumps,
  -fno-strength-reduce, -fno-regmove, -fno-caller-saves, -fno-peephole,
  -fno-delayed-branch, -fno-function-cse, -fno-defer-pop
```

**Ninguna baja de 16 filas.** El eje de banderas queda cerrado para esta unidad
(20 medidas, con control que cambia). Y `-fno-rerun-cse-after-loop` da el
**tamano exacto** con 211 filas: quien busque por tamano se la come.

No he abierto formas de fuente nuevas: la r36b/r36f/r46/r47/r48 llevan 39 y la
r49 dejo el diagnostico cerrado (las 16 filas cuelgan de los mismos 8 B y el eje
que queda es la PRESION, no la forma del `if`).

---

## 7. `UpdateLoaded` (856 contra 860) --- 8 formas nuevas y el mecanismo afinado

El diff entero (21 filas) cuelga de que el objetivo emite **un** `lis r30,
lbl_803FB6B8@ha` **delante del `bl VU0_Atan2`** y lo comparte entre sus **dos
primeros** `lfs` de 1.0f; nosotros rematerializamos `lis r9,$LC917@ha` en cada
uno (+1 instruccion, +1 preservado, marco 0x30 contra 0x28).

**Afinado con el mapa de lineas del original**: el `lis r30` esta atribuido a
`SuspensionTraffic.cpp:288` --- **la misma sentencia que su primer uso**--- y esta
colocado en el bloque que DOMINA a los dos usos, delante de la llamada. Esa es la
firma de **PRE/`gcse`**, no de `cse2`: `gcse` inserta la computacion en el punto
mas temprano donde es anticipable. La medida de la r48 (`-fno-gcse` identico) no
refuta nada, porque mide NUESTRO objeto, donde no hay nada que compartir.

Los dos usos son `1.0f < UMath::Abs(fwd_vel)` (linea 350) y `skid_speed > 1.0f`
(357). Formas medidas, todas 856/860:

```
U1 skid como 1.0f < skid ......... 21 = BASE   U2 Abs > 1.0f ....... 21 = BASE
U3 las dos ....................... 21 = BASE   U4 orden de las condiciones ... 26 PEOR
U5 if anidado en 350 ............. 36 (852 B)  U6 const float one .. 21 = BASE
U7 la tercera comparacion ........ 21 = BASE   U8 la cuarta ........ 21 = BASE
```

**Veda nueva**: el ORDEN DE LOS OPERANDOS de una comparacion en punto flotante
es indiferente (GCC canonicaliza), y **una local `const float one = 1.0f` usada
en los dos sitios NO cambia nada** --- no comparte ni el valor ni la direccion---.
Con las 11 anteriores van **19 formas** en esta funcion.

---

## 8. Verificacion

```
git status --porcelain -- src/LibSN/steering.c src/Speed/GameCube/Src/JoyE.cpp \
    src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp
    ->  VACIO   (ninguna de mis tres fuentes tocada)

python scripts/build_direct.py LibSN/steering Speed/Indep/SourceLists/zPlatform \
    Speed/Indep/SourceLists/zPhysicsBehaviors \
    egami/rcmp/dev/source/decoder/cmn/madidct \
    Packages/vp6/1.0.6/source/decode/gc/criticalpath
    ->  5 ok, 0 fallidas

diff fncmp_before.txt fncmp_after.txt   ->  IDENTICO (las 5 unidades)
python scripts/lcfix.py --check         ->  todas las entradas @lc estan al dia
```

`fncmp` de las cinco, antes = despues:

```
LibSN/steering       6 de   36  --  2.568 B
zPlatform            1 de  136  --  1.588 B
zPhysicsBehaviors    1 de 1120  --    856 B
madidct              2 de    3  --  1.148 B
criticalpath         1 de   21  --    740 B
```

`fndiff` de las once, sin mover: ver la tabla de §1.

Mi scratchpad (`scratchpad/r50_plat/`) queda en **252 kB**: solo los arneses
`.py` y tres `fndiff` de referencia; los `.o` y `.json` de trabajo estan
borrados. **No he generado ni un volcado RTL.**

**Aviso de infraestructura**: `scratchpad/rtl/` tiene **39 MB** de volcados de
`cr_only_cpp` con fecha de HOY que **no son mios** (no he lanzado `rtldump` en
toda la ronda); los deja quien este trabajando con ese reproductor. El disco C:
bajo de 6,4 a 5,3 GB libres durante mi ronda.

---

## 9. Propuestas fuera de territorio

1. **Triaje por TIPO de diferencia, en una linea, para el catalogo** (§1):
   `python scripts/fndiff.py <unidad> <sim> | grep -c "INSERT\|DELETE\|REPLACE"`.
   Si sale **0**, la secuencia de instrucciones ya es la del objetivo y **no
   existe forma de fuente**: es reparto puro y hay que ir al asignador o dejarla.
   En mi territorio separa limpiamente `HandleTriggers` (0 de 42) de `madidct`
   (66 y 77). Encaja debajo de `triaje.py`, que hoy clasifica por otra cosa.
2. **`no_register_coloring`, `register_coloring` y `opt_lifetimes` son NO-OPS en
   el mwcc de GameCube** (§3.2, 22 pragmas con `push`/`pop` acotados). Cualquier
   near-miss de mwcc que este esperando "un pragma de reparto" puede dejar de
   esperarlo. Merece una linea en el catalogo de vedas.
3. **La ley de renumeracion de mwcc** (§5): en una unidad de mwcc, **el orden de
   declaraciones, el portador de un valor y la forma sintactica de un bucle no
   cambian el objeto**; solo el numero de OPERACIONES y el TIPO de las locales.
   Es barato de comprobar en otra unidad de mwcc (3 compilaciones) y, si se
   confirma, **caduca de golpe familias enteras de vedas** en todo el arbol de
   mwcc, no solo en `steering`.
4. **`register` no lo respeta mwcc** (7 locales x 4 combinaciones, objeto
   identico). Distinto de GCC, donde `register T x asm("rN")` si llega a
   `local_alloc` (memoria «el pin de registro»). Conviene que este escrito para
   que nadie gaste una ronda intentando el pin en una unidad de mwcc.
5. **`-fno-rerun-cse-after-loop` da el tamano EXACTO en
   `ActualReadJoystickData`** (1588/1588) con 84,97 % y 211 filas. Tercer caso
   de la temporada de tamano exacto por el motivo equivocado (r48 J4, r49 P4).
   La regla operativa: **el tamano solo vale como filtro cuando ya casan las
   filas**, nunca como senal.
6. **`UpdateLoaded` es un caso de PRE, no de `cse2`** (§7). Si alguien monta un
   experimento para reproducir la insercion de `gcse` --- por ejemplo forzando que
   la constante sea anticipable en el bloque dominante---, la funcion cae y con
   ella 856 B. La medida de la r48 «`-fno-gcse` identico» **no cierra este eje**:
   mide nuestro objeto, que no tiene nada que compartir.
7. **La familia «la copia que parte el rango de vida»** aparece ya en tres
   funciones de mwcc de mi territorio --- `Effect_PerformEnvelope` (`mr r5,r9`),
   `SimThread_Step` (`mr r31,r7`, y otras dos del mismo tipo) y `CookValues`
   (`extsb` rematerializado en vez de copia)---. En las tres, **al objetivo le
   sobra una copia o una rematerializacion que a nosotros nos falta**, y en las
   tres el resto del diff es cascada. Con la propuesta 3 confirmada, es el unico
   frente vivo que le queda a mwcc. **No lo llamo frente todavia**: son tres
   casos en UNA unidad; hay que contarlo en las demas unidades de mwcc antes.

---

## 10. Utiles (en `scratchpad/r50_plat/`, 252 kB, fuera del arbol)

| fichero | que |
|---|---|
| `lib.py` | arnes de variante (parche + `build_direct` + `fndiff`), restaura siempre |
| `e1.py`, `e3.py`, `e4.py` | los 29 ensayos de `Effect_PerformEnvelope` |
| `e2.py` | vuelca el `fndiff` completo de una variante con nombre |
| `si1.py` | los 15 de `SimThread_Init` |
| `ck1..ckb.py` | los ~60 de `CookValues` (incluidas las recetas N8 y P2) |
| `ht1.py`, `ht2.py`, `ht3.py` | los 18 de fuente y los 22 pragmas de `HandleTriggers` |
| `ul1.py` | los 8 de `UpdateLoaded` |
| `joyflags.py` | el barrido de 20 banderas en sombra de `zPlatform` |
| `joy_base.txt`, `si_base.txt`, `ss_base.txt`, `ht_base.txt`, `ul_base.txt` | los `fndiff` citados |
| `logs/fncmp_before.txt`, `logs/fncmp_after.txt` | la verificacion |
