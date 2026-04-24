#ifndef _UIOPTIONSMAIN
#define _UIOPTIONSMAIN

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

// total size: 0x170
struct UIOptionsMain : public IconScrollerMenu {
    bool mCalledFromPauseMenu; // offset 0x16C, size 0x1

    UIOptionsMain(ScreenConstructorData* sd);
    ~UIOptionsMain() override {}

    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void ExitOptions(const char* nextPackage);
};

int GetPlayerToEditForOptions();
void SetPlayerToEditForOptions(int player);

#endif
