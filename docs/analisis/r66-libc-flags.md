# r66 — lote `libc-flags`: los andamios de `src/libc` contra el cflag `-msafe-sda`

**Sin commit.** En el árbol solo he tocado comentarios en `src/libc/itoa.c`,
`src/libc/sf_log10.c` y `src/libc/vfprintf.c`, y los cuatro objetos siguen
con las secciones ALLOC idénticas (§7). No he tocado `configure.py`, nada
de `config/GOWE69/*`, `lcfix.py` ni ninguna cabecera, y no he re-extraído.

## 0. Veredicto

**La hipótesis se confirma en UN fichero de tres, y en ese fichero de punta a punta.**

| fichero | andamios | lleva `-msafe-sda` | hipótesis | resultado |
|---|---|---|---|---|
| `sf_log10.c` | 16 | no | **confirmada** | fuente fdlibm limpia + flag + rango de `splits.txt` → **DOL de referencia** en enlace privado |
| `itoa.c` | 3 | no | **refutada** | todas las variantes de flag cambian ALLOC; el objetivo no usa `.sdata` |
| `vfprintf.c` (+`vfprintf_1.c`) | 34 | no | **refutada** | ídem; `-mstrict-align` añade 320 B que el objetivo no tiene |

Hay **una propuesta atómica** (flag + rango + fuente) que retira **16 andamios**
cuando el jefe la aplique. Los otros 37 siguen irreducibles, y ahora llevan
junto a cada andamio el negativo de flags medido.

## 1. Censo y cruce con `configure.py`

El censo quita los comentarios y cuenta dos cosas:
- **barrera**: una plantilla vacía `__asm__("")`/`asm("")`/`asm volatile("")`, macros incluidas;
- **pin**: `register T x asm("rN"/"frN")`.

| fichero | barreras | pines | total | `extra_cflags` en `configure.py` |
|---|---|---|---|---|
| `itoa.c` | 2 | 1 | 3 | ninguno (`:2201`) |
| `sf_log10.c` | 7 | 9 | 16 | ninguno (`:2211`) |
| `vfprintf.c` | 15 | 19 | 34 | ninguno |
| `vfprintf_1.c` | 0 | 0 | 0 | ninguno (hace `#include "vfprintf.c"`) |
| **total** | **24** | **29** | **53** | |

El encargo hablaba de 36. Mi cifra incluye las **dos ramas** de cada
`#if FLOATING_POINT`/`INTEGER_ONLY`, porque cada rama se compila en uno de los
dos objetos. También incluye las 3 barreras de macro de vfprintf (`:220`,
`:229`, `:230`). Sin las 5 líneas alternativas de `#if` quedan 48.

**El cruce por sí solo no discrimina.** Es verdad que los 53 andamios están en
ficheros sin flag, y que los 10 ficheros con flag tienen cero andamios. Pero
otras ~70 unidades de libc tampoco llevan flag y tampoco tienen ninguno. Lo que
separa de verdad los tres ficheros es **cómo carga el objetivo su pool de
constantes**. `-msafe-sda` solo hace una cosa: `rs6000_select_rtx_section()`
(`rs6000.c:6160`) manda los `CONST_DOUBLE` a `.sdata` con `@sda21` en vez de a
`.rodata`. Una mirada al asm del troceador lo decide:

| fichero | cómo carga el objetivo sus constantes flotantes | ¿compatible con `-msafe-sda`? |
|---|---|---|
| `sf_log10.s` | `lfd f0, lbl_804FF168@sda21(r0)` (el sesgo int→double en `.sdata`) | **sí** |
| `itoa.s` | `lis r9, lbl_8040FB88@ha` + `lfd f0, lbl_8040FB88@l(r9)` (`.rodata`, 64 B, sin `.sdata` en `splits.txt`) | **no** |
| `vfprintf.s` | `lfd f0, lbl_8040FC28@l(r9)` … `lbl_8040FC40` (`.rodata`) | **no** |

Esta comprobación predijo los tres resultados de abajo antes de compilar nada.

## 2. Método

- **Compilación privada** (`scratchpad/libcflags66/cc.py`). Lee los cflags del
  edge con `build_direct.parse_units()`, añade los flags extra y escribe un
  `.o` privado. **Borra la salida antes de compilar**, así que un fallo nunca
  deja un `.o` viejo.
- **Línea base sellada.** Las cuatro fuentes de hoy, recompiladas en privado,
  dan ALLOC **idénticas** a `build/GOWE69/src/libc/{itoa,vfprintf,vfprintf_1,sf_log10}.o`.
  Ninguno estaba rancio.
- **Digests** (`secdig.py`). SHA-1 por sección ALLOC y por sus `.rela.*`,
  ignorando `.line`, `.debug*`, `.comment` y `.stab*`.
- **Enlace privado** (`linkpriv.py`).
  - Toma los 619 objetos del edge `build\GOWE69\main.elf: link` de
    `build.ninja` y enlaza con `ngcld` y los `ldflags` del edge
    (`-strip-unused-data -keep config\GOWE69\keep.lst -T config\GOWE69\ldscript.ld`).
  - Pasa el resultado por `dtk elf2dol`.
  - Sella con sha1 los 619 `.o` antes y después, para detectar objetos movidos
    por otros agentes. **0 movidos.**

## 3. `sf_log10.c`: CONFIRMADA, con el DOL

### 3.1 La fuente limpia

Es fdlibm `e_log10f.c` tal cual, sin una sola barrera, pin o `SDA_*_X`:

- `static const float two25, ivln10, log10_2hi, log10_2lo, zero`;
- `GET_FLOAT_WORD`/`SET_FLOAT_WORD`;
- `if (hx < 0x00800000)`;
- `y = (float)(k + i);`.

Está guardada en `scratchpad/libcflags66/sf_log10_limpio.c`, **fuera del
árbol**, como pide el encargo.

### 3.2 Las cuatro variantes de flags

Todas usan la fuente limpia y se comparan con el `.o` de hoy.

| flags extra | `.text` | `.rela.text` | `.rodata` | `.sdata` | `.sdata2` |
|---|---|---|---|---|---|
| (ninguno) | **296 B** `cf53ae6b` | 264 B | 40 B | — | 20 B |
| **`-msafe-sda`** | **268 B `e7bd46e7` = HOY** | 180 B `2a4793ef` | 0 | **40 B** `4f14d276` | 20 B |
| `-msafe-sda -mstrict-align` | 268 B `e7bd46e7` = HOY | 180 B `2a4793ef` | 0 | 40 B `4f14d276` | 20 B |
| `-mstrict-align` | 296 B `cf53ae6b` | 264 B | 40 B | — | 20 B |

Con `-msafe-sda` el **`.text` sale byte a byte igual al de hoy**. La r65
decía "difiere en una sola fila": esa fila no era de código sino del
**nombre** del destino de la reubicación, y aquí se resuelve.
`-mstrict-align` no añade nada en esta unidad, así que se propone el mínimo,
igual que su vecina `sf_log.c` (`configure.py:2210`).

### 3.3 Por qué los digests ALLOC NO pueden salir idénticos, y qué los sustituye

El flag cambia **quién emite los datos**. Hoy la `.o` referencia siete `extern`
`lbl_804FF158..178` que emite el comodín `auto_08_804FF128_sdata.o`. Con el
flag, la `.o` emite su propia `.sdata`. Por eso `.rela.text` y `.sdata` tienen
que cambiar por construcción. La prueba de equivalencia son tres cosas:

**(a) Cada reubicación cae en la misma dirección**, con la `.sdata` de la unidad en `0x804FF158`:

| instrucción | hoy | limpio + flag | dirección |
|---|---|---|---|
| `lfs f1` (−inf) | `lbl_804FF158` | `.sdata+0x0` | 0x804FF158 |
| `lfs f0` (zero) | `lbl_804FF15C` | `.sdata+0x4` | 0x804FF15C |
| `lfs f0` (two25) | `lbl_804FF160` | `.sdata+0x8` | 0x804FF160 |
| `lfd f0` (sesgo) | `lbl_804FF168` | `.sdata+0x10` | 0x804FF168 |
| `lfs f0` (log10_2lo) | `lbl_804FF170` | `.sdata+0x18` | 0x804FF170 |
| `lfs f13` (ivln10) | `lbl_804FF174` | `.sdata+0x1c` | 0x804FF174 |
| `lfs f12` (log10_2hi) | `lbl_804FF178` | `.sdata+0x20` | 0x804FF178 |

**(b) La `.sdata` nueva es el DOL.** Sus 40 B, `ff800000 00000000 4c000000
00000000 43300000 80000000 355427db 3ede5bd9 3e9a2080 00000000`, son
**idénticos byte a byte** a `orig/GOWE69/sys/main.dol` en `0x804FF158..0x804FF180`
(`dolbytes.py`), relleno incluido.

**(c) El enlace privado, con un control que tiene que fallar:**

| enlace | objetos | DOL |
|---|---|---|
| **C0** | la lista del edge tal cual | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` = referencia |
| **V1** | `sf_log10.o` → limpio + `-msafe-sda`; comodín recortado a `0x804FF128..0x804FF158` | **`9619ba57c9919f95f7f2ac951a2166a3517f91e3` = referencia** |
| **F1** | `sf_log10.o` → limpio + `-msafe-sda`; comodín ENTERO | `03a59393f0dc35ab275938b8cadb29961f658f5e` **ROTO** |

Hay además un control del ensamblador: el `.s` de hoy, ensamblado en privado
con los `asflags` del edge más `dtk elf fixup`, reproduce el comodín del build
con ALLOC idénticas. El recortado es ese mismo `.s` cortado en el bloque de
`0x804FF158`: 48 B, sha1 `4ba3213b39eb`.

### 3.4 Dos avisos de la r64b que el enlace desmiente

1. **"El hueco `0x804FF128..0x804FF158` hay que resolverlo en la misma pasada."**
   No hace falta. Esos doce floats (`ln2_hi`..`Lg1`) son de `sf_log.c`, que los
   referencia como `SDA_FLOAT_X`. Siguen en su comodín, y la lista de enlace ya
   lo pone **entre `sf_log.o` y `sf_log10.o`**. Es justo el criterio de
   `nfsmw-rango-no-basta`: el rango cae entre las dos vecinas en el ORDEN DE
   ENLACE. V1 lo demuestra.
2. **El `.sdata2` de 20 B** con las cinco `static const` escalares (plegadas y
   sin referencias) **lo estripa entero el enlazador**. Si quedara un solo
   byte, correría el `.sdata2` de `ef_pow` y V1 no daría la referencia.

### 3.5 La propuesta atómica

Está guardada en `scratchpad/libcflags66/PROPUESTA_sf_log10.txt`, con los sellos
esperados.

1. `configure.py:2211`
   ```
   -                    Object(Matching, "libc/sf_log10.c"),
   +                    Object(Matching, "libc/sf_log10.c", extra_cflags=["-msafe-sda"]),
   ```
2. `config/GOWE69/splits.txt`, entrada `libc/sf_log10.c` (línea 508), con TAB inicial:
   ```
    libc/sf_log10.c:
    	.text       start:0x8031CDB4 end:0x8031CEC0
   +	.sdata      start:0x804FF158 end:0x804FF180
   ```
3. `src/libc/sf_log10.c` ← `scratchpad/libcflags66/sf_log10_limpio.c`

**Van los tres juntos o el DOL rompe:**
- flag + fuente sin rango = F1, roto;
- fuente sin flag = 272 B (r65);
- rango sin fuente = faltan 40 B.

Orden: `splits.txt` → `dtk dol split`/`configure.py` → compilar → volver a
enlazar desde el edge → sha1. Al aplicarse retira **16 andamios (7 barreras +
9 pines)** y las 7 declaraciones `SDA_*_X`.

## 4. `itoa.c`: REFUTADA

Son 8 compilaciones: la fuente de hoy y la limpia (sin el pin `lo` en r11 ni las
dos barreras), cada una con cuatro juegos de flags. Todas se comparan con el
`.o` de hoy (`.text` 1548 B `b86f14a7`, `.rodata` 64 B, sin `.sdata`).

| fuente | flags | `.text` | resto |
|---|---|---|---|
| hoy | (ninguno) | 1548 B = HOY | ALLOC IDÉNTICAS (control) |
| hoy | `-msafe-sda` | 1460 B | `.rodata` 0, `.sdata` 48 B, `.sdata2` 16 B |
| hoy | `-msafe-sda -mstrict-align` | 1516 B | ídem |
| hoy | `-mstrict-align` | 1608 B | +60 B, todo en `fftoa` (0x4E8 → 0x524) |
| limpia | (ninguno) | 1548 B `f1652ed47eea` | el objeto equivocado de la r65 |
| limpia | `-msafe-sda` | 1460 B | ídem que la fuente de hoy |
| limpia | `-msafe-sda -mstrict-align` | 1516 B | ídem |
| limpia | `-mstrict-align` | 1608 B | ídem |

El objetivo lleva sus `double` en `.rodata` y no tiene `.sdata`, así que
`-msafe-sda` es incompatible **por construcción**. `-mstrict-align` añade
60 B de copias por bloque en `fftoa`, cuyo tamaño objetivo es exactamente el de
hoy. Los 3 andamios siguen siendo de reparto (`allocno_compare`, un pseudo de
más, r65), no de flags.

## 5. `vfprintf.c` / `vfprintf_1.c`: REFUTADA

Se mide sobre la fuente de hoy y contra el `.o` de hoy. En `vfprintf.o` el
`.text` mide 6948 B, la `.rodata` 144 B y la `.sdata` 8 B.

| objeto | flags | `.text` | resto |
|---|---|---|---|
| `vfprintf.o` | `-msafe-sda` | 6924 B | `.rodata` 32 B, `.sdata` 40 B, `.sdata2` 76 B nueva |
| `vfprintf.o` | `-msafe-sda -mstrict-align` | 7240 B | ídem |
| `vfprintf.o` | `-mstrict-align` | 7268 B | +320 B, todo en `_vfprintf_r` (0x17F0 → 0x1930) |
| `vfprintf_1.o` | `-msafe-sda` | 5972 B = HOY | IDÉNTICAS, pero **neutro**: con `INTEGER_ONLY` no hay pool |
| `vfprintf_1.o` | `-mstrict-align` (con o sin `-msafe-sda`) | 6292 B | +320 B |

El tamaño objetivo de `_vfprintf_r` es `0x17F0`, justo el de hoy. Ningún
retoque de andamios, que mueven de 4 a 16 B, borra 320 B de copias por bloque.
No he construido la variante limpia de 34 andamios: el pool en `.rodata` del
objetivo ya descarta `-msafe-sda` sea cual sea la fuente.

## 6. Las dos direcciones

- **Hacia fuera.** `extra_cflags` es por objeto (`Object(..., extra_cflags=...)`)
  y solo entra en el edge de esa unidad. Además, V1 enlaza los 618 objetos
  restantes sin tocar y da la referencia.
- **Hacia dentro.** Metí el flag en los ficheros donde la hipótesis era
  plausible pero no se sostiene (itoa, vfprintf): cambia ALLOC en todas las
  variantes. El control que tenía que fallar, falló.

## 7. Diagnósticos junto a los andamios

Escritos como comentario, para que `previo.py` los indexe con la ronda r66:

- `sf_log10.c`, cabecera: la receta probada, los sellos y los tres DOL.
- `itoa.c`, junto al pin `lo`: el negativo de flags con cifras.
- `vfprintf.c`, junto al bloque de nueve pines: el negativo de flags con cifras.

Verificado tras editar, con recompilación privada: **ALLOC IDÉNTICAS** a la base
y a `build/` en `itoa.o`, `sf_log10.o`, `vfprintf.o` y `vfprintf_1.o`. El censo
no cambia: los comentarios no citan ninguna plantilla ni ningún pin.

## 8. Lo que se lleva a otros lotes

1. **Para saber si un andamio compensa `-msafe-sda`, lee UNA línea del asm
   objetivo**: cómo se carga el pool. `@sda21` sobre un `lbl_` que ningún `VAR_DECL`
   nombra → candidato. `@ha`/`@l` → descartado. Contar ficheros con y sin flag
   no separa nada.
2. **Cuando un flag cambia quién emite un dato, los digests ALLOC no pueden
   quedar iguales.** La prueba tiene tres capas, y la última es la que manda:
   - mapear cada reubicación a su dirección;
   - comparar la sección nueva con los bytes del DOL;
   - **un enlace privado desde el edge, con un control que rompa** (F1).

   Se hace sin re-extraer: basta con recortar en privado el `.s` del comodín.
3. **Un aviso de un informe anterior es una hipótesis, no una restricción.** El
   "hay que resolver el hueco en la misma pasada" de la r64b era falso: el orden
   de enlace ya lo resolvía.

## 9. Herramientas (en `scratchpad/libcflags66/`)

| fichero | qué hace |
|---|---|
| `cc.py` | compila una fuente con los cflags del edge de una unidad más extras, a un `.o` privado; borra la salida antes |
| `secdig.py` | digests por sección ALLOC y sus `.rela.*`; con dos `.o` los compara |
| `dolbytes.py` | bytes de un rango de direcciones del DOL |
| `linkpriv.py` | enlace privado C0/V1/F1 desde el edge de `main.elf`, con sellado de los 619 objetos |
| `sf_log10_limpio.c` | la fuente limpia de la propuesta |
| `PROPUESTA_sf_log10.txt` | las líneas exactas y los sellos esperados |

Los volcados (`.o`, `.elf`, `.dol`, `.rsp`, desensamblados) están borrados.
