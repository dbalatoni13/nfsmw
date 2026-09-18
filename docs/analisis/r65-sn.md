# r65 -- lote `sn`: retirada de andamios en LibSN

`src/LibSN/metrotrk.c`, `src/LibSN/sndvd.c`, `src/LibSN/FSasync.c`.

**Veredicto: PARCIAL.** 6 andamios retirados de 27 intentados, con los tres
objetos byte a byte identicos. Los 21 que quedan salen todos diagnosticados, y
12 de ellos estan en un fichero que **no se compila**.

---

## 1. El censo

Contado sin comentarios y con la definicion del encargo (pin =
`register T x asm("rN")`, barrera = `asm("")` de plantilla vacia; los
`__asm__("nombre")` de enlace de nombre NO cuentan):

| fichero | pines antes | barreras antes | total antes | total despues |
|---|---|---|---|---|
| `metrotrk.c` | 14 | 1 | 15 | 13 |
| `sndvd.c`    | 3  | 2 | 5  | 3  |
| `FSasync.c`  | 4  | 3 | 7  | 5  |
| **suma**     | **21** | **6** | **27** | **21** |

El encargo decia 24 porque el censo cuenta `__asm__("")` y estos tres ficheros
tienen ademas 3 barreras escritas `asm("")` sin los subrayados (metrotrk L126,
FSasync L405 y L408). Con la cuenta del encargo: **24 -> 18**.

## 2. El sello: digests ALLOC antes y despues

Secciones ALLOC (`.text`, `.rodata`, `.data`, `.bss`, `.rela.text`), ignorando
`.line`/`.debug*`/`.comment`/`.stab*`. **Las tres unidades salen identicas.**

| objeto | seccion | antes | despues |
|---|---|---|---|
| `build/GOWE69/src/FSasync.o` | `.text` 2104 B | `34e92584327a` | `34e92584327a` |
| | `.rela.text` 2988 B | `baa35212f76f` | `baa35212f76f` |
| | `.rodata`/`.data`/`.bss` 0 B | `da39a3ee5e6b` | `da39a3ee5e6b` |
| `build/GOWE69/src/LibSN/sndvd.o` | `.text` 1584 B | `024ca07c00e8` | `024ca07c00e8` |
| | `.rodata` 24 B | `b4d9bd5db77c` | `b4d9bd5db77c` |
| | `.rela.text` 1740 B | `ec0d674db9e5` | `ec0d674db9e5` |
| `build/GOWE69/src/LibSN/metrotrk.o` | `.text` 12648 B | `742c131fd67e` | `742c131fd67e` |
| | `.rela.text` 5328 B | `e979d4b639c1` | `e979d4b639c1` |

(sha1 truncado a 12; el baseline se comprobo recompilando **sin tocar nada**
antes de empezar, y reprodujo el `.o` que estaba en el arbol: el compilador es
determinista y el objeto committeado corresponde a la fuente.)

`metrotrk.o` no cambia por construccion: se ensambla del `.s`. Ver 5.

## 3. Las seis retiradas

### 3.1 `sndvd.c` -- `DisDvdBP` y `EnaDvdBP` (2 pines de r3)

Estaba:

```c
static void DisDvdBP(void) {
    volatile u32 dabr = 0;
    register u32 v __asm__("r3");
    v = dabr;
    __asm__ __volatile__ ("mtspr 1013, %0\n\tisync" : : "r" (v));
}
```

El objetivo es `li r0,0; stw r0,8(r1); lwz r3,8(r1); mtspr 1013,r3`. Quitando
el pin a secas sale **lo mismo pero con r9**, y ahi esta la pista: en
`REG_ALLOC_ORDER` de rs6000 los GPR van `9, 10, 8, 7, 6, 5, 4, 3, 31, 30, ...`,
o sea que **r9 es la primera eleccion y r3 solo gana si una COPIA ata el pseudo
a r3**. En una hoja sin llamadas la unica copia posible a r3 es el valor de
retorno. Forma que lo sustituye:

```c
static u32 DisDvdBP(void) {
    volatile u32 dabr = 0;
    u32 v = dabr;
    __asm__ __volatile__ ("mtspr 1013, %0\n\tisync" : : "r" (v));
    return v;
}
```

El helper devuelve el valor que acaba de escribir en el DABR, los llamantes lo
descartan, y el `blr` no necesita ni una instruccion mas porque `v` ya esta en
r3. `.text` 1584 B `024ca07c00e8` antes y despues.

### 3.2 `FSasync.c` -- `CompletePCreadAsync` (2 pines de r0)

`register int condition asm("r0")` x2 -> `int condition;`. El valor solo
alimenta un `cmpwi`/salto y GCC elige r0 el solo. `.text` 2104 B
`34e92584327a` antes y despues. Eran gratis.

### 3.3 `metrotrk.c` -- `fn_80310F94` y `fn_80311A28` (2 pines)

- `fn_80310F94`: `register SNInterruptHandler handler asm("r4")` -> `SNInterruptHandler handler;`.
  Es el segundo argumento de `__OSSetInterruptHandler`, asi que r4 ya le toca.
- `fn_80311A28`: `register unsigned int value asm("r3") = flags` -> `unsigned int value = flags;`.
  Es a la vez el argumento y el valor de retorno: r3 por los dos lados.

Medido contra los BYTES DEL ORIGINAL (ver 5): 7 de 8 funciones identicas antes
y 7 de 8 despues, con exactamente la misma unica diferencia residual.

## 4. Los irreducibles, con diagnostico

Todos anotados **junto al andamio** para que `previo.py` los encuentre.

### 4.1 `sndvd.c` -- pin de r29 en `DSIHandler`: **PERMUTACION de prioridad**

Sin el pin la funcion sale del mismo tamano (628 B) y con las mismas
instrucciones: solo se intercambian `savedContext` (r29 en el objetivo) y
`offset` (r28). Los cuatro preservados son `ea` r31, `insn` r30 y esos dos, y
el orden lo fija `allocno_compare` en `orig/prodg/NGC_GNU_SRC/NGC/gcc/global.c:625`:

```
prioridad = floor_log2(n_refs) * n_refs / live_length * 10000 * size
```

`offset` tiene 3 referencias en un rango corto, `savedContext` 7 en todo el
cuerpo. Con `floor_log2` (3 -> 1, 7 -> 2) gana `offset` y se lleva r29.
Invertirlo pide cambiar el numero de referencias o el rango de vida, o sea
cambiar lo que hace la funcion. Probado ademas `OSContext *savedContext = context;`
sin pin: el coalescing lo funde y da el mismo objeto que no poner nada.

### 4.2 `sndvd.c` -- `__asm__("")` del cross-jumping: **sigue vivo**

Remedido: quitandolo, `.text` 1584 -> **1580 B**. Los dos rabos
`bl ForceDvdDeIrq` + `b done` (el del `default` y el del `case 0xA800`) son
identicos instruccion a instruccion y `jump_optimize` los funde por
`jump_chain` (`jump.c:1991`). El objetivo NO los funde, luego en el original
los dos rabos no eran identicos; no he encontrado que los diferencia sin
cambiar la semantica.

### 4.3 `sndvd.c` -- `__asm__("" : : : "memory")`: **adelanto de sched2**

Sin la barrera el objeto sale del mismo tamano y con las mismas instrucciones:
lo unico que pasa es que el `mr r3,r29` --el montaje del argumento de
`OSLoadContext`, que no depende de nada-- sube una ranura y rellena la burbuja
entre `lwz r9,0x198(r29)` y `addi r9,r9,4`. Tres formas barridas, las tres dan
el mismo objeto que no poner nada:

1. sin barrera;
2. `srr0` declarado `volatile` en el `OSContext` -- el MEM volatil ata memoria
   contra memoria, y el `mr` es registro contra registro;
3. `savedContext->srr0 = savedContext->srr0 + 4;` en vez del `+=`.

Es el caso del PLAYBOOK en que la barrera selectiva **es** la palanca.

### 4.4 `FSasync.c` -- los 2 pines de r30: **deuda del andamio, no de la fuente**

Y aqui hay una **correccion importante**: la cabecera de la veda r36e estaba
rancia. Decia «396 B contra 388 al 93,70%»; hoy `CompletePCreadAsync` esta a
**388 B y al 100%**, y lo que la cerro son los tres `FS_BLOCK_*`.

Remedido escribiendo las dos regiones enteras en C llano (sin los `FS_BLOCK_*`
y sin el pin):

| | `.text` | digest | `CompletePCreadAsync` |
|---|---|---|---|
| con andamio | 2104 B | `34e92584327a` | 388 B, marco 0x28, `stmw r24` |
| C llano | 2112 B | `5e1233484dfc` | 396 B, marco 0x30, `stmw r23` |

Los 8 B son los dos `lis` de mas: el objetivo comparte **un solo** pseudo de
base `@ha` entre el predecremento y el bucle, y en C llano GCC emite uno para
el predecremento (volatil, r9) y otro que `loop.c` saca al precabezal
(preservado). Ese preservado de mas sube el marco.

El pin en si es **PERMUTACION r30<->r31**: sin el, `block_high` se lleva r31 y
la base de `g_pBuffer_804B4F44` r30, al reves que el objetivo, sin cambiar ni
una instruccion. Y la causa es otra vez `allocno_compare`: `block_high` tiene 7
referencias (una del `FS_BLOCK_HIGH` y seis de los LOAD/STORE) contra 5 de la
base de `g_pBuffer`. **Esas 7 referencias las pone el propio andamio.** O sea
que el pin no es deuda de fuente: se va el dia que se vaya el `FS_BLOCK_*`.

**La unica via viva para el `FS_BLOCK_*`, y no es de fuente:** los globales de
esta unidad son consecutivos de 4 en 4 --`g_hHandle_804B4F40`,
`g_pBuffer_804B4F44`, `g_nBlockCnt_804B4F48`, `g_nRemainderCnt_804B4F4C`,
`g_nTotalBytesRemaining_804B4F50`, `g_bDoFSACK_804B4F54`,
`g_nFSLastError_804B4F58`--, o sea que en el original eran casi seguro **una
struct**. Con una struct, `st.blockCnt` da `lis rN,base@ha` +
`lwz r0,base@l+8(rN)`, que es exactamente la forma del objetivo y comparte la
base entre predecremento y bucle. No se puede probar sin tocar
`config/GOWE69/splits.txt`: la reubicacion pasaria a ser `ADDR16_LO` sobre el
simbolo base con sumando 8, y la del objetivo es sobre `g_nBlockCnt_804B4F48`
con sumando 0 -- `.rela.text` cambiaria y el test falla. **Es trabajo de
splits, no de fuente.**

### 4.5 `FSasync.c` -- barrera del `16 << 8` en `DoFSReadHeader`: **sigue viva**

Quitandola, `.text` 2104 -> **2100 B** y `DoFSReadHeader` de 140 a 136: el
`li r0,16` + `slwi r0,r0,8` colapsa en `li r0,0x1000`. El objetivo tiene los
dos separados, luego el 16 no era una constante visible en el punto del
desplazamiento. Las dos familias de forma que lo consiguen en C llano cuestan
instrucciones (`volatile` local mete `stw`+`lhz`; una tabla en `.rodata` mueve
la seccion). Van doce formas.

### 4.6 `FSasync.c` -- las 2 barreras de `EXI2TCHandler`: **siguen vivas**

Remedidas por separado, las dos dan el mismo tamano (2104 B) y un `.text`
distinto -- reordenacion pura:

- quitando `asm("" : "+m"(g_nFSLastError_804B4F58) : "r"(cb));` -> `fb3aa7ef236c`
- quitando `asm("" : "+r"(cb));` -> `53440925bdb3`

Adelantos de sched2 sobre globales `volatile`. Son el caso legitimo de barrera
selectiva y ya llevaban 35 formas barridas en la r36f.

---

## 5. `metrotrk.c`: 12 de mis 21 pines estan en un fichero que no se compila

**Este es el hallazgo del lote.**

`configure.py:1399` dice
`Object(Matching, "LibSN/metrotrk.c", source="LibSN/metrotrk.s")`, y el unico
arco de ninja que produce `build/GOWE69/src/LibSN/metrotrk.o` es la regla `as`
sobre el `.s` (`build.ninja:1724`). `objdiff.json` tambien apunta ya al `.s`
(`source_path: src/LibSN/metrotrk.s`). **Nada lee el `.c`.** Es el fichero
anterior, y la cabecera del propio `.s` lo declara sustituido:

> «Sustituye al metrotrk.c anterior, cuyos 148 B cerrados descansaban sobre
> asm() en linea.»

Y el `.s` trae ademas medido que **metrotrk nunca fue C**: 9.916 de sus 11.988 B
leen no volatiles sin salvarlos o usan `rfi`/`mtmsr`/`mtspr`, y siete
compiladores (seis mwcc y ProDG 3.9.3) emiten `@ha`/`addi` donde el original
tiene `@h`/`ori`. La pregunta «que forma de fuente hace innecesario el pin» no
tiene respuesta porque no hay fuente.

### El oraculo que si existe aqui

Como el `.o` sale del `.s`, **sus bytes son los del original**. Compilando el
`.c` con los cflags de libsn y comparando funcion a funcion contra ese `.o` se
mide de verdad. Baseline: **7 de 8 identicas** (`fn_80310F94` difiere solo en
el orden del prologo/epilogo, ya anotado como «Partial»).

Bisecado pin a pin, 14 compilaciones:

| pin | funcion | sin el pin |
|---|---|---|
| `state` r3, `state` r3, `connected` r4, `header` r6 x3 | 6 sitios | GCC da **siempre r9** (cabeza de `REG_ALLOC_ORDER`) |
| `booted` r4, `disconnected` r5, `current` r4 x2 | 4 sitios | GCC da **siempre r0** |
| `value` r3 (`fn_80311A10`) | 1 sitio | la funcion crece de 24 a **28 B** (un `mr` de mas) |
| `handler` r4 (`fn_80310F94`) | 1 sitio | **IDENTICA -- retirado** |
| `value` r3 (`fn_80311A28`) | 1 sitio | **IDENTICA -- retirado** |

No es una permutacion ni un reparto: es que el original **nombra los registros
a mano** y C no sabe nombrarlos. Los 12 que quedan tienen una sola causa, no
doce. La barrera `asm("" : "+r"(value) : "r"(current))` de `fn_80311A28`
tambien se remidio y tambien sigue viva (la funcion crece de 28 a 32 B).

### PROPUESTA (no hecha)

**Borrar `src/LibSN/metrotrk.c`.** Retira los 12 pines y la barrera de golpe,
no rompe ningun arco de build (comprobado: ninja, objdiff.json y los `.lst` no
lo leen), y deja en el arbol solo el camino sancionado. Queda propuesto y no
hecho porque tocar ficheros fuera de lo que pedia el encargo no me corresponde;
es un `git rm src/LibSN/metrotrk.c` y una recompilacion de la unidad para
confirmar que el `.o` no se mueve (no puede: se ensambla del `.s`).

Mientras tanto el fichero queda **anotado en cabecera** con todo esto, para que
nadie vuelva a intentarlo.

---

## 6. Regresiones y revertidos

- **Regresiones: ninguna.** Los tres objetos salen con los digests ALLOC
  identicos a los de partida, y el baseline se verifico recompilando sin
  cambios antes de empezar.
- **Revertido en el acto, 11 intentos** que movian `.text`: el pin de r29 de
  `DSIHandler` (dos formas), las dos barreras de `sndvd`, `srr0` volatil, el
  `+=` desplegado, los 2 pines de r30 de `FSasync`, el bloque entero en C
  llano, la barrera del `16<<8` y las dos de `EXI2TCHandler`. Mas 12 de los 14
  pines de `metrotrk` (que no mueven ningun `.o`, pero si alejan la fuente de
  los bytes del original).
- No se ha tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, ni se ha
  corrido `lcfix.py`, ni se ha commiteado nada.

## 7. Sorpresas

1. **`sndvd.c` ya estaba hecho en la r64b, pero solo una linea.** Lo que el
   encargo llamaba «repite el metodo» resulto ser un cambio de una barrera por
   otra mas debil, no una retirada.
2. **`REG_ALLOC_ORDER` explica casi todo el lote.** En rs6000 los GPR van
   `9, 10, 8, 7, 6, 5, 4, 3, 31, 30, ...`. Todo pin a r3/r4/r6 que no venga de
   un argumento o de un retorno esta peleando contra eso, y la unica palanca
   legitima es **atar el pseudo a r3 con una copia**, que es lo que hizo
   `DisDvdBP`. Vale la pena mirarlo en los otros lotes.
3. **`allocno_compare` es la respuesta a los «permutados» de dos registros
   preservados**, y esta en el arbol. Dos de los tres irreducibles de reparto
   de este lote son eso, y ninguno es arreglable sin cambiar la funcion.
4. **Una veda con cifras rancias hace perder el tiempo.** La de r36e decia
   93,70% de una funcion que hoy casa al 100%. Remedir antes de creer.
