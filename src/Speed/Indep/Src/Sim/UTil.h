#ifndef SIM_UTIL_H
#define SIM_UTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Credits: Brawltendo
inline unsigned int UTIL_InterprolateIndex(unsigned int last_index, float value, float limit_min, float limit_max, float &ratio) {
    float value_range = limit_max - limit_min;
    float value_offset = value - limit_min;
    ratio = last_index * value_offset / value_range;
    unsigned int index1 = ratio;
    ratio -= index1;
    return index1;
}

#endif
