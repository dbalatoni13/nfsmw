# r68 — explora lote1: los 92 parches de datos de zEcstasy

Agente de exploración. **Nada tocado** en `src/`, `config/`, `keep.lst`, `configure.py` ni `build/`.
Todas las variantes se compilaron en un **espejo con uniones** de `src/` dentro de
`scratchpad/ecsdat68/mir/` y se enlazaron con una **copia** de `keep.lst` en un temporal propio.

## 1. Resumen

| veredicto | bloques | bytes asm | estado |
|---|---:|---:|---|
| **R1** sustituible ya por C | **10** | 18 | **medido**: objeto con `.data` idéntica y DOL igual |
| **R2** sustituible como paquete (C + `keep.lst`) | **34** | 354 | **medido**: los 34 a la vez, DOL igual |
| **R3** del original, sin forma C hoy | **17** | 114 | mecanismo identificado |
| **R4** no decidible con lo disponible | **31** | ~3.300 | medida que lo decide, por bloque |
| **total** | **92** | | |

**El paquete R1+R2 está medido entero**: 44 bloques y 372 B de asm pasan a C legítimo con 56
ediciones y 45 líneas de `keep.lst`. El DOL enlazado queda **exactamente como la base**:
2.468 B distintos, 953 en `.text` y 1.515 en `.rodata`, con las secciones cuadradas. Sin
`keep.lst` se rompe: 99.600 B distintos.

Lo que lo hace posible es una regla del **enlazador ORIGINAL** que no estaba escrita (§2.1).
La mayoría de los `lbl_`/`gap_` de `.data` **no son datos anónimos**: son **variables con
nombre que el enlazador original estripó**. Sus bytes siguen en el DOL y el DWARF las lista
con `AT_location = FFFFFFFF`. En el CU de zEcstasy hay 176 así.

## 2. Reglas nuevas medidas

### 2.1 El enlazador original deja la COLA `size & 7` de cada dato estripado, con los addends crudos
- **Qué dice el DWARF**: una variable global estripada conserva su DIE, con
  `AT_location = 03 FFFFFFFF`. `LFrot`, `SmearBiasBASE`, `KColorWorldReflection` y
  `TestMWDesaturation` están ahí. **No son "variables que el objetivo no tiene"**, como decían
  las notas r59: son del original.
- **Qué queda en el DOL**: en cada hueco, las estripadas en orden de DIE entre sus dos vecinos
  con dirección suman exactamente el tamaño del hueco si de cada una sobreviven `size & 7`
  bytes. Casos:
  - las variables de 4 B quedan enteras (`lbl_8041ACAC`: 10 variables);
  - las de 16 B desaparecen enteras (`FillLightDiffuseColour`, `UMath::Vector4`);
  - las de 12, 20, 36 y 44 B dejan 4 B.
- **Los 4 B que quedan son la COLA, y llevan el addend sin reubicar.** El último puntero de la
  tabla aparece como desplazamiento dentro de la `.rodata` de la unidad (base `0x803DD658`).
  Todas estas colas apuntan a la última cadena de su tabla:

  | bloque | valor | tabla | apunta a |
  |---|---|---|---|
  | `lbl_8041B49C` | `0x1ACC` | `vShaderNames[5]` | "eVS_MAXNUM" |
  | `lbl_8041B468` | `0x1A6C` | `LightModeNames[11]` | "LM_DOUBLECHANNEL_NO_LIGHT_AND2VERT" |
  | `lbl_8041AFC0` | `0x1474` | `s_CaptureBufferAllocationString[9]` | "ECSTASY Specular" |
  | `lbl_8041A62C` | `0x394` | `LightTypeStrings[5]` | "EXCLUDE  " |
  | `lbl_8041A62C` | `0x3C0` | `LightAttenuationStrings[5]` | "INVSQR" |

- **Consecuencia**: en el objeto original la `.data` alineaba a 8 (la regla r58 es `size & ~(alin-1)`).
  En el nuestro alinea a 4, y un dato muerto de 4 B **se va entero**. El DWARF del CU no
  tiene ningún `double`/`long long` en `.data`, así que **no se sabe qué subió la alineación a
  8** (R4, §5).

### 2.2 Los estáticos LOCALES muertos conservan su dirección, y el symtab del ELF no los trae
`once`×6, `fontShift`, `last_zone_hash`, `lastVerifyLevel` y `cBfR` tienen dirección real en el
DWARF aunque el código original no los usa. Prueba: esas funciones casan en el enlace base sin
ellos. El `.symtab` del NFSMWRELEASE.ELF no los lista, así que dtk los convirtió en `lbl_`.

### 2.3 Lo que hace NUESTRO enlace, medido
- **Globales de 1-2 B** muertos sobreviven sin `keep` (`size & ~3` = 0). V2 da la `.data` idéntica.
- **Globales de 4 B** muertos se estripan enteros. Con `keep.lst` **por nombre** quedan exactos
  (V3, V4, V5).
- **Estáticos locales de 4 B** muertos: igual. Sin `keep`, la `.data` enlazada sale 0x20 corta
  (V6). Con `zEcstasy.o:once.24919`… queda exacta (V6k). **Los nombres llevan número: son
  frágiles como los `$LC`**, y `lcfix` no los cubre.
- GCC 2.95 **sí emite** estáticos sin uso, tanto locales de función como de fichero
  (control `ctl/unusedstatic.cpp`: `once.3`, `once.4` y `cBfR` en `.data`).
- `bool` mide **4 B** en este GCC (control: `.size once.3,4`). El DWARF lo marca
  FT_boolean; `Bool` es `uchar`.

### 2.4 Otras medidas
- **Relleno de alineación**: los `gap_` de 1-3 B entre un byte y un `int` son el `.align 2` de
  GCC. Quitarlos deja **idénticas `.text`, `.rodata`, `.data` y `.bss` del objeto** (V1).
- **Referencia estática a un temporal**: `static const int &r = g();` de fichero da `r` en
  `.data` y `.lcomm _.tmp_N,4,4` en `.bss` (control `ctl/reftmp.cpp`). Es el mecanismo candidato
  de `pad_07_8045EC40_bss`.
- **Compensación con `keep.lst` MEDIDA**: tres cadenas muertas que `keep.lst` salva con
  `@lc` salen **DOS VECES** en el enlace base: la copia `$LC` temprana y la del fragmento asm.
  `Attrib::Gen::aivehicle` en 803DD828 y 803DDDC0, `SMS_MESSAGE_%d` en 803DD898 y 803DDED0,
  `GManager` en 803DDA04 y 803DF21C. Hay además una segunda copia de
  `Attrib::Gen::visuallook` en 803DF1B0. Un fragmento de `.rodata` puede estar
  compensando una entrada `@lc`.
- **Trampa de herramienta**: poner una sombra de `src` delante con `-I` cambia `__FILE__` y la
  `.rodata` crece 0x20 aunque no se toque. Hay que usar un espejo con uniones y compilar con
  cwd dentro de él (`mkvar2.py`/`cc3.py`). Control: sha por sección idéntico a la base.
- **`dolwhere` infla**: la base cuenta 3.424 B con su criterio de rangos, y son 2.468 B exactos.

## 3. Patrones

| patrón | bloques | veredicto | casos comprobados |
|---|---:|---|---|
| **P1** resto de variable global estripada (entera, ≤4 B) | 31 | R2 | V3, V4, V5: los 31 a la vez |
| **P1b** resto de estático local muerto con dirección | 3 | R2 | V6k (`once`×6+`fontShift`, `last_zone_hash`, `cBfR`) |
| **P1c** resto de COLA de un dato ≥12 B (`size & 7`) | 6 | R3 | colas decodificadas (§2.1) |
| **P1d** P1 con una pieza sin identificar | 3 | R4 | `lbl_8041ADE8`, `gap_06_8041B0AC`, `lbl_8041B4D8` |
| **P2** relleno de alineación de `.data` | 8 | R1 | V1 (objeto idéntico) |
| **P2b** variable de 1-2 B estripada | 2 | R1 | V2 (sin keep) |
| **P3** relleno de alineación del pool `.rodata` (cadena o `.align 3`) | 9 | R3 | contexto del DOL leído en los 9 |
| **P4** cadenas de tablas de nombres cuya tabla `.data` se estripó | 2 (+2) | R4 | recuentos = DWARF (4, 8, 9, 20, 11, 5) |
| **P5** literales de inlines de cabeceras que nuestro TU no incluye | 5 | R4 | DWARF: las `struct` están en el CU original |
| **P6** constantes de pool duplicadas o ausentes | 9 | R4/R3 | `roda.py` (valor contra `$LC` vivo/muerto) |
| **P7** nombres de asignación ausentes ("ECSTASY SphereMap"…) | 5 | R4 | ninguna en nuestro `.o` |
| **P8** colas y huecos sin candidato en el DWARF | 3 | R4 | `gap_06_8041B530`, `gap_06_8041AB90`, `gap_06_8041B520` |
| **P9** `.bss` de temporales `_.tmp_N` | 1 | R4 | control de referencia estática |
| **P10** alias `.set` absoluto | 1 | R3 | — |

Las filas no suman 92 porque varios bloques mezclan patrones; el veredicto de la tabla es por bloque.

## 4. Tabla bloque a bloque

"Medido" = entra en el paquete V6k. En ese paquete el DOL queda idéntico a la base (2.468 B,
secciones iguales) y el objeto conserva `.text` y `.rodata` idénticas. Los tipos vienen del
DWARF (`dwtype.py`) y los valores del DOL. En la medida, los `enum` se escribieron como `int`
(mismo tamaño); en la fuente final hay que usar el enum del DWARF. **keep** significa sustituir la
línea `zEcstasy.o:<etiqueta>` por una línea `zEcstasy.o:<nombre>` por variable.

| fichero:línea | clase | qué era en el original (fuente) | veredicto | C o paquete propuesto | test | riesgo |
|---|---|---|---|---|---|---|
| EcstasyE.cpp:1357 | DATOS .rodata 199 B | 12 cadenas EVIEW_NONE…EVIEW_UNKNOWN(?) dentro de lbl_803DE010 (refs=2 sólo a la cabeza); ninguna en nuestro .o; el CU no tiene una tabla `char*[12]` (roda.py, dwall) | R4 | — | medida: buscar en el DWARF la función o tabla de nombres de vista (dwrange sobre las DIE de EcstasyE.cpp) | — |
| EcstasyE.cpp:1525 | DATOS .rodata 1 B | relleno tras "WHITE16X16_NOALPHA\0" (19 B) antes de "%s_%s" (DOL 803DE104) | R3 | `.align 2` de la cadena siguiente; cae cuando las vecinas se emitan en su sitio | — | compensa pool |
| EcstasyE.cpp:172 | DATOS lbl_8041ABE4 8 B | efbWcrt=640, xfbWcrt=640: int globales estripadas (DWARF FFFFFFFF). El árbol las declara en .bss (EcstasyE.cpp:406-407) | R2 medido | `int efbWcrt = 640; int xfbWcrt = 640;` + borrar 406-407 + keep | V4: sin keep 99.597 B; con keep 2.468 | keep |
| EcstasyE.cpp:1749 | DATOS .rodata 4 B | 0.001f en lbl_803DE120+0x10 (pool tras "CARSELECTENVMAP", refs=8); nuestro .o no emite 0.001f | R4 | — | medida: reladdr/lcmap de qué instrucción carga 803DE130 | — |
| EcstasyE.cpp:202 | HUECO gap_06_8041AC18 4 B | MaxTotalFrameMallocSize (uint 0, estripada); FrameTimingStage0 (eFrameTiming 200 B) se va entera | R2 medido | `unsigned int MaxTotalFrameMallocSize = 0;` + keep | V5/V6k | keep |
| EcstasyE.cpp:2371 | HUECO gap_06_8041AD0A 2 B | relleno __sync_token (2 B) → EnableHarmonicClear; aFilters (eFilter[2]) entera | R1 medido | borrar el asm | V1: .data del .o idéntica; 2.468 | ninguno |
| EcstasyE.cpp:2385 | DATOS lbl_8041AD10 28 B | UVES_CS_start_clip (uchar 0), UVES_CS_end_clip (255), UVES_CS_R/G/B_modifier (float 0), RGBA_mod (GXColor {0,0,0,128}), UVES_color_shift_RGB_lerp_max (255), _A_lerp_max (255), UVES_color_shift_speed (0.1f); con 2+2 B de relleno | R2 medido | las 9 definiciones en ese orden + keep | V5 | keep |
| EcstasyE.cpp:243 | HUECO gap_06_8041AC70 8 B | AvgCpuTime, AvgGpuTime (float 0); MipMapMinMag, Player1/2ReflectionProjection, PrevCpu/GpuTime se van enteras | R2 medido | `float AvgCpuTime = 0.0f; float AvgGpuTime = 0.0f;` + keep | V5 | keep |
| EcstasyE.cpp:269 | DATOS lbl_8041AC98 8 B | PreviousDisplayBufferMode, DisplayBufferMode (enum EDISPLAY_MODE = 1) | R2 medido | dos definiciones `= 1` con el enum + keep | V5 (con int) | declarar el enum |
| EcstasyE.cpp:287 | DATOS lbl_8041ACAC 40 B | SetScreenIndirectTexture (0), …Off (0), RenderIndirectScreen, do_INTENSITY_REPLACE, do_INTENSITY_ACCUMULATE, do_SCREEN_QUAD_REPLACE, do_GlowBloomEffect (1), **ScreenTint float[3] → 4 B de cola = 0.8f**, ScreenFilterMin/Max (1.0f) | R3 | 36 B serían R2; la cola de ScreenTint no sale con `.data` a 4 | — | queda asm de 4 B |
| EcstasyE.cpp:323 | DATOS lbl_8041ACE8 4 B | EnableDebugMenuRendering (int 1) | R2 medido | `int EnableDebugMenuRendering = 1;` + keep | V5 | keep |
| EcstasyE.cpp:339 | HUECO gap_06_8041ACF8 8 B | TweakDumpWeatherInfo (bool 4 B, estripada) + `last_zone_hash` (estático local de eDisplayFrame, 0, con dirección en el DWARF) | R2 medido | `bool TweakDumpWeatherInfo = 0;` + `static int last_zone_hash = 0;` al principio de eDisplayFrame + keep (`last_zone_hash.NNNNN`) | V6k | nombre numerado |
| EcstasyE.cpp:3416 | DATOS .rodata 16 B | 225.0f, 20.0f (dup de $LC908 vivo), 415.0f, 84.0f: pool de lbl_803DE2A0 (refs=2) delante de DisplayRVMs; 225/415/84 no están en nuestro .o | R4 | — | medida: reladdr sobre DisplayRVMs | compensación |
| EcstasyE.cpp:3621 | HUECO gap_06_8041AD5A 2 B | relleno bEURGB60 (1 B) → _firstFrame | R1 medido | borrar | V1 | — |
| EcstasyE.cpp:3640 | HUECO gap_06_8041AD71 3 B | relleno e_bDither (1 B) → _defaultFIFO | R1 medido | borrar | V1 | — |
| EcstasyE.cpp:3656 | HUECO gap_06_8041AD7D 1 B | bHangRecovery (Bool 1 B, estripada) | R1 medido | `volatile Bool bHangRecovery = 0;` (sin keep) | V2: .data idéntica; 2.468 | la nota r62 (línea 369) la creía de .bss |
| EcstasyE.cpp:3671 | HUECO gap_06_8041AD82 2 B | load_sync (vu16, estripada) | R1 medido | `volatile unsigned short load_sync = 0;` | V2 | idem |
| EcstasyE.cpp:3685 | HUECO gap_06_8041AD86 2 B | relleno last_sync_token (2 B) → e_resync | R1 medido | borrar | V1 | — |
| EcstasyE.cpp:3700 | HUECO gap_06_8041AD90 4 B | e_recover (volatile int 0, estripada; el árbol la tiene en .bss en la 377) | R2 medido | `volatile int e_recover = 0;` + borrar 377 + keep | V5 | keep |
| EcstasyE.cpp:3728 | DATOS lbl_8041ADE8 8 B | 3 = nVerifyLevel (global estripada) + 0 = lastVerifyLevel (estático local con dirección). El DWARF pone también verifyLevels (GXWarningLevel[4]) en ADE8, incompatible con ADEC. Nada de esto está en el árbol | R4 | patrón V6k si se encuentra la función dueña | medida: DIE padre de verifyLevels/lastVerifyLevel (dwrange) | nombre numerado |
| EcstasyE.cpp:3745 | HUECO gap_06_8041ADF2 2 B | relleno bAlwaysCopyDisp (1 B) → e_retrace_count | R1 medido | borrar | V1 | — |
| EcstasyE.cpp:4160 | HUECO gap_06_8041AE02 2 B | relleno _enabled.31332 (1 B) → prevPFmt.31339 (entre funciones) | R1 medido | borrar el asm entero (lleva su `.section ".text"`) | V1 | — |
| EcstasyE.cpp:4399 | DATOS lbl_8041AE3A 26 B | 2 B de relleno + GlowBloomPasses (1), GlowBloomR/G/B (128), GlowBloomA (10), GlowBloomDistanceMultiplier (1.0f) | R2 medido | las 6 definiciones entre las dos funciones + keep | V5 | keep |
| EcstasyE.cpp:4607 | DATOS lbl_8041AE5C 8 B | SmearBiasBASE=2.0f, SmearBiasNOS=15.0f (estripadas; la nuestra tampoco las referencia, refs=0) | R2 medido | definirlas aquí + borrar zEcstasy.cpp:426-427 + keep | V3: sin keep 99.615; con keep 2.468 | la nota r59 ("SÍ se referencian") es falsa |
| EcstasyE.cpp:4641 | DATOS lbl_8041AF1C 16 B | SmearPrint (0), SmearScreenPrint (0), SmearScreenPrintX (30), SmearScreenPrintY (30) | R2 medido | 4 definiciones + keep | V5 | keep |
| EcstasyE.cpp:4686 | DATOS .rodata 28 B | 0, 1.0f, doble mágico, 65535.0f, 639.0f, 479.0f: pool de lbl_803DE45C delante de eMotionBlurEffect; 639/479 no están en nuestro .o | R4 | — | medida: reladdr sobre eMotionBlurEffect | compensación |
| EcstasyE.cpp:4947 | DATOS lbl_8041AF70 20 B | eMonochromeRedR/G/B/A (uchar A0,80,80,FF), MonochromeRedMinSpeed 0.1f, MaxSpeed 25.0f, MaxAlphaScale 1.0f, AlphaScale 0.0f | R2 medido | 8 definiciones + keep | V5 | keep |
| EcstasyE.cpp:4972 | DATOS lbl_8041AF88 28 B | 6 estáticos locales `once` (bool 1), dos por función en eProgressiveScan_EURGB60Proceed, eNTSCInterlace_PALProceed y eProgressiveScan_EURGB60DialogBox, + `fontShift` (estático local int 0). El original no los usa (esas funciones casan sin ellos) | R2 medido | `{ static bool once = true; }` ×2 al principio de cada función + `static int fontShift = 0;` (medido en eDEMOInitROMFont) + keep `once.NNNNN` ×6 y `fontShift.NNNNN` | V6 sin keep de locales: .data −0x20; V6k: 2.468 | nombres numerados; la función dueña de fontShift no está confirmada |
| EcstasyE.cpp:5011 | DATOS .rodata 80 B | pool (65535.0f, 0, doble mágico, 1.0f, 65535.0f) + "SKY_LAYER_BLUE", "SKY_LAYER_CLOUDS", "SKY_LAYER_OVERCAST" delante de eRenderSky; las cadenas no están en nuestro .o (SkyRender.hpp sólo tiene el enum) | R4 | — | medida: DWARF de la tabla de nombres de capa + marks.py | — |
| EcstasyE.cpp:516 | DATOS .rodata 668 B | nombres Attrib: emittergroup/emitterdata ($LC234/$LC233 muertos sin keep); aivehicle, pursuit*, pvehicle, engine, engineaudio, audiosystem, SMS_MESSAGE_*×4, visuallook ($LC muertos CON @lc, **duplicados en el enlace base**); "Attrib::Gen::speech", MGeneric, MAudioReflection, PlayerNum, Dist, Covered ("speech.h/MGeneric.h/MAudioReflection.h", no incluidas; las struct sí están en el DWARF del CU); "Assert: %s:%i (%s)"; dobles mágicos; ruta de EcstasyE.cpp (dup de $LC596); las 7 TARGET_ENVMAP* | R4 | paquete candidato: los includes en el punto del original, cadenas literales en EnvmapTargetNames, el `$LC` @lc emitido aquí y recorte del fragmento | medida: variante en espejo + marks.py de cada $LC | compensa con keep.lst |
| EcstasyE.cpp:5506 | DATOS .rodata 1 B | relleno tras "to 60 Hz mode.\0" | R3 | idem EcstasyE.cpp:1525 | — | compensa pool |
| EcstasyE.cpp:5643 | DATOS .rodata 1 B | relleno tras "to 50 Hz mode.\0" | R3 | idem | — | compensa pool |
| EcstasyE.cpp:5845 | DATOS .rodata 14 B | cola de "*\0" ($LC708 vivo) + "OS ROM FONT\0" (lbl_803DE8A8 refs=4); la cadena no está en nuestro .o | R4 | — | medida: reladdr sobre eDEMOInitROMFont | — |
| EcstasyE.cpp:6004 | DATOS .rodata 4 B | 0.0f suelto entre el doble mágico y 1.0f delante de eDEMOBeforeRender; 803DE920 ya alinea, así que no es relleno | R4 | — | medida: marks.py + reladdr; si es de la función, primer `x = 0.0f; x = f();` | — |
| EcstasyE.cpp:76 | ALIAS 0 B | `.set e_OSBusClock, 0x800000F8`: el original direcciona __OSBusClock (DWARF: variable global de este CU en 0x800000F8) con reubicación absoluta | R3 | GCC 2.95 no emite desde C una reubicación contra dirección fija (nota del árbol: AT_ADDRESS no lo hace); fuera de C sería un símbolo del ldscript | — | config |
| EcstasyEx.cpp:1015 | DATOS .rodata 32 B | 0, 42.6667f, 68.5714f, 0, doble mágico, 28.0 delante de cSphereMap(); 42.67/68.57 SÍ están vivos en nuestro .o ($LC721/$LC722) | R4 | — | medida: dolrod .rodata, dónde caen $LC721/$LC722 | duplicado |
| EcstasyEx.cpp:1169 | DATOS .rodata 20 B | "ECSTASY SphereMap\0" (nombre de asignación), no está en nuestro .o | R4 | — | medida: reladdr sobre cSphereMap::genSphere | — |
| EcstasyEx.cpp:145 | DATOS lbl_8041AFC0 16 B | GXBlendModeTweak=1, GXBlendFactorTweakSrc=4, GXBlendFactorTweakDst=5 (enum) + **cola de s_CaptureBufferAllocationString** (addend 0x1474 → "ECSTASY Specular"); BlendModeNames/BlendFactorNames enteras | R3 | 12 B R2; la cola de 4 B no sale con `.data` a 4 | — | queda asm de 4 B |
| EcstasyEx.cpp:1537 | HUECO gap_06_8041AFDF 1 B | relleno fog_blue → fog_enable | R1 medido | borrar | V1 | — |
| EcstasyEx.cpp:1574 | DATOS .rodata 12 B | "cReflectMap\0", no está en nuestro .o | R4 | — | medida: quién la usa en el DWARF | — |
| EcstasyEx.cpp:2615 | HUECO gap_06_8041B014 8 B | eTevSwapRasterizedColorInputIndex, eTevSwapTextureColorInputIndex (enum 0) | R2 medido | 2 definiciones + keep | V5 (con int) | enum |
| EcstasyEx.cpp:2633 | HUECO gap_06_8041B028 4 B | PrintTextureBuckets (int 0) | R2 medido | + keep | V5 | keep |
| EcstasyEx.cpp:2648 | DATOS lbl_8041B034 24 B | Fastfpow (1), DiffuseRGB/DiffuseA/SpecularRGB (128.0f), SpecularA (255.0f), EnableSpecular (1) | R2 medido | 6 definiciones + keep | V5 | keep |
| EcstasyEx.cpp:2776 | DATOS lbl_8041B090 12 B | cBfR (estático de fichero int 0, con dirección, sin uso) + TexAnimScrollType (enum 1) + PrintSolidViewLocalWorldCached (0) | R2 medido | `static int cBfR = 0; int TexAnimScrollType = 1; int PrintSolidViewLocalWorldCached = 0;` + keep (cBfR sin número) | V6k | keep |
| EcstasyEx.cpp:3900 | DATOS .rodata 52 B | 13 floats (0.4, 0.6667, 1.0, 0.25, 0.03126, 0.2857, 0.00259, 0): pool delante de InitSlotPoolsEx; salvo 0.4/1.0 no están en nuestro .o | R4 | — | medida: reladdr sobre las funciones previas | — |
| EcstasyEx.cpp:3953 | DATOS gap_06_8041B0AC 4 B + lbl_8041B0C8 8 B | B0AC: estático local SIN NOMBRE en el DWARF entre pPrevLocalWorld y position_table (función de EcstasyEx.cpp:3597); B0C8: g_tintscale_base 0.0f + g_tintscale_target 0.2f | R4 | B0C8 es R2 como el patrón | medida: dwrange de la DIE sin nombre (tipo, bloque) | — |
| EcstasyEx.cpp:3987 | DATOS lbl_8041B0F4 4 B | ScreenTintScale 0.1f (TweakContrastGain float[4] entera) | R2 medido | + keep | V5 | keep |
| EcstasyEx.cpp:4017 | DATOS lbl_8041B118 4 B | arn_Alpha 1.0f | R2 medido | + keep | V5 | keep |
| EcstasyEx.cpp:4035 | DATOS lbl_8041B130 4 B | hack_LightColourScale_InGame 1.0f | R2 medido | + keep | V5 | keep |
| EcstasyEx.cpp:4403 | DATOS .rodata 48 B | "ECSTASY ContrastSource", "ECSTASY ContrastRamp", no están en nuestro .o | R4 | — | medida: reladdr sobre eInitContrastSurface | — |
| EcstasyEx.cpp:483 | DATOS .rodata 412 B | cadenas de BlendModeNames[4] (GX_BM_*) y BlendFactorNames[8] (GX_BL_*), pool (0, 2.0, 1.0, −1.0, 0), cadenas de s_CaptureBufferAllocationString[9] (ECSTASY Nothing…Specular). Las tablas .data se estriparon; los recuentos casan con el DWARF | R4 | paquete candidato: definir las 3 tablas (tipos DWARF) en su orden de DIE, detrás de prevDst | medida: ¿nuestro enlace conserva las cadenas de una tabla estripada entera? + marks.py de "GX_BM_NONE" | — |
| EcstasyEx.cpp:86 | HUECO gap_06_8041AFB3 1 B | relleno _alphaRef → prevMode | R1 medido | borrar | V1 | — |
| eEnvMapE.cpp:315 | DATOS lbl_8041B400 8 B | DrawBlackAndWhite (1), ForceBlackAndWhite (0) | R2 medido | + keep | V5 | keep |
| eLightE.cpp:10 | DATOS lbl_8041B13C 8 B | HeadlightGlassDiffuseMinA 0.4f, MaxA 0.8f (eLspec bVector4[3] y WindowDiffuseMin/MaxA enteras) | R2 medido | + keep | V5 | keep |
| eLightE.cpp:487 | DATOS .rodata 4 B | 4E7E0000 = lbl_803DECCC (refs=2): constante del pool de eLightMaterialPlatInterface::UpdatePlatInfo; nuestro $LC821 vivo tiene el valor en otro sitio | R3 | cae cuando UpdatePlatInfo case (pin fr5 + barreras, vetada) | — | — |
| ePShader.cpp:1042 | DATOS lbl_8041B468 4 B | cola de LightModeNames (`char *const [11]`, addend 0x1A6C) | R3 | resto de cola `size & 7` | — | — |
| ePShader.cpp:49 | HUECO gap_06_8041B418 4 B | KColorWorldReflection {0,0,0,0} (estripada) | R2 medido | definirla aquí + borrar zEcstasy.cpp:428 + keep | V3 | keep |
| eStrip.cpp:18 | HUECO gap_06_8041B4A4 4 B | e_strip_verts (eStripVert*, 0; el árbol la tiene en .bss en la 31) | R2 medido | `eStripVert *e_strip_verts = 0;` + borrar 31 + keep | V5 | keep |
| eStrip.cpp:36 | DATOS lbl_8041B4B8 8 B | MemoryUsedByDuplicatedSolids (int 0, de eViewPlat.cpp por orden de DIE) + TestMWDesaturation (GXColor A0A0A0FF, de eVisualTreatment.cpp) | R2 medido | cada una en su fichero (detrás de ViewPlatInfoTable / delante de TestMWPulseBrightness) + keep | V5 | el comentario de eVisualTreatment.cpp:42 está mal |
| eVShader.cpp:19 | DATOS lbl_8041B49C 4 B | cola de vShaderNames (`char *const [5]`, addend 0x1ACC → "eVS_MAXNUM") | R3 | resto de cola | — | — |
| eViewPlat.cpp:27 | DATOS .rodata 908 B | cadenas de pShaderNames[20] (ePS_*), LightModeNames[11] (LM_*), "UNKNOWN", vShaderNames[5] (eVS_*), 4 Duplicated* (asignaciones que no tenemos), pool 0.5/1.0/0 | R4 | paquete candidato: definir las tablas en ePShader.cpp/eVShader.cpp | medida: las tablas están en la .data de ePShader/eVShader y sus cadenas caen delante de GetPlaneState, así que el punto de emisión no es la definición: medir con marks.py | — |
| eVisualTreatment.cpp:183 | DATOS .rodata 32 B | "ookeffect\0" (cola de "Attrib::Gen::visuallookeffect", de visuallookeffect.h) + "visual treatment\0"; nuestro enlace tiene "Attrib::Gen::visuallook" en 803DF1B0 | R4 | — | medida: dolrod .rodata | — |
| eVisualTreatment.cpp:44 | DATOS lbl_8041B4C4 12 B | TestMWColourTint {DC,FA,14,FF}, PrintRenderMWVisualLook 0, PulseBrightnessScale 128.0f | R2 medido | 3 definiciones + keep | V5 | keep |
| eVisualTreatment.cpp:66 | DATOS lbl_8041B4D8 24 B | `once` (estático local bool 1 de UpdateIndirectTexture, con dirección) + 20 B de ceros sin candidato en el DWARF (TweakBlack/ColourBloomMatrix y ParticleTexturesEnumArray se van enteras) | R4 | once = patrón V6k | medida: DIE de TexEnumGen/EmitterSystem entre B4DC y B4F0 | — |
| zEcstasy.cpp:223 | DATOS .rodata 128 B | "GAMECUBE" ($LC58 muerto sin keep; nuestro GCC lo emite DETRÁS del fragmento, en 0x84), "d:/mw/speed/indep/bware/inc/bware.hpp" (no la emitimos), "bad_alloc" ($LC59 muerto), "%f,%f,%f", "%f,%f,%f,%f", "STL", "High"/"Medium"/"Low"/"Reflection" (inlines de cabecera; otras unidades los escriben en asm o como switch muerto), 0.5f | R4 | paquete candidato: @lc de $LC58/$LC59 + recorte, sólo si GCC los emite en el byte 0 | medida: orden de $LC57..59 respecto a la cabecera de bWare | — |
| zEcstasy.cpp:335 | HUECO pad_07_8045EC40_bss 112 B | 14 `_.tmp_N` (int) del DWARF en 0x8045EC44+8k, DIE de nivel de CU detrás de los typedef SND stich (reflection_typedef_stich_collision), alternando con 4 B (resto estripado, .bss a 8) | R4 | control: una referencia estática de fichero a un temporal da `_.tmp_N` en .bss | medida: fichero de esas DIE (.debug_sfnames) + control con 14 referencias | cabecera ausente del árbol |
| zEcstasy.cpp:432 | HUECO gap_06_8041B530 156 B | ceros de cola de .data: el DWARF sólo explica 8 B (Galpha_value_to_kill_at, NoDrag); el resto compensa nuestro relleno de alineación (nota r58) | R4 | — | medida: aplicar V6k, recortar y mirar la dirección de zFe con desplaza | compensa |
| zEcstasy.cpp:535 | DATOS .rodata 4 B | 0.0f en lbl_803DF3D0+4 (pool de __static_initialization_and_destruction_0) | R4 | — | medida: reladdr sobre la función de inicialización | — |
| Ecstasy.cpp:51 | HUECO gap_06_8041A5C4 4 B | PrintFrameMalloc (uint 0) | R2 medido | + keep | V5 | keep |
| EmitterSystem.cpp:1282 | DATOS .rodata 4 B | 0 delante del doble 0x43300000_00000000 (803DF368, alineado a 8): relleno `.align 3` | R3 | cae cuando case el pool anterior | — | compensa pool |
| EmitterSystem.cpp:1680 | DATOS .rodata 4 B | 0 delante del doble mágico (803DF380) | R3 | idem | — | idem |
| EmitterSystem.cpp:1707 | HUECO gap_06_8041B520 8 B | ceros entre warn_once.33364 y random_seed.33848 (estáticos locales); no hay DIE que lo explique | R4 | — | medida: dwrange de SetEmitters/CreateEmitterGroup/GetEmitterGroupsToTrigger | — |
| EmitterSystem.cpp:257 | DATOS .rodata 3 B | relleno tras "ParticleSlotPool\0" | R3 | `.align 2` de cadena | — | compensa pool |
| EmitterSystem.cpp:288 | DATOS .rodata 3 B | relleno tras "EmitterGroupSlotPool\0" | R3 | idem | — | idem |
| EmitterSystem.cpp:57 | DATOS .rodata 36 B | "Attrib::Gen::emitteruv" (emitteruv.h, no en nuestro .o) + "GManager" ($LC400 @lc, **duplicado en el enlace base**) | R4 | paquete candidato: include de emitteruv.h + mover $LC400 | medida: variante en espejo + marks.py | compensa con keep.lst |
| EmitterSystem.cpp:594 | DATOS .rodata 4 B | 0 delante del doble en 803DF2F0 | R3 | `.align 3` | — | compensa pool |
| Texture.cpp:59 | DATOS .rodata 16 B | 10.0f, 4.0f, 1.0f, 0 delante de eInitTextures; los tres están vivos en nuestro .o en otro sitio | R4 | — | medida: dolrod .rodata | duplicado |
| Texture.cpp:613 | DATOS .rodata 29 B | cola de "<null>", "TextureInfo\0", "TextureInfoPlatInfo" (nombres de pool), no están en nuestro .o | R4 | — | medida: reladdr sobre eInitTextures/UpdateTextureAnimations | — |
| eLight.cpp:1048 | HUECO gap_06_8041AB58 12 B | LFrot (int 0), LFangle (ushort 0) + 2 B de relleno, Rotty (int 0) | R2 medido | 3 definiciones + keep | V5 | la nota eLight.cpp:227 ("NO existen") es falsa |
| eLight.cpp:1068 | HUECO gap_06_8041AB70 4 B | CurrentStuffView (eView*, 0; por orden de DIE es de eView.cpp); backupShaperRig (136 B) entera | R2 medido | `eView *CurrentStuffView = 0;` detrás de `eView eViews[22];` en eView.cpp + keep | V5 | keep |
| eLight.cpp:113 | DATOS lbl_8041A688 20 B | PrimaryLightDiffuseIntensity 6.5f, [FillLightDiffuseColour Vector4 entera], FillLightDiffuseIntensity 2.1f, ExposureK 1.0f, MaxLum 2.0f, RyanSaysLightDump 0 | R2 medido | 5 definiciones + keep | V5 | keep |
| eLight.cpp:135 | DATOS lbl_8041A6A8 8 B | colas de BackupHeadlight y BackupBrakelight (eLightFlareParams 44 B → 4 B cada una: 0 y 2) | R3 | restos de cola | — | — |
| eLight.cpp:49 | DATOS lbl_8041A64C 12 B | RefAmbMod 50.0f, AmbientLightScale 0.0f, Light0Distance 10000.0f | R2 medido | 3 definiciones + keep | V5 | keep |
| eLight.cpp:536 | DATOS .rodata 21 B | cola de "ON " (LightStateStrings[1]) + 0 + 1.0f + doble mágico: pool delante de SphericalToCartesian | R4 | — | medida: reladdr/marks | — |
| eLight.cpp:67 | DATOS lbl_8041A65C + gap_06_8041A661 16 B | LightMaterialID (enum eLightMaterialID = 0x2C420D64), CopyLightMaterial_FE_to_IG, _IG_to_FE, CopyAllFEToIG (int 0) | R2 medido | 4 definiciones + keep (2 líneas → 4) | V5 (con int) | enum |
| eLight.cpp:885 | DATOS .rodata 32 B | "Gen::light_flares_cg" (cola de "Attrib::Gen::light_flares_cg" = nuestro $LC448 muerto sin keep) + 2 palabras de lbl_803DDAE4 (tabla de 0x58 B, refs=2) | R4 | paquete candidato: @lc light_flares_cg + recorte (la r63 midió +0x60 al añadir los 5 keep SIN recortar) | medida: offset de $LC448 respecto al fragmento en el .o | — |
| eLight.cpp:97 | HUECO gap_06_8041A670 8 B | LoadedShaperLightRigs (eShaperLightRig*, 0), NumberOfLoadedShaperLightRigs (0) | R2 medido | + keep | V5 | keep |
| eSolid.cpp:36 | HUECO gap_06_8041A624 4 B | VerifyFastMethod (int 0) | R2 medido | + keep | V5 | keep |
| eSolid.cpp:50 | DATOS lbl_8041A62C 16 B | 8 B de ceros sin candidato + colas de LightTypeStrings (0x394 "EXCLUDE  ") y LightAttenuationStrings (0x3C0 "INVSQR"); LightShapeStrings/LightStateStrings enteras | R3 | colas; los 8 B iniciales son R4 | — | — |
| eView.cpp:20 | DATOS .rodata 256 B | "Attrib::Gen::effects"/"audioimpact"/"audioscrape" (effects.h…; esas struct están en el DWARF del CU), WorldBodyConn, Pkt_Body_Open/Service/Send, World_UpdateBody, World_OneShotEffect, Pkt_Effect_Send/Open/Service, WorldEffectConn; nada en nuestro .o | R4 | paquete candidato: los includes del original antes de eView::eView (un inline muerto emite su literal) | medida: variante en espejo + marks.py | — |
| Sun.cpp:178 | DATOS .rodata 1 B | relleno tras "SUNSET\0" | R3 | `.align 2` | — | compensa pool |
| Sun.cpp:18 | HUECO gap_06_8041AB90 76 B | ceros entre SunInfo (Sun.cpp) y pVisualTreatmentPlat (EcstasyE.cpp, pasando por DefragFixer.cpp); ni DIE estripada ni con dirección | R4 | — | medida: DWARF de DefragFixer.cpp/Sun.cpp y dw.py tras recortar (¿compensación?) | compensa |

## 5. Dudas (R4) y la medida que las decide

1. **Qué subió a 8 la alineación de la `.data` del objeto original.** Esa es la llave de los 6
   R3 de cola y la que permitiría **quitar las 45 líneas de keep**. Medida: variante V6k con un
   objeto de alineación 8 al principio de la `.data` de la unidad. Con eso, ¿salen solas las
   colas de `vShaderNames`/`LightModeNames` y sobran las entradas de keep? Buscar además en el
   DWARF de todo el ELF un tipo de `.data` de zEcstasy con alineación 8.
2. **Cadenas de tablas estripadas** (EcstasyEx.cpp:483, eViewPlat.cpp:27): ¿conserva nuestro
   enlazador las cadenas cuando se estripa la tabla entera? ¿En qué punto las emite GCC?
   Variante en espejo con las tablas + `marks.py`.
3. **Literales de cabeceras no incluidas** (EcstasyE.cpp:516, eView.cpp:20, EmitterSystem.cpp:57,
   eVisualTreatment.cpp:183, zEcstasy.cpp:223): variante con los includes (speech.h, effects.h,
   audioimpact.h, audioscrape.h, emitteruv.h, MGeneric.h, MAudioReflection.h) y la posición de
   cada `$LC`. Hay que tratarlos **junto con las 4 entradas `@lc` duplicadas**.
4. **Pools de funciones** (EcstasyE.cpp:1749/3416/4686/6004, EcstasyEx.cpp:1015/3900,
   Texture.cpp:59, eLight.cpp:536, zEcstasy.cpp:535): `reladdr.py`/`lcmap.py` en la instrucción
   que carga cada dirección.
5. **Estáticos sin dueño** (lbl_8041ADE8, gap_06_8041B0AC, lbl_8041B4D8, gap_06_8041B520):
   `dwrange.py` sobre la DIE padre.
6. **pad_07_8045EC40_bss**: localizar la cabecera de SND stich por `.debug_sfnames`.
7. **Colas de compensación** (gap_06_8041B530 156 B, gap_06_8041AB90 76 B): aplicar V6k y medir
   con `desplaza`.

## 6. Reproducir (scratchpad/ecsdat68)

```
python scratchpad/ecsdat68/blocks.py                 # blocks.json del lote
python scratchpad/ecsdat68/genV5.py && python scratchpad/ecsdat68/genV6.py   # spec_V6.json + keep_V6.lst
python scratchpad/ecsdat68/mkvar2.py V6 scratchpad/ecsdat68/spec_V6.json     # espejo con uniones
python scratchpad/ecsdat68/cc3.py V6                 # compila con cwd en el espejo
# keep_V6k = keep_V6.lst + zEcstasy.o:last_zone_hash.24682, once.24919, once.24920, once.24927,
#            once.24928, once.24935, once.24936, fontShift.24940, cBfR
python scratchpad/ecsdat68/dw.py scratchpad/ecsdat68/o/V6.o scratchpad/ecsdat68/keep_V6k.lst
```

Control del espejo: la variante sin cambios reproduce los sha por sección de la base
(`.rodata` dabe8f8c5226, `.data` b49c6d74b962, `.text` 9a5c5191aac5), y el enlace da 2.468 B.
Control negativo: un error de sintaxis en la copia falla la compilación.

Herramientas nuevas: `dwgap.py` (variables del CU en orden de DIE con las estripadas),
`dwtype.py` (tipo DWARF-1), `datahole.py` (hueco contra estripadas), `roda.py` (fragmento
`.rodata` contra `$LC` vivo/muerto), `dwrange.py` (DIE por offset), `mkvar2.py`/`cc3.py`/`dw.py`.
Al portarlas: las constantes DWARF-1 buenas son `AT_mod_u_d_type = 0x0083` y
`AT_subscr_data = 0x00A3` (no 0x73/0xA8).
