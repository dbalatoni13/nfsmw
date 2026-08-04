#include "OnlineManager.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Online/SmartBitstream.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

namespace Online {
    bool IsInitialized();
    void SignalDriverFinish(SmartBitStream &payload_data);
}


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

ExtrapolatedCar::~ExtrapolatedCar() {
    delete mMutex;
    if (mCops) {
        CopMap::iterator iter = mCops->begin();
        while (iter != mCops->end()) {
            if (iter->second) {
                delete iter->second;
            }
            ++iter;
        }
        mCops->clear();
        delete mCops;
    }
}

OnlineRacer::~OnlineRacer() {}

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

bool ExtrapolatedCar::State::IsValidPosition() {
    WCollisionMgr collision_mgr(0, 3);
    float elevation;

    return collision_mgr.GetWorldHeightAtPointRigorous(mPosition, elevation, nullptr);
}

void ExtrapolatedCar::State::SetOnGround(IVehicle *vehicle) {
    UMath::Vector3 forwardvector;

    UMath::ExtractZAxis(mRotation, forwardvector);
    vehicle->SetVehicleOnGround(mPosition, forwardvector);
}

ISimable *ExtrapolatedCar::State::SpawnVehicle(Attrib::Key cartype) {
    UMath::Vector3 forwardvector;

    if (cartype == 0) {
        cartype = Attrib::StringToKey(SkipFEPlayer2Car);
    }
    UMath::ExtractZAxis(mRotation, forwardvector);
    UCrc32 name("PVehicle");
    return ISimable::CreateInstance(
        name, VehicleParams(nullptr, DRIVER_REMOTE, cartype, forwardvector, mPosition, VPF_SNAP_TO_GROUND, nullptr, nullptr));
}

void ExtrapolatedCar::ExtractExtrapolatedPosition(UMath::Vector3 &position) const {
    position = mBlended.mPosition;
}

void ExtrapolatedCar::State::ExtractDirection(UMath::Vector3 &direction) const {
    UMath::ExtractZAxis(mRotation, direction);
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
    if (State == OPS_LOST_CONNECTION || State == OPS_QUIT ||
        State == OPS_DISCONNECTED || State == OPS_DISCERROR || State == OPS_FINISHED) {
        return true;
    }
    return TheOnlineManager.GetState() > OLS_RACING && State < OPS_RACING;
}

void OnlineRacer::ChangeState(eOnlineRacerState new_state) {
    if ((new_state != State) && (State = new_state, new_state == OPS_RACING)) {
        BadnessCountdown.SetTime(20.0f);
        GraceCountdown.SetTime(6.0f);
        BadnessReason = 0;
    }
}

void OnlineRacer::DriverDisconnect(eOnlineRacerState new_state, int finish_reason) {
    if (IsConnected()) {
        ChangeState(new_state);
        FinishedRaceStats.FinishReason = finish_reason;
    } else {
        FinishedRaceStats.FinishReason = finish_reason;
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

void OnlineRacer::Finish(int nRank, bool bBlinkBlinkPoof, int raceFinishReason) {
    SmartBitStream data;
    data.AddByte(DriverNumber);
    data.AddByte(static_cast<uint8>(nRank));
    uint8 value = 0;
    if (bBlinkBlinkPoof || raceFinishReason == 0xb) {
        value = 1;
    }
    data.AddByte(value);
    data.AddByte(static_cast<uint8>(raceFinishReason));
    data.AddRawData(reinterpret_cast<const char *>(CheatTally), 0x20);
    UpdateEndRaceStats();
    data.AddRawData(reinterpret_cast<const char *>(&FinishedRaceStats), 0xc0);
    if (Online::IsInitialized()) {
        Online::SignalDriverFinish(data);
    }
    ChangeState(OPS_FINISHED);
}

void OnlineRacer::SignalFinish(SmartBitStream &data) {
    volatile uint8 rank;
    volatile uint8 blinkPoof;
    volatile uint8 finishReason;
    uint16 remote_cheat_tally[16];

    {
        uint32 v = 0;
        data.GetBits(v, 8);
        rank = v;
    }
    {
        uint32 v = 0;
        data.GetBits(v, 8);
        blinkPoof = v;
    }
    {
        uint32 v = 0;
        data.GetBits(v, 8);
        finishReason = v;
    }
    data.GetRawData(reinterpret_cast<char *>(remote_cheat_tally), 0x20);
    data.GetRawData(reinterpret_cast<char *>(&FinishedRaceStats), 0xc0);

    uint16 *cheat_tally = CheatTally;
    uint16 *buffer = remote_cheat_tally;
    int i;
    for (i = 15; i >= 0; --i) {
        *cheat_tally = *cheat_tally > *buffer ? *cheat_tally : *buffer;
        ++cheat_tally;
        ++buffer;
    }
    ChangeState(OPS_FINISHED);
}

void OnlineRacer::UpdateLocal(float t) {
    if (GRaceStatus::Exists()) {
        BadnessReason = 0;
        EndRaceCountdown = -1.0f;
        IPlayer *p = IPlayer::First(PLAYER_LOCAL);
        GRacerInfo *localRacerInfo = GRaceStatus::Get().GetRacerInfo(p->GetSimable());

        if (!TheOnlineManager.AreAllPlayersFinishedRacing() &&
            TheOnlineManager.TimeupStartTime.IsSet()) {
            EndRaceCountdown = TheOnlineManager.TimeupLength -
                static_cast<float>(WorldTimer.GetPackedTime() -
                                   TheOnlineManager.TimeupStartTime.GetPackedTime()) * 0.00025f;
            if (EndRaceCountdown <= 0.0f && !localRacerInfo->mFinishedRacing) {
                TheOnlineManager.RaceTimeup = true;
                GRaceStatus::Get().SkipToEndOfRaceForRacer(
                    IPlayer::First(PLAYER_LOCAL)->GetSimable(), localRacerInfo->mIndex, 3600.0f);
            }
        }
    }
}
