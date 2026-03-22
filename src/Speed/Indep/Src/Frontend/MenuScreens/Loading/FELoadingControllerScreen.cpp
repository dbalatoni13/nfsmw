#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
extern void eUnloadStreamingTexture(unsigned int *textures, int count);

void *LoadingControllerScreen::mLoadingControllerScreenPtr;

LoadingControllerScreen::LoadingControllerScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}

void LoadingControllerScreen::NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) {}

void LoadingControllerScreen::ShowControllerConfig() {
    FEngSetScript(GetPackageName(), 0x3248E720, 0x001CA7C0, true);
}

void LoadingControllerScreen::HideControllerConfig() {
    FEngSetScript(GetPackageName(), 0x3248E720, 0x0016A259, true);
    WhichControllerTexture = 0;
}

void LoadingControllerScreen::ClearLoadedControllerTexture() {
    unsigned int tex[1];
    tex[0] = WhichControllerTexture;
    if (tex[0]) {
        eUnloadStreamingTexture(tex, 1);
    }
}

void LoadingControllerScreen::FinishLoadingControllerTextureCallback(unsigned int p) {
    ShowControllerConfig();
}

extern bool IsJoystickTypeWheel(JoystickPort port);
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(unsigned int), void *user, int priority);
extern void FinishLoadingControllerTextureCallbackBridge(unsigned int p);

void LoadingControllerScreen::PrepToShowControllerConfig() {
    unsigned int texHash;
    if (!IsJoystickTypeWheel(static_cast<JoystickPort>(FEDatabase->PlayerJoyports[0]))) {
        texHash = 0xed543bac;
        if (FEDatabase->CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[0].DriveWithAnalog != 0) {
            texHash = 0xed543bab;
        }
    } else {
        FEDatabase->CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[0].Config = static_cast<eControllerConfig>(0);
        texHash = 0xb511476b;
    }
    WhichControllerTexture = texHash;
    FEImage *img = FEngFindImage(GetPackageName(), 0x922a39c4);
    FEngSetTextureHash(img, texHash);
    unsigned int texArray[1];
    texArray[0] = WhichControllerTexture;
    eLoadStreamingTexture(texArray, 1, FinishLoadingControllerTextureCallbackBridge, this, 0);
}

void FinishLoadingControllerTextureCallbackBridge(unsigned int p) {
    LoadingControllerScreen *ls;
    if (p != 0) {
        ls = reinterpret_cast<LoadingControllerScreen *>(p);
        ls->FinishLoadingControllerTextureCallback(0);
    }
}

void LoadingControllerScreen::InitLoadingControllerScreen() {
    mLoadingControllerScreenPtr = bMalloc(0x38, nullptr, 0, 0);
}

MenuScreen *CreateLoadingControllerScreen(ScreenConstructorData *sd) {
    return new (LoadingControllerScreen::mLoadingControllerScreenPtr) LoadingControllerScreen(sd);
}

extern unsigned int FindButtonNameHashForFEString(int config, int string_number, JoystickPort player);
extern int FEngSNPrintf(char *, int, const char *, ...);
extern unsigned long FEHashUpper(const char *name);
extern void FEngSetVisible(const char *pkg_name, unsigned int obj_hash);
extern void FEngSetInvisible(const char *pkg_name, unsigned int obj_hash);
extern void FEngSetLanguageHash(const char *pkg_name, unsigned int object_hash, unsigned int language_hash);
extern void FEngSetTextureHash(const char *pkg_name, unsigned int obj_hash, unsigned int texture_hash);

void LoadingControllerScreen::SetupControllerConfig() {
    if (!FEDatabase->IsCareerMode()) {
        cFEng::Get()->QueuePackageMessage(0xde511657, GetPackageName(), nullptr);
    }
    JoystickPort port = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
    int config = FEDatabase->GetPlayerSettings(0)->Config;
    for (int i = 0; i < 17; i++) {
        char sztemp[32];
        FEngSNPrintf(sztemp, 0x20, "BUTTON%d", i + 1);
        unsigned int obj_hash = FEHashUpper(sztemp);
        FEngSNPrintf(sztemp, 0x20, "BUTTON%d_I", i + 1);
        unsigned int img_hash = FEHashUpper(sztemp);
        unsigned int button_hash = FindButtonNameHashForFEString(config, i, port);
        if (button_hash != 0) {
            FEngSetVisible(GetPackageName(), obj_hash);
            FEngSetLanguageHash(GetPackageName(), obj_hash, button_hash);
            FEngSetVisible(GetPackageName(), img_hash);
        } else {
            FEngSetInvisible(GetPackageName(), obj_hash);
            FEngSetInvisible(GetPackageName(), img_hash);
        }
    }
    if (FEDatabase->GetPlayerSettings(0)->DriveWithAnalog != 0) {
        FEngSetTextureHash(GetPackageName(), 0x4592229c, 0x148e38);
    } else {
        FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x4592229c), 0xb30961b);
    }
    FEngSetInvisible(GetPackageName(), 0xf274b86);
    FEngSetInvisible(GetPackageName(), 0x673d77bc);
    FEngSetInvisible(GetPackageName(), 0x351ae442);
    FEImage *img0 = FEngFindImage(GetPackageName(), 0x81b57400);
    FEngSetTextureHash(img0, 0x2959349);
    FEImage *img1 = FEngFindImage(GetPackageName(), 0x81b57401);
    FEngSetTextureHash(img1, 0x6851aaf5);
    FEImage *img2 = FEngFindImage(GetPackageName(), 0x81b57402);
    FEngSetTextureHash(img2, 0x3b7f86d);
}
