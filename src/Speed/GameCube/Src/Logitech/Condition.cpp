#include "Speed/GameCube/Src/Logitech/Force.h"
#include "Speed/GameCube/Src/Logitech/LGDev.h"

#include "dolphin.h"

#include <string.h>

Condition::Condition() {}

long Condition::DownloadForce(long channel, long forceNumber, unsigned long &handle, unsigned char type, unsigned long duration,
                              unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos,
                              short coeffNeg, short coeffPos) {
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
        params.condition[0].offset = offset;
        params.condition[0].deadband = deadband;
        params.condition[0].satNeg = satNeg;
        params.condition[0].satPos = satPos;
        params.condition[0].coeffNeg = coeffNeg;
        params.condition[0].coeffPos = coeffPos;
        params.condition[1] = params.condition[0];

        ret = LGDownloadForceEffect(handle, &this->EffectID[channel][forceNumber], &params);
        if (ret < 0) {
            OSReport("ERROR: DownloadForce(condition force) on channel %d returned %d\n", channel, ret);
            this->EffectID[channel][forceNumber] = -1;
        }
    } else {
        OSReport("ERROR: Trying to download a condition force to channel %d but wheel has not been opened.\n", channel);
    }

    return ret;
}

long Condition::UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset,
                            unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos) {
    LGForceParams params;
    long ret;

    memset(&params, 0, sizeof(params));
    params.type = type;
    params.duration = duration;
    params.startDelay = startDelay;
    params.condition[0].offset = offset;
    params.condition[0].deadband = deadband;
    params.condition[0].satNeg = satNeg;
    params.condition[0].satPos = satPos;
    params.condition[0].coeffNeg = coeffNeg;
    params.condition[0].coeffPos = coeffPos;
    params.condition[1] = params.condition[0];

    ret = LGUpdateForceEffect(this->EffectID[channel][forceNumber], &params);
    if (ret < 0) {
        OSReport("ERROR: UpdateForce(condition force) on channel %d returned %d\n", channel, ret);
        this->EffectID[channel][forceNumber] = -1;
    }

    return ret;
}
