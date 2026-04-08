#include "Speed/Indep/Src/EAXSound/Csis.hpp"

static const char kFX_MAIN_MEMName[] = "FX_MAIN_MEM";
static const char kFX_ROADNOISEName[] = "FX_ROADNOISE";
static const char kFX_ROADNOISE_TRANSName[] = "FX_ROADNOISE_TRANS";
static const char kFX_WINDName[] = "FX_WIND";

namespace Csis {
InterfaceId FX_ROADNOISEId = {kFX_ROADNOISEName, 0x1981, 0x2C48};
InterfaceId FX_ROADNOISE_TRANSId = {kFX_ROADNOISE_TRANSName, 0x1981, 0x1B49};
InterfaceId FX_MAIN_MEMId = {kFX_MAIN_MEMName, 0x1981, 0x58AB};
InterfaceId FX_WINDId = {kFX_WINDName, 0x1981, 0x0DD5};

ClassHandle gFX_ROADNOISEHandle;
ClassHandle gFX_ROADNOISE_TRANSHandle;
ClassHandle gFX_MAIN_MEMHandle;
ClassHandle gFX_WINDHandle;
} // namespace Csis
