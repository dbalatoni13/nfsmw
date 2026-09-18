# r62 - mw: LibSN/steering, madidct, criticalpath

Encargo: las tres unidades "de Metrowerks" (solo steering lo es de verdad;
madidct y criticalpath son ProDG/GCC 2.95.3). 9 funciones abiertas, 4.456 B.

**Resultado: NINGUNA PROMOCIONA, y no se ha aplicado ni un cambio de codigo.**
Lo que traigo son cuatro cosas medidas, en orden de valor:

1. **El `bss+32` de steering esta AISLADO AL BYTE: es el `sh_addralign` de
   `.bss` y nada mas.** Con ese unico campo a 4, la unidad pasa de
   `.text -8  bss+32` a `.text -8`. Y **no se alcanza desde la fuente ni desde
   los cflags** (9 formas medidas). Hay propuesta concreta abajo.
2. **La familia E contestada**: el pragma acotado por funcion **NO se derrama**.
   Tres pragmas sobre `SimThread_Init`, incluido uno que destroza la funcion
   (55,9 %), y las 33 vecinas exactas quedan intactas en los tres. El eje queda
   ABIERTO para las otras cinco funciones de steering.
3. **criticalpath: el pin de `ModY` causa 5 de las 25 filas que quedan**, y aun
   asi quitarlo es peor (39). El frente queda reformulado y acotado. Tres ejes
   nuevos barridos, todos negativos con cifra (barrera volatil, clobber
   "memory", `set_preference`), mas el propio pin.
4. **Una veda de 14 rondas cerrada gratis**: las notas de linea de `-gdwarf+`
   **no cambian el codigo emitido** (medido: 60 lineas de comentario nuevas en
   criticalpath.c dan `.text`/`.rodata`/`.data`/`.sdata`/`.rela.text` byte a
   byte identicos). Eso mata como palanca la pista "los cuerpos de madidct
   vinieron de una macro" (r48 punto 4).

---

## 0. Estado de partida y de llegada (identicos: no hay regresion)

| unidad | fncmp | linkdelta | trypromo |
|---|---|---|---|
| LibSN/steering | 6 de 36 distintas, 2.568 B | `.text -8  bss+32` | DOL ROTO |
| madidct | 2 de 3 distintas, 1.148 B | `.text -20` | DOL ROTO |
| criticalpath | 1 de 21 distinta, 740 B | `.text +0  IGUAL` | DOL ROTO |

`trypromo` de las tres juntas: `DOL ROTO (7779deb36758)`, antes y despues.

Sello triple de los `.o` (tres pasadas de `build_direct.py`, sha1 identico en
las tres):

```
steering      1962b7d16adc91a21e4f8070dacc3113935a219e   (sin cambios)
criticalpath  a4a00513aa7bc1732163806edb3e653cd3091249   (era f5200a22...)
madidct       d7a8d1b0b4daa372f1e55a10c3c4f2fc6085d1ba   (era 99dd931e...)
```

Los dos cambios de sha1 son **de comentarios**: verificado seccion a seccion que
`.text`, `.rodata`, `.data`, `.sdata` y `.rela.text` salen BYTE A BYTE
identicos; lo unico que se mueve es `.debug_line` / `.debug_pubnames`.

---

## 1. steering: el `bss+32` es un solo campo del ELF

`.bss` mide 40 B en los dos lados y su unico habitante es `ia[LG_FILTER_TAPS]`,
el estatico de `SimThread_Init`. La unica diferencia:

```
extraido  .bss  40 B  sh_addralign 4     (su direccion, 0x804B9FFC, no es multiplo de 8)
nuestro   .bss  40 B  sh_addralign 8
```

**Prueba dura** (`scratchpad/mw62/bsstest.py`: se copia nuestro `.o`, se pisan
los 4 bytes del `sh_addralign` de `.bss` y se enlaza el proyecto entero con las
dos versiones):

```
nuestro .o tal cual .............. .text -8   bss +32
el MISMO .o con .bss a4 .......... .text -8
```

Es decir: el `bss+32` **entero** es esa alineacion. Con ella arreglada, a
steering solo le queda **una** causa de colocacion, los -8 B de
`SimThread_Step` (920/924) y `Effect_PerformEnvelope` (192/196).

### No se alcanza desde la fuente ni desde los cflags

Barrido completo, todo con los cflags reales de la unidad
(`scratchpad/mw62/probe.py` y `flagtest.py`):

| forma | resultado |
|---|---|
| `static s32 ia[10];` | `.bss` 40 B **a8** |
| `static s32 ia[10] = {0};` | se va a `.data`, 40 B a8 |
| `#pragma align_array_members off` | `.bss` 40 B a8 |
| `#pragma options align=packed` / `reset` | `.bss` 40 B a8 |
| `#pragma explicit_zero_data on` | `.bss` 40 B a8 |
| `-align 4` | `.bss` 40 B a8; 33 exactas (igual que la base) |
| `-align mac68k4byte` | `.bss` 40 B a8; **20 exactas** (peor) |
| `-align packed` | mwcceppc lo rechaza |
| `-sdatathreshold 0` | `.bss` **60 B** a8; 28 exactas (peor) |
| `__attribute__((aligned(4)))` | (r61) sin efecto |

`mwcceppc GC/2.7` emite **siempre** `.bss` con a8.

### PROPUESTA (no aplicada; toca `tools/`, `configure.py` y el grafo de ninja)

La via es un paso posterior al compilador, igual que `tools/rename_section.py`
hace con `.text=.over`. Tres piezas:

1. **`tools/set_section_align.py`** (nuevo, hermano de `rename_section.py`):
   parchea `sh_addralign` en sitio. Un solo campo por seccion; no mueve ni un
   offset, ni un tamano, ni un indice. Uso:
   `python tools/set_section_align.py -q <obj.o> .bss=4`.
   El parche esta escrito y probado en `scratchpad/mw62/bsstest.py`
   (funcion `set_align`).

2. **`tools/project.py`**, tres retoques:
   - linea 76, junto a `"section_rename": None,` anadir
     `"section_align": None,`
   - junto a `make_section_renames` (linea 370) un `make_section_aligns`
     analogo, validando `<seccion>=<potencia de 2>`.
   - **la parte que NO tiene precedente**: hoy el paso posterior solo existe
     para la regla `prodg` (linea 953, `ngccc_rename_cmd`), y steering usa
     `mwcc_sjis`. Hace falta una regla `mwcc_sjis_align` con
     `"<mwcc_sjis_cmd> && $python <set_section_align> -q $out $section_align"`
     y su rama dentro del `if is_mwcc:` de la linea ~1256.

3. **`configure.py`**, linea 1422, cambiar

   ```python
                    Object(NonMatching, "LibSN/steering.c"),
   ```

   por

   ```python
                    Object(
                        NonMatching,
                        "LibSN/steering.c",
                        # r62: mwcceppc emite .bss con sh_addralign 8 y el
                        # objeto extraido lo trae con 4 (su direccion,
                        # 0x804B9FFC, no es multiplo de 8). Medido: es la causa
                        # UNICA del bss+32 del enlace y no se alcanza ni desde
                        # la fuente ni desde ningun -align.
                        # Ver docs/analisis/r62-mw.md.
                        section_align=".bss=4",
                    ),
   ```

**Aviso de grafo de build** (memory/nfsmw-grafo-de-build-mudo): `build_direct.py`
lee las reglas de `build.ninja` en `parse_units()` y solo aplica el paso
posterior cuando ve `rename`. Si se anade `section_align` hay que ensenarselo
tambien, o los agentes mediran objetos sin parchear mientras ninja los saca
parcheados.

**Huecos `pad_` que crearia: ninguno.** El parche no cambia tamanos ni offsets,
solo la alineacion que se le pide al enlazador.

### Y NO es un frente: medido

El arbol tiene **124 secciones con la alineacion distinta a la del extraido**
(censo en `scratchpad/mw62/aligncensus.py`, 547 pares). Tentador, y falso.
Probado en `zEagl4Anim` (`.bss` 208 B en los dos lados, a8 el extraido contra a4
el nuestro): forzarlo a a8 **lo empeora**, de `.text +4` pasa a
`.text +4 rodata -728 data -96`. Hay que medirlo unidad por unidad; de las seis
unidades con `linkdelta` no-IGUAL, la unica cuyo delta es puramente alineacion
es steering.

(De paso, dato para el que lleve zEagl4Anim: hoy su `linkdelta` es `.text +4`,
sin el `bss+160` que dice el encargo. Se movio.)

---

## 2. steering / familia E: el pragma acotado NO se derrama

Es el ensayo que el encargo pedia, con su observable (contar las vecinas exactas
antes y despues). Base: 33 exactas de 70 simbolos del `.o` (= las 30 funciones
exactas de `fncmp` mas los datos).

| pragma acotado a `SimThread_Init` | tamano | filas | fuzzy | vecinas exactas |
|---|---:|---:|---:|---:|
| (base) | 324 | 9 | 94,691 % | **33** |
| `#pragma scheduling off/on` | 324 | 61 | 55,938 % | **33** |
| `#pragma optimization_level 3` | 324 | 9 | 94,691 % | **33** |
| `#pragma optimization_level 2` | **344** | 70 | 57,852 % | **33** |

**Los tres dejan las 33 intactas, incluido el que destroza la funcion.** El
pragma acotado por funcion es hermetico en esta unidad: el eje esta ABIERTO para
`SimThread_Step` (924 B), `HandleTriggers` (588), `Effect_Init` (276),
`CookValues` (260) y `Effect_PerformEnvelope` (196).

Lo que si queda cerrado es la DIRECCION: `scheduling off` es catastrofico aqui
(la planificacion de MWCC en esta funcion es casi correcta; lo que no casa son
dos insns), y `optimization_level 3` da objeto equivalente a 4, o sea que el
nivel efectivo de esta funcion ya es 3.

### Las 9 filas de SimThread_Init son dos cosas, y las dos estan barridas

- **A) ranuras 51-55**: el objetivo emite `li r3,0` + `mtctr r0` **delante** del
  `lfs f1,@sda21` del bucle de `ia[]`; nosotros detras.
- **B) ranuras 67/71/73/74**: empate desnudo r5/r6 en el bucle final (el
  objetivo pone el cero en r5 y la direccion en r6; nosotros al reves).

Negativos nuevos de la r62, los dos **objeto equivalente** (324 B / 9 filas /
94,691 %):

- `ia[n] = (s32)(a[n] * 4096.0f);` (factores al reves, para retrasar la carga de
  la constante). No mueve la ranura del `lfs`.
- La **ranura explicita**, que es la palanca que SI funciona en este fichero
  (`steering.c:798-808`, y el `clear_value` de `Effect_Init`):
  `register s32 zero; asm { li zero, 0 }` delante del bucle final y
  `st->hist[i] = zero;`. **MWCC coalesce la copia** y sale el mismo reparto: el
  productor explicito no llega al empate r5/r6.

### Correccion al encargo: las mitades de iter4/iter5 YA ESTAN EN EL ARBOL

El informe r60b dice que `Effect_Init` y `CookValues` tienen "media medida en
`scratchpad/codex_20260908_iter4_steering` y `codex_20260908_iter5_*` que NO
esta en el arbol". **Es falso a dia de hoy**, comprobado leyendo el fuente y
midiendo:

- `CookValues`: el arbol tiene `cook_lo` literal (`steering.c:469-494`, con su
  `asm { extsb _mx, _bits }`) y mide 99,077 % / 9 filas / 260 B, que es
  exactamente el numero de `cook_lo` en `iter4`. La cifra "95,77 % / 13 filas"
  del encargo esta rancia.
- `Effect_Init`: el arbol tiene **`effect_clear` Y `effect_seed` combinados**
  (el `asm { li clear_value, 0 }` y el bloque `exponent`/`slot`/`fsubs`), y mide
  95,0 % / 23 filas, que es el numero de `effect_clear` SOLO (`effect_seed`
  solo daba 91,14 % / 32). O sea: **la combinacion ya esta hecha y `effect_seed`
  no aporta nada encima de `effect_clear`**. Ese es justo el negativo que el
  informe pedia medir, y ya estaba medido sin saberlo. No hay que recuperar nada
  de esos directorios.

---

## 3. criticalpath / VP6_PredictFilteredBlock: el frente reformulado

Es lo unico que separa a esta unidad de promocionar: `linkdelta` da
`.text +0, resto IGUAL` y `dolwhere` 163 B, todos dentro de este cuerpo. Son
12.040 B de `.text` de `linked`.

**PASO 0 reproducido sin desviacion** (banco propio: la fuente entera copiada a
scratchpad -- no tiene includes -- con los cflags reales):
base 40 filas, L1 29, L2 40, **L1+L2 25**, todas 740/740 B. Los cuatro numeros
de la r61 al dedillo, asi que el arbol no se ha movido.

### El hallazgo: el pin de ModY causa 5 filas y sostiene 20

`register int ModY asm("r25")` quitado sobre L1+L2: **39 filas** (740 B).
La r47 lo habia medido solo sobre la base sucia de 40 filas y **en fuzzy**
(94,01621 %) -- la trampa de `memory/nfsmw-medidas-que-enganan`. En filas y
sobre la base buena se ve lo que el fuzzy tapaba:

- **CON el pin**, mVy se carga DIRECTAMENTE en r25 (`lhax r25,r6,r9`), asi que
  el `and` de ModY sale `and r25,r25,r3` y no puede emitirse hasta que mVy
  muera: ranura 52. El objetivo hace `lhax r0,...` + `and r25,r0,r3` en la 32.
- **SIN el pin eso se arregla solo**: sale `and r25,r7,r3` (fuente distinta de
  destino) y ademas **desaparecen las dos filas del bloque `else`** (indices 73
  y 74, `and r25,r0,r9` / `and r28,r11,r9`), que con el pin no casan nunca.
- **El precio son 16 filas nuevas en la cola** (25, 27, 44, 58, 60, 65-70, 77,
  85, 110, 112, 115, 119, 121, 125): una rotacion r4/r5/r6/r7/r30 de los scratch
  sin pin. La cola vive del conjunto de cinco pines de la r47.

**El frente ya no es "empujar la ranura 28 con el pin puesto": es recuperar la
cola sin el pin.** Medidas de apoyo: sin pin y sin L2, 43 filas; sin pin, sin L1
ni L2, 50.

### Tres ejes nuevos, todos negativos con cifra

**Barrera volatil**, cinco posiciones, sobre la base de 25 (la r48 solo la habia
medido sobre la base sucia): 736 B en las cinco -- 48, 57, 52, 54 y 56 filas.
**Las cinco pierden una instruccion**: la barrera volatil no es de cero bytes
aqui, deja caer el `mr r5,r11`. Eje cerrado.

**Clobber `"memory"`**, que no estaba en ningun barrido anterior:

- dentro del asm de `rec` (L2): 740 B / 42 filas (el unico de cero bytes)
- suelto, tres posiciones: 736 B / 48, 52 y 56 filas

El clobber suelto se comporta **exactamente** como la barrera volatil, mismos
bytes y mismas cuentas en las mismas posiciones: en GCC 2.95 los dos producen la
misma barrera de scheduling y **no hay que barrerlos por separado nunca mas**.

**La hipotesis de `set_preference` es FALSA** (conviene que quede escrita, porque
es el razonamiento natural al ver el diff): "mVy coge r25 porque MUERE en el
`and` de ModY y `local_alloc` sugiere el registro del destino; dale a mVy un uso
posterior y la preferencia se rompe". Se le dio, con el mecanismo de coste cero
de la r61 (operando de entrada en el asm de `rec`, que va al final del bloque):
`rec + "r"(mVy)`, `+ "r"(mVx)`, y las dos combinaciones en los dos ordenes. Los
cuatro dan **740 B / 28 filas** con sha1 del `.o` distinto, y en el diff **la
fila `and r25,r25,r3` sigue ahi**. Alargar la vida de mVy hasta el final del
bloque no le quita r25. (`rec + "r"(mVy)` y ademas sin el pin: 43 filas.)

### Un item del encargo que ya estaba hecho

El encargo pide medir en VP6 si "un operando de entrada crea arco de dependencia
en haifa" (`prio` 2->3), y dice que ninguna otra funcion del dossier lo mide.
**La r61 ya lo contesto, y con la traza**: sube de 2 a **6**, no gasta ranura
(740/740, la insn del asm sale con code -1 y unit `none`), y aun asi el
resultado es negativo aqui (44 filas). Esta escrito en el bloque r61 de
`criticalpath.c`. La familia C no necesita este ensayo.

---

## 4. madidct: dos correcciones al encargo, y una veda vieja cerrada gratis

No he gastado compilaciones aqui. Las dos cosas que el encargo pedia **ya estan
hechas y son negativas**:

1. El encargo dice "IdctRow (496/516) e IdctColumn (632/632), las dos CADUCAS
   por familia C, con dos `asm` ya presentes donde colgar entradas por cero
   bytes". **La r61 barrio la familia C entera sobre IdctRow -- 34 ensayos,
   todos negativos** -- y mato la receta `int s4 = src[4];` de IdctColumn que el
   informe r60b llamaba "la mas clara de todo el informe" (da 636 B, +4, y sin
   tocar el histograma que dice arreglar). Ademas **no hay ningun `asm` en el
   arbol** en esas dos funciones: los "dos asm ya presentes" son los de la
   RECETA de la r46, que no esta puesta. Confirmado hoy con `fncmp`: IdctColumn
   138 insn, IdctRow `tamano (496/516)`, que son los numeros de la base limpia.

2. **La pista del mapa de lineas (r48 punto 4) esta muerta como palanca.** La
   r48 anoto que `symbols/debug_lines.txt` atribuye las tres funciones de la
   unidad a una sola linea (la 73) y sugirio que los cuerpos vinieran de una
   macro, "lo que explicaria por que ninguna reordenacion de sentencias mueve
   nada"; lo dejo escrito como "la pista mas concreta que hay" y no la
   persiguio. Se cierra sin compilar nada aqui: al meter 60 lineas de
   **comentario** en `criticalpath.c` -- que desplazan el numero de linea de
   todo lo que va detras -- el objeto salio con `.debug_line` distinto y
   `.text`, `.rodata`, `.data`, `.sdata` y `.rela.text` **byte a byte
   identicos**. Las notas de linea de `-gdwarf+` no cambian el codigo emitido, y
   una macro entrega al compilador exactamente las mismas sentencias que el
   fuente expandido a mano. "Era una macro" puede ser cierto historicamente,
   pero **no puede explicar ni una instruccion de diferencia**. Lo que sigue
   vivo del punto 4 es el dato de ATRIBUCION, util para `splits.txt`, no para el
   codigo.

`madidct` sigue en `.text -20`, que son los 20 B de `IdctRow` (496 contra 516).
La receta que da el tamano exacto (r61: `c:t3 + l:t8`, 516 B / 133 filas, la
mejor base de filas medida en esa funcion) sigue sin dejarse puesta: cero bytes.

---

## 5. Higiene: dos de mis tres fuentes NO eran UTF-8

`criticalpath.c` (6 bytes) y `madidct.cpp` (4 bytes) traian comillas angulares
latin-1 (`0xAB`, `0xBB`) metidas por rondas anteriores. **Normalizadas a `"`**
en esta ronda; verificado que el objeto no se mueve. `steering.c` estaba limpio.
Es la misma clase de bomba que describe la regla 7 del encargo. Los tres
ficheros son CRLF.

---

## Lo que dejo, y lo que no

- **Aplicado**: solo COMENTARIOS (bloques `r62` junto a las tres funciones, para
  que `previo.py` los encuentre) y la normalizacion de encoding. Cero cambios de
  codigo, cero `asm` nuevos retenidos, cero regresiones (`fncmp`, `linkdelta` y
  `trypromo` identicos antes y despues).
- **No he corrido `lcfix.py`**. Correcciones de `$LC` pendientes que dejo: **0**.
  Ninguna venenosa: no he movido un solo literal ni una sola constante.
- **Propuesta pendiente del jefe**: `section_align=".bss=4"` para
  `LibSN/steering.c`, con las tres piezas de `tools/` del punto 1. Es lo unico
  de esta ronda que mueve bytes de DOL (`bss -32`).
- Volcados borrados. El banco reutilizable queda en `scratchpad/mw62/`
  (`b.py`, `cpgen.py`/`cprun*.py`, `stgen.py`/`strun*.py`, `bsstest.py`,
  `aligntest.py`, `aligncensus.py`, `probe.py`, `flagtest.py`, `seccmp.py`);
  los `.json`, `.c`, `.o` y `.elf` generados se van.
