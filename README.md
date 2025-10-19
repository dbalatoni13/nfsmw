Need for Speed: Most Wanted (GC and Xbox 360)
[![Build Status]][actions] [![Code Progress]][progress] [![Data Progress]][progress] [![Discord Badge]][discord]
=============

[Build Status]: https://github.com/dbalatoni13/nfsmw/actions/workflows/build.yml/badge.svg
[actions]: https://github.com/dbalatoni13/nfsmw/actions/workflows/build.yml
[Code Progress]: https://decomp.dev/dbalatoni13/nfsmw.svg?mode=shield&measure=code&label=Code
[Data Progress]: https://decomp.dev/dbalatoni13/nfsmw.svg?mode=shield&measure=data&label=Data
[progress]: https://decomp.dev/dbalatoni13/nfsmw
[Discord Badge]: https://img.shields.io/discord/727908905392275526?color=%237289DA&logo=discord&logoColor=%23FFFFFF
[discord]: https://discord.gg/hKx3FJJgrV

A work-in-progress decompilation of the GameCube and Xbox 360 versions of Need for Speed: Most Wanted.

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

  - Xbox 360: simply `NfsMWEuropeGerMilestone.exe` to `NfsMWEuropeGerMilestone.xex` and copy it to `./orig/EUROPEGERMILESTONE/`

# Diffing

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, `configure.py`, `splits.txt` or `symbols.txt`.

![](assets/objdiff.png)

# Contributors

Special thanks to [Brawltendo](https://github.com/Brawltendo) for helping with tooling and letting me use his partial decomp.

Special thanks to [r033](https://github.com/r-033) and [Toru the Red Fox](https://github.com/TorutheRedFox) for sharing their knowledge with us.
