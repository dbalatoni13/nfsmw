#include "Speed/Indep/Src/EAXSound/Csis.hpp"

static const char kNIS_Select_StartName[] = "NIS_Select_Start";
static const char kSoundFX_SelectName[] = "SoundFX_Select";
static const char kNIS_Select_BlacklistName[] = "NIS_Select_Blacklist";
static const char kNIS_Select_EndName[] = "NIS_Select_End";

namespace Csis {
InterfaceId NIS_Select_StartId = {kNIS_Select_StartName, 0x0BE4, 0x67D1};
InterfaceId SoundFX_SelectId = {kSoundFX_SelectName, 0x0BE4, 0x2052};
InterfaceId NIS_Select_BlacklistId = {kNIS_Select_BlacklistName, 0x0BE4, 0x3B39};
InterfaceId NIS_Select_EndId = {kNIS_Select_EndName, 0x0BE4, 0x4888};

FunctionHandle gNIS_Select_StartHandle;
FunctionHandle gSoundFX_SelectHandle;
FunctionHandle gNIS_Select_BlacklistHandle;
FunctionHandle gNIS_Select_EndHandle;
} // namespace Csis
