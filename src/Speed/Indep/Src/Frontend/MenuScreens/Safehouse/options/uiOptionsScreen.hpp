#ifndef _UIOPTIONSSCREEN
#define _UIOPTIONSSCREEN

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

struct AudioSettings;
struct VideoSettings;
struct GameplaySettings;
struct PlayerSettings;

// total size: 0x15C
struct UIOptionsScreen : public UIWidgetMenu {
    static int PlayerToEdit;

    int mCalledFromPauseMenu;                     // offset 0x138, size 0x1
    int NeedsColorCal;                            // offset 0x13C, size 0x1
    int mInitialAutoSaveValue;                    // offset 0x140, size 0x1
    FEToggleWidget* speakeroption;                // offset 0x144, size 0x4
    FESliderWidget* volumeoption;                 // offset 0x148, size 0x4
    AudioSettings* OriginalAudioSettings;         // offset 0x14C, size 0x4
    VideoSettings* OriginalVideoSettings;         // offset 0x150, size 0x4
    GameplaySettings* OriginalGameplaySettings;   // offset 0x154, size 0x4
    PlayerSettings* OriginalPlayerSettings;       // offset 0x158, size 0x4

    UIOptionsScreen(ScreenConstructorData* sd);
    ~UIOptionsScreen() override;

    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;

    void SetupAudio();
    void SetupVideo();
    void SetupGameplay();
    void SetupPlayer();
    void SetupOnline();
    void RestoreDefaults();
    bool OptionsDidNotChange();
    void RestoreOriginals();
    void TogglePlayer(bool revert_changes);
    bool ShouldShowAutoSave();
};

#endif
