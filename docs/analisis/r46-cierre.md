# R46 — cierre

2026-09-08. Ocho agentes en paralelo sobre los 52.924 B pendientes completos,
más la ventana. DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`
después de cada paso.

## Estado

| medida | r45 | ahora |
|---|---:|---:|
| código exacto | 3.893.124 / 3.946.048 B — 98,65881 % | **3.896.580 — 98,74639 %** |
| funciones exactas | 18.373 / 18.432 | **18.378** |
| código linked | 746.188 B — 480 / 617 | **757.604 B — 483 / 616** |
| datos linked | 319.056 B | **355.348 B** |

**+3.456 B de código, +5 funciones, +11.416 B de `linked`.**
Quedan **49.468 B en 24 unidades**.

## Cerrado: cinco funciones, tres unidades a cero

| unidad | función | B |
|---|---|---:|
| `path/pathtrack` | `PATH_createstreamimp` | 424 |
| `realcore/dvd_device` | `StartNonAlignedAyncRead` | 440 |
| `zFe2` | `__12IconScroller` | 384 |
| `zEAXSound2` | `GinsuSynthData::BindToData` | 340 |
| `zEagl4Anim` | `FnStatelessQ::EvalSQT` | 1.480 |

`pathtrack`, `dvd_device` y `zFe2` quedan a cero funciones pendientes.
`DebuggerDriver` suma otros 388 B por cambio de configuración (§ventana).

### Las cuatro palancas nuevas

1. **El área X del marco.** Los 8 B que dos rondas dieron por inalcanzables en
   `pathtrack` no eran una ranura de derrame: son el área X del marco SVR4 de
   rs6000, el temporal de conversión entero↔flotante, que GCC 2.9 reserva al
   **expandir** la conversión y no libera aunque el optimizador se lleve después
   todo su código. Firma buscable: mismas instrucciones, todas idénticas, marco
   del objetivo 8 B mayor, y líneas del original sin una sola instrucción.
2. **Los dos grupos de peso de `sched1`.** `rank_for_schedule` desempata por
   `INSN_REG_WEIGHT`, un nivel que sólo existe en `sched1`. En una tirada de
   escrituras de miembros que leen el mismo registro constante, el store que
   **mata** ese registro pesa −1 y los demás 0: salen **primero todos los de
   peso −1 en orden de fuente y luego todos los de peso 0 en orden de fuente**.
   Y el `REG_DEAD` está en el último uso *en la fuente*, así que mover una
   sentencia decide quién pesa −1. Cerró `zFe2` sin un solo `asm`.
3. **El `asm` que emite la instrucción que falta.** En `EvalSQT` el `.combine`
   enseña que el `subi` del objetivo ya está y lo único que sobra es un
   `zero_extend` que `combine` no puede tirar, porque el pseudo tiene una def
   por copia que envenena `reg_nonzero_bits`. En C no hay salida. Un `asm` cuyo
   operando de **entrada es el campo con su tipo estrecho** —HImode, así que
   GCC pasa el registro crudo— y que emite la instrucción que el objetivo ya
   tiene, deja el tamaño exacto y el diff a cero. No es deuda.
4. **El parámetro que no sale en el diff.** r36e midió siete barreras sobre
   `FileInfo` y se quedó en 436 B. La palanca era sacar `MemPointer` de r4:
   entonces GCC pone `FileInfo` en r28 solo y emite las seis `mr` que faltaban.

### Y una corrección de fuente que no suma bytes todavía

`HolePunchAvoidables` pasa de 44 filas a 9 porque **`bCross` tenía los
argumentos al revés** —calculábamos el producto cruzado con el signo
contrario—. Era la única de las cuatro llamadas del bloque con el orden
invertido, y el asm lo delata: `bCross(a,b)` sale como `fmuls t=b.x*a.y` +
`fmsubs r=a.x*b.y-t`. Corregido eso, la reasociación **vedada tres veces**
(r36b, r36d, r36e) resulta buena: la veda medía la sentencia equivocada.

Regla nueva: el destino del `fsel` de `bMin`/`bMax` dice si la sentencia de
fuente continúa. Y queda pendiente auditar los **35 `bCross(`** del árbol.

## El instrumento de la ronda: los volcados RTL

Dos agentes lo encontraron por separado el mismo día. **El fuente de GCC
2.95.3 está en el árbol** (`orig/prodg/NGC_GNU_SRC/NGC/gcc/`), y aunque
`ngccc.exe` se come `-da`/`-dg` sin hacer nada, invocando `CPP.exe` +
`cc1plus.exe` a mano salen los volcados: una SourceList entera en 22 s.

- El **`.greg`** trae el orden de asignación **ya ordenado por
  `allocno_compare`**, la matriz de conflictos y el `reg_renumber` final. Un
  near-miss de reparto deja de ser adivinanza.
- Con `-dj`/`-dL` se cuentan rtx por bloque. Así el presupuesto de `cse` de
  zCamera pasó de cualitativo a cuantificado: tabla en el rtx 933,
  `flush_hash_table` en el 1.001 (`cse.c:8899/8942`), la tabla cuesta 174 rtx,
  límite 826. Calibrado con tres puntos.

## La ventana

| paso | resultado |
|---|---|
| promocionar `pathtrack` + `dvd_device` | **+5.176 B linked**, DOL OK |
| fundir `asd2` en `gc_interface` y promocionar | **+6.240 B linked**, DOL OK |
| `DebuggerDriver` opción A (`toolchain_version` → `GC/1.2.5`) | **+388 B**, DOL OK |
| `DebuggerDriver` opción B (partir el objeto) | **REVERTIDA**, ver abajo |

### El racimo: `asd2` no era una unidad

Era el trozo que el troceador cortaba del final de `gc_interface` —los dos
`_GLOBAL_.I./.D.` y sus entradas de `.ctors`/`.dtors`—, y por eso ninguna de
las dos podía promocionar por separado: nuestro par `_GLOBAL_` es **LOCAL** y
no puede deduplicarse contra el de `asd2`, y exportar el símbolo con sufijo
desde la fuente está medido como imposible. La fusión era obligatoria.

Y corrige el diagnóstico que yo había dado: GCC no bautiza `_GLOBAL_.I.<X>` por
el estático con **constructor** sino por `first_global_object_name`, la primera
global que la unidad ensambla, tenga constructor o no. `ROOT_DIRECTORY_NAME` ni
siquiera existe en el ELF original: era una global sin referencias que el
enlazador se llevó.

Lo que retenía la unidad era el **juego y el orden** de secciones. GCC 2.9
emite las vtables en orden **inverso** de declaración de clase y las inline
diferidas en orden de **definición**.

Hallazgo transversal: **`ngcld` descarta duplicados y se lleva sus bytes**, y
entre GLOBAL y WEAK gana GLOBAL —entre dos WEAK, la del objeto anterior—. Los
símbolos de `.gnu.linkonce.*` son WEAK y los de `#pragma implementation`
GLOBAL, y por eso nuestro `_._BlockCalculator` perdía y la unidad salía 52 B
corta.

### `DebuggerDriver`: la opción B falla al ENLAZAR, no al compilar

El diagnóstico es sólido y está probado: el objeto son **dos unidades de
traducción**, la costura está en `EXI2_CallBack` (`0x803465D0`), y cortando
ahí cada mitad con su versión de MWCC sale con **cero funciones distintas**,
31 de 31.

Pero al aplicarlo de verdad, `ngcld` da
`AmcExi2.c(1) : error: L0039: Reference to undefined symbol Ecb`.

**La causa, y es una lección general**: el agente verificó el reparto de
estáticos **compilando** las dos mitades, no enlazándolas. `Ecb` es `static` en
la mitad de arriba y la de abajo lo referencia; al compilar por separado cada
objeto sale bien, y sólo el enlace lo destapa.

Y el ELF original dice que la salida no es hacer `Ecb` global: hay **dos `Ecb`
distintos y los dos LOCAL** —`0x804BBD68` de 192 B y `0x804BC0C8` de 24 B—, o
sea que cada unidad de traducción tiene el suyo. Ninguno de los dos está
cubierto por un rango de `splits.txt` hoy.

Revertido a la opción A. La B sigue valiendo **416 B más y los 5.988 B de
`linked`** de la unidad, y el trabajo que falta está acotado: averiguar cuál de
los dos `Ecb` es de cada mitad, darles dueña en `splits.txt`, y repetir. Junto
con el aviso de datos del §1.4 del informe (`SendCount` a `.sdata`, el
`ucEXI2InputPending` que falta, y `exi_804FF598` —otro huérfano— que debería
ser `extern`), es un encargo completo para la r47.

## Frentes cerrados con medida

- **`bigswizzler` no se puede cobrar**: 48 de los 52 B de detrás **son suyos**
  (la constante de pool `0x4330000000000000`, `0.0625f` y la cadena de assert
  que `NDEBUG` elimina). Es la veda estructural del pool, no atribución de
  rango.
- **El censo del `__static_init` de zCamera sale limpio**: las 34 llamadas a
  constructor son idénticas y en el mismo orden, y el multiconjunto de 192
  referencias a globales también. La hipótesis que yo había dirigido, cerrada
  en negativo.
- **`SAP.h`** (4.396 B): el único escape de `jump.c:1760` es que falle
  `invert_jump`, y `can_reverse_comparison_p` devuelve 1 **siempre** con
  `flag_fast_math`, que las dos unidades llevan. Con estos cflags no hay forma
  de fuente.
- **`ActualReadJoystickData`**: los cinco `mr` muertos que r36f daba por
  sentencia de fuente **desaparecen con `-fno-schedule-insns`**. Son copias del
  reparto: el frente vivo es la presión, no la forma del `if`.
- **`steering`**: 19 versiones de MWCC y 232 compilaciones de pragmas sobre la
  fuente actual, ni un acierto.
- **`FSasync` no es Metrowerks** —lo compila la regla `prodg`—, y sus 388 B
  sobrantes son un `lis sym@ha` que no tiene nombre en C, o sea fuera del
  alcance de cualquier operando de `asm`.

## Correcciones a reglas que teníamos por buenas

1. **«Si el DWARF tiene menos locales que tú, ahí está el near-miss» sigue
   valiendo como pista, pero la recíproca es FALSA.** Tres contraejemplos en
   una sola ronda: `slipBoost` y `scale` son locales que el DWARF del original
   no lista y quitarlas **rompe el tamaño exacto**; y las tres locales «sólo
   nuestras» de `DefragmentPool` cuestan 47 filas si se retiran.
2. **`fncmp.py` y `triaje.py` no ven `zFeOverlay` ni `zOnline`**: su código va
   en sección **`.over`**, no `.text`, y son los dos únicos objetos extraídos
   así. `fncmp` aborta y `triaje` contesta «0 funciones, 0 B»: han estado
   invisibles al triaje que reparte el trabajo. Aviso medido: cambiar sólo el
   nombre de la sección da **404 de 467 falsos positivos**, porque la
   normalización de saltos internos está atada al símbolo de sección `.text`.
   Hay que generalizar las dos cosas a la vez.
3. **`GetLoadingPriority`**: r45 lo dio por cerrado y no lo estaba. Medidas ya
   las dos bases (75 filas / 94,72 % contra 71 / 97,18); se queda la de HEAD
   porque la otra emite un árbol que el objetivo no tiene. Y los 16 B del marco
   están **dentro del bloque muerto de `RemoteCaffeinating`**, no en una local
   viva: los dos volcados DWARF tienen árbol de bloques idéntico.
