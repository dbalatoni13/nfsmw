# r66 -- lote `libs-sn`: andamios de bibliotecas y runtime

Encargo: dejar fuente legitima. Test unico: sello por seccion de las ALLOC
(`.text`, `.rodata`, `.data`, `.sdata*`, `.ctors`, `.bss` por tamano, y sus `.rela`)
MAS la tabla de simbolos restringida a las ALLOC (la parte SYM de la r65-libs, que
caza un reorden de `.bss`). Identico -> se acepta; distinto -> se revierte en el acto.

**Veredicto: PARCIAL, y la causa dominante del lote NO era la de flags.**
12 andamios retirados (73 -> 61), 0 regresiones, las 24 unidades del lote
recompiladas desde el arbol con el sello IDENTICO al del `.o` committeado. Los 61
que quedan llevan diagnostico r66 escrito junto al andamio.

---

## 0. Resumen

| grupo | real antes | real despues | regex del encargo antes -> despues |
|---|---|---|---|
| LibSN | 24 | 23 | 20 -> 19 |
| Libs/path | 14 | **4** | 6 -> 1 |
| Libs/realcore | 7 | 7 | 7 -> 7 |
| Libs/snd | 8 | 8 | 7 -> 7 |
| Libs/spch | 5 | 5 | 5 -> 5 |
| Libs/csis | 5 | 5 | 3 -> 3 |
| Packages | 9 | 8 | 6 -> 5 |
| egami | 1 | 1 | 1 -> 1 |
| **total** | **73** | **61** | **55 -> 48** |

"Real" = pines `register T x asm("rN")` + barreras `__asm__("" ...)` **y** `asm("" ...)`
(la grafia corta que la r65 destapo), contado sin comentarios
(`scratchpad/libs65/censo65.py`). La regex del encargo solo ve `__asm__("")`.
El encargo decia 57 con path = 8; mi recuento con su regex da path = 6 y total 55.
No he encontrado de donde salen los 2 de mas.

Retirados (12): pathaction 4, pathsnd 3, pathnode 2, pathevent 1, FSasync 1,
criticalpath 1. Ademas sale una local inventada (`updateStatus` en pathsnd) y
`sfir.c` pasa a ASCII puro.

---

## 1. Metodo y controles (`scratchpad/libssn66/`)

* `cc.py` -- compila una unidad a un `.o` PRIVADO y da el sello ALLOC+SYM. Nunca
  escribe en `build/GOWE69`, asi que no hay "Could not open output file" ni `.o`
  rancio posible.
* `baseline.py` -- compila las 24 unidades del lote y compara con el `.o` del arbol.
  **Al empezar: 24/24 IGUAL-AL-ARBOL.** Nada rancio.
* `sweep.py` -- barrido FLAGS x ANDAMIOS sobre COPIAS del fuente (con `-I <dir
  original>`), sin tocar el arbol: control, cada andamio quitado a solas y todos
  quitados, para cada toggle de flag.
* `formsweep.py` -- prueba formas de fuente (sustituciones) contra el sello A.
* `apply_tree.py` -- aplica una forma AL ARBOL, re-sella y restaura en el acto si
  cambia. Todas las retiradas y anotaciones han pasado por aqui.
* `metrotrk_oracle.py` -- compara funcion a funcion `metrotrk.c` contra el `.o`
  ensamblado del `.s`.
* `annotate.py` -- escribe el diagnostico r66 junto a cada andamio que queda,
  re-sellando cada unidad.

**Controles que pueden fallar**: en el barrido, cada andamio quitado a secas da un
sello DISTINTO del base (comprobado en las 24 unidades; p.ej. sndvd L249/L338/L363 ->
`16cbc252`/`681f464e`/`0763fc3e` contra `934fe3a6`). En cada forma probada va su
negativo natural, y todos cambian (p.ej. pathaction `04a0fe2d`/`4bdf1b7e`).

---

## 2. FLAGS: descartados con medida

**2.352 compilaciones.** 24 unidades (todas las `prodg` del lote), por cada una el
control, cada andamio a solas y todos a la vez, con cada uno de hasta 24 toggles en
la direccion contraria a su estado efectivo: `strength-reduce`, `regmove`,
`caller-saves`, `gcse`, `expensive-optimizations`, `schedule-insns{,2}`,
`rerun-cse-after-loop`, `rerun-loop-opt`, `cse-follow-jumps`, `cse-skip-blocks`,
`force-mem`, `thread-jumps`, `defer-pop`, `omit-frame-pointer`, `branch-count-reg`,
`peephole`, `function-cse`, `strict-aliasing`, `force-addr`, `move-all-movables`,
`unroll-loops`, `-G0/-G8` y `-O2` en las que van con `-O1`.

Antes de barrer se miro que pone `-O2` en este GCC (`toplev.c:5134`): sobre la lista
a mano de `cflags_game` solo anade **`-fstrength-reduce`, `-fcaller-saves` y
`-fregmove`**. Por eso eran las sospechosas en `csis`, `realcore` y `realmemcard`,
que van con `-O1` + lista. Y `flag_strict_aliasing` vale 0 por defecto
(`toplev.c:892`): el `-fno-strict-aliasing` de snd/path/spch no hace nada.

**Resultado: CERO aciertos atribuibles a un flag.** Los unicos aciertos (FSasync L278,
L306; criticalpath L799) salen IGUAL sin tocar ningun flag: son andamios caducados
(sec. 4). Ningun toggle retira un andamio que no se retire ya sin el.

Complementos:

* `csis`: la forma del original `gSystems.Push(&pSystemDesc->linkNode)` probada con
  **18 juegos** de flags (`-fno-force-addr`, `-O2`, `-fstrength-reduce`, `-G8`,
  `-fno-gcse`, los `cse-*`, `schedule-insns*`, `regmove`, `caller-saves` y dos
  combinaciones). Ninguno. Dato: con `-fno-force-addr` la forma Push y la
  andamiada salen IGUALES entre si (`1c551220`), pero cambia el resto de la unidad.
* `-fno-rerun-cse-after-loop` da **error interno del compilador** (`flow.c:993`) en
  FSasync, sndvd, pathevent, exit, gc_driver y criticalpath. El original no pudo
  compilarse asi.
* Correccion al encargo: `criticalpath.c` **no es Metrowerks**. `build.ninja` le
  asigna la regla `prodg` (ngccc, `cflags_vp6`) y sus pines son `asm("rN")` de GCC.

**`propuestas_config`: ninguna.** Ningun flag del lote tiene evidencia.

---

## 3. La causa que SI era: ALIAS, en toda la biblioteca `path`

Cinco ficheros de `path` compartian un patron: una lectura de global que GCC adelanta
por encima de una escritura por puntero, y un andamio para impedirlo. No es reparto:
es `true_dependence` (`gcc/alias.c:1305`), que llama a
`fixed_scalar_and_varying_struct_p` y **descarta el conflicto entre un ESCALAR en
direccion fija y un CAMPO DE STRUCT en direccion variable**. El original no adelanta
esas cargas. La palanca ya existia en el arbol y documentada (`pathi.h:504`,
`pfstate_agg`): una vista con TIPO AGREGADO del mismo simbolo, que pone
`MEM_IN_STRUCT_P` y no emite nada.

| fichero | andamios | forma que los sustituye | sello (ALLOC / SYM), identico | negativo |
|---|---|---|---|---|
| `pathaction.cpp` | 2 barreras + 2 pines | `lastendif = Path::milliseconds_agg[0];` y `lastwhile_agg[0] = Path::milliseconds; action->done = !action->done;` | `7e9c23c92e69ad54` / `1a453820091ff25f` | `04a0fe2d`, `4bdf1b7e` |
| `pathsnd.cpp` ctor | 1 pin + 1 barrera | `PATH_AGG(Path::IPathToSnd::sndimp)->GetDefaultPlayOpts(this->mSndPlayOpts);` | `2de50a498a08f50e` / `47712b9de7145625` | `f4352ca8` |
| `pathsnd.cpp` `Play` | 1 barrera + local `updateStatus` | `subbank->lastplaytime = Path::milliseconds_agg[0]; this->mUpdateStatus = 1;` | (mismo) | `9066e947` |
| `pathnode.cpp` | 2 barreras | `pfstate_agg[0]->timerinterval` | `1df498d13a3c1bf2` / `4fc65d376f7e77e5` | `90494517` |
| `pathevent.cpp` | 1 barrera | las TRES lecturas del bucle por `pfstate_agg[0]` | `0fb91f6f3b2d792e` / `b19e4b097bf5c71a` | `ee1414a6`; solo la condicion `82aafbfd`, solo el cuerpo `0d018b84`, solo pmap `43771ea0` |

`pathi.h` gana `extern unsigned int milliseconds_agg[1] __asm__("_4Path.milliseconds") PATH_SDA;`
(enlace de nombre, lo que el encargo NO cuenta como andamio). Por la regla 4 se
re-sellaron **las 14 unidades de path** antes y despues de tocarla: 14/14 identicas
(pathcontrol `137b44ba`, pathinit `df3d6717`, pathreal `ce11721f`, pathreal6 `c5389cfa`,
pathserv `7f3dab7b`, pathtrack `31cecc37`, pathvol `8e26feb2`, pathbank `e38cbb8c`,
pathdebug `d907112e`, pathrand `a8a47864`, y las cuatro de la tabla).

**Trampa nueva, medida (para TRAMPAS.md)**: en `pathsnd` la vista **con nombre** para
`sndimp` (`extern ... pathsndimp_agg[1] __asm__("_Q24Path10IPathToSnd.sndimp")`, con
`__typeof__`, con tipo explicito o dentro de `namespace Path`) CAMBIA el objeto,
ALLOC y SYM (`7427db76` / `83c08b1c`): una segunda declaracion con el mismo nombre
ensamblador, en una unidad que ya referencia la primera, **reordena la tabla de
simbolos**, y con ella los indices de `.rela`. La lvalue agregada por cast
`((*(__typeof__(x) (*)[1]) &(x))[0])` no declara nada y da el sello exacto. Para
`milliseconds` la vista con nombre si vale.

**Negativos de la misma familia** (anotados junto al andamio):

* `pathserv.cpp` `PATHI_switchproject`: vista agregada en la escritura, en la
  lectura, en las dos, y por cast: las cinco cambian (`d5b6d534`, `ac36b342`,
  `3c7e2fdd`; SYM `c9006d0a`). No es esta palanca.
* `pathsnd.cpp` `CheckStatus` (clobber `"memory"` antes de `timercallsinarow`): vista
  con nombre `f4fe092e`, por cast `f40fb139` (SYM `c1cc4662`), a secas `fc837462`.
* `pathbank.cpp` `PATHI_loadbankdata`: **`int fileop[1]` da el sello IDENTICO sin la
  barrera** (dos grafias). **NO APLICADO**: el DWARF del original
  (`symbols/mw_dwarfdump.nothpp:2845907`) dice `int fileop; // r1+0x208`. Seria
  mentir el tipo de una local que sale en nuestro propio `.debug`. Decision para el
  jefe; queda escrito junto a la barrera.

---

## 4. Andamios caducados (sin flag, sin forma)

* **`FSasync.c` CompletePCreadAsync, pines `block_high asm("r30")`**: quitando
  cualquiera de los dos A SOLAS el sello es identico (`395b1828d28dc4ac` /
  `dad1a80b0785ccef`); quitando los dos, `9347fe9c`. Los dos rompen el mismo empate
  (la permutacion r30<->r31 de la r65). Retirado el del segundo bloque; el otro se
  queda con la nota.
* **`criticalpath.c` VP6_ReadTokensPredictB, pin `finalOffset asm("r0")`**: sobraba
  por su cuenta (`3466ffd0dec401b8` / `54ff8f9bb42c2d16`). Quitar el de `finalIndex`
  cambia (`dd449f5e`), y tambien `asm("" : : "r"(i), ...)` sin local o la expresion
  `i - 1` directa. Corrige la nota r65, que decia que los tres eran un solo andamio.

---

## 5. `metrotrk.c`: los 12 pines clasificados contra el oraculo

El `.c` no se compila (`build.ninja:1724` ensambla el `.s`; `objdiff.json:787` apunta
al `.s`), asi que `build/GOWE69/src/LibSN/metrotrk.o` son los bytes del original.
Compilado el `.c` con los flags de libsn: **7 de 8 funciones identicas**. La octava,
`fn_80310F94`, difiere en 5 instrucciones del orden del prologo y no tiene pines.

La tabla de la r65 decia `header` r6 x3 y cuadraba 11; **son cuatro**. Pin a pin:

| pines | sin el pin | original |
|---|---|---|
| salida del `asm` de direccion: `state` x2, `connected`, `header` x4 (L74, L87, L121, L128, L137, L147, L160) | `lis r9` (`3d20....`) | `lis r3/r4/r6` |
| temporales de un uso: `booted`, `disconnected`, `current` x2 (L75, L88, L148, L161) | r0 (`38000001`, `90040000`, `80060010`) | r4 / r5 |
| `value` de `fn_80311A10` (L146) | 24 -> 28 B, `mr r0,r3` de mas | -- |
| barrera de `fn_80311A28` (L165) | 28 -> 32 B | -- |

Ninguno es de flags ni de forma: **el autor eligio los registros a mano**. Sigue en
pie la propuesta de la r65: `git rm src/LibSN/metrotrk.c` retira los 13 de golpe
sin tocar ningun arco de build. No lo he hecho: borrar un fichero no me corresponde.
Anotado en la cabecera del `.c`.

---

## 6. Irreducibles que quedan (61), con su diagnostico junto al andamio

Todos llevan ademas la linea r66 de FLAGS DESCARTADOS (`annotate.py`, 48 sitios en 21
unidades, cada unidad re-sellada identica; `previo.py r66` los indexa).

| fichero | andamios | diagnostico |
|---|---|---|
| `LibSN/metrotrk.c` | 12 pines + 1 barrera | registros elegidos a mano; fichero muerto (sec. 5) |
| `LibSN/FSasync.c` | 1 pin, 3 barreras | pin: empate r30/r31 que ponen las 7 refs del `FS_BLOCK_*` (deuda de splits); `16<<8`: pliegue de constante, doce formas; EXI2TCHandler: adelantos de sched2 sobre globales `volatile` |
| `LibSN/sndvd.c` | 1 pin, 2 barreras | DSIHandler: `allocno_compare`; cross-jumping de dos rabos identicos; adelanto de sched2 del `mr r3,r29` |
| `LibSN/crt2D1.c`, `ppc2D2.c`, `syscalls.c` | 1 + 1 + 1 | sin DWARF (runtime SN); pliegue de CSE de la direccion (`ppc2D2`, cuatro flags ya descartados alli antes) |
| `Libs/csis/csis.cpp` | 3 pines, 2 barreras | forma del original encontrada (Push/Remove, 98,96/99,02 %), queda la base de CSE; el arreglo esta en `slinklist.h` (compartida con snd); 18 juegos de flags negativos |
| `Libs/path/pathbank.cpp` | 1 barrera | `int fileop[1]` cierra pero contradice el DWARF (sec. 3) |
| `Libs/path/pathserv.cpp` | 1 barrera | alias negativo, cinco formas |
| `Libs/path/pathsnd.cpp` | 2 barreras | `framePad` (hueco de marco); `timercallsinarow`: alias negativo |
| `Libs/realcore/filesys.cpp` | 5 barreras | AddToQueue: ESTRUCTURA (bloque anonimo); forma natural 252 B, 96,83 %, una fila |
| `Libs/realcore/exit.cpp` | 1 pin | permutacion r10/r11 local_alloc/global_alloc; tres formas de una sola local |
| `Libs/realcore/inittmr.cpp` | 1 barrera | desempate de `rank_for_schedule`; nueve formas |
| `Libs/snd/srender.c` | 1 barrera | hoisting de la carga comun a los dos sucesores |
| `Libs/snd/sst.c`, `snddrv.c`, `sfsplit.c` | 2 + 2 + 2 | orden de carga de argumentos / permutacion con el mismo arbol de bloques |
| `Libs/snd/sfir.c` | 1 barrera | MISMO REPARTO en regmap: temporal de FPR del compilador |
| `Libs/spch/spchpick.c` | 3 pines, 2 barreras | ChooseSamples: temporal `bank+0xC` sin local; MakeSampleRequests: empate de sched1 (98,32 %) |
| `Packages/realmemcard/gc_driver.cpp` | 2 pines, 2 barreras | MISMO REPARTO / clobbers de r25 y cr4 |
| `Packages/vp6/criticalpath.c` | 3 pines, 1 barrera | ciclos cerrados de dos (r26/r27, r23/r24), cinco y tres formas medidas |
| `egami/avplayer.cpp` | 1 barrera | adelanto de la copia del parametro (r36d) |

---

## 7. Verificacion final

* **24/24 unidades del lote**, recompiladas desde el arbol final a `.o` privados:
  sello ALLOC y SYM **IGUAL al del `.o` committeado** en `build/GOWE69/src`
  (`baseline.py`). Sellos: crt2D1 `2b1667fc04134f98`, ppc2D2 `5dd104c13671c5af`,
  FSasync `395b1828d28dc4ac`, sndvd `934fe3a63686312b`, syscalls `cd823ee704b8e292`,
  csis `0e8dad66150bcea3`, pathaction `7e9c23c92e69ad54`, pathbank `e38cbb8cff9d93b8`,
  pathsnd `2de50a498a08f50e`, pathnode `1df498d13a3c1bf2`, pathevent `0fb91f6f3b2d792e`,
  pathserv `7f3dab7ba9c33ec7`, filesys `de59e5dfaf2a9107`, exit `ac776748fa2f35a0`,
  inittmr `dc239e2f1b378fdd`, srender `b7d3ad64326a69c5`, sst `313d112fb460ed84`,
  snddrv `14a5081b196a9caf`, sfir `a3e6b083a14c2bc0`, sfsplit `0c186c38611698e7`,
  spchpick `6f1d9608b4ac1278`, gc_driver `518a282b2fc2e9d8`, criticalpath
  `3466ffd0dec401b8`, avplayer `0030f68e5f6aaa4c`. (Sello = sha1 de la lista
  ordenada `seccion=sha1_12:tamano`, truncado a 16.)
* `git diff` del lote: 26 ficheros, 244 inserciones, 61 borrados. Los borrados son
  exactamente los andamios retirados, sus comentarios r18/r65 ya falsos, la local
  `updateStatus` y la declaracion escalar de `lastwhile`.
* **UTF-8**: los 26 validos. `sfir.c` no lo era en HEAD (cuatro `0xAB`/`0xBB` de
  comentario) y pasa a `"`: sello identico, mismo numero de lineas.
* **Finales de linea** preservados fichero a fichero contra la copia de seguridad
  (`FSasync.c` CRLF; `pathaction.cpp` CRLF salvo su cabecera LF, que solo crece).
* No se ha tocado `configure.py`, `config/GOWE69/*`, `splits.txt` ni ningun bloque
  `__ANDROID__`; no se ha corrido `lcfix.py` ni se ha commiteado. El DOL no puede
  moverse (ALLOC identico en todo lo tocado); la verificacion por reenlace queda
  para el jefe.

## 8. Para la siguiente ronda

1. **La palanca de alias se busca por patron, no por fichero**: una barrera `"+m"` o
   un clobber `"memory"` justo antes de leer un global escalar tras escribir por
   `ptr->campo`. En path valio 10 de 14.
2. Decidir `pathbank` (`int fileop[1]`) y el borrado de `metrotrk.c` (13 andamios).
3. Dos trampas para `TRAMPAS.md`: la segunda declaracion con el mismo nombre
   ensamblador que reordena la tabla de simbolos, y el ICE de
   `-fno-rerun-cse-after-loop` (un FALLO del barrido que no es del script).
