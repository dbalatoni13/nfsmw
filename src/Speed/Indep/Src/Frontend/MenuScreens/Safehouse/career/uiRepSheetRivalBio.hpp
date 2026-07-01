#ifndef __UI_REP_SHEET_RIVAL_BIO_HPP__
#define __UI_REP_SHEET_RIVAL_BIO_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

// total size: 0x78
class uiRepSheetRivalBio : public MenuScreen {
  public:
    uiRepSheetRivalBio(ScreenConstructorData *sd);
    ~uiRepSheetRivalBio() override {}

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    void RefreshHeader();

  private:
    void Setup();

    bool bIsInGame;                        // offset 0x2C, size 0x1
    FEImage *pRivalImg;                    // offset 0x30, size 0x4
    FEImage *pTagImg;                      // offset 0x34, size 0x4
    FEImage *pBGImg;                       // offset 0x38, size 0x4
    uiRepSheetRivalStreamer RivalStreamer; // offset 0x3C, size 0x3C
};

#endif
