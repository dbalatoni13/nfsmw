# r76: EL PORCENTAJE EN TODAS LAS VERSIONES — qué se puede medir hoy y qué falta

Encargo: poder comprobar el porcentaje en **todas** las versiones soportadas,
no sólo en GameCube, para verificar que el código es real y portable y no un
match falso apoyado en pines `asm` de GameCube.

Todo lo que sigue está **medido en este árbol** (rama `agent/vedas-oraculo-r2`,
2026-09-16), no estimado. Lo que no pude ejecutar lo digo y digo por qué.
No se ha descargado nada de la red: las piezas que faltan se identifican por
nombre y se da el comando exacto que las trae.

---

## 1. La respuesta corta

**Hoy sólo se puede medir GameCube.** Ninguna de las otras tres versiones da un
porcentaje en local, y por motivos **distintos** en cada una:

| versión | plataforma | ¿mide hoy? | el primer muro |
|---|---|---|---|
| GOWE69 | GameCube (ProDG/gcc 2.95.3) | **SÍ** | — |
| EUROPEGERMILESTONE | X360 (MSVC PPC 14.00.2110) | no | `configure.py` **revienta** (bug nuestro, ~8 líneas) |
| SLES-53558-A124 | PS2 alpha (EE-GCC 2.9) | no | faltan las dos piezas de la cadena MIPS |
| SLUS-21351 | PS2 retail (Black Ed.) | no | además, su `config.yml` no tiene ni una unidad de fuente |

Y el hallazgo que de verdad contesta la pregunta del encargo está en §4:
**las seis unidades que X360 sí sabe comparar no compilan, y cinco de las seis
mueren en nuestros propios andamios `asm` de GCC**, no en código del juego.

---

## 2. Inventario: qué hay en local, versión por versión

Reproducible con `python scripts/pctall.py` (script nuevo, §6).

### GOWE69 — GameCube — **completo**

| pieza | estado |
|---|---|
| `orig/GOWE69/sys/main.dol` | OK (4.541.888 B) |
| `orig/GOWE69/NFSMWRELEASE.ELF` | OK (105 MB, el del DWARF) |
| `config/GOWE69/` | OK: `config.yml`, `splits.txt`, `symbols.txt` (3,4 MB), `keep.lst`, `ldscript.ld`, `build.sha1` |
| compilador `build/compilers/ProDG/3.9.3` | OK |
| `build/ppc_binutils` | OK (16 binarios `powerpc-eabi-*`) |
| `build/tools/dtk.exe`, `objdiff-cli.exe`, `sjiswrap.exe` | OK |
| `build/GOWE69/config.json` (troceo hecho) | OK |
| `build/GOWE69/report.json` | OK |

`python configure.py --version GOWE69` → **OK** (0,5 s en caliente, 8 s en frío).

### EUROPEGERMILESTONE — X360 — **faltan 2 cosas**

| pieza | estado |
|---|---|
| `orig/EUROPEGERMILESTONE/NfsMWEuropeGerMilestone.xex` | OK (10.072.064 B) + `NFS.exe` desencriptado |
| `config/EUROPEGERMILESTONE/config.yml` | OK |
| `config/EUROPEGERMILESTONE/symbols.txt` | OK (6,3 MB) |
| `config/EUROPEGERMILESTONE/splits.txt` | OK pero **mínimo**: 1.273 B, **6 unidades** |
| compilador `build/compilers/X360/14.00.2110` | OK (`cl.exe` + `c1xx.dll` + `link.exe`) |
| XDK `src/Packages/xenonsdk/2.0.2135.2/.../xbox` | OK (213 cabeceras) |
| `build/ppc_binutils` | OK |
| **`build/tools/jeff.exe`** | **FALTA** |
| `build/EUROPEGERMILESTONE/config.json` | existe, del 19-ago (rancio) |

`python configure.py --version EUROPEGERMILESTONE` → **FALLA**:

```
File "configure.py", line 1079, in <module>
    "cflags": [*cflags_path, "-fno-implement-inlines"],
NameError: name 'cflags_path' is not defined. Did you mean: 'cflags_game'?
```

**Causa raíz, confirmada.** `cflags_path` y `cflags_spch` son **nuestros**: las
bibliotecas `path` (music pathing) y `spch` (speech) del middleware de audio son
una adición local — el `configure.py` del oficial (copia en
`scratchpad/r76_upstream/upstream_configure.py`) no las tiene. Se definieron en
la rama `GC_WII` (líneas 282/291 y 383/398) y en la rama `PS2` (líneas 568/577),
pero **no en la rama `X360`**, y los bloques de biblioteca que las usan
(líneas 1079 y 1117) son comunes a las tres plataformas. Es un fallo de los
nuestros, no del upstream.

### SLES-53558-A124 — PS2 alpha — **faltan las 2 piezas MIPS**

| pieza | estado |
|---|---|
| `orig/SLES-53558-A124/NFS.ELF` | OK (91.932.420 B, con los 84,5 MB de `.mdebug`) + `NFS.MAP` |
| `config/SLES-53558-A124/config.yml` | OK, **34 subsegmentos `asmtu`** (uno por SourceList) |
| `config/SLES-53558-A124/symbols.txt` | OK (1,6 MB) |
| `splat64` / `spimdisasm` / `rabbitizer` / `pyelftools` | OK, instalados |
| includes PS2 (`stlps2`, `Speed/PSX2/bWare/...`) | OK, los 5 existen |
| `include/macro.inc`, `include/labels.inc` | OK |
| **`build/compilers/PS2/ee-gcc2.9-991111/bin/ee-gcc.exe`** | **FALTA** |
| **`build/mips_binutils/mips-linux-gnu-as.exe`** | **FALTA** |
| `build/SLES-53558-A124/config.json` | **no existe**: el troceo nunca se corrió (el directorio sí, lo crea `configure.py` al pasar por la versión, pero está vacío) |

`python configure.py --version SLES-53558-A124` → **OK**… pero genera **43
objetivos y ni una sola regla de compilación**. Motivo (medido): en
`tools/project.py:488`, `build_config = load_build_config(...)` lee
`build/<V>/config.json`; si no existe, `build_config` es `None` y `generate_build`
**no emite ni un objeto, ni el enlace, ni `report.json`**. En PS2 ese fichero lo
escribe el troceo de splat, que en el grafo va *antes* de la reconfiguración.

### SLUS-21351 — PS2 retail — **faltan las 2 piezas MIPS y además el split**

| pieza | estado |
|---|---|
| `orig/SLUS-21351/SLUS_213.51.ELF` | OK (4.918.740 B) |
| `config/SLUS-21351/config.yml` | existe pero **sólo 2 subsegmentos `asm`** a pelo — **0 unidades de fuente** |
| `config/SLUS-21351/symbols.txt` | **VACÍO (0 B)** |
| las 2 piezas MIPS | **FALTAN** (igual que SLES) |

`configure.py --version SLUS-21351` → **OK**, y como `build/SLUS-21351/config.json`
sí existe (de una corrida del 16-sep), sí aparecen `all_source` y `report.json`.
Pero `all_source` está **vacío** y el informe **falla al generarse**:

```
build\tools\objdiff-cli.exe report generate ... -o build\SLUS-21351\report.json
Failed: Failed to open .\build\SLUS-21351\obj\1000.o
```

Las cuatro "unidades" de ese `config.json` son `1000`, `3BF880`,
`data/rodata.rodata` y `data/data.data`: trozos crudos del ELF, no fuentes.

---

## 3. La medida que sí conseguí: GameCube

`build/GOWE69/report.json` (2026-09-16 02:39), con el árbol tal cual está:

| medida | valor |
|---|---|
| fuzzy match | **99,97 %** |
| código casado | **99,39 %** (3.922.044 / 3.946.048 B) |
| funciones | **18.407 / 18.432** |
| unidades completas | **525 / 619** (techo real **546**: 73 son comodines `auto_*`) |
| unidades al 100 % | 607 de 619 |
| `complete_code` | 32,04 % — no es el porcentaje del juego, es cuánto entra en unidades enlazadas |
| datos | 43,94 % — proxy de enlace, no mide datos (ver `scripts/estado.py`) |

Las **12 unidades por debajo del 100 %**, de peor a mejor:

| % | tamaño | unidad |
|---|---|---|
| — | 20 B | `auto_00_8000348C_init` |
| 60,78 % | 1.504 B | `egami/rcmp/dev/source/decoder/cmn/madidct` |
| 98,90 % | 8.760 B | `LibSN/steering` |
| 99,89 % | 145.884 B | `zEcstasy` |
| 99,94 % | 63.744 B | `zTrack` |
| 99,98 % | 170.256 B | `zEAXSound2` |
| 99,99 % | 160.956 B | `zWorld` |
| 99,99 % | 113.016 B | `zEagl4Anim` |
| 99,99 % | 141.472 B | `zGameplay` |
| 99,99 % | 236.176 B | `zPhysicsBehaviors` |
| 99,99 % | 125.008 B | `zCamera` |

**El DOL sigue enlazando.** Forcé un reenlace real (toqué `zAnim.o` para ensuciar
la arista): `ProDG` + `LINK main.elf` + `DOL main.dol` en **22,5 s**, sin errores,
y el `main.dol` resultante sale con el **mismo sha1** que antes de todo este
trabajo (`853fae1d…`), o sea que el árbol quedó bit a bit intacto.

---

## 4. Lo que de verdad contesta el encargo: X360 no compila, y muere en NUESTRO asm

`configure.py` genera para X360 **seis** aristas `msvc`, una por SourceList con
split. Como el `configure` está roto y falta `jeff.exe`, **reproduje esas seis
órdenes a mano**, con los `cflags` exactos que emite `build.ninja` (hay que
lanzarlas desde PowerShell: el MSYS de Git Bash destroza los `/I` y `/D`).

Resultado: **0 de 6 compilan.**

| unidad | segundos | errores | primer error |
|---|---|---|---|
| zAI | 4,6 | 80 | `zAI.cpp(91): C2290: C++ 'asm' syntax ignored` |
| zAttribSys | 0,8 | 32 | `zAttribSys.cpp(515): C2290` |
| zMisc | 0,3 | 1 | `zMisc.cpp(3284): C1091: string exceeds 65535 bytes` |
| zLua | 4,0 | 70 | `zLua.cpp(22): C2290` |
| zPhysicsBehaviors | 3,1 | 5 | `zPhysicsBehaviors.cpp(67): C2290` |
| zSim | 2,6 | 6 | `zSim.cpp(80): C2290` |

Desglose de los 194 errores por código:

| código | n | qué es |
|---|---|---|
| C2290 | 54 | `asm` de GCC (MSVC quiere `__asm`) |
| C2143 | 49 | el `;` que falta *después* de cada C2290 — o sea, más de lo mismo |
| C2001 | 42 | salto de línea en constante: las cadenas `".byte …\n"` de los pools a mano |
| C1091 | 1 | cadena de más de 65.535 B: un pool `asm` gigante en zMisc |
| resto (46) | | C++ real: `C2228`, `C2065`, `C3861`, `C1083`… |

**146 de los 194 errores (75 %) son andamios nuestros, no código del juego.** Y
cinco de las seis unidades mueren en la *primera* línea de andamio, antes de
llegar a compilar nada: `zAttribSys.cpp(515)` es el pool
`asm(".section .rodata\n .byte 0x41, 0x74, 0x74, …")` que escribe a mano
`$LC2151484648` y `gap_05_803D0CFB_rodata`.

### Censo de andamios en `src/` (1.640 en 324 ficheros)

| n | tipo |
|---|---|
| 459 | bloque `asm(".section"/".byte")` — pool de literales escrito a mano |
| 427 | alias de símbolo `asm("nombre")` |
| 292 | `asm` con instrucciones PowerPC |
| 210 | alias `asm("$LC…")` — renombra un literal |
| 190 | barrera vacía `asm("")` |
| 52 | `asm volatile` con instrucciones |
| 10 | `asm volatile("")` |

Ninguno de esos siete tipos existe en MSVC ni sobrevive a EE-GCC con sintaxis
PowerPC. Contexto: `r71c-x360-metrica.md` midió que **el 46 % de los 857 `.cpp`
internos compila tal cual con `cl.exe`** y que 82 (9,6 %) fallan exactamente por
estos andamios. Lo que añade r76 es que **por la vía oficial (las SourceLists,
que es lo que mide el CI) la cifra es 0 %**, porque el unity build hace que un
solo andamio tumbe la unidad entera.

### Y aunque compilaran, X360 sólo cubriría el 10 %

`build/EUROPEGERMILESTONE/config.json` reparte los 8.532.948 B de `.text` del
`.xex` así: **856.544 B en las 6 SourceLists con split** y 7.676.404 B en
comodines `auto_*_text`. El denominador medible hoy en X360 es el **10,0 %** del
binario. Para que la métrica X360 signifique algo hay que escribir
`config/EUROPEGERMILESTONE/splits.txt` de verdad (tiene 6 entradas; el de
GameCube tiene 76 kB).

---

## 5. PS2: hasta dónde llegué, y qué falta exactamente

No me quedé en "falta el compilador". **Corrí el troceo de splat de verdad** para
SLES-53558-A124, con el `config.yml` redirigido al scratchpad para no escribir en
`config/` (splat genera ahí `undefined_funcs_auto.txt` y `undefined_syms_auto.txt`):

* **152 s** (2 min 32 s): 77 s escaneando + el resto escribiendo.
* Salida: **34 ficheros `.s`, 77 MB**, uno por SourceList
  (`zAI.cpp.s` solo son 5,2 MB), más `undefined_funcs_auto.txt` (76 líneas) y
  `undefined_syms_auto.txt` (911).
* `config.json` con **34 unidades**, y sus `name` apuntan a
  `Speed/Indep/SourceLists/*.cpp` — **que existen en `src/`**.

Luego reconfiguré con ese `config.json` colocado (vía `--build-dir` al
scratchpad, para no dejar un `config.json` rancio en `build/`) y el grafo pasó de
43 objetivos a **159**: **34 aristas `ee-gcc`** (compilar nuestra fuente), **34
aristas `as`** (ensamblar el `.s` del original para poder comparar), el enlace y
`report.json`. Los `cflags` que saldrían son:

```
-O2 -g2 -Wa,-L -Wno-ctor-dtor-privacy -I src/Speed/Indep/Libs/Support/stlps2 … -G0
-ffast-math -fno-exceptions -fno-rtti -DEA_PLATFORM_PLAYSTATION2 -DEA_BUILD_A124 …
```

**Conclusión: PS2 alpha es medible, y sólo le faltan dos descargas.** No es un
callejón sin salida como SLUS.

Lo que hay que hacer, en orden:

```bash
# 1. las dos piezas MIPS (no las descargué: son ~40 MB de binarios de terceros)
python tools/download_tool.py mips_binutils build/mips_binutils --tag 2.45
#    URL: https://github.com/dbalatoni13/mips-binutils/releases/download/2.45/windows-x86_64.zip
python tools/download_tool.py compilers build/compilers --tag 20251015
#    URL: https://files.decomp.dev/compilers_20251015.zip
#    OJO: el build/compilers local sólo trae GC, ProDG, Wii y X360 — falta el
#    subdirectorio PS2/ee-gcc2.9-991111. O el zip extraído es parcial, o esa
#    versión del zip no lo lleva; hay que comprobarlo al bajarlo.

# 2. el troceo (152 s) y la medida
python configure.py --version SLES-53558-A124   # escribe build.ninja sin objetos
python -m ninja build/SLES-53558-A124/config.json   # SPLIT + reconfigura solo
python -m ninja all_source progress build/SLES-53558-A124/report.json

# 3. dejar el árbol como estaba
python configure.py --version GOWE69
```

**Aviso**: el paso 2 escribe `config/SLES-53558-A124/undefined_funcs_auto.txt` y
`undefined_syms_auto.txt`. Es normal (los genera splat), pero conviene saberlo
antes de hacer `git status`.

**Predicción, y no la doy por hecha**: los 34 `ee-gcc` fallarán casi todos por lo
mismo que X360 — los `asm(".section")` y los `asm("r3")` son sintaxis de GCC
válida, pero las *instrucciones* y los *nombres de registro* son PowerPC, y en
MIPS `ee-gcc` los rechazará al ensamblar. Eso hay que medirlo, no suponerlo.

Para SLUS-21351 hacen falta además **dos cosas que no son descargas**: escribir
`config/SLUS-21351/symbols.txt` (hoy 0 B) y reemplazar los dos subsegmentos
`asm` de su `config.yml` por 34 `asmtu` como los de SLES. Es trabajo de splits,
no de infraestructura.

---

## 6. `scripts/pctall.py` — la tabla de las cuatro versiones

Script nuevo, **no commiteado**. Lee `configure.py:VERSIONS` y el `config.yml` de
cada versión, así que sigue al repo si se añaden versiones.

```
python scripts/pctall.py                 # tabla (sólo lee; no toca nada)
python scripts/pctall.py --configure     # además prueba configure.py por versión
python scripts/pctall.py --por-unidad    # % por unidad de las que tengan informe
python scripts/pctall.py --json out.json
```

Salida real de ahora mismo:

```
version             plataforma  estado                                            unidades  funciones    % matched  % fuzzy
------------------  ----------  ------------------------------------------------  --------  -----------  ---------  -------
GOWE69              GC_WII      OK (informe 2026-09-16 02:39)                     525/619   18407/18432  99,39%     99,97%
EUROPEGERMILESTONE  X360        falta build/tools/jeff.exe                         -         -            -          -
SLES-53558-A124     PS2         falta build/compilers/PS2/ee-gcc2.9-991111 (+2)    -         -            -          -
SLUS-21351          PS2         falta config/SLUS-21351/symbols.txt (vacio) (+2)   -         -            -          -
```

Con `--configure` la columna `estado` de X360 pasa a decir el `NameError` literal.

Garantías, porque el árbol por defecto es GameCube y no puede quedarse en otra:
`--configure` apunta los `configure_args` del `build.ninja` **antes** de empezar y
los restaura con `atexit` — se cumple también si revienta o si lo cortas con
Ctrl-C. Verificado: tras `--configure`, `objdiff.json` vuelve con el **mismo
sha1** que tenía. El script **no borra nada**.

---

## 7. Plan de CI

Propuesta completa en **`scratchpad/r76_multiv/build.yml.propuesto`** (YAML
validado). Cambios frente al `build.yml` actual:

1. **Matriz por `include`**, no por lista de strings: cada versión lleva su
   `binutils` y su marca `experimental`. Añadir una versión ya no obliga a tocar
   el `if [[ ... ]]` de dentro del `run`.
2. **`continue-on-error: ${{ matrix.experimental }}`**: las versiones que aún no
   están listas informan sin tumbar el job de las que sí.
3. Las deps de PS2 se instalan por `matrix.binutils == '/mips_binutils'`, que es
   la condición correcta, en vez de comparar con una versión concreta.
4. Job `resumen` que junta los `report.json` de todas y escribe la tabla
   comparativa en el `$GITHUB_STEP_SUMMARY`. **Esto es lo que da el "porcentaje
   en todas las versiones de un vistazo"** que pedía el encargo.

### Qué hace falta para que corra

| pieza | ¿la tenemos? |
|---|---|
| contenedor `ghcr.io/dbalatoni13/nfs-gc-build:main` | es **privado**, de `dbalatoni13`. Hay que pedir acceso de lectura al paquete para nuestro fork, o montar uno propio (`encounter/dtk-template-build`, ver `docs/github_actions.md`). **Es el bloqueo nº 1.** |
| `/orig` dentro del contenedor | lo trae el contenedor (`cp -R /orig .`); los binarios no van al repo |
| `/ppc_binutils`, `/mips_binutils` | los trae el contenedor |
| `/compilers` | lo trae el contenedor — y ahí **sí** debería estar `PS2/ee-gcc2.9-991111` |
| `requirements.txt` | ya está en el repo y es correcto (`splat64[mips]`, `spimdisasm`, `rabbitizer==1.16.2`, `pyelftools`, `pygfxd`, `sortedcontainers`, `tqdm`) |
| arreglo de `cflags_path`/`cflags_spch` en X360 | **no**: hay que hacerlo (§2) |
| `Platform.WIN32` para SPEED_EXE_1_3 | **no** |

**Sobre las 5 versiones.** La quinta (`SPEED_EXE_1_3`, PC) **no se puede añadir
sólo al YAML**: no está en nuestro `configure.py:VERSIONS` y, sobre todo,
`tools/project.py` no tiene `Platform.WIN32` (nuestro enum llega hasta
`PS2 = 2`). El oficial usa además una herramienta distinta (`delink`, tag
`v0.16.1`) y un `tools/delink_to_config_json.py` que no tenemos. El diff completo
está en `scratchpad/r76_upstream/project_py.diff` (591 líneas). Por eso la dejo
**comentada** en la matriz con la nota de qué hace falta: meterla tal cual sólo
conseguiría que `argparse` rechace la versión y el job muera en el primer paso.

### ¿Nos sirve el devcontainer público del oficial?

**Para desarrollar sí; para el CI no basta.** Según `r71i-oficial-infra.md` el
devcontainer es `mcr.microsoft.com/devcontainers/cpp:2-debian13` + un venv con
splat64/spimdisasm/rabbitizer/pyelftools/pygfxd + objdiff CLI. Eso resuelve las
deps de Python (que aquí ya están) pero **no trae ni `/orig`, ni `/compilers`, ni
los binutils**, que es justo lo que falta. El contenedor de build privado y el
devcontainer son dos cosas distintas.

---

## 8. Coste en tiempo de todo lo anterior

| operación | medido |
|---|---|
| `configure.py --version GOWE69` | 0,5 s (8 s la primera) |
| `configure.py --version <cualquier PS2>` | 0,5 s |
| reenlace completo del DOL | **22,5 s** |
| `scripts/pctall.py` (sólo lectura) | < 1 s |
| `scripts/pctall.py --configure` | 20,6 s |
| troceo splat de SLES-53558-A124 | **152 s** → 77 MB de `.s` |
| las 6 SourceLists de X360 con `cl.exe` | 15,4 s (todas fallan) |

No corté ningún build por tiempo: ninguno de los que intenté llegó a tardar lo
suficiente. Lo que no corrí fue por **piezas que faltan**, no por coste.

---

## 9. Los tres arreglos, por orden de rentabilidad

1. **X360 `cflags_path`/`cflags_spch`** (~8 líneas en `configure.py`, rama
   `X360`, junto a `cflags_snd`). Lo probé con una copia parcheada en
   `scratchpad/r76_multiv/configure_patched.py`: con eso `configure` pasa. Es lo
   más barato del documento y desbloquea que X360 vuelva a la matriz de CI.
2. **Las dos descargas de PS2** (`mips_binutils` 2.45 y el subdirectorio
   `PS2/ee-gcc2.9-991111` de `compilers`). Desbloquean la **primera medida real
   de portabilidad**: 34 unidades de fuente compartidas con GameCube, compiladas
   con otro compilador y otra arquitectura. Es el experimento que el encargo
   quiere.
3. **`splits.txt` de X360** (hoy 6 unidades = 10 % del `.xex`). Sin esto el
   porcentaje X360 no significa nada aunque compile.

Y el trabajo de fondo que destapan los tres: **1.640 andamios `asm` en 324
ficheros**. Mientras estén, ninguna plataforma que no sea GCC/PowerPC va a
compilar las SourceLists completas, y el porcentaje multiplataforma seguirá
siendo 0 por construcción. `docs/analisis/INVENTARIO-ANDAMIOS.md` y
`nfsmw-andamios-caducan` ya avisaban de que retirarlos es trabajo de después del
100 %; r76 añade el dato de **cuánto** cuestan: son el 75 % de los errores de
compilación en X360.

---

## Ficheros de este trabajo

* `scripts/pctall.py` — la tabla multi-versión (nuevo, sin commitear)
* `scratchpad/r76_multiv/build.yml.propuesto` — el workflow propuesto
* `scratchpad/r76_multiv/configure_patched.py` — copia con el arreglo X360, sólo para medir
* `scratchpad/r76_multiv/x360obj/*.log` — los 6 logs de `cl.exe`
* `scratchpad/r76_multiv/sles_split/` — troceo real de PS2 alpha (34 `.s`, 77 MB)
* `scratchpad/r76_multiv/pctall.json` — volcado de la tabla
