/*
 *  A collection of functions to convert to and from various units.
 */
#ifndef CONVERSION_UTIL_H
#define CONVERSION_UTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cmath>

// Credit: Brawltendo

inline float MPS2MPH(const float _mps_) {
    return _mps_ * 2.23699f;
}

inline float MPH2MPS(const float _mph_) {
    return _mph_ * 0.44703001f;
}

inline float DEG2ANGLE(const float _deg_) {
    return _deg_ / 360.f;
}

inline float ANGLE2DEG(const float _arc_) {
    return _arc_ * 360.f;
}

inline float RAD2ANGLE(const float _rad_) {
    return _rad_ / (float)M_TWOPI;
}

inline float ANGLE2RAD(const float _arc_) {
    return _arc_ * (float)M_TWOPI;
}

inline float DEG2RAD(const float _deg_) {
    return _deg_ * (180.f / M_PI);
}

inline float RAD2DEG(const float _rad_) {
    return _rad_ * ((float)M_PI / 180.f);
}

inline float INCH2METERS(const float _inches_) {
    return _inches_ * 0.0254f;
}

inline float RPS2RPM(const float _rps_) {
    return _rps_ * 9.5492964f;
}

inline float RPM2RPS(const float _rpm_) {
    return _rpm_ / 9.5492964f;
}

inline float LBIN2NM(const float _lbin_) {
    return _lbin_ * 175.1268f;
}

inline float NM2LBIN(const float _nm_) {
    return _nm_ / 175.1268f;
}

#endif
