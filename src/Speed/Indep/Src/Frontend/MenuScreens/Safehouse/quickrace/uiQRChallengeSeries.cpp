#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRChallengeSeries.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeArrayScroller.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

GRaceParameters *theChallengeRace = nullptr;
const char *gTUTORIAL_MOVIE_TOLLBOOTH = "tollbooth_tutorial";
void ChallengeDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg != 0x0C407210)
        return;
    if (!IsLocked()) {
        theChallengeRace = race;
    } else {
        theChallengeRace = nullptr;
    }
}

UIQRChallengeSeries::UIQRChallengeSeries(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 4, 3, true), //
      prev_race_hash(0),                 //
      pMovieName(0) {
    theChallengeRace = nullptr;
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img) {
            ImageArraySlot *slot = new ("ImageArraySlot", 0) ImageArraySlot(img);
            AddSlot(slot);
        }
    }
    TrackMap = (FEMultiImage *)FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP"));
    Setup();
}

UIQRChallengeSeries::~UIQRChallengeSeries() {}

eMenuSoundTriggers UIQRChallengeSeries::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x480c9a58) {
        return static_cast<eMenuSoundTriggers>(5);
    }
    return maybe;
}

void UIQRChallengeSeries::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case 0xc98356ba:
            TrackMapStreamer.UpdateAnimation();
            break;
        case 0xc407210:
            if (!theChallengeRace) {
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
                return;
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
                                            first_dialog_button2, LANGUAGE_REP_SHEET_RACE_CONFIRM);
            break;
        case 0xc519bfc3:
            if (static_cast<ChallengeDatum *>(GetCurrentDatum())->race->GetChallengeType() != 0) {
                return;
            }
            FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
            FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_TOLLBOOTH, GetPackageName());
            break;
        case 0x1a2826e1: {
            FEDatabase->GetPlayerSettings(0)->Transmission = 0;
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
        } break;
        case 0x5f5e3886: {
            FEDatabase->GetPlayerSettings(0)->Transmission = 1;
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
        } break;
        case 0xd05fc3a3: {
            signed char port = static_cast<signed char>(FEngMapJoyParamToJoyport(param1));
            FEDatabase->SetPlayersJoystickPort(0, port);
            if (FEDatabase->GetPlayerSettings(0)->TransmissionPromptOn != 0) {
                ChooseTransmission();
                return;
            }
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
        } break;
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            break;
        case 0xc3960eb9:
            FEngSetScript(GetPackageName(), 0x99344537, 0x1744b3, true);
            break;
        case 0x34dc1bcf:
            break;
        case 0x38091fa1:
            break;
    }
}

void UIQRChallengeSeries::ChooseTransmission() {
    DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, LANGUAGE_COMMON_MANUAL, LANGUAGE_COMMON_AUTO, 0x5f5e3886, 0x1a2826e1, 0x34dc1bcf,
                                    (eDialogFirstButtons)(FEDatabase->GetPlayerSettings(0)->Transmission == 0), LANGUAGE_COMMON_TRANSMISSION);
}

void UIQRChallengeSeries::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (!GetCurrentDatum())
        return;

    FEPrintf(GetPackageName(), __NUMBER_GROUP__, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), __NUMBER_OF_GROUP__, "%d", GetNumDatum());

    GRaceParameters *race = static_cast<ChallengeDatum *>(GetCurrentDatum())->race;
    if (race && prev_race_hash == race->GetEventHash())
        return;

    prev_race_hash = race->GetEventHash();
    FEPrintf(GetPackageName(), 0x13c45e, "%.0f", race->GetCashValue());

    const char *distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        distUnits = GetLocalizedString(0x867dcfd9);
    }
    FEPrintf(GetPackageName(), 0x80c9daa, "%$0.1f %s - %d laps", race->GetRaceLengthMeters() * 0.001f, distUnits,
             race->GetNumLaps());

    FEngSetInvisible(FEngFindObject(GetPackageName(), 0xbbf970cd));
    int challengeType = race->GetChallengeType();
    unsigned int iconHash;
    if (challengeType == 0) {
        iconHash = 0x65818ee8;
        cFEng::Get()->QueuePackageMessage(0xb295482e, GetPackageName(), nullptr);
    } else {
        iconHash = FEDatabase->GetMilestoneIconHash(challengeType, true);
        cFEng::Get()->QueuePackageMessage(0xf7b54c7, GetPackageName(), nullptr);
    }
    FEngSetTextureHash(FEngFindImage(GetPackageName(), __EVENT_ICON__), iconHash);

    float goal = static_cast<float>(race->GetChallengeGoal());
    if (FEDatabase->IsMilestoneTimeFormat(race->GetChallengeType())) {
        goal *= (1.0f / 60.0f);
    }
    char buf[32];
    bSNPrintf(buf, 32, "%$0.0f", goal);

    FEPrintf(GetPackageName(), __EVENT_DESCRIPTION__, GetLocalizedString(FEDatabase->GetChallengeDescHash(race->GetLocalizationTag())), buf, buf);

    if (static_cast<ChallengeDatum *>(GetCurrentDatum())->IsLocked()) {
        cFEng::Get()->QueuePackageMessage(0xc5dd9d68, GetPackageName(), nullptr);
        int index = GetCurrentDatumNum();
        int page = (index / 5) * 5;
        int mod = index - page;
        int prevIndex = index - 1;
        if (index < 61) {
            if (static_cast<unsigned int>(mod - 1) <= 1) {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_02), page);
            } else if (static_cast<unsigned int>(mod - 3) <= 1) {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_01), page + 1, page + 2);
            } else {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_01), (index / 5 - 1) * 5 + 3, (index / 5 - 1) * 5 + 4);
            }
        } else {
            FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_02), prevIndex);
        }
    } else {
        cFEng::Get()->QueuePackageMessage(0x38091fa1, GetPackageName(), nullptr);
    }

    for (int i = 0; i < GetNumSlots(); i++) {
        ChallengeDatum *datum = static_cast<ChallengeDatum *>(GetDatumAt(i + GetStartDatumNum()));
        unsigned int check_hash = FEngHashString("CHECK_%d", i + 1);
        if (datum) {
            if (datum->IsLocked()) {
                FEngSetScript(GetPackageName(), check_hash, FEHASH_APPEAR, true);
                FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x18ed48);
            } else if (datum->IsChecked()) {
                FEngSetScript(GetPackageName(), check_hash, FEHASH_APPEAR, true);
                FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x28feadd);
            } else {
                FEngSetScript(GetPackageName(), check_hash, 0x16a259, true);
            }
        } else {
            FEngSetScript(GetPackageName(), check_hash, 0x16a259, true);
        }
    }
    TrackMapStreamer.Init(race, TrackMap, 0, 0);
}

void UIQRChallengeSeries::AddRace(GRaceParameters *race) {
    u32 type = race->GetChallengeType();
    u32 hash;
    if (type == 0) {
        hash = 0x65818ee8;
    } else {
        hash = FEDatabase->GetMilestoneIconHash(type, true);
    }
    AddDatum(new ("ChallengeDatum", 0) ChallengeDatum(hash, FEDatabase->GetChallengeHeaderHash(race->GetLocalizationTag()), race));
}

bool UIQRChallengeSeries::IsRaceValidForMike(GRaceParameters *parms) {
    int build = GetMikeMannBuild();
    if (build == 1) {
        if (bStrCmp(parms->GetEventID(), "19.8.1") == 0)
            return true;
        if (bStrCmp(parms->GetEventID(), "19.8.4") == 0)
            return true;
        if (bStrCmp(parms->GetEventID(), "19.8.10") == 0)
            return true;
        if (bStrCmp(parms->GetEventID(), "19.8.15") == 0)
            return true;
        return bStrCmp(parms->GetEventID(), "19.8.29") == 0;
    } else if (GetMikeMannBuild() == 2) {
        if (bStrICmp(parms->GetEventID(), "19.8.15") == 0)
            return true;
        return bStrICmp(parms->GetEventID(), "19.8.45") == 0;
    }
    return true;
}

void UIQRChallengeSeries::Setup() {
    ClearData();
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(0x13);
    for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
        unsigned int raceHash = bin->GetWorldRaceHash(i);
        GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(raceHash);
        if (GetMikeMannBuild() != 0) {
            if (IsRaceValidForMike(race)) {
                AddRace(race);
            }
        } else {
            if (UnlockSystem::IsEventAvailable(race->GetEventHash())) {
                bool unlocked = UnlockSystem::IsTrackUnlocked(UNLOCK_QUICK_RACE, race->GetEventHash(), 0);
                AddRace(race);
                GetDatumAt(GetNumDatum() - 1)->SetLocked(!unlocked);
                if (GRaceDatabase::Get().IsQuickRaceComplete(raceHash)) {
                    GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
                }
            }
        }
    }
    SetDescLabel(0x790ce49);
    SetInitialPosition(0);
    RefreshHeader();
}
