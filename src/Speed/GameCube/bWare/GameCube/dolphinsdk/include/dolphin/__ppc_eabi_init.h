#ifndef _DOLPHIN__PPC_EABI_INIT
#define _DOLPHIN__PPC_EABI_INIT

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((section(".init"), naked)) void __init_hardware(void);

void __init_user(void);
void __init_cpp(void);

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN__PPC_EABI_INIT
