#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/device.h"
#include "Speed/Indep/Src/Input/Common/FFBTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

#include "dolphin/pad.h"

extern void *gMemoryAllocator;

namespace RealInput {
struct Interface;
}

RealInput::Interface *inputsys = nullptr;
static float input_buzz[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

namespace RealInput {
struct Interface;

struct ConfigOptions {
    ConfigOptions() : allocator(0), callback(0), userData(0), flags(0x20) {}

    void *allocator;
    int (*callback)(Device *, unsigned int, Interface *);
    void *userData;
    int flags;
    int count;
};

struct Event {
    Device *mDevice; // offset 0x0
    int mType;       // offset 0x4
};

// El bloque que devuelve Device::GetData() en GameCube.
struct PadData {
    int mStickX;      // offset 0x0
    int mStickY;      // offset 0x4
    int mPad8;        // offset 0x8
    int mSubStickX;   // offset 0xC
    int mSubStickY;   // offset 0x10
    int mPad14;       // offset 0x14
    int mValues[64];  // offset 0x18
};

struct Interface {
    static Interface *CreateInstance(const ConfigOptions &options);
    virtual unsigned int AddRef();
    virtual unsigned int Release();
    virtual int Update();
    virtual void Reserved3();
    virtual void Reserved4();
    virtual void Reserved5();
    virtual Event *GetEvent();
};
} // namespace RealInput

RealInput::Device *input_devices[4];
bool input_connected[4];
RealInput::Effect *input_effects[4] = {nullptr, nullptr, nullptr, nullptr};
static InputEffectState effect_states[4];

void SteeringWheels_StopAllForces();

static int MyEnumDeviceCallback(RealInput::Device *device, unsigned int unused, RealInput::Interface *iface);
static void UpdatePads(float ms);
static void ReleasePads();

struct DeviceScalarInfo {
    DeviceScalarType type;
    const char *name;
    int system_index;
    float ramp_min;
    float ramp_max;
};

static const DeviceScalarInfo device_infos[] = {
    {kJoyAxis, "ALX_L", 0, 0.0f, 0.0f},
    {kJoyAxis, "ALX_R", 0, 0.0f, 0.0f},
    {kJoyAxis, "ALY_U", 0, 0.0f, 0.0f},
    {kJoyAxis, "ALY_D", 0, 0.0f, 0.0f},
    {kJoyAxis, "ARX_L", 1, 0.0f, 0.0f},
    {kJoyAxis, "ARX_R", 1, 0.0f, 0.0f},
    {kJoyAxis, "ARY_U", 1, 0.0f, 0.0f},
    {kJoyAxis, "ARY_D", 1, 0.0f, 0.0f},
    {kDigitalButton, "DUp", 3, 0.0f, 0.0f},
    {kDigitalButton, "DDown", 2, 0.0f, 0.0f},
    {kDigitalButton, "DLeft", 0, 0.0f, 0.0f},
    {kDigitalButton, "DRight", 1, 0.0f, 0.0f},
    {kDigitalButton, "Start", 11, 0.0f, 0.0f},
    {kDigitalButton, "Y", 10, 0.0f, 0.0f},
    {kDigitalButton, "A", 7, 0.0f, 0.0f},
    {kDigitalButton, "B", 8, 0.0f, 0.0f},
    {kDigitalButton, "X", 9, 0.0f, 0.0f},
    {kDigitalButton, "Z", 4, 0.0f, 0.0f},
    {kAnalogButton, "L", 12, 0.0f, 0.5f},
    {kAnalogButton, "R", 13, 0.0f, 0.5f},
    {kAnalogButton, "W_DPAD_DOWN", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_DPAD_UP", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_DPAD_LEFT", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_DPAD_RIGHT", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_TRIGGER_LEFT", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_TRIGGER_RIGHT", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_X", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_Y", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_A", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_B", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_Z", -1, 0.0f, 0.0f},
    {kJoyAxis, "W_LEFT", -1, 0.0f, 0.0f},
    {kJoyAxis, "W_RIGHT", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_START", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_X_ALT", -1, 0.0f, 0.0f},
    {kAnalogButton, "W_Y_ALT", -1, 0.0f, 0.0f},
    // Centinela: el objetivo tiene el nombre a NULL, no a "None" -- es lo que
    // corta el `while (info->name != 0)` de Initialize.
    {kJoyAxis, NULL, 0, 0.0f, 0.0f},
};

class GameDevice : public InputDevice, public IFeedback {
    RealInput::Effect *mEffect;
    DeviceScalar fPS2DeviceScalars[37];
    float fPS2PrevValues[37];
    float fPS2CurrentValues[37];
    int mNumScalars;
    SteeringWheelDevice *mWheelDevice;

  public:
    GameDevice(int deviceIndex);
    virtual void Initialize();
    virtual bool IsConnected();
    virtual void StartVibration();
    virtual void EndUpdate();
    virtual void UpdateRoadNoise(bool front, const SimSurface &surface, float speed);
    virtual void UpdateTireSkid(bool front, const SimSurface &surface, float speed);
    virtual void UpdateTireSlip(bool front, const SimSurface &surface, float speed);
    virtual void UpdateRPM(float powerband, float overrev, float throttle);
    virtual void UpdateShiftPotential(ShiftPotential potential);
    virtual void UpdateEngineBlown(bool blown);
    virtual void UpdateNOS(bool engaged, float NOSLevel);
    virtual void UpdateShifting(bool shifting);
    virtual void ReportCollision(const COLLISION_INFO &cinfo, bool iamA);
    virtual void StopVibration();
    virtual int GetNumDeviceScalar();
    virtual void PauseEffects();
    virtual void ResumeEffects();
    virtual void ResetEffects();
    virtual void BeginUpdate();

    static InputDevice *Construct(int port) {
        return new ("GameDevice") GameDevice(port);
    }

    virtual bool IsWheel() {
        if (this->mWheelDevice != 0) {
            return this->mWheelDevice->IsConnected();
        }
        return false;
    }

    virtual UTL::COM::IUnknown *GetInterfaces() {
        return static_cast<IFeedback *>(this);
    }

    virtual UTL::COM::IUnknown *GetSecondaryDevice() {
        return this->mWheelDevice;
    }

    virtual ~GameDevice();
    virtual void PollDevice();

    static int mCount;
};

int GameDevice::mCount = 0;

int gShowPortInfo = 0;
static unsigned int pad_ticker = 0;
static float pad_elapsed_ms = 0.0f;

UTL::COM::Factory<int, InputDevice, UCrc32>::Prototype _GameDevice("GameDevice", GameDevice::Construct);

void InputEffectState::Run(float ms) {
    if (ms <= 0.0f) {
        return;
    }
    if (!this->Enabled) {
        return;
    }
    this->On = this->CollisionNoise.Run(ms);
}

void InputEffectState::Push(RealInput::Effect *effect) {
    if (effect == 0) {
        return;
    }
    RealInput::Effect::Info info;
    bool play = false;
    effect->GetInfo(&info);
    if (this->Enabled) {
        play = this->On > 0.1f;
    }
    unsigned int status = effect->GetStatus();
    if (status == 1) {
        if (play) {
            return;
        }
        info.mFullStop = 1;
        effect->SetInfo(&info);
        effect->Stop();
    } else {
        if (!play) {
            return;
        }
        info.mFullStop = 0;
        effect->SetInfo(&info);
        effect->Start();
    }
}

static int MyEnumDeviceCallback(RealInput::Device *device, unsigned int unused, RealInput::Interface *iface) {
    int port = device->GetInfo()->mPortNum;
    if (port <= 3) {
        input_devices[port] = device;
        input_connected[port] = device->GetCapabilities()->mAttached;
    }
    return 1;
}

static void InitEffects() {
    for (int i = 0; i <= 3; i++) {
        RealInput::Device *device = input_devices[i];
        if (device == 0 || device->IsPad() == 0 || input_connected[i] == 0) {
            input_effects[i] = 0;
        }
    }
}

static void InitPads() {
    RealInput::ConfigOptions options;
    RealInput::Interface *m_pInputInterface;
    options.allocator = gMemoryAllocator;
    options.callback = MyEnumDeviceCallback;
    options.count = 4;
    m_pInputInterface = RealInput::Interface::CreateInstance(options);
    inputsys = m_pInputInterface;
    inputsys->AddRef();
    SteeringWheelDevice::InitWheelSupport();
}

static void UpdatePads(float ms) {
    {
        inputsys->Update();
        for (int i = 0; i <= 3; i++) {

            if (input_devices[i] != 0) {

                if (input_devices[i]->Update() != 0) {

                    input_connected[i] = 0;

                } else if (input_devices[i]->IsPad() && input_buzz[i] <= 0.0f) {

                    effect_states[i].Push(input_effects[i]);
                    effect_states[i].Run(ms);

                } else if (input_buzz[i] > 0.0f) {

                    input_buzz[i] -= ms;
                    if (input_buzz[i] <= 0.0f) {

                        RealInput::Effect *effect = input_devices[i]->GetEffect();
                        if (effect != 0) {

                            RealInput::Effect::Info info;
                            effect->GetInfo(&info);
                            info.mFullStop = 1;
                            effect->SetInfo(&info);
                            effect->Stop();
                        }
                    }
                }
            }
        }
    }
    for (RealInput::Event *event = inputsys->GetEvent(); event != 0; event = inputsys->GetEvent()) {
        RealInput::Device *device = event->mDevice;
        switch (event->mType) {
        case 0:
            input_connected[device->GetInfo()->mPortNum] = 1;

            input_buzz[device->GetInfo()->mPortNum] = 0.0f;
            if (input_effects[device->GetInfo()->mPortNum] == 0) {

                effect_states[device->GetInfo()->mPortNum].CollisionNoise.Clear();
                RealInput::Effect::Info info;
                info.mFullStop = 1;
                input_effects[device->GetInfo()->mPortNum] = device->CreateEffect(&info);
            }

            break;

        case 1:
            input_connected[device->GetInfo()->mPortNum] = 0;
            input_buzz[device->GetInfo()->mPortNum] = 0.0f;
            break;
        }
    }
    {
        PADStatus HardwarePadStatus[4];
        PADRead(HardwarePadStatus);
        for (int i = 0; i <= 1; i++) {

            RealInput::Device *device = input_devices[i];
            if (input_connected[i] == 0 && HardwarePadStatus[i].err == -3) {

                input_connected[i] = 1;

                input_buzz[i] = 0.0f;
                if (input_effects[i] == 0) {

                    effect_states[i].CollisionNoise.Clear();
                    RealInput::Effect::Info info;
                    info.mFullStop = 1;
                    input_effects[i] = device->CreateEffect(&info);
                }
            }

            if (input_connected[i] == 1 && HardwarePadStatus[i].err == -1) {

                input_connected[i] = 0;
                input_buzz[i] = 0.0f;
            }

        }

        SteeringWheelDevice::PollWheels();
    }
}

static void ReleasePads() {
    inputsys->Release();
}

void GameDevice::Initialize() {
    int i = 0;
    this->mNumScalars = 0;
    const DeviceScalarInfo *info = device_infos;

    // `while (A && B)` con `i` arrancando en 0: GCC pliega el `i <= 36` de la
    // GUARDA (sabe que 0 <= 36) y deja las dos pruebas solo en la arista de
    // retorno, que es exactamente lo que emite el objetivo. La cadena de
    // `goto` que habia aqui era un artefacto de descompilar a mano.
    while (info->name != 0 && i <= 36) {
        this->fDeviceScalar[i].InitializeDeviceScalar(info->type, info->name, &this->fPrevValues[i], &this->fCurrentValues[i]);
        i++;
        this->mNumScalars++;
        info++;
    }
}

bool GameDevice::IsConnected() {
    if (this->mWheelDevice != 0) {
        if (this->mWheelDevice->IsConnected()) {
            return true;
        }
    }
    if (gShowPortInfo != 0) {
        PADStatus HardwarePadStatus[4];
        PADRead(HardwarePadStatus);
    }
    return input_connected[this->GetDeviceIndex()];
}

void GameDevice::StartVibration() {
    if (this->IsWheel()) {
        return;
    }
    RealInput::Effect::Info effectInfo;
    effectInfo.mFullStop = 0;
    RealInput::Device *device = input_devices[this->GetDeviceIndex()];
    RealInput::Effect *effect = device->CreateEffect(&effectInfo);
    if (effect != 0) {
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
    if (this->GetDeviceIndex() == 0) {

        {
            unsigned int tick = bGetTicker();
            pad_elapsed_ms = pad_ticker ? bGetTickerDifference(pad_ticker) : 0.0f;
            UpdatePads(pad_elapsed_ms);
            pad_ticker = tick;
        }
    }

    bMemSet(this->fCurrentValues, 0, 0x50);

    if (this->mWheelDevice != 0) {

        this->mWheelDevice->ReadInput(this->fCurrentValues + 20);
    }

    bool wheel_connected = SteeringWheelDevice::WheelConnected(this->GetDeviceIndex());
    if (wheel_connected) {
        return;
    }

    RealInput::Device *device = input_devices[this->GetDeviceIndex()];

    if (device == 0 || device->IsPad() == 0) {
        return;
    }

    bMemCpy(this->fPrevValues, this->fCurrentValues, 0x94);

    RealInput::PadData *data = (RealInput::PadData *)device->GetData();

    int axis;
    int axis_min;
    int axis_max;

    axis = data->mStickX;
    axis_min = -72;
    axis_max = 72;
    this->fCurrentValues[0] = UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
    this->fCurrentValues[1] = -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

    axis = data->mStickY;
    axis_min = 72;
    axis_max = -72;
    this->fCurrentValues[2] = UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
    this->fCurrentValues[3] = -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

    axis = data->mSubStickX;
    axis_min = -59;
    axis_max = 59;
    this->fCurrentValues[4] = UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
    this->fCurrentValues[5] = -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

    axis = data->mSubStickY;
    axis_min = 59;
    axis_max = -59;
    this->fCurrentValues[6] = UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, 0.0f, 1.0f);
    this->fCurrentValues[7] = -UMath::Clamp(((float)(axis - axis_min) / (float)(axis_max - axis_min) + -0.5f) * -2.0f, -1.0f, 0.0f);

    const DeviceScalarInfo *di = &device_infos[8];

    float *value = &this->fCurrentValues[8];
    wheel_connected = SteeringWheelDevice::WheelConnected(this->GetDeviceIndex());
    while (di->name != 0) {

        if (wheel_connected || di->system_index >= 0) {

            if (di->type == kAnalogButton) {

                float newval = data->mValues[di->system_index] * 0.006666667f;
                if (di->ramp_max > di->ramp_min) {

                    float range = di->ramp_max - di->ramp_min;
                    newval = UMath::Clamp((newval - di->ramp_min) / range, 0.0f, 1.0f);
                }
                *value = newval;
            } else if (di->type == kDigitalButton) {

                if (data->mValues[di->system_index] != 0) {
                    *value = 1.0f;
                } else {
                    *value = 0.0f;
                }
            }
        }

        value++;
        di++;
    }
}

int GameDevice::GetNumDeviceScalar() {
    return this->mNumScalars;
}

GameDevice::GameDevice(int deviceIndex) : InputDevice(deviceIndex), IFeedback(this) {
    this->mNumScalars = 0;
    if (mCount == 0) {
        InitPads();
        InitEffects();
    }
    mCount++;
    this->fDeviceScalar = this->fPS2DeviceScalars;
    this->fPrevValues = this->fPS2PrevValues;
    this->fCurrentValues = this->fPS2CurrentValues;
    bMemSet(this->fPrevValues, 0, 0x94);
    bMemSet(this->fCurrentValues, 0, 0x94);
    this->mWheelDevice = ::new ("GameDevice::SteeringWheelDevice", 0) SteeringWheelDevice(deviceIndex);
}

GameDevice::~GameDevice() {
    mCount--;
    if (mCount == 0) {
        ReleasePads();
    }
}

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
    int dIndex = this->GetDeviceIndex();
    RealInput::Effect *effect = input_effects[dIndex];
    if (effect == 0) {
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

void GameDevice::EndUpdate() {}

void GameDevice::UpdateRoadNoise(bool front, const SimSurface &surface, float speed) {}

void GameDevice::UpdateTireSkid(bool front, const SimSurface &surface, float speed) {}

void GameDevice::UpdateTireSlip(bool front, const SimSurface &surface, float speed) {}

void GameDevice::UpdateRPM(float powerband, float overrev, float throttle) {}

void GameDevice::UpdateShiftPotential(ShiftPotential potential) {}

void GameDevice::UpdateEngineBlown(bool blown) {}

void GameDevice::UpdateNOS(bool engaged, float NOSLevel) {}

void GameDevice::UpdateShifting(bool shifting) {}

void GameDevice::ReportCollision(const COLLISION_INFO &cinfo, bool iamA) {
    if (IsWheel()) {
        return;
    }

    InputEffectState &state = effect_states[this->GetDeviceIndex()];

    if (!state.Enabled) {
        return;
    }

    float magnitude = 0.0f;

    ISimable *me = ISimable::FindInstance(iamA ? cinfo.objA : cinfo.objB);
    ISimable *them = ISimable::FindInstance(iamA ? cinfo.objB : cinfo.objA);

    if (me == NULL) {
        return;
    }

    UMath::Vector3 current_velocity;

    me->GetLinearVelocity(current_velocity);

    float myspeed = UMath::Length(current_velocity);
    float speedchange = UMath::Distance(current_velocity, iamA ? cinfo.objAVel : cinfo.objBVel);
    float their_speed_change = 0.0f;

    if (them != NULL) {
        UMath::Vector3 tmp;

        them->GetLinearVelocity(tmp);
        their_speed_change = UMath::Distance(tmp, iamA ? cinfo.objAVel : cinfo.objBVel);
    }

    char cType = cinfo.type;

    switch (cType) {
    case 2:
        magnitude = UMath::Min(speedchange * 0.033333335f, 1.0f);
        break;

    case 3:
        magnitude = UMath::Min((speedchange - 1.0f) * 0.33333334f, 1.0f);
        break;

    case 1:
        if (speedchange < 5.0f) {
            bool backEnder = false;

            if (them != NULL) {
                IVehicle *theirVehicle;

                if (them->QueryInterface(&theirVehicle)) {
                    UMath::Vector3 myVelocity = iamA ? cinfo.objAVel : cinfo.objBVel;
                    UMath::Vector3 theirVelocity = iamA ? cinfo.objBVel : cinfo.objAVel;

                    UMath::Normalize(myVelocity);
                    UMath::Normalize(theirVelocity);

                    if (UMath::Dot(myVelocity, theirVelocity) > 0.1f) {
                        speedchange = UMath::Max(UMath::Length(cinfo.objAVel), UMath::Length(cinfo.objAVel)) * 0.5f;
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

    float amplitude = UMath::Min(magnitude, 0.5f);
    float time = amplitude;
    EffectBinary &ramp = state.CollisionNoise;

    if (ramp.Time > 0.0f) {
        ramp.MaxTime = UMath::Max(time, ramp.MaxTime);
        ramp.Time = UMath::Max(time, ramp.Time);
    } else {
        ramp.MaxTime = time;
        ramp.Time = time;
    }
}
