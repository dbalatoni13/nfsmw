# r53-ecs — zEcstasy: `GenerateHorizonFogDisplayList` CERRADA (796 B)

| función | B | filas antes | filas ahora | % |
|---|---:|---:|---:|---:|
| **`GenerateHorizonFogDisplayList`** | **796** | **2** | **0** | **100 %** |
| `epCalculateLocalDirectionalPOS16` | 2.072 | 155 | 155 | 93,305016 |
| `UpdatePlatInfo` | 2.044 | 28 | 28 | 99,33464 |
| `eProject` | 268 | 14 | 14 | 93,970146 |

`fncmp`: **4 funciones / 5.180 B → 3 funciones / 4.384 B**. Único fichero tocado:
`src/Speed/GameCube/Src/Ecstasy/EcstasyEx.cpp`. `linkdelta` **idéntico antes y
después** (`.text +0 rodata−288 data+64 bss+32`): el cambio no mueve una sección.

---

## 1. `fog`: la veda era real, pero de la palanca equivocada

La función llevaba **ocho rondas** (r24, r29, r30, r36b/c/f, r46, r51, r52) con la
misma sentencia: «es un desempate de `rank_for_schedule` y `prio(216)=12` contra
`prio(231)=2` lo decide antes que nada». Es cierto, y esta ronda lo convierte en
**teorema**; lo que estaba mal era suponer que por eso la función estaba cerrada.

### 1.1 El teorema (por qué la prioridad NO se puede tocar)

Las dos cadenas mueren en el **mismo** insn, el `beq` (232) que cierra el bloque:

    216 srawi -> 703 xoris -> 705 stw -> 706 stw -> 707 lfd -> 708 fsub -> 226 frsp -> 227 fmuls -> 232
    231 andi. -> 232

Como `INSN_PRIORITY` es «coste propio + máximo de los dependientes», los dos
sumandos comparten el término `prio(232)`:

> **prio(216) − prio(231) = 10, sea cual sea la forma de la región.**

Da igual si el `beq` gana dependientes (planificación interbloque, otra región,
otro reparto): los dos suben lo mismo. **Ninguna forma de fuente puede invertir
ese desempate.** Ocho rondas de barrido de formas estaban condenadas de partida.

### 1.2 La salida verdadera, leída en la traza ciclo a ciclo

`rtldump.py --file .../EcstasyEx.cpp --like zEcstasy GenerateHorizonFog -dR
--extra "-fsched-verbose-5"` da el bloque básico 2 entero:

    t=1   212 srwi (prio 14, iu2)   +   704 (prio 11, iu2)
    t=2   214 add  (prio 13, iu2)   +   696 fmr (prio 2, fpu)
    t=3   216 srawi (prio 12)       +   231 andi. (prio 2)

231 **está listo desde t=1** y pierde las dos ranuras de t=1 y las de t=2 contra
insns de prioridad 14/11/13 (y el empate a 2 con 696 lo pierde por el último
desempate). En t=3 ya está 216 y gana. La única forma de que 231 salga antes es
que **216 no esté listo en t=3**, o sea **inyectarle una dependencia más**.

Y ahí está el hallazgo de método:

> **Para colgar una barrera de un valor intermedio hay que DARLE NOMBRE.**
> `i / 2` son tres insns (`srwi`, `add`, `srawi`) y **el resultado del `add` no
> existe en C**. Reescrita la división como sus dos mitades, el intermedio pasa a
> ser una variable y admite `__asm__("" : "+r"(x))`, que no emite un byte.

```c
int half = i + (int)(((unsigned int)i) >> 31);
__asm__("" : "+r"(half));
multiple = half >> 1;
```

| forma | % | filas |
|---|---:|---:|
| `multiple = i / 2;` (base de ocho rondas) | 98,995 | 2 |
| `half` escrito a mano, **sin** el `asm` | 98,945 | 3 |
| `half` + `asm`, con `(i>>31) + i` | 99,950 | 1 |
| **`half` + `asm`, con `i + (i>>31)`** | **100** | **0** |
| copia intermedia sin `asm` | 98,995 | 2 |
| una sola expresión | 98,995 | 2 |
| `half` + clobber de r0 en vez de `"+r"` | 97,563 | 9 |

La última fila era **el orden del `add`**: el objetivo emite `add r0,r31,r0`, o
sea `i + (i>>31)`. Escrito al revés sale `add r0,r0,r31`.

**Por qué la r36f no lo encontró**, teniendo la palanca correcta anotada
(«inyectar una dependencia para retrasar un ciclo»): la aplicó sobre variables
que **sí** tienen nombre —`current_index`, `color_base`, `verts_per_strip`,
`multiple`— y todas o rompen el reparto (79-84 filas) o las come el DCE. El
operando que hacía falta era justo el que no tenía nombre.

### 1.3 El `asm` se queda, y con qué molde hay que sustituirlo

Son **796 B con él y 0 sin él**, así que no es la «deuda de cero bytes» que
prohíbe el brief. Pero no es la fuente original. El molde a buscar es:

> una insn que **emite 0 B**, **gasta ranura de emisión** y **está en la cadena
> de dependencias**.

Y hay un **ejemplar natural, en este mismo bloque**: la insn **704**, que es
`*floatsidf2_loadaddr` — `(set (reg:SI 9) (unspec [(const_int 0)] 11))`, el
«carga la dirección del hueco» de la conversión `int -> double`. **No emite ni un
byte** (el bloque tiene 14 insns de RTL y 13 de máquina) y ocupa una ranura iu2
en t=1. Es exactamente lo que la r52 §2.3 pedía para el bloque de literales de
`UpdatePlatInfo`, y ahora se sabe que existe y de dónde sale: **de una conversión
de entero a flotante**.

---

## 2. `UpdatePlatInfo`: el requisito, afilado — y dos palancas más caídas

No cierra. Pero el diagnóstico de la r52 queda más estrecho y con dos negativos
nuevos que ahorran rondas.

### 2.1 CORRECCIÓN: la exclusión de f8 no es una preferencia, es un CONFLICTO

La r48 escribió que `115` (`diffuse_rng_a`) «pierde f8 **y** f5» por
`regs_someone_prefers`. Sobre f8 es falso, y verlo cambia el requisito.
En el objetivo, `envmap_max_scale` vive en f8 desde `80109CF4` y su último uso es
`fmuls f8,f8,f0` en `80109D58`; `115` nace en `80109D54` (`fsubs f5,f24,f7`).
**Se solapan**: f8 está en `hard_reg_conflicts[115]` de verdad, sin preferencia
que valga. En cambio f5 (`envmap_min_scale`) muere en `80109D40`, catorce bytes
antes de que nazca `115`, y por eso el objetivo se lo puede dar.

O sea que el original necesita, a la vez:

* `envmap_max_scale` **local** y renumerado a f8 (para que `205` conserve su
  preferencia y f8 quede fuera por conflicto real), y
* `envmap_min_scale` **en f5 pero NO renumerado** cuando corre `global_conflicts`.

Las dos escalas son estructuralmente distintas en la fuente original. Eso
descarta de un plumazo toda la familia «son la misma variable reutilizada»: un
pseudo global recibe **un** registro para todo su rango (aunque sea discontinuo),
y f5 ≠ f8.

### 2.2 NEGATIVO NUEVO: la palanca de orden de allocnos tampoco sirve

La receta corregida de la r52 (subir `n_refs` metiendo la variable como entrada
`"f"(x)` en un `asm` que ya existe) **funciona sobre el orden** y no emite un
byte. Barrido con `.greg` (1,2 s por medida), añadiendo referencias de
`envmap_min_b` a los tres `__asm__` del `case 0x33A26CB6`:

| refs extra | pos(115) | pos(194) | 115 acaba en |
|---:|---:|---:|---|
| 0 (base) | 99 | 115 | f1 |
| +1 | 99 | 110 | f1 |
| +3 | 99 | 103 | f1 |
| **+4** | 100 | **99** | f1 |
| +6 / +9 / +12 / +18 | 100 | 96 / 92 / 87 / 86 | f1 |

Con +4 el `194` **adelanta** al `115` y `prune_preferences` ya no le mete f5 en
`regs_someone_prefers`… y da igual: ahora `find_reg` procesa antes al `194`, que
**se lleva f5 por su preferencia**, y `115` sigue cayendo en f1. Las dos ramas
del mecanismo acaban en el mismo sitio.

> **Veda:** mover el orden de allocnos no vale. Mientras exista
> `;; 194 preferences: 37`, `115` no puede coger f5 — la coja `194` o se la
> vede `regs_someone_prefers`.

### 2.3 Más formas caídas (todas medidas con build, 18 s)

| forma | filas | nota |
|---|---:|---|
| sin pin + `asm("" : : "f"(min_scale))` en el `default` (global) | — | `p194` desaparece pero f5 queda reservado todo el `switch` |
| **con** pin + el mismo `asm` | — | `p194` desaparece, 115 → f7 |
| `register float rng_a_hard asm("fr5") = DiffuseMaxA − min_a; rng = rng_a_hard;` | 28 | objeto **idéntico** a la base: GCC absorbe el duro |
| copia `t` en `fr5` dentro del `case 0xA6348EE3` | 28 | idéntico |
| copia `ems = envmap_min_scale` para los tres productos | 60 | equivale a quitar el pin |
| copia `ems` sólo para `min_b` | 28 | idéntico; la copia se pliega |
| `asm("" : "+f"(envmap_min_scale))` detrás de `min_b` | 28 | idéntico |

### 2.4 Lo que queda en pie

El requisito no ha cambiado, sólo se ha estrechado: **hay que encontrar la
sentencia del original que hace de `envmap_min_scale` un allocno GLOBAL** (o que
hace que `local_alloc` no lo coloque), sin tocar `envmap_max_scale`. El mapa de
líneas deja el hueco donde buscarla: entre `envmap_max_b` (línea 169 del
original) y el `switch` (175) hay **cinco líneas sin código**, y nosotros
tenemos seis; y entre `default:` (176) y el primer `*= 0.1f` (180) el original
tiene **tres** y nosotros dos. Aviso: el bloque de literales del
`case 0x68E97F75` **ya está alineado al dedillo** con el original (267 →
9 líneas → 277, 278 → 3 líneas → 282), así que la corrección de la r52 §2.5 se
confirma y no hay sentencia que buscar *dentro* de ese `case`.

---

## 3. `eProject`: la palanca nueva no transfiere

El diagnóstico de la r49 («el objetivo atrasa la cadena de la línea 244 —`clipX`—
y adelanta la de la 254») encaja con la palanca que cerró `fog`, así que la
apliqué dándole nombre al intermedio del `fmadds`:

| forma | % | filas |
|---|---:|---:|
| base | 93,970 | 14 |
| `float t2 = eye.z*pm[2]; asm("+f"(t2)); clipX = eye.x*pm[1] + t2;` | 80,030 | 29 |
| `float t1 = eye.x*pm[1]; asm("+f"(t1)); clipX = t1 + eye.z*pm[2];` | 82,388 | 29 |
| `float ex = eye.x; asm("+f"(ex)); clipX = ex*pm[1] + …` | 89,507 | 18 |

La diferencia con `fog` es que allí el intermedio **no tenía registro asignado**
(es el resultado del `add`, un temporal puro) y aquí `t1`/`t2` son productos
flotantes de vida larga: la barrera les cambia el reparto además del horario.
**Veda:** dar nombre al intermedio sólo paga cuando el intermedio es un temporal
sin reparto propio.

---

## 4. Andamios re-medidos (los tres siguen pagando)

Regla «los andamios caducan», comprobada con build:

| andamio | con | sin |
|---|---:|---:|
| `register float envmap_min_scale asm("fr5")` | 28 filas | **60** |
| los tres `__asm__("" : "+f"(envmap_power))` del `case 0x33A26CB6` | 28 | **45** |
| `__asm__("" : "+f"(halfVP2))` de `eProject` | 14 | **23** |

Ninguno ha caducado. El nuevo de `fog` (`"+r"(half))` vale **2 filas → 0**.

---

## 5. Método

* **`-fsched-verbose-5` sobre la unidad entera con `rtldump.py`** (1,2 s el
  `.greg`; ~15 s el `.sched2` de `EcstasyEx.cpp`) da la traza ciclo a ciclo con
  `prio`, unidad y lista de listos. **No hace falta mini-TU.**
* **Cuenta las insns de RTL del bloque contra las del `.o`.** La diferencia son
  las insns de cero bytes, que es lo que ninguna herramienta del árbol enseña.
  Aquí valió el hallazgo entero.
* **Antes de barrer formas, resta las dos prioridades.** Si las dos cadenas
  mueren en el mismo insn, la diferencia es constante y el barrido no puede
  ganar: el frente es el HORARIO (dependencias), no la prioridad.
* Sonda de `.greg` con parche textual (`prefs`, `Register dispositions`,
  posición en `regs to allocate`): 1,2 s por medida. **Ojo**: los números de
  pseudo se desplazan en cuanto cambia la fuente, así que sólo es comparable con
  parches que no añaden variables — si no, hay que medir con `build` + `fndiff`.
* Volcados RTL borrados de `scratchpad/rtl/` (los ocho eran míos; el directorio
  queda vacío). Nada nuevo en `scripts/`.
