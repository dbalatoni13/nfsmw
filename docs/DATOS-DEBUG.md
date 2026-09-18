# FUENTES DE DATOS DE DEBUG — el mapa completo

Todo lo que sabemos del fuente original, dónde vive, y qué herramienta lo
lee. Ordenado por valor. Actualizado 2026-09-15 (r71f).

---

## 1. GC `NFSMWRELEASE.ELF` — el oráculo definitivo (92 MB)

**Fichero**: `orig/GOWE69/NFSMWRELEASE.ELF` (nuestro objetivo, build FINAL)

| sección | tamaño | contenido |
|---|---|---|
| `.debug` | 91.947.256 B | DIEs DWARF-1: 2,5 M de entradas, 316 CUs |
| `.line` | 5.476.518 B | tabla de líneas (la que ya usaba lmap) |
| `.debug_srcinfo` | 1.347.248 B | correlación fuente↔línea |
| `.debug_sfnames` | 242.809 B | nombres de ficheros fuente |
| `.debug_aranges` | 18.576 B | rangos de CU ↔ direcciones |
| `.debug_pubnames` | 500.957 B | índice público de nombres |
| `.symtab` + `.strtab` | 1.298.184 B | todos los símbolos con tamaño |

**Qué da por función**: parámetros y locales con **nombre + tipo + registro**
(p.ej. `distance f31`), bloques léxicos con rango, y el **árbol completo de
`TAG_inlined_subroutine`** (qué funciones se expandieron inline y dónde).
La CU trae la **ruta real** (`D:/mw/Speed/...`) y el compilador exacto
(`XGNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube`).

**Herramienta**: `scripts/dwarf1.py`
```
python scripts/dwarf1.py fn TrackCarCameraMover::Update   # ficha de una función
python scripts/dwarf1.py fn 0x80109358                    # por dirección
python scripts/dwarf1.py cus                              # lista de CUs
python scripts/dwarf1.py at 0x9E82CC                      # qué función contiene un offset
python scripts/dwarf1.py die 0x9E82BE                     # dump crudo de un DIE
python scripts/dwarf1.py scan                             # validación global (16 s)
```
Cache en `%TEMP%`. Formato verificado contra `orig/prodg/NGC_GNU_SRC/NGC/gcc/
dwarfout.c` (el código fuente del compilador SN que produjo el ELF).

**Filtro negativo verificado**: nombres que INVENTAMOS no aparecen
(`interpolationTime` ausente; `halfVP2` presente). Lee lo que hubo.

**No cubre**: la libc SN (precompilada sin DWARF — vfprintf no está).

---

## 2. PS2 A124 `NFS.ELF` — el oráculo de tipos (84 MB mdebug)

**Fichero**: `orig/SLES-53558-A124/NFS.ELF` (alpha 124) + `NFS.MAP`
(29.142 símbolos con dirección y tamaño)

**Sección `.mdebug`** (84.546.242 B, ECOFF): símbolos MIPS con los STABS del
build. Por función: parámetros (`this:P7566`), locales con registro
(`distance:r14` = $f14), bloques anidados (`$LBB/$LBE`), labels de línea
(`$LM` con la línea en el campo index) y las rutas del depot Perforce
(`d:/p4_apex1666_d1001856/mw/speed/indep/src/...`).

**Con la extensión EER de Ghidra** (v2.1.28 instalada): **23.576 funciones
(23.479 con nombre real), 18.575 tipos de datos, 8.994 estructuras/uniones
con miembros y offsets**, overlays VU0/DVP con símbolos. Artefacto permanente:
`docs/analisis/eer_tipos_ps2.md` (61.821 líneas). Proyecto Ghidra guardado en
`%TEMP%/ghidra_eer/eerPS2` (reutilizable con `-process` sin re-analizar).

**Herramientas**:
```
python scripts/mdebug.py fn Update__19TrackCarCameraMoverf  # estructura de una función
python scripts/mdebug.py files [patron]                     # rutas de fuente únicas
python scripts/ps2fn.py <símbolo>                            # lista de llamadas de una función
```
Headless Ghidra: `"ghidra_.../support/analyzeHeadless.bat" %TEMP%/ghidra_eer eerPS2
-process NFS.ELF -noanalysis -postscript tools/scratch/eer_export.py <salida>`

**Límite**: es build ALPHA — las sentencias tardías (los bTan de TrackCar,
RenderFlaresOnCar) NO están. Cubre `speed/indep/` completo, no `GameCube/Src/`.

---

## 3. PS2 final `SLUS_213.51` — el calibrador (stripped)

**Fichero**: `orig/SLUS-21351/SLUS_213.51.ELF` (Black Edition, 4.918.740 B)

Sin debug (retail). Su valor: **diff de funciones contra el A124** — las que
cambiaron delimitan dónde el mdebug del alpha NO es fiable para el final.
MIPS: el codegen no se copia, solo la estructura.

---

## 4. XBOX A138 — el diccionario (x86 MSVC)

**Fichero**: `MW_A138_XBOX.iso` (prototipo 8-oct-2005, posterior a A124)

Sin debug binario, pero conserva los `__FILE__` de sus asserts: **113 rutas
de fuente completas**, **149 nombres `Clase::Método` exactos** (los tipos
internos de Attrib, miembros), y literales con su función dueña (el macro de
aserción emite fichero+función+mensaje juntos).

**Artefacto**: `docs/analisis/xbox-a138-nombres.txt` + `docs/analisis/xbox-a138.md`

---

## 5. X360 EUROPEGERMILESTONE — el validador independiente

**Fichero**: `orig/EUROPEGERMILESTONE/NFS.exe` (PE PPC, prototipo 21-oct-2005,
`.text` de 8,4 MB extraído en `tools/scratch/x360_text.bin` @ VA 0x4F0000)

Sin símbolos (stripped; `.pdata` de formato propietario sin decodificar).
Su valor es como **compilador de contraste**: el `cl.exe` XDK
(`build/compilers/X360/14.00.2110/`) + las cabeceras del XDK in-tree
(`src/Packages/xenonsdk/2.0.2135.2/`) compilan nuestro árbol — y cada asm
de GCC es error de sintaxis en MSVC. La sonda mide cuánto es fuente
auténtica y cuánto parche.

**Herramienta**: `python tools/scratch/x360_sonda/sonda.py [--all|<fichero>]`
→ clasifica cada .cpp: OK / GCCASM (andamios) / PLATAFORMA / OTRO.
Estado actual: **590/857 OK (68,8%, 13,9 MB)**, 129 GCCASM, 60 OTRO.

---

## 6. Config del proyecto — los índices locales

| fichero | contenido | herramienta |
|---|---|---|
| `config/GOWE69/symbols.txt` | 38.748 símbolos del DOL: dirección, tamaño, scope | grep directo; alimentó splits |
| `config/GOWE69/splits.txt` | rangos de secciones por unidad fuente | claimdata, checksplits |
| `config/GOWE69/keep.lst` | símbolos que el enlazador debe conservar | lcfix los sincroniza |
| `build/GOWE69/report.json` | estado por unidad: matched_code, fuzzy % | measure, promote |

---

## Jerarquía de uso (qué consultar primero)

1. **¿Cómo era esta función/variable?** → `dwarf1.py fn` (GC final, con nombres)
2. **¿Cómo era esta clase/estructura?** → `eer_tipos_ps2.md` (8.994 structs) o `mdebug.py files`
3. **¿Esta línea del alpha llegó al final?** → diff A124 vs SLUS_213.51
4. **¿Cómo se llamaba esto?** → xbox-a138-nombres.txt
5. **¿Mi fuente es auténtica o parche?** → `sonda.py` (el MSVC no traga asm GCC)

## Los compiladores (para reproducción)

| compilador | ruta | para |
|---|---|---|
| ProDG GCC 2.95.3 SN v1.76 | `build/compilers/ProDG/3.9.3/ngccc.exe` | el objetivo GC (todas las unidades) |
| MWCC GC 2.3-2.7, 3.0a5.2 | `build/compilers/GC/<ver>/mwcceppc.exe` | barridos de procedencia (nunca confirmado como productor) |
| XDK X360 14.00.2110 | `build/compilers/X360/14.00.2110/cl.exe` | la sonda (con flags `-`, no `/`) |
| **El fuente del compilador SN** | `orig/prodg/NGC_GNU_SRC/NGC/gcc/` | verificar comportamiento del GCC (dwarfout.c cerró el formato DWARF-1) |
