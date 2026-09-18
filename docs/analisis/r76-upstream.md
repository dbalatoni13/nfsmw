# r76 — Que hay en el repo oficial (dbalatoni13/nfsmw) que nos sirve

Investigacion de solo lectura. Fecha: 2026-09-16. Rama local: `agent/vedas-oraculo-r2`.
Estado: `HEAD` esta **1314 commits por delante** y **56 por detras** de `upstream/main`.
Base comun: `59f184a0` — *"5% linked! (#123)"*, 2026-08-21.

Todo lo que sigue esta medido con `git` contra `upstream/*` ya traido (`git fetch upstream`).
No se ha modificado ningun fichero del arbol salvo este informe.

---

## 1. Las ramas de upstream

Aviso metodologico: `git diff --stat main...rama` (tres puntos) mide **desde la base comun**, asi que
ensena tambien trabajo que ya llego a `main` por otra via. Las cifras de "aporta de verdad" de abajo
son de arbol contra arbol (`git diff main rama`), descontando el ruido de `config/SPEED_EXE_1_3/symbols.json`
(26 MB) que ninguna rama tiene porque todas son anteriores a la version de PC.

| Rama | Commits sobre main | Ultimo commit | Autor | Estado | Que aporta |
|---|---|---|---|---|---|
| `main` | — | 2026-09-14 | varios | viva | referencia |
| `dev` | 2 | 2026-04-23 | JohnDeved + dbalatoni13 | **FOSIL** | el import original del frontend; el mismo contenido entro a `main` rebasado por `zFE-dev` |
| `zFE-dev` | **0** | 2026-07-03 | Noxis/dbalatoni13 | **FUSIONADA** | ya esta entera en `main` (merge `80081647`) |
| `physicsdev` | 14 | 2026-07-15 | dbalatoni13 | **CASI FUSIONADA** | su herramienta (`generate-dwarf-report.py`, extension de VS Code, `ps2-type-compare.py`, `add_line_numbers_to_ps2_asm.py`) ya esta en `main` |
| `pr-117` | 15 | 2026-08-17 | tabby (12) + dbalatoni13 | viva, sin mergear | Ecstasy de GameCube: `eMathE`, `eViewPlat`, `eSolidPlat`, `eStrip`, `EcstasyE`. 0 ficheros nuevos que nos falten |
| `xbox_stuff` | 11 | 2026-08-20 | dbalatoni13 | viva, sin mergear | **solo simbolos y splits de X360** (`config/EUROPEGERMILESTONE/symbols.txt` +5760 lineas). 0 ficheros nuevos |
| `ps2_fixes` | 209 | 2026-08-18 | dbalatoni13 (100%) | viva, sin mergear | matching de A124 (PS2): EAXSound/CARSFX (27 ficheros), World (13), TrackStreamer, memoria. Ademas `symbols/PS2/PS2_functions.nothpp` reescrito (+527k lineas) y `tools/map2splat_a124.py` (nuevo) |
| `csis` | 50 | **2026-09-05** | dbalatoni13 | **la mas viva sin mergear** | librerias de middleware: `rcmp`, `vp6`, `realmemcard`, `path`, `spch`, `realcore`, `csis`. 86 ficheros nuevos |
| `eaxsound-clean` | 1169 | 2026-08-10 | dbalatoni13 | rama larga de trabajo | limpieza de zEAXSound; 5 ficheros nuevos que no tenemos: `tools/dwarf_batch.py`, `tools/source_guard.py`, `Src/Speech/SpeechContainers.cpp`, `Src/Speech/MiscSpeechTypes.h` |
| `bit_of_ecstasyy` | 566 | 2026-08-07 | dbalatoni13 | rama larga de trabajo | **el nombre engana**: es `zOnline` + `DebugDraw`, no Ecstasy. 31 ficheros nuevos que NO tenemos |

### Lo que cada rama tiene que a nosotros nos falta (por nombre de fichero, no por ruta)

- **`bit_of_ecstasyy` — 31 ficheros, el filon mas grande.** Area entera que nosotros no hemos tocado:
  `src/Speed/Indep/Src/Online/` (BitStream, BuddyCore, ConditionSimulator, FloatQuantizer, IntQuantizer,
  Lobby{Account,Chat,Core,GameSessions,Games,Login,Ranks,Users}, NetworkCore, NetworkMutex, SmartBitstream,
  VoiceCore, WebOfferUG2), `Online/InGame/` (CSCommon, Client, OnlineGame, OnlinePlayer, OnlinePlayerMgr, Server),
  `Src/Debug/Common/DebugDraw.{cpp,h}`, `Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h`,
  `Src/Physics/Behaviors/RBRemote.cpp`, `src/Speed/PSX2/Src/Online/PS2Isp.hpp`.
- **`csis` — 5 ficheros por nombre** (`rcmp/dev/include/rcmp/rcmp.h`, `rcmp2real.cpp`,
  `rcmp_codec_internal.h`, `spch/dev/include/spch/spchlib.h`, `vp6/1.0.6/include/vp6_pbdll.h`),
  pero ademas **72 rutas nuevas** porque la rama **REUBICA** las librerias:
  nosotros tenemos `src/Packages/vp6`, `src/Packages/realmemcard` y `src/egami/rcmp`;
  ellos las mueven a `src/Speed/Indep/Libs/{vp6,realmemcard,rcmp}`. Esto es un choque de rutas a resolver
  antes de portar nada de esa rama.
- **`dev`/`zFE-dev` — 18 ficheros de frontend** que ni `main` ni nosotros tenemos por nombre:
  `FEng/{FEResourceRequest.h,cFEng.h,fengine_full.h}`, `Frontend/HUD/FeDragTachometer.{cpp,hpp}`,
  `Frontend/MemoryCard/RealmcIface.hpp`, `Frontend/MenuScreens/Common/{FEInputWidget,IconPanel,IconScroller,IconScrollerMenu,UIWidgetMenu}.hpp`,
  `.../Safehouse/customize/{CustomizeEntryPoint,CustomizeTypes}.hpp`, `Frontend/MoviePlayer/MoviePlayer_new.cpp`,
  `Frontend/cFEngRender.hpp`, `World/CarPart.hpp`. (`FeAutoSaveIcon.{cpp,hpp}` si llego a `main` y tambien nos falta.)
- **`eaxsound-clean` — 4 utiles**: `tools/dwarf_batch.py`, `tools/source_guard.py`,
  `Src/Speech/SpeechContainers.cpp`, `Src/Speech/MiscSpeechTypes.h`.
- **`ps2_fixes` — 1**: `tools/map2splat_a124.py` (539 lineas).
- **`xbox_stuff`, `physicsdev`, `pr-117` — 0 ficheros nuevos** que nos falten; su valor es contenido, no estructura.

### Donde estamos por delante

Comparando volumen de fuente (bytes) en `HEAD` contra `upstream/main`:

| Area | Nuestro | Upstream |
|---|---|---|
| `Src/Frontend` | 2.405.416 B | 2.273.457 B |
| `Src/FEng` | 431.368 B | 374.948 B |
| `Src/World` | 1.655.111 B | 1.530.093 B |
| `Src/EAXSound` | 1.553.717 B | 1.399.560 B |
| `Src/Physics` | 979.725 B | 665.917 B |
| `Src/Ecstasy` | 404.087 B | 285.453 B |
| `Src/AI` | 628.147 B | **649.429 B** |

Y arboles enteros que upstream **no tiene**: `src/libc/`, `src/egami/`, `src/Packages/vp6/`,
`src/Runtime.PPCEABI.H/`, mas las bibliotecas `rcmp`/`vp6`/`realmemcard` configuradas en nuestro `configure.py`
(que en upstream solo existen en la rama `csis`, y con otra ruta).

**Conclusion de la seccion 1:** el flujo util es **upstream -> nosotros en infraestructura y portabilidad**,
y **nosotros -> upstream en fuente**, salvo `zOnline`/`DebugDraw` (rama `bit_of_ecstasyy`) y `zAI`,
donde ellos van por delante.

---

## 2. Los 56 commits que nos faltan de `upstream/main`

52 commits normales + 4 merges. Agrupados por tema:

### A. El frontend (23 commits, 2026-04 a 2026-09) — entra por el merge `80081647`

`f170941e` *most of the Frontend* (399 ficheros, +69.763) -> `64500339` *Get the frontend to compile* ->
la tanda de "un-slop" de Noxis (`e4b89d55`, `e8511780`, `0bcf1c71`, `0b6efaea`, `f4e8d911`, `81f0aae1`;
~46.800 lineas tocadas) -> `d402363d` *Fix PS2 compilation in most of FE* -> `f856544b` *fix typdef enums* ->
`4960cfcf` zFe, `62eefd30` zFEng, `a557233c` zFe2, `5e84080c` zFeOverlay -> `9b8d9354` *shorten decl comments* ->
`1c96a243`, `cca26933`.

**Prioridad: BAJA para portar tal cual** — nuestro `Src/Frontend` ya es mayor en bytes y lleva 1314 commits
de trabajo propio encima. Portar esto seria un merge de 1872 ficheros. Lo que si vale es el **subconjunto de
portabilidad** (grupo C) y los 18 ficheros que nos faltan por nombre.

### B. Matching de zAI / zWorld / zEAXSound (12 commits, 2026-09-03)

`902fdd1e` *Match most of zAI* (+7.784), `12807ec9`, `763962aa` *Apply carbon knowledge to zAI*,
`9a424c82`, `c8475951` *Make UCrc32 conversions explicit*, `5a8ee7f3` *Apply != nullptr*, `8ecc0813` *use this->*,
`f6eb41b3` *Apply this-> to zWorld, zWorld2 y zTrack*, `a15fe5d3` *zWorld2 match improvements*,
`40c73527` *Match all of zEAXSound except FE*, `0623bdce` *Make some SND TUs match again*,
`326e2c32` + `3bb4ab24` (PRs de jellejurre).

**Prioridad: MEDIA, y solo `zAI`.** Es la unica area donde upstream tiene mas bytes que nosotros.
Los commits de estilo (`this->`, `!= nullptr`, `UCrc32` explicito) son cosmeticos pero **pueden cambiar el
matching**: no portarlos a ciegas sin medir.

`39a8866a` *Get rid of hash generation for now and use the ones from carbon* borra
`Libs/Support/Miscellaneous/StringHash.h` (37 lineas) y sustituye el hash en tiempo de compilacion por
constantes tomadas de Carbon en `Texture.cpp`, `eLight.cpp`, `Sun.cpp`. **Decision nuestra pendiente**:
va en contra de lo que dice su propio README (*"matching macros, inlines, and static constants"*).

### C. Portabilidad multiplataforma — **ESTE ES EL GRUPO QUE NOS INTERESA** (8 commits)

| Commit | Que hace | Ficheros |
|---|---|---|
| `c28db606` **Fix MSVC compilation errors** | `EA_PLATFORM_GAMECUBE` -> `NATIVE_ENDIAN_BIG` en los 8 `bPlatEndianSwap` de `bWare.hpp`; `#include <cstring>`; `eSolidPlat.hpp` nuevo para PC y Xenon (67 lineas cada uno); `RenameSectionsFor()` en `configure.py` para que `.over` solo se renombre en GC/PS2 | 13 |
| `13189413` **Add the 1.3 PC version** (PR #126) | `Platform.WIN32`, `config/SPEED_EXE_1_3/`, splits de Xenon mejorados, `+5`/`+4` lineas de guardas en 19 SourceLists | 72 |
| `089f7d5f` **Port some frontend symbols to PC** | `config/SPEED_EXE_1_3/symbols.json` (4130 lineas cambiadas) + 4 cabeceras (`EAXSoundTypes.h`, `GameFlow.hpp`, `CarInfo.hpp`, `SFXObj_Pathfinder.cpp`) | 6 |
| `74871635` **Fix xbox compilation errors** | 2 lineas | 1 |
| `104f3e88` **Fix PS2 compilation error** | 3 lineas | 1 |
| `1006ecfc` **Split the PS2 version properly + PS2 release build** | anade `SLUS-21351`; introduce `INCLUDE_ASM_USE_MACRO_INC` en `include/include_asm.h` | 11 |
| `feccdb55` **PS2 asm generation + path fix on Windows** | en `tools/project.py`: genera un `.o` por subsegmento de splat, y pasa las rutas PS2 con `/` porque el preprocesador viejo de EE-GCC no deriva bien el directorio de los includes entrecomillados con `\` | 1 |
| `bfd70240` **Fix path capitalization** | 1 linea | 2 |

**Prioridad: MAXIMA.** Es exactamente el objetivo (b) del usuario.

### D. Infraestructura y herramienta (5 commits)

- `917dc4cf` **feat(dev): add devcontainer (#137)** — 3 ficheros nuevos + 14 lineas de README. **Prioridad ALTA**.
- `5653517c` *Add pyelftools to requirements* — ya lo tenemos.
- `1eb71b7c` *Make dwarf-compare faster + auto-run de la extension* — `dwarf-compare.py` +113/-13
  (cache SQLite de bloques de funcion) y extension de VS Code a 0.1.1 con `nfsmwMatch.autoRefresh`. **Prioridad MEDIA**.
- `03d15d8c` *Minor cleanups*, `a79a867b` *Delete "// File: ..." comments* (97 ficheros, -102), `bb5b1888`.
  **Prioridad BAJA** (cosmetico).

### E. Documentacion (2 commits) — **la directriz que seguimos**

`b53eda83` *chore(docs): add SPEED_EXE_1_3 to supported versions* deja la lista de versiones asi:

```
- `GOWE69`: Rev 0 (GC USA)
- `EUROPEGERMILESTONE`: Oct 21, 2005 prototype (Xbox 360 PAL)
- `SLES-53558-A124`: Sep 20, 2005 prototype (Alpha 124) (PS2 PAL)
- `SLUS-21351`: Black Edition (PS2 USA)
- `SPEED_EXE_1_3`: PC version 1.3
```

`f5c64fd9` *chore(docs): add no slop section to readme*. **Transcripcion integra de la seccion**:

> # SAY NO TO SLOP
>
> We expect that as we progress on this decompilation, vibecoders will attempt to take what we have and slop out a broken port before us. Please do not give sloppers attention. Any releases and ports will be available [here](https://github.com/RevEngin3) when they are ready.
>
> LLMs have been used on this project. Here's why, dispite this, our project isn't slop.
>
> - LLMs are **ONLY** used as a rough pass to get code somewhere in the ballpark that we can then reference or to create tooling. All LLM code must go through extensive manual review to clean it up to the point of being usable. **No slop is allowed to be merged into main.** If we accepted slop we could have been "done" months ago. Raw LLM output is unacceptable due to the mistakes it makes, such as:
>
>   - Manually assigning registers to variables to force a fake match
>   - Accessing struct members by pointer offsets
>   - Replacing *every* string and float value with something completely random
>   - Renaming parameters and variables randomly
>   - Reimplementing the same code multiple times and using include guards to fix the compile errors
>   - Redeclaring every referenced function at the top of each file, often incorrectly
>   - Casting variables to the type they already are
>
> - We are matching compiled machine code *and* debug symbols against multiple versions of the game accross multiple platforms, as well as other games in the series with shared code. To match all at once the code *must* be accurate to the original. This project has a large focus on accuracy to what the original code might have been. This includes matching macros, inlines, and static constants stripped from usage sites.
>
> - No feature or porting work is to be done until we have a *near-perfect matching* base to work against, and no AI will be used for this work.
>
> As long as this canary remains here, **NO AI has been used for any feature or porting work.**

**Lectura directa para nosotros:** el primer punto de la lista de errores inaceptables es literalmente
*"Manually assigning registers to variables to force a fake match"* — los 101 `register T x asm("rN")`
del arbol. Eso convierte el objetivo (a) del usuario en un **requisito de la politica oficial**, no en una
preferencia. El segundo punto, *"Accessing struct members by pointer offsets"*, tambien merece auditoria.

---

## 3. Infraestructura: contenido exacto de upstream y que cambiar

### 3.1 `.github/workflows/build.yml` — upstream vs nosotros

Upstream tiene **5 versiones** en la matriz y nosotros **3**. El resto del fichero es **identico**.
Las diferencias son exactamente tres:

```diff
--- .github/workflows/build.yml (nuestro)
+++ .github/workflows/build.yml (upstream/main)
@@ matriz
-        version: [GOWE69, EUROPEGERMILESTONE, SLES-53558-A124]
+        version: [GOWE69, EUROPEGERMILESTONE, SLES-53558-A124, SLUS-21351, SPEED_EXE_1_3]

@@ dependencias de PS2
     - name: Install PS2 dependencies
-      if: ${{ matrix.version == 'SLES-53558-A124' }}
+      if: ${{ matrix.version == 'SLES-53558-A124' || matrix.version == 'SLUS-21351' }}
       run: pip install --break-system-packages -r requirements.txt

@@ seleccion de binutils
-        if [[ "${{ matrix.version }}" == 'SLES-53558-A124' ]]; then
+        if [[ "${{ matrix.version }}" == 'SLES-53558-A124' || "${{ matrix.version }}" == 'SLUS-21351' ]]; then
           BINUTILS="/mips_binutils"
         else
           BINUTILS="/ppc_binutils"
         fi
```

Lo demas del workflow es igual en los dos: contenedor `ghcr.io/dbalatoni13/nfs-gc-build:main`,
`actions/checkout@v4` con submodulos, `git config --global --add safe.directory`, `cp -R /orig .`,
`python configure.py --map --version X --binutils $BINUTILS --compilers /compilers`,
`ninja all_source progress build/$V/report.json`, el paso de DWARF **solo** en `GOWE69` y solo en `push`
(`dtk dwarf dump` + `split_dwarf_info.py` + `generate-dwarf-report.py`), y las tres subidas de artefactos
(`*_maps`, `GC_Dwarf_report`, `*_report`).

**Ojo:** `SPEED_EXE_1_3` cae en la rama `else` y recibe `/ppc_binutils`. Es inocuo: la plataforma
`WIN32` no usa binutils. No hay que anadir un tercer caso.

**Pero el workflow solo funcionara con 5 versiones si antes existen**: `config/SPEED_EXE_1_3/` (3.4),
`Platform.WIN32` en `tools/project.py`, el bloque `WIN32` de `configure.py`, la herramienta `delink`,
y un `Win32/7.1` dentro del `/compilers` del contenedor.

### 3.2 `.devcontainer/Dockerfile` (contenido exacto, 20 lineas — no lo tenemos)

```dockerfile
FROM mcr.microsoft.com/devcontainers/cpp:2-debian13

RUN apt-get update && apt-get upgrade -y && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends python-is-python3 python3-pip libwayland-client0 libxkbcommon0 clangd mesa-utils libfontconfig


# Install latest objdiff
ADD --chmod=755 https://github.com/encounter/objdiff/releases/latest/download/objdiff-linux-x86_64 /usr/local/bin/objdiff

# Make python work by creating venv
RUN apt install -y python3-venv
RUN python3 -m venv /venv
ENV VIRTUAL_ENV=/venv
ENV PATH=/venv/bin:$PATH

# Copying ../requirements.txt because it can't be referenced directly
RUN python -m pip install pyelftools sortedcontainers pyyaml "splat64[mips]>=0.36.0,<1.0.0" "spimdisasm>=1.36.1,<2.0.0" "rabbitizer==1.13.0" tqdm pygfxd

# Install nfsmw match extension
ADD --chmod=644 https://github.com/dbalatoni13/nfsmw/releases/latest/download/nfsmw-match-annotations.vsix /tmp/nfsmw-match-annotations.vsix 
```

**Dos notas:**
1. Esta **desincronizado con su propio `requirements.txt`** (`splat64>=0.36.0` y `rabbitizer==1.13.0` aqui;
   `>=0.50.0` y `==1.16.2` alli). Si lo adoptamos, conviene arreglarlo de paso.
2. No instala binutils ni compiladores: el devcontainer sirve para **editar y correr `objdiff`**,
   no para reproducir el build de CI. El build de CI usa el contenedor privado `nfs-gc-build`.

### 3.3 `.devcontainer/devcontainer.json` y `devcontainer-lock.json` (exactos)

```jsonc
// For format details, see https://aka.ms/devcontainer.json. For config options, see the
// README at: https://github.com/devcontainers/templates/tree/main/src/cpp
{
	"name": "C++",
	"build": {
		"dockerfile": "Dockerfile"
	},
	"features": {
		"ghcr.io/devcontainers-extra/features/ninja-asdf:2": {}
	},
	"customizations": {
		"vscode": {
			"extensions": [
				"llvm-vs-code-extensions.vscode-clangd",
				"/tmp/nfsmw-match-annotations.vsix"
			]
		}
	},
	"runArgs": [
		"--device",
		"/dev/dri"
	]
	// Features to add to the dev container. More info: https://containers.dev/features.
	// "features": {},
	//
	// Use 'forwardPorts' to make a list of ports inside the container available locally.
	// "forwardPorts": [],
	//
	// Use 'postCreateCommand' to run commands after the container is created.
	// "postCreateCommand": "gcc -v",
	//
	// Configure tool-specific properties.
	// "customizations": {},
	//
	// Uncomment to connect as root instead. More info: https://aka.ms/dev-containers-non-root.
	// "remoteUser": "root"
}
```

```json
{
  "features": {
    "ghcr.io/devcontainers-extra/features/ninja-asdf:2": {
      "version": "2.0.16",
      "resolved": "ghcr.io/devcontainers-extra/features/ninja-asdf@sha256:679ad4a4410c3851ef3cfec684b976a89198b9367401f3912911a34c31b8d49b",
      "integrity": "sha256:679ad4a4410c3851ef3cfec684b976a89198b9367401f3912911a34c31b8d49b"
    }
  }
}
```

Los cuatro ficheros de `.vscode/` (`extensions.json`, `launch.json`, `settings.json`, `tasks.json`)
son **identicos** a los nuestros: no hay nada que portar ahi.

### 3.4 Como definen la version de PC (`SPEED_EXE_1_3`) en su `configure.py`

```python
# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GOWE69",              # 0
    "EUROPEGERMILESTONE",  # 1
    "SLES-53558-A124",     # 2
    "SLUS-21351",          # 3
    "SPEED_EXE_1_3",       # 4
]
```

(el nuestro tiene los cuatro primeros; falta el indice 4)

Seleccion de plataforma y herramientas:

```python
config.compilers_tag = "20260903"          # el nuestro: "20251015"

if version_num in [0]:
    config.platform = Platform.GC_WII
    config.dtk_tag = "v1.8.32"             # el nuestro: v1.8.31
    config.binutils_tag = "2.42-1"
elif version_num in [1]:
    config.platform = Platform.X360
    config.dtk_tag = "v0.1.2"
    config.binutils_tag = "2.42-1"
elif version_num in [2, 3]:
    config.platform = Platform.PS2
    config.binutils_tag = "2.45"
elif version_num in [4]:
    config.platform = Platform.WIN32
    config.delink_tag = "v0.16.1"          # herramienta NUEVA

config.objdiff_tag = "v3.7.0"              # el nuestro: v3.8.0 (vamos por delante)
config.sjiswrap_tag = "v1.2.2"             # el nuestro: v1.2.0
if config.platform == Platform.GC_WII:     # sjiswrap segfault
    config.wibo_tag = "1.1.0"
else:
    config.wibo_tag = "1.2.0"              # el nuestro: 1.1.0 siempre
```

Enlace y compilacion de WIN32:

```python
elif config.platform == Platform.WIN32:
    config.ldflags = [
        "/NODEFAULTLIB",
        f"/PDB:./build/{config.version}/{config.version}.pdb",
        f"/DEBUG",
    ]
```

```python
elif config.platform == Platform.WIN32:
    config.linker_version = "Win32/7.1"

    cflags_base_prodg = [
        "/nologo", "/c",
        "/wd4996",   # string deprecation
        "/wd4355",   # 'this' en inicializador de base
        "/Ox",
        "/Z7",       # debug info por .obj
        "/EHsc",
        f"/I {compilers_path / config.linker_version / 'Include'}",
        "/I src/Packages", "/I src",
        "/DEA_PLATFORM_WIN32", "/D_USE_MATH_DEFINES",
        f"/I build/{config.version}/include",
        f"/DBUILD_VERSION={version_num}",
        f"/DVERSION_{config.version}",
    ]
    config.context_defines = ["EA_PLATFORM_WIN32", "EA_REGION_AMERICA", "_USE_MATH_DEFINES", "_WIN32"]
    cflags_game = [*cflags_base_prodg, "/DLUA_NUMBER=float", "/DDEFAULT_ALLOCATOR=0",
                   "/I src/Speed/Indep/Libs/allocator/1.5.0", ... ]
    config.extra_clang_flags = ["-std=c++98", "-D_WIN32", "-D_WCHAR_T_DEFINED", "-fms-extensions"]
```

`config/SPEED_EXE_1_3/config.yml` (1.993 B) **no es un `.yml` de dtk**: apunta a `splits.json` +
`symbols.json` y lista los 29 SourceLists mas 30 huecos `unassigned_NN.s`:

```yaml
# This file is mainly needed so to provide the file extension (.c vs .cpp)
target_path: orig/SPEED_EXE_1_3/speed.exe
splits_path: config/SPEED_EXE_1_3/splits.json
symbols_path: config/SPEED_EXE_1_3/symbols.json
splits:
  - Speed/Indep/SourceLists/zAI.cpp
  ...
  - unassigned_00.s ... unassigned_29.s
```

Tamanos: `config.yml` 1.993 B, `splits.json` 13.111 B, **`symbols.json` 26.668.172 B** (26 MB — cuidado con el repo).

Y `tools/project.py` monta una regla de troceado distinta:

```python
elif config.platform == Platform.WIN32:
    n.rule(name="split",
           command="$python tools/delink_to_config_json.py $in $out_dir",
           description="SPLIT $in", depfile="$out_dir/dep", deps="gcc")
    n.build(inputs=[delink, config.config_path], outputs=build_config_path,
            rule="split", implicit=[splits_path, symbols_path],
            variables={"out_dir": build_path})
```

que por dentro llama a `delink ida-split symbols.json speed.exe --idapro splits.json -o build/.../obj`.
El binario `delink` se descarga de `https://github.com/dbalatoni13/delink` (entrada nueva `delink` en
`tools/download_tool.py`). En `generate_objdiff_config` anaden el mapeo `"Win32/7.1": "msvc7.1"`,
y `Platform.WIN32` comparte con `X360` la extension `.obj`, la regla `msvc` y el enlazador `link.exe`.

**Ademas, `tools/download_tool.py` cambia de donde salen los compiladores**:

```python
def compilers_url(tag: str) -> str:
    return f"https://github.com/dbalatoni13/compilers/releases/download/compilers_{tag}/compilers_{tag}.zip"
def msvc_url(tag: str) -> str:
    return f"https://files.decomp.dev/compilers_{tag}.zip"
```

y **desactivan `patch-toolchain.py`** (*"Not needed now because we download our custom compilers.zip"*).
Nosotros seguimos en `files.decomp.dev` + parcheo. Sin ese cambio no hay `Win32/7.1` en local
(en CI da igual: viene del contenedor).

> **AVISO — no tomar `tools/project.py` entero.** Nuestro `project.py` tiene tres arreglos que ellos
> **no** tienen y que perderiamos con un `git checkout upstream/main -- tools/project.py`:
>
> 1. `keep.lst` declarado como dependencia implicita del enlace. Sin el, cambiarlo no reenlaza y
>    **el DOL sale corto en silencio**.
> 2. `splits.txt`/`symbols.txt` declarados como dependencias del troceado, y `depfile=None` en la regla
>    `split` de dtk (porque `dtk dol split` no escribe depfile y con uno declarado el arco queda sucio
>    para siempre: bucle SPLIT -> configure).
> 3. La regla `prodg_rename` + `make_section_renames()` para el renombrado de seccion por objeto.
>
> Lo mismo en `tools/download_tool.py` (nuestro "skip download si ya esta extraido", para uso concurrente)
> y en `tools/rename_section.py` (el nuestro es superconjunto: 148 lineas contra 61, con `<old>=<new>` y `-o`).
> La operacion correcta es un **merge selectivo por hunk**, no una sustitucion.

### 3.5 `requirements.txt`

```diff
-splat64[mips]>=0.36.0,<1.0.0
+splat64[mips]>=0.50.0,<1.0.0
```

Unica diferencia. Lo demas coincide (`pyelftools` ya lo tenemos).

### 3.6 Los 40 ficheros que upstream tiene y nosotros no

```
.devcontainer/Dockerfile
.devcontainer/devcontainer-lock.json
.devcontainer/devcontainer.json
config/SPEED_EXE_1_3/config.yml
config/SPEED_EXE_1_3/splits.json
config/SPEED_EXE_1_3/symbols.json
orig/SLUS-21351/.gitkeep
orig/SPEED_EXE_1_3/.gitkeep
src/Speed/GameCube/Src/Ecstasy/eSolidPlat.hpp
src/Speed/GameCube/Src/Logitech/LGWheels.hpp
src/Speed/Indep/Libs/Support/Utility/UTLSequencer.h
src/Speed/Indep/Libs/endian/0.5.2/include/endian/big/endian.h
src/Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/filesys_cc.h
src/Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/std/memory.h
src/Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSLiveLink.hpp
src/Speed/Indep/Src/EAXSound/EAXEnvironment.cpp
src/Speed/Indep/Src/EAXSound/EAXEnvironment.hpp
src/Speed/Indep/Src/EAXSound/RealStream/src/stream.cpp
src/Speed/Indep/Src/FEng/FEImage.h
src/Speed/Indep/Src/FEng/FEngine.h
src/Speed/Indep/Src/Frontend/FEReflected.hpp
src/Speed/Indep/Src/Frontend/FEngHashes/FEHash_Credits.hpp
src/Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp
src/Speed/Indep/Src/Frontend/HUD/FeAutoSaveIcon.cpp
src/Speed/Indep/Src/Frontend/HUD/FeAutoSaveIcon.hpp
src/Speed/Indep/Src/Generated/FEngHash/FEHash_FeArrayScroller.hpp
src/Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp
src/Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp
src/Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp
src/Speed/Indep/Src/Generated/LanguageHashes.hpp
src/Speed/PC/Src/Ecstasy/TextureInfoPlat.hpp
src/Speed/PC/Src/Ecstasy/eLightPlat.hpp
src/Speed/PC/Src/Ecstasy/eSolidPlat.hpp
src/Speed/PC/Src/xSparks.h
src/Speed/PSX2/Src/Ecstasy/eSolidPlat.hpp
src/Speed/Xenon/Src/Ecstasy/eSolidPlat.hpp
src/Speed/Xenon/Src/xSparks.h
tools/apply_objdiff_mappings_delink.py
tools/delink_to_config_json.py
tools/ida_import_names.py
```

Ojo: `src/Speed/PC/` **no existe** en nuestro arbol (tenemos `PSX2`, `Xenon`, `GameCube`, `Indep`).
Y `src/Speed/Indep/Libs/endian/` tampoco.

---

## 4. Como tratan ellos el asm — la prueba de si el proyecto oficial admite pines

### 4.1 Medida en upstream

```
git grep -nE "register[^;]*asm\(|__asm__\(\"\"" upstream/main -- src   ->  0 resultados
git grep -nE 'asm\s*(volatile\s*)?\(\s*""'       upstream/main -- src   ->  0 resultados
```

**CERO pines de registro y CERO barreras vacias en todo `src/` de `upstream/main`.**

Lo unico que hay son **75 lineas de asm legitimo de plataforma**, todas intrinsecos reales que emiten
instrucciones:

- `src/Speed/Indep/bWare/Inc/bMath.hpp`: `frsqrte`, `fmuls`, `fnmsubs`, `fsel`, `fabs` (PPC) y
  `abs.s`, `lqc2 vf1`, `por` (VU de PS2) — el `bMath` original.
- `src/Speed/GameCube/bWare/GameCube/dolphinsdk/include/libc/math.h`: `frsqrte`.
- `src/Speed/GameCube/bWare/GameCube/SN/include/libsn.h`: `snPause()` / `snPollHost()` / `snFlushTtyBuffer()` (`.long`).
- `.../SN/include/machine/fastmath.h`: `__asm__("_f_atan2")` etc. — **alias de simbolo**, no pines.
- `stlport/stl/_threads.h` y `stlps2/stl_alloc.h`: `asm("sync")`, `membar`.

En `include/` solo aparecen los cuatro `__asm__` del propio `include_asm.h`.

**Conclusion: el proyecto oficial NO admite pines de registro ni barreras vacias.** Y el README
(seccion 2.E) lo dice explicitamente como el primer sintoma de "slop". Cualquier PR nuestro que los
lleve sera rechazado.

### 4.2 `include/include_asm.h`: nuestro fichero MODIFICADO ya es el de upstream

`git diff include/include_asm.h` (sin commitear) da:

```diff
+#if INCLUDE_ASM_USE_MACRO_INC
+__asm__(".include \"include/macro.inc\"\n");
+#else
 __asm__(".include \"include/labels.inc\"\n");
+#endif
```

Y el fichero resultante del working tree es **byte a byte identico** a `upstream/main:include/include_asm.h`.
O sea: **no es un cambio nuestro, es el commit `1006ecfc` de upstream aplicado a mano y sin commitear.**
Esta pendiente de commit, no de diseno. Es un port gratis.

### 4.3 Inventario del andamio que hay que quitar en NUESTRO arbol

| Construccion | Ocurrencias | Ficheros |
|---|---|---|
| `register T x asm("rN")` (pines) | **101** | 43 |
| `asm("")` / `__asm__("")` / `asm volatile("")` (barreras y cantidades fantasma) | **208** | 71 |
| **Total** | **309** | **86 ficheros distintos** |

Formas concretas de las barreras: `asm("" : "+r"(x))` x97, `asm("")` seco x26,
`asm("" : : "r"(x))` x23, `asm("" : : : "memory"/"r0")` x11, `asm("" : "=f"(x))` x8, y variantes.

**Reparto por area — y aqui esta el problema de portabilidad:**

| Area | Ficheros con andamio |
|---|---|
| `src/Speed/Indep` | **67** |
| `src/Speed/GameCube` | 7 |
| `src/LibSN` | 5 |
| `src/libc` | 3 |
| `src/egami/rcmp` | 2 |
| `src/Packages/{vp6,realmemcard}` | 2 |

**79 de los 86 ficheros estan en codigo compartido** (`Indep`, `Packages`, `egami`, `libc`, `LibSN`),
es decir, codigo que compilan tambien PS2, X360 y PC.

**Ninguno esta protegido por `#ifdef`**: comprobado en los 10 primeros ficheros, cero ocurrencias de
`EA_PLATFORM_GAMECUBE`/`__MWERKS__`/`__SN__` en el mismo fichero. Son sintaxis GCC (`register ... asm("rN")`,
extended asm) que **MSVC no compila en absoluto** — ni el de X360 ni el de Win32. Por eso hoy el
objetivo (b) es imposible sin el objetivo (a): la matriz de 5 versiones no puede pasar mientras esos
309 constructos esten en codigo `Indep`.

Top de ficheros por pines: `eLightE.cpp` (9), `FEngInterfaceFEObjects.cpp` (7), `CarRender.cpp` (6),
`TrackStreamer.cpp` (5), `WRoadNetwork.cpp` (5), `GManager.cpp` (4), `RawStateChan.cpp` (4), `EcstasyEx.cpp` (4).
Top por barreras: `CarLoader.cpp` (13), `WRoadNetwork.cpp` (12), `EcstasyEx.cpp` (8), `CarRender.cpp` (7),
`EmitterSystem.cpp` (6), `GTrigger.cpp` (5), `CARSFX_Roadnoise.cpp` (5), `JoyE.cpp` (5).

> Aviso de la memoria del proyecto que sigue vigente: `CullParts` es la **excepcion medida tres veces** —
> alli los andamios SOSTIENEN el reparto y quitarlos empeora. Y hay 13 "falsos 100" ya destapados en la r75
> por depineos previos. El depineo hay que medirlo fichero a fichero, no en bloque.

---

## 5. Herramientas suyas que no tenemos

Comparando `git ls-tree -r upstream/main --name-only | grep '^tools/'` (51 entradas) con
`git ls-files tools` (nuestras, sin `tools/scratch/`):

### Nos faltan 3 (todas de `upstream/main`)

| Fichero | Lineas | Para que sirve |
|---|---|---|
| `tools/delink_to_config_json.py` | 98 | **Imprescindible para la version de PC.** Lee `config/SPEED_EXE_1_3/config.yml`, invoca `delink ida-split symbols.json speed.exe --idapro splits.json -o build/.../obj` y genera el `config.json` v1.7.0 que consume el resto del build (`units`, `extract`, `modules`, `links`, `ldscript`). Requiere `pyyaml`. |
| `tools/apply_objdiff_mappings_delink.py` | 56 | La version "delink" de `apply_objdiff_mappings.py`: vuelca los `symbol_mappings` de `objdiff.json` sobre `config/<VERSION>/symbols.json` (JSON, no el `symbols.txt` de dtk). Salta objetivos `TODO` y bases `__save`/`__rest`. **Necesario para renombrar simbolos de PC/X360 desde objdiff.** |
| `tools/ida_import_names.py` | 196 | Script para IDA 9.x (`idat.exe -A -S"ida_import_names.py model.json" db.i64`). Lee el array `names` de un JSON estilo delink, compensa diferencias de image base y renombra solo lo que cambia. Util para el flujo de PC/X360, donde el analisis se hace en IDA y no en Ghidra. |

### Otras tres en ramas sin mergear

| Fichero | Rama | Para que sirve |
|---|---|---|
| `tools/map2splat_a124.py` | `ps2_fixes` | 539 lineas; genera splits de splat desde el MAP de A124. |
| `tools/dwarf_batch.py` | `eaxsound-clean` | procesado por lotes del dump DWARF. |
| `tools/source_guard.py` | `eaxsound-clean` | guardas de fuente (por el nombre, verificacion de que una TU no se rompio). |

### Ficheros comunes donde ellos han avanzado

| Fichero | Delta | Que hay |
|---|---|---|
| `tools/dwarf-compare.py` | +113/-13 | cache SQLite de bloques de funcion (`ensure_function_blocks_cache_db`) -> mucho mas rapido |
| `tools/vscode-nfsmw-match/` | 0.1.0 -> 0.1.1 | ajuste `nfsmwMatch.autoRefresh` (procesar automaticamente los documentos C/C++ al abrirlos) |
| `tools/download_tool.py` | +34 | entrada `delink`, `compilers_url` a su repo propio, `patch-toolchain` desactivado |
| `tools/project.py` | +205 | `Platform.WIN32`, `delink`, generacion de asm PS2, rutas POSIX en PS2 |
| `tools/apply_objdiff_mappings.py` | +9 | solo anade un contador y lo imprime |
| `tools/hasher.py` | -5 | ligado a `39a8866a` (quitar la generacion de hashes) |

### Herramientas nuestras que ellos no tienen

`tools/fnasm.py`, `tools/harvest_strings.py`, `tools/zcam_audit.py`, `tools/zlua_{annot,build,str}.py`
mas todo `tools/scratch/` (unos 2 M de lineas de datos de trabajo: `vedas_*`, `x360_sonda`, `gccasm_ret`).
Nuestro `tools/rename_section.py` es superconjunto del suyo.

**Nota de higiene:** si alguna vez se abre un PR hacia upstream, `tools/scratch/` (casi 2 millones de lineas,
incluido un `bench_main.s` de 1.239.700 lineas) **no debe viajar**. Ya esta documentado en la memoria del
proyecto que los commits locales arrastran scratch y hay que filtrar por fichero.

---

## QUE PORTAR / QUE MONTAR — lista priorizada

Todas las rutas son relativas a `C:\Users\jferr\Desktop\nfsdecompiled`. Nada de esto se ha ejecutado.

### P0 — Gratis, sin riesgo de matching (hacer ya)

1. **Commitear `include/include_asm.h`.** Ya esta modificado en el working tree y es **identico** al de
   upstream. Es el commit `1006ecfc` aplicado a mano. Cero riesgo.
   ```
   git add include/include_asm.h && git commit -m "include_asm: adoptar INCLUDE_ASM_USE_MACRO_INC de upstream (1006ecfc)"
   ```

2. **Devcontainer** (objetivo (b) del usuario, parte "entorno reproducible"):
   ```
   git checkout upstream/main -- .devcontainer/
   ```
   Luego **corregir la desincronizacion** del `Dockerfile`: `splat64[mips]>=0.50.0,<1.0.0` y
   `rabbitizer==1.16.2` para que case con `requirements.txt`.

3. **README: seccion SAY NO TO SLOP + las 5 versiones soportadas.** Es la directriz que seguimos
   (transcrita integra en la seccion 2.E de este informe).
   ```
   git show upstream/main:README.md > README.md   # o portar solo esas dos secciones
   ```

4. **`requirements.txt`**: una linea.
   ```diff
   -splat64[mips]>=0.36.0,<1.0.0
   +splat64[mips]>=0.50.0,<1.0.0
   ```

5. **Las 3 herramientas que nos faltan** (sin efecto sobre el build de GC):
   ```
   git checkout upstream/main -- tools/delink_to_config_json.py tools/apply_objdiff_mappings_delink.py tools/ida_import_names.py
   ```

6. **`tools/dwarf-compare.py` (cache SQLite) y extension de VS Code 0.1.1**:
   ```
   git checkout upstream/main -- tools/dwarf-compare.py tools/vscode-nfsmw-match/
   ```
   (revisar antes si tenemos cambios locales en `dwarf-compare.py`: el diff es +113/-13 y las 13 bajas
   podrian ser nuestras)

7. **`orig/SLUS-21351/.gitkeep`** (trivial, lo necesita la matriz).

### P1 — Portabilidad: lo que habilita las 5 versiones (objetivos (a) y (b))

8. **`.github/workflows/build.yml`: las 3 ediciones exactas de la seccion 3.1.**
   **No hacerlo antes que los pasos 9-13**, o la CI dara rojo en las dos versiones nuevas.
   Sugerencia: subir primero a **4 versiones** (anadir solo `SLUS-21351`, que ya tiene `config/`),
   y meter `SPEED_EXE_1_3` cuando este el paso 13.

9. **`c28db606` (Fix MSVC compilation errors)** — el commit mas rentable del lote:
   ```
   git show c28db606 -- src/Speed/Indep/bWare/Inc/bWare.hpp          # EA_PLATFORM_GAMECUBE -> NATIVE_ENDIAN_BIG (x8) + <cstring>
   git show c28db606 -- src/Speed/PC/Src/Ecstasy/eSolidPlat.hpp src/Speed/Xenon/Src/Ecstasy/eSolidPlat.hpp
   git show c28db606 -- configure.py                                  # RenameSectionsFor()
   ```
   `NATIVE_ENDIAN_BIG` hay que definirlo en `src/types.h` para GC, Xenon y PS2 (grande) y no para WIN32.

10. **`src/types.h`: merge a tres bandas.** Upstream separo el bloque unico `EA_PLATFORM_XENON` en dos
    (`XENON` + `WIN32`) y anadio `EA_PACKED`. Nosotros tenemos ademas un bloque `__ANDROID__` que ellos
    no tienen. **No sustituir el fichero**: aplicar el bloque `EA_PLATFORM_WIN32` nuevo, los `EA_PACKED`
    (`__attribute__((packed))` en GC/PS2, vacio en MSVC) y conservar `__ANDROID__`.
    ```
    git diff HEAD:src/types.h upstream/main:src/types.h
    ```

11. **`tools/project.py`: merge SELECTIVO** (ver el aviso de la seccion 3.4). Coger de upstream:
    - `Platform.WIN32 = 3` en el enum
    - `obj_extension = ".obj" if config.platform in [Platform.X360, Platform.WIN32] else ".o"`
    - `delink_tag` / `delink_path` en `ProjectConfig` y la regla de descarga
    - la rama `elif config.platform == Platform.WIN32:` del troceado (`delink_to_config_json.py`)
    - `"Win32/7.1": "msvc7.1"` en `generate_objdiff_config`
    - `--target=i686-pc-windows-msvc` en `generate_compile_commands`
    - `feccdb55` completo (asm generado de PS2 + rutas POSIX en PS2)

    **Conservar nuestro**: dependencia de `keep.lst`, dependencias `splits.txt`/`symbols.txt` en la regla
    `split` de dtk con `depfile=None`, regla `prodg_rename` + `make_section_renames()`.
    ```
    git diff HEAD:tools/project.py upstream/main:tools/project.py > /tmp/project.diff   # 205 lineas de delta
    ```

12. **`tools/download_tool.py`**: anadir `delink_url()` + entrada `"delink"`. Decidir aparte si cambiamos
    la fuente de compiladores a `github.com/dbalatoni13/compilers` (tag `20260903`) — hace falta para tener
    `Win32/7.1` en local, pero **cambia el compilador de GC** y eso puede mover el matching de todo el DOL.
    **Medir antes.** En CI no hace falta: `/compilers` viene del contenedor.
    Conservar nuestro "skip download si ya esta extraido".

13. **La version de PC completa** (`13189413` + `089f7d5f`):
    ```
    git checkout upstream/main -- config/SPEED_EXE_1_3/ orig/SPEED_EXE_1_3/.gitkeep
    git checkout upstream/main -- src/Speed/PC/                      # 4 ficheros, arbol nuevo
    ```
    y en `configure.py`: `VERSIONS += ["SPEED_EXE_1_3"]`, `elif version_num in [4]: Platform.WIN32 / delink_tag`,
    bloque `ldflags` WIN32, bloque `cflags_base_prodg` WIN32 (seccion 3.4).
    **Aviso: `config/SPEED_EXE_1_3/symbols.json` pesa 26 MB.** Considerar si entra al repo o se genera.

14. **DEPINEO — el objetivo (a).** 309 constructos en 86 ficheros, **79 de ellos en codigo compartido**.
    Es el bloqueante real de la matriz de 5 versiones: `register ... asm("rN")` y el extended asm de GCC
    **no compilan en MSVC**. Estrategia sugerida, en este orden:
    1. Los 67 ficheros de `src/Speed/Indep` primero (son los que rompen PS2/X360/PC).
    2. Medir fichero a fichero contra el ultimo commit (la memoria ya avisa: media funcion no es neutra).
    3. `CullParts` queda **fuera**: medido tres veces, alli los andamios sostienen el reparto.
    4. Los de `src/Speed/GameCube`, `src/LibSN` y `src/libc` (7+5+3) pueden esperar: no los compila
       ninguna otra version.

    Inventario para arrancar:
    ```
    git grep -cE "register[^;]*\basm\s*\(" -- src | sort -t: -k2 -rn
    git grep -cE '(__asm__|asm)\s*(volatile|__volatile__)?\s*\(\s*""' -- src | sort -t: -k2 -rn
    ```

### P2 — Contenido que ellos tienen y nosotros no

15. **`bit_of_ecstasyy` -> `zOnline` + `DebugDraw`: 31 ficheros de un area que no hemos tocado.**
    El mayor aporte neto de todas las ramas.
    ```
    git checkout upstream/bit_of_ecstasyy -- src/Speed/Indep/Src/Online/ src/Speed/Indep/Src/Debug/Common/DebugDraw.cpp src/Speed/Indep/Src/Debug/Common/DebugDraw.h
    ```
    (revisar tambien `configure.py` de esa rama para las entradas de `zOnline`/`zDebug`)

16. **`csis` -> cflags por biblioteca.** La rama define `cflags_path` (`-G8` + `-fstrength-reduce`),
    `cflags_realcore` (`-O2`, `-G8`, `-fno-force-addr`), `cflags_realmemcard` (`-fshort-wchar`,
    `-fstrength-reduce`, `-G8`, `-fno-force-addr`), `cflags_spch` (`-x c++`) y `cflags_vp6`
    (`-O2`, sin `-fforce-addr`, `-G8`, `-mno-ps-nodf`). **Comparar uno a uno con los nuestros** —
    la memoria del proyecto ya dice que los nuestros se copiaron en bloque de `snd` y que un porcentaje
    bajo en codigo escrito es sospecha de flags. Diferencia concreta detectada: ellos usan `-mno-ps-nodf`
    en vp6 y nosotros `-mps-float`.
    ```
    git diff upstream/main...upstream/csis -- configure.py
    ```
    Y 5 cabeceras/fuentes nuevas: `rcmp/dev/include/rcmp/rcmp.h`, `rcmp2real.cpp`,
    `rcmp_codec_internal.h`, `spch/dev/include/spch/spchlib.h`, `vp6/1.0.6/include/vp6_pbdll.h`.
    **Ojo con las rutas**: ellos usan `src/Speed/Indep/Libs/{rcmp,vp6,realmemcard}`, nosotros
    `src/egami/rcmp` y `src/Packages/{vp6,realmemcard}`.

17. **Los 18 ficheros de frontend de `dev`/`zFE-dev`** mas los 4 `FEngHash` y `LanguageHashes.hpp` de `main`:
    ```
    git checkout upstream/main -- src/Speed/Indep/Src/Generated/FEngHash/ src/Speed/Indep/Src/Generated/LanguageHashes.hpp src/Speed/Indep/Src/Frontend/FEngHashes/
    git checkout upstream/dev -- src/Speed/Indep/Src/FEng/cFEng.h ...   # uno a uno
    ```

18. **`zAI`**: la unica area donde upstream tiene mas bytes (649 kB contra 628 kB nuestros).
    Revisar `902fdd1e` y `763962aa` (*Apply carbon knowledge*) funcion a funcion.

19. **`tools/map2splat_a124.py`** (`ps2_fixes`), **`tools/dwarf_batch.py`** y **`tools/source_guard.py`**
    (`eaxsound-clean`) si vamos a trabajar PS2 o Speech.

### P3 — Decidir, no portar

20. **`39a8866a`** (quitar la generacion de hashes y usar las de Carbon): contradice su propio README.
    No portar sin decision explicita del usuario.
21. **`a79a867b`** (borrar los comentarios `// File: ...`, 97 ficheros): cosmetico, y nuestro arbol ha
    divergido mucho. No merece el ruido.
22. **Ramas fosiles**: `dev` y `zFE-dev` no hay que seguirlas; su contenido ya esta en `main`.
    `physicsdev` idem para la herramienta.

---

### Apendice — comandos de verificacion usados

```
git log --oneline HEAD..upstream/main                       # 56
git merge-base HEAD upstream/main                           # 59f184a0
git diff --name-only HEAD upstream/main -- src | wc -l      # 2149
git grep -nE "register[^;]*asm\(|__asm__\(\"\"" upstream/main -- src   # 0
git ls-tree -r upstream/main --name-only | grep -E "^(tools|scripts)/" # 51
git show upstream/main:.github/workflows/build.yml
git show upstream/main:.devcontainer/Dockerfile
git show upstream/main:configure.py | head -120
```

En Git Bash sobre Windows hace falta `export MSYS_NO_PATHCONV=1` antes de cualquier
`git show upstream/main:ruta/con/barras`, o MSYS convierte la ruta y `git` responde
*"ambiguous argument"*.
