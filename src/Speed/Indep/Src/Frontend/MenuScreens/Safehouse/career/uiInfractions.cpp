#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct FECareerRecord;
void eUnloadStreamingTexture(unsigned int *textures, int count);
void WaitForResourceLoadingComplete();
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
void FEngSetVisible(FEObject *obj);

inline void eUnloadStreamingTexture(unsigned int name_hash) {
    eUnloadStreamingTexture(&name_hash, 1);
}

struct PostPursuitInfractionsScreen : MenuScreen {
    PostPursuitInfractionsScreen(ScreenConstructorData *sd);
    ~PostPursuitInfractionsScreen() override;
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
    void NotifyBustedTextureLoaded();
    unsigned int CalcBustedTexture();
    static void TextureLoadedCallback(unsigned int arg) {
        reinterpret_cast<PostPursuitInfractionsScreen *>(arg)->NotifyBustedTextureLoaded();
    }

    FECareerRecord *WorkingCareerRecord; // offset 0x2C
    bool bStrikeLimitReached;            // offset 0x30
    int AmountToPay;                     // offset 0x34
    int AmountPlayerHas;                 // offset 0x38
    bool bHasMarker;                     // offset 0x3C
    unsigned int BustedTexture;          // offset 0x40
    bool bFirstTimeBusted;               // offset 0x44
};

MenuScreen *PostPursuitInfractionsScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) PostPursuitInfractionsScreen(sd);
}

PostPursuitInfractionsScreen::~PostPursuitInfractionsScreen() {
    eUnloadStreamingTexture(BustedTexture);
    WaitForResourceLoadingComplete();
}

void PostPursuitInfractionsScreen::NotifyBustedTextureLoaded() {
    FEngSetVisible(FEngFindObject(GetPackageName(), 0x2347122A));
}