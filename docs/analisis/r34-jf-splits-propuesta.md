# Los 12 símbolos indefinidos que quedan son TODOS de `splits.txt`

Al empezar la ronda había **42 símbolos indefinidos** repartidos en 7 SourceLists.
Escribiendo fuente se han cerrado 30. **Los 12 que quedan no tienen arreglo de
fuente**: son rangos de `splits.txt` que atribuyen a una unidad datos de otra.

Esto es la propuesta, con la evidencia de cada corte. **Sin aplicar**: tocar
`splits.txt` obliga a re-extraer, y eso va en la ventana, de uno en uno y con el
enlace base entre medias.

## Cómo está medida cada atribución

Dos fuentes independientes, ninguna de ellas `splits.txt`:

1. **`scripts/rangechk.py`** — el ELF original conserva el `symtab` con **577
   `STT_FILE`**, y sus nombres SON las SourceLists (`zGameplay.cpp`, `zLua.cpp`,
   `zMain.cpp`…): el original se compilaba con esta misma estructura. Cada
   `STT_FILE` va seguido de los **locales** de su unidad, así que el ELF dice de
   quién es cada estático sin adivinar. Los **globales** no llevan atribución
   —van todos detrás de los locales— y son justo los que caen en los huecos.
2. **Quién referencia cada símbolo**, contado sobre los objetos extraídos. Un
   `lbl_` dentro del rango de X al que **sólo** apunta Y es de Y.

---

## 1. `.data` de zGameplay / zLua — **PROBADO POR CONSTRUCCIÓN**

    zGameplay  .data end:   0x8041D5A4 -> 0x8041D580   (748 B -> 712 B)
    zLua       .data start: 0x8041D5A4 -> 0x8041D580   (584 B -> 620 B)

Desbloquea **4 símbolos con 383 referencias** (`LuaRuntime::mObj` 224,
`LuaPostOffice::fObj` 139, `LuaAttributes::fObj` 13, `LuaBindery::fObj` 7).

`rangechk` acota: los locales de zGameplay acaban en `0x8041D53C` y los de zLua
empiezan en `0x8041D5A4`; las cuatro están en tierra de nadie. Lo que lo cierra
es el experimento: puestas a `= nullptr` (ya commiteado; sin inicializador GCC
2.9 las emite COMMON y el `ldscript` las manda a `.bss`, pero el objetivo las
tiene en `.data`), nuestra `.data` de zLua queda

    +0000 LuaRuntime.mObj · +0004 LuaPostOffice.fObj · +0008 LuaBindery.fObj
    +000C LuaAttributes.fObj · +0010 accessorTable (192 B)

y el objetivo, desde `0x8041D580`: las mismas cuatro en el mismo orden y
`accessorTable.11371` detrás, también de 192 B. Es el orden de los `#include` de
`zLua.cpp` (líneas 24, 26, 28, 30).

## 2. `.bss` de zFeOverlay — la cola es de metrotrk

    zFeOverlay      .bss end:  0x804B4DE8 -> 0x804AC6C0   (-34.600 B)
    LibSN/metrotrk  .bss:      0x804AC6C0-0x804B4DE8      (rango NUEVO)

Desbloquea 3 símbolos. `metrotrk.c` hoy sólo tiene rango `.text` en `splits.txt`,
así que sus 34 kB de `.bss` no tienen dónde ir y se los come zFeOverlay.

Contado símbolo a símbolo en la cola declarada:

    0x804AB0D4    784  TopOrFullScreenRide          <- zFeOverlay
    0x804AB3E4    452  gCarCustomizeManager         <- zFeOverlay
    0x804AB5A8      4  TopOrFullScreenLoading       <- zFeOverlay
    0x804AB5AC      4  gPlayerNum                   <- zFeOverlay
    0x804AB5B0     44  CustomizeHUDTexPackResources <- zFeOverlay
    0x804AB5DC    220  CustomizeHUDTexTextureRes.   <- zFeOverlay
    0x804AC6C0     32  lbl_804AC6C0                 <- metrotrk, SOLO metrotrk
    0x804AC6E0  34564  lbl_804AC6E0                 <- metrotrk, SOLO metrotrk
    0x804B4DE4      4  lbl_804B4DE4                 <- metrotrk, SOLO metrotrk

El corte cae en cualquier punto de `0x804AB6B8`-`0x804AC6C0` (4.104 B sin ningún
símbolo). `0x804AC6C0` es el extremo conservador. Y el final actual coincide con
`NOA_ProgramExc` (`0x804B4DE8`, primer local del TU del depurador,
`ppc2D2.tmp`), que es exactamente el otro extremo del bloque de metrotrk.

## 3. `.data` de zFeOverlay — los últimos 64 B son de crt0

    zFeOverlay  .data end: 0x804394D0 -> 0x80439490   (-64 B)
    crt0        .data:     0x80439490-0x804394D0      (rango NUEVO)

Desbloquea 2 símbolos:

    0x80439490  28  lbl_80439490  <- crt0, SOLO crt0
    0x804394B0  24  lbl_804394B0  <- crt0, SOLO crt0

## 4. `.sdata` de zAI — 16 B de libc

    zAI  .sdata: 0x804FEDC0-0x804FEDD0   -> QUITAR entero

Desbloquea 2 símbolos con 20 referencias (`_impure_ptr` ×19 desde `atexit.o`,
`fclose.o`, `fflush.o`; `_sn_IO_buf_ptr` desde `fopen.o`).

El rango son **16 B que empiezan exactamente en `__SDATA_START__`** y contienen
dos globales de libc. `src/libc/impure.c` declara `extern struct _reent
*_impure_ptr;` y no lo define; su entrada en `splits.txt` sólo tiene `.text`
(8 B, la función `__errno`). Los siguientes locales de la zona (`0x804FEDD0`+)
son de `vfprintf.c`.

**Aquí falta decidir el destino**, y es lo único de los cuatro que no está
cerrado: `_impure_ptr` es de `impure.c` en newlib, `_sn_IO_buf_ptr` parece de
`fopen.c` (cuyo local `_sn_iobf`, 960 B, está en `0x804B4FE0`). Hay que
comprobar si basta con definirlo en `src/libc/impure.c` y darle a esa unidad un
rango `.sdata`.

## 5. `_overlay_end` — no es lo mismo

`.over:0x803C8B50`, 8 B, lo referencia `zMisc.o` 6 veces. Es un símbolo de
frontera de overlay, del tipo que normalmente define el `ldscript`. Va aparte.

---

## Comprobación, la misma para los cuatro

1. `datacmp.py` de las dos unidades del corte: el tamaño «objetivo» tiene que
   moverse justo lo que dice la tabla.
2. Re-extraer y `undlist.py --sl`: los símbolos de esa fila desaparecen.
3. **Enlace base** (2,6 s) entre corte y corte, nunca dos a la vez.
4. DOL: un corte es una partición, no debería mover un byte. Y **mirar la marca
   de tiempo de `main.dol`**, no sólo el sha1.
5. `keepchk.py`: mover una frontera renombra el relleno `gap_`/`pad_` y deja la
   entrada de `keep.lst` sin casar, y entonces `-strip-unused-data` se lleva los
   bytes en silencio.
