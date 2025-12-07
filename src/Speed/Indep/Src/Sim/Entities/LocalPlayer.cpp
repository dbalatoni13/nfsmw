#include "LocalPlayer.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/Events/EPursuitBreaker.hpp"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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
        // TODO is this at the right place?
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
