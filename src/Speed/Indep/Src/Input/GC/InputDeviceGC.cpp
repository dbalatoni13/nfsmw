#include "Speed/Indep/Src/Input/InputDeviceGC.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Input/Common/FFBTypes.h"
#include "Speed/Indep/Src/Input/Device.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
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

unsigned int pad_ticker; // size: 0x4, address: 0x8041E4B8
float pad_elapsed_ms;    // size: 0x4, address: 0x8041E4BC

static void UpdatePads(float ms) {
}

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
  RealInput::Device *device;
  RealInput::Data *data;
  int axis;
  int axis_min;
  int axis_max;
  const DeviceScalarInfo *di;
  float *value;
  bool wheel_connected;

  if (this->GetDeviceIndex() == 0) {
    unsigned int tick = bGetTicker();

    pad_elapsed_ms = pad_ticker != 0 ? bGetTickerDifference(pad_ticker) : 0.0f;
    UpdatePads(pad_elapsed_ms);
    pad_ticker = tick;
  }

  bMemSet(this->fCurrentValues, 0, 0x50);
  if (this->mWheelDevice != nullptr) {
    this->mWheelDevice->ReadInput(this->fCurrentValues + 20);
  }
  if (SteeringWheelDevice::WheelConnected(this->GetDeviceIndex())) {
    return;
  }

  device = input_devices[this->GetDeviceIndex()];
  if (device == nullptr) {
    return;
  }
  if (!device->IsPad()) {
    return;
  }

  bMemCpy(this->fPrevValues, this->fCurrentValues, 0x94);
  data = device->GetData();

  axis = data->mPad.mAxes[0];
  axis_min = -0x48;
  axis_max = 0x48;
  this->fCurrentValues[0] =
      UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
  this->fCurrentValues[1] =
      -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

  axis = data->mPad.mAxes[1];
  axis_min = 0x48;
  axis_max = -0x48;
  this->fCurrentValues[2] =
      UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
  this->fCurrentValues[3] =
      -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

  axis = data->mPad.mAxes[3];
  axis_min = -0x3b;
  axis_max = 0x3b;
  this->fCurrentValues[4] =
      UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
  this->fCurrentValues[5] =
      -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

  axis = data->mPad.mAxes[4];
  axis_min = 0x3b;
  axis_max = -0x3b;
  this->fCurrentValues[6] =
      UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
  this->fCurrentValues[7] =
      -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

  di = device_infos;
  value = this->fCurrentValues + 8;
  wheel_connected = SteeringWheelDevice::WheelConnected(this->GetDeviceIndex());

  // UNSOLVED 2 instructions: di++ shares a block with the loop-condition load,
  // so its INSN_PRIORITY beats value++ and both sched passes hoist it (each
  // one does so on its own; only -fno-schedule-insns -fno-schedule-insns2
  // keeps source order). A for(;;)+break body fixes the order but loses the
  // rotated pre-check, which costs more than it gains.
  while (di->name != nullptr) {
    if (wheel_connected || di->system_index >= 0) {
      if (di->type == kAnalogButton) {
        float newval = (float)data->mPad.mButtons[di->system_index] * 0.00666667f;

        if (di->ramp_max > di->ramp_min) {
          float range = di->ramp_max - di->ramp_min;

          newval = UMath::Clamp((newval - di->ramp_min) / range, 0.0f, 1.0f);
        }
        *value = newval;
      } else if (di->type == kDigitalButton) {
        if (data->mPad.mButtons[di->system_index] != 0) {
          *value = 1.0f;
        } else {
          *value = 0.0f;
        }
      }
    }
    value = value + 1;
    di = di + 1;
  }
}

int GameDevice::GetNumDeviceScalar() {  
        return this->mNumScalars;
};

// UNSOLVED Figure out what constructor to put there
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

//   this->mWheelDevice = new SteeringWheelDevice(deviceIndex);
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

void GameDevice::ReportCollision(const COLLISION_INFO &cinfo, bool iamA) {
  if (this->IsWheel()) {
    return;
  }

  InputEffectState &state = effect_states[this->GetDeviceIndex()];
  float magnitude;
  ISimable *me;
  ISimable *them;
  UMath::Vector3 current_velocity;
  float myspeed;
  float speedchange;
  float their_speed_change;
  char cType;
  float amplitude;
  float time;

  if (!state.Enabled) {
    return;
  }

  magnitude = 0.0f;
  me = ISimable::FindInstance(iamA ? cinfo.objA : cinfo.objB);
  them = ISimable::FindInstance(iamA ? cinfo.objB : cinfo.objA);
  if (me == nullptr) {
    return;
  }

  me->GetLinearVelocity(current_velocity);
  myspeed = UMath::Length(current_velocity);
  speedchange = UMath::Distance(current_velocity, iamA ? cinfo.objAVel : cinfo.objBVel);
  their_speed_change = 0.0f;
  if (them != nullptr) {
    UMath::Vector3 tmp;

    them->GetLinearVelocity(tmp);
    their_speed_change = UMath::Distance(tmp, iamA ? cinfo.objAVel : cinfo.objBVel);
  }

  switch (cinfo.type) {
  case Sim::Collision::Info::WORLD:
    magnitude = UMath::Min(speedchange * 0.0333333f, 1.0f);
    break;
  case Sim::Collision::Info::GROUND:
    magnitude = UMath::Min((speedchange - 1.0f) * 0.333333f, 1.0f);
    break;
  case Sim::Collision::Info::OBJECT:
    if (speedchange < 5.0f) {
      bool backEnder = false;

      if (them != nullptr) {
        IVehicle *theirVehicle;

        if (them->QueryInterface(&theirVehicle)) {
          UMath::Vector3 myVelocity = iamA ? cinfo.objAVel : cinfo.objBVel;
          UMath::Vector3 theirVelocity = iamA ? cinfo.objBVel : cinfo.objAVel;

          UMath::Normalize(myVelocity);
          UMath::Normalize(theirVelocity);
          if (UMath::Dot(myVelocity, theirVelocity) > 0.1f) {
            speedchange = UMath::Max(UMath::Length(cinfo.objAVel),
                                     UMath::Length(cinfo.objAVel)) *
                          0.5f;
            backEnder = true;
          }
        }
      }

      if (backEnder && speedchange > 5.0f) {
        magnitude = UMath::Min((speedchange - 5.0f) * 0.05f, 1.0f);
      } else if (myspeed > 2.0f && their_speed_change > 2.0f) {
        magnitude = UMath::Ramp(myspeed, 20.0f, 50.0f);
      }
    } else {
      magnitude = UMath::Min((speedchange - 5.0f) * 0.05f, 1.0f);
    }
    break;
  }

  amplitude = UMath::Min(magnitude, 0.5f);

  EffectBinary &ramp = state.CollisionNoise;

  if (ramp.Time > 0.0f) {
    ramp.MaxTime = UMath::Max(amplitude, ramp.MaxTime);
    ramp.Time = UMath::Max(amplitude, ramp.Time);
  } else {
    ramp.MaxTime = amplitude;
    ramp.Time = amplitude;
  }
}
