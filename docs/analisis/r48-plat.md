# Ronda 48 --- territorio `plat`

`LibSN/steering` + `Speed/Indep/SourceLists/zPlatform` +
`Speed/Indep/SourceLists/zPhysicsBehaviors`. Base: 5.012 B en 8 funciones.

**No cierra ninguna funcion y no se retiene ningun cambio de fuente**: las tres
unidades quedan byte por byte como estaban (verificacion en §5). Lo que sale de
la ronda son tres cosas medidas:

1. **`ActualReadJoystickData`: los 8 B que faltan tienen NOMBRE y LINEA.**
   `lmap.py` sobre el ORIGINAL los imputa a la **linea 307** del `JoyE.cpp` de
   EA: `mr r9,r0` + `andi. r11,r9,0x8000`, o sea **un AND MUERTO sin su rama**.
   Y once formas en un reproductor minimo demuestran que **ninguna forma de
   `if` en C lo produce** con este cc1: el `andi.` solo sobrevive con su `bc`.
2. **G1 --- las 6 filas de `r18`/`r19` --- NO es «sin palanca desde la fuente»,
   como concluyo la r47.** Es un empate de `allocno_compare` que se rompe SOLO
   con que la funcion crezca UNA instruccion dentro del bucle de puertos.
   Medido: con un `nop` las filas 40 y 41 pasan a ser identicas.
3. **La propuesta 4 de la r47 (`-fno-rerun-cse-after-loop` por objeto para
   `UpdateLoaded`) esta MEDIDA EN SOMBRA y es NEGATIVA**: 848 B, 79 filas,
   84,67 % contra las 21 filas de la base. No hay que pedir ese cambio de
   `configure.py`.

| unidad | antes | despues |
|---|---:|---:|
| `LibSN/steering` | 6 fn / 2.568 B | igual |
| `zPlatform` | 1 fn / 1.588 B | igual (16 filas, 99,3199 %) |
| `zPhysicsBehaviors` | 1 fn / 856 B | igual (21 filas, 97,21028 %) |

Ficheros de fuente tocados: **ninguno**. Todo se midio con copias y con los
arneses de `scratchpad/r48_plat/`, que restauran el arbol pase lo que pase.

---

## 1. `ActualReadJoystickData` (1.588 B): las dos instrucciones tienen linea

### 1.1 `lmap.py` sobre el ORIGINAL cierra la pregunta de QUE falta

```text
python scripts/lmap.py Speed/Indep/SourceLists/zPlatform ActualReadJoystickData__Fv
```

El mapa de lineas del ELF original (`scratchpad/r48_plat/joy_lmap.txt`) da la
linea de fuente de CADA instruccion del objetivo, y el bloque `pad_state == 0`
sale con una estructura de **tres lineas por eje**:

```text
294  lbz 0x34(r29) + conversion             (substickX)
295  addi +0x80 / andi. / beq / li 0 / mr / extsh / cmpwi / ble / li 0xff
296  stb r8, 0x6(r29)                       AnalogRightX
297  lbz 0x35(r29) + conversion             (substickY)
298  subfic 0x80 / ...los mismos clamps...
299  stb r10, 0x7(r29)                      AnalogRightY
304  lbz 0x32(r29) + conversion             (stickX)
305  addi +0x80 / ...clamps... / extsh r0,r11
306  stb r0, 0x4(r29)                       AnalogLeftX
307  mr r9, r0                              <-- LAS DOS QUE FALTAN
307  andi. r11, r9, 0x8000                  <--
308  lbz 0x33(r29) + conversion + subfic    (stickY)
309  stb r9, 0x5(r29)                       AnalogLeftY
317  LTrigger        318  RTrigger
```

O sea: la linea 307 del original es el `if (v & 0x8000) { v = 0; }` que nuestra
fuente ya tiene detras del store de `AnalogLeftX`, y **emite exactamente dos
instrucciones: la copia y el AND, SIN la rama y SIN el `li 0`**. Nosotros lo
borramos entero por muerto. Esos son los 8 B (1.580 contra 1.588).

Dos detalles que el mapa regala y no estaban escritos:

- Los huecos de linea son informacion: 300-303 (4 lineas) y 310-316 (7 lineas)
  no emiten NADA, y 272-289 son la expresion de `DigitalButtons`. La fuente de
  EA es bastante mas verbosa que nuestra reconstruccion.
- En PowerPC **no existe `andi` sin punto**: `andi.` es la unica forma de AND
  con inmediato, asi que un `andi.` sin rama detras no implica un `if` --- puede
  ser una asignacion cualquiera. Aqui el destino (r11) esta muerto igualmente.

### 1.2 El reproductor minimo: ninguna forma de `if` deja el AND solo

`scratchpad/r48_plat/mini/sw.py` compila con **los cflags reales de zPlatform**
(ProDG 3.9.3, `-O1 -ffast-math -fgcse -frerun-cse-after-loop -fschedule-insns
...`) un cuerpo de 20 lineas que reproduce el patron. Once formas:

```text
A  if (v & 0x8000) { v = 0; }                24 insn  1 andi.   <- el AND DESAPARECE
B  ...  + un uso posterior de v              29 insn  2 andi.   con bc y li
C  if ((v & 0x8000) != 0) { v = 0; }         24 insn  1 andi.
D  v = (v & 0x8000) ? 0 : v;                 24 insn  1 andi.
E  cuerpo con store en vez de v = 0          29 insn  2 andi.   con bc
F  while (v & 0x8000) { v = 0; }             24 insn  1 andi.
G  else vacio                                24 insn  1 andi.
H  asm("" : : "r"(v)) detras                 26 insn  2 andi.   con bc y li
I  asm("" : "+r"(v)) detras                  24 insn  1 andi.
J  todo dentro de un bucle                   28 insn  1 andi.
K  en bucle + uso posterior                  33 insn  2 andi.   con bc
```

**Regla que queda medida**: con este cc1 el `andi.` **siempre viene con su
`bc`**. O el valor esta muerto y se van los tres (AND, rama y `li`), o esta vivo
y salen los tres. El estado del original --- AND vivo, rama muerta --- solo se
alcanza si la rama se borra DESPUES de `flow`, y desde C no hay forma de pedirlo.

Lo mismo medido sobre la unidad de verdad (6 formas, `p1.py`):

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base | 1.580 B | 99,3199 % | **16** |
| J1 `asm("" : : "r"(v))` detras del `if` | 1.612 B | 87,98 % | 149 |
| J2 `v` reusada para `AnalogLeftY` | 1.580 B | 98,98 % | 35 |
| J3 ternario `v = (v & 0x8000) ? 0 : v` | 1.580 B | 99,3199 % | 16 (= base) |
| J4 `asm("" : : "r"(v))` tras el bloque de `AnalogLeftY` | **1.588 B** | 87,35 % | 137 |
| J5 `asm` delante del store | 1.600 B | 88,05 % | 150 |
| J6 el `if` delante del store | 1.592 B | 98,19 % | 42 |

J4 da el tamano exacto pero por el motivo equivocado: emite `andi.` **mas**
`beq` **mas** `li` y a cambio pierde tres copias muertas.

### 1.3 EL HALLAZGO: G1 no es un empate irrompible, es la LONGITUD DE VIDA

La r47 dejo escrito que las 6 filas de `r18`/`r19` («o un pseudo de constante
creado antes, imposible, o bajar `n_refs` de RealTimer a 1») no tenian palanca
desde la fuente. **Es falso, y la cuenta lo dice.** `allocno_compare` de
`global.c` calcula

```c
pri = ((double)(floor_log2 (n_refs) * n_refs) / live_length) * 10000 * size;
```

y con los numeros que la r47 midio (`n_refs` 5 en los dos; `live_length` 640 para
`high(RealTimer)` y 638 para `0x43300000`) sale **100000/640 = 156,25 -> 156** y
**100000/638 = 156,74 -> 156**: empatan al truncar, y el desempate es el numero
de allocno. Pero **el empate solo existe en esa ventana de longitudes**: basta
que las dos crezcan una o dos posiciones para que 155 y 156 dejen de empatar y
se asigne primero la constante, que es lo que hace el objetivo.

Medido (`p3.py`, un `asm volatile("nop")` detras del store de `AnalogLeftX`):

| variante | tamano | fuzzy | filas | filas 40/41 |
|---|---:|---:|---:|---|
| base | 1.580 B | 99,3199 % | 16 | **r19/r18 -- MAL** |
| N1 un `nop` | 1.584 B | 99,35768 % | 21 | **r18/r19 -- BIEN** |
| N2 dos `nop` | **1.588 B** | 99,1058 % | 22 | **r18/r19 -- BIEN** |

O sea: **arreglar los 8 B arregla G1 solo, gratis**. Y explica por que los pines
de registro de la r47 empeoraban: acortaban el objeto, que es la direccion
contraria.

### 1.4 Lo que aparece al crecer: hay TRES grupos de empate, no uno

Al meter la instruccion se rompen dos grupos que hoy estan BIEN:

```text
r14/r15   lbl_80403108@ha (el 0x4330000080000000) contra wasWheelConnected@ha
          -> 5 filas (25, 27, 57, 112, 243)
f25/f26/f27  rotacion de las tres constantes flotantes izadas
          -> 5 filas (49, 53, 54, 252, 261/273)
```

Los tres grupos --- `r18/r19`, `r14/r15`, `f25..f27` --- son **pseudos de
invariante de bucle izados al precabezal**, y los tres se reparten por el mismo
empate de `allocno_compare`. El estado de cada uno depende de la longitud exacta
del cuerpo del bucle. Hoy tenemos 2 de 3 bien; con +1 insn, 1 de 3.

### 1.5 La palanca 5 medida hasta el final: el `asm` que emite las dos

`p4.py` (`asm` NO volatil con cuerpo) no sirve: un `asm` con salidas muertas es
un insn muerto y GCC lo borra --- la variante «con el `if`» da el objeto de la
base clavado. Con `asm volatile` y los registros escritos a mano (`p5.py`):

| variante | tamano | fuzzy | filas |
|---|---:|---:|---:|
| X1 en lugar del `if` | 1.584 B | 98,99 % | 23 |
| **X2 `asm volatile("mr 9,0; andi. 11,9,0x8000")` detras del `if`** | **1.588 B** | 99,25693 % | 21 |
| X3 delante del store | 1.588 B | 99,25693 % | 21 |

En X2 **las dos instrucciones casan** (desaparecen del diff) **y G1 casa**, pero
la barrera cuesta 11 filas: mueve el `stb r0,0x4(r29)` dos ranuras (2 filas) y
rompe `r14/r15` y `f25..f27` (10 filas). Base 16 < X2 21, asi que **se
revierte** (regla 9 del brief: cero bytes con `asm` puesto es deuda).

**Lo que queda por probar y no he sabido montar**: meter esas dos instrucciones
**sin barrera**. Un `asm` con salida se borra por muerto; uno sin salidas es
volatil por definicion en GCC. Las dos salidas posibles son (a) que la fuente
las produzca de verdad --- y §1.2 dice que ninguna forma de `if` lo hace --- o
(b) un `asm` cuya salida se consuma en algo que no cueste bytes.

### 1.6 El DWARF, otra vez: `int v` NO existe en el original

`symbols/mw_dwarfdump.nothpp:1851117`. El bloque anonimo del `pad_state == 0`
(rango 0x8026C6B4..0x8026C878, que cubre los cuatro ejes) declara **una sola
local**:

```c
/* anonymous block */ {          // 0x8026C6B4 -> 0x8026C878
    short data;                  // r0
}
```

Ni `int v` ni los cuatro `int data` de bloque que tenemos. Y las tres
expansiones `inline float Timer::GetSeconds()` estan en 0x8026C958, 0x8026C9C4 y
0x8026C9C4: son **TRES**, no dos.

La veda de la r47 sobre «una sola `short data`» sigue en pie por porcentaje
(1.564 B, 48-71 filas), pero **la lectura de por que era falsa hay que
corregirla**: no es que el DWARF nombre mal la local, es que el bloque tiene UNA
y nosotros escribimos CINCO variables. Con `data` de tipo `short`, `data &
0x8000` se simplifica en `combine` (la mascara cabe en los 16 bits bajos, el
`sign_extend` sobra) y da el `andi.` sin `extsh` que el objetivo tiene; y
`data > 255` si necesita el `extsh`. Las dos formas del objetivo salen de ahi.

---

## 2. `UpdateLoaded` (856 B): la propuesta de la r47, medida y muerta

### 2.1 Banderas en sombra (8 medidas)

`q2.py` compila la unidad real a scratch con banderas extra. Base 860 B / 21
filas / 97,21028 %:

```text
BASE                          856/860   97,21028 %   21 filas
-fno-rerun-cse-after-loop     856/848   84,6729  %   79 filas   <- LA PROPUESTA
-fno-cse-skip-blocks          856/892   85,16355 %   84 filas
-fno-gcse                     856/860   97,21028 %   21 filas   (identico)
-fno-cse-follow-jumps         856/860   97,21028 %   21 filas   (identico)
-fno-expensive-optimizations  856/860   97,21028 %   21 filas   (identico)
-fno-regmove                  856/860   97,21028 %   21 filas   (identico)
-fno-caller-saves             856/860   97,21028 %   21 filas   (identico)
```

Dos conclusiones con cifra:

- **`-fno-rerun-cse-after-loop` NO es la medida a hacer.** Apagar `cse2`
  conserva el pseudo de `high($LC917)`, si, pero se lleva por delante todo lo
  demas que `cse2` hace en esa funcion: 79 filas contra 21, y ademas el objeto
  se ACORTA (848). **La propuesta 4 de la r47 queda cerrada en negativo**; no
  hay que pedir un cambio de cflags por objeto.
- **`-fno-gcse` es IDENTICO a la base**, o sea que el PRE que la r47 vio en el
  volcado `.loop` **no aporta nada al objeto final**: `cse2` y
  `update_equiv_regs` deshacen su trabajo entero. El mecanismo estaba bien
  descrito pero no es una palanca por ningun lado.

### 2.2 Seis formas de fuente nuevas, todas negativas

```text
BASE                              856/860   97,21028 %   21 filas
W1 sqrt antes del bloque mSlip    856/868   79,1729  %   75
W2 mSlip con ternario             856/864   95,47664 %   27
W3 slip_speed antes de Atan2a     856/868   88,71028 %   54
W4 !(skid_speed <= 1.0f)          856/860   97,21028 %   21 (= base)
W5 !(Abs(fwd_vel) <= 1.0f)        856/860   97,21028 %   21 (= base)
W6 las dos negadas                856/860   97,21028 %   21 (= base)
```

Con las 5 de la r47 van **once** formas medidas en esta funcion.

### 2.3 Por que el `asm` de SOLO ENTRADA no aplica aqui

El brief propone `asm("" : : "r"(x))` para subir `REG_N_REFS` por encima de 2 y
romper la condicion de `update_equiv_regs`. **No se puede montar**, y el motivo
es el mismo que ya cerro `FSasync` en la r46 §4: lo que hay que mantener vivo es
un pseudo que contiene `high($LC917)`, **la parte alta de una DIRECCION de la
pool de literales**, y eso **no tiene nombre en C** --- ningun operando de `asm`
puede nombrarlo. Nombrar el flotante (`"f"`) ata un FPR distinto; nombrar la
direccion completa hace que GCC materialice `addi @l` y `0(rN)`, que es la forma
equivocada (veda r36e).

**Veda ampliada**: `asm("" : : "r"(x))` sube `n_refs` de VALORES. Para un `high`
de direccion no hay palanca de `asm` de ningun tipo, ni de entrada ni de salida.

---

## 3. `LibSN/steering` (2.568 B): 29 formas nuevas y dos correcciones al catalogo

Arnes: `t.py` de la r47 (una variante = 0,8 s, no toca el arbol).

### 3.1 `Effect_PerformEnvelope` (196 B): 12 formas, y la r46 estaba equivocada

Falta `mr r5,r9`: al objetivo `mag` le vive en r9 y `out` en r5 con una copia
entre las dos; nosotros las fundimos en r6. Doce formas nuevas
(`s1.py`; base 192/196):

```text
A dos ternarios (mag y out por separado)     204/196   PEOR
B dos ternarios, out primero                 204/196   PEOR
C out = ternario; mag = out                  192/196   = BASE
D mag = ternario; ...; out = mag             192/196   = BASE
E if/else en vez de ?: ; out = mag           192/196   = BASE
F out = *(volatile s32 *)&mag                208/196   PEOR
G mag = *(volatile s32 *)&out                228/196   PEOR
H out = level (equivalente por el signo)     192/196   = BASE
I neg calculado primero                      192/196   = BASE
J neg = level < 0                            192/196   = BASE
K mag = neg ? -level : level                 188/196   PEOR (mas corta aun)
L out = mag con un if muerto detras          204/196   PEOR
```

**Correccion a la r46 §2.2.** Alli quedo escrito que «el `mr r5,r9` que le falta
lo produce apagar la CSE global» porque `#pragma global_optimizer off` daba el
tamano exacto de 196 B. **Es falso**: con el pragma puesto y mirando el objeto
instruccion a instruccion (`s2.py`), en +0x1C el objetivo tiene `7D254B78`
(`or r5,r9,r9` = `mr r5,r9`) y **nosotros seguimos teniendo un `bge`**. Los
196 B salen de otras instrucciones, no de la copia. El eje del pragma no apunta
a ningun sitio en esta funcion.

Van **veintitres** formas medidas aqui (r22: 10, r46: 1, r48: 12).

### 3.2 `SimThread_Init` (324 B, 8 insn): 11 formas, todas negativas

Los dos puntos son (1) el `lfs` de la escala `4096.0f` cae DELANTE del
`li r3,0`+`mtctr` y el objetivo lo pone detras, y (2) empate desnudo r5/r6 en el
**cuarto** bucle --- en el segundo, que es identico, casamos. `s3.py`:

```text
S1 escala en una local             8 insn = BASE
S2 a[n] leida por puntero volatil  8 insn = BASE
S3 producto en un temporal float   8 insn = BASE
S4 bucle 3 indexado con i         12 insn PEOR
S5 bucle 3 como while              8 insn = BASE
S6 bucle 4 indexado con n          8 insn = BASE
S7 bucle 4 con hist primero       10 insn PEOR
S8 bucle 4 con una local z = 0     8 insn = BASE
S9 ia[i] leida por puntero volatil 8 insn = BASE
SA bucle 4 como do-while         328/324 PEOR
SB histIndex delante del bucle 4 320/324 PEOR
```

Con las 14 anteriores van **veinticinco** formas en esta funcion. El
`volatile` --- la unica palanca viva de la unidad --- es **neutro en los dos
puntos** (S2 y S9), lo que la descarta para empates de reparto en mwcc: solo
sirve para fijar el PUNTO DE CARGA, y aqui el punto de carga ya es el bueno.

### 3.3 `CookValues` (260 B, 13 insn): la veda de la macro CADUCA

La primera diferencia que no es de registro es la fila 20/22: el objetivo hace
`extsb r0,r5` --- **RE-EXTIENDE el byte crudo** para `_mx = _v` --- y nosotros
`mr r0,r6`, copia del ya extendido. Eso lo produce leer `(vv)` otra vez: mwcc
comparte el `lbz` por CSE pero emite un `extsb` nuevo.

La r47 dejo la veda «cualquier cambio en el cuerpo de `AUTOCAL`/`AUTOCALX` rompe
`HandlePedals`, que hoy casa al 100 %». **La veda se rompe por como se aplica**:
`AUTOCAL` la usan `CookValues` y `HandlePedals`, asi que basta **duplicar la
macro** (`AUTOCALC`) y cambiar solo la copia que usa `CookValues`. Medido
(`s4.py`): en las seis variantes **`HandlePedals` sigue al 100 %**.

Dicho eso, las seis son negativas por tamano:

```text
C1 _mx = (vv)                          264/260
C2 (mx) = (vv) y _mx = _v              264/260
C3 los dos (vv)                        268/260
C4 _mx = (mx) detras del store         264/260
C5 _mx = (vv) antes de (mx) = _v       264/260
C6 _mn = (vv) tambien                  268/260
```

**Lo util es el metodo**: cualquier veda de esta unidad que diga «no se puede
tocar la macro porque rompe la otra funcion» hay que remedirla con una copia
acotada de la macro. Es gratis y no tiene efecto colateral.

---

## 4. Lo que NO he vuelto a probar (y por que)

- **Version de compilador y cflags de `steering`**: cerrado en la r47 con 28
  versiones y 39 combinaciones. Sin novedad.
- **Pragmas por funcion de `steering`**: 232 compilaciones en la r46. Solo he
  vuelto sobre `global_optimizer off` para desmentir su lectura (§3.1).
- **Corte de `steering` en dos TU**: descartado en la r47 (conjuntos anidados).
- **El pin de registro en `ActualReadJoystickData`**: la r47 lo midio y empeora;
  §1.3 explica ahora POR QUE (acorta el objeto y el empate necesita alargarlo).

---

## 5. Verificacion

```text
git status --porcelain -- src/LibSN/steering.c src/Speed/GameCube/Src/JoyE.cpp \
    src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp
    -> vacio  (ningun fichero de mi territorio tocado)

python scripts/build_direct.py LibSN/steering \
       Speed/Indep/SourceLists/zPlatform Speed/Indep/SourceLists/zPhysicsBehaviors
    -> 3 ok, 0 fallidas
```

`fncmp` DESPUES, identico al de ANTES en las tres unidades:

```text
LibSN/steering        6 de   36  --  2.568 B   + 3 alias / 488 B
   924 SimThread_Step  tamano (920/924)     588 HandleTriggers  42 insn
   324 SimThread_Init  8 insn, 7 reub       276 Effect_Init     26 insn, 3 reub
   260 CookValues      13 insn              196 Effect_PerformEnvelope (192/196)

zPlatform             1 de  136  --  1.588 B   + 42 alias / 17.796 B
   1588 ActualReadJoystickData__Fv   tamano (1580/1588)

zPhysicsBehaviors     1 de 1120  --    856 B   + 341 alias / 164.444 B
   856 UpdateLoaded__Q217SuspensionTraffic4Tireffff   tamano (860/856)

python scripts/lcfix.py --check  -> «todas las entradas @lc estan al dia»
```

**Ninguna funcion empeora**, ni las 135 vecinas de `zPlatform` ni las 1.119 de
`zPhysicsBehaviors` ni las 30 de `steering`. Ningun literal nuevo, ningun `asm`
retenido, ninguna cabecera compartida tocada.

---

## 6. Propuestas fuera de territorio

1. **RETIRAR la propuesta 4 de la r47.** `-fno-rerun-cse-after-loop` por objeto
   para `zPhysicsBehaviors` esta medido en sombra: **848 B y 79 filas contra 860
   y 21**. No hay que tocar `configure.py` por esto. (§2.1)
2. **`fncmp.py <unidad> <Nombre>` no imprime detalle cuando el TAMANO difiere**
   --- `scripts/fncmp.py:244-246` mete la fila `'tamano'` y hace `continue`
   antes de llegar al bucle de detalle. Justo los casos de +-4 B, que el brief
   llama «el caso mas barato del censo», son los que se quedan mudos y obligan a
   tirar de `fndiff`. Arreglarlo es dejar que la funcion con otro tamano llegue
   al detalle (comparando hasta el minimo de los dos).
3. **`lmap.py` sobre el ORIGINAL merece un barrido en todo el censo de tamano.**
   Aqui ha convertido «al objetivo le sobran 8 B» en «la linea 307 del JoyE.cpp
   de EA emite `mr`+`andi.`», que es una pregunta que ya se puede contestar. La
   receta corta: `lmap.py <unidad> <simbolo>` y mirar QUE linea imputa las
   instrucciones sobrantes y **que huecos de linea** hay alrededor.
4. **Regla nueva para el catalogo --- el empate de longitud de vida.** Si un
   near-miss ensena un PAR de `lis` izados con dos preservados intercambiados
   (`r18`/`r19`, `r14`/`r15`, `f25`/`f27`...), **no es un empate suelto que se
   arregle con un pin**: es el truncado a entero de
   `floor_log2(n_refs)*n_refs/live_length*10000*size` en `global.c`, y se rompe
   solo con que la funcion cambie de tamano DENTRO del bucle. La consecuencia
   practica: **si al objetivo le sobran bytes, arregla el tamano ANTES de tocar
   el reparto**, porque el reparto se recoloca solo. Y al reves: un pin que
   acorta el objeto empeora estos grupos por construccion. Medido en §1.3 y
   §1.4, donde hay TRES grupos de empate en la misma funcion y el estado de los
   tres depende de la longitud del cuerpo del bucle.
5. **Veda ampliada del `asm` de solo entrada.** `asm("" : : "r"(x))` sube
   `n_refs` de un VALOR. Cuando lo que falta es un pseudo con un `high` de
   DIRECCION (la pool de literales, un `@ha` de global), **no hay palanca de
   `asm` de ninguna clase**: ni `"+r"`, ni solo entrada, ni `"m"`. Segundo caso
   medido (el primero fue `FSasync`, r46 §4).
6. **Metodo para las vedas de macro.** Antes de dar por cerrada una veda del
   tipo «no se puede tocar esa macro porque rompe la otra funcion», **duplica la
   macro y acotala a la funcion que estas mirando**. En `steering` esto ha
   invalidado la lectura de la veda de `AUTOCAL` de la r47 sin coste ninguno.

## 7. Utiles (en `scratchpad/r48_plat/`, no en el arbol)

| fichero | que |
|---|---|
| `t.py`, `g.py`, `fndiff2.py` | los arneses de la r47 (mwcc y GCC), copiados |
| `s1.py` | 12 formas de `Effect_PerformEnvelope` |
| `s2.py` | un `#pragma` acotado a una funcion CON volcado del objeto |
| `s3.py` | 11 formas de `SimThread_Init` |
| `s4.py` | 6 macros `AUTOCALC` acotadas a `CookValues` |
| `p1.py` | 6 formas del bloque `stickX` de `JoyE.cpp` |
| `p3.py` | la medida del empate de longitud de vida (los `nop`) |
| `p4.py`, `p5.py` | la palanca 5 (el `asm` con cuerpo), no volatil y volatil |
| `q2.py` | las 8 banderas en sombra de `UpdateLoaded` |
| `q3.py` | 6 formas de fuente de `UpdateLoaded` |
| `mini/sw.py` | el reproductor minimo de 20 lineas: 11 formas en 4 s |
| `joy_lmap.txt` | el mapa de lineas del ORIGINAL de `ActualReadJoystickData` |
| `joy_base.txt`, `dump_N1/N2/X1/X2/X3/A.txt` | los `fndiff` completos de cada base |
