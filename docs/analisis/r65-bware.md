# r65 — lote `bware`: retirar andamios sin mover un byte

Encargo: dejar fuente legítima, no bytes de DOL. La prueba de cada cambio son los
**digests por sección de las secciones ALLOC** (`.text`, `.rodata`, `.data`,
`.sdata`, `.ctors` y sus `.rela.*`), ignorando `.line`, `.debug*`, `.comment` y
`.stab*`. Idénticas → acepta; cambian → revierte en el acto.

Herramientas propias de la ronda, en `scratchpad/bware65/`:

* `sello.py` — digest por sección ALLOC de un `.o` (ELF32 BE), saltándose `.line`
  y compañía.
* `try.py` — aplica una variante al fuente, compila la unidad (leyendo **la
  última línea** de `build_direct.py`), compara sellos y **restaura siempre** el
  fuente. Opcional `--dwarf=<funcion>` para volcar nuestro reparto.
* `censo.py` — cuenta andamios con un **quitador de comentarios de verdad**
  (máquina de estados `/* */`, `//`, cadenas). Sin eso, un andamio *citado* en un
  comentario de diagnóstico se cuenta como andamio: el censo ingenuo daba 23 en
  mis ficheros cuando quedaban 11.
* `fnasm.py` / `fncmp2.py` — diff de una función entre el `.s` del troceador y
  `dtk elf disasm` del nuestro, normalizando etiquetas y nombres de símbolo.

Control de determinismo pasado en las dos toolchains de mi lote antes de medir
nada: reescribir una palabra de un comentario sin cambiar el número de líneas da
`.o` con sellos ALLOC idénticos.

---

## 1. Censo: 17 → 11

Contado con `censo.py` sobre los ficheros originales (guardados) y sobre los
actuales:

| fichero | antes | después | retirados |
|---|---|---|---|
| `src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | 6 pines + 1 barrera | 4 pines + 1 barrera | **2 pines** |
| `src/Speed/Indep/bWare/Src/Strings.cpp` | 6 pines + 1 barrera | 5 pines + 1 barrera | **1 pin** |
| `src/Speed/Indep/Src/Physics/Common/Smackable.cpp` | 3 barreras | **0** | **3 barreras** |
| **total** | **12 pines + 5 barreras = 17** | **9 pines + 2 barreras = 11** | **6** |

El encargo decía 15 (6+6+3). La diferencia son **dos barreras que el censo del
encargo no vio**: `Strings.cpp:208` y `criticalpath.c:779`, las dos pegadas a sus
pines. Son andamios igual y están contadas aquí.

Además se han retirado **tres locales inventadas** que no son `asm` y por tanto no
entran en el censo, pero son deuda de fuente exactamente igual:
`_OutputPtr` (criticalpath), `c1calc` (Strings) y `collision_mask` (Smackable).

Intentos: **62 variantes distintas compiladas y medidas** (30 en criticalpath,
26 en Strings, 6 en Smackable), dos de ellas controles de determinismo.

---

## 2. Dos correcciones al encargo

**`criticalpath.c` NO la compila Metrowerks.** `build.ninja:8945` dice
`prodg`, `toolchain_version = ProDG\3.9.3`, `-O1 -gdwarf+`. Es **GCC 2.95.3**, y
el catálogo de palancas de GCC sí aplica. Quien tomara el encargo al pie de la
letra habría descartado el `-E`, los volcados RTL y el DWARF por nada.

**`previo.py` no indexa un `/* */` cuyas líneas interiores no empiecen por `//`,
`/*` o `*`.** Su `COMENT` es `^\s*(//|/\*|\*)`. El dossier viejo de criticalpath
está escrito así y previo lo lista con **"1 líneas"**: el trabajo de r36f/r47/r48/
r61/r62 está en el fichero pero *no* en el índice. Todos los bloques que he
escrito llevan `//` en cada línea, y previo los da ahora con sus 12–69 líneas
(comprobado con `previo.py IRREDUCIBLE` y `previo.py r65`).

---

## 3. La retirada grande: `Smackable.cpp`, de 3 barreras a 0

`Smackable::Smackable` la cerró r36c al 100 % con **cinco piezas acumulativas**,
cuatro de ellas `asm` de cero bytes, y un dossier de 60 líneas explicando con
volcados RTL por qué hacía falta cada una. Las cuatro se han ido, y la función
sigue byte a byte igual.

**Lo que faltaba es una palabra: `Behavior *rbbehavior = NULL;`.**

Lo dice el DWARF del original, y hay que leer dos cosas a la vez
(`symbols/mw_dwarfdump.nothpp:1624135` y siguientes):

* el bloque anónimo del `else` del original tiene **exactamente dos locales**,
  `rbbehavior // r11` y `rbparams // r1+0x60`. **No tiene `collision_mask`.**
* el `li r11,0` del objetivo es **la inicialización de `rbbehavior`**, y `cse`
  reutiliza ese mismo cero para el último argumento de `RBComplexParams`.

O sea: la sustitución de `cse` que r36c se pasó tres piezas intentando cortar
**no era el error, era lo que hace el original**. Sólo cogía el registro
equivocado porque el registro correcto —el cero de `rbbehavior`— no existía en
nuestra fuente. Y la local `collision_mask` que se había metido para darle un
cero al argumento le robaba r11 a `rbbehavior`, lo que obligaba a la quinta pieza
(`__asm__("" : : "r"(rbbehavior))`) para devolvérselo. Andamio circular.

Retirado, todo junto y en una sola compilación idéntica:

```
-        UCrc32 smack_class;
-        __asm__("" : : "m"(smack_class));          (pieza 2)
-        __asm__("" : "+r"(simple_physics));        (pieza 3)
-        unsigned int collision_mask = 0;           (pieza 4, local inventada)
-        __asm__("" : : "r"(rbbehavior));           (pieza 5)
+        Behavior *rbbehavior = NULL;
```

(se queda la pieza 1, `if (simple_physics)` pelado, que es fuente normal, y
`UCrc32 smack_class;` sin usar, que **el original también tiene**, en r1+0x58).

Sellos ALLOC de `zPhysics.o`, antes y después:

| sección | antes | después |
|---|---|---|
| `.bss` | 19680 `da39a3ee5e6b4b0d` | igual |
| `.ctors` | 4 `9069ca78e7450a28` | igual |
| `.data` | 444 `531ecb3823dd6673` | igual |
| `.rela.ctors` | 12 `47254f9a46a9abd4` | igual |
| `.rela.data` | 84 `45e1276912ad9167` | igual |
| `.rela.rodata` | 14724 `8ce44b64725a4931` | igual |
| `.rela.text` | 100500 `69efcb723ca12263` | igual |
| `.rodata` | 16712 `0aeb2c34d027cab3` | igual |
| `.text` | 158256 `081fed3288811a02` | igual |

Y la comprobación que convierte esto en un método y no en una casualidad:
**nuestro DWARF pasa a coincidir local a local con el del original**. Antes:
`collision_mask // r11`, `rbbehavior // r3`. Después: `rbbehavior // r11`,
`rbparams // r1+0x60`, sin sobras, y `smack_class // r1+0x58` **sin el `asm` de
memoria**. El `"m"` sólo estaba forzando a mano la ranura de pila que el reparto
bueno da solo.

Camino, con cifras (`.text` de `zPhysics.o`, base 158256 `081fed3288811a02`):

| variante | resultado |
|---|---|
| sin `collision_mask`, sin pieza 5 | 158252 `b14cacc648aeb037` |
| con `collision_mask`, sin pieza 5 | 158252 `de37aaa6d36a96a3` |
| sin `collision_mask`, con pieza 5 | 158256 `70f18370582432d5` — **tamaño exacto, 3 insn cruzadas** |
| `rbbehavior = NULL`, sin `collision_mask` ni pieza 5 | **IDÉNTICO** |
| … y además sin la pieza 3 | **IDÉNTICO** |
| … y además sin la pieza 2 | **IDÉNTICO** |

La tercera fila es la que enseña el camino: con `fncmp2.py` quedaban **3
instrucciones** (`li r11,0` y `addi r9,r1,0x28` cruzadas con
`lwz r0,0x11c(r31)`), y ese `li r11,0` suelto es literalmente la inicialización
que faltaba. **Diagnosticar en instrucciones, no en porcentaje, fue lo que lo
destapó.**

---

## 4. `criticalpath.c`: 2 pines retirados, 4 + 1 barrera diagnosticados

Unidad cerrada al 100 % en r63 con 11 andamios dentro (hoy 7 por el censo
limpio). Sellos de `criticalpath.o` antes y después de las dos retiradas:

| sección | antes | después |
|---|---|---|
| `.bss` | 144 `da39a3ee5e6b4b0d` | igual |
| `.data` | 224 `6404fc0c15e04615` | igual |
| `.rela.text` | 4812 `3dc4bee14a4695b7` | igual |
| `.rodata` | 636 `6c710e23d0bd20cd` | igual |
| `.sdata` | 8 `410adbae24167467` | igual |
| `.text` | 12040 `dc8f68814e4a7b27` | igual |

### Retirada 1 — `OutputPtr` es el parámetro, no una copia pinchada

```
-void VP6_PredictFilteredBlock(struct PB_INSTANCE *pbi, short *_OutputPtr, BLOCK_POSITION _bp) {
-    register short *OutputPtr asm("r24") = _OutputPtr;
+void VP6_PredictFilteredBlock(struct PB_INSTANCE *pbi, short *OutputPtr, BLOCK_POSITION _bp) {
```

El DWARF del original da `OutputPtr` como **parámetro**, en r24, sin copia local.
Se ha ido el pin **y el nombre inventado `_OutputPtr`**.

### Retirada 2 — `Stride` no necesita pin: basta uno de los dos

```
-    register unsigned int Stride asm("r26");
+    unsigned int Stride;
```

### Los cuatro pines y la barrera que quedan, con su diagnóstico

Escritos en el comentario pegado al andamio (previo.py los encuentra).

1. **`bp asm("r23")` — PERMUTACIÓN r23↔r24 de los dos parámetros.** Con los dos
   como parámetros de verdad GCC reparte **exactamente al revés** (OutputPtr r23,
   bp r24, leído con `dtk dwarf dump`): ciclo cerrado de dos. Las tres formas
   sin pin dan el mismo `.text` `6c03acf3f7e90991`, incluida la copia local sin
   `asm` — GCC 2.9 la funde y da el mismo objeto que no ponerla. El pin es la
   única herramienta.
2. **`TempPtr2 asm("r27")` — PERMUTACIÓN r26↔r27 con `Stride`, y basta un pin.**
   El DWARF original dice TempPtr2 r27 (compartido con SrcPtr) y Stride r26; sin
   pines GCC los cruza (TempPtr2 r26, Stride r27) y **todo lo demás sale igual**.
   Pinchar cualquiera de los dos deja el `.o` idéntico; por eso el de Stride se
   fue. Cinco formas sin pines dan **el mismo** `.text` `63b5a8ded88fa1ea`:
   tal cual, `TempPtr1 = 0; TempPtr2 = 0;` en dos sentencias, `TempPtr2 = 34;`
   antes en el `else`, `Stride = …` delante de la sentencia de TempBuffer, y las
   dos declaraciones al revés. GCC canonicaliza las cuatro reordenaciones.
3. **`finalIndex asm("r27")` + `finalOffset asm("r0")` + la barrera — un solo
   andamio, PERMUTACIÓN r26↔r27 de *todo* `VP6_DecodeBlock`.** Quitar **sólo la
   barrera** da exactamente el mismo `.o` que quitar los tres
   (`8d7aff93ada2d88b`): los pines solos son inertes, porque sin la barrera la
   copia muere y el pin se va con ella. Sin ellos, `fncmp` da 28 insn distintas y
   **las 28 son r26↔r27** (`7C1A..` contra `7C1B..`). **No hay oráculo:**
   `mw_dwarfdump` no trae `VP6_DecodeBlock` (sólo `VP6_DecodeBlockMode` y
   `VP6_ReadTokensPredictA`, que es de otra TU), así que `regmap` no puede decir
   qué local sobra o falta. Ocho formas medidas y negativas, entre ellas
   `EobOffsetTable[--i]` (`424dd8ede3c2b06f`) y `register int i asm("r27")` en
   las declaraciones de `VP6_ReadTokensPredictB`, que **pierde una instrucción**
   (12036 B).

---

## 5. `Strings.cpp`: 1 pin retirado, 5 + 1 barrera diagnosticados

Sellos de `zBWare.o` antes y después:

| sección | antes | después |
|---|---|---|
| `.bss` | 12756 `da39a3ee5e6b4b0d` | igual |
| `.ctors` | 4 `9069ca78e7450a28` | igual |
| `.data` | 2896 `cac6dd49bd3dffec` | igual |
| `.rela.ctors` | 12 `c98c0eae009f1e60` | igual |
| `.rela.data` | 72 `d3788ec93b339851` | igual |
| `.rela.rodata` | 216 `993e0c479bbd0687` | igual |
| `.rela.text` | 25212 `fe6a4c2b775bc621` | igual |
| `.rodata` | 2928 `dff26c83dc7cdf1f` | igual |
| `.text` | 38580 `4f4119743db1d9fd` | igual |

### Retirada — el cuarto pin de `bStrNICmp` era una copia inventada

```
-                register char c1calc asm("r3") = bToUpper(s1[-1]);
-                register char c1 asm("r11") = c1calc;
+                register char c1 asm("r11") = bToUpper(s1[-1]);
```

### Dos comentarios `// UNSOLVED` rancios, retirados

`bStrNCmp` y `bStrNICmp` **casan al 100 %**, comprobado con `dtk elf disasm`
contra `zBWare.s` (las únicas diferencias son cómo se renderizan los destinos de
salto). El marcador venía de antes de que se pusieran los pines y mandaba a
cualquiera a re-atacar una función terminada.

### Diagnóstico de lo que queda: TEMPORALES, sin oráculo

El DWARF del original da `int bStrNCmp(s1 r3, s2 r4, n r5)` y
`int bStrNICmp(s1 r3, s2 r4, n r5)` **sin ni una local** (bStrNICmp sólo con
cuatro expansiones inline de `bToUpper`). `c1`, `c2`, `c1wide` no existen en el
original: son temporales del compilador, y por eso `regmap` —que compara
locales— no puede decir nada aquí. Es el cubo "IDÉNTICO / difícil".

* **`bStrNCmp`, dos pines.** El objetivo hace `lbz r9,-1(r3); lbz r0,-1(r4);
  extsb r9,r9; extsb r0,r0; subf r3,r0,r9`. Sin los pines GCC da **la misma
  forma** con r0/r3: reutiliza r3 —el registro de retorno— en cuanto el puntero
  muere. Las **cinco** formas llanas dan el mismo `.o` (`e23d641f18b74e22`):
  expresión directa, con `(int)`, con `*(s1-1)`, con locales `char`, con locales
  `int`. Y ninguna combinación de menos de dos pines vale; el dato más útil es
  que **declarar `c2` antes que `c1` sin ningún pin da el mismo objeto que
  pinchar `c2` a r0** — el orden de declaración ya le da r0 a c2, pero c1 sigue
  sin coger r9.
* **`bStrNICmp`, tres pines y la barrera.** Lo que hay que reproducir es que el
  objetivo mete **las dos** expansiones de la cola en r3 y **paga un
  `mr r11,r3`** para salvar la primera; GCC sin andamios mete la segunda en r4
  (el puntero `s2`, ya muerto), se ahorra el `mr` y la función sale 4 B más
  corta. Cada retirada suelta de los tres pines o de la barrera cambia el
  objeto: son un conjunto.

---

## 6. Veredicto

**FUNCIONA, parcial en cifra y total en método.** 6 andamios de 17 retirados
(35 %), más 3 locales inventadas, con las secciones ALLOC idénticas en las tres
unidades y cero regresiones. Un fichero completo (`Smackable.cpp`) a **cero
andamios**.

Los 11 que quedan están diagnosticados uno a uno junto al andamio, con el
veredicto nombrado y los digests de cada negativo, y son de los dos tipos que el
encargo da por caros: **PERMUTACIÓN** (tres ciclos cerrados de dos registros,
tres de ellos confirmados contra el DWARF del original) y **temporales del
compilador sin oráculo** (las dos de `Strings.cpp`, donde el original no tiene
ni una local que comparar). Ninguno es ESTRUCTURA: la única deuda estructural del
lote era la de `Smackable`, y ha caído.

Lo transferible, por si vale para otros lotes:

1. **La deuda de fuente estaba en una inicialización que faltaba, no en el
   `asm`.** Tres barreras y una local existían para cortar una optimización que
   el original **sí hace**; le faltaba el valor del que el original la alimenta.
2. **Leer el DWARF del original buscando lo que NO tiene.** `collision_mask`,
   `_OutputPtr`, `c1calc` y las locales de `bStrNCmp` se cayeron mirando la lista
   de locales del original y contando.
3. **Medir en instrucciones.** La variante "tamaño exacto, 3 insn cruzadas" es la
   que enseña la respuesta; en fuzzy o en bytes habría parecido un fracaso igual
   que las otras.
4. **El pin que sobra se encuentra quitándolos de uno en uno y por parejas.** En
   criticalpath, tres de cuatro se pueden quitar solos, pero `Stride`+`TempPtr2`
   juntos no: el máximo retirable era dos.
