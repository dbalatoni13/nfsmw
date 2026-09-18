# Ronda 26 — zEAXSound2: `ResolveCarBanks` CERRADA al 100 %, y la deuda de la r25 retirada

**Resultado: `ResolveCarBanks` 99,80821 % → 100,00000 %, +7.508 B, +1 función,
APLICADO al árbol.** El diff pasa de **22 filas a CERO**. Y el
`do { … } while (0)` que la r25 declaró como «constructo que el original no
tenía» **desaparece**: lo sustituye `for (; n >= 0; --n)`, que es **letra por
letra la misma forma que BeginRule3** treinta líneas más arriba.

Unidad: **158.400 → 165.908 / 170.256 B = 93,0364 % → 97,4462 %**, de 921 a
**922 funciones al 100 %**.

    ANTES   7508 B  99.80821%  ResolveCarBanks   22 filas
    AHORA   7508 B 100.00000%  ResolveCarBanks    0 filas   <- CERRADA
     392 B  94.93877%  Play…RoadNoise   29 filas — no cerrada (17 ensayos nuevos)
     340 B  95.88236%  BindToData       12 filas — no cerrada (39 ensayos nuevos,
                                                   mecanismo cerrado al dígito)

`measure.py --cmp` (base y medida seguidas, `build_direct.py` delante de las
dos): **+7.508 B, +1 función, 1 unidad cambia**.
`pctsnap.py --cmp`: **MEJORAN 1, EMPEORAN ninguna.**
`audit.py Speed/Indep/SourceLists/zEAXSound2`: **0 FALLA en dos pasadas**, y las
dos salidas son **idénticas byte a byte**; la única diferencia contra la pasada
de entrada es la línea nueva
`ResolveCarBanks__18CSTATEMGR_CarState  ok  7508 B, 204 ramas, 402 relocs, 0 literales`.
`triage.py zEAXSound2 --muro`: `ResolveCarBanks` **desaparece del listado**
(MURO baja de 7 funciones / 11.124 B a 6 / 3.616 B).
Congelada con `frozen.py cong` (huella **`70f7354e1e7b4123`**); `chk` posterior:
«idéntico al congelado».

El único fichero del árbol que he tocado es
`src/Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.cpp`
(3 hunks, 14 líneas). **Sin commit.**

---

## 1. Lo que cerró `ResolveCarBanks`: DOS cambios que sólo valen JUNTOS

```c
 BeginRule4: {
     int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
-    if (n >= 0) {                                   // (r24/r25)
+    for (; n >= 0; --n) {                           // espejo EXACTO de BeginRule3
         ...
-        do {                                        // DEUDA de la r25: retirada
-            if (FinalEngines.size() + AIEnginesWeWantToLoad.size() <= 4) {
-                goto LoadRemainingEngines;
-            }
-            goto BeginRule4;
-        } while (0);
+        if (FinalEngines.size() + AIEnginesWeWantToLoad.size() <= 4) {
+            goto LoadRemainingEngines;
+        }
+        goto BeginRule4;
     }
 }
```

```c
             if (eax_car->GetEngineInfo()->GetCollection() == mapping.Start) {
                 eax_car->GetEngineInfo()->ChangeWithDefault(mapping.Finish);
-                EngToCarStruct carmapping;
                 found = true;
 
+                EngToCarStruct carmapping;
                 carmapping.EngineKey = mapping.Finish;
```

Medido por separado y juntos (`c26eax2_run.py`, control = 1877 insns / 7508 B /
237 palabras distintas / 99,80821 %):

| # | qué | insns | B | palabras | % |
|---|---|---|---|---|---|
| `ctrl` | árbol de la r25 (`if` + `do{}while(0)` + `found` tras el ctor) | 1877 | 7508 | 237 | 99,80821 |
| `c1_fnd1st` | **sólo** `found = true;` antes del ctor | 1877 | 7508 | 314 | 99,73096 |
| `d3_forbody` | **sólo** `for (; n>=0; --n)` en todo el cuerpo (= forma de la r24) | 1877 | 7508 | 311 | 99,60842 |
| **`d1_for_fnd`** | **los dos** | **1877** | **7508** | **216** | **100,00000** |
| `d2_wh_fnd` | `while (n >= 0)` + los dos | 1877 | 7508 | 216 | 100,00000 |

**Cada uno por su cuenta EMPEORA. Sólo juntos cierran.** Ésa es la razón de que
seis rondas no lo encontraran: los dos ejes se barrieron siempre por separado.

### 1.1 Por qué `found = true` tiene que ir ANTES de `EngToCarStruct carmapping;`

`EngToCarStruct` tiene constructor que pone **los dos campos a cero**
(`EngineKey(0), pCar(nullptr)`, `STATEMGR_CarState.hpp:57`). Con la declaración
delante de `found = true`, `cse` ve que el registro de `found` **todavía vale 0**
y lo reutiliza como fuente de los dos `stw` del constructor:

    objetivo   li r24,0        <- cero PROPIO
               li r20,1        <- found = true, TEMPRANO
               stw r24,0x150(r1) ; stw r24,0x4(r27)   <- el cero propio
               stw r24,0x8(r31)                        <- y el mSize=0 de reserve()

    ctrl       li r21,0        <- cero propio (solo para el mSize=0)
               stw r22,0x150(r1) ; stw r22,0x4(r27)   <- reutiliza el 0 de `found`
               li r22,1        <- found = true, TARDE (obligado)

**Con `alloc.py` sobre la función entera** (`c26eax2_alloc_base.txt` /
`c26eax2_alloc_c1.txt`), el pseudo de `found` es el **2903**:

| forma | pseudo | n_refs | live_len | prioridad | reg |
|---|---|---|---|---|---|
| `ctrl` | 2903 (`found`) | **14** | 204 | **2058** | r22 |
| `c1` | 2903 (`found`) | **8** | 180 | **1333** | **r20** ← el del objetivo |

Las **seis referencias de más** son los ceros del constructor que `cse` le
cuelga. Al quitarlas, `found` baja de prioridad, se asigna más tarde y cae en
r20; el cero propio (pseudo 2935) deja de llegar a `global_alloc` y **`local_alloc`
le da r24**, que es exactamente donde el objetivo lo tiene. Con eso `found`,
el iterador interior y el cero **casan los tres a la primera**.

### 1.2 Por qué hace falta ADEMÁS el bucle sobre todo el cuerpo

Con `c1` solo, quedaba **una** diferencia en la cola: `eax_car` (pseudo **2889**,
prio **2608**) se llevaba **r23** en vez de r22, y eso arrastraba una permutación
global r22↔r23 de ~80 filas (el pseudo grande **3298** caía en r22 y el 1778 en
r23).

La razón es de orden de asignación. Con el bucle **sólo en la cola** (forma
`j1` de la r25) el pseudo grande 3298 tiene 137 refs → prio **2511 < 2608**, así
que se asigna **después** de 2889 y coge lo que quede. Con el bucle envolviendo
**todo el cuerpo** (forma de la r24) sube a 148 refs → prio **2713 > 2608** y se
asigna **antes**… y en la r24 eso le daba r24 y destrozaba el reparto.

Lo que cambia con `c1` aplicado es que **r24 ya está ocupado por el cero de
`EngToCarStruct` que asigna `local_alloc`**. Entonces:

    3298 (prio 2713, primero) -> r24 ocupado -> r23   <- el del objetivo
    2889 (prio 2608, despues) -> r24 y r23 ocupados -> r22   <- el del objetivo

**El bucle grande, que sin `c1` era veneno (99,60842), con `c1` es justo lo que
falta.** Y como el `-1`… perdón, como el pseudo del cero le roba r24 al bucle
grande, ya no hace falta el `do{}while(0)` de la r25 para bajar `n_refs`: el
alcance grande vuelve a ser el bueno.

### 1.3 La deuda de la r25 queda RETIRADA, y la forma aplicada no es inventada

`for (; n >= 0; --n) {` sobre el cuerpo entero es **la misma sentencia, palabra
por palabra, que BeginRule3** (`STATEMGR_CarState.cpp:232-233`):

    BeginRule3: { int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
                  for (; n >= 0; --n) { …
    BeginRule4: { int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
                  for (; n >= 0; --n) { …

Semánticamente es idéntico a `if (n >= 0)`: **el cuerpo siempre sale por un
`goto`**, así que ni `--n` ni la arista de retorno son alcanzables (lo mismo que
razonó la r25 al medir `c3_for_base` = `c5_wh_base`). **No queda ningún
constructo declarado como invención en esta función.**

`while (n >= 0)` da el **mismo objeto exacto** (`d2_wh_fnd`, 100 %); he aplicado
el `for` por el espejo con BeginRule3.

---

## 2. `BindToData` (95,88236 %) — mecanismo CERRADO al dígito, sin palanca

No la he movido, pero ya no queda nada que adivinar: **la única diferencia es
dónde se planifica el `li -1`**, y de ahí sale todo lo demás.

    objetivo   ...  47 lwz r0,0x18(r30) · 49 stw r0,0x14(r31) · 50 mr r7,r0
                    51 li r0,-1                      <- TARDE, y en r0
                    55 stw r0,0xac(r31)
    nuestro         33 li r8,-1                      <- 18 ranuras antes, y en r8
                    55 stw r8,0xac(r31)
                    59 mr r8,r5                      <- la instruccion que sobra

La cadena causal, verificada pieza a pieza:

1. **`sched1` mete el `li -1` en la ranura libre del ciclo 2.** El volcado
   `-fsched-verbose-5` lo dice literalmente (`c26eax2_dump.py … CC1X=-fsched-verbose-5`):

       ;;  Ready list initially:            222  110  170  118
       ;;  Ready list (t = 1):  --> 118 (lsu)   --> 170 (iu2)
       ;;  Ready list (t = 2):  --> 110 (iu2)   --> 222 (iu2)      <- 222 es el `li -1`

   `222` tiene prioridad 3 (= 1 + la del `stw`), la misma que `110` (`i = 0`), y
   la máquina admite dos insns en `iu2` por ciclo: **entra gratis**. En el
   objetivo el ciclo 2 emite **sólo** `li r5,0`, o sea que allí el `li -1`
   **no estaba en la lista de preparadas**.
2. **Rango largo → `local_alloc` no puede darle r0.** El `.lreg` dice
   `Register 153 used 2 times across 44 insns in block 5` y `;; Register 153 in 8.`
   En ese rango r0 está ocupado por el `memdata->sampleCount` de las filas 47-50,
   así que el primer libre en `REG_ALLOC_ORDER` es r8.
3. **r8 ocupado → el `mCyclePos` global cae en r5.** `alloc.py` sobre la función
   (`scripts/alloc.py rtl26/gbase/stub.ii BindToData --sub`):

       pseudo  n_refs  live_len  prio   reg
       160     5       13        7692   r8    <- copia de mCyclePos del precabezal
       142     4       17        4705   r5    <- mCyclePos (add r5,r10,r9)
       (153, el -1, NO llega a global_alloc: lo asigna local_alloc)

   Como 160 y 142 acaban en registros distintos, **la copia del precabezal
   sobrevive**: ése es el `mr r8,r5` de más (`triage`: `sobran 1 … mr-1`).
4. **En el objetivo el `-1` está en r0 y por eso el `li` va a la fila 51**: r0
   lo ocupa `sampleCount` hasta la fila 50 (`mr r7,r0`), y `sched2` no puede
   subir el `li r0,-1` por encima de esa antidependencia. Es decir: **la fila 51
   del objetivo es CONSECUENCIA de que el pseudo esté en r0, no la causa**.

### 2.1 El dilema medido, con su frontera exacta

He barrido la posición de `this->mCurrentBlock = -1;` por **las catorce líneas**
del cuerpo (`g141`…`g154`) y hay una frontera dura en la línea 148
(`this->mFreqPos = …`):

| posición del `-1` | insns | B | % | qué pasa |
|---|---|---|---|---|
| antes de 141-147 (`g141`…`g147`, `f5`, `f7`, `k2`) | **85** | **340** | 91,15-93,74 | **registros PERFECTOS** (`li r5,0`, `add r8,r10,r9`, `li r0,-1`, **sin `mr`**) pero el `stw 0xac` se planifica en la fila 26-49 en vez de la 55 |
| antes de 148, 149, 150 | 86 | 344 | 95,80 | rango largo, vuelve el `mr` |
| **151 (actual)**, antes de 152 y 154 | 86 | 344 | **95,88236** | rango largo, `stw` en la fila 55 (correcta) |
| antes del `#ifdef` (`k1_pre_ifdef`) | **85** | **340** | 94,98 | registros perfectos; el par `li`+`stw` se va a las filas 25/26 |

**No hay posición que dé las dos cosas**: o el `stw` está en su sitio y el `li`
se iza 18 ranuras, o el `li` es corto y el `stw` se adelanta con él.

### 2.2 El DWARF del original, leído (`dwbody.py … both`)

    ORIGINAL                                NUESTRO
    void BindToData(void * ptr /* r30 */)   void BindToData(void * ptr /* r4 */)
        GinsuDataLayout * memdata;              GinsuDataLayout * memdata; // r30
        int minperiod; // r7                    int i;        // r4
        { // 0x800EAEEC -> 0x800EAF88           int minperiod; // r7
            int i; // r5                        { int period; // r0 }
            { int period; // r0 }           }
        }

O sea: en el original **`i` vive en un bloque léxico y `minperiod` no**, y `ptr`
sobrevive a la coalescencia mientras que a nosotros nos sobrevive `memdata`.
**Medido y NO sirve**: `for (int i = 0; …)` (`e1`), `int minperiod;` arriba
(`e2`), los dos (`e3`), `int i;` arriba (`e5`) — **todas 86 insns / 344 B**, y
`e1/e3/e5` bajan a 95,87059.

### 2.3 Lo que queda como veda

**Veda: ninguna posición ni grafía de `this->mCurrentBlock = -1;` alcanza a la
vez el registro y la ranura.** Barridas 14 posiciones de la sentencia, 11 de
`int minperiod = …`, `~0`, constante local `const int noblock = -1;`, el bloque
léxico para `i` en tres alcances, `for (int i…)`, `int i;` a nivel de función y
el bloque de punteros adelantado. **Total 39 variantes, todas revertidas.**

**Lo que NO he probado y es por donde seguiría:** hacer que el pseudo del `-1`
**no reciba registro** para que `reload` lo rematerialice en r0 delante del uso
(que es lo que explica los cuatro hechos del objetivo a la vez). Eso pide o
subir la presión de registros del bloque, o que `update_equiv_regs` borre el
`li` — y esa ruta no la he sabido disparar desde el fuente.

---

## 3. `Play__16CARSFX_RoadNoise` (94,93877 %) — el corte de los ceros, precisado

Confirmo el diagnóstico de la r25 y lo aprieto: **al objetivo le sobra
exactamente UN `li 0`**; nosotros tenemos 6 `li` y él 7.

    objetivo   li r29,0  -> volume(0x8), azimuth(0x10)  Y  refCount=0 (0x8(r1), fila 87)
               li r0,0   -> wetFX(0x2c), type(0x14), secondaryNoise(0x18),
                            speed(0x1c), hiPass(0x24)      (muere antes del bl)
    nuestro    li r30,0  -> los SIETE campos Y el refCount

El corte cae entre `azimuth` (4.º argumento) y `type` (5.º), y el grupo largo
(r29) **no es contiguo en el fuente**: junta el 2.º y el 4.º cero con el
**último** de todos (el `refCount = 0` de `GetRefCount`).

**El DWARF de `Play` es IDÉNTICO al del original**: mismos bloques, mismos once
inlines de setter, mismos rangos relativos salvo que el cuerpo del constructor
del original mide **4 B más** — que son justo ese `li` de más. No falta ninguna
sentencia.

Ensayos nuevos (17), ninguno sirve:

| # | qué | Play | InitSFX |
|---|---|---|---|
| `p0` control | — | 97 insns, 388 B, **94,93877** | 100 |
| `p1_noblk` | sin el bloque anónimo del `new` | 388 B, 94,93877 | 100 |
| `p2_ptr` | puntero local intermedio | 376 B, 88,82653 | 100 |
| `p3_refout` | `refcnt` fuera del bloque | 388 B, 94,93877 | 100 |
| `p4_this` | `this->` explícito | 388 B, 94,93877 | 100 |
| `p5_scnafter` | `SetCsisName` después del `new` | 388 B, 84,74490 | 100 |
| `p6_noref` | sin `refcnt` | 352 B, 81,43877 | 100 |
| `q1_2if` | `SetType` con dos `if` | 388 B, 94,93877 | 100 |
| **`q2_direct`** | `SetType` asigna en cada rama | **392 B**, 92,38776 | 99,8919 |
| **`q3_noclamp`** | `SetType` sin clamp | **392 B**, 92,38776 | 99,8919 |
| `q5_nothis` | setters sin `this->` | 388 B, 94,93877 | 100 |
| `r1_intparam` | `SetType(int x)` | 388 B, 94,93877 | 100 |
| `s1_rc2` | `int refCount; refCount = 0;` | 392 B, **94,93877** | **100** |
| `s2_wetearly` | `SetFilter_Effects_Wet_FX` tras `SetAzimuth` | 392 B, 94,92857 | 99,7568 |
| `s3_zlast` | los cinco ceros al final del ctor | 392 B, 94,93877 | 100 |

**`q2_direct` y `q3_noclamp` son el DECIMOCUARTO caso de «el tamaño exacto
miente»**: dan los **392 B clavados** y bajan a 92,39; el `li` de más que
aparece no es el segundo cero sino el `stw` del `id` adelantado, y de regalo
`InitSFX` cae a 99,89.

Corolario útil que confirma la r25: **`InitSFX` aguanta**. `s1_rc2` y `s3_zlast`
cambian el objeto de `Play` (392 B) **sin mover `InitSFX` del 100 %**; el eje de
la cabecera sigue abierto, pero ninguna de las once formas nuevas parte el grupo
de ceros.

---

## 4. Ensayos numerados de esta ronda (con su cifra)

Control `ResolveCarBanks` = 1877 insns / 7508 B / 237 palabras / 99,80821 %.

### `ResolveCarBanks` (10)
`c1_fnd1st` 314 / 99,73096 · `c2_fndlate` 237 / 99,80821 · `c3_fndpost` 522 /
99,16622 · `c4_fndmid` 237 · `c5_noblank` 237 · `c6_fndtop` 314 / 99,73096 ·
`d3_forbody` 311 / 99,60842 · **`d1_for_fnd` 216 / 100,00000** ·
**`d2_wh_fnd` 216 / 100,00000**.

### `BindToData` (39)
`e1_forscope` 95,87059 · `e2_mptop` 95,88236 · `e3_both` 95,87059 ·
`e4_cbafter` 95,88236 · `e5_itop` 95,87059 · `e6_cblast` 95,88236 ·
`f4_iprefor` 95,87059 · **`f5_cb1st` 85 insns / 340 B / 91,15294** ·
`f6_cb147` 95,80000 · `f7_cbtop` 91,15294 · `g141`…`g154` (13) ·
`h141`…`h151` (11) · `hx_not0` 95,88236 · `hx_local` 95,88236 ·
`hx_ptrs1st` 71,02353 · **`k1_pre_ifdef` 85 insns / 340 B / 94,97647** ·
`k2_in_endian` 91,15294.

### `Play` (17)
`p0`…`p6` · `q1_2if` · `q2_direct` · `q3_noclamp` · `q5_nothis` ·
`r1_intparam` · `s1_rc2` · `s2_wetearly` · `s3_zlast` (cifras en §3).

### Vedas nuevas
| # | qué se barrió | cifra |
|---|---|---|
| `found = true` detrás del ctor de `carmapping` | `c2`, `c4`, `c5` | **objeto idéntico al control**: da igual dónde vaya mientras esté detrás de la declaración |
| `found = true` después del `push_back` | `c3_fndpost` | 522 palabras, 99,16622 — **cambia la semántica del bucle** |
| **posición de `this->mCurrentBlock = -1;`** | 14 posiciones (`g141`…`g154`, `f5`, `f7`, `k1`, `k2`) | frontera dura en la línea 148: o registros o ranura, **nunca las dos** |
| **posición de `int minperiod = this->mSampleCount;`** | 11 posiciones (`h141`…`h151`) | 141-146 → 93,29412; 147-151 → 95,88236. Ninguna quita el `mr` |
| el `-1` como `~0` o como constante local | `hx_not0`, `hx_local` | objeto idéntico |
| el bloque léxico de `i` del DWARF | `e1`, `e3`, `e5`, `f4` | 95,87059: **más fiel al DWARF y PEOR** |
| `SetType` sin clamp / con asignación directa | `q2`, `q3` | 392 B clavados y −2,5 pp; **`InitSFX` baja a 99,89** |
| `refcnt` fuera del bloque / sin bloque / con puntero | `p1`, `p2`, `p3` | ninguna parte el grupo de ceros |

---

## 5. Lo que NO he probado

- **`Play`**: el permutador (ni guiado ni ciego); `alloc.py` sobre los pseudos de
  los ceros (no lo he mirado: el diagnóstico se quedó en el conteo de `li`);
  formas de `SetVolume`/`SetAzimuth` (sólo toqué `SetType`, `GetRefCount` y el
  orden del ctor); y **por qué el grupo r0 del objetivo empieza por `wetFX`**,
  que es el último setter del fuente y sale el primero de su grupo — es la única
  pista de orden que queda sin explicar.
- **`BindToData`**: forzar que el pseudo del `-1` **no reciba registro**
  (§2.3); el permutador; y variantes de la cabecera `ginsu.h` (no la he tocado).
- **El racimo `srawi` de `UTLVector.h`** que la r25 dejó anotado: **ya no existe**
  — cerró solo con `d1_for_fnd`. No he tocado esa cabecera.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, los bloques
  `__ANDROID__`, `ENVIRO_AEMS.h` ni `ginsudata.cpp` del árbol (los barridos de
  cabecera y de `BindToData`/`Play` se hicieron con **árbol sombra** y copias en
  el scratchpad). **Sin commit.**

---

## 6. Herramientas dejadas en el scratchpad (prefijo `c26eax2_`)

| | |
|---|---|
| **`c26eax2_run.py <variantes.py>`** | compila cada variante y da **insns / B / palabras distintas / % de objdiff de VARIOS símbolos a la vez**. El módulo define `BASE_FILE`, `SYM` o `SYMS`, `UNIT`, `EXTRA` y `VARIANTS`. Ciclo: **2,5 s por variante** |
| **`c26eax2_dump.py <cpp> <tag> [pases]`** | vuelca los `.lreg`/`.greg` (u otros) de un `.cpp` suelto con los cflags reales; imprime la base para `scripts/alloc.py`. Env `EXTRAF` (flags de preproceso) y **`CC1X` (p. ej. `-fsched-verbose-5`, que deja `rtl26/<tag>/cc1.err`)** |
| `c26eax2_range.py <o> <sym> <lo> <hi>` | vuelca un RANGO de filas del diff (no sólo las que difieren): es lo que hace legible una permutación |
| `c26eax2_hsw.py` | el barrido de cabecera con árbol sombra de la r22, **ahora con porcentaje** |
| `c26eax2_rows.py`, `c26eax2_pct.py` | copias de `c22eax2_rows2.py` y `c22eax2_pct.py` |
| `c26eax2_helper.py` / `_gh.py` / `_rh.py` | líneas exactas (finales mixtos) de `STATEMGR_CarState.cpp`, `ginsudata.cpp` y `CARSFX_Roadnoise.cpp` |
| `c26eax2_alloc_base.txt` / `_c1.txt` | las tablas de `alloc.py` de §1.1 |
| `c26eax2_audit1/2/3.txt` | las tres pasadas de `audit.py` |

Se reusan sin cambios `c21eax2_var.py` y `c21eax2_fn.py`.

## 7. Convivencia

Entré con el disco al 98 % (**14 GB** libres) y salgo igual (**14 GB**). Borrados
todos mis `.o` y `.ii` de variante y los volcados RTL que no documentan nada;
queda `rtl26/` con 45 MB (los dos volcados de §1.1 y §2). No he tocado ficheros
de otros agentes: durante la ronda había cambios ajenos en `AIPursuit.cpp`,
`eagl4supportdlopen.cpp`, `EAXAemsManager.cpp` (que es de **zEAXSound**, no de
zEAXSound2), `Geometry.cpp` y `bPrintf.cpp`, y ninguno entra en mi unidad.
