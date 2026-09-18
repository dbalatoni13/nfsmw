# r54 — flares (`RenderFlaresOnCar` + `UpdateWheelYRenderOffset`, zWorld)

Territorio: 2 funciones, 3.784 B. **Cero bytes cerrados**, que es lo que pedía el
brief de una ronda de diagnóstico. Lo que sale:

1. **El experimento del volcado escrito en la r52 y nunca corrido está CORRIDO, y
   la hipótesis queda REFUTADA con la cláusula exacta que falla.** No es una veda
   empírica: es estructural, y vale para todo el proyecto.
2. **El frente estaba mal identificado desde la r49.** El `lis` que falta no es
   una decisión sobre `TweakKitWheelOffsetRear` ni la toma `combine_movables`: la
   toma **`cse2`**, sobre el literal `0.0f`. Medido pase a pase.
3. **El frente de cflags de `CarRender.cpp` queda cerrado del todo**: ocho
   banderas nuevas barridas con el control obligatorio, **0 mejoran** las ocho.
   Con las cuatro de r50/r52 son **doce de doce** verificadas.
4. Una herramienta nueva, `hicount.py`, que contesta «¿ha movido la fuente lo que
   importa?» en 10 s y **sin mirar el porcentaje**.

`fncmp` de zWorld **idéntico** antes y después
(`scratchpad/r54_flares/{base,final}_fncmp_zWorld.txt`, diff vacío).
`linkdelta zWorld`: `.text +0  rodata-472 data-128`, igual que lo dejó la r53.
El único cambio de fuente es **un comentario** (§7), verificado byte a byte.

---

## 1. EL EXPERIMENTO: el volcado `.cse` y los usos de `(high Rear)`

### 1.0 Primero, una corrección al brief que cuesta media tanda

> `python scripts/rtldump.py zWorld <fn> -dc     # el volcado .cse`

**`-dc` es `.combine`, no `.cse`.** `toplev.c:5246` (`case 'c': combine_dump = 1;`)
contra `toplev.c:5300` (`case 's': cse_dump = 1;`). La tabla completa que hace
falta aquí:

| letra | volcado | letra | volcado |
|---|---|---|---|
| `-ds` | `.cse` | `-dt` | `.cse2` |
| `-dG` | `.gcse` | `-dc` | `.combine` |
| `-dL` | `.loop` | `-dl` / `-dg` | `.lreg` / `.greg` |

Y el pase que corre **inmediatamente antes de `loop`** no es `cse` sino **`gcse`**
(`toplev.c:4161-4183`), así que la RTL que ve `scan_loop` es la del volcado
`.gcse`, no la del `.cse`. Con `-dG` se ve la entrada exacta a `loop`.

### 1.1 El conteo que pedía la hipótesis: **UNO**

`(high TweakKitWheelOffsetRear)` es el pseudo **169**, definido en el insn 361.
En el `.gcse` —la entrada literal a `loop`— aparece exactamente dos veces, la
definición y **un solo uso**:

```
(insn 361 360 364 (set (reg:SI 169)
        (high:SI (symbol_ref:SI ("TweakKitWheelOffsetRear")))) 506 {elf_high}
    (expr_list:REG_EQUAL (high:SI (symbol_ref:SI ("TweakKitWheelOffsetRear")))))

(insn 364 361 365 (set (reg:SI 170)
        (mem/f:SI (lo_sum:SI (reg:SI 169)
                (symbol_ref:SI ("TweakKitWheelOffsetRear"))) 0)) 512 {movsi+1})
```

`(high TweakKitWheelOffsetFront)` (pseudo 151, insns 282/285) es **idéntico**:
una definición y un uso. Los dos son simétricos, insn a insn.

O sea que **la premisa de la hipótesis se cumple**: `reg_single_usage[169]` está
puesto y no es `const0_rtx`. Y las demás puertas también:

| puerta de `loop.c:939-960` | estado | prueba |
|---|---|---|
| `loop_has_call` | **sí** | 14 `call_insn` con UID en [159, 852] |
| `reg_single_usage != 0, != const0_rtx` | **sí** | un solo uso (arriba) |
| `REGNO_FIRST_UID == INSN_UID(p)` | sí | def 361, primer uso 361 |
| `REGNO_LAST_UID == INSN_UID(uso)` | sí | último uso 364 |
| `set_in_loop == 1` | sí | una sola definición |
| `! side_effects_p (SET_SRC)` | sí | es un `high` |
| `! find_reg_note (p, REG_RETVAL)` | sí | sólo tiene `REG_EQUAL` |
| `! modified_between_p` | sí | 361 y 364 son contiguos |
| `no_labels_between_p` | sí | 361 y 364 son contiguos |
| **`validate_replace_rtx`** | **NO** | ver §1.2 |

### 1.2 Por qué la puerta NO PUEDE abrirse — y no sólo aquí

La sustitución que intenta `scan_loop` es meter el `SET_SRC` dentro del único
uso: cambiar `(reg 169)` por `(high (symbol_ref "TweakKitWheelOffsetRear"))`
dentro de `(mem (lo_sum (reg 169) (symbol_ref ...)))`. El resultado sería

    (mem/f:SI (lo_sum:SI (high:SI (symbol_ref …)) (symbol_ref …)))

y en PowerPC eso **no es una dirección legítima**:

```c
/* rs6000.h:2149 */
#define LEGITIMATE_LO_SUM_ADDRESS_P(MODE, X)            \
  (TARGET_ELF … && GET_CODE (X) == LO_SUM               \
   && GET_CODE (XEXP (X, 0)) == REG                     \  <-- aquí muere
   && REG_OK_FOR_BASE_P (XEXP (X, 0))                   \
   && CONSTANT_P (XEXP (X, 1)))
```

`validate_replace_rtx` llama a `recog`, `recog` pide `GO_IF_LEGITIMATE_ADDRESS`,
y el primer operando del `lo_sum` deja de ser un `REG`. **Falla siempre.**

Corroboración de fuerza bruta, por si el argumento de código no bastara:
`lo_sum:SI (high` aparece **0 veces** en los **siete** volcados
(`.cse`, `.gcse`, `.loop`, `.cse2`, `.combine`, `.flow`, `.lreg`) de las **599
funciones** que reproduce la TU. Ni una.

> **VEDA NUEVA, estructural y de proyecto:** la puerta «potential lossage» de
> `scan_loop` (`loop.c:939`) **no puede disparar nunca sobre un `elf_high` de
> PowerPC cuyo único uso sea la dirección de un `mem`**, porque la sustitución
> produce `(lo_sum (high …) …)` y `LEGITIMATE_LO_SUM_ADDRESS_P` exige `REG`. No
> disparó para nosotros y **tampoco pudo disparar para el objetivo**. La asimetría
> Front/Rear no tiene nada que ver con `scan_loop`.

Y en efecto `loop` iza los dos igual, sin distinguirlos (volcado `.loop`):

```
Loop from 159 to 852: 206 real insns.
Insn 282: regno 151 (life 1), move-insn savings 1  moved to 960     <- (high Front)
Insn 361: regno 169 (life 1), move-insn savings 1  moved to 962     <- (high Rear)
…
Insn 834: regno 325 (life 2), done move-insn matches 785
Insn 837: regno 326 (life 1), done move-insn matches 788
```

Las dos únicas líneas `matches` de `combine_movables` son los regnos **325 y
326**, que no son ningún `high` del literal. **`combine_movables` no toca a
nadie de este frente**, contra lo que decía el comentario de la r49.

---

## 2. Dónde está de verdad la función: el literal `0.0f`

`UpdateWheelYRenderOffset`, 872/876 B, **7 filas**, de las cuales sólo tres son
reales (las otras cuatro son nombres de destino de salto):

```
 48 lis r15, 0x4330                    | lis r16, TweakKitWheelOffsetRear@ha
 49 lis r16, lbl_8040AA84@ha           | lis r15, 0x4330
 97 lis r9, TweakKitWheelOffsetRear@ha |                                  <- los 4 B
 98 lwz r0, …Rear@l(r9)                | lwz r0, …Rear@l(r16)
169 lfs f0, lbl_8040AA84@l(r16)        | lfs f0, $LC573@l(r19)
```

`lbl_8040AA84` es **`.float 0`** (`zWorld.s`, `.rodata:0x…`). Los dos
preencabezados miden **19 insns exactamente iguales** salvo una casilla:

| | objetivo (filas 32-50) | nuestro |
|---|---|---|
| r14 | `high(Front)` | `high(Front)` |
| r19 | `high(0.0f)` → carga `f26` (fila 42) **y** fila 191 | igual |
| **r16** | **`high(0.0f)` #2** → fila 169 | **`high(Rear)`** |
| Rear | rematerializado en el bucle, r9 volátil (fila 97) | izado a r16 |

O sea: **al objetivo no le sobra un registro para `Rear`, le sobra un
`(high 0.0f)` que se lo ocupa.** El `lis` de más de la fila 97 es la
consecuencia, no la causa. Con lo cual el frente es: **cuántos pseudos
`(high $LC 0.0f)` llegan vivos al asignador.**

Y `RenderFlaresOnCar` (2.912/2.908 B, 18 filas) es **la misma cosa y el mismo
literal**: `lbl_8040AD04` también es `.float 0`.

```
123 lis r16, lbl_8040AD04@ha       |                    <- el objetivo lo saca PRONTO
452                                | lis r9, lbl_8040AD04@ha    <- +4 B nuestros
454                                | lfs f1, lbl_8040AD04@l(r9)
460 lfs f1, lbl_8040AD04@l(r16)    | lfs f31, lbl_8040ACF0@l(r9)
600 lfs f1, lbl_8040AD04@l(r16)    | lfs f1, lbl_8040AD04@l(r30)
601 lfs f2, lbl_8040AD04@l(r16)    | lfs f2, lbl_8040AD04@l(r30)
```

`lmap` sobre el objetivo: **r16 se materializa una vez** en `CarRender.cpp:4061`
y sirve las tres cargas hasta `:4314`. Nosotros partimos ese rango en dos: un
volátil r9 para la primera y un preservado r30 creado 338 filas más tarde para
las otras dos. Los **16** `lis r9, 0.0f@ha` volátiles del resto de la función son
**idénticos en las dos columnas**.

---

## 3. El censo por pase: quién funde y quién mata

Herramienta nueva: **`scratchpad/r54_flares/hicount.py`**. Cuenta las
definiciones `(set (reg) (high (symbol_ref "*$LCn")))` que sobreviven a cada
volcado, identificando el literal **por su valor** (`const_double … [0]`) y no por
el número, que cambia con cualquier edición de la fuente.

    definiciones de (high 0.0f) que sobreviven

    funcion   .cse   .gcse   .loop   .cse2   .combine    objetivo
    UWY          3       1       2       2          1           2
    RFC         17       2       2      18         16          17

Lectura, insn a insn:

- **`gcse` (PRE) funde las tres ocurrencias del bucle en una** (`reg 330`,
  insn 932) y la coloca al final del bloque anterior al bucle. Las tres cargas
  del `0.0f` pasan a leer de ella.
- **`loop` crea una segunda** (`reg 345`, insn 974) al reizar la carga invariante
  `lfs f26`: con `-fforce-addr` la dirección se rehace entera
  (`974: high` → `975: lo_sum` → `977: mem`).
- **`cse2` la mata.** En `.loop` el insn 975 es `(lo_sum (reg 345) $LC498)`; en
  `.cse2` es `(lo_sum (reg 330) $LC498)`. El `reg 345` se queda sin usos y
  `combine`/`flow` lo borran.
- En `RFC` el mismo pase hace lo contrario y en grande: `.loop` deja
  `(insn 537 (set (reg 235) (reg 1007)))` —la copia que dejó `move_movables`— y
  `.cse2` la devuelve a `(set (reg 235) (high $LC541))`. **`cse2` DESHACE el izado
  de `loop` para los `high`**, y por eso 16 `lis` vuelven al bucle.

El mecanismo en el fuente de GCC, con línea:

- `HIGH` es `CONSTANT_P` (`rtl.h:243`).
- Por eso `insert()` le asigna `qty_const` a la cantidad del pseudo
  (`cse.c:1580-1600`).
- Y `canon_reg`/`fold_rtx` reescriben cualquier uso posterior al **primer**
  registro de esa cantidad (`qty_first_reg`), que es el de `gcse` porque su
  insn 932 va antes que el 974.

> **AL CATÁLOGO:** *un `(high sym)` izado fuera de un bucle no sobrevive a `cse2`
> si alguno de sus usos cae en el mismo bloque básico extendido que una
> definición anterior del mismo `(high sym)`.* No lo decide `loop`, ni
> `combine_movables`, ni la presión de registros: lo decide `cse2` con
> `qty_const`.

---

## 4. El control que TIENE que cambiar, y el barrido de banderas

### 4.1 El control del mecanismo

Cinco banderas, midiendo **la cuenta de `(high 0.0f)` en `.combine`**, no el
porcentaje:

| bandera quitada | UWY | RFC |
|---|---:|---:|
| **base** | 1 | 16 |
| `-fgcse` | 1 | 16 |
| `-fmove-all-movables` | 1 | 16 |
| `-fforce-addr` | 1 | 16 |
| `-frerun-loop-opt` | 1 | 16 |
| **`-frerun-cse-after-loop`** | **2** ← la del objetivo | **1** |

**Sólo `cse2` mueve la cuenta.** Y la mueve exactamente al 2 que necesita UWY,
lo que cierra el diagnóstico: el pase es ése y no otro.

### 4.2 …pero la bandera está bien: el frente de cflags queda CERRADO

Con `ft.py` sobre la TU reducida (599 funciones, sha1 del `.o` distinto en las
cinco, o sea que la bandera llega):

| bandera quitada | mejoran | empeoran | efecto en UWY / RFC |
|---|---:|---:|---|
| `-ffast-math` | **0** | 92 | 99,38 → 96,99 / 98,62 → 97,94 |
| `-fschedule-insns` | **0** | 42 | 99,38 → 87,68 / 98,62 → 85,24 |
| `-fschedule-insns2` | **0** | 35 | 99,38 → 95,48 / 98,62 → 90,88 |
| `-frerun-cse-after-loop` | **0** | 30 | 99,38 → 93,65 / 98,62 → 87,65 |
| `-fexpensive-optimizations` | **0** | 14 | 99,38 → 99,06 / — |
| `-fcse-skip-blocks` | **0** | 12 | — / 98,62 → 95,01 |
| `-fcse-follow-jumps` | **0** | 10 | 99,38 → 96,60 / 98,62 → 97,88 |
| `-frerun-loop-opt` | **0** | 4 | — |

Sumadas a `-fgcse` (0/26), `-fforce-addr` (0/26), `-fforce-mem` (0/10) y
`-fmove-all-movables` (0/9) de r50/r52: **las DOCE banderas de optimización de
`CarRender.cpp` están verificadas una por una, y ninguna mejora ni una sola de
las 599 funciones.** El frente de cflags de esta unidad está agotado; no volver.

---

## 5. La condición exacta, con el número, y por qué la fuente no la da

`cse_end_of_basic_block` (cse.c) recorre el camino con

```c
  while (p && GET_CODE (p) != CODE_LABEL)   /* … */
```

y `new_basic_block()` —que vacía la tabla y los `qty_const`— se llama **una sola
vez por camino** (`cse.c:8923`), más un vaciado forzoso cada **1.000** insns que
no sean NOTE. O sea que lo único que corta la unificación de `reg 345` con
`reg 330` es **un `CODE_LABEL` entre las dos definiciones**.

> **CONDICIÓN DE VICTORIA:** `CODE_LABELs entre insn 932 y insn 974 ≥ 1`.
> **Hoy son 0.** Y entre las dos hay **15 insns y 1 NOTE**, ninguna etiqueta.

Y esa condición **no es alcanzable desde C, por construcción**: `gcse` inserta al
**final** del bloque que precede al bucle (`insert_insn_end_bb`) y `loop` inserta
**justo antes de `NOTE_INSN_LOOP_BEG`**, que es el final de ese mismo bloque.
**No hay posición de fuente entre las dos.**

Medido, no supuesto. Variante `w_label` — una etiqueta de `goto` calculado
(`void *pl = &&UWY_PRE; UWY_PRE:`) inmediatamente delante del `for`:

```
literal 0.0f = $LC498   defs: ['939', '981']
ultimo code_label ANTES de la primera def:
    (code_label 160 155 884 58104 ("UWY_PRE") [num uses: 3])
CODE_LABELs entre las dos defs: 0
```

La etiqueta **existe en la RTL** y cae **antes** de la inserción de `gcse`, no
entre las dos. Resultado: **872 B y 7 filas, exactamente la base** — cero efecto
y, ojo, **cero coste** (matiza la medida de la r53 en `HolePunchAvoidables`,
donde `&&L` costaba +4/+8 B: ahí la etiqueta iba **dentro** del bucle y el
puntero seguía vivo; aquí `pl` está muerto y GCC borra el `lis/addi`).

Además el objetivo **tiene la misma etiqueta que nosotros** justo delante del
preencabezado (`lmap`: `.L_802CA038` antes de la fila 32, `.L_802CA084` en la
entrada del bucle), así que la estructura de bloques básicos es idéntica en las
dos columnas. No es por ahí.

---

## 6. Las nueve variantes de fuente, con la métrica mecánica

Banco `bank.py` (parchea, compila la TU reducida, mide **filas + bytes + cuenta
de `(high 0.0f)` en `.combine`** de las dos funciones a la vez, restaura).
Base: UWY 7 filas 872/876 · RFC 18 filas 2.912/2.908 · UWY high0=1 · RFC high0=16.

| variante | UWY filas | UWY B | UWY high0 | RFC | veredicto |
|---|---:|---:|---:|---|---|
| `base` | 7 | 872 | 1 | 18 / 2912 / 16 | — |
| `w_swap` (los dos `if` intercambiados) | 17 | 872 | **1** | igual | no mueve el mecanismo |
| `w_rev1` (`0.0f < model_width && …`) | 7 | 872 | **1** | igual | objeto idéntico |
| `w_rev2` (lo mismo en el radius) | 7 | 872 | **1** | igual | objeto idéntico |
| `w_tern` (`0.0f >= WheelYRenderOffset[…]`) | 7 | 872 | **1** | igual | objeto idéntico |
| `w_zero_all` (`const float zero` fuera del bucle, en las 3) | 7 | 872 | **1** | igual | CSE lo normaliza |
| `w_zero_w` (sólo en el `width`) | 7 | 872 | **1** | igual | igual |
| `w_decl` (declaraciones reordenadas) | 67 | 876 | **1** | igual | rompe medio cuerpo |
| `w_label` (`&&UWY_PRE` delante del `for`) | 7 | 872 | **1** | igual | §5: 0 B, 0 efecto |
| **`w_neg2` (control: `> -0.0f` en el radius)** | 10 | **876** | 1 (+1 de `$LC500`) | igual | **la métrica SÍ es sensible** |

**El control obligatorio:** `w_neg2` mete un literal nuevo (`-0.0f`, `$LC500`) y
con él **una materialización más**, y el tamaño sale **876/876, el del objetivo,
al byte**. Confirma dos cosas: (a) la aritmética —un `(high)` de más son
exactamente los 4 B que faltan— y (b) que la métrica se mueve cuando tiene que
moverse. Se **rechaza** porque el insn cae en la fila 191 y no en la 97, y porque
añade a `.rodata` un `-0.0f` que el objetivo no tiene.

Las cuatro vedas de r36e/r47 que ya estaban medidas por filas quedan ahora
medidas **por mecanismo**: `w_rev1`, `w_rev2`, `w_tern` y `w_zero_*` dan objeto
idéntico *y* `high0` idéntico. No eran «formas de la sentencia equivocada»: es
que **ninguna forma de la sentencia puede tocar `qty_const`**.

---

## 7. Lo único que he tocado de la fuente: un comentario que mentía

`src/Speed/Indep/Src/World/CarRender.cpp`, 12 líneas del bloque de comentarios de
`UpdateWheelYRenderOffset`. La atribución de la r49 —«EL MECANISMO TIENE NOMBRE:
`combine_movables` (loop.c:1470)»— es **falsa** (§1.2 y §3) y es la que me ha
costado media ronda de arqueología. Sustituida por el censo por pase, el nombre
correcto del pase y las líneas de `rtl.h`/`cse.c`. **Mismo número de líneas**;
`fncmp` de zWorld idéntico después de recompilar. El párrafo de la r47 que hay
encima **es correcto y no lo he tocado**: predice exactamente lo que he medido
(r19 sirve las filas 42 y 191, r16 la 169).

---

## 8. Lo que NO he tocado, y por qué

- **`DefragmentPool` (684 B, 23 filas).** Confirmado lo de la r53: no hay ningún
  `asm` escrito en la función al que añadirle una entrada `"r"(x)`, así que la
  palanca buena de la r52 no tiene dónde agarrarse, y **crear un `asm` nuevo
  tendría que pagarse solo** partiendo de 14 clobbers ya medidos y negativos.
  Con el hallazgo de §3 el diagnóstico de la r52 (permutación de siete allocnos
  globales) sigue en pie y su ataque es el horario de `sched1`, no la fuente.
- **`SetMemoryPoolSize` (304 B, 2 filas).** Siete ensayos negativos en la r52 y
  el diagnóstico de la r49 (hay que alargar en uno la cadena `lis → … → llamada`,
  y aquí no se puede sin bytes) no lo mueve nada de esta ronda.

---

## 9. Qué haría la r55 con esto (y qué NO)

**NO volver a:** las cflags (§4.2, nueve de nueve), la puerta de `scan_loop`
(§1.2, imposible por la máquina), `combine_movables` (§1.2, no toca este frente),
ni las formas de las comparaciones con `0.0f` (§6, nueve variantes con métrica
mecánica).

**Lo único vivo**, y es una pregunta concreta y contestable con un volcado:
en el objetivo, `gcse` (PRE) atrapó **dos** de las tres ocurrencias del `0.0f` y
dejó la tercera en el bucle —de ahí el segundo pseudo, que sobrevive a `cse2`
porque **su único uso está dentro del bucle**, en otro camino de `cse`—; en
nosotros las atrapa **las tres**. Un `(high)` izado sobrevive a `cse2` **si y sólo
si ninguno de sus usos cae en el mismo camino que una definición anterior**, y
hoy tenemos exactamente **UN** uso así (el insn 975, el `lo_sum` que `-fforce-addr`
fabrica para la carga invariante) cuando hacen falta **CERO**. La pregunta
abierta, entonces, es **qué hace que `pre_delete` deje una ocurrencia sin tocar**,
y se contesta leyendo las tablas de `gcse.c` en el volcado `.gcse` (que ya trae
`22 substs, 13 insns created` para UWY) — no probando más formas de la fuente.

Y si eso no da, la conclusión honesta es que estas dos funciones son **el mismo
techo que `HolePunchAvoidables`**: un empate interno de un pase que la fuente no
alcanza, y el sitio donde hay que gastar la ronda es otro.

---

## 10. Artefactos y sondas — `scratchpad/r54_flares/` (2,4 MB)

Volcados RTL borrados (eran 40 MB; el disco está al 98 %).

- `base_fncmp_zWorld.txt` / `final_fncmp_zWorld.txt` — **diff vacío**.
- `hicount.py` — **candidata a `scripts/`**. Cuenta los `(high $LC)` que
  sobreviven a cada pase, con el literal identificado **por su valor**. Acepta
  `--extra` de banderas. Es la métrica de §3 y §4.1, 10 s por variante, y es
  genérica: cualquier near-miss de «un `lis` de más/de menos» se contesta con
  ella en vez de con media ronda.
- `bank.py` — banco de variantes que mide **filas + bytes + `high0`** de las dos
  funciones a la vez. Es `v.py` de la r52 con la métrica mecánica añadida.
  **Aviso**: restaura desde `CarRender.cpp.HEAD`, que he refrescado con el
  comentario corregido de §7 — si editas la fuente a mano, refréscalo otra vez o
  te comes el cambio (me pasó).
- `x1.py`, `x3.py` — los dos bancos de variantes; `x2.py` imprime las filas de
  `w_neg2`; `x4.py` es la sonda de la etiqueta de §5.
- `ft.py`, `h.py` — heredadas de la r52, repuntadas a este directorio.
- `uwy_{cse,gcse,loop,cse2,combine,lreg}.txt` y `rfc_*.txt` — la RTL de las dos
  funciones extraída pase a pase (2.000-9.400 líneas cada una). Es lo que hay que
  volver a mirar si alguien retoma §9, y ahorra recompilar.
- `d_uwy_base.txt`, `d_rfc_base.txt` — los `fndiff` de la base.
- `lmap_uwy.txt`, `lmap_rfc.txt` — el asm del objetivo con la línea de fuente;
  de ahí salen `.L_802CA038`, `CarRender.cpp:4061` y el rango de r16.
- `f54cr.cpp` — la TU reducida (prefijo de bWare + un `#include`), con nombre
  propio para que sus volcados no choquen con los de otro agente en
  `scratchpad/rtl/`.
