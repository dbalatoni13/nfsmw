# r76 — Despineo del lote MUNDO (zWorld / zWorld2 / zTrack)

Fecha 2026-09-16, rama `agent/vedas-oraculo-r2`. Encargo: quitar **todos** los
pines `register T x asm("rN")` y barreras `asm("" : …)` de seis ficheros de
`Src/World`, prefiriendo **código real a un 100 % falso**.

Método del piloto `MemoryPoolManager::NewBlockAux`: oráculo DWARF primero
(`scripts/dwarf1.py fn <función>`), forma que dice el DWARF después, barrido de
formas plausibles al final, y **nunca** reintroducir pin, barrera, atributo de
colocación, `#line` ni función muerta.

Nada de `configure.py`, `config/`, `keep.lst`, `build.ninja` ni cabeceras
compartidas se ha tocado. No se ha ejecutado `ninja` ni `configure.py`, y no se
ha commiteado.

---

## 0. Resultado en una tabla

**8 pines y 2 barreras fuera. Cero andamio vivo en los cinco ficheros**
(comprobado con `scripts/censoasm.py --clase PIN|BARRERA`, que quita los
comentarios antes de contar).

| función | fichero | unidad | B | andamio retirado | antes | **después** | filas | grupo |
|---|---|---|---:|---|---:|---:|---:|:--:|
| `TrackStreamer::HandleLoading` | `TrackStreamer.cpp` | zTrack | 592 | 2 pines r30, r28 | 100 % | **100 %** | **0** | B |
| `CarRenderInfo::CreateCarLightFlares` | `CarRender.cpp` | zWorld | 748 | 3 pines r11, r0, r19 | 100 % *falso* | 99,73262 % | 8 | **A** |
| `TrackStreamer::GetPredictedZone` | `TrackStreamer.cpp` | zTrack | 660 | 1 pin r28 | 100 % | 99,69697 % | 10 | B |
| `CarRenderInfo::UpdateLightStateTextures` | `CarRender.cpp` | zWorld | 1068 | 1 barrera | 100 % | 98,62921 % | 15 | B |
| `RegionQuery::CalculateRegionInfo` | `WeatherMan.cpp` | zTrack | 1208 | 2 pines fr11, fr0 + 1 barrera | 100 % *falso* | 98,54636 % | 35 | **A** |
| **total** | | | **4.276** | **8 pines + 2 barreras** | | | **68** | |

**Lo que cuesta, dicho sin adornos** (`matched_code` es todo-o-nada): de los
4.276 B que ayer se contabilizaban como terminados, **592 B siguen al 100 % con
código real** y **3.684 B dejan de contar**. A cambio, cinco funciones dicen la
verdad sobre su forma y tres ficheros dejan de necesitar GCC.

De las 1.069 instrucciones de las cinco funciones, **68 difieren** (6,4 %), y
**todas menos una son reparto de registros**: ni una sola instrucción de más ni
de menos salvo el `slwi` de `UpdateLightStateTextures`.

**201 objetos medidos** en el barrido (7 + 18 + 14 + 10 + 152).

---

## 1. Los dos ficheros que ya estaban limpios

El encargo listaba `CarLoader.cpp` (2 pines) y `WRoadNetwork.cpp` (5). **Los dos
tienen CERO andamio vivo.** Esas cifras vienen de un `grep` de la forma
canónica, que cuenta también los comentarios: `CarLoader.cpp` tiene 12 menciones
de `asm(` y las 10 de esa familia son notas de rondas pasadas; `WRoadNetwork.cpp`
lo despinó la r75. Lo que les queda es de otros frentes:

| fichero | lo que queda | frente |
|---|---|---|
| `CarLoader.cpp` | 2 bloques `asm(".section .rodata")` | DATOS (F3) |
| `WRoadNetwork.cpp` | nada | — |

Es la trampa que ya documenta `r76-censo-pines.md` §1 y conviene repetirla: **el
`grep` de `register … asm(` casi duplica el censo real**.

---

## 2. Función por función

### 2.1 `TrackStreamer::HandleLoading` — 2 pines fuera y SIGUE al 100 % (592 B)

El único caso del lote en el que el andamio sobraba del todo.

**DWARF**: `num_sections_unactivated` r30 y `complete` r29 en el bloque de
fuera; `n` r28 en **su propio bloque** {0x802BBF24..0x802BC010} con `section` r4
anidado dentro — o sea, un `for (int n = …)`. Nuestra fuente tenía `n` como
hermana del contador y un `if (num_sections_unactivated < NumTrackStreamingSections)
do { … } while (…)` cuyo único oficio era darle una referencia de más al contador.

**Los dos cambios que la cierran**, ninguno con asm:

1. **Estructura del DWARF**: `for (int n = 0; n < NumTrackStreamingSections; n++)`.
2. **El `||` partido en dos ramas** (`if … else if …`, con `UnactivateSection` y
   el `++` en las dos). Equivalente, porque `LoadingPhase` no puede valer
   `ALLOCATING_GEOMETRY_SECTIONS` y `ALLOCATING_REGULAR_SECTIONS` a la vez.

**Mecanismo** (leído en el `.greg` en la r67, confirmado aquí por la medida):
con el `||` el contador tiene refs 7, live 120, pri 1166 y pierde contra la
constante `0x51eb851f` que izan `IsTextureSection`/`IsLibrarySection` (refs 7,
live 55, pri 2545), así que `global_alloc` le da r30 **a la constante**. Con las
dos ramas el `++` aparece **dos veces dentro del bucle**, el contador sube de
referencias y se lleva r30 como el objetivo. Los pines hacían lo mismo por la
puerta de atrás, metiendo r30/r28 en `regs_ever_live`.

El `TODO` que había en esa línea decía que las dos ramas «messes with the
instructions»: lo hacían **con los pines puestos**.

**Otras 8 formas medidas**: `||` con los lados cambiados 18 filas, `while` 12,
`!=` en la cola 17, límite en una local 24, bucle descendente 11, puntero
corriente 12, `= n + 1` 12, `complete` declarada junto al contador (no compila).

### 2.2 `CarRenderInfo::CreateCarLightFlares` — 100 % falso → 99,73 % (748 B, 8 filas)

**DWARF**: el bloque declara `model` (`eModel*`, **localización vacía:
optimizado fuera**) y `position_marker` r29. `slot_base` y `raw_model` **no
existen**. Tres pines para dos locales inventadas y una que el original ni
siquiera mantiene viva en un registro; con ellos venía además la aritmética de
punteros a pelo (`this + slotIndex*0x5F0 + … + 0xb78`, `& ~3u`) que sólo existía
para sostenerlos.

**Forma aplicada**: `eModel *model = this->mCarPartModels[slotIndex][0][this->mMinLodLevel].GetModel();`

**Lo que queda**: 8 filas, **una rotación de tres** r19/r20/r21 entre `model` y
las dos constantes del `switch` que GCC iza (`0x7adf7ef8`, `0xa2a2fc7c`). Cero
instrucciones de más o de menos. En el `.greg`: `model` refs 5, live 138, pri
724; `0xa2a2fc7c` pri 722; `0x7adf7ef8` pri 718 — con live ≥ 140 (pri 714)
saldría el orden del objetivo.

**7 formas medidas**, ninguna baja de 8: marcador declarado antes, chequeo antes
del marcador, `!model`, marcador sin inicializador, `continue` en una línea, y
el modelo en dos pasos con un `CarPartModel*` intermedio (15 filas).

> **Aviso, y no es de esta ronda**: hay una forma que cierra al 100 % pero
> pertenece al **frente de DATOS**: dar un `case` por cadena a `type_name` (que
> el DWARF describe, sin localización) y borrar el bloque `.asciz` de debajo de
> la función. Mueve `.rodata` +24 B porque GCC alinea cada cadena a 4 y el `asm`
> las pega con `.asciz`. Queda anotada en el código, sin aplicar.

### 2.3 `TrackStreamer::GetPredictedZone` — 100 % → 99,70 % (660 B, 10 filas)

**DWARF**: `predict_position_used` **existe y está en r28** (grupo B). El pin no
mentía sobre la fuente: mentía sobre el reparto. El orden de declaración ya era
el del DWARF.

**Lo que queda**: 10 filas y son **un solo intercambio r27↔r28** entre
`predict_zone_number` y `predict_position_used`. En el `.greg`: zone_number refs
11 live 292 pri 1130; position_used refs 10 live 268 pri 1119 — pierde por 11
puntos.

**14 formas medidas**, ninguna baja de 10. **Lo interesante es el negativo**:
las tres formas que **le suben referencias** a la bandera (guarda en el `for` de
barreras, guarda en el `if` interno, asignación condicional del zone_number) la
**empeoran** — 23, 26 y 21 filas. La referencia de más dentro del bucle vale más
instrucciones que el registro que gana. Esto acota la receta «súbele el `n_refs`
con una forma de fuente»: **sólo sirve si la referencia cae fuera del bucle**.

Además: `break` al invalidar 14 filas; `&&` de los dos `if` exteriores, los dos
`if` planos, `while` en vez de `for` y el return negado, las cuatro 10 filas; y
en la cabecera, `short`/`unsigned` para zone_number, `predict_position` delante e
inicialización separada, las cuatro 10 filas.

### 2.4 `CarRenderInfo::UpdateLightStateTextures` — 100 % → 98,63 % (1064/1068 B, 15 filas)

**DWARF**: el bloque declara `left_light_state` r8, `right_light_state`
(optimizada fuera), `left_light_state_hash` r7, `right_light_state_hash` r11,
**`left_light_glass_state_hash` r8**, `right_light_glass_state_hash` r6 y
`new_headlight_hash` r9.

**Faltaba una local**: el hash del cristal izquierdo se reasignaba encima de
`left_light_state` para forzarle el r8. Ahora es una local de verdad, como en el
original, y el asignador reutiliza r8 por su cuenta.

**Lo que queda es UNA instrucción** (las otras 14 filas son el renombramiento que
arrastra): el objetivo calcula el índice izquierdo en tiempo de ejecución
(`li r8,1` + `slwi r8,r8,2`) y el derecho como literal (`li r0,4`); nosotros
plegamos los dos, porque `lights_always_on` es constante.

**Lo que dice el mapa de líneas** (`symbols/debug_lines.txt`, rango
0x802CAA44..0x802CAC4C): ese `li r8,1` lleva **dos** números de línea, 3448 y
3456 — las dos asignaciones de estado fundidas en una instrucción — y el `slwi`
es ya la 3487 (el primer hash); 3488/3489/3490 son los otros tres. Para que el
`slwi` sobreviva a `combine`, el `li 1` y el desplazamiento tienen que estar en
**bloques básicos distintos**: el original tiene ahí algo que no reconstruimos,
misma familia que la sentencia perdida de `JoyE.cpp:307` en la r75.

**18 formas medidas**, ninguna baja de 15: declaraciones juntas, derecha antes que
izquierda, los dos glass antes que los dos hash (27), lado por lado (35),
asignación booleana directa, ternario, inicializados a 1 con la condición negada,
`if/else` con las dos ramas, estados `unsigned`, estados `bool`, un solo `if`
para los dos lados (43), punteros a las dos tablas (27), el índice izquierdo por
una variable intermedia, `++` en vez de `= 1`.

### 2.5 `RegionQuery::CalculateRegionInfo` — 100 % falso → 98,55 % (1208 B, 35 filas)

**DWARF** (71 locales): el bloque `FogControlOverRide` declara **exactamente
dos** locales, `fog_colour` r11 y `retcol` r11 — las dos las teníamos.
`fogStart` y `fogFalloffY` **no existen**, y `oldDistFogStart` es un **estático**
(`=0x80437108`), no una local. El original asigna los globales a pelo y compara
contra el valor que le reenvía CSE (fila 55: `fcmpu f0,f11`, no un `lfs` de
`DistFogStart`).

**152 objetos medidos**, ninguno baja de 35 filas por la vía plausible:

- **La rejilla completa de órdenes de sentencia**: las 120 permutaciones de las
  cinco asignaciones delante del color y las 24 con `DistFogPower` detrás de
  `retcol`. Mínimo **31 filas** (`pw-fy-start-fo-fx`, 98,45033 %) — menos filas
  pero **menos porcentaje**, y con un orden que nadie escribe. Se queda el orden
  natural (35 filas, 98,54636 %). Dato útil: el orden del **bloque hermano de
  abajo** (`start-pw-fo-fx-fy`, el que usa el `else` de la misma función) da
  **41 filas**, así que tampoco era ése.
- **Palanca nueva y REFUTADA**: subir a la cabecera las locales de ámbito de
  FUNCIÓN que el DWARF permite subir sin romper su orden (`smallest` f8,
  `region_list` r7, `totaleffex` f6; las 8 combinaciones). La idea era ocupar un
  FPR temprano sin pin, que es justo lo que la r65 identificó como necesario.
  **Todas peores**, de 81 a 114 filas.

**Lo que queda**: 35 filas de reparto puro, 1208/1208 B, sin una instrucción de
más ni de menos — GPR r8-r11/r0 rotados en el bloque del color y los pares
f10/f11 y f12/f13 cambiados en los cinco `stfs`.

---

## 3. Portabilidad (`tools/scratch/x360_sonda/sonda.py`)

| fichero | clase | primer bloqueo que queda | ¿es andamio? |
|---|---|---|---|
| `CarRender.cpp` | PLATAFORMA | `include 'dolphin/mtx.h'` (vía `CameraMover.hpp:22`) | no |
| `WeatherMan.cpp` | PLATAFORMA | `include 'dolphin/mtx.h'` (vía `CameraMover.hpp:22`) | no |
| `TrackStreamer.cpp` | GCCASM | **ALIAS** `asm("ClearTable__9bBitTable")` (línea 31) | no — frente F2 |
| `CarLoader.cpp` | GCCASM | **DATOS** `asm(".section .rodata")` (línea 307) | no — frente F3 |
| `WRoadNetwork.cpp` | OTRO | `C4716 WillCrossBarrier must return a value` | no — es un `// STRIPPED` |

**En ninguno de los cinco el bloqueo es ya un pin o una barrera.**

### Corrección al censo de la r76

`r76-censo-pines.md` §3 pone `Indep/Src/World/TrackStreamer.cpp` entre los
**«30 GCCASM puros»**, con la promesa de que «quitarlo = fichero compilable en
los tres compiladores». **Medido: es falso.** Quitados los 3 pines, el fichero
sigue siendo GCCASM porque le quedan un ALIAS (línea 31) y un bloque de DATOS
(línea 78). La sonda clasifica por el **primer** error, y el primero era el pin;
detrás había dos más. Conviene re-sondar los otros 29 antes de prometer lo mismo.

El `C4716` de `WRoadNetwork.cpp` tampoco es un defecto a arreglar: es el patrón
`// STRIPPED` del proyecto (cuerpo vacío para una función cuyo código no está en
el original). GCC lo acepta, MSVC no. Es sistémico, no de este fichero.

---

## 4. Lo que se aprende, para la próxima tanda

1. **Partir un `||` en dos ramas es una palanca de reparto de pleno derecho.**
   Duplica las referencias de lo que se toca dentro del bucle sin añadir una sola
   instrucción cuando las dos condiciones son excluyentes. Cerró `HandleLoading`
   al 100 % sin asm. Hay `||` con ramas excluyentes por todo el árbol.
2. **Subirle `n_refs` a una variable sólo vale si la referencia cae FUERA del
   bucle.** Las tres formas que se la metieron dentro en `GetPredictedZone`
   empeoraron entre 11 y 16 filas. Es el reverso exacto del punto 1: allí la
   referencia de más *sustituía* a una que ya estaba.
3. **La rejilla de permutaciones de sentencias es floja y engaña.** 144 objetos
   en `CalculateRegionInfo` para pasar de 35 a 31 filas **bajando** el
   porcentaje, con un orden implausible. Comparar siempre contra el orden del
   bloque hermano antes de creerse una permutación ganadora.
4. **`build_direct.py` dice «1 ok, 1 fallidas» y eso contiene la cadena `ok`.**
   Un barrido que buscaba `'ok' in salida` midió diez variantes del **mismo .o
   rancio** y las diez dieron 100 %. El harness (`scratchpad/r76_world/barrido.py`)
   ahora exige `0 fallidas` **y** avisa cuando dos variantes dan el mismo sha1.
5. **Un cambio de comentarios mueve el sha1 del `.o` aunque no haya `BNEW`**,
   porque el `.o` lleva `.debug` (1,6 MB) y `.line` (98 kB). Control hecho: una
   línea de comentario de prueba en `WeatherMan.cpp` cambió el sha1 del fichero
   (7118…→7360…) y dejó **idéntico** el digest de todas las secciones ALLOC
   (`2697e2b4926837df`). Para juzgar un comentario, el digest ALLOC, no el sha1.

---

## 5. Comprobaciones de cierre

- `censoasm.py --clase PIN` y `--clase BARRERA`: **cero** apariciones en los
  cinco ficheros.
- `build_direct.py zWorld zWorld2 zTrack`: **3 ok, 0 fallidas**.
- Finales de línea intactos: `CarRender.cpp`, `CarLoader.cpp` y
  `TrackStreamer.cpp` CRLF; `WeatherMan.cpp` y `WRoadNetwork.cpp` LF.
- Estado de las tres unidades tras el despineo (ponderado por bytes, con
  `function_reloc_diffs=none` y `ppc.calculatePoolRelocations=false`):
  zWorld 98,07615 %, zWorld2 98,30945 %, zTrack 98,69739 %. Los símbolos que
  quedan por debajo del 100 % en zWorld2 (`HolePunchAvoidables`, `InitAtSegment`,
  `CheckCollideSRB`, `CookieTrailCurvature`) son los de la r75 y no se han
  tocado; `GetLoadingPriority` (zTrack) sigue marcada `UNSOLVED because of debug
  stuff`; `[.rodata-0]`, `[.data-0]`, `CarSlotIDNames`, `VisibleGroupInfoTable` y
  `lbl_8040DFE8` son del frente de DATOS.
- Herramientas del barrido, sin commitear, en `scratchpad/r76_world/`:
  `barrido.py`, `medir.py`, `ed.py`, `secs.py`, `syms.py`, los ficheros de
  variantes `v_*.py` y los logs `barrido_*.txt` / `cri_out*.txt`.
