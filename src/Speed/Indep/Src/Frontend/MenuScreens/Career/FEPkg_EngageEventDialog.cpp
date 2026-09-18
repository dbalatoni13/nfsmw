#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEPkg_EngageEventDialog.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"
#include "Speed/Indep/Src/Generated/Messages/MAcceptEnterCareerEvent.h"
#include "Speed/Indep/Src/Generated/Messages/MDeclineEnterCareerEvent.h"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

namespace nsEngageEventDialog {

EngageEventDialog::EngageEventDialog(ScreenConstructorData *sd)
    : MenuScreen(sd), mpRaceActivity(reinterpret_cast<GRuntimeInstance *>(sd->Arg)), MapStreamer(nullptr) {
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(mpRaceActivity));

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

    FEPrintf(GetPackageName(), 0xbce13923, "%$0.1f %s",
             parms->GetRaceLengthMeters() * (kph ? 0.001f : 0.0006213728f), distUnits);

    unsigned int hash;
    if (parms->GetCopsEnabled()) {
        hash = LANGUAGE_COMMON_MAXIMUM;
    } else {
        hash = LANGUAGE_COMMON_MINIMUM;
    }
    FEngSetLanguageHash(GetPackageName(), 0x762f1d7a, hash);

    GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(parms->GetEventHash());

    if (parms->GetRaceType() == GRace::kRaceType_P2P || parms->GetRaceType() == GRace::kRaceType_Circuit ||
        parms->GetRaceType() == GRace::kRaceType_Drag || parms->GetRaceType() == GRace::kRaceType_Knockout ||
        parms->GetRaceType() == GRace::kRaceType_Tollbooth) {
        Timer timer(info->mHighScores.mBestTime);
        char buf[64];
        timer.PrintToString(buf, 0);
        FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", buf);
    } else {
        FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", GetLocalizedString(0x472aa00a));
    }

    const char *speedUnits;
    float avg_speed;
    float top_speed;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = GetLocalizedString(0x8569a25f);
        avg_speed = MPS2KPH(info->mAverageSpeed);
        top_speed = MPS2KPH(info->mTopSpeed);
    } else {
        speedUnits = GetLocalizedString(0x8569ab44);
        avg_speed = MPS2MPH(info->mAverageSpeed);
        top_speed = MPS2MPH(info->mTopSpeed);
    }
    FEPrintf(GetPackageName(), 0x35d1ab83, "%$0.0f %s", avg_speed, speedUnits);
    FEPrintf(GetPackageName(), 0xde9145fb, "%$0.0f %s", top_speed, speedUnits);

    FEPrintf(GetPackageName(), 0x45276f1f, "%$0.0f", parms->GetCashValue());

    TrackMap = reinterpret_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    MapStreamer = new ("", 0) UITrackMapStreamer();
    MapStreamer->Init(parms, TrackMap, 0, 0);
}

EngageEventDialog::~EngageEventDialog() {
    delete MapStreamer;
    MapStreamer = nullptr;
}

void EngageEventDialog::NotifyTheGameAcceptEvent() {
    MAcceptEnterCareerEvent().Post(UCrc32(0x20d60dbf));
}

void EngageEventDialog::NotifyTheGameDeclineEvent() {
    MDeclineEnterCareerEvent().Post(UCrc32(0x20d60dbf));
}

// STRIPPED
void EngageEventDialog::NotifyTheGameButton3() {}

void EngageEventDialog::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case 0xc98356ba:
            MapStreamer->UpdateAnimation();
            break;
        case 0x911ab364:
            NotifyTheGameDeclineEvent();
            cFEng::Get()->QueuePackagePop(0);
            break;
        case 0x0c407210: {
            unsigned int objHash = obj->NameHash;
            if (objHash != 0x694b896e) {
                if (objHash == 0xd72f002a) {
                    NotifyTheGameAcceptEvent();
                    cFEng::Get()->QueuePackagePop(0);
                }
            } else {
                NotifyTheGameDeclineEvent();
                cFEng::Get()->QueuePackagePop(0);
            }
            break;
        }
    }
}

} // namespace nsEngageEventDialog
