# r52-ecs — las cuatro de zEcstasy (5.180 B)

Estado de partida y de llegada **idénticos**: 28 / 155 / 2 / 14 filas antes y
después, mismos bytes, `git status` de `src/Speed/GameCube/Src/Ecstasy/` vacío.
**Cero bytes cerrados.**

Lo que sale de la ronda es que **el ciclo de tres de `UpdatePlatInfo` — 21 de
sus 28 filas — está resuelto como mecanismo, con demostración y volcado**: lo
causa **nuestro propio pin `asm("fr5")`**, y por tanto ese andamio **no puede
quedarse**. Eso convierte un frente de seis rondas de barrido de pines en un
requisito concreto y falsable.

| función | B | filas base | mejor medido esta ronda | % |
|---|---:|---:|---:|---:|
| `UpdatePlatInfo` | 2.044 | 28 | 22 (receta Y1 de la r51, con `asm`) | 99,3346 |
| `epCalculateLocalDirectionalPOS16` | 2.072 | 155 | 155 | 93,305 |
| `GenerateHorizonFogDisplayList` | 796 | 2 | 2 | 98,995 |
| `eProject` | 268 | 14 | 14 | 93,970 |

---

## 1. `UpdatePlatInfo`: el ciclo de tres es NUESTRO pin. Demostrado.

Las 28 filas se parten en **21 del ciclo de tres f1/f5/f31** y **7 del bloque de
literales** del `case 0x68E97F75`. La r51 dejó el segundo a una fila con dos
`asm`; esta ronda ataca el primero, que es el grande.

### 1.1 Qué registro quiere cada variable

Contrastando `lmap.py` con el diff (las tres filas 96/102/103):

| variable | objetivo | nuestro |
|---|---|---|
| `envmap_min_g` (pseudo 191) | **f1** | f31 |
| `envmap_max_g` (pseudo 202) | **f31** | f5 |
| `diffuse_rng_a` (pseudo 115) | **f5** | f1 |

### 1.2 La cadena completa, con fichero y línea del GCC del árbol

1. `global.c:1407` — `mark_reg_store` llama a `set_preference (reg, SET_SRC)`
   en **cada** insn que asigna a un pseudo.
2. `global.c:1551` `set_preference` hace `src = XEXP (src, 0)` cuando el `src`
   es de formato `'e'`. Para
   `(set (reg 194) (mult (reg:SF 37) (reg 193)))` — o sea
   `envmap_min_b = envmap_min_scale * material_data->EnvmapMinB` — el primer
   operando **es un registro duro** porque `envmap_min_scale` lleva
   `register float ... asm("fr5")`. Con eso `194` recibe
   `hard_reg_preferences = f5`. **Confirmado en el volcado**: `;; 194
   preferences: 37` (37 = 32+5 = f5).
   *Y da igual quitar el pin y dejarlo como pseudo local*: `set_preference`
   hace antes `if (reg_renumber[src] >= 0) src = reg_renumber[src]`, así que
   un pseudo **local** colocado en f5 por `local_alloc` produce exactamente la
   misma preferencia.
3. `global.c:887 prune_preferences` recorre de menor a mayor prioridad y, para
   cada allocno `i`, mete en `regs_someone_prefers[i]` las
   `hard_reg_full_preferences` de **todos los allocnos de menor prioridad que
   choquen con él**. `194` va detrás de `115` en el orden
   (`;; 131 regs to allocate: … 165 437 511 **115** … 188 93 96 90 **194** …`)
   y choca con él (`;; 115 conflicts: … 188 191 194 …`) → **`f5` entra en
   `regs_someone_prefers[115]`**.
   *Por qué sólo `194` y no también `188`/`191`*: la misma función poda antes
   `hard_reg_full_preferences[j] &= ~hard_reg_conflicts[j]`, y `f5` sigue viva
   cuando nacen `188` y `191`; `194` es el ÚLTIMO uso de `f5`, muere ahí y no
   choca. **La preferencia se pega siempre al último consumidor de la escala.**
4. `global.c:1008 find_reg` sólo excluye `regs_someone_prefers` en la **pasada
   0**, y la pasada 0 exige además que el registro esté en `regs_used_so_far`
   — que en `global.c:396` se siembra con **todos los `call_used_regs`**, o sea
   f0-f13 enteros. Resultado: para `115` la pasada 0 recorre
   `REG_ALLOC_ORDER` (f0, f13, f12, f11, f10, f9, f8, f7, f6, **f5**, f4, f3,
   f2, f1, f31, f30…), descarta f0/f13-f6 por conflicto, **descarta f5 sólo por
   la preferencia ajena**, descarta f4/f3/f2 (allocnos 106/110/165) y se queda
   con **f1**.

### 1.3 La medida que lo cierra

Volcando `.greg` con `envmap_min_b` **conmutado**
(`material_data->EnvmapMinB * envmap_min_scale`, que hace que el primer operando
sea un pseudo y `set_preference` no registre nada):

```
;; 194 preferences: 37        <-- DESAPARECE
115 in 37   (= f5)            <-- objetivo
191 in 33   (= f1)            <-- objetivo
188 in 62   (= f30)           <-- objetivo
```

**El ciclo de tres cae ENTERO y por sí solo**, sin tocar nada más. Es la misma
variante que la r51 midió en **1 fila**; lo nuevo es *por qué*.

### 1.4 La consecuencia, que es el frente de la próxima ronda

Como la forma conmutada emite `fmuls f26,f10,f5` y el objetivo emite
`fmuls f26,f5,f10`, la fuente original **no** está conmutada. Y como un pseudo
**local** colocado en f5 produce la misma preferencia que el pin, sólo queda una
salida:

> **En el original `envmap_min_scale` NO lo coloca `local_alloc`: es un allocno
> GLOBAL al que `global_alloc` le da f5.**
> (`local-alloc.c:406`: sólo entran los pseudos con `REG_BASIC_BLOCK >= 0` y
> `REG_N_DEATHS == 1`; `flow.c:2702` marca GLOBAL todo lo que esté vivo **al
> final de un bloque básico**.)

O sea: **el original usa `envmap_min_scale` fuera del preámbulo**. Y nuestro pin
es un andamio que, además de no hacer falta si eso se encuentra, **impide** que
la función cierre nunca.

Lo probado esta ronda para conseguirlo, todo negativo:

| intento | filas | nota |
|---|---:|---|
| pin fr5 + `envmap_min_scale *= 0.1f;` en el `default:` | **28** | **objeto BYTE A BYTE idéntico**: el store muerto se borra en `flow` antes de fijar `REG_BASIC_BLOCK`. Transformación NEUTRA, no sirve para globalizar |
| sin pin + lo mismo | 60 | idéntico a «sin pin» a secas |
| sin pin (control) | 60 | `envmap_min_scale` se va a **f8** y `envmap_max_scale` a f7 (objetivo: f5 y f8) |
| sin pin + `envmap_min_r = envmap_min_scale;` en el `default:` | 159 | y **2.052 B**: crece el marco |
| pin fr5 **+** pin `fr26` en `envmap_min_b` (para que los dos operandos sean duros y no haya preferencia) | 74 | el rango duro de f26 destroza el resto |

Y el barrido completo del **orden** de los tres productos (por si `194` dejaba
de ser el último usuario de f5): la base es el techo.

| orden de `envmap_min_{r,g,b}` | filas |
|---|---:|
| **r, g, b (base)** | **28** |
| r, b, g | 31 |
| g, r, b | 31 |
| g, b, r | 33 |
| b, r, g | 38 |

En `r,b,g` se ve el mecanismo en directo: la preferencia se muda al nuevo
último usuario y ahora es `envmap_min_b` quien se lleva f5
(`fmuls f5, f5, f10`), con `diffuse_rng_a` todavía en f1.

**Veda nueva (con motivo, no por barrido):** mientras `envmap_min_scale` sea un
registro duro o un pseudo *local* colocado en f5, `diffuse_rng_a` **no puede**
coger f5. Buscar más pines es tiempo perdido; lo que hay que buscar es el uso
de `envmap_min_scale` fuera del preámbulo que lo convierte en allocno global.

---

## 2. El bloque de literales: por qué la receta Y1 funciona, y qué falta

### 2.1 Modelo de `local_alloc` validado contra el volcado

Reproduje `block_alloc` en 40 líneas y **acierta el reparto real**. Las reglas
que hacen falta, todas de `local-alloc.c`:

* `insn_number` **no cuenta NOTEs pero SÍ la etiqueta** de cabecera del bloque
  (`local-alloc.c:1131`). Nacimiento `= 2·n`, muerte `= 2·m`.
* `QTY_CMP_PRI = floor_log2(n_refs)·n_refs·size / (muerte−nacimiento) · 10000`.
* Con `next_qty == 3` **no** se usa `qsort`: se usa una red de intercambios
  (`local-alloc.c:1440-1448`) que compara **los qty 0/1/2 fijos**, no las
  posiciones de `qty_order`. La tercera comparación repite la primera, así que
  **puede deshacer el intercambio y dejar de primero al de MENOR prioridad.**
  Es una rareza real de GCC 2.9 y es la que abre la puerta al reparto del
  objetivo.
* **Vida FALSA**: con `-fschedule-insns2` el primer intento de `find_free_reg`
  usa `nacimiento−2` y `muerte+2` (`local-alloc.c:1490`), o sea **una insn más
  a cada lado**, y `post_mark_life` marca ese rango ampliado. Sin esto el
  modelo no acierta.

Con eso, nuestro bloque (`lisA lisB lisC lfsA lfsB lfsC`) da **A=r9, B=r11,
C=r10**, que es exactamente lo que dice el `.lreg` (`280 in 9`, `282 in 11`,
`284 in 10`).

### 2.2 Qué reparto quiere el objetivo y quién puede producirlo

El objetivo es **A=r11, B=r9, C=r11** (dos GPR, no tres). Barriendo las 90
intercalaciones posibles de los tres pares `lis`/`lfs`, **sólo dos** lo
producen:

    lisB lisA lfsA lfsB lisC lfsC      <- por la red de intercambios rota
    lisB lisC lfsC lfsB lisA lfsA

y ninguna de las dos la puede emitir `sched1`, porque los tres `lis` empatan en
prioridad, en `INSN_REG_WEIGHT` y en número de dependientes, y el último
desempate de `rank_for_schedule` es **el LUID**, que es el orden de la fuente:
`envmap_power`(267) < `diffuse_min_a`(277) < `diffuse_rng_a`(278).

### 2.3 Lo que eso implica sobre el original

El objetivo emite `lis A · lfs A · lis B · lis C · lfs B · lfs C`. Con dos
unidades `iu2` y `issue_rate` 2 —medido en el propio volcado de `sched`: en
todos los demás bloques del `switch` salen **dos `lis` en el ciclo 1**— que
`lfs A` salga en segunda posición exige que **en el ciclo 1 sólo se emitiera
una insn**. O sea: **en el bloque del original hay algo que ocupa la segunda
ranura de emisión y no emite ni un byte.** Eso es justo lo que hace el
`__asm__("")` de la receta Y1, y por eso funciona.

Verificado en el `.lreg` de Y1: el orden que ve `local_alloc` pasa a ser
`asm(clobber r9) · lisA · lfsA · asm("") · lisB · lisC · lfsB · lfsC`; la vida
falsa de A empieza en la ranura del `asm` con r9 ocupado → A=r11, luego B=r9 y
C=r11. Reparto del objetivo, exacto.

### 2.4 Hipótesis descartada con medida

**«Los `case` del grupo están partidos y hay una etiqueta dentro del cuerpo»**
—que explicaría el corte a coste cero—: **falso**. Las ocho `beq` del despacho
que llegan a ese grupo apuntan **todas** a `.L_8010A0EC`; **nadie salta a
`8010A0F4`**. No hay etiqueta dentro del cuerpo.

### 2.5 El mapa de líneas, corregido

La r51 dijo «3 líneas que no tenemos» antes del `case`. Recontado con
`lmap.py`, los desajustes locales del `switch` son cuatro y suman cero:

| tramo (objetivo ↔ nuestro) | desfase |
|---|---|
| 193-217 ↔ 395-419 | +202 |
| 221-236 ↔ 422-437 | +201 (al original le sobra 1 línea tras `diffuse_rng_a` del `case 0x471A1DCA`) |
| 244-251 ↔ 447-454 | +203 (a nosotros nos sobran 2 antes del `case 0x8812634B`) |
| 267 ↔ 468 | +201 (al original le sobran **2** —no 3— entre el `break` y el `envmap_power=6.0f`) |
| 277-309 ↔ 479-511 | +202 (a nosotros nos sobra 1 dentro del cuerpo) |

**Aviso de método**: perseguir estas líneas como «fuente que falta» tiene un
límite duro — `block_alloc` salta los NOTE (`local-alloc.c:1131`) y en
`rank_for_schedule` sólo cuenta el **signo** de la diferencia de LUID, así que
**una línea en blanco no puede cambiar ni un byte**. El mapa sólo sirve para
decir *dónde* mirar, nunca *qué* poner.

---

## 3. `epCalculateLocalDirectionalPOS16`: los 8 B están localizados

Confirmada con aritmética la sospecha de la r51, y acotada más:

    ranuras (r1) usadas          objetivo            nuestro
    locales de la fuente         0x008 .. 0x07f      0x008 .. 0x07f   IDENTICAS
    hueco sin referenciar        0x080 .. 0x087      0x080 .. 0x08f   <-- 8 B de mas
    memoria secundaria DF        0x088 (stfd/lfd)    0x090
    r12                          0x094               0x09c
    r14-r31 (stmw)               0x098               0x0a0
    LR                           0x174               0x17c
    argumentos de pila           0x178 / 0x17c       0x180 / 0x184

Las **dieciséis** ranuras de la fuente (0x008-0x07f, `SF` y `PS`) coinciden una
a una. Tras `0x07c` el `frame_offset` vale 0x080 y una ranura `DF` (alineada a
8) caería ahí: que el objetivo la ponga en 0x088 y nosotros en 0x090 significa
**exactamente una `assign_stack_local` de más**, de 8 B, antes de la de `DF`.

Y lo nuevo: **esa ranura NO está en la RTL previa a `reload`**. El volcado
`.greg` de nuestra versión lista los `(mem:MODE (plus (reg:SI 1 r1) …))` y sólo
salen 0x008-0x07f más los argumentos: **no hay ninguna local de fuente de más**.
La ranura la crea `reload` (`get_secondary_mem`, indexado por modo), y como
**ningún insn la referencia**, es un `secondary_memlocs[modo]` que se reservó y
cuya recarga acabó desapareciendo. El modo candidato sigue siendo `PS`/`V2SF`
(8 B) por los `(subreg:PS (reg:SF))` que censó la r36b.

*Siguiente paso acotado*: el volcado `.greg` de este `cc1plus` **no trae las
líneas «Reloads for insn»**, así que hay que sacarlas por otra vía (`-da` y
filtrar) para ver qué insn pide memoria secundaria de 8 B. Buscar la ranura en
la fuente es tiempo perdido: no está en la fuente.

---

## 4. `fog` y `eProject`

No tocadas. La r51 las dejó con veda motivada (`fog`: el corte de región no
puede ADELANTAR una insn de una sentencia posterior, 7 medidas; `eProject`: 8
medidas, el `asm` de `halfVP2` sigue pagando 14 contra 23). Nada de lo que he
aprendido esta ronda las toca: sus filas no son de preferencias ni de reparto
local de tres cantidades.

---

## 5. Vedas

**Nuevas, con mecanismo:**

1. **`UpdatePlatInfo`, ciclo de tres**: con `envmap_min_scale` como registro
   duro (nuestro pin) **o** como pseudo local colocado en f5, `diffuse_rng_a`
   nunca puede coger f5. Cerrado por `set_preference` +
   `prune_preferences` + `find_reg` pasada 0. **No barrer más pines.**
2. **`UpdatePlatInfo`, bloque de literales**: con sólo tres cantidades GPR en el
   bloque, `sched1` no puede emitir ninguno de los dos órdenes que dan el
   reparto del objetivo (empate en prioridad, en `REG_WEIGHT` y en
   dependientes → gana el LUID = orden de la fuente). Hace falta **una insn de
   cero bytes que ocupe una ranura de emisión**.
3. **No hay etiqueta dentro del cuerpo del `case 0x68E97F75`**: las ocho `beq`
   del despacho van todas a `.L_8010A0EC`.
4. **Un store muerto no globaliza un pseudo**: `envmap_min_scale *= 0.1f;` en
   el `default:` da **objeto byte a byte idéntico**. `flow` borra el store antes
   de que `REG_BASIC_BLOCK` cuente la lectura.
5. **`epCalculate`**: los 8 B de ranura huérfana **no son una local de la
   fuente**; no aparecen en la RTL previa a `reload`.

**Confirmadas:** el orden `r, g, b` de los `envmap_min_*` es el mejor de los
cinco probados (28 contra 31/31/33/38).

---

## 6. Notas de método

* **Un modelo de `local_alloc` de 40 líneas acierta el reparto** si lleva las
  cuatro reglas de §2.1 (etiqueta contada, `QTY_CMP_PRI`, la red de
  intercambios rota de `next_qty==3`, y la **vida falsa ±1 insn**). Validado
  contra el `.lreg` real. Barrer 90 intercalaciones cuesta milisegundos y
  ahorra decenas de compilaciones de 14 s.
* **Al `.greg` de este `cc1plus` le faltan las líneas de `reload`.** Trae el
  orden de allocnos, los conflictos, las preferencias y `Register
  dispositions`, que es lo que ha cerrado §1 — pero para `epCalculate` no basta.
* **Comprobar la sonda con un control que TENGA que cambiar**: mi barrido
  llevaba siempre una variante «= base» y dio 28 en todas las tandas; y dos
  variantes distintas del mismo orden (`b,r,g`) dieron las dos 38.
* Volcados RTL borrados de `scratchpad/rtl/` (sólo los míos, `zEcstasy_cpp.*`;
  los de otros agentes intactos). Nada nuevo en `scripts/`.
