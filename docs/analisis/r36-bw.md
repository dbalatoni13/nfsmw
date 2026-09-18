# r36 · grupo `bw` — zBWare: `.data` cerrada, `.rodata` de −256 a −120

## Verificación del encargo (antes de tocar nada)

    linkdelta.py zBWare  ->  .text +0   rodata-256  data-64
    measure.py   zBWare  ->  35.772/35.772 B  100,0000 %  239 funciones
    trypromo.py  zBWare  ->  DOL ROTO (d26a146e8eb8)

Reproduce **exacto**. El `.text` no estorba y no se ha movido: al acabar sigue
dando `+0` y `measure` sigue en 100 % con las mismas 239 funciones.

## Resultado

| | antes | ahora |
|---|---|---|
| `linkdelta` | `.text +0  rodata−256  data−64` | **`.text +0  rodata−120`** |
| `.data` del objeto | 2.828 / 2.896 B | **2.896 / 2.896 B, byte a byte** |
| `.rodata` del objeto | falta 204, sobra 204 | falta 144, sobra 200 |

**312 B de déficit → 120 B.** No promociona: los 120 que quedan están
diagnosticados byte a byte y **40 de ellos no tienen arreglo de fuente** (§5).

---

## 0. Lo primero: el déficit de zBWare NO era dato ausente, era ESTRIPADO

El objeto ya tenía la `.rodata` del tamaño exacto (2.832/2.832). Los −256 del DOL
salen enteros de `-strip-unused-data`, y la regla se midió y **cuadra al byte**:

> **El enlazador quita `size & ~7` de CADA símbolo de datos muerto.** Un símbolo
> de menos de 8 B no pierde nada.

    $LC228  "SlotPool::GetAllocatedSlots"                  28 B -> 24
    $LC269  "Tracing disabled for this pool"               31 B -> 24
    $LC273  "Persistent Memory Pool"                       23 B -> 16
    $LC280  "bSharedStringPool"                            18 B -> 16
    $LC288  "Parameter %d wouldn't fit into char in %s\n"  43 B -> 40
    _vt.Q32EA9Allocator14ICoreAllocator                    40 B -> 40
    _vt.19bFunkServerPlatform                              48 B -> 48
    _vt.11bFunkServer                                      48 B -> 48
                                                          suma = 256

En el objeto EXTRAÍDO no se estripa nada: **dtk no crea un símbolo por cadena,
crea uno por dirección REFERENCIADA**, y las cadenas muertas quedan absorbidas
dentro de un símbolo vivo más grande (`lbl_803D1288` son 120 B: el formato de
"out of memory", vivo, **más** "Persistent Memory Pool", muerta). Por eso
`keep.lst` sólo necesita 7 entradas para zBWare y ninguna es de esas cadenas.

Las cinco cadenas están muertas **también en el original**: son el argumento
`debug_text` de `bMalloc`, que la `inline` de `bWare.hpp:97` descarta. Se
internan y no se referencian nunca. Comprobado con las reubicaciones de los dos
objetos (sólo aparecen en `.rela.debug`).

---

## 1. El mecanismo nuevo: **pegar la cadena muerta al literal VIVO que la precede**

Si dtk las mete en un símbolo, nosotros también podemos: **en un solo literal de
C, con los NUL de relleno que tiene el objetivo**. `printf` para en el primer
NUL, el puntero es el mismo, el `.text` no se mueve ni un byte, y el símbolo
entero pasa a estar vivo.

    // bMemory.cpp
    #define BWARE_OUT_OF_MEMORY_FMT "ERROR:  Out of memory in pool %s ... Total = %d\n" \
                                    "\0" "Persistent Memory Pool" "\0"
    #define BWARE_PERSISTENT_MEMORY_POOL (BWARE_OUT_OF_MEMORY_FMT + 96)

El segundo `#define` es lo que impide que `cc1plus` interne otra copia en la
llamada a `bMalloc`: apunta **dentro** del literal, al desplazamiento que le da el
objetivo. Cuatro sitios, medidos uno a uno:

| ensayo | símbolo de dtk | qué se pega | `rodata` |
|---|---|---|---|
| **e3** | `lbl_803D1288` (120 B) | `"Persistent Memory Pool"` al formato de out-of-memory | −256 → **−240** |
| **e4** | `lbl_803D13F8` (88 B) | `"…fit into char…"` a `"…fit into short…"` | |
| **e5** | `lbl_803D122C` (60 B) | `"NULL"`, `"Stomp Detector"` y `"Tracing disabled for this pool"` al `"\n"` de `PrintAllocations` | |
| **e6** | `lbl_803D1360` (84 B) | `"Could not find command %s in %s\n"` a `"Too many nested INCLUDESCRIPT…"` | e4+e5+e6: −240 → **−120** |

**e5 y e6 además APORTAN dato que faltaba y que no había forma de escribir**: los
24 B de `"NULL"` + `"Stomp Detector"` son las cadenas del cuerpo de
`MemoryPool::SetFancyStompDetector` (que en nuestro árbol es un stub vacío) y los
36 B de `"Could not find command %s in %s\n"` no estaban en el árbol. Van dentro
del literal vivo, en el desplazamiento exacto del objetivo, **sin escribir una
sola instrucción**.

Los tamaños salen a la primera si se cuentan del volcado del objetivo: el literal
de C tiene que medir `tamaño_del_símbolo − 1` caracteres (el NUL final lo pone el
compilador).

---

## 2. `.data`: cerrada, y cómo

Eran 66 B repartidos en siete sitios, todos ceros o variables muertas.

**e1 — los huecos, escritos a mano con el nombre de dtk** (`genrodata.py` no vale
para `.data`; el bloque `asm()` se escribe igual que en `zAttribSys`):

| dónde va el `asm()` | símbolo | B |
|---|---|---|
| SourceList, entre `bFunkPlat.cpp` y `bList.cpp` | `gap_06_80415944_data` | 8 |
| SourceList, entre `bChunk.cpp` y `bMath.cpp` | `gap_06_80415954_data` | 4 |
| `bMath.cpp`, entre `bFastATanTable` y `bFixATanTableLow` | `gap_06_804161E8_data` | 2 |
| `bMemory.cpp`, tras `bMemoryAllocationNumber` | `gap_06_80416444_data` | 4 |
| `bMemory.cpp`, delante de `pTraceDebugText` | `gap_06_8041644C_data` | 4 |
| SourceList, detrás del último `#include` | `gap_06_80416464_data` | 36 |

Con eso el objeto quedó **2.896/2.896 byte a byte**, pero el enlace seguía en
**−32**: `-strip-unused-data` se llevaba una tirada de variables muertas.

**e2 — la parte que no es evidente.** El objetivo tiene cinco enteros seguidos en
**un solo símbolo**, `lbl_8041641C` (20 B), y `keep.lst` **ya lo nombra**.
Nosotros los emitíamos como cuatro globales de C++ que **nadie referencia**
(`bMemoryPrintEachAllocation`, `EnableCleanupBorrowedMemoryBlock`,
`BorrowMemoryBlockMinSize`, `bMemoryUseSharedStrings`: sólo aparecen en
`.rela.debug`) más dos palabras que en nuestro árbol no tienen nombre —
0x80416424 = 0 y 0x80416428 = 1, una de ellas el
`bMemoryEnableFancyStompDetector` que `bMemory.cpp:150` deja comentado.
Sustituidos por dos bloques `asm()` con los nombres de dtk
(`lbl_8041641C`, 20 B, y `lbl_80416434`, 4 B), **`data` pasó de −32 a 0**, y de
paso se salvaron los dos huecos de 4 B que ni siquiera están en `keep.lst`.

> **Regla**: si el objetivo mete N variables muertas en un símbolo de dtk y ese
> nombre está en `keep.lst`, hay que emitirlas como **un** bloque con ese nombre.
> Escritas por separado en C++ el enlazador se las lleva aunque cada una mida 4 B.

---

## 3. De dónde salen los flotantes muertos que faltan (84 B)

`output_constant_pool` de nuestro compilador —el fuente está en
`orig/prodg/NGC_GNU_SRC/NGC/gcc/varasm.c:3797`— llama a `mark_constant_pool()` y
**salta las entradas que ninguna instrucción referencia**, así que una función
`inline` no emitida NO emite su pool. La veda del encargo («exigen sentencias de
fuente») es correcta en eso, pero la causa real es otra y sí tiene arreglo:

> **`-strip-unused-data` conserva el pool de una función que el enlazador
> descarta.** Medido en nuestro propio objeto: el grupo
> `2e5be6ff / 3f000000 / 3f800000 / 00000000` que sólo referencia
> `bNormalize__FP8bVector2PC8bVector2f` —una de las 134 funciones que emitimos y
> el enlace tira— **sobrevive entero** en el DOL.

O sea: los 84 B son **el pool de funciones que el original SÍ compilaba y
nosotros tenemos como `// STRIPPED` con el cuerpo vacío**. Comprobado con las
reubicaciones del objeto extraído (contando addend, no sólo el símbolo): esas
palabras no las referencia nadie, y están intercaladas entre grupos idénticos que
sí se referencian, que es la firma de «pool de la función muerta de al lado».

Los cinco sitios, con el vecino vivo que los sitúa:

| obj | B | contenido | detrás de (vivo) |
|---|---|---|---|
| `+0x0148` | 28 | `38d1b717 2e5be6ff 3f000000 3f800000 00000000 43800000 00000000` | pool de `bASin__Ff` |
| `+0x0168` | 16 | `2e5be6ff 3f000000 3f800000 00000000` | pool de `bATan__Fff` |
| `+0x0268` | 16 | ídem | pool de `bMatrixToQuaternion` |
| `+0x02A4` | 16 | ídem | pool de `bDistBetween__FPC8bVector3T0` |
| `+0x02F4`, `+0x0321` | 16 + 8 | ídem + `3f000000` | pool de `bNormalize`/`bDistToLine` |

El grupo `5e-11 / 0.5 / 1.0 / 0.0` es la firma de la familia `bNormalize` /
`bDistBetween`; el de `+0x0148` lleva además `1e-4` y `256.0`, que es un `atan2`.
Los stubs vacíos que caben ahí están en `bMath.cpp` (`bOldATan`, `bFastATan`,
`bToPolar`, `bFixSin`, `bFixSinCos`), `bQuaternion.cpp` (`bConjugate`, `bLength`,
`bNormalize`, `bMult`) y `bVector.cpp`. **Escribir esos cuerpos no puede mover el
`.text`**: el objetivo no tiene ninguna de esas funciones, el enlazador las tira
igual que tira las 134 que ya emitimos de más.

---

## 4. Lo que queda: los 120 B, byte a byte

Objeto 2.888 B (objetivo 2.832), enlazado 2.712, base 2.832.
Estripado que queda = **176 B** = 24 (`$LC228`) + 16 (`$LC280`) + 136 (las tres
vtables sobrantes).

**Falta (144 B)**

- 4 B `00000000` en `+0x0067` (un `0.0f` muerto).
- **84 B de pool de flotantes** — §3.
- 56 B: `_vt.Q32EA9Allocator10IAllocator` va **al final** (`+0x0AD8`, detrás de
  `statetable`) y nosotros lo emitimos en `+0x0610`.

**Sobra (200 B)**

- 4 B: un `""` (`$LC57`) que el objetivo no tiene, en `+0x005C`, justo delante de
  todo el pool del compilador. Es la **primera** diferencia de la `.rodata`.
- 4 B en `+0x01E4`.
- 192 B de vtables: `_vt.Q32EA9Allocator14ICoreAllocator` (40),
  `_vt.19bFunkServerPlatform` (48) y `_vt.11bFunkServer` (48) **no existen en
  todo el juego** (`symbols.txt` no los tiene), y los 56 del
  `_vt.Q32EA9Allocator10IAllocator` mal colocado. Salen porque emitimos
  `_._11bFunkServer`, `__11bFunkServerPCc`, `_._19bFunkServerPlatform`,
  `__19bFunkServerPlatformPCci` y `__Q32EA9Allocator14ICoreAllocator` fuera de
  línea: eso les da «key method» y GCC 2.9 emite la vtable. En clase no la
  emitiría (ver `nfsmw-en-clase-es-inline`).

---

## 5. VEDA — dos cadenas que el truco de §1 NO alcanza, y son 40 B

`"SlotPool::GetAllocatedSlots"` (24 B de estripado) y `"bSharedStringPool"`
(16 B) **no se pueden pegar a nada**: en el objetivo su símbolo de dtk **empieza
por una constante de coma flotante del pool**, no por una cadena viva.

    lbl_803D107C (48 B) = 3f000000  +  4 flotantes muertos  +  "SlotPool::GetAllocatedSlots"
    lbl_803D1324 (24 B) = 3dcccccd  +  "bSharedStringPool"

El literal vivo anterior está a cientos de bytes. Las tres salidas, por orden de
coste:

1. **Dos entradas de `keep.lst`** — pero haría falta un nombre ESTABLE, y
   `$LC228`/`$LC280` se renumeran con cualquier edición del fuente. O sea que hay
   que escribir la cadena a mano en un `asm()` (nombre estable) y añadir la
   entrada. Es cambio de `config/`, paquete aparte.
2. **Escribir a mano el símbolo entero de dtk** (`lbl_803D107C`, 48 B) en un
   `asm()` colocado en la **frontera de función** correspondiente de
   `bSlotPool.cpp` / `Strings.cpp`, y redirigir el flotante vivo con
   `extern const float k asm("lbl_803D107C")` en lugar del literal. El bloque
   `asm()` de ámbito de fichero **sí** cae entre dos funciones (el pool de F1 se
   emite en `assemble_end_function`, las cadenas de F2 durante su expansión), así
   que la posición es alcanzable. No probado.
3. Dejarlos: **suelo de −40** para esta unidad.

**No probado y por tanto no vetado**: si al escribir los cuerpos de §3 el
flotante muerto de delante de la cadena pasa a existir, la tirada de símbolos
muertos cambia y habría que volver a medir el estripado antes de dar los 40 B por
bloqueados.

---

## 6. Dos trampas del entorno que cuestan una hora cada una

- **`bMath.cpp`, `bMemory.cpp` y `SpeedScript.cpp` son CRLF, y el preprocesador
  de SN NO acepta continuación de línea `\` + CRLF en un `#define`**: da
  `stray '\' in program` y luego `virtual memory exhausted` señalando una línea
  que no tiene nada que ver. **Las macros van en UNA sola línea**, por larga que
  sea.
- **`\\` se colapsa a `\` dentro de un heredoc en este shell**: los bloques
  `asm()` salieron con saltos de línea REALES dentro del literal (funciona: es la
  extensión de literales multilínea de GCC 2.9, y es lo que ya hay en
  `zAttribSys.cpp`), pero un `#define` con `\n`/`\0` sale roto. **Los guiones de
  parcheo se escriben a fichero, no por heredoc.**

## 7. Qué queda tocado en el árbol (sin commitear por mí)

    src/Speed/Indep/SourceLists/zBWare.cpp        3 bloques asm() de .data  (e1)
    src/Speed/Indep/bWare/Src/bMath.cpp           1 bloque asm() de .data   (e1)
    src/Speed/Indep/bWare/Src/bMemory.cpp         3 asm() (e1) + 2 asm() con nombre de dtk (e2)
                                                  + 2 macros de literal fundido (e3, e5)
    src/Speed/Indep/bWare/Src/SpeedScript.cpp     2 macros de literal fundido (e4, e6)

Radio de explosión **cero**: los cuatro `.cpp` los incluye **sólo** `zBWare.cpp`
(comprobado con `grep -rl`), y `config/`, `configure.py` y `splits.txt` no se han
tocado.

**Aviso**: a mitad de ronda otro agente commiteó (`ad6d1231`) y se llevó dentro
mi `zBWare.cpp`; los otros tres ficheros siguen sin commitear.

## 8. Orden para la próxima pasada, con lo que vale cada paso

1. **Cuerpos de los stubs de `bMath` / `bQuaternion` / `bVector`** (§3): +84 B de
   dato correcto y, como sobreviven al estripado, se lo lleva entero el
   `linkdelta`. **−120 → ≈−36.** Es lo más rentable y no toca el `.text`.
2. **Quitar las tres vtables sobrantes** (§4) pasando a en-clase los
   constructores/destructores de `bFunkServer`, `bFunkServerPlatform` e
   `ICoreAllocator`. No mueve el `linkdelta` (ya se estripaban) pero son 136 B de
   contenido equivocado que impiden la identidad byte a byte.
3. **El `""` de `+0x005C`** — es la primera diferencia de la `.rodata` y desplaza
   todo lo que va detrás.
4. **Los 40 B de §5**, que es donde hace falta decidir entre `keep.lst` y la
   cirugía del `extern const float`.
