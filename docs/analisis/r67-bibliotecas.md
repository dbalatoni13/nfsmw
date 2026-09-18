# r67 -- lote `bibliotecas`: tercera pasada de andamios

Encargo: dejar fuente legitima. Test unico: sello ALLOC por seccion (`.text`, `.rodata`,
`.data`, `.sdata*`, `.ctors`, `.bss` por tamano y sus `.rela`, sin `.debug*`/`.line`/
`.comment`) MAS la tabla de simbolos restringida a las ALLOC (la parte SYM de la r65).
Identico -> se acepta; distinto -> se revierte en el acto.

**Veredicto: PARCIAL. 5 andamios retirados de 31 (encargo) -- 38 -> 33 contando la grafia
corta `asm("")` --, 0 regresiones, las 15 unidades del lote recompiladas desde el arbol final
con el sello ALLOC/SYM IDENTICO al de partida. Los 26 que quedan llevan diagnostico r67
escrito junto al andamio.**

---

## 0. Resumen

| fichero | encargo antes | despues | real antes | real despues | que paso |
|---|---|---|---|---|---|
| `realcore/.../cmn/exit.cpp` | 1 pin | **0** | 1 | **0** | RETIRADO (forma de una sola local) |
| `realcore/.../gc/inittmr.cpp` | 1 barrera | **0** | 1 | **0** | RETIRADO (guarda como retorno temprano) |
| `snd/.../cmn/sst.c` | 2 pines | **0** | 2 | **0** | RETIRADOS (el `else` escribe por `pint`) |
| `snd/.../cmn/srender.c` | 1 barrera | **0** | 1 | **0** | RETIRADO (`validrendermode = 0; continue;`) |
| `realcore/.../file/cmn/filesys.cpp` | 5 barreras | 5 | 5 | 5 | palanca (a) medida: NEUTRA |
| `spch/.../cmn/spchpick.c` | 3 pines + 2 barreras | 5 | 5 | 5 | 11 formas nuevas, negativas |
| `csis/.../cmn/csis.cpp` | 3 pines | 3 | 5 | 5 | cabecera compartida: PROPUESTO |
| `snd/.../gc/snddrv.c` | 2 pines | 2 | 2 | 2 | orden de carga de argumentos |
| `snd/.../mix/sfsplit.c` | 1 pin | 1 | 2 | 2 | idem, mecanismo medido con RTL |
| `snd/.../mix/sfir.c` | 1 barrera | 1 | 2 | 2 | 6 formas, negativas |
| `path/.../pathnode.cpp` | 2 pines (FPR) | 2 | 2 | 2 | 51 formas (24 permutaciones x 2) |
| `path/.../pathbank.cpp` | 1 barrera | 1 | 1 | 1 | mecanismo de alias identificado |
| `vp6/.../criticalpath.c` | 3 pines | 3 | 4 | 4 | no reabierto (asm VP6 original intacto) |
| `realmemcard/.../gc_driver.cpp` | 2 pines | 2 | 4 | 4 | permutacion de dos parametros |
| `egami/.../avplayer.cpp` | 1 barrera | 1 | 1 | 1 | 5 formas, el mismo objeto |
| **total** | **31** | **26** | **38** | **33** | |

Censo sin comentarios (`scratchpad/bibliotecas67/censo67.py`, que reusa `descomenta` de
`censo65.py` y cuenta tambien los pines de FPR `"frN"`; HEAD extraido con `git show`). El
encargo anunciaba 33 para este lote, pero su propia lista por fichero suma 31.

---

## 1. Metodo y controles (`scratchpad/bibliotecas67/`)

* `cc.py`, `formsweep.py`, `sweep.py`, `apply_tree.py`: los de la r66, con salida a mi
  scratchpad. Compilan a `.o` PRIVADOS; nunca escriben en `build/GOWE69`.
* `check.py`: compila desde el arbol y compara con el sello BASE medido al empezar, escrito
  como CONSTANTE en el script (no depende de `build/GOWE69`, que otro agente podria reescribir).
* Al empezar, **15/15 unidades IGUAL-AL-ARBOL** (`baseline.py`): nada rancio. Los sellos
  coinciden con los que publico la r66.
* `dis.py`/`rows.py`: desensamblado lado a lado contra `build/GOWE69/obj` y filas distintas.
  OJO: `rows.py` no distingue destinos de salto con el mismo mnemonico y cuenta como fila la
  diferencia de addend de reubicacion (`lbz r0,0(r9)` contra `lbz r0,63(r9)`): sus filas son
  orientativas. **La verdad es el sello.**
* `rtldump67.py`: copia de `scripts/rtldump.py` con salida a mi scratchpad.
* Controles que pueden fallar: en cada barrido va la retirada a secas y cada una da un sello
  distinto de la base (p.ej. exit `1c5bdb67`, inittmr `9e69f088`, sst `05993f56`, srender
  `44557868`). El CONTROL (copia sin tocar) da la base en todos los barridos.

---

## 2. Los cinco retirados

Base = sello de partida; en los cinco el sello final es el mismo que la base.

### 2.1 `exit.cpp` SYSTEM_addexit -- pin `j asm("r11")`

DWARF del original: solo `int j; // r11`. La r65 midio tres formas "de una sola local" y
declaro el pin como unica palanca. Faltaba una cosa: **la primera prueba escrita con el
indice** (`j = 0; if (exitfunctions[j] == nullptr)`) y no con el literal `[0]`. cprop la sigue
plegando a `lwz 0(r9)`, pero `j` gana referencias antes del reparto y global_alloc la coloca
antes que el indice escalado.

| forma | sello ALLOC |
|---|---|
| base (pin) | `ac776748fa2f35a0` |
| **una local `j` en los dos recorridos, primera prueba con `[j]`** | **`ac776748fa2f35a0`** |
| la misma con `pf`/`i` o con `while` en el primer bucle | `ac776748fa2f35a0` |
| la misma con el literal `[0]` | `a2e630ff` |
| quitar solo el pin de la forma r65 | `1c5bdb67` |
| segundo recorrido como `for` / `while` / `for (;;)` | `0b829f09` / `4e1e4f9a` / `5ee83791` |

Aplicada la forma sin `pf` ni `i`: es la que describe el DWARF.

### 2.2 `inittmr.cpp` TIMER_init -- barrera `"+r"(done)` y local `done`

DWARF: solo now/Period/tmp. Lo que la sustituye es la **guarda como retorno temprano**
(`if (bIsTimerInited) return TIMERhz;`) en vez del `if (!bIsTimerInited) { ... }` que envolvia
el cuerpo, con `bIsTimerInited = 1;` en su sitio. El bloque de la bandera no cambia de sitio;
cambia la numeracion de insn y con ella el desempate de `rank_for_schedule`.

| forma | sello ALLOC |
|---|---|
| **retorno temprano, `= 1`** | **`dc239e2f1b378fdd`** (base) |
| `if` envolvente con `= 1`, `= true` o `&TIMER_restore` | `9e69f088` |

### 2.3 `sst.c` SNDSTRMI_parsedata -- pines `pint asm("r10")` e `intTemp1 asm("r10")`

DWARF: `unsigned int *pint; // r10`, `int *intTemp;` e `int *intTemp1;` **sin registro**. Una
local sin ubicacion es una local que el compilador no llego a usar: el original escribe la
cabecera del paquete en el `else` **a traves de `pint`**, que ya vive en r10.

| forma | sello ALLOC |
|---|---|
| **`pint = (unsigned int *)packet.psamples[0]; pint[-1] = ...;`, sin pines** | **`313d112fb460ed84`** (base) |
| la misma conservando el pin de `pint` | base |
| `intTemp` en el else, o `intTemp1` en las dos lineas (pin de pint puesto) | `07c56a44` (2 filas, r9) |
| escritura directa por cast | `07c56a44` |
| quitar los dos pines con `intTemp1` en el else | `05993f56` (16 filas) |
| cambiar las expresiones de `pint`/`psampledata` | `05993f56` / `af34c980` |

`intTemp1` queda declarada y sin uso, como la lista el DWARF.

### 2.4 `srender.c` SNDI_validrendermode -- barrera `__asm__("")`

La DEUDA DECLARADA de la r26. Sin ella la recarga de `*prenderindex` se izaba por encima del
`and.` y el bloque del `continue` quedaba vacio. La sustituye **la misma sentencia que ya lleva
el segundo `continue` de la funcion**: `validrendermode = 0; continue;`.

| forma | sello ALLOC |
|---|---|
| **`validrendermode = 0; continue;`** | **`b7d3ad64326a69c5`** (base) |
| sin barrera, `!validrendermode`, asignacion en la condicion | `44557868` (29 filas, 184 B) |
| releer `rendermode[*prenderindex]` | `c9d8ec11` (36 filas) |

---

## 3. Irreducibles, con su diagnostico (escrito junto a cada andamio con marca r67)

### 3.1 `filesys.cpp` AddToQueue (5 barreras) -- la palanca (a) del encargo, MEDIDA Y NEGATIVA

El bloque anonimo con `curpriority` que describe el DWARF es **neutro**: con las cinco
barreras da el objeto identico (`if_blk`, `and_blk`, `if_blk_nz`, las tres a la base) y sobre
la forma natural da exactamente el mismo objeto que sin el (`f1a1744d`). Quitada la barrera del
incremento o la del `if` dentro del bloque: `6a34393e` / `0c41f8cf`.
Formas de la primera parte (natural, sin `newid` ni `newpriority`):

| forma | sello |
|---|---|
| natural / con casts / Head() detras / `prev = 0` detras del Head() | `f1a1744d` |
| operandos del `|` al reves | `0ae531bf` |
| dos sentencias `= prio << 24; |= id & m;` y al reves | `6b48d223` / `8b0bfa10` |

El orden de sentencias y el ambito de la prioridad no llegan al empate de sched1 del `slwi`.
No aplicado el bloque (no retira nada; queda documentado como reconstruccion neutra).

### 3.2 `pathbank.cpp` loadbankdata (1 barrera) -- mecanismo de ALIAS identificado

Sin la barrera el almacen de 0xBEDFACED cae detras de las lecturas de la vtable de realimp
(`lwz r9,16(r11)`, `lha r3,56(r9)`), 12 filas. Es la exencion de
`fixed_scalar_and_varying_struct_p` (alias.c): `fileop` es escalar en direccion fija (marco) y
la vtable es campo de struct en direccion variable. **`int fileop[1]` cierra porque el array
pone `MEM_IN_STRUCT_P` en el almacen** (lo que explica el sello identico de la r66). Formas r67,
todas `32a325f9` (= quitar la barrera): declaracion arriba, asignacion aparte, `int fileop(X)`,
vista agregada por cast de `realimp`, `Path::` cualificado. En expr.c solo ponen
`MEM_IN_STRUCT_P` ARRAY_REF, COMPONENT_REF o un INDIRECT_REF con suma, y el frontend pliega
`*&fileop`: la palanca (d) no alcanza a una local escalar.

### 3.3 `pathnode.cpp` calcwaitbeat (2 pines de FPR) -- palanca (b) agotada

Sin la local `notes` (el DWARF no la tiene) y sin pines, las 24 permutaciones de las cuatro
sentencias independientes dan **dos** objetos, siempre 336 B: 17 filas si `fevery` se asigna
antes que `foffset`, 24 si despues. Con `float notes` sin pin, los mismos dos. Solo sin el pin
de fevery 14 filas (`ac1d57c2`), solo sin el de notes 15 (`8e7e3a9e`), sin los dos 17
(`d1362f77`).

### 3.4 `sfsplit.c` (1 pin + 1 barrera) y `snddrv.c` (2 pines) -- orden de carga de argumentos

Medido con RTL (`.sched`/`.sched2` de la forma natural contra la andamiada): los tres `mr` de
la llamada salen en el orden de `load_register_parameters`, r3 -> r4 -> r5, y sched1 no los
reordena. En rs6000 los argumentos se expanden del primero al ultimo: `PUSH_ARGS_REVERSED`
(calls.c:66) exige `PUSH_ROUNDING`, que rs6000 no define. El objetivo carga r3 EL ULTIMO.
En snddrv solo pasa en la 3a y 4a copia, donde el `lis` de snddrv ya esta hecho y las dos
cargas quedan libres a la vez; en las dos primeras la dependencia del `lis` fija el orden.

* sfsplit: natural, con cast del tamano, con `(float *)`, o conservando `copySize` ->
  `0aa8b499` (3 filas, las tres cargas); `frames * sizeof(float)` `e415a38b` (260 B); retorno
  temprano como `goto` `16cfb3c3` (264 B); desclavar solo la copia `6b6171d1`.
* snddrv: llamada directa en la 3a / 4a / las dos -> `8cea5895` / `d0d1b478` / `25233c7f`;
  copia sin pin -> el mismo objeto que la llamada directa. Cada pin quitado son 2 filas.

### 3.5 `sfir.c` calcFIRCoeffs (1 barrera de solo entrada)

Sin ella 932 B / 30 filas (`0a94319c`). `0.0f > sum`, `sum < 0`, `sum = 0.0f - sum` -> el mismo
`0a94319c`; la resta partida `5de98f09` (29 filas); ternario `4cf9fdd8` (916 B). (e) no aplica:
no es un pin y la funcion no tiene asm legitimo.

### 3.6 `spchpick.c` (3 pines + 2 barreras)

* ChooseSamples: DWARF da `sentence` como PARAMETRO en r25 (sin `activeSentence`) y ningun
  `sampleTable` (el r17 es temporal del compilador). Usar `sentence` directo `d647e095` (7 filas);
  la copia sin pin, el MISMO objeto; sin copia ni pin de sampleTable `1b9ef0b9` (12); solo
  sampleTable `6c3a4d87` (5); solo la 1a / 2a barrera `c3627a80` (4) / `3142035a` (2).
* MakeSampleRequests: la natural `86ecd1d6` (524 B); `bankIndex` antes del puntero `306141a3`
  (536 B); `bankHandle` de funcion asignado en la condicion, derecha/izquierda `f0a67e6b` /
  `84969082` (528 B); la llamada relee el handle `ac4473a7` (532 B).

### 3.7 `gc_driver.cpp` (2 pines + 2 clobbers)

* ReadFile: DWARF da `int * nBytesRead /* r25 */` como PARAMETRO. Renombrado y sin copia ->
  `f62728fd`: 8 filas, TODAS el ciclo r24<->r25 entre bytesToRead y nBytesRead (la copia sin
  pin da el mismo objeto). Los dos parametros tienen igual referencias y vida; allocno_compare
  desempata por numero de allocno, que sigue la firma. Sin ademas el clobber de cr4
  `fe7e9f97` (468 B, 80 filas).
* OpenFile: desclavar `result` `c35a2636` (51 filas); sin el clobber de r25 `514c3f2f` (5);
  los dos `65d20937` (17).

### 3.8 `avplayer.cpp` GetFirstFrame (1 barrera)

DWARF: ni `audio_stream` ni `video_latency`. La natural (sin locales ni barrera) `4ce7cb82`:
2 filas, `mr r29,r5` delante del `lwz r3,52(r31)`. El mismo objeto sin la barrera a secas, con
solo una de las dos locales, o con `video_latency` primero.

### 3.9 `csis.cpp` (3 pines + 2 barreras) y `criticalpath.c` (3 pines + 1 barrera) -- no reabiertos

* csis: la forma del original ya esta encontrada (r65) y lo que queda vive en
  `CListDStack::Push/Remove` de `snd/9/source/library/cmn/slinklist.h`, compartida. PROPUESTO.
* criticalpath: tres ciclos de dos del asignador ya medidos (r65, r66). El unico asm legitimo
  del fichero es el codec VP6 escrito a mano, que no toca esos operandos y no se modifica.

---

## 4. Reglas nuevas medidas

1. **Local del DWARF SIN registro = local sin uso en el original.** Si ademas un pin la
   sujeta, busca que otra local del mismo tipo carga sus sentencias (sst: `intTemp1` -> `pint`).
2. **Un literal plegado por cprop no es neutro para el reparto**: `exitfunctions[j]` con
   `j = 0` genera el mismo codigo que `[0]`, pero cuenta referencias de `j` antes de
   global_alloc y le da prioridad (exit).
3. **La forma de la guarda mueve el planificador sin mover codigo**: retorno temprano contra
   `if` envolvente cambia la numeracion de insn y el desempate de `rank_for_schedule` (inittmr).
4. **Un bloque de `continue` vacio se iza; una asignacion redundante lo llena** si el mismo
   valor ya la lleva la rama hermana (srender).
5. **Alias: `int x[1]` != `int x` para el planificador**: el array marca `MEM_IN_STRUCT_P` y
   anula la exencion escalar-fijo/struct-variable. Diagnostico directo de barreras `"+m"` sobre
   locales pasadas por referencia antes de una llamada virtual (pathbank).
6. **rs6000 expande los argumentos de primero a ultimo** (no hay `PUSH_ROUNDING`): un objetivo
   que carga r3 el ultimo no sale por orden de argumentos en la llamada (sfsplit, snddrv).
7. **Palanca (e) para GPR: NO medida.** En este lote no hay ningun asm legitimo en las funciones
   con pin de GPR: no habia donde colgar un operando `"=r"`.

---

## 5. Propuestas (ficheros ajenos)

* **csis / slinklist.h**: como en la r65, impedir que cse pliegue `pnode` en
  `pSystemDesc + off` dentro de `CListDStack::Push/Remove`. No hay lineas exactas medidas: exige
  barrer TODAS las unidades de snd con la cabecera sombreada. Encargo propio.
* **pathbank**: sigue abierta la decision de la r66 (`int fileop[1]` cierra, contradice el
  DWARF). La sec. 3.2 explica por que cierra.
* Ninguna de config ni de flags.

---

## 6. Verificacion final

* `check.py` sobre las 15 unidades, desde el arbol final: **TODO IDENTICO**.

| unidad | ALLOC antes = despues | SYM antes = despues |
|---|---|---|
| filesys | `de59e5dfaf2a9107` | `c10e88e6dcae91c3` |
| exit | `ac776748fa2f35a0` | `3d17c5ef85fe929e` |
| inittmr | `dc239e2f1b378fdd` | `2f1e07904b1f3480` |
| spchpick | `6f1d9608b4ac1278` | `332f58f66040e574` |
| csis | `0e8dad66150bcea3` | `76fde2022fdd4caf` |
| sst | `313d112fb460ed84` | `20a1337f54b413e2` |
| snddrv | `14a5081b196a9caf` | `2a698e45776445a2` |
| sfsplit | `0c186c38611698e7` | `6644face59e2d24c` |
| srender | `b7d3ad64326a69c5` | `18c8598bc2904b71` |
| sfir | `a3e6b083a14c2bc0` | `a5cdaa1889b362be` |
| pathnode | `1df498d13a3c1bf2` | `4fc65d376f7e77e5` |
| pathbank | `e38cbb8cff9d93b8` | `96235e268aa5546d` |
| criticalpath | `3466ffd0dec401b8` | `54ff8f9bb42c2d16` |
| gc_driver | `518a282b2fc2e9d8` | `45d054a42761332c` |
| avplayer | `0030f68e5f6aaa4c` | `6555f587c1a2921e` |

(Sello = sha1 de la lista ordenada `seccion=sha1_12:tamano` de ALLOC y sus `.rela`, truncado
a 16; SYM = sha1 de (seccion, nombre, valor, tamano, info) de los simbolos en ALLOC.)

* `git diff --stat` del lote: 15 ficheros, 230 inserciones, 112 borrados. Finales de linea
  preservados (la diff solo trae las lineas tocadas); los 15 son UTF-8 validos.
* No se ha tocado ningun fichero ajeno, ni cabeceras, ni `configure.py`, `config/GOWE69/*`,
  bloques `__ANDROID__` ni el asm VP6. No se ha corrido `lcfix.py` ni se ha commiteado. El DOL
  no puede moverse (ALLOC identico en todo lo tocado); la verificacion por reenlace queda para
  el jefe.
