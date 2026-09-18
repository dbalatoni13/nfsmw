# r36c — zEcstasy: la barrera tiene DOS grados de libertad, no uno

Encargo: `epCalculateLocalDirectionalPOS16` (2.072 B), `UpdatePlatInfo` (2.044 B)
y `GenerateHorizonFogDisplayList` (796 B).

**Resultado en una línea: `UpdatePlatInfo` pasa de 99,26614 % / 35 filas a
99,33464 % / 28 filas sin emitir un byte, y el ciclo de `local_alloc` que la r36b
dio por cerrado baja de CUATRO a TRES.** La palanca que lo consigue corrige una
creencia del proyecto: el `+N` de `__asm__("" : "+f"(x))` **no vale sólo por el
número N** —la lectura de la r30— sino **también por A QUÉ VARIABLE se lo pones**,
porque además de subir todos los LUID le sube `n_refs` a esa variable, y `n_refs`
es el numerador de `allocno_compare`. Cambiar el destinatario de las tres barreras
de `diffuse_min_a` a `envmap_power` **arregla `envmap_power` y no rompe nada**.

Estado de la unidad, medido contra el último commit con el fichero de HEAD
recompilado (control explícito, §5):

| función | antes | ahora |
|---|---|---|
| `UpdatePlatInfo` | 99,26614 % / 35 filas | **99,33464 % / 28 filas** |
| `epCalculateLocalDirectionalPOS16` | 93,305016 % / 155 | 93,305016 % / 155 (objeto byte a byte igual) |
| `GenerateHorizonFogDisplayList` | 98,99497 % / 2 | 98,99497 % / 2 |
| `zEcstasy` (todo-o-nada) | 140.008/145.884 B · 95,9721 % · 534 fn | **idéntico** |

`lcfix.py zEcstasy --check`: «todas las entradas `@lc` están al día». No he tocado
`configure.py`, `config/GOWE69/*` ni `splits.txt`, ni ningún fichero fuera de
`src/Speed/GameCube/Src/Ecstasy/**`. **No he hecho commit.** Cambios en dos
ficheros: `eLightE.cpp` (los tres `asm` de destinatario + retirada de un pin
redundante) y `EcstasyEx.cpp` (**sólo comentarios**, objeto byte a byte igual).

---

## 1. La palanca: la barrera vacía tiene DOS grados de libertad

La r30 modeló el `+3` como «subir en +3 **todos** los `live_length`» y midió el eje
del NÚMERO (+1 y +2 no bastan, +3/+4/+5 dan lo mismo). La r36b heredó esa lectura y
barrió otra vez el número. **Nadie había barrido el DESTINATARIO.**

`allocno_compare` (global.c) ordena por

    floor_log2(n_refs) * n_refs / live_length * 10000 * size

y una barrera `__asm__("" : "+f"(x))` toca las **dos** variables de esa fórmula:

- sube `live_length` de **todo** lo que esté vivo en ese punto (+1 por barrera), y
- sube `n_refs` de **x** (+1 por barrera).

O sea que las tres barreras son **dos palancas a la vez**, y la segunda apunta a
quien tú elijas. Barrido completo, banco suelto de `eLightE.cpp` (2,7 s por
variante), las 28 variables `float` de la función en el mismo sitio (`case
0x33A26CB6`), N = 3:

| destinatario | % | filas |
|---|---|---|
| **`envmap_power`** | **99,33464** | **28** |
| `diffuse_min_a` (la de la r30/r36b) | 99,26614 | 35 |
| `envmap_max_b`, `diffuse_min_a` | 99,26614 | 35 |
| `envmap_min_r` | 99,21722 | 38 |
| `diffuse_rng_a`, `envmap_min_g` | 99,18787 | 43 |
| `diffuse_min_scale`, `diffuse_max_scale`, `specular_min_scale`, `specular_max_scale`, `envmap_min_scale`, `envmap_max_scale`, `diffuse_min_g` | 99,14873 | 45 |
| las 15 restantes | 91,93 – 99,08 | 45 – 180 |

**`envmap_power` es la ÚNICA que mejora**, y el barrido del número sobre ella
vuelve a dar una meseta en +3: +1 → 44, +2 → 37, **+3 → 28**, +4/+5/+6 → 43,
+7/+8 → 55. Mezclas (`diffuse_min_a`+`envmap_power` en las nueve combinaciones de
1-3 cada una) no pasan de 28.

`regmap` después: **el ciclo baja de cuatro a tres.**

    antes  f5 -> f2 -> f1 -> f31 -> f5   (diffuse_rng_a, envmap_power, envmap_min_g, envmap_max_g)
    ahora  f5 -> f1 -> f31 -> f5         (diffuse_rng_a, envmap_min_g, envmap_max_g)

`envmap_power` queda en **f2, correcto**, y las 51 locales restantes siguen bien.
2.044 B antes y después: **la barrera no emite un byte** (`envmap_power` es de vida
larga, pero la que se le añade es una copia que GCC ya tenía).

### La regla, para el resto del árbol

**Cuando una función esté atascada en un `regs=` alto y tenga —o admita— un `+N`
de barreras vacías, barre el DESTINATARIO antes de dar el eje por agotado.** Es un
barrido de N·V variantes (V = variables vivas en ese punto) y cuesta 2,7 s cada
una. En esta función la diferencia entre el destinatario bueno y el que llevaba
seis rondas es **7 filas y +0,068 pp, gratis**.

### Y un pin que sobraba

Con el `+3` sobre `envmap_power`, el pin `register float diffuse_min_a asm("fr7")`
que la r36b instaló como primer peldaño **es redundante**: quitándolo el objeto
sale **byte a byte igual** (28 filas, 2.044 B). Retirado — un constructo nuestro
menos. El de `envmap_min_scale asm("fr5")` **sí** sigue haciendo falta: sin él 47
filas, y sin ninguno de los dos 60.

---

## 2. `UpdatePlatInfo`: qué queda y qué está vedado

Las 28 filas son el ciclo de tres (25 filas) más **una transposición** en el bloque
de literales de `case 0x68E97F75` (filas 333-339): el objetivo REUTILIZA `r11` para
dos de las tres direcciones y por eso tiene que intercalar `lis`/`lfs`; nosotros
gastamos tres GPR (`r9`, `r11`, `r10`) y agrupamos los tres `lis` delante. Es
presión de GPR, no de FP.

Vedas nuevas, todas sobre la base de 28 filas y todas medidas:

| barrido | nº de medidas | mejor resultado |
|---|---|---|
| barrido del DESTINATARIO: las 28 variables de la función, N=3 | 28 | **28 filas** (`envmap_power`) — §1 |
| pines a su registro OBJETIVO en las 3 del ciclo, sueltos, en pares y los tres | 7 | 67 filas (`envmap_max_g` → fr31) |
| receta CookieTrail (temporal + barrera + pin) sobre las 4 del ciclo de la r36b | 12 | 59 filas |
| pines a su registro CORRECTO en las 22 locales que ya casaban | 22 | 5 absorbidos, el resto 39-101 |
| conjuntos de pines alternativos (quitar uno, `envmap_min_scale` a fr1..fr13, `diffuse_min_a` a fr4..fr13) | 26 | 35 (la base vieja) |
| las 5 permutaciones de `envmap_min_r/g/b` y las 5 de `envmap_max_r/g/b`, intercalado, max-antes-que-min | 12 | 38 |
| N barreras (N=0..8) sobre `diffuse_min_a` con y sin el pin fr7, N sobre `envmap_power`, y las 18 mezclas de las dos | 41 | la meseta de +3 |
| mover / añadir el `+3` a los 9 bloques `case` restantes y al `default` | 36 | 28 (varios neutros) |
| `+N` (N=1..3) sobre las 3 del ciclo en cada uno de los 10 bloques `case` | 90 | ninguna baja de 28 |
| `+N` (N=1..3) sobre 15 variables **encima** del `+3 envmap_power` | 45 | 28 (neutros) |
| las 8 reordenaciones de las 6 conversiones a `int` y las 8 de los 6 `bClamp` | 16 | 50 |
| barreras (N=1..3) sobre 5 variables justo antes de las conversiones | 15 | 45 |

**Total: 350 medidas.** El ciclo de tres no se rompe con ninguna. Lo que sí se
aprende es que **pinchar una variable a su registro CORRECTO casi siempre empeora**
(17 de 22 lo hacen): el pin no es un no-op aunque el registro coincida, porque saca
al pseudo del reparto y `local_alloc` recoloca a los demás.

---

## 3. `epCalculateLocalDirectionalPOS16`: la ranura, cerrada al byte, y 296 negativos

El encargo apuntaba aquí («el mayor racimo de reparto del proyecto»). **He cerrado
el modelo de la ranura y he agotado las tres palancas del brief contra ella. Cero
mejora.** El objeto sale byte a byte igual al del último commit.

### 3.1 El tamaño de la ranura, medido y no supuesto

La r36b dejó «los 8 B de más los pone RELOAD… nosotros abrimos DOS ranuras de 8 B y
el objetivo UNA». Faltaba el número. Con la fórmula de `rs6000_stack_info` leída en
`orig/prodg/NGC_GNU_SRC/NGC/gcc/config/rs6000/rs6000.c:3977-4045`:

    total = ALIGN(vars + parm + fpmem(12) + save(224) + fixed(8), 8)
    objetivo  368 -> vars = 120     nuestro  376 -> vars ∈ (124,132] -> 128

y el **control** que lo decide: añadir una local `volatile float` de 4 B sube el
marco de 0x178 a 0x180. Si `get_frame_size()` valiera 124, los 4 B habrían cabido
en el relleno y el marco no se habría movido. Luego **vale 128**.

Y las 16 ranuras que SÍ se referencian suman **exactamente 120 B** (2 de 4 B en
0x8/0xc + 14 de 8 B de 0x10 a 0x80), idénticas offset a offset en los dos lados.
Conclusión firme: **el objetivo no tiene ranura huérfana y nosotros tenemos UNA de
8 B en 0x80**, y no aparece ni en el volcado `.greg` (post-reload: cero referencias
a 0x80 en los dos lados). O sea que no es un pseudo derramado cuyas referencias
desaparecen: es una `assign_stack_local` que **nunca se referencia** —
`get_secondary_mem` de una iteración de `reload` que luego no se emitió, o un
`spill_stack_slot` de una pasada que se rehízo.

Esto **corrige** la nota de la r36b («DOS ranuras contra UNA»): son **UNA contra
NINGUNA**, y la de más mide 8 B, no 4.

### 3.2 Las tres palancas del brief contra el marco: 296 medidas, cero

Banco de **1,5 s** por variante (compilar a `.s` y leer el `stwu` + contar insns) y
de 2,9 s con objdiff. Todas las cifras contra la base 0x178 / 518 insns / 155 filas:

| barrido | medidas | marco | filas |
|---|---|---|---|
| pines `register sn_ps X asm("frN")`, 21 locales del bloque interno × fr2/fr3/fr4 | 64 | **0x178 en las 64**, 518 insns en las 64 | — |
| barreras `asm("" : "+f"(x))`, una por cada local con inicializador | 56 | ninguna baja; **cinco lo SUBEN** a 0x180 | — |
| barreras ×1/×2/×3 sobre las 55 locales, con objdiff | 165 | — | **ninguna baja de 155** |
| formas estructurales (quitar las dos muertas, `zero` dentro del bucle, partir `envvdotn`/`specular_a`, `hack_scale` en local, pines de `zero`) | 11 | 2 la suben, 1 la baja mal | — |

Detalles que valen para otras funciones:

- **El pin NO toca el marco.** 64 pines, 64 veces 0x178 y 518 insns. El pin mueve
  `local_alloc`; el marco lo decide `reload`, y son pasadas distintas.
- Las cinco barreras que SUBEN el marco son exactamente las de los `float` que
  ocupan las ranuras de 8 B con `(subreg:PS)` paradójico (`specular_power`,
  `specular_bias`, `vdotn`, `specvdotn`, `rdotl0`). Es el mismo mecanismo por el
  otro lado: atarlos añade una ranura.
- `register float hack_scale asm("fr6")` **sí** da marco 0x170 — y es un falso
  positivo perfecto para el cuaderno: lo consigue quitando la ranura de `0x6c`,
  **que el objetivo sí tiene**, y se lleva 8 instrucciones por delante (510).
  Marco correcto, función más lejos.

### 3.3 El diagnóstico de registros: sólo TRES locales

`regmap` es tajante y contradice el «regs=124» del triaje: **89 locales iguales y
tres distintas** — `dcrg` f3→f5, `dcba` f2→f4, `scba` f4→f13, sin ciclo. Pinchadas
a su registro objetivo:

| ensayo | fuzzy | filas | insns exactas |
|---|---|---|---|
| base | 93,305016 | 155 | 376 |
| pin `dcba` fr2 | 92,23359 | **149** | **384** |
| pin `dcrg` fr3 | 91,3417 | 196 | 338 |
| pin `scba` fr4 | 92,37645 | 178 | 354 |
| `dcba`+`dcrg` | 90,89961 | 195 | 339 |
| los tres | 91,75676 | 195 | 338 |

**`pin dcba fr2` es el caso de libro de «dos medidas que engañan», y al revés que
en el brief**: baja el fuzzy 1,07 pp y a la vez **casan 8 instrucciones exactas
más** y hay 6 filas menos. No la aplico —la escalera no continúa (los pares y el
triple son peores) y dejaría la unidad peor en `pctsnap`—, pero queda anotada: si
alguien mide esta función por instrucciones exactas, ese pin es +8.

---

## 4. `GenerateHorizonFogDisplayList`: la barrera de RANURA tampoco

El encargo lo vetaba «salvo que el pin abra algo nuevo». Lo abrió a medias: el
brief de la r36c trae una palanca que la r30/r36b no tenían, **la barrera de ranura
`asm("" : "+m"(v) : : "r0")`**, y es *exactamente* la pensada para ganar un
desempate de `rank_for_schedule`. Diez colocaciones, 31 s en total:

| forma | tamaño |
|---|---|
| ranura sobre `multiple`, delante / detrás | 800 / 804 B |
| sobre `grid_pointX` detrás | 808 B |
| sobre `grid_pointY` delante / detrás | 864 / 864 B |
| sobre `current_index` delante del `i/2` | 812 B |
| clobber `r11` en vez de `r0` | 804 B |
| antes de `tex_coordY` | 852 B |
| temporal intermedio + ranura, y doble ranura | 804 B las dos |

**Las diez EMITEN BYTES** (796 B la base), porque `"+m"` sobre una local de vida
larga la fuerza a memoria. Es la confirmación por una vía más de la sentencia de la
r30: «toda insn con dependientes en esa cadena EMITE BYTES». **Frente cerrado con
16 formas (r30) + 445 del permutador ciego (r36b) + estas 10.** Sigue en
98,99497 % / 2 filas.

---

## 5. Método, control y herramientas

- **El control de la medida.** Para descartar que el cambio hubiera tocado a un
  vecino, he recompilado la unidad con el `eLightE.cpp` de HEAD y con el mío:
  `140008/145884 B · 95,9721 % · 534 funciones` en los dos. Ojo al detalle que casi
  falsea el control: `git show` devuelve el fichero con LF y **estos ficheros son
  CRLF**; hay que reconvertirlo antes de compilar o la medida es de otro fichero.
- **El banco suelto vuelve a montarse en un minuto** y ahora tiene dos velocidades:
  `EcstasyEx.cpp` con el prefacio de la r30 más `-I src/Speed/GameCube/Src/Ecstasy`
  (los `#include "./…"` no resuelven desde el scratchpad) reproduce el objeto de la
  unidad EXACTO en **2,8 s** con objdiff y en **1,5 s** si sólo hace falta el marco
  (`-S` y leer el `stwu`; ojo: el asm crudo de GCC escribe `stwu 1,-376(1)`, sin
  `r`). `eLightE.cpp` sigue sin prefacio, 2,7 s.
- Arneses en el scratchpad, prefijo `c36c_`: `bench.py` (base), `vary.py`
  (variantes CRLF-safe), `c36c_fast.py` (barrido de 1,5 s por el marco),
  `c36c_sweep.py` (barrido con objdiff que imprime **`eq`**, las instrucciones
  exactas, además del fuzzy) y `c36c_rtl.py` (preproceso + `cc1plus -dlgr` para los
  volcados `.lreg`/`.greg`; ngccc **no** propaga `-dlgr`, hay que llamar a
  `cc1plus` a mano).
- **Aviso que hereda la r36b y confirmo**: los tres `.cpp` son CRLF y un fichero de
  variantes con `\n` falla en silencio. `vary.py` convierte los patrones antes de
  buscar; sin eso no aplica nada.

## 6. Lo que NO he tocado

`EmitterSystem::Render` (696 B, 98,074715 %, 34 filas) y `eProject__FfffPA3_fPfN44`
(268 B, 85,821 %) están en mi territorio pero no en mi encargo: siguen como
estaban. No he repetido ninguna de las vedas que el encargo prohíbe (las doce
colocaciones de la barrera en `Render`, la retirada en bloque de las 691 `.size`,
ni el permutador ciego en `GenerateHorizonFog`).
