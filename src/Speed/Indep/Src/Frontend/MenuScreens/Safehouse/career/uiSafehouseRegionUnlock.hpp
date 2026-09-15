#ifndef __UI_SAFE_HOUSE_REGION_UNLOCK_HPP__
#define __UI_SAFE_HOUSE_REGION_UNLOCK_HPP__

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

class uiSafehouseRegionUnlock : public MenuScreen {
  public:
    uiSafehouseRegionUnlock(ScreenConstructorData *sd);
    ~uiSafehouseRegionUnlock() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup();

    FEImage *pRivalImg;
    FEImage *pTagImg;
    FEImage *pBGImg;
    uiRepSheetRivalStreamer RivalStreamer;
};

#endif
