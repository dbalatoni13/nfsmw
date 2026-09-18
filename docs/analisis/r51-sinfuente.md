# r51 · las seis unidades sin fuente

**Resultado: las seis cierran. `DOL OK` una a una y `DOL OK` las seis juntas.**
El paquete **no es atómico**: cada unidad se puede promocionar sola.

`linked` 506 → **512** sobre un techo de 545.

---

## 0. El hallazgo que las explica a las seis

La premisa del encargo era que estas unidades «no emitían nada porque el fuente
de EA estaba vacío». **Es falso, y el ELF original lo dice.**

El `.debug` del original es DWARF1 de GCC (`-gdwarf+`) y trae una CU por cada
uno de los seis ficheros. Las CU **sí declaran las funciones**, y las declaran
con `AT_low_pc = 0xFFFFFFFF`. Ese `-1` es la marca que el enlazador de SN deja
en el DWARF cuando **estripa** una función: el fuente existía y tenía código,
pero nadie lo llamaba y el enlace se lo llevó.

| fichero | lo que declaraba la CU | estado en el DOL |
|---|---|---|
| `sformat.c` | `SNDMEMI_printf`, `SNDMEMI_format` | las dos con `low_pc = 0xFFFFFFFF` |
| `soutputmap.cpp` | `mapChannelToOutput` | `low_pc = 0xFFFFFFFF` |
| `mpegl3base.cpp` | 17 métodos de `CMpegLayer3Base` (`Dequantize`, `Stereo`, `Reorder`, `AntiAlias`, `Imdct36X1`, `Hybrid`, …) | los 17 con `low_pc = 0xFFFFFFFF` |
| `mpeghufftables.cpp` | **ninguna definición**: sus hijos directos son tipos y declaraciones de cabecera, ni un `AT_low_pc` ni un `AT_location` | tablas estáticas, estripadas enteras |
| `smixvec.c` | 7 `global_variable` con `AT_location` explícito | **sobreviven** |
| `author.cpp` | 1 `global_variable` con `AT_location` | **sobrevive** |

Y el corolario que cambia el trabajo: **la unidad de traducción correcta es la
que no emite nada**. Reescribir `SNDMEMI_printf` no acercaría el binario ni un
byte y sí podría meter en `.text` lo que el original no tiene.

### Corolario medido: `-strip-unused-data` estripa CÓDIGO, no sólo dato

Control de la regla 12, sobre `sformat`:

    fuente de control                                    veredicto
    int __probe_ctrl_value = 0x1234;                     DOL OK      <- INVISIBLE
    int __probe_ctrl_fn(int a){return a+__probe_ctrl_value;}

    struct ProbeCtrl { ProbeCtrl(); int v; };            DOL ROTO    <- discrimina
    ProbeCtrl::ProbeCtrl(){ v = 0x1234; }                (.text +120 B, .ctors +4 B)
    static ProbeCtrl g_probe;

O sea: una función global sin llamante **y el dato que ella referencia** se van
los dos con `-strip-unused-data` y el DOL no se entera. El primer control
parecía decir «la sonda no llega al enlace»; el segundo demuestra que sí llega.
Por eso todos los controles de este informe usan un constructor estático: `.ctors`
lo recorre el arranque y no se puede estripar.

**Esto reabre una veda del proyecto**: «el objeto tiene N funciones de más» no
bloquea nada mientras nadie las llame. Ya estaba medido en `linkdelta.py`
(«zLua tiene el `.text` del objeto +18.908 B y enlazado sale igual»); aquí queda
medido también para **dato global sin referencias**.

---

## 1. Cómo se ha probado cada una

Tres medidas, en este orden:

1. `python scripts/promote.py <ruta>` → `LIMPIA: se puede marcar Matching`.
2. `python scripts/trypromo.py <ruta>` → SHA1 del DOL contra
   `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.
3. **Contenido** de las doce secciones cargables del ELF enlazado, byte a byte,
   contra el ELF del enlace base (no tamaños: contenido).

La tercera, con las seis sustituidas a la vez:

    seccion   nuestro ELF vs ELF BASE
    .init     IGUAL      .rodata   IGUAL
    .text     IGUAL      .data     IGUAL
    .over     IGUAL      .sdata    IGUAL
    .ctors    IGUAL      .sdata2   IGUAL
    .dtors    IGUAL      .bss      IGUAL   (addr y size; NOBITS)
                         .sbss     IGUAL
                         .sbss2    IGUAL

    0 secciones cargables distintas

Y cada unidad lleva **su propio control negativo**, para que «DOL OK» no pueda
venir de que la sustitución no llegó a ocurrir.

---

## 2. Las seis, una a una

### 2.1 `sformat` — `Speed/Indep/Libs/snd/9/source/library/cmn/sformat.c`

* **Qué emite el original**: nada. `splits.txt` le da `.text 0x8036D268..0x8036D268`
  (0 B) y ninguna otra sección. El objeto extraído mide 692 B y sus secciones
  cargables son un `.text` de tamaño 0; su tabla de símbolos son el `STT_FILE`
  `sformat.c` y el símbolo de sección, nada más.
* **Fuente escrita**: comentario documentando las dos funciones estripadas.
  Cero emisión. Objeto: `.text`/`.rodata`/`.data`/`.bss` los cuatro a 0 B.
* **Prueba**: `promote.py` LIMPIA · `trypromo.py` **DOL OK**.
* **Control**: con el constructor estático → **DOL ROTO (`46e0fcd53eb7`)**.

### 2.2 `soutputmap` — `.../library/cmn/soutputmap.cpp`

* **Qué emite el original**: nada. `.text 0x8036D7B4..0x8036D7B4`.
* **Fuente escrita**: comentario. Cero emisión.
* **Prueba**: LIMPIA · **DOL OK**.
* **Control**: constructor estático → **DOL ROTO (`b8343cc39550`)**.

### 2.3 `mpeghufftables` — `.../library/extern/coda/cmn/mpeghufftables.cpp`

* **Qué emite el original**: nada. `.text 0x803662C4..0x803662C4`. Es la única
  de las tres vacías cuya CU **no tiene ni una definición**: las tablas de
  Huffman eran estáticas y el enlazador se las llevó enteras.
* **Fuente escrita**: comentario. Cero emisión.
* **Prueba**: LIMPIA · **DOL OK**.
* **Control**: constructor estático → **DOL ROTO (`1ca3a735d1be`)**.

### 2.4 `mpegl3base` — `.../library/extern/coda/cmn/mpegl3base.cpp`

* **Qué emite el original**: `.text` vacío + **4 B de `.rodata`** en
  `0x80412AE8..0x80412AEC`. Leídos del ELF original: `46 FF FE 00` = **`32767.0f`**,
  la escala a entero de 16 bits.
* Ese dato **no tiene nombre en el original**; el troceador lo llama
  `lbl_80412AE8`. Sobrevive al estripado porque **`ssdfx.c` lo usa**: allí ya
  está declarado `extern "C" const float lbl_80412AE8[];` (línea 8) desde una
  ronda anterior.
* **Fuente escrita**: una línea.

      extern "C" const float lbl_80412AE8[1] = { 32767.0f };

  El objeto sale con `.rodata` de 4 B, contenido `46 FF FE 00`, símbolo
  `lbl_80412AE8` GLOBAL OBJECT de 4 B. Idéntico al extraído salvo el `align`
  (4 en el nuestro, 8 en el extraído), que resulta no ser vinculante porque el
  `.rodata` anterior termina justo en `0x80412AE8`.
* **Prueba**: LIMPIA · **DOL OK**.
* **Control**: `32767.0f` → `32766.0f` → **DOL ROTO (`b6dd86106da0`)**.

### 2.5 `author` — `.../library/cmn/author.cpp`

* **Qué emite el original**: `.text` vacío + **64 B de `.data`** en
  `0x804513A4..0x804513E4`. Leídos del ELF original:

      "SNDAUTHOR:  Adamchan, Tuesday 02:10PM Feb 08, 2005, V9.06.00\0" + 3 B a cero

  La CU de DWARF1 tiene **un solo hijo con dato**: `global_variable sndlibauthor`,
  tipo array `[0..60]` de `FT_char` (61 B), `AT_location = 0x804513A4`. Ni una
  función. Los 3 B finales son el relleno de 61 a 64 que pone el propio GCC; el
  troceador les inventó el símbolo `gap_06_804513E1_data`, que no hace falta.
* El símbolo sobrevive al estripado porque **`ssysinit.c` lo referencia**
  (`extern char sndlibauthor;` y `sndlibauthor = 'S';`).
* **Fuente escrita**: una línea.

      char sndlibauthor[] = "SNDAUTHOR:  Adamchan, Tuesday 02:10PM Feb 08, 2005, V9.06.00";

  El objeto sale con `.data` de **64 B**, contenido byte a byte idéntico al
  extraído, y `sndlibauthor` GLOBAL OBJECT de **61 B**. GCC pone el relleno solo.
* **Prueba**: LIMPIA · **DOL OK**.
* **Control**: `Adamchan` → `Adamchao` → **DOL ROTO (`14603eb5ac7a`)**.

### 2.6 `smixvec` — `.../library/mix/smixvec.c` — **el que se daba por imposible**

El encargo decía que probablemente no se podía, porque es `.bss` y en este
proyecto está medido que el `.bss` «no se escribe a mano». **Sí se puede, y sale
a la primera.** La razón por la que aquí la veda no aplica:

* No es un hueco anónimo con `.size`: son **siete símbolos globales con nombre**,
  y los siete están **referenciados** (`slinkmix.c` los rellena en
  `SNDSYS_linkmaincpumixer()`, `snddrv.c` los llama). Nada que estripar.
* La CU de DWARF1 da la dirección de cada uno, así que el **orden no hay que
  adivinarlo**:

      MIXinitfn        0x804CC680        MIXplayfn       0x804CC690
      MIXrestorefn     0x804CC684        MIXstopfn       0x804CC694
      MIXaudioslicefn  0x804CC688        MIXsetpitchfn   0x804CC698
      MIXplayinitfn    0x804CC68C

* Estas unidades se compilan con `-x c++` (toda la biblioteca `snd` lo lleva).
  En C++ un puntero a función en ámbito de espacio de nombres **no es una
  definición tentativa**: GCC 2.95 lo emite directamente en `.bss`, no en
  `COMMON`, y **en orden de declaración**. Sin `-fno-common` ni nada.

* **Qué emite el original**: `.text` vacío + **28 B de `.bss`** en
  `0x804CC680..0x804CC69C`. Sin contenido en el fichero (`.bss` es NOBITS): lo
  que hay que reproducir es la dirección, el tamaño y el orden.
* **Fuente escrita**: los siete punteros en orden de declaración, con las firmas
  que ya usan `slinkmix.c` y `snddrv.c`. Objeto: `.bss` de 28 B con los siete
  símbolos GLOBAL en offsets 0, 4, 8, 0xC, 0x10, 0x14, 0x18.
* **Prueba**: LIMPIA · **DOL OK**.
* **Control**: los siete en **orden inverso** → **DOL ROTO (`4508963bdcbd`)**.

  Ese control corrige de paso una nota de la memoria del proyecto
  (`nfsmw-bss-y-huecos-estripados.md`: «el ORDEN de sus símbolos no tiene
  mando»). En `smixvec` **sí lo tiene**, y la palanca es el orden de declaración
  en el fuente. La nota valía para huecos anónimos; para símbolos C++ con nombre
  no.

---

## 3. Lo que tienes que aplicar tú

### `configure.py` — seis líneas, un `NonMatching` → `Matching` cada una

Están todas seguidas, en el bloque `"lib": "snd"` (líneas 1017-1023 del fichero
tal como está ahora):

| línea | cambio |
|---|---|
| 1017 | `Object(NonMatching, ".../library/mix/smixvec.c")` → `Object(Matching, …)` |
| 1018 | `Object(NonMatching, ".../library/cmn/sformat.c")` → `Object(Matching, …)` |
| 1019 | `Object(NonMatching, ".../library/cmn/soutputmap.cpp")` → `Object(Matching, …)` |
| 1021 | `Object(NonMatching, ".../library/cmn/author.cpp")` → `Object(Matching, …)` |
| 1022 | `Object(NonMatching, ".../coda/cmn/mpeghufftables.cpp")` → `Object(Matching, …)` |
| 1023 | `Object(NonMatching, ".../coda/cmn/mpegl3base.cpp")` → `Object(Matching, …)` |

No hace falta nada más: `tools/project.py` genera la arista de compilación en
cuanto el `.c`/`.cpp` existe, y `obj.completed` es lo único que decide si el
enlace toma el objeto nuestro o el extraído.

### `config/GOWE69/splits.txt` — **nada**

Los seis rangos ya están y ya son correctos. No hay que tocar un carácter.

### `config/GOWE69/keep.lst` — **nada**

Ninguno de los símbolos necesita entrada: los tres que tienen dato están
referenciados desde unidades que ya compilan de fuente (`ssdfx.c`, `ssysinit.c`,
`slinkmix.c`, `snddrv.c`).

### `config/GOWE69/symbols.txt` — **nada**

`gap_06_804513E1_data` (los 3 B de relleno de `author`) puede quedarse donde
está: es un símbolo del troceador sobre el objeto extraído y deja de usarse en
cuanto la unidad promociona.

### Atomicidad

**Ninguna depende de otra.** Medido: las seis dan `DOL OK` por separado y las
seis juntas también. Se pueden aplicar de una en una o de golpe.

### Acoplamiento con el encargo de `datos6` — una sola cosa que vigilar

Tres de las seis viven de que otra unidad **referencie** su símbolo. Dos de esas
referencias son inmunes porque vienen de unidades ya `Matching` (`ssdfx.c` para
`lbl_80412AE8`; `slinkmix.c` y `snddrv.c` para los `MIX*fn`). La tercera no:

* **`author` depende de `ssysinit.c`**, que hoy es `NonMatching` —el enlace toma
  su objeto extraído, y por eso mis medidas no las afecta que `datos6` lo esté
  editando ahora mismo—. Pero si `ssysinit` promociona en esta misma ventana, su
  fuente **tiene que seguir referenciando `sndlibauthor`**; hoy lo hace
  (`extern char sndlibauthor;` en la línea 14, `sndlibauthor = 'S';` en la 101).
  Si alguien quita esa referencia, `-strip-unused-data` se lleva los 64 B y el
  DOL rompe **en `author`, no en `ssysinit`**.

Con `sserver` no hay acoplamiento ninguno.

---

## 4. Ficheros nuevos

    src/Speed/Indep/Libs/snd/9/source/library/cmn/sformat.c                       1.012 B
    src/Speed/Indep/Libs/snd/9/source/library/cmn/soutputmap.cpp                    367 B
    src/Speed/Indep/Libs/snd/9/source/library/cmn/author.cpp                        723 B
    src/Speed/Indep/Libs/snd/9/source/library/mix/smixvec.c                         959 B
    src/Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mpeghufftables.cpp     583 B
    src/Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/mpegl3base.cpp        890 B

Los seis **LF puro**, como el resto de `src/`. Cuatro son comentario y dos
tienen una línea de código; `smixvec.c` tiene siete.

Los `.o` se han dejado en `build/GOWE69/src/…` para que puedas repetir
`trypromo.py` sin recompilar. Pesan 2-4 kB cada uno.

---

## 5. Lo que dejo para quien venga detrás

1. **`AT_low_pc = 0xFFFFFFFF` en el DWARF1 es «el enlazador me estripó».** Es
   una prueba directa, no una conjetura, de que un rango vacío de `splits.txt`
   no necesita fuente. Sirve para cualquier otro rango a cero que aparezca.
2. **La CU de DWARF1 da el orden y la dirección de los datos globales**
   (`AT_location`), y el `subscr_data` del `array_type` da el tamaño exacto
   (`author` = `char[61]`, no `char[64]`). Es más fiable que deducirlo del hueco.
3. **`-strip-unused-data` también estripa código y dato global sin referencias.**
   Cualquier control que dependa de emitir algo que nadie usa es un falso
   negativo. El control válido es un constructor estático.
4. Los seis rangos **no tenían símbolos LOCALES** (por eso `rangechk.py`, que
   sólo mira los `STT_FILE` y sus locales, no veía nada), pero **sí tenían
   símbolos GLOBALES** en los objetos extraídos. Buscar sólo en los locales
   escondió la mitad del problema.
