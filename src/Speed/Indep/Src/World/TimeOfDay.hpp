#ifndef WORLD_TIME_OF_DAY_H
#define WORLD_TIME_OF_DAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/World.hpp"

void TickOverTimeOfday();
void ApplyTimeOfDayTickOver();
void SetCurrentTimeOfDay(float tod);
const char* GetTimeOfDaySuffix(eTimeOfDay tod);
bool NeedsSeperateTODStreamingFile(const char* platform_name);

#endif
