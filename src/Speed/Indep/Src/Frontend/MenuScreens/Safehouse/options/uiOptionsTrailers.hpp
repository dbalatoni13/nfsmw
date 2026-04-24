#ifndef _UIOPTIONSTRAILERS
#define _UIOPTIONSTRAILERS

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

// total size: 0x16C
struct UIOptionsTrailers : public IconScrollerMenu {
    UIOptionsTrailers(ScreenConstructorData* sd);
    ~UIOptionsTrailers() override {}

    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
};

#endif
