#include "uiRepSheetRivalBio.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Generated/Events/EEnterBin.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern int iCurrentViewBin; // TODO: remove

uiRepSheetRivalBio::uiRepSheetRivalBio(ScreenConstructorData *sd)
    : MenuScreen(sd), bIsInGame(sd->Arg != 0), RivalStreamer(sd->PackageFilename, bIsInGame) {
    if (FEDatabase->IsPostRivalMode()) {
        CarViewer::HideAllCars();
        if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 16) {
            new EEnterBin(FEDatabase->GetCareerSettings()->GetCurrentBin() - 1);
        }

        extern int iCurrentViewBin;
        iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        cFEng::Get()->QueuePackageMessage(0xb21a45f, GetPackageName(), nullptr);
    } else {
        const u32 FEObj_CALLFRAMES = 0xaf922178;
        cFEng::Get()->QueuePackageMessage(FEObj_CALLFRAMES, GetPackageName(), nullptr);
        if (!bIsInGame) {
            GarageMainScreen::GetInstance()->DisableCarRendering();
        }
    }
    Setup();
}

void uiRepSheetRivalBio::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_BUTTON0__:
            if (!FEDatabase->IsPostRivalMode()) {
                char buf[64];
                if (iCurrentViewBin == 1) {
                    bSNPrintf(buf, 64, "E3_DEMO_BMW");
                } else {
                    bSNPrintf(buf, 64, "BL%d", iCurrentViewBin);
                }
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *pCar = stable->CreateNewPresetCar(buf);
                if (pCar == nullptr) {
                    break;
                }
                RideInfo ride;
                stable->BuildRideForPlayer(pCar->Handle, 0, &ride);
                CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
                Showcase::FromArgs = 0;
                Showcase::FromPackage = GetPackageName();
                Showcase::BlackListNumber = iCurrentViewBin;
                cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(pCar), 0, false);
            }
            break;
        case __PAD_BUTTON4__:
            if (!FEDatabase->IsPostRivalMode()) {
                char buf[64];
                bSNPrintf(buf, 64, "blacklist_%02d", iCurrentViewBin);
                FEAnyMovieScreen::LaunchMovie(GetPackageName(), buf);
            }
            break;
        case __PAD_ACCEPT__:
            if (FEDatabase->IsPostRivalMode()) {
                if (uiRepSheetRivalFlow::Get()->GetStage() == -1) {
                    uiRepSheetRivalFlow::Get()->StartFlow(5);
                } else {
                    uiRepSheetRivalFlow::Get()->Next();
                }
            }
            break;
        case __PAD_BACK__:
            if (!FEDatabase->IsPostRivalMode()) {
                if (bIsInGame) {
                    cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
                } else {
                    GarageMainScreen::GetInstance()->EnableCarRendering();
                    cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
                }
            }
            break;
    }
}

void uiRepSheetRivalBio::RefreshHeader() {
    char buf[32];
    if (bIsInGame) {
        FEngSNPrintf(buf, 32, GetLocalizedString(0x96ca2471), iCurrentViewBin);
    } else {
        FEngSNPrintf(buf, 32, GetLocalizedString(0x3a64de21), iCurrentViewBin);
    }

    const u32 FEObj_TITLE_GROUP = 0x242657ce;

    FEPrintf(GetPackageName(), 0x242657ce, "%s", buf);

    FEngSetLanguageHash(GetPackageName(), 0x7ac3d0c9, FEngHashString("BLACKLIST_RIVAL_%02d_NAME", iCurrentViewBin));
    FEngSetLanguageHash(GetPackageName(), 0xb1f2748d, FEngHashString("BLACKLIST_RIVAL_%02d_AKA", iCurrentViewBin));
    FEngSetLanguageHash(GetPackageName(), 0x27e1d6d8, FEngHashString("BLACKLIST_RIVAL_%02d_BIO", iCurrentViewBin));
    FEngSetLanguageHash(GetPackageName(), 0xcb5bf41a, FEngHashString("BLACKLIST_RIVAL_%02d_CAR", iCurrentViewBin));
    FEngSetLanguageHash(GetPackageName(), 0xa6f07bf3, FEngHashString("BLACKLIST_RIVAL_%02d_STRENGTH", iCurrentViewBin));
}

void uiRepSheetRivalBio::Setup() {
    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    pBGImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);
    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, pBGImg);
    RefreshHeader();
}
