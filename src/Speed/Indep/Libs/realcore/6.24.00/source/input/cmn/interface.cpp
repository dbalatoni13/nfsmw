#include "../../../include/common/realcore/input.h"
#include "../gc/gc_interface.h"

namespace RealInput {

extern Interface *gInterface;

Interface *Interface::CreateInstance(const ConfigOptions &options) {
    if (gInterface == nullptr) {
        SetAllocator(options.mAllocator);
        gInterface = new GcInterface(options);
    }
    return gInterface;
}

int Interface::AddRef() {
    return 0;
}

int Interface::Release() {
    return 0;
}

void Interface::Update() {}

Device *Interface::GetPad() {
    return nullptr;
}

Device *Interface::GetMouse() {
    return nullptr;
}

Device *Interface::GetKeyboard() {
    return nullptr;
}

Event *Interface::GetEvent() {
    return nullptr;
}

Interface *gInterface = nullptr;

} // namespace RealInput
