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
    if (p != 0) {
        reinterpret_cast<LoadingControllerScreen *>(p)->FinishLoadingControllerTextureCallback(0);
    }
}

void LoadingControllerScreen::InitLoadingControllerScreen() {
    mLoadingControllerScreenPtr = bMalloc(0x38, 0);
}

MenuScreen *CreateLoadingControllerScreen(ScreenConstructorData *sd) {
    return new (LoadingControllerScreen::mLoadingControllerScreenPtr) LoadingControllerScreen(sd);
}