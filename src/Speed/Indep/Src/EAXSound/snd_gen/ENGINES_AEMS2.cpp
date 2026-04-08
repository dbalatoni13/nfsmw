#include "Speed/Indep/Src/EAXSound/Csis.hpp"

static const char kSputter_MessageName[] = "Sputter_Message";
static const char kCARName[] = "CAR";
static const char kCAR_TRANNYName[] = "CAR_TRANNY";

namespace Csis {
InterfaceId Sputter_MessageId = {kSputter_MessageName, 0x383F, 0x0783};
InterfaceId CARId = {kCARName, 0x383F, 0x198B};
InterfaceId CAR_TRANNYId = {kCAR_TRANNYName, 0x383F, 0x6F12};

FunctionHandle gSputter_MessageHandle;
ClassHandle gCARHandle;
ClassHandle gCAR_TRANNYHandle;
} // namespace Csis
