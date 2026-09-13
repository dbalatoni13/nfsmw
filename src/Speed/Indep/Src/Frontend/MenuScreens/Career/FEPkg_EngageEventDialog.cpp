#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEPkg_EngageEventDialog.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Generated/Messages/MAcceptEnterCareerEvent.h"
#include "Speed/Indep/Src/Generated/Messages/MDeclineEnterCareerEvent.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

namespace nsEngageEventDialog {

EngageEventDialog::EngageEventDialog(ScreenConstructorData *sd)
    : MenuScreen(sd), MapStreamer(nullptr), mpRaceActivity(reinterpret_cast<GActivity *>(sd->Arg)) {
    const u32 FEObj_BodyText = 0;
    GActivity *activity = reinterpret_cast<GActivity *>(mpRaceActivity);
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(activity);

    const u32 FEObj_EVENTNAME = 0xa01b9361;
    const u32 FEObj_TRACKNAME = 0xf601f2d4;

    FEngSetTextureHash(GetPackageName(), 0xad9e232a, FEDatabase->GetRaceIconHash(parms->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), FEObj_EVENTNAME, FEDatabase->GetRaceNameHash(parms->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), FEObj_TRACKNAME, CalcLanguageHash("TRACKNAME_", parms));

    FEPrintf(GetPackageName(), 0x644ab208, "%d", parms->GetNumLaps());

    const char *distUnits;
    bool kph;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        kph = true;
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        kph = false;
        distUnits = GetLocalizedString(0x867dcfd9);
    }

    FEPrintf(GetPackageName(), 0xbce13923, "%$0.1f %s", parms->GetRaceLengthMeters() * (kph ? 0.001f : 0.000625f), distUnits);

    uint32 hash;
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
        Timer t(info->mHighScores.mBestTime);
        char buf[64];
        t.PrintToString(buf, 0);
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
    MapStreamer = new ("MapStreamer", 0) UITrackMapStreamer();
    MapStreamer->Init(parms, TrackMap, 0, 0);
}

EngageEventDialog::~EngageEventDialog() {
    if (MapStreamer != nullptr) {
        delete MapStreamer;
    }
    MapStreamer = nullptr;
}

void EngageEventDialog::NotifyTheGameAcceptEvent() {
    MAcceptEnterCareerEvent().Post(0x20d60dbf);
}

void EngageEventDialog::NotifyTheGameDeclineEvent() {
    MDeclineEnterCareerEvent().Post(0x20d60dbf);
}

// STRIPPED
void EngageEventDialog::NotifyTheGameButton3() {}

void EngageEventDialog::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            MapStreamer->UpdateAnimation();
            break;
        case __PAD_BACK__:
            NotifyTheGameDeclineEvent();
            cFEng::Get()->QueuePackagePop(1);
            break;
        case __BUTTON_PRESSED__:
            switch (obj->NameHash) {
                case 0xd72f002a:
                    NotifyTheGameAcceptEvent();
                    cFEng::Get()->QueuePackagePop(1);
                    break;
                case 0x694b896e:
                    NotifyTheGameDeclineEvent();
                    cFEng::Get()->QueuePackagePop(1);
                    break;
            }
            break;
    }
}

} // namespace nsEngageEventDialog
