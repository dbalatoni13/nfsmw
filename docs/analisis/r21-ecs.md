# Ronda 21 — zEcstasy: convertir muros en mecanismos

Estado medido al empezar y al acabar (`build_direct.py zEcstasy` antes de cada
medida, siempre):

    antes    136.340 / 145.884 B   93,4578 %   532 funciones al 100 %
    despues  136.340 / 145.884 B   93,4578 %   532 funciones al 100 %
    measure.py --cmp antes_r21_ecs.json despues_r21_ecs.json
      -> +0 B, +0 funciones, 0 unidades cambian

**Cero bytes cerrados.** Lo que traigo son tres mecanismos con su pase del
compilador identificado y sus números, y siete ensayos con cifra.

## 0. El encargo está VIGENTE (verificado, no heredado)

`triage.py zEcstasy` sobre objetos recién construidos reproduce el brief exacto:

    696 B   98,075 %  Render__13EmitterSystemP5eView   faltan 2, sobran 0   lwz+1, stw+1
    MURO: 6 funciones, 8.848 B

    2072 B  93,305 %  epCalculateLocalDirectionalPOS16   reorden local, dmax 20
    2044 B  99,235 %  UpdatePlatInfo                     64 registros
    1848 B  98,766 %  eProgressiveScan_EURGB60DialogBox  94 registros
    1820 B  99,956 %  UpdateParticles                    4 registros
     796 B  98,995 %  GenerateHorizonFogDisplayList      reorden local, dmax 2
     268 B  88,851 %  eProject                           reorden local, dmax 5

696 + 8.848 = 9.544 B. Coincide con el papel.

## 1. `audit.py` — UN fallo, confirmado en segunda pasada

    __static_initialization_and_destruction_0
      FALLA: reubicacion en un solo lado: lis r11, 0x803e | lis r11, $LC981@ha

Dos pasadas idénticas (no es fantasma del `build/` compartido). Es un caso real
de «el 100 % que miente»: la función sale al **100 %, 4.544 B** en `loss.py`
porque `function_reloc_diffs=none` esconde a qué símbolo apunta cada `@ha/@l`.

Lo que hace el objetivo en `80115968..801159F0`:

    lis  r11, 0x803e        <- @ha sin reubicacion en el troceado
    mtlr r11                <- aparca la mitad alta en LR (spill de GPR de GCC)
    ...
    mflr r7
    stfs f0, LightColourFE2@l(r7)

O sea, **el objetivo aparca `LightColourFE2@ha` en LR y nosotros aparcamos el
`@ha` de una entrada del pool de literales** (`$LC981`). Las dos versiones son
autoconsistentes (cada una usa su propio `@l`), así que puede ser sólo un
reparto distinto de qué base va a LR — pero **hay que comprobarlo pareja a
pareja**, porque si no lo es, 4.544 B están contados como casados sin serlo.
No lo he tocado: está fuera de mi encargo y cuesta 0 B mover.

Nota: `frozen.py chk Speed/Indep/SourceLists/zEcstasy` marca **HA CAMBIADO** por
la corrección de la sección 2. Contrastado con `measure --cmp`: **+0 B**. No he
vuelto a congelar; lo dejo a criterio del usuario.

## 2. `UpdatePlatInfo` (2.044 B) — MECANISMO COMPLETO: un truncamiento entero en `allocno_compare`

### 2.1 La fuente estaba permutada, y hay dos pruebas independientes

`lmap.py` da el orden de sentencias del original sin ambigüedad:

    eLightE.cpp:151  specular_min_scale   lfs f8, 0x5c(r3)
    eLightE.cpp:152  specular_min_r       lfs f13,0x60(r3) ; fmuls f20,f8,f13
    eLightE.cpp:153  specular_min_g       lfs f11,0x64(r3) ; fmuls f19,f8,f11
    eLightE.cpp:154  specular_min_b       lfs f0, 0x68(r3) ; fmuls f18,f8,f0
    eLightE.cpp:155  specular_max_scale   lfs f13,0x6c(r3)
    ...
    eLightE.cpp:162  envmap_min_scale     lfs f5, 0x80(r3)
    eLightE.cpp:163  envmap_min_r         fmuls f30,f5,f12
    eLightE.cpp:164  envmap_min_g         fmuls f1, f5,f11
    eLightE.cpp:165  envmap_min_b         fmuls f26,f5,f10
    eLightE.cpp:166  envmap_max_scale     lfs f8, 0x90(r3)
    eLightE.cpp:167  envmap_max_r         fmuls f25,f8,f9
    eLightE.cpp:168  envmap_max_g         fmuls f31,f8,f13
    eLightE.cpp:169  envmap_max_b         fmuls f8, f8,f0

`dwbody.py` da el **orden de declaración del original** y coincide: `…,
specular_min_scale, specular_min_r, specular_min_g, specular_min_b,
specular_max_scale, …` y `envmap_min_r, envmap_min_g, envmap_min_b,
envmap_max_scale, …`, con `specular_min_r // f20`, `min_g // f19`, `min_b // f18`.

Nuestra fuente tenía `min_g, min_b, max_scale, min_r` y `envmap_min_g/b`
declaradas vacías y asignadas **después** del bloque `max`. Eso viene del commit
`ef92ca4e` («reorden … del permutador + split_decl … 0 B, sin cierre»): **el
permutador encontró un óptimo local de +0,05 pp alejándose de la fuente real.**

    c1  orden natural del bloque specular (min_r, min_g, min_b, max_scale)
        -> 99,235 -> 99,182 %   (64 -> 67 diffs)
    c2  c1 + orden natural del bloque envmap (sin las declaraciones partidas)
        -> 98,583 %             (80 diffs)

**c2 SE QUEDA** (corrección estructural real, `measure --cmp` +0 B). Aunque el
porcentaje baja 0,65 pp, el diff cambia de naturaleza: **con c2 todas las
`lfs`/`fmuls` casan fila a fila con el objetivo** (mismos desplazamientos, mismo
orden) y sólo quedan **nombres de registro**. `triage.py` lo reclasifica de
«64 registros» a «reorden local, dmax 2, CERO reales».

### 2.2 El pase que decide, con los números

`global_alloc` reparte los FPR salvados en orden **descendente** (f31, f30, …,
f14) según el orden de `allocno_compare`:

    prioridad = (int) ( floor_log2(n_refs) * n_refs / live_length * 10000 * size )

Los tres pseudos del bloque specular (142 = `min_r`, 145 = `min_g`,
148 = `min_b`) tienen `n_refs = 3` y `size = 1`, luego `pri = int(30000/len)`:

    pseudo  n_refs  live_len  prio  orden  registro
    142     3       442       67    124    f18     <- min_r  (objetivo: f20)
    145     3       441       68    122    f20     <- min_g  (objetivo: f19)
    148     3       440       68    123    f19     <- min_b  (objetivo: f18)

`30000/441 = 68,027 -> 68` y `30000/442 = 67,873 -> 67`. **El corte entero cae
justo entre 441 y 442.** 145 y 148 empatan a 68 y desempatan por número de
allocno; 142 se cae al cubo 67 y se queda el último → recibe f18.

Si los tres cayeran en el mismo cubo empatarían y el desempate por número de
allocno daría 142, 145, 148 → **f20, f19, f18 = el objetivo**.

    cubo 68  <=>  len en [435, 441]
    cubo 67  <=>  len en [442, 447]

luego **quitar 1 a 5 insns RTL de dentro del rango vivo (o añadir 3 a 5) cierra
la función.** El rango vivo va del `fmuls` (antes del switch) al
`plat_info->SpecularMin* = …` (después del switch): son las 440 insns del switch.

### 2.3 Cómo lo comprobé (y un aviso sobre `lreg.py`)

`lreg.py` **descarta `-G0`** (su filtro `KEEP` sólo conserva `-(O|f|m|g|W|std=|
ansi|pedantic)`), y `-G0` es justo la bandera que mueve estas tres longitudes:

    sin -G0 (lo que compila lreg.py)   len 439/438/437  -> los tres a 68 -> f20,f19,f18  (= OBJETIVO)
    con -G0 (el build de verdad)       len 442/441/440  -> 67/68/68      -> f18,f20,f19  (= nuestro)

Es decir: **`lreg.py` reproduce el reparto del objetivo y el build real no.** No
es que `-G0` sobre — el objetivo lo lleva (usa `lis @ha`/`@l` por todas partes);
es que `-G0` mete 3 insns de materialización de direcciones dentro del rango y
eso cruza el corte. Sirve como banco de pruebas, pero **sus registros no son los
del build**: hay que volver a compilar a mano con los cflags completos.

Dejo en el scratchpad `c21ecs_alloc.sh` (repite el preproceso + `cc1plus -dl -dg`
con los cflags reales, `-G0` incluido) y `c21ecs_show2.py` (tabla
pseudo/n_refs/live_len/prio/orden/registro).

**Segundo aviso de herramienta:** la línea `;; N regs to allocate:` del volcado
`.greg` lleva anotaciones `(2)` de tamaño intercaladas; un `[\d\s]+` la corta a
la mitad (58 de 119 en `UpdateParticles`). Hay que tokenizar ignorando los
`(N)`.

### 2.4 Los dos intentos de mover el rango — los dos fallan por lo mismo

    c3  quitar el `asm("" : : "f"(envmap_power))` del case 0x68E97F75 (-1 insn)
        -> 98,280 %.  Revertido. No es -1 limpio: sin la barrera, `envmap_power
        = 6.0f` se pliega dentro de `envmap_magic` y desaparecen varias insns.
    c4  dos `asm("")` justo antes del switch (+2 insns)
        -> 94,211 %.  Revertido. Los `asm` volátiles son barrera de planificación
        y de CSE: mueven media función, no suman 2 insns.

**Veda escrita: el `asm` vacío NO sirve como ±N de `live_length`.** Medido dos
veces (aquí y en `UpdateParticles`, §4). El ±N tiene que salir de una sentencia
de fuente real.

## 3. `epCalculateLocalDirectionalPOS16` (2.072 B) — 8 BYTES DE PILA DE MÁS

`triage.py` lo llama «reorden local, dmax 20, CERO reales» porque **normaliza los
inmediatos: un marco de pila 8 B mayor es invisible para él.**

    objetivo  stwu r1, -0x170(r1)
    nuestro   stwu r1, -0x178(r1)

De las **155 filas de diff, 49 son literalmente el mismo opcode con el
desplazamiento de pila +8** (todo el prólogo/epílogo y los tres derrames). El
mapa de ranuras es idéntico en todo lo demás:

    los dos:  0x8 0xc (float) · 0x10 0x18 0x20 0x28 0x30 0x38 0x40 (psq) ·
              0x4c (float) · 0x50 0x58 0x60 (psq) · 0x6c 0x74 0x7c (float)
    objetivo: 0x88 (temp de 8 B de conversion int<->float) ; CR 0x94 ; stmw 0x98
    nuestro:  0x90 (el mismo temp)                          ; CR 0x9c ; stmw 0xa0

Ni un lado ni el otro tocan `0x80..0x87`: **el objetivo pide dos ranuras de 8 B
alineadas después de `0x7c` (una muerta) y nosotros pedimos tres (dos muertas).**
Sobra exactamente **un `assign_stack_temp` de 8 B cuyas insns se borraron
después** — las dos conversiones vivas (`stw/stw/lfd` para `float t0 = *(int*)pt`
y `fctiwz/stfd/lwz` para `*(int*)pt = (int)t1`, ambas dentro del `my_fpow`
inline) comparten una sola ranura en los dos lados.

Las otras 106 filas son permutación de f2..f13 dentro del bucle interno.

`lmap.py` confirma que el orden de sentencias es el correcto: las diez `V2(...)`
salen en las líneas 4520..4529 del original, una por línea, en nuestro mismo
orden; y el árbol de inlines del objetivo (líneas 2539-2542, 2640-2674,
2706-2713 = `my_fpow3`, `my_fpow`, `fClamp`) es el que tenemos.

**Cero ensayos aquí**: la palanca es encontrar qué construcción pide el tercer
temp de 8 B, y no la he acotado (ver §7).

## 4. `UpdateParticles` (1.820 B, 99,956 %) — el empate está a UNA insn

Las cuatro filas de diff son **una** decisión: el objetivo mete `257.0f` en f21 y
`1/255` en f20; nosotros al revés. El pool está bien
(`lbl_803DF358 = .float 257`, `lbl_803DF35C = .float 0.003921569`, y nuestros
`$LC939 = 0x43808000`, `$LC940 = 0x3b808081`, en el mismo orden), y las dos
cargas salen en el mismo sitio del precabezal.

Aquí el reparto también es de `global_alloc`, y la cadena de FPR salvados sale
**monótona** con la longitud de vida (`n_refs = 7`, `size = 1`, `pri =
int(140000/len)`):

    pseudo  refs  len   prio  orden  reg
    439     7     630   222   91     f21   <- 1/255    (objetivo: f20)
    428     7     632   221   92     f20   <- 257.0f   (objetivo: f21)
    399     7     636   220   93     f19
    356     7     644   217   95     f18
    334     7     652   214   98     f17
    278     7     656   213   99     f16
    257     7     660   212   101    f15

`140000/630 = 222,2 -> 222` y `140000/632 = 221,5 -> 221`: **otra vez el corte
entero, esta vez entre 630 y 631.** Si los dos cayeran en el mismo cubo
empatarían y ganaría el allocno menor (428 = `257.0f`) → **f21 para 257.0f = el
objetivo**.

    cubo 222 <=> len en [628, 630]
    cubo 221 <=> len en [631, 633]

luego **+1 insn (633/631) o −2 insns (630/628) dentro del bucle cierran los
1.820 B.** Con +1 la cadena entera se conserva: 399→219→f19, 356→217→f18,
334→214→f17, 278→213→f16, 257→211→f15, y 208 (8 B, `size=2`, prio 260) sigue
delante en f22.

    p1  `asm("")` dentro del bucle, tras `particle->mSize = extra_params.x;`
        -> 98,154 % (18 diffs), y f20/f21 NO se intercambian.  Revertido.
        Misma veda que c4: el asm volátil no es un +1.

Vedas anteriores que siguen en pie (r19-plat §4): `rot_scale` declarada antes
(99,692 %), expresión entera en una sentencia (99,385 %), con `+ extra_params.y`
al final (99,385 %). Y el DWARF sigue diciendo que **`rot_scale` no existe en el
original** — quitarla es −1 insn y da 629/631, que **no** empata (222/221).

## 5. `eProject` (268 B, 88,851 %) — el orden del original, medido y descartado

Aquí sí hay una diferencia de orden de sentencias, y las dos herramientas
coinciden en cuál es la del original:

`lmap.py`: `clipX` en 244, `clipY` 245, `clipZ` 246, `oneOverW` 248,
`halfVP2` 251, `halfVP3` 252, `*sx` 254, `*sy` 255, `*sz` 256.

`dwbody.py` (orden de declaración del original):
`local, eye, oneOverW, clipX, clipY, clipZ, halfVP2, halfVP3`, con
`clipX // f13`, `clipZ // f10`, `halfVP2 // f0`, `halfVP3 // f13`.

O sea el original declara `float oneOverW;` **vacía y antes de `clipX`**, y la
asigna en 248; y `halfVP2`/`halfVP3` van juntas **detrás** de `oneOverW`.
Nuestra fuente pone `halfVP2` la primera de todas.

    q1  orden del lmap (clipX,clipY,clipZ, oneOverW, halfVP2, halfVP3)
        -> 88,851 -> 85,821 %  (22 -> 25 diffs)
    q2  q1 + `float oneOverW;` declarada antes de clipX (el split_decl del DWARF)
        -> 85,821 %  IDÉNTICO a q1.

**Revertidos los dos.** A diferencia de `UpdatePlatInfo`, aquí el reorden **no**
mejora la alineación: con q1/q2 hay más parejas INSERT/DELETE (8) que en el
estado actual (6). El planificador ya movía `halfVP2` a su sitio, así que lo
único que cambia el reorden es el reparto de registros, y a peor.

**Veda: barridas la posición de `halfVP2`/`halfVP3` y el sitio de la declaración
de `oneOverW`. Y confirmado (tercera vez en el proyecto, tras r18 c4/c5) que el
SITIO de una declaración partida no es palanca por sí solo.**

Queda anotado, para quien siga: el original tiene `clipX//f13`, `clipZ//f10`,
`halfVP2//f0`, `halfVP3//f13`; nosotros `halfVP2//f12`, `clipX//f0`,
`clipY//f13`, `clipZ//f10`, `halfVP3//f12`.

## 6. `GenerateHorizonFogDisplayList` (796 B, 98,995 %) — UNA instrucción

Todo el diff es esto:

    objetivo  ... add r0,r31,r0 · fmr f30,f22 · andi. r11,r31,0x1 · srawi r0,r0,1 ...
    nuestro   ... add r0,r31,r0 · fmr f30,f22 · srawi r0,r0,1 · andi. r11,r31,0x1 ...

`lmap.py` dice que el `andi.` no lleva nota propia y hereda la de
`EcstasyEx.cpp:767` (`grid_pointY = (i & 1) ? 0.0f : vertex_spacingY;`), y que el
`srawi` es la última insn de `EcstasyEx.cpp:764` (`multiple = i / 2;`). Es decir:
**el objetivo hunde el `srawi` de `i/2` por debajo del `andi.` de `(i&1)`**,
y eso es el planificador (`sched2`), no la fuente — el `srawi` encabeza una
cadena de 7 (`xoris → stw → lfd → fsub → frsp → fmuls f28`) y el `andi.` una de
2, así que por prioridad nuestro orden es el «correcto» y el del objetivo sólo
sale si el `srawi` no estaba listo ese ciclo.

    h1  `int odd = i & 1;` hoisted delante de `multiple = i / 2;`, usado en las
        dos ternarias -> 98,995 % EXACTO, 2 diffs.  NEUTRO.  Revertido.
        GCC ya hacía CSE de `i & 1`: el RTL es idéntico.

Veda anterior (r18-plat c16): `grid_pointY` antes de `multiple = i/2` → 91,482 %.

## 7. Qué NO he probado

- **`eProgressiveScan_EURGB60DialogBox` (1.848 B, 94 registros): cero ensayos.**
  Ni `lmap`, ni `dwbody`, ni la tabla de allocnos. Es el único de los seis al que
  no he mirado el mecanismo, y por tamaño es el tercero.
- **`epCalculateLocalDirectionalPOS16`: cero ensayos.** Sé que sobra una ranura
  muerta de 8 B pero **no he acotado qué la pide**. Lo que falta por hacer, en
  orden: volcar el RTL de expansión (`cc1plus -dr`, ya generado en
  `scratchpad/g0/z.i.rtl`) y listar los `assign_stack_temp` de DFmode de la
  función — mi extractor no casó el formato de `(mem:DF …)` del volcado y lo dejé
  ahí. Candidatos sin descartar: un tercer `assign_stack_temp` de las
  conversiones de `my_fpow`, o un `(sn_ps)escalar` que pide 8 B en la expansión y
  acaba resolviéndose con la ranura de 4 B (`0x6c`/`0x74`/`0x7c`).
  Tampoco he probado a quitar las dos locales muertas `envmap_power`/`envmap_bias`.
- **`Render__13EmitterSystemP5eView`**: no lo he tocado, como pedía el encargo.
- **El ±N limpio de `live_length`** que cierra `UpdatePlatInfo` (−1..−5 o +3..+5)
  y `UpdateParticles` (+1 o −2): probados sólo con `asm` vacíos, que no valen.
  No he probado ninguna variante de **forma de sentencia** dentro del switch de
  `UpdatePlatInfo` (p. ej. partir `envmap_min_b = envmap_min_r = envmap_min_g =
  0.4f;` en tres sentencias) — aunque `lmap.py` dice que el original tiene esa
  asignación encadenada en **una sola línea** (`eLightE.cpp:201`), o sea que esa
  concreta está vedada por el mapa de líneas.
- **El permutador**, ni guiado ni ciego, en ninguna función.
- **La `FALLA` de `__static_initialization_and_destruction_0`**: sólo
  caracterizada, no corregida.
- `pctsnap.py`: no lo he corrido. El único porcentaje por función que se mueve es
  `UpdatePlatInfo` (99,235 → 98,583); todos los demás están donde estaban
  (comprobado uno a uno con `pct_ecs.py`).

## 8. Lo que queda en el árbol

Un solo fichero tocado: `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp`, 5 líneas
por 5 líneas (la corrección c1+c2 de la §2.1, con el recuento de líneas
intacto para que el mapa de `lmap.py` siga cuadrando). `measure --cmp`: +0 B,
0 unidades cambian.
