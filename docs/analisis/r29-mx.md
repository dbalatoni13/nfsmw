# Ronda 29 — mx: zEAXSound2 / zFe2 / zSpeech / zPlatform

**DOS funciones cerradas (804 B) con UNA línea de fuente repetida dos veces, y
cuatro reglas nuevas sobre la barrera — la principal es que la barrera es
DIRECCIONAL y que quien trabaja es el clobber de `"memory"`, no el operando.**

`measure.py --cmp`: **+804 B, +2 funciones, 1 unidad cambia.**
`pctsnap.py --cmp`: **EMPEORAN: ninguna. MEJORAN: 2.**

| función | unidad | antes | después | B |
|---|---|---|---|---|
| **`EAXDispatch::PursuitEscalation`** | zSpeech | 98,44203 | **100,00000** | **552** |
| **`EAXDispatch::BreakAway`** | zSpeech | 95,74603 | **100,00000** | **252** |

Fichero del árbol tocado: **UNO**, sin commit —
`src/Speed/Indep/Src/Speech/EAXDispatch.cpp` (+8 líneas: la misma barrera con su
comentario en los dos sitios).

---

## 0. Verificación del encargo y auditoría

`build_direct.py zEAXSound2 zFe2 zSpeech zPlatform` → 4 ok.
`triage.py … --muro` reproduce el encargo salvo **una cifra rancia**: el encargo
da `ActualReadJoystickData` al **94,595 %** y está al **99,244 %** (el valor con
el que la r28 la dejó). Las otras ocho coinciden al dígito.

| pasada | zEAXSound2 | zFe2 | zSpeech | zPlatform |
|---|---|---|---|---|
| entrada (`audit1_*`) | 922 ok / **0 FALLA** | 1304 / **0** | 699 / **0** | 135 / **0** |
| cierre (`audit2_*`) | 922 / **0** | 1304 / **0** | **701** / **0** | 135 / **0** |

Cero FALLA en las cuatro unidades en las dos pasadas: no hubo nada que confirmar.
`frozen.py cong Speed/Indep/SourceLists/zSpeech` → **`757be4641666b2d8`**.

La medida se tomó **revirtiendo mi fichero a HEAD, reconstruyendo, midiendo,
reaplicando, reconstruyendo y volviendo a medir** (`base2.json` → `final.json`),
porque hay otros agentes tocando el árbol: `git status` trae 70 ficheros
modificados que no son míos. El delta es exactamente 252 + 552.

---

## 1. LA REGLA DE LA BARRERA DIRECCIONAL — cierra 804 B

### El diagnóstico

Las dos funciones acaban igual (macro `SCHEDULE_SPEECH`): rellenan un `struct
data` en la pila y lo pasan a `Manager::ScheduleSpeech<…>(data, Id, Handle,
this)`. El **único** diff real es que el objetivo emite el `stw` de
`data.direction` **inmediatamente detrás del `slw`** que lo calcula, y nosotros
lo hundimos por detrás de los dos `lis` de los argumentos:

```
objetivo                      nuestro
 li r0, 0x1                    li r0, 0x1
 slw r0, r0, r3                slw r10, r0, r3       <- r10, no r0
 stw r0, 0x14(r1)              …
 lwz r0, 0xc(r28)              lwz r0, 0xc(r28)
 lis r4, …DispBreakAwayId@ha   lis r4, …@ha
 …                             …
                               stw r10, 0x14(r1)     <- 7 ranuras tarde
```

Las **15 filas** de `BreakAway` (r0 contra r10 en las nueve insns previas, el
`slw`, y los cuatro `stw` corridos una ranura) **son todas consecuencia de
esa una**: con el `stw` arriba, `dir` puede vivir en **r0**; con el `stw` abajo,
r0 lo pisa el `lwz r0, 0xc(r28)` y `dir` tiene que irse a r10.

### La palanca

```c
     data.direction = static_cast<Csis::Type_direction>(dir);
+    __asm__("");
     data.speaker_id = mSpeakerID;
```

**252 B y 552 B, 0 filas, a la primera compilación en las dos funciones.**

### Las cuatro reglas, todas medidas

1. **La barrera es DIRECCIONAL y va DESPUÉS de la instrucción que se hunde.**
   Mis doce primeros ensayos (`b6`, `b7`, `b8`, `bc`) la pusieron **delante** del
   `stw` — es lo que pide el instinto («que no suba nada»)— y dan **idéntico o
   peor**. Una barrera sólo impide **cruzarla**: delante del `stw` no restringe
   nada, porque todo lo que compite ya está detrás. *Si tu insn llega tarde,
   la barrera va detrás de ella; si llega pronto, delante.*
2. **Quien trabaja es el clobber de `"memory"`, no el operando.** El eje
   «selectivo» del brief (§1, `"+r"(x)` sin `volatile`) **no mueve nada aquí**:

   | forma | BreakAway | PursuitEscalation |
   |---|---|---|
   | `__asm__("" : "+r"(dir))` (`c3`) | 95,74603 | 98,44203 |
   | `__asm__("" : "+m"(data.direction), "+r"(dir))` (`d4`) | 95,74603 | **100** |
   | `__asm__("" : "+m"(data.direction))` (`c5`) | 95,74603 | **100** |
   | `__asm__("" : : "m"(data.direction))` (`d5`) | **100** | **100** |
   | `__asm__("" : : : "memory")` (`c4`) | **100** | **100** |
   | **`__asm__("")` (`c1`, aplicada)** | **100** | **100** |

   Lo que hay que ordenar aquí son **operaciones de memoria** entre sí, y para
   eso hace falta el clobber. `d4` es el caso limpio: un asm con salidas (o sea,
   NO volátil, el eje del brief) cierra la que tiene el `stw` a 3 ranuras y **no**
   la que lo tiene a 7.
3. **`"+m"` de un campo emite código; `"m"` no.** `d1`/`d2`/`d6` (`"+m"(data)`,
   `"+m"(campo) … "memory"`) suben de 64 a **66 insns** en `BreakAway`
   (88,7-88,9 %): GCC materializa la dirección del operando de salida. Con
   `"m"` de entrada (`d5`) salen 63, las del objetivo.
4. **Un intercambio DENTRO de la expansión de una sola llamada no lo toca
   ninguna barrera** (§3: `Setup`, 10 formas).

### Cuántas veces sirve — la extrapolación, contada

De las **ocho** funciones de mis cuatro unidades donde el diff es «una insn en
otra ranura», la barrera **cierra dos**, **mejora una mucho** (`CreateMixCtls`,
19 → 2 filas) y **no hace nada o estropea en cinco**. No es un frente: es una
herramienta con una condición de aplicación clara — **la insn que sobra o falta
de sitio tiene que ser un acceso a MEMORIA, y tiene que llegar TARDE**.

---

## 2. `CreateMixCtls` 97,32143 → **99,91071 %** (19 filas → 2) — NO aplicada

Mismo mecanismo, y aquí el diagnóstico era nuevo: el **`addi r28, r28, 0x1`** (el
`n++` del `for`) se emite en el objetivo **entre** `lwz r11, 0x4(r31)` y
`stw r24, 0x8(r11)` (`NFSMixMapState.cpp:270`, según `lmap`) y nosotros lo
sacamos **5 ranuras antes**, justo detrás del `bl GetQ15FromHundredthsdB`. Los
17 pares r9/r11 que difieren son **consecuencia**.

```c
+            __asm__("");
             pmcp->pudata->CmpdBOut = 0;
```

**97,32143 → 99,91071 %, 19 → 2 filas, 448 B, mismo tamaño.** (`g5`/`g6`.)

**No la he aplicado**: `matched_code` es todo-o-nada, así que aporta **cero
bytes**, y es un constructo que el objetivo no tiene. Queda documentada para
aplicarla en el momento en que se cierren las dos filas que faltan.

Las dos filas restantes son **una sola diferencia**:

```
objetivo   lwz r3, 0x8(r9)   /  neg r3, r3
nuestro    lwz r0, 0x8(r9)   /  neg r3, r0
```

O sea `ntmp = -pmcp->psdata->nOffset;`: el objetivo carga **en el registro del
argumento** y niega en sitio; nosotros usamos un temporal. La forma que lo
arregla es **la de dos pasos** (`ntmp = pmcp->psdata->nOffset; ntmp = -ntmp;`),
que da `lwz r3` + `neg r3,r3` **exactos**… pero **invierte la paridad r9/r11 de
las dos recargas de `pmcp->psdata` anteriores** y deja 4 filas en vez de 2
(99,82143). Diez formas probadas alrededor (`h1`…`h7`, `s1`…`s6`, `t1`…`t4`)
dan **todas 4 filas**: el problema no es la sentencia del `neg`, es que al fundir
los dos pseudos se corre la numeración de cantidades de `local_alloc`.
**Lo que falta es reponer un pseudo antes de ese punto sin emitir código**, y
`t1_uscs` (`int uscs = pparams->nUScaleCntSwing;` usado en las tres ramas) **no
lo consigue**.

---

## 3. `Setup`: la barrera NO lo arregla — la hipótesis de la r28, refutada

La r28 dejó escrito: *«una barrera `__asm__("# …")` delante de la llamada virtual
la arreglaría casi seguro, pero es un constructo que el original no tenía y no lo
he puesto»*. **Puesta y medida: no lo arregla.** Diez formas, **todas
98,25504 % con las mismas 3 filas**:

`e1_dafter` (`__asm__("# d")` detrás), `e2_dbefore` (delante), `e3_totafter`,
`e4_totbefore` (`__asm__("")` detrás/delante), `e5_rprim_before`
(`"+r"(primary)`), `e6_rprim_after` (`"+r"(primary)` + `"memory"`), `e7_memb`
(`: : : "memory"` delante), `ea_selprim_mem_before`, `e8_pvar` (copia de
`primary`). Sólo `e9_rbu_after` (intercambiar `RBUpdate` y la llamada virtual)
mueve algo, y a peor: **91,87248 %, 155 insns**.

**Y la razón es estructural**: las dos filas son el orden interno de las **seis
instrucciones que expande UNA sola llamada virtual**
(`lwz vt / lwz pfn / lha delta / mtlr / add this / blrl`). Ninguna sentencia de
fuente cae en medio, así que **no hay dónde poner la barrera**. El objetivo emite
`lwz pfn` antes que `lha delta` **sólo en este sitio**: sus otras **cinco**
llamadas virtuales de la misma función (`RoadblockFlow.cpp` 368, 376, 377, 387,
400) emiten `lha` primero, igual que nosotros. **Es un desempate del planificador
dentro de una expansión, y desde el fuente no se toca.**

---

## 4. La barrera «selectiva» que no ata nada — `GenerateRoadNoise`

`__asm__("" : "+f"(fLeftPitch))` detrás de `fLeftPitch = fLeftPitch + fLeftPitch
* ftemp;` (`j1`, `j2`, `j3`, `j6`) da **exactamente el mismo objeto** que el
control. **Regla: un asm que sólo nombra el resultado de una insn no la adelanta
— sólo añade una dependencia que YA existía** (el productor ya domina a todos sus
consumidores). Para adelantar hace falta **retrasar a sus competidoras**, y eso
sólo lo hace el clobber… que aquí sale caro: `j4` (barrera total) **95,75806 % y
+2 insns**, `j5` (`"+f"` + `"memory"`) **94,90 % y 44 filas**.

Las 8 filas reales son **dos `fmadds` sobre `f29` (`fRightVol`)** que el objetivo
emite en los índices 102 y 114 y nosotros en 114 y 134; de ahí sale que el
objetivo pueda reusar `f11` en `fmadds f11, f13, f13, f0` y nosotros tengamos que
gastar `f10`, y luego f9/f12 y f7/f10 en el pool de literales. **Es el reparto de
registros de coma flotante, y lo decide la posición del primer `fmadds`.**

`lmap` fija el sitio: el objetivo mete `fmadds f29, f31, f11, f31`
(`CARSFX_Roadnoise.cpp:558`) **entre los dos `lis` de los literales 562/564**.
Las ocho permutaciones del bloque `fRightVol / slipBoost / bLength / fRightVol /
*0.01f` (`k1`…`k8`) son **todas iguales o peores** (86,2-97,1 %), y tres de ellas
(`k1`, `k3`, `k7`) pierden 8 B. **El orden que tenemos es el mejor de los ocho.**

---

## 5. `Play__RoadNoise` — el diagnóstico del cero partido, cerrado con `lmap`

La r28 dejó la hipótesis «hay que partir el cero en dos». **`lmap` da ahora la
frontera exacta y explica por qué ninguna variante lo consigue**: los dos ceros
del objetivo no son dos constantes, es **una constante que sobrevive a las
llamadas y otra que muere antes**.

```
800D86A8  li r29, 0x0       ENVIRO_AEMS.h:330   <- el `x = 0` de SetVolume
800D86B0  li r0,  0x0       ENVIRO_AEMS.h:382   <- el `x = 0` de SetType
…
800D86C4  stw r29, 0x8(r31)   :337  volume
800D86CC  stw r29, 0x10(r31)  :369  azimuth
800D86EC  stw r0,  0x14(r31)  :385  type
800D86F0  stw r0,  0x18(r31)  :401  secondaryNoise
800D86F4  stw r0,  0x1c(r31)  :417  speed
800D86F8  stw r0,  0x24(r31)  :449  hiPass
800D86E8  stw r0,  0x2c(r31)  :481  wetFX
800D86FC  bl CreateInstance…
…
800D872C  stw r29, 0x8(r1)    :491  <- `int refCount = 0;` de GetRefCount()
```

**El pseudo r29 (salvado por el llamado) es el mismo que sirve a
`int refCount = 0;` de `GetRefCount()`.** Por eso es callee-saved y por eso la
frontera cae en `SetType`: CSE arrastra el 0 de `SetVolume` hasta
`GetRefCount`, y el de `SetType` es un pseudo nuevo que muere antes de la
llamada. Nosotros usamos **uno solo** (r30) para los siete campos y para
`refCount`, y por eso nos falta un `li 0` (388 B contra 392).

**Y hay un dato de cabecera que nadie había mirado.** Por los números de línea
del objetivo, en el original `FX_ROADNOISE` va de `SetId` (**314**) a
`GetRefCount` (**488**) = **174 líneas**; en nuestra `ENVIRO_AEMS.h` va de 152 a
293 = **141**. **Al original le sobran ~33 líneas ahí, que son exactamente DOS
pares accesor/getter de 16 líneas** (el paso es 16 clavado: 316→330→346→362→
…→478). Falta comprobar si eso es sólo cosmético (un cuerpo en clase no emite
símbolo) o si mueve el inline — es la regla §3.2 del brief. **No lo he probado.**

---

## 6. Vedas medidas, con la sentencia barrida

### `zSpeech::Setup` (596 B, 3 filas) — 10 formas más (van 16)
Todas en §3. **Ninguna mueve una fila.** Cerrada por construcción hasta que
alguien sepa mover el desempate de `sched` dentro de la expansión de la llamada
virtual.

### `zEAXSound2::MsgBarrier` (140 B, 2 filas) — 11 formas, PRIMERA tanda
La única diferencia es que el objetivo emite `li r4, 0x1` (el 3.er argumento de
`WooshFadeOut.Initialize`) **delante** del `stfs f13, 0x34(r10)`
(`mDurationActive = 0.0f`) y nosotros detrás.

| ensayo | qué se barrió | cifra |
|---|---|---|
| `f1_ACB` … `f5_CBA` | **las 5 permutaciones** de `bGoingToCollide / mDurationActive / Initialize` | 64,3 · 72,0 · 72,0 · 77,1 · 79,6 % |
| `f6_totmid` | `__asm__("")` entre `mDurationActive` e `Initialize` | 67,09 %, 136 B |
| `f7_totpre` | barrera antes de `mDurationActive` | 71,09 %, 136 B |
| `f8_selm` | `"+m"(this->mDurationActive)` | 76,97 % |
| `f9`, `fa`, `fb` | los argumentos (`1`, `LINEAR`) y el `0.0f` en variables | **idénticas** |

**El orden de fuente que tenemos es el mejor de los seis**, y la barrera aquí es
justo del lado malo (hay que **adelantar** el `li r4`, no retrasarlo).

### `zFe2::IconScroller` (384 B) — 23 formas más (van 43)
**Las barreras la destruyen**: `m1`/`m2` (tras `fCurFadeTime`) **48,25 %, 119
insns**; `m5` **50,32 %**; `m6` **46,17 %, 120 insns**; `m3`/`m4` (junto a
`fCurrentAddPos`) 85,69/85,65 % y 101 insns. **La causa está medida: la barrera
rompe el CSE del cero** que alimenta los quince `stfs f0` y GCC lo rematerializa
(+22 instrucciones).

**Sin barrera sí se avanza**: barriendo **las 15 posiciones** de
`fCurFadeTime = 0.0f;` en el constructor (`n00`…`n14`), las posiciones **11, 12 y
13** dan **97,80209 % y 8 filas** (contra 97,77084 % y 11 del control); `n14`
(la última) se hunde a 91,48 %, y `n00`/`n01` a 95,69 %. **No la he aplicado**:
son 0 bytes y **otro reorden sin respaldo del original** encima del que ya metió
la r28. Con `fCurFadeTime` en la posición 11 la ÚNICA fila real es que el
objetivo emite `stfs f0, 0xf8(r31)` en el índice 42 (suelto, delante de
`addi r26, r31, 0xec` y del `stfs f12, 0xfc`) y nosotros en el 48, a la cabeza
del racimo de ceros.

### `zFe2::ToggleCapsLock` (92 B, 3 filas) — 7 formas más (van 22)
`u2_modevar` (`int mode = mnMode;`), `u5_capszero` (`0`/`1` en vez de
`false`/`true`), `u6_notequal` (`if (…!=…) {} else {…}`), `u7_bothvars` (los dos
valores en variables): **idénticas, 91,08696 %**.
`u1_hoist` (`bool isFile = mnMode == MODE_FILENAME;` antes de los dos stores)
**76,91 %, 104 B**; `u3_shiftfirst` **80,65 %**; `u4_selasm` (`"memory"` entre
los stores y el `if`) **85,43 %, 96 B**.
El diff es que el objetivo emite `stw r10, 0x3c(r3)` (`mbShift`) **detrás** del
`cmpwi r9, 0x3`, y usa **r10** para ese cero mientras nosotros usamos **r0**.

### `zFe2::RenderString` (1.572 B) — 8 formas, PRIMERA tanda
**El diagnóstico está cerrado y es de reparto**: el objetivo gasta **tres**
registros salvados donde nosotros gastamos **cuatro**.

```
objetivo   pcString -> r26   matrix -> r27   c -> r27 (REUSA el de matrix)
nuestro    matrix   -> r26   pcString -> r27   c -> r28
```

`c` se puede quedar con el registro de `matrix` porque `matrix` muere en
`cached->SetTransform(matrix)`, que está **antes** del bucle. Nosotros asignamos
`c` **la primera** de las tres (r28 es la más alta libre) y ya no hay reúso: hay
que **bajar la prioridad de allocno de `c`**, o sea alargar su vida o quitarle
referencias.

| ensayo | qué | cifra |
|---|---|---|
| `r1_czero` (`u16 c = 0;`), `r3_cfirst`, `r6_texafter`, `r7_ktop`, `r8_texfirst` | mover/inicializar las declaraciones | **idénticas** 93,32316 |
| `r5_kinit` (`u32 k = 0;`) | | 93,29262 (88 filas) |
| `r2_cint` (`int c`) | | 92,83969 %, **1.568 B** |
| `r4_cbeforeset` (`c = *pcString++` antes de `SetTransform`) | | 92,18829 %, **404 insns** |

**Ninguna forma de declaración cambia la vida de `c`**: GCC la calcula por usos.
Lo que queda es el volcado RTL (§7).

### `zPlatform::ActualReadJoystickData` (1.588 B, 19 filas) — 8 formas más (van 57)
El racimo de las **dos instrucciones que faltan** (somos 1.580 B contra 1.588) ya
tiene lectura exacta con `lmap`:

```
8026C868  extsh r0, r11
8026C86C  mr r9, r0              JoyE.cpp:307   <- la copia que NO tenemos
8026C870  stb r0, 0x4(r29)       JoyE.cpp:306   <- el store usa el TEMPORAL
8026C874  andi. r11, r9, 0x8000  JoyE.cpp:307   <- el 4.º andi., que NO tenemos
```

O sea: en el objetivo `joy_data->stickX` se guarda desde **el temporal del
`extsh`** y el `& 0x8000` se hace sobre **`v`, que es una copia**; nosotros
tenemos un solo pseudo y `flow` borra el `andi.`. Y — dato nuevo — **el
resultado de ese `andi.` es el CERO que alimenta los cinco `mr` de las líneas
317/318**: no hay `beq` detrás.

Ocho formas del recorte 3 (`p1`…`p8`): `p3_svar`, `p4_svar_vfirst`, `p5_extra`,
`p8_shortvar` **idénticas**; `p7_noshort` 99,21915 (20 filas);
**`p1_storeinside`, `p2_vfirst` y `p6_storedata` bajan a 15 filas pero a
1.576 B** — quitan una instrucción y se alejan más del objetivo (1.588). Es la
trampa del §5 de HERRAMIENTAS al revés: **menos filas con menos tamaño no es
avance**.

### `zEAXSound2::GenerateRoadNoise` (1.240 B) — 14 formas
En §4.

### `zEAXSound2::CreateMixCtls` (448 B) — 25 formas
En §2.

---

## 7. Lo que NO he probado

- **`RenderString` (1.572 B): el volcado RTL.** Los umbrales de la r27
  (`pcString` con 35 refs → 4901 contra 4806, o vida de `pcString` ≤ 353, o vida
  de `c` ≥ 236) **no los he verificado contra el `.greg`** — sólo he barrido
  ocho formas de fuente, y ninguna toca la vida de `c`. Hace falta
  `cc1plus -da` a mano (HERRAMIENTAS §8-bis) y `scripts/alloc.py`. **Es el mayor
  premio suelto de las cuatro unidades.**
- **`CreateMixCtls` (448 B): las 2 filas.** Falta una forma que dé el `neg` en
  sitio (la de dos pasos) **sin** correr la paridad r9/r11 de las dos recargas
  anteriores. Diez intentos por el lado de la sentencia; **cero por el lado del
  `.lreg`/`.greg`**, que es donde está la respuesta.
- **`Play__RoadNoise` (392 B):** añadir a `FX_ROADNOISE` los ~2 pares
  accesor/getter que le faltan a nuestra `ENVIRO_AEMS.h` (§5) y volver a medir.
  Ningún ensayo esta ronda; los 6 de la r28 siguen en pie.
- **`ActualReadJoystickData` (1.588 B):** sigue sin probarse la combinación
  `s8_calCL` + algo que devuelva el `li r21, 0x1` a su sitio (el mejor cabo
  suelto según la r28), y el racimo del 4.º `andi.` desde la presión de
  registros.
- **`MsgPlayMiscSound` (780 B, «28 registros») y `CreateSubMixChannels`
  (336 B, «16 registros»): ni un ensayo.** Sólo leídas por `triage`.
- **`LoadSpeechBank` (316 B):** ni un ensayo nuevo. Diagnóstico afinado: somos
  312 B contra 316, y al objetivo le sobran **dos** `mr` de `index->item`
  (`mr r11, r4` en el índice 16 y `mr r4, r11` en el 26) donde nosotros tenemos
  uno (`mr r4, r7` en el 14). Las 6 vedas de la r28 siguen.
- **`ProcessUpdate__CARSFX_Turbo` (672 B):** ni un ensayo. El diff es
  `mr. r7, r3` en el índice 30 del objetivo y el 39 nuestro (llega **tarde**, o
  sea **candidata directa a la barrera de §1**) más `lis r9, $LC524` que llega
  **pronto**. **Es la primera que probaría el siguiente.**

## 8. Ensayos numerados (114)

- **`BreakAway` + `PursuitEscalation` (26)**: `b1`…`bc` (12, todas idénticas o
  peores: `b6` 94,98 · `b7` 91,60 · `ba_short` 93,84/97,43) ·
  `c1`…`c8` (8: **`c1` 100/100**, **`c4` 100/100**, **`c6` 100/100**,
  `c5` 95,75/**100**, resto idénticas) ·
  `d1`…`d6` (6: **`d5` 100/100**, `d4` 95,75/**100**, `d1`/`d2`/`d6` 88,8/**100**).
- **`Setup` (10)**: `e1`…`ea`. Cifras en §3.
- **`MsgBarrier` (11)**: `f1`…`fb`. Cifras en §6.
- **`CreateMixCtls` (25)**: `g1`…`g7`, `h0`…`h7`, `s1`…`s6`, `t1`…`t4`. §2.
- **`GenerateRoadNoise` (14)**: `j1`…`j6`, `k1`…`k8`. §4.
- **`IconScroller` (23)**: `m1`…`m8`, `n00`…`n14`. §6.
- **`ActualReadJoystickData` (8)**: `p1`…`p8`. §6.
- **`RenderString` (8)**: `r1`…`r8`. §6.
- **`ToggleCapsLock` (7)**: `u1`…`u7`. §6.

## 9. Herramientas y convivencia

- **`c27mx_run.py` de la r27 sigue siendo la herramienta**: 8-30 s por variante
  en estas cuatro unidades, con varios símbolos por pasada. Los stubs que hay que
  saber: `EAXDispatch.cpp` y `RoadblockFlow.cpp` piden
  `ATTRIB_NO_INLINE_CLASSKEY` + `SpeechManager.hpp` + el `-I` de `Src/Speech`;
  `JoyE.cpp` pide `Timer.hpp`; `feIconScrollerMenu.cpp` pide `EAXSOund.hpp`
  **y el `-I` de su propio directorio**; `FEpkg_MU_Keyboard.cpp` pide
  `ANIMWORLDTYPES_EMIT_MESSAGE_HASHES` + `ATTRIB_NO_INLINE_CLASSKEY` +
  `FEngInterfaces/FEngInterfaceFEObjects.hpp`.
- **Finales de línea, medidos fichero a fichero**: `EAXDispatch.cpp` **mezcla**
  (319 CRLF de 337 líneas; el bloque 101-185 es todo CRLF),
  `feIconScrollerMenu.cpp` **mezcla con UNA sola línea LF (la 267**, el final de
  la lista de inicializadores del constructor de `IconScroller`),
  `RoadblockFlow.cpp` y `JoyE.cpp` **LF puro**, `CARSFX_Roadnoise.cpp`,
  `NFSMixMapState.cpp`, `CARSFX_PreColWoosh.cpp`, `FEngFont.cpp` y
  `FEpkg_MU_Keyboard.cpp` **CRLF puro**. Doce variantes se me fueron en `NO
  ANCLA` por no mirarlo primero.
- Ficheros de variantes en el scratchpad, prefijo `c29mx_`: `ba`, `ba2`, `ba3`,
  `setup`, `mb`, `cmc`, `cmc2`, `cmc3`, `cmc4`, `grn`, `grn2`, `ics`, `ics2`,
  `joy`, `rs`, `tcl`. Auditorías e instantáneas en `c29mx/`
  (`audit1_*`, `audit2_*`, `base2.json`, `final.json`, `pct_base2.json`,
  `pct_final.json`).
- **Disco**: entré con 16 GB y salgo con 14 GB; borrados mis `.o`/`.ii`/`.json`
  de variante y el árbol sombra. Los 500 MB de `c29ae_*` son de otro agente de
  esta misma ronda, **no los toques**.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, `symbols.txt` ni
  los bloques `__ANDROID__`. **Sin commit.**
