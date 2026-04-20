#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
InterfaceId FX_TRUCK_FXId = {"FX_TRUCK_FX", 0x1981, 0x5D9E};
InterfaceId PlayCommonSampleId = {"PlayCommonSample", 0x3332, 0x2629};
InterfaceId PlayFrontEndSampleId = {"PlayFrontEndSample", 0x3332, 0x1E3D};
InterfaceId PlayFrontEndSample_RSId = {"PlayFrontEndSample_RS", 0x3332, 0x60E7};
InterfaceId FEDriveOnId = {"FEDriveOn", 0x3332, 0x0DE5};

ClassHandle gFX_TRUCK_FXHandle;
ClassHandle gPlayCommonSampleHandle;
ClassHandle gPlayFrontEndSampleHandle;
ClassHandle gPlayFrontEndSample_RSHandle;
ClassHandle gFEDriveOnHandle;
} // namespace Csis
