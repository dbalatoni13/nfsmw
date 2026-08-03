#include "OnlineManager.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

ExtrapolatedCar::ExtrapolatedCar(Attrib::Key cartype) {
    mCarType = cartype;
    mCops = nullptr;
    mMutex = static_cast<NetworkMutex *>(gFastMem.Alloc(1, nullptr));
    mLast = mStateArray;
    mActive = true;
    mUpdateTime = 0;
    mTail = 0;
    mHead = 0;
    mCollisionTime = 0.0f;
    mRepositionCount = 0;
    mHasHeadset = false;
    mPaused = false;
    mUseDriverAI = false;
}

ExtrapolatedCar::State::State() {
    mGear = 1;
    mBlendRate = -1.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mSteering = 0.0f;
    mLinearVelocity.x = 0.0f;
    mLinearVelocity.y = 0.0f;
    mLinearVelocity.z = 0.0f;
    mGas = 0.0f;
    mLinearAcceleration.x = 0.0f;
    mLinearAcceleration.y = 0.0f;
    mLinearAcceleration.z = 0.0f;
    mBrake = 0.0f;
    mRotation.x = 0.0f;
    mRotation.y = 0.0f;
    mRotation.z = 0.0f;
    mRotation.w = 1.0f;
    mAngularVelocity.x = 0.0f;
    mAngularVelocity.y = 0.0f;
    mAngularVelocity.z = 0.0f;
    mAngularVelocity.w = 1.0f;
    mAngularAcceleration.x = 0.0f;
    mAngularAcceleration.y = 0.0f;
    mAngularAcceleration.z = 0.0f;
    mAngularAcceleration.w = 1.0f;
    mBlend = 1.0f;
    mHandBrake = 0.0f;
    mTime = 0.0f;
    mInFlight = false;
    mNOS = false;
}

float ExtrapolatedCar::State::SquaredDistanceTo(State &target) const {
    return VU0_v3distancesquare(mPosition, target.mPosition);
}

bool ExtrapolatedCar::State::IsBlending() const { return mBlend > 0.0f; }

void ExtrapolatedCar::ExtractExtrapolatedPosition(UMath::Vector3 &position) const {
    position = mBlended.mPosition;
}

void ExtrapolatedCar::ExtractExtrapolatedDirection(UMath::Vector3 &direction) const {
    mBlended.ExtractDirection(direction);
}

bool ExtrapolatedCar::IsAbleToSee(ExtrapolatedCar &target) {
    float fVar1;

    fVar1 = mBlended.SquaredDistanceTo(target.mBlended);
    return fVar1 < 40000.0f;
}

OnlineRacer::OnlineRacer(int8 driver_number, bool is_server, const char *persona)
    : ExtrapolatedCar(0)
    , CarCustomization()
    , FinishedRaceStats() {
    DriverNumber = driver_number;
    bIsServer = is_server;
    DisconnectTime.SetTime(0.0f);
    RaceScore = 0;
    State = OPS_DISCONNECTED;
    bShouldRestart = false;
    PhysicsDataCRC = 0;
    BadnessReason = 0;
    SyncScoreMsgID = 0;
    BadnessCountdown.SetTime(0.0f);
    GraceCountdown.SetTime(0.0f);
    PlayerID = -1;
    Reputation = 100;
    LastSpamRealTime = -1.0f;
    EndRaceCountdown = -1.0f;
    ClearCheatInfo();
    DisconnectTime.SetTime(0.0f);
    bMemSet(&FinishedRaceStats, 0, 0xc0);
    if (SkipFE) {
        SetPersona("SkipFE");
    } else if (persona) {
        SetPersona(persona);
    } else {
        bMemSet(Persona, 0, 0x10);
    }
}

void OnlineRacer::SetPersona(const char *persona) {
    bMemCpy(Persona, persona, 0x10);
}

void OnlineRacer::SetRaceScore(int score) {
    if (RaceScore != score) {
        RaceScore = score;
    }
}

bool OnlineRacer::IsFinishedRacing() {
    if (IsConnected() && State != OPS_FINISHED) {
        return TheOnlineManager.GetState() > OLS_RACING && State < OPS_RACING;
    }
    return true;
}

void OnlineRacer::ChangeState(eOnlineRacerState new_state) {
    if ((new_state != State) && (State = new_state, new_state == OPS_RACING)) {
        BadnessCountdown.SetTime(20.0f);
        GraceCountdown.SetTime(6.0f);
        BadnessReason = 0;
    }
}

void OnlineRacer::ClearCheatInfo() {
    int i;
    for (i = 15; i > -1; i--) {
        CheatTally[i] = 0;
    }
}

uint8 OnlineRacer::GetCheatScore() { return 0; }

uint32 OnlineRacer::GetDataCRC(bool recalc) { return PhysicsDataCRC; }

float OnlineRacer::GetEndRaceCountdown() { return EndRaceCountdown; }

void OnlineRacer::UpdateEndRaceStats() {}
