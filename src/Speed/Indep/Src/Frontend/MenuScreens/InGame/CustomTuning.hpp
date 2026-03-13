#ifndef FRONTEND_MENUSCREENS_INGAME_CUSTOMTUNING_H
#define FRONTEND_MENUSCREENS_INGAME_CUSTOMTUNING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

struct CustomTuningScreen {
    static bool IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path);
    unsigned int GetNameForPath(Physics::Tunings::Path path, bool turbo);
    unsigned int GetHelpForPath(Physics::Tunings::Path path, bool active, bool turbo);
};

#endif
