#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiSafehouseRegionUnlock.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

extern int iCurrentViewBin;

uiSafehouseRegionUnlock::uiSafehouseRegionUnlock(ScreenConstructorData *sd) : MenuScreen(sd), RivalStreamer(sd->PackageFilename, false) {
    Setup();
}

uiSafehouseRegionUnlock::~uiSafehouseRegionUnlock() {}

void uiSafehouseRegionUnlock::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    if (msg == 0x406415e3) {
        uiRepSheetRivalFlow::Get()->Next();
    }
}

void uiSafehouseRegionUnlock::Setup() {
    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    pBGImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);
    unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    int next_bin = bin + 1;
    if (bin == 12) {
        FEngSetLanguageHash(GetPackageName(), 0xd6c0e097, 0x29e4b193);
    } else if (next_bin == 9) {
        FEngSetLanguageHash(GetPackageName(), 0xd6c0e097, 0x2b0bca2d);
    }
    RivalStreamer.Init(static_cast<unsigned int>(FEDatabase->GetCareerSettings()->GetCurrentBin()) + 1, pRivalImg, pTagImg, pBGImg);
}
