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

class RaceStarter {
public:
    static void StartRace();
};
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
    , prev_race_hash(0) //
    , pMovieName(0)
{
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
    switch (msg) {
    case 0xc98356ba:
        TrackMapStreamer.UpdateAnimation();
        break;
    case 0xc407210:
        if (!theChallengeRace) {
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
            return;
        }
        DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(1),
            0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), 0x77cf03c5);
        break;
    case 0xc519bfc3:
        if (static_cast<ChallengeDatum *>(currentDatum)->race->GetChallengeType() != 0) {
            return;
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
        FEAnyTutorialScreen_LaunchMovie(gTUTORIAL_MOVIE_TOLLBOOTH, GetPackageName());
        break;
    case 0x1a2826e1:
        FEDatabase->GetPlayerSettings(0)->Transmission = 0;
        goto start_race;
    case 0x5f5e3886:
        FEDatabase->GetPlayerSettings(0)->Transmission = 1;
        goto start_race;
    case 0xd05fc3a3: {
        signed char port = static_cast<signed char>(FEngMapJoyParamToJoyport(param1));
        FEDatabase->SetPlayersJoystickPort(0, port);
        if (FEDatabase->GetPlayerSettings(0)->TransmissionPromptOn != 0) {
            ChooseTransmission();
            return;
        }
    }
start_race:
        {
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theChallengeRace);
            GRaceDatabase::Get().SetStartupRace(race, kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
            RaceStarter::StartRace();
        }
        break;
    case 0x911ab364:
        cFEng::Get()->QueuePackageSwitch("FeQrPkg", 0, 0, false);
        break;
    case 0xc3960eb9:
        FEngSetScript(GetPackageName(), 0x99344537, 0x1744b3, true);
        break;
    }
}

void UIQRChallengeSeries::ChooseTransmission() {
    DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(3),
        0x317d3005, 0x8cd532a0, 0x5f5e3886, 0x1a2826e1, 0x34dc1bcf,
        (eDialogFirstButtons)(FEDatabase->GetPlayerSettings(0)->Transmission == 0), 0x6f5401d1);
}

void UIQRChallengeSeries::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (!currentDatum) return;

    int pos = data.TraversebList(currentDatum);
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", pos);
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", GetNumDatum());

    ChallengeDatum *cd = static_cast<ChallengeDatum *>(currentDatum);
    GRaceParameters *race = cd->race;
    if (!race || prev_race_hash == race->GetEventHash()) return;

    prev_race_hash = race->GetEventHash();
    FEPrintf(GetPackageName(), 0x13c45e, "%.0f", race->GetCashValue());

    const char *unit;
    float conv;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        unit = GetLocalizedString(0x8569a26a);
        conv = 0.001f;
    } else {
        unit = GetLocalizedString(0x867dcfd9);
        conv = 0.000621371f;
    }
    float length = race->GetRaceLengthMeters() * conv;
    int laps = race->GetNumLaps();
    FEPrintf(GetPackageName(), 0x80c9daa, "%$0.1f %s - %d laps", length, unit, laps);

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
    if (FEDatabase->IsMilestoneTimeFormat(race->GetChallengeType())) {
        goal *= (1.0f / 60.0f);
    }
    char buf[32];
    bSNPrintf(buf, 32, "%$0.0f", goal);

    const char *pkg = GetPackageName();
    cFrontendDatabase *db = FEDatabase;
    unsigned int tag = race->GetLocalizationTag();
    unsigned int descHash = db->GetChallengeDescHash(tag);
    const char *desc = GetLocalizedString(descHash);
    FEPrintf(pkg, 0x7b230d64, desc, buf, buf);

    if (static_cast<ChallengeDatum *>(currentDatum)->IsLocked()) {
        cFEng::Get()->QueuePackageMessage(0xc5dd9d68, GetPackageName(), nullptr);
        int lockedPos = data.TraversebList(currentDatum);
        int index = lockedPos - 1;
        int page = (lockedPos / 5) * 5;
        if (lockedPos < 61) {
            if (static_cast<unsigned int>(lockedPos - page - 1) <= 1) {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(0xced8931e), page);
            } else if (static_cast<unsigned int>(lockedPos - page - 3) <= 1) {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(0xced8931d), page + 1, page + 2);
            } else {
                FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(0xced8931d), page - 2, page - 1);
            }
        } else {
            FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(0xced8931e), index);
        }
    } else {
        cFEng::Get()->QueuePackageMessage(0x38091fa1, GetPackageName(), nullptr);
    }

    for (int i = 0; i < GetNumSlots(); i++) {
        ChallengeDatum *datum = static_cast<ChallengeDatum *>(GetDatumAt(i + GetStartDatumNum()));
        unsigned int check_hash = FEngHashString("CHECK_%d", i + 1);
        if (!datum) {
            FEngSetScript(GetPackageName(), check_hash, 0x16a259, true);
        } else if (datum->IsLocked()) {
            FEngSetScript(GetPackageName(), check_hash, 0x5079c8f8, true);
            FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x18ed48);
        } else if (datum->IsChecked()) {
            FEngSetScript(GetPackageName(), check_hash, 0x5079c8f8, true);
            FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x28feadd);
        } else {
            FEngSetScript(GetPackageName(), check_hash, 0x16a259, true);
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
    } else if (GetMikeMannBuild() == 2) {
        if (bStrICmp(parms->GetEventID(), "16_1_4_lap_ko") == 0) return true;
        return bStrICmp(parms->GetEventID(), "16_1_6_speedtrap") == 0;
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
