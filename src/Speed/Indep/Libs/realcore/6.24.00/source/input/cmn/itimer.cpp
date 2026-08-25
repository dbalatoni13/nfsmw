#include "../../../include/common/realcore/input.h"

extern int TIMER_gettick();

namespace RealInput {

unsigned int ITimer::GetTime() {
    return TIMER_gettick();
}

} // namespace RealInput
