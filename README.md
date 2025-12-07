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

- Install [wine-crossover](https://github.com/Gcenx/homebrew-wine):

  ```sh
  brew install --cask --no-quarantine gcenx/wine/wine-crossover
  ```

After OS upgrades, if macOS complains about `Wine Crossover.app` being unverified, you can unquarantine it using:

```sh
sudo xattr -rd com.apple.quarantine '/Applications/Wine Crossover.app'
```

## Linux

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- For non-x86(\_64) platforms: Install wine from your package manager.
  - For x86(\_64), [wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

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

  - GC: Extract `NFSMWRELEASE.elf`, copy it into `orig/GOWE69` and convert it into a DOL using the following command:

    ```sh
    ./build/tools/dtk elf2dol ./orig/GOWE69/NFSMWRELEASE.elf ./orig/GOWE69/sys/main.dol
    ```

  - Xbox 360: simply rename `NfsMWEuropeGerMilestone.exe` to `NfsMWEuropeGerMilestone.xex` and copy it to `./orig/EUROPEGERMILESTONE/`

  - PS2: Copy `NSF.ELF` to `./orig/SLES-53558-A124/`

# Diffing

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, `configure.py`, `splits.txt` or `symbols.txt`.

![](assets/objdiff.png)

# Contribution

## Ghidra

To get a proper Ghidra output, you should either install the necessary extensions or ask me for access to the shared Ghidra project on [decomp.dev](https://ghidra.decomp.dev/).

[Recommended Ghidra build](https://rootcubed.dev/ghidra_builds/)

[Dwarf1 extension to properly load the GC debug info into Ghidra](https://github.com/emoose/ghidra-dwarf1)

[PS2 Ghidra extension](https://github.com/chaoticgd/ghidra-emotionengine-reloaded)

Unfortunately the vtables can't (currently?) be loaded from the GC ELF into Ghidra, so you'll have to copy them over from the PS2 ELF and adjust the corresponding classes. Nested classes have the same problem and solution.

### Use Deprecated Demangler

For Gamecube binaries the standard demangler doesn't work and you have to use the deprecated version.

1. From the Ghidra project window, double-click the program file to open it in the CodeBrowser.
2. In the CodeBrowser, navigate to Analysis > Auto Analyze... (or press Shift + F12).
3. In the Auto Analysis options window, find the "Demangler GNU" analyzer in the list and select it.
4. In the options area (usually on the right side of the window), locate the option named "Use Deprecated Demangler".

## symbols/mw_dwarfdump.nothpp

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

## symbols/bhhunks.txt

This file contains bChunk chunk IDs.

# Contributors

Special thanks to [Brawltendo](https://github.com/Brawltendo) for helping with tooling and letting me use his partial decomp.

Special thanks to [r033](https://github.com/r-033) and [Toru the Red Fox](https://github.com/TorutheRedFox) for sharing their knowledge with us.
