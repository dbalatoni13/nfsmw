#ifndef REALCORE_SOURCE_DEBUG_CMN_PRINTI_H
#define REALCORE_SOURCE_DEBUG_CMN_PRINTI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../../../../Src/EAGL4Anim/eagl4supportprintmessage.h"

// Shared print descriptors, with the member types and layout from retail DWARF.
typedef void (*PRINTFUNCTION)(PRINTCHANNEL, const char *);

struct PRINTCHANNELINFO {
    const char *name; // offset 0x0
    bool enabled; // offset 0x4
    bool library_channel; // offset 0x8
};

struct PRINTDEVICEINFO {
    const char *name; // offset 0x0
    PRINTFUNCTION func; // offset 0x4
    bool enabled; // offset 0x8
};

extern PRINTDEVICEINFO PRINTdevicelist[8];

#endif
