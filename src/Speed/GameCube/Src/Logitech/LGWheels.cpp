#include "LGWheels.hpp"

#include <string.h>

extern "C" {
void LGInit();
}


static const char kPlayForceError[] = "ERROR: trying to play a force on channel %d but no wheel opened.
";

LGWheels::LGWheels() {
    {
        int ii;

        LGInit();
        OverallGain = 0xFF;

        for (ii = 0; ii < 4; ii++) {
            InitVars(ii);
        }
    }
}

void LGWheels::InitVars(long channel) {
    int ii;

    IsAirborne[channel] = 0;
    damperWasPlaying[channel] = 0;
    springWasPlaying[channel] = 0;

    for (ii = 0; ii < 8; ii++) {
        condition.EffectID[channel][ii] = static_cast<unsigned long>(-1);
        condition.Playing[channel][ii] = 0;
        constant.EffectID[channel][ii] = static_cast<unsigned long>(-1);
        constant.Playing[channel][ii] = 0;
        periodic.EffectID[channel][ii] = static_cast<unsigned long>(-1);
        periodic.Playing[channel][ii] = 0;
        ramp.EffectID[channel][ii] = static_cast<unsigned long>(-1);
        ramp.Playing[channel][ii] = 0;
    }
}

void LGWheels::ReadAll() {
    short wheelUnplugged;

    wheelUnplugged = wheels.ReadAll();
    memcpy(this, &wheels, sizeof(LGPosition) * 4);
    if (wheelUnplugged != -1) {
        InitVars(wheelUnplugged);
    }
}

void LGWheels::StopForce(long channel, long forceType) {
    switch (forceType) {
    case 0:
        if (IsPlaying(channel, 0)) {
            condition.Stop(channel, 0);
        }
        break;
    case 1:
        if (IsPlaying(channel, 1)) {
            constant.Stop(channel, 0);
        }
        break;
    case 2:
        if (IsPlaying(channel, 2)) {
            condition.Stop(channel, 1);
        }
        break;
    case 3:
        if (IsPlaying(channel, 3)) {
            constant.Stop(channel, 1);
        }
        break;
    case 4:
        if (IsPlaying(channel, 4)) {
            periodic.Stop(channel, 0);
        }
        break;
    case 5:
        if (IsPlaying(channel, 5)) {
            periodic.Stop(channel, 1);
        }
        break;
    case 6:
        if (IsPlaying(channel, 6)) {
            periodic.Stop(channel, 2);
        }
        break;
    case 7:
        if (IsPlaying(channel, 7)) {
            condition.Stop(channel, 2);
        }
        if (damperWasPlaying[channel]) {
            PlayDamperForce(channel, DamperForceParams[channel].coefficient);
            condition.Playing[channel][1] = 1;
            damperWasPlaying[channel] = 0;
        }
        if (springWasPlaying[channel]) {
            PlaySpringForce(channel, SpringForceParams[channel].offset, SpringForceParams[channel].saturation, SpringForceParams[channel].coefficient);
            condition.Playing[channel][0] = 1;
            springWasPlaying[channel] = 0;
        }
        break;
    case 8:
        if (IsPlaying(channel, 8)) {
            periodic.Stop(channel, 3);
        }
        break;
    case 9:
        if (IsPlaying(channel, 9)) {
            IsAirborne[channel] = 0;
            if (wasPlayingBeforeAirborne[channel][0] == 1) {
                PlaySpringForce(channel, SpringForceParams[channel].offset, SpringForceParams[channel].saturation, SpringForceParams[channel].coefficient);
            }
            if (wasPlayingBeforeAirborne[channel][1] == 1) {
                PlayConstantForce(channel, ConstantForceParams[channel].magnitude, ConstantForceParams[channel].direction);
            }
            if (wasPlayingBeforeAirborne[channel][2] == 1) {
                PlayDamperForce(channel, DamperForceParams[channel].coefficient);
            }
            if (wasPlayingBeforeAirborne[channel][5] == 1) {
                PlayDirtRoadEffect(channel, static_cast<unsigned char>(DirtRoadParams[channel].magnitude));
            }
            if (wasPlayingBeforeAirborne[channel][6] == 1) {
                PlayBumpyRoadEffect(channel, static_cast<unsigned char>(BumpyRoadParams[channel].magnitude));
            }
            if (wasPlayingBeforeAirborne[channel][7] == 1) {
                PlaySlipperyRoadEffect(channel, SlipperyRoadParams[channel].magnitude);
            }
            if (wasPlayingBeforeAirborne[channel][8] == 1) {
                PlaySurfaceEffect(channel, SurfaceEffectParams[channel].type, SurfaceEffectParams[channel].magnitude, SurfaceEffectParams[channel].period);
            }
            {
                int jj;

                for (jj = 0; jj < 10; jj++) {
                    wasPlayingBeforeAirborne[channel][jj] = 0;
                }
            }
        }
        break;
    }
}

bool LGWheels::IsConnected(long channel) {
    return wheels.IsConnected(channel);
}

bool LGWheels::IsPlaying(long channel, long forceType) {
    switch (forceType) {
    case 0:
        if (condition.Playing[channel][0] != 0) {
            return true;
        }
        break;
    case 1:
        if (constant.Playing[channel][0] != 0) {
            return true;
        }
        break;
    case 2:
        if (condition.Playing[channel][1] != 0) {
            return true;
        }
        break;
    case 3:
        if (constant.Playing[channel][1] != 0) {
            return true;
        }
        break;
    case 4:
        if (periodic.Playing[channel][0] != 0) {
            return true;
        }
        break;
    case 5:
        if (periodic.Playing[channel][1] != 0) {
            return true;
        }
        break;
    case 6:
        if (periodic.Playing[channel][2] != 0) {
            return true;
        }
        break;
    case 7:
        if (condition.Playing[channel][2] != 0) {
            return true;
        }
        break;
    case 8:
        if (periodic.Playing[channel][3] != 0) {
            return true;
        }
        break;
    case 9:
        if (IsAirborne[channel] == 1) {
            return true;
        }
        break;
    }

    return false;
}

bool LGWheels::ButtonIsPressed(long channel, unsigned long buttonMask) {
    return wheels.ButtonIsPressed(channel, buttonMask);
}

bool LGWheels::PedalsConnected(long channel) {
    return wheels.PedalsConnected(channel);
}

void LGWheels::PlayAutoCalibAndSpringForce(long channel) {
    if (wheels.IsConnected(channel) && IsAirborne[channel] == 0) {
        if (periodic.EffectID[channel][4] == static_cast<unsigned long>(-1)) {
            periodic.DownloadForce(channel, 4, wheels.WheelHandles[channel], 3, 2200, 0, 180, 90, 2200, 0, 0, 0, 0, 0, 0);
            periodic.Start(channel, 4);
        }

        if (condition.EffectID[channel][0] == static_cast<unsigned long>(-1)) {
            condition.DownloadForce(channel, 0, wheels.WheelHandles[channel], 7, static_cast<unsigned long>(-1), 2200, 0, 0, 180, 180, 180, 180);
            condition.Start(channel, 0);
        }
    }
}

void LGWheels::PlaySpringForce(long channel, signed char offset, unsigned char saturation, short coefficient) {
    int ret;

    ret = 0;
    if (condition.Playing[channel][2] != 0) {
        return;
    }

    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel] != 0) {
            return;
        }

        if (condition.Playing[channel][0] != 0) {
            if (SameSpringForceParams(channel, offset, saturation, coefficient)) {
                return;
            }

            ret = condition.UpdateForce(channel, 0, 7, static_cast<unsigned long>(-1), 0, offset, 0, saturation, saturation, coefficient, coefficient);
            if (ret < 0) {
                return;
            }

            SpringForceParams[channel].offset = offset;
            SpringForceParams[channel].saturation = saturation;
            SpringForceParams[channel].coefficient = coefficient;
            return;
        }

        if (condition.EffectID[channel][0] == static_cast<unsigned long>(-1)) {
            ret = condition.DownloadForce(channel, 0, wheels.WheelHandles[channel], 7, static_cast<unsigned long>(-1), 0, offset, 0, saturation, saturation, coefficient, coefficient);
            if (ret >= 0) {
                SpringForceParams[channel].offset = offset;
                SpringForceParams[channel].saturation = saturation;
                SpringForceParams[channel].coefficient = coefficient;
            }

            condition.Start(channel, 0);
        } else if (SameSpringForceParams(channel, offset, saturation, coefficient)) {
            condition.Start(channel, 0);
        } else {
            ret = condition.UpdateForce(channel, 0, 7, static_cast<unsigned long>(-1), 0, offset, 0, saturation, saturation, coefficient, coefficient);
            if (ret >= 0) {
                SpringForceParams[channel].offset = offset;
                SpringForceParams[channel].saturation = saturation;
                SpringForceParams[channel].coefficient = coefficient;
            }

            condition.Start(channel, 0);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopSpringForce(long channel) {
    this->StopForce(channel, 0);
}

bool LGWheels::SameSpringForceParams(long channel, signed char offset, unsigned char saturation, short coefficient) {
    return SpringForceParams[channel].offset == offset && SpringForceParams[channel].saturation == saturation &&
           SpringForceParams[channel].coefficient == coefficient;
}

void LGWheels::PlayConstantForce(long channel, short magnitude, unsigned short direction) {
    int ret;

    ret = 0;
    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel]) {
            return;
        }

        if (constant.Playing[channel][0] != 0) {
            if (SameConstantForceParams(channel, magnitude, direction)) {
                return;
            }

            ret = constant.UpdateForce(channel, 0, static_cast<unsigned long>(-1), 0, magnitude, direction, 0, 0, 0, 0);
            if (ret < 0) {
                return;
            }

            ConstantForceParams[channel].magnitude = magnitude;
            ConstantForceParams[channel].direction = direction;
            return;
        }

        if (constant.EffectID[channel][0] == static_cast<unsigned long>(-1)) {
            ret = constant.DownloadForce(channel, 0, wheels.WheelHandles[channel], static_cast<unsigned long>(-1), 0, magnitude, direction, 0, 0, 0, 0);
            if (ret >= 0) {
                ConstantForceParams[channel].magnitude = magnitude;
                ConstantForceParams[channel].direction = direction;
            }

            constant.Start(channel, 0);
        } else if (SameConstantForceParams(channel, magnitude, direction)) {
            constant.Start(channel, 0);
        } else {
            ret = constant.UpdateForce(channel, 0, static_cast<unsigned long>(-1), 0, magnitude, direction, 0, 0, 0, 0);
            if (ret >= 0) {
                ConstantForceParams[channel].magnitude = magnitude;
                ConstantForceParams[channel].direction = direction;
            }

            constant.Start(channel, 0);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopConstantForce(long channel) {
    this->StopForce(channel, 1);
}

bool LGWheels::SameConstantForceParams(long channel, short magnitude, unsigned short direction) {
    return ConstantForceParams[channel].magnitude == magnitude && ConstantForceParams[channel].direction == direction;
}

void LGWheels::PlayDamperForce(long channel, short coefficient) {
    int ret;

    ret = 0;
    if (condition.Playing[channel][2] != 0) {
        return;
    }

    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel] != 0) {
            return;
        }

        if (condition.Playing[channel][1] != 0) {
            if (SameDamperForceParams(channel, coefficient)) {
                return;
            }

            ret = condition.UpdateForce(channel, 1, 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, coefficient, coefficient);
            if (ret < 0) {
                return;
            }

            DamperForceParams[channel].coefficient = coefficient;
            return;
        }

        if (condition.EffectID[channel][1] == static_cast<unsigned long>(-1)) {
            ret = condition.DownloadForce(channel, 1, wheels.WheelHandles[channel], 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, coefficient, coefficient);
            if (ret >= 0) {
                DamperForceParams[channel].coefficient = coefficient;
            }

            condition.Start(channel, 1);
        } else if (SameDamperForceParams(channel, coefficient)) {
            condition.Start(channel, 1);
        } else {
            ret = condition.UpdateForce(channel, 1, 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, coefficient, coefficient);
            if (ret >= 0) {
                DamperForceParams[channel].coefficient = coefficient;
            }

            condition.Start(channel, 1);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopDamperForce(long channel) {
    this->StopForce(channel, 2);
}

bool LGWheels::SameDamperForceParams(long channel, short coefficient) {
    return DamperForceParams[channel].coefficient == coefficient;
}

void LGWheels::PlayFrontalCollisionForce(long channel, unsigned char magnitude) {
    int ret;
    Periodic *c;

    ret = 0;
    if (wheels.IsConnected(channel)) {
        if (periodic.Playing[channel][0] != 0) {
            c = &periodic;

            if (!SameFrontalCollisionForceParams(channel, magnitude)) {
                ret = c->UpdateForce(channel, 0, 3, 150, 0, magnitude, 90, 75, 0, 0, 0, 20, 0, 0);
                if (ret >= 0) {
                    FrontalCollisionParams[channel].magnitude = magnitude;
                }
            }

            c->Start(channel, 0);
            return;
        }

        c = &periodic;

        if (c->EffectID[channel][0] == static_cast<unsigned long>(-1)) {
            ret = c->DownloadForce(channel, 0, wheels.WheelHandles[channel], 3, 150, 0, magnitude, 90, 75, 0, 0, 0, 20, 0, 0);
            if (ret >= 0) {
                FrontalCollisionParams[channel].magnitude = magnitude;
            }

            c->Start(channel, 0);
        } else if (SameFrontalCollisionForceParams(channel, magnitude)) {
            c->Start(channel, 0);
        } else {
            ret = c->UpdateForce(channel, 0, 3, 150, 0, magnitude, 90, 75, 0, 0, 0, 20, 0, 0);
            if (ret >= 0) {
                FrontalCollisionParams[channel].magnitude = magnitude;
            }

            c->Start(channel, 0);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

bool LGWheels::SameFrontalCollisionForceParams(long channel, short magnitude) {
    return FrontalCollisionParams[channel].magnitude == magnitude;
}

void LGWheels::PlayDirtRoadEffect(long channel, unsigned char magnitude) {
    int ret;

    ret = 0;
    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel]) {
            return;
        }

        if (periodic.Playing[channel][1] != 0) {
            if (SameDirtRoadEffectParams(channel, magnitude)) {
                return;
            }

            ret = periodic.UpdateForce(channel, 1, 2, static_cast<unsigned long>(-1), 0, magnitude, 90, 65, 0, 0, 0, 0, 0, 0);
            if (ret < 0) {
                return;
            }

            DirtRoadParams[channel].magnitude = magnitude;
            return;
        }

        if (periodic.EffectID[channel][1] == static_cast<unsigned long>(-1)) {
            ret = periodic.DownloadForce(
                channel, 1, wheels.WheelHandles[channel], 2, static_cast<unsigned long>(-1), 0, magnitude, 90,
                65, 0, 0, 0, 0, 0, 0);
            if (ret >= 0) {
                DirtRoadParams[channel].magnitude = magnitude;
            }

            periodic.Start(channel, 1);
        } else if (SameDirtRoadEffectParams(channel, magnitude)) {
            periodic.Start(channel, 1);
        } else {
            ret = periodic.UpdateForce(channel, 1, 2, static_cast<unsigned long>(-1), 0, magnitude, 90, 65, 0, 0, 0, 0, 0, 0);
            if (ret >= 0) {
                DirtRoadParams[channel].magnitude = magnitude;
            }

            periodic.Start(channel, 1);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopDirtRoadEffect(long channel) {
    this->StopForce(channel, 5);
}

bool LGWheels::SameDirtRoadEffectParams(long channel, short magnitude) {
    return DirtRoadParams[channel].magnitude == magnitude;
}

void LGWheels::PlayBumpyRoadEffect(long channel, unsigned char magnitude) {
    int ret;

    ret = 0;
    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel]) {
            return;
        }

        if (periodic.Playing[channel][2] != 0) {
            if (SameBumpyRoadEffectParams(channel, magnitude)) {
                return;
            }

            ret = periodic.UpdateForce(channel, 2, 3, static_cast<unsigned long>(-1), 0, magnitude, 90, 100, 0, 0, 0, 0, 0, 0);
            if (ret < 0) {
                return;
            }

            BumpyRoadParams[channel].magnitude = magnitude;
            return;
        }

        if (periodic.EffectID[channel][2] == static_cast<unsigned long>(-1)) {
            ret = periodic.DownloadForce(
                channel, 2, wheels.WheelHandles[channel], 3, static_cast<unsigned long>(-1), 0, magnitude,
                90, 100, 0, 0, 0, 0, 0, 0);
            if (ret >= 0) {
                BumpyRoadParams[channel].magnitude = magnitude;
            }

            periodic.Start(channel, 2);
        } else if (SameBumpyRoadEffectParams(channel, magnitude)) {
            periodic.Start(channel, 2);
        } else {
            ret = periodic.UpdateForce(channel, 2, 3, static_cast<unsigned long>(-1), 0, magnitude, 90, 100, 0, 0, 0, 0, 0, 0);
            if (ret >= 0) {
                BumpyRoadParams[channel].magnitude = magnitude;
            }

            periodic.Start(channel, 2);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopBumpyRoadEffect(long channel) {
    this->StopForce(channel, 6);
}

bool LGWheels::SameBumpyRoadEffectParams(long channel, short magnitude) {
    return BumpyRoadParams[channel].magnitude == magnitude;
}

void LGWheels::PlaySlipperyRoadEffect(long channel, short magnitude) {
    int ret;

    ret = 0;
    if (IsPlaying(channel, 2)) {
        StopDamperForce(channel);
        condition.Playing[channel][1] = 0;
        damperWasPlaying[channel] = 1;
    }

    if (IsPlaying(channel, 0)) {
        StopSpringForce(channel);
        condition.Playing[channel][0] = 0;
        springWasPlaying[channel] = 1;
    }

    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel] != 0) {
            return;
        }

        if (condition.Playing[channel][2] != 0) {
            if (SameSlipperyRoadEffectParams(channel, magnitude)) {
                return;
            }

            ret = condition.UpdateForce(channel, 2, 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
            if (ret < 0) {
                return;
            }

            SlipperyRoadParams[channel].magnitude = magnitude;
            return;
        }

        if (condition.EffectID[channel][2] == static_cast<unsigned long>(-1)) {
            ret = condition.DownloadForce(channel, 2, wheels.WheelHandles[channel], 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
            if (ret >= 0) {
                SlipperyRoadParams[channel].magnitude = magnitude;
            }

            condition.Start(channel, 2);
        } else if (SameSlipperyRoadEffectParams(channel, magnitude)) {
            condition.Start(channel, 2);
        } else {
            ret = condition.UpdateForce(channel, 2, 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
            if (ret >= 0) {
                SlipperyRoadParams[channel].magnitude = magnitude;
            }

            condition.Start(channel, 2);
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopSlipperyRoadEffect(long channel) {
    this->StopForce(channel, 7);
}

bool LGWheels::SameSlipperyRoadEffectParams(long channel, short magnitude) {
    return SlipperyRoadParams[channel].magnitude == magnitude;
}

void LGWheels::PlaySurfaceEffect(long channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    int ret = 0;

    if (wheels.IsConnected(channel)) {
        if (IsAirborne[channel] != 0) {
            return;
        }

        if (periodic.Playing[channel][3] != 0) {
            if (SameSurfaceEffectParams(channel, type, magnitude, period)) {
                return;
            }

            if (type != SurfaceEffectParams[channel].type) {
                periodic.Destroy(channel, 3);
                ret = periodic.DownloadForce(channel, 3, wheels.WheelHandles[channel], type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
                periodic.Start(channel, 3);
            } else {
                ret = periodic.UpdateForce(channel, 3, type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
            }

            if (ret >= 0) {
                SurfaceEffectParams[channel].type = type;
                SurfaceEffectParams[channel].magnitude = magnitude;
                SurfaceEffectParams[channel].period = period;
            }
            return;
        }

        if (periodic.EffectID[channel][3] == static_cast<unsigned long>(-1)) {
            ret = periodic.DownloadForce(channel, 3, wheels.WheelHandles[channel], type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
            if (ret >= 0) {
                SurfaceEffectParams[channel].type = type;
                SurfaceEffectParams[channel].magnitude = magnitude;
                SurfaceEffectParams[channel].period = period;
            }
            periodic.Start(channel, 3);
            return;
        }

        if (SameSurfaceEffectParams(channel, type, magnitude, period)) {
            periodic.Start(channel, 3);
            return;
        }

        if (type != SurfaceEffectParams[channel].type) {
            periodic.Destroy(channel, 3);
            ret = periodic.DownloadForce(channel, 3, wheels.WheelHandles[channel], type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
        } else {
            ret = periodic.UpdateForce(channel, 3, type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
        }

        if (ret >= 0) {
            SurfaceEffectParams[channel].type = type;
            SurfaceEffectParams[channel].magnitude = magnitude;
            SurfaceEffectParams[channel].period = period;
        }

        periodic.Start(channel, 3);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopSurfaceEffect(long channel) {
    this->StopForce(channel, 8);
}

bool LGWheels::SameSurfaceEffectParams(long channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    return SurfaceEffectParams[channel].type == type && SurfaceEffectParams[channel].magnitude == magnitude &&
           SurfaceEffectParams[channel].period == period;
}

void LGWheels::PlayCarAirborne(long channel) {
    if (wheels.IsConnected(channel)) {
        IsAirborne[channel] = 1;
        if (IsPlaying(channel, 0)) {
            StopSpringForce(channel);
            wasPlayingBeforeAirborne[channel][0] = 1;
        }
        if (IsPlaying(channel, 1)) {
            StopConstantForce(channel);
            wasPlayingBeforeAirborne[channel][1] = 1;
        }
        if (IsPlaying(channel, 2)) {
            StopDamperForce(channel);
            wasPlayingBeforeAirborne[channel][2] = 1;
        }
        if (IsPlaying(channel, 5)) {
            StopDirtRoadEffect(channel);
            wasPlayingBeforeAirborne[channel][5] = 1;
        }
        if (IsPlaying(channel, 6)) {
            StopBumpyRoadEffect(channel);
            wasPlayingBeforeAirborne[channel][6] = 1;
        }
        if (IsPlaying(channel, 7)) {
            StopSlipperyRoadEffect(channel);
            wasPlayingBeforeAirborne[channel][7] = 1;
        }
        if (IsPlaying(channel, 8)) {
            StopSurfaceEffect(channel);
            wasPlayingBeforeAirborne[channel][8] = 1;
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopCarAirborne(long channel) {
    this->StopForce(channel, 9);
}
