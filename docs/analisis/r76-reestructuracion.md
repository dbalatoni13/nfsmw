# r76 — Reestructuración del repositorio: inventario y propuesta

**PROPUESTA. No se ha movido, borrado ni commiteado nada.** Los comandos de la §5
están escritos para revisarse, no para pegarse a ciegas: varios exigen un edit de
código previo que se señala en cada caso.

Medido el 2026-09-16 sobre `agent/vedas-oraculo-r2` (HEAD `c99ec268`).

---

## 0. Resumen ejecutivo

| | número |
|---|---:|
| `.py` en `scripts/` | **290** (139 versionados, 151 no) |
| de ellos NÚCLEO | **48** |
| de ellos ESPECIALIZADA | **71** |
| de ellos DE UNA VEZ (sondas consumidas) | **167** |
| de ellos ROTA/OBSOLETA | **4** |
| ficheros en `tools/` (sin `scratch`) | 64 → **25** núcleo, 17 especializada, 12 de una vez, **7** rota |
| entradas en `tools/scratch/` | 249 (881 recursivo), 128 versionadas |
| `docs/*.md` | 16 (7 son del template dtk ajeno) |
| `docs/analisis/*` | **640** (639 versionados) — 616 son diario de ronda |
| documentos con **cifras caducas** | **6** (§4.3) |
| `scratchpad/` **versionado** | **763 ficheros, 92,3 MB** (83 MB regenerables) |

Tres hallazgos que cambian la forma de la propuesta y que no estaban documentados:

1. **`scripts/` es una raíz de import, no una carpeta.** 96 ficheros hacen
   `sys.path.insert(0, ROOT/'scripts')` y **9 módulos son librerías además de CLI**
   (`build_direct` lo importan **32** scripts). Renombrar `scripts/` o vaciarla es
   una edición de 100+ ficheros, no un `git mv`. La propuesta **no la toca**.
2. **`tools/` es el paquete Python del build.** `configure.py:21` hace
   `from tools.project import ...`, y `project.py` invoca por ruta otros 7 scripts.
   Tampoco se toca su nombre.
3. **124 de los 290 scripts cablean una ruta absoluta de esta máquina**, y 32 de
   ellos apuntan a un scratchpad de **una sesión de Claude ya cerrada**
   (`3eb1ea2d-b037-4ced-b620-8e690908107f`). Es deuda de portabilidad
   independiente de la reorganización, pero la condiciona (§6).

---

## 1. Inventario de `scripts/` (290 ficheros)

Método: docstring leído del fichero (nunca inventado), fecha de
`git log -1 --format=%ad`, y arranque real comprobado. **No se ha lanzado ningún
build**: antes de probar nada se leyó `docs/TRAMPAS.md`, que avisa de que
`build_direct.py --help` **no imprime ayuda, lanza las 33 unidades**. Sólo se han
ejecutado lectores puros; los que enlazan (`dolwhere`, `dolrod`, `undlist`,
`stripmap`) se han clasificado por código, sin correrlos.

### 1.1 NÚCLEO — 48

Se usan en el ciclo normal: medir, comparar, promocionar, DOL, `keep.lst` y los
oráculos del original.

| script | vers. | últ. commit | qué hace (docstring) |
|---|:-:|---|---|
| `estado.py` | sí | 2026-09-09 | el frente entero en una pantalla, y sin creerse las metricas |
| `censo.py` | sí | 2026-09-05 | cuanto CODIGO SIN ESCRIBIR queda, por unidad y ordenado por bytes |
| `censoasm.py` | **no** | sin commit | el censo EXACTO de todo el asm de GCC que queda en el arbol |
| `measure.py` | sí | 2026-09-04 | medida independiente por unidad, abriendo los objetos originales y actuales |
| `unitmap.py` | sí | 2026-08-26 | porcentaje por unidad + total, desde `build/GOWE69/report.json` |
| `linkdelta.py` | sí | 2026-09-09 | la distancia REAL de cada unidad al enlace, seccion por seccion |
| `serie.py` | sí | 2026-09-10 | la tasa REAL de cierre por ronda, reconstruida del historial |
| `pctsnap.py` | sí | 2026-09-03 | instantanea y diff del PORCENTAJE POR FUNCION |
| `promodist.py` | sí | 2026-09-06 | QUE cuesta promocionar cada unidad y CUANTO desbloquea |
| `promopred.py` | sí | 2026-09-09 | el criterio de promocion, como predicado computable |
| `promomap.py` | sí | 2026-09-09 | el mapa del frente de `linked`, en paralelo |
| `promote.py` | sí | 2026-09-10 | comprobacion previa a marcar una unidad como `Matching` |
| `trypromo.py` | sí | 2026-09-05 | prueba una promocion SIN tocar el arbol ni esperar a la ventana |
| `dolwhere.py` | sí | 2026-09-07 | enlaza con una unidad promocionada y dice DONDE difiere el DOL |
| `dolrod.py` | sí | 2026-09-07 | diff de una seccion ENLAZADA en la ventana de una unidad |
| `movidos.py` | sí | 2026-09-09 | que simbolos caen en OTRA DIRECCION al meter una unidad |
| `desplaza.py` | **no** | sin commit | el desplazamiento REAL que una unidad le impone al resto del enlace |
| `undlist.py` | sí | 2026-09-06 | que simbolos INDEFINIDOS impiden promocionar una unidad |
| `stripmap.py` | sí | 2026-09-07 | cuanto del deficit del DOL de cada unidad es DANO DE ESTRIPADO |
| `fncmp.py` | sí | 2026-09-09 | que funciones de una unidad NO son identicas, con o sin SourceList |
| `fndiff.py` | sí | 2026-09-07 | diff de una función contra el objetivo (por SourceList o por ruta) |
| `audit.py` | sí | 2026-09-05 | PRUEBA que una funcion casa, en vez de creerse el porcentaje |
| `symdiff.py` | sí | 2026-08-21 | diff instrucción a instrucción de un símbolo (izq=original, der=nuestro) |
| `symtabdiff.py` | sí | 2026-08-26 | diff de TABLAS DE SIMBOLOS ELF entre el `.o` objetivo y el nuestro |
| `triaje.py` | sí | 2026-09-07 | clasifica los near-miss por TIPO de diferencia, no por porcentaje |
| `triage.py` | sí | 2026-09-03 | clasifica las near-miss por lo que de verdad les pasa |
| `mnemo.py` | sí | 2026-09-02 | triaje por MULTICONJUNTO DE MNEMONICOS: separa fuente de asignador |
| `arboljump.py` | sí | 2026-09-03 | compara el ARBOL DE SALTOS, que es lo que `mnemo` no ve |
| `frozen.py` | sí | 2026-09-04 | congela el diff de una unidad y comprueba que no ha cambiado |
| `lcfix.py` | sí | 2026-09-09 | vuelve a resolver por CONTENIDO los `$LC` que fuerza `keep.lst` |
| `keepchk.py` | sí | 2026-09-07 | comprueba que cada entrada `objeto:simbolo` de `keep.lst` existe |
| `gapchk.py` | sí | 2026-09-07 | huecos escritos a mano que `-strip-unused-data` se lleva |
| `dwarf1.py` | sí | 2026-09-14 | **el oráculo** del `.debug` DWARF-1 del ELF de GameCube |
| `mdebug.py` | sí | 2026-09-14 | lee el `.mdebug` (ECOFF/MIPS) del build de PS2 (SLES-53558-A124) |
| `pubnames.py` | sí | 2026-09-10 | atribuye CADA SIMBOLO del original a SU UNIDAD DE COMPILACION |
| `aranges.py` | sí | 2026-09-10 | las fronteras de unidad AUTORITATIVAS, leidas de `.debug_aranges` |
| `dwarfmap.py` | sí | 2026-09-07 | de quien es CADA dato, leido del DWARF del ELF original |
| `dwbody.py` | sí | 2026-09-05 | diff del CUERPO DWARF (locales + árbol de inlines + bloques) |
| `dwblocks.py` | sí | 2026-09-03 | compara los RANGOS de bloque/inline del DWARF normalizados |
| `lmap.py` | sí | 2026-09-05 | asm del OBJETIVO con la linea de fuente al lado |
| `plan.py` | sí | 2026-08-30 | plan de reconstruccion: asm + linea de fuente + arbol de inlines |
| `fuse.py` | sí | 2026-08-20 | funde el desensamblado del original con el mapa de lineas de EA |
| `previo.py` | sí | 2026-09-10 | **qué se ha PROBADO YA** en una funcion, y con que resultado |
| `build_direct.py` | sí | 2026-08-31 | compila invocando `ngccc` directamente, saltandose ninja |
| `bench.py` | sí | 2026-09-02 | el ciclo de trabajo en 1,4 s en vez de 34 |
| `censorancios.py` | sí | 2026-09-11 | recompila cada unidad PROMOCIONADA y compara: caza `.o` rancios |
| `indice.py` | sí | 2026-09-09 | regenera `docs/HERRAMIENTAS.md` leyendo los docstrings |
| `mapaplaybook.py` | sí | 2026-09-09 | el mapa de `PLAYBOOK.md`, para no cargarlo entero |

**Arranque comprobado (lectores puros, sin build):** arrancan y dan salida útil
`estado`, `censo`, `measure`, `unitmap`, `serie`, `pctsnap`, `promodist`,
`promopred`, `promomap`, `promote`, `fncmp`, `audit`, `triaje`, `triage`,
`mnemo`, `arboljump`, `symtabdiff`, `frozen`, `lcfix`, `keepchk`, `gapchk`,
`dwarf1`, `mdebug`, `pubnames`, `aranges`, `dwarfmap`, `dwbody`, `previo`,
`indice`, `mapaplaybook`. Los que piden argumentos (`fndiff`, `symdiff`,
`dwblocks`, `lmap`, `plan`) dan `IndexError` sin ellos: **es lo normal, no un
fallo**. `linkdelta`, `dolwhere`, `dolrod`, `undlist`, `stripmap`, `desplaza`,
`build_direct`, `bench`, `censorancios` **no se han lanzado**: enlazan o compilan.

**Dos avisos que salieron del arranque y valen más que el inventario:**

- `python scripts/indice.py --check` → **`HERRAMIENTAS.md RANCIO`**. El índice de
  herramientas lleva rancio desde el 10-sep y **no lista los cinco oráculos más
  valiosos del proyecto**: `dwarf1.py`, `mdebug.py`, `pubnames.py`, `aranges.py`
  y `censorancios.py`. Quien lea `HERRAMIENTAS.md` hoy no sabe que existen.
- `python scripts/keepchk.py` → **`2026 ok, 50 RANCIAS`**. Cincuenta entradas de
  `keep.lst` nombran objetos o símbolos que ya no existen.

**Dos NÚCLEO sin versionar**, o sea: se pierden en un clon limpio.
`desplaza.py` (documentado en `HERRAMIENTAS.md`) y `censoasm.py` (recién escrito).

### 1.2 ESPECIALIZADA — 71

Útiles para un frente concreto. Se agrupan por frente, que es como se buscan.

- **Reparto de registros y el compilador por dentro (14)** — `rtldump`,
  `schedtrace`, `lreg`, `alloc`, `regalloc`, `regmap`, `pines`, `sprobe`,
  `swsweep`, `casetree`, `casediff`, `deadreg`, `permuter` (115 kB, el permutador),
  `stmtorder`.
- **Datos, `.rodata` y el pool (24)** — `claimdata`, `claimlbl`, `claimrange`,
  `lcmap`, `lcpool`, `deadstr`, `deadlink`, `dupdata`, `dupstr`, `genrodata`,
  `rodata`, `rodatagaps`, `strseq`, `pool2lit`, `datadiff`, `datacmp`, `seccdiff`,
  `residuo`, `litcheck`, `litpos`, `prefijochk`, `prefijotu`, `rodorden`,
  `stripped`.
- **Enlace y troceado (11)** — `refs`, `extrasym`, `ghostref`, `missingcalls`,
  `relocfantasma`, `checksplits`, `rangechk`, `frontcoste`, `reladdr`, `vtdup`,
  `undefcheck`.
- **Orden de emisión (9)** — `permorden`, `vtord`, `vtable_audit`, `globalini`,
  `textorder`, `missinline`, `parseord`, `reorden`, `derotate`.
- **Otras plataformas y middleware (6)** — `ps2fn`, `ps2map`, `zdump`, `libdiff`,
  `libdwarf`, `pctall` (nuevo, sin versionar: mide las cuatro versiones de
  `configure.py:VERSIONS`, no sólo GameCube).
- **Auditoría y mantenimiento (7)** — `auditecho`, `audit_zeros`, `callcheck`,
  `loss`, `tamfn`, `mangfix`, `scratchclean`.

**Sin versionar en esta clase (5):** `rodatagaps.py`, `frontcoste.py`,
`reladdr.py`, `scratchclean.py`, `pctall.py`. `rodatagaps` y `scratchclean` están
documentados en `HERRAMIENTAS.md` y **no existen en un clon limpio**.

**`rodata.py` no tiene docstring y `HERRAMIENTAS.md` la lista como "deber
pendiente". No lo es: es una LIBRERÍA** que importan `audit`, `claimlbl`,
`litpos`, `reladdr` y `strseq`. Borrarla rompe cinco herramientas.

### 1.3 DE UNA VEZ — 167 (sondas ya consumidas)

Son las familias que `indice.py` ya marca como desechables:
`agent_*` (144), `fe17_*` (3), `jf17_*` (2), `mn_*` (4), `s_*` (5), `sweep_wld`,
`zae_probe`, `zcmp`, `zcmp2`, `_zfe2_nearmiss`, más cuatro sin docstring que son
sondas de hecho: `cases`, `cheap`, `pct_ecs`, `unitscore`.

**151 de ellas no están versionadas**, lo que confirma la clasificación.

**Dependencias internas (importa para archivarlas):** las sondas **sólo se
importan entre ellas**, nunca desde una herramienta. Hay seis concentradores:
`agent_eax57_ventanas` (12 dependientes), `agent_phb_harness` (8),
`agent_fo2_sched` (8), `agent_eax57_cad` (4), `agent_ecs_upi` (2),
`agent_ec2_bisect` (2). **Pero resuelven a sus hermanas vía `<ROOT>/scripts`, no
vía su propio directorio**: si se mueven a `scripts/archivo/`, los imports entre
hermanas se rompen. Como son sondas consumidas, es un coste aceptable — pero hay
que decirlo, no descubrirlo. 21 de ellas además importan `build_direct`, que se
queda en `scripts/`: eso sí sigue funcionando.

### 1.4 ROTA / OBSOLETA — 4

| script | diagnóstico (medido hoy) |
|---|---|
| `keepchk2.py` | **ROTA.** Imprime `no encuentro el edge de main.elf en build.ninja` y no hace nada. La sustituye `keepchk.py`, que sí funciona (`2026 ok, 50 RANCIAS`) |
| `zstat.py` | **ROTA.** Cablea `…/3eb1ea2d-b037-4ced-b620-8e690908107f/scratchpad/base_report.json`, el scratchpad de una sesión cerrada. `FileNotFoundError` seguro. Sin docstring |
| `phantom.py` | **OBSOLETA y autodeclarada.** Su propio docstring empieza por `RESUELTO.`; ejecutada hoy devuelve `0 funciones, 0 B`. Se conserva el texto, no el código |
| `x360ref.py` | **ROTA en esta máquina.** `UnicodeEncodeError: 'charmap' codec can't encode character '\u2194'` al imprimir su propia ayuda: la consola es cp1252. Arreglo de una línea (`sys.stdout.reconfigure(encoding='utf-8')`), no es un rediseño |

---

## 2. Inventario de `tools/` (64 ficheros)

`tools/` **no es el ciclo de trabajo**: es el paquete Python del build heredado
del template dtk más el estrato de herramientas de las primeras rondas.

### 2.1 La cadena intocable

- `configure.py:21` → `from tools.project import (Object, ProgressCategory, …)`
- `tools/project.py:37` → `from . import ninja_syntax`
- `tools/__init__.py` (0 bytes) hace que ese import funcione: **no es basura**.
- `configure.py:2303` invoca `tools/hasher.py` (que usa `StringHash32.py`).
- `tools/project.py` invoca por ruta otros siete: `download_tool.py` (540),
  `decompctx.py` (547, 1321-1323), `transform_dep.py` (867), `rename_section.py`
  (951), `fake_ok.py` (1598), `changes_fmt.py` (1665), `splat_to_config_json.py`
  (1777).
- `download_tool.py:177-180` invoca `patch-toolchain.py` con `check=True`: si
  falla, **la descarga de compiladores aborta**.

**NÚCLEO: 25** (los anteriores + `_common.py`, `compare_common.py`,
`defines_common.py`, `decomp-diff.py`, `decomp-context.py`, y la cadena DWARF
`dwarf_unify.py` → `split_dwarf_info.py` → `lookup.py`, más `dwarf-compare.py`,
`dwarf-audit.py`, `generate-dwarf-report.py` y **`dwarf1_gcc_line_info.py`**, que
no es un fósil sino una librería de `dwarf-compare.py` — ver §2.2).

**ESPECIALIZADA: 17** — PS2 (`add_line_numbers_to_ps2_asm.py`,
`ps2-type-compare.py`, `map2splat_symbols_txt.py`), AttribSys
(`attrib_generator.py`), eventos y mensajes (4 generadores), strings
(`harvest_strings.py`), zLua (`zlua_annot.py`, `zlua_str.py`), y
`flag_permuter.py`, `line_lookup.py`, `bHash.py`, `file_generator.py`,
`fnasm.py`, más la extensión `vscode-nfsmw-match/`.

**DE UNA VEZ: 12** — `find_promotable.py`, `fncheck.py`, `fnside.py`,
`nearmatch.py`, `promote_direct.py` (cablea el SHA-1 del DOL), `promote_test.py`,
`sun_data_{dump,extract,compare}.py` (cablean `zPlatform.o`), `zcam_audit.py`
(cablea un rango de direcciones), `zlua_build.py` (cablea la ruta absoluta del
repo), `thingy.py`. **Nueve de los doce ni siquiera están versionados.**

### 2.2 ROTA / OBSOLETA en `tools/` — 7, con tres grupos de duplicados

| fichero | diagnóstico |
|---|---|
| `replace_rodata.py` | **ROTA, demostrable.** Línea 4: `from defines import DATADIR, WIPE_TYPE, …` y **`tools/defines.py` no existe**. `ImportError` seguro. Es del template, de otro juego |
| `decompctx2.py` | **FÓSIL.** Copia vieja de `decompctx.py`. Su único consumidor es `tools/decomp.bat`, que llama a `..\m2c\m2c.py` **fuera del repo** |
| `cflags_common.py` | **MUERTO.** 60 bytes, apunta a `extern/musyx/include` (otro juego). Sólo lo usa `decompctx2.py` |
| `decomp.bat` | **MUERTO.** 8 de sus 13 líneas comentadas; cierra la cadena anterior |
| `dwarf1_gcc_split_info.py` | **FÓSIL y HUÉRFANO.** Copia bifurcada de `dwarf1_gcc_line_info.py`: cabecera idéntica palabra por palabra, 90 B de diferencia, 15 meses sin tocar. **Cero consumidores en todo el árbol** (comprobado con `grep -rn --include=*.py`) |
| `apply_objdiff_mappings.py` | **SUPLANTADO.** `configure.py:2397` define su propia `apply_objdiff_symbol_mappings()` y la llama en `:2438`. El script quedó huérfano |
| `apply_csv_mappings.py` | **HUÉRFANO.** Cero consumidores; cablea `./symbols/xenon_symbols.csv` |

**Duplicados que NO lo son, y conviene dejar por escrito:** `decomp-context.py`
no es una copia de `decompctx.py` (uno preprocesa para m2c, el otro agrega
contexto para un agente), y `dwarf-audit.py` no es una copia de
`dwarf-compare.py` (uno mira declaraciones de cabecera, el otro el DWARF de una
función). Los cuatro se quedan.

> **Corrección importante, medida al verificar el borrado:**
> **`dwarf1_gcc_line_info.py` NO es un fósil y NO se puede borrar.**
> `tools/dwarf-compare.py:35` hace
> `from dwarf1_gcc_line_info import process_file as export_debug_lines`,
> y a `dwarf-compare.py` lo importa a su vez `generate-dwarf-report.py:18`.
> Es una **librería de la cadena DWARF**, o sea NÚCLEO. El huérfano es sólo su
> gemelo `dwarf1_gcc_split_info.py`, que no lo importa nadie.
> Sin este `grep`, el borrado «obvio» de los dos duplicados habría roto en
> silencio el informe DWARF y la extensión de VS Code.

### 2.3 Artefactos generados que viven en `tools/` por comodidad

`string_map.csv` (17,6 kB), `string_globals.csv` (5,3 kB),
`string_map_summary.json` (363 B, **con una ruta absoluta de esta máquina**),
`ucom_gap_report.txt` (36,1 kB). Son **salidas** del 19-ago, no herramientas.

### 2.4 `tools/scratch/` — 249 entradas, 881 ficheros, 128 versionados

51 directorios, todos rondas numeradas: `vedas_oraculo`, `vedas_r2..r4`, la serie
`physbeh_*` (18), `agent_z*` (6), `scaf_*` (5), el frente X360/PS3 (6). Sólo 7
tienen algo versionado. **Está vivo hoy**: el commit más reciente que lo tocó es
`bf6edb07`, de hoy, y `vedas_r4/` es la ronda en curso.

Tres cosas que importan más que el recuento:

1. **`XexTool.exe` son 14 bytes de texto `404: Not Found`.** Una descarga fallida
   con nombre de ejecutable. Quien lo invoque recibirá un error incomprensible.
2. **Los ejecutables irreemplazables NO están versionados** y un `tmp/` sí.
   `dtk012.exe` (5,2 MB) es, según `docs/X360_EXTRACTION.md`, la única versión de
   dtk que trae el subcomando `xex`; no se regenera desde el repo. Junto con
   `NFS.exe`, XeXtractor e `isocreator.zip` son ~16,5 MB sin versionar. En cambio
   sí está versionado `vedas_oraculo/tmp/dw1_ours_zEagl4Anim.pkl` (88 kB), cuyos
   dos hermanos mayores no lo están: parece un `git add` accidental.
3. Los ficheros más gordos son volcados regenerables: `vedas_r3/tmp/d_zWorld.json`
   (22,0 MB), `vedas_r2/bench_main.s` (20,7 MB), `vedas_oraculo/bench_main.i.lreg`
   (13,1 MB). Ninguno versionado.

---

## 3. Inventario de `docs/`

### 3.1 `docs/*.md` — 16 ficheros, los 16 versionados

| fichero | bytes | commit | clase |
|---|---:|---|---|
| `PLAYBOOK.md` | 442.262 | 2026-09-09 | **PERMANENTE** — el manual; 6.243 líneas, no se abre entero |
| `TRAMPAS.md` | 19.044 | 2026-09-11 | **PERMANENTE** — dónde miente cada herramienta; 31 entradas, todas medidas |
| `DATOS-DEBUG.md` | 6.856 | 2026-09-15 | **PERMANENTE** — el mapa de datos del original, el más fresco de los tres |
| `GHIDRA_NFSMW.md` | 37.771 | 2026-09-14 | PERMANENTE **con reserva** (§3.4) |
| `HERRAMIENTAS.md` | 14.367 | 2026-09-10 | **GENERADO** por `indice.py` — y **RANCIO** |
| `PLAYBOOK-MAPA.md` | 3.274 | 2026-09-10 | **GENERADO** por `mapaplaybook.py` — al día |
| `ESTRATEGIA.md` | 15.501 | 2026-09-10 | **DIARIO/CADUCO**, y se autodeclara (§3.3) |
| `X360_EXTRACTION.md` | 2.214 | 2026-09-01 | PERMANENTE (receta de extracción) |
| `PS3_PKG_EXTRACTION.md` | 2.041 | 2026-09-01 | PERMANENTE (receta de extracción) |
| `getting_started.md` | 5.286 | 2025-05-14 | **TEMPLATE dtk, ajeno** |
| `comment_section.md` | 4.558 | 2025-05-14 | **TEMPLATE dtk, ajeno** |
| `common_bss.md` | 4.303 | 2025-05-14 | **TEMPLATE dtk, ajeno** |
| `splits.md` | 2.259 | 2025-05-14 | **TEMPLATE dtk, ajeno** |
| `symbols.md` | 2.015 | 2025-05-14 | **TEMPLATE dtk, ajeno** |
| `github_actions.md` | 2.203 | 2025-05-14 | **TEMPLATE dtk, ajeno** |
| `dependencies.md` | 1.409 | 2025-05-14 | **TEMPLATE dtk, ajeno** |

Los siete del template se identifican sin ambigüedad: commit **2025-05-14**
(el import inicial), y `getting_started.md:5` enlaza literalmente a
`github.com/new?template_name=dtk-template&template_owner=encounter` y pone de
ejemplo *The Legend of Zelda: The Wind Waker*. Cuatro de ellos
(`comment_section`, `common_bss`, `splits`, `symbols`) describen mecánica de
**mwcc/CodeWarrior**, y este proyecto compila el target principal con **SN ProDG
(GCC 2.95.3)**: son documentación correcta de un compilador que aquí no se usa.

`docs/images/` son 5 PNG (145,8 kB) del mismo template, y **sólo ilustran
`github_actions.md`**: ningún documento propio los referencia.

`docs/congelado/` son **95 JSON de 98–165 B (12,6 kB en total), los 95
versionados**. Cada uno fija el SHA-256 del objeto de una unidad que ya casa
(`{"sha256": "...", "unidad": "Speed/Indep/SourceLists/zCamera"}`). **No es
documentación: es un candado de regresión**, y es el patrón a imitar — 12 kB que
protegen 95 unidades. Sólo está mal colocado.

### 3.2 `docs/analisis/` — 640 entradas

| familia | n |
|---|---:|
| `rNN-*.md` (informes de ronda) | **489** |
| `brief-r*.md` | 43 |
| `iterN-2026*.md` | 39 |
| `resume-2026*.md` | 7 |
| `2026*-*.md` | 6 |
| conocimiento permanente (sin patrón) | **23** |
| `.txt` / `.py` / `.c` / `.json` / `.patch` | 16 / 7 / 3 / 2 / 1 |

639 versionados; la entrada 640 es el subdirectorio **vacío** `docs/analisis/eer/`.

Distribución por ronda: r18–r30 = 109, r31–r45 = 133 (r36 sola aporta 48),
r46–r60 = 143 (r55 sola aporta 24), r61–r75 = 104. **El ritmo documental
colapsó**: r69=2, r70=2, r72=1, r73=1, **r74=0**, r75=4. Los `brief-` murieron en
r55, hace veinte rondas.

**Conocimiento permanente rescatable de `docs/analisis/` (8):**
`accesores-vacios.md` (597 accesores `{}` en 154 cabeceras — causa de near-miss
cruzada), `arbol-de-saltos.md` (método de `arboljump`), `auditoria-techos.md`
(cómo distinguir techo real de falso), `objetos-sin-fuente-identificados.md`,
`tattrib-get-inline.md` (601 símbolos espurios, el mayor bloqueo de `linked`),
`middleware-delta.md`, `eer_tipos_ps2.md` (dato bruto: 23.576 fns, 18.575 tipos),
`xbox-a138.md`.

**Estado del frente, que cambia cada ronda (5):** `DOSSIER-FRENTE.md`,
`INVENTARIO-ANDAMIOS.md`, `PLAN-CIERRE-100.md`, `ventana-pendiente.md`,
`zmisc-linked.md`. Más `revision-20260914-plan.md`, que es una **directiva
vigente** del usuario (prioridad: ASM→C real, luego 100 % match, luego linked).

### 3.3 `ESTRATEGIA.md`: caduco y honesto

Su línea 3 dice, literal: *«NO TE CREAS LAS CIFRAS DE LAS SECCIONES 1 Y 2: están
congeladas en la r51»*, y remite a `estado.py` / `serie.py`. Sólo §3 y §4 están al
día — y §3.1 ya diagnosticó este mismo problema hace seis rondas: *«La mitad de
`scripts/` no son herramientas»*. Es el modelo a seguir: un documento que se sabe
podrido a medias y lo avisa arriba.

### 3.4 `GHIDRA_NFSMW.md`: apunta a `%TEMP%`

Es conocimiento permanente en cuanto al mapa de memoria (24 bloques) y el método
headless, pero **toda su tabla §1 de rutas apunta fuera del repo**: el proyecto
Ghidra, los scripts, el CSV de 25.645 símbolos y los volcados de los «28 muros»
viven en `C:\Users\jferr\AppData\Local\Temp\opencode\`. Una limpieza de `%TEMP%`
convierte el documento en un índice de cosas que ya no existen.

---

## 4. Vigencia: qué está refutado

### 4.1 Lo que refutan las rondas r68–r75

| documento | refutado por | frase que lo mata |
|---|---|---|
| `compiladores-que-faltan.md` | `r75-compiladores-refutado.md` | *«El codegen de SN para GC esta CONGELADO de v1.46 a v1.76 para este codigo (.text y .data con el MISMO SHA-1 en las tres versiones)»* → *«las vedas restantes son TODAS de forma de fuente con el compilador que ya tenemos»*. **Ya lleva la refutación incrustada en su cabecera** (commit de hoy): es el buen patrón |
| `INVENTARIO-ANDAMIOS.md` (tabla) | `r75-depineo-masivo.md` | *«FALSOS 100% confirmados (pines que solo maquillaban)… El caso extremo es `ActualReadJoystickData`: su 100% necesitaba EMITIR INSTRUCCIONES MUERTAS por asm»*. Reales tras despineo: AddToQueue **95,16**, MsgBarrier **94,29**, ActualReadJoystickData **98,55** |
| `PLAN-CIERRE-100.md` (premisa) | `r75-depineo-masivo.md` | *«los pines estaban SUSTITUYENDO A UNA FORMA DE FUENTE… Con `{ }` y CERO asm la escalera de registros del original sale sola»* (87,5 → 99,85). Cae su tesis de que en una función al 100 % el andamio es *load-bearing* |
| `r68-plan-datos.md` (3 afirmaciones) | `r68-refuta-1.md`, `r68-refuta-2.md` | *«§8: “Los lotes de la tanda 2 no mueven el DOL enviado” — REFUTADA, medida. L8, L9, L10, L11 y L12 rompen 9619ba57 cada uno por separado»* |
| 17 vedas de r47–r60 | `r60b-auditoria-vedas.md` | 17 CADUCAS / 7 DUDOSAS / 4 FIRMES sobre 28 auditadas (18.128 B de 33.668 B), **y tres estaban rancias** — recetas que ya no existían en el árbol |
| `r71-auditoria-merge.md` (cifra) | `r75-depineo-masivo.md` | decía *«124 pins en 45 ficheros»*; r75 despineó 9 ficheros a cero. Su hoja de ruta sigue vigente; la cifra no |

**Nota importante, en la otra dirección:** r75 **recuperó dos vedas que el
histórico daba por muertas** (PRINT/PAD a llaves newlib 87,5 → 99,85;
`sampleTable` en su punto de uso 95,53 → 98,00) y una tercera salió **mejor** de
lo previsto (`SetupNextLoad` 99,66). Es la regla de memoria *«las vedas caducan»*
confirmada una vez más: **caducidad no es sólo pérdida de vigencia, también es
oportunidad recuperada.**

### 4.2 Una discrepancia sin reconciliar

`PLAN-CIERRE-100.md` dice **331/423 TU**; `revision-20260914-match-plan.md` y
`auditoria-20260914-100-sin-asm.md` dicen **525/619 unidades**. Los tres dan el
mismo 32,04 %. `estado.py` hoy confirma **525/619**, techo real **545**. El
`331/423` es el dato malo y nadie lo había cruzado.

### 4.3 Los seis documentos con cifras caducas

`INVENTARIO-ANDAMIOS.md`, `PLAN-CIERRE-100.md`, `DOSSIER-FRENTE.md`,
`promocion-a-linked.md`, `ESTRATEGIA.md` §1-§2, `HERRAMIENTAS.md`.
**Ninguno miente en el método; los seis mienten en los números.** Cinco no lo
avisan arriba; sólo `ESTRATEGIA.md` lo hace.

Cifras buenas hoy (`scripts/estado.py`, 2026-09-16):

```
HECHO    1.264.380 / 3.946.048 B   32,04 %     (linked)
CODIGO   3.922.044 / 3.946.048 B   99,3917 %   (25 funciones, 24.004 B)
ENLACE   525 / 619 unidades, techo REAL 545 -> quedan 21 de verdad
DOL de referencia  9619ba57c9919f95f7f2ac951a2166a3517f91e3
```

---

## 5. Propuesta de estructura

### 5.1 Principio: mover sólo lo que nadie importa ni invoca

Tres cosas quedan **fuera** de la propuesta, por medida, no por prudencia:

- **`scripts/` conserva su nombre y sus 48+71 herramientas en plano.** 96 ficheros
  hacen `sys.path.insert(0, <ROOT>/'scripts')` y nueve módulos son librerías
  (`build_direct` ×32, `rodata` ×5, `extrasym` ×5, `dolwhere` ×5, `trypromo` ×3,
  `promote`, `movidos`, `linkdelta`, `lcmap`). Subcarpetas por familia costarían
  una edición de 100+ ficheros y no compran nada: el catálogo lo da
  `HERRAMIENTAS.md`, que se regenera.
- **`tools/` conserva su nombre y su cadena de build** (§2.1).
- **`docs/PLAYBOOK.md`, `docs/PLAYBOOK-MAPA.md` y `docs/HERRAMIENTAS.md` no se
  mueven**: sus rutas están en el código (`mapaplaybook.py:21-22`,
  `indice.py:25`). Moverlos exige editar esas tres líneas primero.

### 5.2 Árbol propuesto

```
docs/
  INDICE.md                    <- NUEVO. La única puerta de entrada (§6 y fichero aparte)
  PLAYBOOK.md                  (se queda: ruta cableada en mapaplaybook.py:21)
  PLAYBOOK-MAPA.md             (se queda: generado, ruta cableada)
  HERRAMIENTAS.md              (se queda: generado, ruta cableada)
  TRAMPAS.md                   (se queda: es el compañero de HERRAMIENTAS.md)
  DATOS-DEBUG.md               (se queda: el mapa de datos del original)
  GHIDRA_NFSMW.md              (se queda)
  metodo/                      <- NUEVO. Conocimiento permanente rescatado de analisis/
      accesores-vacios.md  arbol-de-saltos.md  auditoria-techos.md
      objetos-sin-fuente-identificados.md  tattrib-get-inline.md
      middleware-delta.md  eer_tipos_ps2.md  xbox-a138.md
  estado/                      <- NUEVO. Lo que cambia cada ronda y hay que releer
      ESTRATEGIA.md  PLAN-CIERRE-100.md  DOSSIER-FRENTE.md
      INVENTARIO-ANDAMIOS.md  ventana-pendiente.md  zmisc-linked.md
      revision-20260914-plan.md
  recetas/                     <- NUEVO. Cómo se saca material de cada plataforma
      X360_EXTRACTION.md  PS3_PKG_EXTRACTION.md
  upstream/                    <- NUEVO. Template dtk: correcto pero AJENO (mwcc, no SN)
      getting_started.md  comment_section.md  common_bss.md  dependencies.md
      github_actions.md  splits.md  symbols.md  images/
  rondas/                      <- docs/analisis RENOMBRADO (616 informes de diario)
datos/                         <- NUEVO. Datos generados versionables, no documentación
  congelado/                   <- de docs/congelado (95 candados SHA-256)
  strings/                     <- de tools/*.csv y string_map_summary.json
  ucom_gap_report.txt          <- de tools/
scripts/
  *.py                         <- 48 NUCLEO + 71 ESPECIALIZADA, en plano (sin cambios)
  archivo/                     <- NUEVO. Las 167 sondas consumidas
tools/
  *.py                         <- 24 NUCLEO + 17 ESPECIALIZADA (sin cambios de nombre)
  archivo/                     <- NUEVO. Las 12 sondas de tools/
  scratch/                     <- sin tocar: está VIVO (vedas_r4 es la ronda en curso)
```

### 5.3 Comandos — bloque A: crear los directorios

```bash
cd /c/Users/jferr/Desktop/nfsdecompiled
mkdir -p docs/metodo docs/estado docs/recetas docs/upstream datos scripts/archivo tools/archivo
```

### 5.4 Bloque B: `docs/analisis` → `docs/rondas` (**exige un edit previo**)

`scripts/serie.py:38` es `ANA = os.path.join('docs', 'analisis')`. **Editar esa
línea a `'rondas'` ANTES del `git mv`**, o `serie.py` deja de reconstruir el
historial y su control pasa a mentir en silencio (es exactamente la trampa
«un control contra una referencia RANCIA no es un control» de `TRAMPAS.md`).

```bash
# 1) editar scripts/serie.py:38  ->  ANA = os.path.join('docs', 'rondas')
# 2) mover
git mv docs/analisis docs/rondas
# 3) control: tiene que seguir diciendo CONTROL OK
python scripts/serie.py | head -3
```

### 5.5 Bloque C: rescatar el conocimiento permanente del diario

```bash
git mv docs/rondas/accesores-vacios.md                    docs/metodo/
git mv docs/rondas/arbol-de-saltos.md                     docs/metodo/
git mv docs/rondas/auditoria-techos.md                    docs/metodo/
git mv docs/rondas/objetos-sin-fuente-identificados.md    docs/metodo/
git mv docs/rondas/tattrib-get-inline.md                  docs/metodo/
git mv docs/rondas/middleware-delta.md                    docs/metodo/
git mv docs/rondas/eer_tipos_ps2.md                       docs/metodo/
git mv docs/rondas/xbox-a138.md                           docs/metodo/

git mv docs/rondas/PLAN-CIERRE-100.md          docs/estado/
git mv docs/rondas/DOSSIER-FRENTE.md           docs/estado/
git mv docs/rondas/INVENTARIO-ANDAMIOS.md      docs/estado/
git mv docs/rondas/ventana-pendiente.md        docs/estado/
git mv docs/rondas/zmisc-linked.md             docs/estado/
git mv docs/rondas/revision-20260914-plan.md   docs/estado/
git mv docs/ESTRATEGIA.md                      docs/estado/

git mv docs/X360_EXTRACTION.md      docs/recetas/
git mv docs/PS3_PKG_EXTRACTION.md   docs/recetas/
```

**Ojo:** `X360_EXTRACTION.md` está citado en `scripts/x360ref.py`; es una
mención en texto, no una ruta abierta, pero conviene actualizarla.

### 5.6 Bloque D: aislar el template ajeno

```bash
git mv docs/getting_started.md  docs/comment_section.md docs/common_bss.md \
       docs/dependencies.md     docs/github_actions.md  docs/splits.md \
       docs/symbols.md          docs/upstream/
git mv docs/images docs/upstream/images
```

Y una nota de tres líneas al principio de cada uno: *«Heredado del template dtk.
Describe mwcc/CodeWarrior; este proyecto compila el target principal con SN ProDG
(GCC 2.95.3). Se conserva como referencia.»*

### 5.7 Bloque E: los datos generados salen de `docs/` y de `tools/`

**Exige un edit previo, comprobado:** `scripts/frozen.py:34` es
`CONG = os.path.join(ROOT, 'docs', 'congelado')`. Editarlo **antes** del `git mv`,
o `frozen.py ls` deja de ver los 95 candados y la red de regresión desaparece sin
dar ningún error.

```bash
# 1) editar scripts/frozen.py:34 -> CONG = os.path.join(ROOT, 'datos', 'congelado')
git mv docs/congelado datos/congelado          # NO borrar: 12 kB que protegen 95 unidades
# 2) control: tiene que seguir listando 95
python scripts/frozen.py ls | wc -l

mkdir -p datos/strings
git mv tools/string_map.csv           datos/strings/
git mv tools/string_globals.csv       datos/strings/
git mv tools/string_map_summary.json  datos/strings/
git mv tools/ucom_gap_report.txt      datos/
```

Los cuatro artefactos de `tools/` no los abre nadie por ruta: son salidas
congeladas del 19-ago.

### 5.8 Bloque F: archivar las 167 sondas de `scripts/`

Sólo se mueven las **versionadas** con `git mv`; las 151 sin versionar se mueven
con `mv` normal (git no las conoce).

```bash
# versionadas (git las sigue)
git mv scripts/_zfe2_nearmiss.py scripts/fe17_andscan.py scripts/fe17_andscan2.py \
       scripts/fe17_score.py scripts/jf17_syn.py scripts/jf17_var.py \
       scripts/mn_diff.py scripts/mn_forms.py scripts/mn_repro.py \
       scripts/mn_sweepflags.py scripts/s_wldspeech.py scripts/s_zcam.py \
       scripts/s_zfe.py scripts/s_zfe2.py scripts/s_zlua.py scripts/sweep_wld.py \
       scripts/zae_probe.py scripts/zcmp.py scripts/zcmp2.py \
       scripts/cases.py scripts/cheap.py scripts/pct_ecs.py scripts/unitscore.py \
       scripts/archivo/

# no versionadas (las 144 agent_* y el resto)
mv scripts/agent_*.py scripts/archivo/
```

**Consecuencia aceptada y medida:** las seis sondas concentradoras
(`agent_eax57_ventanas`, `agent_phb_harness`, `agent_fo2_sched`,
`agent_eax57_cad`, `agent_ecs_upi`, `agent_ec2_bisect`) resuelven a sus hermanas
por `<ROOT>/scripts`, así que **los 36 imports entre sondas se rompen**. Si se
quisiera conservarlas ejecutables, basta añadir `scripts/archivo` al `sys.path`
de las seis; pero por definición son sondas consumidas.

### 5.9 Bloque G: archivar las sondas de `tools/`

```bash
git mv tools/zcam_audit.py tools/zlua_build.py tools/thingy.py tools/archivo/
mv tools/find_promotable.py tools/fncheck.py tools/fnside.py tools/nearmatch.py \
   tools/promote_direct.py tools/promote_test.py \
   tools/sun_data_dump.py tools/sun_data_extract.py tools/sun_data_compare.py \
   tools/archivo/
```

### 5.10 Bloque H: lo que se BORRA

Ocho ficheros muertos de `tools/`, con la prueba al lado. Los cuatro primeros son
una cadena entera y se borran juntos o no se borran.

```bash
git rm tools/decomp.bat            # llama a ..\m2c\m2c.py, fuera del repo; 8/13 lineas comentadas
git rm tools/decompctx2.py         # fosil de decompctx.py; unico consumidor, decomp.bat
git rm tools/cflags_common.py      # 60 B; apunta a extern/musyx/include (otro juego)
git rm tools/replace_rodata.py     # ImportError seguro: `from defines import ...`, no existe tools/defines.py

git rm tools/dwarf1_gcc_split_info.py   # duplicado bifurcado, 90 B de diferencia, CERO consumidores
git rm tools/apply_objdiff_mappings.py  # suplantado por configure.py:2397
git rm tools/apply_csv_mappings.py      # huerfano; cablea ./symbols/xenon_symbols.csv

git rm scripts/keepchk2.py         # ROTA: "no encuentro el edge de main.elf"; la sustituye keepchk.py
git rm scripts/zstat.py            # ROTA: cablea un scratchpad de sesion cerrada
git rm scripts/phantom.py          # autodeclarada RESUELTO; hoy devuelve 0 funciones, 0 B
```

`x360ref.py` **NO se borra**: se arregla con una línea
(`sys.stdout.reconfigure(encoding='utf-8')` al principio).

**`tools/dwarf1_gcc_line_info.py` NO está en la lista**: lo importa
`tools/dwarf-compare.py:35`. Ver la corrección de §2.2. El `grep` que lo destapó,
y que hay que volver a correr antes de ejecutar este bloque, es:

```bash
grep -rn "dwarf1_gcc\|decompctx2\|cflags_common\|replace_rodata\|apply_csv\|apply_objdiff" \
     --include=*.py --include=*.bat . | grep -v "^./.claude/worktrees"
```

### 5.11 Bloque I: los 92,3 MB de `scratchpad/` versionado

Es el mayor peso muerto del repositorio: **763 ficheros versionados, 92,3 MB**,
de los cuales **83,05 MB son 365 `.json` de objdiff regenerables**.

```bash
# 1) preservar lo que el commit r75 declara reutilizable
git mv scratchpad/zai_r75_madidct tools/scratch/zai_r75_madidct

# 2) sacar del indice los volcados regenerables (NO borra del disco)
git rm -r --cached scratchpad
echo "scratchpad/" >> .gitignore
```

`git rm --cached` conserva los ficheros en disco y sólo los saca del índice. Los
`.cpp` (363, 9,1 MB) hay que mirarlos antes: si alguno es la única copia de una
variante medida, va a `tools/scratch/` como el harness de madidct.

### 5.12 Bloque J: `.gitignore` y la raíz del repo

`git status --porcelain -uall` cuenta **78.636 ficheros sin seguir**. Casi todos
vienen de cuatro directorios y tres archivos que no están en `.gitignore`:

```
# al final de .gitignore
scratchpad/
ghidra_11.4_DEV_20250425/
ghidra_11.4_DEV_20250425.zip
DS/
DS.rar
Users/
*.rar
configure.py.bak*
r7_backup_*.cpp
medida.json
pct.json
prelude.obj
```

Con eso, `git status` vuelve a ser legible, que hoy no lo es. Los `.7z` de los
prototipos (≈9,1 GB entre los cuatro) ya están cubiertos por el patrón `*.7z`.

### 5.13 Bloque K: regenerar lo generado

Después de cualquier movimiento:

```bash
python scripts/indice.py        # HERRAMIENTAS.md esta RANCIO desde el 10-sep
python scripts/mapaplaybook.py  # idempotente; confirmado hoy
python scripts/keepchk.py       # y atacar las 50 entradas RANCIAS que salen
```

---

## 6. Deuda que la reorganización deja al descubierto (no la crea)

1. **124 de 290 scripts cablean rutas absolutas de esta máquina.** 57 cablean el
   root del repo (`C:/Users/jferr/Desktop/nfsdecompiled`) y **32 cablean el
   scratchpad de una sesión de Claude cerrada**
   (`3eb1ea2d-b037-4ced-b620-8e690908107f`), entre ellos herramientas reales:
   `arboljump`, `bench`, `claimlbl`, `fndiff`, `litcheck`, `litpos`, `loss`,
   `missingcalls`, `mnemo`, `regalloc`, `relocfantasma`, `sprobe`, `strseq`.
   Casi todas lo usan como *fallback* de `os.environ['SCRATCH']`, así que hoy
   funcionan; pero **el repo no arranca en otra máquina ni en otra ruta**.
   Arreglo mecánico: `ROOT = dirname(dirname(abspath(__file__)))` y
   `SCRATCH = os.environ.get('SCRATCH', os.path.join(ROOT, '.scratch'))`.
2. **`HERRAMIENTAS.md` no lista los cinco oráculos nuevos.** `indice.py` clasifica
   por una lista `FAMILIAS` cableada; `dwarf1`, `mdebug`, `pubnames`, `aranges` y
   `censorancios` no están en ninguna familia. Añadir una familia
   *«Los oráculos del original»* con esos cinco y volver a generar.
3. **Seis herramientas documentadas no están versionadas** (`desplaza`,
   `rodatagaps`, `frontcoste`, `reladdr`, `scratchclean`, y las nuevas `censoasm`
   y `pctall`). Un clon limpio no las tiene y `HERRAMIENTAS.md` las promete.
4. **`dtk012.exe` (5,2 MB) no está versionado y no se regenera desde el repo.** Es
   la única versión de dtk con subcomando `xex`. Si se limpia `tools/scratch/`
   sin cuidado, el frente X360 se queda sin su única entrada documentada.
5. **`docs/GHIDRA_NFSMW.md` documenta trabajo caro que vive en `%TEMP%`.**
6. **50 entradas de `keep.lst` son RANCIAS** (`keepchk.py`, hoy).
7. **Existe una auditoría de upstream previa y sin versionar**:
   `scratchpad/codex_20260914_upstream_audit/inventory.json` (1,16 MB, del
   14-sep) con 31 commits y `api_files_incomplete_count: 300`. Trae el eje que a
   este inventario le falta — **qué hay upstream y qué es local** — y señala un
   frente que este clon no tiene (`config/SPEED_EXE_1_3`, 2.065 símbolos).
   Cruzarlo **antes** de ejecutar el bloque H.

---

## 7. Orden de ejecución sugerido

El riesgo no está repartido por igual. En orden de *(valor ÷ riesgo)*:

| # | bloque | riesgo | por qué primero/último |
|---|---|---|---|
| 1 | K (regenerar) + J (`.gitignore`) | **nulo** | `git status` vuelve a ser legible y `HERRAMIENTAS.md` deja de mentir. Cero movimientos |
| 2 | I (sacar `scratchpad/` del índice) | bajo | −92,3 MB del repo; `--cached` no borra nada del disco |
| 3 | A + F + G (archivar sondas) | bajo | nada fuera de las sondas las importa. Es el 58 % de `scripts/` |
| 4 | H (borrados) | medio | diez fósiles con prueba; correr antes el `grep` de §5.10 y cruzar con §6.7 |
| 5 | D + E (template y datos) | medio | **E exige editar `frozen.py:34` primero** (confirmado, no hipotético) |
| 6 | C (rescatar permanentes) | medio | rompe enlaces internos entre documentos; hay que repasarlos |
| 7 | B (`analisis` → `rondas`) | **alto** | 639 ficheros y un edit en `serie.py`. **Lo último**, y con su control corrido |

Y la regla de `TRAMPAS.md` que gobierna todo esto: **exígele a tu medida un
control que TENGA que cambiar**. Aquí el control es
`python scripts/serie.py | head -3` después del bloque B y
`python scripts/indice.py --check` después del K. Si no cambian, no son controles.
