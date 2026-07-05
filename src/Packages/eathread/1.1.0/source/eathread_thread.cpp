#ifdef EA_PLATFORM_GAMECUBE
#include "gc/eathread_thread_gc.cpp"
#else
#ifdef EA_PLATFORM_PS2
#include "ps2/eathread_thread_ps2.cpp"
#endif
#endif
