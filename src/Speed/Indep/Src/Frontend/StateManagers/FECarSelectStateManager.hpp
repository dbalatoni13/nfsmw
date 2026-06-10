#ifndef __FE_CAR_SELECT_STATE_MANAGER__
#define __FE_CAR_SELECT_STATE_MANAGER__

#include <types.h>
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

// File: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp
// total size: 0x1C
// Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:15
class QRCarSelectBustedManager {
  public:
    typedef enum { BUSTED_ANIM_NOTHING = 0, BUSTED_ANIM_SHOW_STRIKE = 1, BUSTED_ANIM_SHOW_IMPOUNDED = 2 } eBustedAnimationTypes;

    FECareerRecord *WorkingCareerRecord; // offset 0x0, size 0x4
    FECarRecord *WorkingCarRecord;       // offset 0x4, size 0x4
    eBustedAnimationTypes Flags;         // offset 0x8, size 0x4

    QRCarSelectBustedManager(const char *pkg_name, int flags); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:25
    virtual ~QRCarSelectBustedManager();                       // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:26

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2);

    static void TextureLoadedCallbackAccessor(uint32 this_screen) {}

    void TextureLoadedCallback();

    void LoadImpoundTexture();

    void MaybeReleaseCar(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:33

    void MaybeAddImpoundBox(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:34

    void SetSelectedCar(FECarRecord *record);

    static void SetPlayerBusted() {}

    static void SetIsCross() {}

    bool IsImpoundInfoVisible();

    bool ShowImpoundedTexture();

    bool ShowNewStrikeAnimation() {}

    bool ShowImpoundedAnimation() {}

  private:
    const char *GetPackageName() {}

    void RefreshHeader(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:54

    bool CalcGameOver(); // Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.hpp:60

    static bool bPlayerJustGotBusted; // size: 0x1, address: 0x80439150, Decl: speed/indep/src/frontend/StateManagers/FECarSelectStateManager.cpp:24
    static bool bIsCross;             // size: 0x1, address: 0xFFFFFFFF

    uint32 ImpoundStampHash; // offset 0xC, size 0x4
    char *ParentPkg;         // offset 0x10, size 0x4
    bool bWantsImpound;      // offset 0x14, size 0x1
};

#endif
