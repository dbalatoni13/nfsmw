#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

typedef GInfractionManager::InfractionType InfractionType;

#include "Speed/Indep/Src/Generated/Events/EReportInfraction.hpp"

GInfractionManager *GInfractionManager::mObj = nullptr;

GInfractionManager::GInfractionManager()
    : mInfractions(0), //
      mNumThisPursuit(0), //
      mSpeeding(false), //
      mDrivingRecklessly(false), //
      mRacing(false) {}

void GInfractionManager::Init() {
    mObj = new GInfractionManager;
}

void GInfractionManager::PursuitStarted() {
    mInfractions = 0;
    mNumThisPursuit = 0;
    mSpeeding = false;
    mDrivingRecklessly = false;
    mRacing = false;
}

void GInfractionManager::ReportInfraction(InfractionType infraction) {
    bool pursuitRace;

    if (!GRaceStatus::Exists()) {
        return;
    }

    pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
        pursuitRace = true;
    }

    if (!GRaceStatus::Get().GetActivelyRacing() || pursuitRace) {
        if ((mInfractions & infraction) == 0) {
            mInfractions |= infraction;
            new EReportInfraction(infraction);
            GManager::Get().TrackValue("total_infractions", static_cast<float>(GetNumInfractions()));
        }

        mNumThisPursuit++;
    }
}

unsigned int GInfractionManager::GetNumInfractions() {
    unsigned int bits;
    unsigned int count;

    count = 0;
    for (bits = mInfractions; bits != 0; bits >>= 1) {
        count += bits & 1;
    }

    return count;
}

bool GInfractionManager::DidInfractionOccur(InfractionType infraction) {
    return (mInfractions & infraction) != 0;
}
