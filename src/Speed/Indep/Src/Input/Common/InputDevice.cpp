#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

DeviceScalar::DeviceScalar() {
    this->fType = kJoyAxis;
    this->fName = UCrc32::kNull;
    this->fPrevValue = nullptr;
    this->fCurrentValue = nullptr;
}

void DeviceScalar::InitializeDeviceScalar(DeviceScalarType type, const char *name, float *prev_value, float *current_value) {
    this->fType = type;
    this->fName = stringhash32(name);
    this->fPrevValue = prev_value;
    this->fCurrentValue = current_value;
}

IMPLEMENT_FACTORY(InputDevice)

InputDevice::InputDevice(int deviceIndex) : UTL::COM::Object(4) {
    this->fDeviceIndex = deviceIndex;
    this->fControllerCurve = 1.0f;
}

InputDevice::~InputDevice() {}

bool InputDevice::DeviceHasChanged() {
    int numScalars = this->GetNumDeviceScalar();

    for (int i = 0; i < numScalars; i++) {
        if (bAbs(this->fCurrentValues[i] - this->fPrevValues[i]) > 0.03f) {
            return true;
        }
    }

    return false;
}

bool InputDevice::DeviceHasAnyActivity() {
    int numScalars = this->GetNumDeviceScalar();

    for (int i = 0; i < numScalars; i++) {
        if (this->fCurrentValues[i] != 0.0f) {
            return true;
        }
    }

    return false;
}

float *InputDevice::SaveCurrentState() {
    float *save = new float[this->GetNumDeviceScalar()];

    for (int i = 0; i < this->GetNumDeviceScalar(); i++) {
        save[i] = this->fCurrentValues[i];
    }

    return save;
}

void InputDevice::RestoreToState(float *currentState) {
    for (int i = 0; i < this->GetNumDeviceScalar(); i++) {
        this->fCurrentValues[i] = currentState[i];
    }
}
