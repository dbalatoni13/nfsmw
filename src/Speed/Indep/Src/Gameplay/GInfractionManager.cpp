#include "GInfractionManager.h"

#include "GManager.h"
#include "GRaceStatus.h"

typedef GInfractionManager::InfractionType InfractionType;

#include "Speed/Indep/Src/Generated/Events/EReportInfraction.hpp"

extern const float TWEAK_SpeedingLimit = 80.5f;
extern const float TWEAK_RacingLimit = 67.0f;
extern const float TWEAK_RecklessDrivingLimit = 89.4f;

GInfractionManager *GInfractionManager::mObj = NULL;

GInfractionManager::GInfractionManager() {
    mInfractions = 0;
    mNumThisPursuit = 0;
    mSpeeding = false;
    mDrivingRecklessly = false;
    mRacing = false;
}

void GInfractionManager::Init() {
    mObj = new ("GInfractionManager", 0) GInfractionManager;
}

void GInfractionManager::PursuitStarted() {
    ClearInfractions();
}

void GInfractionManager::ReportInfraction(InfractionType infraction) {
    if (GRaceStatus::Exists()) {
        bool roaming = GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming;
        bool pursuitRace = false;
        if (GRaceStatus::Get().GetRaceParameters() != NULL && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
            pursuitRace = true;
        }
        if (roaming || pursuitRace) {
            if ((mInfractions & infraction) == 0) {
                mInfractions |= infraction;
                new EReportInfraction(infraction);
                GManager::Get().TrackValue("total_infractions", (int)GetNumInfractions());
            }
            mNumThisPursuit++;
        }
    }
}

unsigned int GInfractionManager::GetNumInfractions() {
    unsigned int infracts = mInfractions;
    unsigned int total = 0;
    while (infracts != 0) {
        total += infracts & 1;
        infracts >>= 1;
    }
    return total;
}

bool GInfractionManager::DidInfractionOccur(InfractionType infraction) {
    return (mInfractions & infraction) != 0;
}
