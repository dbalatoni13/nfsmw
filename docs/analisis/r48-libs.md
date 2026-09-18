# r48 — bibliotecas de GCC (`libs`)

Territorio: `madidct`, `sfir`, `criticalpath`. Entrada: **2.824 B en 4 funciones**.

## Resultado

**Cerrada una funcion, 936 B, y con ella una UNIDAD ENTERA**, mas la **deuda de
datos de `criticalpath`**, que era el unico bloqueo estructural de una unidad de
12.040 B.

| unidad | funcion | antes | despues |
|---|---|---:|---:|
| `Speed/Indep/Libs/snd/9/source/library/mix/sfir` | `calcFIRCoeffs__FP11SNDFIRSTATEi` | 940/936 B / 89,25214 % (75 filas) | **936/936 B, 100,0 %, 0 filas** |

`fndiff` citado literal:

    # calcFIRCoeffs__FP11SNDFIRSTATEi  target=100.0%  ours=100.0%  size=936/936

`fncmp`: `sfir` pasa de **1 de 1** a **0 de 1** funciones con el codigo distinto.
`madidct` y `criticalpath` **no se mueven** (2 de 3 y 1 de 21, los mismos
numeros de la entrada).

`criticalpath`: `.rodata` pasa de 632 a **636 B y BYTE-IDENTICA** al objeto
extraido (sha256 `11fa9fa36e095736` los dos), y `promote.py` pasa de
«`.rodata` mide 632 B y el extraido 636 B» a **«LIMPIA: se puede marcar
Matching»**.

---

## 1. `sfir` / `calcFIRCoeffs` — CERRADA: PRE se apaga desde la FUENTE

r47 dejo localizado el mecanismo (PRE de `gcse.c` nos crea un bloque puente en
la arista `default` del switch y tres copias `mr`) y lo dio por dependiente del
flag `optimize_size`, con `-Os` midiendo 924 B / 57,98 %. **No hacia falta
ningun flag: la condicion es de fuente, y esta escrita en el arbol.**

### El mecanismo, con las tres lineas de GCC

```c
/* gcse.c:1884, hash_scan_set */
int antic_p = ! optimize_size && oprs_anticipatable_p (src, insn);

/* gcse.c:1200, oprs_unchanged_p, caso REG con avail_p == 0 */
return (reg_first_set[REGNO (x)] == NEVER_SET
        || reg_first_set[REGNO (x)] >= INSN_CUID (insn));
```

y `reg_first_set` **se reinicia en cada bloque basico** (`compute_hash_table`).
Es decir: una ocurrencia deja de ser anticipable en cuanto **alguno de sus
operandos se ESCRIBE antes que ella dentro de su mismo bloque**.

Y con eso basta para apagar PRE **entero** para esa expresion, porque
`pre_delete` recorre **solo** `expr->antic_occr`; sin ocurrencia anticipable no
hay borrado, sin borrado no se crea `reaching_reg`, y `pre_edge_insert` y
`pre_insert_copies` **empiezan los dos comprobando `reaching_reg != NULL`**.

El volcado (`python scripts/rtldump.py sfir calcFIRCoeffs -dG`) nombra las dos
expresiones y los bloques exactos:

    PRE: redundant insn 535 (expression 7) in bb 27, reaching reg is 307
    PRE: redundant insn 567 (expression 7) in bb 28, reaching reg is 307
    PRE: redundant insn 537 (expression 8) in bb 27, reaching reg is 311
    PRE: redundant insn 569 (expression 8) in bb 28, reaching reg is 311
    PRE/HOIST: end of bb 6, insn 707, copying expression 7 to reg 307
    ...
    Index 7:  (ashift:SI (reg/v:SI 86) (const_int 2))   -> halfLen*4
    Index 8:  (plus:SI (reg/v:SI 82) (const_int 32))    -> &pfir->coef[0]

(El mismo volcado enseña que **el objetivo NO se compilo con `optimize_size`**:
PRE hoistea tambien `expression 4 = (compare filtType 3)`, y de ahi salen los
`mfcr`/`mtcrf` que el objetivo **si** tiene. `-Os` los pierde. Eso cierra la
duda que dejo r47.)

### El arreglo, tres piezas y ninguna es un flag

```c
    __asm__ __volatile__("" : "+r"(halfLen), "+r"(pfir));
    sum = sum + sum - pfir->coef[halfLen];
    __asm__("" : : "f"(sum));
    if (sum < 0.0f) {
        sum = -sum;
    }
    halfLen = 4;
    for (cnt = 0; cnt <= halfLen; cnt++) {
        pfir->coef[cnt] /= sum;
    }
```

1. **`asm` inout con LOS DOS operandos, en el bloque de la cola (bb 28).** Es un
   `(set (reg))` de los dos pseudos, cuesta cero bytes, y mata el puente y las
   cinco copias `mr` de golpe.
2. **`halfLen = 4;` delante del ultimo bucle.** El `asm` deja `halfLen` opaco
   para cprop y el bucle final pasaria de `cmpwi r31,4` a `cmpw` mas una guarda
   (+2 CMP, +1 BGT). Escribir `cnt <= 4` en ese bucle da **el mismo objeto**.
3. **`__asm__("" : : "f"(sum))` de SOLO ENTRADA** detras de la resta: coloca la
   carga de `$LC5` (0.0f) detras del `fsubs` en vez de delante. Cero bytes.
   El `__volatile__` del `asm` inout es la tercera pieza (equivale a poner el
   inout no volatil mas una barrera de ranura delante: identico).

### Camino medido (`scripts/fndiff.py`; filas = lineas `>>>`)

| ensayo | tamaño | fuzzy | filas |
|---|---:|---:|---:|
| base | 940 B | 89,25214 % | 75 |
| `asm` inout (no volatil) SOLO en bb 28 | 940 B | 93,82478 % | 35 |
| + `halfLen = 4` | 932 B | 97,24359 % | 11 |
| + `asm("" : "+r"(pfir))` dentro del `if (sum<0)` | 936 B | 98,20513 % | 8 |
| + `asm("" : : "f"(sum))` | 936 B | 99,05983 % | 5 |
| + barrera de ranura delante de la resta | **936 B** | **100,0 %** | **0** |

Y luego **podado**: el `+r`(pfir) del `if (sum<0)` sobra (sigue en 100 %) y la
barrera de ranura se funde en el inout haciendolo `__volatile__` (100 %). Quedan
**dos `asm` de cero bytes y una reasignacion**; el tamaño es exacto y el diff es
cero, asi que por la regla 9 del brief **no es deuda**.

### Medido y negativo, con la cifra

- `asm` inout **solo en bb 27** (el bloque del `if (filtType == 3)`):
  944 B / 90,02137 % / 57 filas. **En los dos bloques**: 944 B / 88,93 % / 72.
- **Dos `asm` sueltos** en bb 28 en vez de uno con dos operandos:
  976 B / 88,97 % / 77 filas.
- Solo `halfLen` en bb 28: 956 B / 84,42735 % / 120. Solo `pfir`: 960 B /
  87,73505 % / 87.
- **La forma de SOLO ENTRADA no vale para esto**: `asm("" : : "r"(halfLen),
  "r"(pfir))` en bb 28 da 932 B / 87,83761 % / 91 filas. Es un **uso**, no un
  `set`, y `reg_first_set` no se entera. (Para el efecto de programacion de la
  pieza 3 si vale, y ahi es la buena.)
- Sin `halfLen = 4`: 944 B / 95,448715 % / 28 filas.
- `asm("" : "+f"(sum))` en vez de la entrada: 936 B / 98,16239 % / 9 filas.
- Sobre la base de 5 filas: `sum += sum - coef[halfLen]` 99,0812 % / 4;
  `tmpFloat = pfir->coef[halfLen]` antes 99,0812 % / 5; partir la resta en dos
  99,05983 % / 5; `-(coef - (sum+sum))` 99,05983 % / 5;
  `asm("" : : "f"(pfir->coef[halfLen]))` delante 948 B / 34;
  añadir `"r"(pfir)` a la entrada 936 B / 35.
- `asm` inout tras el `if (sum<0)` en vez de delante de la resta: 944 B / 65
  filas.

---

## 2. `criticalpath` — la deuda de DATOS cerrada (y vale 12.040 B de `linked`)

`.rodata` medía 632 B contra 636. r46 dejo comprobado que los 4 B **no** son una
entrada de `loMaskTbl_VP60`. El objeto extraido lo dice con nombre y todo:

    gap_05_80412774_rodata   .rodata  off=632  size=4  bind=GLOBAL   (0x00000000)

Es el patron que ya usa **`TokenEntropy.c` de este mismo paquete**: un `asm` de
fichero detras del ultimo objeto de `.rodata`.

```c
asm(
    ".section .rodata\n"
    ".globl gap_05_80412774_rodata\n"
    "gap_05_80412774_rodata:\n"
    "  .skip 4\n"
    ".section .rodata\n"
);
```

**Sin `.size`** a proposito: un hueco con `.size` lo estripa el troceador.

Verificado: `.text` **byte-identico al de HEAD** (sha256 `226b1425800d1316`,
12.040 B, no cambia ni una instruccion), y `.rodata` **byte-identica al objeto
extraido** (`11fa9fa36e095736`, 636 B). `.data` y `.sdata` ya lo eran.

    $ python scripts/promote.py Packages/vp6/1.0.6/source/decode/gc/criticalpath
    === Packages/vp6/1.0.6/source/decode/gc/criticalpath
        LIMPIA: se puede marcar Matching

Lo unico que ya separa a la unidad de promocionar es `VP6_PredictFilteredBlock`.
Cuando caiga valen **12.040 B de `linked`**, no 740.

### La funcion: el suelo baja de 40 filas a 25, pero no cierra

Cero bytes con el tamaño ya exacto (740/740), asi que **se ha revertido** y la
receta queda escrita en el `.c`. Las dos palancas:

1. `__asm__("" : "+r"(mVx));` detras de las declaraciones del bloque interior
   (la de r36f): **29 filas**, 93,562164 %. **Casa la cola entera** (TempPtr1 en
   r8 como dice el DWARF, y con el el orden de `add r3,r29,r8` / `add r4,r29,r27`
   en las cuatro llamadas a `FilterBlock`).
2. **NUEVA**: sacar `Recon` a una local con barrera —la misma idea que cerro
   `dvd_device` en r46, *la palanca esta en el operando que NO sale en el diff*:

```c
   { int rec = pbi->mbi.Recon;
     __asm__("" : "+r"(rec));
     TempBuffer = SrcPtr + rec + pbi->mbi.FrameReconStride * my + mx; }
```

   **25 filas**, 95,0 %, tamaño exacto.

Lo que queda en esas 25 filas es `sched1` en un solo bloque: nuestro
`lwz r8,0x88(r31)` ocupa la ranura 28 (en el objetivo es el `lhax` de `mVx`; el
`lwz` va en la 33), y nuestros `and` de `ModX`/`ModY` se **hunden** al final del
bloque (indices 51/52) cuando el objetivo los tiene en 32 y 40 pegados a cada
`lhax` —por eso el objetivo reutiliza registro (`mVy`/`my` comparten r0,
`mVx`/`mx` comparten r10, como dice el DWARF) y nosotros no—. Las 5 filas de la
rama `else` son solo el registro de `MvModMask` y caeran solas.

**Medido y negativo con la cifra** (todo 740/740 salvo nota):

- Asociatividad de `TempBuffer` sobre la base de 29 filas: `+ (stride*my + mx)`
  53 filas, `(my*stride + mx)` 54, `SrcPtr + (Recon + (...))` 59,
  `(mx + stride*my)` 51, `my*stride` sin parentesis 30. Sobre la base limpia:
  48 / 54 / 60 / 43 / 34.
- Orden de sentencias del bloque interior (ModY antes, cadena `y` entera y luego
  `x`, `my` antes de `mx`): **las tres el mismo objeto**, 29 filas.
- **La forma de solo entrada NO es de cero bytes aqui**: `: : "r"(mVx)` 744 B /
  55 filas; sobre `mx`/`my`/`SrcPtr`/`ModX+ModY` 744 B / 64; sobre `ModY` al
  final 744 B / 62. Aqui la que vale es el inout.
- Barrera de ranura `__volatile__("")` delante de `ModX` 736 B / 57 filas, al
  final del bloque 736 B / 58.
- Sobre la base de 25 filas: inout de `ModY` tras `ModY` 27, de `ModX` tras
  `ModX` 31, los dos 30, `mVx`+`mVy` juntos 33, solo `mVy` 736 B / 55,
  `rec`+`my` 32, `rec`+`mx` 53, `rec`+`ModY` 26, `rec`+`pbi` 744 B / 63, `rec`
  con `pbi` de entrada 25 (identico), `rec` volatil 736 B / 52, `rec` de solo
  entrada 744 B / 64, local `bse = SrcPtr + Recon` con barrera 30, `frs` local
  con barrera 44, `frs` local sin barrera 29, `TempBuffer = ... + mx;
  TempBuffer += stride*my;` 40.
- Inout sobre `pbi` en cualquier sitio: 744 B y de 55 a 63 filas.

---

## 3. `madidct` — el eje del ORDEN DE SENTENCIAS barrido y negativo

No cierra. Lo nuevo son tres acotaciones y un barrido que las vedas anteriores
no habian tocado.

### Tres datos que acotan el problema

1. **El deficit de `IdctRow` contado por mnemonico sobre la base LIMPIA** (sin
   ningun `asm`) son exactamente tres entradas y nada mas:

   | mnemonico | objetivo | nuestro |
   |---|---:|---:|
   | `mtctr` | 1 | 0 |
   | `mfctr` | 2 | 0 |
   | `mr` | 10 | 8 |

   **`mflr` y `mtlr` CUADRAN en la base.** El «mflr 2/3, mtlr 2/3» de r46 era ya
   *con la receta puesta*, y eso desplazaba el diagnostico.
2. **Las 13 ultimas instrucciones son identicas en forma** en los dos lados
   (mismo orden de calculo y el mismo orden de stores, con `dest[7]` primero):
   el problema esta acotado al centro de la funcion.
3. **`IdctColumn` (632/632, tamaño ya exacto) NO es reparto puro.** El
   histograma da `lwz` 18/20 y `mr` 12/10: **nosotros RECARGAMOS de la pila lo
   que el objetivo mantiene en registro**. Concretamente, en la cadena de `or`
   del atajo los dos guardamos `src[4]` en `0x8(r1)`, pero el objetivo lo sigue
   teniendo en r5 (`or r0,r0,r5`) y nosotros hacemos `lwz r11,0x8(r1)`.
   **Los dos lados usan LR *y* CTR en esta funcion**, asi que el mecanismo
   «pseudo a registro especial» **si se alcanza en nuestra compilacion**: lo que
   falta en `IdctRow` es presion, no la posibilidad.

### El mapa de lineas del original NO EXISTE para esta unidad

`symbols/debug_lines.txt` tiene **una sola entrada** para todo el rango
`0x8034BAC4..0x8034C0A4`:

    0x8034BAC4: D:/env/egami/rcmp/dev/source/decoder/cmn/madidct.cpp (line 73)

mientras que `maddec.cpp`, su vecino en el mismo directorio, trae linea por
instruccion. Las **tres** funciones de `madidct.cpp` se atribuyen a la linea 73.
Eso encaja con que los cuerpos vinieran de **una macro** (o de una sola sentencia
logica) y explicaria por que ninguna reordenacion de sentencias mueve nada: en el
original no habia sentencias que ordenar. **No lo he perseguido**; queda anotado
porque es la pista mas concreta que hay y no cuesta nada comprobarla
(`#define IDCT_1D(...)` usado por las dos funciones).

### Barrido nuevo: el ORDEN de las sentencias de `IdctRow`

Base 496 B / 46,372093 % / **137 filas**.

| ensayo | tamaño | fuzzy | filas |
|---|---:|---:|---:|
| bloque `t8`/`t9`/`t6`/`t7` delante del bloque `t1..t5` | 452 B | 24,66 % | 157 |
| solo `t8 = MULT(...)` arriba del todo | 496 B | 38,23 % | 143 |
| `t9 = src[2]+src[6]; t9 = t9 + t8;` (partido) | 496 B | 48,92 % | 136 |
| `t9 = t8 + (src[2] + src[6]);` | 496 B | 44,19 % | 140 |
| `t6`/`t7` arriba del todo | **504 B** | 41,08 % | 143 |
| `t9 = (src[6] + src[2]) + t8;` | 496 B | 46,36 % | 138 |
| `t8` intercalado detras de los MULT del bloque 1 | 496 B | 46,37 % | 137 (objeto identico) |
| solo `t6` arriba | 504 B | 41,08 % | 143 |
| solo `t7` arriba | 496 B | 51,30 % | **134** |
| `t6`/`t7` arriba + `t8` arriba | 508 B | 34,47 % | 149 |
| `t6`/`t7` arriba + `t9` partido | 504 B | 41,47 % | 143 |
| `t6`/`t7` arriba + la receta `"+c"`/`"+l"` | 516 B | 49,16 % | 138 |

Ninguna baja de 134 filas y ninguna llega a 516 B sin la receta. **Subir la
presion moviendo `t6`/`t7` arriba SI cuesta 8 B (504)** —es el unico eje de
fuente que mueve el tamaño— pero coloca mal el resto.

### La receta de r46, reproducida y no retenida (otra vez)

516/516 B, 129 instrucciones contra 129, 48,24031 %, **135 filas**. Dato nuevo:
el `mtctr` cae **en la misma ranura que el objetivo** (indice 8). Lo que sobra a
partir de ahi es la vuelta `mfctr` + `mtlr` de los indices 11 y 13: el objetivo
**lee CTR dos veces** (`mfctr` 2) y nosotros lo sacamos a un GPR que el asignador
acaba metiendo en LR (`mflr` 3 / `mtlr` 3 contra 2 y 2). Cero bytes: no se deja.

---

## Propuestas fuera de territorio

### A. `splits.txt`: darle a `sfir.c` su pool de literales — **936 B de `linked`**

`calcFIRCoeffs` esta al 100 % y es **la unica funcion de la unidad** (`.text`
0x8036EDBC–0x8036F164 = 936 B). `promote.py` la rechaza por una sola cosa:

    === Speed/Indep/Libs/snd/9/source/library/mix/sfir
        - secciones que emitimos de mas: .rodata(48B)

Esos 48 B son sus nueve literales (`$LC0`..`$LC8`). En el DOL viven en
**0x80412F60–0x80412F90**, que hoy no lo reclama nadie: `splits.txt` da
`.rodata` a `sfhpffir8.c` hasta 0x80412F60 y a `sfir8.c` desde 0x80412F90, y en
medio solo hay `sfilter.c` (sin `.rodata`) y `sfir.c` (sin entrada de
`.rodata`). El hueco lo cubre el objeto comodin **`auto_05_80412F60_rodata.o`,
que mide exactamente 48 B de `.rodata` y nada mas**, y su contenido es
**byte-identico** al `.rodata` que emitimos:

    3f4ccccd 00000000 43300000 80000000 40490fdb 3f000000
    3f490fdb 00000000 3eeb851f 3f0a3d71 3f800000 00000000

Propuesta: añadir a `Speed/Indep/Libs/snd/9/source/library/mix/sfir.c` en
`splits.txt` la linea

    .rodata     start:0x80412F60 end:0x80412F90

El orden de enlace ya es el correcto (la unidad va entre `sfilter.c` y
`sfir8.c`, que es justo donde cae ese hueco), asi que no deberia mover ningun
dato de sitio —pero conviene medirlo contra el aviso de
`nfsmw-rango-no-basta`—. **No lo he tocado.**

### B. `configure.py`: nada que proponer para `sfir`

r47 dejo abierta la posibilidad de un `-Os` por objeto. **Queda descartada con
medida**: el volcado `-dG` enseña que el objetivo tiene el PRE de
`expression 4 = (compare filtType 3)` —de ahi salen sus dos `mfcr` y los
`mtcrf`—, y `optimize_size` lo apaga (`gcse.c:737`). Con `-Os` el objeto mide
924 B y 57,98 %. **El objetivo se compilo con los cflags que ya tenemos.**

### C. `criticalpath`: promocionable en cuanto caiga una funcion

`promote.py` ya da **LIMPIA**. No hay nada que pedir a coordinacion hoy, pero
conviene que el reparto sepa que esa unidad vale **12.040 B de `linked`** y no
740 de `matched`.

---

## Lo que se lleva la ronda (para el brief)

1. **PRE se apaga desde la fuente, sin tocar un flag.** Un `asm` inout con
   **todos** los operandos de la expresion, **en el bloque donde el volcado dice
   `redundant`**, hace que `oprs_anticipatable_p` sea falso y con eso caen de
   golpe el bloque puente y todas las copias de `pre_insert_copies`. La cadena es
   `antic_occr` → `pre_delete` → `reaching_reg` → `pre_edge_insert` /
   `pre_insert_copies`, y los dos ultimos empiezan comprobando `reaching_reg`.
   **Firma para buscarlo en el arbol**: al objetivo le FALTA un bloque puente que
   nosotros tenemos y le SOBRAN recalculos. Se confirma en un segundo con
   `scripts/rtldump.py <unidad> <fn> -dG`.
2. **La forma de solo entrada y la inout NO son intercambiables.**
   `asm("" : : "r"(x))` es un USO: no toca `reg_first_set`, no apaga PRE, y en
   `criticalpath` ademas cuesta 4 B. `asm("" : "+r"(x))` es un SET. En `sfir`
   hacen falta **las dos**, cada una para una cosa distinta.
3. **Un `asm` inout con dos operandos no es igual que dos `asm` de uno.** En
   `sfir`: 936 B / 100 % contra 976 B / 88,97 %.
4. **Si un `asm` deja una variable opaca, hay que devolverle la constante.**
   `halfLen = 4;` detras de la zona del `asm` vale 12 B (dos `cmpw` y una
   guarda) y no cuesta ninguno.
5. **Un hueco de datos con nombre `gap_*` en el objeto extraido se rellena con
   un `asm` de fichero y sin `.size`** —hay precedente en `TokenEntropy.c`—, y
   eso puede convertir un `promote.py` rojo en LIMPIA sin tocar una instruccion.
6. **Cuenta el deficit por MNEMONICO sobre la base limpia, no sobre un ensayo.**
   En `madidct` el «mflr/mtlr 2/3» de dos rondas era un artefacto de tener la
   receta puesta: en la base cuadran, y el deficit real son solo `mtctr`+`mfctr`
   +2 `mr`.
7. **Cuando una unidad no tiene mapa de lineas es un dato, no un vacio.**
   `madidct.cpp` entero se atribuye a una sola linea mientras su vecino trae
   linea por instruccion: eso apunta a macro, y explicaria que ninguna
   reordenacion de sentencias mueva nada.

## Verificacion

- `fncmp` **antes y despues** sobre las tres unidades
  (`scratchpad/r48_libs/fncmp_before.txt` contra `fncmp_after.txt`): la unica
  linea que cambia es `calcFIRCoeffs`. **Ninguna funcion empeora, en ninguna
  unidad.**
- `fndiff` = **100.0** en la cerrada (citado arriba).
- `python scripts/lcfix.py --check` → `todas las entradas @lc estan al dia`.
- **La fuente vuelve de verdad**: `madidct.cpp` y `criticalpath.c` se
  recompilaron desde la version de HEAD y desde la version editada
  (`scratchpad/r48_libs/textsha.py`). `madidct`: `.text`, `.data` y `.rodata`
  **byte-identicas** a HEAD (`.text` 1484 B, sha256 `bc4067d1394d0dae`) — el
  cambio es solo comentario. `criticalpath`: `.text` **byte-identica** a HEAD
  (12.040 B, `226b1425800d1316`), `.data` y `.sdata` iguales, `.rodata` 632→636
  como se buscaba. Ademas se restauro la version editada y se recompilo: el
  objeto vuelve a salir identico.
- Finales de linea comprobados uno a uno: `sfir.c` **CRLF puro** (243/243),
  `madidct.cpp` **CRLF puro** (258/258), `criticalpath.c` **LF puro** (2154).
  Los tres como estaban.
- Todo se compilo con `python scripts/build_direct.py <unidad>`. **No se lanzo
  ningun `ninja` ni `configure.py`**, no hay commits ni `git add`, y no se toco
  `configure.py`, `config/GOWE69/*` ni `splits.txt`.

## Ficheros modificados

| fichero | que lleva |
|---|---|
| `src/Speed/Indep/Libs/snd/9/source/library/mix/sfir.c` | **codigo** (cierre de `calcFIRCoeffs`) + el mecanismo y el barrido escritos |
| `src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | **datos** (el `asm` del hueco de 4 B de `.rodata`) + veda ampliada |
| `src/egami/rcmp/dev/source/decoder/cmn/madidct.cpp` | solo veda (objeto byte-identico) |

Ensayos y utilidades en `scratchpad/r48_libs/` (`lib.py`, `t_sfir1..8.py`,
`t_crit1..10.py`, `t_mad1.py`, `t_mad2.py`, `mad_recipe.py`, `keep_sfir.py`,
`notes.py`, `fixasm.py`, `textsha.py`, `fncmp_before.txt`, `fncmp_after.txt`).
