#ifndef _UIOPTIONSSCREEN
#define _UIOPTIONSSCREEN

#include <types.h>

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

// total size: 0x15C
class UIOptionsScreen : public UIWidgetMenu {
  public:
    UIOptionsScreen(ScreenConstructorData *sd);
    ~UIOptionsScreen() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup() override;

    void SetupAudio();
    void SetupVideo();
    void SetupGameplay();
    void SetupPlayer();
    void SetupOnline();
    void RestoreDefaults();
    void RestoreOriginals();
    bool OptionsDidNotChange();
    void TogglePlayer(bool revert_changes);
    bool ShouldShowAutoSave();

    void SetupDebug();
    void SetupDebugAI();
    void SetupDebugPhysics();
    void SetupDebugOnline();
    void SetupDebugRendering();

  public:
    static int PlayerToEdit;

  private:
    bool mCalledFromPauseMenu;                  // offset 0x138, size 0x1
    bool NeedsColorCal;                         // offset 0x13C, size 0x1
    bool mInitialAutoSaveValue;                 // offset 0x140, size 0x1
    FEToggleWidget *speakeroption;              // offset 0x144, size 0x4
    FESliderWidget *volumeoption;               // offset 0x148, size 0x4
    AudioSettings *OriginalAudioSettings;       // offset 0x14C, size 0x4
    VideoSettings *OriginalVideoSettings;       // offset 0x150, size 0x4
    GameplaySettings *OriginalGameplaySettings; // offset 0x154, size 0x4
    PlayerSettings *OriginalPlayerSettings;     // offset 0x158, size 0x4
};

#endif
