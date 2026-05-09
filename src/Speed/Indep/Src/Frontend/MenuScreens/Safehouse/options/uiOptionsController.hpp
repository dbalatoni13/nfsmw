#ifndef _UIOPTIONSCONTROLLER
#define _UIOPTIONSCONTROLLER

#include <types.h>

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

// total size: 0x154
class UIOptionsController : public UIWidgetMenu {
  public:
    UIOptionsController(ScreenConstructorData *sd);
    ~UIOptionsController() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void FinishLoadingTexCallback();
    bool OptionsDidNotChange();

  private:
    void Setup() override;

    void RestoreOriginals();
    void TogglePlayer();
    void DetectControllers();
    uint32 CalcControllerTextureToLoad();
    void PrepToShowControllerConfig();
    void ClearLoadedControllerTexture();
    void HideControllerConfig();
    void ShowControllerConfig();
    void SetupControllerConfig();

  public:
    static int PortToConfigure;
    static int isWheelConfig;

  private:
    uint32 WhichControllerTexture; // offset 0x138, size 0x4
    int PrevJoystickType;          // offset 0x13C, size 0x4
    eControllerConfig oldConfig;   // offset 0x140, size 0x4
    bool oldVibration;             // offset 0x144, size 0x1
    bool oldDriveWithAnalog;       // offset 0x148, size 0x1
    bool mCalledFromPauseMenu;     // offset 0x14C, size 0x1
    bool NeedSetup;                // offset 0x150, size 0x1
};

#endif
