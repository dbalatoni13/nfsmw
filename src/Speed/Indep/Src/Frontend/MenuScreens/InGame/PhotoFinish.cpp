#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Generated/Events/EAutoSave.hpp"
#include "Speed/Indep/Src/Generated/Events/ECinematicMoment.hpp"
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
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"

// UNSOLVED
SillyTextureStreamerManager::SillyTextureStreamerManager(const char *stream_pack) {
    bStrNCpy(BundleFileName, stream_pack, sizeof(BundleFileName));
    bMemSet(LoadInfos, 0, sizeof(LoadInfos));
    mCurrentLoadingIndex = -1;
    mMakeSpaceInPoolComplete = false;
    mCurrentlyLoading = true;
    TheTrackStreamer.DisableZoneSwitching();
    int mem_needed = 0x60000;
    TheTrackStreamer.MakeSpaceInPool(mem_needed, MakeSpaceInPoolCallbackBridge, reinterpret_cast<int>(this));
}

SillyTextureStreamerManager::~SillyTextureStreamerManager() {
    if (!mMakeSpaceInPoolComplete) {
        TheTrackStreamer.WaitForCurrentLoadingToComplete();
    }
    eWaitForStreamingTexturePackLoading(nullptr);
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].LoadingTexture) {
            eUnloadStreamingTexture(LoadInfos[i].LoadingTexture);
        }
    }
    eUnloadStreamingTexturePack(BundleFileName);
    TheTrackStreamer.EnableZoneSwitching();
}

void SillyTextureStreamerManager::MakeSpaceInPoolCallback() {
    mMakeSpaceInPoolComplete = true;
    eLoadStreamingTexturePack(BundleFileName, LoadCallbackBridge, reinterpret_cast<uint32>(this), 0);
}

void SillyTextureStreamerManager::LoadCallback() {
    mCurrentlyLoading = false;
    if (mCurrentLoadingIndex >= 0) {
        int param = mCurrentLoadingIndex;
        FEngSetTextureHash(LoadInfos[param].LoadIntoImage, LoadInfos[param].LoadingTexture);
        FEngSetVisible(LoadInfos[param].LoadIntoImage);
        LoadInfos[param].IsLoaded = true;
        mCurrentLoadingIndex = -1;
    }
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].LoadingTexture != 0 && !LoadInfos[i].IsLoaded) {
            mCurrentlyLoading = true;
            mCurrentLoadingIndex = i;
            eLoadStreamingTexture(LoadInfos[i].LoadingTexture, LoadCallbackBridge, reinterpret_cast<uint32>(this), 7);
            return;
        }
    }
    cFEng::Get()->MakeLoadedPackagesDirty();
}

void SillyTextureStreamerManager::Load(uint32 hash, FEImage *image) {
    for (int i = 0; i < 4; i++) {
        if (LoadInfos[i].LoadingTexture == 0) {
            LoadInfos[i].LoadingTexture = hash;
            LoadInfos[i].LoadIntoImage = image;
            FEngSetInvisible(image);
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
            eUnloadStreamingTexture(LoadInfos[i].LoadingTexture);
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
    : MenuScreen(sd), mIceCamTimer(), mSlowdownTimer(), fResultType(static_cast<FERESULTTYPE>(sd->Arg)), mPhotoHash(0),
      StreamTex("GLOBAL\\HUDTEXTURESPHOTOFINISH.BIN") {
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

    mSlowdownTimer = RealTimer;

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

void PhotoFinishScreen::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_ACCEPT__:
            if (fResultType == FERESULTTYPE_SPEEDTRAP) {
                extern int foo; // TODO: idk

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

            if (FEngIsScriptRunning(GetPackageName(), 0x286A9CD4, FEHASH_APPEAR)) {
                return;
            }

            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
                if (parms->GetIsBossRace()) {
                    bool all_races_done;
                    int num_unfinished_races = 0;
                    int bin_number = FEDatabase->GetCareerSettings()->GetCurrentBin();
                    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(bin_number);

                    for (uint32 i = 0; i < bin->GetBossRaceCount(); i++) {
                        if (!GRaceDatabase::Get().IsCareerRaceComplete(bin->GetBossRaceHash(i))) {
                            num_unfinished_races++;
                        }
                    }

                    new EFadeScreenOn(false);

                    if (bin_number != 1) {
                        if (num_unfinished_races == 0) {
                            new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "SafeHouseRivalChallenge.fng");
                            return;
                        }
                    } else if (num_unfinished_races == 1) {
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

            if ((FEDatabase->IsChallengeMode()) && MemoryCard::GetInstance()->ShouldDoAutoSave(false)) {
                MemcardEnter(nullptr, nullptr, 0x100B1, nullptr, nullptr, 0, 0);
            } else {
                new EQuitToFE(GARAGETYPE_MAIN_FE, nullptr);
            }
            return;
        case __PAD_BUTTON4__:
            if (fResultType != FERESULTTYPE_SPEEDTRAP) {
                cFEng::Get()->QueuePackageMessage(bStringHash("PAD_BUTTON4_CB"), GetPackageName(), nullptr);
                if (!cFEng::Get()->IsPackagePushed("InGameBackground.fng")) {
                    cFEng::Get()->QueuePackagePush("InGameBackground.fng", 0, 0, false);
                }
                new EShowResults(fResultType, false);
            }
            return;
        case __PAD_BUTTON5__:
            if (fResultType != FERESULTTYPE_SPEEDTRAP) {
                DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, 0x417B2601, 0x1A294DAD, 0xE1A57D51, 0xB4623F67,
                                                0xB4623F67, first_dialog_button2, 0x4D3399A8);
            }
            return;
        case 0xE1A57D51:
            cFEng::Get()->QueuePackageMessage(bStringHash("PAD_BUTTON5_CB"), GetPackageName(), nullptr);
            mRestartSelected = true;
            new EUnPause();
            return;
        case FEMSG_SCREEN_TICK: {

            if ((mSlowdownTimer.IsSet() != 0) && (RealTimer - mSlowdownTimer).GetSeconds() >= 0.75f) {
                mSlowdownTimer.UnSet();
                mIceCamTimer = RealTimer;

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

            if ((mIceCamTimer.IsSet() != 0) && (RealTimer - mIceCamTimer).GetSeconds() >= 0.75f) {
                extern ICEManager TheICEManager; // TODO: /shrug

                mIceCamTimer.UnSet();

                if (!FEngIsScriptSet(GetPackageName(), 0x47FF4E7C, 0x0013C37B)) {
                    FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x0013C37B, true);
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

                MMiscSound(2).Send("Snd");

                new ESndGameState(7, true);
                SoundPause(true, eSNDPAUSE_PHOTOFINISH);
                SetSoundControlState(false, SNDSTATE_FADEOUT, "CinemSlow");
                SetSoundControlState(true, SNDSTATE_PAUSE, "PhotoFinish");
            }
            return;
        }
    }
}

// UNSOLVED
void PhotoFinishScreen::Setup() {
    FEManager::Get()->AllowControllerError(true);

    uint32 speedUnits = 0x8569AB44;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedUnits = 0x8569A25F;
    }

    if (fResultType == FERESULTTYPE_SPEEDTRAP) {
        float converted_speed = (speedUnits == 0x8569A25F ? MPS2KPH(mSpeedtrapSpeed) : MPS2MPH(mSpeedtrapSpeed));

        FEPrintf(GetPackageName(), bStringHash("SPEEDTRAP_SPEED"), "%$0.0f %s", converted_speed, GetTranslatedString(speedUnits));
        FEPrintf(GetPackageName(), bStringHash("BOUNTY_TEXT"), GetTranslatedString(0x060C058A), static_cast<int>(mSpeedtrapBounty));
    } else {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(0);

        for (int i = 0; i < GRaceStatus::Get().GetRacerCount(); i++) {
            racerInfo =
                GRaceStatus::Get().GetRacerInfo(i); // TODO Is this memcopy a bug? Shouldn't this just assign reference? racerInfo should be a pointer

            if (racerInfo.GetSimable()->IsPlayer()) {
                break;
            }
        }

        float cashEarned = GRaceStatus::Get().GetRaceParameters()->GetCashValue();
        float pointsEarned = racerInfo.GetPointTotal();
        float speed = MPS2MPH(racerInfo.GetFinishingSpeed());

        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
            speed = MPH2KPH(speed);
        } else {
            pointsEarned = MPS2MPH(KPH2MPS(pointsEarned));
        }

        char bonusTime[32];
        Timer bt(GRaceStatus::Get().GetRaceTimeRemaining());
        bt.PrintToString(bonusTime, 0);

        char time[32];
        Timer t(racerInfo.GetRaceTime());
        t.PrintToString(time, 0);

        char timeAndSpeed[64];
        // "time @ speed units"
        bSNPrintf(timeAndSpeed, sizeof(timeAndSpeed), "%s %s %$0.0f %s", time, GetTranslatedString(0x474), speed, GetTranslatedString(speedUnits));

        int cashHash;

        if (FEngIsScriptSet(GetPackageName(), bStringHash("TOLL_BOOTH_GROUP"), FEHASH_APPEAR)) {
            FEPrintf(GetPackageName(), 0x8BB39726, "%$0.0f %s", speed, GetTranslatedString(speedUnits));
            FEPrintf(GetPackageName(), 0x424BB244, "%s", timeAndSpeed);
            FEPrintf(GetPackageName(), 0x8A7F929C, "+%s", bonusTime);
            cashHash = 0x42423E94;
        } else if (FEngIsScriptSet(GetPackageName(), bStringHash("RIVAL_GROUP"), FEHASH_APPEAR)) {
            if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
                FEPrintf(GetPackageName(), 0x37BEA03B, "%s: %$0.0f %s", GetTranslatedString(0x7F54569D), pointsEarned,
                         GetTranslatedString(speedUnits));
            } else {
                FEPrintf(GetPackageName(), 0x37BEA03B, "%s", timeAndSpeed);
            }
            cashHash = 0x9F4DF5BB;
        } else {
            if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
                FEPrintf(GetPackageName(), 0xAB6AAFDD, "%s: %$0.0f %s", GetTranslatedString(0x7F54569D), pointsEarned,
                         GetTranslatedString(speedUnits));
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

    if (GRaceStatus::Get().GetRaceParameters() != nullptr &&
        GRaceStatus::Get().GetRaceParameters()->GetEventHash() == Attrib::StringHash32("19.8.31")) {
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417B2601, 0x1FAB5998, 0x4C54B7EA);
        FEDatabase->GetCareerSettings()->SetAwardedBKReward();
    }
}

MenuScreen *PhotoFinishScreen::Create(ScreenConstructorData *sd) {
    return new ("PhotoFinishScreen", 0) PhotoFinishScreen(sd);
}
