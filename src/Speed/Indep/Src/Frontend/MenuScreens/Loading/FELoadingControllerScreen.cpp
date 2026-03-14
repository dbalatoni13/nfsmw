#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
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