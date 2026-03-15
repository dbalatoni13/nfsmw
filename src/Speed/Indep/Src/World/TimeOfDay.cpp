#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

eTimeOfDay gTheTimeOfDay;
static eTimeOfDay desiredTOD;
int TimeOfDaySwapEnable;

const char* GetTimeOfDaySuffix(eTimeOfDay tod) {
    if (tod == eTOD_SUNSET) {
        return "_Sunset";
    }
    if (tod == eTOD_MIDDAY) {
        return "";
    }
    return "THIS_IS_NOT_A_TIME_OF_DAY_SUFFIX";
}

bool NeedsSeperateTODStreamingFile(const char* platform_name) {
    if (bStrICmp(platform_name, "PSX2") == 0 || bStrICmp(platform_name, "XBOX") == 0) {
        return true;
    }
    return false;
}

eTimeOfDay GetCurrentTimeOfDay() {
    return gTheTimeOfDay;
}

void TickOverTimeOfday() {
}

void ApplyTimeOfDayTickOver() {
    if (TimeOfDaySwapEnable == 0) {
        return;
    }
    if (gTheTimeOfDay == desiredTOD) {
        return;
    }
    gTheTimeOfDay = desiredTOD;
}

void SetCurrentTimeOfDay(float tod) {
}
