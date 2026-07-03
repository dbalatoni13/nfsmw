#include "TimeOfDay.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

eTimeOfDay gTheTimeOfDay = eTOD_MIDDAY;
static eTimeOfDay desiredTOD = eTOD_MIDDAY;

const char *GetTimeOfDaySuffix(eTimeOfDay tod) {
    if (tod == eTOD_SUNSET) {
        return "_Sunset";
    }
    if (tod == eTOD_MIDDAY) {
        return "";
    }
    return "THIS_IS_NOT_A_TIME_OF_DAY_SUFFIX";
}

bool NeedsSeperateTODStreamingFile(const char *platform_name) {
    if (bStrEqual(platform_name, "PSX2") || bStrEqual(platform_name, "XBOX")) {
        return true;
    }
    return false;
}

eTimeOfDay GetCurrentTimeOfDay() {
    return gTheTimeOfDay;
}

void TickOverTimeOfday() {}

void ApplyTimeOfDayTickOver() {
    if (TimeOfDaySwapEnable == 0) {
        return;
    }
    if (gTheTimeOfDay == desiredTOD) {
        return;
    }
    gTheTimeOfDay = desiredTOD;
}

void SetCurrentTimeOfDay(float tod) {}
