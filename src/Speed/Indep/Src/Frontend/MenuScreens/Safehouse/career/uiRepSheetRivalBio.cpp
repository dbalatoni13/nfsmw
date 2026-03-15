#include "uiRepSheetRivalBio.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Generated/Events/EEnterBin.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
unsigned int FEngHashString(const char* format, ...);
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
int FEngSNPrintf(char* buffer, int buf_size, const char* fmt, ...);
const char* GetLocalizedString(unsigned int hash);

extern unsigned int iCurrentViewBin;

namespace Showcase {
extern const char* FromPackage;
extern int FromArgs;
extern int BlackListNumber;
} // namespace Showcase

uiRepSheetRivalBio::uiRepSheetRivalBio(ScreenConstructorData* sd)
    : MenuScreen(sd)
    , bIsInGame(sd->Arg != 0)
    , RivalStreamer(sd->PackageFilename, bIsInGame) {
    if (FEDatabase->IsPostRivalMode()) {
        CarViewer::HideAllCars();
        if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 16) {
            new EEnterBin(FEDatabase->GetCareerSettings()->GetCurrentBin() - 1);
        }
        iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        cFEng::Get()->QueuePackageMessage(0xb21a45f, GetPackageName(), nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(0xaf922178, GetPackageName(), nullptr);
        if (!bIsInGame) {
            GarageMainScreen::GetInstance()->DisableCarRendering();
        }
    }
    Setup();
}

void uiRepSheetRivalBio::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                              unsigned long param2) {
    switch (msg) {
    case 0xc519bfbf:
        if (FEDatabase->IsPostRivalMode()) {
            break;
        }
    {
        char buf[64];
        if (iCurrentViewBin == 1) {
            bSNPrintf(buf, 64, "E3_DEMO_BMW");
        } else {
            bSNPrintf(buf, 64, "BL%d", iCurrentViewBin);
        }
        FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
        FECarRecord* pCar = stable->CreateNewPresetCar(buf);
        if (pCar == nullptr) {
            break;
        }
        RideInfo ride;
        stable->BuildRideForPlayer(pCar->Handle, 0, &ride);
        CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
        Showcase::FromArgs = 0;
        Showcase::FromPackage = GetPackageName();
        Showcase::BlackListNumber = iCurrentViewBin;
        cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast< int >(pCar), 0, false);
        break;
    }
    case 0xc519bfc3:
        if (FEDatabase->IsPostRivalMode()) {
            break;
        }
    {
        char buf[64];
        bSNPrintf(buf, 64, "blacklist_%02d", iCurrentViewBin);
        FEAnyMovieScreen::LaunchMovie(GetPackageName(), buf);
        break;
    }
    case 0x406415e3:
        if (!FEDatabase->IsPostRivalMode()) {
            break;
        }
        if (uiRepSheetRivalFlow::Get()->GetStage() == -1) {
            uiRepSheetRivalFlow::Get()->StartFlow(5);
        } else {
            uiRepSheetRivalFlow::Get()->Next();
        }
        break;
    case 0x911ab364:
        if (FEDatabase->IsPostRivalMode()) {
            break;
        }
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
        } else {
            GarageMainScreen::GetInstance()->EnableCarRendering();
            cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
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
    FEPrintf(GetPackageName(), 0x242657ce, "%s", buf);
    const char* pkgName;
    pkgName = GetPackageName();
    unsigned int hash = FEngHashString("BL_NAME_%d", iCurrentViewBin);
    FEngSetLanguageHash(pkgName, 0x7ac3d0c9, hash);
    pkgName = GetPackageName();
    hash = FEngHashString("BL_RIDE_%d", iCurrentViewBin);
    FEngSetLanguageHash(pkgName, 0xb1f2748d, hash);
    pkgName = GetPackageName();
    hash = FEngHashString("BL_BIO_1_%d", iCurrentViewBin);
    FEngSetLanguageHash(pkgName, 0x27e1d6d8, hash);
    pkgName = GetPackageName();
    hash = FEngHashString("BL_BIO_2_%d", iCurrentViewBin);
    FEngSetLanguageHash(pkgName, 0xcb5bf41a, hash);
    pkgName = GetPackageName();
    hash = FEngHashString("BL_BIO_3_%d", iCurrentViewBin);
    FEngSetLanguageHash(pkgName, 0xa6f07bf3, hash);
}

void uiRepSheetRivalBio::Setup() {
    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    pBGImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);
    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, pBGImg);
    RefreshHeader();
}
