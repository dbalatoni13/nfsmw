# r46 — zWorld / zWorld2 / zTrack

Territorio: 10.112 B en 8 funciones. **+0 B de `matched_code`** (ninguna llega al
100 %), pero `HolePunchAvoidables` pasa de **44 a 9 instrucciones distintas** por
tres correcciones ESTRUCTURALES de fuente, **sin añadir un solo `asm`** (de hecho
se retira un pin que sí hacía falta en el camino).

Estado final de las tres unidades, `fncmp`:

| unidad | antes | después |
|---|---|---|
| zWorld | 5 funciones / 5.608 B | **igual** (fuente restaurada byte a byte) |
| zWorld2 | `HolePunch` 44 insn, `InitAtSegment` 15 | **`HolePunch` 9 insn**, `InitAtSegment` 15 |
| zTrack | `GetLoadingPriority` 72 insn | **igual** |

`lcfix.py --check`: limpio. `audit.py` en las tres: 0 FALLA. Ninguna función
empeora.

---

## 1. `HolePunchAvoidables` — 49 → 9 filas, 98,16913 → 99,919464 %

Tamaño 2980/2980 en todos los ensayos. La cadena completa, medida paso a paso:

| # | cambio (acumulativo) | filas | fuzzy |
|---|---|---:|---:|
| — | base de entrada | 49 | 98,16913 % |
| E2 | `bCross(&cookie_to_avoidable, &Forward)` en `avoidable_offset` | 49 | 97,80805 % |
| E3 | + `approach_time * 0.2f * delta_offset` | **35** | 98,40671 % |
| E4 | + pin `avoidable_offset` a `fr10` | **27** | 98,744965 % |
| E8 | + `bMax(bAbs(left), bAbs(right))` en UNA sentencia | **18** | 99,080536 % |
| E11 | + fundir el `+= extra_width * close_factor` en esa misma sentencia | **9** | 99,919464 % |
| E18 | − el pin de E4 (ya no hace falta) | **9** | 99,919464 % |

E18 produce un objeto **byte a byte idéntico** a E11: el pin era andamio y se
retira. Lo que queda aplicado son **tres cambios de fuente y cero `asm` nuevos**.

### 1.1 La causa raíz: `bCross` con los argumentos al revés

`bCross(a,b)` es `a->x*b->y - b->x*a->y`, o sea `fmuls t = b.x*a.y` y después
`fmsubs r = a.x*b.y - t`. El objetivo emite, para `avoidable_offset`:

```
802FF4AC  fmuls  f10, f6, f10          ; F.x * c2a.y
802FF4BC  fmsubs f10, f3, f5, f10      ; c2a.x*F.y - eso
```

con f5 = `cookie.Forward.y`, f6 = `cookie.Forward.x` (confirmado por los otros
tres `bCross` del bloque y por el `bDot` de `avoidable_d`), y f3 = `cookie_to_avoidable.x`
(`0x1c0(r1)`, r10 = `addi r10,r1,0x1c0`). Eso es `bCross(&cookie_to_avoidable,
&cookie.Forward)`. Nuestra fuente tenía `bCross(&cookie.Forward,
&cookie_to_avoidable)`, que es el **signo contrario**, y era la única de las
cuatro llamadas del bloque con el orden `(Forward, vector)`.

**Esa era la veda de verdad.** La reasociación de `new_current_offset` estaba
diagnosticada como correcta desde la r36b y medida como negativa tres veces
(r36b/r36d/r36e: 55 → 64 filas, y 49 → 58 sobre la base de r36e). Con el
`bCross` corregido **gana 14 filas de golpe**. Es un caso de libro de
[[nfsmw-vedas-caducan]]: se había barrido N formas de la sentencia equivocada.

### 1.2 Cómo se lee que el `bMax` y el `+=` son la MISMA sentencia

`bMax(a,b)` de `bMath.hpp` emite `fsubs c = a-b` y `fsel d,c,a,b`. El objetivo:

```
802FF498  fsubs  f0, f13, f9           ; a = bAbs(left), b = bAbs(right)
802FF4A0  fsel   f0, f0, f13, f9       ; el resultado va a un TEMPORAL, f0
802FF4B8  fmadds f9, f2, f7, f0        ; y la LOCAL es el destino del fmadds
```

Dos cosas a la vez: (a) el primer argumento de `bMax` es `bAbs(left_projection)`,
no `bAbs(right_projection)`; (b) como el `fsel` escribe en un temporal y la local
nace en el `fmadds`, el `bMax` y el `+= extra_width * close_factor` son **una
sola sentencia**. Escribirlas separadas cuesta 9 filas.

Regla generalizable: **si el `fsel` de un `bMin`/`bMax` escribe en un registro
que muere en la instrucción siguiente, esa siguiente instrucción es la misma
sentencia de fuente.**

### 1.3 Lo que queda (9 filas) y lo que se probó contra ello

`regmap` da **MISMO conjunto de locales, mismo árbol de bloques y MISMO REPARTO**
(89 de 89). Las nueve filas son:

- **4 filas de `cr2`/`cr3`** (`is_racer` ↔ `is_drag`). Empate de
  `allocno_compare` con el mismo `n_refs` y el mismo rango.
- **5 filas de los temporales del `bCross` de `avoidable_delta_offset`**: el
  objetivo reparte F.x→f10, F.y→f9, av.x→f0 y encadena las tres cantidades en
  f10 (el `fmsubs` ata su destino al operando 3, que muere); con el pin `fr10`
  nosotros damos F.x→f9, F.y→f8, av.x→f10. El pin **prohíbe** esa cadena (el
  destino está precoloreado y no puede coalescer con la entrada moribunda).

MEDIDO Y NEGATIVO sobre la base de 9 filas (todo 2980/2980 salvo donde se dice):

| ensayo | filas |
|---|---:|
| las 6 permutaciones de `is_racer`/`is_traffic`/`is_drag` — RTD (actual) | **9** |
| … TRD 27 · TDR 31 · RDT 61 · DRT 68 · DTR 76 | 27..76 |
| `asm("" : "+f"(new_current_offset) : "r"(is_drag))` (subir `n_refs`) | 105, **+16 B** |
| quitar el pin `fr10` de `avoidable_delta_offset` | 15 |
| … + `asm("" : "+f"(avoidable_delta_offset))` | 21 |
| … + `asm("" : "+f"(offset_change) : "f"(avoidable_delta_offset))` | 35 |
| mover el pin a `offset_change` | 27 |
| quitar la barrera `asm("" : "+f"(offset_change))` | 58 |
| `register float avoidable_half_width asm("fr9")` sobre la base de 27 | 55 |
| `bMax` en dos sentencias con `bAbs(left)` primero | 54 |
| mover `approach_time *= close_factor;` arriba (línea 3778 del original) | 9 — **objeto idéntico** |

El pin de `avoidable_delta_offset` **cuesta 5 filas y ahorra 10**: se queda. Sin
él, los cinco temporales casan EXACTOS pero la local cae en f11 y arrastra diez
filas de la cadena `offset_change`/`cut_to_position` (permutación limpia
f10↔f11).

El movimiento de `approach_time *= close_factor;` está respaldado por el mapa de
líneas (`WRoadNetwork.cpp:3778` del original, **antes** de la 3783 de
`avoidable_half_width`), pero da objeto idéntico en las dos bases: confirma
[[nfsmw-barrera-selectiva]] («mover sentencias independientes dentro del mismo
bloque básico no cambia ningún `live_length`»). **No se aplica** para no mover
código sin ganancia.

---

## 2. `GetLoadingPriority` (zTrack, 708 B) — las dos bases medidas

El brief pedía medir las dos bases antes de elegir. Hechas las dos:

| base | filas | fuzzy | de esas, sólo el marco (+0x10) | filas REALES |
|---|---:|---:|---:|---:|
| la de HEAD (tres sentencias) | 75 | 94,72317 % | 37 | **38** |
| una sola expresión (la de antes de r37) | 71 | 97,18079 % | 37 | **34** |

**Se queda la de HEAD**, y el porcentaje miente. Con la expresión en una sola
sentencia GCC emite `fmuls f30,f30,f11` = `speed_factor * 0.66999996f` **antes**
del producto angular — un árbol que el objetivo NO tiene. Con las tres
sentencias el racimo 153-159 es instrucción por instrucción el del objetivo
(`fsubs`, `*K1`, `*speed_factor`, `*K2`, `1-x`, `*distance`, `*K3`) y sólo
cambian los registros. El 2,5 % de más de la otra base es crédito parcial de
`objdiff` a filas que sólo cambian de registro.

### 2.1 EVIDENCIA NUEVA: dónde están los 16 B del marco

Enfrentando entrada por entrada el DWARF del original
(`symbols/mw_dwarfdump.nothpp:2177094`) con el nuestro (`dtk dwarf dump` de
`zTrack.o`), **el árbol de bloques y la lista de inlines son IDÉNTICOS** — no
falta ninguna variable declarada. Lo único que cambia:

```
                              original   nuestro
  layer_name[32]              r1+0x8     r1+0x8     (igual)
  pos                         r1+0x28    r1+0x28    (igual)
  face[4]  EXTERIOR           r1+0x38    r1+0x38    (igual)
  face[4]  INTERIOR           r1+0x48    r1+0x68    <-- ÚNICA diferencia previa
  dest (inline bScale)        r1+0xA8    r1+0x98    ) todo lo que viene
  predict_pos                 r1+0xB8    r1+0xA8    ) después está
  direction                   r1+0xC0    r1+0xB0    ) desplazado
  v                           r1+0xC8    r1+0xB8    ) exactamente +0x10
```

O sea: **los 16 B no son una local viva, están dentro del bloque muerto**
`if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating)`. Al original ese
bloque le ocupa 0xA0 B (0x8..0xA8) y a nosotros 0x90 (= 32 de `layer_name` + 16
de `pos` + 48 + 48, exacto). Lo que falta no es una variable sino **la FORMA de
declaración** que hace que GCC deje sólo 0x10 entre el `face` exterior y el
interior y 0x60 detrás del interior.

Sigue vigente la prohibición de rellenarlo con una local inventada (precedente
`9de121ee`). El resto de la función, quitando las 37 filas mecánicas del marco,
son 38 filas de reparto FP.

---

## 3. `RenderFlaresOnCar` (zWorld, 2.908 B) — veda nueva, y da el TAMAÑO EXACTO

Le sobra un `lis lbl_8040AD04@ha`: el objetivo crea UN pseudo `@ha` en el bloque
de entrada (fila 123, `CarRender.cpp:4061` del original) y lo mantiene en r16
hasta la fila 601; nosotros materializamos dos en el preencabezado del bucle.

**Basta UNA referencia a `lbl_8040AD04` antes del bucle** (por ejemplo
`float base_headlight_intensity = lbl_8040AD04;`) para que desaparezca el `lis`
izado: **2912 → 2908 B, tamaño EXACTO**. Pero el reparto entero se corre un
registro (`regmap`: `position` r15→r16 y `flashHeadlights` r14→r15, las dos
únicas que señala) y las filas suben de **18 a 62** (98,61761 → 98,05364 %).
Las dos formas (`headlight` o `brakelight`) dan el MISMO objeto.

También medido:

- Mover las dos declaraciones detrás de `car_type_info` —que es donde el mapa de
  líneas coloca las 4059/4060 del original, entre la 4057 del `GetCarTypeInfo` y
  la 4061 del `is_traffic_car`— es **neutro**: objeto idéntico, 18 filas.
- Sobre la base de 62 filas, `register unsigned int flashHeadlights asm("r14")`
  da **81** filas: el reparto es SÍNTOMA ([[nfsmw-si-el-pin-empeora]]).

Retirado. Con la referencia puesta seguimos materializando DOS `@ha` en el
preencabezado y nos falta el del bloque de entrada: el problema no es cuántos
`lis` hay sino **dónde nace el pseudo**. Anotado en `CarRender.cpp`.

---

## 4. `DefragmentPool` (zWorld, 684 B) — el veredicto de `regmap` no paga

`regmap` da **tres locales que sólo tenemos nosotros** (`params`, `table`,
`zero`, todas en el ámbito b1) y su veredicto manda quitarlas antes de tocar
registros. Medido, base 23 filas / 99,269005 % / 684 B:

| ensayo | filas | fuzzy | B |
|---|---:|---:|---:|
| las tres fuera | 47 | 92,47369 % | **692** |
| sólo `zero` fuera | 29 | 97,95322 % | 684 |
| sólo `table` fuera | 23 | 98,62573 % | 684 |
| sólo `params` fuera | 43 | 94,14035 % | **692** |

Son andamio deliberado (fijan la base del `lwzx` y la constante del store) y hay
que dejarlas. Lo que queda son tres ciclos de registro: r21↔r22 (`this`),
r16→r18→r17→r16 y r25↔r27. Anotado en `CarLoader.cpp`.

**Aviso de herramienta**: «N locales sólo nuestras» no implica que sobren. Aquí
las tres son las que producen la secuencia del objetivo aunque el DWARF no las
nombre.

---

## 5. `CullParts` (zWorld, 836 B) — diagnóstico afinado, sin ensayos nuevos

Le falta un `mr r9, r0` en el preencabezado. Leído el mapa de líneas y el flujo:
al objetivo `r9` le llega `NumPlanes` **antes** del bucle (`mr r9,r0` metido
entre el `cmpw` de la guarda y el `bge`, o sea que se ejecuta también cuando el
bucle no entra) y por eso la comparación posterior `cmpw r31, r9` compara `n`
contra `NumPlanes`; nosotros dejamos ahí el 0 de `debug_print` y encima usamos
r11 para `Polarity` donde el objetivo usa r0. Las vedas ya escritas en la fuente
(inicializar `debug_print` a `NumPlanes` → 824 B; asignarlo también antes del
bucle → 828 B; `register int polarity asm("r0")` → 5→9 difs) siguen vigentes; no
se han repetido.

`UpdateWheelYRenderOffset`, `SetMemoryPoolSize` e `InitAtSegment` no se han
tocado: sus vedas (r36d/r36e/r39/r40/r43) cubren los ejes que quedaban y no ha
aparecido evidencia nueva para ninguna.

---

## Propuestas fuera de territorio

1. **Auditar el orden de argumentos de `bCross` en todo el árbol.** Hay **35
   llamadas** a `bCross(` fuera de `bMath.hpp` (`eMathE.cpp`, `bVector.cpp`,
   `Gps.cpp`, `CameraMover.cpp`, `Cubic.cpp`, `Showcase.cpp`, `TrackCar.cpp`,
   `TrackCop.cpp`, `GTrigger.cpp`, `LuaGameHooks.cpp`, `CarRender.cpp`,
   `WRoadNetwork.cpp`, `HeliSheet.cpp`). El orden se **lee** del asm sin
   adivinar: `fmuls t = b.x*a.y` y `fmsubs r = a.x*b.y - t`, así que la carga que
   alimenta el `fmuls` identifica el SEGUNDO argumento. En `HolePunchAvoidables`
   una sola llamada mal puesta tenía vedadas tres palancas distintas durante
   cinco rondas. Interesa sobre todo en las unidades con near-miss (zCamera:
   `CameraMover`, `Cubic`, `Showcase`, `TrackCar`, `TrackCop`).
2. **Regla nueva para el catálogo**: el destino del `fsel` de un `bMin`/`bMax`
   dice si la sentencia sigue. Si el `fsel` escribe en un temporal que muere en
   la instrucción siguiente, esa instrucción pertenece a la MISMA sentencia de
   fuente; si escribe en la local, la sentencia acaba ahí. Aquí valió 9 filas.
3. **Corrección de una veda del catálogo**: «reasociar `at * (d*K)` a `at*K*d`
   cuesta» era falso — costaba porque la base tenía otro error. Antes de dar por
   agotada una palanca de reasociación, comprueba que el resto de la sentencia
   (orden de argumentos incluido) case con el objetivo.

## Artefactos

`scratchpad/r46_world/`: `hpa_base.txt`, `hpa_E3.txt`, `hpa_E4.txt`,
`hpa_E8.txt`, `hpa_E11.txt` (los `fndiff` de cada escalón), `hpa_lmap.txt`,
`rf_base.txt`, `rf_A1.txt`, `rf_A2.txt`, `rf_A_H.txt`, `rf_A_B.txt`,
`rf_A_B_pin14.txt`, `rf_lmap.txt`, `dp_base.txt`, `dp_D1.txt`, `dp_zero.txt`,
`dp_table.txt`, `dp_params.txt`, `glp_base.txt`, `glp_B.txt`, `glp_lmap.txt`,
`framerows.py` (cuenta cuántas filas de un `fndiff` son sólo el desplazamiento
de marco), `m.sh`, `order.py`, `move.py` y las copias de seguridad de las cuatro
fuentes tocadas.
