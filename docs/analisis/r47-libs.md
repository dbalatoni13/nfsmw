# r47 — bibliotecas de GCC (`libs`)

Territorio: `madidct`, `sfir`, `criticalpath`, `pathnode`, `spchsamp`.
Entrada: **3.604 B en 7 funciones**.

## Resultado

**Cerradas dos funciones, 472 B, y con una de ellas una UNIDAD ENTERA.**

| unidad | funcion | antes | despues |
|---|---|---:|---:|
| `Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp` | `iSPCH_GetSampleSizeData__FP10VOXBANKHDRiPUiT2` | 136/136 B / 85,73529 % (15 filas) | **136/136 B, 100,0 %** |
| `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode` | `PATHI_calcwaitbeat__FiiiP12PATHBEATINFO` | 336/336 B / 98,86905 % (14 filas) | **336/336 B, 100,0 %** |

`fndiff` de las dos, citado literal:

    # iSPCH_GetSampleSizeData__FP10VOXBANKHDRiPUiT2  target=100.0%  ours=100.0%  size=136/136
    # PATHI_calcwaitbeat__FiiiP12PATHBEATINFO  target=100.0%  ours=100.0%  size=336/336

`fncmp`: `spchsamp` pasa de **1 de 2** a **0 de 2** funciones distintas;
`pathnode` de **2 de 11** a **1 de 11**.

### Propuesta a coordinacion: una promocion

`Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp` queda COMPLETA (las dos
funciones identicas, `.text` 184 B contra 184 B del objetivo). **No la he
promocionado yo** (no toco `configure.py` ni `splits.txt`). Vale **184 B de
`linked`**.

`pathnode` se queda a **8 B** de promocionar: solo le falta `PATHI_nextnode`
(300/308 B). Su `.text` es 4.196 contra 4.204. Es decir, **4.204 B de `linked`
cuelgan de una sola funcion**, y la palanca esta identificada y fuera de
territorio (ver §3).

---

## 1. `spchsamp` — no era el reparto, era la latencia del `mullw`

Tres rondas (r38, r44, r46) atacaron esta funcion como un problema de reparto:
el objetivo pone `result` en r0 y nosotros en r9, y de ahi sale una rotacion de
cinco valores. r46 barrio los cinco pines guiados por el DWARF y **las 48
reordenaciones legales del cuerpo**, todo negativo.

**La causalidad estaba al reves, y el volcado `.greg` lo dice literal.** Sacando
el RTL con `CPP.exe` + `cc1plus.exe` (cflags reales + `-dg -dl`), la cabecera de
la funcion trae:

    ;; 14 regs to allocate: 97 88 93 87 83 82 86 94 95 89 91 111 84 85
    ;; 89 conflicts: 82 83 84 85 86 89 95 97 118 119 0 1
    ;; 93 conflicts: 84 85 86 91 93 1

El pseudo 89 es `result` (`(set (reg/v:SI 89) (const_int 0))` /
`(const_int 1)` / `(set (reg/i:SI 3 r3) (reg/v:SI 89))`). **Conflicta con el
hard reg 0**, y ademas con los pseudos 118 y 119 --el `mullw` y el `subf` de la
cola, que son justamente los duenos de r0--. El pseudo 93 (`endOffset`) es el
UNICO de los catorce que no conflicta con r0, y por eso era el unico que lo
cogia.

Y conflicta porque **`result = 1` se planificaba ENTRE el `mullw` y el `subf`**,
rellenando la latencia de la multiplicacion. Es decir: el reparto era el
SINTOMA; el planificador era la causa.

Tres cambios, y hacen falta los tres:

1. `endOffset = endOffset * blockSize - offset;` delante de los dos stores
   (adelanta el `subf`, que el objetivo emite antes del primer `stw`).
2. Dos **barreras de ranura de cero bytes** (`__asm__ __volatile__("")`), una
   delante de los stores y otra delante de `result = 1`.
3. Partir `offset = ((offset << 8) + sampleData[1]) * blockSize;` en dos
   sentencias **sobre la misma variable**, para que el sumando intermedio
   comparta pseudo con `offset` y salga `add r8,r0,r9` + `mullw r8,r8,r11` como
   el objetivo.

Camino medido (todo 136/136 B):

| ensayo | fuzzy | filas |
|---|---:|---:|
| base | 85,73529 % | 15 |
| barrera `"r"(offset),"r"(endOffset)` delante de `result=1` | 92,20588 % | 10 |
| barrera `"r"(offset),"r"(sampleSize)` | 93,52941 % | 5 |
| + reordenar el `subf` + `asm volatile("")` | **99,70588 %** | 2 |
| **+ partir la sentencia de `offset`** | **100,0 %** | **0** |

Combinaciones sin uno de los tres: sin la barrera de delante 92,05882 %; sin la
de detras 93,82353 %; sin partir la sentencia 99,70588 %; sin reordenar el
`subf` 93,82353 %. Con las tres, ademas, la barrera de detras puede ser
cualquiera de nueve formas (todas dan 100 %), asi que se ha dejado la mas
simple: `__asm__ __volatile__("")`.

Medido y negativo por el camino: `asm("" : "+r"(result))` detras de los stores
77,79412 %; `return 1` en vez de `result = 1` 83,94118 %; `result = 1` delante o
entre los stores 93,82353 %; `offset <<= 8; offset += ...; offset *= blockSize`
(tres sentencias) 96,91177 %; `blockSize * (...)` 99,55882 %; invertir los dos
stores 91,76471 % pero con el orden al reves del objetivo.

**Los dos `asm` no son deuda**: la funcion pasa de 85,7 % a 100 % y con ella la
unidad entera. Todo escrito en el `.c`.

---

## 2. `pathnode` / `PATHI_calcwaitbeat` — un pin mas, y el DWARF que miente

336/336 B, 98,86905 %, 14 filas, **todas de registro flotante** (f8/f9/f10/f11).
r36d barrio siete formas de la sentencia y r36f once mas; la unidad llevaba dos
rondas dada por «permutador puro».

El truco fue **mover el pin que ya habia** para leer mejor el diff: con `notes`
pinado a `fr10` en vez de `fr11` el resultado baja de 14 filas a **12**, y ahi
el diff deja de ser una rotacion de cuatro registros y se convierte en un
**swap limpio f10 <-> f11**: la cadena `notes`/`scalar` cae en f10 y `fevery` en
f11, exactamente al reves que el objetivo. Con eso a la vista, el pin que
faltaba es evidente:

```cpp
    float scalar;
    register float fevery asm("fr10");
    float foffset;
```

`notes` sigue en `fr11` (como estaba) y **`fevery` en `fr10`**: 336/336 B,
**100,0 %, cero filas**.

**Aviso sobre el DWARF, que costo tiempo**: el volcado dice `scalar // f11` y
`fevery // f11` --el MISMO registro para las dos locales-- y solo la primera es
cierta; en el objetivo `fevery` vive en f10. Un volcado DWARF puede repetir
registro entre dos locales, y entonces solo una de las dos es la buena.

Medido y peor en esta ronda (todo 336/336 B): partir la division
(`scalar = (float)beatinfo->notes; scalar = scalar / (float)note;`) con pin, sin
pin, o quitando la local `notes`: **17 filas las tres**; pin de `notes` a
fr13/fr12/fr8 17 filas, a fr9 15, a fr0 19; pin de `scalar` a fr11 (con o sin
partir) 328 B y 11-15 filas --pierde dos instrucciones--; `notes` fr10 +
`foffset` fr0 332 B / 20 filas.

---

## 3. `pathnode` / `PATHI_nextnode` — la causa, con la linea de GCC

Reproducida la base de r46 (barrera de lectura sobre `forreal` al final + pin
`register int nextnode asm("r12")`): **308/308 B, 96,207794 %, siete filas**.

Lo unico que separa a las dos versiones: el objetivo tiene **una** carga de
`_4Path.pfstate` (indice 8) y una **copia** `mr r6,r9` (indice 10) que
sobrevive hasta `addi r9,r6,0x40` (`pfstate->track`); nosotros tenemos **dos**
cargas, la del inline y una **rematerializacion**
`lwz r9,_4Path.pfstate@sda21` en el indice 25.

- El `mr` del objetivo es la copia que emite `pre_insert_copies` de `gcse.c`.
- Nuestra rematerializacion es `update_equiv_regs` de `local-alloc.c`, y su
  condicion esta escrita literal en el fuente del arbol
  (`orig/prodg/NGC_GNU_SRC/NGC/gcc/local-alloc.c`):

```c
  if (REG_N_REFS (regno) == 2
      && REG_BASIC_BLOCK (regno) < 0
      && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
    reg_equiv_replace[regno] = 1;
```

Un pseudo con **una definicion y un uso** que cruza de bloque y cuya fuente es
la MEM equivalente se sustituye por la MEM. `pfstate` esta en la SDA, asi que su
carga es UNA instruccion y la sustitucion le sale gratis a GCC. Para romperla
hace falta un TERCER uso del pseudo, y en esta funcion no hay ninguno que no
cueste una instruccion.

Medido en r47 sobre esa base, **sin cambio** (308/308 B, 7 filas): local
`PATHFINDERSTATE *pfs` detras del check de `nodeinfo`, `Path::pfstate`
explicito, `pfstate_agg[0]` para `track`, `track` antes de `branches`, partir el
acceso a `track` en dos sentencias, `asm("" : "+r"(pfs))` sobre la local.
**Peores**: `pfs` declarada arriba del todo 304 B / 9 filas;
`asm("" : : "r"(pfstate))` detras del acceso 312 B / 16 filas, delante 312 B /
15, antes de `branches` 312 B / 13 --el `asm` vuelve a cargar, no es de cero
bytes--; barrera de ranura delante de `track` 13 filas y detras 15;
`asm("" : "+r"(track))` 15 filas; leer `pfstate` por puntero volatil 312 B /
14 filas.

### Propuesta fuera de territorio (la misma que r46, ahora con el mecanismo)

La copia nace dentro del inline **`PATHI_getnode` de
`src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathi.h`**, cabecera compartida
por las 13 unidades de `path`. **No la he tocado.** Lo que hay que conseguir ahi
es que el pseudo que lleva `Path::pfstate` desde el inline hasta el uso de
`pfstate->track` en el llamante tenga **mas de dos referencias**, para que
`update_equiv_regs` no lo sustituya por la MEM. Cualquier cambio en `pathi.h`
hay que medirlo sobre las 13 unidades a la vez: el propio fichero documenta que
la vista agregada `pfstate_agg` da −896 B en `pathnode` pero +188 en `pathtrack`
y +392 en `pathinit`.

**Vale 4.204 B de `linked`**: es lo unico que le falta a `pathnode`.

---

## 4. `sfir` — el mecanismo localizado, y una local de menos (fidelidad, 0 B)

`calcFIRCoeffs`, 940 B contra 936. El deficit sigue siendo **+1 instruccion**.

### Que es exactamente

Es **PRE de `gcse.c`**, y el volcado `-da` lo imprime literal:

    PRE: redundant insn 533 (expression 7) in bb 27
    PRE: redundant insn 565 (expression 7) in bb 28
    PRE/HOIST: end of bb 6, insn 705, copying expression 7 to reg 303
    PRE/HOIST: end of bb 6, insn 708, copying expression 8 to reg 307
    ... (bb 8, 9, 12, 15)

con `expression 7 = (ashift:SI (reg/v:SI 87) (const_int 2))` (`halfLen*4`) y
`expression 8 = (plus:SI (reg/v:SI 82) (const_int 32))` (`&pfir->coef[0]`). El
objetivo no hace ninguna de las dos: recalcula las cinco `slwi r0,r26,2` y las
cuatro `addi rN,r29,0x20`.

El interruptor esta en `orig/prodg/NGC_GNU_SRC/NGC/gcc/gcse.c`:

```c
  /* gcse.c:737 */
  if (optimize_size)  changed |= one_classic_gcse_pass (pass + 1);
  else                changed |= one_pre_gcse_pass (pass + 1);

  /* gcse.c:1884 */
  int antic_p = ! optimize_size && oprs_anticipatable_p (src, insn);
```

o sea que **con `optimize_size` gcse hace CPROP pero NO inserta**, que es
exactamente el comportamiento del objetivo (tiene `cmpwi`/`subfic` con el 4
propagado --eso es cprop-- pero ningun bloque puente).

**Y sin embargo `-Os` no es la respuesta.** Compilado con `-Os` los tres
mnemonicos que nos separan **cuadran exactamente** (addi 13, mr 5, slwi 11, los
del objetivo) pero se rompe el reparto flotante --stfd 6/3, lis 19/23, lfs
22/24, lfd 15/13, mfcr 2/1-- via `local-alloc.c:1506/1527` y
`toplev.c:4419` (`recompute_reg_usage (insns, ! optimize_size)`): **924 B /
57,97863 %**. Conclusion: el objetivo NO se compilo con `-Os`; PRE corrio igual
que aqui y no encontro las expresiones. **No hay sospecha de cflags que
proponer.**

### Medido y negativo (base 940 B / 89,25214 %)

- `-Os` 924 B / 57,979 %.
- `-fno-gcse` 243 instrucciones: `mr` y `slwi` **cuadran** pero CMP 17 contra 10
  y se pierde el par `mfcr`/`mtcrf`.
- `-fno-gcse` + literales `4` en los limites de bucle: **233 instrucciones**
  contra 234; cuadran `mr`, `slwi`, `addi` y las ramas; faltan `mfcr`/`mtcrf` y
  sobran 2 CMP. Lo mas cerca que se ha estado, y sigue sin ser el objetivo.
- **Las cinco versiones de ProDG del arbol** (3.5, 3.5b140, 3.7, 3.8.1, 3.9.3)
  dan el **mismo objeto**: no es el compilador.
- `halfLen = 4` repetido tras el switch 944 B; `default: halfLen = 4; break;`
  928 B pero pierde `mfcr`/`mtcrf`; `default: break;` en primera posicion,
  `int halfLen = 4` con inicializador: identicos a la base;
  `const int halfLen = 4` 900 B / 77,41 %.
- Pin `halfLen` a **r26** (que es lo que dice el DWARF) 980 B / 68,49 %; pin
  `cnt` a r31 920 B / 79,08 %; los dos 964 B / 63,95 %.
- `asm("" : "+r"(halfLen))` dentro del `if` de la cola 944 B, antes de la cola
  956 B, los dos 944 B; sobre `pfir` 960 B y 944 B.
- Reescrituras de la cola (`switch` en vez de `if`, `+=` explicito, orden de
  `sum`, `4 || 2` en el hamming, division sin llaves): **las cinco identicas**.

### Lo que si se ha arreglado: una local que el original no tiene

El DWARF (`symbols/mw_dwarfdump.nothpp`) da las locales del original y son
**cinco** --`sum` f31, `tmpFloat` f29, `halfLen` r26, `cnt` r31,
`fir_coef_var` (sin registro)--: **no existe `halfTmpFloat`**, que era una local
nuestra. Se ha quitado escribiendo `... * 0.5f * tmpFloat` en los dos argumentos
del `case 4`, y el objeto sale **BYTE-IDENTICO** (940 B, sha256 de la funcion
`03227a79de44c912`, el mismo que la base). Con eso la lista de locales y su
ORDEN coinciden exactamente con el DWARF.

**Trampa medida**: las formas que agrupan `(tmpFloat * 0.5f)` --con parentesis,
con `/ 2.0f`, con `(0.5f * tmpFloat)`-- bajan a **936 B, el tamano exacto del
objetivo**, pero NO por quitar el puente: `fold` las reasocia a
`((a-b)*0.5f)*tmpFloat`, el bucle del `case 4` pierde el `fmuls f30,f29,f27`
compartido, gana un `fmuls` y un preservado flotante, y cae a 87,05983 %. **Los
936 B de esas formas son una coincidencia.**

---

## 5. `madidct` — la receta reproducida, y la cantidad fantasma CERRADA

La receta de r46 (`asm("" : "+c"(src6))` + `asm("" : "+l"(t9))`) se reproduce
exacta: **516/516 B, 48,24031 %**. Pero el diff sigue teniendo **~120 filas
distintas de 154**: no llega a cero ni de lejos, asi que sigue siendo deuda de
`asm` por cero bytes y **no se deja puesta**.

Lo nuevo es el diagnostico, sacado de los volcados RTL propios. El `.greg`
imprime:

    IdctColumn  ->  94 in 65 (LR), 102 in 66 (CTR), 106 in 68 (CR0)
    IdctRow     -> 147 in 65 (LR), y NADA en CTR
                   ;; Hard regs used:  0 3 4 5 6 7 8 9 10 11 12 14..31 65

O sea: nuestro `IdctRow` mete **UN** pseudo en registro especial (LR) y el
objetivo mete **DOS** (uno en CTR con dos lecturas, otro en LR). Las cinco
instrucciones que faltan son exactamente `mtctr` x1 + `mfctr` x2 + dos `mr`. Y
`REG_ALLOC_ORDER` de `rs6000.h` pone **66 (CTR) antes que 65 (LR)**
(`..., 12, 64, 66, 65, 73, 1, 2, 67, 76`), asi que al segundo pseudo que no
cabe en GPR le tocaria CTR sin pelear: **lo que falta es presion**, un valor
vivo mas.

**La cantidad fantasma, bien formada, es NEGATIVA aqui.** r44 dejo escrito que
la serie de guardas r14..r31 no movia nada «pero eran cantidades sin
inicializar». Repetida con la forma correcta del brief --`register int g
asm("rN");` + `__asm__("" : "=r"(g));` arriba y
`__asm__("" : "+r"(t6) : "r"(g));` delante de `dest[0]`, que **si** toca un
valor vivo y no la barre el DCE-- se han medido **los 24 registros** r3..r12 y
r14..r31 uno a uno y **ocho combinaciones** de dos a cinco guardas:

- **Los 32 ensayos siguen en 496 B.** Unicas excepciones: r4 da 500 B y la terna
  r23+r25+r29 da 512 B con 24,64 %.
- El fuzzy sube hasta **52,99 %** (r17), **52,58 %** (r26), **52,35 %**
  (r26+r27) sin mover el tamano: la trampa del fuzzy de manual.

Conclusion: la presion no se sube desde la fuente en esta funcion. **El eje de
las guardas queda cerrado**, ahora si con la forma correcta.

---

## 6. `criticalpath` — dos ejes barridos enteros, los dos negativos

`VP6_PredictFilteredBlock`, 740/740 B (tamano exacto), 94,39459 %, 40 filas
concentradas en un solo bloque. El frente sigue siendo el adelanto del
`lwz 0x88(r31)` (`FrameReconStride`) por delante del `lhax` de `mVx`, que es
scheduling puro.

**Los cinco pines que hay puestos son un CONJUNTO, no cinco decisiones
sueltas.** Barridas las 5 supresiones sueltas y las 10 parejas:

| quitado | fuzzy |
|---|---:|
| ninguno (base) | 94,39459 % |
| `OutputPtr`(r24) / `TempPtr2`(r27) / `Stride`(r26), sueltos | 94,39459 % (neutros) |
| `bp`(r23) | 94,28648 % |
| `ModY`(r25) | 94,01621 % |
| parejas | de 93,50270 % a 94,39459 % |
| **los cinco** | **93,23243 %** |

Ninguna mejora: no tocar el conjunto.

**Pines NUEVOS guiados por el DWARF**, uno a uno sobre esa base:
`TempPtr1`→r8 736 B / 92,52973 %, `ModX`→r28 736 B / 92,03243 %,
`MvModMask`→r3 92,65946 %, `SrcPtr`→r27 93,69190 %, `TempBuffer`→r29
93,80000 %, `BicMvSizeLimit`→r6 94,28648 %, `MvShift`→r30 94,39459 %
(exactamente neutro). **El eje del pin esta agotado.**

**Orden de sentencias**, ocho formas del bloque interior (ModY antes de ModX, my
antes de mx, las dos, cadena `y` entera y luego cadena `x`, y al reves): las seis
primeras dan **el mismo objeto**; las dos que funden el desplazamiento en la
asignacion bajan a 93,80000 %. **Cola del bloque**, ocho formas: `Stride`
delante 94,39459 %, `TempPtr1 = TempPtr2 = 0` delante 94,39459 %, separadas en
ese orden 94,39459 %, al reves 94,17838 %, `my * FrameReconStride`
**94,556755 %** (el mejor, como en r46, y sigue siendo 0,16 pt sin un byte),
parentizar `(SrcPtr + Recon) + (...)` 89,75676 %, `mx` antes del producto
90,98919 %.

**Deuda de DATOS, sin cambios**: `.rodata` mide 632 B en nuestro objeto y 636 en
el objetivo; falta un objeto `const` de 4 B a cero al final de la seccion,
detras de `loMaskTbl_VP60` (comprobado que **no** es una entrada de la tabla).

---

## Lo que se lleva la ronda (para el brief)

1. **El `.greg` convierte un near-miss de reparto en un diagnostico.**
   `;; N conflicts: ... 0 1` dice con que hard regs choca cada allocno, y
   `;; Register dispositions` dice donde acabo cada pseudo. En `spchsamp` eso
   destapo que el problema no era el reparto sino el planificador, despues de
   que tres rondas atacaran el reparto.
2. **Si el reparto no sale, mira que se planifica en la latencia.** En
   `spchsamp` la asignacion `result = 1` se colaba en la sombra del `mullw` y
   por eso `result` chocaba con los temporales de la cola.
3. **Partir una sentencia sobre la MISMA variable fuerza el compartir pseudo.**
   `x = (x<<8)+k; x = x*m;` da `add r8,r0,r9` + `mullw r8,r8,r11` donde la
   sentencia unica da `add r0,...` + `mullw r8,r0,r11`. Cerro las dos ultimas
   filas de `spchsamp`.
4. **Mover un pin que ya existe es una herramienta de LECTURA.** En
   `calcwaitbeat`, cambiar el pin de `notes` de fr11 a fr10 no mejoro nada
   (mismo fuzzy) pero convirtio una rotacion de cuatro registros en un swap de
   dos, y el pin que faltaba salio solo.
5. **El DWARF puede dar el MISMO registro a dos locales y solo una es cierta**
   (`scalar // f11` y `fevery // f11`, cuando `fevery` vive en f10).
6. **La cantidad fantasma no siempre sube la presion.** En `IdctRow`, 24
   registros y 8 combinaciones con la forma correcta (con valor vivo en el
   `asm` de cierre) dejan los 496 B intactos.
7. **`optimize_size` cambia el modo de gcse** (`gcse.c:737` y `:1884`): clasico
   sin insercion en vez de PRE. Es el interruptor a mirar cuando al objetivo le
   FALTAN los bloques puente que nosotros tenemos --pero `-Os` arrastra
   `local-alloc.c:1506/1527` y `toplev.c:4419` y rompe el reparto.

## Verificacion

- `fncmp` **antes y despues** sobre las cinco unidades, diff completo: las
  unicas lineas que cambian son las dos funciones cerradas. **Ninguna funcion
  empeora, en ninguna unidad.** (`scratchpad/r47_libs/fncmp_before.txt` contra
  `fncmp_after.txt` / `fncmp_final.txt`.)
- `python scripts/lcfix.py --check` → `todas las entradas @lc estan al dia`.
- `fndiff` = **100.0** en las dos cerradas (citado arriba).
- Los tres ficheros que **solo** llevan comentario nuevo (`madidct.cpp`,
  `criticalpath.c`) se recompilaron desde su copia de base y desde la version
  editada: `.text`, `.rodata`, `.data` y `.sdata` **byte-identicos** (sha256 de
  cada seccion). `sfir.c` lleva ademas el cambio de fuente, y la funcion sale
  byte-identica (sha256 `03227a79de44c912` antes y despues).
- Finales de linea comprobados uno a uno: `madidct.cpp` (197/197), `sfir.c`
  (162/162) y `spchsamp.c` (88/88) son **CRLF puro**; `pathnode.cpp` (663) y
  `criticalpath.c` (2085) son **LF puro**. Los cinco como estaban.
- Todo se compilo con `python scripts/build_direct.py <unidad>`. **No se lanzo
  ningun `ninja` ni `configure.py`**, no hay commits ni `git add`, y no se toco
  `configure.py`, `config/GOWE69/*` ni `splits.txt`.

## Ficheros modificados

| fichero | que lleva |
|---|---|
| `src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c` | **codigo** (cierre) + nota |
| `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp` | **codigo** (cierre de `calcwaitbeat`) + veda nueva de `nextnode` |
| `src/Speed/Indep/Libs/snd/9/source/library/mix/sfir.c` | **codigo** (fidelidad, 0 B) + veda |
| `src/egami/rcmp/dev/source/decoder/cmn/madidct.cpp` | solo veda |
| `src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | solo veda |

Ensayos y utilidades en `scratchpad/r47_libs/` (`probe.py`, `tryv.py`, `m.py`,
`mflags.py`, `flags.py`, `rtl.py`, `hist.py`, `rows.py`, `bytes.py`,
`secs.py`, `textsha.py`, `vers.py`, `base/`).
