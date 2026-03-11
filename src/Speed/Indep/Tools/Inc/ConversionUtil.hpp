/*
 *  A collection of functions to convert to and from various units.
 */
#ifndef CONVERSION_UTIL_H
#define CONVERSION_UTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

// Credit: Brawltendo

typedef float Angle;

typedef float Rpm;
typedef float Mps;
typedef float Mps2;
typedef float Mph;
typedef float Kph;
typedef float Meters;
typedef float Kg;
typedef float FtLbs;
typedef float Hp;
typedef float Nm;
typedef float Newtons;
typedef float Seconds;
typedef float Psi;
typedef float Lbs;
typedef float Kgm2;
typedef float Radians;

inline Mph MPS2MPH(const Mps _mps_) {
    return _mps_ * 2.23699f;
}

inline Mps MPH2MPS(const Mph _mph_) {
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
    return _deg_ * ((float)M_PI / 180.f);
}

inline float RAD2DEG(const float _rad_) {
    return _rad_ * (180.f / (float)M_PI);
}

inline float INCH2METERS(const float _inches_) {
    return _inches_ * 0.0254f;
}

inline Rpm RPS2RPM(const float _rps_) {
    return _rps_ * 9.549296f; // TODO problems on PS2
}

inline float RPM2RPS(const Rpm _rpm_) {
    return _rpm_ / 9.5492958f; // TODO problems on PS2
}

inline Nm LBIN2NM(const float _lbin_) {
    return _lbin_ * 175.1268f;
}

inline float NM2LBIN(const Nm _nm_) {
    return _nm_ / 175.1268f;
}

inline Nm FTLB2NM(const FtLbs _ftlb_) {
    return _ftlb_ * 1.3558f;
}

inline float FTLB2HP(const float _ftlb, const float _rpm) {
    return (_ftlb * _rpm) / 5252.0f;
}

inline Hp NM2HP(const Nm _nm, const Rpm _rpm) {
    return _nm * 0.7376f * _rpm / 5252.0f;
}

inline Mps KPH2MPS(Kph x) {
    return x / 3.6f;
}

namespace ConversionUtil {

template <class T1, class T2>
void Copy4(T2 &out, const T1 &in) {
    out.x = in.x;
    out.y = in.y;
    out.z = in.z;
    out.w = in.w;
}

template <class T>
void Scale3(T &v, float s) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
}

template <class T>
T Make4(float x, float y, float z, float w) {
    T v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

template <class T>
T Make3(float x, float y, float z) {
    T v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

template <class T1, class T2>
void RightToLeftVector4(const T1 &in, T2 &out) {
    out = Make4<T2>(-in.y, in.z, in.x, in.w);
}

template <class T1, class T2>
void RightToLeftVector3(const T1 &in, T2 &out) {
    out = Make3<T2>(-in.y, in.z, in.x);
}

template <class T1, class T2>
void RightToLeftMatrix4(const T1 &in, T2 &out) {
    T2 tmp;
    Copy4(tmp[0], in[0]);
    Copy4(tmp[1], in[1]);
    Copy4(tmp[2], in[2]);
    Copy4(tmp[3], in[3]);
    Scale3(tmp[0], -1.0f);
    RightToLeftVector4(tmp[0], out[0]);
    RightToLeftVector4(tmp[1], out[1]);
    RightToLeftVector4(tmp[2], out[2]);
    RightToLeftVector4(tmp[3], out[3]);
}

} // namespace ConversionUtil

#endif
