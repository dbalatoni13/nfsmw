#ifdef EA_PLATFORM_GAMECUBE
#include "gc/eathread_semaphore_gc.cpp"
#else
#ifdef EA_PLATFORM_PS2
#include "ps2/eathread_semaphore_ps2.cpp"
#endif
#endif
