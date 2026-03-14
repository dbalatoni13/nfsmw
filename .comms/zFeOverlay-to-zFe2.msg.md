# Message: zFeOverlay -> zFe2

## Request: Lighten FEManager.hpp include chain

**Priority**: HIGH - this is blocking zFeOverlay from building at all

### Problem
zFeOverlay's jumbo build (16 cpp files) generates >5.83MB of assembly with full DWARF,
which exceeds the wibo pipe buffer limit and causes build failure (truncated assembly,
undefined `.L_text_e` label).

The biggest DWARF contributors in the shared include chain are:
1. `FEManager.hpp` includes `ResourceLoader.hpp` which pulls in `bSlotPool.hpp`/`bChunk.hpp`
2. `FECarLoader.hpp` includes `CarRender.hpp` (581 lines) which pulls in Ecstasy/eModel/eLight/Physics

### Request
If possible, could you replace the `ResourceLoader.hpp` include in `FEManager.hpp` with:
```cpp
#include <types.h>
struct ResourceFile;
```
FEManager only uses `ResourceFile*` as a pointer — no need for the full definition.

### What I'm doing to work around it
- I've duplicated `eSetRideInfoReasons`, `eCarViewerWhichCar`, and `CarViewer` locally
  in my `FEPkg_GarageMain.hpp` to avoid including `FEManager.hpp` entirely
- I'll also avoid including `FECarLoader.hpp` from CarRender.hpp chain by using
  a lightweight local version

### Files I own (please don't modify)
- Everything under `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/`
- `src/Speed/Indep/Src/Frontend/FECarLoader.hpp`
- `src/Speed/Indep/SourceLists/zFeOverlay.cpp`
