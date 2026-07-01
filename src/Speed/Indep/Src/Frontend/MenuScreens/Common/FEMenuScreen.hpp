#ifndef FEMENUSCREEN_HPP
#define FEMENUSCREEN_HPP

#include <types.h>

#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"

// total size: 0xC
class ScreenConstructorData {
  public:
    const char *PackageFilename; // offset 0x0, size 0x4
    struct FEPackage *pPackage;  // offset 0x4, size 0x4
    int Arg;                     // offset 0x8, size 0x4
};

// total size: 0x2C
class MenuScreen {
  public:
    MenuScreen(ScreenConstructorData *sd);
    virtual ~MenuScreen();

    int GetActiveButtons();

    virtual void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) = 0;
    virtual eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
        return maybe;
    }

    void BaseNotify(u32 Message, FEObject *pObject, u32 Param1, u32 Param2);

    const char *FEngGetEditedString();

    void FEngEndTextInput();

    void FEngAbortTextInput();

    void FEngBeginTextInput(uint32 object_hash, uint32 edit_mode, const char *initial_string, const char *title_string, uint32 max_text_length);

    bool CheckKeyboard(uint32 msg);

    void BaseNotifySound(u32 msg, FEObject *obj, u32 controller_mask, u32 pkg_ptr);

    static void UpdateStatusIcons(int iconType, bool on);

    void UpdateEAMIcons();

    static void MaybeShutdownVoIPChat();

    const char *GetPackageName() {
        return PackageFilename;
    }

    FEPackage *GetPackage() {
        return ConstructData.pPackage;
    }

    void SetAsGarageScreen() {
        IsGarageScreen = true;
    }

  protected:
    bool mPlaySound;            // offset 0x0, size 0x1
    u32 mDirectionForNextSound; // offset 0x4, size 0x4
    bool bEnableEAMessenger;    // offset 0x8, size 0x1

    static bool ShowingEAMessenger; // size: 0x1, address: 0xFFFFFFFF
    static bool EnteringChallenge;  // size: 0x1, address: 0xFFFFFFFF

  private:
    const char *PackageFilename;                 // offset 0xC, size 0x4
    ScreenConstructorData ConstructData;         // offset 0x10, size 0xC
    bool IsGarageScreen;                         // offset 0x1C, size 0x1
    struct FEngTextInputObject *TextInputObject; // offset 0x20, size 0x4
    uint8 mStartCapturingFromKeyboard;           // offset 0x24, size 0x1

    static int gEAMIconState; // size: 0x4, address: 0xFFFFFFFF
};

// Decl: d:/speed/indep/src/frontend/menuscreens/common/FEMenuScreen.hpp:94
typedef MenuScreen *(*MenuScreenCreateFunction)(ScreenConstructorData *sd);

#endif
