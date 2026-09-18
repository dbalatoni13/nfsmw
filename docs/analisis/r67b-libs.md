# r67b — lote libs: barreras cortas `asm("")`

Censo con el patrón `(?<![\w])(?:__asm__|asm)\s*\(\s*""`, **sin comentarios**
(las notas citan barreras y no cuentan):

| fichero | antes | después | pines (sin cambio) |
|---|---|---|---|
| `src/libc/vfprintf.c` (vfprintf + vfprintf_1) | 8 | 8 | 15 |
| `src/libc/memset.c` | 1 | **0** | 0 |
| `src/libc/strstr.c` | 1 | **0** | 0 |
| `src/LibSN/syscalls.c` | 1 | **0** | 0 |
| `src/LibSN/FSasync.c` | 2 | **0** | 1 |
| `csis/.../cmn/csis.cpp` | 2 | 2 | 3 |
| `realmemcard/.../gc/gc_driver.cpp` | 2 | 2 | 2 |
| `vp6/.../gc/criticalpath.c` | 1 | 1 | 3 |
| `snd/9/.../mix/sfsplit.c` | 1 | 1 | 1 |
| `path/5.01.04/.../pathsnd.cpp` | 2 | **1** | 0 |
| `path/5.01.04/.../pathserv.cpp` | 1 | **0** | 0 |
| **total** | **22** | **15** | 25 |

**7 retiradas.** Ningún pin se convirtió ni se retiró. Las 15 que quedan llevan
una nota `r67b` junto al andamio con lo medido.

## 1. Test

Cada cambio se compiló a un temporal y se comparó con la compilación de HEAD
(igual byte a byte que el `.o` en disco en las 12 unidades) por sección:
`.text .rodata .data .sdata .bss .ctors` y sus `.rela.*` (sin `.rela.debug*`),
más `.symtab`/`.strtab`. IDÉNTICAS → se acepta; si cambian → se revierte en el acto
(`scratchpad/libs67b/trial.py`). El control falla de verdad: quitar la barrera de
`memset` sin cambiar la forma da `.text 86d188f7 → fe4a2602`.

## 2. Retiradas

| barrera | palanca | medido |
|---|---|---|
| `memset.c` `"+r"(buffer), "+r"(n)` | **forma literal de newlib** (oráculo público): `UNALIGNED(m)`, `aligned_addr = m`, `c &= 0xff` y el patrón dentro de `if (LBLOCKSIZE == 4)` con su rama de bucle muerta, más las locales sin usar `count`, `i`, `unaligned_addr` | sin barrera y con la forma vieja: la segunda mitad del patrón en r9 y el `cmplwi r5,15` adelantado |
| `strstr.c` `"+r"(i)` | **forma literal de newlib**: un `return` por rama, `while (*searchee)` con `size_t i` en el bloque, `while (1)` con el `i++` al final | con do/for/goto sin barrera, GCC duplica la primera vuelta del bucle interior (+20 B) |
| `syscalls.c` `"+r"(buffer), "+r"(numBytes)` | **el prototipo estaba mal**: `PCwrite(int)` → `PCwrite(int, const void *, size_t)` y la llamada `PCwrite(fd, buffer, numBytes)` | con un argumento, `buffer`/`numBytes` mueren en la rama serie y los `mr r3,r4; mr r4,r5` suben por encima del primer salto |
| `pathserv.cpp` `"+m"(pfstate->idflags)` | **alias al revés**: tipo ESCALAR en las tres lecturas de `idflags` (`*(unsigned long *)&pfstates[p]->idflags`) | solo las dos guardas → hay recarga, pero delante del `stw` (3013a518); vista agregada solo en el almacén → recarga también `pfstate` (d80a682d) |
| `pathsnd.cpp` `: : : "memory"` (timercallsinarow) | vista agregada `PATH_AGG` en **las dos** lecturas (guarda y `correction`) | solo en la guarda → la carga ya va detrás de los `stw` pero en r0 + `mr r9,r0` y recarga de pfstate (+4 B, f4d2da31). La r66 lo midió en un solo sitio |
| `FSasync.c` `"+m"(err) : "r"(cb)` y `"+r"(cb)` | vista agregada por cast en el **almacén de la fase** (o en el del error: el mismo objeto) | en la lectura de `cb`: fb3aa7ef. Antes, sin éxito: `err = phase = ack` (relee la volatile, +4 B), sin local `cb` y un puntero local al error (fb3aa7ef las dos) |

### Por qué funciona la palanca de alias en FSasync y pathserv

`true_dependence` de GCC 2.9 no ve conflicto entre un escalar en dirección fija y un
campo de struct en dirección variable (`fixed_scalar_and_varying_struct_p`), y en
FSasync, entre dos escalares fijos con símbolos distintos. El planificador reordena
entonces carga y almacén. En el original la dependencia sí existía:

- **FSasync**: con la fase escrita como `MEM_IN_STRUCT_P`, el `lwz` de `cb` vuelve a ir
  delante del `stw` de la fase, y el `lis` del error sube solo (`lis r30; lis r9;
  lwz r0; stw; stw; cmpwi`).
- **pathserv**: al revés. El almacén de `pfstate` es un escalar fijo, así que para que
  mate la carga de `idflags` hace falta que la carga NO sea de struct.

## 3. Irreducibles (15), con la nota r67b junto al andamio

| barrera | diagnóstico | intentos r67b |
|---|---|---|
| vfprintf `"+r"(width)` | reparto global de 18 preservados (r65/r67) | quitada sola: 624677c6 / 85b058c1 (mismo tamaño) |
| vfprintf `"+r"(float_format)` | quitada: −4 B (1aec5e3a). Se va el `mr r26,r0` que el original conserva: con `ch` clavado a r26, temporal y `ch` comparten registro | 1 |
| vfprintf `prefix_old_p` (×2) | reparto global | e606baed / 44bc44a7 ; 3a7f700d / 26585644 |
| vfprintf `prefix_flags, ch` | reparto global | 5fda8fc1 / 9ff386ce |
| vfprintf `"+r"(field_count)` | par con el pin a r0 (r65) | 35d44f9e / e343cae9 |
| vfprintf add_separators `"r"(state) : "r10"` | desempate de sched1 (orden `li r7` / `mr r10`) | 4 formas: a secas, `state = 0` primero en el for-init y guarda `!dst` → ce5c55b5; `state = 0` último o `mark = end; state = 0;` sueltos → 4fe51858 |
| vfprintf add_separators `"r"(state)` | r7 ocupado en el bucle de inserción del original; sin barrera 0x55555556 → r7 y `','` → r5 (88dbd660). Sin DWARF | 1 |
| csis `Subscribe`/`Unsubscribe` | base que elige cse en `CListDStack::Push/Remove` (slinklist.h, cabecera de todo snd). No se toca: diagnóstico | 0 (encargo) |
| gc_driver clobber r25 | par con el pin de `result` (r67: 3 formas) | quitado solo: e75676c0 |
| gc_driver clobber cr4 | **forma nueva**: solo el clobber, con el pin → +4 B (60d0f517). La r67 solo lo midió sin pin | 1 |
| criticalpath `finalIndex/finalOffset` | sin oráculo (no hay DWARF de VP6_DecodeBlock); 8 formas r65 + r66 | 0 (sin reabrir) |
| sfsplit `copySource/copySize` | orden de carga r3→r4→r5 (r67, RTL) | guarda envolvente `if (retframes > 0) ... else return` → 3608a566 |
| pathsnd `"=m"(framePad)` | el DWARF del original NO tiene `framePad` (regmap: 10 locales iguales); hueco de 8 B en 0x8 que no usa nadie, y las temporales de conversión van a 0x10 | 2: `maxchunks`/`memsize` con inicializador y `streamimp` con inicializador → ecb901f9 (marco 0x30) |

## 4. Reglas nuevas

1. **La palanca de alias vale en las DOS direcciones y en TODOS los sitios.**
   - En pathsnd la r66 la dio por negativa porque la aplicó en una sola lectura.
   - En pathserv la vista agregada del almacén no sirve. La que sirve es la
     escalar de las lecturas: `*(T *)&p->campo` quita `MEM_IN_STRUCT_P`.
   - Se diagnostica mirando qué carga/almacén adelanta el planificador y cuál de
     los dos es «escalar fijo» o «struct variable».
2. **Un adelanto de `stw` sobre `lwz` entre globales escalares** (FSasync) es la
   misma palanca, no una barrera selectiva: la vista agregada en el almacén
   devuelve la dependencia.
3. **Prototipo `extern` de menos argumentos = liveness de menos.** En LibSN los
   prototipos son de la casa; `PCwrite(fd)` escondía `(fd, buf, n)`. Antes de una
   barrera con `"+r"` sobre parámetros que van a una llamada, comprueba la firma real.
4. **newlib literal incluye lo muerto.** La rama `if (LBLOCKSIZE == 4)` con su
   bucle muerto y las locales sin usar son parte de la forma que casa.

## 5. Propuestas (ficheros ajenos)

Ninguna nueva. Sigue en pie la de la r67 (`r67-bibliotecas.md`): arreglar la
elección de base en `CListDStack::Push/Remove` de
`src/Speed/Indep/Libs/snd/9/source/library/cmn/slinklist.h`, con test en todas las
unidades de snd y csis. Retiraría los dos andamios de csis.cpp. Ni config, ni
`keep.lst`, ni `configure.py`.

## 6. Sellos

Digests SHA-1 (12 cifras):tamaño. **Antes = después en todas**. «Antes» es la
compilación de HEAD, verificada igual al `.o` en disco; «después» es la fuente
final, recompilada.

| objeto | `.text` | `.rela.text` | `.rodata` | `.data` | `.sdata` | otras | `.symtab` | `.strtab` |
|---|---|---|---|---|---|---|---|---|
| vfprintf.o | c3ce1638243b:6948 | 3bf09023f266:6072 | 7bf8feeb9f1e:144 | —:0 | 05fe40575316:8 | `.rela.sdata` a2f8f42abdda:12, `.bss` 128 | 06e1b4c8eae8:576 | 344df2239c30:242 |
| vfprintf_1.o | f48c48979805:5972 | 1fd2b5fccbc7:5148 | —:0 | —:0 | — | | 43be995faaea:544 | 5d06d933602a:306 |
| memset.o | 86d188f79947:148 | 7af97f88938b:84 | —:0 | —:0 | — | | f2192c5bd3ed:224 | 778bdff26cb8:32 |
| strstr.o | a89f5e35c106:104 | b58b41a73389:60 | —:0 | —:0 | — | | 2885022c4811:224 | 6843002a976f:32 |
| syscalls.o | 911ab9998a06:480 | 3cc14a0a7326:264 | —:0 | —:0 | — | | f3cc47e6daa6:480 | e739f61202b6:172 |
| FSasync.o | 34e92584327a:2104 | baa35212f76f:2988 | —:0 | —:0 | — | | 8b2c77f3a7d4:800 | dc8af60ee196:667 |
| csis.o | abb649979074:5728 | 64a9e38d17ba:3276 | ea44508291e1:112 | 40bf0c6cf280:28 | — | `.ctors` 9069ca78e745:4, `.rela.ctors` b99f3a41f52b:12, `.rela.rodata` 05dc231ce69c:96, `.bss` 28 | cbb5fe40b6e7:1408 | 80e704f0a033:3250 |
| gc_driver.o | 75d368acf3b7:8952 | 55985ff2c4a9:5448 | b2c475da0655:184 | —:0 | eb84c5584223:16 | `.rela.rodata` d4192f832a82:72, `.bss` 108 | 8f8754e05e69:2208 | e408bdedf1de:4637 |
| criticalpath.o | dc8f68814e4a:12040 | 3dc4bee14a46:4812 | 6c710e23d0bd:636 | 6404fc0c15e0:224 | 410adbae2416:8 | `.bss` 144 | 96e098bcb51b:1168 | 29fa5f925d8e:951 |
| sfsplit.o | 42786ccdeb8a:380 | b65a8664c564:204 | —:0 | —:0 | — | | ab31b0c99ece:304 | ea0f9c08b82d:180 |
| pathsnd.o | 22abc6425c5f:8452 | 96e97b995428:4524 | c3edb6f88f57:1128 | —:0 | — | `.rela.rodata` bc13e6ea0aa6:1404 | 103c96c51d29:2624 | cb2e6a648605:4258 |
| pathserv.o | b9374e1347e3:1700 | c7bf54b775e6:1188 | —:0 | —:0 | — | | 2975f174a133:704 | 2181ca1b9ed7:832 |

## 7. Notas de proceso

- **Finales de línea.** Casi todos los ficheros del lote son LF puro en la copia de
  trabajo, no CRLF. FSasync.c es CRLF; syscalls.c es mixto (el bloque de `_write`
  en LF). Se conservó lo de cada línea. Todos en UTF-8.
- **Cambios de código.** `git diff`: las líneas `-` que no son comentario son solo
  las sentencias sustituidas a propósito. No se borró ningún andamio por accidente.
- **`__LINE__`/BNEW.** No aparecen en ningún fichero del lote.
- **Unidades del DOL.** Todas promocionadas: toca reenlazar el DOL al cierre, como
  siempre, aunque las ALLOC salen idénticas.
