# r65 — lote `ecs`: retirada de andamios en eLightE / EmitterSystem / CARSFX_PreColWoosh

    Veredicto: PARCIAL en conocimiento, NO en retiradas.
    16 andamios reales antes, 16 despues. 0 retirados, 8 retiradas intentadas
    (16 compilaciones de unidad), 16/16 irreducibles con diagnostico medido.
    Dos creencias del proyecto CORREGIDAS, una de ellas la premisa de este brief.

## 1. El censo, contado limpio

El brief daba **14** andamios en el lote. El censo real de los tres ficheros es
**16**: el barrido del censo busca `__asm__("" ...)` y **no ve dos barreras
escritas `asm("" ...)`**, sin los guiones bajos, dentro de `EmitterSystem.cpp`.

| fichero | barreras | pines | total |
|---|---:|---:|---:|
| `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp` | 4 | 1 | 5 |
| `src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp` | 4 (2 sin ver) | 3 | 7 |
| `src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.cpp` | 3 | 1 | 4 |
| | **11** | **5** | **16** |

Barrido que los encuentra todos (y que no cuenta los de los comentarios):

    grep -nE '^\s*(__asm__|asm)\("" *[:)]|^\s*register [A-Za-z_].* asm\("' <fichero>

**Andamios despues de la ronda: 16.** Ninguna retirada pasa el test.

## 2. El test y los sellos

Digests **por seccion ALLOC** (`.text`, `.rodata`, `.data`, `.ctors` y sus
`.rela.*`; sin `.line`, `.debug*`, `.comment`, `.stab*`). `.sdata`/`.sdata2` no
existen en estas dos unidades.

Base, reproducida **dos veces** antes de tocar nada (compilador determinista) y
otra vez al cerrar:

| seccion | zEcstasy.o | zEAXSound2.o |
|---|---|---|
| `.text` | 153320 `9a5c5191aac5` | 182356 `46501ac84a13` |
| `.rodata` | 8424 `dabe8f8c5226` | 18296 `8e066de6237e` |
| `.data` | 4152 `b49c6d74b962` | 10300 `ec57277b13a7` |
| `.rela.text` | 122088 `7d58f8c53cd0` | 140412 `6899ddf87155` |
| `.rela.rodata` | 72 `70b381a3a237` | 12504 `2d7f721f6180` |
| `.rela.data` | 396 `651d858c4fb6` | 2676 `e35ed0f7a233` |
| `.ctors` / `.rela.ctors` | 4 `9069ca78e745` / 12 `fbeb71aadc0c` | 4 `9069ca78e745` / 12 `7893b45122e0` |
| `.bss` | 82136 (NOBITS) | 3656 (NOBITS) |

**Al cerrar la ronda las 18 filas son IDENTICAS a la base.** Lo unico que ha
cambiado en el arbol son **124 lineas de comentario** y **cero lineas de codigo**
(comprobado con `git diff -U0` filtrando comentarios y lineas en blanco: sale
vacio).

## 3. Las retiradas intentadas, una por una

### 3.1 `CARSFX_PreColWoosh::MsgBarrier` — 3 barreras + 1 pin. IRREDUCIBLE, y demostrado

`regmap zEAXSound2 "CARSFX_PreColWoosh::MsgBarrier"` da **ESTRUCTURA**, pero al
reves de lo habitual: **el original no tiene NI UNA local** (0 bloques anonimos;
nuestras `fadeOut` e `interpolationTime` salen «SOLO NUESTRA»). El objetivo son
tres sentencias a pelo. Escritas a pelo:

| forma | fn | `.text` de zEAXSound2 |
|---|---|---|
| los cuatro andamios (base) | 100 % | `46501ac84a13` |
| los cuatro fuera | 94,28571 % | `ad8869e9a825` |
| `int interpolationTime = 1;` antes del store | 94,28571 % | — |
| solo el pin `asm("r4")`, sin barreras | 94,28571 % | — |
| pin + su barrera, sin las otras dos | 90,85714 % | `bf3e08a7ad06` |
| sin `fadeOut`, con las otras tres | 92,57143 % | `82e28816f565` |
| los cuatro, pero el pin degradado a `int` | != base | `e559a5ad88ae` |

Sin andamios la **unica** diferencia es una transposicion:

    objetivo   addi r3,r10,0x3c | li r4,0x1 | stfs f13,0x34(r10) | fmr f2,f1
    nuestro    addi r3,r10,0x3c | stfs f13,0x34(r10) | li r4,0x1 | fmr f2,f1

**Por que no sale, leido en el `.sched`/`.sched2`** (`rtldump.py zEAXSound2
MsgBarrier -dS -dR --extra "-fsched-verbose-5"`, bloque 4; **las dos pasadas dan
el mismo horario**):

    insn  prio cost unidad   depende de
     101    3    2   lsu     121      <- [%10+0x34]=%13   (el stfs)
     117    3    1   iu2     121      <- %4=0x1           (el li r4)
     121    2    1   bpu     127      <- la llamada
    t=3  ready: 119 117 101  -> emite 101 y luego 117

Los dos se emiten **en el mismo ciclo 3** y con **el mismo prio 3**, asi que
`rank_for_schedule` empata en los cinco primeros niveles y decide el **sexto,
`INSN_LUID`**: gana el LUID menor, y el nuestro lo tiene el `stfs`.

Y el prio empata porque `insn_cost` **no devuelve 0 para la anti-dependencia**
almacenamiento -> llamada: `flush_pending_lists` la crea como `REG_DEP_ANTI`,
pero en `insn_cost` esa rama cae en `LINK_COST_FREE(link)` y **`cost = 1`**
(`haifa-sched.c`, y el clamp `if (ncost <= 1) LINK_COST_FREE(link) = ncost = 1`
al final de `ADJUST_COST`). O sea: **una anti-dependencia nunca vale 0 aqui**.
Eso cierra la via de «bajarle el prio al store».

Y la unica via que queda —que el `1` se materialice antes de la sentencia
`mDurationActive = 0.0f`— **no la da ninguna forma de estas tres sentencias**:
la carga de un parametro en registro duro la emite `load_register_parameters`
siempre pegada a la llamada, y poner el almacenamiento *detras* de la llamada no
vale porque `sched_analyze_1` le cuelga una `REG_DEP_ANTI` de
`last_pending_memory_flush` y no puede subir.

**NO REINTENTAR** sin una palanca nueva que adelante una carga de parametro en
registro duro. Diagnostico escrito entero encima de la funcion.

### 3.2 `EmitterSystem::Render` — 4 barreras + 3 pines. IRREDUCIBLES, revalidados

Revalidacion completa contra el arbol de hoy (la regla «los andamios caducan»
obliga a repetirla; aqui **no** han caducado):

| retirada | `.text` de zEcstasy | nota |
|---|---|---|
| base | 153320 `9a5c5191aac5` | 100 % |
| barrera final `"+r"(num_textures) : "r"(e36guard)` | 153312 `d944805d4692` | **-8 B**, se va el marco del derrame |
| el fantasma (pin `r19` + su `"=r"`) | 153312 `d944805d4692` | **el mismo objeto**: es UNA palanca, no dos |
| pin `r24` de `sprite_hack_flags` | 153320 `d9647bfb4d49` | mismo tamano, `.rela.text` identica: rotacion pura |
| pin `fr6` de `world_size` | 153320 `ea5b8417499a` | idem |
| `asm("" : : "f"(world_size))` | 153320 `53039be46e7b` | 9 filas, 99,655174 % |
| `asm("" : "+m"(xbasis), "+r"(sprite_hack_flags))` | 153320 `53039be46e7b` | **el mismo objeto** que la anterior |

Dos cosas nuevas:

1. **Las dos barreras de ranura son UNA sola palanca.** Quitar cualquiera de las
   dos da el **mismo objeto** (9 filas). La cifra de la r36f («sin el
   `"+m"(xbasis)` 13, sin el `"+r"` 4») **ya no vale**.
2. **`world_size` esta en `f6` EN EL ORIGINAL** (DWARF, via `regmap`). El pin
   `asm("fr6")` no inventa un reparto: reproduce el verdadero. Sigue siendo un
   andamio, pero no es una mentira sobre el reparto.

`regmap` sobre `Render` confirma lo de la r23: 38 locales con el mismo registro,
**0 solo del original**, y la unica «solo nuestra» es `e36guard`, que *es* el
fantasma. Al objetivo **no le falta ninguna variable de fuente**: el 19.o rango
de vida es un temporal del compilador.

### 3.3 `eLightMaterialPlatInterface::UpdatePlatInfo` — 4 barreras + 1 pin. IRREDUCIBLES

**Base real: 3 filas, 99,96086 %, 2.044 B** — no las 28 del brief. Las 28 son de
antes de la r61-ecs, que ya aplico las dos piezas de la r56.

| retirada | fn | filas |
|---|---|---|
| base | 99,96086 % | 3 |
| pin `envmap_min_scale asm("fr5")` | 99,37378 % | 48 |
| pin fuera **y** `envmap_min_b` sin conmutar | 99,37378 % | 48 |
| los tres `"+f"(envmap_power)` | 99,872795 % | 12 |
| el `__asm__("")` del `case 0x68E97F75` | 99,540115 % | 8 |

Las tres filas que quedan no son de andamio: `fmuls f26,f5,f10` contra
`f26,f10,f5` (el precio de la conmutacion) y el par `lis`/`lfs` del 6.0f con
`r11` contra `r9`.

Dato lateral, medido: el pin fuera **con y sin** la conmutacion de
`envmap_min_b` da **el mismo objeto**. O sea que la conmutacion de la r61 no es
una infidelidad independiente: solo tiene efecto **mientras el pin exista**, que
es exactamente el mecanismo de `set_preference` que la r61 documento.

## 4. Las DOS correcciones

### 4.1 La premisa del brief es falsa: el pin `fr5` **no** bloquea el reparto

El brief dice que el original mete dos locales en `f5` y que «un `register asm`
no puede compartir registro por construccion», y que por eso ese pin
**bloquea** el reparto correcto.

**Medido, con el pin PUESTO**, `regmap zEcstasy
"eLightMaterialPlatInterface::UpdatePlatInfo" --all` dice:

    MISMO conjunto de locales, mismo arbol de bloques y MISMO REPARTO.
    (54 iguales, 0 con registro distinto, 0 de ambito equivocado,
     0 solo del original, 0 solo nuestras)

y ahi estan **las dos en f5**: `envmap_min_scale` f5/f5 y `diffuse_rng_a` f5/f5.
O sea que **un `register T x asm("frN")` no impide que otro pseudo con vida
disjunta reciba ese mismo registro**: fija la preferencia, y aqui la fija en la
buena. Sin el pin, `regmap` da **PERMUTACION**: un ciclo limpio de cuatro,
`f7 -> f6 -> f5 -> f8 -> f7` (`diffuse_min_a`, `diffuse_max_scale`,
`diffuse_rng_a`, `specular_min_scale`), con **0 locales de mas o de menos por
ninguno de los dos lados**. Veredicto PERMUTACION = «puede que el pin sea la
unica herramienta». Lo es.

Corolario util: **no hay que retirar un `register asm` solo porque el original
comparta ese registro entre dos vidas disjuntas.** Comprobarlo con `regmap`
primero.

### 4.2 Las lineas en blanco y los numeros de linea NO son una palanca

El arbol tiene sitios donde se rellena con lineas en blanco para cuadrar la
numeracion con el original. Para **codigo** eso es cosmetica, y esta medido en el
fuente de GCC:

* `sched_init` (`orig/prodg/NGC_GNU_SRC/NGC/gcc/haifa-sched.c:8566`) numera los
  `INSN_LUID` recorriendo **todos** los insn, **incluidas las `NOTE`**. Una
  `NOTE_INSN_LINE` de mas desplaza todos los LUID por igual, asi que el **signo**
  de `INSN_LUID(a) - INSN_LUID(b)` —lo unico que usa `rank_for_schedule`— no
  cambia.
* `local_alloc` solo cuenta los insn de clase `'i'`.

Sirve para `.line`/`.debug` (fidelidad del volcado) y para nada mas.

## 5. La otra herramienta que esta ronda ha estrenado: el mapa de lineas contra un `switch`

`symbols/debug_lines.txt` da la linea de fuente del **original** por direccion.
Cruzandolo con el listado del objetivo (el lado `left` del JSON de `objdiff`) se
puede contar, **case por case**, cuantas sentencias tiene el objetivo. En
`UpdatePlatInfo` (`0x80109BB8`, `0x7FC`):

| case | lineas del objetivo | sentencias | las nuestras |
|---|---|---:|---:|
| `default` | 180, 181 + 182 | 2 | 2 |
| `0x33A26CB6` | 187 + 188 | **1** | 1 |
| `0xA6348EE3` | 193, 194 + 195 | 2 | 2 |
| `0x12C9453C` | 201, 202 + 203 | 2 | 2 |
| `0x471A1DCA` | 209, 217 + 221 | 2 lineas / 3 sentencias | 3 |
| `0x89946400` | 225, 234 + 236 | 2 lineas / 3 sentencias | 3 |
| `0x8812634B` | 244, 249 + 251 | 2 lineas / 3 sentencias | 3 + 1 muerta |
| `0x68E97F75` | 267, 277, 278 + 282 | **3** | 3 |

Con esto queda **cerrada en negativo** la veda «a los cases les faltan
sentencias»: en `0x33A26CB6` el objetivo tiene **una sola** sentencia
(`lis`+`lfs`+`fmuls` en la linea 187) y **ninguna entrada de linea extra** en
`0x8010a020` —y las entradas duplicadas en la misma direccion SI se conservan en
este volcado (`0x80109ffc` trae 176 y 180)—, asi que los tres `asm` de ahi no
tapan ninguna sentencia perdida. Igual en `0x68E97F75`: 267, 277 y 278 son
nuestras tres, con nueve lineas en blanco entre la primera y la segunda.

De paso, dos detalles de **formato** del original, gratis: en
`0x471A1DCA`/`0x89946400`/`0x8812634B` mete `diffuse_min_a *= 0.25f;` y
`diffuse_rng_a = ...` en **una misma linea** (217/234/249); y el prologo de la
funcion es `3 + 10 + 10 + 10` sentencias con lineas en blanco en 137, 148 y 159,
exactamente la forma que ya tiene nuestro fuente.

## 6. Regresiones y revertidos

* **Regresiones: ninguna.** Los 18 digests ALLOC de las dos unidades cierran
  identicos a la base.
* **Revertido: TODO.** 8 retiradas probadas (16 compilaciones de unidad) y las 8
  revertidas en el acto por cambiar `.text`.
* `build_direct.py` leyendo siempre la ultima linea: las 16 dieron
  `N ok, 0 fallidas`.
* No se ha tocado `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.
  No se ha corrido `lcfix.py`. No hay commit. No se han tocado los bloques
  `#if defined(__ANDROID__)`.

## 7. Lo que deja esta ronda para la siguiente

1. Los 16 andamios del lote llevan **su diagnostico escrito al lado**, con
   digests, para que `previo.py` los encuentre. Ninguno se debe reintentar a
   ciegas.
2. El censo de andamios **debe buscar `asm("" ...)` ademas de `__asm__("" ...)`**:
   hay al menos 2 invisibles y probablemente mas en el arbol.
3. La palanca que falta para `MsgBarrier`, y que no existe en el catalogo:
   **adelantar la carga de un parametro en registro duro** por delante de la
   sentencia anterior. Es un empate de `INSN_LUID` puro.
4. La palanca que falta para `Render`: **fabricar un temporal de compilador de
   mas** sin emitir codigo (hoy lo hace el fantasma `r19`).
