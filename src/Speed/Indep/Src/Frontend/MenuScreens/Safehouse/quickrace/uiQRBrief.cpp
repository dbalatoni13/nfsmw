#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

// extern FEImage *FEngFindImage(const char *pkg, int hash);
// extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
// struct TextureInfo;
// extern TextureInfo *GetTextureInfo(unsigned int hash, int, int);
// extern unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *params);
// extern bool DoesStringExist(unsigned int hash);
// extern int FEPrintf(const char *pkg, int hash, const char *fmt, ...);
// extern void FEngSetLanguageHash(const char *pkg, unsigned int hash, unsigned int lang_hash);
// extern const char *GetLocalizedString(unsigned int hash);
// extern void FEngSetScript(const char *pkg, unsigned int hash, unsigned int script, bool);
// extern void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar car);
// extern void PlayUISoundFX(EAXSound *snd, eMenuSoundTriggers trigger);
// // extern EAXSound *g_pEAXSound;
// extern void StartRace();

UIQRBrief::UIQRBrief(ScreenConstructorData *sd)
    : MenuScreen(sd),          //
      pSelectedCar(nullptr),   //
      pSelectedTrack(nullptr), //
      randomCount(0) {
    Setup();
    raceSettings.CopsOn = false;
    AccelerationSlider.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    TopSpeedSlider.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    HandlingSlider.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
}

void UIQRBrief::RefreshHeader() {
    FECarRecord *car_rec = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pSelectedCar->mHandle);
    unsigned int manu_logo = car_rec->GetManuLogoHash();
    if (GetTextureInfo(manu_logo, 0, 0)) {
        FEngSetTextureHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x3e01ad1d, manu_logo);
    } else {
        FEngSetTextureHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x3e01ad1d, FEHashUpper("GENERICPLACEHOLDER"));
    }
    unsigned int car_logo = car_rec->GetLogoHash();
    if (GetTextureInfo(car_logo, 0, 0)) {
        FEngSetTextureHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb05dd708, car_logo);
    } else {
        FEngSetTextureHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb05dd708, FEHashUpper("GENERICPLACEHOLDER"));
    }
    GRaceParameters *track_params = pSelectedTrack->pRaceParams;
    FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb5154998, FEDatabase->GetRaceNameHash(track_params->GetRaceType()));
    FEngSetTextureHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x2521e5eb, FEDatabase->GetRaceIconHash(track_params->GetRaceType()));
    unsigned int track_name = CalcLanguageHash("TRACKNAME_", track_params);
    if (DoesStringExist(track_name)) {
        FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb5154999, track_name);
    } else {
        FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb5154999, track_params->GetEventID());
    }
    const char *unit_str;
    if (FEDatabase->GetUserProfile(0)->GetOptions()->TheGameplaySettings.SpeedoUnits == 1) {
        unit_str = GetLocalizedString(0x8569a26a);
    } else {
        unit_str = GetLocalizedString(0x867dcfd9);
    }
    FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499a, "%$0.1f %s", track_params->GetRaceLengthMeters() * 0.001f, unit_str);
    GRace::Type race_type = track_params->GetRaceType();
    if (race_type == static_cast<GRace::Type>(1) || race_type == static_cast<GRace::Type>(3)) {
        FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499b, "%d", raceSettings.NumLaps);
    } else {
        FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499b, "--");
    }
    FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499c, "%d", raceSettings.NumOpponents);
    unsigned int ai_hash;
    switch (raceSettings.TrafficDensity) {
        case 1:
            ai_hash = LANGUAGE_COMMON_MINIMUM;
            break;
        case 2:
            ai_hash = 0xa2cca838;
            break;
        case 3:
            ai_hash = LANGUAGE_COMMON_MAXIMUM;
            break;
        case 0:
            ai_hash = LANGUAGE_COMMON_NONE;
            break;
        default:
            ai_hash = 0;
            break;
    }
    race_type = track_params->GetRaceType();
    if (race_type == static_cast<GRace::Type>(0) || track_params->GetRaceType() == static_cast<GRace::Type>(2)) {
        ai_hash = 0x7f2f7ad6;
    }
    FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499d, ai_hash);
    switch (raceSettings.AISkill) {
        case 0:
            ai_hash = 0x61973e01;
            break;
        case 1:
            ai_hash = 0x3747f6d0;
            break;
        case 2:
            ai_hash = 0x6198e2ee;
            break;
        default:
            ai_hash = 0;
            break;
    }
    FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499e, ai_hash);
    ai_hash = LANGUAGE_COMMON_OFF;
    if (raceSettings.CatchUp) {
        ai_hash = LANGUAGE_COMMON_ON;
    }
    FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb515499e, ai_hash);
    UpdateSliders();
}

void UIQRBrief::UpdateSliders() {
    Physics::Info::Performance stock_perf;
    Physics::Info::Performance tuned_perf;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car_rec = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    Attrib::Gen::pvehicle pveh(car_rec->VehicleKey, 0, nullptr);
    bool hasCustomization = (car_rec->Customization != 0xff);
    if (hasCustomization) {
        FECustomizationRecord *cust = stable->GetCustomizationRecordByHandle(car_rec->Customization);
        cust->WriteRecordIntoPhysics(pveh);
    }
    Physics::Info::EstimatePerformance(pveh, stock_perf);

    AccelerationSlider.SetValue(stock_perf.Acceleration);
    AccelerationSlider.SetPreviewValue(stock_perf.Acceleration);
    AccelerationSlider.Draw();

    TopSpeedSlider.SetValue(stock_perf.TopSpeed);
    TopSpeedSlider.SetPreviewValue(stock_perf.TopSpeed);
    TopSpeedSlider.Draw();

    HandlingSlider.SetValue(stock_perf.Handling);
    HandlingSlider.SetPreviewValue(stock_perf.Handling);
    HandlingSlider.Draw();
}

void UIQRBrief::Setup() {
    FilteredCarsList.DeleteAllElements();
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    unsigned int current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    for (int i = 0; i < 200; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(0xf0001)) {
            unsigned int fekey = fe_car->FEKey;
            Attrib::Gen::frontend fe_attrib(fekey, 0, nullptr);
            unsigned char unlocked_at = fe_attrib.UnlockedAt();
            if (static_cast<int>(unlocked_at) >= static_cast<int>(current_bin) - 1 && static_cast<int>(unlocked_at) <= static_cast<int>(current_bin) + 3 &&
                fe_attrib.GetCollection() != static_cast<unsigned int>(-0x3e3cd251)) {
                FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(fe_car->Handle, false));
            }
        }
    }
    for (unsigned int i = 0; i < GRaceDatabase::Get().GetRaceCount(); i++) {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceParameters(i);
        if (parms->GetRaceType() != static_cast<GRace::Type>(8) && parms->GetRaceType() != static_cast<GRace::Type>(9) &&
            parms->GetRaceType() != static_cast<GRace::Type>(10) && parms->GetRaceType() != static_cast<GRace::Type>(-1) &&
            parms->GetRaceType() != static_cast<GRace::Type>(4)) {
            if (parms->GetEventHash() != Attrib::StringHash32("19.8.31")) {
                if (UnlockSystem::IsEventAvailable(parms->GetEventHash())) {
                    if (UnlockSystem::IsTrackUnlocked(static_cast<eUnlockFilters>(1), parms->GetEventHash(), 0)) {
                        FilteredTracksList.AddTail(new ("SelectableTrack", 0) SelectableTrack(parms, true, 0));
                    }
                }
            }
        }
    }
    cFEng::Get()->QueueGameMessage(0xc519bfc4, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xff);
}

void UIQRBrief::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0xc98356ba: {
            if (randomCount < 1)
                return;
            SelectableCar *next_car = reinterpret_cast<bTNode<SelectableCar> *>(pSelectedCar)->GetNext();
            if (next_car == static_cast<SelectableCar *>(FilteredCarsList.EndOfList())) {
                next_car = FilteredCarsList.GetHead();
            }
            pSelectedCar = next_car;
            SelectableTrack *next_track = reinterpret_cast<bTNode<SelectableTrack> *>(pSelectedTrack)->GetNext();
            if (next_track == static_cast<SelectableTrack *>(FilteredTracksList.EndOfList())) {
                next_track = FilteredTracksList.GetHead();
            }
            pSelectedTrack = next_track;
            FEDatabase->GetRandomRaceOptions(&raceSettings, pSelectedTrack->pRaceParams->GetRaceType());
            RefreshHeader();
            randomCount--;
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x8b));
            if (randomCount != 0)
                return;
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *car_rec = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
            Attrib::Gen::frontend fe_attrib(car_rec->FEKey, 0, nullptr);
            int unlocked_at = fe_attrib.UnlockedAt();
            if (unlocked_at < FEDatabase->GetUserProfile(0)->GetCareer()->GetCurrentBin()) {
                FEngSetScript(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xfe8fdbf7, FEHASH_APPEAR, true);
                char buf[128];
                int req_bin = unlocked_at + 1;
                FEngSNPrintf(buf, 128, "blacklist_rival_%02d_aka", req_bin);
                const char *pkg = reinterpret_cast<MenuScreen *>(this)->GetPackageName();
                const char *locked_str = GetLocalizedString(0x4ef2a115);
                unsigned int bin_hash = FEHashUpper(buf);
                const char *bin_name = GetLocalizedString(bin_hash);
                FEPrintf(pkg, 0xfe8fdbf7, locked_str, bin_name, req_bin);
            }
            RideInfo ride;
            stable->BuildRideForPlayer(pSelectedCar->mHandle, 0, &ride);
            ride.SetRandomPaint();
            ride.SetRandomParts();
            CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
            break;
        }
        case 0x406415e3: {
            cFrontendDatabase *db = FEDatabase;
            char port = FEngMapJoyParamToJoyport(param1);
            db->SetPlayersJoystickPort(0, port);
            break;
        }
        case 0xe1fde1d1: {
            RaceSettings *qr_settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            qr_settings->SelectedCar[0] = 0x12345678;
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *placeholder = stable->GetCarRecordByHandle(0x12345678);
            FECarRecord *real_car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
            *placeholder = *real_car;
            placeholder->FilterBits = 0xf0020;
            FECustomizationRecord *cust_rec = stable->GetCustomizationRecordByHandle(placeholder->Customization);
            RideInfo *player_ride = CarViewer::GetRideInfo(static_cast<eCarViewerWhichCar>(0));
            cust_rec->WriteRideIntoRecord(player_ride);
            Attrib::Gen::pvehicle pveh(placeholder->VehicleKey, 0, nullptr);
            int max_nitrous = Physics::Upgrades::GetMaxLevel(pveh, static_cast<Physics::Upgrades::Type>(6));
            Physics::Upgrades::SetLevel(pveh, static_cast<Physics::Upgrades::Type>(6), max_nitrous);
            cust_rec->WritePhysicsIntoRecord(pveh);
            GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(pSelectedTrack->pRaceParams);
            FEDatabase->FillCustomRace(custom, &raceSettings);
            GRaceDatabase::Get().SetStartupRace(custom, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(custom);
            RaceStarter::StartRace();
            break;
        }
        case 0xc519bfc4:
            pSelectedCar = GetRandomCar();
            pSelectedTrack = GetRandomTrack();
            randomCount = 30;
            GarageMainScreen::GetInstance()->DisableCarRendering();
            cFEng::Get()->QueuePackageMessage(0xa05a328e, nullptr, nullptr);
            FEngSetScript(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xfe8fdbf7, 0x16a259, true);
            break;
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            break;
    }
}

SelectableCar *UIQRBrief::GetRandomCar() {
    int size = FilteredCarsList.CountElements();
    return FilteredCarsList.GetNode(bRandom(size));
}

SelectableTrack *UIQRBrief::GetRandomTrack() {
    int size = FilteredTracksList.CountElements();
    return FilteredTracksList.GetNode(bRandom(size));
}
