#ifndef GAMEPLAY_GRACE_H
#define GAMEPLAY_GRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace GRace {

enum Context {
    kRaceContext_QuickRace = 0,
    kRaceContext_TimeTrial = 1,
    kRaceContext_Online = 2,
    kRaceContext_Career = 3,
    kRaceContext_Count = 4,
};

};

#endif
