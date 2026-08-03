#include "OnlineManager.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

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
