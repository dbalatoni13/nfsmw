# r64 — eagl (zEagl4Anim)

`.text +4 -> .text +0`, `textorder` **163 descolocadas y 3 saltos -> 0 y 0
(ORDEN PERFECTO)**, `movidos` **18 símbolos permutados -> 3**, y el DOL de
`dolwhere` **89.627 B -> 11.309 B**, de los que **2.932 B no son de esta unidad**
(caen en la `.data` de EAXSound/SFXCTL, 0x80417820-0x804183D0, y en
`RegisterSFX`/`RegisterStates`/`Update__8EAXSoundf`: vienen de otra unidad
promocionada, no de este encargo). Dentro de zEagl4Anim quedan **8.377 B**.

Sello del `.o`, TRES compilaciones seguidas:
`989dabcc9cb1fccab39b6cdc9025b70a285a7974`.
`lcfix.py --check zEagl4Anim`: **0 pendientes** (y 0 `@lcsrc`).
No se ha promocionado: `trypromo` sigue dando DOL ROTO.

---

## 1. Los 4 B de `Initialize`: la veda de la r47/r60 era FALSA

`Initialize__Q25EAGL413DynamicLoader` medía 2.356 contra 2.352 y estaba marcada
FIRME desde la r47, con prueba de `stmt.c` y unas 200 combinaciones barridas
entre la r47 y la r60. **Ahora mide 2.352/2.352 y baja de 28 filas a 14.**

El error de las dos rondas anteriores fue el mismo: barrieron casos **añadidos**
dejando `case SHT_SYMTAB:` FIJO, y el objetivo necesita **tres cosas a la vez**.

El objetivo emite en la rama baja del árbol:

```
cmpwi  r0,3 ; beq <cuerpo SHT_STRTAB>    <- {3}, nodo de valor único
cmplwi r0,3 ; bgt FIN                    <- GT high -> hijo derecho [4,7] ACOTADO
cmplwi r0,1 ; ble FIN                    <- LE high(1) -> code_label
<CAE al cuerpo de SHT_SYMTAB>            <- emit_jump BORRADO por jump.c
```

* Ese `ble 1` **no** puede ser un `LT 2 -> default`: **medido**, GCC 2.9 no
  canonicaliza `LTU C` a `LEU C-1` en `emit_case_nodes` (con el nodo `[1,2]`
  sale `cmplwi 1 ; blt`, no `ble`). Sale de UN solo sitio: la rama de RANGO con
  `node->right != 0 && node->left == 0` (stmt.c:6083), o sea un nodo **`[0,1]`**
  (`low == TYPE_MIN`, por eso no lleva el `LT low`) **con hijo derecho `{2}`**.
* `{2}`, colgado de `[0,1]` (high==1) y de `{3}` (low==3), es
  `node_is_bounded`, así que emite un **`emit_jump(<cuerpo SHT_SYMTAB>)`
  pelado**. Ése es el `simplejump` que le faltaba a la condición
  `JUMP_LABEL(range1end) == label2` de `jump.c:1848` — la que la r47 nombró y no
  supo fabricar. Con él, jump.c invierte la raíz, permuta ALTO/BAJO y borra el
  `b` por ser salto a la etiqueta siguiente: **17 instrucciones, no 18**.
* Para que `balance_case_nodes` construya `{3} -> izq [0,1] -> der {2}`
  **con la raíz todavía en `{8}`** hacen falta además:
  * **(a)** partir `SHT_RELA` de `SHT_HASH..SHT_NOTE` con sabores DISTINTOS
    (`break` contra `continue`) para que `group_case_nodes` NO los funda: así el
    subárbol derecho de `{3}` es `{4}` + `[5,7]`, y jump.c se come luego las dos
    instrucciones muertas que ese subárbol genera;
  * **(b)** **dos unidades más de peso por encima de `{9}`** —aquí
    `SHT_SHLIB ... SHT_DYNSYM`— para que la caminata de `balance_case_nodes`
    (`i = (nodos + rangos + 1) / 2`) siga parando en `{8}`.

La forma que casa (ya aplicada en `eagl4supportdlopen.cpp`):

```c
case SHT_NULL ... SHT_PROGBITS:  break;
case SHT_SYMTAB:  ...
case SHT_STRTAB:  ...
case SHT_RELA:                   break;
case SHT_HASH ... SHT_NOTE:      continue;
case SHT_NOBITS:                 break;
case SHT_REL:     ...
case SHT_SHLIB ... SHT_DYNSYM:   break;
case SHT_LOPROC + 5 ... SHT_LOPROC + 6: break;
case SHT_LOPROC + 7 ... SHT_HIUSER:     continue;
default:                         break;
```

Semánticamente es idéntica: dentro del `for`, el `break` de un `switch` y el
`continue` llevan los dos al incremento del bucle, y los casos nuevos hacen lo
mismo que el `default`.

**Cómo se encontró, y la herramienta que queda:** un **simulador de `stmt.c`**
(`group_case_nodes` + `balance_case_nodes` + `emit_case_nodes`, unas 150 líneas)
para cribar sin compilar, y luego **373 compilaciones de un micro** con los
cflags reales (0,14 s cada una) sobre los representantes que el simulador daba
por prometedores: **48 aciertos**. El barrido CIEGO de 1.458 combinaciones de
sabores sin tocar `case SHT_SYMTAB` dio **CERO** — que es exactamente lo que les
pasó a la r47 y a la r60.

> Lección extrapolable: **el simulador de una fase de GCC vale para cribar, pero
> no para juzgar.** De las combinaciones que el simulador daba por imposibles,
> 48 casaban de verdad, porque jump.c limpia bloques muertos que el simulador no
> modela. El ciclo bueno es *simular para elegir, compilar para decidir*.

---

## 2. Los 8 B que descolocaban 163 funciones: `FnAnim::GetAttributes`

Con el `.text` ya a `+0`, `textorder` daba **163 descolocadas y 3 saltos de
delta**, y `permorden` **un solo símbolo desplazado**:
`GetAttributes__CQ29EAGL4Anim6FnAnim`, 8 B, en la posición **245 de 424** en
nuestro objeto y en la **81 de 318** en el objetivo — justo la ranura de
`FnAnim.cpp`, entre `MultMatrix` (eagl4runtimetransform.cpp) y
`FnAnimMemoryMap`. Esos 8 B corridos desplazaban 163 funciones y con ellas
**89.627 B del DOL**.

Causa: el cuerpo estaba **dentro de la clase** en `FnAnim.h`, así que GCC 2.9 la
trata como `inline` y la drena en `finish_file` con las otras cinco virtuales
vacías de `FnAnim` (que el enlace estripa). El original la tiene **fuera de la
clase**, en `FnAnim.cpp`. Aplicado: declaración en el `.h`, definición en el
`.cpp`. Resultado: **ORDEN PERFECTO, 0 descolocadas, 0 saltos de delta.**

### PROPUESTA (regla 5: es cabecera compartida)

`src/Speed/Indep/Src/EAGL4Anim/FnAnim.h` la incluyen **20 SourceLists**. El
cambio está medido y es inocuo:

* **Ninguna otra unidad emite `GetAttributes__CQ29EAGL4Anim6FnAnim`** —
  comprobado sobre los `.o` de todas las SourceLists y sobre los extraídos:
  sólo `zEagl4Anim`, en los dos lados.
* Compiladas `zCamera` y `zMisc` **antes y después**, y comparadas sección a
  sección: `.text`, `.rodata`, `.data` y `.ctors` **byte a byte IDÉNTICAS**
  (zCamera 136.020/9.872/3.136 B, zMisc 84.720/8.520/2.192 B). Lo único que
  cambia es `.debug` (-136 B) y la **renumeración del sufijo de los estáticos
  locales** (`_.tmp_0.13978` -> `_.tmp_0.13975`), porque la cabecera declara
  tres cosas menos y el contador de `DECL_UID` se desplaza.
* Ese sufijo sólo importa donde alguien lo escribe A MANO. En todo `src/Speed`
  hay **un** caso, `Geometry.cpp:589` (`algos.3467`), y su unidad —`zDynamics`—
  **no incluye `FnAnim.h`**, comprobado con un recorrido transitivo de
  `#include`. `keep.lst` no nombra ningún `_.tmp_*`.

---

## 3. Los 12 B de `.data` fuera de sitio

`movidos` daba **18 símbolos permutados** (1.228 B): `lbl_80417104` y
`lbl_8041710C` a **+1228/+1224**, y **todo el bloque de MemoryPoolManager,
`INIT_TABLE_SIZE`, `gReverseDeltaSumEnabled` y `mpFactory` a -12/-16**.

El objetivo **intercala** esos dos huecos con `hashhead`:

```
0x80417104  lbl_80417104            4 B
0x80417108  _5EAGL4.hashhead        4 B
0x8041710C  lbl_8041710C            8 B
```

Estaban en el `asm()` de cola de `zEagl4Anim.cpp`, y **un `asm()` de ámbito de
fichero no se puede intercalar entre dos globales**: GCC 2.9 drena los globales
en `finish_file`, detrás de todos los `asm()` (nota de la r60 en este mismo
árbol). Se han escrito como **objetos C++ con nombre `__asm__`**, en
`eagl4supportdlopen.cpp`, alrededor de `hashhead`:

```c
int lbl_80417104 __asm__("lbl_80417104") = 1;
static HashPointer *hashhead = 0;
int lbl_8041710C[2] __asm__("lbl_8041710C") = {0, 2};
```

Con inicializador, que si no la definición es tentativa y GCC 2.9 no emite nada
(nota «El `= {}` que roba el símbolo»). Resultado: **18 permutados -> 3**, y el
DOL de 89.627 a 11.309 B.

---

## 4. NEGATIVO MEDIDO: los tres símbolos de `.rodata`

Los tres que quedan son **una sola historia**: el bloque `asm()` de prefijo de
`.rodata` **ya contiene** los bytes de `DynamicLoader::AnimBankType`
(offset 0x2A4 = 0x803D428C) y `DynamicLoader::SkeletonType` (0x2BC =
0x803D42A4), y **además** los definimos en C++ en `eagl4supportdlopen.cpp`, que
los emite en el offset 0x488 — en medio del pool. De ahí el `+500` y el `+492`.

Se probó poner el símbolo en el prefijo y quitar la definición C++:

| intento | resultado |
|---|---|
| etiqueta con `.globl`/`.type`/`.size` dentro del bloque | compila; `linkdelta` pasa de `.rodata IGUAL` a **`.rodata -24`** y el DOL de **11.309 a 295.875 B** |
| `.set NOMBRE, lbl_803D3FE8 + 0x2A4` | `ngcas`: *Failed to evaluate* |

**Lo que ese negativo destapa, y es lo más útil de él:** con la copia duplicada,
`linkdelta` daba `.rodata IGUAL` **por compensación**, no porque estuviera bien.
Nos **faltan 24 B de `.rodata`** en alguna otra parte de la unidad, y los tapaba
la copia viva de 24 B de esas dos cadenas. Quien siga: buscar primero esos 24 B
y sólo después mover el símbolo. Está escrito junto al bloque `asm()` en
`zEagl4Anim.cpp`.

El tercero, `gRuntimeAllocType` (16 B, `-6584`), es el mismo frente: el objetivo
lo pone en 0x803D5E28, que son los **últimos 16 B de la `.rodata` de la unidad**,
y nosotros en el offset 0x488. En el ELF original es `scope:local` (o sea un
`static`) y nosotros lo escribimos `extern const char[]`.

---

## 5. Lo que queda dentro de zEagl4Anim (8.377 B)

| bloque | B | qué es |
|---|---|---|
| `.rodata` 0x803D4390..0x803D5E00 | ~6.400 | el pool `$LC` corrido por los 3 símbolos de arriba |
| `EvalSQT*` / `EvalSQTMasked*` (4 funciones) | ~700 | reubicaciones a ese pool corrido, no código |
| `EvalState__...FnRawStateChan` | 84 | 18 filas de reparto |
| `Initialize` | 57 | 14 filas de reparto / orden de operandos |

O sea: **arreglado el pool se van unos 7.100 de los 8.377 B**, y la unidad queda
a dos funciones de reparto de promocionar.

### `Initialize`, las 14 filas que quedan — dos negativos re-medidos

* **Asociatividad de `&s[nameLength + 1]`**: RE-MEDIDA sobre el árbol nuevo y
  **sigue negativa**. `&s[nameLength] + 1`, `s + nameLength + 1`, una local
  `char *tail` y `*(s + nameLength)` dan las **cuatro 16 filas** contra las 14
  de la base (2.352 B en todas).
* **Orden de operandos de `add`/`lwzx`/`stwx` indexados**: el objetivo pone
  siempre el **índice** primero (`add r30,r11,r10`, `lwzx r11,r5,r4`) y nosotros
  la **base**. Medido en micro con los cflags reales: la única forma de fuente
  que da la vuelta al `add` es hacer la cuenta **en enteros**
  (`(S*)(i*sizeof(S) + (unsigned)p)` -> `add 3,0,3`); `&p[i]`, `p + i`, `i + p`,
  `(char*)p + i*sizeof(S)` y el índice sin signo dan las cinco `add 3,3,0`. Y
  **ninguna de las seis mueve el `lwzx`**, que sale `lwzx 4,3,0` en todas. El
  orden del `lwzx` del objetivo **no sale de la forma de la indexación**.

### `EvalState` (456 B, 18 filas)

No tocada esta ronda. El diagnóstico de la r50/r62 sigue en pie y está escrito
entero junto a la función en `RawStateChan.cpp`: hay que **matar el pseudo del
desplazamiento** (`2*mNumFields+10`, vivo en todo el bucle), no pinchar el
índice. La familia C (pin + fantasma + barrera) ya se midió negativa en la r62.

---

## Ficheros tocados

* `src/Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.cpp` — el `switch`, los dos
  objetos de `.data` alrededor de `hashhead`, y las notas.
* `src/Speed/Indep/SourceLists/zEagl4Anim.cpp` — quitados `lbl_80417104` y
  `lbl_8041710C` del `asm()` de cola; nota del negativo de `.rodata`.
* `src/Speed/Indep/Src/EAGL4Anim/FnAnim.h` y `FnAnim.cpp` — **PROPUESTA**, ver
  §2: `GetAttributes` fuera de la clase.
