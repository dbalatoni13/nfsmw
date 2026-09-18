# `.debug_aranges`: la atribución que `splits.txt` adivinaba

*Todo lo de abajo es **lectura**: `orig/GOWE69/NFSMWRELEASE.ELF`, `config/GOWE69/{splits,symbols}.txt`, `build.ninja`, `build/GOWE69/config.json` y los `.o` ya extraídos. Cero escrituras en `config/`, `src/` o `configure.py`; cero re-extracciones.*

---

## 1. Qué hemos sacado

**Dos secciones DWARF-1 parseadas por primera vez**, con herramienta permanente y controles que pueden fallar:

| Herramienta | Qué da | Control |
|---|---|---|
| `scripts/aranges.py` | 316 unidades con sus 4 rangos (`.text/.data/.rodata/.bss`) + 584 definiciones tentativas | **5 controles**, todos pasan; el clave: 299/300 inicios de `.text` coinciden **al byte** con `splits.txt` |
| `scripts/pubnames.py` | 21.431 direcciones → unidad de compilación (16.722 funciones = **93,1 %** del `.text` de `symbols.txt`) | 187/187 aciertos sobre 10 SourceLists ya promocionadas |

Cifras, recontadas hoy (`python scripts/aranges.py`):

- **113 fronteras autoritativas que `splits.txt` no tiene** (`.text` 1, `.rodata` 56, `.data` 40, `.bss` 16). Sin filtrar el vertedero de etiquetas vacías salen 127; **14 son falsas**.
- **48 fronteras de INICIO que `splits.txt` tiene MAL** (`--cotejo`), y las 48 en la misma dirección: aranges pone la raya **antes**. Ni una en `.text`.
- **1.114 rangos de `splits.txt` sostenidos**: `checksplits.py` da hoy `0 solapes, 0 cortes`. De los 316 inicios de `.text`, 299 confirmados; de los finales, 298 de 299.
- **Refutado**: nada. No hay ni una atribución de `splits.txt` que aranges contradiga de raíz (ver §5).
- **Refutado de lo nuestro**: el plan de `docs/analisis/r55-jf-inittmr.md` («faltan 144 B de flotantes») y la propuesta viva de `docs/analisis/r61-eax.md` §4/§8 (raya en `0x8045B1F0` + `asm .space 104`).

Control negativo probado: reparseando con la hipótesis equivocada (los 4 terminadores los primeros pares), el control 1 cae a 188/236. No es un control que no pueda fallar.

---

## 2. El paquete para `splits.txt`

Manda `memory/nfsmw-rango-no-basta`: **el rango correcto no basta**, mover un dato de dueño cambia su posición de enlace. El criterio que ordena todo el paquete, y que es nuevo:

> Mover una frontera es **DOL-neutro** sólo si cedente y receptor están en el **mismo estado**. Los dos `NonMatching` → enlazan los dos objetos extraídos, contiguos: se reparten los mismos bytes en el mismo sitio. Los dos `Matching` → ninguno de los dos extraídos entra en el enlace: sólo cambia el denominador.

Formato: TAB + nombre de sección a 12 columnas, finales **LF** (`splits.txt` y `symbols.txt` son LF puros; sólo `keep.lst` es CRLF — `memory/crlf-grep-miente`).

### P1 — PRIMER PAQUETE ATÓMICO. `zEAXSound`, 4 líneas, una re-extracción

Riesgo mínimo: `zEagl4Anim`(7), `zEAXSound`(8), `zEAXSound2`(9), `zEcstasy`(10) son **contiguas en el orden de enlace y las cuatro `NonMatching`** (`configure.py:649-654`). La imagen base no se puede mover.

```
Speed/Indep/SourceLists/zEagl4Anim.cpp:
	.bss        start:0x8045B110 end:0x8045B1E0      # antes end:0x8045B268
Speed/Indep/SourceLists/zEAXSound.cpp:
	.bss        start:0x8045B1E0 end:0x8045DDF8      # antes 0x8045B268..0x8045DE60
Speed/Indep/SourceLists/zEAXSound2.cpp:
	.bss        start:0x8045DDF8 end:0x8045EC40      # antes 0x8045DE60..0x8045ECB0
Speed/Indep/SourceLists/zEcstasy.cpp:
	.bss        start:0x8045EC40 end:0x804729E0      # antes start:0x8045ECB0
```

**Y con ellas, `config/GOWE69/keep.lst` (CRLF)** — dos huecos anónimos NUEVOS a principio de sección que `-strip-unused-data` se llevaría **en silencio** (`docs/TRAMPAS.md`). Nombre predicho por el patrón de `keep.lst:189/224`; **hay que leerlo del `.o` re-extraído antes de darlo por bueno**:

```
zEAXSound.o:gap_07_8045B1E0_bss
zEAXSound2.o:gap_07_8045DDF8_bss
```
Las dos que ya están (`keep.lst:189` y `:224`) no se tocan: encogen de 140→4 B y de 108→4 B, no cambian de nombre.

**Lo que NO hay que aplicar**: el `asm() .section .bss / .space 104` de `r61-eax.md` §8.2. Con estas cuatro líneas **sobra**: esos 104 B son la cabeza de `.bss` de `zEAXSound2`, no un déficit nuestro. Caso de libro de `memory/nfsmw-andamios-caducan`, cazado antes de aplicarlo.

**Verificación**: `linkdelta zEAXSound` tiene que pasar de `bss+32` a `IGUAL`. Aritmética: ventana nueva `0x8045DDF8-0x8045B1E0 = 11.288`; ventana de hoy `11.256`; diferencia **+32 exactos**, que es el `bss+32` sellado en `r61-eax.md`.
**Efecto de arrastre asumido**: `zEAXSound2` pasará de `IGUAL` a `bss+8`. Eso es **descubrir** 8 B de trabajo real, no una regresión — su `IGUAL` de hoy es artefacto de las dos rayas malas. Y si se mueve `zEAXSound` sin mover `zEcstasy`, `zEAXSound2` queda en **−104**, peor que hoy. **Las cuatro van juntas o no van.**

### P2 — `inittmr`, 1 línea de `splits.txt` + 1 de `symbols.txt` (obligatoria)

```
Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp:
	.text       start:0x8038607C end:0x80386254
	.rodata     start:0x80413EB8 end:0x80413ED0      # LÍNEA NUEVA
	.bss        start:0x804D5018 end:0x804D5040
	.sdata      start:0x804FF628 end:0x804FF630
```
`config/GOWE69/symbols.txt:33094`, partir en dos (sin esto `checksplits.py` canta `CORTAN UN SIMBOLO: 1` — control negativo comprobado):
```
lbl_80413EC8 = .rodata:0x80413EC8; // type:object size:0x4 align:4 data:float
lbl_80413ED0 = .rodata:0x80413ED0; // type:object size:0x90 align:8
```
`0x80413ED0 + 0x90 = 0x80413F60`, justo donde empieza `lbl_80413F60`: corte exacto, sin hueco.

**Cuatro fuentes independientes coinciden**: (a) DELTA4 de aranges = 20 B; (b) los bytes del ELF en `0x80413EB8` son idénticos a la `.rodata` de nuestro `inittmr.o` (dos dobles mágicos + `1.0f` + relleno = 24 B); (c) de `0x80413ED0` a `0x80413F48` hay cinco bloques idénticos de 24 B `"Assert: %s:%i (%s)"`, siete TU de Metrowerks; (d) la correspondencia `$LC0/$LC1/$LC2 ↔ lbl_80413EB8/EC0/EC8` que ya midió la r55.

**Riesgo, y va después de P1 a propósito**: parte `auto_05_80413E30_rodata` en dos, y el segundo trozo (`0x80413ED0..0x80414398`) tiene que colocarse **detrás** de `inittmr` (índice 489→491 en `build/GOWE69/config.json`). Si el troceador los amontona juntos en el 489, el DOL rompe. Indicio a favor, no medida. Es la única línea del paquete que puede subir `linked` de 522 a 523.

### P3 — Las 32 fronteras DOL-neutras de `--cotejo`

Ninguna toca un comodín, ninguna crea un hueco nuevo, ninguna crea unidad. **A)** cedente y receptor los dos `NonMatching`; **B)** los dos `Matching`; **C)** middleware, todo `Matching`.

```
| A) los dos NonMatching
zEagl4Anim   .data 0x804170F4..0x804175E0    .bss 0x8045B110..0x8045B1E0
zEAXSound    .data 0x804175E0..0x80417DA8    .bss 0x8045B1E0..0x8045DDF8
zEAXSound2   .data 0x80417DA8..0x8041A570    .bss 0x8045DDF8..0x8045EC40
zEcstasy     .data 0x8041A570..0x8041B530    .bss 0x8045EC40..0x804729E0
zFe          .data 0x8041B530..0x8041BBD8
zFe2         .data 0x8041BBD8..0x8041D040
zGameplay    .data 0x8041D2B8..0x8041D540
zLua         .data 0x8041D540..0x8041D7EC
zPhysics     .data 0x8041EFE8..0x8041F168    .bss 0x804801D8..0x80484E68
zPhysicsBeh. .data 0x8041F168..0x8041F6A0    .bss 0x80484E68..0x8048BCA8
zPlatform    .data 0x8041F6A0..0x80435768    .bss 0x8048BCA8..0x80496A20
zSpeech      .data 0x80435970..0x80435F28
zTrack       .data 0x80435F28..0x80437170    .bss 0x8049A5A4..0x804A5DB8
zWorld       .data 0x80437170..0x80438F18    .bss 0x804A5DB8..0x804AA560
zWorld2      .data 0x80438F18..0x804390E4    .bss 0x804AA560..0x804AAA88
| B) los dos Matching
zAnim .data 0x804154C0..0x804158B8  .bss 0x80457588..0x80457768
zAttribSys .data 0x804158B8..0x80415918  .bss 0x80457768..0x804577A0
zBWare .data 0x80415918..0x80416488   zFEng .data 0x8041D040..0x8041D178
zFoundation .data 0x8041D178..0x8041D2B8   zRender .data 0x80435768..0x80435778
zSim .data 0x80435778..0x80435970
| C) middleware Matching (las 7 "NUEVA" no dejan hueco)
simpledeblocker.c .rodata 0x804119D0..0x80411AD0
vfwpbdll_if.c     .rodata 0x80411AD0..0x80411AF4   NUEVA
pathinit.cpp .rodata 0x80413140..0x80413160   pathreal.cpp .rodata 0x80413160..0x80413258
pathvol.cpp  .rodata 0x804138A8..0x80413958   pathaction.cpp .rodata 0x80413958..0x80413960 NUEVA
pathbank.cpp .rodata 0x8041399C..0x80413A38   pathdebug.cpp  .rodata 0x80413A38..0x80413A40 NUEVA
pathnode.cpp .rodata 0x80413A40..0x80413A48 NUEVA
lbmpeg.cpp .data 0x80450550..0x80450554  .bss 0x804BED84..0x804BED88 NUEVA
mpegl3base.cpp .data 0x80450554..0x80450558 NUEVA
ssysinit.c .bss 0x804BED68..0x804BED84
public.cpp .bss 0x804D95E8..0x804D9648   tasks.cpp .bss 0x804D9648..0x804D9668 NUEVA
```

**Lo que del cotejo NO propongo, y por qué**: 9 fronteras dejarían el DOL **corto** (NonMatching cede a Matching: `zAI→zAnim −32`, `zLua→zMain −188`, `zWorld2→zFeOverlay −140`…) y 8 lo dejarían **largo** (Matching cede a NonMatching: `zMiscSmall→zPhysics +200`, `zSim→zSpeech +144`…). Ésas exigen tocar fuente antes.

### P4 — Los comodines `auto_*` (24 regiones, ~91 fronteras). **No es de esta ventana**

- **Grupo A (11 comodines disueltos, fronteras limpias)**: `rcmp_vp6_codec_chunk_types` `.rodata 0x804102E8..0x80410308`; `rcmp_mad_codec_chunk_types` `..0x80410440..0x80410468`; `sst3dpos.c` `0x804128B0..0x804128C0`; `ssys.cpp` `0x80412930..0x804129D0`; `sfxrevc.c` `0x80412B78..0x80412B7C`; `sformat.c` `0x80412D7C..0x80412DF8`; `sfrsf.c` `0x80412FE8..0x80412FF0`; `memcard_utilities` `0x804143F8..0x80414408`; `gc_interface.cpp` `0x804144C0..0x80414790`; `gc_device.cpp` `0x80414C40..0x80414CE0`; `printdrv.cpp` `.data 0x804527FC..0x80452860 align:4`.
- **Grupos B/C/D** (varios dueños, cadena de `path/`, comodines que encogen): **16 de sus fronteras caen dentro de un `lbl_*` amasijo** y el troceador aborta con *ends within symbol*; hay que partirlos primero. Los 16 están identificados uno a uno.
- **EL RIESGO QUE LO BLOQUEA TODO**: **43 de los 47 dueños ya están promocionados**. Darle a una unidad `Matching` una `.rodata` que hoy no tiene significa que su **fuente** tiene que emitir esos bytes exactos; si no los emite, el DOL pierde bytes sin dar error. Precio medido de las 49 que no los emiten: **4.364 B** (`scripts/frontcoste.py --precio`). Es el ensayo de libc/libgcc2_4 de la r36, que dio DOL ROTO.

### Orden de aplicación

**P1 → verificar DOL → P2 → verificar DOL → P3 → verificar DOL.** P1 y P2 son independientes: si el DOL rompe, bisecar es trivial. P4 y el bloque COMMON (§6) **después**, y de uno en uno.

---

## 3. Las tres bloqueadas

**`inittmr` — RESUELTA, y refuta el plan de la r55.** Rango `.rodata 0x80413EB8..0x80413ED0` (§2/P2). `docs/analisis/r55-jf-inittmr.md` §«El paquete para la r56» punto 2 decía que había que «escribir los 144 B que faltan» de `lbl_80413EC8`. **Es que no.** `lbl_80413EC8 size:0x98` es un artefacto de fusión de `dtk`: junta el `1.0f` de `inittmr` con 148 B de siete TU de Metrowerks. A `inittmr` **no le falta ni una línea de fuente** (`fncmp` da 0 de 4); le falta el rango. Su `.data` tampoco hace falta: `aranges --list inittmr` da `.data start:0x80451FBC` pero con **0 B de aportación** — es el vertedero de etiquetas vacías (esa dirección es la `.data` de `exit.cpp`, `splits.txt:1841`).

**`steering` — CABO CERRADO. No está, y no puede estar.** Buscado de seis formas en `aranges.py` (`steer`, `LibSN`, `SISteer`, `sn_`…): **salida vacía las seis**. `pubnames.py where` sobre sus cinco direcciones: **«NO lo cubre» las cinco**. Y aunque estuviera, **dos de sus tres rangos son inalcanzables por construcción**: `dwarfout.c:7360-7385` emite exactamente cuatro pares — TEXT, DATA, RODATA, BSS — y los bloques de DATA1/RODATA1 están dentro de `#if 0`; **no hay ninguna entrada de `.sbss`, `.sdata`, `.sdata2`, `.ctors` ni `.init` en toda la sección**. Los rangos propuestos para `steering` son `.bss`(40 B), `.sbss`(20 B) y `.sdata2`(40 B). Para `steering`, **los STT_FILE siguen siendo la única fuente, y lo serán siempre**. No hay nada que aplicar: `splits.txt:602-607` ya lleva lo que propone `ventana-pendiente.md` §1.

**`zEAXSound` — RESUELTA.** `0x8045B1E0..0x8045DDF8`, cierra el `bss+32` **al byte y sin tocar la fuente**. La cabeza tiene doble confirmación: aranges + `pubnames.py where 0x8045B1E4` → `zEAXSound.cpp`, y la aritmética `0x8045B1E0 + 17*8 = 0x8045B268` cierra exacta — la raya de hoy **parte la serie `_.tmp_0.._.tmp_17` por la mitad**. La cola (`0x8045DDF8`) la sostiene **sólo aranges**: son 104 B anónimos, `pubnames` no los cubre. Confianza alta en la cabeza, media en la cola.

> **Contradicción interna que dejo anotada, no resuelta**: `pubnames` ve 18 DIE en `0x8045B1E4..0x8045B264`; `symbols.txt` y la `.symtab` del ELF original no tienen **ni un nombre** en ese tramo. No cambia la atribución (la unidad es la misma en las dos fuentes), pero explica por qué los STT_FILE dejaban 140 B de ambigüedad: la información está en el DWARF, no en la tabla de símbolos.

---

## 4. ¿Sube el techo de 544?

**NO. Y no puede, por construcción.** Contado hoy sobre la regla de enlace de `build.ninja`:

```
616 objetos = 522 de build\GOWE69\src\  (= linked)
            +  22 pendientes de obj\  +  prodg_fixes  +  71 auto_*
techo = 522 + 22 = 544
```

Un comodín disuelto **desaparece del enlace**: baja el denominador, no sube el numerador, **porque su dueño ya estaba contado**. Con las 24 disoluciones posibles: 616 → 592, y el techo sigue siendo `592 − 47 − 1 = 544`. Lo que sube es la razón: **88,31 % → 91,89 %**.

Reparto de los 24 comodines por destino:
- **22** caen en unidades **ya promocionadas** → ni `linked` ni techo.
- **1** cae en una de las 22 pendientes: `auto_05_80413E30_rodata` → `inittmr.cpp`, cuyo objeto extraído **no tiene `.rodata`**. **Es el único que puede dar 523.**
- **2** caen en `spchdata.c`, que **no existe como unidad en `splits.txt`**.

**El techo sube exactamente en un caso**: si `spchdata.c` pasa a ser unidad de verdad (aranges y pubnames la ven, `splits.txt` no). Entonces **545**, y de paso se disuelven `auto_07_804CC6F8_bss` y `auto_06_80451EA4_data`. Techo 545/590.

**Saber que 544 es firme vale**: cierra la pregunta. El premio no está en el techo, está en `linked`, y está en el bloque COMMON (§6).

**Corrección al encargo**: el «46 comodines restantes son `.sdata/.sdata2/.sbss/.ctors/.init`» **está mal, y lo he contado**. Sólo **23** lo son (12 sdata, 7 sdata2, 2 sbss, 1 ctors, 1 init). Los otros 23 son `.rodata/.data/.bss` en territorio libc/Dolphin SDK, donde aranges no llega. Y los «25 con inicio de unidad» son **24**: `auto_05_8041516C_rodata` es un falso positivo del vertedero (`stimerem.c` aporta 0 B).

---

## 5. Lo que aranges DESMIENTE

**El cajón (d) está vacío, y esa es la noticia.** Ni una dirección de aranges cae donde `splits.txt` afirme otro inicio distinto para la **misma** unidad. Las 48 de `--cotejo` son **fronteras corridas, no atribuciones falsas**, y las 48 van en la misma dirección.

Lo que sí desmiente, y es lo más valioso del informe:

1. **Le estamos regalando al vecino de arriba la cabeza de `.data/.rodata/.bss` de 48 unidades.** `.data` 27 unidades (−32 a −216 B), `.bss` 10 (−12 a −168), `.rodata` 11 (−8 a −192). **Trece de ellas son exactamente −32.**
2. **`r55-jf-inittmr.md`**: no faltan 144 B de flotantes. No falta nada de fuente (§3).
3. **`r61-eax.md` §4 y §8.2**: la raya está 16 B desplazada y el andamio de 104 B es la cabeza de `zEAXSound2`.
4. **`memory/nfsmw-inicializador-roba`** queda **corregida** para los cuatro de `zPhysicsBehaviors` (§6).
5. **`configure.py` no sirve como orden de enlace** — 101 inversiones en `.text`, 23 en `.rodata`, 14 en `.data` contra las direcciones. El único orden real es el edge de `main.elf` en `build.ninja` (616 objetos): 0, 0, 0 y 1 en `.bss`.
6. **TRAMPA NUEVA en `aranges.py --dir`**: en `.data/.rodata/.bss` **no hay detección de huecos** como la que sí hay en `.text`. `--dir 0x804B9FFC` (la `.bss` de `steering`) responde `zFeOverlay` — **falso**: la frontera derivada del vecino salta por encima de toda la región de Metrowerks. **En cualquier dirección de Metrowerks, `--dir` imputa al último GCC anterior.**
7. **Dos «huérfanas» que no lo eran**: `0x803A4234` y `0x803C6A40` están en `splits.txt:231` y `:237` como `.over ... rename:.rodata`, al byte. Son confirmaciones. Y los dos casos de `prefijochk.py` (`zGameModes .rodata 0x803EBB48`, `zEcstasy .rodata 0x803DD658`) quedan confirmados: ya no son conjetura.

---

## 6. Las tentativas

**584 pares en 122 unidades**, 302 direcciones distintas. `splits.txt` tiene hoy **UN SOLO** bloque `common`, de 128 B.

**Lo primero, y sin ello todo el cruce es basura: la tentativa NO es COMMON.** Medido con el compilador del árbol (`ngccc.exe -S -O1 -gdwarf+ -G0`):

| Forma | Emite | ¿Entrada de aranges? |
|---|---|---|
| `int gPlainInt;` (ámbito de fichero) | `.globl` + `.lcomm` → `.bss` **real** | **SÍ** |
| `int S::sm;` (dato estático de clase) | `.comm` → **COMMON** | SÍ |
| `int gInit = 0;` | `.data` | **NO** |
| `C gObj;` (con constructor) | `.lcomm` en `.bss` | **NO** |

**Tentativa == sin inicializador y sin constructor.** No implica COMMON ni implica `.bss`. El cruce ingenuo daba «49 REALES de más» en `zEcstasy`, 28 en `postproc.c`… **todos falsos positivos**: ~150 líneas que no se entregaron por medir esto primero.

### Lo que sí sale: el área COMMON, 44.396 B en 19 pares que tilan sin un solo hueco

`0x804F4040..0x804FEDAC`, 13 bloques. El control que lo valida y que podía fallar: **el único `common` que ya existe** (`zMisc.cpp 0x804FE9CC..0x804FEA4C`, 128 B) sale **exacto** como tentativa de `zMisc.cpp`; y `0x804FED4C + 0x60 = 0x804FEDAC = _e_bss` al byte. Con las 13 líneas metidas, `checksplits.py` da **1127 rangos, 0 solapes, 0 cortes, LIMPIO**.

**Choca con los dos comodines más grandes**: `auto_07_804F4040_bss` (43.404 B) y `auto_07_804FEA4C_bss` (864 B) — el **55 %** de los 80.484 B de comodín. Sólo el bloque de `zTrack` son 42.000 B.

**RIESGO 1, y no lo he podido resolver desde aquí**: el orden del área COMMON lo manda `ngcld`, no `splits.txt`. Medido, el orden del original es `zAI, zBWare×2, zWorld2, zEAXSound, zFe2×2, zTrack, **zEAXSound**, zEAXSound2, zFe×2, zFeOverlay, zPhysicsBehaviors×2, zMisc, **zPhysicsBehaviors**×2, gc_interface`. **`zEAXSound` y `zPhysicsBehaviors` salen dos veces en posiciones NO adyacentes**: ningún linker que emita «por objeto, y dentro del objeto por tabla de símbolos» puede producir eso. Y esa es justo la regla que hoy sí se cumple con los dos únicos objetos common que hay.
**RIESGO 2**: no admite prueba parcial — `.bss` normal va **antes** que `*(COMMON)` en `ldscript.ld:75-77`, así que marcar sólo unos bloques convierte los huecos en `.bss` normal y rompe el DOL por una razón que no es la hipótesis. **Las 13 juntas o ninguna.**

**Recomendación**: probar **sólo** `gc_interface.cpp .bss 0x804FED4C..0x804FEDAC align:4 common` (96 B, el último del bloque, el que cierra en `_e_bss`) y medir el DOL. Una prueba de 96 B decide si hay 44.268 B ahí.

**18 de los 34 símbolos del área están UNDEF o ausentes en nuestros `.o`** — lista completa disponible; los cuatro de `zPhysicsBehaviors` tienen la forma medida:
```cpp
template <class T> T *ScratchPtr<T>::mPointer[ScratchPtr<T>::MaxInstances];  // genérica, SIN inicializador
```
→ emite literalmente `.comm _t10ScratchPtr1ZQ29RigidBody8Volatile.mPointer,256,4`. Ni `= {}` (da `.data`) ni la especialización explícita vacía (no emite nada).

### El foco del encargo: **las tentativas NO explican ni `zEAXSound` ni `zWorld2`**

Medido **reenlazando**, no estimado:
- **`zWorld2` bss+64**: los 9 tentativos del original los emitimos ya como COMMON, **9 de 9 con el tamaño exacto**. El +64 está en el `.bss` propio, `0x804AA598..0x804AA79x`: el original tiene `gap_07_804AA618_bss`(8), `..690`(8), `..708`(24), `..730`(48) y nosotros colocamos ahí estáticos de función con otro relleno; desde `0x804AA778` todo se desplaza +40 y acaba en +64. Es **`nfsmw-bss-y-huecos-estripados`**.
- **`zEAXSound` bss+32**: mismo patrón en `0x8045B268..0x8045B364` — **pero eso es con la raya de hoy**. Con la raya de aranges (§2/P1) el `+32` desaparece por aritmética de ventana. Las dos lecturas son compatibles: la fuente tiene 16 B de menos en la cabeza de `.bss` y 16 de más más abajo; el total cierra en `+0` y lo que queda es **orden interno** (`IVehicle.h` incluye `pvehicle.h` antes que `ISimable.h`, `r61-eax.md` §5).

**Al revés — no hay ni un inicializador que sobre.** Cruzados los 298 tentativos con sección real: cero errores. Los 6 candidatos de `zEagl4Anim` (`MemoryPoolManager::gMemoryPool…`) son coincidencia: esos 1.144 B están en `.data` en el **original** y **todos a cero** (0 bytes no nulos, comprobado). Ya casan. **No los toques.**

**De propina, un control que no buscaba**: los tentativos de `0x804FFC6C..0x804FFD58` caen en rangos `.sbss` normales que `splits.txt` ya tiene, y coinciden **al byte en 8 de 9**. El único que no es `maddec.cpp`, y sólo por defecto: aranges llega a `0x804FFC80` y splits a `0x804FFC88` porque los 8 B de en medio son un símbolo **no público**, y aranges sólo lista públicos.

---

## 7. Lo que queda sin resolver

**a) Los finales solapados: RESUELTO, con causa y con salida.** No es enlace parcial ni etiquetas resueltas al grupo. Es que inicio y tamaño se resuelven en momentos distintos: el inicio es `ASM_OUTPUT_DWARF_ADDR` → reubicación que resuelve el **enlazador**; el tamaño es `ASM_OUTPUT_DWARF_DELTA4` → resta de dos etiquetas del **mismo objeto**, que el ensamblador colapsa antes de enlazar. **El tamaño es el de la sección EN EL OBJETO**; el enlace lleva `-strip-unused-data` y el sobrante es el solape. Prueba con su control que podía salir al revés: «objeto − enlazado» es ≥ 0 en **316 de 316**, ni una negativa, 177 exactas, **1.219.784 B estripados**.
**La salida es mejor que «el inicio del siguiente»**: el DIE `TAG_compile_unit` trae `AT_low_pc`/`AT_high_pc` (`dwarfout.c:4957-4958`) — `lo` == inicio de aranges en 316/316, y con `lo/hi` los 316 rangos tilan con **0 solapes**. «El final es el inicio del siguiente» habría sido **falso en 5 sitios** (los 5 huecos de Metrowerks). Para `.data/.rodata/.bss` el DIE no trae equivalente: **sólo vale el inicio**, y el final es el vecino de esa misma sección **filtrando antes las aportaciones vacías**.

**b) Las 240 unidades sin aranges: EXPLICADAS, no pendientes.** 316 en aranges contra **545** en `splits.txt` (no 577; 577 es el número de STT_FILE, que cuenta cosas que `splits.txt` agrupa). Reparto de las 240: **109 libc, 96 Dolphin SDK, 15 comodines, 9 LibSN, 3 OdemuExi2, 3 realmemcard/lib/gc, 4 snd, 1 Runtime.PPCEABI.H**. Todo lo que no pasa por `dwarfout.c`. Confirmación cruzada: **caen exactamente en los 5 huecos del control 2** (el hueco `0x8030EA7C..0x803472B4`, 231.480 B, contiene 181 unidades, 95 libc y 68 SDK). Y `AT_producer` sale en cuatro sabores, **todos GCC, ni un Metrowerks**.
Corrección al encargo: **«los Packages» no son un bloque** — vp6, snd, path, realcore, realmemcard(cmn), egami/rcmp y csis **sí están**. `madidct`, `criticalpath` e `inittmr`, también.

**c) Los 23 comodines de `.sdata/.sdata2/.sbss/.ctors/.init`: NO son frente cerrado.** GCC no emite pares para esas secciones (`dwarfout.c:7360-7385`), pero **los STT_FILE y `pubnames` los cubren**: `auto_08_804FF608_sdata` → `pathaction.cpp`; `auto_08_804FF888_sdata` → `OSFatal.c`; `auto_09_804FFD60_sbss` → `gc_interface.cpp`; `auto_10_80500040/80500140/80500240_sdata2` → `libgcc2.c`; `auto_10_80500388_sdata2` → `ef_fmod.c`; `auto_10_80500C20_sdata2` → `OSFatal.c` (14 símbolos). Sólo por `pubnames`, porque son globales: `auto_08_804FF5B8_sdata` → `pathinit.cpp` (22 símbolos `_4Path.*`); `auto_08_804FF6C8_sdata` → `gc_pad.cpp`. Y `auto_09_804FFED8_sbss` queda **explicado entero**. Multi-dueño (no proponibles hoy): `auto_08_804FEDD8_sdata` y `auto_10_805003A8_sdata2`.

**d) Lo que sigue abierto de verdad:**
- **10 unidades que aranges/pubnames ven y `splits.txt` no**: `spchdata.c` (la única que sube el techo), `eathread.cpp` (exige TU nueva, `Object()` en `configure.py` y mover dos funciones), `zComms.cpp` y `bigfile.cpp` (**vacías**, 0 entradas), más 6 alias de nombre (`gc_public.cpp`≡`public.cpp`, `svecreal6.cpp`≡`svecreal.cpp`, `sdfx.c`≡`ssdfx.c`…).
- **344 símbolos con nombre real de la fuente que `symbols.txt` no tiene** (`ICE_SceneLocations` @0x803D3B4C en zCamera, `planes` @0x80404464 en zSim, `gHuffTable2/3/5`…). Ninguno de `symbols.txt` es contradicho por pubnames. Veta sin minar.
- **El orden del área COMMON** (§6, riesgo 1). Es la única pregunta cuya respuesta vale 44.268 B y que **no se puede contestar sin el DOL**.
- **Un COMMON que nos inventamos**: `_6WWorld.mStartPosition` (16 B) en `zWorld2.o` — **no existe en el ELF original** (buscado en las 26.262 entradas). Hoy no cuesta nada porque nadie lo referencia, pero es ruido.
- El **21 % de offsets desfasados** en `.debug_pubnames` (5.806 de 27.446) queda documentado y sorteado; nadie debe seguir ese offset (`dwarfout.c:6457/6495` contra `:6081/6319`).

*Sondas de un solo uso borradas. `scripts/aranges.py` y `scripts/pubnames.py` quedan en el árbol, con sus controles dentro.*