# r36b — zEcstasy: cuatro funciones, una palanca nueva

Encargo: `epCalculateLocalDirectionalPOS16` (2.072 B), `UpdatePlatInfo` (2.044 B),
`GenerateHorizonFogDisplayList` (796 B) y `EmitterSystem::Render` (696 B).

**Resultado en una línea: `UpdatePlatInfo` pasa de 98,659 % / 75 filas a
99,266 % / 35 filas sin emitir un byte** (2.044 B antes y después), y la palanca
que lo consigue —**el pin de registro `register T x asm("frN")`**— es nueva para
el proyecto y reabre todos los near-miss cuyo diagnóstico sea «reparto de
`local_alloc`».

Estado de la unidad, medido contra el último commit y con `report.json`
regenerado:

| función | antes | ahora |
|---|---|---|
| `UpdatePlatInfo` | 98,65949 % / 75 filas | **99,26614 % / 35 filas** |
| `epCalculateLocalDirectionalPOS16` | 93,305016 % / 155 | 93,305016 % / 155 (objeto byte a byte igual) |
| `GenerateHorizonFogDisplayList` | 98,99497 % / 2 | 98,99497 % / 2 |
| `EmitterSystem::Render` | 98,074715 % / 34 | 98,074715 % / 34 |

`zEcstasy` sigue `NonMatching` en `configure.py`, así que **el enlace usa el
objeto extraído y `main.dol` no puede moverse por esto**. `lcfix.py zEcstasy`
dice «todas las entradas `@lc` están al día» después de recompilar. No he tocado
`configure.py`, `config/GOWE69/*` ni `splits.txt`, y no he hecho commit.

---

## 1. La palanca: `register T x asm("frN")` rompe los empates de `local_alloc`

`UpdatePlatInfo` llevaba **cuatro rondas** (r27-r30) de análisis con un modelo
exacto de `allocno_compare` calibrado contra nueve pseudos, y la conclusión de la
r30 era que lo que quedaba «ya no lo mueve el número de insns». Es cierto: lo que
quedaba no es `global_alloc`, es `local_alloc`, y `local_alloc` **no se dirige
desde la forma de la fuente**. Se dirige desde el pin.

La escalera completa, cada peldaño medido en el banco suelto de `eLightE.cpp`
(≈35 s por variante) y luego confirmado en la unidad entera:

| paso | qué | % | filas | tamaño |
|---|---|---|---|---|
| base | — | 98,65949 | 75 | 2044/2044 |
| +3 | tres `__asm__("" : "+f"(diffuse_min_a));` en `case 0x33A26CB6` | 98,77691 | 66 | 2044/2044 |
| pin 1 | `register float diffuse_min_a asm("fr7")` | 99,01174 | 54 | 2044/2044 |
| pin 2 | `register float envmap_min_scale asm("fr5")` | **99,26614** | **35** | 2044/2044 |

**Ni un byte de más en ninguno de los cuatro.** El `+3` es la receta que la r30
había medido y descartado por ser un `asm`; el brief de la r36 la legitima
explícitamente (un `asm("")` extendido no emite un byte), así que la recupero.

`regmap` después: `diffuse_min_a` f7/f7 **ok** y `envmap_min_scale` f5/f5 **ok**
(las dos estaban distintas), y lo que queda es un **ciclo limpio de cuatro**:

    diffuse_rng_a   objetivo f5   nuestro f2
    envmap_power    objetivo f2   nuestro f1
    envmap_min_g    objetivo f1   nuestro f31
    envmap_max_g    objetivo f31  nuestro f5

O sea `f5 -> f2 -> f1 -> f31 -> f5`. Ese ciclo **no se rompe pinchando dentro de
él**, y eso está barrido entero (§4).

### La regla que sale, para el resto del árbol

- **El pin vale cuando `regmap` da un desplazamiento o un ciclo de registros y la
  fuente ya está agotada.** Es la única palanca que llega a `local_alloc`: la
  prioridad de `qty_compare` (`floor_log2(n_refs)*n_refs*size/(death-birth)`) sólo
  depende de refs y de LUIDs dentro del bloque, y ninguna de las dos cosas se
  mueve reordenando declaraciones (r30 lo midió: siete formas idénticas).
- **Se pincha DE UNO EN UNO y en la variable que `regmap` nombra**, con el
  registro del OBJETIVO. Pinchar dos a la vez casi siempre cuesta bytes: los
  cuatro pines juntos dan 96,331 % y **+8 B**.
- **Después de cada pin hay que volver a leer el diff**: el mapa cambia. El
  `envmap_min_scale -> fr5` que cierra 19 filas sobre la base de 54 estaba en
  `fr8` en el diagnóstico de la r28 y en el de la base sin pines.
- No es ensamblador de instrucciones ni una extensión rara: el árbol ya lo usa
  (`register float world_size asm("fr6")` en `EmitterSystem.cpp`). Lo que no se
  había hecho nunca es usarlo **como herramienta de reparación**; `pines.py` dice
  que los 193 pines del árbol están todos en funciones ya al 100 %.

---

## 2. `epCalculateLocalDirectionalPOS16`: el diagnóstico del triaje está incompleto

El triaje la vende como «el mejor premio: 2.072 B y sólo TRES registros movidos».
**No es eso.** Contadas las 155 filas:

    49 de 155  (32 %)  son SOLO EL MARCO
    106                son reparto/planificación

Normalizando cada offset de `(r1)` del objetivo con `+8`, esas 49 filas quedan
idénticas. Nuestro marco es `0x178` y el suyo `0x170`.

### La ranura, modelada al byte

Con los volcados `.lreg`/`.greg` del compilador (`-dlgr` sobre `EcstasyEx.cpp`
suelto) y el fuente de GCC que hay en `orig/prodg/NGC_GNU_SRC/`, el reparto de
ranuras de las locales **casa una a una con el asm**:

- son **18 pseudos** sin registro duro, saltándose los que llevan `REG_EQUIV`
  (constante o memoria: el 770 = `0x4330000080000000` de la conversión, el 790 =
  `0.5f`, y seis punteros con `pref BASE_REGS`), porque `alter_reg` no les da
  ranura;
- los `SF` que tienen un `(subreg:PS ...)` paradójico ocupan **8 B** y se leen en
  `+4` por la corrección big-endian: por eso 0x48/0x68/0x70/0x78 se referencian
  como 0x4C/0x6C/0x74/0x7C (139, 258, 603 = `envvdotn`, 718 = `specular_a`);
- **esas 18 terminan en 0x80 EN LOS DOS LADOS.**

Y el marco lo cierra `rs6000_stack_info` (`rs6000.c:3970-4080`): `fpmem_size` = 12
(8 + 4 de alineación), `fpmem_offset` = `total_size − 232`, que da **0x88 en el
objetivo y 0x90 en el nuestro**.

**Conclusión: los 8 B de más están entre 0x80 y el par de conversión, y no los
pone la pasada inicial de `alter_reg` sino RELOAD**, en `spill_stack_slot` de un
registro FP duro derramado (el `.greg` trae `Spilling reg 32/35/38/40/43/44/45`).
Nosotros abrimos **dos** ranuras de 8 B ahí y el objetivo **una**. La palanca es
presión FP en el bloque interno, no una local — coherente con que `scba` acabe en
**f13**, el último volátil, donde el objetivo usa f4, y con que `zero` de
`my_fpow3` acabe en f30 donde el objetivo usa f31.

Esto corrige la nota de la r30 («una ranura que `alter_reg` reserva») y le pone
número: **el 32 % de las filas de esta función cuelgan de ahí.**

### Lo que sí he arreglado: el orden del DWARF

El volcado del original declara `dsrg` **entre `rz` y `dsba`**; nosotros la
teníamos entre `dcba` y `specular_a`. Partiendo la declaración del cálculo
(`sn_ps dsrg;` arriba, la asignación donde estaba) **el árbol de locales queda
idéntico al del original y el objeto sale byte a byte igual** (93,305016 %, 155
filas). `dwbody.py` ya no marca ninguna diferencia de locales: sólo de registros.
Aplicado.

Mover también el **cálculo** arriba cuesta 26 filas (93,305 → 92,681 %,
155 → 181): medido y descartado.

---

## 3. `GenerateHorizonFogDisplayList`: el muro de la r30, confirmado por otra vía

Dos filas, una transposición de `srawi r0,r0,1` y `andi. r11,r31,1`. La r30 lo
declaró muro con números (`prio(216)=12` contra `prio(231)=2`, los dos listos en
`t=3`). **Lo he verificado con el permutador CIEGO** —`regmap` dice IDÉNTICO, así
que va el catálogo entero, no el guiado—:

    445 variantes de un solo cambio, 25,9 s cada una
    move_stmt 184 · block_wrap 65 · swap 55 · split_decl 28 · cse_temp 24 ·
    sink_decl 20 · split_assign 13 · bind_ref 12 · init_decl 10 · const 8 · ...
    NINGUNA mejora. score 0,969849 y la misma primera divergencia (fila 75).

Y he leído el criterio en el fuente del compilador que hay en el árbol:
`rank_for_schedule` (`haifa-sched.c:4168`) compara **`INSN_PRIORITY` antes que
nada**; el `INSN_REG_WEIGHT` sólo entra `if (!reload_completed)` y el `INSN_LUID`
—lo único que mueve la fuente— es el último criterio de siete. Con prioridades 12
y 2 no hay empate que romper. **Frente cerrado desde la fuente**: 16 formas de la
r30 + 445 del permutador.

---

## 4. Vedas nuevas, con la sentencia

### `EmitterSystem::Render` — doce colocaciones de la barrera selectiva

El eje que en `HolePunchAvoidables` cerró 34 filas. **Ninguna de las doce añade
un byte** (688 B en las doce), o sea que la barrera **sí** mueve el reparto de
GPR a coste cero; simplemente ninguna va en la dirección buena:

| resultado | dónde |
|---|---|
| absorbidas (34 filas, objeto idéntico) | `part`, `texture_hash`, `submitParticles`, `num_textures`, `em`, `axis_constrained` |
| peores | `sprite_hack_flags` 48 · `particle` 47 · `view` 46 · `plist` 43 · `grp` 41 · `world_size` con `"+f"` en vez de `"f"` 59 |

Sigue haciendo falta lo que dijo la r30: que **un pseudo NO QUEPA** para que
`reload` coja r19 como registro de derrame. La barrera no lo consigue.

### `UpdatePlatInfo` — lo que no rompe el ciclo de cuatro

Todo sobre la base nueva (`+3` + los dos pines, 35 filas):

| ensayo | filas | tamaño |
|---|---|---|
| pin `envmap_power` → fr2 | 38 | 2044 |
| pin `envmap_max_scale` → fr8 | 35 (absorbido) | 2044 |
| pin `envmap_min_g` → fr1 | 72 | 2044 |
| pin `envmap_max_g` → fr31 | 74 | 2044 |
| pin `diffuse_rng_a` → fr5 | 96 | **2040** |
| `epow`+`eming` / `epow`+`emaxg` / `eming`+`emaxg` | 79 / 65 / 134 | 2044/2044/2036 |
| los cuatro del ciclo a la vez | 187 | **2032** |
| pin `diffuse_max_scale` → fr6 encima de los dos | 155 | **2052** |
| barrera detrás de la definición de `envmap_power` / `envmap_min_g` / `envmap_max_g` / `diffuse_rng_a` | 69 / 79 / 145 / 78 | — |
| barrera alargando `diffuse_max_scale` (2 sitios) o `diffuse_rng_a` o un 4.º `diffuse_min_a`, delante del `switch` | 215 / 105 / 113 | 2052 los de `diffuse_max_scale` |
| barrera sobre `envmap_min_scale`, `specular_min_scale`, `envmap_max_scale` delante del `switch` | absorbidas | — |
| número de barreras con los dos pines: 0 → 45, +2 → 44; **+3, +4, +5 y +6 dan todos 35** | — | 2044 |
| número de barreras SIN pines: +1 → 75, +2 → 75, +3..+6 → 66 | — | 2044 |

Es decir: **el `+3` satura** y el ciclo de cuatro que queda no se rompe ni con
pines ni con `live_length`. Es el siguiente muro de esta función.

### `epCalculateLocalDirectionalPOS16`

| ensayo | resultado |
|---|---|
| subir la DECLARACIÓN de `dsrg` (split decl) al sitio del DWARF | **objeto idéntico**, DWARF corregido. APLICADO |
| declarar `sn_ps dsrg;` detrás de `ssrg` en vez de detrás de `rz` | objeto idéntico también (GCC funde el ámbito) |
| subir el CÁLCULO de `dsrg` junto a `dsba` | 93,305 → **92,338 %**, 155 → 181 filas. Revertido |

---

## 5. Herramientas y bancos que dejo montados

- **Los tres bancos sueltos vuelven a funcionar** y reproducen el objeto de la
  unidad EXACTO: `eLightE.cpp` (sin prefacio, 98,65949 %), `EcstasyEx.cpp` (con el
  prefacio de la r30: `dolphin.h`, `EcstasyEx.hpp`, `GameCube/.../Ecstasy.hpp`,
  los dos `extern` de `SphereMap`/`SpecularMap` y las ocho tablas `test*`) y
  `EmitterSystem.cpp` (`Profiler.hpp` + `IPlayer.h`).
- **Aviso para el que herede los arneses**: `EcstasyEx.cpp`, `eLightE.cpp` y
  `EmitterSystem.cpp` son **CRLF**. Un fichero de variantes con `\n` aplica los
  cambios de una línea y falla en silencio los de varias (`count=0`).
- El modelo de ranuras de pila del §2 (18 pseudos, `REG_EQUIV` fuera, `subreg:PS`
  paradójico a 8 B) es reutilizable en cualquier función con marco de más: casa
  offset a offset y dice cuántas ranuras pone `reload` de más.

## 6. Lo que NO he tocado

`eProject__FfffPA3_fPfN44` (268 B, 85,821 %, 25 filas) está en mi territorio pero
no en mi encargo; sigue como estaba. No he tocado ningún fichero fuera de
`src/Speed/GameCube/Src/Ecstasy/**` y `src/Speed/Indep/Src/Ecstasy/**`, ni
`JoyE.cpp`, ni he repetido la retirada en bloque de las 691 directivas `.size`.
