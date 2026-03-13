#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct FEngTextInputObject {
    FEString *DisplayString;
    MenuScreen *ParentPackage;
    int mBlinkTime;
    static int sCursorBlinkCycleTime;

    FEngTextInputObject(MenuScreen *pkg, FEString *obj, unsigned int mode, const char *start_string,
                        unsigned int max_text_length);
    ~FEngTextInputObject();
    void Notify(unsigned int msg);
    void ReturnPressed();
    void EscapePressed();
    char *GetEditedString();
};

struct KeyboardEditString {
    char InitialString[256];
    unsigned short EditStringUCS2[256];
    int CursorPosUCS2;
    char EditStringPacked[256];
    unsigned int ModeFlags;
    int KeysProcessed;
    int MaxTextLength;
    bool mEnabled;
    FEngTextInputObject *TextInputObject;

    bool IsCapturing() {
        return mEnabled && TextInputObject != nullptr;
    }
};

extern KeyboardEditString gKeyboardManager;
extern MenuScreen *g_pOLCurrentScreen;

extern void FESoundControl(bool, const char *);
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void FEngSetButtonTexture(FEImage *img, unsigned int tex_hash);
extern int bStrCmp(const char *s1, const char *s2);
extern char *bStrNCpy(char *to, const char *from, int max_count);
extern void bMemSet(void *dst, int value, unsigned int size);

extern const char lbl_803E59BC[];
extern const char lbl_803E5CB8[];
extern const char lbl_803E5EEC[];
extern const char lbl_803E6D54[];
extern const char lbl_803E6D6C[];
extern const char lbl_803E6D8C[];
extern const char lbl_803E7FC4[];
extern const char lbl_803E85A8[];
extern const char lbl_803E85C4[];
extern const char lbl_803E85E0[];
extern const char lbl_803E8600[];

static bool ShouldTrackOLCurrentScreen(const char *package_name) {
    if (bStrCmp(package_name, lbl_803E5EEC) == 0 || bStrCmp(package_name, lbl_803E6D54) == 0 ||
        bStrCmp(package_name, lbl_803E7FC4) == 0 || bStrCmp(package_name, lbl_803E6D6C) == 0 ||
        bStrCmp(package_name, lbl_803E59BC) == 0 || bStrCmp(package_name, lbl_803E6D8C) == 0 ||
        bStrCmp(package_name, lbl_803E85A8) == 0 || bStrCmp(package_name, lbl_803E85C4) == 0 ||
        bStrCmp(package_name, lbl_803E85E0) == 0) {
        return false;
    }

    return !cFEng::Get()->IsPackagePushed(lbl_803E8600);
}

MenuScreen::MenuScreen(ScreenConstructorData *sd)
    : mPlaySound(true) //
    , mDirectionForNextSound(0) //
    , bEnableEAMessenger(false) //
    , PackageFilename(sd->PackageFilename) //
    , ConstructData(*sd) //
    , IsGarageScreen(false) //
    , TextInputObject(nullptr) //
    , mStartCapturingFromKeyboard(0) {
    FESoundControl(true, PackageFilename);
    FEngSetButtonTexture(FEngFindImage(PackageFilename, 0x6B364F8B), 0x5BC);
    FEngSetButtonTexture(FEngFindImage(PackageFilename, 0x79354351), 0x682);

    if (ShouldTrackOLCurrentScreen(PackageFilename)) {
        g_pOLCurrentScreen = this;
    }
}

MenuScreen::~MenuScreen() {
    FESoundControl(false, PackageFilename);

    if (ShouldTrackOLCurrentScreen(PackageFilename)) {
        g_pOLCurrentScreen = nullptr;
    }
}

void MenuScreen::BaseNotify(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    if (!CheckKeyboard(Message)) {
        if (Message != 0x9803F6E2 || ConstructData.pPackage->Controllers != 0) {
            NotificationMessage(Message, pObject, Param1, Param2);
        }
    }
}

const char *MenuScreen::FEngGetEditedString() {
    return FEDatabase->mFEKeyboardSettings.Buffer;
}

void MenuScreen::FEngEndTextInput() {
    if (TextInputObject != nullptr) {
        delete TextInputObject;
    }

    mStartCapturingFromKeyboard = 0;
    TextInputObject = nullptr;
}

void MenuScreen::FEngBeginTextInput(uint32 /* object_hash */, uint32 edit_mode,
                                    const char *initial_string, const char *title_string,
                                    uint32 max_text_length) {
    FEKeyboardSettings &settings = FEDatabase->mFEKeyboardSettings;

    bStrNCpy(settings.Buffer, initial_string, sizeof(settings.Buffer));
    settings.AcceptCallbackHash = 0xDA5B8712;
    settings.MaxTextLength = max_text_length;
    settings.DeclineCallbackHash = 0xC9D30688;
    settings.DefaultTextHash = 0;
    bMemSet(settings.Title, 0, sizeof(settings.Title));
    bStrNCpy(settings.Title, title_string, sizeof(settings.Title) - 1);

    settings.Mode = 1;
    switch (edit_mode) {
    case 0:
        settings.Mode = 0;
        break;
    case 1:
    case 3:
        settings.Mode = 1;
        break;
    case 2:
        settings.Mode = 5;
        break;
    case 4:
        settings.Mode = 4;
        break;
    case 5:
        settings.Mode = 2;
        break;
    case 6:
        settings.Mode = 3;
        break;
    }

    cFEng::Get()->QueuePackagePush(lbl_803E5CB8, 0, 0, false);
}

bool MenuScreen::CheckKeyboard(uint32 msg) {
    if (msg == 0xC98356BA) {
        if (TextInputObject == nullptr) {
            return false;
        }

        if (mStartCapturingFromKeyboard == 1) {
            mStartCapturingFromKeyboard = 2;
            return true;
        }

        if (mStartCapturingFromKeyboard == 2) {
            mStartCapturingFromKeyboard = 3;
            gKeyboardManager.KeysProcessed = 1;
        }
    }

    if (TextInputObject == nullptr) {
        return false;
    }

    if (gKeyboardManager.KeysProcessed != 0 && gKeyboardManager.MaxTextLength != 0) {
        if (msg == 0x911AB364) {
            TextInputObject->EscapePressed();
        } else if (msg == 0x406415E3) {
            TextInputObject->ReturnPressed();
        } else {
            TextInputObject->Notify(msg);
        }
    }

    return true;
}
