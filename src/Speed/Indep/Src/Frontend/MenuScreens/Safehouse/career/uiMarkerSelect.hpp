// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIMARKERSELECT_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIMARKERSELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"
#include <types.h>

struct FEImage;

#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"

// total size: 0xC8 (from DWARF: 0xC0 data + 0x8 for possible trailing alignment)
struct FEMarkerSelection : public MenuScreen {
    // total size: 0xC
    struct Selection {
        FEMarkerManager::ePossibleMarker Marker; // offset 0x0, size 0x4
        int Param;                                // offset 0x4, size 0x4
        bool Selected;                            // offset 0x8, size 0x1
    };

    FEMarkerSelection(ScreenConstructorData *sd);
    ~FEMarkerSelection() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    void SetUnlockIcon(eUnlockableEntity ent, unsigned int message);
    int GetButtonIndex(unsigned int hash);
    int GetSelectedButtonIndex();
    unsigned int GetIconHashForType(FEMarkerManager::ePossibleMarker marker);
    unsigned int GetCategoryIconHashForType(FEMarkerManager::ePossibleMarker marker);
    unsigned int GetNameHashForType(FEMarkerManager::ePossibleMarker marker);
    unsigned int GetCategoryNameHashForType(FEMarkerManager::ePossibleMarker marker);
    unsigned int GetBlurbHashForType(FEMarkerManager::ePossibleMarker marker);
    unsigned int GetCategoryBlurbHashForType(FEMarkerManager::ePossibleMarker marker);
    int GetNumSelected();
    void Redraw();

    int NumVisibleMarkers;                     // offset 0x2C, size 0x4
    FEImage *pRivalImg;                        // offset 0x30, size 0x4
    FEImage *pTagImg;                          // offset 0x34, size 0x4
    FEImage *pBGImg;                           // offset 0x38, size 0x4
    uiRepSheetRivalStreamer RivalStreamer;      // offset 0x3C, size 0x3C
    Selection TheMarkers[6];                   // offset 0x78, size 0x48
};

#endif
