# r76 — Plan de acción: 100 % con código real y portable

Medido el 2026-09-16 con cuatro investigaciones paralelas (`r76-upstream.md`,
`r76-multiversion.md`, `r76-censo-pines.md`, `r76-reestructuracion.md`) y una prueba
piloto ya aplicada.

## 1. Punto de partida, con cifras

| medida | valor |
|---|---|
| GameCube matched | **99,39 %** (3.922.044 / 3.946.048 B), 18.407/18.432 funciones |
| unidades completas | 525 / 619 |
| funciones abiertas | **23**, 23.968 B (6 son de `steering`, 2 el IDCT de `madidct`) |
| andamios reales | **123** = 54 pines + 69 barreras, en 61 ficheros |
| funciones con 100 % apoyado en andamio | **66** (45.632 B) |
| …de ellas con variable que el DWARF **no** tiene | **34** (26.248 B) — cien por cien falsos |
| alias `__asm__("simbolo")` | 226 (≈150 nombres mangled mal declarados + 69 etiquetas de `libc`) |
| bloques de datos en asm | 458 `.section` |
| X360 (cl.exe del XDK) | 0 de 6 SourceLists compilan; **75 % de los errores son andamios nuestros** |
| PS2 alpha | medible: sólo faltan dos descargas (compilador EE + mips binutils) |

**Upstream (repo oficial): 0 pines, 0 barreras, 0 alias en `src/`.** Su README lista
"asignar registros a mano para forzar un match" como el **primer** síntoma de slop. Lo que
tenemos no es una preferencia distinta: es una divergencia de política que además nos
impide compilar en las otras plataformas.

## 2. El principio (criterio del usuario, ya en vigor)

**Código real por delante del porcentaje.** Un 100 % sostenido por un pin es un 100 % falso:
esconde la forma verdadera de la función y rompe PS2/X360/PC. Antes que poner un pin, se
escribe el código que pudo ser el original y se mide.

**Piloto aplicado hoy** — `MemoryPoolManager::NewBlockAux` (zEagl4Anim, 84 B):
el DWARF sólo declara `idx` (r4) y `r` (r3); nuestras `blockStorage` r8, `poolFree` r11 y
`nextPoolFree` r9 eran invención. Barrido de 6 formas sin pines: la natural
(`gMemoryPoolFree += (idx + 1) * 0x10 + 4;`) da 8 filas, **88,57 %**. Las 8 son sólo reparto
de registros. Resultado: **100 % falso → 88,57 % real, tres pines fuera, un fichero más que
compila en MSVC**.

## 3. Fases

### F1 — Despineo guiado por oráculo (123 andamios) · PRIORIDAD 1
Clasificación ya hecha con `dwarf1.py` sobre 72 funciones:
- **Grupo A (53)**: la variable pineada **no existe** en el original → hay que reescribir la
  función con las locales que sí declara el DWARF. Aquí están los 34 falsos 100.
- **Grupo B (61)**: la variable existe (18 incluso en el mismo registro) → falta la forma de
  fuente; barrido de formas, nunca pin.
- **Grupo C (9)**: `libc`/`LibSN`/`csis`, sin DWARF → último recurso, con PS2 `mdebug` como
  oráculo alternativo.

Orden de ataque (todas grupo A, todas con 100 % falso): `FEngine::ProcessPadsForPackage`
(3.080 B), `GTrigger::GTrigger` (2.544), `QuickGame::CreateCars` (1.908),
`AIPursuit::AssignClosestOffsets` (1.684), `GRaceParameters::GenerateIndex` (1.680),
`SFXCTL_Physics::UpdateNIS` (1.592), `PhysicsObject::PhysicsObject` (1.412),
`SFXObj_Collision::InitSFX` (1.304), `RegionQuery::CalculateRegionInfo` (1.208),
`FEngInterfaceFEObjects.cpp` (8 piezas).

### F2 — Alias → declaración real (≈150) · PRIORIDAD 2
Probado con sonda de mangling: declarar el miembro dentro de su clase emite exactamente el
símbolo que hoy forzamos con `__asm__("_Q26Speech7Manager.mGlobalHistory")`. No cambia el
objeto de GameCube y desbloquea 42 ficheros en X360/PS2/PC.

### F3 — Bloques de datos en asm (458) · PRIORIDAD 3
Ya explorados en la r68 (`r68-plan-datos.md` + las dos refutaciones). Rompen MSVC por
C2001/C1091. Criterio del usuario: sólo constructos plausibles, contenido recortado a cero
anotado, nombres dudosos sólo con evidencia de orden.

### F4 — Las 23 funciones abiertas · en paralelo con F1
`steering` (6 funciones, 2.568 B) no tiene DWARF: su oráculo es el `mdebug` de PS2 y la
sonda de X360. `madidct` (2, 1.148 B) tiene harness propio de la r75.

### F5 — Medida multi-versión · PRIORIDAD 2
`scripts/pctall.py` ya escrito. PS2 alpha entra con dos descargas. CI: matriz de 4 versiones
(`scratchpad/r76_multiv/build.yml.propuesto`); la 5.ª (PC) necesita `Platform.WIN32` y la
herramienta `delink` de upstream.

### F6 — Reestructuración · al cerrar F1
290 scripts (167 de una sola vez), 640 informes, 6 documentos con cifras caducas.
Propuesta completa con comandos en `r76-reestructuracion.md`; índice nuevo en `docs/INDICE.md`.

## 4. Protocolo por función (el del piloto)

1. `python scripts/dwarf1.py fn <funcion>` → locales reales, registros, inlines, bloques.
2. Quitar el andamio y escribir la forma que dice el DWARF.
3. Barrido de formas: `build_direct.py <unidad>` + `fndiff.py <unidad> <simbolo>`; quedarse
   con la de menos filas.
4. **Nunca** reintroducir el pin. Si baja el porcentaje, se documenta y se sigue.
5. Anotar junto al código: qué decía el DWARF, qué formas se midieron y con qué resultado.
6. Al cerrar la unidad: `censorancios`, relink del DOL y, si toca unidad promocionada, que
   el DOL no se mueva.

## 5. Lo que hace falta decidir

1. **Descargas de red** para medir PS2 (compilador EE `ee-gcc2.9-991111` y `mips_binutils`).
2. **Portar de upstream**: `c28db606` (arreglos MSVC), merge a tres bandas de `src/types.h`,
   `include/include_asm.h` (ya aplicado en el árbol sin commitear: es idéntico al suyo).
3. **Cuándo** ejecutar la reestructuración (mueve cientos de ficheros).
