#include "LGWheels.hpp"

#include <string.h>

extern "C" {
int LGDownloadForceEffect(unsigned long handle, unsigned long *effectId, LGForceEffect *force);
int LGUpdateForceEffect(unsigned long effectId, LGForceEffect *force);
void OSReport(const char *fmt, ...);
}

static const char kDownloadConstantForceError[] = "ERROR: DownloadForce(constant force) on channel %d returned %d\n";
static const char kDownloadConstantForceInvalidWheel[] = "ERROR: Trying to download a constant force to channel %d but wheel has not been opened.\n";
static const char kUpdateConstantForceError[] = "ERROR: UpdateForce(constant force) on channel %d returned %d\n";

static inline unsigned long &ConstantGetEffectID(Force *self, int channel, int forceNumber) {
    return reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(self) + 0x80)[channel * 8 + forceNumber];
}

Constant::Constant() : Force() {}

int Constant::DownloadForce(int channel, int forceNumber, unsigned long & handle, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel) {
    LGForceEffect force;
    int ret;

    ret = 0;
    if (ConstantGetEffectID(this, channel, forceNumber) != static_cast<unsigned long>(-1)) {
        Destroy(channel, forceNumber);
    }

    if (handle != static_cast<unsigned long>(-1)) {
        memset(&force, 0, sizeof(force));
        force.type = 0;
        force.duration = duration;
        force.startDelay = startDelay;
        force.p.constant.magnitude = magnitude;
        force.p.constant.direction = direction;
        force.p.constant.envelope.attackTime = attackTime;
        force.p.constant.envelope.fadeTime = fadeTime;
        force.p.constant.envelope.attackLevel = attackLevel;
        force.p.constant.envelope.fadeLevel = fadeLevel;

        ret = LGDownloadForceEffect(handle, &ConstantGetEffectID(this, channel, forceNumber), &force);
        if (ret < 0) {
            OSReport(kDownloadConstantForceError, channel, ret);
            ConstantGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
        }
    } else {
        OSReport(kDownloadConstantForceInvalidWheel, channel);
    }

    return ret;
}

int Constant::UpdateForce(int channel, int forceNumber, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel) {
    LGForceEffect force;
    int ret;

    memset(&force, 0, sizeof(force));
    force.type = 0;
    force.duration = duration;
    force.startDelay = startDelay;
    force.p.constant.magnitude = magnitude;
    force.p.constant.direction = direction;
    force.p.constant.envelope.attackTime = attackTime;
    force.p.constant.envelope.fadeTime = fadeTime;
    force.p.constant.envelope.attackLevel = attackLevel;
    force.p.constant.envelope.fadeLevel = fadeLevel;

    ret = LGUpdateForceEffect(ConstantGetEffectID(this, channel, forceNumber), &force);
    if (ret < 0) {
        OSReport(kUpdateConstantForceError, channel, ret);
        ConstantGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
    }

    return ret;
}
