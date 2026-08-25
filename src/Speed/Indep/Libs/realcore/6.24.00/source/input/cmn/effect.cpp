#include "../../../include/common/realcore/input.h"

namespace RealInput {

Effect::Effect() {}

Effect::~Effect() {}

void Effect::Start() {}

void Effect::Stop() {}

Effect::Status Effect::GetStatus() {
    return STATUS_STOPPED;
}

Device *Effect::GetDevice() {
    return nullptr;
}

void Effect::GetInfo(Info *info) {}

void Effect::SetInfo(Info *info) {}

} // namespace RealInput
