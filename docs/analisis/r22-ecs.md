# Ronda 22 — zEcstasy: el corte de `allocno_compare`, ya con palanca

Estado medido (`build_direct.py zEcstasy` antes de cada medida):

    antes    136.340 / 145.884 B   93,4578 %   532 funciones al 100 %
    despues  136.340 / 145.884 B   93,4578 %   532 funciones al 100 %
    measure.py --cmp antes_r22_ecs.json  scratchpad/c22ecs_despues.json
      -> +0 B, +0 funciones, 0 unidades cambian

**Cero bytes cerrados.** Lo que traigo: (1) la ronda 21 dio la ventana del corte
**demasiado estrecha** y aquí está la buena, (2) **una palanca que mueve
`live_length` a voluntad y está medida**, (3) `UpdatePlatInfo` con **los seis
registros FP del muro ya en el objetivo**, y (4) la `FALLA` de `audit.py`
**resuelta**: los 4.544 B **no** son crédito falso.

## 0. El encargo está VIGENTE (verificado, no heredado)

`triage.py --muro zEcstasy` sobre objetos recién construidos:

    696 B   98,075 %  Render__13EmitterSystemP5eView            faltan 2, sobran 0
    2072 B  93,305 %  epCalculateLocalDirectionalPOS16          reorden local, dmax 20
    2044 B  98,583 %  UpdatePlatInfo                            reorden local, dmax 2
    1848 B  98,766 %  eProgressiveScan_EURGB60DialogBox         94 registros
    1820 B  99,956 %  UpdateParticles                           4 registros
     796 B  98,995 %  GenerateHorizonFogDisplayList             reorden local, dmax 2
     268 B  88,851 %  eProject                                  reorden local, dmax 5

696 + 8.848 = 9.544 B. Coincide con el papel.

## 1. `audit.py` — el FALLA, confirmado en segunda pasada y RESUELTO

    531 ok
    __static_initialization_and_destruction_0
      FALLA: reubicacion en un solo lado: lis r11, 0x803e | lis r11, $LC980@ha

Dos pasadas idénticas. **No es crédito falso: los 4.544 B están bien casados.**
Es un hueco del troceador, y lo he comprobado valor a valor.

Comparadas las 1.136 filas **con las reubicaciones visibles** (sin
`function_reloc_diffs=none`), sólo hay **tres parejas** en las que el objetivo no
lleva reubicación:

    fila 132  lis r11, 0x803e         | lis r11, $LC980@ha
    fila 134  mtlr r11                | mtlr r11          <- IDENTICO en los dos lados
    fila 169  lfs f12, -0xb8c(r7)     | lfs f12, $LC980@l(r7)
    fila 241  lis r4, 0x803e          | lis r4, $LC986@ha
    fila 341  lfs f9,  -0xb74(r15)    | lfs f9,  $LC986@l(r15)
    fila 274  lis r5, 0x803e          | lis r5, $LC996@ha
    fila 392  lfs f1,  -0xb4c(r10)    | lfs f1,  $LC996@l(r10)

Resueltas contra el ELF original y contra nuestro `.o`:

    0x803e0000-0xb8c = 0x803DF474 -> 42a00000 =  80,0f    ==  $LC980 =  80,0f
    0x803e0000-0xb74 = 0x803DF48C -> 3f7ae148 =   0,98f   ==  $LC986 =   0,98f
    0x803e0000-0xb4c = 0x803DF4B4 -> c1d80000 = -27,0f    ==  $LC996 = -27,0f

y los vecinos cuadran uno a uno (`$LC979/980/981` ↔ `0x803DF470/474/478` =
0,55 / 80,0 / 0,2; ídem los otros dos tríos). **Las dos versiones referencian el
mismo literal.**

**La causa está en `config/GOWE69/symbols.txt`**: tres entradas del pool están
declaradas con `size:0x8` cuando todas sus vecinas son `size:0x4`, así que la
segunda palabra se queda sin símbolo propio:

    24237: lbl_803DF470 = .rodata:0x803DF470; // size:0x8   <- deberia ser 0x4 + lbl_803DF474
    24242: lbl_803DF488 = .rodata:0x803DF488; // size:0x8   <- deberia ser 0x4 + lbl_803DF48C
    24251: lbl_803DF4B0 = .rodata:0x803DF4B0; // size:0x8   <- deberia ser 0x4 + lbl_803DF4B4

Sin `lbl_803DF474/48C/4B4` el desensamblado del objetivo no puede emitir la
reubicación y `audit.py` ve «reubicación en un solo lado». **No lo he arreglado
porque `config/GOWE69/*` está prohibido en mi encargo**; son seis líneas
(cambiar tres `size:0x8` a `size:0x4` y añadir las tres mitades que faltan).

Y una corrección a la ronda 21: **`LightColourFE2` NO tiene nada que ver.**
Aparece 4 veces en los dos lados, y el `mtlr r11` de la fila 134 es **idéntico**
en ambos: es GCC aparcando un GPR en LR por falta de registros, y lo que aparca
es el `@ha` del pool, no el de un símbolo de datos.

## 2. La ventana del corte: la ronda 21 la dio DEMASIADO ESTRECHA

`allocno_compare` (global.c 2.95): `pri = int(floor_log2(n_refs) * n_refs /
live_length * 10000 * size)`. Las ventanas reales, calculadas para los deltas
−40..+40, **no son «+1 o −2» ni «−1..−5 o +3..+5»**:

`UpdateParticles` (pseudos 428 y 439, `n_refs 7`, `pri = 140000/len`):

    d = -39 -34 -29 -26 -21 -18 -13 -10 -7 -2 | +1 +4 +7 +10 +13 +16 +19 +22 ...

`UpdatePlatInfo` (142/145/148, `n_refs 3`, `pri = 30000/len`):

    d = -5..-1  |  +2..+5  |  +8..+12  |  +15..+19  |  +22..+26  ...

La 21 se dejó fuera **`+2`** en `UpdatePlatInfo` (que es el que resulta útil) y
todos los tramos lejanos de las dos. Con los tramos completos hay mucho más
sitio del que parecía.

## 3. `UpdatePlatInfo` (2.044 B) — LOS SEIS REGISTROS DEL MURO, EN EL OBJETIVO

### 3.1 La palanca: `live_length` se mueve desde el fuente, y está medida

**`c3`** (mover los tres `plat_info->SpecularMin*` detrás de los
`plat_info->SpecularRng*`) produjo **exactamente** el desplazamiento predicho:

    142/145/148   len 442/441/440  ->  445/444/443     (d = +3, predicho +3)
    prioridades   67 / 68 / 68     ->  67 / 67 / 67    EMPATE
    registros     f18 / f20 / f19  ->  f20 / f19 / f18 = EL OBJETIVO

**Es la primera vez en el proyecto que se mueve `live_length` a un valor pedido
de antemano.** Ya no es diagnóstico: es palanca.

Pero `c3` **rompió el otro trío**: al invertir el orden de las tiendas, `min` sube
+3 y `rng` baja −3, y `specular_rng_r/g/b` (pseudos 153/157/161) **ya estaban
bien** (426/427/428 → 46/46/46 → f16/f15/f14 = objetivo). Medido: 98,571 %.
**Revertido.** Y `lmap.py` confirma que el orden de tiendas del original es el
nuestro (líneas 371-393, una por campo, en nuestro mismo orden), así que `c3`
además era infiel.

Las dos ventanas juntas, con el orden de tiendas original (`min = 442+d`,
`rng = 429+d`):

    min empata  <=>  d en {-5..-1} u {+2..+5} u {+8..+12} ...
    rng empata  <=>  d en {-9..-4} u {-1..+5} u {+8..+15} ...
    INTERSECCION:    d en {-5,-4} u {-1} u {+2..+5} u {+8..+12}

### 3.2 `c5` — SE QUEDA (y baja el porcentaje: es decisión tuya)

El `asm("" : : "f"(envmap_power));` del case `0x68E97F75` **es invención
nuestra** — el original no lo tiene — y vale **exactamente +1 insn** de
`live_length` (0 bytes). Quitarlo da `d = −1`, que está en la intersección:

    142/145/148   442/441/440 -> 441/440/439   pri 68/68/68  -> f20/f19/f18  OBJETIVO
    153/157/161   429/428/427 -> 428/427/426   pri 46/46/46  -> f16/f15/f14  OBJETIVO

    UpdatePlatInfo  98,58317 % (80 diffs)  ->  98,27985 % (74 diffs)   size 2044/2044
    measure --cmp: +0 B, 0 unidades cambian
    pct por funcion sobre las 2.014 del informe: cambia UNA, esta (-0,3033 pp)
    audit.py: 531 ok, el mismo unico FALLA de siempre (el de la §1)

**Los seis registros FP del muro quedan en el objetivo**, el tamaño no se mueve y
el recuento de diffs baja de 80 a 74. El porcentaje baja 0,30 pp: es el caso
documentado en HERRAMIENTAS §5 («el porcentaje tampoco mide avance»). Lo dejo
puesto y te lo señalo, como pide el brief.

**Y una corrección a la ronda 21**: su `c3` decía que sin la barrera
«`envmap_power = 6.0f` se pliega dentro de `envmap_magic` y desaparecen varias
insns». **No es cierto**: el tamaño sigue siendo 2044 B exactos y el número de
diffs BAJA. No desaparece ninguna instrucción.

### 3.3 Lo que queda en `UpdatePlatInfo`: 74 filas de FPR volátiles

Ya no hay ni un registro salvado mal. Las 74 filas son `local_alloc` sobre
`f1..f13` (patrón constante «el objetivo usa f7/f6/f2, nosotros f6/f5/f1»), más
**una sola instrucción movida**: `fsubs fX, f24, fY` (= `diffuse_rng_a =
DiffuseMaxA - diffuse_min_a`, línea 149) sale dos huecos antes en el nuestro
(fila 102 contra 104). Es planificación.

## 4. `UpdateParticles` (1.820 B) — el modelo VALIDADO, y por qué no cierra

Confirmado con volcado propio (§7): pseudo 428 = `257.0f`, `len 632`, pri 221 →
f20; pseudo 439 = `1/255`, `len 630`, pri 222 → f21. El objetivo los quiere al
revés, y como `allocno_compare` desempata por número de allocno y 428 < 439,
**basta con que empaten**.

Los dos `lfs` salen del **mismo sitio del precabezal en los dos lados** (filas
134 y 136, con una insn en medio), o sea que el hueco de 2 es idéntico. Y el
multiconjunto de instrucciones es idéntico y el tamaño también (1820/1820).

**El problema es de contabilidad, y ahora está acotado:** cualquier `+N` mete
`4N` bytes y cualquier `−N` los quita, así que la única forma de que esto cierre
es que el RTL del original tenga **una instrucción más que el nuestro antes de
`reload`** y **una menos después** (o sea, que `reload` le ahorre un derrame que
a nosotros no). No he encontrado la construcción.

Diagnósticos (no candidatos, sólo para validar el modelo):

    d1  `particle->mSize = extra_params.x;` duplicado -> +2 insns (size 1828)
        -> 99,481 % (9 diffs). d=+2 NO empata, tal y como predice la tabla. OK.
    d2  `particle->mColour = ...;` duplicado -> GCC lo borra, +0 insns, 1820 B,
        99,956 % y 4 diffs. La tienda redundante a la misma direccion con el
        mismo valor SI se elimina; la de `mSize` (lfs+stfs) no.

Ensayos:

    c1  calcular `rot_scale`/`adelta` ANTES de `pangle` (para invertir el orden
        de izado de las dos constantes) -> 99,956 -> **93,831 %** (43 diffs).
        Revertido. Reordena el cuerpo entero, no solo el precabezal.
    c2  partir la linea 1083 en dos (para meter una NOTE de linea dentro del
        bucle) -> 99,956 %, IDENTICO, y las longitudes siguen en 630/632.
        Revertido.

**VEDA NUEVA Y MEDIDA: las NOTE de línea NO cuentan en `live_length`.** Era la
única vía de «+1 insn con 0 bytes» que quedaba después de la veda del `asm`, y
está cerrada. (Contra lo que dice el código de `flow.c` de memoria: aquí no
suma.)

## 5. `eProgressiveScan_EURGB60DialogBox` (1.848 B) — el hueco grande, ya con diagnóstico

Cero ensayos en la 21; aquí queda **clasificado entero**. 94 filas de diff,
tamaño 1848/1848, y **son DOS cosas, no 94**:

| filas | qué es |
|---|---|
| **68** | `lis rX, SYM@ha` + `addi rY, rX, SYM@l`: **el objetivo usa r8 y nosotros r9**. 62 de ellas son las cadenas de `eDEMORFPrintf`, 6 son `g_InitPad`/`g_LastInitPad`/`PADMASKS` |
| **26** | el enmascarado de botones de los dos `if` de flanco |

**(a) El scratch del `elf_high`.** Es `local_alloc`, y con el orden de
`REG_ALLOC_ORDER` de rs6000 (`… 0, 9, 11, 10, 8, 7, 6, 5, 4, 3 …`) los dos casos
son consistentes entre sí:

    scratch suelto (bloques de eDEMORFPrintf)  objetivo r8   nuestro r9
    dos a la vez (precabezal, filas 81/82)     objetivo r8,r9  nuestro r9,r11

o sea: **en el objetivo r11 y r10 están ocupados en esos rangos y en el nuestro
no**. No he encontrado qué los ocupa.

**(b) El enmascarado.** Es una diferencia REAL de forma, no de nombres:

    objetivo  rlwinm r9,r10,DOWN ; rlwinm r0,r11,DOWN ; xor r9,r9,r0 ; and. r8,r9,r0
    nuestro   rlwinm r9,r11,DOWN ; rlwinm r0,r10,DOWN ; xor r0,r0,r9 ; and. r9,r11,r0
                                                                            ^^^ CRUDO

**El objetivo hace el `and.` contra el valor ENMASCARADO y nosotros contra el
botón crudo** (`r11`), en las cuatro apariciones (DOWN y UP, rama de mando y rama
de volante). Eso es `combine`: para que no pueda plegar la máscara, el pseudo
`(last & MASK)` tiene que tener **dos usos** — en el objetivo los tiene, en el
nuestro sólo uno. La fuente es
`((now & M) ^ (last & M)) & (last & M)` en los dos casos, así que lo que falla es
que a nosotros **no se nos unifican las dos apariciones de `(last & M)`**.
**Cero ensayos aquí**: es el sitio por donde yo seguiría.

## 6. `epCalculateLocalDirectionalPOS16` (2.072 B) — sigue sin ensayos, pero con la lista de sospechosos

Confirmado el marco +8 (`stwu r1,-0x178` contra `-0x170`) y que en el `.greg`
sólo hay **un** `(mem:DF …)` vivo, o sea que la ranura que sobra **no la
referencia ninguna instrucción**: es un `assign_stack_temp` de 8 B cuyas insns se
borraron.

**No he podido volcar el RTL de expansión: `cc1plus -dr` sobre esta unidad
escribe 71 MB y LLENÓ EL DISCO** (ver §8). Sospechosos sin descartar, por orden:
la conversión `int`→`float` y la `float`→`int` de `my_fpow` (una tercera copia),
y los `(sn_ps)escalar` (`(sn_ps)vdotn`, `(sn_ps)specvdotn`, `(sn_ps)specular_a`,
`(sn_ps)envvdotn`, `(sn_ps)hack_scale`, `(sn_ps)alpha_rng`), que convierten
SFmode→PSmode y podrían pedir 8 B en la expansión.

Aviso para quien lo retome: aunque se cierren los 8 B quedan **106 filas** de
permutación de `f2..f13` en el bucle interno. No es una función de un solo muro.

## 7. Herramientas

- **`lreg.py` arreglado** (copia en `scratchpad/c22ecs_lreg.py`): su `KEEP` es
  `^-(O|f|m|g|W|std=|ansi|pedantic)` y **se come `-G0`**, que es justo la bandera
  que decide este reparto. Con `^-(O|f|m|G|g|W|std=|ansi|pedantic)` reproduce el
  build **exacto**: las longitudes 630/632 y 442/441/440 de la ronda 21 salen
  clavadas. Vale la pena subirlo a `scripts/`.
- `scratchpad/c22ecs_len.py`: tabla `pseudo / n_refs / live_len / size / pri /
  registro` de una función, con el `size` (los `8 bytes` cuentan doble en la
  prioridad, que `lreg.py` no aplica).
- `scratchpad/c22ecs_it.sh`: build + `pct_ecs` de las siete funciones, **y aborta
  si el build falla** — sin eso, con el disco lleno, `pct_ecs` mide el `.o`
  rancio y da la cifra del ensayo ANTERIOR. Me pasó una vez (§8).

## 8. AVISO: el disco de este árbol se llenó a 0 durante la ronda

`cc1plus -dr` sobre `zEcstasy` deja 71 MB y llevó `C:` a **1,9 MB libres**. Con
el disco así, `build_direct.py` **falla en silencio** y `pct_ecs.py` mide el
objeto anterior: es exactamente la trampa «disco lleno se lee como la variante no
cambia nada» de HERRAMIENTAS §5, y me costó una medida falsa (el `d2` que
parecía repetir la cifra de `d1`). Borré mis volcados y al final de la ronda
había 11 GB. **Con ocho agentes a la vez conviene vigilarlo.**

## 9. Qué NO he probado

- **`Render__13EmitterSystemP5eView` (696 B)**: no lo he tocado, como pedía el
  encargo.
- **`eProgressiveScan`**: **cero ensayos de fuente**, sólo el diagnóstico de la
  §5. La palanca a probar es (b): forzar que `(last & MASK)` tenga dos usos.
- **`epCalculateLocalDirectionalPOS16`**: cero ensayos. Falta el volcado `-dr`
  (necesita ~100 MB de disco libre) y probar a quitar las locales muertas
  `envmap_power`/`envmap_bias`.
- **`GenerateHorizonFogDisplayList` (796 B, 2 diffs)** y **`eProject` (268 B)**:
  no los he tocado; la 21 los dejó con veda.
- **`UpdatePlatInfo` con `d = −4`, `−5`, `+2..+5`, `+8..+12`**: sólo he probado
  `d = +3` (c3, rompe `rng`) y `d = −1` (c5, puesto). Los demás tramos de la
  intersección de la §3.1 están sin tocar, y **no sé qué fuente los produce**
  aparte del `asm` que ya he gastado.
- **El permutador**, ni guiado ni ciego, en ninguna función.
- **`config/GOWE69/symbols.txt`**: no lo he tocado (prohibido). Las seis líneas
  que arreglan el `FALLA` están en la §1.
- `frozen.py chk Speed/Indep/SourceLists/zEcstasy` sigue marcando **HA
  CAMBIADO** (viene de la ronda 21 y ahora también de `c5`). No he vuelto a
  congelar: sin cierre, es decisión tuya.

## 10. Lo que queda en el árbol

Un solo fichero tocado por mí: `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp`,
**una línea** (el `asm("" : : "f"(envmap_power))` del case `0x68E97F75`
sustituido por una línea en blanco, para no mover el mapa de líneas).
`measure --cmp`: +0 B, 0 unidades cambian. `audit.py`: sin fallos nuevos.
