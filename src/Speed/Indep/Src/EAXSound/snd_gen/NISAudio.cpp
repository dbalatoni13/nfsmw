#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
InterfaceId NIS_Select_StartId = {"NIS_Select_Start", 0x0BE4, 0x67D1};
InterfaceId SoundFX_SelectId = {"SoundFX_Select", 0x0BE4, 0x2052};
InterfaceId NIS_Select_BlacklistId = {"NIS_Select_Blacklist", 0x0BE4, 0x3B39};
InterfaceId NIS_Select_EndId = {"NIS_Select_End", 0x0BE4, 0x4888};

FunctionHandle gNIS_Select_StartHandle;
FunctionHandle gSoundFX_SelectHandle;
FunctionHandle gNIS_Select_BlacklistHandle;
FunctionHandle gNIS_Select_EndHandle;
} // namespace Csis
