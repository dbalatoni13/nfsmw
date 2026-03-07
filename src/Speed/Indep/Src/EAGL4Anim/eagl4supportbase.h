#ifndef EAGL4ANIM_EAGL4SUPPORTBASE_H
#define EAGL4ANIM_EAGL4SUPPORTBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4supportdef.h"

namespace EAGL4Internal {

void SetMallocOverride(MallocFunctionType nFunc);
void SetFreeOverride(FreeFunctionType nFunc);

}; // namespace EAGL4Internal

#endif
