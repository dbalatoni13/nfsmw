#include "Speed/Indep/Src/Gameplay/GSpeedTrap.h"

bool GSpeedTrap::IsFlagSet(unsigned int mask) const {
    return (mFlags & mask) != 0;
}
bool GSpeedTrap::IsFlagClear(unsigned int mask) const {
    return (mFlags & mask) == 0;
}
