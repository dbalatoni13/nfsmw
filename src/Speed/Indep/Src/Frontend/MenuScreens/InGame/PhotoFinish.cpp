#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Generated/Events/ECameraPhotoFinish.hpp"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

extern Timer RealTimer;

extern unsigned int bStringHash(const char *str);
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void StartCinematicSlowdown(EVIEW_ID view_id, float amount);

extern const char lbl_803E43DC[];
extern const char lbl_803E6080[];
extern const char lbl_803E60A4[];
extern const char lbl_803E60B8[];
extern const char lbl_803E60C8[];
extern const float lbl_803E60C4;

bool PhotoFinishScreen::mRestartSelected = false;
float PhotoFinishScreen::mSpeedtrapSpeed = 0.0f;
float PhotoFinishScreen::mSpeedtrapBounty = 0.0f;
bool PhotoFinishScreen::mActive = false;

PhotoFinishScreen::PhotoFinishScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , mIceCamTimer() //
    , mSlowdownTimer() //
    , fResultType(FERESULTTYPE_RACE) //
    , mPhotoHash(0) //
    , StreamTex(lbl_803E6080) {
    if (sd->Arg == 0 && GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr) {
        const char *photo_texture = lbl_803E60A4;

        if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career ||
            !GRaceStatus::Get().GetRaceParameters()->GetIsBossRace()) {
            photo_texture = GRaceStatus::Get().GetRaceParameters()->GetPhotoFinishTexture();
        }

        mPhotoHash = bStringHash(photo_texture);
        StreamTex.Load(mPhotoHash, FEngFindImage(GetPackageName(), 0x286A9CD4));
    }

    mSlowdownTimer = RealTimer;

    StartCinematicSlowdown(static_cast< EVIEW_ID >(1), lbl_803E60C4);
    SetSoundControlState(true, static_cast< eSNDCTLSTATE >(0xF), lbl_803E60B8);
    new ESndGameState(7, true);
    new ECameraPhotoFinish();
    new EMomentStrm(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, nullptr, 0x9FE1EE17);

    mRestartSelected = false;
    mActive = true;
}

PhotoFinishScreen::~PhotoFinishScreen() {
    StreamTex.UnloadAll();

    if (mRestartSelected) {
        mRestartSelected = false;
        new ERestartRace();
    }

    TheICEManager.SetGenericCameraToPlay(lbl_803E43DC, lbl_803E43DC);
    new ESndGameState(7, false);
    SetSoundControlState(true, static_cast< eSNDCTLSTATE >(7), lbl_803E60C8);
    mActive = false;
}

MenuScreen *PhotoFinishScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) PhotoFinishScreen(sd);
}
