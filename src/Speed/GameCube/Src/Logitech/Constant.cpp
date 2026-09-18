#include "Speed/GameCube/Src/Logitech/Force.h"
#include "Speed/GameCube/Src/Logitech/LGDev.h"

#include "dolphin.h"

#include <string.h>

Constant::Constant() {}

long Constant::DownloadForce(long channel, long forceNumber, unsigned long &handle, unsigned long duration, unsigned long startDelay, short magnitude,
                             unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel,
                             unsigned char fadeLevel) {
    long ret = 0;

    if (this->EffectID[channel][forceNumber] != -1) {
        this->Destroy(channel, forceNumber);
    }

    if (handle != -1) {
        LGForceParams params;

        memset(&params, 0, sizeof(params));
        params.type = ret;
        params.duration = duration;
        params.startDelay = startDelay;
        params.constant.magnitude = magnitude;
        params.constant.direction = direction;
        params.constant.attackTime = attackTime;
        params.constant.attackLevel = attackLevel;
        params.constant.fadeTime = fadeTime;
        params.constant.fadeLevel = fadeLevel;

        ret = LGDownloadForceEffect(handle, &this->EffectID[channel][forceNumber], &params);
        if (ret < 0) {
            OSReport("ERROR: DownloadForce(constant force) on channel %d returned %d\n", channel, ret);
            this->EffectID[channel][forceNumber] = -1;
        }
    } else {
        OSReport("ERROR: Trying to download a constant force to channel %d but wheel has not been opened.\n", channel);
    }

    return ret;
}

long Constant::UpdateForce(long channel, long forceNumber, unsigned long duration, unsigned long startDelay, short magnitude,
                           unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel,
                           unsigned char fadeLevel) {
    LGForceParams params;
    long ret = 0;

    memset(&params, 0, sizeof(params));
    params.type = ret;
    params.duration = duration;
    params.startDelay = startDelay;
    params.constant.magnitude = magnitude;
    params.constant.direction = direction;
    params.constant.attackTime = attackTime;
    params.constant.attackLevel = attackLevel;
    params.constant.fadeTime = fadeTime;
    params.constant.fadeLevel = fadeLevel;

    ret = LGUpdateForceEffect(this->EffectID[channel][forceNumber], &params);
    if (ret < 0) {
        OSReport("ERROR: UpdateForce(constant force) on channel %d returned %d\n", channel, ret);
        this->EffectID[channel][forceNumber] = -1;
    }

    return ret;
}
