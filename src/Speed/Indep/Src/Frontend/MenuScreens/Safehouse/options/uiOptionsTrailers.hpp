#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UIOPTIONSTRAILERS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UIOPTIONSTRAILERS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// total size: 0x16C
class UIOptionsTrailers : public IconScrollerMenu {
  public:
    UIOptionsTrailers(ScreenConstructorData *sd);

    ~UIOptionsTrailers() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void Setup() override;
};

#endif
