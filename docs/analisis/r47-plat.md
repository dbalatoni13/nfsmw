# Ronda 47 --- territorio `plat`

`LibSN/steering` + `Speed/Indep/SourceLists/zPlatform` +
`Speed/Indep/SourceLists/zPhysicsBehaviors`. Base: 5.012 B en 6 funciones.

**No cierra ninguna funcion.** Lo que sale de la ronda es (a) un avance
estructural medido en `ActualReadJoystickData` --- **19 filas distintas -> 16 y
99,24433 % -> 99,3199 %** --- que viene de escribir la fuente con **las
estructuras que declara el DWARF del original**, (b) el eje de compilador y
banderas de `steering` **cerrado con 67 medidas nuevas**, y (c) el mecanismo
exacto de `UpdateLoaded` **probado en los volcados RTL**, no inferido.

| unidad | antes | despues |
|---|---:|---:|
| `LibSN/steering` | 6 fn / 2.568 B | igual |
| `zPlatform` | 1 fn / 1.588 B | igual (fuzzy 99,24433 -> **99,3199 %**, 19 -> **16** filas) |
| `zPhysicsBehaviors` | 1 fn / 856 B | igual |

Fichero tocado: **`src/Speed/GameCube/Src/JoyE.cpp`** (sin `asm`, sin literales
nuevos, sin tocar cabeceras compartidas).

---

## 1. `steering` NO es un caso `OdemuExi2`: es UNA sola TU, y con 28 versiones

El encargo pedia comprobar si `steering` se parte como se partio `OdemuExi2`.
**No.** Barrido de **28 versiones** sobre la fuente ACTUAL (las 19 de `GC` mas
**las 9 de `Wii`, que nadie habia barrido**; `scratchpad/r47_plat/sw.py`):

```text
GC\1.0 .. 1.2.5n     34/36  8.668 B
GC\1.3               11/36  4.676 B
GC\1.3.2 / 1.3.2r     7/36  3.376 B
GC\2.0 .. 2.7         6/36  2.568 B   <- optimo, empate de cinco versiones
GC\3.0a3 .. 3.0a5.2  33/36  8.544 B
Wii\0x4201_127 .. 1.7 33/36  8.544 B  (identicas a 3.0a*)
```

Lo decisivo es que **los conjuntos de fallos son ANIDADOS, nunca disjuntos**:
`2.0-2.7` = {SimThread_Step, HandleTriggers, SimThread_Init, Effect_Init,
CookValues, Effect_PerformEnvelope}; `1.3.2` = ese conjunto **mas**
`Effect_UpdateEffect`; `1.3` = ese mas `Effect_Update`, `Effect_StartEffect` y
`Effect_StopEffect`. En `DebuggerDriver` los dos conjuntos eran **disjuntos y
separados por una direccion**, que es la firma del objeto con varias TU. Aqui
no hay ninguna frontera: **no hay corte que proponer en `splits.txt`.**

## 2. El eje de cflags de `steering`, cerrado con 39 combinaciones nuevas

`scratchpad/r47_plat/e3.py` (las 9 de la r36e no se repiten). **Ninguna mejora**;
las 24 que no aparecen abajo dan EXACTAMENTE la base (6/36, 2.568 B):

```text
(base -O4,s)            6/36   2.568 B
sdata 0                 9/36   2.952 B      sdata2 0      7/36   2.784 B
fp_contract off         7/36   2.784 B      lmw off      12/36   3.892 B
O4,p                   19/36   6.480 B      align mac68k 27/36   7.448 B
O2,s                   35/36   8.740 B
-ipa file / -ipa function / -opt unroll / -volatileasm / -fp none   NO COMPILAN
```

Neutras (mismo objeto que la base): `opt no_lifetimes`, `opt lifetimes`,
`opt no_global`, `opt no_peep`, `opt no_propagation`, `opt no_strength`,
`opt no_loopinvariants`, `opt no_dead`, `opt no_deadassign`, `opt no_bitfield`,
`opt schedule`, `opt intrinsics`, `opt nointrinsics`, `opt nounroll`, `sdata 8`,
`common on`, `func_align 4`, `fp fmadd`, `char signed`, `schedule on`,
`str pool,reuse`, `O4` pelado, `inline auto/on/deferred`, `enum min`, quitar
`-inline off`.

Con esto **el eje de compilador y banderas de `steering` esta agotado**: 28
versiones x fuente actual + 48 conjuntos de banderas + los 232 `#pragma` por
funcion de la r46.

## 3. `steering`: 19 formas de fuente nuevas, todas negativas

Herramienta: `scratchpad/r47_plat/t.py` (una variante = **0,8 s**, no toca el
arbol). Base 2.568 B, 6 funciones.

### 3.1 `CookValues` (260 B, 13 insn) --- la hipotesis del `_v` tipado, muerta

Diagnostico: al objetivo le vive el **byte crudo** de `_v` (`lbz r5` / `extsb
r6,r5`, y luego `stb r5` y **`extsb r0,r5`** para `_mx = _v`), mientras nosotros
machacamos el crudo (`lbz r6` / `extsb r6,r6`) y copiamos (`mr r0,r6`). Eso es
lo que hace una local declarada del tipo estrecho, como el `_dz` de la macro.
**Medido y falso**:

| forma | total | CookValues | dano colateral |
|---|---:|---|---|
| `AUTOCAL`: `int _v` -> `T _v` | 3.080 B | 20 insn (era 13) | rompe `HandlePedals` (520/512) |
| `AUTOCALX`: `int _v` -> `T _v` | 2.568 B | --- | `HandleTriggers` 604/588 (era 42 insn) |
| las dos a la vez | 3.080 B | 20 insn | las dos |
| `{ (mx) = _v; _mx = (mx); }` | 3.080 B | 264/260 | `HandlePedals` 2 insn |
| `{ (mx) = (vv); _mx = _v; }` | 3.080 B | 264/260 | `HandlePedals` 520/512 |
| `{ (mx) = (vv); _mx = (vv); }` | 3.080 B | 268/260 | `HandlePedals` 520/512 |
| `{ (mx) = (vv); _mx = (mx); }` | 3.080 B | 268/260 | `HandlePedals` 520/512 |

**Veda**: la forma actual de `AUTOCAL`/`AUTOCALX` es la buena; cualquier cambio
en el cuerpo de la macro rompe `HandlePedals`, que hoy casa al 100 %. La
diferencia de `CookValues` es un **empate del asignador**, no la macro.

### 3.2 `SimThread_Init` (324 B, 8 insn) --- 9 formas

Dos problemas: (1) el `lfs` de la escala `4096.0f` cae **dos ranuras antes** que
en el objetivo; (2) empate desnudo r5/r6 en el ultimo bucle.

```text
A  ia[n] = (s32)(a[n] * 4096.0f)          8 insn  = BASE
C  declarar n antes que i                  8 insn  = BASE
D  declarar impulse[] primero              8 insn  = BASE
E  cero explicito en una local `z`         8 insn  = BASE
F  intercambiar los dos stores del bucle 4 10 insn PEOR
G  usar `n` como indice del bucle 4        8 insn  = BASE
H  `*(volatile float *)&a[n]`              8 insn  = BASE
I  bucle 3 como do-while                   328/324 PEOR
J  bucle 3 descendente                     332/324 PEOR
```

Con las de la r47/r48 anteriores van **catorce** formas medidas en esta funcion.

### 3.3 `Effect_Init` (276 B, 26 insn) --- 7 formas

Diagnostico firme: al objetivo le vive el **0x43300000** del conversor
int->float en un **SEXTO preservado (r27) izado fuera del bucle** (`stmw r26`
contra nuestro `stmw r27`), y nosotros lo rematerializamos dentro (`lis r0,
0x4330`). Las 20 filas restantes son la renumeracion r26..r31 que eso arrastra.

```text
N1 sin `off` (sine[i] / ramp[i])           272/276  PEOR
N2 `float fi = (float)i` local             284/276  PEOR
N3 `i++` al final del cuerpo               26 insn  = BASE
N4 `for` en vez de do-while                280/276  PEOR
N5 sin `phase` (phase = (i-1)*0x400)       37 insn  PEOR
N6 el store de `ramp` primero              280/276  PEOR
N7 segunda conversion int->float           316/276  PEOR (SI iza, pero cuesta 40 B)
```

N7 es el unico dato con futuro: **una segunda conversion en el bucle SI provoca
el izado** de la constante, o sea que el mecanismo esta identificado; lo que no
hay es una segunda conversion legitima en esta fuente.

---

## 4. `ActualReadJoystickData` (1.588 B): 19 filas -> 16, y por que

### 4.1 El hallazgo: el DWARF trae las ESTRUCTURAS del original

`symbols/mw_dwarfdump.nothpp`, linea 1851117. Ademas de las locales
(`nNewTop` r1+8, `port` r26, `joy_data` r29, `slot` r24, `pad_state`, y `short
data` r0 en 0x8026C6B4..0x8026C878) el volcado trae **los tipos**:

```c
struct JoyData {                    // 0x3C
    struct PadData ThePadData[4];   // 0x00, 0x28
    unsigned char Bytes[4];         // 0x28
    int RegularControllerType;      // 0x2C
    struct PADStatus padSTATUS;     // 0x30, 0x0C
};
struct PadData {                    // 0x0A
    unsigned short DigitalButtons;  // 0x0
    unsigned char LTrigger, RTrigger;            // 0x2, 0x3
    unsigned char AnalogLeftX, AnalogLeftY;      // 0x4, 0x5
    unsigned char AnalogRightX, AnalogRightY;    // 0x6, 0x7
    unsigned char Error, Type;                   // 0x8, 0x9
};
```

O sea que **`slot` no indexa un puntero casteado: indexa un ARRAY MIEMBRO**, y
todas las escrituras de eje son `joy_data->ThePadData[slot].X`, tanto en el
bloque `pad_state == 0` como en la rama del volante.

### 4.2 Lo que arregla, y por que

Escribirlo asi tumba **las tres filas de `add`/`sthx` con los operandos al
reves** (`add r31, r29, r27` del objetivo contra nuestro `add r31, r27, r29`, y
lo mismo en `sthx r0, r29, r27` y `add r10, r29, r27`).

La razon es del expansor: `((LGPosition *)(void *)joy_data + slot)` es un
`PLUS_EXPR` que `expand_binop` emite con los operandos en el orden en que los
saca; `joy_data->ThePadData[slot]` pasa por `get_inner_reference` y la direccion
se construye como `(plus base offset)` --- **base primero**, que es la forma del
objetivo.

**Medido, y es el detalle que importa**: la forma `[slot]` sobre el puntero
casteado (`((LGPosition *)(void *)joy_data)[slot].X`) **NO vale**: da las mismas
19 filas. Hace falta que el array sea un **MIEMBRO de la estructura**.

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base | 1.580 B | 99,24433 % | 19 |
| J1 `((LGPosition *)joy_data)[slot].X` | 1.580 B | 99,24433 % | 19 |
| J3 puntero `pad_slot` izado (un solo `add`) | 1.572 B | 90,28716 % | 129 |
| K2 solo el bloque `pad_state == 0` | 1.580 B | 99,24433 % | 19 |
| **K3/K4 `joy_data->ThePadData[slot]` en las dos ramas** | **1.580 B** | **99,3199 %** | **16** |

K4 = K3 con los nombres del DWARF (`JoyData`, `PadData`, `padSTATUS`); el
renombrado es **neutro en codigo**, comprobado. Es lo que queda en el arbol.

### 4.3 Las 16 filas que quedan, en tres grupos

```text
G1 (6)  lis r18/r19 intercambiados: RealTimer@ha contra la constante 0x43300000
G2 (4)  el `data` del bloque substickY: objetivo r10, nosotros r8
G3 (6)  el `data` del bloque stickX: objetivo r11, nosotros r8
        + las dos que faltan:  mr r9,r0  y  andi. r11,r9,0x8000
```

**G1 esta diagnosticado con numeros** (`python scripts/lreg.py
Speed/Indep/SourceLists/zPlatform ActualReadJoystickData`):

```text
pseudo 387   n_refs 5   live_len 638   prioridad 156   -> r18   GENERAL_REGS   (0x43300000)
pseudo 384   n_refs 5   live_len 640   prioridad 156   -> r19   BASE_REGS      (high(RealTimer))
```

`allocno_compare` calcula `floor_log2(n_refs)*n_refs/live_length*10000*size`:
156,74 y 156,25, que **truncados a int EMPATAN a 156**. El desempate es
`v1 - v2`, el numero de allocno, y como `REG_ALLOC_ORDER` de rs6000 recorre los
GPR **descendiendo**, el que se asigna PRIMERO se lleva **r19** y el segundo
r18. Nosotros asignamos primero `high(RealTimer)` (pseudo 384 < 387); el
objetivo asigna primero la constante.

Para voltearlo harian falta: o un pseudo de constante creado **antes** que la
direccion --- imposible, `expand_float` crea la constante DESPUES de expandir el
operando que convierte, y ese operando es justo la carga de `RealTimer`, y no
hay otra conversion int->float en esa rama ---, o bajar `n_refs` de RealTimer a
1, que exige una sola llamada a `GetSeconds()` cuando **el objetivo hace DOS
cargas**. **Sin palanca desde la fuente.**

### 4.4 El pin dice que el reparto es un SINTOMA

Palanca 3 aplicada a los dos `data` (`register int data asm("r10"/"r11")`):

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base K4 | 1.580 B | 99,3199 % | **16** |
| pin r10 (substickY) | 1.568 B | 98,37531 % | 30 |
| pin r11 (stickX) | 1.576 B | 98,90428 % | 26 |
| los dos pines | 1.572 B | 98,66499 % | 27 |

Los tres **empeoran y ademas ACORTAN el objeto**: G2 y G3 no son un empate
suelto, cuelgan del par que falta. Confirma la regla «si el pin empeora, es
sintoma»: hay que arreglar antes la primera diferencia que **no** es de
registro, que es `mr r9,r0` + `andi. r11,r9,0x8000`.

### 4.5 Lo demas medido y negativo sobre la base NUEVA

La veda de la r36f («una sola `short data`») **remedida sobre K4**, porque el
DWARF la pide y las vedas caducan:

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| N1 una `short data` + `if (data & 0x8000)` tras el store | 1.564 B | 96,46096 % | 48 |
| N2 igual con `(short)data > 255` | 1.564 B | 96,46096 % | 48 |
| N3 una `short data` sin ese `if` | 1.564 B | 95,69270 % | 71 |
| M1 `calibrationTimer=7.0f` detras del `GetSeconds` | 1.580 B | 98,96726 % | 22 |
| M2 `lastCalibTime` delante del descuento | 1.564 B | 98,12343 % | 31 |
| M3 `-=` compuesto | 1.580 B | 99,3199 % | 16 (= base) |
| M4 `float now = RealTimer.GetSeconds()` local | 1.580 B | 99,3199 % | 16 (= base) |

**La veda se confirma con un dato util**: con `short data` aparece un `extsh`
DE MAS justo detras de cada aritmetica (el truncado en la asignacion) y
desaparecen el `mr rX,r0` + `extsh r0,rX` que el objetivo SI tiene tras la
fusion del `if`. O sea que la forma del objetivo es **`int` con el cast en la
comparacion** --- `int data = v; if ((short)data > 255)` ---, que es justo la que
tenemos. El `short data` del DWARF es como el compilador original NOMBRO esa
local, no la forma que hay que escribir.

Van **treinta y una** formas medidas en esta funcion (r36: 5, r36b: 5, r36f: 4,
r46: 5, r47: 12).

---

## 5. `UpdateLoaded` (856 B): el mecanismo, ya no inferido sino leido en el RTL

La r41/r44/r46 dejaron la hipotesis «PRE crea el pseudo y cse2 lo deshace».
**Confirmada en los volcados** (`build/GOWE69/lreg/*.i.{loop,cse2,lreg}`,
extraidos en `scratchpad/r47_plat/ul_*.txt`):

```text
.loop   (tras gcse/PRE)   high($LC917) esta en UN solo pseudo, el 362,
                          con DOS insns de insercion (988 y 991) y CINCO copias
                          (set (reg N) (reg 362)) + REG_EQUAL (high $LC917)
                          para N = 238, 291, 306, 338, 351
.cse2                     las cinco copias se han convertido en
                          (set (reg N) (high $LC917)) DIRECTO; el 362 se queda
                          con un solo uso
.lreg                     cuatro pseudos (362, 238, 291, 351), cada uno con
                          UN def y UN uso
```

Y la condicion exacta esta leida en `orig/prodg/NGC_GNU_SRC/NGC/gcc/local-alloc.c`,
`update_equiv_regs`:

```c
if (REG_N_REFS (regno) == 2
    && REG_BASIC_BLOCK (regno) < 0
    && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
  reg_equiv_replace[regno] = 1;
```

es decir: **un def + un uso y usado en mas de un bloque** -> se hunde la
definicion al uso y el `lis` se rematerializa. Con DOS usos el pseudo sobrevive
y `global_alloc` le da un preservado --- que es el `lis r30` de la fila 86 del
objetivo. **El objetivo conserva DOS copias; nosotros ninguna.**

Formas de fuente medidas esta ronda (24 s cada una, unidad real; base 856/860 B,
97,21028 %, 21 filas de `fndiff`):

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| U1 primer `&&` como `if` anidado | 864 B | 94,21963 % | 52 |
| U2 segundo `&&` como `if` anidado | 860 B | 97,21028 % | 21 (= base) |
| U3 los dos anidados | 864 B | 94,21963 % | 52 |
| U4 `UMath::Abs(fwd_vel) > 1.0f` | 860 B | 97,21028 % | 21 (= base) |
| U5 `1.0f < skid_speed` | 860 B | 97,21028 % | 21 (= base) |

La **palanca 12** (la forma del bucle decide el idioma de la rama) **no aplica
aqui**: el idioma `fcmpu + cror + bso` ya sale igual en las dos, y desanidar el
`&&` empeora 31 filas.

Y el DWARF (`mw_dwarfdump.nothpp:1748848`) confirma que **la fuente ya es la
del original**: `float slip_speed /* f11 */`, `float catchupfriction`,
`float skid_speed /* f1 */`, y ni una local mas. No hay nada que anadir ni que
quitar: lo unico que separa a esta funcion del 100 % es que cse2 folde **una
copia menos**.

---

## 6. Verificacion

```text
git status --porcelain   ->  M src/Speed/GameCube/Src/JoyE.cpp   (y nada mas mio)
                             steering.c y SuspensionTraffic.cpp SIN TOCAR

python scripts/build_direct.py LibSN/steering \
       Speed/Indep/SourceLists/zPlatform Speed/Indep/SourceLists/zPhysicsBehaviors
   -> 3 ok, 0 fallidas

fncmp LibSN/steering          6 de   36  --  2.568 B   (= base)  + 3 alias / 488 B
fncmp zPlatform               1 de  136  --  1.588 B   (= base)  + 42 alias / 17.796 B
fncmp zPhysicsBehaviors       1 de 1120  --    856 B   (= base)  + 341 alias / 164.444 B

fndiff zPlatform ActualReadJoystickData__Fv
   -> target=99.3199%  ours=99.3199%  size=1588/1580   16 filas   (antes 99,24433 %, 19)

python scripts/lcfix.py --check   -> todas las entradas @lc estan al dia
git diff --check                  -> limpio; JoyE.cpp sigue LF entero (0 CR)
```

**Ninguna funcion empeora en ninguna de las tres unidades**, ni las 135 vecinas
de `zPlatform` ni las 1.119 de `zPhysicsBehaviors`.

## 7. Propuestas fuera de territorio

1. **El DWARF trae los TIPOS, no solo las locales.** `mw_dwarfdump.nothpp`
   imprime las `struct` completas del original con offsets y tamanos justo
   detras de cada funcion que las usa. En esta ronda eso valio **3 de 19 filas**
   en una funcion que llevaba 24 formas de fuente medidas, y el motivo es de
   expansor: un ARRAY MIEMBRO da `(plus base offset)` y un puntero casteado mas
   indice da el `plus` al reves. **Merece un barrido**: cualquier near-miss con
   filas `ARG_MISMATCH` en un `add`/`stX` indexado es candidato. La regla corta:
   *si el objetivo pone la BASE primero y nosotros el indice, la fuente del
   original indexa un array miembro.*
2. **`triaje.py`/`fncmp.py` siguen sin ver `.over`** (aviso ya en el brief).
3. **`steering` no admite corte**: queda descartado con las 28 versiones. Si
   alguien quiere volver a esa unidad, el unico frente vivo es `Effect_Init` y
   pasa por conseguir que MWCC ice el `0x43300000` a un sexto preservado; N7
   demuestra que una segunda conversion int->float en el bucle lo consigue.
4. **`UpdateLoaded` necesita tocar `cse2`, no la fuente.** Con `-fgcse` el PRE
   ya crea el pseudo compartido; lo unico que hace falta es que a `cse2` se le
   escapen DOS de las cinco copias. Desde C no hay forma: las cinco copias son
   identicas y `cse_insn` las trata igual. Si en algun momento se admite un
   cambio de banderas POR OBJETO, `-fno-rerun-cse-after-loop` es la medida a
   hacer (no la he hecho: cambiar cflags esta fuera de mi territorio).

## 8. Utiles (en `scratchpad/r47_plat/`, no en el arbol)

| fichero | que |
|---|---|
| `t.py` | variante de `steering` compilada y comparada en **0,8 s**, sin tocar el arbol |
| `g.py` + `fndiff2.py` | lo mismo para unidades de GCC: edita el `.cpp` del arbol, compila la SourceList a scratch (**no** pisa `build/GOWE69/src`) y **restaura la fuente pase lo que pase** |
| `sw.py` | barrido de versiones que ademas lista QUE funciones fallan (para ver si son disjuntas) |
| `e1..e5.py` | los barridos de `steering` (macros, cflags, SimThread_Init, Effect_Init) |
| `p1..pb.py` | los de `zPlatform` |
| `q0/q1.py` | los de `zPhysicsBehaviors` |
| `ul_loop.txt`, `ul_cse2.txt`, `ul_lreg.txt` | el RTL de `UpdateLoaded` en las tres etapas |
| `joy_base.txt`, `joy_k3.txt`, `joy_N1.txt` | los `fndiff` completos de las tres bases |
