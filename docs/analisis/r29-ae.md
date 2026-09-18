# Ronda 29 — zEAXSound + zEagl4Anim (+ zEAXSound2)

Encargo: `GenerateRoadNoise` primero (regla del temporal en coma flotante), y
pasar el filtro «`lis sym@ha` izado sobre un `bl` = falta una conversión» a
`Play__cSampleWarpper`, `FindMatchTime` y `SetupNextLoad`.

## 0. Línea base VERIFICADA

`build_direct.py zEagl4Anim zEAXSound` + `triage.py --muro`, medidos por mí:
coinciden con el encargo **instrucción a instrucción**, salvo un detalle:
**`GenerateRoadNoise` no está en zEAXSound sino en zEAXSound2** (1240 B,
**97,113 %**, no «muro» sin cifra). He trabajado también esa unidad.

    3 unidades  419060/434364 B  96,4767 %  2001 funciones al 100 %
    audit.py:  zEAXSound 766 ok / 0 FALLA · zEAXSound2 922 / 0 · zEagl4Anim 314 / 0

---

## 1. RESULTADO — 4 funciones cerradas, 4.032 B

| función | B | antes | después |
|---|---|---|---|
| `Play__14cSampleWarpperPC10SND_Params` | 2152 | 91,81599 | **100 %** |
| `UpdateParams__16SFXCTL_MasterVolf` | 896 | 98,16964 | **100 %** |
| `UpdateRPM__13SFXCTL_Enginef` | 716 | 99,80447 | **100 %** |
| `MsgPlayMiscSound__9EAXCommonRC10MMiscSound` | 268 | 96,76119 | **100 %** |

`measure.py --cmp` (base y medida seguidas):

    +4032 B, +4 funciones, 1 unidades cambian
       +4032 B  +4 fns  Speed/Indep/SourceLists/zEAXSound   145144 -> 149176

`pctsnap.py --cmp`: **EMPEORAN: ninguna.** (La quinta fila que mejora,
`__9Smackable…` de zPhysics, es de otro agente.)

`audit.py` al cerrar: **zEAXSound 769 ok / 0 FALLA**, **zEAXSound2 922 / 0**,
**zEagl4Anim 314 / 0**. `frozen.py`: zEAXSound **re-congelada**
(`8edc623912ce5c93`), zEagl4Anim idéntica. **zEAXSound2 sale «HA CAMBIADO» y NO
la he re-congelado**: comprobado que **no es mío** — sus secciones son idénticas
con y sin mis cabeceras (`.text=184008 .rodata=17776 .data=10040 .bss=3616` en
los dos casos) y `pctsnap` no mueve ninguna de sus funciones. Hay otro agente en
esa unidad (`BindToData`).

**Ficheros tocados, cuatro**, todos en `src/Speed/Indep/Src/EAXSound/`:
`SND_GEN/STITCH_AEMS.h`, `SND_GEN/MAIN_AEMS.h`, `sfxctl/SFXCTL_Engine.cpp`,
`sfxctl/SFXCTL_MasterVol.cpp`. Ningún ensamblador, ningún pin, sin commit.

---

## 2. EL HALLAZGO GRANDE: los *setters* generados a los que les falta el clamp

**`Play__14cSampleWarpper` (2.152 B) no fallaba por su fuente: fallaba por
`STITCH_AEMS.h`.** El objetivo abre **un registro salvado más** (`stmw r22` y
marco `-0x58` contra nuestro `stmw r23` / `-0x48`) porque mantiene **un 0 vivo
en r25** desde el constructor hasta `m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF`.

El mecanismo: **`expand_inline_function` sólo materializa el argumento en un
pseudo si el parámetro se MODIFICA dentro del inline.** Un *setter* con clamp
(`if (x < 0) x = 0; else if (x > N) x = N;`) modifica `x`; uno sin clamp
(`this->mData.f = x;`) no, y el argumento constante se sustituye directamente y
muere. Con el clamp, el `li rN, 0` nace como pseudo, `cse` lo reutiliza más
adelante y **el asignador tiene que darle un registro salvado**.

Cómo se prueba **sin adivinar**, con `lmap.py`: para cada *setter* en línea, la
distancia entre la **línea de apertura del inline** (donde nace la copia del
parámetro) y la **línea del store** es fija:

| forma del setter | Δ(apertura → store) | ejemplo en el objetivo |
|---|---|---|
| dos clamps | **7** | `SetAz` 183→190, `SetFilter_WetFX` 231→238 |
| un clamp | **6** | `SetID` 136→142, `SetOffset` 200→206 |
| sin clamp | 1-2 | — |

En `STITCH_AEMS.h` el objetivo da Δ=7 para `SetFilter_DryFX` (215→222),
`SetFilter_LoPass` (247→254) y `SetFilter_HiPass` (263→270) — **los tres los
teníamos de 3 líneas y sin clamp** — y Δ=6 para `SetID` y `SetOffset`, que
nosotros teníamos con **sólo el clamp superior**. Además el espaciado entre
*setters* consecutivos es **16 líneas constantes** en el original y 8-13 en el
nuestro: cuerpo de 9 líneas contra 3.

### Ensayos (medidos sobre `STICH_PlayBack.cpp` con `c27ae_v.py`, ~40 s cada uno)

| | clamps añadidos (en las TRES clases AEMS) | % | tamaño |
|---|---|---|---|
| base | — | 91,81599 | 2152 |
| p1 | `SetFilter_HiPass` | 95,11710 | 2152 |
| p2 | `+ SetFilter_LoPass` | 95,11710 | 2152 |
| p3 | `+ SetFilter_DryFX` | 97,92750 | 2140 |
| p4 | p3 `+ SetOffset` (clamp inferior) | 98,60595 | 2152 |
| p5 | p3 `+ SetID` (clamp inferior) | 99,27509 | 2140 |
| **p6** | **p3 + `SetOffset` + `SetID`** | **100,00000** | **2152** |
| p7 | p6 sin `SetFilter_LoPass` | 98,73978 | 2152 |

Los cinco hacen falta: **quitar cualquiera baja**. Los límites usados son
`[0, 0x7FFF]` para los tres `SetFilter_*` y `x < 0 → x = 0` para `SetID` y
`SetOffset`, que es exactamente el patrón de sus hermanos `SetVol`,
`SetFilter_WetFX` y `SetAz` en la misma clase.

### El mismo fallo en `MAIN_AEMS.h`, clase `FX_Radar` (268 B)

`MsgPlayMiscSound__9EAXCommon` construye `Csis::FX_Radar`. El volcado de líneas
da stores en `MAIN_AEMS.h` **2798, 2814, 2830, 2846, 2862** — separación 16 — y
`li r0,0` en 2791 y 2823, o sea **Δ=7 en los cinco**: los cinco *setters* llevan
clamp. Nosotros teníamos cuatro sin clamp (`SetID`, `SetPitch_Offset`,
`SetIntensity`, `SetStop`).

| | % | filas |
|---|---|---|
| base | 96,76119 | 8 |
| q1: clamps en los cuatro | 99,95522 | 3 |
| **q2: q1 + orden del constructor en orden de PARÁMETRO** | **100,00000** | **0** |

Las 3 filas que quedaban en q1 eran **el orden de tres stores**; el constructor
de `FX_Radar` llamaba `SetStop; SetPitch_Offset; SetIntensity` y el original los
llama en el orden de los parámetros. **Barridas las 6 permutaciones**:

    Pitch_Offset Intensity Stop   100,00000   <- orden de parámetro
    Pitch_Offset Stop Intensity    99,97015
    Intensity Pitch_Offset Stop    99,97015
    Stop Intensity Pitch_Offset    99,97015
    Stop Pitch_Offset Intensity    99,95522   (el que teníamos)
    Intensity Stop Pitch_Offset    99,95522

### ¿Es un FRENTE? — CONTADO, y la respuesta es «sólo en dos clases»

Aplicando la regla de «extrapolar frentes», he contado los *setters* sin clamp
en las siete cabeceras generadas de `SND_GEN`:

| cabecera | setters | sin clamp |
|---|---|---|
| `STITCH_AEMS.h` | 18 | **5 (ARREGLADOS)** |
| `MAIN_AEMS.h` | 94 | 10 → **4 de `FX_Radar` ARREGLADOS**, quedan 6 |
| `ENVIRO_AEMS.h` | 63 | 2 (`FX_WIND`) |
| `ENGINES_AEMS2.h` | 65 | 1 (`FX_Turbo…SetCar_id`) |
| `COP_SIREN_AEMS.h`, `FE_AEMS.h`, `TURBO.h` | 17/12/6 | **0** |

**Y los 9 que quedan NO hay que tocarlos**: sus seis clases (`FX_Weather`,
`FX_Camera`, `FX_Scrape`, `FX_WIND`, `FX_Radar`-ya-hecha, ENGINES) se construyen
desde `CARSFX_Rain.cpp:46`, `SFX_Common.cpp:47`, `SFXObj_Collision.cpp:127` y
`CARSFX_WindNoise.cpp:90`, y **esas funciones ya están al 100 %**: añadirles el
clamp las rompería. La forma sin clamp es la correcta ahí — **el generador no es
uniforme, el clamp va por campo**. Contraprueba medida: poner el constructor de
`FX_UVES` en orden de parámetro (`SetID` antes de `SetVolume`) baja
`MsgPlayMiscSound__10SFX_Common` de **99,405 a 97,262** — su orden barajado
actual es el bueno.

> **Regla para el resto del árbol.** En una cabecera de accesores generados,
> `lmap.py` sobre el objetivo mide la distancia *apertura del inline → store* de
> cada `Set*` en línea. **7 = dos clamps, 6 = un clamp.** Si nuestro cuerpo es
> `this->mData.f = x;` a secas y el objetivo da 6-7, falta el clamp — y lo que
> compra no es el clamp (se pliega con argumento constante) sino **el pseudo del
> parámetro**, que puede valer un registro salvado y el marco entero.

---

## 3. `UpdateRPM` (716 B) — la regla del temporal, y el temporal es una local YA EXISTENTE

Las 4 filas eran el acumulador: el objetivo suma en **f1** y nosotros en **f0**.
El DWARF de la función dice `Cur_RPM // f1`, `VisualRPM // f31`,
`NormalRPM // f13`, `PhysicsNewAudioRPM` sin registro — **`f1` es `Cur_RPM`**.
Y `lmap` da la suma repartida en **tres líneas** (572, 573, 576), no en una.

| | forma | % |
|---|---|---|
| base | `NormalRPM = VisualRPM + m_RPM_LFO + m_ComppressionRPM.GetValue() + m_RPM_LFO;` | 99,80447 |
| u1 | tres sentencias acumulando en `NormalRPM` | 99,80447 |
| u2 | `(a+b+c) + d` con paréntesis | 99,80447 |
| u4 | `m_RPM_LFO + VisualRPM + …` (orden de operandos) | 99,77653 |
| u6 | dos sentencias acumulando en `Cur_RPM` | 99,83240 |
| **u8** | **tres sentencias acumulando en `Cur_RPM`** | **100,00000** |

```c
Cur_RPM = VisualRPM + this->m_RPM_LFO;
Cur_RPM = Cur_RPM + this->m_ComppressionRPM.GetValue();
NormalRPM = Cur_RPM + this->m_RPM_LFO;
```

> **La §3.1 del brief, afilada:** cuando el objetivo acumula en un registro que
> no es el del destino, **el DWARF dice QUÉ LOCAL es ese registro**. Aquí el
> temporal no era una variable inventada: era `Cur_RPM`, ya muerta. Es la
> primera vez que la regla se cierra en coma flotante.

## 4. `UpdateParams` (896 B) — 47 filas de permutación FP, cerradas con dos cambios

Las dos piezas, las dos leídas del volcado (`fvol` f0, `fMasterVol` f6):

1. **`lfs f6, 0x0(r11)` seguido de `fmuls f0, f6, f31`** (objetivo) contra
   nuestro `lfs f0` + `fmuls f0, f0, f31`: el original **guarda `MasterVol` en
   `fMasterVol`** antes de escalarlo. `lmap` lo confirma: el `lfs` está en la
   línea **66** y el `fmuls` en la **69** — dos sentencias.
2. **Cada uno de los seis bloques son DOS sentencias** con `fvol` de
   intermedio, no una expresión: `lmap` da 75/76, 83/84, 87/88, 91/92 — pares
   consecutivos, cuatro líneas por bloque.

| | cambio | % | filas |
|---|---|---|---|
| base | (estado r28, sin `__ct8`) | 98,16964 | 49 |
| v1 | sólo (1) | 98,45982 | 42 |
| v2 | (1) + `fvol = (float)nmastervol; fMasterVol = fvol * k;` | 97,99107 | 44 (900 B) |
| **v3** | **(1) + (2) en los seis bloques** | **100,00000** | **0** |

```c
fMasterVol = g_pEAXSound->GetCurAudioSettings()->MasterVol;
nvolindex  = static_cast<int>(fMasterVol * 32767.0f);
nmastervol = NFSMixShape::GetCurveOutput(static_cast<eMIXTABLEID>(1), nvolindex, false);
fMasterVol = static_cast<float>(nmastervol) * 3.051851e-05f;

fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredFEMusicVol() * fMasterVol;
nvol = static_cast<int>(fvol * 32767.0f);
this->SetDMIX_Input(0, nvol);
…  (los seis iguales)
```

**El `static_cast<int>(RadarDetector::mStaticRange * 32767.0f)` de la r28 se
queda tal cual** — no lo he tocado y sigue siendo la forma sin falsificación.

## 5. `GenerateRoadNoise` (1.240 B) — NO cerrada, pero deja de ser un misterio

**La lectura de mi predecesor era falsa**: no es la regla del temporal. Las dos
`fmadds` sobre `f29` son `fRightVol = fLeftVol + fLeftVol*ftemp` (A) y
`fRightVol = fRightVol + fRightVol*0.1f` (C), y **la C está en el bloque básico
equivocado**. Tres pruebas independientes fijan la fuente del original:

1. **El volcado DWARF lista SEIS locales** (`fRightVol` f29, `fLeftVol` f31,
   `fRightPitch` f1, `fLeftPitch` f9, `speed`, `ftemp` f11) y **ninguna en f10**:
   el `float slipBoost = 0.1f;` que hay hoy en el árbol **no existe en el
   original**, y el 0.1f de C es un temporal del compilador (literal en fuente).
2. **`lmap`**: A está en la línea 558, C en la **569** y el segundo
   `bLength(…)*0.01f` en la **574** → **C va ANTES del segundo `bLength`**.
3. **El tamaño lo confirma**: con C detrás del `bLength`, `gcse` comparte el
   0.1f con el `ftemp * 0.1f` posterior y salen **1232 B** (el objetivo son
   1240). Con C delante, 1240.

Y el orden del *pool* de literales lo remata: el objetivo emite **0,15 f antes
de 0,1 f** (`lbl_803D9A5C` < `lbl_803D9A60`), o sea que el 0.1f se materializa
**después** del primer clamp.

### El mecanismo exacto, medido con `alloc.py` sobre el `.greg`

| variante | pseudo 92 (`ftemp`) | 0.1f | % | filas |
|---|---|---|---|---|
| base (`slipBoost` entre A y el `bLength`) | **r43 = f11** ✔ | global, f7 | 97,11290 | 23 |
| c2 (sin `slipBoost`, C antes del `bLength`) | r42 = f10 ✘ | **local → f11** | 96,10968 | 55 |
| c9 (`slipBoost` declarado antes del `if`) | **r43 = f11** ✔ | global, f8 | **97,70968** | 16 |
| objetivo | f11 | **f10** | — | 0 |

`alloc.py` lo da al dígito: en c2 el pseudo del 0.1f **muere en el bloque 8**,
así que **`local_alloc` lo coge ANTES que `global_alloc`** y se lleva f11
(primero libre tras f0/f13/f12), dejando a `ftemp` en f10 aunque `ftemp` sea el
**#0** de la lista de prioridad (`n_refs 34, live_len 63, pri 26984`). En c9 el
0.1f cruza bloque, va a `global_alloc` con `pri 588` (n_refs 2, live_len 34) y
cae en f8. **Para reproducir el objetivo el pseudo tiene que ser global Y tener
prioridad de banda f10 (~1250-1400, o sea `live_len` 14-16)**, y no hay posición
de fuente que dé las dos cosas a la vez sin romper el orden del *pool*.

**c9 NO se adopta pese a ser la mejor cifra**: al declarar el 0.1f antes del
primer `if` **invierte el orden del pool** (emite 0,1 antes de 0,15), o sea
cambia `.rodata` — justo el frente del §2 del brief. **El árbol se queda como
estaba.** Ensayos: **c1..c15 (13 formas medidas)**, todas revertidas.

| | forma barrida | % |
|---|---|---|
| base | `A; float slipBoost=0.1f; bLength; C; *0.01f` | **97,11290** |
| c1/c2/c5/c6/c8/c15 | C antes del `bLength` (con y sin variable, `+=`, operandos al revés, `const`) | 96,10968 |
| c3 | sin `slipBoost`, C detrás del `bLength` | 96,60968 (1232 B) |
| c4 | `fRightVol = (fLeftVol + fLeftVol*ftemp) * 1.1f` | 96,80645 |
| c9 | `slipBoost` antes del primer `if` | 97,70968 **(rompe el pool)** |
| c12/c13 | `slipBoost` declarado tras el `bLength` (lo que dicen las líneas 562/564) | 96,60968 (1232 B) |
| c24 | `__asm__("" : : "f"(slipBoost))` tras el `bLength` | 96,27419 (1244 B) |

**Veda**: barridas las cinco sentencias del bloque
`fRightVol = fLeftVol + fLeftVol*ftemp` … `ftemp = ftemp*0.01f` de
`CARSFX_Roadnoise.cpp:383-387`, y la declaración del 0.1f en **cinco**
posiciones distintas. Lo que queda es un empate de `local_alloc`, no fuente.

## 6. El filtro del encargo (`lis sym@ha` sobre un `bl`): NEGATIVO en las tres

* **`Play__cSampleWarpper`**: los `lis` que hay sobre `bl` son
  `lis g_pEAXSound@ha` (el `this` de la propia llamada) y
  `lis _4Csis.gAEMS_Stich*Handle@ha` (**argumento**, no destino de la
  asignación). No hay ninguna asignación `sym = f()`. La función cerró por otra
  vía (§2).
* **`SetupNextLoad`**: el `lis g_SndAssetList@ha` + `addi` sobre
  `bl InitiateLoad` **ya casa** (filas 13-15 sin diff). No aplica.
* **`FindMatchTime`**: no hay `bl` en el rango del diff; el par es
  `lis $LC326@ha` + `lfd` (la constante `0x4330…` de la conversión), y va
  **12 ranuras antes** en el objetivo. No es una conversión que falte.

## 7. Lo que queda, con diagnóstico

| B | % | función | qué es exactamente |
|---|---|---|---|
| 2352 | 98,876 | `Initialize…DynamicLoader` | veda del grupo B intacta (aritmética de `balance_case_nodes`) |
| 1480 | 99,459 | `EvalSQT` | veda de las 5 formas del `>=`; es `nonzero_bits` de `combine` |
| 1240 | 97,113 | `GenerateRoadNoise` | §5 |
| 1008 | 98,671 | `SetupNextLoad` | **permutación pura de r25-r31**: `this`→r27 (objetivo r29) y el contador de `DeleteRefToAsset`→r31 (objetivo r26). Mismo `stmw r25` y mismo marco. Es orden de `global_alloc`, sin diferencia de estructura |
| 780 | 99,405 | `MsgPlayMiscSound__10SFX_Common` | `FX_UVES`: el objetivo tiene **dos** pseudos de 0 (r27 para 0x8, r0 para 0xc/0x10/0x14/0x18) y nosotros al revés. Su cabecera **ya tiene todos los clamps** y su orden de constructor es el bueno (medido) |
| 720 | 97,361 | `FindMatchTime` | negativo del volcado confirmado en la r28; permutador ciego sin lanzar |
| 672 | 97,500 | `ProcessUpdate__CARSFX_Turbo` | el objetivo adelanta `mr. r7, r3` (`TURBO.h:100`) antes del `mullw`, y así `srawi` reusa r3 |
| 456 | 93,509 | `EvalState` | negativo del volcado; fallo de CSE |
| 392 | 94,939 | `Play__CARSFX_RoadNoise` | `FX_ROADNOISE` de `ENVIRO_AEMS.h` **ya tiene todos los clamps** (comprobado); son dos pseudos de 0 contra uno |
| 384 | 99,688 | `Play__cStichWrapper` | empate r30/r31 entre la base `&ActiveSamplesRefs[0]` y el índice `i*4` |
| 356 | 94,303 | `startnextrequest` | sin tocar |
| 168 | 90,238 | `ResolveCurrentDataMemory` | mecanismo cerrado en la r28 (`sched2`); sigue sin palanca de fuente |
| 140 | 94,286 | `MsgBarrier__CARSFX_PreColWoosh` | sin tocar |

### Ensayo negativo con veda nueva

* **`ProcessUpdate__12CARSFX_Turbo`, t1**: mover
  `this->m_pTurboBlowoffControl->SetAzimuth(Az);` **delante** de la sentencia
  `TmpBlowoffVol` (que es lo que sugiere el mapa de líneas, TURBO.h:99/100 entre
  `CARSFX_Turbo.cpp:263` y `:265`): **79,970 %, 49 filas** contra 97,500 de base.
  **Revertido.** Sentencia barrida: el orden de `SetAzimuth`/`TmpBlowoffVol` del
  primer bloque de `ProcessUpdate`.
* **`FX_UVES` en orden de parámetro** (`SetID` antes de `SetVolume` en el
  constructor): `MsgPlayMiscSound__10SFX_Common` **99,405 → 97,262**.
  **Revertido.** Sentencia barrida: el orden del constructor de `FX_UVES`.

---

## 8. Herramientas dejadas en el scratchpad (prefijo `c29ae_`)

* **`c29ae_hdr.py` / `c29ae_hdr2.py` / `c29ae_hdr3.py`** — inyectan clamps en
  los `SetFilter_*`/`Set*` de `STITCH_AEMS.h` y `MAIN_AEMS.h`.
* **`c29ae_run.sh` / `c29ae_run2.sh` / `c29ae_perm.sh`** — parchean la cabecera,
  miden y **revierten**; `c29ae_perm.sh` barre permutaciones del constructor.
* **`c29ae_secs.py <obj…>`** — tamaños de sección de un ELF sin herramientas
  externas (es lo que probó que zEAXSound2 no se movía).
* `c29ae_mk.py`, `c29ae_mk2.py` (variantes de `CARSFX_Roadnoise.cpp`),
  `c29ae_mkeng.py` (**respeta los finales de línea MEZCLADOS de
  `SFXCTL_Engine.cpp` byte a byte**), `c29ae_mkmv.py`/`c29ae_mkmv2.py`.
* Mapas de línea: `c29ae_grn_lmap.txt`, `c29ae_psw_lmap.txt`,
  `c29ae_snl_lmap.txt`, `c29ae_urpm_lmap.txt`, `c29ae_turbo_lmap.txt`,
  `c29ae_mpm2_lmap.txt`.
* Copias de seguridad: `c29ae_STITCH_AEMS.h.bak`, `c29ae_MAIN_AEMS.h.bak`,
  `c29ae_SFXCTL_Engine.cpp.bak`, `c29ae_SFXCTL_MasterVol.cpp.bak`,
  `c29ae_RoadNoise.cpp.bak`.

### Trampas nuevas o que han vuelto a morder

* **Sombrear una cabecera con `-I` NO funciona aquí**: `STICH_Playback.h` hace
  `#include "SND_GEN/STITCH_AEMS.h"` **relativo**, que se resuelve contra el
  directorio del fichero incluyente antes de mirar los `-I`. Perdí una medida
  creyendo que el clamp «no hacía nada». **Para probar una cabecera hay que
  parchearla en el árbol y revertir** (y `c27ae_v.py` la recoge sin
  `build_direct.py`, en 40 s).
* **`c27ae_v.py` necesita `EXTRAPRE=-I…/src/Speed/Indep/Src/EAXSound`** para
  `STICH_PlayBack.cpp` y `EAXFrontEnd.cpp` (includes relativos).
* **`c27ae_d.py` y `c27ae_v.py` dan porcentajes DISTINTOS de la misma función**
  (97,532 contra 97,113 en `GenerateRoadNoise`): `c27ae_d.py` informa del lado
  del objeto del troceador. **El que coincide con `triage.py` es `c27ae_v.py`.**
* **`triage.py` no muestra el símbolo completo**: `GenerateRoadNoise` se mangla
  `…__16CARSFX_RoadNoise` **sin `Fv`**; con `Fv` `objdiff-cli` no lo encuentra y
  `c27ae_d.py` casca con `TypeError`.
* **El disco bajó a 14 GB.** He borrado mis JSON y volcados RTL intermedios; el
  scratchpad compartido queda en 1,1 GB.

## 9. Qué NO he probado

* **`Initialize` (2.352 B)**: nada. La veda del grupo B sigue en pie.
* **`EvalSQT` (1.480 B)**: nada; no he mirado el `-dc`/`combine`.
* **`SetupNextLoad` (1.008 B)**: **cero ensayos de fuente.** No he sacado su
  tabla de allocnos (`alloc.py` sobre `EAXAemsManager.cpp`), que es lo que dice
  cuánto de cerca están `this` y el contador; ni he probado a mover el
  `int deleteCount` de `EAXAemsManager.h:126`.
* **`MsgPlayMiscSound__10SFX_Common` (780 B)**: sólo el negativo del orden del
  constructor. No he probado a partir el `new Csis::FX_UVES(0,0,0,0,0,0)` ni a
  tocar `SFX_Common.cpp`.
* **`FindMatchTime`**: permutador ciego **no lanzado** (séptima ronda).
* **`ProcessUpdate__CARSFX_Turbo`**: sólo t1. No he probado el segundo bloque
  (`m_pTurboSplControl`) ni el eje del `asm` selectivo.
* **`Play__cStichWrapper`, `startnextrequest`, `MsgBarrier`,
  `CreateMixCtls`, `CreateSubMixChannels`, `Play__CARSFX_RoadNoise`,
  `BindToData`**: cero ensayos.
* **El `asm` selectivo (`__asm__("" : "+r"(x))`)**: sólo lo he usado **una vez**
  y como diagnóstico (c24 en `GenerateRoadNoise`, peor). **El eje sigue casi sin
  barrer** en estas unidades.
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas. Sí he
  comparado `.text/.rodata/.data/.bss` de zEAXSound2 con y sin mis cabeceras
  (idénticas).
* **El DOL**: no lo he construido (va en la verificación de la tanda).
* **`configure.py`, `config/GOWE69/*`, `splits.txt`**: intactos. Ningún
  ensamblador escrito. Ningún `#if defined(__ANDROID__)` tocado. Ningún pin.
