//
//
//
#ifndef FE_AEMS_H
#define FE_AEMS_H

#include "csis/csis.h"

namespace Csis {

enum ePlayCommonSample {
    SCMN_MENUSTART = 0,
    SCMN_MENULEFTRIGHT = 1,
    SCMN_MENUUPDOWN = 2,
    SCMN_MENUSELECT = 3,
    SCMN_MENUCANCEL = 4,
    SCMN_MENUWRONG = 5,
    SCMN_MENUADJUSTVOLUME = 6,
};

enum ePlayFrontEndSample {
    SFE_TRACKLR = 0,
    SFE_CARRIGHT = 1,
    SFE_CARLEFT = 2,
    SFE_CARCOLOR = 3,
    SFE_CARDISPON = 4,
    SFE_CARDISPACTIVE = 5,
    SFE_CARDISPOFF = 6,
    SFE_CARSELECT = 7,
    SFE_CARORBITBMP = 8,
    SFE_TREEUPDOWN = 9,
    SFE_TREELR = 10,
    SFE_TREEZOOM = 11,
    SFE_TREESELECT = 12,
    SFE_TREELOCKED = 13,
    SFE_REWARDVIEW = 14,
    SFE_NAMEKB = 15,
    SFE_NAMESELECT = 16,
    SFE_NAMECASECHANGE = 17,
    SFE_SELRACE = 18,
    SFE_SELPAINT = 19,
    SFE_SELDECAL = 20,
    SFE_SELPARTS = 21,
};

extern InterfaceId PlayCommonSampleId;      // size: 0x8, address: 0x80418880
extern ClassHandle gPlayCommonSampleHandle; // size: 0x8, Decl: 261

// total size: 0x10
// Decl: 19
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} PlayCommonSampleStruct;

extern InterfaceId PlayFrontEndSampleId;
extern ClassHandle gPlayFrontEndSampleHandle;

// total size: 0x10
// Decl: 48
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} PlayFrontEndSampleStruct;

extern InterfaceId PlayFrontEndSample_RSId;
extern ClassHandle gPlayFrontEndSample_RSHandle;

// total size: 0x10
// Decl: 62
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} PlayFrontEndSample_RSStruct;

extern InterfaceId FEDriveOnId;
extern ClassHandle gFEDriveOnHandle;

// total size: 0x10
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} FEDriveOnStruct;

// Unused classes TODO

}; // namespace Csis

#endif
