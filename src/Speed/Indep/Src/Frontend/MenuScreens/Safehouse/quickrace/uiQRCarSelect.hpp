#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCARSELECT_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCARSELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

// total size: 0x1C
class QRCarSelectBustedManager {
  public:
    enum eBustedAnimationTypes {
        BUSTED_ANIM_NOTHING = 0,
        BUSTED_ANIM_SHOW_STRIKE = 1,
        BUSTED_ANIM_SHOW_IMPOUNDED = 2,
    };

    static void TextureLoadedCallbackAccessor(unsigned int this_screen) {}

    static void SetPlayerBusted() {
        bPlayerJustGotBusted = true;
    }

    static void SetIsCross() {}

    bool ShowNewStrikeAnimation() {}

    bool ShowImpoundedAnimation() {}

    QRCarSelectBustedManager(const char *pkg_name, int flags);

    virtual ~QRCarSelectBustedManager();

    bool IsImpoundInfoVisible();

    bool ShowImpoundedTexture();

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2);

    void TextureLoadedCallback();

    void LoadImpoundTexture();

    void SetSelectedCar(FECarRecord *record);

    void MaybeReleaseCar();

    void MaybeAddImpoundBox();

    FECareerRecord *WorkingCareerRecord; // offset 0x0, size 0x4
    FECarRecord *WorkingCarRecord;       // offset 0x4, size 0x4
    enum eBustedAnimationTypes Flags;    // offset 0x8, size 0x4

  private:
    const char *GetPackageName() {}

    void RefreshHeader();
    bool CalcGameOver();

    static bool bPlayerJustGotBusted; // size: 0x1, address: 0x80439150
    static bool bIsCross;             // size: 0x1, address: 0xFFFFFFFF

    unsigned int ImpoundStampHash; // offset 0xC, size 0x4
    const char *ParentPkg;         // offset 0x10, size 0x4
    bool bWantsImpound;            // offset 0x14, size 0x1
};

#endif
