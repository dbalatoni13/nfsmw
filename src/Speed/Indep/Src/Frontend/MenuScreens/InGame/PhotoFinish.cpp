#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/EAXSound/SoundPause.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Generated/Events/EAutoSave.hpp"
#include "Speed/Indep/Src/Generated/Events/ECinematicMoment.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

SillyTextureStreamerManager::SillyTextureStreamerManager(const char *stream_pack) {
    bStrNCpy(BundleFileName, stream_pack, 0x100);
    bMemSet(LoadInfos, 0, 0x30);
    mCurrentlyLoading = 0;

    mCurrentLoadingIndex = -1;
    mCurrentlyLoading = 1;
    mMakeSpaceInPoolComplete = 0;
    TheTrackStreamer.DisableZoneSwitching();
    TheTrackStreamer.MakeSpaceInPool(0x60000, MakeSpaceInPoolCallbackBridge, reinterpret_cast<intptr_t>(this));
}

SillyTextureStreamerManager::~SillyTextureStreamerManager() {
    if (!mMakeSpaceInPoolComplete) {
        TheTrackStreamer.WaitForCurrentLoadingToComplete();
    }
    eWaitForStreamingTexturePackLoading(nullptr);
    for (int i = 0; i <= 3; i++) {
        if (LoadInfos[i].LoadingTexture != 0) {
            unsigned int tex = LoadInfos[i].LoadingTexture;
            eUnloadStreamingTexture(&tex, 1);
        }
    }
    eUnloadStreamingTexturePack(BundleFileName);
    TheTrackStreamer.EnableZoneSwitching();
}

void SillyTextureStreamerManager::MakeSpaceInPoolCallback() {
    mMakeSpaceInPoolComplete = 1;
    eLoadStreamingTexturePack(BundleFileName, reinterpret_cast<void (*)(void *)>(LoadCallbackBridge), this, 0);
}

void SillyTextureStreamerManager::LoadCallback() {
    mCurrentlyLoading = 0;
    int idx = mCurrentLoadingIndex;
    if (idx >= 0) {
        FEngSetTextureHash(LoadInfos[idx].LoadIntoImage, LoadInfos[idx].LoadingTexture);
        FEngSetVisible(reinterpret_cast<FEObject *>(LoadInfos[idx].LoadIntoImage));
        LoadInfos[idx].IsLoaded = 1;
        mCurrentLoadingIndex = -1;
    }
    for (int i = 0; i <= 3; i++) {
        if (LoadInfos[i].LoadingTexture != 0 && LoadInfos[i].IsLoaded == 0) {
            mCurrentlyLoading = 1;
            mCurrentLoadingIndex = i;
            eLoadStreamingTexture(LoadInfos[i].LoadingTexture, reinterpret_cast<void (*)(uintptr_t)>(LoadCallbackBridge),
                                  reinterpret_cast<uintptr_t>(this), 7);
            return;
        }
    }
    cFEng::Get()->MakeLoadedPackagesDirty();
}

void SillyTextureStreamerManager::Load(unsigned int hash, FEImage *image) {
    for (int i = 0; i <= 3; i++) {
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
    for (int i = 0; i <= 3; i++) {
        if (LoadInfos[i].IsLoaded != 0) {
            unsigned int tex = LoadInfos[i].LoadingTexture;
            eUnloadStreamingTexture(&tex, 1);
            LoadInfos[i].LoadingTexture = 0;
            LoadInfos[i].IsLoaded = 0;
        }
    }
}

inline void SillyTextureStreamerManager::MakeSpaceInPoolCallbackBridge(int param) {
    reinterpret_cast<SillyTextureStreamerManager *>(param)->MakeSpaceInPoolCallback();
}

inline void SillyTextureStreamerManager::LoadCallbackBridge(unsigned int param) {
    reinterpret_cast<SillyTextureStreamerManager *>(param)->LoadCallback();
}

float PhotoFinishScreen::mSpeedtrapSpeed = 0.0f;
float PhotoFinishScreen::mSpeedtrapBounty = 0.0f;
bool PhotoFinishScreen::mRestartSelected = false;
bool PhotoFinishScreen::mActive = false;

PhotoFinishScreen::PhotoFinishScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), fResultType(static_cast<FERESULTTYPE>(sd->Arg)), mPhotoHash(0), StreamTex("GLOBAL\\HUDTEXTURESPHOTOFINISH.BIN") {

    if (fResultType == FERESULTTYPE_RACE) {
        bool bossRace = GRaceStatus::Get().GetRaceParameters()->GetIsBossRace();
        bool careerMode = GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career;
        if (careerMode && bossRace) {
            mPhotoHash = bStringHash("PHOTOFINISH_RIVAL");
        } else {
            mPhotoHash = bStringHash(GRaceStatus::Get().GetRaceParameters()->GetPhotoFinishTexture());
        }
        StreamTex.Load(mPhotoHash, FEngFindImage(GetPackageName(), 0x286A9CD4));
    }

    mIceCamTimer = RealTimer;
    CameraAI::StartCinematicSlowdown(EVIEW_PLAYER1, 0.75f);
    SetSoundControlState(true, SNDSTATE_FADEOUT, "CinemSlow");

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
    SetSoundControlState(false, SNDSTATE_PAUSE, "PhotoFin");

    mActive = false;
}

void PhotoFinishScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x406415E3: {
            if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                new EUnPause();
                new EAutoSave();
                MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
                SoundPause(false, eSNDPAUSE_PHOTOFINISH);
                SetSoundControlState(false, SNDSTATE_STOP_MUSIC, "PhotoFinish");
                return;
            }

            if (FEngIsScriptSet(GetPackageName(), 0x286A9CD4, FEHASH_HIDE)) {
                return;
            }
            if (FEngIsScriptRunning(GetPackageName(), 0x286A9CD4, FEHASH_APPEAR)) {
                return;
            }

            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                if (GRaceStatus::Get().GetRaceParameters()->GetIsBossRace()) {
                    int numUnfinished = 0;
                    int currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
                    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(currentBin);

                    for (unsigned int i = 0; i < bin->GetBossRaceCount(); i++) {
                        if (!GRaceDatabase::Get().CheckRaceScoreFlags(bin->GetBossRaceHash(i), GRaceDatabase::kCompleted_ContextCareer)) {
                            numUnfinished++;
                        }
                    }

                    new EFadeScreenOn(false);

                    if (currentBin != 1 && numUnfinished == 0) {
                        new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "SafeHouseRivalChallenge.fng");
                    } else if (currentBin == 1 && numUnfinished == 1) {
                        cFEng::Get()->QueuePackagePop(1);
                        MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
                    } else {
                        cFEng::Get()->QueuePackagePop(1);
                        new ERaceSheetOn(2);
                    }
                    return;
                }

                new EUnPause();
                MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));
                return;
            }

            if ((FEDatabase->GetGameMode() & eFE_GAME_MODE_CHALLENGE) && MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
                MemcardEnter(nullptr, nullptr, 0x100B1, nullptr, nullptr, 0, 0);
                return;
            }

            new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
            return;
        }
        case 0xC519BFC3: {
            if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                return;
            }

            cFEng::Get()->QueuePackageMessage(bStringHash("PAD_BUTTON4_CB"), GetPackageName(), nullptr);

            if (!cFEng::Get()->IsPackagePushed("InGameBackground.fng")) {
                cFEng::Get()->QueuePackagePush("InGameBackground.fng", 0, 0, false);
            }

            new EShowResults(fResultType, false);
            return;
        }
        case 0xC519BFC4: {
            if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                return;
            }

            DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL, 0xE1A57D51, 0xB4623F67,
                                            0xB4623F67, first_dialog_button2, static_cast<unsigned int>(0x4D3399A8));
            return;
        }
        case 0xE1A57D51: {
            cFEng::Get()->QueuePackageMessage(bStringHash("PAD_BUTTON5_CB"), GetPackageName(), nullptr);
            mRestartSelected = true;
            new EUnPause();
            return;
        }
        case 0xC98356BA: {
            if (mIceCamTimer.IsSet()) {
                if ((RealTimer - mIceCamTimer).GetSeconds() >= 0.75f) {
                    mIceCamTimer.UnSet();
                    mSlowdownTimer = RealTimer;
                    HideEverySingleHud();
                    FEManager::RequestPauseSimulation(GetPackageName());
                    TheICEManager.SetUseRealTime(true);

                    if (fResultType == FERESULTTYPE_PURSUIT) {
                        new ECinematicMoment("Cinematics", "DefaultFinish", 0.0f);
                    } else if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                        new ECinematicMoment("Cinematics", "DefaultSpeed", 0.0f);
                    } else {
                        new ECinematicMoment("Cinematics", GRaceStatus::Get().GetRaceParameters()->GetPhotoFinishCamera(), 0.0f);
                    }
                    return;
                }
            }

            if (mSlowdownTimer.IsSet()) {
                if ((RealTimer - mSlowdownTimer).GetSeconds() < 0.75f) {
                    return;
                }

                mSlowdownTimer.UnSet();

                if (!FEngIsScriptSet(GetPackageName(), 0x47FF4E7C, 0x13C37B)) {
                    FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x13C37B, true);
                }

                if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                    if (!FEngIsScriptSet(GetPackageName(), 0x857FB472, FEHASH_APPEAR)) {
                        FEngSetScript(GetPackageName(), 0x857FB472, FEHASH_APPEAR, true);
                    }
                    FEngSetScript(GetPackageName(), bStringHash("SPEEDTRAP_GROUP"), FEHASH_APPEAR, true);
                    cFEng::Get()->QueuePackageMessage(bStringHash("SPEEDTRAP"), GetPackageName(), nullptr);
                } else {
                    if (mPhotoHash == bStringHash("PHOTOFINISH_TOOBOOTH")) {
                        FEngSetScript(GetPackageName(), bStringHash("TOLL_BOOTH_GROUP"), FEHASH_APPEAR, true);
                    } else if (mPhotoHash == bStringHash("PHOTOFINISH_RIVAL")) {
                        FEngSetScript(GetPackageName(), bStringHash("RIVAL_GROUP"), FEHASH_APPEAR, true);
                    } else {
                        FEngSetScript(GetPackageName(), bStringHash("SPRINT_GROUP"), FEHASH_APPEAR, true);
                    }

                    if (!FEngIsScriptSet(GetPackageName(), 0x286A9CD4, FEHASH_APPEAR)) {
                        FEngSetScript(GetPackageName(), 0x286A9CD4, FEHASH_APPEAR, true);
                    }
                }

                Setup();
                TheICEManager.SetUseRealTime(false);
                MMiscSound(2).Send(UCrc32("Snd"));
                new ESndGameState(7, true);
                SoundPause(true, eSNDPAUSE_PHOTOFINISH);
                SetSoundControlState(false, SNDSTATE_FADEOUT, "CinemSlow");
                SetSoundControlState(true, SNDSTATE_PAUSE, "PhotoFinish");
            }
            return;
        }
        default:
            break;
    }
}

void PhotoFinishScreen::Setup() {
    FEManager::Get()->AllowControllerError(true);

    unsigned int speedUnits = 0x8569AB44;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
    }

    if (fResultType == FERESULTTYPE_SPEEDTRAP) {
        float converted_speed;
        if (speedUnits == 0x8569A25F) {
            converted_speed = MPS2KPH(mSpeedtrapSpeed);
        } else {
            converted_speed = MPS2MPH(mSpeedtrapSpeed);
        }
        FEPrintf(GetPackageName(), bStringHash("SPEEDTRAP_SPEED"), "%$0.0f %s", converted_speed, GetTranslatedString(speedUnits));
        FEPrintf(GetPackageName(), bStringHash("BOUNTY_TEXT"), GetTranslatedString(0x060C058A), static_cast<int>(mSpeedtrapBounty));
    } else {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(0);

        for (int i = 0; i < GRaceStatus::Get().GetRacerCount(); i++) {
            racerInfo = GRaceStatus::Get().GetRacerInfo(i);
            if (racerInfo.GetSimable()->IsPlayer()) {
                break;
            }
        }

        float cashEarned = GRaceStatus::Get().GetRaceParameters()->GetCashValue();
        float pointsEarned = racerInfo.GetPointTotal();
        float speed = MPS2MPH(racerInfo.GetFinishingSpeed());

        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
            speed = speed * 1.60931003f;
        } else {
            pointsEarned = MPS2MPH(pointsEarned * 0.27777999f);
        }

        char bonusTime[TIMER_STRING_SIZE];
        Timer bt(GRaceStatus::Get().GetRaceTimeRemaining());
        bt.PrintToString(bonusTime, 0);

        char time[TIMER_STRING_SIZE];
        Timer t(racerInfo.GetRaceTimer().GetTime());
        t.PrintToString(time, 0);

        char timeAndSpeed[64];
        bSNPrintf(timeAndSpeed, 0x40, "%s %s %$0.0f %s", time, GetTranslatedString(0x474), speed, GetTranslatedString(speedUnits));

        int cashHash;
        if (FEngIsScriptSet(GetPackageName(), bStringHash("TOLL_BOOTH_GROUP"), FEHASH_APPEAR)) {
            FEPrintf(GetPackageName(), 0x8BB39726, "%$0.0f %s", speed, GetTranslatedString(speedUnits));
            FEPrintf(GetPackageName(), 0x424BB244, "%s", time);
            FEPrintf(GetPackageName(), 0x8A7F929C, "+%s", bonusTime);
            cashHash = 0x42423E94;
        } else if (FEngIsScriptSet(GetPackageName(), bStringHash("RIVAL_GROUP"), FEHASH_APPEAR)) {
            if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
                FEPrintf(GetPackageName(), 0x37BEA03B, "%s: %$0.0f %s", GetTranslatedString(0x7F54569D), pointsEarned, GetTranslatedString(speedUnits));
            } else {
                FEPrintf(GetPackageName(), 0x37BEA03B, "%s", timeAndSpeed);
            }
            cashHash = 0x9F4DF5BB;
        } else {
            if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
                FEPrintf(GetPackageName(), 0xAB6AAFDD, "%s: %$0.0f %s", GetTranslatedString(0x7F54569D), pointsEarned, GetTranslatedString(speedUnits));
            } else {
                FEPrintf(GetPackageName(), 0xAB6AAFDD, "%s", timeAndSpeed);
            }
            cashHash = 0x3D1773DD;
        }

        if (cashEarned > 0.0f && GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_QuickRace) {
            FEPrintf(GetPackageName(), cashHash, "%s: %$0.0f", GetTranslatedString(0xB7F2B3C8), cashEarned);
        } else {
            FEngSetInvisible(GetPackageName(), cashHash);
        }
    }

    if (GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->GetEventHash() == Attrib::StringHash32("19.8.31")) {
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x1FAB5998, 0x4C54B7EA);
        FEDatabase->GetCareerSettings()->SetAwardedBKReward();
    }
}

MenuScreen *PhotoFinishScreen::Create(ScreenConstructorData *sd) {
    return new ("PhotoFinishScreen", 0) PhotoFinishScreen(sd);
}
