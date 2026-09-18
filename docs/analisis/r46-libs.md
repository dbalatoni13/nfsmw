# r46 — bibliotecas de GCC (`libs`)

Territorio: `sfir`, `madidct`, `criticalpath`, `pathnode`, `pathtrack`,
`spchsamp`, `dvd_device`. Entrada: 4.468 B en 9 funciones.

## Resultado

**Cerradas dos funciones, 864 B, y con ellas DOS UNIDADES ENTERAS.**

| unidad | funcion | antes | despues |
|---|---|---:|---:|
| `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack` | `PATH_createstreamimp__Fiif` | 424 B / 99,92453 % | **424/424 B, 100,0 %** |
| `Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device` | `StartNonAlignedAyncRead__FP11DVDFileInfoPvll` | 416 B contra 440 / 93,59091 % | **440/440 B, 100,0 %** |

`fndiff` de las dos, citado:

    # PATH_createstreamimp__Fiif  target=100.0%  ours=100.0%  size=424/424
    # StartNonAlignedAyncRead__FP11DVDFileInfoPvll  target=100.0%  ours=100.0%  size=440/440

`fncmp` de las dos unidades: **0 de 26** y **0 de 18** funciones con el codigo
distinto. Comprobado ademas que **todas** las secciones que no son de depuracion
coinciden en tamano en los dos objetos (`.text`, `.data`, `.rodata`, `.sdata`,
`.bss`): solo difieren `.line`, `.debug*` y `.note.split`, que no se comparan.

### Propuesta a coordinacion: dos promociones

Las dos unidades quedan completas y **no las he promocionado yo** (no toco
`configure.py` ni `splits.txt`). Valen, medido sobre el `.text` de los objetos:

| unidad | `.text` |
|---|---:|
| `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack` | **2.472 B** de `linked` |
| `Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device` | **2.704 B** de `linked` |

Total **5.176 B de `linked`** listos para promocionar.

---

## 1. `pathtrack` — el area X del marco (palanca NUEVA)

### El hallazgo

Las 106 instrucciones ya eran identicas desde r36e. La unica diferencia eran
8 B de marco: `stwu r1,-0x28` contra `-0x20`, con los cuatro guardados y los
cuatro restaurados desplazados. Dos rondas lo dieron por «una ranura de derrame
que su asignador reservo y acabo no necesitando… desde la fuente no se alcanza».

**No era una ranura de derrame: es el AREA X del marco SVR4 de rs6000, el
temporal de conversion entero<->flotante.** El marco de gcc 2.9 para V.4 es

    8(fijo) + P(parametros) + A(alloca) + V(varargs) + L(locales) + X + C + G + F

El objetivo: 8 + L(8, `trackimp` en `r1+0x8`) + **X(8)** = 0x18, que es
exactamente donde pone `stmw r30`; luego `stfd f31` en 0x20 y total 0x28.
Nosotros: 8 + L(8) + X(**0**) = 0x10, `stmw r30,0x10`, `stfd f31,0x18`, 0x20.

gcc reserva esos 8 B al **EXPANDIR** una conversion entero<->flotante y **no los
libera aunque el optimizador se lleve despues todo el codigo de la conversion**.

Medidas que lo demuestran (todas con `scripts/build_direct.py` + `-S`):

| forma | marco | instrucciones | tamano |
|---|---:|---:|---:|
| base | −0x20 | 106 | 940→424 B, 99,92453 % |
| conversion VIVA (`(int)((float)(vol*escala)/100.0f)`) | **−0x28** | 117 | 468 B, 82,594 % |
| conversion MUERTA (varias formas) | **−0x28** | **106** | 424 B |

### Que dice el original

El mapa de lineas (`symbols/debug_lines.txt`, `0x80378970`–`0x80378B18`) da el
cuerpo entre las lineas 85 y 123 del `pathtrack.cpp` original, y deja **sin una
sola instruccion** los huecos 92, 94 y **101..109** —nueve lineas seguidas entre
`byterate = PATHI_bytesperms(...)` (linea 100) y la llamada a
`CreateStreamTrack` (linea 110)—. Es decir: ahi habia codigo que el optimizador
se llevo entero, que es exactamente lo que reserva el area X.

Lo que **no** es recuperable es la sentencia: el DWARF del original no nombra
ninguna local de mas, y **una local completamente muerta SI sale en el DWARF**
—comprobado compilando `int buffersize;` muerta con `-gdwarf+` y encontrando la
cadena `"buffersize"` en el `.s`—. Asi que el original la escribio de otra
forma. La que dejo puesta es una **reconstruccion del tipo de sentencia**, no de
su texto, y esta documentada como tal en el propio `.cpp`.

### Sitio: importa, y mucho

| donde | resultado |
|---|---|
| dentro del bloque de `byterate` (donde el mapa de lineas deja el hueco de 9) | −0x28 pero **95,36793 %** (mueve el planificador) |
| delante de `trackimp = 0` | **100,0 %** |
| detras de `switchvoice`, o detras de `if (!track)` | **100,0 %** tambien |

Formas medidas que **no** reservan el area (siguen en 99,92453 %, marco −0x20):
local con inicializador y sin usar; `(void)(float)maxrequests;`;
`(void)pad` sobre `float`/`double`/`int`; `result = (int)(buffertime*byterate)`
sola (sin el `if` detras); un `if` con cuerpo vacio. La conversion tiene que
llegar a RTL: **un `if` con una asignacion real en el cuerpo** es lo que la
salva del front-end, y luego DCE se la lleva.

Tambien medido y NEGATIVO: `double slot; __asm__ __volatile__("" : "=m"(slot));`
—la idea de reservar 8 B con un asm de memoria— **no mueve el marco**
(99,92453 %). El area X solo la abre una conversion de verdad.

### Codigo dejado

```c
    if (!PATHI_lock())
        return 0;

    {
        /* muerta: solo reserva el area X del marco -- ver la nota de arriba */
        int buffersize = (int)buffertime;

        if (buffersize < 0)
            buffersize = 0;
    }

    trackimp = 0;
```

Cero `asm`. La nota larga del `.cpp` explica el mecanismo, la evidencia y el
limite de la reconstruccion.

### Por que esto es una palanca y no un truco de una funcion

La firma es facil de reconocer y hay que buscarla en todo el proyecto:
**mismo numero de instrucciones, todas identicas, y el marco del objetivo 8 B
mas grande con los guardados desplazados**. Si en la funcion no queda ninguna
conversion entero<->flotante, el original tenia una que murio.

---

## 2. `dvd_device` — cerrada por la OTRA punta

Faltaban seis instrucciones y las seis eran `mr`: `mr r28,r3` y `mr r7,r4` al
entrar, y `mr r3,r28` delante de cada una de las cuatro llamadas a
`DVDReadAsyncPrio`. r36e diagnostico «el original mantiene `FileInfo` vivo hasta
el final» y midio **siete** barreras sobre `FileInfo`: ninguna paso de 436 B.

**La causa estaba en el otro parametro.** Con `(int)MemPointer` repetido cinco
veces, gcc deja `MemPointer` en r4 y usa **r3** como temporal
(`subf r3,r30,r12`), asi que `FileInfo` se queda en r3 y no hacen falta copias.
En cuanto `MemPointer` sale de r4, gcc lo lleva a r7 —como el DWARF dice— y
**pone `FileInfo` en r28 por su cuenta**, emitiendo las seis `mr`.

Ensayos, todos medidos:

| ensayo | tamano | fuzzy |
|---|---:|---:|
| base | 416 B | 93,59091 % |
| pin `FileInfo` a r28 (local `register … asm("r28")`) | 424 B | 95,16364 % |
| los dos pines (`r28` + `r7`) | 432 B | 95,19091 % |
| pin `MemPointer` a **r7** | 444 B | 98,61818 % |
| pin `MemPointer` a r8 | 444 B | 98,30000 % |
| pin r7 declarado tras el retorno temprano | 444 B | 85,89091 % |
| **`void *mp = MemPointer;` (sin pin)** | **440 B** | **100,0 %** |
| **`int memPointer = (int)MemPointer;` (sin pin)** | **440 B** | **100,0 %** |
| autoasignar el parametro (`MemPointer = (void*)(int)MemPointer`) | 416 B | 93,59091 % |
| reordenar el grupo `gCurRead.Mem*` (MemStart primero) | 416 B | 93,59091 % |

El pin a r7 se queda a 4 B porque el `register asm` vive en toda la funcion y su
rango cruza el `QEndOp()` del retorno temprano, asi que gcc guarda una copia en
un preservado (r26) y la recarga en r7: una `mr` de mas. **Sin pin no pasa.**

Dejo `int memPointer = (int)MemPointer;` porque ademas quita cinco casts
repetidos. Cero `asm`. Nota de fidelidad escrita en el `.cpp`: el DWARF del
original solo nombra `readSize` y el bloque anonimo con `sizealigned`, asi que
esta local es una reconstruccion; el objeto sale exacto.

**Regla generalizable:** cuando faltan `mr` de argumento, la palanca puede estar
en el parametro que **NO** aparece en el diff. Aqui siete barreras sobre el
parametro «culpable» no llegaron, y una local sobre el otro cerro la funcion.

---

## 3. `pathnode` — avance medido, NO retenido (cero bytes)

`PATHI_nextnode` (308 B objetivo, 300 B nuestros, 93,05195 %).

Reproducida la base de r36e y **avanzada**:

| ensayo | tamano | fuzzy | filas reales |
|---|---:|---:|---|
| base | 300 B | 93,05195 % | — |
| barrera de lectura sobre `forreal` al final | 308 B | 95,88312 % | 7 |
| **+ `register int nextnode asm("r12") = -1;`** | **308 B** | **96,207794 %** | **5** |
| solo el pin, sin barrera | 300 B | 93,376625 % | — |
| barrera + pin de `nextnode` a r6 | 308 B | 95,88312 % | — |

Con las dos cosas queda **una sola causa**: sobra nuestro
`lwz r9,_4Path.pfstate@sda21` (indice 25) y falta el `mr r6,r9` del objetivo
(indice 10). El objetivo saca de CSE una **copia** para la tercera referencia a
`Path::pfstate` dentro del inline `PATHI_getnode` y la mantiene viva hasta
`track = pfstate->track[...]`; nosotros fundimos las tres referencias en un
pseudo y **recargamos** el global.

Medido y neutro (los tres siguen en 96,207794 %): `Path::pfstate` explicito,
`pfstate_agg[0]` para el acceso a `track`, y una local `PATHTRACK **tracks`.
Intercambiar las sentencias `branches` / `track` no mueve nada (93,05195 %).

**CFLAGS descartados** (sobre la base con barrera+pin): `-fforce-mem`, `-fgcse`,
`-fcse-skip-blocks` no cambian el 96,207794 %. Y son **nocivos**:
`-fno-cse-follow-jumps` → 316 B / 94,8442 % y ademas rompe `PATHI_seeknextnode`;
`-fno-schedule-insns2` → 92,3117 % y tambien rompe `seeknextnode`;
`-fno-strength-reduce` → rompe CUATRO funciones de la unidad. **Los cflags de
`path` estan bien**; no hay sospecha que proponer.

**Revertido**: barrera + pin son cero bytes. Todo escrito en la veda del `.cpp`.

**Propuesta fuera de territorio.** La palanca que falta esta en
`src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathi.h` (cabecera compartida por
las 13 unidades de `path`), en el inline `PATHI_getnode`, que referencia
`Path::pfstate` tres veces. Hay que conseguir que la tercera referencia salga
como **copia** y no fundida. **No la he tocado.** Cualquier cambio ahi hay que
medirlo sobre las 13 unidades a la vez: el propio `pathi.h` documenta que la
vista agregada `pfstate_agg` da −896 B en `pathnode` pero +188 en `pathtrack` y
+392 en `pathinit`.

`PATHI_calcwaitbeat` (336/336 B, 98,86905 %, 14 filas todas de registro) no se
toco: es PERMUTADOR puro y r44 ya agoto tres hipotesis causales.

---

## 4. `madidct` — CTR y LR vivos a la vez: SI se alcanza

Era lo unico que el brief daba por no probado. **Se alcanza, y con ello el
TAMANO EXACTO de `IdctRow` por primera vez.** Receta completa:

```c
static void IdctRow(int *src, int *dest) {
    int t1, t2, t3, t4, t5, t6, t7, t8, t9;
    int src6 = src[6];               /* y src[6] -> src6 en sus dos usos */

    __asm__("" : "+c"(src6));
    ...
    t9 = (src[2] + src6) + t8;
    __asm__("" : "+l"(t9));
```

**516/516 B, 129 instrucciones contra 129, 48,24031 %** (la base es 496 B /
124 instrucciones / 46,372093 %). Lo que queda del multiconjunto:

    lwz   objetivo  9 / nuestro 10        mr    objetivo 10 / nuestro  8
    mfctr objetivo  2 / nuestro  1        mflr  objetivo  2 / nuestro  3
    mtlr  objetivo  2 / nuestro  3

O sea: falta la **segunda recuperacion de CTR** y sobra **una vuelta por LR**.

Variantes medidas encima de esa base, todas PEORES:

| ensayo | tamano | fuzzy |
|---|---:|---:|
| `"+c"` delante de cada uso de `src6` | 524 B | 40,32558 % |
| `"+c"` arriba mas delante del segundo uso | 524 B | 40,32558 % |
| anadir `"+l"(t8)` | 520 B | 47,372093 % |
| anadir `"+l"(t6)` detras | 516 B | 48,24031 % (identico) |
| `"+l"(t8)` solo | 500 B | 49,155037 % |
| `"+l"(t9)` solo | 524 B | 43,953487 % |
| `"+c"(src6)` solo | 476 B | 31,891474 % |

`IdctColumn` no se mueve en ninguno: 632/632 B, 50,449368 %.

**NO se deja puesta**: son dos `asm` y una local `src6` que el DWARF no nombra,
a cambio de cero bytes. Sigo el precedente de r44/r45 en esta misma unidad. La
receta esta escrita entera en el `.cpp` y se reaplica en un minuto; el siguiente
que la coja empieza con el tamano exacto y solo cinco mnemonicos de diferencia,
no con 20 B y cinco instrucciones de menos.

---

## 5. `sfir` — el deficit contado exacto, y cuatro formas nuevas descartadas

`calcFIRCoeffs`, 940 B contra 936. **La lectura de «4 B de mas y cero de menos =
sobra un `if` defensivo» no se sostiene**: hay 11 INSERT y 10 DELETE, net +1.

- **Nuestros** INSERT: `slwi r28,r11,2` y `addi r29,r30,0x20` en la **arista
  default del switch** (indices 33/34), mas tres `mr r29,r10` —uno por `case`—
  en 46/77/113.
- **Del objetivo**: `slwi r0,r26,2` en 39/70/105/202/210 y `addi rN,r29,0x20`
  en 203/211.

Es decir: gcc nos crea un **bloque puente en la arista default** para que
`&pfir->coef[halfLen]` este disponible en la cola (PRE), y el objetivo lo
recalcula en cada uso. El objetivo mantiene `halfLen` en r26 y `pfir` en r29 y
paga cuatro recalculos; nosotros cacheamos `halfLen*4` en r28 y `pfir+0x20` en
r29 y pagamos cinco copias.

Formas nuevas medidas, **las cuatro dan el objeto IDENTICO a la base**
(940 B / 89,25214 %): `default: break;` en el switch; un puntero local
`float *coef = pfir->coef` antes del switch; quitar la local muerta
`fir_coef_var`; y reescribir la cola sin cambiar la expresion. EMPEORA partir
`sum = sum + sum - pfir->coef[halfLen]` en dos sentencias: 89,209404 %.

No se toco nada. Veda ampliada en el `.c`.

---

## 6. `spchsamp` — barrido completo y negativo

`iSPCH_GetSampleSizeData`, 136/136 B (tamano ya exacto), 85,73529 %, 15 filas.
El DWARF da el reparto entero del original y **la lista de locales coincide
exactamente con la nuestra**; es una rotacion de cinco valores mas el sitio del
`li 1`. El objetivo termina con `subf r0,r8,r0` / `stw r8,0(r5)` /
`stw r0,0(r6)` / `li r0,1` y nosotros metemos el `li 1` **entre los dos stores**,
porque nuestro `result` no comparte r0 con `endOffset`.

| ensayo | fuzzy |
|---|---:|
| base | 85,73529 % |
| pin `blockSize`→r11 | 86,32353 % |
| pin `nextSampleData`→r4 | 86,02941 % |
| pin `offset`→r8 | 85,588234 % |
| pin `result`→r0 | 85,588234 % |
| pin `sampleSize`→r7 | 85,29412 % |
| parejas y los cinco a la vez | de 86,32353 % a 83,382355 % |
| **las 48 reordenaciones legales del cuerpo** | **85,73529 % las 48** (mismo objeto) |
| reutilizar `endOffset` para el producto | 86,47059 % |
| `endOffset *= blockSize` | 86,47059 % |
| `result = 1` delante | 85,73529 % |
| invertir los dos stores | 91,76471 % **pero con el orden de stores al reves del objetivo** — el fuzzy sube bajando filas exactas: no vale |
| locales de cache (`numSamples`, `base`, estilo `dvd_device`) | 85,73529 % |
| invertir el `if (nextIndex >= hdr->numSamples)` | 56,32353 % |

Las 48 reordenaciones son el dato util: **gcc canonicaliza el orden de esas
nueve sentencias**, asi que ese eje esta cerrado. Lo que queda es meter `result`
en r0 (que es lo que fuerza el orden del `li 1`) y el pin directo no lo logra.
Veda nueva escrita en el `.c` (la unidad no tenia ninguna).

---

## 7. `criticalpath` — el adelanto no se corrige reasociando + deuda de datos

`VP6_PredictFilteredBlock`, 740/740 B (tamano exacto), 94,39459 %, 37 filas.
r36f dejo dicho que la primera diferencia que no es de registro es el adelanto
de `lwz 0x88(r31)` por delante del `lhax` de `mVx`. Atacado **desde la fuente**,
reasociando la sentencia de `TempBuffer` —que es de donde sale ese `lwz`—, sin
un solo `asm`:

| forma | fuzzy |
|---|---:|
| base | 94,39459 % |
| `my * pbi->mbi.FrameReconStride` (factores al reves) | **94,556755 %** |
| `... + mx + FrameReconStride * my` | 90,98919 % |
| partida en dos sentencias | 91,0973 % |
| el producto delante del todo | 92,23243 % |

El orden de los factores mueve 0,16 pt y nada mas: **el adelanto es scheduling,
no asociatividad**. No se retuvo (0,16 pt no es un byte). Los 740 B son exactos
en los cinco casos.

### Deuda de DATOS de esta unidad (aparte de la funcion)

`.rodata` mide **632 B** en nuestro objeto y **636** en el objetivo. Los 4 B son
una palabra a cero al final de la seccion, **detras** de `loMaskTbl_VP60`.
Comprobado que **no** es una entrada de la tabla: declarandola `[34]` el simbolo
pasa a 136 B y el objetivo lo tiene en 132 (98,50746 %). Falta un objeto `const`
de 4 B a cero al final de `.rodata`. Anotado en el `.c`; no lo he inventado.

---

## Verificacion

- `fncmp` **antes y despues** sobre las siete unidades, diff completo: las
  unicas lineas que cambian son las dos funciones cerradas. **Ninguna funcion
  empeora, en ninguna unidad.** (`scratchpad/r46_libs/base/fncmp_before.txt`
  contra `scratchpad/r46_libs/fncmp_after.txt`.)
- `python scripts/lcfix.py --check` → `todas las entradas @lc estan al dia`.
- `fndiff` = **100.0** en las dos cerradas (citado arriba).
- Los cinco ficheros que solo llevan comentario nuevo (`sfir.c`, `madidct.cpp`,
  `criticalpath.c`, `pathnode.cpp`, `spchsamp.c`) se recompilaron desde su copia
  de base y desde la version editada: **`.text` byte-identico en los cinco**
  (SHA-256 del blob de seccion, mismo tamano).
- Todo se compilo con `python scripts/build_direct.py <unidad>`. **No se lanzo
  ningun `ninja` ni `configure.py`**, no hay commits ni `git add`, y no se toco
  `configure.py`, `config/GOWE69/*` ni `splits.txt`.

## Ficheros modificados

| fichero | que lleva |
|---|---|
| `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp` | **codigo** (cierre) + nota |
| `src/Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device.cpp` | **codigo** (cierre) + nota |
| `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp` | solo veda |
| `src/egami/rcmp/dev/source/decoder/cmn/madidct.cpp` | solo veda |
| `src/Speed/Indep/Libs/snd/9/source/library/mix/sfir.c` | solo veda |
| `src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c` | solo veda |
| `src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | solo veda |

Finales de linea conservados: `pathtrack.cpp`, `pathnode.cpp` y
`criticalpath.c` siguen LF puro; `sfir.c`, `spchsamp.c`, `dvd_device.cpp` y
`madidct.cpp` siguen CRLF puro (`dvd_device.cpp` tenia UNA linea suelta en LF
dentro de la veda vieja que se ha ido con ella; el fichero queda 454/454).

Ensayos y utilidades en `scratchpad/r46_libs/` (`probe.py`, `tryv.py`,
`frame.py`, `asmof.py`, `flags.py`, `base/`).
