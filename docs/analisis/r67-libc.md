# r67 — lote `libc`: `vfprintf.c` e `itoa.c`

**Sin commit.** Solo he tocado `src/libc/vfprintf.c` y `src/libc/itoa.c`. No he tocado `configure.py`, nada de `config/GOWE69/*`, ninguna cabecera compartida ni `lcfix.py`, y no he re-extraído. Las herramientas están en `scratchpad/libc67/`; los volcados, objetos y fuentes de variantes están borrados.

## 0. Veredicto

**FUNCIONA a medias: 7 andamios retirados de 20. Las secciones ALLOC salen idénticas en los tres objetos y el DOL, reenlazado desde el edge, es la referencia.**

| fichero | antes (encargo) | después | retirados | censo estricto antes → después |
|---|---|---|---|---|
| `itoa.c` | 1 pin + 2 barreras | **0** | **3** | 1p+2b → 0p+0b |
| `vfprintf.c` | 17 pines | 13 pines | **4** | 19p+15b → 15p+15b |
| **total** | **20** | **13** | **7** | 37 → 30 |

El censo estricto (`scratchpad/libc67/censo67.py`) cuenta las dos ramas de `#if` y los dos pines de `add_separators`. Ignora los comentarios.

- **`itoa.c` queda limpio** con la macro `EXTRACT_WORDS` de fdlibm (§3). Fuera el pin y las dos barreras.
- **`vfprintf.c`** pierde cuatro pines:
  - los dos de `fp`, uno por rama;
  - el de `ap` en INTEGER_ONLY;
  - el de `emitted_p`, que ahora es la forma de newlib `fp->_p += size`.

  La barrera de PRINT pierde además sus tres operandos `"r"(ret)`, `"r"(fmt)` y `"r"(fp)`.
- **La palanca (b), el orden de declaración de newlib, es NEGATIVA con medida** (§2): no sobra ningún pin.
- **Lo que sí sale de la ronda es el mecanismo real** (§4): la profundidad de bucle que añade el `do { } while (0)` de las macros. Con PRINT y PAD escritos con llaves, como en newlib, sobran otros cuatro pines, pero queda una permutación de 12 a 20 filas. No se acepta y queda documentada para la ronda siguiente.

## 1. El test

- **Arnés** `t.py`: compila las unidades `libc/vfprintf`, `libc/vfprintf_1` y `libc/itoa` con los cflags del edge a `.o` privados. Borra la salida antes de compilar. Compara los digests SHA-1 por sección ALLOC y sus `.rela.*` (ignorando `.line`, `.debug*`, `.comment` y `.stab*`) contra una **base sellada**. Además puntúa cada función contra el objetivo con objdiff (`ppc.calculatePoolRelocations=false`).
- **Base sellada**: los tres `.o` privados de la fuente de partida dan ALLOC IDÉNTICAS a `build/GOWE69/src/libc/{itoa,vfprintf,vfprintf_1}.o`. Las diez funciones de las tres unidades casan al 100 %.
- **Controles que tienen que fallar, y fallaron**:
  - intercambiar `flags = 0;` y `dprec = 0;` en vfprintf.c da ALLOC DISTINTAS en los dos objetos (2 filas cada uno);
  - intercambiar `prec = ndigits;` e `i = 0;` en itoa.c da ALLOC DISTINTAS (4 filas).
- **vfprintf.c se compila dos veces**: `vfprintf.o` con coma flotante y `vfprintf_1.o` con INTEGER_ONLY. Todas las medidas comprueban los dos.

## 2. La palanca (b): el orden de declaración de newlib — NEGATIVO

Reproduje el bloque de newlib con la firma `_VFPRINTF_R(data, fp, fmt0, ap)`: `fp` y `ap` como parámetros, sin copias. El orden es `fmt, ch, n, m, cp, flags, ret, width, prec, sign, wc, [decimal_point, _fpvalue, trim, expstr], _uquad, base, dprec, realsz, size, number_end, xdigs, buf, ox, state = 0`. Los nueve pines de declaración van quitados.

| variante | `.text` v / v1 | filas distintas v / v1 |
|---|---|---|
| orden actual, sin los 9 pines (`unpin9`) | `9e61a957a4eb` / `7773cdc3855f` | 533 / 653 (+36 B / +24 B) |
| **newlib exacto** | **`fa9435d15b45` / `5c525de9e585`** | **555 / 660** |
| newlib, `state` en su sitio en vez de al final | `fa9435d15b45` / `5c525de9e585` | idénticos a newlib |
| newlib sin `register` | `fa9435d15b45` / `5c525de9e585` | idénticos a newlib |
| orden de newlib pero con copias `fp = fp0`, `ap = ap0` | `9e61a957a4eb` / `7773cdc3855f` | **idénticos a `unpin9`** |
| newlib con el bloque entero **al revés** | `91910ba517dc` / `1d3a50dd8afe` | 563 / 657 |
| newlib con las locales del bucle dentro del `for` (palanca a) | `fa9435d15b45` / `5c525de9e585` | idénticos a newlib |

**Cuántos pines sobran por el orden de newlib: cero.**
- Tres órdenes distintos dan el mismo objeto byte a byte.
- El orden de newlib aplicado a la versión con copias da el mismo objeto que el orden actual.
- Lo único que cambia algo es pasar `fp` y `ap` de copia a parámetro, y eso no es orden.
- El orden **sí** llega al objeto, pero solo como desempate: el bloque invertido da otro objeto, y peor.
- El ámbito (palanca a) no mueve nada.

¿Por qué? Porque la prioridad de `allocno_compare` (`global.c`) casi nunca empata. Lo que la decide se ve en §4.

## 3. `itoa.c`: los tres andamios eran la forma `EXTRACT_WORDS`

La libc de SN es newlib, y fdlibm saca las palabras de un double con:

```c
#define EXTRACT_WORDS(ix0, ix1, d) \
    do { ieee_double_shape_type ew_u; ew_u.value = (d); \
         (ix0) = ew_u.parts.msw; (ix1) = ew_u.parts.lsw; } while (0)
```

Así queda `fftoa`: `unsigned int hi, mant, t, neg, expo; unsigned int lo; ... EXTRACT_WORDS(hi, lo, value); mant = hi & 0xfffff; t = hi >> 20;`.

| variante | resultado |
|---|---|
| **`EXTRACT_WORDS(hi, lo, value)`** (la aplicada) | **ALLOC IDÉNTICAS** |
| `EXTRACT_WORDS(t, lo, value); mant = t & 0xfffff; t >>= 20;` | ALLOC IDÉNTICAS |
| `EXTRACT_WORDS(mant, lo, value); t = mant >> 20; mant &= 0xfffff;` | 10 filas |
| `do { u.d = value; lo = u.w.lo; } while (0)` con el union de fuera, sin pin | 4 filas (solo registro) |
| la misma, con el pin | ALLOC IDÉNTICAS (pero con el pin) |
| sin pin, con las dos barreras | 4 filas (`lo` r8/r11, `neg` r11/r10) |
| sin la primera barrera | 2 filas (`fmr f0,f30` antes de `mr r25,r4`) |
| sin la segunda barrera | 2 filas (`clrlwi` antes de `mr r11,r10`) |

Las dos barreras vacías y el pin imitaban lo que hace la macro. El `do { } while (0)` abre notas de bucle y cambia la ponderación de REG_N_REFS; el union vive en su propio ámbito. Ninguna de las once formas de la r65 tocaba ninguna de las dos cosas.

## 4. Lo que decide el reparto de `_vfprintf_r`: la profundidad de bucle de las macros

### 4.1 Los volcados, con el front end correcto

`ngccc -v` sobre un `.c` invoca **`cc1`**, pero `scripts/rtldump.py` invoca **`cc1plus`** siempre. Para libc hice un volcador propio (`scratchpad/libc67/rtl.py`) que usa `cc1`, y una tabla (`greg.py`) que junta:
- las líneas `Register N used U times across L insns` del `.lreg`;
- el orden y las disposiciones del `.greg`;
- la prioridad calculada como en `allocno_compare`: `floor_log2(U)*U/L*10000`.

En `fftoa` las cuentas de la r65 (76 pseudos con pin, 77 sin él) salen iguales con `cc1`.

### 4.2 Qué pseudo le sobraba a newlib sin pines

En la versión newlib sin pines, el diff arranca con `li r14,1` fuera del bucle y `dprec` en pila (`stw r22,0x184(r1)`). El objetivo carga `li r0,1; sth r0,0xe(r28)` en cada `fp->_file = 1`.

- **No es loop.c**: los volcados `.loop` del actual y de newlib mueven los mismos tres movables (savings 11, 8, 25) y descartan los mismos.
- **Es global**. El movable del `1` es el pseudo 150: HImode, 26 usos, 3334 insns, 41 llamadas, prioridad 311.

| | actual (con pines) = objetivo | newlib sin pines |
|---|---|---|
| const `1` (fp->_file) | pri 311 → **sin registro** (reload la pone en r0) | pri 311 → **r14** |
| `ret` | pinado r14 | 59 usos, pri 876 → r20 |
| `dprec` | pinado r15 | 14 usos, pri 282 → **sin registro** |

En el objetivo, la cola de preservados baja así: const 8 (765) a r18, `width` a r17, const 0 (479) a r16, `dprec` a r15 y `ret` a r14, y la const 1 (311) se queda sin registro. Para eso `ret` necesita una prioridad entre 311 y 479, es decir, de 27 a 32 usos ponderados sobre ~3366 insns. Newlib le da 59.

### 4.3 De dónde salen los usos de más: `do { } while (0)` y los operandos de barrera

REG_N_REFS pondera cada uso por `loop_depth`, y **`do { } while (0)` cuenta como bucle**. Nuestra PRINT y nuestra PAD son `do/while(0)`, así que todo lo que hay dentro de un PRINT dentro de PAD cuenta a profundidad 5. Eso incluye las constantes 0 y 1, la dirección de `blanks`/`zeroes` y los operandos `"r"(ret)`/`"r"(fmt)`/`"r"(fp)` de la barrera. **Newlib escribe PRINT, PAD y FLUSH como bloques `{ }`.**

Con las macros a llaves (BB2 = fuente r67 + llaves + `ret`, `fmt` y `ap` sin pin):

| pseudo | usos | live | prioridad | registro | objetivo |
|---|---|---|---|---|---|
| const 8 | 51 | 3332 | 765 | r18 | r18 |
| const 0 | 23 | 3336 | 275 | r16 | r16 |
| `ret` | 20 | 3366 | 237 | **r14** | r14 |
| const 1 | 17 | 3334 | 203 | **sin registro** | sin registro |

La escalera sale igual que en el original, y los pines de `ret`, `fmt`, `ap` (FLOATING_POINT) y `ch` (INTEGER_ONLY) **sobran**: los objetos con y sin esos pines son byte a byte iguales entre sí. **No se acepta** porque queda una permutación en los PAD de `zeroes`:

| sobre BB2 | filas v / v1 |
|---|---|
| tal cual (lis `@ha` en r26, `addi` en r25, contra r25/r26) | 20 / 20 |
| sin `PAD_KEEP_zeroes` | **12 / 12** (el trío del bucle interno sube un registro: r20/r21/r31 contra r19/r20/r21) |
| `PAD_KEEP_zeroes` movida a la cola | 12 / 12 |
| sin `PAD_KEEP_blanks` | 41 / 41 |
| sin la cola de `blanks` | 20 / 20 |
| sin la barrera de ancho, de signo, de float, de prefijo o el clobber | de 20 a 27 |
| con las formas newlib de `format_byte` o `field_count` | 24 / 24 |
| sin `ch`, `flags`, `width`, `dprec` o `number_end` | de 81 a 157 |

Van tres intentos serios sobre esta vía (el barrido con llaves, quitar PAD_KEEP y mover PAD_KEEP). Por la regla 7 la dejo aquí. **La pista para quien siga:**
- en el original hay, durante el bucle interno de los PAD de `zeroes`, un pseudo de prioridad mayor que 1063 ocupando un preservado alto que nuestra versión con llaves no tiene;
- `PAD_KEEP_zeroes` lo imita inflando la dirección, lo que invierte el par `@ha`/`addi`.

Quien lo encuentre retira de golpe los pines de `ret`, `fmt`, `ap` y `ch`, y probablemente las barreras PAD_KEEP.

## 5. Las retiradas de `vfprintf.c`, en orden de acumulación

Cada paso tiene ALLOC idénticas en los dos objetos; la columna `.text` es `_vfprintf_r`/`_vfiprintf_r`.

| paso | qué | resultado |
|---|---|---|
| 1 | barrera de PRINT sin `"r"(ret)` (con `ret` pinado) | IDÉNTICAS |
| 2 | `fp` sin pin (las dos ramas) + barrera sin `"r"(fp)` | IDÉNTICAS |
| 3 | + `emitted_p` sin pin | IDÉNTICAS |
| 4 | + `ap` sin pin **solo en INTEGER_ONLY** (en FLOATING_POINT, 351 filas) | IDÉNTICAS |
| 5 | + barrera de PRINT sin ningún operando | IDÉNTICAS |
| 6 | + forma newlib de la emisión, sin la local (`fp->_p += size`) | IDÉNTICAS |
| — | barrera de PRINT quitada entera | 152 / 159 filas → revertido |

La r65 midió la forma newlib de la emisión como NEGATIVA. Tenía razón *entonces*: `fp` estaba clavado en r28 y la barrera lo nombraba. Es otra vez el caso de *los andamios caducan*.

Barrido de un pin cada vez sobre la fuente final (filas v / v1):
- `fmt` 37/37;
- `ch` 106/120;
- `flags` 118/93;
- `ret` 81/78 (+48 B);
- `width` 118/92;
- `dprec` 171/129;
- `number_end` 127/27;
- `format_byte` 4/4;
- `field_count` 4/4;
- `mark` 15;
- `state` 17.

Con la barrera de PRINT conservando `"r"(fmt)`, `fmt` baja a 26/26. Con `"r"(ret)`, `ret` baja a 44/44. Los operandos de barrera suben los usos, pero no hasta la franja del original.

## 6. Sellos

Digests ALLOC (SHA-1, 12 cifras:tamaño). **Antes y después son iguales en todos los casos.**

| objeto | `.text` | `.rodata` | `.sdata` | `.bss` | `.rela.text` | `.rela.sdata` |
|---|---|---|---|---|---|---|
| `itoa.o` | `b86f14a700bf`:1548 | `6fe04e6456a5`:64 | — | 164 | `f15b75b6eeb8`:1524 | — |
| `vfprintf.o` | `c3ce1638243b`:6948 | `7bf8feeb9f1e`:144 | `05fe40575316`:8 | 128 | `3bf09023f266`:6072 | `a2f8f42abdda`:12 |
| `vfprintf_1.o` | `f48c48979805`:5972 | `da39a3ee5e6b`:0 | — | 0 | `1fd2b5fccbc7`:5148 | — |

`.data` es `da39a3ee5e6b`:0 en los tres.

El SHA-1 del `.o` completo cambia solo por las secciones no ALLOC (`.line`, `.debug`):

| objeto | build (antes) | r67 |
|---|---|---|
| `itoa.o` | `7ECB58776253` | `40A677074B2B` |
| `vfprintf.o` | `D467B1E13302` | `FA28BDF8838C` |
| `vfprintf_1.o` | `664EDE33EBC8` | `95A2D3360BDE` |

**Reenlace privado desde el edge `build\GOWE69\main.elf: link`** (619 objetos, `ngcld` con los ldflags del edge y `dtk elf2dol`, en `linkpriv67.py`):

| enlace | objetos | DOL |
|---|---|---|
| C0 | la lista del edge tal cual | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` = referencia |
| **V** | `itoa.o`, `vfprintf.o`, `vfprintf_1.o` → los de la fuente r67 | **`9619ba57c9919f95f7f2ac951a2166a3517f91e3` = referencia** |
| F (control) | `vfprintf.o` → fuente r67 con `flags`/`dprec` intercambiados | `400878006122b60a2234f7f1e50f5d0c940c425f` **ROTO** |

Objetos del edge que cambiaron durante la medida: 0.

Fuentes finales: `vfprintf.c` `E82D86F09194`, `itoa.c` `04F0B8748F6B` (copias en `scratchpad/libc67/*.r67`).

## 7. Los irreducibles que quedan (13 por la convención del encargo)

Todos llevan junto al andamio el diagnóstico r65 y, encima, la marca **r67** con lo medido en esta ronda.

| andamio | diagnóstico r67 |
|---|---|
| `ap` r29 (FLOATING_POINT), `fmt` r22 (×2), `ch` r26/r27, `flags` r24, `ret` r14, `width` r17, `dprec` r15, `number_end` r25 | Reparto global. El orden de declaración no llega (§2). La causa medida es la ponderación por profundidad de las macros `do/while(0)` (§4). Con llaves sobran `ret`, `fmt`, `ap` y `ch`, pero quedan 12–20 filas en los PAD de `zeroes`. |
| `format_byte` r0 (×2) | 4 filas sin el pin: `lbz r26; extsb r26,r26` contra `lbz r0; extsb r26,r0`. Con llaves se suman a las 20. |
| `field_count` r0 | 4 filas sin el pin (r15 contra r0). La forma newlib `ret += width > realsz ? width : realsz` da las mismas 4. |
| `mark` r8, `state` r7 (`add_separators`) | 15 y 17 filas sin pin sobre la fuente de hoy. El cambio de macros no toca esta función. |

Las 15 barreras de vfprintf.c siguen, con su diagnóstico r65. La cabecera y los comentarios de PAD_KEEP y PRINT añaden lo medido en r67.

## 8. Reglas nuevas, medidas

1. **Un `do { } while (0)` pesa en el reparto.** Abre notas de bucle, y REG_N_REFS pondera por `loop_depth` todo lo que hay dentro, incluidos los operandos `"r"(x)` de las barreras. Cambiar la macro de `do/while(0)` a `{ }` desplaza prioridades enteras de `allocno_compare`: `ret` pasó de 876 a 237. Sirve en las dos direcciones:
   - en `itoa.c`, **añadir** el `do/while` de `EXTRACT_WORDS` retiró tres andamios;
   - en `vfprintf.c`, **quitarlo** retira cuatro pines de golpe, aunque todavía no casa.
2. **Para libc, el oráculo son las MACROS de newlib y fdlibm, no solo el orden de las locales.** `EXTRACT_WORDS`/`GET_HIGH_WORD` en cualquier función que parta un double. PRINT, PAD y FLUSH con llaves en vfprintf.
3. **Un operando `"r"(x)` de barrera sobre una variable CLAVADA no pinta nada**: se puede quitar gratis (medido con tres). Sobre una variable libre sube sus usos ponderados.
4. **`ngccc` compila un `.c` con `cc1`**, no con `cc1plus`. Los volcados de libc tienen que salir de `cc1` (ver la propuesta).
5. **Palanca (e)**: no se ha usado. En `vfprintf.c` e `itoa.c` no hay asm legítimo del original, así que la regla de GPR con `"=r"` sigue sin medir.

## 9. Propuestas (fichero ajeno)

`scripts/rtldump.py`: que elija el front end por extensión, como hace `ngccc`. Líneas exactas:

```
-    p = SP.run([os.path.join(tools, 'cc1plus.exe')] + keep + dumps + ['-quiet', '-o', base + '.s', ii],
+    cc1 = 'cc1.exe' if src_rel.lower().endswith('.c') else 'cc1plus.exe'
+    p = SP.run([os.path.join(tools, cc1)] + keep + dumps + ['-quiet', '-o', base + '.s', ii],
```

No hace falta ningún cambio en `configure.py` ni en `config/GOWE69/*`.

## 10. Herramientas (`scratchpad/libc67/`)

| script | qué hace |
|---|---|
| `t.py` | `save`/`test`/`score` de las unidades v, v1 e i contra la base sellada; objdiff por función |
| `var.py` | variantes componibles de vfprintf.c (`unpin=…;bar=…;step=…`); **restaura siempre** el original |
| `vari.py` | variantes de itoa.c (ya no usar: restauraría la fuente vieja) |
| `rtl.py` | volcados `.lreg`/`.greg`/`.loop` con `cc1` |
| `greg.py` | tabla de `global.c`: usos, live, llamadas, prioridad y registro |
| `dif.py` | filas distintas con contexto, contra el objetivo |
| `censo67.py` | censo estricto de pines y barreras |
| `apply67.py` | los comentarios r67 de vfprintf.c |
| `ctl.py` | el `.o` de control que tiene que romper |
| `linkpriv67.py` | reenlace privado C0/V/F desde el edge |

`log.txt` y `logi.txt` guardan la salida de todas las variantes.
