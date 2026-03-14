// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRChallengeSeries.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

extern GRaceParameters *theChallengeRace;
extern int GetMikeMannBuild();
extern int bStrCmp(const char *, const char *);
extern int bStrICmp(const char *, const char *);
extern unsigned int FEngHashString(const char *, ...);
extern void FEAnyTutorialScreen_LaunchMovie(const char *movie, const char *pkg);
extern const char *gTUTORIAL_MOVIE_TOLLBOOTH;

void ChallengeDatum::NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) {
    if (msg != 0x0C407210)
        return;
    if (!IsLocked()) {
        theChallengeRace = race;
    } else {
        theChallengeRace = nullptr;
    }
}

UIQRChallengeSeries::UIQRChallengeSeries(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 4, 3, true) //
    , MapHash(0) //
{
    tTimer.ResetLow();
    theChallengeRace = nullptr;
    int numSlots = GetWidth() * GetHeight();
    for (int i = 0; i < numSlots; i++) {
        unsigned int hash = FEngHashString("TRACK_IMAGE_%d", i + 1);
        FEImage *img = FEngFindImage(GetPackageName(), hash);
        if (img) {
            ImageArraySlot *slot = new ImageArraySlot(img);
            AddSlot(slot);
        }
    }
    TrackMap = (FEMultiImage *)FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP"));
    Setup();
}

UIQRChallengeSeries::~UIQRChallengeSeries() {
}

eMenuSoundTriggers UIQRChallengeSeries::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x480c9a58) {
        return static_cast<eMenuSoundTriggers>(5);
    }
    return maybe;
}

void UIQRChallengeSeries::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x5f5e3886) {
        FEDatabase->GetPlayerSettings(0)->Transmission = 1;
    } else if (msg == 0x1a2826e1) {
        FEDatabase->GetPlayerSettings(0)->Transmission = 0;
    } else if (msg == 0xc407210) {
        if (!theChallengeRace) {
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
            return;
        }
        DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(1),
            0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), 0x77cf03c5);
        return;
    } else if (msg == 0xc519bfc3) {
        if (theChallengeRace->GetChallengeType() != 0) {
            return;
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
        FEAnyTutorialScreen_LaunchMovie(gTUTORIAL_MOVIE_TOLLBOOTH, GetPackageName());
        return;
    } else if (msg == 0xc3960eb9) {
        FEngSetScript(GetPackageName(), 0x99344537, 0x1744b3, true);
        return;
    } else if (msg == 0x911ab364) {
        cFEng::Get()->QueuePackageSwitch("FeQrPkg", 0, 0, false);
        return;
    } else if (msg == 0xc98356ba) {
        TrackMapStreamer.UpdateAnimation();
        return;
    } else if (msg == 0xd05fc3a3) {
        signed char port = static_cast<signed char>(FEngMapJoyParamToJoyport(param2));
        FEDatabase->SetPlayersJoystickPort(0, port);
        if (FEDatabase->GetPlayerSettings(0)->Transmission != 0) {
            ChooseTransmission();
            return;
        }
    } else {
        return;
    }
    GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
    GRaceDatabase::Get().SetStartupRace(race, kRaceContext_QuickRace);
    GRaceDatabase::Get().FreeCustomRace(race);
    RaceStarter::StartRace();
}

void UIQRChallengeSeries::ChooseTransmission() {
    DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(3),
        0x317d3005, 0x8cd532a0, 0x5f5e3886, 0x1a2826e1, 0x34dc1bcf,
        (eDialogFirstButtons)(FEDatabase->GetPlayerSettings(0)->Transmission == 0), 0x6f5401d1);
}

void UIQRChallengeSeries::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    ArrayDatum *current = GetCurrentDatum();
    if (!current) return;

    int pos = GetCurrentDatumNum() + 1;
    int total = GetNumDatum();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", pos);
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", total);

    ChallengeDatum *cd = static_cast<ChallengeDatum *>(current);
    GRaceParameters *race = cd->race;
    if (!race) return;
    if (MapHash == race->GetEventHash()) return;

    MapHash = race->GetEventHash();
    int cashValue = static_cast<int>(race->GetCashValue());
    FEPrintf(GetPackageName(), 0x13c45e, "%d", cashValue);

    bool metric = FEDatabase->GetGameplaySettings()->SpeedoUnits == 1;
    const char *unit = GetLocalizedString(metric ? 0x8569a26a : 0x867dcfd9);
    float length = race->GetRaceLengthMeters();
    float conv = metric ? 0.001f : 0.000621371f;
    int laps = race->GetNumLaps();
    FEPrintf(GetPackageName(), 0x80c9daa, "%s x%d %.1f", unit, laps, length * conv);

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
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xa018de49), iconHash);

    float goal = static_cast<float>(race->GetChallengeGoal());
    if (FEDatabase->IsMilestoneTimeFormat(challengeType)) {
        goal *= 0.001f;
    }
    char buf[32];
    bSNPrintf(buf, 32, "%.2f", goal);

    unsigned int tag = race->GetLocalizationTag();
    unsigned int descHash = FEDatabase->GetChallengeDescHash(tag);
    const char *desc = GetLocalizedString(descHash);
    FEPrintf(GetPackageName(), 0x7b230d64, "%s%s", desc, buf);

    if (cd->IsLocked()) {
        cFEng::Get()->QueuePackageMessage(0x38091fa1, GetPackageName(), nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(0xc5dd9d68, GetPackageName(), nullptr);
        int index = pos - 1;
        int groupBase = (pos / 5) * 5;
        int mod = pos % 5;
        if (mod >= 1 && mod <= 2) {
            const char *locStr = GetLocalizedString(0xced8931e);
            FEPrintf(GetPackageName(), 0x68215623, "%s%d", locStr, index);
        } else if (mod >= 3 && mod <= 4) {
            const char *locStr = GetLocalizedString(0xced8931d);
            FEPrintf(GetPackageName(), 0x68215623, "%s%d-%d", locStr, groupBase + 1, groupBase + 2);
        } else {
            const char *locStr = GetLocalizedString(0xced8931d);
            int prevBase = ((pos / 5) - 1) * 5;
            FEPrintf(GetPackageName(), 0x68215623, "%s%d-%d", locStr, prevBase + 3, prevBase + 4);
        }
    }

    int numSlots = GetNumSlots();
    for (int i = 0; i < numSlots; i++) {
        ArrayDatum *datum = GetDatumAt(i + GetCurrentDatumNum());
        unsigned int slotHash = FEngHashString("TRACK_IMAGE_%d", i + 1);
        if (!datum) {
            FEngSetScript(GetPackageName(), slotHash, 0x16a259, true);
        } else if (datum->IsLocked()) {
            FEngSetScript(GetPackageName(), slotHash, 0x5079c8f8, true);
            FEngSetTextureHash(FEngFindImage(GetPackageName(), slotHash), 0x28feadd);
        } else if (datum->IsChecked()) {
            FEngSetScript(GetPackageName(), slotHash, 0x5079c8f8, true);
            FEngSetTextureHash(FEngFindImage(GetPackageName(), slotHash), 0x18ed48);
        }
    }
    TrackMapStreamer.Init(race, TrackMap, 0, 0);
}

void UIQRChallengeSeries::AddRace(GRaceParameters *race) {
    unsigned int iconHash;
    int challengeType = race->GetChallengeType();
    if (challengeType == 0) {
        iconHash = 0x65818ee8;
    } else {
        iconHash = FEDatabase->GetMilestoneIconHash(challengeType, true);
    }
    unsigned int tag = race->GetLocalizationTag();
    unsigned int headerHash = FEDatabase->GetChallengeHeaderHash(tag);
    ChallengeDatum *datum = new ChallengeDatum(iconHash, headerHash, race);
    AddDatum(datum);
}

bool UIQRChallengeSeries::IsRaceValidForMike(GRaceParameters *parms) {
    int build = GetMikeMannBuild();
    if (build == 1) {
        if (bStrCmp(parms->GetEventID(), "16_1_1_circuit") == 0) return true;
        if (bStrCmp(parms->GetEventID(), "16_1_2_sprint") == 0) return true;
        if (bStrCmp(parms->GetEventID(), "16_1_3_drag") == 0) return true;
        if (bStrCmp(parms->GetEventID(), "16_1_4_lap_ko") == 0) return true;
        return bStrCmp(parms->GetEventID(), "16_1_5_tollbooth") == 0;
    } else if (build == 2) {
        if (bStrICmp(parms->GetEventID(), "16_1_4_lap_ko") == 0) return true;
        return bStrICmp(parms->GetEventID(), "16_1_6_speedtrap") == 0;
    }
    return true;
}

void UIQRChallengeSeries::Setup() {
    ClearData();
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(0x13);
    unsigned int raceCount = bin->GetWorldRaceCount();
    for (unsigned int i = 0; i < raceCount; i++) {
        unsigned int raceHash = bin->GetWorldRaceHash(i);
        GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(raceHash);
        int mikeBuild = GetMikeMannBuild();
        if (mikeBuild == 0) {
            unsigned int eventHash = race->GetEventHash();
            if (UnlockSystem::IsEventAvailable(eventHash)) {
                bool unlocked = UnlockSystem::IsTrackUnlocked(UNLOCK_QUICK_RACE, eventHash, 0);
                AddRace(race);
                int count = GetNumDatum();
                ArrayDatum *lastDatum = GetDatumAt(count - 1);
                lastDatum->SetLocked(!unlocked);
                if (GRaceDatabase::Get().CheckRaceScoreFlags(raceHash, static_cast<GRaceDatabase::ScoreFlags>(1))) {
                    count = GetNumDatum();
                    lastDatum = GetDatumAt(count - 1);
                    lastDatum->SetChecked(true);
                }
            }
        } else {
            if (IsRaceValidForMike(race)) {
                AddRace(race);
            }
        }
    }
    SetInitialPosition(0);
    RefreshHeader();
}
