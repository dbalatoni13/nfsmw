#include "Speed/GameCube/Src/Logitech/Force.h"
#include "Speed/GameCube/Src/Logitech/LGDev.h"

#include "dolphin.h"

#include <string.h>

Periodic::Periodic() {}

long Periodic::DownloadForce(long channel, long forceNumber, unsigned long &handle, unsigned char type, unsigned long duration,
                             unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period,
                             unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel,
                             unsigned char fadeLevel) {
    long ret = 0;

    if (this->EffectID[channel][forceNumber] != -1) {
        this->Destroy(channel, forceNumber);
    }

    if (handle != -1) {
        LGForceParams params;

        memset(&params, 0, sizeof(params));
        params.type = type;
        params.duration = duration;
        params.startDelay = startDelay;
        params.periodic.magnitude = magnitude;
        params.periodic.direction = direction;
        params.periodic.period = period;
        params.periodic.phase = phase;
        params.periodic.offset = offset;
        params.periodic.attackTime = attackTime;
        params.periodic.fadeTime = fadeTime;
        params.periodic.attackLevel = attackLevel;
        params.periodic.fadeLevel = fadeLevel;

        ret = LGDownloadForceEffect(handle, &this->EffectID[channel][forceNumber], &params);
        if (ret < 0) {
            OSReport("ERROR: DownloadForce(periodic force) on channel %d returned %d\n", channel, ret);
            this->EffectID[channel][forceNumber] = -1;
        }
    } else {
        OSReport("ERROR: Trying to download a periodic force to channel %d but wheel has not been opened.\n", channel);
    }

    return ret;
}

long Periodic::UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay,
                           unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset,
                           unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel) {
    LGForceParams params;
    long ret;

    memset(&params, 0, sizeof(params));
    params.type = type;
    params.duration = duration;
    params.startDelay = startDelay;
    params.periodic.magnitude = magnitude;
    params.periodic.direction = direction;
    params.periodic.period = period;
    params.periodic.phase = phase;
    params.periodic.offset = offset;
    params.periodic.attackTime = attackTime;
    params.periodic.fadeTime = fadeTime;
    params.periodic.attackLevel = attackLevel;
    params.periodic.fadeLevel = fadeLevel;

    ret = LGUpdateForceEffect(this->EffectID[channel][forceNumber], &params);
    if (ret < 0) {
        OSReport("ERROR: UpdateForce(periodic force) on channel %d returned %d\n", channel, ret);
        this->EffectID[channel][forceNumber] = -1;
    }

    return ret;
}
