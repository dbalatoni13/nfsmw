#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

UIQRBrief::UIQRBrief(ScreenConstructorData *sd)
    : MenuScreen(sd),          //
      pSelectedCar(nullptr),   //
      pSelectedTrack(nullptr), //
      randomCount(0) {
    Setup();
    this->raceSettings.CopsOn = false;
    AccelerationSlider.Init(this->GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    TopSpeedSlider.Init(this->GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    HandlingSlider.Init(this->GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
}

void UIQRBrief::RefreshHeader() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car_record = stable->GetCarRecordByHandle(pSelectedCar->mHandle);

    const u32 FEObj_ManufacturerLogo = 0x3e01ad1d;
    const u32 FEObj_CarBadge = 0xb05dd708;

    uint32 logo_hash = car_record->GetManuLogoHash();
    if (GetTextureInfo(logo_hash, 0, 0) != nullptr) {
        FEngSetTextureHash(GetPackageName(), FEObj_ManufacturerLogo, logo_hash);
    } else {
        FEngSetTextureHash(GetPackageName(), FEObj_ManufacturerLogo, FEHashUpper("GENERICPLACEHOLDER"));
    }

    uint32 badge_hash = car_record->GetLogoHash();
    if (GetTextureInfo(badge_hash, 0, 0) != nullptr) {
        FEngSetTextureHash(this->GetPackageName(), FEObj_CarBadge, badge_hash);
    } else {
        FEngSetTextureHash(this->GetPackageName(), FEObj_CarBadge, FEHashUpper("GENERICPLACEHOLDER"));
    }

    // TODO
    const u32 FEObj_option01 = 0;
    const u32 FEObj_option02 = 0;
    const u32 FEObj_option03 = 0;
    const u32 FEObj_option04 = 0;
    const u32 FEObj_option05 = 0;
    const u32 FEObj_option06 = 0;
    const u32 FEObj_option07 = 0;
    const u32 FEObj_option08 = 0;

    GRaceParameters *track = pSelectedTrack->pRaceParams;
    FEngSetLanguageHash(this->GetPackageName(), 0xb5154998, FEDatabase->GetRaceNameHash(track->GetRaceType()));
    FEngSetTextureHash(this->GetPackageName(), 0x2521e5eb, FEDatabase->GetRaceIconHash(track->GetRaceType()));

    u32 hash = CalcLanguageHash("TRACKNAME_", track);
    if (DoesStringExist(hash)) {
        FEngSetLanguageHash(this->GetPackageName(), 0xb5154999, hash);
    } else {
        FEPrintf(this->GetPackageName(), 0xb5154999, track->GetEventID());
    }

    const char *distUnits = FEDatabase->GetGameplaySettings()->SpeedoUnits == 1 ? GetLocalizedString(0x8569a26a) : GetLocalizedString(0x867dcfd9);

    FEPrintf(this->GetPackageName(), 0xb515499a, "%$0.1f %s", track->GetRaceLengthMeters() * 0.001f, distUnits);

    switch (track->GetRaceType()) {
        case GRace::kRaceType_Circuit:
        case GRace::kRaceType_Knockout:
            FEPrintf(this->GetPackageName(), 0xb515499b, "%d", raceSettings.NumLaps);
            break;
        default:
            FEPrintf(this->GetPackageName(), 0xb515499b, "--");
            break;
    }

    FEPrintf(this->GetPackageName(), 0xb515499c, "%d", raceSettings.NumOpponents);
    switch (raceSettings.TrafficDensity) {
        case 1:
            hash = 0x73c615a3;
            break;
        case 2:
            hash = 0xa2cca838;
            break;
        case 3:
            hash = 0x61d1c5a5;
            break;
        case 0:
            hash = 0x8cdc3937;
            break;
        default:
            hash = 0;
            break;
    }

    if (track->GetRaceType() == GRace::kRaceType_P2P || track->GetRaceType() == GRace::kRaceType_Drag) {
        hash = 0x7f2f7ad6;
    }
    FEngSetLanguageHash(this->GetPackageName(), 0xb515499d, hash);
    switch (raceSettings.AISkill) {
        case 0:
            hash = 0x61973e01;
            break;
        case 1:
            hash = 0x3747f6d0;
            break;
        case 2:
            hash = 0x6198e2ee;
            break;
        default:
            hash = 0;
            break;
    }
    FEngSetLanguageHash(this->GetPackageName(), 0xb515499e, hash);

    hash = 0x70dfe5c2;
    if (raceSettings.CatchUp) {
        hash = 0x417b2604;
    }
    FEngSetLanguageHash(this->GetPackageName(), 0xb515499e, hash);
    UpdateSliders();
}

void UIQRBrief::UpdateSliders() {
    Physics::Info::Performance performance;
    Physics::Info::Performance current_performance;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car_record = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    Attrib::Gen::pvehicle pvehicle(car_record->VehicleKey, 0, nullptr);

    if (car_record->IsCustomized()) {
        stable->GetCustomizationRecordByHandle(car_record->Customization)->WriteRecordIntoPhysics(pvehicle);
    }
    Physics::Info::EstimatePerformance(pvehicle, performance);

    AccelerationSlider.SetValue(performance.Acceleration);
    AccelerationSlider.SetPreviewValue(performance.Acceleration);
    AccelerationSlider.Draw();

    TopSpeedSlider.SetValue(performance.TopSpeed);
    TopSpeedSlider.SetPreviewValue(performance.TopSpeed);
    TopSpeedSlider.Draw();

    HandlingSlider.SetValue(performance.Handling);
    HandlingSlider.SetPreviewValue(performance.Handling);
    HandlingSlider.Draw();
}

void UIQRBrief::Setup() {
    FilteredCarsList.DeleteAllElements();
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    int current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    for (int i = 0; i < 200; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_STOCK)) {
            Attrib::Gen::frontend fe_attrib(fe_car->FEKey, 0, nullptr);
            int unlocked_at = fe_attrib.UnlockedAt();
            if (unlocked_at >= current_bin - 1 && unlocked_at <= current_bin + 3 && fe_attrib.GetCollection() != 0xc1c32daf) {
                FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(fe_car->Handle, false));
            }
        }
    }
    for (unsigned int i = 0; i < GRaceDatabase::Get().GetRaceCount(); i++) {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceParameters(i);
        if (parms->GetRaceType() != GRace::kRaceType_Challenge && parms->GetRaceType() != GRace::kRaceType_JumpToSpeedTrap &&
            parms->GetRaceType() != GRace::kRaceType_JumpToMilestone && parms->GetRaceType() != GRace::kRaceType_None &&
            parms->GetRaceType() != GRace::kRaceType_Tollbooth) {
            if (parms->GetEventHash() != Attrib::StringHash32("19.8.31")) {
                if (UnlockSystem::IsEventAvailable(parms->GetEventHash())) {
                    if (UnlockSystem::IsTrackUnlocked(UNLOCK_QUICK_RACE, parms->GetEventHash(), 0)) {
                        FilteredTracksList.AddTail(new ("SelectableTrack", 0) SelectableTrack(parms, true, 0));
                    }
                }
            }
        }
    }
    cFEng::Get()->QueueGameMessage(__PAD_BUTTON5__, this->GetPackageName(), 0xff);
}

void UIQRBrief::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            if (randomCount < 1)
                return;
            pSelectedCar = FilteredCarsList.GetNextCircular(pSelectedCar);

            pSelectedTrack = FilteredTracksList.GetNextCircular(pSelectedTrack);

            FEDatabase->GetRandomRaceOptions(&raceSettings, pSelectedTrack->pRaceParams->GetRaceType());
            RefreshHeader();
            randomCount--;
            g_pEAXSound->PlayUISoundFX(UISND_QUICK_GAMBLE_BLIP);
            if (randomCount == 0) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                Attrib::Gen::frontend fe_attrib(stable->GetCarRecordByHandle(pSelectedCar->mHandle)->FEKey, 0, nullptr);
                int bl_num = fe_attrib.UnlockedAt();
                if (bl_num < FEDatabase->GetCareerSettings()->GetCurrentBin()) {
                    const u32 FEObj_unlockgroup = 0xfe8fdbf7;
                    FEngSetScript(this->GetPackageName(), FEObj_unlockgroup, FEHASH_APPEAR, true);
                    char rival_name_locdb[128];
                    FEngSNPrintf(rival_name_locdb, 128, "BLACKLIST_%d", bl_num + 1);
                    FEPrintf(this->GetPackageName(), FEObj_unlockgroup, GetLocalizedString(0x4ef2a115),
                             GetLocalizedString(FEHashUpper(rival_name_locdb)), bl_num + 1);
                }
                RideInfo ride;
                stable->BuildRideForPlayer(pSelectedCar->mHandle, 0, &ride);
                ride.SetRandomPaint();
                ride.SetRandomParts();
                CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
            }
            break;

        case __PAD_ACCEPT__:
            FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));
            break;

        case FEHASH_EXITCOMPLETE: {
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(0x12345678, 0);
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *record = stable->GetCarRecordByHandle(0x12345678);
            FECarRecord *from_car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
            *record = *from_car;
            record->FilterBits = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_DEBUG;
            FECustomizationRecord *customization = stable->GetCustomizationRecordByHandle(record->Customization);
            customization->WriteRideIntoRecord(CarViewer::GetRideInfo(eCARVIEWER_PLAYER1_CAR));
            Attrib::Gen::pvehicle car_physics(record->VehicleKey, 0, nullptr);
            int max_level = Physics::Upgrades::GetMaxLevel(car_physics, Physics::Upgrades::PUT_NOS);
            Physics::Upgrades::SetLevel(car_physics, Physics::Upgrades::PUT_NOS, max_level);
            customization->WritePhysicsIntoRecord(car_physics);
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(pSelectedTrack->pRaceParams);
            FEDatabase->FillCustomRace(race, &raceSettings);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
            break;
        }
        case __PAD_BUTTON5__: {
            pSelectedCar = GetRandomCar();
            pSelectedTrack = GetRandomTrack();
            randomCount = 30;
            const u32 FEObj_LOADERAPPEAR = 0xa05a328e;
            const u32 FEObj_unlockgroup = 0xfe8fdbf7;
            GarageMainScreen::GetInstance()->DisableCarRendering();
            cFEng::Get()->QueuePackageMessage(FEObj_LOADERAPPEAR, nullptr, nullptr);
            FEngSetScript(this->GetPackageName(), FEObj_unlockgroup, FEHASH_HIDE, true);
            break;
        }
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch("FeQuickRaceMainMenu.fng", 0, 0, false);
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
