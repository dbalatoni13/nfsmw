#include "LocalPlayer.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GSpeedTrap.h"
#include "Speed/Indep/Src/Gameplay/GTrigger.h"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/Generated/Events/EPursuitBreaker.hpp"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"



#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICarAudio.h"
#include "Speed/Indep/Src/Interfaces/Simables/IInductable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/SimEntity.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

Sim::IEntity::Prototype _LocalPlayer(UCrc32("LocalPlayer"), LocalPlayer::Construct);

LocalPlayer::LocalPlayer(Sim::Param params)
    : IPlayer(this),            //
      mSettingIndex(-1),        //
      mName(""),                //
      mGameBreakerCharge(1.0f), //
      mFFB(nullptr),            //
      mWheelDevice(nullptr),    //
      mRenderPort(-1),          //
      mControllerPort(-1),      //
      mNeighbourhoodHash(0),    //
      mHud(nullptr),            //
      mHudTask(nullptr),        //
      mSpeech(nullptr),         //
      mInGameBreaker(false)
// La guarda tiene que ser la MISMA que la del miembro en LocalPlayer.hpp:139, que es
// `#ifndef EA_PLATFORM_PLAYSTATION2`. Con `EA_BUILD_A124` colaba de casualidad, porque
// el prototipo era la unica build de PS2 que lo definia; en cuanto SLUS dejo de
// definirlo, el inicializador quedo vivo sobre un campo que en PS2 no existe.
#ifndef EA_PLATFORM_PLAYSTATION2
      ,
      mLastPursuit(nullptr)
#endif
{
    IEntity::AddToList(ENTITY_PLAYERS);
    IPlayer::AddToList(PLAYER_LOCAL);
    IPlayer::AddToList(PLAYER_ALL);
    mSpeech = Sim::IActivity::CreateInstance("SoundAI", Sim::Param());
    if (mSpeech) {
        Attach(mSpeech);
    }
    mHudTask = AddTask("WorldUpdate", 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mHudTask, "Hud");
}

void LocalPlayer::ReleaseHud() {
    if (mHud) {
        mHud->Release();
        mHud = nullptr;
    }
}

PlayerSettings *LocalPlayer::GetSettings() const {
    if (mSettingIndex >= 0) {
        return FEDatabase->GetPlayerSettings(mSettingIndex);
    }
    return nullptr;
}

void LocalPlayer::SetHud(ePlayerHudType ht) {
    if (ht == PHT_NONE) {
        ReleaseHud();
        return;
    } else if (mSettingIndex < 0) {
        return;
    }
    const char *hud_name = HudResourceManager::GetHudFengName(ht);
    if (!hud_name) {
        ReleaseHud();
    } else {
        ReleaseHud();
        mHud = ::new ("FEngHUD", __LINE__) FEngHud(ht, hud_name, this, mSettingIndex);
    }
}

void LocalPlayer::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        if (mSpeech) {
            mSpeech->Attach(ivehicle);
        }
        Sim::Collision::AddListener(this, ivehicle, "LocalPlayer");
    }
    Sim::Entity::OnAttached(pOther);
}

void LocalPlayer::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        if (mSpeech) {
            mSpeech->Detach(ivehicle);
        }
        Sim::Collision::RemoveListener(this, ivehicle);
        SetGameBreaker(false);
    }
    if (UTL::COM::ComparePtr(mSpeech, pOther)) {
        mSpeech = nullptr;
    }
    Sim::Entity::OnDetached(pOther);
}

LocalPlayer::~LocalPlayer() {
    if (mFFB) {
        mFFB->ResetEffects();
        mFFB = nullptr;
    }
    SetGameBreaker(false);
    if (mHudTask) {
        Sim::Object::RemoveTask(mHudTask);
    }
    ReleaseHud();
    if (mSpeech) {
        mSpeech->Release();
    }
    Sim::Collision::RemoveListener(this);
}

void LocalPlayer::SetGameBreaker(bool on) {
    if (on != mInGameBreaker) {
        new EPursuitBreaker(on ? 1 : 0);
        mInGameBreaker = on;
    }
}

// UNSOLVED
bool LocalPlayer::CanDoGameBreaker() {
    if (Sim::GetUserMode() != Sim::USER_SINGLE) {
        return false;
    }
    ISimable *isimable = static_cast<IEntity *>(this)->GetSimable();
    if (!isimable) {
        return false;
    }

    if (INIS::Exists()) {
        return false;
    }

    IVehicle *ivehicle;
    if (!isimable->QueryInterface(&ivehicle)) {
        return false;
    }

    float speed_mph = MPS2MPH(ivehicle->GetSpeedometer());
    if (speed_mph < 30.0f) {
        return false;
    }

    if (ivehicle->IsAnimating() || ivehicle->IsStaging() || ivehicle->IsLoading()) {
        return false;
    }
    return true;
}

bool LocalPlayer::ToggleGameBreaker() {
    if (!CanDoGameBreaker() && !mInGameBreaker) {
        return false;
    }
    if (mInGameBreaker) {
        SetGameBreaker(false);
    } else if (mGameBreakerCharge > 0.0f) {
        SetGameBreaker(true);
    } else {
        return false;
    }
    return true;
}

bool LocalPlayer::CanRechargeNOS() const {
    return mInGameBreaker == 0;
}

void LocalPlayer::ResetGameBreaker(bool full) {
    mGameBreakerCharge = full ? 1.0f : 0.0f;
    SetGameBreaker(false);
}

float Tweak_GameBreakerRechargeTime = 25.0f;
float Tweak_GameBreakerRechargeSpeed = 100.0f;
bool Tweak_InfiniteRaceBreaker = false;
// static const float Tweak_GameBreakerMinimumSpeedMPH; // TODO
float Tweak_GameBreakerCollisionMass = 2.0f;

// TODO move
extern int bRumbleEnabled;

void LocalPlayer::DoGameBreaker(float dT, float dT_real) {
    if (!CanDoGameBreaker()) {
        SetGameBreaker(false);
        return;
    }
    ISimable *isimable = static_cast<Entity *>(this)->GetSimable();
    IVehicle *ivehicle;
    if (!isimable || !isimable->QueryInterface(&ivehicle)) {
        SetGameBreaker(false);
        return;
    }

    float speed_mph = MPS2MPH(ivehicle->GetSpeedometer());
    if (mInGameBreaker) {
        if (!Tweak_InfiniteRaceBreaker) {
            mGameBreakerCharge = mGameBreakerCharge - dT_real * 0.1f;
            mGameBreakerCharge = UMath::Max(mGameBreakerCharge, 0.0f);
        }
    } else {
        if (speed_mph > Tweak_GameBreakerRechargeSpeed) {
            mGameBreakerCharge = mGameBreakerCharge + dT / Tweak_GameBreakerRechargeTime;
            mGameBreakerCharge = UMath::Min(mGameBreakerCharge, 1.0f);
        }
    }
    if (mGameBreakerCharge <= 0.0f) {
        SetGameBreaker(false);
    }
}

void LocalPlayer::UpdateHud(float dT) {
    if (!mHud) {
        return;
    }

    ProfileNode profile_node;

    ISimable *isimable = static_cast<IEntity *>(this)->GetSimable();

    if (!isimable) {
        return;
    }

    IVehicle *ivehicle;
    if (!isimable->QueryInterface(&ivehicle)) {
        return;
    }

    IHumanAI *ihumanai;
    if (!isimable->QueryInterface(&ihumanai)) {
        return;
    }

    ISuspension *isuspension;
    if (!isimable->QueryInterface(&isuspension)) {
        return;
    }

    IEngine *iengine;
    if (!isimable->QueryInterface(&iengine)) {
        return;
    }

    IDragEngine *idragengine;
    isimable->QueryInterface(&idragengine);

    IEngineDamage *ienginedamage;
    isimable->QueryInterface(&ienginedamage);

    IInductable *iinductable;
    isimable->QueryInterface(&iinductable);

    ITransmission *itransmission;
    if (!isimable->QueryInterface(&itransmission)) {
        return;
    }

    IVehicleAI *ivehicleai = ivehicle->GetAIVehiclePtr();
    IPursuit *ipursuit = ivehicleai->GetPursuit();

    IPerpetrator *iperpvehicle;
    ivehicle->QueryInterface(&iperpvehicle);

    ISpeedometer *ispeedometer;
    if (mHud->QueryInterface(&ispeedometer)) {

        ispeedometer->SetSpeed(ivehicle->GetSpeed());
    }

    ITachometer *itach;
    if (mHud->QueryInterface(&itach)) {

        bool is_staging = ivehicle->IsStaging();
        float rpm = iengine->GetRPM();

        ICarAudio *iaudible;
        if (!is_staging && ivehicle->GetDriverStyle() != STYLE_DRAG && isimable->QueryInterface(&iaudible)) {
            rpm = iaudible->GetRPM();
        }

        itach->SetRpm(rpm);
        itach->SetRevLimiter(iengine->GetRedline(), iengine->GetMaxRPM());
        itach->SetGear(itransmission->GetGear(), itransmission->GetShiftPotential(), true);
        itach->SetShifting(itransmission->IsGearChanging());

        itach->SetInPerfectLaunchRange(false);
        if (ivehicle->IsStaging()) {

            IRaceEngine *raceEngine;
            if (iengine && iengine->QueryInterface(&raceEngine)) {

                float range = 0.0f;
                float peak_rpm = raceEngine->GetPerfectLaunchRange(range);
                if (range > 0.0f && peak_rpm > 0.0f) {

                    if (rpm >= peak_rpm && rpm <= peak_rpm + range) {
                        itach->SetInPerfectLaunchRange(true);
                    }
                }
            }
        }
    }

    IShiftUpdater *ishiftupdater;
    if (mHud->QueryInterface(&ishiftupdater)) {

        bool hasGoodEnoughTraction = !ivehicle->IsStaging();

        ishiftupdater->SetGear(itransmission->GetGear(), itransmission->GetShiftStatus(), itransmission->GetShiftPotential(), hasGoodEnoughTraction);

        if (idragengine) {

            ishiftupdater->SetEngineTemp(idragengine->GetHeat());
        }
        if (ienginedamage) {

            ishiftupdater->SetEngineBlown(ienginedamage->IsBlown());
        }
    }

    if (iinductable) {

        ITurbometer *iturbometer;
        mHud->SetHasTurbo(iinductable->InductionType() != Physics::Info::INDUCTION_NONE);

        if (mHud->QueryInterface(&iturbometer)) {

            iturbometer->SetInductionPsi(iinductable->GetInductionPSI());
        }
    }

    if (idragengine) {

        IEngineTempGauge *ienginetempgauge;
        if (mHud->QueryInterface(&ienginetempgauge)) {

            ienginetempgauge->SetEngineTemp(idragengine->GetHeat());
        }
    }

    INos *inos;
    if (mHud->QueryInterface(&inos)) {

        float noscap = iengine->GetNOSCapacity();

        inos->SetNos(noscap);
    }

    ISpeedBreakerMeter *ispeedbreakermeter;
    if (mHud->QueryInterface(&ispeedbreakermeter)) {

        ispeedbreakermeter->SetPursuitLevel(mGameBreakerCharge);
    }

    IGetAwayMeter *igetawaydistance;
    mHud->QueryInterface(&igetawaydistance);

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {

        IPursuitBoard *ipursuitboard;
        if (mHud->QueryInterface(&ipursuitboard)) {

            mHud->SetInPursuit(ipursuit && !ipursuit->IsPursuitBailed());
            ipursuitboard->SetInPursuit(ipursuit && !ipursuit->IsPursuitBailed());

            if (ipursuit) {

                ipursuitboard->SetIsInView(ipursuit->IsPerpInSight());
                ipursuitboard->SetPursuitDuration(ipursuit->GetPursuitDuration());
                ipursuitboard->SetCooldownTimeRemaining(ipursuit->GetCoolDownTimeRemaining());
                ipursuitboard->SetCooldownTimeRequired(ipursuit->GetCoolDownTimeRequired());
                ipursuitboard->SetNumCopsInPursuit(ipursuit->GetNumCopsRemainingInWave());
                ipursuitboard->SetNumCopsDestroyed(ipursuit->GetNumCopsDestroyed(), ipursuit->GetMostRecentCopDestroyedType(),
                                                   ipursuit->GetCopDestroyedBonusMultiplier(), ipursuit->GetMostRecentCopDestroyedRepPoints());
                ipursuitboard->SetNumCopsDamaged(ipursuit->GetNumCopsDamaged());
                ipursuitboard->SetTotalNumCopsInvolved(ipursuit->GetTotalNumCopsInvolved());
                ipursuitboard->SetHeliInvolvedInPursuit(ipursuit->IsHeliInPursuit());
                ipursuitboard->SetTimeUntilBusted(ipursuit->TimeUntilBusted(), ipursuit->IsPerpBusted());
                ipursuitboard->SetTimeUntilBackup(ipursuit->GetTimeToBackupSpawned());

                if (iperpvehicle) {

                    float timeUntilHidden = 0.0f;
                    ipursuitboard->SetIsHiding(iperpvehicle->IsPartiallyHidden(timeUntilHidden));
                    ipursuitboard->SetTimeUntilHidden(timeUntilHidden);
                    ipursuitboard->SetPursuitRep(iperpvehicle->GetPendingRepPointsNormal() + iperpvehicle->GetPendingRepPointsFromCopDestruction());
                }
            }
        }
    }

    IMilestoneBoard *imilestoneboard;
    if (mHud->QueryInterface(&imilestoneboard)) {

        if (GRaceStatus::Exists()) {

            GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();

            if (raceParams && raceParams->GetIsPursuitRace() && !FEDatabase->IsFinalEpicChase()) {

                int numMilestones = 1;
                int milestoneboardIndex = 0;

                imilestoneboard->SetNumberOfMilestones(numMilestones);
                imilestoneboard->SetChallengeSeries(true);

                imilestoneboard->SetMilestoneIconHash(milestoneboardIndex, FEDatabase->GetMilestoneIconHash(raceParams->GetChallengeType(), true));
                imilestoneboard->SetMilestoneType(milestoneboardIndex, raceParams->GetChallengeType());
                imilestoneboard->SetMilestoneGoal(milestoneboardIndex, raceParams->GetChallengeGoal());

                imilestoneboard->SetMilestoneCurrValue(milestoneboardIndex, GManager::Get().GetValue(raceParams->GetChallengeType()));
                imilestoneboard->SetMilestoneHeaderHash(milestoneboardIndex, raceParams->GetLocalizationTag());

                float currVal = GManager::Get().GetValue(raceParams->GetChallengeType());
                float goalVal = raceParams->GetChallengeGoal();

                imilestoneboard->SetMilestoneComplete(milestoneboardIndex, currVal >= goalVal);

            } else if (ipursuit && !ipursuit->IsPursuitBailed()) {

                int currBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
                GMilestone *currMilestone = GManager::Get().GetFirstMilestone(false, currBin);
                unsigned char index = 0;
                unsigned char numIncompleteMilestones = 0;

                while (currMilestone) {

                    bool isDone = false;

                    imilestoneboard->SetMilestoneIconHash(index, FEDatabase->GetMilestoneIconHash(currMilestone->GetTypeKey(), true));
                    imilestoneboard->SetMilestoneType(index, currMilestone->GetTypeKey());
                    imilestoneboard->SetMilestoneGoal(index, currMilestone->GetRequiredValue());
                    imilestoneboard->SetMilestoneCurrValue(index, currMilestone->GetCurrentValue());
                    imilestoneboard->SetMilestoneHeaderHash(index, currMilestone->GetLocalizationTag());

                    if (currMilestone->GetIsDonePendingEscape() || currMilestone->GetIsAwarded()) {
                        isDone = true;
                    }
                    imilestoneboard->SetMilestoneComplete(index, isDone);

                    if (!isDone) {
                        numIncompleteMilestones++;
                    }

                    currMilestone = GManager::Get().GetNextMilestone(currMilestone, false, currBin);
                    index++;
                }

                imilestoneboard->SetNumberOfMilestones(index);
                imilestoneboard->SetChallengeSeries(false);
            }
        }

        imilestoneboard->SetInPursuit(ipursuit && !ipursuit->IsPursuitBailed());
    }

    IBustedMeter *ibustedmeter;
    if (mHud->QueryInterface(&ibustedmeter)) {

        ibustedmeter->SetInPursuit(ipursuit && !ipursuit->IsPursuitBailed());

        if (ipursuit) {

            ibustedmeter->SetTimeUntilBusted(ipursuit->TimeUntilBusted());
            ibustedmeter->SetIsBusted(ipursuit->IsPerpBusted());

            if (iperpvehicle) {

                float timeUntilHidden = 0.0f;
                ibustedmeter->SetIsHiding(iperpvehicle->IsPartiallyHidden(timeUntilHidden));
            }
        }
    }

    if (GRaceStatus::IsTollboothRace()) {

        ITimeExtension *itimeextension;
        if (mHud->QueryInterface(&itimeextension)) {

            itimeextension->SetPlayerLapTime(GRaceStatus::Get().GetRaceTimeRemaining());
        }
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {

        IRaceInformation *iraceinformation;
        if (mHud->QueryInterface(&iraceinformation)) {

            GRaceStatus &raceStatus = GRaceStatus::Get();
            GRacerInfo *playerInfo;

            iraceinformation->SetNumRacers(raceStatus.GetRacerCount());
            iraceinformation->SetNumLaps(raceStatus.GetRaceParameters()->GetNumLaps());

            playerInfo = raceStatus.GetRacerInfo(static_cast<IEntity *>(this)->GetSimable());

            if (playerInfo) {

                iraceinformation->SetPlayerPosition(playerInfo->GetRanking());
                iraceinformation->SetPlayerLapNumber(playerInfo->GetLapsCompleted() + 1);

                switch (raceStatus.GetRaceType()) {
                case GRace::kRaceType_Checkpoint:
                case GRace::kRaceType_CashGrab: {
                    float remaining = raceStatus.GetRaceTimeRemaining();
                    float race_time = raceStatus.GetRaceParameters()->GetTimeLimit();
                    float pcnt = 1.0f - remaining / race_time;
                    iraceinformation->SetPlayerPercentComplete(pcnt * 100.0f);
                    break;
                }

                default:
                    iraceinformation->SetPlayerPercentComplete(playerInfo->GetHudPctRaceComplete());
                    break;
                }

                if (raceStatus.GetIsTimeLimited()) {

                    iraceinformation->SetPlayerLapTime(raceStatus.GetRaceTimeRemaining());
                    iraceinformation->SetSuddenDeathMode(raceStatus.GetIsSuddenDeath());
                } else {

                    iraceinformation->SetPlayerLapTime(playerInfo->GetRaceTime());
                    iraceinformation->SetSuddenDeathMode(false);
                }

                if (GRaceStatus::IsTollboothRace()) {

                    iraceinformation->SetPlayerTollboothsCrossed(playerInfo->IsFinishedRacing() ? playerInfo->GetTollboothsCrossed() + 1
                                                                                                : playerInfo->GetTollboothsCrossed());
                    iraceinformation->SetNumTollbooths(raceStatus.GetNumRaceTollbooths() + 1);
                }
            }
        }
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {

        ILeaderBoard *ileaderboard;
        if (mHud->QueryInterface(&ileaderboard)) {

            GRaceStatus &raceStatus = GRaceStatus::Get();
            GRacerInfo *playerInfo;

            ileaderboard->SetNumRacers(raceStatus.GetRacerCount());
            ileaderboard->SetNumLaps(raceStatus.GetRaceParameters()->GetNumLaps());

            playerInfo = GRaceStatus::Get().GetRacerInfo(static_cast<IEntity *>(this)->GetSimable());

            for (int i = 0; i < 4; i++) {

                int ranking = i + 1;
                int racerIndex = -1;

                for (int onRacer = 0; onRacer < raceStatus.GetRacerCount(); onRacer++) {

                    if (raceStatus.GetRacerInfo(onRacer).GetRanking() == ranking) {

                        racerIndex = onRacer;
                        break;
                    }
                }

                ileaderboard->SetRacerHasHeadset(i, false);

                if (racerIndex >= 0) {

                    GRacerInfo &racer_info = raceStatus.GetRacerInfo(racerIndex);

                    if (&racer_info == playerInfo) {
                        ileaderboard->SetPlayerIndex(i);
                    }

                    ileaderboard->SetRacerName(i, racer_info.GetName());
                    ileaderboard->SetRacerNum(i, racer_info.GetRanking());
                    ileaderboard->SetRacerTotalPoints(i, racer_info.GetPointTotal());
                    ileaderboard->SetRacerPercentComplete(i, racer_info.GetHudPctRaceComplete(), racer_info.GetRaceTime(), this);
                    ileaderboard->SetRacerNumLapsCompleted(i, racer_info.GetLapsCompleted(), racer_info.GetLapTime(), this);
                    ileaderboard->SetRacerIsBusted(i, racer_info.GetIsBusted());
                    ileaderboard->SetRacerIsKoed(i, racer_info.GetIsKnockedOut());
                }
            }
        }
    }

    ICostToState *icts;
    if (mHud->QueryInterface(&icts)) {

        if (ipursuit && !ipursuit->IsPursuitBailed()) {

            icts->SetInPursuit(true);

            Sim::IActivity *pursuit_activity = UTL::COM::QueryInterface<Sim::IActivity>(ipursuit);

#ifndef EA_PLATFORM_PLAYSTATION2
            if (pursuit_activity && pursuit_activity->GetInstanceHandle() != mLastPursuit) {

                icts->SetCostToState(0);
                mLastPursuit = pursuit_activity->GetInstanceHandle();
            }
#endif

            icts->SetCostToState(ipursuit->CalcTotalCostToState());
        } else {

            icts->SetInPursuit(false);
        }
    }

    IWrongWay *iwrongway;
    if (ihumanai && mHud->QueryInterface(&iwrongway)) {

        if (ivehicleai) {
            iwrongway->SetWrongWay(ihumanai->IsFacingWrongWay());
        }
    }

    IHeatMeter *iheat;
    if (mHud->QueryInterface(&iheat)) {

        if (iperpvehicle) {

            float vehicleHeat = iperpvehicle->GetHeat();

            iheat->SetVehicleHeat(vehicleHeat);
            GManager::Get().TrackValue("heat_meter", vehicleHeat);
        }
    }

    DoRadar(ipursuit && !ipursuit->IsPursuitBailed(), ipursuit && ipursuit->GetPursuitStatus() == PS_COOL_DOWN);

    if (mHud) {
        mHud->Update(this, dT);
    }

    if (ipursuit && !ipursuit->IsPursuitBailed() && ipursuit->GetMinDistanceToTarget() < 25.0f) {

        if (ivehicle) {

            float speed = ivehicle->GetSpeed();

            bool reckless = speed > GInfractionManager::Get().GetRecklessSpeedThreshold();
            bool speeding = speed > GInfractionManager::Get().GetSpeedLimit();
            bool racing = speed > GInfractionManager::Get().GetRacingSpeedLimit();

            GInfractionManager::Get().ReportRacing(racing);
            GInfractionManager::Get().ReportRecklessDriving(reckless);
            GInfractionManager::Get().ReportSpeeding(speeding);
        }
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
        UpdateNeighbourhood();
    }

    PostRacePursuitScreen::GetPursuitData().PopulateData(ipursuit, iperpvehicle, -1);
}

void LocalPlayer::DoRadar(bool inPursuit, bool isCoolingDown) {
    if (!GRaceStatus::Exists()) {
        return;
    }

    IRadarDetector *iradar;
    if (!mHud->QueryInterface(&iradar)) {
        return;
    }
    if (!iradar) {
        return;
    }
    iradar->SetInPursuit(inPursuit);
    iradar->SetIsCoolingDown(isCoolingDown);

    ISimable *isimable = static_cast<IEntity *>(this)->GetSimable();

    if (!isimable) {
        iradar->SetTarget(IRadarDetector::RADAR_TARGET_NONE, 0.0f, 0.0f);
        return;
    }

    IVehicle *ivehicle;
    if (!isimable->QueryInterface(&ivehicle)) {
        iradar->SetTarget(IRadarDetector::RADAR_TARGET_NONE, 0.0f, 0.0f);
        return;
    }

    IRigidBody *ibody;
    if (!ivehicle->QueryInterface(&ibody)) {
        iradar->SetTarget(IRadarDetector::RADAR_TARGET_NONE, 0.0f, 0.0f);
        return;
    }

    IPerpetrator *perp;
    if (!ivehicle->QueryInterface(&perp)) {
        return;
    }

    float radarRange = 300.0f;
    float angleRange = UMath::Cosr(0.7853982f);

    UMath::Vector3 playerDir;
    ibody->GetForwardVector(playerDir);
    UMath::Normalize(playerDir);

    UMath::Vector3 playerPos = ibody->GetPosition();

    IRadarDetector::RadarTarget targetType = IRadarDetector::RADAR_TARGET_NONE;

    float targetRange = radarRange + 10.0f;

    float targetDot = 1.0f;

    UMath::Vector3 targetPos;
    UMath::Vector3 toCurrentTarget;
    UMath::Vector3 toTarget;

    GRaceStatus &race = GRaceStatus::Get();

    if (race.GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
        unsigned int num_traps = GManager::Get().GetNumSpeedTraps();

        for (unsigned int i = 0; i < num_traps; i++) {
            GSpeedTrap *speedTrap = GManager::Get().GetSpeedTrap(i);
            if (speedTrap->GetIsActive()) {
                GTrigger *trap = speedTrap->GetTrapTrigger();

                if (trap && trap->IsWorldTriggerEnabled()) {
                    trap->GetPosition(targetPos);
                    UMath::Sub(targetPos, playerPos, toCurrentTarget);
                    float currentRange = UMath::Normalize(toCurrentTarget);

                    if (currentRange <= radarRange) {
                        float dot = UMath::Dot(playerDir, toCurrentTarget);

                        if (dot >= angleRange && currentRange < targetRange) {
                            targetRange = currentRange;
                            targetDot = dot;
                            targetType = IRadarDetector::RADAR_TARGET_CAMERA;
                            toTarget = toCurrentTarget;
                        }
                    }
                }
            }
        }
    } else if (race.GetRaceType() == GRace::kRaceType_SpeedTrap) {
        GRacerInfo *racer = perp->GetRacerInfo();
        if (racer) {
            unsigned int num_traps = race.GetNumRaceSpeedTraps();

            for (unsigned int i = 0; i < num_traps; i++) {
                GTrigger *trap = race.GetRaceSpeedTrap(i);

                if (trap && trap->IsWorldTriggerEnabled()) {
                    trap->GetPosition(targetPos);
                    UMath::Sub(targetPos, playerPos, toCurrentTarget);
                    float currentRange = UMath::Normalize(toCurrentTarget);

                    if (currentRange <= radarRange) {
                        float dot = UMath::Dot(playerDir, toCurrentTarget);

                        if (dot >= angleRange && currentRange < targetRange) {
                            targetRange = currentRange;
                            targetDot = dot;
                            targetType = IRadarDetector::RADAR_TARGET_CAMERA;
                            toTarget = toCurrentTarget;
                        }
                    }
                }
            }
        }
    }

    for (IVehicle *const *iter = IVehicle::GetList(VEHICLE_AICOPS).begin(); iter != IVehicle::GetList(VEHICLE_AICOPS).end(); ++iter) {
        IVehicle *iv = *iter;
        if (iv->GetVehicleClass() == VehicleClass::CHOPPER || !iv->IsActive() || iv->IsAnimating() || iv->IsDestroyed()) {
            continue;
        }
        const UMath::Vector3 &copPos = iv->GetPosition();
        UMath::Sub(copPos, playerPos, toCurrentTarget);
        float currentRange = UMath::Normalize(toCurrentTarget);

        if (currentRange > radarRange) {
            continue;
        }
        float dot = UMath::Dot(playerDir, toCurrentTarget);

        if (dot < angleRange || currentRange >= targetRange) {
            continue;
        }
        targetRange = currentRange;
        targetDot = dot;
        targetType = IRadarDetector::RADAR_TARGET_COP;
        toTarget = toCurrentTarget;
    }

    if (targetType != IRadarDetector::RADAR_TARGET_NONE) {
        float targetAngle = ANGLE2RAD(VU0_ACos(UMath::Clamp(targetDot, -1.0f, 1.0f)));
        UMath::Vector3 right;
        ibody->GetRightVector(right);
        if (UMath::Dot(toTarget, right) < 0.0f) {
            targetAngle = -targetAngle;
        }
        targetRange = UMath::Clamp(targetRange / radarRange, 0.0f, 1.0f);
        iradar->SetTarget(targetType, targetRange, targetAngle);
    } else {
        iradar->SetTarget(IRadarDetector::RADAR_TARGET_NONE, 0.0f, 0.0f);
    }
}

// UNSOLVED
void LocalPlayer::UpdateNeighbourhood() {
    bVector3 v;
    TrackPathZone *zone = TheTrackPathManager.FindZone(
        (bConvertFromBond(v, static_cast<IEntity *>(this)->GetPosition()), reinterpret_cast<bVector2 *>(&v)), TRACK_PATH_ZONE_NEIGHBOURHOOD, nullptr);
    unsigned int neighbourhood_hash = 0;
    if (zone) {
        neighbourhood_hash = zone->GetData(0);
    }
    if (neighbourhood_hash != mNeighbourhoodHash) {
        if (mHud) {
            IGenericMessage *igenericmessage;
            if (mHud->QueryInterface(&igenericmessage) && neighbourhood_hash != 0) {
                char *stringToUse = GetTranslatedString(neighbourhood_hash);
                // TODO hash
                igenericmessage->RequestGenericMessage(stringToUse, false, FEHASH_ZOOMINGREEN, 0, 0, GenericMessage_Priority_5);
            }
        }
        mNeighbourhoodHash = neighbourhood_hash;
    }
}

bool LocalPlayer::CanDoFFB() const {
    PlayerSettings *settings = GetSettings();
    if (!settings || !settings->Rumble || !bRumbleEnabled) {
        return false;
    }
    Sim::IStateManager *state_manager = UTL::COM::QueryInterface<Sim::IStateManager>(IGameState::Get());
    if (!state_manager || state_manager->ShouldPauseInput()) {
        return false;
    }
    ISimable *myobject = static_cast<const IEntity *>(this)->GetSimable();
    IHumanAI *ai;
    IVehicle *vehicle;
    if (myobject && myobject->QueryInterface(&ai)) {
        if (ai->GetAiControl()) {
            return false;
        }
        if (myobject->QueryInterface(&vehicle)) {
            return vehicle->IsAnimating() == 0;
        }
    }
    return false;
}

void LocalPlayer::DoFFB() {
    if (!mFFB) {
        return;
    }
    if (!CanDoFFB()) {
        mFFB->PauseEffects();
        return;
    }
    if (mWheelDevice && mWheelDevice->IsConnected()) {
        mWheelDevice->UpdateForces(this);
        return;
    }

    ISimable *isimable = static_cast<IEntity *>(this)->GetSimable();

    IInput *iinput;
    IVehicle *ivehicle;
    ISuspension *isuspension;
    IEngine *iengine;
    ITransmission *itransmission;

    if (!isimable || !isimable->QueryInterface(&iinput) || !isimable->QueryInterface(&ivehicle) || !isimable->QueryInterface(&isuspension) ||
        !isimable->QueryInterface(&iengine) || !isimable->QueryInterface(&itransmission)) {
        return;
    }
    mFFB->ResumeEffects();
    mFFB->BeginUpdate();
    for (unsigned int i = 0; i < isuspension->GetNumWheels(); i++) {
        ISpikeable *ispikeable;
        bool blown = isimable->QueryInterface(&ispikeable) && ispikeable->GetTireDamage(i);

        // TODO hash
        const SimSurface &surface = isuspension->IsWheelOnGround(i) ? (blown ? SimSurface(0xd929e923) : isuspension->GetWheelRoadSurface(i))
                                                                    : (SimSurface(SimSurface::kNull));

        float slip = UMath::Abs(isuspension->GetWheelSlip(i));
        float skid = UMath::Abs(isuspension->GetWheelSkid(i));
        bool front = Physics::Wheels::IsFront(i);
        mFFB->UpdateTireSlip(front, surface, slip);
        mFFB->UpdateTireSkid(front, surface, skid);
        mFFB->UpdateRoadNoise(front, surface, ivehicle->GetAbsoluteSpeed());
    }

    float throttle = iinput->GetControls().fGas;
    float minrpm = iengine->GetMinRPM();
    float rpm = iengine->GetRPM();
    float range = iengine->GetRedline() - minrpm;
    float powerband = 0.0f;
    float overrev = 0.0f;
    if (range > 0.0f) {
        powerband = UMath::Clamp((rpm - minrpm) / range, 0.0f, 1.0f);
    }
    mFFB->UpdateRPM(powerband, overrev, throttle);
    mFFB->UpdateShiftPotential(itransmission->GetShiftPotential());

    if (iengine->HasNOS()) {
        mFFB->UpdateNOS(iengine->IsNOSEngaged(), iengine->GetNOSCapacity());
    }

    IEngineDamage *ienginedamage;
    isimable->QueryInterface(&ienginedamage);
    if (ienginedamage) {
        mFFB->UpdateEngineBlown(ienginedamage->IsBlown() || ienginedamage->IsSabotaged());
    } else {
        mFFB->UpdateShifting(itransmission->IsGearChanging() && (ivehicle->GetSpeed() > 0.1f));
    }

    mFFB->EndUpdate();
}

bool LocalPlayer::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node;

    if (htask == mHudTask) {
        float dT_real = 0.0f;
        float sim_speed = Sim::GetSpeed();
        if (sim_speed > UMath::Epsilon) {
            dT_real = dT / sim_speed;
        }
        UpdateHud(dT);
        DoGameBreaker(dT, dT_real);
        DoFFB();
        return true;
    } else {
        Sim::Object::OnTask(htask, dT);
        return false;
    }
}

IFeedback *LocalPlayer::GetFFB() {
    return mFFB;
}

ISteeringWheel *LocalPlayer::GetSteeringDevice() {
    return mWheelDevice;
}

void LocalPlayer::SetControllerPort(int port) {
    if (port != mControllerPort) {
        mControllerPort = port;
        if (mFFB) {
            mFFB->ResetEffects();
            mFFB = nullptr;
        }
        mWheelDevice = nullptr;
        if (mControllerPort >= 0) {
            InputDevice *device = IOModule::GetIOModule().GetDevice(mControllerPort);
            if (device) {
                mFFB = UTL::COM::QueryInterface<IFeedback>(device->GetInterfaces());
                mWheelDevice = UTL::COM::QueryInterface<ISteeringWheel>(device->GetSecondaryDevice());
            }
        }
        if (mFFB) {
            mFFB->ResetEffects();
        }
    }
}

void LocalPlayer::OnCollision(const COLLISION_INFO &cinfo) {
    ISimable *bodyA = ISimable::FindInstance(cinfo.objA);
    if (!bodyA) {
        return;
    }
    ISimable *bodyB;
    if (cinfo.type == COLLISION_INFO::OBJECT) {
        bodyB = ISimable::FindInstance(cinfo.objB);
    }
    PlayerSettings *settings = GetSettings();
    if (mFFB && settings && settings->Rumble && bRumbleEnabled) {
        mFFB->ReportCollision(cinfo, UTL::COM::ComparePtr(bodyA, static_cast<IEntity *>(this)->GetSimable()));
    }
}
