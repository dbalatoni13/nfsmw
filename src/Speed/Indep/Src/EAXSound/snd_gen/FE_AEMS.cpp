#include "Speed/Indep/Src/EAXSound/Csis.hpp"

static const char kFX_TRUCK_FXName[] = "FX_TRUCK_FX";
static const char kPlayCommonSampleName[] = "PlayCommonSample";
static const char kPlayFrontEndSampleName[] = "PlayFrontEndSample";
static const char kPlayFrontEndSample_RSName[] = "PlayFrontEndSample_RS";
static const char kFEDriveOnName[] = "FEDriveOn";

namespace Csis {
InterfaceId FX_TRUCK_FXId = {kFX_TRUCK_FXName, 0x1981, 0x5D9E};
InterfaceId PlayCommonSampleId = {kPlayCommonSampleName, 0x3332, 0x2629};
InterfaceId PlayFrontEndSampleId = {kPlayFrontEndSampleName, 0x3332, 0x1E3D};
InterfaceId PlayFrontEndSample_RSId = {kPlayFrontEndSample_RSName, 0x3332, 0x60E7};
InterfaceId FEDriveOnId = {kFEDriveOnName, 0x3332, 0x0DE5};

ClassHandle gFX_TRUCK_FXHandle;
ClassHandle gPlayCommonSampleHandle;
ClassHandle gPlayFrontEndSampleHandle;
ClassHandle gPlayFrontEndSample_RSHandle;
ClassHandle gFEDriveOnHandle;
} // namespace Csis
