# r68 — exploración de los parches de datos, lote 4

zWorld/zWorld2 y zLua (pendientes), zFEng, zFeOverlay, zAttribSys y seis bibliotecas
(promocionadas). **82 bloques.** Ronda de solo lectura: nada en `src/`, `config/` ni `build/`.
Cada hipótesis que se da por validada se compiló desde una **copia** del árbol
(`scratchpad/lote4_68/tree`), con los cflags reales de la unidad, y se enlazó a un temporal con
`ngcld -strip-unused-data` y una **copia** de `keep.lst` cuando hacía falta.

## Resumen

| veredicto | bloques | de ellos validados por enlace |
|---|---:|---|
| R1 | 3 | 3 (DOL `9619ba57`) |
| R2 | 54 | 18 enteros + 2 parciales (DOL `9619ba57`, o hash de pendiente idéntico al de la base) |
| R3 | 2 | la regla que los hace R3 está medida |
| R4 | 23 | 3 con una parte R2 validada |

Controles: `mylink.py` sin sustituir nada da `9619ba57`; la copia del árbol compila zAttribSys,
zWorld y zLua a los mismos objetos que `build/` (`zWorld` 23.513 B y `zLua` 1.667 B en el enlace,
hashes `2494248b…` y `27c00e55…`, iguales a los de la r67); zWorld2 ya tiene las secciones
descuadradas en la base (.sdata +0x40) y se mide sobre las secciones que cuadran (174.711 B).

## Reglas nuevas, medidas

1. **En `.data` nuestro enlazador estripa ENTERO el símbolo muerto, mida lo que mida.** zAttribSys
   con el hueco de 4 B escrito como estática de C (`static unsigned int gDefaultExportID = 0;`),
   como global, como array de 12 B y como array global de 20 B: los cuatro dan el mismo DOL roto
   (3.104 B), con la `.data` del objeto byte a byte igual a la base. **`size & ~7` no rige en
   `.data`.** El original sí dejó colas: `lbl_804372DC` (0x10) es el último elemento de
   `CarLightsStateMap[5] = {1,2,4,8,16}` (20 B, DWARF `address: 0xFFFFFFFF`). Consecuencia: todo
   dato muerto de 4 B es **C + entrada de keep.lst con su nombre real**; una cola de un array mayor
   no tiene forma C (R3).
2. **En `.rodata` sí rige `size & ~7` para un objeto con nombre.** `extern const long
   FEKeyInterpDefault[7] = {0,1,1,1,1,1,1};` (28 B, muerto, sin keep.lst) deja su cola de 4 B
   (`0x00000001`) donde el original: DOL OK.
3. **El volcado DWARF distingue los dos muertos.** Global estripada → `address: 0xFFFFFFFF`.
   Estática (de fichero o de función) muerta → **conserva la dirección** (reubicación relativa a
   sección) y no tiene símbolo en el ELF. Cruzado con el symtab de `NFSMWRELEASE.ELF`, cada hueco de
   `.data` sale con nombre (`corpse.py`). Confirmado en 17 huecos.
4. **GCC 2.95 emite las estáticas muertas con inicializador**, de fichero y de función, incluso
   en un cuerpo vacío (`void Rain::UpdateAndRender() { static float ChangeWeatherTime = 0.0f; }`),
   en su punto de definición. Varias de zWorld **ya estaban en C** (`windState`, `changetime`,
   `index`, `testDamageLevel`, `testDelay`, `CarLightAngles`, `lastlight`, las tres estáticas de
   clase de zWorld2) y el asm las **duplicaba**: se estripaban por no tener keep.lst.
5. **keep.lst acepta nombres locales** (`_6Attrib.gDefaultExportID`, `gCarTypeNameHash`), pero el
   de una estática de función lleva el contador del compilador (`index.25651`, `fc.23738`,
   `ChangeWeatherTime.25684`), que **cambia con cualquier edición de la TU**. Hace falta un
   resolvedor tipo `@lc` para esos nombres (lcfix no lo cubre): es la herramienta que falta para
   6 de los bloques validados.
6. **`#line N "d:/mw/speed/indep/bware/inc/bware.hpp"` hace que cc1plus emita `__FILE__` con la
   ruta de EA** exacta (mini-TU con los cflags de zFEng: `.rodata` = la ruta + relleno). El
   prefijo de bWare/STL deja de ser «imposible desde el árbol».
7. **El hueco de 4 B al final de la `.data` de zFeOverlay es relleno del enlazador**: quitando el
   asm (con o sin su entrada de keep.lst) el DOL sigue `9619ba57`.
8. **Los alias `gcc2_compiled._<dir>` apuntan a la primera función del fichero** (`fflush`,
   `ClearSysState_C`, misma dirección en symbols.txt). Llamar a la función por su nombre da el
   mismo DOL, y el alias de `fflush.c` no lo referencia ya nadie del enlace.
9. **Un `static const` solo en `.rodata` sale en su sitio**: `maddeca.cpp` con
   `static const int zigzag[64] = {…}` da DOL OK; la nota «externos antes que estáticos» no aplica
   cuando es el único dato.
10. **Los inicios de `.data` de `.debug_aranges` no sirven para trocear colas**: salen 32 B por
    debajo de splits.txt en zAttribSys y zFEng, e incoherentes en zLua. Para `.data` manda el DWARF
    de la unidad vecina: la cola de 76 B de zFEng contiene `gQuatCompMask` de **zFoundation**
    (0x8041D198), o sea que la frontera zFEng/zFoundation está en 0x8041D198 y no en 0x8041D1C0.
11. **Los `.asciz` de zWorld están mal y se compensan entre sí.** El original tiene esas cadenas
    alineadas a 4; los siete bloques localizados escriben 2.396 B donde el original ocupa 2.520
    (−124 B). La cola `zWorld.cpp:191` (402 B, cadenas del original fuera de su sitio + 50 B
    duplicados de `lbl_80409D58`) cuadra el tamaño. Medido: alinear sólo CarRender descuadra
    (+0x20); alinear los siete y recortar 133 B de la cola cuadra las secciones pero da 24.297 B
    (+784 sobre la base). **Ningún `.asciz` compensa a CarRender en particular: comparten el mismo
    error, y la compensación es la cola.**
12. **`.bss`/`.sbss` sin COMMON desde C: `__attribute__((section(".bss")))` (o `-fno-common`) y el
    orden lo fija la PRIMERA declaración**, también en C (vputil.c): con `static int FData` la
    estática sale detrás de las externas (22 B); global y sin su `extern` previo, también detrás;
    global CON el `extern int FData[88];` que ya va delante de los demás, sale primera y el DOL
    queda `9619ba57`.

## Patrones

| patrón | bloques | casos comprobados |
|---|---:|---|
| A. estática/global muerta de `.data` (DWARF) → C + keep.lst con el nombre real | 17 (+3 parciales) | 17 validados: zAttribSys (2), zFeOverlay (3), zWorld (10), zWorld2 (parcial), zFeOverlay tail (R1) |
| B. cadena muerta del pool → primer `static inline` + `# @lc` | 25 | `LuaBindery` (zLua): 88 `@lc` renumeradas por contenido, enlace idéntico a la base |
| C. prefijo bWare/STL + vocabulario de cabecera escrito verbatim | 7 | `#line` medido en mini-TU; `lbl_80409D58`/`lbl_8040DFE8` duplican 436/311 B que el objeto ya emite como `$LC` muertos |
| D. `.asciz` de zWorld sin alinear + cola de compensación | 12 | audit de los 12 contra el DOL; W1/W1b/W2 medidos |
| E. pool de una función estripada (flotantes/dobles sueltos) | 9 | DWARF: `Range: 0xFFFFFFFF` en FECodeListBox/FEKeyTrack/…; sin reconstruir |
| F. ceros sin DWARF (colas de unidad, huecos de zLua) | 10 | ninguna variable con dirección ni `FFFFFFFF` que encaje |
| G. artefactos del troceador (alias, comodín, frontera) | 5 | fflush, vp6 (uopt+vputil), gc_interface, maddeca validados |

## Tabla bloque a bloque

Test: «DOL OK» = relink con el objeto variante y `9619ba57`; «igual» = unidad pendiente,
`mylink` da el mismo hash/total que la base (zWorld `2494248b` 23.513 B, zLua `27c00e55` 1.667 B).

| fichero:linea | clase | qué era en el original (fuente) | veredicto | C o paquete propuesto | test | riesgo |
|---|---|---|---|---|---|---|
| Packages/realmemcard/…/gc/gc_interface.cpp:55 | DATOS 4B | `const char *Realmc::FILENAME_ALL_FILES` (DWARF CU gc_interface, .sdata 0x804FF68C) → `"*"` del pool propio de gc_interface: aranges pone su `.rodata` en 0x804144C0 y el comodín `auto_05_804144C0_rodata.o` (112 B) son sus `$LC` (`"s"…"ssdd"`, `"*"`, formato del `sprintf`) | R2 **validado** | `const char *FILENAME_ALL_FILES = "*";`; quitar `extern "C" lbl_804144C0/E0` y `REALMC_LCFMT_BASE`; literal en el `sprintf`; **sacar `auto_05_804144C0_rodata.o` del enlace** (rango `.rodata` de gc_interface desde 0x804144C0) | DOL OK con `--drop=auto_05_804144c0`; sin quitar el comodín, 57.989 B | toca splits/configure; hay DOS `gc_interface.o` (realcore), ojo con keep.lst por nombre de objeto; las cinco LC_msg miden <8 B y sobreviven sin keep |
| Packages/vp6/…/cmn/vputil.c:47 | DATOS 920B | etiqueta `gcc2_compiled._8034E180` = dirección de `ClearSysState_C` (symbols.txt) que uoptsystemdependant importa por ese nombre; `FData` (local 352 B .bss), `idct`/`idctc` (260 B .bss) y 12 punteros en .sbss | R2 **validado** | etiqueta: `extern void ClearSysState_C(void);` y `ClearSysState = (VP6_FUNC)ClearSysState_C;` en uoptsystemdependant.c, fuera del asm. Reservas: conservar `extern int FData[88];` (línea 24, delante de los `extern` de `idct`/`idctc`) y definir `int FData[88] __attribute__((section(".bss")));`, `void (*idct[65])(INT16 *, INT16 *, INT16 *) __attribute__((section(".bss")));`, ídem `idctc`, y los 12 punteros con `__attribute__((section(".sbss")))` en el orden de hoy (precedente: dering.c) | DOL OK (V5, las dos unidades). Con `static int FData` o sin su `extern` previo, FData sale la última: 22 B | `FData` queda GLOBAL (el original la tiene local): el DOL no lo ve, el symtab sí; con `-fno-common` en vez del atributo sale igual |
| Speed/Indep/Libs/realcore/…/systemvars.cpp:16 | HUECO 8B | DWARF: 0x804FF650 `static void (*orginal_vbltmrint)(unsigned long)` y 0x804FF654 `static int bIsVBLTimerInited`, de **initvblt.cpp** (su .sdata quedó dentro del rango de systemvars); detrás, `gFontDriver`/`sAllocator` también son de otras unidades | R2 | mover las dos estáticas a initvblt.cpp (+keep) y `gFontDriver`/`sAllocator` a sus dueños, en orden de enlace | sin probar: relink | paquete de 3-4 unidades; base.cpp/fontcreate.cpp llegan por reubicación |
| SourceLists/zAttribSys.cpp:4 | DATOS 896B | prefijo bWare/STL (92 B) + 38 cadenas muertas de AttribSys (`Attrib::Attribute`…`AttribSys.gamecube`) con los nombres `$LC2151…` de dtk, protegidas por nombre en keep.lst | R2 | patrón C: `#line` en la cabecera que usa `__FILE__` + las cadenas por su cabecera/primer en orden + 44 entradas de keep por `# @lc` | sin probar | ZATTRIBSYS_HAND_POOL y `_zAttribSysGAMECUBE` dependen del bloque; renumera todos los `$LC` (lcfix) |
| SourceLists/zAttribSys.cpp:536 | HUECO 4B | `static unsigned int Attrib::gDefaultExportID` (DWARF L242044, 0x804158E4), muerta, sin símbolo en el ELF | R2 **validado** | `namespace Attrib { static unsigned int gDefaultExportID = 0; }` + keep `zAttribSys.o:_6Attrib.gDefaultExportID` en lugar de `gap_06_804158E4_data` | DOL OK; sin keep, 3.104 B | ninguno medido |
| SourceLists/zAttribSys.cpp:569 | HUECO 44B | `static void (*gEditNotifier)(const Collection*,unsigned)` y `static void (*gMessageSender)(const char*)` (DWARF 0x8041590C/10) + 36 B de ceros sin DWARF | R4 (8 B R2 **validado**) | las dos estáticas + keep `_6Attrib.gEditNotifier`,`_6Attrib.gMessageSender`; 36 B siguen anónimos | DOL OK (con X1 a la vez) | los 36 B: medida = DWARF de zBWare no los reclama (su primera variable está en 0x80415938); buscar muertas `FFFFFFFF` de 36+8k B en las TUs que enlazan ahí |
| SourceLists/zFEng.cpp:23 | HUECO 92B | prefijo bWare/STL | R2 | patrón C | sin probar | 31 SourceLists comparten el prefijo |
| SourceLists/zFEng.cpp:54 | HUECO 4B | `0x00000001` en 0x8041D078 entre `FECodeListBox::mpDefaultCallback` y `FEngine::SysGUID`; ninguna variable del DWARF de zFEng (11 `FFFFFFFF`) encaja | R4 | — | medida: listas `FFFFFFFF` de las CU vecinas / volcado PS2 buscando un `int = 1` muerto de FEButtonMap/FECodeListBox | — |
| SourceLists/zFEng.cpp:92 | HUECO 76B | 36 B de ceros al final de zFEng + 40 B que son **zFoundation**: DWARF `static unsigned int gQuatCompMask` en 0x8041D198 (=0xFFFFFFFF), `seedarray` en 0x8041D1AC | R4 (parte R2) | mover los 40 B a la cabeza de la `.data` de zFoundation (`gQuatCompMask` es de UBitPack.cpp, hoy sin incluir) | sin probar | cambia la frontera zFEng/zFoundation (regla «el rango no basta») |
| Src/FEng/FECodeListBox.cpp:180 | HUECO 20B | pool muerto (0, 0x4330000000000000, 0x4330000080000000): conversión int→float de una función estripada; el DWARF lista FECodeListBox::SetCellType/SetAllScale/… con `Range: 0xFFFFFFFF` | R4 | cuerpo de la función estripada vecina | medida: identificar cuál (plan.py/PS2) y compilar su cuerpo; el enlazador tira el `.text` y conserva el pool (r36-bw) | — |
| Src/FEng/FEKeyInterpLinear.cpp:270 | HUECO 36B | pool muerto de flotantes (0, 1.0, …, 1e-5) tras FELerpColor | R4 | ídem | ídem | — |
| Src/FEng/FEKeyTypes.cpp:21 | HUECO 56B | 4 B = cola de `long FEKeyInterpDefault[7]` (DWARF `FFFFFFFF`, 28 B); 52 B = `"Scroll Left/Right/Up/Down"` | R2 (4 B R1 **validado**) | `extern const long FEKeyInterpDefault[7]; const long FEKeyInterpDefault[7] = {0,1,1,1,1,1,1};` (sin keep) + las cuatro cadenas por primer + `@lc` | DOL OK (la parte de 4 B) | el comentario del fichero dice que el objetivo no la emite: sí emite su cola |
| Src/FEng/FEListBox.cpp:138 | HUECO 12B | pool muerto de ceros (SetNumRows) | R4 | cuerpo estripado | ídem FECodeListBox | — |
| Src/FEng/FEListBox.cpp:30 | HUECO 16B | pool muerto 0 / 1.0 / −1/720 / 1.0 (constructor) → constructor de copia estripado | R4 | cuerpo estripado | ídem | — |
| Src/FEng/FEListBox.cpp:456 | HUECO 232B | cadenas del volcado de lista (`[HEADER]`, `Languages`, `[LABEL]`, `Major version: 3`…) + 3 flotantes | R2 | cadenas por primer + `@lc`; flotantes con la asignación pisada (r67b) o el cuerpo estripado | sin probar | renumera `$LC` |
| Src/FEng/FEObject.cpp:232 | HUECO 8B | pool muerto 0 / 1e-6 | R4 | cuerpo estripado | ídem | — |
| Src/FEng/FEObject.cpp:356 | HUECO 8B | pool muerto 0 / 1e-6 | R4 | cuerpo estripado | ídem | — |
| Src/FEng/FEPackage.cpp:425 | HUECO 44B | `__FILE__` de EA `d:/mw/speed/indep/src/feng/FEPackage.cpp` | R2 | `#line` (regla 6) + la función que usa `__FILE__` o primer + `@lc` | sin probar | `#line` mueve `__LINE__` si no se pone el número real |
| Src/FEng/FEScript.cpp:19 | HUECO 12B | pool muerto 0 / 0x4330000080000000 (lbl_803EAC3C) | R4 | cuerpo estripado | ídem | — |
| Src/FEng/FETypeLib.cpp:178 | HUECO 114B | cadenas (`Init`, `Hide`, `cmod`, `cset`, `cseta`, `cadd`, `wa`, `sr`…) + flotantes de pool | R2 | primer + `@lc`; flotantes aparte | sin probar | — |
| Src/FEng/FEngStandard.cpp:33 | HUECO 37B | `"FEngMalloc"`, `"FEngMalloc[somefile]"` | R2 | primer + `@lc` | sin probar | — |
| Src/FEng/FEngine.cpp:818 | HUECO 84B | nombres de ámbito de perfilado (`FEngine::Render()`, `Setup`, `Object traversal`…) | R2 | primer + `@lc` | sin probar | — |
| SourceLists/zFeOverlay.cpp:151 | HUECO 4B | relleno de alineación del enlazador tras `MarkerSelectInfos` | R1 **validado** | borrar el asm (y la línea 812 de keep.lst, caduca) | DOL OK con y sin la entrada | ninguno |
| SourceLists/zFeOverlay.cpp:26 | DATOS 92B | prefijo bWare/STL (`.over`) | R2 | patrón C | sin probar | keep.lst lo nombra en minúsculas |
| Src/Frontend/…/FEPkg_GarageMain.cpp:67 | DATOS 1068B | 56 cadenas de cabecera (etiquetas AttribSys, `Attrib::Gen::*`, WorldConn/SoundConn, versiones, `SMS_MESSAGE_%d*`) que nuestra TU no emite (dup4: 1/1 cada una, sólo el asm); `_feov_lc_19_8_31` lo usa el código como el literal `"19.8.31"` | R2 | los `#include`/primers en el orden del objetivo + `@lc`; el uso de `_feov_lc_19_8_31` pasa a literal | sin probar | las `@lc` de zFeOverlay en keep.lst ya nombran varias de estas cadenas: revisar con lcfix antes |
| Src/Frontend/…/DebugCarCustomize.cpp:19 | HUECO 4B | `static unsigned int gCarTypeNameHash` (DWARF L2512544, 0x80439234) | R2 **validado** | `static uint32 gCarTypeNameHash = 0;` aquí, **quitando** `static uint32 gCarTypeNameHash;` de CarCustomize.hpp:712 + keep `zFeOverlay.o:gCarTypeNameHash` | DOL OK | la cabecera la incluyen otras unidades (sin usos) |
| Src/Frontend/…/DebugCarCustomize.cpp:32 | DATOS 4B | `static int gInstallCarPartID = -1` (L2512546, 0x8043923C) | R2 **validado** | `static int gInstallCarPartID = -1;` + keep `zFeOverlay.o:gInstallCarPartID` | DOL OK | — |
| Src/Frontend/…/uiQRCarSelect.cpp:35 | HUECO 4B | `bool QRCarSelectBustedManager::bIsCross` (DWARF `FFFFFFFF`, declarada en uiQRCarSelect.hpp:81) | R2 **validado** | `bool QRCarSelectBustedManager::bIsCross = false;` + keep `_24QRCarSelectBustedManager.bIsCross` | DOL OK | — |
| Src/Frontend/…/uiQRCarSelect.cpp:426 | HUECO 4B | 0.25f muerto entre los pools de CommitChangeStartRace y NotificationMessage | R4 | — | medida: función con `Range: 0xFFFFFFFF` entre las dos en el DWARF de zFeOverlay | — |
| SourceLists/zLua.cpp:108 | HUECO 25B | `"Lua emergency allocation"` (muerta en el árbol) | R2 | primer + `@lc` (mecanismo validado con LuaBindery) | sin probar | renumera `$LC` |
| SourceLists/zLua.cpp:118 | HUECO 148B | `"LuaRuntime temp compression buffer"`, `"LuaRuntime VM reset snapshot"`, nombres de tipo de Lua, `"(?)"`, `", "`, `" "`, `"="`, `"\""` | R2 | primer + `@lc` | sin probar | ídem |
| SourceLists/zLua.cpp:163 | HUECO 12B | tras `LuaRuntime::mObj`; DWARF: `bool LuaRuntime::mPrintAlloc` `FFFFFFFF` declarada justo después (4 B); 8 B sin nada | R4 (4 B R2) | `bool LuaRuntime::mPrintAlloc = false;` + keep; 8 B anónimos | sin probar | — |
| SourceLists/zLua.cpp:176 | HUECO 14B | `"LuaPostOffice"` | R2 | primer + `@lc` | sin probar | — |
| SourceLists/zLua.cpp:183 | HUECO 8B | ceros tras `LuaPostOffice::fObj`; nada en el DWARF | R4 | — | medida: cruzar con el volcado PS2 (orden de estáticas de LuaPostOffice/LuaBindery) | — |
| SourceLists/zLua.cpp:291 | HUECO 108B | ceros entre `accessorTable` y `flagMapping`; nada en el DWARF | R4 | — | ídem | — |
| SourceLists/zLua.cpp:315 | HUECO 4B | ceros entre `kPrintScriptMessages` y `sRealloc`; nada | R4 | — | ídem | — |
| SourceLists/zLua.cpp:347 | HUECO 160B | nombres de tipo del asignador de depuración de EA (`CHAR`, `INT`, `STATE`… `INVALID`) | R2 | primer + `@lc` | sin probar | — |
| SourceLists/zLua.cpp:390 | HUECO 192B | ceros al final de la `.data`; zMain empieza en 0x8041D7EC | R4 | — | ídem | — |
| SourceLists/zLua.cpp:421 | HUECO 16B | **no es dato del original**: `.space 16` de compensación (8 B son el `bf800000` que falta en 0x803EEA48) | R3 | se retira cuando se coloque ese pool | — | quitarlo sin arreglar el déficit corre `.data`/`.bss` |
| SourceLists/zLua.cpp:71 | HUECO 9B | `"MGeneric"` | R2 | primer + `@lc` | sin probar | — |
| SourceLists/zLua.cpp:9 | HUECO 92B | prefijo bWare/STL | R2 | patrón C | sin probar | `BWARE_PREFIX_*` apuntan dentro del bloque |
| SourceLists/zLua.cpp:92 | HUECO 28B | `"Attrib::Gen::milestonetypes"` (el `$LC` de su cabecera es muerto: cadáver `"pes"`) | R2 | el `#include` natural + `@lc` | sin probar | — |
| SourceLists/zLua.cpp:96 | HUECO 34B | `"LuaRuntime memory pool"`, `"LuaRuntime"` | R2 | primer + `@lc` | sin probar | — |
| Src/Lua/LuaAttributes.cpp:266 | HUECO 14B | `"LuaAttributes"` | R2 | primer + `@lc` | sin probar | — |
| Src/Lua/LuaBindery.cpp:152 | HUECO 11B | `"LuaBindery"` | R2 **validado** | `static inline const char *_zLuaPoolBindery(int n) { switch (n) { case 0: return "LuaBindery"; } return 0; }` + `# @lc zLua "LuaBindery"` | igual (1.667 B, `27c00e55`); sin la `@lc`, 174.644 B | lcfix obligatorio (88 `@lc` cambian de número) |
| Src/Lua/LuaGameHooks.cpp:80 | HUECO 16B | `"SmackableParams"` | R2 | primer + `@lc` | sin probar | — |
| Src/Lua/LuaGameHooks.cpp:91 | HUECO 136B | seis etiquetas de SoundConn.h, `High/Medium/Low/Reflection` y un 0.5f | R2 | `#include` de SoundConn.h/primer + `@lc`; el 0.5f con asignación pisada | sin probar | — |
| Src/Lua/source/ldo.c:346 | HUECO 72B | los dos mensajes de `lua_resume` (función estripada en el original) | R2 | primer **separado** de `lua_resume` + `@lc` (nombrar el `$LC` propio de lua_resume resucita 192 B de `.text`, r64) | sin probar | ese efecto |
| SourceLists/zWorld.cpp:128 | HUECO 55B | `"CarSkin Composite - Pixels/Colours"`, alineadas en el original (56 B) | R2 | primer + `@lc`, dentro del paquete D | sin probar | paquete D |
| SourceLists/zWorld.cpp:191 | HUECO 402B | cadenas del original **fuera de su sitio** + 50 B duplicados (`audioimpact`/`audioscrape`, también en `lbl_80409D58`): compensa los −124 B de los `.asciz` sin alinear | R2 | paquete D: cada cadena por primer en su punto + alinear los `.asciz` | medido: W2 + recorte 133 B cuadra secciones, 24.297 B (+784) | es la compensación de todo zWorld |
| SourceLists/zWorld.cpp:244 | HUECO 64B | ceros al final de la `.data` de zWorld; nada en el DWARF | R4 | — | medida: PS2/estáticas de VehiclePartDamage | — |
| SourceLists/zWorld.cpp:4 | DATOS 564B | prefijo bWare/STL + 472 B verbatim (etiquetas AttribSys y WorldConn) que el objeto **ya emite** como `$LC` muertos (dup4: 436 B por duplicado) | R2 | patrón C + `@lc` para las 24 cadenas | sin probar | keep.lst tiene `pad_05_80409D58_rodata` y el bloque se llama `lbl_80409D58` |
| SourceLists/zWorld.cpp:92 | HUECO 126B | 8 cadenas en otro orden que el original (`speech` va seguida de `aivehicle`) | R2 | primers en su punto + `@lc` | sin probar | paquete D |
| SourceLists/zWorld2.cpp:159 | HUECO 239B | vocabulario de cola (10 B duplicados: `<unnamed>`) | R2 | ídem 191 | sin probar | — |
| SourceLists/zWorld2.cpp:16 | DATOS 500B | prefijo + 408 B (cadenas emitidas de nuevo por el objeto: 311 B dup; flotantes de pools) | R2 | patrón C; los flotantes, pools de sus funciones | sin probar | — |
| SourceLists/zWorld2.cpp:186 | HUECO 256B | seis etiquetas **al final de la TU** (compensación de tamaño): 3 son `WCollisionAssets::sTriggerDataSize`, `WGrid::fNumGrids`, `WRoadNetwork::fIntersections` (DWARF `FFFFFFFF`, ya definidas en C); 244 B (80438F84, 80439038, 80438F6C) sin DWARF | R4 (12 B R2 **validado**) | quitar las tres etiquetas + keep `_16WCollisionAssets.sTriggerDataSize`, `_5WGrid.fNumGrids`, `_12WRoadNetwork.fIntersections` | 174.711 → 174.682 B (sin keep, 175.278) | la base ya descuadra .sdata |
| Src/World/CarInfo.cpp:1683 | HUECO 178B | `DRIVER_TYPE_*`, `DriverInfo`; el original no las tiene pegadas a `"_CV"` | R2 | primers + `@lc`, alineadas | sin probar | paquete D |
| Src/World/CarInfo.cpp:927 | HUECO 22B | `"Attrib::Gen::frontend"` (24 B alineada) | R2 | ídem | sin probar | paquete D |
| Src/World/CarLoader.cpp:1569 | HUECO 56B | `SpongeAllocation` + `QUEUED/LOADING/LOADED` + **una tabla de 3 `const char*` y 2.5f** en `.rodata` entre `"LOADED "` y `"CARLOADER: %d "` (DOL 0x8040CAD8) | R2 | la tabla `static const char *const [3]` y las cadenas | sin probar | el bloque actual no tiene la tabla: 16 B de menos |
| Src/World/CarLoader.cpp:2300 | HUECO 52B | `"CarLoaderDefrag but with a really long debug name!!"` (52 B, ya del tamaño bueno) | R2 | primer + `@lc` | sin probar | — |
| Src/World/CarLoader.cpp:254 | HUECO 1802B | 52 `PERF_PART_*`; el original las tiene alineadas (1.888 B) | R2 | primers + `@lc` | sin probar | paquete D (+86 B) |
| Src/World/CarRender.cpp:2494 | HUECO 250B | los `type_name` muertos de CreateCarLightFlares (272 B alineados) | R2 | parche r67 (`type_name = "…"` por `case`) + paquete D | W1 solo: +0x20; W1+50 B de cola: cuadra, 57.928 B | es la compensación de la cola |
| Src/World/CarRender.cpp:395 | DATOS 4B | cola de `uint32 CarLightsStateMap[5] = {1,2,4,8,16}` (DWARF `FFFFFFFF`, 20 B; ya en C en :428) | R3 | — | regla 1: nuestro enlazador estripa entero en `.data` | inventar una variable de 4 B no es el original |
| Src/World/CarRender.cpp:4252 | HUECO 4B | `int OpToM` (DWARF L2319644 `FFFFFFFF`) | R2 **validado** | `int OpToM = 0;` + keep `zWorld.o:OpToM` | igual | — |
| Src/World/CarRender.cpp:477 | HUECO 4B | ceros entre `NISRaceDriverVisible` y `AlphaWritesEnabled`; nada en el DWARF | R4 | — | medida: PS2 | — |
| Src/World/CarRender.cpp:5039 | DATOS 12B | `static unsigned int fc` de UpdateCarStreakingFlares (DWARF `@ 0x80437614`) + `CarLightAngles` + `lastlight` (ya en C) | R2 **validado** | `void UpdateCarStreakingFlares(eView *view) { static unsigned int fc = 0; }` + keep `fc.<N>`,`CarLightAngles`,`lastlight` | igual | nombre `fc.23738` inestable (regla 5) |
| Src/World/CarRender.cpp:5137 | DATOS 16B | flotantes del pool de funciones de CarRender referenciados por `extern const float lbl_…` | R4 | — | medida: `pool2lit.py` en esas funciones + `fncmp` (el comentario habla de un `@ha` de vida larga) | codegen |
| Src/World/SkyRender.cpp:134 | HUECO 54B | `Attrib::Gen::fuelcell_effect` (0x8040AE98, seguida de flotantes) y `eReplacementTextureTable`, **no contiguas** en el original | R2 | primers en dos puntos + `@lc` | sin probar | paquete D |
| Src/World/SkyRender.cpp:305 | HUECO 188B | `SKY_SUNSET_A_*` (196 B alineadas) | R2 | primers + `@lc` | sin probar | paquete D |
| Src/World/VehiclePartDamage.cpp:338 | DATOS 8B | `static unsigned int testDamageLevel = 0, testDelay = 10` (DWARF 0x80438F08/0C), ya en C en :158 | R2 **validado** | borrar el asm + keep `zWorld.o:testDamageLevel`,`testDelay` | igual; sin keep 92.766 B | — |
| Src/World/VisualTreatment.cpp:505 | DATOS 4B | flotante de pool por `extern const float` | R4 | — | ídem CarRender:5137 | — |
| Src/World/World.cpp:142 | DATOS 24B | `RenderChosenPotentialDriveTarget`, `RenderMyCarDriveTarget`, `RenderOtherCarDriveTarget` (int, DWARF `FFFFFFFF`) + `Tweak_World_RestoreScenery` (bool, ya en C en :347) + 8 B sin DWARF | R2 **validado** (16 B) | las tres `int … = 0;` + keep con sus nombres; 8 B anónimos detrás del bool | igual | — |
| Src/World/World.cpp:412 | HUECO 4B | cero tras `g_tweakAIDriftOpponents`; nada en el DWARF | R4 | — | medida: PS2 | — |
| Src/World/World.cpp:46 | HUECO 4B | `float TrafficTeleporterDistanceAroundBubble` (DWARF L2318669 `FFFFFFFF`, justo tras `UglyTimestepHack`) | R2 **validado** | `float TrafficTeleporterDistanceAroundBubble = 0.0f;` + keep | igual | — |
| Src/World/World.cpp:95 | HUECO 27B | `High/Medium/Low/Reflection` (32 B alineadas) | R2 | primers + `@lc` | sin probar | paquete D |
| Src/World/rain.cpp:361 | DATOS 4B | `static int index` de CreateWindRotMatrix (DWARF `@ 0x80438A34`), ya en C | R2 **validado** | borrar el asm + keep `index.<N>` | igual | nombre inestable |
| Src/World/rain.cpp:401 | DATOS 8B | `static ChangingStatus windState = CHANGE; static float changetime` de Rain::Wind, ya en C | R2 **validado** | borrar el asm + keep `windState.<N>`,`changetime.<N>` | igual | nombre inestable |
| Src/World/rain.cpp:473 | DATOS 8B | `float BaseDampness = 1.0f` (DWARF `FFFFFFFF` tras `FOGbias`) + `static float ChangeWeatherTime` de Rain::UpdateAndRender | R2 **validado** | `float BaseDampness = 1.0f;` + la estática en el cuerpo + keep | igual | nombre inestable |
| egami/…/av/cmn/avplayer.cpp:195 | HUECO 22B | `"Assert: %s:%i %s (%s)"` muerto | R2 | primer + `@lc` | sin probar | — |
| egami/…/av/cmn/avplayer.cpp:673 | HUECO 8B | segundo 2^31 (0x41E0000000000000) de pool, sin referencias | R4 | — | medida: función estripada con conversión unsigned→double en el DWARF de avplayer | — |
| egami/…/decoder/cmn/maddeca.cpp:21 | DATOS 256B | `static const int zigzag[64]` (symbols.txt local, 0x80411358) | R1 **validado** | `static const int zigzag[64] = {0, 8, 1, 2, …, 55, 63};` en lugar del `extern` + asm | DOL OK | — |
| libc/fflush.c:4 | ALIAS 0B | `gcc2_compiled.` de fflush.c; dtk nombró así la dirección de `fflush`; ya no lo referencia nadie del enlace | R1 **validado** | borrar la línea | DOL OK | stdio.c tiene el mismo alias (fuera del lote) |

## Oportunidades por orden de valor

1. **zWorld, 10 bloques de `.data` listos** (W0, W34, W56, W7, W8): el enlace de la pendiente sale
   idéntico con cada uno. Coste: 14 entradas de keep.lst renombradas y 6 de ellas con el contador
   de estática local (hace falta el resolvedor de la regla 5 antes de commitear).
2. **zFeOverlay, 4 bloques** (X35 + X6) y **zAttribSys, 2** (X12): DOL OK hoy.
3. **maddeca, fflush, vp6 (etiqueta), gc_interface**: DOL OK; gc_interface pide sacar el comodín
   `auto_05_804144C0_rodata.o` del enlace.
4. **zLua, 12 bloques de cadenas** con la receta de LuaBindery: lcfix hace el resto.
5. **Patrón C (prefijo)**: `#line` funciona; falta localizar la inline de cabecera que usa
   `__FILE__` y medirlo en una sola unidad promocionada (zFEng) con sus `@lc`.

## Artefactos

`scratchpad/lote4_68/`: `idx.py` (DWARF + symtab original + symbols.txt + DOL por ventana),
`corpse.py` (huecos de `.data` con candidatos del DWARF), `ascizaudit.py`, `dupcheck.py`,
`dup4.py`, `relref.py`, `var.py` (compila una variante desde la copia del árbol),
`mylink.py` (relink con sustituciones, `--keep`, `--drop=`), `mkkeep.py`, `lcres.py`
(`@lc` por contenido a un keep.lst temporal), y las variantes `e_*.py` de cada prueba.
