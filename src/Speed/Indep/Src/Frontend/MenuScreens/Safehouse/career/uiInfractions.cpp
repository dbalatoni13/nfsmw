#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

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

extern eLanguages GetCurrentLanguage();

unsigned int PostPursuitInfractionsScreen::CalcBustedTexture() {
    switch (GetCurrentLanguage()) {
    case 1:
        return 0xb419f122;
    case 2:
        return 0xb419f3c3;
    case 3:
        return 0xb419fe23;
    case 4:
        return 0xb41a2829;
    case 5:
        return 0xb419e912;
    case 6:
        return 0xb41a2914;
    case 7:
        return 0xb419e678;
    case 8:
        return 0xb41a0611;
    case 9:
        return 0xb419e319;
    case 10:
        return 0xb41a0000;
    case 11:
        return 0xb41a2b62;
    case 12:
        return 0xb41a1b50;
    case 13:
        return 0xb419f002;
    default:
        return 0xb419ec5f;
    }
}