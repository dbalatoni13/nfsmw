#ifndef WORLD_WTRIGGERLIST_H
#define WORLD_WTRIGGERLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

class WTrigger;

class WTriggerList : public UTL::Std::vector<WTrigger *, _type_vector> {};

#endif
