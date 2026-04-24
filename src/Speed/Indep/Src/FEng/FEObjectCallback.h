#ifndef FENG_FEOBJECTCALLBACK_H
#define FENG_FEOBJECTCALLBACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

struct FEObject;

// total size: 0x4
struct FEObjectCallback {
    inline FEObjectCallback() {}
    inline virtual ~FEObjectCallback() {}
    virtual bool Callback(FEObject* obj) = 0;
};

#endif
