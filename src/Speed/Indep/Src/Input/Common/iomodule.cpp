#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"

IOModule &IOModule::GetIOModule() {
    static IOModule instance;
    return instance;
}

bool IOModule::EnableUpdating(bool enable) {
    fUpdateEnabled = enable;

    return enable;
}

IOModule::IOModule() {
    fNumDevices = 0;
    fDisconnected = 0;
    fUpdateEnabled = false;

    LastTimeDeviceChanged = RealTimer;
}

bool IOModule::CheckUnplugged() {
    int requiredports = 0;

    for (ActionQueue *const *iter = ActionQueue::GetList().begin(); iter != ActionQueue::GetList().end(); iter++) {
        ActionQueue *q = *iter;
        int port = q->GetPort();

        if (port >= 0 && fDevices[port] != NULL) {
            if (q->IsRequired() && q->IsEnabled()) {
                requiredports |= 1 << port;
            }
        }
    }

    int disconnected = 0;

    for (int port = 0; port < fNumDevices; port++) {
        InputDevice *device = fDevices[port];

        if (device != NULL) {
            if (!device->IsConnected()) {
                disconnected |= 1 << port;
            }
        }
    }

    bool needs_required_ports = requiredports != 0;
    bool had_required_ports = (fDisconnected & requiredports) == 0;
    bool has_required_ports = (disconnected & requiredports) == 0;

    bool flush = false;

    if (needs_required_ports && had_required_ports != has_required_ports) {
        flush = true;
    }

    for (ActionQueue *const *iter = ActionQueue::GetList().begin(); iter != ActionQueue::GetList().end(); iter++) {
        ActionQueue *q = *iter;
        int port = q->GetPort();

        if (port >= 0 && fDevices[port] != NULL) {
            if (flush && q->IsEnabled()) {
                q->IO_Flush();
            }

            if ((disconnected >> port) & 1) {
                q->IO_SetConnected(false);
            } else {
                q->IO_SetConnected(true);
            }
        }
    }

    if (flush) {
        for (int i = 0; i < fNumDevices; i++) {
            InputDevice *device = fDevices[i];

            if (device != NULL) {
                for (int j = 0; j < device->GetNumDeviceScalar(); j++) {
                    DeviceScalar *button = device->GetDeviceScalar(i);

                    if (button != NULL) {
                        button->OverwriteValue(0.0f);
                    }
                }
            }
        }
    }

    fDisconnected = disconnected;

    return needs_required_ports ? !has_required_ports : false;
}

void IOModule::PollDevices() {
    for (int i = 0; i < this->fNumDevices; i++) {
        InputDevice *device = this->fDevices[i];
        if (device != 0) {
            device->PollDevice();
        }
    }
}

void IOModule::Update() {
    if (fUpdateEnabled) {
        ActionQueue::BeginJoylogFrame();

        if (!Joylog::IsReplaying()) {
            PollDevices();
            UpdateAllDevices();
        }

        ActionQueue::EndJoylogFrame();
    }
}

void IOModule::CreateDevices() {
    this->fDisconnected = 0;
    for (int i = 0; i <= 1; i++) {
        InputDevice *newDevice = InputDevice::CreateInstance(UCrc32("GameDevice"), i);
        if (newDevice != 0) {
            newDevice->Initialize();
            this->fDevices[this->fNumDevices] = newDevice;
            this->fDisconnected |= (1 << i);
        } else {
            this->fDevices[this->fNumDevices] = newDevice;
        }
        this->fNumDevices++;
    }

    InputDevice *mse = InputDevice::CreateInstance(UCrc32("MouseDevice"), this->fNumDevices);
    if (mse != 0) {
        this->fDevices[this->fNumDevices] = mse;
        this->fDisconnected |= (1 << this->fNumDevices);
        this->fNumDevices++;
    }

    InputDevice *keybrd = InputDevice::CreateInstance(UCrc32("KeyboardDevice"), this->fNumDevices);
    if (keybrd != 0) {
        this->fDevices[this->fNumDevices] = keybrd;
        this->fDisconnected |= (1 << this->fNumDevices);
        this->fNumDevices++;
    }
}

void IOModule::UpdateAllDevices() {
    if (HaveAnyDevicesChanged()) {
        LastTimeDeviceChanged = RealTimer;
    }

    bool required_only = CheckUnplugged();

    for (int i = 0; i < fNumDevices; i++) {
        InputDevice *device = fDevices[i];
    }

    for (ActionQueue *const *iter = ActionQueue::GetList().begin(); iter != ActionQueue::GetList().end(); iter++) {
        ActionQueue *q = *iter;

        if (!required_only || q->IsRequired()) {
            q->IO_UpdateFromDevice();
        }
    }
}

bool IOModule::HaveAnyDevicesChanged() {
    for (int i = 0; i < this->fNumDevices; i++) {
        InputDevice *device = this->fDevices[i];
        if (device != 0) {
            if (device->IsConnected()) {
                if (device->DeviceHasChanged()) {
                    return true;
                }
            }
        }
    }
    return false;
}

void IOModule::Initialize() {
    CreateDevices();
}

bool IsJoystickTypeWheel(JoystickPort port) {
    if (port < 0) {
        return false;
    }
    if (port >= IOModule::GetIOModule().GetNumDevices()) {
        return false;
    }
    InputDevice *device = IOModule::GetIOModule().GetDevice(port);
    if (device == 0) {
        return false;
    }
    if (device->IsWheel()) {
        return true;
    }
    return false;
}
