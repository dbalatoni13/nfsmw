# zBWare `DOL OK`: cinco causas de colocación, y ninguna era cantidad

    linked      13,75 % -> 17,39 %   (470 -> 474 ficheros)
    Game Code    0,88 % ->  5,19 %   (4 -> 8 ficheros)
    matched     98,10 % sin moverse
    main.dol    9619ba57c991 byte a byte

zBWare entra con zFEng, zFoundation y zRender. Es la segunda SourceList grande
que enlaza exacta.

## 0. El objeto engaña; el enlace es el juez

`datadiff` sobre el objeto decía **+128 B de exceso**. El enlace decía **−48**.
No es un error de la herramienta: `-strip-unused-data` se lleva los símbolos
muertos pero **CONSERVA el pool de la función descartada**, así que el `.rodata`
de un objeto y el del enlace no son la misma lista.

Todo lo que sigue se midió con dos herramientas nuevas del scratchpad, que
enlazan el proyecto con la unidad sustituida y comparan contra `main.dol`:
`dolwhere.py` (qué direcciones difieren y de qué símbolo son) y `dolrod.py`
(diff con resincronización de una sección, acotado a la ventana de la unidad).
La segunda es imprescindible: `difflib` sobre los 313 kB de `.rodata` no termina.

## 1. La pareja de pragmas mueve las vtables de sitio

`iallocator.h:6` tenía `#pragma interface` y `zBWare.cpp:1` `#pragma
implementation "iallocator.h"`. Probado con `ngccc -S` en las cuatro
combinaciones, en un fichero de 25 líneas:

| combinación | orden emitido |
|---|---|
| sin pragmas | `_vt.MemAlloc`, Tabla, Estado, **`_vt.IAlloc` al final**, sin `ICore` |
| sólo `interface` | `_vt.IAlloc` **no se emite** |
| **las dos** | `_vt.MemAlloc`, `_vt.ICore`, `_vt.IAlloc`, y luego las tablas |

La primera fila **es la disposición del objetivo**: `_vt.16bMemoryAllocator`
pronto y `_vt.Q32EA9Allocator10IAllocator` **el último de la unidad**, detrás de
`bCrcTable` y `statetable`. Con las dos, salían las tres juntas y por delante, y
además `_vt.Q32EA9Allocator14ICoreAllocator`, **que no existe en ninguna parte
del binario original**.

Se apaga **por unidad** (`BWARE_ALLOCATOR_IMPLEMENTATION`): quitar el `#pragma
interface` del árbol entero le cuesta a zEAXSound +52 B de `.text` y +56 B de
`.rodata`. Con la guarda, zEAXSound sale byte a byte igual que antes.

## 2. El pool dice qué hace la función, y de quién es

Tres correcciones, todas leídas del pool enlazado:

- `bOldATan`: el objetivo tiene `{0.0f, 1e-4f, 5e-11, 0.5f, 1.0f}` y luego
  `{0.0f, 256.0f}`. O sea que la guarda va sobre el **módulo al cuadrado** (por
  eso no hay constante negativa) y **el 256.0f no es suyo**: llama a `bFastATan`.
- El cuadruplete de `bSqrt` que yo había escrito en `bNormalize(bQuaternion)` es
  de **`bDistBetween(bVector4)`**: en el objetivo cae *detrás* de la cadena de
  bSlotPool, y bQuaternion.cpp se incluye *antes*.
- Faltaba el `{0.0f, 0.5f}` de **`bGetPolyArea`** (fórmula del cordón de zapato:
  acumulador a `0.0f`, el medio al final).

La atribución no es adivinanza: las reubicaciones de `.rela.text` contra los
`$LC` de `.rodata` dicen qué función referencia cada constante.

## 3. Cuatro bytes de cadena vacía costaban 308 B de `.text`

`attribuserinclude.h` se parsea antes que `bDebug.cpp`, y su `mString = ""`
internaba la cadena vacía 4 B antes de sitio (en el objetivo no nace hasta el
`OSPanic("", 0, "")` de `bBreak`, detrás de `"CODEINE"`). Cuatro bytes que
desplazaban **308 B de `.text`** por reubicación, en once funciones que no tenían
un solo byte mal. `ATTRIB_EMPTY_STRING` apunta al byte 8 del prefijo escrito a
mano, que ya es el terminador de `"GAMECUBE"`.

## 4. Dos reglas nuevas del estripado

**Un hueco escrito a mano con `.size` se estripa entero aunque mida 4 B.** Cuatro
`gap_*_data` de zBWare no tenían entrada en `keep.lst`; sin ellos `bDefaultSeed`
cae 4 B antes y con él toda la `.data` posterior de la imagen. La aritmética
`size & ~7` que teníamos medida **no cubre este caso**: aquí desaparece el
símbolo completo.

**El `.bss` no se puede escribir con un `asm()`.** cc1plus emite el `.bss`
**diferido**, al final de la unidad, así que un bloque escrito a mano cae
**delante de todas** las variables por mucho que se ponga al final del fichero
(probado en las dos posiciones: siempre en `+0x0000`). Los 12 B que cierran la
unidad tienen que ser **una variable más**, declarada detrás de `eARAMMM` y
renombrada:

    char _zBWareBssTail[12] asm("gap_07_8045A968_bss");

## 5. La fragilidad del `$LC`, y la red

Las cadenas muertas se salvan nombrando el `$LC<n>` que emite cc1plus. **n se
desplaza en cuanto se añade un literal antes en la unidad** — una constante de
coma flotante nueva basta. Cuando pasa, la entrada apunta a otra cosa, la cadena
se estripa y **el DOL rompe en silencio**, porque `keepchk` la sigue dando por
buena: el símbolo existe.

Me costó **tres iteraciones** en esta sesión. La red es `scripts/lcfix.py`: cada
entrada `$LC` de `keep.lst` lleva encima

    # @lc zBWare "SlotPool::GetAllocatedSlots"
    zBWare.o:$LC243

y `lcfix.py` la vuelve a resolver **por contenido** contra el objeto recién
compilado. Hay que pasarlo después de cada recompilación de una unidad con
entradas `@lc`.
