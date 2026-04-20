#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
InterfaceId Sputter_MessageId = {"Sputter_Message", 0x383F, 0x0783};
InterfaceId CARId = {"CAR", 0x383F, 0x198B};
InterfaceId CAR_TRANNYId = {"CAR_TRANNY", 0x383F, 0x6F12};

FunctionHandle gSputter_MessageHandle;
ClassHandle gCARHandle;
ClassHandle gCAR_TRANNYHandle;
} // namespace Csis
