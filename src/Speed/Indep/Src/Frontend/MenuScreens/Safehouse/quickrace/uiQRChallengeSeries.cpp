#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRChallengeSeries.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeArrayScroller.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

GRaceParameters *theChallengeRace = nullptr;

void ChallengeDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == __BUTTON_PRESSED__) {
        if (!IsLocked()) {
            theChallengeRace = race;
        } else {
            theChallengeRace = nullptr;
        }
    }
}

UIQRChallengeSeries::UIQRChallengeSeries(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 4, 3, true), //
      prev_race_hash(0),                 //
      pMovieName(nullptr) {
    theChallengeRace = nullptr;
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img != nullptr) {
            AddSlot(new ("ImageArraySlot", 0) ImageArraySlot(img));
        }
    }
    TrackMap = (FEMultiImage *)FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP"));
    Setup();
}

UIQRChallengeSeries::~UIQRChallengeSeries() {}

eMenuSoundTriggers UIQRChallengeSeries::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == FEHASH_SOUND_BACK) {
        return UISND_COMMON_BACK;
    }
    return maybe;
}

// UNSOLVED
void UIQRChallengeSeries::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            TrackMapStreamer.UpdateAnimation();
            break;
        case __BUTTON_PRESSED__:
            if (theChallengeRace == nullptr) {
                extern EAXSound *g_pEAXSound;
                g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
                return;
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                            dialog_message_no, dialog_message_no, first_dialog_button2, LANGUAGE_REP_SHEET_RACE_CONFIRM);
            break;
        case __PAD_BUTTON4__: {
            GRaceParameters *race = static_cast<ChallengeDatum *>(GetCurrentDatum())->race;
            if (race->GetChallengeType() == 0) {
                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = FEHASH_HIDE;
                extern const char *gTUTORIAL_MOVIE_TOLLBOOTH;

                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_HIDE, true);
                FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_TOLLBOOTH, GetPackageName());
            }
            break;
        }
        case 0x1a2826e1: {
            FEDatabase->GetPlayerSettings(0)->Transmission = 0;
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
            break;
        }
        case 0x5f5e3886: {
            FEDatabase->GetPlayerSettings(0)->Transmission = 1;
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
            break;
        }
        case dialog_message_yes: {
            int joyPort = FEngMapJoyParamToJoyport(param1);
            FEDatabase->SetPlayersJoystickPort(0, joyPort);
            if (FEDatabase->GetPlayerSettings(0)->TransmissionPromptOn) {
                ChooseTransmission();
            } else {
                GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
                GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
                GRaceDatabase::Get().FreeCustomRace(race);
                RaceStarter::StartRace();
            }
            break;
        }

        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch("FeQrPkg", 0, 0, false);
            break;
        case 0xc3960eb9: {
            const u32 FEObj_MASTERBLASTER = 0x99344537;
            const u32 FEObj_Init = FEHASH_INIT;

            FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_Init, true);
            break;
        }
    }
}

void UIQRChallengeSeries::ChooseTransmission() {
    eDialogFirstButtons first_button = FEDatabase->GetPlayerSettings(0)->Transmission ? first_dialog_button1 : first_dialog_button2;
    DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, 0x317d3005, 0x8cd532a0, 0x5f5e3886, 0x1a2826e1, dialog_message_no,
                                    first_button, 0x6f5401d1);
}

// UNSOLVED
void UIQRChallengeSeries::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (GetCurrentDatum() == nullptr)
        return;

    FEPrintf(GetPackageName(), __NUMBER_GROUP__, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), __NUMBER_OF_GROUP__, "%d", GetNumDatum());

    GRaceParameters *race = static_cast<ChallengeDatum *>(GetCurrentDatum())->race;
    if ((race != nullptr) && prev_race_hash == race->GetEventHash()) {
        return;
    }

    prev_race_hash = race->GetEventHash();
    FEPrintf(GetPackageName(), 0x13c45e, "%.0f", race->GetCashValue());

    const char *distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        distUnits = GetLocalizedString(0x867dcfd9);
    }

    FEPrintf(GetPackageName(), 0x80c9daa, "%$0.1f %s - %d laps", race->GetRaceLengthMeters() * 0.001f, distUnits, race->GetNumLaps());

    FEngSetInvisible(GetPackageName(), 0xbbf970cd);

    u32 type = race->GetChallengeType();
    u32 hash;
    if (type == 0) {
        hash = 0x65818ee8;
        cFEng::Get()->QueuePackageMessage(0xb295482e, GetPackageName(), nullptr);
    } else {
        hash = FEDatabase->GetMilestoneIconHash(hash, true);
        cFEng::Get()->QueuePackageMessage(0xf7b54c7, GetPackageName(), nullptr);
    }
    FEngSetTextureHash(GetPackageName(), __EVENT_ICON__, hash);

    float goal = race->GetChallengeGoal();
    if (FEDatabase->IsMilestoneTimeFormat(race->GetChallengeType())) {
        goal /= 60.0f;
    }
    char buf[32];
    bSNPrintf(buf, sizeof(buf), "%$0.0f", goal);
    FEPrintf(GetPackageName(), __EVENT_DESCRIPTION__, GetLocalizedString(FEDatabase->GetChallengeDescHash(race->GetLocalizationTag())), buf, buf);

    if (static_cast<ChallengeDatum *>(GetCurrentDatum())->IsLocked()) {
        cFEng::Get()->QueuePackageMessage(0xc5dd9d68, GetPackageName(), nullptr);
        int index = GetCurrentDatumNum() - 1;
        int mod = (index + 1) % 5;
        if (index < 61) {
            int page = (index + 1) / 5;
            if (mod == 1 || mod == 2) {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_02), page * 5);
            } else if (mod == 3 || mod == 4) {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_01), page * 5 + 1, page * 5 + 2);
            } else {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_01), (page - 1) * 5 + 3,
                         (page - 1) * 5 + 4);
            }
        } else {
            FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(LANGUAGE_CHALLENGE_SERIES_UNLOCK_MSG_02), index);
        }
    } else {
        cFEng::Get()->QueuePackageMessage(0x38091fa1, GetPackageName(), nullptr);
    }

    for (int i = 0; i < GetNumSlots(); i++) {
        ChallengeDatum *datum = static_cast<ChallengeDatum *>(GetDatumAt(i + GetStartDatumNum()));
        uint32 check_hash = FEngHashString("CHECK_%d", i + 1);
        if (datum != nullptr) {
            if (datum->IsLocked()) {
                FEngSetScript(GetPackageName(), check_hash, FEHASH_APPEAR, true);
                FEngSetTextureHash(GetPackageName(), check_hash, STRINGHASH_LOCK);
            } else if (datum->IsChecked()) {
                FEngSetScript(GetPackageName(), check_hash, FEHASH_APPEAR, true);
                FEngSetTextureHash(GetPackageName(), check_hash, STRINGHASH_CHECK);
            } else {
                FEngSetScript(GetPackageName(), check_hash, FEHASH_HIDE, true);
            }
        } else {
            FEngSetScript(GetPackageName(), check_hash, FEHASH_HIDE, true);
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
    if (GetMikeMannBuild() == 1) {
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
                GetDatumAt(GetNumDatum())->SetLocked(!unlocked);
                if (GRaceDatabase::Get().IsQuickRaceComplete(raceHash)) {
                    GetDatumAt(GetNumDatum())->SetChecked(true);
                }
            }
        }
    }
    SetDescLabel(0x790ce49);
    SetInitialPosition(0);
    RefreshHeader();
}
