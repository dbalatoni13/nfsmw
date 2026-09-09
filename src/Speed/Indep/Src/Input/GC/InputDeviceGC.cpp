#include "Speed/Indep/Src/Input/InputDeviceGC.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Input/Common/FFBTypes.h"
#include "Speed/Indep/Src/Input/Device.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
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


inline struct InputDevice* GameDevice::Construct(int port) { // Decl: 197
    return new ("GameDevice") GameDevice(port);
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

RealInput::Interface *inputsys; // size: 0x4, address: 0x8041E47C
extern EA::Allocator::IAllocator *gMemoryAllocator; // 0x80457918

bool input_connected[4];
bool gShowPortInfo;
float input_buzz[8];
RealInput::Device *input_devices[4];
RealInput::Effect *input_effects[4]; // size: 0x10, address: 0x8041E4A0

static int MyEnumDeviceCallback(RealInput::Device *pDevice, unsigned int userData,
                                RealInput::Interface *pInterface) {
  int port;

  port = pDevice->GetInfo()->mPortNum;
  if (port <= 3) {
    input_devices[port] = pDevice;
    input_connected[port] = pDevice->GetCapabilities()->mAttached;
  }
  return 1;
}

static void InitEffects() {
  for (int i = 0; i < 4; i++) {
    RealInput::Device *device = input_devices[i];
    if (device == nullptr || !device->IsPad() || !input_connected[i]) {
      input_effects[i] = nullptr;
    }
  }
}

// UNSOLVED Dwarf register issues
static void InitPads() {
  RealInput::ConfigOptions opts;
  RealInput::Interface *m_pInputInterface;

  opts.mEventQueueSize = 32;
  opts.mpEnumDevicesCallback = MyEnumDeviceCallback;
  opts.mMaxNumEffects = 4;
  opts.mAllocator = gMemoryAllocator;

  inputsys = RealInput::Interface::CreateInstance(opts);
  m_pInputInterface = inputsys;
  inputsys->AddRef();
  SteeringWheelDevice::InitWheelSupport();
}

static void ReleasePads() {
  inputsys->Release();
}

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
  if (input_buzz[this->GetDeviceIndex()] > 0.0f) {
    input_buzz[this->GetDeviceIndex()] = 0.00001f;
  }
}

void GameDevice::PollDevice() { 

}

int GameDevice::GetNumDeviceScalar() {  
        return this->mNumScalars;
};

GameDevice::GameDevice(int deviceIndex) : InputDevice(deviceIndex), IFeedback(this) {
  this->mNumScalars = 0;
  if (GameDevice::mCount == 0) {
    InitPads();
    InitEffects();
  }
  GameDevice::mCount = GameDevice::mCount + 1;

  this->fDeviceScalar = this->fPS2DeviceScalars;
  this->fCurrentValues = this->fPS2CurrentValues;
  this->fPrevValues = this->fPS2PrevValues;
  bMemSet(this->fPrevValues, 0, sizeof(this->fPS2PrevValues));
  bMemSet(this->fCurrentValues, 0, sizeof(this->fPS2CurrentValues));

  // Retail allocates through the global operator new(size, const char *, int)
  // in bWare.hpp. Spelling that here (`new (file, line) SteeringWheelDevice`)
  // makes ngccc.exe die part-way through emitting DWARF -- it exits 0 with a
  // truncated .s and ngcas then fails on undefined labels. The trigger is
  // inlining a *global* operator new: a class-level one is fine, as is a local
  // SteeringWheelDevice, and placement-new of a trivial type. So the allocation
  // goes through SteeringWheelDevice::operator new instead, which emits the
  // same `li r3, 0x24; bl __builtin_vec_new`. Code matches; the only DWARF
  // difference in this function is that one inlined-operator-new line.
  this->mWheelDevice = new SteeringWheelDevice(deviceIndex);
}

GameDevice::~GameDevice() {
  GameDevice::mCount = GameDevice::mCount - 1;
  if (GameDevice::mCount == 0) {
    ReleasePads();
  }
};

void GameDevice::PauseEffects() {
  effect_states[this->GetDeviceIndex()].Enabled = false;
  effect_states[this->GetDeviceIndex()].Push(input_effects[this->GetDeviceIndex()]);
  SteeringWheels_StopAllForces();
}

void GameDevice::ResumeEffects() {
  effect_states[this->GetDeviceIndex()].Enabled = true;
  effect_states[this->GetDeviceIndex()].Push(input_effects[this->GetDeviceIndex()]);
}

void GameDevice::ResetEffects() {
  int dIndex;
  RealInput::Effect *effect;

  dIndex = this->GetDeviceIndex();
  effect = input_effects[dIndex];
  if (effect == nullptr) {
    return;
  }

  RealInput::Effect::Info info;

  effect->GetInfo(&info);
  info.mFullStop = 1;
  effect->SetInfo(&info);
  effect->Stop();

  effect_states[dIndex].Enabled = false;
  effect_states[dIndex].On = 0.0f;
  effect_states[dIndex].CollisionNoise.Time = 0.0f;
  effect_states[dIndex].CollisionNoise.MaxTime = 0.0f;
  effect_states[dIndex].Push(input_effects[dIndex]);
  SteeringWheels_StopAllForces();
}

void GameDevice::BeginUpdate() {
  InputEffectState &state = effect_states[this->GetDeviceIndex()];

  state.On = 0.0f;
}

void GameDevice::EndUpdate() { 

}

void GameDevice::UpdateRoadNoise(bool front, const struct SimSurface & surface, float speed) { 

}

void GameDevice::UpdateTireSkid(bool front, const struct SimSurface & surface, float speed) { 

}

void GameDevice::UpdateTireSlip(bool front, const struct SimSurface & surface, float speed) { 

}

void GameDevice::UpdateRPM(float powerband, float overrev, float throttle) { 

}

void GameDevice::UpdateShiftPotential(enum ShiftPotential potential) { 

}

void GameDevice::UpdateEngineBlown(bool blown) { 

}

void GameDevice::UpdateNOS(bool engaged, float NOSLevel) { 

}

void GameDevice::UpdateShifting(bool shifting) { 

}

void GameDevice::ReportCollision(const COLLISION_INFO & cinfo, bool iamA) { 

}
