# r20 — snd: cinco unidades (sfir, sfrsf, slib, srender, SNDI_root1x)

Linea base `base_r20_snd.json` (696/2692 B). Resultado `despues_r20_snd.json`:
**1568/2692 B, +872 B, +2 funciones, 0 regresiones.**

| unidad | antes | despues | estado |
|---|---|---|---|
| `mix/sfrsf` | 100/828 B (80,797%) | **828/828 B** | **CERRADA**, `audit.py` ok, congelada |
| `cmn/SNDI_root1x` | 0/144 B (89,028%) | **144/144 B** | **CERRADA**, `audit.py` ok, congelada |
| `cmn/srender` | 0/188 B (69,468%) | 0/188 B (**93,298%**) | no cierra: 1 instruccion |
| `mix/sfir` | 0/936 B (89,252%) | 0/936 B (89,252%) | REVERTIDA al estado r19 |
| `cmn/slib` | 360/360 B | 360/360 B | ya estaba al 100% (el encargo la daba a cero) |

`slib` y `sfir8` ya estaban cerradas: la tabla del encargo (`slib` 360 B a
escribir) no coincidia con `base_r20_snd.json`. El trabajo real eran cuatro
funciones, no cinco.

---

## 1. SNDI_root1x — `SNDI_rootof1plusx(float)` — CERRADA (144 B)

Serie de Taylor de `sqrt(1+x)`. Ya existia al 89,028%.

**La pista fue el orden del pool**: el objetivo emite `1.0` **el ultimo**
(`80413014`), detras de las seis constantes negativas. GCC emite el pool en
orden de creacion, asi que `result = term + 1.0f` tiene que ir **despues** de
las seis multiplicaciones `x * K`.

| # | variante | medida |
|---|---|---|
| v01 | seis locales `u1..u6` antes de `result = term + 1.0f`, cadena `term *= u_i` | 69,722 % (pool ya correcto, planificacion no) |
| v03 | v01 con `t2` explicito y el resto encadenado | 54,167 % |
| v04/v05/v06 | `result` declarado aparte, `1.0f + term`, `result += (term *= u)` | 69,722 % (identicas) |
| **v02** | **una local por termino (`t1..t7`) y una sola suma `1.0f + t1 + … + t7`** | **100 %** |
| v07 | v02 con `t6 * u6` en la expresion final | 100 % (equivalente) |

Es el mismo estilo que las hermanas `SNDI_sin.c` / `SNDI_cos.c`, que tambien
declaran todas las potencias arriba. La forma SSA (un nombre por valor) es la
que reparte los registros como el objetivo; reasignar `term` los reutiliza.

## 2. sfrsf — `SFILTER_rsf` — CERRADA (728 B; la unidad, 828 B)

Venia del muro de la ronda anterior (80,797 %, «rotacion r27/r28 y el
cross-jump del bloque error»). **No era reparto de registros: era la
estructura.** Cinco ensayos, todos medidos:

| # | cambio | medida |
|---|---|---|
| c1 | `ptemp = pdst + (outputframes - 4)` en vez de `pdst + outputframes - 4` | 80,797 → **81,923 %** |
| c2 | **el bloque de error escrito EN LINEA en las dos ramas** (no `goto error` con una sola copia al final) **y la rama 2 con `if (inputframes > 0 && pfnnext != 0) { temp = …; if (temp <= 0) {error} }`** | 81,923 → **95,500 %** |
| c3 | los cuatro `history[i] = 0.0f` en orden `0,1,2,3` (no `3,0,1,2`) | 95,500 → 95,522 % |
| c4 | `inputframes = outputframes;` **antes** del `if (firstdone)`, no dentro del `else` | 95,522 → **96,099 %** |
| c5 | `return outputframes;` **dentro de cada rama**, no una vez al final | 96,099 → **98,681 %** |
| c6 | `inputframes = outputframes + (4 - prss->centersample)` (parentesis a la derecha) | 98,681 → **100 %** |

**Lo que hay que llevarse (c2), porque es reutilizable:** el objetivo tenia el
bloque de error **en medio** de la funcion, no al final, y las tres guardas de
la rama 2 saltaban a la **continuacion**, no al error. GCC 2.95 no reordena
bloques: el orden del `.text` **es** el orden de la fuente. Un bloque de manejo
de error que aparece en mitad de la funcion y al que salta otra rama de lejos
es la firma del **cross-jump**: la fuente tenia **dos copias identicas** del
manejador y `jump.c` fusiono la cola, dejando el `ble` de la primera rama
apuntando a la copia de la segunda. Con `goto error` y una sola copia al final
nunca sale esa disposicion.

Variantes descartadas y medidas: expresion unica `outputframes + 4 - cs`
(96,099 %, identica), `inputframes += 4; inputframes -= cs` (98,681 %), local
extra `nframes` para el temporal (99,753 %), `nframes` en las 12 posiciones
posibles de la lista de declaraciones (99,753 %, invariante), `4 - cs` calculado
primero (97,500 %).

El DWARF (`libdwarf.py cu sfrsf.c`) confirma el reparto final entero:
`outputframes` r28, `pdstp` r27, `psrc` r30, `pdst` r29, `ptemp`/`inputframes`
r4, `temp` r3, `i` r11, `accwhole`/`accfrac` en pila. Todo casa.

## 3. srender — `SNDI_validrendermode` — 69,468 → 93,298 %, NO CIERRA

**El DWARF dio el interfaz exacto** (`libdwarf.py cu srender.c`):
`prenderindex` r8, `pph` r4, y **tres** locales — `validrendermode` r3,
`globalplayloc` **r3 tambien** (comparten registro: uno se asigna del otro) y
`globalspatialization` r0. No hay local para `pph->rendermode` ni para el
elemento del array: la fuente los relee.

| # | variante | medida |
|---|---|---|
| w01 | fuente previa (`result = b;` incondicional antes del `if`) | 69,468 % |
| w02 | igual con los nombres del DWARF | 69,468 % |
| **w03** | **`if (pph->rendermode & 0x70C) { … } else { validrendermode = globalplayloc; }`** — if/else, no preasignacion | **93,298 %** |
| w05 | `while` con el resto anidado bajo `if (validrendermode)` | 68,298 % |
| w06 | polaridad invertida del segundo `if` | 62,021 % |
| y01 | `while` + `(*prenderindex)++` explicito antes de cada `continue` | 75,213 % |
| y04 | `goto` a una etiqueta que incrementa | 93,298 % |
| z01 | `globalspatialization` leido dentro de las dos ramas del `if` de 0x60 | 70,213 % |
| z04 | `globalspatialization` en dos sentencias | 90,638 % |
| q01/q03 | `\|\|` / `&&` cortocircuitados con `validrendermode = globalplayloc` incondicional | 69,468 % |

**Lo unico que falta es UNA instruccion** (46 nuestras contra 47 del objetivo):

    objetivo:  beq A ; and. r3,r10,r3 ; bne A ; lwz r9,0(r8) ; b T ;  A: lwz r9,0(r8) …
    nuestro:   beq A ; lwz r9,0(r8) ; and. r3,r10,r3 ; beq T ;        A: lwz r9,0(r8) …

Los dos insertan la **misma** carga de `*prenderindex` (la que necesita el
`(*prenderindex)++` del incremento del `for`), pero en **aristas distintas**:
el objetivo en la arista `B→cola` (critica, se parte y queda `lwz; b`), el
nuestro al **principio del bloque B**, con lo que la cola queda vacia y `jump.c`
invierte `bne A; b T` en `beq T` y se ahorra el `b`. Es colocacion de LCM/PRE,
el mismo mecanismo que bloquea `sfir`.

Barridas y **vedadas**: la sentencia del `continue` (`== 0`, `!`, `goto`,
`continue` explicito), la posicion del incremento del `return`
(`(*p)++` / `*p = *p + 1`), la forma del bucle (`for` / `while` / `for(;;)` con
incremento explicito), el orden de las declaraciones de las locales, la
polaridad de los dos `if`, y `globalspatialization` calculado dentro/fuera de
las ramas. **Ninguna mueve la carga.**

**Lo que NO he probado en srender:** el permutador, restricciones de registro, y
sacar el cuerpo del `if` a una funcion estatica que se inline.

## 4. sfir — `calcFIRCoeffs` — REVERTIDA a 89,252 %

El encargo daba media hora. La conclusion es que **no son las banderas** y que
la palanca sigue oculta.

**Correccion al informe de r19: el DWARF SI trae locales.** `libdwarf.py cu
sfir.c` da `sum` f31, `tmpFloat` f29, `halfLen` **r26**, `cnt` r31 y
`fir_coef_var` **sin localizacion** (eliminada). No aparece `halfTmpFloat`,
pero quitarla **empeora** (89,252 → 87,060 %), asi que probablemente esta
declarada en un bloque anidado que `libdwarf.py` no recorre.

**Barrido de banderas (20 combinaciones, solo diagnostico, nada tocado):**

    (base) 89,252 % 940 B · -fno-gcse 72,222 % · -O1 50,829 % · -Os 57,979 % 924 B
    -O3 89,252 % · -fno-schedule-insns 70,765 % · -fno-schedule-insns2 84,521 %
    -fstrength-reduce 72,278 % · -fno-force-mem 88,932 % · -fno-expensive-optimizations 89,209 %
    -fno-cse-follow-jumps -fno-cse-skip-blocks 88,355 % · sin efecto: -fno-caller-saves,
    -fno-move-all-movables, -fno-reduce-all-givs, -fno-peephole, -fno-function-cse,
    -fno-defer-pop, -fno-thread-jumps, -fno-inline, -fno-regmove, -fno-delayed-branch

**Ninguna bandera reproduce el objetivo.** Descarta la hipotesis de cflags por
fichero, y descarta tambien `-Os` (que cambia PRE por GCSE clasico: da 924 B,
cuatro *menos* que el objetivo).

El objetivo **no elimina ni una sola** de las siete `addi rX, r29, 0x20`
(`&coef[0]`) ni de las cinco `slwi rX, r26, 2` (`halfLen*4`), incluidas dos que
son trivialmente redundantes en linea recta. Nosotros las hoisteamos las dos y
metemos `mr` de mas en cada `case` (940 B contra 936 B, una instruccion).

**Hallazgo nuevo, con medida, que no cierra:** usar `fir_coef_var` como indice
**solo en la cola** (`fir_coef_var = halfLen;` justo antes del `if (filtType ==
3)`, y luego `pfir->coef[fir_coef_var]` en las dos sentencias de la cola)
rompe la unificacion de `halfLen*4` y sube a **92,030 %** — coloca `slwi r0,
r26, 2` y `mr r30, r10` en su sitio dentro de los tres `case`. **Pero el tamano
sube a 944 B** (dos instrucciones mas que la base): PRE se limita a hoistear
ahora `fir_coef_var*4`. Por eso **lo he revertido**: el porcentaje sube y el
tamano se aleja. Es el reverso de la trampa del brief.

Medidas del barrido: `halfLen = 4` repetido en cada `case` 84,368 % ·
`fir_coef_var` en los `case` 89,252 % · `fir_coef_var` solo en el `sum` final
89,252 % · `sum` partido en dos sentencias 89,209 % · `+= 1.0f` explicito
89,252 % · `cnt < halfLen + 1` en el bucle final 89,252 %.

**Lo que NO he probado en sfir:** sacar el cuerpo de un `case` a una funcion
estatica inline, el permutador, y restricciones de registro.

---

## Promociones — NO aplicadas (`configure.py` sin tocar)

`trypromo.py` sobre las tres unidades al 100 % de mi lote:

    Speed/Indep/Libs/snd/9/source/library/mix/sfrsf          DOL ROTO (be5372edd00e)
    Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_root1x    DOL ROTO (14282a16ce0e)
    Speed/Indep/Libs/snd/9/source/library/cmn/slib           DOL ROTO (f0e611dcf9ba)

Las tres estan al 100 % en `.text` y las tres rompen el enlace. **No promocionar
ninguna.** `slib` ya estaba al 100 % desde antes de esta ronda y tambien lo
rompe, asi que el problema es de la biblioteca, no de mi trabajo.
