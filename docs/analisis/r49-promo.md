# r49 `promo` — tres promociones, y el falso positivo que enmascaraba el frente

Territorio: las unidades sin enlazar cuyo bloqueo **no** son las plantillas —
`zGameModes`, `zAI`, `pathbank`, `ef_rem_pio2`, `kf_rem_pio2`, `zMisc`,
`avplayer`—.

## Resultado

| unidad | B de `linked` | veredicto |
|---|---:|---|
| `libc/ef_rem_pio2` | 848 | **DOL OK** |
| `libc/kf_rem_pio2` | 2.180 | **DOL OK** |
| `Speed/…/path/…/cmn/pathbank` | 2.156 | **DOL OK** |
| **las tres juntas** | **5.184** | **DOL OK** |
| `egami/…/av/cmn/avplayer` | 4.224 | `.rodata` **idéntica byte a byte**; falta un rango en `splits.txt` |
| `zGameModes` | 124 | negativo medido: faltan 612 de 844 B de `.rodata` |
| `zMisc` | 78.008 | negativo medido: `.rodata` +2.840 tras enlazar |
| `zAI` | 272.796 | negativo medido: `.rodata` −1.344 y `.data` −384 tras enlazar |

`trypromo` de las tres, juntas y por separado, imprime `DOL OK`, o sea
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`:

```
  libc/ef_rem_pio2 + libc/kf_rem_pio2 + Speed/…/pathbank      DOL OK
```

Verificación: `fncmp` sobre las cinco unidades tocadas da **0 funciones con el
código distinto** en todas, y `lcfix.py --check` sale limpio.

---

## 1. El falso positivo que mandaba la ronda al sitio equivocado

El censo de la r48 clasificó cuatro unidades como «nos falta un símbolo»:
`zAI`, `pathbank`, `ef_rem_pio2` y `kf_rem_pio2`. **Tres de las cuatro estaban
mal diagnosticadas.** `promote.py` decía, por ejemplo:

```
- no definimos 5 simbolo(s) que el extraido exporta:
    lbl_8041399C <- auto_01_8037A2B8_text; …
```

`auto_01_8037A2B8_text`, `auto_01_8031E868_text` y `auto_01_8031EC3C_text`
**no están en el enlace**: `grep -c` sobre `build.ninja` da 0 para los tres. Son
objetos rancios de una extracción vieja, de cuando esas `.c`/`.cpp` todavía no
estaban en `splits.txt`; siguen en `build/GOWE69/obj/` y nadie los borra.

La causa está en `scripts/promote.py`, en `_construye_quien()`: recorre
`os.walk(OBJ)` **entero** en vez de la lista de objetos del enlace. El índice de
UNDEF que alimenta `_referenciados()` sale contaminado, y con él la línea «no
definimos N símbolos» y la de «quién lo referencia».

**Parche propuesto** (no lo aplico: `promote.py` cachea ese índice en
`scratchpad/.promote_undef.pkl` con un sello que sólo mira el número y la fecha
de los `.o`, así que cambiar el script a mitad de ronda daría a cada agente un
resultado distinto según quién regenere la caché primero): en
`_construye_quien()`, construir primero el conjunto de rutas absolutas que
devuelve `objetos_del_enlace()` —el mismo helper que usa `trypromo.py`— y saltar
en el `os.walk` todo `.o` que no esté en él. Conviene además invalidar la caché
(cambiarle el nombre al fichero) en el mismo commit.

Y de paso: `scripts/refs.py` y `scripts/promopred.py` **sí** parten de
`objetos_del_enlace()`, así que sus cifras son buenas. La que hay que releer es
la columna «nos falta un símbolo» del mapa de la r48.

**El único caso legítimo de esa familia es `zAI`** (§5).

---

## 2. `ef_rem_pio2` y `kf_rem_pio2` — el tamaño mentido de `kf_tan.c` generaliza

El bloqueo real de las dos era una sección de más:

| | `.text` | `.sdata` | `.sdata2` |
|---|---:|---:|---:|
| `ef_rem_pio2` extraído | 848 | 56 | — |
| `ef_rem_pio2` nuestro (antes) | 848 | 56 | **920** |
| `kf_rem_pio2` extraído | 2.180 | 40 | — |
| `kf_rem_pio2` nuestro (antes) | 2.180 | 40 | **84** |

Y sin embargo `linkdelta` daba `TODAS IGUALES` para `ef_rem_pio2`: los 920 B se
compensan exactamente porque, al dejar de referenciar el comodín, el enlazador
estripa **su** copia. **Mismo tamaño y DOL roto es una diferencia de POSICIÓN**,
no de bytes: la copia del comodín vive en 0x8050042C (dentro de
`auto_10_805003A8_sdata2`, puesto 136 del enlace) y la nuestra caía en el puesto
155, que es donde está `libc/ef_rem_pio2.o`.

Las dos tablas están en el DOL con el nombre que les puso el troceador, y el
objeto comodín las exporta:

```
auto_10_805003A8_sdata2.o:
   .sdata2  132  792  GLOBAL OBJECT  two_over_pi_8050042C
   .sdata2  924  128  GLOBAL OBJECT  npio2_hw_80500744
   .sdata2 1052   12  GLOBAL OBJECT  init_jk_805007C4
   .sdata2 1064   44  GLOBAL OBJECT  PIo2_805007D0
```

La receta es la de `kf_tan.c:18` (`extern const float T[2] __asm__("T_805003F8")`,
«tamaño mentido: <= 8 B para que salga @sda21»), y **funciona igual con arrays
grandes**:

```c
/* src/libc/ef_rem_pio2.c */
extern const int two_over_pi[2] __asm__("two_over_pi_8050042C");
extern const int npio2_hw[2]    __asm__("npio2_hw_80500744");

/* src/libc/kf_rem_pio2.c */
extern const int   init_jk[2] __asm__("init_jk_805007C4");
extern const float PIo2[2]    __asm__("PIo2_805007D0");
```

`.text` se queda en 848 y 2.180 B exactos, y `.sdata` sigue siendo **idéntica
byte a byte** al DOL (0x804FF350 y 0x804FF388). El tamaño de 2 elementos es lo
que mantiene el `SYMBOL_REF_FLAG` y con él el direccionamiento `@sda21`; con el
tamaño real se cumple la veda medida en la r36f (`.text` +12 B).

En `kf_rem_pio2` sobraban además siete escalares. El DOL sólo trae los primeros
60 B de su `.sdata2` (`init_jk` 12 + `PIo2` 44 + `zero` 4, este último bajo el
nombre `gap_10_805007FC_sdata2`); `one`, `two8`, `twon8`, `eighth`, `eight` y
`half` **no existen en ninguna parte de la imagen**. Nuestro código no los
referencia —el front-end los pliega y el pool va a `.sdata` con `-msafe-sda`—,
así que pasan a `#define` y `.sdata` sigue saliendo byte a byte igual.

### Cambio en `configure.py`

```python
# línea 2137
Object(Matching, "libc/ef_rem_pio2.c", extra_cflags=["-msafe-sda", "-mstrict-align"]),
# línea 2140
Object(Matching, "libc/kf_rem_pio2.c", extra_cflags=["-msafe-sda", "-mstrict-align"]),
```

---

## 3. `pathbank` — un `$LC` muerto se estripa y desplaza 4 B; un `asm()` de fichero no

El objetivo tiene 172 B de `.rodata` y nosotros 164. Los 8 que faltan son una
**segunda copia** del literal muerto `"%s  %s"` de la DEUDA r28: el DOL lo trae
dos veces, en 0x80413A38 y en 0x80413A40.

Lo que costó cerrarlo, y vale como catálogo de lo que GCC 2.9 **no** hace:

| intento | resultado |
|---|---|
| dos `inline` muertas que devuelven `"%s  %s"` | `.rodata` 164: **funde los dos literales idénticos de la misma TU** |
| un solo literal de 15 B con dos NUL embebidos | `ngccc`: **`virtual memory exhausted`** — muere con un `\0` escapado dentro de la cadena. Reproducido dos veces |
| `static const char x[] = "%s  %s";` | no lo emite: estático sin usar |
| … con `__attribute__((section(".rodata")))` | tampoco |
| … referenciado desde una `inline` muerta | tampoco |
| `const char x[] = …` a secas | tampoco: en **C++ un `const` de ámbito de fichero tiene enlace INTERNO** |
| `extern const char x[]; const char x[] = …` | sí lo emite, pero en **`.sdata2`** |
| … más `__attribute__((section(".rodata")))` | `.rodata` **172 B, idéntica byte a byte**… y el DOL **sigue roto** |

Ese último caso es el hallazgo. Con las secciones ya iguales (el enlace completo del
proyecto daba `TODAS IGUALES`) el DOL diferenciaba **8 B, y sólo 8**:

```
off 00410A3A  VA 0x80413A3A  ORIG 2020      NUE 0000
off 00410A3E  VA 0x80413A3E  ORIG 0000      NUE 2020
off 00410A42  VA 0x80413A42  ORIG 20202573  NUE 00000000
```

o sea, los dos literales **desplazados 4 B**: `-strip-unused-data` se lleva parte
del `$LC5` muerto —que es LOCAL y aun así se estripa— y compacta lo que viene
detrás. La cura es que los bytes **no tengan símbolo**: un `asm()` de fichero.

```c
/* src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp, al final */
asm("\t.section\t.rodata\n\t.align\t2\n"
    "\t.byte\t37,115,32,32,37,115,0,0\n"
    "\t.byte\t37,115,32,32,37,115,0,0\n\t.text");
```

(en el fichero va en una sola línea; `pathbank.cpp` es CRLF entero.)
`.align 2` es obligatorio: sin él los `.byte` empiezan en 163 en vez de 164 y
salen los mismos 8 B mal.

**Esto es una palanca nueva y general**: cualquier unidad cuyo único defecto sea
un literal muerto de tamaño no múltiplo de 8 al final de su `.rodata` está en la
misma situación. Las otras seis TU de `path` con la DEUDA r28 ya promocionan, así
que ahí no queda nada, pero el patrón «`.rodata` idéntica y DOL roto en 4-8 B» es
diagnosticable con un solo enlace.

### Cambio en `configure.py`

```python
# línea 1063
Object(Matching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp"),
```

---

## 4. `avplayer` — 104 B idénticos, y un rango que empieza tarde y acaba pronto

`avplayer` referencia tres símbolos que caen **fuera** de su propio rango:
`lbl_8040FEE0`, `lbl_8040FF40` y `lbl_8040FF48`. Con el pool per-TU eso no puede
ser: su `.rodata` de verdad es **0x8040FEE0..0x8040FF48**, y `splits.txt` le da
0x8040FEFC..0x8040FF40 —28 B tarde por delante y 8 B pronto por detrás—.

Lo que faltaba en la fuente, todo comprobado contra los bytes del DOL:

1. `"Assert: %s:%i %s (%s)"` — literal **muerto** de la TU, entre `""` y
   `"AV::VideoStreamBuffer"`. Sin símbolo (mismo `asm()` de fichero que
   `pathbank`) y colocado **detrás de `class CHUNK`**: los literales de cadena de
   GCC 2.9 salen en orden de aparición en la FUENTE, no de emisión de funciones
   (`$LC0` se usa en `.text+1882` y `$LC1` en `.text+386`, y aun así `$LC0` va
   primero).
2. Una **segunda copia** de `0x41E0000000000000` (2^31, la conversión
   `unsigned -> float`), muerta, entre la de `GetFrame` y el sesgo i2d de
   `IsTimeForDecode`. Las constantes de coma flotante del pool sí salen al final
   de la función que las usa, así que ese `asm()` va **entre las dos funciones**.

Resultado: `.rodata` **104 B, idéntica byte a byte al DOL en
0x8040FEE0..0x8040FF48**, `.text` clavado en 4.224 y `fncmp` con 0 funciones
distintas.

Aviso medido: sustituir `lbl_8040FF48[0]` por el literal `0.001f` **rompe**
`IsTimeForDecode` (reparto y orden, 15 filas). La forma buena es la que ya tenía
la fuente, el `extern const float lbl_8040FF48[]`.

**Medida que respalda la propuesta**: con el rango actual, el enlace completo da
`rodata+8`; el rango pide exactamente los 36 B que faltan (28 delante + 8
detrás), y `auto_05_8040FEE0_rodata.o` mide **exactamente 28 B** —desaparece
entero, sin fragmento sobrante por delante—.

### Propuesta

```
egami/rcmp/dev/source/av/cmn/avplayer.cpp:
	.text       start:0x803472B4 end:0x80348334
	.rodata     start:0x8040FEE0 end:0x8040FF48     # antes 0x8040FEFC..0x8040FF40
```

No hace falta tocar `symbols.txt`: `lbl_8040FEE0` mide 0x1C y acaba justo en
0x8040FEFC, `lbl_8040FF40` mide 8 y acaba justo en 0x8040FF48, y `lbl_8040FF48`
(0x24 B) se queda **entero** en el comodín, que es donde lo seguimos
referenciando.

**Riesgo declarado (Regla A de `ventana-pendiente.md` §1)**: el trozo trasero de
`auto_05_8040FF40_rodata.o` (312 B) queda en **304 B empezando en 0x8040FF48**.
Son >4 B, o sea el modo de fallo que rompió cinco rangos en la ventana 12. A
favor: 0x8040FF48 **ya está alineado a 8**, así que el realineado que describe la
regla sería un no-op. Se prueba en 30 s con `trypromo` después de re-extraer; si
rompe, se deja `avplayer` como `NonMatching` y el trabajo de fuente no se pierde
(la `.rodata` sigue siendo la correcta).

Con el rango aplicado: `Object(Matching, "egami/rcmp/dev/source/av/cmn/avplayer.cpp")`
(línea 757).

---

## 5. `zAI` — `value.10688_80456560` es la BASE de la `.bss` de la imagen

Es el único «nos falta un símbolo» legítimo, y no es lo que parecía. `crt0`
referencia `value.10688_80456560` porque el troceador le puso ese nombre al
símbolo que está en `.bss:0x80456560`, que es **el primer byte de la `.bss` de la
imagen entera**: `crt0` lo usa como base del `memset` que la limpia.

```
extraido zAI.o                             nuestro zAI.o
.bss   0  4  GLOBAL  value.10688_80456560  .bss   0  4  LOCAL  lower.13230
.bss   4  4  LOCAL   _.tmp_0.10689         .bss   4  4  LOCAL  _.tmp_0.13231
.bss   8  4  LOCAL   k.16707               .bss   8  4  LOCAL  upper.13235
                                           …
                                           .bss  64  4  LOCAL  k.16606
```

El original tiene **una** pareja `<estático, _.tmp_N>` antes de `k.16707`;
nosotros tenemos **ocho**. De ahí el `.bss` +64 B y los 1.114 símbolos
desplazados exactamente +64 que lista `promote.py`. **Y no es el bloqueo**: tras
enlazar el proyecto entero, el delta de `.bss` es **0** —esos estáticos están
muertos y el enlazador se los lleva— y el de `.text` también, con +5.660 B en el
objeto.

Lo que queda, medido con `linkdelta`:

```
zAI    .text +0    rodata-1344  data-384
```

**Emitimos MENOS dato del que hace falta**, que es el frente que la r48 dejó sin
diagnosticar. De la `.data`, casi la mitad es una frontera de `splits.txt`:

| símbolo que sólo tiene el extraído | B |
|---|---:|
| `pad_06_80415180_data` | **172** |
| `lbl_80415438` | 68 |
| `gap_06_80415230_data` | 44 |
| `gap_06_80415498_data` | 40 |
| resto (`gap_*`, `lbl_*`) | 72 |

`pad_06_80415180_data` son **172 bytes a cero al principio del rango**: el primer
símbolo de verdad del objeto extraído (`RandomSortTCDir`) está en 0x8041522C. O
sea que el `.data` de `zAI` **empieza 172 B tarde** y esos ceros son de quien va
antes en el orden de enlace.

**Propuesta medida** (no probada: hace falta re-extraer):

```
Speed/Indep/SourceLists/zAI.cpp:
	.data       start:0x8041522C end:0x804154C0     # antes 0x80415180
```

Con eso el déficit de `.data` baja de 384 a ~212 B. Para el `value.10688` no hace
falta nada mientras `zAI` no promocione; cuando promocione, el alias va sobre la
**definición** del primer estático de la unidad (`static int value
__asm__("value.10688_80456560");`) y hay que quitar antes las siete parejas
sobrantes, porque el símbolo tiene que caer en el offset 0 de la `.bss`.

---

## 6. `zGameModes` — el `EmotionManager` SÍ está en el DOL

El censo de la r48 leyó «emitimos `.data` de 64 B y `.rodata` de 248 donde el
extraído no lleva ninguna de las dos» como que el subsistema no se enlazó. Se
enlazó: sus literales están en el DOL, en **0x803EBD64..0x803EBE94**, dentro del
rango que `splits.txt` le da hoy a `zGameplay`.

Lo que se estripó fueron las **tablas de punteros** (`aEmotionalSummaryTypeStrings`
y compañeras: no hay ni un puntero a 0x803EBD64 en toda la imagen), no las
cadenas. Encaja con lo que ya decía `stripped.py`: el enlazador de SN calcula la
vida **una sola vez**, así que la tabla muere pero lo que ella referenciaba ya
estaba marcado vivo.

Y las cadenas del DOL **no son las que teníamos**:

| nuestro | el DOL |
|---|---|
| `PURSUIT_BEGINS`, `PURSUIT_ADDS_CAR`, `PURSUIT_ADDS_HELI`, `PURSUIT_ADDS_ROADBLOCK`, `PURSUIT_ENDS`, `ARRESTED` | `PursuitBegins`, `PursuitAddsCar`, `PursuitAddsHeli`, `PursuitAddsRoadblock`, `PursuitEnds`, `Arrested` |
| `"Created"` como primera fase | **no existe**: entre `Arrested` e `Initial` no hay nada |

Corregido en `src/Speed/Indep/Src/Gamemodes/EmotionManager.cpp` (los seis
nombres, y la primera entrada del array de fases pasa a `0`; el `enum` no se
toca, porque `GetEmotionalResponsePhaseString` compara contra `MaxPhases` y sigue
casando al 100 %). **La `.rodata` pasa de coincidir en 0 B a coincidir en los
primeros 188 de 232**, y `fncmp` sigue con 0 funciones distintas.

Lo que falta para que promocione, con la cifra:

| tramo | B | qué es |
|---|---:|---|
| 0x803EBB48..0x803EBD64 | **540** | literales de cabeceras que nuestra SourceList no incluye: `"GAMECUBE"`, `"d:/mw/speed/indep/bware/inc/bware.hpp"`, `"bad_alloc"`, `"%f,%f,%f"`, `"Attrib::Gen::audioscrape"`, `"Pkt_Body_Open"`, … |
| 0x803EBD64..0x803EBE20 | 188 | **ya idéntico** |
| 0x803EBE20..0x803EBE94 | **116** | `"EmotionManagerImpl"`, una tabla de 16 flotantes, `"EmotionalResponse"` y el `1.0f` del pool; nosotros ponemos ahí `_vt.14EmotionManager` (24 B) y el `1.0f` |

Total real de su `.rodata`: **844 B**; producimos 232. Por 124 B de `linked` no
sale a cuenta esta ronda, pero el trozo del medio ya no hay que rehacerlo, y
`zGameplay` tiene 844 B de `.rodata` que no son suyos.

---

## 7. `zMisc` — `lbl_8041F8C3` no es un bloqueo: ya está resuelto

El mapa de la r48 dejó `zMisc` y `avplayer` como «reubicación con sufijo de
dirección», y a `zMisc` como la que podía no tener salida. **No es el caso:**

```
lbl_8041F8C3 = .data:0x8041F8C3;   // size:0x1
```

lo define **nuestra propia fuente**, en `src/Speed/GameCube/Src/Render/SunE.cpp:57`
(`unsigned char lbl_8041F8C3 = 0xf4;`), que compila dentro de `zPlatform` —cuyo
`.data` es 0x8041F728..0x80435768—. La referencia resuelve promocione `zPlatform`
o no, así que el racimo no existe.

El bloqueo real de `zMisc` es de secciones, y tras enlazar el proyecto entero es
sólo uno:

```
zMisc  .text +0   rodata+2840  data+32
```

El `.text` +6.888 B del objeto **no cuesta nada** (se estripa entero). Lo que
queda son 2.840 B de `.rodata` de más. Con el predicado de la r48 (Medida 4) lo
que toca es mirar cuáles de sus 98 símbolos de más están vivos, no cuántos hay.

---

## Resumen de cambios a aplicar

**Fuente (ya en el árbol, sin commitear):**

| fichero | qué |
|---|---|
| `src/libc/ef_rem_pio2.c` | `two_over_pi`/`npio2_hw` pasan a `extern … __asm__(…)` con tamaño mentido |
| `src/libc/kf_rem_pio2.c` | ídem `init_jk`/`PIo2`; los siete escalares pasan a `#define` |
| `src/Speed/Indep/Libs/path/…/pathbank.cpp` | los dos literales muertos, desde un `asm()` de fichero |
| `src/egami/…/av/cmn/avplayer.cpp` | la cadena de assert muerta y el 2^31 duplicado, desde `asm()` de fichero |
| `src/Speed/Indep/Src/Gamemodes/EmotionManager.cpp` | los seis disparadores en CamelCase y sin `"Created"` |

**`configure.py` — promocionar (probado con `trypromo`, DOL OK):**

```python
línea 1063  Object(Matching, "Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp"),
línea 2137  Object(Matching, "libc/ef_rem_pio2.c",  extra_cflags=["-msafe-sda", "-mstrict-align"]),
línea 2140  Object(Matching, "libc/kf_rem_pio2.c",  extra_cflags=["-msafe-sda", "-mstrict-align"]),
```

**`splits.txt` — para la ventana (no probado, hace falta re-extraer):**

```
egami/rcmp/dev/source/av/cmn/avplayer.cpp:  .rodata start:0x8040FEE0 end:0x8040FF48
Speed/Indep/SourceLists/zAI.cpp:            .data   start:0x8041522C
```

**`scripts/promote.py`**: filtrar `_construye_quien()` por `objetos_del_enlace()`
(§1). Sin eso, «no definimos N símbolos» sigue mintiendo.
