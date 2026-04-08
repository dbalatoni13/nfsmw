#include "Speed/Indep/Src/EAXSound/Csis.hpp"

static const char kAEMS_StichCollisionName[] = "AEMS_StichCollision";
static const char kAEMS_StichWooshName[] = "AEMS_StichWoosh";
static const char kAEMS_StichStaticName[] = "AEMS_StichStatic";

namespace Csis {
InterfaceId AEMS_StichCollisionId = {kAEMS_StichCollisionName, 0x344F, 0x1F9A};
InterfaceId AEMS_StichWooshId = {kAEMS_StichWooshName, 0x344F, 0x3512};
InterfaceId AEMS_StichStaticId = {kAEMS_StichStaticName, 0x344F, 0x5FA3};

ClassHandle gAEMS_StichCollisionHandle;
ClassHandle gAEMS_StichWooshHandle;
ClassHandle gAEMS_StichStaticHandle;
} // namespace Csis
