#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct FEObject;
struct FEImage;
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);

extern unsigned int iCurrentViewBin;

struct uiSafehouseRegionUnlock : public MenuScreen {
    FEImage* pRivalImg;
    FEImage* pTagImg;
    FEImage* pBGImg;
    uiRepSheetRivalStreamer RivalStreamer;

    uiSafehouseRegionUnlock(ScreenConstructorData* sd);
    ~uiSafehouseRegionUnlock() override;
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) override;
    void Setup();
};

uiSafehouseRegionUnlock::uiSafehouseRegionUnlock(ScreenConstructorData* sd)
    : MenuScreen(sd)
    , RivalStreamer(sd->PackageFilename, false) {
    Setup();
}

uiSafehouseRegionUnlock::~uiSafehouseRegionUnlock() {
}

void uiSafehouseRegionUnlock::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    if (msg == 0x406415e3) {
        uiRepSheetRivalFlow::Get()->Next();
    }
}

void uiSafehouseRegionUnlock::Setup() {
    pRivalImg = FEngFindImage(PackageFilename, 0xc1f62308);
    pTagImg = FEngFindImage(PackageFilename, 0xf5a2a087);
    pBGImg = FEngFindImage(PackageFilename, 0x2cbe1dd0);
    unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    int next_bin = bin + 1;
    if (bin == 12) {
        FEngSetLanguageHash(PackageFilename, 0xd6c0e097, 0x29e4b193);
    } else if (next_bin == 9) {
        FEngSetLanguageHash(PackageFilename, 0xd6c0e097, 0x2b0bca2d);
    }
    RivalStreamer.Init(static_cast<unsigned int>(FEDatabase->GetCareerSettings()->GetCurrentBin()) + 1, pRivalImg, pTagImg, pBGImg);
}
