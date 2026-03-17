#include "LGWheels.hpp"

struct SpringForceParams {
    char offset;
    unsigned char saturation;
    short coefficient;
};

struct ConstantForceParams {
    short magnitude;
    unsigned short direction;
};

struct DamperForceParams {
    short coefficient;
};

struct RoadEffectParams {
    short magnitude;
};

struct SurfaceEffectParams {
    unsigned char type;
    unsigned char magnitude;
    unsigned short period;
};

static inline Wheels *GetWheels(LGWheels *self) {
    return reinterpret_cast<Wheels *>(reinterpret_cast<char *>(self) + 0x828);
}

static inline const Wheels *GetWheels(const LGWheels *self) {
    return reinterpret_cast<const Wheels *>(reinterpret_cast<const char *>(self) + 0x828);
}

static inline SpringForceParams *GetSpringForceParams(LGWheels *self) {
    return reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(self) + 0x167C);
}

static inline const SpringForceParams *GetSpringForceParams(const LGWheels *self) {
    return reinterpret_cast<const SpringForceParams *>(reinterpret_cast<const char *>(self) + 0x167C);
}

static inline ConstantForceParams *GetConstantForceParams(LGWheels *self) {
    return reinterpret_cast<ConstantForceParams *>(reinterpret_cast<char *>(self) + 0x168C);
}

static inline const ConstantForceParams *GetConstantForceParams(const LGWheels *self) {
    return reinterpret_cast<const ConstantForceParams *>(reinterpret_cast<const char *>(self) + 0x168C);
}

static inline DamperForceParams *GetDamperForceParams(LGWheels *self) {
    return reinterpret_cast<DamperForceParams *>(reinterpret_cast<char *>(self) + 0x169C);
}

static inline const DamperForceParams *GetDamperForceParams(const LGWheels *self) {
    return reinterpret_cast<const DamperForceParams *>(reinterpret_cast<const char *>(self) + 0x169C);
}

static inline RoadEffectParams *GetDirtRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16BC);
}

static inline const RoadEffectParams *GetDirtRoadParams(const LGWheels *self) {
    return reinterpret_cast<const RoadEffectParams *>(reinterpret_cast<const char *>(self) + 0x16BC);
}

static inline RoadEffectParams *GetBumpyRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16C4);
}

static inline const RoadEffectParams *GetBumpyRoadParams(const LGWheels *self) {
    return reinterpret_cast<const RoadEffectParams *>(reinterpret_cast<const char *>(self) + 0x16C4);
}

static inline RoadEffectParams *GetSlipperyRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16CC);
}

static inline const RoadEffectParams *GetSlipperyRoadParams(const LGWheels *self) {
    return reinterpret_cast<const RoadEffectParams *>(reinterpret_cast<const char *>(self) + 0x16CC);
}

static inline SurfaceEffectParams *GetSurfaceEffectParams(LGWheels *self) {
    return reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(self) + 0x16D4);
}

static inline const SurfaceEffectParams *GetSurfaceEffectParams(const LGWheels *self) {
    return reinterpret_cast<const SurfaceEffectParams *>(reinterpret_cast<const char *>(self) + 0x16D4);
}

bool LGWheels::IsConnected(int channel) {
    return GetWheels(this)->IsConnected(channel);
}

bool LGWheels::ButtonIsPressed(int channel, unsigned long buttonMask) {
    return GetWheels(this)->ButtonIsPressed(channel, buttonMask);
}

bool LGWheels::PedalsConnected(int channel) {
    return GetWheels(this)->PedalsConnected(channel);
}

void LGWheels::StopSpringForce(int channel) {
    this->StopForce(channel, 0);
}

bool LGWheels::SameSpringForceParams(int channel, char offset, unsigned char saturation, short coefficient) {
    const SpringForceParams &params = GetSpringForceParams(this)[channel];
    return params.offset == offset && params.saturation == saturation && params.coefficient == coefficient;
}

void LGWheels::StopConstantForce(int channel) {
    this->StopForce(channel, 1);
}

bool LGWheels::SameConstantForceParams(int channel, short magnitude, unsigned short direction) {
    const ConstantForceParams &params = GetConstantForceParams(this)[channel];
    return params.magnitude == magnitude && params.direction == direction;
}

void LGWheels::StopDamperForce(int channel) {
    this->StopForce(channel, 2);
}

bool LGWheels::SameDamperForceParams(int channel, short coefficient) {
    return GetDamperForceParams(this)[channel].coefficient == coefficient;
}

void LGWheels::StopDirtRoadEffect(int channel) {
    this->StopForce(channel, 5);
}

bool LGWheels::SameDirtRoadEffectParams(int channel, short magnitude) {
    return GetDirtRoadParams(this)[channel].magnitude == magnitude;
}

void LGWheels::StopBumpyRoadEffect(int channel) {
    this->StopForce(channel, 6);
}

bool LGWheels::SameBumpyRoadEffectParams(int channel, short magnitude) {
    return GetBumpyRoadParams(this)[channel].magnitude == magnitude;
}

void LGWheels::StopSlipperyRoadEffect(int channel) {
    this->StopForce(channel, 7);
}

bool LGWheels::SameSlipperyRoadEffectParams(int channel, short magnitude) {
    return GetSlipperyRoadParams(this)[channel].magnitude == magnitude;
}

void LGWheels::StopSurfaceEffect(int channel) {
    this->StopForce(channel, 8);
}

bool LGWheels::SameSurfaceEffectParams(int channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    const SurfaceEffectParams &params = GetSurfaceEffectParams(this)[channel];
    return params.type == type && params.magnitude == magnitude && params.period == period;
}

void LGWheels::StopCarAirborne(int channel) {
    this->StopForce(channel, 9);
}
