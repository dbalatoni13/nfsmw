#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "Speed/GameCube/Src/Logitech/LGWheels.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

LGWheels *SteeringWheelDevice::lgwheels = 0;

void SteeringWheelDevice::UpdateForces(IPlayer *player) {

    static float previousVelocity[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static float timeAtCollision[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float speedParam;
    float velocity;
    float forceLeftWheelAmplitude;
    float forceRightWheelAmplitude;
    float frontWheelVectorForceAmplitude;
    float constantForceAmplitude;
    int currentSurfaceType;
    int currentSurfaceMaxMagnitude;
    int currentSurfacePeriod;
    float speedDiffBetwFrames;
    float previousSpeedDiffBetwFrames = 0.0f;
    float normalizedCollisionParameter;
    float normalizedBottomingParameter;
    float timeBetwCollisions;

    if (!lgwheels->IsConnected(this->mDeviceIndex)) {
        return;
    }

    if (!this->IsConnected()) {
        return;
    }

    {
        PlayerSettings *settings = player->GetSettings();
        Sim::IStateManager *state_manager = UTL::COM::QueryInterface<Sim::IStateManager>(IGameState::Get());

        {
            ISimable *simable;
            IVehicle *vehicle;
            ISuspension *suspension;
            IInput *input;
            IPlayer *localPlayer;

            if (!settings->Rumble || !state_manager || state_manager->ShouldPauseInput() || player == NULL ||
                (simable = player->GetSimable()) == NULL ||
                (vehicle = UTL::COM::QueryInterface<IVehicle>(simable)) == NULL ||
                (suspension = UTL::COM::QueryInterface<ISuspension>(vehicle)) == NULL) {

                StopAllForces();
                return;
            }

            this->mManualTransmission = false;

            if (vehicle->QueryInterface(&input)) {
                this->mManualTransmission = !input->IsAutomaticShift();
            }

            velocity = vehicle->GetSpeed();

            speedParam = bMin(1.0f, bAbs(velocity * 0.05f));

            if (GRaceStatus::IsDragRace()) {

                lgwheels->PlaySpringForce(this->mDeviceIndex, 0, (unsigned char)this->ScaledForceParam(220.0f),
                                          (short)this->ScaledForceParam(220.0f));
            } else {

                lgwheels->PlaySpringForce(this->mDeviceIndex, 0, (unsigned char)(this->ScaledForceParam(100.0f) * speedParam),
                                          (short)this->ScaledForceParam(100.0f));
            }

            forceLeftWheelAmplitude = suspension->GetWheelSkid(0);
            forceRightWheelAmplitude = suspension->GetWheelSkid(1);
            constantForceAmplitude = forceLeftWheelAmplitude + forceRightWheelAmplitude;

            if (bAbs(velocity) > 1.0f) {

                frontWheelVectorForceAmplitude = UMath::Clamp(constantForceAmplitude * 100.0f, -255.0f, 255.0f);
            } else {

                frontWheelVectorForceAmplitude = 0.0f;
            }

            localPlayer = IPlayer::First(PLAYER_ALL);

            if (localPlayer != NULL && localPlayer->InGameBreaker()) {

                lgwheels->PlayConstantForce(this->mDeviceIndex, (short)this->ScaledForceParam(frontWheelVectorForceAmplitude * 0.5f), 0x10e);

            } else if (GRaceStatus::IsDragRace()) {

                lgwheels->StopConstantForce(this->mDeviceIndex);

            } else {

                lgwheels->PlayConstantForce(this->mDeviceIndex, (short)this->ScaledForceParam(frontWheelVectorForceAmplitude), 0x10e);
            }

            lgwheels->PlayDamperForce(this->mDeviceIndex, (short)(this->ScaledForceParam(100.0f) * (1.0f - speedParam)));

            if (!suspension->IsWheelOnGround(0) && !suspension->IsWheelOnGround(1)) {

                if (!lgwheels->IsPlaying(this->mDeviceIndex, 9)) {

                    lgwheels->PlayCarAirborne(this->mDeviceIndex);
                }
            } else {

                if (lgwheels->IsPlaying(this->mDeviceIndex, 9)) {

                    lgwheels->StopCarAirborne(this->mDeviceIndex);
                }
            }

            speedDiffBetwFrames = previousVelocity[this->mDeviceIndex] - velocity;

            if (bAbs(speedDiffBetwFrames) > 4.0f) {

                if (previousSpeedDiffBetwFrames > 3.0f) {
                    speedDiffBetwFrames += previousSpeedDiffBetwFrames;
                    timeAtCollision[this->mDeviceIndex] = 0.0f;
                }

                normalizedCollisionParameter = bMin(1.0f, (speedDiffBetwFrames - 4.0f) * 0.06666667f);
                timeBetwCollisions = WorldTimer.GetSeconds() - timeAtCollision[this->mDeviceIndex];

                if (timeBetwCollisions < 0.0f) {

                    timeAtCollision[this->mDeviceIndex] = 0.0f;

                } else if (timeBetwCollisions >= 0.15f) {

                    timeAtCollision[this->mDeviceIndex] = WorldTimer.GetSeconds();
                    lgwheels->PlayFrontalCollisionForce(
                        this->mDeviceIndex, (unsigned char)(this->ScaledForceParam(255.0f) * normalizedCollisionParameter));
                }
            }

            const SimSurface &surfaceLeft = suspension->IsWheelOnGround(0) ? suspension->GetWheelRoadSurface(0) : SimSurface::kNull;
            const SimSurface &surfaceRight = suspension->IsWheelOnGround(1) ? suspension->GetWheelRoadSurface(1) : SimSurface::kNull;

            int surfaceForceType = surfaceLeft.WheelSurfaceEffect();
            int surfaceForceMaxMag = (surfaceLeft.WheelEffectIntensity() + surfaceRight.WheelEffectIntensity()) >> 1;
            int surfaceForcePeriod = (surfaceLeft.WheelEffectFrequency() + surfaceRight.WheelEffectFrequency()) >> 1;

            if (surfaceForceType == 2) {
                surfaceForceType = 4;
            } else if (surfaceForceType == 1) {
                surfaceForceType = 3;
            } else {
                surfaceForceType = 2;
            }

            lgwheels->PlaySurfaceEffect(this->mDeviceIndex, (unsigned char)surfaceForceType,
                                        (unsigned char)(this->ScaledForceParam(surfaceForceMaxMag) * speedParam), surfaceForcePeriod);

            previousVelocity[this->mDeviceIndex] = velocity;
        }
    }
}

void SteeringWheelDevice::StopAllForces() {
    lgwheels->StopConstantForce(this->mDeviceIndex);
    lgwheels->StopSurfaceEffect(this->mDeviceIndex);
    lgwheels->StopDamperForce(this->mDeviceIndex);
    lgwheels->StopCarAirborne(this->mDeviceIndex);
    lgwheels->StopSlipperyRoadEffect(this->mDeviceIndex);
    lgwheels->PlaySpringForce(this->mDeviceIndex, 0, (unsigned char)this->ScaledForceParam(220.0f), (short)this->ScaledForceParam(220.0f));
}

void SteeringWheelDevice::ReadInput(float *inputBuffer) {
    if (lgwheels->IsConnected(this->mDeviceIndex)) {
        {
            this->isActivated = true;

            inputBuffer[0] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x4) ? 1.0f : 0.0f;
            inputBuffer[1] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x8) ? 1.0f : 0.0f;
            inputBuffer[2] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x1) ? 1.0f : 0.0f;
            inputBuffer[3] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x2) ? 1.0f : 0.0f;

            inputBuffer[8] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x100) ? 1.0f : 0.0f;
            inputBuffer[9] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x200) ? 1.0f : 0.0f;
            inputBuffer[6] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x400) ? 1.0f : 0.0f;
            inputBuffer[7] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x800) ? 1.0f : 0.0f;
            inputBuffer[10] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x10) ? 1.0f : 0.0f;
            inputBuffer[13] = lgwheels->ButtonIsPressed(this->mDeviceIndex, 0x1000) ? 1.0f : 0.0f;

            inputBuffer[4] = (unsigned int)lgwheels->Position[this->mDeviceIndex].triggerLeft * 0.003921569f;
            inputBuffer[5] = (unsigned int)lgwheels->Position[this->mDeviceIndex].triggerRight * 0.003921569f;

            if (lgwheels->PedalsConnected(this->mDeviceIndex)) {

                inputBuffer[15] = (unsigned int)lgwheels->Position[this->mDeviceIndex].brake * 0.003921569f;
                inputBuffer[14] = (unsigned int)lgwheels->Position[this->mDeviceIndex].accelerator * 0.003921569f;

            } else {

                inputBuffer[15] = (unsigned int)lgwheels->Position[this->mDeviceIndex].triggerLeft * 0.003921569f;
                inputBuffer[14] = (unsigned int)lgwheels->Position[this->mDeviceIndex].triggerRight * 0.003921569f;
            }

            float wheel = this->ConvertWheelRotation(this->mDeviceIndex);

            inputBuffer[12] = UMath::Clamp(wheel, 0.0f, 1.0f);
            inputBuffer[11] = -UMath::Clamp(wheel, -1.0f, 0.0f);
        }
    } else {
        {
            for (int i = 0; i <= 15; i++) {
                inputBuffer[i] = 0.0f;
            }
        }
    }
}

void SteeringWheelDevice::InitWheelSupport() {
    if (lgwheels == 0) {
        int i = 0;
        LGWheels *wheels = new ("LGWheel", 0) LGWheels;
        lgwheels = wheels;
        wheels->ReadAll();
        for (; i <= 3; i++) {
            if (lgwheels->IsConnected(i)) {
                lgwheels->PlaySpringForce(i, 0, 180, 180);
            }
        }
    }
}

void SteeringWheelDevice::PollWheels() {
    if (lgwheels != 0) {
        lgwheels->ReadAll();
    }
}

float SteeringWheelDevice::ConvertWheelRotation(int channel) {
    float val = lgwheels->Position[channel].wheel;
    float originalVal = UMath::Clamp(val * 0.007874016f, -1.0f, 1.0f);
    val = originalVal * 0.8f + originalVal * originalVal * originalVal * 0.2f;
    return val;
}

float SteeringWheelDevice::ScaledForceParam(float value) {
    return value * 0.85f;
}

bool SteeringWheelDevice::IsConnected() {
    if (lgwheels == 0) {
        return false;
    }
    return lgwheels->IsConnected(this->mDeviceIndex);
}

ISteeringWheel::SteeringType SteeringWheelDevice::GetSteeringType() {
    if (this->IsConnected()) {
        return ISteeringWheel::kWheelSpeedSensitive;
    }
    return ISteeringWheel::kGamePad;
}

bool SteeringWheelDevice::WheelConnected(int port) {
    return lgwheels->IsConnected(port);
}
