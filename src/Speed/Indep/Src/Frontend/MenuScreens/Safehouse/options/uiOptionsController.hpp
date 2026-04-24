#ifndef _UIOPTIONSCONTROLLER
#define _UIOPTIONSCONTROLLER

#include <types.h>

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

// total size: 0x154
struct UIOptionsController : public UIWidgetMenu {
    static int PortToConfigure;
    static int isWheelConfig;

    unsigned int WhichControllerTexture;  // offset 0x138, size 0x4
    int PrevJoystickType;                 // offset 0x13C, size 0x4
    eControllerConfig oldConfig;          // offset 0x140, size 0x4
    bool oldVibration;                    // offset 0x144, size 0x1
    bool oldDriveWithAnalog;              // offset 0x148, size 0x1
    bool mCalledFromPauseMenu;            // offset 0x14C, size 0x1
    bool NeedSetup;                       // offset 0x150, size 0x1

    UIOptionsController(ScreenConstructorData* sd);
    ~UIOptionsController() override;

    bool OptionsDidNotChange();

    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;

    void SetupControllerConfig();
    void DetectControllers();
    void ClearLoadedControllerTexture();
    void FinishLoadingTexCallback();
    unsigned int CalcControllerTextureToLoad();
    void PrepToShowControllerConfig();
    void ShowControllerConfig();
    void HideControllerConfig();
    void RestoreOriginals();
    void TogglePlayer();
};

#endif
