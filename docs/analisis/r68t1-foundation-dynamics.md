# r68 tanda 1: foundation-dynamics (zFoundation + zDynamics)

Parches de datos escritos en asm pasados a C legitimo. HEAD a5c438bd, DOL de referencia 9619ba57.
Sin commit. Nada tocado en `config/`, `build/`, `configure.py` ni en el keep.lst real.

## 0. Resumen

| | |
|---|---|
| Bloques objetivo | 5 (UMath.cpp:107, UMath.cpp:173, USpline.cpp:196, Geometry.cpp:607, Geometry.cpp:639) |
| Retirados | **5** |
| Se quedan en asm | 0 |
| keep.lst | sin cambios |
| DOL final (las dos unidades a la vez, con copia de keep.lst) | **9619ba57 OK** |

## 1. Bloque a bloque

| bloque | que era | C que lo sustituye | evidencia | resultado |
|---|---|---|---|---|
| `UMath.cpp:107` (4 B, `lbl_803EB444`) | 1a entrada del pool de literales de `BuildRotate`: 0x3B360B61 = 1/360 | `r *= 0.0027777778f;`, borrando los 4 `extern const float lbl_803EB44x` | DOL 0x803EB444: `3B360B61 00000000 3F800000 40C90FDB`; DWARF `BuildRotate` en `symbols/mw_dwarfdump.nothpp:1024838` | **retirado** |
| `UMath.cpp:173` (4 B, `lbl_803EB450`) | 4a entrada del mismo pool: 0x40C90FDB = 2*pi | `angle = r * 6.2831855f;` | igual | **retirado** |
| `USpline.cpp:196` (12 B, `lbl_803EB73C/740/744`) | pool de `USpline::EvaluateCurvatureXZ`: -0.0001f, 0.0001f, 1000.0f | `UMath::Clamp(tangentLengthCubed, -0.0001f, 0.0001f)` y `return 1000.0f;`, borrando los 3 externs | DOL 0x803EB73C: `B8D1B717 38D1B717 447A0000`; DWARF `USpline::EvaluateCurvatureXZ` con el inline `Clamp(a, amin, amax)`, `mw_dwarfdump.nothpp:1029497` | **retirado** |
| `Geometry.cpp:607` (36 B, `algos.3467`) | tabla de despacho, estatica LOCAL de `Geometry::FindIntersection` | dentro de la funcion: typedef local `IntersectionAlgo`, `static const IntersectionAlgo algos[Geometry::MAXSHAPES][Geometry::MAXSHAPES] = {{0,0,0},{0,BoxVsBox,BoxVsSphere},{0,SphereVsBox,SphereVsSphere}};` y la local `handler`. Se borran el `typedef Algo` de fichero, el `extern ... __asm__("algos.3467")` y el asm | DWARF `mw_dwarfdump.nothpp:387514-387519`: typedef `IntersectionAlgo`, `const bool (* algos[3][3])(...) // @ 0x803D3F50`, local `handler` en r0. DOL 0x803D3F50: `0 0 0 / 0 80089658 8008A604 / 0 80089D50 8008A4B8`, que en `symbols.txt` son BoxVsBox, BoxVsSphere, SphereVsBox y SphereVsSphere | **retirado** (medido junto con :639) |
| `Geometry.cpp:639` (4 B a cero) | relleno de alineacion entre el pool de `FindIntersection` (-100000.0f en 0x803D3F74) y las vtables | nada: borrado | DOL 0x803D3F7C = 0; lo pone la `.align` de la vtable siguiente (K10) | **retirado** (medido junto con :607) |

Los nombres de `lbl_` no se sustituyen por otro nombre: eran etiquetas del splitter para entradas
anonimas de pool, y en C son literales. `algos`, `IntersectionAlgo` y `handler` salen del DWARF, que
no deja dudas: no hace falta evidencia de orden (criterio 3). Nada de lo usado esta prohibido por el
criterio 1: ni primer, ni `#line`, ni `aligned`, ni `section`.

Cada sustitucion lleva su nota r68 en la fuente, con el bloque, el DWARF, el valor del DOL y la medida.

## 2. Comprobaciones previas

- **Otros usos de los `lbl_` que se borran**: `grep` de `lbl_803EB44[0-9A-F]`, `lbl_803EB450`,
  `lbl_803EB73C`, `lbl_803EB74[04]`, `algos.3467` y `algos3467` en `src/` e `include/`, fuera de los
  tres ficheros: 0 apariciones.
- **keep.lst** (sha1 b0646b04): ninguna entrada con esos nombres. zFoundation tiene
  `pad_05_803EB230_rodata`, `SN_FPE`, `lbl_803EB3A8`, `lbl_803EB3F0`, `lbl_803EBB3C`, `lbl_8041D204`
  y `gap_*`; zDynamics tiene `lbl_803D3DC8`, `lbl_803D3FD8`, `gap_06_804170CC_data` y los dos
  `_t9zDynTail*`. Ninguna se toca.
- **@lc**: 0 lineas `# @lc zFoundation` y 0 `# @lc zDynamics`, y ninguna `zFoundation.o:$LC` ni
  `zDynamics.o:$LC`. Los literales flotantes renumeran los `$LC` de la unidad, pero keep.lst no
  apunta a ninguno.
- **Trampa `__LINE__`**: ni `BNEW`, ni `__LINE__`, ni `assert` en los tres ficheros, asi que la
  cuenta de lineas puede cambiar. Lo confirma el `.text` identico.
- **Finales de linea**: `UMath.cpp` sigue LF (195 LF, 0 CRLF, igual que en HEAD); `USpline.cpp`
  sigue CRLF (214/214) y `Geometry.cpp` tambien (631/631). Todo ASCII.

## 3. Medidas

Compilacion con `scratchpad/foundation-dynamics68t1/cc.py`, una copia de `compila()` de
`jefe/cmphead.py`: cflags de `build_direct.parse_units`, cwd = raiz del repo y rutas normales, asi
que `__FILE__` no cambia. Escribe en `scratchpad/foundation-dynamics68t1/o/`. Enlace con
`scratchpad/lote5_68/relink.py`.

| variante | objeto(s) | secciones ALLOC frente a build/ | DOL |
|---|---|---|---|
| base, sin sustituciones | los de build/ | — | **9619ba57 OK** |
| base recompilada, arbol sin tocar | zFoundation ea1b2b89, zDynamics 0e12874f | todas `=` | **9619ba57 OK** |
| Geometry :607 + :639 | zDynamics e0742571 | `.text` 29268, `.rodata` 544, `.data` 44, `.bss` 16, `.rela.text`, `.rela.rodata`: todas `=` | **9619ba57 OK** |
| UMath :107/:173 + USpline :196 | zFoundation c94e4f0b | `.text` 36184, `.rodata` 2328, `.data` 248, `.bss` 228, `.rela.rodata`: `=`; `.rela.text` mismo tamano (23376) y distinto contenido: las reubicaciones van ahora a los `$LC` locales en vez de a los `lbl_` globales | **9619ba57 OK** |
| **final**: las dos unidades a la vez, keep = copia de keep.lst | zFoundation c94e4f0b, zDynamics e0742571 | — | **9619ba57 OK** |
| **control A (tiene que romper)**: final con `.rodata+0x214` de zFoundation 3B360B61 -> 3B360B62 | zFoundation parcheado | — | 58fa2e87 **ROTO**, primera dif en 0x803EB447 (`$LC156`), 1 palabra |
| **control B (tiene que romper)**: final con `.rodata+0x188` de zDynamics (algos[0][0]) 0 -> 1 | zDynamics parcheado | — | 051192e5 **ROTO**, primera dif en 0x803D3F53 (`algos.2280`), 1 palabra |

- Ninguna receta toca keep.lst, asi que no hay control «sin el cambio de keep». Los dos controles
  demuestran otra cosa: que el enlace usa MI objeto y que el literal y la tabla caen exactamente en
  0x803EB444 y 0x803D3F50.
- **Sellado**: una recompilacion posterior desde el arbol, en `o/final_zf` y `o/final_zd`, da los
  mismos sha1 (c94e4f0b..., e0742571...). Los objetos corresponden a la fuente final, notas incluidas.

## 4. keep.diff

`scratchpad/foundation-dynamics68t1/keep.diff`: **sin cambios**, 0 lineas `-` y 0 `+`. Solo lleva
un comentario.

## 5. Para el coordinador

- **Nombre del estatico local**: el original (y `symbols.txt`) lo llama `algos.3467`; nuestro
  cc1plus, `algos.2280`. Es LOCAL, no esta en keep.lst y el DOL no lo ve, pero si promote.py u
  objdiff comparan la tabla de simbolos con el extraido, puede salir como simbolo de datos sin
  pareja. El numero es el contador interno de GCC y no se fija con C legitimo (K6). Conviene
  mirarlo en el informe tras el build.
- `config/GOWE69/symbols.txt` sigue nombrando `lbl_803EB444/448/44C/450`, `lbl_803EB73C/740/744` y
  `algos.3467`. Eso sirve para el objeto extraido y no afecta a la unidad promocionada. No lo toco.
- Siguen en asm, fuera de este lote, los huecos de pool de funciones estripadas de estos mismos
  ficheros: UMath.cpp:35/68/81/181 y USpline.cpp:100/123/186 (R2/R4 del plan).
- Cierre: build, sha1 del DOL, y sellar los sha1 de `zFoundation.o` (c94e4f0b...) y `zDynamics.o`
  (e0742571...) contra la fuente.

## 6. Reproducir

```
python scratchpad/foundation-dynamics68t1/cc.py zFoundation scratchpad/foundation-dynamics68t1/o final_zf
python scratchpad/foundation-dynamics68t1/cc.py zDynamics   scratchpad/foundation-dynamics68t1/o final_zd
python scratchpad/lote5_68/relink.py build/GOWE69/src/Speed/Indep/SourceLists/zFoundation.o=scratchpad/foundation-dynamics68t1/o/final_zf/zFoundation.o build/GOWE69/src/Speed/Indep/SourceLists/zDynamics.o=scratchpad/foundation-dynamics68t1/o/final_zd/zDynamics.o keep=scratchpad/foundation-dynamics68t1/keep_copy.lst
python scratchpad/foundation-dynamics68t1/patch.py <in.o> <out.o> .rodata 0x214 3B360B62   # control A
```
Se quedan `o/final_zf` y `o/final_zd` para el verificador. Los demas `.o` (base, v1, ctlA, ctlB)
estan borrados.
