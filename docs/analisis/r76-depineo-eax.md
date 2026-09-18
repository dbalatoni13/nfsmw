# r76 — DESPINEO del lote zEAXSound + zEAXSound2

**Encargo**: quitar TODO pin de registro y TODA barrera `asm` de
`src/Speed/Indep/Src/EAXSound/`, escribiendo en su lugar la forma que dice el
DWARF del original, y aceptando la bajada de porcentaje antes que sostener un
100 % falso.

**Resultado: CERO pines y CERO barreras en EAXSound.** `censoasm.py` sobre el
árbol ya no imprime ni un `PIN` ni una `BARRERA` en ninguno de los ficheros del
directorio. Se retiraron **11 andamios** (5 pines + 6 barreras) y, de propina,
**1 instrucción PPC escrita a mano** (`__asm__("li %0,-1")` de `ginsudata.cpp`)
y **6 locales inventadas** que el oráculo dice que el original no tiene.

Sellos de los objetos con el árbol final (dos compilaciones seguidas, mismo
sello las dos veces):

    zEAXSound   94c282cbea056e3c3015e2e8c52d934cd8962c06
    zEAXSound2  d911986b8724273786fa6bc1683fbee06c1c7e63

---

## 1. Tabla por función

`%` y `filas` son de `fndiff.py` con `ppc.calculatePoolRelocations=false`.
`B` es el tamaño del objetivo; cuando el nuestro difiere se indica aparte.

| función | fichero | unidad | B | ANTES | AHORA | andamio retirado |
|---|---|---|---:|---|---|---|
| `SFXCTL_Physics::UpdateNIS` | `sfxctl/SFXCTL_NISReving.cpp` | zEAXSound | 1.592 | **100 %** (0 filas) | **99,96231 %** (3) | 1 barrera `"+r"(current_state)` + clobber r9 |
| `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | `sfxctl/SFXCTL_3DObjPos.cpp` | zEAXSound | 1.488 | **100 %** (0) | **95,67204 %** (34) · 1.432 B | 2 barreras clobber r9 |
| `cStichWrapper::Play(const SND_Params*)` | `STICH_PlayBack.cpp` | zEAXSound | 384 | **100 %** (0) | **99,68750 %** (4) | 1 barrera `"r"(this->ActiveSamplesRefs)` |
| `startnextrequest` | `realstream/src/stream.cpp` | zEAXSound | 356 | **100 %** (0) | **96,71910 %** (17) | 1 pin `nopendingrequest` r30 |
| `SFXCTL_AccelTrans::UpdateParams` | `sfxctl/SFXCTL_AccelTrans.cpp` | zEAXSound | 232 | **100 %** (0) | **99,48276 %** (6) | 1 pin `isAccelerating` r11 + su barrera |
| `SFXObj_Collision::InitSFX` | `CARSFX/SFXObj_Collision.cpp` | zEAXSound2 | 1.304 | **100 %** (0) | **99,96932 %** (2) | 1 pin `collisionReverbSlot` r10 |
| `CARSFX_RoadNoise::GenerateRoadNoise` | `CARSFX/CARSFX_Roadnoise.cpp` | zEAXSound2 | 1.240 | **96,80322 %** (20) | **96,10968 %** (55) | 1 pin `slipBoost` fr10 |
| `CARSFX_RoadNoise::Play` | `SND_GEN/ENVIRO_AEMS.h` | zEAXSound2 | 392 | **100 %** (0) | **99,64286 %** (7) | 1 barrera seca `__asm__("")` |
| `GinsuSynthData::BindToData` | `Ginsu/ginsudata.cpp` | zEAXSound2 | 340 | **100 %** (0) | **95,87059 %** (12) · 344 B | 1 pin `currentBlock` r0 + 1 `asm` INSTR |

Los dos objetivos de cabecera del encargo (`UpdateNIS` 1.592 B y `InitSFX`
1.304 B, los dos «100 % FALSO») quedan en **99,96 % con 3 filas** y **99,97 %
con 2 filas**: el código ahora es real y lo que queda es, en los dos casos,
**una sola permutación de registro**.

### Locales inventadas retiradas (el DWARF no las tiene)

`current_state` (UpdateNIS), `collisionReverbSlot` (InitSFX), `slipBoost`
(GenerateRoadNoise), `currentBlock` (BindToData), `isAccelerating` y
`oldIsAccelerating` (UpdateParams). En los cuatro casos en que se midieron las
dos formas, quitar la local da **exactamente las mismas filas** que dejarla sin
pin, así que se queda la del oráculo.

---

## 2. Tabla por fichero

| fichero | pin | bar | ahora | sonda X360 | .obj |
|---|---:|---:|---|---|---:|
| `sfxctl/SFXCTL_NISReving.cpp` | 0 | 1 | **0 / 0** | **OK** | 36.312 |
| `sfxctl/SFXCTL_3DObjPos.cpp` | 0 | 2 | **0 / 0** | GCCASM (INSTR, ver §4) | — |
| `sfxctl/SFXCTL_AccelTrans.cpp` | 1 | 1 | **0 / 0** | **OK** | 24.279 |
| `CARSFX/SFXObj_Collision.cpp` | 1 | 0 | **0 / 0** | **OK** | 36.328 |
| `CARSFX/CARSFX_Roadnoise.cpp` | 1 | 0 | **0 / 0** | **OK** | 51.418 |
| `Ginsu/ginsudata.cpp` | 1 | 0 | **0 / 0** | **OK** | 11.597 |
| `realstream/src/stream.cpp` | 1 | 0 | **0 / 0** | OTRO (ver §4) | — |
| `STICH_PlayBack.cpp` | 0 | 1 | **0 / 0** | OTRO (ver §4) | — |
| `SND_GEN/ENVIRO_AEMS.h` | 0 | 1 | **0 / 0** | (cabecera; su consumidor `CARSFX_Roadnoise.cpp` pasa a OK) |
| **TOTAL** | **5** | **6** | **0 / 0** | **6 de 9 desbloqueados** | |

De los ocho `.cpp`, **cinco pasan de GCCASM a OK** en la sonda del XDK. Los
otros tres ya no tienen bloqueo de `asm`: `stream.cpp` y `STICH_PlayBack.cpp`
caen a **OTRO** (errores C++ reales del árbol) y `SFXCTL_3DObjPos.cpp` sigue en
GCCASM por el frente **INSTR**, que es otro encargo (§4).

### Porcentaje de unidad (ponderado por bytes, `pctsnap.py`)

| unidad | antes | ahora | funciones bajo el 100 % |
|---|---|---|---|
| zEAXSound | 99,99775 % | **99,94699 %** | 6 (5 mías + 1 de la r75) |
| zEAXSound2 | 99,97084 % | **99,95639 %** | 6 (4 mías + 2 de la r75) |

Las dos unidades siguen NonMatching y el DOL enlaza el objeto extraído, así que
esta bajada **no toca el DOL**. Las tres funciones bajo el 100 % que no son
mías (`EAXAemsManager::SetupNextLoad` 99,66 %, `NFSMixMapState::CreateSubMixChannels`
99,40 %, `CARSFX_PreColWoosh::MsgBarrier` 94,29 %) son el coste ya asentado del
despineo de EAXSound de la **r75**; sus ficheros no se han tocado.

---

## 3. Lo que se midió, función por función

Cada función lleva su nota completa junto al código, con el DWARF, las formas
medidas y el sha1 de cada objeto. Resumen de los barridos:

**`GenerateSinglePlayerMix` — 6 formas, todas idénticas (1.432 B, 34 filas).**
Es la única de las nueve con un **hallazgo de mecanismo nuevo**, y no era
reparto: el clobber de r9 estaba **impidiendo el cross-jumping**. El objetivo
tiene cuatro colas del Newton-Raphson de `bSqrt` sin fundir porque en los casos
1 y 2 la raíz sale en `f0` y el `100.0f` en `f12`, y en los 3 y 4 al revés; no
son instrucciones idénticas, así que `jump.c` sólo puede fundir desde
`.L_800C3700` (`fmuls f0,f12,f0`, el mismo insn por conmutatividad). Sin el
clobber nuestras cuatro colas salen con los MISMOS registros, `cross_jump` las
funde y desaparecen 7 instrucciones en cada uno de los dos sitios: **56 B
cortos**. Las seis formas probadas (borrado a secas; variable propia para la 2.ª
distancia; llamada en línea sin variable; `100.0f * d`; `float d;` a nivel de
función; producto en sentencia propia) dan los seis un objeto distinto y **las
mismas 34 filas**: el cross-jump se decide DESPUÉS del reparto y ninguna forma
de fuente cambia la asignación FPR de las dos colas.

**`startnextrequest` — 4 formas, y una GANA 4 filas.** El mapa de líneas del
original (746/748/750 saltando a 751 o a 754/755) dicta
`if (!curreq || (state != PENDING && !next)) nop = 1; else { …; nop = 0; }`.
Esa estructura vale **21 → 17 filas** (96,494 → 96,719 %). El orden de
declaración del DWARF (`req`, `lockstate`, `nopendingrequest`) no cambia nada
pero se escribe igual, por ser el del oráculo. Es un caso de **grupo B**: la
variable existe y está en r30 de verdad; lo que el pin escondía era la prioridad
del allocno (`.greg`: 1.818 contra 2.000 de `req` y 2.307 del CR guardado).

**`BindToData` — 2 formas, las dos a 12 filas y 344 B.** El mapa de líneas pone
`this->mCurrentBlock = -1;` en la 172, DELANTE de la 174 (`minperiod`), y así
queda escrito. **Caduca el «+8 B» de la r67**: hoy las dos colocaciones dan
+4 B. El defecto es que sched1 adelanta el `li -1` unas veinte ranuras, el valor
vive en r8 todo el prólogo y obliga a un `mr r8,r5` de más dentro del bucle.

**`GenerateRoadNoise` — 2 formas sin pin, las dos a 55 filas.** Es la que más
paga en filas (20 → 55) aunque no cambia de tamaño. Se queda el **literal
`0.1f` en la sentencia**, sin variable, porque el DWARF lista `fRightVol` f29,
`fLeftVol` f31, `fRightPitch` f1, `fLeftPitch` f9, `speed` y `ftemp` f11 y
**`slipBoost` no está**. Las rondas r47–r61 dejaron medidas más de quince
colocaciones de esa variable y ninguna baja de 51 sin el pin.

**`UpdateNIS`, `InitSFX`, `UpdateParams`, las dos `Play`** — el borrado a secas
ya da el suelo (3, 2, 6, 4 y 7 filas) y las formas del DWARF miden lo mismo, así
que se escribió la del oráculo. Los diagnósticos RTL heredados de las r66/r67
(empate de `local_alloc` en las tres constantes de `InitSFX`, `REG_ALLOC_ORDER`
en `UpdateNIS` y `UpdateParams`, `REG_LIVE_LENGTH` y `REG_N_REFS` en las dos
`Play`) siguen siendo válidos y están copiados junto a cada función: en los
cinco casos el registro que el objetivo usa estaba ocupado por algo que su
código final no muestra.

---

## 4. PROPUESTAS (fuera de este encargo)

1. **`realstream/src/stream.cpp`: falta `<string.h>`.** La sonda X360 ya no dice
   GCCASM sino OTRO, y el único error es `C3861 'strcmp': identifier not found`
   en `:520`. Es un `#include` de una línea, pero **incluir una cabecera puede
   reordenar el `.bss` sin ninguna señal** (`docs/TRAMPAS.md`), así que no se ha
   tocado: hace falta medir el enlace, no sólo el objeto.
2. **`STICH_PlayBack.cpp:480`: error C++ real.** `C2998` sobre
   `UTL::FixedVector<SampleQueueItem, 43, 16> cSTICH_PlayBack::mQueuedSampleList[3];`
   («cannot be a template definition»). Es el último bloqueo X360 del fichero y
   es de la familia «error C++ del árbol», no de andamio.
3. **`sfxctl/SFXCTL_3DObjPos.cpp`: el clon `bSqrtNoCJ` (5 `asm` INSTR).** Son
   `frsqrte` / `fmuls` / `fnmsubs` escritos a mano, y son **las mismas cinco
   instrucciones que `bMath.hpp:72-87` ya trae**: el clon no añade ningún
   bloqueo nuevo, sólo repite el de la cabecera compartida. Desbloquear este
   fichero para X360 es el mismo trabajo que desbloquear `bMath.hpp`, y ése es
   el frente INSTR entero (118 apariciones). El clon **no sobra**: sin él, con
   el `bDistBetween` de `bMath.hpp` en los cuatro casos, la función cae a 98
   filas y 74,9 %.
4. **`CARSFX/CARSFX_Roadnoise.cpp`: `if (0) { ftemp = 0.15f; }`.** Es una
   **semilla de pool** (código muerto cuyo único efecto es colocar el `0,15f`
   delante del `0,1f` en la `.rodata`). Por el criterio de sustitutos del
   usuario —«sin primer»— es andamio y debería irse, pero **no es un pin ni una
   barrera** y quitarlo renumera los `$LC`, lo que obliga a tocar `keep.lst` y a
   pasar `lcfix.py`, las dos cosas prohibidas en este encargo. Queda propuesto.
5. **Lo que sigue en EAXSound y no es pin ni barrera**: `SFXObj_Pathfinder.cpp`
   (INSTR ×1), `EAXSND8Wrapper.cpp` (ALIAS ×3), `zEAXSound.cpp` (ALIAS ×1,
   DATOS ×6) y `zEAXSound2.cpp` (DATOS ×22). Son los frentes de alias manglados
   y de pools `asm(".section")`, distintos de éste.

---

## 5. Reproducir

```
python scripts/censoasm.py --json <salida>            # 0 PIN y 0 BARRERA en EAXSound
python scripts/build_direct.py zEAXSound              # compila las DOS unidades
SCRATCH=<dir> python scripts/fndiff.py zEAXSound2 <simbolo_mangled>
python scripts/lmap.py zEAXSound <simbolo>            # el mapa de lineas del original
python scripts/dwarf1.py fn <NombreFuncion>           # el oraculo de locales
python tools/scratch/x360_sonda/sonda.py <fichero>    # la clase de portabilidad
```

Banco de la r76 en `scratchpad/r76_eax/`: `ed.py` (edición exacta en bytes, que
respeta los finales de línea mezclados de `STICH_PlayBack.cpp`), `vp.py` (aplica
una variante, compila, mide y revierte), `side.py` (imprime un solo lado del
JSON de objdiff sin alinear, para leer el código nuestro cuando el alineador de
`fndiff` engaña) y los `a_*.json` con las 20 variantes medidas.
