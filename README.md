Need for Speed: Most Wanted Decompilation
[![Build Status]][actions] [![Code Progress]][progress] [![Data Progress]][progress] [![Discord Badge]][discord]
=============

[Build Status]: https://github.com/dbalatoni13/nfsmw/actions/workflows/build.yml/badge.svg
[actions]: https://github.com/dbalatoni13/nfsmw/actions/workflows/build.yml
[Code Progress]: https://decomp.dev/dbalatoni13/nfsmw.svg?mode=shield&measure=code&label=Code
[Data Progress]: https://decomp.dev/dbalatoni13/nfsmw.svg?mode=shield&measure=data&label=Data
[progress]: https://decomp.dev/dbalatoni13/nfsmw
[Discord Badge]: https://img.shields.io/discord/727908905392275526?color=%237289DA&logo=discord&logoColor=%23FFFFFF
[discord]: https://discord.gg/hKx3FJJgrV

A work-in-progress decompilation of the **GameCube**, Xbox 360 and PS2 versions of Need for Speed: Most Wanted. The focus is currently on the **GameCube** version.

This repository does **not** contain any game assets or assembly whatsoever. An existing copy of the game is required.

Supported versions:

- `GOWE69`: Rev 0 (GC USA)
- `EUROPEGERMILESTONE`: Oct 21, 2005 prototype (Xbox 360 PAL)
- `SLES-53558-A124`: Sep 20, 2005 prototype (Alpha 124) (PS2)

# Dependencies

## Windows

On Windows, it's **highly recommended** to use native tooling. WSL or msys2 are **not** required.  
When running under WSL, [objdiff](#diffing) is unable to get filesystem notifications for automatic rebuilds.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
  - Also available from the [Windows Store](https://apps.microsoft.com/store/detail/python-311/9NRWMJP3717K).
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

## macOS

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

[wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

## Linux

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).

[wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

# Building

- Clone the repository:

  ```sh
  git clone https://github.com/dbalatoni13/nfsmw.git
  ```

- Install dependencies (PS2 only)

  ```sh
  python -m pip install -r requirements.txt
  ```

- Configure:

  ```sh
  python configure.py
  ```

  To use a version other than `GOWE69` (USA), specify it with `--version`, for example `--version EUROPEGERMILESTONE`.

- Build:

  ```sh
  ninja
  ```

- Extracting the binaries
  - GC: Extract `NFSMWRELEASE.ELF`, copy it into `orig/GOWE69`, and convert it into a DOL using the following command:

    ```sh
    ./build/tools/dtk elf2dol ./orig/GOWE69/NFSMWRELEASE.ELF ./orig/GOWE69/sys/main.dol
    ```

  - Xbox 360: simply rename `NfsMWEuropeGerMilestone.exe` to `NfsMWEuropeGerMilestone.xex` and copy it to `./orig/EUROPEGERMILESTONE/`

  - PS2: Copy `NFS.ELF` to `./orig/SLES-53558-A124/`

- Sharing large assets across git worktrees

  If you use multiple git worktrees, you can deduplicate the large immutable inputs
  and downloaded tool binaries while keeping each worktree's generated build files
  separate:

  ```sh
  python tools/share_worktree_assets.py link --all
  ```

  This shares the ignored debug/tool assets under the git common directory, including
  extracted `orig/*` contents, `symbols/*`, and downloaded tool binaries under
  `build/`. It intentionally does **not** share `build.ninja`,
  `objdiff.json`, `compile_commands.json`, or per-worktree object outputs.

  After creating a fresh worktree, bootstrap its local generated files with:

  ```sh
  python tools/share_worktree_assets.py bootstrap
  ```

  `bootstrap` links the shared assets for the current worktree, runs `configure.py`,
  generates the local split config when needed, and reruns `configure.py` so fresh
  worktrees end up with `build.ninja`, `objdiff.json`, and `compile_commands.json`
  without manual copying.

# Diffing

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, `configure.py`, `splits.txt` or `symbols.txt`.

![](assets/objdiff.png)

# Contribution

## Ghidra

To get a proper Ghidra output, you should either install the necessary extensions or ask me for access to the shared Ghidra project on [decomp.dev](https://ghidra.decomp.dev/).

### Ghidra server

[Recommended Ghidra build](https://github.com/RootCubed/ghidra-ci/releases/download/2025-04-25/ghidra_11.4_DEV_20250425.zip)

### Manual setup

[Dwarf1 extension to properly load the GC debug info into Ghidra](https://github.com/emoose/ghidra-dwarf1)

[PS2 Ghidra extension](https://github.com/chaoticgd/ghidra-emotionengine-reloaded)

Unfortunately the vtables can't (currently?) be loaded from the GC ELF into Ghidra, so you'll have to copy them over from the PS2 ELF and adjust the corresponding classes. Nested classes have the same problem and solution.

#### Use Deprecated Demangler

For Gamecube binaries the standard demangler doesn't work and you have to use the deprecated version.
For PS2 binaries the deprecated version gives nicer results.

1. From the Ghidra project window, double-click the program file to open it in the CodeBrowser.
2. In the CodeBrowser, navigate to Analysis > Auto Analyze... (or press Shift + F12).
3. In the Auto Analysis options window, find the "Demangler GNU" analyzer in the list and select it.
4. In the options area (usually on the right side of the window), locate the option named "Use Deprecated Demangler".

## symbols/mw_dwarfdump.nothpp

```
./build/tools/dtk dwarf dump ./orig/GOWE69/NFSMWRELEASE.ELF -o ./symbols/mw_dwarfdump.nothpp
```

This is the dwarf dump of the whole GC version of the game. The `.nothpp` extension is to make sure that the IDE doesn't parse it on weak laptops. This should be your main source of information. It even shows which inlines a function calls. Namespaces only show up in generics. For regular functions and variables you can search `symbols.txt` for the right name.

## symbols/PS2

This folder contains the debug info dump of PS2 build alpha 124. It is useful for figuring out member visibility and the declaration order of virtual functions.

## symbols/debug_lines.txt

GameCube Address -> line mapping

## symbols/file_names.txt

GameCube file list. Headers that don't contain any inlines are not listed.

## ProStreet's PDBs

Another great source where you can see visibility and namespaces is the PDBs of the Xbox 360 builds of ProStreet. You can use [resym](https://resym.chimpsatsea.com/) to open them up in the browser.

## symbols/Classes

This folder contains AttribSys classes which were automatically generated by `tools/attrib_generator.py` of which we haven't been made sure yet that they can be included (perhaps they use structs that require imports).

## symbols/vlt.txt

This file contains the hashes used in AttribSys.

## symbols/hashes.txt

This file contains hashes used in different parts of the game. We are currently using a template solution to calculate the hash at compile time. But as you can see in the file, this is quite verbose, we'll want to find a better solution.

## symbols/bchunks.txt

This file contains bChunk chunk IDs.

# Guide for AI contributions

## Setup

- Run

  ```
  ./build/tools/dtk dwarf dump ./orig/GOWE69/NFSMWRELEASE.ELF -o ./symbols/mw_dwarfdump.nothpp
  python ./tools/split_dwarf_info.py ./symbols/mw_dwarfdump.nothpp ./symbols/Dwarf
  ```

- Set up the project and Ghidra as described above (take the Ghidra repo from the decomp.dev server, you'll have to request access).

- Import the ELF files from `orig/` into the Ghidra project so the program names stay
  `NFSMWRELEASE.ELF` and `NFS.ELF`:

  ```sh
  ghidra import ./orig/GOWE69/NFSMWRELEASE.ELF
  ghidra import ./orig/SLES-53558-A124/NFS.ELF
  ```

- Download [ghidra-cli](https://github.com/akiselev/ghidra-cli) and put it into your path.

- Tell ghidra-cli your Ghidra installation's path

  ```
  ghidra config set ghidra_install_dir <YOUR_PATH>/ghidra_11.4_DEV_20250425/ghidra_11.4_DEV
  ```

- Tell ghidra-cli the path to the Ghidra project

  ```
  ghidra config set ghidra_project_dir <PATH>
  ```

- Set NeedForSpeed as the default project

  ```
  ghidra config set default_project NeedForSpeed
  ```

- Set the GC version as the default program
  ```
  ghidra config set default_program NFSMWRELEASE.ELF
  ```

## Workflow

Just tell your favourite clanker to reference `AGENTS.md` to decompile a translation unit of your choice, for example `main/Speed/Indep/SourceLists/zEAXSound`.

When introducing or forward-declaring a type, preserve the original `class` / `struct`
kind. Check existing headers first with `python tools/find-symbol.py <TypeName>`, then use
GC Dwarf and PS2 type info when the real declaration is missing or incomplete.

Preserve real member names, types, order, and offset comments too. For recovered game
types, do not invent `pad`, `unk`, or `field_XXXX` members to force a guessed layout; use
the debug data and leave a short TODO when a field is still unresolved.

If a project type already has a header in `src/`, include that header instead of adding a
local forward declaration.

## Style tooling

The repo ships with a decomp-aware style helper:

```sh
python tools/code_style.py audit --base origin/main
```

Use `audit` to classify branch changes into safer vs match-sensitive buckets and to flag repo-specific issues such as jumbo include spacing, stray top-level declarations in `SourceLists` files, touched `class` / `struct` declarations that disagree with known headers or the PS2 visibility rule, touched project forward declarations that should be replaced by real includes, touched type members that look like invented padding or placeholder names, and touched style-guide issues that clang-format cannot fix for you (`using namespace`, `NULL`, bad cast spacing, or missing `EA_PRAGMA_ONCE_SUPPORTED` guard blocks).
Repeated findings are grouped by file so large branch audits stay readable.

Useful focused passes:

```sh
python tools/code_style.py audit --base origin/main --category safe-cpp
python tools/code_style.py audit --base origin/main --category match-sensitive-cpp
python tools/code_style.py format --check --base origin/main --category safe-cpp
```

If you have `clang-format` installed locally, you can also use:

```sh
python tools/code_style.py format --check --base origin/main
python tools/code_style.py format --check src/Speed/Indep/Src/Frontend/FEManager.cpp
```

The formatter wrapper targets eligible changed C/C++ files by default, including match-sensitive code. If you want a smaller focused pass, restrict it with `--category safe-cpp`, which currently maps to `src/Speed/Indep/Src/Frontend/` and `src/Speed/Indep/Src/FEng/`.
`format --check` now distinguishes whitespace-only formatter deltas from other non-whitespace output changes.
Files that use the repo's initializer-list guard comments (`//`) are formatter targets too. If a formatting pass touches match-sensitive code, rebuild and verify the affected unit afterwards instead of assuming the change is automatically byte-stable.
For declaration-kind checks, header declarations are treated as the repo source of truth; otherwise the helper falls back to the PS2 dump rule (`public:` / `private:` / `protected:` means `class`, no visibility labels means `struct`).

`clang-format` is optional. Recommended installs:

- macOS: `brew install clang-format`
- Linux: `sudo apt install clang-format`
- Windows: `winget install LLVM.LLVM`

If your binary lives outside `PATH`, set `CLANG_FORMAT` to the executable path before running `tools/code_style.py format`.

# Contributors

Special thanks to [Brawltendo](https://github.com/Brawltendo) for helping with tooling and letting me use his partial decomp.

Special thanks to [r033](https://github.com/r-033) and [Toru the Red Fox](https://github.com/TorutheRedFox) for sharing their knowledge with us.
