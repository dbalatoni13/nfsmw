# r36b — zWorld / zWorld2 (8 funciones, 10.688 B)

Base verificada al empezar (`build_direct` + `triaje`), y **la misma al terminar**
salvo lo que se diga aquí:

```
                                          BASE      AHORA
   2980 zWorld2  HolePunchAvoidables       98,133 %  98,133 %  55 difs
   2908 zWorld   RenderFlaresOnCar         98,618 %  98,618 %  18 difs
   1284 zWorld2  CookieTrailCurvature      99,657 % 100,000 %  CERRADA  <-- 1.284 B
    876 zWorld   UpdateWheelYRenderOffset  99,384 %  99,384 %   7 difs
    836 zWorld   CullParts                 99,426 %  99,426 %   5 difs
    816 zWorld2  InitAtSegment             99,152 %  99,152 %  42 difs
    684 zWorld   DefragmentPool            99,269 %  99,269 %  23 difs (estructura CERRADA)
    304 zWorld   SetMemoryPoolSize         97,368 %  97,368 %   2 difs  <- no venía en el encargo
```

`pctsnap --cmp` contra la base: **1 mejora, 0 empeoran.**

`SetMemoryPoolSize` (304 B) **sí forma parte de los 10.688 B**: 2980+2908+1284+
876+836+816+684 = 10.384, y los 304 que faltan son suyos. Queda diagnosticada
abajo.

**El porcentaje que imprime `triaje.py` viene de `report.json`, que `build_direct`
no regenera: después de recompilar es RANCIO.** La medida buena es la cabecera de
`fndiff.py` (`ours=…%`), que sí recalcula. En el ensayo 2 `triaje` seguía diciendo
98,133 % cuando la función había caído a 97,919 %. Cuidado con eso.

---

## 0. Lo que se lleva la ronda (resumen)

0. **`CookieTrailCurvature` CERRADA al 100 % (1.284 B)** con una palanca nueva:
   **la barrera selectiva MÁS pines de registro**. La barrera crea la copia que
   faltaba y los pines la ponen donde el objetivo la tiene. Cuatro rondas
   (r25-r29) habían barrido quince formas de esa sentencia con binario idéntico.
   Detalle y receta en §6.
1. **La etiqueta de tipo abre un bloque VACÍO en el DWARF.** `enum {...};` o
   `struct X {...};` dentro de un bloque emite **cero instrucciones** y a la vez
   fuerza a GCC 2.9 a emitir ese `lexical_block`, **sin que aparezca nada dentro**
   en el volcado. Es la única construcción que lo hace, y cierra el frente
   «el nivel de bloque que no se fabrica con llaves» que la r36 dio por imposible.
   Aplicado en `CarLoader::DefragmentPool`: `.text` byte a byte idéntica y
   `regmap` pasa de *2 desajustes de bloque + 1 local de ámbito equivocado* a
   **ninguno**. Ver §5 — hay **90 bloques anónimos vacíos en 76 funciones** del
   volcado del original, así que esto no es de una función sola.
2. **`-ffast-math` reasocia los productos flotantes**, y eso decide cómo hay que
   escribir una sentencia. Regla y tabla de medidas en §1.
3. **Aviso de medida**: el porcentaje difuso de objdiff **da crédito parcial** a
   una fila que sólo cambia de registro, así que **puede SUBIR mientras casan
   MENOS instrucciones**. Pasó aquí con un cambio real: +0,097 pp de fuzzy con
   690 → 680 instrucciones exactas y el `score` del permutador de 0,9315 a
   0,9181. Cuando el fuzzy sube y el recuento de diferencias sube también, no es
   una mejora — y `pctsnap` sólo ve el fuzzy.
4. **El permutador guiado dio dos falsos positivos** en esta tanda: su `score`
   compuesto subió en las dos funciones grandes y objdiff dice «tres puntos peor»
   en una y «exactamente igual» en la otra. Y **no compila la unidad entera**, así
   que su recuento de instrucciones puede no ser el real. Detalle en §4.
5. Nueve vedas nuevas medidas (§2), seis pines negativos con su regla (§6) y el
   diagnóstico cerrado de las ocho funciones (§3).

---

## 1. El hallazgo de la ronda: `-ffast-math` REASOCIA, y eso decide una sentencia

`HolePunchAvoidables`, fila 625 del objetivo:

```
802FF470  fmuls f8, f14, f12      ; f14 = delta_offset, f12 = la constante 0,2
802FF4A8  fmadds f1, f31, f8, f15 ; f31 = approach_time, f15 = current_offset
```

o sea `approach_time * (delta_offset * 0,2f) + current_offset`, con el producto
`delta_offset * 0,2f` **evaluado aparte**. Que es exactamente lo que dice nuestra
fuente. Y sin embargo nosotros emitimos:

```
fmuls f9, f31, f0        ; approach_time * 0,2f
fmadds f1, f9, f14, f15  ; (approach_time*0,2f) * delta_offset + current_offset
```

**La causa es `-ffast-math`** (está en los cflags de todas las SourceLists): el
`associate` de `fold` sólo se salta los flotantes cuando NO hay `flag_fast_math`,
así que `a * (b * K)` se reasocia a `(a*K) * b`. Medido con el compilador del
proyecto sobre `float f(float at, float d, float c)`:

| forma | emite |
|---|---|
| `at * (d * 0.2f) + c` | `fmuls at,at,K` + `fmadds r,at,d,c`  <- reasociado |
| `at * (d / 5.0f) + c` | idem (fast-math convierte la división antes) |
| `c + at * (d * 0.2f)` | idem |
| `at * (0.2f * d) + c` | idem |
| `(d * 0.2f) * at + c` | idem, con los operandos del fmadds al revés |
| **`at * 0.2f * d + c`** | **`fmuls d,d,K` + `fmadds r,at,d,c`  <- el objetivo** |
| **`(at * 0.2f) * d + c`** | **igual que la anterior** |
| `float t = d*0.2f; at*t+c` | igual que el objetivo (pero añade local) |
| `at * inline_mul(d,0.2f) + c` | igual que el objetivo |

**La regla, que vale para todo el proyecto**: con `-ffast-math`, `associate`
mueve la constante al OTRO operando. Si el objetivo multiplica la constante por
`b`, la fuente tiene que escribirla pegada a `a`. Un paréntesis no protege nada;
sólo una frontera de sentencia (una local temporal) o una llamada inline lo hace.

### Y aun así es NEGATIVO aquí (ensayo 2, revertido)

Con `approach_time * 0.2f * delta_offset + current_offset`:

- el `fmuls f31,f31,f7` (`approach_time *= close_factor`) **cae en su fila
  correcta, la 639**, en vez de 19 filas antes;
- pero desencadena un renumerado de flotantes —nuestro `f5` pasa a `f7`, `f8` a
  `f4`, `f7` a `f8`— que **rompe seis filas ya casadas** (554, 556, 564, 568,
  591, 598);
- neto **98,133 % -> 97,919 %, de 55 a 64 diferencias**. Revertido.

La local temporal explícita (`float scaled_delta = delta_offset * 0.2f;`) da
**el mismo objeto byte a byte** que la forma sin local: GCC la pliega. O sea que
la veda no es «no metas una local», es que la corrección de asociación sola no
paga; hace falta que venga con el reparto de flotantes arreglado.

El diagnóstico queda escrito en la fuente (`WRoadNetwork.cpp`, justo encima de
la sentencia) para quien la retome.

---

## 2. Vedas nuevas, medidas

| función | ensayo | resultado |
|---|---|---|
| `HolePunchAvoidables` | `approach_time * 0.2f * delta_offset` (arregla la asociación) | 98,133 -> **97,919 %**, 55 -> 64 difs |
| `HolePunchAvoidables` | local `float scaled_delta = delta_offset * 0.2f` | **byte a byte igual** que la anterior |
| `CullParts` | `for (; n < (debug_print = plane_info->NumPlanes); n++)` sin la asignación en el cuerpo | 99,426 -> **96,364 %**, 832 -> 828 B |
| `CullParts` | lo mismo pero CONSERVANDO `debug_print = NumPlanes` en el cuerpo | 99,426 -> **98,876 %**, 828 B |
| `DefragmentPool` | `int zero = 0;` subida a ámbito de función (antes del primer `bMalloc`) | 99,269 -> **96,520 %**; iza el `li r18,0` fuera del bucle |
| `DefragmentPool` | `int zero` declarada la PRIMERA del bloque del `while` | 99,269 -> **98,626 %** |
| `HolePunchAvoidables` | declarar `is_drag` la PRIMERA (para que su `cmpwi` izada cogiera cr3) | 98,133 -> **92,758 %**: `IsDragRace()` es una LLAMADA y reordena el prólogo |
| `CookieTrailCurvature` | la barrera selectiva DETRÁS de la copia (`float w = mx; asm("" : "+f"(mx));`) | idéntico a la base: GCC coalesce igual |
| `CookieTrailCurvature` | el pin `asm("fr13")` SIN barrera | idéntico a la base |
| `HolePunchAvoidables` | la mejor variante del permutador (subir `is_racer` tres sentencias) | 98,133 -> **95,254 %**, 55 -> 93 difs |
| `RenderFlaresOnCar` | la mejor variante del permutador (bajar `ProfileNode profile_node` dos sentencias) | **exactamente igual** en la unidad completa |
| `HolePunchAvoidables` | reasociación + pin de `avoidable_half_width` a fr9 | fuzzy +0,097 pp pero 55 -> 65 difs; revertido (§0.3) |

Los seis pines negativos van en §6, con su regla.

Ninguna deja rastro: `pctsnap --cmp` contra la base da **1 mejora
(`CookieTrailCurvature`) y 0 empeoran**.

---

## 3. Diagnóstico por función

### `HolePunchAvoidables` (2.980 B, 98,133 %, 55 difs)

La base de la r36 tenía 55 diferencias en cuatro racimos, todos leídos con
`fuse.py`:

1. **cr2/cr3 (filas 176, 178, 241, 650)** — dos comparaciones invariantes izadas
   al prólogo: `is_racer` (r29, se usa en la 241) y `is_drag` (r31, en la 650).
   El objetivo le da **cr2 a `is_racer`** (el que se usa antes) y cr3 a
   `is_drag`; nosotros al revés. Las dos `cmpwi` salen en el mismo orden en los
   dos lados, así que no es orden de emisión: es el orden de allocno del banco
   `CR_REGS`. VEDA: declarar `is_drag` la primera cuesta 5,4 puntos
   (`GRaceStatus::IsDragRace()` es una llamada y reordena el prólogo).
2. **filas 524/525** — el mismo par de `lfs` (0x180 = `forward.z`, 0x170 =
   `right.z`) en orden cambiado, dentro de `right_diagonal`. Dos filas. Es
   `sched2`, no orden de fuente: nuestra primera componente SÍ casa
   (`forward.x` antes que `right.x`) y la segunda sale invertida respecto de
   nuestra propia primera.
3. **f10 <-> f11 (ocho filas, 539-587)** — intercambio limpio:
   `avoidable_delta_offset`/`offset_change` son f10 en el objetivo y f11 en el
   nuestro, y el temporal que el objetivo pone en f11 nosotros lo ponemos en
   f10. `avoidable_offset` (declarada después) es f10 en LOS DOS: el objetivo
   REUSA f10, nosotros gastamos f11 primero. Empate del asignador; el pin a fr10
   lo arregla y rompe diez filas más (§6).
4. **filas 619-650 (~26 difs)** — la asociación de §1 más el entrelazado de las
   cuatro expansiones de `bCross`.

**Probado y REVERTIDO, y de aquí sale un aviso que vale para todo el proyecto:**
la reasociación de §1 emparejada con el pin de `avoidable_half_width` a fr9 sube
el **porcentaje difuso de objdiff** (98,133 → 98,230 %) y a la vez **empeora las
otras dos medidas**: las diferencias suben de 55 a 65 —o sea, de 690 a 680
instrucciones EXACTAMENTE iguales de 745— y el `score` del permutador baja de
0,9315 a 0,9181. El porcentaje difuso da **crédito parcial** a una fila que sólo
cambia de registro, así que puede subir mientras casan MENOS instrucciones.
Como un near-miss vale cero bytes en cualquier caso, mando la medida honesta
(instrucciones iguales) y lo dejo en la base. **Regla: cuando el fuzzy sube y el
recuento de diferencias sube también, no es una mejora.**

`regmap`: 85 locales iguales, 0 de más, 0 de menos, mismo árbol de bloques, 4
registros movidos.

### `RenderFlaresOnCar` (2.908 B, 98,618 %, 18 difs)

Confirmado el diagnóstico que ya estaba en la fuente: **nos SOBRA una
instrucción** (2.912 contra 2.908). El objetivo crea UN pseudo `@ha` de
`lbl_8040AD04` (el 0,0f) en la fila 123 —dentro del bloque de `GetCarTypeInfo`,
línea 4061 del original— y ese pseudo vive hasta la fila 693 sirviendo a las tres
cargas de las líneas 4225, 4310 y 4314. Nosotros materializamos DOS: uno local
para la primera carga y otro izado al preencabezado del bucle de flares. De ahí
salen los tres registros de `regmap` (`flashHeadlights` r14/r16, `is_brakelight`
r30/r17, `is_headlight` r17/r16).

Las otras dieciséis cargas del 0,0f las rematerializa el objetivo con un
`lis r9` cada una, igual que nosotros: la diferencia es sólo ese pseudo de
larga vida.

### `CookieTrailCurvature` (1.284 B) — **CERRADA al 100 %**

Faltaba UN `fmr`: el objetivo saca el `Max` a un temporal **volátil** (f13) y
luego copia a f31; nosotros escribíamos directamente en f31 las dos veces.
Quince formas barridas en la r29, todas con binario idéntico.

Se cierra con **barrera selectiva + dos pines de registro** (§6). Y de paso
corrige la regla de la r36 sobre cuándo vale la barrera: su firma NO es sólo
«preservado donde el objetivo usa volátil por cruzar una llamada». Aquí no hay
llamada de por medio y **funciona igual**, porque lo que la barrera impide no es
un adelanto del planificador sino el **coalescing** de dos pseudos que el
asignador podía fundir. Son dos usos distintos de la misma palanca.

### `UpdateWheelYRenderOffset` (876 B, 99,384 %, 7 difs)

Falta un `lis` (872 contra 876). Verificado con `fuse.py`: el objetivo tiene
**DOS pseudos `@ha` de `lbl_8040AA84`** (el 0,0f) vivos en el preencabezado,
r19 y r16 —además de `f26`, que precarga el mismo 0,0f para la línea 3248—, y
usa r16 en la comparación del *width* (línea 3278) y r19 en la del *radius*
(3288). Nosotros compartimos un solo `@ha` (r19) para las dos y con el registro
libre izamos `TweakKitWheelOffsetRear@ha`, que el objetivo rematerializa dentro
del bucle. La fuente ya lleva once formas barridas. Tampoco encaja la firma de
la barrera selectiva (es un registro de dirección, preservado en los dos lados, y
atar un PUNTERO ya costó −3,3 puntos en `GenerateIndex`).

### `CullParts` (836 B, 99,426 %, 5 difs)

Falta UNA instrucción (832 contra 836): un `mr r9, r0` en el preencabezado del
bucle. Leído con `fuse.py`, **ese `mr` pertenece a la línea 661 del original, que
es la del propio `for`**, y va DESPUÉS del `cmpw r9, r0` de la guarda. O sea que
el objetivo deja `debug_print = NumPlanes` hecho aunque el bucle no se ejecute
—y la guarda todavía usa el 0 viejo de `debug_print`—. La lectura natural es que
la asignación va en la CONDICIÓN del `for`; probado en las dos formas (con y sin
la asignación en el cuerpo) y las dos EMPEORAN (ver §2): la forma da 828 B, no
836.

Causa raíz distinta y más simple: el objetivo tiene `Polarity` en **r0** y
nosotros en **r11**; como en el nuestro r11 vale 0 después del `bne`, GCC lo
reusa como cero para la guarda (`cmpw r11, r0`) y ya no necesita el `mr`. El
objetivo, con `Polarity` en r0 —que se pisa acto seguido con `NumPlanes`— tiene
que tirar del cero de `debug_print` (r9). No he encontrado palanca para forzar
`Polarity` a r0.

### `InitAtSegment` (816 B, 99,152 %, 42 difs)

Confirmado el diagnóstico de la r29 que ya está en la fuente y NO lo he movido:
una sola decisión de registro (`laneInd` r24 objetivo / r26 nuestro) más **12 B
de pila jamás referenciada** (0x50-0x57 y 0x60-0x63), que dejan el marco en 0x98
contra el 0x90 del objetivo. Hacen falta las dos cosas a la vez. El permutador
guiado con profundidad 2 y la barrera selectiva ya estaban barridos.

### `DefragmentPool` (684 B, 99,269 %, 23 difs) — ESTRUCTURA CERRADA

**Las 23 diferencias son PURA PERMUTACIÓN de registros y el tamaño ya casa
(684/684).** Tres ciclos independientes:

```
this              objetivo r21   nuestro r22      (transposición con...)
allocation_num+1  objetivo r22   nuestro r21
num_hole_filling  objetivo r27   nuestro r25      (transposición con...)
ChunkMovementOffset@ha objetivo r25   nuestro r27
CarLoaderMemoryPoolNumber@ha objetivo r17  nuestro r16   (ciclo de tres)
table (dirección) objetivo r18   nuestro r17
zero              objetivo r16   nuestro r18
```

Y **corrijo el diagnóstico del triaje de la r36** sobre el árbol de bloques. El
volcado DWARF del original dice:

```
b1 (cuerpo del while)  [+0x14C, +0x1FC]   allocation, allocation_size, movement
  b1/b0                [+0x170, +0x1FC]   VACIO: ni locales ni inlines
    b1/b0/b0           [+0x198, +0x1C8]   hole
```

y el nuestro:

```
b1                     [+0x14C, +0x1FC]   params, table, zero, allocation, allocation_size, movement
  b1/b0                [+0x198, +0x1FC]   hole
```

O sea: los dos abren el bloque de `hole` en **+0x198**; lo que el objetivo tiene
de más es un bloque **vacío** que arranca en +0x170 (el cuerpo del `if`) y que a
nosotros GCC no nos emite. **Y no se puede fabricar con llaves ni con ninguna
construcción normal**: comprobado compilando una reducción mínima con los cflags
del proyecto y volcando el DWARF con `dtk dwarf dump`. `dwarfout.c` de GCC 2.9
sólo emite un `lexical_block` si `BLOCK_VARS != NULL` —si el bloque declara algo
DIRECTAMENTE—; un bloque vacío se salta y sus hijos se cuelgan del padre. Probado:

| forma | árbol que sale |
|---|---|
| lo nuestro (`if { for(;;) { void *hole; } }`) | 2 niveles (`hole` colgando del `while`) |
| llaves extra alrededor del `for` | **2 niveles** — el bloque vacío NO se emite |
| `do { } while(1)` en vez del `for` | 2 niveles |
| declaración cualquiera en el cuerpo del `if` | **3 niveles**, pero con esa local en medio |
| `for (int spin = 0;; spin++)` | 3 niveles, con `spin` en medio |
| `if (int guard = 1)` | 3 niveles, con `guard` en medio |

Faltaba una fila en esa tabla, y **es la que cierra el frente**:

| forma | árbol que sale |
|---|---|
| `typedef int foo_t;` en el cuerpo del `if` | 3 niveles, pero el typedef SE VE |
| una etiqueta (`label:`) | 2 niveles (la etiqueta sube a ámbito de función) |
| **`enum { kX = 1 };` en el cuerpo del `if`** | **3 niveles y el bloque sale VACÍO** |
| **`struct MyTag { int a; };`** | **igual: 3 niveles, bloque VACÍO** |

Una **etiqueta de tipo** llena `BLOCK_VARS` con su `TYPE_DECL` (que es lo que
`dwarfout` mira para decidir si emite el bloque) pero el volcado saca la
definición del tipo a las *Inner declarations* de la función, así que el bloque
queda vacío — exactamente la forma del objetivo. Y **no emite ni una
instrucción**.

Aplicado (`enum { kHoleProbeSize = 1 };` al principio del cuerpo del `if`), con
el comentario que explica el mecanismo en `CarLoader.cpp`:

```
antes  b1/b0 falta, "hole" en el ambito equivocado, 2 desajustes de bloque
ahora  hole en b1/b0/b0 en LOS DOS, 0 desajustes, 0 locales de ambito equivocado
.text  684/684, 99,269 %  ->  BYTE A BYTE IDENTICA
```

Lo que queda son los tres ciclos de registros de arriba: **`triaje` ya lo llamaba
PERMUTADOR y ahora `regmap` no le lleva la contraria**. Vía: el permutador.

Nota: las tres locales «sólo nuestras» (`params` r26, `table` r17, `zero` r18) no
son ruido: el objetivo materializa esos mismos tres pseudos (r26 para la
dirección de `DefragmentParams`, r18 para la de la tabla, r16 para el cero), sólo
que sin nombre. Quitarlas no es la corrección — moverlas tampoco (§2).

### `SetMemoryPoolSize` (304 B, 97,368 %, 2 difs) — no venía en el encargo

Dos diferencias, y son UNA: el `stw r0, CarLoaderMemoryPoolNumber@l(r29)` de
`CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum();` sale en el hueco de ANTES
del `addi r6, r6, $LC@l` que prepara la cadena `"Cars"` para `bInitMemoryPool`, y
el objetivo lo pone DESPUÉS. Mismo tamaño, mismas instrucciones, mismo orden en
todo lo demás; `regmap` dice IDENTICO. Es un empate puro de `sched2` entre dos
instrucciones sin dependencia — la familia que la r36 ya dio por inalcanzable
desde la fuente en `AV_PLAYER::GetFirstFrame` y `VDevice_RecalcGammaTable`.

---

## 4. Permutador

Lanzado `--guided --depth 2 --combos 300 --anneal 0.02 -j 6` sobre las dos
funciones grandes (16 núcleos; dos procesos de 6 dejan margen para compilar).

Las dos tandas terminaron, **las dos con la misma variante en la ronda 1 y nada
mejor en las rondas 2 y 3**, y **las dos son un FALSO POSITIVO**. Esto es lo más
importante que sale de la sección:

| función | mejor variante | score permutador | y con objdiff |
|---|---|---|---|
| `HolePunchAvoidables` | `move_stmt:move-3@24` = subir `const bool is_racer` tres sentencias, detrás de `nav_forward_3d` | 0,9315 -> **0,9423** | 98,133 -> **95,254 %**, 55 -> **93** difs |
| `RenderFlaresOnCar` | `move_stmt:move+2@0` = bajar `ProfileNode profile_node` dos sentencias | 0,9712 -> **0,9752** | **exactamente igual** (2.912 B, 18 difs) |

Dos avisos que valen para cualquiera que use la herramienta:

1. **El `score` compuesto del permutador puede subir mientras su propio
   subíndice `opcodes` baja.** En `HolePunchAvoidables`: score 0,9315 -> 0,9423
   pero `opcodes` 0,9893 -> 0,9812. El compuesto pesa mucho el reparto de
   registros; el de opcodes es el que sigue a objdiff. **Mide con `fndiff` antes
   de aplicar nada que salga del permutador.**
2. **El permutador NO compila la unidad entera**: monta un envoltorio con el
   prefijo del SourceList hasta el fichero objetivo y sólo ese fichero. En
   `RenderFlaresOnCar` su variante daba **727 instrucciones** —el número exacto
   del objetivo, que es lo que la hacía tan atractiva— y en la unidad completa da
   **728, igual que la base**: el contexto no es el mismo. Ahí no hay ganancia,
   sólo una medida en otro entorno.

(`ProfileNode profile_node` además va la PRIMERA en el DWARF del original, así
que bajarla también contradecía el volcado. Revertida.)

**Nota de contabilidad**: `zWorld` y `zWorld2` son `linked False` en
`build.ninja`, así que el DOL sigue enlazando los objetos ORIGINALES de las dos
unidades y **nada de esta ronda puede romperlo**. `lcfix.py --check` limpio
después de cada recompilación.

---

## 5. La etiqueta de tipo: alcance fuera de zWorld

La palanca no es de esta función. Contados sobre `symbols/mw_dwarfdump.nothpp`
los bloques anónimos cuyo contenido DIRECTO está vacío (sólo la línea `Range` y,
como mucho, bloques anidados):

```
bloques anonimos VACIOS en el DWARF del original: 90
funciones afectadas: 76   (contando solo firmas NO inline)
```

Entre ellas `CarLoader::DefragmentPool` (la de aquí), `CarRenderInfo::Render`,
`CarLoader::RemoveSomethingFromCarMemoryPool`, `ConfirmDialog::OpenModalDialog`,
`DisculatorDriver::LoadGiantFiles`, `CARSFX_Rain::QueueWeatherStream`… No todas
son trabajo pendiente —muchas ya casan al 100 %—, pero **cada vez que `regmap`
diga «el bloque bX/bY FALTA en el nuestro» y no nombre ninguna local del original
dentro, la respuesta es una etiqueta de tipo, no unas llaves**. Coste: cero
bytes. Comprobación: `regmap … --all` antes y después.

Receta:

```c
if (cond) {
    enum { kAlgo = 1 };   // abre el bloque; no emite nada
    ...
}
```

Y la regla general que sale de la reducción mínima (medida con los cflags del
proyecto, no deducida): **GCC 2.9 emite un `lexical_block` sólo si el bloque
declara algo DIRECTAMENTE** (`BLOCK_VARS != NULL` en `dwarfout.c`); si no, lo
salta y **cuelga sus hijos del padre**. Por eso unas llaves de más nunca se ven,
y por eso una local de más se ve DEMASIADO. La etiqueta de tipo es el único
punto medio.

---

---

## 6. La palanca nueva: barrera SELECTIVA + PIN de registro

`CookieTrailCurvature` estaba al 99,657 % con 3 diferencias desde la r25, con
**quince formas de la sentencia barridas** y el veredicto escrito en la fuente:
«es coalescing puro, no queda fuente que corregir». Era verdad — y aun así se
cierra.

El objetivo:

```
fmr f13, f0          ; el Max escribe en un VOLATIL
fcmpu ... / bso
fmr f13, f30
mr  r4, r29
fmr f31, f13         ; y DESPUES copia al preservado   <- esta faltaba
```

Nosotros escribíamos directamente en f31 las dos veces: el asignador coalesce el
temporal con la local porque **puede** darles el mismo registro, y ninguna forma
de la fuente se lo impide.

Las dos piezas, medidas paso a paso:

| paso | resultado |
|---|---|
| base | 99,657 %, **1280** B (falta el `fmr`) |
| `asm("" : "+f"(mx))` entre el `Max` y la copia | 99,081 %, **1284** B — aparece el `fmr`, pero la copia cae en f27 y arrastra un ciclo de cuatro (f27→f31→f30→f28→f27) |
| + `register float mx asm("fr13")` | 99,704 %, 1284 B |
| + `register float apex_width asm("fr31")` | **100,000 %** |

La receta:

```c
register float tmp asm("fr13") = <expresion>;   // el registro del OBJETIVO para el temporal
asm("" : "+f"(tmp));                            // impide el coalescing: fuerza la copia
register float dest asm("fr31") = tmp;          // el registro del OBJETIVO para la local
```

**Sintaxis, que costó un intento**: el nombre del banco de coma flotante en el
GCC 2.9 de PowerPC es **`fr13`, no `f13`** (`invalid register name`). Los enteros
sí son `r24`. Y **no se puede pinchar un PARÁMETRO**: `register char b asm("r24")`
en la firma es *parse error*; hay que hacer una copia local, que añade una local.

### Dónde NO vale: siete negativos medidos

El pin **no es un ajuste fino, es una restricción dura**: el asignador reusa ese
registro para otros pseudos cuando el pinchado no está vivo, y en una función
saturada eso desplaza más de lo que arregla.

| función | pin | resultado |
|---|---|---|
| `DefragmentPool` | los tres a la vez (`num_hole_filling`→r27, `table`→r18, `zero`→r16) | 99,269 -> **93,678 %** |
| `DefragmentPool` | sólo `zero`→r16 | 99,269 -> **97,865 %** — el `stw r16` SÍ casa, pero r16 se reusa en otras cinco filas |
| `HolePunchAvoidables` | `avoidable_delta_offset` y `offset_change` a fr10 | 98,133 -> **98,001 %** — las ocho filas de f10 casan y se rompen diez más (`extra_width` se va de f2 a f30) |
| `HolePunchAvoidables` | `hole_punch_safety_margin` a fr4 | 98,133 -> **97,376 %** |
| `RenderFlaresOnCar` | `flashHeadlights` a r14 (para liberar r16 al `@ha`) | 98,618 -> **97,853 %** |
| `InitAtSegment` | copia local de `laneInd` pinchada a r24 | 99,152 -> **95,926 %**, +4 B |
| `CullParts` | copia local de `Polarity` pinchada a r0 (es donde la tiene el objetivo) | 99,426 -> **98,421 %** |

**La regla que sale**: el pin vale cuando el valor tiene **vida corta y el racimo
es pequeño** —en `CookieTrailCurvature` son dos valores y el temporal vive tres
instrucciones—, y cuesta caro en cuanto la función está saturada de registros
del mismo banco. Y **la barrera va primero**: sin ella el pin solo no hace nada
(medido: `register float mx asm("fr13")` sin barrera da los 1280 B de la base,
GCC coalesce igual).

---

## 7. Qué queda, y por dónde

Verificación final (`build_direct` de las dos unidades, `pctsnap --cmp` contra la
base tomada al empezar): **1 mejora, 0 empeoran**; `lcfix.py --check` limpio;
`regmap` de `DefragmentPool` con **0 desajustes de bloque**.

Las siete que siguen abiertas, ordenadas por lo que yo haría:

1. **`DefragmentPool` (684 B, 23 difs)** — el mejor premio del grupo: el **tamaño
   ya casa** (684/684), la estructura ya casa desde esta ronda, y lo único que
   queda son **tres ciclos de registros** enteros. No hay nada que leer.
2. **`RenderFlaresOnCar` (2.908 B)** — nos SOBRA un `lis`; el objetivo comparte
   UN pseudo `@ha` del 0,0f entre las tres cargas del bucle y nosotros
   materializamos dos. Diagnóstico cerrado, palanca sin encontrar.
3. **`HolePunchAvoidables` (2.980 B, 55 difs)** — cuatro racimos identificados
   (§3). El de la reasociación (§1) es una corrección REAL que aún no paga.
4. **`CullParts` (836 B)** y **`UpdateWheelYRenderOffset` (876 B)** — a UNA
   instrucción cada una, las dos con once y quince formas ya barridas en la
   fuente. Ahí no metería más horas hasta que aparezca otra palanca.
5. **`InitAtSegment` (816 B)** — un registro más 12 B de pila muerta; hacen falta
   los dos a la vez (r29 ya lo dejó escrito).
6. **`SetMemoryPoolSize` (304 B)** — empate puro de `sched2`, dos instrucciones
   sin dependencia. Misma familia que `GetFirstFrame` y `RecalcGammaTable`.

**Fuera de mi territorio, para quien reparta**: la etiqueta de tipo (§5) es una
palanca de coste cero que se puede pasar por todas las unidades donde `regmap`
diga «el bloque bX/bY FALTA en el nuestro». Y las dos reglas de medida (§0.3 y
§4) valen para cualquier agente: el fuzzy de objdiff da crédito parcial, y el
`score` del permutador no es un proxy fiable — hay que confirmar con `fndiff`.
