# r62 -- `eagl` (zEagl4Anim): el `bss+160` cerrado con UN caracter, y -9.510 B por ORDEN DE EMISION

Agente `eagl`. Unidad: `zEagl4Anim` (en exclusiva).

**Diez ficheros tocados, los diez EXCLUSIVOS de zEagl4Anim** (comprobado: cada
uno lo incluye SOLO `SourceLists/zEagl4Anim.cpp`). **Cero cabeceras
compartidas**, cero `config/`, cero `splits.txt`, cero `keep.lst`, cero
`configure.py`, cero `scripts/`.

    src/Speed/Indep/Src/EAGL4Anim/DeltaChan.cpp
    src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp            (solo comentario)
    src/Speed/Indep/Src/EAGL4Anim/eagl4runtimetransform.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnRunBlender.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnDeltaQ.h  FnDeltaQ.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnDeltaQFast.h  FnDeltaQFast.cpp
    src/Speed/Indep/Src/EAGL4Anim/FnDeltaSingleQ.h  FnDeltaSingleQ.cpp

Las tres `.h` NO son compartidas: `FnDeltaQ.h`, `FnDeltaQFast.h` y
`FnDeltaSingleQ.h` solo las incluyen `FnDeltaQ.cpp`, `FnDeltaQFast.cpp`,
`FnDeltaSingleQ.cpp` y `MemoryPoolManager.cpp`, los cuatro de esta unidad.

---

## 0. Titular

| medida | ANTES (r61) | DESPUES | delta |
|---|---:|---:|---|
| `linkdelta` | `.text +4  bss+160` | `.text +4` **IGUAL** | **cerrado el peor delta de seccion que quedaba** |
| bytes distintos en los rangos de `splits.txt` | 93.279 | **83.769** | **-9.510** |
| ...de los cuales `.text` | 90.368 | **80.895** | -9.473 |
| ...`.rodata` | 2.860 | **2.827** | -33 |
| ...`.data` | 49 | **45** | -4 |
| bytes distintos FUERA de mis rangos (veneno a vecinos) | 15.944 | **15.331** | **-613** |
| `ltorder` (bloques de permutacion en el ENLACE) | 39 | **27** | -12 |
| `textorder` saltos de delta / descolocadas | 75 / 273 | **47 / 251** | -28 / -22 |
| `fncmp` | 2 de 318 (2.808 B) | **2 de 318 (2.808 B)** | **IDENTICO** |

**Cero regresiones de codigo.** `checksplits` (0 solapes, 0 cortes), `gapchk`
(total 0) y `prefijochk` (LIMPIO) de la unidad.

Sello, **tres compilaciones seguidas del arbol final, las tres iguales**:

    zEagl4Anim   005db215f0e0d4ed0faadc5618873cbcffa240b0   (x3)

**NO promociona, y no podia**: `trypromo zEagl4Anim` da `DOL ROTO
(991d08e78669)`. Le quedan 83.769 B de COLOCACION.

`lcfix`: **1 correccion pendiente, mia, y NO es venenosa**:

    CORRIGE  linea 2811: zEagl4Anim.o:$LC109 -> zEagl4Anim.o:$LC124   (la cadena "?")

Medido: con `keep.lst` tal cual y con una copia privada ya corregida, `rangos`
da **83.769 en los dos casos**. Ninguna cifra de este informe depende de ella.

---

## 1. `bss+160`: era UN caracter, y estaba anunciado en el propio fuente

El encargo decia que los 160 B eran "trabajo descubierto" al mover la raya de
`.bss` con `.debug_aranges`. **Lo eran, y el trabajo estaba ya escrito.**

`DeltaChan.cpp:553-557` emite los dos huecos del objetivo como variables de C++:

    char gapAnteQt0[8]   asm("gap_07_8045B1B8_bss");
    float qt0[7];
    char gapTrasQt0[140] asm("gap_07_8045B1DC_bss");

y el comentario de la r60 que hay encima decia, textualmente, que la raya de
`splits.txt` estaba **136 B tarde** y que la buena era `0x8045B1E0`. La r61c la
movio (`splits.txt` dice ya `.bss start:0x8045B110 end:0x8045B1E0`) y **nadie
volvio a leer el `.o` re-extraido**: ahi `gap_07_8045B1DC_bss` mide **4 B**, no
140.

    char gapTrasQt0[140]  ->  char gapTrasQt0[4]

| variante | `.bss` del `.o` | `linkdelta` | B distintos FUERA de mis rangos |
|---|---:|---|---:|
| `[140]` (r61) | 344 B | `bss+160` | 15.944 |
| `[4]` (hoy) | **208 B** | **IGUAL** | **15.331** |

208 B es exactamente el `.bss` del objeto EXTRAIDO. Los **-613 B de FUERA** son
lo que le dejabamos de mas a `zEAXSound` y siguientes cada vez que alguien
sustituia esta unidad para medir.

**La leccion, que es la trampa de la r61 al derecho**: cuando la ventana mueve
una raya, **el dueno del hueco tiene que releer el `.o` re-extraido**. El nombre
no cambio (`gap_07_8045B1DC_bss`, `keep.lst:189` sigue valiendo) y por eso nada
dio error: solo cambio el TAMANO. `linkdelta` lo canta; `fncmp`, `textorder` y
`checksplits` no.

Comprobacion cruzada que pedia el encargo: `aranges.py --list zEagl4Anim` da
**UNA sola tentativa de `.bss`**, `0x8045B1C0` de 28 B, que es `qt0` -- ya
emitida. **No queda nada de `.bss` que descubrir en esta unidad.**

---

## 2. La palanca de la ronda: el `inline` que manda tres funciones a la cola (-7.917 B)

`ltorder.py` (sonda mia, seccion 8) compara la secuencia de simbolos de `.text`
**del ELF ENLAZADO**, no la del `.o`. Dice algo que `textorder` no puede decir:

    objetivo 318 simbolos, nuestro 318 -- solo objetivo 0, solo nuestro 0

O sea: **el `.text` de zEagl4Anim es una PERMUTACION PURA**. No falta ni sobra
una funcion en el enlace (las 113 "solo nuestras" del `.o` las tira
`-strip-unused-data`). Todo el problema son bloques de orden.

El primero, y el mas caro: `EulF3Interp`, `QuatF4Interp` y `TranF3Interp`
(`DeltaChan.cpp:586-604`). El objetivo las tiene en la posicion **364 de 318**
-- la COLA de inlines diferidas de `finish_file` --, pegadas a
`EulF3/QuatF4/TranF3`, que ya eran `inline`. Nosotros las teniamos en la **130**,
en su punto de parseo.

Se les toma la direccion (`RawPoseChannel.cpp:70-76`), asi que GCC 2.9 las emite
igual; con `inline` las emite **desde `finish_file`**, que es donde el objetivo
las tiene.

| variante | B distintos | textorder |
|---|---:|---|
| base r61 | 93.279 | 273 descolocadas / 75 saltos |
| **`inline` en las tres** | **85.362** | 251 / 73 |

**-7.917 B con tres palabras.** Es `memory/nfsmw-decl-comdat-inline` al reves:
alli habia que retrasar una inline; aqui hay que **convertir en `inline` una que
no lo era** para que salga tarde.

---

## 3. Los cuerpos en clase que el objetivo tiene fuera (-1.676 B)

Mismo mecanismo, direccion contraria. `ltorder` daba cuatro bloques seguidos:

    delete  obj 3 fn/256 B  | _._Q29EAGL4Anim8FnDeltaQ
    delete  obj 1 fn/148 B  | _._Q29EAGL4Anim12FnDeltaQFast
    delete  obj 1 fn/100 B  | GetLength__CQ29EAGL4Anim12FnDeltaQFastRf
    delete  obj 3 fn/320 B  | _._Q29EAGL4Anim14FnDeltaSingleQ

`delete obj` = el objetivo las tiene AHI y nosotros no (las teniamos en la cola).
Las cuatro son cuerpos escritos DENTRO de la clase, o sea `inline`
(`memory/nfsmw-en-clase-es-inline`), y por eso salian de `finish_file`. Sacados
al `.cpp`, en el hueco exacto que marca la secuencia del enlace:

| paso | funciones | B distintos |
|---|---|---:|
| base (con la seccion 2) | | 85.362 |
| `FnDeltaQ`: `~FnDeltaQ`, `SetAnimMemoryMap`, `GetLength` delante de `Eval` | 3 / 256 B | **84.814** (-548) |
| `FnDeltaQFast`: `~FnDeltaQFast` delante de `SetAnimMemoryMap`; `GetLength` delante de `Eval` | 2 / 248 B | |
| `FnDeltaSingleQ`: las tres delante de `Eval` | 3 / 320 B | **83.686** (-1.128) |

**-1.676 B.** El sitio no se adivina: sale de la secuencia de simbolos del
enlace, y las tres cabeceras se comprobaron privadas antes de tocarlas.

---

## 4. NEGATIVO CON CIFRA: permutar DENTRO de un bloque descolocado NO PAGA

Esto es lo mas util del informe para el que venga, porque ahorra rondas enteras.

**Ensayo A -- `Transform` (`eagl4runtimetransform.cpp`).** El objetivo tiene
`PostMult, ExtractQuatTrans, BuildSQT, MultMatrix`; nosotros teniamos las dos
primeras al reves. Intercambiadas: `textorder` baja de 73 a 70 saltos, `ltorder`
pierde el bloque... y **`rangos` SUBE 74 B (83.695 -> 83.769)**.

**Ensayo B -- `FnRunBlender.cpp`, las 20 funciones reordenadas al orden del
objetivo** (leido del ELF enlazado):

    FnRunBlender, ~FnRunBlender, EvalSQT, Eval, SetWeight, CycleTime,
    ComputeCycleIdx, EvalPhase, EvalVel2D, FindMatchTime, ComputeAlignQ,
    AlignCycleBeginEnd, AlignRootQ, AlignVel, BlendVel, BlendFacing,
    GetFrequency, ComputeRootQ, ComputeBeginRootQ, ComputeEndRootQ

Las 20 casan una a una despues. **`rangos` sube 9 B (83.686 -> 83.695).**

**La regla que sale de los dos, medida cuatro veces esta ronda:**

> Una permutacion **dentro** de un tramo que sigue en la direccion equivocada es
> **byte-neutra**: no cambia el tamano del tramo, asi que no mueve nada de lo que
> va detras, y lo unico que varia son coincidencias de byte. Lo que **paga** es
> mover una funcion de un tramo a **OTRO** (el `inline` de la seccion 2: -7.917 B; sacar
> los cuerpos de clase de la seccion 3: -1.676 B), porque eso cambia el TAMANO de los dos
> tramos y **realinea todo lo que va detras**.

Los dos ensayos se dejan APLICADOS a proposito, con su cifra escrita en el
fuente: son correctos contra el `.o` extraido, y +83 B de coincidencia no valen
volver a descolocar 22 funciones. Pero **que nadie gaste una ronda permutando
bloques sueltos esperando bytes.**

---

## 5. NEGATIVO MEDIDO: la receta que `r60b-auditoria-vedas.md` da para `EvalState` no sirve

`r60b` (linea 199) decia: *"CADUCA por combinacion... lo que falta es el pin
sobre una local que todavia no existe... la receta ya casa al 100 % en la
hermana `FindTime`... aplicarlo en :215 y :235... 456 B clavados"*.

Aplicada literalmente, y por separado en cada uno de los dos sitios:

    k = c->GetKeyData(0);
    register int keyOffset asm("r9") = i * c->GetKeySize();
    k += keyOffset;

| variante | filas | B | fuzzy |
|---|---:|---:|---:|
| base (la fuente de hoy) | **18** | **456** | **98,070 %** |
| los DOS sitios | 32 | 448 | 91,272 % |
| solo el `while` (:215) | 21 | 452 | 93,816 % |
| solo el `for` (:235) | 23 | 456 | 95,526 % |

Las tres PEOR, y ninguna toca el observable que pedia el informe: las filas 39 y
78 siguen siendo `add r4,r30,r11` en vez de `mr r4,r11`, y la 19 sigue con
`mr r12,r0`. **`EvalState` no es de familia C. Vuelve a FIRME.**

Y de paso, el **porque** del recalculo, que no estaba escrito y acota el frente:
nuestra fila 34 (`add r8,r30,r11`) **SI iza** la base `mDecodeData+2nf+10` para
la rama PAR de `GetKeyData` (fila 37: `addi r4,r8,0x2`), pero la rama IMPAR la
vuelve a calcular (fila 39: `add r4,r30,r11`), asi que el pseudo del
**desplazamiento** (`2*nf+10`) sigue vivo en todo el bucle. El objetivo tiene
**UN solo pseudo** (r11 = base+2nf+10) y saca la rama par con `addi r4,r11,2`.
Quien vuelva: el objetivo es **matar el pseudo del desplazamiento**, no pinchar
el indice. Escrito en `RawStateChan.cpp`, donde `previo.py` lo encuentra.

---

## 6. Lo que queda, ordenado por lo que puede valer

`fncmp` sigue en **2 de 318 (2.808 B)** y las dos siguen donde estaban:

* **`Initialize` (2.352 B, es el `.text +4` entero)** -- **NO TOCADA**, como
  pedia el encargo. El bloque de `eagl4supportdlopen.cpp:472-580` la cierra con
  `stmt.c` en la mano y con 161 combinaciones barridas.
* **`EvalState` (456 B)** -- 5. Sigue firme, ahora con una veda mas.

Y **27 bloques de permutacion**, que es TODO lo demas (80.895 B de `.text`).
Los cinco mas gordos, con su forma:

| bloque | B | forma |
|---|---:|---|
| `Eval__FnCycle` y 6 mas | 1.620 | permutacion entre `FnCycle.cpp` y la cola |
| `EulF3/QuatF4/TranF3(+Interp)` | 1.956 | el grupo de la seccion 2: ya en la cola, pero en el PUNTO de la cola equivocado |
| `EvalVel2D__FnRunBlender` y 2 | 1.652 | reparto entre `FnRunBlender.cpp` y `FnTurnBlender.cpp` |
| `ComputeCycleIdx__FnRunBlender` y 3 mas | 1.272 | idem |
| `EvalPhase__FnTurnBlender` y 2 | 776 | `FnTurnBlender.h`: cuerpos en clase (misma receta que el 3) |

**El unico bloque que NO puedo tocar yo**: `GetAttributes__CQ29EAGL4Anim6FnAnim`
(8 B). Su cuerpo esta EN CLASE en `FnAnim.h:105`, y `FnAnim.h` la arrastra
`eagl4AnimBank.h` -> `Animation/AnimBank.cpp` -> **`zAnim`**, que esta
`Matching`. Sacarla de la clase la dejaria sin definir para `zAnim`. **Se
PROPONE**, no se aplica (regla 5).

---

## 7. Lo que pido, con la cifra

### 7.1 `splits.txt` -- la raya de `.data`, y esta vez la aritmetica CIERRA

`aranges.py --cotejo` dice hoy:

    zEagl4Anim.cpp   .data  splits 0x804170F4   aranges 0x804170D0   -36
    zEAXSound.cpp    .data  splits 0x80417634   aranges 0x804175E0   -84

**El movimiento del INICIO no se puede hacer**, y hay que decirlo: el cedente
seria `zDynamics`, que es **`Matching`** (`configure.py:648`). Ceder 36 B de una
`Matching` a una `NonMatching` deja el DOL **largo** salvo que se edite la fuente
de `zDynamics`. `r61c` 2/P3 ya lo excluia y hace bien.

**El movimiento del FINAL si es DOL-neutro** (`zEagl4Anim` y `zEAXSound` las dos
`NonMatching`, `configure.py:649-650`), y ademas **la aritmetica cierra al byte
contra lo que ya emitimos**:

```
Speed/Indep/SourceLists/zEagl4Anim.cpp:
	.data       start:0x804170F4 end:0x804175E0      # antes end:0x80417634
Speed/Indep/SourceLists/zEAXSound.cpp:
	.data       start:0x804175E0 end:0x80417DA8      # antes start:0x80417634
```

**La prueba, y es del mismo tipo que la del `.bss` de la seccion 1**: hoy
`zEagl4Anim.cpp:230-240` emite a mano un hueco llamado, literalmente,
**`gap_06_804175E0_data`, de `0x54` = 84 B** (`keep.lst:187`). Y
`0x80417634 - 0x804175E0 = 84`. **Ese hueco es la CABEZA de la `.data` de
`zEAXSound`, no nuestra cola** -- exactamente el caso de `zEAXSound2` que la r61c
cazo en `.bss`.

Si se aplica:

* de `zEagl4Anim.cpp` hay que **BORRAR** el bloque `asm()` de
  `gap_06_804175E0_data` y **la linea 187 de `keep.lst`**;
* en `zEAXSound` aparece un hueco de cabecera NUEVO. **Hay que LEER su nombre del
  `.o` re-extraido**, no pegarlo (trampa de la r61: se llaman `pad_NN_DIR_sec` o
  `gap_NN_DIR_sec` segun el caso, y `-strip-unused-data` se los lleva EN SILENCIO
  si `keep.lst` no los nombra);
* **verificacion**: `linkdelta zEagl4Anim` tiene que seguir dando `IGUAL` en el
  resto. Si sale `.data +44`, es trabajo descubierto de la misma familia que los
  136 B de la seccion 1, y es MIO.

Va **con** la cadena de `zEAXSound/zEAXSound2/zEcstasy`, nunca sola: `zEAXSound`
se quedaria 84 B larga.

### 7.2 Cabecera compartida -- `FnAnim.h`

Sacar `GetAttributes()` del cuerpo de la clase (`FnAnim.h:105-107`) a
`FnAnim.cpp`. Vale **8 B colocados y el bloque que arrastran**, pero `FnAnim.h`
llega a `zAnim` (`Matching`) por `eagl4AnimBank.h` -> `Animation/AnimBank.cpp`.
**No la he tocado.** Quien la mida, que use el sombreado con `-I` de
`r61-eagl.md` seccion 5 y compruebe `zAnim` antes.

### 7.3 `lcfix`

**1 correccion, mia, no venenosa** (`keep.lst:2811`, `$LC109 -> $LC124`, la
cadena `"?"`). Medido: `rangos` da **83.769 con y sin ella**. No bloquea a nadie.

---

## 8. Herramientas (todas en `scratchpad/eagl62/`, ninguna en `scripts/`)

Heredadas de `eagl61`: `bd.py` (compila SOLO la unidad y la sella con sha1),
`rangos.py` (la metrica), `lcpriv.py`, `control.py`, `reftext.py`, ...

**Nuevas de esta ronda, y las dos primeras valen para cualquier unidad:**

| script | que hace |
|---|---|
| **`ltorder.py <u>`** | El orden de los simbolos de `.text` **EN EL ENLACE**, no en el `.o`, con el diff por bloques (`difflib`) y las dos secuencias volcadas a fichero. Es lo que `textorder.py` no puede dar: `textorder` cuenta los 113 simbolos que emitimos de mas y que el enlace TIRA, asi que sus deltas llevan ruido. `ltorder` dijo en un segundo que zEagl4Anim es **permutacion pura, 318 contra 318**. |
| **`arrastre.py <u> <obj.o>`** | Bytes distintos contra el enlace BASE separados en **DENTRO** y **FUERA** de los rangos de la unidad. El "FUERA" es el veneno que le echas a los vecinos cada vez que alguien te sustituye para medir; es la cifra que demostro los -613 B de la seccion 1. |
| `bsscmp.py` | Simbolos de `.bss`/`.sbss`/COMMON del `.o` extraido contra el nuestro, con tamano. Un vistazo y sale el `140` contra `4`. |
| `doldif.py` | Bytes exactos del DOL. **Inutil aqui**: con `.text +4` la seccion `.over` se va 8 B y aborta con "SECCIONES DISTINTAS". Se deja documentado para que nadie lo reescriba. |

**TRAMPA NUEVA, y me costo media hora**: si sustituyes un objeto en el enlace por
una COPIA con **otro nombre de fichero**, `keep.lst` deja de resolver -- sus
entradas son `zEagl4Anim.o:$LCn`, indexadas por BASENAME. El enlace no protesta y
`-strip-unused-data` se lleva las cadenas muertas: en zEagl4Anim eso daba
`.rodata -728` y `.data -96` **fantasma**. Cualquier sonda que enlace con un `.o`
alternativo tiene que **copiarlo con su nombre original**.

Segunda trampa, mas tonta y mas cara: `subprocess` en Windows **no encuentra el
`.exe`** si le pasas la ruta relativa con **barras normales**. Los `scripts/*.py`
usan `os.path.join` por eso; si copias una sonda y cambias esa linea, sale un
`FileNotFoundError` que parece que falta el compilador.

---

## 9. Sorpresas

1. **Una unidad puede tener 160 B de `.bss` de mas y el arreglo estar ya escrito
   en su propio fuente.** El comentario de la r60 decia la direccion buena de la
   raya; la r61c la movio; nadie releyo el `.o`. **Mover una raya crea trabajo
   para el DUENO del hueco, y nada se lo avisa: solo `linkdelta`.**
2. **`textorder` y la metrica de bytes pueden ir en direcciones contrarias.** 73
   -> 70 saltos y +74 B a la vez. La causa esta medida (4).
3. **El `.text` de zEagl4Anim es permutacion PURA**: 318 simbolos contra 318, ni
   uno de mas ni de menos en el enlace, con 80.895 B distintos. Merece la pena
   pasar `ltorder` a las otras unidades de colocacion antes de escribir nada.
4. **`inline` puede ser lo que HAY que anadir**, no lo que hay que quitar. Tres
   `inline` valieron 7.917 B porque mandan la funcion a la cola de `finish_file`,
   que es donde el objetivo la tiene. Con la direccion tomada, GCC 2.9 la emite
   igual.
5. **La receta de una funcion hermana no se hereda.** `FindTime` casa al 100 %
   con el pin `asm("r9")`; `EvalState`, en el mismo fichero y con la misma
   llamada, empeora con las tres formas de aplicarlo.
6. **`keep.lst` se indexa por BASENAME del objeto.** Renombrar un `.o` para medir
   dos versiones lo desactiva entero, sin un solo aviso.
