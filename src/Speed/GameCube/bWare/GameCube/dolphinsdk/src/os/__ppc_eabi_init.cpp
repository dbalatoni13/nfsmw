#include <dolphin/__ppc_eabi_init.h>

extern void __OSFPRInit(void);
extern void __OSPSInit(void);
extern void __OSCacheInit(void);

__attribute__((section(".init"), naked)) void __init_hardware() {
  // clang-format off
  __asm__ volatile (
  "mfmsr 0\n"
  "ori 0, 0, 0x2000\n"
  "mtmsr 0\n"
  "mflr 31\n"
  "bl __OSPSInit\n"
  "bl __OSFPRInit\n"
  "bl __OSCacheInit\n"
  "mtlr 31\n"
  );
  // clang-format on
}
