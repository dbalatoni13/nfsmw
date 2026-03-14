# zFeOverlay Agent Status

## Current State
- **Match**: 0% (0/467 functions)
- **Target**: 90%+
- **Working on**: Fixing DWARF truncation build blocker

## Files Being Edited
- src/Speed/Indep/SourceLists/zFeOverlay.cpp (jumbo include list)
- src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.cpp
- src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp
- src/Speed/Indep/Src/Frontend/FECarLoader.hpp
- src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/*.cpp and *.hpp
- src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/*.cpp and *.hpp
- src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiMarkerSelect.*

## Critical Blocker
The wibo pipe buffer limit (~5.83MB) truncates assembly output for this TU.
With all 16 includes and full DWARF (-gdwarf+), the assembly exceeds the limit.
Heavy include chains (FEManager.hpp -> ResourceLoader.hpp, FECarLoader.hpp -> CarRender.hpp)
are the biggest contributors. I'm working around this by duplicating needed enums/types
locally instead of including the heavy shared headers.

## Recent Progress
- All headers scaffolded from DWARF
- ~27 function bodies written
- Build still fails due to DWARF truncation

## Notes
- zFeOverlay is a jumbo build with 16 cpp includes (Safehouse screens)
- 467 total functions, ~141KB .text
- Please do not modify files listed above
