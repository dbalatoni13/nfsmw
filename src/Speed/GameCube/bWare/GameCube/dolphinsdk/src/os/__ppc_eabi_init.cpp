#include <dolphin/__ppc_eabi_init.h>

extern void __OSFPRInit(void);
extern void __OSPSInit(void);
extern void __OSCacheInit(void);

asm void __init_hardware(void) {
    // clang-format off
  nofralloc
  mfmsr r0
  ori r0, r0, 0x2000
  mtmsr r0

  mflr r31
  bl __OSPSInit
  bl __OSCacheInit
  mtlr r31
  blr
    // clang-format on
}
