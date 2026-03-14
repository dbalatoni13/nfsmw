#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELanguageSelect.hpp"

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

static ScreenCreateFunc ScreenFactoryData[] = {
    CreateMainMenu,
    CreateOptionsScreen,
    CreateQRBrief,
    CreateQRTrackSelect,
    CreateQRTrackOptions,
    CreateQRCarSelect,
    CreateQRPressStart,
    CreateQRChallengeSeries,
    CreateShowcase,
    CreateFadeScreen,
    CreateWorldMap,
    CreateSMS,
    CreateSMSMessage,
    CreateControllerUnplugged,
    CreateMovieScreen,
    CreateSplashScreen,
    CreateLanguageSelectScreen,
    CreateSixDaysLaterScreen,
    CreateEngageEventDialog,
    CreateUISafeHouseRaceSheet,
    CreateUIRapSheetLogin,
    CreateUIRapSheetMain,
    CreateUIRapSheetRS,
    CreateUIRapSheetUS,
    CreateUIRapSheetVD,
    CreateUIRapSheetCTS,
    CreateUIRapSheetTEP,
    CreateUIRapSheetPD,
    CreateUIRapSheetRankings,
    CreateUIRapSheetRankingsDetail,
    CreateUISafeHouseRepSheetMain,
    CreateUISafeHouseRivalChallenge,
    CreateUISafeHouseRivalBio,
    CreateUISafeHouseMilestones,
    CreateUISafeHouseRegionUnlock,
    CreateUISafeHouseBounty,
    CreateUISafeHouseMarkers,
    CreateGameWonScreen,
    CreateDebugCarCustomize,
    CreateMyCarsManager,
    CreateCustomizeMainScreen,
    CreateCustomizeSubScreen,
    CreateCustomizeShoppingCartScreen,
    CreateCustomizePartsScreen,
    CreateCustomHUDColorScreen,
    CreateDecalsScreen,
    CreateNumbersScreen,
    CreatePaintScreen,
    CreateRimmingScreen,
    CreateSpoilersScreen,
    CreateCustomizePerformanceScreen,
    CreateBustedOverlayScreen,
    CreatePostRacePursuitScreen,
    CreatePostRaceMilestonesScreen,
    CreateCreditsScreen,
    CreateUIEATraxScreen,
    CreateOptionsControllerScreen,
};

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