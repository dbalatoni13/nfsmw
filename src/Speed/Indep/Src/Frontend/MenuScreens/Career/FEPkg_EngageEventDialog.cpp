#include "Speed/Indep/Src/Misc/FixedPoint.hpp"

#ifndef GRACESAVEINFO_DEFINED
#define GRACESAVEINFO_DEFINED
struct GRaceSaveInfo {
    unsigned int mRaceHash;                              // offset 0x0, size 0x4
    unsigned int mFlags;                                 // offset 0x4, size 0x4
    float mHighScores;                                   // offset 0x8, size 0x4
    unsigned short mTopSpeed;                            // offset 0xC, size 0x2
    unsigned short mAverageSpeed;                        // offset 0xE, size 0x2
};
#endif

struct UITrackMapStreamer {
    char _data[0xD8];
    virtual ~UITrackMapStreamer();
    UITrackMapStreamer();
    void Init(GRaceParameters *track, FEMultiImage *map, int unused, int region_unlock);
    void UpdateAnimation();
};

unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *parms);

namespace nsEngageEventDialog {

struct EngageEventDialog : MenuScreen {
    GRuntimeInstance *mpRaceActivity;  // offset 0x2C
    UITrackMapStreamer *MapStreamer;    // offset 0x30
    FEMultiImage *TrackMap;            // offset 0x34

    EngageEventDialog(ScreenConstructorData *sd);
    ~EngageEventDialog() override;
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
    void NotifyTheGameAcceptEvent();
    void NotifyTheGameDeclineEvent();
};

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

    if (parms->GetRaceType() == GRace::kRaceType_P2P ||
        parms->GetRaceType() == GRace::kRaceType_Circuit ||
        parms->GetRaceType() == GRace::kRaceType_Drag ||
        parms->GetRaceType() == GRace::kRaceType_Knockout ||
        parms->GetRaceType() == GRace::kRaceType_Tollbooth) {
        Timer timer;
        timer.SetTime(info->mHighScores);
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

    float avg_speed = static_cast<float>(info->mAverageSpeed) / static_cast<float>(FixedPoint<unsigned short, 10, 2>::GetScale()) * conversion;
    float top_speed = static_cast<float>(info->mTopSpeed) / static_cast<float>(FixedPoint<unsigned short, 10, 2>::GetScale()) * conversion;
    FEPrintf(GetPackageName(), 0x35d1ab83, "%$0.0f %s", avg_speed, speedUnits);
    FEPrintf(GetPackageName(), 0xde9145fb, "%$0.0f %s", top_speed, speedUnits);

    FEPrintf(GetPackageName(), 0x45276f1f, "%$0.0f", parms->GetCashValue());

    TrackMap = static_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    MapStreamer = new ("", 0) UITrackMapStreamer();
    MapStreamer->Init(parms, TrackMap, 0, 0);
}

} // namespace nsEngageEventDialog
