#ifndef __UI_MARKER_SELECT_HPP__
#define __UI_MARKER_SELECT_HPP__

#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"
#include <types.h>

// total size: 0xC8 (from DWARF: 0xC0 data + 0x8 for possible trailing alignment)
class FEMarkerSelection : public MenuScreen {
  public:
    // total size: 0xC
    struct Selection {
        FEMarkerManager::ePossibleMarker Marker; // offset 0x0, size 0x4
        int Param;                               // offset 0x4, size 0x4
        bool Selected;                           // offset 0x8, size 0x1
    };

    FEMarkerSelection(ScreenConstructorData *sd);
    ~FEMarkerSelection() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  protected:
    int GetSelectedButtonIndex();
    int GetButtonIndex(uint32 hash);
    uint32 GetIconHashForType(FEMarkerManager::ePossibleMarker marker);
    uint32 GetCategoryIconHashForType(FEMarkerManager::ePossibleMarker marker);
    uint32 GetNameHashForType(FEMarkerManager::ePossibleMarker marker);
    uint32 GetCategoryNameHashForType(FEMarkerManager::ePossibleMarker marker);
    uint32 GetBlurbHashForType(FEMarkerManager::ePossibleMarker marker);
    uint32 GetCategoryBlurbHashForType(FEMarkerManager::ePossibleMarker marker);
    void Redraw();
    int GetNumSelected();
    void SetUnlockIcon(eUnlockableEntity ent, uint32 message);

  public:
    int NumVisibleMarkers; // offset 0x2C, size 0x4
  protected:
    FEImage *pRivalImg;                    // offset 0x30, size 0x4
    FEImage *pTagImg;                      // offset 0x34, size 0x4
    FEImage *pBGImg;                       // offset 0x38, size 0x4
    uiRepSheetRivalStreamer RivalStreamer; // offset 0x3C, size 0x3C
    Selection TheMarkers[6];               // offset 0x78, size 0x48
};

#endif
