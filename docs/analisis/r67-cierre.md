# r67 — cierre

Tercera pasada de limpieza de andamios. Ronda de fidelidad: no buscaba bytes de DOL,
buscaba fuente legítima. Siete lotes con propiedad exclusiva de ficheros (70 ficheros).
En la ventana, además, las tres propuestas de `keep.lst` que llevaban cuatro rondas sin
aplicar.

## Progreso oficial verificado

| medida | antes r67 | después r67 |
|---|---:|---:|
| unidades completas | 525 / 619 | 525 / 619 |
| andamios de reparto (censo corregido: pin + `__asm__("")` + `asm("")`) | **238** | **209** |
| bytes de DOL distintos | — | 0 (ronda de fidelidad) |
| unidades rancias (`censorancios`) | — | 0 |
| `dolwhere zAI` / `zCamera` / `zLua` | 237.380 / 21.388 / secciones descuadradas | **53.453 / 15.557 / 1.667 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, relinkado desde el edge de
`main.elf` tras cada paso (nunca leído de `build/GOWE69/main.dol`).

---

## Primero, la corrección de la medida: eran 238, no 166

El censo de las r65–r66 contaba barreras con `__asm__\s*\(\s*""`. GCC 2.95 acepta también
la palabra clave **`asm`** (no hay `#define asm` en el árbol), y **70 barreras están escritas
así** — `asm("" : "+r"(value) : "r"(current))`. El patrón de pin, además, no veía
declaradores con `__typeof__(...)`: 2 más. En HEAD `46f48c49`:

| forma | HEAD |
|---|---:|
| pines `register T x asm("rN")` | 116 |
| barreras `__asm__("" ...)` | 52 |
| barreras `asm("" ...)` — **invisibles hasta hoy** | 70 |
| **andamios de reparto** | **238** |

La serie publicada 362 → 190 → 166 contó siempre sin la forma corta. La lección está en la
memoria de métricas que engañan (caso 13): el censo no daba un dato malo, daba **menos datos
de los que hay**. Herramienta nueva: `inventasm2.py` (literales concatenados, contexto de
declaración, y clasifica también el asm que no es de reparto).

### El resto del ensamblador que queda en fuente C (HEAD)

| categoría | n | qué es |
|---|---:|---|
| DATOS | 164 | `asm()` de fichero que define una etiqueta con nombre y emite datos |
| HUECO | 324 | `gap_`/`pad_`, `.space`/`.skip`, cadenas anónimas en `.byte`/`.asciz` |
| SECCION / ALIAS | 3 / 17 | empujones de orden, `.globl`/`.set` |
| INSTR | 67 (+~20 sin clasificar: `fres`, `dcbt`) | funciones en asm, bloques `"=f"`, VP6 original |
| NOMBRE | 492 | `__asm__("simbolo")` en declaraciones: no es andamio |

Los **508 parches de datos** son el frente siguiente de la limpieza, con otro método
(variable real desde el DWARF + posición en el ELF enlazado). CreateCarLightFlares (abajo)
es el primer aviso de lo que cuesta: un parche de datos puede estar compensando otro.

---

## Por lote (censo corregido, HEAD contra árbol, fichero a fichero)

| lote | retirados | dónde |
|---|---:|---|
| libc | 7 | `itoa.c` 1 pin + 2 barreras (limpio); `vfprintf.c` 4 pines |
| juego-promovido | 7 (+1 corta) | `Strings.cpp` 5 pines + 1 `asm("")`; `QuickGame.cpp` OnManageTime 2 pines |
| bibliotecas | 5 | `sst.c` 2 pines, `exit.cpp` 1 pin, `inittmr.cpp` 1 barrera, `srender.c` 1 barrera |
| juego-audio-resto | 5 (+1 corta) | `EAXAemsManager.cpp` 2 barreras, `EaxSoundTypes.cpp` 1 pin + 1 `asm("")`, `SFXObj_Pathfinder.cpp` 1 pin, `ScratchPtr.h` 1 barrera |
| libsn | 3 | `crt2D1.c` 1 pin, `FSasync.c` 1 barrera, `metrotrk.c` 1 pin |
| juego-fe-ecs | 0 | 33 irreducibles con diagnóstico; el pin fr5 de UpdatePlatInfo devuelto |
| juego-world-snd | 0 | 25 irreducibles con la cifra del `.greg`; una propuesta (CreateCarLightFlares) |
| **total** | **29** | ningún pin convertido en otra forma (INSTR/DATOS/BARRc no crecen) |

Todos los irreducibles llevan su diagnóstico r67 junto al andamio (`previo.py r67`).

## Lo que retiró andamios

Casi nada salió de las palancas de reparto tal cual. Salió de **la forma que dicen el DWARF,
el mapa de líneas y los oráculos públicos**:

- **Oráculos públicos**: `crt2D1.c` es `DO_GLOBAL_CTORS_BODY` de libgcc2 literal; `itoa.c`
  cae con la macro `EXTRACT_WORDS` de fdlibm, y `vfprintf.c` con las `PRINT/PAD` de newlib
  como `{ }` y no `do/while(0)`.
- **Un `return` por rama** escribe directo en r3 (`bStrNCmp`/`bStrNICmp`: 5 pines + 1 barrera).
- **Una local que el DWARF lista sin registro** es una local que el original no usó: sus
  sentencias van a otra (`sst.c`).
- **El sentido de una comparación** (`a < b` contra `b > a`) decide el reparto con el mismo
  `subfc` (`EaxSoundTypes.cpp`).
- **Constante en un bloque básico anterior**: cse1 la olvida en la etiqueta y combine no
  cruza bloques (`FSasync.c`, en lugar de la barrera selectiva).

## Reglas nuevas medidas

1. **Reparto GPR de operandos `"=r"`**: `REG_ALLOC_ORDER` da r0, r9, r11, r10, r8…; salida
   usada como base → r9; temporal de un uso → r0; salida devuelta → r3. rs6000 no tiene letra
   de clase para un GPR suelto: **ningún operando alcanza r4–r6**.
2. **`do { } while (0)` pesa**: abre notas de bucle y `REG_N_REFS` pondera por `loop_depth`.
3. **El volcado DWARF lista las locales de una copia INLINE en orden inverso**; las de la
   función, en orden.
4. **Un pin mete su registro en `regs_ever_live` antes de `global_alloc`**, y la pasada 0 de
   `find_reg` sólo acepta registros ya usados: fija repartos que la prioridad no puede ordenar.
5. **La clase «una referencia de más»**: la barrera no volátil suma 1 a `n_refs` sin emitir
   nada; si el hueco del `.greg` se cierra con +1 ref y la vida no se puede tocar, no hay C.
6. **`int x[1]` no es `int x`**: el array marca `MEM_IN_STRUCT_P` y anula la exención de
   `fixed_scalar_and_varying_struct_p`.
7. **Una constante de pool tiene `REG_EQUAL`** y local-alloc le duplica la vida para
   global-alloc; un alias `lbl_` no: cambiar literal por alias cambia el reparto.

## Propuestas: decisión de cada una (protocolo nuevo)

| propuesta | decisión |
|---|---|
| auditoría r60–r66: `keep.lst` de zAI (3 líneas, r63) | **APLICADA** (`c2f2c2d0`): `dolwhere zAI` 237.380 → **53.453 B** (−183.927), secciones cuadran, DOL base intacto |
| auditoría r60–r66: `keep.lst` de zCamera (1 línea, r63) | **APLICADA** (`c2f2c2d0`): `dolwhere zCamera` 21.388 → **15.557 B** (−5.831, la cifra exacta de la r64) |
| auditoría r60–r66: paquete `keep.lst` de zLua (r64-track) | **APLICADA** (`c2f2c2d0`): −2 entradas caducadas (`GAMECUBE`, `%d`) y +7 `@lc`; `lcfix zLua` al día; `dolwhere zLua` pasa de secciones descuadradas (`.rodata` 96 B corta) a **1.667 B** cuadrando. De esos, 1.231 B son un solo rango (`$LC684`): la unidad más cerca de promocionar |
| `scripts/rtldump.py`: `cc1` para los `.c` | **APLICADA** (`39287e35`; control: volcados de itoa) |
| `CarRender.cpp` CreateCarLightFlares: 3 pines + 1 asm de datos fuera con el contenido muerto `type_name` del DWARF | **RECHAZADA POR MEDIDA, aplazada como paquete** (detalle abajo) |
| `git rm src/LibSN/metrotrk.c` (r65, r66, r67) | **APLICADA por decisión del usuario** (`a042cab2`): no lo leía ningún arco (`tools/project.py` resuelve `src_path` desde `source=` al `.s`; `splits.txt` y `configure.py` sólo usan el nombre de la unidad). Retira 11 pines + 1 barrera: 209 → 197 |
| `EPlayRaceNIS.cpp`: barrera → store muerto `nisType = CAnimChooser::Intro;` | **APLICADA por decisión del usuario, «hasta encontrar algo mejor»**. Remedida en copias estáticas de HEAD `20d50bfa` (con agentes de la r67b tocando cabeceras de zMain): con el store, zMain IDÉNTICO en ALLOC y symtab; control sin barrera ni store, DISTINTO (`.text` 175.204 → 175.196). La local la da el DWARF; la sentencia concreta no deja rastro: es reconstrucción, y así queda escrito junto a ella. 197 → 196 |
| `slinklist.h` para csis | aplazada: encargo propio con barrido de todo snd |
| pathbank `int fileop[1]` | **RECHAZADA**: contradice el DWARF (`int fileop`) |
| filesys AddToQueue, bloque anónimo de `curpriority` (neutro) | aplazada: fidelidad sin retirada |

### CreateCarLightFlares: correcta en su sitio, rota en el enlace

La variante del lote (un `case` por cadena con `type_name = "...";` y sin el `.asciz` de
la r64) deja la función al 100 % sin pines, y el lote midió que el `.rodata` del objeto casa
276 B seguidos con el original donde la base casa 15. En el ENLACE:

| estado | `.rodata` zWorld enlazada | `dolwhere zWorld` |
|---|---|---|
| base | `4C520` = original | 23.513 B, secciones cuadran |
| variante sola | `4C480` (−160 B) | secciones descuadradas: las 15 cadenas quedan muertas y `-strip-unused-data` se las lleva |
| variante + 15 `@lc` (lcfix, 0 FALLO) | `4C540` (**+32 B**) | secciones descuadradas |
| revertida (control) | `4C520` | **23.513 B**, cuadran |

El `.asciz` pegado de la r64 compensaba otro bloque de `.rodata` de zWorld demasiado largo.
La variante sólo entra junto con esa compensación localizada; candidatos del propio agente:
los `.asciz` de `CarLoader.cpp` (58), `CarInfo.cpp` (11), `SkyRender.cpp` (10). El parche y
el `keep.lst` de la variante quedan en el scratchpad de la sesión.

## Verificación de la ventana

1. **Propiedad**: 70 ficheros modificados, 70 en su lote, 0 fuera.
2. **Censo HEAD/árbol** por fichero con las tres formas: −29, sin conversiones.
3. **`cmphead.py`** (HEAD extraído con `git archive` + los 34 `Generated/Hashes` ignorados;
   controles: negativo IDÉNTICO, positivo DISTINTO): **43 de 43 unidades IDÉNTICAS**, las 20
   pendientes que el DOL no ve incluidas. `ScratchPtr.h` entra en cuatro unidades (zMain,
   zPhysics, zPhysicsBehaviors, zSim), no en dos, y `vfprintf.c` también en `vfprintf_1`.
4. Reconstrucción de los 43 `.o` en build/ (0 fallidas) y `censorancios.py`: **`RANCIO 0`**,
   `NOCOMPILA 0`, `SIN_O 0`, `ok 521`, `ok_debug 4`.
5. Relink del DOL desde el edge: 619 objetos, `9619ba57…`, **DOL OK**; repetido tras cada
   cambio de `keep.lst` y tras revertir CarRender.
6. `lcfix.py --check`: **0 CORRIGE**; las FALLO leídas una a una, todas de la clase inerte
   («no tiene símbolo `$LC` propio»), ninguna `@lcsrc`. 55 al cerrar la ronda, **53** tras el
   paquete de zLua (se van su `GAMECUBE` y su `%d`).

Commits: `0ba188b9` libsn, `9fa97f87` libc, `9aa11fd2` bibliotecas, `aceb251b`
juego-promovido, `d0378a08` juego-fe-ecs, `e4d10c89` juego-world-snd, `5e7c3777`
juego-audio-resto, `39287e35` rtldump.py, `c2f2c2d0` keep.lst. Antes de commitear: UTF-8 en
los 70, y el único fichero con finales mixtos (`FEngInterfaceFEObjects.cpp`) normalizado.

## Lo que queda

- **209 andamios de reparto**, de ellos **68 barreras `asm("")`** que ninguna ronda ha mirado
  todavía: la r67b.
- **508 parches de datos** y ~87 bloques de instrucciones: frente propio.
- **zLua a 1.667 B** del DOL, con 1.231 B en un solo rango: candidata a promoción.
