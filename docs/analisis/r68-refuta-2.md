# r68 — escéptico 2: la evidencia de QUÉ ERA el dato

Ataque al plan `r68-plan-datos.md` con la lente de la **identidad del dato en el original**: nombre,
tipo, tamaño y si estaba referenciado. Contrastado con el DWARF (`symbols/mw_dwarfdump.nothpp`),
la symtab de `orig/GOWE69/NFSMWRELEASE.ELF`, `symbols.txt`/`splits.txt`, el DOL original y
nuestros objetos de `build/`. Solo lectura: no se ha tocado `src/`, `config/` ni `build/`, y no se ha
enlazado nada. Las dos sondas de compilación van a un temporal borrado. HEAD 3c08534b.

Herramientas en `scratchpad/escep2_68/`:
- `ix.py`: índice DWARF + ELF + symbols + splits, y ventanas con bytes del DOL.
- `ff.py`: FFFFFFFF contra símbolos vivos, estáticos con dirección caducada y `$LC` del ELF.
- `between.py`: candidatos del DWARF entre los dos vecinos vivos de un hueco.
- `probe2.py`: sonda de estáticos locales.

El volcado `ix.pkl` está borrado: `python ix.py build` lo regenera en 15 s.

## 0. Veredicto

- **Lo que se retira en la r68 aguanta en identidad**, con dos excepciones:
  - **`fontShift`** (L8/L15) está escrito con una construcción que no es la del original;
  - **el paquete L6** rompe la propiedad de L5, porque la cabecera también toca zAnim.
- He revisado 33 huecos de los lotes L3, L6, L8, L9, L10, L11, L12, L14 y zMisc:
  - **25** tienen un único candidato en el DWARF o valores que casan tipo a tipo;
  - **4** son plausibles solo por adyacencia inmediata;
  - **4** son ambiguos, y en ellos el DOL no puede decidir porque valen cero.
- **Cae la contraprueba de K2** («la `.data` de zMain empieza en 0x8041D7EC, luego va a 4 y sus
  huecos no son colas»). Hay pruebas directas de que la `.data` original de zMain perdía bytes como
  una sección a 8. Con eso se reabren las filas R4 de zMain de lote2 y crece el alcance de la palanca
  «`.data` a 8».
- **K5 se confirma y se amplía.** Un estático local CON inicializador sale en `.data` incluso dentro
  de una inline de clase nunca llamada o de una función `static` sin uso. La única forma de que no
  salga en `.data` es no llevar inicializador.

## 1. Afirmaciones atacadas

| K | intento de refutación (qué comprobé) | resultado |
|---|---|---|
| K1 | Sin re-enlace (fuera de la lente). Leí en el DOL las colas de zEcstasy en las que se apoya la regla: `lbl_8041B49C` = 0x1ACC y 0x803DD658 + 0x1ACC = `"eVS_MAXNUM"`, la última cadena de `vShaderNames[5]`; `lbl_8041B468` = 0x1A6C → `"LM_DOUBLECHANNEL_NO_LIGHT_AND2VERT"`. | No cae. Lo que queda es el ÚLTIMO elemento: se quita por delante, a 8. |
| K2 (globales) | 2.475 DIE con `address: 0xFFFFFFFF`, 855 de nivel superior y 853 nombres. 163 de esos nombres están en `symbols.txt` (nombres puestos por el proyecto) y **0** en la symtab del ELF original. | No cae: FFFFFFFF ⇔ sin símbolo en el enlace original. |
| K2 (estáticas conservan la dirección) | `ff.py 2`: 436 de 2.320 DIE estáticas con dirección caen DENTRO de otro símbolo vivo. Casi todas son `_.tmp` de `.bss`. En `.data` hay dos: `static char bMemoryDebugStringNoName[8]` @0x80416460, que es la dirección de `bMemoryPersistentPoolNumber` (vivo), y `DecalsWeCareAbout` @0x804383F4, dentro de `CarMemoryInfoTable`. | **Cae la formulación** (§2 R-4): una estática estripada entera conserva una dirección CADUCADA. Ninguna estática de los lotes r68 está en ese caso. |
| K2 (orden de fuente) | `between.py` en 33 huecos: en **8** el vecino vivo de detrás va ANTES en el volcado (`bDefaultSeed` L256360, `pCurrentWorld` L2315382, `TheAvoidables` L359285, `ENABLE_ROLL_STOPS_THRESHOLD` L1653980, `m_SpeechModule` L2109774, `ScenerySectionLODOffset` L2196334, `found_memory_stomp_once` y `oldDistFogColour`, estáticos de función). | **Débil como método**: el orden del volcado es parcial. Los estáticos de función van dentro de su DIE, y una global declarada en una cabecera sale en su primera declaración. |
| K2 (evidencias) | NFS.MAP: `bStreamingPositionFromICE` 5dfd48, `NearClipInCar` 5dfd54, `JR2ServerExists` 5dfd58; `Demo1On`/`Demo2On` contiguos (5e0aa4/5e0aa8). zMisc: 0x8041E5CC-0x8041E5F4 (40 B) = 10 muertas de 4 B exactas. Aranges de zBWare: las únicas muertas de `.data` con `size&~7` > 0 son `bSinTable` (1024) y el `char[8]` (8). `kZero`/`_S_free_list` son miembros de clase sangrados. | No caen (aranges no recalculado). |
| **K2 contraprueba zMain** | Ver §2 R-1. | **REFUTADA** |
| K3 | Lente: un DOL igual valida BYTES, no NOMBRES. En un hueco a cero cualquier permutación de nombres muertos da el mismo DOL. | Débil como validación de identidad (§3 D-1). |
| K4 | Comprobé las líneas de keep.lst de L3/L6 que cita §4: 131, 135, 136, 146, 150-152 y 809-812 existen en HEAD con esas etiquetas. Los nombres destructurados son coherentes con sus vecinos vivos: `_6Attrib.`, `_13VehicleSystem.`, `_24QRCarSelectBustedManager.bIsCross` y `_21DebugWorldCameraMover.SlowSpeed` son miembros en el DIE de la clase. `_9TrackInfo.MaxTrackInfo` es `static int MaxTrackInfo` en la clase (volcado L90767 y TrackInfo.hpp:100). | No cae. La conducta del enlazador no la he atacado. |
| K5 | `probe2.py` con los cflags de zBWare reproduce P1 nombre a nombre y añade seis formas; ver la lista bajo la tabla. | **Confirmada y ampliada.** El negativo de lote3 solo se explica sin inicializador. |
| K6 | `fontShift` y `lastVerifyLevel` en el volcado; ver §2 R-2. | **Cae el ejemplo `fontShift.24940`** (§2 R-2). `once`×6 y `last_zone_hash` sí son locales (`@`). |
| K7 | ELF original: 1.711 símbolos de `.rodata`, 0 `$LC`, 113 locales en `.rodata`. | Cifra confirmada. **El mecanismo es débil** (§3 D-3); el efecto lo miden lote5 y lote3. |
| K8 | Nuestros `zDynamics.o`/`zMission.o` y los extraídos empiezan con `"GAMECUBE"` + ruta (52 B). En +52 va el `"bad_alloc"` de cc1plus (`$LC162` y `$LC60`, 10 B) y después la segunda mitad del prefijo. zSim y zBWare no lo tienen. | La partición se confirma. «El primer no puede llevar `bad_alloc`» es deducción sin medir (§3 D-4). |
| K9 | Re-ejecuté `sintesis68/probe.py`. `aligned(8)` en la primera: align 8, size 16. Sin atributo: 4 y 12. `double`: 8 y 16. Estática muerta `aligned(8)`: 8 y 16. Igual con zEcstasy. | Objeto confirmado; enlace sin medir, como dice el plan. |
| K10 | zFeOverlay.cpp:151. Detrás de `MarkerSelectInfos` (L2512561) no hay ninguna FFFFFFFF de nivel superior hasta el fin del CU. 0x8043948C = 0 y la cadena de crt0 empieza en 0x80439490, múltiplo de 8. | No cae: relleno. |
| K11, K13 | Fuera de la lente. | No atacadas. |
| K12 | Dato nuevo: nueve `.data` de `splits.txt` empiezan en 4 mod 8 (zEcstasy 8041A5A4, zMain 8041D7EC, zSim 80435814, zFeOverlay 804390E4, zFe 8041B5CC, zEAXSound 80417634, zMiscSmall 8041ED7C, zPhysicsBehaviors 8041F1A4, zEagl4Anim 804170F4). Las nueve llevan delante ≥16 B de ceros sin símbolo. | Refuerza que las fronteras de `.data` no son fiables para deducir la alineación. |

Las seis formas añadidas por `probe2.py` (K5):
- miembro fuera de clase sin inicializador → `.bss` (`seen_yellow_screen.3`);
- el mismo con `= 0` → `.data`;
- detrás de un `return` → `.data`;
- **inline de clase NUNCA llamada → `.data`**;
- función `static` sin uso → `.data`;
- `bool` → 4 B.

### Identidad por lote (bytes del DOL + DWARF + ELF)

| lote | comprobado | resultado |
|---|---|---|
| L1 | `zigzag` @0x80411358 = (0, 8, 1, 2, 9, 16, 24, …, 62, 55, 63), local en el ELF. | Casa con `{0, 8, 1, 2, …, 55, 63}`. |
| L2 | 0x803EB444 = 0x3B360B61 (1/360); 0x803EB450 = 0x40C90FDB (2π); 0x803EB73C/740/744 = −1e-4, 1e-4, 1000. | Casa. |
| L3 | 0x8041641C = {0, 0, 0, 1, 0x19000} (5 candidatas exactas, L256580-84); `lbl_80416434` = 1 (única); 0x80415944 8 B = `SendPacketFunction`/`ServiceMonitorFunction` (únicas); 0x80415954 y 0x80416444 = 0, por adyacencia con vecinos invertidos. `bSinTable[513]` @L256544, 0x804161E8 = 0000. | Casa. Dos por adyacencia. |
| L6 | `gDefaultExportID` @0x804158E4, `gCarTypeNameHash` @0x80439234 y `gInstallCarPartID` @0x8043923C (= 0xFFFFFFFF) con dirección exacta. 0x80439154 = 0 con 3 candidatas (§3 D-1). | Casa; `bIsCross` es ambiguo. |
| L8 | `lbl_8041ABE4` = 0x280, 0x280 (únicas `efbWcrt`/`xfbWcrt`); 0x8041AB70 = `CurrentStuffView` (única; `backupShaperRig` 136 B no deja cola); 0x8041B4B8 = 0 y A0A0A0FF (`MemoryUsedByDuplicatedSolids` + `TestMWDesaturation`, únicas); `once`×6 = 1, `@` en las tres funciones que da lote1. | Casa, salvo `fontShift` (§2 R-2). |
| L9 | `NearClipInCar` = 0.4f, `nWhichChaseCamera` = 2 y `SlowSpeed` = 0.05f, únicas. `Demo1On`/`Demo2On`: únicas y con NFS.MAP. 0x80416538 (24 B) = las 6 Tweak/test tras `Tweak_ForceICEReplay` (vecino de detrás invertido). | Casa. |
| L10 | 0x8041EFF0 = 120, 2.0, 1.25, 2500, 50, 10; 0x8041F00C = 8.0, 0, 0, 2.0, 0; 0x8041F024 = 0.15, 0.1, 0.1, 4000, 0.3, 1.0, 120, 1.25, 0.05, 160, 0.2, 0.1. El tipo int/float casa con el DWARF palabra a palabra. | Casa (fuerte: valores no nulos). |
| L11 | `OpToM` única; `testDamageLevel`/`testDelay` @0x80438F08/0C = 0 y 10, con dirección. `TrafficTeleporterDistanceAroundBubble` ambigua (§3 D-1). | Casa; una ambigua. |
| L12 | `MaxTrackInfo`, `DropTopSceneryLOD` e `InTunnel` únicas; `HorizFog*` = 0.62, 0, 1.0, 0.58, 229.0; `frame_max`/`frame_speed` = 120, 0.2; `DarkFalloff`/`DarkFallon`/`tunnelDark` = 0.06, 0.1, 70; `lbl_8043710C` = 1.8, 0.45, 0, 1, 2, 3 (§3 D-2); `RunPrecullingTool` por adyacencia. | Casa. |
| L14 | `lbl_803EBE90` = 1.0f, dentro de la `.rodata` de zGameModes (0x803EBB48-0x803EBE98). `SPEECHFLOW_DISPLAY_DITCHED` ambigua (§3 D-1). | Casa. |
| bool | ELF: `CameraDebugWatchCar` y `mIsRaceStart` tienen `st_size` 4 con DWARF `size: 0x1`. | Confirmado: 4 B. |

## 2. Refutadas

**R-1. La contraprueba de K2 (zMain a 4) es falsa.** Tres pruebas independientes:
1. **`static bool gVerbose`** (namespace Query, volcado L1422560) tiene dirección 0x8041DFA0 y ningún
   símbolo en el ELF: está muerta. Aun así sus 4 B existen: `kTypeHandlerCount` empieza en
   0x8041DFA4 y el DOL tiene 0 en 0x8041DFA0.
   - Con `.data` a 4 el estripado habría quitado los 4 B, y su dirección caducada caería sobre
     `kTypeHandlerCount`, como pasa con `bMemoryDebugStringNoName`. No cae.
2. **`lbl_8041E470` = {0x2E28, 0, 0}.** La `.rodata` de zMain empieza en 0x803EF580, y
   0x803EF580 + 0x2E28 = 0x803F23A8 es `"VIRTUAL"`, precedido de las constantes de VirtualMemory.
   - Es el addend sin reubicar de un puntero muerto (patrón R-f de lote6/lote5) que sobrevivió
     entero en `.data`. A 4 no sobreviviría.
3. **zEcstasy empieza en 0x8041A5A4 (4 mod 8) y tiene colas probadas a 8** (K1). Las nueve unidades a
   4 mod 8 llevan ceros anónimos delante. «Arranque no múltiplo de 8 ⇒ alineación 4» no es una
   inferencia válida.

Consecuencias:
- La fila zMain de lote2 §1.4 queda sin base.
- Las filas R4 de lote2 zMain.cpp:715, :734, :864 y :883 se apoyaban en «con `.data` a 4 no pueden
  ser colas».
  - :734 es `gVerbose`, que además se decía «GCC no la emite», refutado por K5.
  - :864 empieza por un `char *` muerto a `"VIRTUAL"`.
- zMain entra en la lista de la palanca «`.data` a 8» (§6 y §7.1 del plan), junto a zEcstasy y zAI.
- Ningún bloque de la r68 cambia: L7 solo toca el prefijo de zMain.

**R-2. `fontShift` no es un estático local numerado** (K6, detalle de L8, control de L15, §4 L8
línea 314).
- El volcado da `static int fontShift; // size: 0x4, address: 0x8041AFA0` en L749106, a NIVEL DE
  FICHERO. Es el mismo formato que `static int cBfR` (L749352) y que `static float
  FogCurrentBrightness` (L749124), cuyo nombre en el ELF no lleva número.
- Los locales van con `@`, incluso en funciones estripadas: `once` L720387 y siguientes,
  `last_zone_hash` L718072, `chattables` L947667.
- El árbol ya tiene la construcción: `dolphinsdk/src/demo/DEMOPuts.c:10` `static s32 fontShift;`,
  de fichero, usada por la inicialización de la fuente de ROM.
- lote1 midió otra cosa: `genV6.py:39` inserta `    static int fontShift = 0;` DENTRO de una
  función, con keep `fontShift.24940`.

Consecuencias:
- `fontShift` no necesita L15 (sin número).
- Su posición de emisión como estático de fichero está **sin medir**: EcstasyE.cpp:4972 hay que
  re-medirlo.
- El control de aceptación de L15 («las 9 entradas de keep_V6k») incluye una línea que en la forma
  original no existe.
- De paso, `static int lastVerifyLevel` (L749020) también es de fichero, no local como dice lote1.
  Es una fila R4, fuera de la r68.

**R-3. El riesgo de cabecera de L6 está incompleto y rompe la propiedad de L5.** Nuestros objetos
llevan `gCarTypeNameHash` (local, `.bss`) en:
- **`zAnim.o` (+488)**, que la incluye por `AnimChooseArrest.cpp:7` → `uiQRCarSelect.hpp` →
  `CarCustomize.hpp`;
- `zFe2.o` (+3440);
- `zFeOverlay.o` (+2856).

zAnim está promocionada y es la unidad de L5 en la MISMA tanda. Quitar la línea 712 de la cabecera
(que es lo correcto: el DWARF original solo tiene `gCarTypeNameHash` en zFeOverlay, L2512544)
cambia `zAnim.o`. Eso invalida la base de L5 y exige sellar el sha1 de zAnim y pasar el DOL. Hay que
secuenciar L5 y L6, o dar la cabecera y el sello de zAnim a un solo dueño.

**R-4. «Las estáticas conservan la dirección» no prueba ni supervivencia ni uso** (K2, y el corolario
de lote3 regla 4, «delata un usuario real»).
- Una estática estripada entera deja una dirección caducada: `bMemoryDebugStringNoName[8]` →
  `bMemoryPersistentPoolNumber`, y `DecalsWeCareAbout` → dentro de `CarMemoryInfoTable`.
- Y por K5/`probe2`, la estática se emite sin uso.
- La supervivencia se comprueba con «4 B (o `size % 8`) y sin colisión con un símbolo vivo». Las
  estáticas de L6/L11 lo cumplen.

## 3. Débiles (no caen, no están demostradas)

**D-1. Nombres de huecos a cero validados solo por bytes.** En estos, el test del DOL no distingue
nombres:
- **`bIsCross`** (0x80439154): entre sus vecinos hay 3 FFFFFFFF de 4 B:
  - `bIsCross` L2505432, contiguo a `bPlayerJustGotBusted` de su misma clase;
  - `RenderLookAtPoint` L2512026;
  - `CarLoadDelay` L2512156, `const float`.
- **`TrafficTeleporterDistanceAroundBubble`** (0x8043724C): 15 muertas de 4 B en la ventana
  invertida (`SuperEasyAIMode` … `PerformanceSimState`); gana por ser la línea siguiente a
  `UglyTimestepHack`. Las otras 14 no caben en la `.data` de zWorld tal como la corta splits: o van a
  `.bss` o son los 216 B de «cabecera» de K12.
- **`SPEECHFLOW_DISPLAY_DITCHED`** (0x8043597C): `DEBUG_SPEECHAI` L2116602 también es candidata.
  Probablemente es la dueña del hueco de 4 B en 0x80435974, que no está en ningún lote.
- **Solo por adyacencia**: `bPListAllocationNumber`, `bMemoryBreakOnFancyStompDetection`,
  `RunPrecullingTool` y las 6 de 0x80416538.
- **Medida que decide**: el orden por objeto en NFS.MAP de PS2 para cada nombre, o su uso en el
  árbol. El DOL no puede decidirlo.

**D-2. L12 `lbl_8043710C`.**
- Entre `distfall` y `ColourTintParamRed` el volcado pone `enum ScreenEffectType
  TestScreenEffectType` (4 B, FFFFFFFF) y `TestScreenEffectDef` (80 B).
- Leer {1.8, 0.45, 0, 1, 2, 3} exige que `TestScreenEffectType` no tuviera inicializador (`.bss`).
  Es plausible pero inferido.
- `TestScreenEffectDef` pudo estar en `.data` y estriparse entera: es inocuo para el DOL.

**D-3. K7, el mecanismo.** El ELF final conserva 113 locales en `.rodata` y ningún `$LC`. No se
distingue entre dos explicaciones:
- «el `$LC` no tenía tamaño y por eso no se estripó»;
- «el enlazador o el ensamblador de SN no escribía los `$LC` en la symtab».

La necesidad de `@lc` sí está medida (lote5 CAnimProperty, lote3 S1).

**D-4. K8, «el primer de zDynamics/zMission no puede llevar `"bad_alloc"`».**
- Si el primer lo referencia primero, GCC reutiliza una única etiqueta en el punto de la primera
  referencia, que sería +52 igual que hoy.
- Ni esa variante ni la de dos primeros alrededor del `"bad_alloc"` están medidas.
- Medida: `-S` de zDynamics con cada variante y ver el offset de `"bad_alloc"`.

**D-5. L3 `bSinTable[513] = {0}`.** Del original solo es verificable el elemento 512 (= 0). Un
inicializador todo a cero es contenido inventado. No mueve el DOL, pero no es «el C del original».

**D-6. Contabilidad de aranges de lote3** (zBWare 1.032): coherente con las muertas del volcado, pero
no la he recalculado.

**D-7. Sin atacar, fuera de la lente:** K1 en el enlace, K3/K4 como conducta del enlazador, K9 en el
enlace, K11 y K13.

## 4. Reproducir

```
python scratchpad/escep2_68/ix.py build
python scratchpad/escep2_68/ix.py win 0x8041DF90 0x8041DFB0 --bytes   # gVerbose (R-1)
python scratchpad/escep2_68/ix.py win 0x8041E460 0x8041E480 --bytes   # lbl_8041E470 (R-1)
python scratchpad/escep2_68/ff.py 1 | head ; python scratchpad/escep2_68/ff.py 3
python scratchpad/escep2_68/between.py 0x80439154 0x80439158 0x8043724C 0x80437250   # D-1
python scratchpad/escep2_68/probe2.py                                    # K5, temporal propio
```
