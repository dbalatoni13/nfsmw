#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os.h>
#endif

void DEBUG_break() {
#ifdef EA_PLATFORM_GAMECUBE
    OSPanic("", 0, "");
#endif
}
