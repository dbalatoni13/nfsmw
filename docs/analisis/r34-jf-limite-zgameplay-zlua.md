# El límite `.data` entre zGameplay y zLua está 36 B alto

**Estado: medido y sin aplicar.** Cambiar `splits.txt` obliga a re-extraer, y hay
cuatro agentes en vuelo. Va para la ventana.

## Lo que bloquea

`undlist.py zGameplay` da **391 errores, 8 símbolos**; los cuatro gordos son
estáticos de clase de Lua, con 224 y 139 referencias:

    _10LuaRuntime.mObj        224x   zGameplay.o, zLua.o, zMain.o
    _13LuaPostOffice.fObj     139x   zGameplay.o, zLua.o, zMain.o
    _13LuaAttributes.fObj      13x   zLua.o, zSim.o
    _10LuaBindery.fObj          7x   zLua.o, zSim.o

Los cuatro **existen** en el árbol (`LuaRuntime.cpp:18`, `LuaPostOffice.cpp:11`,
`LuaBindery.cpp:441`, `LuaAttributes.cpp:246`), todos sin inicializador, y los
cuatro `.cpp` están en `zLua.cpp` (líneas 24-30). Nuestro compilador los emite
**COMMON**, y el `ldscript` manda `*(COMMON)` a `.bss`.

## Por qué el objetivo los pone en zGameplay: no los pone

`symbols.txt` los sitúa en `.data:0x8041D580 / 590 / 59C / 5A0`, y el rango de
zGameplay es `.data 0x8041D2B8-0x8041D5A4`. Por eso el zGameplay **extraído** los
define y el nuestro no. Pero el rango es el que está mal:

    0x8041D580  _10LuaRuntime.mObj        LuaRuntime.cpp:18      (zLua, línea 24)
    0x8041D590  _13LuaPostOffice.fObj     LuaPostOffice.cpp:11   (zLua, línea 26)
    0x8041D59C  _10LuaBindery.fObj        LuaBindery.cpp:441     (zLua, línea 28)
    0x8041D5A0  _13LuaAttributes.fObj     LuaAttributes.cpp:246  (zLua, línea 30)
    0x8041D5A4  accessorTable.11371       LuaAttributes.cpp:294  (zLua, línea 30)  <-- inicio de zLua

**El orden de las cuatro es exactamente el de `zLua.cpp`.** Y la de arriba es la
prueba que cierra el caso: `LuaAttributes::fObj` (línea 246) y
`accessorTable.11371` (línea **294**, estático local de una función del **mismo
fichero**) están a 4 B, y `splits.txt` mete el límite **entre los dos**. Un
límite de unidad no puede caer dentro del `.data` de un solo `.cpp`.

## La corrección

    zGameplay  .data end:   0x8041D5A4 -> 0x8041D580   (748 B -> 712 B)
    zLua       .data start: 0x8041D5A4 -> 0x8041D580   (584 B -> 620 B)

Y con eso el zLua **extraído** pasa a definir los cuatro, así que dejan de
bloquear la promoción de zGameplay sin escribir una línea de fuente.

## Comprobación pendiente en la ventana

1. `.data` de zGameplay debe pasar de «objetivo 748 / nuestro 560» a
   «objetivo 712 / nuestro 560»: el hueco baja de 188 a 152 B.
2. Re-extraer y confirmar que `undlist.py zGameplay` pierde los cuatro símbolos.
3. Enlace base + DOL: el límite es una partición, no debería mover un byte.
4. Ojo al `pad_`/`gap_` de `keep.lst` en la frontera movida (ver `keepchk.py`).

## PROBADO POR CONSTRUCCION (no era inferencia)

Dos pruebas nuevas, las dos independientes de `splits.txt`.

### 1. El ELF original trae 577 `STT_FILE`, y son las SourceLists

`orig/GOWE69/NFSMWRELEASE.ELF` conserva el `symtab`, y en un `symtab` cada
`STT_FILE` va seguido de los simbolos **locales** de esa unidad de traduccion.
Los nombres son `zGameplay.cpp`, `zLua.cpp`, `zMain.cpp`...: **el original se
compilaba con estas mismas SourceLists**. Herramienta nueva: `scripts/rangechk.py`.

    zGameplay  .data  locales 0x8041D31C-0x8041D53C   declarado 0x8041D2B8-0x8041D5A4
    zLua       .data  locales 0x8041D5A4-0x8041D72C   declarado 0x8041D5A4-0x8041D7EC

El ultimo local de zGameplay acaba en `0x8041D53C` y el primero de zLua esta en
`0x8041D5A4`: las cuatro estaticas caen en tierra de nadie. Los GLOBALES no
llevan atribucion --en un `symtab` van todos detras de los locales, asi que el
`STT_FILE` que les toca es el del ultimo fichero y miente: a estas cuatro les
sale `OSSemaphore.c`--. O sea que el ELF acota el limite pero no lo fija.

### 2. El experimento que si lo fija

Las cuatro estaban sin inicializador, y GCC 2.9 las emite **COMMON** (que el
`ldscript` manda a `.bss`). El objetivo las tiene en `.data`, o sea que el
original las inicializaba. Puestas a `= nullptr` y recompilada zLua sola:

    nuestro .data de zLua          objetivo con el limite en 0x8041D580
    +0000  LuaRuntime.mObj         +0000 (0x8041D580)  LuaRuntime.mObj
    +0004  LuaPostOffice.fObj      +0010 (0x8041D590)  LuaPostOffice.fObj
    +0008  LuaBindery.fObj         +001C (0x8041D59C)  LuaBindery.fObj
    +000C  LuaAttributes.fObj      +0020 (0x8041D5A0)  LuaAttributes.fObj
    +0010  accessorTable  192 B    +0024 (0x8041D5A4)  accessorTable  192 B

**Mismo orden, y `accessorTable` mide 192 B en los dos.** Las cuatro encabezan
la `.data` de zLua y `accessorTable` va detras: es exactamente lo que el
objetivo tiene en `0x8041D580`. Lo unico que nos falta son los 12 B de detras de
`mObj` y los 8 B de detras de `LuaPostOffice::fObj`.

El `= nullptr` esta **aplicado y commiteado**: es correcto por si solo (el
original las tiene en `.data`), y sube la `.data` de zLua de 280 a 296 B.
Lo que sigue pendiente de la ventana es solo mover el limite.
