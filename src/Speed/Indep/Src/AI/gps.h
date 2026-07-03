#ifndef AI_GPS_H
#define AI_GPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

void GPS_Disengage();
bool GPS_Engage(const UMath::Vector3 &target /* r4 */, float maxDeviation /* f1 */);
bool GPS_IsEngaged();

#endif
