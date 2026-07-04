#ifdef EA_PLATFORM_GAMECUBE
#include "eathread/1.1.0/source/gc/eathread_thread_gc.cpp"
#else
#ifdef EA_PLATFORM_PS2
#include "eathread/1.1.0/source/ps2/eathread_thread_ps2.cpp"
#endif
#endif
