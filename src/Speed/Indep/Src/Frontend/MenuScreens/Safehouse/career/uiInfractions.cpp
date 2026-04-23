#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"

struct FECareerRecord;
void eUnloadStreamingTexture(unsigned int *textures, int count);
void WaitForResourceLoadingComplete();
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
void FEngSetVisible(FEObject *obj);
extern void FEngSetButtonState(const char *pkg_name, unsigned int button_hash, bool enabled);
extern void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(unsigned int), unsigned int param0, int priority);
extern void FEngSetCurrentButton(const char *pkg_name, unsigned int button_hash);

inline void FEngSetInvisible(const char *pkg_name, unsigned int obj_hash) {
    FEngSetInvisible(FEngFindObject(pkg_name, obj_hash));
}

inline void FEngDisableButton(const char *pkg_name, unsigned int button_hash) {
    FEngSetButtonState(pkg_name, button_hash, false);
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

PostPursuitInfractionsScreen::PostPursuitInfractionsScreen(ScreenConstructorData *sd)
    : MenuScreen(sd)
{
    WorkingCareerRecord = nullptr;
    bStrikeLimitReached = false;
    BustedTexture = 0;
    bFirstTimeBusted = false;

    if (!FEDatabase->GetCareerSettings()->HasBeenBustedOnce()) {
        if (TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0) < 1) {
            for (int i = 0; i < 4; i++) {
                TheFEMarkerManager.AddMarkerToInventory(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
            }
        }
        bFirstTimeBusted = true;
    }

    FEDatabase->GetCareerSettings()->SetBeenBustedOnce();

    const int FEObj_BustedStamp = 0x2347122A;
    FEngSetInvisible(GetPackageName(), FEObj_BustedStamp);

    BustedTexture = CalcBustedTexture();
    FEngSetTextureHash(GetPackageName(), FEObj_BustedStamp, BustedTexture);
    eLoadStreamingTexture(BustedTexture, TextureLoadedCallback, reinterpret_cast<unsigned int>(this), 0);

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *record = stable->GetCarRecordByHandle(FEDatabase->GetCareerSettings()->GetCurrentCar());
    WorkingCareerRecord = stable->GetCareerRecordByHandle(record->CareerHandle);

    FEInfractionsData scott_says_i_should_call_this_previous_infractions_and_phil_needs_to_have_it_spelled_correctly(GInfractionManager::Get().GetInfractions());

    int this_pursuit_cost = scott_says_i_should_call_this_previous_infractions_and_phil_needs_to_have_it_spelled_correctly.GetFineValue();
    const int FEObj_THISPURSUITCOST = 0xBD66334A;
    FEPrintf(GetPackageName(), FEObj_THISPURSUITCOST, "%$d", this_pursuit_cost);

    int num_infractions_pursuit = scott_says_i_should_call_this_previous_infractions_and_phil_needs_to_have_it_spelled_correctly.NumInfractions();
    const int FEObj_NUMBEROFINFRACTIONSTHISPURSUIT = 0xB967F64D;
    FEPrintf(GetPackageName(), FEObj_NUMBEROFINFRACTIONSTHISPURSUIT, "%d", num_infractions_pursuit);

    int infraction_total_cost = WorkingCareerRecord->GetInfractions(true).GetFineValue();
    const int FEObj_UNSERVEDINFRACTIONSCOST = 0xA4C79522;
    FEPrintf(GetPackageName(), FEObj_UNSERVEDINFRACTIONSCOST, "%$d", infraction_total_cost - this_pursuit_cost);

    int total_unserved_number = WorkingCareerRecord->GetInfractions(true).NumInfractions();
    const int FEObj_NUMBEROFINFRACTIONSUNSERVED = 0x5344F2A6;
    FEPrintf(GetPackageName(), FEObj_NUMBEROFINFRACTIONSUNSERVED, "%d", total_unserved_number - num_infractions_pursuit);

    bHasMarker = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0) > 0;

    FEPrintf(GetPackageName(), 0x5B875870, "%d", TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0));
    FEPrintf(GetPackageName(), 0xEA8AECD9, "%d", TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0));

    if (!bHasMarker) {
        const int FEObj_Button1Text = 0xF9363F30;
        const int GREY = 0x163C76;
        const int FEObj_MARKER = 0x6B6973C1;
        const int FEObj_Button1 = 0xB8A7C6CC;
        FEngSetScript(GetPackageName(), FEObj_Button1Text, GREY, true);
        FEngSetScript(GetPackageName(), FEObj_MARKER, GREY, true);
        FEngSetScript(GetPackageName(), 0x39F11E5C, GREY, true);
        FEngDisableButton(GetPackageName(), FEObj_Button1);
    } else {
        const int FEObj_NORMAL = 0x6EBBFB68;
        FEngSetScript(GetPackageName(), 0x39F11E5C, FEObj_NORMAL, true);
    }

    AmountToPay = WorkingCareerRecord->GetInfractions(true).GetFineValue();
    const int FEObj_TOTALCOSTDATA = 0x854AF1F4;
    FEPrintf(GetPackageName(), FEObj_TOTALCOSTDATA, "%$d", AmountToPay);

    AmountPlayerHas = FEDatabase->GetCareerSettings()->GetCash();
    const int FEObj_CASHDATA = 0x1930B057;
    FEPrintf(GetPackageName(), FEObj_CASHDATA, "%$d", AmountPlayerHas);
}

void PostPursuitInfractionsScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
    case 0x35f8620b: {
        const int FEObj_Button2 = 0xB8A7C6CC;
        const int FEObj_Button1 = 0xB8A7C6CD;
        if (bFirstTimeBusted) {
            FEngSetCurrentButton(GetPackageName(), FEObj_Button2);
            DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417b2601, 0xb4edeb6d, 0x9c14b5f1);
        } else {
            FEngSetCurrentButton(GetPackageName(), FEObj_Button1);
        }
        break;
    }
    case 0x0c407210: {
        bool paid_with_cash = (pobj->NameHash == 0xB8A7C6CD);
        bool paid_with_marker = (pobj->NameHash == 0xB8A7C6CC);
        bool not_enough_cash;
        bool busted_by_cross;

        if (paid_with_cash) {
            FEDatabase->GetCareerSettings()->SpendCash(AmountToPay);
            bStrikeLimitReached = WorkingCareerRecord->TheImpoundData.NotifyBusted();
            WorkingCareerRecord->ServeAllIncractions();
            WorkingCareerRecord->SetVehicleHeat(1.0f);
        } else if (paid_with_marker) {
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
            int num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
            FEPrintf(GetPackageName(), 0x5b875870, "%d", num_markers);
            FEPrintf(GetPackageName(), 0xea8aecd9, "%d", num_markers);
            if (num_markers <= 0) {
                const int GREY = 0x163c76;
                const int FEObj_MARKER = 0x6b6973c1;
                FEngSetScript(GetPackageName(), FEObj_MARKER, GREY, true);
                FEngSetScript(GetPackageName(), 0x39f11e5c, GREY, true);
            }
            WorkingCareerRecord->WaiveIncractions(GInfractionManager::Get().GetInfractions());
        }

        not_enough_cash = false;
        if (!paid_with_marker) {
            not_enough_cash = (AmountToPay > AmountPlayerHas);
        }

        FEImpoundData::eImpoundReasons impound_reason = FEImpoundData::IMPOUND_REASON_NONE;
        unsigned int message_hash = 0;
        if (bStrikeLimitReached) {
            message_hash = 0x78f0e298;
            impound_reason = FEImpoundData::IMPOUND_REASON_STRIKE_LIMIT_REACHED;
        } else if (not_enough_cash) {
            message_hash = 0x1ecffa6e;
            impound_reason = FEImpoundData::IMPOUND_REASON_INSUFFICIENT_FUNDS;
        }

        if (message_hash != 0) {
            WorkingCareerRecord->TheImpoundData.BecomeImpounded(impound_reason);
            DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417b2601, 0x34dc1bec, message_hash);
        } else {
            if (paid_with_marker) {
                cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0x100, 0, false);
            }
        }
        break;
    }
    case 0x34dc1bec:
        cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0x200, 0, false);
        break;
    }
}
