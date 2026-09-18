# Ronda 32 — nxt: zFe2 / zPhysicsBehaviors / zTrack (el siguiente escalón)

**CERO funciones cerradas, y el hallazgo que manda sobre el encargo: NINGUNA de
las tres unidades promociona esta ronda aunque cierre las seis funciones. Medido
con el enlazador delante, ANTES de gastar un ensayo: `zFe2` NI SIQUIERA ENLAZA
(101 errores, 5 símbolos de datos y 8 pares de clase local), y `zPhysicsBehaviors`
y `zTrack` enlazan pero rompen el DOL por DATOS (`.data2` −1.536 / `.data3` +416
y `.data2` −2.048 / `.data3` −416). Los 550.652 B del encargo están detrás de los
frentes §2 y §3 del brief, no detrás de mis seis funciones.**

Lo que sí traigo:

1. **La segunda mitad del hallazgo de la r31 sobre `GetLoadingPriority`, y es una
   mala noticia medida: los 16 B de marco valen 43 de las 71 filas (97,18079 →
   97,42938 %) y NO CIERRAN la función.** Quedan 28 filas que son un intercambio
   de dos FPR salvados (f30↔f31). El hallazgo de la r31 era necesario y no es
   suficiente; ahora está cuantificado.
2. **Un diagnóstico nuevo y completo de `UpdateLoaded`** (856 B, nunca tocada):
   los 4 B que sobran son **dos `lis $LC931@ha` que rematerializamos** donde el
   original iza **uno solo a r30** y lo mantiene vivo **a través de dos `bl`**.
   No es bandera (10 barridas) ni estructura de bloques (4 formas).
3. **Una veda nueva y cara en `RBGrid::Add`** (1.488 B, 99,301 %): las 8 filas son
   **una sola diferencia repetida 4 veces** — la arista de retorno del `while` de
   `SAP::Grid<T>::Axis::Node::Node`. Mecanismo identificado al insn; **24 ensayos
   en la unidad real, ~45 en caso mínimo y 34 de banderas** no lo mueven.
4. **Dos vedas viejas reconfirmadas con formas nuevas** (`ToggleCapsLock` +7,
   `IconScroller` +6).

**Ficheros del árbol tocados: NINGUNO.** Todo con parche temporal + restauración
en `finally`. Sin commits.

---

## 0. Verificación del encargo — reproduce ENTERO, al dígito

`build_direct.py zFe2 zPhysicsBehaviors zTrack` → 3 ok. `triage.py … --muro`
da las seis funciones con las cifras del encargo:

| bytes | % | función | unidad | `triage` |
|---|---|---|---|---|
| 1.572 | 93,323 | `FEngFont::RenderString` | zFe2 | reorden local, dmax 5 (18 filas crudas, **CERO reales**) |
| 1.488 | 99,301 | `RBGrid::Add` | zPhysicsBehaviors | faltan 4, sobran 4, **de ellas 4 SUST** — `b+4, blt-4` |
| 856 | 97,210 | `SuspensionTraffic::Tire::UpdateLoaded` | zPhysicsBehaviors | faltan 2, sobran 3 — `lwz-1, stw-1, lis-1, stmw+1` |
| 708 | 97,181 | `TrackStreamer::GetLoadingPriority` | zTrack | 67 registros |
| 384 | 97,771 | `IconScroller::IconScroller` | zFe2 | reorden local, dmax 9 (2 filas crudas, **CERO reales**) |
| 92 | 91,087 | `FEKeyboard::ToggleCapsLock` | zFe2 | reorden local, dmax 2 (2 filas crudas, **CERO reales**) |

El encargo daba 99,304 para `RBGrid::Add`; `triage` da **99,301** y `fndiff`
**99,30108**. Es la única cifra que baila, y por redondeo.

**La segunda función de `zPhysicsBehaviors` que el encargo dejaba sin nombre es
`SuspensionTraffic::Tire::UpdateLoaded(float,float,float,float)`** — ojo, el
símbolo lleva **cuatro** `f` (`…4Tireffff`), no tres; con el nombre de tres
`fndiff` contesta `not found` sin más explicación.

**Repetido el `triage` al cerrar, tras reconstruir: las seis cifras idénticas.**

`audit.py` sobre las tres rutas completas, **dos pasadas independientes**:

| unidad | ok | FALLA (1.ª) | FALLA (2.ª) |
|---|---|---|---|
| `Speed/Indep/SourceLists/zFe2` | 1.094 | **0** | **0** |
| `Speed/Indep/SourceLists/zPhysicsBehaviors` | 985 | **0** | **0** |
| `Speed/Indep/SourceLists/zTrack` | 212 | **0** | **0** |

No hubo nada que confirmar.

---

## 1. `trypromo.py`: el veredicto de las tres, al abrir y al cerrar

**Idénticos** (mismos sha1 en las dos pasadas: nada de lo que hice movió el DOL).

### `zFe2` — **ENLACE FALLA**, 101 errores

Detalle con `scratchpad/c32nxt_link.py` (copia del `c31ub_link.py` de la r31, que
imprime **todos** los errores agrupados y, si enlaza, el DOL sección a sección):

| veces | símbolo indefinido | quién lo referencia |
|---|---|---|
| **64** | `TheUnlockData` | **nuestro** `zFe2.o` |
| 8 | `gTradeInFactor` | `zFeOverlay.o` (extraído) |
| 4 | `gMaxPartLevels` | **nuestro** `zFe2.o` |
| 3 | `GetVehicleVectors__FP8bVector2T0P8ISimable` | objeto extraído |
| 2 | `TWK_RadarDetectorMinThreshold` | **nuestro** `zFe2.o` |
| 2 | `DriveConfigs` | **nuestro** `zFe2.o` |
| 2 | `HudConfigs` | **nuestro** `zFe2.o` |
| 8×2 | `Callback__CQ313FEPlayerCarDB…` + `_._Q313FEPlayerCarDB…` | **nuestro** `zFe2.o` |

Los 16 últimos son **ocho pares de clase local declarada DENTRO de una función**
de `FEPlayerCarDB` (`GetNumCareerCarsWithARecord`, `GetTotalFines`,
`GetNumImpoundedCars`, `GetTotalBustedPursuits`, `GetTotalEvadedPursuits`,
`GetTotalBounty`, `GetTotalNumInfractions`, `GetNumInfraction`): cada una
referencia su `Callback__…` y su destructor `_._Q3…` y **ninguno de los dos
existe**. Es el frente §2 del brief, no el mío. **No lo he tocado.**

### `zPhysicsBehaviors` — **ENLAZA**, DOL ROTO −1.120 B (`824fd3b7e988`)

```
DOL ROTO  (4.540.768 B contra 4.541.888 B)
  sec text2  CABECERA distinta: off 003a11c0/003a11c0 adr 803a41c0/803a41b8  siz igual  (adr +8)
  sec data2  CABECERA distinta: siz 0004bf20/0004c520   (d = -1536)   <- FALTA dato
  sec data3  CABECERA distinta: siz 00041580/000413e0   (d =  +416)   <- SOBRA pool
  sec data4/data5  off y adr corridos, siz igual
  sec text1  1.248.416 B distintos de 3.804.448, 15.030 rachas
```

**Le pasan las DOS cosas a la vez**: `.data2` corta **−1.536 B** (§2 del brief) y
`.data3` larga **+416 B** (§3). Y hay un tercer detalle que no vi en ninguna otra
unidad y que dejo apuntado para `lk`/`und`: **`.text2` sale 8 B más ARRIBA que en
el original con el mismo offset y el mismo tamaño** — algo entre `.text` y
`.text2` mide 8 B de más en nuestro enlace.

Las 15.030 rachas de `text1` **no son mías**: con `.data2` corta 1.536 B, todo
símbolo posterior se corre y cada `lis/addi` cambia 2 B.

### `zTrack` — **ENLAZA**, DOL ROTO −2.464 B (`13bdc17467c7`)

**Reproduce la medida de la r31 al dígito**, mismo sha1 incluido:

```
  sec data2  siz 0004bd20/0004c520   (d = -2048)
  sec data3  siz 00041240/000413e0   (d =  -416)
  sec text1  39.818 B distintos de 3.804.448, 20.054 rachas (punteros corridos)
  sec text2  1.543 B distintos de 149.920, 896 rachas
```

**Le falta dato en las dos secciones**, y sigue necesitando lo que pedía la r31.

### Consecuencia para el encargo

Cerrar las seis funciones era **necesario y no suficiente en ninguna de las
tres**. Reordené el trabajo por probabilidad de cierre, empezando por lo que
tenía diagnóstico previo o pista estructural.

---

## 2. `TrackStreamer::GetLoadingPriority` — el hallazgo de la r31, cuantificado… y NO basta

La r31 localizó el hueco «al byte» y no gastó ni un ensayo. Lo he gastado yo, y
el resultado cambia el pronóstico de la función.

### 2.1 El aritmética del marco, cerrada

DWARF del original y nuestro (`dwbody.py … both`, con el volcado nuestro
regenerado — el que había era más viejo que el `.o` y `dwbody` avisa):

| objeto | original | nuestro | tamaño implícito en el ORIGINAL |
|---|---|---|---|
| `layer_name[32]` | r1+0x08 | r1+0x08 | 32 ✔ |
| `pos` (FloatVector, 12 B) | r1+0x28 | r1+0x28 | 16 ✔ |
| `face[4]` exterior | r1+0x38 | r1+0x38 | **16** ✗ (debería ser 48) |
| `face[4]` del bucle | **r1+0x48** | r1+0x68 | **96** ✗ (debería ser 48) |
| `bScale::dest` | r1+0xA8 | r1+0x98 | — |
| `predict_pos`/`direction`/`v`/`bNormalize::dest` | B8/C0/C8/D0 | A8/B0/B8/C0 | — |
| marco | **0x120** | 0x110 | |

**El árbol de bloques y la lista de locales son IDÉNTICOS** (mismo bloque anónimo,
mismo `for`, mismos `point1`/`point2`, mismos inlines). Y el `.text` del original
**no toca nada por debajo de `r1+0xA8`**: el bloque `RemoteCaffeinating` es código
muerto y sólo RESERVA pila. O sea: el problema entero son **16 B de reserva**.

**Y ese reparto no se puede reproducir con nuestras locales**, y es una
observación nueva: el `face[4]` interior del original arranca en 0x48, o sea
**SOLAPADO** con el exterior (0x38..0x68). Ningún reparto no solapado de dos
`FloatVector[4]` da a la vez `outer=0x38`, `inner=0x48` y `bScale::dest=0xA8`:
si el exterior mide 48, el interior nace en 0x68; si el interior nace en 0x48, al
exterior le tocaron 16 B. **Nuestro asignador nunca solapa dos arrays de bloques
anidados.** Lo dejo escrito porque invalida la lectura ingenua de «falta una
local de 16 B declarada en el mismo sitio».

### 2.2 Lo que vale, medido

| ensayo | qué | cifra | filas |
|---|---|---|---|
| base | — | 97,18079 | 71 |
| **`m1`** | **`FloatVector spare;` detrás de `pos` (relleno de 16 B)** | **97,42938** | **28** |
| `m2` | dos `bVector2` detrás de `pos` | 97,18079 (idéntica) | 71 |

Con `m1` el marco pasa a **0x120** y **todos los desplazamientos de pila casan**
(`stfd f10, 0xe8(r1)`, `psq_st f29, 0x108(r1)`, `stmw r28, 0xf8(r1)`, `lwz r0,
0x124(r1)`, `addi r1, r1, 0x120`). **43 de las 71 filas eran el marco.**

`m2` prueba de paso que **dos `bVector2` sin usar no reservan nada**: sólo el
objeto BLKmode de 12→16 B (`FloatVector`) crea ranura.

**`m1` NO SE QUEDA**: es la local inventada contra la que avisa el comentario del
propio `TrackStreamer.cpp`. Su valor es diagnóstico.

### 2.3 Las 28 filas que quedan: un intercambio de f30/f31

`regmap.py`: **mismo conjunto de locales, mismo árbol de bloques, 14 iguales y 8
con registro distinto, 0 sólo del original, 0 sólo nuestras**. Las que mandan:

| local | original | nuestro |
|---|---|---|
| `speed` | **f31** | f30 |
| `angle_factor` | f0 | (plegada) |
| `adjusted_distance` | **f0** | f31 |
| `priority` | r0 | (plegada) |

En el objetivo, `f31` lleva `speed` (y luego `speed_factor`) y **`f30` lleva la
constante `1.0f`**; nosotros al revés, y de ahí que la cola aritmética entera use
`f31` de acumulador donde el objetivo usa `f0`:

```
objetivo  fmuls f0,f12,f9 / fmuls f0,f0,f31 / fmuls f11,f0,f11 / fsubs f0,f30,f11
nuestro   fmuls f30,f30,f11 (pronto) / fmuls f13,f13,f30 / fmuls f13,f13,f9 / fsubs f31,f31,f13
```

La reasociación del producto (`speed_factor * 0.67` izado) es CONSECUENCIA, no
causa: **cinco paréntesis distintos dan el objeto idéntico** (`fold` canonicaliza).

| ensayo | qué se barrió | cifra | filas |
|---|---|---|---|
| `n1`,`n2`,`n3` | tres paréntesis de `adjusted_distance` | **idénticas** 97,18079 | 71 |
| `n4` | `speed_factor` delante de la constante | 96,22034, **696 B** | 71 |
| `n5` | `(speed_factor * 0.67f)` agrupado | 95,1469, **696 B** | 72 |
| `p1` | `bMin(1.0f, speed*K)` (argumentos al revés) | 97,37288 | 28 |
| `p2` | `speed * K` a variable antes del `bMin` | 97,42938 (idéntica a `m1`) | 28 |
| `p3` | `float one = 1.0f;` para el `1.0f -` | 97,42938 (idéntica) | 28 |
| `p4` | `speed` con `if/else` en vez de asignar y sobrescribir | 83,932, **712 B** | 70 |
| `p5` | `speed_factor` como primer factor | 96,89, **696 B** | 19 |

(`p1`…`p5` llevan el relleno `m1` puesto, para poder leer las filas.)
`p5` es el enésimo caso de la trampa del §5 de HERRAMIENTAS: **menos filas con
MENOS tamaño**.

### 2.4 Veredicto

> **El hueco de 16 B de la r31 es real y vale 43 filas, pero `GetLoadingPriority`
> NO se cierra con él.** Quedan 28 filas que son un cambio de dos FPR salvados
> (`speed` ↔ la constante `1.0f`) y ocho ensayos de fuente no lo mueven. Quien
> vuelva necesita **dos** cosas, no una: la forma legítima de los 16 B **y** el
> modelo del registro duro de `find_reg` (la capa que la r30 dejó sin mirar en
> `RenderString`). Con una sola no hay byte.

---

## 3. `SuspensionTraffic::Tire::UpdateLoaded` (856 B) — diagnóstico nuevo y completo

Nunca se había tocado. **Los 4 B que sobran (860 contra 856) son UNA cosa.**

El objetivo iza el `@ha` de la constante `1.0f` a **r30** y lo mantiene vivo a
través de **dos llamadas**; nosotros lo rematerializamos en cada uso:

```
objetivo  .L_8024B460:  fmr f1,f30 / lis r30,lbl_803FB6B8@ha / bl VU0_Atan2__Fff
          .L_8024B49C:  lfs f0,lbl_803FB6B8@l(r30)      <- uso 1
                        ... bl VU0_sqrt__Ff ...
                        lfs f0,lbl_803FB6B8@l(r30)      <- uso 2
nuestro                 lis r9,$LC931@ha / lfs f0,$LC931@l(r9)    <- uso 1  (+1 insn)
                        lis r9,$LC931@ha / lfs f0,$LC931@l(r9)    <- uso 2  (+1 insn)
```

Consecuencia en el prólogo/epílogo: el objetivo salva **r30 y r31**
(`stmw r30, 0x10(r1)` / `lmw r30`) y nosotros sólo r31 (`stw r31`/`lwz r31`), con
el marco 0x30 contra 0x28. Ése es todo el `faltan 2, sobran 3` de `triage`.

**Los otros DOS usos de la misma constante (`lis r8` y `lis r9` más adelante) los
rematerializan LOS DOS LADOS**: el objetivo sólo comparte los usos 1 y 2, que son
los que domina `.L_8024B460`.

### Lo que NO es

- **No es bandera.** Diez barridas sobre la unidad real:
  `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-expensive-optimizations`,
  `-fno-force-addr`, `-fno-move-all-movables`, `-fno-rerun-loop-opt`
  → **idénticas** (97,21028 %, 860 B); `-fno-force-mem` 96,98; y
  `-fno-cse-skip-blocks` (85,16 / 892 B), `-fno-rerun-cse-after-loop`
  (84,67 / 848 B) y `-fno-schedule-insns` (75,81) **destrozan** la función.
  Que `-fno-gcse` no cambie NADA dice que en nuestro lado gcse no está tocando
  ese `elf_high` en ninguna dirección.
- **No es el sentido de las comparaciones.** `r1`…`r6`: seis permutaciones de
  `0.0f < x` / `x > 0.0f` en las cuatro comparaciones implicadas → **las seis
  idénticas al dígito**. `fold` canonicaliza el `<`/`>` antes de expandir (es la
  misma veda que la r30 midió para el `+` de punteros).
- **No es la estructura de bloques.** `t1` (sin `else`, asignando `mSlip = 0.0f`
  antes) 94,22 / 848 B; `t2` (ternario) 95,48 / 864 B; `t3` (ifs anidados) 94,22 /
  864 B; `t4` (variable local `slip`) 93,38 / 852 B. **Las cuatro peores.** La
  forma actual del `if/else` es la del objetivo (su bloque `else` tiene su propio
  `lis lbl_803FB6B0@ha`, igual que el nuestro).

### La prueba que cierra el diagnóstico

`alloc.py`/`.lreg` del volcado RTL: en NUESTRA función **ningún pseudo
`BASE_REGS` cruza llamadas salvo el `this` (reg 82, 47 usos, 3 llamadas)**. O sea
que los dos `elf_high` **nunca llegan a ser un solo pseudo**: no es un problema
del asignador, es que la fusión no ocurre en RTL. En el objetivo ese pseudo existe
y cruza dos `bl`.

> **Lo que queda por probar y yo no he hecho**: por qué el original fusiona los
> dos `high(symbol_ref)` y nosotros no, con el volcado `.gcse`/`.cse2` delante.
> Es el mecanismo del §8 de HERRAMIENTAS («`lis sym@ha` es RTL de primera clase;
> cuántos viven lo deciden `gcse`/PRE, `loop`, `cse2` y `local-alloc`») aplicado a
> una función que **no tiene bucles**, así que sólo pueden ser gcse o cse2.
> Es la función más cerca de cerrarse de todo mi encargo: **4 B y una sola causa**.

---

## 4. `RBGrid::Add` (1.488 B, 99,301 %) — VEDA NUEVA, con el mecanismo al insn

Las 8 filas son **la misma diferencia repetida 4 veces** (las cuatro copias
inline del constructor `SAP::Grid<T>::Axis::Node::Node`, en `SAP.h`). El bucle:

```cpp
while (head != nullptr && head->mPosition < this->mPosition) {
    node = head;
    head = head->mTail;
}
```

```
objetivo  .L_8024B... : cror un,eq,gt / bso salida        <- bloque COMPARTIDO
                        mr r6,r9 / lwz r9,0x4(r9) / cmpwi r9,0 / beq salida
                        lfs f13,8(r9) / lfs f0,8(r8) / fcmpu f13,f0
                        b <el cror>                       <- arista INCONDICIONAL
nuestro                 ... / lwz r9,0x4(r6) / ...
                        lfs / lfs / fcmpu
                        blt <el cuerpo>                   <- arista CONDICIONAL
```

**El mecanismo, verificado con un caso mínimo que reproduce los DOS diffs exactos**
(`scratchpad/c32nxt_min.cpp`, plantilla, 2 s por compilación):

1. `expand_end_loop` de `stmt.c` **mueve el test del `while` al final del bucle**
   (se ve en el volcado `.rtl`: `jump_insn 55: goto L63` al principio, el test en
   L63 al final, `jump_insn 71: if (lt) goto L58`).
2. Con el test abajo, la salida queda `cror; bso SALIDA` seguida de `b CUERPO`
   seguida de la etiqueta `SALIDA:`. Eso dispara **el «conditional jump jumping
   over an unconditional jump» de `jump.c`**, que invierte y borra el `b`
   → nuestro `blt`.
3. En el objetivo esa inversión NO ocurrió, y entonces las dos colas idénticas
   `cror; bso SALIDA` (la de arriba y la de abajo) las funde el **cross-jump**,
   dejando el `b` que apunta al `cror` compartido. Es exactamente el patrón del
   §8 de HERRAMIENTAS.

**Ensayos (24 en la unidad real, base 99,30108 % / 8 filas):**

| ensayo | qué se barrió | cifra |
|---|---|---|
| `g1_p` | `float p = head->mPosition;` + test sobre `p` + `break` del nulo | 92,836, **1.544 B** |
| `h1` | `while (head->mPos < …) { … if (head==0) break; }` | 88,272, **1.440 B** |
| `h2` | `for(;;)` con los dos `break` | 88,272, 1.440 B |
| `h3` | `while (head!=0) { if (…) {…} else break; }` | 86,277, 1.456 B |
| `h4` | el `while` actual + `break` redundante | 88,272, 1.440 B |
| `h5` | `head->GetPosition() <` | 86,277, 1.456 B |
| `h6` | `GetPosition()` en los dos lados + `GetTail()` | 86,277, 1.456 B |
| `h7` | `const float p = head->mPosition;` dentro del cuerpo | 88,272, 1.440 B |
| `h8` | etiqueta + `goto again` | 86,957, 1.448 B |
| `h9` | `if (…) do { … } while (…)` | **idéntica** 99,30108 |
| `ha` | `for (head = …; …; head = head->mTail)` | **idéntica** |
| `hb` | `head = head->GetTail();` | **idéntica** |
| `i1`,`i2`,`i3`,`i6` | `__asm__("")` al final / al principio / detrás del bucle / entre las dos sentencias | **las cuatro idénticas** |
| `i4` | `__asm__("")` delante del `while` | 94,65 |
| `i5` | barrera `"memory"` | no compila (`SAP.h` es plantilla, el `:::` no pasa) |
| `j1` | `bool less = …` recalculado en el pie | 92,836, 1.544 B |
| `j2` | `int less` con `if/else` | 93,55, 1.568 B |
| `j3` | `mPosition` sin `this->` | **idéntica** |
| `j4` | `position` (el parámetro) en vez de `this->mPosition` | 97,70, 1.472 B |
| `j5` | `head &&` en vez de `head != nullptr &&` | **idéntica** |
| `j6` | `head = node->mTail;` | **idéntica** |

Y en el caso mínimo, **~45 formas más** (`for`, `do/while`, `while(1)`, ternario,
`!(a>=b)`, `mSort`, `continue`, `register`, coma, condición de tres términos,
declaración fuera…) y **34 barridos de bandera** — incluidos `-fno-fast-math`,
`-fno-thread-jumps`, `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`,
`-O0/-O2/-O3/-Os`, `-ffloat-store`, `-fno-trapping-math`, `-mno-ps-nodf`:
**ninguna produce la `b` incondicional.**

> **Veda: la arista de retorno de un `while (p && p->campo < x)` no se toca desde
> el fuente.** El `blt` nace de `expand_end_loop` moviendo el test al pie y
> `jump.c` invirtiendo el par «condicional que salta por encima de incondicional»;
> las dos cosas ocurren **antes** de que el cross-jump pueda fundir las colas.
> 24 ensayos en la unidad real, ~45 en caso mínimo y 34 de banderas. Las formas
> que sí cambian la forma del bucle (`h1`…`h8`, `j1`, `j2`) **quitan una copia del
> `fcmpu` y bajan a 1.440-1.456 B**: se alejan del objetivo, no se acercan.
>
> **Aviso de alcance: `SAP.h` lo incluye también `zAI`** (vía `AIAvoidable.h`).
> Cualquier forma que se pruebe ahí exige A/B por objetos sobre las dos unidades.
> Yo no he dejado nada puesto, así que no he tenido que medirlo.

---

## 5. `FEKeyboard::ToggleCapsLock` (92 B) — veda de la r30 CONFIRMADA, 7 formas nuevas

Las 2 filas siguen siendo las de la r30: el objetivo emite `stw r10, 0x3c(r3)`
(`mbShift`) **entre** el `cmpwi r9, 0x3` y su `bne`, y en **r10**; nosotros
delante del `cmpwi` y en **r0**.

| ensayo | qué se barrió | cifra | filas |
|---|---|---|---|
| `u1` | `mbShift = false;` ANTES de `mbCaps = mbCaps != 1;` | 80,652 | 13 |
| `u2` | `mbCaps = !mbCaps;` | 76,609, **80 B** | 12 |
| `u3` | `return` temprano en vez del `if` envolvente | **idéntica** 91,08696 | 3 |
| `u4` | `MODE_FILENAME == mnMode` (operandos al revés) | **idéntica** | 3 |
| `u5` | `if/else` con `mbShift` en las dos ramas | 58,739, **104 B** | 20 |
| `u6` | `mbShift = 0;` | **idéntica** | 3 |
| `u7` | `mbShift` duplicado en las dos ramas del `if` interior | 69,043, **104 B** | 11 |

Van **37 formas entre las tres rondas**. La veda de la r30 (el desempate es
`INSN_LUID`, o sea el orden de fuente, y las formas que lo consiguen destruyen la
función) **sigue en pie y ahora con siete pruebas más**. `u3`, `u4` y `u6`
demuestran de paso que el `return` temprano, el orden de operandos del `==` y el
`0` en vez de `false` **dan el objeto idéntico**: no hay que volver a probarlos.

---

## 6. `IconScroller::IconScroller` (384 B) — veda de la r29 CONFIRMADA, 6 formas nuevas

Las filas son **puro orden de los `stfs f0` de inicialización**. Con los
desplazamientos traducidos a miembros (`FEIconScrollerMenu.hpp`):

```
objetivo   … 0xF8 fCurFadeTime … 0xFC fMaxFadeTime … 0xE4 fHeight, 0xE8 fXCenter,
              0xEC fYCenter, 0xF4 fCurrentAddPos, 0xE0 fWidth
nuestro    … 0xFC fMaxFadeTime … 0xF4 fCurrentAddPos … 0xE4 fHeight,
              0xF8 fCurFadeTime, 0xE8 fXCenter, 0xEC fYCenter, 0xE0 fWidth
```

O sea: **`fCurFadeTime` (0xF8) y `fCurrentAddPos` (0xF4) están intercambiados**, y
el objetivo emite las asignaciones **en el orden del fuente** (`fCurFadeTime`
pegada a `fMaxFadeTime`; `fCurrentAddPos` pegada a `fWidth`) mientras el nuestro
las baraja. **Nuestro fuente ya tiene ese orden**: es el planificador el que las
mueve, y son dos `stfs` de coste y dependientes idénticos.

| ensayo | permutación probada | cifra | filas |
|---|---|---|---|
| `w1` | `fHeight` bajada junto a `fXCenter` | 97,7604 | 12 |
| `w2` | `fCurFadeTime` la primera de todas | 95,6875 | 13 |
| `w3` | `fCurrentAddPos` detrás de `fWidth` | **idéntica** 97,770836 | 11 |
| `w4` | `fMaxFadeTime` antes de `fCurFadeTime` | **idéntica** | 11 |
| `w5` | el par `fCurFadeTime`/`fMaxFadeTime` antes de `fHeight` | 94,2708 | 32 |
| `w6` | `w1` + `w3` | 97,7604 | 12 |

Van **49 formas con las 43 de la r29**. La barrera está vedada aquí desde la r29
(destruye la función: 48-85 %) y no la he probado. **Veda confirmada.**

---

## 7. `FEngFont::RenderString` (1.572 B) — CERO ensayos, y por qué

La r30 gastó 5 ensayos y el volcado RTL, refutó el umbral de la r27 y dejó
escrito que **lo único vivo es el modelo del REGISTRO DURO**
(`local_reg_n_refs[]/local_reg_live_length[]` de `find_reg`). `triage` la sigue
dando como **«18 filas crudas, CERO reales»**: no falta ni sobra una instrucción.
No he abierto ese frente porque **es exactamente el mismo que bloquea las 28 filas
que me quedan en `GetLoadingPriority`** (§2.3) y me pareció más honesto dejar la
capa entera para quien la ataque con el `.lreg` delante, en las dos a la vez.

---

## 8. Ensayos numerados (70 en unidad real + ~45 en caso mínimo + 34 de banderas)

- **`RBGrid::Add` (24)**: `g1_p`, `h1`…`hb` (11), `i1`…`i6` (6), `j1`…`j6` (6). §4.
- **`GetLoadingPriority` (13)**: `m1`,`m2`, `n1`…`n5`, `p0`…`p5`. §2. Mejor:
  **`m1` 97,42938 %** (28 filas), no aplicable.
- **`UpdateLoaded` (20)**: `r1`…`r6`, `t1`…`t4`, 10 banderas. §3. Ninguna mejora.
- **`ToggleCapsLock` (7)**: `u1`…`u7`. §5. Ninguna mejora.
- **`IconScroller` (6)**: `w1`…`w6`. §6. Ninguna mejora.
- **`RenderString` (0)**: §7.
- **Caso mínimo `c32nxt_min.cpp`**: 13 + 9 + 19 + 12 formas de bucle y 27+7
  banderas. §4.

---

## 9. Vedas nuevas y palancas

**Nuevas:**

1. **La arista de retorno de un `while (p && p->campo < x)` no se toca desde el
   fuente** (§4). Mecanismo al insn: `expand_end_loop` mueve el test al pie y
   `jump.c` invierte el par condicional-sobre-incondicional antes de que el
   cross-jump pueda fundir las colas. 24+45+34 pruebas.
2. **`GetLoadingPriority` necesita DOS cosas, no una** (§2.4). Los 16 B valen 43
   de 71 filas y dejan 28 de reparto de FPR.
3. **El `face[4]` interior del original SOLAPA al exterior** (0x48 dentro de
   0x38..0x68). Ninguna declaración nuestra reproduce ese reparto: la lectura
   «falta una local de 16 B» de la r31 hay que matizarla (§2.1).
4. **En `UpdateLoaded` no es bandera** (10 barridas) **ni estructura de bloques**
   (4 formas) **ni sentido de las comparaciones** (6 formas). Queda gcse/cse2 (§3).
5. **`ToggleCapsLock`: `return` temprano, `MODE_FILENAME == mnMode` y
   `mbShift = 0` dan el objeto IDÉNTICO** — tres formas menos que probar (§5).
6. **`IconScroller`: `fCurrentAddPos` detrás de `fWidth` y `fMaxFadeTime` antes de
   `fCurFadeTime` dan el objeto IDÉNTICO** — el planificador normaliza (§6).

**Caducadas: ninguna.** Reabrí la de `IconScroller` (r29, 43 formas) y la de
`ToggleCapsLock` (r30, 30 formas) con 13 formas nuevas entre las dos: **las dos
aguantan**.

**Palanca nueva, y es de método:** para `RBGrid::Add` monté un **caso mínimo que
es una PLANTILLA**. Sin eso el caso mínimo miente: con una clase normal, GCC 2.95
**no hace inline de un miembro definido en clase si lo usa un miembro DECLARADO
ANTES** (procesa los cuerpos diferidos en orden de declaración), así que
`GetTail()`/`GetPosition()` salían como `bl` y el caso no reproducía nada.
Convertido en plantilla, reproduce **los dos diffs exactos** de la unidad real.
Es la contrapartida de la nota «En clase = inline en GCC 2.9» que hay en memoria.

---

## 10. Estado del árbol, herramientas y convivencia

- **Ficheros del árbol tocados por mí: NINGUNO.** Verificado con `git diff` al
  cerrar sobre los cinco ficheros que parcheé (`SAP.h`, `TrackStreamer.cpp`,
  `SuspensionTraffic.cpp`, `FEpkg_MU_Keyboard.cpp`, `feIconScrollerMenu.cpp`).
  Los tres `git diff` que quedan en ficheros que yo parcheé **son de OTROS
  agentes** (el frente §2 del brief, `extern` convertido en definición). Mi runner
  restaura byte a byte y los respeta:
  - `TrackStreamer.cpp`: `SeeulatorToolActive`, `ScenerySectionToBlink`,
    `ForceHoleFillerMethod`, `ShowSectionBoarder` (ya lo avisaba la r31).
  - `FEpkg_MU_Keyboard.cpp`: `gFEKeyboard`, `KeyboardActive` (ya lo avisaba la r30).
  - `feIconScrollerMenu.cpp`: `gTUTORIAL_MOVIE_DRAG`, `gTUTORIAL_MOVIE_SPEEDTRAP`
    (nuevo de esta ronda).
- **No he tocado** `configure.py`, `config/GOWE69/*`, `splits.txt`, `symbols.txt`,
  `keep.lst`, `build.ninja` ni los bloques `__ANDROID__`. **Sin commit.**
- **Herramientas nuevas en el scratchpad** (prefijo `c32nxt_`):
  - `c32nxt_link.py <unidades…>` — copia del `c31ub_link.py` de la r31.
  - `c32nxt_run.py` — el runner de la r31 con el `bak` renombrado (necesita
    `SCRATCH` en el entorno, no sólo `SCR`).
  - **`c32nxt_run2.py <unit> <src> <sym> <edits.json> [tags…]`** — el mismo, pero
    aplica **VARIAS ediciones a la vez** (JSON `{"__file__":abs, "tag":[[old,new],…]}`),
    ancla línea a línea con `splitlines(True)` y restaura en `finally`. Es lo que
    hace falta en cuanto una variante necesita tocar dos sitios (p. ej. relleno de
    marco + forma de la expresión).
  - `c32nxt_min.cpp` + `c32nxt_cc.sh` + `c32nxt_sweep/sw2/sw3/sw4.py` — el caso
    mínimo de plantilla y sus barridos (1-2 s por forma).
  - `c32nxt_flags.py` — barrido de banderas sobre el caso mínimo.
- **Stubs de `mn_repro.py` verificados** (todos reproducen el porcentaje del `.o`
  de la unidad **al dígito**):
  - `RigidBody.cpp`, `SuspensionTraffic.cpp` (zPhysicsBehaviors) y
    `TrackStreamer.cpp` (zTrack): **sin `PRE`**.
  - `FEpkg_MU_Keyboard.cpp` (zFe2): `PRE` de la r30
    (`ANIMWORLDTYPES_EMIT_MESSAGE_HASHES` + `ATTRIB_NO_INLINE_CLASSKEY` +
    `FEngInterfaces/FEngInterfaceFEObjects.hpp`) — **sigue valiendo**.
  - **`feIconScrollerMenu.cpp` (zFe2): `PRE='#include
    "Speed/Indep/Src/EAXSound/EAXSOund.hpp"'`** — nuevo. Sin él falla con
    `g_pEAXSound undeclared` en la línea 690, que no tiene nada que ver con la
    función que se mide.
- **Volcados RTL**: `c30mx_rtl.py` de la r30 funciona tal cual. Aviso de tamaño:
  con `SL=<ruta>` preprocesa **la SourceList entera** y el `.lreg` de
  zPhysicsBehaviors pesa **26 MB** (y el `.greg` 22). Los he borrado.
- **Finales de línea medidos**: `SAP.h` es **CRLF puro**; `TrackStreamer.cpp`
  tiene finales **mezclados** (git avisa `LF will be replaced by CRLF`).
  El anclaje línea a línea de los dos runners lo absorbe.
- **Disco**: entré con 19 GB y salgo con **17 GB**, tras borrar mis
  `mnd_*.json` (18 MB cada uno, ~25), los `r_*.o`, `stub_*.cpp`, los dos
  desensamblados `dtk` y el directorio de volcados RTL: **−1,1 GB**. Los
  `d_z*.json` grandes de `fndiff` (`d_zFe2`, `d_zPhysicsBehaviors`) los dejo para
  quien reuse `REUSE=1`.

---

## 11. Lo que haría el siguiente, por orden

1. **`UpdateLoaded` (zPhysicsBehaviors, 856 B): 4 B y UNA sola causa.** Volcado
   `.gcse`/`.cse2` delante, a ver por qué el original fusiona los dos
   `high(symbol_ref $LC931)` en un pseudo que cruza dos `bl` y nosotros no. Es la
   función más cerca de cerrarse de las seis.
2. **El modelo del registro duro de `find_reg`**, en `GetLoadingPriority` (28
   filas) y `RenderString` (18 filas) **a la vez**: es la misma capa y la r30 ya
   dejó el `.greg` de `RenderString` leído. Sin ella no cae ninguna de las dos.
3. **La forma legítima de los 16 B de `GetLoadingPriority`**, sabiendo ya que
   sola no cierra nada (§2.4) y que el reparto del original solapa (§2.1).
4. **Los datos, que es donde está el dinero de verdad**: `zFe2` a 5 símbolos +
   8 pares de clase local de enlazar (174.200… en realidad **250.732 B**);
   `zPhysicsBehaviors` a `.data2` +1.536 y `.data3` −416 (**236.176 B**);
   `zTrack` a `.data2` +2.048 y `.data3` +416 (**63.744 B**). Y el detalle nuevo
   de `zPhysicsBehaviors`: **`.text2` sale 8 B más arriba**.
5. **NO volver a** `RBGrid::Add` (§4), `ToggleCapsLock` (§5) ni `IconScroller`
   (§6) por la vía de la forma del fuente.
