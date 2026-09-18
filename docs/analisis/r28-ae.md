# Ronda 28 — zEAXSound + zEagl4Anim

Encargo: aplicar el filtro por `low_pc` a `FindMatchTime`, `EvalState` y
`DataLoadCB`; retirar la falsificación `__ct8` de `UpdateParams` y medir.

## 0. Línea base VERIFICADA

`build_direct.py zEagl4Anim zEAXSound` + `triage.py --muro`, medidos por mí:

    2352 B  98.876%  zEagl4Anim  Initialize__…DynamicLoader   faltan 3, sobran 4, 7 SUST
    2152 B  91.816%  zEAXSound   Play__14cSampleWarpper       muro, dmax 7
    1480 B  99.459%  zEagl4Anim  EvalSQT__…FnStatelessQ       faltan 0, sobran 2 (clrlwi)
    1008 B  98.671%  zEAXSound   SetupNextLoad                muro, dmax 2
     896 B  98.170%  zEAXSound   UpdateParams__16SFXCTL_MasterVolf  49 registros
     720 B  97.361%  zEagl4Anim  FindMatchTime                muro, dmax 7
     716 B  99.804%  zEAXSound   UpdateRPM                    4 registros
     456 B  93.509%  zEagl4Anim  EvalState__…FnRawStateChan   faltan 2, sobran 2
     384 B  99.688%  zEAXSound   Play__13cStichWrapper        4 registros
     356 B  94.303%  zEAXSound   startnextrequest             muro, dmax 2
     320 B  94.487%  zEAXSound   DataLoadCB                   muro, dmax 2
     268 B  96.761%  zEAXSound   MsgPlayMiscSound             muro, dmax 6
     168 B  90.238%  zEAXSound   ResolveCurrentDataMemory     muro, dmax 2

**Coincide con el encargo instrucción a instrucción.** `audit.py` al empezar:
**zEagl4Anim 314 ok / 0 FALLA**, **zEAXSound 764 ok / 0 FALLA**.

---

## 1. RESULTADO

| | antes | después |
|---|---|---|
| `DataLoadCB__14EAXAemsManagerii` (320 B) | 94,48750 % | **100 % — CERRADA** |
| `UpdateParams__16SFXCTL_MasterVolf` (896 B) | 98,17000 % (con `__ct8`) | 98,16964 % — **falsificación retirada a coste cero** |

`measure.py --cmp` (base y medida SEGUIDAS: revertir mis dos cambios →
`build_direct.py` → medir → restaurar → `build_direct.py` → medir; era
obligatorio porque **otro agente está tocando `SpeechManager.cpp`, que también
entra en zEAXSound**):

    +320 B, +1 funciones, 1 unidades cambian
       +320 B  +1 fns  Speed/Indep/SourceLists/zEAXSound   144824 -> 145144

`pctsnap.py --cmp`: **EMPEORAN: ninguna.** `+5,513 pp` en `DataLoadCB`. Las otras
cinco filas que mejoran (zPlatform, zSpeech, zDynamics, zPhysics) son de otros
agentes.

`audit.py` al cerrar: **zEagl4Anim 314 ok / 0 FALLA**, **zEAXSound 765 ok /
0 FALLA**. `DataLoadCB` audita entera: **320 B, 12 ramas, 30 relocs, 0 literales**.
`frozen.py`: zEagl4Anim **idéntica a la congelada** (no la he tocado en neto);
**zEAXSound re-congelada** (`fecb28500c472d12`) — a diferencia de la r27, aquí sí
la he tocado y audita limpia.

**Ficheros tocados, dos:** `src/Speed/Indep/Src/EAXSound/EAXAemsManager.cpp`
(una línea) y `src/Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_MasterVol.cpp` (retirada
de `__ct8`). Ningún ensamblador, ninguna cabecera, sin commit.

---

## 2. `DataLoadCB` (320 B) — CERRADA, y la regla es nueva y extrapolable

La r27 dejó localizada la causa: el objetivo iza `lis r29, StartBankLoadTicks@ha`
**por encima de `bl bGetTicker__Fv`** y lo guarda en un registro salvado, y
`-fno-schedule-insns` demostró que **no era `sched1` bajándolo: nunca estuvo
arriba**. Faltaba «qué forma de fuente legitima la dirección ANTES de evaluar la
llamada». Es esto:

    // volcado del original, línea 529627
    int StartBankLoadTicks;   // size 0x4, address 0x8041780C
    // nuestro EAXAemsManager.cpp:33
    extern unsigned int StartBankLoadTicks;   // <- MENTÍA

`bGetTicker()` devuelve `unsigned int`. Con el global declarado `unsigned int` la
sentencia es `StartBankLoadTicks = bGetTicker();` y el árbol es un **`CALL_EXPR`
pelado**; con el global declarado `int` la conversión implícita lo envuelve en un
**`NOP_EXPR`**. Y `expand_assignment` de GCC 2.9 tiene este atajo:

```c
  /* If the rhs is a function call and its value is not an aggregate,
     call the function before we start to compute the lhs. ... */
  if (TREE_CODE (from) == CALL_EXPR && ! aggregate_value_p (from) ...)
```

**La condición es `TREE_CODE(from) == CALL_EXPR` exacto.** Con el `NOP_EXPR`
delante el atajo no entra, se cae al camino ordinario —`to_rtx = expand_expr(to)`
primero, `store_expr(from, to_rtx)` después— y el `(high (symbol_ref))` nace
ANTES de la llamada, con lo que vive en un registro salvado y abre `stwu -0x18` +
`stmw r29`.

### Ensayo

| | cambio | % |
|---|---|---|
| base | `extern unsigned int StartBankLoadTicks;` | 94,48750 |
| **c1** | **`extern int StartBankLoadTicks;`** | **100,00000** |

Un solo carácter de tipo. `int` no cambia el mangling de un global en C++, así
que el símbolo no se mueve; el `static_cast<int>(StartBankLoadTicks)` de la línea
634 se queda en un no-op y no mueve nada (`triage` de la unidad entera: cero
regresiones).

### La regla, para el resto del árbol

> **Si el objetivo iza un `lis sym@ha` por encima de un `bl` y ese `sym` es el
> DESTINO de la asignación cuyo lado derecho es esa llamada, el original NO tenía
> un `CALL_EXPR` pelado: había una conversión de tipo.** Y la conversión suele
> estar en el TIPO DECLARADO del global, que el volcado da literalmente. Es
> «cabeceras que mienten» aplicado a un `extern` de una línea.
>
> Corolario: **el atajo también se cae con `a = (T)f();`, con `a = f() + 0;` y
> con cualquier envoltorio**; si el objetivo hace lo contrario (el `lis` DEBAJO
> del `bl`), lo que hace falta es que los tipos coincidan exactamente.

---

## 3. `UpdateParams` (896 B): la falsificación `__ct8`, retirada — qué pasó

**Primero la prueba del brief** (contar líneas de DWARF del original en el rango
`0x800C28BC..0x800C2C3C`): **224 instrucciones, 139 con anotación de línea y 78
líneas de fuente distintas**, repartidas entre 11 ficheros
(`SFXCTL_MasterVol.cpp` 89, `SndBase.hpp` 50, `FEDatabase.hpp` 14…). Son
**decenas**, no 2-4: el original es C++, no `asm`. La prueba sale POSITIVA para
código C, o sea que el rango no está falsificado como bloque; lo que estaba
falsificado era **el nombre de una local**.

El volcado (`Range: 0x800C28BC`) da el interfaz exacto:

    void SFXCTL_MasterVol::UpdateParams(float t /* f1 */) override {
        float fvol;        // f0
        int RandarRange;   // r9
        /* anonymous block */ { float fMasterVol; // f6
                                int nvolindex;    // r4
                                int nmastervol;   // sin registro
                                int nvol; }       // r0

**Dos locales de función, ni una más: `__ct8` no existe en el original.**

### Los tres ensayos

| | forma de la última sentencia | % | filas |
|---|---|---|---|
| base | `__typeof__(RandarRange) __ct8 = …; RandarRange = __ct8; SetDMIX_Input(10, __ct8);` | 98,17000 | 49 |
| **c2** | `RandarRange = …; SetDMIX_Input(10, RandarRange);` | **98,12500** | **51** |
| **c3** | `fvol = mStaticRange*32767.f; RandarRange = (int)fvol; SetDMIX_Input(10, RandarRange);` | **98,08036** | — |
| **c4** | `this->SetDMIX_Input(10, static_cast<int>(RadarDetector::mStaticRange * 32767.0f));` | **98,16964** | **49** |

**c4 reproduce exactamente el resultado de `__ct8`** (mismas 49 filas, y las dos
últimas —`lwz r9,0x44(r1)` / `stw r9,0x28(r11)`— pasan a casar), con una
sentencia C++ normal y sin nombre de temporal del compilador. **Es la forma que
queda en el árbol: la falsificación está retirada a coste cero.**

Por qué funciona: `RandarRange` también recibe `g_EAXIsPaused()` (que llega en
r3), y eso ata el pseudo a r3. Si la última sentencia no lo reusa, el valor final
es un pseudo nuevo de vida corta y `local-alloc` le da **r9**, que es el primer
GPR del `REG_ALLOC_ORDER` de rs6000 después de r0 (`0, 9, 11, 10, 8, 7, 6, 5, 4,
3, 31…`). Eso es lo que el `__ct8` estaba comprando a mano.

**Lo que queda son 47 filas de permutación de registros de coma flotante**, con
`fMasterVol` en **f11** (nuestro) contra **f6** (objetivo) y la constante `1.0f`
en f12 contra f9. Estructura idéntica instrucción a instrucción; sólo cambian los
nombres. `matched_code` es todo-o-nada, así que la función sigue valiendo cero
bytes — pero ya no tiene ningún constructo inventado dentro.

---

## 4. `FindMatchTime` (720 B): el filtro por `low_pc` sale NEGATIVO, y ahora con el volcado

Era el encargo explícito. Volcado del original, `Range: 0x8009E0D0 -> 0x8009E3A0`:

    bool FnRunBlender::FindMatchTime(const MatchPhaseInput & input /*r4*/, float & time /*r26*/) const {
        struct PhaseValue phase; // r1+0x8
        float a; //f31    float da; //f13   float na; //f11
        int n; //r28      int s; //r25      int i; //r30      int minIdx; //r27
        float diffAngle; //f0  float minAngle; //f30  float angle; //f28  float dAngle; //f26
        struct FnRunBlender * nonConstThis;
        inline PhaseValue::PhaseValue() @0x8009E0FC
        inline int FloatToInt(float f)  @0x8009E0FC
        inline int FloatToInt(float f)  @0x8009E184
    }

**Nuestro `FnRunBlender.cpp:393` tiene las TRECE locales con el mismo nombre y en
el mismo orden, un solo `PhaseValue phase;` y exactamente dos `FloatToInt`.**
No hay ni una sentencia que falte ni una que sobre. Esto **confirma con el volcado**
(no sólo con `regmap`) el «IDÉNTICO» de la r27.

Las 8 filas siguen siendo la pareja `lis $LC326@ha` + `lfd f13` (la constante
`0x4330…` de la conversión entero→float) que el objetivo carga 12 ranuras antes,
más el f13↔f12 que arrastra. **Sigue siendo permutador CIEGO; no lo he lanzado.**

---

## 5. `EvalState` (456 B): el filtro por `low_pc` también sale NEGATIVO, y la causa es CSE

Volcado, `Range: 0x800A3090 -> 0x800A3258`: locales `c` r30, `i` r31, `k` r4,
`dest` sin registro — **las cuatro que tenemos** — más 5 `GetKeyData`,
5 `GetKeySize` y 3 `GetNumKeys` en línea, que son exactamente los que salen de
nuestras cinco llamadas a `GetKeyData` (cada una expande su `GetKeySize`
interno). **Ni una sentencia de más ni de menos.**

Las 2 filas reales están localizadas al dígito, y **no son de fuente**:

    fila 40   NUESTRO  add r4, r30, r10     OBJETIVO  mr r4, r11
    fila 80   NUESTRO  add r4, r30, r9      OBJETIVO  mr r4, r9

Los dos brazos de `GetKeyData` (`&mDecodeData[mNumFields]` y `… + 1`) comparten
base. El objetivo calcula esa base una vez en el precabezal del bucle
(`add r11, r30, r9`) y dentro del bucle el brazo impar es una **copia**; nosotros
mantenemos vivos **el desplazamiento y la base a la vez** (`r10` y `r8`) y
**recalculamos** `r30 + r10`. Es un fallo de CSE/PRE, no un hueco estructural.

### Ensayos y vedas

* **g1** — `GetKeyData` con una base explícita compartida
  (`unsigned short *base = &mDecodeData[mNumFields];` y los dos brazos sobre
  `base`): **49,48544 % y 412 B** (el objetivo son 456). GCC pliega el `if`
  entero. **Revertido.** Sentencia barrida: **el cuerpo de
  `RawStateChan::GetKeyData(int)`**.
* Diagnóstico de banderas (sólo diagnóstico): base 93,50877 ·
  `-fno-gcse` **67,84210** · `-fno-schedule-insns2` **87,92982** ·
  `-fno-schedule-insns` **86,40351**. **Ninguna acerca.**

Con las tres formas de la r26/r27 (`mKeyIdx` directo 89,868 · reusar `i`
83,412 · `i = mKeyIdx` en los dos bucles 83,451) van **cuatro formas medidas** y
es el **cuarto negativo** de esta función.

---

## 6. `ResolveCurrentDataMemory` (168 B): mecanismo IDENTIFICADO — es `sched2`, y hay bandera que la cierra

No estaba en mi encargo pero es la que más cerca está, y ha dado el hallazgo de
método más útil de la ronda después del §2.

Las 4 filas son **una pareja intercambiada, dos veces**:

    OBJETIVO                              NUESTRO
    lwz r4, 0x28(r9)                      lis r3, TheTrackStreamer@ha
    lis r3, TheTrackStreamer@ha           lwz r4, 0x28(r9)
    addi r3, r3, TheTrackStreamer@l       addi r3, r3, TheTrackStreamer@l
    bl FreeUserMemory__13TrackStreamerPv  bl …

Medido con `c27ae_v.py` (reproduce la unidad al dígito, 90,23810 % / 168 B):

| banderas | % | orden que sale |
|---|---|---|
| base (las dos activas) | 90,23810 | `lis ; lwz ; addi` |
| `-fno-schedule-insns` (sólo sched1 apagado) | 90,23810 | `lis ; lwz ; addi` |
| **`-fno-schedule-insns2`** | **100,00000** | **`lwz ; lis ; addi`** |
| las dos apagadas | 90,23810 | `lis ; addi ; lwz` |
| `-fno-sched-interblock` | 90,23810 | — |
| `-fsched-spec-load` | 90,23810 | — |

O sea, y esto es lo que hay que llevarse:

1. **El orden de generación de RTL es `lis ; addi ; lwz`** (`expand_call` computa
   `this` —argumento 0— antes que el argumento 1, en `precompute_register_parameters`,
   que va en orden creciente). Eso lo prueba la fila «las dos apagadas».
2. **`sched1` produce EL ORDEN DEL OBJETIVO** (`lwz ; lis ; addi`) —lo prueba la
   fila `-fno-schedule-insns2`, que da 100 %— y lo hace por el nivel 2 de
   `rank_for_schedule`, `INSN_REG_WEIGHT`, **que sólo actúa en `sched1`**
   (brief §2.3).
3. **`sched2` lo DESHACE.** Con las prioridades post-reload `lis` gana el ciclo 0,
   `addi` no está listo hasta el ciclo 1 y el `lwz` se cuela en el hueco.

**Ensayo h1** (local temporal para el argumento en las dos llamadas miembro, para
adelantar el `lwz` en el orden de RTL): **90,23810 %, idéntico**. Como era de
esperar: la prioridad manda antes que el LUID.

**Ensayo c5** (reescribir la función SIN locales, que es lo que dice el volcado —
`void EAXAemsManager::ResolveCurrentDataMemory() {}`, cero locales y cero
inlines): **77,675 % y 160 B**. Escribiendo `gAEMSMgr.m_pCurLoadSDLP->…` directo,
GCC **pliega el desplazamiento en el símbolo** (`lwz r9, gAEMSMgr+0x118@l(r9)`) y
desaparece el `addi r31, r9, gAEMSMgr@l` que el objetivo sí tiene. **Revertido.**
*Nota de método: aquí el volcado DWARF miente por omisión — el objetivo necesita
`&gAEMSMgr` en un registro, que es justo lo que produce nuestra local `mgr`.*

**No hay palanca de fuente conocida** y `-fno-schedule-insns2` es una bandera de
unidad (764 funciones) que además **empeora a dos de sus vecinas**, así que no la
propongo. Queda documentado el mecanismo exacto para quien la retome.

### Y NO es un frente: la bandera no generaliza

Medido en las otras cuatro «reorden local» de zEAXSound:

| | base | `-fno-schedule-insns2` |
|---|---|---|
| `ResolveCurrentDataMemory` | 90,238 | **100,000** |
| `startnextrequest` | 94,303 | 96,629 (mejora, no cierra) |
| `SetupNextLoad` | 98,671 | **97,393 (peor)** |
| `MsgPlayMiscSound` | 96,761 | **92,060 (peor)** |

(Aplicando la regla de «extrapolar frentes»: **un hallazgo en una unidad no es un
frente hasta contarlo en las demás**. Aquí no lo es.)

---

## 7. `EvalSQT` (1480 B): a dos instrucciones, y cinco formas vedadas

No estaba en el encargo; es el segundo bocado más grande de zEagl4Anim y su firma
—`faltan 0, sobran 2, de ellas 0 SUST`— es la más tratable que hay. Las dos filas
son el mismo patrón repetido (filas 54 y 161):

    NUESTRO   subi r0, r5, 0x1 ; clrlwi r0, r0, 16 ; cmpw r30, r0
    OBJETIVO  subi r0, r5, 0x1 ;                     cmpw r30, r0

El `clrlwi` lo pone **el `static_cast<unsigned short>` de `FnStatelessQ.cpp:74` y
`:132`**: `if (floorKey >= static_cast<unsigned short>(statelessQ->mNumKeys - 1))`.

**Sentencia barrida: esa comparación (las dos ocurrencias).** Cinco formas, todas
peores, y las cuatro sin truncado **meten una extensión ANTES** (`clrlwi r9, r5, 16`)
en vez de quitarla:

| | forma | % | filas |
|---|---|---|---|
| base | `>= static_cast<unsigned short>(mNumKeys - 1)` | **99,45946** | 2 |
| f1 | `>= mNumKeys - 1` | 98,07527 | 17 |
| f2 | `>= static_cast<int>(mNumKeys) - 1` | 98,05135 | 17 |
| f3 | `floorKey + 1 >= mNumKeys` | 97,31081 | 36 |
| f4 | `!(floorKey < mNumKeys - 1)` | 98,05135 | 17 |
| f5 | `>= (int)(unsigned short)mNumKeys - 1` | 98,05135 | 17 |

Lo que hace el objetivo es usar **directamente** el pseudo del `lhz` (que ya
viene con los 16 bits altos a cero) sin volver a extenderlo: es `nonzero_bits`
de `combine`, no una forma de fuente. Descartada también la vía del accesor: el
volcado de `EvalSQT` (`Range: 0x800A0018`) **no lista ningún `GetNumKeys` en
línea**, o sea que el original usaba el miembro crudo `mNumKeys`, como nosotros.

---

## 8. `SetupNextLoad` (1008 B): por qué el filtro por `low_pc` no se puede aplicar

    // ERROR: Failed to process tag E5B398 (GlobalSubroutine)
    // Range: 0x800AB790 -> 0x800AB7DC

**El volcado NO tiene cuerpo DWARF para `SetupNextLoad`**: el volcador falló al
procesar su tag, y el siguiente rango que imprime es ya el de
`ResetBankLoadParams`. Por eso la r27 tuvo que ir con `lmap.py` y por eso el
filtro por `low_pc` devuelve vacío ahí.

**Hay 59 ocurrencias de `ERROR: Failed to process tag` en
`symbols/mw_dwarfdump.nothpp`.** Es un hueco de herramienta, no de la
decompilación: cualquier función que «no aparezca» en el volcado hay que
comprobar primero si es una de esas 59 antes de concluir nada.

Confirmado además el negativo de la r27 con banderas: `-fno-schedule-insns2`
**empeora** (98,671 → 97,393). Cero ensayos de fuente.

---

## 9. `Initialize`, `Play__14cSampleWarpper`, `MsgPlayMiscSound`, `startnextrequest`

* **`Initialize` (2.352 B)**: **no tocada.** La veda del grupo B de la r27 está
  cerrada con aritmética (`balance_case_nodes`: exige a la vez `i_izq = 4` y un
  conjunto izquierdo de 4 nodos con ≤2 rangos, y **6 ≠ 7**) y con 34 formas
  medidas sobre el caso mínimo. Los grupos A y C no valen bytes mientras B viva.
* **`Play__14cSampleWarpper` (2.152 B)**: he pasado el filtro por `low_pc`
  buscando el desajuste de TIPOS que cerró `DataLoadCB`. **No lo hay**: el
  volcado da `int TempVol` r26, `int TempPitch` r28, `int TempAz` r23,
  `float PitchScale` f13, `int RefCount` r0 — **los cinco con nuestro tipo
  exacto** — y el mismo árbol de inlines. Sigue en pie el diagnóstico de la r27
  (una constante 0 viva en registro salvado). **Cero ensayos.**
* **`MsgPlayMiscSound` (268 B)** y **`startnextrequest` (356 B)**: sólo el
  diagnóstico de bandera del §6. Sin ensayos de fuente.

---

## 10. Herramientas dejadas en el scratchpad (prefijo `c28ae_`)

* **`c28ae_dw.py <UNIT> <SYM> [N]`** — resuelve el rango con `lmap.py` y **vuelca
  el cuerpo DWARF del original filtrado por `low_pc`**, que es la operación que
  ha cerrado funciones en las tres últimas rondas. Avisa con `NO ENCONTRADO` si
  el volcado no lo trae (§8).
* `c28ae_up_lmap.txt`, `c28ae_sq_lmap.txt`, `c28ae_dlcb_lmap.txt` — mapas de
  línea de `UpdateParams`, `EvalSQT` y `DataLoadCB`.
* `c28ae_up_all.txt`, `c28ae_up_c2.txt`, `c28ae_up_c4.txt` — el diff completo de
  `UpdateParams` en las tres variantes.
* Variantes medidas: `c28ae_sq_f2..f5.cpp`, `c28ae_am_h1.cpp`.
* Copias de seguridad: `c28ae_EAXAemsManager.cpp.bak`,
  `c28ae_SFXCTL_MasterVol.cpp.bak`, `c28ae_RawStateChan.h.bak`,
  `c28ae_FnStatelessQ.cpp.bak`, `c28ae_am_base.cpp`, `c28ae_rsc_base.cpp`.

### Trampas que han vuelto a morder o son nuevas

* **`c27ae_d.py` y `c27ae_v.py` tienen la orientación CONTRARIA**, y además
  `c27ae_d.py` imprime **primero el tamaño del OBJETIVO** y luego el nuestro
  (`size=1480 (obj 1488)` con el objetivo en 1480). Verificado contra `lmap`:
  en `c27ae_d.py` la **izquierda es NUESTRO**; en `c27ae_v.py` la izquierda es
  el OBJETIVO.
* **`c27ae_v.py` falla con `SL=` si el `.cpp` variante vive en el scratchpad** y
  el fichero usa `#include` relativos (`FnStatelessF3.h: No such file`). Se
  arregla con `EXTRAPRE=-I<directorio del .cpp original>`.
* **`lmap.py` revienta con `TypeError: NoneType + int`** cuando el símbolo no
  existe (nombre truncado por `triage.py`, que corta a 42 caracteres). El nombre
  completo sale de `build/GOWE69/asm/…/<unidad>.s`.
* **`objdiff-cli` no encuentra `EvalSQT` por el nombre truncado**, con lo que
  `c27ae_d.py` casca en `B['match_percent']`.
* **Hay otro agente editando `SpeechManager.cpp`, que es de zEAXSound.** Toda
  medida de unidad de esta ronda está tomada revirtiendo mis dos cambios y
  volviendo a medir en el acto.

---

## 11. Qué NO he probado

* **`Initialize`**: nada. Grupos A y C sin valor mientras B viva; permutador no
  lanzado; no he probado a partir la función.
* **`Play__14cSampleWarpper`**: cero ensayos. Siguen sin probar las formas que
  sugería la r27 para forzar la constante 0 a vivir (`const int off = 0;`, pasar
  `eSTITCH_PLAY_STATUS_OFF` como último argumento). El fichero necesita
  `EXTRAPRE=-I…/EAXSound` para compilar con `c27ae_v.py` (falló sin él).
* **`FindMatchTime`**: permutador ciego no lanzado (sexta ronda que se queda ahí).
* **`UpdateParams`**: no he atacado la permutación de 47 registros FP. La pista
  que dejo: el objetivo pone `fMasterVol` en **f6** y nosotros en **f11** —
  cuarto y noveno del `REG_ALLOC_ORDER` de FP (`f0, f13, f12, f11, f10, f9, f8,
  f7, f6…`), o sea que el objetivo tenía **más pseudos vivos antes**; y nosotros
  **reusamos un registro** (`fmuls f11, f11, f13`) donde el objetivo estrena uno
  (`fmuls f6, f0, f12`), que es `combine_regs` de `local-alloc`.
* **`ResolveCurrentDataMemory`**: no he buscado una forma que suba la prioridad
  del `lwz` en `sched2` ni una que baje la del `lis` (habría que ver el
  `-fsched-verbose-5` de `sched2`, que no he volcado por espacio en disco).
* **`EvalSQT`**: no he mirado el `-dc`/`combine` para ver por qué
  `nonzero_bits(r5)` no vale 0xFFFF en nuestro lado.
* **`SetupNextLoad`**, **`UpdateRPM`**, **`Play__13cStichWrapper`**: sin ensayos
  de fuente (negativos previos).
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas.
* **El DOL**: no lo he construido (va en la verificación de la tanda).
* **`configure.py`, `config/GOWE69/*`, `splits.txt`**: intactos. Ningún
  ensamblador escrito ni retirado; lo retirado es la falsificacion de NOMBRE `__ct8`. Ningún `#if defined(__ANDROID__)`
  tocado. Ningún pin nuevo. **Ninguna cabecera compartida modificada** — las dos
  que toqué (`RawStateChan.h`) están revertidas y `git diff` lo confirma.

**Disco: 14 GB libres.** He borrado sólo mis `.json`/`.o` intermedios; los
directorios grandes del scratchpad (`c28lk_try` 903 MB, `c28bw_mn1` 45 MB) son de
otros agentes y no los he tocado.
