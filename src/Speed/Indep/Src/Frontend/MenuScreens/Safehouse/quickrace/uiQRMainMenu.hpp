#ifndef UIQRMAINMENU_H
#define UIQRMAINMENU_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

#include <types.h>

// total size: 0x16C
class UIQRMainMenu : public IconScrollerMenu {
  public:
    UIQRMainMenu(ScreenConstructorData *sd);
    ~UIQRMainMenu() override {};

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void RefreshHeader() override;

  private:
    void Setup() override;
};

#endif
