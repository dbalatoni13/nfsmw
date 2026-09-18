#include "../../../include/common/realcore/input.h"

namespace RealInput {

Event::Event()
    : mpDevice(nullptr)
    , mType(TYPE_RELEASED)
    , mObject(OBJECT_UNKNOWN)
    , mObjectIndex(0)
    , mTimestamp(0) {
}

Event::~Event() {}

} // namespace RealInput
