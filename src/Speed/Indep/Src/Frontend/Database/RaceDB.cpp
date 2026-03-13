#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

#include "types.h"

#include <string.h>

unsigned int GetFECarNameHashFromFEKey(unsigned int fekey);

int CareerPursuitScores::GetValue(ePursuitDetailTypes type) const {
    return Value[type];
}

void HighScoresDatabase::Default() {
    memset(this, 0, sizeof(*this));
}

unsigned int HighScoresDatabase::GetPreviouslyPursuedCarNameHash() const {
    return GetFECarNameHashFromFEKey(PreviouslyPursuedCarFEKey);
}

void HighScoresDatabase::CommitHighScoresPauseQuit() {}
