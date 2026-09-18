# r65 — lote `vfprintf`: retirar andamios de `src/libc`

Ficheros: `src/libc/vfprintf.c` (+ su segunda compilación `vfprintf_1.c`),
`src/libc/itoa.c`, `src/libc/fseek.c`.

**Sin commit.** Nada más tocado: ni `configure.py`, ni `config/GOWE69/*`, ni
`splits.txt`, ni `lcfix.py`, ni cabeceras compartidas.

---

## 1. El test, y por qué no se puede perder nada

Cada cambio se mide con los **digests SHA-1 por sección de las secciones ALLOC**
(`.text`, `.rodata`, `.data`, `.sdata`, `.rela.*` de esas, más el tamaño de
`.bss`), **ignorando `.line`, `.debug*`, `.comment` y `.stab*`**. Criterio:
idénticas → acepta; cambian → revierte en el acto.

Herramientas propias en `scratchpad/vfprintf65/`:

| script | qué hace |
|---|---|
| `secdig.py` | parsea el ELF y saca el digest de cada sección ALLOC |
| `try_.py` | aplica/revierte una variante, compila y compara con la línea base |
| `test.py` | verificación de las tres unidades (`--save` fija la línea base) |
| `censo.py` | censo limpio de pines y barreras |
| `sweep.py` | quita **un** andamio cada vez y mide (barrido) |
| `greedy.py` | acumulación voraz: añade retiradas mientras siga idéntico |
| `combos.py` | combinaciones dirigidas (pin + su barrera, familias completas) |

Detalle crítico: **`vfprintf.c` se compila DOS VECES** (`vfprintf.o` con coma
flotante y `vfprintf_1.o` con `INTEGER_ONLY`). Toda medida de este informe
comprueba **los dos objetos**; medir solo uno habría dado falsos aciertos en los
pines que viven dentro de `#ifdef FLOATING_POINT` / `#else`.

`build_direct.py` devolvió `0 fallidas` en las 340 compilaciones del lote; el
harness aborta si la última línea no lo dice.

### Línea base (idéntica antes y después de todo el trabajo)

| unidad | `.text` | `.rodata` | `.sdata` | `.data` | `.bss` | `.rela.text` | `.rela.sdata` |
|---|---|---|---|---|---|---|---|
| `itoa.o` | `b86f14a700bf`:1548 | `6fe04e6456a5`:64 | — | `da39a3ee5e6b`:0 | 164 | `f15b75b6eeb8`:1524 | — |
| `fseek.o` | `d08be34bb0c1`:996 | `da39a3ee5e6b`:0 | — | `da39a3ee5e6b`:0 | 0 | `581c33bba662`:744 | — |
| `vfprintf.o` | `c3ce1638243b`:6948 | `7bf8feeb9f1e`:144 | `05fe40575316`:8 | `da39a3ee5e6b`:0 | 128 | `3bf09023f266`:6072 | `a2f8f42abdda`:12 |
| `vfprintf_1.o` | `f48c48979805`:5972 | `da39a3ee5e6b`:0 | — | `da39a3ee5e6b`:0 | 0 | `1fd2b5fccbc7`:5148 | — |

Reconstrucción limpia final (los cuatro `.o` borrados y recompilados): **los 24
digests, idénticos**. `fncmp.py` sigue dando `0 de 4`, `0 de 1` y `0 de 4`
funciones con el código distinto.

---

## 2. Cifras

Censo estricto (pines `register T x asm("rN")` + barreras de plantilla vacía,
contando las dos ramas de cada `#if`):

| fichero | antes | después | retirados |
|---|---|---|---|
| `vfprintf.c` | 58 (29 pines + 29 barreras) | 34 (19 + 15) | **24** |
| `itoa.c` | 3 (1 + 2) | 3 | 0 |
| `fseek.c` | 3 (3 + 0) | **0** | **3** |
| **total** | **64** | **37** | **27** |

Contando solo las ramas activas de cada compilación (que es como el encargo
llegó a 33): 59 → 32, **27 retirados**, los mismos.

**Veredicto: FUNCIONA — 27 de 64 fuera, con los 24 digests ALLOC intactos.** De
los 37 que quedan, los 37 están diagnosticados y el diagnóstico está escrito en
el comentario pegado al andamio, donde `previo.py` lo encuentra.

---

## 3. Las retiradas, una por una

### 3.1 `fseek.c` — los tres, y con la forma de newlib (ESTRUCTURA)

`fseek.c` es newlib puro y el fuente original es público. Nuestra versión había
inventado **dos locales que newlib no tiene**, `blockOffset` y `blockMask`, y
las había clavado en r29 y r0; `ptr` estaba clavado en r30.

```c
-    register struct _reent *ptr __asm__("r30");
+    struct _reent *ptr;
     fpos_t(*seekfn)(void *, fpos_t, int);
     fpos_t target, curoff;
     size_t n;
-    register size_t blockOffset __asm__("r29");
-    register size_t blockMask __asm__("r0");
...
-    blockMask = ~(fp->_blksize - 1);
-    blockOffset = target & blockMask;
-    if ((*seekfn)(fp->_cookie, (fpos_t)blockOffset, SEEK_SET) == POS_ERR)
+    curoff = target & ~(fp->_blksize - 1);
+    if ((*seekfn)(fp->_cookie, curoff, SEEK_SET) == POS_ERR)
         goto dumb;
     fp->_r = 0;
     fp->_flags &= ~__SEOF;
-    n = target - blockOffset;
+    n = target - curoff;
```

newlib reutiliza `curoff`, que ya estaba declarada, para el offset alineado a
bloque. Con eso los tres andamios se caen de golpe.

`fseek.o` — `.text d08be34bb0c1`:996, `.rodata da39a3ee5e6b`:0,
`.data da39a3ee5e6b`:0, `.bss` 0 B, `.rela.text 581c33bba662`:744
→ **idénticos antes y después**.

Este es el caso de libro del veredicto ESTRUCTURA: sobraban dos locales y los
registros se recolocaron solos.

### 3.2 `vfprintf.c` — el `%.*` (ESTRUCTURA, 5 andamios de un golpe)

Nuestra versión escribía la lectura del argumento de precisión con dos bloques
anidados, dos pines (`precision_arg` en r3, `parsed_prec` en r30) y tres
barreras:

```c
-                {
-                    register int *precision_arg asm("r3") = &va_arg(ap, int);
-                    asm("" : "+r"(precision_arg));
-                    n = *precision_arg;
-                }
-                {
-                    register int parsed_prec asm("r30") = n;
-                    asm("" : "+r"(parsed_prec));
-                    prec = parsed_prec;
-                }
-                asm("" : "+r"(prec));
-                if (prec < -1)
-                    prec = -1;
+                n = va_arg(ap, int);
+                prec = n < 0 ? -1 : n;
```

La forma de la derecha es **literalmente la de newlib**
(`n = va_arg(ap, int); prec = n < 0 ? -1 : n;`). Aviso medido: la forma tiene
que ser esa. `prec = va_arg(ap, int); if (prec < -1) prec = -1;` **no vale**
(pierde 4 B y mueve `.rela.text`), y la variante intermedia
`n = va_arg(...); prec = n; if (prec < -1) prec = -1;` sí vale, pero el `?:` es
la de newlib.

`vfprintf.o` `.text c3ce1638243b`:6948 y `vfprintf_1.o` `.text f48c48979805`:5972
→ **idénticos**, igual que sus `.rela.text` (`3bf09023f266` / `1fd2b5fccbc7`) y
`.rodata`/`.sdata`/`.bss`.

Detalle del método: esta retirada **no salió del barrido de uno en uno** —
cada una de las cinco líneas, sola, movía `.text`. Salió de `combos.py`
probando la familia entera como unidad. Es la lección táctica de la tanda: los
andamios vienen en **racimos**, y un racimo solo se retira completo.

### 3.3 `vfprintf.c` — el bloque de emisión (2 locales falsas fuera)

```c
-                register int emitted_count;
-                register unsigned char *emitted_p asm("r0");
-                register int emitted_w;
                 memcpy(fp->_p, cp, size);
-                emitted_count = size;
-                emitted_p = fp->_p;
-                emitted_w = fp->_w;
-                emitted_p += emitted_count;
-                fp->_p = emitted_p;
-                fp->_w = emitted_w - emitted_count;
+                register unsigned char *emitted_p asm("r0");
+                memcpy(fp->_p, cp, size);
+                emitted_p = fp->_p + size;
+                fp->_p = emitted_p;
+                fp->_w -= size;
```

`emitted_count` y `emitted_w` eran andamio puro (uno de los dos, `emitted_w`,
estaba además clavado en r9). Queda **un** pin, el de r0, que sí es
irreducible (§4.3). La forma es ya la de newlib salvo ese pin.

### 3.4 `vfprintf.c` — `format_byte` sin disfraz

```c
-            register unsigned char format_byte asm("r0") = *fmt++;
-            ch = (signed char)format_byte;
+            register char format_byte asm("r0") = *fmt++;
+            ch = format_byte;
```

El pin sigue (§4.2), pero el `unsigned char` + cast a `signed char` era ruido:
**medido, `char` es con signo en ngccc**, así que `*fmt++`, `(signed char)*fmt++`
y el rodeo por `unsigned char` dan el mismo objeto. Queda en las dos copias del
bucle de formato.

### 3.5 `vfprintf.c` — las 17 que salieron del barrido voraz

`sweep.py` encontró 22 andamios que, retirados **solos**, dejaban los cuatro
objetos idénticos; `greedy.py` los acumuló y 17 aguantaron juntos. Otras dos
se abrieron al re-barrer sobre el árbol nuevo (**los andamios caducan**: un
andamio medido como imprescindible deja de serlo cuando cambia lo que tiene
alrededor). Lista, con lo que eran:

| qué era | forma que lo sustituyó |
|---|---|
| `register int prec asm("r25")` | `register int prec;` |
| `register int parsed_prec asm("r30")` | (desapareció en §3.2) |
| `register int float_format asm("r0")` | `register int float_format` (la barrera se queda) |
| `register unsigned char *prefix_p asm("r11")` | `register unsigned char *prefix_p;` |
| `register int prefix_w asm("r9")` | `register int prefix_w;` |
| `register unsigned int prefix_flags asm("r9")` | `register unsigned int prefix_flags` |
| `register int emitted_count asm("r11")` | (desapareció en §3.3) |
| `register int emitted_w asm("r9")` | (desapareció en §3.3) |
| `register int count asm("r12")` | `register int count = 0;` |
| 8 barreras `asm("" : "+r"(...))` | borradas |

Los 24 digests ALLOC, idénticos en cada paso (el voraz mide uno a uno).

Y un arreglo de corrección que salió de aquí: al borrar la barrera que seguía a
`prefix_finished:` quedó una etiqueta pegada a `}`, que no es C válido; ahora es
`prefix_finished:;`. Objeto idéntico.

---

## 4. Los irreducibles, con diagnóstico

Están los 37 comentados en el fuente con la marca `IRREDUCIBLE (r65)` y las
palabras que `previo.py` indexa (`MEDIDO`, `NEGATIVO`). Lo que sigue es el
resumen; el detalle está junto al andamio.

### 4.1 El bloque de declaraciones de `_VFPRINTF_R` — 9 pines, 13 líneas

`fp`, `ap`, `fmt`, `ch`, `flags`, `ret`, `width`, `dprec`, `number_end`
(r28/r29, r29/r28, r22, r26/r27, r24, r14, r17, r15, r25).

**Un solo problema: el reparto global.** Medido con el volcado `.greg` de GCC
2.95.3 (`scripts/rtldump.py -dg`) y con el ASM del objetivo:

- el original tiene **18 preservados vivos a la vez** — r14 `ret`, r15 `dprec`,
  r16, r17 `width`, r18..r21, r22 `fmt`, r23 `realsz`, r24 `flags`, r25
  `prec`/`number_end`, r26 `ch`, r27, r28 `fp`, r29 `ap`, r30, r31 — y además
  **guarda CR** (`mfcr r12` / `stw r12`) porque usa `cr4`;
- quitar **un solo** pin descoloca la cadena entera. El caso extremo es
  `number_end`: **127 instrucciones distintas**, el marco encoge 0x10 B,
  desaparece el guardado de CR (`cmpwi cr4, r26, 0x67` pasa a
  `cmpwi`+`mfcr r30`+`mtcrf`) y `prec` se muda de r25 a r27. Los demás dan
  entre 1 y 12 instrucciones movidas, siempre permutación de preservados.
- `number_end` además **no es una local inventada**: se pudo sustituir por
  `buf + BUF` en la llamada a `add_separators` (es lo que vale siempre), y el
  objeto sale igual de mal. No es estructura, es reparto.

**NEGATIVO r65 — el orden de declaración NO es palanca.** Medido aparte en
`add_separators` (§4.5) con seis órdenes distintos, con y sin `register`, con y
sin inicializador: **todos dan exactamente el mismo objeto**. Lo que decide es
`allocno_compare` de `global.c`, que ordena por prioridad
(`n_refs * freq / live_length`), no por la posición en la fuente. Cualquier
ronda que vuelva por ahí pierde el tiempo.

La única salida real: encontrar **qué pseudo le sobra o le falta** a nuestra
versión respecto de la original. El `.greg` los cuenta y basta ±1 para mover
todo el reparto (§4.6 lo demuestra en `fftoa` con números).

### 4.2 `format_byte` en r0 — 2 pines

`r0` es el **último** registro de `REG_ALLOC_ORDER` de rs6000
(`9,10,8,7,6,5,4,3,31..13,12,11,2,1,0`): GCC solo llega a él cuando todo lo
demás conflictúa. Cuatro formas medidas (`ch = *fmt++`,
`ch = (signed char)*fmt++`, `unsigned char` sin pin, `char` sin pin) dan el
mismo objeto equivocado. Cuelga de §4.1.

### 4.3 `emitted_p` en r0 y `field_count` en r0 — 1 pin + 1 pin + 1 barrera

Mismo r0, misma causa. En `field_count` hay un matiz medido que conviene no
volver a descubrir: **pin y barrera son una unidad**. Sin la barrera GCC pliega
la copia y el pin no llega a nada; sin el pin GCC coge **r15** (`dprec`, muerto
ahí) en vez de r0. La forma natural
`ret += width > realsz ? width : realsz;` genera exactamente las cuatro
instrucciones del objetivo (`mr`, `cmpw`, `mr`, `add`) pero con r15.

### 4.4 El bloque de prefijo — 4 barreras (y el `goto` cruzado)

El `goto prefix_store` / `goto prefix_advance` que salta **dentro de la otra
rama** del `if` no es adorno: es lo único que reproduce el empalme de colas del
original. **NEGATIVO r65**: la forma limpia de newlib

```c
if (sign) { PRINT(&sign, 1); }
else if (flags & HEXPREFIX) { ox[0] = '0'; ox[1] = ch; PRINT(ox, 2); }
```

compila **16 B de más** porque GCC 2.95 no cruza esas dos colas aquí: emite
`stw r11,0(r28)` / `stw r9,8(r28)` y `add r0,r0,r9` / `stw r0,0(r28)`
duplicados en vez de saltar al punto común. Cuatro variantes probadas (con y sin
barrera sobre `ch`, leyendo `fp->_flags` antes o no): los mismos 16 B. Con la
forma actual la diferencia son 13 instrucciones, de las cuales 9 son solo
registro.

Dato útil que salió de ahí: en el objetivo, `ch` (r26) **sigue vivo** después de
`ox[1] = ch` — el original hace `clrlwi r0,r26,24` y no `clrlwi r26,r26,24`. No
encontré qué lo mantiene vivo; es la misma incógnita de §4.1.

### 4.5 `add_separators` — 2 pines + 2 barreras: PERMUTACIÓN

| local | original | nuestro sin pines |
|---|---|---|
| `state` | r7 | r9 |
| `mark` | r8 | r3 |
| `count` | r12 | r6 |
| `p` | r6 | r8 |

Mismas instrucciones, cuatro registros permutados, más un `li` adelantado.
`count` en **r12** (vigésimo octavo de `REG_ALLOC_ORDER`) es la firma de una
prioridad de allocno muy baja que no sabemos reproducir.

**NEGATIVO r65 — seis formas de declaración medidas**: `mark/state/count` en
tres órdenes, con y sin `register`, con `int state = 0` en vez de asignación, y
con `mark = end` como sentencia suelta fuera del `for`. **Las seis dan el mismo
objeto equivocado** (`.text 708d10e3dab3`). Es la medida que sostiene la
afirmación general de §4.1: en esta versión de GCC el orden de la fuente no
llega al asignador.

### 4.6 `fftoa` en `itoa.c` — 1 pin + 2 barreras, y la cuenta de pseudos

`lo` clavado en r11, con una barrera delante y otra detrás de
`u.d = value; lo = u.w.lo;`.

El objetivo pone `lo` en r11 y `neg` en r10; sin el pin nos salen `lo` en r8 y
`neg` en r11. De 316 instrucciones **solo 7 son distintas**: dos parejas que el
planificador intercambia y tres registros. r11 es el antepenúltimo de
`REG_ALLOC_ORDER`.

La medida que cierra el diagnóstico: **el volcado `.greg` dice 76 pseudos a
repartir con el pin y 77 sin él**. Al original le sobra exactamente uno
respecto de cualquier forma en C que sepamos escribir. Y eso es lo que hay que
buscar, no una reordenación.

**NEGATIVOS r65 — once formas de fuente, todas con el mismo objeto equivocado**
(`.text f1652ed47eea`): `lo` declarado el primero, el último, en medio, en línea
propia, como `int`, como `unsigned long`; `lo = u.w.lo` antes de `mant`,
después, tras `t`, al final del bloque; `hi` y `lo` juntos; `neg` y `expo` en el
otro orden. Las dos formas que **sí** cambian el reparto lo empeoran: sin la
variable `lo` (usando `u.w.lo` dentro del `if`) la función encoge 4 B, y leyendo
por puntero (`((unsigned int *)&u)[1]`) encoge 8 B y mueve `.rela.text`.

Las barreras tampoco son negociables: quitar la primera, la segunda o las dos da
**tres objetos distintos** y ninguno es el bueno.

### 4.7 Los cuatro sueltos del bucle de formato y las tres barreras de macro

- `asm volatile("" : : : "r27"/"r26", "r25", "r24", "r17", "r15")` (una por
  compilación): sin el clobber GCC adelanta la recarga de los preservados del
  parser por encima de la expansión de `PRINT`.
- `asm("" : "+r"(width))`: sin ella GCC funde `width = 0` con el `sign = width`
  de dos líneas más abajo y pierde el `li` de r17.
- `asm volatile("" : : "m"(sign) : "r24")`: `sign` vive en pila y el original lo
  escribe **antes** de tocar r24 (`flags`); sin el clobber el orden se invierte.
- `asm("" : "+r"(float_format))`: el **pin** a r0 sí se retiró; la barrera no.
  Sin ella GCC pliega la copia y la función pierde 4 B, y la forma directa
  `ch = ch == 'g' ? 'e' : 'E';` con barrera sobre `ch` pierde los mismos 4 B.
- Las tres barreras dentro de macros (`PRINT`, `PAD_KEEP_blanks`,
  `PAD_KEEP_zeroes`): sostienen las direcciones de los dos arrays de relleno y
  los preservados a través de cada expansión. Vaciadas por separado o las dos de
  `PAD` a la vez: `.text` se mueve en las dos compilaciones.

---

## 5. Lo que esta tanda deja como método

1. **`build_direct.py` con nombre suelto vale para las unidades que no son
   SourceLists**: `itoa`, `fseek`, `vfprintf` funcionan, y `vfprintf` arrastra
   `vfprintf_1`. Pero hay que pedir las dos y comparar las dos: una fuente puede
   producir **varios** objetos, y medir solo uno da falsos aciertos.
2. **Barrido de uno en uno primero, racimos después.** El barrido dio 17
   retiradas gratis (andamios caducados de rondas anteriores). El racimo del
   `%.*` —5 andamios que, uno a uno, todos fallaban— solo sale probando la
   familia completa.
3. **Re-barrer después de cada retirada aceptada.** Dos andamios se volvieron
   retirables solo después de que cambiara lo que tenían al lado.
4. **El orden de declaración no es una palanca en GCC 2.95.** Medido dos veces
   e independientemente (once formas en `fftoa`, seis en `add_separators`):
   todas colapsan al mismo objeto. Esto cierra una veta entera y debería ahorrar
   rondas.
5. **`char` es con signo en ngccc.** `*fmt++`, `(signed char)*fmt++` y el rodeo
   por `unsigned char` dan el mismo objeto: los casts de ese tipo en el árbol
   son ruido, no forma.
6. **El `.greg` cuenta pseudos, y ese número es el diagnóstico.** 76 con pin
   contra 77 sin pin en `fftoa` convierte "no sé por qué no sale" en "a nuestra
   versión le sobra un pseudo". Es la pista concreta que un agente siguiente
   puede perseguir.

---

## 6. Nota al margen

`git status` muestra también `src/libc/memset.c` modificado y `src/libc/_v.c`
sin seguimiento. **No son míos**: no toqué ninguno de los dos. Los dejo como
estaban.
