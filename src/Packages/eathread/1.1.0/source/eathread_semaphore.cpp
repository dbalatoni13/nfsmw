#ifdef EA_PLATFORM_GAMECUBE
#include "eathread/1.1.0/source/gc/eathread_semaphore_gc.cpp"
#else
#ifdef EA_PLATFORM_PS2
#include "eathread/1.1.0/source/ps2/eathread_semaphore_ps2.cpp"
#endif
#endif
