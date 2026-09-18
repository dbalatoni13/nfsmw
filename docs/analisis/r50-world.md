# r50 — zWorld / zWorld2 / zTrack

Territorio: 10.112 B en 8 funciones. **`CullParts` CIERRA al 100 % (836/836):
+836 B de `matched_code`.** Venía del 99,42584 %, de cinco rondas atascada y de
un par de docenas de vedas. Eran **cuatro** cosas a la vez, y ninguna de ellas
era la que la r47/r48/r49 estaban buscando.

`zWorld` pasa de **5 funciones / 5.608 B** a **4 funciones / 4.772 B**.
`zWorld2` y `zTrack` sin cambios.

Lo importante, en una línea cada uno:

- **`CullParts` = 100 %.** (1) El test post-bucle compara contra **memoria**, y lo
  demuestra el `mr r9,r0` que faltaba: es la **copia del representante de
  gcse-PRE**. (2) `debug_print` **no puede tocarse dentro de su bucle** o pierde
  el registro compartido con ese representante. (3) El `li r9,0` que sobraba lo
  decide **`make_regs_eqv` (cse.c:1005) en cse2, no en cse1**, y la regla es
  «gana el que tiene el ÚLTIMO USO más tarde». (4) **Fuera el clobber de r11**:
  era andamio del árbol viejo, y con el árbol bueno costaba las últimas 4 filas.
- **Un `asm` load-bearing puede dejar de serlo cuando cambia el árbol.** El
  clobber de r11 estaba medido tres rondas seguidas como imprescindible (5 → 9
  filas) y con el árbol correcto es **al revés** (4 → 0). Hay que remedirlos.
- **`-fmove-all-movables` queda VERIFICADA** para `CarRender.cpp` con el control
  que exige el brief: quitarla empeora **9 funciones y mejora 0**.
- `UpdateWheelYRenderOffset`, `SetMemoryPoolSize`, `InitAtSegment`,
  `DefragmentPool`, `RenderFlaresOnCar`, `GetLoadingPriority` y
  `HolePunchAvoidables`: **sin cambios**, con lo medido abajo.

## Verificación

`fncmp` antes / después de las TRES unidades
(`scratchpad/r50_world/base_fncmp_*.txt` y `final_fncmp_*.txt`):

| unidad | antes | después |
|---|---|---|
| zWorld | 5 funciones / **5.608 B** | 4 funciones / **4.772 B** |
| zWorld2 | 2 funciones / 3.796 B | **`diff` vacío** |
| zTrack | 1 función / 708 B | **`diff` vacío** |

La única línea que cambia en todo el territorio es la de `CullParts`, y
desaparece. **Ninguna función empeora.**

`fndiff Speed/Indep/SourceLists/zWorld CullParts…`:
`target=100.0% ours=100.0% size=836/836`, **cero filas**.
`lcfix.py --check`: «todas las entradas @lc estan al dia».
`audit.py` en las tres: **0 FALLA**.
`regmap zWorld "CarPartCuller::CullParts"`: 20 iguales, 0 con registro distinto,
0 de ámbito equivocado, 0 solo del original, 0 solo nuestras.
`git status`: sólo `src/Speed/Indep/Src/World/CarRender.cpp`.

---

## 1. `CullParts` (zWorld, 836 B) — cerrada, y las cuatro piezas

### 1.1 El árbol

```c
int debug_print = 0;                                   // línea 634 del original
if (plane_info->Polarity == CULLING_POLARITY_ANY_VISIBLE) {
    int n = 0;
    for (; n < plane_info->NumPlanes; n++) {
        ...
        float distance = bDot(&v, &normal) - plane_info->NormalDistance[n];
        if (distance < 0.0f) break;                    // <- sin `debug_print` aquí
    }
    if (n == plane_info->NumPlanes) visible = false;   // <- contra MEMORIA
} else if (plane_info->Polarity == CULLING_POLARITY_ALL_MUST_BE_VISIBLE) {
    for (int n = 0; n < plane_info->NumPlanes; n++) {
        debug_print = plane_info->NumPlanes;           // <- la mención tardía
        ...
    }
}
```

…y **sin** el `asm("" : : : "r11")` que había delante.

### 1.2 El `mr r9,r0` es la copia del representante de gcse-PRE

El objetivo emite, en el preencabezado del bucle:

```
 99 lwz  r0, 0xc(r30)   ; NumPlanes
100 li   r31, 0x0       ; n = 0
101 cmpw r9, r0         ; la guarda
102 mr   r9, r0         ; <- el insn que faltaba desde la r47
103 bge
```

`bge` salta al `cmpw r31, r9` de después del bucle. Por ese camino el segundo
operando vale **NumPlanes**, no 0 — o sea que **no puede ser `n == debug_print`**,
porque ahí `debug_print` vale 0. El `mr` es la **copia del representante** que
inserta `gcse`-PRE cuando la expresión `plane_info->NumPlanes` se calcula dentro
del bucle (la cota, `lwz r9, 0xc(r30)`) y se vuelve a leer detrás: PRE le da un
pseudo representante y en el borde que no lo calcula mete la copia.

Prueba directa: cambiando **sólo** `if (n == debug_print)` por
`if (n == plane_info->NumPlanes)`, las filas 101 y 102 salen exactas a la primera
(5 → 4 filas).

### 1.3 El `li r9,0` lo decide `make_regs_eqv`, y en CSE2

El `li r9,0` es `debug_print = 0`, un **almacenamiento muerto**: su único lector
es la sustitución que hace CSE en la guarda `n < NumPlanes` (los dos valen 0).
En los volcados:

- **cse1 YA hace lo correcto.** `cr_only_cpp.i.cse`, insn 953:
  `(set (reg:CC 303) (compare:CC (reg/v:SI 178) (reg:SI 302)))`, con **reg 178 =
  `debug_print`** — exactamente el objetivo.
- **cse2 (`-frerun-cse-after-loop`) lo estropea**: la misma insn 953 pasa a
  `(compare:CC (reg:SI 180) (reg:SI 302))`, con **reg 180 = `Polarity`**.

El mecanismo es `make_regs_eqv` (cse.c:1005). Cuando `record_jump_cond` mete el
registro de `Polarity` en la clase de equivalencia del 0, ese nuevo miembro pasa
a ser **el primero de la cadena** —y `canon_reg` sustituye siempre por el
primero— si y sólo si:

```c
(uid_cuid[REGNO_LAST_UID (new)] > cse_basic_block_end
 || uid_cuid[REGNO_FIRST_UID (new)] < cse_basic_block_start)
&& (uid_cuid[REGNO_LAST_UID (new)] > uid_cuid[REGNO_LAST_UID (firstr)])
```

La segunda condición es la palanca: **gana el que tenga el ÚLTIMO USO más
tarde**. `Polarity` se relee en el `else if`, que va **después** del
`if (n == …)`, así que gana. Con una mención de `debug_print` por detrás de ese
`else if`, deja de ganar y vuelve a mandar `debug_print`.

Dos detalles de calendario que hacen que la mención cueste **cero bytes**:

- `reg_scan` corre **justo antes de cse2** (toplev.c:4242), así que la mención
  tiene que existir en ese momento;
- `flow` —que borra el almacenamiento muerto— va **después de cse2**, así que la
  mención desaparece antes del asignador y no emite un byte. (Confirmado: el
  bucle 2 sale byte a byte.)

### 1.4 `debug_print` no puede tocarse dentro de su propio bucle

Si `debug_print = plane_info->NumPlanes;` se queda en el bucle 1, su rango de
vida **cruza el bucle**, deja de poder compartir r9 con el representante de PRE,
y sale un `mr` de más **dentro** del bucle: **840 B y 18 filas**. Con la mención
sólo en el bucle 2 el rango es `[634 .. la guarda]`, comparte r9, y salen
836/836. Esto explica por qué la r49 midió «4 filas» con el test contra memoria y
«8 filas / 828 B» con la asignación delante del bucle: le faltaba esta pieza.

### 1.5 El clobber de r11: andamio que había que retirar

Con el árbol correcto quedaban 4 filas —un swap r16↔r17 en el preencabezado del
bucle EXTERIOR entre `(high TheGameFlowManager)` y el `0x4330` de la conversión
de `stang`—. Estaba cuantificado como un empate de `allocno_compare`
(`scratchpad/r50_world/rtl/cull_alloc.txt`):

| pseudo | qué es | n_refs | live | pri |
|---|---|---:|---:|---:|
| 147 | `culldiv@ha` | 3 | 328 | **91** |
| 124 | `(high TheGameFlowManager)` | 3 | 332 | **90** |
| 141 | el `0x4330` de la conversión | 3 | 330 | **90** |
| 114 | `CarPartCullingPlaneInfoTable@l` | 3 | 338 | **88** |

124 y 141 empatan en 90 y `allocno_compare` (global.c) desempata **por número de
allocno** (`v1 - v2`), así que ganaba 124. Ninguna forma de fuente lo movía (ver
la tabla de negativos). **Lo movió quitar el clobber de r11**: la cantidad
fantasma que ese `asm` fabricaba desplazaba el reparto entero.

La lección, que vale para todo el árbol: **el clobber estaba medido como
load-bearing tres rondas seguidas** (r36d lo puso, r49 remidió 5 → 9 filas al
quitarlo) y con el árbol corregido es **al revés**: 4 → **0**. Un `asm` de
andamio se mide contra el árbol que hay, y hay que volver a medirlo cada vez que
el árbol cambia.

### 1.6 MEDIDO Y NEGATIVO r50 sobre `CullParts`

Sobre el árbol bueno **con** el clobber todavía puesto (todo 836/836 y 4 filas
salvo donde se dice):

| ensayo | filas |
|---|---:|
| `modifier = stang / culldiv` (conversión implícita) | 6, **840 B** |
| conversión en una local (`float fstang = …`) | 4 |
| `float cd = culldiv;` en la rama / delante del `if` | 6 / 9 |
| declarar `modifier` antes que `omodifier` | 4 |
| `static_cast<float>(static_cast<unsigned int>(stang))` | 4 |
| `IsGameFlowInFrontEnd() == false` | 4 |
| invertir el test de `type` | 4 |
| `modifier = 0.0f` muerto (tres posiciones) | 4 |
| `omodifier` repetido en la ventana | 4 |
| **movable de cero bytes** `asm("" : "+r"(unModcamera_eyeP))` × 1 / 2 / 3 | 18 / 35 / 35 |
| … × 1 / 2 delante del ternario de `camera_eye` | 19 / 36 |
| pin `register int debug_print asm("r9"/"r11"/"r10"/"r8")` | 4 y **832 B** los cuatro |

Sobre el árbol viejo, camino a la solución (todo 832 B salvo donde se dice):

| ensayo | filas |
|---|---:|
| base r49 | 5 |
| test contra memoria, sin más | 4 |
| … + declarar `debug_print` fuera del `if (NumPlanes>0)` / `int n = debug_print` | 4 / 4 |
| árbol viejo + mención tardía detrás del `else if` (`asm("" : "+r")`) | 2 |
| árbol viejo + mención tardía arriba del bucle 2 | **1** (falta el `mr`) |
| árbol viejo + `debug_print = 0` muerto detrás de la cadena | 2 |
| árbol viejo + la asignación delante del bucle 1 / en su propio test | 8, **828 B** |
| árbol viejo + la asignación arriba del cuerpo del bucle 1 | 67, 836 B |
| test contra memoria + mención tardía **con** la asignación del bucle 1 | 18-25, **840 B** |
| mención tardía dentro del `if (distance >= 0)` del bucle 2 | 5 (no llega a `reg_scan`) |
| mención tardía en posición simétrica del bucle 2 (tras `distance`) | 20 |
| mención tardía al final del cuerpo del bucle 2 | 4 |

---

## 2. `-fmove-all-movables`: verificada, no supuesta

La r49 dejó `UpdateWheelYRenderOffset` colgando de `combine_movables` y la
sospecha natural era la bandera. Medida sobre `CarRender.cpp` entero con
`scratchpad/r50_world/flagtest.py`, que exige el **control** del brief (el sha1
del objeto tiene que cambiar, y cambia: `89ccb430c7` → `42e09fcd3d`):

| función | con la bandera | sin ella |
|---|---:|---:|
| `Render__13CarRenderInfo…` | **100,0000** | 93,4290 |
| `UpdateCarParts` | **100,0000** | 89,3664 |
| `DrawAmbientShadow` | **100,0000** | 97,8461 |
| `InitEmitterPositions` | **100,0000** | 94,4586 |
| `CreateCarLightFlares` | **100,0000** | 81,8503 |
| `RenderFrontEndCars` | **100,0000** | 95,5294 |
| `CullParts` | **99,9043** | 96,0287 |
| `RenderFlaresOnCar` | **98,6176** | 90,0701 |
| `UpdateWheelYRenderOffset` | **99,3836** | 88,1050 |

**9 empeoran, 0 mejoran.** La bandera es correcta y ese frente no se abre por
ahí.

---

## 3. Las otras siete, sin cambios

- **`UpdateWheelYRenderOffset`** (876 B, 7 filas, −4 B). Vedas de r36e/r47/r49
  remedidas, todas en pie, y tres ensayos nuevos (todo 872 B): el `else if` de
  `TweakKitWheelOffsetRear` escrito como `else { if … }` → 7 filas;
  `desired_width > 0.0f && model_width > 0.0f` → 9;
  `desired_radius > 0.0f && model_radius > 0.0f` → 9; el ternario escrito
  `0.0f >= …` → 7 (objeto idéntico). El diagnóstico de r49 se confirma: el
  objetivo tiene **un `lis TweakKitWheelOffsetRear@ha` DENTRO del bucle** (fila
  97) y en el preencabezado un **segundo `(high 0,0f)`** donde nosotros izamos la
  dirección de `TweakKitWheelOffsetRear`.
  **Pendiente para la r51**: es exactamente el mismo perfil que tenía
  `CullParts` —un `asm` de andamio (aquí ninguno) y un árbol que no se había
  cuestionado— y ahora hay una herramienta nueva para atacarlo: mirar `.cse` y
  `.cse2` por separado.
- **`InitAtSegment`** (816 B, 12 filas). Cuatro formas nuevas, todas 816/816:
  intercambiar `SetLaneInd`/`SetLaneOffset` 12 filas; `!fNodeInd` en vez de
  `fNodeInd == 0` 12; intercambiar `fStartPos`/`fEndPos` 21; sacar los dos
  `GetNode` a locales 74 filas y **812 B**; `SetLaneOffset(0.0f)` delante de
  `fStartPos` 31. Dato nuevo del `.lreg`: entre la definición del
  `(high $LC472)` (insn 414) y su uso (insn 417) sólo hay **dos** insns en el
  código ya planificado, pero `live` vale 8 porque lo fija el `life_analysis`
  **anterior a sched1**. Los dos umbrales de la r49 (`live(212) ≤ 6` o
  `live(176) ≥ 41`) siguen siendo la única salida.
- **`SetMemoryPoolSize`** (304 B, 2 filas). Leído `rank_for_schedule` del fuente,
  la regla de clase de la r49 se confirma con un matiz que abre una rendija: la
  clase es 3 **también** cuando `insn_cost (last_scheduled_insn, link, insn) == 1`,
  o sea que con latencia 1 el `addi` no quedaría en clase 1 y decidiría
  `depend_count`. Con la latencia real de rs6000 no lo es.
- **`DefragmentPool`** (684 B, 23 filas). **Corrección a la r49**: para invertir
  el ciclo A hace falta `live(222) ≤ 47` **o `live(82) ≥ 173`** — la r49 escribió
  «163», que da 270000/163 = 1656 y **no** baja de los 1568 de 222; el primer
  valor que sí lo hace es 173 (1560).
- **`RenderFlaresOnCar`** (2.908 B, 18 filas, +4 B), **`GetLoadingPriority`**
  (708 B, 75 filas) y **`HolePunchAvoidables`** (2.980 B, 4 filas, cerrado por
  construcción): sin ensayos nuevos.

---

## 4. Propuestas fuera de territorio

1. **`make_regs_eqv` merece entrada en el catálogo, y la regla es de una línea:
   cuando dos pseudos valen lo mismo, CSE sustituye por el que tiene el ÚLTIMO
   USO más tarde.** Es la respuesta a la familia entera de near-misses «el
   objetivo compara contra un registro que nosotros no usamos». El corolario es
   operativo: **una MENCIÓN posterior —aunque sea un almacenamiento muerto—
   cambia la elección de CSE y cuesta cero bytes**, porque `reg_scan` corre antes
   de cse2 (toplev.c:4242) y `flow` borra la escritura después.

2. **Mirar `.cse` y `.cse2` por separado.** El volcado `.cse` daba ya el registro
   bueno y el `.cse2` lo estropeaba. Tres rondas buscaron la causa en la forma de
   la sentencia porque sólo se miraba el resultado. Regla práctica: **si el
   `.cse` coincide con el objetivo y el objeto no, la causa está en
   `-frerun-cse-after-loop` y es un problema de RANGOS DE VIDA, no de árbol.**

3. **La copia del representante de gcse-PRE (`mr rA, rB` en un preencabezado) es
   una FIRMA.** Dice que la expresión se lee dentro del bucle **y también fuera**,
   y por tanto que la sentencia de después del bucle lee la MEMORIA y no una
   variable. Es el argumento que decidió el árbol de `CullParts` y sirve para
   cualquier «me falta un `mr` justo antes de la rama de la guarda».

4. **Los `asm` de andamio caducan con el árbol.** El clobber de r11 de
   `CullParts` estaba medido como load-bearing en r36d, r48 y r49 (5 → 9 filas al
   quitarlo) y con el árbol corregido pasó a ser **lo único que faltaba** (4 →
   0). Añadir a la lista de comprobaciones: **cada vez que un cambio de árbol
   mejore una función, remedir TODOS sus `asm`**. Es una regla hermana de «las
   vedas caducan», pero más agresiva: aquí la veda no había caducado sola, la
   caducó el arreglo.

5. **`scripts/flagtest.py` (nuevo, en `scratchpad/r50_world/`) debería subir a
   `scripts/`.** Compila la TU reducida con y sin una bandera y compara **todas**
   las funciones de la unidad, con el control obligatorio: si el sha1 del objeto
   no cambia, avisa de que la bandera no ha llegado al compilador. Nueve
   funciones medidas en 20 s. Cualquier sospecha de cflags se contesta con una
   orden en vez de con una ronda.

6. **La lista `;; N regs to allocate:` del `.greg` es el orden YA ordenado por
   `allocno_compare`**, y `;; Register dispositions:` da el mapa pseudo → hard
   reg. Juntas resuelven un near-miss de dos registros intercambiados en tres
   minutos. Merece una línea en la descripción de `rtldump.py`.

7. **Cuidado con «bajar el `live` del otro».** En `CullParts` el camino aparente
   (bajar `live(141)` de 330 a 329) **empeora**: la prioridad sube hasta empatar
   con un TERCER allocno (`culldiv@ha`, 147) y el desempate por número lo manda a
   un registro equivocado. Antes de fijar el objetivo numérico de un empate hay
   que mirar **los vecinos de arriba y de abajo en la lista ordenada**, no sólo
   la pareja del diff.

---

## 5. Artefactos

`scratchpad/r50_world/` (889 kB en total; los volcados RTL grandes, 1,3 GB de
objetos de variante y el `.s` de 16 MB, borrados):

- `base_fncmp_*.txt` / `final_fncmp_*.txt` — el antes/después de las tres unidades.
- `d_cull_base.txt` / `d_cull_final.txt` — el `fndiff` de `CullParts` antes
  (99,42584 %, 832/836) y después (**100,0 %, 836/836, cero filas**).
- `d_ias_base.txt`, `d_smp_base.txt`, `d_uwy_base.txt` — los `fndiff` citados.
- `rtl/cull_alloc.txt` — las cuatro líneas del `.lreg`, la lista ordenada de
  allocnos y las disposiciones de `CullParts`, de donde salen las cifras del §1.5.
- `cr_only.cpp`, `cl_only.cpp`, `ts_only.cpp`, `wrn_only.cpp` — las TU reducidas
  (heredadas de la r49; siguen reproduciendo las ocho funciones byte a byte).
- `h.py` (medir una variante), `v.py` (banco: parchea, mide y restaura),
  `d.py` (diff alineado de una variante ya construida), **`flagtest.py`** (nuevo)
  y los bancos `c1.py`…`c19.py`, `i1.py`, `u1.py`.
- `*.cpp.HEAD` — las copias de partida de las cuatro fuentes.

**Aviso para el que herede el banco** (vigente desde la r49): `v.py` restaura la
fuente desde la copia `.HEAD`. Si has editado la fuente a mano antes de lanzar
una tanda, **refresca la copia `.HEAD` primero** o el banco te borra el trabajo.
