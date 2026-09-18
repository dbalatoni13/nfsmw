# r48 — zWorld / zWorld2 / zTrack

Territorio: 10.112 B en 8 funciones. **+0 B de `matched_code`** (ninguna llega al
100 %). Lo que sí deja la ronda:

- `InitAtSegment` pasa de **15 a 12 filas** (99,5588 → **99,6569 %**, 816/816)
  **quitando** andamio, no poniéndolo: fuera un pin de registro que era síntoma y
  dentro la inline `SetLaneInd` que el DWARF del original sí tiene.
- **La rama del «+7 insns» de `HolePunchAvoidables` queda CERRADA con la cifra**:
  el padding arregla el empate `cr2`/`cr3` (reparto EXACTO, comprobado en el
  `.greg`) pero rompe un **segundo** empate de truncamiento, y los dos conjuntos
  de longitudes válidas **no se cortan** salvo en N = −1. O sea: el único camino
  que queda es *un insn menos*, y ya no hay que volver a probar padding.
- El diagnóstico de marco de `GetLoadingPriority` que pedía el brief, hecho: los
  16 B **no son un hueco**, son 32 B ahorrados y 48 gastados en sitios distintos.
- El diagnóstico de `SetMemoryPoolSize` de la r47 (**«desempate por LUID»**)
  queda **desmentido** con la traza del planificador: las dos instrucciones se
  emiten **en el mismo ciclo**, en unidades distintas.
- Dos herramientas nuevas: la **traza de `sched` ciclo a ciclo**
  (`scripts/schedtrace.py`) y la **TU reducida** que reproduce la función byte a
  byte en 11 s en vez de 40.

## Verificación

`fncmp` antes / después de las TRES unidades (ficheros completos en
`scratchpad/r48_world/base_fncmp_*.txt` y `final_fncmp_*.txt`):

| unidad | antes | después |
|---|---|---|
| zWorld | 5 funciones / 5.608 B | **idéntico** (fuente sin tocar) |
| zWorld2 | `HolePunch` 4 insn, `InitAtSegment` **15** | `HolePunch` 4 insn, `InitAtSegment` **12** |
| zTrack | `GetLoadingPriority` 72 insn | **idéntico** |

`diff` de los tres `fncmp`: la **única** línea que cambia en todo el territorio
es la de `InitAtSegment` (15 → 12). **Ninguna función empeora.**
`lcfix.py --check`: «todas las entradas @lc estan al dia».
`audit.py` en las tres: **0 FALLA**.
`git status`: sólo `WRoadNetwork.cpp` (código + comentarios), `CarLoader.cpp` y
`TrackStreamer.cpp` (**sólo comentarios**). `CarRender.cpp` sin tocar.

---

## 1. `HolePunchAvoidables` (2.980 B, 4 filas) — la rama del «+7» queda cerrada

La r47 dejó cuantificado el empate: `pri = int(30000/live_length)`,
`live(505)=483 → 62`, `live(901)=492 → 60`, `live(334)=494 → 60`, empate entre
901 y 334 que gana el pseudo menor. Y dejó abiertas dos salidas: **−1/−2 insns**
(quitar la barrera; arregla el reparto pero rompe la FP: 46 filas) o **+7/+8**.

### 1.1 El modelo, leído entero del volcado

`live(x) = T + s(x)`, donde `s(x)` son los insns que van desde la definición de
`x` hasta el final del preencabezado. Del `.lreg`:

```
insn 3254  (set (reg:CC 334) ...)   is_racer
insn 3341  lfs  $LC564  (el literal 0,2f)      <- 1 insn de por medio
insn 3342  (set (reg:CC 901) ...)   is_drag
insn 3325  lfs  $LC554 (-6,0f)
insn 3235..3239  5 x addi r31+K
insn 3253, 3268  2 x high(simbolo)             <- 8 insns de por medio
insn 3283  (set (reg:CC 505) ...)  is_traffic
```

s(334)−s(901) = 2 y s(901)−s(505) = 9, que es **exactamente** la diferencia de
las tres `live_length`. Cuadra al insn, y con eso el efecto de cualquier padding
se predice sin compilar.

**Corolario medido y nuevo: los insns de fuente ANTERIORES al bucle no cuentan.**
Un `asm("")` delante del `for` cae **delante de los movables**, o sea delante de
las tres definiciones: `live` sale 483/494/492 **idéntico** y el objeto también.
Los movables los inserta `move_movables` (loop.c) pegados a `loop_start`, después
de todo el código de fuente del preencabezado.

### 1.2 El «+7» funciona… y destapa un SEGUNDO empate

Con 7 insns de cero bytes **dentro** del bucle el reparto de los CR sale
**EXACTO** (`505→cr4, 901→cr3, 334→cr2`, live 490/499/501, pri 61/60/59,
comprobado en el `.greg`). Pero:

| dónde van los 7 `asm("")` | filas | tamaño |
|---|---:|---|
| rabo del bucle (`tail7`) | 17 | 2980/2980 |
| … el mismo sitio con 1 sola / con 8 | 10 / 21 | 2980/2980 |
| cabeza del bucle, 1 sola | 14 | 2980/2980 |
| en el `continue` de `elevation` con llaves, 1 sola | 14 | **2984** |
| **dentro del `if (cut_flags == 0) {}`** (`mid7`) | **6** | 2980/2980 |
| … el mismo sitio con 8 | 24 | 2980/2980 |
| 7 × `asm("" : "+r"(closest_avoidable))` en el rabo | 140 | **2988** |

Las **6** filas del mejor caso son un **swap f16/f17** de los dos literales del
`bClamp(elevation, -5.0f, 5.0f)`. Y ese swap es **el mismo mecanismo**: pseudos
268 (−5,0f) y 271 (5,0f), `n_refs = 5`, o sea `pri = int(100000/live)`, con

```
live(271) = 1030 + 2N        live(268) = 1034 + 2N
```

donde `N` son los insns añadidos al bucle. **Las constantes suman SIEMPRE el
doble que las CC** (comprobado en seis variantes: base 1034/1030, tail1
1036/1032, mid7 y tail7 1048/1044, va1 1032/1028).

- Para que gane 271 (lo que quiere el objetivo) hace falta
  `live(271) ∈ [1038,1041] ∪ [1049,1052]`, o sea **N = 4, 5, 10 u 11**.
- Para que gane 901 hace falta `live(901) ∈ {482,483,490,491,499,500}`, o sea
  **N = −10, −9, −2, −1, +7 u +8**.

**Los dos conjuntos no se cortan salvo en N = −1.** Es decir: **el «+7» es un
callejón sin salida por construcción**, y quitar la barrera arregla **los dos**
empates a la vez (live 482/491/493 y 1028/1032). No hay que volver a probar
padding en esta función.

### 1.3 El −1: no es el asignador, es la programación

Sin la barrera son 46 filas y el `fmuls` de `extra_width` sale **20 filas antes**
de su sitio. **Ningún pin lo recupera** (todo 2980/2980):

| pin sobre la base SIN barrera | filas |
|---|---:|
| (base sin barrera) | 46 |
| `offset_change` → fr0 / fr10 / fr13 / fr12 | 52 / 53 / 56 / 57 |
| `avoidable_delta_offset` → fr10 / fr12 / fr13 | 48 / 48 / 48 |
| `cut_to_position.x` → fr13 / fr12 | 48 / 57 |

Y las formas de fuente tampoco:

| ensayo (todo 2980/2980 salvo donde se dice) | filas |
|---|---:|
| `extra_width` detrás de `close_factor` / `avoidable_offset` / `right_projection` / `left_projection`, sin barrera | 58 las cuatro |
| `extra_width` delante de `cut_to_position.x` | **66 CON y SIN barrera** |
| `extra_width` entre `cut_x` y `cut_z` | **55 CON y SIN barrera** |
| `(1.0f - 0.8f) * offset_change` en vez de `offset_change * (1.0f - 0.8f)` | objeto idéntico (4 con barrera, 46 sin) |
| sin el pin `fr11` de `cut_to_position.x` y sin barrera | 56 |
| `cut_to_position.z` antes que `cut_to_position.x`, sin barrera | 56 |
| `asm volatile("fmuls …")` con cuerpo sustituyendo `cut_to_position.z -=`, sin barrera | **170**, y **2976 B** |

Las dos filas «CON y SIN barrera idénticas» son un dato: **fuera de su hueco la
barrera no pinta nada**, sólo ancla cuando está entre `cut_to_position.z` y
`extra_width`.

El `asm volatile` con cuerpo **es** el −1 que hace falta (ancla *y* emite la
instrucción en un solo insn) y efectivamente da el reparto CR exacto; lo que pasa
es que un `asm` volátil es barrera **total** de programación y destroza el resto,
además de perder 4 B por un CSE que deja de hacerse.

**Lo que sigue faltando, y ahora es lo único**: un ancla de **cero insns** para
ese `fmuls`.

---

## 2. `InitAtSegment` (816 B) — 15 → 12 filas quitando andamio

Dos cambios, los dos de coste cero:

1. **Fuera `register const WRoadSegment *segment asm("r25")`.** Era síntoma: el
   DWARF del original da `segment // r25` y **nuestro `local_alloc` ya lo pone
   ahí solo**. Con el pin puesto sobraban tres filas —`mulli r10` por `mulli r8`,
   `lwz r25` por `lwz r10` y `add r25,r25,r10` por `add r25,r10,r8`—, es decir
   el pin forzaba a coalescer la base de `fSegments` en el destino. Pines
   alternativos medidos, todos peores: **r24 29 filas, r26 27, r27 30, r30 42**.
2. **Dentro `SetLaneInd((char)laneInd)`**, en los dos ámbitos. No mueve un byte,
   pero el DWARF del original (`mw_dwarfdump.nothpp:2380796`) lista
   `inline void WRoadNav::SetLaneInd(char ind)` **dos veces**, una por ámbito,
   justo donde nosotros escribíamos los dos campos a mano; y `lmap` sobre el
   original imputa esos dos `stb` a `WRoadNetwork.h:525` y los tres `stfs` de
   `SetLaneOffset` a `h:530`. **El método ya estaba en la cabecera**, sólo había
   que llamarlo.

Las **12 filas** que quedan son un **swap limpio r10↔r11**: el objetivo lleva
`fNodes` en r10 y la dirección del literal 0,0f en r11, y nosotros al revés. En
`REG_ALLOC_ORDER` (rs6000.h:932) r11 va antes que r10, así que gana la cantidad
que `local_alloc` reparte primero — en el objetivo, la del literal.

MEDIDO Y NEGATIVO (todo 816 B salvo donde se dice):

| ensayo | filas | tamaño |
|---|---:|---|
| `asm("" : : "f"(0.0f))` entre `fStartPos` y `fEndPos` | 36 | **832** |
| … delante de `fStartPos` | 15 | **824** |
| … detrás de `fEndPos` | 56 | **824** |
| `fToLaneInd/fLaneInd` delante de `fStartPos` | 28 | **824** |
| `SetLaneOffset(0.0f)` delante de `fStartPos` | 31 | 816 |
| `SetLaneOffset(0.0f)` detrás de `fEndPos` (antes de las lane inds) | 12 | 816 |

**Aviso**: el `asm` de solo entrada **no es gratis** cuando su operando es una
constante que aún no existe — la materializa de verdad y cuesta bytes. La veda
de r47 («`asm("" : : "r"(x))` no emite un byte») vale para valores **ya vivos**.

### 2.1 La variante de 6 filas que NO se aplica

`SetLaneOffset(0.0f)` **entre** `fStartPos` y `fEndPos` da **6 filas y 816/816**:
el swap r10/r11 desaparece entero. Pero entonces los tres `stfs` salen **17 filas
antes** de su sitio. `lmap` sobre el original dice que ahí el `lis` del literal va
temprano (`8030199C`, imputado a `h:530`) y los tres `stfs` van tarde
(`80301A30`, mismo `h:530`) — que es **exactamente** lo que hace la versión
aplicada. O sea que la posición de la sentencia es la buena y lo único malo es el
reparto: 6 filas con el árbol equivocado no valen más que 12 con el bueno
(precedente r37/r46, «el porcentaje sube con el árbol equivocado»).

---

## 3. `GetLoadingPriority` (zTrack, 708 B) — los 16 B no son un hueco

El diagnóstico que pedía el brief, con `regmap … --all`. **MISMO conjunto de
locales y MISMO árbol de bloques**: 14 iguales, 0 de más, 0 de menos. De las de
pila sólo cuadran `layer_name` (0x8) y `pos` (0x28):

| local | original | nuestro |
|---|---|---|
| `face` exterior (b0) | r1+0x38 | r1+0x38 |
| `face` interior (b0/b0/b0) | r1+**0x48** | r1+**0x68** |
| `predict_pos` | r1+0xB8 | r1+0xA8 |
| `direction` | r1+0xC0 | r1+0xB0 |
| `v` | r1+0xC8 | r1+0xB8 |
| `dest` de la inline `bScale` | r1+0xA8 | r1+0x98 |

O sea que **no es un hueco de 16 B**: el original **ahorra 32** solapando los dos
`FloatVector face[4]` (0x38..0x68 contra 0x48..0x78; `FloatVector` mide 0xC,
tres floats sin relleno, confirmado en el DWARF, luego el array mide 0x30) y
**gasta 48 más** entre el final del array interior (0x78) y el `dest` de `bScale`
(0xA8), donde nosotros no dejamos ni un byte. 32 − 48 = los −16 que vemos.

Con asignación secuencial (`expand_decl` → `assign_stack_local`, sin reúso) eso
es imposible salvo que el objeto que el original pone en 0x38 mida 0x10. La
prohibición de rellenarlo con una local inventada sigue en pie (precedente
`ICEMover` / `9de121ee`); la prueba siguiente es reproducir **0x38+0x10** y
**0x48+0x30+0x30** con formas de declaración reales, no con relleno.

Y aparte del marco, las 75 filas de `fndiff` llevan un **swap f30↔f31**: el
objetivo tiene `speed` en f31 y el literal 20,0f en f30, y nosotros al revés
(`adjusted_distance` nos cae además en f31 donde el objetivo usa f0). Las 21
vedas de la r37 sobre pines FP siguen vigentes y no se han repetido.

---

## 4. `SetMemoryPoolSize` (zWorld, 304 B, 2 filas) — la r47 lo diagnosticó mal

La r47 decía «`rank_for_schedule` desempata por prioridad, luego por clase
respecto al último insn programado y por último por `INSN_LUID`». Con la traza
del planificador (`-fsched-verbose-2`) se ve que **ninguna de las tres decide**:

```
t=13  listos: 261 266 268 240  ->  240 (mr r0,r3) en iu2  y  268 (lwz r5) en lsu
t=14  listos: 264 261 266      ->  266 en lsu  y  261 (el lis) en iu2
                                   (y AHI se resuelve la dependencia de 244)
t=15  listos: 270 244 264      ->  264 en iu2  Y  244 (el stw) en lsu
```

Las dos instrucciones que salen en orden distinto (`addi` y `stw`) **se emiten en
el MISMO CICLO**, en unidades distintas: no compiten por una ranura, sólo por el
orden dentro del ciclo. Además:

- El `stw` **no puede subir más**: tiene `REG_DEP_ANTI` contra
  `lwz r5, 0x64(r31)` (alias no resuelto entre el global y `this->MemoryPoolSize`).
- **El orden PREVIO a sched2 ya es el del objetivo.** En el `.greg`:
  `call, mr r0/r3, lwz r5, lwz r4, lis, addi, stw, call`. Es sched2 quien lo
  estropea, no la expansión.
- `lmap` sobre el original confirma que son **dos sentencias** (el `stw` es
  `CarLoader.cpp:789` y los argumentos la `790`), o sea que nuestra fuente tiene
  la forma buena.

MEDIDO Y NEGATIVO r48: `asm("" : "+r"(size))` delante de la llamada (la barrera
de ranura) deja las **2 filas y 304/304 B** — el `asm`, al no ser volátil, se
programa en otro sitio. Las diez vedas de r36e siguen vigentes.

---

## 5. Las otras cuatro — sin ensayos nuevos, con el diagnóstico remedido

- **`RenderFlaresOnCar`** (2.908 B, 18 filas, +4 B): el diagnóstico de r17/r46/r47
  se confirma y no hay evidencia nueva. Sigue siendo *dónde nace el pseudo* del
  `@ha` de `lbl_8040AD04`, no cuántos hay.
- **`UpdateWheelYRenderOffset`** (876 B, 7 filas, −4 B): el `fndiff` de esta ronda
  confirma la r47 fila a fila. En el preencabezado el objetivo pone
  `lis r16, lbl_8040AA84@ha` (el 0,0f) donde nosotros ponemos
  `lis r16, TweakKitWheelOffsetRear@ha` (filas 48/49, con el `lis r15, 0x4330` del
  sesgo cruzado); y en la fila 97 al objetivo le sobra respecto a nosotros un
  `lis r9, TweakKitWheelOffsetRear@ha` **dentro** del bucle, o sea que lo
  rematerializa en el volátil en vez de izarlo. En la 169 el objetivo lee el 0,0f
  de r16 y nosotros de r19. El frente es la **fusión de CSE**, no la presión.
- **`CullParts`** (836 B, 5 filas, −4 B): las cinco filas son las que describe la
  r47, verificadas instrucción a instrucción: `Polarity` en r0 (objetivo) contra
  r11 (nuestro) en las filas 95/97/144, la guarda comparando contra r9 en vez de
  reusar el r11 que ya vale cero (fila 101), y el `mr r9, r0` izado que nos falta
  (fila 102). Las dos decisiones son de CSE (`record_jump_equiv` + `fold_rtx`).
- **`DefragmentPool`** (684 B, 23 filas) y las tres locales «sólo nuestras»: no se
  han tocado, el aviso del brief (retirarlas cuesta 47 filas y +8 B) sigue en pie.

---

## 6. Herramientas nuevas

1. **`scripts/schedtrace.py`** — la traza del planificador **ciclo a ciclo**.
   `rtldump.py` sólo deja pasar banderas `-d*`, y la que hace falta es
   `-fsched-verbose-N` (haifa-sched.c). Con ella el volcado trae, por bloque:
   `Ready list (t = 15): 270 244 264` y `--> scheduling insn <<<264>>> on unit iu2`.
   Eso separa las tres causas que siempre se confunden —prioridad, clase respecto
   al último programado, y relleno de ranura de latencia— y aquí desmintió un
   diagnóstico de la ronda anterior.
   **Aviso medido**: con `-fsched-verbose` cc1plus **se cae con ICE** en algunas
   unidades grandes (zWorld en una plantilla de STL, zWorld2 en `WCollider.cpp`) y
   el volcado se corta antes de tu función. El script lo avisa.
2. **La TU reducida** (`scratchpad/r48_world/wrn_only.cpp`, y `--file` en
   `schedtrace.py` / `rtldump.py`). Se coge el **preámbulo entero** de la
   SourceList (los `#define` y el `asm` de cabecera, que son load-bearing) y se
   deja **un solo `#include`** del `.cpp` que interesa. Medido: reproduce
   `HolePunchAvoidables` **byte a byte** —46 filas / 2980 B / 97,25638 %, igual
   que el build entero de zWorld2— y compila en **11 s** contra los 40 de la
   unidad. Con eso una tanda de 20 variantes cabe en cinco minutos, y además
   esquiva el ICE del volcado verboso. Los bancos de la ronda
   (`scratchpad/r48_world/q.py`, `q2.py`, `s1.py`…`s8.py`) están construidos así.

---

## 7. Propuestas fuera de territorio

1. **La TU reducida es aplicable a todo el árbol.** Cualquier SourceList que
   incluya varios `.cpp` puede tener su banco rápido con el preámbulo + un
   `#include`. Hay que comprobar la fidelidad UNA vez (comparar la función contra
   el build completo) y a partir de ahí el ciclo de medida baja de 40 s a ~11 s.
   El único requisito es copiar el preámbulo tal cual: los `#define` de
   `zWorld2.cpp` (`BWARE_NO_INLINE_GLOBAL_NEW`, `ATTRIB_NO_INLINE_CLASSKEY`) y su
   `asm` de `.rodata` cambian el código generado.

2. **`-fsched-verbose` para el catálogo.** Las palancas de planificación del brief
   (los dos grupos de peso de `sched1`, la barrera de ranura, la barrera
   selectiva) se pueden **verificar** ahora en vez de inferirse. Y sale una
   corrección: `INSN_REG_WEIGHT` está bajo `if (!reload_completed)` en
   `haifa-sched.c`, o sea que **sólo actúa en sched1** — coincide con lo que dice
   el brief, pero además el modelo de rs6000 es **superescalar** (unidades `lsu`,
   `iu`, `fpu`, `iu2`, `iu2`), así que **dos insns pueden salir en el mismo
   ciclo** y entonces el orden no lo decide `rank_for_schedule` sino el reparto de
   unidades. Antes de escribir «es un desempate por LUID» conviene mirar la traza.

3. **El empate de `allocno_compare` es una familia, no un caso.** En esta función
   coexisten **dos** empates de truncamiento (`int(30000/live)` con n_refs=3 y
   `int(100000/live)` con n_refs=5) y las dos poblaciones **suman distinto** ante
   el mismo cambio (las constantes de bloque, x2; los CC izados, x1). Cuando una
   ronda vaya a «meter N insns de cero bytes» conviene **contar todos los empates
   del `.greg`**, no sólo el que sale en el diff: aquí eso habría ahorrado la
   tanda entera del «+7».

4. **`n_refs` en `allocno_compare` es el campo «used N times» del `.lreg`, no
   `used + set`.** Validado seis veces en esta ronda (las tres CC con n_refs=3 y
   las dos constantes con n_refs=5); con `used + set` la fórmula predice el
   reparto **al revés** en el caso de las constantes.

---

## 8. Artefactos

`scratchpad/r48_world/`:

- `base_fncmp_*.txt` / `final_fncmp_*.txt` (el antes/después de las tres unidades).
- `hpa_nobar.txt`, `hpa_mid7.txt`, `ias_base.txt`, `ias_sinpin.txt`, `ias_slo.txt`,
  `glp_base.txt`, `d_SetMemoryPoolSize*.txt`, `d_CullParts*.txt`,
  `d_UpdateWheelYRenderOffset*.txt` (los `fndiff` citados).
- `rtl/hpa_{base,pre1,top1,tail1,tail7,mid7,elev1,va1}.{greg,lreg}` — de donde
  salen todas las tablas de `live_length`.
- `rtl/smps.{greg,sched2,sched2v}` — la traza del planificador de
  `SetMemoryPoolSize`.
- `wrn_only.cpp` — la TU reducida (y `wrn_only.o`).
- `m.py`, `t.py`, `tv.py`, `tc.py`, `cc.py`, `cf.py`, `cv.py`, `rv.py`, `q.py`,
  `q2.py`, `s1.py`…`s8.py` — los bancos de variantes de cada eje.
- `WRoadNetwork.cpp.HEAD`, `CarRender.cpp.HEAD`, `CarLoader.cpp.HEAD`,
  `TrackStreamer.cpp.HEAD` — las copias de partida.

En `scripts/`: **`schedtrace.py`**.
