#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEPkg_EngageEventDialog.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"
#include "Speed/Indep/Src/Generated/Messages/MAcceptEnterCareerEvent.h"
#include "Speed/Indep/Src/Generated/Messages/MDeclineEnterCareerEvent.h"

namespace nsEngageEventDialog {

EngageEventDialog::EngageEventDialog(ScreenConstructorData *sd) : MenuScreen(sd) {
    MapStreamer = nullptr;

    GActivity *activity = reinterpret_cast<GActivity *>(sd->Arg);
    mpRaceActivity = reinterpret_cast<GRuntimeInstance *>(activity);
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(activity);

    FEngSetTextureHash(GetPackageName(), 0xad9e232a, FEDatabase->GetRaceIconHash(parms->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), 0xa01b9361, FEDatabase->GetRaceNameHash(parms->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), 0xf601f2d4, CalcLanguageHash("TRACKNAME_", parms));

    FEPrintf(GetPackageName(), 0x644ab208, "%d", parms->GetNumLaps());

    bool kph;
    const char *distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        kph = true;
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        kph = false;
        distUnits = GetLocalizedString(0x867dcfd9);
    }

    float length = parms->GetRaceLengthMeters();
    if (kph) {
        length *= 0.001f;
    } else {
        length *= 0.000625f;
    }
    FEPrintf(GetPackageName(), 0xbce13923, "%$0.1f %s", length, distUnits);

    unsigned int hash;
    if (parms->GetCopsEnabled()) {
        hash = 0x61d1c5a5;
    } else {
        hash = 0x73c615a3;
    }
    FEngSetLanguageHash(GetPackageName(), 0x762f1d7a, hash);

    GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(parms->GetEventHash());

    if (parms->GetRaceType() == GRace::kRaceType_P2P || parms->GetRaceType() == GRace::kRaceType_Circuit ||
        parms->GetRaceType() == GRace::kRaceType_Drag || parms->GetRaceType() == GRace::kRaceType_Knockout ||
        parms->GetRaceType() == GRace::kRaceType_Tollbooth) {
        Timer timer;
        timer.SetTime(info->mHighScores.mBestTime);
        char buf[64];
        timer.PrintToString(buf, 0);
        FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", buf);
    } else {
        FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", GetLocalizedString(0x472aa00a));
    }

    const char *speedUnits;
    float conversion;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        conversion = 3.6f;
        speedUnits = GetLocalizedString(0x8569a25f);
    } else {
        conversion = 2.23699f;
        speedUnits = GetLocalizedString(0x8569ab44);
    }

    float avg_speed = static_cast<float>(info->mAverageSpeed) * conversion;
    float top_speed = static_cast<float>(info->mTopSpeed) * conversion;
    FEPrintf(GetPackageName(), 0x35d1ab83, "%$0.0f %s", avg_speed, speedUnits);
    FEPrintf(GetPackageName(), 0xde9145fb, "%$0.0f %s", top_speed, speedUnits);

    FEPrintf(GetPackageName(), 0x45276f1f, "%$0.0f", parms->GetCashValue());

    TrackMap = reinterpret_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    MapStreamer = new ("", 0) UITrackMapStreamer();
    MapStreamer->Init(parms, TrackMap, 0, 0);
}

EngageEventDialog::~EngageEventDialog() {
    if (MapStreamer) {
        delete MapStreamer;
        MapStreamer = nullptr;
    }
}

void EngageEventDialog::NotifyTheGameAcceptEvent() {
    UCrc32 port(0x20d60dbf);
    MAcceptEnterCareerEvent msg;
    msg.Post(port);
}

void EngageEventDialog::NotifyTheGameDeclineEvent() {
    UCrc32 port(0x20d60dbf);
    MDeclineEnterCareerEvent msg;
    msg.Post(port);
}

// STRIPPED
void EngageEventDialog::NotifyTheGameButton3() {}

void EngageEventDialog::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x911ab364:
            NotifyTheGameDeclineEvent();
            cFEng::Get()->QueuePackagePop(1);
            break;
        case 0xc98356ba:
            if (MapStreamer) {
                MapStreamer->UpdateAnimation();
            }
            break;
        case 0x0c407210: {
            unsigned int objHash = obj->NameHash;
            if (objHash == 0x694b896e) {
                NotifyTheGameDeclineEvent();
                cFEng::Get()->QueuePackagePop(1);
            } else if (objHash == 0xd72f002a) {
                NotifyTheGameAcceptEvent();
                cFEng::Get()->QueuePackagePop(1);
            }
            break;
        }
    }
}

} // namespace nsEngageEventDialog
