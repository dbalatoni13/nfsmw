# r35 / `spl` — los cuatro cortes de datos y los cuatro rangos `.text`

**Resultado**: `undlist.py --sl` **12 → 1**, `rangechk.py --text` **4 → 0**, enlace
base `DOL OK`, **cero rancias nuevas** en `keep.lst`. El único símbolo que queda
es `_overlay_end`, que la propuesta de la r34 deja explícitamente aparte (§5).

El árbol se ha devuelto a la línea base: `splits.txt`, `keep.lst` y `configure.py`
tienen el **md5 idéntico** al de partida, `undlist` vuelve a dar 12 y el enlace
base vuelve a dar `DOL OK` con 610 objetos. El paquete propuesto está en el
scratchpad (ver §7).

## 0. Verificación de partida

| medida | esperado | medido |
|---|---|---|
| `undlist.py --sl` | 12 | **12** (zAI 2, zFeOverlay 6, zGameplay 4; 431 errores) |
| `rangechk.py --text` | 4 | **4** |
| `keepchk.py` | 21 rancias | **21** |
| enlace base | `DOL OK` | **`DOL OK`**, 610 objetos, 2,5 s |

Aviso de lectura: `grep -c RANCIA` sobre `keepchk` da **22**, porque la línea de
resumen dice «21 RANCIAS». Cuenta `'  RANCIA'` con los dos espacios.

---

## 1. El mecanismo que NO estaba en la propuesta, y que decide los cuatro cortes

La propuesta de la r34 daba por hecho que mover el rango basta. **No basta**, y
es la lección general de la ronda:

> Un rango de `splits.txt` sólo decide qué contiene el objeto **extraído**. Si la
> unidad que recibe el rango se construye **de fuente nuestra** —`src/LibSN/*.s`
> ensamblado, o `src/libc/*.c` compilado— el objeto que se enlaza es el NUESTRO,
> y ahí el símbolo **sigue sin definirse**. Hay que escribir también la
> definición.

Se ve en `build.ninja`: `build\GOWE69\obj\...` son los extraídos por dtk, y
`build\GOWE69\src\...` los nuestros. `metrotrk.o`, `crt0.o`, `impure.o` y
`sn_buf.o` son de los segundos.

El corte 2 lo demostró en carne: con el rango puesto y sin tocar la fuente, el
enlace base pasó de `DOL OK` a **`ENLACE FALLA` con 18 errores** y `undlist` subió
de 12 a **92 símbolos en 29 SourceLists**. El `.s` que dtk genera en
`build/GOWE69/asm/LibSN/metrotrk.s` **sí** traía el bloque correcto; lo que se
enlaza es `src/LibSN/metrotrk.s`, que no.

Emitir ese bloque es legítimo: es **dato** (`.skip`, `.string`), no instrucciones.

## 2. La segunda trampa: `keep.lst` en tres de los cuatro cortes

Mover una frontera **renombra el relleno de objeto**, no de nombre: el hueco
conserva su nombre (`gap_06_8041D584_data`) y **cambia de `.o`**. La entrada de
`keep.lst` queda sin casar, `-strip-unused-data` se lleva los bytes en silencio y
el DOL sale corto **sin un solo error**.

El corte 1 lo enseñó limpio: rango correcto, `undlist` bajando de 12 a 8, y aun
así **`DOL ROTO`**. Con las dos entradas movidas a `zLua.o`: `DOL OK`.

Cinco entradas de `keep.lst` cambian de objeto en el paquete:

    zGameplay.o:gap_06_8041D584_data   -> zLua.o:...            (corte 1)
    zGameplay.o:gap_06_8041D594_data   -> zLua.o:...            (corte 1)
    zFeOverlay.o:gap_06_804394AC_data  -> crt0.o:...            (corte 3)
    zFeOverlay.o:gap_06_804394C8_data  -> crt0.o:...            (corte 3)
    zAI.o:gap_08_804FEDC8_sdata        -> auto_08_804FEDC8_sdata.o:pad_08_804FEDC8_sdata

La última cambia además de **nombre**: al quedar sin dueño, dtk se lleva el hueco
a un split automático y el símbolo pasa de `gap_…` (global) a `pad_…` (local).

## 3. Los cuatro cortes, uno a uno

Todos con enlace base entre medias, y todos cerrados con `DOL OK` y cero rancias
nuevas.

### Corte 1 — `.data` zGameplay/zLua (4 símbolos, 383 refs)

    zGameplay .data end   0x8041D5A4 -> 0x8041D580
    zLua      .data start 0x8041D5A4 -> 0x8041D580

`symbols.txt` lo cierra sin ambigüedad: el último local de zGameplay
(`kObjectTemplateKey.37663`, `0x8041D524` + `0x18`) acaba en **`0x8041D53C`**, y
**`0x8041D580` es exactamente el inicio de `_10LuaRuntime.mObj`**. Frontera de
símbolo, y `0x580 mod 8 = 0`. Más `keep.lst` (§2).

Detalle que corrige la propuesta: el objetivo **no** tiene las cuatro estáticas
contiguas. Están en `0x8041D580`, `…590`, `…59C`, `…5A0`, con 12 y 8 B de relleno
entre medias — que son justo los dos `gap_` que hay que mover.

### Corte 2 — `.bss` de zFeOverlay a metrotrk (3 símbolos)

    zFeOverlay .bss end 0x804B4DE8 -> 0x804AC6C0
    LibSN/metrotrk.c  .bss 0x804AC6C0-0x804B4DE8   (nuevo, 34.600 B)

Más el bloque `.bss` al final de `src/LibSN/metrotrk.s`, copiado tal cual lo emite
dtk (§1).

La veda de ciclo **no aplicaba**: zFeOverlay no tiene `.text`, tiene `.over`. Y el
`.bss` ya contradice hoy al `.text` sin romper nada (`itoa.c` tiene `.text` antes
que zFeOverlay y `.bss` después).

### Corte 3 — `.data` de zFeOverlay a crt0 (2 símbolos)

    zFeOverlay .data end 0x804394D0 -> 0x80439490
    LibSN/crt0.s .data 0x80439490-0x804394D0       (nuevo, 64 B)

Más el bloque `.data` al final de `src/LibSN/crt0.s`. El contenido confirma la
atribución por sí solo: son las dos cadenas de LibSN,
`"Waiting for SN Debugger...\n"` y `"<< libsn version %d >>\n"`.

### Corte 4 — `.sdata` de zAI: **no era una unidad, eran dos**

Éste era el único con el destino sin decidir, y **la propuesta se quedaba corta**:
no basta con `impure.c`. Los 16 B son de **dos** unidades.

Los tres punteros del rango apuntan a locales, y los locales **sí** llevan
`STT_FILE` en el ELF:

    impure_data   0x804397E8   772 B   -> impure.c
    _sn_IO_buf    0x80439AEC    80 B   -> sn_buf.cpp
    _sn_iobf      0x804B4FE0   960 B   -> fopen.c

Un global (`_impure_ptr`) que apunta a un local de `impure.c` es de `impure.c`; el
que apunta a un local de `sn_buf.cpp` es de `sn_buf.cpp`. De ahí:

    zAI        .sdata 0x804FEDC0-0x804FEDD0   QUITAR
    libc/impure.c   .sdata 0x804FEDC0-0x804FEDC4   (nuevo)
    libc/sn_buf.cpp .sdata 0x804FEDC4-0x804FEDC8   (nuevo)

y los 8 B de cola (`0x804FEDC8`-`0x804FEDD0`) sin dueño, que dtk se lleva a
`auto_08_804FEDC8_sdata.o`.

Las definiciones, con el idioma que el propio proyecto ya usa en `sn_buf.cpp`
(`__asm__("<nombre>_<dir>")` para alcanzar los símbolos del split automático):

    /* src/libc/impure.c */
    extern struct _reent impure_data __asm__("impure_data_804397E8");
    struct _reent *_impure_ptr = &impure_data;

    /* src/libc/sn_buf.cpp, dentro del extern "C" */
    extern struct __sbuf _sn_IO_buf[10] __asm__("_sn_IO_buf_80439AEC");
    struct __sbuf *_sn_IO_buf_ptr = _sn_IO_buf;

Nota de contexto: el rango `.text` que `splits.txt` llama `libc/impure.c`
(`0x80312C10`, la función `__errno`) el ELF se lo da a **`errno.c`**. El
`impure.c` original es una TU **sin `.text`**: sólo `.data` y `.sdata`.

## 4. VEDA ROTA: un rango que empieza en `4 mod 8` **sí** se expresa

La r33 vetó los rangos que empiezan en `4 mod 8`. El corte 4 los usa:
`libc/sn_buf.cpp .sdata start:0x804FEDC4` (`0x4 mod 8`). dtk emite

    .section .sdata, "wa"
    .balign 4
    # .sdata:0x0 | 0x804FEDC4 | size: 0x4
    .obj _sn_IO_buf_ptr, global

y el enlace base da **`DOL OK`**. Es decir: dtk **ajusta el `.balign` al rango**, y
lo que la r33 midió tuvo que ser otra cosa. Los rangos `.text` del §5 usan otros
tres (`0x8039BED4`, `0x8039BF2C`, `0x80318E94`), los tres con `DOL OK`.

## 5. Los cuatro rangos `.text`: partidos, y dos estaban mal nombrados

`rangechk --text` pasa de **4 a 0**. Las marcas `gcc2_compiled.` del ELF dan las
fronteras exactas, y de paso corrigen el nombre de la unidad **principal** en dos
de los cuatro casos:

| `splits.txt` decía | el ELF dice | se partió en |
|---|---|---|
| `libc/wcstring.c` `0x8039BE6C-0x8039BF54` | **`wcscat.c`** | `wcscat.c` + `wcscpy.c` `0x8039BED4` + `wcslen.c` `0x8039BF2C` |
| `libc/locale.c` `0x80318E6C-0x80318EC4` | **`isdigit.c`** | `isdigit.c` + `isspace.c` `0x80318E80` + `locale.c` `0x80318E94` |
| `base.cpp` `0x80387A74-0x80387C30` | **`allocator.cpp`** | `allocator.cpp` + `shpdestroy.cpp` `0x80387B90` |
| `ppc2D2.tmp` `0x80311C50-0x803125E0` | `ppc2D2.tmp` | `ppc2D2.tmp` + `FSasync.c` `0x80311DA8` |

Los tres primeros eran unidades `Matching` **ya en el DOL como nuestras**, así que
partirlas tenía que conservar `DOL OK` — y lo conserva: la fuente se parte en
ficheros nuevos, `configure.py` pasa de un `Object` a dos o tres **en el mismo
sitio de la lista** (esa lista ES el orden de enlace), y el enlace base sube de
610 a **617 objetos** con `DOL OK`.

**El que paga es `ppc2D2.tmp`**, y bastante. Su fuente documenta que los siete
alias de 64 bits de SN (`__shr2u`, `__div2i`, …, 28 B) **no se pueden escribir en
C** porque GCC 2.9 no hace llamadas de cola. Mientras el rango fuera uno solo,
esos 28 B bloqueaban los **2.104 B de `FSasync.c`**. Partido:

    ppc2D2.o    nuestro   300 B   rango   344 B   (los 7 alias, imposibles)
    FSasync.o   nuestro  2108 B   rango  2104 B   (a 4 B)

`FSasync.c` pasa a ser una unidad que puede promocionar sola. Su entrada
`Object(NonMatching, "FSasync.c")` **ya existía** en `configure.py` sin `source=`,
y `src/LibSN/FSasync.c` era un fichero **vacío**: sólo le faltaba el rango.

## 6. Lo que queda

- **`_overlay_end`** (`.over:0x803C8B50`, 8 B, 6 refs desde `zMisc.o`) — es un
  símbolo de frontera de overlay, del tipo que define el `ldscript`. Sigue aparte,
  como decía la propuesta.
- Las cuatro unidades del frente de datos (`zAI`, `zGameplay`, `zLua`,
  `zFeOverlay`) pasan de **`ENLACE FALLA` a `DOL ROTO`**: el frente de enlace queda
  cerrado y lo que las bloquea ya es sólo dato, que es el trabajo del §4 del brief.
- `rangechk` sin argumentos sigue señalando que el rango de `metrotrk.c` se traga
  `pro2D4.tmp` (9 símbolos), `fil2D3.tmp` (5) y parte de `ppc2D2.tmp` (2). No
  salen en `--text` porque son `.tmp` sin marca `gcc2_compiled.`; el mismo
  mecanismo del §5 debería servir.

## 7. El paquete

En el scratchpad, `c35spl_paquete/`:

    config/GOWE69/splits.txt      13 rangos tocados (4 cortes de dato + 4 de .text)
    config/GOWE69/keep.lst        5 entradas cambian de objeto
    configure.py                  5 Object -> 12
    src/LibSN/metrotrk.s          + bloque .bss   (dato)
    src/LibSN/crt0.s              + bloque .data  (dato)
    src/LibSN/ppc2D2.c            recortado a 108 lineas
    src/LibSN/FSasync.c           la mitad que sale de ppc2D2.c
    src/libc/impure.c             + definicion de _impure_ptr
    src/libc/sn_buf.cpp           + definicion de _sn_IO_buf_ptr
    src/libc/locale.c             recortado a _localeconv_r + localeconv
    src/libc/{isdigit,isspace,wcscat,wcscpy,wcslen}.c    nuevos
    src/{allocator,shpdestroy}.cpp                       nuevos
    BORRAR.txt        src/libc/wcstring.c y src/base.cpp los sustituye el paquete

Aviso al aplicarlo: **`splits.txt` y las fuentes van juntos**. Con el `splits.txt`
del paquete y las fuentes viejas el enlace falla por símbolo indefinido; con las
fuentes del paquete y el `splits.txt` viejo falla por **símbolo definido dos
veces**. Y `dtk dol split` **reescribe `splits.txt`** (normaliza el orden de los
bloques), así que el fichero del paquete ya es su forma canónica.

Herramienta del ciclo: `c35spl_ed.py` (parte siempre de la copia, aplica una lista
de operaciones `add`/`del`/`start`/`end`/`ren`/`new`, trocea, reconfigura, compara
las rancias contra la base y hace el enlace base).
