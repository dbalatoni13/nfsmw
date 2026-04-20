#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
InterfaceId AEMS_StichCollisionId = {"AEMS_StichCollision", 0x344F, 0x1F9A};
InterfaceId AEMS_StichWooshId = {"AEMS_StichWoosh", 0x344F, 0x3512};
InterfaceId AEMS_StichStaticId = {"AEMS_StichStatic", 0x344F, 0x5FA3};

ClassHandle gAEMS_StichCollisionHandle;
ClassHandle gAEMS_StichWooshHandle;
ClassHandle gAEMS_StichStaticHandle;
} // namespace Csis
