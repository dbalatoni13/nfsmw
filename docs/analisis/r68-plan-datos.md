# r68: plan de limpieza de los parches de datos

Síntesis de los seis informes de exploración (`r68-explora-lote1.md` … `lote6.md`) sobre los
502 bloques `asm()` de fichero que emiten datos. HEAD 3c08534b, DOL de referencia 9619ba57.
Documento de solo lectura: no se ha tocado `src/`, `config/`, `configure.py` ni `build/`.

La síntesis añade dos cosas a los informes:
- **dos sondas de compilación nuevas** (`scratchpad/sintesis68/probe.py`, a un temporal borrado),
  que zanjan dos contradicciones entre informes (§2, K5 y K9);
- **un recuento por lote contra la lista canónica** (`scratchpad/sintesis68/lotes.py` sobre
  `jefe/bloques_datos68.txt`). El script falla si una clave no existe o si un bloque sale en dos
  lotes.

## 0. Resumen

| | bloques | bytes |
|---|---:|---:|
| Frente total | 502 | 54.494 |
| **Limpiables en la r68** (lotes L1-L14) | **131** | **3.765** |
| + con el resolvedor de estáticos numerados (L15) | +6 | +68 |
| Palanca `.data` a 8 en pendientes (r69, depende de la sonda de L5) | 21 | 180 |
| Compensadores de cola (fuera de la r68) | 16 | 10.253 |
| No son datos (código, macros sin uso, alias de codegen) | 6 | 16 |
| Resto: R2 sin medir y R4 (r69 y siguientes) | ~322 | ~40.200 |

- **Qué se retira.** De los 131 bloques, **86 tienen el test ya medido** por un explorador (relink
  9619ba57, secciones ALLOC idénticas o hash del enlace igual a la base). Los otros 45 son
  extensión directa de una receta medida o la sonda E1. Todos siguen en la tabla de §3 con el test
  a pasar.
- **Por qué pocos bytes.** El 19 % de los bytes del frente son 16 bloques de cola que compensan
  tamaño. El resto de lo grande son tramos mixtos (prefijo + pools + etiquetas Attrib) que no
  salen de un golpe.
- **Organización.** Dos tandas por la cadencia del proyecto (tanda, DOL, tanda):
  - tanda 1: 7 agentes, sólo unidades **promocionadas**; el test decisivo es el DOL;
  - tanda 2: 7 agentes, **pendientes**; el test es dolwhere o el hash del enlace contra la base;
  - una herramienta (L15) se hace en paralelo a la tanda 1.

## 1. Armonización de los veredictos

Los seis informes no coinciden en algunos puntos. Así queda cada uno, y por qué:

| punto | informes | queda | por qué |
|---|---|---|---|
| Regla de estripado en `.data` | lote4: «entero, mida lo que mida»; lotes 2/3/5: `size & ~(align-1)` | **`size & ~(sh_addralign-1)`** (K1) | Las medidas de lote4 (4, 12 y 20 B) son múltiplos de 4 con `.data` a 4: es el mismo caso. |
| Prefijo bWare/STL de 92 B | lotes 2/5/6: R3/R4; lotes 3/4: R2 | **R2** (K8) | Medido en zSim S2: `.rodata` del objeto idéntica byte a byte y relink 9619ba57. |
| Estático sin uso | lote3 (y lote2 en zMain.cpp:734): «GCC no lo emite»; lotes 1/4/6: «lo emite» | **lo emite** (K5) | Sonda P1 con los cflags de zBWare y zEcstasy. El negativo ×3 de lote3 queda sin explicar. |
| `clamp.c:3` (gcc2_compiled) | lote3: R3 de herramienta | **R1** | Es el patrón de DeInterlace/abortmsg: quitarlo, medido por lote5, deja DOL 9619ba57. |
| `EcstasyE.cpp:76` (`.set e_OSBusClock`) | lote1: R3 | **R2 de config** | Mismo mecanismo que `GXVert.h:34` (GXWGFifo, lote6): símbolo absoluto en el ldscript. Va a la r69. |
| Nombres Attrib de cabecera (zFoundation.cpp:129, zDebug.cpp:14) | lote6: R3 | **R2 sin medir** | La receta primer + `@lc` está medida en zSim S1 y LuaBindery. Van mezclados con otros tramos: r69. |
| Inicios de `.data` en `.debug_aranges` | lote3: son el inicio real, y las colas son la cabecera de la unidad siguiente; lote4: salen 32 B corridos, no sirven | **en disputa** (K12) | Ningún lote de la r68 depende de ello. Los 6 bloques «G» de lote3 quedan fuera. |
| zMisc.cpp:3699 «sin keep.lst» | lote2 | **contradicción interna** | La alineación 8 de zMisc la da el `.balign 8` del propio bloque: si se pasa a C, se va. Necesita la palanca de K9. r69. |

## 2. Afirmaciones clave

Numeradas para el ataque de los escépticos. Cada una lleva su evidencia y, si la hay, su contraprueba.

**K1. El enlazador quita `size & ~(sh_addralign-1)` de cada dato muerto.** Cuenta la alineación de
la sección DE ENTRADA. Quita por delante, deja la cola y se lleva el símbolo.
- Sonda de lote2 (t2/t3/t4, ngcld real). Con `.data` a 4, `float`, `float[3]`, `float[5]` e
  `int[4]` desaparecen enteros. Con un `double` vivo (sección a 8), `float` queda entero,
  `float[5]` deja 4 B, `float[3]` y un struct de 60 B dejan 4 B, y `float[2]` desaparece.
- Lote5: parchear sólo el `sh_addralign` de la `.data` de zAnim.o de 4 a 8 hace aparecer +8 B
  exactos.
- Lote3 T1a: un `.balign 8` al principio de la `.data` de zBWare conserva sin keep los muertos de
  4 B, y el relink da 9619ba57.
- Lote4: en `.rodata` (a 8), `FEKeyInterpDefault[7]` (28 B) deja su cola de 4 B y el DOL sale OK.
- Contraprueba: lote4 midió 4, 12 y 20 B en `.data` a 4 con el mismo DOL roto. Encaja con la regla.

**K2. Los huecos con nombre de `.data` son las variables muertas del original, en orden de fuente.**
Las globales llevan `address: 0xFFFFFFFF` en `mw_dwarfdump.nothpp`. Las estáticas conservan la
dirección y no tienen símbolo en el ELF. Los valores salen del DOL.
- Lote1 V5: 31 bloques de zEcstasy a la vez; con keep, el enlace queda en 2.468 B, como la base.
- Lote3: la contabilidad de aranges cuadra al byte en zBWare (3928 − 2896 = 1032 = bSinTable 1024
  + 8).
- Lote6: M3/M5 en zCamera, y el orden coincide con NFS.MAP y PS2_globals.hpp.
- Lote4: 17 huecos validados con `corpse.py`.
- Lote2: zMisc, 40 B exactos.
- Contraprueba: la `.data` de zMain empieza en 0x8041D7EC, que no es múltiplo de 8. Sus huecos no
  pueden ser colas de alineación 8.

**K3. Sin keep, un dato muerto con nombre de 4 B en `.data` a 4 desaparece entero en nuestro
enlace.** Con una entrada de keep.lst por su nombre real se conserva entero y en su sitio, y el
DOL no se mueve.
- Lote1 V3/V4/V5: sin keep, 99.600 B distintos; con keep, 2.468 = base.
- Lote3 V1/V2: sin keep, ROTO por 66 B; con keep, 9619ba57. T1b: las 10 entradas, 9619ba57.
- Lote6 M4/M5: sin keep, secciones −0x20; con keep, 15.557 = 15.557 B.
- Lote4: zAttribSys pasa de 3.104 B distintos a OK; zWorld, de 92.766 B a igual.

**K4. keep.lst casa por NOMBRE DE FICHERO del objeto, y una entrada inexistente no da error.**
- Lote2 t4b: un nombre inexistente deja el enlace idéntico.
- Lote3 S2: renombrar el `.o` hizo perder −416 B de `.rodata`.
- Lote5, nota de `relink.py`.
- Consecuencia: el objeto de prueba ha de llamarse igual que el real, y una errata en keep.lst sólo
  la delata el DOL.

**K5. Este GCC 2.95 emite todo estático sin uso, de fichero o de función, incluso bajo `if (0)`.**
Si lleva inicializador va a `.data`; si no, a `.bss`. El de función se llama `<nombre>.<N>`, con
N el contador del compilador.
- Sonda P1 de esta síntesis, con los cflags de zBWare y de zEcstasy y el mismo resultado:
  - van a `.data`: `a1.3` (`static int a1 = 5`), `a2.7` (`= 0`), `d1.15` (bajo `if (0)`),
    `e1.19`, `e2.23` y el de fichero `c1`;
  - van a `.bss`: `a3.11` (sin inicializador) y el de fichero `c2`.
- Coincide con el control de lote1 (`once.3`, `cBfR`) y la regla 4 de lote4.
- **Refuta la regla 4 de lote3 tal como está escrita.** Sus T1c/T1d/T1e (`seen_yellow_screen`)
  siguen sin explicar. Hipótesis: se escribieron sin inicializador, fueron a `.bss` y la `.data`
  quedó 4 B corta.
- Medida que lo decide: repetir T1c con `= 0`.
- Corolario, también de P1: un `static T x;` en una cabecera se emite en la `.bss` de cada TU que
  la incluye (afecta al lote L6).

**K6. Los nombres de los estáticos de función cambian con cualquier edición de la TU.** Hoy nada los
resuelve.
- Lote1 V6k: `once.24919`, `fontShift.24940`.
- Lote4, regla 5: `index.25651` y `fc.23738` frente a los `index.14943` y `windState.14947` que
  lleva el árbol.
- `scripts/lcfix.py` sólo entiende `# @lc` y `// @lcsrc` (grep de sus líneas 73 y 126).
- Consecuencia: 6 bloques (L15) esperan una herramienta.

**K7. Un literal muerto de nuestra TU necesita `# @lc`.**
- Nuestro cc1plus da `.type`/`.size` a cada `$LC` (lote6 R-a; lote5 regla 5, visto con `-S`).
- El ELF original no tiene ningún símbolo `$LC` (1.711 símbolos de `.rodata`, 0 `$LC`; lote2
  §1.5), así que sus cadenas muertas quedaron enteras.
- Receta literal + `@lc` + lcfix, medida en lote5 con CAnimProperty: sin keep, ROTO (494d226a);
  con keep sin renumerar, ROTO; con renumeración, 9619ba57.
- Misma receta en lote3 S1 (zSim, 9619ba57) y en lote4 LuaBindery (hash igual a la base).

**K8. El prefijo bWare/STL de 92 B sale en C.**
- Receta: `#line 1 "d:/mw/speed/indep/bware/inc/bware.hpp"` + primer muerto con los 6 literales
  (el de la ruta devuelve `__FILE__`) + `#line` de vuelta + 6 `@lc`, renumerando +6 la unidad.
- Lote3 S2 en zSim: `.rodata` del objeto idéntica byte a byte (8.344 B) y relink 9619ba57.
- Lote4: `#line` en una mini-TU emite la ruta exacta de EA.
- Sin medir en las otras 18 unidades.
- En zDynamics y zMission cc1plus ya emite `"bad_alloc"` entre las dos mitades del prefijo, y GCC
  funde los literales iguales de una TU (lote2 §1.7). El primer de esas dos no puede llevar
  `"bad_alloc"`.

**K9. `__attribute__((aligned(8)))` sobre la primera variable de `.data` sube la sección a
`sh_addralign = 8` y redondea `sh_size` a múltiplo de 8.** Es lo mismo que hace un `double`.
- Sonda P2 de esta síntesis, con los cflags de zBWare y de zEcstasy:
  - `int a aligned(8), b, c`: align 8, size **16**;
  - la misma TU sin atributo: align 4, size 12;
  - con `double a`: align 8, size 16.
- Coincide con lote2 §1.6: NgcAs rellena la sección hasta su alineación.
- El objeto sería equivalente al del `.balign 8` en el offset 0 que lote3 enlazó en zBWare
  (9619ba57).
- **Sin medir en el enlace en forma C.** Tampoco es la construcción original: nadie ha encontrado
  qué subía la `.data` a 8 (no hay `double` en el DWARF de zEcstasy, zAI ni zPlatform).
- No es universal: en zPhysics la alineación 8 resucita un dato muerto nuestro (lote3 P-align,
  `.data` +0x20).
- El redondeo de `sh_size` puede mover la unidad siguiente: lote5 Z2 sólo parcheó `sh_addralign`,
  sin tocar el tamaño.

**K10. Borrar un relleno o marcador que ya ponen el compilador, el ensamblador o el enlazador no mueve el DOL.**
- Relink 9619ba57 midiéndolos por separado: criticalpath.c:418, Geometry.cpp:639, DeInterlace.c:3,
  abortmsg.cpp:7 (lote5), inittmr.cpp:38 (lote3 I1), fflush.c:4 y zFeOverlay.cpp:151 (lote4).
- Objeto idéntico: los 8 rellenos de zEcstasy (lote1 V1), TokenEntropy.c:13 (lote6 M2) y
  allocator.cpp:41 (lote2).
- Contraprueba: los 4 B de `.sdata` de filesys.cpp:2022 sí son contenido (lote5: quitarlos rompe).

**K11. Los bloques de cadenas y de cola se compensan entre sí, y eso sólo se ve en el enlace.**
- Lote4, regla 11: los `.asciz` de zWorld escriben −124 B y la cola zWorld.cpp:191 lo cuadra.
  Alinear los siete y recortar la cola cuadra las secciones pero da +784 B.
- Lote1: `aivehicle`, `SMS_MESSAGE_%d`, `GManager` y `visuallook` salen DOS veces en el enlace
  base, la copia `@lc` y la del fragmento.
- Lote3: quitar zTrack.cpp:212 deja la `.rodata` −128 B.
- Precedente: la lección de la r67.

**K12. Los inicios de `.data` de `.debug_aranges` están en disputa.** Lote3 los toma como el
arranque real; lote4 los mide 32 B por debajo de splits.txt en zAttribSys y zFEng, y el DWARF de
zFoundation (`gQuatCompMask` en 0x8041D198) no encaja con ellos. **Nada de la r68 depende de esto.**

**K13. dolwhere infla las cuentas, así que el test de una pendiente es relativo.**
- Lote1: la base de zEcstasy cuenta 3.424 B en dolwhere y son 2.468 exactos.
- Test válido: mismo número y mismo hash que la base, medidos al principio de la r68 en el mismo
  HEAD y con la misma herramienta. Nunca un umbral absoluto.

## 3. Los lotes de la r68

**Reglas comunes a todos los agentes**
- **Propiedad.** Cada agente es dueño exclusivo de los ficheros de su lote.
- **Qué entrega.** La fuente, más un **diff de keep.lst** en un fichero aparte. keep.lst, lcfix,
  splits y configure son del coordinador.
- **Cómo prueba:**
  - compila en un espejo o copia, con cwd dentro (trampa de lote1: una sombra con `-I` cambia
    `__FILE__`);
  - enlaza en un temporal con una **copia** de keep.lst con su diff, lcfix pasado sobre esa copia y
    el objeto **con el mismo nombre de fichero** (K4);
  - reutiliza los arneses de los exploradores: `lote5_68/relink.py` y `trysrc.py`,
    `lote4_68/mylink.py` y `lcres.py`, `lote668/dolwhere_obj.py`, `ecsdat68/mkvar2.py`,
    `cc3.py` y `dw.py`.
- **Orden dentro de una unidad.**
  1. Primero `.data`/`.bss`, que no renumeran nada.
  2. Relink.
  3. Después los `@lc` y el prefijo, que renumeran la unidad.
  4. Relink.
- **Test por tipo de unidad:**
  - **promocionada**: relink del DOL = 9619ba57;
  - **pendiente**: hash y número del enlace iguales a la base de r68-inicio y secciones cuadradas
    (K13).
- **Cierre del coordinador en la ventana:**
  - aplicar el diff de keep.lst, que es CRLF;
  - `lcfix.py` y `lcfix --check`, y además `keepchk.py`;
  - build;
  - sha1 del DOL;
  - **sellar el sha1 de cada `.o` promocionado contra su fuente**, porque `build_direct` puede
    mentir;
  - `dolwhere` de cada pendiente tocada.

### Tanda 1: unidades promocionadas (51 bloques)

| lote | unidad(es) y ficheros en propiedad | bloques | método | medido |
|---|---|---|---|---|
| **L1** bibliotecas | vp6: `allocator.cpp`, `TokenEntropy.c`, `DeInterlace.c`, `clamp.c`, `criticalpath.c`, `vputil.c`, `uoptsystemdependant.c`; realcore: `inittmr.cpp`, `abortmsg.cpp`; egami: `maddeca.cpp`; libc: `fflush.c` | 10 (1.192 B) | ver detalle L1 | 9 de 10: todos salvo clamp.c:3, que es el patrón de DeInterlace/abortmsg |
| **L2** zFoundation + zDynamics + zMission | `UMath.cpp`, `USpline.cpp`, `SourceLists/zFoundation.cpp`, `Geometry.cpp`, `Collision.cpp`, `SourceLists/zDynamics.cpp`, `SourceLists/zMission.cpp`, `MissionEdit.cpp` | 10 | A1 (UMath, USpline, Geometry) y luego el prefijo por unidad | 5 de 10 (UMath/USpline ALLOC idénticas; Geometry por relink) |
| **L3** zBWare | `SourceLists/zBWare.cpp`, `bMemory.cpp`, `bMath.cpp` | 7 | 6 muertas con nombre + keep, `bSinTable[513]` sin keep; luego el prefijo `zBWare.cpp:6` | 6 de 7 (T1b, B1) |
| **L4** zSim | `GameplayActivity.cpp`, `NISActivity.cpp`, `LocalPlayer.cpp`, `SourceLists/zSim.cpp`, `QuickGame.cpp` (sólo el `.set` de la línea 733) | 6 | S1 (4 cabezas → `@lc`), `zSim.cpp:211` con la misma receta, S2 (prefijo) | 5 de 6 |
| **L5** zAnim | `AnimScene.cpp`, `AnimPlayer.cpp`, `AnimEngineManager.cpp`, `SourceLists/zAnim.cpp`, `AnimCandidates.cpp`, `AnimChooseArrest.cpp`, `AnimCtrl.cpp`, `WorldAnimCtrl.cpp` | 4 + 5 de sonda | C1: `new ("Nombre", 0)` + `@lc`; **sonda E1** de la palanca `aligned(8)` (K9) | CAnimProperty medido; la sonda se midió por parcheo de cabecera, no en C |
| **L6** zFeOverlay + zAttribSys | `DebugCarCustomize.cpp`, `CarCustomize.hpp` (línea 712), `uiQRCarSelect.cpp`, `SourceLists/zFeOverlay.cpp`, `SourceLists/zAttribSys.cpp` | 6 | muertas con nombre + keep; borrar `zFeOverlay.cpp:151`; prefijo `zFeOverlay.cpp:26` | 5 de 6 |
| **L7** prefijo de zMain + zFEng + zOnline | `SourceLists/zMain.cpp`, `SourceLists/zFEng.cpp`, `SourceLists/zOnline.cpp` | 3 | receta S2 | 0 (extensión) |

**Detalle L1**
- Borrar: `allocator.cpp:41`, `TokenEntropy.c:13`, `DeInterlace.c:3`, `clamp.c:3`,
  `criticalpath.c:418`, `inittmr.cpp:38`, `abortmsg.cpp:7`, `fflush.c:4`.
- `maddeca.cpp:21`: `static const int zigzag[64] = {0, 8, 1, 2, …, 55, 63};` en lugar del
  `extern` y el asm.
- `vputil.c:47` (920 B, medido V5 en las dos unidades):
  - en `uoptsystemdependant.c`: `extern void ClearSysState_C(void);` y
    `ClearSysState = (VP6_FUNC)ClearSysState_C;`;
  - en `vputil.c`: conservar el `extern int FData[88];` previo y definir
    `int FData[88] __attribute__((section(".bss")));`, `idct[65]` e `idctc` igual, y los 12
    punteros con `section(".sbss")`, en el orden de hoy.
- Test: relink tras cada fichero, y otro al final con todo.
- Riesgo 1: `FData` queda GLOBAL. En el original es local: el DOL no lo ve, la tabla de símbolos sí.
- Riesgo 2: los tres `gcc2_compiled` los pusieron promote.py/objdiff, que comparan la tabla de
  símbolos con el extraído. El coordinador comprueba que el informe no degrada la unidad.

**Detalle L2**
- `UMath.cpp:107/173`: `r *= 0.0027777778f;` y `angle = r * 6.2831855f;`, borrando los externs
  `lbl_803EB44x` y los asm.
- `USpline.cpp:196`: `UMath::Clamp(tangentLengthCubed, -0.0001f, 0.0001f)` y `return 1000.0f;`.
- `Geometry.cpp:607`: dentro de `FindIntersection`,
  `static const Algo algos[Geometry::MAXSHAPES][Geometry::MAXSHAPES] = {{0,0,0},{0,BoxVsBox,BoxVsSphere},{0,SphereVsBox,SphereVsSphere}};`.
- `Geometry.cpp:639`: borrar.
- Los dos de Geometry se midieron por separado: hay que medirlos juntos.
- Prefijos:
  - zFoundation: `zFoundation.cpp:6` entero;
  - zDynamics: `zDynamics.cpp:20` + `Collision.cpp:9` (dos mitades alrededor del `"bad_alloc"`
    que ya emite cc1plus: el primer no lo lleva, K8);
  - zMission: `zMission.cpp:16` + `MissionEdit.cpp:5`, igual.

**Detalle L3** (C de lote3, medido en T1a/T1b)
```cpp
void (*SendPacketFunction)(struct bFunkPacketHeader *, const void *, int) = 0;
void (*ServiceMonitorFunction)() = 0;          // zBWare.cpp:41
int bPListAllocationNumber = 0;                // zBWare.cpp:65
int bMemoryPrintEachAllocation = 0; unsigned int bMemoryPrintAllocationRangeLow = 0;
unsigned int bMemoryPrintAllocationRangeHigh = 0; int EnableCleanupBorrowedMemoryBlock = 1;
int BorrowMemoryBlockMinSize = 0x19000;        // bMemory.cpp:173 (orden del DWARF)
int bMemoryUseSharedStrings = 1;               // bMemory.cpp:196
int bMemoryBreakOnFancyStompDetection = 0;     // bMemory.cpp:219
unsigned short bSinTable[513] = {0};           // bMath.cpp:242, sin keep
```

**Detalle L5**
- Receta C1 (`new ("Nombre", 0)` + `# @lc zAnim "Nombre"`), borrando el bloque y el alias
  `zanim_lc_*`:
  - `AnimScene.cpp:586`;
  - `AnimPlayer.cpp:139`;
  - `AnimEngineManager.cpp:66`, con el nombre por defecto `"Unnamed MyEAGLNewOverride"`;
  - `zAnim.cpp:466`.
- **Sonda E1**, sólo si C1 ha pasado:
  1. `__attribute__((aligned(8)))` en la primera variable de la `.data` de zAnim.
  2. Las 5 muertas del DWARF en lugar de los asm: `g_WorldMarkerPlayed[21]`, borrar
     `AnimChooseArrest.cpp:105` (sus variables ya están escritas), `AnimDelayTimeAddition`,
     `gCarAnimationStatesActive`, `WorldAnimDelayTimeAddition`.
  3. Comparar `sh_size` de `.data` antes y después (K9: el redondeo puede sumar 4 B).
  4. Relink.
- Si da 9619ba57, K9 queda medida en enlace y la palanca pasa a ser el mecanismo de la r69 para 21
  bloques más. Si no, se revierte **sólo la sonda** y se informa.
- `AnimScene.cpp:28` no entra: lleva 4 B sin identificar.

**Detalle L6**
- `namespace Attrib { static unsigned int gDefaultExportID = 0; }` (zAttribSys.cpp:536).
- `static uint32 gCarTypeNameHash = 0;` en DebugCarCustomize.cpp:19, **quitando**
  `static uint32 gCarTypeNameHash;` de `CarCustomize.hpp:712`.
- `static int gInstallCarPartID = -1;` (DebugCarCustomize.cpp:32).
- `bool QRCarSelectBustedManager::bIsCross = false;` (uiQRCarSelect.cpp:35).
- Borrar `zFeOverlay.cpp:151`.
- Prefijo `zFeOverlay.cpp:26`: va en `.over`, y keep.lst lo escribe en minúsculas
  (`lbl_803c6a40`).
- **Riesgo de cabecera.** `CarCustomize.hpp` también la incluye `FEPackageData.cpp`, de zFe2
  (pendiente). Por K5, zFe2 emite hoy esa estática en su `.bss` y dejará de hacerlo. El test de L6
  suma `dolwhere zFe2` idéntico a la base.

### Tanda 2: unidades pendientes (80 bloques)

| lote | unidad y ficheros en propiedad | bloques | método | medido |
|---|---|---|---|---|
| **L8** zEcstasy | `EcstasyE.cpp`, `EcstasyEx.cpp`, `eEnvMapE.cpp`, `eLightE.cpp`, `ePShader.cpp`, `eStrip.cpp`, `eVisualTreatment.cpp`, `eViewPlat.cpp`, `eView.cpp`, `Src/Ecstasy/Ecstasy.cpp`, `eLight.cpp`, `eSolid.cpp`, `SourceLists/zEcstasy.cpp` | 42 | 10 R1 (8 rellenos borrados, `volatile Bool bHangRecovery = 0;`, `volatile unsigned short load_sync = 0;`) + 32 muertas con nombre + keep (paquete V5/V6k sin los numerados) | **42 de 42** (V1, V2, V5, V6k: 2.468 B = base) |
| **L9** zCamera | `Camera.cpp`, `CameraMover.cpp`, `TrackCop.cpp`, `DebugWorld.cpp`, `Cubic.cpp` | 5 | muertas con nombre + keep | 4 de 5 (M3/M5: 15.557 = 15.557) |
| **L10** zPhysics | `VehicleSystem.cpp`, `SourceLists/zPhysics.cpp` | 4 | 23 globales de `VehicleSystem` + keep por nombre mangled; luego el prefijo `zPhysics.cpp:37` | 3 de 4 (P-keep) |
| **L11** zWorld | `CarRender.cpp`, `World.cpp`, `VehiclePartDamage.cpp` | 3 | `int OpToM = 0;`, `float TrafficTeleporterDistanceAroundBubble = 0.0f;`, borrar el asm de VehiclePartDamage.cpp:338 (las estáticas ya están en :158) + keep | 3 de 3 (hash 2494248b = base) |
| **L12** zTrack | `TrackInfo.cpp`, `Scenery.cpp`, `WeatherMan.cpp`, `ScreenEffects.cpp`, `SourceLists/zTrack.cpp` | 8 | muertas con nombre + keep | medido con `.balign 8` (T-align), **la variante keep está sin medir** |
| **L13** zLua | `LuaBindery.cpp`, `LuaAttributes.cpp`, `LuaGameHooks.cpp`, `SourceLists/zLua.cpp` | 8 | primer `static inline` + `@lc`: LuaBindery.cpp:152, zLua.cpp:71/96/108/176, LuaAttributes.cpp:266, LuaGameHooks.cpp:80; luego el prefijo `zLua.cpp:9` | 1 de 8 (LuaBindery, 27c00e55 = base) |
| **L14** pendientes varias | `SourceLists/zEAXSound.cpp`, `SourceLists/zEAXSound2.cpp`, `GRaceStatus.cpp`, `SourceLists/zGameplay.cpp`, `SpeechManager.cpp`, `SourceLists/zSpeech.cpp`, `SourceLists/zAI.cpp`, `SourceLists/zFe.cpp` | 10 | ver detalle L14 | 2 de 10 |

**Detalle L8**
- El C de cada bloque está en la tabla de lote1. Specs y generador: `scratchpad/ecsdat68/genV5.py`
  y `spec_V5.json`.
- Obligatorio en el mismo paquete:
  - borrar las declaraciones que chocan: `EcstasyE.cpp:377`, `EcstasyE.cpp:406-407`,
    `eStrip.cpp:31` y `zEcstasy.cpp:426-428`;
  - poner `CurrentStuffView` en eView.cpp, `MemoryUsedByDuplicatedSolids` en eViewPlat.cpp y
    `TestMWDesaturation` en eVisualTreatment.cpp;
  - usar el `enum` del DWARF donde V5 midió con `int`: `DisplayBufferMode`,
    `eTevSwap*InputIndex`, `LightMaterialID`, `GXBlendModeTweak`.
- `EcstasyEx.cpp:2776` lleva `cBfR`, estático de fichero: sin número, entra aquí.
- `EcstasyE.cpp:339` y `:4972` llevan numerados: van a L15.
- Test: enlace = 2.468 B y el mismo hash que la base de r68-inicio, con secciones iguales.

**Detalle L12**
- Test previo: repetir T-align sin `.balign 8` y con keep por nombre.
- Si la variante keep no cuadra, el lote se para: no se usa el asm de alineación.

**Detalle L14**
- zEAXSound2: borrar los dos asm vacíos (`zEAXSound2.cpp:452` y `:460`); prefijo `:4`.
- zEAXSound: prefijo `:4`.
- zGameplay: quitar `lbl_803EBE90` (`GRaceStatus.cpp:2223`); es de zGameModes y está duplicado.
  Prefijo `:9`.
- zSpeech: `int SPEECHFLOW_DISPLAY_DITCHED = 0;` en `SpeechManager.cpp:124`; prefijo `:38`.
- zAI: prefijo `:78`.
- zFe: prefijo `:17`.
- Se puede partir en dos agentes: (zEAXSound, zEAXSound2) y (zGameplay, zSpeech, zAI, zFe).

### L15: herramienta de estáticos numerados (en paralelo con la tanda 1)

- Nueva directiva de keep.lst que resuelva `<nombre>.<N>` por contenido, como `@lc`. Propuesta:
  `# @lcl <unidad> <nombre> <k>`, que se traduce en `<unidad>.o:<nombre>.<N>` para el k-ésimo
  símbolo local con ese nombre, por dirección en la sección.
- La integran lcfix y keepchk.
- **Control de aceptación:** regenerar las 9 entradas de `keep_V6k` de lote1 (`last_zone_hash`,
  `once` ×6, `fontShift`, `cBfR`) y obtener las mismas líneas.
- Desbloquea en la tanda 2 (se añaden a L8 y L11):
  - `EcstasyE.cpp:339` y `:4972`;
  - `rain.cpp:361`, `:401` y `:473`;
  - `CarRender.cpp:5039`.
- Si la sonda E1 sale bien, la palanca haría innecesaria la herramienta **sólo** en las unidades
  donde se aplique: un estático de 4 B sobrevive sin keep con `.data` a 8.

## 4. Paquetes de ventana (los aplica el coordinador)

Los números de línea son de keep.lst en 3c08534b; hay que volver a buscar cada etiqueta antes de
editar. keep.lst es CRLF.

**L1, L2, L7:** sin entradas de datos en keep.lst (grep negativo de `gap_05_8041227E`,
`gap_08_804FF5A4`, `gap_05_80412774`, `zigzag`, `algos`, `lbl_803EB44x`, `lbl_803EB73C`).
Los prefijos llevan sus líneas en §4-prefijo.

**L3 zBWare:**
```
135 zBWare.o:lbl_8041641C          -> zBWare.o:bMemoryPrintEachAllocation
                                      zBWare.o:bMemoryPrintAllocationRangeLow
                                      zBWare.o:bMemoryPrintAllocationRangeHigh
                                      zBWare.o:EnableCleanupBorrowedMemoryBlock
                                      zBWare.o:BorrowMemoryBlockMinSize
136 zBWare.o:lbl_80416434          -> zBWare.o:bMemoryUseSharedStrings
146 zBWare.o:gap_06_80415944_data  -> zBWare.o:SendPacketFunction
                                      zBWare.o:ServiceMonitorFunction
150 zBWare.o:gap_06_80415954_data  -> zBWare.o:bPListAllocationNumber
151 zBWare.o:gap_06_804161E8_data  -> (borrar: bSinTable va sin keep)
152 zBWare.o:gap_06_80416444_data  -> zBWare.o:bMemoryBreakOnFancyStompDetection
147-149 (comentario "Huecos escritos a mano ...") -> actualizar o borrar
```

**L6 zAttribSys / zFeOverlay:**
```
131 zAttribSys.o:gap_06_804158E4_data -> zAttribSys.o:_6Attrib.gDefaultExportID
809 zFeOverlay.o:lbl_8043923C         -> zFeOverlay.o:gInstallCarPartID
810 zFeOverlay.o:gap_06_80439154_data -> zFeOverlay.o:_24QRCarSelectBustedManager.bIsCross
811 zFeOverlay.o:gap_06_80439234_data -> zFeOverlay.o:gCarTypeNameHash
812 zFeOverlay.o:gap_06_8043948C_data -> (borrar, caduca)
```

**L4 zSim** (números antes de lcfix; lcfix los renumera):
```
+ # @lc zSim "MNotifySimTick"        + zSim.o:$LC626
+ # @lc zSim "MNISComplete"          + zSim.o:$LC550
+ # @lc zSim "MNotifyMovieFinished"  + zSim.o:$LC555
+ # @lc zSim "FEngHUD"               + zSim.o:$LC478
+ # @lc zSim "SimModel"              + zSim.o:$LC511   (zSim.cpp:211; SmackableParams y
                                                        Attrib::Gen::world: numero que dé lcfix)
```

**L5 zAnim:**
```
+ # @lc zAnim "CAnimProperty"               + zAnim.o:$LC439   (lcfix: $LC458 -> $LC459)
+ # @lc zAnim "CAnimResourceFileProxy"      + (lcfix)
+ # @lc zAnim "Unnamed MyEAGLNewOverride"   + (lcfix)
+ # @lc zAnim "GenericNISControlScenario"   + (lcfix)
sonda E1 aceptada: 82 zAnim.o:lbl_80415678 -> borrar
                   (84 gap_06_8041577C_data se queda: AnimScene.cpp:28 no entra)
```

**L8 zEcstasy:**
- Borrar las 10 entradas de L8-R1: líneas 339 (AD0A), 340 (AD5A), 341 (AD71), 342 (AD7D),
  343 (AD82), 344 (AD86), 346 (ADF2), 347 (AE02), 348 (AFB3) y 349 (AFDF).
- Sustituir estas 33 líneas por las líneas con nombre que genera `scratchpad/ecsdat68/genV5.py` +
  `genV6.py` (`keep_V6.lst`, una por variable), **sin** las 9 que V6k añade a mano
  (`last_zone_hash`, `once` ×6, `fontShift`, `cBfR`). De esas 9, `cBfR` entra aquí porque no lleva
  número; las otras 8 esperan a L15. Líneas a sustituir:
  - `lbl_`: 300, 301, 302, 304, 305, 307, 308, 310, 311, 312, 313, 316, 317, 319, 320, 321, 322,
    323, 326, 327;
  - `gap_`: 329, 330, 331, 332, 333, 334, 336, 337, 345, 350, 351, 353, 354.
- Se quedan las entradas de R3/R4: 299 (A62C), 303 (A6A8), 306 (ACAC), 309 (ADE8), 314 (AF88,
  espera a L15), 315 (AFC0), 318 (B0C8), 324 (B468), 325 (B49C), 328 (B4D8), 335 (AB90),
  338 (ACF8, espera a L15), 352 (B0AC), 355 (B520) y 356 (B530).
- Con L15: 314 → `once.<N>` ×6 + `fontShift.<N>` por directiva; 338 → `TweakDumpWeatherInfo` +
  `last_zone_hash.<N>`.

**L9 zCamera:**
```
160 zCamera.o:lbl_804164C8         -> zCamera.o:NearClipInCar
161 zCamera.o:lbl_80416FF4         -> zCamera.o:nWhichChaseCamera
162 zCamera.o:lbl_80417008         -> zCamera.o:_21DebugWorldCameraMover.SlowSpeed
165 zCamera.o:gap_06_80416538_data -> zCamera.o:Tweak_EnableRoadEditor
                                      zCamera.o:Tweak_PursuitStartCamTest
                                      zCamera.o:Tweak_JumpCamTest
                                      zCamera.o:bICETest
                                      zCamera.o:bCinematicSlowdownTest
                                      zCamera.o:bShowcaseTest
167 zCamera.o:gap_06_80416FE8_data -> zCamera.o:Demo1On
                                      zCamera.o:Demo2On          (sin medir)
```

**L10 zPhysics** (prefijo `zPhysics.o:_13VehicleSystem.` en todas):
```
489 zPhysics.o:lbl_8041EFF0 -> CAR_LASER_LIFESPAN CRASH_RESET_COUNT CRASH_RESET_COUNT_BIKE
                               TRAFFIC_OPT_DISTANCE_SQUARE COLLISION_LANDING_DELAY
                               LANDING_ASSIST_ENABLE_COUNT
490 zPhysics.o:lbl_8041F00C -> BASE_FRICTION_MASS ENABLE_TWO_WHEEL_TESTS ENABLE_BOOST_TESTS
                               SHRED_DRAG ENABLE_COLOR_CHECK
491 zPhysics.o:lbl_8041F024 -> SKID_AUDIO_SCALE ROLLING_RESISTANCE MIN_BUTTON_VALUE
                               WHEEL_SPIN_EXTRA_RPM MAXSPEED_REVERSELIGHTS DAMAGE_SCALE_COLLISION
                               EMP_LIFETIME TYRE_DAMAGE_RADIUS MAX_WHEEL_SPIN_RATE_AI
                               POST_BRAKE_ACCEL_COUNT POST_BRAKE_ACCEL_SCALE POST_BRAKE_ACCEL_MIN
```
(una línea `zPhysics.o:_13VehicleSystem.<NOMBRE>` por nombre: 23 líneas)

**L11 zWorld:**
```
723 zWorld.o:lbl_80438F08         -> zWorld.o:testDamageLevel
                                     zWorld.o:testDelay
769 zWorld.o:gap_06_80437604_data -> zWorld.o:OpToM
765 zWorld.o:gap_06_8043724C_data -> zWorld.o:TrafficTeleporterDistanceAroundBubble
con L15: 720 lbl_80437614 -> fc.<N> + CarLightAngles + lastlight;
         721/722 lbl_80438AA4/AA8 -> BaseDampness + ChangeWeatherTime.<N>;
         entradas de index/windState/changetime -> por directiva (grep previo: hoy no hay línea con nombre)
```

**L12 zTrack:**
```
583 zTrack.o:gap_06_80435F70_data -> zTrack.o:_9TrackInfo.MaxTrackInfo
585 zTrack.o:gap_06_80436004_data -> zTrack.o:DropTopSceneryLOD
587 zTrack.o:gap_06_80437064_data -> zTrack.o:RunPrecullingTool
579 zTrack.o:lbl_8043710C         -> distColAff distfall ColourTintParamRed ColourTintParamGreen
                                     ColourTintParamBlue ColourTintParamIntensity
578 zTrack.o:lbl_804370EC         -> HorizFogFalloff HorizFogFalloffX HorizFogFalloffY
                                     HorizWeatherFog HorizWeatherFogStart
580 zTrack.o:lbl_8043712C         -> frame_max frame_speed
588 zTrack.o:gap_06_80437138_data -> InTunnel
581 zTrack.o:lbl_80437144         -> DarkFalloff DarkFallon tunnelDark
```

**L13 zLua:** `# @lc zLua "LuaBindery"` y una `# @lc` por literal de ≥ 8 B de los otros 6 bloques.
lcfix renumera las 88 `@lc` de zLua.

**L14:** `567 zSpeech.o:gap_06_8043597C_data` → borrar. No hace falta keep: la `.data` de zSpeech
va a 8, y esa alineación la da el asm de `zSpeech.cpp:245`, que en la r68 NO se toca.

**§4-prefijo** (L2, L3, L4, L6, L7, L10, L13, L14). Por unidad, la entrada actual del prefijo:
```
zBWare.o:lbl_803D0E18   zSim.o:pad_05_80403F48_rodata   zPhysics.o:pad_05_803F68F0_rodata
zFEng.o:pad_05_803EA7E8_rodata   zLua.o:pad_05_803ED338_rodata   zFoundation.o:pad_05_803EB230_rodata
zEAXSound.o:lbl_803D5E38   zEAXSound2.o:lbl_803D9050   zGameplay.o:lbl_803EBE98   zAI.o:lbl_803C8C60
zMain.o:lbl_803EF580   zSpeech.o:lbl_80405E10   zOnline.o:lbl_803a4234   zFe.o:lbl_803DF568
zFeOverlay.o:lbl_803c6a40 (808)   zDynamics.o:lbl_803D3DC8   zMission.o:pad_05_803F6890_rodata
```
- Se sustituye por 6 pares `# @lc <u> "<literal>"` + `<u>.o:$LCn`, como en S2 (zSim `$LC0`-`$LC5`):
  `"GAMECUBE"`, `"d:/mw/speed/indep/bware/inc/bware.hpp"`, `"bad_alloc"`, `"%f,%f,%f"`,
  `"%f,%f,%f,%f"`, `"STL"`.
- zDynamics y zMission: sin `"bad_alloc"` (K8).
- A continuación, `lcfix` en la unidad (+6 en todas sus `@lc`). Hay que revisar a mano los `.set`
  y `@lcsrc` que nombren un `$LC`; en zSim es `QuickGame.cpp:733`, que pasa a `$LC529`.

**splits.txt / configure.py:** ninguna línea en la r68.
- Aplazado a la r69, con medida pendiente:
  - `gc_interface.cpp:55`: sacar `auto_05_804144C0_rodata.o` (lote4, DOL OK con `--drop`);
  - los símbolos absolutos `GXWGFifo` y `e_OSBusClock` en el ldscript;
  - re-extracción por el alias de `stdio.c`.
- La medida: `grep -n 804144C0 config/GOWE69/splits.txt` y relink con la frontera.

## 5. Bloques que se compensan y deben ir juntos

1. **Cadenas de zWorld ↔ su cola.** Los `.asciz` de zWorld (CarInfo.cpp:927 y :1683;
   CarLoader.cpp:254, :1569 y :2300; CarRender.cpp:2494; SkyRender.cpp:134 y :305; World.cpp:95;
   zWorld.cpp:92 y :128) compensan con la cola zWorld.cpp:191 (−124 B contra +cola). Fuera de la
   r68.
2. **zTrack.cpp:4 ↔ zTrack.cpp:212** (−128 B de contenido real).
3. **zEagl4Anim.cpp:20 ↔ zEagl4Anim.cpp:225.** Los `AnimBankType`/`SkeletonType` duplicados tapan
   24 B que faltan en la `.rodata`.
4. **zAI.cpp:639 (761 B) ↔ zAI.cpp:718** (`.space 280`).
5. **SpeechManager.cpp:124 (L14) ↔ zSpeech.cpp:245.** El `.balign 8` de :245 es lo que mantiene
   viva la variable sin keep. :245 no se toca antes de tener la palanca K9.
6. **zMisc.cpp:3699.** Su propio `.balign 8` sostiene la supervivencia de sus 13 muertas: pasarlo a
   C exige `aligned(8)` en la misma edición.
7. **zEcstasy, `.rodata`.** EcstasyE.cpp:516 y EmitterSystem.cpp:57 compensan con las `@lc`
   duplicadas del enlace base (aivehicle, SMS_MESSAGE_*, GManager, visuallook). Los 9 rellenos del
   pool (EcstasyE.cpp:1525/5506/5643, EmitterSystem.cpp:257/288/594/1282/1680, Sun.cpp:178) caen
   con sus vecinas.
8. **zEcstasy, `.data`.** `gap_06_8041B530` (156 B) y `Sun.cpp:18` (76 B) son candidatos a
   compensar nuestra alineación. Tras L8, medir con `desplaza` antes de tocarlos.
9. **zEAXSound/zEAXSound2.** Pasar sus muertas de COMMON a `.data` cambia la `.bss`, así que va con
   el re-barrido de `zEAXSound2.cpp:480` (`.space 56`) y `zEAXSound.cpp:471` (`.space 16`).
   `lbl_80418978`, `lbl_8041897C` y el orden de `szMixMapFiles` van juntos. r69.
10. **zLua.cpp:421** (`.space 16`) ↔ el pool `bf800000` que falta en 0x803EEA48.
11. **zPhysicsBehaviors.cpp:185 y :232** (andamios de tamaño) ↔ su `.bss`, `.sdata` y `.sdata2`.
12. **Prefijo (C2) ↔ numeración `$LC` de la unidad.** Incluye los `.set` y `@lcsrc`. Siempre en la
    misma edición que su lcfix.
13. **zFEng.cpp:92** (40 B que son de zFoundation) ↔ la cabeza de la `.data` de zFoundation y la
    frontera zFEng/zFoundation.
14. **GRaceStatus.cpp:2223 (L14)** ↔ la ventana de 32 B de la `.rodata` de zGameplay: salen 4 B
    menos, dentro de la ventana.
15. **Palanca K9 ↔ keep por nombre.** En una unidad con `.data` a 8:
    - un array muerto de ≥ 12 B NO puede llevar keep, porque lo conservaría entero y el original
      sólo tiene la cola;
    - el redondeo de `sh_size` puede mover la unidad siguiente;
    - en zPhysics resucita +0x20: allí sólo vale keep.
16. **zAttribSys.cpp:569** (36 B anónimos) ↔ el arranque de la `.data` de zBWare. Disputa de
    aranges, K12.

## 6. Lo que es R3 de verdad

Recuento propio tras la armonización de §1. No se cuentan como R3 los que ya tienen receta medida.

**No son parches de datos (6 bloques, 16 B):**
- `sndvd.c:333`: `.long 1` dentro de `.text`. Es la trampa del Target Manager, código.
- `libsn.h:98-100`: macros de trampa de ProDG que **ninguna unidad compila**. No emiten un solo
  byte; son contenido del SDK.
- `ppc2D2.c:58`: siete trampolines `b __xxxdi3`. En el original eran asm: no hay CU de DWARF, y
  GCC 2.95.3 no hace llamada de cola (medido en lote5).
- `eathread_thread_gc.cpp:50`: el alias `_GLOBAL_.I.*`. Existe porque el static-init está escrito a
  mano: es frente de codegen, no de datos.

**Contenido del original en posición falsa (16 bloques, 10.253 B), el grueso del frente:**
- zFe.cpp:427, zFe2.cpp:771, zPhysicsBehaviors.cpp:139, zEAXSound.cpp:98, zEAXSound2.cpp:395,
  zGameplay.cpp:89, zWorld.cpp:191, zTrack.cpp:212, zEagl4Anim.cpp:225;
- los `.space` de zAI.cpp:718, zSpeech.cpp:289 y zLua.cpp:421;
- las palabras de ajuste de ventana de zEAXSound.cpp:416 y zEAXSound2.cpp:506;
- zPhysicsBehaviors.cpp:232 y zPlatform.cpp:248.

Como bloque no tienen forma C: un primer al final sería igual de falso. Sólo se disuelven cadena a
cadena, con cada constructo en su sitio, y midiendo en el enlace (K11).

**Otros R3 (17 bloques):**
- Relleno del pool `.rodata` (9): cae con las vecinas (compensación 7).
- Guardas `_.tmp` de `.bss` (3): zCamera.cpp:235, zEAXSound.cpp:471 y zEAXSound2.cpp:480.
  Calibrados por tamaño.
- Pools de funciones estripadas con sesgo int→double (3): SimSurface.cpp:72, el resto de
  zRender.cpp:14 y los pools de zEagl4Anim.cpp:20. El cuerpo no se puede verificar contra el DOL.
- `pathbank.cpp:312`: dos `"%s  %s"` en una TU, y GCC funde los literales iguales.
- `eLightE.cpp:487`: constante de `UpdatePlatInfo`, que no casa (vetada).

**Condicionales a la palanca `.data` a 8 (26 bloques):** 21 de la tabla E2 y 5 de la sonda E1.
- Tienen la declaración C conocida variable a variable; hoy no hay forma C del original que dé la
  alineación.
- `aligned(8)` (K9) **es C, pero no el constructo original**. Si la sonda E1 pasa, **decide el
  usuario** si ese atributo es aceptable.
- Si no lo es, estos 26 siguen en asm: lo que se deja es la cola `size & 7` de un dato que el
  enlazador original recortó.
- `CarRender.cpp:395` y `zEAXSound2.cpp:343` (colas de globales) están en esta lista.

**En disputa (6 bloques):** los «G» de lote3 (zBWare.cpp:106, zEagl4Anim.cpp:249, zPhysics.cpp:481,
zRender.cpp:94, zTrack.cpp:293, NISActivity.cpp:466). No se clasifican hasta resolver K12.

## 7. Después de la r68: medidas que más desbloquean

1. **Resultado de E1 (K9 en enlace):** 26 bloques. Además quita líneas de keep y los numerados de
   las unidades a 8.
2. **zEAXSound + zEAXSound2** (compensación 9): unos 20 bloques con nombre (DWARF + NFS.MAP). Sin
   medir; mueven `.bss`.
3. **zAI/zPlatform:** ~10 muertas identificadas (lote2). Sin medir; keep o `.data` a 8.
4. **zMain:** 12 bloques de etiquetas Attrib (≈1,8 kB) con `ATTRIB_TAG` natural + `@lc`.
   Renumeran las 79 `@lc`.
5. **zMiscSmall.cpp:107** (Config.cpp, 620 B): primero medir si ngcld escribe el addend de un
   `char*` cuyo `$LC` se estripa (lote6 R-f).
6. **Paquete egami del assert:** rcmpbase, bigyuvswizzler, bigswizzler:4, avplayer:195,
   rcmp_mad_codec:225 y rcmp_vp6_codec:257, 6 TU.
7. **Pools de funciones estripadas sin sesgo:** UMath:35/81/181, USpline:100/123/186, UEALibs:34,
   UVectorMath:613, zFoundation:42/60/72/91/152, AnimCtrl:148, WorldAnimCtrl:184, Util.cpp:44.
   Antes hay una **decisión de política**: escribir cuerpos que el enlazador tira y que sólo se
   verifican por su pool.
8. **Literales de cabecera R4:** EcstasyE:516, eView:20, EmitterSystem:57, zFe:88, NISActivity:84
   y otros. Se deciden con variantes en espejo + `marks.py`/`reladdr.py`.
9. **K12 (aranges):** decide los 6 «G» y varias colas de `.data`.
10. **K5, negativo de lote3:** repetir T1c con `= 0`. Reabre `bMemory.cpp:545`, `Simulation.cpp:175`
    y `zMain.cpp:734`.

## 8. Riesgos

- **Renumeración `$LC`.** Cada `@lc` o prefijo desplaza toda la unidad. lcfix sólo cubre `@lc` y
  `@lcsrc`: los `.set` a `$LC` escritos en asm se revisan a mano. El DOL rompe en silencio.
- **Numerados.** Sin L15, cualquier edición posterior de zEcstasy o zWorld invalida los nombres
  `<n>.<N>`. Por eso los 6 bloques no entran sin la herramienta.
- **K4.** Una errata en keep.lst no da error. El único detector es el DOL: nunca aplicar un diff de
  keep sin el enlace.
- **Cabeceras compartidas.** `CarCustomize.hpp` (L6) toca zFe2. Cualquier otra cabecera que se
  toque exige buscar sus includers y medir sus unidades.
- **Pendientes sin cambio en el DOL.** Los lotes de la tanda 2 no mueven el DOL enviado, porque se
  enlaza el objeto extraído. Su valor es acercar la unidad a la promoción y su test es relativo
  (K13). Hay que medir la base de cada pendiente **al empezar la r68**, no reutilizar las cifras de
  los exploradores.
- **Objetos rancios.** Promocionar o certificar exige sellar el sha1 de cada `.o` contra su fuente.
- **Símbolos que cambian de binding.** `vputil.c` deja `FData` global. El DOL no lo ve, pero la
  comparación de tabla de símbolos de promote/objdiff sí; también los tres `gcc2_compiled`.
- **Sonda E1.** Si pasa, no convertir automáticamente el resto: la palanca rompe en zPhysics
  (compensación 15) y cada unidad se mide aparte.
- **Veredictos «medidos» con otra variante.** zTrack (L12) se midió con `.balign 8`. La variante
  keep puede no cuadrar; entonces el lote se para, no se degrada a asm.

## 9. Reproducir

```
python scratchpad/sintesis68/lotes.py    # recuento por lote contra jefe/bloques_datos68.txt
python scratchpad/sintesis68/probe.py    # sondas P1 (estáticos) y P2 (aligned(8)), temporal propio
```
Los arneses de los exploradores siguen en `scratchpad/ecsdat68`, `lote2_68`, `lote4_68`,
`lote5_68` y `lote668`.
