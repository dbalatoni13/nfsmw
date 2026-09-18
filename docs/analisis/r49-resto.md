# r49 --- territorio `resto`

`LibSN/steering`, `zPlatform`, `zPhysicsBehaviors`, `zEAXSound2`, `zSpeech`,
`madidct`, `criticalpath`. Entrada: **8.756 B en 13 funciones**.

## Resultado

**No cierra ninguna funcion y no se retiene ningun cambio de fuente.** Las siete
unidades quedan byte por byte como estaban (verificacion en §8: `fncmp` antes y
despues es **identico caracter a caracter**). Lo que sale de la ronda:

1. **Una palanca nueva y general, con la linea de GCC**: en `sched1` el nivel 2
   de `rank_for_schedule` (`INSN_REG_WEIGHT`) HUNDE al final del bloque toda
   instruccion que sea un `PARALLEL` --- y `andsi3` de rs6000 SIEMPRE lo es, por
   el `(clobber (match_scratch:CC))`. Mover la sentencia para que un operando
   MUERA en ella baja el peso de 2 a 1 o a 0 y la sube de golpe. En
   `VP6_PredictFilteredBlock` vale **14 filas sin un solo `asm`** (40 -> 26).
2. **`criticalpath` queda acotada con numeros**: el suelo baja de 40 a 26 filas
   con una reordenacion de sentencias limpia (mejor que las 25 de la receta de
   la r48, que lleva dos `asm`), y lo que queda esta **demostrado inalcanzable
   desde la fuente** (§2.4). Ademas: **las CINCO versiones de ProDG del arbol
   dan el objeto IDENTICO**, y tres de sus cinco pines son andamio.
3. **`GenerateRoadNoise` baja de 23 a 18 filas sin `asm`** y con la posicion de
   la segunda acumulacion **probada por el objetivo** (esta dentro del bloque
   basico del `bLength`). Lo que queda es UNA cosa: nosotros izamos el `0.1f` a
   `f30` y el objetivo lo RECARGA dos veces.
4. **`Effect_Init` deja de ser «26 insn de rotacion de registros»**: sus 31
   filas son UNA causa --- el objetivo conserva `lis rN,0x4330` fuera del bucle
   en un SEXTO preservado y nosotros lo rematerializamos dentro---. Es la misma
   forma que `UpdateLoaded`, **y esa es de GCC y esta de mwcc**.
5. **`ActualReadJoystickData` es de UNA causa**: las 8 filas de registro
   (158-189) salen del par que falta, porque el `andi.` de +0x1380 **parte el
   rango de vida de r11**. Con las 6 de G1 (que la r48 ya ato al tamano), las
   **16 filas cuelgan de los mismos 8 B**.

| unidad | funcion | antes | despues |
|---|---|---:|---:|
| todas | las 13 | igual | **igual** |

Ficheros de fuente tocados: **ninguno** (`git diff HEAD` vacio en los siete).

---

## 1. LA PALANCA NUEVA --- el peso de registro hunde los `and`

`haifa-sched.c:4158`, `rank_for_schedule`, nivel 2:

```c
  /* prefer an insn with smaller contribution to registers-pressure */
  if (!reload_completed &&
      (weight_val = INSN_REG_WEIGHT (tmp) - INSN_REG_WEIGHT (tmp2)))
    return (weight_val);
```

y `haifa-sched.c:5243-5339`, `find_insn_reg_weight`, cuenta

```
reg_weight = (#SET + #CLOBBER con destino REG)  -  (#notas REG_DEAD/REG_UNUSED)
```

**y en un `PARALLEL` cuenta CADA `SET` y CADA `CLOBBER`.** `andsi3` de rs6000
lleva siempre `(clobber (match_scratch:CC))`, asi que un `and` reg-reg entra con
**2**, contra **1** de un `li`/`lwz`/`mr` y **0** de una copia cuyo origen muere.
Consecuencia: en cuanto un `and` tiene `INSN_PRIORITY` empatada con el resto
(tipico: su resultado se usa en OTRO bloque, luego su prioridad es 2), **se va al
final del bloque siempre**.

**El arreglo es de fuente y de cero bytes**: mover la sentencia para que sus
operandos MUERAN en ella. Si el `and` es la ultima lectura de su fuente, peso 1;
si ademas es la ultima de la mascara, peso 0.

Verificado contra la traza (`scripts/schedtrace.py criticalpath
VP6_PredictFilteredBlock -dS -fsched-verbose-5`), lista de listos del bloque 4 en
`t = 10` --- se imprime **de peor a mejor**:

```
;;	Ready list (t = 10):    158  156  129  636  633  134  122
;;		--> scheduling insn <<<122>>> on unit iu2     (prio 3)
;;		--> scheduling insn <<<134>>> on unit iu2     ModY = mVy & MvModMask
```

134 (peso 0, LUID menor) gana a 633/636 (copias, peso 0), y los dos ganan a 129
(`ModX`, peso 1) y a 156/158 (`li 0`, peso 1). **El modelo predice el orden
exacto de los siete.**

---

## 2. `criticalpath` / `VP6_PredictFilteredBlock` --- 740 B, y **12.040 B de `linked`**

Base `# VP6_PredictFilteredBlock  target=94.39459%  ours=94.39459%  size=740/740`
**40 filas**.

### 2.1 Lo mejor medido: 26 filas, sin un solo `asm`

```c
-        ModX = mVx & MvModMask;
-        ModY = mVy & MvModMask;
-
         mx = mVx + (MvModMask & (mVx >> 31));
         my = mVy + (MvModMask & (mVy >> 31));
 
         mx = mx >> MvShift;
         my = my >> MvShift;
+
+        ModX = mVx & MvModMask;
+        ModY = mVy & MvModMask;
```

**740/740 B, 96,01621 %, 26 filas** (base 40). En la traza de `sched1` los dos
`and` pasan de emitirse los ULTIMOS del bloque a salir en los ciclos 10 y 12
(`ModY` primero, como el objetivo), y con ellos caen **catorce filas**.

Comparado con la receta de la r48 (dos `asm`): L1 (`"+r"(mVx)`) 29 filas /
93,562164 %; L2 (`rec` local con barrera) SOLA 40 filas / 94,12432 %; **L1+L2 25
filas / 95,0 %** (reproducida). O sea que **una reordenacion limpia vale casi lo
mismo que las dos barreras**, y no es deuda.

**NO SE RETIENE**, y el motivo no es la regla 9 sino la evidencia: el mapa de
lineas del ORIGINAL pone `ModX` en `recon.inl:80` y `ModY` en `:81`, **delante**
de `mx` (`:84`) y `my` (`:85`). D1 puntua mejor con una forma que sabemos que NO
es la del original. Se deja escrita para reaplicarla en un minuto.

Otras 11 ordenaciones medidas (todas 740/740):

```
D1/D2/D4  Mod al final / tras el add / cadena x y cadena y   26 filas  96,01621 %
F7        los dos shifts fundidos en la asignacion            28       94,63784 %
D3/F4/F5/F6  Mod al final invertido, entre add y shift, ...   30       95,90811 %
F1        solo ModX al final                                  32       94,82703 %
F2        solo ModY al final                                  40 = BASE
```

### 2.2 La cadena causal, entera y con numeros

`lmap.py` sobre el original + la traza de `sched1` dan las tres diferencias del
bloque `if (VpProfile == 0 || UseLoopFilter == 0)`, y **las tres cuelgan de una**:

1. **indice 28**: el objetivo emite `lhax r10,r7,r11` (mVx) y nosotros
   `lwz r8,0x88(r31)` (FrameReconStride). Es la ranura `lsu` de `t = 3`:
   `prio(lwz) = 9`, `prio(lhax) = 8`. **Se arregla igualando las prioridades**:
   con `TempBuffer = SrcPtr + Recon + (FrameReconStride*my + mx)` la cadena de
   `mx` gana un eslabon, las dos quedan a 9, empatan en peso y decide
   `depend_count` (3 contra 2) --- **el `lhax` cae en el indice 28, comprobado**.
   Coste: 48 filas sobre la base limpia, 50 sobre L1+L2, 42 sobre D1. La
   asociatividad es la del objetivo (`(SrcPtr+Recon) + (stride*my + mx)`, leida
   de `add r0,r27,r0` / `add r8,r8,r10` / `add r29,r0,r8`) pero cuesta mas de lo
   que arregla.
2. **indices 51/52**: los dos `and`. §1.
3. Todo lo demas (mVy en r25 en vez de r0, mx en r11 en vez de r10, y las 5
   filas de la rama `else` con `MvModMask` en r0 en vez de r9) es **cascada**:
   con los `and` al final, `mVx` y `mVy` siguen vivos hasta el final del bloque
   y `mx`/`my` no pueden reutilizar sus registros. El DWARF lo dice:
   `int mVx; // r10` `int mVy; // r0` `int mx; // r10` `int my; // r0`.

### 2.3 Los pines: TRES de los cinco son andamio

Sobre la base (740/740, 40 filas), quitando uno cada vez:

```
sin OutputPtr(r24)   40 filas  IDENTICO      <- andamio
sin TempPtr2(r27)    40 filas  IDENTICO      <- andamio
sin Stride(r26)      40 filas  IDENTICO      <- andamio
sin bp(r23)          42 filas  94,28648 %
sin ModY(r25)        50 filas  94,01621 %
sin NINGUNO          72 filas  93,23243 %
```

**Y no son intercambiables**: quitando los tres andamios A LA VEZ salen **44
filas** (94,324326 %), y sobre D1 lo mismo: uno a uno identicos, los tres juntos
44. Es `regs_someone_prefers` puro. Sobre D1: sin bp 28, sin ModY 37.

Pines nuevos, todos negativos: `mVy` a r0 52 filas, `mVx` a r10 = base, los dos
42, `my` a r0 = base, `mx` a r10 36, `ModX` a r28 sobre D1 **736 B** / 59 filas,
`mVy` a r0 sobre D1 42.

### 2.4 EL MURO, demostrado --- no hay forma de fuente

El objetivo coloca `and ModY` en la **posicion 8** del bloque y `and ModX` en la
**16** (de 27). Para eso su `INSN_PRIORITY` tendria que ser mayor que 2. Y no
puede serlo:

- `INSN_PRIORITY` es la longitud del camino mas largo hasta el final del bloque.
  El unico dependiente de los dos `and` dentro del bloque es **el salto**, luego
  la prioridad es **2**. Confirmado en la traza (`129 ... prio 2`, `134 ... prio
  2`) contra 3-14 de todo lo demas.
- El peso solo desempata a prioridad IGUAL, y **su suelo es 0**: un `and` tiene
  dos operandos, luego como mucho mueren dos registros (2 - 2 = 0). Con D1 ya lo
  alcanzamos en `ModY`.

**Conclusion medida: para reproducir el objetivo, sus `ModX`/`ModY` tienen que
tener un dependiente DENTRO del bloque que nosotros no tenemos, o su region de
planificacion incluia el bloque de union.** Lo segundo esta descartado por la
traza: `find_rgns` da **31 regiones de 1 bloque** en esta funcion (el limite es
`MAX_RGN_BLOCKS 10` / `MAX_RGN_INSNS 100`, `haifa-sched.c:226`), y
`-fsched-interblock` **explicito da el objeto IDENTICO**. Lo primero no lo he
sabido expresar: en el bloque solo quedan `Stride = CurrentReconStride` y
`TempPtr1 = TempPtr2 = 0`, y ninguna forma de esas dos puede depender de
`ModX`/`ModY` sin emitir una instruccion de mas.

### 2.5 Version de compilador --- CERRADA

Las cinco ProDG del arbol, con los cflags de la unidad (quitando
`-fno-static-dtors`, que 3.5/3.5b140/3.7/3.8.1 no aceptan):

```
ProDG/3.5        740/740 B  94.39459%  filas=40
ProDG/3.5b140    740/740 B  94.39459%  filas=40
ProDG/3.7        740/740 B  94.39459%  filas=40
ProDG/3.8.1      740/740 B  94.39459%  filas=40
ProDG/3.9.3      740/740 B  94.39459%  filas=40
```

**Objeto identico en las cinco.** El eje de version esta cerrado para esta
unidad (y probablemente para todo el paquete `vp6`, que comparte cflags).

### 2.6 Banderas en sombra (no tocan produccion)

Sobre la base: `-fno-force-mem` **30 filas** (la mejor, 740/740);
`-fno-cse-follow-jumps` 44 / 748 B; `-fno-expensive-optimizations` 50 / 732 B;
`-fno-gcse` 87 / 744 B; `-Os` 84 / 744 B; `-fno-schedule-insns` 78 / 732 B;
`-fno-schedule-insns2` 76; las dos 116 / 732 B.
**IDENTICAS a la base**: `-fsched-interblock`, `-fno-sched-interblock`,
`-fsched-spec`, `-fno-sched-spec`, `-fno-regmove`, `-fno-caller-saves`,
`-fno-strength-reduce`.
Sobre D1: `-fno-force-mem` 27; `-fno-force-addr`, `-fno-peephole`,
`-fno-delayed-branch` identicas (26); `-fno-schedule-insns` 81 / 732 B;
`-fno-schedule-insns2` 77.
**AVISO**: `-fno-rerun-cse-after-loop` **rompe el compilador** en esta unidad ---
`criticalpath.c:1061: Internal compiler error at flow.c:993` en
`VP6_decodeModeAndMotionVector`---. No se puede ni medir en sombra.

### 2.7 Lo demas, medido y negativo (todo 740/740 salvo nota)

Asociatividad de `TempBuffer` sobre D1: `my*stride` 27, `mx` antes del `mul` 51,
`mx` primero 52, `Recon` al final 58. Sin D1: 30 / 46 / 55 / 60.
Barrera `__volatile__("")` tras ModY 48 filas / **736 B**; `asm` de solo entrada
sobre ModX+ModY, ModY o ModX (los tres) 55 filas / **744 B**; ModX/ModY leidos
directamente de `pbi->mbi.Mv[bp]` 54 / 744 B.

---

## 3. `GenerateRoadNoise` (1.240 B) --- de 23 a 18 filas sin `asm`, y la causa restante nombrada

Base `target=97.1129% size=1240/1240`, **23 filas** (`fndiff`; el «33» del censo
es la cuenta de `fncmp`).

### 3.1 La posicion de la segunda acumulacion la dice el OBJETIVO

El bloque basico que va del `fmr f11,f12` (indice 96) al `bso` (117) contiene, en
el objetivo, **la segunda acumulacion como ULTIMA instruccion util** (`fmadds
f29,f29,f10,f29`, indice 114) --- es decir, DENTRO de la expansion de
`bLength(m_bvTotalLeftWheelSlip)` y delante de la guarda del `sqrt`. Nosotros la
emitimos en el indice **134**, en el bloque de DESPUES del `sqrt`. Como una
expresion no se puede partir, **la sentencia tiene que estar delante del
`bLength` en la fuente** y el planificador la hunde hasta el final del bloque.

### 3.2 La forma: 18 filas, sin `asm`

```c
     float speed;
     float ftemp;
+    float slipBoost = 0.1f;
     ...
-    float slipBoost = 0.1f;
-    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip);
     fRightVol = fRightVol + fRightVol * slipBoost;
+    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip);
     ftemp = ftemp * 0.01f;
```

**1240/1240 B, 98,09677 %, 18 filas.** La primera acumulacion cae en la ranura
exacta del objetivo (indice 102) y la segunda dentro de su bloque. Identico si
`slipBoost` se declara sin inicializador arriba y se asigna al principio de la
funcion (X5, 18 filas).

Las dos mitades por separado: mover SOLO la acumulacion (V1) da **55 filas** ---
la funcion entera permuta f10<->f11---; declarar SOLO arriba no cambia nada.

### 3.3 Lo que queda: el `0.1f` izado a f30

Las 18 filas son cinco de preambulo (16, 17, 23, 25, 26) y trece del bloque, y
**todas tienen la misma causa**: nosotros izamos el `0.1f` a **f30** (un
preservado) en el indice 17 y el objetivo lo **RECARGA** en `lbl_803D9A60@l`
**dos veces**, en los indices 104 y 148. Leido del DOL:

```
803D9A60  3dcccccd  0.10000000149011612
803D9A58  3c23d70a  0.009999999776482582
803D9A5C  3e19999a  0.15000000596046448
803D9A40  3f000000  0.5
```

O sea: **en el original el `0.1f` NO es una variable izada**. Con el literal
suelto (X3) el tamano sigue siendo exacto (1240 B --- la veda de la r48 «sin
`slipBoost` mide 1.232 B» **CADUCA en cuanto la acumulacion se mueve**) pero
vuelve la permuta f10/f11: 55 filas. Con la variable pinchada a `fr10` (W3) los
registros casan y quedan **20 filas**, todas de orden de las cargas del pool
dentro del bloque.

**`slipBoost` sigue siendo imprescindible en la forma actual** (sin mover la
acumulacion): eso no cambia.

### 3.4 Medido y negativo (todo 1240/1240 salvo nota)

```
V1  acumulacion antes del bLength, decl in situ ........ 55 filas  96,10968 %
V2/V5  acumulacion tras el *0.01f ...................... 23 = BASE
V3  += en vez de = ..................................... 55
V4  bLength*0.01 fundido, acumulacion antes ............ 55
W1/W2  pin de ftemp a fr11 / fr10 sobre V1 ............. 115 / 116
W3  pin de slipBoost a fr10 sobre V1 ................... 20      96,80322 %
W4  pin a fr11 .................................. 55 · Y1 fr9 22 · Y2 fr8 21
Y3  fr12 26 · Y4 fr7 21
Y5  decl arriba + pin fr10 .................. 20 filas pero **1.232 B** (MAL)
Y7/Y8  pin fr10 con la acumulacion detras .............. 25
X1  decl arriba sin init, asignada junto a la acumulacion  55
X2/X3  literal 0.1f en la acumulacion .................. 55
X4  decl arriba + acumulacion tras el bLength .......... 21
Z1  literal + t1 (primer bLength) pinchado a fr11 ...... 36
Z2  literal + t1 local sin pin ......................... 27
Z3  base + t1 pinchado ................................. 39
Z4  literal + local t2 para el segundo bLength ......... 55
AA/AB  literal + t2 pinchado a fr11 / fr12 ............. 55
AD  pin fr10 + literal en la linea 403 ................. 20
BA  la receta de 16 filas de la r48 (V1 + asm("")) ..... 16      97,241936 %
BB  W5 + asm("") ....................................... 23
BC  W5 + asm tras el bLength ........................... 18 (= W5)
BD  W3 + asm("") ....................................... 16
```

Unas **35 formas** nuevas, sobre las ~80 de la r48.

---

## 4. `LibSN/steering` --- 30 formas nuevas y DOS diagnosticos nuevos

### 4.1 `Effect_Init` (276 B, 31 filas): NO son 26 insn de rotacion, es UNA cosa

El diff entero cuelga de que el objetivo salva **SEIS** preservados
(`stmw r26, 0x18(r1)`, marco identico) y nosotros **CINCO** (`stmw r27,
0x1c(r1)`). El sexto es `lis r27, 0x4330` --- **la parte alta del sesgo del
conversor int->float**, que el objetivo IZA fuera del bucle de la tabla de senos
y nosotros rematerializamos DENTRO (`lis r0, 0x4330`, indice 38):

```
objetivo  36 lis r27, 0x4330      <- fuera del bucle
          37 xoris r0, r26, 0x8000
          39 stw r27, 0x8(r1)     <- dentro
nuestro   37 xoris r3, r29, 0x8000
          38 lis r0, 0x4330       <- dentro
          39 stw r3, 0xc(r1)
```

Todo lo demas (la rotacion r26..r31 y las dos filas desplazadas del bucle de
puesta a cero) es cascada del preservado de mas.

`#pragma optimization_level` (la funcion va ya en un bloque `level 2`):

```
0  312 B  62,405796 %  61 filas      3  344 B  41,95652 %  63 filas
1  312 B  74,478264 %  48 filas      4  344 B  41,95652 %  63 filas
2  = BASE (276 B, 88,62319 %, 31)   <- el nivel actual es el correcto
```

Y **IDENTICAS a la base**, todas con `level 2`: `opt_loop_invariants on`,
`opt_strength_reduction on`, `opt_unroll_loops on`, `opt_propagation on`,
`opt_common_subs on`, `opt_lifetimes on`.

Formas del bucle: `(float)i` a una local 284 B / 45 filas; `i * (PI/128.0f)`
268 B / 38; conversion a `double` 276 B / 38; `i++` al final = BASE; `ramp`
primero 280 B / 39; `for` en vez de `do-while` 280 B / 35; `phase/63` en flotante
316 B / 53.

### 4.2 `Effect_PerformEnvelope` (196 B contra 192): 18 formas mas, y la condicion escrita

El `mr r5,r9` que falta es **`out = mag`**: el objetivo tiene `mag` en r9 y `out`
en **r5, el registro del parametro `level`**; nosotros fundimos las dos en r6.

**La condicion, escrita como se puede falsar**: `out` y `mag` se solapan del
indice 7 al 41, pero **no hay NINGUNA lectura de `mag` posterior a una escritura
de `out`** (las tres son `subf r3,r4,r9` / `subf r3,r9,r3` / `add r5,r9,r0`, y la
tercera escribe `out` en la misma instruccion). Con eso el coalescing es legal y
mwcc lo hace SIEMPRE. Para romperlo hace falta un uso de `mag` **detras** de una
asignacion a `out`, y la fuente que tenemos no lo tiene.

Las 18 nuevas (base 192/196, 31 filas):

```
P1  level reusado como out ...................... = BASE
P2  level = mag = abs(level) .................... 26 filas  90,30612 %  (192 B)
P3  neg primero + level = mag ................... = BASE
P4  abs con if + level = mag .................... **196/196 B** 36 filas 78,67 %
Q1/Q2  abs con if, out o mag como base .......... 39 filas  76,938774 %
Q3  neg primero + abs con if .................... 34 filas  76,63265 %
Q4/Q5/Q6  out = mag separado / antes de neg / neg = level<0 ..... = BASE
R1/R2/R3  register en mag, out o los dos ........ = BASE
R4/R5  orden de declaracion ..................... = BASE
R6..R9  out = mag+0 / mag*1 / (s32)mag / -(-mag) . = BASE
T1  neg por puntero volatil ..................... 208 B  37 filas
T2/T3  mag por puntero volatil en el uso 1 o 3 .. 208 B  39 / 38 filas
T4  out inicial por puntero volatil ............. 208 B  37 filas
```

**P4 da el tamano EXACTO (196/196)** pero con 36 filas: los 4 B salen del `abs`
con rama, no de la copia. Van **41 formas** medidas en esta funcion (r22 10,
r46 1, r48 12, r49 18).

---

## 5. `ActualReadJoystickData` (1.588 B) --- las 16 filas son UNA causa

Corrige el diagnostico heredado. Las 16 filas son tres grupos:

- **6 filas** (40, 41, 249, 254, 276, 280): G1, r18/r19. La r48 ya midio que se
  arreglan SOLAS en cuanto la funcion crece una instruccion.
- **8 filas** (158, 159, 162, 165, 184, 185, 188, 189): el objetivo usa **r10 y
  r11** de temporal en los dos clamps y nosotros **r8**.
- **2 filas** (190, 192): `mr r9,r0` + `andi. r11,r9,0x8000`, las que faltan.

**Las 8 del medio son consecuencia de las 2**, y se ve en el rango de vida de
r11. Los cinco `mr rX,r11` de los indices 194-203 --- que la r46 identifico como
copias que mete el REPARTO despues de planificar, y que **son identicos en los
dos objetos**--- leen r11. En el objetivo r11 lo reescribe el `andi.` del indice
192, asi que su valor anterior muere en el 189 y **r11 queda libre para los dos
clamps**. En el nuestro r11 se escribe en el indice 179 (`lwz r11,0x14(r1)`) y
tiene que seguir vivo hasta el 203, asi que los clamps caen en r8.

O sea: **las 16 filas de esta funcion cuelgan de los mismos 8 B**, y no hay tres
frentes sino uno. (Corrige tambien la lectura de la r48 §1.1: no es «un AND
muerto sin su rama»; el `if (v & 0x8000) v = 0;` de la fuente **ya sale**, en los
indices 181-183 con su `beq` y su `li r0,0`, y casa. Lo que falta es una SEGUNDA
lectura del mismo valor DETRAS del clamp `> 255`.)

No he hecho ensayos nuevos aqui: la r36b/r36f/r46/r47/r48 llevan 5+4+5+8+17
formas y el eje que queda es el que dejo escrito la r46 --- **la PRESION en ese
punto, no la forma del `if`**---, ahora con la razon de por que.

---

## 6. `UpdateLoaded` (856 B) --- el numero que faltaba

Las 21 filas son una sola cosa: el objetivo iza `lis r30, lbl_803FB6B8@ha`
(**1.0f**, verificado en el DOL: `803FB6B8 3f800000`) y lo comparte entre los
**dos primeros** de sus **cuatro** `lfs` de 1.0f; los otros dos se rematerializan
(`lis r8` en la linea 310, `lis r9` en la 329). Nosotros rematerializamos los
cuatro: +1 instruccion (860 contra 856), un segundo GPR preservado y marco 0x30
contra 0x28.

**El numero que faltaba en el diagnostico de la r36c**: `update_equiv_regs` hunde
el pseudo cuando `REG_N_REFS == 2`. El del objetivo tiene **DOS usos** (3 refs) y
no se hunde; el nuestro tiene **UNO** (2 refs) y si. La pregunta correcta no es
«por que cse2 rematerializa» sino **«por que cse2 rematerializa CINCO en el
nuestro y solo TRES en el objetivo»**. El reparto de las cuatro cargas en el
original esta en `scratchpad/r49_resto/ul_lmap.txt`.

**Y ojo al patron**: es exactamente la misma forma que `Effect_Init` (§4.1) ---
«el objetivo guarda la parte alta de una constante en un preservado y nosotros la
rematerializamos»--- pero aquella es **mwcc** y esta es **GCC**. Es un frente,
no una anecdota: ver la propuesta 3.

---

## 7. `Setup__RoadblockFlow` y `madidct`

- **`Setup__RoadblockFlow`** (596 B, 3 filas): no lo he vuelto a tocar. La r48
  lo cerro con causa medida --- el empate es exacto en los cinco niveles de
  `rank_for_schedule` y lo decide `INSN_LUID`, heredado de `sched1`, donde la
  asimetria es estructural del modelo de costes (antes de `reload` no hay `mtlr`
  y el pfn va directo a la llamada)---. Repetirlo era gastar la ronda.
- **`madidct`** (`IdctColumn` 632 B / 152 filas, `IdctRow` 516/496 B / 137
  filas): **no atacada**, siguiendo la instruccion del encargo de cerrar antes
  las pequenas. Sigue como estaba. Lo unico que anado es que **el frente de §1
  no le sirve**: sus diferencias son `lwz` 18/20 contra `mr` 12/10 (recargas de
  pila) y `mtctr`/`mfctr`, no colocacion de `and`.

---

## 8. Verificacion

```
python scripts/build_direct.py LibSN/steering Speed/Indep/SourceLists/zPlatform \
   Speed/Indep/SourceLists/zPhysicsBehaviors Speed/Indep/SourceLists/zEAXSound2 \
   Speed/Indep/SourceLists/zSpeech Speed/Indep/SourceLists/zEAXSound \
   egami/rcmp/dev/source/decoder/cmn/madidct \
   Packages/vp6/1.0.6/source/decode/gc/criticalpath
   ->  8 ok, 0 fallidas

diff fncmp_before.txt fncmp_after.txt   ->  VACIO  (las 8 unidades, identico)
python scripts/lcfix.py --check         ->  todas las entradas @lc estan al dia
git diff HEAD -- <mis 7 fuentes>        ->  vacio
find src -name "__r49*"                 ->  nada
```

`fncmp` de las ocho, antes = despues:

```
LibSN/steering       6 de   36  --  2.568 B
zPlatform            1 de  136  --  1.588 B
zPhysicsBehaviors    1 de 1120  --    856 B
zEAXSound2           1 de  930  --  1.240 B
zSpeech              1 de  703  --    596 B
zEAXSound            0 de  773  --      0 B   <- GATE cumplido
madidct              2 de    3  --  1.148 B
criticalpath         1 de   21  --    740 B
```

`fndiff` de las trece, sin mover:

```
SimThread_Step  95,671 %  924/920 · HandleTriggers 98,29932 % 588/588
SimThread_Init  94,69136 % 324/324 · Effect_Init 88,62319 % 276/276
CookValues 95,76923 % 260/260 · Effect_PerformEnvelope 93,265305 % 196/192
ActualReadJoystickData 99,3199 % 1588/1580 · GenerateRoadNoise 97,1129 % 1240/1240
IdctColumn 50,449368 % 632/632 · IdctRow 46,372093 % 516/496
UpdateLoaded 97,21028 % 856/860 · VP6_PredictFilteredBlock 94,39459 % 740/740
Setup__RoadblockFlow 98,255035 % 596/596
```

Volcados RTL borrados (`scratchpad/rtl/criticalpath_c.*`,
`scratchpad/schedtrace/*VP6_PredictFilteredBlock*`, ~5 MB). Mi scratchpad queda
en **519 kB**. **Aviso de infraestructura: el disco C: bajo de 4,3 GB a 3,2 GB
libres durante la ronda con seis agentes trabajando**; los directorios
`scratchpad/codex_*` de rondas viejas suman mas de 4 GB y nadie los usa.

---

## 9. Propuestas fuera de territorio

1. **Palanca nueva para el catalogo --- «el `and` que se hunde»** (§1). En
   `sched1`, `INSN_REG_WEIGHT` mete a un `and` reg-reg con peso **2** (el
   `PARALLEL` de `andsi3` cuenta el `SET` y el `(clobber (match_scratch:CC))`) y
   lo hunde por debajo de cualquier `li`/`mr`/`lwz`. **Firma para buscarlo**: al
   objetivo le salen los `and` INTERCALADOS en el bloque y a nosotros PEGADOS al
   salto final, y detras vienen filas de reparto que parecen independientes.
   **Arreglo de cero bytes**: mover la sentencia para que sus operandos MUERAN
   en ella. Vale 14 filas en `VP6_PredictFilteredBlock`. Se comprueba en un
   segundo con `schedtrace.py <unidad> <fn> -dS -fsched-verbose-5` mirando la
   lista de listos (se imprime de PEOR a MEJOR).
2. **Corolario del muro (§2.4), que ahorra rondas**: si al objetivo le sale una
   instruccion de **prioridad 2** (unico dependiente: el salto del bloque) en
   mitad del bloque y a nosotros al final, **no hay forma de fuente**, porque el
   peso solo desempata a prioridad igual y su suelo es 0. Antes de barrer formas,
   mira la columna `prio` de la traza: si es 2 y en el objetivo no esta al final,
   la fuente original tiene un USO que nosotros no tenemos.
3. **Frente nuevo, medido en dos unidades y en dos compiladores: «la parte alta
   que el objetivo preserva»**. `UpdateLoaded` (GCC, +4 B y un preservado) y
   `Effect_Init` (mwcc, +1 preservado) tienen EXACTAMENTE la misma forma: el
   objetivo guarda `lis rN, <constante>@ha` (o `0x4330`) en un registro
   preservado y lo comparte entre varios usos, y nosotros lo rematerializamos.
   **Se detecta a un grep sobre los diff ya generados**: `stmw rN` con N distinto
   entre los dos lados, o un `lis` que sale DELETE en el objetivo y INSERT en el
   nuestro. Merece un barrido del censo entero; si aparece en 5-6 funciones mas,
   es un frente y no dos casos.
4. **`rtldump.py` deberia aceptar `--extra`.** Lo pidio la r48 y sigue sin estar;
   he tenido que usar `schedtrace.py` (que si pasa `-fsched-verbose`) para sacar
   la traza. Es una linea.
5. **`-fno-rerun-cse-after-loop` provoca un ICE de cc1plus en `criticalpath`**
   (`flow.c:993`, en `VP6_decodeModeAndMotionVector`). Conviene anotarlo junto al
   ICE de `-dS`/`-dR` que documento la r48: hay al menos dos banderas que tumban
   este compilador en unidades concretas, y un agente que las use en sombra ve un
   fallo que parece suyo.
6. **La veda «sin `slipBoost` la funcion mide 1.232 B» CADUCA** (§3.3): en cuanto
   la segunda acumulacion se mueve delante del `bLength`, el literal suelto
   vuelve a dar **1.240 B**. Es el tercer caso esta temporada de una veda que
   dependia de la forma del resto de la sentencia, no del hecho medido.
7. **Barrido de versiones de compilador por PAQUETE, no por funcion** (§2.5). Las
   cinco ProDG dan el objeto identico en `criticalpath`; el arnes son 20 lineas y
   descarta un eje entero de golpe. Vale la pena pasarlo por las unidades de
   middleware (`vp6`, `egami`, `snd`) antes de que otro agente lo sospeche.

## 10. Utiles (en `scratchpad/r49_resto/`, 519 kB, fuera del arbol)

| fichero | que |
|---|---|
| `lib.py` | arnes de variante (parche + `build_direct` + `fndiff`), restaura siempre |
| `mini.py` | reproduccion minima: UN `.cpp` con los cflags de su SourceList (8,2 s contra 33 de `zEAXSound2`), con copia hermetica --- **no toca produccion** |
| `shadow.py` | compila una unidad con cflags EXTRA a un `.o` de scratch y lo diffea |
| `c1..c11.py` | los 60 ensayos de `criticalpath` |
| `rn.py`, `rn1..rn10.py` | los 35 de `GenerateRoadNoise` |
| `s1..s6.py` | los 30 de `steering` |
| `crit_HEAD.c`, `crit_L12.c` | la fuente de HEAD y la de la receta de la r48 |
| `crit25.txt`, `crit40.txt`, `critE1.txt`, `critE5.txt`, `critA1clean.txt` | los `fndiff` completos de cada base de `criticalpath` |
| `joy_lmap_r49.txt`, `joy_base_r49.txt`, `ul_lmap.txt`, `rn_base.txt` | los mapas de linea y diffs citados |
| `logs/fncmp_before.txt`, `logs/fncmp_after.txt` | la verificacion |
