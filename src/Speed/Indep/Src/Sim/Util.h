#ifndef SIM_UTIL_H
#define SIM_UTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// Credits: Brawltendo
inline unsigned int UTIL_InterprolateIndex(unsigned int last_index, float value, float limit_min, float limit_max, float &ratio) {
    float value_range = limit_max - limit_min;
    float value_offset = value - limit_min;
    ratio = last_index * value_offset / value_range;
    unsigned int index1 = static_cast<unsigned int>(ratio);
    ratio -= index1;
    return index1;
}

UMath::Matrix4 Util_GenerateMatrix(const UMath::Vector3 &vec, const UMath::Vector3 *InUp);
UMath::Vector3 Util_GenerateCarTensor(const float mass, const float width, const float height, const float length, const UMath::Vector3 &tensorScale);

#endif
