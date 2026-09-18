# r51-ecs — las cuatro de zEcstasy (5.180 B)

Estado de partida y de llegada **idénticos**: `fncmp` da las mismas cuatro
funciones con los mismos bytes antes y después. **Cero bytes cerrados**, y el
árbol queda como estaba (`git diff` de `src/Speed/GameCube/Src/Ecstasy/` vacío).

Lo que sí sale de la ronda es que **la veda de la r49 sobre `UpdatePlatInfo`
está refutada con medida**, dos palancas nuevas que la rompen, y la función a
**UNA FILA** del 100 %.

| función | B | filas base | mejor filas medido | % |
|---|---:|---:|---:|---:|
| `UpdatePlatInfo` | 2.044 | 28 | **1** | 99,3346 → **99,98042** |
| `epCalculateLocalDirectionalPOS16` | 2.072 | 155 | 155 | 93,305 |
| `GenerateHorizonFogDisplayList` | 796 | 2 | 2 | 98,995 |
| `eProject` | 268 | 14 | 14 | 93,970 |

---

## 1. `UpdatePlatInfo`: la veda de la r49 es falsa

La r49 cerró el frente con esto:

> «LA FUNCION NO PUEDE LLEGAR AL 100 % Y ESO CIERRA EL FRENTE ENTERO. Las filas
> 333-339 del diff (el bloque de literales) son 6-7 y están demostradas
> irreducibles.»

**Son cero.** El bloque de literales del `case 0x68E97F75` (`envmap_power=6.0f;
diffuse_min_a=0.22f; diffuse_rng_a=0.86f`) casa **entero, orden y registros**,
con dos `asm` de cero bytes. Las 28 filas de la base se reparten así:

    28 filas base = 22 (ciclo de tres f1/f5/f31) + 6 (bloque de literales)

y el bloque de literales **ya no existe** como diferencia.

### 1.1 Las dos palancas

**(a) `__asm__("")` como CORTE DE REGIÓN de `sched1`.** Un `asm` sin operandos
es volátil, no emite un byte y parte el bloque básico en dos regiones de
planificación. Colocado detrás de `envmap_power = 6.0f;`:

    objetivo   lis A · lfs A · lis B · lis C · lfs B · lfs C
    base       lis A · lis B · lis C · lfs A · lfs B · lfs C
    con corte  lis A · lfs A · lis B · lis C · lfs B · lfs C   <- IDÉNTICO

La r48 había cerrado esto como imposible («con prio(lis) > prio(lfs) y dos
ranuras iu2 no hay forma de fuente que lo cambie: las 8 permutaciones/espaciados
del cuerpo del case dan el objeto IDÉNTICO»). Es cierto **dentro de un bloque**;
lo que no se probó fue partir el bloque. Es la primera vez en el proyecto que la
barrera total sirve para algo: en `eProject`, en `fog` y en `epCalculate` estaba
medida como destructiva, y de ahí venía la creencia de que no es palanca.

**(b) `__asm__("" : : : "rN")` para mover `local_alloc` en los ENTEROS.** Con
sólo (a) quedaban dos filas: el primer `lis` se lleva `r9` y el objetivo usa
`r11`. Eso **no se puede arreglar con la fuente**, y la demostración está en §1.3.
Lo que sí lo arregla es un clobber de `r9` en un `asm` de cero bytes **delante**
del `envmap_power = 6.0f;`: el registro deja de estar libre para la primera
cantidad y cae en `r11`, exactamente como el objetivo.

### 1.2 La escalera, con cifras (cada medida = un build de 14 s de la unidad)

| variante | filas | fuzzy |
|---|---:|---:|
| base (HEAD) | 28 | 99,33464 |
| `__asm__("")` **detrás** de `envmap_power = 6.0f;` | 24 | 99,74560 |
| … **más** `__asm__("" : : : "r9")` **delante** (= receta **Y1**) | **22** | 99,76520 |
| Y1 + `envmap_min_b` conmutada | **1** | **99,98042** |
| Y1 + pin `fr5` del `0.86f` con barrera (n1) | 14 | 99,83370 |

Con **Y1** las 22 filas que quedan son **sólo** el ciclo de tres
`f1→f31→f5→f1`; el bloque de literales no aparece en el diff. O sea: Y1 es
progreso **sin construcciones falsas** (dos `asm` vacíos), y la conmutación es la
que baja de 22 a 1.

### 1.3 Por qué `(r11, r9, r11)` era inalcanzable — demostración

`REG_ALLOC_ORDER` de rs6000 (`rs6000.h:932`) da para los GPR el orden
**r0, r9, r11, r10, r8, r7, r6, r5, r4, r3, r31…**; `r0` no es BASE_REGS, así que
la primera cantidad que se reparte se lleva **r9** siempre.

`QTY_CMP_PRI` (local-alloc.c) con `n_refs = 2` vale `2/(muerte−nacimiento)`, y las
cantidades se reparten de mayor a menor prioridad (empate → índice de aparición).
Llamando A/B/C a las tres direcciones (`6.0f`, `0.22f`, `0.86f`), para que salga
`A=r11, B=r9, C=r11` hace falta:

* B se reparte primero (se lleva r9) → `pri(B) > pri(A)` y `pri(B) ≥ pri(C)`;
* A choca con B (si no, cogería r9) y C choca con B;
* A **no** choca con C (comparten r11).

Con seis insns en un bloque y A ∩ C = ∅, el rango de B tiene que puentear los dos
rangos disjuntos: quedan 3 posiciones a cada lado del corte, luego el corte está
en la posición 2 y `len(B) ≥ 3` mientras `len(A) ≤ 2` → `pri(B) ≤ 6666 <
pri(A) ≥ 10000`. **Contradicción: no existe orden de insns que lo produzca.**

El modelo está **validado contra el propio objetivo**: reproduce al registro los
otros bloques del `switch` (`case 0x12C9453C` → r9,r11; `case 0x471A1DCA` →
r9,r11,r10).

La única salida que deja el código de `find_free_reg` es
`IOR_HARD_REG_SET (used, regs_live_at[ins])`: **que r9 esté ocupado**. Eso es lo
que hace el clobber, y por eso funciona. Y explica por qué el original lo
consigue sin `asm`: el mapa de líneas (`lmap.py` sobre el objetivo) dice que el
original tiene **3 líneas que nosotros no tenemos** justo antes de ese `case`
(nuestra 454 ↔ original 251, nuestra 468 ↔ original 267: el desplazamiento pasa
de +203 a +201) y **una línea de menos dentro** (468→479 son 11 líneas nuestras
contra 10 del original). Ahí hay fuente que falta.

### 1.4 La receta exacta (para aplicar en la ventana si se decide)

En `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp`, `case 0x68E97F75:` (línea 468):

```c
        __asm__("" : : : "r9");
        envmap_power = 6.0f;
        __asm__("");
```

y, para bajar de 22 a 1 fila, línea 355:

```c
    float envmap_min_b = material_data->EnvmapMinB * envmap_min_scale;
```

**No aplicada.** Da 0 bytes (`matched_code` es todo-o-nada) y mete dos `asm` —
regla 7 del brief— y la forma conmutada **está demostrado que no es la fuente
original**: es justamente la única fila que queda.

### 1.5 La fila que queda, y por qué no cae

    98:  objetivo  fmuls f26, f5, f10
         nuestro   fmuls f26, f10, f5

La cadena, leída en `global.c` (confirmando y completando el diagnóstico de la
r48): `set_preference` hace `src = XEXP (src, 0)` para un `src` de formato `'e'`,
así que de `(set 194 (mult (reg f5) (reg Q)))` el pseudo 194 (`envmap_min_b`)
**prefiere f5**; `prune_preferences` mete esa preferencia en
`regs_someone_prefers[115]` porque 194 tiene menor prioridad y choca con 115
(`diffuse_rng_a`), y 115 pierde f5 → ciclo de tres. Conmutar el producto mata la
preferencia (el primer operando pasa a ser un pseudo) pero cambia la instrucción.

**La salida existe en el código y está identificada**: `prune_preferences` hace
`AND_COMPL_HARD_REG_SET (temp, hard_reg_full_preferences[allocno])` cuando
`allocno_size[j] <= allocno_size[i]` (los dos son SF, tamaño 1). O sea: **si 115
prefiriese también f5, se salva**. Todo lo probado para dárselo:

| forma | filas |
|---|---:|
| `register float t asm("fr5") = arnMaxA` en `case 0xA6348EE3` (Y1) | **6** — el ciclo de tres CAE, pero el pin rompe 6 filas de ese bloque |
| `{ register float t86 asm("fr5") = 0.86f; diffuse_rng_a = t86; }` | 22 (combine se come la copia: objeto idéntico a Y1) |
| … con `__asm__("" : "+f"(t86))` para que la copia sobreviva | **14** — el ciclo de tres CAE; rompe 6 filas del bloque specular (f18/f19/f20) y 2 del de literales |
| … x2 | 14 |
| lo mismo en el `case 0x2388DD82` | 19 / 32 |
| `__asm__("" : : "f"(envmap_min_scale))` detrás de `min_b` (alargar f5 para que 194 CHOQUE con f5 y se le pode la preferencia sola) | 267-271 y cambia el marco: destruye el juego de preservados |

`q1` (6 filas) es la prueba de que **el mecanismo del `AND_COMPL` es el correcto**:
con la fuente **sin conmutar** el ciclo de tres desaparece entero. Lo que falta es
una forma de darle a 115 la preferencia de f5 sin emitir un registro distinto.

### 1.6 Barrido negativo de la ronda (todas sobre la base con corte)

* **Sin el pin `fr5`** de `envmap_min_scale`: 57 filas; con `asm` de
  `envmap_power` x0/x1/x2/x4/x5/x6 → 72/72/57/69/69/81. El pin sigue siendo
  imprescindible (regla 10 re-medida).
* **`asm` de `envmap_power`**: sobre la base limpia x2 → 37, x4 → 43 (x3 sigue
  siendo el óptimo); **con el corte, x2 y x3 dan lo mismo (24)**, o sea que con la
  receta Y1 uno de los tres sobra.
* **Registro del pin** (con corte): fr4 → 31, fr31 → 31, fr5 → 24. fr5 sigue
  ganando.
* **Copia `ems` del pin** (para que el primer operando sea un pseudo): x3 usos →
  57 (mata el pin), 1 uso → 24 (combine la absorbe), con barrera → 83.
* **Emitir el `fmuls` desde un `asm`**: min_b → 63, min_g → **14**, min_r → 48,
  g+b → 63, los tres → 132.
* **Pinchar los resultados** en vez de la escala (`min_r` fr30, `min_g` fr1,
  `min_b` fr26, `max_*` fr25/fr31/fr8): 92 / 98 / 92 / 65 / 126.
* **Pinchar `envmap_max_scale`** a fr8 en vez de la escala mínima: 50-66.
* **Bajar `diffuse_rng_a` debajo del bloque envmap** (la r36f decía «objeto
  idéntico»: **confirmado**, 22 filas) y pincharla a fr5 allí: 105; sin el pin de
  la escala: 72; a fr1: 32. Los dos pines a `fr5` siguen siendo incompatibles: el
  rango de `DiffuseMaxA` empieza mucho antes que el `fsubs`.
* **Escala en línea** (`material_data->EnvmapMinScale * …` x3, sin variable):
  57; min+max: 57.
* **Clobbers en el corte**: `r10`/`r12`/`r8`/`r0`/`cc`/`memory` → 3 (no tocan el
  reparto entero); `r9` **delante** → 1; `r9` **detrás** → 5; `r9,r11` → 7;
  `r9` en los dos sitios → 4; `r9` sólo delante → 6.
* **Sitio del corte**: detrás de `envmap_power` → 24; delante → 28; detrás de
  `diffuse_min_a` → 28; x2 → 27; dos cortes seguidos → 3 (igual que uno).

---

## 2. `GenerateHorizonFogDisplayList` — la palanca nueva NO entra (7 medidas)

Las 2 filas son la transposición `srawi`/`andi.` que llevan seis rondas
cerradas. La barrera de región **no puede** producirla: sólo impide adelantos, y
aquí hace falta **adelantar** una insn de una sentencia posterior (`(i & 1)`) por
encima de una anterior (`i / 2`). Medido igualmente:

| barrera | filas | fuzzy |
|---|---:|---:|
| detrás de `multiple = i / 2;` | 4 | 98,6683 |
| delante de `multiple = i / 2;` | 5 | 97,9648 |
| detrás de `grid_pointX` | 17 | 92,3367 |
| delante de `grid_pointY` | 17 | 92,3367 |
| detrás de `grid_pointY` | **2** (objeto idéntico a la base) | 98,9950 |
| `clobber r0` detrás de `i/2` | 9 | 97,2864 |
| `clobber r11` detrás de `grid_pointX` | 17 | 92,3367 |

**Veda nueva**: el corte de región de `sched1` no vale para una transposición
dentro de un bloque cuando la insn que hay que adelantar viene de una sentencia
POSTERIOR. El muro numérico de la r30 (prio 12 contra 2) sigue en pie.

---

## 3. `eProject` — 8 medidas, nada (y el andamio sigue haciendo falta)

| forma | filas | fuzzy |
|---|---:|---:|
| base | 14 | 93,9701 |
| `halfVP2`/`halfVP3` declaradas **por encima** de `clipX` | 30 | 81,6119 |
| … más barrera | 28 | 78,3731 |
| barrera delante de `clipX` | 14 (objeto idéntico) | 93,9701 |
| barrera detrás de `clipZ` | 49 | 44,3881 |
| barrera detrás de `oneOverW` | 46 | 65,3582 |
| barrera detrás de `halfVP3` | 40 | 74,5373 |
| **sin** el `__asm__("" : "+f"(halfVP2))` | 23 | 86,1194 |

**Regla 10 re-medida**: el andamio de la r36d **sigue pagando** (14 contra 23).
La r49 dice que el original no lo lleva; sigue siendo cierto, y sigue sin haber
sustituto.

---

## 4. `epCalculateLocalDirectionalPOS16` — no tocada, con motivo

155 filas, de las que 49 son **sólo el marco** (los 8 B de ranura huérfana
desplazan todos los offsets de `(r1)`). El diagnóstico de las r36b/r36c/r36d/r46/
r47 (≈600 medidas) apunta a `get_secondary_mem` de `reload`, y aporto una
precisión leyendo `reload.c`: **`secondary_memlocs` está indexado POR MODO**
(`secondary_memlocs[(int) mode]`), así que dos modos distintos que necesiten
memoria secundaria abren **dos ranuras**. El objetivo tiene una (el par
`stfd`/`lwz` de la conversión, DFmode) y nosotros dos. El segundo modo candidato
es **V2SF** (`sn_ps`, 8 B), que entra por los `(subreg:PS (reg:SF …))`
paradójicos que la r36b ya había censado (pseudos 139, 258, 603=`envvdotn`,
718=`specular_a`).

**Siguiente paso acotado, distinto del de la r47**: no bisecar sentencias, sino
volcar `-dr`/`.greg` y buscar los `assign_stack_local` por MODO — si aparece una
ranura V2SF, el trabajo es quitar la recarga GPR↔FPR de un valor `sn_ps`, no
tocar el cuerpo del bucle. No he podido medirlo en esta ronda.

---

## 5. Vedas

**Caducadas (rotas con medida):**

1. **r49, `UpdatePlatInfo`: «la función no puede llegar al 100 %, filas 333-339
   irreducibles».** Son cero filas con dos `asm` vacíos.
2. **r48, `UpdatePlatInfo`: «con prio(lis) > prio(lfs) y dos ranuras iu2 no hay
   forma de fuente que lo cambie».** Cierto dentro del bloque; falso si se parte
   el bloque.
3. **«la barrera total (`__asm__("")`) no es palanca en este proyecto».** Lo es:
   es un corte de REGIÓN de `sched1`, y aquí vale 6 filas.
4. **r36f, «bajar `diffuse_rng_a` debajo del bloque envmap da objeto idéntico»**
   — confirmada, no rota; queda anotada porque es una transformación NEUTRA que
   permite pines nuevos.

**Nuevas:**

5. `fog`: el corte de región no sirve para adelantar una insn de una sentencia
   posterior (7 medidas, ninguna baja de 2).
6. `eProject`: subir `halfVP2`/`halfVP3` por encima de `clipX` empeora (30/28);
   las barreras de región dentro de la función van de 40 a 49 filas.
7. `UpdatePlatInfo`: dar a `diffuse_rng_a` la preferencia de f5 con un pin
   funciona (el ciclo de tres cae) pero **siempre cuesta más filas de las que
   ahorra** en las cinco colocaciones probadas (6, 14, 14, 19, 32).
8. `UpdatePlatInfo`: alargar la vida de `fr5` con `asm` de sólo entrada para
   podarle la preferencia a 194 cambia el juego de preservados y el marco
   (267-271 filas). El eje está muerto.

---

## 6. Notas de método

* **El orden del `$LC` NO es el orden de la `.rodata`.** Perseguí una hipótesis
  entera sobre que nuestro `pool` tenía el `0.0f` fuera de sitio porque
  `$LC831 < $LC834`; en el objeto los dos lados dan **la misma secuencia**
  (0.1, 0.25, 0.4, 2.0, 1.18, 8.0, **0.0**, 6.0, 0.22, 0.86, 0.3, 128.0).
  Contrastar el `.rodata` con bytes, no los números de etiqueta.
* **El scratchpad es COMPARTIDO entre agentes**: otro agente sobrescribió mi
  `probe.py` y luego borró mis copias de seguridad a media ronda. Ficheros con
  prefijo propio y, mejor, subdirectorio propio.
* **Otro agente rompió `EventSequencer.h`** durante ~25 min y todos los builds de
  `zEcstasy` fallaban con un error que parecía mío. La sonda lleva reintento y
  distingue «error en MI fichero» de «cabecera compartida rota».
* Volcados RTL borrados (`scratchpad/rtl/eLightE_cpp*`), sondas borradas, nada
  nuevo en `scripts/`.
