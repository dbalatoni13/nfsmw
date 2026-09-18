# Ronda 30 — mx: zEAXSound2 / zFe2 / zSpeech / zPlatform

**UNA función cerrada (448 B) y la barrera de la r29 RETIRADA: no hacía falta.
La causa real de `CreateMixCtls` era una sentencia DUPLICADA en las dos ramas que
`jump.c` funde por cross-jump — la misma regla del §8 de HERRAMIENTAS. Y dos
vetos cuantificados con los números del propio compilador (`ProcessUpdate` por
prioridad de `sched`, `RenderString` por `allocno_compare`), uno de los cuales
REFUTA el umbral que la r27 dejó escrito.**

`measure.py --cmp`: **+448 B, +1 función, 1 unidad cambia** (zEAXSound2
165.908 → 166.356).
`pctsnap.py --cmp`: **EMPEORAN: ninguna. MEJORAN: 1.**

| función | unidad | antes | después | B |
|---|---|---|---|---|
| **`NFSMixMapState::CreateMixCtls`** | zEAXSound2 | 97,32143 | **100,00000** | **448** |

Fichero del árbol tocado: **UNO**, sin commit —
`src/Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMapState.cpp`
(−11 líneas netas: el bloque `{ int ntmp; … }` sustituido por el `if/else` con la
llamada en las dos ramas, más el `// UNSOLVED` retirado). **Sin `asm` de ningún
tipo: la barrera que la r29 dejó documentada NO se ha aplicado y NO hace falta.**

---

## 0. Verificación del encargo

`build_direct.py zEAXSound2 zFe2 zSpeech zPlatform` → 4 ok.
`triage.py … --muro` reproduce el encargo salvo **dos cifras rancias** (las
mismas que ya avisó la r29 y una nueva):

- `ActualReadJoystickData`: el encargo dice **94,595 %**, está al **99,244 %**.
- `RenderString`: el encargo dice 93,32316 % ✔.
- `ProcessUpdate__CARSFX_Turbo`: el encargo dice «~muro», está al **97,500 %**
  con **4 filas crudas** según `triage`, **7 reales** según `fndiff`.

| pasada | zEAXSound2 | zFe2 | zSpeech | zPlatform |
|---|---|---|---|---|
| `audit1_*` | **923** ok / **0 FALLA** | 1304 / **0** | 701 / **0** | 135 / **0** |
| `audit2_zEAXSound2` (confirmación) | **923** / **0** | — | — | — |

Los 1304 / 701 / 135 coinciden **al dígito** con los de cierre de la r29, y los
923 de zEAXSound2 son los 922 de la r29 **+1 (`CreateMixCtls`)**. Cero FALLA en
las dos pasadas: no hubo nada que confirmar. `audit.py` da además
`CreateMixCtls__14NFSMixMapState  ok  448 B, 6 ramas, 12 relocs, 0 literales`.

`frozen.py cong Speed/Indep/SourceLists/zEAXSound2` → **`b02187667d939040`**.

La medida se tomó **revirtiendo mi fichero a HEAD, reconstruyendo, midiendo,
reaplicando, reconstruyendo y volviendo a medir** (`base2.json` → `final.json`),
porque hay otros agentes tocando el árbol. El delta es exactamente 448.

---

## 1. `CreateMixCtls` 97,32143 → **100,00000 %** — y la barrera se retira

### Lo que la r29 dejó escrito

*«la barrera la lleva de 97,32 a 99,91 % (19 → 2 filas) — el `n++` del `for` se
hundía 5 ranuras»*, y las 2 filas que faltaban eran

```
objetivo   lwz r3, 0x8(r9)   /  neg r3, r3
nuestro    lwz r0, 0x8(r9)   /  neg r3, r0
```

con **25 ensayos** por el lado de la sentencia del `neg` y el diagnóstico
*«al fundir los dos pseudos se corre la numeración de cantidades de
`local_alloc`; falta reponer un pseudo sin emitir código»*.

### Lo que era en realidad

**Reproducido primero** (`g6_bar` 99,91071 %, 2 filas; `g6_two` 99,82143 %, 4
filas — las cifras de la r29 al dígito). Luego, en vez de buscar el pseudo, miré
la ESTRUCTURA: el objetivo salta con un `b` a un `bl` compartido, que es la firma
del **cross-jump** (HERRAMIENTAS §8: *«un bloque en medio de la función es
cross-jump, no un `goto`: sólo sale si la fuente tiene DOS copias idénticas y
`jump.c` funde la cola»*).

```c
-            {
-                int ntmp;
-
-                if ((pparams->nUScaleCntSwing & 0x8000) == 0) {
-                    pmcp->psdata->nOffset = pparams->nUScaleCntSwing & 0x7FFF;
-                    ntmp = -pmcp->psdata->nOffset;
-                } else {
-                    ntmp = pparams->nUScaleCntSwing | 0xFFFF0000;
-                }
-
-                pmcp->psdata->nRatio = 0x7FFF - NFSMixShape::GetQ15FromHundredthsdB(ntmp);
-            }
+            if ((pparams->nUScaleCntSwing & 0x8000) == 0) {
+                pmcp->psdata->nOffset = pparams->nUScaleCntSwing & 0x7FFF;
+                pmcp->psdata->nRatio = 0x7FFF - NFSMixShape::GetQ15FromHundredthsdB(-pmcp->psdata->nOffset);
+            } else {
+                pmcp->psdata->nRatio = 0x7FFF - NFSMixShape::GetQ15FromHundredthsdB(pparams->nUScaleCntSwing | 0xFFFF0000);
+            }
```

Sin `ntmp`, el pseudo de la carga es LOCAL al bloque y `combine_regs` de
`local_alloc` lo funde con el `neg` → `lwz r3` + `neg r3,r3`. Y las dos copias de
la sentencia `nRatio` las funde `jump.c`, dejando el `b` del objetivo.

| ensayo | qué | cifra |
|---|---|---|
| `u1_dup` (aplicada, sin barrera como `w1`) | la llamada duplicada, `-pmcp->psdata->nOffset` inline | **100,00000** |
| `u2_dup_two` | duplicada + `ntmp` de dos pasos en la rama | **100,00000** |
| `u3_dup_var` | duplicada + `int ntmp = -…` en la rama | **100,00000** |
| `u4_dup_zero` | duplicada + `0 - …` | **100,00000** |
| `u5_dup_bothv` | duplicada + variable en las dos ramas | **100,00000** |
| **`w1_dup_nobar`** | **`u1` SIN la barrera** | **100,00000** |
| `w2`, `w3` | duplicada + el `n++` movido al cuerpo | **100,00000** |

**Cinco formas distintas dan 100 %, y la barrera es irrelevante en las siete.**

### Las dos reglas que salen de aquí

1. **`matched_code` es todo-o-nada, así que una mejora de 19 → 2 filas que «no se
   aplica» es una señal de que el diagnóstico está en la capa equivocada.** La
   barrera arreglaba 17 filas que eran CONSECUENCIA; la causa era una sola.
2. **Cuando el objetivo tiene un `b` a un `bl` compartido, prueba la sentencia
   duplicada ANTES de tocar el asignador.** Aquí costó **8 ensayos**; el enfoque
   del asignador llevaba 25 sin cerrar.

---

## 2. `ProcessUpdate__CARSFX_Turbo` (672 B) — VETO con los números de `sched`

Era «la primera que probaría el siguiente» y la única sin ensayos. **19 ensayos y
un veto medido: no se puede desde el fuente.**

### El diagnóstico

Las 7 filas salen de **una** insn: el objetivo emite `mr. r7, r3` (el
`x = Az` + `if (x < 0)` de `SetAzimuth`, `TURBO.h:100`) en el índice **30** y
nosotros en el **39**. Con el `mr.` arriba, r3 muere pronto y el objetivo reusa
r3 para `srawi r3, r11, 15` y para `TmpBlowoffVol`; nosotros gastamos r30 y r9.

### El veto, del propio compilador

`cc1plus -dS -fsched-verbose-5` (HERRAMIENTAS §8-bis) imprime la tabla del bloque:

```
;;   --- Region Dependences --- b 4 bb 0
;;      insn  code  bb  dep  prio  cost   units
;;       89   512    0    0    23     2   lsu       <- lwz BlowoffVol
;;       91    94    0    1    21     4   [iu2 imuldiv]
;;      109   512    0    0    14     1   iu2       <- lis 0x4330
;;      130   514    0    0     2     1   [iu iu2]  <- el mr.  <<<<
;;      131   763    0   23     1     1   bpu       <- el bge
```

**`INSN_PRIORITY` del `mr.` es 2** porque su único dependiente es el salto final
del bloque (`prio(130) = insn_cost + prio(131) = 1 + 1`). El planificador de
listas coge por prioridad descendente, así que el `mr.` es **estructuralmente el
último** del bloque:

```
Ready list (t = 1):  130 125 501 499 101 110 109 494 89   -> 89, 494
Ready list (t = 6):  130                                  -> 130
```

Para que salga en la ranura 2, su prioridad tendría que ser ≥ 15, o sea que su
resultado alimentara una cadena de 15 insns **dentro del mismo bloque básico**.
No hay forma de fuente que haga eso: r7 sólo lo consumen los `bge`/`cmpw` de las
ramas del clamp, que están en OTROS bloques.

### Ensayos (19), todos idénticos o peores

| ensayo | qué se barrió | cifra |
|---|---|---|
| `q1`,`q3` | `__asm__("")` / `"memory"` detrás de `SetAzimuth` | 96,30952 (10 filas) |
| `q2` | barrera delante de `SetAzimuth` | 96,10119 (15) |
| `q4`,`q5` | barrera detrás/delante del `>> 15` | **91,13 (27, +5 insns)** · 97,32143 (12) |
| `q6` | barrera detrás de `TmpBlowoffVol` | 96,10119 (15) |
| `q7` | `SetAzimuth` antes de `TmpBlowoffVol` | **79,97 (49, +14 insns)** |
| `q8` | `SetAzimuth` antes del `>> 15` | 84,04 (40) |
| `q9` | `SetAzimuth` la última | 92,89 (24) |
| `qa`,`qb`,`qe`,`qg`,`qh` | `Az` a variable · decl. del `Tmp` antes · `GetValue()` a variable · float intermedio · copia de `Az` | **idénticas** 97,50000 |
| `qd`,`qj` | variable nueva en vez de reasignar `nDMixOut` · shift inline | 97,05357 (19) |
| `qf` | orden de operandos del producto | 96,72619, **668 B** |
| `qi` | el puntero a una variable local | 94,97, **664 B** |
| `qc` | `SetVolume` con la expresión inline | 79,97 |

**Veda: `ProcessUpdate` es muro del planificador. Ninguna forma de fuente puede
subir una insn cuyo único consumidor es el salto del bloque.** Y esto se
generaliza: **antes de barrer una insn «que llega tarde», mira su `prio` en
`-fsched-verbose-5`; si es 2, no hay nada que hacer.**

---

## 3. `RenderString` (1.572 B) — los umbrales de la r27, VERIFICADOS y REFUTADOS

Era «el mayor premio suelto». **El volcado RTL que la r29 dejó pendiente está
hecho, y dice que la hipótesis era falsa.**

### Los números, ahora del `.greg`

`regmap.py` y `dwbody.py` confirman lo estructural: **mismo conjunto de locales,
mismo árbol de inlines, mismos bloques**; sólo cambian **4 registros**:

| local | original | nuestro |
|---|---|---|
| `pcString` | **r26** | r27 |
| `matrix` | **r27** | r26 |
| `c` | **r27** (reusa el de `matrix`) | r28 |
| `pGlyph` | **r28** | r29 |

`scripts/alloc.py` sobre el `.greg` (76 allocnos llegan a `global_alloc`):

```
   #    pseudo  n_refs  live_len  pri     reg
   30   143     20      95        8421    r29   <- pGlyph
   36    82     37      383       4830    r31   <- this
   37   133     28      233       4806    r28   <- c
   38    84     34      357       4761    r27   <- pcString
   43   360      6       47       2553    r26
   60    86      2       46        434    r26   <- matrix
```

Los tres umbrales que la r27 dejó escritos **se verifican al dígito**:
`c` = 4806 contra `pcString` = 4761; con 35 refs `pcString` daría **4901**; con
`live_len` ≤ 353 daría 4816; con `live_len(c)` ≥ 236 `c` bajaría a 4745.

### La refutación

Aplicada la palanca nueva del brief (**el asm no volátil como referencia**),
`__asm__("" : "=r"(pcString) : "0"(pcString))` dentro del bucle (`x1`):

```
   36    84     38      358       5307    r31   <- pcString: SUBE de #38 a #36...
   37    82     37      384       4817    r27   <- ...pero se lleva r31, no r26
   38   133     28      234       4786    r28   <- c sigue en r28
```

**El orden cambia exactamente como predice la fórmula, y el resultado es PEOR**
(93,04326 %, 107 filas): `pcString` se lleva **r31** y desplaza a `this` a r27.
**`find_reg` no reparte por orden: reparte por la razón
`local_reg_n_refs[reg]/local_reg_live_length[reg]`, así que subir la prioridad no
baja el registro.** Y el objetivo tiene `pcString` en r26, el registro **MÁS
BAJO** del grupo — o sea que en el original se asignó **TARDE**, no pronto: la
dirección del umbral de la r27 está **al revés**.

| ensayo | qué | cifra |
|---|---|---|
| `x1_asmref_loop` | `"=r"/"0"` sobre `pcString` en el bucle | 93,04326 (107 filas) |
| `x2_kern_pcs` | `GetKern(pGlyph, pcString[-2])` en vez de `prevChar` | 91,42748, **1.580 B** |
| `x3_if_pcs` | `if (pcString[-2] != 0 && k != 0)` | 92,42239, **1.576 B** |
| `x4_w2_prev` | `prevChar` en vez de `pcString[-2]` en el `GetCharacterWidth` | 89,03562, **1.580 B** |
| `x5_prev_deref` | `*(pcString - 2)` | **idéntica** |

**Veda: `RenderString` no se abre subiendo referencias.** Lo que queda —y no lo
he probado— es el **modelo del registro duro**: `local_reg_n_refs[]` /
`local_reg_live_length[]` por registro, que decide r26 contra r31 y que sale del
`.lreg`. Es la única capa que no se ha mirado.

---

## 4. `Play__RoadNoise` (392 B) — la hipótesis de los pares accesor, MEDIDA Y MUERTA

La r29 dejó: *«al original le sobran ~33 líneas en `FX_ROADNOISE`, que son
exactamente DOS pares accesor/getter de 16 líneas»*. **Es falso, y se demuestra
sin compilar nada.**

Contando el paso entre `store` consecutivos en el volcado del objetivo
(`ENVIRO_AEMS.h` 337 → 369 → 385 → 401 → 417 → 449 → 481) el paso es **16 por
par**, y en nuestra cabecera (`SetId` 152, `SetVolume` 165, `SetPitch` 178,
`SetAzimuth` 191, `SetType` 204, … `GetRefCount` 295) es **13 clavado**. Del
`store` de `volume` al de `wet_FX` hay **9 pares en los dos lados**:
(481−337)/16 = 9 y (287−170)/13 = 9.

Y lo mismo en `TURBO.h`, medido aparte: `SetVolume` del objetivo con el `if` en
68 y el `store` en 74, `SetAzimuth` con el `if` en 100 y el `store` en 106 —
**+6 en vez de nuestro +5, y 16 líneas por par en vez de 13**.

**No faltan accesores: al original le sobran 3 líneas POR PAR, que son el estilo
de llaves (`}` y `else if` en líneas distintas). Es cosmético y no mueve el
inline.** El diagnóstico del brief §3.1 (distancia apertura→store = 7 con dos
clamps) se confirma en las dos cabeceras: los clamps están todos.

Los 7 ensayos nuevos confirman la veda de la r29 («cualquier variable rompe el
plegado»):

| ensayo | qué | cifra |
|---|---|---|
| `a1`,`a2` | `static_cast<…TypeType>(0)` / `(…TypeType)0` | **idénticas** 94,93877 |
| `a5`,`a6` | `GetRefCount()` sin variable · sin el bloque anónimo | **idénticas** |
| `a3_zvar_va` | los ceros de `volume`/`azimuth` a variable | **76,38776 %, 432 B** |
| `a4_zvar_rest` | los cinco ceros restantes a variable | **58,16327 %, 476 B** |
| `a7_ptr` | puntero local para el `new` | 88,82653, **376 B** |

**Lo que falta sigue siendo partir el `li 0` en dos SIN meter una variable.**

---

## 5. `ActualReadJoystickData` (1.588 B) — 8 ensayos, 2 vedas nuevas

Somos **1.580 B** contra 1.588; faltan `mr r9, r0` y `andi. r11, r9, 0x8000`.
Las 19 filas son **tres** problemas independientes:

1. **r18/r19 permutados** (`lis RealTimer@ha` contra `lis 0x4330`) — 6 filas.
2. **el orden de operandos del `add`**: objetivo `add r31, r29, r27` /
   `sthx r0, r29, r27`, nuestro `add r31, r27, r29` — 3 filas.
3. **el racimo del 4.º `andi.`** — 8 filas + las 2 insns que faltan.

### Veda nueva y medida: el orden del `add` NO sale del fuente

| ensayo | qué | cifra |
|---|---|---|
| `z1_slotfirst` | `(slot + (LGPosition *)(void *)joy_data)` (las 10 apariciones) | **idéntica** 99,24433 |
| `z2_index` | `&((LGPosition *)(void *)joy_data)[slot]` (las 10) | **idéntica** |
| `z3_hoist` | el puntero a una local | 95,53904 %, **1.556 B**, 170 filas |

**`fold` canonicaliza el `PLUS_EXPR` antes de expandir: escribir `índice + base`
da el MISMO objeto que `base + índice`.** (Es el hermano de la veda del brief §4
sobre flotantes.) Cualquier barrido que permute los operandos de un `+` de
punteros tira el presupuesto.

### El racimo del `andi.` — 5 formas más, todas en la trampa del tamaño

| ensayo | qué | cifra |
|---|---|---|
| `y2`,`y3`,`y5` | dos variables · `v` primero y cast al almacenar · `short sx` | **idénticas** 99,24433 (19 filas) |
| `y1_storecast` | `joy_data->stickX = (short)data; v = (short)data;` | 99,05541, **1.576 B**, 15 filas |
| `y4_storedata` | almacenar `data` y probar `(short)v` | 98,79093, **1.572 B**, 16 filas |

Igual que los `p1`/`p2`/`p6` de la r29: **menos filas con MENOS tamaño no es
avance** — nos alejamos de los 1.588 B.

**Lo que sigue sin probarse**: la combinación `s8_calCL` + devolver el `li r21,1`
a su sitio (el cabo suelto de la r28), y el racimo del `andi.` desde la presión
de registros. Y ahora hay un dato nuevo: **el resultado del `andi.` que falta es
la fuente (basura) de los cinco `mr` de las expansiones `*floatsidf2_loadaddr` de
las líneas 317/318** — o sea que el `andi.` sobrevive en el objetivo porque su
destino lo pilla ese pseudo artificial, no porque haga falta.

---

## 6. `BindToData` (340 B) — la barrera en la dirección no probada, 8 ensayos

El `-1` de `mCurrentBlock` llega **PRONTO** (índice 33 nuestro contra 51 del
objetivo) y por eso roba r8 a `mCyclePos`. Como llega pronto, la barrera va
**DELANTE** — dirección que la r29 no probó (sus 20 formas eran posiciones de
sentencia).

| ensayo | qué | cifra |
|---|---|---|
| `b1_tot_before_cur` | `__asm__("")` delante de `mCurrentBlock = -1` | 92,49412 %, **340 B exactos**, 13 filas, 88 insns |
| `b2_mem_before_cur` | `"memory"` delante | 85,55294, 352 B |
| `b3_tot_after_cur` | barrera detrás | 90,98824, 19 filas |
| `b4`,`b5` | barrera delante de `mCyclePos` / `mSampleData` | 76,50 · 83,60 |
| `b6_tot_after_min` | barrera detrás de `minperiod` | 93,10588 |
| `b7`,`b8` | el `-1` detrás de `minperiod` · el `-1` desde una local | **idénticas** 95,88236 |

`b1` da **el tamaño exacto del objetivo (340 B) y EMPEORA**: es el noveno caso
documentado de la trampa del §5 de HERRAMIENTAS. **Veda: la barrera no vale en
`BindToData` en ninguna de las dos direcciones.**

---

## 7. `ToggleCapsLock` (92 B) — diagnóstico nuevo, 8 ensayos más (van 30)

La única diferencia es que el objetivo emite `stw r10, 0x3c(r3)` (`mbShift`)
**entre** el `cmpwi r9, 0x3` y su `bne`, y nosotros delante del `cmpwi`.

**Diagnóstico nuevo**: los dos son de prioridad ~2 en `sched2`
(el `stw` no tiene dependientes; el `cmpwi` sólo alimenta el `bne`), así que
**el desempate es `INSN_LUID`, o sea el ORDEN DE FUENTE**, y en el objetivo el
`cmpwi` tiene LUID menor que el `stw`. Eso exige que la comparación
`mnMode == MODE_FILENAME` se evalúe ANTES del `mbShift = false`, y las dos formas
que lo consiguen destruyen la función:

| ensayo | qué | cifra |
|---|---|---|
| `c1`,`c7` | `__asm__("")` delante / detrás de `mbShift = false` | 89,56522 · 89,78261 |
| `c2` | barrera delante de `mbCaps` | 89,78261 |
| `c3`,`c4` | **`mbShift = false` DUPLICADA en las dos ramas** (truco del cross-jump) | 69,04 · 69,09, **104 B** |
| `c5` | `mbShift` después del `if` | 71,17, 100 B |
| `c6` | `mbCaps` con `if/else` explícito | 76,70, 96 B |

**El truco que cerró `CreateMixCtls` aquí no vale**: no hay cola común que fundir
(`jump.c` necesita dos colas IDÉNTICAS, y aquí la rama `then` lleva además
`mbCaps = true`).

---

## 8. Lo que NO he probado

- **`RenderString`: el modelo del REGISTRO DURO.** El orden de allocnos está
  resuelto y verificado; lo que decide r26 contra r31 es
  `local_reg_n_refs[regno] / local_reg_live_length[regno]` de `find_reg`, que
  sale del `.lreg` y que **nadie ha mirado nunca**. Es la única capa viva de la
  función más grande del encargo.
- **`ActualReadJoystickData`**: sigue sin probarse `s8_calCL` combinado con
  devolver el `li r21,1` a su sitio, y el racimo del 4.º `andi.` por presión de
  registros. Tampoco he tocado el par r18/r19 (6 de las 19 filas).
- **`Play__RoadNoise`**: partir el `li 0` en dos **desde la cabecera**
  (`ENVIRO_AEMS.h`), que es donde tiene que estar la causa una vez descartados
  los pares accesor. Es cabecera compartida: exige A/B por objetos sobre todas
  las unidades que la incluyen.
- **`MsgPlayMiscSound` (780 B, «28 registros») y `CreateSubMixChannels`
  (336 B, «16 registros»): ni un ensayo**, tercera ronda seguida. Sólo leídas por
  `triage`.
- **`GenerateRoadNoise` (1.240 B)** y **`Setup` (596 B)**: ni un ensayo nuevo;
  las vedas de la r29 (14 y 16 formas) siguen en pie y `Setup` está cerrada por
  construcción.
- **`IconScroller` (384 B)**: ni un ensayo nuevo; las 43 formas de la r29 siguen.

---

## 9. Ensayos numerados (65)

- **`CreateMixCtls` (10)**: `g6_bar` 99,91071 · `g6_two` 99,82143 ·
  **`u1`…`u5` los cinco 100** · **`w1`…`w3` los tres 100 SIN barrera**.
- **`ProcessUpdate` (19)**: `q1`…`qc` (12), `qd`…`qj` (7). §2.
- **`RenderString` (5 + el volcado RTL)**: `x1`…`x5`. §3.
- **`ActualReadJoystickData` (8)**: `y1`…`y5`, `z1`…`z3`. §5.
- **`Play__RoadNoise` (7)**: `a1`…`a7`. §4.
- **`BindToData` (8)**: `b1`…`b8`. §6.
- **`ToggleCapsLock` (8)**: `c1`…`c7`. §7.

## 10. Herramientas, convivencia y estado del árbol

- **`c27mx_run.py` de la r27 sigue siendo la herramienta** (5-30 s por variante).
  Stubs que hay que saber, añadido a lo que ya documentó la r29:
  `CARSFX_Turbo.cpp`, `CARSFX_Roadnoise.cpp`, `NFSMixMapState.cpp`,
  `ginsudata.cpp` y `FEngFont.cpp` sólo piden el `-I` de su propio directorio;
  `JoyE.cpp` pide `Timer.hpp`; `FEpkg_MU_Keyboard.cpp` pide
  `ANIMWORLDTYPES_EMIT_MESSAGE_HASHES` + `ATTRIB_NO_INLINE_CLASSKEY` +
  `FEngInterfaces/FEngInterfaceFEObjects.hpp`.
- **Volcados RTL**: `c30mx_rtl.py` (copia del `c27ae_rtl.py` de otro agente con
  el directorio de salida renombrado). Dos avisos medidos: **con `-da` completo
  `cc1plus` da ICE en `_tree.c:639` DESPUÉS de emitir los volcados** (los
  ficheros están bien; el `rc=33` es ruido), y **`alloc.py` necesita `.lreg` Y
  `.greg`: con `-dg` solo revienta con un traceback**. Usa `gl`.
- **`fndiff.py` de `scripts/` funciona; el formateador correcto del JSON es
  `e['instruction']['formatted']`**, no `e['arguments']`.
- **Finales de línea, medidos**: `CARSFX_Turbo.cpp`, `NFSMixMapState.cpp`,
  `ginsudata.cpp` y `FEpkg_MU_Keyboard.cpp` **CRLF puro**; `JoyE.cpp` **LF puro**.
- **Disco**: entré con 15 GB y salgo con 14 GB; borrados mis `c30mx_rtl_*`,
  `o_*.o`, `v_*.cpp`, `stub_*.cpp` y los `d_z*.json` de `fndiff` (100 MB).
  El scratchpad tiene 2,2 GB de otros agentes (`c30cam_*`, `c30wag_*`,
  `c27ae_*`): **no los he tocado**.
- **ESTADO DEL ÁRBOL**: además de mi fichero, `git status` trae
  **`src/Speed/Indep/Src/Frontend/MenuScreens/InGame/FEpkg_MU_Keyboard.cpp`
  modificado por OTRO agente** (dos `extern` convertidos en definiciones,
  `gFEKeyboard` y `KeyboardActive` — el frente de `.data` del brief §2). **No es
  mío y no lo he revertido**; si al enlazar salen símbolos duplicados, viene de
  ahí. Mi medida no está contaminada por él: entre `base2` y `final` sólo
  reconstruí zEAXSound2.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, `symbols.txt`,
  `build.ninja` ni los bloques `__ANDROID__`. **Sin commit.**
