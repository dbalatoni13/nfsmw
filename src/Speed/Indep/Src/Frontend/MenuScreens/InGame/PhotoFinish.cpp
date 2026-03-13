#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Generated/Events/EAutoSave.hpp"
#include "Speed/Indep/Src/Generated/Events/ECinematicMoment.hpp"
#include "Speed/Indep/Src/Generated/Events/ECameraPhotoFinish.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern Timer RealTimer;

extern unsigned int bStringHash(const char *str);
extern int FEPrintf(const char *pkg_name, int hash, const char *fmt, ...);
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern void FEngSetInvisible(FEObject *obj);
extern bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
extern bool FEngIsScriptRunning(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash,
                          bool start_at_beginning);
extern void HideEverySingleHud();
extern void SoundPause(bool pause, eSNDPAUSE_REASON reason);
extern void StartCinematicSlowdown(EVIEW_ID view_id, float amount);

extern const char lbl_803E485C[];
extern const char lbl_803E43DC[];
extern const char lbl_803E6080[];
extern const char lbl_803E60A4[];
extern const char lbl_803E60B8[];
extern const char lbl_803E60C8[];
extern const char lbl_803E60D4[];
extern const char lbl_803E60E0[];
extern const char lbl_803E60FC[];
extern const char lbl_803E610C[];
extern const char lbl_803E611C[];
extern const char lbl_803E6128[];
extern const char lbl_803E6138[];
extern const char lbl_803E6148[];
extern const char lbl_803E6158[];
extern const char lbl_803E6164[];
extern const char lbl_803E619C[];
extern const char lbl_803E4CF0[];
extern const char lbl_803E4FF8[];
extern const char lbl_803E5084[];
extern const char lbl_803E5EEC[];
extern const char lbl_803E5F18[];
extern const char lbl_803E5FD8[];
extern const char lbl_803E617C[];
extern const char lbl_803E6190[];
extern const char lbl_803E61C4[];
extern const char lbl_803E61D4[];
extern const char lbl_803E61E0[];
extern const char lbl_803E61F0[];
extern const char lbl_803E4744[];
extern const float lbl_803E60C4;
extern const float lbl_803E61BC;
extern const float lbl_803E61C0;
extern const float lbl_803E6200;
extern const float lbl_803E6204;
extern const float lbl_803E6208;
extern const float lbl_803E620C;
extern const float lbl_803E6210;

bool PhotoFinishScreen::mRestartSelected = false;
float PhotoFinishScreen::mSpeedtrapSpeed = 0.0f;
float PhotoFinishScreen::mSpeedtrapBounty = 0.0f;
bool PhotoFinishScreen::mActive = false;

PhotoFinishScreen::PhotoFinishScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , mIceCamTimer() //
    , mSlowdownTimer() //
    , fResultType(static_cast< FERESULTTYPE >(sd->Arg)) //
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

void PhotoFinishScreen::Setup() {
    FEManager::Get();

    unsigned int locale_hash = 0x8569AB44;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        locale_hash = 0x8569A25F;
    }

    if (fResultType == FERESULTTYPE_SPEEDTRAP) {
        float display_speed = mSpeedtrapSpeed * (locale_hash == 0x8569A25F ? lbl_803E6200 : lbl_803E6204);
        unsigned int speed_hash = bStringHash(lbl_803E61C4);
        unsigned int bounty_hash = bStringHash(lbl_803E61D4);

        FEPrintf(GetPackageName(), speed_hash, lbl_803E4FF8, GetTranslatedString(locale_hash), display_speed);
        FEPrintf(GetPackageName(), bounty_hash, GetTranslatedString(0x060C058A), static_cast< int >(mSpeedtrapBounty));
        return;
    }

    GRaceStatus &race_status = GRaceStatus::Get();
    GRaceParameters *race_params = race_status.GetRaceParameters();
    GRacerInfo racer_info = race_status.GetRacerInfo(0);
    int racer_count = race_status.GetRacerCount();

    for (int i = 0; i < racer_count; ++i) {
        racer_info = race_status.GetRacerInfo(i);
        if (racer_info.GetSimable() != nullptr) {
            break;
        }
    }

    float cash = race_params->GetCashValue();
    float finishing_speed = racer_info.GetFinishingSpeed() * lbl_803E6204;
    float point_total = racer_info.GetPointTotal();

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        finishing_speed *= lbl_803E6208;
    } else {
        point_total *= lbl_803E620C;
        point_total *= lbl_803E6204;
    }

    Timer race_time;
    Timer lap_time;
    char race_time_buffer[64];
    char lap_time_buffer[64];
    char summary_buffer[64];

    race_time.SetTime(race_status.GetRaceTimeRemaining());
    race_time.PrintToString(race_time_buffer, 0);
    lap_time.SetTime(racer_info.GetRaceTimer().GetTime());
    lap_time.PrintToString(lap_time_buffer, 0);

    bSNPrintf(summary_buffer, 64, lbl_803E61E0, lap_time_buffer, GetTranslatedString(0x474),
              GetTranslatedString(locale_hash), finishing_speed);

    unsigned int result_hash;

    if (FEngIsScriptSet(GetPackageName(), bStringHash(lbl_803E617C), 0x5079C8F8)) {
        FEPrintf(GetPackageName(), 0x8BB39726, lbl_803E4FF8, GetTranslatedString(locale_hash), finishing_speed);
        FEPrintf(GetPackageName(), 0x424BB244, lbl_803E4CF0, summary_buffer);
        FEPrintf(GetPackageName(), 0x8A7F929C, lbl_803E5084, race_time_buffer);
        result_hash = 0x42423E94;
    } else if (FEngIsScriptSet(GetPackageName(), bStringHash(lbl_803E6190), 0x5079C8F8)) {
        if (race_params->GetRaceType() == GRace::kRaceType_SpeedTrap) {
            FEPrintf(GetPackageName(), 0x37BEA03B, lbl_803E61F0, GetTranslatedString(0x7F54569D),
                     GetTranslatedString(locale_hash), point_total);
        } else {
            FEPrintf(GetPackageName(), 0x37BEA03B, lbl_803E4CF0, summary_buffer);
        }
        result_hash = 0x9F4DF5BB;
    } else {
        if (race_params->GetRaceType() == GRace::kRaceType_SpeedTrap) {
            FEPrintf(GetPackageName(), 0xAB6AAFDD, lbl_803E61F0, GetTranslatedString(0x7F54569D),
                     GetTranslatedString(locale_hash), point_total);
        } else {
            FEPrintf(GetPackageName(), 0xAB6AAFDD, lbl_803E4CF0, summary_buffer);
        }
        result_hash = 0x3D1773DD;
    }

    if (cash > lbl_803E6210 && race_params != nullptr) {
        FEPrintf(GetPackageName(), result_hash, lbl_803E5FD8, GetTranslatedString(0xB7F2B3C8), cash);
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), result_hash));
    }

    if (race_params != nullptr && race_params->GetEventHash() == Attrib::StringHash32(lbl_803E4744)) {
        DialogInterface::ShowOneButton(GetPackageName(), lbl_803E43DC, static_cast< eDialogTitle >(1),
                                       0x417B2601, 0x1FAB5998, 0x4C54B7EA);
        FEDatabase->GetCareerSettings()->SpecialFlags |= 0x2000;
    }
}

void PhotoFinishScreen::NotificationMessage(unsigned long msg, FEObject *, unsigned long, unsigned long) {
    switch (msg) {
        case 0x406415E3:
            if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                UCrc32 kind(0x20D60DBF);
                MFlowReadyForOutro outro_message;

                new EUnPause();
                new EAutoSave();
                outro_message.Post(kind);
                SoundPause(false, static_cast< eSNDPAUSE_REASON >(0xA));
                SetSoundControlState(false, static_cast< eSNDCTLSTATE >(0xC), lbl_803E60D4);
                return;
            }

            if (FEngIsScriptSet(GetPackageName(), 0x286A9CD4, 0x0016A259) ||
                FEngIsScriptRunning(GetPackageName(), 0x286A9CD4, 0x5079C8F8)) {
                return;
            }

            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();
                if (race_parameters->GetIsBossRace()) {
                    GRaceDatabase &race_database = GRaceDatabase::Get();
                    unsigned char current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
                    GRaceBin *race_bin = race_database.GetBinNumber(current_bin);
                    int remaining_races = 0;

                    for (unsigned int i = 0; i < race_bin->GetBossRaceCount(); ++i) {
                        if (!race_database.CheckRaceScoreFlags(race_bin->GetBossRaceHash(i),
                                                               GRaceDatabase::kCompleted_ContextCareer)) {
                            ++remaining_races;
                        }
                    }

                    new EFadeScreenOn(false);

                    if (current_bin == 1 && remaining_races == 1) {
                        UCrc32 kind(0x20D60DBF);
                        MFlowReadyForOutro outro_message;

                        cFEng::Get()->QueuePackagePop(1);
                        outro_message.Post(kind);
                    } else if (current_bin != 1 && remaining_races == 0) {
                        new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, lbl_803E60E0);
                    } else {
                        cFEng::Get()->QueuePackagePop(1);
                        new ERaceSheetOn(2);
                    }
                    return;
                }

                UCrc32 kind(0x20D60DBF);
                MFlowReadyForOutro outro_message;

                new EUnPause();
                outro_message.Post(kind);
                return;
            }

            if ((FEDatabase->GetGameMode() & 0x2) && MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
                MemcardEnter(nullptr, nullptr, 0x100B1, nullptr, nullptr, 0, 0);
            } else {
                new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
            }
            return;
        case 0xC519BFC3:
            if (fResultType != FERESULTTYPE_SPEEDTRAP) {
                cFEng::Get()->QueuePackageMessage(bStringHash(lbl_803E60FC), GetPackageName(), nullptr);
                if (!cFEng::Get()->IsPackagePushed(lbl_803E5F18)) {
                    cFEng::Get()->QueuePackagePush(lbl_803E5F18, 0, 0, false);
                }
                new EShowResults(fResultType, false);
            }
            return;
        case 0xC519BFC4:
            if (fResultType != FERESULTTYPE_SPEEDTRAP) {
                DialogInterface::ShowTwoButtons(GetPackageName(), lbl_803E5EEC, static_cast< eDialogTitle >(1),
                                                0x417B2601, 0x1A294DAD, 0xE1A57D51, 0xB4623F67, 0xB4623F67,
                                                static_cast< eDialogFirstButtons >(1), 0x4D3399A8);
            }
            return;
        case 0xE1A57D51:
            cFEng::Get()->QueuePackageMessage(bStringHash(lbl_803E610C), GetPackageName(), nullptr);
            mRestartSelected = true;
            new EUnPause();
            return;
        case 0xC98356BA: {
            int packed_time = mSlowdownTimer.GetPackedTime();
            if (packed_time != 0 && packed_time != 0x7FFFFFFF &&
                RealTimer.GetSeconds() - mSlowdownTimer.GetSeconds() >= lbl_803E61BC) {
                mSlowdownTimer = Timer();
                mIceCamTimer = RealTimer;

                HideEverySingleHud();
                FEManager::RequestPauseSimulation(GetPackageName());

                if (fResultType == FERESULTTYPE_PURSUIT) {
                    new ECinematicMoment(lbl_803E611C, lbl_803E6128, lbl_803E61C0);
                } else if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                    new ECinematicMoment(lbl_803E611C, lbl_803E6138, lbl_803E61C0);
                } else {
                    new ECinematicMoment(lbl_803E611C, GRaceStatus::Get().GetRaceParameters()->GetPhotoFinishCamera(),
                                         lbl_803E61C0);
                }
                return;
            }

            packed_time = mIceCamTimer.GetPackedTime();
            if (packed_time != 0 && packed_time != 0x7FFFFFFF &&
                RealTimer.GetSeconds() - mIceCamTimer.GetSeconds() >= lbl_803E61BC) {
                mIceCamTimer = Timer();

                if (!FEngIsScriptSet(GetPackageName(), 0x47FF4E7C, 0x0013C37B)) {
                    FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x0013C37B, true);
                }

                if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                    if (!FEngIsScriptSet(GetPackageName(), 0x857FB472, 0x5079C8F8)) {
                        FEngSetScript(GetPackageName(), 0x857FB472, 0x5079C8F8, true);
                    }

                    FEngSetScript(GetPackageName(), bStringHash(lbl_803E6148), 0x5079C8F8, true);
                    cFEng::Get()->QueuePackageMessage(bStringHash(lbl_803E6158), GetPackageName(), nullptr);
                } else {
                    if (mPhotoHash == bStringHash(lbl_803E6164)) {
                        FEngSetScript(GetPackageName(), bStringHash(lbl_803E617C), 0x5079C8F8, true);
                    } else if (mPhotoHash == bStringHash(lbl_803E60A4)) {
                        FEngSetScript(GetPackageName(), bStringHash(lbl_803E6190), 0x5079C8F8, true);
                    } else {
                        FEngSetScript(GetPackageName(), bStringHash(lbl_803E619C), 0x5079C8F8, true);
                    }

                    if (!FEngIsScriptSet(GetPackageName(), 0x286A9CD4, 0x5079C8F8)) {
                        FEngSetScript(GetPackageName(), 0x286A9CD4, 0x5079C8F8, true);
                    }
                }

                Setup();

                MMiscSound sound_message(2);
                sound_message.Send(Attrib::StringHash32(lbl_803E485C));

                new ESndGameState(7, true);
                SoundPause(true, static_cast< eSNDPAUSE_REASON >(0xA));
                SetSoundControlState(false, static_cast< eSNDCTLSTATE >(0xF), lbl_803E60B8);
                SetSoundControlState(true, static_cast< eSNDCTLSTATE >(1), lbl_803E60D4);
            }
            return;
        }
    }
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
