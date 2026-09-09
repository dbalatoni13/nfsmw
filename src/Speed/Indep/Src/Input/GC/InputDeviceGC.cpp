#include "Speed/Indep/Src/Input/InputDeviceGC.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Input/Common/FFBTypes.h"
#include "Speed/Indep/Src/Input/Device.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "dolphin/pad.h"

// Decl: 23
struct DeviceScalarInfo {
    DeviceScalarType type; // offset 0x0, size 0x4, Decl: 24
    const char * name; // offset 0x4, size 0x4, Decl: 25
    int system_index; // offset 0x8, size 0x4, Decl: 26
    float ramp_min; // offset 0xC, size 0x4
    float ramp_max; // offset 0x10, size 0x4
};

static struct InputEffectState effect_states[4]; // size: 0x50, address: 0x8047E874, Decl: 81

static const struct DeviceScalarInfo device_infos[37] = {

}; // size: 0x2E4, address: 0x803F424C, Decl: 89


struct InputDevice* GameDevice::Construct(int port) { 

}

inline bool GameDevice::IsWheel() { 
    if (mWheelDevice != nullptr) {
        return mWheelDevice->IsConnected();
    }
    return false;
} 

inline struct UTL::COM::IUnknown* GameDevice::GetInterfaces() { return this; }; 

inline struct UTL::COM::IUnknown* GameDevice::GetSecondaryDevice() { return mWheelDevice; }  

int GameDevice::mCount; // size: 0x4, address: 0x8041E4B0, Decl: 734

void GameDevice::Initialize() { 
    int i;
    const DeviceScalarInfo *info;

    i = 0;
    this->mNumScalars = i;
    info = device_infos;

    while (info->name != nullptr && i <= 0x24) {
        this->fDeviceScalar[i].InitializeDeviceScalar(
        info->type, info->name, &this->fPrevValues[i],
            &this->fCurrentValues[i]);
        info = info + 1;
        i = i + 1;
        this->mNumScalars = this->mNumScalars + 1;
    }
 }

// I am unsure where this is defined?
// UNSOLVED
bool input_connected[4];
bool gShowPortInfo;
float input_buzz[8];
RealInput::Device *input_devices[4];

// void calls are likely from a debug build that are stripped out.
// TODO figure out using undercover
bool GameDevice::IsConnected() { 
    if ((this->mWheelDevice != nullptr) && this->mWheelDevice->IsConnected()) {
        (void)this->GetDeviceIndex();
        (void)this->GetDeviceIndex();
        return true;
    }
    if (gShowPortInfo) {
        PADStatus HardwarePadStatus[4];
        PADRead(HardwarePadStatus);
        (void)this->GetDeviceIndex();
        (void)this->GetDeviceIndex();
        (void)this->GetDeviceIndex();
        (void)this->GetDeviceIndex();
    }
    return input_connected[this->GetDeviceIndex()];
}


void GameDevice::StartVibration() {
  if (this->IsWheel()) {
    return;
  }

  RealInput::Effect::Info effectInfo;
  RealInput::Device *device;
  RealInput::Effect *effect;

  effectInfo.mFullStop = 0;
  device = input_devices[this->GetDeviceIndex()];
  effect = device->CreateEffect(&effectInfo);
  if (effect != nullptr) {
    effect->Start();
  }
  input_buzz[this->GetDeviceIndex()] = 500.0f;
}

void GameDevice::StopVibration() { 

}

void GameDevice::PollDevice() { 

}

int GameDevice::GetNumDeviceScalar() {  
        return this->mNumScalars;
};

GameDevice::GameDevice(int deviceIndex) : InputDevice(deviceIndex), IFeedback(nullptr){ 

}

GameDevice::~GameDevice(){ 

}; 

void PauseEffects() { 

}

void ResumeEffects() { 

}

void ResetEffects() { 

}

void BeginUpdate() { 

}

void EndUpdate() { 

}

void UpdateRoadNoise(bool front, const struct SimSurface & surface, float speed) { 

}

void UpdateTireSkid(bool front, const struct SimSurface & surface, float speed) { 

}

void UpdateTireSlip(bool front, const struct SimSurface & surface, float speed) { 

}

void UpdateRPM(float powerband, float overrev, float throttle) { 

}

void UpdateShiftPotential(enum ShiftPotential potential) { 

}

void UpdateEngineBlown(bool blown) { 

}

void UpdateNOS(bool engaged, float NOSLevel) { 

}

void UpdateShifting(bool shifting) { 

}

void ReportCollision(const COLLISION_INFO & cinfo, bool iamA) { 

}
