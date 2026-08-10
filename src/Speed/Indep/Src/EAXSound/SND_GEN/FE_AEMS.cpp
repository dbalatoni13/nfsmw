
#include "Speed/Indep/Src/EAXSound/SND_GEN/FE_AEMS.h"

namespace Csis {

InterfaceId PlayCommonSampleId = {"PlayCommonSample", 0x3332, 0x2629}; // size: 0x8, Decl: 12
ClassHandle gPlayCommonSampleHandle;                                   // size: 0x10, Decl: 13

InterfaceId PlayFrontEndSampleId = {"PlayFrontEndSample", 0x3332, 0x1E3D}; // size: 0x8, Decl: 18
ClassHandle gPlayFrontEndSampleHandle;                                     // size: 0x10, Decl: 19

InterfaceId PlayFrontEndSample_RSId = {"PlayFrontEndSample_RS", 0x3332, 0x60E7}; // size: 0x8, Decl: 21
ClassHandle gPlayFrontEndSample_RSHandle;                                        // size: 0x10, Decl: 22

InterfaceId FEDriveOnId = {"FEDriveOn", 0x3332, 0x0DE5}; // size: 0x8, Decl: 24
ClassHandle gFEDriveOnHandle;                            // size: 0x10, Decl: 25

// InterfaceId FX_STREAKId;                  // size: 0x8, Decl: 27
// ClassHandle gFX_STREAKHandle;             // size: 0x10, Decl: 28

}; // namespace Csis
