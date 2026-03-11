#ifndef FENG_FENGINE_H
#define FENG_FENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct FEngine {
    char _pad[0x525c];
    bool bErrorScreenMode; // offset 0x525c
};

#endif
