/*
 *  A collection of functions to convert to and from various units.
 */
#ifndef CONVERSION_UTIL_H
#define CONVERSION_UTIL_H

#include "types.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

// Credit: Brawltendo

typedef float Angle;

inline Mph MPS2MPH(const Mps _mps_) {
    return _mps_ * 2.23699f;
}

inline Kph MPS2KPH(const Mps _mps_) {
    return _mps_ * 3.6f;
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
    return _deg_ * 0.017453f; //  ((float)M_PI / 180.f); // TODO why doesn't the PI one match?
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

inline Meters METERS2FT(const Meters _meters_) {
    return _meters_ * 3.28080f;
}

// MEDIDO al bit sobre el ELF, y la ronda anterior lo dejo AL REVES: las
// **17** apariciones de 0x3E8E38E4 (`x / 3.6f`) del original viven TODAS en el
// .rodata de zAI (0x803C9858..0x803CA970), que es justo quien llama a esta
// macro; las 10 de 0x3E8E392E (`0.27778f`) estan TODAS en .over y en el
// .rodata del frontend (0x803E5024..0x803EEBF8), donde el fuente escribe
// `* 0.27778f` A MANO (FEPKg_PostRace.cpp, FeLeaderBoard.cpp,
// uiQRTrackSelect.cpp, GSpeedTrap.cpp) y NO llama a KPH2MPS. O sea que la
// "pista" de los cuatro sitios escritos a mano probaba lo contrario de lo que
// se leyo. Verificado con litpos.py: 27 referencias de zAI con el valor
// cambiado.
inline Mps KPH2MPS(Kph x) {
    return x / 3.6f;
}

namespace ConversionUtil {

template <class T, class U>
void Copy4(U &out, const T &in) {
    out.x = in.x;
    out.y = in.y;
    out.z = in.z;
    out.w = in.w;
}

template <class T>
void Scale3(T &v, float scale) {
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
}

template <class T>
T Make3(float x, float y, float z) {
    T v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
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

template <class T, class U>
void RightToLeftVector3(const T &in, U &out) {
    out = Make3<U>(-in.y, in.z, in.x);
}

template <class T, class U>
void RightToLeftVector4(const T &in, U &out) {
    out = Make4<U>(-in.y, in.z, in.x, in.w);
}

template <class T, class U>
void RightToLeftMatrix4(const T &in, U &out) {
    UMath::Vector4 tmp[4];
    UMath::Vector4 &t0 = tmp[0];

    Copy4(t0, in.v1);
    Copy4(tmp[1], in.v2);
    Copy4(tmp[2], in.v0);
    Copy4(tmp[3], in.v3);

    Scale3(t0, -1.0f);

    RightToLeftVector4(t0, out.v0);
    RightToLeftVector4(tmp[1], out.v1);
    RightToLeftVector4(tmp[2], out.v2);
    RightToLeftVector4(tmp[3], out.v3);
}

}; // namespace ConversionUtil

inline float MPH2KPH(const float _mph_) {
    return _mph_ * 1.60931f;
}

inline float LB2KG(const float _lb_) {
    return _lb_ * 0.45359f;
}

#endif
