#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/CustomTuning.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/uiPause.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELanguageSelect.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiCareerMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiCareerManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRMainMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRModeSelect.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/fengine_full.h"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

static const char* gLoadinScreenPackageName;

void SetLoadingScreenPackageName(const char* name) {
    gLoadinScreenPackageName = name;
}

const char* GetLoadingScreenPackageName() {
    return gLoadinScreenPackageName;
}

struct ScreenButtonDatum {
    unsigned int ScreenHash; // offset 0x0, size 0x4
    unsigned char LastButton; // offset 0x4, size 0x1
    unsigned int GameMode; // offset 0x8, size 0x4
};

extern unsigned long FEHashUpper(const char *str);
extern ScreenButtonDatum *FindScreenButtonDatum(unsigned int hash);
extern void HackClearCache(FEPackage *pkg);
extern FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg);
extern cFEng *cFEng_mInstance;

struct ScreenFactoryDatum;
static ScreenFactoryDatum *FindScreenCreateData(unsigned int hash);
static MenuScreen *CreateCustomTuningScreen(ScreenConstructorData *sd);

static ScreenButtonDatum ScreenButtonData[0x32];

static ScreenButtonDatum *FindAvailableButtonDatum() {
    for (int i = 0; i <= 0x31; i++) {
        if (ScreenButtonData[i].ScreenHash == 0) {
            return &ScreenButtonData[i];
        }
    }
    return nullptr;
}

unsigned char FEngGetLastButton(const char *pkg_name) {
    ScreenButtonDatum *sd = FindScreenButtonDatum(FEHashUpper(pkg_name));
    if (!sd) {
        return 0;
    }
    return sd->LastButton;
}

struct UIMain : MenuScreen { UIMain(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x144]; };
struct UIOptionsScreen : MenuScreen { UIOptionsScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x130]; };
struct UIQRBrief : MenuScreen { UIQRBrief(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x10C]; };
struct UIQRTrackSelect : MenuScreen { UIQRTrackSelect(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xF4]; };
struct UIQRTrackOptions : MenuScreen { UIQRTrackOptions(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x128]; };
struct UIQRCarSelect : MenuScreen { UIQRCarSelect(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x18C]; };
struct uiQRPressStart : MenuScreen { uiQRPressStart(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x8]; };
struct UIQRChallengeSeries : MenuScreen { UIQRChallengeSeries(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1A4]; };
struct Showcase : MenuScreen { Showcase(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x44]; };
struct WorldMap : MenuScreen { WorldMap(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x170]; };
struct uiSMS : MenuScreen { uiSMS(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xD4]; };
struct uiSMSMessage : MenuScreen { uiSMSMessage(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xBC]; };
struct ControllerUnplugged : MenuScreen { ControllerUnplugged(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x4]; };
struct UISafehouseRaceSheet : MenuScreen { UISafehouseRaceSheet(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1A8]; };
struct uiRapSheetLogin : MenuScreen { uiRapSheetLogin(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x8]; };
struct uiRapSheetMain : MenuScreen { uiRapSheetMain(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x110]; };
struct uiRapSheetRS : MenuScreen { uiRapSheetRS(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} };
struct uiRapSheetUS : MenuScreen { uiRapSheetUS(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xC0]; };
struct uiRapSheetVD : MenuScreen { uiRapSheetVD(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xBC]; };
struct uiRapSheetCTS : MenuScreen { uiRapSheetCTS(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xBC]; };
struct uiRapSheetTEP : MenuScreen { uiRapSheetTEP(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x114]; };
struct uiRapSheetPD : MenuScreen { uiRapSheetPD(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x4]; };
struct uiRapSheetRankings : MenuScreen { uiRapSheetRankings(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x8]; };
struct uiRapSheetRankingsDetail : MenuScreen { uiRapSheetRankingsDetail(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xC4]; };
struct uiRepSheetMain : MenuScreen { uiRepSheetMain(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x194]; };
struct uiRepSheetRival : MenuScreen { uiRepSheetRival(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x60]; };
struct uiRepSheetRivalBio : MenuScreen { uiRepSheetRivalBio(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x4C]; };
struct uiRepSheetMilestones : MenuScreen { uiRepSheetMilestones(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xC8]; };
struct uiSafehouseRegionUnlock : MenuScreen { uiSafehouseRegionUnlock(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x48]; };
struct uiRepSheetBounty : MenuScreen { uiRepSheetBounty(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x14C]; };
struct FEMarkerSelection : MenuScreen { FEMarkerSelection(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x94]; };
struct FEGameWonScreen : MenuScreen { FEGameWonScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} };
struct DebugCarCustomizeScreen : MenuScreen { DebugCarCustomizeScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x54]; };
struct MyCarsManager : MenuScreen { MyCarsManager(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x194]; };
struct CustomizeMain : MenuScreen { CustomizeMain(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1A8]; };
struct CustomizeSub : MenuScreen { CustomizeSub(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1AC]; };
struct CustomizeShoppingCart : MenuScreen { CustomizeShoppingCart(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x15C]; };
struct CustomizeParts : MenuScreen { CustomizeParts(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1C8]; };
struct CustomizeHUDColor : MenuScreen { CustomizeHUDColor(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1CC]; };
struct CustomizeDecals : MenuScreen { CustomizeDecals(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1BC]; };
struct CustomizeNumbers : MenuScreen { CustomizeNumbers(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x8C]; };
struct CustomizePaint : MenuScreen { CustomizePaint(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x2F8]; };
struct CustomizeRims : MenuScreen { CustomizeRims(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1C4]; };
struct CustomizeSpoiler : MenuScreen { CustomizeSpoiler(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1CC]; };
struct CustomizePerformance : MenuScreen { CustomizePerformance(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x29C]; };
struct uiCredits : MenuScreen { uiCredits(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x1C]; };
struct UIEATraxScreen : MenuScreen { UIEATraxScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xF4]; };
struct UIOptionsController : MenuScreen { UIOptionsController(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x128]; };
namespace nsEngageEventDialog {
struct EngageEventDialog : MenuScreen { EngageEventDialog(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0xC]; };
}
struct MovieScreen : MenuScreen { MovieScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x28]; };
struct SplashScreen : MenuScreen {
    SplashScreen(ScreenConstructorData *);
    ~SplashScreen() override;
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override {
        if (bAllowContinue) {
            return maybe;
        }
        return static_cast<eMenuSoundTriggers>(-1);
    }
    bool bAllowContinue;
    unsigned int CopyrightNotice;
    unsigned int SplashStartedTimer;
};

static MenuScreen *CreateMainMenu(ScreenConstructorData *sd) {
    return new ("", 0) UIMain(sd);
}

static MenuScreen *CreateSubMenu(ScreenConstructorData *sd) {
    if (FEDatabase->IsOptionsMode()) {
        if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_TRAILERS) {
            return new ("", 0) UIOptionsTrailers(sd);
        }
        return new ("", 0) UIOptionsMain(sd);
    }
    if (FEDatabase->IsCareerMode()) {
        return new ("", 0) uiCareerCrib(sd);
    }
    if (FEDatabase->IsCareerManagerMode()) {
        return new ("", 0) uiCareerManager(sd);
    }
    if (FEDatabase->IsModeSelectMode()) {
        return new ("", 0) UIQRModeSelect(sd);
    }
    if (FEDatabase->IsQuickRaceMode()) {
        return new ("", 0) UIQRMainMenu(sd);
    }
    return new ("", 0) UIOptionsMain(sd);
}

static MenuScreen *CreateCommonPauseMenu(ScreenConstructorData *sd) {
    if (FEDatabase->IsOptionsMode()) {
        return new ("", 0) UIOptionsMain(sd);
    }
    return new ("", 0) PauseMenu(sd);
}

static MenuScreen *CreateOptionsScreen(ScreenConstructorData *sd) {
    return new ("", 0) UIOptionsScreen(sd);
}

static MenuScreen *CreateQRBrief(ScreenConstructorData *sd) {
    return new ("", 0) UIQRBrief(sd);
}

static MenuScreen *CreateQRTrackSelect(ScreenConstructorData *sd) {
    return new ("", 0) UIQRTrackSelect(sd);
}

static MenuScreen *CreateQRTrackOptions(ScreenConstructorData *sd) {
    return new ("", 0) UIQRTrackOptions(sd);
}

static MenuScreen *CreateQRCarSelect(ScreenConstructorData *sd) {
    return new ("", 0) UIQRCarSelect(sd);
}

static MenuScreen *CreateQRPressStart(ScreenConstructorData *sd) {
    return new ("", 0) uiQRPressStart(sd);
}

static MenuScreen *CreateQRChallengeSeries(ScreenConstructorData *sd) {
    return new ("", 0) UIQRChallengeSeries(sd);
}

static MenuScreen *CreateShowcase(ScreenConstructorData *sd) {
    return new ("", 0) Showcase(sd);
}

static MenuScreen *CreateFadeScreen(ScreenConstructorData *sd) {
    return new ("", 0) FadeScreen(sd);
}

static MenuScreen *CreateWorldMap(ScreenConstructorData *sd) {
    return new ("", 0) WorldMap(sd);
}

static MenuScreen *CreateSMS(ScreenConstructorData *sd) {
    return new ("", 0) uiSMS(sd);
}

static MenuScreen *CreateSMSMessage(ScreenConstructorData *sd) {
    return new ("", 0) uiSMSMessage(sd);
}

static MenuScreen *CreateControllerUnplugged(ScreenConstructorData *sd) {
    return new ("", 0) ControllerUnplugged(sd);
}

static MenuScreen *CreateMovieScreen(ScreenConstructorData *sd) {
    return new ("", 0) MovieScreen(sd);
}

static MenuScreen *CreateSplashScreen(ScreenConstructorData *sd) {
    return new ("", 0) SplashScreen(sd);
}

static MenuScreen *CreateLanguageSelectScreen(ScreenConstructorData *sd) {
    return new ("", 0) LanguageSelectScreen(sd);
}

static MenuScreen *CreateSixDaysLaterScreen(ScreenConstructorData *sd) {
    return new ("", 0) SixDaysLater(sd);
}

static MenuScreen *CreateEngageEventDialog(ScreenConstructorData *sd) {
    return new ("", 0) nsEngageEventDialog::EngageEventDialog(sd);
}

static MenuScreen *CreateUISafeHouseRaceSheet(ScreenConstructorData *sd) {
    return new ("", 0) UISafehouseRaceSheet(sd);
}

static MenuScreen *CreateUIRapSheetLogin(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetLogin(sd);
}

static MenuScreen *CreateUIRapSheetMain(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetMain(sd);
}

static MenuScreen *CreateUIRapSheetRS(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetRS(sd);
}

static MenuScreen *CreateUIRapSheetUS(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetUS(sd);
}

static MenuScreen *CreateUIRapSheetVD(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetVD(sd);
}

static MenuScreen *CreateUIRapSheetCTS(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetCTS(sd);
}

static MenuScreen *CreateUIRapSheetTEP(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetTEP(sd);
}

static MenuScreen *CreateUIRapSheetPD(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetPD(sd);
}

static MenuScreen *CreateUIRapSheetRankings(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetRankings(sd);
}

static MenuScreen *CreateUIRapSheetRankingsDetail(ScreenConstructorData *sd) {
    return new ("", 0) uiRapSheetRankingsDetail(sd);
}

static MenuScreen *CreateUISafeHouseRepSheetMain(ScreenConstructorData *sd) {
    return new ("", 0) uiRepSheetMain(sd);
}

static MenuScreen *CreateUISafeHouseRivalChallenge(ScreenConstructorData *sd) {
    return new ("", 0) uiRepSheetRival(sd);
}

static MenuScreen *CreateUISafeHouseRivalBio(ScreenConstructorData *sd) {
    return new ("", 0) uiRepSheetRivalBio(sd);
}

static MenuScreen *CreateUISafeHouseMilestones(ScreenConstructorData *sd) {
    return new ("", 0) uiRepSheetMilestones(sd);
}

static MenuScreen *CreateUISafeHouseRegionUnlock(ScreenConstructorData *sd) {
    return new ("", 0) uiSafehouseRegionUnlock(sd);
}

static MenuScreen *CreateUISafeHouseBounty(ScreenConstructorData *sd) {
    return new ("", 0) uiRepSheetBounty(sd);
}

static MenuScreen *CreateUISafeHouseMarkers(ScreenConstructorData *sd) {
    return new ("", 0) FEMarkerSelection(sd);
}

static MenuScreen *CreateGameWonScreen(ScreenConstructorData *sd) {
    return new ("", 0) FEGameWonScreen(sd);
}

static MenuScreen *CreateDebugCarCustomize(ScreenConstructorData *sd) {
    return new ("", 0) DebugCarCustomizeScreen(sd);
}

static MenuScreen *CreateMyCarsManager(ScreenConstructorData *sd) {
    return new ("", 0) MyCarsManager(sd);
}

static MenuScreen *CreateCustomizeMainScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeMain(sd);
}

static MenuScreen *CreateCustomizeSubScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeSub(sd);
}

static MenuScreen *CreateCustomizeShoppingCartScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeShoppingCart(sd);
}

static MenuScreen *CreateCustomizePartsScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeParts(sd);
}

static MenuScreen *CreateCustomHUDColorScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeHUDColor(sd);
}

static MenuScreen *CreateDecalsScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeDecals(sd);
}

static MenuScreen *CreateNumbersScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeNumbers(sd);
}

static MenuScreen *CreatePaintScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizePaint(sd);
}

static MenuScreen *CreateRimmingScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeRims(sd);
}

static MenuScreen *CreateSpoilersScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizeSpoiler(sd);
}

static MenuScreen *CreateCustomizePerformanceScreen(ScreenConstructorData *sd) {
    return new ("", 0) CustomizePerformance(sd);
}

static MenuScreen *CreateBustedOverlayScreen(ScreenConstructorData *sd) {
    return new ("", 0) BustedOverlayScreen(sd);
}

static MenuScreen *CreatePostRacePursuitScreen(ScreenConstructorData *sd) {
    return new ("", 0) PostRacePursuitScreen(sd);
}

static MenuScreen *CreatePostRaceMilestonesScreen(ScreenConstructorData *sd) {
    return new ("", 0) PostRaceMilestonesScreen(sd);
}

static MenuScreen *CreateCreditsScreen(ScreenConstructorData *sd) {
    return new ("", 0) uiCredits(sd);
}

static MenuScreen *CreateUIEATraxScreen(ScreenConstructorData *sd) {
    return new ("", 0) UIEATraxScreen(sd);
}

static MenuScreen *CreateOptionsControllerScreen(ScreenConstructorData *sd) {
    return new ("", 0) UIOptionsController(sd);
}

typedef MenuScreen *(*ScreenCreateFunc)(ScreenConstructorData *);

struct ScreenFactoryDatum {
    const char *Name;
    ScreenCreateFunc CreateFunc;
};

static ScreenFactoryDatum ScreenFactoryData[] = {
    {"MainMenu.fng", CreateMainMenu},
    {"MainMenu_Sub.fng", CreateSubMenu},
    {"Options.fng", CreateOptionsScreen},
    {"OptionsPCDisplay.fng", CreateOptionsScreen},
    {"Quick_Race_Brief.fng", CreateQRBrief},
    {"Track_Select.fng", CreateQRTrackSelect},
    {"Track_Options.fng", CreateQRTrackOptions},
    {"Car_Select.fng", CreateQRCarSelect},
    {"PressStart.fng", CreateQRPressStart},
    {"ChallengeSeries.fng", CreateQRChallengeSeries},
    {"Showcase.fng", CreateShowcase},
    {"Pause_Main.fng", CreateCommonPauseMenu},
    {"Pause_Performance_Tuning.fng", CreateCustomTuningScreen},
    {"FadeScreen.fng", CreateFadeScreen},
    {"WorldMapMain.fng", CreateWorldMap},
    {"Pause_Options.fng", CreateOptionsScreen},
    {"HUD_SingleRace.fng", nullptr},
    {"HUD_Drag.fng", nullptr},
    {"InGameAnyMovie.fng", nullptr},
    {"WS_InGameAnyMovie.fng", nullptr},
    {"InGameAnyTutorial.fng", nullptr},
    {"EngageEventDialog.fng", CreateEngageEventDialog},
    {"SafehouseRaceSheet.fng", CreateUISafeHouseRaceSheet},
    {"OPM_SafehouseRaceSheet.fng", CreateUISafeHouseRaceSheet},
    {"SafehouseReputationOverview.fng", CreateUISafeHouseRepSheetMain},
    {"RapSheetLogin.fng", CreateUIRapSheetLogin},
    {"RapSheetLogin2.fng", CreateUIRapSheetLogin},
    {"RapSheetMain.fng", CreateUIRapSheetMain},
    {"RapSheetRS.fng", CreateUIRapSheetRS},
    {"RapSheetUS.fng", CreateUIRapSheetUS},
    {"RapSheetVD.fng", CreateUIRapSheetVD},
    {"RapSheetCTS.fng", CreateUIRapSheetCTS},
    {"RapSheetTEP.fng", CreateUIRapSheetTEP},
    {"RapSheetPD.fng", CreateUIRapSheetPD},
    {"RapSheetRankings.fng", CreateUIRapSheetRankings},
    {"RapSheetRankingsDetail.fng", CreateUIRapSheetRankingsDetail},
    {"SafehouseRivalChallenge.fng", CreateUISafeHouseRivalChallenge},
    {"SafehouseRivalBio.fng", CreateUISafeHouseRivalBio},
    {"SafehouseMilestones.fng", CreateUISafeHouseMilestones},
    {"SafehouseRegionUnlock.fng", CreateUISafeHouseRegionUnlock},
    {"SafehouseBounty.fng", CreateUISafeHouseBounty},
    {"SafehouseMarkers.fng", CreateUISafeHouseMarkers},
    {"SixDaysLater.fng", CreateSixDaysLaterScreen},
    {"InGameRaceSheet.fng", CreateUISafeHouseRaceSheet},
    {"InGameReputationOverview.fng", CreateUISafeHouseRepSheetMain},
    {"InGameMilestones.fng", CreateUISafeHouseMilestones},
    {"InGameRivalChallenge.fng", CreateUISafeHouseRivalChallenge},
    {"InGameRivalBio.fng", CreateUISafeHouseRivalBio},
    {"InGameBounty.fng", CreateUISafeHouseBounty},
    {"SMS_Mailboxes.fng", CreateSMS},
    {"SMS_Message.fng", CreateSMSMessage},
    {"GameWon.fng", CreateGameWonScreen},
    {"RapSheetLogin_ENDGAME.fng", CreateGameWonScreen},
    {"RapSheetLogin2_ENDGAME.fng", CreateGameWonScreen},
    {"RapSheetMain_ENDGAME.fng", CreateGameWonScreen},
    {"ControllerUnplugged.fng", CreateControllerUnplugged},
    {"UI_DebugCarCustomize.fng", CreateDebugCarCustomize},
    {"MyCarsManager.fng", CreateMyCarsManager},
    {"CustomizeMain.fng", CreateCustomizeMainScreen},
    {"CustomizeCategory.fng", CreateCustomizeSubScreen},
    {"CustomizeCategory_BACKROOM.fng", CreateCustomizeSubScreen},
    {"CustomizeGenericTop.fng", CreateCustomizeSubScreen},
    {"CustomizeGenericTop_BACKROOM.fng", CreateCustomizeSubScreen},
    {"ShoppingCart.fng", CreateCustomizeShoppingCartScreen},
    {"ShoppingCart_QR.fng", CreateCustomizeShoppingCartScreen},
    {"ShoppingCart_BACKROOM.fng", CreateCustomizeShoppingCartScreen},
    {"CustomizeParts.fng", CreateCustomizePartsScreen},
    {"CustomizeParts_BACKROOM.fng", CreateCustomizePartsScreen},
    {"CustomHUD.fng", CreateCustomizePartsScreen},
    {"CustomHUD_BACKROOM.fng", CreateCustomizePartsScreen},
    {"CustomHUDColor.fng", CreateCustomHUDColorScreen},
    {"CustomHUDColor_BACKROOM.fng", CreateCustomHUDColorScreen},
    {"Decals.fng", CreateDecalsScreen},
    {"Decals_BACKROOM.fng", CreateDecalsScreen},
    {"Numbers.fng", CreateNumbersScreen},
    {"Paint.fng", CreatePaintScreen},
    {"Paint_BACKROOM.fng", CreatePaintScreen},
    {"Rims.fng", CreateRimmingScreen},
    {"Rims_BACKROOM.fng", CreateRimmingScreen},
    {"Spoilers.fng", CreateSpoilersScreen},
    {"Spoilers_BACKROOM.fng", CreateSpoilersScreen},
    {"CustomizePerformance.fng", CreateCustomizePerformanceScreen},
    {"CustomizePerformance_BACKROOM.fng", CreateCustomizePerformanceScreen},
    {"GarageMain.fng", nullptr},
    {"DiscError.fng", nullptr},
    {"Dialog.fng", nullptr},
    {"GenericDialog_ThreeButton.fng", nullptr},
    {"GameOver.fng", nullptr},
    {"EA_Trax_Jukebox.fng", CreateUIEATraxScreen},
    {"EA_Trax.fng", nullptr},
    {"Chyron_IG.fng", nullptr},
    {"InGameDialog.fng", nullptr},
    {"Keyboard.fng", nullptr},
    {"Keyboard_GC.fng", nullptr},
    {"ScreenPrintf.fng", nullptr},
    {"Credits.fng", CreateCreditsScreen},
    {"FEAnyMovie.fng", nullptr},
    {"WS_FEAnyMovie.fng", nullptr},
    {"FEAnyTutorial.fng", nullptr},
    {"LS_EALogo.fng", CreateSplashScreen},
    {"LS_EA_hidef.fng", CreateSplashScreen},
    {"LS_PSA.fng", CreateSplashScreen},
    {"LS_THXMovie.fng", CreateSplashScreen},
    {"MW_LS_IntroFMV.fng", CreateSplashScreen},
    {"MW_LS_AttractFMV.fng", CreateSplashScreen},
    {"MW_LS_Splash.fng", CreateMovieScreen},
    {"WS_LS_EALogo.fng", CreateSplashScreen},
    {"WS_LS_EA_hidef.fng", CreateSplashScreen},
    {"WS_LS_PSA.fng", CreateSplashScreen},
    {"WS_LS_IntroFMV.fng", CreateSplashScreen},
    {"WS_MW_LS_AttractFMV.fng", CreateSplashScreen},
    {"WS_MW_LS_Splash.fng", CreateMovieScreen},
    {"Loading_Tips.fng", CreateLoadingTipsScreen},
    {"loading_boot.fng", nullptr},
    {"LS_LangSelect.fng", CreateLanguageSelectScreen},
    {"Loading.fng", CreateLoadingScreen},
    {"WS_Loading.fng", CreateLoadingScreen},
    {"Loading_Controller.fng", CreateLoadingControllerScreen},
    {"WS_Loading_Controller.fng", CreateLoadingControllerScreen},
    {"UI_OptionsController.fng", CreateOptionsControllerScreen},
    {"Pause_Controller.fng", CreateOptionsControllerScreen},
    {"PostRace_Results.fng", CreatePostRaceResultsScreen},
    {"BUSTED_OVERLAY.fng", CreateBustedOverlayScreen},
    {"PostBusted.fng", CreatePostRacePursuitScreen},
    {"Infractions.fng", nullptr},
    {"InGamePhotoMaster.fng", nullptr},
    {"PostRace_Pursuit.fng", CreatePostRacePursuitScreen},
    {"PostRace_MilestoneRewards.fng", CreatePostRaceMilestonesScreen},
    {"MC_ProfileManager.fng", nullptr},
    {"MC_Deleteprofile.fng", nullptr},
    {"MC_Bootup.fng", nullptr},
    {"MC_Bootup_GC.fng", nullptr},
    {"MC_List.fng", nullptr},
    {"InGame_MC_Main.fng", nullptr},
    {"InGame_MC_Main_GC.fng", nullptr},
    {"MC_Main.fng", nullptr},
    {"MC_Main_GC.fng", nullptr},
};

static const int kScreenFactoryDataCount = sizeof(ScreenFactoryData) / sizeof(ScreenFactoryData[0]);

static ScreenFactoryDatum *FindScreenCreateData(unsigned int hash) {
    for (int i = 0; i < kScreenFactoryDataCount; i++) {
        unsigned int nameHash = FEHashUpper(ScreenFactoryData[i].Name);
        if (hash == nameHash) {
            return &ScreenFactoryData[i];
        }
    }
    return nullptr;
}

static MenuScreen *ScreenFactory(unsigned int hash, FEPackage *pkg, int arg) {
    for (int i = 0; i < kScreenFactoryDataCount; i++) {
        unsigned int nameHash = FEHashUpper(ScreenFactoryData[i].Name);
        if (hash == nameHash && ScreenFactoryData[i].CreateFunc) {
            ScreenConstructorData sd;
            sd.PackageFilename = ScreenFactoryData[i].Name;
            sd.pPackage = pkg;
            sd.Arg = arg;
            return ScreenFactoryData[i].CreateFunc(&sd);
        }
    }
    return nullptr;
}

void FEPackageData::Activate(FEPackage *pkg, int arg) {
    pPackage = pkg;
    pkg->SetUserParam(reinterpret_cast<unsigned long>(this));
    mInScreenConstructor++;
    pScreen = ScreenFactory(GetNameHash(), pkg, arg);
    LastKnownControlMask = pkg->GetControlMask();
    mInScreenConstructor--;
}

void FEPackageData::NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) {
    if (pScreen) {
        pScreen->BaseNotify(Message, pObject, Param1, Param2);
    }
}

void FEPackageData::NotifySoundMessage(unsigned long msg, FEObject *obj, unsigned long control_mask, unsigned long pkg_ptr) {
    if (pScreen) {
        pScreen->BaseNotifySound(msg, obj, control_mask, pkg_ptr);
    }
}

FEPackageData::FEPackageData(bChunk *chunk) {
    IsVisible = 1;
    MyChunk = chunk;
    pScreen = nullptr;
    pPackage = nullptr;
    LastKnownControlMask = 0;
    bWasSetupForHotchunk = 0;
    IsPermanent = 0;
    CreateData = nullptr;
    RenderInfo.EpolySlotPool = nullptr;
    RenderInfo.AllowOverflows = false;
    mArg = 0;

    if (chunk->GetID() == 0x30203) {
        DataChunk = reinterpret_cast<int *>(chunk) + 2;
    } else if (chunk->GetID() == 0x30210) {
        DataChunk = nullptr;
    }

    CreateData = FindScreenCreateData(GetNameHash());
}

FEPackageData::~FEPackageData() {
    if (MyChunk->GetID() != 0x30203 && MyChunk->GetID() == 0x30210) {
        bFree(DataChunk);
    }
    DataChunk = nullptr;
}

void FEPackageData::UnActivate() {
    if (pScreen) {
        delete pScreen;
    }
    pScreen = nullptr;
    if (pPackage) {
        pPackage->SetUserParam(0);
    }
    pPackage = nullptr;
    if (MyChunk->GetID() == 0x30210) {
        bFree(DataChunk);
        DataChunk = nullptr;
    }
}

void FEPackageData::Close() {
    if (pPackage) {
        HackClearCache(pPackage);
        RenderObjectDisconnect cb(HACK_FEPkgMgr_GetPackageRenderInfo(pPackage), cFEngRender::mInstance);
        pPackage->ForAllObjects(cb);
        cFEng_mInstance->mFEng->UnloadPackage(pPackage);
        UnActivate();
    }
}

void *FEPackageData::GetDataChunk() {
    if (MyChunk->GetID() == 0x30203) {
        return DataChunk;
    }
    if (MyChunk->GetID() == 0x30210) {
        int decompressedSize = reinterpret_cast<int *>(MyChunk)[5];
        DataChunk = bMalloc(decompressedSize, GetVirtualMemoryAllocParams());
        LZDecompress(reinterpret_cast<unsigned char *>(reinterpret_cast<int *>(MyChunk) + 3),
                     static_cast<unsigned char *>(DataChunk));
        return DataChunk;
    }
    return nullptr;
}

unsigned int FEPackageData::GetNameHash() {
    if (MyChunk->GetID() == 0x30203) {
        unsigned int *data = static_cast<unsigned int *>(GetDataChunk());
        unsigned int magic = data[0];
        unsigned int swappedMagic = (magic >> 24) | (magic << 24) | ((magic & 0xFF00) << 8) | ((magic >> 8) & 0xFF00);
        if (swappedMagic == 0xE76E4546) {
            unsigned int pkHd = data[2];
            unsigned int swappedPkHd = (pkHd >> 24) | (pkHd << 24) | ((pkHd & 0xFF00) << 8) | ((pkHd >> 8) & 0xFF00);
            if (swappedPkHd == 0x64486B50) {
                unsigned int version = data[4];
                unsigned int swappedVersion = (version >> 24) | (version << 24) | ((version & 0xFF00) << 8) | ((version >> 8) & 0xFF00);
                if (swappedVersion > 0x1FFFF) {
                    return FEHashUpper(reinterpret_cast<const char *>(data + 10));
                }
            }
        }
    } else if (MyChunk->GetID() == 0x30210) {
        return reinterpret_cast<unsigned int *>(MyChunk)[2];
    }
    return 0;
}