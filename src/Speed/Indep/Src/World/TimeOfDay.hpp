#ifndef WORLD_TIME_OF_DAY_H
#define WORLD_TIME_OF_DAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum eTimeOfDay {
    eTOD_NUM_TIMES_OF_DAY = 2,
    eTOD_SUNSET = 1,
    eTOD_MIDDAY = 0,
    eTOD_ERROR = -1,
};

void TickOverTimeOfday();
void ApplyTimeOfDayTickOver();
void SetCurrentTimeOfDay(float tod);
const char *GetTimeOfDaySuffix(eTimeOfDay tod);
bool NeedsSeperateTODStreamingFile(const char *platform_name);
eTimeOfDay GetCurrentTimeOfDay();

#endif
