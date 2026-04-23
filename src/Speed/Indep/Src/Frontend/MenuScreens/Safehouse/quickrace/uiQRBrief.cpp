#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

extern FEImage *FEngFindImage(const char *pkg, int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
struct TextureInfo;
extern TextureInfo *GetTextureInfo(unsigned int hash, int, int);
extern unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *params);
extern bool DoesStringExist(unsigned int hash);
extern int FEPrintf(const char *pkg, int hash, const char *fmt, ...);
extern void FEngSetLanguageHash(const char *pkg, unsigned int hash, unsigned int lang_hash);
extern const char *GetLocalizedString(unsigned int hash);
extern void FEngSetScript(const char *pkg, unsigned int hash, unsigned int script, bool);
extern void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar car);
extern void PlayUISoundFX(EAXSound *snd, eMenuSoundTriggers trigger);
extern EAXSound *g_pEAXSound;
extern void StartRace();

UIQRBrief::UIQRBrief(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      pSelectedCar(nullptr) //
      ,
      pSelectedTrack(nullptr) //
      ,
      randomCount(0) {
    raceSettings.Default();
    Setup();
    AccelerationSlider.Init(GetPackageName(), GetPackageName(), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    TopSpeedSlider.Init(GetPackageName(), GetPackageName(), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    HandlingSlider.Init(GetPackageName(), GetPackageName(), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

UIQRBrief::~UIQRBrief() {}

SelectableCar *UIQRBrief::GetRandomCar() {
    int size = FilteredCarsList.CountElements();
    return FilteredCarsList.GetNode(bRandom(size));
}

SelectableTrack *UIQRBrief::GetRandomTrack() {
    int size = FilteredTracksList.CountElements();
    return FilteredTracksList.GetNode(bRandom(size));
}

void UIQRBrief::Setup() {
    FilteredCarsList.DeleteAllElements();
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    unsigned int current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    for (int i = 0; i < 200; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(0xf0001)) {
            Attrib::Gen::frontend fe_attrib(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), fe_car->FEKey), 0, nullptr);
            unsigned char unlocked_at = fe_attrib.UnlockedAt();
            if (static_cast<int>(unlocked_at) >= static_cast<int>(current_bin - 1) && unlocked_at <= current_bin + 3 &&
                fe_attrib.GetCollection() != static_cast<unsigned int>(-0x3e3cd251)) {
                SelectableCar *car = new SelectableCar(fe_car->Handle, false);
                FilteredCarsList.AddTail(car);
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
                        SelectableTrack *track = new SelectableTrack(parms, true, 0);
                        FilteredTracksList.AddTail(track);
                    }
                }
            }
        }
    }
    cFEng::Get()->QueueGameMessage(0xc519bfc4, GetPackageName(), 0xff);
}

void UIQRBrief::RefreshHeader() {
    FECarRecord *car_rec = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pSelectedCar->mHandle);
    unsigned int manu_logo = car_rec->GetManuLogoHash();
    if (GetTextureInfo(manu_logo, 0, 0)) {
        FEImage *img = FEngFindImage(PackageFilename, 0x3e01ad1d);
        FEngSetTextureHash(img, manu_logo);
    } else {
        unsigned int placeholder = FEHashUpper("GENERICPLACEHOLDER");
        FEImage *img = FEngFindImage(PackageFilename, 0x3e01ad1d);
        FEngSetTextureHash(img, placeholder);
    }
    unsigned int car_logo = car_rec->GetLogoHash();
    if (GetTextureInfo(car_logo, 0, 0)) {
        FEImage *img = FEngFindImage(PackageFilename, 0xb05dd708);
        FEngSetTextureHash(img, car_logo);
    } else {
        unsigned int placeholder = FEHashUpper("GENERICPLACEHOLDER");
        FEImage *img = FEngFindImage(PackageFilename, 0xb05dd708);
        FEngSetTextureHash(img, placeholder);
    }
    GRaceParameters *track_params = pSelectedTrack->pRaceParams;
    unsigned int race_name = FEDatabase->GetRaceNameHash(track_params->GetRaceType());
    FEngSetLanguageHash(PackageFilename, 0xb5154998, race_name);
    unsigned int race_icon = FEDatabase->GetRaceIconHash(track_params->GetRaceType());
    FEImage *icon_img = FEngFindImage(PackageFilename, 0x2521e5eb);
    FEngSetTextureHash(icon_img, race_icon);
    unsigned int track_name = CalcLanguageHash("TRACKNAME_", track_params);
    if (DoesStringExist(track_name)) {
        FEngSetLanguageHash(PackageFilename, 0xb5154999, track_name);
    } else {
        FEPrintf(PackageFilename, 0xb5154999, track_params->GetEventID());
    }
    unsigned int unit_hash = 0x867dcfd9;
    if (FEDatabase->GetUserProfile(0)->GetOptions()->TheGameplaySettings.SpeedoUnits == 1) {
        unit_hash = 0x8569a26a;
    }
    const char *unit_str = GetLocalizedString(unit_hash);
    float race_length = track_params->GetRaceLengthMeters() * 0.001f;
    FEPrintf(PackageFilename, 0xb515499a, "%$0.1f %s", race_length, unit_str);
    GRace::Type race_type = track_params->GetRaceType();
    if (race_type == static_cast<GRace::Type>(1) || race_type == static_cast<GRace::Type>(3)) {
        FEPrintf(PackageFilename, 0xb515499b, "%d", raceSettings.NumLaps);
    } else {
        FEPrintf(PackageFilename, 0xb515499b, "--");
    }
    FEPrintf(PackageFilename, 0xb515499c, "%d", raceSettings.NumOpponents);
    unsigned int ai_hash;
    switch (raceSettings.TrafficDensity) {
        case 0:
            ai_hash = 0x8cdc3937;
            break;
        case 1:
            ai_hash = 0x73c615a3;
            break;
        case 2:
            ai_hash = 0xa2cca838;
            break;
        case 3:
            ai_hash = 0x61d1c5a5;
            break;
        default:
            ai_hash = 0;
            break;
    }
    race_type = track_params->GetRaceType();
    if (race_type == static_cast<GRace::Type>(0) || track_params->GetRaceType() == static_cast<GRace::Type>(2)) {
        ai_hash = 0x7f2f7ad6;
    }
    FEngSetLanguageHash(PackageFilename, 0xb515499d, ai_hash);
    unsigned int traffic_hash;
    switch (raceSettings.AISkill) {
        case 0:
            traffic_hash = 0x61973e01;
            break;
        case 1:
            traffic_hash = 0x3747f6d0;
            break;
        case 2:
            traffic_hash = 0x6198e2ee;
            break;
        default:
            traffic_hash = 0;
            break;
    }
    FEngSetLanguageHash(PackageFilename, 0xb515499e, traffic_hash);
    unsigned int cops_hash = 0x70dfe5c2;
    if (raceSettings.CatchUp) {
        cops_hash = 0x417b2604;
    }
    FEngSetLanguageHash(PackageFilename, 0xb515499e, cops_hash);
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
    float acc_val = stock_perf.Acceleration;
    float acc_min = AccelerationSlider.GetMin();
    float acc_max = AccelerationSlider.GetMax();
    if (acc_val < acc_min)
        acc_val = acc_min;
    if (acc_val > acc_max)
        acc_val = acc_max;
    AccelerationSlider.SetPreviewValue(acc_val);
    AccelerationSlider.Draw();

    TopSpeedSlider.SetValue(stock_perf.TopSpeed);
    float top_val = stock_perf.TopSpeed;
    float top_min = TopSpeedSlider.GetMin();
    float top_max = TopSpeedSlider.GetMax();
    if (top_val < top_min)
        top_val = top_min;
    if (top_val > top_max)
        top_val = top_max;
    TopSpeedSlider.SetPreviewValue(top_val);
    TopSpeedSlider.Draw();

    HandlingSlider.SetValue(stock_perf.Handling);
    float hdl_val = stock_perf.Handling;
    float hdl_min = HandlingSlider.GetMin();
    float hdl_max = HandlingSlider.GetMax();
    if (hdl_val < hdl_min)
        hdl_val = hdl_min;
    if (hdl_val > hdl_max)
        hdl_val = hdl_max;
    HandlingSlider.SetPreviewValue(hdl_val);
    HandlingSlider.Draw();
}

void UIQRBrief::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0xc98356ba: {
            if (randomCount < 1)
                return;
            SelectableCar *next_car = static_cast<SelectableCar *>(pSelectedCar->GetNext());
            if (next_car == static_cast<SelectableCar *>(FilteredCarsList.EndOfList())) {
                next_car = FilteredCarsList.GetHead();
            }
            pSelectedCar = next_car;
            SelectableTrack *next_track = static_cast<SelectableTrack *>(pSelectedTrack->GetNext());
            if (next_track == static_cast<SelectableTrack *>(FilteredTracksList.EndOfList())) {
                next_track = FilteredTracksList.GetHead();
            }
            pSelectedTrack = next_track;
            FEDatabase->GetRandomRaceOptions(&raceSettings, pSelectedTrack->pRaceParams->GetRaceType());
            RefreshHeader();
            randomCount--;
            PlayUISoundFX(g_pEAXSound, static_cast<eMenuSoundTriggers>(0x8b));
            if (randomCount != 0)
                return;
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *car_rec = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
            Attrib::Gen::frontend fe_attrib(car_rec->FEKey, 0, nullptr);
            int unlocked_at = fe_attrib.UnlockedAt();
            if (unlocked_at < FEDatabase->GetUserProfile(0)->GetCareer()->GetCurrentBin()) {
                FEngSetScript(PackageFilename, 0xfe8fdbf7, 0x5079c8f8, true);
                char buf[128];
                int req_bin = unlocked_at + 1;
                FEngSNPrintf(buf, 128, "BLACKLIST_%d", req_bin);
                const char *pkg = PackageFilename;
                const char *locked_str = GetLocalizedString(0x4ef2a115);
                unsigned int bin_hash = FEHashUpper(buf);
                const char *bin_name = GetLocalizedString(bin_hash);
                FEPrintf(pkg, 0xfe8fdbf7, locked_str, bin_name, req_bin);
            }
            RideInfo ride;
            stable->BuildRideForPlayer(pSelectedCar->mHandle, 0, &ride);
            ride.SetRandomPaint();
            ride.SetRandomParts();
            SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
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
            GRaceDatabase::Get().SetStartupRace(custom, kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(custom);
            StartRace();
            break;
        }
        case 0xc519bfc4:
            pSelectedCar = GetRandomCar();
            pSelectedTrack = GetRandomTrack();
            randomCount = 30;
            GarageMainScreen::GetInstance()->DisableCarRendering();
            cFEng::Get()->QueuePackageMessage(0xa05a328e, nullptr, nullptr);
            FEngSetScript(PackageFilename, 0xfe8fdbf7, 0x16a259, true);
            break;
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch("FeQuickRaceMainMenu.fng", 0, 0, false);
            break;
    }
}
