# Ronda 28 — mx: zEAXSound2 / zFe2 / zSpeech / zPlatform

**DOS funciones cerradas (676 B), dos mejoradas y aplicadas, y CUATRO reglas de
fuente nuevas, todas extrapolables.** `measure.py --cmp`: **+676 B, +2 funciones**.
`pctsnap.py --cmp`: **EMPEORAN: ninguna, MEJORAN 4.**

| función | unidad | antes | después | B |
|---|---|---|---|---|
| **`UnlockPalette`** | zPlatform | 99,41860 | **100,00000** | **172** |
| **`Manager::NotifyEventCompletion`** | zSpeech | 98,37302 | **100,00000** | **504** |
| `ActualReadJoystickData` | zPlatform | 97,35516 | **99,24433** | (41 → 19 filas) |
| `IconScroller::IconScroller` | zFe2 | 95,47916 | **97,77084** | (18 → 11 filas) |

Ficheros del árbol tocados: **CUATRO**, sin commit —
`Ecstasy/TextureInfoPlat.cpp`, `GameCube/Src/JoyE.cpp`,
`EAXSound/Stream/SpeechManager.cpp`,
`Frontend/MenuScreens/Common/feIconScrollerMenu.cpp`.

---

## 0. Verificación del encargo y auditoría

`build_direct.py zEAXSound2 zFe2 zSpeech zPlatform` → 4 ok.
`triage.py … --muro` reproduce el encargo **exactamente** (4 en «falta o sobra»,
14 de muro, 7.740 B).

| pasada | zPlatform | zSpeech | zEAXSound2 | zFe2 |
|---|---|---|---|---|
| entrada (`audit1_*`) | 134 ok / **0 FALLA** | — | — | — |
| cierre (`audit2_*`, `audit3_zFe2`) | **135 ok / 0 FALLA** | **699 / 0** | **922 / 0** | **1304 / 0** |

Cero FALLA en las cuatro unidades, así que no hubo nada que confirmar con una
segunda pasada. `frozen.py cong` sobre **zPlatform** (`e43f8d75fe7b0263`) y
**zSpeech** (`95d58e6c4e344c2c`), las dos que cambian de cuenta de funciones.

La medida se tomó **revirtiendo mis cuatro ficheros a HEAD, reconstruyendo y
volviendo a medir**, no contra una base vieja (había otros nueve agentes
tocando el árbol al mismo tiempo). El delta es exactamente 172 + 504.

---

## 1. LA REGLA DEL TEMPORAL SEPARADO — cierra `UnlockPalette` (172 B)

Tu predecesor barrió **24 permutaciones del `|` más 34 formas de expresión** y se
quedó clavado en **2 filas**: `fold` canoniza el nodo con la constante como
operando **derecho** y el objetivo lo tiene a la **izquierda**.

**La forma que faltaba no es una permutación: es sacar el subárbol a un temporal
que NO sea la variable de destino.**

```c
     if (a > 0xEF) {
-        result = 0xFFFF8000 | (b >> 3) | ((r >> 3) << 10) | ((g >> 3) << 5);
+        unsigned int t = 0xFFFF8000 | ((r >> 3) << 10);
+        result = t | ((g >> 3) << 5) | (b >> 3);
     }
```

**99,41860 → 100,00000 %, 0 filas, 172 B, 43 insns.** A la primera compilación.

### Por qué funciona, y por qué las 34 formas anteriores no

El objetivo es `((C|r)|g)|b` con el acumulador intermedio en **r0** (un temporal)
y sólo el `or` final escribiendo en **r11** (`result`):

```
objetivo   or r0,  r0, r11      <- ((C|r) | g)   en un TEMPORAL
           or r11, r0, r9       <- | b           -> result
```

Tu predecesor probó esa misma agrupación **usando `result` como acumulador**
(`result = C|r; result |= g; result |= b`). Eso hace que `result` (r11) sea el
acumulador **desde el primer término**, y las dos filas no se mueven. Con un
temporal distinto, `fold` no reasocia (los tres operandos de `t | g | b` son no
constantes → asociación por la izquierda), y el asignador da r0 al temporal
porque muere en el `or` final.

**Regla: cuando el objetivo acumula en un registro que NO es el de la variable de
destino, el original tenía un temporal, no un `|=` sobre el destino.** Es la cara
complementaria de la regla del §1.2 de la r27 (el orden del `|`): primero se
arregla el orden, luego se saca el acumulador.

Barrido completo (`c28mx_tex1.py`, 18 variantes, ~2 s cada una):

| variante | % | filas |
|---|---|---|
| `t1_tmpCr` `t = C\|r; result = t\|g\|b` | **100,00000** | **0** |
| `t3_tmpCr_top` (`t` declarada arriba) | **100,00000** | **0** |
| `t4_tmpCr_oreq` (`t \|= g` y `result = t\|b`) | **100,00000** | **0** |
| `t5_tmpCrg_oreq` | 99,76744 | 2 |
| `t2_tmpCrg` `t = C\|r\|g; result = t\|b` | 98,25581 | 5 |
| `p1_paren` `((C\|r)\|g)\|b` | 98,25581 | 5 |
| `p2_paren_tmp` | 98,25581 | 5 |

**Veda medida — `0xFFFF8000` como variable NO era el camino** (era lo único que
la r27 dejó sin probar). Las nueve formas `v1`…`v9` con
`unsigned int k = 0xFFFF8000;` dan **87,5-93,9 % y 172-176 B con 44-45 insns**:
se materializa la constante aparte y sobra una instrucción. `w1_ktmp` 93,95,
`w2_koreq` 92,21. **Esa vía queda cerrada.**

---

## 2. LA REGLA DEL TERNARIO — cierra `NotifyEventCompletion` (504 B)

Las **2 filas** eran un `lis _Q26Speech7Manager.mLastSpeakerID@ha` que el
objetivo tiene en el **bloque dominante** (antes del `cmplwi`/`ble`) y nosotros
**dentro del cuerpo del `if`**.

```c
-    if (speakerID > 1) {
-        mLastSpeakerID = speakerID;
-    }
+    mLastSpeakerID = (speakerID > 1) ? speakerID : mLastSpeakerID;
```

**98,37302 → 100,00000 %, 0 filas, 504 B, y una instrucción MENOS (126 vs 127).**

### El mecanismo, y cómo detectarlo sin barrer

En `x = cond ? y : x;` GCC expande **el lvalue primero**: la dirección
(`high(sym)`) se emite **antes** de evaluar la condición, así que el `lis` cae en
el bloque dominante. Con `if (cond) x = y;` la dirección se expande dentro del
cuerpo. **Ningún pase mueve ese `lis` después**: con una sola ocurrencia ni
`gcse`/PRE ni `hoist_code` tienen material.

**El delator está en `lmap.py`, y es barato:**

```
8028E664  lis r9, mLastSpeakerID@ha   SpeechManager.cpp:1917
8028E668  cmplwi r5, 0x1
8028E66C  ble .L_8028E674
8028E670  sth r5, mLastSpeakerID@l(r9)
```

**El `lis`, el `cmplwi`, el `ble` y el `sth` están TODOS en la línea 1917.** Un
`if` con llaves ocuparía 3 líneas; un ternario ocupa una. Cuando el mapa de
líneas mete una condición y su cuerpo en la misma línea **y** hay una dirección o
un valor calculado antes del `cmp`, **el original tenía un ternario**.

Las otras siete formas dan todas **98,37302 %** (idénticas): una línea sin
llaves (`n1_oneline`), `>= 2` (`n3`), `!(x <= 1)` (`n7`), calificar con
`Manager::` (`n8`), `speakerID = 0` en la declaración (`n5`). **Sólo el ternario
mueve el `lis`.** (`mLastSpeakerID` es `short`, no `unsigned short`: las
variantes con `unsigned short *`/`&` no compilan.)

---

## 3. UNA VARIABLE POR USO — `ActualReadJoystickData` 97,355 → **99,244 %**

Tres bloques de recorte idénticos comparten en nuestro fuente **una sola**
variable `data`. El objetivo les da **tres registros distintos** (r8, r10, r11);
nosotros uno (r5), porque *una variable = un pseudo = un registro*.

```c
                 int data;
                 int v;
                 …
-                data = v;
-                if ((short)data > 255) { data = 255; }
-                joy_data->substickX = data;
+                {
+                int data = v;
+                if ((short)data > 255) { data = 255; }
+                joy_data->substickX = data;
+                }
```
(los cuatro bloques, `v` se queda fuera)

**97,35516 → 97,53149 %, 41 → 29 filas, mismo tamaño.** El primer recorte pasa a
casar entero. `k1_numed` (`data1`…`data4` en el mismo ámbito, sin llaves) da
**exactamente lo mismo**: lo que cuenta es que sean cuatro DECL, no el ámbito.

Encaja con el hallazgo de DWARF de la r27 («el original declara `short data` y no
tiene `v`»): **el `data` del original es nuestra `v`; los temporales del recorte
no son variables declaradas**, y por eso el objetivo los reparte libremente.

**Veda: `short` en los cuatro** (`k3_scoped_short`) **96,34761 % y 1.564 B** —
sigue coalesciendo, como en la r27.

---

## 4. EL ORDEN DE LAS SENTENCIAS DEL BLOQUE — 29 → 19 filas, y +2,29 pp en `IconScroller`

El bloque `else` de `ActualReadJoystickData` (rama sin volante) tiene **cuatro
asignaciones independientes**. Permutarlas son **24 variantes = 2 min**:

| orden | % | filas | insns |
|---|---|---|---|
| **`W N F T`** (wasWheel, notYet, field8, type) | **99,24433** | **19** | 397 |
| `N W F T` | 99,21915 | 21 | 397 |
| `F * * *` (5 órdenes) | 98,74056 | 21 | 398 |
| `T W N F` = **el orden original nuestro** | 97,53149 | 29 | 400 |
| `N T F W` | 97,36776 | 37 | 400 |

**La diferencia entre el mejor y el peor orden es 1,88 pp y 18 filas**, con las
MISMAS cuatro sentencias. El planificador **no** conserva el orden de fuente
(prioridad y `INSN_REG_WEIGHT` mandan antes que el LUID), así que **el orden
bueno hay que barrerlo, no razonarlo**: nuestro orden «natural» era el peor de
los 24.

Aplicado. Con eso el bloque de `PADReset` casa **entero** (16 instrucciones).

Lo mismo en el constructor de `IconScroller` (15 asignaciones): mover
`AlignmentToSelected`, `iCurSelectedIndex` y `fHeight` al principio da
**95,47916 → 97,77084 %, 18 → 11 filas** (`q4_hgfirst`, aplicado). Aviso: **es un
reorden sin respaldo del original** — el mapa de líneas no sirve aquí porque el
planificador mueve las instrucciones y `lmap` les cuelga la línea de la última
nota que las precede, así que **todas** salen en `feIconScrollerMenu.cpp:485`.
Revertir es cambiar el orden de vuelta a
`NB CF MF AC IC FC AL CS HG XC YC AP WD BL SP`.

---

## 5. `ActualReadJoystickData`: lo que queda (19 filas, 4 racimos)

```
40/41    lis r18,RealTimer@ha / lis r19,0x4330   <- r18 y r19 cambiados
158-165  recorte 2: objetivo r10, nuestro r8
184-189  recorte 3: objetivo r11, nuestro r8
190/192  mr r9,r0 + andi. r11,r9,0x8000          <- FALTAN las dos
310/334/352  add r31,r29,r27 / sthx r0,r29,r27   <- operandos del `+` al revés
```

- **El racimo r18/r19 se arregla solo** con el orden
  `calibrationTimer = 7.0f; lastCalibTime = …; wasWheelConnected = 1;`
  (`s8_calCL`: **15 filas**), pero entonces el `li r21, 0x1` se planifica tarde y
  aparecen 2 filas nuevas con **+1 instrucción** (398 contra 397). Neto:
  98,81612 % contra 99,24433. **No aplicado**; es el mejor cabo suelto.
- **Las dos instrucciones que faltan (190/192)**: el objetivo hace `mr r9, r0`
  (copia de `v`) y luego `andi. r11, r9, 0x8000`, y usa **r11** como origen del
  cero para los cinco `mr` siguientes; nosotros reusamos el `andi.` del recorte
  anterior (r9) y `flow` borra el cuarto. **Sospecha fuerte: los racimos 2, 3 y
  este son la misma causa** — si naciera el cuarto `andi.`, r11 quedaría ocupado
  y el reparto de los recortes 2/3 se movería.
- **El orden del `+`**: `pointer_int_sum` pone SIEMPRE el puntero primero, así
  que **no se toca desde el fuente**. Seis formas medidas idénticas
  (`(slot + ptr)`, `&ptr[slot]`, `(*(ptr+slot))`, `(LGPosition*)((int)p + slot*sizeof)`,
  `((LGPosition*)(void*)(joy_data+slot))` 20 filas). Quien lo decide es `loop`
  al reconstruir el GIV. `m8_noslot` (quitar `slot`) da 93,02 % y 185 filas:
  el `add` es real, no cosmético.

---

## 6. Vedas medidas, con la sentencia barrida

### `zEAXSound2::BindToData` (340 B, sobra 1 `mr`) — 20 formas, ninguna mueve nada

**El diagnóstico está cerrado**: quitar `this->mCurrentBlock = -1;`
(`h0_nocb`) da **97,50588 %, 332 B, 4 filas** y borra **tres** instrucciones (el
`li -1`, su `stw` y el `mr r8, r5`). O sea: **el pseudo del `-1` se queda con r8,
que es el registro que `mCyclePos` necesita dentro del bucle**, y de ahí sale la
copia. El objetivo le da **r0** porque nace tarde (justo detrás de `mr r7, r0`).

| ensayo | qué se barrió | cifra |
|---|---|---|
| `g1`,`ga` | `mCurrentBlock = -1` **detrás** de `minperiod` | 95,88236 (idéntico) |
| `g2`,`g3`,`g4` | delante de `mFreqPos` / `mSampleData` / `mCyclePos` | 95,80000 (peor) |
| `g5`,`g6`,`gb` | quitar `int i = 0;` y usar `for (int i = 0; …)` | 95,87059 |
| `g7` | `int i;` sin inicializar + `for (i = 0; …)` | 95,87059 |
| `g8` | línea en blanco antes de `minperiod` (la que tiene el original) | idéntico |
| `g9` | `int cb = -1; mCurrentBlock = cb;` | idéntico |
| `gc` | `int i = 0;` justo antes de `mFreqPos` | idéntico |
| `h1` | puntero local `int *cp = mCyclePos` en el bucle | 95,70588 |
| `h2` | `if (minperiod > period)` | 95,70588 |
| `h3` | sin la variable `period` | idéntico |
| `h4` | `mCyclePos` por variable local `cyc` | idéntico |
| `h5` | `mCurrentBlock = ~0;` | idéntico |
| `h7` | `while` en vez de `for` | idéntico |

**La posición de la sentencia es irrelevante**: 12 posiciones distintas del
`-1` dan el mismo objeto. Lo que decide es la planificación, y no se mueve desde
la fuente.

Y el **mapa de líneas del original** (`ginsudata.cpp` 163-175) dice que el orden
de sentencias del original **es exactamente el nuestro**, con dos diferencias de
formato: no tiene `int i = 0;` suelto (usa `for (…; i < mCycleCount; i++)`) y sí
tiene una línea en blanco entre `mCurrentBlock = -1;` y `int minperiod`. Las dos
medidas, las dos neutras o peores.

### `zSpeech::LoadSpeechBank` (316 B, falta 1 `mr`) — 6 formas

El objetivo mantiene **dos** registros con `index->item` (`mr r11,r4` en la
precabecera, `mr r4,r11` **dentro** del bucle, redundante) y nosotros uno.
`lmap` confirma que las dos copias son de las líneas 970 y 981 del original, o
sea del `if` exterior y del `if (…key == key) goto found;`. Es `loop`/`regmove`.

| ensayo | qué | cifra |
|---|---|---|
| `l1_noblank` | quitar la línea en blanco tras las declaraciones | idéntico |
| `l2_gotoline` | `if (…) goto found;` en una línea (como el original) | idéntico |
| `l4_ptrform` | `(index->item + i)->key` | **88,48101 %**, 320 B |
| `l5_forloop` | `for (; cond; )` en vez de `while` | idéntico |
| `l6_keyvar` | `unsigned int k = index->item[i].key;` | idéntico |

### `zSpeech::RoadblockFlow::Setup` (596 B, 2 filas) — 6 formas, ninguna mueve

Las 2 filas son las ranuras de `lha`/`lwz` y `mtlr`/`add` de la llamada virtual
`primary->PursuitApproaching()`. **El propio original tiene los DOS órdenes**:
en `RoadblockFlow.cpp:400` (`cop_in_rb->RBApproach()`) emite `lha` primero —
igual que nosotros—, y en la 419 emite `lwz` primero. La diferencia es de
contexto (en la 419 hay un `bl RBUpdate` delante), no de la sentencia.

`r1_disp` (`EAXDispatch *disp = ai->GetDispatch();`), `r3_pvar` (copia de
`primary`), `r4_oneline` (`if (primary == 0) return;`), `r5_thisarg`
(`(*primary).PursuitApproaching()`), `r6_neq` (`if (!primary)`): **idénticas**.
`r2_ifnot` (`if (primary != 0) { … }` en vez de `return` temprano): 98,22147 (peor).

### `zFe2::ToggleCapsLock` (92 B, 2 filas) — 7 formas más (van 15)

`c1_shift0` (`mbShift = 0;`), `c3_tmp` (`int z = 0; mbShift = z;`),
`c4_capstern`/`c7` (`if (…) mbCaps = true;` en una línea),
`c6_this` (`this->mbShift`): **idénticas, 91,08696 %**.
`c2_tern` (`mbCaps = (mnMode == MODE_FILENAME) ? true : mbCaps;`) **76,52 %, 96 B** —
aquí el ternario **empeora**: el `if` sí es la forma del original.
`c5_shiftlast` (`mbShift = false;` detrás del `if`) **71,17 %, 100 B**.

### `zPlatform::ActualReadJoystickData` — 8 formas más de los recortes

`u1_storein` / `u2_vfirst` / `u5_sh` (guardar `joy_data->stickX` **dentro** del
ámbito del recorte 3) dan **15 filas pero 1.576 B**: quitan una instrucción y se
alejan del objetivo (1.588). `u3_wvar`, `u4_inscope`: idénticas.
`u6_nostore` (mover el `stb` detrás del `if`): 98,11 %, 1.592 B, 45 filas.
`m3_calswap`, `s6`, `s7` (órdenes de calibración): peores.
`s1_uslot` (`unsigned int slot`), `s2_slotinit` (`int slot = 0;`),
`s5_calN`: idénticas.
`s3_lgvar` (cachear `(LGPosition*)joy_data + slot` en una variable):
**90,29 %, 129 filas** — el objetivo lo recalcula tres veces.

### `zEAXSound2::Play__RoadNoise` (392 B) — la vía del «cero partido», cerrada

Diagnóstico nuevo (§7) y **seis formas medidas**: sustituir los ceros literales
del constructor por una variable **rompe el plegado de los recortes** de cada
`SetXxx` y explota:

| ensayo | qué se barrió | cifra |
|---|---|---|
| `p1_zva` | `int zero = 0;` para `volume` y `azimuth` | **76,39 %, 432 B** (113 insns) |
| `p2_zvb` | `int zero = 0;` para `type`/`sec`/`speed`/`hi`/`wet` | **58,16 %, 476 B** |
| `p5_zeroall` | `zero` para los siete | **50,98 %, 524 B** |
| `p3_cast` | `(Csis::FXROADNOISETypeType)0` en vez del enumerado | idéntico |
| `p4_norefvar` | `GetRefCount()` sin asignar a `refcnt` | idéntico |
| `p6_ptrvar` | puntero local `rn` y `m_pRoadNoiseControl[side] = rn;` | 88,83 %, **376 B** |

**El cero hay que partirlo sin sacarlo de la expresión constante** — cualquier
variable lo mata. Queda por probar: partirlo por el **tipo** del argumento
(`type` es un enumerado y `volume`/`azimuth` son `int`).

### `zEAXSound2::ProcessUpdate__CARSFX_Turbo` (672 B) — 5 órdenes, todas peores

La única fila real es que el objetivo emite `mr. r7, r3` (el recorte de
`SetAzimuth`, `TURBO.h:100`) **como segunda instrucción del bloque** y nosotros
la novena, y de ahí sale `srawi r3` contra `srawi r30`. El mapa de líneas
confirma que **el orden de sentencias del original es el nuestro** (263 →
265 → TURBO.h:99/100). Mover `SetAzimuth` delante (`w1`, `w5`) da **84,04 % y
181 insns**; en medio (`w2`) **79,97 % y 184**; `SetVolume` antes que
`SetAzimuth` (`w3`) 92,89 %. Quitar la línea en blanco: idéntico.

### `zFe2::IconScroller` — 20 órdenes del constructor

`i2_ac1` (`bAllowColorAnim = 1;`), `i5_bl4` (la cadena `a=b=c=d=false` en cuatro
sentencias), `i1/i3/i4/i6/ia` (mover una sentencia): **idénticas, 95,47916**.
`i7_spfirst` (`fIconSpacing` al principio) **84,97 %**;
`q9_alcssp` **89,42 %**; `q5_icfc` **92,99 %**.
Todo lo que empieza por `AL, CS` da 12 filas; añadir `HG` da **11**
(`q4_hgfirst`, aplicado). **Las dos filas que quedan son `fCurFadeTime` (0xf8) y
`fCurrentAddPos` (0xf4) intercambiadas, y el intercambio es del planificador**:
las diez órdenes probadas con CF y AP en posiciones distintas dan las mismas 11-12
filas.

---

## 7. Lo que NO he probado

- **`ActualReadJoystickData`**: combinar `s8_calCL` (arregla r18/r19) con una
  forma que devuelva el `li r21, 0x1` a su sitio. Y el racimo del cuarto `andi.`
  desde el lado de la presión de registros (subirla para que `v` no coalesca con
  el temporal del `extsh`). Volcados RTL: ninguno esta ronda.
- **`Setup`**: una barrera `__asm__("# …")` delante de la llamada virtual —hay
  tres ya en esa misma función— la arreglaría casi seguro, pero es un constructo
  que el original no tenía y **no lo he puesto**.
- **`PursuitEscalation` / `BreakAway`** (804 B): sin ensayos. Sigue en pie el
  diagnóstico de la r27 (mover `…Id`/`…Handle` a `.sdata` para que el `lis`
  desaparezca); toca enlace.
- **`Play__RoadNoise`** (392 B): seis ensayos (§6). Diagnóstico nuevo y
  **medido con el mapa de campos**: al objetivo le sobra un `li 0` porque tiene
  **DOS pseudos con valor 0** — r29 (que **cruza las llamadas**) para
  `volume`(0x8), `azimuth`(0x10) y el `refCount = 0` de `GetRefCount()`, y r0
  para `type`(0x14), `secondaryNoise`(0x18), `speed`(0x1c), `hiPass`(0x24) y
  `wetFX`(0x2c). Nosotros usamos **uno** (r30) para los siete. Es exactamente la
  regla del §3 («una variable por uso») aplicada a los argumentos del
  constructor `Csis::FX_ROADNOISE(...)`: **hay que partir el cero en dos**.
- **`RenderString`** (1.572 B): nada. Sigue valiendo el aviso de la r27: antes de
  mover umbrales hay que explicar el tramo 250-300.
- **`GenerateRoadNoise`** (1.240 B): sin ensayos. Las 8 filas son **dos `fmadds`
  sobre el acumulador `f29`** que el objetivo emite en los índices 102 y 114 y
  nosotros en 114 y 134, más el reparto de los registros del pool de literales.
  Es el mismo patrón del §1 (quién acumula), pero en coma flotante: candidata
  directa a la regla del temporal separado.
- `MsgPlayMiscSound` (780 B), `CreateMixCtls` (448), `CreateSubMixChannels`
  (336), `MsgBarrier` (140): sólo leídas por `triage`.

---

## 8. Ensayos numerados (125)

- **`UnlockPalette` (18)**: `t1`…`t5`, `v1`…`v9`, `w1`,`w2`, `p1`,`p2`. Cifras en §1.
- **`NotifyEventCompletion` (8)**: `n1_oneline` 98,37302 · `n2_ref` no compila ·
  `n3_ge2` 98,37302 · **`n4_ternary` 100,00000** · `n5_noelse` sin ancla ·
  `n6_ptr` no compila · `n7_not` 98,37302 · `n8_intsp` 98,37302.
- **`ActualReadJoystickData` (49)**: `j1`…`j5`, `k1`…`k5`, `m1`…`m8`,
  `p_****` (24 permutaciones), `s1`…`s8`, `t1`…`t6`, `u1`…`u6`. Cifras en §3-§6.
- **`BindToData` (20)**: `g1`…`gc`, `h0`…`h7`. Cifras en §6.
- **`LoadSpeechBank` (6)**: `l1`…`l6` (`l3` no compila).
- **`Setup` (6)**: `r1`…`r6`.
- **`Play__RoadNoise` (6)**: `p1`…`p6`.
- **`ProcessUpdate__CARSFX_Turbo` (5)**: `w1`…`w5`.
- **`ToggleCapsLock` (7)**: `c1`…`c7`.
- **`IconScroller` (20)**: `i1`…`ia`, `n1`…`na`, `q1`…`qa`.

## 9. Herramientas y convivencia

- **`c27mx_run.py` de la r27 es la herramienta de la ronda**: 2 s por variante en
  `TextureInfoPlat`/`JoyE`, 8-25 s en `zFe2`/`zSpeech`. Las 24 permutaciones del
  bloque `else` de JoyE costaron 2 minutos y valieron 1,7 pp.
- **Trampa nueva del stub**: cuatro de los cinco ficheros nuevos **no compilan
  sueltos**; hay que añadir a `PRE` la cabecera que la SourceList ya había
  incluido antes — `EAXSOund.hpp` (feIconScrollerMenu), `SpeechManager.hpp`
  (RoadblockFlow, o `SpeechManagerLite.h` redefine `Speech::EventHistory`),
  `FEngInterfaceFEObjects.hpp` (FEpkg_MU_Keyboard). Y `RoadblockFlow.cpp`
  necesita **además** el `-I` de su directorio.
- Dejado: **`c28mx_rows.py <unidad> <sym> [ctx]`** (sólo las filas que difieren
  con contexto — mucho más barato que volcar el rango entero) y
  **`c28mx_vrows.py <unidad> <sym> <objeto> [ctx]`**, que hace lo mismo contra el
  `.o` de una variante del barrido sin tocar el árbol.
- Ficheros de variantes: `c28mx_tex1`, `c28mx_spm`, `c28mx_lsb`, `c28mx_ginsu`,
  `c28mx_ginsu2`, `c28mx_joy`…`c28mx_joy7`, `c28mx_ics`…`c28mx_ics3`,
  `c28mx_rbf`, `c28mx_kbd`. Auditorías en `c28mx/audit*_*.txt`; instantáneas en
  `c28mx/antes.json`, `final.json`, `pct_antes.json`, `pct_final.json`.
- **Disco**: entré con 16 GB y salgo con 16 GB; borrados los `.o`/`.ii`/`.json`
  de variante y el árbol sombra. Quedan 311 MB de scratchpad compartido.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, `symbols.txt` ni
  los bloques `__ANDROID__`. **Sin commit.**
