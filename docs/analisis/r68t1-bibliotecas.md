# r68 tanda 1 — lote bibliotecas (L1)

HEAD a5c438bd. DOL de referencia 9619ba57c9919f95f7f2ac951a2166a3517f91e3. Todas las unidades
del lote están promocionadas: el enlace usa nuestro objeto, así que el test que decide es el DOL.
No he tocado build/, config/, configure.py ni keep.lst, y no he hecho commit.

**Resultado: 9 bloques de 9 retirados.** Ocho son rellenos o marcadores que ya ponen el
compilador, el ensamblador o el enlazador (K10). Se borran sin sustituto y en su lugar queda una
nota r68. El noveno, `maddeca.cpp:21`, pasa a `static const int zigzag[64] = {...}`. keep.lst no
cambia.

## 1. Bloque a bloque

| bloque | qué era | decisión | C que lo sustituye | evidencia | medida |
|---|---|---|---|---|---|
| `vp6/.../cmn/allocator.cpp:41` | asm `.skip 4` en `.sdata` (`gap_08_804FF5A4_sdata`) | **retirado** | nada (nota r68) | En el DWARF de allocator.cpp solo aparece `gAllocator`. En el DOL, 0x804FF5A4..A8 = `00000000`. La `.sdata` de 8 B y align 8 ya sale de `aligned(8)`, porque el ensamblador redondea la sección | ALLOC igual a la base; relink 9619ba57 |
| `vp6/.../cmn/TokenEntropy.c:13` | asm `.skip 2` en `.rodata` (`gap_05_8041227E_rodata`) | **retirado** | nada (nota r68) | `VP6_DcUpdateProbs` ocupa 22 B (0x80412268..7E) y GCC alinea a 4 el array de char siguiente. En el DOL, 0x8041227E = `0000` y `ScanBandUpdateProbs` empieza en 0x80412280 (`ff84849f…`) | ALLOC igual; relink 9619ba57 |
| `vp6/.../cmn/DeInterlace.c:3` | etiqueta GLOBAL `gcc2_compiled._80353734`, 0 B | **retirado** | nada (nota r68) | symbols.txt: `gcc2_compiled.` en 0x80353734 con scope:local, y cc1 ya lo emite local | ALLOC igual; relink 9619ba57 |
| `vp6/.../cmn/clamp.c:3` | etiqueta GLOBAL `gcc2_compiled._8034F154`, 0 B | **retirado** | nada (nota r68) | symbols.txt:16435, scope:local | ALLOC igual; relink 9619ba57 |
| `vp6/.../gc/criticalpath.c:418` | asm `.skip 4` al final de `.rodata` (`gap_05_80412774_rodata`, de la r48) | **retirado** | nada (nota r68); el asm VP6 de instrucciones no se toca | En el DWARF, `loMaskTbl_VP60` acaba en 0x80412774. En el DOL, 0x80412774 = `00000000` y la `.rodata` de la unidad siguiente arranca en 0x80412778, múltiplo de 8: es el relleno del enlace | `.rodata` pasa de 636 a 632 B, resto de ALLOC igual; relink 9619ba57 |
| `realcore/.../gc/inittmr.cpp:38` | asm `.byte 0,0,0,0,0,0` en `.sdata` detrás de `bIsTimerInited` | **retirado** | nada (nota r68) | En el DOL, 0x804FF628..30 = 0. Con el `.align 3` de la línea 36 la sección ya es align 8 y el ensamblador la redondea a 8 B | `.sdata` sigue en 8 B align 8, ALLOC igual; relink 9619ba57 |
| `realcore/.../cmn/abortmsg.cpp:7` | etiqueta GLOBAL `gcc2_compiled._80399588`, 0 B | **retirado** | nada (nota r68, integrada en el comentario que ya había) | symbols.txt:18108, scope:local | ALLOC igual; relink 9619ba57 |
| `egami/.../cmn/maddeca.cpp:21` | `extern const int zigzag[64];` más un `__asm__` con la etiqueta y 64 `.long` en `.rodata` | **retirado** | `static const int zigzag[64] = {0, 8, 1, 2, 9, 16, 24, 17, …, 46, 39, 47, 54, 61, 62, 55, 63};` | En `symbols/mw_dwarfdump.nothpp:2556883`: `static const int zigzag[64]; // size: 0x100, address: 0x80411358`. symbols.txt:32668 lo da scope:local. Los 64 enteros se leyeron en el DOL, en 0x80411358, y coinciden uno a uno | `.rodata` de 256 B idéntica a la del asm; relink 9619ba57 |
| `libc/fflush.c:4` | etiqueta GLOBAL `gcc2_compiled._80312CD0`, 0 B | **retirado** | nada (nota r68) | symbols.txt:15265, scope:local, en la misma dirección que `fflush` | ALLOC igual; relink 9619ba57 |

No queda ningún bloque en asm dentro de este lote. `inittmr.cpp:36` (1 B delante de la bandera)
no es de este lote y sigue como estaba. `vputil.c` y `uoptsystemdependant.c` tampoco lo son: su
receta usa `section(".bss")`, que el criterio prohíbe.

**La nota de maddeca sobre el orden externo/estático, medida.** La nota vieja decía que GCC 2.9
emite los datos de enlace externo antes que los estáticos. Aquí no afecta, porque zigzag es el
único dato definido de la unidad; el pool de vp6/postproc que citaba ya no está en este fichero.
- Variante `extern const int zigzag[64] = {...}` (símbolo GLOBAL): DOL 9619ba57 OK.
- Se elige `static` porque así lo dicen el DWARF y symbols.txt.
- Un `const int zigzag[64]` a secas no sirve como variante externa: en C++ una `const` de ámbito
  de espacio de nombres tiene enlace interno y da el mismo objeto que `static`, byte a byte
  (sha1 0272d0f0…).

Ningún fichero del lote usa `__LINE__`, `BNEW` ni assert (grep). Aun así no hace falta conservar
la cuenta de líneas. Los finales de línea se mantienen: allocator.cpp, TokenEntropy.c,
inittmr.cpp y abortmsg.cpp siguen en CRLF puro; DeInterlace.c, clamp.c, criticalpath.c,
maddeca.cpp y fflush.c siguen en LF puro.

## 2. Medidas

El arnés está en `scratchpad/bibliotecas68t1/`:
- `t.py` compila con `compila()` de cmphead.py, con cwd en la raíz del repo y hacia un directorio
  propio;
- `fin_relinks.sh` llama a `scratchpad/lote5_68/relink.py`;
- `ctl.py` hace las variantes de control dentro del árbol, para que `__FILE__` no cambie, y
  restaura cada fichero comprobando su sha1.

**Base**
- `relink.py` sin sustituciones: **DOL 9619ba57 OK**.
- Las 9 unidades compiladas desde el árbol sin tocar (= HEAD) tienen las secciones ALLOC idénticas
  a las de los objetos `build/GOWE69/src/...` que usa el enlace. Sus sha1:

  | unidad | sha1 del objeto base |
  |---|---|
  | allocator | 554fb12c |
  | TokenEntropy | fd102cd6 |
  | DeInterlace | d539dc7a |
  | clamp | c8bfc079 |
  | criticalpath | 52411edb |
  | inittmr | b3eb7d74 |
  | abortmsg | 22d3c607 |
  | maddeca | 61013e8f |
  | fflush | 748ab2d4 |

- Relink con las 9 sustituidas por esos objetos base: **DOL 9619ba57 OK**. El arnés sustituye y
  reproduce.

**Final**, un relink por fichero con solo esa unidad sustituida:

| unidad | sha1 del .o final | relink |
|---|---|---|
| allocator | e4d150a3134a5e1bd0ca924d5df2af3a9f7a8c31 | DOL 9619ba57 OK |
| TokenEntropy | 86de5cf396d1fcb059249bb335e9d741dd2c56de | DOL 9619ba57 OK |
| DeInterlace | 94beff8698e187af0090d042b06c22a83802179f | DOL 9619ba57 OK |
| clamp | 927c4140862de0724b16af54d98c01cd02048a2b | DOL 9619ba57 OK |
| criticalpath | 243e1850d5f4824a9462ca5c3d970ec631c0000e | DOL 9619ba57 OK |
| inittmr | 47e0a099dd0c53de0fd15d27b05600160bdeb896 | DOL 9619ba57 OK |
| abortmsg | 03098cfa5d33911a4d84193c1f5cd2f86e529ff9 | DOL 9619ba57 OK |
| maddeca | 0272d0f072083fe63c3b1a24f842e7d0550275cf | DOL 9619ba57 OK |
| fflush | 0ff502516625a1ad0f0a1327e35fb4ec1bf64ed5 | DOL 9619ba57 OK |
| **las 9 juntas** (keep por defecto) | — | **DOL 9619ba57 OK** |
| **las 9 juntas** con `keep=scratchpad/bibliotecas68t1/keep.lst` | — | **DOL 9619ba57 OK** |

**Sellado.** Al terminar recompilé las 9 desde el árbol y los 9 sha1 coinciden con los de la tabla.

**Controles que pueden fallar.** No hay receta con keep. Estos controles demuestran que el objeto
sustituido de cada unidad llega de verdad al DOL y que el arnés ve un cambio en la sección que
toca cada bloque:

| control | variante | resultado |
|---|---|---|
| pad_allocator | `.skip 12` en `.sdata` | **ROTO**: `.sdata` del DOL 2816→2848 B y `.text` distinto |
| pad_TokenEntropy | `.skip 6` delante de `ScanBandUpdateProbs` | **ROTO**: la sección 804FEDC0 se desplaza 0x20 |
| pad_criticalpath | `.skip 12` al final de `.rodata` | **ROTO**: el mismo desplazamiento |
| pad_inittmr | 14 B detrás de la bandera | **ROTO**: `.sdata` 2816→2848 B |
| txt_DeInterlace / txt_clamp / txt_abortmsg / txt_fflush | +4 B de `.text` | **ROTO** los cuatro |
| malo (maddeca) | último valor de zigzag 63→64 | **ROTO**: DOL 3028025abe3613ac6547606c7cb6912dfacdde91, primera diferencia en 0x80411457 (zigzag), 1 palabra |
| extern (maddeca) | `extern const int zigzag[64] = {...}` | OK (informativo, ver §1) |

## 3. keep.lst

`scratchpad/bibliotecas68t1/keep.diff`: **sin cambios**, cero líneas `-` y cero `+`.
- Ninguno de los nombres del lote aparece en keep.lst: grep de `gcc2_compiled`,
  `gap_08_804FF5A4`, `gap_05_8041227E`, `gap_05_80412774`, `gap_08_804FF62A` y `zigzag`, con
  0 aciertos.
- Ninguna sustitución necesita entrada: zigzag está referenciado por `madvlcdecode`.
- El enlace final se hizo con una copia byte-idéntica (sha1 b0646b0412ddf53bbd857b95deadc18ba9f4120c).

## 4. Tabla de símbolos frente al objeto extraido (lo que compara objdiff)

Son **cuatro** los ficheros que borran una etiqueta `gcc2_compiled._<dir>`, no tres: DeInterlace,
clamp, abortmsg y fflush. Comparé nuestro objeto final con `build/GOWE69/obj/...`, con la tabla de
símbolos y con `objdiff-cli diff -c ppc.calculatePoolRelocations=false`, antes y después:

- **Las cuatro `gcc2_compiled`: neutras para objdiff.**
  - En el extraido la etiqueta es un símbolo sin tipo (ni función ni objeto).
  - objdiff no la emparejaba ni antes (base) ni ahora.
  - Las funciones (`CFastDeInterlace`, `ClampLevels_C`, `REAL_abortmessage`/`PRINT_…`, `fflush`)
    siguen al 100 % y `.text` al 100 %.
  - En la tabla solo cambia que ya no exportamos `gcc2_compiled._<dir>` GLOBAL y nos queda el
    `gcc2_compiled.` LOCAL que emite cc1, que coincide con symbols.txt.
  - Nadie la referencia: el enlace no falla y el DOL es 9619ba57. promote.py solo cuenta como
    falta un símbolo *referenciado*, así que tampoco la ve.
- **Tres `gap_*`: aquí sí cambia lo que cuenta objdiff.**
  - En el extraido, `gap_08_804FF5A4_sdata` (allocator, 4 B), `gap_05_8041227E_rodata`
    (TokenEntropy, 2 B) y `gap_05_80412774_rodata` (criticalpath, 4 B) son `SYMBOL_OBJECT`.
  - En la base los emparejaba al 100 % porque nuestro asm definía el mismo nombre.
  - Ahora quedan **sin pareja: 10 B de datos** que objdiff dejará de contar como casados en esas
    tres unidades.
  - Las secciones siguen al 100 % en objdiff y todas las funciones y demás objetos igual.
  - En criticalpath, promote.py verá además `.rodata` de 632 B frente a 636 B en el extraido.
- **inittmr:** sin cambio. El `gap_08_804FF62A_sdata` del extraido ya estaba sin pareja en la base,
  porque el asm borrado no tenía etiqueta.
- **maddeca:** mejora. `zigzag` pasa de etiqueta sin tipo y tamaño 0 a `OBJECT` LOCAL de 256 B,
  como en el extraido. objdiff sigue sin nada por debajo del 100 %.

## 5. Propuestas para el coordinador

1. **Métrica de objdiff de las tres `gap_*`.** Hay dos salidas:
   - quitar esas tres entradas de `config/GOWE69/symbols.txt` y re-extraer allocator, TokenEntropy
     y criticalpath: son rellenos anónimos y no datos, así que los 10 B dejan de contarse en los
     dos lados;
   - aceptar la bajada de 10 B de "matched data", que no mueve el DOL.

   Para criticalpath, promote.py con la `.rodata` de 632 B frente a 636 B pediría lo mismo.
2. **Las cuatro `gcc2_compiled._<dir>`** no necesitan nada para el DOL ni para objdiff. Si se quiere
   que el extraido deje de exportarlas, el sitio es la herramienta (dtk/symbols.txt), no la fuente.
   `stdio.c` conserva el mismo alias y no es de este lote.
3. **Cierre.** Sellar el sha1 de los 9 `.o` promocionados contra la fuente (tabla del §2) tras el
   build real. `build_direct` puede mentir.

## 6. Ficheros

- Fuente modificada, sin commit:
  - `src/Packages/vp6/1.0.6/source/decode/cmn/{allocator.cpp,TokenEntropy.c,DeInterlace.c,clamp.c}`
  - `src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c`
  - `src/Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp`
  - `src/Speed/Indep/Libs/realcore/6.24.00/source/system/debug/cmn/abortmsg.cpp`
  - `src/egami/rcmp/dev/source/decoder/cmn/maddeca.cpp`
  - `src/libc/fflush.c`
- Objetos finales, que se conservan para que el verificador repita el enlace:
  `scratchpad/bibliotecas68t1/fin/*.o`.
- Copia de keep: `scratchpad/bibliotecas68t1/keep.lst`.
- Borrados: los objetos base, los de control y los JSON de objdiff.
