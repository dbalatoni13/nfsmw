# r63 - mw: LibSN/steering (las seis de la familia E)

Encargo: las seis funciones abiertas de `src/LibSN/steering.c` (2.568 B, 6 de las
27 abiertas del proyecto), con el ensayo del `#pragma scheduling` acotado a
`SimThread_Init` como cabeza de serie.

**Resultado: NO PROMOCIONA, y el codigo no se ha tocado** -- el `.o` sale con el
mismo sha1 que al empezar. Lo que traigo son cinco cosas medidas, en orden de
valor:

1. **Una PALANCA NUEVA de Metrowerks, caracterizada al byte**: `asm { li rN, 0 }`
   dentro de una funcion **excluye `rN` del reparto de esa funcion**, cuesta
   **cero bytes** (la instruccion se elimina) y es **independiente de la
   posicion**. Es la transposicion a MWCC de la cantidad fantasma, y
   `memory/nfsmw-catalogo-metrowerks` no la tenia. Sirve para steering,
   OdemuExi2 y Packages. **En estas seis funciones es negativa**, pero esta
   medida y es reutilizable.
2. **La familia E contestada del todo por el eje de los `#pragma`: 150 medidas,
   ninguna mejora.** 25 pragmas (23 legales) por las 6 funciones. El eje queda
   **CERRADO** para la unidad entera, no solo para `SimThread_Init`.
3. **MWCC no tiene pin de registro local, y lo dice el propio compilador.**
   `register int v asm("r6")` compila con el aviso *"global register variable
   declarations are ignored when they occur within a function"*. Cierra la
   transposicion de `nfsmw-pin-de-registro` a las unidades de Metrowerks.
4. **Dos afirmaciones de `r60b-auditoria-vedas.md` sobre esta unidad son
   FALSAS**, y las dos mandaban a la proxima ronda a repetir trabajo hecho.
5. **CookValues queda con el negativo mas fuerte del fichero**: 24 ordenes de
   sentencia dan **UN SOLO objeto** byte a byte, mas 13 formas y 63 subconjuntos
   de registro bloqueado. Sigue en 9 filas y una sola transposicion r4<->r6.

---

## 0. Estado de partida y de llegada (identicos: no hay regresion)

| | |
|---|---|
| `fncmp` | 6 de 36 distintas, 2.568 B (30 exactas + 3 solo-alias) |
| `linkdelta` | `.text -8  bss+32` |
| `dolwhere` | no aplica: la unidad no enlaza (`Object(NonMatching, ...)`) |
| `textorder` | no aplica: no es SourceList, un solo `.c` |
| `trypromo` | DOL ROTO |

**Sello triple** del `.o`, tres pasadas de `build_direct.py`, sha1 identico en
las tres, ANTES y DESPUES:

```
steering  1962b7d16adc91a21e4f8070dacc3113935a219e
```

Y las cifras reales por funcion, medidas hoy con `fndiff`
(`function_reloc_diffs=none`, `ppc.calculatePoolRelocations=false`):

| funcion | B | tamano | filas | fuzzy | tipo |
|---|---:|---:|---:|---:|---|
| `SimThread_Step` | 924 | **920**/924 | 68 | 95,671 % | reparto + 4 B de menos |
| `HandleTriggers` | 588 | 588/588 | 42 | 98,299 % | reparto |
| `SimThread_Init` | 324 | 324/324 | 9 | 94,691 % | 2 de ranura + empate r5/r6 |
| `Effect_Init` | 276 | 276/276 | 23 | **95,0 %** | reparto |
| `CookValues` | 260 | 260/260 | 9 | **99,077 %** | UNA transposicion r4<->r6 |
| `Effect_PerformEnvelope` | 196 | **192**/196 | 31 | 93,265 % | falta un `mr`, 4 B de menos |

---

## 1. LA PALANCA NUEVA: `asm { li rN, 0 }` excluye un registro, gratis

Nacio buscando por que `CookValues` da `_v` en r4 y `_mn` en r6 cuando el
objetivo los da al reves. Es lo unico de toda la ronda que MUEVE el reparto de
MWCC de forma dirigida.

### Que hace, medido

```c
static void CookValues(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal)
{
    ...
    asm { li r6, 0 }          /* <- esto */
    _bits = (u8)raw->steering;
```

Antes: `_mn` en **r6**. Despues: `_mn` en **r7**. Tamano de la funcion: **260 B
en los dos casos**, o sea que **el `li` no se emite**: MWCC lo apunta como uso
del registro, lo elimina como codigo muerto, y el asignador ya no puede repartir
`r6`.

### Las cuatro propiedades, cada una con su medida

| propiedad | como se midio | resultado |
|---|---|---|
| **cuesta 0 bytes** | tamano de `CookValues` con y sin | 260 / 260 |
| **es por FUNCION** | `fncmp` de las otras 35 | intactas en las 63 pruebas |
| **es INDEPENDIENTE DE LA POSICION** | 4 posiciones dentro del bloque x 10 registros | **sha1 identico** en las 4 para cada registro |
| **el `asm {}` vacio es INERTE** | `asm { }` al principio de `SimThread_Step` y de `CookValues` | `.o` **byte a byte identico** (`1962b7d1`) |

Esa ultima fila es el control que hace valida a la tabla: lo que mueve el codigo
es **nombrar el registro**, no meter un `asm`. (Y el contraste: `asm { nop }` SI
emite y cambia el tamano, 920 -> 916.)

### Aviso: con un registro PRESERVADO crece el marco

Bloquear `r14`/`r15` no es gratis: `HandleTriggers` pasa de 588 a **604 B** y
`Effect_PerformEnvelope` de 192 a **208 B**. La palanca es gratis solo en los
volatiles.

### Barrido completo sobre las seis: NEGATIVO

Un `asm { li rN, 0 }` como primera sentencia, `rN` de r3 a r12 mas r14/r15, en
cada una de las seis (72 medidas). **Ninguna baja de su base**:

```
CookValues              base  9 filas   mejor con guarda  9 (r6, y r7..r12 = base)
SimThread_Init          base  9 filas   mejor con guarda 11 (r8)
Effect_Init             base 23 filas   mejor con guarda 26 (r5)
HandleTriggers          base 42 filas   mejor con guarda 50 (r9)
Effect_PerformEnvelope  base 31 filas   mejor con guarda 31 (r4/r5/r8/r9), y NUNCA llega a 196 B
SimThread_Step          base 68 filas   mejor con guarda 116 (r6), y NUNCA llega a 924 B
```

Y en `CookValues` se barrieron ademas **los 63 subconjuntos** de
{r3,r4,r5,r6,r7,r8,r9} de tamano 1, 2 y 3: minimo 9 filas, el de la base.

**Como se aplica en otra unidad de MWCC**: si `regmap`/`fndiff` dice que una
local esta en el registro equivocado y el objetivo usa uno mas alto, meter
`asm { li <el registro que sobra>, 0 }` en la funcion y volver a medir. Cuesta
una compilacion de 1,1 s y cero bytes.

---

## 2. El eje de los `#pragma`: 150 medidas, cerrado

`r62` probo **tres** pragmas sobre `SimThread_Init`. Aqui van **25 pragmas por
las 6 funciones**, siempre acotados con su `off`/`on` alrededor de la funcion.

**Control de validez, primero**: con `-warn pragmas` (que la unidad ya lleva),
`#pragma esto_no_existe` avisa *"illegal #pragma"*. Pasado el control, de los 25
probados **dos son ILEGALES y no cuentan**: `inline_max_size` y
`no_register_coloring`. Los otros 23 son reconocidos.

### Los que MUEVEN el codigo de la unidad

`scheduling`, `peephole`, `opt_common_subs`, `opt_lifetimes`, `global_optimizer`,
`optimization_level`, `use_lmw_stmw`, `opt_propagation`, `opt_dead_assignments`,
`optimize_for_size`, `opt_pointer_analysis`, `optimizewithasm`, `sym`,
`force_active`.

### Los que NO mueven NADA en ninguna de las seis (objeto identico)

`opt_loop_invariants`, `opt_strength_reduction`, `opt_strength_reduction_strict`,
`opt_unroll_loops`, `opt_vectorize_loops`, `opt_dead_code`, `fp_contract`,
`ppc_unroll_speculative`, `opt_unroll_count`, `function_align`.

### Y el resultado: NINGUNO mejora ninguna de las seis

El unico que llega a empatar es `optimization_level 3` (objeto identico a 4, o
sea que el nivel efectivo ya es 3). Lo demas empeora, y varios mucho:

| pragma sobre | resultado |
|---|---|
| `scheduling off` en `SimThread_Init` | 9 -> **50 filas** (la planificacion de MWCC aqui ya es casi la buena) |
| `scheduling off` en `CookValues` | 9 -> 32 |
| `scheduling off` en `HandleTriggers` | 42 -> 52 |
| `scheduling off` en `SimThread_Step` | 68 -> 141 |
| `optimization_level 0/1/2` | siempre mas grande y peor |
| `opt_common_subs off` / `global_optimizer off` | +12/+16 B en cuatro de las seis |

**El ensayo que pedia el encargo, contestado**: el `#pragma scheduling` acotado
por funcion **funciona y NO se derrama** -- con `SimThread_Init` destrozado a 50
filas, `fncmp` sigue dando **30 exactas** y las mismas 3 solo-alias, antes y
despues. Lo que queda cerrado es la DIRECCION: `off` es catastrofico en las
cuatro funciones donde se probo, asi que el eje de planificacion de esta unidad
esta agotado por arriba y por abajo.

### La trampa del `optimize_for_size` (y no es del compilador, es mia)

`#pragma optimize_for_size on` ... `off` alrededor de una funcion **rompe las
otras cinco**. No es que el pragma se derrame: la unidad compila con **`-O4,s`**,
o sea que el estado de reposo es `on`, y el `off` de cierre desactiva el tamano
para todo lo que viene despues. **La restauracion correcta es `on`.** Queda
apuntado porque es el mismo error que haria cualquiera que lea una tabla de
pragmas sin mirar los cflags.

---

## 3. MWCC no tiene pin de registro local (y lo dice el)

Tres sintaxis, compiladas con los cflags reales:

```
register int v : "r6";        -> Error: ';' expected                    (no existe)
register int v asm("r6");     -> Warning: global register variable declarations
register int v __asm__("r6");    are ignored when they occur within a function
```

Es decir: **MWCC parsea la forma pero la ignora dentro de una funcion**; solo la
honra a nivel de fichero, y eso es una variable de registro GLOBAL, que reserva
el registro para toda la unidad. **`nfsmw-pin-de-registro` no se puede transponer
a MWCC.** La palanca equivalente es la de la seccion 1.

---

## 4. Dos afirmaciones de `r60b-auditoria-vedas.md` que son FALSAS

Las dos decian que hay media medida de iter4/iter5 que no esta en el arbol y
contra la que nadie ha vuelto a medir, y las dos mandaban a repetir trabajo
hecho.

### 4a. `Effect_Init`: las dos mitades YA estan combinadas, y combinar es lo mejor

El arbol de hoy (`steering.c:979-1038`) tiene **las dos**: el `clear_value` de
`effect_clear` y el `slot`/`exponent`/`fsubs` de `effect_seed`. Recompiladas hoy
las cuatro combinaciones:

| variante | sha1 `.o` | filas | fuzzy |
|---|---|---:|---:|
| **las dos (lo que hay en el arbol)** | `1962b7d1` | **23** | **95,000 %** |
| solo `clear` | `b4bc4c0a` | 22 | 92,478 % |
| solo `seed` | `8c47f1c4` | 32 | 91,145 % |
| ninguna | `8c54714d` | 31 | 88,623 % |

O sea que **combinarlas ya se hizo y es el optimo de las cuatro**. El 88,62 % que
sale en `estado.py` y en el encargo es **la cifra de `report.json`, que esta
RANCIA**: es el "ninguna" de la ultima fila.

### 4b. `CookValues`: `cook_lo` SI esta en el arbol

`steering.c:469-495` es literalmente el `cook_lo.c` de
`scratchpad/codex_20260908_iter4_steering`. Su cifra real es **99,077 % / 9
filas**, no el 95,77 % del encargo (otra vez `report.json` rancio). Y el
`audit_negative_result.json` de iter4 ya dejaba escrito
`"cook_order_all_function_code_equals_cook_lo": true`, o sea que la otra mitad
tampoco aporta nada.

**Correccion para el proximo**: de las seis, las dos cifras del informe global
que hay que corregir son estas dos. Las otras cuatro coinciden.

---

## 5. `CookValues` (260 B, 99,077 %): el negativo mas fuerte del fichero

Las 9 filas son **una sola transposicion**: el objetivo pone `_mn` en r4 y `_v`
en r6; nosotros al reves. Nada mas. Mismas instrucciones, mismo orden, mismo
tamano.

```
objetivo                      nuestro
lbz   r4, 0x3(r31)   _mn      lbz   r6, 0x3(r31)
extsb r6, r5         _v       extsb r4, r5
extsb r4, r4                  extsb r6, r6
cmpw  r6, r4                  cmpw  r4, r6
mr    r4, r6                  mr    r6, r4
subf  r0, r4, r0              subf  r0, r6, r0
add   r0, r4, r5              add   r0, r6, r5
subf  r0, r0, r6              subf  r0, r0, r4
```

Barrido de hoy, todo con sello de sha1:

- **Las 24 permutaciones del orden de las 4 asignaciones iniciales** (`_v`,
  `_mn`, `_dz`, `_mx` tras `_bits`) dan **UN SOLO objeto**, `1962b7d1`, byte a
  byte. Esto **refuta para este caso** la linea de `nfsmw-catalogo-metrowerks`
  que dice que "el orden de las asignaciones dentro de una rama SI le mueve":
  aqui MWCC lo normaliza entero.
- **13 formas y tipos**: `register` en `_mn`, quitar `register` de `_v`, `long`
  en `_v` y en `_mn`, `_bits` declarado el primero, `_dz` a `int`, `_mn` con
  doble cast, `_v` leido de `raw` en vez de `_bits`. **Ocho dan el objeto
  identico**; las cinco que mueven, empeoran (15, 19, 19, 21 filas y una que
  crece a 268 B).
- **63 subconjuntos de registro bloqueado** (seccion 1): minimo 9 filas.

Y una pista para quien vuelva, que sale de la unica variante que cambio el ORDEN
de reparto: con `asm { li r4, 0 }` MWCC reparte `_mn` **antes** que `_bits`
(carga `cal->min` la primera, en r7). O sea que **el orden de reparto de esta
funcion SI se puede voltear**; lo que no se ha encontrado es como voltearlo
dejando los registros en r4/r5/r6. Ahi esta la funcion, y es la mas barata de
las seis: 260 B a una transposicion.

---

## 6. Lo que NO se ha podido tocar, y por que

- **`Effect_PerformEnvelope` (192 vs 196)**: le falta el `mr r5, r9`, o sea que
  el objetivo tiene `out` y `mag` en registros distintos y MWCC nos los funde.
  Los 12 registros bloqueados **no llegan nunca a 196 B**. Sigue FIRME.
- **`SimThread_Step` (920 vs 924)**: igual, 12 guardas y ninguna llega a 924.
- **`SimThread_Init`**: sus 9 filas son 2 de ranura (el `lfs` del bucle de `ia[]`)
  y un empate desnudo r5/r6. `scheduling off` -> 50 filas; los 12 registros
  bloqueados, todos peores.
- **`HandleTriggers`**: 42 filas de reparto; 12 guardas y 23 pragmas, todos
  neutros o peores.

---

## 7. La `.bss` a8: la propuesta de r62 sigue viva y RECONFIRMADA hoy

Medida de hoy, con el `.o` limpio (`1962b7d1`) y enlazando el proyecto entero
(`scratchpad/mw63/bsstest.py`, copia de la de r62):

```
nuestro .o tal cual (.bss 40 B a8) ....... .text -8  bss +32
el MISMO .o con sh_addralign de .bss a 4 . .text -8
```

Identico a lo que midio r62, y `linkdelta.py --all` con el arbol quieto tambien
da `LibSN/steering -8 bss+32`. **La propuesta de `docs/analisis/r62-mw.md`
seccion 1 sigue en pie sin cambios** (`tools/set_section_align.py`, la regla
`mwcc_sjis_align` de `tools/project.py` y el `section_align=".bss=4"` en
`configure.py:1422`). No la repito aqui para que no haya dos versiones.

**Pero ojo con el orden de los trabajos**: aplicarla deja la unidad a UNA sola
causa de colocacion, los -8 B de `SimThread_Step` y `Effect_PerformEnvelope`.
Y esos -8 B son de las dos funciones que hoy estan mas lejos. Es decir: **el
parche de alineacion no promociona nada por si solo**, y solo tiene sentido
hacerlo cuando esas dos esten cerradas.

**Huecos `pad_` que crearia: ninguno.** No cambia tamanos ni offsets.

---

## 8. Un aviso de metodo que me costo dos medidas

`linkdelta.py --all` tarda ~3 min. Lo lance dos veces sobre un `.o` que no era
el del arbol y salio **`LibSN/steering -12 bss+32`**, que no es el numero de
nadie:

1. la primera vez, porque estaba corriendo **a la vez** que un barrido que
   recompilaba `build/GOWE69/src/LibSN/steering.o`;
2. la segunda, porque mis barridos hacen `reset()` **de la FUENTE** al acabar
   pero **no recompilan**, asi que el `.o` que queda en disco es el de la ultima
   variante, no el del arbol.

Con el arbol quieto y el `.o` recompilado: **`-8 bss+32`**.

Es `nfsmw-build-direct-miente` pero al reves --no miente el compilador, miento yo
al medir-- y no esta escrito en ningun sitio. Dos reglas:
**(a) ninguna herramienta que ENLACE puede correr a la vez que un barrido que
compila**, y **(b) todo barrido tiene que terminar recompilando el arbol y
sellando el sha1**, no solo restaurando la fuente. Los `.o` son estado global
compartido entre agentes.

---

## 9. Propuestas para el jefe

**Ninguna linea de `configure.py` / `keep.lst` / `splits.txt` / `symbols.txt`
esta cambiada ni hace falta cambiarla en esta ronda.** La unica propuesta viva
es la de la seccion 7, que es la de r62 sin tocar y que **no conviene aplicar
todavia** (seccion 7, ultimo parrafo).

**Y una que no toca configure sino la memoria**: `memory/nfsmw-catalogo-metrowerks`
dice hoy dos cosas que hay que corregir con lo medido aqui -- (a) le falta la
palanca de la seccion 1, que es la unica que mueve el reparto de MWCC de forma
dirigida, y (b) su linea "el orden de las asignaciones dentro de una rama SI le
mueve" queda REFUTADA para `CookValues` (24 permutaciones, un solo objeto). No la
he tocado yo: la dejo propuesta.

`lcfix.py` no se ha corrido y no hace falta: no se ha metido ni un literal nuevo.
El `.rodata`, `.sdata2` y `.data` de la unidad salen identicos al empezar y al
acabar, y el `.o` tiene el mismo sha1, asi que **no dejo nada venenoso**.

## 10. Ficheros

- `docs/analisis/r63-mw.md` (este)
- `src/LibSN/steering.c` -- **solo comentarios**, con los negativos junto a cada
  funcion para que `previo.py` los encuentre. Verificado: el `.o` sale con el
  mismo sha1 (`1962b7d1`) que antes de escribirlos.
- `scratchpad/mw63/` -- arnes (`m.py`, `edit.py`), barridos (`sweep2.py`,
  `cook.py`..`cook4.py`, `guard.py`, `eff.py`) y `bsstest.py`.
