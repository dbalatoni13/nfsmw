#ifndef _UIOPTIONSTRAILERS
#define _UIOPTIONSTRAILERS

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// total size: 0x16C
class UIOptionsTrailers : public IconScrollerMenu {
  public:
    UIOptionsTrailers(ScreenConstructorData *sd);
    ~UIOptionsTrailers() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
};

#endif
