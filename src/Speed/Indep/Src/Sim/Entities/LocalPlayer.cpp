#include "LocalPlayer.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/Generated/Events/EPursuitBreaker.hpp"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/SimEntity.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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
#ifndef EA_BUILD_A124
      ,
      mLastPursuit(nullptr)
#endif
{
    IEntity::AddToList(ENTITY_PLAYERS);
    IPlayer::AddToList(PLAYER_LOCAL);
    IPlayer::AddToList(PLAYER_ALL);
    mSpeech = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance("SoundAI", Sim::Param());
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
        mHud = ::new (__FILE__, __LINE__) FEngHud(ht, hud_name, this, mSettingIndex);
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
    IVehicle *ivehicle;
    if (!isimable || !INIS::Exists() || !isimable->QueryInterface(&ivehicle)) {
        return false;
    }
    float speed_mph = MPS2MPH(ivehicle->GetSpeedometer());
    if (speed_mph < 30.0f) {
        return false;
    }
    if (!ivehicle->IsAnimating() && !ivehicle->IsStaging() && !ivehicle->IsLoading()) {
        return true;
    }
    return false;
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

// UNSOLVED
void LocalPlayer::UpdateNeighbourhood() {
    bVector3 v;
    bConvertFromBond(v, static_cast<IEntity *>(this)->GetPosition());
    TrackPathZone *zone = TheTrackPathManager.FindZone(reinterpret_cast<bVector2 *>(&v), TRACK_PATH_ZONE_NEIGHBOURHOOD, nullptr);
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
                igenericmessage->RequestGenericMessage(stringToUse, false, 0xa19bb14c, 0, 0, GenericMessage_Priority_5);
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
        bool front = IsFront(i);
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
    ProfileNode profile_node("TODO", 0);

    if (htask == mHudTask) {
        float dT_real = 0.0f;
        float sim_speed = Sim::GetSpeed();
        if (sim_speed > FLOAT_EPSILON) {
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
