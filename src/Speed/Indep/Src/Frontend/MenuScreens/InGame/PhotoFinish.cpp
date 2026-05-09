#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
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
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"

static bool gSillyTextureStreamerActive;

SillyTextureStreamerManager::SillyTextureStreamerManager(const char *stream_pack) {
    bStrNCpy(BundleFileName, stream_pack, 0x100);
    bMemSet(LoadInfos, 0, sizeof(LoadInfos));
    mCurrentLoadingIndex = -1;
    mMakeSpaceInPoolComplete = false;
    mCurrentlyLoading = true;
    gSillyTextureStreamerActive = true;
    TheTrackStreamer.MakeSpaceInPool(0x60000, MakeSpaceInPoolCallbackBridge, reinterpret_cast<int>(this));
}

SillyTextureStreamerManager::~SillyTextureStreamerManager() {
    if (!mMakeSpaceInPoolComplete) {
        TheTrackStreamer.WaitForCurrentLoadingToComplete();
    }
    eWaitForStreamingTexturePackLoading(nullptr);
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].LoadingTexture) {
            unsigned int tex = LoadInfos[i].LoadingTexture;
            eUnloadStreamingTexture(&tex, 1);
        }
    }
    eUnloadStreamingTexturePack(BundleFileName);
    gSillyTextureStreamerActive = false;
}

void SillyTextureStreamerManager::MakeSpaceInPoolCallback() {
    mMakeSpaceInPoolComplete = true;
    eLoadStreamingTexturePack(BundleFileName, reinterpret_cast<void (*)(void *)>(LoadCallbackBridge), reinterpret_cast<void *>(this), 0);
}

void SillyTextureStreamerManager::LoadCallback() {
    mCurrentlyLoading = false;
    if (mCurrentLoadingIndex >= 0) {
        int idx = mCurrentLoadingIndex;
        FEngSetTextureHash(LoadInfos[idx].LoadIntoImage, LoadInfos[idx].LoadingTexture);
        FEngSetVisible(reinterpret_cast<FEObject *>(LoadInfos[idx].LoadIntoImage));
        LoadInfos[idx].IsLoaded = true;
        mCurrentLoadingIndex = -1;
    }
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].LoadingTexture != 0 && !LoadInfos[i].IsLoaded) {
            mCurrentlyLoading = true;
            mCurrentLoadingIndex = i;
            unsigned int tex = LoadInfos[i].LoadingTexture;
            eLoadStreamingTexture(&tex, 1, LoadCallbackBridge, reinterpret_cast<unsigned int>(this), 7);
            return;
        }
    }
    cFEng::Get()->MakeLoadedPackagesDirty();
}

void SillyTextureStreamerManager::Load(unsigned int hash, FEImage *image) {
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].LoadingTexture == 0) {
            LoadInfos[i].LoadingTexture = hash;
            LoadInfos[i].LoadIntoImage = image;
            FEngSetInvisible(reinterpret_cast<FEObject *>(image));
            if (!mCurrentlyLoading) {
                LoadCallback();
            }
            return;
        }
    }
}

void SillyTextureStreamerManager::UnloadAll() {
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].IsLoaded) {
            unsigned int tex = LoadInfos[i].LoadingTexture;
            eUnloadStreamingTexture(&tex, 1);
            LoadInfos[i].LoadingTexture = 0;
            LoadInfos[i].IsLoaded = false;
        }
    }
}

bool PhotoFinishScreen::mRestartSelected = false;
float PhotoFinishScreen::mSpeedtrapSpeed = 0.0f;
float PhotoFinishScreen::mSpeedtrapBounty = 0.0f;
bool PhotoFinishScreen::mActive = false;

PhotoFinishScreen::PhotoFinishScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      mIceCamTimer() //
      ,
      mSlowdownTimer() //
      ,
      fResultType(static_cast<FERESULTTYPE>(sd->Arg)) //
      ,
      mPhotoHash(0) //
      ,
      StreamTex("GLOBAL\\HUDTEXTURESPHOTOFINISH.BIN") {
    if (fResultType == FERESULTTYPE_RACE) {
        if (GRaceStatus::Exists()) {
            GRaceStatus &race_status = GRaceStatus::Get();
            GRaceParameters *race_parameters = race_status.GetRaceParameters();
            if (race_parameters != nullptr) {
                bool is_boss_race = race_parameters->GetIsBossRace();
                const char *photo_texture = "PHOTOFINISH_RIVAL";

                if (race_status.GetRaceContext() != GRace::kRaceContext_Career || !is_boss_race) {
                    photo_texture = race_parameters->GetPhotoFinishTexture();
                }

                mPhotoHash = bStringHash(photo_texture);
                StreamTex.Load(mPhotoHash, FEngFindImage(GetPackageName(), 0x286A9CD4));
            }
        }
    }

    mSlowdownTimer = RealTimer;

    CameraAI::StartCinematicSlowdown(EVIEW_PLAYER1, 0.75f);
    SetSoundControlState(true, static_cast<eSNDCTLSTATE>(0xF), "CinemSlow");
    new EMomentStrm(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, nullptr, 0x9FE1EE17);
}

PhotoFinishScreen::~PhotoFinishScreen() {
    StreamTex.UnloadAll();

    if (mRestartSelected) {
        mRestartSelected = false;
        new ERestartRace();
    }

    TheICEManager.SetGenericCameraToPlay("", "");
    new ESndGameState(7, false);
    SetSoundControlState(false, static_cast<eSNDCTLSTATE>(1), "PhotoFin");
    mActive = false;
}

void PhotoFinishScreen::NotificationMessage(unsigned long msg, FEObject *, unsigned long, unsigned long) {
    switch (msg) {
        case 0x406415E3:
            if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                new EUnPause();
                new EAutoSave();

                MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
                SoundPause(false, eSNDPAUSE_PHOTOFINISH);
                SetSoundControlState(false, SNDSTATE_STOP_MUSIC, "PhotoFinish");
                return;
            }

            if (FEngIsScriptSet(GetPackageName(), 0x286A9CD4, 0x0016A259)) {
                return;
            }

            if (FEngIsScriptRunning(GetPackageName(), 0x286A9CD4, 0x5079C8F8)) {
                return;
            }

            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();
                if (race_parameters->GetIsBossRace()) {
                    unsigned char current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
                    GRaceBin *race_bin = GRaceDatabase::Get().GetBinNumber(current_bin);
                    int remaining_races = 0;

                    for (unsigned int i = 0; i < race_bin->GetBossRaceCount(); ++i) {
                        if (!GRaceDatabase::Get().CheckRaceScoreFlags(race_bin->GetBossRaceHash(i), GRaceDatabase::kCompleted_ContextCareer)) {
                            ++remaining_races;
                        }
                    }

                    new EFadeScreenOn(false);

                    if (current_bin != 1) {
                        if (remaining_races == 0) {
                            new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "SafeHouseRivalChallenge.fng");
                            return;
                        }
                    } else if (remaining_races == 1) {
                        cFEng::Get()->QueuePackagePop(1);

                        MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
                        return;
                    }

                    cFEng::Get()->QueuePackagePop(1);
                    new ERaceSheetOn(2);
                    return;
                }

                new EUnPause();

                MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
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
                cFEng::Get()->QueuePackageMessage(bStringHash("PAD_BUTTON4_CB"), GetPackageName(), nullptr);
                if (!cFEng::Get()->IsPackagePushed("InGameBackground.fng")) {
                    cFEng::Get()->QueuePackagePush("InGameBackground.fng", 0, 0, false);
                }
                new EShowResults(fResultType, false);
            }
            return;
        case 0xC519BFC4:
            if (fResultType != FERESULTTYPE_SPEEDTRAP) {
                DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD,
                                                0xE1A57D51, 0xB4623F67, 0xB4623F67, static_cast<eDialogFirstButtons>(1), 0x4D3399A8);
            }
            return;
        case 0xE1A57D51:
            cFEng::Get()->QueuePackageMessage(bStringHash("PAD_BUTTON5_CB"), GetPackageName(), nullptr);
            mRestartSelected = true;
            new EUnPause();
            return;
        case 0xC98356BA: {
            if (mSlowdownTimer.IsSet() && static_cast<float>(RealTimer.GetPackedTime() - mSlowdownTimer.GetPackedTime()) * 0.00025f >= 0.75f) {
                mSlowdownTimer.UnSet();
                mIceCamTimer = RealTimer;

                HideEverySingleHud();
                FEManager::RequestPauseSimulation(GetPackageName());
                *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&TheICEManager) + 0x7C) = 1;

                if (fResultType == FERESULTTYPE_PURSUIT) {
                    new ECinematicMoment("Cinematics", "DefaultFinish", 0.0f);
                } else if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                    new ECinematicMoment("Cinematics", "DefaultSpeed", 0.0f);
                } else {
                    new ECinematicMoment("Cinematics", GRaceStatus::Get().GetRaceParameters()->GetPhotoFinishCamera(), 0.0f);
                }
                return;
            }

            if (mIceCamTimer.IsSet() && static_cast<float>(RealTimer.GetPackedTime() - mIceCamTimer.GetPackedTime()) * 0.00025f >= 0.75f) {
                mIceCamTimer.UnSet();

                if (!FEngIsScriptSet(GetPackageName(), 0x47FF4E7C, 0x0013C37B)) {
                    FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x0013C37B, true);
                }

                if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                    if (!FEngIsScriptSet(GetPackageName(), 0x857FB472, 0x5079C8F8)) {
                        FEngSetScript(GetPackageName(), 0x857FB472, 0x5079C8F8, true);
                    }

                    FEngSetScript(GetPackageName(), bStringHash("SPEEDTRAP_GROUP"), 0x5079C8F8, true);
                    cFEng::Get()->QueuePackageMessage(bStringHash("SPEEDTRAP"), GetPackageName(), nullptr);
                } else {
                    if (mPhotoHash == bStringHash("PHOTOFINISH_TOOBOOTH")) {
                        FEngSetScript(GetPackageName(), bStringHash("TOLL_BOOTH_GROUP"), 0x5079C8F8, true);
                    } else if (mPhotoHash == bStringHash("PHOTOFINISH_RIVAL")) {
                        FEngSetScript(GetPackageName(), bStringHash("RIVAL_GROUP"), 0x5079C8F8, true);
                    } else {
                        FEngSetScript(GetPackageName(), bStringHash("SPRINT_GROUP"), 0x5079C8F8, true);
                    }

                    if (!FEngIsScriptSet(GetPackageName(), 0x286A9CD4, 0x5079C8F8)) {
                        FEngSetScript(GetPackageName(), 0x286A9CD4, 0x5079C8F8, true);
                    }
                }

                Setup();
                *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&TheICEManager) + 0x7C) = 0;

                MMiscSound sound_message(2);
                sound_message.Send(Attrib::StringHash32("Snd"));

                new ESndGameState(7, true);
                SoundPause(true, static_cast<eSNDPAUSE_REASON>(0xA));
                SetSoundControlState(false, static_cast<eSNDCTLSTATE>(0xF), "CinemSlow");
                SetSoundControlState(true, static_cast<eSNDCTLSTATE>(1), "PhotoFinish");
            }
            return;
        }
    }
}

void PhotoFinishScreen::Setup() {
    FEManager *fe_manager = FEManager::Get();
    fe_manager->AllowControllerError(true);

    unsigned int locale_hash = 0x8569AB44;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        locale_hash = 0x8569A25F;
    }

    if (fResultType == FERESULTTYPE_SPEEDTRAP) {
        float display_speed = mSpeedtrapSpeed * (locale_hash == 0x8569A25F ? 3.6f : 2.23699f);
        unsigned int speed_hash = bStringHash("SPEEDTRAP_SPEED");

        FEPrintf(GetPackageName(), speed_hash, "%$0.0f %s", GetTranslatedString(locale_hash), display_speed);
        unsigned int bounty_hash = bStringHash("BOUNTY_TEXT");
        FEPrintf(GetPackageName(), bounty_hash, GetTranslatedString(0x060C058A), static_cast<int>(mSpeedtrapBounty));
        return;
    }

    GRaceStatus &race_status = GRaceStatus::Get();
    GRaceParameters *race_params = race_status.GetRaceParameters();
    GRacerInfo *racer_info = &race_status.GetRacerInfo(0);
    int racer_count = race_status.GetRacerCount();

    for (int i = 0; i < racer_count; ++i) {
        racer_info = &race_status.GetRacerInfo(i);
        if (racer_info->GetSimable() != nullptr) {
            break;
        }
    }

    float cash = race_params->GetCashValue();
    float finishing_speed = racer_info->GetFinishingSpeed() * 2.23699f;
    float point_total = racer_info->GetPointTotal();

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        finishing_speed *= 1.60931f;
    } else {
        point_total *= 0.27778f;
        point_total *= 2.23699f;
    }

    Timer race_time;
    Timer lap_time;
    char race_time_buffer[32];
    char lap_time_buffer[32];
    char summary_buffer[64];

    race_time.SetTime(race_status.GetRaceTimeRemaining());
    race_time.PrintToString(race_time_buffer, 0);
    lap_time.SetTime(racer_info->GetRaceTime());
    lap_time.PrintToString(lap_time_buffer, 0);

    bSNPrintf(summary_buffer, 64, "%s %s %$0.0f %s", lap_time_buffer, GetTranslatedString(0x474), GetTranslatedString(locale_hash), finishing_speed);

    unsigned int result_hash;

    if (FEngIsScriptSet(GetPackageName(), bStringHash("TOLL_BOOTH_GROUP"), 0x5079C8F8)) {
        FEPrintf(GetPackageName(), 0x8BB39726, "%$0.0f %s", GetTranslatedString(locale_hash), finishing_speed);
        FEPrintf(GetPackageName(), 0x424BB244, "%s", summary_buffer);
        FEPrintf(GetPackageName(), 0x8A7F929C, "+%s", race_time_buffer);
        result_hash = 0x42423E94;
    } else if (FEngIsScriptSet(GetPackageName(), bStringHash("RIVAL_GROUP"), 0x5079C8F8)) {
        if (race_params->GetRaceType() == GRace::kRaceType_SpeedTrap) {
            FEPrintf(GetPackageName(), 0x37BEA03B, "%s: %$0.0f %s", GetTranslatedString(0x7F54569D), GetTranslatedString(locale_hash), point_total);
        } else {
            FEPrintf(GetPackageName(), 0x37BEA03B, "%s", summary_buffer);
        }
        result_hash = 0x9F4DF5BB;
    } else {
        if (race_params->GetRaceType() == GRace::kRaceType_SpeedTrap) {
            FEPrintf(GetPackageName(), 0xAB6AAFDD, "%s: %$0.0f %s", GetTranslatedString(0x7F54569D), GetTranslatedString(locale_hash), point_total);
        } else {
            FEPrintf(GetPackageName(), 0xAB6AAFDD, "%s", summary_buffer);
        }
        result_hash = 0x3D1773DD;
    }

    if (cash > 0.0f) {
        FEPrintf(GetPackageName(), result_hash, "%s: %$0.0f", GetTranslatedString(0xB7F2B3C8), cash);
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), result_hash));
    }

    if (race_params->GetEventHash() == Attrib::StringHash32("19.8.31")) {
        DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417B2601, 0x1FAB5998, 0x4C54B7EA);
        FEDatabase->GetCareerSettings()->SpecialFlags |= 0x2000;
    }
}

MenuScreen *PhotoFinishScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) PhotoFinishScreen(sd);
}
